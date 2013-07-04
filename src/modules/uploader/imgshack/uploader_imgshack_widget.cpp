/***************************************************************************
 *   Copyright (C) 2009 - 2013 by Artem 'DOOMer' Galichkin                        *
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

#include "uploader_imgshack_widget.h"
#include "ui_uploader_imgshack_widget.h"

#include <QDebug>

Uploader_ImgShack_Widget::Uploader_ImgShack_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uploader_ImgShack_Widget)
{
    ui->setupUi(this);
	
	QString warningTitle = tr("Warning!");
	QString warningText =  tr("Resize makes on servers imageshack.us");
	ui->labResizeWarning->setText("<font color='red'><b>" + warningTitle + "</b></font><br />" + warningText);	
}

Uploader_ImgShack_Widget::~Uploader_ImgShack_Widget()
{
    delete ui;
}

void Uploader_ImgShack_Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QVariantMap Uploader_ImgShack_Widget::settingsMap() const
{
	QVariantMap map;

	map.insert("resize", ui->cbxResize->currentIndex() - 1);
	map.insert("anonimous", ui->checkAnonimusUpload->isChecked());
	
	return map;
}
