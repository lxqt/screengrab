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

#ifndef UPLOADER_IMGUR_WIDGET_H
#define UPLOADER_IMGUR_WIDGET_H

#include <QWidget>
#include <QVariant>

namespace Ui {
class Uploader_ImgUr_Widget;
}

class Uploader_ImgUr_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Uploader_ImgUr_Widget(QWidget *parent = 0);
    ~Uploader_ImgUr_Widget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Uploader_ImgUr_Widget *_ui;
};

#endif // UPLOADER_IMGUR_WIDGET_H
