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

#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QPair>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QUrl>

typedef QPair<QByteArray, QString> ResultString_t;

const QByteArray UL_DIRECT_LINK = "direct_link";
const QByteArray UL_HTML_CODE = "html_code";
const QByteArray UL_BB_CODE = "bb_code";
const QByteArray UL_HTML_CODE_THUMB = "html_code_thumb";
const QByteArray UL_BB_CODE_THUMB = "bb_code_thumb";
const QByteArray UL_DELETE_URL = "delete_url";

class Uploader : public QObject
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent = 0);
    virtual ~Uploader();

    // overriding methods
    void getUserSettings(const QVariantMap& settings);
    virtual void startUploading();
    QMap<QByteArray, ResultString_t> parsedLinks();
    QList<ResultString_t> parsedLinksToGui();

Q_SIGNALS:
    void uploadStart();
    void uploadFail(const QByteArray &error);
    void uploadDoneStr(const QString &directLink);
    void uploadDone();
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

public Q_SLOTS:

protected Q_SLOTS:
    virtual void replyFinished(QNetworkReply* reply) {Q_UNUSED(reply)};
    void replyProgress(qint64 bytesSent, qint64 bytesTotal);

protected:
    // methods
    QByteArray boundary(bool cleared = false);
    QMap<QByteArray, QByteArray> parseResultStrings(const QVector<QByteArray>& keytags, const QByteArray& result);

    virtual QUrl apiUrl();
    virtual void createData(bool inBase64 = false);
    virtual void createRequest(const QByteArray& requestData, const QUrl url);

    // vars
    QByteArray imageData;
    QHttpMultiPart *_multipartData;
    QString _uploadFilename;
    QString _formatString;
    QByteArray _strBoundary;
    QMap<QByteArray, ResultString_t> _uploadedStrings;
    QVariantMap _userSettings;
    QNetworkAccessManager *_net;
    QNetworkRequest _request;
    QNetworkReply *_serverReply;

private:
    void initUploadedStrList();
};

#endif // UPLOADER_H
