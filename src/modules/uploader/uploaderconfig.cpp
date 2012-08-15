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

#include "core/config.h"

#include <QDebug>

const QString  groupName = "imageshack.us";
QStringList UploaderConfig::_labelsList = QStringList() << "ImgUr" << "ImageShack";

UploaderConfig::UploaderConfig()
{
    QString configFile = Config::getConfigDir();
#ifdef Q_WS_X11    
    configFile += "uploader.conf";
#endif

#ifdef Q_WS_WIN
    configFile += "uploader.ini";
#endif
    _settings = new QSettings(configFile, QSettings::IniFormat);        
		
	_groupsList << "imgur.com" << "imageshack.us";
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
