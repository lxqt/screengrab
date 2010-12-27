/***************************************************************************
 *   Copyright (C) 2009 by Artem 'DOOMer' Galichkin                        *
 *   doomer3d@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include "src/core/core.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_X11

#include "src/common/netwm/netwm.h"
using namespace netwm;

#include <X11/Xlib.h>
#include <QX11Info>
#endif

Core* Core::corePtr = 0;

Core::Core()
{
    qRegisterMetaType<StateNotifyMessage>("StateNotifyMessage");

    conf = Config::instance();
    conf->loadSettings();

    pixelMap = new QPixmap;
    scrNum = 0;

    sleep(250);
    // delay on 250 msec
//     QMutex mutex;
//     mutex.lock();
//     QWaitCondition pause;
//     pause.wait(&mutex, 250);
}

Core::Core(const Core& ): QObject()
{

}

Core& Core::operator=(const Core& )
{

}

Core* Core::instance()
{
    if (!corePtr)
    {
	corePtr = new Core;
    }
    return corePtr;
}

Core::~Core()
{
    delete pixelMap;
    conf->killInstance();
}

void Core::sleep(quint8 msec)
{
    QMutex mutex;
    mutex.lock();
    QWaitCondition pause;
    pause.wait(&mutex, msec); // def 240
    mutex.unlock();
}


void Core::coreQuit()
{
    if (corePtr)
    {
	delete corePtr;
	corePtr = NULL;
    }

    qApp->quit();
}


// get screenshot
void Core::screenShot(bool first)
{
    if (first == true)
    {
        qDebug() << "first screen";
    }
    else
    {
        qDebug() << "NON first screen";
    }
    
    // grb pixmap of desktop
    switch(conf->getTypeScreen())
    {
        case 0:
        {
            *pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
            break;
        }
        case 1:
            {
#ifdef Q_WS_WIN
    getActiveWind_Win32();
#endif
#ifdef Q_WS_X11
    getActiveWind_X11();
#endif
                break;
            }
        case 2:
            {
		RegionSelect *selector;
		selector = new RegionSelect(conf);
		int resilt = selector->exec();

		if (resilt == QDialog::Accepted)
		{
		    *pixelMap = selector->getSelection();
		}
		delete selector;
		break;
            }
        default:
            *pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId()); break;
    }

    if (conf->getAutoSave() == true)
    {
	// small hack for display tray message on first screenshot (on starting
	// ScreenGrab in KDE, fluxbox and something wm)
	if (first == true)
	{
	    if (conf->getAutoSaveFirst() == true)
	    {
		QTimer::singleShot(600, this, SLOT(autoSave()));
	    }
	}
	else
	{
	    autoSave();
	}
    }
    else
    {
	if (first == false)
	{
	    StateNotifyMessage message(tr("New screen"), tr("New screen is getted!"));
	    Q_EMIT 	sendStateNotifyMessage(message);
	}
    }

    Q_EMIT newScreenShot(pixelMap);
}

#ifdef Q_WS_X11
void Core::getActiveWind_X11()
{
    netwm::init();
    Window *wnd = reinterpret_cast<ulong *>(netwm::property(QX11Info::appRootWindow(), NET_ACTIVE_WINDOW, XA_WINDOW));

    if(!wnd)
    {
        *pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
        exit(1);
    }

    // no dectortions option is select
    if (conf->getNoDecorX11() == true)
    {
        *pixelMap = QPixmap::grabWindow(*wnd);
        return;
    }

    unsigned int d;
    int status;
    int stat;

//    if (status != 0) {
      Window rt, *children, parent;

    // Find window manager frame
    while (true)
    {
        status = XQueryTree(QX11Info::display(), *wnd, &rt, &parent, &children, &d);
        if (status && (children != None))
        {
            XFree((char *) children);
        }

        if (!status || (parent == None) || (parent == rt))
        {
            break;
        }

        *wnd = parent;
    }

    XWindowAttributes attr; // window attributes
    stat = XGetWindowAttributes(QX11Info::display(), *wnd, &attr);

    if ((stat == False) || (attr.map_state != IsViewable))
    {
	CmdLine::print("Not window attributes.");
    }

    // get wnd size
    int rx = 0, ry = 0, rw = 0, rh = 0;
    rw = attr.width;
    rh = attr.height;
    rx = attr.x;
    ry = attr.y;

    *pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId(), rx, ry, rw, rh);

    XFree(wnd);
}
#endif

#ifdef Q_WS_WIN
void Core::getActiveWind_Win32()
{
    HWND findWindow = GetForegroundWindow();

    if (findWindow == NULL)
    {
        return;
    }

    if (findWindow == GetDesktopWindow())
    {
        return;
    }

    ShowWindow(findWindow, SW_SHOW);
    SetForegroundWindow(findWindow);

    HDC hdcScreen = GetDC(NULL);

    RECT rcWindow;
    GetWindowRect(findWindow, &rcWindow);

    if (IsZoomed(findWindow))
    {
        if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
        {
          rcWindow.right -= 8;
          rcWindow.left += 8;
          rcWindow.top += 8;
          rcWindow.bottom -= 8;
        }
        else
        {
          rcWindow.right += 4;
          rcWindow.left -= 4;
          rcWindow.top += 4;
          rcWindow.bottom -= 4;
        }
    }

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmCapture = CreateCompatibleBitmap(hdcScreen, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top);
    SelectObject(hdcMem, hbmCapture);

    BitBlt(hdcMem, 0, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, hdcScreen, rcWindow.left, rcWindow.top, SRCCOPY);

    ReleaseDC(findWindow, hdcMem);
    DeleteDC(hdcMem);

    *pixelMap = QPixmap::fromWinHBITMAP(hbmCapture);

    DeleteObject(hbmCapture);
}
#endif

// TODO - rebuild in Config class
QString Core::getSaveFilePath(QString format)
{
    QString initPath;
    if (conf->getDateTimeInFilename() == true)
    {
        #ifdef Q_WS_X11
            initPath = conf->getSaveDir()+conf->getSaveFileName() +"-"+getDateTimeFileName() +"."+format;
        #endif
        #ifdef Q_WS_WIN
            initPath = conf->getSaveDir()+conf->getSaveFileName()+"-"+getDateTimeFileName();
        #endif
    }
    else
    {
        if (scrNum != 0 && conf->getAutoSave() == true)
        {
        #ifdef Q_WS_X11
            initPath = conf->getSaveDir()+conf->getSaveFileName()+"-" +QString::number(scrNum) +"."+format;
        #endif
        #ifdef Q_WS_WIN
            initPath = conf->getSaveDir()+conf->getSaveFileName()+"-"+QString::number(scrNum);
        #endif
        }
        else
        {
        #ifdef Q_WS_X11
            initPath = conf->getSaveDir() + conf->getSaveFileName()+"."+format;
        #endif
        #ifdef Q_WS_WIN
            initPath = conf->getSaveDir()+conf->getSaveFileName();
        #endif
        }
    }
    return initPath;
}

QString Core::getDateTimeFileName()
{
    QString currentDateTime = QDateTime::currentDateTime().toString(conf->getDateTimeTpl());

    return currentDateTime;
}

// save screen
bool Core::writeScreen(QString& fileName, QString& format)
{
    // aitoncrement number screen in autosaving
    if (conf->getAutoSave() == true && conf->getDateTimeInFilename() == false)
    {
        scrNum++;
    }

    // adding extension  format
    if (!fileName.contains("."+format) )
    {
        fileName.append("."+format);
    }

    // writing file
    bool saved;
    if (fileName.isEmpty() == false)
    {        ;
        if (pixelMap->save(fileName,format.toAscii()) == true)
        {
            saved = true;
	    StateNotifyMessage message(tr("Saved"), tr("Saved to ") + fileName);
	    Q_EMIT 	sendStateNotifyMessage(message);
        }
        else
        {
            saved = false;
        }
    }
    else
    {
        saved = false;
    }

    return saved;
}

void Core::copyScreen()
{
    QApplication::clipboard()->setPixmap(*pixelMap, QClipboard::Clipboard);
    StateNotifyMessage message(tr("Copied"), tr("Screenshot is copied to clipboard"));
    Q_EMIT sendStateNotifyMessage(message);
}

void Core::autoSave()
{

	QString format = conf->getSaveFormat();
	QString fileName = getSaveFilePath(format);

	writeScreen(fileName, format);
}


QString Core::getVersionPrintable()
{
    QString str = "ScreenGrab: " + qApp->applicationVersion() + QString("\n");
    str += "Qt: " + QString(qVersion()) + QString("\n");
    return str;
}

QPixmap Core::getPixmap()
{
    return *pixelMap;
}
