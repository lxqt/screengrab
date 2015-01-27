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

#include "extedit.h"
#include "core/core.h"

#include <QDebug>
#include <QMimeDatabase>

ExtEdit::ExtEdit(QObject *parent) :
    QObject(parent), _watcherEditedFile(new QFileSystemWatcher(this))
{
    createAppList();
    _fileIsChanged = false;
    connect(_watcherEditedFile, SIGNAL(fileChanged(QString)), this, SLOT(editedFileChanged(QString)));
}

QList<XdgAction*> ExtEdit::getActions()
{
    return _actionList;
}

void ExtEdit::runExternalEditor()
{
    XdgAction *action = static_cast<XdgAction*>(sender());

    Core *core = Core::instance();
    QString format = core->config()->getSaveFormat();
    if (format.isEmpty())
        format = "png";

    _editFilename = core->getTempFilename(format);
    core->writeScreen(_editFilename, format, true);

    QProcess *execProcess = new QProcess(this);
    connect(execProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(closedExternalEditor(int, QProcess::ExitStatus)));
     execProcess->start(action->desktopFile().expandExecString().first(),
                        QStringList() << _editFilename);
    _watcherEditedFile->addPath(_editFilename);
}

void ExtEdit::closedExternalEditor(int, QProcess::ExitStatus)
{
    Core *core = Core::instance();

    if (_fileIsChanged == true)
        core->updatePixmap();

    _fileIsChanged = false;
    _watcherEditedFile->removePath(_editFilename);

    sender()->deleteLater();
    core->killTempFile();
    _editFilename.clear();
}

void ExtEdit::editedFileChanged(const QString&)
{
    _fileIsChanged = true;
}

void ExtEdit::createAppList()
{
    Core *core = Core::instance();
    QString format = core->config()->getSaveFormat();
    if (format.isEmpty())
        format = "png";

    QString fileName = _editFilename.isEmpty() ? core->getTempFilename(format) : _editFilename;
    QMimeDatabase db;
    QMimeType mt = db.mimeTypeForFile(fileName);
    _appList = XdgDesktopFileCache::getApps(mt.name());

    foreach (XdgDesktopFile *app, _appList)
        _actionList << new XdgAction(app);
}
