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

#include "singleapp.h"
#include "core/core.h"
#include "ui/mainwindow.h"

#include <signal.h>
#include <QDebug>

void handleQuitSignals (const std::vector<int>& quitSignals)
{ // a minimal signal handler
    auto handler = [](int sig)->void {
        Q_UNUSED(sig)
        QCoreApplication::quit();
    };
    for (int sig : quitSignals)
        signal (sig, handler);
}

int main(int argc, char *argv[])
{
    SingleApp scr(argc, argv, QString::fromUtf8(qgetenv("USER")) + QStringLiteral("-screengrab-") + QStringLiteral(VERSION));
    scr.setApplicationVersion(QStringLiteral(VERSION));
    scr.setOrganizationName(QStringLiteral("lxqt"));
    scr.setOrganizationDomain(QStringLiteral("lxqt-project.org"));
    scr.setApplicationName(QStringLiteral("screengrab"));
    scr.setDesktopFileName(QStringLiteral("screengrab"));

    handleQuitSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});

    Core *ScreenGrab = Core::instance();
    ScreenGrab->processCmdLineOpts(scr.arguments());
    // SingleApp should be initialized only after processing command-line options
    // because otherwise, if the help or version option is given, the app will
    // exit suddenly, without clearing the shared memory.
    scr.init();

    if (!ScreenGrab->config()->getAllowMultipleInstance() && scr.isRunning())
    {
        // Send a message to the current instance and exit.
        QString type = QString::number(ScreenGrab->config()->getDefScreenshotType());
        scr.sendMessage(QStringLiteral("screengrab --type=") + type);
        return 0;
    }

    // Initialize the modules as well as the window and listen to messages.
    ScreenGrab->modules()->initModules();
    ScreenGrab->initWindow();
    QObject::connect(&scr, &SingleApp::messageReceived, ScreenGrab, &Core::initWindow);

    return scr.exec();
}
