/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NS_SCR_SUPPORT_H
#define NS_SCR_SUPPORT_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

//#include <QtCore/QtGlobal>


class QString;
class QList<class T>;


int executeScript(const QString& scriptFileName, const QList<QString> args, bool hasDisplay);


/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif // NS_SCR_SUPPORT_H
