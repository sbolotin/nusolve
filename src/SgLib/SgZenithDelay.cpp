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


#include <math.h>


#include <SgConstants.h>
#include <SgLogger.h>
#include <SgVlbiStationInfo.h>
#include <SgZenithDelay.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgZenithDelay::className()
{
  return "SgZenithDelay";
};



//
double SgZenithDelay::calc(const SgMeteoData&, const SgVlbiStationInfo* stnInfo)
{
  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Zenith delay result(%s): zd=%20.16f(m)",
      qPrintable(stnInfo->getKey()), 0.0);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return 0.0;
};



//
double SgZenithDelay::calcVapourPressure(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo)
{
  /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
   * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
   * October 1994. Page 14, Eq. (16): 
   * "...a variation of the simple model of Bean and Dutton [1966]:"
   *
   */                  
  double                        height(stnInfo->getHeight());
/**/
  double                        rh(meteo.relativeHumidity(height, stnInfo));
/**/
  double                        t(meteo.temperature(height, stnInfo));
  double                        vp;
  vp = 0.0;
/* CALC (calc11/catmm.f):
  TEMP = TC+237.3D0
  ESAT = 6.11D0 * EXP(17.269D0*TC/TEMP)
*/

/**/
  //  0<= rh <=1:
  vp = 6.11*rh*exp(17.26938819745534263007*t/(237.3 + t));
/**/
/*
  // From A.Niell e-mail:
*/
//  vp = 6.105*exp(25.22*((t + 0.15)/(t + 273.15)) - 5.31*log((t + 273.15)/273.0));

  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Vapor pressure input (%s): rh=%12.6f, t=%12.6f",
      qPrintable(stnInfo->getKey()), rh, t);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Vapor pressure result(%s): vp=%20.16f(hPa)",
      qPrintable(stnInfo->getKey()), vp);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return vp;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
//
// static first:
const QString SgDryZD_Saastamoinen::className()
{
  return "SgDryZD_Saastamoinen";
};



//
double SgDryZD_Saastamoinen::calc(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo)
{
  double                        latitude=stnInfo->getLatitude(), height=stnInfo->getHeight();
  double                        zd;
  double                        p(meteo.pressure(height, stnInfo));
  
  zd = 2.2768e-3*p/(1.0 - 2.66e-3*cos(2.0*latitude) - 2.8e-7*height);

  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Zenith delay input (%s): P=%12.6f(Pa), latitude=%12.6f(d), height=%12.6f(m)",
      qPrintable(stnInfo->getKey()), p, latitude*RAD2DEG, height);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Zenith delay result(%s): zd=%20.16f(m)",
      qPrintable(stnInfo->getKey()), zd);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return zd;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
//
// static first:
const QString SgWetZD_Saastamoinen::className()
{
  return "SgWetZD_Saastamoinen";
};



//
double SgWetZD_Saastamoinen::calc(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo)
{
  double                        height(stnInfo->getHeight());
  double                        t(meteo.temperature(height, stnInfo));
  double                        zd(0.0), vaporPressure(calcVapourPressure(meteo, stnInfo));

  zd = 2.277e-3*(0.05 + 1255.0/(t + 273.15))*vaporPressure;

/* CALC:
  ZW = 0.002277D0 * (1255.D0/(TC+273.16D0) + 0.05D0) * RH * ESAT
*/

  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Zenith delay input (%s): t=%12.6f(C), vaporPressure=%12.6f(Pa)",
      qPrintable(stnInfo->getKey()), t, vaporPressure);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Zenith delay result(%s): zd=%20.16f(m)",
      qPrintable(stnInfo->getKey()), zd);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return zd;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//
const double SgZenithDelay::k1_ = 77.604;        // K/mbar
const double SgZenithDelay::k2_ = 64.79;         // K/mbar
const double SgZenithDelay::k3_ = 3.776e+5;      // K/mbar^2
/*=====================================================================================================*/








/*=====================================================================================================*/
