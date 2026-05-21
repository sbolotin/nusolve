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

#ifndef SG_OPEN_DB_H
#define SG_OPEN_DB_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


// NetCDF library:
#include <netcdf.h>


#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QVector>



#include <SgAttribute.h>
#include <SgConstants.h>
#include <SgIoDriver.h>
#include <SgMJD.h>
#include <SgNetCdf.h>
#include <SgVlbiSessionInfo.h>





class QTextStream;
class SgModelsInfo;
class SgVlbiHistory;
class SgVlbiHistoryRecord;
class SgVlbiSession;


/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgVdbVariable
{
public:
  enum Scope
  {
    S_Session,              //! 
    S_Scan,                 //! 
    S_Obs,                  //! 
  };
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgVdbVariable(const QString& stdStub, const QString& stdSubDir);
  
  SgVdbVariable(const SgVdbVariable& v);

  /**A destructor.
   * Destroys the object.
   */
  inline ~SgVdbVariable(){};

  //
  // Interfaces:
  //
  inline const QString& getStub() const {return stub_;};
  inline const QString& getKind() const {return kind_;};
  inline const QString& getInstitution() const {return institution_;};
  inline int getVersion() const {return version_;};
  inline const QString& getBand() const {return band_;};
  inline const QString& getCurrentInstitution() const {return currentInstitution_;};
  inline const QString& getSubDir() const {return subDir_;};
  inline const QString& getFileName() const {return fileName_;};
  inline const QString& getFileName4Output() const {return fileName4Output_;};

  inline void setStub(const QString& str) {stub_=str;};
  inline void setKind(const QString& str) {kind_=str;};
  inline void setInstitution(const QString& str) {institution_=str;};
  inline void setVersion(int n) {version_=n;};
  inline void setBand(const QString& str) {band_=str;};
  inline void setCurrentInstitution(const QString& str) {currentInstitution_=str;};
  inline void setSubDir(const QString& str) {subDir_=str;};
  inline void setFileName(const QString& str) {fileName_=str;};
  inline void setFileName4Output(const QString& str) {fileName4Output_=str;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**
   */
  QString compositeName() const;

  /**
   */
  void parseString(const QString&);

  /**
   */
  void empty() {stub_ = "";};

  /**
   */
  bool isEmpty() const;

  /**
   */
  QString name4export(const QString& rootDir, SgNetCdf::OperationMode om, const QString& aBand="");
   
  //
  // Friends:
  //

  //
  // I/O:
  //

private:
  QString                       stub_;
  QString                       type_;
  QString                       band_;
  QString                       kind_;
  QString                       institution_;
  int                           version_;
  QString                       stdStub_;
  QString                       currentInstitution_;
  //
  QString                       subDir_;
  QString                       fileName_;
  QString                       stdSubDir_;
  QString                       fileName4Output_;
  bool                          have2adjustPermissions_;
};
/*=====================================================================================================*/









/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgVgosDb : public SgIoDriver
{
  // "Mind is a battlefield
  // All hope is gone"
  //
public:
  enum CorrelatorType
  {
    CT_Unknown    =    0,       //!
    CT_Mk3        =    1,       //!
    CT_VLBA       =    2,       //!
    CT_CRL        =    3,       //!
    CT_GSI        =    4,       //!
    CT_S2         =    5,       //!
    CT_Mk4        =    6,       //!
    CT_Haystack   =    7,       //!
    CT_Difx       =    8,       //!
  };


  class FmtChkVar
  {
  public:
    FmtChkVar(const char* name, nc_type type, bool isMandatory, const QList<int> l, 
      const QString& attLCode="", const QString& attDefinition="", const QString& attUnits="", 
      const QString& attBand="")
        : name_(name), dims_(l)
      {typeOfData_=type; isMandatory_=isMandatory; attLCode_=attLCode; attDefinition_=attDefinition;
        attUnits_=attUnits; attBand_=attBand; have2hideLCode_=false;};
//    FmtChkVar(const FmtChkVar& fcv) : name_(fcv.name_), dims_(fcv.dims_)
//      {typeOfData_=fcv.typeOfData_; isMandatory_=fcv.isMandatory_; attLCode_=fcv.attLCode_; 
//        attDefinition_=fcv.attDefinition_; attUnits_=fcv.attUnits_; attBand_=fcv.attBand_;};
    ~FmtChkVar() {dims_.clear();};
    inline const QString& name() const {return name_;};
    inline nc_type typeOfData() const {return typeOfData_;};
    inline bool isMandatory() const {return isMandatory_;};
    inline bool have2hideLCode() const {return have2hideLCode_;};
    inline const QList<int>& dims() const {return dims_;};
    inline const QString& attLCode() const {return attLCode_;};
    inline const QString& attDefinition() const {return attDefinition_;};
    inline const QString& attUnits() const {return attUnits_;};
    inline const QString& attBand() const {return attBand_;};
    inline void alternateDimension(int i, int num) {dims_[i]=num;};
    inline void addDimension(int num) {dims_<<num;};
    inline void clearDimensions() {dims_.clear();};
    inline void setHave2hideLCode(bool have2) {have2hideLCode_=have2;};
  private:
    const QString               name_;
    nc_type                     typeOfData_;
    bool                        isMandatory_;
    bool                        have2hideLCode_;
    QList<int>                  dims_;
    QString                     attLCode_;
    QString                     attDefinition_;
    QString                     attUnits_;
    QString                     attBand_;
  };
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgVgosDb(SgIdentities *ids, SgVersion *driverVersion);


  /**A destructor.
   * Destroys the object.
   */
  virtual ~SgVgosDb();

  //
  // Interfaces:
  //
  //  inline QMap<QString, StationDescriptor>& stnDescriptorByName() {return stnDescriptorByName_;};
  // gets:


//  inline virtual const SgIdentities* getIdentities() const {return identities_;};
//  inline virtual const SgVersion* getDriverVersion() const {return driverVersion_;};


  /**
   */
  inline const QString& getSessionCode() const {return sessionCode_;};

  /**
   */
  inline const QString& getSessionName() const {return sessionName_;};

  /**
   */
  inline const QString& getPath2RootDir() const {return path2RootDir_;};

  /**
   */
  inline const QString& getWrapperFileName() const {return wrapperFileName_;};

  /**
   */
  inline int getCurrentVersion() const {return currentVersion_;};

  /**
   */
  inline double getInputFormatVersion() const {return inputFormatVersion_;};

  /**
   */
  inline const SgMJD& getInputFormatEpoch() const {return inputFormatEpoch_;};

  /**
   */
  inline bool getHave2redoLeapSecond() const {return have2redoLeapSecond_;};

  /**
   */
  inline CorrelatorType getCorrelatorType() const {return correlatorType_;};

  /**
   */
  inline SgNetCdf::OperationMode getOperationMode() const {return operationMode_;};
  
  inline int getNumOfUserCorrections() const {return obsUserCorrections_.size();};

  inline const SgMJD& getLastModified() const {return lastModified_;};

  inline bool hasSidebandOrder() const {return hasSidebandOrder_;};


 
  // sets:
  /**
   */
  inline void setSessionCode(const QString& code) {sessionCode_=code;};

  /**
   */
  inline void setSessionName(const QString& name) {sessionName_=name;};

  /**
   */
  inline void setPath2RootDir(const QString& path) {path2RootDir_=path;};

  /**
   */
  inline void setWrapperFileName(const QString& name) {wrapperFileName_=name;};

  /**
   */
  inline void setCurrentVersion(int ver) {currentVersion_=ver;};

  /**
   */
  inline void setNumOfChannels(int num) {numOfChan_=num;};

  /**
   */
  inline void setOutputFormatVersion(double v) {outputFormatVersion_=v;};

  /**
   */
  inline void setOutputFormatEpoch(const SgMJD& t) {outputFormatEpoch_=t;};

  /**
   */
  inline void setCorrelatorType(CorrelatorType ct) {correlatorType_=ct;};

  /**
   */
  inline void setOperationMode(SgNetCdf::OperationMode om) {operationMode_=om;};


  /**
   */
  inline SgVdbVariable& vClockBreak() {return vClockBreak_;};
  inline SgVdbVariable& vClockApriori() {return vClockApriori_;};


  //
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**
   */
  bool init(const QString path, const QString fileName);

  /**
   */
  bool init(SgVlbiSession*);

  /**
   */
  inline bool hasXBandData();

  /**
   */
  inline bool hasSBandData();

  /**
   */
  inline QList<QString> getListOfBands() const {return bandDataByName_.keys();};

  /**
   */
  inline bool hasBand(const QString& band) const {return bandDataByName_.contains(band);};


  /**Returns list of files that contain data
   */
  virtual const QList<QString> listOfInputFiles() const;

  //
  // input:
  bool loadEpochs4Scans(QList<SgMJD>&);
  bool loadEpochs4Obs(QList<SgMJD>&);

//  bool loadSessionHead(QString& corrType, QString& corrName, QString& piName, QList<QString>& stations,
//    QList<QString>& sources, SgMJD& tStart, SgMJD& tFinis);
  bool loadSessionHead(QString& corrType, QString& corrName, QString& piName, QString& experimentName,
    int& experimentSerialNumber, QString& experimentDescription, QString& recordingMode, 
    QList<QString>& stations, QList<QString>& sources, SgMJD& tStart, SgMJD& tFinis, int& cppsIdx);
  //
  // reading "obs" part:
  /**
   */
  bool loadObsObjectNames(QList<QString>&, QList<QString>&, QList<QString>&);
  /**
   */
  bool loadObsCrossRefs(QVector<int>& obs2Scan);
  /**
   */
  bool loadObsSingleBandDelays(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsGroupDelays(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsRates(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsPhase(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsRefFreqs(const QString& band, SgVector*&);
  /**
   */
  bool loadObsEffFreqs(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsAmbigSpacing(const QString& band, SgVector*&);
  /**
   */
  bool loadObsQualityCodes(const QString& band, QVector<QString>& qualityCodes);
  /**
   */
  bool loadObsCorrelation(const QString &band, SgVector* &correlations);
  /**
   */
  bool loadObsUVFperAsec(const QString &band, SgMatrix* &uvfPerAsec);
  /**
   */
  bool loadObsSNR(const QString &band, SgVector* &snrs);
  /**
   */
  bool loadObsDtec(SgVector* &dTec, SgVector* &dTecStdDev);
  /**
   */
  bool loadObsDelayDataFlag(const QString &band, QVector<int>& delUFlag);
  /**
   */
  bool loadObsDataFlag(const QString &band, QVector<int>& delUFlag);
  /**
   */
  bool loadObsTheoreticals(SgVector*& delays, SgVector*& rates);
  //
  // correlator's stuff:
  /**
   */
  bool loadObsChannelInfo(const QString& band, // SgVlbiSessionInfo::OriginType originType,
    QVector<QString>& channelIds,                         // CHAN ID
    QVector<QString>& polarizations,                      // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& bitsPerSamples,                         // BITSAMPL
    QVector< QVector<int> >& errorRates_1,                // ERRORATE @1, phasecal data
    QVector< QVector<int> >& errorRates_2,                // ERRORATE @2, phasecal data
    QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
    QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
    QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
    QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&loFreqs_1,                                  // LO FREQ @1
    SgMatrix*&loFreqs_2,                                  // LO FREQ @2
    SgMatrix*&channelBandwidth,
    SgSidebandOrder &sbOrder
  );
  /**
   */
  bool loadObsPhaseCalInfo(const QString& band,
    SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
    SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
    SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
    SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2);
  /**
   */
  bool loadObsCorrelatorInfo(const QString& band, QList<QString>& fourfitOutputFName, 
    QList<QString>& fourfitErrorCodes, SgVector*& scanStartSec, SgVector*& scanStopSec);
  /**
   */
  bool loadObsCorrelatorInfo(const QString& band,
    QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//  QVector<int>& corelVersions,                      // fcCORELVER
    QVector<int>& startOffsets,                       // fcStartOffset
    QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
//  QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    SgVector*&deltaEpochs,                            // fcDELTAEPO
    SgMatrix*&urvrs,                                  // fcURVR
    SgMatrix*&instrDelays,                            // fcIDELAY
    SgMatrix*&starElev,                               // fcSTARELEV
    SgMatrix*&zenithDelays,                           // fcZDELAY
    SgMatrix*&searchParams,                           // fcSRCHPAR
    QVector<QString>& baselineCodes,                  // fcCORBASCD
    QVector<QString>& tapeQualityCodes,               // fcTapeCode
    QVector<int>& stopOffsets,                        // fcStopOffset
    QVector<int>& hopsRevisionNumbers,                // fcHopsRevNum
    SgVector*&sampleRate,                             // fcRECSETUP
    SgVector*&sbdResids,                              // fcSBRESID
    SgVector*&rateResids,                             // fcRATRESID
    SgVector*&effDurations,                           // fcEffectiveDuration
    SgVector*&startSeconds,                           // fcStartSec
    SgVector*&stopSeconds,                            // fcStopSec
    SgVector*&percentOfDiscards,                      // fcDISCARD
    SgVector*&uniformityFactors,                      // fcQBFACTOR
    SgVector*&geocenterPhaseResids,                   // fcGeocResidPhase
    SgVector*&geocenterPhaseTotals,                   // fcGeocPhase
    SgVector*&geocenterSbDelays,                      // fcGeocSBD
    SgVector*&geocenterGrDelays,                      // fcGeocMBD
    SgVector*&geocenterDelayRates,                    // fcGeocRate
    SgVector*&probOfFalses,                           // fcProbFalseDetection
    SgVector*&corrAprioriDelays,                      // fcABASDEL
    SgVector*&corrAprioriRates,                       // fcABASRATE
    SgVector*&corrAprioriAccs,                        // fcABASACCE
    SgVector*&incohAmps,                              // fcINCOHAMP
    SgVector*&incohAmps2,                             // fcINCOH2
    SgVector*&delayResids,                            // fcDELRESID
    QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
    QVector<QString>& tapeIds_1,                      // fcTAPEID
    QVector<QString>& tapeIds_2,                      // fcTAPEID
    QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    QVector< QVector<int> >& epochsOfFourfitings,     // fcFOURFUTC
    QVector< QVector<int> >& epochsOfNominalScanTime, // fcUTCScan
    SgMatrix*&corrClocks,                             // fcCORCLOCK
    SgMatrix*&mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
    SgMatrix*&mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM, 
                                                            // fcUTCVLB2, fcDELOBSVM
  );
  
  //
  // edits:
  /**
   */
  bool loadObsEditData(QVector<int>& delUFlag, QVector<int>& phsUFlag, QVector<int>& ratUFlag, 
    QVector<int>& uAcSup);
  /**
   */
  bool loadObsUserSup(QVector<int>& uAcSup);
  /**
   */
  bool loadObsNumGroupAmbigs(const QString &band, QVector<int> &numAmbigs, QVector<int> &numSubAmbigs);
  /**
   */
  bool loadObsNumPhaseAmbigs(const QString &band, QVector<int> &numAmbigs);
  /**
   */
  bool loadObsCalIonGroup(const QString& band, SgMatrix*& ionCals, SgMatrix*& ionSigmas, 
    QVector<int> &ionDataFlag);
  //
  // Calibrations:
  /**
   */
  bool loadObsCalEarthTide(SgMatrix*&, QString &kind);
  /**
   */
  bool loadObsCalFeedCorr(SgMatrix*&);
  /**
   */
  bool loadObsCalFeedCorr(const QString& band, SgMatrix*&);
  /**
   */
  bool loadObsCalHiFyErp(SgMatrix*&, SgMatrix*&, QString &kind);
  /**
   */
  bool loadObsCalOcean(SgMatrix*&);
  bool loadObsCalOceanOld(SgMatrix*&);
  /**
   */
  bool loadObsCalOceanPoleTideLoad(SgMatrix*&);
  /**
   */
  bool loadObsCalPoleTide(SgMatrix*&, QString &kind);
  /**
   */
  bool loadObsCalPoleTideOldRestore(SgMatrix*& cals);
  /**
   */
  bool loadObsCalTiltRmvr(SgMatrix*& cals);
  /**
   */
  bool loadObsFractC(SgVector*& v);
  /**
   */
  bool loadObsCalUnphase(const QString& band, SgMatrix*& unPhaseCal_1, SgMatrix*& unPhaseCal_2);
  /**
   */
  bool loadObsCalNutWahr(SgMatrix*&);
  /**
   */
  bool loadObsCalHiFyLibration(SgMatrix*&, QString &kind);
  /**
   */
  bool loadObsCalHiFyLibration(SgMatrix*&, SgMatrix*&, QString &kind);
  /**
   */
  bool loadObsCalWobble(SgMatrix*&, SgMatrix*&);
  /**
   */
  bool loadObsCalBend(SgMatrix*& cals);
  /**
   */
  bool loadObsCalBendSun(SgMatrix*& cals);
  /**
   */
  bool loadObsCalBendSunHigher(SgMatrix*& cals);
  /**
   */
  bool loadObsCalParallax(SgMatrix*& cals);
  //
  // Partials:
  /**
   */
  bool loadObsPartBend(SgMatrix*& partBend);
  /**
   */
  bool loadObsPartGamma(SgMatrix*& part);
  /**
   */
  bool loadObsPartParallax(SgMatrix*& part);
  /**
   */
  bool loadObsPartPoleTides(SgMatrix*& partX, SgMatrix*& partY, QString& kind);
  /**
   */
  bool loadObsPartEOP(SgMatrix*& dV_dPx, SgMatrix*& dV_dPy, SgMatrix*& dV_dUT1);
  /**
   */
  bool loadObsPartNut2KXY(SgMatrix*& dV_dCipX, SgMatrix*& dV_dCipY, QString& kind);
  /**
   */
  bool loadObsPartRaDec(SgMatrix*& dV_dRA, SgMatrix*& dV_dDN);
  /**
   */
  bool loadObsPartXYZ(SgMatrix*& dTau_dR_1, SgMatrix*& dRat_dR_1); //here dTau_dR_2 = -dTau_dR_1
  /**
   */
  bool loadObsUserCorrections(int idx, SgMatrix* m, QString& name);
  //
  //
  //
  //
  // Scans:
  /**
   */
  bool loadScanName(QVector<QString>& scanNames, QVector<QString>& scanFullNames);
  /**
   */
  bool loadScanCrootFname(QVector<QString>& corrRootFnames);

  bool loadScanEphemeris(SgMatrix*& rSun, SgMatrix*& rMoon, SgMatrix*& rEarth,
    SgMatrix*& vSun, SgMatrix*& vMoon, SgMatrix*& vEarth, SgMatrix*& aEarth);
  /**
   */
  bool loadScanNutationEqxWahr(SgMatrix*& pPsiEps, SgMatrix*& pPsiEps_rates);
  /**
   */
  bool loadScanNutationEqx(SgMatrix*& pPsiEps, SgMatrix*& pPsiEps_rates, QString& kind);
  /**
   */
  bool loadScanNutationNro(SgMatrix*& pXys, SgMatrix*& pXys_rates, QString& kind);
  /**
   */
  bool loadScanTrf2crf(SgMatrix*& val, SgMatrix*& rat, SgMatrix*& acc);
  /**
   */
  bool loadErpApriori(SgVector*& ut1_tai, SgMatrix*& pm);
  //
  //
  // Stations:
  /**
   */
  bool loadStationName(const QString& stnKey);
  /**
   */
  bool loadStationEpochs(const QString& stnName, QList<SgMJD>& epochs);
  /**
   */
  bool loadStationAzEl(const QString& stnName, SgMatrix* &azTheo, SgMatrix* &elTheo);
  /**
   */
  bool loadStationParAngle(const QString& stnName, SgVector* &parAngle);
  /**
   */
  bool loadStationCalAxisOffset(const QString& stnName, SgMatrix* &cal);
  /**
   */
  bool loadStationCalCable(const QString& stnName, SgMatrix* &cal, QString& origin, QString& kind);
  /**
   */
  bool loadStationCalCblCorrections(const QString& stnName, SgMatrix* &cal, QString& origin, 
    QString& kind);
  /**
   */
  bool loadStationMet(const QString& stnName, SgVector* &metAtmPres, SgVector* &metAtmRH, 
    SgVector* &metAtmTemp, QString& origin, QString& kind);
  /**
   */
  bool loadStationCalSlantPathTropDry(const QString& stnName, SgMatrix* &calNmfDry, QString &kind);
  /**
   */
  bool loadStationCalSlantPathTropWet(const QString& stnName, SgMatrix* &calNmfWet, QString &kind);
  /**
   */
  bool loadStationCalOceanLoad(const QString& stnName, SgMatrix* &calHorzOLoad, SgMatrix* &calVertOLoad);
  /**
   */
  bool loadStationPartAxisOffset(const QString& stnName, SgMatrix* &partAxisOffset);
  /**
   */
  bool loadStationOceanLdDisp(const QString& stnName, SgMatrix* &dis, SgMatrix* &vel);
  /**
   */
  bool loadStationPartZenithDelayDry(const QString& stnName, SgMatrix* &part, QString &kind);
  /**
   */
  bool loadStationPartZenithDelayWet(const QString& stnName, SgMatrix* &part, QString &kind);
  /**
   */
  bool loadStationPartHorizonGrad(const QString& stnName, SgMatrix* &part, QString &kind);
  /**
   */
  bool loadStationRefClockOffset(const QString& stnName, SgVector* &refClockOffset);
  /**
   */
  bool loadStationTsys(const QString& stnName, SgMatrix* &tsyses, QVector<double>& frqs, 
    QVector<QString>& ids, QVector<QString>& sbs, QVector<QString>& polzs);
  
  //
  //
  // A priori:
  /**
   */
  bool loadSourcesApriories(QVector<QString> &sourcesNames, SgMatrix* &cooRaDe, 
    QVector<QString> &sourcesApRefs);
  /**
   */
  bool loadClockApriories(QVector<QString> &stationNames, SgMatrix* &offsets_n_rates);
  /**
   */
  bool loadStationsApriories(QVector<QString> &stationsNames, SgMatrix* &cooXYZ, 
    QVector<QString> &tectonicPlateNames);
  /**
   */
  bool loadAntennaApriories(QVector<QString> &stationsNames, QVector<int> &axisTypes, 
    SgVector* &axisOffsets, SgMatrix* &axisTilts);
  /**
   */
  bool loadEccentricities(QVector<QString> &stationsNames, QVector<QString> &eccTypes,
    QVector<QString> &eccNums, SgMatrix* &eccVals);

  //
  //
  // Solve's stuff:
  /**
   */
  bool loadAtmSetup(SgVector* &interval, SgVector* &rateConstraint, QVector<QString> &stationsNames);
  /**
   */
  bool loadClockSetup(QList<QString> &refClocks, SgVector* &interval, SgVector* &rateConstraint, 
    QVector<QString> &stationsNames);
  /**
   */
  bool loadCalcEop(SgVector* &pmArrayInfo, SgVector* &utArrayInfo, SgMatrix* &pmValues, 
    SgVector* &utValues, QString &sUtMode, QString &sUtModule, QString &sUtOrigin, QString &sPmMode, 
    QString &sPmModule, QString &sPmOrigin);
  /**
   */
  bool loadCalibrations(int &obsCalFlags, QVector<int> &statCalFlags, 
    QVector< QVector<int> > &flybyFlags, QVector<QString> &statCalNames, QVector<QString> &flybyNames,
    QVector<QString> &calSiteNames, QVector<QString> &obsCalNames);
  /**
   */
  bool loadSelectionStatus(QVector<int> &sourcesSelection, QVector< QVector<int> > &baselinesSelection);
  /**
   */
  bool loadSolveWeights(QVector<QString> &baselineNames, SgMatrix* &groupBlWeights);
  /**
   */
  bool loadClockBreaks(QVector<QString> &cbNames, QVector<SgMJD> &cbEpochs, QVector<int> &cbFlags);
  /**
   */
  bool loadLeapSecond(int &leapSecond);
  /**
   */
  bool loadMiscCable(QMap<QString, QString>& cableSignByKey);
  /**
   */
  bool loadMiscFourFit(const QString& band,
    QVector<QString>& fourfitControlFile, QVector<QString>& fourfitCommand,
    QVector<int>& numLags, QVector<double>& apLength);
  
  /**
   */
  bool loadCalcInfo(int &tidalUt1, double &calcVersionValue);
  bool loadCalcInfo(SgModelsInfo& calcInfo);

  /**
   */
  bool loadHistory(QList<SgVlbiHistoryRecord*>&);



  // output:
  bool storeEpochs4Scans(const QList<SgMJD>&);
  bool storeEpochs4Obs(const QList<SgMJD>&);

  /**
   */
  bool storeObsCalIonGroup(const QString& band, 
    const SgMatrix* ionCals, const SgMatrix* ionSigmas, const QVector<int> &ionDataFlag);
  /**
   */
  bool storeIonoBits(const QVector<int>& ionoBits);
  /**
   */
  bool storeObsEditData(const QVector<int>& delUFlag, const QVector<int>& phsUFlag, 
    const QVector<int>& ratUFlag, const QVector<int>& uAcSup);
  /**
   */
  bool storeObsUserSup(const QVector<int>& uAcSup);

  /**
   */
  bool storeObsNumGroupAmbigs(const QString &band, const QVector<int> &numAmbigs, 
    const QVector<int> &numSubAmbigs);
  /**
   */
  bool storeObsNumPhaseAmbigs(const QString &band, const QVector<int> &numAmbigs);
  /**
   */
  bool storeAtmSetup(const SgVector* interval, const SgVector* rateConstraint, 
    const QList<QString> &sites);
  /**
   */
  bool storeClockSetup(const SgVector* interval, const SgVector* rateConstraint, 
    const QList<QString> &sites, const QList<QString> &refSites);
  /**
   */
  bool storeErpSetup(double pmOffsetConstraint, double utOffsetConstraint);
  /**
   */
  bool storeIonoSetup(const QList<int> &flags, const QList<QString> &sites);
  /**
   */
  bool storeSolveWeights(const QVector<QString> &baselineNames, SgMatrix* groupBlWeights);
  /**
   */
  bool storeSelectionStatus(const QVector<int> &sourcesSelection,
    const QVector< QVector<int> > &stationsSelection);
  /**
   */
  bool storeBaselineClockSetup(const QList<QString>& baselines);
  /**
   */
  bool storeEccentricities(const QVector<QString> &stationsNames, 
    const QVector<QString> &eccTypes, const QVector<QString> &eccNums, SgMatrix* eccVals, 
    const QString &eccFileName);
  /**
   */
  bool storeCalibrations(int obsCalFlags, const QVector<int> &statCalFlags, 
    const QVector< QVector<int> > &flybyFlags, const QVector<QString> &statCalNames,
    const QVector<QString> &flybyNames, const QVector<QString> &calSiteNames, 
    const QVector<QString> &obsCalNames);
  /**
   */
  bool storeClockBreaks(const QVector<QString> &cbNames, const QVector<SgMJD> &cbEpochs, 
    const QVector<int> &cbFlags, const QString& band="");
  /**
   */
  bool storeLeapSecond(int leapSeconds);
  /**
   */
  bool storeMiscCable(const QMap<QString, QString>& cableSignByKey);
  /**
   */
  bool storeMiscFourFit(const QString& band, SgVlbiSessionInfo::OriginType originType,
    const QVector<QString>& fourfitControlFile, const QVector<QString>& fourfitCommand,
    const QVector<int>& numLags, const QVector<double>& apLength);
  //
  /**
   */
  bool storeObsObjectNames(const QList<QString>& stations_1, const QList<QString>& stations_2, 
    const QList<QString>& sources);
  /**
   */
  bool storeSessionHead(const QString& corrType, const QString& corrName, const QString& piName,
    const QString& experimentName, int experimentSerialNumber, 
    const QString& experimentDescription, const QString& recordingMode,
    const QList<QString>& stations, const QList<QString>& sources,
    const SgMJD& tStart, const SgMJD& tFinis, int cppsIdx);
  /**
   */
  bool storeCalcInfo(const SgModelsInfo& calcInfo);
  /**
   */
  bool storeCalcEop(const SgVector* pmArrayInfo, const SgVector* utArrayInfo, const SgMatrix* pmValues,
    const SgVector* utValues, const SgModelsInfo& calcInfo);
  /**
   */
  bool storeSourcesApriories(const QList<QString> &sourcesNames, const QList<QString> &sourcesRefs,
    const SgMatrix* cooRaDe);
  /**
   */
  bool storeClockApriories(const QList<QString> &siteNames, const SgMatrix* offsets_n_rates);
  /**
   */
  bool storeStationsApriories(const QList<QString> &stationsNames, const SgMatrix* cooXYZ, 
    const QList<QString> &tectonicPlateNames);
  /**
   */
  bool storeAntennaApriories(const QList<QString> &stationsNames, const QList<int> &axisTypes, 
    const SgVector* axisOffsets, const SgMatrix* axisTilts);


  //
  //                                    Scan:
  /**
   */
  bool storeScanName(const QList<QString>& scanNames, const QList<QString>& scanFullNames);

  bool storeScanTimeMjd(const QList<SgMJD>& epochs);
  
  bool storeScanCrootFname(const QList<QString>& corrRootFnames);

  bool storeScanEphemeris(const SgMatrix* rSun, const SgMatrix* rMoon, const SgMatrix* rEarth,
    const SgMatrix* vSun, const SgMatrix* vMoon, const SgMatrix* vEarth, const SgMatrix* aEarth, 
    const QString& kind);
  /**
   */
  bool storeScanNutationEqxWahr(const SgMatrix* pPsiEps, const SgMatrix* pPsiEps_rates);
  /**
   */
  bool storeScanNutationEqx(const SgMatrix* pPsiEps, const SgMatrix* pPsiEps_rates, const QString& kind);
  /**
   */
  bool storeScanNutationNro(const SgMatrix* pXys, const SgMatrix* pXys_rates, const QString& kind);

  bool storeScanTrf2crf(const SgMatrix* val, const SgMatrix* rat, const SgMatrix* acc);
  bool storeErpApriori(const SgVector* ut1_tai, const SgMatrix* pm);
  
  //
  //                                  Observations:
  //
  bool storeObsCrossRefs(const QList<int>& obs2scan,
    const QList<int>& obs2stn_1, const QList<int>& obs2stn_2);

  bool storeSourceCrossRefs(const QList<QString>& sources, const QList<int>& scan2src);
  
  bool storeStationCrossRefs(const QList<int>& numScansPerStation, const QList<QString>& stations, 
    const QMap<QString, QList<int> >& station2scan, const QMap<QString, QList<int> >& scan2station);

  bool storeObsSingleBandDelays(const QString& band, const SgMatrix* singleBandDelays);
  
  bool storeObsGroupDelays(const QString& band, const SgMatrix* groupDelays, const QString& kind);
  
  bool storeObsGroupDelaysFull(const QString& band, const SgVector* groupDelays);
 
  bool storeObsPhaseDelaysFull(const QString& band, const SgVector* groupDelays, 
    const SgVector* phaseDelaySigs);

  bool storeObsRates(const QString& band, const SgMatrix* rates, const QString& kind);
  
  bool storeObsPhase(const QString& band, const SgMatrix* phases);

  bool storeObsRefFreqs(const QString& band, const SgVector* rf);
  
  bool storeObsEffFreqs(const QString& band, const SgMatrix* freqs, bool areEqWgts=false);
  
  bool storeObsAmbigSpacing(const QString& band, const SgVector* ambigs);
  
  bool storeObsQualityCodes(const QString& band, const QVector<char>& qualityCodes);

  bool storeObsCorrelation(const QString &band, const SgVector* correlations);

  bool storeObsUVFperAsec(const QString &band, const SgMatrix* uvfPerAsec);

  bool storeObsSNR(const QString &band, const SgVector* snrs);

  bool storeObsDtec(const SgVector* dTec, const SgVector* dTecStdDev);
  
  bool storeObsDataFlag(const QString &band, const QVector<int>& dataFlags);

  /**
   */
  bool storeObsChannelInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
    const QVector<QString>& channelIds,                         // CHAN ID
    const QVector<QString>& polarizations,                      // POLARIZ
    const QVector<int>& numOfChannels,                          // #CHANELS
    const QVector<int>& bitsPerSamples,                         // BITSAMPL
    const QVector< QVector<int> >& errorRates_1,                // ERRORATE @1, phasecal data
    const QVector< QVector<int> >& errorRates_2,                // ERRORATE @2, phasecal data
    const QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
    const QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
    const QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
    const QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
    const SgVector* sampleRate,                                 // SAMPLRAT
    const SgMatrix* residFringeAmplByChan,                      // AMPBYFRQ Amp
    const SgMatrix* residFringePhseByChan,                      // AMPBYFRQ Phs
    const SgMatrix* refFreqByChan,                              // RFREQ
    const SgMatrix* numOfSamplesByChan_USB,                     // #SAMPLES usb
    const SgMatrix* numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    const SgMatrix* numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    const SgMatrix* numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    const SgMatrix* loFreqs_1,                                  // LO FREQ @1
    const SgMatrix* loFreqs_2,                                  // LO FREQ @2
    const SgMatrix* bandwidthByChan,
    SgSidebandOrder sbOrder
  );
  /**
  */
  bool storeObsPhaseCalInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
    const SgMatrix* phCalFrqs_1ByChan, 
    const SgMatrix* phCalAmps_1ByChan, 
    const SgMatrix* phCalPhss_1ByChan, 
    const SgMatrix* phCalOffs_1ByChan, 
    const SgVector* phCalRates_1, 

    const SgMatrix* phCalFrqs_2ByChan, 
    const SgMatrix* phCalAmps_2ByChan, 
    const SgMatrix* phCalPhss_2ByChan, 
    const SgMatrix* phCalOffs_2ByChan, 
    const SgVector* phCalRates_2
  );
  /**
  */
  bool storeObsCorrelatorInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
    const QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    const QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
    const QVector<QString>& frqGrpCodes,                    // fcFRQGROUP 
    const QVector<int>& corelVersions,                      // fcCORELVER
    const QVector<int>& startOffsets,                       // fcStartOffset
    const QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
    const QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    const SgVector* deltaEpochs,                            // fcDELTAEPO
    const SgMatrix* urvrs,                                  // fcURVR
    const SgMatrix* instrDelays,                            // fcIDELAY
    const SgMatrix* starElev,                               // fcSTARELEV
    const SgMatrix* zenithDelays,                           // fcZDELAY
    const SgMatrix* searchParams,                           // fcSRCHPAR
    const QVector<QString>& baselineCodes,                  // fcCORBASCD
    const QVector<QString>& tapeQualityCodes,               // fcTapeCode
    const QVector<int>& stopOffsets,                        // fcStopOffset
    const QVector<int>& hopsRevisionNumbers,                // fcHopsRevNum
    const SgVector* sampleRate,                             // fcRECSETUP
    const SgVector* sbdResids,                              // fcSBRESID
    const SgVector* rateResids,                             // fcRATRESID
    const SgVector* effDurations,                           // fcEffectiveDuration
    const SgVector* startSeconds,                           // fcStartSec
    const SgVector* stopSeconds,                            // fcStopSec
    const SgVector* percentOfDiscards,                      // fcDISCARD
    const SgVector* uniformityFactors,                      // fcQBFACTOR
    const SgVector* geocenterPhaseResids,                   // fcGeocResidPhase
    const SgVector* geocenterPhaseTotals,                   // fcGeocPhase
    const SgVector* geocenterSbDelays,                      // fcGeocSBD
    const SgVector* geocenterGrDelays,                      // fcGeocMBD
    const SgVector* geocenterDelayRates,                    // fcGeocRate
    const SgVector* probOfFalses,                           // fcProbFalseDetection
    const SgVector* corrAprioriDelays,                      // fcABASDEL
    const SgVector* corrAprioriRates,                       // fcABASRATE
    const SgVector* corrAprioriAccs,                        // fcABASACCE
    const SgVector* incohAmps,                              // fcINCOHAMP
    const SgVector* incohAmps2,                             // fcINCOH2
    const SgVector* delayResids,                            // fcDELRESID
    const QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
    const QVector<QString>& tapeIds_1,                      // fcTAPEID
    const QVector<QString>& tapeIds_2,                      // fcTAPEID
    const QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    const QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    const QVector< QVector<int> >& epochsOfFourfitings,     // fcFOURFUTC
    const QVector< QVector<int> >& epochsOfNominalScanTime, // fcUTCScan
    const SgMatrix* corrClocks,                             // fcCORCLOCK
    const SgMatrix* mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
    const SgMatrix* mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM, 
                                                            // fcUTCVLB2, fcDELOBSVM
  );
  //                      calcables:
  /**
   */
  bool storeObsDelayTheoretical(const SgVector* v);
  bool storeObsRateTheoretical (const SgVector* v);

  bool storeObsStd1Contribution(const QString& varName, const SgMatrix* contrib, 
    SgVdbVariable& var, const QList<SgVgosDb::FmtChkVar*>& fcf);
  bool storeObsStd2Contribution(const QString& varName, const QList<const SgMatrix*> &contribs,
    SgVdbVariable& var, const QList<SgVgosDb::FmtChkVar*>& fcf);

  /**
   */
  bool storeObsCalEarthTide(const SgMatrix*, const QString& kind);
  /**
   */
  bool storeObsCalFeedCorr(const SgMatrix*);
  /**
   */
  bool storeObsCalFeedCorr(const QString &band, const SgMatrix*);
  /**
   */
  bool storeObsCalHiFyErp(const SgMatrix*, const SgMatrix*, const QString& kind);
  /**
   */
  bool storeObsCalOcean(const SgMatrix*);
  bool storeObsCalOceanOld(const SgMatrix*);
  /**
   */
  bool storeObsCalOceanPoleTideLoad(const SgMatrix*);
  /**
   */
  bool storeObsCalPoleTide(const SgMatrix*, const QString& kind);
  /**
   */
  bool storeObsCalPoleTideOldRestore(const SgMatrix* cals);
  /**
   */
  bool storeObsCalTiltRmvr(const SgMatrix* cals);
  /**
   */
  bool storeObsCalNutWahr(const SgMatrix*);
  /**
   */
  bool storeObsCalHiFyLibration(const SgMatrix*, const SgMatrix*, const QString& kind);
  /**
   */
  bool storeObsCalWobble(const SgMatrix*, const SgMatrix*);
  /**
   */
  bool storeObsCalBend(const SgMatrix* cals);
  /**
   */
  bool storeObsCalBendSun(const SgMatrix* cals);
  /**
   */
  bool storeObsCalBendSunHigher(const SgMatrix* cals);
  /**
   */
  bool storeObsCalParallax(const SgMatrix* cals);
  
  bool storeObsFractC(const SgVector* v);  

  /**
   */
  bool storeObsCalUnphase(const QString &band, const SgMatrix* phaseCal_1, const SgMatrix* phaseCal_2);
  //
  //                           obs partials:
  /**
   */
  bool storeObsPartBend(const SgMatrix* partBend);
  /**
   */
  bool storeObsPartEOP(const SgMatrix* dV_dPx, const SgMatrix* dV_dPy, const SgMatrix* dV_dUT1);
  /**
   */
  bool storeObsPartNut2KXY(const SgMatrix* dV_dCipX, const SgMatrix* dV_dCipY, const QString& kind);
  /**
   */
  bool storeObsPartRaDec(const SgMatrix* dV_dRA, const SgMatrix* dV_dDN);
  /**
   */
  bool storeObsPartXYZ(const SgMatrix* dDel_dR_1, const SgMatrix* dRat_dR_1); // dTau_dR_2 = -dTau_dR_1
  /**
   */
  bool storeObsPartGamma(const SgMatrix* part);
  /**
   */
  bool storeObsPartParallax(const SgMatrix* part);
  /**
   */
  bool storeObsPartPoleTides(const SgMatrix* dV_dPtdX, const SgMatrix* dV_dPtdY, const QString& kind);


  //                                        Stations:
  /**
   */
  bool storeStationEpochs(const QString& stnName, const QList<SgMJD>& epochs);
  /**
   */
  bool storeStationCalCable(const QString& stnName, const SgMatrix* cal,
    const QString& origin, const QString& kind);
  /**
   */
  bool storeStationCalCblCorrections(const QString& stnName, const SgMatrix* cal,
    const QString& origin /*, const QString& kind*/);
  /**
   */
  bool storeStationMet(const QString& stnName, SgVector*const metAtmPres, 
    SgVector*const metAtmRH, SgVector*const metAtmTemp, const QString& origin, const QString& kind);
  /**
   */
  bool storeStationAzEl(const QString& stnName, SgMatrix*const azTheo, SgMatrix*const elTheo);
  /**
   */
  bool storeStationPartAxisOffset(const QString& stnName, const SgMatrix* partAxisOffset);
  /**
   */
  bool storeStationParAngle(const QString& stnName, const SgVector* parAngle);
  /**
   */
  bool storeStationCalAxisOffset(const QString& stnName, const SgMatrix* cal);
  /**
   */
  bool storeStationCalSlantPathTropDry(const QString& stnName, const SgMatrix* cal, const QString& kind);
  /**
   */
  bool storeStationCalSlantPathTropWet(const QString& stnName, const SgMatrix* cal, const QString& kind);
  /**
   */
  bool storeStationCalOceanLoad(const QString& stnName, 
    const SgMatrix* calHorzOLoad, const SgMatrix* calVertOLoad);
  /**
   */
  bool storeStationOceanLdDisp(const QString& stnName, const SgMatrix* dis, const SgMatrix* vel);
  /**
   */
  bool storeStationPartZenithDelayDry(const QString& stnName, const SgMatrix* part, const QString& kind);
  /**
   */
  bool storeStationPartZenithDelayWet(const QString& stnName, const SgMatrix* part, const QString& kind);
  /**
   */
  bool storeStationPartHorizonGrad(const QString& stnName, const SgMatrix* part, const QString& kind);
  /**
   */
  bool storeStationRefClockOffset(const QString& stnName, const SgVector* v);
  /**
   */
  bool storeStationTsys(const QString& stnName, const SgMatrix* tsyses, const QVector<double>& frqs, 
    const QVector<QString>& ids, const QVector<QString>& sbs, const QVector<QString>& polzs);
  //
  //                                        End of Station
  //
  /**
   */
  bool composeWrapperFile();

  /**
   */
  bool saveLocalHistory(const SgVlbiHistory& history);

  /**
   */
  bool saveForeignHistory(const SgVlbiHistory& history, const QString& softwareName, 
    const SgMJD& epochOfCreation, const QString& creator, const QString& defaultDir,
    const QString& historyFileName, const QString& version, bool isMk3Compatible=true);


  /**
   */
  const QList<SgVdbVariable*>& getActiveVars() const {return activeVars_;};



  //
  // Friends:
  //

  //
  // I/O:
  //
  void dump2stdout();

private:
  struct StationDescriptor
  {
    QString                     stationName_;   // name of a station, as it in databases
    QString                     stationKey_;    // name of directory where the stn data are placed
    int                         numOfPts_;
    SgVdbVariable               vTimeUTC_;
    SgVdbVariable               vMet_;
    SgVdbVariable               vAzEl_;
    SgVdbVariable               vFeedRotation_;
    SgVdbVariable               vCal_AxisOffset_;
    SgVdbVariable               vCal_Cable_;
    SgVdbVariable               vCal_CableCorrections_; // an array of FS log, CDMA and PCMT data; prev version
    SgVdbVariable               vCal_CblCorrections_;   // an array of FS log, CDMA and PCMT data
    SgVdbVariable               vCal_SlantPathTropDry_;
    SgVdbVariable               vCal_SlantPathTropWet_;
    SgVdbVariable               vCal_OceanLoad_;
    //    SgVdbVariable               vCal_UnphaseCal_;
    SgVdbVariable               vPart_AxisOffset_;
    SgVdbVariable               vPart_ZenithPathTropDry_; 
    SgVdbVariable               vPart_ZenithPathTropWet_;
    SgVdbVariable               vPart_HorizonGrad_;
    SgVdbVariable               vDis_OceanLoad_;
    // KOMB:
    SgVdbVariable               vRefClockOffset_;
    // TSYS:
    SgVdbVariable               vTsys_;
    StationDescriptor();
    void propagateStnKey();
  };
  struct HistoryDescriptor
  {
    QString                     processName_;
    SgMJD                       epochOfCreation_;         //  "RunTimeTag 2014/02/10 08:28:53"
    QString                     creator_;                 //  "CreatedBy John M. Gipson"
    QString                     defaultDir_;              //  "Default_dir History"
    QString                     historyFileName_;         //  "History    08AUG12XA_kMK3DB_V004.hist"
    QString                     inputWrapperFileName_;    //  "InputWrapper 15JUL28BB_V001_kall.wrp"
    QString                     version_;                 //  "Version   Mixed"
    bool                        isMk3Compatible_;
    HistoryDescriptor();
  };
  struct ProgramGenericDescriptor
  {
    QString                     programName_;
    QList<QString>              content_;
    ProgramGenericDescriptor();
  };
  struct ProgramSolveDescriptor
  {
    QString                     programName_;
    // Session:
    SgVdbVariable               vCalcInfo_;
    SgVdbVariable               vCalibrationSetup_;
    SgVdbVariable               vAtmSetup_;
    SgVdbVariable               vClockSetup_;
    SgVdbVariable               vErpSetup_;
    SgVdbVariable               vIonoSetup_;
    SgVdbVariable               vCalcErp_;
    SgVdbVariable               vBaselineClockSetup_;
    SgVdbVariable               vSelectionStatus_;
    // Scan:
    SgVdbVariable               vScanTimeMJD_;
    // Observation:
    SgVdbVariable               vIonoBits_;
    SgVdbVariable               vFractC_;
    SgVdbVariable               vUnPhaseCalFlag_;
    SgVdbVariable               vUserSup_;
    QList<SgVdbVariable*>       vdbVars_;
    ProgramSolveDescriptor();
    bool hasSomething4output();
  };
  struct BandData
  {
    QString                     bandName_;
    // observables:
    SgVdbVariable               vChannelInfo_;
    SgVdbVariable               vPhaseCalInfo_;
    SgVdbVariable               vCorrInfo_;
    SgVdbVariable               vQualityCode_;
    SgVdbVariable               vRefFreq_;
    SgVdbVariable               vAmbigSize_;
    SgVdbVariable               vSNR_;
    SgVdbVariable               vCorrelation_;
    SgVdbVariable               vPhase_;
    SgVdbVariable               vSBDelay_;
    SgVdbVariable               vGroupDelay_;
    SgVdbVariable               vGroupRate_;
    SgVdbVariable               vDelayDataFlag_;
    SgVdbVariable               vDataFlag_;
    // obs derived:
    SgVdbVariable               vPhaseDelayFull_;
    SgVdbVariable               vUVFperAsec_;
    SgVdbVariable               vEffFreq_EqWt_;
    SgVdbVariable               vEffFreq_;
    SgVdbVariable               vCal_SlantPathIonoGroup_;
    // theoretical values:
    SgVdbVariable               vCal_FeedCorrection_;
    SgVdbVariable               vCal_Unphase_;
    // edits:
    SgVdbVariable               vNumGroupAmbig_;
    SgVdbVariable               vNumPhaseAmbig_;
    SgVdbVariable               vGroupDelayFull_;
    // session dir:
    SgVdbVariable               vMiscFourFit_;
    //
    BandData();
  };
  //
  // ----------------------------------------------------------------------------
  // members:

  // identities:
//  SgIdentities                 *identities_;
//  SgVersion                    *driverVersion_;
  bool                          isNewFile_;
  bool                          have2adjustPermissions_;

  // general info:
  int                           numOfObs_;
  int                           numOfScans_;
  int                           numOfChan_;
  int                           numOfStn_;
  int                           numOfSrc_;
  QList<SgVdbVariable*>         activeVars_;
  CorrelatorType                correlatorType_;

  // path and dirname:
  QString                       path2RootDir_;
  QString                       wrapperFileName_;
  QString                       corrTypeId_;
  // wrapper file format version:
  double                        inputFormatVersion_;
  SgMJD                         inputFormatEpoch_;
  double                        outputFormatVersion_;
  SgMJD                         outputFormatEpoch_;
  QString                       outputFormatId_;
  //
  // History part:
//  QMap<QString, HistoryDescriptor> 
//                                historyDescriptorByName_;
  QList<HistoryDescriptor>      historyDescriptors_;
  HistoryDescriptor             localHistory_;
  int                           currentVersion_;
  SgMJD                         lastModified_;
  
  
  //
  //          sections of the wrapper file:
  //
  // Section SESSION:
  QString                       sessionCode_;
  QString                       sessionName_;
  SgVdbVariable                 vHead_;
  //
  // session dir:
  SgVdbVariable                 vGroupBLWeights_;
  SgVdbVariable                 vClockBreak_;
  SgVdbVariable                 vLeapSecond_;
  bool                          have2redoLeapSecond_;
  SgVdbVariable                 vMiscFourFit_;
  SgVdbVariable                 vMiscCable_;
  //
  // theo:
  // a priori:
  SgVdbVariable                 vStationApriori_;
  SgVdbVariable                 vSourceApriori_;
  SgVdbVariable                 vClockApriori_;  
  SgVdbVariable                 vAntennaApriori_;
  SgVdbVariable                 vEccentricity_;
  //
  // crossRefs:
  SgVdbVariable                 vStationCrossRef_;
  SgVdbVariable                 vSourceCrossRef_;
  // End of section SESSION
  //
  //
  // Section STATIONS:
  QMap<QString, StationDescriptor>
                                stnDescriptorByKey_;
  QMap<QString, StationDescriptor*>
                                stnDescriptorByName_;
  // End of section STATIONS
  //
  //
  // Section SCAN:
  SgVdbVariable                 vScanTimeUTC_;
  SgVdbVariable                 vScanName_;
  SgVdbVariable                 vErpApriori_;
  SgVdbVariable                 vEphemeris_;
  SgVdbVariable                 vNutationEqx_kWahr_;
  SgVdbVariable                 vNutationEqx_;
  SgVdbVariable                 vNutationNro_;
  SgVdbVariable                 vRot_CF2J2K_;
  SgVdbVariable                 vCorrRootFile_;
  //  QMap<QString, SgVdbVariable>  vCorrRootFileByBand_;
  // End of section SCAN
  //
  //
  // Section OBSERVATION:
  SgVdbVariable                 vObservationTimeUTC_;
  SgVdbVariable                 vBaseline_;
  SgVdbVariable                 vSource_;
  //
  //
  QMap<QString, BandData>       bandDataByName_;
  //
  // VGOS:
  SgVdbVariable                 vDiffTec_;
  // theo.cals:
  SgVdbVariable                 vCal_Bend_;
  SgVdbVariable                 vCal_BendSun_;
  SgVdbVariable                 vCal_BendSunHigher_;
  SgVdbVariable                 vCal_EarthTide_;
  SgVdbVariable                 vCal_FeedCorrection_;
  SgVdbVariable                 vCal_HfErp_;
  SgVdbVariable                 vCal_OceanLoad_;
  SgVdbVariable                 vCal_OceanLoadOld_;
  SgVdbVariable                 vCal_OceanPoleTideLoad_;
  SgVdbVariable                 vCal_Parallax_;
  SgVdbVariable                 vCal_PoleTide_;
  SgVdbVariable                 vCal_PoleTideOldRestore_;
  SgVdbVariable                 vCal_TiltRemover_;
  SgVdbVariable                 vCal_Wobble_;
  SgVdbVariable                 vCal_HfLibration_;
  SgVdbVariable                 vCal_HiFreqLibration_;
  //
  // obsEdit:
  SgVdbVariable                 vEdit_;
  SgVdbVariable                 vNGSQualityFlag_;
  //
  // crossRefs:
  SgVdbVariable                 vObsCrossRef_;
  //
  // obsDerived:
  SgVdbVariable                 vFeedRotNet_;
  //
  // obsTheo:
  SgVdbVariable                 vDelayTheoretical_;
  SgVdbVariable                 vRateTheoretical_;
  //
  // obsPartials:
  SgVdbVariable                 vPart_Bend_;
  SgVdbVariable                 vPart_Gamma_;
  SgVdbVariable                 vPart_Erp_;
  SgVdbVariable                 vPart_NutationEqx_;
  SgVdbVariable                 vPart_NutationNro_;
  SgVdbVariable                 vPart_Parallax_;
  SgVdbVariable                 vPart_PoleTide_;
  SgVdbVariable                 vPart_Precession_;
  SgVdbVariable                 vPart_RaDec_;
  SgVdbVariable                 vPart_Xyz_;
  // End of section OBSERVATION

  // ObsUserCorrections:
  QList<SgVdbVariable*>         obsUserCorrections_;
  

  // Section Program:
  ProgramSolveDescriptor        progSolveDescriptor_;
  QMap<QString, ProgramGenericDescriptor> // all others
                                progDescriptorByName_;
  // End of section Program

  QList<SgVdbVariable*>         vdbVariables_;
  SgNetCdf::OperationMode       operationMode_;
  
  //
  SgSidebandOrder               sbOrder_;
  bool                          hasSidebandOrder_;
  

  //
  //
  // functions:
  int parseVersionBlock(QTextStream&, const QString&);
  int parseHistoryBlock(QTextStream&);
  int parseHistoryProcessSubBlock(QTextStream&, const QString&);
  int parseSessionBlock(QTextStream&);
  int parseStationBlock(QTextStream&, const QString&);
  int parseScanBlock(QTextStream&);
  int parseObservationBlock(QTextStream&);
  int parseProgramBlock(QTextStream&, const QString&);
  
  void collectStationNames();
  void makeWorkAround4KnownBugs();
  
  int procNcString(const QString& str, const QString& subDir, SgVdbVariable& destination);
  int procNcStringSpec(const QString& str, const QString& subDir, SgVdbVariable& destination);
  void parseBandKey(const QString& str, QString& key);
  bool isValidNcFile(const QString&);
  bool isValidNcFiles();
  
//  void dump2stdout();
  
  bool checkFormat(const QList<FmtChkVar*>&, const SgNetCdf&, bool ok2fail=false);
  bool setupFormat(const QList<FmtChkVar*>&, SgNetCdf&,
    const QString& stationKey = QString(""), const QString& bandKey = QString(""));
  bool loadStdObsCalibration(SgMatrix*& cals, QString& origin,
    const QString& varName, const SgVdbVariable& odbV, const FmtChkVar& fc, 
    const QList<FmtChkVar*>& fcf, bool = true);
  const QString guessDimName(const QString& varName, int len);
  int guessDimVal(const QString&, int d, const QString& auxKey);
  //
  void createWrapperFileName(QString &newName);
  void composeVersionBlock(QTextStream&);
  void composeHistoryBlock(QTextStream&);
  void composeSessionBlock(QTextStream&);
  void composeStationBlock(QTextStream&, const StationDescriptor&);
  void composeScanBlock(QTextStream&);
  void composeObservationBlock(QTextStream&);
  void composeProgramSolveBlock(QTextStream&);
  void composeProgramGenericBlock(QTextStream&, const ProgramGenericDescriptor&);

//  void splitNcFileName(const QString& fileName, QString& subDir, QString& baseName);
  void writeNcFileName(QTextStream&, QString& currentSubDir, const SgVdbVariable& var);
  //
  //
  // correlator infos:
  bool loadObsCorrelatorInfoMk3(const QString& band, QList<QString>& fourfitOutputFName, 
    QList<QString>& fourfitErrorCodes, SgVector*& scanStartSec, SgVector*& scanStopSec);
  bool loadObsCorrelatorInfoCrl(const QString& band, QList<QString>& fourfitOutputFName, 
    QList<QString>& fourfitErrorCodes, SgVector*& scanStartSec, SgVector*& scanStopSec);
  bool loadObsCorrelatorInfoMk4(const QString& band, QList<QString>& fourfitOutputFName, 
    QList<QString>& fourfitErrorCodes, SgVector*& scanStartSec, SgVector*& scanStopSec);

  // updated functions:
  bool loadObsCorrelatorInfoMk3(const QString& band,
    QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//  QVector<int>& corelVersions,                      // fcCORELVER
    QVector<int>& startOffsets,                       // fcStartOffset
    QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
//  QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    SgVector*&deltaEpochs,                            // fcDELTAEPO
    SgMatrix*&urvrs,                                  // fcURVR
    SgMatrix*&instrDelays,                            // fcIDELAY
    SgMatrix*&starElev,                               // fcSTARELEV
    SgMatrix*&zenithDelays,                           // fcZDELAY
    SgMatrix*&searchParams,                           // fcSRCHPAR
    QVector<QString>& baselineCodes,                  // fcCORBASCD
    QVector<QString>& tapeQualityCodes,               // fcTapeCode
    QVector<int>& stopOffsets,                        // fcStopOffset
    QVector<int>& hopsRevisionNumbers,                // fcHopsRevNum
    SgVector*&sampleRate,                             // fcRECSETUP
    SgVector*&sbdResids,                              // fcSBRESID
    SgVector*&rateResids,                             // fcRATRESID
    SgVector*&effDurations,                           // fcEffectiveDuration
    SgVector*&startSeconds,                           // fcStartSec
    SgVector*&stopSeconds,                            // fcStopSec
    SgVector*&percentOfDiscards,                      // fcDISCARD
    SgVector*&uniformityFactors,                      // fcQBFACTOR
    SgVector*&geocenterPhaseResids,                   // fcGeocResidPhase
    SgVector*&geocenterPhaseTotals,                   // fcGeocPhase
    SgVector*&geocenterSbDelays,                      // fcGeocSBD
    SgVector*&geocenterGrDelays,                      // fcGeocMBD
    SgVector*&geocenterDelayRates,                    // fcGeocRate
    SgVector*&probOfFalses,                           // fcProbFalseDetection
    SgVector*&corrAprioriDelays,                      // fcABASDEL
    SgVector*&corrAprioriRates,                       // fcABASRATE
    SgVector*&corrAprioriAccs,                        // fcABASACCE
    SgVector*&incohAmps,                              // fcINCOHAMP
    SgVector*&incohAmps2,                             // fcINCOH2
    SgVector*&delayResids,                            // fcDELRESID
    QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
    QVector<QString>& tapeIds_1,                      // fcTAPEID
    QVector<QString>& tapeIds_2,                      // fcTAPEID
    QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    QVector< QVector<int> >& epochsOfFourfitings,     // fcFOURFUTC
    QVector< QVector<int> >& epochsOfNominalScanTime, // fcUTCScan
    SgMatrix*&corrClocks,                             // fcCORCLOCK
    SgMatrix*&mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
    SgMatrix*&mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM, 
                                                      // fcUTCVLB2, fcDELOBSVM
  );
  bool loadObsCorrelatorInfoCrl(const QString& band,
    QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//  QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    SgVector*&deltaEpochs,                            // fcDELTAEPO
    SgMatrix*&searchParams,                           // fcSRCHPAR
    QVector<QString>& baselineCodes,                  // fcCORBASCD
    QVector<QString>& tapeQualityCodes,               // fcTapeCode
    SgVector*&sampleRate,                             // fcRECSETUP
    SgVector*&sbdResids,                              // fcSBRESID
    SgVector*&rateResids,                             // fcRATRESID
    SgVector*&effDurations,                           // fcEffectiveDuration
    SgVector*&startSeconds,                           // fcStartSec
    SgVector*&stopSeconds,                            // fcStopSec
    SgVector*&percentOfDiscards,                      // fcDISCARD
    SgVector*&geocenterPhaseResids,                   // fcGeocResidPhase
    SgVector*&geocenterPhaseTotals,                   // fcGeocPhase
    SgVector*&probOfFalses,                           // fcProbFalseDetection
    SgVector*&corrAprioriDelays,                      // fcABASDEL
    SgVector*&corrAprioriRates,                       // fcABASRATE
    SgVector*&corrAprioriAccs,                        // fcABASACCE
    SgVector*&incohAmps,                              // fcINCOHAMP
    SgVector*&incohAmps2,                             // fcINCOH2
    SgVector*&delayResids,                            // fcDELRESID
    QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
    QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    SgMatrix*&mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
    SgMatrix*&mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM, 
                                                      // fcUTCVLB2, fcDELOBSVM
  );
  bool loadObsCorrelatorInfoS2(const QString& band,
    QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//  QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    SgVector*&deltaEpochs,                            // fcDELTAEPO
    SgMatrix*&searchParams,                           // fcSRCHPAR
    QVector<QString>& baselineCodes,                  // fcCORBASCD
    QVector<QString>& tapeQualityCodes,               // fcTapeCode
    SgVector*&sampleRate,                             // fcRECSETUP
    SgVector*&sbdResids,                              // fcSBRESID
    SgVector*&rateResids,                             // fcRATRESID
    SgVector*&effDurations,                           // fcEffectiveDuration
    SgVector*&startSeconds,                           // fcStartSec
    SgVector*&stopSeconds,                            // fcStopSec
    SgVector*&percentOfDiscards,                      // fcDISCARD
    SgVector*&geocenterPhaseResids,                   // fcGeocResidPhase
    SgVector*&geocenterPhaseTotals,                   // fcGeocPhase
    SgVector*&probOfFalses,                           // fcProbFalseDetection
    SgVector*&corrAprioriDelays,                      // fcABASDEL
    SgVector*&corrAprioriRates,                       // fcABASRATE
    SgVector*&corrAprioriAccs,                        // fcABASACCE
    SgVector*&incohAmps,                              // fcINCOHAMP
    SgVector*&incohAmps2,                             // fcINCOH2
    SgVector*&delayResids,                            // fcDELRESID
    QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
    QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    SgMatrix*&mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
    SgMatrix*&mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM, 
                                                      // fcUTCVLB2, fcDELOBSVM
  );
  bool loadObsCorrelatorInfoMk4(const QString& band,
    QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
    QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//  QVector<int>& corelVersions,                      // fcCORELVER
    QVector<int>& startOffsets,                       // fcStartOffset
    QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
//  QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
    SgVector*&deltaEpochs,                            // fcDELTAEPO
    SgMatrix*&urvrs,                                  // fcURVR
    SgMatrix*&instrDelays,                            // fcIDELAY
    SgMatrix*&starElev,                               // fcSTARELEV
    SgMatrix*&zenithDelays,                           // fcZDELAY
    SgMatrix*&searchParams,                           // fcSRCHPAR
    QVector<QString>& baselineCodes,                  // fcCORBASCD
    QVector<QString>& tapeQualityCodes,               // fcTapeCode
    QVector<int>& stopOffsets,                        // fcStopOffset
    QVector<int>& hopsRevisionNumbers,                // fcHopsRevNum
    SgVector*&sbdResids,                              // fcSBRESID
    SgVector*&rateResids,                             // fcRATRESID
    SgVector*&effDurations,                           // fcEffectiveDuration
    SgVector*&startSeconds,                           // fcStartSec
    SgVector*&stopSeconds,                            // fcStopSec
    SgVector*&percentOfDiscards,                      // fcDISCARD
    SgVector*&uniformityFactors,                      // fcQBFACTOR
    SgVector*&geocenterPhaseResids,                   // fcGeocResidPhase
    SgVector*&geocenterPhaseTotals,                   // fcGeocPhase
    SgVector*&geocenterSbDelays,                      // fcGeocSBD
    SgVector*&geocenterGrDelays,                      // fcGeocMBD
    SgVector*&geocenterDelayRates,                    // fcGeocRate
    SgVector*&probOfFalses,                           // fcProbFalseDetection
    SgVector*&corrAprioriDelays,                      // fcABASDEL
    SgVector*&corrAprioriRates,                       // fcABASRATE
    SgVector*&corrAprioriAccs,                        // fcABASACCE
    SgVector*&incohAmps,                              // fcINCOHAMP
    SgVector*&incohAmps2,                             // fcINCOH2
    SgVector*&delayResids,                            // fcDELRESID
    QVector<QString>& tapeIds_1,                      // fcTAPEID
    QVector<QString>& tapeIds_2,                      // fcTAPEID
    QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
    QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
    QVector< QVector<int> >& epochsOfFourfitings,     // fcFOURFUTC
    QVector< QVector<int> >& epochsOfNominalScanTime, // fcUTCScan
    SgMatrix*&corrClocks                              // fcCORCLOCK
  );
  //
  // channel infos:
  bool loadObsChannelInfoMk3(const QString& band,
    QVector<QString>& channelIds,                         // CHAN ID
    QVector<QString>& polarizations,                      // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& bitsPerSamples,                         // BITSAMPL
    QVector< QVector<int> >& errorRates_1,                // ERRORATE @1, phasecal data
    QVector< QVector<int> >& errorRates_2,                // ERRORATE @2, phasecal data
    QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
    QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
    QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
    QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&loFreqs_1,                                  // LO FREQ @1
    SgMatrix*&loFreqs_2,                                  // LO FREQ @2
    SgMatrix*&channelBandwidth,
    SgSidebandOrder &sbOrder
  );
  bool loadObsChannelInfoS2(const QString& band,
    QVector<QString>& channelIds,                         // CHAN ID
    QVector<QString>& polarizations,                      // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& bitsPerSamples,                         // BITSAMPL
    QVector< QVector<int> >& errorRates_1,                // ERRORATE @1, phasecal data
    QVector< QVector<int> >& errorRates_2,                // ERRORATE @2, phasecal data
    QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
    QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
    QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
    QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&loFreqs_1,                                  // LO FREQ @1
    SgMatrix*&loFreqs_2,                                  // LO FREQ @2
    SgMatrix*&channelBandwidth,
    SgSidebandOrder &sbOrder
  );
  bool loadObsChannelInfoMk4(const QString& band,
    QVector<QString>& channelIds,                         // CHAN ID
    QVector<QString>& polarizations,                      // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& bitsPerSamples,                         // BITSAMPL
    QVector< QVector<int> >& errorRates_1,                // ERRORATE @1, phasecal data
    QVector< QVector<int> >& errorRates_2,                // ERRORATE @2, phasecal data
    QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
    QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
    QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
    QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&loFreqs_1,                                  // LO FREQ @1
    SgMatrix*&loFreqs_2,                                  // LO FREQ @2
    SgMatrix*&channelBandwidth,
    SgSidebandOrder &sbOrder
  );
  //
  // phase cal infos:
  bool loadObsPhaseCalInfoMk3(const QString& band,
    SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
    SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
    SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
    SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2);
  bool loadObsPhaseCalInfoS2(const QString& band,
    SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
    SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
    SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
    SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2);
  bool loadObsPhaseCalInfoMk4(const QString& band,
    SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
    SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
    SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
    SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2);

};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb inline members:                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:



// A regular constructor:



// A destructor:



//
// INTERFACES:
//



//
// FUNCTIONS:
//
//
//
inline bool SgVgosDb::hasXBandData()
{
  if (bandDataByName_.contains("X"))
    return true;
  else 
    return false;
};



//
inline bool SgVgosDb::hasSBandData()
{
  if (bandDataByName_.contains("S"))
    return true;
  else 
    return false;
};



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
//
// const:
//
// Variables:
extern SgVgosDb::FmtChkVar      fcSecond, fcYmdhm, fcCalIonoGroup, fcCalIonoGroupSigma, 
  fcCalIonoGroupDataFlag, fcIonoBits, fcDelayFlag, fcRateFlag, fcPhaseFlag, fcUserSup, fcNumGroupAmbig,
  fcNumPhaseAmbig, fcNumGroupSubAmbig,
  fcAtmInterval, fcAtmRateConstraint, fcAtmRateSite, fcClockInterval, fcClockRateConstraint, 
  fcClockRateName, fcPmOffsetConstraint, fcUtOffsetConstraint, fcIonoSolveFlag, fcIonoStationList,
  fcGroupBLWeights, fcGroupBLWeightName, fcSrcSelectionFlag, fcBlnSelectionFlag,
  fcSrcSelectionName, fcBlnSelectionName, fcBaselineClock, fcEccentricityType, fcEccentricityName,
  fcEccentricityMonument, fcEccentricityVector, fcObsCalFlag, fcStatCalFlag, fcFlybyFlag, fcStatCalName,
  fcFlybyName, fcCalStationName, fcObsCalName, fcClockBreakFlag, fcClockBreakStatus, fcClockBreakNumber,
  fcClockBreakSite, fcClockBreakEpoch, fcTai_Utc, fcLeapSecond, fcCableSign, fcCableStnName,
  fcFourFitCmdCString_v1002, fcNumLagsUsed_v1002, fcFourfitControlFile_v1002, fcAplength_v1002, 
  fcFourFitCmdCString_v1004, fcNumLagsUsed_v1004, fcFourfitControlFile_v1004, fcAplength_v1004,
  fcCorrPostProcSoftware,
  fcFractC
  ;
extern SgVgosDb::FmtChkVar      fcBaseline, fcSource, fcObs2Baseline, fcObs2Scan, fcCrossRefSourceList,
  fcScan2Source, fcNumSource, fcNumStation, fcExpName, fcExpSerialNumber, fcExpDescription,
  fcRecordingMode,
  fcNumScansPerStation, fcCrossRefStationList, fcStation2Scan, fcScan2Station, 
  fcSBDelay, fcSBDelaySig, fcGroupDelay, fcGroupDelaySig, fcGroupRate, fcGroupRateSig, 
  fcPhase, fcPhaseSig, fcGroupDelayFull, fcPhaseDelayFull, fcPhaseDelaySigFull, fcRefFreq, fcDtec, 
  fcDtecStdErr, fcFreqRateIon, fcFreqPhaseIon, fcFreqGroupIon, 
  fcFreqRateIonEqWgts, fcFreqPhaseIonEqWgts, fcFreqGroupIonEqWgts,  
  fcAmbigSize, fcQualityCode, fcCorrelation,
  fcSNR, fcDelayDataFlag, fcDataFlag, fcDelayTheoretical, fcRateTheoretical, fcUVFperAsec,
  fcNumChannels, fcSampleRate, fcFOURFFIL, fcNumAp, fcNumAccum, fcSidebandOrder,
  fcPhaseCalFreq, fcPhaseCalAmp, fcPhaseCalPhase, fcPhaseCalOffset, fcPhaseCalRate,
  fcPhaseCalFreqS2, fcPhaseCalPhaseS2, fcPhaseCalAmpS2,
  fcChannelFreq, fcVIRTFREQ, fcNumSamples, fcChanAmpPhase, fcVFRQAM, fcVFRQPH,
  fcCalEarthTide,  fcCalFeedCorrection, fcCalUt1Ortho, fcCalWobOrtho, fcCalOceanLoad, fcCalOceanLoadOld,
  fcCalPoleTide, fcCalOceanPoleTideLoad,
  fcCalPoleTideOldRestore, fcCalTiltRemover, fcCalUnPhase, fcCalHfLibration,
  fcCalHiFreqLibrationPm, fcCalHiFreqLibrationUt,
  fcCalXwobble,fcCalYwobble, fcCalBend, fcCalBendSun, fcCalBendSunHigher, fcCalParallax, fcPartBend,
  fcPartWobble, fcPartUt1, fcPartGamma, fcPartNutationNro, fcPartRaDec, fcPartXyz, fcPartParallax,
  fcPartPolarTide, fcUserCorrections
  ;
extern SgVgosDb::FmtChkVar      fcFRNGERR, fcFRNGERR4S2, fcFRQGROUP, fcCORELVER, fcStartOffset,
  fcBITSAMPL, fcFOURFVER,
  fcDBEDITVE, fcREFCLKER, fcDELTAEPO, fcURVR, fcIDELAY, fcSTARELEV, fcZDELAY, fcSRCHPAR, fcCORBASCD,
  fcStopOffset, fcHopsRevNum, fcTapeCode, fcSBRESID, fcRATRESID, fcEffectiveDuration, fcStartSec,
  fcDISCARD, fcQBFACTOR, fcGeocResidPhase, fcGeocSBD, fcGeocRate, fcGeocMBD, fcProbFalseDetection,
  fcABASRATE, fcABASDEL, fcABASACCE, fcStopSec, fcGeocPhase, fcINCOHAMP, fcINCOH2, fcDELRESID,
  fcUTCCorr, fcUTCMidObs, fcFOURFUTC, fcUTCScan, fcTAPEID, fcCORCLOCK,
  fcBBCIndex, fcChannelID, fcPolarization, fcINDEXNUM, fcERRORATE, fcLOFreq, fcChannelBandwidth
  ;
extern SgVgosDb::FmtChkVar      fcRECSETUP, fcVLB1FILE, fcDLYEPOp1, fcDLYEPOCH, fcDLYEPOm1, fcAPCLOFST,
  fcTOTPCENT, fcRATOBSVM, fcUTCVLB2, fcDELOBSVM, fcUTCErr, fcVFDWELL, fcS2EFFREQ, fcS2REFREQ,
  fcS2PHEFRQ, fcTotalFringeErr, fcDURATION, fcTimeSinceStart, fcSunXyz, fcMoonXyz, fcEarthXyz,
  //
  fcDLYEPOp1_V2, fcDLYEPOCH_V2, fcDLYEPOm1_V2
  ;
// Scan:
extern SgVgosDb::FmtChkVar      fcScanName, fcScanNameInput, fcScanNameFull, fcNutationEqxWahr,
  fcNutationEqx, fcNutationNro, fcCROOTFIL, fcMjd, fcDayFrac, fcRotCf2J2K, fcUt1_Tai, fcPolarMotion
  ;
// Station:
extern SgVgosDb::FmtChkVar      fcAzTheo, fcElTheo, fcFeedRotation, fcCalAxisOffset, fcCalCable,
  fcCalCableCorrections, fcCalCblCorrections,
  fcCalSlantPathTropDry, fcCalSlantPathTropWet, fcCalStationOceanLoadVert, fcCalStationOceanLoadHoriz,
  fcCalUnPhaseCal, fcTempC, fcAtmPres, fcRelHum, fcPartAxisOffset, fcDisOceanLoad,
  fcPartZenithPathTropDry, fcPartZenithPathTropWet, fcPartHorizonGrad, fcRefClockOffset, 
  fcTsysData, fcTsysIfFreq, fcTsysIfId, fcTsysIfSideBand, fcTsysIfPolarization

  ;
// Session:
extern SgVgosDb::FmtChkVar      fcCorrelatorType, fcCorrelator, fcNumObs, fcNumScan, 
  fcPrincipalInvestigator, fcStationList, fcSourceList, fciUTCInterval, fcTidalUt1Control, fcCalcVersion,
  fcCalcControlNames, fcCalcControlValues, fcATMMessage, fcATMControl, fcAxisOffsetMessage,
  fcAxisOffsetControl, fcFeedhornMessage, fcEarthTideMessage, fcEarthTideControl,
  fcPoleTideMessage, fcPoleTideControl, fcNutationMessage, fcNutationControl, 
  fcOceanMessage, fcOceanControl, fcATIMessage, fcATIControl, fcCTIMessage, fcCTIControl,
  fcParallaxMessage, fcParallaxControl, fcStarMessage, fcStarControl, 
  fcTheoryMessage, fcRelativityControl,
  fcSiteMessage, fcFeedhornMessage, fcPepMessage, fcWobbleControl, fcUT1Control, fcOceanStationsFlag,
  fcRelativityData, fcPrecessionData, fcEarthTideData, fcUT1EPOCH, fcWOBEPOCH, fcSiteZenithDelay,
  fcOceanPoleTideCoef, fcOceanUpPhase, fcOceanHorizontalPhase, fcOceanUpAmp, fcOceanHorizontalAmp,
  fcWobArrayInfo, fcUT1ArrayInfo, fcUT1Values, fcWobValues, fcUT1IntrpMode, fcCalcUt1Module,
  fcUT1Origin, fcCalcWobModule, fcWobIntrpMode, fcWobbleOrigin,
  fcSourceNameApriori, fcSource2000RaDec, fcSourceReference,
  fcClockAprioriSite, fcClockAprioriOffset, fcClockAprioriRate,
  fcStationNameApriori, fcStationXYZ, 
  fcStationPlateName, fcAxisType, fcAntennaName, fcAxisOffset, fcAxisTilt
  
  ;
  
  





// netCDF files:
extern QList<SgVgosDb::FmtChkVar*>
                                fcfTimeUTC, fcfCalSlantPathIonoGroup, fcfIonoBits, *fcfEdit, fcfUserSup,
  fcfEdit_v1001, fcfEdit_v1002,
  fcfNumGroupAmbig, fcfNumGroupAmbigE, fcfNumPhaseAmbig,
  fcfAtmSetup, fcfClockSetup, fcfErpSetup, fcfIonoSetup, 
  fcfGroupBLWeights, fcfSelectionStatus, fcfBlnClockSetup,
  fcfEccentricity, fcfCalibrationSetup, fcfClockBreak, fcfLeapSecondIn, fcfLeapSecond, fcfMiscCable, 
  fcfMiscFourFit_v1002, fcfMiscFourFit_v1004, fcfFractC
  ;
extern QList<SgVgosDb::FmtChkVar*> 
                                fcfBaseline, fcfSource, fcfObsCrossRef, fcfSourceCrossRef, 
  fcfStationCrossRef, fcfSBDelay, fcfGroupDelay, fcfGroupRate, fcfPhase, fcfGroupDelayFull, 
  fcfPhaseDelayFull, fcfRefFreq, fcfEffFreq, fcfEffFreqEqWgts, fcfDTec,
  fcfAmbigSize, fcfQualityCode, fcfCorrelation, fcfUVFperAsec, fcfSNR, fcfDelayDataFlag, fcfDataFlag, 
  fcfDelayTheoretical, fcfRateTheoretical, 
//  
//expected format for input:
  fcfChannelInfoInptMk3, fcfChannelInfoInptMk4, fcfChannelInfoInptS2, fcfChannelInfoInptCRL, 
  fcfChannelInfoInptGSI, fcfChannelInfoInptVLBA, fcfChannelInfoInptHaystack, fcfChannelInfoInptDifx,
//format for output:
  fcfChannelInfoMk4, fcfChannelInfoKomb, 
//expected format for input:
  fcfPhaseCalInfoMk3, fcfPhaseCalInfoVLBA, fcfPhaseCalInfoCRL, fcfPhaseCalInfoGSI,
  fcfPhaseCalInfoS2, fcfPhaseCalInfoMk4, fcfPhaseCalInfoHaystack, fcfPhaseCalInfoDifx,
//format for output:
  fcfPhaseCalInfo,
//expected format for input:
  fcfCorrInfoInptMk3, fcfCorrInfoInptMk4, fcfCorrInfoInptS2, fcfCorrInfoInptCRL, fcfCorrInfoInptGSI, 
  fcfCorrInfoInptVLBA, fcfCorrInfoInptHaystack, fcfCorrInfoInptDifx,
//format for output:
  fcfCorrInfoMk4, fcfCorrInfoKomb, fcfCorrInfoKomb_V2,
  //
  fcfCalEarthTide, fcfCalFeedCorrection, fcfCalHiFyErp, fcfCalOceanLoad, fcfCalOceanLoadOld, 
  fcfCalPoleTide, fcfCalPoleTideOldRestore, fcfCalOceanPoleTideLoad, fcfCalTiltRemover, fcfCalUnphase,
  fcfCalHfLibration, fcfCalHiFreqLibration, fcfCalWobble, fcfCalBend, fcfCalBendSun,
  fcfCalBendSunHigher, fcfCalParallax, fcfPart_Bend, fcfPart_Erp, fcfPart_Gamma, fcfPart_NutationNro,
  fcfPart_RaDec, fcfPart_Xyz, fcfPart_Parallax, fcfPart_PolarTide, fcfUserCorrections
  ;
// Scan:
extern QList<SgVgosDb::FmtChkVar*> 
  fcfScanName, fcfScanNameInput, fcfNutationEqxWahr, fcfNutationEqx, 
  fcfNutationNro, fcfCorrRootFile, fcfScanTimeMjd, fcfEphemeris, fcfRotCf2J2K, fcfErpApriori
  ;
// Station:
extern QList<SgVgosDb::FmtChkVar*> 
  fcfAzEl, fcfFeedRotation, fcfCalAxisOffset, fcfCalCable, fcfCalCableCorrections, fcfCalCblCorrections, 
  fcfCalSlantPathTropDry, fcfCalSlantPathTropWet, fcfCalStationOceanLoad, fcfCalUnPhaseCal, 
  fcfMet, fcfPartAxisOffset, fcfDisOceanLoad, fcfPartZenithPathTropDry, fcfPartZenithPathTropWet,
  fcfPartHorizonGrad, fcfRefClockOffset, fcfTsys
  ;

// Session:
extern QList<SgVgosDb::FmtChkVar*>
  fcfHead, fcfCalcInfo, fcfCalcEop, fcfSourceApriori, fcfClockApriori, fcfStationApriori, 
  fcfAntennaApriori;
  ;


/*=====================================================================================================*/
#endif //SG_OPEN_DB_H
