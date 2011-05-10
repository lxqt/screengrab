/***************************************************************************
 *   Copyright (C) 2010 by Artem 'DOOMer' Galichkin                        *
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

#include <core/core.h>

#include "uploader.h"
#include "uploaderdialog.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QDebug>

Uploader::Uploader()
{
    strBoundary = "uploadbound";
    UploaderDialog *dlg = new UploaderDialog(this);
    dlg->show();
    
    qDebug() << "create uploader";
    qDebug() << "Boundary is " << boundary();
//     Core::instance()->getPixmap().c
}

Uploader::~Uploader()
{
    qDebug() << "kill uploader";
}

void Uploader::uploadScreen()
{
    qDebug() << "upload screen slot";
    
    QByteArray data = createUploadData();
}

QByteArray& Uploader::boundary()
{
    QByteArray& retBoundary = strBoundary;
    
    return retBoundary;
}

QByteArray Uploader::createUploadData()
{
    qDebug() << __PRETTY_FUNCTION__ << " start";
    QByteArray uploadData;
    
    QString tmpFileName = QDir::tempPath() + QDir::separator() + "uplodscreen";   
    QTemporaryFile tempFile(tmpFileName);
    if (tempFile.open() == true)
    {
        QFileInfo info(tempFile);
        qDebug() << "tempfile is " << info.absoluteFilePath();
        
        // save screenshot totemp file
        Core *core = Core::instance();
        QString format = core->conf->getSaveFormat();
        tmpFileName = info.absoluteFilePath();
        core->writeScreen(tmpFileName, format, true);
        
        qDebug() << "tempfile size " << info.size();
    }
    
    
    return uploadData;
    qDebug() << __PRETTY_FUNCTION__ << " end";
}

QByteArray Uploader::createRequest()
{
    qDebug() << __PRETTY_FUNCTION__ << " start";
    QByteArray request;
    
    return request;
    qDebug() << __PRETTY_FUNCTION__ << " end";
}
