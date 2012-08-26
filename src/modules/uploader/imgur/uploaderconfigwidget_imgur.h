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

#ifndef UPLOADERCONFIGWIDGET_IMGUR_H
#define UPLOADERCONFIGWIDGET_IMGUR_H

#include <QtGui/QWidget>

namespace Ui {
class UploaderConfigWidget_ImgUr;
}

class UploaderConfigWidget_ImgUr : public QWidget
{
    Q_OBJECT
    
public:
    explicit UploaderConfigWidget_ImgUr(QWidget *parent = 0);
    ~UploaderConfigWidget_ImgUr();
	
public Q_SLOTS:
	void saveSettings();
    
protected:
    void changeEvent(QEvent *e);
    
private:
    Ui::UploaderConfigWidget_ImgUr *ui;
};

#endif // UPLOADERCONFIGWIDGET_IMGUR_H
