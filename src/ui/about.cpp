/***************************************************************************
 *   Copyright (C) 2009 - 2012 by Artem 'DOOMer' Galichkin                        *
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

#include "about.h"
#include "ui_aboutwidget.h"

#include "src/core/core.h"
#include <QtGui/QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::aboutWidget)
{
    setWindowFlags(Qt::Dialog |  Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);

    ui->setupUi(this);
    ui->labAppName->setText(ui->labAppName->text() + QString(" <b>") + qApp->applicationVersion() + QString("</b>"));

    QString versionInfo;
    versionInfo = tr("built on ");
    versionInfo.append(__DATE__);
    versionInfo.append(" ");
    versionInfo.append(__TIME__);

    ui->labQtVer->setText(tr("using Qt ") + qVersion());

    ui->labVersion->setText(versionInfo);

    QTabBar *tabs = new QTabBar;


    ui->frame->layout()->addWidget(tabs);
    ui->frame->layout()->addWidget(ui->txtArea);

    tabs->setFixedHeight(24);
    tabs->insertTab(0, tr("About"));
    tabs->insertTab(1, tr("Contacts"));
    tabs->insertTab(2, tr("Thanks"));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

    ui->txtArea->setHtml(getAbouT());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::changeTab(int tabIndex)
{
    // trnder text info
    switch(tabIndex)
    {
    case 0:
        ui->txtArea->setHtml(getAbouT()); break;
    case 1:
        ui->txtArea->setHtml(getContscts()); break;
    case 2:
        ui->txtArea->setHtml(getThanks()); break;
    default:
        ui->txtArea->setHtml(getAbouT());
    }
}

void AboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AboutDialog::on_butAboutQt_clicked()
{
    qApp->aboutQt();
}

void AboutDialog::on_butClose_clicked()
{
    accept();
}

QString AboutDialog::getAbouT()
{
    QString str;
    str += "<b>ScreenGrab</b> ";
    str += tr("is crossplatform application for fast creation screenshots of your desktop.");
    str += "<br><br>";
    str += tr("Copyright &copy; 2009-2012, Artem 'DOOMer' Galichkin");
    str += "<div align=right>";
    str += tr("Licensed under the ");
    str += " <a href=http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>GPL v2</a></div>";
    return str;
}

QString AboutDialog::getContscts()
{
    QString str;
    str += tr("E-Mail")+" ";
    str += "<a href=mailto:doomer3d@gmail.com>mailto:doomer3d@gmail.com</a>";
    str += "<br><br>";
    str += "<a href=http://screengrab-qt.googlecode.com>http://screengrab-qt.googlecode.com</a> -- En";
    str += "<br>";
    str += "<a href=http://mapper.ru/screengrab>http://mapper.ru/screengrab/</a> -- Ru";

    return str;
}

QString AboutDialog::getThanks()
{
    QString str;
    str += "<b>" + tr("Translate:") + "</b>";
    str += "<br>";
    str += tr(" Brazilian Portuguese translation")  + "<br>";
    str += tr("Marcio Moraes") + " &lt;marciopanto@gmail.com&gt;<br>";
    str += "<br>";
    str += tr(" Ukrainian translation") + "<br>";
    str += tr("Gennadi Motsyo") + " &lt;drool@altlinux.ru&gt;<br>";
    str += "<br>";
    str += "<b>" + tr("Testing:") + "</b>";
    str += "<br>";
    str += "Alexantia - " + tr("win32-build [Windows XP]") + "<br>";
    str += "iNight - " + tr("win32-build [Windows Vista]") + "<br>";

    return str;
}

void AboutDialog::on_txtArea_anchorClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
}
