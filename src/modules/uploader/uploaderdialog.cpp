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

#include "uploaderdialog.h"
#include "ui_uploaderdialog.h"
#include "core/core.h"

#include <QDebug>

UploaderDialog::UploaderDialog(Uploader* uploader, QWidget* parent)
    :QDialog(parent), ui(new Ui::UploaderDialog), loader(uploader)
{
    ui->setupUi(this);
    ui->shotLabel->setPixmap(Core::instance()->getPixmap().scaled(ui->shotLabel->size(), 						      Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // set suze to tooltip for preview pixmap
    int width = Core::instance()->getPixmap().width();
    int height = Core::instance()->getPixmap().height();
    QString pixmapSize = QString::number(width) + "x" + QString::number(height) + tr(" pixel");
    ui->shotLabel->setToolTip(pixmapSize);

    connect(ui->butClose, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->butUpload, SIGNAL(clicked(bool)), loader, SLOT(uploadScreen()));

    qDebug() << "Core::instance()->getPixmap().width() " << Core::instance()->getPixmap().width();
}

UploaderDialog::~UploaderDialog()
{

}

void UploaderDialog::closeEvent(QCloseEvent* e)
{
    qDebug() << "loader ;" <<  loader;
    delete loader;

    QDialog::closeEvent(e);
}
