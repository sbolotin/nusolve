/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
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

#include <iostream>
#include <stdlib.h>


#include <SgMeteoData.h>
#include <SgLogger.h>

#ifndef SEPARATED_LOG2ANT
#     include <SgVlbiStationInfo.h>
#endif


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
double SgMeteoData::dewPt2Rho(double temperature, double dewPtTemperature)
{
  const double a = 17.271;      //pwxcb: 17.269
  const double b = 237.7;       //pwxcb: 237.3
  // These values are taken by Ronnie from Wikipedia:
  // "http://en.wikipedia.org/wiki/Dew_point"
  
  return exp(a*dewPtTemperature/(b + dewPtTemperature) - a*temperature/(b + temperature));
};


#ifndef SEPARATED_LOG2ANT
//
double SgMeteoData::temperature(double height, const SgVlbiStationInfo* stnInfo) const
{
  double                        t(temperature_);
  /*
  if (t<-70.0 || t>50.0 || 
      isAttr(Attr_BAD_DATA) || 
      isAttr(Attr_ARTIFICIAL_DATA) ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO) ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)  )
  {
    //
    // /mk5/libs/cutil/metfix.f:
    // TEMP=293.15D0-(6.5D-3)*ALT-273.16D0
    //
    t = 293.15 - 6.5e-3*height - 273.16;
  };
  */
  return t;
};



//  
double SgMeteoData::pressure(double height, const SgVlbiStationInfo* stnInfo) const
{
  double                        p(pressure_);
  /*
  if (p<600.0 || p>1100.0 ||
      isAttr(Attr_BAD_DATA) || 
      isAttr(Attr_ARTIFICIAL_DATA) ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO) ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)  )
  {
    //    p = 1013.0*exp(-0.03412476651981319179/(273.15 + 10.0)*height);
    //
    // /mk5/libs/cutil/metfix.f:
    // X=1.D0-(6.5D-3)*ALT/293.15D0
    // ATMPR=1013.25D0*(X**5.26D0)
    //
    p = 1013.25*pow(1.0 - 6.5e-3*height/293.15, 5.26);
  };
*/
  return p;
};



//  
double SgMeteoData::relativeHumidity(double /*height*/, const SgVlbiStationInfo* stnInfo) const
{
  double                        rho(relativeHumidity_);
  if (rho < 0.0)
    rho = 0.0;
  if (1.0 < rho)
    rho = 1.0;
  if (isAttr(Attr_BAD_DATA) || isAttr(Attr_ARTIFICIAL_DATA) ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO)    ||
      stnInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)     )
    rho = 0.5;
  
  return rho;
};
#endif
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
