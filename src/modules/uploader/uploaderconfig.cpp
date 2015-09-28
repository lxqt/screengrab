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

#include "uploaderconfig.h"
#include "core/config.h"

#include <QFile>

#include <QDebug>
#include <QDir>

// common defaults
#define DEF_AUTO_COPY_RESULT_LIMK   false
#define DEF_DEFAULT_HOST            "Imgur"

// mediacru.sh settings
#define DEF_MCSH_URL                "https://mediacru.sh/api/upload/file"

QStringList UploaderConfig::_labelsList = QStringList() << "Imgur" << "MediaCrush";

UploaderConfig::UploaderConfig()
{
    QString configFile = Config::getConfigDir() + QDir::separator() + "uploader.conf";
    _settings = new QSettings(configFile, QSettings::IniFormat);
    _groupsList << "imgur.com" << "mediacru.sh";
}

UploaderConfig::~UploaderConfig()
{
    delete _settings;
}

QStringList UploaderConfig::labelsList()
{
    return _labelsList;
}

QVariantMap UploaderConfig::loadSettings(const QByteArray& group, QVariantMap& mapValues)
{
    QVariantMap map;

    _settings->beginGroup(group);

    QVariant defValue, iterValue;
    QVariantMap::iterator iter = mapValues.begin();
    while(iter != mapValues.end())
    {
        defValue =  iter.value();
        iterValue = _settings->value(iter.key(), defValue);
        map.insert(iter.key(), iterValue);
        ++iter;
    }

    _settings->endGroup();

    return map;
}

QVariant UploaderConfig::loadSingleParam(const QByteArray& group, const QByteArray& param)
{
    QVariant var;

    _settings->beginGroup(group);
    var = _settings->value(param);
    _settings->endGroup();

    return var;
}


void UploaderConfig::saveSettings(const QByteArray& group, QVariantMap& mapValues)
{
    _settings->beginGroup(group);

    QVariantMap::iterator iter = mapValues.begin();
    while(iter != mapValues.end())
    {
        _settings->setValue(iter.key(), iter.value());
        ++iter;
    }

    _settings->endGroup();
}

void UploaderConfig::defaultSettings()
{
    _settings->beginGroup("common");
    _settings->setValue(KEY_AUTO_COPY_RESULT_LIMK, DEF_AUTO_COPY_RESULT_LIMK);
    _settings->setValue(KEY_DEFAULT_HOST, DEF_DEFAULT_HOST);
    _settings->endGroup();

    // mediacru.sh settings
    _settings->beginGroup(_groupsList[0]);
    _settings->setValue(KEY_MCSH_URL, DEF_MCSH_URL);
    _settings->endGroup();

    // imgur.com settings
    _settings->beginGroup(_groupsList[1]);
    _settings->endGroup();
}

bool UploaderConfig::autoCopyResultLink()
{
    _settings->beginGroup("common");
    bool ret = _settings->value(KEY_AUTO_COPY_RESULT_LIMK, DEF_AUTO_COPY_RESULT_LIMK).toBool();
    _settings->endGroup();

    return ret;
}


bool UploaderConfig::checkExistsConfigFile() const
{
    return QFile::exists(_settings->fileName());
}
