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

#ifndef SG_ECC_SITE_H
#define SG_ECC_SITE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QList>
#include <QtCore/QString>

#include <SgMJD.h>


/***===================================================================================================*/
/**
 * SgEccSite -- an collections of eccentricitiew for one site.
 *
 */
/**====================================================================================================*/
class SgEccRec;
class SgEccSite
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgEccSite(const QString& siteName);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgEccSite();

  //
  // Interfaces:
  //

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  bool insertRecord(SgEccRec*);
  
  void checkRecords();
  
  SgEccRec* findRecord(const SgMJD&);
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  QList<SgEccRec*>  records_;
  QString           siteName_;
  SgMJD             tBegin_;
  SgMJD             tEnd_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgEccDat inline members:                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgEccSite::SgEccSite(const QString& siteName) :
  siteName_(siteName)
{
  tBegin_ = tZero;
  tEnd_ = tZero;
};





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
#endif //SG_ECC_SITE_H
