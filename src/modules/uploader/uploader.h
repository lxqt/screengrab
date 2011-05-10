/***************************************************************************
 *   Copyright (C) 2010 by Artem 'DOOMer' Galichkin                        *
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

#ifndef UPLOADER_H
#define UPLOADER_H

#include <QObject>
#include <QByteArray>

class Uploader : public QObject
{
    Q_OBJECT
public:
    Uploader();
    virtual ~Uploader();

public Q_SLOTS:
    void uploadScreen();    
    
Q_SIGNALS:
    void uploadStart();
    void uploadFail();
    void uploadDone();
    
private:
    QByteArray& boundary();
    QByteArray createUploadData();
    QByteArray createRequest();
    
    QByteArray imageData;
    QByteArray strBoundary;
};

#endif // UPLOADER_H
