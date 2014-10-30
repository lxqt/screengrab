/***************************************************************************
 *   Copyright (C) 2009 - 2013 by Artem 'DOOMer' Galichkin                        *
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

#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QHash>
#include <QHashIterator>
#include <QRegExp>
#include <QTimer>

#include "src/common/netwm/netwm.h"
using namespace netwm;

#include <X11/Xlib.h>
#include <QX11Info>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow), _core(Core::instance())
{
    _ui->setupUi(this);
    _trayed =false;

#ifdef SG_GLOBAL_SHORTCUTS
    // signal mapper
    _globalShortcutSignals = new QSignalMapper(this);

    //     global shirtcuts
    _fullScreen = new QxtGlobalShortcut(this);
    _activeWindow = new QxtGlobalShortcut(this);
    _areaSelection = new QxtGlobalShortcut(this);
    _globalShortcuts << _fullScreen << _activeWindow << _areaSelection;

        for (int i = 0; i < _globalShortcuts.count(); ++i )
        {
            connect(_globalShortcuts[i], SIGNAL(activated()), _globalShortcutSignals, SLOT(map()) );
            _globalShortcutSignals->setMapping(_globalShortcuts[i], i);
        }

    connect(_globalShortcutSignals, SIGNAL(mapped(int)), this, SLOT(globalShortcutActivate(int)));
#endif

    _trayIcon = NULL;
	_hideWnd = NULL;
    updateUI();

    delayBoxChange(_core->conf->getDelay());
    _ui->cbxTypeScr->setCurrentIndex(_core->conf->getTypeScreen());

//     connect buttons to slots
    connect(_ui->butOpt, SIGNAL(clicked()), this, SLOT(showOptions()));
    connect(_ui->butSave, SIGNAL(clicked()), this, SLOT(saveScreen()));
    connect(_ui->butQuit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(_ui->butNew, SIGNAL(clicked()), this, SLOT(newScreen()) );
    connect(_ui->butCopy, SIGNAL(clicked()), this, SLOT(copyScreen()));

	// Create advanced menu
	QList<QAction*> modulesActions = _core->modules()->generateModulesActions();
	int  insIndex = _ui->layotButtons->indexOf(_ui->butCopy) + 1;

	if (modulesActions.count() > 0)
	{
		for (int i = 0; i < modulesActions.count(); ++i)
		{
			if (modulesActions.at(i) != 0)
			{
				QString objName = "but" + modulesActions.at(i)->objectName().remove(0, 3);
				QString text = modulesActions.at(i)->text();
				QPushButton* btn = createButton(objName, text);
				btn->addAction(modulesActions.at(i));
				connect(btn, SIGNAL(clicked(bool)), modulesActions.at(i), SIGNAL(triggered(bool)));
				_ui->layotButtons->insertWidget(insIndex, btn);
				insIndex++;
			}
		}
	}

	QList<QMenu*> modulesMenus = _core->modules()->generateModulesMenus();
	if (modulesMenus.count() > 0)
	{
		for (int i = 0; i < modulesMenus.count(); ++i)
		{
			if (modulesMenus.at(i) != 0)
			{
				QString objName = "but" + modulesMenus.at(i)->objectName().remove(0, 3);
				QString text = modulesMenus.at(i)->title();
				QPushButton* btn = createButton(objName, text);
				btn->setMenu(modulesMenus.at(i));
				_ui->layotButtons->insertWidget(insIndex, btn);
				insIndex++;
			}
		}
	}
	// end creation advanced menu

	QMenu *menuInfo = new QMenu(this);
	menuInfo->addAction(actHelp);
	menuInfo->addAction(actAbout);
	_ui->butHelp->setMenu(menuInfo);

//     connect(_ui->butHelp, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(_ui->delayBox, SIGNAL(valueChanged(int)), this, SLOT(delayBoxChange(int)));
    connect(_ui->cbxTypeScr, SIGNAL(activated(int)), this, SLOT(typeScreenShotChange(int)));

    connect(_core, SIGNAL(newScreenShot(QPixmap*)), this, SLOT(restoreWindow()) );
    QIcon icon(":/res/img/logo.png");
    setWindowIcon(icon);

    resize(_core->conf->getRestoredWndSize().width(), _core->conf->getRestoredWndSize().height());

    move(QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber() ).width()/2 - width()/2,
         QApplication::desktop()->availableGeometry(
                QApplication::desktop()->screenNumber()).height()/2 - height()/2);

    displayPixmap();

	_ui->scrLabel->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete _ui;
}


void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (_core->conf->getCloseInTray() == true && _core->conf->getShowTrayIcon() == true)
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
    Q_UNUSED(event)
    // get size dcreen pixel map
    QSize scaleSize = _core->getPixmap()->size(); // get orig size pixmap

    scaleSize.scale(_ui->scrLabel->size(), Qt::KeepAspectRatio);

    // if not scrlabel pixmap
    if (!_ui->scrLabel->pixmap() || scaleSize != _ui->scrLabel->pixmap()->size())
    {
        displayPixmap();
    }

}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == _ui->scrLabel && event->type() == QEvent::ToolTip)
	{
		displatScreenToolTip();
	}

	if (obj == _ui->scrLabel && event->type() == QEvent::MouseButtonDblClick)
	{
		_core->openInExtViewer();
	}


    return QObject::eventFilter(obj, event);
}

void MainWindow::show()
{
    if (_trayIcon != NULL)
    {
        _trayIcon->setVisible(true);
    }
    QMainWindow::show();
}

bool MainWindow::isTrayed() const
{
    if (_trayIcon != NULL)
    {
        return true;
    }
    else
	{
		return false;
	}
}


void MainWindow::showHelp()
{
    // open help file
    QString localeHelpFile;

    localeHelpFile = QString(SG_DOCDIR) + "%1html%1" + Config::getSysLang()+"%1index.html";
    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

    if (QFile::exists(localeHelpFile) != true)
    {
        localeHelpFile = QString(SG_DOCDIR) + "%1html%1" + Config::getSysLang().section("_", 0, 0)  + "%1index.html";
        localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

        if (QFile::exists(localeHelpFile) != true)
        {
            localeHelpFile = QString(SG_DOCDIR) + "%1html%1en%1index.html";
            localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));
        }
    }

    // open find localize or eng help help
    QDesktopServices::openUrl(QUrl::fromLocalFile(localeHelpFile));
}


void MainWindow::showOptions()
{
    ConfigDialog *options;
    options = new ConfigDialog();
#ifdef SG_GLOBAL_SHORTCUTS
    globalShortcutBlock(true);
#endif

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
#ifdef SG_GLOBAL_SHORTCUTS
    globalShortcutBlock(false);
#endif
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
    if (_core->conf->getShowTrayIcon() == true)
    {
		//  unblock tray signals
		_trayIcon->blockSignals(true);
		_trayIcon->setContextMenu(NULL); // enable context menu
    }

    if (_core->conf->getDelay() == 0)
    {
		// if select 0s delay & hide window -- make 0.2s delay for hiding window
		QTimer::singleShot(200, _core, SLOT(screenShot()));
    }
    else
    {
		QTimer::singleShot(1000*_core->conf->getDelay(), _core, SLOT(screenShot()));
    }
}

void MainWindow::copyScreen()
{
    _core->copyScreen();
}

void MainWindow::displatScreenToolTip()
{
	quint16 w = _core->getPixmap()->size().width();
	quint16 h = _core->getPixmap()->size().height();
	QString toolTip = tr("Screenshot ") + QString::number(w) + "x" + QString::number(h);
	if (_core->conf->getEnableExtView() == 1)
	{
		toolTip += "\n\n";
		toolTip += tr("Double click for open screenshot in external default image viewer");
	}

	_ui->scrLabel->setToolTip(toolTip);
}

QPushButton* MainWindow::createButton(const QString& objName, const QString& text)
{
	QPushButton* btn = new QPushButton(this);
	btn->setObjectName(objName);
	btn->setText(text);
	return btn;
}

// crete tray
void MainWindow::createTray()
{
    _trayed = false;

    // create actions menu
    actQuit = new QAction(tr("Quit"), this);
    actSave = new QAction(tr("Save"), this);
    actNew = new QAction(tr("New"), this);
    actCopy = new QAction(tr("Copy"), this);
    actHideShow = new QAction(tr("Hide"), this);
    actAbout = new QAction(tr("About"), this);
    mOptions = new QAction(tr("Options"), this);
    actHelp = new QAction(tr("Help"), this);

    // connect to slots
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(actSave, SIGNAL(triggered()), this, SLOT(saveScreen()) );
    connect(actCopy, SIGNAL(triggered()), this, SLOT(copyScreen()));
    connect(actNew, SIGNAL(triggered()), this, SLOT(newScreen()));
    connect(actHideShow, SIGNAL(triggered()), this, SLOT(windowHideShow()));
    connect(mOptions, SIGNAL(triggered()), this, SLOT(showOptions()) );
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()) );
    connect(actAbout, SIGNAL(triggered()), this, SLOT(showAbout()) );
    connect(_core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)), this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    // create tray menu
    menuTray = new QMenu(this);
    menuTray->addAction(actHideShow);
    menuTray->addSeparator();
    menuTray->addAction(actNew); // TODO - add icons (icon, action)
    menuTray->addAction(actSave);
    menuTray->addAction(actCopy);
    menuTray->addSeparator();
    menuTray->addAction(mOptions);
    menuTray->addSeparator();
    menuTray->addAction(actHelp);
    menuTray->addAction(actAbout);
    menuTray->addSeparator();
    menuTray->addAction(actQuit);

    // icon menu
    QIcon icon(":/res/img/logo.png");

    _trayIcon = new QSystemTrayIcon(this);

    _trayIcon->setContextMenu(menuTray);
    _trayIcon->setIcon(icon);
    _trayIcon->show();
    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)) ,
             this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)) );
}

void MainWindow::killTray()
{
    disconnect(_core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)), this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    _trayed = false;
    delete _trayIcon;
    _trayIcon = NULL;

    delete actHelp;
    delete mOptions;
    delete actAbout;
    delete actHideShow;
    delete actCopy;
    delete actNew;
    delete actSave;
    delete actQuit;
}

void MainWindow::delayBoxChange(int delay)
{
    if (delay == 0)
    {
        _ui->delayBox->setSpecialValueText(tr("None"));
    }
    _core->conf->setDelay(delay);
}

void MainWindow::typeScreenShotChange(int type)
{
    _core->conf->setTypeScreen(type);
}


void MainWindow::receivedStateNotifyMessage(StateNotifyMessage state)
{
    trayShowMessage(state.header, state.message);
}

void MainWindow::quit()
{
	_core->conf->setRestoredWndSize(width(), height());
    _core->conf->saveWndSize();

    _core->coreQuit();
}


// updating UI from configdata
void MainWindow::updateUI()
{
    // update delay spinbox
    _ui->delayBox->setValue(_core->conf->getDelay());

    // update shortcuts
    createShortcuts();

    // create tray object
    if (_core->conf->getShowTrayIcon() == true && _trayIcon == NULL)
    {
        createTray();
    }

    // kill tray object, if created
    if (_core->conf->getShowTrayIcon() == false && _trayIcon != NULL)
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
        actHideShow->setText(tr("Hide"));
        _trayed = false;
        showNormal();
        activateWindow();
    }
    else if (isHidden() == false)
    {
        actHideShow->setText(tr("Show"));
        showMinimized();
        hide();
        _trayed = true;
    }
}

void MainWindow::showWindow(const QString& str)
{
    // get char of type screen (last) form reviewd string
    QString typeNum = str[str.size() - 1];
    int type = typeNum.toInt();

    // change type scrren in config & on main window
    _ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);

    _core->sleep(250); // it hack for non capture  interface of apss
    _core->screenShot();

    Q_UNUSED(str)
    if (isHidden() == true && _core->conf->getShowTrayIcon() == true)
    {
        actHideShow->setText(tr("Hide"));
        _trayed = false;
        showNormal();
    }

    netwm::init(); // initialize NETWM
    netwm::climsg(this->winId(), NET_ACTIVE_WINDOW, 2, QX11Info::appUserTime());

    // small gnome hack
    if (qgetenv("DESKTOP_SESSION") == "gnome")
    {
        showNormal();
    }
}

// show tray messages
void MainWindow::trayShowMessage(QString titleMsg, QString bodyMsg )
{
    if (_core->conf->getShowTrayIcon() == true)
    {
        switch(_core->conf->getTrayMessages())
        {
            case 0: break; // is never sjow
            case 1: // is hidden main wnd
            {
                if (isHidden() == true && _trayed == true)
                {
                    _trayIcon->showMessage(titleMsg, bodyMsg,
                    QSystemTrayIcon::MessageIcon(), _core->conf->getTimeTrayMess()*1000 ); //5000
                }
                break;
            }
            case 2: // always show
            {
                _trayIcon->showMessage(titleMsg, bodyMsg,
                QSystemTrayIcon::MessageIcon(), _core->conf->getTimeTrayMess()*1000 );
                break;
            }
            default: break;
        }
    }
}


void MainWindow::displayPixmap()
{
    _ui->scrLabel->setPixmap(_core->getPixmap()->scaled(_ui->scrLabel->size(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::restoreWindow()
{
    displayPixmap();

    if (isVisible() == false && _trayed == false)
    {
        showNormal();
    }

    // if show trat
    if (_core->conf->getShowTrayIcon() == true)
    {
        _trayIcon->blockSignals(false);
        _trayIcon->setContextMenu(menuTray); // enable context menu
    }
}


void MainWindow::saveScreen()
{
    // create initial filepath
    QHash<QString, QString> formatsAvalible;

    formatsAvalible["png"] = tr("PNG Files");
    formatsAvalible["jpg"] = tr("JPEG Files");
    formatsAvalible["bmp"] = tr("BMP Files");

    QString format = _core->conf->getSaveFormat();
    QString filePath = _core->getSaveFilePath(format);

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
    if (qgetenv("DESKTOP_SESSION").contains("kde") || qgetenv("DESKTOP_SESSION") == "gnome")
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected);
    }
    else
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected, QFileDialog::DontUseNativeDialog);
    }

    QRegExp rx("\\(\\*\\.[a-z]{3,4}\\)");
    quint8 tmp = filterSelected.size() - rx.indexIn(filterSelected);

    filterSelected.chop(tmp + 1);
    format = formatsAvalible.key(filterSelected);

    // if user canceled saving
    if (fileName.isEmpty() == true)
    {
        return ;
    }

    _core->writeScreen(fileName, format);
}


void MainWindow::createShortcuts()
{
    _ui->butNew->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutNew));
    _ui->butSave->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutSave));
    _ui->butCopy->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutCopy));
    _ui->butOpt->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutOptions));
    _ui->butHelp->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutHelp));

    if (_core->conf->getCloseInTray() == true && _core->conf->getShowTrayIcon() == true)
    {
		_ui->butQuit->setShortcut(QKeySequence());
		_hideWnd = new QShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutClose), this);
		connect(_hideWnd, SIGNAL(activated()), this, SLOT(close()));
    }
    else
    {
		if (_hideWnd != NULL)
		{
			delete _hideWnd;
		}
		_ui->butQuit->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutClose));
    }


#ifdef SG_GLOBAL_SHORTCUTS
    for (int i = 0; i < _globalShortcuts.count(); ++i )
    {
        _globalShortcuts[i]->setShortcut(QKeySequence(_core->conf->shortcuts()->getShortcut(i)));
    }
#endif
}

#ifdef SG_GLOBAL_SHORTCUTS
void MainWindow::globalShortcutBlock(bool state)
{
    for (int i = 0; i < _globalShortcuts.count(); ++i )
    {
		_globalShortcuts[i]->setDisabled(state);
    }
}


void MainWindow::globalShortcutActivate(int type)
{
    _ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);

    if (_trayed == false)
    {
        hide();
    }
    QTimer::singleShot(200, _core, SLOT(screenShot()));

}
#endif
