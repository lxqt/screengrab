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

#include "moduleuploader.h"
#include "dialoguploader.h"
#include "uploaderconfigwidget.h"
#include "uploaderconfig.h"
#include "imgshack/uploader_imgshack.h"
#include "imgur/uploader_imgur.h"

#include "src/core/core.h"
#include "src/core/cmdline.h"

#include <QApplication>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtGui/QClipboard>
#include <QDebug>

const QString UPLOAD_CMD_PARAM = "upload";

ModuleUploader::ModuleUploader(QObject *parent) :
	QObject(parent), _ignoreCmdParam(false)
{
    Core *core = Core::instance();
	core->cmdLine()->registerParam(UPLOAD_CMD_PARAM, "Automatically upload screenshot to default image hosting", CmdLineParam::Util);	
}

QString ModuleUploader::moduleName()
{
	return tr("Uploading");
}

void ModuleUploader::init()
{
	Core *core = Core::instance();
	if (core->cmdLine()->checkParam(UPLOAD_CMD_PARAM) == true  && _ignoreCmdParam == false)
	{
		// TODO - add implement shadow supload screenshot to selected host
		UploaderConfig config;
		QString selectedtHost = config.loadSingleParam(QByteArray("common"), KEY_DEFAULT_HOST.toAscii()).toString();
		
		Uploader *uploader = 0;
		switch(config.labelsList().indexOf(selectedtHost))
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
		
		connect(uploader, SIGNAL(uploadDone(QString)), this, SLOT(shadowUploadDone(QString)));
		connect(uploader, SIGNAL(uploadFail(QByteArray)), this, SLOT(shadowUploadFail(QByteArray)));
		uploader->startUploading();
		
		_ignoreCmdParam = true;
	}
	else
	{
		DialogUploader *ui = new DialogUploader();
		ui->exec();	
	}
}

QWidget* ModuleUploader::initConfigWidget()
{
	QWidget* configWidget =  new UploaderConfigWidget;
	return configWidget;
}

void ModuleUploader::defaultSettings()
{	
	UploaderConfig config;
	config.defaultSettings();
}

QMenu* ModuleUploader::initModuleMenu()
{
	return 0;
}

QAction* ModuleUploader::initModuleAction()
{
	QAction *act = new QAction(QObject::tr("Upload"), 0);
	act->setObjectName("actUpload");
	connect(act, SIGNAL(triggered(bool)), this, SLOT(init()));
	return act;
}


void ModuleUploader::shadowUploadDone(const QString& directLink)
{
	sender()->deleteLater();
	QString str = "Upload done, direct link to image: " + directLink;
	CmdLine::print(str);
	Q_EMIT uploadCompleteWithQuit();
}

void ModuleUploader::shadowUploadFail(const QByteArray& error)
{
	sender()->deleteLater();
	QString str = "Upload failed: " + error;
	CmdLine::print(str);
}
