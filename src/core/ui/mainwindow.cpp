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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../core.h"
#include "../shortcutmanager.h"

#include <QDir>
#include <QFileDialog>
#include <QScreen>
#include <QHash>
#include <QHashIterator>
#include <QRegularExpression>
#include <QTimer>
#include <QToolButton>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    _ui(new Ui::MainWindow), _conf(nullptr), _trayMenu(nullptr)
{
    _ui->setupUi(this);
    _trayed = false;

    _trayIcon = nullptr;
    _trayMenu = nullptr;

    // create actions menu
    actNew = new QAction(QIcon::fromTheme(QStringLiteral("document-new")), tr("New"), this);
    actSave = new QAction(QIcon::fromTheme(QStringLiteral("document-save")), tr("Save"), this);
    actCopy = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("Copy"), this);
    actOptions = new QAction(QIcon::fromTheme(QStringLiteral("configure")), tr("Options"), this);
    actAbout = new QAction(QIcon::fromTheme(QStringLiteral("help-about")), tr("About"), this);
    actQuit = new QAction(QIcon::fromTheme(QStringLiteral("application-exit")), tr("Quit"), this);

    // connect actions to slots
    Core *c = Core::instance();

    connect(actQuit, &QAction::triggered, c, &Core::coreQuit);
    connect(actNew, &QAction::triggered, c, &Core::newScreenshot);
    connect(actSave, &QAction::triggered, this, &MainWindow::saveScreen);
    connect(actCopy, &QAction::triggered, c, &Core::copyScreen);
    connect(actOptions, &QAction::triggered, this, &MainWindow::showOptions);
    connect(actAbout, &QAction::triggered, this, &MainWindow::showAbout);

    _ui->toolBar->addAction(actNew);
    _ui->toolBar->addAction(actSave);
    _ui->toolBar->addAction(actCopy);
    _ui->toolBar->addAction(actOptions);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(actAbout);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _ui->toolBar->addWidget(spacer);

    _ui->toolBar->addAction(actQuit);

    connect(_ui->delayBox, &QSpinBox::valueChanged, this, &MainWindow::delayBoxChange);
    connect(_ui->checkIncludeCursor, &QCheckBox::toggled, this, &MainWindow::checkIncludeCursor);
    connect(_ui->checkNoDecoration, &QCheckBox::toggled, this, &MainWindow::checkNoDecoration);
    connect(_ui->checkZommMouseArea, &QCheckBox::toggled, this, &MainWindow::checkZommMouseArea);

    if (QGuiApplication::platformName() == QStringLiteral("wayland"))
    {
        // Wayland does not support window screenshot for now.
        // WARNING: If window shot becomes possible, change this and all occurrences of
        // _ui->cbxTypeScr->setCurrentIndex as well as MainWindow::typeScreenShotChange
        // in this file.
        _ui->cbxTypeScr->addItem(tr("Full screen"));
        _ui->cbxTypeScr->addItem(tr("Screen area"));
        _ui->cbxTypeScr->addItem(tr("Last selected area"));

        auto screens = QGuiApplication::screens();
        if (screens.size() > 1)
        {
            // sort screens from left to right and top to bottom
            std::sort(screens.begin(), screens.end(), [](QScreen *a1, QScreen *a2) {
                QPoint p1(a1->geometry().topLeft());
                QPoint p2(a2->geometry().topLeft());
                return (qAbs(p1.x() - p2.x()) > qAbs(p1.y() - p2.y())
                        ? p1.x() < p2.x() : p1.y() < p2.y());
            });
            for (const auto &screen : std::as_const(screens))
                _ui-> cbxScr->addItem(screen->name());
            _ui->cbxScr->setCurrentIndex(0);
            connect(_ui->cbxScr, &QComboBox::currentTextChanged, this,
                    [this] (const QString &text) {
                _conf->setScreen(text);
            });
        }
        else
        {
            _ui->labScr->hide();
            _ui->cbxScr->hide();
        }
    }
    else
    {
        _ui->cbxTypeScr->addItem(tr("Full screen"));
        _ui->cbxTypeScr->addItem(tr("Window"));
        _ui->cbxTypeScr->addItem(tr("Screen area"));
        _ui->cbxTypeScr->addItem(tr("Last selected area"));

        _ui->labScr->hide();
        _ui->cbxScr->hide();
    }

    _ui->cbxTypeScr->setCurrentIndex(0);
    connect(_ui->cbxTypeScr, &QComboBox::currentIndexChanged, this, &MainWindow::typeScreenShotChange);

    appIcon = QIcon::fromTheme (QStringLiteral("screengrab"));
    if (appIcon.isNull())
        appIcon = QIcon(QStringLiteral(":/res/img/logo.png"));

    setWindowIcon(appIcon);

    _ui->scrLabel->installEventFilter(this);
    _ui->delayBox->installEventFilter(this);
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
    if (!Core::instance()->noWin() && _conf->getCloseInTray() && _conf->getShowTrayIcon())
    {
        if (findChildren<QDialog*>().isEmpty()) // do not hide with a modal dialog
            windowHideShow();
        e->ignore();
    }
    else
        actQuit->activate(QAction::Trigger);
}

void MainWindow::fitPixmap()
{
    QSize scaleSize = Core::instance()->getPixmap()->size(); // orig pixmap size
    scaleSize.scale(_ui->scrLabel->contentsRect().size(), Qt::KeepAspectRatio);
    const QPixmap pixmap = _ui->scrLabel->pixmap(Qt::ReturnByValue);
    if (pixmap.isNull() || scaleSize != pixmap.size())
        updatePixmap(Core::instance()->getPixmap());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    fitPixmap();
}

void MainWindow::showEvent(QShowEvent *event)
{
    if (QGuiApplication::platformName() != QStringLiteral("wayland")
        && !event->spontaneous())
    { // center the window on X11
        auto screen = QGuiApplication::screenAt(QCursor::pos());
        if (screen == nullptr)
            screen = QGuiApplication::primaryScreen();
        const QRect geometry = screen ? screen->availableGeometry() : QRect();
        move(geometry.x() + (geometry.width() - width()) / 2,
             geometry.y() + (geometry.height() - height()) / 2);
    }
    QMainWindow::showEvent(event);
    fitPixmap();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == _ui->scrLabel)
    {
        if (event->type() == QEvent::ToolTip)
            displatScreenToolTip();
        else if (event->type() == QEvent::MouseButtonDblClick)
            Core::instance()->openInExtViewer();
    }
    else if (obj == _ui->delayBox && event->type() == QEvent::ShortcutOverride)
    { // filter out Ctrl+C because we need it
        if (QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event))
        {
            if ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_C)
                return true;
        }
    }

    return QObject::eventFilter(obj, event);
}


void MainWindow::updatePixmap(QPixmap *pMap)
{
    QSize lSize = _ui->scrLabel->contentsRect().size();
    // never scale up the image beyond its real size
    _ui->scrLabel->setPixmap(lSize.width() < pMap->width() || lSize.height() < pMap->height()
                                 ? pMap->scaled(lSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                                 : *pMap);
}

void MainWindow::updateModulesActions(const QList<QAction *> &list)
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

void MainWindow::updateModulesMenus(const QList<QMenu *> &list)
{
    if (list.count() > 0)
    {
        for (int i = 0; i < list.count(); ++i)
        {
            QMenu *menu = list.at(i);
            if (menu != nullptr)
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

QScreen* MainWindow::selectedScreen() const
{
    auto screens = QGuiApplication::screens();
    if (screens.isEmpty())
        return qApp->primaryScreen();
    if (screens.size() == 1)
        return screens.at(0);
    for (const auto &screen : std::as_const(screens))
    {
        if (_ui->cbxScr->currentText() == screen->name())
            return screen;
    }
    std::sort(screens.begin(), screens.end(), [](QScreen *a1, QScreen *a2) {
        QPoint p1(a1->geometry().topLeft());
        QPoint p2(a2->geometry().topLeft());
        return (qAbs(p1.x() - p2.x()) > qAbs(p1.y() - p2.y())
                ? p1.x() < p2.x() : p1.y() < p2.y());
    });
    return screens.at(0);
}

void MainWindow::show()
{
    if (!isVisible() && !_trayed)
        showNormal();
    if (_trayIcon){
        if (_conf->getShowTrayIcon())
        {
            _trayIcon->blockSignals(false);
            _trayIcon->setContextMenu(_trayMenu);
        }

    _trayIcon->setVisible(true);
    }
    QMainWindow::show();
}

bool MainWindow::isTrayed() const
{
    return _trayIcon != nullptr;
}


void MainWindow::setConfig(Config *config)
{
    _conf = config;
    updateUI();
}

void MainWindow::showOptions()
{
    ConfigDialog *options = new ConfigDialog(this);

    if (isMinimized())
    {
        showNormal();
        disableTrayMenuActions(true);
        if (options->exec() == QDialog::Accepted)
            updateUI();
        disableTrayMenuActions(false);
        if (_trayIcon) // the tray may have been removed
            windowHideShow(); // hides the window in this case
    }
    else
    {
        if (options->exec() == QDialog::Accepted)
            updateUI();
    }

    delete options;
}

void MainWindow::showAbout()
{
    const auto dialogs  = findChildren<QDialog*>();
    for (const auto &dialog : dialogs)
    {
        if (dialog->objectName() == QStringLiteral("aboutDialog"))
        {
            return;
        }
    }

    AboutDialog *about = new AboutDialog(this);

    if (isMinimized())
    {
        showNormal();
        disableTrayMenuActions(true);
        about->exec();
        disableTrayMenuActions(false);
        windowHideShow(); // hides the window in this case
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
    QString toolTip = tr("Screenshot ") + QString::number(w) + QStringLiteral("x") + QString::number(h);
    if (_conf->getEnableExtView())
    {
        toolTip += QLatin1String("\n\n");
        toolTip += tr("Double click to open screenshot in external default image viewer");
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
    _trayMenu->addAction(actAbout);
    _trayMenu->addSeparator();
    _trayMenu->addAction(actQuit);

    _trayIcon = new QSystemTrayIcon(this);

    _trayIcon->setContextMenu(_trayMenu);
    _trayIcon->setIcon(appIcon);
    _trayIcon->show();
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayClick);
}

void MainWindow::disableTrayMenuActions(bool disable)
{
    // On the one hand, QSystemTrayIcon::setContextMenu() can't be used for changing/removing
    // the menu. On the other hand, deleting/recreating the current menu isn't a good way of
    // disabling the context menu. Instead, we disable/enable menu ACTIONS when needed.
    if (_trayMenu)
    {
        const QList<QAction*> actions = _trayMenu->actions();
        for (QAction *action : actions)
            action->setDisabled(disable);
    }
}

void MainWindow::killTray()
{
    // the actions should be enabled because they're shared with the main window
    disableTrayMenuActions(false);

    _trayed = false;

    delete _trayMenu;
    _trayMenu = nullptr;

    delete _trayIcon;
    _trayIcon = nullptr;
}

void MainWindow::delayBoxChange(int delay)
{
    _conf->setDelay(delay);
}

void MainWindow::typeScreenShotChange(int type)
{
    if (QGuiApplication::platformName() == QStringLiteral("wayland"))
    {
        _ui->checkIncludeCursor->setVisible(true);
        if (type == 0)
            _conf->setDefScreenshotType(0); // fullscreen
        else if (type < 3)
            _conf->setDefScreenshotType(type + 1);
        _ui->checkNoDecoration->hide();
        _ui->checkZommMouseArea->hide();
    }
    else
    {
        if (type < 4)
            _conf->setDefScreenshotType(type);
        // show/hide checkboxes according to the type
        _ui->checkNoDecoration->setVisible(type == 1);
        _ui->checkIncludeCursor->setVisible(type < 2);
        _ui->checkZommMouseArea->setVisible(type >= 2);
    }
}

void MainWindow::checkIncludeCursor(bool include)
{
    _conf->setIncludeCursor(include);
}

void MainWindow::checkNoDecoration(bool noDecor)
{
    _conf->setNoDecoration(noDecor);
}

void MainWindow::checkZommMouseArea(bool zoom)
{
    _conf->setZoomAroundMouse(zoom);
}

// updating UI from configdata
void MainWindow::updateUI()
{
    _ui->delayBox->setValue(_conf->getDelay());

    int type = _conf->getDefScreenshotType();
    if (QGuiApplication::platformName() == QStringLiteral("wayland"))
    {
        _ui->checkIncludeCursor->setVisible(true);
        if (type < 2)
        {
            _ui->cbxTypeScr->setCurrentIndex(0);
        }
        else if (type < 4)
            _ui->cbxTypeScr->setCurrentIndex(type - 1);
        _ui->checkNoDecoration->hide();
        _ui->checkZommMouseArea->hide();

        // if the last screen should be remembered, set up the combo-box
        if (_conf->getRemLastScreen())
            _ui->cbxScr->setCurrentText(_conf->getScreen());
        // if the screen of the config file does not exist, correct it
        _conf->setScreen(_ui->cbxScr->currentText());
    }
    else
    {
        if (type < 4)
            _ui->cbxTypeScr->setCurrentIndex(type);
        // show/hide checkboxes according to the type
        _ui->checkNoDecoration->setVisible(type == 1);
        _ui->checkIncludeCursor->setVisible(type < 2);
        _ui->checkZommMouseArea->setVisible(type >= 2);
    }

    _ui->checkZommMouseArea->setChecked(_conf->getZoomAroundMouse());
    _ui->checkNoDecoration->setChecked(_conf->getNoDecoration());
    _ui->checkIncludeCursor->setChecked(_conf->getIncludeCursor());

    updateShortcuts();

    // create tray object, but not with the "-n" option
    if (!Core::instance()->noWin() && _conf->getShowTrayIcon() && !_trayIcon)
        createTray();

    // kill tray object, if tray was disabled in the configuration dialog
    if (!_conf->getShowTrayIcon() && _trayIcon)
        killTray();
}

// mouse clicks on tray icom
void MainWindow::trayClick(QSystemTrayIcon::ActivationReason reason)
{
    if (findChildren<QDialog*>().count() > 0)
    { // just activate the window when there's a dialog
        if (QGuiApplication::platformName() != QStringLiteral("wayland"))
        {
            activateWindow();
            raise();
        }
        return;
    }
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
    else if (findChildren<QDialog*>().isEmpty()) // do not hide with a modal dialog
    {
        actHideShow->setText(tr("Show"));
        showMinimized();
        hide();
        _trayed = true;
    }
}

void MainWindow::hideToShot()
{
    if (_trayIcon && _conf->getShowTrayIcon())
    {
        _trayIcon->blockSignals(true);
        disableTrayMenuActions(true);
    }

    hide();
}

void MainWindow::showWindow(const QString& str)
{
    // get char of type screen (last) form received string
    QString typeNum = str[str.size() - 1];
    int type = typeNum.toInt();

    // change screenshot type in config & on main window
    if (QGuiApplication::platformName() == QStringLiteral("wayland"))
    {
        if (type < 2)
        {
            _ui->cbxTypeScr->setCurrentIndex(0);
            typeScreenShotChange(0);
        }
        else if (type < 4)
        {
            _ui->cbxTypeScr->setCurrentIndex(type - 1);
            typeScreenShotChange(type - 1);
        }
    }
    else if (type < 4)
    {
        _ui->cbxTypeScr->setCurrentIndex(type);
        typeScreenShotChange(type);
    }
}

void MainWindow::restoreFromShot()
{
    if (_trayIcon && _conf->getShowTrayIcon())
    {
        _trayIcon->blockSignals(false);
        disableTrayMenuActions(false);
    }
    showNormal();
}

void MainWindow::saveScreen()
{
    bool wasMinimized(isMinimized());
    if (wasMinimized)
    {
        showNormal();
        disableTrayMenuActions(true);
    }

    // create initial filepath
    QHash<QString, QString> formatsAvalible;
    const QStringList formatIDs = _conf->getFormatIDs();
    if (formatIDs.isEmpty()) return;
    for (const QString &formatID : formatIDs)
        formatsAvalible[formatID] = tr("%1 Files").arg(formatID.toUpper());

    QString format = formatIDs.at(qBound(0, _conf->getDefaultFormatID(), formatIDs.size() - 1));

    Core* c = Core::instance();
    QString filePath = c->getSaveFilePath(format);

    QString filterSelected;

    // create file filters
    QStringList fileFilters;
    QHash<QString, QString>::const_iterator iter = formatsAvalible.constBegin();
    while (iter != formatsAvalible.constEnd())
    {
        QString str = iter.value() + QStringLiteral(" (*.") + iter.key() + QStringLiteral(")");
        if (iter.key() == format)
            filterSelected = str;
        fileFilters << str;
        ++iter;
    }

    QString fileName;
    fileName = QFileDialog::getSaveFileName(this, tr("Save As..."),  filePath, fileFilters.join(QStringLiteral(";;")), &filterSelected);

    static const QRegularExpression rx(QStringLiteral(R"(\(\*\.[a-z]{3,4}\))"));
    quint8 tmp = filterSelected.size() - filterSelected.indexOf(rx);

    filterSelected.chop(tmp + 1);
    format = formatsAvalible.key(filterSelected);

    if (wasMinimized)
    {
        disableTrayMenuActions(false);
        windowHideShow(); // hides the window in this case
    }

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
    actQuit->setShortcut(_conf->shortcuts()->getShortcut(Config::shortcutClose));
}
