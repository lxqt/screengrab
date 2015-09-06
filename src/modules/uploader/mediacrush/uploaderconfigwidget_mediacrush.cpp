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

#include "uploaderconfigwidget_mediacrush.h"
#include "ui_uploaderconfigwidget_mediacrush.h"

#include "uploaderconfig.h"

#include <QDebug>

UploaderConfigWidget_MediaCrush::UploaderConfigWidget_MediaCrush(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploaderConfigWidget_MediaCrush)
{
    ui->setupUi(this);

    // load settings
    UploaderConfig config;

    QVariantMap loadedValues;
    loadedValues.insert(KEY_MCSH_URL, "");

    loadedValues = config.loadSettings("mediacru.sh", loadedValues);
    ui->editUrl->setText(loadedValues[KEY_MCSH_URL].toString());

}

UploaderConfigWidget_MediaCrush::~UploaderConfigWidget_MediaCrush()
{
    delete ui;
}

void UploaderConfigWidget_MediaCrush::saveSettings()
{
    UploaderConfig config;

    QVariantMap savingValues;
    savingValues.insert(KEY_MCSH_URL, ui->editUrl->text());

    config.saveSettings("mediacru.sh", savingValues);
}
