/***************************************************************************
 *   Copyright (C) 2009 - 2013 by Artem 'DOOMer' Galichkin                        *
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

#include "src/core/config.h"
#include "core.h"

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QVector>
#include <QtGui/QDesktopServices>

#include <QDebug>

const QString KEY_SAVEDIR = "defDir";
const QString KEY_SAVENAME = "defFilename";
const QString KEY_SAVEFORMAT = "defImgFormat";
const QString KEY_DELAY_DEF = "defDelay";
const QString KEY_DELAY = "delay";
const QString KEY_IMG_QUALITY = "imageQuality";
const QString KEY_FILENAMEDATE = "insDateTimeInFilename";
const QString KEY_DATETIME_TPL = "templateDateTime";
const QString KEY_FILENAME_TO_CLB = "CopyFilenameToClipboard";
const QString KEY_AUTOSAVE = "autoSave";
const QString KEY_AUTOSAVE_FIRST = "autoSaveFirst";
const QString KEY_SHOW_TRAY = "showTrayIcon";

const QString KEY_SAVEWND = "zoomAroundMouse";
const QString KEY_WND_WIDTH = "windowWidth";
const QString KEY_WND_HEIGHT = "windowHeight";
const QString KEY_ZOOMBOX = "zoomAroundMouse";
const QString KEY_TRAYMESSAGES = "trayMessages";
const QString KEY_TIME_NOTIFY = "timeTrayMessages";
const QString KEY_ALLOW_COPIES = "AllowCopies";

const QString KEY_CLOSE_INTRAY = "closeInTray";
const QString KEY_TYPE_SCREEN = "typeScreenDefault";

const QString KEY_ENABLE_EXT_VIEWER = "enbaleExternalView";

#ifdef Q_WS_X11
const QString KEY_NODECOR = "noDecorations";
#endif

Config* Config::ptrInstance = 0;

// constructor
Config::Config()
{
    settings = new QSettings(getConfigFile(), QSettings::IniFormat );

    _shortcuts = new ShortcutManager(settings);

    // check existing config file
    if (!QFile::exists(getConfigFile()))
    {
        // creating conf fole from set defaults
        setDefaultSettings();
        saveSettings();
    }

    imageFormats << "png" << "jpg" << "bmp";

#if QT_VERSION >= 0x040500
    settings->setIniCodec("UTF-8");
#endif
    scrNum = 0;
}

Config::~Config()
{
    delete _shortcuts;
    delete settings;
}

Config* Config::instance()
{
    if (!ptrInstance)
    {
        ptrInstance = new Config;
    }

    return ptrInstance;
}

void Config::setValue(const QString &key, QVariant val)
{
    confData[key] = val;
}

QVariant Config::value(const QString &key)
{
    return confData[key];
}

void Config::killInstance()
{
    if (ptrInstance)
    {
        delete ptrInstance;
        ptrInstance = 0;
    }
}

QString Config::getConfigFile()
{
    QString configFile;

#ifdef Q_WS_X11
    configFile = Config::getConfigDir() + "screengrab.conf";
    
    // moving old stile storange setting to XDG_CONFIG_HOME storage
    QString oldConfigFile = QDir::homePath()+ QDir::separator()+".screengrab"+ QDir::separator() + "screengrab.conf";
    if (QFile::exists(oldConfigFile) == true && QFile::exists(configFile) == false)
    {
        QFile::rename(oldConfigFile, configFile);
    }
#endif

#ifdef Q_WS_WIN
    configFile = Config::getConfigDir() + +"screengrab.ini";
#endif
    
    return configFile;
}

QString Config::getConfigDir()
{
    QString configDir;
#ifdef Q_WS_X11
    #ifdef SG_XDG_CONFIG_SUPPORT
        // old style config path    
        QString oldConfigDir = QDir::homePath()+ QDir::separator()+".screengrab"+ QDir::separator();
        configDir = qgetenv("XDG_CONFIG_HOME");
        
        // Ubuntu hack -- if XDG_CONFIG_HOME is missing
        if (configDir.isEmpty() == true)
        {
            configDir = QDir::homePath();
            configDir += QDir::separator();
            configDir += ".config";
        }
        
        configDir.append(QDir::separator());
        configDir.append("screengrab");
        configDir.append(QDir::separator());
    #else
        configDir = QDir::homePath()+ QDir::separator()+".screengrab"+ QDir::separator();
    #endif    
#endif
    
    // if win32
#ifdef Q_WS_WIN
    configDir.append(QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "ScreenGrab" + QDir::toNativeSeparators(QDir::separator()));
#endif
    return configDir;
}


// public methods

QString Config::getScrNumStr()
{
    QString str = QString::number(scrNum);
    
    if (scrNum < 10)
    {
        str.prepend("0");
    }
    
    return str;
}

int Config::getScrNum() const
{
    return scrNum;
}

void Config::increaseScrNum()
{
    scrNum++;
}

void Config::resetScrNum()
{
    scrNum = 0;
}

void Config::updateLastSaveDate()
{
    dateLastSaving = QDateTime::currentDateTime();
}

QDateTime Config::getLastSaveDate() const
{
    return dateLastSaving;
}

bool Config::getEnableExtView()
{
	return value(KEY_ENABLE_EXT_VIEWER).toBool();
}

void Config::setEnableExtView(bool val)
{
	setValue(KEY_ENABLE_EXT_VIEWER, val);
}


QString Config::getSaveDir()
{
    return value(KEY_SAVEDIR).toString();
}

void Config::setSaveDir(QString path)
{
    setValue(KEY_SAVEDIR, path);
}

QString Config::getSaveFileName()
{
    return value(KEY_SAVENAME).toString();
}

void Config::setSaveFileName(QString fileName)
{
    setValue(KEY_SAVENAME, fileName);
}

QString Config::getSaveFormat()
{
    return value(KEY_SAVEFORMAT).toString();
}

void Config::setSaveFormat(QString format)
{
    setValue(KEY_SAVEFORMAT, format);
}

quint8 Config::getDefDelay()
{
    return value(KEY_DELAY_DEF).toInt();
}

void Config::setDefDelay(quint8 sec)
{
    setValue(KEY_DELAY_DEF, sec);
}

quint8 Config::getDelay()
{
    return value(KEY_DELAY).toInt();
}

void Config::setDelay(quint8 sec)
{
    setValue(KEY_DELAY, sec);
}

quint8 Config::getAutoCopyFilenameOnSaving()
{
    return value(KEY_FILENAME_TO_CLB).toInt();
}

void Config::setAutoCopyFilenameOnSaving(quint8 val)
{
    setValue(KEY_FILENAME_TO_CLB, val);
}


quint8 Config::getTrayMessages()
{
    return value(KEY_TRAYMESSAGES).toInt();
}

void Config::setTrayMessages(quint8 type)
{
    setValue(KEY_TRAYMESSAGES, type);
}

bool Config::getAllowMultipleInstance()
{
    return value(KEY_ALLOW_COPIES).toBool();
}

void Config::setAllowMultipleInstance(bool val)
{
    setValue(KEY_ALLOW_COPIES, val);
}

bool Config::getCloseInTray()
{
    return value(KEY_CLOSE_INTRAY).toBool();
}

void Config::setCloseInTray(bool val)
{
    setValue(KEY_CLOSE_INTRAY, val);
}

int Config::getTypeScreen()
{
    return value(KEY_TYPE_SCREEN).toInt();
}

void Config::setTypeScreen(quint8 type)
{
    setValue(KEY_TYPE_SCREEN, type);
}

bool Config::getSavedSizeOnExit()
{
    return value(KEY_SAVEWND).toBool();
}

void Config::setSavedSizeOnExit(bool val)
{
    setValue(KEY_SAVEWND, val);
}

quint8 Config::getTimeTrayMess()
{
    return value(KEY_TIME_NOTIFY).toInt();
}

void Config::setTimeTrayMess(int sec)
{
    setValue(KEY_TIME_NOTIFY, sec);
}

QSize Config::getRestoredWndSize()
{
    QSize wndSize(value(KEY_WND_WIDTH).toInt(), value(KEY_WND_HEIGHT).toInt());
    return wndSize;
}

void Config::setRestoredWndSize(int w, int h)
{
    if (w <= DEF_WND_WIDTH)
    {
        w = DEF_WND_WIDTH;
    }

    if (h <= DEF_WND_HEIGHT)
    {
        h = DEF_WND_HEIGHT;
    }

    setValue(KEY_WND_WIDTH, w);
    setValue(KEY_WND_HEIGHT, h);
}

bool Config::getDateTimeInFilename()
{
    return value(KEY_FILENAMEDATE).toBool();
}

void Config::setDateTimeInFilename(bool val)
{
    setValue(KEY_FILENAMEDATE, val);
}

bool Config::getAutoSave()
{
    return value(KEY_AUTOSAVE).toBool();
}

void Config::setAutoSave(bool val)
{
    setValue(KEY_AUTOSAVE, val);
}

quint8 Config::getImageQuality()
{
    return value(KEY_IMG_QUALITY).toInt();
}

void Config::setImageQuality(quint8 qualuty)
{
    setValue(KEY_IMG_QUALITY, qualuty);
}

bool Config::getAutoSaveFirst()
{
    return value(KEY_AUTOSAVE_FIRST).toBool();
}

void Config::setAutoSaveFirst(bool val)
{
    setValue(KEY_AUTOSAVE_FIRST, val);
}


QString Config::getDateTimeTpl()
{
    return value(KEY_DATETIME_TPL).toString();
}

void Config::setDateTimeTpl(QString tpl)
{
    setValue(KEY_DATETIME_TPL, tpl);
}

bool Config::getZoomAroundMouse()
{
    return value(KEY_ZOOMBOX).toBool();
}

void Config::setZoomAroundMouse(bool val)
{
    setValue(KEY_ZOOMBOX, val);
}

bool Config::getShowTrayIcon()
{
    return value(KEY_SHOW_TRAY).toBool();
}

void Config::setShowTrayIcon(bool val)
{
    setValue(KEY_SHOW_TRAY, val);
}

#ifdef Q_WS_X11
bool Config::getNoDecorX11()
{
    return value(KEY_NODECOR).toBool();
}

void Config::setNoDecorX11(bool val)
{
    setValue(KEY_NODECOR, val);
}
#endif

void Config::saveWndSize()
{
    // saving size
    settings->beginGroup("Display");
    settings->setValue(KEY_WND_WIDTH, getRestoredWndSize().width());
    settings->setValue(KEY_WND_HEIGHT, getRestoredWndSize().height());
    settings->endGroup();
}

// load all settings  from conf file
void Config::loadSettings()
{
    settings->beginGroup("Base");
    setSaveDir(settings->value(KEY_SAVEDIR, getDirNameDefault()).toString() );
    setSaveFileName(settings->value(KEY_SAVENAME,DEF_SAVE_NAME).toString());
    setSaveFormat(settings->value(KEY_SAVEFORMAT, DEF_SAVE_FORMAT).toString());
    setDefDelay(settings->value(KEY_DELAY, DEF_DELAY).toInt());
    setAutoCopyFilenameOnSaving(settings->value(KEY_FILENAME_TO_CLB, DEF_FILENAME_TO_CLB).toInt());
    setDateTimeInFilename(settings->value(KEY_FILENAMEDATE, DEF_DATETIME_FILENAME).toBool());
    setDateTimeTpl(settings->value(KEY_DATETIME_TPL, DEF_DATETIME_TPL).toString());
    setAutoSave(settings->value(KEY_AUTOSAVE, DEF_AUTO_SAVE).toBool());
    setAutoSaveFirst(settings->value(KEY_AUTOSAVE_FIRST, DEF_AUTO_SAVE_FIRST).toBool());
#ifdef Q_WS_X11
    setNoDecorX11(settings->value(KEY_NODECOR, DEF_X11_NODECOR).toBool());
#endif
    setImageQuality(settings->value(KEY_IMG_QUALITY, DEF_IMG_QUALITY).toInt());
    settings->endGroup();

    settings->beginGroup("Display");
    setTrayMessages(settings->value(KEY_TRAYMESSAGES, DEF_TRAY_MESS_TYPE).toInt());
    setSavedSizeOnExit(settings->value(KEY_SAVEWND, DEF_SAVED_SIZE).toBool());
    setTimeTrayMess(settings->value(KEY_TIME_NOTIFY, DEF_TIME_TRAY_MESS).toInt( ));
    setZoomAroundMouse(settings->value(KEY_ZOOMBOX, DEF_ZOOM_AROUND_MOUSE).toBool());
    setRestoredWndSize(settings->value(KEY_WND_WIDTH, DEF_WND_WIDTH).toInt(), settings->value(KEY_WND_HEIGHT, DEF_WND_HEIGHT).toInt());
    setShowTrayIcon(settings->value(KEY_SHOW_TRAY, DEF_SHOW_TRAY).toBool());
    settings->endGroup();

    settings->beginGroup("System");
    setCloseInTray(settings->value(KEY_CLOSE_INTRAY, DEF_CLOSE_IN_TRAY).toBool());
    setAllowMultipleInstance(settings->value(KEY_ALLOW_COPIES, DEF_ALLOW_COPIES).toBool());
	setEnableExtView(settings->value(KEY_ENABLE_EXT_VIEWER, DEF_ENABLE_EXT_VIEWER).toBool());
    settings->endGroup();

    setDelay(getDefDelay());

    _shortcuts->loadSettings();
}

void Config::saveSettings()
{
    settings->beginGroup("Base");
    settings->setValue(KEY_SAVEDIR, getSaveDir());
    settings->setValue(KEY_SAVENAME, getSaveFileName());
    settings->setValue(KEY_SAVEFORMAT, getSaveFormat());
    settings->setValue(KEY_DELAY, getDefDelay());
    settings->setValue(KEY_FILENAME_TO_CLB, getAutoCopyFilenameOnSaving());
    settings->setValue(KEY_FILENAMEDATE, getDateTimeInFilename());
    settings->setValue(KEY_DATETIME_TPL, getDateTimeTpl());
    settings->setValue(KEY_AUTOSAVE, getAutoSave());
    settings->setValue(KEY_AUTOSAVE_FIRST, getAutoSaveFirst());
    settings->setValue(KEY_IMG_QUALITY, getImageQuality());
#ifdef Q_WS_X11
    settings->setValue(KEY_NODECOR, getNoDecorX11());
#endif
    settings->endGroup();

    settings->beginGroup("Display");
    settings->setValue(KEY_TRAYMESSAGES, getTrayMessages());
    settings->setValue(KEY_TIME_NOTIFY, getTimeTrayMess());
    settings->setValue(KEY_SAVEWND, getSavedSizeOnExit());
    settings->setValue(KEY_ZOOMBOX, getZoomAroundMouse());
    settings->setValue(KEY_SHOW_TRAY, getShowTrayIcon());
    settings->endGroup();
    saveWndSize();

    settings->beginGroup("System");
    settings->setValue(KEY_CLOSE_INTRAY, getCloseInTray());
    settings->setValue(KEY_ALLOW_COPIES, getAllowMultipleInstance());
	settings->setValue(KEY_ENABLE_EXT_VIEWER, getEnableExtView());
    settings->endGroup();

    _shortcuts->saveSettings();
    
    resetScrNum();
}

// set default values
void Config::setDefaultSettings()
{
    setSaveDir(getDirNameDefault());
    setSaveFileName(DEF_SAVE_NAME);
    setSaveFormat(DEF_SAVE_FORMAT);
    setDefDelay(DEF_DELAY);
    setImageQuality(DEF_IMG_QUALITY);
    setDateTimeInFilename(DEF_DATETIME_FILENAME);
    setDateTimeTpl(DEF_DATETIME_TPL);
    setAutoCopyFilenameOnSaving(DEF_FILENAME_TO_CLB);
    setAutoSave(DEF_AUTO_SAVE);
    setAutoSaveFirst(DEF_AUTO_SAVE_FIRST);
    setTrayMessages(DEF_TRAY_MESS_TYPE);
    setZoomAroundMouse(DEF_ZOOM_AROUND_MOUSE);
    setCloseInTray(DEF_CLOSE_IN_TRAY);
    setTimeTrayMess(DEF_TIME_TRAY_MESS);
    setAllowMultipleInstance(DEF_ALLOW_COPIES);
    setRestoredWndSize(DEF_WND_WIDTH, DEF_WND_HEIGHT);
    setShowTrayIcon(DEF_SHOW_TRAY);
	setEnableExtView(DEF_ENABLE_EXT_VIEWER);

    _shortcuts->setDefaultSettings();

#ifdef Q_WS_X11
    setNoDecorX11(DEF_X11_NODECOR);
#endif

    setDelay(DEF_DELAY);
    setSavedSizeOnExit(DEF_SAVED_SIZE);

	quint8 countModules = Core::instance()->modules()->count();	
	for (int i = 0; i < countModules; ++i) 
	{
		Core::instance()->modules()->getModule(i)->defaultSettings();
	}
}

// get defaukt directory path
QString Config::getDirNameDefault()
{
#ifdef Q_WS_X11
    return QDir::homePath()+QDir::separator();    ;
#endif
#ifdef Q_WS_WIN
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() ;
#endif
}

// get id of default save format
int Config::getDefaultFormatID()
{
    return imageFormats.indexOf(getSaveFormat());
}

QString Config::getSysLang()
{
#ifdef Q_WS_X11
    QByteArray lang = qgetenv("LC_ALL");

    if (lang.isEmpty())
    {
        lang = qgetenv("LC_MESSAGES");
    }
    if (lang.isEmpty())
    {
        lang = qgetenv("LANG");
    }
    if (!lang.isEmpty())
    {
        return QLocale (lang).name();
    }
    else
    {
        return  QLocale::system().name();
    }
#endif
#ifdef Q_WS_WIN
    return  QLocale::system().name();
#endif
}


// CmdLine* Config::cmdLine()
// {
//     return cmd;
// }

ShortcutManager* Config::shortcuts()
{
    return _shortcuts;
}
