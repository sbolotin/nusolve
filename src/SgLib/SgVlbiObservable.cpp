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


#include <QtCore/QDataStream>
#include <QtCore/QVector>


#include <SgLogger.h>
#include <SgMathSupport.h>
#include <SgTaskConfig.h>
#include <SgVector.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservable.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>



//
// declarations (just to use them only here):
void findPeak(const std::complex<double> *cA, int n, double &dD, double &dP);

//
bool evaluatePhaseCals(const SgVector& pcPhases, const SgVector& pcAmplitudes,
  const SgVector& pcFreqByChan, double refFreq,
  double& phaseCalGrd, double& phaseCalGrdAmbig, 
  double& phaseCalPhd, double& phaseCalPhdAmbig,
  const QString& oId);



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiObservable::className()
{
  return "SgVlbiObservable";
};



// CONSTRUCTORS:
//
// An empty constructor:
SgVlbiObservable::SgVlbiObservable(SgVlbiObservation* obs) :
  bandKey_(""),
  sbDelay_("Single Band Delay", SgTaskConfig::VD_SB_DELAY, SgTaskConfig::VR_NONE),
  grDelay_("Group Delay", SgTaskConfig::VD_GRP_DELAY, SgTaskConfig::VR_NONE),
  phDelay_("Phase Delay", SgTaskConfig::VD_PHS_DELAY, SgTaskConfig::VR_NONE),
  phDRate_("Phase Rate", SgTaskConfig::VD_NONE, SgTaskConfig::VR_PHS_RATE),
  errorCode_(""),
  tapeQualityCode_(""),
  nonUsableReason_(),
  fourfitOutputFName_(""),
  fourfitControlFile_(""),
  fourfitCommandOverride_(""),
  epochOfCorrelation_(tZero),
  epochOfFourfitting_(tZero),
  epochOfScan_(tZero),
  epochCentral_(tZero),
  tStart_(tZero),
  tStop_(tZero)
{
  mediaIdx_ = -1;
  owner_ = obs;
  band_ = NULL;
  baseline_ = NULL;
  stn_1_ = NULL;
  stn_2_ = NULL;
  src_ = NULL;
  referenceFrequency_ = 0.0;
  totalPhase_ = 0.0;
  corrCoeff_ = 0.0;
  snr_ = 0.0;
  qualityFactor_ = 0;
  numOfChannels_ = 0;
  isUsable_ = true;
//  nonUsableReason_ = NUR_UNDEF;
  activeDelay_ = &sbDelay_;
  activeRate_  = NULL;
  activeMeasurement_ = activeDelay_;
  fourfitVersion_[0] = -1;
  fourfitVersion_[1] = -1;
  //
  startOffset_ = 0;
  stopOffset_ = 0;
  centrOffset_ = 0.0;
  sampleRate_ = 0.0;
  bitsPerSample_ = 0;
  effIntegrationTime_ = 0.0;
  acceptedRatio_ = 0.0;
  discardRatio_ = 0.0;
  incohChanAddAmp_ = 0.0;
  incohSegmAddAmp_ = 0.0;
  probabOfFalseDetection_ = 0.0;
  geocenterTotalPhase_ = 0.0;
  geocenterResidPhase_ = 0.0;
  aPrioriDra_[0] = aPrioriDra_[1] = aPrioriDra_[2] = 0.0;
  calcFeedCorrDelay_ = calcFeedCorrRate_ = 0.0;
  correlStarElev_1_ = correlZdelay_1_ = 0.0;
  correlStarElev_2_ = correlZdelay_2_ = 0.0;

  //
  for (int i=0; i<2; i++)
  {
    phaseCalGrDelays_[i] = phaseCalPhDelays_[i] = 
    phaseCalGrAmbigSpacings_[i] = phaseCalPhAmbigSpacings_[i] =
    phaseCalRates_[i] = uvFrPerAsec_[i] = uRvR_[i] = corrClocks_[i][0] = 
    corrClocks_[i][1] = instrDelay_[i] = 0.0;
    phaseCalGrAmbigMultipliers_[i] = phaseCalPhAmbigMultipliers_[i] = 0;
  };
  nLags_ = 0;
  for (int i=0; i<6; i++)
    fourfitSearchParameters_[i] = 0.0;
  hopsRevisionNumber_ = 0;

  numOfAccPeriodsByChan_USB_ = NULL;
  numOfAccPeriodsByChan_LSB_ = NULL;
  numOfSamplesByChan_USB_ = NULL;
  numOfSamplesByChan_LSB_ = NULL;
  refFreqByChan_ = NULL;
  fringeAmplitudeByChan_ = NULL;
  fringePhaseByChan_ = NULL;
  channelBandwidth_   = NULL;
  polarization_1ByChan_ = NULL;
  polarization_2ByChan_ = NULL;
  phaseCalData_1ByChan_ = NULL;
  phaseCalData_2ByChan_ = NULL;
  vDlys_ = NULL;
  vAuxData_ = NULL;
  corelIndexNumUSB_ = NULL;
  corelIndexNumLSB_ = NULL;

  chanIdByChan_     = NULL;
  loFreqByChan_1_   = NULL;
  loFreqByChan_2_   = NULL;
  bbcIdxByChan_1_   = NULL;
  bbcIdxByChan_2_   = NULL;


  phaseCalModes_ = -1;
  //
  // temporary/tests:
  sbdDiffBand_ = 0.0;
  grdDiffBand_ = 0.0;
  phrDiffBand_ = 0.0;
  sbdQ_ = 0.0;
  grdQ_ = 0.0;
  phrQ_ = 0.0;
  dTauS_= 0.0;
  dTest_ = dTest2_ = 0.0;
  isActive_ = false;
};



//
SgVlbiObservable::SgVlbiObservable(SgVlbiObservation* obs, SgVlbiBand* band) :
  bandKey_(band?band->getKey():""),
  sbDelay_("Single Band Delay", SgTaskConfig::VD_SB_DELAY, SgTaskConfig::VR_NONE),
  grDelay_("Group Delay", SgTaskConfig::VD_GRP_DELAY, SgTaskConfig::VR_NONE),
  phDelay_("Phase Delay", SgTaskConfig::VD_PHS_DELAY, SgTaskConfig::VR_NONE),
  phDRate_("Phase Rate", SgTaskConfig::VD_NONE, SgTaskConfig::VR_PHS_RATE),
  errorCode_(""),
  tapeQualityCode_(""),
  nonUsableReason_(),
  fourfitOutputFName_(""),
  fourfitControlFile_(""),
  fourfitCommandOverride_(""),
  epochOfCorrelation_(tZero),
  epochOfFourfitting_(tZero),
  epochOfScan_(tZero),
  epochCentral_(tZero),
  tStart_(tZero),
  tStop_(tZero)
{
  mediaIdx_ = -1;
  owner_ = obs;
  band_ = band;
  baseline_ = NULL;
  stn_1_ = NULL;
  stn_2_ = NULL;
  src_ = NULL;
  referenceFrequency_ = 0.0;
  totalPhase_ = 0.0;
  corrCoeff_ = 0.0;
  snr_ = 0.0;
  qualityFactor_ = 0;
  numOfChannels_ = 0;
  isUsable_ = true;
//  nonUsableReason_ = NUR_UNDEF;
  activeMeasurement_ = activeDelay_ = &sbDelay_;
  activeRate_ = NULL;
  fourfitVersion_[0] = 0;
  fourfitVersion_[1] = 0;
  //
  startOffset_ = 0;
  stopOffset_ = 0;
  centrOffset_ = 0.0;
  sampleRate_ = 0.0;
  bitsPerSample_ = 0;
  effIntegrationTime_ = 0.0;
  acceptedRatio_ = 0.0;
  discardRatio_ = 0.0;
  incohChanAddAmp_ = 0.0;
  incohSegmAddAmp_ = 0.0;
  probabOfFalseDetection_ = 0.0;
  geocenterTotalPhase_ = 0.0;
  geocenterResidPhase_ = 0.0;
  aPrioriDra_[0] = aPrioriDra_[1] = aPrioriDra_[2] = 0.0 ;
  calcFeedCorrDelay_ = calcFeedCorrRate_ = 0.0;
  correlStarElev_1_ = correlZdelay_1_ = 0.0;
  correlStarElev_2_ = correlZdelay_2_ = 0.0;
  //
  for (int i=0; i<2; i++)
  {
    phaseCalGrDelays_[i] = phaseCalPhDelays_[i] = 
    phaseCalGrAmbigSpacings_[i] = phaseCalPhAmbigSpacings_[i] =
    phaseCalRates_[i] = uvFrPerAsec_[i] = uRvR_[i] = corrClocks_[i][0] = 
    corrClocks_[i][1] = instrDelay_[i] = 0.0;
    phaseCalGrAmbigMultipliers_[i] = phaseCalPhAmbigMultipliers_[i] = 0;
  };
  nLags_ = 0;
  for (int i=0; i<6; i++)
    fourfitSearchParameters_[i] = 0.0;
  hopsRevisionNumber_ = 0;
  //
  numOfAccPeriodsByChan_USB_ = NULL;
  numOfAccPeriodsByChan_LSB_ = NULL;
  numOfSamplesByChan_USB_ = NULL;
  numOfSamplesByChan_LSB_ = NULL;
  refFreqByChan_ = NULL;
  fringeAmplitudeByChan_ = NULL;
  fringePhaseByChan_ = NULL;
  channelBandwidth_   = NULL;
  polarization_1ByChan_ = NULL;
  polarization_2ByChan_ = NULL;
  phaseCalData_1ByChan_ = NULL;
  phaseCalData_2ByChan_ = NULL;
  vDlys_ = NULL;
  vAuxData_ = NULL;
  corelIndexNumUSB_ = NULL;
  corelIndexNumLSB_ = NULL;

  chanIdByChan_     = NULL;
  loFreqByChan_1_   = NULL;
  loFreqByChan_2_   = NULL;
  bbcIdxByChan_1_   = NULL;
  bbcIdxByChan_2_   = NULL;

  phaseCalModes_ = -1;
  //
  // temporary/tests:
  sbdDiffBand_ = 0.0;
  grdDiffBand_ = 0.0;
  phrDiffBand_ = 0.0;
  sbdQ_ = 0.0;
  grdQ_ = 0.0;
  phrQ_ = 0.0;
  dTauS_= 0.0;
  dTest_ = dTest2_ = 0.0;
  isActive_ = false;
};






// A copy constructor:
SgVlbiObservable::SgVlbiObservable(SgVlbiObservation* obs, const SgVlbiObservable& o) :
  bandKey_(o.getBandKey()),
  sbDelay_(o.sbDelay_),
  grDelay_(o.grDelay_),
  phDelay_(o.phDelay_),
  phDRate_(o.phDRate_),
  errorCode_(o.getErrorCode()),
  tapeQualityCode_(o.getTapeQualityCode()),
  fourfitOutputFName_(o.getFourfitOutputFName()),
  fourfitControlFile_(o.getFourfitControlFile()),
  fourfitCommandOverride_(o.getFourfitCommandOverride()),
  epochOfCorrelation_(o.getEpochOfCorrelation()),
  epochOfFourfitting_(o.getEpochOfFourfitting()),
  epochOfScan_(o.getEpochOfScan()),
  epochCentral_(o.getEpochCentral()),
  tStart_(o.getTstart()),
  tStop_(o.getTstop())
{
  owner_ = obs;
  band_ = o.band_;
  baseline_ = o.getBaseline();
  stn_1_ = o.getStn_1();
  stn_2_ = o.getStn_2();
  src_ = o.getSrc();
  setMediaIdx(o.getMediaIdx());
  setReferenceFrequency(o.getReferenceFrequency());
  setTotalPhase(o.getTotalPhase());
  setCorrCoeff(o.getCorrCoeff());
  setSnr(o.getSnr());
  setQualityFactor(o.getQualityFactor());
  setNumOfChannels(o.getNumOfChannels());
  activeMeasurement_ = activeDelay_ = &sbDelay_;
  activeRate_  = NULL;
  fourfitVersion_[0] = o.getFourfitVersion(0);
  fourfitVersion_[1] = o.getFourfitVersion(1);
  //
  setStartOffset(o.getStartOffset());
  setStopOffset(o.getStopOffset());
  setCentrOffset(o.getCentrOffset());
  setSampleRate(o.getSampleRate());
  setBitsPerSample(o.getBitsPerSample());
  setEffIntegrationTime(o.getEffIntegrationTime());
  setAcceptedRatio(o.getAcceptedRatio());
  setDiscardRatio(o.getDiscardRatio());
  setIncohChanAddAmp(o.getIncohChanAddAmp());
  setIncohSegmAddAmp(o.getIncohSegmAddAmp());
  setProbabOfFalseDetection(o.getProbabOfFalseDetection());
  setGeocenterTotalPhase(o.getGeocenterTotalPhase());
  setGeocenterResidPhase(o.getGeocenterResidPhase());
  setAprioriDra(0, o.getAprioriDra(0));
  setAprioriDra(1, o.getAprioriDra(1));
  setAprioriDra(2, o.getAprioriDra(2));
  setCalcFeedCorrDelay(o.getCalcFeedCorrDelay());
  setCalcFeedCorrRate(o.getCalcFeedCorrRate());
  setCorrelStarElev_1(o.getCorrelStarElev_1());
  setCorrelStarElev_2(o.getCorrelStarElev_2());
  setCorrelZdelay_1(o.getCorrelZdelay_1());
  setCorrelZdelay_2(o.getCorrelZdelay_2());

  for (int i=0; i<2; i++)
  {
    setPhaseCalGrDelays(i, o.getPhaseCalGrDelays(i));
    setPhaseCalPhDelays(i, o.getPhaseCalPhDelays(i));
    setPhaseCalRates(i, o.getPhaseCalRates(i));
    setUvFrPerAsec(i, o.getUvFrPerAsec(i));
    setUrVr(i, o.getUrVr(i));
    setCorrClocks(i, 0, o.getCorrClocks(i, 0));
    setCorrClocks(i, 1, o.getCorrClocks(i, 1));
    setInstrDelay(i, o.getInstrDelay(i));
  }
  setNlags(o.getNlags());
  for (int i=0; i<6; i++)
    setFourfitSearchParameters(i, o.getFourfitSearchParameters(i));
  setHopsRevisionNumber(o.getHopsRevisionNumber());
    
  //
  if (o.numOfAccPeriodsByChan_USB())
    numOfAccPeriodsByChan_USB_ = new SgVector(*o.numOfAccPeriodsByChan_USB());
  else
    numOfAccPeriodsByChan_USB_ = NULL;
    
  if (o.numOfAccPeriodsByChan_LSB())
    numOfAccPeriodsByChan_LSB_ = new SgVector(*o.numOfAccPeriodsByChan_LSB());
  else
    numOfAccPeriodsByChan_LSB_ = NULL;
    
  if (o.numOfSamplesByChan_USB())
    numOfSamplesByChan_USB_ = new SgVector(*o.numOfSamplesByChan_USB());
  else
    numOfSamplesByChan_USB_ = NULL;
    
  if (o.numOfSamplesByChan_LSB())
    numOfSamplesByChan_LSB_ = new SgVector(*o.numOfSamplesByChan_LSB());
  else
    numOfSamplesByChan_LSB_ = NULL;
    
  if (o.refFreqByChan())
    refFreqByChan_ = new SgVector(*o.refFreqByChan());
  else
    refFreqByChan_ = NULL;
    
  if (o.fringeAmplitudeByChan())
    fringeAmplitudeByChan_ = new SgVector(*o.fringeAmplitudeByChan());
  else
    fringeAmplitudeByChan_ = NULL;

  if (o.fringePhaseByChan())
    fringePhaseByChan_ = new SgVector(*o.fringePhaseByChan());
  else
    fringePhaseByChan_ = NULL;

  if (o.channelBandwidth())
    channelBandwidth_ = new SgVector(*o.channelBandwidth());
  else
    channelBandwidth_ = NULL;

  if (o.polarization_1ByChan())
    polarization_1ByChan_ = new QVector<char>(*o.polarization_1ByChan());
  else
    polarization_1ByChan_ = NULL;

  if (o.polarization_2ByChan())
    polarization_2ByChan_ = new QVector<char>(*o.polarization_2ByChan());
  else
    polarization_2ByChan_ = NULL;

  if (o.phaseCalData_1ByChan())
    phaseCalData_1ByChan_ = new SgMatrix(*o.phaseCalData_1ByChan());
  else
    phaseCalData_1ByChan_ = NULL;

  if (o.vDlys())
    vDlys_ = new SgVector(*o.vDlys());
  else
    vDlys_ = NULL;
  
  if (o.vAuxData())
    vAuxData_ = new SgVector(*o.vAuxData());
  else
    vAuxData_ = NULL;

  if (o.phaseCalData_2ByChan())
    phaseCalData_2ByChan_ = new SgMatrix(*o.phaseCalData_2ByChan());
  else
    phaseCalData_2ByChan_ = NULL;

  setPhaseCalModes(o.getPhaseCalModes());

  if (o.corelIndexNumUSB())
    corelIndexNumUSB_ = new QVector<int>(*o.corelIndexNumUSB());
  else
    corelIndexNumUSB_ = NULL;
    
  if (o.corelIndexNumLSB())
    corelIndexNumLSB_ = new QVector<int>(*o.corelIndexNumLSB());
  else
    corelIndexNumLSB_ = NULL;
  
  //
  if (o.chanIdByChan())
    chanIdByChan_ = new QVector<char>(*o.chanIdByChan());
  else
    chanIdByChan_ = NULL;

  if (o.loFreqByChan_1())
    loFreqByChan_1_ = new SgVector(*o.loFreqByChan_1());
  else
    loFreqByChan_1_ = NULL;
  if (o.loFreqByChan_2())
    loFreqByChan_2_ = new SgVector(*o.loFreqByChan_2());
  else
    loFreqByChan_2_ = NULL;
  
  bbcIdxByChan_1_ = o.bbcIdxByChan_1() ? new QVector<int>(*o.bbcIdxByChan_1()) : NULL;
  bbcIdxByChan_2_ = o.bbcIdxByChan_2() ? new QVector<int>(*o.bbcIdxByChan_2()) : NULL;
  //
  // temporary/tests:
  setSbdDiffBand(o.getSbdDiffBand());
  setGrdDiffBand(o.getGrdDiffBand());
  setPhrDiffBand(o.getPhrDiffBand());
  /*
  setSbdQ(o.getSbdQ());
  setGrdQ(o.getGrdQ());
  setPhrQ(o.getPhrQ());
  */
  dTauS_ = o.getTauS();
  dTest_ = o.dTest_;
  dTest2_ = o.dTest2_;
  isActive_ = o.getIsActive();
};



//
void SgVlbiObservable::setupActiveMeasurements(const SgTaskConfig* cfg)
{
  switch (cfg->getUseDelayType())
  {
  default:
  case SgTaskConfig::VD_NONE:
    activeDelay_ = NULL;
    break;
  case SgTaskConfig::VD_SB_DELAY:
    activeDelay_ = &sbDelay_;
    break;
  case SgTaskConfig::VD_GRP_DELAY:
    activeDelay_ = &grDelay_;
    break;
  case SgTaskConfig::VD_PHS_DELAY:
    activeDelay_ = &phDelay_;
    break;
  };
  switch (cfg->getUseRateType())
  {
  default:
  case SgTaskConfig::VR_NONE:
    activeRate_ = NULL;
    break;
  case SgTaskConfig::VR_PHS_RATE:
    activeRate_ = &phDRate_;
    break;
  };
  activeMeasurement_ = activeDelay_?activeDelay_:activeRate_;
};



//
void SgVlbiObservable::checkUsability(const SgTaskConfig* cfg)
{
  isUsable_ = true;
  nonUsableReason_.clearAll();
  //
  if (cfg && getQualityFactor() < cfg->getQualityCodeThreshold())
  {
    isUsable_ = false;
    nonUsableReason_.addAttr(NUR_LOW_QF);
  };
  if (1 < band_->getMaxNumOfChannels() && numOfChannels_ < 2)
  {
    isUsable_ = false;
    nonUsableReason_.addAttr(NUR_ONE_CHANNEL);
  };
  //
  if (errorCode_.simplified().size() &&
      !((cfg->getUseQualityCodeG() && errorCode_.contains('G')) ||
        (cfg->getUseQualityCodeH() && errorCode_.contains('H'))  )  )
  {
    isUsable_ = false;
    nonUsableReason_.addAttr(NUR_HAS_ERROR_CODE);
  };
};



//
const SgMJD& SgVlbiObservable::epoch() const
{
  return owner_?(*owner_):tZero;
};



//
QString SgVlbiObservable::strId() const
{
  QString                       srcN("?"), blnN("?:?");
  SgVlbiSourceInfo             *si=src_;
  SgVlbiBaselineInfo           *bi=baseline_;
  if (!si)
    si = owner_->session()->lookupSourceByIdx(owner_->getSourceIdx());
  if (!bi)
    bi = owner_->session()->lookupBaselineByIdx(owner_->getBaselineIdx());
  if (si)
    srcN = si->getKey();
  if (bi)
    blnN = bi->getKey();
  return 
    " #" + QString("").setNum(mediaIdx_) + " on " + bandKey_ + "-band, " + 
    epoch().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " of " + srcN + " at " + blnN + 
    " [" + owner_->getScanName() + "]";
};



//
void SgVlbiObservable::allocateChannelsSetupStorages(int numOfChans)
{
  releaseChannelsSetupStorages();
  if ((numOfChannels_=numOfChans) > 0)
  {
    numOfAccPeriodsByChan_USB_ = new SgVector(numOfChannels_);
    numOfAccPeriodsByChan_LSB_ = new SgVector(numOfChannels_);
    numOfSamplesByChan_USB_ = new SgVector(numOfChannels_);
    numOfSamplesByChan_LSB_ = new SgVector(numOfChannels_);
    refFreqByChan_ = new SgVector(numOfChannels_);
    fringeAmplitudeByChan_ = new SgVector(numOfChannels_);
    fringePhaseByChan_ = new SgVector(numOfChannels_);
    channelBandwidth_   = new SgVector(numOfChannels_);
    polarization_1ByChan_ = new QVector<char>(numOfChannels_);
    polarization_2ByChan_ = new QVector<char>(numOfChannels_);
    phaseCalData_1ByChan_ = new SgMatrix(5, numOfChannels_);
    phaseCalData_2ByChan_ = new SgMatrix(5, numOfChannels_);
    //
    vDlys_ = new SgVector(3);
    vAuxData_ = new SgVector(5);
    // fixed size (mimic dbedit behavior):
    corelIndexNumUSB_ = new QVector<int>(numOfChannels_);
    corelIndexNumLSB_ = new QVector<int>(numOfChannels_);
    //
    chanIdByChan_ = new QVector<char>(numOfChannels_);
    loFreqByChan_1_ = new SgVector(numOfChannels_);
    loFreqByChan_2_ = new SgVector(numOfChannels_);
    bbcIdxByChan_1_ = new QVector<int>(numOfChannels_);
    bbcIdxByChan_2_ = new QVector<int>(numOfChannels_);
    //
    for (int i=0; i<numOfChannels_; i++)
    {
      (*chanIdByChan_)[i] = '-';
      (*polarization_1ByChan_)[i] = '-';
      (*polarization_2ByChan_)[i] = '-';
    };
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      ": allocateChannelsSetupStorages(): an attempt to allocate data with zero number of channels");
};



//
void SgVlbiObservable::releaseChannelsSetupStorages()
{
  if (numOfAccPeriodsByChan_USB_)
  {
    delete numOfAccPeriodsByChan_USB_;
    numOfAccPeriodsByChan_USB_ = NULL;
  };
  if (numOfAccPeriodsByChan_LSB_)
  {
    delete numOfAccPeriodsByChan_LSB_;
    numOfAccPeriodsByChan_LSB_ = NULL;
  };
  if (numOfSamplesByChan_USB_)
  {
    delete numOfSamplesByChan_USB_;
    numOfSamplesByChan_USB_ = NULL;
  };
  if (numOfSamplesByChan_LSB_)
  {
    delete numOfSamplesByChan_LSB_;
    numOfSamplesByChan_LSB_ = NULL;
  };
  if (refFreqByChan_)
  {
    delete refFreqByChan_;
    refFreqByChan_ = NULL;
  };
  if (fringeAmplitudeByChan_)
  {
    delete fringeAmplitudeByChan_;
    fringeAmplitudeByChan_ = NULL;
  };
  if (fringePhaseByChan_)
  {
    delete fringePhaseByChan_;
    fringePhaseByChan_ = NULL;
  };
  if (channelBandwidth_)
  {
    delete channelBandwidth_;
    channelBandwidth_ = NULL;
  };
  if (polarization_1ByChan_)
  {
    delete polarization_1ByChan_;
    polarization_1ByChan_ = NULL;
  };
  if (polarization_2ByChan_)
  {
    delete polarization_2ByChan_;
    polarization_2ByChan_ = NULL;
  };
  if (phaseCalData_1ByChan_)
  {
    delete phaseCalData_1ByChan_;
    phaseCalData_1ByChan_ = NULL;
  };
  if (phaseCalData_2ByChan_)
  {
    delete phaseCalData_2ByChan_;
    phaseCalData_2ByChan_ = NULL;
  };
  if (vDlys_)
  {
    delete vDlys_;
    vDlys_ = NULL;
  };
  if (vAuxData_)
  {
    delete vAuxData_;
    vAuxData_ = NULL;
  };
  if (corelIndexNumUSB_)
  {
    delete corelIndexNumUSB_;
    corelIndexNumUSB_ = NULL;
  }; 
  if (corelIndexNumLSB_)
  {
    delete corelIndexNumLSB_;
    corelIndexNumLSB_ = NULL;
  }; 

  if (chanIdByChan_)
  {
    delete chanIdByChan_;
    chanIdByChan_ = NULL;
  }; 
  if (loFreqByChan_1_)
  {
    delete loFreqByChan_1_;
    loFreqByChan_1_ = NULL;
  }; 
  if (loFreqByChan_2_)
  {
    delete loFreqByChan_2_;
    loFreqByChan_2_ = NULL;
  }; 
  if (bbcIdxByChan_1_)
  {
    delete bbcIdxByChan_1_;
    bbcIdxByChan_1_ = NULL;
  }; 
  if (bbcIdxByChan_2_)
  {
    delete bbcIdxByChan_2_;
    bbcIdxByChan_2_ = NULL;
  }; 
};



//
/*
int SgVlbiObservable::qualityFactor() const
{
  int                           qFactor;
  bool                          isOk;
  qFactor = qualityCode_.toInt(&isOk);
  if (!isOk)
    qFactor = -1;
  return qFactor;
};
*/



//
int SgVlbiObservable::errorCode2Int() const
{
  int                           n=-29;
  if (errorCode_ == "" || errorCode_ == " " || errorCode_ == "  ")
    n = 0;
  else if (errorCode_ == "A")
    n = -1;
  else if (errorCode_ == "B")
    n = -2;
  else if (errorCode_ == "C")
    n = -3;
  else if (errorCode_ == "D")
    n = -4;
  else if (errorCode_ == "E")
    n = -5;
  else if (errorCode_ == "F")
    n = -6;
  else if (errorCode_ == "G")
    n = -7;
  else if (errorCode_ == "H")
    n = -8;
  else if (errorCode_ == "I")
    n = -9;
  else if (errorCode_ == "J")
    n = -10;
  else if (errorCode_ == "K")
    n = -11;
  else if (errorCode_ == "L")
    n = -12;
  else if (errorCode_ == "M")
    n = -13;
  else if (errorCode_ == "N")
    n = -14;
  else if (errorCode_ == "O")
    n = -15;
  else if (errorCode_ == "P")
    n = -16;
  else if (errorCode_ == "Q")
    n = -17;
  else if (errorCode_ == "R")
    n = -18;
  else if (errorCode_ == "S")
    n = -19;
  else if (errorCode_ == "T")
    n = -20;
  else if (errorCode_ == "U")
    n = -21;
  else if (errorCode_ == "V")
    n = -22;
  else if (errorCode_ == "W")
    n = -23;
  else if (errorCode_ == "X")
    n = -24;
  else if (errorCode_ == "Y")
    n = -25;
  else if (errorCode_ == "Z")
    n = -26;
  return n;
};



//
void SgVlbiObservable::resetAllEditings()
{
  sbDelay_.resetAllEditings();
  grDelay_.resetAllEditings();
  phDelay_.resetAllEditings();
  phDRate_.resetAllEditings();
  if (sbDelay_.getSigma() < 5.0e-16)
  {
    owner()->addAttr(SgVlbiObservation::Attr_SBD_NOT_VALID);
    sbDelay_.addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
  };
};



//
bool SgVlbiObservable::saveIntermediateResults(QDataStream& s) const
{
  s << bandKey_ << mediaIdx_ << isUsable_ << nonUsableReason_.getAttributes();
  if (s.status() == QDataStream::Ok &&
      sbDelay_.saveIntermediateResults(s) &&
      grDelay_.saveIntermediateResults(s) &&
      phDelay_.saveIntermediateResults(s) &&
      phDRate_.saveIntermediateResults(s))
    return s.status() == QDataStream::Ok;
  else
    return false;
};



//
bool SgVlbiObservable::loadIntermediateResults(QDataStream& s)
{
  QString                       bandKey;
  int                           mediaIdx;
  bool                          is;
  unsigned int                  attributes;
  s >> bandKey >> mediaIdx >> is >> attributes;
  if (s.status() == QDataStream::Ok)
  {
    if (bandKey_ != bandKey)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error reading " + bandKey_ + 
        "-band data: bandKey mismatch: got [" + bandKey + "], expected [" + bandKey_ + "]");
      return false;
    };
    if (mediaIdx_ != mediaIdx)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error reading " + bandKey_ + 
        "-band data: media index mismatch: got [" + 
        QString("").setNum(mediaIdx) + "], expected [" + QString("").setNum(mediaIdx_) + "]");
      return false;
    };
    isUsable_ = is;
    nonUsableReason_.setAttributes(attributes);
    if (sbDelay_.loadIntermediateResults(s) &&
        grDelay_.loadIntermediateResults(s) &&
        phDelay_.loadIntermediateResults(s) &&
        phDRate_.loadIntermediateResults(s))
      return true;
  };
  logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
    ": loadIntermediateResults(): error reading data: " +
    (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
  return false;
};



//
bool SgVlbiObservable::selfCheck()
{
  bool isOk = true;

  return isOk;
};



//
void SgVlbiObservable::calcPhaseDelay(bool isAmbigResolved)
{
  if (snr_ <= 0.0) // just noise
    return;
  
  if (!owner_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseDelay(): cannot calculate phase delay: the owner is NULL");
    return;
  };
  if (!band_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseDelay(): cannot calculate phase delay: the band is NULL");
    return;
  };
  SgVlbiAuxObservation         *aux_1=owner_->auxObs_1(), *aux_2=owner_->auxObs_2();
  if (!(aux_1 && aux_2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseDelay(): cannot calculate phase delay: the aux obss are NULL");
    return;
  };
  double                        vPhDly, ePhDly, aPhDly, dNumAmbigs;

  vPhDly = (totalPhase_ - (aux_2->getParallacticAngle() - aux_1->getParallacticAngle()))*1.0e-6
                                                                        /(2.0*M_PI*referenceFrequency_);
  if (1.0e-10 < snr_)
    ePhDly = 1.0e-6/(2.0*M_PI*referenceFrequency_*snr_);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
      ": calcPhaseDelay(): got unusual value for SNR: " + QString("").setNum(snr_));
    ePhDly = 1.0e-6/(2.0*M_PI*referenceFrequency_);
  };
  aPhDly = 1.0e-6/referenceFrequency_; // refFreq is in Mhz
  dNumAmbigs = round((grDelay_.getValue() + grDelay_.ambiguity() - vPhDly)/aPhDly);

  phDelay_.setValue(vPhDly + dNumAmbigs*aPhDly);
  phDelay_.setSigma(ePhDly);
  phDelay_.setAmbiguitySpacing(aPhDly);
};



//
void SgVlbiObservable::calcPhaseCalDelay()
{
  if (!band_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: band is NULL");
    return;
  };
  if (!refFreqByChan_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: ref.freq storage is NULL");
    return;
  };
  if (!phaseCalData_1ByChan_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: pcal phases @1 storage is NULL");
    return;
  };
  if (!phaseCalData_2ByChan_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: pcal phases @2 storage is NULL");
    return;
  };
  if (refFreqByChan_->n() != phaseCalData_1ByChan_->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: size mismatch for @1: " +
      QString("").sprintf("#chan(%d) != #PCchan(%d)", 
        refFreqByChan_->n(), phaseCalData_1ByChan_->nCol()));
    return;
  };
  if (refFreqByChan_->n() != phaseCalData_2ByChan_->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": calcPhaseCalDelay(): cannot calculate phase cal effects: size mismatch for @2");
    return;
  };
  int                           n=refFreqByChan_->n();
  SgVector                     *pcPhases_1 = new SgVector(n);
  SgVector                     *pcPhases_2 = new SgVector(n);
  SgVector                     *pcAmplitudes_1 = new SgVector(n);
  SgVector                     *pcAmplitudes_2 = new SgVector(n);

  for (int i=0; i<n; i++)
  {
    pcPhases_1    ->setElement(i, phaseCalData_1ByChan_->getElement(PCCI_PHASE, i) + 
                                  phaseCalData_1ByChan_->getElement(PCCI_OFFSET, i));
    pcPhases_2    ->setElement(i, phaseCalData_2ByChan_->getElement(PCCI_PHASE, i) + 
                                  phaseCalData_2ByChan_->getElement(PCCI_OFFSET, i));
//
//  pcAmplitudes_1->setElement(i, phaseCalData_1ByChan_->getElement(PCCI_AMPLITUDE, i)*1.0e-4);
//  pcAmplitudes_2->setElement(i, phaseCalData_2ByChan_->getElement(PCCI_AMPLITUDE, i)*1.0e-4);
//
    pcAmplitudes_1->setElement(i, phaseCalData_1ByChan_->getElement(PCCI_AMPLITUDE, i));
    pcAmplitudes_2->setElement(i, phaseCalData_2ByChan_->getElement(PCCI_AMPLITUDE, i));
    pcAmplitudes_1->setElement(i, 1.0);
    pcAmplitudes_2->setElement(i, 1.0);
  };

  double                        phaseCalGrd=0.0;
  double                        phaseCalGrdAmbig=0.0;
  double                        phaseCalPhd=0.0;
  double                        phaseCalPhdAmbig=0.0;

  if (evaluatePhaseCals(*pcPhases_1, *pcAmplitudes_1, *refFreqByChan_, referenceFrequency_,
    phaseCalGrd, phaseCalGrdAmbig, phaseCalPhd, phaseCalPhdAmbig, strId()))
  {
    phaseCalGrDelays_[0] = phaseCalGrd*1.0e-9;
    phaseCalPhDelays_[0] = phaseCalPhd*1.0e-9;
    phaseCalGrAmbigSpacings_[0] = phaseCalGrdAmbig*1.0e-9;
    phaseCalPhAmbigSpacings_[0] = phaseCalPhdAmbig*1.0e-9;
  };

  if (evaluatePhaseCals(*pcPhases_2, *pcAmplitudes_2, *refFreqByChan_, referenceFrequency_,
    phaseCalGrd, phaseCalGrdAmbig, phaseCalPhd, phaseCalPhdAmbig, strId()))
  {
    phaseCalGrDelays_[1] = phaseCalGrd*1.0e-9;
    phaseCalPhDelays_[1] = phaseCalPhd*1.0e-9;
    phaseCalGrAmbigSpacings_[1] = phaseCalGrdAmbig*1.0e-9;
    phaseCalPhAmbigSpacings_[1] = phaseCalPhdAmbig*1.0e-9;
  };
  //
  delete pcPhases_1;
  delete pcPhases_2;
  delete pcAmplitudes_1;
  delete pcAmplitudes_2;
};



void SgVlbiObservable::propagateChannelBandwidth(double bw)
{
  if (channelBandwidth_)
  {
    double                      effFreq4GR, effFreq4PH, effFreq4RT;
    for (unsigned int i=0; i<channelBandwidth_->n(); i++)
      channelBandwidth_->setElement(i, bw);
    setSampleRate(bw*2.0e6);
    
    effFreq4GR = effFreq4PH = effFreq4RT = 0.0;
    evaluateEffectiveFreqs(*numOfAccPeriodsByChan_USB_, *numOfAccPeriodsByChan_LSB_,
      *refFreqByChan_, *fringeAmplitudeByChan_,
      *numOfSamplesByChan_USB_, *numOfSamplesByChan_LSB_,
      *channelBandwidth_, referenceFrequency_, 
      channelBandwidth_->n(), effFreq4GR, effFreq4PH, effFreq4RT, 
      owner_->session()->getCorrelatorType(), this);
    grDelay().setEffFreq(effFreq4GR);
    phDelay().setEffFreq(effFreq4PH);
    phDRate().setEffFreq(effFreq4RT);
    //
    if (true)
    {
      effFreq4GR = effFreq4PH = effFreq4RT = 0.0;
      evaluateEffectiveFreqs(*numOfAccPeriodsByChan_USB_, *numOfAccPeriodsByChan_LSB_,
        *refFreqByChan_, *fringeAmplitudeByChan_,
        *numOfSamplesByChan_USB_, *numOfSamplesByChan_LSB_,
        *channelBandwidth_, referenceFrequency_, 
        channelBandwidth_->n(), effFreq4GR, effFreq4PH, effFreq4RT, 
        owner_->session()->getCorrelatorType(), this, false, true);
      setTest (effFreq4GR);
      setTest2(effFreq4GR - grDelay().getEffFreq());
    };
    // calculate the effective frequencies with equal weights:
    effFreq4GR = effFreq4PH = effFreq4RT = 0.0;
    evaluateEffectiveFreqs(*numOfAccPeriodsByChan_USB_, *numOfAccPeriodsByChan_LSB_,
      *refFreqByChan_, *fringeAmplitudeByChan_,
      *numOfSamplesByChan_USB_, *numOfSamplesByChan_LSB_,
      *channelBandwidth_, referenceFrequency_,
      channelBandwidth_->n(), effFreq4GR, effFreq4PH, effFreq4RT, 
      owner_->session()->getCorrelatorType(), this, true);
    grDelay().setEffFreqEqWgt(effFreq4GR);
    phDelay().setEffFreqEqWgt(effFreq4PH);
    phDRate().setEffFreqEqWgt(effFreq4RT);
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": propagateChannelBandwidth(): the vector is NULL");
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
//
bool evaluatePhaseCals(const SgVector& pcPhases, const SgVector& pcAmplitudes,
  const SgVector& pcFreqByChan, double refFreq,
  double& phaseCalGrd, double& phaseCalGrdAmbig, 
  double& phaseCalPhd, double& phaseCalPhdAmbig, 
  const QString& oId)
{
  int                           nChan=pcFreqByChan.n();
  phaseCalGrdAmbig = 0.0;
  phaseCalPhdAmbig = 0.0;
  if (nChan == 0)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      "evaluatePhaseCals: number of actual channels is zero, nothing to do for the obs " + oId);
    return false;
  };
  if (nChan == 1)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      "evaluatePhaseCals: have only one actual channel, nothing to do for the obs " + oId);
    return false;
  };
  //
  std::complex<double>         *x_a, *x_A;
  double                        rfMin, diffMin, d;
  int                          *idxs=new int[nChan], nData=512;
  double                        dD, dP;
  diffMin = 1.0e12;
  rfMin   = 1.0e15;
  //
  for (int i=0; i<nChan; i++)
  {
    if ( (d=fabs(pcFreqByChan.getElement(i))) < rfMin)
      rfMin = d;
    for (int j=i+1; j<nChan; j++)
    {
      d = fabs(pcFreqByChan.getElement(j) - pcFreqByChan.getElement(i));
      if (0.0 < d && d < diffMin)
        diffMin = d;
    };
  };
  phaseCalGrdAmbig = 1.0/diffMin*1.0e3;
  phaseCalPhdAmbig = 1.0/refFreq*1.0e3;
  //
  for (int i=0; i<nChan; i++)
    if (nData <= (idxs[i]=((fabs(pcFreqByChan.getElement(i)) - rfMin)/diffMin)))
    {
      logger->write(SgLogger::INF, SgLogger::PREPROC,
        "evaluatePCal4GrpDelay: Array overrun for the obs " + oId +
        QString("").sprintf(": idxs[%d]=%d for %.2f and Fmin=%.2f Dmin=%.2f",
        i, idxs[i], pcFreqByChan.getElement(i), rfMin, diffMin));
      delete[] idxs;
      return false;
    };
  //
  x_a   = new std::complex<double>[nData];
  x_A   = new std::complex<double>[nData];
  for (int i=0; i<nData; i++)
    x_a[i] = x_A[i] = std::complex<double>(0.0, 0.0);
  for (int i=0; i<nChan; i++)
    x_a[idxs[i]] = std::complex<double>(pcAmplitudes.getElement(i)*cos(pcPhases.getElement(i)),
                                        pcAmplitudes.getElement(i)*sin(pcPhases.getElement(i)));
  //
  // perform FFT:
  fft(x_a, x_A, nData, FFT_Inverse);
  //
  findPeak(x_A, nData, dD, dP);
  //
  phaseCalGrd = phaseCalGrdAmbig*dD/nData;
  phaseCalGrd = fmod(phaseCalGrd + phaseCalGrdAmbig*2.5, phaseCalGrdAmbig) - 0.5*phaseCalGrdAmbig;
  //
  phaseCalPhd = dP/(2.0*M_PI)*phaseCalPhdAmbig + (refFreq - rfMin)*1.0e-3*phaseCalGrd;
  //
  delete[] idxs;
  delete[] x_a;
  delete[] x_A;
  return true;
};



//
void findPeak(const std::complex<double> *cA, int n, double &dD, double &dP)
{
  double                        dX1, dX2, dX3, dA1, dA2, dA3, dP1, dP2, dP3;
  double                        d, dx;
  int                           idx1, idx2, idx3;
  
  idx1 = idx2 = idx3 = 0;
  dX1 = dX2 = dX3 = dA1 = dA3 = dP1 = dP2 = dP3 = 0.0;
  dA2 = -1.0;
  for (int i=0; i<n; i++)
    if (dA2 <= (d=abs(cA[i])) )
    {
      dA2 = d;
      idx1 = idx2 = idx3 = i;
    };
  dX1 = idx2 - 1;
  dX2 = idx2;
  dX3 = idx2 + 1;
  
  if (--idx1 < 0)
    idx1 += n;
  if (n <= ++idx3)
    idx3 -= n;
  // get amplitudes and phases:
  dA1 = abs(cA[idx1]);
  dA3 = abs(cA[idx3]);
  dP1 = arg(cA[idx1]);
  dP2 = arg(cA[idx2]);
  dP3 = arg(cA[idx3]);
  // adjust the phases if necessary:
  if (M_PI <= fabs(dP1 - dP2))
    dP1 = dP2 + fmod(dP1 - dP2 + 5.0*M_PI, 2.0*M_PI) - M_PI;
  if (M_PI <= fabs(dP2 - dP3))
    dP3 = dP2 + fmod(dP3 - dP2 + 5.0*M_PI, 2.0*M_PI) - M_PI;
  //
  if (false)      // general expression for maximum:
  {
    dD =  0.5*(dX3*dX3*(dA1 - dA2) + dX2*dX2*(dA3 - dA1) + dX1*dX1*(dA2 - dA3))/
              (dX3*    (dA1 - dA2) + dX2*    (dA3 - dA1) + dX1*    (dA2 - dA3));
    dx = dD - dX2;
  }
  else            // simplified, assuming x1=-1, x2=0, x3=1:
  {
    dx = 0.5*(dA1 - dA3)/(dA1 - 2.0*dA2 + dA3);
    dD = dX2 + dx;
  };
  //
  // it is assumed x1=-1, x2=0, x3=1:
  dP = dP2 + 0.5*(dP3 - dP1)*dx + 0.5*(dP1 - 2.0*dP2 + dP3)*dx*dx;
  return;
};
/*=====================================================================================================*/
//
// statics:
//
/*=====================================================================================================*/




/*=====================================================================================================*/
