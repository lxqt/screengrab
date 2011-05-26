/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef UPLOADERDIALOG_H
#define UPLOADERDIALOG_H

#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QtGui/QDialog>

#include "uploader.h"

class Uploader;

namespace Ui {
    class UploaderDialog;
}

class UploaderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UploaderDialog(Uploader *uploader, QWidget* parent = 0);
    ~UploaderDialog();

protected:
    void closeEvent ( QCloseEvent *e);

private:
    Ui::UploaderDialog *ui;
    Uploader *loader;
    QVector<QByteArray> extCodes;
    
    inline void copyLink(const QString& link);

public Q_SLOTS:
    void updateProgerssbar(qint64 bytesSent, qint64 bytesTotal);
    
private Q_SLOTS:
    void uploadStart();
    void uploadDone(const QVector<QByteArray>& resultStrings);
    void uploadFailed(const QByteArray& errorCode);
    
    void copyDirectLink();
    void copyExtCode();
    void changeExtCode(int code);
};

#endif // UPLOADERDIALOG_H
