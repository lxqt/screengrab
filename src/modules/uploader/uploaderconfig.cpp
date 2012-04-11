/***************************************************************************
 *   Copyright (C) 2009 - 2012 by Artem 'DOOMer' Galichkin                        *
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

#include "uploaderconfig.h"
#include <QString>
#include <QFile>
#include <QDir>

#include "core/config.h"

#include <QDebug>

const QString  groupName = "imageshack.us";

UploaderConfig::UploaderConfig()
{    
    QString configFile = Config::getConfigDir();
#ifdef Q_WS_X11 
    // old style config settings storage
    QString oldConfigFile = QDir::homePath()+ QDir::separator()+".screengrab"+ QDir::separator() + "uploader.conf";
    
    configFile += "uploader.conf";
    
    // move config  file to new location
    if (QFile::exists(oldConfigFile) == true && QFile::exists(configFile) == false)
    {
        QFile::rename(oldConfigFile, configFile);
    }
#endif
    
#ifdef Q_WS_WIN
    configFile += "uploader.ini";
#endif
    _settings = new QSettings(configFile, QSettings::IniFormat);        
    _settingsList << "username" << "password";
}


UploaderConfig::~UploaderConfig()
{
    delete _settings;
}


QStringList UploaderConfig::loadSettings()
{
    QStringList retList;
    
    _settings->beginGroup(groupName);
        
    for (int i = 0; i < _settingsList.count(); ++i)
    {
        retList << _settings->value(_settingsList.at(i)).toString();
    }
    
    _settings->endGroup();
    
    return retList;
}


void UploaderConfig::saveSettings(const QStringList& settings)
{
    _settings->beginGroup(groupName);
    
    qDebug() << "list " << settings;
    
    for (int i = 0; i < _settingsList.count(); ++i)
    {
        QVariant val = settings.at(i);        
        _settings->setValue(_settingsList.at(i), val);
    }
    
    _settings->endGroup();
}

QVariant UploaderConfig::loadparam(const QString& param)
{
    QVariant retVal;
    _settings->beginGroup(groupName);
    retVal = _settings->value(param);
    _settings->endGroup();
    
    return retVal;
}

void UploaderConfig::saveParameter(const QString& param, const QVariant& val)
{
    _settings->beginGroup(groupName);
    _settings->setValue(param, val);
    _settings->endGroup();
}
