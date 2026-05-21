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
#include <QtCore/QRegularExpression>


#include <SgLogger.h>
#include <SgTaskConfig.h>
#include <SgTaskManager.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservable.h>
#include <SgVlbiObservation.h>




bool doubleSortingOrderLessThan(double a, double b);


/*=======================================================================================================
*
*               SgVlbiBand's METHODS:
* 
*======================================================================================================*/
// An empty constructor:
SgVlbiBand::SgVlbiBand() :
  SgObjectInfo(-1, "_"),
  observables_(),
  history_(),
  stationsByName_(),
  baselinesByName_(),
  sourcesByName_(),
  stationsByIdx_(),
  baselinesByIdx_(),
  sourcesByIdx_(),
  sampleRateByCount_(),
  bitsPerSampleByCount_(),
  recordingMode_(""),
  grdAmbigsBySpacing_(),
  strGrdAmbigsStat_(""),
  phdAmbigsBySpacing_(),
  strPhdAmbigsStat_(""),
  phCalOffset_1ByBln_(),
  phCalOffset_2ByBln_()
{
  frequency_ = 0.0;
  tCreation_ = tZero;
  inputFileName_ = "null";
  inputFileVersion_ = 0;
  correlatorType_ = "Mk0";
  maxNumOfChannels_ = 0;
  sampleRate_ = 0.0;
  bitsPerSample_ = 0;
  meanEffectiveIntegrationTime_ = 0.0;
  typicalGrdAmbigSpacing_ = 0.0;
  typicalPhdAmbigSpacing_ = 0.0;
};



// destructor:
SgVlbiBand::~SgVlbiBand()
{
  stationsByIdx_.clear();
  baselinesByIdx_.clear();
  sourcesByIdx_.clear();
  sampleRateByCount_.clear();
  bitsPerSampleByCount_.clear();
  grdAmbigsBySpacing_.clear();
  phdAmbigsBySpacing_.clear();
  phCalOffset_1ByBln_.clear();
  phCalOffset_2ByBln_.clear();

  // clear maps:
  for (QMap<QString, SgVlbiStationInfo*>::iterator it=stationsByName_.begin(); 
    it != stationsByName_.end(); ++it)
    delete it.value();
  stationsByName_.clear();

  for (QMap<QString, SgVlbiBaselineInfo*>::iterator it=baselinesByName_.begin(); 
    it != baselinesByName_.end(); ++it)
    delete it.value();
  baselinesByName_.clear();

  for (QMap<QString, SgVlbiSourceInfo*>::iterator it=sourcesByName_.begin(); 
    it != sourcesByName_.end(); ++it)
    delete it.value();
  sourcesByName_.clear();

  // clear observations container:
  observables_.clear();
};



//
void SgVlbiBand::resetAllEditings()
{
  // attributes:
  delAttr(Attr_NOT_VALID);
  //
  for (QMap<QString, SgVlbiStationInfo*>::iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    it.value()->resetAllEditings();
  for (QMap<QString, SgVlbiSourceInfo*>::iterator it=sourcesByName_.begin(); 
    it!=sourcesByName_.end(); ++it)
    it.value()->resetAllEditings();
  for (QMap<QString, SgVlbiBaselineInfo*>::iterator it=baselinesByName_.begin(); 
    it!=baselinesByName_.end(); ++it)
    it.value()->resetAllEditings();
  //
  SgObjectInfo::resetAllEditings();
};



//
bool SgVlbiBand::saveIntermediateResults(QDataStream& s) const
{
  s << getKey() << getAttributes() << getSigma2add(DT_DELAY);
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  //
  for (QMap<QString, SgVlbiStationInfo*>::const_iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    if (s.status() == QDataStream::Ok)
    {
      s << it.value()->getKey(); // put a station name to check order
      it.value()->clockBreaks().saveIntermediateResults(s);
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": saveIntermediateResults(): error writting clock break data: " + it.value()->getKey());
      return false;
    };
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiBand::loadIntermediateResults(QDataStream& s)
{
  QString                       key;
  unsigned int                  attributes;
  double                        sigma2add;
  s >> key >> attributes >> sigma2add;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (getKey() != key)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: wrong order, key mismatch: got [" + key +
      "], expected [" + getKey() + "]");
    return false;
  };

  for (QMap<QString, SgVlbiStationInfo*>::iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    if (s.status() == QDataStream::Ok)
    {
      s >> key; // first, check the order:
      if (it.value()->getKey() != key)
      {
        logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
          ": loadIntermediateResults(): error reading data: wrong order, key mismatch: got [" + key +
          "], expected [" + it.value()->getKey() + "]");
        return false;
      }
      if (!it.value()->clockBreaks().loadIntermediateResults(s))
      {
        logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
          ": loadIntermediateResults(): error reading data: clock break data for " +
          it.value()->getKey());
        return false;
      }
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error writting clock break data for " + it.value()->getKey() + 
        ": " + (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  setAttributes(attributes);
  setSigma2add(DT_DELAY, sigma2add);
  //
  return s.status()==QDataStream::Ok;
};



//
bool SgVlbiBand::selfCheck()
{
  QMap<QString, QMap<QString, int> >
                                numByScanIdBySrc;
  QString                       str("");
  QList<double>                 effIntgrTimes;
  bool                          isOk=true;
  int                           numOfZCodes=0;
  int                           num;
  maxNumOfChannels_ = 0;
  sampleRate_ = 0.0;
  bitsPerSample_ = 0;
  meanEffectiveIntegrationTime_ = 0.0;
  sampleRateByCount_.clear();
  channelBandwidthByCount_.clear();
  bitsPerSampleByCount_.clear();
  grdAmbigsBySpacing_.clear();
  phdAmbigsBySpacing_.clear();
  
  // set up "typical" group delay ambiguity spacing for the band:
  for (int i=0; i<observables_.size(); i++)
  {
    SgVlbiObservable           *o=observables_.at(i);
    grdAmbigsBySpacing_[o->grDelay().getAmbiguitySpacing()]++;
    phdAmbigsBySpacing_[o->phDelay().getAmbiguitySpacing()]++;
    if (maxNumOfChannels_ < o->getNumOfChannels())
      maxNumOfChannels_ = o->getNumOfChannels();
    if (o->getErrorCode() == "Z")
      numOfZCodes++;
    //
    numByScanIdBySrc[o->owner()->src()->getKey()][o->owner()->getScanId()]++;
    if (o->getSampleRate() != 0.0 || 0 < o->getQualityFactor())
      sampleRateByCount_[o->getSampleRate()]++;
    //
    // check channel bandwidth:
    if (0 < o->getQualityFactor() && 0 < o->getNumOfChannels())
      for (unsigned int j=0; j<o->channelBandwidth()->n(); j++)
        if (o->channelBandwidth()->getElement(j) != 0.0)
          channelBandwidthByCount_[o->channelBandwidth()->getElement(j)]++;
    //    
    bitsPerSampleByCount_[o->getBitsPerSample()]++;
    //
    if (o->stn_1() && o->polarization_1ByChan() && o->polarization_1ByChan()->size())
      for (int j=0; j<o->polarization_1ByChan()->size(); j++)
        o->stn_1()->numByPolarization()[QString(o->polarization_1ByChan()->at(j))]++;
    if (o->stn_2() && o->polarization_2ByChan() && o->polarization_2ByChan()->size())
      for (int j=0; j<o->polarization_2ByChan()->size(); j++)
        o->stn_2()->numByPolarization()[QString(o->polarization_2ByChan()->at(j))]++;
    //
    effIntgrTimes << o->getEffIntegrationTime();
    //
  };
  //
  // check for polarization setup:
  //
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    for (QMap<QString, int>::iterator jt=si->numByPolarization().begin(); 
      jt!=si->numByPolarization().end(); ++jt)
    {
      QString                     polrz(jt.key().toUpper());
    
      if (polrz == "X")
        si->addNetPolarization(SgVlbiStationInfo::NP_LinearPolarizX);
      else if (polrz == "Y")
        si->addNetPolarization(SgVlbiStationInfo::NP_LinearPolarizY);
      else if (polrz == "R")
        si->addNetPolarization(SgVlbiStationInfo::NP_CircPolarizRight);
      else if (polrz == "L")
        si->addNetPolarization(SgVlbiStationInfo::NP_CircPolarizLeft);
      else
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::selfCheck(): got an unknown polarization \"" + polrz + 
          "\" for station \"" + si->getKey() + "\"");    
    };
  };
  //
  //
  //
  // set up the typical sample rate:
  SgMJD                         t0=*observables_.at(0)->owner();
  if (sampleRateByCount_.size() == 0) //?
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): cannot find the sample rate for the " + getKey() + "-band");
  else if (sampleRateByCount_.size() == 1)
  {
    sampleRate_ = sampleRateByCount_.begin().key();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): set up the SampleRate: " + t0.toString(SgMJD::F_SOLVE_SPLFL) + " " + 
      QString("").sprintf("%.2f", sampleRate_) + " for the " + getKey() + "-band");
  }
  else
  {
    num = 0;
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): found multiple values of the sample rate at the " + getKey() + "-band:");
    for (QMap<double, int>::iterator it=sampleRateByCount_.begin(); it!=sampleRateByCount_.end(); ++it)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::selfCheck(): SampleRate: " + t0.toString(SgMJD::F_SOLVE_SPLFL) + " " + 
          QString("").sprintf("%.2f -> %d times", it.key(), it.value()));
      if (it.value() > num)
      {
        num = it.value();
        sampleRate_ = it.key();
      };
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): set up the typical sample rate to " + QString("").sprintf("%.2f", sampleRate_) + 
      " for the " + getKey() + "-band");
  };
  //
  // channel bandwdith:
  if (channelBandwidthByCount_.size() == 0)
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): cannot find the channel bandwidth info for the " + getKey() + "-band");
  else if (channelBandwidthByCount_.size() == 1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the channel bandwidth is " + 
      QString("").sprintf("%.2f MHz", channelBandwidthByCount_.begin().key()) + 
      " for the " + getKey() + "-band");
  }
  else
  {
    QString                     sStr("");
    for (QMap<double, int>::iterator it=channelBandwidthByCount_.begin();
      it!=channelBandwidthByCount_.end(); ++it)
      sStr += QString("").sprintf("%.2f (%d), ", it.key(), it.value());
    sStr.chop(2);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): found multiple values of the channel bandwidth at the " + getKey() + "-band: " + 
      sStr);
  };
  //
  // set up the typical bits per sample:
  if (bitsPerSampleByCount_.size() == 0) //?
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): cannot find the bits per sample for the " + getKey() + "-band");
  else if (bitsPerSampleByCount_.size() == 1)
  {
    bitsPerSample_ = bitsPerSampleByCount_.begin().key();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): set up the bits per sample to " + QString("").sprintf("%d", bitsPerSample_) + 
      " for the " + getKey() + "-band");
  }
  else
  {
    num = 0;
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): found multiple values of the bits per sample at the " + getKey() + "-band:");
    for (QMap<int, int>::iterator it=bitsPerSampleByCount_.begin(); it!=bitsPerSampleByCount_.end(); 
      ++it)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::selfCheck(): " + QString("").sprintf("%d -> %d times", it.key(), it.value()));
      if (it.value() > num)
      {
        num = it.value();
        bitsPerSample_ = it.key();
      };
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): set up the typical bits per sample to " + 
      QString("").sprintf("%d", bitsPerSample_) + " for the " + getKey() + "-band");
  };
  recordingMode_.sprintf("NChan:%d Rbw:%.1f Bps:%d", 
    maxNumOfChannels_, maxNumOfChannels_*sampleRate_*1.0e-6, bitsPerSample_);

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the record mode has been set to \"" + recordingMode_ + 
    "\" for the " + getKey() + "-band");

  // temporary: recalc the effFreqs:

  for (int i=0; i<observables_.size(); i++)
  {
    double                      effFreq4GR, effFreq4PH, effFreq4RT;
    SgVlbiObservable           *o=observables_.at(i);
    effFreq4GR = effFreq4PH = effFreq4RT = 0.0;
    //
    if (1 < o->getNumOfChannels())
    {
      evaluateEffectiveFreqs(*o->numOfAccPeriodsByChan_USB(), *o->numOfAccPeriodsByChan_LSB(),
        *o->refFreqByChan(), *o->fringeAmplitudeByChan(),
        *o->numOfSamplesByChan_USB(), *o->numOfSamplesByChan_LSB(),
        *o->channelBandwidth(), o->getReferenceFrequency(), o->getNumOfChannels(), 
//        effFreq4GR, effFreq4PH, effFreq4RT, correlatorType_, o->strId());
        effFreq4GR, effFreq4PH, effFreq4RT, correlatorType_, o);
      o->grDelay().setEffFreq(effFreq4GR);
      o->phDelay().setEffFreq(effFreq4PH);
      o->phDRate().setEffFreq(effFreq4RT);
    };
  };
  //
  //
  num = 0;
  // group delay:
  for (QMap<double, int>::iterator it=grdAmbigsBySpacing_.begin(); it!=grdAmbigsBySpacing_.end(); ++it)
  {
    if (it.value() > num)
    {
      num = it.value();
      typicalGrdAmbigSpacing_ = it.key();
    };
  };
  if (grdAmbigsBySpacing_.size() == 1)
    strGrdAmbigsStat_.sprintf("%.1f", grdAmbigsBySpacing_.begin().key()*1.0e9);
  else
  {
    strGrdAmbigsStat_ = "";
    num = observables_.size();
    for (QMap<double, int>::iterator it=grdAmbigsBySpacing_.begin(); it!=grdAmbigsBySpacing_.end(); ++it)
      strGrdAmbigsStat_ += QString("").sprintf("%.1f (%.1f%%), ", it.key()*1.0e9, it.value()*100.0/num);
    strGrdAmbigsStat_ = strGrdAmbigsStat_.left(strGrdAmbigsStat_.size() - 2);
  };
  //
  // phase delay:
  num = 0;
  for (QMap<double, int>::iterator it=phdAmbigsBySpacing_.begin(); it!=phdAmbigsBySpacing_.end(); ++it)
  {
    if (it.value() > num)
    {
      num = it.value();
      typicalPhdAmbigSpacing_ = it.key();
    };
  };
  if (phdAmbigsBySpacing_.size() == 1)
    strPhdAmbigsStat_.sprintf("%.3f", phdAmbigsBySpacing_.begin().key()*1.0e9);
  else
  {
    strPhdAmbigsStat_ = "";
    num = observables_.size();
    for (QMap<double, int>::iterator it=phdAmbigsBySpacing_.begin(); it!=phdAmbigsBySpacing_.end(); ++it)
      strPhdAmbigsStat_ += QString("").sprintf("%.3f (%.1f%%), ", it.key()*1.0e9, it.value()*100.0/num);
    strPhdAmbigsStat_ = strPhdAmbigsStat_.left(strPhdAmbigsStat_.size() - 2);
  };
  //
  //
  if (typicalGrdAmbigSpacing_ > 0.0)
  {
    addAttr(Attr_HAS_AMBIGS);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the typical ambig.spacing for the " + getKey() + "-band was set to " + 
      QString("").sprintf("%.2fns", typicalGrdAmbigSpacing_*1.0e9));
  }
  else
  {
    delAttr(Attr_HAS_AMBIGS);
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): set up of the typical ambig.spacing for the " + getKey() + "-band failed");
  };
  // adjust error codes: Z code was added by the import procedure
  // if all codes are "Z", that is an old DSN session, remove them:
  if (numOfZCodes == observables_.size() && !isAttr(Attr_HAS_AMBIGS))
  {
    for (int i=0; i<observables_.size(); i++)
      observables_.at(i)->setErrorCode(" ");
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): looks like an old DSN session, the error codes have been cleared");
  };
  //
  // baselines:
  for (BaselinesByName_it it=baselinesByName_.begin(); it!=baselinesByName_.end(); ++it)
    isOk = isOk && it.value()->selfCheck();
  //
  // sources:
  for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
  {
    SgVlbiSourceInfo           *si=it.value();
    if (numByScanIdBySrc.contains(si->getKey()))
      si->setTotalScanNum(numByScanIdBySrc.value(si->getKey()).size());
    else
    {
      str = si->getKey();
      // the source have no any observations on the primary band, remove it from the maps:
      --it;
      sourcesByName_.remove(str);
      delete si;
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::selfCheck(): the source \"" + str + "\" has no any good observations in the primary band"
        ", it was removed from the " + getKey() + "-band too");
    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): numbers of scans per a source were calculated", true);
  //
  //
  if (1 < effIntgrTimes.size())
  {
    qSort(effIntgrTimes.begin(), effIntgrTimes.end(), doubleSortingOrderLessThan);
    meanEffectiveIntegrationTime_ = effIntgrTimes.at(effIntgrTimes.size()/2);
  }
  else if (0 < effIntgrTimes.size())
    meanEffectiveIntegrationTime_ = effIntgrTimes.at(0);

  //
  // collect info on polarization setup:
  checkHistory();  
  //
  // report found polarization setup:
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): found polarization setup for the band " + getKey() + ":");
  for (QMap<QString, SgVlbiStationInfo*>::iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): \t" + si->getKey() + ": " + netPolarizations2string(si->getNetPolarizations()));
  };
  //
  return isOk;
};



//
bool SgVlbiBand::checkHistory()
{
  bool                          isOk=true;

  QString                       hc("");
  QRegularExpression            rxHasPol("if station ([a-zA-Z0-9]) and f_group ([A-Z]).*\n.*\n.*delay_offs_([a-z]).*\n");
  QRegularExpressionMatch       match;
  QRegularExpressionMatchIterator
                                it;
  
  QRegularExpression            rxStationsOnn("^\\s*\\+STATIONS\\s*$");
  QRegularExpression            rxStationsOff("^\\s*\\+([A-Z0-9]+)\\s*$");
  QRegularExpression            rxStation("^\\s*([_A-Za-z0-9-]{2})\\s+([_A-Z0-9-]{2,8})\\s+([A-Za-z0-9])\\s*$");
  bool                          isStationOn;
  QMap<QString, QString>        nameByKey;


  isStationOn = false;
  //
  for (int i=0; i<history_.size(); i++)
  {
    const QString&              st=history_.at(i)->getText();
    hc += st + "\n";
    //
//
//+STATIONS
//
//station name     mk4
//--------------------
//Hb      HOBART12 d
//Ht      HART15M  q
//Ke      KATH12M  a
//Kv      SEJONG   j
//Ma      MATERA   I
//Ns      NYALE13S w
//Nt      NOTO     S
//Ny      NYALES20 N
//On      ONSALA60 X
//Wz      WETTZELL V
//Yg      YARRA12M i
//
//* station  2-char station ID
//* name     3- to 8-char station name
//* mk4      1-char HOPS station code
//
//+NOTES
//
    if (!isStationOn && (match=rxStationsOnn.match(st)).hasMatch())
      isStationOn = true;
    else if (isStationOn && (match=rxStationsOff.match(st)).hasMatch())
      isStationOn = false;
    else if (isStationOn && (match=rxStation.match(st)).hasMatch())
      nameByKey[match.captured(3)] = match.captured(2).leftJustified(8, ' ');
  };

  QString                       sId;            // station one-char id
  QString                       sBandKey;       // a band key
  QString                       sPolarization;  // polarization
  QString                       sName;          // station name

  it = rxHasPol.globalMatch(hc);
  while (it.hasNext())
  {
    match = it.next();

//if station d and f_group S
//  pc_mode manual
//  delay_offs_x abcd 450.0 450.0 450.0 450.0
//  delay_offs_y abcd 327.8 327.8 327.8 327.8
    
    if (match.captured(2).toUpper() == getKey().toUpper())
    {
      sId = match.captured(1);
      sBandKey = match.captured(2);
      sPolarization = match.captured(3).toUpper();
      if (nameByKey.contains(sId))
      {
        sName = nameByKey.value(sId);
        if (stationsByName_.contains(sName))
        {
          SgVlbiStationInfo     *si=stationsByName_.value(sName);
          if (si)
          {
            if (sPolarization == "X")
              si->addNetPolarization(SgVlbiStationInfo::NP_LinearPolarizX);
            else if (sPolarization == "Y")
              si->addNetPolarization(SgVlbiStationInfo::NP_LinearPolarizY);
            else if (sPolarization == "R")
              si->addNetPolarization(SgVlbiStationInfo::NP_CircPolarizRight);
            else if (sPolarization == "L")
              si->addNetPolarization(SgVlbiStationInfo::NP_CircPolarizLeft);
            else
              logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
                "::checkHistory(): got an unknown polarization \"" + sPolarization + 
                "\" for station \"" + sName + "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
              "::checkHistory(): the station info is NULL for \"" + sName + "\"");
        }
        else
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::checkHistory(): cannot find station \"" + sName + "\" in the map of stations");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::checkHistory(): cannot find station one-char key \"" + sId + "\" in the STATIONS block");
    };
  };
  //
  return isOk;
};



//
SgMJD SgVlbiBand::lastHistoryEntry()
{
  SgMJD                         t(tZero);
  SgMJD                         t_control(tZero);
  if (1 < history_.size())
  {
    int                         idx=history_.size() - 1;
    int                         v=history_.at(idx)->getVersion() - 1; // the last processed version
    while (history_.at(idx)->getVersion() != v && 0 < idx)
    {
      idx--;
    };
    t = history_.at(idx)->getEpoch();
    
    idx = 0;
    while (history_.at(idx)->getVersion() != v && idx<history_.size())
    {
      idx++;
    };
    t_control = history_.at(idx)->getEpoch();
    if (t < t_control)
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::lastHistoryEntry(): the time conversion bug was detected,  t_begin: " + 
        t_control.toString() + " t_end: " + t.toString() + " for the version " + QString("").setNum(v));
      t = t_control + 0.1/DAY2SEC;
    };
  };
  return t;
};



//
SgMJD SgVlbiBand::calcHistoryEntry()
{
  SgMJD                         t(tZero);
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;
  
  nYear = nMonth = nDay = nHour = nMin = 0;
  dSec = 0.0;

  if (1 < history_.size())
  {
    int                         idx=history_.size() - 1;
    while (!history_.at(idx)->getText().contains("Driver  ID: vgosDbCalc")  && 0 < idx)
    {
      idx--;
    };
    t = history_.at(idx)->getEpoch();
    t.toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);

    while ((nHour == 4 || nHour == 5) && nMin==0 && dSec < 1.0e-4 && 0 < idx)
    {
      idx--;
      t = history_.at(idx)->getEpoch();
      t.toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
    };
//  t = history_.at(idx)->getEpoch();
  };
  return t;
}



//
void SgVlbiBand::propagateChannelBandwidth(double bw)
{
  for (int i=0; i<observables_.size(); i++)
    observables_.at(i)->propagateChannelBandwidth(bw);
  addAttr(SgVlbiBand::Attr_CHANNEL_INFO_MODIFIED);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
//
bool doubleSortingOrderLessThan(double a, double b)
{
  return a < b;
};
/*=====================================================================================================*/
