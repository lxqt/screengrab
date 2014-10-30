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

#ifndef EXTEDIT_H
#define EXTEDIT_H

#include <QObject>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QAction>

struct ExtApp_t {
	QString exec;
	QString name;
};

typedef QList<ExtApp_t> ExtAppsList_t;

class ExtEdit : public QObject
{
    Q_OBJECT
public:
    explicit ExtEdit(QObject *parent = 0);
    QStringList listAppNames();
    void addAppAction(QAction* act);

public Q_SLOTS:
    void runExternalEditor();

private Q_SLOTS:
	void closedExternalEditor(int exitCode, QProcess::ExitStatus exitStatus);
	void editedFileChanged(const QString & path);

private:
	void createAppList();
	ExtApp_t readDesktopFile(QString filename, QByteArray path);

	ExtAppsList_t _appList;
	QList<QAction*> _actionList;
	QString _editFilename;
	bool _fileIsCnaged;
	QFileSystemWatcher *_watcherEditedFile;
};

#endif // EXTEDIT_H
