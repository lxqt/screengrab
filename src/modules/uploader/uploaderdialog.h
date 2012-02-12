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
    
    void loadSettings();
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
    void showSettings();
    
    void useAccount(bool use);
};

#endif // UPLOADERDIALOG_H
