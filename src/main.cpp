/***************************************************************************
 *   Copyright (C) 2009 by Artem 'DOOMer' Galichkin                        *
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
//#include <QApplication>
#include <QTranslator>

#include "src/common/singleapp/singleapp.h"
#include <QDebug>
#include "src/core/screengrab.h"
#include "src/ui/mainwindow.h"

int main(int argc, char *argv[])
{
    SingleApp scr(argc, argv, VERSION);
    scr.setApplicationVersion(VERSION);

#ifdef Q_WS_WIN
    QStringList libPath;
    libPath << scr.applicationDirPath();
    scr.setLibraryPaths(libPath );
#endif

    QTranslator localize;

#ifdef Q_WS_X11
    QString localizeFile = PREFIX;
    localizeFile.append("/share/screengrab/localize/screengrab_"+Config::getSysLang()+".qm");
    localize.load(localizeFile);

#endif
#ifdef Q_WS_WIN // QLocale::system().name()
    localize.load(scr.applicationDirPath()+"/localize/screengrab_"+Config::getSysLang()+".qm");
#endif

    scr.installTranslator(&localize);

    // creating main window
//    MainWindow ScreenGrab;
    screengrab *ScreenGrab = screengrab::instance();
    MainWindow mainWnd;
    mainWnd.show();

    QObject::connect(&scr, SIGNAL(messageReceived(const QString&)), &mainWnd, SLOT(showWindow(const QString&) ) );

    if (!ScreenGrab->conf->getAllowMultipleInstance() && scr.isRunning())
    {
        scr.sendMessage("wake up");
        return 0;
    }

    if (ScreenGrab->conf->cmdLine()->isCreated() &&
ScreenGrab->conf->cmdLine()->getParam("help"))
    {
        CmdLine::print("--help \t\tDisplay this screen");
        CmdLine::print("--version \tDisplay version info");
        CmdLine::print("--fullscreen \tSset fullscreen mode");
        CmdLine::print("--active \tSet active window mode");
        CmdLine::print("--region \tSet region selet mode mode");
        return 0;
    }

    if (ScreenGrab->conf->cmdLine()->isCreated() &&
ScreenGrab->conf->cmdLine()->getParam("version"))
    {
        QString version = screengrab::getVersionPrintable();
        CmdLine::print(version);
        return 0;
    }

    return scr.exec();
}

