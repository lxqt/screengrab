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

#include "moduleuploader.h"
#include "dialoguploader.h"
#include "uploaderconfigwidget.h"
#include "uploaderconfig.h"
#include "imgur/uploader_imgur.h"

#include "core/core.h"

#include <QApplication>
#include <QWaitCondition>
#include <QMutex>
#include <QClipboard>

#include <QDebug>

const QString UPLOAD_CMD_PARAM = "upload";
const QString UPLOAD_CMD_PARAM_SHORT = "u";

ModuleUploader::ModuleUploader(QObject *parent) :
    QObject(parent), _ignoreCmdParam(false),
    _optUpload(QStringList() << UPLOAD_CMD_PARAM_SHORT << UPLOAD_CMD_PARAM)
{
    QString optUploadDescr = tr("Upload the screenshot to the default image host");
    _optUpload.setDescription(optUploadDescr);
    Core::instance()->addCmdLineOption(_optUpload);
}

QString ModuleUploader::moduleName()
{
    return tr("Uploading");
}

void ModuleUploader::init()
{
    Core *core = Core::instance();

    if (core->checkCmdLineOption(_optUpload) == true  && _ignoreCmdParam == false)
    {
        UploaderConfig config;
        QString selectedtHost = config.loadSingleParam(QByteArray("common"), QByteArray(KEY_DEFAULT_HOST)).toString();

        Uploader *uploader = 0;
        switch(config.labelsList().indexOf(selectedtHost))
        {
        case 0:
            uploader = new Uploader_ImgUr;
            break;
        default:
            uploader = new Uploader_ImgUr;
        }

        connect(uploader, &Uploader::uploadDoneStr, this, &ModuleUploader::shadowUploadDone);
        connect(uploader, &Uploader::uploadFail, this, &ModuleUploader::shadowUploadFail);

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
    connect(act, &QAction::triggered, this, &ModuleUploader::init);
    return act;
}


void ModuleUploader::shadowUploadDone(const QString& directLink)
{
    sender()->deleteLater();
    QString str = "Upload done, direct link to image: " + directLink;
    qWarning() << str;

    Q_EMIT uploadCompleteWithQuit();
}

void ModuleUploader::shadowUploadFail(const QByteArray& error)
{
    sender()->deleteLater();
    QString str = "Upload failed: " + error;
    qWarning() << str;

    Q_EMIT uploadCompleteWithQuit();
}
