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

#include "uploaderconfigwidget_imgur.h"
#include "ui_uploaderconfigwidget_imgur.h"

#include "uploaderconfig.h"

#include <QVariant>

UploaderConfigWidget_ImgUr::UploaderConfigWidget_ImgUr(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::UploaderConfigWidget_ImgUr)
{
    _ui->setupUi(this);

    // load settings
    UploaderConfig config;

    QVariantMap loadedValues;
    // TODO add imgur settings load

}

UploaderConfigWidget_ImgUr::~UploaderConfigWidget_ImgUr()
{
    delete _ui;
}

void UploaderConfigWidget_ImgUr::saveSettings()
{
    UploaderConfig config;

    QVariantMap savingValues;
}


void UploaderConfigWidget_ImgUr::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
