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

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    _ui(new Ui::MainWindow), _conf(NULL), _trayMenu(NULL)
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
    Core *c = Core::instance();

    connect(actQuit, &QAction::triggered, c, &Core::coreQuit);
    connect(actNew, &QAction::triggered, c, &Core::setScreen);
    connect(actSave, &QAction::triggered, this, &MainWindow::saveScreen);
    connect(actCopy, &QAction::triggered, c, &Core::copyScreen);
    connect(actOptions, &QAction::triggered, this, &MainWindow::showOptions);
    connect(actAbout, &QAction::triggered, this, &MainWindow::showAbout);
    connect(actHelp, &QAction::triggered, this, &MainWindow::showHelp);

    _ui->toolBar->addAction(actNew);
    _ui->toolBar->addAction(actSave);
    _ui->toolBar->addAction(actCopy);
    _ui->toolBar->addAction(actOptions);
    _ui->toolBar->addSeparator();
    QMenu *menuInfo = new QMenu(this);
    menuInfo->addAction(actHelp);
    menuInfo->addAction(actAbout);
    QToolButton *help = new QToolButton(this);
    help->setText(tr("Help"));
    help->setPopupMode(QToolButton::InstantPopup);
    help->setIcon(QIcon::fromTheme("system-help"));
    help->setMenu(menuInfo);

    _ui->toolBar->addWidget(help);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _ui->toolBar->addWidget(spacer);

    _ui->toolBar->addAction(actQuit);

    void (QSpinBox::*delayChange)(int) = &QSpinBox::valueChanged;
    connect(_ui->delayBox, delayChange, this, &MainWindow::delayBoxChange);
    void (QComboBox::*typeScr)(int) = &QComboBox::currentIndexChanged;
    connect(_ui->cbxTypeScr, typeScr, this, &MainWindow::typeScreenShotChange);
    connect(_ui->checkIncludeCursor, &QCheckBox::toggled, this, &MainWindow::checkIncludeCursor);

    QIcon icon(":/res/img/logo.png");
    setWindowIcon(icon);

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
    if (_conf->getCloseInTray() && _conf->getShowTrayIcon())
    {
        windowHideShow();
        e->ignore();
    }
    else
        actQuit->activate(QAction::Trigger);
}

// resize main window
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // get size dcreen pixel map
    QSize scaleSize = Core::instance()->getPixmap()->size(); // get orig size pixmap

    scaleSize.scale(_ui->scrLabel->size(), Qt::KeepAspectRatio);

    // if not scrlabel pixmap
    if (!_ui->scrLabel->pixmap() || scaleSize != _ui->scrLabel->pixmap()->size())
        updatePixmap(Core::instance()->getPixmap());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _ui->scrLabel && event->type() == QEvent::ToolTip)
        displatScreenToolTip();

    else if (obj == _ui->scrLabel && event->type() == QEvent::MouseButtonDblClick)
        Core::instance()->openInExtViewer();

    return QObject::eventFilter(obj, event);
}


void MainWindow::updatePixmap(QPixmap *pMap)
{
    _ui->scrLabel->setPixmap(pMap->scaled(_ui->scrLabel->size(),
                                          Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::updateModulesActions(QList<QAction *> list)
{
    _ui->toolBar->insertSeparator(actOptions);
    if (list.count() > 0)
    {
        for (int i = 0; i < list.count(); ++i)
        {
            QAction *action = list.at(i);
            if (action)
                _ui->toolBar->insertAction(actOptions, action);
        }
    }
}

void MainWindow::updateModulesenus(QList<QMenu *> list)
{
    if (list.count() > 0)
    {
        for (int i = 0; i < list.count(); ++i)
        {
            QMenu *menu = list.at(i);
            if (menu != 0)
            {
                QToolButton* btn = new QToolButton(this);
                btn->setText(menu->title());
                btn->setPopupMode(QToolButton::InstantPopup);
                btn->setToolTip(menu->title());
                btn->setMenu(list.at(i));
                _ui->toolBar->insertWidget(actOptions, btn);
            }
        }
        _ui->toolBar->insertSeparator(actOptions);
    }
}

void MainWindow::show()
{
    if (!isVisible() && !_trayed)
        showNormal();

    if (_conf->getShowTrayIcon())
    {
        _trayIcon->blockSignals(false);
        _trayIcon->setContextMenu(_trayMenu); // enable context menu
    }

    if (_trayIcon)
        _trayIcon->setVisible(true);

    QMainWindow::show();
}

bool MainWindow::isTrayed() const
{
    return _trayIcon != NULL;
}

void MainWindow::showTrayMessage(const QString& header, const QString& message)
{
    if (_conf->getShowTrayIcon())
    {
        switch(_conf->getTrayMessages())
        {
            case 0: break; // never shown
            case 1: // main window hidden
            {
                if (isHidden() && _trayed)
                {
                    _trayIcon->showMessage(header, message,
                    QSystemTrayIcon::MessageIcon(), _conf->getTimeTrayMess()*1000 ); //5000
                }
                break;
            }
            case 2: // always show
            {
                _trayIcon->showMessage(header, message,
                QSystemTrayIcon::MessageIcon(), _conf->getTimeTrayMess()*1000 );
                break;
            }
            default: break;
        }
    }
}


void MainWindow::setConfig(Config *config)
{
    _conf = config;
    updateUI();
}


void MainWindow::showHelp()
{
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
        hideToShot();
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
        hideToShot();
    }
    else
        about->exec();

    delete about;
}

void MainWindow::displatScreenToolTip()
{
    QSize pSize = Core::instance()->getPixmap()->size();
    quint16 w = pSize.width();
    quint16 h = pSize.height();
    QString toolTip = tr("Screenshot ") + QString::number(w) + "x" + QString::number(h);
    if (_conf->getEnableExtView())
    {
        toolTip += "\n\n";
        toolTip += tr("Double click for open screenshot in external default image viewer");
    }

    _ui->scrLabel->setToolTip(toolTip);
}

void MainWindow::createTray()
{
    _trayed = false;
    actHideShow = new QAction(tr("Hide"), this);
    connect(actHideShow, &QAction::triggered, this, &MainWindow::windowHideShow);

    // create tray menu
    _trayMenu = new QMenu(this);
    _trayMenu->addAction(actHideShow);
    _trayMenu->addSeparator();
    _trayMenu->addAction(actNew); // TODO - add icons (icon, action)
    _trayMenu->addAction(actSave);
    _trayMenu->addAction(actCopy);
    _trayMenu->addSeparator();
    _trayMenu->addAction(actOptions);
    _trayMenu->addSeparator();
    _trayMenu->addAction(actHelp);
    _trayMenu->addAction(actAbout);
    _trayMenu->addSeparator();
    _trayMenu->addAction(actQuit);

    // icon menu
    QIcon icon(":/res/img/logo.png");

    _trayIcon = new QSystemTrayIcon(this);

    _trayIcon->setContextMenu(_trayMenu);
    _trayIcon->setIcon(icon);
    _trayIcon->show();
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayClick);
}

void MainWindow::killTray()
{
    _trayed = false;
    _trayMenu->clear();

    delete _trayIcon;
    _trayIcon = NULL;
}

void MainWindow::delayBoxChange(int delay)
{
    if (delay == 0)
        _ui->delayBox->setSpecialValueText(tr("None"));
    _conf->setDelay(delay);
}

void MainWindow::typeScreenShotChange(int type)
{
    _conf->setTypeScreen(type);
}

void MainWindow::checkIncludeCursor(bool include)
{
    _conf->setIncludeCursor(include);
}

// updating UI from configdata
void MainWindow::updateUI()
{
    _ui->cbxTypeScr->setCurrentIndex(_conf->getTypeScreen());
    _ui->delayBox->setValue(_conf->getDelay());
    _ui->checkIncludeCursor->setChecked(_conf->getIncludeCursor());

    updateShortcuts();

    // create tray object
    if (_conf->getShowTrayIcon() && !_trayIcon)
        createTray();

    // kill tray object, if tray was disabled in the configuration dialog
    if (!_conf->getShowTrayIcon() && _trayIcon)
        killTray();
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

void MainWindow::hideToShot()
{
    if (_conf->getShowTrayIcon())
    {
        _trayIcon->blockSignals(true);
        _trayIcon->setContextMenu(NULL); // enable context menu
    }

    hide();
}

void MainWindow::showWindow(const QString& str)
{
    // get char of type screen (last) form received string
    QString typeNum = str[str.size() - 1];
    int type = typeNum.toInt();

    // change type scrren in config & on main window
    _ui->cbxTypeScr->setCurrentIndex(type);
    typeScreenShotChange(type);
}

void MainWindow::restoreFromShot()
{
    if (!isVisible() && !_trayed)
        showNormal();

    // if show tray
    if (_conf->getShowTrayIcon())
    {
        _trayIcon->blockSignals(false);
        _trayIcon->setContextMenu(_trayMenu); // enable context menu
    }
}

void MainWindow::saveScreen()
{
    // create initial filepath
    QHash<QString, QString> formatsAvalible;

    formatsAvalible["png"] = tr("PNG Files");
    formatsAvalible["jpg"] = tr("JPEG Files");
    formatsAvalible["bmp"] = tr("BMP Files");

    QString format = "png";
    _conf->getSaveFormat();

    Core* c = Core::instance();
    QString filePath = c->getSaveFilePath(format);

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
    fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters, &filterSelected, QFileDialog::DontUseNativeDialog);

    QRegExp rx("\\(\\*\\.[a-z]{3,4}\\)");
    quint8 tmp = filterSelected.size() - rx.indexIn(filterSelected);

    filterSelected.chop(tmp + 1);
    format = formatsAvalible.key(filterSelected);

    // if user canceled saving
    if (fileName.isEmpty())
        return;

    c->writeScreen(fileName, format);
}

void MainWindow::updateShortcuts()
{
    actNew->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutNew));
    actSave->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutSave));
    actCopy->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutCopy));
    actOptions->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutOptions));
    actHelp->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutHelp));
    actQuit->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutClose));

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
