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

#include "shortcutmanager.h"
#include "src/core/config.h"

const QString DEF_SHORTCUT_NEW = "Ctrl+N";
const QString DEF_SHORTCUT_SAVE = "Ctrl+S";
const QString DEF_SHORTCUT_COPY = "Ctrl+C";
const QString DEF_SHORTCUT_OPT = "Ctrl+P";
const QString DEF_SHORTCUT_HELP = "F1";
const QString DEF_SHORTCUT_CLOSE = "Esc";
const QString DEF_SHORTCUT_FULL = "";
const QString DEF_SHORTCUT_ACTW = "";
const QString DEF_SHORTCUT_AREA = "";

const QString KEY_SHORTCUT_FULL = "FullScreen";
const QString KEY_SHORTCUT_ACTW = "ActiveWindow";
const QString KEY_SHORTCUT_AREA = "AreaSelection";
const QString KEY_SHORTCUT_NEW = "NewScreen";
const QString KEY_SHORTCUT_SAVE = "SaveScreen";
const QString KEY_SHORTCUT_COPY = "CopyScreen";
const QString KEY_SHORTCUT_OPT = "Options";
const QString KEY_SHORTCUT_HELP = "Help";
const QString KEY_SHORTCUT_CLOSE = "Close";

ShortcutManager::ShortcutManager(QSettings *settings) :
    _shortcutSettings(new QSettings)
{
    _shortcutSettings = settings;

    for (int i = Config::shortcutFullScreen; i <= Config::shortcutClose; ++i)
        _listShortcuts << Shortcut();
}

ShortcutManager::~ShortcutManager()
{
    _shortcutSettings = NULL;
    delete _shortcutSettings;
}

void ShortcutManager::loadSettings()
{
    _shortcutSettings->beginGroup("LocalShortcuts");
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_NEW, DEF_SHORTCUT_NEW).toString(),
                Config::shortcutNew, Config::localShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_SAVE, DEF_SHORTCUT_SAVE).toString(),
                Config::shortcutSave, Config::localShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_COPY, DEF_SHORTCUT_COPY).toString(),
                Config::shortcutCopy, Config::localShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_OPT, DEF_SHORTCUT_OPT).toString(),
                Config::shortcutOptions, Config::localShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_HELP, DEF_SHORTCUT_HELP).toString(),
                Config::shortcutHelp, Config::localShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_CLOSE, DEF_SHORTCUT_CLOSE).toString(),
                Config::shortcutClose, Config::localShortcut);
    _shortcutSettings->endGroup();

    _shortcutSettings->beginGroup("GlobalShortcuts");
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_FULL, DEF_SHORTCUT_FULL).toString(),
                Config::shortcutFullScreen, Config::globalShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_ACTW, DEF_SHORTCUT_ACTW).toString(),
                Config::shortcutActiveWnd, Config::globalShortcut);
    setShortcut(_shortcutSettings->value(KEY_SHORTCUT_AREA, DEF_SHORTCUT_AREA).toString(),
                Config::shortcutAreaSelect, Config::globalShortcut);
    _shortcutSettings->endGroup();
}

void ShortcutManager::saveSettings()
{
    _shortcutSettings->beginGroup("LocalShortcuts");
    _shortcutSettings->setValue(KEY_SHORTCUT_NEW, getShortcut(Config::shortcutNew));
    _shortcutSettings->setValue(KEY_SHORTCUT_SAVE, getShortcut(Config::shortcutSave));
    _shortcutSettings->setValue(KEY_SHORTCUT_COPY, getShortcut(Config::shortcutCopy));
    _shortcutSettings->setValue(KEY_SHORTCUT_OPT, getShortcut(Config::shortcutOptions));
    _shortcutSettings->setValue(KEY_SHORTCUT_HELP, getShortcut(Config::shortcutHelp));
    _shortcutSettings->setValue(KEY_SHORTCUT_CLOSE, getShortcut(Config::shortcutClose));
    _shortcutSettings->endGroup();

    _shortcutSettings->beginGroup("GlobalShortcuts");
    _shortcutSettings->setValue(KEY_SHORTCUT_FULL, getShortcut(Config::shortcutFullScreen));
    _shortcutSettings->setValue(KEY_SHORTCUT_ACTW, getShortcut(Config::shortcutActiveWnd));
    _shortcutSettings->setValue(KEY_SHORTCUT_AREA, getShortcut(Config::shortcutAreaSelect));
    _shortcutSettings->endGroup();
}

void ShortcutManager::setDefaultSettings()
{
    setShortcut(DEF_SHORTCUT_NEW,Config::shortcutNew, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_SAVE,Config::shortcutSave, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_COPY,Config::shortcutCopy, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_OPT,Config::shortcutOptions, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_HELP,Config::shortcutHelp, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_CLOSE,Config::shortcutClose, Config::localShortcut);

    setShortcut(DEF_SHORTCUT_FULL,Config::shortcutFullScreen, Config::globalShortcut);
    setShortcut(DEF_SHORTCUT_ACTW,Config::shortcutActiveWnd, Config::globalShortcut);
    setShortcut(DEF_SHORTCUT_AREA,Config::shortcutAreaSelect, Config::globalShortcut);
}

void ShortcutManager::setShortcut(QString key, int action, int type)
{
    _listShortcuts[action].key = key;
    _listShortcuts[action].action = action;
    _listShortcuts[action].type = type;
}

QKeySequence ShortcutManager::getShortcut(int action)
{
    return QKeySequence(_listShortcuts[action].key);;
}

int ShortcutManager::getShortcutType(int action)
{
    return _listShortcuts[action].type;
}

QStringList ShortcutManager::getShortcutsList(int type)
{
    QStringList retList;
    for (int i = Config::shortcutFullScreen; i <= Config::shortcutClose; ++i)
    {
        if (_listShortcuts[i].type == type)
        {
            if (!_listShortcuts[i].key.isNull())
                retList << _listShortcuts[i].key;
            else
                retList << QString("");
        }
    }
    return retList;
}
