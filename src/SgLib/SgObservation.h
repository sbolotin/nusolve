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

#ifndef SG_OBSERVATION_H
#define SG_OBSERVATION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>
#include <QtCore/QString>



#include <SgMJD.h>
#include <SgAttribute.h>
#include <SgVector.h>


class SgTaskConfig;
class SgTaskManager;
class SgVlbiBand;

enum TechniqueID
{
  TECH_VLBI,                    //!< R: Radio interferometry;
  TECH_OA,                      //!< A: optical astrometry;
  TECH_LLR,                     //!< M: Lunar laser ranging;
  TECH_GPS,                     //!< P: Global Positioning System;
  TECH_SLR,                     //!< L: satellite laser ranging;
  TECH_DORIS,                   //!< D: satellite Doppler tracking;
  TECH_COMBINED,                //!< C: combined data;
  TECH_CG,                      //!< G: conventional geodesy (for SSC)
  TECH_AD,                      //!< *: atmospheric data
  TECH_TIE,                     //!< T: ties;
  TECH_UNKN,                    //!< ?: uknown.
  TECH_ANY                      //!< _: any above
};






/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgObservation : public SgMJD, public SgAttribute 
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<<0, //!< omit the observation;
    Attr_PROCESSED          = 1<<1, //!< the observation has been processed;
    Attr_FORCE_2_PROCESS    = 1<<2, //!< include the observation in data analysis in any condition;
  };

  // Statics:


  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgObservation();

 /**A constructor.
  * Creates a copy of an object.
  * \param obs -- an observation record.
  */
  inline SgObservation(const SgObservation& obs);

  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgObservation();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgObservation& operator=(const SgObservation& obs);

  /**Returns a technique ID.
   */
  inline virtual TechniqueID getTechniqueID() const;

  /**Returns current index.
   */
  inline virtual int getMediaIdx() const;

  /**Sets up technique ID.
   * \param id -- tech. ID;
   */
  inline virtual void setTechniqueID(TechniqueID id);

  /**Sets up index.
   * \param idx -- index of the record in a list;
   */
  inline virtual void setMediaIdx(int idx);

  /**Explicit type conversion, returns epoch.
   */
  inline virtual const SgMJD& getMJD() const;

  /**Explicit type conversion, setups epoch.
   */
  inline virtual void setMJD(const SgMJD&);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  inline virtual const QString className() const;

  /**
   */
  virtual bool selfCheck() =0;
  
  /**
   */
  inline virtual bool isEligible(const SgTaskConfig*);

  /**
   */
  virtual void evaluateTheoreticalValues(SgTaskManager*) =0;

  /**
   */
  virtual void prepare4Analysis(SgTaskManager*) =0;

  /**
   */
//  virtual void evaluateResiduals(SgTaskManager*, SgVlbiBand*, bool) =0;

  /**
   */
  virtual void evaluateResiduals(SgTaskManager*) =0;

  /**
   */
  virtual const SgVector& o_c() =0;

  /**
   */
  virtual const SgVector& sigma() =0;
  
  /**
   */
  inline virtual bool operator==(const SgObservation& obs) const;
  
  /**
   */
  inline virtual bool operator!=(const SgObservation& obs) const;
  
  /**
   */
  inline virtual bool operator<(const SgObservation& obs) const;



  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  TechniqueID       techniqueID_;
  int               mediaIdx_;
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
inline SgObservation::SgObservation() : 
    SgMJD(tZero), 
    SgAttribute()
{
  techniqueID_ = TECH_UNKN;
  mediaIdx_ = -1;
};



// A regular constructor:
inline SgObservation::SgObservation(const SgObservation& obs) : 
    SgMJD(obs), 
    SgAttribute(obs)
{
  setTechniqueID(obs.getTechniqueID());
  setMediaIdx(obs.getMediaIdx());
};



// A destructor:
inline SgObservation::~SgObservation()
{
  // nothing to do
};



//
// INTERFACES:
//
// returns Tech. ID
inline TechniqueID SgObservation::getTechniqueID() const
{
  return techniqueID_;
};



// returns current index
inline int SgObservation::getMediaIdx() const
{
  return mediaIdx_;
};



//
inline void SgObservation::setTechniqueID(TechniqueID id)
{
  techniqueID_ = id;
};



//
inline void SgObservation::setMediaIdx(int idx)
{
  mediaIdx_ = idx;
};



//
inline const SgMJD& SgObservation::getMJD() const 
{
  return *this;
};



//
inline void SgObservation::setMJD(const SgMJD& t)
{
  SgMJD::operator=(t);
};



//
// FUNCTIONS:
//
//
//
inline const QString SgObservation::className() const 
{
  return "SgObservation";
};



//
inline bool SgObservation::isEligible(const SgTaskConfig*)
{
  // clear attribute:
  return !isAttr(Attr_NOT_VALID) || isAttr(Attr_FORCE_2_PROCESS);
};



//
inline bool SgObservation::operator==(const SgObservation& obs) const
{
  return SgMJD::operator==(obs) && 
          getMediaIdx() == obs.getMediaIdx() && 
          getTechniqueID() == obs.getTechniqueID();
};



//
inline bool SgObservation::operator!=(const SgObservation& obs) const
{
  return !(obs==*this);
};



//
inline bool SgObservation::operator<(const SgObservation& obs) const
{
//  return *(SgMJD*)this < (const SgMJD&)obs ? true :
  return *this < (const SgMJD&)obs ? true :
  (SgMJD::operator==(obs) ? 
    (getMediaIdx()<obs.getMediaIdx() ? true : 
      (getMediaIdx()==obs.getMediaIdx() ? getTechniqueID()<obs.getTechniqueID() : false) ) : false);
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
//
// variables:
//



/*=====================================================================================================*/
#endif // SG_OBSERVATION_H
