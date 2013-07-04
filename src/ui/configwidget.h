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

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "src/core/config.h"

#include <QtGui/QDialog>
#include <QtCore/QTextCodec>
#include <QtCore/QDateTime>
#include <QtCore/QModelIndex>
#include <QtGui/QTreeWidgetItem>

namespace Ui {
    class configwidget;
}
// , public Config
class ConfigDialog : public QDialog{
    Q_OBJECT
public:
    ConfigDialog( QWidget *parent = 0);
    ~ConfigDialog();
    Config *conf;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::configwidget *_ui;
    void loadSettings();
    QString getFormat();
    bool checkUsedShortcuts();    
    void showErrorMessage(QString text);
	
	QStringList _moduleWidgetNames;
	
#ifdef SG_GLOBAL_SHORTCUTS
    bool avalibelGlobalShortcuts(const QKeySequence& seq);
#endif

private slots:
    void collapsTreeKeys(QModelIndex index);
    void doubleclickTreeKeys(QModelIndex index);
    void toggleCheckShowTray(bool checked);
    void currentItemChanged(const QModelIndex c ,const QModelIndex p);
    void editDateTmeTpl(QString str);
    void setVisibleDateTplEdit(bool);
    void changeTrayMsgType(int type);
    void changeTimeTrayMess(int sec);
    void changeDefDelay(int val);
    void setVisibleAutoSaveFirst(bool status);
    void changeFormatType(int type);
    void changeImgQualituSlider(int pos);
    void saveSettings();
    void selectDir();
    void restoreDefaults();
    void acceptShortcut(const QKeySequence &seq);
    void changeShortcut(const QKeySequence &seq);
    void clearShrtcut();
    void keyNotSupported();

};

#endif // CONFIGWIDGET_H
