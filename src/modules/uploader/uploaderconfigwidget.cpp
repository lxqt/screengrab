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

#include "uploaderconfigwidget.h"
#include "ui_uploaderconfigwidget.h"

#include "uploaderconfig.h"

#include <QtCore/QVariantMap>

#include <QDebug>


UploaderConfigWidget::UploaderConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UploaderConfigWidget)
{
    ui->setupUi(this);
	
	ui->settings->setCurrentWidget(ui->commonSettings);
	
	QStringList hosts = UploaderConfig::labelsList();
	ui->cbxHosts->addItems(hosts);
	ui->cbxDefaultHost->addItems(hosts);
	
	loadSettings();
	
	imgur = new UploaderConfigWidget_ImgUr(this);
	imgshack = new UploaderConfigWidget_ImgShack(this);
	
	ui->stackedHosts->addWidget(imgur);
	ui->stackedHosts->addWidget(imgshack);
	
	connect(ui->cbxHosts, SIGNAL(currentIndexChanged(int)), ui->stackedHosts, SLOT(setCurrentIndex(int)));
}

UploaderConfigWidget::~UploaderConfigWidget()
{
    delete ui;
}

void UploaderConfigWidget::loadSettings()
{
	qDebug() << "load uploder common settings";
	
	UploaderConfig config;
	QVariantMap loadValues;
	loadValues.insert("autoCopyDirectLink", QVariant(false));
	loadValues.insert(KEY_DEFAULT_HOST, "");
	loadValues = config.loadSettings("common", loadValues);
		
	QString defaultHost = loadValues[KEY_DEFAULT_HOST].toString();
	if (defaultHost.isEmpty() == true)
	{
		ui->cbxDefaultHost->setCurrentIndex(0);
	}
	else
	{
		qint8 index = config.labelsList().indexOf(defaultHost);
		
		if (index == -1)
		{
			index++;
		}

		ui->cbxDefaultHost->setCurrentIndex(index);
	}
	
	ui->checkAutoCopyMainLink->setChecked(loadValues["autoCopyDirectLink"].toBool());
}

void UploaderConfigWidget::saveSettings()
{
	UploaderConfig config;
	QVariantMap savingValues;
	savingValues.insert(KEY_AUTO_COPY_RESULT_LIMK, ui->checkAutoCopyMainLink->isChecked());
	
	QString defaultHost = config.labelsList().at(ui->cbxDefaultHost->currentIndex());
	savingValues.insert(KEY_DEFAULT_HOST, defaultHost);
	
	config.saveSettings("common", savingValues);
	
	QMetaObject::invokeMethod(imgur, "saveSettings");
	QMetaObject::invokeMethod(imgshack, "saveSettings");

}

void UploaderConfigWidget::changeEvent(QEvent *e)
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
