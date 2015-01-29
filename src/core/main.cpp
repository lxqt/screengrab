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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "singleapp.h"
#include "core/core.h"
#include "ui/mainwindow.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    SingleApp scr(argc, argv, VERSION);
    scr.setApplicationVersion(VERSION);
    Core *ScreenGrab = Core::instance();
    ScreenGrab->modules()->initModules();
    ScreenGrab->processCmdLineOpts(scr.arguments());

    QObject::connect(&scr, &SingleApp::messageReceived, ScreenGrab, &Core::initWindow);

    if (!ScreenGrab->config()->getAllowMultipleInstance() && scr.isRunning())
    {
        QString type = QString::number(ScreenGrab->config()->getTypeScreen());
        scr.sendMessage("screengrab --type=" + type);
        return 0;
    }

    return scr.exec();
}
