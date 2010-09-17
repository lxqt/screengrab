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

#include "src/core/screengrab.h"
#include "src/core/shortcutmanager.h"

#include <QDebug>

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
    m_ui(new Ui::MainWindow), core(screengrab::instance())
{
    m_ui->setupUi(this);

    // connect  actions slotss

//     trayed = false;

    trayIcon = NULL;
    updateUI();
//     createTray();


//     on_delayBox_valueChanged(conf->getDelay());
    m_ui->delayBox->setValue(core->conf->getDelay());
    m_ui->cbxTypeScr->setCurrentIndex(core->conf->getTypeScreen());

//     signal mapper
//     globalShortcutSignals = new QSignalMapper(this);

//     global shirtcuts
//     fullScreen = new QxtGlobalShortcut(this);
//     activeWindow = new QxtGlobalShortcut(this);
//     areaSelection = new QxtGlobalShortcut(this);
//     globalShortcuts << fullScreen << activeWindow << areaSelection;

//     for (int i = 0; i < globalShortcuts.count(); ++i )
//     {
// 	connect(globalShortcuts[i], SIGNAL(activated()), globalShortcutSignals, SLOT(map()) );
// 	globalShortcutSignals->setMapping(globalShortcuts[i], i);
//     }
//     connect(fullScreen, SIGNAL(activated()), this, SLOT(showAbout()));
//     connect(globalShortcutSignals, SIGNAL(mapped(int)), this, SLOT(globalShortcutActivate(int)));

//     connect buttons to slots
    connect(m_ui->butOpt, SIGNAL(clicked()), this, SLOT(showOptions()));
    connect(m_ui->butSave, SIGNAL(clicked()), this, SLOT(saveScreen()));
    connect(m_ui->butInfo, SIGNAL(clicked()), this, SLOT(showAbout()));
    connect(m_ui->butQuit, SIGNAL(clicked()), core, SLOT(slotQuit()));
    connect(m_ui->butNew, SIGNAL(clicked()), this, SLOT(newScreen()) );
    connect(m_ui->butCopy, SIGNAL(clicked()), this, SLOT(copyScreen()));
    connect(m_ui->butHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(m_ui->delayBox, SIGNAL(valueChanged(int)), this, SLOT(delayBoxChange(int)));
    connect(m_ui->cbxTypeScr, SIGNAL(activated(int)), this, SLOT(typeScreenShotChange(int)));

    connect(core, SIGNAL(newScreenShot(QPixmap*)), this, SLOT(restoreWindow()) );
    QIcon icon(":/res/img/logo.png");
    setWindowIcon(icon);

//     resize(conf->getRestoredWndSize().width(), conf->getRestoredWndSize().height());

    move(QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber() ).width()/2 - width()/2,
         QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber()).height()/2 - height()/2);
//     createShortcuts();
//     show();
    displayPixmap();
    qDebug() << "creating wnd object";
}

MainWindow::~MainWindow()
{
    delete m_ui;
    qDebug() << "deleting wnd object";
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

/*-------------------------------------
void MainWindow::closeEvent(QCloseEvent *e)
{
    if (conf->getCloseInTray() == true && conf->getShowTrayIcon() == true)
    {
        windowHideShow();
        e->ignore();
    }
    else
    {
        //exit();
        screengrab->quit();)
    }

}
*/
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
/*
// close button clicked
void MainWindow::exit()
{
    if (conf->getSavedSizeOnExit() == true)
    {
        conf->setRestoredWndSize(width(), height());
        conf->saveWndSize();
    }
    qApp->quit();
}
*/

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
    configwidget *options;
    options = new configwidget();

    // FIXME uncomen fo a trayed mode
    // fi minimized main wnd
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
//     int result = options->exec();
//
//     if (result == QDialog::Accepted)
//     {
// 	updateUI();
//     }

    delete options;
}

void MainWindow::showAbout()
{
    aboutWidget *about;
    about = new aboutWidget(this);

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
    // FIXME add trayed mode
//     about->exec();

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

    //     screenShot();

    qDebug() << "delay " << core->conf->getDelay();;
    if (core->conf->getDelay() == 0)
    {
	// if select 0s delay & hide window -- make 0.25sdelay for hiding window
// 	QTimer::singleShot(200, core, SLOT(getScreen()) );
QTimer::singleShot(200, core, SLOT(screenShot()) );

    }
    else
    {
	qDebug() << "else";
// 	QTimer::singleShot(1000*core->conf->getDelay(), core, SLOT(getScreen()));
	QTimer::singleShot(1000*core->conf->getDelay(), core, SLOT(screenShot()));
    }
//     setVisible(true);
}

void MainWindow::copyScreen()
{
    core->copyScreen();
//     trayShowMessage(tr("Copied"),tr("Screenshot copied"));
}

// crete tray
void MainWindow::createTray()
{
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
    connect(mQuit, SIGNAL(triggered()), core, SLOT(slotQuit()) );
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
    qDebug() << " header " << state.header;
    qDebug() << " message " << state.message;
    trayShowMessage(state.header, state.message);
}


// updating UI from configdata
void MainWindow::updateUI()
{
    qDebug() << "update ui";
    // update delay spinbox
    m_ui->delayBox->setValue(core->conf->getDelay());

    // update shortcuts
//     createShortcuts();
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
//         trayed = false;
        showNormal();
        activateWindow();
    }
    else if (isHidden() == false)
    {
        mHideShow->setText(tr("Show"));
        showMinimized();
        hide();
//         trayed = true;
    }
}

/*
void MainWindow::showWindow(const QString& str)
{
    Q_UNUSED(str)
    qDebug() << "show window";
    if (isHidden() == true)
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
*/

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
                if (isHidden() == true )//&& trayed == true
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
    if (isVisible() == false)
    {
	showNormal();
// 	setVisible(true);
    }

    // autosave screenshot
    // move to private screenshot
    //core->autoSave();
//     if (core->conf->getAutoSave() == true)
//     {
	// qstring autosavew
// 	     trayShowMessage(tr("Saved"),tr("Saved")+ fileName + tr(" is saved"));
//     }
//     else
//     {
// 	trayShowMessage(tr("New screen"), tr("New screen is getting!"));
//     }

    // if show trat
    if (core->conf->getShowTrayIcon() == true)
    {
	//  unblock tray signals
	trayIcon->blockSignals(false);
	trayIcon->setContextMenu(menuTray); // enable context menu
    }
}


/*
void MainWindow::on_butNew_clicked()
{
    m_ui->butNew->setEnabled(false);

    // if show tray  is enbled
    if (conf->getShowTrayIcon() == true)
    {
        trayIcon->setContextMenu(0); // disable context menu
        trayIcon->blockSignals(true); // block tray signals
    }

    setHidden(true);

    if (conf->getDelay() == 0)
    {
        // if select 0s delay & hide window -- make 0.25sdelay for hiding window
        QTimer::singleShot(200, this, SLOT(newScreenUI()) );

    }
    else
    {
        QTimer::singleShot(1000*conf->getDelay(), this, SLOT(newScreenUI()) );

    }
}

// copy screen into clipboarad
void MainWindow::on_butCopy_clicked()
{
    copyScreen();
//     trayShowMessage(tr("Copied"),tr("Screenshot copied"));
}
*/
void MainWindow::saveScreen()
{
    qDebug() << "saved slot";
    // create initial filepath
    QHash<QString, QString> formatsAvalible;

    formatsAvalible["png"] = tr("PNG Files");
    formatsAvalible["jpg"] = tr("JPEG Files");
    formatsAvalible["bmp"] = tr("BMP Files");

    qDebug() << formatsAvalible;
        ;
    QString format = core->conf->getSaveFormat();
    QString filePath = core->getSaveFilePath(format);

    qDebug() << "file path == " << format;
    qDebug() << "file path == " << filePath;

    // create file filters
    QString fileFilters;
//     fileFilters = "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)";
    QString filterSelected;
    filterSelected = formatsAvalible[format];
    qDebug() << "filterSelected" << filterSelected;

    // TODO - kill em all =)
//     if (format == "png")
//     {
//         filterSelected = "PNG (*.png)";
//     }
//     if (format == "jpg")
//     {
//         filterSelected = "JPEG (*.jpg)";
//     }
//     if (format == "bmp")
//     {
//         filterSelected = "BMP (*.bmp)";
//     }

    QHash<QString, QString>::const_iterator iter = formatsAvalible.constBegin();

    while(iter != formatsAvalible.constEnd())
    {
	if (qgetenv("DESKTOP_SESSION") == "kde" && iter.key() == format )
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
    qDebug() << "fileFilters =  " << fileFilters;
    // if is KDE (fix bug in kde 4.4.x with native filedialog)
//     if (qgetenv("DESKTOP_SESSION") == "kde")
//     {
//         if (format == "png")
//         {
//             fileFilters = "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)";
//         }
//         if (format == "jpg")
//         {
//             fileFilters = "JPEG (*.jpg);;PNG (*.png);;BMP (*.bmp)";
//         }
//         if (format == "bmp")
//         {
//             fileFilters = "BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg)";
//         }
//     }

    // show file save dialog
QString fileName;
#ifdef Q_WS_X11
    if (qgetenv("DESKTOP_SESSION") == "kde" || qgetenv("DESKTOP_SESSION") == "gnome")
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

    // detecting selected format
//     if (filterSelected.toAscii() == "PNG (*.png)" )
//     {
//         format = "png";
//     }
//     if (filterSelected.toAscii() == "JPEG (*.jpg)")
//     {
//         format = "jpg";
//     }
//     if (filterSelected.toAscii() == "BMP (*.bmp)")
//     {
//         format = "bmp";
//     }

    // if user canceled saving
    if (fileName.isEmpty() == true)
    {
        return ;
    }

    if (core->writeScreen(fileName, format) == true)
    {
//         trayShowMessage(tr("Saved"),tr("Saved")+ fileName + tr(" is saved"));
	qDebug() << "saved " ;
    }
    else
    {
       qDebug() << "Error saving file";
    }
}
/*

void MainWindow::createShortcuts()
{
    m_ui->butNew->setShortcut(conf->shortcuts()->getShortcut(Config::shortcutNew));
    m_ui->butSave->setShortcut(conf->shortcuts()->getShortcut(Config::shortcutSave));
    m_ui->butCopy->setShortcut(conf->shortcuts()->getShortcut(Config::shortcutCopy));
    m_ui->butOpt->setShortcut(conf->shortcuts()->getShortcut(Config::shortcutOptions));
    m_ui->butHelp->setShortcut(conf->shortcuts()->getShortcut(Config::shortcutHelp));

    for (int i = 0; i < globalShortcuts.count(); ++i )
    {
	globalShortcuts[i]->setShortcut(QKeySequence(conf->shortcuts()->getShortcut(i)));
    }
}

// *********************************
// E X P E R I M E N T A L   C O D E
// *********************************
void MainWindow::globalShortcutActivate(int type)
{
    m_ui->cbxTypeScr->setCurrentIndex(type);
    on_cbxTypeScr_activated(type);
#ifdef Q_WS_X11
    if (trayed == true)
    {
	newScreenUI(); // work
	displayPixmap();
	showNormal();
	return;
    }
    else
    {
	on_butNew_clicked();
	displayPixmap();
	return;
    }
    if (isMinimized() == true && trayed != true)
    {
// 	FIXME -- non activate window if it minimized
// 	(KDE 4.5|qt 4.7.0)) not worked
// 	fluxbox is worked
// 	crash on GNOME 23.31.x
	on_butNew_clicked();

	activateWindow();
	return;
    }

#endif
#ifdef Q_WS_WIN
    on_butNew_clicked();
    activateWindow();
#endif
}

*/