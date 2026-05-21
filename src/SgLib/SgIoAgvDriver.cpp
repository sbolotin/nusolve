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

#include <iostream>
#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>



#include <Sg3dVector.h>
#include <SgConstants.h>
#include <SgEccRec.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>
#include <SgVlbiStationInfo.h>

#include <SgIoAgvDriver.h>


//#define LOCAL_DEBUG

extern const QString            sCalList[], sFclList[];


/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvDriver implementation                                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
SgAgvDriver::SgAgvDriver(const SgIdentities* ids, const SgVersion* dv)
  : SgIoDriver(ids, dv),
    path2File_(""),
    fileName_(""),
    numOfChansByBand_(),
    numOfObsByStn_(),
    numOfObsBySidx_(),
    stnIdxByKey_(),
    srcIdxByKey_(),
    bndIdxByKey_(),
    scanIdxByObsIdx_(),
    scanSrcNames_(),
    epochByScanFullName_(),
    epochByScanId_(),
    scanNames_(),
    scanFullNames_(),
    crootFileNames_(),
    stnNames_(),
    startEpoch_(tZero),
    datumByKey_(),
    mandatoryDatumByKey_(),
    knownKeys_(),
    c1Data_(), i2Data_(), i4Data_(), i8Data_(), r4Data_(), r8Data_(),

    adNobsSta_(adNobsSta), adNumbObs_(adNumbObs), adNumbSca_(adNumbSca), adNumbSta_(adNumbSta), 
    adObsTab_(adObsTab), adSiteNames_(adSiteNames), adBandNames_(adBandNames), adCorrPlace_(adCorrPlace),
    adCorrType_(adCorrType), adExpCode_(adExpCode), adExpDesc_(adExpDesc), adExpName_(adExpName),
    adMk3DbName_(adMk3DbName),  adPiName_(adPiName), adRecMode_(adRecMode), adSrcName_(adSrcName),
    adScanNames_(adScanNames), adQualCodes_(adQualCodes), adExpSerNo_(adExpSerNo), adNumSrc_(adNumSrc),
    adNumBands_(adNumBands), adNumChnTot_(adNumChnTot), adNumChnBnd_(adNumChnBnd),
    adNumAvaBnd_(adNumAvaBnd), adMjdObs_(adMjdObs), adSrcIdx_(adSrcIdx), adStnIdx_(adStnIdx),
    adStnCoord_(adStnCoord), adSkyFrqChn_(adSkyFrqChn), adSrcCoord_(adSrcCoord), adUtcMtai_(adUtcMtai),
    adUtcObs_(adUtcObs), adDelRate_(adDelRate), adGrDelAmbg_(adGrDelAmbg), adGrDelErr_(adGrDelErr),
    adGrDelVal_(adGrDelVal), adGrDelIonV_(adGrDelIonV), adGrDelIonE_(adGrDelIonE),
    adPhRatIonV_(adPhRatIonV), adPhRatIonE_(adPhRatIonE), adPhRatErr_(adPhRatErr), adRefFreq_(adRefFreq),
    adSbDelErr_(adSbDelErr), adSbDelVal_(adSbDelVal), adSnr_(adSnr), adTotPhase_(adTotPhase),
    adInterval4_(adInterval4), adCorrelatn_(adCorrelatn), adCrootFnam_(adCrootFnam), 
    adFScanName_(adFScanName), adNumLags_(adNumLags), adUvf_Asec_(adUvf_Asec), adApLength_(adApLength),
    adFourfCmd_(adFourfCmd), adFourfCtrl_(adFourfCtrl), adUnPhaseCl_(adUnPhaseCl),
    adBitsSampl_(adBitsSampl), adNusedChnl_(adNusedChnl), adSamplRate_(adSamplRate),
    adIonCorr_(adIonCorr), adIonRms_(adIonRms), adIonDtFlg_(adIonDtFlg), adEffFreq_(adEffFreq),
    adEffFrew_(adEffFrew), adIonoBits_(adIonoBits), adDelayFlag_(adDelayFlag), adRateFlag_(adRateFlag),
    adPhaseFlag_(adPhaseFlag), adUAcSup_(adUAcSup), adNumGrAmbg_(adNumGrAmbg), adNumPhAmbg_(adNumPhAmbg),
    adAtmInterv_(adAtmInterv), adAtmConstr_(adAtmConstr), adClkInterv_(adClkInterv), 
    adClkConstr_(adClkConstr), adRefClocks_(adRefClocks), adErrorK_(adErrorK),  adErrorBl_(adErrorBl),
    adSrcStatus_(adSrcStatus), adBlnStatus_(adBlnStatus), adBlnClocks_(adBlnClocks),
    adCbrNumber_(adCbrNumber), adCbrSite_(adCbrSite), adCbrImjd_(adCbrImjd), adCbrTime_(adCbrTime),
    adObsCalFlg_(adObsCalFlg), adStnCalFlg_(adStnCalFlg), adFlybyFlg_(adFlybyFlg),
    adObsCalNam_(adObsCalNam), adStnCalNam_(adStnCalNam), adFlyCalNam_(adFlyCalNam), 
    adAirTemp_(adAirTemp), adAirPress_(adAirPress), adRelHumd_(adRelHumd), adCableDel_(adCableDel),
    adCblsSet_(adCblsSet),
    adFourfErr_(adFourfErr), adFourfFile_(adFourfFile), adStrtOffst_(adStrtOffst), 
    adStopOffst_(adStopOffst), adHopsVer_(adHopsVer), adFourFtVer_(adFourFtVer), adScanUtc_(adScanUtc),
    adUtcMtag_(adUtcMtag), adCorrUtc_(adCorrUtc), adFourUtc_(adFourUtc), adTapQcode_(adTapQcode),
    adQbFactor_(adQbFactor), adDiscard_(adDiscard), adDeltaEpo_(adDeltaEpo), adStartSec_(adStartSec),
    adStopSec_(adStopSec), adSbResid_(adSbResid), adRtResid_(adRtResid), adEffDura_(adEffDura),
    adGcSbDel_(adGcSbDel), adGcMbDel_(adGcMbDel), adGcRate_(adGcRate), adGcPhase_(adGcPhase),
    adGcPhsRes_(adGcPhsRes), adFalseDet_(adFalseDet), adIncohAmp_(adIncohAmp), adIncohAmp2_(adIncohAmp2),
    adGrResid_(adGrResid), adAprioriDl_(adAprioriDl), adAprioriRt_(adAprioriRt),
    adAprioriAc_(adAprioriAc), adUrVr_(adUrVr), adIdelay_(adIdelay), adSrchPar_(adSrchPar),
    adCorrClock_(adCorrClock), adCorBasCd_(adCorBasCd), adZdelay_(adZdelay), adStarElev_(adStarElev),
    adPhcRate_(adPhcRate), adDtec_(adDtec), adDtecAdj_(adDtecAdj), adDtecSig_(adDtecSig), 
    adCableSgn_(adCableSgn), adCablOnTp_(adCablOnTp), adCablOnTx_(adCablOnTx), adMeteOnTp_(adMeteOnTp), 
    adMeteOnTx_(adMeteOnTx), adSrcApRef_(adSrcApRef), adTectPlNm_(adTectPlNm), adConsnDel_(adConsnDel), 
    adConsnRat_(adConsnRat), adCtSite1_(adCtSite1), adEtdCont_(adEtdCont), adOceCont_(adOceCont), 
    adOceOld_(adOceOld), adOptlCont_(adOptlCont), adPtdCont_(adPtdCont), adPtoLdCon_(adPtoLdCon), 
    adTiltRmvr_(adTiltRmvr), adUt1Ortho_(adUt1Ortho), adWobOrtho_(adWobOrtho), adUt1Libra_(adUt1Libra), 
    adWobLibra_(adWobLibra), adWobXcont_(adWobXcont), adWobYcont_(adWobYcont), adConCont_(adConCont), 
    adSunCont_(adSunCont), adSun2cont_(adSun2cont), adPlx1pSec_(adPlx1pSec), adFeedCor_(adFeedCor), 
    adBendPart_(adBendPart), adWobPart_(adWobPart), adUt1Part_(adUt1Part), adConsPart_(adConsPart), 
    adNut06Xyp_(adNut06Xyp), adPlxPart_(adPlxPart), adPtdXyPar_(adPtdXyPar), adStrPart_(adStrPart), 
    adSitPart_(adSitPart), adSunData_(adSunData), adMunData_(adMunData), adEarthCe_(adEarthCe), 
    adNutWahr_(adNutWahr), adNut2006a_(adNut2006a), adNut06xys_(adNut06xys), adCf2J2k_0_(adCf2J2k_0), 
    adCf2J2k_1_(adCf2J2k_1), adCf2J2k_2_(adCf2J2k_2), adUt1_tai_(adUt1_tai), adPolarXy_(adPolarXy), 
    adElTheo_(adElTheo), adAzTheo_(adAzTheo), adAxoCont_(adAxoCont), adNdryCont_(adNdryCont), 
    adNwetCont_(adNwetCont), adOceVert_(adOceVert), adOceHorz_(adOceHorz), adOceDeld_(adOceDeld), 
    adParangle_(adParangle), adAxoPart_(adAxoPart), adNgradPar_(adNgradPar), adNdryPart_(adNdryPart), 
    adNwetPart_(adNwetPart), adCalcFlgV_(adCalcFlgV), adCalcFlgN_(adCalcFlgN), adTidalUt1_(adTidalUt1), 
    adCalcVer_(adCalcVer), adRelData_(adRelData), adPreData_(adPreData), adEtdData_(adEtdData), 
    adAtmMess_(adAtmMess), adAtmCflg_(adAtmCflg), adAxoMess_(adAxoMess), adAxoCflg_(adAxoCflg), 
    adEtdMess_(adEtdMess), adEtdCflg_(adEtdCflg), adPtdMess_(adPtdMess), adPtdCflg_(adPtdCflg), 
    adNutMess_(adNutMess), adNutCflg_(adNutCflg), adOceMess_(adOceMess), adOceCflg_(adOceCflg), 
    adAtiMess_(adAtiMess), adAtiCflg_(adAtiCflg), adCtiMess_(adCtiMess), adCtiCflg_(adCtiCflg), 
    adPlxMess_(adPlxMess), adPlxCflg_(adPlxCflg), adStrMess_(adStrMess), adStrCflg_(adStrCflg), 
    adTheMess_(adTheMess), adRelCflg_(adRelCflg), adSitMess_(adSitMess), adPanMess_(adPanMess), 
    adPepMess_(adPepMess), adWobCflg_(adWobCflg), adUt1Cflg_(adUt1Cflg), adOceStat_(adOceStat), 
    adUt1Epoch_(adUt1Epoch), adWobEpoch_(adWobEpoch), adSiteZens_(adSiteZens), adOPTLCoef_(adOPTLCoef), 
    adSitOcPhs_(adSitOcPhs), adSitOcAmp_(adSitOcAmp), adSitHWOPh_(adSitHWOPh), adSitHWOAm_(adSitHWOAm), 
    adSitHSOPh_(adSitHSOPh), adSitHSOAm_(adSitHSOAm), adAxisTyps_(adAxisTyps), adAxisOffs_(adAxisOffs), 
    adAxisTilt_(adAxisTilt), adEccTypes_(adEccTypes), adEccCoord_(adEccCoord), adEccNames_(adEccNames), 
    adFwobInf_(adFwobInf), adFut1Inf_(adFut1Inf), adUt1Intrp_(adUt1Intrp), adUt1Mess_(adUt1Mess), 
    adWobMess_(adWobMess), adWobIntrp_(adWobIntrp), adFut1Text_(adFut1Text), adFwobText_(adFwobText), 
    adFut1Pts_(adFut1Pts), adFwobXnYt_(adFwobXnYt),
    //
    adNumAp1_(adNumAp1), adNumAp2_(adNumAp2), adCiNum1_(adCiNum1),
    adBbcIdx1_(adBbcIdx1), adBbcIdx2_(adBbcIdx2),
    adErrRate1_(adErrRate1), adErrRate2_(adErrRate2), adChanId1_(adChanId1), adChanId2_(adChanId2),
    adPolariz1_(adPolariz1), adPolariz2_(adPolariz2),
    adRfFreq1_(adRfFreq1), adRfFreq2_(adRfFreq2), adLoFreq1_(adLoFreq1), adLoFreq2_(adLoFreq2),
    adNSampls1_(adNSampls1), adNSampls2_(adNSampls2),
    adApByFrq1_(adApByFrq1), adApByFrq2_(adApByFrq2), adPhCFrq1_(adPhCFrq1), adPhCFrq2_(adPhCFrq2),
    adPhCAmp1_(adPhCAmp1), adPhCAmp2_(adPhCAmp2),
    adPhCPhs1_(adPhCPhs1), adPhCPhs2_(adPhCPhs2), adPhCOff1_(adPhCOff1), adPhCOff2_(adPhCOff2),
    adIndChn1_(adIndChn1), adIndChn2_(adIndChn2),
    adNumSam1_(adNumSam1), adNumSam2_(adNumSam2), adUvChn1_(adUvChn1), adUvChn2_(adUvChn2),
    adPCalFr1_(adPCalFr1), adPCalFr2_(adPCalFr2),
    adPCalCm1_(adPCalCm1), adPCalCm2_(adPCalCm2),
      
    //
    adPimaCnt_(adPimaCnt), adPimaVer_(adPimaVer), adFrnAmpl_(adFrnAmpl),
    adFrTypFit_(adFrTypFit), adAprCloOf_(adAprCloOf), adAprCloRt_(adAprCloRt), 
    adAprDel_(adAprDel), adAprRate_(adAprRate), adAzimuth_(adAzimuth), adElev_(adElev), 
    adPolariz_(adPolariz), adScanDur_(adScanDur), adStaClRf_(adStaClRf),
    //
    adAntGain_(adAntGain), adAprPhas_(adAprPhas), adAprPhGc_(adAprPhGc), adAtmCns_(adAtmCns),
    adAtmIntr_(adAtmIntr), adAutoSup_(adAutoSup),
    adAprEop_(adAprEop), adDerDel_(adDerDel),
    adDerRat_(adDerRat), adBand2nd_(adBand2nd), adBasUse_(adBasUse), adBsclCns_(adBsclCns),
    adBsclEst_(adBsclEst), adCalSts_(adCalSts), adCloCns_(adCloCns), adCloIntr_(adCloIntr),
    adDaTyp_(adDaTyp), adDgClEst_(adDgClEst), adEditSts_(adEditSts), adEopCns_(adEopCns),
    adEopEst_(adEopEst), adRwBasNam_(adRwBasNam), adRwDelVal_(adRwDelVal), adRwRatVal_(adRwRatVal),
    adSoCoCns_(adSoCoCns), adSoCoEst_(adSoCoEst), adSouUse_(adSouUse),
    adStaUse_(adStaUse), adStPsCns_(adStPsCns), adStPsEst_(adStPsEst), adSupMet_(adSupMet),
    adTecSts_(adTecSts), adTilIntr_(adTilIntr), adTlOfCns_(adTlOfCns), adTlRtCns_(adTlRtCns),
    adUserRec_(adUserRec), adUserSup_(adUserSup),
    adCalInfo_(adCalInfo), adCalName_(adCalName),
    adMeanCabl_(adMeanCabl), adNcalib_(adNcalib), adEopTab_(adEopTab), adMjdEop_(adMjdEop),
    adNutDer_(adNutDer), adNaprEop_(adNaprEop), adStepEop_(adStepEop), adTaiEop_(adTaiEop),
    adThGrDel_(adThGrDel), adThPhDel_(adThPhDel), adThPhRat_(adThPhRat), adThProg_(adThProg),
    adThRuDat_(adThRuDat), adUvCoord_(adUvCoord), adRateCen_(adRateCen), adRateWdt_(adRateWdt),
    adResGrRat_(adResGrRat), adResMbDel_(adResMbDel), adResPhas_(adResPhas), adResPhRat_(adResPhRat),
    adResSbDel_(adResSbDel), adResGrDl_(adResGrDl), adResPhGc_(adResPhGc), adResRate_(adResRate),
    adSpChWdt_(adSpChWdt), adSrtOffs_(adSrtOffs), adTsys1_(adTsys1), adTsys2_(adTsys2),
    adUvStaOrd_(adUvStaOrd), adChanSdb_(adChanSdb), adChanWdt_(adChanWdt), adDelWcen_(adDelWcen),
    adDelWwdt_(adDelWwdt), adNoiseRms_(adNoiseRms), adPindObs_(adPindObs), adCorVers_(adCorVers),
    adFrtOffs_(adFrtOffs), adGrRatErr_(adGrRatErr), adGrRate_(adGrRate), adNumClRf_(adNumClRf),
    adPhDelErr_(adPhDelErr), adScanPima_(adScanPima)

    
{
  session_  = NULL;
  adNumOfAp_= NULL;
  adIndexNum_ = NULL;
  adChanId_ = NULL;
  adPolarz_ = NULL;
  adBbcIdx_ = NULL;
  adErrRate_ = NULL;
  adRfFreq_ = NULL;
  adIndChn_ = NULL;
  adLoRfFreq_ = NULL;
  adNumSmpls_ = NULL;
  adChAmpPhs_ = NULL;
  adPhcFrq_ = NULL;
  adPhcAmp_ = NULL;
  adPhcPhs_ = NULL;
  adPhcOff_ = NULL;
  
  numOfObs_ = 0;
  numOfScans_ = 0;
  numOfBands_ = 0;
  numOfSrc_ = 0;
  numOfStn_ = 0;
  numOfBln_ = 0;
  numOfStnPts_ = 0;
  numOfChans2_ = 0;
  maxNumPerStn_= 0;
  knownKeys_ << &adNobsSta_ << &adNumbObs_ << &adNumbSca_ << &adNumbSta_ << &adObsTab_ << &adSiteNames_
    << &adBandNames_ << &adCorrPlace_ << &adCorrType_ << &adExpCode_ << &adExpDesc_
    << &adExpName_ << &adMk3DbName_ << &adPiName_ << &adRecMode_ << &adSrcName_ << &adScanNames_
    << &adQualCodes_ << &adExpSerNo_ << &adNumSrc_ << &adNumBands_ << &adNumChnTot_ << &adNumChnBnd_
    << &adNumAvaBnd_ << &adMjdObs_ << &adSrcIdx_ << &adStnIdx_ << &adStnCoord_ << &adSkyFrqChn_
    << &adSrcCoord_ << &adUtcMtai_ << &adUtcObs_ << &adDelRate_ << &adGrDelAmbg_ << &adGrDelErr_
    << &adGrDelVal_ << &adGrDelIonV_ << &adGrDelIonE_ << &adPhRatIonV_ << &adPhRatIonE_ << &adPhRatErr_
    << &adRefFreq_ << &adSbDelErr_ << &adSbDelVal_ << &adSnr_ << &adTotPhase_ << &adInterval4_
    << &adCorrelatn_ << &adCrootFnam_ << &adFScanName_ << &adNumLags_ << &adUvf_Asec_ << &adApLength_
    << &adFourfCmd_ << &adFourfCtrl_ << &adUnPhaseCl_ << &adBitsSampl_ << &adNusedChnl_ << &adSamplRate_
    << &adIonCorr_ << &adIonRms_ << &adIonDtFlg_ << &adEffFreq_ << &adEffFrew_ << &adIonoBits_
    << &adDelayFlag_ << &adRateFlag_ << &adPhaseFlag_ << &adUAcSup_ << &adNumGrAmbg_ << &adNumPhAmbg_ 
    << &adAtmInterv_
    << &adAtmConstr_ << &adClkInterv_ << &adClkConstr_ << &adRefClocks_ << &adErrorK_ << &adErrorBl_
    << &adSrcStatus_ << &adBlnStatus_ << &adBlnClocks_ << &adCbrNumber_ << &adCbrSite_ << &adCbrImjd_
    << &adCbrTime_ << &adObsCalFlg_ << &adStnCalFlg_ << &adFlybyFlg_ << &adObsCalNam_ << &adStnCalNam_ 
    << &adFlyCalNam_ << &adAirTemp_ << &adAirPress_ << &adRelHumd_ << &adCableDel_ << &adCblsSet_
    << &adFourfErr_
    << &adFourfFile_ << &adStrtOffst_ << &adStopOffst_ << &adHopsVer_ << &adFourFtVer_ << &adScanUtc_
    << &adUtcMtag_ << &adCorrUtc_ << &adFourUtc_ << &adTapQcode_ << &adQbFactor_ << &adDiscard_
    << &adDeltaEpo_ << &adStartSec_ << &adStopSec_ << &adSbResid_ << &adRtResid_ << &adEffDura_
    << &adGcSbDel_ << &adGcMbDel_ << &adGcRate_ << &adGcPhase_ << &adGcPhsRes_ << &adFalseDet_
    << &adIncohAmp_ << &adIncohAmp2_ << &adGrResid_ << &adAprioriDl_ << &adAprioriRt_ << &adAprioriAc_
    << &adUrVr_ << &adIdelay_ << &adSrchPar_ << &adCorrClock_ << &adCorBasCd_ << &adZdelay_
    << &adStarElev_ << &adPhcRate_ << &adDtec_ << &adDtecAdj_ << &adDtecSig_ << &adCableSgn_ 
    << &adCablOnTp_
    << &adCablOnTx_ << &adMeteOnTp_ << &adMeteOnTx_ << &adSrcApRef_ << &adTectPlNm_ << &adConsnDel_
    << &adConsnRat_ << &adCtSite1_ << &adEtdCont_ << &adOceCont_ << &adOceOld_ << &adOptlCont_
    << &adPtdCont_ << &adPtoLdCon_ << &adTiltRmvr_ << &adUt1Ortho_ << &adWobOrtho_ << &adUt1Libra_
    << &adWobLibra_ << &adWobXcont_ << &adWobYcont_ << &adConCont_ << &adSunCont_ << &adSun2cont_
    << &adPlx1pSec_ << &adFeedCor_ << &adBendPart_ << &adWobPart_ << &adUt1Part_ << &adConsPart_
    << &adNut06Xyp_ << &adPlxPart_ << &adPtdXyPar_ << &adStrPart_ << &adSitPart_ << &adSunData_
    << &adMunData_ << &adEarthCe_ << &adNutWahr_ << &adNut2006a_ << &adNut06xys_ << &adCf2J2k_0_
    << &adCf2J2k_1_ << &adCf2J2k_2_ << &adUt1_tai_ << &adPolarXy_ << &adElTheo_ << &adAzTheo_
    << &adAxoCont_ << &adNdryCont_ << &adNwetCont_ << &adOceVert_ << &adOceHorz_ << &adOceDeld_
    << &adParangle_ << &adAxoPart_ << &adNgradPar_ << &adNdryPart_ << &adNwetPart_ << &adCalcFlgV_
    << &adCalcFlgN_ << &adTidalUt1_ << &adCalcVer_ << &adRelData_ << &adPreData_ << &adEtdData_
    << &adAtmMess_ << &adAtmCflg_ << &adAxoMess_ << &adAxoCflg_ << &adEtdMess_ << &adEtdCflg_
    << &adPtdMess_ << &adPtdCflg_ << &adNutMess_ << &adNutCflg_ << &adOceMess_ << &adOceCflg_
    << &adAtiMess_ << &adAtiCflg_ << &adCtiMess_ << &adCtiCflg_ << &adPlxMess_ << &adPlxCflg_
    << &adStrMess_ << &adStrCflg_ << &adTheMess_ << &adRelCflg_ << &adSitMess_ << &adPanMess_
    << &adPepMess_ << &adWobCflg_ << &adUt1Cflg_ << &adOceStat_ << &adUt1Epoch_ << &adWobEpoch_
    << &adSiteZens_ << &adOPTLCoef_ << &adSitOcPhs_ << &adSitOcAmp_ << &adSitHWOPh_ << &adSitHWOAm_
    << &adSitHSOPh_ << &adSitHSOAm_ << &adAxisTyps_ << &adAxisOffs_ << &adAxisTilt_ << &adEccTypes_
    << &adEccCoord_ << &adEccNames_ << &adFwobInf_ << &adFut1Inf_ << &adUt1Intrp_ << &adUt1Mess_
    << &adWobMess_ << &adWobIntrp_ << &adFut1Text_ << &adFwobText_ << &adFut1Pts_ << &adFwobXnYt_;

  knownKeys_
    << &adNumAp1_   << &adNumAp2_   << &adCiNum1_  << &adBbcIdx1_ << &adBbcIdx2_
    << &adErrRate1_ << &adErrRate2_ << &adChanId1_ << &adChanId2_ << &adPolariz1_ << &adPolariz2_
    << &adRfFreq1_  << &adRfFreq2_  << &adLoFreq1_ << &adLoFreq2_ << &adNSampls1_ << &adNSampls2_
    << &adApByFrq1_ << &adApByFrq2_ << &adPhCFrq1_ << &adPhCFrq2_ << &adPhCAmp1_  << &adPhCAmp2_
    << &adPhCPhs1_  << &adPhCPhs2_  << &adPhCOff1_ << &adPhCOff2_ << &adIndChn1_  << &adIndChn2_
    << &adNumSam1_  << &adNumSam2_  << &adUvChn1_  << &adUvChn2_  << &adPCalFr1_  << &adPCalFr2_
    << &adPCalCm1_  << &adPCalCm2_  << &adAutoSup_ << &adUserSup_
    ;


  knownKeys_
    << &adPimaCnt_ << &adPimaVer_ << &adFrnAmpl_ << &adFrTypFit_ << &adAprCloOf_ << &adAprCloRt_
    << &adAprDel_ << &adAprRate_ << &adAzimuth_ << &adElev_ << &adPolariz_ << &adScanDur_ << &adStaClRf_  

    ;

  knownKeys_
    << &adAntGain_ << &adAprPhas_ << &adAprPhGc_ << &adAtmCns_ << &adAtmIntr_ 
    << &adAprEop_ << &adDerDel_ << &adDerRat_ << &adBand2nd_ << &adBasUse_ << &adBsclCns_ << &adBsclEst_
    << &adCalSts_ << &adCloCns_ << &adCloIntr_ << &adDaTyp_ << &adDgClEst_ << &adEditSts_ << &adEopCns_
    << &adEopEst_ << &adRwBasNam_ << &adRwDelVal_ << &adRwRatVal_ << &adSoCoCns_ << &adSoCoEst_
    << &adSouUse_ << &adStaUse_ << &adStPsCns_ << &adStPsEst_ << &adSupMet_ << &adTecSts_
    << &adTilIntr_ << &adTlOfCns_ << &adTlRtCns_ << &adUserRec_ << &adCalInfo_
    << &adCalName_ << &adMeanCabl_ << &adNcalib_ << &adEopTab_ << &adMjdEop_ << &adNutDer_ << &adNaprEop_
    << &adStepEop_ << &adTaiEop_ << &adThGrDel_ << &adThPhDel_ << &adThPhRat_ << &adThProg_
    << &adThRuDat_ << &adUvCoord_ << &adRateCen_ << &adRateWdt_ << &adResGrRat_ << &adResMbDel_ 
    << &adResPhas_ << &adResPhRat_ << &adResSbDel_ << &adResGrDl_ << &adResPhGc_ << &adResRate_
    << &adSpChWdt_ << &adSrtOffs_ << &adTsys1_ << &adTsys2_ << &adUvStaOrd_
    << &adChanSdb_ << &adChanWdt_ << &adDelWcen_ << &adDelWwdt_ << &adNoiseRms_ << &adPindObs_
    << &adCorVers_ << &adFrtOffs_ << &adGrRatErr_ << &adGrRate_ << &adNumClRf_
    << &adPhDelErr_ << &adScanPima_


    ;
  expectedStyle_ = ACS_NONE;
  hasCccSet_ = false;
};



//
SgAgvDriver::~SgAgvDriver()
{
  for (int i=0; i<size(); i++)
    delete at(i);
  clear();

  datumByKey_.clear();
  mandatoryDatumByKey_.clear();
  knownKeys_.clear();

  for (QMap<QString, SgAgvDatumString*>::iterator it=c1Data_.begin(); it!=c1Data_.end(); ++it)
    delete it.value();
  c1Data_.clear();
  
  for (QMap<QString, SgAgvDatum<short int>*>::iterator it=i2Data_.begin(); it!=i2Data_.end(); ++it)
    delete it.value();
  i2Data_.clear();
  
  for (QMap<QString, SgAgvDatum<int>*>::iterator it=i4Data_.begin(); it!=i4Data_.end(); ++it)
    delete it.value();
  i4Data_.clear();

  for (QMap<QString, SgAgvDatum<long int>*>::iterator it=i8Data_.begin(); it!=i8Data_.end(); ++it)
    delete it.value();
  i8Data_.clear();

  for (QMap<QString, SgAgvDatum<float>*>::iterator it=r4Data_.begin(); it!=r4Data_.end(); ++it)
    delete it.value();
  r4Data_.clear();

  for (QMap<QString, SgAgvDatum<double>*>::iterator it=r8Data_.begin(); it!=r8Data_.end(); ++it)
    delete it.value();
  r8Data_.clear();

  numOfChansByBand_.clear();
  numOfObsByStn_.clear();
  numOfObsBySidx_.clear();
  stnIdxByKey_.clear();
  srcIdxByKey_.clear();
  bndIdxByKey_.clear();
  scanIdxByObsIdx_.clear();
  scanSrcNames_.clear();
  epochByScanFullName_.clear();
  epochByScanId_.clear();

  session_ = NULL;

  delete []adNumOfAp_;
  delete []adIndexNum_;
  delete []adChanId_;
  delete []adPolarz_;
  delete []adBbcIdx_;
  delete []adErrRate_;
  delete []adRfFreq_;
  delete []adIndChn_;
  delete []adLoRfFreq_;
  delete []adNumSmpls_;
  delete []adChAmpPhs_;
  delete []adPhcFrq_;
  delete []adPhcAmp_;
  delete []adPhcPhs_;
  delete []adPhcOff_;

  adNumOfAp_  = NULL;
  adIndexNum_ = NULL;
  adChanId_   = NULL;
  adPolarz_   = NULL;
  adBbcIdx_   = NULL;
  adErrRate_  = NULL;
  adRfFreq_   = NULL;
  adIndChn_   = NULL;
  adLoRfFreq_ = NULL;
  adNumSmpls_ = NULL;
  adChAmpPhs_ = NULL;
  adPhcFrq_   = NULL;
  adPhcAmp_   = NULL;
  adPhcPhs_   = NULL;
  adPhcOff_   = NULL;
};



//
const QList<QString> SgAgvDriver::listOfInputFiles() const
{
//QString                       path2root("");
  QList<QString>                list;

  return list;
};



//
bool SgAgvDriver::insertDatum(QMap<QString, SgAgvDatumDescriptor*> &datumByKey, SgAgvDatumDescriptor* dd)
{
  if (!dd)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::insertDatum(): the datum descriptor is NULL");
  else if (datumByKey.contains(dd->getLCode()))
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::insertDatum(): the lCode \"" + dd->getLCode() + "\" already registered");
  else
  {
    datumByKey.insert(dd->getLCode(), dd);
    return true;
  };
  return false;
};



//
void SgAgvDriver::createDatum(SgAgvDatumDescriptor *dd)
{
  if (dd->getDriver())
    return;
    
  // set up the driver:
  dd->setDriver(this);
  // allocate data structures:
  switch (dd->getDataType())
  {
    case ADT_CHAR:
      if (c1Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in C1 map");
      else
        c1Data_.insert(dd->getLCode(), new SgAgvDatumString(dd));
    break;
    case ADT_I2:
      if (i2Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in I2 map");
      else
        i2Data_.insert(dd->getLCode(), new SgAgvDatum<short int>(dd));
    break;
    case ADT_I4:
      if (i4Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in I4 map");
      else
        i4Data_.insert(dd->getLCode(), new SgAgvDatum<int>(dd));
    break;
    case ADT_I8:
      if (i8Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in I8 map");
      else
        i8Data_.insert(dd->getLCode(), new SgAgvDatum<long int>(dd));
    break;
    case ADT_R4:
      if (r4Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in R4 map");
      else
        r4Data_.insert(dd->getLCode(), new SgAgvDatum<float>(dd));
    break;
    case ADT_R8:
      if (r8Data_.contains(dd->getLCode()))
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::createDatum(): the datum descriptor with the lCode \"" + dd->getLCode() + 
          "\" is already in R8 map");
      else
        r8Data_.insert(dd->getLCode(), new SgAgvDatum<double>(dd));
    break;
    case ADT_NONE:
    default:
    
    break;
  };
};



//
void SgAgvDriver::allocateData()
{
  // create a datum for each descriptor:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=mandatoryDatumByKey_.begin(); 
    it!=mandatoryDatumByKey_.end(); ++it)
    createDatum(it.value());
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=datumByKey_.begin(); 
    it!=datumByKey_.end(); ++it)
    createDatum(it.value());

  // allcoate space for datum:
  for (QMap<QString, SgAgvDatumString*>::iterator it=c1Data_.begin(); it!=c1Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();

  for (QMap<QString, SgAgvDatum<short int>*>::iterator it=i2Data_.begin(); it!=i2Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();

  for (QMap<QString, SgAgvDatum<int>*>::iterator it=i4Data_.begin(); it!=i4Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();

  for (QMap<QString, SgAgvDatum<long int>*>::iterator it=i8Data_.begin(); it!=i8Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();

  for (QMap<QString, SgAgvDatum<float>*>::iterator it=r4Data_.begin(); it!=r4Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();

  for (QMap<QString, SgAgvDatum<double>*>::iterator it=r8Data_.begin(); it!=r8Data_.end(); ++it)
    if (!it.value()->isAllocated())
      it.value()->allocateSpace();
};



//
QString SgAgvDriver::data2str(SgAgvDatumDescriptor* dd, int idx1, int idx2, int idx3, int idx4)
{
  QString                       str("NULL");
  SgAgvDatumString             *dstr=NULL;
  SgAgvDatum<short int>        *dsin=NULL;
  SgAgvDatum<int>              *dint=NULL;
  SgAgvDatum<long int>         *dlin=NULL;
  SgAgvDatum<float>            *dflt=NULL;
  SgAgvDatum<double>           *ddbl=NULL;
  
  if (!dd)
    return str;
  str = "N/A";
  
  switch (dd->getDataType())
  {
    case ADT_CHAR:
      if ((dstr = c1Data_.value(dd->getLCode())))
      {
        str = dstr->getValue(idx2, idx3, idx4).trimmed();
        str.replace(' ', '_');
      };
    break;
    case ADT_I2:
      if ((dsin = i2Data_.value(dd->getLCode())))
        str.setNum(dsin->getValue(idx1, idx2, idx3, idx4));
    break;
    case ADT_I4:
      if ((dint = i4Data_.value(dd->getLCode())))
        str.setNum(dint->getValue(idx1, idx2, idx3, idx4));
    break;
    case ADT_I8:
      if ((dlin = i8Data_.value(dd->getLCode())))
        str.setNum(dlin->getValue(idx1, idx2, idx3, idx4));
    break;
    case ADT_R4:
      if ((dflt = r4Data_.value(dd->getLCode())))
        str.setNum(dflt->getValue(idx1, idx2, idx3, idx4), 'E', 7);
    break;
    case ADT_R8:
      if ((ddbl = r8Data_.value(dd->getLCode())))
        str.setNum(ddbl->getValue(idx1, idx2, idx3, idx4), 'E', 15);
    break;
    case ADT_NONE:
    default:
      
    break;
  };
  return str;
};



//
bool SgAgvDriver::exportData(const QString& path2File, const QString& basicName)
{
  startEpoch_  = SgMJD::currentMJD();
  fillDataStructures();
  path2File_ = path2File.size()?path2File:".";
  fileName_ = basicName;
  if (basicName.indexOf('.') == -1) // no extension at all
    fileName_ += ".vda";

#ifdef LOCAL_DEBUG
  std::cout << "   --- path2File_=[" << qPrintable(path2File_) << "]\n";
  std::cout << "   --- fileName_ =[" << qPrintable(fileName_) << "]\n";
#endif


  QFile                         f(path2File_ + "/" + fileName_);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
      "::exportData(): error opening output file: " + path2File_ + "/" + fileName_);
    return false;
  };
  QTextStream                   ts(&f);

  for (int i=0; i<size(); i++)
    at(i)->exportData(ts, this);

  ts.setDevice(NULL);
  f.close();

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvDriver::exportData: done"
  //<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
    << qPrintable(QString(", dt= %1 ms").arg((SgMJD::currentMJD() - startEpoch_)*86400000.0, 0, 'f', 2))
    << "\n";
#endif

  return true;
};



//
void SgAgvDriver::initMandatoryVars()
{
  if (!mandatoryDatumByKey_.contains(adNobsSta_.getLCode())) // add the mandatory records:
  {
    mandatoryDatumByKey_.insert(adNobsSta_.getLCode(),   &adNobsSta_);
    mandatoryDatumByKey_.insert(adNumbObs_.getLCode(),   &adNumbObs_);
    mandatoryDatumByKey_.insert(adNumbSca_.getLCode(),   &adNumbSca_);
    mandatoryDatumByKey_.insert(adNumbSta_.getLCode(),   &adNumbSta_);
    mandatoryDatumByKey_.insert(adObsTab_.getLCode(),    &adObsTab_);
    mandatoryDatumByKey_.insert(adSiteNames_.getLCode(), &adSiteNames_);
  };
};



//
void SgAgvDriver::initEssentialVars()
{
  int                           n;
  //session scope:
  //strings:
  insertDatum(datumByKey_, &adBandNames_);
  insertDatum(datumByKey_, &adCorrPlace_);
  insertDatum(datumByKey_, &adCorrType_);
  insertDatum(datumByKey_, &adExpCode_);
  insertDatum(datumByKey_, &adExpDesc_);
  insertDatum(datumByKey_, &adExpName_);
  insertDatum(datumByKey_, &adMk3DbName_);
  insertDatum(datumByKey_, &adPiName_);
  insertDatum(datumByKey_, &adRecMode_);
  insertDatum(datumByKey_, &adSrcName_);
  //ints:
  insertDatum(datumByKey_, &adExpSerNo_);
  insertDatum(datumByKey_, &adNumSrc_);
  insertDatum(datumByKey_, &adNumBands_);
  insertDatum(datumByKey_, &adNumChnTot_);
  insertDatum(datumByKey_, &adNumChnBnd_);
  insertDatum(datumByKey_, &adNumAvaBnd_);
  insertDatum(datumByKey_, &adInterval4_);

  // clock breaks, mix:
  insertDatum(datumByKey_, &adCbrNumber_);
  insertDatum(datumByKey_, &adCbrSite_);
  insertDatum(datumByKey_, &adCbrImjd_);
  insertDatum(datumByKey_, &adCbrTime_);
  
  
  //reals:
  
  //4CALC:
  if (session_->isAttr(SgVlbiSessionInfo::Attr_HAS_CALC_DATA))
  {
    insertDatum(datumByKey_, &adStnCoord_);
    insertDatum(datumByKey_, &adSrcCoord_);
    insertDatum(datumByKey_, &adSrcApRef_);
    insertDatum(datumByKey_, &adTectPlNm_);
    insertDatum(datumByKey_, &adUtcMtai_);
    //
    insertDatum(datumByKey_, &adConsnDel_);
    insertDatum(datumByKey_, &adConsnRat_);
    insertDatum(datumByKey_, &adCtSite1_);
    insertDatum(datumByKey_, &adEtdCont_);
    insertDatum(datumByKey_, &adOceCont_);
    insertDatum(datumByKey_, &adOceOld_);
    insertDatum(datumByKey_, &adOptlCont_);
    insertDatum(datumByKey_, &adPtdCont_);
    insertDatum(datumByKey_, &adPtoLdCon_);
    insertDatum(datumByKey_, &adTiltRmvr_);
    insertDatum(datumByKey_, &adUt1Ortho_);
    insertDatum(datumByKey_, &adWobOrtho_);
    insertDatum(datumByKey_, &adUt1Libra_);
    insertDatum(datumByKey_, &adWobLibra_);
    insertDatum(datumByKey_, &adWobXcont_);
    insertDatum(datumByKey_, &adWobYcont_);
    insertDatum(datumByKey_, &adConCont_);
    insertDatum(datumByKey_, &adSunCont_);
    insertDatum(datumByKey_, &adSun2cont_);
    insertDatum(datumByKey_, &adPlx1pSec_);
    insertDatum(datumByKey_, &adFeedCor_);
    insertDatum(datumByKey_, &adBendPart_);
    insertDatum(datumByKey_, &adWobPart_);
    insertDatum(datumByKey_, &adUt1Part_);
    insertDatum(datumByKey_, &adConsPart_);
    insertDatum(datumByKey_, &adNut06Xyp_);
    insertDatum(datumByKey_, &adPlxPart_);
    insertDatum(datumByKey_, &adPtdXyPar_);
    insertDatum(datumByKey_, &adStrPart_);
    insertDatum(datumByKey_, &adSitPart_);
    insertDatum(datumByKey_, &adSunData_);
    insertDatum(datumByKey_, &adMunData_);
    insertDatum(datumByKey_, &adEarthCe_);
    insertDatum(datumByKey_, &adNutWahr_);
    insertDatum(datumByKey_, &adNut2006a_);
    insertDatum(datumByKey_, &adNut06xys_);
    insertDatum(datumByKey_, &adCf2J2k_0_);
    insertDatum(datumByKey_, &adCf2J2k_1_);
    insertDatum(datumByKey_, &adCf2J2k_2_);
    insertDatum(datumByKey_, &adUt1_tai_);
    insertDatum(datumByKey_, &adPolarXy_);
    insertDatum(datumByKey_, &adElTheo_);
    insertDatum(datumByKey_, &adAzTheo_);
    insertDatum(datumByKey_, &adAxoCont_);
    insertDatum(datumByKey_, &adNdryCont_);
    insertDatum(datumByKey_, &adNwetCont_);
    insertDatum(datumByKey_, &adOceVert_);
    insertDatum(datumByKey_, &adOceHorz_);
    insertDatum(datumByKey_, &adOceDeld_);
    insertDatum(datumByKey_, &adParangle_);
    insertDatum(datumByKey_, &adAxoPart_);
    insertDatum(datumByKey_, &adNgradPar_);
    insertDatum(datumByKey_, &adNdryPart_);
    insertDatum(datumByKey_, &adNwetPart_);
    

    // adjust the size:
    if ((n=session_->calcInfo().controlFlagNames().size()))
    {
      adCalcFlgN_.setDim2(n);
      adCalcFlgV_.setDim1(n);
      insertDatum(datumByKey_, &adCalcFlgN_);
      insertDatum(datumByKey_, &adCalcFlgV_);
      adCalcFlgV_.setHasData(true); // they are zeros...
    };
    
    insertDatum(datumByKey_, &adTidalUt1_);
    insertDatum(datumByKey_, &adCalcVer_);
    insertDatum(datumByKey_, &adRelData_);
    insertDatum(datumByKey_, &adPreData_);
    insertDatum(datumByKey_, &adEtdData_);
    insertDatum(datumByKey_, &adAtmMess_);
    insertDatum(datumByKey_, &adAtmCflg_);
    insertDatum(datumByKey_, &adAxoMess_);
    insertDatum(datumByKey_, &adAxoCflg_);
    insertDatum(datumByKey_, &adEtdMess_);
    insertDatum(datumByKey_, &adEtdCflg_);
    insertDatum(datumByKey_, &adPtdMess_);
    insertDatum(datumByKey_, &adPtdCflg_);
    insertDatum(datumByKey_, &adNutMess_);
    insertDatum(datumByKey_, &adNutCflg_);
    insertDatum(datumByKey_, &adOceMess_);
    insertDatum(datumByKey_, &adOceCflg_);
    insertDatum(datumByKey_, &adAtiMess_);
    insertDatum(datumByKey_, &adAtiCflg_);
    insertDatum(datumByKey_, &adCtiMess_);
    insertDatum(datumByKey_, &adCtiCflg_);
    insertDatum(datumByKey_, &adPlxMess_);
    insertDatum(datumByKey_, &adPlxCflg_);
    insertDatum(datumByKey_, &adStrMess_);
    insertDatum(datumByKey_, &adStrCflg_);
    insertDatum(datumByKey_, &adTheMess_);
    insertDatum(datumByKey_, &adRelCflg_);
    insertDatum(datumByKey_, &adSitMess_);
    insertDatum(datumByKey_, &adPanMess_);
    insertDatum(datumByKey_, &adPepMess_);
    insertDatum(datumByKey_, &adWobCflg_);
    insertDatum(datumByKey_, &adUt1Cflg_);

    // adjust the size:
    if ((n=session_->calcInfo().oLoadStationStatus().size()))
    {
      adOceStat_.setDim2(n);
      insertDatum(datumByKey_, &adOceStat_);
    };
    //
    if (session_->calcInfo().ut1InterpData() && (n=session_->calcInfo().ut1InterpData()->nRow()))
    {
      adUt1Epoch_.setDim2(n);
      insertDatum(datumByKey_, &adUt1Epoch_);
    };
    if (session_->calcInfo().wobInterpData() && (n=session_->calcInfo().wobInterpData()->nRow()))
    {
      adWobEpoch_.setDim2(n);
      insertDatum(datumByKey_, &adWobEpoch_);
    };
    //
    if ((n=session_->calcInfo().siteZenDelays().size()))
    {
      adSiteZens_.setDim1(n);
      insertDatum(datumByKey_, &adSiteZens_);
    };
    //
    if ((n=session_->calcInfo().stations().size()))
    {
      adOPTLCoef_.setDim1(n);
      adSitOcPhs_.setDim1(n);
      adSitOcAmp_.setDim1(n);
      adSitHWOPh_.setDim1(n);
      adSitHWOAm_.setDim1(n);
      adSitHSOPh_.setDim1(n);
      adSitHSOAm_.setDim1(n);
      adAxisTyps_.setDim1(n);
      adAxisOffs_.setDim1(n);
      adAxisTilt_.setDim1(n);
      
      adEccTypes_.setDim2(n);
      adEccCoord_.setDim1(n);
      adEccCoord_.setHasData(true); // usually, all they are zeros
      adEccNames_.setDim2(n);
      
      insertDatum(datumByKey_, &adOPTLCoef_);
      insertDatum(datumByKey_, &adSitOcPhs_);
      insertDatum(datumByKey_, &adSitOcAmp_);
      insertDatum(datumByKey_, &adSitHWOPh_);
      insertDatum(datumByKey_, &adSitHWOAm_);
      insertDatum(datumByKey_, &adSitHSOPh_);
      insertDatum(datumByKey_, &adSitHSOAm_);
      insertDatum(datumByKey_, &adAxisTyps_);
      insertDatum(datumByKey_, &adAxisOffs_);
      insertDatum(datumByKey_, &adAxisTilt_);
      insertDatum(datumByKey_, &adEccTypes_);
      insertDatum(datumByKey_, &adEccCoord_);
      insertDatum(datumByKey_, &adEccNames_);
    };

    if (session_->args4Ut1Interpolation() && session_->args4Ut1Interpolation()->n()   >0  &&
        session_->tabs4Ut1Interpolation() && session_->tabs4Ut1Interpolation()->nRow()>0  &&
        session_->args4PxyInterpolation() && session_->args4PxyInterpolation()->n()   >0  &&
        session_->tabs4PxyInterpolation() && session_->tabs4PxyInterpolation()->nRow()>0   )
    {
      insertDatum(datumByKey_, &adFut1Inf_);
      insertDatum(datumByKey_, &adUt1Intrp_);
      insertDatum(datumByKey_, &adUt1Mess_);
      insertDatum(datumByKey_, &adFwobInf_);
      insertDatum(datumByKey_, &adWobIntrp_);
      insertDatum(datumByKey_, &adWobMess_);
      insertDatum(datumByKey_, &adFut1Text_);
      insertDatum(datumByKey_, &adFwobText_);
      insertDatum(datumByKey_, &adFut1Pts_);
      insertDatum(datumByKey_, &adFwobXnYt_);
    };
  };

  insertDatum(datumByKey_, &adSkyFrqChn_);

  if (session_->isAttr(SgVlbiSessionInfo::Attr_PRE_PROCESSED))
  {
    insertDatum(datumByKey_, &adAtmInterv_);
    insertDatum(datumByKey_, &adAtmConstr_);
    insertDatum(datumByKey_, &adClkInterv_);
    insertDatum(datumByKey_, &adClkConstr_);

    insertDatum(datumByKey_, &adDelayFlag_);
    insertDatum(datumByKey_, &adRateFlag_);
    insertDatum(datumByKey_, &adPhaseFlag_);
    insertDatum(datumByKey_, &adUAcSup_);
    insertDatum(datumByKey_, &adNumGrAmbg_);
    insertDatum(datumByKey_, &adNumPhAmbg_);

    insertDatum(datumByKey_, &adSrcStatus_);
    insertDatum(datumByKey_, &adBlnStatus_);
    insertDatum(datumByKey_, &adBlnClocks_);

    insertDatum(datumByKey_, &adObsCalFlg_);
    insertDatum(datumByKey_, &adStnCalFlg_);
    insertDatum(datumByKey_, &adFlybyFlg_);
    insertDatum(datumByKey_, &adObsCalNam_);
    insertDatum(datumByKey_, &adStnCalNam_);
    insertDatum(datumByKey_, &adFlyCalNam_);

    //
    insertDatum(datumByKey_, &adErrorK_);
    insertDatum(datumByKey_, &adErrorBl_);

    insertDatum(datumByKey_, &adRefClocks_);
/*
    insertDatum(datumByKey_, &adAutoSup_);
    insertDatum(datumByKey_, &adUserSup_);
*/
  };
  

  //scan scope:
  //strings:
  insertDatum(datumByKey_, &adScanNames_);
  //added:
  insertDatum(datumByKey_, &adCrootFnam_);
  insertDatum(datumByKey_, &adFScanName_);
  //ints:
  insertDatum(datumByKey_, &adMjdObs_);
  insertDatum(datumByKey_, &adSrcIdx_);
  //reals:
  insertDatum(datumByKey_, &adUtcObs_);

  //baseline scope:  
  //strings:
  insertDatum(datumByKey_, &adQualCodes_);
  // added:
  insertDatum(datumByKey_, &adFourfCmd_);
  insertDatum(datumByKey_, &adFourfCtrl_);
    
  //ints:
  insertDatum(datumByKey_, &adStnIdx_);
  // added:
  insertDatum(datumByKey_, &adNumLags_);
  insertDatum(datumByKey_, &adUvf_Asec_);
  insertDatum(datumByKey_, &adBitsSampl_);
  insertDatum(datumByKey_, &adNusedChnl_);
  
  
  //reals:
  insertDatum(datumByKey_, &adDelRate_);
  insertDatum(datumByKey_, &adGrDelAmbg_);
  insertDatum(datumByKey_, &adGrDelErr_);
  insertDatum(datumByKey_, &adGrDelVal_);
  insertDatum(datumByKey_, &adGrDelIonV_);
  insertDatum(datumByKey_, &adGrDelIonE_);
  insertDatum(datumByKey_, &adPhRatIonV_);
  insertDatum(datumByKey_, &adPhRatIonE_);
  insertDatum(datumByKey_, &adPhRatErr_);
  insertDatum(datumByKey_, &adRefFreq_);
  insertDatum(datumByKey_, &adSbDelErr_);
  insertDatum(datumByKey_, &adSbDelVal_);
  insertDatum(datumByKey_, &adSnr_);
  insertDatum(datumByKey_, &adTotPhase_);

  // added:
  if (session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_HOPS || 
      session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_C5PP)
  {
    insertDatum(datumByKey_, &adCorrelatn_);
  } 
  else if (session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA)
  {
    insertDatum(datumByKey_, &adFrnAmpl_);
  };
  
  
  insertDatum(datumByKey_, &adApLength_);
  insertDatum(datumByKey_, &adUnPhaseCl_);
  insertDatum(datumByKey_, &adSamplRate_);
  
  // ionosphere corrections:
  // add if numOfBands > 1:
  if (numOfBands_ > 1)
  {
    insertDatum(datumByKey_, &adIonCorr_);
    insertDatum(datumByKey_, &adIonRms_);
    insertDatum(datumByKey_, &adIonDtFlg_);
    insertDatum(datumByKey_, &adIonoBits_);
  };
  insertDatum(datumByKey_, &adEffFreq_);
  insertDatum(datumByKey_, &adEffFrew_);
  
  
  // VGOS:
  insertDatum(datumByKey_, &adDtec_);
  insertDatum(datumByKey_, &adDtecAdj_); //PIMA's
  insertDatum(datumByKey_, &adDtecSig_);
  

  // correlator info data:
  insertDatum(datumByKey_, &adFourfErr_);
  insertDatum(datumByKey_, &adFourfFile_);
  insertDatum(datumByKey_, &adStrtOffst_);
  insertDatum(datumByKey_, &adStopOffst_);
  insertDatum(datumByKey_, &adHopsVer_);
  insertDatum(datumByKey_, &adFourFtVer_);
  insertDatum(datumByKey_, &adScanUtc_);
  insertDatum(datumByKey_, &adUtcMtag_);
  insertDatum(datumByKey_, &adCorrUtc_);
  insertDatum(datumByKey_, &adFourUtc_);
  insertDatum(datumByKey_, &adTapQcode_);
  insertDatum(datumByKey_, &adQbFactor_);
  insertDatum(datumByKey_, &adDiscard_);
  insertDatum(datumByKey_, &adDeltaEpo_);
  insertDatum(datumByKey_, &adStartSec_);
  insertDatum(datumByKey_, &adStopSec_);
  insertDatum(datumByKey_, &adSbResid_);
  insertDatum(datumByKey_, &adRtResid_);
  insertDatum(datumByKey_, &adEffDura_);
  insertDatum(datumByKey_, &adGcSbDel_);
  insertDatum(datumByKey_, &adGcMbDel_);
  insertDatum(datumByKey_, &adGcRate_);
  insertDatum(datumByKey_, &adGcPhase_);
  insertDatum(datumByKey_, &adGcPhsRes_);
  insertDatum(datumByKey_, &adFalseDet_);
  insertDatum(datumByKey_, &adIncohAmp_);
  insertDatum(datumByKey_, &adIncohAmp2_);
  insertDatum(datumByKey_, &adGrResid_);
  insertDatum(datumByKey_, &adAprioriDl_);
  insertDatum(datumByKey_, &adAprioriRt_);
  insertDatum(datumByKey_, &adAprioriAc_);
  insertDatum(datumByKey_, &adUrVr_);
  insertDatum(datumByKey_, &adIdelay_);
  insertDatum(datumByKey_, &adSrchPar_);
  insertDatum(datumByKey_, &adCorrClock_);
  insertDatum(datumByKey_, &adCorBasCd_);
  insertDatum(datumByKey_, &adZdelay_);
  insertDatum(datumByKey_, &adStarElev_);
  //
  // phase cal info:
  insertDatum(datumByKey_, &adPhcRate_);

  
  // 4 testing:
/*
  adFourfErr_.setHasData(true);
  adFourfFile_.setHasData(true);
  adStrtOffst_.setHasData(true);
  adStopOffst_.setHasData(true);
  adHopsVer_.setHasData(true);
  adFourFtVer_.setHasData(true);
  adScanUtc_.setHasData(true);
  adUtcMtag_.setHasData(true);
  adCorrUtc_.setHasData(true);
  adFourUtc_.setHasData(true);
  adQbFactor_.setHasData(true);
  adDiscard_.setHasData(true);
  adDeltaEpo_.setHasData(true);
  adStartSec_.setHasData(true);
  adStopSec_.setHasData(true);
  adSbResid_.setHasData(true);
  adRtResid_.setHasData(true);
  adEffDura_.setHasData(true);
  adGcSbDel_.setHasData(true);
  adGcMbDel_.setHasData(true);
  adGcRate_.setHasData(true);
  adGcPhase_.setHasData(true);
  adGcPhsRes_.setHasData(true);
  adFalseDet_.setHasData(true);
  adIncohAmp_.setHasData(true);
  adIncohAmp2_.setHasData(true);
  adGrResid_.setHasData(true);
  adAprioriDl_.setHasData(true);
  adAprioriRt_.setHasData(true);
  adAprioriAc_.setHasData(true);
  adUrVr_.setHasData(true);
  adIdelay_.setHasData(true);
  adSrchPar_.setHasData(true);
  adCorrClock_.setHasData(true);
  adPhcRate_.setHasData(true);
*/

  // stations:
  if (session_->isAttr(SgVlbiSessionInfo::Attr_HAS_AUX_OBS))
  {
    insertDatum(datumByKey_, &adAirTemp_);
    insertDatum(datumByKey_, &adAirPress_);
    insertDatum(datumByKey_, &adRelHumd_);
    insertDatum(datumByKey_, &adCableDel_);
    insertDatum(datumByKey_, &adCableSgn_);
    insertDatum(datumByKey_, &adCablOnTp_);
    insertDatum(datumByKey_, &adCablOnTx_);
    insertDatum(datumByKey_, &adMeteOnTp_);
    insertDatum(datumByKey_, &adMeteOnTx_);
  };

  //
  //
  hasCccSet_ = false;
  QMap<QString, SgVlbiStationInfo*>::const_iterator
                                it=session_->stationsByName().constBegin();
  for (; !hasCccSet_ && it!=session_->stationsByName().constEnd(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS))
      hasCccSet_ = true;
    if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT))
      hasCccSet_ = true;
  };
  if (hasCccSet_)
    insertDatum(datumByKey_, &adCblsSet_);


  
  //
  //
  adIonDtFlg_.setHasData(true); //<- it is ok to have all zeros.
  //?
  adDelayFlag_.setHasData(true);
  adRateFlag_.setHasData(true);
  adPhaseFlag_.setHasData(true);
  adUAcSup_.setHasData(true);
  adNumGrAmbg_.setHasData(true); // its ok if all of them are zeros
};



//
void SgAgvDriver::allocateFuzzyDescriptors()
{
  // Number of accumulated periods:
  adNumOfAp_    = new SgAgvDatumDescriptor[numOfBands_];
  adIndexNum_   = new SgAgvDatumDescriptor[numOfBands_];
  adChanId_     = new SgAgvDatumDescriptor[numOfBands_];
  adPolarz_     = new SgAgvDatumDescriptor[numOfBands_];
  adBbcIdx_     = new SgAgvDatumDescriptor[numOfBands_];
  adErrRate_    = new SgAgvDatumDescriptor[numOfBands_];
  adRfFreq_     = new SgAgvDatumDescriptor[numOfBands_];
  adIndChn_     = new SgAgvDatumDescriptor[numOfBands_];
  adLoRfFreq_   = new SgAgvDatumDescriptor[numOfBands_];
  adNumSmpls_   = new SgAgvDatumDescriptor[numOfBands_];
  adChAmpPhs_   = new SgAgvDatumDescriptor[numOfBands_];
  adPhcFrq_     = new SgAgvDatumDescriptor[numOfBands_];
  adPhcAmp_     = new SgAgvDatumDescriptor[numOfBands_];
  adPhcPhs_     = new SgAgvDatumDescriptor[numOfBands_];
  adPhcOff_     = new SgAgvDatumDescriptor[numOfBands_];

  //fuzzyDescriptors_.append(adNumOfAp_);

  for (int bndIdx=0; bndIdx<numOfBands_; bndIdx++)
  {
    SgAgvDatumDescriptor       *d;
    const QString&              bandKey=session_->bands().at(bndIdx)->getKey();
    int                         numOfChannels=numOfChansByBand_[bandKey];
    if (numOfChannels) // can be 0
    {
      // ints:
      // Number of accumulated periods:
      d = adNumOfAp_ + bndIdx;
      d->setLCode(QString("").sprintf("NUM_AP%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Number of accumulation periods used in band %d per channel "
        "per sideband (USB, LSB)", bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_I2);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // Corel index numbers
      d = adIndexNum_ + bndIdx;
      d->setLCode(QString("").sprintf("CI_NUM%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Corel index numbers in band %d per channel "
        "per sideband (USB, LSB)", bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_I2);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // Physical BBC number by channel:
      d = adBbcIdx_ + bndIdx;
      d->setLCode(QString("").sprintf("BBC_IDX%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Physical BBC number per channel per station in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_I2);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // Error rate:
      d = adErrRate_ + bndIdx;       //12345678
      d->setLCode(QString("").sprintf("ERRATE_%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Log err rate per channel per station in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_I2);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // strings:
      // Channel IDs:
      d = adChanId_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("CHANID%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Space separated one-letter Fourfit channel IDs in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_CHAR);
      d->setDim1(1);
      d->setDim2(1);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);
    
      // polarizations:
      d = adPolarz_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("POLARZ%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Space separated polarization per sta/chan in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_CHAR);
      d->setDim1(1);
      d->setDim2(1);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);
    
      // channel frequencies:
      d = adRfFreq_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("RFREQ%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("RF freq by channel (MHz) in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(1);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // LO frequencies:
      d = adLoRfFreq_ + bndIdx;      //12345678
      d->setLCode(QString("").sprintf("LO_FREQ%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("LO frequencies per cha/sta MHz in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // number of samples:
      d = adNumSmpls_ + bndIdx;      //12345678
      d->setLCode(QString("").sprintf("NSAMPLS%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Number of samples per channel, sideband (USB, LSB) in "
        "band %d", bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);

      // Amplitudes and phases:
      d = adChAmpPhs_ + bndIdx;      //12345678
      d->setLCode(QString("").sprintf("APBYFRQ%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Amp(0-1), phs(-180to180) by chan in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      fuzzyDescriptors_.append(d);
    
      // phase cal data:
      // frequencies:
      d = adPhcFrq_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("PHCFRQ_%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Phase cal freqs by channel by station in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      d->setHasData(true);
      fuzzyDescriptors_.append(d);

      // amplitudes:
      d = adPhcAmp_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("PHCAMP_%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Phase cal amplitudes by channel by station in band %d",
        bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      d->setHasData(true);
      fuzzyDescriptors_.append(d);

      // phases:
      d = adPhcPhs_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("PHCPHS_%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Phase cal phases (-18000 to 18000) by channel by station"
        " in band %d", bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      d->setHasData(true);
      fuzzyDescriptors_.append(d);

      // offsets:
      d = adPhcOff_ + bndIdx;        //12345678
      d->setLCode(QString("").sprintf("PHCOFF_%d", bndIdx + 1));
      d->setDescription(QString("").sprintf("Phase cal offsets (-18000 to 18000) by channel by station "
        "in band %d", bndIdx + 1));
      d->setDataScope(ADS_BASELINE);
      d->setDataType(ADT_R8);
      d->setDim1(numOfChannels);
      d->setDim2(2);
      d->setDim3(numOfObs_);
      d->setDim4(1);
      insertDatum(datumByKey_, d);
      d->setHasData(true);
      fuzzyDescriptors_.append(d);
      // end of phase cal info
    };
  };
};



//
void SgAgvDriver::fillDataStructures()
{
  if (!session_)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::fillDataStructures(): the session is NULL, nothing to do");
    return;
  };
  if (!session_->numberOfBands())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::fillDataStructures(): the session has no any band, nothing to do");
    return;
  };
  //

  numOfBln_ = session_->baselinesByName().size();
  numOfObs_ = session_->observations().size();
  numOfBands_ = session_->bands().size();
  //
  scanIdxByObsIdx_.resize(numOfObs_);
  //
  for (int i=0; i<numOfBands_; i++)
  {
    const QString&              bandKey=session_->bands().at(i)->getKey();
    numOfChansByBand_[bandKey] = session_->bands().at(i)->getMaxNumOfChannels();
    bndIdxByKey_[bandKey] = i;
  };
  int                           idx=0;
  for (int i=0; i<numOfObs_; i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    // set up scan indexes:
    // 
    if (!epochByScanId_.contains(obs->getScanId()))             // new scan
//  if (!epochByScanFullName_.contains(obs->getScanFullName())) // new scan
    {
      epochByScanId_.insert(obs->getScanId(), *obs);
//    epochByScanFullName_.insert(obs->getScanFullName(), *obs);
      idx++;
      scanNames_.append(obs->getScanName());
      scanFullNames_.append(obs->getScanFullName());
      crootFileNames_.append(obs->getCorrRootFileName());
      scanSrcNames_.append(obs->src()->getKey());
    };
    scanIdxByObsIdx_[i] = idx - 1;
  };
//numOfScans_ = scanFullNames_.size();
  numOfScans_ = epochByScanId_.size();
  numOfStn_ = session_->stationsByName().size();
  numOfSrc_ = session_->sourcesByName().size();
  
  numOfStnPts_ = -1;
  numOfChans2_ = -1;
  //
  //
  //
  // stations:
  idx = 0;
  for (QMap<QString, SgVlbiStationInfo*>::const_iterator it=session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it, idx++)
  {
    SgVlbiStationInfo        *stn=it.value();
    int                       num=stn->auxObservationByScanId()->size();
    stnNames_ << stn->getKey();
    numOfObsByStn_[stn->getKey()] = num;
    numOfObsBySidx_[idx] = num;
    stnIdxByKey_[stn->getKey()] = idx;
    if (maxNumPerStn_ < num)
      maxNumPerStn_ = num;
  };
  //
  //
  //
  // sources:
  idx = 0;
  for (QMap<QString, SgVlbiSourceInfo*>::const_iterator it=session_->sourcesByName().begin();
    it!=session_->sourcesByName().end(); ++it, idx++)
  {
    SgVlbiSourceInfo         *src=it.value();
    srcIdxByKey_[src->getKey()] = idx;
  };

  initMandatoryVars();
  initEssentialVars();
  allocateFuzzyDescriptors();

  // allocate data (all special sizes should be already resolved here):
  allocateData();

  // fill data:
  fillMandatoryVars();
  fillSessVars();
  fillScanVars();
  fillBslnVars();
  fillSttnVars();
  
  prepare4output();
  //
  SgAgvChunk                   *chunk;
  append(chunk = new SgAgvChunk(size()));
  
  chunk->fillDataStructures(session_, this);
};



//
void SgAgvDriver::prepare4output()
{
  // adjust first dimension (==length of a string) for strings:
  for (QMap<QString, SgAgvDatumString*>::iterator it=c1Data_.begin(); it!=c1Data_.end(); ++it)
  {
    SgAgvDatumString           *ds=it.value();
    SgAgvDatumDescriptor       *dd=ds->descriptor();
    int                         length=ds->value(0, 0, 0).size();
    for (int l=0; l<dd->d4(); l++)
      for (int k=0; k<dd->d3(); k++)
        for (int j=0; j<dd->d2(); j++)
          if (length < ds->value(j, k, l).size())
            length = ds->value(j, k, l).size();
    if (dd->getDim1() != length)
    {
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::prepare4output(): the length of strings has been changed from " + 
        QString("").setNum(dd->getDim1()) + " to " + QString("").setNum(length) + " for the lCode " +
        dd->getLCode());
      dd->setDim1(length);
    };
  };
  //
  // check records for presense of the data:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=datumByKey_.begin(); it!=datumByKey_.end(); 
    ++it)
  {
    SgAgvDatumDescriptor       *dd=it.value();
    switch (dd->getDataType())
    {
      case ADT_CHAR:
        if (c1Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!c1Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_I2:
        if (i2Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!i2Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_I4:
        if (i4Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!i4Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_I8:
        if (i8Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!i8Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_R4:
        if (r4Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!r4Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_R8:
        if (r8Data_.contains(dd->getLCode()) && !dd->getHasData())
          dd->setHasData(!r8Data_.value(dd->getLCode())->isEmpty());
      break;
      case ADT_NONE:
      default:
      
      break;
    };
  };
};



//
void SgAgvDriver::fillMandatoryVars()
{
  int                            idx=0;

  if (i4Data_.contains(adNobsSta_.getLCode()))
    for (QMap<QString, int>::iterator it=numOfObsByStn_.begin(); it!=numOfObsByStn_.end(); ++it, idx++)
      i4Data_.value(adNobsSta_.getLCode())->value(idx, 0, 0, 0) = it.value();

  if (i4Data_.contains(adNumbObs_.getLCode()))
    i4Data_.value(adNumbObs_.getLCode())->value(0, 0, 0, 0) = numOfObs_;

  if (i4Data_.contains(adNumbSca_.getLCode()))
    i4Data_.value(adNumbSca_.getLCode())->value(0, 0, 0, 0) = numOfScans_;

  if (i4Data_.contains(adNumbSta_.getLCode()))
    i4Data_.value(adNumbSta_.getLCode())->value(0, 0, 0, 0) = numOfStn_;

  if (i4Data_.contains(adObsTab_.getLCode()))
    for (int idx=0; idx<numOfObs_; idx++)
    {
      SgVlbiObservation        *obs=session_->observations().at(idx);
      i4Data_.value(adObsTab_.getLCode())->value(0, idx, 0, 0) = scanIdxByObsIdx_[idx] + 1;
      i4Data_.value(adObsTab_.getLCode())->value(1, idx, 0, 0) = stnIdxByKey_[obs->stn_1()->getKey()] + 1;
      i4Data_.value(adObsTab_.getLCode())->value(2, idx, 0, 0) = stnIdxByKey_[obs->stn_2()->getKey()] + 1;
    };
  adNobsSta_.setHasData(true);
  adNumbObs_.setHasData(true);
  adNumbSca_.setHasData(true);
  adNumbSta_.setHasData(true);
  adObsTab_.setHasData(true);
};



//
void SgAgvDriver::fillSessVars()
{
  QString                        str("");
  int                           idx=0, totChannels=0, numOfClockBreak=0;
  QList<QString>                refSites;
  QList<QString>                cBrkSites;
  QList<SgMJD>                  cBrkEpochs;
  //
  //
  //
  //                                           Stations:
  idx = 0;
  for (QMap<QString, int>::iterator it=stnIdxByKey_.begin(); it!=stnIdxByKey_.end(); ++it, idx++)
  {
    SgVlbiStationInfo           *stn=session_->stationsByName().value(it.key());
    if (stn)
    {
      // strings:
      if (c1Data_.contains(adSiteNames_.getLCode()))
        c1Data_.value(adSiteNames_.getLCode())->value(idx, 0, 0) = it.key();
      if (c1Data_.contains(adTectPlNm_.getLCode()))
        c1Data_.value(adTectPlNm_.getLCode())->value(idx, 0, 0) = stn->getTectonicPlateName();

      // r8:
      if (r8Data_.contains(adStnCoord_.getLCode()))
      {
        r8Data_.value(adStnCoord_.getLCode())->value(0, idx, 0, 0) = stn->getR().at(X_AXIS);
        r8Data_.value(adStnCoord_.getLCode())->value(1, idx, 0, 0) = stn->getR().at(Y_AXIS);
        r8Data_.value(adStnCoord_.getLCode())->value(2, idx, 0, 0) = stn->getR().at(Z_AXIS);
      };
      
      if (r8Data_.contains(adAtmInterv_.getLCode()))
        r8Data_.value(adAtmInterv_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getPcZenith().getPwlStep()*24.0;

      if (r8Data_.contains(adAtmConstr_.getLCode()))
        r8Data_.value(adAtmConstr_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getPcZenith().getPwlAPriori()/(1.0e-12*24.0*vLight*100.0);

      if (r8Data_.contains(adClkInterv_.getLCode()))
        r8Data_.value(adClkInterv_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getPcClocks().getPwlStep()*24.0;

      if (r8Data_.contains(adClkConstr_.getLCode()))
        r8Data_.value(adClkConstr_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getPcClocks().getPwlAPriori()/864.0;

      // i2:
      if (i2Data_.contains(adCableSgn_.getLCode()))
        i2Data_.value(adCableSgn_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getCableCalMultiplierDBCal();
      // cable cal and meteo origin info:
      if (i2Data_.contains(adCablOnTp_.getLCode()))
        i2Data_.value(adCablOnTp_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getCableCalsOrigin();
      if (i2Data_.contains(adMeteOnTp_.getLCode()))
        i2Data_.value(adMeteOnTp_.getLCode())->value(idx, 0, 0, 0) = 
          stn->getMeteoDataOrigin();
      // strings:
      if (c1Data_.contains(adCablOnTx_.getLCode()))
      {
        const QString          &s=stn->getCableCalsOriginTxt();
        c1Data_.value(adCablOnTx_.getLCode())->value(idx, 0, 0) = 
          s.size()?stn->getCableCalsOriginTxt():"_";
      };
      if (c1Data_.contains(adMeteOnTx_.getLCode()))
      {
        const QString          &s=stn->getMeteoDataOriginTxt();
        c1Data_.value(adMeteOnTx_.getLCode())->value(idx, 0, 0) = 
          s.size()?stn->getMeteoDataOriginTxt():"_";
      };

      // collect info:
      if (stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
        refSites << stn->getKey();

      // clock breaks:
      if (stn->clockBreaks().size())
      {
        stn->calcCBEpochs4Export(*stn->auxObservationByScanId());
        for (int j=0; j<stn->clockBreaks().size(); j++)
        {
          SgParameterBreak     *pb=stn->clockBreaks().at(j);
          cBrkSites << stn->getKey();
          cBrkEpochs << pb->getEpoch4Export();
        };
      };
      // station calibrations:
      // currently, we use only cable cal from the list:
      if (i2Data_.contains(adStnCalFlg_.getLCode()))
        i2Data_.value(adStnCalFlg_.getLCode())->value(idx, 0, 0, 0) =
          stn->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL) ? 0 : 1;
      // this is predetrimined also:
      if (i2Data_.contains(adFlybyFlg_.getLCode()))
      {
        i2Data_.value(adFlybyFlg_.getLCode())->value(idx, 0, 0, 0) = 1<<(8-1);
        for (int j=1; j<7; j++)
          i2Data_.value(adFlybyFlg_.getLCode())->value(idx, j, 0, 0) = 0;
      };
      //
      //
      //
      int                       jdx=0;
      for (QMap<QString, int>::iterator jt=stnIdxByKey_.begin(); jt!=stnIdxByKey_.end(); ++jt, jdx++)
      {
        SgVlbiStationInfo       *stn_2=session_->stationsByName().value(jt.key());
        if (stn_2)
        {
          QString                   st1Name(stn->getKey()), st2Name(stn_2->getKey());
          int                       bitArray;
          bitArray = 0;
          SgVlbiBaselineInfo       *bi, *biR;
          if (session_->baselinesByName().contains(st1Name + ":" + st2Name))
            bi = session_->baselinesByName().value(st1Name + ":" + st2Name);
          else
            bi = NULL;
          if (session_->baselinesByName().contains (st2Name + ":" + st1Name))
            biR = session_->baselinesByName().value(st2Name + ":" + st1Name);
          else
            biR = NULL;
          //
          //  ! *   Format of BASLSTAT_I2 array:                                       *
          //  ! *          Array BASLSTAT_I2  contains NUMSTA**2 elements, where       *
          //  ! *                NUMSTA is the number of stations.                     *
          //  ! *          Baselines ISIT1, ISIT2 has index (ISITE1-1)*NUMSTA + ISITE2 *
          //  ! *                    Where ISIT1, ISIT2 are indeces of the station     *
          //  ! *                    in interval [1, NUMSTA]. Stations index           *
          //  ! *                    correspond station name table kept in LCODE       *
          //  ! *                    SITNAMES. In general order of stations in this    *
          //  ! *                    station list MAY NOT coincide with order of       *
          //  ! *                    stations in the array ISITN_CHR kept in psfil.i !!*
          //  ! *          Each element of the array is 16-bits bit field.             *
          //  ! *          Bits are counted from 1.                                    *
          //  ! *          1-st bit is set when at least one observation at the        *
          //  ! *               baseline ISIT1, ISIT2 (but not ISIT2, ISIT1) took place*
          //  ! *          2-nd bit is set when baseline ISIT1, ISIT2 (or ISIT2, ISIT1)*
          //  ! *               was selected in group delay solution.                  *
          //  ! *          3-nd bit is set when baseline ISIT1, ISIT2 (or ISIT2, ISIT1)*
          //  ! *               was selected in phase delay solution.                  *
          //  ! *          4-th bit is set when baseline-dependent clock for the       *
          //  ! *               baseline ISIT1, ISIT2 (or ISIT2, ISIT1) was estimated  *
          //  ! *               in group delay solution.                               *
          //  ! *          5-th bit is set when baseline-dependent clock for the       *
          //  ! *               baseline ISIT1, ISIT2 (or ISIT2, ISIT1) was estimated  *
          //  ! *               in phase delay solution.                               *
          //  ! *          bits 6-16 are reserved for future used and always are zero. *
          //  !
          // set the first bit:
          if (bi && bi->numTotal(DT_DELAY))
            bitArray |= (1<<0);
          //
          // bi and biR can be NULLs!
          //
          // set the second and third bits:
          if ( ((bi  && !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))   ||
                (biR && !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))) && 
                (stn && !stn->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))   && 
              (stn_2 && !stn_2->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))  )
          {
            bitArray |= (1<<1);
            bitArray |= (1<<2);
          };
          // set the fourth and fifth bits:
          if ((bi  && 
               bi ->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) && 
              !bi ->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)) ||
              (biR && 
              biR->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) &&
              !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))  )
          {
            bitArray |= (1<<3);
            bitArray |= (1<<4);
          };
          if (i2Data_.contains(adBlnStatus_.getLCode()))
            i2Data_.value(adBlnStatus_.getLCode())->value(idx, jdx, 0, 0) = bitArray;
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
            "::fillSessVars(): cannot find a station \"" + jt.key() + "\" in the map");
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::fillSessVars(): cannot find a station \"" + it.key() + "\" in the map");
  
  // end of station iteration.
  };
  adSiteNames_.setHasData(true);

  if (c1Data_.contains(adRefClocks_.getLCode()) && refSites.size()>0)
  {
    adRefClocks_.setDim2(refSites.size());
    c1Data_.value(adRefClocks_.getLCode())->allocateSpace();
    for (int i=0; i<refSites.size(); i++)
      c1Data_.value(adRefClocks_.getLCode())->value(i, 0, 0) = refSites.at(i);
  };
  //
  //
  // store clock breaks:
  if ((numOfClockBreak = cBrkSites.size()))
  {
    //
    // enforce use of this LCode (it can be == 0.0 UTC):
    adCbrTime_.setHasData(true);
    //
    if (i4Data_.contains(adCbrNumber_.getLCode()))
      i4Data_.value(adCbrNumber_.getLCode())->value(0, 0, 0, 0) = numOfClockBreak;
    if (c1Data_.contains(adCbrSite_.getLCode()))
    {
      adCbrSite_.setDim2(numOfClockBreak);
      c1Data_.value(adCbrSite_.getLCode())->allocateSpace();
      for (int i=0; i<numOfClockBreak; i++)
        c1Data_.value(adCbrSite_.getLCode())->value(i, 0, 0) = cBrkSites.at(i);
    };
    if (i4Data_.contains(adCbrImjd_.getLCode()) && r8Data_.contains(adCbrTime_.getLCode()))
    {
      adCbrImjd_.setDim1(numOfClockBreak);
      adCbrTime_.setDim1(numOfClockBreak);
      i4Data_.value(adCbrImjd_.getLCode())->allocateSpace();
      r8Data_.value(adCbrTime_.getLCode())->allocateSpace();
      for (int i=0; i<numOfClockBreak; i++)
      {
        const SgMJD            &t=cBrkEpochs.at(i);
        i4Data_.value(adCbrImjd_.getLCode())->value(i, 0, 0, 0) = t.getDate();
        r8Data_.value(adCbrTime_.getLCode())->value(i, 0, 0, 0) = t.getTime()*86400.0;
      };
    };
  };

  // calibrations:
  if (c1Data_.contains(adStnCalNam_.getLCode()))
  {
    adStnCalNam_.setDim2(6);
    c1Data_.value(adStnCalNam_.getLCode())->allocateSpace();
    for (int i=0; i<6; i++)
    {
      c1Data_.value(adStnCalNam_.getLCode())->value(i, 0, 0) = sCalList[i];
      // special case:
      c1Data_.value(adStnCalNam_.getLCode())->value(i, 0, 0).replace(' ', '_');
    }
  };
  if (c1Data_.contains(adFlyCalNam_.getLCode()))
  {
    adFlyCalNam_.setDim2(8);
    c1Data_.value(adFlyCalNam_.getLCode())->allocateSpace();
    for (int i=0; i<8; i++)
      c1Data_.value(adFlyCalNam_.getLCode())->value(i, 0, 0) = sFclList[i];
  };

  int                           obsCalFlags;
  QList<QString>                calList;
  session_->formObsCalibrationSetup(obsCalFlags, calList);

  if (i2Data_.contains(adObsCalFlg_.getLCode()))
  {
    i2Data_.value(adObsCalFlg_.getLCode())->value(0, 0, 0, 0) = obsCalFlags;
  };
  if (c1Data_.contains(adObsCalNam_.getLCode()))
  {
    adObsCalNam_.setDim2(calList.size());
    c1Data_.value(adObsCalNam_.getLCode())->allocateSpace();
    for (int i=0; i<calList.size(); i++)
      c1Data_.value(adObsCalNam_.getLCode())->value(i, 0, 0) = calList.at(i);
  };
  calList.clear();

  //
  //                                           Sources:
  idx = 0;
  for (QMap<QString, SgVlbiSourceInfo*>::const_iterator it=session_->sourcesByName().begin();
    it!=session_->sourcesByName().end(); ++it, idx++)
  {
    SgVlbiSourceInfo           *src=it.value();
    // strings:
    if (c1Data_.contains(adSrcName_.getLCode()))
      c1Data_.value(adSrcName_.getLCode())->value(idx, 0, 0) = src->getKey();
    if (c1Data_.contains(adSrcApRef_.getLCode()))
      c1Data_.value(adSrcApRef_.getLCode())->value(idx, 0, 0) = src->getAprioriReference();
    // r8:
    if (r8Data_.contains(adSrcCoord_.getLCode()))
    {
      r8Data_.value(adSrcCoord_.getLCode())->value(0, idx, 0, 0) = src->getRA();
      r8Data_.value(adSrcCoord_.getLCode())->value(1, idx, 0, 0) = src->getDN();
    };
    //
    if (i2Data_.contains(adSrcStatus_.getLCode()))
      i2Data_.value(adSrcStatus_.getLCode())->value(idx, 0, 0, 0) = src->calculateStatusBits();
  };

 
  //
  //                                           Baselines:
  QList<QString>                baselineNames;
  QList<double>                 blWght4delays;
  QList<double>                 blWght4rates;
  QList<QString>                clocks4baselines;
  for (QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=session_->baselinesByName().begin(); 
    it!=session_->baselinesByName().end(); ++it)
  {
    SgVlbiBaselineInfo         *bi=it.value();
    str = bi->getKey();
    if (str.at(8) == ':')
      str.remove(8, 1);
    //
    baselineNames.append(str);
    blWght4delays.append(bi->getSigma2add(DT_DELAY));
    blWght4rates .append(bi->getSigma2add(DT_RATE ));
    //
    if (bi->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) && 
       !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)         )
      clocks4baselines << str;
  };
  //
  if (c1Data_.contains(adErrorBl_.getLCode()))
  {
    for (int i=0; i<numOfBln_; i++)
      c1Data_.value(adErrorBl_.getLCode())->value(i, 0, 0) = baselineNames.at(i);
  };
  if (r8Data_.contains(adErrorK_.getLCode()))
  {
    for (int i=0; i<numOfBln_; i++)
    {
      r8Data_.value(adErrorK_.getLCode())->value(0, i, 0, 0) = blWght4delays.at(i);
      r8Data_.value(adErrorK_.getLCode())->value(1, i, 0, 0) = blWght4rates.at(i);
    };
  };
  if (c1Data_.contains(adBlnClocks_.getLCode()) && clocks4baselines.size())
  {
    // adjust the size:
    adBlnClocks_.setDim2(clocks4baselines.size());
    c1Data_.value(adBlnClocks_.getLCode())->allocateSpace();
    for (int i=0; i<clocks4baselines.size(); i++)
      c1Data_.value(adBlnClocks_.getLCode())->value(i, 0, 0) = clocks4baselines.at(i);
  }
  else
  {
    //??
  };
  //
  //                                           Bands:
  for (int i=0; i<session_->bands().size(); i++)
  {
    const QString               &bandKey=session_->bands().at(i)->getKey();
    totChannels += numOfChansByBand_[bandKey];
    // string:
    if (c1Data_.contains(adBandNames_.getLCode()))
      c1Data_.value(adBandNames_.getLCode())->value(i, 0, 0) = bandKey;
    // i4:
    if (i4Data_.contains(adNumChnBnd_.getLCode()))
      i4Data_.value(adNumChnBnd_.getLCode())->value(0, i, 0, 0) = numOfChansByBand_[bandKey];
  };


  //
  //                                          General:
  if (c1Data_.contains(adCorrPlace_.getLCode()))
    c1Data_.value(adCorrPlace_.getLCode())->value(0, 0, 0) = session_->getCorrelatorName();
  if (c1Data_.contains(adCorrType_.getLCode()))
    c1Data_.value(adCorrType_.getLCode())->value(0, 0, 0) = session_->getCorrelatorType();
  if (c1Data_.contains(adExpCode_.getLCode()))
    c1Data_.value(adExpCode_.getLCode())->value(0, 0, 0) = session_->getSessionCode();
  if (c1Data_.contains(adExpDesc_.getLCode()))
    c1Data_.value(adExpDesc_.getLCode())->value(0, 0, 0) = session_->getDescription();
  //
  //??
  if (c1Data_.contains(adExpName_.getLCode()))
    c1Data_.value(adExpName_.getLCode())->value(0, 0, 0) = session_->getSessionCode();
  //
  if (c1Data_.contains(adMk3DbName_.getLCode()))
    c1Data_.value(adMk3DbName_.getLCode())->value(0, 0, 0) = session_->getName();
  if (c1Data_.contains(adPiName_.getLCode()))
    c1Data_.value(adPiName_.getLCode())->value(0, 0, 0) = session_->getPiAgencyName();
  if (c1Data_.contains(adRecMode_.getLCode()))
    c1Data_.value(adRecMode_.getLCode())->value(0, 0, 0) = session_->getRecordingMode();
  // i2:
  if (i2Data_.contains(adExpSerNo_.getLCode()))
    i2Data_.value(adExpSerNo_.getLCode())->value(0, 0, 0, 0) = session_->getExperimentSerialNumber();
  // i4:
  if (i4Data_.contains(adNumSrc_.getLCode()))
    i4Data_.value(adNumSrc_.getLCode())->value(0, 0, 0, 0) = srcIdxByKey_.size();
  if (i4Data_.contains(adNumBands_.getLCode()))
    i4Data_.value(adNumBands_.getLCode())->value(0, 0, 0, 0) = numOfBands_;
  if (i4Data_.contains(adNumChnTot_.getLCode()))
    i4Data_.value(adNumChnTot_.getLCode())->value(0, 0, 0, 0) = totChannels;
  if (i4Data_.contains(adNumAvaBnd_.getLCode()))
    i4Data_.value(adNumAvaBnd_.getLCode())->value(0, 0, 0, 0) = numOfBands_;
  //
  // added:
  if (i2Data_.contains(adInterval4_.getLCode()))
  {
    int                         ep[5];
    double                      f;
    session_->getTStart().toYMDHMS_tr(ep[0], ep[1], ep[2], ep[3], ep[4], f);
    for (int i=0; i<5; i++)
      i2Data_.value(adInterval4_.getLCode())->value(0, i, 0, 0) = ep[i];
    session_->getTFinis().toYMDHMS_tr(ep[0], ep[1], ep[2], ep[3], ep[4], f);
    for (int i=0; i<5; i++)
      i2Data_.value(adInterval4_.getLCode())->value(1, i, 0, 0) = ep[i];
  };

  if (r8Data_.contains(adUtcMtai_.getLCode()))
    r8Data_.value(adUtcMtai_.getLCode())->value(0, 0, 0, 0) = -session_->getLeapSeconds();// reverse sign


  //4CALC:
  if (c1Data_.contains(adCalcFlgN_.getLCode()) && i2Data_.contains(adCalcFlgV_.getLCode()))
  {
    for (int i=0; i<session_->calcInfo().controlFlagNames().size(); i++)
    {
      i2Data_.value(adCalcFlgV_.getLCode())->value(i, 0, 0, 0) = 
        session_->calcInfo().controlFlagValues().at(i);
      c1Data_.value(adCalcFlgN_.getLCode())->value(i, 0, 0) =
        session_->calcInfo().controlFlagNames().at(i);
    };
  };
  
  if (i2Data_.contains(adTidalUt1_.getLCode()))
    i2Data_.value(adTidalUt1_.getLCode())->value(0, 0, 0, 0) = session_->calcInfo().getFlagTidalUt1();
  if (r8Data_.contains(adCalcVer_.getLCode()))
    r8Data_.value(adCalcVer_.getLCode())->value(0, 0, 0, 0) = session_->calcInfo().getDversion();
  if (r8Data_.contains(adRelData_.getLCode()))
    r8Data_.value(adRelData_.getLCode())->value(0, 0, 0, 0) = session_->calcInfo().getRelativityData();
  if (r8Data_.contains(adPreData_.getLCode()))
    r8Data_.value(adPreData_.getLCode())->value(0, 0, 0, 0) = session_->calcInfo().getPrecessionData();
  if (r8Data_.contains(adEtdData_.getLCode()))
  {
    r8Data_.value(adEtdData_.getLCode())->value(0, 0, 0, 0) = session_->calcInfo().getEarthTideData(0);
    r8Data_.value(adEtdData_.getLCode())->value(1, 0, 0, 0) = session_->calcInfo().getEarthTideData(1);
    r8Data_.value(adEtdData_.getLCode())->value(2, 0, 0, 0) = session_->calcInfo().getEarthTideData(2);
  };
  //
  setCalcInfoModelMessNCtrlF(adAtmMess_.getLCode(), adAtmCflg_.getLCode(), 
    session_->calcInfo().troposphere());
  setCalcInfoModelMessNCtrlF(adAxoMess_.getLCode(), adAxoCflg_.getLCode(), 
    session_->calcInfo().axisOffset());
  setCalcInfoModelMessNCtrlF(adEtdMess_.getLCode(), adEtdCflg_.getLCode(), 
    session_->calcInfo().earthTide());
  setCalcInfoModelMessNCtrlF(adPtdMess_.getLCode(), adPtdCflg_.getLCode(), 
    session_->calcInfo().poleTide());
  setCalcInfoModelMessNCtrlF(adNutMess_.getLCode(), adNutCflg_.getLCode(), 
    session_->calcInfo().nutation());
  setCalcInfoModelMessNCtrlF(adOceMess_.getLCode(), adOceCflg_.getLCode(), 
    session_->calcInfo().oceanLoading());
  setCalcInfoModelMessNCtrlF(adAtiMess_.getLCode(), adAtiCflg_.getLCode(), 
    session_->calcInfo().atomicTime());
  setCalcInfoModelMessNCtrlF(adCtiMess_.getLCode(), adCtiCflg_.getLCode(), 
    session_->calcInfo().coordinateTime());
  setCalcInfoModelMessNCtrlF(adPlxMess_.getLCode(), adPlxCflg_.getLCode(), 
    session_->calcInfo().parallax());
  setCalcInfoModelMessNCtrlF(adStrMess_.getLCode(), adStrCflg_.getLCode(), 
    session_->calcInfo().star());
  setCalcInfoModelMessNCtrlF(adTheMess_.getLCode(), adRelCflg_.getLCode(), 
    session_->calcInfo().relativity());

  if (c1Data_.contains(adSitMess_.getLCode()))
    c1Data_.value(adSitMess_.getLCode())->value(0, 0, 0) = 
      session_->calcInfo().site().getDefinition();
  if (c1Data_.contains(adPanMess_.getLCode()))
    c1Data_.value(adPanMess_.getLCode())->value(0, 0, 0) = 
      session_->calcInfo().feedHorn().getDefinition();
  if (c1Data_.contains(adPepMess_.getLCode()))
    c1Data_.value(adPepMess_.getLCode())->value(0, 0, 0) = 
      session_->calcInfo().ephemeris().getDefinition();
  if (c1Data_.contains(adWobCflg_.getLCode()))
    c1Data_.value(adWobCflg_.getLCode())->value(0, 0, 0) = 
      session_->calcInfo().polarMotion().getControlFlag();
  if (c1Data_.contains(adUt1Cflg_.getLCode()))
    c1Data_.value(adUt1Cflg_.getLCode())->value(0, 0, 0) = 
      session_->calcInfo().ut1().getControlFlag();

  if (c1Data_.contains(adOceStat_.getLCode()))
    for (int i=0; i<session_->calcInfo().oLoadStationStatus().size(); i++)
      c1Data_.value(adOceStat_.getLCode())->value(i, 0, 0) = 
        session_->calcInfo().oLoadStationStatus().at(i);

  if (r8Data_.contains(adSiteZens_.getLCode()))
    for (int i=0; i<session_->calcInfo().siteZenDelays().size(); i++)
      r8Data_.value(adSiteZens_.getLCode())->value(i, 0, 0, 0) = 
        session_->calcInfo().siteZenDelays().at(i);

  if (r8Data_.contains(adUt1Epoch_.getLCode()))
    for (unsigned int i=0; i<session_->calcInfo().ut1InterpData()->nRow(); i++)
      for (unsigned int j=0; j<session_->calcInfo().ut1InterpData()->nCol(); j++)
        r8Data_.value(adUt1Epoch_.getLCode())->value(j, i, 0, 0) = 
          session_->calcInfo().ut1InterpData()->getElement(i, j);
  
  if (r8Data_.contains(adWobEpoch_.getLCode()))
    for (unsigned int i=0; i<session_->calcInfo().wobInterpData()->nRow(); i++)
      for (unsigned int j=0; j<session_->calcInfo().wobInterpData()->nCol(); j++)
        r8Data_.value(adWobEpoch_.getLCode())->value(j, i, 0, 0) = 
          session_->calcInfo().wobInterpData()->getElement(i, j);

  for (int i=0; i<session_->calcInfo().stations().size(); i++)
  {
    SgVlbiStationInfo        *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      if (r8Data_.contains(adSitOcPhs_.getLCode()) &&
          r8Data_.contains(adSitOcAmp_.getLCode()) &&
          r8Data_.contains(adSitHWOPh_.getLCode()) &&
          r8Data_.contains(adSitHWOAm_.getLCode()) &&
          r8Data_.contains(adSitHSOPh_.getLCode()) &&
          r8Data_.contains(adSitHSOAm_.getLCode())  )
        for (int j=0; j<11; j++)
        {
          r8Data_.value(adSitOcPhs_.getLCode())->value(i, j, 0, 0) = si->getOLoadPhase(j, 0);
          r8Data_.value(adSitHWOPh_.getLCode())->value(i, j, 0, 0) = si->getOLoadPhase(j, 1);
          r8Data_.value(adSitHSOPh_.getLCode())->value(i, j, 0, 0) = si->getOLoadPhase(j, 2);
          r8Data_.value(adSitOcAmp_.getLCode())->value(i, j, 0, 0) = si->getOLoadAmplitude(j, 0);
          r8Data_.value(adSitHWOAm_.getLCode())->value(i, j, 0, 0) = si->getOLoadAmplitude(j, 1);
          r8Data_.value(adSitHSOAm_.getLCode())->value(i, j, 0, 0) = si->getOLoadAmplitude(j, 2);
        };
      //
      if (r8Data_.contains(adOPTLCoef_.getLCode()))
        for (int j=0; j<6; j++)
          r8Data_.value(adOPTLCoef_.getLCode())->value(i, j, 0, 0) = si->getOptLoadCoeff(j);
      
      if (i2Data_.contains(adAxisTyps_.getLCode()))
        i2Data_.value(adAxisTyps_.getLCode())->value(i, 0, 0, 0) = 
          SgVlbiStationInfo::mntType2int(si->getMntType());
      
      if (r8Data_.contains(adAxisOffs_.getLCode()))
        r8Data_.value(adAxisOffs_.getLCode())->value(i, 0, 0, 0) = si->getAxisOffset();

      if (r8Data_.contains(adAxisTilt_.getLCode()))
      {
        r8Data_.value(adAxisTilt_.getLCode())->value(i, 0, 0, 0) = si->getTilt(0);
        r8Data_.value(adAxisTilt_.getLCode())->value(i, 1, 0, 0) = si->getTilt(1);
      };
      
      if (c1Data_.contains(adEccTypes_.getLCode()))
        c1Data_.value(adEccTypes_.getLCode())->value(i, 0, 0) = 
          si->eccRec()->getEccType()==SgEccRec::ET_XYZ ? "XY" : 
            si->eccRec()->getEccType()==SgEccRec::ET_NEU ? "NE" : "N/A";
      if (r8Data_.contains(adEccCoord_.getLCode()))
      {
        r8Data_.value(adEccCoord_.getLCode())->value(i, 0, 0, 0) = si->eccRec()->getDR().at(X_AXIS);
        r8Data_.value(adEccCoord_.getLCode())->value(i, 1, 0, 0) = si->eccRec()->getDR().at(Y_AXIS);
        r8Data_.value(adEccCoord_.getLCode())->value(i, 2, 0, 0) = si->eccRec()->getDR().at(Z_AXIS);
      };
      if (c1Data_.contains(adEccNames_.getLCode()))
        c1Data_.value(adEccNames_.getLCode())->value(i, 0, 0) = 
          si->eccRec()->getCdpNumber().leftJustified(10, ' ');
      
      
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::fillSessVars(): the stnInfo is NULL for the idx=" + QString("").setNum(i));
  };

  if (r8Data_.contains(adFwobInf_.getLCode())  && 
      r8Data_.contains(adFut1Inf_.getLCode())  && 
      c1Data_.contains(adUt1Intrp_.getLCode()) && 
      c1Data_.contains(adUt1Mess_.getLCode())  && 
      c1Data_.contains(adWobMess_.getLCode())  && 
      c1Data_.contains(adWobIntrp_.getLCode()) && 
      c1Data_.contains(adFut1Text_.getLCode()) && 
      c1Data_.contains(adFwobText_.getLCode()) && 
      r8Data_.contains(adFut1Pts_.getLCode())  && 
      r8Data_.contains(adFwobXnYt_.getLCode())  )
  {
    if (!session_->args4Ut1Interpolation())
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::fillSessVars(): the set of argumets for UT1 interpolation is not defined");
    else if (!session_->args4PxyInterpolation())
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::fillSessVars(): the set of argumets for Polar motion interpolation is not defined");
    else
    {
      double                       d;
      // 0:
      d = session_->args4Ut1Interpolation()->getElement(0);
      r8Data_.value(adFut1Inf_.getLCode())->value(0, 0, 0, 0) = d>2390000.0?d:d+2400000.5;
      d = session_->args4PxyInterpolation()->getElement(0);
      r8Data_.value(adFwobInf_.getLCode())->value(0, 0, 0, 0) = d>2390000.0?d:d+2400000.5;
      // 1:
      r8Data_.value(adFut1Inf_.getLCode())->value(1, 0, 0, 0) = 
        session_->args4Ut1Interpolation()->n()>1?
          (session_->args4Ut1Interpolation()->getElement(1) - 
            session_->args4Ut1Interpolation()->getElement(0)):1.0;
      r8Data_.value(adFwobInf_.getLCode())->value(1, 0, 0, 0) =   
        session_->args4PxyInterpolation()->n()>1?
          (session_->args4PxyInterpolation()->getElement(1) - 
            session_->args4PxyInterpolation()->getElement(0)):1.0;
      // 2:
      r8Data_.value(adFut1Inf_.getLCode())->value(2, 0, 0, 0) = 
        session_->args4Ut1Interpolation()->n();
      r8Data_.value(adFwobInf_.getLCode())->value(2, 0, 0, 0) =   
        session_->args4PxyInterpolation()->n();
      // 3:
      r8Data_.value(adFut1Inf_.getLCode())->value(3, 0, 0, 0) = 1.0;
      //
      for (unsigned int i=0; i<session_->tabs4Ut1Interpolation()->nRow(); i++)
        r8Data_.value(adFut1Pts_.getLCode())->value(i, 0, 0, 0) =
          session_->tabs4Ut1Interpolation()->getElement(i, 0);
      for (unsigned int i=0; i<session_->tabs4PxyInterpolation()->nRow(); i++)
      {
        r8Data_.value(adFwobXnYt_.getLCode())->value(i, 0, 0, 0) = 
          session_->tabs4PxyInterpolation()->getElement(i, 0);
        r8Data_.value(adFwobXnYt_.getLCode())->value(i, 1, 0, 0) = 
          session_->tabs4PxyInterpolation()->getElement(i, 1);
      };
      if (c1Data_.contains(adUt1Intrp_.getLCode()))
        c1Data_.value(adUt1Intrp_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().ut1Interpolation().getControlFlag();
      if (c1Data_.contains(adWobIntrp_.getLCode()))
        c1Data_.value(adWobIntrp_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().polarMotionInterpolation().getControlFlag();

      if (c1Data_.contains(adUt1Mess_.getLCode()))
        c1Data_.value(adUt1Mess_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().ut1Interpolation().getDefinition();
      if (c1Data_.contains(adWobMess_.getLCode()))
        c1Data_.value(adWobMess_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().polarMotionInterpolation().getDefinition();

      if (c1Data_.contains(adFut1Text_.getLCode()))
        c1Data_.value(adFut1Text_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().ut1Interpolation().getOrigin();
      if (c1Data_.contains(adFwobText_.getLCode()))
        c1Data_.value(adFwobText_.getLCode())->value(0, 0, 0) = 
          session_->calcInfo().polarMotionInterpolation().getOrigin();
    
    };
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::fillSessVars(): no ERP interpolation data to store");
};



//
void SgAgvDriver::fillScanVars()
{
  bool                          hasScanNames, hasMjdObs, hasSrcIdx, hasUtcObs;
  bool                          hasCrootFnam, hasFScanName;
  int                           idx;

  hasScanNames = c1Data_.contains(adScanNames_.getLCode());
  hasMjdObs = i4Data_.contains(adMjdObs_.getLCode());
  hasSrcIdx = i4Data_.contains(adSrcIdx_.getLCode());
  hasUtcObs = r8Data_.contains(adUtcObs_.getLCode());
  // added:
  hasCrootFnam = c1Data_.contains(adCrootFnam_.getLCode());
  hasFScanName = c1Data_.contains(adFScanName_.getLCode());

  // per scan:
  //  for (int i=0; i<scanFullNames_.size(); i++)
  
  idx = 0;
  for (QMap<QString, SgMJD>::iterator it=epochByScanId_.begin(); it!=epochByScanId_.end(); ++it, idx++)
  {
    //const QString            &scanId=it.key();
    //    
    //const QString            &scnFullName=scanFullNames_.at(i);
    //const QString            &scnName=scanNames_.at(i);
    // string:                 here, the scan idx is #3
    if (hasScanNames)
      c1Data_.value(adScanNames_.getLCode())->value(0, idx, 0) = qPrintable(scanNames_.at(idx));
    // added:
    if (hasCrootFnam)
      c1Data_.value(adCrootFnam_.getLCode())->value(0, idx, 0) = qPrintable(crootFileNames_.at(idx));
    if (hasFScanName)
      c1Data_.value(adFScanName_.getLCode())->value(0, idx, 0) = qPrintable(scanFullNames_.at(idx));
  
    // i4:
    if (hasMjdObs)
      i4Data_.value(adMjdObs_.getLCode())->value(0, 0, idx, 0) = it.value().getDate();
//      epochByScanFullName_[scnFullName].getDate();
    if (hasSrcIdx)
      i4Data_.value(adSrcIdx_.getLCode())->value(0, 0, idx, 0) = srcIdxByKey_[scanSrcNames_.at(idx)] + 1;

    // R8:
    if (hasUtcObs)
      r8Data_.value(adUtcObs_.getLCode())->value(0, 0, idx, 0) = 
//      epochByScanFullName_[scnFullName].getTime()*86400.0;
        it.value().getTime()*86400.0;
  };
};



//
void SgAgvDriver::fillBslnVars()
{
  QString                       str("");
  int                           nYear, nMonth, nDay, nHour, nMin, scanIdx;
  double                        dSec;
  QMap<QString, SgMJD>          epochByScanId;
  epochByScanId.clear();
  scanIdx = 0;

  // per obs:
  for (int i=0; i<numOfObs_; i++)
  {
    SgVlbiObservation        *obs=session_->observations().at(i);
    
    // +per band:
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin();
      it!=obs->observableByKey().end(); ++it)
    {
      SgVlbiObservable       *o=it.value();
      const QString           &bandKey=o->getBandKey();
      int                     bndIdx=bndIdxByKey_[bandKey];
      //
      // string:                 here, the obs idx is #3
      if (c1Data_.contains(adQualCodes_.getLCode()))
        c1Data_.value(adQualCodes_.getLCode())->value(bndIdx, i, 0) = 
          QString("").sprintf("%d", o->getQualityFactor());
      if (c1Data_.contains(adFourfCmd_.getLCode()))
        c1Data_.value(adFourfCmd_.getLCode())->value(bndIdx, i, 0) = o->getFourfitCommandOverride();
      if (c1Data_.contains(adFourfCtrl_.getLCode()))
        c1Data_.value(adFourfCtrl_.getLCode())->value(bndIdx, i, 0) = o->getFourfitControlFile();

      // reals:
      // SB delays:
      if (r8Data_.contains(adSbDelVal_.getLCode()))
        r8Data_.value(adSbDelVal_.getLCode())->value(bndIdx, 0, i, 0) = o->sbDelay().getValue();
      if (r8Data_.contains(adSbDelErr_.getLCode()))
        r8Data_.value(adSbDelErr_.getLCode())->value(bndIdx, 0, i, 0) = o->sbDelay().getSigma();

      // group delays:
      if (r8Data_.contains(adGrDelVal_.getLCode()))
        r8Data_.value(adGrDelVal_.getLCode())->value(bndIdx, 0, i, 0) = o->grDelay().getValue();
      if (r8Data_.contains(adGrDelErr_.getLCode()))
        r8Data_.value(adGrDelErr_.getLCode())->value(bndIdx, 0, i, 0) = o->grDelay().getSigma();

      // delay rates:
      if (r8Data_.contains(adDelRate_.getLCode()))
        r8Data_.value(adDelRate_.getLCode())->value(bndIdx, 0, i, 0) = o->phDRate().getValue();
      if (r8Data_.contains(adPhRatErr_.getLCode()))
        r8Data_.value(adPhRatErr_.getLCode())->value(bndIdx, 0, i, 0) = o->phDRate().getSigma();
      
      // aux:
      if (r8Data_.contains(adGrDelAmbg_.getLCode()))
        r8Data_.value(adGrDelAmbg_.getLCode())->value(bndIdx, 0, i, 0) = 
        o->grDelay().getAmbiguitySpacing();
      if (r8Data_.contains(adRefFreq_.getLCode()))  // MHz->Hz
        r8Data_.value(adRefFreq_.getLCode())->value(bndIdx, 0, i, 0) = o->getReferenceFrequency()*1.0E6;
      if (r8Data_.contains(adSnr_.getLCode()))
        r8Data_.value(adSnr_.getLCode())->value(bndIdx, 0, i, 0) = o->getSnr();
      if (r8Data_.contains(adTotPhase_.getLCode()))
        r8Data_.value(adTotPhase_.getLCode())->value(bndIdx, 0, i, 0) = o->getTotalPhase();
      
      // added:
      if (r8Data_.contains(adCorrelatn_.getLCode()))
        r8Data_.value(adCorrelatn_.getLCode())->value(bndIdx, 0, i, 0) = o->getCorrCoeff(); 

      if (r8Data_.contains(adFrnAmpl_.getLCode()))
        r8Data_.value(adFrnAmpl_.getLCode())->value(bndIdx, 0, i, 0) = o->getCorrCoeff();

      if (i2Data_.contains(adNumLags_.getLCode()))
        i2Data_.value(adNumLags_.getLCode())->value(bndIdx, 0, i, 0) = o->getNlags();

      if (r8Data_.contains(adUvf_Asec_.getLCode()))
      {
        r8Data_.value(adUvf_Asec_.getLCode())->value(bndIdx, 0, i, 0) = o->getUvFrPerAsec(0);
        r8Data_.value(adUvf_Asec_.getLCode())->value(bndIdx, 1, i, 0) = o->getUvFrPerAsec(1);
      };

      if (r8Data_.contains(adUnPhaseCl_.getLCode()))
      {
        r8Data_.value(adUnPhaseCl_.getLCode())->value(bndIdx, 0, i, 0) = o->getPhaseCalGrDelays(0); 
        r8Data_.value(adUnPhaseCl_.getLCode())->value(bndIdx, 1, i, 0) = o->getPhaseCalGrDelays(1); 
      };
      if (r8Data_.contains(adSamplRate_.getLCode()))
        r8Data_.value(adSamplRate_.getLCode())->value(bndIdx, 0, i, 0) = o->getSampleRate(); 
      
      //
      //
      // ionosphere corrections:
      //
      if (r8Data_.contains(adIonCorr_.getLCode()))
      {
        r8Data_.value(adIonCorr_.getLCode())->value(0, bndIdx, i, 0) = o->grDelay().getIonoValue();
        r8Data_.value(adIonCorr_.getLCode())->value(1, bndIdx, i, 0) = o->phDRate().getIonoValue();
      };
      if (r8Data_.contains(adIonRms_.getLCode()))
      {
        r8Data_.value(adIonRms_.getLCode())->value(0, bndIdx, i, 0) = o->grDelay().getIonoSigma();
        r8Data_.value(adIonRms_.getLCode())->value(1, bndIdx, i, 0) = o->phDRate().getIonoSigma();
      };
      if (i2Data_.contains(adIonDtFlg_.getLCode()))
      {
        short int               flg=-1;
        if (obs->observableByKey().size()==1)
          flg = -1;
        else
        {
          flg = 0;
          //The -3 flag is set if
          //ION_DELAY_SIGMA  < 1.d-14    or ION_RATE_SIGMA  < 1.d-17
          if (o->grDelay().getIonoSigma()<1.0e-14 || o->phDRate().getIonoSigma()<1.0e-17)
            flg = -3;
          //The -4 flag is set if
          //ION_DELAY_SIGMA >1.d-8  or ION_RATE_SIGMA > 1.d-8
          if (o->grDelay().getIonoSigma()>1.0e-8 || o->phDRate().getIonoSigma()>1.0e-8)
            flg = -4;
        };
        i2Data_.value(adIonDtFlg_.getLCode())->value(bndIdx, 0, i, 0) = flg;
      };
      //
      // end of ionosphere corrections.
      //

      if (r8Data_.contains(adEffFreq_.getLCode()))
      {
        r8Data_.value(adEffFreq_.getLCode())->value(0, bndIdx, i, 0) = o->grDelay().getEffFreq()*1.0e6;
        r8Data_.value(adEffFreq_.getLCode())->value(1, bndIdx, i, 0) = o->phDelay().getEffFreq()*1.0e6;
        r8Data_.value(adEffFreq_.getLCode())->value(2, bndIdx, i, 0) = o->phDRate().getEffFreq()*1.0e6;
      };
      if (r8Data_.contains(adEffFrew_.getLCode()))
      {
        r8Data_.value(adEffFrew_.getLCode())->value(0, bndIdx, i, 0) = 
          o->grDelay().getEffFreqEqWgt()*1.0e6;
        r8Data_.value(adEffFrew_.getLCode())->value(1, bndIdx, i, 0) = 
          o->phDelay().getEffFreqEqWgt()*1.0e6;
        r8Data_.value(adEffFrew_.getLCode())->value(2, bndIdx, i, 0) = 
          o->phDRate().getEffFreqEqWgt()*1.0e6;
      };
      
      if (i4Data_.contains(adNumGrAmbg_.getLCode()))
        i4Data_.value(adNumGrAmbg_.getLCode())->value(bndIdx, 0, i, 0) = 
          o->grDelay().getNumOfAmbiguities();
      
      if (i4Data_.contains(adNumPhAmbg_.getLCode()))
        i4Data_.value(adNumPhAmbg_.getLCode())->value(bndIdx, 0, i, 0) = 
          o->phDelay().getNumOfAmbiguities();
    
      // correlator info data:
      if (c1Data_.contains(adFourfErr_.getLCode()))
        c1Data_.value(adFourfErr_.getLCode())->value(bndIdx, i, 0) = 
          o->getErrorCode()==" "?"_":o->getErrorCode();
      if (c1Data_.contains(adFourfFile_.getLCode()))
        c1Data_.value(adFourfFile_.getLCode())->value(bndIdx, i, 0) = 
          qPrintable(o->getFourfitOutputFName());
      if (i2Data_.contains(adStrtOffst_.getLCode()))
        i2Data_.value(adStrtOffst_.getLCode())->value(bndIdx, 0, i, 0) = o->getStartOffset();
      if (i2Data_.contains(adStopOffst_.getLCode()))
        i2Data_.value(adStopOffst_.getLCode())->value(bndIdx, 0, i, 0) = o->getStopOffset();
//
//    obs->setCorrRootFileName(vlb1FileNames.at(obsIdx));
//    obs->auxObs_1()->setTapeId(tapeIds_1.at(obsIdx));
//    obs->auxObs_2()->setTapeId(tapeIds_2.at(obsIdx));
//
      if (c1Data_.contains(adTapQcode_.getLCode()))
        c1Data_.value(adTapQcode_.getLCode())->value(bndIdx, i, 0) = 
          QString("").sprintf("%-6s", qPrintable(o->getTapeQualityCode()));

      if (i2Data_.contains(adHopsVer_.getLCode()))
        i2Data_.value(adHopsVer_.getLCode())->value(bndIdx, 0, i, 0) = o->getHopsRevisionNumber();
      if (i2Data_.contains(adFourFtVer_.getLCode()))
      {
        i2Data_.value(adFourFtVer_.getLCode())->value(bndIdx, 0, i, 0) = o->getFourfitVersion(0);
        i2Data_.value(adFourFtVer_.getLCode())->value(bndIdx, 1, i, 0) = o->getFourfitVersion(1);
      };
      if (i2Data_.contains(adScanUtc_.getLCode()) && tZero < o->getEpochOfScan())
      {
        o->getEpochOfScan().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 0, i, 0) = nYear;
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 1, i, 0) = nMonth;
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 2, i, 0) = nDay;
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 3, i, 0) = nHour;
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 4, i, 0) = nMin;
        i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 5, i, 0) = (int)trunc(dSec);
      };
      if (i2Data_.contains(adUtcMtag_.getLCode()) && tZero < o->getEpochCentral())
      {
        o->getEpochCentral().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 0, i, 0) = nYear;
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 1, i, 0) = nMonth;
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 2, i, 0) = nDay;
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 3, i, 0) = nHour;
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 4, i, 0) = nMin;
        i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 5, i, 0) = (int)trunc(dSec);
      };
      if (i2Data_.contains(adCorrUtc_.getLCode())  && tZero < o->getEpochOfCorrelation())
      {
        o->getEpochOfCorrelation().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 0, i, 0) = nYear;
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 1, i, 0) = nMonth;
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 2, i, 0) = nDay;
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 3, i, 0) = nHour;
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 4, i, 0) = nMin;
        i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 5, i, 0) = (int)trunc(dSec);
      };
      if (i2Data_.contains(adFourUtc_.getLCode()) && tZero < o->getEpochOfFourfitting())
      {
        o->getEpochOfFourfitting().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 0, i, 0) = nYear;
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 1, i, 0) = nMonth;
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 2, i, 0) = nDay;
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 3, i, 0) = nHour;
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 4, i, 0) = nMin;
        i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 5, i, 0) = (int)trunc(dSec);
      };
      // reals:
      if (r8Data_.contains(adQbFactor_.getLCode()))
        r8Data_.value(adQbFactor_.getLCode())->value(bndIdx, 0, i, 0) = o->getAcceptedRatio();
      if (r8Data_.contains(adDiscard_.getLCode()))
        r8Data_.value(adDiscard_.getLCode())->value(bndIdx, 0, i, 0) = o->getDiscardRatio();
      if (r8Data_.contains(adDeltaEpo_.getLCode()))
        r8Data_.value(adDeltaEpo_.getLCode())->value(bndIdx, 0, i, 0) = o->getCentrOffset();
      if (r8Data_.contains(adStartSec_.getLCode()))
      {
        o->getTstart().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        r8Data_.value(adStartSec_.getLCode())->value(bndIdx, 0, i, 0) = 60.0*nMin + dSec;
      };
      if (r8Data_.contains(adStopSec_.getLCode()))
      {
        o->getTstop().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
        r8Data_.value(adStopSec_.getLCode())->value(bndIdx, 0, i, 0) = 60.0*nMin + dSec;
      };
      if (r8Data_.contains(adSbResid_.getLCode()))
        r8Data_.value(adSbResid_.getLCode())->value(bndIdx, 0, i, 0) = 
          o->sbDelay().getResidualFringeFitting();
      if (r8Data_.contains(adGrResid_.getLCode()))
        r8Data_.value(adGrResid_.getLCode())->value(bndIdx, 0, i, 0) =
          o->grDelay().getResidualFringeFitting();
      if (r8Data_.contains(adRtResid_.getLCode()))
        r8Data_.value(adRtResid_.getLCode())->value(bndIdx, 0, i, 0) =
          o->phDRate().getResidualFringeFitting();
      if (r8Data_.contains(adEffDura_.getLCode()))
        r8Data_.value(adEffDura_.getLCode())->value(bndIdx, 0, i, 0) = o->getEffIntegrationTime();
      if (r8Data_.contains(adFalseDet_.getLCode()))
        r8Data_.value(adFalseDet_.getLCode())->value(bndIdx, 0, i, 0) = o->getProbabOfFalseDetection();
      if (r8Data_.contains(adGcSbDel_.getLCode()))
        r8Data_.value(adGcSbDel_.getLCode())->value(bndIdx, 0, i, 0) = o->sbDelay().getGeocenterValue();
      if (r8Data_.contains(adGcMbDel_.getLCode()))
        r8Data_.value(adGcMbDel_.getLCode())->value(bndIdx, 0, i, 0) = o->grDelay().getGeocenterValue();
      if (r8Data_.contains(adGcRate_.getLCode()))
        r8Data_.value(adGcRate_.getLCode())->value(bndIdx, 0, i, 0) = o->phDRate().getGeocenterValue();
      if (r8Data_.contains(adGcPhase_.getLCode()))
        r8Data_.value(adGcPhase_.getLCode())->value(bndIdx, 0, i, 0) = 
          o->getGeocenterTotalPhase()*RAD2DEG;
      if (r8Data_.contains(adGcPhsRes_.getLCode()))
        r8Data_.value(adGcPhsRes_.getLCode())->value(bndIdx, 0, i, 0) = 
          o->getGeocenterResidPhase()*RAD2DEG;
      if (r8Data_.contains(adIncohAmp_.getLCode()))
        r8Data_.value(adIncohAmp_.getLCode())->value(bndIdx, 0, i, 0) = o->getIncohChanAddAmp();
      if (r8Data_.contains(adIncohAmp2_.getLCode()))
        r8Data_.value(adIncohAmp2_.getLCode())->value(bndIdx, 0, i, 0) = o->getIncohSegmAddAmp();
      if (r8Data_.contains(adAprioriDl_.getLCode()))
        r8Data_.value(adAprioriDl_.getLCode())->value(bndIdx, 0, i, 0) = o->getAprioriDra(0);
      if (r8Data_.contains(adAprioriRt_.getLCode()))
        r8Data_.value(adAprioriRt_.getLCode())->value(bndIdx, 0, i, 0) = o->getAprioriDra(1);
      if (r8Data_.contains(adAprioriAc_.getLCode()))
        r8Data_.value(adAprioriAc_.getLCode())->value(bndIdx, 0, i, 0) = o->getAprioriDra(2);
      if (r8Data_.contains(adUrVr_.getLCode()))
      {
        r8Data_.value(adUrVr_.getLCode())->value(bndIdx, 0, i, 0) = o->getUrVr(0);
        r8Data_.value(adUrVr_.getLCode())->value(bndIdx, 1, i, 0) = o->getUrVr(1);
      };
      if (r8Data_.contains(adFeedCor_.getLCode()))
      {
        r8Data_.value(adFeedCor_.getLCode())->value(0, bndIdx, i, 0) = o->getCalcFeedCorrDelay();
        r8Data_.value(adFeedCor_.getLCode())->value(1, bndIdx, i, 0) = o->getCalcFeedCorrRate();
      };
      if (r8Data_.contains(adIdelay_.getLCode()))
      {
        r8Data_.value(adIdelay_.getLCode())->value(bndIdx, 0, i, 0) = o->getInstrDelay(0);
        r8Data_.value(adIdelay_.getLCode())->value(bndIdx, 1, i, 0) = o->getInstrDelay(1);
      };
      if (r8Data_.contains(adSrchPar_.getLCode()))
        for (int j=0; j<6; j++)
          r8Data_.value(adSrchPar_.getLCode())->value(bndIdx, j, i, 0) = o->getFourfitSearchParameters(j);
      if (r8Data_.contains(adCorrClock_.getLCode()))
      {
        r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 0, i, 0) = o->getCorrClocks(0, 0);
        r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 1, i, 0) = o->getCorrClocks(0, 1);
        r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 2, i, 0) = o->getCorrClocks(1, 0);
        r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 3, i, 0) = o->getCorrClocks(1, 1);
      };
      if (c1Data_.contains(adCorBasCd_.getLCode()))
        c1Data_.value(adCorBasCd_.getLCode())->value(bndIdx, i, 0) = 
          QString("").sprintf("%c%c", obs->stn_1()->getCid(), obs->stn_2()->getCid());

      if (r8Data_.contains(adZdelay_.getLCode()))
      {
        r8Data_.value(adZdelay_.getLCode())->value(bndIdx, 0, i, 0) = o->getCorrelZdelay_1();
        r8Data_.value(adZdelay_.getLCode())->value(bndIdx, 1, i, 0) = o->getCorrelZdelay_2();
      };
      if (r8Data_.contains(adStarElev_.getLCode()))
      {
        r8Data_.value(adStarElev_.getLCode())->value(bndIdx, 0, i, 0) = o->getCorrelStarElev_1();
        r8Data_.value(adStarElev_.getLCode())->value(bndIdx, 1, i, 0) = o->getCorrelStarElev_2();
      };
      //
      // EoCIS
      // phase cal info stuff:
      if (r8Data_.contains(adPhcRate_.getLCode()))
      {
        r8Data_.value(adPhcRate_.getLCode())->value(bndIdx, 0, i, 0) = o->getPhaseCalRates(0);
        r8Data_.value(adPhcRate_.getLCode())->value(bndIdx, 1, i, 0) = o->getPhaseCalRates(1);
      };

      // channel dependent data:
      if (i2Data_.contains(adBitsSampl_.getLCode()))
        i2Data_.value(adBitsSampl_.getLCode())->value(bndIdx, 0, i, 0) = o->getBitsPerSample();
      if (i2Data_.contains(adNusedChnl_.getLCode()))
        i2Data_.value(adNusedChnl_.getLCode())->value(bndIdx, 0, i, 0) = o->getNumOfChannels();
      //
      QString                    chanIds("");
      QString                    polariz("");
      for (int j=0; j<o->getNumOfChannels(); j++)
      {
        if (i2Data_.contains((adNumOfAp_ + bndIdx)->getLCode()))
        {
          i2Data_.value((adNumOfAp_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->numOfAccPeriodsByChan_USB()->getElement(j);
          i2Data_.value((adNumOfAp_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->numOfAccPeriodsByChan_LSB()->getElement(j);
        };
        if (i2Data_.contains((adIndexNum_ + bndIdx)->getLCode()))
        {
          i2Data_.value((adIndexNum_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->corelIndexNumUSB()->at(j);
          i2Data_.value((adIndexNum_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->corelIndexNumLSB()->at(j);
        };
/*
        chanIds += QString("").sprintf("%-2c", o->chanIdByChan()->at(j));
        polariz += QString("").sprintf("%-2c%-2c", 
          o->polarization_1ByChan()->at(j), o->polarization_2ByChan()->at(j));
*/
        chanIds += QString("").sprintf("%c", o->chanIdByChan()->at(j));
        polariz += QString("").sprintf("%c%c", 
          o->polarization_1ByChan()->at(j), o->polarization_2ByChan()->at(j));

        if (i2Data_.contains((adBbcIdx_ + bndIdx)->getLCode()))
        {
          i2Data_.value((adBbcIdx_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->bbcIdxByChan_1()->at(j);
          i2Data_.value((adBbcIdx_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->bbcIdxByChan_2()->at(j);
        };
        if (i2Data_.contains((adErrRate_ + bndIdx)->getLCode()))
        {
          i2Data_.value((adErrRate_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->phaseCalData_1ByChan()->getElement(4, j);
          i2Data_.value((adErrRate_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->phaseCalData_2ByChan()->getElement(4, j);
        };
        if (r8Data_.contains((adRfFreq_ + bndIdx)->getLCode()))
          r8Data_.value((adRfFreq_ + bndIdx)->getLCode())->value(j, 0, i, 0) = 
            o->refFreqByChan()->getElement(j);
        if (r8Data_.contains((adLoRfFreq_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adLoRfFreq_ + bndIdx)->getLCode())->value(j, 0, i, 0) = 
            o->loFreqByChan_1()->getElement(j);
          r8Data_.value((adLoRfFreq_ + bndIdx)->getLCode())->value(j, 1, i, 0) = 
            o->loFreqByChan_2()->getElement(j);
        };
        if (r8Data_.contains((adNumSmpls_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adNumSmpls_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->numOfSamplesByChan_USB()->getElement(j);
          r8Data_.value((adNumSmpls_ + bndIdx)->getLCode())->value(j, 1, i, 0) = 
            o->numOfSamplesByChan_LSB()->getElement(j);
        };
        if (r8Data_.contains((adChAmpPhs_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adChAmpPhs_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->fringeAmplitudeByChan()->getElement(j);
          r8Data_.value((adChAmpPhs_ + bndIdx)->getLCode())->value(j, 1, i, 0) = 
            o->fringePhaseByChan()->getElement(j)*RAD2DEG;
        };
        // phase cal info data:
        if (r8Data_.contains((adPhcFrq_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adPhcFrq_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_FREQUENCY, j);
          r8Data_.value((adPhcFrq_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_FREQUENCY, j);
        };
        if (r8Data_.contains((adPhcAmp_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adPhcAmp_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_AMPLITUDE, j);
          r8Data_.value((adPhcAmp_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_AMPLITUDE, j);
        };
        if (r8Data_.contains((adPhcPhs_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adPhcPhs_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_PHASE, j)*RAD2DEG*100.0;
          r8Data_.value((adPhcPhs_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_PHASE, j)*RAD2DEG*100.0;
        };
        if (r8Data_.contains((adPhcOff_ + bndIdx)->getLCode()))
        {
          r8Data_.value((adPhcOff_ + bndIdx)->getLCode())->value(j, 0, i, 0) =
            o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_OFFSET, j)*RAD2DEG*100.0;
          r8Data_.value((adPhcOff_ + bndIdx)->getLCode())->value(j, 1, i, 0) =
            o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_OFFSET, j)*RAD2DEG*100.0;
        };
        // end of channels
      };
      if (c1Data_.contains((adChanId_ + bndIdx)->getLCode()) && chanIds.size())
        c1Data_.value((adChanId_ + bndIdx)->getLCode())->value(0, i, 0) = chanIds;
      if (c1Data_.contains((adPolarz_ + bndIdx)->getLCode()) && polariz.size())
        c1Data_.value((adPolarz_ + bndIdx)->getLCode())->value(0, i, 0) = polariz;
      //
    };
    // end of band dependent data
    
    //ints:
    if (i4Data_.contains(adStnIdx_.getLCode()))
    {
      i4Data_.value(adStnIdx_.getLCode())->value(0, 0, i, 0) = stnIdxByKey_[obs->stn_1()->getKey()] + 1;
      i4Data_.value(adStnIdx_.getLCode())->value(1, 0, i, 0) = stnIdxByKey_[obs->stn_2()->getKey()] + 1;
    };
    
    if (r8Data_.contains(adApLength_.getLCode()))
      r8Data_.value(adApLength_.getLCode())->value(0, 0, i, 0) = obs->getApLength();

    if (r8Data_.contains(adDtec_.getLCode()))
      r8Data_.value(adDtec_.getLCode())->value(0, 0, i, 0) = obs->getDTec();
    if (r8Data_.contains(adDtecSig_.getLCode()))
      r8Data_.value(adDtecSig_.getLCode())->value(0, 0, i, 0) = obs->getDTecStdDev();

    if (i2Data_.contains(adIonoBits_.getLCode()))
      i2Data_.value(adIonoBits_.getLCode())->value(0, 0, i, 0) = obs->calculateIonoBits();

    short int                 uAcSup=0;
    SgVlbiObservable         *pbObs=obs->primeObs(), *sbObs=NULL;
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
      it!=obs->observableByKey().end(); ++it)
      if (it.value()->getBandKey() != pbObs->getBandKey())
        sbObs = it.value();
//    if (obs->passiveObses().size())
//      sbObs = obs->passiveObses().at(0);
    
    if (session_->isAttr(SgVlbiSessionInfo::Attr_FF_OUTLIERS_PROCESSED) || 
        session_->isAttr(SgVlbiSessionInfo::Attr_FF_EDIT_INFO_MODIFIED)  )
    {
      if (pbObs->grDelay().getUnweightFlag()==0)
      {
        if (pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
          pbObs->grDelay().setUnweightFlag(1);
        else if (!obs->isAttr(SgObservation::Attr_PROCESSED))
          pbObs->grDelay().setUnweightFlag(2);
        //
        if (sbObs) // can be NULL
        {
          int                   qCodeSb=sbObs->getQualityFactor();
          if (qCodeSb==0 && !obs->isAttr(SgObservation::Attr_PROCESSED))
            pbObs->grDelay().setUnweightFlag(4);
        };
        //
        if (numOfBands_>1 && !sbObs)
          pbObs->grDelay().setUnweightFlag(8);
      }
      else if (!pbObs->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && 
                obs->isAttr(SgObservation::Attr_PROCESSED))
        pbObs->grDelay().setUnweightFlag(0);
    };


    if (pbObs->grDelay().getUnweightFlag()==0 && (sbObs || numOfBands_==1))
      uAcSup = -32768;
    else if (pbObs->grDelay().getUnweightFlag()==1)
      uAcSup = -32767;
    else
      uAcSup = -32763;

    if (i2Data_.contains(adUAcSup_.getLCode()))
      i2Data_.value(adUAcSup_.getLCode())->value(0, 0, i, 0) = uAcSup;
    if (i2Data_.contains(adDelayFlag_.getLCode()))
      i2Data_.value(adDelayFlag_.getLCode())->value(0, 0, i, 0) = pbObs->grDelay().getUnweightFlag();
    if (i2Data_.contains(adRateFlag_.getLCode()))
      i2Data_.value(adRateFlag_.getLCode())->value(0, 0, i, 0) = pbObs->phDRate().getUnweightFlag();
    if (i2Data_.contains(adPhaseFlag_.getLCode()))
      i2Data_.value(adPhaseFlag_.getLCode())->value(0, 0, i, 0) = pbObs->phDelay().getUnweightFlag();


    //4CALC:
    if (r8Data_.contains(adConsnDel_.getLCode()))
      r8Data_.value(adConsnDel_.getLCode())->value(0, 0, i, 0) = obs->getCalcConsensusDelay();
    if (r8Data_.contains(adConsnRat_.getLCode()))
      r8Data_.value(adConsnRat_.getLCode())->value(0, 0, i, 0) = obs->getCalcConsensusRate();
    if (r8Data_.contains(adCtSite1_.getLCode()))
      r8Data_.value(adCtSite1_.getLCode())->value(0, 0, i, 0) = obs->getFractC();
    
    // contributions:
    if (r8Data_.contains(adEtdCont_.getLCode()))
    {
      r8Data_.value(adEtdCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcEarthTideDelay();
      r8Data_.value(adEtdCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcEarthTideRate();
    };
    if (r8Data_.contains(adOceCont_.getLCode()))
    {
      r8Data_.value(adOceCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcOceanTideDelay();
      r8Data_.value(adOceCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcOceanTideRate();
    };
    if (r8Data_.contains(adOceOld_.getLCode()))
    {
      r8Data_.value(adOceOld_.getLCode())->value(0, 0, i, 0) = obs->getCalcOceanTideOldDelay();
      r8Data_.value(adOceOld_.getLCode())->value(1, 0, i, 0) = obs->getCalcOceanTideOldRate();
    };
    if (r8Data_.contains(adOptlCont_.getLCode()))
    {
      r8Data_.value(adOptlCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcOceanPoleTideLdDelay();
      r8Data_.value(adOptlCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcOceanPoleTideLdRate();
    };
    if (r8Data_.contains(adPtdCont_.getLCode()))
    {
      r8Data_.value(adPtdCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcPoleTideDelay();
      r8Data_.value(adPtdCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcPoleTideRate();
    };
    if (r8Data_.contains(adPtoLdCon_.getLCode()))
    {
      r8Data_.value(adPtoLdCon_.getLCode())->value(0, 0, i, 0) = obs->getCalcPoleTideOldDelay();
      r8Data_.value(adPtoLdCon_.getLCode())->value(1, 0, i, 0) = obs->getCalcPoleTideOldRate();
    };
    if (r8Data_.contains(adTiltRmvr_.getLCode()))
    {
      r8Data_.value(adTiltRmvr_.getLCode())->value(0, 0, i, 0) = obs->getCalcTiltRemvrDelay();
      r8Data_.value(adTiltRmvr_.getLCode())->value(1, 0, i, 0) = obs->getCalcTiltRemvrRate();
    };
    if (r8Data_.contains(adUt1Ortho_.getLCode()))
    {
      r8Data_.value(adUt1Ortho_.getLCode())->value(0, 0, i, 0) = obs->getCalcHiFyUt1Delay();
      r8Data_.value(adUt1Ortho_.getLCode())->value(1, 0, i, 0) = obs->getCalcHiFyUt1Rate();
    };
    if (r8Data_.contains(adWobOrtho_.getLCode()))
    {
      r8Data_.value(adWobOrtho_.getLCode())->value(0, 0, i, 0) = obs->getCalcHiFyPxyDelay();
      r8Data_.value(adWobOrtho_.getLCode())->value(1, 0, i, 0) = obs->getCalcHiFyPxyRate();
    };
    if (r8Data_.contains(adUt1Libra_.getLCode()))
    {
      r8Data_.value(adUt1Libra_.getLCode())->value(0, 0, i, 0) = obs->getCalcHiFyUt1LibrationDelay();
      r8Data_.value(adUt1Libra_.getLCode())->value(1, 0, i, 0) = obs->getCalcHiFyUt1LibrationRate();
    };
    if (r8Data_.contains(adWobLibra_.getLCode()))
    {
      r8Data_.value(adWobLibra_.getLCode())->value(0, 0, i, 0) = obs->getCalcHiFyPxyLibrationDelay();
      r8Data_.value(adWobLibra_.getLCode())->value(1, 0, i, 0) = obs->getCalcHiFyPxyLibrationRate();
    };
    if (r8Data_.contains(adWobXcont_.getLCode()))
    {
      r8Data_.value(adWobXcont_.getLCode())->value(0, 0, i, 0) = obs->getCalcPxDelay();
      r8Data_.value(adWobXcont_.getLCode())->value(1, 0, i, 0) = obs->getCalcPxRate();
    };
    if (r8Data_.contains(adWobYcont_.getLCode()))
    {
      r8Data_.value(adWobYcont_.getLCode())->value(0, 0, i, 0) = obs->getCalcPyDelay();
      r8Data_.value(adWobYcont_.getLCode())->value(1, 0, i, 0) = obs->getCalcPyRate();
    };
    if (r8Data_.contains(adConCont_.getLCode()))
    {
      r8Data_.value(adConCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcConsBendingDelay();
      r8Data_.value(adConCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcConsBendingRate();
    };
    if (r8Data_.contains(adSunCont_.getLCode()))
    {
      r8Data_.value(adSunCont_.getLCode())->value(0, 0, i, 0) = obs->getCalcConsBendingSunDelay();
      r8Data_.value(adSunCont_.getLCode())->value(1, 0, i, 0) = obs->getCalcConsBendingSunRate();
    };
    if (r8Data_.contains(adSun2cont_.getLCode()))
    {
      r8Data_.value(adSun2cont_.getLCode())->value(0, 0, i, 0) = obs->getCalcConsBendingSunHigherDelay();
      r8Data_.value(adSun2cont_.getLCode())->value(1, 0, i, 0) = obs->getCalcConsBendingSunHigherRate();
    }
    if (r8Data_.contains(adPlx1pSec_.getLCode()))
    {
      r8Data_.value(adPlx1pSec_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dParallaxRev();
      r8Data_.value(adPlx1pSec_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dParallaxRev();
    };


    if (r8Data_.contains(adBendPart_.getLCode()))
    {
      r8Data_.value(adBendPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dBend();
      r8Data_.value(adBendPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dBend();
    };
    if (r8Data_.contains(adWobPart_.getLCode()))
    {
      r8Data_.value(adWobPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dPx();
      r8Data_.value(adWobPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dPx();
      r8Data_.value(adWobPart_.getLCode())->value(0, 1, i, 0) = obs->getDdel_dPy();
      r8Data_.value(adWobPart_.getLCode())->value(1, 1, i, 0) = obs->getDrat_dPy();
    };
    if (r8Data_.contains(adUt1Part_.getLCode()))
    {
      r8Data_.value(adUt1Part_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dUT1()/86400.0;
      r8Data_.value(adUt1Part_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dUT1()/86400.0;
      r8Data_.value(adUt1Part_.getLCode())->value(0, 1, i, 0) = obs->getD2del_dUT12();
      r8Data_.value(adUt1Part_.getLCode())->value(1, 1, i, 0) = obs->getD2rat_dUT12();
    };
    if (r8Data_.contains(adConsPart_.getLCode()))
    {
      r8Data_.value(adConsPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dGamma();
      r8Data_.value(adConsPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dGamma();
    };
    if (r8Data_.contains(adNut06Xyp_.getLCode()))
    {
      r8Data_.value(adNut06Xyp_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dCipX();
      r8Data_.value(adNut06Xyp_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dCipX();
      r8Data_.value(adNut06Xyp_.getLCode())->value(0, 1, i, 0) = obs->getDdel_dCipY();
      r8Data_.value(adNut06Xyp_.getLCode())->value(1, 1, i, 0) = obs->getDrat_dCipY();
    };
    if (r8Data_.contains(adPlxPart_.getLCode()))
    {
      r8Data_.value(adPlxPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dParallax();
      r8Data_.value(adPlxPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dParallax();
    };
    if (r8Data_.contains(adPtdXyPar_.getLCode()))
    {
      r8Data_.value(adPtdXyPar_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dPolTideX();
      r8Data_.value(adPtdXyPar_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dPolTideX();
      r8Data_.value(adPtdXyPar_.getLCode())->value(0, 1, i, 0) = obs->getDdel_dPolTideY();
      r8Data_.value(adPtdXyPar_.getLCode())->value(1, 1, i, 0) = obs->getDrat_dPolTideY();
    };
    if (r8Data_.contains(adStrPart_.getLCode()))
    {
      r8Data_.value(adStrPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dRA();
      r8Data_.value(adStrPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dRA();
      r8Data_.value(adStrPart_.getLCode())->value(0, 1, i, 0) = obs->getDdel_dDN();
      r8Data_.value(adStrPart_.getLCode())->value(1, 1, i, 0) = obs->getDrat_dDN();
    };
    if (r8Data_.contains(adSitPart_.getLCode()))
    {
      r8Data_.value(adSitPart_.getLCode())->value(0, 0, i, 0) = obs->getDdel_dR_1().at(X_AXIS);
      r8Data_.value(adSitPart_.getLCode())->value(0, 1, i, 0) = obs->getDdel_dR_1().at(Y_AXIS);
      r8Data_.value(adSitPart_.getLCode())->value(0, 2, i, 0) = obs->getDdel_dR_1().at(Z_AXIS);
      r8Data_.value(adSitPart_.getLCode())->value(1, 0, i, 0) = obs->getDrat_dR_1().at(X_AXIS);
      r8Data_.value(adSitPart_.getLCode())->value(1, 1, i, 0) = obs->getDrat_dR_1().at(Y_AXIS);
      r8Data_.value(adSitPart_.getLCode())->value(1, 2, i, 0) = obs->getDrat_dR_1().at(Z_AXIS);
    };
    //
    if (!epochByScanId.contains(obs->getScanId())) // new scan:
    {
      SgAgvDatum<double>       *p;
      const Sg3dVector         *r;
      epochByScanId.insert(obs->getScanId(), *obs);
      //
      if (r8Data_.contains(adSunData_.getLCode()) && (p=r8Data_.value(adSunData_.getLCode())))
      {
        r = &obs->getRsun();
        p->value(0, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(0, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(0, 2, scanIdx, 0) = r->at(Z_AXIS);
        r = &obs->getVsun();
        p->value(1, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(1, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(1, 2, scanIdx, 0) = r->at(Z_AXIS);
      };
      //
      if (r8Data_.contains(adMunData_.getLCode()) && (p=r8Data_.value(adMunData_.getLCode())))
      {
        r = &obs->getRmoon();
        p->value(0, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(0, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(0, 2, scanIdx, 0) = r->at(Z_AXIS);
        r = &obs->getVmoon();
        p->value(1, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(1, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(1, 2, scanIdx, 0) = r->at(Z_AXIS);
      };
      //
      if (r8Data_.contains(adEarthCe_.getLCode()) && (p=r8Data_.value(adEarthCe_.getLCode())))
      {
        r = &obs->getRearth();
        p->value(0, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(0, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(0, 2, scanIdx, 0) = r->at(Z_AXIS);
        r = &obs->getVearth();
        p->value(1, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(1, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(1, 2, scanIdx, 0) = r->at(Z_AXIS);
        r = &obs->getAearth();
        p->value(2, 0, scanIdx, 0) = r->at(X_AXIS);
        p->value(2, 1, scanIdx, 0) = r->at(Y_AXIS);
        p->value(2, 2, scanIdx, 0) = r->at(Z_AXIS);
      };
      //
      if (r8Data_.contains(adNutWahr_.getLCode()) && (p=r8Data_.value(adNutWahr_.getLCode())))
      {
        p->value(0, 0, scanIdx, 0) = obs->getCalcNutWahr_dPsiV();
        p->value(1, 0, scanIdx, 0) = obs->getCalcNutWahr_dEpsV();
        p->value(0, 1, scanIdx, 0) = obs->getCalcNutWahr_dPsiR();
        p->value(1, 1, scanIdx, 0) = obs->getCalcNutWahr_dEpsR();
      };
      if (r8Data_.contains(adNut2006a_.getLCode()) && (p=r8Data_.value(adNut2006a_.getLCode())))
      {
        p->value(0, 0, scanIdx, 0) = obs->getCalcNut2006_dPsiV();
        p->value(1, 0, scanIdx, 0) = obs->getCalcNut2006_dEpsV();
        p->value(0, 1, scanIdx, 0) = obs->getCalcNut2006_dPsiR();
        p->value(1, 1, scanIdx, 0) = obs->getCalcNut2006_dEpsR();
      };
      if (r8Data_.contains(adNut06xys_.getLCode()) && (p=r8Data_.value(adNut06xys_.getLCode())))
      {
        p->value(0, 0, scanIdx, 0) = obs->getCalcCipXv();
        p->value(1, 0, scanIdx, 0) = obs->getCalcCipYv();
        p->value(2, 0, scanIdx, 0) = obs->getCalcCipSv();
      
        p->value(0, 1, scanIdx, 0) = obs->getCalcCipXr();
        p->value(1, 1, scanIdx, 0) = obs->getCalcCipYr();
        p->value(2, 1, scanIdx, 0) = obs->getCalcCipSr();
      };

      if (r8Data_.contains(adCf2J2k_0_.getLCode()) && (p=r8Data_.value(adCf2J2k_0_.getLCode())))
        for (int k=0; k<3; k++)
          for (int l=0; l<3; l++)
            p->value(k, l, scanIdx, 0) = obs->getTrf2crfVal().at((DIRECTION)k, (DIRECTION)l);
      if (r8Data_.contains(adCf2J2k_1_.getLCode()) && (p=r8Data_.value(adCf2J2k_1_.getLCode())))
        for (int k=0; k<3; k++)
          for (int l=0; l<3; l++)
            p->value(k, l, scanIdx, 0) = obs->getTrf2crfRat().at((DIRECTION)k, (DIRECTION)l);
      if (r8Data_.contains(adCf2J2k_2_.getLCode()) && (p=r8Data_.value(adCf2J2k_2_.getLCode())))
        for (int k=0; k<3; k++)
          for (int l=0; l<3; l++)
            p->value(k, l, scanIdx, 0) = obs->getTrf2crfAcc().at((DIRECTION)k, (DIRECTION)l);

      if (r8Data_.contains(adUt1_tai_.getLCode()) && (p=r8Data_.value(adUt1_tai_.getLCode())))
        p->value(0, 0, scanIdx, 0) = obs->getCalcUt1_Tai();
      if (r8Data_.contains(adPolarXy_.getLCode()) && (p=r8Data_.value(adPolarXy_.getLCode())))
      {
        p->value(0, 0, scanIdx, 0) = obs->getCalcPmX();
        p->value(1, 0, scanIdx, 0) = obs->getCalcPmY();
      };

      scanIdx++;
    };
// ---------------> eoc    
  };

  epochByScanId.clear();
};



//
void SgAgvDriver::fillSttnVars()
{
  // per stn:
  QString                        str("");
  int                           stnIdx;
  SgAgvDatum<double>           *p;
  //
  //
  stnIdx = 0;
  for (QMap<QString, int>::iterator it=stnIdxByKey_.begin(); it!=stnIdxByKey_.end(); ++it, stnIdx++)
  {
    SgVlbiStationInfo           *stn=session_->stationsByName().value(it.key());
    if (stn)
    {
      int                       obsIdx;
      obsIdx = 0;
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt, obsIdx++)
      {
        SgVlbiAuxObservation   *auxObs=jt.value();
        if (r8Data_.contains(adAirTemp_.getLCode()))
          r8Data_.value(adAirTemp_.getLCode())->value(0, 0, obsIdx, stnIdx) = 
            auxObs->meteoData().getTemperature() + 273.15;
        if (r8Data_.contains(adAirPress_.getLCode()))
          r8Data_.value(adAirPress_.getLCode())->value(0, 0, obsIdx, stnIdx) = 
            auxObs->meteoData().getPressure()*100.0;
        if (r8Data_.contains(adRelHumd_.getLCode()))
          r8Data_.value(adRelHumd_.getLCode())->value(0, 0, obsIdx, stnIdx) = 
            auxObs->meteoData().getRelativeHumidity();

        if (r8Data_.contains(adCableDel_.getLCode()))
          r8Data_.value(adCableDel_.getLCode())->value(0, 0, obsIdx, stnIdx) = 
            auxObs->getCableCalibration();
        
        if (r8Data_.contains(adCblsSet_.getLCode()))
        {
          r8Data_.value(adCblsSet_.getLCode())->value(0, 0, obsIdx, stnIdx) = 
            auxObs->cableCorrections().getElement(0);
          r8Data_.value(adCblsSet_.getLCode())->value(1, 0, obsIdx, stnIdx) = 
            auxObs->cableCorrections().getElement(1);
          r8Data_.value(adCblsSet_.getLCode())->value(2, 0, obsIdx, stnIdx) = 
            auxObs->cableCorrections().getElement(2);
        };
        //
        //4CALC:
        if (r8Data_.contains(adElTheo_.getLCode()) && (p=r8Data_.value(adElTheo_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getElevationAngle();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getElevationAngleRate();
        };
        if (r8Data_.contains(adAzTheo_.getLCode()) && (p=r8Data_.value(adAzTheo_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getAzimuthAngle();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getAzimuthAngleRate();
        };
        if (r8Data_.contains(adAxoCont_.getLCode()) && (p=r8Data_.value(adAxoCont_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getCalcAxisOffset4Delay();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getCalcAxisOffset4Rate();
        };
        if (r8Data_.contains(adNdryCont_.getLCode()) && (p=r8Data_.value(adNdryCont_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getCalcNdryCont4Delay();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getCalcNdryCont4Rate();
        };
        if (r8Data_.contains(adNwetCont_.getLCode()) && (p=r8Data_.value(adNwetCont_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getCalcNwetCont4Delay();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getCalcNwetCont4Rate();
        };
        if (r8Data_.contains(adOceVert_.getLCode()) && (p=r8Data_.value(adOceVert_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getCalcOLoadVert4Delay();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getCalcOLoadVert4Rate();
        };
        if (r8Data_.contains(adOceHorz_.getLCode()) && (p=r8Data_.value(adOceHorz_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getCalcOLoadHorz4Delay();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getCalcOLoadHorz4Rate();
        };
        if (r8Data_.contains(adOceDeld_.getLCode()) && (p=r8Data_.value(adOceDeld_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getOcnLdR().at(X_AXIS);
          p->value(0, 1, obsIdx, stnIdx) = auxObs->getOcnLdR().at(Y_AXIS);
          p->value(0, 2, obsIdx, stnIdx) = auxObs->getOcnLdR().at(Z_AXIS);
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getOcnLdV().at(X_AXIS);
          p->value(1, 1, obsIdx, stnIdx) = auxObs->getOcnLdV().at(Y_AXIS);
          p->value(1, 2, obsIdx, stnIdx) = auxObs->getOcnLdV().at(Z_AXIS);
        };
        if (r8Data_.contains(adParangle_.getLCode()) && (p=r8Data_.value(adParangle_.getLCode())))
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getParallacticAngle();
        if (r8Data_.contains(adAxoPart_.getLCode()) && (p=r8Data_.value(adAxoPart_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getDdel_dAxsOfs();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getDrat_dAxsOfs();
        };
        if (r8Data_.contains(adNgradPar_.getLCode()) && (p=r8Data_.value(adNgradPar_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getDdel_dTzdGrdN();
          p->value(0, 1, obsIdx, stnIdx) = auxObs->getDdel_dTzdGrdE();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getDrat_dTzdGrdN();
          p->value(1, 1, obsIdx, stnIdx) = auxObs->getDrat_dTzdGrdE();
        };
        if (r8Data_.contains(adNdryPart_.getLCode()) && (p=r8Data_.value(adNdryPart_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getDdel_dTzdDry();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getDrat_dTzdDry();
        };
        if (r8Data_.contains(adNwetPart_.getLCode()) && (p=r8Data_.value(adNwetPart_.getLCode())))
        {
          p->value(0, 0, obsIdx, stnIdx) = auxObs->getDdel_dTzdWet();
          p->value(1, 0, obsIdx, stnIdx) = auxObs->getDrat_dTzdWet();
        };
        // eo4c
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::fillSttnVars(): cannot find a station \"" + it.key() + "\" in the map");
  };
};



// ------------------------------  import stuff: ----------------------------------------
//
bool SgAgvDriver::importData(const QString& fileName)
{
  QFile                         f(fileName);
  startEpoch_ = SgMJD::currentMJD();

  if (inputIdentities_) 
    delete inputIdentities_;
  inputIdentities_ = new SgIdentities;
//  if (inputDriverVersion_) 
//    delete inputDriverVersion_;
//  inputDriverVersion_ = new SgVersion;
  
  inputIdentities_->setUserName("");
  inputIdentities_->setUserEmailAddress("");
  inputIdentities_->setUserDefaultInitials("");
  inputIdentities_->setAcFullName("");
  inputIdentities_->setAcAbbrevName("");
  inputIdentities_->setAcAbbName("");
  inputIdentities_->setExecDir("");
  inputIdentities_->setCurrentDir("");
  inputIdentities_->setExecBinaryName("");
  inputIdentities_->setMachineNodeName("");
  inputIdentities_->setMachineMachineName("");
  inputIdentities_->setMachineSysName("");
  inputIdentities_->setMachineRelease("");
  
  if (!f.open(QIODevice::ReadOnly))
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
      "::importData(): error opening input file: \"" + fileName_ + "\"");
    return false;
  };
  QTextStream                   ts(&f);

  SgAgvChunk                   *chunk;
  while (!ts.atEnd())
  {
    append(chunk = new SgAgvChunk(size()));
#ifdef LOCAL_DEBUG
    //std::cout << "SgAgvDriver::importData appended chunk #" << chunk->getIdx() 
    //<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
    //<< "\n";
#endif
    chunk->importData(ts, this);
#ifdef LOCAL_DEBUG
    //std::cout << "SgAgvDriver::importData imported chunk #" << chunk->getIdx() 
    //<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
    //<< "\n";
#endif
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::importData(): the chunk #" + QString("").setNum(chunk->getIdx()) + " has been read");
  };
  
  checkCollectedDescriptors();

#ifdef LOCAL_DEBUG
  //std::cout << " SgAgvDriver::importData init session start"
  //<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
  //<< "\n";
#endif
  initSession();
#ifdef LOCAL_DEBUG
  //std::cout << " SgAgvDriver::importData init session done"
  //<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
  //<< "\n";
#endif
  ts.setDevice(NULL);
  f.close();

  return true;
};



//
void SgAgvDriver::checkCollectedDescriptors()
{
  QMap<QString, SgAgvDatumDescriptor*>  knownKeyByLcode;

  // check for missed lcodes:
  for (int i=0; i<knownKeys_.size(); i++)
  {
    SgAgvDatumDescriptor       *expectedDd=knownKeys_.at(i);
    SgAgvDatumDescriptor       *actualDd=datumByKey_.value(expectedDd->getLCode());
    knownKeyByLcode[expectedDd->getLCode()] = expectedDd;
    if (actualDd)
    {
      if (expectedDd->getDataType() != actualDd->getDataType())
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::checkCollectedDescriptors(): the LCode \"" + expectedDd->getLCode() + 
          "\" have unexpected data type, " + 
          SgAgvDatumDescriptor::dataType2str(actualDd->getDataType()) + 
          ", expected: " + SgAgvDatumDescriptor::dataType2str(expectedDd->getDataType()));
        actualDd->setIsUsable(false);
      };
      
      if (expectedDd->getDataScope() != actualDd->getDataScope() &&
          !expectedDd->getHasMutableScope())
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::checkCollectedDescriptors(): the LCode \"" + expectedDd->getLCode() + 
          "\" have unexpected data scope: " + 
          SgAgvDatumDescriptor::dataScope2str(actualDd->getDataScope()) + 
          " (expected " + SgAgvDatumDescriptor::dataScope2str(expectedDd->getDataScope()) + ")");
        actualDd->setIsUsable(false);
      };
    }
    else if (expectedDd->isExpected(getExpectedStyle()))
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
        "::checkCollectedDescriptors(): cannot find LCode \"" + expectedDd->getLCode() + "\"");
  };
  //
  // check for unknown lcodes:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=datumByKey_.begin();  it!=datumByKey_.end(); 
    ++it)
  {
    SgAgvDatumDescriptor       *actualDd=it.value();
    if (!knownKeyByLcode.contains(actualDd->getLCode()))
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
        "::checkCollectedDescriptors(): got an unknown LCode \"" + actualDd->getLCode() + "\"");
  };
  knownKeyByLcode.clear();
};




//
bool SgAgvDriver::strVal(const QString& content,
  QString& lCode, int& d1, int& d2, int& d3, int& d4, QString& val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(.*)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  d1 = d2 = d3 = d4 = 0;
  lCode = "";
  val = "";
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
            val = re.cap(6);
        };
      };
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::strVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
bool SgAgvDriver::sintVal(const QString& content, 
  QString& lCode, int& d1, int& d2, int& d3, int& d4, short int &val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([-+0-9]+)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  lCode = "";
  d1 = d2 = d3 = d4 = 0;
  val = 0;
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
            val = re.cap(6).toShort(&isOk);
        };
      };
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::sintVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
bool SgAgvDriver::intVal(const QString& content, 
  QString& lCode, int& d1, int& d2, int& d3, int& d4, int &val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([-+0-9]+)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  lCode = "";
  d1 = d2 = d3 = d4 = val = 0;
  val = 0;
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
            val = re.cap(6).toInt(&isOk);
        };
      };
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::intVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
bool SgAgvDriver::lintVal(const QString& content, 
  QString& lCode, int& d1, int& d2, int& d3, int& d4, long int &val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([-+0-9]+)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  lCode = "";
  d1 = d2 = d3 = d4 = val = 0;
  val = 0;
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
            val = re.cap(6).toLong(&isOk);
        };
      };
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::lintVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
bool SgAgvDriver::floatVal(const QString& content, 
  QString& lCode, int& d1, int& d2, int& d3, int& d4, float &val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([-+0-9\\.eEdD]+)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  d1 = d2 = d3 = d4 = 0;
  lCode = "";
  val = 0.0;
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
            val = re.cap(6).toFloat(&isOk);
        };
      };
    };
  }
  else if (content.contains("NaN", Qt::CaseInsensitive)) // it's ok
  {
    val = 0.0;
    isOk = true;
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::floatVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
bool SgAgvDriver::doubleVal(const QString& content, 
  QString& lCode, int& d1, int& d2, int& d3, int& d4, double &val)
{
  QRegExp                        re("(.{8})\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+([-+0-9\\.eEdD]+)",
                                  Qt::CaseInsensitive);
  bool                           isOk=false;
  d1 = d2 = d3 = d4 = 1;
  val = 0.0;
  lCode = "";
  if (re.indexIn(content) != -1)
  {
    lCode = re.cap(1);
    d3 = re.cap(2).toInt(&isOk);
    if (isOk)
    {
      d4 = re.cap(3).toInt(&isOk);
      if (isOk)
      {
        d1 = re.cap(4).toInt(&isOk);
        if (isOk)
        {
          d2 = re.cap(5).toInt(&isOk);
          if (isOk)
          {
            QString             str(re.cap(6));
            str.replace("D", "E", Qt::CaseInsensitive);
            val = str.toDouble(&isOk);
          };
        };
      };
    };
  }
  else if (content.contains("NaN", Qt::CaseInsensitive)) // it's ok
  {
    val = 0.0;
    isOk = true;
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::doubleVal(): cannot parse the record \"" + content +  "\"");
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;
  return isOk;
};



//
void SgAgvDriver::figureOutImplicitDimensions(const QList<SgAgvRecord*>* section)
{
  QString                        lCode(""), valStr("");
  int                           idx1, idx2, idx3, idx4, val;
  if (section)
  {
    if (section->size() >= 5)
    {
      // find the mandatory records:
      if (datumByKey_.contains(adNobsSta_.getLCode()))
        adNobsSta_ = *datumByKey_.value(adNobsSta_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adNobsSta_.getLCode() +
          "\" was not found");
        return;
      };
      if (datumByKey_.contains(adNumbObs_.getLCode()))
        adNumbObs_ = *datumByKey_.value(adNumbObs_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adNumbObs_.getLCode() +
          "\" was not found");
        return;
      };
      if (datumByKey_.contains(adNumbSca_.getLCode()))
        adNumbSca_ = *datumByKey_.value(adNumbSca_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adNumbSca_.getLCode() +
          "\" was not found");
        return;
      };
      if (datumByKey_.contains(adNumbSta_.getLCode()))
        adNumbSta_ = *datumByKey_.value(adNumbSta_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adNumbSta_.getLCode() +
          "\" was not found");
        return;
      };
      if (datumByKey_.contains(adObsTab_.getLCode()))
        adObsTab_ = *datumByKey_.value(adObsTab_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adObsTab_.getLCode() +
          "\" was not found");
        return;
      };
      if (datumByKey_.contains(adSiteNames_.getLCode()))
        adSiteNames_ = *datumByKey_.value(adSiteNames_.getLCode());
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): the mandatory record \"" + adSiteNames_.getLCode() +
          "\" was not found");
        return;
      };

      bool                       haveNobsSta, haveNumbObs, haveNumbSca, haveNumbSta, haveSiteNames;
      bool                       haveAll;
      haveNobsSta = haveNumbObs = haveNumbSca = haveNumbSta = haveSiteNames = haveAll = false;
      QMap<int, int>            stnobsByIdx;
      QMap<int, QString>        stnnameByIdx;
      
      // check for mandatory records:
      for (int i=0; i<section->size() && !haveAll; i++)
      {
        const QString&           str=section->at(i)->content();
        const QString&           lcd=section->at(i)->lCode();
        lCode = "";
        idx1 = idx2 = idx3 = idx4 = val = 0;
        valStr = "";
        
        if (lcd != adSiteNames_.getLCode())
        {
          intVal(str, lCode, idx1, idx2, idx3, idx4, val);
          if (val == 0)
          {
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::figureOutImplicitDimensions(): the value is 0 for the " + QString("").setNum(i) +
              "'s record \"" + str +  "\"");
            return;
          }
        }
        else
          strVal(str, lCode, idx1, idx2, idx3, idx4, valStr);
        //
        //
        // checking in alpabet order, as the records should be:
        if (adNobsSta_.getLCode() == lCode)
        {
          stnobsByIdx[idx1] = val;
          if (idx1 == adNobsSta_.getDim1())
            haveNobsSta = true;
        };
        
        if (adNumbObs_.getLCode() == lCode)
        {
          numOfObs_ = val;
          haveNumbObs = true;
        };
        if (adNumbSca_.getLCode() == lCode)
        {
          numOfScans_ = val;
          haveNumbSca = true;
        };
        if (adNumbSta_.getLCode() == lCode)
        {
          numOfStn_ = val;
          haveNumbSta = true;
        };

        if (adSiteNames_.getLCode() == lCode)
        {
          valStr = valStr.leftJustified(8, ' ');
          stnNames_ << valStr;
          stnnameByIdx[idx2] = valStr;
          if (idx2 == adSiteNames_.getDim2())
            haveSiteNames = true;
        };
        haveAll = haveNobsSta && haveNumbObs && haveNumbSca && haveNumbSta && haveSiteNames;
      };
      if (haveAll)
      {
        if (stnobsByIdx.size() == stnnameByIdx.size())
        {
          maxNumPerStn_ = 0;
          for (QMap<int, int>::iterator it=stnobsByIdx.begin(); it!=stnobsByIdx.end(); ++it)
          {
            numOfObsByStn_[stnnameByIdx[it.key()]] = it.value();
            if (maxNumPerStn_ < it.value())
              maxNumPerStn_ = it.value();
          };
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::figureOutImplicitDimensions(): sizes of NOBS_STA and SITNAMES mismatch");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::figureOutImplicitDimensions(): cannot find all necessary mandatory records");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::figureOutImplicitDimensions(): not enough mandatory records, " + 
        QString("").setNum(section->size()));
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::figureOutImplicitDimensions(): the section is NULL");

};



//
void SgAgvDriver::digestData(const QList<SgAgvRecord*>* section)
{
  QString                        lCode("");
  int                           idx1, idx2, idx3, idx4;
  if (!section)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): the section is NULL");
    return;
  };
  if (section->size() <= 5)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): not enough records to get data: " + QString("").setNum(section->size()));
    return;
  };
  //
  for (int i=0; i<section->size(); i++)
  {
    QString                     sVal("");
    short int                    siVal=0;
    int                          iVal=0;
    long int                    liVal=0;
    float                       fVal=0.0;
    double                       dVal=0.0;
    const QString&               str=section->at(i)->content();
    const QString&               lcd=section->at(i)->lCode();
    lCode = "";
    idx1 = idx2 = idx3 = idx4 = 0;
    SgAgvDatumDescriptor       *dd=NULL;
    if (datumByKey_.contains(lcd))
    {
      dd = datumByKey_.value(lcd);
      switch (dd->getDataType())
      {
        case ADT_CHAR:
          if (c1Data_.contains(dd->getLCode()))
          {
            if (strVal(str, lCode, idx1, idx2, idx3, idx4, sVal))
              c1Data_.value(dd->getLCode())->value(idx2-1, idx3-1, idx4-1) = sVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in C1 map");
        break;
        case ADT_I2:
          if (i2Data_.contains(dd->getLCode()))
          {
            if (sintVal(str, lCode, idx1, idx2, idx3, idx4, siVal))
              i2Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = siVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I2 map");
        break;
        case ADT_I4:
          if (i4Data_.contains(dd->getLCode()))
          {
            if (intVal(str, lCode, idx1, idx2, idx3, idx4, iVal))
              i4Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = iVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I4 map");
        break;
        case ADT_I8:
          if (i8Data_.contains(dd->getLCode()))
          {
            if (lintVal(str, lCode, idx1, idx2, idx3, idx4, liVal))
              i8Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = liVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I8 map");
        break;
        case ADT_R4:
          if (r4Data_.contains(dd->getLCode()))
          {
            if (floatVal(str, lCode, idx1, idx2, idx3, idx4, fVal))
              r4Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = fVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in R4 map");
        break;
        case ADT_R8:
          if (r8Data_.contains(dd->getLCode()))
          {
            if (doubleVal(str, lCode, idx1, idx2, idx3, idx4, dVal))
              r8Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = dVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in R8 map");
        break;
        case ADT_NONE:
        default:
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::digestData(): got an unknown datum descriptor with the lCode \"" + 
            dd->getLCode() + "\"");
        break;
      };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::digestData(): cannot find lCode \"" + lcd + "\" in the map");
      return;
    };
  };
  //
  if (datumByKey_.contains(adBandNames_.getLCode()))
    numOfBands_ = datumByKey_.value(adBandNames_.getLCode())->d2();
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): cannot figure out a number of bands");

#ifdef LOCAL_DEBUG
/*
std::cout << " ++ Colllected " << c1Data_.size() << " elements of String:\n";
for (QMap<QString, SgAgvDatumString*>::iterator it=c1Data_.begin(); it!=c1Data_.end(); ++it)
{
  SgAgvDatumDescriptor       *dd=NULL;
  if (datumByKey_.contains(it.key()))
  {
    dd = datumByKey_.value(it.key());
    for (int i=0; i<dd->d2(); i++)
      for (int j=0; j<dd->d3(); j++)
        for (int k=0; k<dd->d4(); k++)
std::cout << " ++        " << qPrintable(dd->getLCode()) << "[" << i << "][" << j << "][" << k << "]: " <<
" \"" << qPrintable(it.value()->value(i, j, k)) << "\"\n";
  };
    
};
*/
/*
std::cout << " ++ Colllected " << i2Data_.size() << " elements of short:\n";
for (QMap<QString, SgAgvDatum<short int>*>::iterator it=i2Data_.begin(); it!=i2Data_.end(); ++it)
{
  SgAgvDatumDescriptor       *dd=NULL;
  if (datumByKey_.contains(it.key()))
  {
    dd = datumByKey_.value(it.key());
    for (int i=0; i<dd->d1(); i++)
      for (int j=0; j<dd->d2(); j++)
        for (int k=0; k<dd->d3(); k++)
          for (int l=0; l<dd->d4(); l++)
std::cout << " ++        " << qPrintable(dd->getLCode()) << "[" << i << "][" << j << "][" << k << "][" << l << "]:= "
<< it.value()->value(i, j, k, l) << "\n";
  };
};
*/
#endif
};



//
void SgAgvDriver::digestDataNoRegEx(const QList<SgAgvRecord*>* section)
{
  int                           idx1, idx2, idx3, idx4;
  if (!section)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): the section is NULL");
    return;
  };
  if (section->size() <= 5)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): not enough records to get data: " + QString("").setNum(section->size()));
    return;
  };
  //
  for (int i=0; i<section->size(); i++)
  {
    QString                     sVal("");
    short int                    siVal=0;
    int                          iVal=0;
    long int                    liVal=0;
    float                       fVal=0.0;
    double                       dVal=0.0;
    const QString&               str=section->at(i)->content().mid(9);
    const QString&               lcd=section->at(i)->lCode();
    idx1 = idx2 = idx3 = idx4 = 0;
//
// ------------------ check timing here:
//  
    SgAgvDatumDescriptor       *dd=NULL;
    if (datumByKey_.contains(lcd))
    {
      dd = datumByKey_.value(lcd);
      switch (dd->getDataType())
      {
        case ADT_CHAR:
          if (c1Data_.contains(dd->getLCode()))
          {
            if (strValNoRegEx(str, idx1, idx2, idx3, idx4, sVal))
              c1Data_.value(dd->getLCode())->value(idx2-1, idx3-1, idx4-1) = sVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in C1 map");
        break;
        case ADT_I2:
          if (i2Data_.contains(dd->getLCode()))
          {
            if (sintValNoRegEx(str, idx1, idx2, idx3, idx4, siVal))
              i2Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = siVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I2 map");
        break;
        case ADT_I4:
          if (i4Data_.contains(dd->getLCode()))
          {
            if (intValNoRegEx(str, idx1, idx2, idx3, idx4, iVal))
              i4Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = iVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I4 map");
        break;
        case ADT_I8:
          if (i8Data_.contains(dd->getLCode()))
          {
            if (lintValNoRegEx(str, idx1, idx2, idx3, idx4, liVal))
              i8Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = liVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in I8 map");
        break;
        case ADT_R4:
          if (r4Data_.contains(dd->getLCode()))
          {
            if (floatValNoRegEx(str, idx1, idx2, idx3, idx4, fVal))
              r4Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = fVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in R4 map");
        break;
        case ADT_R8:
          if (r8Data_.contains(dd->getLCode()))
          {
            if (doubleValNoRegEx(str, idx1, idx2, idx3, idx4, dVal))
              r8Data_.value(dd->getLCode())->value(idx1-1, idx2-1, idx3-1, idx4-1) = dVal;
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::digestData(): the datum descriptor with the lCode \"" + dd->getLCode() + 
              "\" is not in R8 map");
        break;
        case ADT_NONE:
        default:
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::digestData(): got an unknown datum descriptor with the lCode \"" + 
            dd->getLCode() + "\"");
        break;
      };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::digestData(): cannot find lCode \"" + lcd + "\" in the map");
      return;
    };
  };
  //
  if (datumByKey_.contains(adBandNames_.getLCode()))
    numOfBands_ = datumByKey_.value(adBandNames_.getLCode())->d2();
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::digestData(): cannot figure out a number of bands");
};



//
bool SgAgvDriver::getContentIdxs(const QString& str, int& i1, int& i2, int& i3, int& i4)
{
  int                           i, strLength;
  i = i1 = i2 = i3 = i4 = 0;
  strLength = str.size();
  while (i<strLength && (i1==0 || i2==0 || i3==0 || i4==0))
  {
    if (i1==0 && str.at(i)==' ')
    {
      i1 = i;
      while (i<strLength && str.at(i)==' ')
        i++;
    }
    else if (i2==0 && str.at(i)==' ')
    {
      i2 = i;
      while (i<strLength && str.at(i)==' ')
        i++;
    }
    else if (i3==0 && str.at(i)==' ')
    {
      i3 = i;
      while (i<strLength && str.at(i)==' ')
        i++;
    }
    else if (i4==0 && str.at(i)==' ')
      i4 = i;
    i++;
  };
  return !(  i1==0 || i2==0 || i3==0 || i4==0 || 
            i1==i2 || i1==i3 || i1==i4 || i2==i3 || i2==i4 || i3==i4 ||
            strLength <= i4-1);
};



//
bool SgAgvDriver::strValNoRegEx(const QString& content, int& d1, int& d2, int& d3, int& d4, QString& val)
{
  int                           i1, i2, i3, i4;
  bool                           isOk=false;
  int                           strLength=content.size();
  char                          buff[2048];
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::strVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = "";
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::strVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (!true)
  {
    if ((i1 = sscanf(qPrintable(content), "%d %d %d %d %s", &d3, &d4, &d1, &d2, buff)) == 5)
    {
      isOk = true;
      val = QString(buff);
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::strVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = content.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = content.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = content.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = content.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
            val = content.mid(i4).simplified();
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::strVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }  
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::strVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::strVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::strVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
bool SgAgvDriver::sintValNoRegEx(const QString& content, 
  int& d1, int& d2, int& d3, int& d4, short int &val)
{
  int                           i1, i2, i3, i4;
  bool                           isOk=false;
  int                           strLength=content.size();
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::sintVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = 0;
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::sintVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (true)
  {
    if ((i1 = sscanf(qPrintable(content), "%d %d %d %d %hd", &d3, &d4, &d1, &d2, &val)) == 5)
      isOk = true;
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::sintVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = content.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = content.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = content.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = content.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
          {
            val = content.mid(i4).toShort(&isOk);
            if (!isOk)
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::sintVal(): cannot parse the value; content: \"" + content +  "\"");
          }  
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::sintVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::sintVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::sintVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::sintVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
bool SgAgvDriver::intValNoRegEx(const QString& content,
  int& d1, int& d2, int& d3, int& d4, int &val)
{
  int                           i1, i2, i3, i4;
  bool                           isOk=false;
  int                           strLength=content.size();
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::intVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = 0;
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::intVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::intVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (true)
  {
    if ((i1 = sscanf(qPrintable(content), "%d %d %d %d %d", &d3, &d4, &d1, &d2, &val)) == 5)
      isOk = true;
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::intVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = content.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = content.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = content.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = content.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
          {
            val = content.mid(i4).toInt(&isOk);
            if (!isOk)
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::intVal(): cannot parse the value; content: \"" + content +  "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::intVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::intVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::intVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::intVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
bool SgAgvDriver::lintValNoRegEx(const QString& content,
  int& d1, int& d2, int& d3, int& d4, long int &val)
{
  int                           i1, i2, i3, i4;
  bool                           isOk=false;
  int                           strLength=content.size();
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::lintVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = 0;
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::lintVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };

  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::lintVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (true)
  {
    if ((i1 = sscanf(qPrintable(content), "%d %d %d %d %ld", &d3, &d4, &d1, &d2, &val)) == 5)
      isOk = true;
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::lintVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = content.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = content.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = content.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = content.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
          {
            val = content.mid(i4).toLong(&isOk);
            if (!isOk)
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::lintVal(): cannot parse the value; content: \"" + content +  "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::lintVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::lintVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::lintVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::lintVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
bool SgAgvDriver::floatValNoRegEx(const QString& content,
  int& d1, int& d2, int& d3, int& d4, float &val)
{
  int                           i1, i2, i3, i4;
  bool                           isOk=false;
  int                           strLength=content.size();
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::floatVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = 0.0;
  if (!getContentIdxs(content, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::floatVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (true)
  {
    if ((i1 = sscanf(qPrintable(content), "%d %d %d %d %e", &d3, &d4, &d1, &d2, &val)) == 5)
      isOk = true;
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::floatVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = content.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = content.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = content.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = content.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
          {
            val = content.mid(i4).toFloat(&isOk);
            if (!isOk)
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::floatVal(): cannot parse the value; content: \"" + content +  "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::floatVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::floatVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::floatVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::floatVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
bool SgAgvDriver::doubleValNoRegEx(const QString& content,
  int& d1, int& d2, int& d3, int& d4, double &val)
{
  bool                           isOk=false;
  int                           i1, i2, i3, i4;
  int                           strLength=content.size();
  if (strLength < 9)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::doubleVal(): the string is too short: \"" + content +  "\"");
    return isOk;
  };
  d1 = d2 = d3 = d4 = 0;
  val = 0.0;
  QString                       str(content);
  str.replace("D", "E", Qt::CaseInsensitive);
  
  if (!getContentIdxs(str, i1, i2, i3, i4))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::doubleVal(): cannot parse the string \"" + content +  "\": " +
      QString("").sprintf("idxs=(%d:%d:%d:%d)", i1, i2, i3, i4));
    return isOk;
  };
  if (true)
  {
    if ((i1 = sscanf(qPrintable(str), "%d %d %d %d %le", &d3, &d4, &d1, &d2, &val)) == 5)
      isOk = true;
    else if (content.contains("NaN", Qt::CaseInsensitive)) // it's ok
    {
      val = 0.0;
      isOk = true;
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::doubleVal(): cannot sscan the string \"" + content +  "\", retCode= " +
        QString("").setNum(i1));
  }
  else
  {
    d3 = str.mid(0, i1).toInt(&isOk);
    if (isOk)
    {
      d4 = str.mid(i1, i2-i1).toInt(&isOk);
      if (isOk)
      {
        d1 = str.mid(i2, i3-i2).toInt(&isOk);
        if (isOk)
        {
          d2 = str.mid(i3, i4-i3).toInt(&isOk);
          if (isOk)
          {
            val = str.mid(i4).toDouble(&isOk);
            if (!isOk)
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::doubleVal(): cannot parse the value; content: \"" + content +  "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::doubleVal(): cannot parse the 2nd idx; content: \"" + content +  "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
            "::doubleVal(): cannot parse the 1st idx; content: \"" + content +  "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::doubleVal(): cannot parse the 4th idx; content: \"" + content +  "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::doubleVal(): cannot parse the 3rd idx; content: \"" + content +  "\"");
  };
  if (d1 == 0)
    d1 = 1;
  if (d2 == 0)
    d2 = 1;
  if (d3 == 0)
    d3 = 1;
  if (d4 == 0)
    d4 = 1;

  return isOk;
};



//
void SgAgvDriver::lookupFuzzyDescriptor(const QString& lc, int bndIdx, const QList<QString>& bandNames,
  SgAgvDatumDescriptor*& dd, bool doCheckNumOfChannels, unsigned int expectedStyles)
{
  QString                       str("");
  str.sprintf("%s%d", qPrintable(lc), bndIdx + 1);
  str = str.leftJustified(8, ' ');
  if (datumByKey_.contains(str))
  {
//    *(dd + bndIdx) = datumByKey_.value(str);
//    if (numOfChansByBand_[bandNames.at(bndIdx)] < (*(d + bndIdx))->getDim1())
//      numOfChansByBand_[bandNames.at(bndIdx)] = (*(d + bndIdx))->getDim1();
    dd = datumByKey_.value(str);
    if (doCheckNumOfChannels && numOfChansByBand_[bandNames.at(bndIdx)] < dd->getDim1())
      numOfChansByBand_[bandNames.at(bndIdx)] = dd->getDim1();
  }
  else
  {
//    *(dd + bndIdx) = NULL;
    dd = NULL;
    if (expectedStyles & expectedStyle_)
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
        "::lookupFuzzyDescriptor(): the lCode \"" + lc + "\" for the band #" + 
        QString("").setNum(bndIdx + 1) + " was not found");
  };
};



//
void SgAgvDriver::initSession()
{
  int                           d=0, n=0;
  QString                       str("");
  QList<QString>                sourceNames, bandNames;
  SgVlbiSessionInfo::CorrelatorPostProcSoftware
                                cppSoft=SgVlbiSessionInfo::CPPS_UNKNOWN;
  //
  // collect neccessarry info:
  //
  if (c1Data_.contains(adPimaCnt_.getLCode()) || c1Data_.contains(adPimaVer_.getLCode()))
    cppSoft = SgVlbiSessionInfo::CPPS_PIMA;
  else if (c1Data_.contains(adFourfCmd_.getLCode()) || c1Data_.contains(adFourfCtrl_.getLCode()) ||
           c1Data_.contains(adFourFtVer_.getLCode()))
    cppSoft = SgVlbiSessionInfo::CPPS_HOPS;
  else if (i4Data_.contains(adFrTypFit_.getLCode()))
  {
    d = i4Data_.value(adFrTypFit_.getLCode())->value(0, 0, 0, 0);
    if (0<d && d<100)
      cppSoft = SgVlbiSessionInfo::CPPS_HOPS;
    else if (100 < d && d < 200)
      cppSoft = SgVlbiSessionInfo::CPPS_PIMA;
  };
  //
  session_->setCppsSoft(cppSoft);

  //
  //
  // source names:
  if (datumByKey_.contains(adSrcName_.getLCode()))
  {
    for (int i=0; i<datumByKey_.value(adSrcName_.getLCode())->d2(); i++)
      sourceNames << c1Data_.value(adSrcName_.getLCode())->value(i, 0, 0).leftJustified(8, ' ');
    numOfSrc_ = sourceNames.size();
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::initSession(): a list of sources was not found");
  //
  // scan names:
  scanNames_.clear();
  if (datumByKey_.contains(adScanNames_.getLCode()))
  {
    for (int i=0; i<datumByKey_.value(adScanNames_.getLCode())->d3(); i++)
      scanNames_ << c1Data_.value(adScanNames_.getLCode())->value(0, i, 0);
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::initSession(): a list of scans was not found");
  //
  // band names:
  if (datumByKey_.contains(adBandNames_.getLCode()))
  {
    for (int i=0; i<datumByKey_.value(adBandNames_.getLCode())->d2(); i++)
      bandNames << c1Data_.value(adBandNames_.getLCode())->value(i, 0, 0);
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::initSession(): a list of bands was not found");
  //
  //
  // check the fuzzy descriptors:
  SgAgvDatumDescriptor        **adNumOfAp=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adIndexNum=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adChanId=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPolarz=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adBbcIdx=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adErrRate=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adRfFreq=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adIndChn=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adLoRfFreq=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adNumSmpls=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adChAmpPhs=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adUvChn=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPhcFrq=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPhcAmp=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPhcPhs=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPhcCm=new SgAgvDatumDescriptor*[numOfBands_];
  SgAgvDatumDescriptor        **adPhcOff=new SgAgvDatumDescriptor*[numOfBands_];
  //
  numOfChansByBand_.clear();
  for (int bndIdx=0; bndIdx<numOfBands_; bndIdx++)
  {
    numOfChansByBand_[bandNames.at(bndIdx)] = 0;
    //
    lookupFuzzyDescriptor("NUM_AP",  bndIdx, bandNames, *(adNumOfAp  + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("CI_NUM",  bndIdx, bandNames, *(adIndexNum + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("BBC_IDX", bndIdx, bandNames, *(adBbcIdx   + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("ERRATE_", bndIdx, bandNames, *(adErrRate  + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("CHANID",  bndIdx, bandNames, *(adChanId   + bndIdx), false, ACS_NATIVE);
    lookupFuzzyDescriptor("POLARZ",  bndIdx, bandNames, *(adPolarz   + bndIdx), false, ACS_NATIVE);
    lookupFuzzyDescriptor("RFREQ",   bndIdx, bandNames, *(adRfFreq   + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("IND_CHN", bndIdx, bandNames, *(adIndChn   + bndIdx), true,  ACS_ANY);
    lookupFuzzyDescriptor("LO_FREQ", bndIdx, bandNames, *(adLoRfFreq + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("APBYFRQ", bndIdx, bandNames, *(adChAmpPhs + bndIdx), true,  ACS_NATIVE);
    lookupFuzzyDescriptor("UV_CHN",  bndIdx, bandNames, *(adUvChn    + bndIdx), true,  ACS_NATIVE);
    if (cppSoft == SgVlbiSessionInfo::CPPS_PIMA)
    {
      lookupFuzzyDescriptor("NUM_SAM", bndIdx, bandNames, *(adNumSmpls + bndIdx), true,  ACS_GVH);
      lookupFuzzyDescriptor("PCAL_FR", bndIdx, bandNames, *(adPhcFrq   + bndIdx), true,  ACS_GVH);

      lookupFuzzyDescriptor("PCAL_CM", bndIdx, bandNames, *(adPhcCm    + bndIdx), true,  ACS_GVH);

      *(adPhcAmp + bndIdx) = NULL;
      *(adPhcPhs + bndIdx) = NULL;
      *(adPhcOff + bndIdx) = NULL;
    }
    else if (cppSoft == SgVlbiSessionInfo::CPPS_HOPS)
    {
      lookupFuzzyDescriptor("NSAMPLS", bndIdx, bandNames, *(adNumSmpls + bndIdx), true, ACS_NATIVE);
      lookupFuzzyDescriptor("PHCFRQ_", bndIdx, bandNames, *(adPhcFrq   + bndIdx), true, ACS_NATIVE);

      lookupFuzzyDescriptor("PHCAMP_", bndIdx, bandNames, *(adPhcAmp   + bndIdx), true, ACS_NATIVE);
      lookupFuzzyDescriptor("PHCPHS_", bndIdx, bandNames, *(adPhcPhs   + bndIdx), true, ACS_NATIVE);
      lookupFuzzyDescriptor("PHCOFF_", bndIdx, bandNames, *(adPhcOff   + bndIdx), true, ACS_NATIVE);
      *(adPhcCm + bndIdx) = NULL;
    }
  };

#ifdef LOCAL_DEBUG
  for (QMap<QString, int>::iterator it=numOfChansByBand_.begin(); it!=numOfChansByBand_.end(); ++it)
  std::cout << "the band[" << qPrintable(it.key()) << "] has " << it.value() << " channels"
  << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
  << "\n";
#endif


  //
  // phase 1:
  //
  //
  bool                           hasFourFitFileName;
  
  hasFourFitFileName = datumByKey_.contains(adFourfFile_.getLCode()) && 
                       datumByKey_.value(adFourfFile_.getLCode())->d3();

  //
  SgAgvDatumDescriptor           *dd;
  QString                       station1Name, station2Name, sourceName, baselineName;
  QString                       scanName, scanId, obsKey, bandKey;
  SgMJD                         epoch;
  SgVlbiBand                   *band=NULL;
  SgVlbiObservation            *obs=NULL;
  SgVlbiObservable             *o=NULL;
  SgVlbiStationInfo            *station1Info, *station2Info;
  SgVlbiStationInfo            *bandStation1Info, *bandStation2Info;
  SgVlbiSourceInfo             *sourceInfo, *bandSourceInfo;
  SgVlbiBaselineInfo           *baselineInfo, *bandBaselineInfo;
  QMap<QString, SgVlbiAuxObservation*>
                               *auxObsByScan=NULL;
  SgMeteoData                   meteo1, meteo2;

  // auxiliaries:
  int                            scanIdx, stn_1Idx, stn_2Idx, srcIdx;
  int                           mjdDay;
  double                         seconds;
  bool                          isTmp;
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                         dSec;
  SgMJD                          tStart, tStop;
  //
  int                            numOfAllChannels=0;
  double                        *frqByChanNum=NULL;

  SgAgvDatum<double>           *pGrDelAmbg=NULL, *pSnr=NULL, *pD;

  if (r8Data_.contains(adGrDelAmbg_.getLCode()))
    pGrDelAmbg = r8Data_.value(adGrDelAmbg_.getLCode());
  if (r8Data_.contains(adSnr_.getLCode()))
    pSnr = r8Data_.value(adSnr_.getLCode());


  //
  //                                          General:
  if (c1Data_.contains(adCorrPlace_.getLCode()))
    session_->setCorrelatorName(
      c1Data_.value(adCorrPlace_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adCorrType_.getLCode()))
    session_->setCorrelatorType(
      c1Data_.value(adCorrType_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adExpCode_.getLCode()))
    session_->setSessionCode(
      c1Data_.value(adExpCode_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adExpDesc_.getLCode()))
    session_->setDescription(
      c1Data_.value(adExpDesc_.getLCode())->value(0, 0, 0));
  //
  if (c1Data_.contains(adExpName_.getLCode()) && !c1Data_.contains(adExpCode_.getLCode()))
    session_->setSessionCode(
      c1Data_.value(adExpName_.getLCode())->value(0, 0, 0));
  //
  if (c1Data_.contains(adMk3DbName_.getLCode()))
  {
    str = c1Data_.value(adMk3DbName_.getLCode())->value(0, 0, 0);
    if (str.at(0) == '$')
      str.remove(0, 1);
    session_->setName(str);
    if (session_->getName().size() == 9)
      session_->setNetworkSuffix(session_->getName().mid(8, 1));
    else
      session_->setNetworkSuffix("");
  };
  if (c1Data_.contains(adPiName_.getLCode()))
    session_->setPiAgencyName(
      c1Data_.value(adPiName_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adRecMode_.getLCode()))
    session_->setRecordingMode(
      c1Data_.value(adRecMode_.getLCode())->value(0, 0, 0));
  if (i2Data_.contains(adExpSerNo_.getLCode()))
    session_->setExperimentSerialNumber(
      i2Data_.value(adExpSerNo_.getLCode())->value(0, 0, 0, 0));

  if (datumByKey_.contains(adSkyFrqChn_.getLCode()) && r8Data_.contains(adSkyFrqChn_.getLCode()))
  {
    numOfAllChannels = datumByKey_.value(adSkyFrqChn_.getLCode())->d1();
    if (numOfAllChannels)
    {
      frqByChanNum = new double[numOfAllChannels];
      for (int i=0; i<numOfAllChannels; i++)
        frqByChanNum[i] = r8Data_.value(adSkyFrqChn_.getLCode())->value(i, 0, 0, 0)*1.0e-6;
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::initSession(): numOfAllChannels is zero");
      frqByChanNum = NULL;
    };
  }
  else if (session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::initSession(): cannot find the lCode \"" + adSkyFrqChn_.getLCode() + "\"");
  };


  // first, create bands:
  for (int i=0; i<numOfBands_; i++)
  {
    bandKey = bandNames.at(i);
    band = new SgVlbiBand;
    band->setKey(bandKey);
    session_->bands().append(band);
    session_->bandByKey().insert(bandKey, band);
    band->setTCreation(getDateOfCreation());
    band->setInputFileName(fileName_);
    //
    if ((r8Data_.contains(adIonCorr_.getLCode()) && r8Data_.contains(adIonRms_.getLCode())) ||
        (r8Data_.contains(adDtec_.getLCode()) && r8Data_.contains(adDtecSig_.getLCode()))    )
      band->addAttr(SgVlbiBand::Attr_HAS_IONO_SOLUTION);
  };
  //
  //
  //
  // add session attributes:
  if ((r8Data_.contains(adIonCorr_.getLCode()) && r8Data_.contains(adIonRms_.getLCode())) ||
      (r8Data_.contains(adDtec_.getLCode()) && r8Data_.contains(adDtecSig_.getLCode()))    )
  {
    session_->addAttr(SgVlbiSessionInfo::Attr_HAS_IONO_CORR);
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_ION_C_CALCULATED);
  };
  //
  if (r8Data_.contains(adDtec_.getLCode()) && r8Data_.contains(adDtecSig_.getLCode()))
    session_->addAttr(SgVlbiSessionInfo::Attr_HAS_DTEC);
  //
  if (i2Data_.contains(adDelayFlag_.getLCode()) ||
      i2Data_.contains(adRateFlag_.getLCode())  ||
      i2Data_.contains(adPhaseFlag_.getLCode())  )
  {
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_OUTLIERS_PROCESSED);
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_EDIT_INFO_MODIFIED);
  };
  //
  if (i4Data_.contains(adNumGrAmbg_.getLCode()))
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED);
  // 
  if (r8Data_.contains(adAirPress_.getLCode()) ||  r8Data_.contains(adCableDel_.getLCode()))
  {
    session_->addAttr(SgVlbiSessionInfo::Attr_HAS_AUX_OBS);
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_AUX_OBS_MODIFIED);
  };
  //
  if (r8Data_.contains(adAtmInterv_.getLCode()) && r8Data_.contains(adAtmConstr_.getLCode()))
    session_->addAttr(SgVlbiSessionInfo::Attr_PRE_PROCESSED);
  //
  if (r8Data_.contains(adConsnDel_.getLCode()) && r8Data_.contains(adConsnRat_.getLCode()))
  {
    session_->addAttr(SgVlbiSessionInfo::Attr_HAS_CALC_DATA);
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_CALC_DATA_MODIFIED);
  };
  //

  // and a couple more below...
  //
  //
  if (r8Data_.contains(adUtcMtai_.getLCode()))
    session_->setLeapSeconds(-r8Data_.value(adUtcMtai_.getLCode())->value(0, 0, 0, 0)); // reverse sign

  // make a skeleton of a session:
  for (int obsIdx=0; obsIdx<numOfObs_; obsIdx++)
  {
    scanIdx  = i4Data_.value(adObsTab_.getLCode())->value(0, obsIdx, 0, 0) - 1;
    stn_1Idx = i4Data_.value(adObsTab_.getLCode())->value(1, obsIdx, 0, 0) - 1;
    stn_2Idx = i4Data_.value(adObsTab_.getLCode())->value(2, obsIdx, 0, 0) - 1;
    // these are per scan:
    srcIdx   = i4Data_.value(adSrcIdx_.getLCode())->value(0, 0, scanIdx, 0) - 1;
    mjdDay   = i4Data_.value(adMjdObs_.getLCode())->value(0, 0, scanIdx, 0);
    seconds  = r8Data_.value(adUtcObs_.getLCode())->value(0, 0, scanIdx, 0);

    epoch.setDate(mjdDay);
    epoch.setTime(seconds/DAY2SEC);


    // stations and source names:
    station1Name = stnNames_.at(stn_1Idx);
    station2Name = stnNames_.at(stn_2Idx);
    sourceName   = sourceNames.at(srcIdx);
    scanName     = scanNames_.at(scanIdx);
    baselineName = station1Name + ":" + station2Name;

#ifdef LOCAL_DEBUG
  if (numOfObs_ < 100)
  std::cout << " Got (" << obsIdx << ") [" 
            << qPrintable(station1Name) << "]:[" << qPrintable(station2Name) << "] @["
            << qPrintable(sourceName) << "], scan=[" << qPrintable(scanName) << "] at "
            << qPrintable(epoch.toString()) << "\n";
#endif

    // scanId and scanName:
    scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
    obsKey.sprintf("%s",
      qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));

    //
    // pick up or create an observation:
    if (session_->observationByKey().contains(obsKey))
      obs = session_->observationByKey().value(obsKey);
    else
    {
      obs = new SgVlbiObservation(session_);
      obs->setMJD(epoch);
      obs->setScanName(scanName);
      obs->setScanId(scanId);
      obs->setKey(obsKey);
      obs->setMediaIdx(obsIdx);
      session_->observations().append(obs);
      session_->observationByKey().insert(obsKey, obs);
      if (c1Data_.contains(adFScanName_.getLCode()))
        obs->setScanFullName(c1Data_.value(adFScanName_.getLCode())->value(0, scanIdx, 0));
      if (c1Data_.contains(adCrootFnam_.getLCode()))
        obs->setCorrRootFileName(c1Data_.value(adCrootFnam_.getLCode())->value(0, scanIdx, 0));
    };
    //
    // station #1:
    if (session_->stationsByName().contains(station1Name))
      station1Info = session_->stationsByName().value(station1Name);
    else // new station, add it to the container and register its index:
    {
      station1Info = new SgVlbiStationInfo(session_->stationsByName().size(), station1Name);
      session_->stationsByName().insert(station1Info->getKey(), station1Info);
      session_->stationsByIdx().insert(station1Info->getIdx(), station1Info);
    };
    // station #2:
    if (session_->stationsByName().contains(station2Name))
      station2Info = session_->stationsByName().value(station2Name);
    else // new station, add it to the container and register its index:
    {
      station2Info = new SgVlbiStationInfo(session_->stationsByName().size(), station2Name);
      session_->stationsByName().insert(station2Info->getKey(), station2Info);
      session_->stationsByIdx().insert(station2Info->getIdx(), station2Info);
    };
    // source:
    if (session_->sourcesByName().contains(sourceName))
      sourceInfo = session_->sourcesByName().value(sourceName);
    else // new source, add it to the container and register its index:
    {
      sourceInfo = new SgVlbiSourceInfo(session_->sourcesByName().size(), sourceName);
      session_->sourcesByName().insert(sourceInfo->getKey(), sourceInfo);
      session_->sourcesByIdx().insert(sourceInfo->getIdx(), sourceInfo);
    };
    // baseline:
    if (session_->baselinesByName().contains(baselineName))
      baselineInfo = session_->baselinesByName().value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      baselineInfo = new SgVlbiBaselineInfo(session_->baselinesByName().size(), baselineName);
      session_->baselinesByName().insert(baselineInfo->getKey(), baselineInfo);
      session_->baselinesByIdx().insert(baselineInfo->getIdx(), baselineInfo);
    };
    obs->setStation1Idx(station1Info->getIdx());
    obs->setStation2Idx(station2Info->getIdx());
    obs->setSourceIdx(sourceInfo->getIdx());
    obs->setBaselineIdx(baselineInfo->getIdx());
    //
    //
    // auxObs: just create entries:
    // the first station:
    auxObsByScan = station1Info->auxObservationByScanId();
    if (!auxObsByScan->contains(scanId)) // new scan, insert data:
    {
      SgVlbiAuxObservation     *auxObs=new SgVlbiAuxObservation;
      auxObs->setMJD(*obs);
      auxObsByScan->insert(scanId, auxObs);
    };
    // the second station:
    auxObsByScan = station2Info->auxObservationByScanId();
    if (!auxObsByScan->contains(scanId)) // new scan, insert data:
    {
      SgVlbiAuxObservation     *auxObs=new SgVlbiAuxObservation;
      auxObs->setMJD(*obs);
      auxObsByScan->insert(scanId, auxObs);
    };
    //
    //
    /*
    if (r8Data_.contains(adApLength_.getLCode()))
      obs->setApLength(
        r8Data_.value(adApLength_.getLCode())->value(0, 0, obsIdx, 0));
    */
    dd = datumByKey_.value(adApLength_.getLCode());
    if (dd && r8Data_.contains(dd->getLCode()))
    {
      if (dd->getDataScope() == ADS_BASELINE)
        obs->setApLength(r8Data_.value(dd->getLCode())->value(0, 0, obsIdx, 0));
      else if ( dd->getDataScope() == ADS_SESSION &&
          dd->getDim3() == 1 && 
          dd->getDim4() == 1)
        obs->setApLength(r8Data_.value(dd->getLCode())->value(0, 0, 0, 0));
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
          "::initSession(): got an uknown format of \"" + dd->getLCode() + "\" lCode");
    };
    if (r8Data_.contains(adDtec_.getLCode()))
      obs->setDTec(
        r8Data_.value(adDtec_.getLCode())->value(0, 0, obsIdx, 0));
    else if (r8Data_.contains(adDtecAdj_.getLCode()))
      obs->setDTec(
        r8Data_.value(adDtecAdj_.getLCode())->value(0, 0, obsIdx, 0));
    if (r8Data_.contains(adDtecSig_.getLCode()))
      obs->setDTecStdDev(
        r8Data_.value(adDtecSig_.getLCode())->value(0, 0, obsIdx, 0));

//        
    // suppresion codes from PIMA's version:
    if (i4Data_.contains(adAutoSup_.getLCode()))
      obs->setPimaAutoSup(i4Data_.value(adAutoSup_.getLCode())->value(0, 0, obsIdx, 0));
    if (i4Data_.contains(adUserRec_.getLCode()))
      obs->setPimaUserRec(i4Data_.value(adUserRec_.getLCode())->value(0, 0, obsIdx, 0));
    if (i4Data_.contains(adUserSup_.getLCode()))
      obs->setPimaUserSup(i4Data_.value(adUserSup_.getLCode())->value(0, 0, obsIdx, 0));




//std::cout << "  -- 0\n";
    // run for each of bands:
    for (int bndIdx=0; bndIdx<numOfBands_; bndIdx++)
    {
      bandKey = bandNames.at(bndIdx);
//std::cout << "  -- 0 band " << qPrintable(bandKey) << "\n";
      band = session_->bandByKey().value(bandKey);
      o = NULL;

      if ((pGrDelAmbg && 0.0<pGrDelAmbg->value(bndIdx, 0, obsIdx, 0) &&
           pSnr && 0.0<pSnr->value(bndIdx, 0, obsIdx, 0))                  ||
           //  false
           session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA
         )
      {
        o = new SgVlbiObservable(obs, band);
        o->setMediaIdx(obsIdx);
        obs->addObservable(band->getKey(), o);
        obs->setupActiveObservable(bandKey);
//std::cout << "  -- 1 band \n";

        // 4band:
        // band's station #1:
        if (band->stationsByName().contains(station1Name))
          bandStation1Info = band->stationsByName().value(station1Name);
        else // new station, add it to the container:
        {
          bandStation1Info = new SgVlbiStationInfo(station1Info->getIdx(), station1Name);
          band->stationsByName().insert(bandStation1Info->getKey(), bandStation1Info);
          band->stationsByIdx().insert(bandStation1Info->getIdx(), bandStation1Info);
        };
        // band's station #2:
        if (band->stationsByName().contains(station2Name))
          bandStation2Info = band->stationsByName().value(station2Name);
        else // new station, add it to the container:
        {
          bandStation2Info = new SgVlbiStationInfo(station2Info->getIdx(), station2Name);
          band->stationsByName().insert(bandStation2Info->getKey(), bandStation2Info);
          band->stationsByIdx().insert(bandStation2Info->getIdx(), bandStation2Info);
        };
        // band's source:
        if (band->sourcesByName().contains(sourceName))
          bandSourceInfo = band->sourcesByName().value(sourceName);
        else // new source, add it to the container:
        {
          bandSourceInfo = new SgVlbiSourceInfo(sourceInfo->getIdx(), sourceName);
          band->sourcesByName().insert(bandSourceInfo->getKey(), bandSourceInfo);
          band->sourcesByIdx().insert(bandSourceInfo->getIdx(), bandSourceInfo);
        };
        // band's baselines:
        if (band->baselinesByName().contains(baselineName))
          bandBaselineInfo = band->baselinesByName().value(baselineName);
        else // new baseline, add it to the container and register its index:
        {
          bandBaselineInfo = new SgVlbiBaselineInfo(baselineInfo->getIdx(), baselineName);
          band->baselinesByName().insert(bandBaselineInfo->getKey(), bandBaselineInfo);
          band->baselinesByIdx().insert(bandBaselineInfo->getIdx(), bandBaselineInfo);
        };
        //
        // plus additional info:
        if (hasFourFitFileName)
          o->setFourfitOutputFName(c1Data_.value(adFourfFile_.getLCode())->value(bndIdx, obsIdx, 0));
        else
          o->setFourfitOutputFName("n/a");
        //
        //
        // fringe error code:
        if (c1Data_.contains(adFourfErr_.getLCode()))
        {
          str = c1Data_.value(adFourfErr_.getLCode())->value(bndIdx, obsIdx, 0);
          o->setErrorCode(str=="_"?" ":str);
        }
        else
          o->setErrorCode(" ");

        // quality factor:
        if (c1Data_.contains(adQualCodes_.getLCode()))
        {
          str = c1Data_.value(adQualCodes_.getLCode())->value(bndIdx, obsIdx, 0)
            .replace('_', ' ').simplified();
          d = str.toInt(&isTmp);
          if (!isTmp)
          {
            d = 0;
            if (c1Data_.contains(adFourfErr_.getLCode()))
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::initSession(): canot convert quality factor [" + str + "] to int");
            else // its ok
              o->setErrorCode(str);
          };
          o->setQualityFactor(d);
        };

        if (session_->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA &&
            o->getErrorCode() != " ")
        {
//std::cout << "  ++ PIMA && errorCode!=' ': [" << qPrintable(o->getErrorCode()) << "]\n";
        }
        else
        {
          // collect observables:
          // single band delay:
          if (r8Data_.contains(adSbDelVal_.getLCode()))
            o->sbDelay().setValue(r8Data_.value(adSbDelVal_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          if (r8Data_.contains(adSbDelErr_.getLCode()))
            o->sbDelay().setSigma(r8Data_.value(adSbDelErr_.getLCode())->value(bndIdx, 0, obsIdx, 0));

          // group delays:
          if (r8Data_.contains(adGrDelVal_.getLCode()))
            o->grDelay().setValue(r8Data_.value(adGrDelVal_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          if (r8Data_.contains(adGrDelErr_.getLCode()))
            o->grDelay().setSigma(r8Data_.value(adGrDelErr_.getLCode())->value(bndIdx, 0, obsIdx, 0));

          // delay rates:
          if (r8Data_.contains(adDelRate_.getLCode()))
            o->phDRate().setValue(r8Data_.value(adDelRate_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          if (r8Data_.contains(adPhRatErr_.getLCode()))
            o->phDRate().setSigma(r8Data_.value(adPhRatErr_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          //
          // ambiguity spacing:
          if (pGrDelAmbg)
            o->grDelay().setAmbiguitySpacing(pGrDelAmbg->value(bndIdx, 0, obsIdx, 0));
          // ambiguity resolution:
          // group delays:
          if (i4Data_.contains(adNumGrAmbg_.getLCode()))
            o->grDelay().setNumOfAmbiguities(
              i4Data_.value(adNumGrAmbg_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          // phase delays:
          if (i4Data_.contains(adNumPhAmbg_.getLCode()))
            o->phDelay().setNumOfAmbiguities(
              i4Data_.value(adNumPhAmbg_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          //
          if (pSnr)
            o->setSnr(pSnr->value(bndIdx, 0, obsIdx, 0));

//std::cout << "  -- 2 band \n";

          if (r8Data_.contains(adCorrelatn_.getLCode()))
            o->setCorrCoeff(
              r8Data_.value(adCorrelatn_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          if (r8Data_.contains(adFrnAmpl_.getLCode()))
            o->setCorrCoeff(
              r8Data_.value(adFrnAmpl_.getLCode())->value(bndIdx, 0, obsIdx, 0));

          if (r8Data_.contains(adRefFreq_.getLCode()))  // Hz->MHz
            o->setReferenceFrequency(
              r8Data_.value(adRefFreq_.getLCode())->value(bndIdx, 0, obsIdx, 0)*1.0E-6);

          if (r8Data_.contains(adEffFreq_.getLCode()))
          {
            o->grDelay().setEffFreq(
              r8Data_.value(adEffFreq_.getLCode())->value(0, bndIdx, obsIdx, 0)*1.0e-6);
            o->phDelay().setEffFreq(
              r8Data_.value(adEffFreq_.getLCode())->value(1, bndIdx, obsIdx, 0)*1.0e-6);
            o->phDRate().setEffFreq(
              r8Data_.value(adEffFreq_.getLCode())->value(2, bndIdx, obsIdx, 0)*1.0e-6);
          };
          if (r8Data_.contains(adEffFrew_.getLCode()))
          {
            o->grDelay().setEffFreqEqWgt(
              r8Data_.value(adEffFrew_.getLCode())->value(0, bndIdx, obsIdx, 0)*1.0e-6);
            o->phDelay().setEffFreqEqWgt(
              r8Data_.value(adEffFrew_.getLCode())->value(1, bndIdx, obsIdx, 0)*1.0e-6);
            o->phDRate().setEffFreqEqWgt(
              r8Data_.value(adEffFrew_.getLCode())->value(2, bndIdx, obsIdx, 0)*1.0e-6);
          };
        
          if (r8Data_.contains(adIonCorr_.getLCode()))
          {
            o->grDelay().setIonoValue(
              r8Data_.value(adIonCorr_.getLCode())->value(0, bndIdx, obsIdx, 0));
            o->phDRate().setIonoValue(
              r8Data_.value(adIonCorr_.getLCode())->value(1, bndIdx, obsIdx, 0));
          };
        }; // end of the letter error code == just a placeholder
        //
        //
        
        //
        if (r8Data_.contains(adTotPhase_.getLCode()))
          o->setTotalPhase(
            r8Data_.value(adTotPhase_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (i2Data_.contains(adDelayFlag_.getLCode()))
          o->grDelay().setUnweightFlag(
            i2Data_.value(adDelayFlag_.getLCode())->value(0, 0, obsIdx, 0));

        // config_ can be NULL:
        if (session_->getConfig() && session_->getConfig()->getUseSolveObsSuppresionFlags() && 
          (o->grDelay().getUnweightFlag()==1 || o->grDelay().getUnweightFlag()==2))
          o->propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
        //
        //
        //
        if (i2Data_.contains(adRateFlag_.getLCode()))
          o->phDRate().setUnweightFlag(
            i2Data_.value(adRateFlag_.getLCode())->value(0, 0, obsIdx, 0));
        if (i2Data_.contains(adPhaseFlag_.getLCode()))
          o->phDelay().setUnweightFlag(
            i2Data_.value(adPhaseFlag_.getLCode())->value(0, 0, obsIdx, 0));

        if (r8Data_.contains(adIonRms_.getLCode()))
        {
          o->grDelay().setIonoSigma(
            r8Data_.value(adIonRms_.getLCode())->value(0, bndIdx, obsIdx, 0));
          o->phDRate().setIonoSigma(
            r8Data_.value(adIonRms_.getLCode())->value(1, bndIdx, obsIdx, 0));
        };
        
//        if (cppSoft == SgVlbiSessionInfo::CPPS_HOPS)
        if (expectedStyle_ == ACS_NATIVE)
        {
          if (r8Data_.contains(adUnPhaseCl_.getLCode()) && 
              datumByKey_.value(adUnPhaseCl_.getLCode())->getIsUsable())
          {
            o->setPhaseCalGrDelays(0, r8Data_.value(adUnPhaseCl_.getLCode())->value(bndIdx, 0, obsIdx, 0)); 
            o->setPhaseCalGrDelays(1, r8Data_.value(adUnPhaseCl_.getLCode())->value(bndIdx, 1, obsIdx, 0)); 
          };
        };
        //
        dd = datumByKey_.value(adSamplRate_.getLCode());
        if (r8Data_.contains(dd->getLCode()))
        {
          if (dd->getDataScope() == ADS_BASELINE)
            o->setSampleRate(r8Data_.value(dd->getLCode())->value(bndIdx, 0, obsIdx, 0));
          else if (dd->getDataScope() == ADS_SESSION &&
                   dd->getDim3() == 1 && 
                   dd->getDim4() == 1)
            o->setSampleRate(r8Data_.value(dd->getLCode())->value(0, 0, 0, 0));
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
              "::initSession(): got an uknown format of \"" + dd->getLCode() + "\" lCode");
        };
        if (r8Data_.contains(adEffDura_.getLCode()))
          o->setEffIntegrationTime(r8Data_.value(adEffDura_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        else if (r8Data_.contains(adScanDur_.getLCode()))
          o->setEffIntegrationTime(r8Data_.value(adScanDur_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (c1Data_.contains(adFourfCmd_.getLCode()))
          o->setFourfitCommandOverride(
            c1Data_.value(adFourfCmd_.getLCode())->value(bndIdx, obsIdx, 0));
        else
          o->setFourfitCommandOverride("n/a");
        if (c1Data_.contains(adFourfCtrl_.getLCode()))
          o->setFourfitControlFile(
            c1Data_.value(adFourfCtrl_.getLCode())->value(bndIdx, obsIdx, 0));
        else
          o->setFourfitControlFile("n/a");

        if (i2Data_.contains(adNumLags_.getLCode()))
          o->setNlags(
            i2Data_.value(adNumLags_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adUvf_Asec_.getLCode()))
        {
          o->setUvFrPerAsec(0, r8Data_.value(adUvf_Asec_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setUvFrPerAsec(1, r8Data_.value(adUvf_Asec_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        };
//std::cout << "  -- 3 band \n";
        //
        //
        // per channel data:
        QString                 chanIds(""), polariz("");
        int                     numOfChannels=0;
        double                  re, im;
        //
        dd = datumByKey_.value(adBitsSampl_.getLCode());
        if (dd && i2Data_.contains(dd->getLCode()))
        {
          if (dd->getDataScope() == ADS_BASELINE)
            o->setBitsPerSample(i2Data_.value(dd->getLCode())->value(bndIdx, 0, obsIdx, 0));
          else if ( dd->getDataScope() == ADS_SESSION &&
                    dd->getDim3() == 1 && 
                    dd->getDim4() == 1)
              o->setBitsPerSample(i2Data_.value(dd->getLCode())->value(0, 0, 0, 0));
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
              "::initSession(): got an uknown format of \"" + dd->getLCode() + "\" lCode");
        };
        if (i2Data_.contains(adNusedChnl_.getLCode()))
          o->setNumOfChannels(
            (numOfChannels=i2Data_.value(adNusedChnl_.getLCode())->value(bndIdx, 0, obsIdx, 0)));
        o->allocateChannelsSetupStorages(numOfChannels);

//std::cout << "  -- 3.1 numOfChannels=" << numOfChannels << "\n";

        dd = *(adChanId + bndIdx);
        if (dd && c1Data_.contains(dd->getLCode()))
          chanIds = c1Data_.value(dd->getLCode())->value(0, obsIdx, 0);
        //
        dd = *(adPolarz + bndIdx);
        if (dd && c1Data_.contains(dd->getLCode()))
          polariz = c1Data_.value(dd->getLCode())->value(0, obsIdx, 0);
        // the second version of polarization data:
        dd = datumByKey_.value(adPolariz.getLCode());
        if (dd && c1Data_.contains(dd->getLCode()))
          polariz = c1Data_.value(dd->getLCode())->value(bndIdx, obsIdx, 0);   
        //
//std::cout << "  -- 3.2\n";
//
//      channel info:
        for (int chIdx=0; chIdx<numOfChannels; chIdx++)
        {
          //
//std::cout << "  -- 3.2.0 chIdx=" << chIdx << "\n";
          dd = *(adNumOfAp + bndIdx);
          if (dd && i2Data_.contains(dd->getLCode()))
          {
            o->numOfAccPeriodsByChan_USB()->setElement(chIdx,
              i2Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            o->numOfAccPeriodsByChan_LSB()->setElement(chIdx,
              i2Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
          };
          //
//std::cout << "  -- 3.2.1\n";
          dd = *(adNumSmpls + bndIdx);
          if (dd && r8Data_.contains(dd->getLCode()))
          {
            o->numOfSamplesByChan_USB()->setElement(chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            if (dd->getDim2() == 2) // PIMA could skip zeros
              o->numOfSamplesByChan_LSB()->setElement(chIdx, 
                r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
          };
          //
//std::cout << "  -- 3.2.2\n";
          dd = *(adRfFreq + bndIdx);
          if (dd && r8Data_.contains(dd->getLCode()))
            o->refFreqByChan()->setElement(chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
          //
//std::cout << "  -- 3.2.3\n";
          dd = *(adIndChn + bndIdx);
          if (dd && frqByChanNum && i2Data_.contains(dd->getLCode()))
          {
            d = i2Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0) - 1;
            if (0<=d && d<numOfAllChannels)
              o->refFreqByChan()->setElement(chIdx,  frqByChanNum[d]);
            else
            {
              logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
                ": initSession(): the channel index " + QString("").setNum(d) + " is out of range [0.." +
                QString("").setNum(numOfAllChannels) + "]");
            };
          };
          //
//std::cout << "  -- 3.2.4\n";
          dd = *(adChAmpPhs + bndIdx);
          if (dd && r8Data_.contains(dd->getLCode()))
          {
            o->fringeAmplitudeByChan()->setElement(chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            o->fringePhaseByChan()->setElement(chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0)/RAD2DEG);
          };
          // PIMA:
//std::cout << "  -- 3.2.5\n";
          dd = *(adUvChn + bndIdx);
          if (dd && r4Data_.contains(dd->getLCode()))
          {
            re = r4Data_.value(dd->getLCode())->value(0, chIdx, obsIdx, 0);
            im = r4Data_.value(dd->getLCode())->value(1, chIdx, obsIdx, 0);
            o->fringeAmplitudeByChan()->setElement(chIdx,  sqrt(re*re + im*im));
            o->fringePhaseByChan    ()->setElement(chIdx, atan2(im, re));
          };
          //
          //
//std::cout << "  -- 3.2.6\n";
          dd = *(adPhcFrq + bndIdx);
          if (dd && r8Data_.contains(dd->getLCode()))
          {
            if (dd->getDataScope()==ADS_BASELINE)
            {
              o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_FREQUENCY, chIdx,
                r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
              o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_FREQUENCY, chIdx,
                r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
            }
            else if (dd->getDataScope()==ADS_STATION)
            {
              o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_FREQUENCY, chIdx,
                r8Data_.value(dd->getLCode())->
                  value(0, chIdx, station1Info->auxObservationByScanId()->size()-1, stn_1Idx));
              o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_FREQUENCY, chIdx,
                r8Data_.value(dd->getLCode())->
                  value(0, chIdx, station2Info->auxObservationByScanId()->size()-1, stn_2Idx));
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
                "::initSession(): got unexpected scope of \"" + dd->getLCode() + "\" lCode");
          };
//std::cout << "  -- 3.2.6.1\n";
          //
          //
          dd = adPhcCm?*(adPhcCm + bndIdx):NULL;
//std::cout << "  -- 3.2.6.2\n";
          if (dd && r8Data_.contains(dd->getLCode()))
          {
//std::cout << "  -- 3.2.6.3\n";
            if (dd->getDataScope()==ADS_STATION)
            {
              re = r8Data_.value(dd->getLCode())->
                value(0, chIdx, station1Info->auxObservationByScanId()->size()-1, stn_1Idx);
              im = r8Data_.value(dd->getLCode())->
                value(1, chIdx, station1Info->auxObservationByScanId()->size()-1, stn_1Idx);
              o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_AMPLITUDE, chIdx,
                sqrt(re*re + im*im));
              o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_PHASE, chIdx, atan2(im, re));
              o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_OFFSET, chIdx, 0.0);

              re = r8Data_.value(dd->getLCode())->
                value(0, chIdx, station2Info->auxObservationByScanId()->size()-1, stn_2Idx);
              im = r8Data_.value(dd->getLCode())->
                value(1, chIdx, station2Info->auxObservationByScanId()->size()-1, stn_2Idx);
              o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_AMPLITUDE, chIdx,
                sqrt(re*re + im*im));
              o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_PHASE, chIdx, atan2(im, re));
              o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_OFFSET, chIdx, 0.0);
//std::cout << "  -- 3.2.6.10\n";
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
                "::initSession(): got unexpected scope of \"" + dd->getLCode() + "\" lCode");
          };
          //
          // eop
          //
          //
//std::cout << "  -- 3.2.7\n";
          if (cppSoft==SgVlbiSessionInfo::CPPS_HOPS && (dd=*(adPhcAmp + bndIdx)))
          {
            o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_AMPLITUDE, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_AMPLITUDE, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
          };
          //
//std::cout << "  -- 3.2.8\n";
//          dd = *(adPhcPhs + bndIdx);
//          if (dd && r8Data_.contains(dd->getLCode()))
          if (cppSoft==SgVlbiSessionInfo::CPPS_HOPS && (dd=*(adPhcPhs + bndIdx)))
          {
            o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_PHASE, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0)/(RAD2DEG*100.0));
            o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_PHASE, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0)/(RAD2DEG*100.0));
          };
          //
//std::cout << "  -- 3.2.9\n";
//          dd = *(adPhcOff + bndIdx);
//          if (dd && r8Data_.contains(dd->getLCode()))
          if (cppSoft==SgVlbiSessionInfo::CPPS_HOPS && (dd=*(adPhcOff + bndIdx)))
          {
            o->phaseCalData_1ByChan()->setElement(SgVlbiObservable::PCCI_OFFSET, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0)/(RAD2DEG*100.0));
            o->phaseCalData_2ByChan()->setElement(SgVlbiObservable::PCCI_OFFSET, chIdx,
              r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0)/(RAD2DEG*100.0));
          };
          //
//std::cout << "  -- 3.2.10\n";
          dd = *(adIndexNum + bndIdx);
          if (dd && i2Data_.contains(dd->getLCode()))
          {
            (*o->corelIndexNumUSB())[chIdx] =
              i2Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0);
            (*o->corelIndexNumLSB())[chIdx] =
              i2Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0);
          };
          //
//std::cout << "  -- 3.2.11\n";
          dd = *(adBbcIdx + bndIdx);
          if (dd && i2Data_.contains(dd->getLCode()))
          {
            (*o->bbcIdxByChan_1())[chIdx] =
              i2Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0);
            (*o->bbcIdxByChan_2())[chIdx] =
              i2Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0);
          };
          //
//std::cout << "  -- 3.2.12\n";
          dd = *(adErrRate + bndIdx);
          if (dd && i2Data_.contains(dd->getLCode()))
          {
            o->phaseCalData_1ByChan()->setElement(4, chIdx, 
              i2Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            o->phaseCalData_2ByChan()->setElement(4, chIdx, 
              i2Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
          };
          //
//std::cout << "  -- 3.2.13\n";
          dd = *(adLoRfFreq + bndIdx);
          if (dd && r8Data_.contains(dd->getLCode()))
          {
            o->loFreqByChan_1()->setElement(chIdx, 
              r8Data_.value(dd->getLCode())->value(chIdx, 0, obsIdx, 0));
            o->loFreqByChan_2()->setElement(chIdx, 
              r8Data_.value(dd->getLCode())->value(chIdx, 1, obsIdx, 0));
          };
          //
//std::cout << "  -- 3.2.15\n";
          if (chanIds.size() == numOfChannels)
            (*o->chanIdByChan())[chIdx] = chanIds.at(chIdx).toLatin1();
          //
//std::cout << "  -- 3.2.16\n";
          if (polariz.size() == 2*numOfChannels)
          {
            (*o->polarization_1ByChan())[chIdx] = polariz.at(2*chIdx    ).toLatin1();
            (*o->polarization_2ByChan())[chIdx] = polariz.at(2*chIdx + 1).toLatin1();
          };
//std::cout << "  -- 3.2.--\n";
        }; // end of channel iteration
        //
      if (polariz.size() == 2)
        for (int ii=0; ii<numOfChannels; ii++)
        {
          (*o->polarization_1ByChan())[ii] = polariz.at(0).toLatin1();
          (*o->polarization_2ByChan())[ii] = polariz.at(1).toLatin1();
        };




//std::cout << "  -- 4 band \n";
        //
        SgVector&               numOfAccPeriodsByChan_USB = *o->numOfAccPeriodsByChan_USB();
        SgVector&               numOfAccPeriodsByChan_LSB = *o->numOfAccPeriodsByChan_LSB();
        SgVector&               numOfSamplesByChan_USB =    *o->numOfSamplesByChan_USB();
        SgVector&               numOfSamplesByChan_LSB =    *o->numOfSamplesByChan_LSB();
        SgVector&               refFreqByChan =             *o->refFreqByChan();
        SgVector&               fringeAmplitudeByChan =     *o->fringeAmplitudeByChan();
        double                  effFreq4GR=0.0, effFreq4PH=0.0, effFreq4RT=0.0;
        //
//        if (cppSoft == SgVlbiSessionInfo::CPPS_HOPS)

        if (*(adPhcFrq + bndIdx) /*&& !r8Data_.contains(adUnPhaseCl_.getLCode())*/)
          o->calcPhaseCalDelay();

        if (expectedStyle_ == ACS_NATIVE)
        {
          if (*(adPhcFrq + bndIdx) && !r8Data_.contains(adUnPhaseCl_.getLCode()))
            o->calcPhaseCalDelay();
          if (!r8Data_.contains(adEffFreq_.getLCode()))
          {
            if (numOfChannels)
              evaluateEffectiveFreqs(numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, refFreqByChan,
                fringeAmplitudeByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB,
                *o->channelBandwidth(), o->getReferenceFrequency(), numOfChannels,
                effFreq4GR, effFreq4PH, effFreq4RT, band->getCorrelatorType(), o);
            else
              effFreq4GR = effFreq4PH = effFreq4RT = o->getReferenceFrequency();
            o->grDelay().setEffFreq(effFreq4GR);
            o->phDelay().setEffFreq(effFreq4PH);
            o->phDRate().setEffFreq(effFreq4RT);
            //
            if (numOfChannels)
              evaluateEffectiveFreqs(numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, refFreqByChan,
                fringeAmplitudeByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB,
                *o->channelBandwidth(), o->getReferenceFrequency(), numOfChannels,
                effFreq4GR, effFreq4PH, effFreq4RT, band->getCorrelatorType(), o, true);
            o->grDelay().setEffFreqEqWgt(effFreq4GR);
            o->phDelay().setEffFreqEqWgt(effFreq4PH);
            o->phDRate().setEffFreqEqWgt(effFreq4RT);
          };
        }
        else if (cppSoft==SgVlbiSessionInfo::CPPS_PIMA)
        {
//          ...
        }
        else
        {

        };

        
        //
        if (i2Data_.contains(adStrtOffst_.getLCode()))
          o->setStartOffset(i2Data_.value(adStrtOffst_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (i2Data_.contains(adStopOffst_.getLCode()))
          o->setStopOffset(i2Data_.value(adStopOffst_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (c1Data_.contains(adTapQcode_.getLCode()))
          o->setTapeQualityCode(c1Data_.value(adTapQcode_.getLCode())->value(bndIdx, obsIdx, 0));

        if (i2Data_.contains(adHopsVer_.getLCode()))
          o->setHopsRevisionNumber(i2Data_.value(adHopsVer_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (i2Data_.contains(adFourFtVer_.getLCode()))
        {
          o->setFourfitVersion(0,
            i2Data_.value(adFourFtVer_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setFourfitVersion(1,
            i2Data_.value(adFourFtVer_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        }
        else
        {
          o->setFourfitVersion(0, 0);
          o->setFourfitVersion(1, 0);
        };

        if (i2Data_.contains(adScanUtc_.getLCode()))
        {
          nYear = i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 0, obsIdx, 0);
          nMonth= i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 1, obsIdx, 0);
          nDay   = i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 2, obsIdx, 0);
          nHour = i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 3, obsIdx, 0);
          nMin   = i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 4, obsIdx, 0);
          dSec   = i2Data_.value(adScanUtc_.getLCode())->value(bndIdx, 5, obsIdx, 0);
          o->setEpochOfScan(SgMJD(nYear, nMonth, nDay, nHour, nMin, dSec));
        };
        if (i2Data_.contains(adUtcMtag_.getLCode()))
        {
          nYear = i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 0, obsIdx, 0);
          nMonth= i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 1, obsIdx, 0);
          nDay   = i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 2, obsIdx, 0);
          nHour = i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 3, obsIdx, 0);
          nMin   = i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 4, obsIdx, 0);
          dSec   = i2Data_.value(adUtcMtag_.getLCode())->value(bndIdx, 5, obsIdx, 0);
          o->setEpochCentral(SgMJD(nYear, nMonth, nDay, nHour, nMin, dSec));
        };
        if (i2Data_.contains(adCorrUtc_.getLCode()))
        {
          nYear = i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 0, obsIdx, 0);
          nMonth= i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 1, obsIdx, 0);
          nDay   = i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 2, obsIdx, 0);
          nHour = i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 3, obsIdx, 0);
          nMin   = i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 4, obsIdx, 0);
          dSec   = i2Data_.value(adCorrUtc_.getLCode())->value(bndIdx, 5, obsIdx, 0);
          o->setEpochOfCorrelation(SgMJD(nYear, nMonth, nDay, nHour, nMin, dSec));
        };
        if (i2Data_.contains(adFourUtc_.getLCode()))
        {
          nYear = i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 0, obsIdx, 0);
          nMonth= i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 1, obsIdx, 0);
          nDay   = i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 2, obsIdx, 0);
          nHour = i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 3, obsIdx, 0);
          nMin   = i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 4, obsIdx, 0);
          dSec   = i2Data_.value(adFourUtc_.getLCode())->value(bndIdx, 5, obsIdx, 0);
          o->setEpochOfFourfitting(SgMJD(nYear, nMonth, nDay, nHour, nMin, dSec));
        };
        //
//std::cout << "  -- 5 band \n";
        // start & stop seconds:
        if (r8Data_.contains(adStartSec_.getLCode()) && r8Data_.contains(adStopSec_.getLCode()))
        {
          SgMJD::MJD_reverse(o->owner()->getDate(), o->owner()->getTime(),
            nYear, nMonth, nDay, nHour, nMin, dSec);

          tStart = SgMJD(nYear, nMonth, nDay, nHour, 0, 
            r8Data_.value(adStartSec_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          tStop  = SgMJD(nYear, nMonth, nDay, nHour, 0,
            r8Data_.value(adStopSec_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          if (tStart - *o->owner() > 40.0/24.0/60.0)
            tStart -= 1.0/24.0;
          if (*o->owner() - tStart > 40.0/24.0/60.0)
            tStart += 1.0/24.0;
          if (*o->owner() - tStop > 40.0/24.0/60.0)
            tStop += 1.0/24.0;
          if (tStop - *o->owner() > 40.0/24.0/60.0)
            tStop -= 1.0/24.0;
          if (tStart < tStop)
          {
            o->setTstart(tStart);
            o->setTstop (tStop);
          }
          else
          {
            o->setTstart(tStop);
            o->setTstop (tStart);
          };
        };
        //
        if (r8Data_.contains(adQbFactor_.getLCode()))
          o->setAcceptedRatio(
            r8Data_.value(adQbFactor_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adDiscard_.getLCode()))
          o->setDiscardRatio(
            r8Data_.value(adDiscard_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adDeltaEpo_.getLCode()))
          o->setCentrOffset(
            r8Data_.value(adDeltaEpo_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (r8Data_.contains(adGcPhsRes_.getLCode()))
          o->setGeocenterResidPhase(
            r8Data_.value(adGcPhsRes_.getLCode())->value(bndIdx, 0, obsIdx, 0)/RAD2DEG);
        if (r8Data_.contains(adGcPhase_.getLCode()))
          o->setGeocenterTotalPhase(
            r8Data_.value(adGcPhase_.getLCode())->value(bndIdx, 0, obsIdx, 0)/RAD2DEG);

        if (r8Data_.contains(adGcSbDel_.getLCode()))
          o->sbDelay().setGeocenterValue(
            r8Data_.value(adGcSbDel_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adGcMbDel_.getLCode()))
          o->grDelay().setGeocenterValue(
            r8Data_.value(adGcMbDel_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adGcRate_.getLCode()))
          o->phDRate().setGeocenterValue(
            r8Data_.value(adGcRate_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adSbResid_.getLCode()))
          o->sbDelay().setResidualFringeFitting(
            r8Data_.value(adSbResid_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adGrResid_.getLCode()))
          o->grDelay().setResidualFringeFitting(
            r8Data_.value(adGrResid_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adRtResid_.getLCode()))
          o->phDRate().setResidualFringeFitting(
            r8Data_.value(adRtResid_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        
        if (r8Data_.contains(adFalseDet_.getLCode()))
          o->setProbabOfFalseDetection(
            r8Data_.value(adFalseDet_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (r8Data_.contains(adAprioriDl_.getLCode()) && (pD=r8Data_.value(adAprioriDl_.getLCode())))
          o->setAprioriDra(0, pD->value(bndIdx, 0, obsIdx, 0));
        else if (r8Data_.contains(adAprDel_.getLCode()) && (pD=r8Data_.value(adAprDel_.getLCode())))
          o->setAprioriDra(0, pD->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adAprioriRt_.getLCode()) && (pD=r8Data_.value(adAprioriRt_.getLCode())))
          o->setAprioriDra(1, pD->value(bndIdx, 0, obsIdx, 0));
        else if (r8Data_.contains(adAprRate_.getLCode()) && (pD=r8Data_.value(adAprRate_.getLCode())))
          o->setAprioriDra(1, pD->value(bndIdx, 0, obsIdx, 0));


        if (r8Data_.contains(adAprioriAc_.getLCode()))
          o->setAprioriDra(2, 
            r8Data_.value(adAprioriAc_.getLCode())->value(bndIdx, 0, obsIdx, 0));





        if (r8Data_.contains(adIncohAmp_.getLCode()))
          o->setIncohChanAddAmp(
            r8Data_.value(adIncohAmp_.getLCode())->value(bndIdx, 0, obsIdx, 0));
        if (r8Data_.contains(adIncohAmp2_.getLCode()))
          o->setIncohSegmAddAmp(
            r8Data_.value(adIncohAmp2_.getLCode())->value(bndIdx, 0, obsIdx, 0));

        if (r8Data_.contains(adUrVr_.getLCode()))
        {
          o->setUrVr(0,
            r8Data_.value(adUrVr_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setUrVr(1,
            r8Data_.value(adUrVr_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        };
        if (r8Data_.contains(adFeedCor_.getLCode()))
        {
          o->setCalcFeedCorrDelay(r8Data_.value(adFeedCor_.getLCode())->value(0, bndIdx, obsIdx, 0));
          o->setCalcFeedCorrRate (r8Data_.value(adFeedCor_.getLCode())->value(1, bndIdx, obsIdx, 0));
        };
        if (r8Data_.contains(adIdelay_.getLCode()))
        {
          o->setInstrDelay(0,
            r8Data_.value(adIdelay_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setInstrDelay(1,
            r8Data_.value(adIdelay_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        };
        for (int j=0; j<6; j++)
          if (r8Data_.contains(adSrchPar_.getLCode()))
            o->setFourfitSearchParameters(j,
              r8Data_.value(adSrchPar_.getLCode())->value(bndIdx, j, obsIdx, 0));
          else
            o->setFourfitSearchParameters(j, 0);
//std::cout << "  -- 6 band \n";

        if (r8Data_.contains(adCorrClock_.getLCode()))
        {
          o->setCorrClocks(0, 0,
            r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setCorrClocks(0, 1,
            r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 1, obsIdx, 0));
          o->setCorrClocks(1, 0,
            r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 2, obsIdx, 0));
          o->setCorrClocks(1, 1,
            r8Data_.value(adCorrClock_.getLCode())->value(bndIdx, 3, obsIdx, 0));
        };

        if (r8Data_.contains(adAprCloOf_.getLCode()) && (pD=r8Data_.value(adAprCloOf_.getLCode())))
        {
          SgAgvDatum<double>    *pD2=r8Data_.value(adAprCloRt_.getLCode());
          if (pD2)
          {
            o->setCorrClocks(0, 0,
              pD->value(0, 0, station1Info->auxObservationByScanId()->size()-1, stn_1Idx));
            o->setCorrClocks(0, 1,
              pD->value(0, 0, station2Info->auxObservationByScanId()->size()-1, stn_2Idx));
            o->setCorrClocks(1, 0,
              pD2->value(0, 0, station1Info->auxObservationByScanId()->size()-1, stn_1Idx));
            o->setCorrClocks(1, 1,
              pD2->value(0, 0, station2Info->auxObservationByScanId()->size()-1, stn_2Idx));            
          };
        };
        
        if (c1Data_.contains(adCorBasCd_.getLCode()) && 
            c1Data_.value(adCorBasCd_.getLCode())->value(bndIdx, obsIdx, 0).size() == 2)
        {
          char                  cid;
          cid = c1Data_.value(adCorBasCd_.getLCode())->value(bndIdx, obsIdx, 0).at(0).toLatin1();
        
          if (station1Info->getCid() == ' ')
          {
            station1Info->setCid(cid);
            logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
              "::initSession(): station#1 " + station1Info->getKey() + " set CID to \"" + 
              cid + "\" at " + o->strId());
          }
          else if (station1Info->getCid() != cid)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::initSession(): station#1 " + station1Info->getKey() + " CID missmatch: \"" + 
              station2Info->getCid() + "\" vs \"" + cid + "\" at " + o->strId());
          };

          cid = c1Data_.value(adCorBasCd_.getLCode())->value(bndIdx, obsIdx, 0).at(1).toLatin1();
          if (station2Info->getCid() == ' ')
          {
            station2Info->setCid(cid);
            logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
              "::initSession(): station#2 " + station2Info->getKey() + " set CID to \"" + 
              cid + "\" at " + o->strId());
          }
          else if (station2Info->getCid() != cid)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::initSession(): station#2 " + station2Info->getKey() + " CID missmatch: \"" + 
              station2Info->getCid() + "\" vs \"" + cid + "\" at " + o->strId());
          };
        };
        if (r8Data_.contains(adZdelay_.getLCode()))
        {
          o->setCorrelZdelay_1(r8Data_.value(adZdelay_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setCorrelZdelay_2(r8Data_.value(adZdelay_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        };
        if (r8Data_.contains(adStarElev_.getLCode()))
        {
          o->setCorrelStarElev_1(r8Data_.value(adStarElev_.getLCode())->value(bndIdx, 0, obsIdx, 0));
          o->setCorrelStarElev_2(r8Data_.value(adStarElev_.getLCode())->value(bndIdx, 1, obsIdx, 0));
        };
        //
        // per band statistics:
        band->incNumTotal(DT_DELAY);
        band->incNumTotal(DT_RATE);
        bandStation1Info->incNumTotal(DT_DELAY);
        bandStation1Info->incNumTotal(DT_RATE);
        bandStation2Info->incNumTotal(DT_DELAY);
        bandStation2Info->incNumTotal(DT_RATE);
        bandSourceInfo->incNumTotal(DT_DELAY);
        bandSourceInfo->incNumTotal(DT_RATE);
        bandBaselineInfo->incNumTotal(DT_DELAY);
        bandBaselineInfo->incNumTotal(DT_RATE);
      }
      else
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::initSession(): skipped observable #" + QString("").setNum(obsIdx) + 
          " at the " + bandKey + "-band: no data");
      //
      //
    };// end of band iteration
    //
    //
//std::cout << "  -- 7 \n";
    //
    //
    //
     //4CALC:
    if (r8Data_.contains(adConsnDel_.getLCode()))
      obs->setCalcConsensusDelay(
        r8Data_.value(adConsnDel_.getLCode())->value(0, 0, obsIdx, 0));
    if (r8Data_.contains(adConsnRat_.getLCode()))
      obs->setCalcConsensusRate(
        r8Data_.value(adConsnRat_.getLCode())->value(0, 0, obsIdx, 0));
    if (r8Data_.contains(adCtSite1_.getLCode()))
      obs->setFractC(
        r8Data_.value(adCtSite1_.getLCode())->value(0, 0, obsIdx, 0));

    // contributions:
    /**/
    if (r8Data_.contains(adEtdCont_.getLCode()))
    {
      obs->setCalcEarthTideDelay(r8Data_.value(adEtdCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcEarthTideRate (r8Data_.value(adEtdCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adOceCont_.getLCode()))
    {
      obs->setCalcOceanTideDelay(r8Data_.value(adOceCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcOceanTideRate (r8Data_.value(adOceCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adOceOld_.getLCode()))
    {
      obs->setCalcOceanTideOldDelay(r8Data_.value(adOceOld_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcOceanTideOldRate (r8Data_.value(adOceOld_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adOptlCont_.getLCode()))
    {
      obs->setCalcOceanPoleTideLdDelay(r8Data_.value(adOptlCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcOceanPoleTideLdRate (r8Data_.value(adOptlCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adPtdCont_.getLCode()))
    {
      obs->setCalcPoleTideDelay(r8Data_.value(adPtdCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcPoleTideRate (r8Data_.value(adPtdCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adPtoLdCon_.getLCode()))
    {
      obs->setCalcPoleTideOldDelay(r8Data_.value(adPtoLdCon_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcPoleTideOldRate (r8Data_.value(adPtoLdCon_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adTiltRmvr_.getLCode()))
    {
      obs->setCalcTiltRemvrDelay(r8Data_.value(adTiltRmvr_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcTiltRemvrRate (r8Data_.value(adTiltRmvr_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adUt1Ortho_.getLCode()))
    {
      obs->setCalcHiFyUt1Delay(r8Data_.value(adUt1Ortho_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcHiFyUt1Rate (r8Data_.value(adUt1Ortho_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adWobOrtho_.getLCode()))
    {
      obs->setCalcHiFyPxyDelay(r8Data_.value(adWobOrtho_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcHiFyPxyRate (r8Data_.value(adWobOrtho_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adUt1Libra_.getLCode()))
    {
      obs->setCalcHiFyUt1LibrationDelay(r8Data_.value(adUt1Libra_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcHiFyUt1LibrationRate (r8Data_.value(adUt1Libra_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adWobLibra_.getLCode()))
    {
      obs->setCalcHiFyPxyLibrationDelay(r8Data_.value(adWobLibra_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcHiFyPxyLibrationRate (r8Data_.value(adWobLibra_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adWobXcont_.getLCode()))
    {
      obs->setCalcPxDelay(r8Data_.value(adWobXcont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcPxRate (r8Data_.value(adWobXcont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adWobYcont_.getLCode()))
    {
      obs->setCalcPyDelay(r8Data_.value(adWobYcont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcPyRate (r8Data_.value(adWobYcont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adConCont_.getLCode()))
    {
      obs->setCalcConsBendingDelay(r8Data_.value(adConCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcConsBendingRate (r8Data_.value(adConCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adSunCont_.getLCode()))
    {
      obs->setCalcConsBendingSunDelay(r8Data_.value(adSunCont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcConsBendingSunRate (r8Data_.value(adSunCont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adSun2cont_.getLCode()))
    {
      obs->setCalcConsBendingSunHigherDelay(r8Data_.value(adSun2cont_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setCalcConsBendingSunHigherRate (r8Data_.value(adSun2cont_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adPlx1pSec_.getLCode()))
    {
      obs->setDdel_dParallaxRev(r8Data_.value(adPlx1pSec.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dParallaxRev(r8Data_.value(adPlx1pSec.getLCode())->value(1, 0, obsIdx, 0));
    };
   
    if (r8Data_.contains(adBendPart_.getLCode()))
    {
      obs->setDdel_dBend(r8Data_.value(adBendPart_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dBend(r8Data_.value(adBendPart_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adWobPart_.getLCode()))
    {
      obs->setDdel_dPx(r8Data_.value(adWobPart_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dPx(r8Data_.value(adWobPart_.getLCode())->value(1, 0, obsIdx, 0));
      obs->setDdel_dPy(r8Data_.value(adWobPart_.getLCode())->value(0, 1, obsIdx, 0));
      obs->setDrat_dPy(r8Data_.value(adWobPart_.getLCode())->value(1, 1, obsIdx, 0));
    };
    if (r8Data_.contains(adUt1Part_.getLCode()))
    {
      obs->setDdel_dUT1(r8Data_.value(adUt1Part_.getLCode())->value(0, 0, obsIdx, 0)*86400.0);
      obs->setDrat_dUT1(r8Data_.value(adUt1Part_.getLCode())->value(1, 0, obsIdx, 0)*86400.0);
      obs->setD2del_dUT12(r8Data_.value(adUt1Part_.getLCode())->value(0, 1, obsIdx, 0));
      obs->setD2rat_dUT12(r8Data_.value(adUt1Part_.getLCode())->value(1, 1, obsIdx, 0));
    };
    if (r8Data_.contains(adConsPart_.getLCode()))
    {
      obs->setDdel_dGamma(r8Data_.value(adConsPart_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dGamma(r8Data_.value(adConsPart_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adNut06Xyp_.getLCode()))
    {
      obs->setDdel_dCipX(r8Data_.value(adNut06Xyp_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dCipX(r8Data_.value(adNut06Xyp_.getLCode())->value(1, 0, obsIdx, 0));
      obs->setDdel_dCipY(r8Data_.value(adNut06Xyp_.getLCode())->value(0, 1, obsIdx, 0));
      obs->setDrat_dCipY(r8Data_.value(adNut06Xyp_.getLCode())->value(1, 1, obsIdx, 0));
    };
    if (r8Data_.contains(adPlxPart_.getLCode()))
    {
      obs->setDdel_dParallax(r8Data_.value(adPlxPart_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dParallax(r8Data_.value(adPlxPart_.getLCode())->value(1, 0, obsIdx, 0));
    };
    if (r8Data_.contains(adPtdXyPar_.getLCode()))
    {
      obs->setDdel_dPolTideX(r8Data_.value(adPtdXyPar_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dPolTideX(r8Data_.value(adPtdXyPar_.getLCode())->value(1, 0, obsIdx, 0));
      obs->setDdel_dPolTideY(r8Data_.value(adPtdXyPar_.getLCode())->value(0, 1, obsIdx, 0));
      obs->setDrat_dPolTideY(r8Data_.value(adPtdXyPar_.getLCode())->value(1, 1, obsIdx, 0));
    };
    if (r8Data_.contains(adStrPart_.getLCode()))
    {
      obs->setDdel_dRA(r8Data_.value(adStrPart_.getLCode())->value(0, 0, obsIdx, 0));
      obs->setDrat_dRA(r8Data_.value(adStrPart_.getLCode())->value(1, 0, obsIdx, 0));
      obs->setDdel_dDN(r8Data_.value(adStrPart_.getLCode())->value(0, 1, obsIdx, 0));
      obs->setDrat_dDN(r8Data_.value(adStrPart_.getLCode())->value(1, 1, obsIdx, 0));
    };
    if (r8Data_.contains(adSitPart_.getLCode()))
    {
      Sg3dVector                r(r8Data_.value(adSitPart_.getLCode())->value(0, 0, obsIdx, 0), 
                                  r8Data_.value(adSitPart_.getLCode())->value(0, 1, obsIdx, 0), 
                                  r8Data_.value(adSitPart_.getLCode())->value(0, 2, obsIdx, 0));
      obs->setDdel_dR_1( r);
      obs->setDdel_dR_2(-r);
      
      r = Sg3dVector(  r8Data_.value(adSitPart_.getLCode())->value(1, 0, obsIdx, 0), 
                      r8Data_.value(adSitPart_.getLCode())->value(1, 1, obsIdx, 0), 
                      r8Data_.value(adSitPart_.getLCode())->value(1, 2, obsIdx, 0));
      obs->setDrat_dR_1( r);
      obs->setDrat_dR_2(-r);
    };
  
    // per scan data:
    //
    if (r8Data_.contains(adSunData_.getLCode()) && (pD=r8Data_.value(adSunData_.getLCode())))
    {
      obs->setRsun(Sg3dVector(pD->value(0, 0, scanIdx, 0), 
                              pD->value(0, 1, scanIdx, 0), 
                              pD->value(0, 2, scanIdx, 0)));
      obs->setVsun(Sg3dVector(pD->value(1, 0, scanIdx, 0), 
                              pD->value(1, 1, scanIdx, 0), 
                              pD->value(1, 2, scanIdx, 0)));
    };
    //
    if (r8Data_.contains(adMunData_.getLCode()) && (pD=r8Data_.value(adMunData_.getLCode())))
    {
      obs->setRmoon(Sg3dVector(pD->value(0, 0, scanIdx, 0), 
                               pD->value(0, 1, scanIdx, 0), 
                               pD->value(0, 2, scanIdx, 0)));
      obs->setVmoon(Sg3dVector(pD->value(1, 0, scanIdx, 0), 
                               pD->value(1, 1, scanIdx, 0), 
                               pD->value(1, 2, scanIdx, 0)));
    };
    //
    if (r8Data_.contains(adEarthCe_.getLCode()) && (pD=r8Data_.value(adEarthCe_.getLCode())))
    {
      obs->setRearth(Sg3dVector(pD->value(0, 0, scanIdx, 0), 
                                pD->value(0, 1, scanIdx, 0), 
                                pD->value(0, 2, scanIdx, 0)));
      obs->setVearth(Sg3dVector(pD->value(1, 0, scanIdx, 0), 
                                pD->value(1, 1, scanIdx, 0), 
                                pD->value(1, 2, scanIdx, 0)));
      obs->setAearth(Sg3dVector(pD->value(2, 0, scanIdx, 0), 
                                pD->value(2, 1, scanIdx, 0), 
                                pD->value(2, 2, scanIdx, 0)));
    };
    //
    if (r8Data_.contains(adNutWahr_.getLCode()) && (pD=r8Data_.value(adNutWahr_.getLCode())))
    {
      obs->setCalcNutWahr_dPsiV(pD->value(0, 0, scanIdx, 0));
      obs->setCalcNutWahr_dEpsV(pD->value(1, 0, scanIdx, 0));
      obs->setCalcNutWahr_dPsiR(pD->value(0, 1, scanIdx, 0));
      obs->setCalcNutWahr_dEpsR(pD->value(1, 1, scanIdx, 0));
    };
    if (r8Data_.contains(adNut2006a_.getLCode()) && (pD=r8Data_.value(adNut2006a_.getLCode())))
    {
      obs->setCalcNut2006_dPsiV(pD->value(0, 0, scanIdx, 0));
      obs->setCalcNut2006_dEpsV(pD->value(1, 0, scanIdx, 0));
      obs->setCalcNut2006_dPsiR(pD->value(0, 1, scanIdx, 0));
      obs->setCalcNut2006_dEpsR(pD->value(1, 1, scanIdx, 0));
    };
    if (r8Data_.contains(adNut06xys_.getLCode()) && (pD=r8Data_.value(adNut06xys_.getLCode())))
    {
      obs->setCalcCipXv(pD->value(0, 0, scanIdx, 0));
      obs->setCalcCipYv(pD->value(1, 0, scanIdx, 0));
      obs->setCalcCipSv(pD->value(2, 0, scanIdx, 0));
      
      obs->setCalcCipXr(pD->value(0, 1, scanIdx, 0));
      obs->setCalcCipYr(pD->value(1, 1, scanIdx, 0));
      obs->setCalcCipSr(pD->value(2, 1, scanIdx, 0));
    };
    if (r8Data_.contains(adCf2J2k_0_.getLCode()) && (pD=r8Data_.value(adCf2J2k_0_.getLCode())))
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfVal()((DIRECTION)k, (DIRECTION)l) = pD->value(k, l, scanIdx, 0);
    if (r8Data_.contains(adCf2J2k_1_.getLCode()) && (pD=r8Data_.value(adCf2J2k_1_.getLCode())))
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfRat()((DIRECTION)k, (DIRECTION)l) = pD->value(k, l, scanIdx, 0);
    if (r8Data_.contains(adCf2J2k_2_.getLCode()) && (pD=r8Data_.value(adCf2J2k_2_.getLCode())))
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfAcc()((DIRECTION)k, (DIRECTION)l) = pD->value(k, l, scanIdx, 0);

    if (r8Data_.contains(adUt1_tai_.getLCode()) && (pD=r8Data_.value(adUt1_tai_.getLCode())))
      obs->setCalcUt1_Tai(pD->value(0, 0, scanIdx, 0));
    if (r8Data_.contains(adPolarXy_.getLCode()) && (pD=r8Data_.value(adPolarXy_.getLCode())))
    {
      obs->setCalcPmX(pD->value(0, 0, scanIdx, 0));
      obs->setCalcPmY(pD->value(1, 0, scanIdx, 0));
    };
    //-----> end of calc    
    
    // cumulative statistics (do we need it?):
    station1Info->incNumTotal(DT_DELAY);
    station2Info->incNumTotal(DT_DELAY);
    sourceInfo  ->incNumTotal(DT_DELAY);
    baselineInfo->incNumTotal(DT_DELAY);

    station1Info->incNumTotal(DT_RATE);
    station2Info->incNumTotal(DT_RATE);
    sourceInfo  ->incNumTotal(DT_RATE);
    baselineInfo->incNumTotal(DT_RATE);
  };// end of obs iteration
  //
  if (frqByChanNum)
  {
    delete[] frqByChanNum;
    frqByChanNum = NULL;
  };
  //

  //
  for (int i=0; i<numOfBands_; i++)
  {
    band = session_->bands().at(i);
    bandKey = band->getKey();
    n = 0;
    o = session_->observations().at(n)->observable(bandKey);
    while (!o && n<session_->observations().size())
      o = session_->observations().at(n++)->observable(bandKey);
    if (o)
      band->setFrequency(o->getReferenceFrequency());
    //
    // histories:
    if (band->history().size())
    {
      for (int j=0; j<band->history().size(); j++)
        delete band->history().at(j);
      band->history().clear();
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        ": initSession(): history of the band " + bandKey + " has been cleared");
    };
    // pick up history records:
    n = 0;
    for (int cIdx=0; cIdx<size(); cIdx++)
    {
      for (int j=0; j<at(cIdx)->history().size(); j++)
      {
/*
        SgVlbiHistoryRecord     *rec=at(cIdx)->history().at(j);
        if (rec->getVersion() < 0)
          n = cIdx;
        else
          n = rec->getVersion();
        band->history().append(new SgVlbiHistoryRecord(rec->getEpoch(), n, rec->getText()));
*/
        band->history().append(new SgVlbiHistoryRecord(*at(cIdx)->history().at(j)));
      };
    };
    n = 1;
    if (!band->history().isEmpty())
      n = band->history().last()->getVersion();
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::initSession(): no history records exist for the band " + bandKey);
    // put fingerprint: (should it be here?)
//    band->history().append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), n,
//      libraryVersion.name() + ": data have been read from vgosDa file"));
    band->setInputFileVersion(n);
  };
  //
  //

  //4CALC:
  if (!session_->calcInfo().ut1InterpData())
  {
    session_->calcInfo().ut1InterpData() = new SgMatrix(1, 2);
  };
  if (!session_->calcInfo().wobInterpData())
  {
    session_->calcInfo().wobInterpData() = new SgMatrix(1, 2);
  };

  //4CALC:
  if (c1Data_.contains(adCalcFlgN_.getLCode()) && i2Data_.contains(adCalcFlgV_.getLCode()))
  {
    n = datumByKey_.value(adCalcFlgN_.getLCode())->d2();
    QList<int>*                  flagVals =&session_->calcInfo().controlFlagValues();
    QList<QString>*              flagNames=&session_->calcInfo().controlFlagNames();
    flagVals->clear();
    flagNames->clear();
    for (int i=0; i<n; i++)
    {
      flagVals->append(i2Data_.value(adCalcFlgV_.getLCode())->value(i, 0, 0, 0));
      flagNames->append(c1Data_.value(adCalcFlgN_.getLCode())->value(i, 0, 0));
    };
  };
  
  if (i2Data_.contains(adTidalUt1_.getLCode()))
    session_->calcInfo().setFlagTidalUt1(i2Data_.value(adTidalUt1_.getLCode())->value(0, 0, 0, 0));
  if (r8Data_.contains(adCalcVer_.getLCode()))
    session_->calcInfo().setDversion(r8Data_.value(adCalcVer_.getLCode())->value(0, 0, 0, 0));
  if (r8Data_.contains(adRelData_.getLCode()))
    session_->calcInfo().setRelativityData(r8Data_.value(adRelData_.getLCode())->value(0, 0, 0, 0));
  if (r8Data_.contains(adPreData_.getLCode()))
    session_->calcInfo().setPrecessionData(r8Data_.value(adPreData_.getLCode())->value(0, 0, 0, 0));
  if (r8Data_.contains(adEtdData_.getLCode()))
  {
    session_->calcInfo().setEarthTideData(0, r8Data_.value(adEtdData_.getLCode())->value(0, 0, 0, 0));
    session_->calcInfo().setEarthTideData(1, r8Data_.value(adEtdData_.getLCode())->value(1, 0, 0, 0));
    session_->calcInfo().setEarthTideData(2, r8Data_.value(adEtdData_.getLCode())->value(2, 0, 0, 0));
  };
  //
  getCalcInfoModelMessNCtrlF(adAtmMess_.getLCode(), adAtmCflg_.getLCode(), 
    session_->calcInfo().troposphere());
  getCalcInfoModelMessNCtrlF(adAxoMess_.getLCode(), adAxoCflg_.getLCode(), 
    session_->calcInfo().axisOffset());
  getCalcInfoModelMessNCtrlF(adEtdMess_.getLCode(), adEtdCflg_.getLCode(), 
    session_->calcInfo().earthTide());
  getCalcInfoModelMessNCtrlF(adPtdMess_.getLCode(), adPtdCflg_.getLCode(), 
    session_->calcInfo().poleTide());
  getCalcInfoModelMessNCtrlF(adNutMess_.getLCode(), adNutCflg_.getLCode(), 
    session_->calcInfo().nutation());
  getCalcInfoModelMessNCtrlF(adOceMess_.getLCode(), adOceCflg_.getLCode(), 
    session_->calcInfo().oceanLoading());
  getCalcInfoModelMessNCtrlF(adAtiMess_.getLCode(), adAtiCflg_.getLCode(), 
    session_->calcInfo().atomicTime());
  getCalcInfoModelMessNCtrlF(adCtiMess_.getLCode(), adCtiCflg_.getLCode(), 
    session_->calcInfo().coordinateTime());
  getCalcInfoModelMessNCtrlF(adPlxMess_.getLCode(), adPlxCflg_.getLCode(), 
    session_->calcInfo().parallax());
  getCalcInfoModelMessNCtrlF(adStrMess_.getLCode(), adStrCflg_.getLCode(), 
    session_->calcInfo().star());
  getCalcInfoModelMessNCtrlF(adTheMess_.getLCode(), adRelCflg_.getLCode(), 
    session_->calcInfo().relativity());

  if (c1Data_.contains(adSitMess_.getLCode()))
    session_->calcInfo().site().setDefinition(c1Data_.value(adSitMess_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adPanMess_.getLCode()))
    session_->calcInfo().feedHorn().setDefinition(c1Data_.value(adPanMess_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adPepMess_.getLCode()))
    session_->calcInfo().ephemeris().setDefinition(c1Data_.value(adPepMess_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adWobCflg_.getLCode()))
    session_->calcInfo().polarMotion().setControlFlag(
      c1Data_.value(adWobCflg_.getLCode())->value(0, 0, 0));
  if (c1Data_.contains(adUt1Cflg_.getLCode()))
    session_->calcInfo().ut1().setControlFlag(c1Data_.value(adUt1Cflg_.getLCode())->value(0, 0, 0));

  if (c1Data_.contains(adOceStat_.getLCode()))
  {
    session_->calcInfo().oLoadStationStatus().clear();
    n = datumByKey_.value(adOceStat_.getLCode())->d2();
    for (int i=0; i<n; i++)
      session_->calcInfo().oLoadStationStatus().
        append(c1Data_.value(adOceStat_.getLCode())->value(i, 0, 0));
  };

  if (r8Data_.contains(adSiteZens_.getLCode()))
  {
    session_->calcInfo().siteZenDelays().clear();
    n = datumByKey_.value(adSiteZens_.getLCode())->d1();
    for (int i=0; i<n; i++)
      session_->calcInfo().siteZenDelays().
        append(r8Data_.value(adSiteZens_.getLCode())->value(i, 0, 0, 0));
  };

  if (r8Data_.contains(adUt1Epoch_.getLCode()))
  {
    n = datumByKey_.value(adUt1Epoch_.getLCode())->d2();
    session_->calcInfo().prepareUt1InterpData(n);
    for (unsigned int i=0; i<session_->calcInfo().ut1InterpData()->nRow(); i++)
      for (unsigned int j=0; j<session_->calcInfo().ut1InterpData()->nCol(); j++)
        session_->calcInfo().ut1InterpData()->setElement(i, j,  
          r8Data_.value(adUt1Epoch_.getLCode())->value(j, i, 0, 0));
  };
  //
  if (r8Data_.contains(adWobEpoch_.getLCode()))
  {
    n = datumByKey_.value(adWobEpoch_.getLCode())->d2();
    session_->calcInfo().prepareWobInterpData(n);
    for (unsigned int i=0; i<session_->calcInfo().wobInterpData()->nRow(); i++)
      for (unsigned int j=0; j<session_->calcInfo().wobInterpData()->nCol(); j++)
        session_->calcInfo().wobInterpData()->setElement(i, j,
          r8Data_.value(adWobEpoch_.getLCode())->value(j, i, 0, 0));
  };

  //
  //
  // end of phase 1 / skeleton
  // figure out the index of "CABL_DEL":
  int                           cableCalFlagIdx;
  bool                          isCableCalMatched=false;
  cableCalFlagIdx = -1;
  if (c1Data_.contains(adStnCalNam_.getLCode()))
  {
    n = datumByKey_.value(adStnCalNam_.getLCode())->d2();
    for (int i=0; i<n; i++)
      if (c1Data_.value(adStnCalNam_.getLCode())->value(i, 0, 0) == "CABL_DEL")
        {
          isCableCalMatched = true;
          cableCalFlagIdx = i;
        };
  };


  //
  //
  // phase 2, stations:
  Sg3dVector                    v3;
  for (int stnIdx=0; stnIdx<numOfStn_; stnIdx++) // use station order from the input file
  {
    const QString               stnName(stnNames_.at(stnIdx));
    SgVlbiStationInfo          *stn=session_->stationsByName().value(stnName);
    if (stn)
    {
    //int                       nObs=numOfObsByStn_[stnName];
      int                       obsIdx=0;
      bool                       hasMeteoT, hasMeteoP, hasMeteoR, hasMeteo, hasCable; 
      hasMeteoT = r8Data_.contains(adAirTemp_.getLCode());
      hasMeteoP = r8Data_.contains(adAirPress_.getLCode());
      hasMeteoR = r8Data_.contains(adRelHumd_.getLCode());
      hasMeteo = hasMeteoT || hasMeteoP || hasMeteoR;
      hasCable = r8Data_.contains(adCableDel_.getLCode());
      //
      if (hasMeteo)
      {
        stn->addAttr(SgVlbiStationInfo::Attr_HAS_METEO);
        stn->addAttr(SgVlbiStationInfo::Attr_METEO_MODIFIED);
        //
        stn->setMeteoDataOrigin(SgVlbiStationInfo::MDO_FS_LOG);
        stn->setCableCalsOriginTxt("Data are from somewhere");
      };
      if (hasCable)
      {
        stn->addAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL);
        stn->addAttr(SgVlbiStationInfo::Attr_CABLE_CAL_MODIFIED);
        //:
        stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_FS_LOG);
        stn->setCableCalsOriginTxt("Data are from somewhere");
      };
      //
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt, obsIdx++)
      {
        SgVlbiAuxObservation   *auxObs=jt.value();
        if (hasMeteoT)
          auxObs->meteoData().setTemperature(
            r8Data_.value(adAirTemp_.getLCode())->value(0, 0, obsIdx, stnIdx) - 273.15);
        if (hasMeteoP)
          auxObs->meteoData().setPressure(
            r8Data_.value(adAirPress_.getLCode())->value(0, 0, obsIdx, stnIdx)/100.0);
        if (hasMeteoR)
          auxObs->meteoData().setRelativeHumidity(
            r8Data_.value(adRelHumd_.getLCode())->value(0, 0, obsIdx, stnIdx));

        if (hasCable)
          auxObs->setCableCalibration(
            r8Data_.value(adCableDel_.getLCode())->value(0, 0, obsIdx, stnIdx));

        //4CALC:
        if (r8Data_.contains(adElTheo_.getLCode()) && (pD=r8Data_.value(adElTheo_.getLCode())))
        {
          auxObs->setElevationAngle     (pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setElevationAngleRate (pD->value(1, 0, obsIdx, stnIdx));
        }
        else if (r8Data_.contains(adElev_.getLCode()) && (pD=r8Data_.value(adElev_.getLCode())))
          auxObs->setElevationAngle     (pD->value(0, 0, obsIdx, stnIdx));

        if (r8Data_.contains(adAzTheo_.getLCode()) && (pD=r8Data_.value(adAzTheo_.getLCode())))
        {
          auxObs->setAzimuthAngle       (pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setAzimuthAngleRate   (pD->value(1, 0, obsIdx, stnIdx));
        }
        else if (r8Data_.contains(adAzimuth_.getLCode()) && (pD=r8Data_.value(adAzimuth_.getLCode())))
          auxObs->setAzimuthAngle       (pD->value(0, 0, obsIdx, stnIdx));

        if (r8Data_.contains(adAxoCont_.getLCode()) && (pD=r8Data_.value(adAxoCont_.getLCode())))
        {
          auxObs->setCalcAxisOffset4Delay(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setCalcAxisOffset4Rate (pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adNdryCont_.getLCode()) && (pD=r8Data_.value(adNdryCont_.getLCode())))
        {
          auxObs->setCalcNdryCont4Delay(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setCalcNdryCont4Rate (pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adNwetCont_.getLCode()) && (pD=r8Data_.value(adNwetCont_.getLCode())))
        {
          auxObs->setCalcNwetCont4Delay(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setCalcNwetCont4Rate (pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adOceVert_.getLCode()) && (pD=r8Data_.value(adOceVert_.getLCode())))
        {
          auxObs->setCalcOLoadVert4Delay(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setCalcOLoadVert4Rate (pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adOceHorz_.getLCode()) && (pD=r8Data_.value(adOceHorz_.getLCode())))
        {
          auxObs->setCalcOLoadHorz4Delay(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setCalcOLoadHorz4Rate (pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adOceDeld_.getLCode()) && (pD=r8Data_.value(adOceDeld_.getLCode())))
        {
          auxObs->setOcnLdR(Sg3dVector(  pD->value(0, 0, obsIdx, stnIdx),
                                        pD->value(0, 1, obsIdx, stnIdx),
                                        pD->value(0, 2, obsIdx, stnIdx)));
          auxObs->setOcnLdV(Sg3dVector(  pD->value(1, 0, obsIdx, stnIdx),
                                        pD->value(1, 1, obsIdx, stnIdx),
                                        pD->value(1, 2, obsIdx, stnIdx)));
        };
        if (r8Data_.contains(adParangle_.getLCode()) && (pD=r8Data_.value(adParangle_.getLCode())))
          auxObs->setParallacticAngle(pD->value(0, 0, obsIdx, stnIdx));
        if (r8Data_.contains(adAxoPart_.getLCode()) && (pD=r8Data_.value(adAxoPart_.getLCode())))
        {
          auxObs->setDdel_dAxsOfs(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setDrat_dAxsOfs(pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adNgradPar_.getLCode()) && (pD=r8Data_.value(adNgradPar_.getLCode())))
        {
          auxObs->setDdel_dTzdGrdN(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setDdel_dTzdGrdE(pD->value(0, 1, obsIdx, stnIdx));
          auxObs->setDrat_dTzdGrdN(pD->value(1, 0, obsIdx, stnIdx));
          auxObs->setDrat_dTzdGrdE(pD->value(1, 1, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adNdryPart_.getLCode()) && (pD=r8Data_.value(adNdryPart_.getLCode())))
        {
          auxObs->setDdel_dTzdDry(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setDrat_dTzdDry(pD->value(1, 0, obsIdx, stnIdx));
        };
        if (r8Data_.contains(adNwetPart_.getLCode()) && (pD=r8Data_.value(adNwetPart_.getLCode())))
        {
          auxObs->setDdel_dTzdWet(pD->value(0, 0, obsIdx, stnIdx));
          auxObs->setDrat_dTzdWet(pD->value(1, 0, obsIdx, stnIdx));
        };
        // eo4c
            
            
            
      };
      //
      // from <<session>> scope:
      if (r8Data_.contains(adStnCoord_.getLCode()))
      {
        v3(X_AXIS) = r8Data_.value(adStnCoord_.getLCode())->value(0, stnIdx, 0, 0);
        v3(Y_AXIS) = r8Data_.value(adStnCoord_.getLCode())->value(1, stnIdx, 0, 0);
        v3(Z_AXIS) = r8Data_.value(adStnCoord_.getLCode())->value(2, stnIdx, 0, 0);
        stn->setR(v3);
      };
      if (c1Data_.contains(adTectPlNm_.getLCode()))
        stn->setTectonicPlateName(c1Data_.value(adTectPlNm_.getLCode())->value(stnIdx, 0, 0));

      session_->calcInfo().stations().append(stn);

      if (r8Data_.contains(adAtmInterv_.getLCode()))
        stn->pcZenith()->setPwlStep(
          r8Data_.value(adAtmInterv_.getLCode())->value(stnIdx, 0, 0, 0)/24.0);
      if (r8Data_.contains(adAtmConstr_.getLCode()))
        stn->pcZenith()->setPwlAPriori(
          r8Data_.value(adAtmConstr_.getLCode())->value(stnIdx, 0, 0, 0)*1.0e-12*24.0*vLight*100.0);

      if (r8Data_.contains(adClkInterv_.getLCode()))
        stn->pcClocks()->setPwlStep(
          r8Data_.value(adClkInterv_.getLCode())->value(stnIdx, 0, 0, 0)/24.0);
      if (r8Data_.contains(adClkConstr_.getLCode()))
        stn->pcClocks()->setPwlAPriori(
          r8Data_.value(adClkConstr_.getLCode())->value(stnIdx, 0, 0, 0)*864.0);

      // i2:
      if (i2Data_.contains(adCableSgn_.getLCode()))
      {
        if (i2Data_.value(adCableSgn_.getLCode())->value(stnIdx, 0, 0, 0) == 0)
          stn->delAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL);
        else
          stn->setCableCalMultiplierDBCal(i2Data_.value(adCableSgn_.getLCode())->value(stnIdx, 0, 0, 0));
      };
      // cable cal and meteo origin info:
      if (i2Data_.contains(adCablOnTp_.getLCode()))
        stn->setCableCalsOrigin(SgVlbiStationInfo::CableCalsOrigin
          (i2Data_.value(adCablOnTp_.getLCode())->value(stnIdx, 0, 0, 0)));
      if (i2Data_.contains(adMeteOnTp_.getLCode()))
        stn->setMeteoDataOrigin(SgVlbiStationInfo::MeteoDataOrigin
          (i2Data_.value(adMeteOnTp_.getLCode())->value(stnIdx, 0, 0, 0)));
      // strings:
      if (c1Data_.contains(adCablOnTx_.getLCode()))
      {
        const QString           &s=c1Data_.value(adCablOnTx_.getLCode())->value(stnIdx, 0, 0);
        stn->setCableCalsOriginTxt(s==QString("_")?QString(""):s);
      };
      if (c1Data_.contains(adMeteOnTx_.getLCode()))
      {
        const QString           &s=c1Data_.value(adMeteOnTx_.getLCode())->value(stnIdx, 0, 0);
        stn->setMeteoDataOriginTxt(s==QString("_")?QString(""):s);
      };

      // station calibrations:
      // currently, we use only cable cal from the list:
      if (isCableCalMatched && i2Data_.contains(adStnCalFlg_.getLCode()))
      {
        d = i2Data_.value(adStnCalFlg_.getLCode())->value(stnIdx, 0, 0, 0);
        if (d & 1<<cableCalFlagIdx)
          stn->delAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
        else
          stn->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
      };
      //
      //
      //
      if (i2Data_.contains(adBlnStatus_.getLCode()))
      {
        SgVlbiBaselineInfo     *bi=NULL, *biR=NULL;
        for (int j=0; j<numOfStn_; j++)
        {
          station2Name = stnNames_.at(j);
          d = i2Data_.value(adBlnStatus_.getLCode())->value(stnIdx, j, 0, 0);

          if (session_->baselinesByName().contains(stnName + ":" + station2Name))
            bi = session_->baselinesByName().value(stnName + ":" + station2Name);
          else
            bi = NULL;
          if (session_->baselinesByName().contains (station2Name + ":" + stnName))
            biR = session_->baselinesByName().value(station2Name + ":" + stnName);
          else
            biR = NULL;
          // check for deselect flag:
          if (!(d & (1<<1)))
          {
            if (bi && !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
            {
              bi ->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
              logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                "::initSession(): the baseline " + bi->getKey() + " has been deselected");
            };
            if (biR && !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
            {
              biR->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
              logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                "::initSession(): the baseline " + biR->getKey() + " has been deselected");
            };
          };
          // check for baseline clocks flag:
          if ((d & (1<<3)))
          {
            if (bi)
            {
              bi ->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
              logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                "::initSession(): clock estimation has been assigned to the baseline " + bi->getKey());
            };
            if (biR)
            {
              biR->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
              logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                "::initSession(): clock estimation has been assigned to the baseline " + biR->getKey());
            };
          };
        };
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::initSession(): cannot find a station #" + stnIdx + " in the map");
  };
  //
  //
  // sources:
  for (int srcIdx=0; srcIdx<numOfSrc_; srcIdx++) // use sources order from the input file
  {
    sourceName = sourceNames.at(srcIdx);
    SgVlbiSourceInfo           *src=session_->sourcesByName().value(sourceName);
    if (src)
    {
      if (r8Data_.contains(adSrcCoord_.getLCode()))
      {
        src->setRA(r8Data_.value(adSrcCoord_.getLCode())->value(0, srcIdx, 0, 0));
        src->setDN(r8Data_.value(adSrcCoord_.getLCode())->value(1, srcIdx, 0, 0));
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
          "::initSession(): cannot find coordinates of the source \"" + sourceName + "\"");

      if (c1Data_.contains(adSrcApRef_.getLCode()))
        src->setAprioriReference(c1Data_.value(adSrcApRef_.getLCode())->value(srcIdx, 0, 0));
      
      if (i2Data_.contains(adSrcStatus_.getLCode()))
      {
        d = i2Data_.value(adSrcStatus_.getLCode())->value(srcIdx, 0, 0, 0);
        src->applyStatusBits(d);
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::initSession(): cannot find source \"" + sourceName + "\" in the map");
  };

  //4CALC:
  //
  if (r8Data_.contains(adOPTLCoef_.getLCode()) &&
      r8Data_.contains(adSitOcPhs_.getLCode()) &&
      r8Data_.contains(adSitOcAmp_.getLCode()) &&
      r8Data_.contains(adSitHWOPh_.getLCode()) &&
      r8Data_.contains(adSitHWOAm_.getLCode()) &&
      r8Data_.contains(adSitHSOPh_.getLCode()) &&
      r8Data_.contains(adSitHSOAm_.getLCode())  )
  {
    int                         l;
    n = datumByKey_.value(adOPTLCoef_.getLCode())->d1();
    l = datumByKey_.value(adSitOcPhs_.getLCode())->d2();
    
    if (n != datumByKey_.value(adSitOcPhs_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanUpPhase mismatch");
    else if (n != datumByKey_.value(adSitHWOPh_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanHorizontalWestPhase mismatch");
    else if (n != datumByKey_.value(adSitHSOPh_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanHorizontalSouthPhase mismatch");
    else if (n != datumByKey_.value(adSitOcAmp_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanUpAmp mismatch");
    else if (n != datumByKey_.value(adSitHWOAm_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanHorizontalWestAmp mismatch");
    else if (n != datumByKey_.value(adSitHSOAm_.getLCode())->d1())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and OceanHorizontalSouthAmp mismatch");
    else if (n != session_->calcInfo().stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of OceanPoleTideCoef and number of stations mismatch");
    else if (l != 11)
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): unexpected dimension of OceanUpPhase");
    else if (l != datumByKey_.value(adSitHWOPh_.getLCode())->d2())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): unexpected dimension(2) of OceanHorizontalWestPhase");
    else if (l != datumByKey_.value(adSitHSOPh_.getLCode())->d2())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): unexpected dimension(2) of OceanHorizontalSouthPhase");
    else if (l != datumByKey_.value(adSitOcAmp_.getLCode())->d2())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): unexpected dimension of OceanUpAmp");
    else
    {
      for (int i=0; i<n; i++)
      {
        SgVlbiStationInfo       *si=session_->calcInfo().stations().at(i);
        if (si)
        {
          for (int j=0; j<l; j++)
          {
            si->setOLoadPhase(j, 0,  r8Data_.value(adSitOcPhs_.getLCode())->value(i, j, 0, 0));
            si->setOLoadPhase(j, 1,  r8Data_.value(adSitHWOPh_.getLCode())->value(i, j, 0, 0));
            si->setOLoadPhase(j, 2,  r8Data_.value(adSitHSOPh_.getLCode())->value(i, j, 0, 0));
            
            si->setOLoadAmplitude(j, 0, r8Data_.value(adSitOcAmp_.getLCode())->value(i, j, 0, 0));
            si->setOLoadAmplitude(j, 1, r8Data_.value(adSitHWOAm_.getLCode())->value(i, j, 0, 0));
            si->setOLoadAmplitude(j, 2, r8Data_.value(adSitHSOAm_.getLCode())->value(i, j, 0, 0));
          };
          for (int j=0; j<6; j++)
            si->setOptLoadCoeff(j, r8Data_.value(adOPTLCoef_.getLCode())->value(i, j, 0, 0));
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::initSession(): the station # " + QString("").setNum(i) + " is NULL");
      };
    };
  };
  //
  //
  if (r8Data_.contains(adFwobInf_ .getLCode()) && 
      r8Data_.contains(adFut1Inf_ .getLCode()) && 
      c1Data_.contains(adUt1Intrp_.getLCode()) && 
      c1Data_.contains(adUt1Mess_ .getLCode()) && 
      c1Data_.contains(adWobMess_ .getLCode()) && 
      c1Data_.contains(adWobIntrp_.getLCode()) && 
      c1Data_.contains(adFut1Text_.getLCode()) && 
      c1Data_.contains(adFwobText_.getLCode()) && 
      r8Data_.contains(adFut1Pts_ .getLCode()) && 
      r8Data_.contains(adFwobXnYt_.getLCode())  )
  {
    double                      t0;
    double                      dt;
    unsigned int                numOfPts;

    t0 = r8Data_.value(adFut1Inf_.getLCode())->value(0, 0, 0, 0);
    dt = r8Data_.value(adFut1Inf_.getLCode())->value(1, 0, 0, 0);
    numOfPts = round(r8Data_.value(adFut1Inf_.getLCode())->value(2, 0, 0, 0));

    SgVector                   *utValues=new SgVector(numOfPts);
    for (unsigned int i=0; i<numOfPts; i++)
      utValues->setElement(i,  r8Data_.value(adFut1Pts_.getLCode())->value(i, 0, 0, 0));

    if (t0 > 2390000.0)
      t0 -= 2400000.5;
      
    session_->allocUt1InterpolEpochs(t0, dt, numOfPts);
    *utValues *= -1.0;
    session_->allocUt1InterpolValues(utValues);
    session_->setHasUt1Interpolation(true);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): loaded " + QString().setNum(numOfPts) + 
      " records for UT1 interpolation");

    t0 = r8Data_.value(adFwobInf_.getLCode())->value(0, 0, 0, 0);
    dt = r8Data_.value(adFwobInf_.getLCode())->value(1, 0, 0, 0);
    numOfPts = round(r8Data_.value(adFwobInf_.getLCode())->value(2, 0, 0, 0));
    if (t0 > 2390000.0)
      t0 -= 2400000.5;

    SgMatrix                   *pmValues=new SgMatrix(numOfPts, 2);
    for (unsigned int i=0; i<numOfPts; i++)
    {
      pmValues->setElement(i, 0, r8Data_.value(adFwobXnYt_.getLCode())->value(i, 0, 0, 0));
      pmValues->setElement(i, 1, r8Data_.value(adFwobXnYt_.getLCode())->value(i, 1, 0, 0));
    };

    session_->allocPxyInterpolEpochs(t0, dt, numOfPts);
    session_->allocPxyInterpolValues(pmValues);
    session_->setHasPxyInterpolation(true);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): loaded " + QString().setNum(numOfPts) + 
      " records for polar motion interpolation");

    session_->calcInfo().ut1Interpolation().setControlFlag(
      c1Data_.value(adUt1Intrp_.getLCode())->value(0, 0, 0));
    session_->calcInfo().polarMotionInterpolation().setControlFlag(
      c1Data_.value(adWobIntrp_.getLCode())->value(0, 0, 0));
    session_->calcInfo().ut1Interpolation().setDefinition(
      c1Data_.value(adUt1Mess_.getLCode())->value(0, 0, 0));
    session_->calcInfo().polarMotionInterpolation().setDefinition(
      c1Data_.value(adWobMess_.getLCode())->value(0, 0, 0));
    session_->calcInfo().ut1Interpolation().setOrigin(
      c1Data_.value(adFut1Text_.getLCode())->value(0, 0, 0));
    session_->calcInfo().polarMotionInterpolation().setOrigin(
      c1Data_.value(adFwobText_.getLCode())->value(0, 0, 0));
    //
    switch (session_->calcInfo().getFlagTidalUt1())
    {
      case 1:
        session_->setTabsUt1Type(SgTidalUt1::CT_FULL);
        str = "Seriesfull of tides";
      break;
      case -1: // UT1R
        session_->setTabsUt1Type(SgTidalUt1::CT_SHORT_TERMS_REMOVED);
        str = "Only short period tidal terms are removed";
      break;
      case -2: // UT1S
        session_->setTabsUt1Type(SgTidalUt1::CT_ALL_TERMS_REMOVED);
        str = "All tidal terms are removed";
      break;
      default:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got undocumented value for the TIDALUT1 value: " + 
          QString().setNum(session_->calcInfo().getFlagTidalUt1()) + 
          "; the UT1 type has been reverted to default");
      break;
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): the type of the UT1 inner series has been set to " + str);
    //
  }
  else
  {
    if (session_->args4Ut1Interpolation())
    {
      delete session_->args4Ut1Interpolation();
      session_->args4Ut1Interpolation() = NULL;
    };
    if (session_->tabs4Ut1Interpolation())
    {
      delete session_->tabs4Ut1Interpolation();
      session_->tabs4Ut1Interpolation() = NULL;
    };
    session_->setHasUt1Interpolation(false);
  
    if (session_->args4PxyInterpolation())
    {
      delete session_->args4PxyInterpolation();
      session_->args4PxyInterpolation() = NULL;
    };
    if (session_->tabs4PxyInterpolation())
    {
      delete session_->tabs4PxyInterpolation();
      session_->tabs4PxyInterpolation() = NULL;
    };
    session_->setHasPxyInterpolation(false);
  };
  //
  //


  if (i2Data_.contains(adAxisTyps_.getLCode()))
  {
    if ((n = datumByKey_.value(adAxisTyps_.getLCode())->d1()) != session_->calcInfo().stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of AxisTyps and number of stations mismatch");
    else
      for (int i=0; i<n; i++)
      {
        SgVlbiStationInfo       *si=session_->calcInfo().stations().at(i);
        if (si)
          si->setMntType(SgVlbiStationInfo::int2mntType(
            i2Data_.value(adAxisTyps_.getLCode())->value(i, 0, 0, 0)));
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::initSession(): the station # " + QString("").setNum(i) + " is NULL");
      };
  };

  if (r8Data_.contains(adAxisOffs_.getLCode()))
  {
    if ((n = datumByKey_.value(adAxisOffs_.getLCode())->d1()) != session_->calcInfo().stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of AxisOffs and number of stations mismatch");
    else
      for (int i=0; i<n; i++)
      {
        SgVlbiStationInfo       *si=session_->calcInfo().stations().at(i);
        if (si)
          si->setAxisOffset(r8Data_.value(adAxisOffs_.getLCode())->value(i, 0, 0, 0));
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::initSession(): the station # " + QString("").setNum(i) + " is NULL");
      };
  };
  if (r8Data_.contains(adAxisTilt_.getLCode()))
  {
    if ((n = datumByKey_.value(adAxisTilt_.getLCode())->d1()) != session_->calcInfo().stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of AxisTilt and number of stations mismatch");
    else
      for (int i=0; i<n; i++)
      {
        SgVlbiStationInfo       *si=session_->calcInfo().stations().at(i);
        if (si)
        {
          si->setTilt(0, r8Data_.value(adAxisTilt_.getLCode())->value(i, 0, 0, 0));
          si->setTilt(1, r8Data_.value(adAxisTilt_.getLCode())->value(i, 1, 0, 0));
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::initSession(): the station # " + QString("").setNum(i) + " is NULL");
      };
  };
  //
  // ECCs:
  if (c1Data_.contains(adEccTypes_.getLCode()) && 
      r8Data_.contains(adEccCoord_.getLCode()) && 
      c1Data_.contains(adEccNames_.getLCode())  )
  {
    if ((n = datumByKey_.value(adEccTypes_.getLCode())->d2()) != session_->calcInfo().stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::initSession(): dimensions of EccTypes and number of stations mismatch");
    else
      for (int i=0; i<n; i++)
      {
        SgVlbiStationInfo       *si=session_->calcInfo().stations().at(i);
        if (si)
        {
          si->eccRec() = new SgEccRec();
          if (c1Data_.value(adEccTypes_.getLCode())->value(i, 0, 0) == "XY")
            si->eccRec()->setEccType(SgEccRec::ET_XYZ);
          else if (c1Data_.value(adEccTypes_.getLCode())->value(i, 0, 0) == "NE")
            si->eccRec()->setEccType(SgEccRec::ET_NEU);
          else
          {
            si->eccRec()->setEccType(SgEccRec::ET_N_A);
            logger->write(SgLogger::ERR, SgLogger::IO, className() +
              "::initSession(): cannot figure out Ecc type from the string \"" + 
              c1Data_.value(adEccTypes_.getLCode())->value(i, 0, 0) +  "\" for the station \"" +
              si->getKey() + "\"");
          };
          //
          si->eccRec()->setDR(
            Sg3dVector( r8Data_.value(adEccCoord_.getLCode())->value(i, 0, 0, 0),
                        r8Data_.value(adEccCoord_.getLCode())->value(i, 1, 0, 0),
                        r8Data_.value(adEccCoord_.getLCode())->value(i, 2, 0, 0)));
          //
          si->eccRec()->setCdpNumber(c1Data_.value(adEccNames_.getLCode())->value(i, 0, 0).simplified());
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::initSession(): the station # " + QString("").setNum(i) + " is NULL");
      };
  };
  //
  //
  //
  //
  // phase three:

  // reference clocks:
  bool                          hasRC(false);
  if (c1Data_.contains(adRefClocks_.getLCode()))
  {
    for (int i=0; i<datumByKey_.value(adRefClocks_.getLCode())->d2(); i++)
    {
      QString                   clockSiteName(
                                  c1Data_.value(adRefClocks_.getLCode())->value(i, 0, 0).
                                  leftJustified(8, ' '));
      if (session_->stationsByName().contains(clockSiteName))
      {
        session_->stationsByName().value(clockSiteName)->
          addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
        hasRC = true;
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::initSession(): Reference Clock attribute has been assigned to the station " +
          session_->stationsByName().value(clockSiteName)->getKey());
      };
    };
  }
  else if (c1Data_.contains(adStaClRf_.getLCode()))
  {
    for (int i=0; i<datumByKey_.value(adStaClRf_.getLCode())->d2(); i++)
    {
      QString                   clockSiteName(
                                  c1Data_.value(adStaClRf_.getLCode())->value(i, 0, 0).
                                  leftJustified(8, ' '));
      if (session_->stationsByName().contains(clockSiteName))
      {
        session_->stationsByName().value(clockSiteName)->
          addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
        hasRC = true;
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::initSession(): Reference Clock attribute has been assigned to the station " +
          session_->stationsByName().value(clockSiteName)->getKey() + ", PIMA's version");
      };
    };
  };
  if (!hasRC)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::initSession(): no Reference Clock attribute has been found for the session");

  //
  // clock breaks:
  if (i4Data_.contains(adCbrNumber_.getLCode()) && c1Data_.contains(adCbrSite_.getLCode()) &&
      i4Data_.contains(adCbrImjd_.getLCode()) && r8Data_.contains(adCbrTime_.getLCode()))
  {
    int                         numOfClockBreak=
                                  i4Data_.value(adCbrNumber_.getLCode())->value(0, 0, 0, 0);
    //
    for (int i=0; i<numOfClockBreak; i++)
    {
      epoch.setDate(i4Data_.value(adCbrImjd_.getLCode())->value(i, 0, 0, 0));
      epoch.setTime(r8Data_.value(adCbrTime_.getLCode())->value(i, 0, 0, 0)/86400.0);
      station1Name = c1Data_.value(adCbrSite_.getLCode())->value(i, 0, 0).leftJustified(8, ' ');
      if (session_->stationsByName().contains(station1Name))
      {
        station1Info = session_->stationsByName().value(station1Name);
        SgParameterBreak       *clockBreak=new SgParameterBreak(epoch, 0.0);
        clockBreak->addAttr(SgParameterBreak::Attr_DYNAMIC);
        if (station1Info->clockBreaks().addBreak(clockBreak))
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
            "::initSession(): a clock break at station " + station1Info->getKey() +
            " that occurred on " + epoch.toString() + " has been added");
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::initSession(): adding new clock break at " + station1Info->getKey() +
            " station on " + epoch.toString() + " has failed");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::initSession(): cannot find station \"" + station1Name +
          "\" in the map of stations, adding a clock break has failed");
    };
    session_->addAttr(SgVlbiSessionInfo::Attr_HAS_CLOCK_BREAKS);
  };
  

  // calibrations:
  if (c1Data_.contains(adStnCalNam_.getLCode()))
  {
    adStnCalNam_.setDim2(6);
    c1Data_.value(adStnCalNam_.getLCode())->allocateSpace();
    for (int i=0; i<6; i++)
    {
      c1Data_.value(adStnCalNam_.getLCode())->value(i, 0, 0) = sCalList[i];
      // special case:
      c1Data_.value(adStnCalNam_.getLCode())->value(i, 0, 0).replace(' ', '_');
    }
  };
  if (c1Data_.contains(adFlyCalNam_.getLCode()))
  {
    adFlyCalNam_.setDim2(8);
    c1Data_.value(adFlyCalNam_.getLCode())->allocateSpace();
    for (int i=0; i<8; i++)
      c1Data_.value(adFlyCalNam_.getLCode())->value(i, 0, 0) = sFclList[i];
  };
  
  //
  //
  // aux weights for baselines:
  if (c1Data_.contains(adErrorBl_.getLCode()) && r8Data_.contains(adErrorK_.getLCode()))
  {
    numOfBln_ = datumByKey_.value(adErrorBl_.getLCode())->d2();
    if (numOfBln_ != (d=datumByKey_.value(adErrorK_.getLCode())->d2()))
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::initSession(): dimensions of ErrorBl (" + QString("").setNum(numOfBln_) + 
        ") and ErrorK (" + QString("").setNum(d) + ") lCodes mismatch");
    else
      for (int i=0; i<numOfBln_; i++)
      {
        baselineName = c1Data_.value(adErrorBl_.getLCode())->value(i, 0, 0).leftJustified(16, ' ');
        baselineName.replace('_', ' ');
        baselineName = baselineName.insert(8, ":");
//      station1Name = baselineName.left(8);
//      station2Name = baselineName.right(8);
//      baselineName = station1Name + ":" + station2Name;
        if (session_->baselinesByName().contains(baselineName))
        {
          baselineInfo = session_->baselinesByName().value(baselineName);
          baselineInfo->setSigma2add(DT_DELAY, r8Data_.value(adErrorK_.getLCode())->value(0, i, 0, 0));
          baselineInfo->setSigma2add(DT_RATE,  r8Data_.value(adErrorK_.getLCode())->value(1, i, 0, 0));
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::initSession(): cannot find baseline \"" + baselineName +
            "\" in the map of baselines, cannot set up weight corrections");
      };
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_WEIGHTS_CORRECTED);
  };
  // baseline clocks:
  if (c1Data_.contains(adBlnClocks_.getLCode()))
  {
    // adjust the size:
    d = datumByKey_.value(adBlnClocks_.getLCode())->d2();
    for (int i=0; i<d; i++)
    {
      baselineName = c1Data_.value(adBlnClocks_.getLCode())->value(i, 0, 0).leftJustified(16, ' ');
      baselineName.replace('_', ' ');
      baselineName = baselineName.insert(8, ":");
      if (session_->baselinesByName().contains(baselineName))
      {
        baselineInfo = session_->baselinesByName().value(baselineName);
        if (!baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS)) // wtf?
        {
          baselineInfo->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::initSession(): baseline clocks were corrected for \"" + baselineName +
            "\" (missed in BASLSTAT)");
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::initSession(): cannot find baseline \"" + baselineName +
          "\" in the map of baselines, cannot set up baseline clocks");
    };
  };


  delete []adNumOfAp;
  delete []adIndexNum;
  delete []adChanId;
  delete []adPolarz;
  delete []adBbcIdx;
  delete []adErrRate;
  delete []adRfFreq;
  delete []adLoRfFreq;
  delete []adNumSmpls;
  delete []adChAmpPhs;
  delete []adUvChn;
  delete []adPhcFrq;
  delete []adPhcAmp;
  delete []adPhcPhs;
  delete []adPhcCm;
  delete []adPhcOff;


/*
std::cout << "   extr: UserName         = [" << qPrintable(inputIdentities_->getUserName()) << "]\n";
std::cout << "   extr: UserEmailAddress = [" << qPrintable(inputIdentities_->getUserEmailAddress()) << "]\n";
std::cout << "   extr: AcAbbrevName     = [" << qPrintable(inputIdentities_->getAcAbbrevName()) << "]\n";

std::cout << "   extr: creator=     [" << qPrintable(inputIdentities_->getDriverVersion().name()) 
<< "] released " << qPrintable(inputIdentities_->getDriverVersion().getReleaseEpoch().toString()) << "\n";
std::cout << "   extr: released by= [" << qPrintable(inputIdentities_->getDriverVersion().name())
<< "] released " << qPrintable(inputIdentities_->getDriverVersion().getReleaseEpoch().toString()) << "\n";


std::cout << "initSession(): done"
<< qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
<< "\n";
*/
};



//
void SgAgvDriver::setCalcInfoModelMessNCtrlF(const QString& messLc, const QString& cflgLc,
  const SgModelsInfo::DasModel& model)
{
  if (c1Data_.contains(messLc) && c1Data_.contains(cflgLc))
  {
    c1Data_.value(messLc)->value(0, 0, 0) = model.getDefinition();
    c1Data_.value(cflgLc)->value(0, 0, 0) = model.getControlFlag();
  };
};



//
void SgAgvDriver::getCalcInfoModelMessNCtrlF(const QString& messLc, const QString& cflgLc,
  SgModelsInfo::DasModel& model)
{
  if (c1Data_.contains(messLc) && c1Data_.contains(cflgLc))
  {
    model.setDefinition (c1Data_.value(messLc)->value(0, 0, 0));
    model.setControlFlag(c1Data_.value(cflgLc)->value(0, 0, 0));
  };
};

/*=====================================================================================================*/








/*=====================================================================================================*/

/*=====================================================================================================*/
