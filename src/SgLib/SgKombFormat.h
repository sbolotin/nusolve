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

#ifndef SG_KOMB_FORMAT_H
#define SG_KOMB_FORMAT_H


#include <math.h>
#include <string.h>


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QDataStream>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextStream>
//#include <QtCore/QHash>



#include <Sg3dVector.h>
#include <SgMathSupport.h>
#include <SgMJD.h>






class SgKombHeader;

/***===================================================================================================*/
/**
 *    
 *    
 */
/**====================================================================================================*/
class SgKombStream : public QDataStream // just reserved type for this kind of operations
{};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombHeaderRecord
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombHeaderRecord();
  inline ~SgKombHeaderRecord() {};

  //
  // Interfaces:
  //
  inline int idx() const {return idx_;};
  inline const char* prefix() const {return prefix_;};
  inline const char* bandId() const {return bandId_;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombHeaderRecord&);
  friend SgKombStream &operator>>(SgKombStream&, SgKombHeader&);

protected:
  short                         idx_;
  char                          prefix_[5];
  char                          bandId_[3];
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombHeader
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombHeader();
  inline ~SgKombHeader();

  //
  // Interfaces:
  //
  inline short idx() const {return idx_;};
  inline const char* prefix() const {return prefix_;};
  inline const char* prefixIdx() const {return prefixIdx_;};
  inline const char* experimentCode() const {return experimentCode_;};
  inline short obsIdx() const {return obsIdx_;};
  inline const char* baselineId() const {return baselineId_;};
  inline short totalNumberOfRecs() const {return totalNumberOfRecs_;};
  inline short totalNumberOfHeaderRecs() const {return totalNumberOfHeaderRecs_;};
  inline const char* kombFileName() const {return kombFileName_;};
  inline const QList<SgKombHeaderRecord*>& entries() const {return entries_;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombHeader&);

protected:
  int                           idx_;
  char                          prefix_[3];
  char                          prefixIdx_[3];
  char                          experimentCode_[11];
  short                         obsIdx_;
  char                          baselineId_[3];
  short                         totalNumberOfRecs_;
  short                         totalNumberOfHeaderRecs_;
  char                          kombFileName_[7];
  QList<SgKombHeaderRecord*>    entries_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombOb01Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombOb01Record();
  inline ~SgKombOb01Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* experimentCode() const {return experimentCode_;};
  inline short obsIdx() const {return obsIdx_;};
  inline const char* baselineId() const {return baselineId_;};
  inline short epochObsStart(int i) const {return (0<=i&&i<5)?epochObsStart_[i]:0;};
  inline short epochObsStop(int i) const {return (0<=i&&i<5)?epochObsStop_[i]:0;};
  inline short epochObsRef(int i) const {return (0<=i&&i<5)?epochObsRef_[i]:0;};
  inline const char* correlatorFileName() const {return correlatorFileName_;};
  inline const char* kombFileName() const {return kombFileName_;};
  inline short epochProcCorr(int i) const {return (0<=i&&i<4)?epochProcCorr_[i]:0;};
  inline short periodPp() const {return periodPp_;};
  inline short numOfPp() const {return numOfPp_;};
  inline float samplingPeriod() const {return samplingPeriod_;};
  inline float videoBandWidth() const {return videoBandWidth_;};
  inline const char* correlatorMode() const {return correlatorMode_;};
  inline const char* sourceName() const {return sourceName_;};
  inline const char* station1Name() const {return station1Name_;};
  inline const char* station2Name() const {return station2Name_;};
  inline double aPrioriObses(int i) const {return (0<=i&&i<4)?aPrioriObses_[i]:0.0;};
  inline double aPrioriClockError() const {return aPrioriClockError_;};
  inline double aPrioriRefClockOffset() const {return aPrioriClockOffset_;};
  inline double clockRateDiff() const {return clockRateDiff_;};
  inline double instrumentalDelay() const {return instrumentalDelay_;};
  inline const SgMJD& tStart() const {return tStart_;};
  inline const SgMJD& tFinis() const {return tFinis_;};
  inline const SgMJD& tRefer() const {return tRefer_;};
  inline const SgMJD& tProcByCorr() const {return tProcByCorr_;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombOb01Record&);

protected:
  char                          prefix_[5];
  char                          experimentCode_[11];
  short                         obsIdx_;
  char                          baselineId_[3];
  short                         epochObsStart_[5];
  short                         epochObsStop_[5];
  short                         epochObsRef_[5];
  char                          correlatorFileName_[7];
  char                          kombFileName_[7];
  short                         epochProcCorr_[4];
  short                         periodPp_;
  short                         numOfPp_;
  float                         samplingPeriod_;
  float                         videoBandWidth_;
  char                          correlatorMode_[3];
  char                          sourceName_[9];
  char                          station1Name_[9];
  char                          station2Name_[9];
  double                        aPrioriObses_[4];
  double                        aPrioriClockError_;
  double                        aPrioriClockOffset_;
  double                        clockRateDiff_;
  double                        instrumentalDelay_;
  SgMJD                         tStart_;
  SgMJD                         tFinis_;
  SgMJD                         tRefer_;
  SgMJD                         tProcByCorr_;
  float                         srcRa_;
  float                         srcDe_;
  Sg3dVector                    r1_;
  Sg3dVector                    r2_;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombOb02Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombOb02Record();
  inline ~SgKombOb02Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline short numOfChannels() const {return numOfChannels_;};
  inline short idxNumTable(int i, int j) const {return (0<=i&&i<16 && 0<=j&&j<2)?idxNumTable_[i][j]:0;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombOb02Record&);

protected:
  char                          prefix_[5];
  short                         numOfChannels_;
  short                         idxNumTable_[16][2]; // chan x sideband[=USB,LSB]
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombOb03Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombOb03Record();
  inline ~SgKombOb03Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline double rfFreqs(int i) const {return (0<=i&&i<16)?rfFreqs_[i]:0.0;};
  inline float phCalFreqs(int i) const {return (0<=i&&i<16)?phCalFreqs_[i]:0.0;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombOb03Record&);

protected:
  char                          prefix_[5];
  double                        rfFreqs_[16];
  float                         phCalFreqs_[16];
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombBd01Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombBd01Record();
  inline ~SgKombBd01Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* bwsMode() const {return  bwsMode_;};
  inline const char* bandId() const {return  bandId_;};
  inline short epochProcKomb(int i) const {return (0<=i&&i<4)?epochProcKomb_[i]:0;};
  inline short kombProcsdNumber() const {return  kombProcsdNumber_;};
  inline short epochCorrStart(int i) const {return (0<=i&&i<6)?epochCorrStart_[i]:0;};
  inline short epochCorrFinis(int i) const {return (0<=i&&i<6)?epochCorrFinis_[i]:0;};
  inline short totalNumOfProcChannels() const {return  totalNumOfProcChannels_;};
  inline short idxNumTable(int i, int j) const {return (0<=i&&i<16 && 0<=j&&j<2)?idxNumTable_[i][j]:0;};
  inline const char* tapeQualityCode() const {return tapeQualityCode_;};
  inline double procRefFreq() const {return procRefFreq_;};
  inline double rfFreqs(int i) const {return (0<=i&&i<16)?rfFreqs_[i]:0.0;};
  inline const SgMJD& tProcByKomb() const {return tProcByKomb_;};
  inline const SgMJD& tCorrStart() const {return tCorrStart_;};
  inline const SgMJD& tCorrFinis() const {return tCorrFinis_;};

  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombBd01Record&);

protected:
  char                          prefix_[5];
  char                          bwsMode_[5];
  char                          bandId_[3];
  short                         epochProcKomb_[4];
  short                         kombProcsdNumber_;
  short                         epochCorrStart_[6];
  short                         epochCorrFinis_[6];
  short                         totalNumOfProcChannels_;
  short                         idxNumTable_[16][2]; // chan x sideband[=USB,LSB]
  char                          tapeQualityCode_[7];
  double                        procRefFreq_;
  double                        rfFreqs_[16];
  SgMJD                         tProcByKomb_;
  SgMJD                         tCorrStart_;
  SgMJD                         tCorrFinis_;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombBd02Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombBd02Record();
  inline ~SgKombBd02Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* bwsMode() const {return bwsMode_;};
  inline const char* bandId() const {return bandId_;};
  inline const char* kombQualityCode() const {return kombQualityCode_;};
  inline const char* errorStatusMaskCode(int i) const 
    {const char *p=(const char *)errorStatusMaskCode_; return (0<=i&&i<20)?(p+5*i):p;};
  inline short procPpNumTable(int i, int j) const 
    {return (0<=i&&i<16 && 0<=j&&j<2)?procPpNumTable_[i][j]:0;};
  inline float rmsPpNumByChan() const {return rmsPpNumByChan_;};
  inline float effectiveIntegrationPeriod() const {return effectiveIntegrationPeriod_;};
  inline float rejectionRate() const {return rejectionRate_;};
  inline short epochObsCentral(int i) const {return (0<=i&&i<6)?epochObsCentral_[i]:0;};
  inline double obsDelayAtCentrlEpoch() const {return obsDelayAtCentrlEpoch_;};
  inline double obsDelayRateAtCentrlEpoch() const {return obsDelayRateAtCentrlEpoch_;};
  inline float obsTotalPhaseAtCentrlEpoch() const {return obsTotalPhaseAtCentrlEpoch_;};
  inline float searchWndCoarseDelay(int i) const {return (0<=i&&i<2)?searchWndCoarseDelay_[i]:0.0;};
  inline float searchWndFineDelay(int i) const {return (0<=i&&i<2)?searchWndFineDelay_[i]:0.0;};
  inline float searchWndFineDelayRate(int i) const {return (0<=i&&i<2)?searchWndFineDelayRate_[i]:0.0;};
  inline double deltaEpochs() const {return deltaEpochs_;};
  inline float obsTotalPhaseAtReferEpoch() const {return obsTotalPhaseAtReferEpoch_;};
  inline float obsTotalPhaseAtEarthCenterEpoch() const {return obsTotalPhaseAtEarthCenterEpoch_;};
  inline float obsResidPhaseAtEarthCenterEpoch() const {return obsResidPhaseAtEarthCenterEpoch_;};
  inline const SgMJD& tObsCentral() const {return  tObsCentral_;};


  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombBd02Record&);

protected:
  char                          prefix_[5];
  char                          bwsMode_[5];
  char                          bandId_[3];
  char                          kombQualityCode_[3];
  char                          errorStatusMaskCode_[20][5];
  short                         procPpNumTable_[16][2]; // chan x sideband[=USB,LSB]
  float                         rmsPpNumByChan_;
  float                         effectiveIntegrationPeriod_;
  float                         rejectionRate_;
  short                         epochObsCentral_[6];
  double                        obsDelayAtCentrlEpoch_;
  double                        obsDelayRateAtCentrlEpoch_;
  float                         obsTotalPhaseAtCentrlEpoch_;
  float                         searchWndCoarseDelay_[2];
  float                         searchWndFineDelay_[2];
  float                         searchWndFineDelayRate_[2];
  double                        deltaEpochs_; // (PRT - Earth centered epoch), sec
  float                         obsTotalPhaseAtReferEpoch_;
  float                         obsTotalPhaseAtEarthCenterEpoch_;
  float                         obsResidPhaseAtEarthCenterEpoch_;
  SgMJD                         tObsCentral_;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombBd03Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombBd03Record();
  inline ~SgKombBd03Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* bwsMode() const {return bwsMode_;};
  inline const char* bandId() const {return bandId_;};
  inline double phaseCalRate1() const {return phaseCalRate1_;};
  inline double phaseCalRate2() const {return phaseCalRate2_;};
  inline float phaseCalAmpPhase1(int i, int j) const 
    {return (0<=i&&i<16 && 0<=j&&j<2)?phaseCalAmpPhase1_[i][j]:0.0;};


  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombBd03Record&);

protected:
  char                          prefix_[5];
  char                          bwsMode_[5];
  char                          bandId_[3];
  double                        phaseCalRate1_;
  double                        phaseCalRate2_;
  float                         phaseCalAmpPhase1_[16][2];
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombBd04Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombBd04Record();
  inline ~SgKombBd04Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* bwsMode() const {return bwsMode_;};
  inline const char* bandId() const {return bandId_;};
  inline float phaseCalAmpPhase2(int i, int j) const 
    {return (0<=i&&i<16 && 0<=j&&j<2)?phaseCalAmpPhase2_[i][j]:0.0;};


  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombBd04Record&);

protected:
  char                          prefix_[5];
  char                          bwsMode_[5];
  char                          bandId_[3];
  float                         phaseCalAmpPhase2_[16][2];
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombBd05Record
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombBd05Record();
  inline ~SgKombBd05Record() {};

  //
  // Interfaces:
  //
  inline const char* prefix() const {return prefix_;};
  inline const char* bwsMode() const {return bwsMode_;};
  inline const char* bandId() const {return bandId_;};
  inline float fineSearchFringeAmplitude() const {return fineSearchFringeAmplitude_;};
  inline float coarseSearchFringeAmplitude() const {return coarseSearchFringeAmplitude_;};
  inline float snr() const {return snr_;};
  inline float incohFringeAmplitude() const {return incohFringeAmplitude_;};
  inline float falseDetectionProbability() const {return falseDetectionProbability_;};
  inline double obsDelayAtReferEpoch() const {return obsDelayAtReferEpoch_;};
  inline double obsDelayResid() const {return obsDelayResid_;};
  inline float obsDelaySigma() const {return obsDelaySigma_;};
  inline float obsDelayAmbiguity() const {return obsDelayAmbiguity_;};
  inline double obsDelayRateAtReferEpoch() const {return obsDelayRateAtReferEpoch_;};
  inline double obsDelayRateResid() const {return obsDelayRateResid_;};
  inline float obsDelayRateSigma() const {return obsDelayRateSigma_;};
  inline double obsCoarseDelayAtReferEpoch() const {return obsCoarseDelayAtReferEpoch_;};
  inline double obsCoarseDelayResid() const {return obsCoarseDelayResid_;};
  inline float obsCoarseDelaySigma() const {return obsCoarseDelaySigma_;};
  inline double obsCoarseDelayRateResid() const {return obsCoarseDelayRateResid_;};
  inline double phaseDelayAtReferEpoch() const {return phaseDelayAtReferEpoch_;};
  inline double phaseDelayAtReferEpochPlus_1sec() const {return phaseDelayAtReferEpochPlus_1sec_;};
  inline double phaseDelayAtReferEpochMinus_1sec() const {return phaseDelayAtReferEpochMinus_1sec_;};
  inline float fringeAmpPhase(int i, int j) const 
    {return (0<=i&&i<16 && 0<=j&&j<2)?fringeAmpPhase_[i][j]:0.0;};


  //
  // Functions:
  //
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //
  friend SgKombStream &operator>>(SgKombStream&, SgKombBd05Record&);

protected:
  char                          prefix_[5];
  char                          bwsMode_[5];
  char                          bandId_[3];
  float                         fineSearchFringeAmplitude_;
  float                         coarseSearchFringeAmplitude_;
  float                         snr_;
  float                         incohFringeAmplitude_;
  float                         falseDetectionProbability_;
  double                        obsDelayAtReferEpoch_;
  double                        obsDelayResid_;
  float                         obsDelaySigma_;
  float                         obsDelayAmbiguity_;
  double                        obsDelayRateAtReferEpoch_;
  double                        obsDelayRateResid_;
  float                         obsDelayRateSigma_;
  double                        obsCoarseDelayAtReferEpoch_;
  double                        obsCoarseDelayResid_;
  float                         obsCoarseDelaySigma_;
  double                        obsCoarseDelayRateResid_;
  double                        phaseDelayAtReferEpoch_;
  double                        phaseDelayAtReferEpochPlus_1sec_;
  double                        phaseDelayAtReferEpochMinus_1sec_;
  float                         fringeAmpPhase_[16][2];
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 */
/**====================================================================================================*/
class SgKombReader
{
public:
  // Statics:
  static const QString className();

  //
  // constructors/destructors:
  //
  inline SgKombReader();
  inline ~SgKombReader();

  //
  // Interfaces:
  //
  inline bool getIsVerbose() const {return isVerbose_;};

  inline const SgKombOb01Record& ob01() const {return ob01_;};
  inline const SgKombOb02Record& ob02() const {return ob02_;};
  inline const SgKombOb03Record& ob03() const {return ob03_;};

  inline const QMap<QString, SgKombBd01Record*>& bd01ByBand() const {return bd01ByBand_;};
  inline const QMap<QString, SgKombBd02Record*>& bd02ByBand() const {return bd02ByBand_;};
  inline const QMap<QString, SgKombBd03Record*>& bd03ByBand() const {return bd03ByBand_;};
  inline const QMap<QString, SgKombBd04Record*>& bd04ByBand() const {return bd04ByBand_;};
  inline const QMap<QString, SgKombBd05Record*>& bd05ByBand() const {return bd05ByBand_;};
  inline const QMap<QString, QString>& mapOfBands() const {return mapOfBands_;};

  inline const SgKombBd01Record* bd01(const QString& bandKey) const
    {return bd01ByBand_.contains(bandKey)?bd01ByBand_.value(bandKey):NULL;};
  inline const SgKombBd02Record* bd02(const QString& bandKey) const
    {return bd02ByBand_.contains(bandKey)?bd02ByBand_.value(bandKey):NULL;};
  inline const SgKombBd03Record* bd03(const QString& bandKey) const
    {return bd03ByBand_.contains(bandKey)?bd03ByBand_.value(bandKey):NULL;};
  inline const SgKombBd04Record* bd04(const QString& bandKey) const
    {return bd04ByBand_.contains(bandKey)?bd04ByBand_.value(bandKey):NULL;};
  inline const SgKombBd05Record* bd05(const QString& bandKey) const
    {return bd05ByBand_.contains(bandKey)?bd05ByBand_.value(bandKey):NULL;};
  inline bool isOk() const {return isOk_;};
  inline void setIsVerbose(bool is) {isVerbose_ = is;};
  //
  // Functions:
  //
  inline const char* experimentCode() const 
    {return headers_.size()?headers_.at(0)->experimentCode():"";};
  void readFile(const QString& fileName);
  void clearData();
  void debugReport(); // temporary

  //
  // Friends:
  //

  //
  // I/O:
  //

protected:
  QString                       fileName_;
  QList<SgKombHeader*>          headers_;
  QList<SgKombHeaderRecord*>    headRecs_;
  SgKombOb01Record              ob01_;
  SgKombOb02Record              ob02_;
  SgKombOb03Record              ob03_;
  QMap<QString, SgKombBd01Record*>
                                bd01ByBand_;
  QMap<QString, SgKombBd02Record*>
                                bd02ByBand_;
  QMap<QString, SgKombBd03Record*>
                                bd03ByBand_;
  QMap<QString, SgKombBd04Record*>
                                bd04ByBand_;
  QMap<QString, SgKombBd05Record*>
                                bd05ByBand_;
  QMap<QString, QString>        mapOfBands_;
  bool                          isOk_;
  bool                          isVerbose_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombHeaderRecord inline members:                                                                  */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombHeaderRecord::SgKombHeaderRecord()
{
  idx_ = -1;
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  prefix_[4] = bandId_[2] = 0;
};



//
// FUNCTIONS:
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





/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombHeader inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombHeader::SgKombHeader()
{
  idx_ = -1;
  prefix_[0] = prefix_[1] = '?'; 
  prefixIdx_[0] = prefixIdx_[1] = '?'; 
  experimentCode_[0] = experimentCode_[1] = experimentCode_[2] = experimentCode_[3] = 
    experimentCode_[4] = experimentCode_[5] = experimentCode_[6] = experimentCode_[7] = 
    experimentCode_[8] = experimentCode_[9] = '?';
  obsIdx_=-1; 
  baselineId_[0] = baselineId_[1] = '?';
  totalNumberOfRecs_ = 0;
  totalNumberOfHeaderRecs_ = 0;
  kombFileName_[0] = kombFileName_[1] = kombFileName_[2] = kombFileName_[3] = kombFileName_[4] =
    kombFileName_[5] = '?';
  prefix_[2] = prefixIdx_[2] = experimentCode_[10] = baselineId_[2] = kombFileName_[6] = 0;
};



//
inline SgKombHeader::~SgKombHeader()
{
  for (int i=0; i<entries_.size(); i++) 
    delete entries_.at(i); 
  entries_.clear();
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombOb01Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombOb01Record::SgKombOb01Record() :
  tStart_(tZero),
  tFinis_(tZero),
  tRefer_(tZero),
  tProcByCorr_(tZero),
  r1_(v3Zero),
  r2_(v3Zero)
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  experimentCode_[0] = experimentCode_[1] = experimentCode_[2] = experimentCode_[3] = 
    experimentCode_[4] = experimentCode_[5] = experimentCode_[6] = experimentCode_[7] = 
    experimentCode_[8] = experimentCode_[9] = '?';
  obsIdx_ = -1;
  baselineId_[0] = baselineId_[1] = '?';
  epochObsStart_[0] = epochObsStart_[1] = epochObsStart_[2] = epochObsStart_[3] = epochObsStart_[4] = 0;
  epochObsStop_[0] = epochObsStop_[1] = epochObsStop_[2] = epochObsStop_[3] = epochObsStop_[4] = 0;
  epochObsRef_[0] = epochObsRef_[1] = epochObsRef_[2] = epochObsRef_[3] = epochObsRef_[4] = 0;
  correlatorFileName_[0] = correlatorFileName_[1] = correlatorFileName_[2] = correlatorFileName_[3] = 
    correlatorFileName_[4] = correlatorFileName_[5] = '?';
  kombFileName_[0] = kombFileName_[1] = kombFileName_[2] = kombFileName_[3] = kombFileName_[4] =
    kombFileName_[5] = '?';
  epochProcCorr_[0] = epochProcCorr_[1] = epochProcCorr_[2] = epochProcCorr_[3] = 0;
  periodPp_ = 0;
  numOfPp_ = 0;
  samplingPeriod_ = 0.0;
  videoBandWidth_ = 0.0;
  correlatorMode_[0] = correlatorMode_[1] = '?';
  sourceName_[0] = sourceName_[1] = sourceName_[2] = sourceName_[3] = sourceName_[4] = 
    sourceName_[5] = sourceName_[6] = sourceName_[7] = '?';
  station1Name_[0] = station1Name_[1] = station1Name_[2] = station1Name_[3] = station1Name_[4] = 
    station1Name_[5] = station1Name_[6] = station1Name_[7] = '?';
  station2Name_[0] = station2Name_[1] = station2Name_[2] = station2Name_[3] = station2Name_[4] = 
    station2Name_[5] = station2Name_[6] = station2Name_[7] = '?';
  aPrioriObses_[0] = aPrioriObses_[1] = aPrioriObses_[2] = aPrioriObses_[3] = 0.0;
  aPrioriClockError_ = 0.0;
  aPrioriClockOffset_ = 0.0;
  clockRateDiff_ = 0.0;
  instrumentalDelay_ = 0.0;

  prefix_[4] = experimentCode_[10] = baselineId_[2] = correlatorFileName_[6] = kombFileName_[6] = 
    correlatorMode_[2] = sourceName_[8] = station1Name_[8] = station2Name_[8] = 0;
  srcRa_ = srcDe_ = 0.0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombOb02Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombOb02Record::SgKombOb02Record()
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  numOfChannels_ = 0;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      idxNumTable_[i][j] = 0;
  prefix_[4] = 0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombOb03Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombOb03Record::SgKombOb03Record()
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  prefix_[4] = 0;
  for (int i=0; i<16; i++)
    rfFreqs_[i] = phCalFreqs_[i] = 0.0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombBd01Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombBd01Record::SgKombBd01Record() :
  tProcByKomb_(tZero),
  tCorrStart_(tZero),
  tCorrFinis_(tZero)
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bwsMode_[0] = bwsMode_[1] = bwsMode_[2] = bwsMode_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  tapeQualityCode_[0] = tapeQualityCode_[1] = tapeQualityCode_[2] = tapeQualityCode_[3] = 
    tapeQualityCode_[4] = tapeQualityCode_[5] = '?';
  prefix_[4] = bwsMode_[4] = bandId_[2] = tapeQualityCode_[6] = 0;
  epochProcKomb_[0] = epochProcKomb_[1] = epochProcKomb_[2] = epochProcKomb_[3] = 0;
  kombProcsdNumber_ = 0;
  epochCorrStart_[0] = epochCorrStart_[1] = epochCorrStart_[2] = epochCorrStart_[3] = 
    epochCorrStart_[4] = epochCorrStart_[5] = 0;
  epochCorrFinis_[0] = epochCorrFinis_[1] = epochCorrFinis_[2] = epochCorrFinis_[3] = 
    epochCorrFinis_[4] = epochCorrFinis_[5] = 0;
  totalNumOfProcChannels_ = 0;
  procRefFreq_ = 0;
  for (int i=0; i<16; i++)
  {
    rfFreqs_[i] = 0.0;
    for (int j=0; j<2; j++)
      idxNumTable_[i][j] = 0;
  };
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombBd02Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombBd02Record::SgKombBd02Record() :
  tObsCentral_(tZero)
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bwsMode_[0] = bwsMode_[1] = bwsMode_[2] = bwsMode_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  kombQualityCode_[0] = kombQualityCode_[1] = '?';
  for (int i=0; i<20; i++)
  {
    errorStatusMaskCode_[i][0] = errorStatusMaskCode_[i][1] = 
      errorStatusMaskCode_[i][2] = errorStatusMaskCode_[i][3] = '?';
    errorStatusMaskCode_[i][4] = 0;
  };
  prefix_[4] = bwsMode_[4] = bandId_[2] = kombQualityCode_[2] = 0;
  rmsPpNumByChan_ = effectiveIntegrationPeriod_ = rejectionRate_ = 0.0;
  epochObsCentral_[0] = epochObsCentral_[1] = epochObsCentral_[2] = epochObsCentral_[3] = 
    epochObsCentral_[4] = epochObsCentral_[5] = 0;
  obsDelayAtCentrlEpoch_ = obsDelayRateAtCentrlEpoch_ = 0.0;
  obsTotalPhaseAtCentrlEpoch_ = 0.0;
  searchWndCoarseDelay_[0] = searchWndCoarseDelay_[1] = 0.0;
  searchWndFineDelay_[0] = searchWndFineDelay_[1] = 0.0;
  searchWndFineDelayRate_[0] = searchWndFineDelayRate_[1] = 0.0;
  deltaEpochs_ = 0.0;
  obsTotalPhaseAtReferEpoch_ = obsTotalPhaseAtEarthCenterEpoch_ = obsResidPhaseAtEarthCenterEpoch_ = 0.0;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      procPpNumTable_[i][j] = 0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombBd03Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombBd03Record::SgKombBd03Record()
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bwsMode_[0] = bwsMode_[1] = bwsMode_[2] = bwsMode_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  phaseCalRate1_ = phaseCalRate2_ = 0.0;
  prefix_[4] = bwsMode_[4] = bandId_[2] = 0;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      phaseCalAmpPhase1_[i][j] = 0.0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombBd04Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombBd04Record::SgKombBd04Record()
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bwsMode_[0] = bwsMode_[1] = bwsMode_[2] = bwsMode_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  prefix_[4] = bwsMode_[4] = bandId_[2] = 0;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      phaseCalAmpPhase2_[i][j] = 0.0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombBd05Record inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombBd05Record::SgKombBd05Record()
{
  prefix_[0] = prefix_[1] = prefix_[2] = prefix_[3] = '?';
  bwsMode_[0] = bwsMode_[1] = bwsMode_[2] = bwsMode_[3] = '?';
  bandId_[0] = bandId_[1] = '?';
  prefix_[4] = bwsMode_[4] = bandId_[2] = 0;
  fineSearchFringeAmplitude_ = coarseSearchFringeAmplitude_ = snr_ = incohFringeAmplitude_ =
    falseDetectionProbability_ = 0.0;
  obsDelayAtReferEpoch_ = obsDelayResid_ = 0.0;
  obsDelaySigma_ = obsDelayAmbiguity_ = 0.0;
  obsDelayRateAtReferEpoch_ = obsDelayRateResid_ = 0.0;
  obsDelayRateSigma_ = obsCoarseDelaySigma_ = 0.0;
  obsCoarseDelayAtReferEpoch_ = obsCoarseDelayResid_ = obsCoarseDelayRateResid_ = 
    phaseDelayAtReferEpoch_ = phaseDelayAtReferEpochPlus_1sec_ = phaseDelayAtReferEpochMinus_1sec_ = 0.0;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      fringeAmpPhase_[i][j] = 0;
};



//
// FUNCTIONS:
//
//

//
// FRIENDS:
//
//
//
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgKombReader inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgKombReader::SgKombReader() :
  fileName_(""),
  headers_(),
  headRecs_(),
  ob01_(),
  ob02_(),
  ob03_(),
  bd01ByBand_(),
  bd02ByBand_(),
  bd03ByBand_(),
  bd04ByBand_(),
  bd05ByBand_(),
  mapOfBands_()
{
  isOk_ = false;
  isVerbose_ = false;
};



//
inline SgKombReader::~SgKombReader()
{
  clearData();
};



//
// FUNCTIONS:
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
#endif //SG_KOMB_FORMAT_H
