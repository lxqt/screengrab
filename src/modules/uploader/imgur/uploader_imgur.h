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

#ifndef UPLOADER_IMGUR_H
#define UPLOADER_IMGUR_H

#include <uploader.h>

#include <QUrl>

namespace UploadImgUr {
    enum Error {
        ErrorFile,
        ErrorNetwork,
        ErrorCredits,
        ErrorUpload,
        ErrorCancel,
    };
};

class Uploader_ImgUr : public Uploader
{
    Q_OBJECT
public:
    explicit Uploader_ImgUr(QObject* parent = 0);
    virtual ~Uploader_ImgUr();

    virtual void startUploading();

protected:
    virtual QUrl apiUrl();
    virtual void createData(bool inBase64 = false);

protected Q_SLOTS:
    virtual void replyFinished(QNetworkReply* reply);
};

#endif // UPLOADER_IMGUR_H

