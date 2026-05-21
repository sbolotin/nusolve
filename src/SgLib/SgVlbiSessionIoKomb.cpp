/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
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



#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>
#include <QtCore/QVector>



#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgKombFormat.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>







/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/





// these two variables are declared in SgVlbiSessionIoFringes.cpp:
extern QMap<QString, QString>          stn2stn;
extern QMap<QString, QString>          src2src;


/*=======================================================================================================
*
*                           I/O utilities:
* 
*======================================================================================================*/
//
bool SgVlbiSession::getDataFromKombFiles(const QString& path2data,
  const QString& altDatabaseName, const QString& altCorrelatorName, 
  const QString& historyFileName, const QString& mapFileName, bool need2correctRefClocks, 
  const QStringList& masterfileSuffixes)
{
  bool                          isOk=false;
  QDir                          dir(path2data);
  QList<QString>                files2read;
  QStringList                   entryList;
  QRegExp                       reKombFileName("B[0-9]{2,}");
  int                           idx;
  QString                       correlatorComments(historyFileName);
  // for name maps:
  QMap<QString, QString>        stn2stn;
  QMap<QString, QString>        src2src;
  QMap<QString, QString>        bnd2bnd;
  QRegularExpression            reOldDbName("(\\d{2})([A-Z]{3})(\\d{2})([A-Z0-9]{1,2})");
  QRegularExpression            reNewDbName("(\\d{8})-([\\S]{2,12})");
  QRegularExpressionMatch       match;

  //
  if (need2correctRefClocks)
    addAttr(Attr_REF_CLOCKS_ADJUSTED);

  //
  files2read = dir.entryList(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
  if (files2read.size() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getDataFromKombFiles(): nothing to read");
    return isOk;
  };

  // get the maps:
  //
  importMapFile(mapFileName, stn2stn, src2src, bnd2bnd);
  if (stn2stn.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromKombFiles(): loaded " + QString("").setNum(stn2stn.size()) + 
      " entries for station name maps");
  if (src2src.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromKombFiles(): loaded " + QString("").setNum(src2src.size()) + 
      " entries for source name maps");
  if (bnd2bnd.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromKombFiles(): loaded " + QString("").setNum(bnd2bnd.size()) + 
      " entries for band code maps");
  //
  //
  for (int i=0; i<files2read.size(); i++)
    if (files2read.at(i).contains(reKombFileName))
      entryList << files2read.at(i);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::getDataFromKombFiles(): filtered " + QString("").setNum(entryList.size()) + 
    " scan entries from " + QString("").setNum(files2read.size()) + " total number of directories");

  //
  //
  for (int i=0; i<entryList.size(); i++)
  {
    const QString              &inKombFileName=entryList.at(i);
    QFileInfo                   fi(path2data + "/" + inKombFileName);
    int                         kombFileSize=QFileInfo(path2data + "/" + inKombFileName).size();
    if (kombFileSize%256 != 0)
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::getDataFromKombFiles(): unexpected size of the file " + path2data + "/" + inKombFileName + 
        ": " + QString("").setNum(kombFileSize) + ", skipped");
    else
      processKombFile(path2data, inKombFileName, stn2stn, src2src, bnd2bnd);
  };
  //
  //
  // check did we read something at all:
  if (!bands_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromKombFiles(): nothing useful found");
    return false;
  };
  //
  //
  setOriginType(OT_KOMB);    // flag it
  setCorrelatorType("GSI     ");
  addAttr(Attr_FF_CREATED);
  //
  if (cppsSoft_ == CPPS_UNKNOWN)
    cppsSoft_ = CPPS_C5PP;
  //
  //
  // set global stuff here:
  //
  if (2 < altDatabaseName.size())
  {
    if ((match=reOldDbName.match(altDatabaseName)).hasMatch())
    {
      setName(altDatabaseName);
      setNetworkSuffix(altDatabaseName.at(8));
      setOfficialName("UNKN");
      setSubmitterName("UNKN");
      setSchedulerName("UNKN");
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::getDataFromKombFiles(): database name was explicitly set to " + altDatabaseName + 
        ", a version 1 database name pattern");
    }
    else if ((match=reNewDbName.match(altDatabaseName)).hasMatch())
    {
      setName(altDatabaseName);
      setNetworkSuffix("-");
      setOfficialName("UNKN");
      setSubmitterName("UNKN");
      setSchedulerName("UNKN");
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): database name was explicitly set to " + altDatabaseName + 
        ", a version 2 database name pattern");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromKombFiles(): the provided database name, " + altDatabaseName + 
        ", does not fit any known pattern and was ignored");
  };
  //
  //
  QMap<double, SgVlbiBand*>     bandByFreq;
  QString                       str("");
  for (int i=0; i<bands_.size(); i++)
  {
    SgVlbiBand                 *band=bands_.at(i);
    QString                     bandKey=band->getKey();
    band->setTCreation(SgMJD::currentMJD().toUtc());
    band->setInputFileName(path2data + "/*");
    band->setInputFileVersion(0);
    //
    // quick'n'dirty:
    idx = 0;
    SgVlbiObservable           *o=observations_.at(idx)->observable(bandKey);
    while (!o && idx<observations_.size())
      o = observations_.at(idx++)->observable(bandKey);
    if (o)
      band->setFrequency(o->getReferenceFrequency());
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromKombFiles(): cannot set up a frequency for " + bandKey + "-band");
    if (bandByFreq.contains(band->getFrequency()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromKombFiles(): the frequency " + QString("").setNum(band->getFrequency()) + 
        " is already registered");
    }
    else
      bandByFreq.insert(band->getFrequency(), band);
  };
  //
  //
  // load correlator's comments for history records:
  //
  if (correlatorComments.size()==0)
  {
    dir.setPath(path2data + "/../control/");
    str = "";
    if (!dir.exists())
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromKombFiles(): cannot get correlator comments, the directory does not exist: " +
        dir.path());
    else
    {
      entryList.clear();
      entryList = dir.entryList(QStringList() << "*.corr", 
        QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name);
      if (entryList.size() == 0)
      { 
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromKombFiles(): cannot get correlator comments, no any *.corr file found: " + 
          dir.path());
      }
      else if (entryList.size() > 1)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromKombFiles(): found more than one file with correlator comments:");
        for (int j=0; j<entryList.size(); j++)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromKombFiles(): " + entryList.at(j));
        str = entryList.at(0);
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromKombFiles(): picked up this one: " + str);
      }
      else
        str = entryList.at(0);
    };
    if (str.size())
      correlatorComments = path2data + "/../control/" + str;
  };
  if (correlatorComments.size())
    getCorrelatorHistory(correlatorComments);
  else
  {
    for (int i=0; i<bands_.size(); i++)
    {
      bands_.at(i)->history().addHistoryRecord("== The correlator report was not provided. ==", 
        SgMJD::currentMJD().toUtc());
      bands_.at(i)->history().addHistoryRecord("== ", SgMJD::currentMJD().toUtc());
    };
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromKombFiles(): the correlator report was not provided.");
  };
  //
  //
  // set up a primary band:
  // standard IVS case:
  QString                       primeBandKey;
  if (bandByKey_.size()==2 && bandByKey_.contains("X") && bandByKey_.contains("S"))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromKombFiles(): importing typical IVS session");
    primaryBand_ = bandByKey_.value("X");
  }
  else
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromKombFiles(): importing a foreign session setup");
    if (bandByFreq.size() > 1)
      primaryBand_ = bandByFreq.begin().value();
    else
      primaryBand_ = bands_.at(0);
  };
  primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
  primeBandKey = primaryBand_->getKey();

  // set the order:
  setSidebandOrder(SO_USB_LSB);
  //
  //
  // remove observations that are not in the primary band:
  int                           num;
  idx = num = 0;
  while (idx<observations_.size())
  {
    SgVlbiObservation          *obs=observations_.at(idx);
    if (!obs->observable(primeBandKey))
    {
      observationByKey_.remove(obs->getKey());
      observations_.removeAt(idx);
      idx--;
      num++;
      delete obs;
    };
    idx++;
  };
  if (num)
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
      "::getDataFromKombFiles(): " + QString("").setNum(num) + 
      " second-band-only observation" + (num==1?"":"s") + " were removed");
  //
  //
  if (selfCheck(false, masterfileSuffixes))
    logger->write(SgLogger::INF, SgLogger::IO, className() + 
      "::getDataFromKombFiles(): session selfcheck complete");
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::getDataFromKombFiles(): the selfcheck for the session failed");
    return false;
  };
  //
  // overwrite correlator name with user-provided info:
  if (altCorrelatorName.size())
  {
    setCorrelatorName(altCorrelatorName);
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::getDataFromKombFiles(): correlator name was explicitly set to " + altCorrelatorName);
  };
  //
  // calculate aux data:
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
      it!=obs->observableByKey().end(); ++it)
    {
      SgVlbiObservable           *o=it.value();
      o->calcPhaseCalDelay();
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO, className() + 
    "::getDataFromKombFiles(): phase cal delays were calculated");
  //
  //
  //
  return true;
};



//
void SgVlbiSession::processKombFile(const QString& path2file, const QString& kombFileName, 
  const QMap<QString, QString>& stn2stn, const QMap<QString, QString>& src2src,
  const QMap<QString, QString>& bnd2bnd)
{
  SgKombReader                  kombReader;
  QString                       str("");

  logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() +
    "::processKombFile(): reading KOMB file " + kombFileName);
  kombReader.readFile(path2file + "/" + kombFileName);
  if (!kombReader.isOk())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
      "::processKombFile(): the KOMB file " + kombFileName + " is not Ok; skipped");
    return;
  };

// what need to be checked:
// stationInfo->setSid('-', '-'); ?

  SgVlbiBand                   *band=NULL;
  //
  QString                       bandKey("");
  QString                       station1Name, station2Name, sourceName, baselineName;
  QString                       scanName, scanId, obsKey;
  int                           obsIdx;
  int                           nTmp;
  double                        f;
  SgMJD                         epoch;
  SgVlbiStationInfo            *station1Info, *station2Info;
  SgVlbiStationInfo            *bandStation1Info, *bandStation2Info;
  SgVlbiSourceInfo             *sourceInfo, *bandSourceInfo;
  SgVlbiBaselineInfo           *baselineInfo, *bandBaselineInfo;
  SgVlbiObservation            *obs=NULL;
  SgVlbiObservable             *o=NULL;
  SgVlbiAuxObservation         *auxObs_1=NULL, *auxObs_2=NULL;
    
  double                        refFreq=0.0;
  bool                          isSbdSigmaNan, isSbdSigmaInf;
  bool                          isGrdSigmaNan, isGrdSigmaInf;
  bool                          isPhrSigmaNan, isPhrSigmaInf;
  bool                          isTmp;
  int                           numOfChannels;
  double                        effFreq4GR=0.0, effFreq4PH=0.0, effFreq4RT=0.0;
  bool                          need2adjust4clocks=isAttr(Attr_REF_CLOCKS_ADJUSTED);
  double                        ap_0, ap_1, ap_2, ap_3, prtOffset;
  ap_0 = ap_1 = ap_2 = ap_3 = 0.0;
  prtOffset = 0.0;

  // check experiment code:
  str = QString(kombReader.experimentCode()).simplified();
  if (sessionCode_=="UNKN")
    setSessionCode(str);
  else if (str != sessionCode_)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
      "::processKombFile(): the KOMB file " + kombFileName + " is belong to another session, " + str +
      " ours is " + sessionCode_ + "; skipped");
    return;
  };
  

  for (QMap<QString, QString>::const_iterator it=kombReader.mapOfBands().begin(); 
    it!=kombReader.mapOfBands().end(); ++it)
  {
    bandKey = QString(it.key()).simplified();
    //
    if (bnd2bnd.size())
    {
      if (!check4NameMap(bnd2bnd, bandKey))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processKombFile(): skipping the observation " + kombFileName + 
          ": the band \"" + bandKey + "\" have to be skipped");
        continue;
      };
    };
    //
    if (bandByKey_.contains(bandKey))
      band = bandByKey_.value(bandKey);
    else
    {
      band = new SgVlbiBand;
      band->setKey(bandKey);
      bands_.append(band);
      bandByKey_.insert(bandKey, band);
    };
    const SgKombOb01Record     *ob01=&kombReader.ob01();
  //const SgKombOb02Record     *ob02=&kombReader.ob02();
    const SgKombOb03Record     *ob03=&kombReader.ob03();
    const SgKombBd01Record     *bd01=kombReader.bd01(it.key()); // it.key() contains a space char
    const SgKombBd02Record     *bd02=kombReader.bd02(it.key());
    const SgKombBd03Record     *bd03=kombReader.bd03(it.key());
    const SgKombBd04Record     *bd04=kombReader.bd04(it.key());
    const SgKombBd05Record     *bd05=kombReader.bd05(it.key());
    if (!bd01)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::processKombFile(): the KOMB file " + kombFileName + ": BD01 record is NULL for the " + 
        bandKey + "-band; skipped");
      return;
    };
    if (!bd02)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::processKombFile(): the KOMB file " + kombFileName + ": BD02 record is NULL for the " + 
        bandKey + "-band; skipped");
      return;
    };
    if (!bd03)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::processKombFile(): the KOMB file " + kombFileName + ": BD03 record is NULL for the " + 
        bandKey + "-band; skipped");
      return;
    };
    if (!bd04)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::processKombFile(): the KOMB file " + kombFileName + ": BD04 record is NULL for the " + 
        bandKey + "-band; skipped");
      return;
    };
    if (!bd05)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
        "::processKombFile(): the KOMB file " + kombFileName + ": BD05 record is NULL for the " + 
        bandKey + "-band; skipped");
      return;
    };
    //
    epoch = ob01->tRefer();
    station1Name.sprintf("%-8s", ob01->station1Name());
    station2Name.sprintf("%-8s", ob01->station2Name());
    sourceName  .sprintf("%-8s", ob01->sourceName());
    //
    // map the names:
    if (stn2stn.size())
    {
      if (!check4NameMap(stn2stn, station1Name))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processKombFile(): skipping the observation " + kombFileName + 
          ": the station \"" + station1Name + "\" have to be skipped");
        return;
      };
      if (!check4NameMap(stn2stn, station2Name))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processKombFile(): skipping the observation " + kombFileName + 
          ": the station \"" + station2Name + "\" have to be skipped");
        return;
      };
    };
    if (src2src.size())
    {
      if (!check4NameMap(src2src, sourceName))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processKombFile(): skipping the observation " + kombFileName + 
          ": the source \"" + sourceName + "\" have to be skipped");
        return;
      };
    };
    //
    //
    baselineName = station1Name + ":" + station2Name;
    // set up scanId and scanName (KOMB files do not contain scan names):
    scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
    scanName.sprintf("%03d-%02d%02d  ",
      ob01->epochObsStart(1), ob01->epochObsStart(2), ob01->epochObsStart(3));
//    ob01->tStart().calcDayOfYear(), ob01->tStart().calcHour(), ob01->tStart().calcMin());
//    epoch.calcDayOfYear(), epoch.calcHour(), epoch.calcMin());
    //
    obsKey.sprintf("%s", 
      qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));
    //
    //
    // check for NaN in delay and rate:
    f = bd05->obsDelayAtReferEpoch();
    if (isinf(f) != 0 || isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): an observation from KOMB input file \"" + kombFileName + 
        "\" (the baseline " + baselineName + 
        " observing " + sourceName + " source at " + epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        ") contains wrong group delay (NaN or Inf); the observation is skipped", true);
      return;
    };
   f = bd05->obsDelayRateAtReferEpoch();
    if (isinf(f) != 0 || isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): an observation from KOMB input file \"" + kombFileName + 
        "\" (the baseline " + baselineName + 
        " observing " + sourceName + " source at " + epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        ") contains wrong delay rate (NaN or Inf); the observation is skipped", true);
      return;
    };
    // ...continue:
    //
    if (observationByKey_.contains(obsKey))
      obs = observationByKey_.value(obsKey);
    else
    {
      obs = new SgVlbiObservation(this);
      obs->setMJD(epoch);
      obs->setScanName(scanName);
      obs->setScanId(scanId);
      obs->setCorrRootFileName(ob01->correlatorFileName());
      obs->setKey(obsKey);
      obs->setMediaIdx(observations_.size());
      observations_.append(obs);
      observationByKey_.insert(obsKey, obs);
    };
    // pick up or create an observation:
    o = new SgVlbiObservable(obs, band);
    obsIdx = obs->getMediaIdx();
    o->setMediaIdx(obsIdx);
    obs->addObservable(bandKey, o);
    obs->setupActiveObservable(bandKey);
    // determine number of channels:
    numOfChannels = bd01->totalNumOfProcChannels();
    o->allocateChannelsSetupStorages(numOfChannels);
    //
    //
    // station #1:
    if (stationsByName_.contains(station1Name))
      station1Info = stationsByName_.value(station1Name);
    else // new station, add it to the container and register its index:
    {
      station1Info = new SgVlbiStationInfo(stationsByName_.size(), station1Name);
      stationsByName_.insert(station1Info->getKey(), station1Info);
      stationsByIdx_.insert(station1Info->getIdx(), station1Info);
      station1Info->setCid(*ob01->baselineId());
      station1Info->setSid('-', '-');
    };
    // station #2:
    if (stationsByName_.contains(station2Name))
      station2Info = stationsByName_.value(station2Name);
    else // new station, add it to the container and register its index:
    {
      station2Info = new SgVlbiStationInfo(stationsByName_.size(), station2Name);
      stationsByName_.insert(station2Info->getKey(), station2Info);
      stationsByIdx_.insert(station2Info->getIdx(), station2Info);
      station2Info->setCid(*(ob01->baselineId()+1));
      station2Info->setSid('-', '-');
    };
    // source:
    if (sourcesByName_.contains(sourceName))
      sourceInfo = sourcesByName_.value(sourceName);
    else // new source, add it to the container and register its index:
    {
      sourceInfo = new SgVlbiSourceInfo(sourcesByName_.size(), sourceName);
      sourcesByName_.insert(sourceInfo->getKey(), sourceInfo);
      sourcesByIdx_.insert(sourceInfo->getIdx(), sourceInfo);
    };
    // baseline:
    if (baselinesByName_.contains(baselineName))
      baselineInfo = baselinesByName_.value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      baselineInfo = new SgVlbiBaselineInfo(baselinesByName_.size(), baselineName);
      baselinesByName_.insert(baselineInfo->getKey(), baselineInfo);
      baselinesByIdx_.insert(baselineInfo->getIdx(), baselineInfo);
    };
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
    obs->setStation1Idx(station1Info->getIdx());
    obs->setStation2Idx(station2Info->getIdx());
    obs->setSourceIdx(sourceInfo->getIdx());
    obs->setBaselineIdx(baselineInfo->getIdx());
    //
    // plus additional info:
    o->setFourfitOutputFName(kombFileName);
    //
    //
    ap_0 = ob01->aPrioriObses(0);
    ap_1 = ob01->aPrioriObses(1);
    ap_2 = ob01->aPrioriObses(2);
    ap_3 = ob01->aPrioriObses(3);
    prtOffset = ob01->aPrioriRefClockOffset();
    //
    // collect observables:
    // single band delay:
    o->sbDelay().setValue(bd05->obsCoarseDelayAtReferEpoch());    // [s]
    //
    if (need2adjust4clocks)
    {
      // correct the observables:
      // group delay:
      double                    dDel;
      dDel = ap_1*prtOffset + 1.0/2.0*ap_2*prtOffset*prtOffset + 
              1.0/6.0*ap_3*prtOffset*prtOffset*prtOffset;
      o->grDelay().setValue(bd05->obsDelayAtReferEpoch() + dDel);          // [s]
      // delay rate:
      double                    dRat;
      dRat = ap_2*prtOffset + 1.0/2.0*ap_3*prtOffset*prtOffset;
      o->phDRate().setValue(bd05->obsDelayRateAtReferEpoch() + dRat);      // [s/s]
      //
      // store original correlator data as "alternate" values:
      o->grDelay().setAlternateValue(bd05->obsDelayAtReferEpoch());          // [s]
      // delay rate:
      o->phDRate().setAlternateValue(bd05->obsDelayRateAtReferEpoch());      // [s/s]
    }
    else
    {
      o->grDelay().setValue(bd05->obsDelayAtReferEpoch());          // [s]
      // delay rate:
      o->phDRate().setValue(bd05->obsDelayRateAtReferEpoch());      // [s/s]    
    };
    //
    //
    // workaround the correlator "feature" (not sure we need it for GSI):
    isSbdSigmaNan = isSbdSigmaInf = isGrdSigmaNan = isGrdSigmaInf = isPhrSigmaNan = 
      isPhrSigmaInf = false;
    // single band delay sigma (in sec):
    f = bd05->obsCoarseDelaySigma();
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the single band delay", true);
        isSbdSigmaInf = true;
      f = 1.0;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the single band delay", true);
      isSbdSigmaNan = true;
      f = 1.0;
    };
    o->sbDelay().setSigma(f);
    //
    // group delay sigma (in seconds):
    f = bd05->obsDelaySigma();
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isGrdSigmaInf = true;
      f = 1.0e-6;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isGrdSigmaNan = true;
      f = 1.0e-6;
    };
    o->grDelay().setSigma(f);
    //
    // delay rate sigma:
    f = bd05->obsDelayRateSigma();
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isPhrSigmaInf = true;
      f = 1.0e-6;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline " + baselineName + " observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isPhrSigmaNan = true;
      f = 1.0e-6;
    };
    o->phDRate().setSigma(f);
    // complain:
    if (isSbdSigmaNan || isSbdSigmaInf ||
        isGrdSigmaNan || isGrdSigmaInf ||
        isPhrSigmaNan || isPhrSigmaInf   )
    {
      QString                   s1(""), s2(""), s3("");
      s1 = (isSbdSigmaNan || isGrdSigmaNan || isPhrSigmaNan)?"(nan)":"(inf)";
      if (isSbdSigmaNan || isSbdSigmaInf)
        s2 = "single band delay, ";
      if (isGrdSigmaNan || isGrdSigmaInf)
        s2+= "group delay, ";
      if (isPhrSigmaNan || isPhrSigmaInf)
        s2+= "delay rate, ";
      s2 = s2.left(s2.size() - 2);
      if (o->getFourfitOutputFName().size())
        s3 = obs->getScanName().trimmed() + "/" + o->getFourfitOutputFName();
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): The observation #" + QString("").setNum(obsIdx) +
        ", " + s3 + " contains wrong sigma " + s1 + " for " + s2 + ".");
    };
    //
    // single band:
    o->sbDelay().setResidualFringeFitting(bd05->obsCoarseDelayResid());    // sec
    // group delay:
    o->grDelay().setResidualFringeFitting(bd05->obsDelayResid());          // sec
    // delay rate:
    o->phDRate().setResidualFringeFitting(bd05->obsDelayRateResid());      // sec per sec
    // bd05->obsCoarseDelayRateResid() ?
    //
    //
    o->grDelay().setAmbiguitySpacing(bd05->obsDelayAmbiguity());
    // KOMB mix quality code and error code in one field:
    str = bd02->kombQualityCode();
    nTmp = str.toInt(&isTmp);
    o->setQualityFactor(isTmp ? nTmp : 0);
    o->setErrorCode(isTmp ? "" : str);

//std::cout << "  ---- i=" << obsIdx << "  KOMBf= [" << qPrintable(kombFileName) << "]" 
//<< " errorCode: [" << qPrintable(o->getErrorCode()) << "]\n";
    //
    o->setTapeQualityCode(bd01->tapeQualityCode());
    //
    o->setReferenceFrequency((refFreq=bd01->procRefFreq()*1.0e-6)); // Hz->MHz
    o->setCorrCoeff(bd05->fineSearchFringeAmplitude());
    o->setTotalPhase(bd02->obsTotalPhaseAtReferEpoch()*DEG2RAD); // in radians
    o->setIncohChanAddAmp(bd05->coarseSearchFringeAmplitude());
    o->setIncohSegmAddAmp(bd05->incohFringeAmplitude());
    o->setSnr(bd05->snr());
    o->setProbabOfFalseDetection(bd05->falseDetectionProbability());
    o->setGeocenterTotalPhase(bd02->obsTotalPhaseAtEarthCenterEpoch()*DEG2RAD); // in radians
    o->setGeocenterResidPhase(bd02->obsResidPhaseAtEarthCenterEpoch()*DEG2RAD); // in radians

    o->setAprioriDra(0, ap_0);
    o->setAprioriDra(1, ap_1);
    o->setAprioriDra(2, ap_2);
    // undefined:
    // single band:
    o->sbDelay().setGeocenterValue(0.0);
    // group delay:
    o->grDelay().setGeocenterValue(0.0);
    // delay rate:
    o->phDRate().setGeocenterValue(0.0);
    //
    //
    // alloc auxiliary data:
    if (!station1Info->auxObservationByScanId()->contains(scanId)) // new scan, insert data:
    {
      auxObs_1 = new SgVlbiAuxObservation;
      auxObs_1->setMJD(epoch);
      station1Info->auxObservationByScanId()->insert(scanId, auxObs_1);
    }
    else
      auxObs_1 = station1Info->auxObservationByScanId()->value(scanId);
    if (!station2Info->auxObservationByScanId()->contains(scanId)) // new scan, insert data:
    {
      auxObs_2 = new SgVlbiAuxObservation;
      auxObs_2->setMJD(epoch);
      station2Info->auxObservationByScanId()->insert(scanId, auxObs_2);
    }
    else
      auxObs_2 = station2Info->auxObservationByScanId()->value(scanId);
    //
    if (auxObs_1->getRefClockOffset() != 0.0 && 
        auxObs_1->getRefClockOffset() != ob01->aPrioriRefClockOffset())
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processKombFile(): the observation #" + QString("").setNum(obsIdx) + 
        " extracted from the KOMB file " + kombFileName +
        " at the baseline " + baselineName + " observing " + sourceName + " at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " contains a reference clock offset (" + 
        QString("").setNum(ob01->aPrioriRefClockOffset()) + 
        ") that differs from previous data value (" + 
        QString("").setNum( auxObs_1->getRefClockOffset()) + "), diff=" + 
        QString("").setNum((auxObs_1->getRefClockOffset() - ob01->aPrioriRefClockOffset())*1.0e9) + 
        "ns");
    };
    if (prtOffset != 0.0)
      auxObs_1->setRefClockOffset(prtOffset);
    //
    //
    // aux data:
    // undefined in KOMB files:
    o->setStartOffset(0);
    o->setStopOffset (0);
    o->setCentrOffset(0.0);
    o->setEffIntegrationTime(bd02->effectiveIntegrationPeriod());
    o->setAcceptedRatio(0.0);
    o->setDiscardRatio(bd02->rejectionRate());

    // set up epochs of events:
    // correlation:
    o->setEpochOfCorrelation(ob01->tProcByCorr());
    // fringing:
    o->setEpochOfFourfitting(bd01->tProcByKomb());
    // nominal scan time:
    o->setEpochOfScan(tZero);
    //
    // "central" scan epoch:
    o->setEpochCentral(bd02->tObsCentral());
    // and another couple of epochs:
    // "start":
    o->setTstart(ob01->tStart());
    // "stop":
    o->setTstop(ob01->tFinis());
    //
    // ?
    o->setSampleRate(ob01->videoBandWidth()*2.0); // Hz
    //
//    o->setBitsPerSample(si_1.bits_sample_); ???
    //
    // collect data necessary to evaluate the effective frequencies (for ionospheric correction):
    for (int i=0; i<numOfChannels; i++)
    {
      // "Yes the NPP data in USB and LSB order in the komb file":
      o->numOfAccPeriodsByChan_USB()->setElement(i, bd02->procPpNumTable(i, 0));
      o->numOfAccPeriodsByChan_LSB()->setElement(i, bd02->procPpNumTable(i, 1));
      /*
        o->numOfAccPeriodsByChan_LSB()->setElement(i, bd02->procPpNumTable(i, 0));
        o->numOfAccPeriodsByChan_USB()->setElement(i, bd02->procPpNumTable(i, 1));
      */
      //
      // just no weights here:
      o->numOfSamplesByChan_USB() ->setElement(i, 0.0);
      o->numOfSamplesByChan_LSB() ->setElement(i, 0.0);
      o->refFreqByChan()          ->setElement(i, bd01->rfFreqs(i)*1.0e-6); // ->MHz
      //(*o->polarization_1ByChan())[i] = ;
      //(*o->polarization_2ByChan())[i] = ;
      o->fringeAmplitudeByChan()  ->setElement(i, bd05->fringeAmpPhase(i, 0));
      o->fringePhaseByChan()      ->setElement(i, bd05->fringeAmpPhase(i, 1)*DEG2RAD);
      o->channelBandwidth()       ->setElement(i, ob01->videoBandWidth()*1.0e-6); // Hz -> MHz
      //
      // phase calibration data:
      // phase cal data: amplitude, phase, freq, offset and error rate:
      o->phaseCalData_1ByChan()   ->setElement(0,i, bd03->phaseCalAmpPhase1(i, 0)*1.0e4);
      o->phaseCalData_2ByChan()   ->setElement(0,i, bd04->phaseCalAmpPhase2(i, 0)*1.0e4);
      // phases:
      o->phaseCalData_1ByChan()   ->setElement(1,i, bd03->phaseCalAmpPhase1(i, 1)*DEG2RAD);
      o->phaseCalData_2ByChan()   ->setElement(1,i, bd04->phaseCalAmpPhase2(i, 1)*DEG2RAD);
      // freqs:
      o->phaseCalData_1ByChan()   ->setElement(2,i, ob03->phCalFreqs(i)*1.0e-3);
      o->phaseCalData_2ByChan()   ->setElement(2,i, ob03->phCalFreqs(i)*1.0e-3);
      // offsets (there are no phase cal offsets in KOMB files)
      o->phaseCalData_1ByChan()   ->setElement(3,i, 0.0);
      o->phaseCalData_2ByChan()   ->setElement(3,i, 0.0);
      // error rates (there are no error rates in KOMB files too):
      o->phaseCalData_1ByChan()   ->setElement(4,i, 0.0);
      o->phaseCalData_2ByChan()   ->setElement(4,i, 0.0);
      //
      //(*o->chanIdByChan())    [i] = ;
      //(*o->corelIndexNumG())[i] = ;
      //(*o->corelIndexNumLSB())[i] = ;
      //  o->loFreqByChan_1()->setElement(i, ??);
      //(*o->bbcIdxByChan_1())[i] = ??;
      //  o->loFreqByChan_2()->setElement(i, ??);
      //(*o->bbcIdxByChan_2())[i] = ??;
    };
    //    
    // check and rescale phase cal data
    //
    for (int i=0; i<numOfChannels; i++)
    {
      for (int j=0; j<5; j++)
      {
        if (isnan(o->phaseCalData_1ByChan()->getElement (j,i)))
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processKombFile(): st1 one of phase cal values is NAN " + 
            QString("").sprintf("(chan=%d:dat=%d)", i, j) + " from the scan " + obs->getScanName() + 
            " KOMB file " + kombFileName);
          o->phaseCalData_1ByChan()->setElement (j,i, -100.0);
        };
        if (isnan(o->phaseCalData_2ByChan()->getElement (j,i)))
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processKombFile(): st2 one of phase cal values is NAN " + 
            QString("").sprintf("(chan=%d:dat=%d)", i, j) + " from the scan " + obs->getScanName() + 
            " KOMB file " + kombFileName);
          o->phaseCalData_2ByChan()->setElement (j,i, -100.0);
        };
      };
    };
    //
    evaluateEffectiveFreqs(*o->numOfAccPeriodsByChan_USB(), *o->numOfAccPeriodsByChan_LSB(), 
      *o->refFreqByChan(), *o->fringeAmplitudeByChan(), 
      *o->numOfSamplesByChan_USB(), *o->numOfSamplesByChan_LSB(),
      *o->channelBandwidth(), refFreq, numOfChannels,
      effFreq4GR, effFreq4PH, effFreq4RT, getCorrelatorType(), o);
    o->grDelay().setEffFreq(effFreq4GR);
    o->phDelay().setEffFreq(effFreq4PH);
    o->phDRate().setEffFreq(effFreq4RT);
//  o->setPhaseCalModes(t207->pcal_mode);
  //
  //
//  o->setFourfitVersion(0, t204->ff_version[0]);
//  o->setFourfitVersion(1, t204->ff_version[1]);
//  o->setFourfitControlFile(t204->control_file);
//  o->setFourfitCommandOverride(t204->override);
  //
//  o->setPhaseCalRates(0, bd03->phaseCalRate1()*1.0e6);   // sec->usec
//  o->setPhaseCalRates(1, bd03->phaseCalRate2()*1.0e6);   // sec->usec
    o->setPhaseCalRates(0, bd03->phaseCalRate1());   // sec
    o->setPhaseCalRates(1, bd03->phaseCalRate2());   // sec
//  o->setUvFrPerAsec(0, t202->u);
//  o->setUvFrPerAsec(1, t202->v);
//  o->setUrVr(0, t202->uf);
//  o->setUrVr(1, t202->vf);
//  o->setCorrClocks(0, 0, t202->ref_clock*1.0e-6); // musec -> sec
//  o->setCorrClocks(0, 1, t202->rem_clock*1.0e-6); // musec -> sec
//  o->setCorrClocks(1, 0, t202->ref_clockrate);
//  o->setCorrClocks(1, 1, t202->rem_clockrate);
//  o->setInstrDelay(0, t202->ref_idelay*1.0e-6); // musec -> sec
//  o->setInstrDelay(1, t202->rem_idelay*1.0e-6); // musec -> sec
//  o->setNlags(t202->nlags);
    o->setCentrOffset((ob01->tRefer() - bd02->tObsCentral())*DAY2SEC);  // DELTAEPO
    o->vDlys()->setElement(0, bd05->phaseDelayAtReferEpochMinus_1sec());
    o->vDlys()->setElement(1, bd05->phaseDelayAtReferEpoch());
    o->vDlys()->setElement(2, bd05->phaseDelayAtReferEpochPlus_1sec());
    o->vAuxData()->setElement(0, ob01->aPrioriClockError()*1.0e6);                // APCLOFST sec->usec
    o->vAuxData()->setElement(1, bd02->obsTotalPhaseAtCentrlEpoch()*DEG2RAD);     // TOTPCENT
    o->vAuxData()->setElement(2, bd02->obsDelayRateAtCentrlEpoch());              // RATOBSVM
    o->vAuxData()->setElement(3, bd02->obsDelayAtCentrlEpoch());                  // DELOBSVM

    o->setFourfitSearchParameters(0, bd02->searchWndCoarseDelay(0)*1.0e6);  // sec->usec
    o->setFourfitSearchParameters(1, bd02->searchWndCoarseDelay(1)*1.0e6);  // sec->usec
    o->setFourfitSearchParameters(2, bd02->searchWndFineDelay(0)*1.0e6);    // sec->usec
    o->setFourfitSearchParameters(3, bd02->searchWndFineDelay(1)*1.0e6);    // sec->usec
    o->setFourfitSearchParameters(4, bd02->searchWndFineDelayRate(0)*1.0e6);// sec->usec
    o->setFourfitSearchParameters(5, bd02->searchWndFineDelayRate(1)*1.0e6);// sec->usec
    //
    // per band statistics:
    bandStation1Info->incNumTotal(DT_DELAY);
    bandStation2Info->incNumTotal(DT_DELAY);
    bandSourceInfo->incNumTotal(DT_DELAY);
    bandBaselineInfo->incNumTotal(DT_DELAY);

    bandStation1Info->incNumTotal(DT_RATE);
    bandStation2Info->incNumTotal(DT_RATE);
    bandSourceInfo->incNumTotal(DT_RATE);
    bandBaselineInfo->incNumTotal(DT_RATE);
  };
};
/*=====================================================================================================*/






/*=====================================================================================================*/
