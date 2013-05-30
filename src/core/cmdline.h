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

#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdio.h>

#include <QtCore/QMap>
#include <QtCore/QStringList>

namespace CmdLineParam
{
	enum CmdLineParam {
		ScreenType = 0,
		Util = 1,
		Printable = 2,
	};
}

class CmdLine
{
public:
    CmdLine();
    ~CmdLine();
	
	void printHelp();
    static void print(const QString& string);

	void registerParam(const QString& param, const QString& description, CmdLineParam::CmdLineParam paramType);
	bool checkParam(const QString& param);
	void parse();
	qint8 selectedScreenType();
	
private:
	QStringList _screenTypeParams;
	QStringList _utilityParams;
	QStringList _onlyPrintParams;
	QMap<QString, QString> _regstredParams;
	QStringList _usedParams;
	QStringList _invalidParams;
};

#endif // CMDLINE_H
