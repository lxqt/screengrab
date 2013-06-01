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

#ifndef MODULEUPLOADER_H
#define MODULEUPLOADER_H

#include "src/modules/abstractmodule.h"

#include <QtCore/QObject>
#include <QtGui/QWidget>

class ModuleUploader: public QObject, public AbstractModule
{
	Q_OBJECT
public:
    ModuleUploader(QObject *parent = 0);
	QString moduleName();
	QWidget* initConfigWidget();
	void defaultSettings();
	QMenu* initModuleMenu();	
	QAction* initModuleAction();
	
private Q_SLOTS:
	void init();
	
private:
	bool _ignoreCmdParam;
};

#endif // MODULEUPLOADER_H
