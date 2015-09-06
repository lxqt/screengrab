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

#include "moduleextedit.h"

#include <QObject>

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

QString ModuleExtEdit::moduleName()
{
    return QObject::tr("External edit");
}


void ModuleExtEdit::init()
{

}

QMenu* ModuleExtEdit::initModuleMenu()
{
    QMenu *menu = new QMenu(QObject::tr("Edit in..."), 0);
    QList<XdgAction*> actionsList = _extEdit->getActions();

    foreach (XdgAction *appAction, actionsList)
    {
        menu->addAction(appAction);
        appAction->disconnect(SIGNAL(triggered()));
        QObject::connect(appAction, SIGNAL(triggered()), _extEdit, SLOT(runExternalEditor()));
    }

    menu->setObjectName("menuExtedit");
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
