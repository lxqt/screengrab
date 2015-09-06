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

#ifndef MODULEUPLOADER_H
#define MODULEUPLOADER_H

#include "modules/abstractmodule.h"

#include <QObject>
#include <QCommandLineOption>
#include <QWidget>

class ModuleUploader: public QObject, public AbstractModule
{
    Q_OBJECT
public:
    ModuleUploader(QObject *parent = 0);
    QString moduleName();
    QWidget* initConfigWidget();
    void defaultSettings();
    QMenu* initModuleMenu();
    QAction* initModuleAction();

public Q_SLOTS:
    void init();

private Q_SLOTS:
    void shadowUploadDone(const QString &directLink);
    void shadowUploadFail(const QByteArray &error);

Q_SIGNALS:
    void uploadCompleteWithQuit();

private:
    bool _ignoreCmdParam;
    QCommandLineOption _optUpload;
};

#endif // MODULEUPLOADER_H
