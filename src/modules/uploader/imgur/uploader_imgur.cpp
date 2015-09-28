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

#include "uploader_imgur.h"

#include <QDebug>

Uploader_ImgUr::Uploader_ImgUr(QObject* parent): Uploader(parent)
{
    qDebug() << " create Imgur uploader";
}

Uploader_ImgUr::~Uploader_ImgUr()
{
    qDebug() << " kill Imgur uploader";
}

/*!
 * Start upload process
 */
void Uploader_ImgUr::startUploading()
{
    createData();
    createRequest(imageData, apiUrl());
    _request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    Uploader::startUploading();
}

/*!
 * Return url for upload image
 */
QUrl Uploader_ImgUr::apiUrl()
{
    return QUrl("https://api.imgur.com/2/upload");
}

/*!
 * Prepare image data for uploading
 */
void Uploader_ImgUr::createData(bool inBase64)
{
    inBase64 = true;
    Uploader::createData(inBase64);

    // create data for upload
    QByteArray uploadData;

    uploadData.append(QString("key=").toUtf8());
    uploadData.append(QUrl::toPercentEncoding("6920a141451d125b3e1357ce0e432409"));
    uploadData.append(QString("&image=").toUtf8());
    uploadData.append(QUrl::toPercentEncoding(this->imageData));

    this->imageData = uploadData;
}

/*!
 * Process server reply data
 */
void Uploader_ImgUr::replyFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray replyXmalText = reply->readAll();

        // creating list of element names
        QVector<QByteArray> listXmlNodes;
        listXmlNodes << "original" << "imgur_page" << "large_thumbnail" << "small_square" << "delete_page";

        QMap<QByteArray, QByteArray> replyXmlMap = parseResultStrings(listXmlNodes, replyXmalText);

        _uploadedStrings[UL_DIRECT_LINK].first = replyXmlMap["original"];
        _uploadedStrings[UL_HTML_CODE].first = "<img src=\"" + replyXmlMap["original"] + "\" />";
        _uploadedStrings[UL_BB_CODE].first = "[img]" + replyXmlMap["original"] +"[/img]";
        _uploadedStrings[UL_HTML_CODE_THUMB].first = "<a href=\"" + replyXmlMap["original"] + "\"><img src=\"" + replyXmlMap["small_square"] + "\" /></a>";
        _uploadedStrings[UL_BB_CODE_THUMB].first = "[url=" + replyXmlMap["original"] + "][img]"+ replyXmlMap["small_square"] +"[/img][/url]";
        _uploadedStrings[UL_DELETE_URL].first = replyXmlMap["delete_page"];
        qDebug() << "done" << _uploadedStrings[UL_DIRECT_LINK].first;
        Q_EMIT uploadDoneStr(_uploadedStrings[UL_DIRECT_LINK].first);
        Q_EMIT uploadDone();
    }
    else
    {
        Q_EMIT uploadFail(reply->errorString().toLatin1());
    }

    reply->deleteLater();
}
