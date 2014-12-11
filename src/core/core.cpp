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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QMutex>
#include <QWaitCondition>
#include <QApplication>
#include <QDesktopWidget>

#include <QChar>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QUuid>

#include <QDebug>

#include "core/core.h"
#include <KF5/KWindowSystem/KWindowSystem>

Core* Core::corePtr = 0;

Core::Core()
{
    qRegisterMetaType<StateNotifyMessage>("StateNotifyMessage");

    conf = Config::instance();
    conf->loadSettings();

    _pixelMap = new QPixmap;
    _selector = 0;
    _firstScreen = true;

    _cmdLine.setApplicationDescription("ScreenGrab " + tr("is a crossplatform application for fast creating screenshots of your desktop."));
    _cmdLine.addHelpOption();
    _cmdLine.addVersionOption();

    QString optFullScreenDescr = tr("Take a fullscreen screenshot");
    QCommandLineOption optFullScreen(
                QStringList() << "f" << "fullscreen", optFullScreenDescr);
    _cmdLine.addOption(optFullScreen);
    _screenTypeOpts.append(optFullScreen);

    QString optActiveWndDescr = tr("Take a screenshot of the active window");
    QCommandLineOption optActiveWnd(
                QStringList() << "a" << "active", optActiveWndDescr);
    _cmdLine.addOption(optActiveWnd);
    _screenTypeOpts.append(optActiveWnd);

    QString optSelectedRectDescr = tr("Take a screenshot of the active window");
    QCommandLineOption optSelectedRect(
                QStringList() << "r" << "region", optSelectedRectDescr);
    _cmdLine.addOption(optSelectedRect);
    _screenTypeOpts.append(optSelectedRect);

    QString optRunMinimizedDescr = tr("Run the application with a hidden main window");
    QCommandLineOption optRunMinimized(
                QStringList() << "m" << "minimized", optRunMinimizedDescr);
    _cmdLine.addOption(optRunMinimized);

    sleep(250);
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
    conf->killInstance();
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
    sleep(400); // delay for hide "fade effect" bug in the KWin with compositing
    _firstScreen = first;

    // Update date last crenshot, if it is  a first screen
    if (_firstScreen == true)
        conf->updateLastSaveDate();

    // grb pixmap of desktop
    switch(conf->getTypeScreen())
    {
    case 0:
    {
        *_pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
        checkAutoSave(first);
        Q_EMIT newScreenShot(_pixelMap);
        break;
    }
    case 1:
    {
        getActiveWindow();
        checkAutoSave(first);
        Q_EMIT newScreenShot(_pixelMap);
        break;
    }
    case 2:
    {
        _selector = new RegionSelect(conf);
        connect(_selector, SIGNAL(processDone(bool)), this, SLOT(regionGrabbed(bool)));
        break;
    }
    case 3:
    {
        _selector = new RegionSelect(conf, _lastSelectedArea);
        connect(_selector, SIGNAL(processDone(bool)), this, SLOT(regionGrabbed(bool)));
        break;
    }
    default:
        *_pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
        break;
    }
}

void Core::checkAutoSave(bool first)
{
    if (conf->getAutoSave())
    {
        // hack
        if (first)
        {
            if (conf->getAutoSaveFirst())
                QTimer::singleShot(600, this, SLOT(autoSave()));
        }
        else
            autoSave();
    }
    else
    {
        if (!first)
        {
            StateNotifyMessage message(tr("New screen"), tr("New screen is getted!"));
            Q_EMIT sendStateNotifyMessage(message);
        }
    }
}

void Core::getActiveWindow()
{
    WId wnd = KWindowSystem::activeWindow();

    if (!wnd)
    {
        *_pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId());
        exit(1);
    }

    // no decorations option is selected
    if (conf->getNoDecoration())
    {
        *_pixelMap = QPixmap::grabWindow(wnd);
        return;
    }

    KWindowInfo info(wnd, NET::XAWMState | NET::WMFrameExtents);

    if (info.mappingState() != NET::Visible)
        qWarning() << "Window not visible";

    QRect geometry = info.frameGeometry();
    *_pixelMap = QPixmap::grabWindow(QApplication::desktop()->winId(),
                                     geometry.x(),
                                     geometry.y(),
                                     geometry.width(),
                                     geometry.height());
}

QString Core::getSaveFilePath(QString format)
{
    QString initPath;

    do
    {
        if (conf->getDateTimeInFilename())
            initPath = conf->getSaveDir() + conf->getSaveFileName() + "-" + getDateTimeFileName() + "." + format;
        else
        {
            if (conf->getScrNum() != 0)
                initPath = conf->getSaveDir() + conf->getSaveFileName() + conf->getScrNumStr() + "." + format;
            else
                initPath = conf->getSaveDir() + conf->getSaveFileName() + "." + format;
        }
    } while (checkExsistFile(initPath));

    return initPath;
}

bool Core::checkExsistFile(QString path)
{
    bool exist = QFile::exists(path);

    if (exist)
        conf->increaseScrNum();

    return exist;
}

QString Core::getDateTimeFileName()
{
    QString currentDateTime = QDateTime::currentDateTime().toString(conf->getDateTimeTpl());

    if (currentDateTime == conf->getLastSaveDate().toString(conf->getDateTimeTpl()) && conf->getScrNum() != 0)
        currentDateTime += "-" + conf->getScrNumStr();
    else
        conf->resetScrNum();

    return currentDateTime;
}

void Core::updatePixmap()
{
    if (QFile::exists(_tempFilename))
    {
        _pixelMap->load(_tempFilename, "png");
        Q_EMIT newScreenShot(_pixelMap);
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
    if (QFile::exists(_tempFilename) == true)
    {
        QFile::remove(_tempFilename);
        _tempFilename.clear();
    }
}

// save screen
bool Core::writeScreen(QString& fileName, QString& format, bool tmpScreen)
{
    // adding extension format
    if (!fileName.contains("." + format))
        fileName.append("." + format);

    // saving temp file (for uploader module)
    if (tmpScreen)
    {
        if (!fileName.isEmpty())
            return _pixelMap->save(fileName, format.toLatin1(), conf->getImageQuality());
        else
            return false;
    }

    // writing file
    bool saved;
    if (!fileName.isEmpty())
    {
        if (format == "jpg")
            saved = _pixelMap->save(fileName,format.toLatin1(), conf->getImageQuality());
        else
            saved = _pixelMap->save(fileName,format.toLatin1(), -1);

        if (saved)
        {
            StateNotifyMessage message(tr("Saved"), tr("Saved to ") + fileName);

            message.message = message.message + copyFileNameToCliipboard(fileName);
            conf->updateLastSaveDate();
            Q_EMIT sendStateNotifyMessage(message);
        }
        else
            qWarning() << "Error saving file " << fileName;
    }
    else
        saved = false;

    return saved;
}

QString Core::copyFileNameToCliipboard(QString file)
{
    QString retString = "";
    switch (conf->getAutoCopyFilenameOnSaving())
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

void Core::copyScreen()
{
    QApplication::clipboard()->setPixmap(*_pixelMap, QClipboard::Clipboard);
    StateNotifyMessage message(tr("Copied"), tr("Screenshot is copied to clipboard"));
    Q_EMIT sendStateNotifyMessage(message);
}

void Core::openInExtViewer()
{
    if (conf->getEnableExtView() == 1)
    {
        QString format = "png";
        QString tempFileName = getTempFilename(format);
        writeScreen(tempFileName, format, true);

        QString exec;
        exec = "xdg-open";
        QStringList args;
        args << tempFileName;

        QProcess *execProcess = new QProcess(this);
        connect(execProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                this, SLOT(closeExtViewer(int,QProcess::ExitStatus)));
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
    for (int i = 0; i < options.count(); ++i) {
        if (_cmdLine.isSet(options.at(i))) {
            return true;
        }
    }

    return false;
}

void Core::processCmdLineOpts(const QStringList& arguments)
{
    _cmdLine.process(arguments);

    // Check commandline parameters and set screenshot type
    for (int i=0; i < _screenTypeOpts.count(); ++i) {
        if (_cmdLine.isSet(_screenTypeOpts.at(i))) {
            conf->setTypeScreen(i);
        }
    }
}

bool Core::runAsMinimized()
{
    bool isMinnimize = false;

    if (_cmdLine.isSet("minimized") || _cmdLine.isSet("m")) {
        isMinnimize = true;
    }

    return isMinnimize;
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

QPixmap* Core::getPixmap()
{
    return _pixelMap;
}

QByteArray Core::getScreen()
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    _pixelMap->save(&buffer, conf->getSaveFormat().toLatin1());
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

    Q_EMIT newScreenShot(_pixelMap);
    _selector->deleteLater();
}
