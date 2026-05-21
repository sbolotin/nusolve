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

#ifndef SG_IO_AGV_DRIVER
#define SG_IO_AGV_DRIVER


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include <iostream>
#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTextStream>
#include <QtCore/QVector>

#include <SgIoAgv.h>

#include <SgConstants.h>
#include <SgIoDriver.h>
#include <SgLogger.h>
#include <SgModelsInfo.h>
#include <SgVlbiHistory.h>






/***===================================================================================================*/
/**
 *
 *
 */
/*=====================================================================================================*/
class SgAgvDriver : public SgIoDriver, public QList<SgAgvChunk*>
{
public:
  SgAgvDriver(const SgIdentities* ids, const SgVersion* dv);
  ~SgAgvDriver();
  static inline QString className() {return "SgAgvDriver";};
 
  inline const QString& path2File() const {return path2File_;};
  inline const QString& fileName() const {return fileName_;};
 
  inline AgvContentStyle getExpectedStyle() const {return expectedStyle_;};
  inline int getNumOfObs() const {return numOfObs_;};
  inline int getNumOfScans() const {return numOfScans_;};
  inline const QMap<QString, int>& getNumOfChansByBand() const {return numOfChansByBand_;};
  inline int getNumOfBands() const {return numOfBands_;};
  inline int getNumOfSrc() const {return numOfSrc_;};
  inline int getNumOfStn() const {return numOfStn_;};
  inline int getNumOfBln() const {return numOfBln_;};
  inline int getNumOfStnPts(int idx) const 
    {return 0<=idx && idx<numOfObsBySidx_.size()?numOfObsBySidx_.value(idx):0;};
  inline int getNumOfChans2() const {return numOfChans2_;};
  inline const QMap<QString, int>& getNumOfObsByStn() const {return numOfObsByStn_;};
  inline int getMaxNumPerStn() const {return maxNumPerStn_;};

  inline const SgVlbiSession* getSession() const {return session_;};

  inline void setSession(SgVlbiSession *session) {session_ = session;};

  inline void setExpectedStyle(AgvContentStyle stl) {expectedStyle_ = stl;};
  inline void setNumOfObs(int n) {numOfObs_ = n;};
  inline void setNumOfScans(int n) {numOfScans_ = n;};
  inline void setMaxNumPerStn(int n) {maxNumPerStn_ = n;};
  inline void setNumOfSrc(int n) {numOfSrc_ = n;};
  inline void setNumOfStn(int n) {numOfStn_ = n;};
  inline void setNumOfBands(int n) {numOfBands_ = n;};
  inline void setNumOfBln(int n) {numOfBln_ = n;};
  
  virtual const QList<QString> listOfInputFiles() const;
  
  bool exportData(const QString& path2File, const QString& fileName);
  bool importData(const QString& fileName);

//QList<SgAgvDatumDescriptor*>& dataList() {return dataList_;};
  QMap<QString, SgAgvDatumDescriptor*>& datumByKey() {return datumByKey_;};
  QMap<QString, SgAgvDatumDescriptor*>& mandatoryDatumByKey() {return mandatoryDatumByKey_;};
  
  
  QMap<QString, SgAgvDatumString*>&       c1Data() {return c1Data_;};
  QMap<QString, SgAgvDatum<short int>*>&  i2Data() {return i2Data_;};
  QMap<QString, SgAgvDatum<int>*>&        i4Data() {return i4Data_;};
  QMap<QString, SgAgvDatum<long int>*>&   i8Data() {return i8Data_;};
  QMap<QString, SgAgvDatum<float>*>&      r4Data() {return r4Data_;};
  QMap<QString, SgAgvDatum<double>*>&     r8Data() {return r8Data_;};

  QString data2str(SgAgvDatumDescriptor* dd, int idx1, int idx2, int idx3, int idx4);
  bool inline addDatumDescriptor(SgAgvDatumDescriptor* dd) {return insertDatum(datumByKey_, dd);};
  void allocateData();

  void figureOutImplicitDimensions(const QList<SgAgvRecord*>*);
  void digestData(const QList<SgAgvRecord*>*);
  void digestDataNoRegEx(const QList<SgAgvRecord*>*);
  
protected:
  SgVlbiSession                *session_;
  QString                       path2File_;
  QString                       fileName_;
  AgvContentStyle               expectedStyle_;

  int                           numOfObs_;
  int                           numOfScans_;
  QMap<QString, int>            numOfChansByBand_;
  int                           numOfBands_;
  int                           numOfSrc_;
  int                           numOfStn_;
  int                           numOfBln_;
  int                           numOfStnPts_;
  int                           numOfChans2_;
  QMap<QString, int>            numOfObsByStn_;
  QMap<int, int>                numOfObsBySidx_;
  int                           maxNumPerStn_; // grrr..

  QMap<QString, int>            stnIdxByKey_;
  QMap<QString, int>            srcIdxByKey_;
  QMap<QString, int>            bndIdxByKey_;
  QVector<int>                  scanIdxByObsIdx_;
  QList<QString>                scanSrcNames_;

  QMap<QString, SgMJD>          epochByScanFullName_;
  QMap<QString, SgMJD>          epochByScanId_;
  QList<QString>                scanNames_;
  QList<QString>                scanFullNames_;
  QList<QString>                crootFileNames_;
  QList<QString>                stnNames_;
  SgMJD                         startEpoch_;
  //
  // special vars:
  bool                          hasCccSet_;


  // 
  QMap<QString, SgAgvDatumDescriptor*>
                                datumByKey_;
  QMap<QString, SgAgvDatumDescriptor*>
                                mandatoryDatumByKey_;
  QList<SgAgvDatumDescriptor*>  knownKeys_;

  //
  QMap<QString, SgAgvDatumString*>
                                c1Data_;
  QMap<QString, SgAgvDatum<short int>*>
                                i2Data_;
  QMap<QString, SgAgvDatum<int>*>
                                i4Data_;
  QMap<QString, SgAgvDatum<long int>*>
                                i8Data_;
  QMap<QString, SgAgvDatum<float>*>
                                r4Data_;
  QMap<QString, SgAgvDatum<double>*>
                                r8Data_;

  
  SgAgvDatumDescriptor          adNobsSta_, adNumbObs_, adNumbSca_, adNumbSta_, adObsTab_, adSiteNames_;

  SgAgvDatumDescriptor          adBandNames_, adCorrPlace_, adCorrType_, adExpCode_, adExpDesc_,
                                adExpName_, adMk3DbName_,  adPiName_, adRecMode_, adSrcName_,
                                adScanNames_, adQualCodes_, adExpSerNo_, adNumSrc_, adNumBands_,
                                adNumChnTot_, adNumChnBnd_, adNumAvaBnd_, adMjdObs_, adSrcIdx_,
                                adStnIdx_, adStnCoord_, adSkyFrqChn_, adSrcCoord_, adUtcMtai_, adUtcObs_,
                                adDelRate_, adGrDelAmbg_, adGrDelErr_, adGrDelVal_, adGrDelIonV_,
                                adGrDelIonE_, adPhRatIonV_, adPhRatIonE_,  adPhRatErr_, adRefFreq_,
                                adSbDelErr_, adSbDelVal_, adSnr_, adTotPhase_;

  SgAgvDatumDescriptor          adInterval4_, adCorrelatn_, adCrootFnam_, adFScanName_, adNumLags_,
                                adUvf_Asec_, adApLength_,  adFourfCmd_, adFourfCtrl_, adUnPhaseCl_,
                                adBitsSampl_,  adNusedChnl_,
                                adSamplRate_, adIonCorr_, adIonRms_, adIonDtFlg_, adEffFreq_, adEffFrew_,
                                adIonoBits_, adDelayFlag_, adRateFlag_, adPhaseFlag_, adUAcSup_,
                                adNumGrAmbg_, adNumPhAmbg_, adAtmInterv_, adAtmConstr_, adClkInterv_, 
                                adClkConstr_, adRefClocks_, adErrorK_,  adErrorBl_, adSrcStatus_, 
                                adBlnStatus_, adBlnClocks_, adCbrNumber_, adCbrSite_, adCbrImjd_, 
                                adCbrTime_, adObsCalFlg_, adStnCalFlg_, adFlybyFlg_, adObsCalNam_, 
                                adStnCalNam_, adFlyCalNam_, adAirTemp_, adAirPress_, adRelHumd_, 
                                adCableDel_, adCblsSet_,
                                adFourfErr_, adFourfFile_, adStrtOffst_, adStopOffst_, adHopsVer_,
                                adFourFtVer_, adScanUtc_, adUtcMtag_, adCorrUtc_, adFourUtc_,
                                adTapQcode_,
                                adQbFactor_, adDiscard_, adDeltaEpo_, adStartSec_, adStopSec_,
                                adSbResid_, adRtResid_, adEffDura_, adGcSbDel_, adGcMbDel_, adGcRate_,
                                adGcPhase_, adGcPhsRes_, adFalseDet_, adIncohAmp_, adIncohAmp2_,
                                adGrResid_, adAprioriDl_, adAprioriRt_, adAprioriAc_, adUrVr_, adIdelay_,
                                adSrchPar_, adCorrClock_,  adCorBasCd_, adZdelay_, adStarElev_,
                                adPhcRate_, adDtec_, adDtecAdj_, adDtecSig_, adCableSgn_,
                                adCablOnTp_, adCablOnTx_, adMeteOnTp_, adMeteOnTx_,
                                adSrcApRef_, adTectPlNm_,
                                // 4CALC:
                                adConsnDel_, adConsnRat_, adCtSite1_,  adEtdCont_,  adOceCont_,  adOceOld_,
                                adOptlCont_, adPtdCont_, adPtoLdCon_, adTiltRmvr_, adUt1Ortho_, 
                                adWobOrtho_, adUt1Libra_, adWobLibra_, adWobXcont_, adWobYcont_,
                                adConCont_, adSunCont_, adSun2cont_, adPlx1pSec_, adFeedCor_,
                                adBendPart_, adWobPart_, adUt1Part_, adConsPart_, adNut06Xyp_,
                                adPlxPart_, adPtdXyPar_, adStrPart_, adSitPart_,
                                adSunData_, adMunData_, adEarthCe_,
                                adNutWahr_, adNut2006a_, adNut06xys_,
                                adCf2J2k_0_, adCf2J2k_1_, adCf2J2k_2_,
                                adUt1_tai_, adPolarXy_, 
                                adElTheo_, adAzTheo_, adAxoCont_, adNdryCont_, adNwetCont_, adOceVert_,
                                adOceHorz_, adOceDeld_, adParangle_, adAxoPart_, adNgradPar_, 
                                adNdryPart_, adNwetPart_,
                                
                                adCalcFlgV_, adCalcFlgN_, adTidalUt1_, adCalcVer_,
                                adRelData_, adPreData_, adEtdData_,
                                adAtmMess_, adAtmCflg_,
                                adAxoMess_, adAxoCflg_,  adEtdMess_, adEtdCflg_,  adPtdMess_, adPtdCflg_,
                                adNutMess_, adNutCflg_,
                                adOceMess_, adOceCflg_, adAtiMess_, adAtiCflg_, adCtiMess_, adCtiCflg_,
                                adPlxMess_, adPlxCflg_, adStrMess_, adStrCflg_, adTheMess_, adRelCflg_,                                
                                adSitMess_, adPanMess_, adPepMess_, adWobCflg_, adUt1Cflg_, adOceStat_,
                                adUt1Epoch_, adWobEpoch_, adSiteZens_, adOPTLCoef_, adSitOcPhs_,
                                adSitOcAmp_, adSitHWOPh_, adSitHWOAm_, adSitHSOPh_, adSitHSOAm_,
                                adAxisTyps_, adAxisOffs_, adAxisTilt_,
                                adEccTypes_, adEccCoord_, adEccNames_, 
                                adFwobInf_, adFut1Inf_, adUt1Intrp_, adUt1Mess_, adWobMess_,
                                adWobIntrp_, adFut1Text_, adFwobText_, adFut1Pts_, adFwobXnYt_,
                                //
                                adNumAp1_, adNumAp2_, adCiNum1_, adBbcIdx1_, adBbcIdx2_,
                                adErrRate1_, adErrRate2_, adChanId1_, adChanId2_,
                                adPolariz1_, adPolariz2_, adRfFreq1_,  adRfFreq2_, adLoFreq1_, adLoFreq2_,
                                adNSampls1_, adNSampls2_, adApByFrq1_, adApByFrq2_,
                                adPhCFrq1_, adPhCFrq2_, adPhCAmp1_, adPhCAmp2_,
                                adPhCPhs1_,  adPhCPhs2_, adPhCOff1_, adPhCOff2_, adIndChn1_, adIndChn2_,
                                adNumSam1_,  adNumSam2_, adUvChn1_, adUvChn2_, adPCalFr1_, adPCalFr2_,
                                adPCalCm1_, adPCalCm2_,
                                //
                                adPimaCnt_, adPimaVer_, adFrnAmpl_, adFrTypFit_,
                                adAprCloOf_, adAprCloRt_, adAprDel_, adAprRate_,
                                adAzimuth_, adElev_, adPolariz_, adScanDur_, adStaClRf_,
                              
                                adAntGain_, adAprPhas_, adAprPhGc_, adAtmCns_, adAtmIntr_, adAutoSup_,
                                adAprEop_, adDerDel_, adDerRat_, adBand2nd_, adBasUse_, adBsclCns_,
                                adBsclEst_, adCalSts_, adCloCns_, adCloIntr_, adDaTyp_, adDgClEst_, 
                                adEditSts_, adEopCns_, adEopEst_, adRwBasNam_, adRwDelVal_, adRwRatVal_,
                                adSoCoCns_, adSoCoEst_, adSouUse_, adStaUse_, adStPsCns_,
                                adStPsEst_, adSupMet_, adTecSts_, adTilIntr_, adTlOfCns_, adTlRtCns_,
                                adUserRec_, adUserSup_, adCalInfo_, adCalName_, adMeanCabl_, adNcalib_,
                                adEopTab_, adMjdEop_, adNutDer_, adNaprEop_, adStepEop_, adTaiEop_,
                                adThGrDel_, adThPhDel_, adThPhRat_, adThProg_, adThRuDat_, adUvCoord_,
                                adRateCen_, adRateWdt_, adResGrRat_, adResMbDel_, adResPhas_, 
                                adResPhRat_, adResSbDel_, adResGrDl_, adResPhGc_, adResRate_, adSpChWdt_,
                                adSrtOffs_, adTsys1_, adTsys2_, adUvStaOrd_, adChanSdb_, adChanWdt_,
                                adDelWcen_, adDelWwdt_, adNoiseRms_, adPindObs_, adCorVers_, adFrtOffs_,
                                adGrRatErr_, adGrRate_, adNumClRf_, adPhDelErr_, adScanPima_



                                
                                ;

  // channel info:
  SgAgvDatumDescriptor         *adNumOfAp_;
  SgAgvDatumDescriptor         *adIndexNum_;
  SgAgvDatumDescriptor         *adChanId_;
  SgAgvDatumDescriptor         *adPolarz_;
  SgAgvDatumDescriptor         *adBbcIdx_;
  SgAgvDatumDescriptor         *adErrRate_;
  SgAgvDatumDescriptor         *adRfFreq_;
  SgAgvDatumDescriptor         *adIndChn_;
  SgAgvDatumDescriptor         *adLoRfFreq_;
  SgAgvDatumDescriptor         *adNumSmpls_;
  SgAgvDatumDescriptor         *adChAmpPhs_;
  SgAgvDatumDescriptor         *adPhcFrq_;
  SgAgvDatumDescriptor         *adPhcAmp_;
  SgAgvDatumDescriptor         *adPhcPhs_;
  SgAgvDatumDescriptor         *adPhcOff_;
  
  QList<SgAgvDatumDescriptor*>  fuzzyDescriptors_;


  void fillDataStructures();

  void allocateFuzzyDescriptors();
  
  void createDatum(SgAgvDatumDescriptor* dd);
  void initMandatoryVars();
  void initEssentialVars();
  
  void fillMandatoryVars();

  void fillSessVars();
  void fillScanVars();
  void fillBslnVars();
  void fillSttnVars();
  
  void prepare4output();
  bool insertDatum(QMap<QString, SgAgvDatumDescriptor*> &datumByKey, SgAgvDatumDescriptor* dd);
  
  void initSession();

  static bool strVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, QString &val);
  static bool sintVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, short int &val);
  static bool intVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, int &val);
  static bool lintVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, long int &val);
  static bool floatVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, float &val);
  static bool doubleVal(const QString& content,
    QString& lCode, int& d1, int& d2, int& d3, int& d4, double &val);


  static bool getContentIdxs(const QString& str, int& d1, int& d2, int& d3, int& d4);
  static bool strValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, QString& val);
  static bool sintValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, short int &val);
  static bool intValNoRegEx(const QString& content,  int& d1, int& d2, int& d3, int& d4, int &val);
  static bool lintValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, long int &val);
  static bool floatValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, float &val);
  static bool doubleValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, double &val);


  void lookupFuzzyDescriptor(const QString& lc, int bndIdx, const QList<QString>& bandNames,
    SgAgvDatumDescriptor*& dd, bool doCheckNumOfChannels, unsigned int expectedStyles);
  

  // support:
  void setCalcInfoModelMessNCtrlF(const QString& messLc, const QString& cflgLc,
    const SgModelsInfo::DasModel& model);
  void getCalcInfoModelMessNCtrlF(const QString& messLc, const QString& cflgLc,
    SgModelsInfo::DasModel& model);
  
  void checkCollectedDescriptors();
    
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// external data:
//

extern const SgAgvDatumDescriptor
                                adNobsSta, adNumbObs, adNumbSca, adNumbSta, adObsTab, adSiteNames;

extern const SgAgvDatumDescriptor
                                adBandNames, adCorrPlace, adCorrType, adExpCode, adExpDesc,
                                adExpName, adMk3DbName,  adPiName, adRecMode, adSrcName,  adScanNames,
                                adQualCodes, adExpSerNo, adNumSrc, adNumBands, adNumChnTot, adNumChnBnd,
                                adNumAvaBnd, adMjdObs, adSrcIdx, adStnIdx, adStnCoord, adSkyFrqChn,
                                adSrcCoord, adUtcMtai, adUtcObs, adDelRate, adGrDelAmbg, adGrDelErr,
                                adGrDelVal, adGrDelIonV, adGrDelIonE, adPhRatIonV, adPhRatIonE,
                                adPhRatErr, adRefFreq, adSbDelErr, adSbDelVal, adSnr, adTotPhase;
// added:
extern const SgAgvDatumDescriptor
                                adInterval4, adCorrelatn, adCrootFnam, adFScanName, adNumLags, 
                                adUvf_Asec, adApLength, adFourfCmd, adFourfCtrl, adUnPhaseCl,
                                adBitsSampl, adNusedChnl,
                                adSamplRate, adIonCorr, adIonRms, adIonDtFlg, adEffFreq, adEffFrew,
                                adIonoBits,  adDelayFlag, adRateFlag, adPhaseFlag, adUAcSup, 
                                adNumGrAmbg, adNumPhAmbg,
                                adAtmInterv, adAtmConstr, adClkInterv, adClkConstr, adRefClocks,
                                adErrorK,  adErrorBl, adSrcStatus, adBlnStatus, adBlnClocks, adCbrNumber,
                                adCbrSite, adCbrImjd, adCbrTime, adObsCalFlg, adStnCalFlg, adFlybyFlg,
                                adObsCalNam, adStnCalNam, adFlyCalNam, adAirTemp, adAirPress, adRelHumd,
                                adCableDel, adCblsSet, adFourfErr, adFourfFile, adStrtOffst, adStopOffst, 
                                adHopsVer, adFourFtVer, adScanUtc, adUtcMtag, adCorrUtc, adFourUtc, 
                                adTapQcode, adQbFactor,
                                adDiscard, adDeltaEpo, adStartSec, adStopSec, adSbResid, adRtResid,
                                adEffDura, adGcSbDel, adGcMbDel, adGcRate, adGcPhase, adGcPhsRes,
                                adFalseDet, adIncohAmp, adIncohAmp2, adGrResid, adAprioriDl,
                                adAprioriRt, adAprioriAc, adUrVr, adIdelay, adSrchPar, adCorrClock,
                                adCorBasCd, adZdelay, adStarElev,
                                adPhcRate, adDtec, adDtecAdj, adDtecSig, adCableSgn, 
                                adCablOnTp, adCablOnTx, adMeteOnTp, adMeteOnTx,
                                adSrcApRef, adTectPlNm,
                                // CALC:
                                adConsnDel, adConsnRat, adCtSite1, adEtdCont,  adOceCont,  adOceOld,
                                adOptlCont, adPtdCont,  adPtoLdCon, adTiltRmvr, adUt1Ortho, adWobOrtho,
                                adUt1Libra, adWobLibra, adWobXcont, adWobYcont, adConCont, adSunCont,
                                adSun2cont, adPlx1pSec, adFeedCor,
                                adBendPart, adWobPart, adUt1Part, adConsPart, adNut06Xyp,
                                adPlxPart, adPtdXyPar, adStrPart, adSitPart,
                                adSunData, adMunData, adEarthCe,
                                adNutWahr, adNut2006a, adNut06xys,
                                adCf2J2k_0, adCf2J2k_1, adCf2J2k_2,
                                adUt1_tai, adPolarXy, 
                                adElTheo, adAzTheo, adAxoCont, adNdryCont, adNwetCont, adOceVert,
                                adOceHorz, adOceDeld, adParangle, adAxoPart, adNgradPar, adNdryPart,
                                adNwetPart,

                                
                                adCalcFlgV, adCalcFlgN, adTidalUt1, adCalcVer, 
                                adRelData, adPreData, adEtdData,
                                adAtmMess, adAtmCflg,  adAxoMess, adAxoCflg,
                                adEtdMess, adEtdCflg,  adPtdMess, adPtdCflg, adNutMess, adNutCflg,
                                adOceMess, adOceCflg, adAtiMess, adAtiCflg, adCtiMess, adCtiCflg,
                                adPlxMess, adPlxCflg, adStrMess, adStrCflg, adTheMess, adRelCflg,
                                
                                adSitMess, adPanMess, adPepMess, adWobCflg, adUt1Cflg, adOceStat,
                                adUt1Epoch, adWobEpoch, adSiteZens, adOPTLCoef, adSitOcPhs,
                                adSitOcAmp, adSitHWOPh, adSitHWOAm, adSitHSOPh, adSitHSOAm,
                                adAxisTyps, adAxisOffs, adAxisTilt,
                                adEccTypes, adEccCoord, adEccNames, 
                                adFwobInf, adFut1Inf, adUt1Intrp, adUt1Mess, adWobMess,
                                adWobIntrp, adFut1Text, adFwobText, adFut1Pts, adFwobXnYt,

                                // fuzzy:
                                adNumAp1, adNumAp2, adCiNum1, adBbcIdx1, adBbcIdx2,
                                adErrRate1, adErrRate2, adChanId1, adChanId2, adPolariz1, adPolariz2,
                                adRfFreq1, adRfFreq2, adLoFreq1, adLoFreq2, adNSampls1, adNSampls2,
                                adApByFrq1, adApByFrq2, adPhCFrq1, adPhCFrq2, adPhCAmp1, adPhCAmp2,
                                adPhCPhs1, adPhCPhs2, adPhCOff1, adPhCOff2, adIndChn1, adIndChn2,
                                adNumSam1, adNumSam2, adUvChn1, adUvChn2, adPCalFr1, adPCalFr2,
                                adPCalCm1, adPCalCm2,

                                // PIMA's:
                                adPimaCnt, adPimaVer, adFrnAmpl, adFrTypFit, 
                                adAprCloOf, adAprCloRt, adAprDel, adAprRate,
                                adAzimuth, adElev, adPolariz, adScanDur, adStaClRf,

                                // unused:
                                adAntGain, adAprPhas, adAprPhGc, adAtmCns, adAtmIntr, adAutoSup,
                                adAprEop, adDerDel, adDerRat, adBand2nd, adBasUse, adBsclCns, adBsclEst, 
                                adCalSts, adCloCns, adCloIntr, adDaTyp, adDgClEst,
                                adEditSts, adEopCns, adEopEst, adRwBasNam, adRwDelVal, adRwRatVal,
                                adSoCoCns, adSoCoEst, adSouUse, adStaUse, adStPsCns,
                                adStPsEst, adSupMet, adTecSts, adTilIntr, adTlOfCns, adTlRtCns,
                                adUserRec, adUserSup, adCalInfo, adCalName, adMeanCabl, adNcalib,
                                adEopTab, adMjdEop, adNutDer, adNaprEop, adStepEop, adTaiEop, adThGrDel,
                                adThPhDel, adThPhRat, adThProg, adThRuDat, adUvCoord,
                                adRateCen, adRateWdt, adResGrRat, adResMbDel, adResPhas, adResPhRat,
                                adResSbDel, adResGrDl, adResPhGc, adResRate, adSpChWdt, adSrtOffs,
                                adTsys1, adTsys2, adUvStaOrd, adChanSdb, adChanWdt, adDelWcen, adDelWwdt,
                                adNoiseRms, adPindObs, adCorVers, adFrtOffs, adGrRatErr, adGrRate,
                                adNumClRf, adPhDelErr, adScanPima


                                
                                
                                
                                ;







/*=====================================================================================================*/

/*=====================================================================================================*/
#endif     //SG_IO_AGV_DRIVER

