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

#ifndef SG_ECC_DAT_H
#define SG_ECC_DAT_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>


/***===================================================================================================*/
/**
 * SgEccDat -- an interface to work with CALC/SOLVE's ECCDAT.ecc file.
 *
 */
/**====================================================================================================*/
class SgEccRec;
class SgEccSite;
class SgMJD;
class SgEccDat
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgEccDat(QString="./");

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgEccDat();

  //
  // Interfaces:
  //
  inline const QString& getPath2File() const;
  inline const QString& getFileName() const;
  
  inline void setPath2File(const QString&);
  inline void setFileName(const QString&);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  void importEccFile();
  
  SgEccRec* lookupRecord(const QString&, const SgMJD&);
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  QString                     path2File_;
  QString                     fileName_;
  QMap<QString, SgEccSite*>   siteByName_;
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
inline SgEccDat::SgEccDat(QString path2) :
  path2File_(path2),
  fileName_("ECCDAT.ecc")
{
};






//
// INTERFACES:
//
//
inline const QString& SgEccDat::getPath2File() const
{
  return path2File_;
};



//
inline const QString& SgEccDat::getFileName() const
{
  return fileName_;
};



//  
inline void SgEccDat::setPath2File(const QString& s)
{
  path2File_ = s;
};



//
inline void SgEccDat::setFileName(const QString& s)
{
  fileName_ = s;
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
#endif //SG_ECC_DAT_H
