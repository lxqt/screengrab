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

#include "src/ui/mainwindow.h"
#include "ui_mainwindow.h"

#include "src/core/shortcutmanager.h"

#include <QtCore/QDir>
#include <QtGui/QFileDialog>
#include <QtGui/QDesktopWidget>
#include <QtCore/QHash>
#include <QtCore/QHashIterator>
#include <QtCore/QRegExp>
#include <QtCore/QTimer>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_X11

#include "src/common/netwm/netwm.h"
using namespace netwm;

#include <X11/Xlib.h>
#include <QtGui/QX11Info>
#endif

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow), core(Core::instance())
{
    m_ui->setupUi(this);
    trayed =false;

    // signal mapper
    globalShortcutSignals = new QSignalMapper(this);

    //     global shirtcuts
    fullScreen = new QxtGlobalShortcut(this);
    activeWindow = new QxtGlobalShortcut(this);
    areaSelection = new QxtGlobalShortcut(this);
    globalShortcuts << fullScreen << activeWindow << areaSelection;

        for (int i = 0; i < globalShortcuts.count(); ++i )
        {
	    connect(globalShortcuts[i], SIGNAL(activated()), globalShortcutSignals, SLOT(map()) );
    	globalShortcutSignals->setMapping(globalShortcuts[i], i);
        }
    /////connect(fullScreen, SIGNAL(activated()), this, SLOT(showAbout()));
    connect(globalShortcutSignals, SIGNAL(mapped(int)), this, SLOT(globalShortcutActivate(int)));

    trayIcon = NULL;
    updateUI();

//     on_delayBox_valueChanged(conf->getDelay());
    m_ui->delayBox->setValue(core->conf->getDelay());
    m_ui->cbxTypeScr->setCurrentIndex(core->conf->getTypeScreen());

//     connect buttons to slots
    connect(m_ui->butOpt, SIGNAL(clicked()), this, SLOT(showOptions()));
    connect(m_ui->butSave, SIGNAL(clicked()), this, SLOT(saveScreen()));
    connect(m_ui->butInfo, SIGNAL(clicked()), this, SLOT(showAbout()));
    connect(m_ui->butQuit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(m_ui->butNew, SIGNAL(clicked()), this, SLOT(newScreen()) );
    connect(m_ui->butCopy, SIGNAL(clicked()), this, SLOT(copyScreen()));
    connect(m_ui->butHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(m_ui->delayBox, SIGNAL(valueChanged(int)), this, SLOT(delayBoxChange(int)));
    connect(m_ui->cbxTypeScr, SIGNAL(activated(int)), this, SLOT(typeScreenShotChange(int)));

    connect(core, SIGNAL(newScreenShot(QPixmap*)), this, SLOT(restoreWindow()) );
    QIcon icon(":/res/img/logo.png");
    setWindowIcon(icon);

    resize(core->conf->getRestoredWndSize().width(), core->conf->getRestoredWndSize().height());

    move(QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber() ).width()/2 - width()/2,
         QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber()).height()/2 - height()/2);

//     core->screenShot(true);
    displayPixmap();

    createShortcuts();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (core->conf->getCloseInTray() == true && core->conf->getShowTrayIcon() == true)
    {
        windowHideShow();
        e->ignore();
    }
    else
    {
	quit();
    }
}

// resize main window
void MainWindow::resizeEvent(QResizeEvent *event)
{
    // get size dcreen pixel map
    QSize scaleSize = core->getPixmap().size(); // get orig size pixmap

    scaleSize.scale(m_ui->scrLabel->size(), Qt::KeepAspectRatio);

    // if not scrlabel pixmap
    if (!m_ui->scrLabel->pixmap() || scaleSize != m_ui->scrLabel->pixmap()->size())
    {
        displayPixmap();
    }

}

void MainWindow::show()
{
    qDebug() << "strt show";
    if (trayIcon != NULL)
    {
        trayIcon->setVisible(true);
    }
    QMainWindow::show();
    qDebug() << "end show";
}


void MainWindow::showHelp()
{
    // open help file
    QString localeHelpFile;

    #ifdef Q_WS_X11
    //    defaultHelpFile = QString(PREFIX)+"%1share%1doc%1screengrab%1html%1en%1index.html";
    localeHelpFile = QString(PREFIX)+"%1share%1doc%1screengrab%1html%1"+Config::getSysLang()+"%1index.html";
    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

    if (QFile::exists(localeHelpFile) != true)
    {
	localeHelpFile = QString(PREFIX)+"%1share%1doc%1screengrab%1html%1"+Config::getSysLang().section("_", 0, 0)  +"%1index.html";
	localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

	if (QFile::exists(localeHelpFile) != true)
	{
	    localeHelpFile = QString(PREFIX)+"%1share%1doc%1screengrab%1html%1en%1index.html";
	    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));
	}
    }
    #endif
    #ifdef Q_WS_WIN
    localeHelpFile = QApplication::applicationDirPath()+QString("%1docs%1html%1")+Config::getSysLang()+QString("%1index.html");
    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

    if (QFile::exists(localeHelpFile) != true)
    {
	localeHelpFile = QApplication::applicationDirPath()+QString("%1docs%1html%1")+Config::getSysLang().section("_", 0, 0)+QString("%1index.html");
	localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

	if (QFile::exists(localeHelpFile) != true)
	{
	    localeHelpFile = QApplication::applicationDirPath()+"%1docs%1html%1en%1index.html";
	    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));
	}
    }
    #endif

    // open find localize or eng help help
    QDesktopServices::openUrl(QUrl::fromLocalFile(localeHelpFile));
}


void MainWindow::showOptions()
{
    ConfigDialog *options;
    options = new ConfigDialog();
    globalShortcutBlock(true);

    if (isMinimized() == true)
    {
        showNormal();
        int result = options->exec();

        if (result == QDialog::Accepted)
        {
            updateUI();
        }
        hide();
    }
    else
    {
        int result = options->exec();

        if (result == QDialog::Accepted)
        {
            updateUI();
        }
    }

    globalShortcutBlock(false);
    delete options;
}

void MainWindow::showAbout()
{
    AboutDialog *about;
    about = new AboutDialog(this);

    // fi minimized main wnd
        if (isMinimized() == true)
        {
	    showNormal();
            about->exec();
            hide();
        }
        else
        {
	    about->exec();
        }

    delete about;
}


/*
// clicked on new screen
*/
void MainWindow::newScreen()
{
    setHidden(true);

    // if show trat
    if (core->conf->getShowTrayIcon() == true)
    {
	//  unblock tray signals
	trayIcon->blockSignals(true);
	trayIcon->setContextMenu(NULL); // enable context menu
    }

    if (core->conf->getDelay() == 0)
    {
	// if select 0s delay & hide window -- make 0.2s delay for hiding window
	QTimer::singleShot(200, core, SLOT(screenShot()));
    }
    else
    {
	QTimer::singleShot(1000*core->conf->getDelay(), core, SLOT(screenShot()));
    }
}

void MainWindow::copyScreen()
{
    core->copyScreen();
}

// crete tray
void MainWindow::createTray()
{    
    trayed = false;
    
    // create actions menu
    mQuit = new QAction(tr("Quit"), this);
    mSave = new QAction(tr("Save"), this);
    mNew = new QAction(tr("New"), this);
    mCopy = new QAction(tr("Copy"), this);
    mHideShow = new QAction(tr("Hide"), this);
    mAbout = new QAction(tr("About"), this);
    mOptions = new QAction(tr("Options"), this);
    mHelp = new QAction(tr("Help"), this);

    // connect to slots
    connect(mQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(mSave, SIGNAL(triggered()), this, SLOT(saveScreen()) );
    connect(mCopy, SIGNAL(triggered()), this, SLOT(copyScreen()));
    connect(mNew, SIGNAL(triggered()), this, SLOT(newScreen()));
    connect(mHideShow, SIGNAL(triggered()), this, SLOT(windowHideShow()));
    connect(mOptions, SIGNAL(triggered()), this, SLOT(showOptions()) );
    connect(mHelp, SIGNAL(triggered()), this, SLOT(showHelp()) );
    connect(mAbout, SIGNAL(triggered()), this, SLOT(showAbout()) );
    connect(core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)), this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    // create tray menu
    menuTray = new QMenu(this);
    menuTray->addAction(mHideShow);
    menuTray->addSeparator();
    menuTray->addAction(mNew); // TODO - add icons (icon, action)
    menuTray->addAction(mSave);
    menuTray->addAction(mCopy);
    menuTray->addSeparator();
    menuTray->addAction(mOptions);
    menuTray->addSeparator();
    menuTray->addAction(mHelp);
    menuTray->addAction(mAbout);
    menuTray->addSeparator();
    menuTray->addAction(mQuit);

    // icon menu
    QIcon icon(":/res/img/logo.png");

    trayIcon = new QSystemTrayIcon(this);
    
    trayIcon->setContextMenu(menuTray);
    trayIcon->setIcon(icon);
    trayIcon->show();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)) ,
             this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)) );    
}

void MainWindow::killTray()
{
    disconnect(core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)), this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    trayed = false;
    delete trayIcon;
    trayIcon = NULL;

    delete mHelp;
    delete mOptions;
    delete mAbout;
    delete mHideShow;
    delete mCopy;
    delete mNew;
    delete mSave;
    delete mQuit;
}

void MainWindow::delayBoxChange(int delay)
{
    if (delay == 0)
    {
        m_ui->delayBox->setSpecialValueText(tr("None"));
    }
    core->conf->setDelay(delay);
}

void MainWindow::typeScreenShotChange(int type)
{
    core->conf->setTypeScreen(type);
}


void MainWindow::receivedStateNotifyMessage(StateNotifyMessage state)
{
    trayShowMessage(state.header, state.message);
}

void MainWindow::quit()
{
    if (core->conf->getSavedSizeOnExit() == true)
    {
	core->conf->setRestoredWndSize(width(), height());
	core->conf->saveWndSize();
    }
    core->coreQuit();
}


// updating UI from configdata
void MainWindow::updateUI()
{
    // update delay spinbox
    m_ui->delayBox->setValue(core->conf->getDelay());

    // update shortcuts
    createShortcuts();
    // create tray object
    if (core->conf->getShowTrayIcon() == true && trayIcon == NULL)
    {
        createTray();
    }

    // kill tray object, if created
    if (core->conf->getShowTrayIcon() == false && trayIcon != NULL)
    {
        killTray();
    }
}

// obrabotka mouse clicks on tray icom
void MainWindow::trayClick(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            windowHideShow();
	    break;
        default: ;
    }
}

// hide or show main window
void MainWindow::windowHideShow()
{
    if (isHidden() == true)
    {
        mHideShow->setText(tr("Hide"));
        trayed = false;
        showNormal();
        activateWindow();
    }
    else if (isHidden() == false)
    {
        mHideShow->setText(tr("Show"));
        showMinimized();
        hide();
        trayed = true;
    }
}

void MainWindow::showWindow(const QString& str)
{
    // get char of type screen (last) form reviewd string
    QString typeNum = str[str.size() - 1];
    int type = typeNum.toInt();
    
    // change type scrren in config & on main window
    m_ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);
    
    core->sleep(250); // it hack for non capture  interface of apss
    core->screenShot();
    
    Q_UNUSED(str)
    if (isHidden() == true && core->conf->getShowTrayIcon() == true)
    {
        mHideShow->setText(tr("Hide"));
        trayed = false;
        showNormal();
    }

#ifdef Q_WS_X11
    netwm::init(); // initialize NETWM
    netwm::climsg(this->winId(), NET_ACTIVE_WINDOW, 2, QX11Info::appUserTime());
#endif
#ifdef Q_WS_WIN
	// TODO -- make normal activate window with Wim32API
	ShowWindow(this->winId(), SW_RESTORE);
	SetForegroundWindow(this->winId());
	this->activateWindow();
#endif
}

// show tray messages
void MainWindow::trayShowMessage(QString titleMsg, QString bodyMsg )
{
    if (core->conf->getShowTrayIcon() == true)
    {
        switch(core->conf->getTrayMessages())
        {
            case 0: break; // is never sjow
            case 1: // is hidden main wnd
            {
		if (isHidden() == true && trayed == true)
                {
                    trayIcon->showMessage(titleMsg, bodyMsg,
                    QSystemTrayIcon::MessageIcon(), core->conf->getTimeTrayMess()*1000 ); //5000
                }
                break;
            }
            case 2: // always show
            {
                trayIcon->showMessage(titleMsg, bodyMsg,
                QSystemTrayIcon::MessageIcon(), core->conf->getTimeTrayMess()*1000 );
                break;
            }
            default: break;
        }
    }
}


void MainWindow::displayPixmap()
{
    m_ui->scrLabel->setPixmap(core->getPixmap().scaled(m_ui->scrLabel->size(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::restoreWindow()
{
    displayPixmap();

#ifdef Q_WS_WIN
    // small hack for blocking segfault on shortcuted selection area screen on win32
    if (core->conf->getShowTrayIcon() == true && trayed == true && core->conf->getTypeScreen() == 2 )
    {
	hide();
    }
#endif

    if (isVisible() == false && trayed == false)
    {
	showNormal();
// 	setVisible(true);
    }

    // if show trat
    if (core->conf->getShowTrayIcon() == true)
    {
	//  unblock tray signals
	trayIcon->blockSignals(false);
	trayIcon->setContextMenu(menuTray); // enable context menu
    }
}


void MainWindow::saveScreen()
{
    // create initial filepath
    QHash<QString, QString> formatsAvalible;

    formatsAvalible["png"] = tr("PNG Files");
    formatsAvalible["jpg"] = tr("JPEG Files");
    formatsAvalible["bmp"] = tr("BMP Files");

    QString format = core->conf->getSaveFormat();
    QString filePath = core->getSaveFilePath(format);

    // create file filters
    QString fileFilters;

    QString filterSelected;
    filterSelected = formatsAvalible[format];

    QHash<QString, QString>::const_iterator iter = formatsAvalible.constBegin();

    while(iter != formatsAvalible.constEnd())
    {
        if (QString(qgetenv("DESKTOP_SESSION")).contains("kde") && iter.key() == format )
        {
            fileFilters.prepend(iter.value() + " (*." + iter.key() + ");;");
        }
        else
        {
            fileFilters.append(iter.value() + " (*." + iter.key() + ");;");
        }
	++iter;
    }
    fileFilters.chop(2);

QString fileName;
#ifdef Q_WS_X11
    if (qgetenv("DESKTOP_SESSION").contains("kde") || qgetenv("DESKTOP_SESSION") == "gnome")    
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected);
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected, QFileDialog::DontUseNativeDialog);
    }
#endif
#ifdef Q_WS_WIN
    fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected);
#endif

    QRegExp rx("\\(\\*\\.[a-z]{3,4}\\)");
    quint8 tmp = filterSelected.size() - rx.indexIn(filterSelected);

    filterSelected.chop(tmp + 1);
    format = formatsAvalible.key(filterSelected);

    // if user canceled saving
    if (fileName.isEmpty() == true)
    {
        return ;
    }

    core->writeScreen(fileName, format);
}


void MainWindow::createShortcuts()
{
    m_ui->butNew->setShortcut(core->conf->shortcuts()->getShortcut(Config::shortcutNew));
    m_ui->butSave->setShortcut(core->conf->shortcuts()->getShortcut(Config::shortcutSave));
    m_ui->butCopy->setShortcut(core->conf->shortcuts()->getShortcut(Config::shortcutCopy));
    m_ui->butOpt->setShortcut(core->conf->shortcuts()->getShortcut(Config::shortcutOptions));
    m_ui->butHelp->setShortcut(core->conf->shortcuts()->getShortcut(Config::shortcutHelp));

    for (int i = 0; i < globalShortcuts.count(); ++i )
    {
	globalShortcuts[i]->setShortcut(QKeySequence(core->conf->shortcuts()->getShortcut(i)));
    }
}

void MainWindow::globalShortcutBlock(bool state)
{
    for (int i = 0; i < globalShortcuts.count(); ++i )
    {
	globalShortcuts[i]->setDisabled(state);
    }
}


void MainWindow::globalShortcutActivate(int type)
{
    m_ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);
#ifdef Q_WS_WIN
    // small hack for blocking segfault on shortcuted selection area screen on win32
    if (core->conf->getShowTrayIcon() == true && trayed == true && type == 2 )
    {
        showMaximized();
    }
#endif

    if (trayed == false)
    {
        hide();
    }
    QTimer::singleShot(200, core, SLOT(screenShot()));

}

