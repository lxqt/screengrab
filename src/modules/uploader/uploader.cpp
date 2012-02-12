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

#include <core/core.h>

#include "uploader.h"
#include "uploaderdialog.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QRegExp>
#include <QtCore/QVector>
#include <QtCore/QUuid>
#include <QtCore/QStringList>
#include <QtNetwork/QNetworkRequest>

#include <QDebug>

Uploader::Uploader()
{
    strBoundary = "uploadbound";
    sizes << QSize(100,75) << QSize(150,112) << QSize(320,240) << QSize(640,480) << QSize(800,600) << QSize(1024,768) << QSize(1280,1024) << QSize(1600,1200);;
    selectedSize = -1;
    
    net = new QNetworkAccessManager(this);
    UploaderDialog *dlg = new UploaderDialog(this);
    dlg->show();
}

Uploader::~Uploader()
{
    qDebug() << "kill uploader";
}

void Uploader::setUsername(const QString& name)
{
    _username = name;
}

void Uploader::setPassword(const QString& pass)
{
    _password = pass;
}

void Uploader::useAccount(bool use)
{
    _useAccount = use;
}


void Uploader::uploadScreen()
{
    qDebug() << "upload screen slot";
    
    QByteArray data = createUploadData();
    QNetworkRequest request = createRequest(data);    

    connect(net, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    serverReply = net->post(request, data);
    connect(serverReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(replyProgress(qint64,qint64)));    
}

QByteArray Uploader::boundary(bool cleared)
{
    QByteArray retBoundary = strBoundary;
    
    if (cleared == false)
    {
        retBoundary.append("\r\n");
        retBoundary.prepend("--");
    }
    
    return retBoundary;
}

QByteArray Uploader::createUploadData()
{    
    QByteArray uploadData;
    
    Core *core = Core::instance();
    QString format = core->conf->getSaveFormat();
    QString tmpFileName = createFilename(format);
    core->writeScreen(tmpFileName, format , true);

    QByteArray screenData;
    screenData = core->getScreen();
            
    uploadData.append(boundary());
    uploadData.append("content-disposition: ");
    uploadData.append("form-data; name=\"public\"\r\n");
    uploadData.append("\r\n");
    uploadData.append("yes");
    uploadData.append("\r\n");
    
    qDebug() << "USERNAME " << _username;
    qDebug() << "PASSWORD " << _password;
    
    // resize image    
    if (selectedSize != -1)
    {
        QByteArray newSize = QByteArray::number(sizes[selectedSize].width()) + "x" + QByteArray::number(sizes[selectedSize].height());
        
        uploadData.append(boundary());
        uploadData.append("content-disposition: ");
        uploadData.append("form-data; name=\"optimage\"\r\n");
        uploadData.append("\r\n");
        uploadData.append("1");
        uploadData.append("\r\n");
        
        uploadData.append(boundary());
        uploadData.append("content-disposition: ");
        uploadData.append("form-data; name=\"optsize\"\r\n");
        uploadData.append("\r\n");
        uploadData.append(newSize);
        uploadData.append("\r\n");
    }
    
    if (_useAccount == true)
    {
        
        qDebug() << "use acc" << _useAccount;
        qDebug() << "use acc" << _username;
        qDebug() << "use acc" << _password;
        
        uploadData.append(boundary());
        uploadData.append("content-disposition: ");
        uploadData.append("form-data; name=\"a_username\"\r\n");
        uploadData.append("\r\n");
        uploadData.append(_username);
        uploadData.append("\r\n"); 
        
        uploadData.append(boundary());
        uploadData.append("content-disposition: ");
        uploadData.append("form-data; name=\"a_password\"\r\n");
        uploadData.append("\r\n");
        uploadData.append(_password);
        uploadData.append("\r\n");         
    }
    
    uploadData.append(boundary());
    uploadData.append("content-disposition: ");
    uploadData.append("form-data; name=\"key\"\r\n");
    uploadData.append("\r\n");
    uploadData.append("BXT1Z35V8f6ee0522939d8d7852dbe67b1eb9595");
    uploadData.append("\r\n");        
    
        //fileupload
    uploadData.append(boundary());
    uploadData.append("content-disposition: ");
    uploadData.append("form-data; name='fileupload'; ");
    uploadData.append("filename='" + tmpFileName + "'\r\n");
    
    if (format == "jpg")
    {
        uploadData.append("Content-Type: image/jpeg\r\n");
    }
    else
    {
        uploadData.append("Content-Type: image/" + format + "\r\n");
    }
    
    
    uploadData.append("\r\n");
    uploadData.append(screenData);
    uploadData.append("\r\n");

    uploadData.append(boundary());

    return uploadData;    
}

QNetworkRequest Uploader::createRequest(const QByteArray& requestData)
{
    QNetworkRequest request;
    
    request.setUrl(QUrl("http://imageshack.us/upload_api.php"));
    request.setRawHeader("Host", "imageshack.us");
    request.setRawHeader("Content-Type", "multipart/form-data; boundary=" + boundary(true));
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("User-Agent", "My User-Agent");
    request.setRawHeader("Content-Length", QByteArray::number(requestData.size()));
        
    return request;
}

void Uploader::replyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {   
        QByteArray replyXmalText = reply->readAll();
        
        // error parsing
        if (replyXmalText.contains("error id=") == true)
        {
            qDebug() << "error";
            
            // TODO -- emiting error signal (rerror type == errList(0))
                        
            QRegExp err("<error id=\"([^<]*)\"");
            int pos = err.indexIn(replyXmalText);
            int len = err.matchedLength();
;
            QByteArray errorStrCode = replyXmalText.mid(pos, len).replace("<error id=\"", "").replace("\"", "");
            
            Q_EMIT uploadFail(errorStrCode);
            return ;
        }
        
        QVector<QByteArray> listXmlNodes;
        QRegExp re;      
        QRegExp re2;
        
        //  creating list of element names
        listXmlNodes << "image_link" << "image_html" << "image_bb" << "image_bb2" << "thumb_html" << "thumb_bb" << "thumb_bb2";
        int inStart = 0;
        int outStart = 0;
        int len = 0;
        
        // parsing xml 
        for (int i = 0; i < listXmlNodes.count(); ++i)
        {
            // FIXME -- dirty hack for capture link text without right unversal regexp
            // set patterns  for full lenght item
            re.setPattern("<"+listXmlNodes[i]+">"); // open tag
            re2.setPattern("</"+listXmlNodes[i]+">"); //close tag
            
            // get start pos and lenght ite in xml
            inStart = re.indexIn(replyXmalText); // ops open tag start
            outStart = re2.indexIn(replyXmalText); // pos close tag start
            len = outStart - inStart + re2.matchedLength(); // length of full string
            
            // extract item and replase spec html sumbols
            QByteArray extractedText = replyXmalText.mid(inStart, len);
            extractedText = extractedText.replace("&quot;","'");
            extractedText = extractedText.replace("&lt;","<");
            extractedText = extractedText.replace("&gt;",">");
            extractedText = extractedText.replace("<"+listXmlNodes[i]+">","");
            extractedText = extractedText.replace("</"+listXmlNodes[i]+">","");
            
            listXmlNodes[i] = extractedText;
        }
        
        Q_EMIT uploadDone(listXmlNodes);
    }
    else
    {
        qDebug() << "reply error" ;
    }
    reply->deleteLater();
}

void Uploader::replyProgress(qint64 bytesSent, qint64 bytesTotal)
{
    Q_EMIT uploadProgress(bytesSent, bytesTotal);
}

QString Uploader::createFilename(QString& format)
{
    QString tmpFileName = QUuid::createUuid().toString();
    int size = tmpFileName.size() - 2;
    tmpFileName = tmpFileName.mid(1, size).left(8);
    
    tmpFileName = QDir::tempPath() + QDir::separator() + "screenshot-" + tmpFileName + "." + format;  
    
    return tmpFileName;
}

void Uploader::selectResizeMode(int mode)
{
    selectedSize = mode-1;
    qDebug() << "mode " << selectedSize;
}
