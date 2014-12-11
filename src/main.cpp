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

#include "common/singleapp/singleapp.h"
#include "core/core.h"
#include "ui/mainwindow.h"

#ifdef SG_EXT_UPLOADS
// TODO for future (move call uploader from main() function to app core)
#include "modules/uploader/moduleuploader.h"
#endif

#include <QDebug>

int main(int argc, char *argv[])
{
    SingleApp scr(argc, argv, VERSION);
    scr.setApplicationVersion(VERSION);
    Core *ScreenGrab = Core::instance();
    ScreenGrab->modules()->initModules();
    ScreenGrab->processCmdLineOpts(scr.arguments());
    MainWindow mainWnd;

    if (!scr.isRunning() || (scr.isRunning() && ScreenGrab->conf->getAllowMultipleInstance()))
    {
        ScreenGrab->screenShot(true);

        if (ScreenGrab->runAsMinimized())
        {
            if (mainWnd.isTrayed())
                mainWnd.windowHideShow();
            else
                mainWnd.showMinimized();
        }
        else
            mainWnd.show();
    }

#ifdef SG_EXT_UPLOADS
// TODO for future (move call uploader from main() function to app core process cmdline opts)
    if (ScreenGrab->checkCmdLineOptions(QStringList() << "upload" << "u" ))
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

    return scr.exec();
}
