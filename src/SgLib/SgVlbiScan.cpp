/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2026 Sergei Bolotin.
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

#include <iostream>
#include <stdlib.h>


#include <SgVlbiScan.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
// An empty constructor:
SgVlbiScan::SgVlbiScan() :
  observations_(),
  key_(""),
  scheduleName_("")
{
  session_ = NULL;
  extrPmX_Hf_ = extrPmY_Hf_ = extrPmX_Lf_ = extrPmY_Lf_ = extrUt1_Hf_ = extrUt1_Lf_ = extrCpX_Lf_ =
  extrCpY_Lf_ =
  intrPmX_Hf_ = intrPmY_Hf_ = intrPmX_Lf_ = intrPmY_Lf_ = intrUt1_Hf_ = intrUt1_Lf_ = intrCpX_Lf_ =
  intrCpY_Lf_ =
  actlPmX_Hf_ = actlPmY_Hf_ = actlPmX_Lf_ = actlPmY_Lf_ = actlUt1_Hf_ = actlUt1_Lf_ = actlCpX_Lf_ =
  actlCpY_Lf_ = 0.0;
};



//
// static first:
const QString SgVlbiScan::className()
{
  return "SgVlbiScan";
};





/*=====================================================================================================*/
//
//                           FRIENDS:
//
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/





/*=====================================================================================================*/
