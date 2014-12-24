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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../core.h"
#include "../shortcutmanager.h"

#include <QDir>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QHash>
#include <QHashIterator>
#include <QRegExp>
#include <QTimer>
#include <QToolButton>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    _trayed = false;

#ifdef SG_GLOBAL_SHORTCUTS
    // signal mapper
    _globalShortcutSignals = new QSignalMapper(this);

    // global shortcuts
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

    // create actions menu
    actNew = new QAction(QIcon::fromTheme("document-new"), tr("New"), this);
    actSave = new QAction(QIcon::fromTheme("document-save"), tr("Save"), this);
    actCopy = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy"), this);
    actOptions = new QAction(QIcon::fromTheme("configure"), tr("Options"), this);
    actHelp = new QAction(QIcon::fromTheme("system-help"), tr("Help"), this);
    actAbout = new QAction(QIcon::fromTheme("system-about"), tr("About"), this);
    actQuit = new QAction(QIcon::fromTheme("application-exit"), tr("Quit"), this);

    // connect actions to slots
    connect(actNew, SIGNAL(triggered()), this, SLOT(newScreen()));
    connect(actSave, SIGNAL(triggered()), this, SLOT(saveScreen()));
    connect(actCopy, SIGNAL(triggered()), this, SLOT(copyScreen()));
    connect(actOptions, SIGNAL(triggered()), this, SLOT(showOptions()));
    connect(actHelp, SIGNAL(triggered()), this, SLOT(showHelp()));
    connect(actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(actQuit, SIGNAL(triggered()), this, SLOT(quit()));

    updateUI();

//    delayBoxChange(_core->conf->getDelay()); // TO REWORK
//    _ui->cbxTypeScr->setCurrentIndex(conf->getTypeScreen());

    _ui->toolBar->addAction(actNew);
    _ui->toolBar->addAction(actSave);
    _ui->toolBar->addAction(actCopy);
    _ui->toolBar->addSeparator();

    // Create advanced menu
//    QList<QAction*> modulesActions = _core->modules()->generateModulesActions();

//    if (modulesActions.count() > 0)
//    {
//        for (int i = 0; i < modulesActions.count(); ++i)
//        {
//            QAction *action = modulesActions.at(i);
//            if (action)
//                _ui->toolBar->addAction(action);
//        }
//    }

//    QList<QMenu*> modulesMenus = _core->modules()->generateModulesMenus();
//    if (modulesMenus.count() > 0)
//    {
//        for (int i = 0; i < modulesMenus.count(); ++i)
//        {
//            QMenu *menu = modulesMenus.at(i);
//            if (menu != 0)
//            {
//                QToolButton* btn = new QToolButton(this);
//                btn->setText(menu->title());
//                btn->setPopupMode(QToolButton::InstantPopup);
//                btn->setToolTip(menu->title());
//                btn->setMenu(modulesMenus.at(i));
//                _ui->toolBar->addWidget(btn);
//            }
//        }
//    }
    // end creation advanced menu

    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(actOptions);

    QMenu *menuInfo = new QMenu(this);
    menuInfo->addAction(actHelp);
    menuInfo->addAction(actAbout);
    QToolButton *help = new QToolButton(this);
    help->setText(tr("Help"));
    help->setPopupMode(QToolButton::InstantPopup);
    help->setIcon(QIcon::fromTheme("system-help"));
    help->setMenu(menuInfo);

    _ui->toolBar->addWidget(help);
    _ui->toolBar->addAction(actQuit);

    connect(_ui->delayBox, SIGNAL(valueChanged(int)), this, SLOT(delayBoxChange(int)));
    connect(_ui->cbxTypeScr, SIGNAL(activated(int)), this, SLOT(typeScreenShotChange(int)));

    // TODO move to core
//    connect(_core, SIGNAL(newScreenShot(QPixmap*)), this, SLOT(restoreWindow()));
    QIcon icon(":/res/img/logo.png");
    setWindowIcon(icon);

//    resize(_core->conf->getRestoredWndSize().width(), _core->conf->getRestoredWndSize().height());

    QRect geometry = QApplication::desktop()->availableGeometry(QApplication::desktop()->screenNumber());
    move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);

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

// TODO - refactor
//    if (_core->conf->getCloseInTray() && _core->conf->getShowTrayIcon())
//    {
//        windowHideShow();
//        e->ignore();
//    }
//    else
//        quit();
}

// resize main window
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // get size dcreen pixel map
//    QSize scaleSize = _core->getPixmap()->size(); // get orig size pixmap

//    scaleSize.scale(_ui->scrLabel->size(), Qt::KeepAspectRatio);

//    // if not scrlabel pixmap
//    if (!_ui->scrLabel->pixmap() || scaleSize != _ui->scrLabel->pixmap()->size())
//        displayPixmap();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _ui->scrLabel && event->type() == QEvent::ToolTip)
        //displatScreenToolTip();
        qDebug() << "Move to core";

    else if (obj == _ui->scrLabel && event->type() == QEvent::MouseButtonDblClick)
//        _core->openInExtViewer();
        qDebug() << "Move to core";

    return QObject::eventFilter(obj, event);
}

void MainWindow::show()
{
    if (_trayIcon)
        _trayIcon->setVisible(true);

    QMainWindow::show();
}

bool MainWindow::isTrayed() const
{
    return _trayIcon != NULL;
}

void MainWindow::showHelp()
{
    // open help file
    QString localeHelpFile;

    localeHelpFile = QString(SG_DOCDIR) + "%1html%1" + Config::getSysLang()+"%1index.html";
    localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

    if (!QFile::exists(localeHelpFile))
    {
        localeHelpFile = QString(SG_DOCDIR) + "%1html%1" + Config::getSysLang().section("_", 0, 0)  + "%1index.html";
        localeHelpFile = localeHelpFile.arg(QString(QDir::separator()));

        if (!QFile::exists(localeHelpFile))
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
    ConfigDialog *options = new ConfigDialog();
#ifdef SG_GLOBAL_SHORTCUTS
    globalShortcutBlock(true);
#endif

    if (isMinimized())
    {
        showNormal();
        if (options->exec() == QDialog::Accepted)
            updateUI();
        hide();
    }
    else
    {
        if (options->exec() == QDialog::Accepted)
            updateUI();
    }

#ifdef SG_GLOBAL_SHORTCUTS
    globalShortcutBlock(false);
#endif
    delete options;
}

void MainWindow::showAbout()
{
    AboutDialog *about = new AboutDialog(this);

    if (isMinimized())
    {
        showNormal();
        about->exec();
        hide();
    }
    else
        about->exec();

    delete about;
}


/*
 * clicked on new screen
 */
void MainWindow::newScreen()
{
    setHidden(true);

// MOVE TO CORE
    // if show tray
//    if (->conf->getShowTrayIcon())
//    {
//        //  unblock tray signals
//        _trayIcon->blockSignals(true);
//        _trayIcon->setContextMenu(NULL); // enable context menu
//    }

    // if select 0s delay & hide window -- make 0.2s delay for hiding window
//    if (_core->conf->getDelay() == 0)
//        QTimer::singleShot(200, _core, SLOT(screenShot()));
//    else
//        QTimer::singleShot(1000 * _core->conf->getDelay(), _core, SLOT(screenShot()));
}

void MainWindow::copyScreen()
{
//    _core->copyScreen();
}

void MainWindow::displatScreenToolTip()
{
//    quint16 w = _core->getPixmap()->size().width();
//    quint16 h = _core->getPixmap()->size().height();
//    QString toolTip = tr("Screenshot ") + QString::number(w) + "x" + QString::number(h);
//    if (_core->conf->getEnableExtView())
//    {
//        toolTip += "\n\n";
//        toolTip += tr("Double click for open screenshot in external default image viewer");
//    }

//    _ui->scrLabel->setToolTip(toolTip);
}

void MainWindow::createTray()
{
    _trayed = false;
    actHideShow = new QAction(tr("Hide"), this);
    connect(actHideShow, SIGNAL(triggered()), this, SLOT(windowHideShow()));

    // Подключать наоборот из ядра в окно
//    connect(_core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)),
//            this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    // create tray menu
    menuTray = new QMenu(this);
    menuTray->addAction(actHideShow);
    menuTray->addSeparator();
    menuTray->addAction(actNew); // TODO - add icons (icon, action)
    menuTray->addAction(actSave);
    menuTray->addAction(actCopy);
    menuTray->addSeparator();
    menuTray->addAction(actOptions);
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
    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(trayClick(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::killTray()
{
//    disconnect(_core, SIGNAL(sendStateNotifyMessage(StateNotifyMessage)), this, SLOT(receivedStateNotifyMessage(StateNotifyMessage)));

    _trayed = false;
    delete _trayIcon;
    _trayIcon = NULL;
    delete actHideShow;
}

void MainWindow::delayBoxChange(int delay)
{
    if (delay == 0)
        _ui->delayBox->setSpecialValueText(tr("None"));
//    ->conf->setDelay(delay);
}

void MainWindow::typeScreenShotChange(int type)
{
    /// TODO - подключать наоборот из ядра
//    _core->conf->setTypeScreen(type);
}

/* TO REWORK */
//void MainWindow::receivedStateNotifyMessage(StateNotifyMessage state)
//{
//    trayShowMessage(state.header, state.message);
//}

void MainWindow::quit()
{
//    >conf->setRestoredWndSize(width(), height());
//    >conf->saveWndSize();
//    >coreQuit();
}

// updating UI from configdata
void MainWindow::updateUI()
{
    // update delay spinbox
//    _ui->delayBox->setValue(_->conf->getDelay());

    // update shortcuts
    createShortcuts();

    // create tray object
//    if (_core->conf->getShowTrayIcon() && !_trayIcon)
//        createTray();

//    // kill tray object, if created
//    if (!_core->conf->getShowTrayIcon() && _trayIcon)
//        killTray();
}

// mouse clicks on tray icom
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
    if (isHidden())
    {
        actHideShow->setText(tr("Hide"));
        _trayed = false;
        showNormal();
        activateWindow();
    }
    else
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

//    _core->sleep(250); // hack for WMs with compositing fade-out effects
//    _core->screenShot();

    // TODO  - make with conf
//    if (isHidden() && _>conf->getShowTrayIcon())
//    {
//        actHideShow->setText(tr("Hide"));
//        _trayed = false;
//        showNormal();
//    }
}

// show tray messages
void MainWindow::trayShowMessage(QString titleMsg, QString bodyMsg )
{
    qDebug() << "Show tray message";
    // TODO - make unv tray
//    if (_core->conf->getShowTrayIcon())
//    {
//        switch(_core->conf->getTrayMessages())
//        {
//            case 0: break; // never shown
//            case 1: // main window hidden
//            {
//                if (isHidden() && _trayed)
//                {
//                    _trayIcon->showMessage(titleMsg, bodyMsg,
//                    QSystemTrayIcon::MessageIcon(), _core->conf->getTimeTrayMess()*1000 ); //5000
//                }
//                break;
//            }
//            case 2: // always show
//            {
//                _trayIcon->showMessage(titleMsg, bodyMsg,
//                QSystemTrayIcon::MessageIcon(), _core->conf->getTimeTrayMess()*1000 );
//                break;
//            }
//            default: break;
//        }
//    }
}

void MainWindow::displayPixmap()
{
    qDebug() << "Dusplay pixmap";
//    _ui->scrLabel->setPixmap(_core->getPixmap()->scaled(_ui->scrLabel->size(),
//                Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::restoreWindow()
{
    displayPixmap();

    if (!isVisible() && !_trayed)
        showNormal();

    // if show tray
//    if (_conf->getShowTrayIcon())
//    {
//        _trayIcon->blockSignals(false);
//        _trayIcon->setContextMenu(menuTray); // enable context menu
//    }
}

void MainWindow::saveScreen()
{
    // create initial filepath
    QHash<QString, QString> formatsAvalible;

    formatsAvalible["png"] = tr("PNG Files");
    formatsAvalible["jpg"] = tr("JPEG Files");
    formatsAvalible["bmp"] = tr("BMP Files");

    QString format = "png";
//    _>conf->getSaveFormat();
    QString filePath = "";
//    _getSaveFilePath(format);

    // create file filters
    QString fileFilters;

    QString filterSelected;
    filterSelected = formatsAvalible[format];

    QHash<QString, QString>::const_iterator iter = formatsAvalible.constBegin();
    while (iter != formatsAvalible.constEnd())
    {
        fileFilters.append(iter.value() + " (*." + iter.key() + ");;");
        ++iter;
    }
    fileFilters.chop(2);

    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected);

    QRegExp rx("\\(\\*\\.[a-z]{3,4}\\)");
    quint8 tmp = filterSelected.size() - rx.indexIn(filterSelected);

    filterSelected.chop(tmp + 1);
    format = formatsAvalible.key(filterSelected);

    // if user canceled saving
    if (fileName.isEmpty())
        return;

    // TODO - передавать в ядро через сигнал
    //_core->writeScreen(fileName, format);
}

void MainWindow::createShortcuts()
{

    // TODO - подключать наоборот из ядра
//    actNew->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutNew));
//    actSave->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutSave));
//    actCopy->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutCopy));
//    actOptions->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutOptions));
//    actHelp->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutHelp));

//    if (_core->conf->getCloseInTray() && _core->conf->getShowTrayIcon())
//    {
//        actQuit->setShortcut(QKeySequence());
//        _hideWnd = new QShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutClose), this);
//        connect(_hideWnd, SIGNAL(activated()), this, SLOT(close()));
//    }
//    else
//    {
//        if (_hideWnd)
//            delete _hideWnd;
//        actQuit->setShortcut(_core->conf->shortcuts()->getShortcut(Config::shortcutClose));
//    }

#ifdef SG_GLOBAL_SHORTCUTS
    for (int i = 0; i < _globalShortcuts.count(); ++i)
        _globalShortcuts[i]->setShortcut(QKeySequence(_core->conf->shortcuts()->getShortcut(i)));
#endif
}

#ifdef SG_GLOBAL_SHORTCUTS
void MainWindow::globalShortcutBlock(bool state)
{
    for (int i = 0; i < _globalShortcuts.count(); ++i)
        _globalShortcuts[i]->setDisabled(state);
}


void MainWindow::globalShortcutActivate(int type)
{
    _ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);

    if (!_trayed)
        hide();
    QTimer::singleShot(200, _core, SLOT(screenShot()));
}
#endif
