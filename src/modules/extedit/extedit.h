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

#ifndef EXTEDIT_H
#define EXTEDIT_H

#include <QtCore/QObject>

struct ExtApp_t {
	QByteArray exec;
	QString name;
};

typedef QList<ExtApp_t> ExtAppsList_t;

class ExtEdit : public QObject
{
    Q_OBJECT
public:
    explicit ExtEdit(QObject *parent = 0);
    QStringList listAppNames();
    
public slots:
    
private:
	void createAppList();
	ExtApp_t readDesktopFile(QString filename);
	
	ExtAppsList_t _appList;		
	const QByteArray _globalAppListPath_c = "/usr/share/applications/";
};

#endif // EXTEDIT_H
