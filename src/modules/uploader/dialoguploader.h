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

#ifndef DIALOGUPLOADER_H
#define DIALOGUPLOADER_H

#include "uploader.h"

#include <QDialog>

namespace Ui {
class DialogUploader;
}

class DialogUploader : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUploader(QWidget *parent = 0);
    ~DialogUploader();

protected:
    void changeEvent(QEvent *e);

private Q_SLOTS:
    void slotUploadStart();
    void slotSeletHost(int type);
    void slotUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void slotUploadDone();
    void slotUploadFail(const QByteArray &error);
    void slotChangeExtCode(int code);
    void slotCopyLink();
    void slotOpenDirectLink();
    void slotOpenDeleteLink();

private:
    Ui::DialogUploader *_ui;

    Uploader* _uploader;
    QWidget* _uploaderWidget;

    // storage id curren selected img sho
    qint8 _selectedHost;

    QStringList _resultLinks;
};

#endif // DIALOGUPLOADER_H
