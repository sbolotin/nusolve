/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2012-2020 Sergei Bolotin.
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

#ifndef SG_A_PRIORI_DATA_H
#define SG_A_PRIORI_DATA_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QMultiMap>
#include <QtCore/QString>

#include <SgVector.h>

#include <SgMJD.h>



// this is a preliminary realizaion of accessing to a priori files, should be reworked later.

class QTextStream;

/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgAPrioriRecComponent
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgAPrioriRecComponent() : dValues_(), bValues_() {};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgAPrioriRecComponent() {dValues_.clear(); bValues_.clear();};

  //
  // Interfaces:
  //
  // gets:
  inline double getDvalue(const QString& idx) const {return dValues_[idx];};
  inline bool getBvalue(const QString& idx) const {return bValues_[idx];};

  // sets:
  inline void setDvalue(const QString& idx, double v) {dValues_[idx] = v;};
  inline void setBvalue(const QString& idx, bool b) {bValues_[idx] = b;};
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

private:
  QMap<QString, double>         dValues_;
  QMap<QString, bool>           bValues_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgAPrioriRec : public QList<SgAPrioriRecComponent>
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgAPrioriRec() : key_("NONAME"), tSince_(tInf), comments_("") {};


  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgAPrioriRec() {clear();};


  //
  // Interfaces:
  //
  // gets:
  inline const QString& getKey() const;
  inline const SgMJD& getTsince() const;
  inline const QString& getComments() const {return comments_;};

  // sets:
  inline void setKey(const QString&);
  inline void setTsince(const SgMJD&);
  inline void setComments(const QString& str) {comments_ = str;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

private:
  QString                       key_;
  SgMJD                         tSince_;
  QString                       comments_;
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgAPriories : public QMultiMap<QString, SgAPrioriRec*>
{
public:
  enum DataType
  {
    DT_UNDEF            = 0,
    DT_STN_POS          = 1,
    DT_STN_VEL          = 2,
    DT_SRC_POS          = 3,
    DT_AXS_OFS          = 4,
    DT_STN_GRD          = 5,
    DT_SRC_SSM          = 6,
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgAPriories(DataType=DT_UNDEF);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgAPriories();

  //
  // Interfaces:
  //
  // gets:
  inline DataType getDataType() const;
  
  inline const QString& getFileName() const;

  inline const SgMJD& getT0() const;

  // sets:
  inline void setDataType(DataType);

  inline void setT0(const SgMJD&);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  bool readFile(const QString& fileName, DataType=DT_UNDEF);

  SgAPrioriRec* lookupApRecord(const QString&, const SgMJD& =tZero);


private:
  DataType                      dataType_;
  QString                       fileName_;
  SgMJD                         t0_;

  // private finctions:
  //
  bool parseString4StnPos(const QString& str, SgAPrioriRec& rec);
  //
  bool parseString4StnVel(const QString& str, SgAPrioriRec& rec);
  //
  bool parseString4SrcPos(const QString& str, SgAPrioriRec& rec);
  //
  bool parseString4AxsOfs(const QString& str, SgAPrioriRec& rec);
  //
  bool parseString4StnGrd(const QString& str, SgAPrioriRec& rec);
  //
  //bool parseString4SrcSsm(const QString& str, QTextStream& s, SgAPrioriRec& rec);
  //
  bool parseFileSrcSsm(QTextStream& s);
  //
  void clearStorage();
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAPrioriRec inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/


//
// INTERFACES:
//
//
inline const QString& SgAPrioriRec::getKey() const
{
  return key_;
};



//  
inline const SgMJD& SgAPrioriRec::getTsince() const
{
  return tSince_;
};


// sets:
inline void SgAPrioriRec::setKey(const QString& key)
{
  key_ = key;
};



//
inline void SgAPrioriRec::setTsince(const SgMJD& t)
{
  tSince_ = t;
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
/*                                                                                                     */
/* SgAPriories inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgAPriories::SgAPriories(DataType dataType) :
  QMultiMap<QString, SgAPrioriRec*>(),
  fileName_(""),
  t0_(tZero)
{
  dataType_ = dataType;
};



// A destructor:
inline SgAPriories::~SgAPriories()
{
  clearStorage();
};



//
inline SgAPriories::DataType SgAPriories::getDataType() const
{
  return dataType_;
};



//
inline const QString& SgAPriories::getFileName() const
{
  return fileName_;
};



//
inline const SgMJD& SgAPriories::getT0() const
{
  return t0_;
};



//
inline void SgAPriories::setDataType(SgAPriories::DataType dataType)
{
  dataType_ = dataType;
};



//
inline void SgAPriories::setT0(const SgMJD& t)
{
  t0_ = t;
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
#endif //SG_A_PRIORI_DATA_H
