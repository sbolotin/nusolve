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

#ifndef SG_VLBI_SESSION_H
#define SG_VLBI_SESSION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

class QDataStream;

#include <QtCore/QMap>
#include <QtCore/QList>


#include <SgVlbiSessionInfo.h>

#include <SgAPrioriData.h>
#include <SgConstants.h>
#include <SgExternalErpFile.h>
#include <SgExternalTrp.h>
#include <SgExternalWeights.h>
#include <SgModelEop_JMG_96_hf.h>
#include <SgModelsInfo.h>
#include <SgParametersDescriptor.h>
#include <SgSolutionReporter.h>
#include <SgTaskConfig.h>
#include <SgTidalUt1.h>
#include <SgVlbiBaselineInfo.h>
#include <SgVlbiHistory.h>
#include <SgVlbiSourceInfo.h>
#include <SgVlbiStationInfo.h>

class SgVlbiObservable;



// temporary?:
class SgClockBreakAgent
{
public:
  SgMJD                         epoch_;
  SgVlbiStationInfo            *stn_;
  double                        shift_;
  double                        sigma_;
  SgParameter                  *pC_;
  SgClockBreakAgent() : epoch_(tZero) {stn_=NULL; shift_=0.0; sigma_=0.0; pC_=NULL;};
  ~SgClockBreakAgent() {stn_=NULL; shift_=0.0; sigma_=0.0; pC_=NULL;};
};





class SgCubicSpline;
class SgDbhImage;
class SgIdentities;
class SgIoDriver;
class SgLogger;
class SgNetworkStations;
class SgVgosDb;
//class SgSolutionReporter;
class SgVersion;
class SgVlbiBand;
class SgVlbiObservation;
class SgVlbiSourceInfo;


//
// for internal use:
class StationInfo;
class VexInfo;

//
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgVlbiSession : public SgVlbiSessionInfo
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static QString className();

  // SgVlbiSessionIoSupport.cpp:
  //
  static bool guessWrapperFileNameBySession(const QString& inputArg, const QString& path2VgosDb,
    const QString& acAbbName, bool useOwnOnly,
    QString& path2wrapperFile, QString& wrapperFileName, QString& sessionName, int& version,
    bool noYears=false);
  //
  static bool guessSessionByWrapperFileName(const QString& inputArg, const QString& path2VgosDb,
    QString& path2wrapperFile, QString& wrapperFileName, QString& sessionName, int& version);

  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgVlbiSession();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgVlbiSession();



  //
  // Interfaces:
  //
  // gets:
  /**Returns a pointer on a current config.
   */
  inline SgTaskConfig* getConfig();

  /**Returns a pointer on a parameter descriptor.
   */
  inline SgParametersDescriptor* getParametersDescriptor();

  /**Returns a pointer on a reporter.
   */
  inline SgSolutionReporter* getReporter();
  inline int getNumOfParameters() const {return numOfParameters_;};
  inline int getNumOfConstraints() const {return numOfConstraints_;};
  inline int getNumOfDOF() const {return numOfDOF_;};
  
  
  /**Returns a const reference on a container of external a priori station positions.
   */
  inline const SgAPriories& getApStationPositions() const;
  
  /**Returns a const reference on a container of external a priori station velocities.
   */
  inline const SgAPriories& getApStationVelocities() const;
  
  /**Returns a const reference on a container of external a priori source positions.
   */
  inline const SgAPriories& getApSourcePositions() const;
  
  /**Returns a const reference on a container of external a priori source positions.
   */
  inline const SgAPriories& getApSourceStrModel() const;
  
  /**Returns a const reference on a container of external a priori axis offsets.
   */
  inline const SgAPriories& getApAxisOffsets() const;

  /**Returns a const reference on a container of external a priori mean site gradients.
   */
  inline const SgAPriories& getApStationGradients() const;

  /**Returns a const reference on a hi freq variations EOP model.
   */
  inline const SgModelEop_JMG_96_hf& getApHiFyEop() const;
  
  inline const SgExternalTrp& externalTrpFile() const {return externalTrpFile_;};
//  inline double getCalcVersionValue() const {return calcVersionValue_;};

  inline bool getHasUt1Interpolation() const;
  inline void setHasUt1Interpolation(bool has);
  
  inline bool getHasPxyInterpolation() const;
  inline void setHasPxyInterpolation(bool has);

  inline bool isAble2InterpolateErp() const;
  inline SgTidalUt1::UT1TideContentType getTabsUt1Type() const {return tabsUt1Type_;};
  inline void setTabsUt1Type(SgTidalUt1::UT1TideContentType tp) {tabsUt1Type_ = tp;};

  inline const QVector<bool>& getUserCorrectionsUse() const {return userCorrectionsUse_;};
  inline QVector<bool>& userCorrectionsUse() {return userCorrectionsUse_;};
  inline const QVector<QString>& getUserCorrectionsName() const {return userCorrectionsName_;};
  inline QVector<QString>& userCorrectionsName() {return userCorrectionsName_;};
  
  
  /**
   */
  inline bool hasOceanTideContrib()       const {return hasOceanTideContrib_;};
  inline bool hasPoleTideContrib()        const {return hasPoleTideContrib_;};
  inline bool hasEarthTideContrib()       const {return hasEarthTideContrib_;};
  inline bool hasWobblePxContrib()        const {return hasWobblePxContrib_;};
  inline bool hasWobblePyContrib()        const {return hasWobblePyContrib_;};
  inline bool hasWobbleNutContrib()       const {return hasWobbleNutContrib_;};
  inline bool hasWobbleHighFreqContrib()  const {return hasWobbleHighFreqContrib_;};
  inline bool hasUt1HighFreqContrib()     const {return hasUt1HighFreqContrib_;};
  inline bool hasFeedCorrContrib()        const {return hasFeedCorrContrib_;};
  inline bool hasTiltRemvrContrib()       const {return hasTiltRemvrContrib_;};
  inline bool hasPxyLibrationContrib()    const {return hasPxyLibrationContrib_;};
  inline bool hasUt1LibrationContrib()    const {return hasUt1LibrationContrib_;};
  inline bool hasOceanPoleTideContrib()   const {return hasOceanPoleTideContrib_;};
  inline bool hasGpsIonoContrib()         const {return hasGpsIonoContrib_;};
  inline bool hasPoleTideOldContrib()     const {return hasPoleTideOldContrib_;};
  inline bool hasOceanTideOldContrib()    const {return hasOceanTideOldContrib_;};
  inline bool hasNdryContrib()            const {return hasNdryContrib_;};
  inline bool hasNwetContrib()            const {return hasNwetContrib_;};
  inline bool hasUnPhaseCalContrib()      const {return hasUnPhaseCalContrib_;};
  inline bool hasGradPartials()           const {return hasGradPartials_;};
  inline bool hasCipPartials()            const {return hasCipPartials_;};
  
  //
  inline SgVlbiHistory& contemporaryHistory() {return contemporaryHistory_;};
  inline const SgVlbiHistory& contemporaryHistory() const {return contemporaryHistory_;};

  inline const SgIoDriver* getInputDriver() const {return inputDriver_;};

  // temporary:
  inline double dUt1Value() const {return reporter_?reporter_->dUt1Value():0.0;};
  inline double dUt1Correction() const {return reporter_?reporter_->dUt1Correction():0.0;};
  inline double dUt1StdDev() const {return reporter_?reporter_->dUt1StdDev():0.0;};
  
  inline int getExpectedMasterfileVersion() const {return expectedMasterfileVersion_;};
  inline const SgMJD& getLastProcessed() const {return lastProcessed_;};

  inline const SgSidebandOrder getSidebandOrder() const {return sidebandOrder_;};
  
  
  //
  // sets:
  /**Set up a pointer on a current config.
   * \parameter cfg -- a config.
   */
  inline void setConfig(SgTaskConfig* cfg);

  /**Set up a pointer on a reporter.
   * \parameter reporter -- a reporter.
   */
  inline void setReporter(SgSolutionReporter* reporter);

  inline void setNumOfParameters(int n) {numOfParameters_ = n;};
  inline void setNumOfConstraints(int n) {numOfConstraints_ = n;};
  inline void setNumOfDOF(int n) {numOfDOF_ = n;};


  /**Set up a pointer on a parameter descriptor.
   * \parameter parametersDescriptor -- a descriptor.
   */
  inline void setParametersDescriptor(SgParametersDescriptor* parametersDescriptor);

  /**Returns band container.
   */
  inline QList<SgVlbiBand*>& bands();
  inline const QList<SgVlbiBand*>& bands() const {return bands_;};
  inline QMap<QString, SgVlbiBand*>& bandByKey() {return bandByKey_;};

  /**Returns a pointer on a reference band.
   */
  inline SgVlbiBand* primaryBand();
  inline const SgVlbiBand* primaryBand() const {return primaryBand_;};

  /**Returns observations container.
   */
  inline QList<SgVlbiObservation*>& observations();

  inline QMap<QString, SgVlbiObservation*>& observationByKey() {return observationByKey_;};

  
  /**Returns observations container.
   */
  inline const QList<SgVlbiObservation*>& observations() const {return observations_;};

  /**Returns a list of scan epochs.
   */
  inline const QList<SgMJD*>& scanEpochs() const {return scanEpochs_;};
  
  /**Sets a pointer on a reference band by band's index.
   */
  void setPrimaryBandByIdx(int);

  /**Returns a const reference on set (actually, a map: name => station info) of stations info.
   */
  inline QMap<QString, SgVlbiStationInfo*>& stationsByName();
  inline const QMap<QString, SgVlbiStationInfo*>& stationsByName() const {return stationsByName_;};

  /**Returns a const reference on set (actually, a map: name => baseline info) of baselines info.
   */
  inline QMap<QString, SgVlbiBaselineInfo*>& baselinesByName();
  inline const QMap<QString, SgVlbiBaselineInfo*>& baselinesByName() const {return baselinesByName_;};

  /**Returns a const reference on set (actually, a map: name => source info) of sources info.
   */
  inline QMap<QString, SgVlbiSourceInfo*>& sourcesByName();
  inline const QMap<QString, SgVlbiSourceInfo*>& sourcesByName() const {return sourcesByName_;};
  
  //
  inline QMap<int, SgVlbiStationInfo*>& stationsByIdx() {return stationsByIdx_;}
  inline QMap<int, SgVlbiBaselineInfo*>& baselinesByIdx() {return baselinesByIdx_;}
  inline QMap<int, SgVlbiSourceInfo*>& sourcesByIdx() {return sourcesByIdx_;}
  
  
  inline void setPath2Masterfile(const QString&);

  inline void setPath2APrioriFiles(const QString&);

  inline void setPath2TrpFiles(const QString& str) {path2TrpFiles_=str;};

  inline QMap<QString, double>& skyFreqByIfId() {return skyFreqByIfId_;};
  inline const QMap<QString, double>& skyFreqByIfId() const {return skyFreqByIfId_;};

  // temporary:
  /**Returns a pointer on the parameter: polar motion, x-coordiante.
   */
  inline SgParameter* pPolusX();

  /**Returns a pointer on the parameter: polar motion, y-coordiante.
   */
  inline SgParameter* pPolusY();

  /**Returns a pointer on the parameter: polar motion, d(UT1-UTC).
   */
  inline SgParameter* pUT1();

  /**Returns a pointer on the parameter: polar motion, d(UT1-UTC) rate.
   */
  inline SgParameter* pUT1Rate();

  /**Returns a pointer on the parameter: polar motion, x-coordinate rate.
   */
  inline SgParameter* pPolusXRate();

  /**Returns a pointer on the parameter: polar motion, y-coordinate rate.
   */
  inline SgParameter* pPolusYRate();

  /**Returns a pointer on the parameter: nutation angle, x-coordiante.
   */
  inline SgParameter* pNutX();

  inline SgParameter* pNutXRate();

  /**Returns a pointer on the parameter: nutation angle, y-coordiante.
   */
  inline SgParameter* pNutY();

  inline SgParameter* pNutYRate();

  inline double getLeapSeconds() {return leapSeconds_;};

  inline void setLeapSeconds(double s) {leapSeconds_ = s;};

  inline void setHave2InteractWithGui(bool have2) {have2InteractWithGui_ = have2;};

  inline void setLongOperationStart(void (*operation)(int, int, const QString&)) 
    {longOperationStart_ = operation;};

  inline void setLongOperationProgress(void (*operation)(int)) {longOperationProgress_ = operation;};

  inline void setLongOperationStop(void (*operation)()) {longOperationStop_ = operation;};
  
  inline void setLongOperationMessage(void (*operation)(const QString&)) 
    {longOperationMessage_ = operation;};
  inline void setLongOperationShowStats(void (*operation)
    (const QString& label, int numTot, int numUsb, int numPrc, double wrms, double chi2))
    {longOperationShowStats_ = operation;};

//  inline void setCalcVersionValue(double v) {calcVersionValue_ = v;};

  inline void setInputDriver(SgIoDriver* drv) {inputDriver_ = drv;};
  
  inline SgModelsInfo& calcInfo() {return calcInfo_;};

  inline SgVector*& args4Ut1Interpolation() {return args4Ut1Interpolation_;};
  inline SgMatrix*& tabs4Ut1Interpolation() {return tabs4Ut1Interpolation_;};
  inline SgVector*& args4PxyInterpolation() {return args4PxyInterpolation_;};
  inline SgMatrix*& tabs4PxyInterpolation() {return tabs4PxyInterpolation_;};

  inline void setExpectedMasterfileVersion(int v) {expectedMasterfileVersion_ = v;};
  inline void setLastProcessed(const SgMJD& t) {lastProcessed_ = t;};

  inline void setSidebandOrder(SgSidebandOrder order) {sidebandOrder_ = order;};

  //
  // Functions:
  //
  inline const SgMJD& tRefer() const {return tRefer_;};
  
  /** Returns number of bands.
   */
  int numberOfBands() const;
  
  /** Makes import of data from DBH image, returns true if everything is ok.
   */
  bool getDataFromDbhImages(QMap<QString, SgDbhImage*> &images, 
    bool have2LoadImmatureSession, bool guiExpected, const QStringList& masterfileSuffixes);

  /** Makes import of data from DBH image, returns true if everything is ok.
   */
  bool getDataFromDbhImage(SgDbhImage *image, 
    bool have2getAll, bool isStandardSession, bool have2LoadImmatureSession, bool guiExpected);

  /** Exports data into DBH image, returns true if everything is ok.
   */
  bool putDataIntoDbhImage(SgDbhImage *image);
  
  /** Exports data into NGS file, returns true if everything is ok.
   */
  bool exportDataIntoNgsFile(const QString& fileName);

  /** Imports data from VgosDb file tree, returns true if everything is ok.
   */
  bool getDataFromVgosDb(SgVgosDb* vgosDb, bool have2LoadImmatureSession, bool guiExpected,
    const QStringList& masterfileSuffixes);

  /** Saves data into VgosDb file tree, returns true if everything is ok.
   */
  bool putDataIntoVgosDb(SgVgosDb* vgosDb);

  /** Imports data from AGV file, returns true if everything is ok.
   */
  bool getDataFromAgvFile(const QString& fileName, SgIdentities *ids, SgVersion *driverVersion, 
    const QStringList& masterfileSuffixes);

  /** Saves data into AGV file, returns true if everything is ok.
   */
  bool putDataIntoAgvFile(const QString& dirName, const QString& fileName, 
    SgIdentities *ids, SgVersion *driverVersion);

  /** Imports data from fringe files, returns true if everything is ok.
   */
  bool getDataFromFringeFiles(const QString& path2, const QString& altDatabaseName, 
    const QString& altCorrelatorName, const QString& historyFileName, const QString& mapFileName,
    const QList<QString>&, const QStringList& masterfileSuffixes, bool acceptAllRootFiles);

  /** Imports data from KOMB files, returns true if everything is ok.
   */
  bool getDataFromKombFiles(const QString& path2, const QString& altDatabaseName,
    const QString& altCorrelatorName, const QString& historyFileName, const QString& mapFileName,
    bool need2correctRefClocks, const QStringList& masterfileSuffixes);

  /** Make check for consistency after loading data.
   */
  bool selfCheck(bool guiExpected, const QStringList& masterfileSuffixes);
  
  
  /** Returns stationInfo object for the index (if exists).
   */
  inline SgVlbiStationInfo* lookupStationByIdx(int);

  /** Returns sourceInfo object for the index (if exists).
   */
  inline SgVlbiSourceInfo* lookupSourceByIdx(int) const;

  /** Returns baselineInfo object for the index (if exists).
   */
  inline SgVlbiBaselineInfo* lookupBaselineByIdx(int) const;

  /**Performs necessary manipulations before starting data analysis.
   */
  void prepare4Analysis();

  // temporary:
  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters();

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();

  /**Restores all parameters that can be altered by a user to theirs default state.
   */
  void resetAllEditings(bool have2resetConfig);

  /**Evaluates ionospheric corrections.
   */
  void calculateIonoCorrections(const SgTaskConfig*);

  /**Sets ionospheric corrections to zeros.
   */
  void zerofyIonoCorrections(const SgTaskConfig*);

  /**Evaluates clocks function #1 (supplementary).
   */
  void calculateBaselineClock_F1();

  /**Evaluates a parameter of a clock break for a given epoch and station.
   */
  void calculateClockBreakParameter(const SgMJD&, SgVlbiStationInfo*, SgVlbiBand*, double&, double&);
  
  /**Evaluates parameters of a clock breaks for a given list of epochs and stations.
   */
  void calculateClockBreaksParameters(QList<SgClockBreakAgent*>&, SgVlbiBand*);
  
  /**Attempts to detect a clock break, return true in success.
   */
  bool detectClockBreaks_mode1(SgVlbiBand*, QString&, SgMJD&, QMultiMap<QString, SgMJD>&);
  bool detectClockBreaks_mode2(SgVlbiBand*, QString&, SgMJD&, QMultiMap<QString, SgMJD>&);

  /**Checks particular band for clock breaks, correct them if detected.
   */
  int checkBandForClockBreaks_part1(int, bool);
  int checkBandForClockBreaks_part2(int, bool);

  /**Checks whole session for clock breaks and correct them if detected.
   */
  void detectAndProcessClockBreaks();

  /**Checks for outliers, marks them for omitting in data analysis.
   */
  void eliminateOutliersMode1();

  /**Checks a band for outliers, marks them for skipping in data analysis.
   * \param bandIdx -- an index of a band;
   * \param maxNumOfPasses -- maximal number of passes;
   * \param threshold -- a multiplier of sigma, i.e., 3.0 for 3*sigma.
   */
  void eliminateOutliersSimpleMode(int bandIdx, int maxNumOfPasses, double threshold, double=0.0);

  /**Checks a band for outliers, marks them for skipping in data analysis.
   * \param bandIdx -- an index of a band;
   * \param maxNumOfPasses -- maximal number of passes;
   * \param wrmsRatio -- a ration of wrms pre and post removing an outlier.
   */
  void eliminateLargeOutliers(int bandIdx, int maxNumOfPasses, double wrmsRatio);

  /**Checks a band for outliers, marks them for skipping in data analysis.
   * \param bandIdx -- an index of a band;
   * \param maxNumOfPasses -- maximal number of passes;
   */
  int eliminateOutliers(int bandIdx);

  /**Checks a band for outliers, marks them for including in data analysis.
   * \param bandIdx -- an index of a band;
   * \param maxNumOfPasses -- maximal number of passes;
   */
  int restoreOutliers(int bandIdx);

  /** Scans baselines to check ambiguities.
   * \param bandIdx -- an index of a band;
   */
  void scanBaselines4GrDelayAmbiguities(int bandIdx);
  
  void resetDelayAmbiguities(int bandIdx, SgTaskConfig::VlbiDelayType dType);

  /** Makes data analysis of the session (single session).
   */
  void process(bool haveProcessAllBands, bool interactWithGui);

  /** Makes user specified post-read actions.
   */
  void doPostReadActions_old();

  int suppressNotSoGoodObs();

  /** Makes user specified post-read actions:
   */
  bool doPostReadActions();

  // automated processing, case of INT sessions, tests:
  bool doPostReadActions4Ints(const SgTaskConfig::AutomaticProcessing&);
  
  void pickupReferenceClocksStation();
  void setReferenceClocksStation(const QString& stnKey);
  
  void pickupReferenceCoordinatesStation();

  void checkUseOfManualPhaseCals();
  
  void setClockModelOrder4Stations(int);
  
  int doReWeighting();
  
  bool check4ClockBreaksAtSBD();
  
  // end of automatizations
  //
  /** Adds a clock break to the specified station, adjusts residuals
   */
  void correctClockBreak(SgVlbiBand*, SgVlbiStationInfo*, const SgMJD&, double, double, bool, bool=true);

  /** Clears additional sigmas (that are used to normalize Chi2):
   */
  void zerofySigma2add();

//  void reweightByBaselines();
  
  QString name4SirFile(bool);
  
  bool isInUse(SgVlbiBaselineInfo*);
  
  bool isInUse(SgVlbiStationInfo*);
  
  bool need2runAutomaticDataProcessing();

  bool importDataFromLogFiles(const QString& path2sessions, const QList<QString>& stations, 
    const QList<QString>& kinds, 
    const QMap<QString, int>* defaultCableSignByStn,
    const QMap<QString, QString>* rinexFileNameByStn, 
    const QMap<QString, double>* rinexPressureOffsetByStn, 
    bool have2overwriteAntabFile, 
    bool reportAllTsysData,
    const QString& altPath2InputFiles,
    const QString& orderOfMeteo);
  bool resetDataFromLogFiles(bool doCable, bool doMeteo, bool doTsys, const QList<QString>& stations);
  
  void allocUt1InterpolEpochs(double t0, double dt, unsigned int numOfPts);
  void allocPxyInterpolEpochs(double t0, double dt, unsigned int numOfPts);
  void allocUt1InterpolValues(const SgVector* vals);
  void allocPxyInterpolValues(const SgMatrix* vals);

  void getAprioriErp(const SgMJD& t, 
    double& vUt1, double& rUt1, double& vPx, double& rPx, double& vPy, double& rPy,
    double& vCx, double& rCx, double& vCy, double& rCy);

  void setupTimeRefer();

  bool hasReferenceClocksStation();
  
  bool hasReferenceCoordinatesStation();

  bool writeUserData2File(const QString& fileName);
  
  void propagateChannelBandwidth(double bw);

  //
  // Friends:
  //

  // I/O support:
  /**
   */
  void applyObsCalibrationSetup(int bitFlags, const QList<QString> &calList);
  void formObsCalibrationSetup(int& bitFlags, QList<QString> &calList);

  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //
  

private:
  // a list of bands, we are the owner:
  QList<SgVlbiBand*>                    bands_;
  QMap<QString, SgVlbiBand*>            bandByKey_;
  SgVlbiBand                           *primaryBand_;
  // a list of observations, an owner:
  QList<SgVlbiObservation*>             observations_;
  QList<SgMJD*>                         scanEpochs_;
  QMap<QString, SgVlbiObservation*>     observationByKey_;  
  // owners of pointers (the allocated memmory should be released in the destructor):
  StationsByName                        stationsByName_;
  BaselinesByName                       baselinesByName_;
  SourcesByName                         sourcesByName_;
  SgMJD                                 tRefer_;          // a reference epoch for polynomial parameters

  // current configuration:
  SgTaskConfig                         *config_;
  SgTaskConfig                          storedConfig_;
  SgTaskConfig                          lastProcessedConfig_;
  SgParametersDescriptor               *parametersDescriptor_;
  SgParametersDescriptor                storedParametersDescriptor_;

  // just for quick look-ups, not the owners:
  QMap<int, SgVlbiStationInfo*>         stationsByIdx_;
  QMap<int, SgVlbiBaselineInfo*>        baselinesByIdx_;
  QMap<int, SgVlbiSourceInfo*>          sourcesByIdx_;
  
  //
  SgModelsInfo                          calcInfo_;
  // available "contributions" from a database file:
  bool                                  hasOceanTideContrib_;
  bool                                  hasPoleTideContrib_;
  bool                                  hasEarthTideContrib_;
  bool                                  hasWobblePxContrib_;
  bool                                  hasWobblePyContrib_;
  bool                                  hasWobbleNutContrib_;
  bool                                  hasWobbleHighFreqContrib_;
  bool                                  hasUt1HighFreqContrib_;
  bool                                  hasFeedCorrContrib_;
  bool                                  hasTiltRemvrContrib_;
  bool                                  hasPxyLibrationContrib_;
  bool                                  hasUt1LibrationContrib_;
  bool                                  hasOceanPoleTideContrib_;
  bool                                  hasGpsIonoContrib_;
  bool                                  hasPoleTideOldContrib_;
  bool                                  hasOceanTideOldContrib_;
  bool                                  hasNdryContrib_;
  bool                                  hasNwetContrib_;
  bool                                  hasUnPhaseCalContrib_;
  bool                                  hasGradPartials_;
  bool                                  hasCipPartials_;
 
  // temporary place for EOP-estimations:
  SgParameter                          *pPolusX_;
  SgParameter                          *pPolusY_;
  SgParameter                          *pUT1_;
  SgParameter                          *pUT1Rate_;
  SgParameter                          *pPolusXRate_;
  SgParameter                          *pPolusYRate_;
  SgParameter                          *pNutX_;
  SgParameter                          *pNutY_;
  SgParameter                          *pNutXRate_;
  SgParameter                          *pNutYRate_;
  
  // and temporary place for pathes:
  QString                               path2Masterfile_;
  QString                               path2APrioriFiles_;
  QString                               path2TrpFiles_;
  // and apriories:
  SgAPriories                           apStationPositions_;
  SgAPriories                           apStationVelocities_;
  SgAPriories                           apSourcePositions_;
  SgAPriories                           apSourceStrModel_;
  SgAPriories                           apAxisOffsets_;
  SgModelEop_JMG_96_hf                  apHiFyEop_;
  SgAPriories                           apStationGradients_;
  SgExternalWeights                     externalWeights_;
  SgExternalEopFile                     externalErpFile_;
  SgExternalTrp                         externalTrpFile_;
  int                                   numOfPts4ErpInterpolation_;
  // info for ERP interpolation from the database:
  SgVector                             *args4Ut1Interpolation_;
  SgMatrix                             *tabs4Ut1Interpolation_;
  bool                                  hasUt1Interpolation_;
  SgVector                             *args4PxyInterpolation_;
  SgMatrix                             *tabs4PxyInterpolation_;
  bool                                  hasPxyInterpolation_;
  SgTidalUt1::UT1TideContentType        tabsUt1Type_;
  SgCubicSpline                        *innerUt1Interpolator_;
  SgCubicSpline                        *innerPxyInterpolator_;
  SgCubicSpline                        *externalErpInterpolator_;
  bool                                  isAble2InterpolateErp_;

  static const QString                  sSkipCode_;
  
  // reporter:
  SgSolutionReporter                   *reporter_;
  // info on the last solution:
  int                                   numOfParameters_;
  int                                   numOfConstraints_;
  int                                   numOfDOF_;
  
  // aux:
  double                                leapSeconds_; // mimic CALC/SOLVE behavior
  // 
  bool                                  hasOutlierEliminationRun_;
  bool                                  hasOutlierRestoratioRun_;
  //
  SgVlbiHistory                         contemporaryHistory_; // collector for history records
  // input driver:
  SgIoDriver                           *inputDriver_;
  //
  //
  QMap<QString, double>                 skyFreqByIfId_;
  //
  //
  // User corrections:
  QVector<bool>                         userCorrectionsUse_;
  QVector<QString>                      userCorrectionsName_;
  //
  // temporary options:
  int                                   expectedMasterfileVersion_;
  //
  SgMJD                                 lastProcessed_;
  //
  //
  SgSidebandOrder                       sidebandOrder_;


  //                 Functions:
  //
  // interactions with GUI:
  bool                                  have2InteractWithGui_;
  void (*longOperationStart_)(int minStep, int maxStep, const QString& message);
  void (*longOperationProgress_)(int step);
  void (*longOperationStop_)();
  void (*longOperationMessage_)(const QString& message);
  void (*longOperationShowStats_) (const QString& label, int numTot, int numUsb, int numPrc,
    double wrms, double chi2);

  
  // internal functions:
  //
  /** Make check of channels set up.
   */
  bool checkChannelSetup(bool guiExpected);

  /** Calculates mean epoch.
   */
  virtual void calcTMean();

  /** Checks and sets up a primary band.
   */
  virtual void setUpPrimaryBand();

  /** Check for deselected stations.
   */
  void checkExcludedStations();


  /** Collects a priori info from external files.
   */
  void collectAPriories();
  
  /** Consults with external file with weights.
   */
  void lookupExternalWeights();

  /** Makes necessary operations to interpolate ERP.
   */
  void prepare4ErpInterpolation();
  
  /** Fills top part of the "history" part of a database image.
   */
  void makeHistoryIntro(SgVlbiHistory&);

  /** Fills the "history" part of a database image.
   */
  void makeHistory(SgVlbiHistory&);
  
  /**
   */
  void restoreIonCorrections();

  /**
   */
  void parseVexFile(const QString&, QString&, QString&);

  /**
   */
  void processVexFile(const QString&, QMap<QString, StationInfo>&, VexInfo&, 
    const QMap<QString, QString>& stn2stn, QString& correlatorName);

  /**
   */
  void processFringeFile(const QString& path2file, const QString& fringeFileName, 
    const QMap<QString, StationInfo>& stnsInfo, const VexInfo&, const QString& vexFileName,
    const QMap<QString, QString>& stnNameById, const QMap<QString, QString>& stnNameByI, 
    const QMap<QString, QString>& stn2stn, const QMap<QString, QString>& src2src,
    const QMap<QString, QString>& bnd2bnd,
    const QMap<QString, int>& fringeErrorCodeByInt, int& expSerialNumber);

  /**
   */
  void processKombFile(const QString& path2file, const QString& kombFileName,
    const QMap<QString, QString>& stn2stn, const QMap<QString, QString>& src2src, 
    const QMap<QString, QString>& bnd2bnd);
    
  /**
   */
  bool getCorrelatorHistory(const QString& fileName);


  /** Reads source and station names maps from a file. If nothing found, the maps are empty.
   *  The method is in SgVlbiSessionIoSupport.cpp.
   */
  void importMapFile(const QString& mapFileName, QMap<QString, QString>& map4Stn, 
    QMap<QString, QString>& map4Src, QMap<QString, QString>& map4Bnd);

  bool check4NameMap(const QMap<QString, QString> &map, QString& name);


  
  //
  void search4missedLogFiles(QMap<QString, QString>& logFileByKey, const SgNetworkStations &ns_codes);
  void stnInpt2Key(const QString& stnNameInpt, SgNetworkStations& ns_codes, QString& key);

  
  // ambig.resolv. support:
  void resolveGrAmbigTriangles(int bandIdx);
  
};
/*=====================================================================================================*/


//
void evaluatePCal4GrpDelay (SgVector&, const SgVector&, int, double&, double&, int, const SgMJD&,
                            const QString&, const QString&);

//
void evaluateEffectiveFreqs(
  // input:
  const SgVector& numOfAccPeriodsByChan_USB, const SgVector& numOfAccPeriodsByChan_LSB,
  const SgVector& refFreqByChan, const SgVector& fringeAmplitudeByChan,
  const SgVector& numOfSamplesByChan_USB, const SgVector& numOfSamplesByChan_LSB,
  const SgVector& channelBandwidth, double refFreq, int numOfChannels,
  // output:
  double& effFreq4GR, double& effFreq4PH, double& effFreq4RT,
  // type of a correlator
  const QString& correlatorType,
  // info about the observation (for Log's purposes) and equalWeightsFlag:
  SgVlbiObservable* o,
  bool useEqualWeights=false,
  bool mimicSignError=false);




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiSession inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// INTERFACES:
//
//
inline SgTaskConfig* SgVlbiSession::getConfig()
{
  return config_;
};



//
inline SgParametersDescriptor* SgVlbiSession::getParametersDescriptor()
{
  return parametersDescriptor_;
};



//
inline SgSolutionReporter* SgVlbiSession::getReporter()
{
  return reporter_;
};



//
inline const SgAPriories& SgVlbiSession::getApStationPositions() const
{
  return apStationPositions_;
};



//  
inline const SgAPriories& SgVlbiSession::getApStationVelocities() const
{
  return apStationVelocities_;
};



//
inline const SgAPriories& SgVlbiSession::getApSourcePositions() const
{
  return apSourcePositions_;
};



//
inline const SgAPriories& SgVlbiSession::getApSourceStrModel() const
{
  return apSourceStrModel_;
};


//
inline const SgAPriories& SgVlbiSession::getApAxisOffsets() const
{
  return apAxisOffsets_;
};



//
inline const SgAPriories& SgVlbiSession::getApStationGradients() const
{
  return apStationGradients_;
};



//
inline const SgModelEop_JMG_96_hf& SgVlbiSession::getApHiFyEop() const
{
  return apHiFyEop_;
};



//
inline bool SgVlbiSession::getHasUt1Interpolation() const
{
  return hasUt1Interpolation_;
};



//
inline void SgVlbiSession::setHasUt1Interpolation(bool has)
{
  hasUt1Interpolation_ = has;
};



//  
inline bool SgVlbiSession::getHasPxyInterpolation() const
{
  return hasPxyInterpolation_;
};



//  
inline void SgVlbiSession::setHasPxyInterpolation(bool has)
{
  hasPxyInterpolation_ = has;
};



//  
inline bool SgVlbiSession::isAble2InterpolateErp() const
{
  return isAble2InterpolateErp_;
};



//
inline void SgVlbiSession::setConfig(SgTaskConfig* cfg)
{
  config_              = cfg;
  lastProcessedConfig_ = *config_;
  storedConfig_        = *config_;
};



//
inline void SgVlbiSession::setReporter(SgSolutionReporter* reporter)
{
  reporter_ = reporter;
};



//
inline void SgVlbiSession::setParametersDescriptor(SgParametersDescriptor* parametersDescriptor)
{
  parametersDescriptor_       = parametersDescriptor;
  storedParametersDescriptor_ = *parametersDescriptor_;
};



//
inline QList<SgVlbiBand*>& SgVlbiSession::bands()
{
  return bands_;
};



//
inline SgVlbiBand* SgVlbiSession::primaryBand()
{
  return primaryBand_;
};



//
inline QList<SgVlbiObservation*>& SgVlbiSession::observations()
{
  return observations_;
};



//
/*
inline QList<SgVlbiMbandObservation*>* SgVlbiSession::compositeObservations()
{
  return &compositeObservations_;
};



//
inline QList<SgVlbiMbandObservation*>* SgVlbiSession::matchedObservations()
{
  return &matchedObservations_;
};
*/


//
inline QMap<QString, SgVlbiStationInfo*>&  SgVlbiSession::stationsByName()
{
  return stationsByName_;
};



//
inline QMap<QString, SgVlbiBaselineInfo*>&  SgVlbiSession::baselinesByName()
{
  return baselinesByName_;
};



//
inline QMap<QString, SgVlbiSourceInfo*>&  SgVlbiSession::sourcesByName()
{
  return sourcesByName_;
};



//
inline SgParameter* SgVlbiSession::pPolusX()
{
  return pPolusX_;
};



//
inline SgParameter* SgVlbiSession::pPolusY()
{
  return pPolusY_;
};



//
inline SgParameter* SgVlbiSession::pUT1()
{
  return pUT1_;
};



//
inline SgParameter* SgVlbiSession::pUT1Rate()
{
  return pUT1Rate_;
};



//
inline SgParameter* SgVlbiSession::pPolusXRate()
{
  return pPolusXRate_;
};



//
inline SgParameter* SgVlbiSession::pPolusYRate()
{
  return pPolusYRate_;
};



//
inline SgParameter* SgVlbiSession::pNutX()
{
  return pNutX_;
};
inline SgParameter* SgVlbiSession::pNutXRate()
{
  return pNutXRate_;
};



//
inline SgParameter* SgVlbiSession::pNutY()
{
  return pNutY_;
};
inline SgParameter* SgVlbiSession::pNutYRate()
{
  return pNutYRate_;
};



//
inline void SgVlbiSession::setPath2Masterfile(const QString& path)
{
  path2Masterfile_ = path;
};



//
inline void SgVlbiSession::setPath2APrioriFiles(const QString& path)
{
  path2APrioriFiles_ = path;
};



//
// FUNCTIONS:
//
//
//
//
inline SgVlbiStationInfo* SgVlbiSession::lookupStationByIdx(int idx)
{
  if (stationsByIdx_.contains(idx))
    return stationsByIdx_.value(idx);
  else
    return NULL;
};



//
inline SgVlbiSourceInfo* SgVlbiSession::lookupSourceByIdx(int idx) const 
{
  if (sourcesByIdx_.contains(idx))
    return sourcesByIdx_.value(idx);
  else
    return NULL;
};



//
inline SgVlbiBaselineInfo* SgVlbiSession::lookupBaselineByIdx(int idx) const
{
  if (baselinesByIdx_.contains(idx))
    return baselinesByIdx_.value(idx);
  else
    return NULL;
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
#endif // SG_VLBI_SESSION_H
