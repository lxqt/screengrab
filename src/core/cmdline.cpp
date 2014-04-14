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
#include "cmdline.h"

#include <QtGui/QApplication>

#include <iostream>

CmdLine::CmdLine()
{

}

CmdLine::~CmdLine()
{

}

void CmdLine::registerParam(const QString& param, const QString& description, CmdLineParam::CmdLineParam paramType)
{
	switch(paramType)
	{
		case CmdLineParam::ScreenType:
		{
			_screenTypeParams << param;
			break;
		}
		case CmdLineParam::Util:
		{
			_utilityParams << param;
			break;
		}
		case CmdLineParam::Printable:
		{
			_onlyPrintParams << param;
			break;
		}
		default:
			break;
	}

	_regstredParams.insert(param, description);
}

bool CmdLine::checkParam(const QString& param)
{
	return _usedParams.contains(param);
}

void CmdLine::parse()
{
	QString param;
    for (int i= 1; i != QApplication::argc(); ++i)
    {
		param = QApplication::argv()[i];

		if (param.startsWith("--") != true)
		{
			_invalidParams << param;
		}
		else
		{
			param = param.remove(0, 2);

			if (_regstredParams.contains(param) == true)
			{
				_usedParams << param;
			}
			else
			{
				_invalidParams << param;
			}
		}
    }

    if (_invalidParams.count() > 0)
	{
		QString printable;
		for (int i =0; i < _invalidParams.count(); ++i)
		{
			printable = "unrecognized command line option: '" + _invalidParams.at(i) + "'";
			print(printable);
		}
	}
}

qint8 CmdLine::selectedScreenType()
{
	for (int i = 0; i < _screenTypeParams.count(); ++i)
	{
		if (_usedParams.contains(_screenTypeParams.at(i)))
		{
			return i;
		}
	}
	return -1;
}

void CmdLine::printHelp()
{
	print("usage: screengrab [options] [screenshot type]");

	QString optStr;

	print("\nScreenshot types:");

	for (int i = 0; i < _screenTypeParams.count(); ++i)
	{
		optStr += "  --" + _screenTypeParams.at(i) + "\t" + _regstredParams.value(_screenTypeParams.at(i));
		print(optStr);
		optStr.clear();
	}

	print("\nStartup behaviour:");

	for (int i = 0; i < _utilityParams.count(); ++i)
	{
		optStr += "  --" + _utilityParams.at(i) + "\t" + _regstredParams.value(_utilityParams.at(i));
		print(optStr);
		optStr.clear();
	}

	print("\nMiscellaneous:");

	for (int i = 0; i < _onlyPrintParams.count(); ++i)
	{
		optStr += "  --" + _onlyPrintParams.at(i) + "\t" + _regstredParams.value(_onlyPrintParams.at(i));
		print(optStr);
		optStr.clear();
	}
}

void CmdLine::print(const QString& string)
{
	std::cout << string.toStdString() << std::endl;
}
