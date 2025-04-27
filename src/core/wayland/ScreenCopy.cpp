/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Marcus Britanicus (https://gitlab.com/marcusbritanicus)
 * Copyright (c) 2021 Abrar (https://gitlab.com/s96Abrar)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

/**
 * STEPS to obtain the screenshot
 * 1. SCM requests to capture the output.
 * 2. We get SCF object.
 * 3. SCF emits a series of signals - one for each buffer format.
 * 4. Once all formats are received, bufferDone is emitted.
 * 5. Now, we can try to write the image.
 *    a. Search if we have a suitable format.
 *    b. Create the buffer for that format with suitable size, and stride
 *    c. If buffer was created, then perform the copy() or copy_with_damage()
 *    d. If we were successful, emit read(...) with the buffer object.
 *       Otherwise, emit ready( nullptr ) along with failed().
 * NOTE: If you called copy_with_damage(), i.e., copyWithDamage(), a series
 * of damage(...) signals will be emitted to inform the user about the regions
 * that were damaged before (and until) the copy was started. The net damage
 * is the union of all the previous damage rects.
 **/

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <png.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

#include <QObject>
#include <QDebug>
#include <QImage>
#include <QGuiApplication>

#include <qpa/qplatformnativeinterface.h>

#include "ScreenCopy.h"


wl_output * getWlOutputFromQScreen( QScreen *screen ) {
    if ( !screen ) {
        qWarning( "Invalid QScreen pointer" );
        return nullptr;
    }

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if ( !native ) {
        qWarning( "Not a native platform application" );
        return nullptr;
    }

    // Get the wl_output associated with this screen
    void *output = native->nativeResourceForScreen( "output", screen );

    if ( !output ) {
        qWarning( "Failed to get wl_output for QScreen" );
        return nullptr;
    }

    return static_cast<wl_output *>( output );
}


LXQt::Wayland::_buffer::~_buffer() {
    if ( buffer ) {
        wl_buffer_destroy( buffer );
    }

    if ( ( data != MAP_FAILED ) && data ) {
        munmap( data, info.stride * info.height );
    }
}


bool LXQt::Wayland::ScreenFrameBuffer::initializeBuffer( LXQt::Wayland::FrameBufferInfo bufInfo, wl_shm *shm ) {
    // First, check if the new buffer info is different from the existing one
    bool sameInfo = (
        info.format == bufInfo.format &&
        info.width == bufInfo.width &&
        info.height == bufInfo.height &&
        info.stride == bufInfo.stride
    );

    // If buffer exists and info is the same, return true
    if ( sameInfo && ( buffer != nullptr ) ) {
        return true;
    }

    // Clean up existing resources if parameters changed
    if ( buffer ) {
        wl_buffer_destroy( buffer );
        buffer = nullptr;
    }

    if ( ( data != MAP_FAILED ) && data ) {
        munmap( data, info.stride * info.height );
        data = nullptr;
    }

    // Update info with new buffer parameters
    info = bufInfo;

    //qDebug() << "Creating fresh buffer";

    int size = info.stride * info.height;

    char shm_name[] = "/tmp/wayqt-screencopy-shared-XXXXXX";
    int  fd         = mkstemp( shm_name );

    // Handle potential EINTR during ftruncate
    int ret;
    do {
        ret = ftruncate( fd, size );
    } while ( ret == -1 && errno == EINTR );

    if ( ret < 0 ) {
        qCritical() << "Failed to create temporary file";

        /** Failed to create backing file */
        return false;
    }

    // Unlink immediately after creation for security
    unlink( shm_name );

    data = mmap( NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

    if ( data == MAP_FAILED ) {
        qCritical() << "Creating mmap failed:" << strerror( errno );
        close( fd );

        return false;
    }

    if ( shm == nullptr ) {
        qCritical() << "Unable to allocate shared memory";

        /** Release the mapped memory and close the fd */
        munmap( data, size );
        close( fd );

        return false;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool( shm, fd, size );

    if ( pool == nullptr ) {
        qCritical() << "Failed to create SHM pool";

        /** Release the mapped memory and close the fd */
        munmap( data, size );
        close( fd );

        return false;
    }

    /** Create the buffer */
    buffer = wl_shm_pool_create_buffer( pool, 0, info.width, info.height, info.stride, info.format );

    /** Destroy the pool */
    wl_shm_pool_destroy( pool );
    close( fd );

    if ( buffer == nullptr ) {
        qCritical() << "Failed to create Wayland buffer";

        /** Release the mapped memory */
        munmap( data, size );

        return false;
    }

    /** Everything went on smoothly!! */
    return true;
}


LXQt::Wayland::ScreenCopyManager::ScreenCopyManager( struct zwlr_screencopy_manager_v1 *mgr ) {
    mObj = mgr;
}


LXQt::Wayland::ScreenCopyManager::~ScreenCopyManager() {
    zwlr_screencopy_manager_v1_destroy( mObj );
}


LXQt::Wayland::ScreenCopyFrame *LXQt::Wayland::ScreenCopyManager::captureOutput( bool drawCursor, QScreen *screen ) {
    wl_output *output = getWlOutputFromQScreen( screen );

    if ( !output ) {
        return nullptr;
    }

    struct zwlr_screencopy_frame_v1 *frame = zwlr_screencopy_manager_v1_capture_output( mObj, ( drawCursor ? 1 : 0 ), output );

    return new ScreenCopyFrame( frame );
}


LXQt::Wayland::ScreenCopyFrame *LXQt::Wayland::ScreenCopyManager::captureOutputRegion( bool drawCursor, QScreen *screen, QRect rect ) {
    wl_output *output = getWlOutputFromQScreen( screen );

    if ( !output ) {
        return nullptr;
    }

    struct zwlr_screencopy_frame_v1 *frame = zwlr_screencopy_manager_v1_capture_output_region(
        mObj, ( drawCursor ? 1 : 0 ), output, rect.x(), rect.y(), rect.width(), rect.height()
    );

    return new ScreenCopyFrame( frame );
}


zwlr_screencopy_manager_v1 *LXQt::Wayland::ScreenCopyManager::get() {
    return mObj;
}


LXQt::Wayland::ScreenCopyFrame::ScreenCopyFrame( zwlr_screencopy_frame_v1 *frame ) {
    mObj = frame;

    if ( wl_proxy_get_listener( (wl_proxy *)mObj ) != &mListener ) {
        zwlr_screencopy_frame_v1_add_listener( mObj, &mListener, this );
    }
}


LXQt::Wayland::ScreenCopyFrame::~ScreenCopyFrame() {
    zwlr_screencopy_frame_v1_destroy( mObj );
}


void LXQt::Wayland::ScreenCopyFrame::setup() {
    if ( mIsSetup == false ) {
        mIsSetup = true;

        if ( mBufferDonePending ) {
            mBufferDonePending = false;
            emit bufferDone();
        }
    }
}


QList<LXQt::Wayland::FrameBufferInfo> LXQt::Wayland::ScreenCopyFrame::availableFormats() {
    return mReceivedBuffers;
}


void LXQt::Wayland::ScreenCopyFrame::attachBuffer( LXQt::Wayland::ScreenFrameBuffer *buf ) {
    mBuffer = buf;
}


void LXQt::Wayland::ScreenCopyFrame::copy() {
    if ( mBuffer == nullptr ) {
        qWarning() << "No buffer attached. Call attachBuffer(...) before calling this function.";
        return;
    }

    if ( mBuffer->buffer == nullptr ) {
        qWarning() << "Failed to create buffer with format" << mBuffer->info.format;
        return;
    }

    zwlr_screencopy_frame_v1_copy( mObj, mBuffer->buffer );
}


void LXQt::Wayland::ScreenCopyFrame::copyWithDamage() {
    if ( mBuffer->buffer == nullptr ) {
        qWarning() << "Failed to create buffer with format" << mBuffer->info.format;
        return;
    }

    zwlr_screencopy_frame_v1_copy_with_damage( mObj, mBuffer->buffer );
}


zwlr_screencopy_frame_v1 *LXQt::Wayland::ScreenCopyFrame::get() {
    return mObj;
}


void LXQt::Wayland::ScreenCopyFrame::handleBuffer( void *data, struct zwlr_screencopy_frame_v1 *, uint32_t fmt, uint32_t w, uint32_t h, uint32_t stride ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );
    FrameBufferInfo info       = { (wl_shm_format)fmt, w, h, stride };

    scrnFrame->mReceivedBuffers << info;
}


void LXQt::Wayland::ScreenCopyFrame::handleFlags( void *data, struct zwlr_screencopy_frame_v1 *, uint32_t flags ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );

    scrnFrame->mYInvert = flags & ZWLR_SCREENCOPY_FRAME_V1_FLAGS_Y_INVERT;
}


void LXQt::Wayland::ScreenCopyFrame::handleReady( void *data, struct zwlr_screencopy_frame_v1 *, uint32_t secs_hi, uint32_t secs_lo, uint32_t nsecs ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );

    scrnFrame->mBuffer->time.secs  = ( ( 1ll * secs_hi ) << 32ll ) | secs_lo;
    scrnFrame->mBuffer->time.nsecs = nsecs;

    emit scrnFrame->ready( scrnFrame->mBuffer );
}


void LXQt::Wayland::ScreenCopyFrame::handleFailed( void *data, struct zwlr_screencopy_frame_v1 * ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );
    emit scrnFrame->failed();

    zwlr_screencopy_frame_v1_destroy( scrnFrame->mObj );
    scrnFrame->mObj = nullptr;

    emit scrnFrame->ready( nullptr );
}


void LXQt::Wayland::ScreenCopyFrame::handleDamage( void *data, struct zwlr_screencopy_frame_v1 *, uint32_t x, uint32_t y, uint32_t w, uint32_t h ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );

    emit scrnFrame->damage( QRect( x, y, w, h ) );
}


void LXQt::Wayland::ScreenCopyFrame::handleLinuxDmabuf( void *data, struct zwlr_screencopy_frame_v1 *, uint32_t format, uint32_t width, uint32_t height ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );

    // Log that Linux DmaBuf is available
    //qDebug() << "Linux DmaBuf frame available:" << "Format:" << format << "Width:" << width << "Height:" << height;

    // Emit a signal to notify about Linux DmaBuf availability
    emit scrnFrame->linuxDmabuf();

    // Optional: Add the DmaBuf format to received buffers
    FrameBufferInfo dmabufInfo = {
        static_cast<wl_shm_format>( format ),
        width,
        height,
        width * 4  // Assuming 4 bytes per pixel, adjust as needed
    };
    scrnFrame->mReceivedBuffers << dmabufInfo;
}


void LXQt::Wayland::ScreenCopyFrame::handleBufferDone( void *data, struct zwlr_screencopy_frame_v1 * ) {
    ScreenCopyFrame *scrnFrame = reinterpret_cast<ScreenCopyFrame *>( data );

    if ( scrnFrame->mIsSetup ) {
        emit scrnFrame->bufferDone();
    }

    else {
        scrnFrame->mBufferDonePending = true;
    }
}


const zwlr_screencopy_frame_v1_listener LXQt::Wayland::ScreenCopyFrame::mListener = {
    handleBuffer,
    handleFlags,
    handleReady,
    handleFailed,
    handleDamage,
    handleLinuxDmabuf,
    handleBufferDone,
};
