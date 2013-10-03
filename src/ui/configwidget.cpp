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

#include <QtGui/QKeyEvent>

#include "src/ui/configwidget.h"
#include "ui_configwidget.h"

#ifdef SG_GLOBAL_SHORTCUTS
#include <QxtGui/QxtGlobalShortcut>
#include <src/core/core.h>
#endif

#include <QtCore/QDir>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidgetItemIterator>
// #include <X11/Xlib.h>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::configwidget)
{
    _ui->setupUi(this);
    conf = Config::instance();

    connect(_ui->butSaveOpt, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(_ui->buttonBrowse, SIGNAL(clicked()), this, SLOT(selectDir()));
    connect(_ui->butRestoreOpt, SIGNAL(clicked()), this, SLOT(restoreDefaults()) );
    connect(_ui->checkIncDate, SIGNAL(toggled(bool)), this, SLOT(setVisibleDateTplEdit(bool)));
    connect(_ui->keyWidget, SIGNAL(keySequenceAccepted(QKeySequence)), this, SLOT(acceptShortcut(QKeySequence)));
    connect(_ui->keyWidget, SIGNAL(keyNotSupported()), this, SLOT(keyNotSupported()));
    connect(_ui->checkAutoSave, SIGNAL(clicked(bool)), this, SLOT(setVisibleAutoSaveFirst(bool)));
    connect(_ui->butCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));
    connect(_ui->treeKeys, SIGNAL(expanded(QModelIndex)), _ui->treeKeys, SLOT(clearSelection()));
    connect(_ui->treeKeys, SIGNAL(collapsed(QModelIndex)), this, SLOT(collapsTreeKeys(QModelIndex)));
    connect(_ui->checkShowTray, SIGNAL(toggled(bool)), this, SLOT(toggleCheckShowTray(bool)));
    connect(_ui->editDateTmeTpl, SIGNAL(textEdited(QString)), this, SLOT(editDateTmeTpl(QString)));
    connect(_ui->defDelay, SIGNAL(valueChanged(int)), this, SLOT(changeDefDelay(int)));
    connect(_ui->timeTrayMess, SIGNAL(valueChanged(int)), this, SLOT(changeTimeTrayMess(int)));
    connect(_ui->cbxTrayMsg, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTrayMsgType(int)));
    connect(_ui->treeKeys, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleclickTreeKeys(QModelIndex)));
    connect(_ui->treeKeys, SIGNAL(activated(QModelIndex)), this, SLOT(doubleclickTreeKeys(QModelIndex)));
    connect(_ui->treeKeys->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentItemChanged(const QModelIndex,const QModelIndex)));
    connect(_ui->keyWidget, SIGNAL(keySequenceCleared()), this, SLOT(clearShrtcut()));
    connect(_ui->listWidget, SIGNAL(currentRowChanged(int)), _ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(_ui->slideImgQuality, SIGNAL(valueChanged(int)), this, SLOT(changeImgQualituSlider(int)));
    connect(_ui->cbxFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFormatType(int)));

    loadSettings();
    changeDefDelay(conf->getDefDelay());
    setVisibleDateTplEdit(conf->getDateTimeInFilename());
    
    setVisibleAutoSaveFirst(conf->getAutoSave());
    
    _ui->listWidget->setCurrentRow(0);
	_ui->tabMain->setCurrentIndex(0);
    
    editDateTmeTpl(conf->getDateTimeTpl());

    _ui->treeKeys->expandAll();
    _ui->treeKeys->header()->setResizeMode(QHeaderView::Stretch);

    // adding shortcut values in treewidge
    int action = 0;
    QTreeWidgetItemIterator iter(_ui->treeKeys);
    while(*iter)
    {
        if ((*iter)->parent() != NULL)
        {
            (*iter)->setData(1, Qt::DisplayRole, conf->shortcuts()->getShortcut(action));        
            
#ifndef SG_GLOBAL_SHORTCUTS
            if (conf->shortcuts()->getShortcutType(action) == Config::globalShortcut)
            {
                (*iter)->setHidden(true);
            }
#endif
            ++action;        
        }
        else
        {
#ifndef SG_GLOBAL_SHORTCUTS
            int numGlobalShortcuts = conf->shortcuts()->getShortcutsList(Config::globalShortcut).count();
            if ((*iter)->childCount() == numGlobalShortcuts)
            {
                (*iter)->setHidden(true);
            }
#endif
        }
    ++iter;
    }

    // set false visibility to edit hokey controls
    _ui->labUsedShortcut->setVisible(false);
    _ui->keyWidget->setVisible(false);

	// Load config widgets for modules
	quint8 countModules = Core::instance()->modules()->count();
	
	for (int i = 0; i < countModules; ++i)
	{
		AbstractModule* currentModule = Core::instance()->modules()->getModule(i);
		
		if (currentModule->initConfigWidget() != 0)
		{
			_ui->listWidget->addItem(currentModule->moduleName());
			QWidget *currentModWidget = currentModule->initConfigWidget();
			_ui->stackedWidget->addWidget(currentModWidget);	
			_moduleWidgetNames << currentModWidget->objectName();
		}				
	}
}

ConfigDialog::~ConfigDialog()
{
    delete _ui;
    conf = NULL;
    delete conf;

}


void ConfigDialog::loadSettings()
{
    // main tab
    _ui->editDir->setText(conf->getSaveDir());
    _ui->editFileName->setText(conf->getSaveFileName());

    _ui->cbxFormat->addItem("png");
    _ui->cbxFormat->addItem("jpg");
    _ui->cbxFormat->addItem("bmp");
    _ui->cbxFormat->setCurrentIndex(conf->getDefaultFormatID());

    _ui->defDelay->setValue(conf->getDefDelay());
    _ui->checkIncDate->setChecked(conf->getDateTimeInFilename());
    _ui->editDateTmeTpl->setText(conf->getDateTimeTpl());
    _ui->cbxCopyFileName->setCurrentIndex(conf->getAutoCopyFilenameOnSaving());

    // display tab
    _ui->cbxTrayMsg->setCurrentIndex(conf->getTrayMessages());
    changeTrayMsgType(_ui->cbxTrayMsg->currentIndex());
    _ui->timeTrayMess->setValue(conf->getTimeTrayMess());
    _ui->checkAutoSave->setChecked(conf->getAutoSave());;
    _ui->checkAutoSaveFirst->setChecked(conf->getAutoSaveFirst());;
    _ui->checkZommMouseArea->setChecked(conf->getZoomAroundMouse());

    // integration tab
    _ui->checkInTray->setChecked(conf->getCloseInTray());
    _ui->checkAllowCopies->setChecked(conf->getAllowMultipleInstance());

#ifdef Q_WS_X11
    _ui->checkNoDecorX11->setChecked(conf->getNoDecorX11());
#endif
#ifdef Q_WS_WIN
    _ui->checkNoDecorX11->setVisible(false);
#endif
    _ui->checkShowTray->setChecked(conf->getShowTrayIcon());
//     on_checkShowTray_toggled(conf->getShowTrayIcon());
    toggleCheckShowTray(conf->getShowTrayIcon());

    _ui->slideImgQuality->setValue(conf->getImageQuality());
	_ui->cbxEnableExtView->setChecked(conf->getEnableExtView());
}


void ConfigDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ConfigDialog::setVisibleAutoSaveFirst(bool status)
{
    _ui->checkAutoSaveFirst->setVisible(status);
}

void ConfigDialog::changeFormatType(int type)
{
	if (type == 1)
	{
		_ui->slideImgQuality->setVisible(true);;
		_ui->labImgQuality->setVisible(true);
		_ui->labImgQualityCurrent->setVisible(true);;
	}
	else
	{
		_ui->slideImgQuality->setVisible(false);
		_ui->labImgQuality->setVisible(false);
		_ui->labImgQualityCurrent->setVisible(false);;
	}
}


void ConfigDialog::changeImgQualituSlider(int pos)
{
    QString text = " " + QString::number(pos) + "%";
    _ui->labImgQualityCurrent->setText(text);
}

void ConfigDialog::saveSettings()
{
    QDir screenshotDir(_ui->editDir->text());
    if (screenshotDir.exists() == false)
    {
        QMessageBox msg;
        msg.setText(tr("Directory %1 does not exist. Do you want to create it?").arg(QDir::toNativeSeparators(screenshotDir.path()) + QDir::separator()));
        msg.setWindowTitle("ScreenGrab" + QString(" - ") + tr("Warning"));
        msg.setIcon(QMessageBox::Question);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        
        int res = msg.exec();
        
        if (res == QMessageBox::No)
        {
            return ;
        }
        else
        {            
            screenshotDir.mkpath(screenshotDir.path());
            
            if (screenshotDir.path().endsWith(QDir::separator()) == false)
            {
                QString updatedPath = screenshotDir.path() + QDir::separator();
                updatedPath = QDir::toNativeSeparators(updatedPath);
                _ui->editDir->setText(updatedPath);
            }
        }
    }

    // set new values of general settings
    conf->setSaveDir(_ui->editDir->text());
    conf->setSaveFileName(_ui->editFileName->text());
    conf->setSaveFormat(_ui->cbxFormat->currentText());
    conf->setDefDelay(_ui->defDelay->value());
    conf->setDateTimeInFilename(_ui->checkIncDate->isChecked());
    conf->setDateTimeTpl(_ui->editDateTmeTpl->text());
    conf->setAutoCopyFilenameOnSaving(_ui->cbxCopyFileName->currentIndex());
    conf->setAutoSave(_ui->checkAutoSave->isChecked());
    conf->setAutoSaveFirst(_ui->checkAutoSaveFirst->isChecked());
    conf->setTrayMessages(_ui->cbxTrayMsg->currentIndex());
    conf->setCloseInTray(_ui->checkInTray->isChecked());
    conf->setZoomAroundMouse(_ui->checkZommMouseArea->isChecked());
    conf->setAllowMultipleInstance(_ui->checkAllowCopies->isChecked());
    conf->setTimeTrayMess(_ui->timeTrayMess->value());
    conf->setShowTrayIcon(_ui->checkShowTray->isChecked());
    conf->setImageQuality(_ui->slideImgQuality->value());
	conf->setEnableExtView(_ui->cbxEnableExtView->isChecked());
#ifdef Q_WS_X11
    conf->setNoDecorX11(_ui->checkNoDecorX11->isChecked());
#endif
    // save shortcuts in shortcutmanager
    int action = 0;
    QTreeWidgetItemIterator iter(_ui->treeKeys);
    while(*iter)
    {
    if ((*iter)->parent() != NULL)
    {
        switch((*iter)->parent()->childCount())
        {
        case 3:
            conf->shortcuts()->setShortcut((*iter)->data(1, Qt::DisplayRole).toString(), action, 0);
            break;
        case 6:
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
	
	// call save method on modeule's configwidgets'	
	for (int i = 0; i < _moduleWidgetNames.count(); ++i)
	{
		QString name = _moduleWidgetNames.at(i);
		QWidget* currentWidget = _ui->stackedWidget->findChild<QWidget*>(name);
		if (currentWidget)
		{
			QMetaObject::invokeMethod(currentWidget, "saveSettings");
		}
	}
	
    // accep changes
    accept();
}


QString ConfigDialog::getFormat()
{
    switch (_ui->cbxFormat->currentIndex())
    {
        case 0: return "png";
        case 1: return "jpg";
        default: return "png";
    }
}

void ConfigDialog::selectDir()
{
QString *directory = new QString;
#ifdef Q_WS_X11
{
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             _ui->editDir->text(), QFileDialog::ShowDirsOnly)+QDir::separator();
    if (directory->toUtf8() != QDir::separator())
    {
        _ui->editDir->setText( *directory);
    }
}
#endif
#ifdef Q_WS_WIN
    *directory = QFileDialog::getExistingDirectory(this, trUtf8("Select directory"),
             _ui->editDir->text(), QFileDialog::ShowDirsOnly)+ "/";
    if (directory->toUtf8() != "/")
    {
        _ui->editDir->setText( QDir::toNativeSeparators(*directory));
    }
#endif
    delete directory;
}

void ConfigDialog::restoreDefaults()
{
    QMessageBox msg;
    msg.setText(tr("Do you want to reset the settings to the defaults?"));
    msg.setWindowTitle("ScreenGrab" + QString(" - ") + tr("Warning"));
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    
    int res = msg.exec();
    
    if (res == QMessageBox::Yes)
    {
        conf->setDefaultSettings();
        conf->saveSettings();
        QDialog::accept();
    }
}

void ConfigDialog::changeDefDelay(int val)
{
    if (val == 0 )
    {
        _ui->defDelay->setSpecialValueText(tr( "None"));
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
        _ui->labTimeTrayMess->setVisible(false);
        _ui->timeTrayMess->setVisible(false);
        break;
    }
    default:
    {
        _ui->labTimeTrayMess->setVisible(true);
        _ui->timeTrayMess->setVisible(true);
        break;
    }
    }
}

void ConfigDialog::setVisibleDateTplEdit(bool checked)
{
        if (checked == false)
        {
            _ui->editDateTmeTpl->setVisible(false);
            _ui->labMask->setVisible(false);
            _ui->labMaskExample->setVisible(false);
        }
        else
        {
            _ui->editDateTmeTpl->setVisible(true);
            _ui->labMask->setVisible(true);
            _ui->labMaskExample->setVisible(true);
        }
}


void ConfigDialog::editDateTmeTpl(QString str)
{
    QString currentDateTime = QDateTime::currentDateTime().toString(str );
    _ui->labMaskExample->setText(tr("Example: ") + currentDateTime);
}

void ConfigDialog::toggleCheckShowTray(bool checked)
{
    _ui->labTrayMessages->setEnabled(checked);
    _ui->cbxTrayMsg->setEnabled(checked);
    _ui->timeTrayMess->setEnabled(checked);
    _ui->labTimeTrayMess->setEnabled(checked);
    _ui->checkInTray->setEnabled(checked);
}

void ConfigDialog::currentItemChanged(const QModelIndex c, const QModelIndex p)
{
    Q_UNUSED(p)
    if (c.parent().isValid() == true)
    {
    _ui->labUsedShortcut->setVisible(true);
    _ui->keyWidget->setVisible(true);

    QTreeWidgetItem *item = _ui->treeKeys->currentItem();
    _ui->keyWidget->setKeySequence(QKeySequence(item->data(1, Qt::DisplayRole).toString()));
    }
    else
    {
    _ui->labUsedShortcut->setVisible(false);
    _ui->keyWidget->setVisible(false);
    }
}


void ConfigDialog::doubleclickTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == true)
    {
    connect(_ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
    _ui->keyWidget->captureKeySequence();
    }
}


void ConfigDialog::collapsTreeKeys(QModelIndex index)
{
    if (index.parent().isValid() == false)
    {
    _ui->labUsedShortcut->setVisible(false);
    _ui->keyWidget->setVisible(false);
    }
}

void ConfigDialog::acceptShortcut(const QKeySequence& seq)
{
    if (checkUsedShortcuts() == false)
    {
#ifdef SG_GLOBAL_SHORTCUTS
        if (avalibelGlobalShortcuts(seq) == true)
        {
            changeShortcut(seq);
        }
        else
        {
                showErrorMessage(tr("This key is already used in your system! Please select another."));
        }
#else
    changeShortcut(seq);
#endif        
    }
    else if (checkUsedShortcuts() == true && seq.toString() != "")
    {
        showErrorMessage(tr("This key is already used in ScreenGrab! Please select another."));
    }
}

void ConfigDialog::changeShortcut(const QKeySequence& seq)
{
    disconnect(_ui->keyWidget, SIGNAL(keySequenceChanged(QKeySequence)), this, SLOT(changeShortcut(QKeySequence)));
    QTreeWidgetItem *item = _ui->treeKeys->selectedItems().first();
    item->setData(1, Qt::DisplayRole, seq.toString());
}

void ConfigDialog::clearShrtcut()
{
    QTreeWidgetItem *item = _ui->treeKeys->selectedItems().first();
    item->setData(1, Qt::DisplayRole, QString(""));
}


void ConfigDialog::keyNotSupported()
{
    showErrorMessage(tr("This key is not supported on your system!"));
}

bool ConfigDialog::checkUsedShortcuts()
{
    QTreeWidgetItem *item = _ui->treeKeys->selectedItems().first();
    QTreeWidgetItemIterator iter(_ui->treeKeys);
    while (*iter)
    {
    if ((*iter) != item && (*iter)->data(1, Qt::DisplayRole) == _ui->keyWidget->keySequence().toString())
    {
        return true;
    }
    ++iter;
    }
    return false;
}

#ifdef SG_GLOBAL_SHORTCUTS
bool ConfigDialog::avalibelGlobalShortcuts(const QKeySequence& seq)
{
    bool ok = false;
    QxtGlobalShortcut *tmpShortcut = new QxtGlobalShortcut;
    if (tmpShortcut->setShortcut(QKeySequence(seq)) == true)
    {
        tmpShortcut->setDisabled(true);
    ok = true;
    }
    delete tmpShortcut;
    return ok;
}
#endif

void ConfigDialog::showErrorMessage(QString text)
{
    _ui->keyWidget->clearKeySequence();
    QMessageBox msg;
    msg.setWindowTitle(tr("Error"));
    msg.setText(text);
    msg.setIcon(QMessageBox::Information);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.exec();
}
