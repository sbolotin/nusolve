/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#include <SgRegularExpressions.h>


//                              reSrc("([0-9A-Z]{1}[\\.0-9A-Z\\s_+-]{1,6}[0-9A-Z_]{1})"
QString                         strReSourceName ("([\\.A-Z0-9_+-]{2,8})");
QString                         strReStationName("([_A-Z0-9-]{2,8})");
QString                         strReDouble("([dDeE0-9\\.+-]+)");


/*=====================================================================================================*/





