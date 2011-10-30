/***************************************************************************
 *   Copyright (C) 2009 - 2011 by Artem 'DOOMer' Galichkin                        *
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

#include "uploaderdialog.h"
#include "ui_uploaderdialog.h"
#include "uploaderconfig.h"
#include "core/core.h"

#include <QtGui/QMessageBox>
#include <QDebug>

UploaderDialog::UploaderDialog(Uploader* uploader, QWidget* parent)
    :QDialog(parent), ui(new Ui::UploaderDialog), loader(uploader)
{
    ui->setupUi(this);
    
    ui->shotLabel->setFixedWidth(150);
    ui->shotLabel->setFixedHeight(128);

    ui->shotLabel->setPixmap(Core::instance()->getPixmap().scaled(ui->shotLabel->size(),                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // set suze to tooltip for preview pixmap
    int width = Core::instance()->getPixmap().width();
    int height = Core::instance()->getPixmap().height();
    QString pixmapSize = QString::number(width) + "x" + QString::number(height) + tr(" pixel");
    ui->shotLabel->setToolTip(pixmapSize);
	ui->butUpload->setFixedWidth(ui->shotLabel->geometry().width());
    
    QString warningTitle = tr("Warning!");
    QString warningText =  tr("Resize makes on servers imageshack.us");
    
    ui->labResizeWarning->setText("<font color='red'><b>" + warningTitle + "</b></font><br />" + warningText);

    connect(ui->butClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->butSettings, SIGNAL(clicked(bool)), this, SLOT(showSettings()));
    connect(ui->butUpload, SIGNAL(clicked(bool)), this, SLOT(uploadStart()));
    connect(ui->checkUseAccount, SIGNAL(toggled(bool)), this, SLOT(useAccount(bool)));
    
    connect(loader, SIGNAL(uploadDone(QVector<QByteArray>)), this, SLOT(uploadDone(QVector<QByteArray>)));
    connect(loader, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(updateProgerssbar(qint64,qint64)));
    connect(loader, SIGNAL(uploadFail(QByteArray)), this, SLOT(uploadFailed(QByteArray)));
    
    connect(ui->butCopyLink, SIGNAL(clicked(bool)), this, SLOT(copyDirectLink()));
    connect(ui->butCopyExtCode, SIGNAL(clicked(bool)), this, SLOT(copyExtCode()));
    connect(ui->cbxExtCode, SIGNAL(currentIndexChanged(int)), this, SLOT(changeExtCode(int)));
    connect(ui->cbxResize, SIGNAL(currentIndexChanged(int)), loader, SLOT(selectResizeMode(int)));
    
    qDebug() << "Core::instance()->getPixmap().width() " << Core::instance()->getPixmap().width();
    
    ui->progressBar->setFormat(tr("Uploaded ") + "%p%" + " (" + "%v" + " of " + "%m bytes");
    ui->progressBar->setVisible(false);
    ui->labStatus->setVisible(false);
    
    // load settings
    loadSettings();
    
    ui->stackedWidget->setCurrentIndex(0);
}

UploaderDialog::~UploaderDialog()
{

}

void UploaderDialog::loadSettings()
{
    UploaderConfig conf;
    
    QStringList settingsList = conf.loadSettings();
    ui->editUsername->setText(settingsList.at(0));
    ui->editPassword->setText(settingsList.at(1));
    
    bool copyLink = conf.loadparam("autoCopyDirectLink").toBool();
    qDebug() << "copylin " << copyLink;
    ui->checkCopyDirectLink->setChecked(copyLink);
    
    bool useAcc = conf.loadparam("useAccount").toBool();
    ui->checkUseAccount->setChecked(useAcc);
    loader->useAccount(useAcc);
    
    useAccount(useAcc); 
}


void UploaderDialog::closeEvent(QCloseEvent* e)
{
    if (ui->stackedWidget->currentIndex() == 2)
    {
        ui->stackedWidget->setCurrentIndex(0);
        ui->butClose->setText(tr("Close"));
        ui->butSettings->setVisible(true);
        ui->butUpload->setVisible(true);
        ui->butSettings->setText(tr("Settings"));

        // return original dialpog ttitle
        setWindowTitle(tr("Upload screenshot"));
        e->ignore();
    }
    else
    {
        qDebug() << "loader ;" <<  loader;
        
        delete loader;
        loader = 0;
        
        QDialog::closeEvent(e);   
    }
}

void UploaderDialog::updateProgerssbar(qint64 bytesSent, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesSent);
    
    if (bytesSent == bytesTotal)
    {
        qDebug() << "all is send!!!!";
        ui->progressBar->setFormat(tr("Upload completed!"));
        ui->labStatus->setText(tr("Receiving a response from the server"));
    }
}

void UploaderDialog::uploadStart()
{
    ui->butSettings->setVisible(false);
    ui->butClose->setEnabled(false);
//     ui->butUpload->setEnabled(false);
	ui->butUpload->setVisible(false);
    ui->cbxResize->setEnabled(false);
    ui->progressBar->setVisible(true);
    ui->labStatus->setVisible(true);
    ui->labStatus->setText(tr("Sending screenshot on the server"));
    
    loader->setUsername(ui->editUsername->text());
    loader->setPassword(ui->editPassword->text());
    
    ui->labUsername->setVisible(false);
    ui->labPassword->setVisible(false);
    ui->editUsername->setVisible(false);
    ui->editPassword->setVisible(false);
    ui->checkUseAccount->setVisible(false);

    loader->uploadScreen();
}

void UploaderDialog::uploadDone(const QVector< QByteArray >& resultStrings)
{
    ui->labSuccessfully->setText(tr("Screenshot uploaded successfully!"));
    ui->butClose->setEnabled(true);
    ui->butUpload->setVisible(false);
    
    ui->editDirectLink->setText(resultStrings.at(0));
    
    extCodes.resize(resultStrings.count() - 1);
    for (int i = 1; i < resultStrings.count(); i++)
    {
        extCodes[i-1] = resultStrings[i];        
    }
        
    ui->editExtCode->setText(extCodes[ui->cbxExtCode->currentIndex()]);

    // check autocopy direct link
    UploaderConfig conf;
    bool copyLink = conf.loadparam("autoCopyDirectLink").toBool();
    
    if (copyLink == true)
    {
        copyDirectLink();
    }
    
    ui->stackedWidget->setCurrentIndex(1);
}

void UploaderDialog::uploadFailed(const QByteArray& errorCode)
{
    qDebug() << "upload screenshot is failed - " << errorCode;
    
    // genetate messagebox text
    QString errorMessageText;
    QString errorMessageTitle = tr("Error uploading screenshot!");
    
    if (errorCode == "wrong_file_type" )
    {
        errorMessageText = tr("Server is not support this image type");
    }
    
    // show messagebox
    QMessageBox msg;
    msg.setWindowTitle(errorMessageTitle);
    msg.setText(errorMessageText);
    msg.setIcon(QMessageBox::Warning);
    msg.exec();
    
    close();
}


inline void UploaderDialog::copyLink(const QString& link)
{
    qApp->clipboard()->setText(link);
}

void UploaderDialog::copyDirectLink()
{
    copyLink(ui->editDirectLink->text());
}

void UploaderDialog::copyExtCode()
{
    copyLink(ui->editExtCode->text());
}


void UploaderDialog::changeExtCode(int code)
{
    ui->editExtCode->setText(extCodes[code]);
}

void UploaderDialog::showSettings()
{
    if (ui->stackedWidget->currentIndex() == 2)
    {
        //saveSettings
        UploaderConfig conf;
        
        QStringList settingsList;
        settingsList << ui->editUsername->text();
        settingsList << ui->editPassword->text();
        
        conf.saveSettings(settingsList);
        
        bool copyLink = ui->checkCopyDirectLink->isChecked();                
        conf.saveParameter("autoCopyDirectLink", copyLink);
        
        bool useAcc = ui->checkUseAccount->isChecked();
        conf.saveParameter("useAccount", useAcc);
        ui->butSettings->setText(tr("Settings"));
        setWindowTitle(tr("Upload screenshot"));
        ui->stackedWidget->setCurrentIndex(0);
        ui->butClose->setText(tr("Close"));
        ui->butUpload->setVisible(true);
        loader->useAccount(useAcc);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->butClose->setText(tr("Cancel"));
        ui->butSettings->setText(tr("Save"));
        ui->butUpload->setVisible(false);
        setWindowTitle(tr("Settings upload screenshot"));
        
        loadSettings(); 
    }
}


void UploaderDialog::useAccount(bool use)
{
    ui->labUsername->setVisible(use);
    ui->labPassword->setVisible(use);
    ui->editUsername->setVisible(use);
    ui->editPassword->setVisible(use);
//     loader->useAccount(use);
}
