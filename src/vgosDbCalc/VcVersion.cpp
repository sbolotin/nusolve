/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
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

#include "vgosDbCalc.h"


SgVersion                       vgosDbCalcVersion("vgosDbCalc", 0, 5, 3,
                                                  "Piney River",
                                                  SgMJD(   2026,  5,  26,
                                                                 15,  15));

SgVersion                      *driverVersion=&vgosDbCalcVersion;

/*=====================================================================================================*/
  
