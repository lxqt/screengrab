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
    _settings = new QSettings(getConfigFile(), QSettings::IniFormat );

    _shortcuts = new ShortcutManager(_settings);

    // check existing config file
    if (!QFile::exists(getConfigFile()))
    {
        // creating conf fole from set defaults
        setDefaultSettings();
        saveSettings();
    }

    _imageFormats << "png" << "jpg" << "bmp";

#if QT_VERSION >= 0x040500
    _settings->setIniCodec("UTF-8");
#endif
    _scrNum = 0;
}

Config::~Config()
{
    delete _shortcuts;
    delete _settings;
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
    _confData[key] = val;
}

QVariant Config::value(const QString &key)
{
    return _confData[key];
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
    QString str = QString::number(_scrNum);
    
    if (_scrNum < 10)
    {
        str.prepend("0");
    }
    
    return str;
}

int Config::getScrNum() const
{
    return _scrNum;
}

void Config::increaseScrNum()
{
    _scrNum++;
}

void Config::resetScrNum()
{
    _scrNum = 0;
}

void Config::updateLastSaveDate()
{
    _dateLastSaving = QDateTime::currentDateTime();
}

QDateTime Config::getLastSaveDate() const
{
    return _dateLastSaving;
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
    _settings->beginGroup("Display");
    _settings->setValue(KEY_WND_WIDTH, getRestoredWndSize().width());
    _settings->setValue(KEY_WND_HEIGHT, getRestoredWndSize().height());
    _settings->endGroup();
}

// load all settings  from conf file
void Config::loadSettings()
{
    _settings->beginGroup("Base");
    setSaveDir(_settings->value(KEY_SAVEDIR, getDirNameDefault()).toString() );
    setSaveFileName(_settings->value(KEY_SAVENAME,DEF_SAVE_NAME).toString());
    setSaveFormat(_settings->value(KEY_SAVEFORMAT, DEF_SAVE_FORMAT).toString());
    setDefDelay(_settings->value(KEY_DELAY, DEF_DELAY).toInt());
    setAutoCopyFilenameOnSaving(_settings->value(KEY_FILENAME_TO_CLB, DEF_FILENAME_TO_CLB).toInt());
    setDateTimeInFilename(_settings->value(KEY_FILENAMEDATE, DEF_DATETIME_FILENAME).toBool());
    setDateTimeTpl(_settings->value(KEY_DATETIME_TPL, DEF_DATETIME_TPL).toString());
    setAutoSave(_settings->value(KEY_AUTOSAVE, DEF_AUTO_SAVE).toBool());
    setAutoSaveFirst(_settings->value(KEY_AUTOSAVE_FIRST, DEF_AUTO_SAVE_FIRST).toBool());
#ifdef Q_WS_X11
    setNoDecorX11(_settings->value(KEY_NODECOR, DEF_X11_NODECOR).toBool());
#endif
    setImageQuality(_settings->value(KEY_IMG_QUALITY, DEF_IMG_QUALITY).toInt());
    _settings->endGroup();

    _settings->beginGroup("Display");
    setTrayMessages(_settings->value(KEY_TRAYMESSAGES, DEF_TRAY_MESS_TYPE).toInt());
    setSavedSizeOnExit(_settings->value(KEY_SAVEWND, DEF_SAVED_SIZE).toBool());
    setTimeTrayMess(_settings->value(KEY_TIME_NOTIFY, DEF_TIME_TRAY_MESS).toInt( ));
    setZoomAroundMouse(_settings->value(KEY_ZOOMBOX, DEF_ZOOM_AROUND_MOUSE).toBool());
    setRestoredWndSize(_settings->value(KEY_WND_WIDTH, DEF_WND_WIDTH).toInt(), _settings->value(KEY_WND_HEIGHT, DEF_WND_HEIGHT).toInt());
    setShowTrayIcon(_settings->value(KEY_SHOW_TRAY, DEF_SHOW_TRAY).toBool());
    _settings->endGroup();

    _settings->beginGroup("System");
    setCloseInTray(_settings->value(KEY_CLOSE_INTRAY, DEF_CLOSE_IN_TRAY).toBool());
    setAllowMultipleInstance(_settings->value(KEY_ALLOW_COPIES, DEF_ALLOW_COPIES).toBool());
	setEnableExtView(_settings->value(KEY_ENABLE_EXT_VIEWER, DEF_ENABLE_EXT_VIEWER).toBool());
    _settings->endGroup();

    setDelay(getDefDelay());

    _shortcuts->loadSettings();
}

void Config::saveSettings()
{
    _settings->beginGroup("Base");
    _settings->setValue(KEY_SAVEDIR, getSaveDir());
    _settings->setValue(KEY_SAVENAME, getSaveFileName());
    _settings->setValue(KEY_SAVEFORMAT, getSaveFormat());
    _settings->setValue(KEY_DELAY, getDefDelay());
    _settings->setValue(KEY_FILENAME_TO_CLB, getAutoCopyFilenameOnSaving());
    _settings->setValue(KEY_FILENAMEDATE, getDateTimeInFilename());
    _settings->setValue(KEY_DATETIME_TPL, getDateTimeTpl());
    _settings->setValue(KEY_AUTOSAVE, getAutoSave());
    _settings->setValue(KEY_AUTOSAVE_FIRST, getAutoSaveFirst());
    _settings->setValue(KEY_IMG_QUALITY, getImageQuality());
#ifdef Q_WS_X11
    _settings->setValue(KEY_NODECOR, getNoDecorX11());
#endif
    _settings->endGroup();

    _settings->beginGroup("Display");
    _settings->setValue(KEY_TRAYMESSAGES, getTrayMessages());
    _settings->setValue(KEY_TIME_NOTIFY, getTimeTrayMess());
    _settings->setValue(KEY_SAVEWND, getSavedSizeOnExit());
    _settings->setValue(KEY_ZOOMBOX, getZoomAroundMouse());
    _settings->setValue(KEY_SHOW_TRAY, getShowTrayIcon());
    _settings->endGroup();
    saveWndSize();

    _settings->beginGroup("System");
    _settings->setValue(KEY_CLOSE_INTRAY, getCloseInTray());
    _settings->setValue(KEY_ALLOW_COPIES, getAllowMultipleInstance());
	_settings->setValue(KEY_ENABLE_EXT_VIEWER, getEnableExtView());
    _settings->endGroup();

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
    return _imageFormats.indexOf(getSaveFormat());
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
