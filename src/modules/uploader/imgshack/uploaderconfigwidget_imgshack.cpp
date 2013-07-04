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

#include "uploaderconfigwidget_imgshack.h"
#include "ui_uploaderconfigwidget_imgshack.h"

#include "uploaderconfig.h"

#include <QtCore/QVariant>

UploaderConfigWidget_ImgShack::UploaderConfigWidget_ImgShack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploaderConfigWidget_ImgShack)
{
    ui->setupUi(this);
	
	// load settings
	UploaderConfig config;
	
	QVariantMap loadedValues;
	loadedValues.insert(KEY_IMGSHK_USER, "");
	loadedValues.insert(KEY_IMGSHK_PASS, "");
	
	loadedValues = config.loadSettings("imageshack.us", loadedValues);
	
	ui->editUsername->setText(loadedValues[KEY_IMGSHK_USER].toString());
	ui->editPass->setText(loadedValues[KEY_IMGSHK_PASS].toString());
}

UploaderConfigWidget_ImgShack::~UploaderConfigWidget_ImgShack()
{
    delete ui;
}

void UploaderConfigWidget_ImgShack::saveSettings()
{
	UploaderConfig config;
	
	QVariantMap savingValues;
	
	savingValues.insert(KEY_IMGSHK_USER, ui->editUsername->text());
	savingValues.insert(KEY_IMGSHK_PASS, ui->editPass->text());
	
	config.saveSettings("imageshack.us", savingValues);
}


void UploaderConfigWidget_ImgShack::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
