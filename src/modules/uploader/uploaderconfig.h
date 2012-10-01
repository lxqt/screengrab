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

#ifndef UPLOADERCONFIG_H
#define UPLOADERCONFIG_H

#include <QtCore/QSettings>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QMap>

//  Uploader config file common keys
const QString KEY_AUTO_COPY_RESULT_LIMK = "autoCopyDirectLink";
const QString KEY_DEFAULT_HOST = "defaultHost";

//  Uploader config file imageshack.us keys
const QString KEY_IMGSHK_USER = "username";
const QString KEY_IMGSHK_PASS = "password";

class UploaderConfig
{

public:
    UploaderConfig();
    ~UploaderConfig();
    
	static QStringList labelsList();
	
	QVariantMap loadSettings(const QByteArray& group, QVariantMap& mapValues);
	void saveSettings(const QByteArray& group, QVariantMap& mapValues);
	void defaultSettings();
	bool checkExistsConfigFile() const;
	bool autoCopyResultLink();
    
private:
    QSettings *_settings;
	
	QStringList _groupsList;
	static QStringList _labelsList;
};

#endif // UPLOADERCONFIG_H
