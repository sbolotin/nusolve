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

#ifndef SG_VERSION_H
#define SG_VERSION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <iostream>

#include <QtCore/QString>

class QDataStream;

#include <SgMJD.h>



class SgVersion;
extern SgVersion                libraryVersion;
extern SgVersion               *driverVersion;

/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgVersion
{
public:
  enum NameFormat
  {
    NF_Human      =    0,       //!
    NF_Cylon      =    1,       //!
    NF_Petrov     =    2,       //!
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  inline SgVersion();

  inline SgVersion(const QString&, int, int, int, const QString&, const SgMJD&);

  inline SgVersion(const SgVersion&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVersion();


  //
  // Interfaces:
  //
  // gets:
  //
  //
  inline const QString& getSoftwareName() const;

  //
  inline int getMajorNumber() const;
  
  //
  inline int getMinorNumber() const;
  
  //
  inline int getTeenyNumber() const;
  
  //
  inline const QString& getCodeName() const;

  //
  inline const SgMJD& getReleaseEpoch() const;


  // sets:
  //
  inline void setSoftwareName(const QString&);
  
  //
  inline void setMajorNumber(int);
  
  //
  inline void setMinorNumber(int);
  
  //
  inline void setTeenyNumber(int);
  
  //
  inline void setCodeName(const QString&);
  
  //
  inline void setReleaseEpoch(const SgMJD&);
  
  //
  // Functions:
  //
  //
  inline QString className() const {return "SgVersion";};
  
  //
  inline QString toString() const;
  
  
  bool parseString(const QString& str);

  //
  QString name(NameFormat fmt=NF_Human) const;
  
  /**Assign a version to another one.
   * \param v -- a version to copy.
   */
  inline SgVersion& operator=(const SgVersion& ver);

  //
  inline bool operator==(const SgVersion&) const;
  
  //
  inline bool operator!=(const SgVersion&) const;
  
  //
  inline bool operator<(const SgVersion&) const;
  
  //
  inline bool operator<=(const SgVersion&) const;
  
  //
  inline bool operator>(const SgVersion&) const;
  
  //
  inline bool operator>=(const SgVersion&) const;
  
  
  //
  // Friends:
  //


  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //

protected:
  QString                       softwareName_;
  int                           majorNumber_;
  int                           minorNumber_;
  int                           teenyNumber_;
  QString                       codeName_;
  SgMJD                         releaseEpoch_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgVersion inline members:                                                                           */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVersion::SgVersion() :
  softwareName_(""),
  codeName_(""),
  releaseEpoch_(tZero)
{
  majorNumber_ = 0;
  minorNumber_ = 0;
  teenyNumber_ = 0;
};



//
inline SgVersion::SgVersion(const QString& softwareName, int major, int minor, int teeny, 
  const QString& codeName, const SgMJD& epoch) :
  softwareName_(softwareName),
  codeName_(codeName),
  releaseEpoch_(epoch)
{
  majorNumber_ = major;
  minorNumber_ = minor;
  teenyNumber_ = teeny;
};



//
inline SgVersion::SgVersion(const SgVersion& ver) :
  softwareName_(ver.getSoftwareName()),
  codeName_(ver.getCodeName()),
  releaseEpoch_(ver.getReleaseEpoch())
{
  majorNumber_ = ver.getMajorNumber();
  minorNumber_ = ver.getMinorNumber();
  teenyNumber_ = ver.getTeenyNumber();
};



//
inline SgVersion::~SgVersion()
{
};



// A destructor:



//
// INTERFACES:
//
//
inline const QString& SgVersion::getSoftwareName() const
{
  return softwareName_;
};



//
inline int SgVersion::getMajorNumber() const
{
  return majorNumber_;
};



//
inline int SgVersion::getMinorNumber() const
{
  return minorNumber_;
};



//
inline int SgVersion::getTeenyNumber() const
{
  return teenyNumber_;
};



//
inline const QString& SgVersion::getCodeName() const
{
  return codeName_;
};



//
inline const SgMJD& SgVersion::getReleaseEpoch() const
{
  return releaseEpoch_;
};



// sets:
//
//
inline void SgVersion::setSoftwareName(const QString& name)
{
  softwareName_ = name;
};



//
inline void SgVersion::setMajorNumber(int v)
{
  majorNumber_ = v;
};



//
inline void SgVersion::setMinorNumber(int v)
{
  minorNumber_ = v;
};



//
inline void SgVersion::setTeenyNumber(int v)
{
  teenyNumber_ = v;
};



//
inline void SgVersion::setCodeName(const QString& str)
{
  codeName_ = str;
};



//
inline void SgVersion::setReleaseEpoch(const SgMJD& t)
{
  releaseEpoch_ = t;
};



//
// FUNCTIONS:
//
//
//
inline QString SgVersion::toString() const
{
  return QString().sprintf("%1d.%d.%d", majorNumber_, minorNumber_, teenyNumber_);
};



//
inline SgVersion& SgVersion::operator=(const SgVersion& ver)
{
  softwareName_ = ver.getSoftwareName();
  codeName_ = ver.getCodeName();
  releaseEpoch_ = ver.getReleaseEpoch();
  majorNumber_ = ver.getMajorNumber();
  minorNumber_ = ver.getMinorNumber();
  teenyNumber_ = ver.getTeenyNumber();
  return *this;
};



//
inline bool SgVersion::operator==(const SgVersion& v) const
{
  return 
    (softwareName_==v.softwareName_) && (codeName_==v.codeName_) &&
    (majorNumber_==v.majorNumber_) && (minorNumber_==v.minorNumber_) && (teenyNumber_==v.teenyNumber_);
};



//
inline bool SgVersion::operator!=(const SgVersion& v) const
{
  return !(*this==v);
};



//
inline bool SgVersion::operator<(const SgVersion& v) const
{
  return 
        1000*  majorNumber_ +   minorNumber_ + 0.0001*  teenyNumber_ < 
        1000*v.majorNumber_ + v.minorNumber_ + 0.0001*v.teenyNumber_;
};



//
inline bool SgVersion::operator<=(const SgVersion& v) const
{
  return 
        1000*  majorNumber_ +   minorNumber_ + 0.0001*  teenyNumber_ <=
        1000*v.majorNumber_ + v.minorNumber_ + 0.0001*v.teenyNumber_;
};



//
inline bool SgVersion::operator>(const SgVersion& v) const
{
  return 
        1000*  majorNumber_ +   minorNumber_ + 0.0001*  teenyNumber_ >
        1000*v.majorNumber_ + v.minorNumber_ + 0.0001*v.teenyNumber_;
};



//
inline bool SgVersion::operator>=(const SgVersion& v) const
{
  return 
        1000*  majorNumber_ +   minorNumber_ + 0.0001*  teenyNumber_ >=
        1000*v.majorNumber_ + v.minorNumber_ + 0.0001*v.teenyNumber_;
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
#endif // SG_VERSION_H

