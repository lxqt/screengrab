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

#ifndef UPLOADERCONFIG_H
#define UPLOADERCONFIG_H

#include <QSettings>
#include <QStringList>
#include <QVariant>
#include <QMap>

//  Uploader config file common keys
#define KEY_AUTO_COPY_RESULT_LIMK   "autoCopyDirectLink"
#define KEY_DEFAULT_HOST            "defaultHost"

class UploaderConfig
{

public:
    UploaderConfig();
    ~UploaderConfig();

    static QStringList labelsList();

    QVariantMap loadSettings(const QByteArray& group, QVariantMap& mapValues);
    QVariant loadSingleParam(const QByteArray& group, const QByteArray& param);
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
