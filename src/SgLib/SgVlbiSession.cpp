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


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QMessageBox>
#else
#   include <QtGui/QMessageBox>
#endif



#include <SgVlbiSession.h>

#include <SgAPrioriData.h>
#include <SgCubicSpline.h>
#include <SgEccDat.h>
#include <SgEccRec.h>
#include <SgLogger.h>
#include <SgMasterRecord.h>
#include <SgTaskConfig.h>
#include <SgVector.h>
#include <SgVersion.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>
#include <SgVlbiObservation.h>



// aux functions:
bool wavelengthSortingOrderLessThan(SgVlbiBand*, SgVlbiBand*);
bool observationSortingOrderLessThan(SgVlbiObservation*, SgVlbiObservation*);
bool observationSortingOrderLessThan4newSession(SgVlbiObservation*, SgVlbiObservation*);



const QString                   SgVlbiSession::sSkipCode_("---");



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
QString SgVlbiSession::className()
{
  return "SgVlbiSession";
};



// CONSTRUCTORS:
//
// A constructor:
SgVlbiSession::SgVlbiSession() : 
  SgVlbiSessionInfo(),
  bands_(),
  bandByKey_(),
  observations_(),
  scanEpochs_(),
  observationByKey_(),
  stationsByName_(),
  baselinesByName_(),
  sourcesByName_(),
  tRefer_(tZero),
  storedConfig_(),
  lastProcessedConfig_(),
  stationsByIdx_(),
  baselinesByIdx_(),
  sourcesByIdx_(),
  calcInfo_(),
  path2Masterfile_("./"),
  path2APrioriFiles_("./"),
  path2TrpFiles_("./"),
  apStationPositions_(SgAPriories::DT_STN_POS),
  apStationVelocities_(SgAPriories::DT_STN_VEL),
  apSourcePositions_(SgAPriories::DT_SRC_POS),
  apSourceStrModel_(SgAPriories::DT_SRC_SSM),
  apAxisOffsets_(SgAPriories::DT_AXS_OFS),
  apHiFyEop_(),
  apStationGradients_(SgAPriories::DT_STN_GRD),
  externalWeights_(),
  externalErpFile_(),
  externalTrpFile_(),
  contemporaryHistory_(),
  skyFreqByIfId_(),
  userCorrectionsUse_(),
  userCorrectionsName_(),
  lastProcessed_(tZero)
{
  hasOceanTideContrib_ = false;
  hasPoleTideContrib_= false;
  hasEarthTideContrib_= false;
  hasWobblePxContrib_= false;
  hasWobblePyContrib_= false;
  hasWobbleNutContrib_= false;
  hasWobbleHighFreqContrib_ = false;
  hasUt1HighFreqContrib_ = false;
  hasFeedCorrContrib_= false;
  hasTiltRemvrContrib_= false;
  hasPxyLibrationContrib_ = false;
  hasUt1LibrationContrib_ = false;
  hasOceanPoleTideContrib_ = false;
  hasGpsIonoContrib_ = false;
  hasPoleTideOldContrib_ = false;
  hasOceanTideOldContrib_ = false;
  hasNdryContrib_= false;
  hasNwetContrib_= false;
  hasUnPhaseCalContrib_= false;
  hasGradPartials_ = false;
  hasCipPartials_ = false;
//  calcVersionValue_ = 0.0;
  primaryBand_ = NULL;

  // config & descriptor of parameters:
  config_ = NULL;
  parametersDescriptor_ = NULL;
  
  leapSeconds_ = 0.0;

  // parameters:
  pPolusX_ = NULL;
  pPolusY_ = NULL;
  pUT1_ = NULL;
  pUT1Rate_ = NULL;
  pPolusXRate_ = NULL;
  pPolusYRate_ = NULL;
  pNutX_ = NULL;
  pNutY_ = NULL;
  pNutXRate_ = NULL;
  pNutYRate_ = NULL;

  reporter_ = NULL;
  numOfParameters_ = 0;
  numOfConstraints_= 0;
  numOfDOF_= 0;

  hasOutlierEliminationRun_ = false;
  hasOutlierRestoratioRun_ = false;

  have2InteractWithGui_ = false;
  longOperationStart_ = NULL;
  longOperationProgress_ = NULL;
  longOperationStop_ = NULL;
  longOperationMessage_ = NULL;
  longOperationShowStats_ = NULL;
  
  numOfPts4ErpInterpolation_ = 15;

  args4Ut1Interpolation_ = NULL;
  tabs4Ut1Interpolation_ = NULL;
  hasUt1Interpolation_ = false;
  args4PxyInterpolation_ = NULL;
  tabs4PxyInterpolation_ = NULL;
  hasPxyInterpolation_ = false;
  tabsUt1Type_ = SgTidalUt1::CT_FULL;
  innerUt1Interpolator_ = NULL;
  innerPxyInterpolator_ = NULL;
  externalErpInterpolator_ = NULL;
  isAble2InterpolateErp_ = false;
  
  inputDriver_ = NULL;
  sidebandOrder_ = SO_UNDEF;
};



// A destructor:
SgVlbiSession::~SgVlbiSession()
{
  releaseParameters();
  stationsByIdx_.clear();
  baselinesByIdx_.clear();
  sourcesByIdx_.clear();

  primaryBand_ = NULL;
  for (StationsByName_it i=stationsByName_.begin(); i!=stationsByName_.end(); ++i)
    delete i.value();
  stationsByName_.clear();

  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    delete i.value();
  baselinesByName_.clear();

  for (SourcesByName_it i=sourcesByName_.begin(); i!=sourcesByName_.end(); ++i)
    delete i.value();
  sourcesByName_.clear();

  // not an onwer:
  config_ = NULL;
  parametersDescriptor_ = NULL;
  inputDriver_ = NULL;
  
  // free bands:
  for (int i=0; i<bands_.size(); i++)
    delete bands_.at(i);
  bands_.clear();
  bandByKey_.clear();

  // free observations:
  observationByKey_.clear();
  for (int i=0; i<observations_.size(); i++)
    delete observations_.at(i);
  observations_.clear();
  
  for (int i=0; i<scanEpochs_.size(); i++)
    delete scanEpochs_.at(i);
  scanEpochs_.clear();
  
  if (args4Ut1Interpolation_)
  {
    delete args4Ut1Interpolation_;
    args4Ut1Interpolation_ = NULL;
  };
  if (tabs4Ut1Interpolation_)
  {
    delete tabs4Ut1Interpolation_;
    tabs4Ut1Interpolation_ = NULL;
  };
  if (args4PxyInterpolation_)
  {
    delete args4PxyInterpolation_;
    args4PxyInterpolation_ = NULL;
  };
  if (tabs4PxyInterpolation_)
  {
    delete tabs4PxyInterpolation_;
    tabs4PxyInterpolation_ = NULL;
  };
  if (innerUt1Interpolator_)
  {
    delete innerUt1Interpolator_;
    innerUt1Interpolator_ = NULL;
  };
  if (innerPxyInterpolator_)
  {
    delete innerPxyInterpolator_;
    innerPxyInterpolator_ = NULL;
  };
  if (externalErpInterpolator_)
  {
    delete externalErpInterpolator_;
    externalErpInterpolator_ = NULL;
  };
  skyFreqByIfId_.clear();
  userCorrectionsUse_.clear();
  userCorrectionsName_.clear();
};



// FUNCTIONS:
//
//
void SgVlbiSession::calcTMean()
{
  tMean_ = (tStart_.toDouble() + tFinis_.toDouble())/2.0;
};



//
int SgVlbiSession::numberOfBands() const
{
  return bands_.size();
};



//
void SgVlbiSession::createParameters()
{
  QString eopPrefix = "EOP: ";
  releaseParameters();
  pPolusX_ = new SgParameter(eopPrefix + "P_x, mas");
  pPolusY_ = new SgParameter(eopPrefix + "P_y, mas");
  pUT1_ = new SgParameter(eopPrefix + "dUT1, ms");
  pUT1Rate_ = new SgParameter(eopPrefix + "dUT1Rate, ms/day");
  pPolusXRate_ = new SgParameter(eopPrefix + "dPxRate, mas/day");
  pPolusYRate_ = new SgParameter(eopPrefix + "dPyRate, mas/day");
  pNutX_ = new SgParameter(eopPrefix + "dX, mas");
  pNutY_ = new SgParameter(eopPrefix + "dY, mas");
  pNutXRate_ = new SgParameter(eopPrefix + "dXRate, mas/day");
  pNutYRate_ = new SgParameter(eopPrefix + "dYRate, mas/day");

  for (StationsByName_it i=stationsByName_.begin(); i!=stationsByName_.end(); ++i)
  {
    i.value()->createParameters();
    i.value()->clockBreaks().setT0(tRefer_);
  };
  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    i.value()->createParameters();
  // The sky is a neighborhood
  for (SourcesByName_it i=sourcesByName_.begin(); i!=sourcesByName_.end(); ++i)
    i.value()->createParameters();
  // So keep it down

/*
  // later, remove this part:
  for (int i=0; i<bands_.size(); i++)
  {
    SgVlbiBand                 *band=bands_.at(i);
    for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
      it.value()->createParameters();
  };
  //
*/
};



//
void SgVlbiSession::releaseParameters()
{
  for (StationsByName_it i=stationsByName_.begin(); i!=stationsByName_.end(); ++i)
    i.value()->releaseParameters();
  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    i.value()->releaseParameters();
  for (SourcesByName_it i=sourcesByName_.begin(); i!=sourcesByName_.end(); ++i)
    i.value()->releaseParameters();
/*
  // later, remove this part:
  for (int i=0; i<bands_.size(); i++)
  {
    SgVlbiBand                 *band=bands_.at(i);
    for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
      it.value()->releaseParameters();
  };
  //
*/
  if (pPolusX_)
  {
    delete pPolusX_;
    pPolusX_ = NULL;
  };
  if (pPolusY_)
  {
    delete pPolusY_;
    pPolusY_ = NULL;
  };
  if (pUT1_)
  {
    delete pUT1_;
    pUT1_ = NULL;
  };
  if (pUT1Rate_)
  {
    delete pUT1Rate_;
    pUT1Rate_ = NULL;
  };
  if (pPolusXRate_)
  {
    delete pPolusXRate_;
    pPolusXRate_ = NULL;
  };
  if (pPolusYRate_)
  {
    delete pPolusYRate_;
    pPolusYRate_ = NULL;
  };
  if (pNutX_)
  {
    delete pNutX_;
    pNutX_ = NULL;
  };
  if (pNutY_)
  {
    delete pNutY_;
    pNutY_ = NULL;
  };
  
  if (pNutXRate_)
  {
    delete pNutXRate_;
    pNutXRate_ = NULL;
  };
  if (pNutYRate_)
  {
    delete pNutYRate_;
    pNutYRate_ = NULL;
  };
};



//
bool SgVlbiSession::checkChannelSetup(bool guiExpected)
{
  //
  QMap<QString, QList<QString> >
                                strLstByBand;
  int                           num=0;
  QString                       str("");
  //
  // collect info:
  for (int i=0; i<bands_.size(); i++)
  {
    SgVlbiBand                 *band=bands_.at(i);
    for (BaselinesByName_it it=band->baselinesByName().begin(); it!=band->baselinesByName().end(); ++it)
    {
      SgVlbiBaselineInfo       *bi=it.value();
      if (!bi->numOfChanByCount().size())
        logger->write(SgLogger::WRN, SgLogger::DATA, className() +
          "::checkChannelSetup(): cannot check channels for baseline " + bi->getKey() + 
          ", the numOfChanByCount map is empty");
      else if (bi->numOfChanByCount().size() > 1)
      {
        num = bi->observables().size();
        str = bi->getKey() + ":  ";
        for (QMap<int, int>::const_iterator jt=bi->numOfChanByCount().begin(); 
          jt!=bi->numOfChanByCount().end(); ++jt)
          str += QString("").sprintf("%d (%.1f%%), ", jt.key(), jt.value()*100.0/num);
        str = str.left(str.size() - 2);
        strLstByBand[band->getKey()] << str;
      };
    };
  };
  //
  // make a report:
  if (strLstByBand.size())
  {
    QString                     str("");
    logger->write(SgLogger::WRN, SgLogger::DATA, className() +
      "::checkChannelSetup(): the session has a mixed channel setup:");
    for (QMap<QString, QList<QString> >::iterator it=strLstByBand.begin(); it!=strLstByBand.end(); ++it)
    {
      const QList<QString>&     strLst=it.value();
      QString                   key=it.key();
      str += "Numbers of channels at the " + key + "-band: \n";
      for (int i=0; i<strLst.size(); i++)
      {
        str += "\t" + strLst.at(i) + "\n";
        logger->write(SgLogger::WRN, SgLogger::DATA, className() +
          "::checkChannelSetup(): " + key + "-band: " + strLst.at(i));
      };
      str += "\n";
    };
    //
    if (guiExpected)
      QMessageBox::warning(NULL, "Warning: mixed channels setup",
        "The session contains mixed channels setup for the following baseline(s):\n\n" + str + 
        "\nSometimes this happens by mistake, you may need to report it to the correlator.");
  }
  else
    logger->write(SgLogger::INF, SgLogger::DATA, className() +
      "::checkChannelSetup(): the session is ok");
  //
  strLstByBand.clear();
  return true;
};



//
bool SgVlbiSession::selfCheck(bool guiExpected, const QStringList& suffixes)
{
  QVector<QChar>                abetka;
  QString                       str("");
  double                        dTmp;
  bool                          isOk=true;
  SgMJD                         t(tZero);
  int                           numBands(bands().size());
  QRegularExpression            reOldDbName("(\\d{2})([A-Z]{3})(\\d{2})([A-Z0-9]{1,2})");
  QRegularExpression            reNewDbName("(\\d{8})-([\\S]{2,12})");
  QRegularExpressionMatch       match;


  abetka.resize(14);
  abetka[ 0] = 'a';
  abetka[ 1] = 'b';
  abetka[ 2] = 'c';
  abetka[ 3] = 'd';
  abetka[ 4] = 'e';
  abetka[ 5] = 'f';
  abetka[ 6] = 'g';
  abetka[ 7] = 'h';
  abetka[ 8] = 'i';
  abetka[ 9] = 'j';
  abetka[10] = 'k';
  abetka[11] = 'l';
  abetka[12] = 'm';
  abetka[13] = 'n';
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the operation started", true);

  // make selfcheck here:
  if (!observations_.size()) // there is no observations at all
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::selfCheck(): the session [" + name_ + "] does not contain any observations");
    return false;
  };
  // sort in wavelength order:
  qSort(bands_.begin(), bands_.end(), wavelengthSortingOrderLessThan);


  //
  // check for deselected stations (the info is in baselines)
  checkExcludedStations();
  
  //
  // set up a primary band:
  if (!primaryBand_)
    setUpPrimaryBand();
  QString                       pBandKey(primaryBand_->getKey());
  logger->write(SgLogger::INF, SgLogger::IO_DBH | SgLogger::DATA, className() +
    "::selfCheck(): the " + pBandKey + "-band is set as the primary one");
  //
  QMap<QString, QMap<QString, QString> >
                                scanNameByIdByStn;
  QMap<QString, QMap<QString, int> >
                                numByScanIdBySrc;
  int                           iObs;
  dTmp = 0.0;
  iObs = 0;
  //
  // process the data in time order:
  for (QMap<QString, SgVlbiObservation*>::iterator it=observationByKey_.begin(); 
    it!=observationByKey_.end(); ++it, iObs++)
  {
    SgVlbiObservation          *obs=it.value();
    if (obs->getMediaIdx() == -1)
      obs->setMediaIdx(iObs);
    // mark unmatched observables:
    if (obs->observableByKey().size() != numBands)
      obs->addAttr(SgVlbiObservation::Attr_NOT_MATCHED);
    // set up identities:
    obs->setupIdentities();
    obs->setupActiveObservable(pBandKey);
    obs->selfCheck();
    scanNameByIdByStn[obs->stn_1()->getKey()][obs->getScanId()] = obs->getScanName();
    scanNameByIdByStn[obs->stn_2()->getKey()][obs->getScanId()] = obs->getScanName();
    numByScanIdBySrc[obs->src()->getKey()][obs->getScanId()]++;
    dTmp += obs->getDTecStdDev();
    /*
    SgVlbiObservable           *pbObs=obs->primeObs();
    if (pbObs)
    {
      
    };
    */
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the identities of observations were set up", true);
  //
  if (dTmp > 0.0)
  {
    addAttr(Attr_HAS_DTEC);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): found dTec values");
  };
  //
  // and sort it:
  //
//->  if (originType_==OT_MK4 || originType_==OT_KOMB)
  qSort(observations_.begin(), observations_.end(), observationSortingOrderLessThan4newSession);
//->  else
//->    qSort(observations_.begin(), observations_.end(), observationSortingOrderLessThan);

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the observations were sorted", true);

  //
  // collect scan epochs and check scan names:
  // This is a new session, check and adjust scan names:
  QMap<QString, QString>        scanIdByName;
  QMap<QString, QMap<QString, QString> >
                                duplicateByName;
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    if (t != obs->getMJD())
    {
      t = obs->getMJD();
      scanEpochs_.append(new SgMJD(t));
    };
    // collect scan names:
    if (!scanIdByName.contains(obs->getScanName()))
      scanIdByName.insert(obs->getScanName(), obs->getScanId());
    else if (scanIdByName[obs->getScanName()] != obs->getScanId())
    {
      if (!duplicateByName.contains(obs->getScanName()))
        duplicateByName[obs->getScanName()].insert(scanIdByName[obs->getScanName()], obs->getScanName());
      duplicateByName[obs->getScanName()].insert(obs->getScanId(), obs->getScanName());  
    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): " + QString("").setNum(scanEpochs_.size()) + 
    " scans have been found in the session", true);
  //
  // Check for duplicate scan names:
  if (duplicateByName.size())
  {
    logger->write(originType_==OT_KOMB?SgLogger::INF:SgLogger::WRN, SgLogger::PREPROC, className() +
      "::selfCheck(): found " + QString("").setNum(duplicateByName.size()) + 
      " duplicate scan names:", true);
    for (QMap<QString, QMap<QString, QString> >::iterator it=duplicateByName.begin(); 
      it!=duplicateByName.end(); ++it)
    {
      int                       idx=0;
      for (QMap<QString, QString>::iterator jt=it.value().begin(); jt!=it.value().end(); ++jt, idx++)
      {
        logger->write(originType_==OT_KOMB?SgLogger::INF:SgLogger::WRN, SgLogger::PREPROC, className() +
          "::selfCheck(): the scan ID " + jt.key() + " has a scan name \"" + jt.value() + "\"");
        // alter the scan name:
        if (originType_==OT_KOMB)
        {
          str = jt.value().simplified() + abetka[idx];
          jt.value() = str;
        };
      };
    };
    // correct scan names:
    if (originType_==OT_KOMB)
    {
      QString                   oldScanName("");
      for (int i=0; i<observations_.size(); i++)
      {
        SgVlbiObservation      *obs=observations_.at(i);
        if (duplicateByName.contains(obs->getScanName()))
        {
          oldScanName = obs->getScanName();
          if (!duplicateByName.value(obs->getScanName()).contains(obs->getScanId()))
            logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
              "::selfCheck(): cannot find the corrected scan name for scan ID " + obs->getScanId() + 
              " and scan name \"" + oldScanName + "\"");
          else
          {
            str = duplicateByName.value(obs->getScanName())[obs->getScanId()];
            if (str.size())
            {
              obs->setScanName(str);
              logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
                "::selfCheck(): the scan name of " + obs->getScanId() + 
                " scan has been changed from \"" + oldScanName + "\" to \"" + str + "\"");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
                "::selfCheck(): the corrected scan name is empty for scan ID " + obs->getScanId() + 
                " and scan name \"" + oldScanName + "\"");
          };
        }
      };
    };
  };
  scanIdByName.clear();
  duplicateByName.clear();
  //
  //
  //
  // set up band-dependent stuff:
  int                           numChan=0, bitsPerSample=0;
  double                        rbw=0.0;
  for (int i=0; i<numBands; i++)
  {
    bands_.at(i)->selfCheck();
    //
    numChan += bands_.at(i)->getMaxNumOfChannels();
    rbw += bands_.at(i)->getMaxNumOfChannels()*bands_.at(i)->sampleRate();

    if (bitsPerSample == 0)
      bitsPerSample = bands_.at(i)->bitsPerSample();
    else if (bitsPerSample != bands_.at(i)->bitsPerSample())
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::selfCheck(): at the " + bands_.at(i)->getKey() + 
        "-band there is a different value for bits per sample: " +
        QString("").sprintf("%d vs %d", bitsPerSample, bands_.at(i)->bitsPerSample()));

  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the selfcheck procedure for the bands was done", true);
  // set up the recording mode:
  recordingMode_.sprintf("NChan:%d Rbw:%.1f Bps:%d", numChan, rbw*1.0e-6, bitsPerSample);
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::selfCheck(): the record mode has been set to \"" + recordingMode_ + "\"");
  //
  // calculate baseline length:
  if (isAttr(Attr_HAS_CALC_DATA))
    for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    {
      SgVlbiBaselineInfo       *bi=i.value();
      SgVlbiStationInfo        *si1=NULL, *si2=NULL;
      str = bi->getKey().left(8);
      if (stationsByName_.contains(str))
        si1 = stationsByName_.value(str);
      else
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::selfCheck(): cannot find the station \"" + str + "\" in the map", true);
      str = bi->getKey().right(8);
      if (stationsByName_.contains(str))
        si2 = stationsByName_.value(str);
      else
        logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
          "::selfCheck(): cannot find the station \"" + str + "\" in the map", true);
      if (si1 && si2)
      {
        bi->setLength((si2->getR() - si1->getR()).module());
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::selfCheck(): set the length of the baseline \"" + bi->getKey() + "\" to " +
          QString("").sprintf("%.3f (m)", bi->getLength()));
      };
    };
  //
  // set up a clock reference attribute for the session:
  SgVlbiStationInfo            *bandRC=NULL;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    if (it.value()->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      bandRC = it.value();
  if (!bandRC) // there is no presetupped RC staion on a session level, pick one:
  {
    for (int i=0; i<numBands; i++)
    {
      SgVlbiBand               *band=bands_.at(i);
      for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
        if (it.value()->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
          bandRC = it.value();
    };
    if (bandRC) // one of bands has one of stations with RC-flag
      stationsByName_[bandRC->getKey()]->addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
  };
  if (bandRC && config_ && config_->getIsSolveCompatible())
    stationsByName_[bandRC->getKey()]->setClocksModelOrder(0);
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): a reference clock station has been set up", true);

  // set up proper time of creation:
  t = tZero;
  for (int i=0; i<numBands; i++)
  {
    if (!bands_.at(i)->observables().size()) // there is no observations at some band
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::selfCheck(): the band " + bands_.at(i)->getKey() + 
        " at the session " + name_ + " does not contain any observations");
      return false;
    };
    if (t<bands_.at(i)->getTCreation())
      t = bands_.at(i)->getTCreation();
  };
  //
  // set up correct tStart and tFinis:
  tStart_ = observations_.at(0)->getMJD();
  tFinis_ = observations_.at(observations_.size() - 1)->getMJD();
  setTCreation(t); // set up "creation time" to the oldest from the bands
  calcTMean();
  setupTimeRefer();
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): epochs of start/finish were established", true);
  //
  //
  // check masterfile:
  SgMasterRecord                mr;
  if (suffixes.size())
    mr.setSuffixes(suffixes);
  if (originType_==OT_MK4 || originType_==OT_KOMB)
  {
    // if user explicitly specified the database name:
    if (name_!="unnamed" && 2<name_.size())
    {
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::selfCheck(): trying to check the provided by a user the database name \"" + name_ + "\"");
      if (mr.lookupRecordByName(name_, path2Masterfile_))
      {
        sessionCode_ = mr.getCode();
        sessionType_ = mr.getType();
        officialName_ = mr.getName();
        if (correlatorName_.size() == 0)
          correlatorName_ = mr.getCorrelatedBy();
        submitterName_ = mr.getSubmittedBy();
        schedulerName_ = mr.getScheduledBy();
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::selfCheck(): a corresponding masterfile record was found");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::selfCheck(): cannot find a masterfile record for the database \"" + name_ + "\"");
    }
    else
    {
      QString                   sYear;
      int                       year(tStart_.calcYear());

      if (getExpectedMasterfileVersion() == 2)                    // version 2.0 of masterfile
        sYear.sprintf("%04d", year);
      else
        sYear.sprintf("%02d", year - (year>=2000?2000:1900));     // version 1.0 of masterfile
      if (mr.lookupRecordByCode(sessionCode_, sYear, path2Masterfile_))
      {
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::selfCheck(): a masterfile record was found");

        if (getExpectedMasterfileVersion() == 2)                  // version 2.0 of masterfile
          name_ = sYear + QString("").sprintf("%02d%02d", tStart_.calcMonth(), tStart_.calcDay()) + 
                  "-" + mr.getCode().toLower();
        else
          name_ = sYear + mr.getDate() + mr.getDbcCode();         // version 1.0 of masterfile

        networkSuffix_ = "+";
        officialName_ = mr.getName();
        if (mr.getType().size())
          sessionType_ = mr.getType();
        if (correlatorName_.size() == 0)
          correlatorName_ = mr.getCorrelatedBy();
        submitterName_ = mr.getSubmittedBy();
        schedulerName_ = mr.getScheduledBy();
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::selfCheck(): cannot find a masterfile record for the session code \"" + sessionCode_ + "\"");
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): for the session code: " + sessionCode_ + 
      ", the following attributes were assigned:");
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    type:          " + sessionType_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    name:          " + name_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    official name: " + officialName_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    scheduler/correlator/submitter: " + 
      schedulerName_ + "/" + correlatorName_ + "/" + submitterName_);
  }
  else
  {
    if (mr.lookupRecordByName(name_, path2Masterfile_))
    {
      sessionCode_ = mr.getCode();
      sessionType_ = mr.getType();
      officialName_ = mr.getName();
      if (correlatorName_.size() == 0)
        correlatorName_ = mr.getCorrelatedBy();
      submitterName_ = mr.getSubmittedBy();
      schedulerName_ = mr.getScheduledBy();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::selfCheck(): a masterfile record was found");
    }
    else
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::selfCheck(): cannot find a masterfile record for the session [" + name_ + "] ");
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): for the session name: " + name_ + 
      ", the following attributes were assigned:");
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    code:          " + sessionCode_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    type:          " + sessionType_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    official name: " + officialName_);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck():    scheduler/correlator/submitter: " + 
      schedulerName_ + "/" + correlatorName_ + "/" + submitterName_, true);
  };
  //
  //
  //
  // check for Eccs:
  if (originType_==OT_MK4 || originType_==OT_KOMB || !isAttr(Attr_HAS_CALC_DATA))
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::selfCheck(): skipping the eccentricities set up: incomplete data set ", true);
  }
  else if (config_)
  {
    SgEccDat                    eccDat(path2APrioriFiles_);
    if (config_->getEccentricitiesFileName().size() > 0)
    {
      eccDat.setFileName(config_->getEccentricitiesFileName());
      if (config_->getEccentricitiesFileName().at(0) == '/')
        eccDat.setPath2File("");
    };
    eccDat.importEccFile();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the eccentricity file was read", true);
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      if (!si->eccRec())
      {
        SgEccRec               *eccRec=eccDat.lookupRecord(si->getKey(), tRefer_);
        if (eccRec) // setup eccentricity:
        {
          si->eccRec() = new SgEccRec(*eccRec);
          si->setCdpNumber(eccRec->getNCdp());
          if (eccRec->getDR().module() > 0.0)
            si->adjustEccVector();
          addAttr(Attr_FF_ECC_UPDATED);
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::selfCheck(): set up the eccentricity for the station " + si->getKey());
        }
        else
        {
          si->eccRec() = new SgEccRec();
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::selfCheck(): cannot find the eccentricity record for the station " + si->getKey());
        };
      };
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the eccentricities of stations were set up", true);
  };
  //
  //
  //
  //
  // check for the external a priori data:
  if (originType_==OT_MK4 || originType_==OT_KOMB || !isAttr(Attr_HAS_CALC_DATA))
  {
    //
  }
  else if (config_)
  {
    collectAPriories();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the external a priori were collected", true);

    // lookup a file with external weights:
    externalWeights_.setSessionName(name_);
    lookupExternalWeights();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the file with external weights was read", true);
  };
  //
  //
  // perform station set up:
  // check for different observations of a station at the same epoch (a rare case, just for curiosity),
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->checkAuxObs(scanNameByIdByStn[si->getKey()]);
//    if (parametersDescriptor_)
//    {
//      si->pcClocks()->setPMode(parametersDescriptor_->getClock0Mode());
//      si->pcZenith()->setPMode(parametersDescriptor_->getZenithMode());
//    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the auxiliary observations were checked", true);
  //
  // sources:
  //
  //for (QMap<QString, QMap<QString, int> >::iterator it=numByScanIdBySrc.begin();
  //  it!=numByScanIdBySrc.end(); ++it)
  //{
  //  std::cout << " +++ source " << qPrintable(it.key()) << ":\n";
  //  for (QMap<QString, int>::iterator jt=it.value().begin(); jt!=it.value().end(); ++jt)
  //    std::cout << "            " << qPrintable(jt.key()) << " => " << jt.value() << "\n";
  //};
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
        ", it was removed from the whole session");
    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): numbers of scans per a source were calculated", true);
  //
  //
  if (calcInfo_.stations().size()==0)
  {
    calcInfo_.stations() = stationsByName_.values();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::selfCheck(): the calcInfo's list of stations is empty; set it up: " + 
      QString("").setNum(calcInfo_.stations().size()) + " stations in the list");
  };
  // 
  // guess the network ID:
  guessNetworkId();
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::selfCheck(): the network ID was set up", true);
  //
  checkChannelSetup(guiExpected);
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::selfCheck(): the session selfcheck has been completed", true);
  //
  return isOk;
};



//
void SgVlbiSession::setupTimeRefer()
{
  if (tStart_ == tFinis_)                             // trivial case
    tRefer_ = tStart_;
  else if (tStart_.getDate() != tFinis_.getDate())    // set tRefer to midnight:
  {
    tRefer_ = tFinis_;
    tRefer_.setTime(0); // 0hr
  }
  else
  {
    int                         nYr_s, nMon_s, nDay_s, nHr_s, nMi_s;
    int                         nYr_f, nMon_f, nDay_f, nHr_f, nMi_f;
    double                      dSec_s, dSec_f;
    
    tStart_.toYMDHMS_tr(nYr_s, nMon_s, nDay_s, nHr_s, nMi_s, dSec_s);
    tFinis_.toYMDHMS_tr(nYr_f, nMon_f, nDay_f, nHr_f, nMi_f, dSec_f);
    //
    if (nHr_s == nHr_f)
    {
      if (20 < nMi_f-nMi_s)
        nMi_s = 10*round(double(nMi_s + nMi_f)/20.0);
      else
        nMi_s = round(double(nMi_s + nMi_f)/2.0);
    }
    else if (nHr_s+1 == nHr_f)
    {
      if (nMi_s > 20)
      {
        nHr_s++;
        nMi_s = 0;
      }
      else
        nMi_s = 30;
    }
    else
    {
      nHr_s = round(double(nHr_s + nHr_f)/2.0);
      nMi_s = 0;
    };
    
    tRefer_.setUpEpoch(nYr_s, nMon_s, nDay_s, nHr_s, nMi_s, 0.0);
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::setupTimeRefer(): the reference epoch was set to " + tRefer_.toString());
};



//
void SgVlbiSession::setPrimaryBandByIdx(int idx)
{
  if (!bands_.size())
    return;
  if (idx<0 || bands_.size()<=idx)
    return;
  for (int i=0; i<bands_.size(); i++)
  {
    if (i==idx)
    {
      primaryBand_ = bands_.at(i);
      primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
    }
    else
      if (bands_.at(i)->isAttr(SgVlbiBand::Attr_PRIMARY))
        bands_.at(i)->delAttr(SgVlbiBand::Attr_PRIMARY);
  };
};



//
void SgVlbiSession::setUpPrimaryBand()
{
  // there is no any band:
  if (!bands_.size())
    return;

  // one band, trivia:
  if (bands_.size()==1)
  {
    primaryBand_ = bands_.at(0);
    primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
    return;
  };
  int numOfPrimaryBands=0;
  for (int i=0; i<bands_.size(); i++)
    if (bands_.at(i)->isAttr(SgVlbiBand::Attr_PRIMARY))
    {
      primaryBand_ = bands_.at(i);
      numOfPrimaryBands++;
    };
  if (numOfPrimaryBands==1) // ok, there is one pband, just return;
    return;

  if (numOfPrimaryBands>1)  // something get wrong, clear flags and set up pband
    for (int i=0; i<bands_.size(); i++)
      bands_.at(i)->delAttr(SgVlbiBand::Attr_PRIMARY);
  primaryBand_ = NULL;
  for (int i=0; i<bands_.size(); i++)
  {
    if (bands_.at(i)->getKey() == "X")
    {
      primaryBand_ = bands_.at(i);
      primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
    };
  };
  if (!primaryBand_) // "X"-band was not found
  {
    primaryBand_ = bands_.at(0); // first element, highest freq
    primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
  };
};



//
void SgVlbiSession::prepare4Analysis()
{
  // clear statistics:
  // common observations:
  for (StationsByName_it i=stationsByName_.begin(); i!=stationsByName_.end(); ++i)
  {
    i.value()->clearWrms();
    QMap<QString, SgVlbiAuxObservation*>::iterator auxObsIt=i.value()->auxObservationByScanId()->begin();
    for (; auxObsIt!=i.value()->auxObservationByScanId()->end(); ++auxObsIt)
    {
      SgVlbiAuxObservation     *auxObs=auxObsIt.value();
      auxObs->setEstClocks(0.0);
      auxObs->setEstClocksSigma(0.0);
      auxObs->setEstZenithDelay(0.0);
      auxObs->setEstZenithDelaySigma(0.0);
      auxObs->setEstAtmGradN(0.0);
      auxObs->setEstAtmGradNSigma(0.0);
      auxObs->setEstAtmGradE(0.0);
      auxObs->setEstAtmGradESigma(0.0);
      auxObs->delAttr(SgObservation::Attr_PROCESSED);
    };
  };
  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    i.value()->clearWrms();
  for (SourcesByName_it i=sourcesByName_.begin(); i!=sourcesByName_.end(); ++i)
    i.value()->clearWrms();
  // per band observations:
  for (int iBand=0; iBand<bands_.size(); iBand++)
  {
    SgVlbiBand                 *band=bands_.at(iBand);
    band->clearWrms();
    for (StationsByName_it i=band->stationsByName().begin(); i!=band->stationsByName().end(); ++i)
      i.value()->clearWrms();
    for (BaselinesByName_it i=band->baselinesByName().begin(); i!=band->baselinesByName().end(); ++i)
      i.value()->clearWrms();
    for (SourcesByName_it i=band->sourcesByName().begin(); i!=band->sourcesByName().end(); ++i)
      i.value()->clearWrms();
  };
};



//
void SgVlbiSession::calculateIonoCorrections(const SgTaskConfig* cfg)
{
  if (!observations_.size())
    return;
  for (int i=0; i<observations_.size(); i++)
    observations_.at(i)->calcIonoCorrections(cfg);
  if (cfg)
  {
    addAttr(Attr_HAS_IONO_CORR);
    addAttr(Attr_FF_ION_C_CALCULATED);
    addAttr(Attr_FF_AMBIGS_RESOLVED); // assume ambigs are ok so set the flag on.
  };
};



//
void SgVlbiSession::zerofyIonoCorrections(const SgTaskConfig*)
{
  for (int i=0; i<observations_.size(); i++)
    observations_.at(i)->zerofyIono();
  delAttr(Attr_HAS_IONO_CORR);
  delAttr(Attr_FF_ION_C_CALCULATED);
};



//
void SgVlbiSession::calculateBaselineClock_F1()
{
  for (int iBand=0; iBand<bands_.size(); iBand++)
  {
    SgVlbiBand                 *band=bands_.at(iBand);
    QMap<QString, SgVlbiBaselineInfo*>::iterator itBaselineInfo=band->baselinesByName().begin();
    for (int iBaseline=0; itBaselineInfo!=band->baselinesByName().end(); ++itBaselineInfo, iBaseline++)
    {
      SgVlbiBaselineInfo       *baselineInfo=itBaselineInfo.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        baselineInfo->calculateClockF1(config_);
    };
  };
};



//
void SgVlbiSession::zerofySigma2add()
{
  double                        sig4bln4del=0.0, sig4bln4rat=0.0, sig4bnd4del=0.0, sig4bnd4rat=0.0;
  if (config_->getWcMode() == SgTaskConfig::WCM_BAND)
  {
    sig4bnd4del = config_->getInitAuxSigma4Delay();
    sig4bnd4rat = config_->getInitAuxSigma4Rate();
  }
  else if (config_->getWcMode() == SgTaskConfig::WCM_BASELINE)
  {
    sig4bln4del = config_->getInitAuxSigma4Delay();
    sig4bln4rat = config_->getInitAuxSigma4Rate();
  }
  else
  {
    //
  };
  for (int iBand=0; iBand<bands_.size(); iBand++)
  {
    bands_.at(iBand)->setSigma2add(DT_DELAY, sig4bnd4del);
    bands_.at(iBand)->setSigma2add(DT_RATE,  sig4bnd4rat);
    for (BaselinesByName_it jt=bands_.at(iBand)->baselinesByName().begin(); 
      jt!=bands_.at(iBand)->baselinesByName().end(); ++jt)
    {
      jt.value()->setSigma2add(DT_DELAY, sig4bln4del);
      jt.value()->setSigma2add(DT_RATE,  sig4bln4rat);
    };
  };
  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
  {
    i.value()->setSigma2add(DT_DELAY, sig4bln4del);
    i.value()->setSigma2add(DT_RATE,  sig4bln4rat);
  };
};



//
void SgVlbiSession::resetAllEditings(bool have2resetConfig)
{
  // Attributes:
  delAttr(Attr_PRE_PROCESSED);
  delAttr(Attr_HAS_CLOCK_BREAKS);
  delAttr(Attr_HAS_IONO_CORR);
  delAttr(Attr_FF_OUTLIERS_PROCESSED);
  delAttr(Attr_FF_AMBIGS_RESOLVED);
  delAttr(Attr_FF_WEIGHTS_CORRECTED);
  delAttr(Attr_FF_ION_C_CALCULATED);
  delAttr(Attr_FF_ECC_UPDATED);
  delAttr(Attr_FF_EDIT_INFO_MODIFIED);
  

  // bands:
  for (int i=0; i<bands_.size(); i++)
    bands_.at(i)->resetAllEditings();
  //
  // stations:
  for (QMap<QString, SgVlbiStationInfo*>::const_iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    it.value()->resetAllEditings();
  //
  // sources:
  for (QMap<QString, SgVlbiSourceInfo*>::const_iterator it=sourcesByName_.begin(); 
    it!=sourcesByName_.end(); ++it)
    it.value()->resetAllEditings();
  //
  // baselines:
  for (QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=baselinesByName_.begin(); 
    it!=baselinesByName_.end(); ++it)
    it.value()->resetAllEditings();
  //
  // observations:
  for (int i=0; i<observations_.size(); i++)
    observations_.at(i)->resetAllEditings();
  //
  // config:
  if (have2resetConfig)
    *config_ = storedConfig_; // ? do we need to reset the config at all?
  //
  // parameterization:
  *parametersDescriptor_ = storedParametersDescriptor_;
  //
  suppressNotSoGoodObs();
  //
  const SgTaskConfig::AutomaticProcessing
                               *ap=(config_->apByNetId().contains(networkID_)?
                               &config_->apByNetId()[networkID_] : 
                               &config_->apByNetId()[defaultNetIdName]);
  if (ap->doSessionSetup_)
  {
    pickupReferenceClocksStation();
    pickupReferenceCoordinatesStation();
    checkUseOfManualPhaseCals();
  };
  //
  numOfParameters_ = 0;
  numOfConstraints_= 0;
  numOfDOF_= 0;
  //
  logger->write(SgLogger::INF, SgLogger::DATA, className() +
    "::resetAllEditings(): all editings have been cleared");
};



//
QString SgVlbiSession::name4SirFile(bool isThroughCatalog)
{
  QString                       str(name_);
  QString                       altName(config_->getName());
  if (altName.size() > 0)
    altName += ".";
  str += QString("").sprintf("_V%03d_M%02d_C%c.%ssir", 
    primaryBand_->getInputFileVersion(), (int)originType_, isThroughCatalog?'Y':'N', 
    qPrintable(altName));
  return str;
};



//
bool SgVlbiSession::saveIntermediateResults(QDataStream& s) const
{
  s << name_ << getAttributes();
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data");
    return false;
  };
  // store the current version too:
  if (!libraryVersion.saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data for the version");
    return false;
  };
  //
  // bands:
  for (int i=0; i<bands_.size(); i++)
  {
    if (!bands_.at(i)->saveIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data for " + bands_.at(i)->getKey() + "-band: " + 
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  }
  //
  // stations:
  for (QMap<QString, SgVlbiStationInfo*>::const_iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    if (!it.value()->saveIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data for station " + it.value()->getKey() +  ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // sources:
  for (QMap<QString, SgVlbiSourceInfo*>::const_iterator it=sourcesByName_.begin(); 
    it!=sourcesByName_.end(); ++it)
    if (!it.value()->saveIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data for source " + it.value()->getKey() + ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // baselines:
  for (QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=baselinesByName_.begin(); 
    it!=baselinesByName_.end(); ++it)
    if (!it.value()->saveIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data for baseline " + it.value()->getKey() +  ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // observations:
  for (int i=0; i<observations_.size(); i++)
    if (!observations_.at(i)->saveIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data for observation " + 
        observations_.at(i)->getKey() + ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // config:
  if (!config_->saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data for config: " + 
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  // parameterization:
  if (!parametersDescriptor_->saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data for parameters setup: " + 
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };

  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiSession::loadIntermediateResults(QDataStream& s)
{
  QString                       name;
  unsigned int                  attributes;
  SgVersion                     version(libraryVersion);
  s >> name >> attributes;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (getName() != name)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: wrong session, names mismatch: got [" + name +
      "], expected [" + getName() + "]");
    return false;
  };
  if (!version.loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data for version: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
/* 
  if (version != libraryVersion)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): version mismatch, wouldn't dare to read that file");
    return false;
  };
*/
  if (version != libraryVersion)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): version mismatch, could get a problem");
//  return false;
  };
  //
  // bands:
  for (int i=0; i<bands_.size(); i++)
    if (!bands_.at(i)->loadIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error writting data for " + bands_.at(i)->getKey() + "-band: " + 
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // stations:
  for (QMap<QString, SgVlbiStationInfo*>::const_iterator it=stationsByName_.begin(); 
    it!=stationsByName_.end(); ++it)
    if (!it.value()->loadIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error writting data for station " + it.value()->getKey() +  ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // sources:
  for (QMap<QString, SgVlbiSourceInfo*>::const_iterator it=sourcesByName_.begin(); 
    it!=sourcesByName_.end(); ++it)
    if (!it.value()->loadIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error writting data for source " + it.value()->getKey() + ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // baselines:
  for (QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=baselinesByName_.begin(); 
    it!=baselinesByName_.end(); ++it)
    if (!it.value()->loadIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error writting data for baseline " + it.value()->getKey() +  ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // observations:
  for (int i=0; i<observations_.size(); i++)
    if (!observations_.at(i)->loadIntermediateResults(s))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error writting data for observation " + 
        observations_.at(i)->getKey() + ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  //
  // config:
  if (!config_->loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error writting data for config: " + 
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  // parameterization:
  if (!parametersDescriptor_->loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error writting data for parameters setup: " + 
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  setAttributes(attributes);
  //
  return s.status()==QDataStream::Ok;
};



//
bool SgVlbiSession::isInUse(SgVlbiBaselineInfo* bi)
{
  if (!bi)
    return false;
  if (baselinesByName_.contains(bi->getKey()))
    return 
      (!baselinesByName_.value(bi->getKey())->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID) && 
        baselinesByName_.value(bi->getKey())->numProcessed(DT_DELAY));
  else
    return false;
};



//
bool SgVlbiSession::isInUse(SgVlbiStationInfo* si)
{
  if (!si)
    return false;
  if (stationsByName_.contains(si->getKey()))
    return 
      (!stationsByName_.value(si->getKey())->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
        stationsByName_.value(si->getKey())->numProcessed(DT_DELAY));
  else
    return false;
};



//
void SgVlbiSession::allocUt1InterpolEpochs(double t0, double dt, unsigned int numOfPts)
{
  if (args4Ut1Interpolation_ && args4Ut1Interpolation_->n() != numOfPts)
  {
    delete args4Ut1Interpolation_;
    args4Ut1Interpolation_ = NULL;
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocUt1InterpolEpochs(): the vector of UT1 arguments has been deleted");
  };
  if (!args4Ut1Interpolation_)
  {
    args4Ut1Interpolation_ = new SgVector(numOfPts);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocUt1InterpolEpochs(): the vector of UT1 arguments has been created");
  };
  for (unsigned int i=0; i<numOfPts; i++)
    args4Ut1Interpolation_->setElement(i, t0 + dt*i);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::allocUt1InterpolEpochs(): the vector of UT1 arguments has been filled");
};



//
void SgVlbiSession::allocPxyInterpolEpochs(double t0, double dt, unsigned int numOfPts)
{
  if (args4PxyInterpolation_ && args4PxyInterpolation_->n() != numOfPts)
  {
    delete args4PxyInterpolation_;
    args4PxyInterpolation_ = NULL;
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocPxyInterpolEpochs(): the vector of Pxy arguments has been deleted");
  };
  if (!args4PxyInterpolation_)
  {
    args4PxyInterpolation_ = new SgVector(numOfPts);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocPxyInterpolEpochs(): the vector of Pxy arguments has been created");
  };
  for (unsigned int i=0; i<numOfPts; i++)
    args4PxyInterpolation_->setElement(i, t0 + dt*i);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::allocPxyInterpolEpochs(): the vector of Pxy arguments has been filled");
};



//
void SgVlbiSession::allocUt1InterpolValues(const SgVector* vals)
{
  if (!vals)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::allocUt1InterpolValues(): the input vector is NULL");
    return;
  };
  if (vals->n() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::allocUt1InterpolValues(): the size of the input vector is zero");
    return;
  };

  if (tabs4Ut1Interpolation_ && tabs4Ut1Interpolation_->nRow() != vals->n())
  {
    delete tabs4Ut1Interpolation_;
    tabs4Ut1Interpolation_ = NULL;
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocUt1InterpolValues(): the matrix of UT1 values has been deleted");
  };
  if (!tabs4Ut1Interpolation_)
  {
    tabs4Ut1Interpolation_ = new SgMatrix(vals->n(), 1);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocUt1InterpolValues(): the matrix of UT1 values has been created");
  };
  for (unsigned int i=0; i<vals->n(); i++)
    tabs4Ut1Interpolation_->setElement(i, 0, vals->getElement(i));
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::allocUt1InterpolValues(): the matrix of UT1 values has been filled");
};



//
void SgVlbiSession::allocPxyInterpolValues(const SgMatrix* vals)
{
  if (!vals)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::allocPxyInterpolValues(): the input matrix is NULL");
    return;
  };
  if (vals->nRow()==0 || vals->nCol()!=2)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::allocPxyInterpolValues(): the size of the input matrix is wrong");
    return;
  };
  if (tabs4PxyInterpolation_ && tabs4PxyInterpolation_->nRow() != vals->nRow())
  {
    delete tabs4PxyInterpolation_;
    tabs4PxyInterpolation_ = NULL;
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocPxyInterpolValues(): the matrix of Pxy values has been deleted");
  };
  if (!tabs4PxyInterpolation_)
  {
    tabs4PxyInterpolation_ = new SgMatrix(vals->nRow(), 2);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::allocPxyInterpolValues(): the matrix of Pxy values has been created");
  };
  for (unsigned int i=0; i<vals->nRow(); i++)
  {
    tabs4PxyInterpolation_->setElement(i, 0,  vals->getElement(i, 0));
    tabs4PxyInterpolation_->setElement(i, 1,  vals->getElement(i, 1));
  };
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::allocPxyInterpolValues(): the matrix of Pxy values has been filled");
};



//
void SgVlbiSession::propagateChannelBandwidth(double bw)
{
  for (int i=0; i<bands_.size(); i++)
    bands_.at(i)->propagateChannelBandwidth(bw);
  addAttr(Attr_FF_CHANNEL_INFO_MODIFIED);
};


/*=====================================================================================================*/






/*=====================================================================================================*/
//
//                           AUX Functions:
// 
/*=====================================================================================================*/
// sorts bands by frequency:
bool wavelengthSortingOrderLessThan(SgVlbiBand *b1, SgVlbiBand *b2)
{
  return b1->getFrequency() > b2->getFrequency();
};



//
bool observationSortingOrderLessThan(SgVlbiObservation *obs1, SgVlbiObservation *obs2)
{
  SgVlbiObservable             *o1, *o2;
  o1 = obs1->primeObs();
  o2 = obs2->primeObs();
  if (obs1->getMJD() == obs2->getMJD())
  {
    if (o1 && o2)
      return o1->getMediaIdx() < o2->getMediaIdx();
    else
      return obs1->getMediaIdx() < obs2->getMediaIdx();
  };
  return obs1->getMJD() < obs2->getMJD();
};



//
bool observationSortingOrderLessThan4newSession(SgVlbiObservation *obs1, SgVlbiObservation *obs2)
{
  if (obs1->getMJD() == obs2->getMJD())
  {
    if (obs1->src()->getKey() == obs2->src()->getKey())
    {
      if (obs1->stn_1()->getKey() == obs2->stn_1()->getKey())
        return obs1->stn_2()->getKey() < obs2->stn_2()->getKey();
      else 
        return obs1->stn_1()->getKey() < obs2->stn_1()->getKey();
    }
    else
      return obs1->src()->getKey() < obs2->src()->getKey();
  };
  return obs1->getMJD() < obs2->getMJD();
};
/*=====================================================================================================*/

/*=====================================================================================================*/
