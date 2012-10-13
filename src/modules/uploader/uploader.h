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

#ifndef UPLOADER_H
#define UPLOADER_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QUrl>

typedef QPair<QByteArray, QString> ResultString_t;

const QByteArray UL_DIRECT_LINK = "direct_link";
const QByteArray UL_HTML_CODE = "html_code";
const QByteArray UL_BB_CODE = "bb_code";
const QByteArray UL_HTML_CODE_THUMB = "html_code_thumb";
const QByteArray UL_BB_CODE_THUMB = "bb_code_thumb";

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
    
Q_SIGNALS:
    void uploadStart();
    void uploadFail(const QByteArray &error);
//     void uploadDone(const QVector<QByteArray>& resultStrings);
	void uploadDone();
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);	
    
public Q_SLOTS:
	
protected Q_SLOTS:
    virtual void replyFinished(QNetworkReply* reply) {Q_UNUSED(reply)};
	void replyProgress(qint64 bytesSent, qint64 bytesTotal);
	
protected:
    // methods
    QByteArray boundary(bool cleared = false);
    QString createFilename(QString& format);
	QMap<QByteArray, QByteArray> parseResultStrings(const QVector<QByteArray>& keytags, const QByteArray& result);
    
    virtual QUrl apiUrl();   	 
    virtual void createData(bool inBase64 = false); 
	virtual void createRequest(const QByteArray& requestData, const QUrl url);
    
    // vars
    QByteArray imageData;
    QString _uploadFilename;
    QString _formatString;
    QByteArray _strBoundary;
	QMap<QByteArray, ResultString_t> _uploadedStrings;
	QVariantMap _userSettings;
    QNetworkAccessManager *_net;
    QNetworkRequest _request;
    QNetworkReply *serverReply;	

private:
	void initUploadedStrList();
};

#endif // UPLOADER_H
