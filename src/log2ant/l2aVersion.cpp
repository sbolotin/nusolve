/*
 *    This file is a part of log2ant utility. The utility is a part of
 *    nuSolve distribution and is designed to extract meteo parameters,
 *    cable calibrations, tsys data from stations log files and store them
 *    in ANTAB format.
 *    Copyright (C) 2020 Sergei Bolotin.
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

#include "log2ant.h"


SgVersion                       log2antVersion("log2ant", 0, 1, 3,
                                               "Old Rag",
                                                SgMJD(2026,  4, 29,
                                                            17, 49));

SgVersion                      *driverVersion=&log2antVersion;

/*=====================================================================================================*/
