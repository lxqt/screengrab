/***************************************************************************
 *   Copyright (C) 2009 by Artem 'DOOMer' Galichkin                        *
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

#include <QtGui/QKeyEvent>

#include "src/ui/configwidget.h"
#include "ui_configwidget.h"

#include <QxtGui/QxtGlobalShortcut>

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidgetItemIterator>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configwidget)
{
    ui->setupUi(this);
    conf = Config::instance();

    ui->tabWidget->setCurrentIndex(0);
    loadSettings();
    changeDefDelay(conf->getDefDelay());
    setVisibleDateTplEdit(conf->getDateTimeInFilename());

    setVisibleAutoSaveFirst(conf->getAutoSave());

    connect(ui->butSaveOpt, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(selectDir()));
    connect(ui->butRestoreOpt, SIGNAL(clicked()), this, SLOT(restoreDefaults()) );
    connect(ui->checkIncDate, SIGNAL(toggled(bool)), this, SLOT(setVisibleDateTplEdit(bool)));
    connect(ui->keyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcut(QKeySequence)));
    connect(ui->keyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(ui->checkAutoSave, SIGNAL(clicked(bool)), this, SLOT(setVisibleAutoSaveFirst(bool)));
    connect(ui->butCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(ui->treeKeys, SIGNAL(expanded(QModelIndex)), ui->treeKeys, SLOT(clearSelection()));
    connect(ui->treeKeys, SIGNAL(collapsed(QModelIndex)), this, SLOT(collapsTreeKeys(QModelIndex)));
    connect(ui->checkShowTray, SIGNAL(toggled(bool)), this, SLOT(toggleCheckShowTray(bool)));
    connect(ui->editDateTmeTpl, SIGNAL(textEdited(QString)), this, SLOT(editDateTmeTpl(QString)));
    connect(ui->defDelay, SIGNAL(valueChanged(int)), this, SLOT(changeDefDelay(int)));
    connect(ui->timeTrayMess, SIGNAL(valueChanged(int)), this, SLOT(changeTimeTrayMess(int)));
    connect(ui->cbxTrayMsg, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTrayMsgType(int)));
    connect(ui->treeKeys, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleclickTreeKeys(QModelIndex)));
    connect(ui->treeKeys, SIGNAL(activated(QModelIndex)), this, SLOT(doubleclickTreeKeys(QModelIndex)));
    connect(ui->treeKeys->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentItemChanged(const QModelIndex,const QModelIndex)));
    connect(ui->keyWidget, SIGNAL(keySequenceCleared()), this, SLOT(clearShrtcut()));

    editDateTmeTpl(conf->getDateTimeTpl());

    ui->treeKeys->expandAll();
    ui->treeKeys->header()->setResizeMode(QHeaderView::Stretch);

    // adding shortcut values in treewidge
    int action = 0;
    QTreeWidgetItemIterator iter(ui->treeKeys);
    while(*iter)
    {
	if ((*iter)->parent() != NULL)
	{
	    (*iter)->setData(1, Qt::DisplayRole, conf->shortcuts()->getShortcut(action));
	    ++action;
	}
	++iter;
    }

    // set false visibility to edit hokey controls
    ui->labUsedShortcut->setVisible(false);
    ui->keyWidget->setVisible(false);

}

ConfigDialog::~ConfigDialog()
{
    delete ui;
    conf = NULL;
    delete conf;

}


void ConfigDialog::loadSettings()
{
    // main tab
    ui->editDir->setText(conf->getSaveDir());
    ui->editFileName->setText(conf->getSaveFileName());

    ui->cbxFormat->addItem("png");
    ui->cbxFormat->addItem("jpg");
    ui->cbxFormat->addItem("bmp");
    ui->cbxFormat->setCurrentIndex(conf->getDefaultFormatID());

    ui->defDelay->setValue(conf->getDefDelay());
    ui->checkIncDate->setChecked(conf->getDateTimeInFilename());
    ui->editDateTmeTpl->setText(conf->getDateTimeTpl());

    // display tab
    ui->cbxTrayMsg->setCurrentIndex(conf->getTrayMessages());
    changeTrayMsgType(ui->cbxTrayMsg->currentIndex());
    ui->checkSaveSize->setChecked(conf->getSavedSizeOnExit());
    ui->timeTrayMess->setValue(conf->getTimeTrayMess());
    ui->checkAutoSave->setChecked(conf->getAutoSave());;
    ui->checkAutoSaveFirst->setChecked(conf->getAutoSaveFirst());;
    ui->checkZommMouseArea->setChecked(conf->getZoomAroundMouse());

    // integration tab
    ui->checkInTray->setChecked(conf->getCloseInTray());
    ui->checkAllowCopies->setChecked(conf->getAllowMultipleInstance());

#ifdef Q_WS_X11
    ui->checkNoDecorX11->setChecked(conf->getNoDecorX11());
#endif
#ifdef Q_WS_WIN
    ui->checkNoDecorX11->setVisible(false);
#endif
    ui->checkShowTray->setChecked(conf->getShowTrayIcon());
//     on_checkShowTray_toggled(conf->getShowTrayIcon());
    toggleCheckShowTray(conf->getShowTrayIcon());

}


void ConfigDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConfigDialog::setVisibleAutoSaveFirst(bool status)
{
    ui->checkAutoSaveFirst->setVisible(status);
}

void ConfigDialog::saveSettings()
{
    // set new values of general settings
    conf->setSaveDir(ui->editDir->text());
    conf->setSaveFileName(ui->editFileName->text());
    conf->setSaveFormat(ui->cbxFormat->currentText());
    conf->setDefDelay(ui->defDelay->value());
    conf->setDateTimeInFilename(ui->checkIncDate->isChecked());
    conf->setDateTimeTpl(ui->editDateTmeTpl->text());
    conf->setAutoSave(ui->checkAutoSave->isChecked());
    conf->setAutoSaveFirst(ui->checkAutoSaveFirst->isChecked());
    conf->setTrayMessages(ui->cbxTrayMsg->currentIndex());
    conf->setCloseInTray(ui->checkInTray->isChecked());
    conf->setZoomAroundMouse(ui->checkZommMouseArea->isChecked());
    conf->setAllowMultipleInstance(ui->checkAllowCopies->isChecked());
    conf->setSavedSizeOnExit(ui->checkSaveSize->isChecked());
    conf->setTimeTrayMess(ui->timeTrayMess->value());
    conf->setShowTrayIcon(ui->checkShowTray->isChecked());
#ifdef Q_WS_X11
    conf->setNoDecorX11(ui->checkNoDecorX11->isChecked());
#endif
    // save shortcuts in shortcutmanager
    int action = 0;
    QTreeWidgetItemIterator iter(ui->treeKeys);
    while(*iter)
    {
	if ((*iter)->parent() != NULL)
	{
	    switch((*iter)->parent()->childCount())
	    {
		case 3:
		    conf->shortcuts()->setShortcut((*iter)->data(1, Qt::DisplayRole).toString(), action, 0);
		    break;
		case 5:
		    conf->shortcuts()->setShortcut((*iter)->data(1, Qt::DisplayRole).toString(), action, 1);
		    break;
		default:
		    break;
	    }
	    ++action;
	}
	++iter;
    }

    // update values of front-end settings
    conf->saveSettings();
    conf->setDelay(conf->getDefDelay());
    // accep changes
    accept();
}


QString ConfigDialog::getFormat()
{
    switch (ui->cbxFormat->currentIndex())
    {
        case 0: return "png";
        case 1: return "jpg";
        default: return "png";
    }
}

void ConfigDialog::selectDir()
{
QString *directory;
directory = new QString;
#ifdef Q_WS_X11
{
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             ui->editDir->text(), QFileDialog::ShowDirsOnly)+QDir::separator();
    if (directory->toUtf8() != QDir::separator())
    {
        ui->editDir->setText( *directory);
    }
}
#endif
#ifdef Q_WS_WIN
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             ui->editDir->text(), QFileDialog::ShowDirsOnly)+ "/";
    if (directory->toUtf8() != "/")
    {
        ui->editDir->setText( QDir::toNativeSeparators(*directory));
    }
#endif
    delete directory;
}

void ConfigDialog::restoreDefaults()
{
    conf->setDefaultSettings();
    conf->saveSettings();

    // show inf message
    QMessageBox::information(this, tr("Message"), tr("Settings will be restored to default values!"), QMessageBox::Ok );

    // close config window with accepting
    accept();
}

void ConfigDialog::changeDefDelay(int val)
{
    if (val == 0 )
    {
	ui->defDelay->setSpecialValueText(tr( "None"));
    }
}

void ConfigDialog::changeTimeTrayMess(int sec)
{
    conf->setTimeTrayMess(sec);
}

void ConfigDialog::changeTrayMsgType(int type)
{
    switch(type)
    {
	case 0:
	{
	    ui->labTimeTrayMess->setVisible(false);
	    ui->timeTrayMess->setVisible(false);
	    break;
	}
	default:
	{
	    ui->labTimeTrayMess->setVisible(true);
	    ui->timeTrayMess->setVisible(true);
	    break;
	}
    }
}

void ConfigDialog::setVisibleDateTplEdit(bool checked)
{
        if (checked == false)
        {
            ui->editDateTmeTpl->setVisible(false);
            ui->labMask->setVisible(false);
            ui->labMaskExample->setVisible(false);
        }
        else
        {
            ui->editDateTmeTpl->setVisible(true);
            ui->labMask->setVisible(true);
            ui->labMaskExample->setVisible(true);
        }
}


void ConfigDialog::editDateTmeTpl(QString str)
{
    QString currentDateTime = QDateTime::currentDateTime().toString(str );
    ui->labMaskExample->setText(tr("Example: ") + currentDateTime);
}

void ConfigDialog::toggleCheckShowTray(bool checked)
{
    ui->labTrayMessages->setVisible(checked);
    ui->cbxTrayMsg->setVisible(checked);
    ui->timeTrayMess->setVisible(checked);
    ui->labTimeTrayMess->setVisible(checked);
    ui->checkInTray->setVisible(checked);
}

// void ConfigDialog::currentItemChanged(QTreeWidgetItem* c, QTreeWidgetItem* p)
// {
//     QKeySequence ks("");
//     qDebug() << "c->parent() " << c->parent();
//     qDebug() << "p " << p << "  " << c->data(1, Qt::DisplayRole).toString();
//     if (c->parent() != NULL)
//     {
// 	ui->labUsedShortcut->setVisible(true);
//     	ui->keyWidget->setVisible(true);
//
//     	ui->keyWidget->setKeySequence(QKeySequence(c->data(1, Qt::DisplayRole).toString()));
//     }
//     else
//     {
// 	ui->labUsedShortcut->setVisible(false);
//     	ui->keyWidget->setVisible(false);
//     }
// }

void ConfigDialog::currentItemChanged(const QModelIndex c, const QModelIndex p)
{
    qDebug() << c.parent() ;
    if (c.parent().isValid() == true)
    {
	ui->labUsedShortcut->setVisible(true);
	ui->keyWidget->setVisible(true);

	qDebug() << "ui->treeKeys " << ui->treeKeys->currentItem();
	QTreeWidgetItem *item = ui->treeKeys->currentItem();
	qDebug() << "get key str " << item->data(1, Qt::DisplayRole).toString();
	ui->keyWidget->setKeySequence(QKeySequence(item->data(1, Qt::DisplayRole).toString()));
    }
    else
    {
	ui->labUsedShortcut->setVisible(false);
	ui->keyWidget->setVisible(false);
    }
}


void ConfigDialog::doubleclickTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == true)
    {
	connect(ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
	ui->keyWidget->captureKeySequence();
    }
}


void ConfigDialog::collapsTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == false)
    {
	ui->labUsedShortcut->setVisible(false);
	ui->keyWidget->setVisible(false);
    }
}

void ConfigDialog::acceptShortcut(const QKeySequence& seq)
{
    if (checkUsedShortcuts() == false)
    {
	// iterate for
	if (avalibelGlobalShortcuts(seq) == true)
	{
	    changeShortcut(seq);
	}
	else
	{
	    QMessageBox msg;
	    msg.setWindowTitle(tr("Error"));
	    msg.setText(tr("This key is exist in global shortcuts on yoy system! Please select other keys"));
	    msg.setIcon(QMessageBox::Information);
	    msg.setStandardButtons(QMessageBox::Ok);
	    msg.exec();
	    ui->keyWidget->clearKeySequence();
	}
    }
    else if (checkUsedShortcuts() == true && seq.toString() != "")
    {
	QMessageBox msg;
	msg.setWindowTitle(tr("Error"));
	msg.setText(tr("This key is exist! Please select other keys"));
	msg.setIcon(QMessageBox::Information);
	msg.setStandardButtons(QMessageBox::Ok);
	msg.exec();
	ui->keyWidget->clearKeySequence();
    }
}

void ConfigDialog::changeShortcut(const QKeySequence& seq)
{
    qDebug() << "---- " << ui->treeKeys->selectedItems().first();
    disconnect(ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
    QTreeWidgetItem *item = ui->treeKeys->selectedItems().first();
    item->setData(1, Qt::DisplayRole, seq.toString());
}

void ConfigDialog::clearShrtcut()
{
    qDebug() << "clear slot";
    QTreeWidgetItem *item = ui->treeKeys->selectedItems().first();
    item->setData(1, Qt::DisplayRole, QString(""));
}


void ConfigDialog::keyNotSupported()
{
    QMessageBox msg;
    msg.setWindowTitle(tr("Error"));
    msg.setText(tr("This key is not supported on your system!"));
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

bool ConfigDialog::checkUsedShortcuts()
{
    QTreeWidgetItem *item = ui->treeKeys->selectedItems().first();
    QTreeWidgetItemIterator iter(ui->treeKeys);
    while (*iter)
    {
	if ((*iter) != item && (*iter)->data(1, Qt::DisplayRole) == ui->keyWidget->keySequence().toString())
	{
	    return true;
	}
	++iter;
    }
    return false;
}

bool ConfigDialog::avalibelGlobalShortcuts(const QKeySequence& seq)
{
    bool ok = false;
    QxtGlobalShortcut tmpShortcut;
    if (tmpShortcut.setShortcut(QKeySequence(seq)) == true)
    {
	tmpShortcut.setDisabled(true);
	ok = true;
    }
    return ok;
}
