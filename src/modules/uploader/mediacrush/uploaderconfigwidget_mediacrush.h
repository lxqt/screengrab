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

#ifndef UPLOADERCONFIGWIDGET_MEDIACRUSH_H
#define UPLOADERCONFIGWIDGET_MEDIACRUSH_H

#include <QWidget>

namespace Ui {
class UploaderConfigWidget_MediaCrush;
}

class UploaderConfigWidget_MediaCrush : public QWidget
{
    Q_OBJECT

public:
    explicit UploaderConfigWidget_MediaCrush(QWidget *parent = 0);
    ~UploaderConfigWidget_MediaCrush();

public Q_SLOTS:
    void saveSettings();

private:
    Ui::UploaderConfigWidget_MediaCrush *ui;
};

#endif // UPLOADERCONFIGWIDGET_MEDIACRUSH_H
