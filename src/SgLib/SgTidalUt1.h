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

#ifndef SG_UT1R_H
#define SG_UT1R_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgMJD.h>
#include <SgVector.h>



// Ocean zonal tides
struct SgUt1TidalTableEntry
{
  int                         n_[5];
  double                      ut1_sin_, ut1_cos_;
};




/***===================================================================================================*/
/**
 * UT1S -- removed all tidal terms 
 * UT1R -- removed only short period (up to 35 days) tidal terms
 */
/**====================================================================================================*/
class SgTidalUt1
{
public:
  enum UT1TideContentType
  {
    CT_FULL                 = 0, // UT1
    CT_SHORT_TERMS_REMOVED  = 1, // UT1R
    CT_ALL_TERMS_REMOVED    = 2, // UT1S
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgTidalUt1(UT1TideContentType, double);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgTidalUt1();

  //
  // Interfaces:
  //
  // gets:
  //

  // sets:
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Calculates and returns UT1-UT1S correction (in seconds).
   */
  double calc(const SgMJD&);
  
  
private:
  UT1TideContentType            tideContent_;
  double                        calcVersionValue_;
  const SgUt1TidalTableEntry   *ut1Ttable_;
  const int                    *numOfUt1TableRecs_;
  const int                     numOfUt1TableRecs4ShortPeriods_;
  const int                     numZero_;

  void (*fundArgs_)(const SgMJD&, double[5]);
  void calcTidalUt1(const double args[5], double& dUt1);

};
/*=====================================================================================================*/



//
// constants:
//
extern const SgUt1TidalTableEntry
                                ut1Ttable_IersConv2010[];
extern const int                numOfUt1TableRecs_IersConv2010;
//
extern const SgUt1TidalTableEntry
                                ut1Ttable_IersConv2003[];
extern const int                numOfUt1TableRecs_IersConv2003;
//
extern const SgUt1TidalTableEntry
                                ut1Ttable_IersConv1996[];
extern const int                numOfUt1TableRecs_IersConv1996;
//
extern const SgUt1TidalTableEntry
                                ut1Ttable_IersStds1992[];
extern const int                numOfUt1TableRecs_IersStds1992;




/*=====================================================================================================*/
/*                                                                                                     */
/* SgTidalUt1 inline members:                                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
//



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
#endif //SG_UT1R_H
