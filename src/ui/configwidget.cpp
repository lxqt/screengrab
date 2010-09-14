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
    on_defDelay_valueChanged(conf->getDefDelay());
    setVisibleDateTplEdit(conf->getDateTimeInFilename());    

    connect(m_ui->butSaveOpt, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(m_ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(selectDir()));
    connect(m_ui->butRestoreOpt, SIGNAL(clicked()), this, SLOT(restoreDefaults()) );
    connect(m_ui->checkIncDate, SIGNAL(toggled(bool)), this, SLOT(setVisibleDateTplEdit(bool)));
    connect(m_ui->keyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcut(QKeySequence)));
    connect(m_ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
    connect(m_ui->keyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    on_editDateTmeTpl_textEdited(conf->getDateTimeTpl());

    m_ui->treeKeys->expandAll();
    m_ui->treeKeys->header()->setResizeMode(QHeaderView::Stretch);
    
    // adding shortcut values in treewidget    
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
    on_cbxTrayMsg_currentIndexChanged(m_ui->cbxTrayMsg->currentIndex() );
    m_ui->checkSaveSize->setChecked(conf->getSavedSizeOnExit());
    m_ui->timeTrayMess->setValue(conf->getTimeTrayMess());
    m_ui->checkAutoSave->setChecked(conf->getAutoSave());;
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
    on_checkShowTray_toggled(conf->getShowTrayIcon());

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

void configwidget::on_butCancel_clicked()
{
    // rejecting changes
    reject();
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

void configwidget::on_defDelay_valueChanged(int )
{
    if (m_ui->defDelay->value() == 0 )
    {
        m_ui->defDelay->setSpecialValueText(tr( "None"));
    }
}

void configwidget::on_timeTrayMess_valueChanged(int )
{
    conf->setTimeTrayMess(m_ui->timeTrayMess->value());
}

void configwidget::on_cbxTrayMsg_currentIndexChanged(int index)
{
    switch(m_ui->cbxTrayMsg->currentIndex())
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



void configwidget::on_editDateTmeTpl_textEdited(QString str)
{
    QString currentDateTime = QDateTime::currentDateTime().toString(str );
    m_ui->labMaskExample->setText(tr("Example: ") + currentDateTime);
}


void configwidget::on_checkShowTray_toggled(bool checked)
{
    if (checked == true)
    {
        m_ui->labTrayMessages->setVisible(true);
        m_ui->cbxTrayMsg->setVisible(true);
        m_ui->timeTrayMess->setVisible(true);
        m_ui->labTimeTrayMess->setVisible(true);
        m_ui->checkInTray->setVisible(true);
    }
    else
    {
        m_ui->labTrayMessages->setVisible(false);
        m_ui->cbxTrayMsg->setVisible(false);
        m_ui->timeTrayMess->setVisible(false);
        m_ui->labTimeTrayMess->setVisible(false);
        m_ui->checkInTray->setVisible(false);
    }
}


void configwidget::on_treeKeys_clicked(QModelIndex index)
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

void configwidget::on_treeKeys_collapsed(QModelIndex index)
{
    if (index.parent().isValid() == false)
    {
        m_ui->labUsedShortcut->setVisible(false);
        m_ui->keyWidget->setVisible(false);
    }
}

void configwidget::on_treeKeys_expanded(QModelIndex index)
{
    m_ui->treeKeys->clearSelection();
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
