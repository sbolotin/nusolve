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

#ifndef SG_SOURCE_KEY_H
#define SG_SOURCE_KEY_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>
#include <QtCore/QString>






/***==============================================================================================*/
/**
 * 
 *
 */
/**===============================================================================================*/
class SgSourceKey
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgSourceKey();

 /**A constructor.
  * Creates a copy of an object.
  * \param key -- source name from a database file.
  */
  inline SgSourceKey(const QString& key);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgSourceKey();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgSourceKey& operator=(const SgSourceKey& srcKey);

  /**Returns a key.
   */
  inline const QString& getKey() const;

  /**Sets a key.
   * \param srcKey -- new key;
   */
  void setKey(const QString& srcKey);

  //
  // Functions:
  //

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  QString                   key_;          //! a key;

};
/*================================================================================================*/





/*================================================================================================*/
/*                                                                                                */
/* SgSourceKey inline members:                                                                    */
/*                                                                                                */
/*================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgSourceKey::SgSourceKey() : key_("1234+678")
{
};



// A regular constructor:
inline SgSourceKey::SgSourceKey(const QString& key)
{
  key_ = key;
};



// A destructor:
inline SgSourceKey::~SgSourceKey()
{
  // nothing to do
};



//
// INTERFACES:
//
// returns number of MJDays
inline const QString& SgSourceKey::getKey() const 
{
  return key_;
};



//
inline void SgSourceKey::setKey(const QString& key)
{
  key_ = key;
};



//
// FUNCTIONS:
//
//
//



//
// FRIENDS:
//
//
//


/*================================================================================================*/





/*================================================================================================*/
//
// aux functions:
//


/*================================================================================================*/
#endif //SG_SOURCE_KEY_H
