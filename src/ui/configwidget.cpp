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

configwidget::configwidget(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::configwidget)
{
    m_ui->setupUi(this);
    conf = Config::instance();

    m_ui->tabWidget->setCurrentIndex(0);
    loadSettings();
    changeDefDelay(conf->getDefDelay());
    setVisibleDateTplEdit(conf->getDateTimeInFilename());

    setVisibleAutoSaveFirst(conf->getAutoSave());

    connect(m_ui->butSaveOpt, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(m_ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(selectDir()));
    connect(m_ui->butRestoreOpt, SIGNAL(clicked()), this, SLOT(restoreDefaults()) );
    connect(m_ui->checkIncDate, SIGNAL(toggled(bool)), this, SLOT(setVisibleDateTplEdit(bool)));
    connect(m_ui->keyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcut(QKeySequence)));
    connect(m_ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
    connect(m_ui->keyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(m_ui->checkAutoSave, SIGNAL(clicked(bool)), this, SLOT(setVisibleAutoSaveFirst(bool)));
    connect(m_ui->butCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(m_ui->treeKeys, SIGNAL(expanded(QModelIndex)), m_ui->treeKeys, SLOT(clearSelection()));
    connect(m_ui->treeKeys, SIGNAL(collapsed(QModelIndex)), this, SLOT(collapsTreeKeys(QModelIndex)));
    connect(m_ui->treeKeys, SIGNAL(clicked(QModelIndex)), this, SLOT(clickTreeKeys(QModelIndex)));
    connect(m_ui->checkShowTray, SIGNAL(toggled(bool)), this, SLOT(toggleCheckShowTray(bool)));
    connect(m_ui->editDateTmeTpl, SIGNAL(textEdited(QString)), this, SLOT(editDateTmeTpl(QString)));
    connect(m_ui->defDelay, SIGNAL(valueChanged(int)), this, SLOT(changeDefDelay(int)));
    connect(m_ui->timeTrayMess, SIGNAL(valueChanged(int)), this, SLOT(changeTimeTrayMess(int)));
    connect(m_ui->cbxTrayMsg, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTrayMsgType(int)));

    editDateTmeTpl(conf->getDateTimeTpl());

    m_ui->treeKeys->expandAll();
    m_ui->treeKeys->header()->setResizeMode(QHeaderView::Stretch);

    // adding shortcut values in treewidge
    int action = 0;
    QTreeWidgetItemIterator iter(m_ui->treeKeys);
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
    m_ui->labUsedShortcut->setVisible(false);
    m_ui->keyWidget->setVisible(false);

}

configwidget::~configwidget()
{
    delete m_ui;
    conf = NULL;
    delete conf;

}


void configwidget::loadSettings()
{
    // main tab
    m_ui->editDir->setText(conf->getSaveDir());
    m_ui->editFileName->setText(conf->getSaveFileName());

    m_ui->cbxFormat->addItem("png");
    m_ui->cbxFormat->addItem("jpg");
    m_ui->cbxFormat->addItem("bmp");
    m_ui->cbxFormat->setCurrentIndex(conf->getDefaultFormatID());

    m_ui->defDelay->setValue(conf->getDefDelay());
    m_ui->checkIncDate->setChecked(conf->getDateTimeInFilename());
    m_ui->editDateTmeTpl->setText(conf->getDateTimeTpl());

    // display tab
    m_ui->cbxTrayMsg->setCurrentIndex(conf->getTrayMessages());
    changeTrayMsgType(m_ui->cbxTrayMsg->currentIndex());
//     on_cbxTrayMsg_currentIndexChanged(m_ui->cbxTrayMsg->currentIndex() );
    m_ui->checkSaveSize->setChecked(conf->getSavedSizeOnExit());
    m_ui->timeTrayMess->setValue(conf->getTimeTrayMess());
    m_ui->checkAutoSave->setChecked(conf->getAutoSave());;
    m_ui->checkAutoSaveFirst->setChecked(conf->getAutoSaveFirst());;
    m_ui->checkZommMouseArea->setChecked(conf->getZoomAroundMouse());

    // integration tab
    m_ui->checkInTray->setChecked(conf->getCloseInTray());
    m_ui->checkAllowCopies->setChecked(conf->getAllowMultipleInstance());

#ifdef Q_WS_X11
    m_ui->checkNoDecorX11->setChecked(conf->getNoDecorX11());
#endif
#ifdef Q_WS_WIN
    m_ui->checkNoDecorX11->setVisible(false);
#endif
    m_ui->checkShowTray->setChecked(conf->getShowTrayIcon());
//     on_checkShowTray_toggled(conf->getShowTrayIcon());
    toggleCheckShowTray(conf->getShowTrayIcon());

}


void configwidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void configwidget::setVisibleAutoSaveFirst(bool status)
{
    m_ui->checkAutoSaveFirst->setVisible(status);
}

void configwidget::saveSettings()
{
    // set new values of general settings
    conf->setSaveDir(m_ui->editDir->text());
    conf->setSaveFileName(m_ui->editFileName->text());
    conf->setSaveFormat(m_ui->cbxFormat->currentText());
    conf->setDefDelay(m_ui->defDelay->value());
    conf->setDateTimeInFilename(m_ui->checkIncDate->isChecked());
    conf->setDateTimeTpl(m_ui->editDateTmeTpl->text());
    conf->setAutoSave(m_ui->checkAutoSave->isChecked());
    conf->setAutoSaveFirst(m_ui->checkAutoSaveFirst->isChecked());
    conf->setTrayMessages(m_ui->cbxTrayMsg->currentIndex());
    conf->setCloseInTray(m_ui->checkInTray->isChecked());
    conf->setZoomAroundMouse(m_ui->checkZommMouseArea->isChecked());
    conf->setAllowMultipleInstance(m_ui->checkAllowCopies->isChecked());
    conf->setSavedSizeOnExit(m_ui->checkSaveSize->isChecked());
    conf->setTimeTrayMess(m_ui->timeTrayMess->value());
    conf->setShowTrayIcon(m_ui->checkShowTray->isChecked());
#ifdef Q_WS_X11
    conf->setNoDecorX11(m_ui->checkNoDecorX11->isChecked());
#endif
    // save shortcuts in shortcutmanager
    int action = 0;
    QTreeWidgetItemIterator iter(m_ui->treeKeys);
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


QString configwidget::getFormat()
{
    switch (m_ui->cbxFormat->currentIndex())
    {
        case 0: return "png";
        case 1: return "jpg";
        default: return "png";
    }
}

void configwidget::selectDir()
{
QString *directory;
directory = new QString;
#ifdef Q_WS_X11
{
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             m_ui->editDir->text(), QFileDialog::ShowDirsOnly)+QDir::separator();
    if (directory->toUtf8() != QDir::separator())
    {
        m_ui->editDir->setText( *directory);
    }
}
#endif
#ifdef Q_WS_WIN
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             m_ui->editDir->text(), QFileDialog::ShowDirsOnly)+ "/";
    if (directory->toUtf8() != "/")
    {
        m_ui->editDir->setText( QDir::toNativeSeparators(*directory));
    }
#endif
    delete directory;
}

void configwidget::restoreDefaults()
{
    conf->setDefaultSettings();
    conf->saveSettings();

    // show inf message
    QMessageBox::information(this, tr("Message"), tr("Settings will be restored to default values!"), QMessageBox::Ok );

    // close config window with accepting
    accept();
}

void configwidget::changeDefDelay(int val)
{
    if (val == 0 )
    {
	m_ui->defDelay->setSpecialValueText(tr( "None"));
    }
}

void configwidget::changeTimeTrayMess(int sec)
{
    conf->setTimeTrayMess(sec);
}

void configwidget::changeTrayMsgType(int type)
{
    switch(type)
    {
	case 0:
	{
	    m_ui->labTimeTrayMess->setVisible(false);
	    m_ui->timeTrayMess->setVisible(false);
	    break;
	}
	default:
	{
	    m_ui->labTimeTrayMess->setVisible(true);
	    m_ui->timeTrayMess->setVisible(true);
	    break;
	}
    }
}

void configwidget::setVisibleDateTplEdit(bool checked)
{
        if (checked == false)
        {
            m_ui->editDateTmeTpl->setVisible(false);
            m_ui->labMask->setVisible(false);
            m_ui->labMaskExample->setVisible(false);
        }
        else
        {
            m_ui->editDateTmeTpl->setVisible(true);
            m_ui->labMask->setVisible(true);
            m_ui->labMaskExample->setVisible(true);
        }
}


void configwidget::editDateTmeTpl(QString str)
{
    QString currentDateTime = QDateTime::currentDateTime().toString(str );
    m_ui->labMaskExample->setText(tr("Example: ") + currentDateTime);
}

void configwidget::toggleCheckShowTray(bool checked)
{
    m_ui->labTrayMessages->setVisible(checked);
    m_ui->cbxTrayMsg->setVisible(checked);
    m_ui->timeTrayMess->setVisible(checked);
    m_ui->labTimeTrayMess->setVisible(checked);
    m_ui->checkInTray->setVisible(checked);
}

void configwidget::clickTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == true)
    {
	m_ui->labUsedShortcut->setVisible(true);
	m_ui->keyWidget->setVisible(true);
	//QTreeWidgetItem item =
	QTreeWidgetItem *item = m_ui->treeKeys->selectedItems().first();
	m_ui->keyWidget->setKeySequence(QKeySequence(item->data(1, Qt::DisplayRole).toString()));

    }
    else
    {
	m_ui->labUsedShortcut->setVisible(false);
	m_ui->keyWidget->setVisible(false);
    }
}

void configwidget::collapsTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == false)
    {
	m_ui->labUsedShortcut->setVisible(false);
	m_ui->keyWidget->setVisible(false);
    }
}

void configwidget::acceptShortcut(const QKeySequence& seq)
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
	    m_ui->keyWidget->clearKeySequence();
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
	m_ui->keyWidget->clearKeySequence();
    }
}

void configwidget::changeShortcut(const QKeySequence& seq)
{
    QTreeWidgetItem *item = m_ui->treeKeys->selectedItems().first();
    item->setData(1, Qt::DisplayRole, seq.toString());
}


void configwidget::keyNotSupported()
{
    QMessageBox msg;
    msg.setWindowTitle(tr("Error"));
    msg.setText(tr("This key is not supported on your system!"));
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}

bool configwidget::checkUsedShortcuts()
{
    QTreeWidgetItem *item = m_ui->treeKeys->selectedItems().first();
    QTreeWidgetItemIterator iter(m_ui->treeKeys);
    while (*iter)
    {
	if ((*iter) != item && (*iter)->data(1, Qt::DisplayRole) == m_ui->keyWidget->keySequence().toString())
	{
	    return true;
	}
	++iter;
    }
    return false;
}

bool configwidget::avalibelGlobalShortcuts(const QKeySequence& seq)
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
