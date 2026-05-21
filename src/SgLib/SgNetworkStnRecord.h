/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#ifndef SG_NETWORK_STN_RECORD_H
#define SG_NETWORK_STN_RECORD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMultiMap>



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgNetworkStnRecord
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
   * Creates an object.
   */
  inline SgNetworkStnRecord();

  SgNetworkStnRecord(const SgNetworkStnRecord&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgNetworkStnRecord();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgNetworkStnRecord& operator=(const SgNetworkStnRecord& r);

  inline bool isValid() const;
  
  inline const QString& getName() const;
  
  inline const QString& getCode() const;

  inline const QString& getDomesId() const;
  
  inline const QString& getCdpId() const;
  
  inline const QString& getDescription() const;
  

  inline void setName(const QString& s);
  
  inline void setCode(const QString& s);
  
  inline void setDomesId(const QString& s);
  
  inline void setCdpId(const QString& s);
  
  inline void setDescription(const QString& s);
  

  //
  // Functions:
  //
  bool parseString(const QString&);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  bool                          isValid_;
  // a masterfile record:
  QString                       name_;
  QString                       code_;
  QString                       domesId_;
  QString                       cdpId_;
  QString                       description_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgNetworkStations
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
   * Creates an object.
   */
  inline SgNetworkStations(const QString& path2file);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgNetworkStations();

  //
  // Interfaces:
  //
  inline bool isOk() const {return isOk_;};
  inline const QString& getFileName() const {return fileName_;};
  inline const QString& getPath2file() const {return path2file_;};

  inline void setFileName(const QString& fn) {fileName_ = fn;};
  inline void setPath2file(const QString& path) {path2file_ = path;};
  inline const QMultiMap<QString, SgNetworkStnRecord>& recsByName() const {return recByName_;};
  inline const QMultiMap<QString, SgNetworkStnRecord>& recsById() const {return recById_;};

  //
  // Functions:
  //
  bool readFile();
  const SgNetworkStnRecord& lookUp(const QString&);
  
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  bool                          isOk_;
  QString                       path2file_;
  QString                       fileName_;
  QMultiMap<QString, SgNetworkStnRecord>
                                recByName_;
  QMultiMap<QString, SgNetworkStnRecord>
                                recById_;
};
/*=====================================================================================================*/







/*=====================================================================================================*/
/*                                                                                                     */
/* SgNetworkStnRecord inline members:                                                                  */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgNetworkStnRecord::SgNetworkStnRecord() :
  name_(""),
  code_(""),
  domesId_(""),
  cdpId_(""),
  description_("")
{
  isValid_ = false;
};



// A destructor:
inline SgNetworkStnRecord::~SgNetworkStnRecord()
{
};



//
// INTERFACES:
//
// 
// Gets:
//
inline bool SgNetworkStnRecord::isValid() const
{
  return isValid_;
};



//
inline const QString& SgNetworkStnRecord::getName() const
{
  return name_;
};



//
inline const QString& SgNetworkStnRecord::getCode() const
{
  return code_;
};



//
inline const QString& SgNetworkStnRecord::getDomesId() const
{
  return domesId_;
};



//
inline const QString& SgNetworkStnRecord::getCdpId() const
{
  return cdpId_;
};



//
inline const QString& SgNetworkStnRecord::getDescription() const
{
  return description_;
};


// Sets:
//
inline void SgNetworkStnRecord::setName(const QString& s)
{
  name_ = s;
};



//
inline void SgNetworkStnRecord::setCode(const QString& s)
{
  code_ = s;
};



//
inline void SgNetworkStnRecord::setDomesId(const QString& s)
{
  domesId_ = s;
};



//
inline void SgNetworkStnRecord::setCdpId(const QString& s)
{
  cdpId_ = s;
};



//
inline void SgNetworkStnRecord::setDescription(const QString& s)
{
  description_ = s;
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
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgNetworkStations's inline members:                                                                 */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgNetworkStations::SgNetworkStations(const QString& path2file) :
  path2file_(path2file),
  fileName_("ns-codes.txt"),
  recByName_(),
  recById_()
{
  isOk_ = false;
};



// A destructor:
inline SgNetworkStations::~SgNetworkStations()
{
  recByName_.clear();
  recById_.clear();
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
#endif //SG_NETWORK_STN_RECORD_H
