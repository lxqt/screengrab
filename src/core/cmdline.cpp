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
#include "cmdline.h"

#include <QtCore/QStringListIterator>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

bool CmdLine::instance = false;

CmdLine::CmdLine()
{
    typeDefined = false;
    instance = true;
    optsLong << "version" << "help" << "active" << "fullscreen" << "region";

    for (int i= 1; i != QApplication::argc(); ++i)
    {        
        if (parseOpt(QApplication::argv()[i]) != true)
        {
            qDebug() << QApplication::argv()[i] << " is not valid param";
        }
    }    
}

CmdLine::~CmdLine()
{
    instance = false;
}

bool CmdLine::isCreated()
{
    return instance;
}

bool CmdLine::parseOpt(char* opt)
{
    QString str ;
    bool retVal = false;
    
    if (opt[0] == '-' && opt[1] == '-')
    {        
        for (unsigned int i = 2; i != strlen(opt); ++i )
        {
            str += opt[i];
        }
        if (addParam(str) == true)
        {
            retVal = true;
        }

        // парсинг длинного параметра
    }
    else
    {
        qDebug() << "'" << opt << "' is incorrect option"; // TODO -- print out normal
        retVal = false;
    }
    
    return retVal;
}


bool CmdLine::addParam(QString param)
{
    bool retVal = false;
    if (optsLong.contains(param))
    {
        if (typeDefined == false && (param == "fullscreen" || param == "active" || param == "region"))
        {
            optsFound << param;
            typeDefined = true;
        }
        if (param != "fullscreen" && param != "active" && param != "region")
        {
            optsFound << param;
        }
        retVal = true;
    }
    return retVal;
}

bool CmdLine::getParam(QString name)
{
    return optsFound.contains(name);
}

bool CmdLine::isNotEmpty() const
{
    return cmdNotEmpty;
}

void CmdLine::print(QString &string)
{
    fprintf(stdout, "%s", string.toUtf8().constData());
}

void CmdLine::print(const char *string)
{
    fprintf(stdout, "%s \n", string);
}
