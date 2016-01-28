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

#include "uploaderconfigwidget.h"
#include "ui_uploaderconfigwidget.h"

#include "uploaderconfig.h"

#include <QVariantMap>

#include <QDebug>


UploaderConfigWidget::UploaderConfigWidget(QWidget *parent) :
    QWidget(parent),
    _ui(new Ui::UploaderConfigWidget)
{
    _ui->setupUi(this);

    _ui->settings->setCurrentWidget(_ui->commonSettings);

    QStringList hosts = UploaderConfig::labelsList();
    _ui->cbxHosts->addItems(hosts);
    _ui->cbxDefaultHost->addItems(hosts);

    loadSettings();

    _imgur = new UploaderConfigWidget_ImgUr(this);

    _ui->stackedHosts->addWidget(_imgur);

    void (QComboBox::*hostChanged)(int) = &QComboBox::currentIndexChanged;
    connect(_ui->cbxHosts, hostChanged, _ui->stackedHosts, &QStackedWidget::setCurrentIndex);

    _ui->stackedHosts->setCurrentIndex(_ui->cbxDefaultHost->currentIndex());
}

UploaderConfigWidget::~UploaderConfigWidget()
{
    delete _ui;
}

void UploaderConfigWidget::loadSettings()
{
    qDebug() << "load uploder common settings";

    UploaderConfig config;
    QVariantMap loadValues;
    loadValues.insert("autoCopyDirectLink", QVariant(false));
    loadValues.insert(KEY_DEFAULT_HOST, "");
    loadValues = config.loadSettings("common", loadValues);

    QString defaultHost = loadValues[KEY_DEFAULT_HOST].toString();
    if (defaultHost.isEmpty() == true)
    {
        _ui->cbxDefaultHost->setCurrentIndex(0);
    }
    else
    {
        qint8 index = config.labelsList().indexOf(defaultHost);

        if (index == -1)
        {
            index++;
        }

        _ui->cbxDefaultHost->setCurrentIndex(index);
    }
    _ui->cbxHosts->setCurrentIndex(_ui->cbxDefaultHost->currentIndex());

    _ui->checkAutoCopyMainLink->setChecked(loadValues["autoCopyDirectLink"].toBool());
}

void UploaderConfigWidget::saveSettings()
{
    UploaderConfig config;
    QVariantMap savingValues;
    savingValues.insert(KEY_AUTO_COPY_RESULT_LIMK, _ui->checkAutoCopyMainLink->isChecked());

    QString defaultHost = config.labelsList().at(_ui->cbxDefaultHost->currentIndex());
    savingValues.insert(KEY_DEFAULT_HOST, defaultHost);

    config.saveSettings("common", savingValues);

    QMetaObject::invokeMethod(_imgur, "saveSettings");
}

void UploaderConfigWidget::changeEvent(QEvent *e)
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
