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

#include "uploader_imgshack.h"
#include "uploaderconfig.h"

#include <QtCore/QRegExp>
#include <QtCore/QVariantMap>

#include <QDebug>

Uploader_ImgShack::Uploader_ImgShack(QObject* parent): Uploader(parent)
{
    qDebug() << " create imageshack uploader";
	_sizes << QSize(100,75) << QSize(150,112) << QSize(320,240) << QSize(640,480) << QSize(800,600) << QSize(1024,768) << QSize(1280,1024) << QSize(1600,1200);
}

Uploader_ImgShack::~Uploader_ImgShack()
{
    qDebug() << " kill imageshack uploader";
}

/*!
 * 	Start upload process
 */
void Uploader_ImgShack::startUploading()
{
    createData();
	createRequest(imageData, apiUrl());

   _request.setRawHeader("Host", "imageshack.us");
   _request.setRawHeader("Content-Type", "multipart/form-data; boundary=" + boundary(true));
   _request.setRawHeader("Connection", "Keep-Alive");
   _request.setRawHeader("User-Agent", "My User-Agent");
   _request.setRawHeader("Content-Length", QByteArray::number(imageData.size()));
    
    Uploader::startUploading();
}

/*!
 * 	Return url for upload image
 */
QUrl Uploader_ImgShack::apiUrl()
{
    return QUrl("http://imageshack.us/upload_api.php");
}

/*!
 * 	Prepare image datafor uploading
 */
void Uploader_ImgShack::createData(bool inBase64)
{
    Uploader::createData(inBase64);

    // create data for upload 
    QByteArray uploadData;
    uploadData.append(boundary());
    uploadData.append("content-disposition: ");
    uploadData.append("form-data; name=\"public\"\r\n");
    uploadData.append("\r\n");
    uploadData.append("yes");
    uploadData.append("\r\n");

	// resize image    
	qint8 selectedSize = _userSettings["resize"].toInt();
	
	if (selectedSize != -1)
	{
		QByteArray newSize = QByteArray::number(_sizes[selectedSize].width()) + "x" + QByteArray::number(_sizes[selectedSize].height());
			
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

	// use accounts
	if (_userSettings["anonimous"].toBool() == false)
	{
		UploaderConfig config;
		QVariantMap configParams;
		configParams.insert(KEY_IMGSHK_USER, "");
		configParams.insert(KEY_IMGSHK_PASS, "");
		configParams = config.loadSettings("imageshack.us", configParams);
		
		uploadData.append(boundary());
		uploadData.append("content-disposition: ");
		uploadData.append("form-data; name=\"a_username\"\r\n");
		uploadData.append("\r\n");
		uploadData.append(configParams[KEY_IMGSHK_USER].toString());
		uploadData.append("\r\n"); 
		
		uploadData.append(boundary());
		uploadData.append("content-disposition: ");
		uploadData.append("form-data; name=\"a_password\"\r\n");
		uploadData.append("\r\n");
		uploadData.append(configParams[KEY_IMGSHK_PASS].toString());
		uploadData.append("\r\n");     		
	}

    // key field
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
    uploadData.append("filename='" + _uploadFilename + "'\r\n");
    
    if (_formatString == "jpg")
    {
        uploadData.append("Content-Type: image/jpeg\r\n");
    }
    else
    {
        uploadData.append("Content-Type: image/" + _formatString + "\r\n");
    }
    
    uploadData.append("\r\n");
    uploadData.append(imageData);
    uploadData.append("\r\n");
    
    uploadData.append(boundary());
    
    imageData = uploadData;
}

/*!
 * 	Process server reply data
 */
void Uploader_ImgShack::replyFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray replyXmalText = reply->readAll();

        // error parsing
        if (replyXmalText.contains("error id=") == true)
        {                      
            QRegExp err("<error id=\"([^<]*)\"");
            int pos = err.indexIn(replyXmalText);
            int len = err.matchedLength();
;
            QByteArray errorStrCode = replyXmalText.mid(pos, len).replace("<error id=\"", "").replace("\"", "");
            
            Q_EMIT uploadFail(errorStrCode);
            return ;
        }
		
        //  creating list of element names
		QVector<QByteArray> listXmlNodes;
        listXmlNodes << "image_link" << "image_html" << "image_bb2" << "thumb_html" << "thumb_bb2";
        
		QMap<QByteArray, QByteArray> replyXmlMap = parseResultStrings(listXmlNodes, replyXmalText);

		_uploadedStrings[UL_DIRECT_LINK].first = replyXmlMap["image_link"];
		_uploadedStrings[UL_HTML_CODE].first = replyXmlMap["image_html"];
		_uploadedStrings[UL_BB_CODE].first = replyXmlMap["image_bb2"];
		_uploadedStrings[UL_HTML_CODE_THUMB].first = replyXmlMap["thumb_htm"];
		_uploadedStrings[UL_BB_CODE_THUMB].first = replyXmlMap["thumb_bb2"];
		qDebug() << "done " << _uploadedStrings[UL_DIRECT_LINK].first;
		Q_EMIT uploadDone(_uploadedStrings[UL_DIRECT_LINK].first);
		Q_EMIT uploadDone();
	}
	else
	{
		Q_EMIT uploadFail(reply->errorString().toAscii());
	}
	
	reply->deleteLater();
}
