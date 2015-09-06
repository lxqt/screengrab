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

#include "uploader_mediacrush_widget.h"
#include "ui_uploader_mediacrush_widget.h"

Uploader_MediaCrush_Widget::Uploader_MediaCrush_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Uploader_MediaCrush_Widget)
{
    ui->setupUi(this);
}

Uploader_MediaCrush_Widget::~Uploader_MediaCrush_Widget()
{
    delete ui;
}
