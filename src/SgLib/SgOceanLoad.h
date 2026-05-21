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

#ifndef SG_OCEAN_LOAD_H
#define SG_OCEAN_LOAD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <QtCore/QString>


#include <Sg3dVector.h>


class SgTaskConfig;
class SgVlbiStationInfo;
class SgMJD;
/***===================================================================================================*/
/**
 * SgOceanLoad -- an ocean loading calculator.
 *
 */
/**====================================================================================================*/
class SgOceanLoad : public Sg3dVector
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgOceanLoad(const SgTaskConfig*);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgOceanLoad();

  //
  // Interfaces:
  //

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  const Sg3dVector& calcDisplacement(const SgVlbiStationInfo*, const SgMJD&);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  static const double   freq[11];
  static const double   argNumbers[4][11];
  static const SgMJD    _1975;
  double                angle[11];
  void                  calcArgs(const SgMJD&);

  const SgTaskConfig    *cfg_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgOceanLoad inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/


//
// INTERFACES:
//
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
#endif //SG_OCEAN_LOAD_H
