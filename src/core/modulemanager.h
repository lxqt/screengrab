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

#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "src/modules/abstractmodule.h"

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtGui/QMenu>

#include <QtGui/QAction>

const QByteArray MOD_UPLOADER = "uploader";
const QByteArray MOD_EXT_EDIT = "extedit";

typedef QMap<QByteArray, AbstractModule*> ModuleList_t;

class ModuleManager
{
public:
    ModuleManager();
	void initModules();
	AbstractModule* getModule(const QByteArray& name);
	AbstractModule* getModule(const quint8 numid);
	QList<QMenu*> generateModulesMenus(QStringList modules = QStringList());
	QList<QAction*> generateModulesActions(QStringList modules = QStringList());
	quint8 count();
	
private:
	ModuleList_t *_modules;
};

#endif // MODULEMANAGER_H
