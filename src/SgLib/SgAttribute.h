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

#ifndef SG_ATTRIBUTE_H
#define SG_ATTRIBUTE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>




/***===================================================================================================*/
/**
 * SgAttribute provides attribute manipulations. Other classes have to inherit this one.
 *
 */
/**====================================================================================================*/
class SgAttribute
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgAttribute();

 /**A constructor.
  * Creates a copy of an object.
  * \param a -- a attribute to be copied.
  */
  inline SgAttribute(const SgAttribute& a);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgAttribute();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgAttribute& operator=(const SgAttribute& a);

  /**Returns the attributes.
   */
  inline unsigned int getAttributes() const;

  /**Sets up the attributes.
   */
  inline void setAttributes(unsigned int a);


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Adds the attribute to the storage.
   */
  inline void addAttr(uint a);
  
  /**Deletes the attribute from the storage.
   */
  inline void delAttr(uint a);

  /**Set or unset the attribute.
   */
  inline void assignAttr(uint a, bool b) {if (b) addAttr(a); else delAttr(a);};

  /**Toggles the attribute in the storage.
   */
  inline void xorAttr(uint a);

  /**Returns TRUE if the attribute is set.
   */
  inline bool isAttr(uint a) const;

  /**Removes all attributes.
   */
  inline void clearAll();
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  unsigned int          attributes_;     //!< storage for 32 different attributes (is it enough?)
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgAttribute inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgAttribute::SgAttribute() 
{
  attributes_ = 0;
};



// A regular constructor:
inline SgAttribute::SgAttribute(const SgAttribute& a)  
{
  attributes_ = a.attributes_;
};




// A destructor:
inline SgAttribute::~SgAttribute()
{
  // nothing to do
};



//
// INTERFACES:
//
//
inline SgAttribute& SgAttribute::operator=(const SgAttribute& a)
{
  attributes_ = a.attributes_;
  return *this;
};



//
inline unsigned int SgAttribute::getAttributes() const 
{
  return attributes_;
};



//
inline void SgAttribute::setAttributes(unsigned int a)
{
  attributes_ = a;
};


//
// FUNCTIONS:
//
//
//
inline void SgAttribute::addAttr(uint a)
{
  attributes_ |= a;
};


  
//
inline void SgAttribute::delAttr(uint a)
{
  attributes_ &= ~a;
};



//
inline void SgAttribute::xorAttr(uint a)
{
  attributes_ ^= a;
};



//
inline bool SgAttribute::isAttr(uint a) const 
{
  return attributes_ & a;
};



//
inline void SgAttribute::clearAll() 
{
  attributes_ = 0;
};



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
#endif //SG_ATTRIBUTE_H
