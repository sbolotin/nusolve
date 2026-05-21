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

#ifndef SG_TASK_CONFIG_H
#define SG_TASK_CONFIG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>

#include <SgAttribute.h>
#include <SgMJD.h>

#include <iostream>

class QDataStream;



extern const QString sCableCalSourceNames[4];

/***===================================================================================================*/
/**
 * SgTaskConfig -- describes which models, algorithms have to be applied in the analysis.
 *
 */
/**====================================================================================================*/
class SgTaskConfig
{
public:
  enum VlbiDataType
  {
    VDT_DELAY                 = 0,
    VDT_RATE                  = 1,
  };
  enum VlbiDelayType
  {
    VD_NONE                   = 0,
    VD_SB_DELAY               = 1,
    VD_GRP_DELAY              = 2,
    VD_PHS_DELAY              = 3,
  };
  enum VlbiRateType
  {
    VR_NONE                   = 0,
    VR_PHS_RATE               = 1,
  };
  enum WeightCorrectionMode
  {
    WCM_BAND                  = 0,
    WCM_BASELINE              = 1,
  };
  enum OutliersProcessingMode
  {
    OPM_BAND                  = 0,
    OPM_BASELINE              = 1,
  };
  enum OutliersProcessingAction
  {
    OPA_ELIMINATE             = 0,
    OPA_RESTORE               = 1,
  };
  enum EstimatorPwlMode
  {
    EPM_INCRATE               = 0,
    EPM_BSPLINE_LINEA         = 1,
    EPM_BSPLINE_QUADR         = 2,
  };
  // models:
  enum RefractionModel
  {
    RM_NONE                   = 0,
    RM_CONTRIB_INTERNAL       = 1, // contributions from the database
    RM_CONTRIB_EXTERNAL       = 2, // contributions from the external files
    RM_FLYBY_NMF              = 3, // fly by, NMF
    RM_FLYBY_MTT              = 4, // fly by, MTT
  };
  // cable corrections:
  enum CableCalSource
  {
    CCS_DEFAULT               = 0,
    CCS_FSLG                  = 1,
    CCS_CDMS                  = 2,
    CCS_PCMT                  = 3,
  };
  //
  struct AutomaticProcessing
  {
    enum FinalSolution
    {
      FS_BASELINE               = 0,
      FS_UT1                    = 1,
    };
    bool                        doSessionSetup_;
    bool                        doIonoCorrection4SBD_;
    bool                        doAmbigResolution_;
    bool                        doClockBreaksDetection_;
    bool                        doIonoCorrection4All_;
    bool                        doOutliers_;
    bool                        doWeights_;
    bool                        doReportNotUsedData_;
    FinalSolution               finalSolution_;
    inline AutomaticProcessing() 
      {doSessionSetup_=doIonoCorrection4SBD_=true; doIonoCorrection4All_=doAmbigResolution_=
      doClockBreaksDetection_=doOutliers_=doWeights_=doReportNotUsedData_=false; 
      finalSolution_=FS_BASELINE;};
    inline AutomaticProcessing(const AutomaticProcessing& ap)
      {*this = ap;};

    inline ~AutomaticProcessing(){};
    inline AutomaticProcessing& operator=(const AutomaticProcessing& ap)
      {doSessionSetup_=ap.doSessionSetup_; doIonoCorrection4SBD_=ap.doIonoCorrection4SBD_; 
      doAmbigResolution_=ap.doAmbigResolution_; doClockBreaksDetection_=ap.doClockBreaksDetection_;
      doIonoCorrection4All_=ap.doIonoCorrection4All_; doOutliers_=ap.doOutliers_; 
      doWeights_=ap.doWeights_; doReportNotUsedData_=ap.doReportNotUsedData_;
      finalSolution_=ap.finalSolution_; return *this;};
  };

  //
  static QString evaluatePath2(const QString& dir2, const QString& path2);

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgTaskConfig();

  /**A constructor.
   * Creates a copy of the object.
   */
  inline SgTaskConfig(const SgTaskConfig&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgTaskConfig();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgTaskConfig& operator=(const SgTaskConfig&);

  bool operator==(const SgTaskConfig&) const;
  inline bool operator!=(const SgTaskConfig& cfg) const {return !(*this==cfg);};

  // gets:
  /**Returns a name of config set.
   */
  inline const QString& getName() const;
  
  /**Returns a threshold of quality codes to use in analysis.
   */
  inline int getQualityCodeThreshold() const;

  /**Returns a true if observations with a quality code 'G' should be used.
   */
  inline bool getUseQualityCodeG() const;

  inline bool getUseQualityCodeH() const;

  /**
   */
  inline int getGoodQualityCodeAtStartup() const;

  /**
   */
  inline bool getUseGoodQualityCodeAtStartup() const;

  /**Returns type of observables included in analysis.
   */
  inline VlbiDataType  getDataType() const {return dataType_;};
  inline VlbiDelayType getUseDelayType() const;
  inline VlbiRateType  getUseRateType() const;

  /**Returns an index of the active band.
   */
  inline int getActiveBandIdx() const;

  /**Returns true if weight correction have to be performed.
   */
  inline bool getDoWeightCorrection() const;

  /**Returns true if SOLVE's behavior should be reproduced.
   */
  inline bool getIsSolveCompatible() const;

  /**Returns true if clock breaks are estimated in a common solution.
   */
  inline bool getUseDynamicClockBreaks() const;

  /**Returns true if the SOLVE's observation suppresion flags should be honored.
   */
  inline bool getUseSolveObsSuppresionFlags() const;

  /**Returns true if external weight should be used.
   */
  inline bool getUseExternalWeights() const;

  /**Returns a mode of the weight correction action.
   */
  inline WeightCorrectionMode getWcMode() const;

  /**Returns a mode of the pwl parameters.
   */
  inline EstimatorPwlMode getPwlMode() const;

  inline bool getDoDownWeight() const;

  /**Returns a name of a file with external weights.
   */
  inline const QString& getExtWeightsFileName() const;

  inline double getMinAuxSigma4Delay() const {return minAuxSigma4Delay_;};
  inline double getMinAuxSigma4Rate () const {return minAuxSigma4Rate_;};
  inline double getInitAuxSigma4Delay() const {return initAuxSigma4Delay_;};
  inline double getInitAuxSigma4Rate () const {return initAuxSigma4Rate_;};

  /**Returns a mode of the outliers processing action.
   */
  inline OutliersProcessingMode getOpMode() const;

  /**Returns an action that should be performed on outliers: elimination or restoration.
   */
  inline OutliersProcessingAction getOpAction() const;

  /**Returns a threshold for outliers processing action.
   */
  inline double getOpThreshold() const;

  /**Returns a number of iterations for outliers processing.
   */
  inline int getOpIterationsLimit() const;

  /**Returns true if reweighting (if specified) should be applied during outliers processing.
   */
  inline bool getOpHave2SuppressWeightCorrection() const;

  /**Returns true if reweighting (if specified) should run in SOLVE compatible mode.
   */
  inline bool getOpIsSolveCompatible() const;

  /**Returns true if residuals should be normalized (default mode).
   */
  inline bool getOpHave2NormalizeResiduals() const;

  /**Returns true if ionospheric corrections are applied fo single band delays.
   */
  inline bool getDoIonoCorrection4SBD() const;

  /**Returns true if clock breaks processed automatically (mode 1).
   */
  inline bool getDoClockBreakDetectionMode1() const;

  /**Returns true if early stage (mode 1) outliers elimination should be performed.
   */
  inline bool getDoOutliersEliminationMode1() const;

  /**Returns true if switching band's tabs by user switches active band too.
   */
  inline bool getIsActiveBandFollowsTab() const;

  /**Returns true if user's selection of observable on plots changes the observable in the config too.
   */
  inline bool getIsObservableFollowsPlot() const;

  inline bool getIsNoviceUser() const {return isNoviceUser_;};
  
  inline bool getHave2outputCovarMatrix() const {return have2outputCovarMatrix_;};

  /**Returns true if run a tests.
   */
  inline bool getIsTesting() const;
 
  /**Returns true if an external file with stations positions should be used.
   */
  inline bool getUseExtAPrioriSitesPositions() const;

  /**Returns true if an external file with stations velocities should be used.
   */
  inline bool getUseExtAPrioriSitesVelocities() const;

  /**Returns true if an external file with sources positions should be used.
   */
  inline bool getUseExtAPrioriSourcesPositions() const;

  /**Returns true if an external file with sources positions should be used.
   */
  inline bool getUseExtAPrioriSourceSsm() const;

  /**Returns true if an external file with EOP should be used.
   */
  inline bool getUseExtAPrioriErp() const;

  /**Returns true if an external file with stations axis offsets should be used.
   */
  inline bool getUseExtAPrioriAxisOffsets() const;

  /**Returns true if an external file with diurnal and semidiurnal EOP variations model should be used.
   */
  inline bool getUseExtAPrioriHiFyErp() const;
  
  /**Returns true if an external file with a priori mean tropospheric gradients should be used.
   */
  inline bool getUseExtAPrioriMeanGradients() const;

  /**Returns name of an external file with stations positions should be used.
   */
  inline const QString& getExtAPrioriSitesPositionsFileName() const;

  /**Returns name of an external file with stations velocities should be used.
   */
  inline const QString& getExtAPrioriSitesVelocitiesFileName() const;

  /**Returns name of an external file with sources positions that should be used.
   */
  inline const QString& getExtAPrioriSourcesPositionsFileName() const;

  /**Returns name of an external file with sources positions that should be used.
   */
  inline const QString& getExtAPrioriSourceSsmFileName() const;

  /**Returns name of an external file with EOP that should be used.
   */
  inline const QString& getExtAPrioriErpFileName() const;

  /**Returns name of an external file with stations axis offsets should be used.
   */
  inline const QString& getExtAPrioriAxisOffsetsFileName() const;
  
  /**Returns name of an external file with stations diurnal and semidiurnal EOP variations model 
   * should be used.
   */
  inline const QString& getExtAPrioriHiFyErpFileName() const;

  /**Returns name of an external file with a priori mean tropospheric gradients
   * should be used.
   */
  inline const QString& getExtAPrioriMeanGradientsFileName() const;

  /**Returns name of a file with eccentricities
   */
  inline const QString& getEccentricitiesFileName() const;

  inline bool getContribsAreFromDatabase()          const {return contribsAreFromDatabase_;};
  inline bool getHave2ApplyPxContrib()              const {return have2ApplyPxContrib_;};
  inline bool getHave2ApplyPyContrib()              const {return have2ApplyPyContrib_;};
  inline bool getHave2ApplyEarthTideContrib()       const {return have2ApplyEarthTideContrib_;};
  inline bool getHave2ApplyOceanTideContrib()       const {return have2ApplyOceanTideContrib_;};
  inline bool getHave2ApplyPoleTideContrib()        const {return have2ApplyPoleTideContrib_;};
  inline bool getHave2ApplyGpsIonoContrib()         const {return have2ApplyGpsIonoContrib_;};
  inline bool getHave2ApplySourceSsm()              const {return have2ApplySourceSsm_;};
  inline bool getHave2ApplyUt1OceanTideHFContrib()  const {return have2ApplyUt1OceanTideHFContrib_;};
  inline bool getHave2ApplyPxyOceanTideHFContrib()  const {return have2ApplyPxyOceanTideHFContrib_;};
  inline bool getHave2ApplyNutationHFContrib()      const {return have2ApplyNutationHFContrib_;};
  inline bool getHave2ApplyUt1LibrationContrib()    const {return have2ApplyUt1LibrationContrib_;};
  inline bool getHave2ApplyPxyLibrationContrib()    const {return have2ApplyPxyLibrationContrib_;};
  inline bool getHave2ApplyOceanPoleTideContrib()   const {return have2ApplyOceanPoleTideContrib_;};
  inline bool getHave2ApplyFeedCorrContrib()        const {return have2ApplyFeedCorrContrib_;};
  inline bool getHave2ApplyTiltRemvrContrib()       const {return have2ApplyTiltRemvrContrib_;};
  inline bool getHave2ApplyOldOceanTideContrib()    const {return have2ApplyOldOceanTideContrib_;};
  inline bool getHave2ApplyOldPoleTideContrib()     const {return have2ApplyOldPoleTideContrib_;};

  inline bool getHave2ApplyAxisOffsetContrib()      const {return have2ApplyAxisOffsetContrib_;};
  inline bool getHave2ApplyUnPhaseCalContrib()      const {return have2ApplyUnPhaseCalContrib_;};
  inline bool getIsIonosphereFreeUncorrelated()     const {return isIonosphereFreeUncorrelated_;};
  inline bool getUseOldMode4IonosphereSigma()       const {return useOldMode4IonosphereSigma_;};
  
  

  //inline TropZenithMap  getFlybyTropZenithMap()     const {return flybyTropZenithMap_;};
  inline CableCalSource getFlybyCableCalSource()    const {return flybyCableCalSource_;};

  //
  inline RefractionModel getRefractionModel()       const {return refractionModel_;};

  inline const QString& getLastModifiedNetId()      const {return lastModifiedNetId_;};

  inline const SgMJD& getT2Bgn()                    const {return t2Bgn_;};
  inline const SgMJD& getT2End()                    const {return t2End_;};

  //
  // sets:
  //
  /**Sets up name of the config.
   */
  inline void setName(const QString&);

  /**Sets up a threshold of quality codes to use in analysis.
   */
  inline void setQualityCodeThreshold(int);

  /**Sets up usability of observations with a quality code 'G'.
   */
  inline void setUseQualityCodeG(bool);

  inline void setUseQualityCodeH(bool);

  /**
   */
  inline void setGoodQualityCodeAtStartup(int);

  /**
   */
  inline void setUseGoodQualityCodeAtStartup(bool);

  /**Sets up type of observables that will be included in analysis.
   */
  inline void setDataType(VlbiDataType t) {dataType_ = t;};
  inline void setUseDelayType(VlbiDelayType);
  inline void setUseRateType(VlbiRateType);

  /**Sets up an index of the active band.
   */
  inline void setActiveBandIdx(int);

  /**Sets up automatic ionosphere correction for single band delay.
   */
  inline void setDoIonoCorrection4SBD(bool);

  /**Turns on or off the weight corrections.
   */
  inline void setDoWeightCorrection(bool);

  /**Turns on or off the reproductions of SOLVE's behavior.
   */
  inline void setIsSolveCompatible(bool);
  
  /**Turns on or off the estimation of clock breaks in a common solution.
   */
  inline void setUseDynamicClockBreaks(bool);
  
  /**Turns on or off the use of SOLVE's observation suppresion flags.
   */
  inline void setUseSolveObsSuppresionFlags(bool);

  /**Turns on or off the using of external weight.
   */
  inline void setUseExternalWeights(bool);

  /**Sets up a mode of the weight correction.
   */
  inline void setWcMode(WeightCorrectionMode);

  /**Sets up a mode of the pwl parameters.
   */
  inline void setPwlMode(EstimatorPwlMode);

  inline void setDoDownWeight(bool);

  /**Sets up a name of a file with external weights.
   */
  inline void setExtWeightsFileName(const QString&);
  
  inline void setMinAuxSigma4Delay(double s) {minAuxSigma4Delay_ = s;};
  inline void setMinAuxSigma4Rate (double s) {minAuxSigma4Rate_  = s;};
  inline void setInitAuxSigma4Delay(double s) {initAuxSigma4Delay_ = s;};
  inline void setInitAuxSigma4Rate (double s) {initAuxSigma4Rate_  = s;};
  
  /**Sets up a mode of the outliers processing.
   */
  inline void setOpMode(OutliersProcessingMode);

  /**Sets up an action that should be performed on outliers: elimination or restoration.
   */
  inline void setOpAction(OutliersProcessingAction);

  /**Sets up a threshold for the outliers processing action.
   */
  inline void setOpThreshold(double);

  /**Set up a limit for iterations in outliers processing.
   */
  inline void setOpIterationsLimit(int);
  
  /**Set up either reweighting (if specified) should be applied during outliers processing.
   */
  inline void setOpHave2SuppressWeightCorrection(bool);

  /**Set up either reweighting (if specified) should run in SOLVE compatible mode.
   */
  inline void setOpIsSolveCompatible(bool);

  /**Set up either residuals should be normalized.
   */
  inline void setOpHave2NormalizeResiduals(bool);

  /**Sets up the automatic clock break processing (mode 1).
   */
  inline void setDoClockBreakDetectionMode1(bool);

  /**Sets up the early stage (mode 1) outliers elimination.
   */
  inline void setDoOutliersEliminationMode1(bool);

  /**Sets up active band behavior.
   */
  inline void setIsActiveBandFollowsTab(bool);

  /**Sets up observable behavior.
   */
  inline void setIsObservableFollowsPlot(bool);

  inline void setIsNoviceUser(bool is) {isNoviceUser_ = is;};

  inline void setHave2outputCovarMatrix(bool have2) {have2outputCovarMatrix_ = have2;};
  
  /**Sets up testing some feature.
   */
  inline void setIsTesting(bool);

  /**Turns on or off usage of an external file with stations positions.
   */
  inline void setUseExtAPrioriSitesPositions(bool);

  /**Turns on or off usage of an external file with stations velocities.
   */
  inline void setUseExtAPrioriSitesVelocities(bool);

  /**Turns on or off usage of an external file with sources positions.
   */
  inline void setUseExtAPrioriSourcesPositions(bool);

  /**Turns on or off usage of an external file with sources positions.
   */
  inline void setUseExtAPrioriSourceSsm(bool);

  /**Turns on or off usage of an external file with EOP.
   */
  inline void setUseExtAPrioriErp(bool);

  /**Turns on or off usage of an external file with stations axis offsets.
   */
  inline void setUseExtAPrioriAxisOffsets(bool);

  /**Turns on or off usage of an external file with diurnal and semidiurnal EOP variations model.
   */
  inline void setUseExtAPrioriHiFyErp(bool);
  
  /**Turns on or off usage of an external file with a priori mean tropospheric gradients.
   */
  inline void setUseExtAPrioriMeanGradients(bool);

  /**Sets up a name of an external file with stations positions should be used.
   */
  inline void setExtAPrioriSitesPositionsFileName(const QString&);

  /**Sets up a name of an external file with stations velocities should be used.
   */
  inline void setExtAPrioriSitesVelocitiesFileName(const QString&);

  /**Sets up a name of an external file with sources positions should be used.
   */
  inline void setExtAPrioriSourcesPositionsFileName(const QString&);

  /**Sets up a name of an external file with sources positions should be used.
   */
  inline void setExtAPrioriSourceSsmFileName(const QString&);

  /**Sets up a name of an external file with EOP should be used.
   */
  inline void setExtAPrioriErpFileName(const QString&);

  /**Sets up a name of an external file with stations axis offsets should be used.
   */
  inline void setExtAPrioriAxisOffsetsFileName(const QString&);

  /**Sets up a name of an external file with diurnal and semidiurnal EOP variations model should be used.
   */
  inline void setExtAPrioriHiFyErpFileName(const QString&);
  
  /**Sets up a name of an external file with a priori mean tropospheric gradients should be used.
   */
  inline void setExtAPrioriMeanGradientsFileName(const QString&);
  
  /**Sets up a name of a file with eccentricities.
   */
  inline void setEccentricitiesFileName(const QString&);
  
  //
  inline void setContribsAreFromDatabase(bool b)          {contribsAreFromDatabase_ = b;};
  inline void setHave2ApplyPxContrib(bool b)              {have2ApplyPxContrib_ = b;};
  inline void setHave2ApplyPyContrib(bool b)              {have2ApplyPyContrib_ = b;};
  inline void setHave2ApplyEarthTideContrib(bool b)       {have2ApplyEarthTideContrib_ = b;};
  inline void setHave2ApplyOceanTideContrib(bool b)       {have2ApplyOceanTideContrib_ = b;};
  inline void setHave2ApplyPoleTideContrib(bool b)        {have2ApplyPoleTideContrib_ = b;};
  inline void setHave2ApplyGpsIonoContrib(bool b)         {have2ApplyGpsIonoContrib_ = b;};
  inline void setHave2ApplySourceSsm(bool b)              {have2ApplySourceSsm_ = b;};
  inline void setHave2ApplyUt1OceanTideHFContrib(bool b)  {have2ApplyUt1OceanTideHFContrib_ = b;};
  inline void setHave2ApplyPxyOceanTideHFContrib(bool b)  {have2ApplyPxyOceanTideHFContrib_ = b;};
  inline void setHave2ApplyNutationHFContrib(bool b)      {have2ApplyNutationHFContrib_ = b;};
  inline void setHave2ApplyUt1LibrationContrib(bool b)    {have2ApplyUt1LibrationContrib_ = b;};
  inline void setHave2ApplyPxyLibrationContrib(bool b)    {have2ApplyPxyLibrationContrib_ = b;};
  inline void setHave2ApplyOceanPoleTideContrib(bool b)   {have2ApplyOceanPoleTideContrib_ = b;};
  inline void setHave2ApplyFeedCorrContrib(bool b)        {have2ApplyFeedCorrContrib_ = b;};
  inline void setHave2ApplyTiltRemvrContrib(bool b)       {have2ApplyTiltRemvrContrib_ = b;};
  inline void setHave2ApplyOldOceanTideContrib(bool b)    {have2ApplyOldOceanTideContrib_ = b;};
  inline void setHave2ApplyOldPoleTideContrib(bool b)     {have2ApplyOldPoleTideContrib_ = b;};
  //
  inline void setHave2ApplyAxisOffsetContrib(bool b)      {have2ApplyAxisOffsetContrib_ = b;};
  inline void setHave2ApplyUnPhaseCalContrib(bool b)      {have2ApplyUnPhaseCalContrib_ = b;};
  inline void setIsIonosphereFreeUncorrelated(bool b)     {isIonosphereFreeUncorrelated_ = b;};
  inline void setUseOldMode4IonosphereSigma(bool b)       {useOldMode4IonosphereSigma_ = b;};

  inline void setFlybyCableCalSource(CableCalSource c)    {flybyCableCalSource_ = c;};

  //
  inline void setRefractionModel(RefractionModel m)       {refractionModel_ = m;};


  inline void setLastModifiedNetId(const QString& netId)  {lastModifiedNetId_ = netId;};
  inline void setT2Bgn(const SgMJD& t)                    {t2Bgn_ = t;};
  inline void setT2End(const SgMJD& t)                    {t2End_ = t;};


  inline QMap<QString, AutomaticProcessing>& apByNetId()  {return apByNetId_;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  

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
  QString                       name_;
  // eligible observations:
  int                           qualityCodeThreshold_;
  bool                          useQualityCodeG_;                     // tmp?
  bool                          useQualityCodeH_;                     // tmp?
  int                           goodQualityCodeAtStartup_;
  bool                          useGoodQualityCodeAtStartup_;
  // observables:
  VlbiDataType                  dataType_;
  VlbiDelayType                 useDelayType_;
  VlbiRateType                  useRateType_;
  int                           activeBandIdx_;
  
  // Compatibility:
  bool                          isSolveCompatible_;
  bool                          useDynamicClockBreaks_;
  bool                          useSolveObsSuppresionFlags_;

  // Reweighting stuff:
  bool                          doWeightCorrection_;
  bool                          useExternalWeights_;
  WeightCorrectionMode          wcMode_;
  QString                       extWeightsFileName_;
  double                        minAuxSigma4Delay_;
  double                        minAuxSigma4Rate_;
  double                        initAuxSigma4Delay_;
  double                        initAuxSigma4Rate_;
  
  // Estimator:
  EstimatorPwlMode              epmPwlMode_;
  bool                          doDownWeight_;

  // Outliers processing:
  OutliersProcessingMode        opMode_;
  OutliersProcessingAction      opAction_;
  double                        opThreshold_;
  int                           opIterationsLimit_;
  bool                          opHave2SuppressWeightCorrection_;
  bool                          opIsSolveCompatible_;
  bool                          opHave2NormalizeResiduals_;

  // Options for preprocessing:
  bool                          doIonoCorrection4SBD_;
  bool                          doClockBreakDetectionMode1_;
  bool                          doOutliersEliminationMode1_;

  // GUI behavior:
  bool                          isActiveBandFollowsTab_;
  bool                          isObservableFollowsPlot_;
  
  // inclusion a priori data from external files:
  bool                          useExtAPrioriSitesPositions_;
  bool                          useExtAPrioriSitesVelocities_;
  bool                          useExtAPrioriSourcesPositions_;
  bool                          useExtAPrioriSourceSsm_;
  bool                          useExtAPrioriErp_;
  bool                          useExtAPrioriAxisOffsets_;
  bool                          useExtAPrioriHiFyErp_;
  bool                          useExtAPrioriMeanGradients_;
  QString                       extAPrioriSitesPositionsFileName_;
  QString                       extAPrioriSitesVelocitiesFileName_;
  QString                       extAPrioriSourcesPositionsFileName_;
  QString                       extAPrioriSourceSsmFileName_;
  QString                       extAPrioriErpFileName_;
  QString                       extAPrioriAxisOffsetsFileName_;
  QString                       extAPrioriHiFyErpFileName_;
  QString                       extAPrioriMeanGradientsFileName_;
  QString                       eccentricitiesFileName_;

  // contributions:
  // obs dependent:
  bool                          contribsAreFromDatabase_;
  bool                          have2ApplyPxContrib_;
  bool                          have2ApplyPyContrib_;
  bool                          have2ApplyEarthTideContrib_;
  bool                          have2ApplyOceanTideContrib_;
  bool                          have2ApplyPoleTideContrib_;
  bool                          have2ApplyUt1OceanTideHFContrib_; 
  bool                          have2ApplyPxyOceanTideHFContrib_;
  bool                          have2ApplyNutationHFContrib_;
  bool                          have2ApplyUt1LibrationContrib_;
  bool                          have2ApplyPxyLibrationContrib_;
  bool                          have2ApplyOceanPoleTideContrib_;
  bool                          have2ApplyGpsIonoContrib_;
  bool                          have2ApplySourceSsm_;
  
  bool                          have2ApplyFeedCorrContrib_;
  bool                          have2ApplyTiltRemvrContrib_;
  bool                          have2ApplyOldOceanTideContrib_;
  bool                          have2ApplyOldPoleTideContrib_;
  // station dependent?
  bool                          have2ApplyAxisOffsetContrib_;
  bool                          have2ApplyUnPhaseCalContrib_;
  //
  bool                          isIonosphereFreeUncorrelated_;
  bool                          useOldMode4IonosphereSigma_;
  
  // flybies:
  CableCalSource                flybyCableCalSource_;
  
  // models (mix of contributions and flybies):
  RefractionModel               refractionModel_;

  
  // novice user mode:
  bool                          isNoviceUser_;
  //
  bool                          have2outputCovarMatrix_;
  // test:
  bool                          isTesting_;
  QMap<QString, AutomaticProcessing>
                                apByNetId_;
  QString                       lastModifiedNetId_;
  // 
  SgMJD                         t2Bgn_;
  SgMJD                         t2End_;
};
/*=====================================================================================================*/







/*=====================================================================================================*/
/*                                                                                                     */
/* SgTaskConfig inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// A copying constructor:
inline SgTaskConfig::SgTaskConfig(const SgTaskConfig& cfg) :
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
  *this = cfg;
};



// A destructor:
inline SgTaskConfig::~SgTaskConfig()
{
  // nothing to do
};



//
// INTERFACES:
//
//
// gets:
//
inline const QString& SgTaskConfig::getName() const
{
  return name_;
};



//
inline int SgTaskConfig::getQualityCodeThreshold() const
{
  return qualityCodeThreshold_;
};



//
inline bool SgTaskConfig::getUseQualityCodeG() const
{
  return useQualityCodeG_;
};



//
inline bool SgTaskConfig::getUseQualityCodeH() const
{
  return useQualityCodeH_;
};



//
inline int SgTaskConfig::getGoodQualityCodeAtStartup() const
{
  return goodQualityCodeAtStartup_;
}



//
inline bool SgTaskConfig::getUseGoodQualityCodeAtStartup() const
{
  return useGoodQualityCodeAtStartup_;
};



//
inline SgTaskConfig::VlbiDelayType SgTaskConfig::getUseDelayType() const
{
  return useDelayType_;
};



//
inline SgTaskConfig::VlbiRateType SgTaskConfig::getUseRateType() const
{
  return useRateType_;
};



//
inline int SgTaskConfig::getActiveBandIdx() const
{
  return activeBandIdx_;
};



//
inline bool SgTaskConfig::getDoIonoCorrection4SBD() const
{
  return doIonoCorrection4SBD_;
};



//
inline bool SgTaskConfig::getDoWeightCorrection() const
{
  return doWeightCorrection_;
};



//
inline bool SgTaskConfig::getIsSolveCompatible() const
{
  return isSolveCompatible_;
};



//
inline bool SgTaskConfig::getUseDynamicClockBreaks() const
{
  return useDynamicClockBreaks_;
};



//
inline bool SgTaskConfig::getUseSolveObsSuppresionFlags() const
{
  return useSolveObsSuppresionFlags_;
};



//
inline bool SgTaskConfig::getUseExternalWeights() const
{
  return useExternalWeights_;
};



//
inline SgTaskConfig::WeightCorrectionMode SgTaskConfig::getWcMode() const
{
  return wcMode_;
};



//
inline SgTaskConfig::EstimatorPwlMode SgTaskConfig::getPwlMode() const
{
  return epmPwlMode_;
};



//
inline bool SgTaskConfig::getDoDownWeight() const
{
  return doDownWeight_;
};



//
inline const QString& SgTaskConfig::getExtWeightsFileName() const
{
  return extWeightsFileName_;
};



//
inline SgTaskConfig::OutliersProcessingMode SgTaskConfig::getOpMode() const
{
  return opMode_;
};



//
inline SgTaskConfig::OutliersProcessingAction SgTaskConfig::getOpAction() const
{
  return opAction_;
};



//
inline double SgTaskConfig::getOpThreshold() const
{
  return opThreshold_;
};



//
inline int SgTaskConfig::getOpIterationsLimit() const
{
  return opIterationsLimit_;
};



//
inline bool SgTaskConfig::getOpHave2SuppressWeightCorrection() const
{
  return opHave2SuppressWeightCorrection_;
};



//
inline bool SgTaskConfig::getOpIsSolveCompatible() const
{
  return opIsSolveCompatible_;
};



//
inline bool SgTaskConfig::getOpHave2NormalizeResiduals() const
{
  return opHave2NormalizeResiduals_;
};



//
inline bool SgTaskConfig::getDoClockBreakDetectionMode1() const
{
  return doClockBreakDetectionMode1_;
};



//
inline bool SgTaskConfig::getDoOutliersEliminationMode1() const
{
  return doOutliersEliminationMode1_;
};



//
inline bool SgTaskConfig::getIsActiveBandFollowsTab() const
{
  return isActiveBandFollowsTab_;
};



//
inline bool SgTaskConfig::getIsObservableFollowsPlot() const
{
  return isObservableFollowsPlot_;
};



//
inline bool SgTaskConfig::getIsTesting() const
{
  return isTesting_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriSitesPositions() const
{
  return useExtAPrioriSitesPositions_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriSitesVelocities() const
{
  return useExtAPrioriSitesVelocities_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriSourcesPositions() const
{
  return useExtAPrioriSourcesPositions_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriSourceSsm() const
{
  return useExtAPrioriSourceSsm_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriErp() const
{
  return useExtAPrioriErp_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriAxisOffsets() const
{
  return useExtAPrioriAxisOffsets_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriHiFyErp() const
{
  return useExtAPrioriHiFyErp_;
};



//
inline bool SgTaskConfig::getUseExtAPrioriMeanGradients() const
{
  return useExtAPrioriMeanGradients_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriSitesPositionsFileName() const
{
  return extAPrioriSitesPositionsFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriSitesVelocitiesFileName() const
{
  return extAPrioriSitesVelocitiesFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriSourcesPositionsFileName() const
{
  return extAPrioriSourcesPositionsFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriSourceSsmFileName() const
{
  return extAPrioriSourceSsmFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriErpFileName() const
{
  return extAPrioriErpFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriAxisOffsetsFileName() const
{
  return extAPrioriAxisOffsetsFileName_;
};




//
inline const QString& SgTaskConfig::getExtAPrioriHiFyErpFileName() const
{
  return extAPrioriHiFyErpFileName_;
};



//
inline const QString& SgTaskConfig::getExtAPrioriMeanGradientsFileName() const
{
  return extAPrioriMeanGradientsFileName_;
};



//
inline const QString& SgTaskConfig::getEccentricitiesFileName() const
{
  return eccentricitiesFileName_;
};



//
// sets:
//
inline void SgTaskConfig::setName(const QString& name)
{
  name_ = name;
};



//
inline void SgTaskConfig::setQualityCodeThreshold(int q)
{
  qualityCodeThreshold_ = q;
};



//
inline void SgTaskConfig::setUseQualityCodeG(bool use)
{
  useQualityCodeG_ = use;
};



//
inline void SgTaskConfig::setUseQualityCodeH(bool use)
{
  useQualityCodeH_ = use;
};



//
inline void SgTaskConfig::setGoodQualityCodeAtStartup(int qc)
{
  goodQualityCodeAtStartup_ = qc;
};



//
inline void SgTaskConfig::setUseGoodQualityCodeAtStartup(bool use)
{
  useGoodQualityCodeAtStartup_ = use;
};



//
inline void SgTaskConfig::setUseDelayType(VlbiDelayType type)
{
  useDelayType_ = type;
};



//
inline void SgTaskConfig::setUseRateType(VlbiRateType type)
{
  useRateType_ = type;
};



//
inline void SgTaskConfig::setActiveBandIdx(int idx)
{
  activeBandIdx_ = idx;
};



//
inline void SgTaskConfig::setDoIonoCorrection4SBD(bool doAction)
{
  doIonoCorrection4SBD_ = doAction;
};



//
inline void SgTaskConfig::setDoWeightCorrection(bool doAction)
{
  doWeightCorrection_ = doAction;
};



//
inline void SgTaskConfig::setIsSolveCompatible(bool is)
{
  isSolveCompatible_ = is;
};



//
inline void SgTaskConfig::setUseDynamicClockBreaks(bool use)
{
  useDynamicClockBreaks_ = use;
};



//
inline void SgTaskConfig::setUseSolveObsSuppresionFlags(bool use)
{
  useSolveObsSuppresionFlags_ = use;
};



//
inline void SgTaskConfig::setUseExternalWeights(bool have2use)
{
  useExternalWeights_ = have2use;
};



//
inline void SgTaskConfig::setWcMode(WeightCorrectionMode mode)
{
  wcMode_ = mode;
};



//
inline void SgTaskConfig::setPwlMode(EstimatorPwlMode mode)
{
  epmPwlMode_ = mode;
};



//
inline void SgTaskConfig::setDoDownWeight(bool doIt)
{
  doDownWeight_ = doIt;
};



//
inline void SgTaskConfig::setExtWeightsFileName(const QString& fileName)
{
  extWeightsFileName_ = fileName;
};



//
inline void SgTaskConfig::setOpMode(SgTaskConfig::OutliersProcessingMode m)
{
  opMode_ = m;
};



//
inline void SgTaskConfig::setOpAction(OutliersProcessingAction a)
{
  opAction_ = a;
};



//
inline void SgTaskConfig::setOpThreshold(double d)
{
  opThreshold_ = d;
};



//
inline void SgTaskConfig::setOpIterationsLimit(int limit)
{
  opIterationsLimit_ = limit;
};



//
inline void SgTaskConfig::setOpHave2SuppressWeightCorrection(bool doIt)
{
  opHave2SuppressWeightCorrection_ = doIt;
};



//
inline void SgTaskConfig::setOpIsSolveCompatible(bool is)
{
  opIsSolveCompatible_ = is;
};



//
inline void SgTaskConfig::setOpHave2NormalizeResiduals(bool have2)
{
  opHave2NormalizeResiduals_ = have2;
};



//
inline void SgTaskConfig::setDoClockBreakDetectionMode1(bool doIt)
{
  doClockBreakDetectionMode1_ = doIt;
};



//
inline void SgTaskConfig::setDoOutliersEliminationMode1(bool doAction)
{
  doOutliersEliminationMode1_ = doAction;
};



//
inline void SgTaskConfig::setIsActiveBandFollowsTab(bool is)
{
  isActiveBandFollowsTab_ = is;
};



//
inline void SgTaskConfig::setIsObservableFollowsPlot(bool is)
{
  isObservableFollowsPlot_ = is;
};



//
inline void SgTaskConfig::setIsTesting(bool is)
{
  isTesting_ = is;
};



//
inline void SgTaskConfig::setUseExtAPrioriSitesPositions(bool use)
{
  useExtAPrioriSitesPositions_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriSitesVelocities(bool use)
{
  useExtAPrioriSitesVelocities_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriSourcesPositions(bool use)
{
  useExtAPrioriSourcesPositions_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriSourceSsm(bool use)
{
  useExtAPrioriSourceSsm_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriErp(bool use)
{
  useExtAPrioriErp_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriAxisOffsets(bool use)
{
  useExtAPrioriAxisOffsets_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriHiFyErp(bool use)
{
  useExtAPrioriHiFyErp_ = use;
};



//
inline void SgTaskConfig::setUseExtAPrioriMeanGradients(bool use)
{
  useExtAPrioriMeanGradients_ = use;
};



//
inline void SgTaskConfig::setExtAPrioriSitesPositionsFileName(const QString& s)
{
  extAPrioriSitesPositionsFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriSitesVelocitiesFileName(const QString& s)
{
  extAPrioriSitesVelocitiesFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriSourcesPositionsFileName(const QString& s)
{
  extAPrioriSourcesPositionsFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriSourceSsmFileName(const QString& s)
{
  extAPrioriSourceSsmFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriErpFileName(const QString& s)
{
  extAPrioriErpFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriAxisOffsetsFileName(const QString& s)
{
  extAPrioriAxisOffsetsFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriHiFyErpFileName(const QString& s)
{
  extAPrioriHiFyErpFileName_ = s;
};



//
inline void SgTaskConfig::setExtAPrioriMeanGradientsFileName(const QString& s)
{
  extAPrioriMeanGradientsFileName_ = s;
};



//
inline void SgTaskConfig::setEccentricitiesFileName(const QString& s)
{
  eccentricitiesFileName_ = s;
};



//
// FRUNCTIONS:
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
#endif //SG_TASK_CONFIG_H
