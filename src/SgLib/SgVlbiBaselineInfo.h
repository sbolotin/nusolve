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

#ifndef SG_VLBI_BASELINE_INFO_H
#define SG_VLBI_BASELINE_INFO_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>

class QDataStream;

#include <SgMJD.h>
#include <SgObjectInfo.h>
#include <SgParameter.h>
#include <SgTaskConfig.h>



// temporary here:
class SgClockBreakIndicator
{
  public:
  enum Options
  {
    O_NOT_DEFINED   = 0,
    O_MAX           = 1,
    O_MIN           = 2,
  };
  SgMJD             epoch_;
  double            totWrms_;
  double            minWrms_;
  double            extremum_;
  int               numOnLeft_;
  int               numOnRight_;
  SgMJD             tOnLeft_;
  SgMJD             tOnRight_;
  int               options_;
  SgClockBreakIndicator() : epoch_(tZero), tOnLeft_(tZero), tOnRight_(tZero)
    {totWrms_=minWrms_=extremum_=0.0; numOnLeft_=numOnRight_=0; options_=O_NOT_DEFINED;}
    
};




//class SgTaskConfig;
//class SgVlbiObservation;
class SgVlbiObservable;
class SgVlbiStationInfo;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
class SgVlbiBaselineInfo;
typedef QMap<QString, SgVlbiBaselineInfo*>            BaselinesByName;
typedef QMap<QString, SgVlbiBaselineInfo*>::iterator  BaselinesByName_it; 
//
//
class SgVlbiBaselineInfo : public SgObjectInfo
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<< 0, //!< omit the baseline;
    Attr_ESTIMATE_CLOCKS    = 1<< 1, //!< estimate baseline clocks;
    Attr_BIND_TROPOSPHERE   = 1<< 2, //!< the troposphere parameters should be tied between the stations;
    Attr_USE_IONO4GRD       = 1<< 3, //!< use ionosphere corrections for group delays;
    Attr_USE_IONO4PHD       = 1<< 4, //!< use ionosphere corrections for phase delays;
    Attr_SKIP_WC            = 1<< 5, //!< do not make weight corrections for this baseline;
  };

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
  inline SgVlbiBaselineInfo(int idx=-1, const QString& key="Unknown", const QString& aka="Unknown too");

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiBaselineInfo();



  //
  // Interfaces:
  //
  /**Returns a reference on the list of observations at this baseline.
   */
  inline QList<SgVlbiObservable*> &observables();

  inline double getCbdTotalWrms() const;

  inline double getMeanGrDelResiduals() const;

  inline double getMeanGrDelResidualsSigma() const;

  inline double getAuxSign() const;

  inline const QString& strGrdAmbigsStat() const {return strGrdAmbigsStat_;};

  inline const QString& strPhdAmbigsStat() const {return strPhdAmbigsStat_;};

  inline int getTypicalNumOfChannels() const {return typicalNumOfCannels_;};
  inline double getTypicalGrdAmbigSpacing() const {return typicalGrdAmbigSpacing_;};
  inline double getTypicalPhdAmbigSpacing() const {return typicalPhdAmbigSpacing_;};
  inline double getTypicalAmbigSpacing(SgTaskConfig::VlbiDelayType type) const 
    {return type==SgTaskConfig::VD_GRP_DELAY?typicalGrdAmbigSpacing_:
      (type==SgTaskConfig::VD_PHS_DELAY?typicalPhdAmbigSpacing_:0.0);};
  inline double getLength() const {return length_;};

  inline double dClock() const {return dClock_;};
  inline double dClockSigma() const {return dClockSigma_;};
  
  inline SgClockBreakIndicator* cbIndicator();

  /**Sets an auxiliary sign.
   * \param  s -- new sign;
   */
  inline void setAuxSign(double s);

//  inline void setAuxSigmaDelay(double s) {auxSigmaDelay_ = s;};

//  inline void setAuxSigmaRate(double s) {auxSigmaRate_ = s;};

  inline void setLength(double l) {length_ = l;};

  /**Returns a pointer on the parameter.
   */
  inline SgParameter*& pAux();
  
  inline SgParameter* pClock();

  inline SgParameter* pBx();

  inline SgParameter* pBy();

  inline SgParameter* pBz();

  inline const QMap<int, int>& numOfChanByCount() const {return numOfChanByCount_;};


  //
  // Functions:
  //
  bool selfCheck();

  /**Calculates values of a clock break detection filter.
   */
  void calculateClockF1(SgTaskConfig*);
  void calculateClockF1_save(SgTaskConfig*);
  void calculateClockF2(SgTaskConfig*);
  SgClockBreakIndicator* lookupClockF2(SgTaskConfig*, const SgClockBreakIndicator* target);

  void calculateTest(SgTaskConfig*);
  void calculateTest2(SgTaskConfig*);

  /**Evaluates clock break indicator.
   */
  void evaluateCBIndicator();
  void evaluateCBIndicator2();

  /**Check residuals for ambiguities.
   */
  void scan4Ambiguities();

  /**Evaluates mean group delay residuals (for ambiguity resolution purposes).
   */
  void evaluateMeanGrDelResiduals();

  /**Adjust baseline's numbers of abiguities.
   */
  void shiftAmbiguities(int deltaN);

  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters();

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();

  SgVlbiStationInfo* stn_1(QMap<QString, SgVlbiStationInfo*>) const;
  SgVlbiStationInfo* stn_2(QMap<QString, SgVlbiStationInfo*>) const;
  
  inline void resetAllEditings();
  void setGrdAmbiguities2min();
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //

private:
  QList<SgVlbiObservable*>      observables_;     // not an owner
  // for clock break detector needs:
  double                        cbd_total_wrms_;  // total wrms residuals for the clock breack detector
  SgClockBreakIndicator        *cbIndicator_;

  // temporary place for the parameters:
  SgParameter                  *pAux_;

  // for ambiguity resolutions:
  double                        meanGrDelResiduals_;
  double                        meanGrDelResidualsSigma_;
  double                        auxSign_;
  //
  // for checking numbers of channels:
  QMap<int, int>                numOfChanByCount_;
  int                           typicalNumOfCannels_;

  // Private functions:
  void                          scan4Ambiguities_m1();
  void                          scan4Ambiguities_m2();
  // temporary place for parameters:
  SgParameter                  *pClock_;
  SgParameter                  *pBx_;
  SgParameter                  *pBy_;
  SgParameter                  *pBz_;
  double                        dClock_;
  double                        dClockSigma_;
  double                        length_;
  //
  //
  // 4group delays:
  QMap<double, int>             grdAmbigsBySpacing_;
  double                        typicalGrdAmbigSpacing_;
  QString                       strGrdAmbigsStat_;
  // 4phase delays:
  QMap<double, int>             phdAmbigsBySpacing_;
  double                        typicalPhdAmbigSpacing_;
  QString                       strPhdAmbigsStat_;

};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiBaselineInfo inline members:                                                                  */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVlbiBaselineInfo::SgVlbiBaselineInfo(int idx, const QString& key, const QString& aka) :
  SgObjectInfo(idx, key, aka), 
  observables_(),
  numOfChanByCount_(),
  grdAmbigsBySpacing_(),
  strGrdAmbigsStat_(""),
  phdAmbigsBySpacing_(),
  strPhdAmbigsStat_("")
{
  addAttr(Attr_USE_IONO4GRD); // use it by default if available
  cbd_total_wrms_ = 0.0;
  cbIndicator_ = NULL;
  meanGrDelResiduals_ = 0.0;
  meanGrDelResidualsSigma_ = 0.0;
  auxSign_    = 1.0;
  //
  //auxNumProc_ = 0.0;
  //auxSumRMS2_ = 0.0;
  //auxSumW_    = 0.0;
  //auxSumOne_  = 0.0;
  //auxSumTwo_  = 0.0;
  //
  pAux_   = NULL;
  pClock_ = NULL;
  pBx_    = NULL;
  pBy_    = NULL;
  pBz_    = NULL;
  dClock_ = 0.0;
  dClockSigma_ = 0.0;
  typicalNumOfCannels_ = 0;
  typicalGrdAmbigSpacing_ = 0.0;
  typicalPhdAmbigSpacing_ = 0.0;
  length_ = 0.0;
};



// A destructor:
inline SgVlbiBaselineInfo::~SgVlbiBaselineInfo()
{
  if (cbIndicator_)
    delete cbIndicator_;
  releaseParameters();
  observables_.clear();  // not a owner of the pointers
  numOfChanByCount_.clear();
};



//
// INTERFACES:
//
// 
inline QList<SgVlbiObservable*> &SgVlbiBaselineInfo::observables()
{
  return observables_;
};



//
inline double SgVlbiBaselineInfo::getCbdTotalWrms() const
{
  return cbd_total_wrms_;
};



//
inline double SgVlbiBaselineInfo::getMeanGrDelResiduals() const
{
  return meanGrDelResiduals_;
};



//
inline double SgVlbiBaselineInfo::getMeanGrDelResidualsSigma() const
{
  return meanGrDelResidualsSigma_;
};



//
inline double SgVlbiBaselineInfo::getAuxSign() const
{
  return auxSign_;
};



//
inline SgClockBreakIndicator* SgVlbiBaselineInfo::cbIndicator()
{
  return cbIndicator_;
};



//
inline SgParameter*& SgVlbiBaselineInfo::pAux()
{
  return pAux_;
};



//
inline SgParameter* SgVlbiBaselineInfo::pClock()
{
  return pClock_;
};



//
inline SgParameter* SgVlbiBaselineInfo::pBx()
{
  return pBx_;
};



//
inline SgParameter* SgVlbiBaselineInfo::pBy()
{
  return pBy_;
};



//
inline SgParameter* SgVlbiBaselineInfo::pBz()
{
  return pBz_;
};



//
inline void SgVlbiBaselineInfo::setAuxSign(double s)
{
  auxSign_ = s;
};



//
// FUNCTIONS:
//
//
//
inline void SgVlbiBaselineInfo::resetAllEditings()
{
  SgObjectInfo::resetAllEditings();
  cbd_total_wrms_ = dClock_ = dClockSigma_ = 0.0;
  delAttr(Attr_NOT_VALID);
  delAttr(Attr_ESTIMATE_CLOCKS);
  delAttr(Attr_BIND_TROPOSPHERE);
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
#endif //SG_VLBI_BASELINE_INFO_H
