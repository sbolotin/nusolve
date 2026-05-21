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

#ifndef SG_ZENITH_DELAY_H
#define SG_ZENITH_DELAY_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgMeteoData.h>



class SgVlbiStationInfo;
/***===================================================================================================*/
/**
 * SgZenithDelay -- an abstract class for zenith delays.
 *
 */
/**====================================================================================================*/
class SgZenithDelay
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgZenithDelay();

  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgZenithDelay();

  //
  // Interfaces:
  //
  /** Returns refraction coefficient k1, K/mbar.
   */

  double k1() const {return k1_;};

  /** Returns refraction coefficient k2, K/mbar.
   */
  double k2() const {return k2_;};

  /** Returns refraction coefficient k3, K/mbar^2.
   */
  double k3() const {return k3_;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Calcs hydrostatic zenith delay.
   * \param meteo is a meteodata;
   * \param stnInfo is a pointer to the SgStationInfo.
   * \return 0.0 meters (dumb class).
   */
  virtual double calc(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo);

  /**Calcs partial water vapour pressure, mbar.
   * \param t is a dry temperature, C;
   * \param rh is a relative humidity, %.
   * \return partial water vapour pressure, mbar.
   **/
  static double calcVapourPressure(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  static const double           k1_;             //!< refraction coeff. #1, K/mbar;   Thayer, 1974
  static const double           k2_;             //!< refraction coeff. #2, K/mbar;   Thayer, 1974
  static const double           k3_;             //!< refraction coeff. #3, K/mbar^2; Thayer, 1974
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * SgDryZD_Saastamoinen -- an implementation of the hydrostatic zenith delay according to
 * Saastamoinen (1972).
 *
 */
/**====================================================================================================*/
class SgDryZD_Saastamoinen : public SgZenithDelay
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Calcs hydrostatic zenith delay.
   * \param meteo is a meteodata;
   * \param stnInfo is a pointer to the SgStationInfo.
   * \return hydrostatic zenith delay.
   */
  virtual double calc(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo);
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * SgWetZD_Saastamoinen -- an implementation of the wet zenith delay according to
 * Saastamoinen (1973).
 *
 */
/**====================================================================================================*/
class SgWetZD_Saastamoinen : public SgZenithDelay
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Calcs hydrostatic zenith delay.
   * \param meteo is a meteodata;
   * \param stnInfo is a pointer to the SgStationInfo.
   * \return wet zenith delay.
   */
  virtual double calc(const SgMeteoData& meteo, const SgVlbiStationInfo* stnInfo);
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgZenithDelay inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgZenithDelay::SgZenithDelay() 
{
};



// A destructor:
inline SgZenithDelay::~SgZenithDelay()
{
  // nothing to do
};



//
// INTERFACES:
//

//
// FUNCTIONS:
//
//
//

// FRIENDS:
//
//
//
/*=====================================================================================================*/








/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif //SG_ZENITH_DELAY_H
