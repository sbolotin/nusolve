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

#ifndef SG_MASTER_RECORD_H
#define SG_MASTER_RECORD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>


extern QStringList              defaultMasterfileSuffixes;


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
//
//
class SgMasterRecord
{
public:
  enum MasterFileVersion
  {
    MFV_UNDEF           = 0,
    MFV_V01             = 1,
    MFV_V02             = 2,
  };
  //
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
  inline SgMasterRecord();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgMasterRecord();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgMasterRecord& operator=(const SgMasterRecord& r);

  inline bool isValid() const;
  
  inline const QString& getName() const;

  inline const QString& getType() const;
  
  inline const QString& getCode() const;

  inline const QString& getDate() const;
  
  inline const QString& getStations() const;
  
  inline const QString& getDbcCode() const;
  
  inline const QString& getScheduledBy() const;
  
  inline const QString& getCorrelatedBy() const;
  
  inline const QString& getSubmittedBy() const;
  
  inline MasterFileVersion mfVersion() const {return mfVersion_;};
  
  inline const QStringList& getSuffixes() const {return suffixes_;};
  
  

  inline void setName(const QString& s);

  inline void setType(const QString& s);
  
  inline void setCode(const QString& s);
  
  inline void setDate(const QString& s);
  
  inline void setStations(const QString& s);
  
  inline void setDbcCode(const QString& s);
  
  inline void setScheduledBy(const QString& s);
  
  inline void setCorrelatedBy(const QString& s);
  
  inline void setSubmittedBy(const QString& s);

  void setSuffixes(const QStringList& lst);
  void addSuffix(const QString& sfx);

  //
  // Functions:
  //
  bool lookupRecordByName(const QString& nickName, const QString& path);

  bool lookupRecordByCode(const QString& sessionCode, const QString& sYear, const QString& path);

  void parseString_v1(const QString&);

  void parseString_v2(const QString&);

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
  QString                       type_;
  QString                       code_;
  QString                       date_;
  QString                       stations_;
  QString                       dbcCode_;
  QString                       scheduledBy_;
  QString                       correlatedBy_;
  QString                       submittedBy_;
  MasterFileVersion             mfVersion_;
  QStringList                   suffixes_;


  bool findRecordByName(const QString& date, const QString& dbcCode, const QString& fileName);
  bool findRecordByCode(const QString& sessionCode, const QString& fileName);
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObjectInfo inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgMasterRecord::SgMasterRecord() :
  name_("NONE"),
  type_(""),
  code_("NONE"),
  date_("FEB31"),
  stations_("0 -0"),
  dbcCode_("ZQ"),
  scheduledBy_("UNESCO"),
  correlatedBy_("UNESCO"),
  submittedBy_("UNESCO"),
  suffixes_(defaultMasterfileSuffixes)
{
  isValid_ = false;
  mfVersion_ = MFV_UNDEF;
};



// A destructor:
inline SgMasterRecord::~SgMasterRecord()
{
  suffixes_.clear();
};



//
// INTERFACES:
//
// 
//
inline bool SgMasterRecord::isValid() const
{
  return isValid_;
};



//
inline const QString& SgMasterRecord::getName() const
{
  return name_;
};



//
inline const QString& SgMasterRecord::getType() const
{
  return type_;
};



//
inline const QString& SgMasterRecord::getCode() const
{
  return code_;
};



//
inline const QString& SgMasterRecord::getDate() const
{
  return date_;
};



//
inline const QString& SgMasterRecord::getStations() const
{
  return stations_;
};



//
inline const QString& SgMasterRecord::getDbcCode() const
{
  return dbcCode_;
};



//
inline const QString& SgMasterRecord::getScheduledBy() const
{
  return scheduledBy_;
};



//
inline const QString& SgMasterRecord::getCorrelatedBy() const
{
  return correlatedBy_;
};



//
inline const QString& SgMasterRecord::getSubmittedBy() const
{
  return submittedBy_;
};



//
inline void SgMasterRecord::setName(const QString& s)
{
  name_ = s;
};



//
inline void SgMasterRecord::setType(const QString& s)
{
  type_ = s;
};



//
inline void SgMasterRecord::setCode(const QString& s)
{
  code_ = s;
};



//
inline void SgMasterRecord::setDate(const QString& s)
{
  date_ = s;
};



//
inline void SgMasterRecord::setStations(const QString& s)
{
  stations_ = s;
};



//
inline void SgMasterRecord::setDbcCode(const QString& s)
{
  dbcCode_ = s;
};



//
inline void SgMasterRecord::setScheduledBy(const QString& s)
{
  scheduledBy_ = s;
};



//
inline void SgMasterRecord::setCorrelatedBy(const QString& s)
{
  correlatedBy_ = s;
};



//
inline void SgMasterRecord::setSubmittedBy(const QString& s)
{
  submittedBy_ = s;
};




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
#endif //SG_MASTER_RECORD_H
