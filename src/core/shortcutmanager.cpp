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

#include "shortcutmanager.h"
#include "src/core/config.h"

const QString DEF_SHORTCUT_NEW = "Ctrl+N";
const QString DEF_SHORTCUT_SAVE = "Ctrl+S";
const QString DEF_SHORTCUT_COPY = "Ctrl+C";
const QString DEF_SHORTCUT_OPT = "Ctrl+O";
const QString DEF_SHORTCUT_HELP = "F1";
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

ShortcutManager::ShortcutManager(QSettings *settings) :
    shortcutSettings(new QSettings)
{
    shortcutSettings = settings;

    for(int i = Config::shortcutFullScreen; i <= Config::shortcutHelp; ++i)
    {
	listShortcuts << Shortcut();
    }
}

ShortcutManager::~ShortcutManager()
{
    shortcutSettings = NULL;
    delete shortcutSettings;
}

void ShortcutManager::loadSettings()
{
    shortcutSettings->beginGroup("LocalShortcuts");
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_NEW, DEF_SHORTCUT_NEW).toString(),Config::shortcutNew, Config::localShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_SAVE, DEF_SHORTCUT_SAVE).toString(),Config::shortcutSave, Config::localShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_COPY, DEF_SHORTCUT_COPY).toString(),Config::shortcutCopy, Config::localShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_OPT, DEF_SHORTCUT_OPT).toString(),Config::shortcutOptions, Config::localShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_HELP, DEF_SHORTCUT_HELP).toString(),Config::shortcutHelp, Config::localShortcut);
    shortcutSettings->endGroup();

    shortcutSettings->beginGroup("GlobalShortcuts");
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_FULL, DEF_SHORTCUT_FULL).toString(),Config::shortcutFullScreen, Config::globalShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_ACTW, DEF_SHORTCUT_ACTW).toString(),Config::shortcutActiveWnd, Config::globalShortcut);
    setShortcut(shortcutSettings->value(KEY_SHORTCUT_AREA, DEF_SHORTCUT_AREA).toString(),Config::shortcutAreaSelect, Config::globalShortcut);
    shortcutSettings->endGroup();
}

void ShortcutManager::saveSettings()
{
    shortcutSettings->beginGroup("LocalShortcuts");
    shortcutSettings->setValue(KEY_SHORTCUT_NEW, getShortcut(Config::shortcutNew));
    shortcutSettings->setValue(KEY_SHORTCUT_SAVE, getShortcut(Config::shortcutSave));
    shortcutSettings->setValue(KEY_SHORTCUT_COPY, getShortcut(Config::shortcutCopy));
    shortcutSettings->setValue(KEY_SHORTCUT_OPT, getShortcut(Config::shortcutOptions));
    shortcutSettings->setValue(KEY_SHORTCUT_HELP, getShortcut(Config::shortcutHelp));
    shortcutSettings->endGroup();

    shortcutSettings->beginGroup("GlobalShortcuts");
    shortcutSettings->setValue(KEY_SHORTCUT_FULL, getShortcut(Config::shortcutFullScreen));
    shortcutSettings->setValue(KEY_SHORTCUT_ACTW, getShortcut(Config::shortcutActiveWnd));
    shortcutSettings->setValue(KEY_SHORTCUT_AREA, getShortcut(Config::shortcutAreaSelect));
    shortcutSettings->endGroup();
}

void ShortcutManager::setDefaultSettings()
{
    setShortcut(DEF_SHORTCUT_NEW,Config::shortcutNew, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_SAVE,Config::shortcutSave, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_COPY,Config::shortcutCopy, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_OPT,Config::shortcutOptions, Config::localShortcut);
    setShortcut(DEF_SHORTCUT_HELP,Config::shortcutHelp, Config::localShortcut);

    setShortcut(DEF_SHORTCUT_FULL,Config::shortcutFullScreen, Config::globalShortcut);
    setShortcut(DEF_SHORTCUT_ACTW,Config::shortcutActiveWnd, Config::globalShortcut);
    setShortcut(DEF_SHORTCUT_AREA,Config::shortcutAreaSelect, Config::globalShortcut);
}

void ShortcutManager::setShortcut(QString key, int action, int type)
{
    listShortcuts[action].key = key;
    listShortcuts[action].action = action;
    listShortcuts[action].type = type;
}

QKeySequence ShortcutManager::getShortcut(int action)
{
    return QKeySequence(listShortcuts[action].key);;
}

QStringList ShortcutManager::getShortcutsList(int type)
{
    QStringList retList;
    for (int i = Config::shortcutFullScreen; i <= Config::shortcutHelp; ++i)
    {
	if (listShortcuts[i].type == type)
	{
	    if (listShortcuts[i].key.isNull() != true)
	    {
		retList << listShortcuts[i].key;
	    }
	    else
	    {
		retList << QString("");
	    }
	}
    }
    return retList;
}
