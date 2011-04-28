/***************************************************************************
 *   Copyright (C) 2009 - 2011 by Artem 'DOOMer' Galichkin                        *
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "src/core/core.h"
#include "src/ui/about.h"
#include "src/ui/configwidget.h"

#ifdef SG_GLOBAL_SHORTCUTS 
#include <QxtGui/QxtGlobalShortcut>
#endif

#include <QtGui/QMenu>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QDesktopServices>
#include <QtGui/QCloseEvent>
#include <QtCore/QUrl>
#include <QtCore/QSignalMapper>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow //, public screengrab
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void show();

public Q_SLOTS:
    void showWindow(const QString& str);
//
protected:
    void closeEvent(QCloseEvent *e);
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *event); // event resuze window    
//
private:
    Ui::MainWindow *m_ui;
    Core *core;

    QSystemTrayIcon *trayIcon;
    QAction *mQuit;
    QAction *mAbout;
    QAction *mNew;
    QAction *mSave;
    QAction *mCopy;
    QAction *mOptions;
    QAction *mHelp;
    QAction *mHideShow;
    QMenu *menuTray;

    bool trayed;

#ifdef SG_GLOBAL_SHORTCUTS
    QxtGlobalShortcut *fullScreen;
    QxtGlobalShortcut *activeWindow;
    QxtGlobalShortcut *areaSelection;

    QVector<QxtGlobalShortcut*> globalShortcuts;
    QSignalMapper *globalShortcutSignals;
#endif
    
    void createTray();
    void killTray();
    void trayShowMessage(QString titleMsg, QString bodyMsg );
    void createShortcuts();

 private Q_SLOTS:
    void displayPixmap();
    void restoreWindow();
    void newScreen();
    void copyScreen();
    void windowHideShow();
    void saveScreen();
    void showHelp();
    void showOptions();
    void showAbout();
    void delayBoxChange(int);
    void typeScreenShotChange(int type);
    void receivedStateNotifyMessage(StateNotifyMessage state);
    void quit();
    void updateUI();
    void trayClick(QSystemTrayIcon::ActivationReason reason);
    
#ifdef SG_GLOBAL_SHORTCUTS 
    void globalShortcutActivate(int type);
    void globalShortcutBlock(bool state);
#endif
};

#endif // MAINWINDOW_H
