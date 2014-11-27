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
#include <QTranslator>

#include "common/singleapp/singleapp.h"
#include "core/core.h"
#include "ui/mainwindow.h"

#ifdef SG_EXT_UPLOADS
// FIXME for v1.1 (move call uploader form main() function to app core)
#include "src/modules/uploader/moduleuploader.h"
#endif

int main(int argc, char *argv[])
{
    SingleApp scr(argc, argv, VERSION);
    scr.setApplicationVersion(VERSION);

    QTranslator localize;

    QString localizeFile = PREFIX;
    localizeFile.append("/share/screengrab/translations/screengrab_"+Config::getSysLang()+".qm");
    localize.load(localizeFile);

    scr.installTranslator(&localize);

    Core *ScreenGrab = Core::instance();
    ScreenGrab->modules()->initModules();
    ScreenGrab->parseCmdLine();

    MainWindow mainWnd;

    if (scr.isRunning() == false || (scr.isRunning() == true && ScreenGrab->conf->getAllowMultipleInstance() == true))
    {
        ScreenGrab->screenShot(true);

        if ( ScreenGrab->cmdLine()->checkParam("minimized"))
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
    }

#ifdef SG_EXT_UPLOADS
// FIXME for v1.1 (move call uploader form main() function to app core)
    if (ScreenGrab->cmdLine()->checkParam("upload"))
    {
        mainWnd.hide();

        ModuleUploader *uploader = static_cast<ModuleUploader*>(ScreenGrab->modules()->getModule(MOD_UPLOADER));
        QObject::connect(uploader, SIGNAL(uploadCompleteWithQuit()), &scr, SLOT(quit()));
        uploader->init();
    }
#endif

    QObject::connect(&scr, SIGNAL(messageReceived(const QString&)), &mainWnd, SLOT(showWindow(const QString&) ) );

    if (!ScreenGrab->conf->getAllowMultipleInstance() && scr.isRunning())
    {
        QString type = QString::number(ScreenGrab->conf->getTypeScreen());
        scr.sendMessage("screengrab --type=" + type);
        return 0;
    }

    if (ScreenGrab->cmdLine()->checkParam("help"))
    {
        ScreenGrab->cmdLine()->printHelp();
        return 0;
    }

    if (ScreenGrab->cmdLine()->checkParam("version"))
    {
        QString version = Core::getVersionPrintable();
        CmdLine::print(version);
        return 0;
    }

    return scr.exec();
}
