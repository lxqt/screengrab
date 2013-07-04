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

#include "dialoguploader.h"
#include "ui_dialoguploader.h"

#include "uploaderconfig.h"
#include "imgshack/uploader_imgshack.h"
#include "imgshack/uploader_imgshack_widget.h"
#include "imgur/uploader_imgur.h"
#include "imgur/uploader_imgur_widget.h"
#include <core/core.h>

#include <QtGui/QMessageBox>

#include <QDebug>

DialogUploader::DialogUploader(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUploader)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    uploader = 0;
	_uploaderWidget = 0;
	slotSeletHost(0);
	
    ui->cbxUploaderList->addItems(UploaderConfig::labelsList());
	
	UploaderConfig config;
	QString defaultHost = config.loadSingleParam(QByteArray("common"), KEY_DEFAULT_HOST.toAscii()).toString();
	
	if (defaultHost.isEmpty() == true)
	{
		selectedHost = 0;
	}
	else
	{
		selectedHost = config.labelsList().indexOf(defaultHost);
	}

    // load ishot preview
    QSize imgSize = Core::instance()->getPixmap()->size();
    QString pixmapSize = tr("Size: ") + QString::number(imgSize.width()) + "x" + QString::number(imgSize.height()) + tr(" pixel");
    ui->labImgSize->setText(pixmapSize);

    ui->labImage->setFixedWidth(256);
    ui->labImage->setFixedHeight(192);
    ui->labImage->setPixmap(Core::instance()->getPixmap()->scaled(ui->labImage->size(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // progressbar
    ui->progressBar->setVisible(false);
    ui->progressBar->setFormat(tr("Uploaded ") + "%p%" + " (" + "%v" + " of " + "%m bytes");

    // upload staus labelsList
    ui->labUploadStatus->setText(tr("Ready to upload"));

    connect(ui->butClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->butUpload, SIGNAL(clicked(bool)), this, SLOT(slotUploadStart()));
    connect(ui->cbxUploaderList, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSeletHost(int)));
	
	ui->cbxUploaderList->setCurrentIndex(selectedHost);
}

DialogUploader::~DialogUploader()
{
    qDebug() << "delete dialog upload";

    if (uploader != 0)
    {
        delete uploader;
    }
	
	delete _uploaderWidget;
	
    delete ui;
}

void DialogUploader::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void DialogUploader::slotUploadStart()
{
    ui->progressBar->setVisible(true);
    ui->butUpload->setEnabled(false);
    ui->labUploadStatus->setText(tr("Upload preocessing... Please wait"));

    switch(selectedHost)
    {
    case 0:
        uploader = new Uploader_ImgUr;		
        break;
    case 1:
        uploader = new Uploader_ImgShack;
        break;
    default:
        uploader = new Uploader_ImgShack;
    }

    QVariantMap userSettings;
	QMetaObject::invokeMethod(_uploaderWidget, "settingsMap", Qt::DirectConnection, Q_RETURN_ARG(QVariantMap, userSettings));	
    uploader->getUserSettings(userSettings);;
    
    // start uploading process
    connect(uploader, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(slotUploadProgress(qint64,qint64)));
    uploader->startUploading();
    connect(uploader, SIGNAL(uploadDone())	, this, SLOT(slotUploadDone()));
    connect(uploader, SIGNAL(uploadFail(QByteArray)), this, SLOT(slotUploadFail(QByteArray)));
//     connect(ui->cbxExtCode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeExtCode(int)));
    connect(ui->butCopyLink, SIGNAL(clicked(bool)), this, SLOT(slotCopyLink()));
	connect(ui->butCopyExtCode, SIGNAL(clicked(bool)), this, SLOT(slotCopyLink()));
}

void DialogUploader::slotSeletHost(int type)
{
    selectedHost = type;
	
	if (_uploaderWidget)
	{		
		delete _uploaderWidget;		
	}
	
	switch(selectedHost)
	{
		case 0:
		{
			_uploaderWidget = new Uploader_ImgUr_Widget();
			break;
		}
		case 1:
		{
			_uploaderWidget = new Uploader_ImgShack_Widget();
			break;
		}
		default:
			_uploaderWidget = new Uploader_ImgUr_Widget();
	}
	
	ui->stackedWidget->addWidget(_uploaderWidget);
	ui->stackedWidget->setCurrentWidget(_uploaderWidget);
}

void DialogUploader::slotUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);

    if (bytesSent == bytesTotal)
    {
        ui->progressBar->setFormat(tr("Receiving a response from the server"));
    }
}

void DialogUploader::slotUploadDone()
{
	QList<ResultString_t> links = uploader->parsedLinksToGui();
	ui->editDirectLink->setText(links.first().first);
	
	for (int i =1; i < links.count(); ++i)
	{
		ui->cbxExtCode->addItem(links.at(i).second);
		_resultLinks << links.at(i).first;
	}
    
    ui->stackedWidget->setCurrentIndex(0);
    ui->labUploadStatus->setText(tr("Upload completed"));
    ui->progressBar->setVisible(false);
    ui->cbxUploaderList->setEnabled(false);

	UploaderConfig config;
	if (config.autoCopyResultLink() == true)
	{
		QApplication::clipboard()->setText(ui->editDirectLink->text());
	}
	
	connect(ui->cbxExtCode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeExtCode(int)));
	ui->cbxExtCode->setCurrentIndex(0);
	ui->editExtCode->setText(_resultLinks.at(0));
	ui->butClose->setText(tr("Close"));
}

void DialogUploader::slotUploadFail(const QByteArray& error)
{
	Q_UNUSED(error);
    QMessageBox msg(this);
    msg.setText(tr("Error uploading screenshot"));
    msg.setWindowTitle(tr("Error"));
    msg.setIcon(QMessageBox::Critical);
    msg.exec();

    ui->progressBar->setVisible(false);
    ui->labUploadStatus->setText(tr("Ready to upload"));
    ui->butUpload->setEnabled(true);
	ui->butClose->setText(tr("Close"));
}

void DialogUploader::slotChangeExtCode(int code)
{
	ui->editExtCode->setText(_resultLinks.at(code));
}

void DialogUploader::slotCopyLink()
{
	QString objName = sender()->objectName();
	QString copyText;
	
	if (objName == "butCopyLink")
	{
		copyText = ui->editDirectLink->text();
	}
	
	if (objName == "butCopyExtCode")
	{
		copyText = ui->editExtCode->text();
	}
	
	qApp->clipboard()->setText(copyText);
}

