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

#include <wayland-client.h>
#include "ScreenShot.h"
#include "ScreenCopy.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QTimer>
#include <QDebug>

#include <QtGui/private/qguiapplication_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>

QMap<wl_shm_format, QImage::Format> mFormats{
    { WL_SHM_FORMAT_XRGB8888, QImage::Format_ARGB32 },
    { WL_SHM_FORMAT_ARGB8888, QImage::Format_ARGB32 },
    { WL_SHM_FORMAT_XBGR8888, QImage::Format_RGBA8888 },
    { WL_SHM_FORMAT_ABGR8888, QImage::Format_RGBA8888 },
};

LXQt::Wayland::ScreenShot::ScreenShot(bool drawCursor, QScreen *screen, const QRect &rect, QObject *parent) :
    QObject (parent),
    scrnCopyMgr(nullptr)  {

    /** Get the QWaylandDisplay object. We can do everything else from here. */
    QtWaylandClient::QWaylandDisplay *qDisplay = nullptr;
    QtWaylandClient::QWaylandIntegration *waylandIntegration =
        static_cast<QtWaylandClient::QWaylandIntegration *>(QGuiApplicationPrivate::platformIntegration());
    if (waylandIntegration) {
        qDisplay = waylandIntegration->display();
    }
    if (qDisplay == nullptr)
    {
        qCritical() << "Failed to get Wayland display";
        QTimer::singleShot(0, this, [this]() {
            Q_EMIT screenShotReady(QPixmap());
        });
        return;
    }

    wl_shm *shm = nullptr;


    // NOTE: If there are persistent issues in detecting wlr_screencopy_manager_interface,
    // then use a QEventLoop and wait for the QtWaylandClient::QWaylandDisplay::globalAdded
    // to advertise the wlr_screencopy_manager_interface. The required code is commented out.

    // qDisplay->initialize();

    /*QEventLoop loop;

    QObject::connect(
        qDisplay, &QtWaylandClient::QWaylandDisplay::globalAdded, [ &loop ] (const QtWaylandClient::QWaylandDisplay::RegistryGlobal& global) {
            qDebug() << global.interface << "added";

            if ((global.interface == zwlr_screencopy_manager_v1_interface.name) && (scrnCopyMgr == nullptr))
            {
                zwlr_screencopy_manager_v1 *wlrScreenCopyMgr = (zwlr_screencopy_manager_v1 *)wl_registry_bind(global.registry, global.id, &zwlr_screencopy_manager_v1_interface, 3);

                if (wlrScreenCopyMgr)
                {
                    scrnCopyMgr = new LXQt::Wayland::ScreenCopyManager( wlrScreenCopyMgr );
                }

                loop.quit();
            }
            else if (global.interface == wl_shm_interface.name)
            {
                shm = (wl_shm *)wl_registry_bind(global.registry, global.id, &wl_shm_interface, global.version);
            }
        }
    );*/

    for (QtWaylandClient::QWaylandDisplay::RegistryGlobal global: qDisplay->globals())
    {
        if (global.interface == QString::fromUtf8(zwlr_screencopy_manager_v1_interface.name))
        {
            zwlr_screencopy_manager_v1 *wlrScreenCopyMgr = (zwlr_screencopy_manager_v1 *)wl_registry_bind(global.registry, global.id, &zwlr_screencopy_manager_v1_interface, 3);

            if (wlrScreenCopyMgr)
            {
                scrnCopyMgr = new LXQt::Wayland::ScreenCopyManager(wlrScreenCopyMgr);
                //qDebug() << "Ready";
            }
        }
        else if (global.interface == QString::fromUtf8(wl_shm_interface.name))
        {
            shm = (wl_shm *)wl_registry_bind(global.registry, global.id, &wl_shm_interface, global.version);
        }
    }

    /*if ((scrnCopyMgr == nullptr) || (shm == nullptr))
    {
        loop.exec();
    }*/

    if (scrnCopyMgr == nullptr || shm == nullptr)
    {
        QTimer::singleShot(0, this, [this]() {
            Q_EMIT screenShotReady(QPixmap());
        });
        return;
    }


    LXQt::Wayland::ScreenCopyFrame *frame =
        rect.isEmpty() ? scrnCopyMgr->captureOutput(drawCursor, screen)
                       : scrnCopyMgr->captureOutputRegion(drawCursor, screen, rect);

    if ( frame == nullptr )
    {
        QTimer::singleShot(0, this, [this]() {
            Q_EMIT screenShotReady(QPixmap());
        });
        return;
    }

    LXQt::Wayland::ScreenFrameBuffer *buffer = new LXQt::Wayland::ScreenFrameBuffer;

    QObject::connect(frame, &LXQt::Wayland::ScreenCopyFrame::bufferDone, this, [frame, buffer, shm]() {
        for (LXQt::Wayland::FrameBufferInfo info: frame->availableFormats())
        {
            if (mFormats.contains(info.format))
            {
                buffer->initializeBuffer(info, shm);

                if (buffer->buffer == nullptr)
                {
                    qWarning() << "Failed to create buffer with format" << buffer->info.format;
                    continue;
                }

                frame->attachBuffer(buffer);
                frame->copy();
            }
        }
    });

    QObject::connect(frame, &LXQt::Wayland::ScreenCopyFrame::ready, this, [this]
                     (LXQt::Wayland::ScreenFrameBuffer *buffer) {
        if (buffer == nullptr)
        {
            //qDebug() << "Screenshot failed";
            Q_EMIT screenShotReady(QPixmap());
            return;
        }
        QImage img((uchar *)buffer->data,
                   buffer->info.width,
                   buffer->info.height,
                   buffer->info.stride,
                   mFormats[buffer->info.format]);
        QPixmap px;
        px.convertFromImage(img);
        Q_EMIT screenShotReady(px);
    });

    frame->setup();
}

LXQt::Wayland::ScreenShot::~ScreenShot() {
    delete scrnCopyMgr;
    scrnCopyMgr = nullptr;
}
