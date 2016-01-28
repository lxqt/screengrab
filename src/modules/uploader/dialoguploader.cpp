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

#include "dialoguploader.h"
#include "ui_dialoguploader.h"

#include "uploaderconfig.h"
#include "imgur/uploader_imgur.h"
#include "imgur/uploader_imgur_widget.h"
#include <core/core.h>

#include <QMessageBox>
#include <QDesktopServices>

#include <QDebug>

DialogUploader::DialogUploader(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::DialogUploader)
{
    _ui->setupUi(this);
    _ui->stackedWidget->setCurrentIndex(0);
    _uploader = 0;
    _uploaderWidget = 0;
    slotSeletHost(0);

    _ui->cbxUploaderList->addItems(UploaderConfig::labelsList());

    UploaderConfig config;
    QString defaultHost = config.loadSingleParam(QByteArray("common"), QByteArray(KEY_DEFAULT_HOST)).toString();

    if (defaultHost.isEmpty())
        _selectedHost = 0;
    else
    {
        _selectedHost = config.labelsList().indexOf(defaultHost);
        if (_selectedHost == -1)
            _selectedHost = 0;
    }

    // load ishot preview
    QSize imgSize = Core::instance()->getPixmap()->size();
    QString pixmapSize = tr("Size: ") + QString::number(imgSize.width()) + "x" + QString::number(imgSize.height()) + tr(" pixel");
    _ui->labImgSize->setText(pixmapSize);

    _ui->labImage->setFixedWidth(256);
    _ui->labImage->setFixedHeight(192);
    _ui->labImage->setPixmap(Core::instance()->getPixmap()->scaled(_ui->labImage->size(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // progressbar
    _ui->progressBar->setVisible(false);
    _ui->progressBar->setFormat(tr("Uploaded ") + "%p%" + " (" + "%v" + " of " + "%m bytes");

    // upload staus labelsList
    _ui->labUploadStatus->setText(tr("Ready to upload"));

    connect(_ui->butClose, &QPushButton::clicked, this, &DialogUploader::close);
    connect(_ui->butUpload, &QPushButton::clicked, this, &DialogUploader::slotUploadStart);

    void (QComboBox::*uploaderChnage)(int) = &QComboBox::currentIndexChanged;
    connect(_ui->cbxUploaderList, uploaderChnage, this, &DialogUploader::slotSeletHost);

    _ui->cbxUploaderList->setCurrentIndex(_selectedHost);
}

DialogUploader::~DialogUploader()
{
    qDebug() << "delete dialog upload";

    if (_uploader)
        delete _uploader;
    delete _uploaderWidget;
    delete _ui;
}

void DialogUploader::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void DialogUploader::slotUploadStart()
{
    _ui->progressBar->setVisible(true);
    _ui->butUpload->setEnabled(false);
    _ui->labUploadStatus->setText(tr("Upload processing... Please wait"));

    switch(_selectedHost)
    {
    case 0:
        _uploader = new Uploader_ImgUr;
        break;
    default:
        _uploader = new Uploader_ImgUr;
    }

    QVariantMap userSettings;
    _uploader->getUserSettings(userSettings);;

    _uploader->startUploading();

    connect(_uploader, &Uploader::uploadProgress, this, &DialogUploader::slotUploadProgress);

    void (Uploader::*uploadDone)() = &Uploader::uploadDone;
    connect(_uploader, uploadDone, this, &DialogUploader::slotUploadDone);
    connect(_uploader, &Uploader::uploadFail, this, &DialogUploader::slotUploadFail);
    connect(_ui->butCopyLink, &QPushButton::clicked, this, &DialogUploader::slotCopyLink);
    connect(_ui->butCopyExtCode, &QPushButton::clicked, this, &DialogUploader::slotCopyLink);
    connect(_ui->butOpenDirectLink, &QPushButton::clicked, this, &DialogUploader::slotOpenDirectLink);
    connect(_ui->butDeleteLink, &QPushButton::clicked, this, &DialogUploader::slotOpenDeleteLink);
}

void DialogUploader::slotSeletHost(int type)
{
    _selectedHost = type;

    if (_uploaderWidget)
        delete _uploaderWidget;

    switch(_selectedHost)
    {
        case 0:
            _uploaderWidget = new Uploader_ImgUr_Widget();
            break;
        default:
            _uploaderWidget = new Uploader_ImgUr_Widget();
    }

    _ui->stackedWidget->addWidget(_uploaderWidget);
    _ui->stackedWidget->setCurrentWidget(_uploaderWidget);
}

void DialogUploader::slotUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    _ui->progressBar->setMaximum(bytesTotal);
    _ui->progressBar->setValue(bytesSent);

    if (bytesSent == bytesTotal)
        _ui->progressBar->setFormat(tr("Receiving a response from the server"));
}

void DialogUploader::slotUploadDone()
{
qDebug() << "start dialog uploader done";
    QList<ResultString_t> links = _uploader->parsedLinksToGui();
    _ui->editDirectLink->setText(links.first().first);
    _ui->editDeleteLink->setText(links.last().first);

    for (int i =1; i < links.count()-1; ++i)
    {
        _ui->cbxExtCode->addItem(links.at(i).second);
        _resultLinks << links.at(i).first;
    }

    _ui->stackedWidget->setCurrentIndex(0);
    _ui->labUploadStatus->setText(tr("Upload completed"));
    _ui->progressBar->setVisible(false);
    _ui->cbxUploaderList->setEnabled(false);

    UploaderConfig config;
    if (config.autoCopyResultLink())
        QApplication::clipboard()->setText(_ui->editDirectLink->text());

    if (_resultLinks.count() > 0)
    {
        connect(_ui->cbxExtCode, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeExtCode(int)));
        _ui->cbxExtCode->setCurrentIndex(0);
        _ui->editExtCode->setText(_resultLinks.at(0));
    }
    else
    {
        _ui->editExtCode->setVisible(false);
        _ui->cbxExtCode->setVisible(false);
        _ui->butCopyExtCode->setVisible(false);
        _ui->labExtCode_2->setVisible(false);
    }

    _ui->butClose->setText(tr("Close"));
}

void DialogUploader::slotUploadFail(const QByteArray& error)
{
    Q_UNUSED(error);
    QMessageBox msg(this);
    msg.setText(tr("Error uploading screenshot"));
    msg.setWindowTitle(tr("Error"));
    msg.setIcon(QMessageBox::Critical);
    msg.exec();

    _ui->progressBar->setVisible(false);
    _ui->labUploadStatus->setText(tr("Ready to upload"));
    _ui->butUpload->setEnabled(true);
    _ui->butClose->setText(tr("Close"));
}

void DialogUploader::slotChangeExtCode(int code)
{
    _ui->editExtCode->setText(_resultLinks.at(code));
}

void DialogUploader::slotCopyLink()
{
    QString objName = sender()->objectName();
    QString copyText;

    if (objName == "butCopyLink")
        copyText = _ui->editDirectLink->text();

    if (objName == "butCopyExtCode")
        copyText = _ui->editExtCode->text();

    qApp->clipboard()->setText(copyText);
}

void DialogUploader::slotOpenDirectLink()
{
    QDesktopServices::openUrl(QUrl(_ui->editDirectLink->text()));
}

void DialogUploader::slotOpenDeleteLink()
{
    QMessageBox msg(this);
    msg.setText(tr("Open this link in your default web-browser, it may directly delete your uploaded image, without any warnings."));
    msg.setInformativeText(tr("Are you sure you want to continue?"));
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msg.setDefaultButton(QMessageBox::No);
    int result = msg.exec();

    if (result == QMessageBox::Yes)
        QDesktopServices::openUrl(QUrl(_ui->editDeleteLink->text()));
}
