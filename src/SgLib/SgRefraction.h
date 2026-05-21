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

#ifndef SG_REFRACTION_H
#define SG_REFRACTION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgMappingFunction.h>


class SgMeteoData;
class SgTaskConfig;
class SgVlbiSourceInfo;
class SgVlbiStationInfo;
class SgZenithDelay;



/***===================================================================================================*/
/**
 * SgRefraction -- an abstract class for zenith delays.
 *
 */
/**====================================================================================================*/
class SgRefraction
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgRefraction(const SgTaskConfig*);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgRefraction();

  //
  // Interfaces:
  //
  /**Returns value of the evaluated hydrostatic zenith delay.
   */
  inline double dDryZD() const;

  /**Returns value of the evaluated wet zenith delay.
   */
  inline double dWetZD() const;

  /**Returns value of the evaluated hydrostatic mapping function.
   */
  inline double dDryMF() const;

  /**Returns value of the evaluated wet mapping function.
   */
  inline double dWetMF() const;

  /**Returns value of the evaluated gradient mapping function.
   */
  inline double dGrdMF() const;

  /**Returns value of the delay due to gradients.
   */
  inline double dGradientDelay() const;

  /**Returns value of the evaluated total zenith delay.
   */
  inline double dTotalZD() const;

  /**Returns value of the evaluated total tropospheric delay.
   */
  inline double dTotalDelay() const;

  inline const SgMappingFunction* dryMappingFunction() const {return dryMappingFunction_;};
  inline const SgMappingFunction* wetMappingFunction() const {return wetMappingFunction_;};
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  double calcTroposphericDelay(const SgMJD& epoch, const SgMeteoData& meteo, double e, double a,
          SgVlbiStationInfo* stnInfo, const SgVlbiSourceInfo* srcInfo);



  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  const SgTaskConfig    *cfg_;
  SgZenithDelay         *dryZenithDelay_;
  SgZenithDelay         *wetZenithDelay_;
  SgMappingFunction     *dryMappingFunction_;
  SgMappingFunction     *wetMappingFunction_;
  SgMappingFunction     *gradientMappingFunction_;
  double                dDryZD_;
  double                dWetZD_;
  double                dDryMF_;
  double                dWetMF_;
  double                dGrdMF_;
  double                dGradientDelay_;
  double                dTotalZD_;
  double                dTotalDelay_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgRefraction inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/


//
// INTERFACES:
//
//
inline double SgRefraction::dDryZD() const
{
  return dDryZD_;
};



//
inline double SgRefraction::dWetZD() const
{
  return dWetZD_;
};



//
inline double SgRefraction::dDryMF() const
{
  return dDryMF_;
};



//
inline double SgRefraction::dWetMF() const
{
  return dWetMF_;
};



//
inline double SgRefraction::dGrdMF() const
{
  return dGrdMF_;
};



//
inline double SgRefraction::dGradientDelay() const
{
  return dGradientDelay_;
};



//
inline double SgRefraction::dTotalZD() const
{
  return dTotalZD_;
};



//
inline double SgRefraction::dTotalDelay() const
{
  return dTotalDelay_;
};




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
#endif //SG_REFRACTION_H
