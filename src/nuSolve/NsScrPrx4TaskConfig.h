/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2017-2020 Sergei Bolotin.
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

#ifndef NS_SCR_PRX_4_TASK_CONFIG_H
#define NS_SCR_PRX_4_TASK_CONFIG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <QtCore/QtGlobal>
#include <QtCore/QObject>


#if 0 < HAVE_SCRIPTS
#    include <QtScript/QScriptEngine>
#    include <QtScript/QScriptValue>
#endif


#include <SgTaskConfig.h>
#include <SgVlbiSession.h>



class QString;

extern const QString            str_WRONG;

extern const QString            str_VD_NONE;
extern const QString            str_VD_SB_DELAY;
extern const QString            str_VD_GRP_DELAY;
extern const QString            str_VD_PHS_DELAY;

extern const QString            str_VR_NONE;
extern const QString            str_VR_PHS_RATE;

extern const QString            str_WCM_BAND;
extern const QString            str_WCM_BASELINE;

extern const QString            str_OPM_BAND;
extern const QString            str_OPM_BASELINE;

extern const QString            str_OPA_ELIMINATE;
extern const QString            str_OPA_RESTORE;

extern const QString            str_EPM_INCRATE;
extern const QString            str_EPM_BSPLINE_LINEA;
extern const QString            str_EPM_BSPLINE_QUADR;

extern const QString            str_RM_NONE;
extern const QString            str_RM_CONTRIB_INTERNAL;
extern const QString            str_RM_CONTRIB_EXTERNAL;
extern const QString            str_RM_FLYBY_NMF;
extern const QString            str_RM_FLYBY_MTT;

extern const QString            str_CCS_DEFAULT;
extern const QString            str_CCS_FSLG;
extern const QString            str_CCS_CDMS;
extern const QString            str_CCS_PCMT;





/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4TaskConfig : public QObject
{
  Q_OBJECT
  Q_ENUMS (VlbiDelayType VlbiRateType WeightCorrectionMode OutliersProcessingMode
    OutliersProcessingAction EstimatorPwlMode RefractionModel CableCalSource)

  Q_PROPERTY(int qualityCodeThreshold
    WRITE setQualityCodeThreshold READ getQualityCodeThreshold)
  Q_PROPERTY(int goodQualityCodeAtStartup
    WRITE setGoodQualityCodeAtStartup READ getGoodQualityCodeAtStartup)
  Q_PROPERTY(bool useGoodQualityCodeAtStartup
    WRITE setUseGoodQualityCodeAtStartup READ getUseGoodQualityCodeAtStartup)
  Q_PROPERTY(VlbiDelayType useDelayType
    WRITE setUseDelayType READ getUseDelayType)
  Q_PROPERTY(VlbiRateType useRateType
    WRITE setUseRateType READ getUseRateType)
  Q_PROPERTY(int activeBandIdx
    WRITE setActiveBandIdx READ getActiveBandIdx)
  Q_PROPERTY(bool isSolveCompatible
    WRITE setIsSolveCompatible READ getIsSolveCompatible)
  Q_PROPERTY(bool useDynamicClockBreaks
    WRITE setUseDynamicClockBreaks READ getUseDynamicClockBreaks)
  Q_PROPERTY(bool useSolveObsSuppresionFlags
    WRITE setUseSolveObsSuppresionFlags READ getUseSolveObsSuppresionFlags)
  Q_PROPERTY(bool doWeightCorrection
    WRITE setDoWeightCorrection READ getDoWeightCorrection)
  Q_PROPERTY(bool useExternalWeights
    WRITE setUseExternalWeights READ getUseExternalWeights)
  Q_PROPERTY(WeightCorrectionMode wcMode
    WRITE setWcMode READ getWcMode)
  Q_PROPERTY(QString extWeightsFileName
    WRITE setExtWeightsFileName READ getExtWeightsFileName)
  Q_PROPERTY(EstimatorPwlMode pwlMode
    WRITE setPwlMode READ getPwlMode)
  Q_PROPERTY(bool doDownWeight
    WRITE setDoDownWeight READ getDoDownWeight)
  Q_PROPERTY(OutliersProcessingMode opMode
    WRITE setOpMode READ getOpMode)
  Q_PROPERTY(OutliersProcessingAction opAction
    WRITE setOpAction READ getOpAction)
  Q_PROPERTY(double opThreshold
    WRITE setOpThreshold READ getOpThreshold)
  Q_PROPERTY(int opIterationsLimit
    WRITE setOpIterationsLimit READ getOpIterationsLimit)
  Q_PROPERTY(bool opHave2SuppressWeightCorrection
    WRITE setOpHave2SuppressWeightCorrection READ getOpHave2SuppressWeightCorrection)
  Q_PROPERTY(bool opIsSolveCompatible
    WRITE setOpIsSolveCompatible READ getOpIsSolveCompatible)
  Q_PROPERTY(bool opHave2NormalizeResiduals
    WRITE setOpHave2NormalizeResiduals READ getOpHave2NormalizeResiduals)
  Q_PROPERTY(bool useExtAPrioriSitesPositions
    WRITE setUseExtAPrioriSitesPositions READ getUseExtAPrioriSitesPositions)
  Q_PROPERTY(bool useExtAPrioriSitesVelocities
    WRITE setUseExtAPrioriSitesVelocities READ getUseExtAPrioriSitesVelocities)
  Q_PROPERTY(bool useExtAPrioriSourcesPositions
    WRITE setUseExtAPrioriSourcesPositions READ getUseExtAPrioriSourcesPositions)
  Q_PROPERTY(bool useExtAPrioriSsm
    WRITE setUseExtAPrioriSsm READ getUseExtAPrioriSsm)
  Q_PROPERTY(bool useExtAPrioriErp
    WRITE setUseExtAPrioriErp READ getUseExtAPrioriErp)
  Q_PROPERTY(bool useExtAPrioriAxisOffsets
    WRITE setUseExtAPrioriAxisOffsets READ getUseExtAPrioriAxisOffsets)
  Q_PROPERTY(bool useExtAPrioriHiFyErp
    WRITE setUseExtAPrioriHiFyErp READ getUseExtAPrioriHiFyErp)
  Q_PROPERTY(bool useExtAPrioriMeanGradients
    WRITE setUseExtAPrioriMeanGradients READ getUseExtAPrioriMeanGradients)
  Q_PROPERTY(QString extAPrioriSitesPositionsFileName
    WRITE setExtAPrioriSitesPositionsFileName READ getExtAPrioriSitesPositionsFileName)
  Q_PROPERTY(QString extAPrioriSitesVelocitiesFileName
    WRITE setExtAPrioriSitesVelocitiesFileName READ getExtAPrioriSitesVelocitiesFileName)
  Q_PROPERTY(QString extAPrioriSourcesPositionsFileName
    WRITE setExtAPrioriSourcesPositionsFileName READ getExtAPrioriSourcesPositionsFileName)
  Q_PROPERTY(QString extAPrioriSsmFileName
    WRITE setExtAPrioriSsmFileName READ getExtAPrioriSsmFileName)
  Q_PROPERTY(QString extAPrioriErpFileName
    WRITE setExtAPrioriErpFileName READ getExtAPrioriErpFileName)
  Q_PROPERTY(QString extAPrioriAxisOffsetsFileName
    WRITE setExtAPrioriAxisOffsetsFileName READ getExtAPrioriAxisOffsetsFileName)
  Q_PROPERTY(QString extAPrioriHiFyErpFileName
    WRITE setExtAPrioriHiFyErpFileName READ getExtAPrioriHiFyErpFileName)
  Q_PROPERTY(QString extAPrioriMeanGradientsFileName
    WRITE setExtAPrioriMeanGradientsFileName READ getExtAPrioriMeanGradientsFileName)
  Q_PROPERTY(QString eccentricitiesFileName
    WRITE setEccentricitiesFileName READ getEccentricitiesFileName)
  Q_PROPERTY(bool have2ApplySsm
    WRITE setHave2ApplySsm READ getHave2ApplySsm)
  Q_PROPERTY(bool have2ApplyPxContrib
    WRITE setHave2ApplyPxContrib READ getHave2ApplyPxContrib)
  Q_PROPERTY(bool have2ApplyPyContrib
    WRITE setHave2ApplyPyContrib READ getHave2ApplyPyContrib)
  Q_PROPERTY(bool have2ApplyEarthTideContrib
    WRITE setHave2ApplyEarthTideContrib READ getHave2ApplyEarthTideContrib)
  Q_PROPERTY(bool have2ApplyOceanTideContrib
    WRITE setHave2ApplyOceanTideContrib READ getHave2ApplyOceanTideContrib)
  Q_PROPERTY(bool have2ApplyPoleTideContrib
    WRITE setHave2ApplyPoleTideContrib READ getHave2ApplyPoleTideContrib)
  Q_PROPERTY(bool have2ApplyUt1OceanTideHFContrib
    WRITE setHave2ApplyUt1OceanTideHFContrib READ getHave2ApplyUt1OceanTideHFContrib)
  Q_PROPERTY(bool have2ApplyPxyOceanTideHFContrib
    WRITE setHave2ApplyPxyOceanTideHFContrib READ getHave2ApplyPxyOceanTideHFContrib)
  Q_PROPERTY(bool have2ApplyNutationHFContrib
    WRITE setHave2ApplyNutationHFContrib READ getHave2ApplyNutationHFContrib)
  Q_PROPERTY(bool have2ApplyUt1LibrationContrib
    WRITE setHave2ApplyUt1LibrationContrib READ getHave2ApplyUt1LibrationContrib)
  Q_PROPERTY(bool have2ApplyPxyLibrationContrib
    WRITE setHave2ApplyPxyLibrationContrib READ getHave2ApplyPxyLibrationContrib)
  Q_PROPERTY(bool have2ApplyOceanPoleTideContrib
    WRITE setHave2ApplyOceanPoleTideContrib READ getHave2ApplyOceanPoleTideContrib)
  Q_PROPERTY(bool have2ApplyFeedCorrContrib
    WRITE setHave2ApplyFeedCorrContrib READ getHave2ApplyFeedCorrContrib)
  Q_PROPERTY(bool have2ApplyTiltRemvrContrib
    WRITE setHave2ApplyTiltRemvrContrib READ getHave2ApplyTiltRemvrContrib)
  Q_PROPERTY(bool have2ApplyOldOceanTideContrib
    WRITE setHave2ApplyOldOceanTideContrib READ getHave2ApplyOldOceanTideContrib)
  Q_PROPERTY(bool have2ApplyOldPoleTideContrib
    WRITE setHave2ApplyOldPoleTideContrib READ getHave2ApplyOldPoleTideContrib)
  Q_PROPERTY(bool have2ApplyAxisOffsetContrib
    WRITE setHave2ApplyAxisOffsetContrib READ getHave2ApplyAxisOffsetContrib)
  Q_PROPERTY(bool have2ApplyUnPhaseCalContrib
    WRITE setHave2ApplyUnPhaseCalContrib READ getHave2ApplyUnPhaseCalContrib)
/*
  Q_PROPERTY(bool have2ApplyNdryContrib
    WRITE setHave2ApplyNdryContrib READ getHave2ApplyNdryContrib)
  Q_PROPERTY(bool have2ApplyNwetContrib
    WRITE setHave2ApplyNwetContrib READ getHave2ApplyNwetContrib)
    * 
  Q_PROPERTY(TropZenithMap flybyTropZenithMap
    WRITE setFlybyTropZenithMap READ getFlybyTropZenithMap)
*/
  Q_PROPERTY(RefractionModel refractionModel
    WRITE setRefractionModel READ getRefractionModel)
  Q_PROPERTY(CableCalSource flybyCableCalSource
    WRITE setFlybyCableCalSource READ getFlybyCableCalSource)
  Q_PROPERTY(bool have2outputCovarMatrix
    WRITE setHave2outputCovarMatrix READ getHave2outputCovarMatrix)
  



public:
  enum VlbiDelayType
  {
    VD_NONE                   = SgTaskConfig::VD_NONE,
    VD_SB_DELAY               = SgTaskConfig::VD_SB_DELAY,
    VD_GRP_DELAY              = SgTaskConfig::VD_GRP_DELAY,
    VD_PHS_DELAY              = SgTaskConfig::VD_PHS_DELAY,
  };  
  enum VlbiRateType
  {
    VR_NONE                   = SgTaskConfig::VR_NONE,
    VR_PHS_RATE               = SgTaskConfig::VR_PHS_RATE,
  };
  enum WeightCorrectionMode
  {
    WCM_BAND                  = SgTaskConfig::WCM_BAND,
    WCM_BASELINE              = SgTaskConfig::WCM_BASELINE,
  };
  enum OutliersProcessingMode
  {
    OPM_BAND                  = SgTaskConfig::OPM_BAND,
    OPM_BASELINE              = SgTaskConfig::OPM_BASELINE,
  };
  enum OutliersProcessingAction
  {
    OPA_ELIMINATE             = SgTaskConfig::OPA_ELIMINATE,
    OPA_RESTORE               = SgTaskConfig::OPA_RESTORE,
  };
  enum EstimatorPwlMode
  {
    EPM_INCRATE               = SgTaskConfig::EPM_INCRATE,
    EPM_BSPLINE_LINEA         = SgTaskConfig::EPM_BSPLINE_LINEA,
    EPM_BSPLINE_QUADR         = SgTaskConfig::EPM_BSPLINE_QUADR,
  };
/*
  enum TropZenithMap
  {
    TZM_NONE                  = SgTaskConfig::TZM_NONE,
    TZM_NMF                   = SgTaskConfig::TZM_NMF,
    TZM_MTT                   = SgTaskConfig::TZM_MTT,
  };
*/
  enum RefractionModel
  {
    RM_NONE 									= SgTaskConfig::RM_NONE,
    RM_CONTRIB_INTERNAL 			= SgTaskConfig::RM_CONTRIB_INTERNAL,
    RM_CONTRIB_EXTERNAL 			= SgTaskConfig::RM_CONTRIB_EXTERNAL,
    RM_FLYBY_NMF 							= SgTaskConfig::RM_FLYBY_NMF,
    RM_FLYBY_MTT 							= SgTaskConfig::RM_FLYBY_MTT,
	};
  enum CableCalSource
  {
    CCS_DEFAULT               = SgTaskConfig::CCS_DEFAULT,
    CCS_FSLG                  = SgTaskConfig::CCS_FSLG,
    CCS_CDMS                  = SgTaskConfig::CCS_CDMS,
    CCS_PCMT                  = SgTaskConfig::CCS_PCMT
  };
  


  inline NsScrPrx4TaskConfig(SgTaskConfig& cfg, QObject *parent=0) : QObject(parent) {config_=&cfg;};

  inline ~NsScrPrx4TaskConfig() {config_=NULL;};


public slots:
  inline int   getQualityCodeThreshold() const {return config_->getQualityCodeThreshold();};
  inline int   getGoodQualityCodeAtStartup() const {return config_->getGoodQualityCodeAtStartup();};
  inline bool  getUseGoodQualityCodeAtStartup() const 
    {return config_->getUseGoodQualityCodeAtStartup();};
  inline VlbiDelayType getUseDelayType() const {return (VlbiDelayType) config_->getUseDelayType();};
  inline VlbiRateType getUseRateType() const {return (VlbiRateType) config_->getUseRateType();};
  inline int getActiveBandIdx() const {return config_->getActiveBandIdx();};
  inline bool getIsSolveCompatible() const {return config_->getIsSolveCompatible();};
  inline bool getUseDynamicClockBreaks() const {return config_->getUseDynamicClockBreaks();};
  inline bool getUseSolveObsSuppresionFlags() const {return config_->getUseSolveObsSuppresionFlags();};
  inline bool getDoWeightCorrection() const {return config_->getDoWeightCorrection();};
  inline bool getUseExternalWeights() const {return config_->getUseExternalWeights();};
  inline WeightCorrectionMode getWcMode() const {return (WeightCorrectionMode)config_->getWcMode();};
  inline QString getExtWeightsFileName() const {return config_->getExtWeightsFileName();};
  inline EstimatorPwlMode getPwlMode() const {return (EstimatorPwlMode)config_->getPwlMode();};
  inline bool getDoDownWeight() const {return config_->getDoDownWeight();};
  inline OutliersProcessingMode getOpMode() const {return (OutliersProcessingMode)config_->getOpMode();};
  inline OutliersProcessingAction getOpAction() const
    {return (OutliersProcessingAction)config_->getOpAction();};
  inline double getOpThreshold() const {return config_->getOpThreshold();};
  inline int getOpIterationsLimit() const {return config_->getOpIterationsLimit();};
  inline bool getOpHave2SuppressWeightCorrection() const
    {return config_->getOpHave2SuppressWeightCorrection();};
  inline bool getOpIsSolveCompatible() const {return config_->getOpIsSolveCompatible();};
  inline bool getOpHave2NormalizeResiduals() const {return config_->getOpHave2NormalizeResiduals();};
  inline bool getUseExtAPrioriSitesPositions() const {return config_->getUseExtAPrioriSitesPositions();};
  inline bool getUseExtAPrioriSitesVelocities() const
    {return config_->getUseExtAPrioriSitesVelocities();};
  inline bool getUseExtAPrioriSourcesPositions() const
    {return config_->getUseExtAPrioriSourcesPositions();};
  inline bool getUseExtAPrioriSsm() const
    {return config_->getUseExtAPrioriSourceSsm();};
  inline bool getUseExtAPrioriErp() const {return config_->getUseExtAPrioriErp();};
  inline bool getUseExtAPrioriAxisOffsets() const {return config_->getUseExtAPrioriAxisOffsets();};
  inline bool getUseExtAPrioriHiFyErp() const {return config_->getUseExtAPrioriHiFyErp();};
  inline bool getUseExtAPrioriMeanGradients() const {return config_->getUseExtAPrioriMeanGradients();};
  inline QString getExtAPrioriSitesPositionsFileName() const
    {return config_->getExtAPrioriSitesPositionsFileName();};
  inline QString getExtAPrioriSitesVelocitiesFileName() const
    {return config_->getExtAPrioriSitesVelocitiesFileName();};
  inline QString getExtAPrioriSourcesPositionsFileName() const
    {return config_->getExtAPrioriSourcesPositionsFileName();};
  inline QString getExtAPrioriSsmFileName() const
    {return config_->getExtAPrioriSourceSsmFileName();};
  inline QString getExtAPrioriErpFileName() const {return config_->getExtAPrioriErpFileName();};
  inline QString getExtAPrioriAxisOffsetsFileName() const
    {return config_->getExtAPrioriAxisOffsetsFileName();};
  inline QString getExtAPrioriHiFyErpFileName() const {return config_->getExtAPrioriHiFyErpFileName();};
  inline QString getExtAPrioriMeanGradientsFileName() const
    {return config_->getExtAPrioriMeanGradientsFileName();};
  inline QString getEccentricitiesFileName() const {return config_->getEccentricitiesFileName();};
  inline bool getHave2ApplySsm() const {return config_->getHave2ApplySourceSsm();};
  inline bool getHave2ApplyPxContrib() const {return config_->getHave2ApplyPxContrib();};
  inline bool getHave2ApplyPyContrib() const {return config_->getHave2ApplyPyContrib();};
  inline bool getHave2ApplyEarthTideContrib() const {return config_->getHave2ApplyEarthTideContrib();};
  inline bool getHave2ApplyOceanTideContrib() const {return config_->getHave2ApplyOceanTideContrib();};
  inline bool getHave2ApplyPoleTideContrib() const {return config_->getHave2ApplyPoleTideContrib();};
  inline bool getHave2ApplyUt1OceanTideHFContrib() const
    {return config_->getHave2ApplyUt1OceanTideHFContrib();};
  inline bool getHave2ApplyPxyOceanTideHFContrib() const
    {return config_->getHave2ApplyPxyOceanTideHFContrib();};
  inline bool getHave2ApplyNutationHFContrib() const {return config_->getHave2ApplyNutationHFContrib();};
  inline bool getHave2ApplyUt1LibrationContrib() const
    {return config_->getHave2ApplyUt1LibrationContrib();};
  inline bool getHave2ApplyPxyLibrationContrib() const
    {return config_->getHave2ApplyPxyLibrationContrib();};
  inline bool getHave2ApplyOceanPoleTideContrib() const
    {return config_->getHave2ApplyOceanPoleTideContrib();};
  inline bool getHave2ApplyFeedCorrContrib() const {return config_->getHave2ApplyFeedCorrContrib();};
  inline bool getHave2ApplyTiltRemvrContrib() const {return config_->getHave2ApplyTiltRemvrContrib();};
  inline bool getHave2ApplyOldOceanTideContrib() const
    {return config_->getHave2ApplyOldOceanTideContrib();};
  inline bool getHave2ApplyOldPoleTideContrib() const
    {return config_->getHave2ApplyOldPoleTideContrib();};
  inline bool getHave2ApplyAxisOffsetContrib() const {return config_->getHave2ApplyAxisOffsetContrib();};
  inline bool getHave2ApplyUnPhaseCalContrib() const {return config_->getHave2ApplyUnPhaseCalContrib();};
//inline bool getHave2ApplyNdryContrib() const {return config_->getHave2ApplyNdryContrib();};
//inline bool getHave2ApplyNwetContrib() const {return config_->getHave2ApplyNwetContrib();};
  inline bool getHave2outputCovarMatrix() const {return config_->getHave2outputCovarMatrix();};

  inline RefractionModel getRefractionModel() const 
    {return (RefractionModel)config_->getRefractionModel();};

  inline CableCalSource getFlybyCableCalSource() const 
    {return (CableCalSource)config_->getFlybyCableCalSource();};

  inline void setQualityCodeThreshold(int th) {config_->setQualityCodeThreshold(th);};
  inline void setGoodQualityCodeAtStartup(int th) {config_->setGoodQualityCodeAtStartup(th);};
  inline void setUseGoodQualityCodeAtStartup(bool use) {config_->setUseGoodQualityCodeAtStartup(use);};
  inline void setUseDelayType(VlbiDelayType t)
    {config_->setUseDelayType((SgTaskConfig::VlbiDelayType) t);};
  inline void setUseRateType(VlbiRateType t) {config_->setUseRateType((SgTaskConfig::VlbiRateType) t);};
  inline void setActiveBandIdx(int idx) {config_->setActiveBandIdx(idx);};
  inline void setIsSolveCompatible(bool is) {config_->setIsSolveCompatible(is);};
  inline void setUseDynamicClockBreaks(bool use) {config_->setUseDynamicClockBreaks(use);};
  inline void setUseSolveObsSuppresionFlags(bool use) {config_->setUseSolveObsSuppresionFlags(use);};
  inline void setDoWeightCorrection(bool l) {config_->setDoWeightCorrection(l);};
  inline void setUseExternalWeights(bool use) {config_->setUseExternalWeights(use);};
  inline void setWcMode(WeightCorrectionMode mode)
    {config_->setWcMode((SgTaskConfig::WeightCorrectionMode)mode);};
  inline void setExtWeightsFileName(QString fname) {config_->setExtWeightsFileName(fname);};
  inline void setPwlMode(EstimatorPwlMode mode)
    {config_->setPwlMode((SgTaskConfig::EstimatorPwlMode)mode);};
  inline void setDoDownWeight(bool l) {config_->setDoDownWeight(l);};
  inline void setOpMode(OutliersProcessingMode mode)
    {config_->setOpMode((SgTaskConfig::OutliersProcessingMode)mode);};
  inline void setOpAction(OutliersProcessingAction act)
    {config_->setOpAction((SgTaskConfig::OutliersProcessingAction)act);};
  inline void setOpThreshold(double thr) {config_->setOpThreshold(thr);};
  inline void setOpIterationsLimit(int lim) {config_->setOpIterationsLimit(lim);};
  inline void setOpHave2SuppressWeightCorrection(bool have)
    {config_->setOpHave2SuppressWeightCorrection(have);};
  inline void setOpIsSolveCompatible(bool is) {config_->setOpIsSolveCompatible(is);};
  inline void setOpHave2NormalizeResiduals(bool have) {config_->setOpHave2NormalizeResiduals(have);};
  inline void setUseExtAPrioriSitesPositions(bool b) {config_->setUseExtAPrioriSitesPositions(b);};
  inline void setUseExtAPrioriSitesVelocities(bool b) {config_->setUseExtAPrioriSitesVelocities(b);};
  inline void setUseExtAPrioriSourcesPositions(bool b) {config_->setUseExtAPrioriSourcesPositions(b);};
  inline void setUseExtAPrioriSsm(bool b) {config_->setUseExtAPrioriSourceSsm(b);};
  inline void setUseExtAPrioriErp(bool b) {config_->setUseExtAPrioriErp(b);};
  inline void setUseExtAPrioriAxisOffsets(bool b) {config_->setUseExtAPrioriAxisOffsets(b);};
  inline void setUseExtAPrioriHiFyErp(bool b) {config_->setUseExtAPrioriHiFyErp(b);};
  inline void setUseExtAPrioriMeanGradients(bool b) {config_->setUseExtAPrioriMeanGradients(b);};
  inline void setExtAPrioriSitesPositionsFileName(QString fname)
    {config_->setExtAPrioriSitesPositionsFileName(fname);};
  inline void setExtAPrioriSitesVelocitiesFileName(QString fname)
    {config_->setExtAPrioriSitesVelocitiesFileName(fname);};
  inline void setExtAPrioriSourcesPositionsFileName(QString fname)
    {config_->setExtAPrioriSourcesPositionsFileName(fname);};
  inline void setExtAPrioriSsmFileName(QString fname)
    {config_->setExtAPrioriSourceSsmFileName(fname);};
  inline void setExtAPrioriErpFileName(QString fname) {config_->setExtAPrioriErpFileName(fname);};
  inline void setExtAPrioriAxisOffsetsFileName(QString fname)
    {config_->setExtAPrioriAxisOffsetsFileName(fname);};
  inline void setExtAPrioriHiFyErpFileName(QString fname)
    {config_->setExtAPrioriHiFyErpFileName(fname);};
  inline void setExtAPrioriMeanGradientsFileName(QString fname)
    {config_->setExtAPrioriMeanGradientsFileName(fname);};
  inline void setEccentricitiesFileName(QString fname) {config_->setEccentricitiesFileName(fname);};
  inline void setHave2ApplySsm(bool have) {config_->setHave2ApplySourceSsm(have);};
  inline void setHave2ApplyPxContrib(bool have) {config_->setHave2ApplyPxContrib(have);};
  inline void setHave2ApplyPyContrib(bool have) {config_->setHave2ApplyPyContrib(have);};
  inline void setHave2ApplyEarthTideContrib(bool have) {config_->setHave2ApplyEarthTideContrib(have);};
  inline void setHave2ApplyOceanTideContrib(bool have) {config_->setHave2ApplyOceanTideContrib(have);};
  inline void setHave2ApplyPoleTideContrib(bool have) {config_->setHave2ApplyPoleTideContrib(have);};
  inline void setHave2ApplyUt1OceanTideHFContrib(bool have)
    {config_->setHave2ApplyUt1OceanTideHFContrib(have);};
  inline void setHave2ApplyPxyOceanTideHFContrib(bool have)
    {config_->setHave2ApplyPxyOceanTideHFContrib(have);};
  inline void setHave2ApplyNutationHFContrib(bool have) {config_->setHave2ApplyNutationHFContrib(have);};
  inline void setHave2ApplyUt1LibrationContrib(bool have)
    {config_->setHave2ApplyUt1LibrationContrib(have);};
  inline void setHave2ApplyPxyLibrationContrib(bool have)
    {config_->setHave2ApplyPxyLibrationContrib(have);};
  inline void setHave2ApplyOceanPoleTideContrib(bool have)
    {config_->setHave2ApplyOceanPoleTideContrib(have);};
  inline void setHave2ApplyFeedCorrContrib(bool have) {config_->setHave2ApplyFeedCorrContrib(have);};
  inline void setHave2ApplyTiltRemvrContrib(bool have) {config_->setHave2ApplyTiltRemvrContrib(have);};
  inline void setHave2ApplyOldOceanTideContrib(bool have)
    {config_->setHave2ApplyOldOceanTideContrib(have);};
  inline void setHave2ApplyOldPoleTideContrib(bool have)
    {config_->setHave2ApplyOldPoleTideContrib(have);};
  inline void setHave2ApplyAxisOffsetContrib(bool have) {config_->setHave2ApplyAxisOffsetContrib(have);};
  inline void setHave2ApplyUnPhaseCalContrib(bool have) {config_->setHave2ApplyUnPhaseCalContrib(have);};
//inline void setHave2ApplyNdryContrib(bool have) {config_->setHave2ApplyNdryContrib(have);};
//inline void setHave2ApplyNwetContrib(bool have) {config_->setHave2ApplyNwetContrib(have);};
  inline void setHave2outputCovarMatrix(bool have) {config_->setHave2outputCovarMatrix(have);};
  inline void setRefractionModel(RefractionModel m) 
    {config_->setRefractionModel((SgTaskConfig::RefractionModel) m);};
  inline void setFlybyCableCalSource(CableCalSource s) 
    {config_->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);};
  
private:
  SgTaskConfig                 *config_;


};
/*=====================================================================================================*/



#if 0 < HAVE_SCRIPTS
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::VlbiDelayType);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::VlbiRateType);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::WeightCorrectionMode);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::OutliersProcessingMode);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::OutliersProcessingAction);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::EstimatorPwlMode);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::RefractionModel);
Q_DECLARE_METATYPE(NsScrPrx4TaskConfig::CableCalSource);
#endif


/*=====================================================================================================*/
//
// aux functions:
//

//   enums to string convertors:
//
//  enum VlbiDelayType
//
#if 0 < HAVE_SCRIPTS


#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigDtype(QScriptEngine */*eng*/,
  const NsScrPrx4TaskConfig::VlbiDelayType &t)
{
  return
    t==NsScrPrx4TaskConfig::VD_NONE?str_VD_NONE:
      (t==NsScrPrx4TaskConfig::VD_SB_DELAY?str_VD_SB_DELAY:
        (t==NsScrPrx4TaskConfig::VD_GRP_DELAY?str_VD_GRP_DELAY:
          (t==NsScrPrx4TaskConfig::VD_PHS_DELAY?str_VD_PHS_DELAY:str_WRONG)));
};
#else
inline QScriptValue toScriptValue4ConfigDtype(QScriptEngine *eng,
  const NsScrPrx4TaskConfig::VlbiDelayType &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigDtype(const QScriptValue &obj, NsScrPrx4TaskConfig::VlbiDelayType &t)
{
  t = (NsScrPrx4TaskConfig::VlbiDelayType) obj.toInt32();
};



//  enum VlbiRateType
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigRtype(QScriptEngine */*eng*/,
  const NsScrPrx4TaskConfig::VlbiRateType &t)
{
  return
    t==NsScrPrx4TaskConfig::VR_NONE?str_VR_NONE:
      (t==NsScrPrx4TaskConfig::VR_PHS_RATE?str_VR_PHS_RATE:str_WRONG);
};
#else
inline QScriptValue toScriptValue4ConfigRtype(QScriptEngine *eng,
  const NsScrPrx4TaskConfig::VlbiRateType &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigRtype(const QScriptValue &obj, NsScrPrx4TaskConfig::VlbiRateType &t)
{
  t = (NsScrPrx4TaskConfig::VlbiRateType) obj.toInt32();
};



//  enum WeightCorrectionMode
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigWrMode(QScriptEngine */*eng*/,
  const NsScrPrx4TaskConfig::WeightCorrectionMode &m)
{
  return
    m==NsScrPrx4TaskConfig::WCM_BAND?str_WCM_BAND:
      (m==NsScrPrx4TaskConfig::WCM_BASELINE?str_WCM_BASELINE:str_WRONG);
};
#else
inline QScriptValue toScriptValue4ConfigWrMode(QScriptEngine *eng,
  const NsScrPrx4TaskConfig::WeightCorrectionMode &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigWrMode(const QScriptValue &obj,
  NsScrPrx4TaskConfig::WeightCorrectionMode &m)
{
  m = (NsScrPrx4TaskConfig::WeightCorrectionMode) obj.toInt32();
};




//  enum RefractionModel
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigRefractionModel(QScriptEngine */*eng*/,
  const NsScrPrx4TaskConfig::RefractionModel &m)
{
  return
    m==NsScrPrx4TaskConfig::RM_NONE?str_RM_NONE:
      (m==NsScrPrx4TaskConfig::RM_CONTRIB_INTERNAL?str_RM_CONTRIB_INTERNAL:
        (m==NsScrPrx4TaskConfig::RM_CONTRIB_EXTERNAL?str_RM_CONTRIB_EXTERNAL:
					(m==NsScrPrx4TaskConfig::RM_FLYBY_NMF?str_RM_FLYBY_NMF:
						(m==NsScrPrx4TaskConfig::RM_FLYBY_MTT?str_RM_FLYBY_MTT:str_WRONG))));
};
#else
inline QScriptValue toScriptValue4ConfigRefractionModel(QScriptEngine *eng,
  const NsScrPrx4TaskConfig::RefractionModel &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif



inline void fromScriptValue4ConfigRefractionModel(const QScriptValue &obj,
  NsScrPrx4TaskConfig::RefractionModel &m)
{
  m = (NsScrPrx4TaskConfig::RefractionModel) obj.toInt32();
};



// enum CableCalSource
//
inline QScriptValue toScriptValue4ConfigCableCalSource(QScriptEngine */*eng*/,
  const NsScrPrx4TaskConfig::CableCalSource &s)
{
  return
    s==NsScrPrx4TaskConfig::CCS_DEFAULT?str_CCS_DEFAULT:
      (s==NsScrPrx4TaskConfig::CCS_FSLG?str_CCS_FSLG:
        (s==NsScrPrx4TaskConfig::CCS_CDMS?str_CCS_CDMS:
          (s==NsScrPrx4TaskConfig::CCS_PCMT?str_CCS_PCMT:str_WRONG)));
};



//
inline void fromScriptValue4ConfigCableCalSource(const QScriptValue &obj,
  NsScrPrx4TaskConfig::CableCalSource &s)
{
  s = (NsScrPrx4TaskConfig::CableCalSource) obj.toInt32();
};






//  enum OutliersProcessingMode:
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigOpMode(QScriptEngine */*eng*/, 
  const NsScrPrx4TaskConfig::OutliersProcessingMode &m)
{
  return 
    m==NsScrPrx4TaskConfig::OPM_BAND?str_OPM_BAND:
      (m==NsScrPrx4TaskConfig::OPM_BASELINE?str_OPM_BASELINE:str_WRONG);
};
#else
inline QScriptValue toScriptValue4ConfigOpMode(QScriptEngine *eng, 
  const NsScrPrx4TaskConfig::OutliersProcessingMode &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigOpMode(const QScriptValue &obj, 
  NsScrPrx4TaskConfig::OutliersProcessingMode &m)
{
  m = (NsScrPrx4TaskConfig::OutliersProcessingMode) obj.toInt32();
};




//  enum OutliersProcessingAction:
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigOpAction(QScriptEngine */*eng*/, 
  const NsScrPrx4TaskConfig::OutliersProcessingAction &a)
{
  return 
    a==NsScrPrx4TaskConfig::OPA_ELIMINATE?str_OPA_ELIMINATE:
      (a==NsScrPrx4TaskConfig::OPA_RESTORE?str_OPA_RESTORE:str_WRONG);
};
#else
inline QScriptValue toScriptValue4ConfigOpAction(QScriptEngine *eng, 
  const NsScrPrx4TaskConfig::OutliersProcessingAction &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigOpAction(const QScriptValue &obj,
  NsScrPrx4TaskConfig::OutliersProcessingAction &a)
{
  a = (NsScrPrx4TaskConfig::OutliersProcessingAction) obj.toInt32();
};



//  enum EstimatorPwlMode:
//
#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ConfigEpwlMode(QScriptEngine */*eng*/, 
  const NsScrPrx4TaskConfig::EstimatorPwlMode &m)
{
  return 
    m==NsScrPrx4TaskConfig::EPM_INCRATE?str_EPM_INCRATE:
      (m==NsScrPrx4TaskConfig::EPM_BSPLINE_LINEA?str_EPM_BSPLINE_LINEA:
        (m==NsScrPrx4TaskConfig::EPM_BSPLINE_QUADR?str_EPM_BSPLINE_QUADR:str_WRONG));
};
#else
inline QScriptValue toScriptValue4ConfigEpwlMode(QScriptEngine *eng, 
  const NsScrPrx4TaskConfig::EstimatorPwlMode &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ConfigEpwlMode(const QScriptValue &obj, 
  NsScrPrx4TaskConfig::EstimatorPwlMode &m)
{
  m = (NsScrPrx4TaskConfig::EstimatorPwlMode) obj.toInt32();
};



#endif // SCRIPT_SUPPORT

/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_TASK_CONFIG_H
