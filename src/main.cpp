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
//#include <QApplication>
#include <QTranslator>

#include "src/common/singleapp/singleapp.h"
#include "src/core/core.h"
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

    Core *ScreenGrab = Core::instance();
	
#ifdef SG_EXT_UPLOADS
		if (ScreenGrab->conf->cmdLine()->isCreated() && ScreenGrab->conf->cmdLine()->getParam("upload"))
		{
			qDebug() << "Upload ";			
		}
#endif
	
    MainWindow mainWnd;    
    
    if (scr.isRunning() == false || (scr.isRunning() == true && ScreenGrab->conf->getAllowMultipleInstance() == true))
    {
        ScreenGrab->screenShot(true);
		if (ScreenGrab->conf->cmdLine()->isCreated() && ScreenGrab->conf->cmdLine()->getParam("minimized"))
		{
			if (mainWnd.isTrayed() == true)
			{
				mainWnd.windowHideShow();
			}
			else
			{
				mainWnd.showMinimized();
			}
		}
		else
		{		
			mainWnd.show();
		}
//         mainWnd.show();
    }

    QObject::connect(&scr, SIGNAL(messageReceived(const QString&)), &mainWnd, SLOT(showWindow(const QString&) ) );

    if (!ScreenGrab->conf->getAllowMultipleInstance() && scr.isRunning())
    {
        QString type = QString::number(ScreenGrab->conf->getTypeScreen());
        scr.sendMessage("screengrab --type=" + type);
        return 0;
    }

    if (ScreenGrab->conf->cmdLine()->isCreated() &&
ScreenGrab->conf->cmdLine()->getParam("help"))
    {
        CmdLine::print("--help \t\tDisplay this screen");
        CmdLine::print("--version \tDisplay version info");
        CmdLine::print("--fullscreen \tSset fullscreen mode");
        CmdLine::print("--active \tSet active window mode");
        CmdLine::print("--region \tSet region select mode");
		CmdLine::print("--minimized \tRun minimised");
#ifdef SG_EXT_UPLOADS
		CmdLine::print("--upload \tUpload scrennshot to selected image host");
#endif
        return 0;
    }

    if (ScreenGrab->conf->cmdLine()->isCreated() &&
ScreenGrab->conf->cmdLine()->getParam("version"))
    {
        QString version = Core::getVersionPrintable();
        CmdLine::print(version);
        return 0;
    }
    
    return scr.exec();
}

