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

#include "uploader_mediacrush.h"

#include <QStringList>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>

#include "uploaderconfig.h"

#include <QDebug>

Uploader_MediaCrush::Uploader_MediaCrush(const QString& format, QObject* parent): Uploader(parent)
{
    _host = "mediacru.sh";
    qDebug() << " create MediaCrush uploader";
    UpdateUploadedStrList();
    setCurrentFormat(format);
}

Uploader_MediaCrush::~Uploader_MediaCrush()
{
    qDebug() << " kill MediaCrush uploader";
}

/*!
 * Start upload process
 */
void Uploader_MediaCrush::startUploading()
{
    createData();
    createRequest(imageData, apiUrl());

   _request.setRawHeader("Host", _host);
    Uploader::startUploading();
}

/*!
 * Set type of uploading image, for generate right direct link on it.
 */
void Uploader_MediaCrush::setCurrentFormat(const QString& format)
{
    _currentFormat = format.toLatin1();
}

/*!
 * Return url for upload image
 */
QUrl Uploader_MediaCrush::apiUrl()
{
    UploaderConfig config;
    // QUrl("https://mediacru.sh/api/upload/file")

    return config.loadSingleParam("mediacru.sh", KEY_MCSH_URL).toUrl();
}

/*!
 * Prepare image data for uploading
 */
void Uploader_MediaCrush::createData(bool inBase64)
{
    Uploader::createData(inBase64);

    _multipartData = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;
    if (_formatString == "jpg")
    {
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
    }
    else
    {
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/" + _formatString));
    }
    QByteArray disposition = "form-data; name=\"file\"; filename='"+ _uploadFilename.toLatin1() +"'";
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(disposition));
    imagePart.setBody(imageData);

    _multipartData->append(imagePart);

    imageData.clear();
}

/*!
 * Process server reply data
 */
void Uploader_MediaCrush::replyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();

        if (response.split(':').count() >= 2)
        {
            response = response.split(':').at(1);
            response = response.mid(2, 12);
        }

        _uploadedStrings[UL_DIRECT_LINK].first = "https://" + _host + "/" + response + "." + _currentFormat;
        _uploadedStrings[UL_DELETE_URL].first = "https://" + _host + "/" + response + "/delete";

        Q_EMIT uploadDoneStr(_uploadedStrings[UL_DIRECT_LINK].first);
        Q_EMIT uploadDone();
    }
    else
    {
        Q_EMIT uploadFail(reply->errorString().toLatin1());
    }

    reply->deleteLater();
}

void Uploader_MediaCrush::UpdateUploadedStrList()
{
    QStringList nonUsed = QStringList() << UL_BB_CODE << UL_BB_CODE_THUMB << UL_HTML_CODE << UL_HTML_CODE_THUMB;

    for (int i =0; i < nonUsed.count(); ++i)
    {
        _uploadedStrings.remove(nonUsed.at(i).toLatin1());
    }
}
