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



#include <math.h>

#include <SgConstants.h>
#include <SgLogger.h>
#include <SgNetCdf.h>
#include <SgVersion.h>
#include <SgVgosDb.h>








/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- loadObs part of vgosDb data tree)                              */
/*                                                                                                     */
/*=====================================================================================================*/
//
bool SgVgosDb::loadEpochs4Obs(QList<SgMJD>& epochs)
{
  if (vObservationTimeUTC_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Obs(): the vgosDb variable ObservationTimeUTC is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vObservationTimeUTC_.getFileName());
  ncdf.getData();
  //
  if (!checkFormat(fcfTimeUTC, ncdf, true))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Obs(): format check failed");
    return false;
  };
  //
  const double                 *pSeconds=ncdf.lookupVar(fcSecond.name())->data2double();
  const short                  *pYMDHM  =ncdf.lookupVar(fcYmdhm .name())->data2short();
  epochs.clear();
  numOfObs_ = ncdf.lookupVar(fcSecond.name())->dimensions().at(0)->getN();
  for (int i=0; i<numOfObs_; i++)
    epochs.append( SgMJD(pYMDHM[5*i  ], pYMDHM[5*i+1], pYMDHM[5*i+2], pYMDHM[5*i+3], 
                                        pYMDHM[5*i+4], pSeconds[i]));
  if (epochs.size())
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Obs(): read " + QString("").setNum(epochs.size()) + 
      " observation epochs from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsObjectNames(QList<QString>& stations_1, QList<QString>& stations_2, 
                                  QList<QString>& sources)
{
  if (vBaseline_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): the vgosDb variable Baseline is empty");
    return false;
  };
  if (vSource_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): the vgosDb variable Source is empty");
    return false;
  };
  //
  // stations:
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vBaseline_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfBaseline, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): baseline format check failed");
    return false;
  };
  const char                   *p=ncdf.lookupVar(fcBaseline.name())->data2char();
  int                           strLength=fcBaseline.dims().last();
  stations_1.clear();
  stations_2.clear();
  for (int i=0; i<numOfObs_; i++)
  {
    stations_1.append(QString::fromLatin1(p + strLength* 2*i   , strLength));
    stations_2.append(QString::fromLatin1(p + strLength*(2*i+1), strLength));
  };
  if (stations_1.size())
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): read " + QString("").setNum(stations_1.size()) + 
      " station names from " + ncdf.getFileName());
  //
  // sources:
  //
  ncdf.reset();
  ncdf.setFileName(path2RootDir_ + "/" + vSource_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfSource, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): source format check failed");
    return false;
  };
  p = ncdf.lookupVar(fcSource.name())->data2char();
  strLength = fcSource.dims().last();
  sources.clear();
  for (int i=0; i<numOfObs_; i++)
    sources.append(QString::fromLatin1(p + strLength*i, strLength));
  if (sources.size())
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadObsObjectNames(): read " + QString("").setNum(sources.size()) + 
      " source names from " + ncdf.getFileName());
  //
  return true;
};



//
bool SgVgosDb::loadObsCrossRefs(QVector<int>& obs2Scan)
{
  if (vObsCrossRef_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCrossRefs(): the vgosDb variable ObsCrossRef is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vObsCrossRef_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfObsCrossRef, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCrossRefs(): format check failed");
    return false;
  };
  const int                    *p=ncdf.lookupVar(fcObs2Scan.name())->data2int();
  obs2Scan.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    obs2Scan[i] = p[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCrossRefs(): read " + QString("").setNum(numOfObs_) + 
    " obs2scan references from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsSingleBandDelays(const QString &band, SgMatrix* &singleBandDelays)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsSingleBandDelays(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vSBDelay_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsSingleBandDelays(): the vgosDb variable SBDelay for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfSBDelay, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsSingleBandDelays(): format check failed");
    return false;
  };
  const double                 *pVal=ncdf.lookupVar(fcSBDelay.name())->data2double();
  const double                 *pSig=NULL;
  if (ncdf.lookupVar(fcSBDelaySig.name()))
    pSig=ncdf.lookupVar(fcSBDelaySig.name())->data2double();
  singleBandDelays = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    singleBandDelays->setElement(i,0,  pVal[i]);
    if (pSig)
      singleBandDelays->setElement(i,1,  pSig[i]);
    else
      singleBandDelays->setElement(i,1,  1.0e-9);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsSingleBandDelays(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsGroupDelays(const QString& band, SgMatrix*& groupDelays)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsGroupDelays(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vGroupDelay_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsGroupDelays(): the vgosDb variable GroupDelay for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfGroupDelay, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsGroupDelays(): format check failed");
    return false;
  };
  const double                 *pVal=ncdf.lookupVar(fcGroupDelay   .name())->data2double();
  const double                 *pSig=ncdf.lookupVar(fcGroupDelaySig.name())->data2double();
  groupDelays = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    groupDelays->setElement(i,0,  pVal[i]);
    groupDelays->setElement(i,1,  pSig[i]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsGroupDelays(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsRates(const QString& band, SgMatrix*& rates)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsRates(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vGroupRate_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsRates(): the vgosDb variable GroupRate for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfGroupRate, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsRates(): format check failed");
    return false;
  };
  const double                 *pVal=ncdf.lookupVar(fcGroupRate   .name())->data2double();
  const double                 *pSig=ncdf.lookupVar(fcGroupRateSig.name())->data2double();
  rates = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    rates->setElement(i,0,  pVal[i]);
    rates->setElement(i,1,  pSig[i]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsRates(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPhase(const QString& band, SgMatrix*& phases)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsPhase(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhase_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhase(): the vgosDb variable Phase for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPhase, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhase(): format check failed");
    return false;
  };
  const double                 *pVal=ncdf.lookupVar(fcPhase   .name())->data2double();
  const double                 *pSig=ncdf.lookupVar(fcPhaseSig.name())?
    ncdf.lookupVar(fcPhaseSig.name())->data2double():NULL;
  phases = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    phases->setElement(i,0,  pVal[i]);
    phases->setElement(i,1,  pSig?pSig[i]:0.0);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPhase(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsRefFreqs(const QString& band, SgVector*& rf)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsRefFreqs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vRefFreq_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsRefFreqs(): the vgosDb variable RefFreq for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfRefFreq, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsRefFreqs(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcRefFreq.name())->data2double();
  rf = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    rf->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsRefFreqs(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsEffFreqs(const QString& band, SgMatrix*& freqs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsEffFreqs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vEffFreq_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsEffFreqs(): the vgosDb variable EffFreq for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfEffFreq, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsEffFreqs(): format check failed");
    return false;
  };
  const double                 *pG=ncdf.lookupVar(fcFreqGroupIon.name())?
                                  ncdf.lookupVar(fcFreqGroupIon.name())->data2double():NULL;
  const double                 *pP=ncdf.lookupVar(fcFreqPhaseIon.name())?
                                  ncdf.lookupVar(fcFreqPhaseIon.name())->data2double():NULL;
  const double                 *pR=ncdf.lookupVar(fcFreqRateIon .name())?
                                  ncdf.lookupVar(fcFreqRateIon .name())->data2double():NULL;
  if (!(pG || pP || pR))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsEffFreqs(): nothing to read");
    return false;
  };
  freqs = new SgMatrix(numOfObs_, 3);
  for (int i=0; i<numOfObs_; i++)
  {
    freqs->setElement(i,0,  pG?pG[i]:0.0);
    freqs->setElement(i,1,  pP?pP[i]:0.0);
    freqs->setElement(i,2,  pR?pR[i]:0.0);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsEffFreqs(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsAmbigSpacing(const QString& band, SgVector*& ambigs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsAmbigSpacing(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vAmbigSize_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsAmbigSpacing(): the vgosDb variable AmbigSize for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfAmbigSize, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsAmbigSpacing(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcAmbigSize.name())->data2double();
  ambigs = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    ambigs->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsAmbigSpacing(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsQualityCodes(const QString& band, QVector<QString>& qualityCodes)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsQualityCodes(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vQualityCode_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsQualityCodes(): the vgosDb variable QualityCode for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfQualityCode, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsQualityCodes(): format check failed");
    return false;
  };
  const char                 *p=ncdf.lookupVar(fcQualityCode.name())->data2char();
  qualityCodes.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    qualityCodes[i] = QString::fromLatin1(p + i, 1);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsQualityCodes(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCorrelation(const QString &band, SgVector* &correlations)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelation(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrelation_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelation(): the vgosDb variable Correlation for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrelation, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelation(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcCorrelation.name())->data2double();
  correlations = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    correlations->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelation(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsUVFperAsec(const QString &band, SgMatrix* &uvfPerAsec)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsUVFperAsec(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vUVFperAsec_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsUVFperAsec(): the vgosDb variable UVFperAsec for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfUVFperAsec, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsUVFperAsec(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcUVFperAsec.name())->data2double();
  uvfPerAsec = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    uvfPerAsec->setElement(i,0,  p[2*i    ]);
    uvfPerAsec->setElement(i,1,  p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsUVFperAsec(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsSNR(const QString &band, SgVector* &snrs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsSNR(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vSNR_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsSNR(): the vgosDb variable SNR for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfSNR, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsSNR(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcSNR.name())->data2double();
  snrs = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    snrs->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsSNR(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsDtec(SgVector* &dTec, SgVector* &dTecStdDev)
{
  SgVdbVariable                &var=vDiffTec_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsDtec(): the vgosDb variable DiffTec is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfDTec, ncdf))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::loadObsDtec(): format check failed");
    return false;
  };
  const double                 *d=ncdf.lookupVar(fcDtec       .name())->data2double();
  const double                 *e=ncdf.lookupVar(fcDtecStdErr .name())->data2double();
  dTec        = new SgVector(numOfObs_);
  dTecStdDev  = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    dTec      ->setElement(i, d[i]);
    dTecStdDev->setElement(i, e[i]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsDtec(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsDelayDataFlag(const QString &band, QVector<int>& delUFlag)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsDelayDataFlag(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vDelayDataFlag_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsDelayDataFlag(): the vgosDb variable DelayDataFlag for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfDelayDataFlag, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsDelayDataFlag(): format check failed");
    return false;
  };
  const short                 *p=ncdf.lookupVar(fcDelayDataFlag.name())->data2short();
  delUFlag.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    delUFlag[i] = p[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsDelayDataFlag(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsDataFlag(const QString &band, QVector<int>& delUFlag)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsDataFlag(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vDataFlag_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsDataFlag(): the vgosDb variable DataFlag for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfDataFlag, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsDataFlag(): format check failed");
    return false;
  };
  const short                  *p=ncdf.lookupVar(fcDataFlag.name())?
                                  ncdf.lookupVar(fcDataFlag.name())->data2short():NULL;
  if (!p)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsDataFlag(): nothing to read for " + band + "-band");
    return false;
  };
  delUFlag.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    delUFlag[i] = p[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsDataFlag(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsTheoreticals(SgVector*& delays, SgVector*& rates)
{
  // delays:
  if (vDelayTheoretical_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsTheoreticals(): the vgosDb variable DelayTheoretical is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vDelayTheoretical_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfDelayTheoretical, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsTheoreticals(): DelayTheoretical format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcDelayTheoretical.name())->data2double();
  delays = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    delays->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsTheoreticals(): data loaded successfully from " + ncdf.getFileName());
  //
  // and rates:
  if (vRateTheoretical_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsTheoreticals(): the vgosDb variable RateTheoretical is empty");
    return false;
  };
  ncdf.reset();
  ncdf.setFileName(path2RootDir_ + "/" + vRateTheoretical_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfRateTheoretical, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsTheoreticals(): RateTheoretical format check failed");
    return false;
  };
  p = ncdf.lookupVar(fcRateTheoretical.name())->data2double();
  rates = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    rates->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsTheoreticals(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsChannelInfo(const QString& band,    // SgVlbiSessionInfo::OriginType originType,
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
  )
{
  bool                          isOk=false;
  //
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfo(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vChannelInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfo(): the vgosDb variable ChannelInfo for the band [" + band + "] is empty");
    return false;
  };
  // clear the containers:
  channelIds.clear();
  polarizations.clear();
  numOfChannels.clear();
  bitsPerSamples.clear();
  
  if (errorRates_1.size())
    for (int i=0; i<errorRates_1.size(); i++)
      errorRates_1[i].clear();
  errorRates_1.clear();
  if (errorRates_2.size())
    for (int i=0; i<errorRates_2.size(); i++)
      errorRates_2[i].clear();
  errorRates_2.clear();

  if (bbcIdxs_1.size())
    for (int i=0; i<bbcIdxs_1.size(); i++)
      bbcIdxs_1[i].clear();
  bbcIdxs_1.clear();
  if (bbcIdxs_2.size())
    for (int i=0; i<bbcIdxs_2.size(); i++)
      bbcIdxs_2[i].clear();
  bbcIdxs_2.clear();

  if (corelIdxNumbersUsb.size())
    for (int i=0; i<corelIdxNumbersUsb.size(); i++)
      corelIdxNumbersUsb[i].clear();
  corelIdxNumbersUsb.clear();
  if (corelIdxNumbersLsb.size())
    for (int i=0; i<corelIdxNumbersLsb.size(); i++)
      corelIdxNumbersLsb[i].clear();
  corelIdxNumbersLsb.clear();
    
  sampleRate = NULL;
  residFringeAmplByChan = NULL;
  residFringePhseByChan = NULL;
  refFreqByChan = NULL;
  numOfSamplesByChan_USB = NULL;
  numOfSamplesByChan_LSB = NULL;
  numOfAccPeriodsByChan_USB = NULL;
  numOfAccPeriodsByChan_LSB = NULL;
  loFreqs_1 = NULL;
  loFreqs_2 = NULL;
  
  sbOrder = sbOrder_;

  if (correlatorType_ == CT_Mk3     || 
      correlatorType_ == CT_VLBA    ||
      correlatorType_ == CT_CRL     ||
      correlatorType_ == CT_GSI      )
    isOk = loadObsChannelInfoMk3(band, channelIds, polarizations, numOfChannels, 
      bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
      corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
      refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
      numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
      sbOrder);
  else if (correlatorType_ == CT_S2 )
    isOk = loadObsChannelInfoS2(band, channelIds, polarizations, numOfChannels, 
      bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
      corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
      refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
      numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
      sbOrder);
  else if ( correlatorType_ == CT_Mk4     ||
            correlatorType_ == CT_Haystack||
            correlatorType_ == CT_Difx     )
    isOk = loadObsChannelInfoMk4(band, channelIds, polarizations, numOfChannels, 
      bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
      corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
      refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
      numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
      sbOrder);
  else
  {
    // Unknown, try each of types:
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfo(): unknown correlator type, trying Mk4 data format");
    isOk = loadObsChannelInfoMk4(band, channelIds, polarizations, numOfChannels, 
      bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
      corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
      refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
      numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
      sbOrder);
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsChannelInfo(): unknown correlator type, trying S2 data format");
      isOk = loadObsChannelInfoS2(band, channelIds, polarizations, numOfChannels, 
        bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
        corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
        refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
        numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
        sbOrder);
    };
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsChannelInfo(): unknown correlator type, trying Mk3 data format");
      isOk = loadObsChannelInfoMk3(band, channelIds, polarizations, numOfChannels, 
        bitsPerSamples, errorRates_1, errorRates_2, bbcIdxs_1, bbcIdxs_2, corelIdxNumbersUsb, 
        corelIdxNumbersLsb, sampleRate, residFringeAmplByChan, residFringePhseByChan, 
        refFreqByChan, numOfSamplesByChan_USB, numOfSamplesByChan_LSB, 
        numOfAccPeriodsByChan_USB, numOfAccPeriodsByChan_LSB, loFreqs_1, loFreqs_2, channelBandwidth,
        sbOrder);
    };
  };
  return isOk;
};



//
bool SgVgosDb::loadObsChannelInfoMk3(const QString& band,
    QVector<QString>& /*channelIds*/,                     // CHAN ID
    QVector<QString>& /*polarizations*/,                  // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& /*bitsPerSamples*/,                     // BITSAMPL
    QVector< QVector<int> >& /*errorRates_1*/,            // ERRORATE @1, phasecal data
    QVector< QVector<int> >& /*errorRates_2*/,            // ERRORATE @2, phasecal data
    QVector< QVector<int> >& /*bbcIdxs_1*/,               // BBC IND @1
    QVector< QVector<int> >& /*bbcIdxs_2*/,               // BBC IND @2
    QVector< QVector<int> >& /*corelIdxNumbersUsb*/,      // INDEXNUM usb
    QVector< QVector<int> >& /*corelIdxNumbersLsb*/,      // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&/*loFreqs_1*/,                              // LO FREQ @1
    SgMatrix*&/*loFreqs_2*/,                              // LO FREQ @2
    SgMatrix*&/*channelBandwidth*/,
    SgSidebandOrder &sbOrder
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk3(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vChannelInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk3(): the vgosDb variable ChannelInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfChannelInfoInptMk3, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk3(): format check failed");
    return false;
  };

  const short                  *pNumChannels=ncdf.lookupVar(fcNumChannels.name())->data2short();
  const double                 *pSampleRate=ncdf.lookupVar(fcSampleRate.name())->data2double();
  const double                 *pChannelFreq=ncdf.lookupVar(fcChannelFreq.name())->data2double();
  const short                  *pNumAp=ncdf.lookupVar(fcNumAp.name())->data2short();
  const double                 *pChanAmpPhase=ncdf.lookupVar(fcChanAmpPhase.name())->data2double();
  const short                  *pSidebandOrder=ncdf.lookupVar(fcSidebandOrder.name())?
                                  ncdf.lookupVar(fcSidebandOrder.name())->data2short():NULL;
  
  int                           usbOffset, lsbOffset;
  if (pSidebandOrder)
  {
    sbOrder = (SgSidebandOrder)*pSidebandOrder;
    hasSidebandOrder_ = true;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk3(): the sideband order is unavailable, it was set to the default, {" + 
      sidebandOrderByType[sbOrder] + "}");
  switch(sbOrder)
  {
    case SO_LSB_USB:
      usbOffset = 1;
      lsbOffset = 0;
      break;
    case SO_USB_LSB:
    default:
      usbOffset = 0;
      lsbOffset = 1;
      break;
  };
  
  
  //
  numOfChan_ = ncdf.lookupVar(fcNumAp.name())->dimensions().at(1)->getN();
  //
  numOfChannels.resize(numOfObs_);
  numOfAccPeriodsByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
  numOfAccPeriodsByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  sampleRate = new SgVector(numOfObs_);
  refFreqByChan = new SgMatrix(numOfObs_, numOfChan_);
  residFringeAmplByChan = new SgMatrix(numOfObs_, numOfChan_);
  residFringePhseByChan = new SgMatrix(numOfObs_, numOfChan_);
  numOfSamplesByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
  numOfSamplesByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  for (int i=0; i<numOfObs_; i++)
  {
    numOfChannels[i] = pNumChannels[i];
    sampleRate->setElement(i, pSampleRate[i]);
    for (int j=0; j<numOfChan_; j++)
    {
      numOfAccPeriodsByChan_USB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + usbOffset]);
      numOfAccPeriodsByChan_LSB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + lsbOffset]);
      refFreqByChan->setElement(i,j, pChannelFreq[numOfChan_*i + j]);
      residFringeAmplByChan->setElement(i,j, pChanAmpPhase[numOfChan_*2*i + 2*j + 0]);
      residFringePhseByChan->setElement(i,j, pChanAmpPhase[numOfChan_*2*i + 2*j + 1]);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    ":loadObsChannelInfoMk3(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsChannelInfoS2(const QString& band,
    QVector<QString>& /*channelIds*/,                     // CHAN ID
    QVector<QString>& /*polarizations*/,                  // POLARIZ
    QVector<int>& numOfChannels,                          // #CHANELS
    QVector<int>& /*bitsPerSamples*/,                     // BITSAMPL
    QVector< QVector<int> >& /*errorRates_1*/,            // ERRORATE @1, phasecal data
    QVector< QVector<int> >& /*errorRates_2*/,            // ERRORATE @2, phasecal data
    QVector< QVector<int> >& /*bbcIdxs_1*/,               // BBC IND @1
    QVector< QVector<int> >& /*bbcIdxs_2*/,               // BBC IND @2
    QVector< QVector<int> >& /*corelIdxNumbersUsb*/,      // INDEXNUM usb
    QVector< QVector<int> >& /*corelIdxNumbersLsb*/,      // INDEXNUM lsb
    SgVector*&sampleRate,                                 // SAMPLRAT
    SgMatrix*&residFringeAmplByChan,                      // AMPBYFRQ Amp
    SgMatrix*&residFringePhseByChan,                      // AMPBYFRQ Phs
    SgMatrix*&refFreqByChan,                              // RFREQ
    SgMatrix*&numOfSamplesByChan_USB,                     // #SAMPLES usb
    SgMatrix*&numOfSamplesByChan_LSB,                     // #SAMPLES lsb
    SgMatrix*&numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
    SgMatrix*&numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
    SgMatrix*&/*loFreqs_1*/,                              // LO FREQ @1
    SgMatrix*&/*loFreqs_2*/,                              // LO FREQ @2
    SgMatrix*&/*channelBandwidth*/,
    SgSidebandOrder &sbOrder
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoS2(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vChannelInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoS2(): the vgosDb variable ChannelInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfChannelInfoInptS2, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoS2(): format check failed");
    return false;
  };

  // S2 miss some of them:
  const short                  *pNumChannels=ncdf.lookupVar(fcNumChannels.name())?
                                  ncdf.lookupVar(fcNumChannels.name())->data2short():NULL;
  const double                 *pSampleRate=ncdf.lookupVar(fcSampleRate.name())?
                                  ncdf.lookupVar(fcSampleRate.name())->data2double():NULL;
  const double                 *pChannelFreq=ncdf.lookupVar(fcChannelFreq.name())?
                                  ncdf.lookupVar(fcChannelFreq.name())->data2double():NULL;
  const double                 *pVIRTFREQ=ncdf.lookupVar(fcVIRTFREQ.name())?
                                  ncdf.lookupVar(fcVIRTFREQ.name())->data2double():NULL;
  const short                  *pNumAp=ncdf.lookupVar(fcNumAp.name())?
                                  ncdf.lookupVar(fcNumAp.name())->data2short():NULL;
  const short                  *pNumAccum=ncdf.lookupVar(fcNumAccum.name())?
                                  ncdf.lookupVar(fcNumAccum.name())->data2short():NULL;
  const double                 *pChanAmp=ncdf.lookupVar(fcVFRQAM.name())?
                                  ncdf.lookupVar(fcVFRQAM.name())->data2double():NULL;
  const double                 *pChanPhs=ncdf.lookupVar(fcVFRQPH.name())?
                                  ncdf.lookupVar(fcVFRQPH.name())->data2double():NULL;

  const short                  *pSidebandOrder=ncdf.lookupVar(fcSidebandOrder.name())?
                                  ncdf.lookupVar(fcSidebandOrder.name())->data2short():NULL;

  int                           usbOffset, lsbOffset;
/*
  if (pSidebandOrder)
    sbOrder = (SgSidebandOrder)*pSidebandOrder;
  switch(sbOrder)
  {
    case SO_LSB_USB:
      usbOffset = 1;
      lsbOffset = 0;
      break;
    case SO_USB_LSB:
    default:
      usbOffset = 0;
      lsbOffset = 1;
      break;
  };
*/

  if (pSidebandOrder)
  {
    sbOrder = (SgSidebandOrder)*pSidebandOrder;
    hasSidebandOrder_ = true;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoS2(): the sideband order is unavailable, it was set to the default, {" + 
      sidebandOrderByType[sbOrder] + "}");
  switch(sbOrder)
  {
    case SO_LSB_USB:
      usbOffset = 1;
      lsbOffset = 0;
      break;
    case SO_USB_LSB:
    default:
      usbOffset = 0;
      lsbOffset = 1;
      break;
  };

  // S2 specific:
  if (pVIRTFREQ)
    numOfChan_ = ncdf.lookupVar(fcVIRTFREQ.name())->dimensions().at(1)->getN();
  else if (pChannelFreq)
    numOfChan_ = ncdf.lookupVar(fcChannelFreq.name())->dimensions().at(1)->getN();
  //
  if (pNumChannels)
    numOfChannels.resize(numOfObs_);
  if (pNumAp || pNumAccum)
  {
    numOfAccPeriodsByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
    numOfAccPeriodsByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  };
  if (pSampleRate)
    sampleRate = new SgVector(numOfObs_);
  if (pChannelFreq)
    refFreqByChan = new SgMatrix(numOfObs_, numOfChan_);
  if (pChanAmp)
    residFringeAmplByChan = new SgMatrix(numOfObs_, numOfChan_);
  if (pChanPhs)
    residFringePhseByChan = new SgMatrix(numOfObs_, numOfChan_);
  if (false) //??
  {
    numOfSamplesByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
    numOfSamplesByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  };
  
  for (int i=0; i<numOfObs_; i++)
  {
    if (pNumChannels)
      numOfChannels[i] = pNumChannels[i];
    if (pSampleRate)
      sampleRate->setElement(i, pSampleRate[i]);
    for (int j=0; j<numOfChan_; j++)
    {
      if (pNumAp)
      {
        numOfAccPeriodsByChan_USB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + usbOffset]);
        numOfAccPeriodsByChan_LSB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + lsbOffset]);
      }
      else if (pNumAccum) //?? only one side band?
      {
        numOfAccPeriodsByChan_USB->setElement(i,j, pNumAccum[numOfChan_*i + j]);
        numOfAccPeriodsByChan_LSB->setElement(i,j, pNumAccum[numOfChan_*i + j]);        
      };
      if (pChannelFreq)
        refFreqByChan->setElement(i,j, pChannelFreq[numOfChan_*i + j]);
      if (pChanAmp)
        residFringeAmplByChan->setElement(i,j, pChanAmp[numOfChan_*i + j]);
      if (pChanPhs)
        residFringePhseByChan->setElement(i,j, pChanPhs[numOfChan_*i + j]);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    ":loadObsChannelInfoS2(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsChannelInfoMk4(const QString& band,
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
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk4(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vChannelInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk4(): the vgosDb variable ChannelInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfChannelInfoInptMk4, ncdf, true))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk4(): format check failed");
    // return false;
  };

  const char                   *pChannelID=ncdf.lookupVar(fcChannelID.name())?
                                  ncdf.lookupVar(fcChannelID.name())->data2char():NULL;
  const char                   *pPolarization=ncdf.lookupVar(fcPolarization.name())?
                                  ncdf.lookupVar(fcPolarization.name())->data2char():NULL;
  const short                  *pNumChannels=ncdf.lookupVar(fcNumChannels.name())->data2short();

  const short                  *pBITSAMPL=ncdf.lookupVar(fcBITSAMPL.name())?
                                  ncdf.lookupVar(fcBITSAMPL.name())->data2short():NULL;
  const short                  *pERRORATE=ncdf.lookupVar(fcERRORATE.name())?
                                  ncdf.lookupVar(fcERRORATE.name())->data2short():NULL;
  const short                  *pBBCIndex=ncdf.lookupVar(fcBBCIndex.name())?
                                  ncdf.lookupVar(fcBBCIndex.name())->data2short():NULL;
  const short                  *pINDEXNUM=ncdf.lookupVar(fcINDEXNUM.name())?
                                  ncdf.lookupVar(fcINDEXNUM.name())->data2short():NULL;

  const double                 *pSampleRate=ncdf.lookupVar(fcSampleRate.name())->data2double();
  const double                 *pChannelFreq=ncdf.lookupVar(fcChannelFreq.name())->data2double();
  const short                  *pNumAp=ncdf.lookupVar(fcNumAp.name())->data2short();
  const double                 *pChanAmpPhase=ncdf.lookupVar(fcChanAmpPhase.name())->data2double();
  const double                 *pNumSamples=ncdf.lookupVar(fcNumSamples.name())?
                                  ncdf.lookupVar(fcNumSamples.name())->data2double():NULL;
  
  const double                 *pLOFreq=ncdf.lookupVar(fcLOFreq.name())?
                                  ncdf.lookupVar(fcLOFreq.name())->data2double():NULL;
  const double                 *pChannelBandwidth=ncdf.lookupVar(fcChannelBandwidth.name())?
                                  ncdf.lookupVar(fcChannelBandwidth.name())->data2double():NULL;

  const short                  *pSidebandOrder=ncdf.lookupVar(fcSidebandOrder.name())?
                                  ncdf.lookupVar(fcSidebandOrder.name())->data2short():NULL;

  int                           usbOffset, lsbOffset;
  int                           lChannelID, lPolarization;
/*
  if (pSidebandOrder)
    sbOrder = (SgSidebandOrder)*pSidebandOrder;
  switch(sbOrder)
  {
    case SO_LSB_USB:
      usbOffset = 1;
      lsbOffset = 0;
      break;
    case SO_USB_LSB:
    default:
      usbOffset = 0;
      lsbOffset = 1;
      break;
  };
*/
  if (pSidebandOrder)
  {
    sbOrder = (SgSidebandOrder)*pSidebandOrder;
    hasSidebandOrder_ = true;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsChannelInfoMk4(): the sideband order is unavailable, it was set to the default, {" + 
      sidebandOrderByType[sbOrder] + "}");
  switch(sbOrder)
  {
    case SO_LSB_USB:
      usbOffset = 1;
      lsbOffset = 0;
      break;
    case SO_USB_LSB:
    default:
      usbOffset = 0;
      lsbOffset = 1;
      break;
  };
  //
  numOfChan_ = ncdf.lookupVar(fcNumAp.name())->dimensions().at(1)->getN();
  //
  if (pChannelID)
    channelIds.resize(numOfObs_);
  if (pPolarization)
    polarizations.resize(numOfObs_);
  numOfChannels.resize(numOfObs_);
  if (pBITSAMPL)
    bitsPerSamples.resize(numOfObs_);
  //
  if (pERRORATE)
  {
    errorRates_1.resize(numOfObs_);
    errorRates_2.resize(numOfObs_);
  };
  bbcIdxs_1.resize(numOfObs_);
  bbcIdxs_2.resize(numOfObs_);
  corelIdxNumbersUsb.resize(numOfObs_);
  corelIdxNumbersLsb.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    if (pERRORATE)
    {
      errorRates_1[i].resize(numOfChan_);
      errorRates_2[i].resize(numOfChan_);
    };
    bbcIdxs_1[i].resize(numOfChan_);
    bbcIdxs_2[i].resize(numOfChan_);
    corelIdxNumbersUsb[i].resize(numOfChan_);
    corelIdxNumbersLsb[i].resize(numOfChan_);
  };

  //
  numOfAccPeriodsByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
  numOfAccPeriodsByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  sampleRate = new SgVector(numOfObs_);
  refFreqByChan = new SgMatrix(numOfObs_, numOfChan_);
  if (pNumSamples)
  {
    numOfSamplesByChan_USB = new SgMatrix(numOfObs_, numOfChan_);
    numOfSamplesByChan_LSB = new SgMatrix(numOfObs_, numOfChan_);
  };
  if (pChanAmpPhase)
  {
    residFringeAmplByChan = new SgMatrix(numOfObs_, numOfChan_);
    residFringePhseByChan = new SgMatrix(numOfObs_, numOfChan_);
  };
  loFreqs_1 = new SgMatrix(numOfObs_, numOfChan_);
  loFreqs_2 = new SgMatrix(numOfObs_, numOfChan_);
  if (pChannelBandwidth)
    channelBandwidth = new SgMatrix(numOfObs_, numOfChan_);
  //
  //
  //
  lChannelID    = pChannelID?ncdf.lookupVar(fcChannelID.name())->dimensions().at(1)->getN():0;
  lPolarization = pPolarization?
    ncdf.lookupVar(fcPolarization.name())->dimensions().at(1)->getN()*
    ncdf.lookupVar(fcPolarization.name())->dimensions().at(2)->getN():0;

  for (int i=0; i<numOfObs_; i++)
  {
    if (pChannelID)
      channelIds[i] = QString::fromLatin1(pChannelID + i*lChannelID, lChannelID);
    if (pPolarization)
      polarizations[i] = QString::fromLatin1(pPolarization + i*lPolarization, lPolarization);

    numOfChannels[i] = pNumChannels[i];
    if (pBITSAMPL)
      bitsPerSamples[i] = pBITSAMPL[i];
    
    sampleRate->setElement(i, pSampleRate[i]);
    for (int j=0; j<numOfChan_; j++)
    {
      numOfAccPeriodsByChan_USB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + usbOffset]);
      numOfAccPeriodsByChan_LSB->setElement(i,j, pNumAp[2*numOfChan_*i + 2*j + lsbOffset]);
      refFreqByChan->setElement(i,j, pChannelFreq[numOfChan_*i + j]);
      if (pNumSamples)
      {
        numOfSamplesByChan_USB->setElement(i,j, pNumSamples[2*numOfChan_*i + 2*j + usbOffset]);
        numOfSamplesByChan_LSB->setElement(i,j, pNumSamples[2*numOfChan_*i + 2*j + lsbOffset]);
      };
      if (pChanAmpPhase)
      {
        residFringeAmplByChan->setElement(i,j, pChanAmpPhase[numOfChan_*2*i + 2*j + 0]);
        residFringePhseByChan->setElement(i,j, pChanAmpPhase[numOfChan_*2*i + 2*j + 1]);
      };
      if (pERRORATE)
      {
        errorRates_1[i][j] = pERRORATE[4*numOfChan_*i + 4*j    ];
        errorRates_2[i][j] = pERRORATE[4*numOfChan_*i + 4*j + 1];
      };
      //
      if (pBBCIndex)
      {
        bbcIdxs_1[i][j] = pBBCIndex[2*numOfChan_*i + 2*j    ];
        bbcIdxs_2[i][j] = pBBCIndex[2*numOfChan_*i + 2*j + 1];
      }
      else
      {
        bbcIdxs_1[i][j] = 0;
        bbcIdxs_2[i][j] = 0;
      };
      //
      if (pINDEXNUM)
      {
        corelIdxNumbersUsb[i][j] = pINDEXNUM[2*numOfChan_*i + 2*j + usbOffset];
        corelIdxNumbersLsb[i][j] = pINDEXNUM[2*numOfChan_*i + 2*j + lsbOffset];
      }
      else
      {
        corelIdxNumbersUsb[i][j] = 0;
        corelIdxNumbersLsb[i][j] = 0;
      };
      //
      if (pLOFreq)
      {
        loFreqs_1->setElement(i,j, pLOFreq[2*numOfChan_*i + 2*j    ]);
        loFreqs_2->setElement(i,j, pLOFreq[2*numOfChan_*i + 2*j + 1]);
      }
      else
      {
        loFreqs_1->setElement(i,j, 0.0);
        loFreqs_2->setElement(i,j, 0.0);
      };
      if (pChannelBandwidth)
        channelBandwidth->setElement(i,j, pChannelBandwidth[numOfChan_*i + j]);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsChannelInfoMk4(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
//
//
bool SgVgosDb::loadObsPhaseCalInfo(const QString& band,
  SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
  SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
  SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
  SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2
  )
{
  bool                          isOk=false;
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfo(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseCalInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfo(): the vgosDb variable PhaseCalInfo for the band [" + band + "] is empty");
    return false;
  };
  //
  if (correlatorType_ == CT_Mk3   || 
      correlatorType_ == CT_VLBA  ||
      correlatorType_ == CT_CRL   ||
      correlatorType_ == CT_GSI    )
    isOk = loadObsPhaseCalInfoMk3(band, 
      phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
      phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
  else if (correlatorType_ == CT_S2 )
    isOk = loadObsPhaseCalInfoS2(band,
      phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
      phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
  else if ( correlatorType_ == CT_Mk4     ||
            correlatorType_ == CT_Haystack||
            correlatorType_ == CT_Difx      )
    isOk = loadObsPhaseCalInfoMk4(band,
      phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
      phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
  else
  {
    // Unknown, try each of types:
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfo(): unknown correlator type, trying Mk4 data format");
    isOk = loadObsPhaseCalInfoMk4(band, 
      phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
      phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsPhaseCalInfo(): unknown correlator type, trying S2 data format");
      isOk = loadObsPhaseCalInfoS2(band,
        phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
        phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
    };
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsPhaseCalInfo(): unknown correlator type, trying Mk3 data format");
      isOk = loadObsPhaseCalInfoMk3(band,
        phaseCalFreqs_1, phaseCalAmps_1, phaseCalPhases_1, phaseCalOffsets_1, phaseCalRates_1,
        phaseCalFreqs_2, phaseCalAmps_2, phaseCalPhases_2, phaseCalOffsets_2, phaseCalRates_2);
    };
  };
  return isOk;
};



//
bool SgVgosDb::loadObsPhaseCalInfoMk3(const QString& band,
  SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
  SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
  SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
  SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk3(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseCalInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk3(): the vgosDb variable PhaseCalInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPhaseCalInfoMk3, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk3(): format check failed");
    return false;
  };
  const short                  *pPhCalFreqs=ncdf.lookupVar(fcPhaseCalFreq.name())->data2short();
  const short                  *pPhCalAmps =ncdf.lookupVar(fcPhaseCalAmp.name())->data2short();
  const short                  *pPhCalPhass=ncdf.lookupVar(fcPhaseCalPhase.name())->data2short();
  const double                 *pPhCalRates=ncdf.lookupVar(fcPhaseCalRate.name())->data2double();
  //
  phaseCalFreqs_1   = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalAmps_1    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_1  = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalOffsets_1 = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalRates_1   = new SgVector(numOfObs_);
  phaseCalFreqs_2   = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalAmps_2    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_2  = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalOffsets_2 = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalRates_2   = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    phaseCalRates_1->setElement(i,  pPhCalRates[2*i    ]);
    phaseCalRates_2->setElement(i,  pPhCalRates[2*i + 1]);
    for (int j=0; j<numOfChan_; j++)
    {
      phaseCalFreqs_1 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j    ]);
      phaseCalFreqs_2 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j + 1]);
      phaseCalAmps_1  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j    ]);
      phaseCalAmps_2  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j + 1]);
      phaseCalPhases_1->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j    ]);
      phaseCalPhases_2->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j + 1]);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPhaseCalInfoMk3(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPhaseCalInfoS2(const QString& band,
  SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
  SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
  SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
  SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoS2(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseCalInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoS2(): the vgosDb variable PhaseCalInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPhaseCalInfoS2, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoS2(): format check failed");
    return false;
  };
  const double                 *pPhCalFreqs=ncdf.lookupVar(fcPhaseCalFreqS2.name())?
                                  ncdf.lookupVar(fcPhaseCalFreqS2.name())->data2double():NULL;
  const double                 *pPhCalAmps =ncdf.lookupVar(fcPhaseCalAmpS2.name())->data2double();
  const double                 *pPhCalPhass=ncdf.lookupVar(fcPhaseCalPhaseS2.name())->data2double();
  const double                 *pPhCalRates=ncdf.lookupVar(fcPhaseCalRate.name())->data2double();
  //
  if (pPhCalFreqs)
  {
    phaseCalFreqs_1   = new SgMatrix(numOfObs_, numOfChan_);
    phaseCalFreqs_2   = new SgMatrix(numOfObs_, numOfChan_);
  };
  phaseCalAmps_1    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalAmps_2    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_1  = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_2  = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalOffsets_1 = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalOffsets_2 = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalRates_1   = new SgVector(numOfObs_);
  phaseCalRates_2   = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    phaseCalRates_1->setElement(i,  pPhCalRates[2*i    ]);
    phaseCalRates_2->setElement(i,  pPhCalRates[2*i + 1]);
    for (int j=0; j<numOfChan_; j++)
    {
      if (pPhCalFreqs)
      {
        phaseCalFreqs_1 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j    ]);
        phaseCalFreqs_2 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j + 1]);
      };
      phaseCalAmps_1  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j    ]);
      phaseCalAmps_2  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j + 1]);
      phaseCalPhases_1->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j    ]);
      phaseCalPhases_2->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j + 1]);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPhaseCalInfoS2(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPhaseCalInfoMk4(const QString& band,
  SgMatrix*& phaseCalFreqs_1, SgMatrix*& phaseCalAmps_1, SgMatrix*& phaseCalPhases_1, 
  SgMatrix*& phaseCalOffsets_1, SgVector*& phaseCalRates_1,
  SgMatrix*& phaseCalFreqs_2, SgMatrix*& phaseCalAmps_2, SgMatrix*& phaseCalPhases_2, 
  SgMatrix*& phaseCalOffsets_2, SgVector*& phaseCalRates_2
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk4(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseCalInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk4(): the vgosDb variable PhaseCalInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPhaseCalInfoMk4, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPhaseCalInfoMk4(): format check failed");
    return false;
  };
  const short                  *pPhCalFreqs=ncdf.lookupVar(fcPhaseCalFreq.name())?
                                  ncdf.lookupVar(fcPhaseCalFreq.name())->data2short():NULL;
  const short                  *pPhCalAmps =ncdf.lookupVar(fcPhaseCalAmp.name())->data2short();
  const short                  *pPhCalPhass=ncdf.lookupVar(fcPhaseCalPhase.name())->data2short();
  const short                  *pPhCalOffss=ncdf.lookupVar(fcPhaseCalOffset.name())?
                                  ncdf.lookupVar(fcPhaseCalOffset.name())->data2short():NULL;
  const double                 *pPhCalRates=ncdf.lookupVar(fcPhaseCalRate.name())->data2double();
  //
  if (pPhCalFreqs)
  {
    phaseCalFreqs_1   = new SgMatrix(numOfObs_, numOfChan_);
    phaseCalFreqs_2   = new SgMatrix(numOfObs_, numOfChan_);
  };
  phaseCalAmps_1    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalAmps_2    = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_1  = new SgMatrix(numOfObs_, numOfChan_);
  phaseCalPhases_2  = new SgMatrix(numOfObs_, numOfChan_);
  if (pPhCalOffss)
  {
    phaseCalOffsets_1 = new SgMatrix(numOfObs_, numOfChan_);
    phaseCalOffsets_2 = new SgMatrix(numOfObs_, numOfChan_);
  };
  phaseCalRates_1   = new SgVector(numOfObs_);
  phaseCalRates_2   = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    phaseCalRates_1->setElement(i,  pPhCalRates[2*i    ]);
    phaseCalRates_2->setElement(i,  pPhCalRates[2*i + 1]);
    for (int j=0; j<numOfChan_; j++)
    {
      if (pPhCalFreqs)
      {
        phaseCalFreqs_1 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j    ]);
        phaseCalFreqs_2 ->setElement(i,j, pPhCalFreqs[2*numOfChan_*i + 2*j + 1]);
      };
      phaseCalAmps_1  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j    ]);
      phaseCalAmps_2  ->setElement(i,j, pPhCalAmps [2*numOfChan_*i + 2*j + 1]);
      phaseCalPhases_1->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j    ]);
      phaseCalPhases_2->setElement(i,j, pPhCalPhass[2*numOfChan_*i + 2*j + 1]);
      if (pPhCalOffss)
      {
        phaseCalOffsets_1->setElement(i,j,  pPhCalOffss[2*numOfChan_*i + 2*j    ]);
        phaseCalOffsets_2->setElement(i,j,  pPhCalOffss[2*numOfChan_*i + 2*j + 1]); 
      };
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPhaseCalInfoMk4(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCorrelatorInfo(const QString& band,
  QList<QString>& fourfitOutputFName,
  QList<QString>& fourfitErrorCodes,
  SgVector*& scanStartSec, SgVector*& scanStopSec
  )
{
  bool                          isOk=false;
  //
  if (correlatorType_ == CT_Mk3 || 
      correlatorType_ == CT_VLBA)
    isOk = loadObsCorrelatorInfoMk3(band, fourfitOutputFName, fourfitErrorCodes,
      scanStartSec, scanStopSec);
  else if ( correlatorType_ == CT_CRL ||
            correlatorType_ == CT_GSI ||
            correlatorType_ == CT_S2   )
    isOk = loadObsCorrelatorInfoCrl(band, fourfitOutputFName, fourfitErrorCodes,
      scanStartSec, scanStopSec);
  else if ( correlatorType_ == CT_Mk4     ||
            correlatorType_ == CT_Haystack||
            correlatorType_ == CT_Difx     )
    isOk = loadObsCorrelatorInfoMk4(band, fourfitOutputFName, fourfitErrorCodes,
      scanStartSec, scanStopSec);
  else
  {
    // Unknown, try each of types:
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfo(): unknown correlator type, trying Mk4 data format");
    isOk = loadObsCorrelatorInfoMk4(band, fourfitOutputFName, fourfitErrorCodes,
      scanStartSec, scanStopSec);
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfo(): unknown correlator type, trying S2 data format");
      isOk = loadObsCorrelatorInfoCrl(band, fourfitOutputFName, fourfitErrorCodes,
        scanStartSec, scanStopSec);
    };
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfo(): unknown correlator type, trying Mk3 data format");
      isOk = loadObsCorrelatorInfoMk3(band, fourfitOutputFName, fourfitErrorCodes,
        scanStartSec, scanStopSec);
    };
  };
  return isOk;
};



//
bool SgVgosDb::loadObsCorrelatorInfoMk3(const QString& band,
  QList<QString>& fourfitOutputFName,
  QList<QString>& fourfitErrorCodes,
  SgVector*& scanStartSec, SgVector*& scanStopSec
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk3(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk3(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrInfoInptMk3, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk3(): format check failed");
    return false;
  };
  if (ncdf.lookupVar(fcVLB1FILE.name()))
  {
    const char                 *p=ncdf.lookupVar(fcVLB1FILE.name())->data2char();
    int                         l;
    //
    l = ncdf.lookupVar(fcVLB1FILE.name())->dimensions().at(1)->getN();
    //
    fourfitOutputFName.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitOutputFName.append(QString::fromLatin1(p + l*i, l));
  };
  if (ncdf.lookupVar(fcFRNGERR.name()))
  {
    const char                 *p=ncdf.lookupVar(fcFRNGERR.name())->data2char();
    //
    fourfitErrorCodes.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitErrorCodes.append(QString::fromLatin1(p + i, 1));
  };
  if (ncdf.lookupVar(fcStartSec.name()) && ncdf.lookupVar(fcStopSec.name()))
  {
    const double               *pStart=ncdf.lookupVar(fcStartSec.name())->data2double();
    const double               *pStop =ncdf.lookupVar(fcStopSec.name())->data2double();
    scanStartSec = new SgVector(numOfObs_);
    scanStopSec  = new SgVector(numOfObs_);
    for (int i=0; i<numOfObs_; i++)
    {
      scanStartSec->setElement(i, pStart[i]);
      scanStopSec ->setElement(i, pStop [i]);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoMk3(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCorrelatorInfoCrl(const QString& band,
  QList<QString>& fourfitOutputFName,
  QList<QString>& fourfitErrorCodes,
  SgVector*& scanStartSec, SgVector*& scanStopSec
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrInfoInptCRL, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): format check failed");
    return false;
  };
  /*
  if (ncdf.lookupVar(fcVLB1FILE.name()))
  {
    const char                 *p=ncdf.lookupVar(fcVLB1FILE.name())->data2char();
    int                           l;
    //
    l = ncdf.lookupVar(fcVLB1FILE.name())->dimensions().at(1)->getN();
    //
    fourfitOutputFName.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitOutputFName.append(QString::fromLatin1(p + l*i, l));
  };
  */
  if (ncdf.lookupVar(fcFOURFFIL.name()))
  {
    const char                 *pFOURFFIL=ncdf.lookupVar(fcFOURFFIL.name())->data2char();
    int                         l;
    //
    l = ncdf.lookupVar(fcFOURFFIL.name())->dimensions().at(1)->getN();
    //
    fourfitOutputFName.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitOutputFName.append(QString::fromLatin1(pFOURFFIL + l*i, l));
  };
  if (ncdf.lookupVar(fcFRNGERR4S2.name()))
  {
    const short                *p=ncdf.lookupVar(fcFRNGERR4S2.name())->data2short();
    //
    fourfitErrorCodes.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitErrorCodes.append(*p==0?" ":"Q");
  };
  if (ncdf.lookupVar(fcStartSec.name()) && ncdf.lookupVar(fcStopSec.name()))
  {
    const double               *pStart=ncdf.lookupVar(fcStartSec.name())->data2double();
    const double               *pStop =ncdf.lookupVar(fcStopSec.name())->data2double();
    scanStartSec = new SgVector(numOfObs_);
    scanStopSec  = new SgVector(numOfObs_);
    for (int i=0; i<numOfObs_; i++)
    {
      scanStartSec->setElement(i, pStart[i]);
      scanStopSec ->setElement(i, pStop [i]);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoCrl(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCorrelatorInfoMk4(const QString& band,
  QList<QString>& fourfitOutputFName,
  QList<QString>& fourfitErrorCodes,
  SgVector*& scanStartSec, SgVector*& scanStopSec
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrInfoInptMk4, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): format check failed");
    return false;
  };
  if (ncdf.lookupVar(fcFOURFFIL.name()))
  {
    const char                 *pFOURFFIL=ncdf.lookupVar(fcFOURFFIL.name())->data2char();
    int                         l;
    //
    l = ncdf.lookupVar(fcFOURFFIL.name())->dimensions().at(1)->getN();
    //
    fourfitOutputFName.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitOutputFName.append(QString::fromLatin1(pFOURFFIL + l*i, l));
  };
  if (ncdf.lookupVar(fcFRNGERR.name()))
  {
    const char                 *p=ncdf.lookupVar(fcFRNGERR.name())->data2char();
    //
    fourfitErrorCodes.clear();
    for (int i=0; i<numOfObs_; i++)
      fourfitErrorCodes.append(QString::fromLatin1(p + i, 1));
  };
  if (ncdf.lookupVar(fcStartSec.name()) && ncdf.lookupVar(fcStopSec.name()))
  {
    const double               *pStart=ncdf.lookupVar(fcStartSec.name())->data2double();
    const double               *pStop =ncdf.lookupVar(fcStopSec.name())->data2double();
    scanStartSec = new SgVector(numOfObs_);
    scanStopSec  = new SgVector(numOfObs_);
    for (int i=0; i<numOfObs_; i++)
    {
      scanStartSec->setElement(i, pStart[i]);
      scanStopSec ->setElement(i, pStop [i]);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoMk4(): data loaded successfully from " + ncdf.getFileName());
  return true;
};





//
// full input functions:
//

//
bool SgVgosDb::loadObsCorrelatorInfo(const QString& band,
  QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
  QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//QVector<int>& corelVersions,                      // fcCORELVER
  QVector<int>& startOffsets,                       // fcStartOffset
  QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
//QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
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
)
{
  bool                          isOk=false;
  // reset the storages:
  fourfitOutputFName.clear();
  fourfitErrorCodes.clear();
//corelVersions.clear();
  startOffsets.clear();
  fourfitVersions.clear();
//dbeditVersion.clear();
  baselineCodes.clear();
  tapeQualityCodes.clear();
  stopOffsets.clear();
  hopsRevisionNumbers.clear();
  vlb1FileNames.clear();
  tapeIds_1.clear();
  tapeIds_2.clear();
  epochsOfCorrelations.clear();
  epochsOfCentrals.clear();
  epochsOfFourfitings.clear();
  epochsOfNominalScanTime.clear();
  
  QList<SgVector*>              vecs2clear;
  QList<SgMatrix*>              mats2clear;
  
  vecs2clear  << deltaEpochs << sbdResids << rateResids << effDurations << startSeconds << stopSeconds
              << percentOfDiscards << uniformityFactors << geocenterPhaseResids << geocenterPhaseTotals
              << geocenterSbDelays << geocenterGrDelays << geocenterDelayRates << probOfFalses
              << corrAprioriDelays << corrAprioriRates << corrAprioriAccs << incohAmps << incohAmps2
              << delayResids << sampleRate;
  mats2clear  << urvrs << instrDelays << searchParams << corrClocks << mDlys << mAux
              << starElev << zenithDelays;

  for (int i=0; i<vecs2clear.size(); i++)
  {
    SgVector                   *v=vecs2clear[i];
    if (v)
    {
      delete v;
      v = NULL;
    };
  };
  for (int i=0; i<mats2clear.size(); i++)
  {
    SgMatrix                   *m=mats2clear[i];
    if (m)
    {
      delete m;
      m = NULL;
    };
  };
  //
  //
  if (correlatorType_ == CT_Mk3 ||  
      correlatorType_ == CT_VLBA)
    isOk = loadObsCorrelatorInfoCrl(band, fourfitOutputFName, fourfitErrorCodes,
      deltaEpochs, searchParams, baselineCodes, tapeQualityCodes, sampleRate, sbdResids, rateResids, 
      effDurations, startSeconds, stopSeconds, percentOfDiscards, geocenterPhaseResids, 
      geocenterPhaseTotals, probOfFalses, corrAprioriDelays, corrAprioriRates, corrAprioriAccs, 
      incohAmps, incohAmps2, delayResids, vlb1FileNames, epochsOfCorrelations, epochsOfCentrals, 
      mDlys, mAux);
  else if ( correlatorType_ == CT_S2   )
    isOk = loadObsCorrelatorInfoS2(band, fourfitOutputFName, fourfitErrorCodes,
      deltaEpochs, searchParams, baselineCodes, tapeQualityCodes, sampleRate, sbdResids, rateResids, 
      effDurations, startSeconds, stopSeconds, percentOfDiscards, geocenterPhaseResids, 
      geocenterPhaseTotals, probOfFalses, corrAprioriDelays, corrAprioriRates, corrAprioriAccs, 
      incohAmps, incohAmps2, delayResids, vlb1FileNames, epochsOfCorrelations, epochsOfCentrals, 
      mDlys, mAux);
  else if ( correlatorType_ == CT_CRL ||
            correlatorType_ == CT_GSI  )
    isOk = loadObsCorrelatorInfoCrl(band, fourfitOutputFName, fourfitErrorCodes,
      deltaEpochs, searchParams, baselineCodes, tapeQualityCodes, sampleRate, sbdResids, rateResids, 
      effDurations, startSeconds, stopSeconds, percentOfDiscards, geocenterPhaseResids, 
      geocenterPhaseTotals, probOfFalses, corrAprioriDelays, corrAprioriRates, corrAprioriAccs, 
      incohAmps, incohAmps2, delayResids, vlb1FileNames, epochsOfCorrelations, epochsOfCentrals, 
      mDlys, mAux);
  else if ( correlatorType_ == CT_Mk4     ||
            correlatorType_ == CT_Haystack||
            correlatorType_ == CT_Difx     )
    isOk = loadObsCorrelatorInfoMk4(band, fourfitOutputFName, fourfitErrorCodes, 
      startOffsets, fourfitVersions, deltaEpochs, urvrs, instrDelays, starElev, zenithDelays,
      searchParams, baselineCodes, tapeQualityCodes, stopOffsets, hopsRevisionNumbers,
      sbdResids, rateResids, effDurations, startSeconds, stopSeconds, percentOfDiscards,
      uniformityFactors, geocenterPhaseResids, geocenterPhaseTotals, geocenterSbDelays,
      geocenterGrDelays, geocenterDelayRates, probOfFalses, corrAprioriDelays, corrAprioriRates,
      corrAprioriAccs, incohAmps, incohAmps2, delayResids, tapeIds_1, tapeIds_2, epochsOfCorrelations,
      epochsOfCentrals, epochsOfFourfitings, epochsOfNominalScanTime, corrClocks);
  else
  {
    // Unknown, try each of types:
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfo(): unknown correlator type, trying Mk4 data format");
/*
    isOk = loadObsCorrelatorInfoMk4(band, fourfitOutputFName, fourfitErrorCodes,
      startSeconds, stopSeconds);
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfo(): unknown correlator type, trying S2 data format");
      isOk = loadObsCorrelatorInfoCrl(band, fourfitOutputFName, fourfitErrorCodes,
        startSeconds, stopSeconds);
    };
    if (!isOk)
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfo(): unknown correlator type, trying Mk3 data format");
      isOk = loadObsCorrelatorInfoMk3(band, fourfitOutputFName, fourfitErrorCodes,
        startSeconds, stopSeconds);
    };
*/
  };
  return isOk;
};



//
bool SgVgosDb::loadObsCorrelatorInfoCrl(const QString& band,
  QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
  QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
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
)
{
  int                           dlyEpochOffset;
  int                           dlyEpochSize;
  // a regular K5 output:
  dlyEpochOffset  = 0;
  dlyEpochSize    = 1;

  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();

  if (!checkFormat(fcfCorrInfoKomb, ncdf))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoCrl(): the regular format check failed");
    if (!checkFormat(fcfCorrInfoKomb_V2, ncdf))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfoCrl(): the alternative format check failed");
      return false;
    }
    else
    {
      // a dbedit'ed K5 output:
      dlyEpochOffset  = 0;
      dlyEpochSize    = 2;
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::loadObsCorrelatorInfoCrl(): the alternative format check is ok");
    };
  }
  else 
  {
    // a regular K5 output:
    dlyEpochOffset  = 0;
    dlyEpochSize    = 1;
  };
  
  //
  //
  // strings:
  const char                   *pCORBASCD=ncdf.lookupVar(fcCORBASCD.name())?
                                  ncdf.lookupVar(fcCORBASCD.name())->data2char():NULL;
  const char                   *pFOURFFIL=ncdf.lookupVar(fcFOURFFIL.name())?
                                  ncdf.lookupVar(fcFOURFFIL.name())->data2char():NULL;
  const char                   *pTapeCode=ncdf.lookupVar(fcTapeCode.name())?
                                  ncdf.lookupVar(fcTapeCode.name())->data2char():NULL;
  const char                   *pVLB1FILE=ncdf.lookupVar(fcVLB1FILE.name())?
                                  ncdf.lookupVar(fcVLB1FILE.name())->data2char():NULL;
  //
  // shorts:
  const short                  *pUTCCorr=ncdf.lookupVar(fcUTCCorr.name())?
                                  ncdf.lookupVar(fcUTCCorr.name())->data2short():NULL;
  const short                  *pUTCMidObs=ncdf.lookupVar(fcUTCMidObs.name())->data2short();
//const short                  *pDBEDITVE=ncdf.lookupVar(fcDBEDITVE.name())->data2short();
  // non-mandatory:
  const short                  *pFRNGERR4S2=NULL;
  const char                   *pFRNGERR   =NULL;
  const short                  *pRECSETUP=ncdf.lookupVar(fcRECSETUP.name())?
                                  ncdf.lookupVar(fcRECSETUP.name())->data2short():NULL;
  //const short                *pUTCVLB2=ncdf.lookupVar(fcUTCVLB2.name())?
  //                              ncdf.lookupVar(fcUTCVLB2.name())->data2short():NULL;
  //
  // non-mandatory:
  // doubles:
  const double                 *pSRCHPAR=ncdf.lookupVar(fcSRCHPAR.name())->data2double();
  const double                 *pEffectiveDuration=
                                  ncdf.lookupVar(fcEffectiveDuration.name())->data2double();
  const double                 *pDISCARD=ncdf.lookupVar(fcDISCARD.name())->data2double();
  const double                 *pGeocResidPhase=ncdf.lookupVar(fcGeocResidPhase.name())->data2double();
  const double                 *pProbFalseDetection=
                                  ncdf.lookupVar(fcProbFalseDetection.name())->data2double();
  const double                 *pABASRATE=ncdf.lookupVar(fcABASRATE.name())?
                                  ncdf.lookupVar(fcABASRATE.name())->data2double():NULL;
  const double                 *pABASDEL=ncdf.lookupVar(fcABASDEL.name())?
                                  ncdf.lookupVar(fcABASDEL.name())->data2double():NULL;
  const double                 *pABASACCE=ncdf.lookupVar(fcABASACCE.name())?
                                  ncdf.lookupVar(fcABASACCE.name())->data2double():NULL;
  const double                 *pGeocPhase=ncdf.lookupVar(fcGeocPhase.name())->data2double();
  const double                 *pINCOHAMP=ncdf.lookupVar(fcINCOHAMP.name())->data2double();
  const double                 *pINCOH2=ncdf.lookupVar(fcINCOH2.name())->data2double();
  const double                 *pDELRESID=ncdf.lookupVar(fcDELRESID.name())->data2double();
  // non-mandatory:
  const double                 *pDELTAEPO=ncdf.lookupVar(fcDELTAEPO.name())?
                                  ncdf.lookupVar(fcDELTAEPO.name())->data2double():NULL;
  const double                 *pSBRESID=ncdf.lookupVar(fcSBRESID.name())?
                                  ncdf.lookupVar(fcSBRESID.name())->data2double():NULL;
  const double                 *pRATRESID=ncdf.lookupVar(fcRATRESID.name())?
                                  ncdf.lookupVar(fcRATRESID.name())->data2double():NULL;
  const double                 *pStartSec=ncdf.lookupVar(fcStartSec.name())?
                                  ncdf.lookupVar(fcStartSec.name())->data2double():NULL;
  const double                 *pStopSec=ncdf.lookupVar(fcStopSec.name())?
                                  ncdf.lookupVar(fcStopSec.name())->data2double():NULL;
  const double                 *pDLYEPOp1;
  const double                 *pDLYEPOCH;
  const double                 *pDLYEPOm1;
  if (dlyEpochSize == 1)
  {
    pDLYEPOp1=ncdf.lookupVar(fcDLYEPOp1.name())?ncdf.lookupVar(fcDLYEPOp1.name())->data2double():NULL;
    pDLYEPOCH=ncdf.lookupVar(fcDLYEPOCH.name())?ncdf.lookupVar(fcDLYEPOCH.name())->data2double():NULL;
    pDLYEPOm1=ncdf.lookupVar(fcDLYEPOm1.name())?ncdf.lookupVar(fcDLYEPOm1.name())->data2double():NULL;
  }
  else
  {
    pDLYEPOp1=ncdf.lookupVar(fcDLYEPOp1_V2.name())?ncdf.lookupVar(fcDLYEPOp1_V2.name())->data2double():
      NULL;
    pDLYEPOCH=ncdf.lookupVar(fcDLYEPOCH_V2.name())?ncdf.lookupVar(fcDLYEPOCH_V2.name())->data2double():
      NULL;
    pDLYEPOm1=ncdf.lookupVar(fcDLYEPOm1_V2.name())?ncdf.lookupVar(fcDLYEPOm1_V2.name())->data2double():
      NULL;
  };

  const double                 *pAPCLOFST=ncdf.lookupVar(fcAPCLOFST.name())?
                                  ncdf.lookupVar(fcAPCLOFST.name())->data2double():NULL;
  const double                 *pTOTPCENT=ncdf.lookupVar(fcTOTPCENT.name())?
                                  ncdf.lookupVar(fcTOTPCENT.name())->data2double():NULL;
  const double                 *pRATOBSVM=ncdf.lookupVar(fcRATOBSVM.name())?
                                  ncdf.lookupVar(fcRATOBSVM.name())->data2double():NULL;
  const double                 *pDELOBSVM=ncdf.lookupVar(fcDELOBSVM.name())?
                                  ncdf.lookupVar(fcDELOBSVM.name())->data2double():NULL;
  //
  //
  double                        d;
  //
  // adjust the sizes:
  if (pFOURFFIL)
    fourfitOutputFName.resize(numOfObs_);

  // can be either char or int:
  if (ncdf.lookupVar(fcFRNGERR4S2.name()) && 
    ncdf.lookupVar(fcFRNGERR4S2.name())->getTypeOfData() == fcFRNGERR4S2.typeOfData())
    pFRNGERR4S2 = ncdf.lookupVar(fcFRNGERR4S2.name())->data2short();
  else if (ncdf.lookupVar(fcFRNGERR.name()) &&
    ncdf.lookupVar(fcFRNGERR.name())->getTypeOfData() == fcFRNGERR.typeOfData())
    pFRNGERR = ncdf.lookupVar(fcFRNGERR.name())->data2char();

  if (pFRNGERR || pFRNGERR4S2)
    fourfitErrorCodes.resize(numOfObs_);
  if (pCORBASCD)
    baselineCodes.resize(numOfObs_);
  if (pTapeCode)
    tapeQualityCodes.resize(numOfObs_);
  if (pVLB1FILE)
    vlb1FileNames.resize(numOfObs_);
  //dbeditVersion.resize(numOfObs_);

  // ?mandatory:
  epochsOfCorrelations.resize(numOfObs_);
  epochsOfCentrals.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
//  dbeditVersion[i].resize(3);
    epochsOfCorrelations[i].resize(6);
    epochsOfCentrals[i].resize(6);
  };
  //
  if (pDELTAEPO)
    deltaEpochs = new SgVector(numOfObs_);
  if (pSBRESID)
    sbdResids = new SgVector(numOfObs_);
  if (pRATRESID)
    rateResids = new SgVector(numOfObs_);
  if (pEffectiveDuration)
    effDurations = new SgVector(numOfObs_);
  if (pStartSec)
    startSeconds = new SgVector(numOfObs_);
  if (pStopSec)
    stopSeconds = new SgVector(numOfObs_);
  if (pDISCARD)
    percentOfDiscards = new SgVector(numOfObs_);
  if (pGeocResidPhase)
    geocenterPhaseResids = new SgVector(numOfObs_);
  if (pGeocPhase)
    geocenterPhaseTotals = new SgVector(numOfObs_);
  if (pProbFalseDetection)
    probOfFalses = new SgVector(numOfObs_);
  if (pABASDEL)
    corrAprioriDelays = new SgVector(numOfObs_);
  if (pABASRATE)
    corrAprioriRates = new SgVector(numOfObs_);
  if (pABASACCE)
    corrAprioriAccs = new SgVector(numOfObs_);
  if (pINCOHAMP)
    incohAmps = new SgVector(numOfObs_);
  if (pINCOH2)
    incohAmps2 = new SgVector(numOfObs_);
  if (pDELRESID)
    delayResids = new SgVector(numOfObs_);
  //
  searchParams = new SgMatrix(numOfObs_, 6);
  //
  if (pRECSETUP)
    sampleRate = new SgVector(numOfObs_);
  if (pDLYEPOm1 || pDLYEPOCH || pDLYEPOp1)
    mDlys = new SgMatrix(numOfObs_, 3);
  if (pAPCLOFST || pTOTPCENT || pRATOBSVM || pDELOBSVM)
    mAux  = new SgMatrix(numOfObs_, 4);


//int                           lCORBASCD, lFOURFFIL, lTapeCode, lVLB1FILE;
  int                           lCORBASCD, lFOURFFIL, lFRNGERR, lTapeCode, lVLB1FILE;
  lCORBASCD = pCORBASCD?ncdf.lookupVar(fcCORBASCD.name())->dimensions().at(1)->getN():0;
  lFOURFFIL = pFOURFFIL?ncdf.lookupVar(fcFOURFFIL.name())->dimensions().at(1)->getN():0;
  lTapeCode = pTapeCode?ncdf.lookupVar(fcTapeCode.name())->dimensions().at(1)->getN():0;
  lVLB1FILE = pVLB1FILE?ncdf.lookupVar(fcVLB1FILE.name())->dimensions().at(1)->getN():0;
  lFRNGERR  = 1;

  for (int i=0; i<numOfObs_; i++)
  {
    //
    // strings:
    if (pCORBASCD)
      baselineCodes[i] = QString::fromLatin1(pCORBASCD + lCORBASCD*i, lCORBASCD);
    if (pFOURFFIL)
      fourfitOutputFName[i] = QString::fromLatin1(pFOURFFIL + lFOURFFIL*i, lFOURFFIL);
    if (pTapeCode)
      tapeQualityCodes[i] = QString::fromLatin1(pTapeCode + lTapeCode*i, lTapeCode);
    //
    // shorts:
    for (int j=0; j<6; j++)
    {
      if (pUTCCorr)
        epochsOfCorrelations[i][j] = pUTCCorr[6*i + j];
      if (pUTCMidObs)
        epochsOfCentrals[i][j] = pUTCMidObs[6*i + j];
    };
//  dbeditVersion[i][0] = pDBEDITVE[3*i  ];
//  dbeditVersion[i][1] = pDBEDITVE[3*i+1];
//  dbeditVersion[i][2] = pDBEDITVE[3*i+2];
    if (pSBRESID)
      sbdResids->setElement(i, pSBRESID[i]);
    if (pRATRESID)
      rateResids->setElement(i, pRATRESID[i]);
    if (pStartSec)
    {     
      if (100000.0 < (d=pStartSec[i])) // some of K5 sessions have different interpretion of this LCode
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoCrl(): got unexpected value for start second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      startSeconds->setElement(i, d);
    };
    if (pStopSec)
    {
      if (100000.0 < (d=pStopSec[i])) // some of K5 sessions have different interpretion of this LCode
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoCrl(): got unexpected value for stop second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      stopSeconds->setElement(i, d);
    };
    if (pDELTAEPO)
      deltaEpochs->setElement(i, pDELTAEPO[i]);
    //
    // doubles:
    if (pEffectiveDuration)
      effDurations->setElement(i, pEffectiveDuration[i]);
    if (pABASRATE)
      corrAprioriRates->setElement(i, pABASRATE[i]);
    if (pABASDEL)
      corrAprioriDelays->setElement(i, pABASDEL[i]);
    if (pABASACCE)
      corrAprioriAccs->setElement(i, pABASACCE[i]);
    if (pDISCARD)
      percentOfDiscards->setElement(i, pDISCARD[i]);
    if (pGeocResidPhase)
      geocenterPhaseResids->setElement(i, pGeocResidPhase[i]);
    if (pGeocPhase)
      geocenterPhaseTotals->setElement(i, pGeocPhase[i]);
    if (pProbFalseDetection)
      probOfFalses->setElement(i, pProbFalseDetection[i]);
    if (pINCOHAMP)
      incohAmps->setElement(i, pINCOHAMP[i]);
    if (pINCOH2)
      incohAmps2->setElement(i, pINCOH2[i]);
    if (pDELRESID)
      delayResids->setElement(i, pDELRESID[i]);
    for (int j=0; j<6; j++)
      searchParams->setElement(i,j, pSRCHPAR[6*i + j]);
    //
    // GSI-specific variables:
    if (pVLB1FILE)
      vlb1FileNames[i] = QString::fromLatin1(pVLB1FILE + lVLB1FILE*i, lVLB1FILE);

    if (pFRNGERR)
      fourfitErrorCodes[i] = QString::fromLatin1(pFRNGERR + lFRNGERR*i, lFRNGERR);
    else if (pFRNGERR4S2)
      fourfitErrorCodes[i] = pFRNGERR4S2[i]==0?" ":"F";

    if (pRECSETUP)
      sampleRate->setElement(i, pRECSETUP[3*i]*1.0e3);
    //
    if (pDLYEPOm1)
      mDlys->setElement(i,0,  pDLYEPOm1[dlyEpochSize*i + dlyEpochOffset]);
    else if (mDlys)
      mDlys->setElement(i,0,  0.0);
    if (pDLYEPOCH)
      mDlys->setElement(i,1,  pDLYEPOCH[dlyEpochSize*i + dlyEpochOffset]);
    else if (mDlys)
      mDlys->setElement(i,1,  0.0);
    if (pDLYEPOp1)
      mDlys->setElement(i,2,  pDLYEPOp1[dlyEpochSize*i + dlyEpochOffset]);
    else if (mDlys)
      mDlys->setElement(i,2,  0.0);
    //
    if (pAPCLOFST)
      mAux->setElement(i, 0,  pAPCLOFST[i]);
    if (pTOTPCENT)
      mAux->setElement(i, 1,  pTOTPCENT[i]);
    if (pRATOBSVM)
      mAux->setElement(i, 2,  pRATOBSVM[i]);
    if (pDELOBSVM)
      mAux->setElement(i, 3,  (pDELOBSVM[2*i] + pDELOBSVM[2*i + 1])*1.0e-6);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoCrl(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCorrelatorInfoS2(const QString& band,
  QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
  QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
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
)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoS2(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoS2(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrInfoInptS2, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoS2(): format check failed");
    return false;
  };
  //
  //
  // strings:
  const char                   *pVLB1FILE=ncdf.lookupVar(fcVLB1FILE.name())?
                                  ncdf.lookupVar(fcVLB1FILE.name())->data2char():NULL;
  // doubles:
  const short                  *pRECSETUP=ncdf.lookupVar(fcRECSETUP.name())?
                                  ncdf.lookupVar(fcRECSETUP.name())->data2short():NULL;
  const double                 *pSBRESID=ncdf.lookupVar(fcSBRESID.name())?
                                  ncdf.lookupVar(fcSBRESID.name())->data2double():NULL;
  const double                 *pRATRESID=ncdf.lookupVar(fcRATRESID.name())?
                                  ncdf.lookupVar(fcRATRESID.name())->data2double():NULL;
  const double                 *pDELRESID=ncdf.lookupVar(fcDELRESID.name())?
                                  ncdf.lookupVar(fcDELRESID.name())->data2double():NULL;
  const double                 *pStartSec=ncdf.lookupVar(fcStartSec.name())?
                                  ncdf.lookupVar(fcStartSec.name())->data2double():NULL;
  const double                 *pStopSec=ncdf.lookupVar(fcStopSec.name())?
                                  ncdf.lookupVar(fcStopSec.name())->data2double():NULL;
  const double                 *pGeocPhase=ncdf.lookupVar(fcGeocPhase.name())?
                                  ncdf.lookupVar(fcGeocPhase.name())->data2double():NULL;
/* Will use them later:
  const double                 *pUTCErr=ncdf.lookupVar(fcUTCErr.name())?
                                  ncdf.lookupVar(fcUTCErr.name())->data2double():NULL;
  const double                 *pVFDWELL=ncdf.lookupVar(fcVFDWELL.name())?
                                  ncdf.lookupVar(fcVFDWELL.name())->data2double():NULL;
  const double                 *pS2EFFREQ=ncdf.lookupVar(fcS2EFFREQ.name())?
                                  ncdf.lookupVar(fcS2EFFREQ.name())->data2double():NULL;
  const double                 *pS2REFREQ=ncdf.lookupVar(fcS2REFREQ.name())?
                                  ncdf.lookupVar(fcS2REFREQ.name())->data2double():NULL;
  const double                 *pS2PHEFRQ=ncdf.lookupVar(fcS2PHEFRQ.name())?
                                  ncdf.lookupVar(fcS2PHEFRQ.name())->data2double():NULL;
  const double                 *pTotalFringeErr=ncdf.lookupVar(fcTotalFringeErr.name())?
                                  ncdf.lookupVar(fcTotalFringeErr.name())->data2double():NULL;
  const double                 *pTimeSinceStart=ncdf.lookupVar(fcTimeSinceStart.name())?
                                  ncdf.lookupVar(fcTimeSinceStart.name())->data2double():NULL;
*/
  const double                 *pDURATION=ncdf.lookupVar(fcDURATION.name())?
                                  ncdf.lookupVar(fcDURATION.name())->data2double():NULL;

  //
  //
  double                        d;
  //
  //
  // adjust the sizes:
  if (pVLB1FILE)
    vlb1FileNames.resize(numOfObs_);

  //
  if (pRECSETUP)
    sampleRate = new SgVector(numOfObs_);
  if (pSBRESID)
    sbdResids = new SgVector(numOfObs_);
  if (pRATRESID)
    rateResids = new SgVector(numOfObs_);
  if (pDELRESID)
    delayResids = new SgVector(numOfObs_);
  if (pStartSec)
    startSeconds = new SgVector(numOfObs_);
  if (pStopSec)
    stopSeconds = new SgVector(numOfObs_);
  if (pGeocPhase)
    geocenterPhaseTotals = new SgVector(numOfObs_);
  if (pDURATION)
    effDurations = new SgVector(numOfObs_);


  int                           lVLB1FILE;
  lVLB1FILE = pVLB1FILE?ncdf.lookupVar(fcVLB1FILE.name())->dimensions().at(1)->getN():0;

  for (int i=0; i<numOfObs_; i++)
  {
    //
    // strings:
    if (pVLB1FILE)
      vlb1FileNames[i] = QString::fromLatin1(pVLB1FILE + lVLB1FILE*i, lVLB1FILE);
    // doubles:
    if (pRECSETUP)
      sampleRate->setElement(i, pRECSETUP[3*i]*1.0e3);
    if (pSBRESID)
      sbdResids->setElement(i, pSBRESID[i]);
    if (pRATRESID)
      rateResids->setElement(i, pRATRESID[i]);
    if (pDELRESID)
      delayResids->setElement(i, pDELRESID[i]);
    if (pStartSec)
    {
      if (40000.0 < (d=pStartSec[i]))
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoS2(): got unexpected value for start second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      startSeconds->setElement(i, d);
    };
    if (pStopSec)
    {
      if (40000.0 < (d=pStopSec[i]))
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoS2(): got unexpected value for stop second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      stopSeconds->setElement(i, d);
    };
    if (pGeocPhase)
      geocenterPhaseTotals->setElement(i, pGeocPhase[i]);

    if (pDURATION)
      effDurations->setElement(i, pDURATION[i]);

  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoS2(): data loaded successfully from " + ncdf.getFileName());
  return true;
};




//
bool SgVgosDb::loadObsCorrelatorInfoMk4(const QString& band,
  QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
  QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
//QVector<int>& corelVersions,                      // fcCORELVER
  QVector<int>& startOffsets,                       // fcStartOffset
  QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
//QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
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
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): the vgosDb variable CorrInfo for the band [" + band + "] is empty");
    return false;
  };
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrInfoMk4, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCorrelatorInfoMk4(): format check failed");
    return false;
  };
  //
  //

  // strings:
  const char                   *pCORBASCD=ncdf.lookupVar(fcCORBASCD.name())?
                                  ncdf.lookupVar(fcCORBASCD.name())->data2char():NULL;
  const char                   *pFOURFFIL=ncdf.lookupVar(fcFOURFFIL.name())?
                                  ncdf.lookupVar(fcFOURFFIL.name())->data2char():NULL;
  const char                   *pFRNGERR=ncdf.lookupVar(fcFRNGERR.name())?
                                  ncdf.lookupVar(fcFRNGERR.name())->data2char():NULL;
  const char                   *pTAPEID=ncdf.lookupVar(fcTAPEID.name())?
                                  ncdf.lookupVar(fcTAPEID.name())->data2char():NULL;
  const char                   *pTapeCode=ncdf.lookupVar(fcTapeCode.name())?
                                  ncdf.lookupVar(fcTapeCode.name())->data2char():NULL;

  // shorts:
  const short                  *pUTCCorr=ncdf.lookupVar(fcUTCCorr.name())?
                                  ncdf.lookupVar(fcUTCCorr.name())->data2short():NULL;
  const short                  *pUTCMidObs=ncdf.lookupVar(fcUTCMidObs.name())->data2short();
//const short                  *pDBEDITVE=ncdf.lookupVar(fcDBEDITVE.name())->data2short();
  // non-mandatory:
//const short                  *pCORELVER=ncdf.lookupVar(fcCORELVER.name())?
//                                ncdf.lookupVar(fcCORELVER.name())->data2short():NULL;
  const short                  *pFOURFVER=ncdf.lookupVar(fcFOURFVER.name())?
                                  ncdf.lookupVar(fcFOURFVER.name())->data2short():NULL;
  const short                  *pStartOffset=ncdf.lookupVar(fcStartOffset.name())?
                                  ncdf.lookupVar(fcStartOffset.name())->data2short():NULL;
  const short                  *pStopOffset=ncdf.lookupVar(fcStopOffset.name())?
                                  ncdf.lookupVar(fcStopOffset.name())->data2short():NULL;
  const short                  *pHopsRevNum=ncdf.lookupVar(fcHopsRevNum.name())?
                                  ncdf.lookupVar(fcHopsRevNum.name())->data2short():NULL;
  const short                  *pFOURFUTC=ncdf.lookupVar(fcFOURFUTC.name())?
                                  ncdf.lookupVar(fcFOURFUTC.name())->data2short():NULL;
  const short                  *pUTCScan=ncdf.lookupVar(fcUTCScan.name())?
                                  ncdf.lookupVar(fcUTCScan.name())->data2short():NULL;
  // doubles:
  const double                 *pSRCHPAR=ncdf.lookupVar(fcSRCHPAR.name())->data2double();
  const double                 *pEffectiveDuration=
                                  ncdf.lookupVar(fcEffectiveDuration.name())->data2double();
  const double                 *pDISCARD=ncdf.lookupVar(fcDISCARD.name())->data2double();
  const double                 *pGeocResidPhase=ncdf.lookupVar(fcGeocResidPhase.name())->data2double();
  const double                 *pProbFalseDetection=
                                  ncdf.lookupVar(fcProbFalseDetection.name())->data2double();
  const double                 *pABASRATE=ncdf.lookupVar(fcABASRATE.name())?
                                  ncdf.lookupVar(fcABASRATE.name())->data2double():NULL;
  const double                 *pABASDEL=ncdf.lookupVar(fcABASDEL.name())?
                                  ncdf.lookupVar(fcABASDEL.name())->data2double():NULL;
  const double                 *pABASACCE=ncdf.lookupVar(fcABASACCE.name())?
                                  ncdf.lookupVar(fcABASACCE.name())->data2double():NULL;
  const double                 *pGeocPhase=ncdf.lookupVar(fcGeocPhase.name())->data2double();
  const double                 *pINCOHAMP=ncdf.lookupVar(fcINCOHAMP.name())->data2double();
  const double                 *pINCOH2=ncdf.lookupVar(fcINCOH2.name())->data2double();
  const double                 *pDELRESID=ncdf.lookupVar(fcDELRESID.name())->data2double();
  // non-mandatory:
  const double                 *pURVR=ncdf.lookupVar(fcURVR.name())?
                                  ncdf.lookupVar(fcURVR.name())->data2double():NULL;
  const double                 *pIDELAY=ncdf.lookupVar(fcIDELAY.name())?
                                  ncdf.lookupVar(fcIDELAY.name())->data2double():NULL;
  const double                 *pSTARELEV=ncdf.lookupVar(fcSTARELEV.name())?
                                  ncdf.lookupVar(fcSTARELEV.name())->data2double():NULL;
  const double                 *pZDELAY=ncdf.lookupVar(fcZDELAY.name())?
                                  ncdf.lookupVar(fcZDELAY.name())->data2double():NULL;
  const double                 *pQBFACTOR=ncdf.lookupVar(fcQBFACTOR.name())?
                                  ncdf.lookupVar(fcQBFACTOR.name())->data2double():NULL;
  const double                 *pGeocSBD=ncdf.lookupVar(fcGeocSBD.name())?
                                  ncdf.lookupVar(fcGeocSBD.name())->data2double():NULL;
  const double                 *pGeocRate=ncdf.lookupVar(fcGeocRate.name())?
                                  ncdf.lookupVar(fcGeocRate.name())->data2double():NULL;
  const double                 *pGeocMBD=ncdf.lookupVar(fcGeocMBD.name())?
                                  ncdf.lookupVar(fcGeocMBD.name())->data2double():NULL;
  const double                 *pCORCLOCK=ncdf.lookupVar(fcCORCLOCK.name())?
                                  ncdf.lookupVar(fcCORCLOCK.name())->data2double():NULL;
  const double                 *pDELTAEPO=ncdf.lookupVar(fcDELTAEPO.name())?
                                  ncdf.lookupVar(fcDELTAEPO.name())->data2double():NULL;
  const double                 *pSBRESID=ncdf.lookupVar(fcSBRESID.name())?
                                  ncdf.lookupVar(fcSBRESID.name())->data2double():NULL;
  const double                 *pRATRESID=ncdf.lookupVar(fcRATRESID.name())?
                                  ncdf.lookupVar(fcRATRESID.name())->data2double():NULL;
  const double                 *pStartSec=ncdf.lookupVar(fcStartSec.name())?
                                  ncdf.lookupVar(fcStartSec.name())->data2double():NULL;
  const double                 *pStopSec=ncdf.lookupVar(fcStopSec.name())?
                                  ncdf.lookupVar(fcStopSec.name())->data2double():NULL;
  //
  bool                          have2swapGeocMbdAndRate=false;
  double                        d;
  //
  // workaround for the bug of swapped geocentric MBD and rates:
  SgNcdfVariable               *svcV;
  if ( (svcV=ncdf.lookupServiceVar(SgNetCdf::svcProgram)) )
  {
    const char                 *c=svcV->data2char();
    int                         l=svcV->dimensions().at(0)->getN();
    SgVersion                   v;
    if (v.parseString(QString::fromLatin1(c, l)))
    {
      if (v.getSoftwareName() == "vgosDbMake" &&
        v < SgVersion("", 0, 4, 4, "", tZero))
      {
        have2swapGeocMbdAndRate = true;
        logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoMk4(): found a file with the geocentric delay and rate swap bug; "
          "the bug has been corrected");
      };
    };
  };

  //
  // adjust the sizes:
  if (pFOURFFIL)
    fourfitOutputFName.resize(numOfObs_);
  if (pFRNGERR)
    fourfitErrorCodes.resize(numOfObs_);
  if (pCORBASCD)
    baselineCodes.resize(numOfObs_);
  if (pTapeCode)
    tapeQualityCodes.resize(numOfObs_);
  if (pTAPEID)
  {
    tapeIds_1.resize(numOfObs_);
    tapeIds_2.resize(numOfObs_);
  };
//corelVersions.resize(numOfObs_);
  if (pStartOffset)
    startOffsets.resize(numOfObs_);
  if (pStopOffset)
    stopOffsets.resize(numOfObs_);
  if (pHopsRevNum)
    hopsRevisionNumbers.resize(numOfObs_);
  if (pFOURFVER)
    fourfitVersions.resize(numOfObs_);
//dbeditVersion.resize(numOfObs_);
  epochsOfCorrelations.resize(numOfObs_);
  epochsOfCentrals.resize(numOfObs_);
  if (pFOURFUTC)
    epochsOfFourfitings.resize(numOfObs_);
  if (pUTCScan)
    epochsOfNominalScanTime.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    fourfitVersions[i].resize(2);
//  dbeditVersion[i].resize(3);
    epochsOfCorrelations[i].resize(6);
    epochsOfCentrals[i].resize(6);
    if (pFOURFUTC)
      epochsOfFourfitings[i].resize(6);
    if (pUTCScan)
      epochsOfNominalScanTime[i].resize(6);
  };
  //
  if (pDELTAEPO)
    deltaEpochs = new SgVector(numOfObs_);
  if (pSBRESID)
    sbdResids = new SgVector(numOfObs_);
  if (pRATRESID)
    rateResids = new SgVector(numOfObs_);
  effDurations = new SgVector(numOfObs_);
  if (pStartSec)
    startSeconds = new SgVector(numOfObs_);
  if (pStopSec)
    stopSeconds = new SgVector(numOfObs_);
  percentOfDiscards = new SgVector(numOfObs_);
  if (pQBFACTOR)
    uniformityFactors = new SgVector(numOfObs_);
  geocenterPhaseResids = new SgVector(numOfObs_);
  geocenterPhaseTotals = new SgVector(numOfObs_);
  if (pGeocSBD)
    geocenterSbDelays = new SgVector(numOfObs_);
  if (pGeocMBD)
    geocenterGrDelays = new SgVector(numOfObs_);
  if (pGeocRate)
    geocenterDelayRates = new SgVector(numOfObs_);
  probOfFalses = new SgVector(numOfObs_);
  corrAprioriDelays = new SgVector(numOfObs_);
  corrAprioriRates = new SgVector(numOfObs_);
  corrAprioriAccs = new SgVector(numOfObs_);
  incohAmps = new SgVector(numOfObs_);
  incohAmps2 = new SgVector(numOfObs_);
  delayResids = new SgVector(numOfObs_);
  if (pURVR)
    urvrs = new SgMatrix(numOfObs_, 2);
  if (pIDELAY)
    instrDelays = new SgMatrix(numOfObs_, 2);
  if (pSTARELEV)
    starElev = new SgMatrix(numOfObs_, 2);
  if (pZDELAY)
    zenithDelays = new SgMatrix(numOfObs_, 2);
  searchParams = new SgMatrix(numOfObs_, 6);
  if (pCORCLOCK)
    corrClocks = new SgMatrix(numOfObs_, 4);
  //
//int                           lCORBASCD, lFOURFFIL, lFRNGERR, lTAPEID, lTapeCode;
  int                           lCORBASCD, lFOURFFIL, lFRNGERR, lTAPEID, lTapeCode;
  lCORBASCD = pCORBASCD?ncdf.lookupVar(fcCORBASCD.name())->dimensions().at(1)->getN():0;
  lFOURFFIL = pFOURFFIL?ncdf.lookupVar(fcFOURFFIL.name())->dimensions().at(1)->getN():0;
//lFRNGERR  = pFRNGERR ?ncdf.lookupVar(fcFRNGERR.name()) ->dimensions().at(1)->getN():0;
  lFRNGERR  = 1;
  lTAPEID   = pTAPEID  ?ncdf.lookupVar(fcTAPEID.name())  ->dimensions().at(2)->getN():0;
  lTapeCode = pTapeCode?ncdf.lookupVar(fcTapeCode.name())->dimensions().at(1)->getN():0;

  for (int i=0; i<numOfObs_; i++)
  {
    //
    // strings:
    if (pCORBASCD)
      baselineCodes[i] = QString::fromLatin1(pCORBASCD + lCORBASCD*i, lCORBASCD);
    if (pFOURFFIL)
//    fourfitOutputFName[i] = QString::fromLatin1(pFOURFFIL + lFOURFFIL*i);
      fourfitOutputFName[i] = QString::fromLatin1(pFOURFFIL + lFOURFFIL*i, 
                                  ((size_t)lFOURFFIL<strlen(pFOURFFIL + lFOURFFIL*i) ? lFOURFFIL : -1));
    if (pFRNGERR)
      fourfitErrorCodes[i] = QString::fromLatin1(pFRNGERR + lFRNGERR*i, lFRNGERR);
    if (pTAPEID)
    {
      tapeIds_1[i] = QString::fromLatin1(pTAPEID + lTAPEID*2*i,       2*lTAPEID);
      tapeIds_2[i] = QString::fromLatin1(pTAPEID + lTAPEID*(2*i + 1), 2*lTAPEID);
    };
    if (pTapeCode)
      tapeQualityCodes[i] = QString::fromLatin1(pTapeCode + lTapeCode*i, lTapeCode);
    //
    // shorts:
    for (int j=0; j<6; j++)
    {
      if (pUTCCorr)
        epochsOfCorrelations[i][j] = pUTCCorr[6*i + j];
      if (pUTCMidObs)
        epochsOfCentrals[i][j] = pUTCMidObs[6*i + j];
      if (pFOURFUTC)
        epochsOfFourfitings[i][j] = pFOURFUTC[6*i + j];
      if (pUTCScan)
        epochsOfNominalScanTime[i][j] = pUTCScan[6*i + j];
    };
//  dbeditVersion[i][0] = pDBEDITVE[3*i  ];
//  dbeditVersion[i][1] = pDBEDITVE[3*i+1];
//  dbeditVersion[i][2] = pDBEDITVE[3*i+2];
//  if (pCORELVER)
//    corelVersions[i] = pCORELVER[i];
    if (pStartOffset)
      startOffsets[i] = pStartOffset[i];
    if (pStopOffset)
      stopOffsets[i] = pStopOffset[i];
    if (pHopsRevNum)
      hopsRevisionNumbers[i] = pHopsRevNum[i];
    if (pFOURFVER)
    {
      fourfitVersions[i][0] = pFOURFVER[2*i  ];
      fourfitVersions[i][1] = pFOURFVER[2*i+1];
    };
    //
    // doubles:
    effDurations->setElement(i, pEffectiveDuration[i]);
    if (pABASRATE)
      corrAprioriRates->setElement(i, pABASRATE[i]);
    if (pABASDEL)
      corrAprioriDelays->setElement(i, pABASDEL[i]);
    if (pABASACCE)
      corrAprioriAccs->setElement(i, pABASACCE[i]);
    percentOfDiscards->setElement(i, pDISCARD[i]);
    geocenterPhaseResids->setElement(i, pGeocResidPhase[i]);
    geocenterPhaseTotals->setElement(i, pGeocPhase[i]);
    probOfFalses->setElement(i, pProbFalseDetection[i]);
    incohAmps->setElement(i, pINCOHAMP[i]);
    incohAmps2->setElement(i, pINCOH2[i]);
    delayResids->setElement(i, pDELRESID[i]);
    for (int j=0; j<6; j++)
      searchParams->setElement(i,j, pSRCHPAR[6*i + j]);
    //
    if (pSBRESID)
      sbdResids->setElement(i, pSBRESID[i]);
    if (pRATRESID)
      rateResids->setElement(i, pRATRESID[i]);
    if (pStartSec)
    {
      if (40000.0 < (d=pStartSec[i]))
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoMk4(): got unexpected value for start second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      startSeconds->setElement(i, d);
    };
    if (pStopSec)
    {
      if (40000.0 < (d=pStopSec[i]))
      {
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::loadObsCorrelatorInfoMk4(): got unexpected value for stop second: " +
          QString("").setNum(d) + " for obs #" + QString("").setNum(i));
        d = 0.0;
      };        
      stopSeconds->setElement(i, d);
    };
    if (pDELTAEPO)
      deltaEpochs->setElement(i, pDELTAEPO[i]);
    if (pGeocSBD)
      geocenterSbDelays->setElement(i, pGeocSBD[i]);
    
    if (have2swapGeocMbdAndRate)
    {
      if (pGeocMBD)
        geocenterGrDelays->setElement(i, pGeocRate[i]);
      if (pGeocRate)
        geocenterDelayRates->setElement(i, pGeocMBD[i]);
    }
    else
    {
      if (pGeocMBD)
        geocenterGrDelays->setElement(i, pGeocMBD[i]);
      if (pGeocRate)
        geocenterDelayRates->setElement(i, pGeocRate[i]);
    };
    
    if (pQBFACTOR)
      uniformityFactors->setElement(i, pQBFACTOR[i]);

    if (pURVR)
    {
      urvrs->setElement(i,0,  pURVR[2*i  ]);
      urvrs->setElement(i,1,  pURVR[2*i+1]);
    };
    if (pIDELAY)
    {
      instrDelays->setElement(i,0,  pIDELAY[2*i  ]);
      instrDelays->setElement(i,1,  pIDELAY[2*i+1]);
    };
    if (pSTARELEV)
    {
      starElev->setElement(i,0,  pSTARELEV[2*i  ]);
      starElev->setElement(i,1,  pSTARELEV[2*i+1]);
    };
    if (pZDELAY)
    {
      zenithDelays->setElement(i,0,  pZDELAY[2*i  ]);
      zenithDelays->setElement(i,1,  pZDELAY[2*i+1]);
    };
    if (pCORCLOCK)
    {
      corrClocks->setElement(i,0,  pCORCLOCK[4*i    ]);
      corrClocks->setElement(i,1,  pCORCLOCK[4*i + 1]);
      corrClocks->setElement(i,2,  pCORCLOCK[4*i + 2]);
      corrClocks->setElement(i,3,  pCORCLOCK[4*i + 3]);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCorrelatorInfoMk4(): data loaded successfully from " + ncdf.getFileName());
  return true;
};




//
bool SgVgosDb::loadObsEditData(QVector<int>& delUFlag, QVector<int>& phsUFlag, QVector<int>& ratUFlag,
  QVector<int>& uAcSup)
{
  if (vEdit_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsEditData(): the vgosDb variable Edit is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vEdit_.getFileName());
  ncdf.getData();
  if (!checkFormat(*fcfEdit, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsEditData(): format check failed");
    return false;
  };
  const short                  *pDel=ncdf.lookupVar(fcDelayFlag.name())->data2short();
  const short                  *pRat=ncdf.lookupVar(fcRateFlag .name())->data2short();
  const short                  *pPha=ncdf.lookupVar(fcPhaseFlag.name())?
                                     ncdf.lookupVar(fcPhaseFlag.name())->data2short():NULL;
  const short                  *pUsS=ncdf.lookupVar(fcUserSup  .name())?
                                     ncdf.lookupVar(fcUserSup  .name())->data2short():NULL;
  delUFlag.resize(numOfObs_);
  phsUFlag.resize(numOfObs_);
  ratUFlag.resize(numOfObs_);
  if (inputFormatVersion_ == 1.001)
    uAcSup  .resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    delUFlag[i] = pDel[i];
    ratUFlag[i] = pRat[i];
    phsUFlag[i] = pPha?pPha[i]:0;
    if (inputFormatVersion_ == 1.001)
      uAcSup  [i] = pUsS?pUsS[i]:-32768;
  };
  //
  if (1.002 <= inputFormatVersion_)
    loadObsUserSup(uAcSup);
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsEditData(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsUserSup(QVector<int>& uAcSup)
{
  SgVdbVariable                &var=progSolveDescriptor_.vUserSup_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsUserSup(): the vgosDb variable UserSup is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfUserSup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsUserSup(): format check failed");
    return false;
  };
  const short                  *pUsS=ncdf.lookupVar(fcUserSup  .name())?
                                     ncdf.lookupVar(fcUserSup  .name())->data2short():NULL;
  if (!pUsS)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsUserSup(): nothing to get");
    return false;
  }
  uAcSup  .resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    uAcSup  [i] = pUsS[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsUserSup(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsNumGroupAmbigs(const QString &band, QVector<int> &numAmbigs, 
  QVector<int> &numSubAmbigs)
{
  numAmbigs   .clear();
  numSubAmbigs.clear();

  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsNumGroupAmbigs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vNumGroupAmbig_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsNumGroupAmbigs(): the vgosDb variable NumGroupAmbig for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  //
  if (checkFormat(fcfNumGroupAmbigE, ncdf))
  {
    const short                *pN=ncdf.lookupVar(fcNumGroupAmbig.name())->data2short();
    const short                *pS=ncdf.lookupVar(fcNumGroupSubAmbig.name())?
                                    ncdf.lookupVar(fcNumGroupSubAmbig.name())->data2short():NULL;
    numAmbigs   .resize(numOfObs_);
    if (pS)
      numSubAmbigs.resize(numOfObs_);
    for (int i=0; i<numOfObs_; i++)
    {
      numAmbigs   [i] = pN[i];
      if (pS)
        numSubAmbigs[i] = pS[i];
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadObsNumGroupAmbigs(): data " + (pS?"(E) ":"") + 
      "were successfully loaded from " + ncdf.getFileName());
    return true;
  }
  else if (!checkFormat(fcfNumGroupAmbig, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsNumGroupAmbigs(): format (E) check failed");
    return false;
  };
  const short                  *p=ncdf.lookupVar(fcNumGroupAmbig.name())->data2short();
  numAmbigs.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    numAmbigs[i] = p[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsNumGroupAmbigs(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsNumPhaseAmbigs(const QString &band, QVector<int> &numAmbigs)
{
  numAmbigs   .clear();
  //
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsNumPhaseAmbigs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vNumPhaseAmbig_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsNumPhaseAmbigs(): the vgosDb variable NumPhaseAmbig for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  //
  if (!checkFormat(fcfNumPhaseAmbig, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsNumPhaseAmbigs(): format check failed");
    return false;
  };
  const int                    *p=ncdf.lookupVar(fcNumPhaseAmbig.name())->data2int();
  numAmbigs.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    numAmbigs[i] = p[i];
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsNumPhaseAmbigs(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalIonGroup(const QString& band, SgMatrix*& ionCals, SgMatrix*& ionSigmas, 
  QVector<int> &ionDataFlag)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCalIonGroup(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_SlantPathIonoGroup_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCalIonGroup(): the vgosDb variable Cal_SlantPathIonoGroup for the band [" + band + 
      "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalSlantPathIonoGroup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalIonGroup(): format check failed");
    return false;
  };
  const double                 *pVal=ncdf.lookupVar(fcCalIonoGroup        .name())->data2double();
  const double                 *pSig=ncdf.lookupVar(fcCalIonoGroupSigma   .name())->data2double();
  const short                  *pFlg=ncdf.lookupVar(fcCalIonoGroupDataFlag.name())?
                                     ncdf.lookupVar(fcCalIonoGroupDataFlag.name())->data2short():NULL;
  ionCals   = new SgMatrix(numOfObs_, 2);
  ionSigmas = new SgMatrix(numOfObs_, 2);
  ionDataFlag.resize(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
  {
    ionCals->setElement  (i, 0,  pVal[2*i    ]);
    ionCals->setElement  (i, 1,  pVal[2*i + 1]);
    ionSigmas->setElement(i, 0,  pSig[2*i    ]);
    ionSigmas->setElement(i, 1,  pSig[2*i + 1]);
    ionDataFlag[i] = pFlg?pFlg[i]:0;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalIonGroup(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
// Load of Calibrations:
//
//
bool SgVgosDb::loadStdObsCalibration(SgMatrix*& cals, QString& origin,
  const QString& varName, const SgVdbVariable& odbV, const SgVgosDb::FmtChkVar& fc,
  const QList<SgVgosDb::FmtChkVar*>& fcf, bool isMandatory)
{
  origin = "";
  if (odbV.isEmpty())
  {
    logger->write(isMandatory?SgLogger::ERR:SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadStdObsCalibration(): the vgosDb variable " + varName + " is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + odbV.getFileName());
  ncdf.getData();
  if (!checkFormat(fcf, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStdObsCalibration(): format check failed");
    return false;
  };
  SgNcdfVariable               *nV=ncdf.lookupVar(fc.name());
  if (!nV)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadStdObsCalibration(): the variable " + fc.name() + " was not found in the file");
    return false;
  };
  int                           nRow=nV->dimensions().at(0)->getN();
  int                           nCol=nV->dimensions().size()==1?1:nV->dimensions().at(1)->getN();
  const double                 *p=nV->data2double();
  cals = new SgMatrix(nRow, nCol);
  for (int i=0; i<nRow; i++)
    for (int j=0; j<nCol; j++)
      cals->setElement(i, j,  p[nCol*i + j]);
  //
  //
  SgNcdfVariable               *svcV;
  if ( (svcV=ncdf.lookupServiceVar(SgNetCdf::svcDataOrigin)) )
  {
    const char                 *c=svcV->data2char();
    int                         l=svcV->dimensions().at(0)->getN();
    origin = QString::fromLatin1(c, l);
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadStdObsCalibration(): cannot find service variable \"" + SgNetCdf::svcDataOrigin + 
      "\" in " + ncdf.getFileName());
  //
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStdObsCalibration(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalEarthTide(SgMatrix*& cals, QString &kind)
{
  bool                          isOk;
  QString                       origin;
  kind = "";
  isOk = loadStdObsCalibration(cals, origin, "Cal-EarthTide", 
    vCal_EarthTide_, fcCalEarthTide, fcfCalEarthTide, false);
  kind = vCal_EarthTide_.getKind();
  return isOk;
};



//
bool SgVgosDb::loadObsCalFeedCorr(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-FeedCorrection", 
    vCal_FeedCorrection_, fcCalFeedCorrection, fcfCalFeedCorrection, false);
};



//
bool SgVgosDb::loadObsCalFeedCorr(const QString& band, SgMatrix*& contrib)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCalFeedCorr(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_FeedCorrection_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCalFeedCorr(): the vgosDb variable Cal_FeedCorrection for the band [" + band + 
      "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalFeedCorrection, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalFeedCorr(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcCalFeedCorrection.name())?
                                  ncdf.lookupVar(fcCalFeedCorrection.name())->data2double():NULL;
  if (!p)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCalFeedCorr(): cannot find the variable " + fcCalFeedCorrection.name() + " in the file");
    return false;
  };
  contrib = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    contrib->setElement(i,0,  p[2*i    ]);
    contrib->setElement(i,1,  p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalFeedCorr(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalHiFyErp(SgMatrix*& calsUt, SgMatrix*& calsXY, QString &kind)
{
  kind = "";
  if (vCal_HfErp_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCalHiFyErp(): the vgosDb variable Cal_HfErp is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vCal_HfErp_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalHiFyErp, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalHiFyErp(): format check failed");
    return false;
  };
  const double                 *pUt=ncdf.lookupVar(fcCalUt1Ortho.name())->data2double();
  const double                 *pPm=ncdf.lookupVar(fcCalWobOrtho.name())->data2double();
  calsUt = new SgMatrix(numOfObs_, 2);
  calsXY = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    calsUt->setElement(i,0,  pUt[2*i]);
    calsUt->setElement(i,1,  pUt[2*i + 1]);
    calsXY->setElement(i,0,  pPm[2*i]);
    calsXY->setElement(i,1,  pPm[2*i + 1]);
  };
  kind = vCal_HfErp_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalHiFyErp(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalOcean(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-OceanLoad", 
    vCal_OceanLoad_, fcCalOceanLoad, fcfCalOceanLoad, false);
};



//
bool SgVgosDb::loadObsCalOceanOld(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-OceanLoadOld", vCal_OceanLoadOld_, fcCalOceanLoadOld, 
    fcfCalOceanLoadOld, false);
};



//
bool SgVgosDb::loadObsCalOceanPoleTideLoad(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-OceanPoleTideLoad", vCal_OceanPoleTideLoad_, 
    fcCalOceanPoleTideLoad, fcfCalOceanPoleTideLoad, false);
};



//
bool SgVgosDb::loadObsCalPoleTide(SgMatrix*& cals, QString &kind)
{
  bool                          isOk;
  QString                       origin;
  kind = "";
  isOk = loadStdObsCalibration(cals, origin, "Cal-PoleTide", 
    vCal_PoleTide_, fcCalPoleTide, fcfCalPoleTide, false);
  kind = vCal_PoleTide_.getKind();
  return isOk;
};



//
bool SgVgosDb::loadObsCalPoleTideOldRestore(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-PoleTideOldRestore", 
    vCal_PoleTideOldRestore_, fcCalPoleTideOldRestore, fcfCalPoleTideOldRestore, false);
};



//
bool SgVgosDb::loadObsCalTiltRmvr(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-TiltRemover", 
    vCal_TiltRemover_, fcCalTiltRemover, fcfCalTiltRemover, false);
};



//
bool SgVgosDb::loadObsFractC(SgVector*& v)
{
  SgVdbVariable                &var=progSolveDescriptor_.vFractC_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsFractC(): the vgosDb variable FractC is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfFractC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsFractC(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcFractC.name())?
                                  ncdf.lookupVar(fcFractC.name())->data2double():NULL;
  if (!p)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsFractC(): nothing to get");
    return false;
  }
  v = new SgVector(numOfObs_);
  for (int i=0; i<numOfObs_; i++)
    v->setElement(i, p[i]);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsFractC(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalUnphase(const QString& band, SgMatrix*& unPhaseCal_1, SgMatrix*& unPhaseCal_2)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCalUnphase(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_Unphase_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCalUnphase(): the vgosDb variable Cal_Unphase for the band [" + band + "] is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalUnphase, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalUnphase(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcCalUnPhase.name())?
                                  ncdf.lookupVar(fcCalUnPhase.name())->data2double():NULL;
  if (!p)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadObsCalUnphase(): cannot find the variable " + fcCalUnPhase.name() + " in the file");
    return false;
  };
  unPhaseCal_1 = new SgMatrix(numOfObs_, 2);
  unPhaseCal_2 = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    unPhaseCal_1->setElement(i,0,  p[4*i    ]);
    unPhaseCal_2->setElement(i,0,  p[4*i + 1]);
    unPhaseCal_1->setElement(i,1,  p[4*i + 2]);
    unPhaseCal_2->setElement(i,1,  p[4*i + 3]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalUnphase(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalHiFyLibration(SgMatrix*& cals, QString &kind)
{
  bool                          isOk;
  QString                       origin;
  kind = "";
  isOk = loadStdObsCalibration(cals, origin, "Cal-HiFreqLibration",
    vCal_HfLibration_, fcCalHfLibration, fcfCalHfLibration, false);
  kind = vCal_HfLibration_.getKind();
  return isOk;
};




//
bool SgVgosDb::loadObsCalHiFyLibration(SgMatrix*& calsPm, SgMatrix*& calsUt, QString &kind)
{
  kind = "";
  if (vCal_HiFreqLibration_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
      "::loadObsCalHiFreqLibration(): the vgosDb variable Cal_HiFreqLibration is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vCal_HiFreqLibration_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalHiFreqLibration, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalHiFreqLibration(): format check failed");
    return false;
  };
  const double                 *pUt=ncdf.lookupVar(fcCalHiFreqLibrationUt.name())?
                                  ncdf.lookupVar(fcCalHiFreqLibrationUt.name())->data2double():NULL;
  const double                 *pPm=ncdf.lookupVar(fcCalHiFreqLibrationPm.name())?
                                  ncdf.lookupVar(fcCalHiFreqLibrationPm.name())->data2double():NULL;
  if (pUt)
    calsUt = new SgMatrix(numOfObs_, 2);
  if (pPm)
    calsPm = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    if (pUt)
    {
      calsUt->setElement(i,0,  pUt[2*i    ]);
      calsUt->setElement(i,1,  pUt[2*i + 1]);
    };
    if (pPm)
    {
      calsPm->setElement(i,0,  pPm[2*i    ]);
      calsPm->setElement(i,1,  pPm[2*i + 1]);
    };
  };
  kind = vCal_HiFreqLibration_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalHiFreqLibration(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalWobble(SgMatrix*& calsXwobble, SgMatrix*& calsYwobble)
{
  if (vCal_Wobble_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsCalWobble(): the vgosDb variable Cal-Wobble is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vCal_Wobble_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalWobble, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsCalWobble(): format check failed");
    return false;
  };
  const double                 *pX=ncdf.lookupVar(fcCalXwobble.name())->data2double();
  const double                 *pY=ncdf.lookupVar(fcCalYwobble.name())->data2double();
  calsXwobble = new SgMatrix(numOfObs_, 2);
  calsYwobble = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    calsXwobble->setElement(i,0,  pX[2*i    ]);
    calsXwobble->setElement(i,1,  pX[2*i + 1]);
    calsYwobble->setElement(i,0,  pY[2*i    ]);
    calsYwobble->setElement(i,1,  pY[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsCalWobble(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsCalBend(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-Bend", 
    vCal_Bend_, fcCalBend, fcfCalBend, false);
};



//
bool SgVgosDb::loadObsCalBendSun(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-BendSun", 
    vCal_BendSun_, fcCalBendSun, fcfCalBendSun, false);
};



//
bool SgVgosDb::loadObsCalBendSunHigher(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-BendSunHigher", 
    vCal_BendSunHigher_, fcCalBendSunHigher, fcfCalBendSunHigher, false);
};



//
bool SgVgosDb::loadObsCalParallax(SgMatrix*& cals)
{
  QString                       origin;
  return loadStdObsCalibration(cals, origin, "Cal-Parallax", 
    vCal_Parallax_, fcCalParallax, fcfCalParallax, false);
};



//
//
// Load of Partials:
//
//
bool SgVgosDb::loadObsPartBend(SgMatrix*& partBend)
{
  if (vPart_Bend_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartBend(): the vgosDb variable Part-Bend is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vPart_Bend_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_Bend, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartBend(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartBend.name())->data2double();
  partBend = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    partBend->setElement(i, 0,  p[2*i    ]);
    partBend->setElement(i, 1,  p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartBend(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartGamma(SgMatrix*& part)
{
  SgVdbVariable                &var=vPart_Gamma_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartGamma(): the vgosDb variable \"" + var.getStub() + "\" is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_Gamma, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartGamma(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartGamma.name())->data2double();
  part = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    part->setElement(i, 0,  p[2*i    ]);
    part->setElement(i, 1,  p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartGamma(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartParallax(SgMatrix*& part)
{
  SgVdbVariable                &var=vPart_Parallax_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartParallax(): the vgosDb variable \"" + var.getStub() + "\" is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_Parallax, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartParallax(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartParallax.name())->data2double();
  part = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    part->setElement(i, 0,  p[2*i    ]);
    part->setElement(i, 1,  p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartParallax(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartPoleTides(SgMatrix*& partX, SgMatrix*& partY, QString& kind)
{
  SgVdbVariable                &var=vPart_PoleTide_;
  kind = "";
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartPoleTides(): the vgosDb variable \"" + var.getStub() + "\" is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_PolarTide, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartPoleTides(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartPolarTide.name())->data2double();
  partX = new SgMatrix(numOfObs_, 2);
  partY = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    partX->setElement(i, 0, p[4*i    ]);
    partY->setElement(i, 0, p[4*i + 1]);
    partX->setElement(i, 1, p[4*i + 2]);
    partY->setElement(i, 1, p[4*i + 3]);
  };
  kind = var.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartPoleTides(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartEOP(SgMatrix*& dV_dPx, SgMatrix*& dV_dPy, SgMatrix*& dV_dUT1)
{
  if (vPart_Erp_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartEOP(): the vgosDb variable Part-Erp is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vPart_Erp_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_Erp, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartEOP(): format check failed");
    return false;
  };
  const double                 *pPm=ncdf.lookupVar(fcPartWobble.name())->data2double();
  const double                 *pUt=ncdf.lookupVar(fcPartUt1   .name())->data2double();
  dV_dPx  = new SgMatrix(numOfObs_, 2);
  dV_dPy  = new SgMatrix(numOfObs_, 2);
  dV_dUT1 = new SgMatrix(numOfObs_, 4);
  for (int i=0; i<numOfObs_; i++)
  {
    dV_dPx ->setElement(i, 0,   pPm[4*i    ]);
    dV_dPy ->setElement(i, 0,   pPm[4*i + 1]);
    dV_dPx ->setElement(i, 1,   pPm[4*i + 2]);
    dV_dPy ->setElement(i, 1,   pPm[4*i + 3]);

    dV_dUT1->setElement(i, 0,   pUt[4*i    ]);
    dV_dUT1->setElement(i, 1,   pUt[4*i + 1]);
    dV_dUT1->setElement(i, 2,   pUt[4*i + 2]);
    dV_dUT1->setElement(i, 3,   pUt[4*i + 3]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartEOP(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartNut2KXY(SgMatrix*& dV_dCipX, SgMatrix*& dV_dCipY, QString& kind)
{
  kind = "";
  if (vPart_NutationNro_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartNut2KXY(): the vgosDb variable Part-NutationNRO is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vPart_NutationNro_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_NutationNro, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartNut2KXY(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartNutationNro.name())?
                                  ncdf.lookupVar(fcPartNutationNro.name())->data2double():NULL;
  if (!p)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartNut2KXY(): cannot find variable " + fcPartNutationNro.name() + " in the file");
    return false;
  };
  dV_dCipX = new SgMatrix(numOfObs_, 2);
  dV_dCipY = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    dV_dCipX->setElement(i, 0,  p[2*2*i    ]);
    dV_dCipY->setElement(i, 0,  p[2*2*i + 1]);
    dV_dCipX->setElement(i, 1,  p[2*2*i + 2]);
    dV_dCipY->setElement(i, 1,  p[2*2*i + 3]);
  };
  kind = vPart_NutationNro_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartNut2KXY(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartRaDec(SgMatrix*& dV_dRA, SgMatrix*& dV_dDN)
{
  if (vPart_RaDec_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartRaDec(): the vgosDb variable Part-RaDec is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vPart_RaDec_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_RaDec, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartRaDec(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartRaDec.name())->data2double();
  dV_dRA = new SgMatrix(numOfObs_, 2);
  dV_dDN = new SgMatrix(numOfObs_, 2);
  for (int i=0; i<numOfObs_; i++)
  {
    dV_dRA->setElement(i, 0,   p[2*2*i    ]);
    dV_dDN->setElement(i, 0,   p[2*2*i + 1]);
    dV_dRA->setElement(i, 1,   p[2*2*i + 2]);
    dV_dDN->setElement(i, 1,   p[2*2*i + 3]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartRaDec(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsPartXYZ(SgMatrix*& dTau_dR_1, SgMatrix*& dRat_dR_1)
{
  if (vPart_Xyz_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsPartXYZ(): the vgosDb variable Part-XYZ is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vPart_Xyz_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPart_Xyz, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsPartXYZ(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartXyz.name())->data2double();
  dTau_dR_1 = new SgMatrix(numOfObs_, 3);
  dRat_dR_1 = new SgMatrix(numOfObs_, 3);
  for (int i=0; i<numOfObs_; i++)
  {
    dTau_dR_1->setElement(i, 0,   p[2*3*i        ]);
    dTau_dR_1->setElement(i, 1,   p[2*3*i     + 1]);
    dTau_dR_1->setElement(i, 2,   p[2*3*i     + 2]);
    dRat_dR_1->setElement(i, 0,   p[2*3*i + 3    ]);
    dRat_dR_1->setElement(i, 1,   p[2*3*i + 3 + 1]);
    dRat_dR_1->setElement(i, 2,   p[2*3*i + 3 + 2]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsPartXYZ(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadObsUserCorrections(int idx, SgMatrix* m, QString& name)
{
  if (idx<0 || obsUserCorrections_.size()-1<idx)
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsUserCorrections(): index " + QString("").setNum(idx) + " out of range");
    return false;
  };
  const SgVdbVariable          *var=obsUserCorrections_.at(idx);
  if (!var)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsUserCorrections(): the var #" + QString("").setNum(idx) + " is NULL");
    return false;
  };
  if (var->isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadObsUserCorrections(): the vgosDb variable " + var->getStub() + " is empty");
    return false;
  };
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var->getFileName());
  ncdf.getData();
  if (!checkFormat(fcfUserCorrections, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadObsUserCorrections(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcUserCorrections.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    m->setElement(i, idx, p[i]);
  };
  name = var->getFileName4Output();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadObsUserCorrections(): data loaded successfully from " + ncdf.getFileName());
  return true;
};




/*=====================================================================================================*/





/*=====================================================================================================*/
//
// Constants:
//
const QList<int>                
                                dim_Obs_x_Stn       (QList<int>() << SD_NumObs << 2),
                                dim_Obs_x_Chan_x_Stn(QList<int>() << SD_NumObs << SD_NumChans << 2);

// Variable descriptions:
SgVgosDb::FmtChkVar
  fcBaseline          ("Baseline",          NC_CHAR,    true, QList<int>() << SD_NumObs << 2 << 8,
    "BASELINE", "Ref and rem site names"),
  fcSource            ("Source",            NC_CHAR,    true, QList<int>() << SD_NumObs << 8,
    "STAR ID ", "Radio source name"),
  fcObs2Baseline      ("Obs2Baseline",      NC_SHORT,   true, QList<int>() << SD_NumObs << 2,
    "", "Cross reference from observation to baseline"),
  fcObs2Scan          ("Obs2Scan",          NC_INT,     true, QList<int>() << SD_NumObs,
    "", "Cross reference from observation to scan"),
  fcCrossRefSourceList("CrossRefSourceList",NC_CHAR,    true, QList<int>() << SD_NumSrc << 8,
    "", "A list of sources names"),
  fcScan2Source       ("Scan2Source",       NC_INT,     true, QList<int>() << SD_NumScans,
    "", "Cross reference scan to source"),
  fcNumScansPerStation("NumScansPerStation",NC_INT,     true, QList<int>() << SD_NumStn,
    "", "Number of scans per station."),
  fcCrossRefStationList
                      ("CrossRefStationList",NC_CHAR,   true, QList<int>() << SD_NumStn << 8,
    "", "Site names in alphabetical order"),
  fcStation2Scan      ("Station2Scan",      NC_INT,     true, QList<int>() << SD_Any << SD_NumStn,
    "", "Cross reference station-scan to schedule-scan"),
  fcScan2Station      ("Scan2Station",      NC_INT,     true, QList<int>() << SD_NumScans << SD_NumStn,
    "", "Cross reference scans to station"),
  fcSBDelay           ("SBDelay",           NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "SB DELAY", "Single band delay", "second", "?"),
  fcSBDelaySig        ("SBDelaySig",        NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "SB SIGMA", "Single band delay error", "second", "?"),
  fcGroupDelay        ("GroupDelay",        NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "DEL OBSV", "Delay observable produced by fringing", "second", "?"),
  fcGroupDelaySig     ("GroupDelaySig",     NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "DELSIGMA", "Delay Measurement Sigma", "second", "?"),
  fcGroupRate         ("GroupRate",         NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "RAT OBSV", "Rate Observable", "second/second", "?"),
  fcGroupRateSig      ("GroupRateSig",      NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "RATSIGMA", "Rate Measurement Sigma", "second/second", "?"),
  fcPhase             ("Phase",             NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "TOTPHASE", "Total phase", "radian", "?"),
  fcPhaseSig          ("PhaseSig",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "", "Total phase sigma", "radian", "?"),
  fcGroupDelayFull    ("GroupDelayFull",    NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "Delay Observable with ambiguities resolved and added.", "second", "?"),
  fcPhaseDelayFull    ("PhaseDelayFull",    NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "Phase Delay \"Observable\" with ambiguities resolved and added.", "second", "?"),
  fcPhaseDelaySigFull ("PhaseDelaySigFull", NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "Phase Delay Sigma", "second", "?"),
  fcRefFreq           ("RefFreq",           NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "REF FREQ", "Frequency to which phase is referenced", "MHz", "?"),
  fcFreqRateIon       ("FreqRateIono",      NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "", "Effective Group Rate Ionospheric Frequency", "MHz", "?"),
  fcFreqPhaseIon      ("FreqPhaseIono",     NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "PHIONFRQ", "Effective Phase Delay Ionospheric Frequency", "MHz", "?"),
  fcFreqGroupIon      ("FreqGroupIono",     NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "GRIONFRQ", "Effective Group Delay Ionospheric Frequency", "MHz", "?"),
  fcFreqRateIonEqWgts ("FreqRateIono",      NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "", "Effective Group Rate Ionospheric Frequency. All channels equal wt.", "MHz", "?"),
  fcFreqPhaseIonEqWgts("FreqPhaseIono",     NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "", "Effective Phase Delay Ionospheric Frequency. All channels equal wt.", "MHz", "?"),
  fcFreqGroupIonEqWgts("FreqGroupIono",     NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "", "Effective Group Delay Ionospheric Frequency. All channels equal wt.", "MHz", "?"),
  fcDtec              ("diffTec",           NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "Difference of the total electron contents", "TEC units"),
  fcDtecStdErr        ("diffTecStdDev",     NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "Standard deviation of dTec estimation", "TEC units"),
  fcAmbigSize         ("AmbigSize",         NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "GPDLAMBG", "Group delay ambiguity spacing", "second", "?"),
  fcQualityCode       ("QualityCode",       NC_CHAR,    true, QList<int>() << SD_NumObs,
    "QUALCODE", "FRNGE quality  index 0 --> 9", "", "?"),
  fcCorrelation       ("Correlation",       NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "COHERCOR", "Corr coeff (0 --> 1)", "", "?"),
  fcSNR               ("SNR",               NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "SNRATIO ", "Signal to noise  ratio", "", "?"),
  fcDelayDataFlag     ("DelayDataFlag",     NC_SHORT,   true, QList<int>() << SD_NumObs           ),
  fcDataFlag          ("DataFlag",          NC_SHORT,   false,QList<int>() << SD_NumObs,
    "", "0=OK, -1=Missing,  -2=bad,-3=sigma small, -4=sigma big", "", "?"),
  fcDelayTheoretical  ("DelayTheoretical",  NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "CONSNDEL", "Consensus theoretical delay", "second"),
  fcRateTheoretical   ("RateTheoretical",   NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "CONSNRAT", "Consensus theoretical rate", "second/second"),
  fcUVFperAsec        ("UVFperAsec",        NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "UVF/ASEC", "U V in FR per arcsec  from CALC", "", "?"),
  //
  //  begin of channel info:
  //
  fcChannelID         ("ChannelID",         NC_CHAR,    false,QList<int>() << SD_NumObs << SD_2NumChans,
    "CHAN ID ", "One-letter Fourfit channel ID...", "", "?"),
  fcPolarization      ("Polarization",      NC_CHAR,    false,QList<int>() << SD_NumObs << SD_NumChans 
                                                                                        << 4,
    "POLARIZ ", "Polarization per sta/chan R/L...", "", "?"),
  fcNumChannels       ("NumChannels",       NC_SHORT,   true, QList<int>() << SD_NumObs,
    "#CHANELS", "No. of U-L pairs in integration.", "", "?"),
  fcNumAp             ("NumAp",             NC_SHORT,   false,QList<int>() << SD_NumObs << SD_NumChans 
                                                                           << 2,
    "NO.OF AP", "# of AP by sideband and channel.", "", "?"),
  fcNumAccum          ("NumAccum",          NC_SHORT,   false,QList<int>() << SD_NumObs << 32,
    "VFRQ#APS", "No. of accum. periods in Channel (S2)", "", "?"),
  fcERRORATE          ("ERRORATE",          NC_SHORT,   false,QList<int>() << SD_NumObs << SD_NumChans
                                                                                        << 2 << 2,
    "ERRORATE", "Log err rate by sta, sb, channel", "", "?"),
  fcBITSAMPL          ("BITSAMPL",          NC_SHORT,   false,QList<int>() << SD_NumObs,
    "BITSAMPL", "Number of bits per sample.......", "", "?"),
  fcBBCIndex          ("BBCIndex",          NC_SHORT,   false,QList<int>() << SD_NumObs << SD_NumChans
                                                                                        << 2,
    "BBC IND ", "Physical BBC number by channel..", "", "?"),
  fcINDEXNUM          ("INDEXNUM",          NC_SHORT,   false,QList<int>() << SD_NumObs << SD_NumChans
                                                                                        << 2,
    "INDEXNUM", "Corel index numbers by sb,freq..", "", "?"),
//fcChannelFreq       ("ChannelFreq",       NC_DOUBLE,  true, QList<int>() << SD_NumObs << SD_NumChans,
  fcChannelFreq       ("ChannelFreq",       NC_DOUBLE,  false, QList<int>() << SD_NumObs << SD_NumChans,
    "RFREQ   ", "RF freq by channel (MHz)........", "MHz", "?"),
  fcVIRTFREQ          ("VIRTFREQ",          NC_DOUBLE,  true, QList<int>() << SD_NumObs << SD_NumChans,
    "VIRTFREQ", "Sky Frequencies", "MHz", "?"),

  fcLOFreq            ("LOFreq",            NC_DOUBLE,  false,QList<int>() << SD_NumObs << SD_NumChans
                                                                                        << 2,
    "LO FREQ ", "LO frequencies per cha/sta MHz", "MHz", "?"),
  fcChannelBandwidth  ("ChannelBandwidth",  NC_DOUBLE,  false,QList<int>() << SD_NumObs << SD_NumChans,
    "CHBWIDTH", "Channel bandwidth (MHz)", "MHz", "?"),

  fcChanAmpPhase      ("ChanAmpPhase",      NC_DOUBLE,  true, QList<int>() << SD_NumObs << SD_NumChans
                                                                           << 2,
    "AMPBYFRQ", "Amp(0-1), phs(-180to180) by chan", "", "?"),
  fcVFRQAM            ("VFRQAM",            NC_DOUBLE,  true, QList<int>() << SD_NumObs << SD_NumChans,
    "VFRQAM", "Normalized channel amplitude", "", "?"),
  fcVFRQPH            ("VFRQPH",            NC_DOUBLE,  true, QList<int>() << SD_NumObs << SD_NumChans,
    "VFRQPH", "Channel Phase (degrees)", "", "?"),

  fcSidebandOrder     ("SidebandOrder",     NC_SHORT,   false, QList<int>() << 1,
    "", "Sideband order: 0=Undefined, 1={USB,LSB}; 2={LSB,USB}", "", "?"),

  fcNumSamples        ("NumSamples",        NC_DOUBLE,  false,QList<int>() << SD_NumObs << SD_NumChans
                                                                           << 2,
    "#SAMPLES", "# of samples by sideband and cha", "", "?"),
  fcSampleRate        ("SampleRate",        NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "SAMPLRAT", "Sample rate (Hz)", "Hz", "?"),
  //
  //  begin of phase cal info:
  //
  fcPhaseCalRate      ("PhaseCalRate",      NC_DOUBLE,  true,   dim_Obs_x_Stn,
    "PHASECAL", "PC rate by sta ( us per s)......", "", "?"),
  fcPhaseCalFreq      ("PhaseCalFreq",      NC_SHORT,   false,  dim_Obs_x_Chan_x_Stn,
    "", "PhaseCalFreq by channel, Station", "", "?"),
  fcPhaseCalOffset    ("PhaseCalOffset",    NC_SHORT,   false,  dim_Obs_x_Chan_x_Stn,
    "PHCALOFF", "Phase cal offset (-18000/18000).", "centidegrees", "?"),
  fcPhaseCalPhase     ("PhaseCalPhase",     NC_SHORT,   true,   dim_Obs_x_Chan_x_Stn,
    "", "PhaseCalPhase by channel, Station(-18000 to 18000)", "centidegrees", "?"),
  fcPhaseCalAmp       ("PhaseCalAmp",       NC_SHORT,   true,   dim_Obs_x_Chan_x_Stn,
    "", "PhaseCalAmp by channel, Station", "", "?"),
  // phase cal data from S2:
  fcPhaseCalFreqS2    ("PhaseCalFreqS2",    NC_DOUBLE,  false,  dim_Obs_x_Chan_x_Stn,
    "VFRQPCFR", "Phase cal tone Frequencies", "", "?"),
  fcPhaseCalPhaseS2   ("PhaseCalPhaseS2",   NC_DOUBLE,  true,   dim_Obs_x_Chan_x_Stn,
    "VFRQPCPH", "PhaseCalPhase by channel, Station(-18000 to 18000)", "centidegrees", "?"),
  fcPhaseCalAmpS2     ("PhaseCalAmpS2",     NC_DOUBLE,  true,   dim_Obs_x_Chan_x_Stn,
    "VFRQPCAM", "Phase cal tone Amplitudes", "", "?"),
  //
  //  begin of correlator info:
  //
  fcFOURFFIL          ("FOURFFIL",          NC_CHAR,    false,QList<int>() << SD_NumObs << 16,
    "FOURFFIL", "Fourfit output filename", "", "?"),
  fcFRNGERR           ("FRNGERR",           NC_CHAR,    false,QList<int>() << SD_NumObs,
    "FRNGERR ", "Fourfit error flag blank=OK.....", "", "?"),
  fcFRNGERR4S2        ("FRNGERR",           NC_SHORT,   false,QList<int>() << SD_NumObs,
    "FRNGERR ", "Fourfit error flag blank=OK.", "", "?"),
  fcFRQGROUP          ("FRQGROUP",          NC_CHAR,    true, QList<int>() << SD_NumObs << 2,
    "FRQGROUP", "Frequency group code............", "", "?"),
  fcCORELVER          ("CORELVER",          NC_SHORT,   false,QList<int>() << SD_NumObs,
    "CORELVER", "Correlator software version numb", "", "?"),
  fcFOURFVER          ("FOURFVER",          NC_SHORT,   false,QList<int>() << SD_NumObs << 2,
    "FOURFVER", "Fourfit version number..........", "", "?"),
  fcDBEDITVE          ("DBEDITVE",          NC_SHORT,   false,QList<int>() << SD_NumObs << 3,
    "DBEDITVE", "Dbedit revision date YYYY MM DD", "", "?"),
  fcREFCLKER          ("REFCLKER",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "REFCLKER", "Ref sta clock epoch microsec....", "", "?"),
  fcStartOffset       ("StartOffset",       NC_SHORT,   false,QList<int>() << SD_NumObs,
    "STARTOFF", "Offset nominal start time (sec).", "", "?"),
  fcDELTAEPO          ("DELTAEPO",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "DELTAEPO", "Offset from center of scan (sec)", "", "?"),
  fcStopOffset        ("StopOffset",        NC_SHORT,   false,QList<int>() << SD_NumObs,
    "STOP OFF", "Offset nominal stop time (sec)..", "", "?"),
  fcHopsRevNum        ("HopsRevisionNumber",NC_SHORT,   false,QList<int>() << SD_NumObs,
    "", "HOPS software revision number", "", "?"),
  fcURVR              ("URVR",              NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "URVR    ", "Rate derivatives mHz per asec...", "", "?"),
  fcIDELAY            ("IDELAY",            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "IDELAY  ", "Corel instrumental delay (sec)..", "", "?"),
  fcSTARELEV          ("STARELEV",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "STARELEV", "Elev angles calc by COREL.......", "", "?"),
  fcZDELAY            ("ZDELAY",            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "ZDELAY  ", "Corel zenith atmos. delay (sec).", "", "?"),
  fcSRCHPAR           ("SRCHPAR",           NC_DOUBLE,  true, QList<int>() << SD_NumObs << 6,
    "SRCHPAR ", "FRNGE/Fourfit search parameters.", "", "?"),
  //
  fcCORBASCD          ("CORBASCD",          NC_CHAR,    false,QList<int>() << SD_NumObs << 2,
    "CORBASCD", "Correlator baseline code (2 ch).", "", "?"),
  fcTapeCode          ("TapeCode",          NC_CHAR,    false,QList<int>() << SD_NumObs << 6,
    "TAPQCODE", "Tape quality code...............", "", "?"),
  fcSBRESID           ("SBRESID",           NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "SBRESID ", "Single band delay residual......", "", "?"),
  fcRATRESID          ("RATRESID",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "RATRESID", "Rate resid (sec per sec)........", "", "?"),
  fcEffectiveDuration ("EffectiveDuration", NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "EFF.DURA", "Effective run duration sec......", "", "?"),
  fcStartSec          ("StartSec",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "STARTSEC", "Start time in sec past hour.....", "", "?"),
  fcStopSec           ("StopSec",           NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "STOP SEC", "Stop  time in sec past hour.....", "", "?"),
  fcDISCARD           ("DISCARD",           NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "DISCARD ", "Percent data discarded by FRNGE.", "", "?"),
  fcQBFACTOR          ("QBFACTOR",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "QBFACTOR", "Measure of uniformity of data...", "", "?"),
  fcGeocResidPhase    ("GeocResidPhase",    NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "GCRESPHS", "Resid phs corrected to cen of E.", "", "?"),
  fcGeocSBD           ("GeocSBD",           NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "GC SBD  ", "Tot geocenter sbd delay (sec)...", "", "?"),
  fcGeocRate          ("GeocRate",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "GC RATE ", "Tot geocenter delay rate (s/s)..", "", "?"),
  fcGeocMBD           ("GeocMBD",           NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "GC MBD  ", "Tot geocenter group delay (sec).", "second", "?"),
  fcProbFalseDetection("ProbFalseDetection",NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "FALSEDET", "Prob of false det from FRNGE....", "", "?"),
  fcABASRATE          ("ABASRATE",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "ABASRATE", "Corel bas/apr delay rate (s/s)..", "", "?"),
  fcABASDEL           ("ABASDEL",           NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "ABASDEL ", "Corel bas/apr delay (sec).......", "", "?"),
  fcABASACCE          ("ABASACCE",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "ABASACCE", "Corel bas/apr accel (1/sec**2)..", "", "?"),
  fcGeocPhase         ("GeocPhase",         NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "GC PHASE", "Tot phase ref to cen of Earth...", "", "?"),
  fcINCOHAMP          ("INCOHAMP",          NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "INCOHAMP", "Fr. amp from incoh int of chan..", "", "?"),
  fcINCOH2            ("INCOH2",            NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "INCOH2  ", "Incoh amp from FRNGE plot segs..", "", "?"),
  fcDELRESID          ("DELRESID",          NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "DELRESID", "Delay residual (sec)............", "", "?"),
  
  fcUTCCorr           ("UTCCorr",           NC_SHORT,   false,QList<int>() << SD_NumObs << 6,
    "CORR UTC", "UTC time tag of correlation.....", "", "?"),
  fcUTCMidObs         ("UTCMidObs",         NC_SHORT,   true, QList<int>() << SD_NumObs << 6,
    "UTCM TAG", "UTC at central epoch YMDHMS.....", "", "?"),
  fcFOURFUTC          ("FOURFUTC",          NC_SHORT,   false,QList<int>() << SD_NumObs << 6,
    "FOURFUTC", "Fourfit processing time YMDHMS..", "", "?"),
  fcUTCScan           ("UTCScan",           NC_SHORT,   false,QList<int>() << SD_NumObs << 6,
    "SCAN UTC", "Nominal scan time YMDHMS........", "", "?"),
  fcTAPEID            ("TAPEID",            NC_CHAR,    false,QList<int>() << SD_NumObs << 2 << 8,
    "TAPEID  ", "Raw data tape ID for ref and rem", "", "?"),
  fcCORCLOCK          ("CORCLOCK",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2 << 2,
    "CORCLOCK", "Clock offset(sec)/rate(sec/sec).", "", "?"),
  //
  // FmtChkVar(const char* name, nc_type type, bool isMandatory, const QList<int> l, 
  //  const QString& attLCode="", const QString& attDefinition="", const QString& attUnits="", 
  //  const QString& attBand="") 
  // extension from GSI(/Mk3?):
  fcRECSETUP          ("RECSETUP",          NC_SHORT,   false,QList<int>() << SD_NumObs << 3,
    "RECSETUP", "SAMP rate(KHz),FRAMES/PP,PP/AP..", "", "?"),
  fcVLB1FILE          ("VLB1FILE",          NC_CHAR,    false,QList<int>() << SD_NumObs << 6,
    "VLB1FILE", "Correlator file name ...........", "", "?"),
  fcDLYEPOp1          ("DLYEPO+1",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "DLYEPO+1", "Phase delay at epoch+1 usec ....", "", "?"),
  fcDLYEPOCH          ("DLYEPOCH",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "DLYEPOCH", "Phase delay at epoch   usec ....", "", "?"),
  fcDLYEPOm1          ("DLYEPO-1",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "DLYEPO-1", "Phase delay at epoch-1 usec ....", "", "?"),
  fcAPCLOFST          ("APCLOFST",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "APCLOFST", "Apriori clock offset microsec ..", "", "?"),
  fcTOTPCENT          ("TOTPCENT",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "TOTPCENT", "TOT phase at central epoch .....", "", "?"),
  fcRATOBSVM          ("RATOBSVM",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "RATOBSVM", "OBS rate at central epoch ......", "", "?"),
  fcUTCVLB2           ("UTCVLB2",           NC_SHORT,   false,QList<int>() << SD_NumObs << 6,
    "VLB2 UTC", "UTC of frnge PROC YMDHMS........", "", "?"),
  fcDELOBSVM          ("DELOBSVM",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "DELOBSVM", "OBS delay at central epoch uS ..", "", "?"),
  //
  // for the case when K5 data were prepared with dbedit:
  fcDLYEPOp1_V2       ("DLYEPO+1",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "DLYEPO+1", "Phase delay at epoch+1 usec ....", "", "?"),
  fcDLYEPOCH_V2       ("DLYEPOCH",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "DLYEPOCH", "Phase delay at epoch   usec ....", "", "?"),
  fcDLYEPOm1_V2       ("DLYEPO-1",          NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "DLYEPO-1", "Phase delay at epoch-1 usec ....", "", "?"),


  //  
  // S2 artefacts:
  fcUTCErr            ("UTCErr",            NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "UTC ERR ", "A priori UTC error site 1 (sec)", "", "?"),
  fcVFDWELL           ("VFDWELL",           NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "VFDWELL ", "Dwell time in each channel (sec)", "", "?"),
  fcS2EFFREQ          ("S2EFFREQ",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "S2EFFREQ", "Effective group freq for ion.", "", "?"),
  fcS2REFREQ          ("S2REFREQ",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "S2REFREQ", "Effective frequency for rate", "", "?"),
  fcS2PHEFRQ          ("S2PHEFRQ",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "S2PHEFRQ", "Effective phase frequency", "", "?"),
  fcTotalFringeErr    ("TotalFringeErr",    NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "TPHA ERR", "Total fringe phase error (deg)", "", "?"),
  fcDURATION          ("DURATION",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "TPHA ERR", "Scan duration (sec).", "", "?"),
  fcTimeSinceStart    ("TimeSinceStart",    NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "STR TIME", "Interval since start time (sec).", "", "?"),

 
  
  //
  // end of correlator info
  //
  fcCalEarthTide      ("Cal-EarthTide",     NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "ETD CONT", "Earth tide contributions def."),
  fcCalFeedCorrection ("Cal-FeedCorrection",NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "FEED.COR", "Feedhorn corr. in CORFIL scheme"),
  fcCalUt1Ortho       ("Cal-UT1Ortho",      NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "UT1ORTHO", "ORTHO_EOP Tidal UT1 contribution"),
  fcCalWobOrtho       ("Cal-WobOrtho",      NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "WOBORTHO", "ORTHO_EOP tidal wobble contribtn"),
  fcCalOceanLoad      ("Cal-OceanLoad",     NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "OCE CONT", "Obs dependent ocean loading"),
  fcCalOceanLoadOld   ("Cal-OceanLoadOld",  NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "OCE_OLD ", "Add to Cal-OceanLoad to get Cal10 OceanLoading"),
  fcCalOceanPoleTideLoad ("Cal-OceanPoleTideLoad",
                                            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "OPTLCONT", "Ocean Pole Tide Load Contribution"),
  fcCalPoleTide       ("Cal-PoleTide",      NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "PTD CONT", "Pole tide contributions def."),
  fcCalPoleTideOldRestore("Cal-PoleTideOldRestore",
                                            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "PTOLDCON", "Old Pole Tide Restorer Contrib."),
  fcCalTiltRemover    ("Cal-TiltRemover",   NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "TILTRMVR", "Axis Tilt Contribution Remover"),
  fcCalUnPhase        ("Cal-UnPhase",       NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2 << 2,
    "UNPHASCL", "UnPhaseCal effect - group&rate"),
  fcCalHfLibration    ("Cal-HiFreqLibrationPM",
                                            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "", ""),
  fcCalHiFreqLibrationPm
                      ("Cal-HiFreqLibrationPM",
                                            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "WOBLIBRA", "Hi Freq Wobble Libration Contribution"),
  fcCalHiFreqLibrationUt
                      ("Cal-HiFreqLibrationUT1",
                                            NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2,
    "UT1LIBRA", "Hi Freq UT1 Libration Contribution"),
  fcCalXwobble        ("Cal-Xwobble",       NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "WOBXCONT", "X Wobble contribution definition"),
  fcCalYwobble        ("Cal-Ywobble",       NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "WOBYCONT", "Y Wobble contribution definition"),
  fcCalBend           ("Cal-Bend",          NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "CON CONT", "Consensus bending contrib. (sec)"),
  fcCalBendSun        ("Cal-BendSun",       NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "SUN CONT", "Consensus bending contrib. (sec)"),
  fcCalBendSunHigher  ("Cal-BendSunHigher", NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "SUN2CONT", "High order bending contrib.(sec)"),
  fcCalParallax       ("Cal-Parallax",      NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "PLX1PSEC", "Parallax partial/contr  1 parsec"),
  
  //
  //
  fcPartBend          ("Part-Bend",         NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "BENDPART", "Grav. bend. partial w.r.t. Gamma"),
  fcPartWobble        ("Part-Wobble",       NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2 << 2,
    "WOB PART", "Wobble partial derivatives def."),
  fcPartUt1           ("Part-UT1",          NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2 << 2,
    "UT1 PART", "UT1 partial derivatives def."),
  fcPartGamma         ("Part-Gamma",        NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "CONSPART", "Consensus partial w.r.t. Gamma"),
  fcPartNutationNro   ("Part-NutationNRO",  NC_DOUBLE,  false,QList<int>() << SD_NumObs << 2 << 2,
    "NUT06XYP", "2000/2006 Nut/Prec X,Y Partials"),
  fcPartRaDec         ("Part-RaDec",        NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2 << 2,
    "STR PART", "Star partial derivatives def."),
  fcPartXyz           ("Part-XYZ",          NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2 << 3,
    "SIT PART", "Site partials: dtau/dr_1=-dtau/dr_2"),
  fcPartParallax      ("Part-Parallax",     NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2,
    "PLX PART", "Parallax partial deriv. def."),
  fcPartPolarTide     ("Part-PoleTide",     NC_DOUBLE,  true, QList<int>() << SD_NumObs << 2 << 2,
    "PTDXYPAR", "Pole Tide Partials w.r.t. X & Y"),
  fcFractC            ("FractC",            NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "CT SITE1", "Coordinate time at site 1", "day"),
  //
  fcUserCorrections   ("Corrections",       NC_DOUBLE,  true, QList<int>() << SD_NumObs,
    "", "")
  ;




// netCDF files:
QList<SgVgosDb::FmtChkVar*>
  fcfBaseline     = QList<SgVgosDb::FmtChkVar*>() << &fcBaseline,
  fcfSource       = QList<SgVgosDb::FmtChkVar*>() << &fcSource,
  fcfObsCrossRef  = QList<SgVgosDb::FmtChkVar*>() << &fcObs2Baseline << &fcObs2Scan,
  fcfSourceCrossRef
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCrossRefSourceList << &fcScan2Source,
  fcfStationCrossRef
                  = QList<SgVgosDb::FmtChkVar*>() << &fcNumScansPerStation << &fcCrossRefStationList
                                                  << &fcStation2Scan << &fcScan2Station,
  fcfSBDelay      = QList<SgVgosDb::FmtChkVar*>() << &fcSBDelay << &fcSBDelaySig,
  fcfGroupDelay   = QList<SgVgosDb::FmtChkVar*>() << &fcGroupDelay << &fcGroupDelaySig,
  fcfGroupRate    = QList<SgVgosDb::FmtChkVar*>() << &fcGroupRate << &fcGroupRateSig,
  fcfPhase        = QList<SgVgosDb::FmtChkVar*>() << &fcPhase << &fcPhaseSig,
  fcfGroupDelayFull
                  = QList<SgVgosDb::FmtChkVar*>() << &fcGroupDelayFull,
  fcfPhaseDelayFull
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPhaseDelayFull << &fcPhaseDelaySigFull,
  fcfRefFreq      = QList<SgVgosDb::FmtChkVar*>() << &fcRefFreq,
  fcfEffFreq      = QList<SgVgosDb::FmtChkVar*>() << &fcFreqRateIon << &fcFreqPhaseIon 
                                                  << &fcFreqGroupIon,
  fcfEffFreqEqWgts= QList<SgVgosDb::FmtChkVar*>() << &fcFreqRateIonEqWgts << &fcFreqPhaseIonEqWgts 
                                                  << &fcFreqGroupIonEqWgts,
  fcfDTec         = QList<SgVgosDb::FmtChkVar*>() << &fcDtec << &fcDtecStdErr,
  fcfAmbigSize    = QList<SgVgosDb::FmtChkVar*>() << &fcAmbigSize,
  fcfQualityCode  = QList<SgVgosDb::FmtChkVar*>() << &fcQualityCode,
  fcfCorrelation  = QList<SgVgosDb::FmtChkVar*>() << &fcCorrelation,
  fcfUVFperAsec   = QList<SgVgosDb::FmtChkVar*>() << &fcUVFperAsec,
  fcfSNR          = QList<SgVgosDb::FmtChkVar*>() << &fcSNR,
  fcfDelayDataFlag= QList<SgVgosDb::FmtChkVar*>() << &fcDelayDataFlag,
  fcfDataFlag     = QList<SgVgosDb::FmtChkVar*>() << &fcDataFlag,
  fcfDelayTheoretical
                  = QList<SgVgosDb::FmtChkVar*>() << &fcDelayTheoretical,
  fcfRateTheoretical
                  = QList<SgVgosDb::FmtChkVar*>() << &fcRateTheoretical,
  //
  // channel info (what we expect to read):
  //
  //
  fcfChannelInfoInptMk3
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate 
    << &fcSidebandOrder,
  fcfChannelInfoInptVLBA                                              // == fcfChannelInfoInptMk3
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate
    << &fcSidebandOrder,
  fcfChannelInfoInptCRL                                               // == fcfChannelInfoInptMk3
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate
    << &fcSidebandOrder,
  fcfChannelInfoInptGSI                                               // == fcfChannelInfoInptMk3
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate
    << &fcSidebandOrder,
  fcfChannelInfoInptS2
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcSampleRate << &fcVFRQAM << &fcVFRQPH 
    << &fcNumAccum << &fcVIRTFREQ << &fcSidebandOrder,
  fcfChannelInfoInptMk4
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate << &fcNumSamples
    << &fcSidebandOrder,
  fcfChannelInfoInptHaystack                                          // == fcfChannelInfoInptMk4
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate << &fcNumSamples
    << &fcSidebandOrder,
  fcfChannelInfoInptDifx                                              // == fcfChannelInfoInptMk4
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate << &fcNumSamples
    << &fcSidebandOrder,
  //
  // what to do with these:
  // cannot find the variable "NumAp" in the file 2002/02OCT09XG/Observables/ChannelInfo_bX.nc
  // cannot find the variable "NumAp" in the file 2002/02SEP11XG/Observables/ChannelInfo_bX.nc
  //
  //         short NumAccum(NumObs, Dim000032) ;
  //                NumAccum:LCODE = "VFRQ#APS" ;
  //                NumAccum:CreateTime = "2002/12/09 14:10:54 " ;
  //                NumAccum:Band = "X" ;
  //                NumAccum:Definition = "No. of accum. periods in Channel (S2)" ;
  //
  //
  // Channel info for MK4 type of correlator output:
  fcfChannelInfoMk4 = QList<SgVgosDb::FmtChkVar*>() 
    << &fcChannelID << &fcPolarization << &fcNumChannels << &fcNumAp << &fcERRORATE << &fcBITSAMPL
    << &fcBBCIndex << &fcINDEXNUM << &fcChannelFreq << &fcLOFreq << &fcChanAmpPhase << &fcNumSamples
    << &fcSampleRate << &fcChannelBandwidth << &fcSidebandOrder,
  // Channel info for KOMB type of correlator output:
  fcfChannelInfoKomb = QList<SgVgosDb::FmtChkVar*>()
    << &fcNumChannels << &fcNumAp << &fcChannelFreq << &fcChanAmpPhase << &fcSampleRate 
    << &fcChannelBandwidth << &fcSidebandOrder
    //??<< &fcERRORATE
    ,
  //
  // ------------------------------------------------------------------
  //
  fcfPhaseCalInfo = QList<SgVgosDb::FmtChkVar*>()
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalOffset << &fcPhaseCalRate,

  fcfPhaseCalInfoMk3 
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalRate,
  fcfPhaseCalInfoVLBA
                  = QList<SgVgosDb::FmtChkVar*>()                               //==fcfCorrInfoMk3
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalRate,
  fcfPhaseCalInfoCRL
                  = QList<SgVgosDb::FmtChkVar*>()                               //==fcfCorrInfoMk3
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalRate,
  fcfPhaseCalInfoGSI
                  = QList<SgVgosDb::FmtChkVar*>()                               //==fcfCorrInfoMk3
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalRate,
  fcfPhaseCalInfoS2
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcPhaseCalFreqS2 << &fcPhaseCalAmpS2 << &fcPhaseCalPhaseS2 << &fcPhaseCalRate,
  fcfPhaseCalInfoMk4
                  = QList<SgVgosDb::FmtChkVar*>()
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalOffset << &fcPhaseCalRate,
  fcfPhaseCalInfoHaystack
                  = QList<SgVgosDb::FmtChkVar*>()                               //==fcfCorrInfoMk4
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalOffset << &fcPhaseCalRate,
  fcfPhaseCalInfoDifx
                  = QList<SgVgosDb::FmtChkVar*>()                               //==fcfCorrInfoMk4
    << &fcPhaseCalFreq << &fcPhaseCalAmp << &fcPhaseCalPhase << &fcPhaseCalOffset << &fcPhaseCalRate,
  //
  // ------------------------------------------------------------------
  //
  // correlator info (what we expect to read):
  fcfCorrInfoInptMk3
                  = QList<SgVgosDb::FmtChkVar*>() << &fcVLB1FILE << &fcFRNGERR 
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptVLBA     //==fcfCorrInfoMk3
                  = QList<SgVgosDb::FmtChkVar*>() << &fcVLB1FILE << &fcFRNGERR
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptCRL
                  = QList<SgVgosDb::FmtChkVar*>() << &fcVLB1FILE << &fcFRNGERR4S2
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptGSI      // ==fcfCorrInfoInptCRL
                  = QList<SgVgosDb::FmtChkVar*>() << &fcVLB1FILE << &fcFRNGERR4S2
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptMk4
                  = QList<SgVgosDb::FmtChkVar*>() << &fcFOURFFIL << &fcFRNGERR
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptHaystack //==fcfCorrInfoMk4
                  = QList<SgVgosDb::FmtChkVar*>() << &fcFOURFFIL << &fcFRNGERR
                      << &fcStartSec << &fcStopSec,
  fcfCorrInfoInptDifx     //==fcfCorrInfoMk4
                  = QList<SgVgosDb::FmtChkVar*>() << &fcFOURFFIL << &fcFRNGERR
                      << &fcStartSec << &fcStopSec,
  //
  // Info for MK4 type of correlator:
  fcfCorrInfoMk4  = QList<SgVgosDb::FmtChkVar*>() 
    << &fcFOURFFIL << &fcFRNGERR << &fcFRQGROUP 
    << &fcCORELVER << &fcStartOffset << &fcFOURFVER << &fcDBEDITVE << &fcDELTAEPO
    << &fcURVR << &fcIDELAY << &fcSTARELEV << &fcZDELAY << &fcSRCHPAR << &fcCORBASCD << &fcStopOffset
    << &fcHopsRevNum << &fcTapeCode << &fcSBRESID << &fcRATRESID << &fcEffectiveDuration << &fcStartSec
    << &fcDISCARD << &fcQBFACTOR << &fcGeocResidPhase << &fcGeocSBD << &fcGeocRate << &fcGeocMBD
    << &fcProbFalseDetection << &fcABASRATE << &fcABASDEL << &fcABASACCE << &fcStopSec << &fcGeocPhase
    << &fcINCOHAMP << &fcINCOH2 << &fcDELRESID << &fcUTCCorr << &fcUTCMidObs << &fcFOURFUTC << &fcUTCScan
    << &fcTAPEID << &fcCORCLOCK,
  //
  // Info for KOMB type of correlator:
  fcfCorrInfoKomb = QList<SgVgosDb::FmtChkVar*>() 
    << &fcFOURFFIL << &fcFRNGERR4S2 << &fcFRQGROUP << &fcDBEDITVE << &fcDELTAEPO
    << &fcSRCHPAR << &fcCORBASCD << &fcTapeCode << &fcSBRESID << &fcRATRESID << &fcEffectiveDuration
    << &fcStartSec << &fcDISCARD << &fcGeocResidPhase << &fcProbFalseDetection << &fcABASRATE
    << &fcABASDEL << &fcABASACCE << &fcStopSec << &fcGeocPhase << &fcINCOHAMP << &fcINCOH2 << &fcDELRESID
    << &fcUTCCorr << &fcUTCMidObs << &fcRECSETUP << &fcVLB1FILE << &fcDLYEPOp1 << &fcDLYEPOCH
    << &fcDLYEPOm1 << &fcAPCLOFST << &fcTOTPCENT << &fcRATOBSVM << &fcUTCVLB2 << &fcDELOBSVM,
  //
  // KOMB type correlator, databases created with dbedit:
  fcfCorrInfoKomb_V2 = QList<SgVgosDb::FmtChkVar*>() 
    << &fcFOURFFIL << &fcFRNGERR4S2 << &fcFRQGROUP << &fcDBEDITVE << &fcDELTAEPO
    << &fcSRCHPAR << &fcCORBASCD << &fcTapeCode << &fcSBRESID << &fcRATRESID << &fcEffectiveDuration
    << &fcStartSec << &fcDISCARD << &fcGeocResidPhase << &fcProbFalseDetection << &fcABASRATE
    << &fcABASDEL << &fcABASACCE << &fcStopSec << &fcGeocPhase << &fcINCOHAMP << &fcINCOH2 << &fcDELRESID
    << &fcUTCCorr << &fcUTCMidObs << &fcRECSETUP << &fcVLB1FILE << &fcDLYEPOp1_V2 << &fcDLYEPOCH_V2
    << &fcDLYEPOm1_V2 << &fcAPCLOFST << &fcTOTPCENT << &fcRATOBSVM << &fcUTCVLB2 << &fcDELOBSVM,

  //
  // S2:
  fcfCorrInfoInptS2 = QList<SgVgosDb::FmtChkVar*>() 
    << &fcVLB1FILE << &fcRECSETUP << &fcSBRESID << &fcRATRESID << &fcDELRESID << &fcStartSec 
    << &fcStopSec << &fcGeocPhase << &fcUTCErr << &fcVFDWELL << &fcS2EFFREQ << &fcS2REFREQ
    << &fcS2PHEFRQ << &fcTotalFringeErr << &fcDURATION << &fcTimeSinceStart

//  << &fcFRNGERR4S2
    

    ,


  //
  //
  fcfCalEarthTide = QList<SgVgosDb::FmtChkVar*>() << &fcCalEarthTide,
  fcfCalFeedCorrection
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalFeedCorrection,
  fcfCalHiFyErp   = QList<SgVgosDb::FmtChkVar*>() << &fcCalUt1Ortho << &fcCalWobOrtho,
  fcfCalOceanLoad = QList<SgVgosDb::FmtChkVar*>() << &fcCalOceanLoad,
  fcfCalOceanLoadOld
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalOceanLoadOld,
  fcfCalOceanPoleTideLoad
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalOceanPoleTideLoad,
  fcfCalPoleTide  = QList<SgVgosDb::FmtChkVar*>() << &fcCalPoleTide,
  fcfCalPoleTideOldRestore
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalPoleTideOldRestore,
  fcfCalTiltRemover
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalTiltRemover,
  fcfCalUnphase   = QList<SgVgosDb::FmtChkVar*>() << &fcCalUnPhase,
  fcfCalHfLibration
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalHfLibration,
  fcfCalHiFreqLibration
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalHiFreqLibrationUt << &fcCalHiFreqLibrationPm,
  fcfCalWobble    = QList<SgVgosDb::FmtChkVar*>() << &fcCalXwobble << &fcCalYwobble,
  fcfCalBend      = QList<SgVgosDb::FmtChkVar*>() << &fcCalBend,
  fcfCalBendSun   = QList<SgVgosDb::FmtChkVar*>() << &fcCalBendSun,
  fcfCalBendSunHigher
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalBendSunHigher,
  fcfCalParallax  = QList<SgVgosDb::FmtChkVar*>() << &fcCalParallax,

  fcfPart_Bend    = QList<SgVgosDb::FmtChkVar*>() << &fcPartBend,
  fcfPart_Erp     = QList<SgVgosDb::FmtChkVar*>() << &fcPartWobble << &fcPartUt1,
  fcfPart_Gamma   = QList<SgVgosDb::FmtChkVar*>() << &fcPartGamma,
  fcfPart_NutationNro
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartNutationNro,
  fcfPart_RaDec   = QList<SgVgosDb::FmtChkVar*>() << &fcPartRaDec,
  fcfPart_Xyz     = QList<SgVgosDb::FmtChkVar*>() << &fcPartXyz,
  fcfUserCorrections
                  = QList<SgVgosDb::FmtChkVar*>() << &fcUserCorrections,
  fcfPart_Parallax= QList<SgVgosDb::FmtChkVar*>() << &fcPartParallax,
  fcfPart_PolarTide
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartPolarTide,
  fcfFractC       = QList<SgVgosDb::FmtChkVar*>() << &fcFractC
  ;

  
/*=====================================================================================================*/

