/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2024 Sergei Bolotin.
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

#include <iostream>
#include <stdlib.h>


#include <QtCore/QDataStream>
#include <QtCore/QRegExp>


#include <SgVlbiObservation.h>


#include <SgConstants.h>
#include <SgEstimator.h>
#include <SgLogger.h>
#include <SgRefraction.h>
#include <SgTaskConfig.h>
#include <SgTaskManager.h>
#include <SgVlbiBand.h>
#include <SgVlbiBaselineInfo.h>
#include <SgVlbiSession.h>
#include <SgVlbiSourceInfo.h>
#include <SgVlbiStationInfo.h>



#define DBG_MODE_ 1
#define DBG_MODE_2_ 1




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
// An empty constructor:
SgVlbiObservation::SgVlbiObservation(SgVlbiSession* session)
  : SgObservation(), 
    observableByKey_(),
    passiveObses_(),
    scanName_(""), 
    corrRootFileName_(""),
    scanId_(""),
    scanFullName_(""),
    rSun_(v3Zero), 
    vSun_(v3Zero), 
    rMoon_(v3Zero), 
    vMoon_(v3Zero), 
    rEarth_(v3Zero), 
    vEarth_(v3Zero), 
    aEarth_(v3Zero),
    trf2crfVal_(m3E),
    trf2crfRat_(m3E),
    trf2crfAcc_(m3E),
    dDel_dR_1_(v3Zero), 
    dRat_dR_1_(v3Zero), 
    dDel_dR_2_(v3Zero),
    dRat_dR_2_(v3Zero),
    userCorrections_(),
    parameters_()
{
  session_ = session;
  techniqueID_ = TECH_VLBI;
  station1Idx_ = -1;
  station2Idx_ = -1;
  sourceIdx_ = -1;
  baselineIdx_ = -1;
  activeObs_ = NULL;
  primeObs_ = NULL;
  apLength_ = 0.0;
  dTec_ = 0.0;
  dTecStdDev_ = 0.0;
  
  calcConsensusDelay_  = 0.0;
  calcConsensusRate_   = 0.0;
  calcConsBendingDelay_= 0.0;
  calcConsBendingRate_ = 0.0;
  calcConsBendingSunDelay_ = 0.0;
  calcConsBendingSunRate_ = 0.0;
  calcConsBendingSunHigherDelay_ = 0.0;
  calcConsBendingSunHigherRate_ = 0.0;
  calcGpsIonoCorrectionDelay_ = 0.0;
  calcGpsIonoCorrectionRate_ = 0.0;
  calcOceanTideDelay_ = 0.0;
  calcOceanTideRate_ = 0.0;
  extDelayHiFyUt1_ = 0.0;
  extRateHiFyUt1_ = 0.0;
  extDelayHiFyPxy_ = 0.0;
  extRateHiFyPxy_ = 0.0;
  calcPoleTideDelay_ = 0.0;
  calcPoleTideRate_ = 0.0;
  calcEarthTideDelay_ = 0.0;
  calcEarthTideRate_ = 0.0;
  calcPxDelay_ = 0.0;
  calcPxRate_ = 0.0;
  calcPyDelay_ = 0.0;
  calcPyRate_ = 0.0;
  calcWobNutatContrib_ = 0.0;
  calcFeedCorrDelay_ = 0.0;
  calcFeedCorrRate_ = 0.0;
  calcTiltRemvrDelay_ = 0.0;
  calcTiltRemvrRate_ = 0.0;
  calcHiFyUt1Delay_ = 0.0;
  calcHiFyUt1Rate_ = 0.0;
  calcHiFyPxyDelay_ = 0.0;
  calcHiFyPxyRate_ = 0.0;
  extDelayErp_ = 0.0;
  extRateErp_  = 0.0;
  aPrioriPxHfContrib_ = 0.0;
  aPrioriPyHfContrib_ = 0.0;
  aPrioriUt1HfContrib_= 0.0;
  aPrioriPxLfContrib_ = 0.0;
  aPrioriPyLfContrib_ = 0.0;
  aPrioriUt1LfContrib_= 0.0;
  aPrioriCxLfContrib_ = 0.0;
  aPrioriCyLfContrib_ = 0.0;
  //
  calcOceanPoleTideLdDelay_ = 0.0;
  calcOceanPoleTideLdRate_ = 0.0;
  calcHiFyUt1LibrationDelay_ = 0.0;
  calcHiFyUt1LibrationRate_ = 0.0;
  calcHiFyPxyLibrationDelay_ = 0.0;
  calcHiFyPxyLibrationRate_ = 0.0;
  calcOceanTideOldDelay_ = 0.0;
  calcOceanTideOldRate_ = 0.0;
  calcPoleTideOldDelay_ = 0.0;
  calcPoleTideOldRate_ = 0.0;
  calcUt1_Tai_ = 0.0;
  calcPmX_ = 0.0;
  calcPmY_ = 0.0;
  calcCipXv_ = 0.0;
  calcCipYv_ = 0.0;
  calcCipSv_ = 0.0;
  calcCipXr_ = 0.0;
  calcCipYr_ = 0.0;
  calcCipSr_ = 0.0;
  calcNutWahr_dPsiV_ = 0.0;
  calcNutWahr_dEpsV_ = 0.0;
  calcNutWahr_dPsiR_ = 0.0;
  calcNutWahr_dEpsR_ = 0.0;
  calcNut2006_dPsiV_ = 0.0;
  calcNut2006_dEpsV_ = 0.0;
  calcNut2006_dPsiR_ = 0.0;
  calcNut2006_dEpsR_ = 0.0;
  //
  dDel_dRA_ = 0.0;
  dRat_dRA_ = 0.0;
  dDel_dDN_ = 0.0;
  dRat_dDN_ = 0.0;
  dDel_dBend_ = 0.0;
  dRat_dBend_ = 0.0;
  dDel_dUT1_ = 0.0;
  dRat_dUT1_ = 0.0;
  d2Del_dUT12_ = 0.0;
  d2Rat_dUT12_ = 0.0;
  dDel_dPx_ = 0.0;
  dRat_dPx_ = 0.0;
  dDel_dPy_ = 0.0;
  dRat_dPy_ = 0.0;
  dDel_dCipX_ = 0.0;
  dRat_dCipX_ = 0.0;
  dDel_dCipY_ = 0.0;
  dRat_dCipY_ = 0.0;
  dDel_dGamma_ = 0.0;
  dRat_dGamma_ = 0.0;
  dDel_dParallax_ = 0.0;
  dRat_dParallax_ = 0.0;
  dDel_dParallaxRev_ = 0.0;
  dRat_dParallaxRev_ = 0.0;
  dDel_dPolTideX_ = 0.0;
  dRat_dPolTideX_ = 0.0;
  dDel_dPolTideY_ = 0.0;
  dRat_dPolTideY_ = 0.0;
  fractC_ = 0.0;

  //  ionoBits_ = (1<<1);                       // No matching group data for GION

  stn_1_ = NULL;
  stn_2_ = NULL;
  src_   = NULL;
  baseline_ = NULL;
  auxObs_1_ = NULL;
  auxObs_2_ = NULL;
  theoDelay_ = 0.0;
  theoRate_ = 0.0;
  extAprioriCorrections4delay_ = 0.0;
  extAprioriCorrections4rate_ = 0.0;
  baselineClock_F1_ = 0.0;
  baselineClock_F2_ = 0.0;
  baselineClock_F1l_ = 0.0;
  baselineClock_F1r_ = 0.0;
  sumAX_4delay_ = sumAX_4rate_ = 0.0;
  reweightAuxSum4delay_ = 0.0;
  reweightAuxSum4rate_ = 0.0;
  
  pimaAutoSup_ = 0;
  pimaUserSup_ = 0;
  pimaUserRec_ = 0;
};



// A destructor:
SgVlbiObservation::~SgVlbiObservation()
{
  // clear the map:
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    delete it.value();
  observableByKey_.clear();
  parameters_.clear();
  userCorrections_.clear();
  activeObs_ = NULL;
  primeObs_ = NULL;
  passiveObses_.clear();
  stn_1_ = NULL;
  stn_2_ = NULL;
  src_   = NULL;
  baseline_ = NULL;
  auxObs_1_ = NULL;
  auxObs_2_ = NULL;
};



//
bool SgVlbiObservation::selfCheck()
{
  bool isOk = true;
  
  // make selfcheck here:
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    o->selfCheck();
    o->calcPhaseDelay(session_->isAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED));
  };

//  if (session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA)
  if (session_->getOriginType() == SgVlbiSessionInfo::OT_AGV)
  {
    //if (pimaAutoSup_ & PAS_INIT__SPS)
    digestPimaAutoSupFlags();
    digestPimaUserSupFlags();
    digestPimaUserRecFlags();
  };
  //  
  //  else
  //  std::cout << "session_->getOriginType()= " << session_->getOriginType() << "\n";

  return isOk;
};


//
bool SgVlbiObservation::addObservable(const QString& bandKey, const SgVlbiObservable& o)
{
  if (observableByKey_.contains(bandKey))
  {
    // at this point the credentials of the observable are not set up yet:
    logger->write(SgLogger::ERR, SgLogger::DATA, className() +
      ":addObservable(): the observable for the " + bandKey + "-band already registered");
    return false;
  };
  observableByKey_.insert(bandKey, new SgVlbiObservable(this, o));
  return true;
};



//
bool SgVlbiObservation::addObservable(const QString& bandKey, SgVlbiObservable* o)
{
  if (observableByKey_.contains(bandKey))
  {
    // at this point the credentials of the observable are not set up yet:
    logger->write(SgLogger::ERR, SgLogger::DATA, className() +
      ":addObservable(): the observable for the " + bandKey + "-band already registered");
    return false;
  };
  observableByKey_.insert(bandKey, o);
  return true;
};



//
int SgVlbiObservation::minQualityFactor() const
{
  int                           qf=observableByKey_.begin().value()->getQualityFactor();
  QMap<QString, SgVlbiObservable*>::const_iterator it;
  for (it=observableByKey_.begin(); it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    if (o->getQualityFactor() < qf)
      qf = o->getQualityFactor();
  };
  return qf;
};



//
void SgVlbiObservation::setupIdentities()
{
  if (!(stn_1_=session_->lookupStationByIdx(station1Idx_)))
  {
    logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the station #1 (idx= " + QString("").setNum(station1Idx_) +
      ") in the session's station infos map");
    return;
  };
  if (!(stn_2_=session_->lookupStationByIdx(station2Idx_)))
  {
    logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the station #2 (idx= " + QString("").setNum(station2Idx_) +
      ") in the session's station infos map");
    return;
  };
  if (!(src_=session_->lookupSourceByIdx(sourceIdx_)))
  {
    logger->write(SgLogger::ERR, SgLogger::SOURCE | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the source (idx= " + QString("").setNum(sourceIdx_) +
      ") in the session's source infos map");
    return;
  };
  if (!(baseline_=session_->lookupBaselineByIdx(baselineIdx_)))
  {
    logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the baseline (idx= " + QString("").setNum(baselineIdx_) +
      ") in the session's baseline infos map");
    return;
  };
  //
  if (stn_1_->auxObservationByScanId()->contains(scanId_))
  {
    auxObs_1_ = stn_1_->auxObservationByScanId()->value(scanId_);
    auxObs_1_->setOwner(this);
  }
  else 
    logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the auxObs_1 for the scan " + scanId_ + ", station: " +
      stn_1_->getKey() + ", obsKey: " + key_, true);
  if (stn_2_->auxObservationByScanId()->contains(scanId_))
  {
    auxObs_2_ = stn_2_->auxObservationByScanId()->value(scanId_);
    auxObs_2_->setOwner(this);
  }
  else 
    logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
      ": setupIdentities(): cannot find the auxObs_2 for the scan " + scanId_ + ", station: " +
      stn_1_->getKey() + ", obsKey: " + key_, true);
  
  // per band identities: 
  QMap<QString, SgVlbiObservable*>::iterator it;
  for (it=observableByKey_.begin(); it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    if (session_->bandByKey().contains(o->getBandKey()))
    {
      SgVlbiBand               *band=session_->bandByKey().value(o->getBandKey());
      if (!(o->stn_1()=band->lookupStationByIdx(station1Idx_)))
      {
        logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
          ": setupIdentities(): cannot find the station #1 (idx= " + QString("").setNum(station1Idx_) +
          ") in the band's station infos map", true);
        return;
      };
      if (!(o->stn_2()=band->lookupStationByIdx(station2Idx_)))
      {
        logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
          ": setupIdentities(): cannot find the station #2 (idx= " + QString("").setNum(station2Idx_) +  
          ") in the band's station infos map", true);
        return;
      };
      if (!(o->src()=band->lookupSourceByIdx(sourceIdx_)))
      {
        logger->write(SgLogger::ERR, SgLogger::SOURCE | SgLogger::DELAY, className() +
          ": setupIdentities(): cannot find the source (idx= " + QString("").setNum(sourceIdx_) +
          ") in the band's source infos map", true);
        return;
      };
      if (!(o->baseline()=band->lookupBaselineByIdx(baselineIdx_)))
      {
        logger->write(SgLogger::ERR, SgLogger::STATION | SgLogger::DELAY, className() +
          ": setupIdentities(): cannot find the baseline (idx= " + QString("").setNum(baselineIdx_) +
          ") in the band's baseline infos map", true);
        return;
      };
      // copy the sigmas to the band level:
      o->baseline()->copySigmas2add(baseline_);
      band->observables().append(o);
      o->baseline()->observables().append(o);
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::DATA | SgLogger::DELAY, className() +
        ": setupIdentities(): cannot find a band (key= " + o->getBandKey() + 
        ") in the band's map", true);
      return;
    };
  };
  //
  if (observableByKey_.contains(session_->primaryBand()->getKey()))
    primeObs_ = observableByKey_.value(session_->primaryBand()->getKey());
  else 
  {
    primeObs_ = NULL;
    logger->write(SgLogger::WRN, SgLogger::DATA, className() +
      ": setupIdentities(): a primary observable is NULL. Obs#" + QString("").setNum(mediaIdx_) +
      " key: " + key_);
    logger->write(SgLogger::WRN, SgLogger::DATA, className() +
      ": setupIdentities(): epoch=[" + toString() + "], scanName=[" + scanName_ + 
      "], numOfBands=" + QString("").setNum(observableByKey_.size()));
  };
};



//
void SgVlbiObservation::setupActiveObservable(const QString& bandKey)
{
  // set up active obs:
  passiveObses_.clear();
  //  
  if ((activeObs_ = observable(bandKey)))
    activeObs_->setIsActive(true);
  else if (bandKey == "X")
    logger->write(SgLogger::ERR, SgLogger::DELAY | SgLogger::DATA, className() +
      ": setupActiveObservable(): cannot find the an active observable for the observation " + key_ +
      " at the X-band", true);
  // collect the passive observables:
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    if (it.value()->getBandKey() != bandKey)
    {
      passiveObses_.append(it.value());
      it.value()->setIsActive(false);
    };
};



//
void SgVlbiObservation::setupActiveMeasurement(const SgTaskConfig* cfg)
{
  QMap<QString, SgVlbiObservable*>::iterator 
                                it;
  for (it=observableByKey_.begin(); it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    o->setupActiveMeasurements(cfg);
    // set up sigmas:
    double                      s2add4delay=1.0E-12;
    double                      s2add4rate =1.0E-15;
    //
    switch (cfg->getWcMode())
    {
    default:
    case SgTaskConfig::WCM_BAND:
      if (cfg->getDoWeightCorrection())
      {
        s2add4delay = session_->bandByKey().value(it.key())->getSigma2add(DT_DELAY);
        s2add4rate  = session_->bandByKey().value(it.key())->getSigma2add(DT_RATE);
      }
      else if (cfg->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
        s2add4delay = 100.0E-12;
      break;
    case SgTaskConfig::WCM_BASELINE:
//      s2add4delay = baseline_->getSigma2add(DT_DELAY);
//      s2add4rate  = baseline_->getSigma2add(DT_RATE);
//      if (cfg->getDoWeightCorrection())
//      {
      s2add4delay = o->baseline()->getSigma2add(DT_DELAY);
      s2add4rate  = o->baseline()->getSigma2add(DT_RATE);
//      };
      break;
    };
    //
    if (o->activeDelay()) // can be NULL:
    {
      o->activeDelay()->setSigma2add(s2add4delay);
      o->activeDelay()->setupSigma2Apply(cfg);
    };
    o->phDRate().setSigma2add(s2add4rate);
    o->phDRate().setupSigma2Apply(cfg);
  };
};



//
bool SgVlbiObservation::isEligible(const SgTaskConfig* cfg)
{
  bool                          isEligible=true;
  sumAX_4delay_ = sumAX_4rate_ = 0.0;

  eradicateAttr(SgVlbiMeasurement::Attr_PROCESSED);
  //
  // the observable is NULL:
  if (!activeObs_)
    return false;

  //
  // check usability of observations at each of bands:
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin();
    it!=observableByKey_.end(); ++it)
    it.value()->checkUsability(cfg);
  //
  //
  // observation is before time to start:
  if (*this < cfg->getT2Bgn() )
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_TOO_EARLY);
    isEligible = false;
  };
  // observation is after time to end:
  if (cfg->getT2End() < *this)
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_TOO_LATE);
    isEligible = false;
  };
  //
  // deselected station #1 || #2:
  if (stn_1()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) || 
      stn_2()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)  )
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_DESELECTED_STATION);
    isEligible = false;
  };
  //
  // deselected source:
  if (src()->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_DESELECTED_SOURCE);
    isEligible = false;
  };
  //
  // deselected baseline:
  if (baseline()->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_DESELECTED_BASELINE);
    isEligible = false;
  };
  //
  //
  //
  // dual band combinations, check other band:
  if (session()->bands().size()>1 &&                              // have more than one band
      (
        activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_REQ_IONO_CORR) ||     // ionosphere corrections were requested
        (activeObs_->activeRate() && activeObs_->activeRate()->isAttr(SgVlbiMeasurement::Attr_REQ_IONO_CORR))
      ) &&
      ( cfg->getUseRateType()==SgTaskConfig::VR_PHS_RATE ||
        cfg->getUseDelayType()==SgTaskConfig::VD_SB_DELAY ||
          (cfg->getUseDelayType()==SgTaskConfig::VD_GRP_DELAY && baseline()->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD)) ||
          (cfg->getUseDelayType()==SgTaskConfig::VD_PHS_DELAY && baseline()->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD))  )
      )
  {
    // no observation on the other band:
    if (observableByKey().size() == 1)
    {
      activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_UNMATED);
      activeObs_->setIsUsable(false);
    }
    else if (activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR) || 
      ( activeObs_->activeRate() && 
        activeObs_->activeRate()->isAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR)) )
      for (int i=0; i<passiveObses().size(); i++)
      {
        SgVlbiObservable       *o=passiveObses().at(i);
        if (!o->isUsable())
        {
          activeObs_->setIsUsable(false);
          if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_LOW_QF))
            activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_MATE_LOW_QF);
          if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_ONE_CHANNEL))
            activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_MATE_ONE_CHANNEL);
          if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_HAS_ERROR_CODE))
            activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_MATE_HAS_ERROR_CODE);
        };
        //
        if (o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
        {
          activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_MATE_DESELECTED_OBS);
          isEligible = false; // not eligible but can be restored
        };
      };
  };
  //
  //
  if ((cfg->getDataType() == SgTaskConfig::VDT_DELAY && activeObs_->activeDelay() && 
        activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID)) ||
      (cfg->getDataType() == SgTaskConfig::VDT_RATE  && activeObs_->activeRate()  &&
        activeObs_->activeRate()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
     )
  {
    activeObs_->nonUsableReason().addAttr(SgVlbiObservable::NUR_DESELECTED_OBS);
    isEligible = false;  // not eligible but can be restored
  };
  //
  return activeObs_->isUsable() && isEligible;
};



//
void SgVlbiObservation::propagateAttr(uint attr)
{
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    o->propagateAttr(attr);
  };
};



//
void SgVlbiObservation::eradicateAttr(uint attr)
{
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    it.value()->eradicateAttr(attr);
};



//
void SgVlbiObservation::eradicateAttr(uint attr, SgTaskConfig::VlbiDelayType tp)
{
  activeObs_->eradicateAttr(attr, tp);
/*
  // ionosphere corrections are available:
  if (session()->isAttr(SgVlbiSession::Attr_HAS_IONO_CORR) &&
       ( (activeObs_->activeRate() && activeObs_->activeRate()->getRateType()==SgTaskConfig::VR_PHS_RATE) ||
        activeObs_->activeDelay()->getDelayType()==SgTaskConfig::VD_SB_DELAY ||
          (activeObs_->activeDelay()->getDelayType()==SgTaskConfig::VD_GRP_DELAY && 
            baseline()->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD)) ||
          (activeObs_->activeDelay()->getDelayType()==SgTaskConfig::VD_PHS_DELAY && 
            baseline()->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD))
       )
     )
  {
    for (int i=0; i<passiveObses().size(); i++)
      passiveObses().at(i)->eradicateAttr(attr, tp);
  };
*/
  if (activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR))
  {
    for (int i=0; i<passiveObses().size(); i++)
      passiveObses().at(i)->eradicateAttr(attr, tp);
  };

};


//
void SgVlbiObservation::evaluateTheoreticalValues(SgTaskManager* mgr)
{
  QString                       str;
  const SgTaskConfig           *cfg=mgr->getTask()->config();

  theoDelay_ = getCalcConsensusDelay();
  theoRate_  = getCalcConsensusRate();
  // first, remove clock model:
  // a priori clock functions:
  double                        dt1( *this - session_->tRefer());
  double                        dt2((*this - session_->tRefer()) + getCalcConsensusDelay()/DAY2SEC);
  double                        d1(1.0), d2(1.0), c1(0.0), c2(0.0), b1(0.0), b2(0.0);
  //
  // delay:
  for (int i=0; i<stn_1_->getClocksModelOrder(); i++)
  {
    c1 += stn_1_->getEstClockModel(i)*d1;
    d1 *= dt1;
  };
  for (int i=0; i<stn_2_->getClocksModelOrder(); i++)
  {
    c2 += stn_2_->getEstClockModel(i)*d2;
    d2 *= dt2;
  };
  theoDelay_ += (c2 - c1);
  //
  // rate:
  d1 = d2 = 1.0;
  for (int i=1; i<stn_1_->getClocksModelOrder(); i++)
  {
    b1 += i*stn_1_->getEstClockModel(i)*d1;
    d1 *= dt1;
  };
  for (int i=1; i<stn_2_->getClocksModelOrder(); i++)
  {
    b2 += i*stn_2_->getEstClockModel(i)*d2;
    d2 *= dt2;
  };
  theoRate_ += (b2 - b1)/DAY2SEC;
  //
  // a priori clocks (if exists):
  if (stn_1_->getNeed2useAPrioriClocks())
    theoDelay_ += -stn_1_->getAPrioriClockTerm_0();
  if (stn_2_->getNeed2useAPrioriClocks())
    theoDelay_ +=  stn_2_->getAPrioriClockTerm_0();
  
#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      ": The observation #" + QString("").setNum(getMediaIdx()) + " of " + src_->getKey() +
      " @ " + stn_1_->getKey() + ":" + stn_2_->getKey() + " on " + 
      toString(SgMJD::F_YYYYMMDDHHMMSSSS));

    str.sprintf("Added contribution: clock model  (ps):       %18.2f", (c2 - c1)*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
  };
#endif  //DBG_MODE_
  //
  // then, other contributions:
  /*double                        dtAtm1(0.0), dtAtm2(0.0), dtTroposphere(0.0)*/;
  double                        dContrib(0.0), dContribA(0.0), dContribB(0.0);
  // 
  // these contributions are already included in the delay:
  //The delays (CONSNDEL) contain the following effects:
  //   axis offset corrections
  //   pole tide corrections
  //   solid Earth tide corrections
  //   nutation/precession
  //   UT1
  //   polar motion
  //   gravitational bending
  //   axis tilt corrections (if tilt is known)
  if (!cfg->getHave2ApplyPxContrib())
  {
    theoDelay_ -= getCalcPxDelay();
    theoRate_  -= getCalcPxRate();
  };
  if (!cfg->getHave2ApplyPyContrib())
  {
    theoDelay_ -= getCalcPyDelay();
    theoRate_  -= getCalcPyRate();
  };
  if (!cfg->getHave2ApplyEarthTideContrib())
  {
    theoDelay_ -= getCalcEarthTideDelay();
    theoRate_  -= getCalcEarthTideRate();
  };
  if (!cfg->getHave2ApplyPoleTideContrib())
  {
    theoDelay_ -= getCalcPoleTideDelay();
    theoRate_  -= getCalcPoleTideRate();
  };
  if (cfg->getHave2ApplyTiltRemvrContrib()) // this one looks like should be added to remove the effect
  {
    theoDelay_ += getCalcTiltRemvrDelay();
    theoRate_  += getCalcTiltRemvrRate();
  };
  // these contributions are not in the theoretical delay:
  if (cfg->getHave2ApplyOceanTideContrib())
  {
    theoDelay_ += getCalcOceanTideDelay();
    theoRate_  += getCalcOceanTideRate();
  };
  if (cfg->getHave2ApplyOldOceanTideContrib())
  {
    theoDelay_ += getCalcOceanTideOldDelay();
    theoRate_  += getCalcOceanTideOldRate();
  };
  /* an alternative way:
  if (cfg->getHave2ApplyOceanTideContrib() && auxObs_1_ && auxObs_2_)
  {
    setCalcOLoadDelay(
        auxObs_1_->getCalcOLoadHorzDelay() + auxObs_1_->getCalcOLoadVertDelay() 
      - auxObs_2_->getCalcOLoadHorzDelay() - auxObs_2_->getCalcOLoadVertDelay() );
    theoDelay_ += getCalcOLoadDelay();
  };*/
  //
  if (cfg->getHave2ApplyPxyOceanTideHFContrib())
  {
    theoDelay_ += getCalcHiFyPxyDelay();
    theoRate_  += getCalcHiFyPxyRate();
  };
  if (cfg->getHave2ApplyUt1OceanTideHFContrib())
  {
    theoDelay_ += getCalcHiFyUt1Delay();
    theoRate_  += getCalcHiFyUt1Rate();
  };
  if (cfg->getHave2ApplyFeedCorrContrib())
  {
    theoDelay_ += getCalcFeedCorrDelay();
    theoRate_  += getCalcFeedCorrRate();
  };
  //
//  if (session_->getCalcVersionValue()<11.0)
  if (session_->calcInfo().getDversion() < 11.0)
  {
    if (cfg->getHave2ApplyNutationHFContrib())
    {
      theoDelay_ += getCalcWobNutatContrib();
    //theoRate_  += getCalcWobNutatContrib();  ?
    };
  }
  else
  {
    if (cfg->getHave2ApplyPxyLibrationContrib())
    {
      theoDelay_ += getCalcHiFyPxyLibrationDelay();
      theoRate_  += getCalcHiFyPxyLibrationRate();
    };
    if (cfg->getHave2ApplyUt1LibrationContrib())
    {
      theoDelay_ += getCalcHiFyUt1LibrationDelay();
      theoRate_  += getCalcHiFyUt1LibrationRate();
    };
  };
  if (cfg->getHave2ApplyOceanPoleTideContrib())
  {
    theoDelay_ += getCalcOceanPoleTideLdDelay();
    theoRate_  += getCalcOceanPoleTideLdRate();
  };
  // old?
  if (cfg->getHave2ApplyOldPoleTideContrib())
  {
    theoDelay_ += getCalcPoleTideOldDelay();
    theoRate_  += getCalcPoleTideOldRate();
  };
  if (cfg->getHave2ApplyGpsIonoContrib())
  {
    theoDelay_ += getCalcGpsIonoCorrectionDelay();
    theoRate_  += getCalcGpsIonoCorrectionRate();
  };

#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Theoretical delay, initial (ps):             %18.2f", getCalcConsensusDelay()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("Theoretical delay after contributions (ps):  %18.2f", theoDelay_*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    // check for contributions:
    // these contributions are already included:
    str.sprintf("                       Pole tides (ps):      %18.2f", getCalcPoleTideDelay()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("                       Wobble x (ps):        %18.2f", getCalcPxDelay()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("                       Wobble y (ps):        %18.2f", getCalcPyDelay()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("                       Earth tides (ps):     %18.2f", getCalcEarthTideDelay()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    if (!cfg->getHave2ApplyPoleTideContrib())
    {
      str.sprintf("Removed contribution: Pole tides (ps):  %18.2f", getCalcPoleTideDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (!cfg->getHave2ApplyEarthTideContrib())
    {
      str.sprintf("Removed contribution: Earth tides (ps): %18.2f", getCalcEarthTideDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (!cfg->getHave2ApplyPxContrib())
    {
      str.sprintf("Removed contribution: wobble x (ps):    %18.2f", getCalcPxDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (!cfg->getHave2ApplyPyContrib())
    {
      str.sprintf("Removed contribution: wobble y (ps):    %18.2f", getCalcPyDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    //
    // these contributions are not in theo. delay:
    if (cfg->getHave2ApplyOceanTideContrib())
    {
      str.sprintf("Added contribution: ocean loading (ps):      %18.2f", 
        getCalcOceanTideDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyOldOceanTideContrib())
    {
      str.sprintf("Added contribution: old ocean loading (ps):  %18.2f", 
        getCalcOceanTideOldDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyPxyOceanTideHFContrib())
    {
      dContribA = getCalcHiFyPxyDelay();
      str.sprintf("Added contribution: high freq PM (ps):       %18.2f", dContribA*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyUt1OceanTideHFContrib())
    {
      dContribB = getCalcHiFyUt1Delay();
      str.sprintf("Added contribution: high freq UT1 (ps):      %18.2f", dContribB*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyPxyOceanTideHFContrib() && 
        cfg->getHave2ApplyUt1OceanTideHFContrib())
    {
      str.sprintf("Added contribution: sum high freq EOP (ps):  %18.2f", (dContribA + dContribB)*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyFeedCorrContrib())
    {
      str.sprintf("Added contribution: feed corr (ps):          %18.2f", getCalcFeedCorrDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (!cfg->getHave2ApplyTiltRemvrContrib())
    {
      str.sprintf("Removed contribution: tilt remover (ps):     %18.2f", getCalcTiltRemvrDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (session_->calcInfo().getDversion() < 11.0)
    {
      if (cfg->getHave2ApplyNutationHFContrib())
      {
        str.sprintf("Added contribution: hi freq nutation in PM:  %18.2f", 
          getCalcWobNutatContrib()*1.0e12);
        logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
      };
    }
    else
    {
      if (cfg->getHave2ApplyPxyLibrationContrib())
      {
        str.sprintf("Added contribution: libration in PM:         %18.2f", 
          getCalcHiFyPxyLibrationDelay()*1.0e12);
        logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
      };
      if (cfg->getHave2ApplyUt1LibrationContrib())
      {
        str.sprintf("Added contribution: libration in UT1:        %18.2f", 
          getCalcHiFyUt1LibrationDelay()*1.0e12);
        logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
      };
    };
    if (cfg->getHave2ApplyOceanPoleTideContrib())
    {
      str.sprintf("Added contribution: ocean pole tide (ps):    %18.2f", 
        getCalcOceanPoleTideLdDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
    if (cfg->getHave2ApplyOldPoleTideContrib())
    {
      str.sprintf("Added contribution: old pole tide (ps):      %18.2f", 
        getCalcPoleTideOldDelay()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    };
  };
#endif  //DBG_MODE_
  //
  //  External a priori:
  //
  double                        dStnPos(0.0), dSrcPos(0.0), dAxsOfs(0.0);
  double                        dMeanGradients(0.0), dExternalErps(0.0);
  double                        dEccCor1(0.0), dEccCor2(0.0), dHiFyPm(0.0), dHiFyUt1(0.0);
  double                        dD, dR;
  //
  // if we have external a priori, correct the RHS:
  extAprioriCorrections4delay_ = 0.0; 
  extAprioriCorrections4rate_  = 0.0;
  if (cfg->getUseExtAPrioriSitesPositions())
  {
    evalExtApriori_StnPos(cfg, dD, dR);
    extAprioriCorrections4delay_ += (dStnPos=dD);
    extAprioriCorrections4rate_  += dR;
  };
  if (cfg->getUseExtAPrioriSourcesPositions())
  {
    evalExtApriori_SrcPos(dD, dR);
    extAprioriCorrections4delay_ += (dSrcPos=dD);
    extAprioriCorrections4rate_  += dR;
  };
  if (cfg->getUseExtAPrioriAxisOffsets())
  {
    evalExtApriori_AxsOfs(dD, dR);
    extAprioriCorrections4delay_ += (dAxsOfs=dD);
    extAprioriCorrections4rate_  += dR;
  };
  if (cfg->getUseExtAPrioriMeanGradients())
  {
    evalExtApriori_MeanGrads(dD, dR);
    extAprioriCorrections4delay_ += (dMeanGradients=dD);
    extAprioriCorrections4rate_  += dR;
  };
  
  
// -------------------
  
  if (cfg->getUseExtAPrioriErp() && session_->isAble2InterpolateErp())
  {
    extAprioriCorrections4delay_ -= (dExternalErps= getExtDelayErp());
    extAprioriCorrections4rate_  -=                 getExtRateErp();
  };

  if (cfg->getUseExtAPrioriHiFyErp())
  {
    extAprioriCorrections4delay_ -= (dHiFyPm =getExtDelayHiFyPxy());
    extAprioriCorrections4delay_ -= (dHiFyUt1=getExtDelayHiFyUt1());
    extAprioriCorrections4rate_  -=           getExtRateHiFyPxy();
    extAprioriCorrections4rate_  -=           getExtRateHiFyUt1();
  };

  if (stn_1_->getIsEccNonZero())
  {
    extAprioriCorrections4delay_ -= (dEccCor1=getDdel_dR_1()*stn_1_->getV3Ecc());
    extAprioriCorrections4rate_  -=           getDrat_dR_1()*stn_1_->getV3Ecc();
  };
  if (stn_2_->getIsEccNonZero())
  {
    extAprioriCorrections4delay_ -= (dEccCor2=getDdel_dR_2()*stn_2_->getV3Ecc());
    extAprioriCorrections4rate_  -=           getDrat_dR_2()*stn_2_->getV3Ecc();
  };
//------------------------------
    
#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("A priori adjustment: stations position (ps): %18.2f", dStnPos*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("A priori adjustment: source positions (ps):  %18.2f", dSrcPos*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("A priori adjustment: axis offsets (ps):      %18.2f", dAxsOfs*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("A priori adjustment: mean gradients (ps):    %18.2f", dMeanGradients*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("A priori adjustment: eccentricities (ps):    %18.2f", (dEccCor1+dEccCor2)*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("External HiFreq model of ERP: PM (ps):       %18.2f", dHiFyPm*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("External HiFreq model of ERP: UT1(ps):       %18.2f", dHiFyUt1*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("A priori adjustment: sum of all     (ps):    %18.2f", 
      extAprioriCorrections4delay_*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
  };
#endif  //DBG_MODE_
  
  //
  //
  //
  // tropospheric delays:
  //
  //
  
  if (auxObs_1_ && auxObs_2_)
  {
    const SgTroposphereModelData
                               *tmd_1=auxObs_1_->getTmdActual(),
                               *tmd_2=auxObs_2_->getTmdActual();
    //
    // dry contribution:
    theoDelay_ += tmd_2->getVal0_delay() - tmd_1->getVal0_delay();
    theoRate_  += tmd_2->getVal0_rate()  - tmd_1->getVal0_rate();
    // wet contribution:
    theoDelay_ += tmd_2->getVal1_delay() - tmd_1->getVal1_delay();
    theoRate_  += tmd_2->getVal1_rate()  - tmd_1->getVal1_rate();

    auxObs_1_->setZenithDelayH(tmd_1->getVal0_delay()/tmd_1->getMap0_delay()*vLight);
    auxObs_1_->setZenithDelayW(tmd_1->getVal1_delay()/tmd_1->getMap1_delay()*vLight);
    auxObs_2_->setZenithDelayH(tmd_2->getVal0_delay()/tmd_2->getMap0_delay()*vLight);
    auxObs_2_->setZenithDelayW(tmd_2->getVal1_delay()/tmd_2->getMap1_delay()*vLight);

    if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
    {
      str.sprintf(" dry MFunct= %20.16f, dry zDelay= %20.16f (m), dry slant= %20.16f (m)", 
        tmd_1->getMap0_delay(), auxObs_1_->getZenithDelayH(), tmd_1->getVal0_delay()*vLight);
      logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + 
        stn_1_->getKey() + ": " + str);
      str.sprintf(" dry MFunct= %20.16f, dry zDelay= %20.16f (m), dry slant= %20.16f (m)", 
        tmd_2->getMap0_delay(), auxObs_2_->getZenithDelayH(), tmd_2->getVal0_delay()*vLight);
      logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + 
        stn_2_->getKey() + ": " + str);

      str.sprintf(" wet MFunct= %20.16f, wet zDelay= %20.16f (m), wet slant= %20.16f (m)", 
        tmd_1->getMap1_delay(), auxObs_1_->getZenithDelayW(), tmd_1->getVal1_delay()*vLight);
      logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + 
        stn_1_->getKey() + ": " + str);
      str.sprintf(" wet MFunct= %20.16f, wet zDelay= %20.16f (m), wet slant= %20.16f (m)", 
        tmd_2->getMap1_delay(), auxObs_2_->getZenithDelayW(), tmd_2->getVal1_delay()*vLight);
      logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + 
        stn_2_->getKey() + ": " + str);
    };
    //
    //
    if (!cfg->getHave2ApplyAxisOffsetContrib())
    {
      theoDelay_ -= 
        (dContrib= auxObs_2_->getCalcAxisOffset4Delay() - auxObs_1_->getCalcAxisOffset4Delay());
// ??   (dContrib= auxObs_1_->getCalcAxisOffset4Delay() + auxObs_2_->getCalcAxisOffset4Delay());
#ifdef DBG_MODE_  
      str.sprintf("Removed contribution: axis offsets  (ps):    %18.2f", -dContrib*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
#endif  //DBG_MODE_
      theoRate_ -= 
// ??   (dContrib= auxObs_1_->getCalcAxisOffset4Rate() + auxObs_2_->getCalcAxisOffset4Rate());
        (dContrib= auxObs_2_->getCalcAxisOffset4Rate() - auxObs_1_->getCalcAxisOffset4Rate());
    };
    if (cfg->getHave2ApplyUnPhaseCalContrib())
    {
      theoDelay_ += (dContrib= auxObs_2_->getCalcUnPhaseCal() - auxObs_1_->getCalcUnPhaseCal());
#ifdef DBG_MODE_
      str.sprintf("Added contribution: unphasecal      (ps):    %18.2f", dContrib*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
#endif  //DBG_MODE_
    };
    //
    //
    if (cfg->getIsTesting())
    {
      theoDelay_ -= auxObs_2_->getRefClockOffset() + theoRate_*auxObs_2_->getRefClockOffset();
    };
    //
    //
    // cable calibration corrections:
    theoDelay_ -= 
      (dContrib=
        (stn_2_->effectiveCableCalMultiplier()*auxObs_2_->effectiveCableCalValue(stn_2_->cccIdx()) -
         stn_1_->effectiveCableCalMultiplier()*auxObs_1_->effectiveCableCalValue(stn_1_->cccIdx()) ));
/*
      (dContrib=(stn_2_->effectiveCableCalMultiplier()*auxObs_2_->getCableCalibration() -
                 stn_1_->effectiveCableCalMultiplier()*auxObs_1_->getCableCalibration() ));
*/
    //
  };  // end of station dependent contribs
  //
  // session-wide clock breaks:
  theoDelay_ +=
    (stn_2_->clockBreaks().value(*this) - stn_1_->clockBreaks().value(*this))*1.0e-09;
  theoRate_  +=
    (stn_2_->clockBreaks().rate (*this) - stn_1_->clockBreaks().rate (*this))*1.0e-09;
  //
#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Cable calibraiton at %-8s (ps):          %18.2f",
      qPrintable(stn_2_->getKey()), 
      stn_2_->effectiveCableCalMultiplier()*auxObs_2_->effectiveCableCalValue(stn_2_->cccIdx())*1.0e12);
//    stn_2_->effectiveCableCalMultiplier()*auxObs_2_->getCableCalibration()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    //
    str.sprintf("Cable calibraiton at %-8s (ps):          %18.2f",
      qPrintable(stn_1_->getKey()), 
      - stn_1_->effectiveCableCalMultiplier()*auxObs_1_->effectiveCableCalValue(stn_1_->cccIdx())*1.0e12);
//    - stn_1_->effectiveCableCalMultiplier()*auxObs_1_->getCableCalibration()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    //
    str.sprintf("Added contribution: cable calibration (ps):  %18.2f", dContrib*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    //
    str.sprintf("Theoretical delay, final (ps):               %18.2f", theoDelay_*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str, true);
  };
#endif  //DBG_MODE_
  //
  //
  //
  // User corrections:
  if (session_->getUserCorrectionsUse().size())
  {
    for (int i=0; i<session_->getUserCorrectionsUse().size(); i++)
      if (session_->getUserCorrectionsUse().at(i))
        theoDelay_ += userCorrections_.at(i);
  };
  //
  //
  if (activeObs_)
  {
    activeObs_->setTauS(0.0);
    if (cfg->getHave2ApplySourceSsm() && 
        src_->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM) && 
        src_->sModel().size())
    {
      double                      tauS;
      tauS = src_->tauS(activeObs_->getReferenceFrequency()*1.0e6,
                      activeObs_->getUvFrPerAsec(0)*360.0*3600.0/2.0/M_PI,
                      activeObs_->getUvFrPerAsec(1)*360.0*3600.0/2.0/M_PI);
      theoDelay_ += tauS;
      activeObs_->setTauS(tauS);
    };
  };
};



//
void SgVlbiObservation::prepare4Analysis(SgTaskManager* mgr)
{
  storage4O_C   .setElement(0, 0.0);
  storage4Sigmas.setElement(0, 0.0);
  if (!activeObs_)
    return;
    
  QString                       str;
  const SgTaskConfig           *cfg=mgr->getTask()->config();
  double                        bandClockBreakShift(0.0), bandClockBreakRate(0.0);
  double                        o_c(0.0), stdDev(0.0);

  logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": ");

  // evaluate and submit the partials:
  fillPartials(mgr);

  // add "processed" flag to the obs and aux obses:
  addAttr(Attr_PROCESSED);
  auxObs_1_->addAttr(SgVlbiAuxObservation::Attr_PROCESSED);
  auxObs_2_->addAttr(SgVlbiAuxObservation::Attr_PROCESSED);
  
  //
  bandClockBreakShift =  (activeObs_->stn_2()->clockBreaks().value(*this) - 
                          activeObs_->stn_1()->clockBreaks().value(*this)  )*1.0e-09;
  bandClockBreakRate  =  (activeObs_->stn_2()->clockBreaks().rate (*this) - 
                          activeObs_->stn_1()->clockBreaks().rate (*this)  )*1.0e-09;
  //
  //
  if (cfg->getDataType() == SgTaskConfig::VDT_DELAY)
  {
    //activeObs_->activeDelay()->addAttr(SgVlbiMeasurement::Attr_PROCESSED);
    //activeObs_->propagateAttr(SgVlbiMeasurement::Attr_PROCESSED);
    propagateAttr(SgVlbiMeasurement::Attr_PROCESSED);
    switch (cfg->getUseDelayType())
    {
    case SgTaskConfig::VD_NONE:
    default:
      break;
    case SgTaskConfig::VD_SB_DELAY:
      o_c = activeObs_->sbDelay().getValue()
          + extAprioriCorrections4delay_
          - activeObs_->sbDelay().getIonoValue()
          - theoDelay_;
      break;
    case SgTaskConfig::VD_GRP_DELAY:
      o_c = activeObs_->grDelay().getValue()
          + activeObs_->grDelay().ambiguity()
          + activeObs_->grDelay().subAmbig()
          + extAprioriCorrections4delay_
          - (theoDelay_ + bandClockBreakShift);
      if (baseline_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD))
        o_c -= activeObs_->grDelay().getIonoValue();
      break;
    case SgTaskConfig::VD_PHS_DELAY:
      o_c = activeObs_->phDelay().getValue()
          + activeObs_->phDelay().ambiguity()
          + extAprioriCorrections4delay_
          - (theoDelay_ + bandClockBreakShift);
      if (baseline_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD))
        o_c -= activeObs_->phDelay().getIonoValue();
      break;
    };
    stdDev = activeObs_->activeDelay()->sigma2Apply();
    if (cfg->getDoDownWeight() && cfg->getUseRateType()!=SgTaskConfig::VR_NONE)
      stdDev *= 1.0e4;
  }
  else if (cfg->getDataType() == SgTaskConfig::VDT_RATE)
  {
    activeObs_->activeRate()->addAttr(SgVlbiMeasurement::Attr_PROCESSED);
    activeObs_->propagateAttr(SgVlbiMeasurement::Attr_PROCESSED);
    o_c = activeObs_->phDRate().getValue()
        + extAprioriCorrections4rate_
        - activeObs_->phDRate().getIonoValue()
        - (theoRate_ + bandClockBreakRate);

    stdDev = activeObs_->phDRate().sigma2Apply();
//    o_c     *= DAY2SEC;
//    stdDev  *= DAY2SEC;
  }
  else
    logger->write(SgLogger::ERR, SgLogger::DELAY, className() + 
      "::prepare4Analysis(): uknown data type");

  storage4O_C   .setElement(0, o_c);
  storage4Sigmas.setElement(0, stdDev);
  //
  //
#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Ionospheric group delay (ps):                %18.2f",
      activeObs_->grDelay().getIonoValue()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    str.sprintf("Ionospheric SB delay (ps):                   %18.2f",
      activeObs_->sbDelay().getIonoValue()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);


    str.sprintf("Ambiguity GRD correction (ps):               %18.2f",
      activeObs_->grDelay().ambiguity()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    str.sprintf("Ambiguity SBD correction (ps):               %18.2f",
      activeObs_->sbDelay().ambiguity()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);


    str.sprintf("Observed group delay (ps):                   %18.2f",
      activeObs_->grDelay().getValue()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);


    str.sprintf("Observed SB delay (ps):                      %18.2f",
      activeObs_->sbDelay().getValue()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    str.sprintf("Resulted O-C (ps):                           %18.2f",
      storage4O_C.getElement(0)*1.0e12);
  
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": +++");

    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": __ sigmas __");
    str.sprintf("Group delay sigma (ps): %8.4f", activeObs_->grDelay().getSigma()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("GRD Iono. sigma: (ps)   %8.4f", activeObs_->grDelay().getIonoSigma()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": __ sigmas __");
    str.sprintf("SB delay sigma (ps):    %8.4f", activeObs_->sbDelay().getSigma()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("SBD Iono sigma: (ps)    %8.4f", activeObs_->sbDelay().getIonoSigma()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);

    str.sprintf("Reweighting sigma (ps): %8.4f", activeObs_->activeDelay()->getSigma2add()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
    str.sprintf("Resulted sigma (ps):    %8.4f", activeObs_->activeDelay()->sigma2Apply()*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": " + str);
  };
#endif
};



//
void SgVlbiObservation::evaluateResiduals(SgTaskManager* mgr)
{
  SgTaskConfig                 *cfg=mgr->getTask()->config();
  const SgParametersDescriptor *parameters=mgr->getTask()->parameters();
  SgEstimator                  *estimator=mgr->estimator();
  QString                       str("");
  
  reweightAuxSum4delay_ = reweightAuxSum4rate_ = 0.0;
  // delay:
  // zerofy all partials:
  estimator->clearPartials();
  // evaluate and submit the partials:
  cfg->setDataType(SgTaskConfig::VDT_DELAY);
  fillPartials(mgr);
  sumAX_4delay_ = estimator->calcAX(getMJD(), true);
  reweightAuxSum4delay_ = estimator->calc_aT_P_a_opt(getMJD(), parameters_);
//  reweightAuxSum4delay_ = estimator->calc_aT_P_a_opt(getMJD());
  //
  //
  // rate:
  // zerofy all partials:
  estimator->clearPartials();
  // evaluate and submit the partials:
  cfg->setDataType(SgTaskConfig::VDT_RATE);
  fillPartials(mgr);
  sumAX_4rate_ = estimator->calcAX(getMJD(), true);
  reweightAuxSum4rate_ = estimator->calc_aT_P_a_opt(getMJD(), parameters_);
//  reweightAuxSum4rate_ = estimator->calc_aT_P_a_opt(getMJD());
  //
  //
  //
  // residuals themselves:
  double                        sbDelayResidual, grDelayResidual, phDelayResidual, phDRateResidual;
  QMap<QString, SgVlbiObservable*>::iterator
                                it;
  for (it=observableByKey_.begin(); it!=observableByKey_.end(); ++it)
  {
    SgVlbiObservable           *o=it.value();
    SgVlbiBand                 *band=session_->bandByKey().value(o->getBandKey());
    double                      bandClockBreakShift(0.0), bandClockBreakRate(0.0);
    bandClockBreakShift =  (o->stn_2()->clockBreaks().value(*this) - 
                            o->stn_1()->clockBreaks().value(*this)  )*1.0e-09;
    bandClockBreakRate  =  (o->stn_2()->clockBreaks().rate (*this) - 
                            o->stn_1()->clockBreaks().rate (*this)  )*1.0e-09;
    //
    // sb delay:
    sbDelayResidual = (o->sbDelay().getValue()
                - o->sbDelay().getIonoValue() - theoDelay_ + extAprioriCorrections4delay_)
                - sumAX_4delay_;
    // group delay:
    grDelayResidual = (o->grDelay().getValue() + o->grDelay().ambiguity() + o->grDelay().subAmbig()
                - (theoDelay_ + bandClockBreakShift) + extAprioriCorrections4delay_)
                - sumAX_4delay_;
    if (baseline_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD))
      grDelayResidual -= o->grDelay().getIonoValue();
    // phase delay:
    phDelayResidual = (o->phDelay().getValue() + o->phDelay().ambiguity()
                - (theoDelay_ + bandClockBreakShift) + extAprioriCorrections4delay_)
                - sumAX_4delay_;
    if (baseline_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD))
      phDelayResidual -= o->phDelay().getIonoValue();
    // rate:
    phDRateResidual = (o->phDRate().getValue() +
                - (theoRate_ + bandClockBreakRate) + extAprioriCorrections4rate_)
                - o->phDRate().getIonoValue()
                - sumAX_4rate_;
    //
    // store them:
    o->sbDelay().setResidual(sbDelayResidual);
    o->grDelay().setResidual(grDelayResidual);
    o->phDelay().setResidual(phDelayResidual);
    o->phDRate().setResidual(phDRateResidual);
    if (o->isUsable())
    {
      band->incNumUsable(DT_DELAY);
      o->stn_1()->incNumUsable(DT_DELAY);
      o->stn_2()->incNumUsable(DT_DELAY);
      o->src  ()->incNumUsable(DT_DELAY);
      o->baseline()->incNumUsable(DT_DELAY);
      band->incNumUsable(DT_RATE);
      o->stn_1()->incNumUsable(DT_RATE);
      o->stn_2()->incNumUsable(DT_RATE);
      o->src  ()->incNumUsable(DT_RATE);
      o->baseline()->incNumUsable(DT_RATE);
    };
#ifdef DBG_MODE_
    if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
    {
      str.sprintf("GrD: Observation:   %18.2f", o->grDelay().getValue()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: Ambiguities:   %18.2f", o->grDelay().ambiguity()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: Ionosphere :   %18.2f", o->grDelay().getIonoValue()*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: Theoretical:   %18.2f", theoDelay_*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: BandClckBrk:   %18.2f", bandClockBreakShift*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: ExtAPriori :   %18.2f", extAprioriCorrections4delay_*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      str.sprintf("GrD: A*x        :   %18.2f", sumAX_4delay_*1.0e12);
      logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
        it.key() + "-Band " + str);
      if (activeObs_) // it's ok to be NULL
      {
        str.sprintf("Group Delay O-C (ps): %s  %s   %05d  %18.2f %18.2f %18.2f",
          (activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)?"Y":"N"), 
          qPrintable(toString(SgMJD::F_YYYYMMDDHHMMSSSS)), mediaIdx_,
          (grDelayResidual + sumAX_4delay_)*1.0e12, grDelayResidual*1.0e12, 
          o->grDelay().sigma2Apply()*1.0e12);
        logger->write(SgLogger::DBG, SgLogger::DELAY, className() + "::evaluateResiduals(): " + 
          it.key() + "-Band " + str);
      };
    };
#endif
    //
    // update statistics:
    double                      appliedSigma, residual, weight;
//  if (isAttr(Attr_PROCESSED))
    if (activeObs_ && activeObs_->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      if (o->activeDelay())
      {
        appliedSigma = o->activeDelay()->sigma2Apply();
        residual     = o->activeDelay()->getResidual();
        weight       = appliedSigma>0.0 ? 1.0/appliedSigma/appliedSigma : 1.0;
        o->stn_1()   -> addWrms(DT_DELAY, *this, residual, weight, reweightAuxSum4delay_);
        o->stn_2()   -> addWrms(DT_DELAY, *this, residual, weight, reweightAuxSum4delay_);
        o->src  ()   -> addWrms(DT_DELAY, *this, residual, weight, reweightAuxSum4delay_);
        o->baseline()-> addWrms(DT_DELAY, *this, residual, weight, reweightAuxSum4delay_);
        band         -> addWrms(DT_DELAY, *this, residual, weight, reweightAuxSum4delay_);
      };
      appliedSigma = o->phDRate().sigma2Apply();
      residual     = o->phDRate().getResidual();
      weight       = appliedSigma>0.0 ? 1.0/appliedSigma/appliedSigma : 1.0;
      o->stn_1()   -> addWrms(DT_RATE, *this, residual, weight, reweightAuxSum4rate_);
      o->stn_2()   -> addWrms(DT_RATE, *this, residual, weight, reweightAuxSum4rate_);
      o->src  ()   -> addWrms(DT_RATE, *this, residual, weight, reweightAuxSum4rate_);
      o->baseline()-> addWrms(DT_RATE, *this, residual, weight, reweightAuxSum4rate_);
      band         -> addWrms(DT_RATE, *this, residual, weight, reweightAuxSum4rate_);
    };
  };
  //
  // update station dependent time-varying estimates:
  double                        scale_c, scale_z, scale_g;
  scale_c = parameters->getClock0().getScale();
  scale_z = parameters->getZenith().getScale();
  scale_g = parameters->getAtmGrad().getScale();
  // the first station:
  if (auxObs_1_ && auxObs_1_->isAttr(SgVlbiAuxObservation::Attr_PROCESSED))
  {
    if (stn_1_->pClock0()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      auxObs_1_->setEstClocks(stn_1_->pClock0()->getSolution()*scale_c);
      auxObs_1_->setEstClocksSigma(stn_1_->pClock0()->getSigma()*scale_c);
    }
    else
    {
      auxObs_1_->setEstClocks(0.0);
      auxObs_1_->setEstClocksSigma(0.0);
    };
    //
    auxObs_1_->setEstZenithDelay(stn_1_->pZenithDelay()->getSolution()*scale_z);
    auxObs_1_->setEstZenithDelaySigma(stn_1_->pZenithDelay()->getSigma()*scale_z);
    //
    auxObs_1_->setEstAtmGradN(stn_1_->pAtmGradN()->getSolution()*scale_g);
    auxObs_1_->setEstAtmGradNSigma(stn_1_->pAtmGradN()->getSigma()*scale_g);
    auxObs_1_->setEstAtmGradE(stn_1_->pAtmGradE()->getSolution()*scale_g);
    auxObs_1_->setEstAtmGradESigma(stn_1_->pAtmGradE()->getSigma()*scale_g);
  };
  // the second station:
  if (auxObs_2_ && auxObs_2_->isAttr(SgVlbiAuxObservation::Attr_PROCESSED))
  {
    if (stn_2_->pClock0()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      auxObs_2_->setEstClocks(stn_2_->pClock0()->getSolution()*scale_c);
      auxObs_2_->setEstClocksSigma(stn_2_->pClock0()->getSigma()*scale_c);
    }
    else
    {
      auxObs_2_->setEstClocks(0.0);
      auxObs_2_->setEstClocksSigma(0.0);
    };
    //
    auxObs_2_->setEstZenithDelay(stn_2_->pZenithDelay()->getSolution()*scale_z);
    auxObs_2_->setEstZenithDelaySigma(stn_2_->pZenithDelay()->getSigma()*scale_z);
    //
    auxObs_2_->setEstAtmGradN(stn_2_->pAtmGradN()->getSolution()*scale_g);
    auxObs_2_->setEstAtmGradNSigma(stn_2_->pAtmGradN()->getSigma()*scale_g);
    auxObs_2_->setEstAtmGradE(stn_2_->pAtmGradE()->getSolution()*scale_g);
    auxObs_2_->setEstAtmGradESigma(stn_2_->pAtmGradE()->getSigma()*scale_g);
  };
};



//
const SgVector& SgVlbiObservation::o_c()
{
  return storage4O_C;
};



//
const SgVector& SgVlbiObservation::sigma()
{
  return storage4Sigmas;
};



//
void SgVlbiObservation::fillPartials(SgTaskManager* mgr)
{
  if (mgr->getTask()->config()->getDataType() == SgTaskConfig::VDT_DELAY)
    fillPartials4delay(mgr);
  else if (mgr->getTask()->config()->getDataType() == SgTaskConfig::VDT_RATE)
    fillPartials4rate(mgr);
  else
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + 
      "::fillPartials(): unexpected data type");
};



//
void SgVlbiObservation::fillPartials4delay(SgTaskManager* mgr)
{
  SgTaskConfig                 *cfg=mgr->getTask()->config();
  QString                       str;
  double                        dt1 = (*this - session_->tRefer());
  double                        dt2 = (dt1 + getCalcConsensusDelay()/DAY2SEC);
  parameters_.clear();

  // partials for clocks:
  double                        d1=1.0;
  double                        d2=1.0;
  int                           nCl;
  nCl = mgr->getTask()->parameters()->getClock0Mode()==SgParameterCfg::PM_STC ? 1 :
    std::max(stn_1_->getClocksModelOrder(), stn_2_->getClocksModelOrder());

  for (int i=0; i<nCl; i++)
  {
    // station #1:
    if (stn_1_->pClocks(i)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pClocks(i)->setD(-d1);
      parameters_.append(stn_1_->pClocks(i));
    };
    // station #2:
    if (stn_2_->pClocks(i)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pClocks(i)->setD(d2);
      parameters_.append(stn_2_->pClocks(i));
    };
    d1 *= dt1;
    d2 *= dt2;
  };
  // clock breaks:
  if (stn_1_->clockBreaks().size())
    stn_1_->clockBreaks().propagatePartials(parameters_, *this, dt1, -1.0);
  if (stn_2_->clockBreaks().size())
    stn_2_->clockBreaks().propagatePartials(parameters_, *this, dt2,  1.0);


#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_1_->pClocks(0)->getName()), stn_1_->pClocks(0)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  
    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_1_->pClocks(1)->getName()), stn_1_->pClocks(1)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  
    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_1_->pClocks(2)->getName()), stn_1_->pClocks(2)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);

    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_2_->pClocks(0)->getName()), stn_2_->pClocks(0)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  
    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_2_->pClocks(1)->getName()), stn_2_->pClocks(1)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  
    str.sprintf("Partials: %-30s: %32.15f", 
      qPrintable(stn_2_->pClocks(2)->getName()), stn_2_->pClocks(2)->getD());
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  };
#endif

  //
  //
  //
  // refraction:
  if (auxObs_1_ && auxObs_2_)
  {
    if (stn_1_->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pZenithDelay()->setD(-auxObs_1_->tmdActual()->getMap1_delay()/vLight);
      parameters_.append(stn_1_->pZenithDelay());
    };

    if (stn_2_->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pZenithDelay()->setD( auxObs_2_->tmdActual()->getMap1_delay()/vLight);
      parameters_.append(stn_2_->pZenithDelay());
    };

    if (stn_1_->pAtmGradN()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pAtmGradN()->setD(-auxObs_1_->tmdActual()->getGrdN_delay()/vLight);
      stn_1_->pAtmGradE()->setD(-auxObs_1_->tmdActual()->getGrdE_delay()/vLight);
      parameters_.append(stn_1_->pAtmGradN());
      parameters_.append(stn_1_->pAtmGradE());
    };
    if (stn_2_->pAtmGradN()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pAtmGradN()->setD( auxObs_2_->tmdActual()->getGrdN_delay()/vLight);
      stn_2_->pAtmGradE()->setD( auxObs_2_->tmdActual()->getGrdE_delay()/vLight);
      parameters_.append(stn_2_->pAtmGradN());
      parameters_.append(stn_2_->pAtmGradE());
    };
  };



#ifdef DBG_MODE_  
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Partials: %-30s: %32.15f or %32.15f", 
      qPrintable(stn_1_->pZenithDelay()->getName()), 
      stn_1_->pZenithDelay()->getD(),
      stn_1_->pZenithDelay()->getD()*vLight);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);

    str.sprintf("Partials: %-30s: %32.15f or %32.15f",
      qPrintable(stn_2_->pZenithDelay()->getName()), 
      stn_2_->pZenithDelay()->getD(),
      stn_2_->pZenithDelay()->getD()*vLight);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() +
      "::fillPartials4delay(): " + str);
  };
#endif //DBG_MODE_ 

  // Station #1:
  if (stn_1_->pRx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_1_->pRx()->setD(getDdel_dR_1().at(X_AXIS));
    stn_1_->pRy()->setD(getDdel_dR_1().at(Y_AXIS));
    stn_1_->pRz()->setD(getDdel_dR_1().at(Z_AXIS));
    parameters_.append(stn_1_->pRx());
    parameters_.append(stn_1_->pRy());
    parameters_.append(stn_1_->pRz());
  };

  // Station #2:
  if (stn_2_->pRx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_2_->pRx()->setD(getDdel_dR_2().at(X_AXIS));
    stn_2_->pRy()->setD(getDdel_dR_2().at(Y_AXIS));
    stn_2_->pRz()->setD(getDdel_dR_2().at(Z_AXIS));
    parameters_.append(stn_2_->pRx());
    parameters_.append(stn_2_->pRy());
    parameters_.append(stn_2_->pRz());
  };
    
  // Axis offsets:
  if (stn_1_->pAxisOffset()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_1_->pAxisOffset()->setD( auxObs_1_->getDdel_dAxsOfs() );
    parameters_.append(stn_1_->pAxisOffset());
  };
  if (stn_2_->pAxisOffset()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_2_->pAxisOffset()->setD(-auxObs_2_->getDdel_dAxsOfs() );
    parameters_.append(stn_2_->pAxisOffset());
  };
  // Source:
  if (src_->pRA()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    src_->pRA()->setD(getDdel_dRA());
    src_->pDN()->setD(getDdel_dDN());
    parameters_.append(src_->pRA());
    parameters_.append(src_->pDN());
  };

  //
  //
  double                        scl=360.0*3600.0/2.0/M_PI;
  if (activeObs())
  {
    {
      // multipoint SS-model:
      if (cfg->getHave2ApplySourceSsm() &&
        src_->getIsSsModelEstimating() && src_->sModel().size())
      {
        src_->processPartials4SrcStructModel(parameters_, 
          activeObs()->getReferenceFrequency()*1.0e6,
          activeObs()->getUvFrPerAsec(0)*scl, 
          activeObs()->getUvFrPerAsec(1)*scl);
      };
    };
  };
  //

  // Baseline:
  if (baseline_->pClock()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    baseline_->pClock()->setD(1.0);
    parameters_.append(baseline_->pClock());
  };
  if (baseline_->pBx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    baseline_->pBx()->setD(getDdel_dR_2().at(X_AXIS));
    baseline_->pBy()->setD(getDdel_dR_2().at(Y_AXIS));
    baseline_->pBz()->setD(getDdel_dR_2().at(Z_AXIS));
    parameters_.append(baseline_->pBx());
    parameters_.append(baseline_->pBy());
    parameters_.append(baseline_->pBz());
  };
  // EOP:
  // UT1:
  if (session_->pUT1()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pUT1()->setD(getDdel_dUT1());
    parameters_.append(session_->pUT1());
  };
  if (session_->pUT1Rate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pUT1Rate()->setD(getDdel_dUT1()*dt1);
    parameters_.append(session_->pUT1Rate());
  };
  // P_x & P_y:
  if (session_->pPolusX()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pPolusX()->setD(getDdel_dPx());
    session_->pPolusY()->setD(getDdel_dPy());
    parameters_.append(session_->pPolusX());
    parameters_.append(session_->pPolusY());
  };
  // rates of P_x & P_y:
  if (session_->pPolusXRate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pPolusXRate()->setD(getDdel_dPx()*dt1);
    session_->pPolusYRate()->setD(getDdel_dPy()*dt1);
    parameters_.append(session_->pPolusXRate());
    parameters_.append(session_->pPolusYRate());
  };
  // CIP_X & CIP_Y:
  if (session_->pNutX()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pNutX()->setD(getDdel_dCipX());
    session_->pNutY()->setD(getDdel_dCipY());
    parameters_.append(session_->pNutX());
    parameters_.append(session_->pNutY());
  };
  // rates of CIP_X & CIP_Y:
  if (session_->pNutXRate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pNutXRate()->setD(getDdel_dCipX()*dt1);
    session_->pNutYRate()->setD(getDdel_dCipY()*dt1);
    parameters_.append(session_->pNutXRate());
    parameters_.append(session_->pNutYRate());
  };

  // notify PWLs:
  for (int i=0; i<parameters_.size(); i++)
    parameters_.at(i)->setDataDomain(SgPartial::DD_OFFSET);
};



//
void SgVlbiObservation::fillPartials4rate(SgTaskManager* mgr)
{
  QString                       str;
  double                        dt1=(*this - session_->tRefer());
  double                        dt2=(dt1 + getCalcConsensusDelay()/DAY2SEC);
  parameters_.clear();


  // partials for clocks:
  int                           nCl;
  nCl = mgr->getTask()->parameters()->getClock0Mode()==SgParameterCfg::PM_STC ? 1 :
    std::max(stn_1_->getClocksModelOrder(), stn_2_->getClocksModelOrder());

  // special case:
  if (mgr->getTask()->parameters()->getClock0Mode()!=SgParameterCfg::PM_NONE &&
      mgr->getTask()->parameters()->getClock0Mode()!=SgParameterCfg::PM_LOC   )
  {
    if (stn_1_->pClocks(0)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pClocks(0)->setD(-1.0/DAY2SEC);
      parameters_.append(stn_1_->pClocks(0));
    };
    // station #2:
    if (stn_2_->pClocks(0)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pClocks(0)->setD( 1.0/DAY2SEC);
      parameters_.append(stn_2_->pClocks(0));
    };
  };
  //
  //
  double                        d1, d2;
  d1 = d2 = 1.0;
  for (int i=1; i<nCl; i++)
  {
    // station #1:
    if (stn_1_->pClocks(i)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pClocks(i)->setD(-i*d1/DAY2SEC);
      parameters_.append(stn_1_->pClocks(i));
    };
    // station #2:
    if (stn_2_->pClocks(i)->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pClocks(i)->setD( i*d2/DAY2SEC);
      parameters_.append(stn_2_->pClocks(i));
    };
    d1 *= dt1 /* /DAY2SEC */;
    d2 *= dt2 /* /DAY2SEC */;
  };
  // clock breaks:
  if (stn_1_->clockBreaks().size())
    stn_1_->clockBreaks().propagatePartials4rates(parameters_, *this, dt1, -1.0/DAY2SEC);
  if (stn_2_->clockBreaks().size())
    stn_2_->clockBreaks().propagatePartials4rates(parameters_, *this, dt2,  1.0/DAY2SEC);
  //
  // 
  // troposphere:
  //
  if (auxObs_1_ && auxObs_2_)
  {
    if (stn_1_->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pZenithDelay()->setD(-auxObs_1_->tmdActual()->getMap1_rate()/vLight);
      parameters_.append(stn_1_->pZenithDelay());
    };
    if (stn_2_->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pZenithDelay()->setD( auxObs_2_->tmdActual()->getMap1_rate()/vLight);
      parameters_.append(stn_2_->pZenithDelay());
    };
    if (stn_1_->pAtmGradN()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_1_->pAtmGradN()->setD(-auxObs_1_->tmdActual()->getGrdN_rate()/vLight);
      stn_1_->pAtmGradE()->setD(-auxObs_1_->tmdActual()->getGrdE_rate()/vLight);
      parameters_.append(stn_1_->pAtmGradN());
      parameters_.append(stn_1_->pAtmGradE());
    };
    if (stn_2_->pAtmGradN()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      stn_2_->pAtmGradN()->setD( auxObs_2_->tmdActual()->getGrdN_rate()/vLight);
      stn_2_->pAtmGradE()->setD( auxObs_2_->tmdActual()->getGrdE_rate()/vLight);
      parameters_.append(stn_2_->pAtmGradN());
      parameters_.append(stn_2_->pAtmGradE());
    };
  };

  // Station #1:
  if (stn_1_->pRx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_1_->pRx()->setD( getDrat_dR_1().at(X_AXIS));
    stn_1_->pRy()->setD( getDrat_dR_1().at(Y_AXIS));
    stn_1_->pRz()->setD( getDrat_dR_1().at(Z_AXIS));
    parameters_.append(stn_1_->pRx());
    parameters_.append(stn_1_->pRy());
    parameters_.append(stn_1_->pRz());
  };

  // Station #2:
  if (stn_2_->pRx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_2_->pRx()->setD( getDrat_dR_2().at(X_AXIS));
    stn_2_->pRy()->setD( getDrat_dR_2().at(Y_AXIS));
    stn_2_->pRz()->setD( getDrat_dR_2().at(Z_AXIS));
    parameters_.append( stn_2_->pRx());
    parameters_.append( stn_2_->pRy());
    parameters_.append( stn_2_->pRz());
  };

  // Axis offsets:
  if (stn_1_->pAxisOffset()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_1_->pAxisOffset()->setD( auxObs_1_->getDrat_dAxsOfs() );
    parameters_.append(stn_1_->pAxisOffset());
  };
  if (stn_2_->pAxisOffset()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    stn_2_->pAxisOffset()->setD(-auxObs_2_->getDrat_dAxsOfs() );
    parameters_.append(stn_2_->pAxisOffset());
  };
  // Source:
  if (src_->pRA()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    src_->pRA()->setD(getDrat_dRA());
    src_->pDN()->setD(getDrat_dDN());
    parameters_.append(src_->pRA());
    parameters_.append(src_->pDN());
  };

  // Baseline:
  if (baseline_->pBx()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    baseline_->pBx()->setD(getDrat_dR_2().at(X_AXIS));
    baseline_->pBy()->setD(getDrat_dR_2().at(Y_AXIS));
    baseline_->pBz()->setD(getDrat_dR_2().at(Z_AXIS));
    parameters_.append(baseline_->pBx());
    parameters_.append(baseline_->pBy());
    parameters_.append(baseline_->pBz());
  };
  // EOP:
  // UT1:
  if (session_->pUT1()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pUT1()->setD(getDrat_dUT1());
    parameters_.append(session_->pUT1());
  };
  if (session_->pUT1Rate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pUT1Rate()->setD(getDrat_dUT1()*dt1);
    parameters_.append(session_->pUT1Rate());
  };
  // P_x & P_y:
  if (session_->pPolusX()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pPolusX()->setD(getDrat_dPx());
    session_->pPolusY()->setD(getDrat_dPy());
    parameters_.append(session_->pPolusX());
    parameters_.append(session_->pPolusY());
  };
  // rates of P_x & P_y:
  if (session_->pPolusXRate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pPolusXRate()->setD(getDrat_dPx()*dt1);
    session_->pPolusYRate()->setD(getDrat_dPy()*dt1);
    parameters_.append(session_->pPolusXRate());
    parameters_.append(session_->pPolusYRate());
  };
  // CIP_X & CIP_Y:
  if (session_->pNutX()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pNutX()->setD(getDrat_dCipX());
    session_->pNutY()->setD(getDrat_dCipY());
    parameters_.append(session_->pNutX());
    parameters_.append(session_->pNutY());
  };
  // rates of CIP_X & CIP_Y:
  if (session_->pNutXRate()->isAttr(SgPartial::Attr_IS_IN_RUN))
  {
    session_->pNutXRate()->setD(getDrat_dCipX()*dt1);
    session_->pNutYRate()->setD(getDrat_dCipY()*dt1);
    parameters_.append(session_->pNutXRate());
    parameters_.append(session_->pNutYRate());
  };

  // notify PWLs:
  for (int i=0; i<parameters_.size(); i++)
    parameters_.at(i)->setDataDomain(SgPartial::DD_RATE);
};


//
void SgVlbiObservation::evalExtApriori_StnPos(const SgTaskConfig* config, double& dD, double& dR)
{
  QString                       str;
  double                        dT=0.0;
  SgMJD                         t0(session_->getApStationVelocities().getT0());

  dD = dR = 0.0;
  
  if (config->getIsSolveCompatible())
    dT = session_->tRefer() - t0;
  else
    dT = *this - t0;

  Sg3dVector                    dR1_ea(stn_1_->getR_ea() + stn_1_->getV_ea()*dT);
  Sg3dVector                    dR2_ea(stn_2_->getR_ea() + stn_2_->getV_ea()*dT);
  Sg3dVector                    dR1 = stn_1_->getR() - dR1_ea;
  Sg3dVector                    dR2 = stn_2_->getR() - dR2_ea;

  // the signs for the first station are already reverted:
  dD = getDdel_dR_1()*dR1 + getDdel_dR_2()*dR2;
  dR = getDrat_dR_1()*dR1 + getDrat_dR_2()*dR2;

#ifdef DBG_MODE_2_
  if (logger->isEligible(SgLogger::DBG, SgLogger::DELAY))
  {
    str.sprintf("Epoch     : %s", qPrintable(toString()));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("T0        : %s", qPrintable(t0.toString()));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("difference: %14.8f (days)", dT);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    // stn #1
    str.sprintf("St_1, database positions: %13.4f %13.4f %13.4f", 
      stn_1_->getR().at(X_AXIS), 
      stn_1_->getR().at(Y_AXIS), 
      stn_1_->getR().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_1, e.a.file positions: %13.4f %13.4f %13.4f", 
      stn_1_->getR_ea().at(X_AXIS), 
      stn_1_->getR_ea().at(Y_AXIS), 
      stn_1_->getR_ea().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_1, e.a.file velocity : %13.10f %13.10f %13.10f (mm/yr)", 
      stn_1_->getV_ea().at(X_AXIS)*1000.0*365.25, 
      stn_1_->getV_ea().at(Y_AXIS)*1000.0*365.25, 
      stn_1_->getV_ea().at(Z_AXIS)*1000.0*365.25);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_1, e.a.file velocity : %13.10f %13.10f %13.10f (m/d)", 
      stn_1_->getV_ea().at(X_AXIS), 
      stn_1_->getV_ea().at(Y_AXIS), 
      stn_1_->getV_ea().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_1, e.a.position at t : %13.4f %13.4f %13.4f", 
      dR1_ea.at(X_AXIS), dR1_ea.at(Y_AXIS), dR1_ea.at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_1, difference db-e.a : %10.7f %10.7f %10.7f", 
      dR1.at(X_AXIS), dR1.at(Y_AXIS), dR1.at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    // stn #2
    str.sprintf("St_2, database positions: %13.4f %13.4f %13.4f", 
      stn_2_->getR().at(X_AXIS), 
      stn_2_->getR().at(Y_AXIS), 
      stn_2_->getR().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_2, e.a.file positions: %13.4f %13.4f %13.4f", 
      stn_2_->getR_ea().at(X_AXIS),
      stn_2_->getR_ea().at(Y_AXIS),
      stn_2_->getR_ea().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_2, e.a.file velocity : %13.10f %13.10f %13.10f (mm/yr)", 
      stn_2_->getV_ea().at(X_AXIS)*1000.0*365.25, 
      stn_2_->getV_ea().at(Y_AXIS)*1000.0*365.25, 
      stn_2_->getV_ea().at(Z_AXIS)*1000.0*365.25);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
    str.sprintf("St_2, e.a.file velocity : %13.10f %13.10f %13.10f (m/d)", 
      stn_2_->getV_ea().at(X_AXIS),
      stn_2_->getV_ea().at(Y_AXIS),
      stn_2_->getV_ea().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    str.sprintf("St_2, e.a.position at t : %13.4f %13.4f %13.4f", 
      dR2_ea.at(X_AXIS), dR2_ea.at(Y_AXIS), dR2_ea.at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    str.sprintf("St_2, difference db-e.a : %10.7f %10.7f %10.7f", 
      dR2.at(X_AXIS), dR2.at(Y_AXIS), dR2.at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    str.sprintf("Partials, dTau/dSt_1_i: %20.12g %20.12g %20.12g",
      getDdel_dR_1().at(X_AXIS), 
      getDdel_dR_1().at(Y_AXIS), 
      getDdel_dR_1().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    str.sprintf("Partials, dTau/dSt_2_i: %20.12g %20.12g %20.12g",
      getDdel_dR_2().at(X_AXIS), 
      getDdel_dR_2().at(Y_AXIS), 
      getDdel_dR_2().at(Z_AXIS));
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);

    str.sprintf("Resulted correction (ps): %8.4f", dD*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::DELAY, className() + ": site position mapping: " + str);
  };
#endif

  return;
};



//
void SgVlbiObservation::evalExtApriori_SrcPos(double& dD, double& dR)
{
  dD =  getDdel_dRA()*(src_->getRA() - src_->getRA_ea()) +
        getDdel_dDN()*(src_->getDN() - src_->getDN_ea())  ;
  dR =  getDrat_dRA()*(src_->getRA() - src_->getRA_ea()) +
        getDrat_dDN()*(src_->getDN() - src_->getDN_ea())  ;
};



//
void SgVlbiObservation::evalExtApriori_AxsOfs(double& dD, double& dR)
{
  dD = dR = 0.0;
  if (auxObs_1_ && auxObs_2_)
  {
    dD = -auxObs_1_->getDdel_dAxsOfs()*(stn_1_->getAxisOffset() - stn_1_->getAxisOffset_ea()) + 
          auxObs_2_->getDdel_dAxsOfs()*(stn_2_->getAxisOffset() - stn_2_->getAxisOffset_ea()) ;
    dR = -auxObs_1_->getDrat_dAxsOfs()*(stn_1_->getAxisOffset() - stn_1_->getAxisOffset_ea()) + 
          auxObs_2_->getDrat_dAxsOfs()*(stn_2_->getAxisOffset() - stn_2_->getAxisOffset_ea()) ;
  };
};



//
void SgVlbiObservation::evalExtApriori_MeanGrads(double& dD, double& dR)
{
  dD = dR = 0.0;
  if (session_->hasGradPartials())
  {
    dD = - stn_1_->getGradNorth()*auxObs_1_->getDdel_dTzdGrdN()
         - stn_1_->getGradEast ()*auxObs_1_->getDdel_dTzdGrdE()
         + stn_2_->getGradNorth()*auxObs_2_->getDdel_dTzdGrdN()
         + stn_2_->getGradEast ()*auxObs_2_->getDdel_dTzdGrdE();
    dD /= -1.0*vLight;
    dR = - stn_1_->getGradNorth()*auxObs_1_->getDrat_dTzdGrdN()
         - stn_1_->getGradEast ()*auxObs_1_->getDrat_dTzdGrdE()
         + stn_2_->getGradNorth()*auxObs_2_->getDrat_dTzdGrdN()
         + stn_2_->getGradEast ()*auxObs_2_->getDrat_dTzdGrdE();
    dD /= -1.0*vLight;
  }
  else
  {
//    c = (stn_2_->getGradientDelay() - stn_1_->getGradientDelay());
  };
  //
//// invert sign: should to be added to theoretical delay:
//  return -c/vLight;
};



//
void SgVlbiObservation::zerofyIono()
{
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    it.value()->zerofyIono();
};



//
void SgVlbiObservation::calcIonoCorrections(const SgTaskConfig* cfg)
{
  propagateAttr(SgVlbiMeasurement::Attr_REQ_IONO_CORR); // the iono correction calcs were called
  eradicateAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR); // reset the flags
  
  QString                       str("");
  if (observableByKey_.size() != 2)
  {
    if (!cfg && observableByKey_.size()>2)
      logger->write(SgLogger::INF, SgLogger::IONO, className() +
        ": calcIonoCorrections(): cannot calculate, number of bands (" + 
        str.setNum(observableByKey_.size()) + ") is not equal 2; " +
        baseline_->getKey() + " on " + src_->getKey());
    return;
  };

  bool                          isSimpleMode=(cfg?false:true);
  bool                          hasEffFreqs=true;
  bool                          isPima=session()->getCppsSoft()==SgVlbiSessionInfo::CPPS_PIMA;
//double                        coeff4SB, coeff4GR, coeff4GRp, coeff4PH, coeff4RT;
  double                        coeff4SB, coeff4GR, coeff4PH, coeff4RT;
  double                        sumSigs2_SB, sumSigs2_GR, sumSigs2_PH, sumSigs2_RT;
  double                        sDif, xDif, rDif, pDif;
//double                        sbdQ, grdQ, phrQ;
  double                        corr4SB[2], corr4GR[2], dClB4GR[2], corr4PH[2], corr4RT[2];
  double                        sigma4SB[2], sigma4GR[2], sigma4PH[2], sigma4RT[2];
  SgVlbiObservable             *o[2];
  int                           idx;
  coeff4SB = coeff4GR = coeff4PH = coeff4RT = 0.0;
  sumSigs2_SB = sumSigs2_GR = sumSigs2_PH = sumSigs2_RT = 0.0;
//coeff4SB = coeff4GR = coeff4GRp = coeff4PH = coeff4RT = 0.0;
//sbdQ = grdQ = phrQ = 0.0;

//  if (o->owner()->session()->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA && o->getQualityFactor()==0)


  for (int i=0; i<2; i++)
  {
    o[i] = NULL;
    corr4SB[i] = corr4GR[i] = corr4PH[i] = corr4RT[i] = sigma4SB[i] = sigma4GR[i] =
      sigma4PH[i] = sigma4RT[i] = 0.0;
    dClB4GR[i] = (stn_2_->clockBreaks().value(*this) - stn_1_->clockBreaks().value(*this))*1.0e-09;
  
  /*  
    if (o[i]->grDelay().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      logger->write(SgLogger::WRN, SgLogger::IONO, className() +
        ": calcIonoCorrections(): cannot calculate, the group delay ionosphere effective frequency is "
        "zero on " + o[i]->strId());
    }
    if (o[i]->phDelay().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      logger->write(SgLogger::WRN, SgLogger::IONO, className() +
        ": calcIonoCorrections(): cannot calculate, the phase delay ionosphere effective frequency is "
        "zero on " + o[i]->strId());
    }
    if (o[i]->phDRate().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      logger->write(SgLogger::WRN, SgLogger::IONO, className() +
        ": calcIonoCorrections(): cannot calculate, the phase rate ionosphere effective frequency is "
        "zero on " + o[i]->strId());
    }
    */
  };
  //
  
  
  QMap<double, SgVlbiObservable*>
                                observableByFrq;
  QMap<QString, SgVlbiObservable*>::iterator
                                it_name;
  idx = 0;
  for (it_name=observableByKey_.begin(); it_name!=observableByKey_.end() && idx<2; ++it_name, idx++)
  {
    SgVlbiObservable*           oo=it_name.value();
    if (observableByFrq.contains(oo->getReferenceFrequency()))
    {
      if (!(isPima && oo->getQualityFactor()==0))
        logger->write(SgLogger::ERR, SgLogger::IONO, className() +
          ": calcIonoCorrections(): cannot calculate, the reference frequency is the same for two bands "
          "on " + oo->strId());
      return;
    }
    else
      observableByFrq.insert(oo->getReferenceFrequency(), oo);

    if (oo->grDelay().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      if (!(isPima && oo->getQualityFactor()==0))
        logger->write(SgLogger::WRN, SgLogger::IONO, className() +
          ": calcIonoCorrections(): cannot calculate, the group delay ionosphere effective frequency is "
          "zero on " + oo->strId());
    }
    if (oo->phDelay().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      if (!(isPima && oo->getQualityFactor()==0))
        logger->write(SgLogger::WRN, SgLogger::IONO, className() +
          ": calcIonoCorrections(): cannot calculate, the phase delay ionosphere effective frequency is "
          "zero on " + oo->strId());
    }
    if (oo->phDRate().getEffFreq() <= 0.0)
    {
      hasEffFreqs = false;
      if (!(isPima && oo->getQualityFactor()==0))
        logger->write(SgLogger::WRN, SgLogger::IONO, className() +
          ": calcIonoCorrections(): cannot calculate, the phase rate ionosphere effective frequency is "
          "zero on " + oo->strId());
    }
  };
  if (!hasEffFreqs)
    return;

  
  idx = 0;
  QMap<double, SgVlbiObservable*>::iterator it_frq;
  for (it_frq=observableByFrq.begin(); it_frq!=observableByFrq.end() && idx<2; ++it_frq, idx++)
  {
    SgVlbiObservable           *obs=it_frq.value();
    o[idx] = obs;
    SgVlbiStationInfo          *stn1=obs->stn_1();
    SgVlbiStationInfo          *stn2=obs->stn_2();
    if (stn1 && stn2)
      dClB4GR[idx] += (stn2->clockBreaks().value(*this) - stn1->clockBreaks().value(*this))*1.0e-09;
  };

  // 0 -> S
  // 1 -> X
  /*
  if (false)
  {
    coeff4GR = o[0]->getReferenceFrequency()*o[0]->getReferenceFrequency()/
      (o[1]->getReferenceFrequency()*o[1]->getReferenceFrequency() -
       o[0]->getReferenceFrequency()*o[0]->getReferenceFrequency());
  }
  else
  {
    coeff4GR = o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq()/
      (o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq() -
       o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq());
  };
  */

  coeff4GR = o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq()/
    (o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq() -
     o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq());
  coeff4PH = o[0]->phDelay().getEffFreq()*o[0]->phDelay().getEffFreq()/
    (o[1]->phDelay().getEffFreq()*o[1]->phDelay().getEffFreq() -
     o[0]->phDelay().getEffFreq()*o[0]->phDelay().getEffFreq());
  coeff4RT = o[0]->phDRate().getEffFreq()*o[0]->phDRate().getEffFreq()/
    (o[1]->phDRate().getEffFreq()*o[1]->phDRate().getEffFreq() -
     o[0]->phDRate().getEffFreq()*o[0]->phDRate().getEffFreq());
  //coeff4GRp= (1.0 + coeff4GR)/coeff4GR;
  coeff4SB = coeff4GR;

  sDif = o[1]->sbDelay().getValue() - o[0]->sbDelay().getValue();
  xDif = o[1]->grDelay().getValue() + o[1]->grDelay().ambiguity() -
        (o[0]->grDelay().getValue() + o[0]->grDelay().ambiguity()) - (dClB4GR[1] - dClB4GR[0]);
  pDif = o[1]->phDelay().getValue() + o[1]->phDelay().ambiguity() -
        (o[0]->phDelay().getValue() + o[0]->phDelay().ambiguity());
  rDif = o[1]->phDRate().getValue() - o[0]->phDRate().getValue();

  o[1]->setSbdDiffBand(-sDif); // opposite band with respect to this one
  o[0]->setSbdDiffBand( sDif);
  o[1]->setGrdDiffBand(-xDif);
  o[0]->setGrdDiffBand( xDif);
  o[1]->setPhrDiffBand(-rDif);
  o[0]->setPhrDiffBand( rDif);

/*
  o[0]->grDelay().setQ2(o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq()/
                       (o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq() -
                        o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq()));
*/
/*
  sbdQ = o[1]->phDelay().getEffFreq()*o[1]->phDelay().getEffFreq()*
         o[0]->phDelay().getEffFreq()*o[0]->phDelay().getEffFreq()/
        (o[1]->phDelay().getEffFreq()*o[1]->phDelay().getEffFreq() -
         o[0]->phDelay().getEffFreq()*o[0]->phDelay().getEffFreq())*sDif;
  if (false)
  {
   grdQ = o[1]->getReferenceFrequency()*o[1]->getReferenceFrequency()*
          o[0]->getReferenceFrequency()*o[0]->getReferenceFrequency()/
         (o[1]->getReferenceFrequency()*o[1]->getReferenceFrequency() -
          o[0]->getReferenceFrequency()*o[0]->getReferenceFrequency())*xDif;
  }
  else
  {
    grdQ = o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq()*
           o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq()/
          (o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq() -
           o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq())*xDif;
  };
  grdQ = o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq()*
         o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq()/
        (o[1]->grDelay().getEffFreq()*o[1]->grDelay().getEffFreq() -
         o[0]->grDelay().getEffFreq()*o[0]->grDelay().getEffFreq())*xDif;
  phrQ = o[1]->phDRate().getEffFreq()*o[1]->phDRate().getEffFreq()*
         o[0]->phDRate().getEffFreq()*o[0]->phDRate().getEffFreq()/
        (o[1]->phDRate().getEffFreq()*o[1]->phDRate().getEffFreq() -
         o[0]->phDRate().getEffFreq()*o[0]->phDRate().getEffFreq())*rDif;
*/

  o[0]->grDelay().setQ2(1.0 + coeff4GR);
  o[1]->grDelay().setQ2(coeff4GR);

  o[0]->sbDelay().setQ2(1.0 + coeff4SB);
  o[1]->sbDelay().setQ2(coeff4SB);
  
  o[0]->phDelay().setQ2(1.0 + coeff4PH);
  o[1]->phDelay().setQ2(coeff4PH);
  
  o[0]->phDRate().setQ2(1.0 + coeff4RT);
  o[1]->phDRate().setQ2(coeff4RT);


  sumSigs2_SB = sqrt( o[1]->sbDelay().getSigma()*o[1]->sbDelay().getSigma() +
                      o[0]->sbDelay().getSigma()*o[0]->sbDelay().getSigma() );

  corr4SB[1] = - coeff4SB*sDif;
  corr4SB[0] = - (1.0 + coeff4SB)*sDif;
  sigma4SB[1] = coeff4SB*sumSigs2_SB;
  sigma4SB[0] = (1.0 + coeff4SB)*sumSigs2_SB;

  o[1]->sbDelay().setIonoValue(corr4SB[1]);
  o[1]->sbDelay().setIonoSigma(sigma4SB[1]);
  o[0]->sbDelay().setIonoValue(corr4SB[0]);
  o[0]->sbDelay().setIonoSigma(sigma4SB[0]);
  
  o[0]->sbDelay().addAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR);
  o[1]->sbDelay().addAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR);
  if (!isSimpleMode)
  {
    corr4GR[1] = - coeff4GR*xDif;
    corr4RT[1] = - coeff4RT*rDif;
    corr4PH[1] = - coeff4PH*pDif;
    corr4GR[0] = - (1.0 + coeff4GR)*xDif;
    corr4RT[0] = - (1.0 + coeff4RT)*rDif;
    corr4PH[0] = - (1.0 + coeff4PH)*pDif;
    
    sumSigs2_GR = sqrt( o[1]->grDelay().getSigma()*o[1]->grDelay().getSigma() +
                        o[0]->grDelay().getSigma()*o[0]->grDelay().getSigma() );
    sumSigs2_PH = sqrt( o[1]->phDelay().getSigma()*o[1]->phDelay().getSigma() +
                        o[0]->phDelay().getSigma()*o[0]->phDelay().getSigma() );
    sumSigs2_RT = sqrt( o[1]->phDRate().getSigma()*o[1]->phDRate().getSigma() +
                        o[0]->phDRate().getSigma()*o[0]->phDRate().getSigma() );

    if (cfg->getUseOldMode4IonosphereSigma()) // csolve_2023-10-31/progs/solve/sdbh/ionoc.f
      sigma4GR[1] = coeff4GR*sqrt(
        o[1]->grDelay().getSigma()*o[1]->grDelay().getSigma()*(coeff4GR + 2.0)/coeff4GR +
        o[0]->grDelay().getSigma()*o[0]->grDelay().getSigma());
    else 
      sigma4GR[1] = coeff4GR*sumSigs2_GR;

    sigma4PH[1] = coeff4PH*sumSigs2_PH;
    sigma4RT[1] = coeff4RT*sumSigs2_RT;

    sigma4GR[0] = (1.0 + coeff4GR)*sumSigs2_GR;
    sigma4PH[0] = (1.0 + coeff4PH)*sumSigs2_PH;
    sigma4RT[0] = (1.0 + coeff4RT)*sumSigs2_RT;

    o[1]->grDelay().setIonoValue(corr4GR[1]);
    o[1]->grDelay().setIonoSigma(sigma4GR[1]);
    o[0]->grDelay().setIonoValue(corr4GR[0]);
    o[0]->grDelay().setIonoSigma(sigma4GR[0]);

    o[1]->phDelay().setIonoValue(corr4PH[1]);
    o[1]->phDelay().setIonoSigma(sigma4PH[1]);
    o[0]->phDelay().setIonoValue(corr4PH[0]);
    o[0]->phDelay().setIonoSigma(sigma4PH[0]);

    o[1]->phDRate().setIonoValue(corr4RT[1]);
    o[1]->phDRate().setIonoSigma(sigma4RT[1]);
    o[0]->phDRate().setIonoValue(corr4RT[0]);
    o[0]->phDRate().setIonoSigma(sigma4RT[0]);

    propagateAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR);
  };


  if (logger->isEligible(SgLogger::DBG, SgLogger::IONO))
  {
    str.sprintf("GrDelay: effFreq (MHz): S= %14.8f X= %14.8f  coeff4GR= %14.12f xDif= %14.12f (ps) "
      "Sigma S= %14.12f X= %14.12f", 
      o[0]->grDelay().getEffFreq(), o[1]->grDelay().getEffFreq(),
      coeff4GR, xDif*1.0e12, sigma4GR[0]*1.0e12, sigma4GR[1]*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::IONO, className() +
      "::calcIonoCorrections(): " + str);
    
    str.sprintf("PhDelay: effFreq (MHz): S= %14.8f X= %14.8f  coeff4GR= %14.12f xDif= %14.12f (ps) "
      "Sigma S= %14.12f X= %14.12f", 
      o[0]->phDelay().getEffFreq(), o[1]->phDelay().getEffFreq(),
      coeff4PH, pDif*1.0e12, sigma4PH[0]*1.0e12, sigma4PH[1]*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::IONO, className() +
      "::calcIonoCorrections(): " + str);

    str.sprintf("PhDRate: effFreq (MHz): S= %14.8f X= %14.8f  coeff4GR= %14.12f xDif= %14.12f (E+12) "
      "Sigma S= %14.12f X= %14.12f", 
      o[0]->phDRate().getEffFreq(), o[1]->phDRate().getEffFreq(),
      coeff4RT, rDif*1.0e12, sigma4RT[0]*1.0e12, sigma4RT[1]*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::IONO, className() +
      "::calcIonoCorrections(): " + str);
  };
  //
  return;
};



//
void SgVlbiObservation::resetAllEditings()
{
  delAttr(Attr_NOT_VALID);
  // delAttr(Attr_PROCESSED);
  delAttr(Attr_FORCE_2_PROCESS);
  delAttr(Attr_CLOCK_BREAK_AT_1);
  delAttr(Attr_CLOCK_BREAK_AT_2);
  delAttr(Attr_PASSED_CL_FN1);
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    it.value()->resetAllEditings();
};



//
bool SgVlbiObservation::saveIntermediateResults(QDataStream& s) const
{
  s << key_ << getAttributes() << observableByKey_.size();
  for (QMap<QString, SgVlbiObservable*>::const_iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
    if (s.status() == QDataStream::Ok)
      it.value()->saveIntermediateResults(s);
    else
     return false;

  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiObservation::loadIntermediateResults(QDataStream& s)
{
  QString                       key;
  int                           num;
  unsigned int                  attributes;
  s >> key >> attributes >> num;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (key_ != key)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: bandKey mismatch: got [" + key +
      "], expected [" + key_ + "]");
    return false;
  };
  setAttributes(attributes);
  if (num != observableByKey_.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: observableByKey_ size mismatch: got [" + 
      QString("").setNum(num) + "], expected [" + QString("").setNum(observableByKey_.size()) + "]");
    return false;
  };
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
  {
    if (s.status() == QDataStream::Ok)
      it.value()->loadIntermediateResults(s);
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error reading data for the " + it.value()->getBandKey() + 
        "-band of " + key_);
      return false;
    };
  };
  return s.status()==QDataStream::Ok;
};



//
int SgVlbiObservation::calculateIonoBits()
{
  int                           bits;
  bits = 0;

  if (!primeObs_)
  {
    logger->write(SgLogger::ERR, SgLogger::IONO, className() +
      ": calculateIonoBits(): the prime band obs is NULL");
    return bits;
  };
  if (observableByKey_.size()!=2)
  {
    if (2 < observableByKey_.size())
      logger->write(SgLogger::INF, SgLogger::IONO, className() +
        ": calculateIonoBits(): cannot calculate, number of bands (" + 
        QString("").setNum(observableByKey_.size()) + ") is not equal 2; " +
        baseline_->getKey() + " on " + src_->getKey());
    return bits;
  };
  SgVlbiObservable             *secObs=NULL;
  for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
    it!=observableByKey_.end(); ++it)
  {
    if (it.value()->getBandKey() != primeObs_->getBandKey())
      secObs = it.value();
  };  
  //
  //  !  Apply to corrections using group delay:
  //  !      0001           1      Matching obs has quality code of
  //  !                              8 or 9, but was unweighted by SOLVE
  //  !      0002           2      No matching group data for GION
  //  !      0004           3      Matching obs has quality code of
  //  !                              1 to 7 (not used after 2001.01.17)
  //  !      0010           4      GION correction available
  //  !      0020           5      Downweight flag for GION (replaces IUNW=8)
  //  !                              (can be reset by other programs only if
  //  !                               bits 2 and 6 are not turned on)
  //  !                              ( not used after 2001.01.17 )
  //  !      0040           6      Matching obs has quality code of
  //  !                              0 ("no fringes")
  //  !
  //  !  Apply to corrections using phase delay:
  //  !      0100           7      Matching obs has quality code of
  //  !                              8 or 9, but was unweighted by SOLVE
  //  !      0200           8      No matching phase data for PHION
  //  !      0400           9      Matching obs has quality code of
  //  !                              1 to 7
  //  !      1000          10      PHION correction available
  //  !      2000          11      Downweight flag for PHION (replaces IUNW=8)
  //  !                              (can be reset by other programs only if
  //  !                               bits 8 and 12 are not turned on)
  //  !      4000          12      Matching obs has quality code of
  //  !                              0 ("no fringes")

  // currently, only group delays are reported:
  if (0.0 < primeObs_->grDelay().getIonoSigma())
    bits |= 1<<3;
  
  if (!secObs)
    bits |= 1<<1;
  else
  {
    int                         qCode4MatchedObs=secObs->getQualityFactor();
    if (7<qCode4MatchedObs &&
        (isAttr(Attr_NOT_VALID) || 
         primeObs_->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
//       activeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
      )
      bits |= 1<<0;
    else if (qCode4MatchedObs <= 0)
      bits |= 1<<5;
  };
  return bits;
};



//
void SgVlbiObservation::digestPimaAutoSupFlags()
{
  //
  //    libs/cutil/meta_supr_inq.f
  //
  SgVlbiObservable             *secObs=NULL;
  if (observableByKey_.size())
    for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
      it!=observableByKey_.end(); ++it)
      if (it.key() != primeObs_->getBandKey())
        secObs = it.value();
  
  //PAS_INIT__SPS = 1<< 27,  //  The bit field has been initialized
  if (pimaAutoSup_ & PAS_INIT__SPS)
  {
    // PAS_BQCX__SPS = 1<<  1,  //  Bad quality code for X-band
    // PAS_BQCS__SPS = 1<<  2,  //  Bad quality code for S-band
    // PAS_NOFX__SPS = 1<<  3,  //  No fringes for X-band
    // PAS_NOFS__SPS = 1<<  4,  //  No fringes for S-band
    if (pimaAutoSup_ & (PAS_BQCX__SPS | PAS_NOFX__SPS))
      primeObs_->propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    if (secObs && (pimaAutoSup_ & (PAS_BQCS__SPS | PAS_NOFS__SPS)))
      secObs->propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    //
    // we do not use this:
    // PAS_CUEL__SPS = 1<<  5,  //  Observation made below elevation cut off limit
    if (pimaAutoSup_ & PAS_CUEL__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the CUEL__SPS (observation made below elevation cut off limit) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //
    //
    // PAS_DSBS__SPS = 1<<  6,  //  Observation at deselected baseline
    if (pimaAutoSup_ & PAS_DSBS__SPS)
    {
      baseline_->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
    };
    // PAS_DSSO__SPS = 1<<  7,  //  Observation of deselected source
    if (pimaAutoSup_ & PAS_DSSO__SPS)
    {
      src_->addAttr(SgVlbiSourceInfo::Attr_NOT_VALID);
    };
    //
    //
    //
    // PAS_BWVR__SPS = 1<<  8,  //  Bad WVR mask
    if (pimaAutoSup_ & PAS_BWVR__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the BWVR__SPS (bad WVR mask) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    // PAS_BPRN__SPS = 1<<  9,  //  No parangle correction available
    if (pimaAutoSup_ & PAS_BPRN__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the BPRN__SPS (No parangle correction available) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //
    //
    //  PAS_GION__SPS = 1<< 10,  //  GION calibration is not available *compatibility*
    //  PAS_GIO1__SPS = 1<< 11,  //  GION calibration is bad           *compatibility*
    //  PAS_GIO2__SPS = 1<< 12,  //  GION calibration is bad           *compatibility*
    //  PAS_GIO3__SPS = 1<< 13,  //  GION calibration is bad           *compatibility*
    //  PAS_GIO4__SPS = 1<< 14,  //  GION calibration is bad           *compatibility*
    if (pimaAutoSup_ & (PAS_GION__SPS | PAS_GIO1__SPS | PAS_GIO2__SPS | PAS_GIO3__SPS | PAS_GIO4__SPS))
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): one of the GIO?__SPS (GION calibration is bad) is detected, the "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_PION__SPS = 1<< 15,  //  PION calibration is not available *compatibility*
    //  PAS_PIO1__SPS = 1<< 16,  //  PION calibration is bad           *compatibility*
    //  PAS_PIO2__SPS = 1<< 17,  //  PION calibration is bad           *compatibility*
    if (pimaAutoSup_ & (PAS_PION__SPS | PAS_PIO1__SPS | PAS_PIO2__SPS))
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the PION__SPS or PIO1__SPS (PION calibration is bad) is detected, the "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //
    //
    //  PAS_EXTS__SPS = 1<< 18,  //  Suppress by the flagging in the external file
    if (pimaAutoSup_ & PAS_EXTS__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the EXTS__SPS (Suppress by the flagging in the external file) is "
        "detected, the observation is deselected; prime obs: " + primeObs_->strId());
      propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    };
    //
    //  PAS_LSNR__SPS = 1<< 19,  //  Low SNR
    if (pimaAutoSup_ & PAS_LSNR__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the LSNR__SPS (Low SNR) is "
        "detected, the observation is deselected; prime obs: " + primeObs_->strId());
      propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    };
    //
    //  PAS_FURE__SPS = 1<< 20,  //  Recoverable for FUSED data type
    if (pimaAutoSup_ & PAS_FURE__SPS)
    {
      /*
       logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the FURE__SPS (Recoverable for FUSED data type) "
        "flag is not supported, prime obs: " + primeObs_->strId());
      */
    };
    //  PAS_XAMB__SPS = 1<< 21,  //  X-band phase ambiguity not resolved
    if (pimaAutoSup_ & PAS_XAMB__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the XAMB__SPS (X-band phase ambiguity not resolved) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_SAMB__SPS = 1<< 22,  //  S-band phase ambiguity not resolved
    if (pimaAutoSup_ & PAS_SAMB__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the SAMB__SPS (S-band phase ambiguity not resolved) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_IUNW__SPS = 1<< 23,  //  IUNW code is not zero
    if (pimaAutoSup_ & PAS_IUNW__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the IUNW__SPS (IUNW code is not zero) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_WPAS__SPS = 1<< 24,  //  Wrong phase delay ambiguity spacings
    if (pimaAutoSup_ & PAS_WPAS__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the PAS_WPAS__SPS (Wrong phase delay ambiguity spacings) "
        "flag is detected; phDelay(s) are deselected, prime obs: " + primeObs_->strId());
      primeObs_->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      if (secObs)
        secObs->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    };
    //  PAS_IOUS__SPS = 1<< 25,  //  Ionospheric calibration is used if available
    if (pimaAutoSup_ & PAS_IOUS__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the IOUS__SPS (Ionospheric calibration is used if available) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_DECM__SPS = 1<< 26,  //  Decimation is set on
    if (pimaAutoSup_ & PAS_DECM__SPS)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the DECM__SPS (Decimation is set on) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_SET1__SPS = 1<< 28,  //  Circumstnaces bits are set up
    //  PAS_SET2__SPS = 1<< 29,  //  Usage status bits are set up
    if (pimaAutoSup_ & (PAS_SET1__SPS | PAS_SET2__SPS))
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): the SET?__SPS ((Circumstnaces|Usage status) bits are set up) "
        "flag is not supported, prime obs: " + primeObs_->strId());
    };
    //  PAS_GOOD__SPS = 1<< 30,  //  Observation is marked as good
    if (! (pimaAutoSup_ & PAS_GOOD__SPS) )
    {
/*
      if (primeObs_->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::digestPimaAutoSupFlags(): recovered previously deselected observation due to the "
          "GOOD__SPS (Observation is marked as good) flag, prime obs: " + primeObs_->strId());
      eradicateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::digestPimaAutoSupFlags(): did not detected "
        "GOOD__SPS (Observation is marked as good) flag, prime obs: " + primeObs_->strId());
*/
    };
    //  PAS_CBAD__SPS = 1<< 31,  //  Observation is marked as conditionally bad, but may become recoverable
    if (pimaAutoSup_ & PAS_CBAD__SPS)
    {
      primeObs_->grDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      /*
      if (primeObs_->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::digestPimaAutoSupFlags(): detected CBAD__SPS (Observation is marked as conditionally bad, "
          "but may become recoverable) flag, prime obs: " + primeObs_->strId());
      */
      //propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::digestPimaAutoSupFlags(): the pimaAutoSup flag was not initialized, prime obs: " + 
      primeObs_->strId() + " pimaAutoSup_ = " + QString("").setNum(pimaAutoSup_));
};



//
void SgVlbiObservation::digestPimaUserSupFlags()
{
  // pimaUserSup_;
  SgVlbiObservable             *secObs=NULL;
  if (observableByKey_.size())
    for (QMap<QString, SgVlbiObservable*>::iterator it=observableByKey_.begin(); 
      it!=observableByKey_.end(); ++it)
      if (it.key() != primeObs_->getBandKey())
        secObs = it.value();
/*
  if (0 <= primeObs_->getQualityFactor())
    return; // no fringes, the user flags are useless
  if (secObs && 0 <= secObs->getQualityFactor())
    return; // no fringes, the user flags are useless
*/

  // PST_PHSRAT__DTP = 1 <<  2, // PHSRAT__DTC='Phase delay & rate  ' )
  if (pimaUserSup_ & PST_PHSRAT__DTP)
  {
    primeObs_->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    if (secObs)
      secObs->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  };
  // PST_SNBRAT__DTP = 1 <<  3, // SNBRAT__DTC='N.Band delay & rate ' )
  if (pimaUserSup_ & PST_SNBRAT__DTP)
    primeObs_->sbDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);

  // PST_GRPONL__DTP = 1 <<  4, // GRPONL__DTC='Group delay only    ' )
  if (pimaUserSup_ & PST_GRPONL__DTP)
    primeObs_->grDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);

  // PST_PHSONL__DTP = 1 <<  5, // PHSONL__DTC='Phase delay only    ' )
  if (pimaUserSup_ & PST_PHSONL__DTP)
  {
    primeObs_->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    if (secObs)
      secObs->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  };
  // PST_SNBONL__DTP = 1 <<  6, // SNBONL__DTC='N.Band delay only   ' )
  if (pimaUserSup_ & PST_SNBONL__DTP)
  {
    primeObs_->sbDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    if (secObs)
      secObs->sbDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  };
  // PST_RATONL__DTP = 1 <<  7, // RATONL__DTC='Rate only           ' )
  if (pimaUserSup_ & PST_RATONL__DTP)
    primeObs_->phDRate().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  //
  // PST_P_PXS__DTP  = 1 << 15, // P_PXS__DTC='P-Pxs combination   ' )
  if (pimaUserSup_ & PST_P_PXS__DTP)
  {
    primeObs_->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
    if (secObs)
      secObs->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  };
  //  PST_GX__DTP     = 1 << 16, // GX__DTC='Group delay X-band  ' )
  if (pimaUserSup_ & PST_GX__DTP)
    primeObs_->grDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  //  PST_GS__DTP     = 1 << 17, // GS__DTC='Group delay S-band  ' )
  if (secObs && (pimaUserSup_ & PST_GS__DTP))
    secObs->grDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  //  PST_PX__DTP     = 1 << 18, // PX__DTC='Phase delay X-band  ' )
  if (pimaUserSup_ & PST_PX__DTP)
    primeObs_->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  //  PST_PS__DTP     = 1 << 19, // PS__DTC='Phase delay S-band  ' )
  if (secObs && (pimaUserSup_ & PST_PS__DTP))
    secObs->phDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  // PST_SNG_X__DTP  = 1 << 20, // SNG_X__DTC='Single band X-band  ' )
  if (pimaUserSup_ & PST_SNG_X__DTP)
    primeObs_->sbDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  // PST_SNG_S__DTP  = 1 << 21, // SNG_S__DTC='Single band S-band  ' )
  if (secObs && (pimaUserSup_ & PST_SNG_X__DTP))
    secObs->sbDelay().addAttr(SgVlbiMeasurement::Attr_NOT_VALID);


//UNRC__SPS  
//PHSRAT__DTP  
//SNBRAT__DTP  
//PHSONL__DTP  
//SNBONL__DTP  
//RATONL__DTP  
//P_PXS__DTP  
//PS__DTP  
//SNG_X__DTP

  //
  // undigested:
  //
  // PST_GRPRAT__DTP = 1 <<  1, // GRPRAT__DTC='Group delay & rate  ' )
  if (pimaUserSup_ & PST_GRPRAT__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the GRPRAT__DTP (Group delay & rate) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_G_GXS__DTP  = 1 <<  8, // G_GXS__DTC='G-Gxs combination   ' )
  if (pimaUserSup_ & PST_G_GXS__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the G_GXS__DTP (G-Gxs combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PX_GXS__DTP = 1 <<  9, // PX_GXS__DTC='Px-Gxs combination  ' )
  if (pimaUserSup_ & PST_PX_GXS__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PX_GXS__DTP (Px-Gxs combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PS_GXS__DTP = 1 << 10, // PS_GXS__DTC='Ps-Gxs combination  ' )
  if (pimaUserSup_ & PST_PS_GXS__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PS_GXS__DTP (Ps-Gxs combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PX_GX__DTP  = 1 << 11, // PX_GX__DTC='Px-Gx combination   ' )
  if (pimaUserSup_ & PST_PX_GX__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PX_GX__DTP (Px-Gx combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PX_GS__DTP  = 1 << 12, // PX_GS__DTC='Px-Gs combination   ' )
  if (pimaUserSup_ & PST_PX_GS__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PX_GS__DTP (Px-Gs combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PS_GX__DTP  = 1 << 13, // PS_GX__DTC='Ps-Gx combination   ' )
  if (pimaUserSup_ & PST_PS_GX__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PS_GX__DTP (Ps-Gx combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
  // PST_PS_GS__DTP  = 1 << 14, // PS_GS__DTC='Ps-Gs combination   ' )
  if (pimaUserSup_ & PST_PS_GS__DTP)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::digestPimaUserSupFlags(): the PS_GS__DTP (Ps-Gs combination) "
      "flag of USER_SUP is not supported, prime obs: " + primeObs_->strId());
};



//
void SgVlbiObservation::digestPimaUserRecFlags()
{
  // pimaUserRec_;
  
};



//
//
QString SgVlbiObservation::strId() const
{
  QString                       srcN("?"), blnN("?:?");
  SgVlbiSourceInfo             *si=src_;
  SgVlbiBaselineInfo           *bi=baseline_;
  if (!si)
    si = session_->lookupSourceByIdx(getSourceIdx());
  if (!bi)
    bi = session_->lookupBaselineByIdx(getBaselineIdx());
  if (si)
    srcN = si->getKey();
  if (bi)
    blnN = bi->getKey();
  return
    this->toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " of " + srcN + " at " + blnN + " : " + getScanName();
};





/*=====================================================================================================*/




/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//


/*=====================================================================================================*/
//
// aux functions:
//
/*=====================================================================================================*/


/*=====================================================================================================*/
//
// statics:
//
SgVector SgVlbiObservation::storage4O_C(1);
SgVector SgVlbiObservation::storage4Sigmas(1);
/*=====================================================================================================*/




/*=====================================================================================================*/
