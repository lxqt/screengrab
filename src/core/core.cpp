/***************************************************************************
 *   Copyright (C) 2009 - 2013 by Artem 'DOOMer' Galichkin                 *
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include <QMutex>
#include <QWaitCondition>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QChar>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QUuid>

#include <QDebug>

#include <KF5/KWindowSystem/KWindowSystem>
#include <xcb/xfixes.h>

#ifdef X11_XCB_FOUND
#include "x11utils.h"
#endif

#include "core/core.h"

#ifdef SG_DBUS_NOTIFY
#include "dbusnotifier.h"
#endif

#ifdef SG_EXT_UPLOADS
#include "modules/uploader/moduleuploader.h"
#endif

Core* Core::corePtr = 0;

Core::Core()
{
    qRegisterMetaType<StateNotifyMessage>("StateNotifyMessage");

    _conf = Config::instance();
    _conf->loadSettings();
    _lastSelectedArea = _conf->getLastSelection();

    _pixelMap = new QPixmap;
    _selector = 0;
    _firstScreen = true;

    _cmdLine.setApplicationDescription("ScreenGrab " + tr("is a crossplatform application for fast creating screenshots of your desktop."));
    _cmdLine.addHelpOption();
    _cmdLine.addVersionOption();

    QCommandLineOption optFullScreen(QStringList() << "f" << "fullscreen", tr("Take a fullscreen screenshot"));
    _cmdLine.addOption(optFullScreen);
    _screenTypeOpts.append(optFullScreen);

    QCommandLineOption optActiveWnd(QStringList() << "a" << "active", tr("Take a screenshot of the active window"));
    _cmdLine.addOption(optActiveWnd);
    _screenTypeOpts.append(optActiveWnd);

    QCommandLineOption optSelectedRect(QStringList() << "r" << "region", tr("Take a screenshot of a selection of the screen"));
    _cmdLine.addOption(optSelectedRect);
    _screenTypeOpts.append(optSelectedRect);

    QCommandLineOption optRunMinimized(QStringList() << "m" << "minimized", tr("Run the application with a hidden main window"));
    _cmdLine.addOption(optRunMinimized);

    sleep(250);

    _wnd = NULL;
}

Core::Core(const Core& ): QObject()
{
}

Core& Core::operator=(const Core &)
{
    return *this;
}

Core* Core::instance()
{
    if (!corePtr)
        corePtr = new Core;
    return corePtr;
}

Core::~Core()
{
    delete _pixelMap;
    _conf->killInstance();
}

void Core::initWindow(const QString& ipcMessage)
{
    if (!_wnd) {
        _wnd = new MainWindow;
        _wnd->setConfig(_conf);
        _wnd->updateModulesActions(_modules.generateModulesActions());
        _wnd->updateModulesenus(_modules.generateModulesMenus());

        screenShot(true); // first screenshot

        _wnd->resize(_conf->getRestoredWndSize());

        if (_wnd) {
            if (runAsMinimized())
            {
                if (_wnd->isTrayed())
                    _wnd->windowHideShow();
                else
                    _wnd->showMinimized();
            } else
                _wnd->show();
        }
    } else {
        _wnd->showWindow(ipcMessage);
        screenShot();
    }
}

void Core::sleep(int msec)
{
    QMutex mutex;
    mutex.lock();
    QWaitCondition pause;
    pause.wait(&mutex, msec); // def 240
    mutex.unlock();
}

void Core::coreQuit()
{
    _conf->setLastSelection(_lastSelectedArea);
    _conf->saveScreenshotSettings();

    _conf->setRestoredWndSize(_wnd->width(), _wnd->height());
    _conf->saveWndSize();

    if (_wnd) {
        _wnd->close();
    }

    if (corePtr)
    {
        delete corePtr;
        corePtr = NULL;
    }

    qApp->quit();
}

void Core::setScreen()
{
    _wnd->hideToShot();

    // new code experimental
    if (_conf->getDelay() == 0)
        QTimer::singleShot(200, this, SLOT(screenShot()));
    else
        QTimer::singleShot(1000 * _conf->getDelay(), this, SLOT(screenShot()));

}


// get screenshot
void Core::screenShot(bool first)
{
    sleep(400); // delay for hide "fade effect" bug in the KWin with compositing
    _firstScreen = first;

    // Update date last crenshot, if it is  a first screen
    if (_firstScreen)
        _conf->updateLastSaveDate();

    switch(_conf->getDefScreenshotType())
    {
    case Core::FullScreen:
    {
        const QList<QScreen *> screens = qApp->screens();
        const QDesktopWidget *desktop = QApplication::desktop();
        const int screenNum = desktop->screenNumber(QCursor::pos());
        *_pixelMap = screens[screenNum]->grabWindow(desktop->winId());
        grabCursor(0, 0);

        checkAutoSave(first);
        _wnd->updatePixmap(_pixelMap);
        break;
    }
    case Core::Window:
    {
        getActiveWindow();
        checkAutoSave(first);
        _wnd->updatePixmap(_pixelMap);
        break;
    }
    case Core::Area:
    {
        _selector = new RegionSelect(_conf);
        connect(_selector, &RegionSelect::processDone, this, &Core::regionGrabbed);
        break;
    }
    case Core::PreviousSelection:
    {
        _selector = new RegionSelect(_conf, _lastSelectedArea);
        connect(_selector, &RegionSelect::processDone, this, &Core::regionGrabbed);
        break;
    }
    default:
        *_pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
        break;
    }



    _wnd->updatePixmap(_pixelMap);
    _wnd->restoreFromShot();
}

void Core::checkAutoSave(bool first)
{
    if (_conf->getAutoSave())
    {
        // hack
        if (first)
        {
            if (_conf->getAutoSaveFirst())
                QTimer::singleShot(600, this, SLOT(autoSave()));
        }
        else
            autoSave();
    }
}

void Core::getActiveWindow()
{
    const QList<QScreen *> screens = qApp->screens();
    const QDesktopWidget *desktop = QApplication::desktop();
    const int screenNum = desktop->screenNumber(QCursor::pos());

    WId wnd = KWindowSystem::activeWindow();

    // this window screenshot will be invalid
    // if there's no active window or the active window is ours
    bool invalid(!wnd || !KWindowSystem::hasWId(wnd) || (_wnd && _wnd->winId() == wnd));
    if (!invalid)
    { // or if it isn't on the current desktop
        KWindowInfo info(wnd, NET::WMDesktop);
        invalid = info.valid() && !info.isOnDesktop(KWindowSystem::currentDesktop());
        if (!invalid)
        { // or if it is a desktop or panel/dock
            info = KWindowInfo(wnd, NET::WMWindowType);
            QFlags<NET::WindowTypeMask> flags;
            flags |= NET::DesktopMask;
            flags |= NET::DockMask;
            invalid = info.valid() && NET::typeMatchesMask(info.windowType(NET::AllTypesMask), flags);
        }
    }
    // if this is an invalid screenshot, take a fullscreen shot instead
    if (invalid)
    {
        *_pixelMap = screens[screenNum]->grabWindow(desktop->winId());
        return;
    }

    // no decorations option is selected
    if (_conf->getNoDecoration())
    {
        *_pixelMap = screens[screenNum]->grabWindow(wnd);
        return;
    }

    KWindowInfo info(wnd, NET::XAWMState | NET::WMFrameExtents);

    if (info.mappingState() != NET::Visible)
        qWarning() << "Window not visible";

    QRect geometry = info.frameGeometry();
    *_pixelMap = screens[screenNum]->grabWindow(QApplication::desktop()->winId(),
                                     geometry.x(),
                                     geometry.y(),
                                     geometry.width(),
                                                geometry.height());
    grabCursor(geometry.x(), geometry.y());
}

void Core::grabCursor(int offsetX, int offsetY)
{
#ifdef XCB_XFOXES_FOUND
    if (_conf->getIncludeCursor())
        X11Utils::compositePointer(offsetX, offsetY, _pixelMap);
#else
    Q_UNUSED(offsetx);
    Q_UNUSED(offsety);
#endif


}

void Core::sendSystemNotify(const StateNotifyMessage& /*notify*/)
{
    qDebug() << "Send system notification";
}

QString Core::getSaveFilePath(QString format)
{
    QString initPath;

    do
    {
        QDir dir(_conf->getSaveDir());
        const QString filePath = dir.filePath(_conf->getSaveFileName());
        if (_conf->getDateTimeInFilename())
            initPath = filePath + "-" + getDateTimeFileName() + "." + format;
        else
        {
            if (_conf->getScrNum() != 0)
                initPath = filePath + _conf->getScrNumStr() + "." + format;
            else
                initPath = filePath + "." + format;
        }
    } while (checkExsistFile(initPath));

    return initPath;
}

bool Core::checkExsistFile(QString path)
{
    bool exist = QFile::exists(path);

    if (exist)
        _conf->increaseScrNum();

    return exist;
}

QString Core::getDateTimeFileName()
{
    QString currentDateTime = QDateTime::currentDateTime().toString(_conf->getDateTimeTpl());

    if (currentDateTime == _conf->getLastSaveDate().toString(_conf->getDateTimeTpl()) && _conf->getScrNum() != 0)
        currentDateTime += "-" + _conf->getScrNumStr();
    else
        _conf->resetScrNum();

    return currentDateTime;
}

Config *Core::config()
{
    return _conf;
}

void Core::updatePixmap()
{
    if (QFile::exists(_tempFilename))
    {
        _pixelMap->load(_tempFilename, "png");
        _wnd->updatePixmap(_pixelMap);
    }
}

QString Core::getTempFilename(const QString& format)
{
    _tempFilename = QUuid::createUuid().toString();
    int size = _tempFilename.size() - 2;
    _tempFilename = _tempFilename.mid(1, size).left(8);
    _tempFilename = QDir::tempPath() + QDir::separator() + "screenshot-" + _tempFilename + "." + format;
    return _tempFilename;
}

void Core::killTempFile()
{
    if (QFile::exists(_tempFilename))
    {
        QFile::remove(_tempFilename);
        _tempFilename.clear();
    }
}

bool Core::writeScreen(QString& fileName, QString& format, bool tmpScreen)
{
    // adding extension format
    if (!fileName.contains("." + format))
        fileName.append("." + format);

    // saving temp file (for uploader module)
    if (tmpScreen)
    {
        if (!fileName.isEmpty())
            return _pixelMap->save(fileName, format.toLatin1(), _conf->getImageQuality());
        else
            return false;
    }

    // writing file
    bool saved = false;
    if (!fileName.isEmpty())
    {
        if (format == "jpg")
            saved = _pixelMap->save(fileName,format.toLatin1(), _conf->getImageQuality());
        else
            saved = _pixelMap->save(fileName,format.toLatin1(), -1);

        if (saved)
        {
            StateNotifyMessage message(tr("Saved"), tr("Saved to ") + fileName);

            message.message = message.message + copyFileNameToCliipboard(fileName);
            _conf->updateLastSaveDate();
            sendNotify(message);
        }
        else
            qWarning() << "Error saving file " << fileName;
    }

    return saved;
}

QString Core::copyFileNameToCliipboard(QString file)
{
    QString retString = "";
    switch (_conf->getAutoCopyFilenameOnSaving())
    {
    case Config::nameToClipboardFile:
    {
        file = file.section('/', -1);
        QApplication::clipboard()->setText(file);
        retString = QChar(QChar::LineSeparator) + tr("Name of saved file is copied to the clipboard");
        break;
    }
    case Config::nameToClipboardPath:
    {
        QApplication::clipboard()->setText(file);
        retString = QChar(QChar::LineSeparator) + tr("Path to saved file is copied to the clipboard");
        break;
    }
    default:
        break;
    }
    return retString;
}

void Core::sendNotify(const StateNotifyMessage &message)
{
#ifdef SG_DBUS_NOTIFY
    DBusNotifier *notifier = new DBusNotifier();
    notifier->displayNotify(message);
#else
    _wnd->showTrayMessage(message.header, message.message);
#endif
}

void Core::copyScreen()
{
    QApplication::clipboard()->setPixmap(*_pixelMap, QClipboard::Clipboard);
    StateNotifyMessage message(tr("Copied"), tr("Screenshot is copied to clipboard"));
    sendNotify(message);
}

void Core::openInExtViewer()
{
    if (_conf->getEnableExtView())
    {
        QString format = _conf->getSaveFormat();
        if (format.isEmpty())
            format = "png";

        QString tempFileName = getTempFilename(format);
        writeScreen(tempFileName, format, true);

        QString exec;
        exec = "xdg-open";
        QStringList args;
        args << tempFileName;

        QProcess *execProcess = new QProcess(this);

        void (QProcess:: *signal)(int, QProcess::ExitStatus) = &QProcess::finished;
        connect(execProcess, signal, this, &Core::closeExtViewer);
        execProcess->start(exec, args);
    }
}

void Core::closeExtViewer(int, QProcess::ExitStatus)
{
    sender()->deleteLater();
    killTempFile();
}

ModuleManager* Core::modules()
{
    return &_modules;
}

void Core::addCmdLineOption(const QCommandLineOption& option)
{
    _cmdLine.addOption(option);
}

bool Core::checkCmdLineOption(const QCommandLineOption& option)
{
    return _cmdLine.isSet(option);
}

bool Core::checkCmdLineOptions(const QStringList &options)
{
    for (int i = 0; i < options.count(); ++i)
        if (_cmdLine.isSet(options.at(i)))
            return true;
    return false;
}

void Core::processCmdLineOpts(const QStringList& arguments)
{
    _cmdLine.process(arguments);

    // Check commandline parameters and set screenshot type
    for (int i=0; i < _screenTypeOpts.count(); ++i)
        if (_cmdLine.isSet(_screenTypeOpts.at(i)))
            _conf->setDefScreenshotType(i);

#ifdef SG_EXT_UPLOADS
    /// FIXMA - In module interface need add the mthod for geting module cmdLine options
    const QString UPLOAD_CMD_PARAM = "upload";
    const QString UPLOAD_CMD_PARAM_SHORT = "u";
    QCommandLineOption u(QStringList() << UPLOAD_CMD_PARAM_SHORT << UPLOAD_CMD_PARAM);

    if (_cmdLine.isSet(u)) {
        ModuleUploader *uploader = static_cast<ModuleUploader*>(_modules.getModule(MOD_UPLOADER));
        connect(uploader, &ModuleUploader::uploadCompleteWithQuit, qApp, &QApplication::quit);
        uploader->init();
    } else
        initWindow();
#endif

}

bool Core::runAsMinimized()
{
    return (_cmdLine.isSet("minimized") || _cmdLine.isSet("m"));
}

void Core::autoSave()
{
    QString format = _conf->getSaveFormat();
    QString fileName = getSaveFilePath(format);
    writeScreen(fileName, format);
}

QString Core::getVersionPrintable()
{
    QString str = "ScreenGrab: " + qApp->applicationVersion() + QString("\n");
    str += "Qt: " + QString(qVersion()) + QString("\n");
    return str;
}

QPixmap* Core::getPixmap()
{
    return _pixelMap;
}

QByteArray Core::getScreenData()
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    _pixelMap->save(&buffer, _conf->getSaveFormat().toLatin1());
    return bytes;
}

void Core::regionGrabbed(bool grabbed)
{
    if (grabbed)
    {
        *_pixelMap = _selector->getSelection();

        int x = _selector->getSelectionStartPos().x();
        int y = _selector->getSelectionStartPos().y();
        int w = _pixelMap->rect().width();
        int h = _pixelMap->rect().height();
        _lastSelectedArea.setRect(x, y, w, h);

        checkAutoSave();
    }

    _wnd->updatePixmap(_pixelMap);
    _selector->deleteLater();
}
