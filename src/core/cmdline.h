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

#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdio.h>

#include <QtCore/QList>
#include <QtCore/QStringList>

// -f -- fuscreen
// -a -- active window
// -r -- region select

struct optData
{
    QString shotName;
    QString longName;
};

class CmdLine
{
public:
    CmdLine();
    ~CmdLine();
    bool isNotEmpty() const;
    bool getParam(QString);

    static void print(QString&);
    static void print(const char *string);

    static bool isCreated();    
private:
    bool cmdNotEmpty;    
    bool parseOpt(char* );
    bool addParam(QString);

    QStringList optsLong;
    QStringList optsFound;

    static bool instance;
    bool typeDefined;

};

#endif // CMDLINE_H
