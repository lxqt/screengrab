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

#include "about.h"
#include "ui_aboutwidget.h"

#include "../core.h"
#include <QDesktopServices>

AboutDialog::AboutDialog(QWidget *parent):
    QDialog(parent),
    _ui(new Ui::aboutWidget)
{
    _ui->setupUi(this);
    setObjectName(QStringLiteral("aboutDialog"));
    _ui->labAppName->setText(_ui->labAppName->text() + QStringLiteral(" <b>") + qApp->applicationVersion() + QStringLiteral("</b>"));

    _ui->labQtVer->setText(tr("using Qt %1").arg(QString::fromUtf8(qVersion())));

    QTabBar *tabs = new QTabBar;

    _ui->frame->layout()->addWidget(tabs);
    _ui->frame->layout()->addWidget(_ui->txtArea);

    tabs->insertTab(0, tr("About"));
    tabs->insertTab(1, tr("Thanks"));
    tabs->insertTab(2, tr("Help us"));

    connect(tabs, &QTabBar::currentChanged, this, &AboutDialog::changeTab);

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
    str += QLatin1String("<b>ScreenGrab</b> ");
    str += tr("is a crossplatform application for fast creating screenshots of your desktop.");
    str += QLatin1String("<br><br>");
    str += tr("It is a light and powerful application, written in Qt.");
    str += QLatin1String("<br><br>");

    str += tr("Website - <a href=\"%1\">%1</a>").arg(QLatin1String("https://github.com/lxqt/screengrab"));
    str += QLatin1String("<br><br>");

    str += tr("Licensed under the <a href=%1>GPL v2</a>")
        .arg(QLatin1String("http://www.gnu.org/licenses/old-licenses/gpl-2.0.html"));
    str += QLatin1String("<br><br>");

    str += tr("Copyright &copy; 2009-2013, Artem 'DOOMer' Galichkin");
    str += QLatin1String("<br>");
    str += tr("Copyright &copy; 2013-2025, LXQt team");
    return str;
}

QString AboutDialog::tabHelpUs()
{
    QString str;
    str += tr("You can join us and help us if you want. This is an invitation if you like this application.");
    str += QLatin1String("<br><br>");

    str += tr("What you can do?");

    str += QLatin1String("<ul>");
    str += QLatin1String("<li>") + tr("Translate ScreenGrab to other languages") + QLatin1String("</li>");
    str += QLatin1String("<li>") + tr("Make suggestions for next releases") + QLatin1String("</li>");
    str += QLatin1String("<li>") + tr("Report bugs and issues") + QLatin1String("</li>");
    str += QLatin1String("</ul>");

    str += tr("Bug tracker");
    str += QLatin1String("<br>");
    str += QLatin1String("<a href=https://github.com/lxqt/screengrab/issues>https://github.com/lxqt/screengrab/issues/</a>");

    return str;
}

QString AboutDialog::tabThanks()
{
    QString str;

    str += QLatin1String("<b>") + tr("Special thanks to:") + QLatin1String("</b>");
    str += QLatin1String("<ul>");
    str += QLatin1String("<li>") + QLatin1String("Artem Galichkin - DOOMer @ GitHub (") + tr("for creating ScreenGrab") + QLatin1String(")</li>");
    str += QLatin1String("<li>") + QLatin1String("Marcus Britanicus @ GitHub (") + tr("for supporting wlr screencopy protocol") + QLatin1String(")</li>");

    return str;
}

void AboutDialog::on_txtArea_anchorClicked(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}
