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

#include "moduleextedit.h"

#include <QtCore/QObject>

ModuleExtEdit::ModuleExtEdit()
{
	_extEdit = new ExtEdit();
}

ModuleExtEdit::~ModuleExtEdit()
{
	if (_extEdit)
	{
		delete _extEdit;
	}
}

void ModuleExtEdit::init()
{
	
}


QMenu* ModuleExtEdit::initModuleMenu()
{
	QList<QAction*> list;
	
	QStringList appList = _extEdit->listAppNames();
	
	for (int i = 0; i < appList.count(); ++i)
	{
		QAction* action = new QAction(0);
		action->setText(appList.at(i));
		QObject::connect(action, SIGNAL(triggered(bool)), _extEdit, SLOT(runExternalEditor()));
		list.append(action);
		_extEdit->addAppAction(action);
	}
	
	QMenu *menu = new QMenu(QObject::tr("Edit in..."), 0);
	menu->addActions(list);
	return menu;
}

QWidget* ModuleExtEdit::initConfigWidget()
{
	return 0;
}

void ModuleExtEdit::defaultSettings()
{

}


QAction* ModuleExtEdit::initModuleAction()
{
	return 0;
}
