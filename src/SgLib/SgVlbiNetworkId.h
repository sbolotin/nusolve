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

#ifndef SG_VLBI_NETWORK_ID_H
#define SG_VLBI_NETWORK_ID_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QList>
#include <QtCore/QString>



class SgVlbiNetworkId;
extern QList<SgVlbiNetworkId>   networks;
extern const QString            defaultNetIdName;

/***===================================================================================================*/
/**
 * The class makes a simple guess about network ID.
 *
 */
/**====================================================================================================*/
class SgVlbiNetworkId : public QList<QString>
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
  inline SgVlbiNetworkId();

  /**A constructor.
   * Creates an object.
   */
  inline SgVlbiNetworkId(const QString& name);

  /**A constructor.
   * Creates an object.
   */
  inline SgVlbiNetworkId(const QString& name, const QList<QString>&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgVlbiNetworkId();


  //
  // Interfaces:
  //
  // gets:
  inline const QString& getName() const;
  

  // sets:
  inline void setName(const QString&);

  // functions:
  inline void addKey(const QString&);

  int countMatches(const QString&) const;
 
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  QString                       name_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObservation inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVlbiNetworkId::SgVlbiNetworkId() : 
  name_("")
{
};



//
inline SgVlbiNetworkId::SgVlbiNetworkId(const QString& name) :
  QList<QString>(),
  name_(name)
{
};



inline SgVlbiNetworkId::SgVlbiNetworkId(const QString& name, const QList<QString>& lst) :
  QList<QString>(),
  name_(name)
{
  for (int i=0; i<lst.size(); i++)
    addKey(lst.at(i));
};




// A destructor:
inline SgVlbiNetworkId::~SgVlbiNetworkId()
{
  // nothing to do
};



//
inline const QString& SgVlbiNetworkId::getName() const
{
  return name_;
};



//
inline void SgVlbiNetworkId::setName(const QString& name)
{
  name_ = name;
};



//
inline void SgVlbiNetworkId::addKey(const QString& key)
{
  append(key);
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
#endif // SG_VLBI_NETWORK_ID_H
