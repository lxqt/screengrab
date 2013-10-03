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

#include "about.h"
#include "ui_aboutwidget.h"

#include "src/core/core.h"
#include <QtGui/QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::aboutWidget)
{
    setWindowFlags(Qt::Dialog |  Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint);

    _ui->setupUi(this);
    _ui->labAppName->setText(_ui->labAppName->text() + QString(" <b>") + qApp->applicationVersion() + QString("</b>"));

    QString versionInfo;
    versionInfo = tr("built on ");
    versionInfo.append(__DATE__);
    versionInfo.append(" ");
    versionInfo.append(__TIME__);

    _ui->labQtVer->setText(tr("using Qt ") + qVersion());

    _ui->labVersion->setText(versionInfo);

    QTabBar *tabs = new QTabBar;


    _ui->frame->layout()->addWidget(tabs);
    _ui->frame->layout()->addWidget(_ui->txtArea);

    tabs->setFixedHeight(24);
    tabs->insertTab(0, tr("About"));
    tabs->insertTab(1, tr("Thanks"));
    tabs->insertTab(2, tr("Help us"));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(changeTab(int)));

    _ui->txtArea->setHtml(tabAbout());
}

AboutDialog::~AboutDialog()
{
    delete _ui;
}

void AboutDialog::changeTab(int tabIndex)
{
    // trnder text info
    switch(tabIndex)
    {
    case 0:
        _ui->txtArea->setHtml(tabAbout());
        break;
    case 1:
        _ui->txtArea->setHtml(tabThanks());
        break;
    case 2:
        _ui->txtArea->setHtml(tabHelpUs());
        break;
    default:
        _ui->txtArea->setHtml(tabAbout());
    }
}

void AboutDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        _ui->retranslateUi(this);
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

QString AboutDialog::tabAbout()
{
    QString str;
    str += "<b>ScreenGrab</b> ";
    str += tr("is a crossplatform application for fast creating screenshots of your desktop.");
    str += "<br><br>";
    str += tr("It is a light and powerful application and has been written using the Qt framework, so that you are able to use in Windows and Linux.");
    str += "<br><br>";

    str += tr("E-Mail")+" - ";
    str += "<a href=mailto:doomer3d@gmail.com>doomer3d@gmail.com</a>";
    str += "<br>";
    str += tr("Web site")+" - ";
    str += "<a href=http://screengrab.doomer.org>http://screengrab.doomer.org/</a>";
    str += "<br><br>";

    str += tr("Licensed under the ");
    str += " <a href=http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>GPL v2</a>";
    str += "<br><br>";

    str += tr("Copyright &copy; 2009-2013, Artem 'DOOMer' Galichkin");
    return str;
}

QString AboutDialog::tabHelpUs()
{
    QString str;
	str += tr("You can join us and help us if you want. This is an invitation if you like this application.");
	str += "<br><br>";
	
	str += tr("What you can do?");
	
	str += "<ul>";
	str += "<li>" + tr("Translate ScreenGrab to other languages") + "</li>";
	str += "<li>" + tr("Make suggestions for next releases") + "</li>";
	str += "<li>" + tr("Report bugs and issues") + "</li>";
	str += "</ul>";
	
    str += tr("E-Mail");
	str += "<br>";
    str += "<a href=mailto:doomer3d@gmail.com>mailto:doomer3d@gmail.com</a>";
    str += "<br><br>";
	str += tr("Bug tracker");
	str += "<br>";
    str += "<a href=https://github.com/DOOMer/screengrab/issues>https://github.com/DOOMer/screengrab/issues/</a>";

    return str;
}

QString AboutDialog::tabThanks()
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
    str += tr(" Spanish translation") + "<br>";
    str += tr("Burjans L García D") + " &lt;burjans@gmail.com&gt;<br>";
    str += "<br>";
    str += tr(" Italian translation") + "<br>";
    str += "speps &lt;dreamspepser@yahoo.it&gt;<br>";
    str += "<br>";

    str += "<b>" + tr("Testing:") + "</b>";
    str += "<br>";
    str += "Jerome Leclanche - " + tr("Dual monitor support and other in Linux") + "<br>";
    str += "Alexander Sokolov - " + tr("Dual monitor support in Linux") + "<br>";
    str += "Alexantia - " + tr("win32-build [Windows XP and 7]") + "<br>";
    str += "iNight - " + tr("old win32-build [Windows Vista]") + "<br>";
    str += "burjans - " + tr("win32-build [Windows 7]") + "<br>";

    return str;
}

void AboutDialog::on_txtArea_anchorClicked(QUrl url)
{
    QDesktopServices::openUrl(url);
}
