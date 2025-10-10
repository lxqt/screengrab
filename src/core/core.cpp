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
#include <QScreen>
#include <QChar>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QMimeDatabase>

#include <QDebug>

#include <XdgMimeApps>
#include <qt6xdg/XdgDesktopFile>

#include <KWindowSystem>
#include <KX11Extras>
#include <KWindowInfo>
#include <xcb/xfixes.h>

#ifdef X11_XCB_FOUND
#include "x11utils.h"
#endif

#include "core/core.h"

#ifdef SG_DBUS_NOTIFY
#include "dbusnotifier.h"
#endif

#include "wayland/ScreenShot.h"

Core* Core::corePtr = nullptr;

Core::Core()
{
    qRegisterMetaType<StateNotifyMessage>("StateNotifyMessage");

    _conf = Config::instance();
    _conf->loadSettings();
    _lastSelectedArea = _conf->getLastSelection();

    _pixelMap = new QPixmap;
    _selector = nullptr;
    _firstScreen = true;

    _cmdLine.setApplicationDescription(QStringLiteral("ScreenGrab ") + tr("is a crossplatform application for fast creating screenshots of your desktop."));
    _cmdLine.addHelpOption();
    _cmdLine.addVersionOption();

    QCommandLineOption optFullScreen(QStringList() << QStringLiteral("f") << QStringLiteral("fullscreen"), tr("Take a fullscreen screenshot"));
    _cmdLine.addOption(optFullScreen);
    _screenTypeOpts.append(optFullScreen);

    QCommandLineOption optActiveWnd(QStringList() << QStringLiteral("a") << QStringLiteral("active"), tr("Take a screenshot of the active window"));
    _cmdLine.addOption(optActiveWnd);
    _screenTypeOpts.append(optActiveWnd);

    QCommandLineOption optSelectedRect(QStringList() << QStringLiteral("r") << QStringLiteral("region"), tr("Take a screenshot of a selection of the screen"));
    _cmdLine.addOption(optSelectedRect);
    _screenTypeOpts.append(optSelectedRect);

    QCommandLineOption optRunMinimized(QStringList() << QStringLiteral("m") << QStringLiteral("minimized"), tr("Run the application with a hidden main window"));
    _cmdLine.addOption(optRunMinimized);

    QCommandLineOption optShowNoWin(QStringList() << QStringLiteral("n") << QStringLiteral("no-win"), tr("Save screenshot and exit, without showing window"));
    _cmdLine.addOption(optShowNoWin);

    if (QGuiApplication::platformName() != QStringLiteral("wayland"))
        sleep(250);

    _wnd = nullptr;
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
    killTempFile();
    delete _selector;
    delete _pixelMap;
    _conf->killInstance();
}

void Core::initWindow(const QString& ipcMessage)
{
    if (!_wnd)
    {
        _wnd = new MainWindow;
        _wnd->setConfig(_conf);
        _wnd->updateModulesActions(_modules.generateModulesActions());
        _wnd->updateModulesMenus(_modules.generateModulesMenus());

        screenShot(true); // first screenshot
    }
    else
    {
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
    if (_wnd && !_wnd->findChildren<QDialog*>().isEmpty())
        return; // WARNING: quitting with a modal dialog causes crash

    _conf->setLastSelection(_lastSelectedArea);
    _conf->saveScreenshotSettings();

    if (_wnd) {
        _conf->setRestoredWndSize(_wnd->width(), _wnd->height());
        _conf->saveWndSize();
        if (_wnd->isVisible())
            _wnd->close();
    }

    delete corePtr;
    corePtr = nullptr;

    qApp->quit();
}

void Core::newScreenshot()
{
    _wnd->hideToShot();

    if (_conf->getDelay() == 0)
        QTimer::singleShot(200, this, SLOT(screenShot()));
    else
    {
        if (QGuiApplication::platformName() == QStringLiteral("wayland")
            && (_conf->getDefScreenshotType() == Core::Area
                || _conf->getDefScreenshotType() == Core::PreviousSelection))
        {
            // Since a Wayland area screenshot is taken on a transparent overlay,
            // the delay should be applied after selecting the area.
            screenShot(false, true);
            return;
        }
        QTimer::singleShot(1000 * _conf->getDelay(), this, SLOT(screenShot()));
    }

}

void Core::getFullScreenPixmap(QScreen* screen)
{
    const auto siblings = screen->virtualSiblings();
    if (siblings.size() == 1)
        *_pixelMap = screen->grabWindow(0); // 0 for the entire screen
    else
    { // consider all siblings
        QPixmap pix = QPixmap(screen->virtualSize());
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        for (const auto& sc : siblings)
            painter.drawPixmap(sc->geometry().topLeft(), sc->grabWindow(0));
        *_pixelMap = pix;
    }
}

// get screenshot
void Core::screenShot(bool first, bool delayed)
{
    // WARNING: With a modal dialog, the mouse and keyboard can be blocked
    // when an area screenshot is taken (e.g., by a global shortcut).
    if (!_wnd->findChildren<QDialog*>().isEmpty()
        && (_conf->getDefScreenshotType() == Core::Area
            || _conf->getDefScreenshotType() == Core::PreviousSelection))
    {
        return;
    }

    killTempFile(); // remove the old temp file if any

    if (QGuiApplication::platformName() != QStringLiteral("wayland"))
        sleep(400); // delay for hide "fade effect" bug in the KWin with compositing
    _firstScreen = first;

    // Update the last saving date, if this is the first screenshot
    if (_firstScreen)
        _conf->updateLastSaveDate();

    if (QGuiApplication::platformName() == QStringLiteral("wayland"))
    {
        waylandScreenShot(delayed);
        return;
    }

    switch(_conf->getDefScreenshotType())
    {
    case Core::FullScreen:
    {
        auto screen = QGuiApplication::screenAt(QCursor::pos());
        if (screen == nullptr)
            screen = QGuiApplication::screens().at(0);
        getFullScreenPixmap(screen);
        grabCursor(0, 0);

        checkAutoSave();
        _wnd->updatePixmap(_pixelMap);
        showScreenshot();
        break;
    }
    case Core::Window:
    {
        getActiveWindow();
        checkAutoSave();
        _wnd->updatePixmap(_pixelMap);
        showScreenshot();
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
        getFullScreenPixmap(QGuiApplication::primaryScreen());
        _wnd->updatePixmap(_pixelMap);
        showScreenshot();
        break;
    }
}

// Should be called only on Wayland.
void Core::waylandScreenShot(bool delayed)
{
    if (_selector != nullptr)
        return; // an area screenshot is in progress

    switch(_conf->getDefScreenshotType())
    {
    case Core::Area:
        _selector = new RegionSelect(_conf, _wnd->selectedScreen());
        [[fallthrough]];
    case Core::PreviousSelection:
    {
        if (_selector == nullptr)
            _selector = new RegionSelect(_conf, _lastSelectedArea, _wnd->selectedScreen());
        connect(_selector, &RegionSelect::processDone, this, [this, delayed](bool grabbed) {
            if (!grabbed)
            {
                _wnd->restoreFromShot();
                _selector->deleteLater();
                _selector = nullptr;
                return;
            }
            _selector->hide();
            if (delayed)
                QTimer::singleShot(1000 * _conf->getDelay(), this, SLOT(takeWaylandAreaScreenshot()));
            else
                takeWaylandAreaScreenshot(false);
        });
        break;
    }
    default: // for now, we can take only a fullscreen shot
    {
        auto ws = new LXQt::Wayland::ScreenShot(_conf->getIncludeCursor(),
                                                _wnd->selectedScreen(),
                                                QRect(),
                                                this);
        connect(ws, &LXQt::Wayland::ScreenShot::screenShotReady, this,
                [this, ws] (const QPixmap& pixmap) {
            if (!pixmap.isNull())
            {
                *_pixelMap = pixmap;
                checkAutoSave();
                _wnd->updatePixmap(_pixelMap);
            }
            showScreenshot();
            ws->deleteLater();
        });
        break;
    }
    }
}

void Core::takeWaylandAreaScreenshot(bool checkCursor)
{
    if (_selector == nullptr)
        return;
    auto ws = new LXQt::Wayland::ScreenShot(checkCursor ? _conf->getIncludeCursor() : false,
                                            _wnd->selectedScreen(),
                                            _selector->getSelectionRect(),
                                            this);
    connect(ws, &LXQt::Wayland::ScreenShot::screenShotReady, this,
            [this, ws] (const QPixmap& pixmap) {
        if (!pixmap.isNull())
        {
            *_pixelMap = pixmap;
            checkAutoSave();
            _wnd->updatePixmap(_pixelMap);
        }
        showScreenshot();
        _lastSelectedArea = _selector->getSelectionRect();
        _selector->deleteLater();
        _selector = nullptr;
        ws->deleteLater();
    });
}

void Core::showScreenshot()
{
    if (noWin())
    { // quit if no window should be shown
        QTimer::singleShot(0, this, &Core::coreQuit);
        return;
    }
    _wnd->resize(_conf->getRestoredWndSize()); // before showing, to center the window on X11
    _wnd->restoreFromShot();
    if (runAsMinimized())
    {
        if (_wnd->isTrayed())
            _wnd->windowHideShow();
        else
            _wnd->showMinimized();
    }
}

void Core::checkAutoSave()
{
    if (noWin())
    { // always auto-save with no window
        QTimer::singleShot(0, this, SLOT(autoSave()));
        return;
    }
    if (_conf->getAutoSave())
    {
        if (_firstScreen)
        {
            if (_conf->getAutoSaveFirst())
                QTimer::singleShot(600, this, SLOT(autoSave()));
        }
        else
            autoSave();
    }
}

void Core::getActiveWindow() // called only with window screenshots
{
    auto screen = QGuiApplication::screenAt(QCursor::pos());
    if (screen == nullptr)
        screen = QGuiApplication::screens().at(0);

    WId wnd = KX11Extras::activeWindow();

    // this window screenshot will be invalid
    // if there's no active window or the active window is ours
    bool invalid(!wnd || !KX11Extras::hasWId(wnd) || (_wnd && _wnd->winId() == wnd));
    if (!invalid)
    { // or if it isn't on the current desktop
        KWindowInfo info(wnd, NET::WMDesktop);
        invalid = info.valid() && !info.isOnDesktop(KX11Extras::currentDesktop());
        if (!invalid)
        { // or if it is a desktop or panel/dock
            info = KWindowInfo(wnd, NET::WMWindowType);
            QFlags<NET::WindowTypeMask> flags;
            flags |= NET::DesktopMask;
            flags |= NET::DockMask;
            invalid = info.valid() && NET::typeMatchesMask(info.windowType(NET::AllTypesMask), flags);
        }
    }

    // also invalid if the window is shaded/invisible
    KWindowInfo info(wnd, NET::XAWMState | NET::WMFrameExtents);
    if (!invalid && info.mappingState() != NET::Visible)
        invalid = true;

    // if this is an invalid screenshot, take a fullscreen shot instead
    if (invalid)
    {
        qWarning() << "Could not take a window screenshot.";
        *_pixelMap = screen->grabWindow(0);
        return;
    }

    // no decorations option is selected
    if (_conf->getNoDecoration())
    {
        *_pixelMap = screen->grabWindow(wnd);
        return;
    }

    QRect geometry = info.frameGeometry();

    // WARNING: Until now, "KWindowInfo::frameGeometry" does not consider the screens's
    // device pixel ratio. So, the frame geometry should be transformed.
    qreal pixelRatio = screen->devicePixelRatio();
    geometry.setTopLeft(QPointF(geometry.topLeft() / pixelRatio).toPoint());
    geometry.setBottomRight(QPointF(geometry.bottomRight() / pixelRatio).toPoint());

    // The offscreen part of the window will appear as a black area in the screenshot.
    // Until a better method is found, the offscreen area is ignored here.
    QRect r = screen->virtualGeometry().intersected(geometry);

    // the window positon should be calculated relative to the screen
    *_pixelMap = screen->grabWindow(0,
                                    r.x() - screen->geometry().x(),
                                    r.y() - screen->geometry().y(),
                                    r.width(),
                                    r.height());

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

QString Core::getSaveFilePath(const QString &format)
{
    QString initPath;

    QDir dir(_conf->getSaveDir());
    const QString fileName = _conf->getSaveFileName();
    do
    {
        if (_conf->getDateTimeInFilename())
        {
            initPath = fileName.isEmpty()
                       ? dir.filePath(getDateTimeFileName() + QStringLiteral(".") + format)
                       : dir.filePath(fileName) + QStringLiteral("-") + getDateTimeFileName() + QStringLiteral(".") + format;
        }
        else
        {
            if (_conf->getScrNum() != 0)
            {
                initPath = fileName.isEmpty()
                           ? dir.filePath(QStringLiteral("screengrab")) + _conf->getScrNumStr() + QStringLiteral(".") + format
                           : dir.filePath(fileName) + _conf->getScrNumStr() + QStringLiteral(".") + format;
            }
            else
            {
                initPath = fileName.isEmpty()
                           ? dir.filePath(QStringLiteral("screengrab")) + QStringLiteral(".") + format
                           : dir.filePath(fileName) + QStringLiteral(".") + format;
            }
        }
    } while (checkExsistFile(initPath));

    return initPath;
}

bool Core::checkExsistFile(const QString &path)
{
    bool exist = QFile::exists(path);

    if (exist)
        _conf->increaseScrNum();

    return exist;
}

QString Core::getDateTimeFileName()
{
    QString dateFormat = _conf->getDateTimeTpl();
    if (dateFormat.isEmpty())
        dateFormat = QStringLiteral("yyyy-MM-dd-hh:mm:ss");
    QString currentDateTime = QDateTime::currentDateTime().toString(dateFormat);

    if (currentDateTime == _conf->getLastSaveDate().toString(dateFormat) && _conf->getScrNum() != 0)
        currentDateTime += QStringLiteral("-") + _conf->getScrNumStr();
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
    if (_tempFilename.isEmpty())
    {
        _tempFilename = QUuid::createUuid().toString();
        int size = _tempFilename.size() - 2;
        _tempFilename = _tempFilename.mid(1, size).left(8);
        _tempFilename = QDir::tempPath() + QDir::separator()
                        + QStringLiteral("screenshot-") + _tempFilename
                        + QStringLiteral(".") + format;
    }
    return _tempFilename;
}

void Core::killTempFile()
{
    if (QFile::exists(_tempFilename))
        QFile::remove(_tempFilename);
    _tempFilename.clear();
}

bool Core::writeScreen(QString& fileName, QString& format, bool tmpScreen)
{
    // adding extension format
    if (!fileName.contains(QStringLiteral(".") + format))
        fileName.append(QStringLiteral(".") + format);

    // saving temp file
    if (tmpScreen)
    {
        if (!fileName.isEmpty())
            return _pixelMap->save(fileName, format.toLatin1().constData(), _conf->getImageQuality());
        else
            return false;
    }

    // writing file
    bool saved = false;
    if (!fileName.isEmpty())
    {
        if (format == QLatin1String("jpg"))
            saved = _pixelMap->save(fileName,format.toLatin1().constData(), _conf->getImageQuality());
        else
            saved = _pixelMap->save(fileName,format.toLatin1().constData(), -1);

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
    QString retString = QLatin1String("");
    switch (_conf->getAutoCopyFilenameOnSaving())
    {
    case Config::nameToClipboardFile:
    {
        file = file.section(QLatin1Char('/'), -1);
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
    if (_conf->hasNotification())
    {
        DBusNotifier *notifier = new DBusNotifier();
        notifier->displayNotify(message);
    }
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
            format = QLatin1String("png");

        QString tempFileName = getTempFilename(format);
        writeScreen(tempFileName, format, true);

        QMimeDatabase db;
        XdgMimeApps mimeAppsDb;
        QMimeType mt = db.mimeTypeForFile(tempFileName);
        auto app = mimeAppsDb.defaultApp(mt.name());
        if (app != nullptr)
        {
            app->startDetached(QStringList() << tempFileName);
            delete app;
        }
    }
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
    {
        if (_cmdLine.isSet(_screenTypeOpts.at(i)))
            _conf->setDefScreenshotType(i);
    }
}

bool Core::runAsMinimized()
{
    return (_cmdLine.isSet(QStringLiteral("minimized")) || _cmdLine.isSet(QStringLiteral("m")));
}

bool Core::noWin()
{
    return (_firstScreen
            && (_cmdLine.isSet(QStringLiteral("no-win"))
                || _cmdLine.isSet(QStringLiteral("n"))));
}

void Core::autoSave()
{
    QString format = _conf->getSaveFormat();
    QString fileName = getSaveFilePath(format);
    writeScreen(fileName, format);
}

QString Core::getVersionPrintable()
{
    QString str = QStringLiteral("ScreenGrab: ") + qApp->applicationVersion() + QStringLiteral("\n");
    str += QStringLiteral("Qt: ") + QString::fromLatin1(qVersion()) + QStringLiteral("\n");
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
    _pixelMap->save(&buffer, _conf->getSaveFormat().toLatin1().constData());
    return bytes;
}

void Core::regionGrabbed(bool grabbed)
{
    if (grabbed)
    {
        *_pixelMap = _selector->getSelection();

        int x = _selector->getSelectionStartPos().x();
        int y = _selector->getSelectionStartPos().y();
        int w = _pixelMap->rect().width() / _pixelMap->devicePixelRatio();
        int h = _pixelMap->rect().height() / _pixelMap->devicePixelRatio();
        _lastSelectedArea.setRect(x, y, w, h);

        checkAutoSave();
    }

    _wnd->updatePixmap(_pixelMap);
    showScreenshot();

    // The selector should be closed only after it is hidden; otherwise, with the "-m"
    // command-line option, the app will exit because its last window will be closed.
    _selector->hide();
    _selector->deleteLater();
    _selector = nullptr;
}
