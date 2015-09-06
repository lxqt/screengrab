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

#include "src/core/regionselect.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>

RegionSelect::RegionSelect(Config *mainconf, QWidget *parent)
    :QWidget(parent)
{
    _conf = mainconf;
    sharedInit();

    move(0, 0);
    drawBackGround();

    _processSelection = false;

    show();

    grabKeyboard();
    grabMouse();
}

RegionSelect::RegionSelect(Config* mainconf, const QRect& lastRect, QWidget* parent)
    : QWidget(parent)
{
    _conf = mainconf;
    sharedInit();
    _selectRect = lastRect;

    move(0, 0);
    drawBackGround();

    _processSelection = false;

    show();

    grabKeyboard();
    grabMouse();
}

RegionSelect::~RegionSelect()
{
    _conf = NULL;
    delete _conf;
}

void RegionSelect::sharedInit()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    setWindowState(Qt::WindowFullScreen);
    setCursor(Qt::CrossCursor);

    _sizeDesktop = QApplication::desktop()->size();
    resize(_sizeDesktop);

    const QList<QScreen *> screens = qApp->screens();
    const QDesktopWidget *desktop = QApplication::desktop();
    const int screenNum = desktop->screenNumber(QCursor::pos());

    if (screenNum < screens.count()) {
        _desktopPixmapBkg = screens[screenNum]->grabWindow(desktop->winId());
    }

    _desktopPixmapClr = _desktopPixmapBkg;
}

void RegionSelect::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    painter.drawPixmap(QPoint(0, 0), _desktopPixmapBkg);

    drawRectSelection(painter);
}

void RegionSelect::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    _selStartPoint = event->pos();
    _processSelection = true;
}

void RegionSelect::mouseReleaseEvent(QMouseEvent* event)
{
    _selEndPoint = event->pos();
    _processSelection = false;
}

void RegionSelect::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return;

    Q_EMIT processDone(true);
}

void RegionSelect::mouseMoveEvent(QMouseEvent *event)
{
    if (_processSelection)
    {
        _selEndPoint = event->pos();
        _selectRect = QRect(_selStartPoint, _selEndPoint).normalized();
        update();
    }
}

void RegionSelect::keyPressEvent(QKeyEvent* event)
{
    // canceled select screen area
    if (event->key() == Qt::Key_Escape)
        Q_EMIT processDone(false);
    // canceled select screen area
    else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        Q_EMIT processDone(true);
    else
        event->ignore();
}

void RegionSelect::drawBackGround()
{
    // create painter on  pixelmap of desktop
    QPainter painter(&_desktopPixmapBkg);

    // set painter brush on 85% transparency
    painter.setBrush(QBrush(QColor(0, 0, 0, 85), Qt::SolidPattern));

    // draw rect of desktop size in poainter
    painter.drawRect(QApplication::desktop()->rect());

    QRect txtRect = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    QString txtTip = QApplication::tr("Use your mouse to draw a rectangle to screenshot or exit pressing\nany key or using the right or middle mouse buttons.");

    txtRect.setHeight(qRound((float) (txtRect.height() / 10))); // rounded val of text rect height

    painter.setPen(QPen(Qt::red)); // ste message rect border color
    painter.setBrush(QBrush(QColor(255, 255, 255, 180), Qt::SolidPattern));
    QRect txtBgRect = painter.boundingRect(txtRect, Qt::AlignCenter, txtTip);

    // set height & width of bkg rect
    txtBgRect.setX(txtBgRect.x() - 6);
    txtBgRect.setY(txtBgRect.y() - 4);
    txtBgRect.setWidth(txtBgRect.width() + 12);
    txtBgRect.setHeight(txtBgRect.height() + 8);

    painter.drawRect(txtBgRect);

    // Draw the text
    painter.setPen(QPen(Qt::black)); // black color pen
    painter.drawText(txtBgRect, Qt::AlignCenter, txtTip);

    // set bkg to pallette widget
    QPalette newPalette = palette();
    newPalette.setBrush(QPalette::Window, QBrush(_desktopPixmapBkg));
    setPalette(newPalette);
}

void RegionSelect::drawRectSelection(QPainter &painter)
{
    painter.drawPixmap(_selectRect, _desktopPixmapClr, _selectRect);
    painter.setPen(QPen(QBrush(QColor(0, 0, 0, 255)), 2));
    painter.drawRect(_selectRect);

    QString txtSize = QApplication::tr("%1 x %2 pixels ").arg(_selectRect.width()).arg(_selectRect.height());
    painter.drawText(_selectRect, Qt::AlignBottom | Qt::AlignRight, txtSize);

    if (!_selEndPoint.isNull() && _conf->getZoomAroundMouse())
    {
        const quint8 zoomSide = 200;

        // create magnifer coords
        QPoint zoomStart = _selEndPoint;
        zoomStart -= QPoint(zoomSide/5, zoomSide/5); // 40, 40

        QPoint zoomEnd = _selEndPoint;
        zoomEnd += QPoint(zoomSide/5, zoomSide/5);

        // creating rect area for magnifer
        QRect zoomRect = QRect(zoomStart, zoomEnd);

        QPixmap zoomPixmap = _desktopPixmapClr.copy(zoomRect).scaled(QSize(zoomSide, zoomSide), Qt::KeepAspectRatio);

        QPainter zoomPainer(&zoomPixmap); // create painter from pixmap maignifer
        zoomPainer.setPen(QPen(QBrush(QColor(255, 0, 0, 180)), 2));
        zoomPainer.drawRect(zoomPixmap.rect()); // draw
        zoomPainer.drawText(zoomPixmap.rect().center() - QPoint(4, -4), "+");

        // position for drawing preview
        QPoint zoomCenter = _selectRect.bottomRight();

        if (zoomCenter.x() + zoomSide > _desktopPixmapClr.rect().width()
            || zoomCenter.y() + zoomSide > _desktopPixmapClr.rect().height())
            zoomCenter -= QPoint(zoomSide, zoomSide);
        painter.drawPixmap(zoomCenter, zoomPixmap);
    }
}

QPixmap RegionSelect::getSelection()
{
    QPixmap sel;
    sel = _desktopPixmapClr.copy(_selectRect);
    return sel;
}

QPoint RegionSelect::getSelectionStartPos()
{
    return _selectRect.topLeft();
}
