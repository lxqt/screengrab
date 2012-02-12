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
#include <QtCore/QVector>
#include <QtCore/QSize>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

class Uploader : public QObject
{
    Q_OBJECT
public:
    Uploader();
    virtual ~Uploader();
    
    void setUsername(const QString& name);
    void setPassword(const QString& pass);
    void useAccount(bool use);

public Q_SLOTS:
    void uploadScreen();
    void selectResizeMode(int mode);
    
private Q_SLOTS:
    void replyFinished(QNetworkReply* reply);
    void replyProgress(qint64 bytesSent, qint64 bytesTotal);
    
Q_SIGNALS:
    void uploadStart();
    void uploadFail(const QByteArray &error);
    void uploadDone(const QVector<QByteArray>& resultStrings);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    
private:
    QByteArray boundary(bool cleared = false);
    QByteArray createUploadData();
    QNetworkRequest createRequest(const QByteArray& requestData);
    
    QByteArray imageData;
    QByteArray strBoundary;
    QNetworkAccessManager *net;
    QNetworkReply *serverReply;
    
    QString createFilename(QString& format);
    QVector<QSize> sizes;
    qint8 selectedSize;
    
    bool _useAccount;
    QString _username;
    QString _password;
};

#endif // UPLOADER_H
