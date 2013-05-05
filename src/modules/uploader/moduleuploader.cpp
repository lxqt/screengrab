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

#include "moduleuploader.h"
#include "dialoguploader.h"
#include "uploaderconfigwidget.h"
#include "uploaderconfig.h"

#include <QDebug>

ModuleUploader::ModuleUploader()
{
    
}

void ModuleUploader::init()
{
    DialogUploader *ui = new DialogUploader();
    ui->exec();
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
	return act;
}
