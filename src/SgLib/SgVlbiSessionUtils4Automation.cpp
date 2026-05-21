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

#include <QtCore/QRegularExpression>


#include <SgVlbiSession.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QMessageBox>
#else
#   include <QtGui/QMessageBox>
#endif






#include <SgLogger.h>
#include <SgRegularExpressions.h>
#include <SgTaskConfig.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>



#define LOCAL_DEBUG

extern QList<QString>           clockRefStations4Int, clockRefStations4R4, clockRefStations4Gen;
extern QList<QString>           coordRefStations4Gen;


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
bool SgVlbiSession::doPostReadActions()
{
  if (!(config_ && parametersDescriptor_))
    return false;
  //
  const SgTaskConfig::AutomaticProcessing
                               ap=config_->apByNetId().contains(networkID_)?
                               config_->apByNetId()[networkID_] : config_->apByNetId()[defaultNetIdName];
  //
  // special case (4testing):
  if (networkID_ == "INT") // special case:
    return doPostReadActions4Ints(ap);
  //
  //
  if (config_->apByNetId().contains(networkID_))
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions(): post read actions initiated for the session " + getName() +
      " using " + networkID_ + " scenario", true);
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions(): post read actions initiated for the session " + getName() +
      " using " + defaultNetIdName + " scenario", true);
  //
  //
  // prepare the stuff:
  suppressNotSoGoodObs();
  //
  // prepare the session:
  if (ap.doSessionSetup_)
  {
    pickupReferenceClocksStation();
    pickupReferenceCoordinatesStation();
    checkUseOfManualPhaseCals();
  };
  //
  //
  config_->setUseDelayType(SgTaskConfig::VD_SB_DELAY);
  //
  if (ap.doIonoCorrection4SBD_ && bands().size()>1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions(): evaluating ionospheric corrections for single band delays", true);
    calculateIonoCorrections(NULL);
  };
  //
  if (ap.doClockBreaksDetection_) 
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions(): clock break detections not implemened yet", true);
    //  check4ClockBreaksAtSBD();
    //    detectAndProcessClockBreaks();
  };
  //
  if (ap.doOutliers_)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions(): elimination of outliers, early stage", true);
    eliminateOutliersMode1();
  };
  //
  return true;
};



//
//
bool SgVlbiSession::doPostReadActions4Ints(const SgTaskConfig::AutomaticProcessing& ap)
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::doPostReadActions4Ints(): post read actions initiated for the session " + getName() + 
    ", INT mode", true);
  
  QMap<QString, int>            bandIdxByKey;
  QList<SgParametersDescriptor::ParIdx> 
                                parIdxs;
  double                        opThresholdSaved=config_->getOpThreshold();
  int                           primaryBandIdx=-1;
  //
  // preparations:
  //
  if (primaryBand_->getInputFileVersion() > 3) // clear GSI editings
    resetAllEditings(true);
  else
  {
    suppressNotSoGoodObs();
    if (ap.doSessionSetup_)
    {
      pickupReferenceClocksStation();
      pickupReferenceCoordinatesStation();
      checkUseOfManualPhaseCals();
    };
  }
  //
  setClockModelOrder4Stations(2);
  //
  // init aux sigmas:
  for (BaselinesByName_it it=baselinesByName_.begin(); it!=baselinesByName_.end(); ++it)
    it.value()->setSigma2add(DT_DELAY, 20.0e-12);
  //
  //
  for (int i=0; i<bands_.size(); i++)
  {
    bandIdxByKey[bands_.at(i)->getKey()] = i;
    if (bands_.at(i)->getKey() == primaryBand_->getKey())
      primaryBandIdx = i;
  };
  if (!bandIdxByKey.contains("X"))
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): nonstandard band naming: missed X-band");
  if (!bandIdxByKey.contains("S"))
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): nonstandard band naming: missed S-band");
  if (primaryBand_->getKey() != "X")
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): nonstandard band naming: the primary band is not X-band");
  //
  //
  //
  if (ap.doIonoCorrection4SBD_ && bands().size()>1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): evaluating ionospheric corrections for single band delays", true);
    calculateIonoCorrections(NULL);
  };
  //
  if (ap.doClockBreaksDetection_) 
  {
    if (check4ClockBreaksAtSBD())
    {
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::doPostReadActions4Ints(): a clock break was detected for SB delays, cannot proceed in "
        "automatic mode");
      resetAllEditings(true);
      return false;
    }
    else
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::doPostReadActions4Ints(): scan for clock break @SBD completed");
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): scan for clock break @SBD was not required");
  //
  //
  //
  if (ap.doAmbigResolution_)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): starting to resolve ambiguities", true);
    // SBDelay:
    parametersDescriptor_->unsetAllParameters();
    parIdxs.clear();
    parIdxs << SgParametersDescriptor::Idx_CLOCK_0 << SgParametersDescriptor::Idx_CLOCK_1;
    parametersDescriptor_->setMode4Parameters(SgParameterCfg::PM_LOC, parIdxs);
    process(true, false);
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): obtained SB solution", true);
    //
    // Group Delays:
    config_->setUseDelayType(SgTaskConfig::VD_GRP_DELAY);
    config_->setOpMode(SgTaskConfig::OPM_BASELINE);

    //
    for (int i=0; i<bands_.size(); i++)
    {
      config_->setActiveBandIdx(i);
      scanBaselines4GrDelayAmbiguities(i);
      process(true, false);
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::doPostReadActions4Ints(): finished " + 
        bands_.at(i)->getKey() + "-band clocks only solution");
      eliminateOutliersSimpleMode(i, (int)(observations_.size()*0.20), 7.0, 25.0e-9);
    };
    setClockModelOrder4Stations(3);
    parIdxs << SgParametersDescriptor::Idx_ZENITH;
    parametersDescriptor_->setMode4Parameters(SgParameterCfg::PM_LOC, parIdxs);
    process(true, false);
    for (int i=0; i<bands_.size(); i++)
    {
      config_->setActiveBandIdx(i);
      scanBaselines4GrDelayAmbiguities(i);
      process(true, false);
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::doPostReadActions4Ints(): finished " + 
        bands_.at(i)->getKey() + "-band clocks&zenith solution");
      eliminateOutliersSimpleMode(i, (int)(observations_.size()*0.20), 5.0, 15.0e-9);
    };
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): done ambiguities", true);
    config_->setActiveBandIdx(primaryBandIdx);
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): check for ambiguities was not required");
  //
  //
  if (ap.doClockBreaksDetection_)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): running clock break detections, early stage", true);
    detectAndProcessClockBreaks();
  };
  //
  //
  if (ap.doIonoCorrection4All_)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): adding ionospheric corrections", true);
    calculateIonoCorrections(config_);
  };
  //
  //
  if (ap.doOutliers_ || ap.doWeights_)
  {
    int                         numOfRestored, numOfEliminated;
    //
    // make a full solution:
    setClockModelOrder4Stations(3);
    parametersDescriptor_->unsetAllParameters();
    parIdxs.clear();
    parIdxs << SgParametersDescriptor::Idx_CLOCK_0 << SgParametersDescriptor::Idx_CLOCK_1 
            << SgParametersDescriptor::Idx_ZENITH;
    if (ap.finalSolution_ == SgTaskConfig::AutomaticProcessing::FS_BASELINE)
      parIdxs << SgParametersDescriptor::Idx_BL_LENGTH;
    else
      parIdxs << SgParametersDescriptor::Idx_POLUSUT1;

    parametersDescriptor_->setMode4Parameters(SgParameterCfg::PM_LOC, parIdxs);
    config_->setOpThreshold(3.0);
    config_->setActiveBandIdx(primaryBandIdx);
    config_->setUseDelayType(SgTaskConfig::VD_GRP_DELAY);
    config_->setOpMode(SgTaskConfig::OPM_BASELINE);
//    process(true, false);
    //
    // 
    if (ap.doWeights_)
      doReWeighting();
    else
      process(true, false);
    //
    /**/
    if (ap.doOutliers_)
    {
      numOfRestored = numOfEliminated = 0;
      do
      {
        if ((numOfRestored=restoreOutliers(primaryBandIdx)) && ap.doWeights_)
          doReWeighting();
        if ((numOfEliminated=eliminateOutliers(primaryBandIdx)) && ap.doWeights_)
          doReWeighting();
      }
      while (numOfRestored + numOfEliminated);
    };
    //
    config_->setDoWeightCorrection(false);
    if (ap.finalSolution_ == SgTaskConfig::AutomaticProcessing::FS_BASELINE)
    {
      parametersDescriptor_->unsetParameter(SgParametersDescriptor::Idx_BL_LENGTH);
      parametersDescriptor_->setMode4Parameter(SgParameterCfg::PM_LOC,
                                            SgParametersDescriptor::Idx_POLUSUT1);
      process(true, false);
    };
  };
  //
  //
  // restore pointers:
//  delete config_;
//  delete parametersDescriptor_;
//  config_ = config_saved;
//  parametersDescriptor_ = parametersDescriptor_saved;
  //
  config_->setOpThreshold(opThresholdSaved);
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::doPostReadActions4Ints(): processing the session " + name_ + 
    " in automatic mode has been finished");
  //
  //
  // check the results:
  if ((ap.doOutliers_ || ap.doWeights_) && primaryBand_->numProcessed(DT_DELAY) < numOfParameters_+1)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): looks like the automatic mode has failed: the number of good obs (" +
      QString("").setNum(primaryBand_->numProcessed(DT_DELAY)) +
      ") is close to the number of parameters (" + 
      QString("").setNum(numOfParameters_) + ")");
    return false;
  };
  if ((ap.doOutliers_ || ap.doWeights_) && 
        primaryBand_->numProcessed(DT_DELAY) < observations_.size()*0.3)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): looks like the automatic mode has failed: the number of good obs (" +
      QString("").setNum(primaryBand_->numProcessed(DT_DELAY)) +
      ") is less than 30% of total observations (" + 
      QString("").setNum(observations_.size()) + ")");
    return false;
  };
  if ((ap.doOutliers_ || ap.doWeights_) && 80.0 < primaryBand_->wrms(DT_DELAY)*1.0E12)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::doPostReadActions4Ints(): looks like the automatic mode has failed: the WRMS (" +
      QString("").setNum(primaryBand_->wrms(DT_DELAY)*1.0E12) + "ps) is too high");
    return false;
  };

  if (ap.doAmbigResolution_ && ap.doClockBreaksDetection_ && ap.doIonoCorrection4All_ &&
      ap.doOutliers_ && ap.doWeights_)
    addAttr(Attr_FF_AUTOPROCESSED);
  return true;
};



//
int SgVlbiSession::doReWeighting()
{
  config_->setDoWeightCorrection(true);
  process(true, false);
  double                        prevChi=primaryBand_->chi2(DT_DELAY);
  double                        delta=1.0e-3;
  int                           n, limit=7;
  process(true, false);
  n = 0;
  while (delta<fabs(prevChi-primaryBand_->chi2(DT_DELAY)) && n<limit)
  {
    prevChi = primaryBand_->chi2(DT_DELAY);
    process(true, false);
    n++;
  };
  return n;
};



//
void SgVlbiSession::pickupReferenceClocksStation()
{
  // first, check the presence of reference clock station:
  bool                          hasFound;
  hasFound = false;
  for (StationsByName_it it = stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    if (it.value()->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      hasFound = true;
  if (hasFound)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::pickupReferenceClocksStation(): a reference clock station was already set for the session");
    return;
  };
  //
  QList<QString>               &refStations=clockRefStations4Gen;
  if (networkID_ == "INT")
    refStations = clockRefStations4Int;
  else if (networkID_ == "IVS-R4")
    refStations = clockRefStations4R4;
  int                           num=refStations.size(), idx;
  SgVlbiStationInfo            *stn=NULL;
  idx = 0;
  hasFound = false;
  while (idx<num && !hasFound)
  {
    if ( (hasFound=stationsByName_.contains(refStations.at(idx))) )
      stn = stationsByName_[refStations.at(idx)];
    idx++;
  };
  if (!hasFound) // yet
  {
    stn = stationsByName_.begin().value();
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::pickupReferenceClocksStation(): cannot find any predefined stations in the session, "
      "use first available one");
  };
  if (stn)
  {
    stn->addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
    if (config_->getIsSolveCompatible())
      stn->setClocksModelOrder(0);
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::pickupReferenceClocksStation(): set station " + stn->getKey() +
      " as a reference clocks station");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::pickupReferenceClocksStation(): no reference clocks station set for the session");
};



//
void SgVlbiSession::setReferenceClocksStation(const QString& stnKey)
{
  // first, check the presence of reference clock station:
  if (!stationsByName_.contains(stnKey))
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::setReferenceClocksStation(): cannot find the station " + stnKey + " in the session");
    return;
  };
  //
  for (StationsByName_it it = stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    //
    if (stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) && stn->getKey()!=stnKey)
    {
      stn->delAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
      if (config_->getIsSolveCompatible())
        stn->setClocksModelOrder(3);
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::setReferenceClocksStation(): unset a station " + stn->getKey() +
        " as a reference clocks station");
    };
    if (stn->getKey()==stnKey && !stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
    {
      stn->addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
      if (config_->getIsSolveCompatible())
        stn->setClocksModelOrder(0);
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::setReferenceClocksStation(): set a station " + stn->getKey() +
        " as a reference clocks station");
    };
  };
};



//
void SgVlbiSession::pickupReferenceCoordinatesStation()
{
  QList<QString>               &refStations=coordRefStations4Gen;
  int                           num=refStations.size(), idx;
  bool                          hasFound;
  SgVlbiStationInfo            *stn=NULL;

  if (config_->getIsSolveCompatible())
  { 
    idx = 0;
    hasFound = false;
    while (idx<num && !hasFound)
    {
      if ( (hasFound=stationsByName_.contains(refStations.at(idx))) )
        stn = stationsByName_[refStations.at(idx)];
      idx++;
    };
    if (!hasFound) // yet
    {
      stn = stationsByName_.begin().value();
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::pickupReferenceCoordinatesStation(): cannot find any predefined stations in the session, "
        "use the first available one");
    };
    if (stn)
    {
      stn->delAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO);
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::pickupReferenceCoordinatesStation(): set a station " + stn->getKey() +
        " as a reference coordinate station");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::pickupReferenceCoordinatesStation(): no reference coordinate station set for the session");
  };
};



//
void SgVlbiSession::checkUseOfManualPhaseCals()
{
  QRegularExpression            reStationBlock("^\\+(?:STATION_NOTES|STATIONS)",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reNotesBlock("^\\+NOTES",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reManualPcalBlock("^\\+MANUAL_PCAL",
                                  QRegularExpression::CaseInsensitiveOption);
  
  QRegularExpression            reEndBlockMark("^\\+\\S+");
  

  QRegularExpressionMatch       match;
  //
  const QString                 phcBlockTag1("+MANUAL");
  const QString                 phcBlockTag2("PCAL");

  const SgVlbiHistory&          history=primaryBand_->history();

  QMap<QString, bool>           manPhCal1_ByKey; // from stations report
  QMap<QString, bool>           manPhCal2_ByKey; // from "+MANUAL PCAL" string
  QMap<QString, QString>        keyByAbbr;
  QMap<QString, QList<QString> >eventsByKey;
  
  QString                       stnName, stnAbbr;
  int                           idx;
  bool                          hasFound;
  bool                          isFmtVer3;

  isFmtVer3 = false;  
  idx = 0;
  hasFound = false;
  // search station notes block:
  while (idx<history.size() && !hasFound)
  {
    if ((match=reStationBlock.match(history.at(idx)->getText())).hasMatch())
      hasFound = true;
    idx++;
  };
  if (!hasFound)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::checkUseOfManualPhaseCals(): cannot find station notes block in the history records");
    return;
  };
  // parse it:
  if (match.captured(0) == "+STATIONS")
  {
    isFmtVer3 = true;
    //
    // parsing:
    //
    //+STATIONS
    //
    //station name     mk4
    //--------------------
    //Ft      FORTLEZA F
    //Hb      HOBART12 L
    //
    //* station  2-char station ID
    //* name     3- to 8-char station name
    //* mk4      1-char HOPS station code
    //
    //+NOTES
    //
    hasFound = false;
    stnName = "";
    stnAbbr = "";
    QRegularExpression          rx("^([A-Za-z0-9]{2})\\s+" + strReStationName + "\\s+([-A-Za-z0-9]{1})");
    QString                     sa1(""), sa2("");
    while (idx<history.size() && !hasFound)
    {
      const QString&            str=history.at(idx)->getText();
      if (reEndBlockMark.match(str).hasMatch())
        hasFound = true;
      else
      {
        if ((match=rx.match(str)).hasMatch())
        {
          sa2     = match.captured(1);
          stnName = match.captured(2);
          sa1     = match.captured(3);
//        std::cout << "stnName: [" << qPrintable(stnName) << "]"
//                  << " => (" << qPrintable(sa2) << ") -> {" << qPrintable(sa1) << "}\n";
          if (stnName.size() < 8)
            stnName = stnName.leftJustified(8, ' ');
          stnAbbr = (sa1.size()==2)?sa1:(sa2.size()==2?sa2:"");
          if (stnAbbr == "")
            logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
              "::checkUseOfManualPhaseCals(): cannot find stnAbbr, the pair is [" + sa1 +
              "," + sa2 + "]");
          else
            keyByAbbr[stnAbbr] = stnName;
        };
      };
      idx++;
    };
    //
    // search for "Notes" block:
    idx--;
    hasFound = false;
    while (idx<history.size() && !hasFound)
    {
      if (reNotesBlock.match(history.at(idx)->getText()).hasMatch())
        hasFound = true;
      idx++;
    };
    // parse it:
    hasFound = false;
    QRegularExpression          re("^([A-Za-z0-9]{2})\\s{3,}(.*)");
    while (idx<history.size() && !hasFound)
    {
      const QString&            str=history.at(idx)->getText();
      if (reEndBlockMark.match(str).hasMatch())
        hasFound = true;
      else
      {
        if ((match=re.match(str)).hasMatch())
        {
          sa2     = match.captured(1);
          sa1     = match.captured(2);
//        std::cout << "stnId: [" << qPrintable(sa2) << "]"
//                  << " => (" << qPrintable(sa1) << ")\n";
          if (keyByAbbr.contains(sa2))
            eventsByKey[keyByAbbr[sa2]] << sa1;
          else
            logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
              "::checkUseOfManualPhaseCals(): cannot find station ID [" + sa2 + "] in the station list");
        };
      };
      idx++;
    };
    //
    // search for manual ph.cal block:
    // start from the begin:
    if (!hasFound)
      idx=0;
    else
    {
      idx--;
      hasFound = false;
    };
    //
    while (idx<history.size() && !hasFound)
    {
      if (reManualPcalBlock.match(history.at(idx)->getText()).hasMatch())
        hasFound = true;
      idx++;
    };
    hasFound = false;
    while (idx<history.size() && !hasFound)
    {
      const QString&            str=history.at(idx)->getText();
      if (reEndBlockMark.match(str).hasMatch())
        hasFound = true;
      else
      {
        if (str.size() == 2)
        {
          if (keyByAbbr.contains(str))
          {
            manPhCal2_ByKey[keyByAbbr[str]] = true;
            logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
              "::checkUseOfManualPhaseCals(): the use of manual phasecal was mention for the station " + 
              keyByAbbr[str] + "(" + str + ") in the manual phasecal block");
          }
          else
            logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
              "::checkUseOfManualPhaseCals(): cannot find station abbreviation " + str + 
              " in the station description block");
        }
        else if (0 < str.size())
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::checkUseOfManualPhaseCals(): got an unexpected string: [" + str + 
            "] in the MAUAL_PCAL block");
      };
      idx++;
    };
  }
  else
  {
    //
    // parse the sequences: 
    // 
    //ISHIOKA  (Is/Q): Ok.
    //
    //KATH12M  (Ke/a): Applied manual pcal.
   
    hasFound = false;
    stnName = "";
    stnAbbr = "";
    QRegularExpression          rx("^\\s*" + strReStationName + "\\s*"
                                   "\\(([-A-Za-z0-9]{1,2})[-/]{1}([A-Za-z0-9]{1,2})\\):");
    QString                     sa1(""), sa2("");
    while (idx<history.size() && !hasFound)
    {
      const QString&            str=history.at(idx)->getText();
      if (reEndBlockMark.match(str).hasMatch())
        hasFound = true;
      else
      {
        if ((match=rx.match(str)).hasMatch())
        {
          stnName = match.captured(1);
          sa1     = match.captured(2);
          sa2     = match.captured(3);
//            std::cout << "stnName: [" << qPrintable(stnName) << "]"
//                      << " => (" << qPrintable(sa1) << ") -> {" << qPrintable(sa2) << "}\n";
          if (stnName.size() < 8)
            stnName = stnName.leftJustified(8, ' ');
          stnAbbr = (sa1.size()==2)?sa1:(sa2.size()==2?sa2:"");
          if (stnAbbr == "")
            logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
              "::checkUseOfManualPhaseCals(): cannot find stnAbbr, the pair is [" + sa1 +
              "," + sa2 + "]");
          else
          {
            keyByAbbr[stnAbbr] = stnName;
            sa1 = str.right(str.size() - str.indexOf(':') - 1);
            eventsByKey[stnName] << sa1;
          };
        }
        else if (str.simplified().size()>0 && stnName.size()>0)
          eventsByKey[stnName] << str.simplified();
      };
      idx++;
    };
    //
    //
    // search for manual ph.cal block:
    QString                       phcStr;
    // start from the begin:
    if (!hasFound)
      idx=0;
    else
    {
      idx--;
      hasFound = false;
    };
    //
    while (idx<history.size() && !hasFound)
    {
      if (history.at(idx)->getText().contains(phcBlockTag1) && 
          history.at(idx)->getText().contains(phcBlockTag2)   )
      {
        phcStr = history.at(idx)->getText();
        phcStr.replace(',', ' ');
        hasFound = true;
      }
      idx++;
    };
    if (!hasFound)
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): cannot find manual phasecal block in the history records");
    else
    {
      QStringList               manualPcalStns = phcStr.right(phcStr.size() - 
        phcStr.indexOf(phcBlockTag2) - phcBlockTag2.length()).split(" ", QString::SkipEmptyParts);
      for (int i=0; i<manualPcalStns.size(); i++)
      {
        const QString            &abbr=manualPcalStns.at(i);
        if (keyByAbbr.contains(abbr))
        {
          manPhCal2_ByKey[keyByAbbr[abbr]] = true;
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::checkUseOfManualPhaseCals(): the use of manual phasecal was mention for the station " + 
            keyByAbbr[abbr] + "(" + abbr + ") in the manual phasecal block");
        }
        else
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::checkUseOfManualPhaseCals(): cannot find station abbreviation " + abbr + 
            " in the station description block");
      };
    };
  };
  //
  //
  //
  // check collected stuff:
  if (!isFmtVer3)
  {
    if (keyByAbbr.size() != eventsByKey.size())
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): abbreviations (" + QString("").setNum(keyByAbbr.size()) + 
        ") and stations (" + QString("").setNum(eventsByKey.size()) + ") size mismatch");
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      if (!eventsByKey.contains(it.key()))
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::checkUseOfManualPhaseCals(): cannot find station " + it.key() +
          " in the station notes block of the history records");
  };
  //
  // debug output:
  if (false)
  {
    std::cout << "Got: " << keyByAbbr.size() << " abbrs and " << eventsByKey.size() << " stns \n";
    std::cout << " Abbrs: \n";
    for (QMap<QString, QString>::iterator it=keyByAbbr.begin(); it!=keyByAbbr.end(); ++it)
      std::cout << " abbr: [" << qPrintable(it.key()) << "],  key: [" << qPrintable(it.value()) << "]\n";
    std::cout << " Stns: \n";
    for (QMap<QString, QList<QString> >::iterator it=eventsByKey.begin(); it!=eventsByKey.end(); ++it)
    {
      std::cout << " key: [" << qPrintable(it.key()) << "]: " << "\n";
      const QList<QString> &lst=it.value();
      for (int i=0; i<lst.size(); i++)
        std::cout << "     " << i << ":  [" << qPrintable(lst.at(i)) << "]\n";
    };
  };
  // eod
  // check for manual phase calibration comments:
  for (QMap<QString, QList<QString> >::iterator it=eventsByKey.begin(); it!=eventsByKey.end(); ++it)
  {
    const QString              &sName=it.key();
    const QList<QString>       &lst=it.value();
    for (int i=0; i<lst.size(); i++)
    {
      const QString              &rec=lst.at(i);
      if (rec.contains("manual",  Qt::CaseInsensitive)    && 
          rec.contains("pcal",    Qt::CaseInsensitive)    &&
          rec.contains("applied", Qt::CaseInsensitive)
          )
        manPhCal1_ByKey[sName] = true;
      if (rec.contains("manual",  Qt::CaseInsensitive)    && 
          rec.contains("phase",   Qt::CaseInsensitive)    &&
          rec.contains("calibration", Qt::CaseInsensitive)&&
          rec.contains("applied", Qt::CaseInsensitive)
          )
        manPhCal1_ByKey[sName] = true;
      if (rec.contains("manual",  Qt::CaseInsensitive)    && 
          rec.contains("phasecal",Qt::CaseInsensitive)    &&
          rec.contains("applied", Qt::CaseInsensitive)
          )
        manPhCal1_ByKey[sName] = true;
      if (rec.contains("manual",  Qt::CaseInsensitive)    && 
          rec.contains("phase",   Qt::CaseInsensitive)    &&
          rec.contains("cal",     Qt::CaseInsensitive)    &&
          rec.contains("applied", Qt::CaseInsensitive)
          )
        manPhCal1_ByKey[sName] = true;
    };
    if (manPhCal1_ByKey.contains(sName) && manPhCal1_ByKey[sName])
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): found manual phasecal record for " + sName + 
        " in the station block");
  };
  //
  //
  // 
  //
  // compare two lists of stations with manual phase cals:
  for (QMap<QString, bool>::const_iterator it=manPhCal1_ByKey.begin(); it!=manPhCal1_ByKey.end(); ++it)
    if (!manPhCal2_ByKey.contains(it.key()))
    {
      // well, usually, they just forget to add the station, complain:
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): according to the station notes block manual phase cals "
        "were applied to station " + it.key() + ", but it was not listed in the manual phasecal block");
      // add it:
      manPhCal2_ByKey[it.key()] = true;
    };
  // and the second list, just to check:
  for (QMap<QString, bool>::const_iterator it=manPhCal2_ByKey.begin(); it!=manPhCal2_ByKey.end(); ++it)
  {
    const QString              &sName=it.key();
    if (!manPhCal1_ByKey.contains(sName))
      // well, usually, they just forget to add the station, complain:
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): according to the manual phasecal block manual phase cals "
        "were applied to station " + sName + ", but it was not mention in the station notes block");
    if (stationsByName_.contains(sName))
    {
      stationsByName_.value(sName)->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
      stationsByName_.value(sName)->addAttr(SgVlbiStationInfo::Attr_HAS_MANUAL_PCAL);
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): cable calibrations for station " + sName + 
        " have been turned off because of applied manual phase calibrations");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::checkUseOfManualPhaseCals(): cannot find station " + sName + " in the list of stations "
        "of the session, cannot turn off cable cals for it");
  };
};



//
void SgVlbiSession::setClockModelOrder4Stations(int n)
{
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    if (stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) && config_->getIsSolveCompatible())
      stn->setClocksModelOrder(0);
    else
      stn->setClocksModelOrder(n);
  };
};



//
bool SgVlbiSession::check4ClockBreaksAtSBD()
{
  bool                          isCbOccurred=false;
  SgClockBreakAgent             agent;
  QList<SgClockBreakAgent*>     cbas, existedCBs;
  QList<SgClockBreakAgent*>     cbAgents;
  QList<SgVlbiStationInfo*>     stations;
  QList<SgParametersDescriptor::ParIdx> 
                                parIdxs;
  int                           primaryBandIdx=-1;
  double                        wrms_0;
  
  for (int i=0; i<bands_.size(); i++)
    if (bands_.at(i)->getKey() == primaryBand_->getKey())
      primaryBandIdx = i;
  if (primaryBandIdx<0)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::check4ClockBreaksAtSBD(): cannot find an index of the primary band, check failed");
    return false;
  };
  
  cbas << &agent;

  if (stationsByName_.size() == 2) // one baseline, only one station need to be checked
  {
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      if (!it.value()->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
        stations << it.value();
  }
  else // otherwice, take all staitons into the list:
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      if (!it.value()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)) // well, at this stage there should no
                                                                  // be excluded stations, but who knows
        stations << it.value();
  //
  // obtain a solution:
  config_->setUseDelayType(SgTaskConfig::VD_SB_DELAY);
  config_->setActiveBandIdx(primaryBandIdx);
  parametersDescriptor_->unsetAllParameters();
  parIdxs.clear();
  parIdxs << SgParametersDescriptor::Idx_CLOCK_0 << SgParametersDescriptor::Idx_CLOCK_1;
  parametersDescriptor_->setMode4Parameters(SgParameterCfg::PM_LOC, parIdxs);
  process(true, false);
  //
  //
  eliminateOutliersSimpleMode(primaryBandIdx, (int)(observations_.size()*0.20), 7.0, 30.0e-9);
  //
  //
  wrms_0 = primaryBand_->wrms(DT_DELAY);
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::check4ClockBreaksAtSBD(): got a first SB solution", true);
  //
  //
  // pick up existing clock breaks:
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    SgBreakModel               &breakModel=stn->clockBreaks();
    for (int brkIdx=0; brkIdx<breakModel.size(); brkIdx++)
    {
      SgParameterBreak         *brk=breakModel.at(brkIdx);
      SgClockBreakAgent        *cbAgent=new SgClockBreakAgent;
      cbAgent->epoch_ = *brk;
      cbAgent->stn_   = stn;
      existedCBs << cbAgent;
    };
  };
  //
  // there is no sence to process each band separately for the single band delays
  for (int j=0; j<stations.size(); j++)
  {
    SgVlbiStationInfo          *stn=stations.at(j);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::check4ClockBreaksAtSBD(): processing " + stn->getKey() + " station", true);
    for (QMap<QString, SgVlbiAuxObservation*>::iterator it=stn->auxObservationByScanId()->begin();
      it!=stn->auxObservationByScanId()->end(); ++it)
    {
      if (it.value()->isAttr(SgVlbiAuxObservation::Attr_PROCESSED))
      {
        SgMJD                   t=*it.value();
        agent.epoch_ = t;
        agent.stn_ = stn;
        agent.shift_ = 0.0;
        agent.sigma_ = 0.0;
        if (existedCBs.size())
        {
          cbas.clear();
          cbas << existedCBs;
          cbas << &agent;
        };
        calculateClockBreaksParameters(cbas, primaryBand_);
        if (agent.sigma_>0.0 && fabs(agent.shift_/agent.sigma_)>5.0 && fabs(agent.shift_)>5.0e-9)
        {
          SgClockBreakAgent        *cba=new SgClockBreakAgent;
          cba->epoch_ = t;
          cba->stn_   = stn;
          cba->shift_ = agent.shift_;
          cba->sigma_ = agent.sigma_;
          cbAgents << cba;
        };
      };
    };
  };
  //
  //
  if (cbAgents.size() > 0)
  {
    cbas.clear();
    QMap< QString, QList<SgClockBreakAgent*> >
                                cbasByStn;
    for (int i=0; i<cbAgents.size(); i++)
    {
      SgClockBreakAgent        *cba=cbAgents.at(i);
      cbasByStn[cba->stn_->getKey()] << cba;
    };
    for (QMap< QString, QList<SgClockBreakAgent*> >::iterator it=cbasByStn.begin(); 
      it!=cbasByStn.end(); ++it)
    {
      SgClockBreakAgent        *a=NULL;
      double                    d=0.0;
      QList<SgClockBreakAgent*>&l=it.value();
      for (int i=0; i<l.size(); i++)
        if (l.at(i)->sigma_>0.0 && fabs(l.at(i)->shift_/l.at(i)->sigma_)>d)
        {
          a = l.at(i);
          d = fabs(l.at(i)->shift_/l.at(i)->sigma_);
        };
      if (a && d>0.0)
        cbas << a;
    };
    SgClockBreakAgent        *a=NULL;
    double                    d=0.0;
    for (int i=0; i<cbas.size(); i++)
      if (cbas.at(i)->sigma_>0.0 && fabs(cbas.at(i)->shift_/cbas.at(i)->sigma_)>d)
      {
        a = cbas.at(i);
        d = fabs(cbas.at(i)->shift_/cbas.at(i)->sigma_);
      };
    if (a && d>0.0) // ok, this is our break
    {
      correctClockBreak(primaryBand_, a->stn_, a->epoch_, round(1.0E9*a->shift_), false, false);
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::check4ClockBreaksAtSBD(): running additional solutions to "
        "check an effect of the clock break");
      process(true, false);
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::check4ClockBreaksAtSBD(): finished; old WRMS were " + 
        QString("").sprintf("%.3fps", wrms_0*1.0e12) + ", the new WRMS are " + 
        QString("").sprintf("%.3fps", primaryBand_->wrms(DT_DELAY)*1.0e12));
      if (wrms_0/primaryBand_->wrms(DT_DELAY) > 2.0)
        check4ClockBreaksAtSBD();
      else
      {
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::check4ClockBreaksAtSBD(): the clock break looks like not real, removing it");
        a->stn_->clockBreaks().delBreak(a->epoch_);
      };
    };
  };

  isCbOccurred = false;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    if (!it.value()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) && 
         it.value()->clockBreaks().size()>0)
    {
      SgVlbiStationInfo        *si=it.value();
      isCbOccurred = true;
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::check4ClockBreaksAtSBD(): detected SB clock break(s) at the station " + 
        si->getKey());
      for (int i=0; i<si->clockBreaks().size(); i++)
      {
        SgParameterBreak       *pb=si->clockBreaks().at(i);
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::check4ClockBreaksAtSBD(): on " + pb->toString(SgMJD::F_Simple) + "  with magnitude " +
          QString("").sprintf("%.2fps", pb->getA0()*1.0e12));
      };
  };
  return isCbOccurred;
};



//

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
QList<QString>
  clockRefStations4Int(QList<QString>() << "KOKEE   " << "TSUKUB32"),
  clockRefStations4R4 (QList<QString>() << "KOKEE   " << "WETTZELL" << "NYALES20"),
  clockRefStations4Gen(QList<QString>() << "KOKEE   " << "WETTZELL" << "NYALES20" << "TSUKUB32"
                                        << "MACGO12M" << "KOKEE12M" << "GGAO12M " << "WESTFORD");

QList<QString>
  coordRefStations4Gen(QList<QString>() << "WETTZELL" << "KOKEE   " << "WESTFORD" << "NYALES20"
                                        << "HOBART26" << "HARTRAO " << "ONSALA60"
  );

/*=====================================================================================================*/
