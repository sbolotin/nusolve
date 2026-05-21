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

#ifndef SG_VLBI_OBSERVABLE_H
#define SG_VLBI_OBSERVABLE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QList>
#include <QtCore/QString>

class QDataStream;


#include <SgVlbiMeasurement.h>


class SgMatrix;
class SgTaskConfig;
class SgVector;
class SgVlbiBand;
class SgVlbiBaselineInfo;
class SgVlbiObservation;
class SgVlbiStationInfo;
class SgVlbiSourceInfo;


/***===================================================================================================*/
/**
 * A class that represents a VLBI observation from one band (quasi-measurements plus additional info).
 *
 */
/**====================================================================================================*/
class SgVlbiObservable
{
public:
  //
  enum PhaseCalContentIndex
  {
    PCCI_AMPLITUDE      =  0,
    PCCI_PHASE          =  1,
    PCCI_FREQUENCY      =  2,
    PCCI_OFFSET         =  3,
    PCCI_RATE           =  4
  };
  enum NonUsableReason
  {
    NUR_LOW_QF              =  1<< 0,
    NUR_ONE_CHANNEL         =  1<< 1,
    NUR_DESELECTED_OBS      =  1<< 2,
    NUR_DESELECTED_SOURCE   =  1<< 3,
    NUR_DESELECTED_STATION  =  1<< 4,
    NUR_DESELECTED_BASELINE =  1<< 5,
    NUR_HAS_ERROR_CODE      =  1<< 6,
    NUR_TOO_EARLY           =  1<< 7,
    NUR_TOO_LATE            =  1<< 8,
    NUR_UNMATED             =  1<< 9,
    NUR_MATE_DESELECTED_OBS =  1<<10,
    NUR_MATE_LOW_QF         =  1<<11,
    NUR_MATE_ONE_CHANNEL    =  1<<12,
    NUR_MATE_HAS_ERROR_CODE =  1<<13,
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgVlbiObservable(SgVlbiObservation*);

  SgVlbiObservable(SgVlbiObservation*, SgVlbiBand*);

  SgVlbiObservable(SgVlbiObservation*, const SgVlbiObservable&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiObservable();



  //
  // Interfaces:
  //
  // gets:
  /**
   */
  inline const QString& getBandKey() const;

  /**
   */
  inline int getMediaIdx() const;

  /**
   */
  inline SgVlbiObservation* owner();

  /**
   */
  inline SgVlbiBand* band();

  /**
   */
  inline SgVlbiBaselineInfo* &baseline();

  /**
   */
  inline SgVlbiStationInfo* &stn_1();

  /**
   */
  inline SgVlbiStationInfo* &stn_2();

  /**
   */
  inline SgVlbiSourceInfo* &src();

  /**
   */
  inline SgVlbiBaselineInfo* getBaseline() const;

  /**
   */
  inline SgVlbiStationInfo* getStn_1() const;

  /**
   */
  inline SgVlbiStationInfo* getStn_2() const;

  /**
   */
  inline SgVlbiSourceInfo* getSrc() const;

  /**
   */
  inline SgVlbiMeasurement& sbDelay();

  /**
   */
  inline SgVlbiMeasurement& grDelay();

  /**
   */
  inline SgVlbiMeasurement& phDelay();

  /**
   */
  inline SgVlbiMeasurement& phDRate();

  /**
   */
  inline const QString& getErrorCode() const;

  /**
   */
  inline int getQualityFactor() const;

  /**
   */
  inline double getReferenceFrequency() const;

  /**
   */
  inline double getTotalPhase() const;

  /**
   */
  inline double getCorrCoeff() const;

  /**
   */
  inline double getSnr() const;

  /**
   */
  inline int getNumOfChannels() const;

  inline SgVector* numOfAccPeriodsByChan_USB() {return numOfAccPeriodsByChan_USB_;};
  inline SgVector* numOfAccPeriodsByChan_LSB() {return numOfAccPeriodsByChan_LSB_;};
  inline SgVector* numOfSamplesByChan_USB() {return numOfSamplesByChan_USB_;};
  inline SgVector* numOfSamplesByChan_LSB() {return numOfSamplesByChan_LSB_;};
  inline SgVector* refFreqByChan() {return refFreqByChan_;};
  inline SgVector* fringeAmplitudeByChan() {return fringeAmplitudeByChan_;};
  inline SgVector* fringePhaseByChan() {return fringePhaseByChan_;};
  inline SgVector* channelBandwidth() {return channelBandwidth_;};
  inline QVector<char>* polarization_1ByChan() {return polarization_1ByChan_;};
  inline QVector<char>* polarization_2ByChan() {return polarization_2ByChan_;};
  inline SgMatrix* phaseCalData_1ByChan() {return phaseCalData_1ByChan_;};
  inline SgMatrix* phaseCalData_2ByChan() {return phaseCalData_2ByChan_;};
  inline int getPhaseCalModes() const {return phaseCalModes_;};
  inline SgVector* vDlys() {return vDlys_;};
  inline SgVector* vAuxData() {return vAuxData_;};
  inline QVector<int>* corelIndexNumUSB() {return corelIndexNumUSB_;};
  inline QVector<int>* corelIndexNumLSB() {return corelIndexNumLSB_;};

  inline QVector<char>* chanIdByChan() {return chanIdByChan_;};
  inline SgVector* loFreqByChan_1() {return loFreqByChan_1_;};
  inline SgVector* loFreqByChan_2() {return loFreqByChan_2_;};
  inline QVector<int>* bbcIdxByChan_1() {return bbcIdxByChan_1_;};
  inline QVector<int>* bbcIdxByChan_2() {return bbcIdxByChan_2_;};

  inline const SgVector* numOfAccPeriodsByChan_USB() const {return numOfAccPeriodsByChan_USB_;};
  inline const SgVector* numOfAccPeriodsByChan_LSB() const {return numOfAccPeriodsByChan_LSB_;};
  inline const SgVector* numOfSamplesByChan_USB() const {return numOfSamplesByChan_USB_;};
  inline const SgVector* numOfSamplesByChan_LSB() const {return numOfSamplesByChan_LSB_;};
  inline const SgVector* refFreqByChan() const {return refFreqByChan_;};
  inline const SgVector* fringeAmplitudeByChan() const {return fringeAmplitudeByChan_;};
  inline const SgVector* fringePhaseByChan() const {return fringePhaseByChan_;};
  inline const SgVector* channelBandwidth() const {return channelBandwidth_;};
  inline const QVector<char>* polarization_1ByChan() const {return polarization_1ByChan_;};
  inline const QVector<char>* polarization_2ByChan() const {return polarization_2ByChan_;};
  inline const SgMatrix* phaseCalData_1ByChan() const {return phaseCalData_1ByChan_;};
  inline const SgMatrix* phaseCalData_2ByChan() const {return phaseCalData_2ByChan_;};
  inline const SgVector* vDlys() const {return vDlys_;};
  inline const SgVector* vAuxData() const {return vAuxData_;};
  inline const QVector<int>* corelIndexNumUSB() const {return corelIndexNumUSB_;};
  inline const QVector<int>* corelIndexNumLSB() const {return corelIndexNumLSB_;};

  inline const QVector<char>* chanIdByChan() const {return chanIdByChan_;};
  inline const SgVector* loFreqByChan_1() const {return loFreqByChan_1_;};
  inline const SgVector* loFreqByChan_2() const {return loFreqByChan_2_;};
  inline const QVector<int>* bbcIdxByChan_1() const {return bbcIdxByChan_1_;};
  inline const QVector<int>* bbcIdxByChan_2() const {return bbcIdxByChan_2_;};

  inline double getPhaseCalGrDelays(int i) const {return phaseCalGrDelays_[-1<i&&i<2?i:0];};
  inline double getPhaseCalPhDelays(int i) const {return phaseCalPhDelays_[-1<i&&i<2?i:0];};
  inline double getPhaseCalGrAmbigSpacings(int i) const 
    {return phaseCalGrAmbigSpacings_[-1<i&&i<2?i:0];};
  inline double getPhaseCalPhAmbigSpacings(int i) const
    {return phaseCalPhAmbigSpacings_[-1<i&&i<2?i:0];};
  inline int getPhaseCalGrAmbigMultipliers(int i) const 
    {return phaseCalGrAmbigMultipliers_[-1<i&&i<2?i:0];};
  inline int getPhaseCalPhAmbigMultipliers(int i) const
    {return phaseCalPhAmbigMultipliers_[-1<i&&i<2?i:0];};  
  inline double getPhaseCalRates(int i) const {return phaseCalRates_[-1<i&&i<2?i:0];};
  inline double getUvFrPerAsec(int i) const {return uvFrPerAsec_[-1<i&&i<2?i:0];};
  inline double getUrVr(int i) const {return uRvR_[-1<i&&i<2?i:0];};
  inline double getCorrClocks(int i, int j) const {return corrClocks_[-1<i&&i<2?i:0][-1<j&&j<2?j:0];};
  inline double getInstrDelay(int i) const {return instrDelay_[-1<i&&i<2?i:0];};
  inline int getNlags() const {return nLags_;};
  inline double getFourfitSearchParameters(int i) const {return fourfitSearchParameters_[-1<i&&i<6?i:0];};
  inline int getHopsRevisionNumber() const {return hopsRevisionNumber_;};



  /**
   */
  inline const QString& getFourfitOutputFName() const;
  inline int getFourfitVersion(int idx) const {return fourfitVersion_[-1<idx&&idx<2?idx:0];};
  inline const QString& getFourfitControlFile() const {return fourfitControlFile_;};
  inline const QString& getFourfitCommandOverride() const {return fourfitCommandOverride_;};
  
  inline const SgMJD& getEpochOfCorrelation() const;
  inline const SgMJD& getEpochOfFourfitting() const;
  inline const SgMJD& getEpochOfScan() const;
  inline const SgMJD& getEpochCentral() const;
  inline const QString& getTapeQualityCode() const {return tapeQualityCode_;};
  inline const SgMJD& getTstart() const {return tStart_;};
  inline const SgMJD& getTstop() const {return tStop_;};

  inline int getStartOffset() const {return startOffset_;};
  inline int getStopOffset() const {return stopOffset_;};
  inline double getCentrOffset() const {return centrOffset_;};
  inline double getSampleRate() const {return sampleRate_;};
  inline int getBitsPerSample() const {return bitsPerSample_;};
  inline double getEffIntegrationTime() const {return effIntegrationTime_;};
  inline double getAcceptedRatio() const {return acceptedRatio_;};
  inline double getDiscardRatio() const {return discardRatio_;};
  inline double getIncohChanAddAmp() const {return incohChanAddAmp_;};
  inline double getIncohSegmAddAmp() const {return  incohSegmAddAmp_;};
  inline double getProbabOfFalseDetection() const {return  probabOfFalseDetection_;};
  inline double getGeocenterTotalPhase() const {return  geocenterTotalPhase_;};
  inline double getGeocenterResidPhase() const {return  geocenterResidPhase_ ;};
  inline double getAprioriDra(int i) const {return  aPrioriDra_[-1<i&&i<3?i:0];};
  inline double getCalcFeedCorrDelay() const {return calcFeedCorrDelay_;};
  inline double getCalcFeedCorrRate () const {return calcFeedCorrRate_;};

  inline double getCorrelStarElev_1() const {return correlStarElev_1_;};
  inline double getCorrelStarElev_2() const {return correlStarElev_2_;};
  inline double getCorrelZdelay_1  () const {return correlZdelay_1_;};
  inline double getCorrelZdelay_2  () const {return correlZdelay_2_;};
  


  inline SgVlbiMeasurement* activeDelay();  // can be NULL
  inline SgVlbiMeasurement* activeRate();   // can be NULL
  inline SgVlbiMeasurement* activeMeasurement();   // can be NULL


  // temporary/tests:
  inline double getSbdDiffBand() const;
  inline double getGrdDiffBand() const;
  inline double getPhrDiffBand() const;

  inline double getTauS() const {return dTauS_;};
  inline double getTest() const {return dTest_;};
  inline double getTest2() const {return dTest2_;};


  inline bool getIsActive() const {return isActive_;};



  // sets:
  /**
   */
  inline void setBandKey(const QString&);

  /**
   */
  inline void setMediaIdx(int);

  /**
   */
  inline void setErrorCode(const QString&);

  /**
   */
  inline void setQualityFactor(int);

  /**
   */
  inline void setReferenceFrequency(double);

  /**
   */
  inline void setTotalPhase(double);

  /**
   */
  inline void setCorrCoeff(double);

  /**
   */
  inline void setSnr(double);

  /**
   */
  inline void setNumOfChannels(int);

  /**
   */
  inline void setFourfitOutputFName(const QString&);
  inline void setFourfitVersion(int idx, int ver) {fourfitVersion_[-1<idx&&idx<2?idx:0]=ver;};
  inline void setFourfitControlFile(const QString& cf) {fourfitControlFile_=cf;};
  inline void setFourfitCommandOverride(const QString& co) {fourfitCommandOverride_=co;};
  
  inline void setEpochOfCorrelation(const SgMJD&);
  inline void setEpochOfFourfitting(const SgMJD&);
  inline void setEpochOfScan(const SgMJD&);
  inline void setEpochCentral(const SgMJD&);
  inline void setTapeQualityCode(const QString& code) {tapeQualityCode_ = code;};
  
  inline void setTstart(const SgMJD& t) {tStart_=t;};
  inline void setTstop(const SgMJD& t) {tStop_=t;};

  inline void setStartOffset(int offset) {startOffset_=offset;};
  inline void setStopOffset(int offset) {stopOffset_=offset;};
  inline void setCentrOffset(double offset) {centrOffset_=offset;};
  inline void setSampleRate(double sr) {sampleRate_=sr;};
  inline void setBitsPerSample(int bps) {bitsPerSample_=bps;};
  inline void setEffIntegrationTime(double d) {effIntegrationTime_=d;};
  inline void setAcceptedRatio(double d) {acceptedRatio_=d;};
  inline void setDiscardRatio(double d) {discardRatio_=d;};
  inline void setIncohChanAddAmp(double d) {incohChanAddAmp_=d;};
  inline void setIncohSegmAddAmp(double d) {incohSegmAddAmp_=d;};
  inline void setProbabOfFalseDetection(double d) {probabOfFalseDetection_=d;};
  inline void setGeocenterTotalPhase(double d) {geocenterTotalPhase_=d;};
  inline void setGeocenterResidPhase(double d) {geocenterResidPhase_=d;};
  inline void setAprioriDra(int i, double d) {aPrioriDra_[-1<i&&i<3?i:0]=d;};
  inline void setCalcFeedCorrDelay(double v) {calcFeedCorrDelay_ = v;};
  inline void setCalcFeedCorrRate (double v) {calcFeedCorrRate_ = v;};
  inline void setCorrelStarElev_1(double v) {correlStarElev_1_ = v;};
  inline void setCorrelStarElev_2(double v) {correlStarElev_2_ = v;};
  inline void setCorrelZdelay_1  (double v) {correlZdelay_1_   = v;};
  inline void setCorrelZdelay_2  (double v) {correlZdelay_2_   = v;};
 
  
  inline void setPhaseCalGrDelays(int i, double d) {phaseCalGrDelays_[-1<i&&i<2?i:0]=d;};
  inline void setPhaseCalPhDelays(int i, double d) {phaseCalPhDelays_[-1<i&&i<2?i:0]=d;};
  inline void setPhaseCalGrAmbigSpacings(int i, double d) {phaseCalGrAmbigSpacings_[-1<i&&i<2?i:0]=d;};
  inline void setPhaseCalPhAmbigSpacings(int i, double d) {phaseCalPhAmbigSpacings_[-1<i&&i<2?i:0]=d;};
  inline void setPhaseCalGrAmbigMultipliers(int i, int n)
    {phaseCalGrAmbigMultipliers_[-1<i&&i<2?i:0]=n;};
  inline void setPhaseCalPhAmbigMultipliers(int i, int n)
    {phaseCalPhAmbigMultipliers_[-1<i&&i<2?i:0]=n;};
  
  inline void setPhaseCalRates(int i, double d) {phaseCalRates_[-1<i&&i<2?i:0]=d;};
  inline void setUvFrPerAsec(int i, double d)  {uvFrPerAsec_[-1<i&&i<2?i:0]=d;};
  inline void setUrVr(int i, double d)  {uRvR_[-1<i&&i<2?i:0]=d;};
  inline void setCorrClocks(int i, int j, double d) {corrClocks_[-1<i&&i<2?i:0][-1<j&&j<2?j:0]=d;};
  inline void setInstrDelay(int i, double d) {instrDelay_[-1<i&&i<2?i:0]=d;};
  inline void setNlags(int n) {nLags_=n;};
  inline void setFourfitSearchParameters(int i, double d) {fourfitSearchParameters_[-1<i&&i<6?i:0]=d;};
  inline void setHopsRevisionNumber(int num) {hopsRevisionNumber_=num;};

  // temporary/tests:
  inline void setSbdDiffBand(double);
  inline void setGrdDiffBand(double);
  inline void setPhrDiffBand(double);
//  inline void setSbdQ(double);
//  inline void setGrdQ(double);
//  inline void setPhrQ(double);
  inline void setTauS(double d) {dTauS_ = d;};
  inline void setTest(double d) {dTest_ = d;};
  inline void setTest2(double d) {dTest2_ = d;};
  
  inline void setPhaseCalModes(int modes) {phaseCalModes_=modes;};
  inline void setIsUsable(bool is) {isUsable_=is;};

  inline void setIsActive(bool is) {isActive_ = is;};

  //
  // Functions:
  //
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**
   */
  bool selfCheck();


  inline bool isUsable() const {return isUsable_;};
  inline const SgAttribute& nonUsableReason() const {return nonUsableReason_;};
  inline SgAttribute& nonUsableReason() {return nonUsableReason_;};
  
  inline void zerofy();

  inline void zerofyIono();
    
  void setupActiveMeasurements(const SgTaskConfig*);
  
  const SgMJD& epoch() const;

  //SgVlbiMeasurement* measurement(const SgTaskConfig*);


  int errorCode2Int() const;
  
  //int qualityFactor() const;
  
  void resetAllEditings();

  QString strId() const;
  
  void calcPhaseDelay(bool isAmbigResolved);
  void calcPhaseCalDelay();
  
  void allocateChannelsSetupStorages(int numOfChans);
  void releaseChannelsSetupStorages();
  
  void checkUsability(const SgTaskConfig*);

  inline void propagateAttr(uint attr);
  inline void eradicateAttr(uint attr);
  inline void eradicateAttr(uint attr, SgTaskConfig::VlbiDelayType tp);
  inline void eradicateAttr(uint attr, SgTaskConfig::VlbiRateType tp);
  
  void propagateChannelBandwidth(double bw);


  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //
  
private:
  //
  // band id:
  QString                       bandKey_;
  int                           mediaIdx_;                // DBH idx
  SgVlbiObservation            *owner_;
  SgVlbiBand                   *band_;
  SgVlbiBaselineInfo           *baseline_;
  SgVlbiStationInfo            *stn_1_;
  SgVlbiStationInfo            *stn_2_;
  SgVlbiSourceInfo             *src_;
  //
  // measurements:
  SgVlbiMeasurement             sbDelay_;
  SgVlbiMeasurement             grDelay_;
  SgVlbiMeasurement             phDelay_;
  SgVlbiMeasurement             phDRate_;
  // characteristics of the observable:
  QString                       errorCode_;
  int                           qualityFactor_;
  QString                       tapeQualityCode_;
  double                        referenceFrequency_;
  double                        totalPhase_;
  double                        corrCoeff_;
  double                        snr_;
  int                           numOfChannels_;
  bool                          isUsable_;
  SgAttribute                   nonUsableReason_;
  // about data origin
  QString                       fourfitOutputFName_;
  int                           fourfitVersion_[2];
  QString                       fourfitControlFile_;
  QString                       fourfitCommandOverride_;
  SgMJD                         epochOfCorrelation_;
  SgMJD                         epochOfFourfitting_;
  SgMJD                         epochOfScan_;
  SgMJD                         epochCentral_;
  //
  int                           startOffset_; // type_200.h: "Nom. bline start rel. to scantime (s)"
  int                           stopOffset_;  // type_200.h: "Nom. bline stop rel. to scantime (s)"
  double                        centrOffset_; // type_205.h: "Offset of FRT from scan ctr sec "
  SgMJD                         tStart_;      // type_205.h: "Start of requested data span"
  SgMJD                         tStop_;       // type_205.h: "End of requested data span"
  double                        sampleRate_;
  int                           bitsPerSample_;
  double                        effIntegrationTime_;// type_206.h: "Effective integration time (sec)"
  double                        acceptedRatio_;     // type_206.h: "% ratio min/max data accepted"
  double                        discardRatio_;      // type_206.h: "% data discarded"
  double                        incohChanAddAmp_;
  double                        incohSegmAddAmp_;
  double                        probabOfFalseDetection_;
  double                        geocenterTotalPhase_;
  double                        geocenterResidPhase_;
  double                        aPrioriDra_[3];       // a priori delay, rate and acceleration, from FRNG
  double                        calcFeedCorrDelay_;
  double                        calcFeedCorrRate_;
  double                        correlStarElev_1_;
  double                        correlStarElev_2_;
  double                        correlZdelay_1_;
  double                        correlZdelay_2_;
  //
  //
  // For I/O purposes:
  // channels set up:
  SgVector                     *numOfAccPeriodsByChan_USB_, *numOfAccPeriodsByChan_LSB_;
  SgVector                     *numOfSamplesByChan_USB_, *numOfSamplesByChan_LSB_;
  SgVector                     *refFreqByChan_;
  SgVector                     *fringeAmplitudeByChan_;
  SgVector                     *fringePhaseByChan_;
  SgVector                     *channelBandwidth_;
  QVector<char>                *polarization_1ByChan_, *polarization_2ByChan_;
  QVector<int>                 *corelIndexNumUSB_, *corelIndexNumLSB_;
  //
  QVector<char>                *chanIdByChan_;
  SgVector                     *loFreqByChan_1_, *loFreqByChan_2_;
  QVector<int>                 *bbcIdxByChan_1_, *bbcIdxByChan_2_;
  //
  double                        phaseCalGrDelays_[2];           // for reference and remote stations
  double                        phaseCalPhDelays_[2];           // for reference and remote stations
  double                        phaseCalGrAmbigSpacings_[2];    // for reference and remote stations
  double                        phaseCalPhAmbigSpacings_[2];    // for reference and remote stations
  int                           phaseCalGrAmbigMultipliers_[2]; // for reference and remote stations
  int                           phaseCalPhAmbigMultipliers_[2]; // for reference and remote stations
  double                        phaseCalRates_[2];    // 
  double                        uvFrPerAsec_[2];
  double                        uRvR_[2];
  double                        corrClocks_[2][2];
  double                        instrDelay_[2];
  int                           nLags_;
  double                        fourfitSearchParameters_[6];
  int                           hopsRevisionNumber_;
  //
  // arrays for phase calibrations:
  // phase cal data: amplitude, phase, freq, offset and error rate:
  SgMatrix                     *phaseCalData_1ByChan_; // data for the station #1
  SgMatrix                     *phaseCalData_2ByChan_; // the same for the station #2
  int                           phaseCalModes_;
  // 4 KOMB-type files:
  SgVector                     *vDlys_;       // array of 3: phaseDelay@{t-1sec, t, t+1sec,}
  SgVector                     *vAuxData_;    // array of 5: a storage for APCLOFST, DELTAEPO, TOTPCENT,
                                              // RATOBSVM and DELOBSVM LCODEs

  // end of I/O stuff
  
  // 
  SgVlbiMeasurement            *activeDelay_;
  SgVlbiMeasurement            *activeRate_;
  SgVlbiMeasurement            *activeMeasurement_;
  
  
  //
  // temporary/testing:
  double                        sbdDiffBand_;
  double                        grdDiffBand_;
  double                        phrDiffBand_;
  double                        sbdQ_;
  double                        grdQ_;
  double                        phrQ_;
  double                        dTauS_;
  //
  double                        dTest_;
  double                        dTest2_;
  //
  bool                          isActive_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiMbandObservation inline members:                                                              */
/*                                                                                                     */
/*=====================================================================================================*/
//
//




// A destructor:
inline SgVlbiObservable::~SgVlbiObservable()
{
  owner_ = NULL;
  band_ = NULL;
  baseline_ = NULL;
  stn_1_ = NULL;
  stn_2_ = NULL;
  src_ = NULL;
  activeDelay_ = NULL;
  activeRate_  = NULL;
  activeMeasurement_  = NULL;
  releaseChannelsSetupStorages();
};



//
// INTERFACES:
//
//
// Gets:
//
inline const QString& SgVlbiObservable::getBandKey() const
{
  return bandKey_;
};



//
inline int SgVlbiObservable::getMediaIdx() const
{
  return mediaIdx_;
};



//
inline SgVlbiObservation* SgVlbiObservable::owner()
{
  return owner_;
};



//
inline SgVlbiBand* SgVlbiObservable::band()
{
  return band_;
};



//
inline SgVlbiBaselineInfo* &SgVlbiObservable::baseline()
{
  return baseline_;
};



//
inline SgVlbiStationInfo* &SgVlbiObservable::stn_1()
{
  return stn_1_;
};



//
inline SgVlbiStationInfo* &SgVlbiObservable::stn_2()
{
  return stn_2_;
};



//
inline SgVlbiSourceInfo* &SgVlbiObservable::src()
{
  return src_;
};



//
inline SgVlbiBaselineInfo* SgVlbiObservable::getBaseline() const
{
  return baseline_;
};



//
inline SgVlbiStationInfo* SgVlbiObservable::getStn_1() const
{
  return stn_1_;
};



//
inline SgVlbiStationInfo* SgVlbiObservable::getStn_2() const
{
  return stn_2_;
};



//
inline SgVlbiSourceInfo* SgVlbiObservable::getSrc() const
{
  return src_;
};



//
inline SgVlbiMeasurement& SgVlbiObservable::sbDelay()
{
  return sbDelay_;
};



//
inline SgVlbiMeasurement& SgVlbiObservable::grDelay()
{
  return grDelay_;
};



//
inline SgVlbiMeasurement& SgVlbiObservable::phDelay()
{
  return phDelay_;
};



//
inline SgVlbiMeasurement& SgVlbiObservable::phDRate()
{
  return phDRate_;
};



//
inline const QString& SgVlbiObservable::getErrorCode() const
{
  return errorCode_;
};



//
inline int SgVlbiObservable::getQualityFactor() const
{
  return qualityFactor_;
};



//
inline double SgVlbiObservable::getReferenceFrequency() const
{
  return referenceFrequency_;
};



//
inline double SgVlbiObservable::getTotalPhase() const
{
  return totalPhase_;
};



//
inline double SgVlbiObservable::getCorrCoeff() const
{
  return corrCoeff_;
};



//
inline double SgVlbiObservable::getSnr() const
{
  return snr_;
};



//
inline int SgVlbiObservable::getNumOfChannels() const
{
  return numOfChannels_;
};



//
inline const QString& SgVlbiObservable::getFourfitOutputFName() const
{
  return fourfitOutputFName_;
};



//
inline const SgMJD& SgVlbiObservable::getEpochOfCorrelation() const
{
  return epochOfCorrelation_;
};



//
inline const SgMJD& SgVlbiObservable::getEpochOfFourfitting() const
{
  return epochOfFourfitting_;
};



//
inline const SgMJD& SgVlbiObservable::getEpochOfScan() const
{
  return epochOfScan_;
};



//
inline const SgMJD& SgVlbiObservable::getEpochCentral() const
{
  return epochCentral_;
};



//
inline SgVlbiMeasurement* SgVlbiObservable::activeDelay()
{
  return activeDelay_;
};



//
inline SgVlbiMeasurement* SgVlbiObservable::activeRate()
{
  return activeRate_;
};



//
inline SgVlbiMeasurement* SgVlbiObservable::activeMeasurement()
{
  return activeMeasurement_;
};



// temporary/tests:
inline double SgVlbiObservable::getSbdDiffBand() const
{
  return sbdDiffBand_;
};
inline double SgVlbiObservable::getGrdDiffBand() const
{
  return grdDiffBand_;
};
inline double SgVlbiObservable::getPhrDiffBand() const
{
  return phrDiffBand_;
};
inline void SgVlbiObservable::setSbdDiffBand(double s)
{
  sbdDiffBand_ = s;
};
inline void SgVlbiObservable::setGrdDiffBand(double s)
{
  grdDiffBand_ = s;
};
inline void SgVlbiObservable::setPhrDiffBand(double s)
{
  phrDiffBand_ = s;
};




//
// Sets:
//
inline void SgVlbiObservable::setBandKey(const QString& k)
{
  bandKey_ = k;
};



//
inline void SgVlbiObservable::setMediaIdx(int idx)
{
  mediaIdx_ = idx;
};



//
inline void SgVlbiObservable::setErrorCode(const QString& q)
{
  errorCode_ = q;
};



//
inline void SgVlbiObservable::setQualityFactor(int q)
{
  qualityFactor_ = q;
};



//
inline void SgVlbiObservable::setReferenceFrequency(double v)
{
  referenceFrequency_ = v;
};



//
inline void SgVlbiObservable::setTotalPhase(double p)
{
  totalPhase_ = p;
};



//
inline void SgVlbiObservable::setCorrCoeff(double c)
{
  corrCoeff_ = c;
};



//
inline void SgVlbiObservable::setSnr(double r)
{
  snr_ = r;
};



//
inline void SgVlbiObservable::setNumOfChannels(int n)
{
  numOfChannels_ = n;
};



//
inline void SgVlbiObservable::setFourfitOutputFName(const QString& name)
{
  fourfitOutputFName_ = name;
};



//
inline void SgVlbiObservable::setEpochOfCorrelation(const SgMJD& date)
{
  epochOfCorrelation_ = date;
};



//
inline void SgVlbiObservable::setEpochOfFourfitting(const SgMJD& date)
{
  epochOfFourfitting_ = date;
};



//
inline void SgVlbiObservable::setEpochOfScan(const SgMJD& date)
{
  epochOfScan_ = date;
};



//
inline void SgVlbiObservable::setEpochCentral(const SgMJD& date)
{
  epochCentral_ = date;
};



//
// FUNCTIONS:
//
//
inline void SgVlbiObservable::zerofy()
{
  sbDelay_.zerofy();
  grDelay_.zerofy();
  phDelay_.zerofy();
  phDRate_.zerofy();
  errorCode_ = "";
  qualityFactor_ = 0;
  fourfitOutputFName_ = "";
  fourfitVersion_[0] = 0;
  fourfitVersion_[1] = 0;
  fourfitControlFile_ = "";
  fourfitCommandOverride_ = "";
  referenceFrequency_ = 0.0;
  totalPhase_ = 0.0;
  corrCoeff_ = 0.0;
  snr_ = 0.0;
};



//
inline void SgVlbiObservable::zerofyIono()
{
  sbDelay_.zerofyIono();
  grDelay_.zerofyIono();
  phDelay_.zerofyIono();
  phDRate_.zerofyIono();
  //
  setSbdDiffBand(0.0);
  setGrdDiffBand(0.0);
  setPhrDiffBand(0.0);
  //setSbdQ(0.0);
  //setGrdQ(0.0);
  //setPhrQ(0.0);
};


//
inline void SgVlbiObservable::propagateAttr(uint attr)
{
  sbDelay_.addAttr(attr);
  grDelay_.addAttr(attr);
  phDelay_.addAttr(attr);
  phDRate_.addAttr(attr);
};



//
inline void SgVlbiObservable::eradicateAttr(uint attr)
{
  sbDelay_.delAttr(attr);
  grDelay_.delAttr(attr);
  phDelay_.delAttr(attr);
  phDRate_.delAttr(attr);
};


//
inline void SgVlbiObservable::eradicateAttr(uint attr, SgTaskConfig::VlbiDelayType tp)
{
  if (tp == sbDelay_.getDelayType())
    sbDelay_.delAttr(attr);
  if (tp == grDelay_.getDelayType())
    grDelay_.delAttr(attr);
  if (tp == phDelay_.getDelayType())
    phDelay_.delAttr(attr);
};



//
inline void SgVlbiObservable::eradicateAttr(uint attr, SgTaskConfig::VlbiRateType tp)
{
  if (tp == phDRate_.getRateType() && tp != SgTaskConfig::VR_NONE)
    phDRate_.delAttr(attr);
};


/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_VLBI_OBSERVABLE_H
