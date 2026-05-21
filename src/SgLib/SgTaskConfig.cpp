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


#include <QtCore/QDataStream>


#include <SgLogger.h>
#include <SgTaskConfig.h>
#include <SgVlbiNetworkId.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTaskConfig::className()
{
  return "SgTaskConfig";
};



//
QString SgTaskConfig::evaluatePath2(const QString& dir2, const QString& path2)
{
  QString                       path2return("");
  if (path2.size() < 1)
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      ": evaluatePath2(): the file name is empty");
  else
    path2return = (path2.at(0)=='/')? path2 : dir2 + "/" + path2;
  return path2return;
};



//
//
// CONSTRUCTORS:
//
// An empty constructor:
SgTaskConfig::SgTaskConfig() : 
  name_(""), 
  extWeightsFileName_("glo_baseline.wgt"),
  extAPrioriSitesPositionsFileName_("glo.sit"),
  extAPrioriSitesVelocitiesFileName_("glo.vel"),
  extAPrioriSourcesPositionsFileName_("glo.src"),
  extAPrioriSourceSsmFileName_("glo.ssm"),
  extAPrioriErpFileName_("last.erp"),
  extAPrioriAxisOffsetsFileName_("glo.axis"),
  extAPrioriHiFyErpFileName_("jmg96.hf"),
  extAPrioriMeanGradientsFileName_("gsfc_dao_9095.mgr"),
  eccentricitiesFileName_("ECCDAT.ecc"),
  apByNetId_(),
  lastModifiedNetId_(""),
  t2Bgn_(tZero),
  t2End_(tInf)
{
  // eligible observations:
  qualityCodeThreshold_ = 5;
//  useQualityCodeG_ = false;
  useQualityCodeG_ = true;
  useQualityCodeH_ = true;
  goodQualityCodeAtStartup_ = 8;
  useGoodQualityCodeAtStartup_ = true;
  // observables:
  dataType_ = VDT_DELAY;
  useDelayType_ = VD_SB_DELAY;
  useRateType_ = VR_NONE;
  
  activeBandIdx_ = 0;
  // Compatibility:
  isSolveCompatible_ = true;
  useDynamicClockBreaks_ = false;
  useSolveObsSuppresionFlags_ = true;
  // Reweigting options:
  doWeightCorrection_ = false;
  useExternalWeights_ = false;
  wcMode_ = WCM_BASELINE;
  minAuxSigma4Delay_ = 0.0;
  minAuxSigma4Rate_  = 0.0;
  initAuxSigma4Delay_ = 5.0e-12;    // 5ps
  initAuxSigma4Rate_  = 100.0e-15;  // 100 fs/s

  // Estimator:
  epmPwlMode_ = EPM_BSPLINE_LINEA;
  doDownWeight_ = false;

  // Outliers processing:
  opMode_ = OPM_BASELINE;
  opAction_ = OPA_ELIMINATE;
  opThreshold_ = 3.0;
  opIterationsLimit_ = 40;
  opHave2SuppressWeightCorrection_ = true; // male?
  opIsSolveCompatible_ = true;
  opHave2NormalizeResiduals_ = true;

  // Options for preprocessing:
  doIonoCorrection4SBD_ = true;
  doClockBreakDetectionMode1_ = false;
  doOutliersEliminationMode1_ = false;

  // inclusion a priori data from external files:
  useExtAPrioriSitesPositions_        = false;
  useExtAPrioriSitesVelocities_       = false;
  useExtAPrioriSourcesPositions_      = false;
  useExtAPrioriSourceSsm_             = false;
  useExtAPrioriErp_                   = false;
  useExtAPrioriAxisOffsets_           = false;
  useExtAPrioriHiFyErp_               = false;
  useExtAPrioriMeanGradients_         = false;

  // GUI behavior:
  isActiveBandFollowsTab_ = true;
  isObservableFollowsPlot_ = true;
  
  // contributions:
  // already included:
  contribsAreFromDatabase_ = false;
  have2ApplyPxContrib_ = true;
  have2ApplyPyContrib_ = true;
  have2ApplyEarthTideContrib_ = true;
  have2ApplyOceanTideContrib_ = true;
  have2ApplyPoleTideContrib_ = true;
  have2ApplyPoleTideContrib_ = false;
  have2ApplyTiltRemvrContrib_ = false;
  have2ApplyAxisOffsetContrib_ = true;
  // not included:
  have2ApplyOceanTideContrib_ = true;
  have2ApplyNutationHFContrib_ = true;
  have2ApplyPxyOceanTideHFContrib_ = true;
  have2ApplyUt1OceanTideHFContrib_ = true;
  have2ApplyFeedCorrContrib_ = false;
  have2ApplyUt1LibrationContrib_ = true;
  have2ApplyPxyLibrationContrib_ = true;
  have2ApplyOceanPoleTideContrib_ = true;
  have2ApplyGpsIonoContrib_ = false;
  have2ApplySourceSsm_ = false;
  have2ApplyOldOceanTideContrib_ = false;
  have2ApplyOldPoleTideContrib_ = false;
  //
  have2ApplyUnPhaseCalContrib_  = false;
  isIonosphereFreeUncorrelated_ = false;
  useOldMode4IonosphereSigma_   = false;

  
//  flybyTropZenithMap_ = TZM_NMF;
  flybyCableCalSource_= CCS_DEFAULT;
  refractionModel_    = RM_CONTRIB_INTERNAL;

  isNoviceUser_ = true;
  have2outputCovarMatrix_ = false;
  isTesting_ = false;

  // two automatic processing setups:
  AutomaticProcessing           ap;
  ap.doSessionSetup_          = true;
  ap.doIonoCorrection4SBD_    = true;
  ap.doAmbigResolution_       = false;
  ap.doClockBreaksDetection_  = false;
  ap.doIonoCorrection4All_    = false;
  ap.doOutliers_              = false;
  ap.doWeights_               = false;
  ap.doReportNotUsedData_     = false;
  ap.finalSolution_           = AutomaticProcessing::FS_UT1;
//  apByNetId_[defaultNetIdName]= ap;
  apByNetId_["DEFAULT"]= ap;
  ap.doSessionSetup_          = true;
  ap.doIonoCorrection4SBD_    = true;
  ap.doAmbigResolution_       = true;
  ap.doClockBreaksDetection_  = false;
  ap.doIonoCorrection4All_    = false;
  ap.doOutliers_              = false;
  ap.doWeights_               = false;
  ap.doReportNotUsedData_     = false;
  ap.finalSolution_           = AutomaticProcessing::FS_UT1;
  apByNetId_["INT"] = ap;
//  lastModifiedNetId_ = defaultNetIdName;
  lastModifiedNetId_ = "DEFAULT";
};



//
SgTaskConfig& SgTaskConfig::operator=(const SgTaskConfig& cfg)
{
  // id:
  setName(cfg.getName());
  
  // eligible observations:
  setQualityCodeThreshold(cfg.getQualityCodeThreshold());
  setUseQualityCodeG(cfg.getUseQualityCodeG());
  setUseQualityCodeH(cfg.getUseQualityCodeH());
  setGoodQualityCodeAtStartup(cfg.getGoodQualityCodeAtStartup());
  setUseGoodQualityCodeAtStartup(cfg.getUseGoodQualityCodeAtStartup());
  
  // type of observables:
  // observables:
  setUseDelayType(cfg.getUseDelayType());
  setUseRateType (cfg.getUseRateType());
  setActiveBandIdx(cfg.getActiveBandIdx());
  // compatibility:
  setIsSolveCompatible(cfg.getIsSolveCompatible());
  setUseDynamicClockBreaks(cfg.getUseDynamicClockBreaks());
  setUseSolveObsSuppresionFlags(cfg.getUseSolveObsSuppresionFlags());
  // reweighting:
  setDoWeightCorrection(cfg.getDoWeightCorrection());
  setUseExternalWeights(cfg.getUseExternalWeights());
  setWcMode(cfg.getWcMode());
  setExtWeightsFileName(cfg.getExtWeightsFileName());
  
  setMinAuxSigma4Delay(cfg.getMinAuxSigma4Delay());
  setMinAuxSigma4Rate (cfg.getMinAuxSigma4Rate());
  setInitAuxSigma4Delay(cfg.getInitAuxSigma4Delay());
  setInitAuxSigma4Rate (cfg.getInitAuxSigma4Rate());

  // Estimator:
  setPwlMode(cfg.getPwlMode());
  setDoDownWeight(cfg.getDoDownWeight());
  // outliers processing:
  setOpMode(cfg.getOpMode());
  setOpAction(cfg.getOpAction());
  setOpThreshold(cfg.getOpThreshold());
  setOpIterationsLimit(cfg.getOpIterationsLimit());
  setOpHave2SuppressWeightCorrection(cfg.getOpHave2SuppressWeightCorrection());
  setOpIsSolveCompatible(cfg.getOpIsSolveCompatible());
  setOpHave2NormalizeResiduals(cfg.getOpHave2NormalizeResiduals());

  // post-read-actions::
  setDoIonoCorrection4SBD(cfg.getDoIonoCorrection4SBD());
  setDoClockBreakDetectionMode1(cfg.getDoClockBreakDetectionMode1());
  setDoOutliersEliminationMode1(cfg.getDoOutliersEliminationMode1());
  // gui's:
  setIsActiveBandFollowsTab(cfg.getIsActiveBandFollowsTab());
  setIsObservableFollowsPlot(cfg.getIsObservableFollowsPlot());
  setUseExtAPrioriSitesPositions(cfg.getUseExtAPrioriSitesPositions());
  setUseExtAPrioriSitesVelocities(cfg.getUseExtAPrioriSitesVelocities());
  setUseExtAPrioriSourcesPositions(cfg.getUseExtAPrioriSourcesPositions());
  setUseExtAPrioriSourceSsm(cfg.getUseExtAPrioriSourceSsm());
  setUseExtAPrioriErp(cfg.getUseExtAPrioriErp());
  setUseExtAPrioriAxisOffsets(cfg.getUseExtAPrioriAxisOffsets());
  setUseExtAPrioriHiFyErp(cfg.getUseExtAPrioriHiFyErp());
  setUseExtAPrioriMeanGradients(cfg.getUseExtAPrioriMeanGradients());
  setExtAPrioriSitesPositionsFileName(cfg.getExtAPrioriSitesPositionsFileName());
  setExtAPrioriSitesVelocitiesFileName(cfg.getExtAPrioriSitesVelocitiesFileName());
  setExtAPrioriSourcesPositionsFileName(cfg.getExtAPrioriSourcesPositionsFileName());
  setExtAPrioriSourceSsmFileName(cfg.getExtAPrioriSourceSsmFileName());
  setExtAPrioriErpFileName(cfg.getExtAPrioriErpFileName());
  setExtAPrioriAxisOffsetsFileName(cfg.getExtAPrioriAxisOffsetsFileName());
  setExtAPrioriHiFyErpFileName(cfg.getExtAPrioriHiFyErpFileName());
  setExtAPrioriMeanGradientsFileName(cfg.getExtAPrioriMeanGradientsFileName());
  setEccentricitiesFileName(cfg.getEccentricitiesFileName());
  // contributions:
  setHave2ApplyPxContrib(cfg.getHave2ApplyPxContrib());
  setHave2ApplyPyContrib(cfg.getHave2ApplyPyContrib());
  setHave2ApplyEarthTideContrib(cfg.getHave2ApplyEarthTideContrib());
  setHave2ApplyOceanTideContrib(cfg.getHave2ApplyOceanTideContrib());
  setHave2ApplyPoleTideContrib(cfg.getHave2ApplyPoleTideContrib());
  setHave2ApplyGpsIonoContrib(cfg.getHave2ApplyGpsIonoContrib());
  setHave2ApplySourceSsm(cfg.getHave2ApplySourceSsm());
  setHave2ApplyUt1OceanTideHFContrib(cfg.getHave2ApplyUt1OceanTideHFContrib());
  setHave2ApplyPxyOceanTideHFContrib(cfg.getHave2ApplyPxyOceanTideHFContrib());
  setHave2ApplyNutationHFContrib(cfg.getHave2ApplyNutationHFContrib());
  setHave2ApplyUt1LibrationContrib(cfg.getHave2ApplyUt1LibrationContrib());
  setHave2ApplyPxyLibrationContrib(cfg.getHave2ApplyPxyLibrationContrib());
  setHave2ApplyOceanPoleTideContrib(cfg.getHave2ApplyOceanPoleTideContrib());
  setHave2ApplyFeedCorrContrib(cfg.getHave2ApplyFeedCorrContrib());
  setHave2ApplyTiltRemvrContrib(cfg.getHave2ApplyTiltRemvrContrib());
  setHave2ApplyOldOceanTideContrib(cfg.getHave2ApplyOldOceanTideContrib());
  setHave2ApplyOldPoleTideContrib(cfg.getHave2ApplyOldPoleTideContrib());
  //
  setHave2ApplyAxisOffsetContrib(cfg.getHave2ApplyAxisOffsetContrib());
  setHave2ApplyUnPhaseCalContrib(cfg.getHave2ApplyUnPhaseCalContrib());
  setIsIonosphereFreeUncorrelated(cfg.getIsIonosphereFreeUncorrelated());
  setUseOldMode4IonosphereSigma(cfg.getUseOldMode4IonosphereSigma());
  
  setFlybyCableCalSource(cfg.getFlybyCableCalSource());
  setRefractionModel(cfg.getRefractionModel());
  //

  setIsNoviceUser(cfg.getIsNoviceUser());
  setHave2outputCovarMatrix(cfg.getHave2outputCovarMatrix());
  setIsTesting(cfg.getIsTesting());
  
  apByNetId_.clear();
  for (QMap<QString, AutomaticProcessing>::const_iterator it=cfg.apByNetId_.begin(); 
    it!=cfg.apByNetId_.end(); ++it)
    apByNetId_[it.key()] = it.value();
  lastModifiedNetId_ = cfg.getLastModifiedNetId();
  
  setT2Bgn(cfg.getT2Bgn());
  setT2End(cfg.getT2End());
  return *this;
};



//
bool SgTaskConfig::operator==(const SgTaskConfig& cfg) const
{
  bool                          is;
  is =  (name_==cfg.getName()) && 
        (qualityCodeThreshold_==cfg.getQualityCodeThreshold()) && 
        (useQualityCodeG_==cfg.getUseQualityCodeG()) && 
        (useQualityCodeH_==cfg.getUseQualityCodeH()) && 
        (goodQualityCodeAtStartup_==cfg.getGoodQualityCodeAtStartup()) && 
        (useGoodQualityCodeAtStartup_==cfg.getUseGoodQualityCodeAtStartup()) && 
//      (dataType_==cfg.getDataType()) && 
        (useDelayType_==cfg.getUseDelayType()) && 
        (useRateType_ ==cfg.getUseRateType()) && 
        (activeBandIdx_==cfg.getActiveBandIdx()) && 
        (isSolveCompatible_==cfg.getIsSolveCompatible()) && 
        (useDynamicClockBreaks_==cfg.getUseDynamicClockBreaks()) && 
        (useSolveObsSuppresionFlags_==cfg.getUseSolveObsSuppresionFlags()) && 
        (doWeightCorrection_==cfg.getDoWeightCorrection()) && 
        (useExternalWeights_==cfg.getUseExternalWeights()) && 
        (wcMode_==cfg.getWcMode()) && 
        (extWeightsFileName_==cfg.getExtWeightsFileName()) && 
        (minAuxSigma4Delay_==cfg.getMinAuxSigma4Delay()) && 
        (minAuxSigma4Rate_==cfg.getMinAuxSigma4Rate()) && 
        (initAuxSigma4Delay_==cfg.getInitAuxSigma4Delay()) && 
        (initAuxSigma4Rate_==cfg.getInitAuxSigma4Rate()) && 
        (epmPwlMode_==cfg.getPwlMode()) && 
        (doDownWeight_==cfg.getDoDownWeight()) && 
        (opMode_==cfg.getOpMode()) && 
        (opAction_==cfg.getOpAction()) && 
        (opThreshold_==cfg.getOpThreshold()) && 
        (opIterationsLimit_==cfg.getOpIterationsLimit()) && 
        (opHave2SuppressWeightCorrection_==cfg.getOpHave2SuppressWeightCorrection()) && 
        (opIsSolveCompatible_==cfg.getOpIsSolveCompatible()) && 
        (opHave2NormalizeResiduals_==cfg.getOpHave2NormalizeResiduals()) && 
        (doIonoCorrection4SBD_==cfg.getDoIonoCorrection4SBD()) && 
        (doClockBreakDetectionMode1_==cfg.getDoClockBreakDetectionMode1()) && 
        (doOutliersEliminationMode1_==cfg.getDoOutliersEliminationMode1()) && 
        (isActiveBandFollowsTab_==cfg.getIsActiveBandFollowsTab()) && 
        (isObservableFollowsPlot_==cfg.getIsObservableFollowsPlot()) && 
        (useExtAPrioriSitesPositions_==cfg.getUseExtAPrioriSitesPositions()) && 
        (useExtAPrioriSitesVelocities_==cfg.getUseExtAPrioriSitesVelocities()) && 
        (useExtAPrioriSourcesPositions_==cfg.getUseExtAPrioriSourcesPositions()) && 
        (useExtAPrioriSourceSsm_==cfg.getUseExtAPrioriSourceSsm()) && 
        (useExtAPrioriErp_==cfg.getUseExtAPrioriErp()) && 
        (useExtAPrioriAxisOffsets_==cfg.getUseExtAPrioriAxisOffsets()) && 
        (useExtAPrioriHiFyErp_==cfg.getUseExtAPrioriHiFyErp()) && 
        (useExtAPrioriMeanGradients_==cfg.getUseExtAPrioriMeanGradients()) && 
        (extAPrioriSitesPositionsFileName_==cfg.getExtAPrioriSitesPositionsFileName()) && 
        (extAPrioriSitesVelocitiesFileName_==cfg.getExtAPrioriSitesVelocitiesFileName()) && 
        (extAPrioriSourcesPositionsFileName_==cfg.getExtAPrioriSourcesPositionsFileName()) && 
        (extAPrioriSourceSsmFileName_==cfg.getExtAPrioriSourceSsmFileName()) && 
        (extAPrioriErpFileName_==cfg.getExtAPrioriErpFileName()) && 
        (extAPrioriAxisOffsetsFileName_==cfg.getExtAPrioriAxisOffsetsFileName()) && 
        (extAPrioriHiFyErpFileName_==cfg.getExtAPrioriHiFyErpFileName()) && 
        (extAPrioriMeanGradientsFileName_==cfg.getExtAPrioriMeanGradientsFileName()) && 
        (eccentricitiesFileName_==cfg.getEccentricitiesFileName()) && 
        (contribsAreFromDatabase_==cfg.getContribsAreFromDatabase()) && 
        (have2ApplyPxContrib_==cfg.getHave2ApplyPxContrib()) && 
        (have2ApplyPyContrib_==cfg.getHave2ApplyPyContrib()) && 
        (have2ApplyEarthTideContrib_==cfg.getHave2ApplyEarthTideContrib()) && 
        (have2ApplyOceanTideContrib_==cfg.getHave2ApplyOceanTideContrib()) && 
        (have2ApplyPoleTideContrib_==cfg.getHave2ApplyPoleTideContrib()) && 
        (have2ApplyGpsIonoContrib_==cfg.getHave2ApplyGpsIonoContrib()) && 
        (have2ApplySourceSsm_==cfg.getHave2ApplySourceSsm()) && 
        (have2ApplyUt1OceanTideHFContrib_==cfg.getHave2ApplyUt1OceanTideHFContrib()) && 
        (have2ApplyPxyOceanTideHFContrib_==cfg.getHave2ApplyPxyOceanTideHFContrib()) && 
        (have2ApplyNutationHFContrib_==cfg.getHave2ApplyNutationHFContrib()) && 
        (have2ApplyUt1LibrationContrib_==cfg.getHave2ApplyUt1LibrationContrib()) && 
        (have2ApplyPxyLibrationContrib_==cfg.getHave2ApplyPxyLibrationContrib()) && 
        (have2ApplyOceanPoleTideContrib_==cfg.getHave2ApplyOceanPoleTideContrib()) && 
        (have2ApplyFeedCorrContrib_==cfg.getHave2ApplyFeedCorrContrib()) && 
        (have2ApplyTiltRemvrContrib_==cfg.getHave2ApplyTiltRemvrContrib()) && 
        (have2ApplyOldOceanTideContrib_==cfg.getHave2ApplyOldOceanTideContrib()) && 
        (have2ApplyOldPoleTideContrib_==cfg.getHave2ApplyOldPoleTideContrib()) && 
        (have2ApplyAxisOffsetContrib_==cfg.getHave2ApplyAxisOffsetContrib()) && 
        (have2ApplyUnPhaseCalContrib_==cfg.getHave2ApplyUnPhaseCalContrib()) && 
        (isIonosphereFreeUncorrelated_==cfg.getIsIonosphereFreeUncorrelated()) && 
        (useOldMode4IonosphereSigma_==cfg.getUseOldMode4IonosphereSigma()) && 
        (flybyCableCalSource_==cfg.getFlybyCableCalSource()) &&
        (refractionModel_==cfg.getRefractionModel()) &&
        (isNoviceUser_==cfg.getIsNoviceUser()) &&
        (have2outputCovarMatrix_==cfg.getHave2outputCovarMatrix()) &&
        (isTesting_==cfg.getIsTesting()) &&
        (lastModifiedNetId_==cfg.getLastModifiedNetId()) &
        (t2Bgn_ == cfg.getT2Bgn()) &
        (t2End_ == cfg.getT2End())
        ;

  //QMap<QString, AutomaticProcessing> apByNetId_;
  return is;
};



//
bool SgTaskConfig::saveIntermediateResults(QDataStream& s) const
{
  // ! we do not save name_
  s << qualityCodeThreshold_ << useQualityCodeG_ << useQualityCodeH_ << goodQualityCodeAtStartup_ 
    << useGoodQualityCodeAtStartup_ << (unsigned int)useDelayType_ << (unsigned int)useRateType_
    << activeBandIdx_ << isSolveCompatible_ << useDynamicClockBreaks_ << useSolveObsSuppresionFlags_ 
    << doWeightCorrection_
    << useExternalWeights_ << (unsigned int)wcMode_ << extWeightsFileName_ 
    << minAuxSigma4Delay_ << minAuxSigma4Rate_ << initAuxSigma4Delay_ << initAuxSigma4Rate_
    << (unsigned int)epmPwlMode_ << doDownWeight_
    << (unsigned int)opMode_ << (unsigned int)opAction_ << opThreshold_ << opIterationsLimit_ 
    << opHave2SuppressWeightCorrection_ << opIsSolveCompatible_ << opHave2NormalizeResiduals_ 
    << doIonoCorrection4SBD_ << doClockBreakDetectionMode1_ 
    << doOutliersEliminationMode1_ << isActiveBandFollowsTab_ << isObservableFollowsPlot_ 
    << useExtAPrioriSitesPositions_ << useExtAPrioriSitesVelocities_ 
    << useExtAPrioriSourcesPositions_ << useExtAPrioriSourceSsm_
    << useExtAPrioriErp_ << useExtAPrioriAxisOffsets_ 
    << useExtAPrioriHiFyErp_ << useExtAPrioriMeanGradients_ << extAPrioriSitesPositionsFileName_ 
    << extAPrioriSitesVelocitiesFileName_ << extAPrioriSourcesPositionsFileName_ 
    << extAPrioriSourceSsmFileName_
    << extAPrioriErpFileName_ << extAPrioriAxisOffsetsFileName_ << extAPrioriHiFyErpFileName_ 
    << extAPrioriMeanGradientsFileName_ << eccentricitiesFileName_ 

    << have2ApplyPxContrib_
    << have2ApplyPyContrib_
    << have2ApplyEarthTideContrib_
    << have2ApplyOceanTideContrib_
    << have2ApplyPoleTideContrib_
    << have2ApplyUt1OceanTideHFContrib_
    << have2ApplyPxyOceanTideHFContrib_
    << have2ApplyNutationHFContrib_
    << have2ApplyUt1LibrationContrib_
    << have2ApplyPxyLibrationContrib_
    << have2ApplyOceanPoleTideContrib_
    << have2ApplyGpsIonoContrib_
    << have2ApplySourceSsm_
    << have2ApplyFeedCorrContrib_
    << have2ApplyTiltRemvrContrib_
    << have2ApplyOldOceanTideContrib_
    << have2ApplyOldPoleTideContrib_
    << have2ApplyAxisOffsetContrib_
    << have2ApplyUnPhaseCalContrib_
    << isIonosphereFreeUncorrelated_
    << useOldMode4IonosphereSigma_
    << (unsigned int)flybyCableCalSource_
    << (unsigned int)refractionModel_
    << isNoviceUser_
    << have2outputCovarMatrix_
    << isTesting_
    << t2Bgn_.getDate() << t2Bgn_.getTime()
    << t2End_.getDate() << t2End_.getTime()
    ;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgTaskConfig::loadIntermediateResults(QDataStream& s)
{
  // ! we do not load name_
  int                           qualityCodeThreshold, goodQualityCodeAtStartup, activeBandIdx,
                                opIterationsLimit;
  bool                          useQualityCodeG, useQualityCodeH, useGoodQualityCodeAtStartup, 
                                isSolveCompatible, useDynamicClockBreaks,
                                useSolveObsSuppresionFlags, doWeightCorrection, useExternalWeights,
                                opHave2SuppressWeightCorrection, opIsSolveCompatible, 
                                opHave2NormalizeResiduals,
                                doIonoCorrection4SBD, doClockBreakDetectionMode1, 
                                doOutliersEliminationMode1, isActiveBandFollowsTab, 
                                isObservableFollowsPlot, useExtAPrioriSitesPositions, 
                                useExtAPrioriSitesVelocities, useExtAPrioriSourcesPositions,
                                useExtAPrioriSourceSsm, 
                                useExtAPrioriErp, useExtAPrioriAxisOffsets, useExtAPrioriHiFyErp,
                                useExtAPrioriMeanGradients, have2ApplyOceanTideContrib,
                                have2ApplyPoleTideContrib, have2ApplyEarthTideContrib, 
                                have2ApplyPxContrib, have2ApplyPyContrib, 
                                have2ApplyAxisOffsetContrib, have2ApplyNutationHFContrib,
                                have2ApplyPxyOceanTideHFContrib, have2ApplyUt1OceanTideHFContrib,
                                have2ApplyFeedCorrContrib, have2ApplyTiltRemvrContrib, 
                                have2ApplyUnPhaseCalContrib, 
                                isIonosphereFreeUncorrelated, useOldMode4IonosphereSigma,
                                have2ApplyUt1LibrationContrib,
                                have2ApplyPxyLibrationContrib, have2ApplyOceanPoleTideContrib,
                                have2ApplyGpsIonoContrib, have2ApplySourceSsm,
                                have2ApplyOldOceanTideContrib, have2ApplyOldPoleTideContrib,
                                isNoviceUser, have2outputCovarMatrix, isTesting, doDownWeight;
  double                        opThreshold, minAuxSigma4Delay, minAuxSigma4Rate, initAuxSigma4Delay,
                                initAuxSigma4Rate;
  unsigned int                  useDelayType, useRateType, wcMode, epmPwlMode, opMode, opAction,
                                /*flybyTropZenithMap, */flybyCableCalSource, refractionModel;
  QString                       extWeightsFileName, extAPrioriSitesPositionsFileName,
                                extAPrioriSitesVelocitiesFileName, extAPrioriSourcesPositionsFileName,
                                extAPrioriSourceSsmFileName,
                                extAPrioriErpFileName, extAPrioriAxisOffsetsFileName, 
                                extAPrioriHiFyErpFileName, extAPrioriMeanGradientsFileName,
                                eccentricitiesFileName;
  int                           t2BgnDate, t2EndDate;
  double                        t2BgnTime, t2EndTime;

  //
  s >> qualityCodeThreshold >> useQualityCodeG >> useQualityCodeH >> goodQualityCodeAtStartup
    >> useGoodQualityCodeAtStartup >> useDelayType >> useRateType >> activeBandIdx
    >> isSolveCompatible >> useDynamicClockBreaks >> useSolveObsSuppresionFlags >> doWeightCorrection
    >> useExternalWeights >> wcMode >> extWeightsFileName 
    >> minAuxSigma4Delay >> minAuxSigma4Rate >> initAuxSigma4Delay >> initAuxSigma4Rate
    >> epmPwlMode >> doDownWeight >> opMode
    >> opAction >> opThreshold >> opIterationsLimit >> opHave2SuppressWeightCorrection
    >> opIsSolveCompatible >> opHave2NormalizeResiduals >> doIonoCorrection4SBD 
    >> doClockBreakDetectionMode1 
    >> doOutliersEliminationMode1 >> isActiveBandFollowsTab >> isObservableFollowsPlot
    >> useExtAPrioriSitesPositions >> useExtAPrioriSitesVelocities
    >> useExtAPrioriSourcesPositions >> useExtAPrioriSourceSsm >> useExtAPrioriErp
    >> useExtAPrioriAxisOffsets
    >> useExtAPrioriHiFyErp >> useExtAPrioriMeanGradients >> extAPrioriSitesPositionsFileName
    >> extAPrioriSitesVelocitiesFileName >> extAPrioriSourcesPositionsFileName
    >> extAPrioriSourceSsmFileName
    >> extAPrioriErpFileName >> extAPrioriAxisOffsetsFileName >> extAPrioriHiFyErpFileName
    >> extAPrioriMeanGradientsFileName >> eccentricitiesFileName 
    >> have2ApplyPxContrib
    >> have2ApplyPyContrib
    >> have2ApplyEarthTideContrib
    >> have2ApplyOceanTideContrib
    >> have2ApplyPoleTideContrib
    >> have2ApplyUt1OceanTideHFContrib
    >> have2ApplyPxyOceanTideHFContrib
    >> have2ApplyNutationHFContrib
    >> have2ApplyUt1LibrationContrib
    >> have2ApplyPxyLibrationContrib
    >> have2ApplyOceanPoleTideContrib
    >> have2ApplyGpsIonoContrib
    >> have2ApplySourceSsm
    >> have2ApplyFeedCorrContrib
    >> have2ApplyTiltRemvrContrib
    >> have2ApplyOldOceanTideContrib
    >> have2ApplyOldPoleTideContrib
    >> have2ApplyAxisOffsetContrib
    >> have2ApplyUnPhaseCalContrib
    >> isIonosphereFreeUncorrelated
    >> useOldMode4IonosphereSigma
    >> flybyCableCalSource
    >> refractionModel
    >> isNoviceUser
    >> have2outputCovarMatrix
    >> isTesting
    >> t2BgnDate >> t2BgnTime
    >> t2EndDate >> t2EndTime
    ;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  setQualityCodeThreshold(qualityCodeThreshold);
  setUseQualityCodeG(useQualityCodeG);
  setUseQualityCodeH(useQualityCodeH);
  setGoodQualityCodeAtStartup(goodQualityCodeAtStartup);
  setUseGoodQualityCodeAtStartup(useGoodQualityCodeAtStartup);
  setActiveBandIdx(activeBandIdx);
  setIsSolveCompatible(isSolveCompatible);
  setUseDynamicClockBreaks(useDynamicClockBreaks);
  setUseSolveObsSuppresionFlags(useSolveObsSuppresionFlags);
  setDoWeightCorrection(doWeightCorrection);
  setUseExternalWeights(useExternalWeights);
  setExtWeightsFileName(extWeightsFileName);
  setMinAuxSigma4Delay(minAuxSigma4Delay);
  setMinAuxSigma4Rate (minAuxSigma4Rate);
  setInitAuxSigma4Delay(initAuxSigma4Delay);
  setInitAuxSigma4Rate (initAuxSigma4Rate);
  setOpThreshold(opThreshold);
  setOpIterationsLimit(opIterationsLimit);
  setOpHave2SuppressWeightCorrection(opHave2SuppressWeightCorrection);
  setOpIsSolveCompatible(opIsSolveCompatible);
  setOpHave2NormalizeResiduals(opHave2NormalizeResiduals);
  setDoIonoCorrection4SBD(doIonoCorrection4SBD);
  setDoClockBreakDetectionMode1(doClockBreakDetectionMode1);
  setDoOutliersEliminationMode1(doOutliersEliminationMode1);
  setIsActiveBandFollowsTab(isActiveBandFollowsTab);
  setIsObservableFollowsPlot(isObservableFollowsPlot);
  setUseExtAPrioriSitesPositions(useExtAPrioriSitesPositions);
  setUseExtAPrioriSitesVelocities(useExtAPrioriSitesVelocities);
  setUseExtAPrioriSourcesPositions(useExtAPrioriSourcesPositions);
  setUseExtAPrioriSourceSsm(useExtAPrioriSourceSsm);
  setUseExtAPrioriErp(useExtAPrioriErp);
  setUseExtAPrioriAxisOffsets(useExtAPrioriAxisOffsets);
  setUseExtAPrioriHiFyErp(useExtAPrioriHiFyErp);
  setUseExtAPrioriMeanGradients(useExtAPrioriMeanGradients);
  setExtAPrioriSitesPositionsFileName(extAPrioriSitesPositionsFileName);
  setExtAPrioriSitesVelocitiesFileName(extAPrioriSitesVelocitiesFileName);
  setExtAPrioriSourcesPositionsFileName(extAPrioriSourcesPositionsFileName);
  setExtAPrioriSourceSsmFileName(extAPrioriSourceSsmFileName);
  setExtAPrioriErpFileName(extAPrioriErpFileName);
  setExtAPrioriAxisOffsetsFileName(extAPrioriAxisOffsetsFileName);
  setExtAPrioriHiFyErpFileName(extAPrioriHiFyErpFileName);
  setExtAPrioriMeanGradientsFileName(extAPrioriMeanGradientsFileName);
  setEccentricitiesFileName(eccentricitiesFileName);

  setHave2ApplyPxContrib(have2ApplyPxContrib);
  setHave2ApplyPyContrib(have2ApplyPyContrib);
  setHave2ApplyEarthTideContrib(have2ApplyEarthTideContrib);
  setHave2ApplyOceanTideContrib(have2ApplyOceanTideContrib);
  setHave2ApplyPoleTideContrib(have2ApplyPoleTideContrib);
  setHave2ApplyUt1OceanTideHFContrib(have2ApplyUt1OceanTideHFContrib);
  setHave2ApplyPxyOceanTideHFContrib(have2ApplyPxyOceanTideHFContrib);
  setHave2ApplyNutationHFContrib(have2ApplyNutationHFContrib);
  setHave2ApplyUt1LibrationContrib(have2ApplyUt1LibrationContrib);
  setHave2ApplyPxyLibrationContrib(have2ApplyPxyLibrationContrib);
  setHave2ApplyOceanPoleTideContrib(have2ApplyOceanPoleTideContrib);
  setHave2ApplyGpsIonoContrib(have2ApplyGpsIonoContrib);
  setHave2ApplySourceSsm(have2ApplySourceSsm);
  setHave2ApplyFeedCorrContrib(have2ApplyFeedCorrContrib);
  setHave2ApplyTiltRemvrContrib(have2ApplyTiltRemvrContrib);
  setHave2ApplyOldOceanTideContrib(have2ApplyOldOceanTideContrib);
  setHave2ApplyOldPoleTideContrib(have2ApplyOldPoleTideContrib);
  setHave2ApplyAxisOffsetContrib(have2ApplyAxisOffsetContrib);
  setHave2ApplyUnPhaseCalContrib(have2ApplyUnPhaseCalContrib);
  setIsIonosphereFreeUncorrelated(isIonosphereFreeUncorrelated);
  setUseOldMode4IonosphereSigma (useOldMode4IonosphereSigma);
  setFlybyCableCalSource((CableCalSource)flybyCableCalSource);
  setRefractionModel((RefractionModel)refractionModel);
  setIsNoviceUser(isNoviceUser);
  setHave2outputCovarMatrix(have2outputCovarMatrix);
  setIsTesting(isTesting);
  setUseDelayType((VlbiDelayType)useDelayType);
  setUseRateType ((VlbiRateType)useRateType);
  setWcMode((WeightCorrectionMode)wcMode);
  setPwlMode((EstimatorPwlMode)epmPwlMode);
  setDoDownWeight(doDownWeight);
  setOpMode((OutliersProcessingMode)opMode);
  setOpAction((OutliersProcessingAction)opAction);
  t2Bgn_.setDate(t2BgnDate);
  t2Bgn_.setTime(t2BgnTime);
  t2End_.setDate(t2EndDate);
  t2End_.setTime(t2EndTime);
  //
  return s.status()==QDataStream::Ok;
};
/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//

const QString sCableCalSourceNames[4] = {"Default", "FS Log", "CDMS", "PCMT"};


/*=====================================================================================================*/
