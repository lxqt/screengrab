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

#pragma once

#include <QMap>
#include <QRect>
#include <QImage>
#include <QObject>
#include <QScreen>
#include <QString>
#include <wayland-client-protocol.h>

#include <wayland-client.h>
#include "wayland-wlr-screencopy-unstable-v1-client-protocol.h"


struct wl_buffer;
struct wl_output;
struct wl_shm;

namespace LXQt {
    namespace Wayland {
        class ScreenCopyManager;
        class ScreenCopyFrame;

        typedef struct _bufferInfo {
            enum wl_shm_format format;
            uint32_t width;
            uint32_t height;
            uint32_t stride;
        } FrameBufferInfo;

        typedef struct _time_stamp_t {
            uint32_t secs  = 0;
            uint32_t nsecs = 0;
        } BufferTimeStamp;

        typedef struct _buffer {
            struct wl_buffer *buffer = nullptr;
            void             *data   = nullptr;
            FrameBufferInfo  info;
            BufferTimeStamp  time;

            ~_buffer();
            bool initializeBuffer( FrameBufferInfo, wl_shm *shm );
        } ScreenFrameBuffer;
    }
}

class LXQt::Wayland::ScreenCopyManager : public QObject {
    Q_OBJECT

    public:
        ScreenCopyManager( struct ::zwlr_screencopy_manager_v1 *scrnMgr );
        ~ScreenCopyManager();

        /**
         * Capture an output.
         * The first argument allows the user to choose if the mouse pointer needs to be drawn
         */
        ScreenCopyFrame *captureOutput( bool, QScreen * );

        /**
         * Capture a region of an output.
         * The first argument allows the user to choose if the mouse pointer needs to be drawn
         */
        ScreenCopyFrame *captureOutputRegion( bool, QScreen *, QRect );

        struct zwlr_screencopy_manager_v1 *get();

    private:
        /** Create a shared memory buffer */
        struct wl_buffer *createShmBuffer( FrameBufferInfo, void ** );

        /** Raw zwlr_screencopy_manager_v1 pointer */
        struct zwlr_screencopy_manager_v1 *mObj;

        /** A map of screen names and buffers */
        QMap<QString, LXQt::Wayland::ScreenFrameBuffer *> mScreenBufferMap;
};

class LXQt::Wayland::ScreenCopyFrame : public QObject {
    Q_OBJECT

    public:
        enum Error {
            AlreadyUsed = 0,
            InvalidBuffer
        };

        ScreenCopyFrame( struct ::zwlr_screencopy_frame_v1 * );
        ~ScreenCopyFrame();

        /** Setup the listener. First connect the signals to your slots, and then call this */
        void setup();

        QList<FrameBufferInfo> availableFormats();

        /** Attach the screen buffer to this object */
        void attachBuffer( LXQt::Wayland::ScreenFrameBuffer *buf );

        /** Copy the whole frame */
        void copy();

        /** Wait for damage and then copy the damaged rect only */
        void copyWithDamage();

        struct zwlr_screencopy_frame_v1 *get();

    private:
        static void handleBuffer( void *, struct zwlr_screencopy_frame_v1 *, uint32_t, uint32_t, uint32_t, uint32_t );
        static void handleFlags( void *, struct zwlr_screencopy_frame_v1 *, uint32_t );
        static void handleReady( void *, struct zwlr_screencopy_frame_v1 *, uint32_t, uint32_t, uint32_t );
        static void handleFailed( void *, struct zwlr_screencopy_frame_v1 * );
        static void handleDamage( void *, struct zwlr_screencopy_frame_v1 *, uint32_t, uint32_t, uint32_t, uint32_t );
        static void handleLinuxDmabuf( void *, struct zwlr_screencopy_frame_v1 *, uint32_t, uint32_t, uint32_t );
        static void handleBufferDone( void *, struct zwlr_screencopy_frame_v1 * );

        static const struct zwlr_screencopy_frame_v1_listener mListener;

        zwlr_screencopy_frame_v1 *mObj;

        bool mIsSetup           = false;
        bool mBufferDonePending = false;

        /** We handle only the following formats */
        QList<wl_shm_format> mFormats {
            WL_SHM_FORMAT_XRGB8888,
            WL_SHM_FORMAT_ARGB8888,
            WL_SHM_FORMAT_XBGR8888,
            WL_SHM_FORMAT_ABGR8888,
        };

        QList<FrameBufferInfo> mReceivedBuffers;
        ScreenFrameBuffer *mBuffer = nullptr;
        QRectF mDamage;

        bool mYInvert  = false;
        bool mCopyDone = false;
        bool mFailed   = false;

    Q_SIGNALS:
        void ready( LXQt::Wayland::ScreenFrameBuffer * );
        void format( uint32_t );
        void failed();
        void damage( QRect );
        void linuxDmabuf();
        void bufferDone();
};
