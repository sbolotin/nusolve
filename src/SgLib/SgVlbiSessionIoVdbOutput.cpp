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


#include <QtCore/QRegExp>


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgEccRec.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>



// to do: add ACM

//
extern const QString            sCalList[], sFclList[];


/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/
//
bool SgVlbiSession::putDataIntoVgosDb(SgVgosDb* vgosDb)
{
  QString                       str("");
  SgMJD                         startEpoch(SgMJD::currentMJD());
  bool                          isOk(true);
  int                           numOfObs=observations_.size();
  int                           numOfChans;

  if (vgosDb->getOperationMode() == SgNetCdf::OM_DRY_RUN)
    std::cout << "\nRunning in a DRY RUN mode, no files will be created:\n\n";


  // AGV specific stuff:
  if (originType_ == OT_AGV)
  {
    str = getCorrelatorType();
    if (str.contains("MK3", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_Mk3);
    else if (str.contains("MK4", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_Mk4);
    else if (str.contains("VLBA", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_VLBA);
    else if (str.contains("CRL", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_CRL);
    else if (str.contains("GSI", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_GSI);
    else if (str.contains("S2", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_S2);
    else if (str.contains("difx", Qt::CaseInsensitive))
      vgosDb->setCorrelatorType(SgVgosDb::CT_Difx);
    else
    {
      vgosDb->setCorrelatorType(SgVgosDb::CT_Unknown);
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): canot guess correlator type: \"" + str + "\"");
    };
  };


  // new session, no vgosDb tree exists yet, or 
  //    some updates to channel info
  if (isAttr(Attr_FF_CREATED) || isAttr(Attr_FF_CHANNEL_INFO_MODIFIED))
  {
    QList<SgVector*>            vectors2del;
    QList<SgMatrix*>            matrices2del;

    QList<SgMJD>                epochs4Scans;
    QList<SgMJD>                epochs4Obs;
    QMap<QString, SgMJD>        epochByScanId;
    QList<QString>              stations_1, stations_2, sources;
    QList<QString>              scanNames, scanFullNames, crootFnames;
    //
    QList<QString>              bands;
    QMap<QString, SgMatrix*>    sbDelaysByBand, grDelaysByBand, phRatesByBand, phasesByBand;
    QMap<QString, SgMatrix*>    grDelaysAltByBand, phRatesAltByBand;
    QMap<QString, SgMatrix*>    phaseCal_1ByBand, phaseCal_2ByBand;
    QMap<QString, SgVector*>    ambigsByBand, refFreqsByBand, correlationByBand, snrByBand;
    QMap<QString, QVector<char> >
                                qualityCodesByBand;
    //
    QMap<QString, int>          maxNumOfChanByBand;
    QMap<QString, QVector<int> >
                                numOfChannelsByBand, dataFlagsByBand;
    QMap<QString, QVector<QString> >
                                fourfitOutputFNameByBand;
    QMap<QString, SgVector*>    sampleRateByBand;
    QMap<QString, SgMatrix*>    numOfAccPeriodsByChan_USBByBand, numOfAccPeriodsByChan_LSBByBand,
                                refFreqByChanByBand,
                                phCalAmps_1ByChanByBand, phCalAmps_2ByChanByBand, 
                                phCalPhss_1ByChanByBand, phCalPhss_2ByChanByBand, 
                                phCalFrqs_1ByChanByBand, phCalFrqs_2ByChanByBand,
                                phCalOffs_1ByChanByBand, phCalOffs_2ByChanByBand,
                                numOfSamplesByChan_USBByBand,
                                numOfSamplesByChan_LSBByBand, 
                                residualFringeAmplByChanByBand, 
                                residualFringePhseByChanByBand;
    //
    QList<int>                  obs2scan, obs2stn_1, obs2stn_2;
    QList<int>                  scan2src;
    // StationCrossRef's clients:
    QList<int>                  numScansPerStation;
    QMap<QString, QList<int> >  station2Scan, scan2Station;
    //
    QMap<QString, int>          stnIdxByName;
    QMap<QString, int>          srcIdxByName;
    int                         n;
    char                        buff[32];
    // correlInfo's aux stuff:
    QMap<QString, QVector<QString> > 
                                fourfitErrorCodesByBand, frqGrpCodesByBand,
                                fourfitControlFileByBand, fourfitCommandByBand;
    QMap<QString, QVector<int> >corelVersionsByBand, startOffsetsByBand, bitsPerSamplesByBand,
                                numLagsByBand;
    QMap<QString, QVector<double> >
                                apLengthByBand;
    QMap<QString, QVector< QVector<int> > > 
                                fourfitVersionsByBand, dbeditVersionByBand;
//    QMap<QString, SgVector*>    refClkErrByBand, deltaEpochsByBand;
    QMap<QString, SgVector*>    deltaEpochsByBand;
    QMap<QString, SgMatrix*>    urvrsByBand, instrDelaysByBand, starElevByBand, zenithDelaysByBand, 
                                searchParamsByBand;
    QMap<QString, SgMatrix*>    mDlysByBand, mAuxByBand;
    QMap<QString, QVector<QString> > 
                                baselineCodesByBand, tapeQualityCodesByBand;
    QMap<QString, QVector<int> >stopOffsetsByBand, hopsRevisionNumbersByBand;
    QMap<QString, SgVector*>    sbdResidsByBand, rateResidsByBand, effDurationsByBand, 
                                startSecondsByBand, stopSecondsByBand, percentOfDiscardsByBand,
                                uniformityFactorsByBand, geocenterPhaseResidsByBand,
                                geocenterPhaseTotalsByBand, geocenterSbDelaysByBand,
                                geocenterGrDelaysByBand, geocenterDelayRatesByBand,
                                probOfFalsesByBand, corrAprioriDelaysByBand, corrAprioriRatesByBand,
                                corrAprioriAccsByBand, incohAmpsByBand, incohAmps2ByBand,
                                delayResidsByBand, phaseCalRates_1ByBand, phaseCalRates_2ByBand;
    //
    QMap<QString, QVector<QString> > 
                                corrRootFileNamesByBand, tapeIds_1ByBand, tapeIds_2ByBand, 
                                channelIdsByBand, polarizationsByBand;
    QMap<QString, QVector< QVector<int> > >
                                epochsOfCorrelationsByBand, epochsOfCentralsByBand, 
                                epochsOfFourfitingsByBand, epochsOfNominalScanTimeByBand, 
                                bbcIdxs_1ByBand, bbcIdxs_2ByBand, corelIdxNumbersUsbByBand,
                                corelIdxNumbersLsbByBand, errorRates_1ByBand, errorRates_2ByBand;
    QMap<QString, SgMatrix*>    corrClocksByBand, loFreqs_1ByBand, loFreqs_2ByBand, 
                                channelBandwidthByBand;
    SgVector                   *dTec, *dTecStdDev;
   
   
    // create the data structures:
    for (int iB=0; iB<bands_.size(); iB++)
    {
      //
      const QString&            bandKey=bands_.at(iB)->getKey();
      numOfChans = bands_.at(iB)->getMaxNumOfChannels();
      sbDelaysByBand[bandKey] = new SgMatrix(numOfObs, 2);
      grDelaysByBand[bandKey] = new SgMatrix(numOfObs, 2);
      phRatesByBand [bandKey] = new SgMatrix(numOfObs, 2);
      grDelaysAltByBand[bandKey] = new SgMatrix(numOfObs, 2);
      phRatesAltByBand [bandKey] = new SgMatrix(numOfObs, 2);

      phasesByBand  [bandKey] = new SgMatrix(numOfObs, 2);
      phaseCalRates_1ByBand[bandKey] 
                              = new SgVector(numOfObs);
      phaseCalRates_2ByBand[bandKey] 
                              = new SgVector(numOfObs);
      
      refFreqsByBand[bandKey] = new SgVector(numOfObs);
      phaseCal_1ByBand[bandKey]
                              = new SgMatrix(numOfObs, 4);
      phaseCal_2ByBand[bandKey]
                              = new SgMatrix(numOfObs, 4);
      ambigsByBand  [bandKey] = new SgVector(numOfObs);
      correlationByBand[bandKey] = new SgVector(numOfObs);
      snrByBand     [bandKey] = new SgVector(numOfObs);
    
      qualityCodesByBand  [bandKey].resize(numOfObs);
      numOfChannelsByBand [bandKey].resize(numOfObs);
      dataFlagsByBand     [bandKey].resize(numOfObs);
      dataFlagsByBand     [bandKey].fill(-1);
      fourfitOutputFNameByBand[bandKey].resize(numOfObs);
      sampleRateByBand    [bandKey] = new SgVector(numOfObs);
      numOfAccPeriodsByChan_USBByBand [bandKey] = new SgMatrix(numOfObs, numOfChans);
      numOfAccPeriodsByChan_LSBByBand [bandKey] = new SgMatrix(numOfObs, numOfChans);

      phCalAmps_1ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalAmps_2ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalPhss_1ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalPhss_2ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalFrqs_1ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalFrqs_2ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalOffs_1ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);
      phCalOffs_2ByChanByBand[bandKey] = new SgMatrix(numOfObs, numOfChans);

      refFreqByChanByBand             [bandKey] = new SgMatrix(numOfObs, numOfChans);
      numOfSamplesByChan_USBByBand    [bandKey] = new SgMatrix(numOfObs, numOfChans);
      numOfSamplesByChan_LSBByBand    [bandKey] = new SgMatrix(numOfObs, numOfChans);
      residualFringeAmplByChanByBand  [bandKey] = new SgMatrix(numOfObs, numOfChans);
      residualFringePhseByChanByBand  [bandKey] = new SgMatrix(numOfObs, numOfChans);
      
      maxNumOfChanByBand[bandKey] = numOfChans;
      
      fourfitErrorCodesByBand   [bandKey].resize(numOfObs);
      frqGrpCodesByBand         [bandKey].resize(numOfObs);
      fourfitControlFileByBand  [bandKey].resize(numOfObs);
      fourfitCommandByBand      [bandKey].resize(numOfObs);   
      corelVersionsByBand       [bandKey].resize(numOfObs);
      startOffsetsByBand        [bandKey].resize(numOfObs);
      bitsPerSamplesByBand      [bandKey].resize(numOfObs);
      numLagsByBand             [bandKey].resize(numOfObs);
      apLengthByBand            [bandKey].resize(numOfObs);
      fourfitVersionsByBand     [bandKey].resize(numOfObs);
      dbeditVersionByBand       [bandKey].resize(numOfObs);
      baselineCodesByBand       [bandKey].resize(numOfObs);
      tapeQualityCodesByBand    [bandKey].resize(numOfObs);
      stopOffsetsByBand         [bandKey].resize(numOfObs);
      hopsRevisionNumbersByBand [bandKey].resize(numOfObs);
//      refClkErrByBand         [bandKey] = new SgVector(numOfObs);
      deltaEpochsByBand         [bandKey] = new SgVector(numOfObs);
      urvrsByBand               [bandKey] = new SgMatrix(numOfObs, 2);
      instrDelaysByBand         [bandKey] = new SgMatrix(numOfObs, 2);
      starElevByBand            [bandKey] = new SgMatrix(numOfObs, 2);
      zenithDelaysByBand        [bandKey] = new SgMatrix(numOfObs, 2);
      searchParamsByBand        [bandKey] = new SgMatrix(numOfObs, 6);
      mDlysByBand               [bandKey] = new SgMatrix(numOfObs, 3);
      mAuxByBand                [bandKey] = new SgMatrix(numOfObs, 4);
      sbdResidsByBand           [bandKey] = new SgVector(numOfObs);
      rateResidsByBand          [bandKey] = new SgVector(numOfObs);
      effDurationsByBand        [bandKey] = new SgVector(numOfObs);
      startSecondsByBand        [bandKey] = new SgVector(numOfObs);
      stopSecondsByBand         [bandKey] = new SgVector(numOfObs);
      percentOfDiscardsByBand   [bandKey] = new SgVector(numOfObs);
      uniformityFactorsByBand   [bandKey] = new SgVector(numOfObs);
      geocenterPhaseResidsByBand[bandKey] = new SgVector(numOfObs);
      geocenterPhaseTotalsByBand[bandKey] = new SgVector(numOfObs);
      geocenterSbDelaysByBand   [bandKey] = new SgVector(numOfObs);
      geocenterGrDelaysByBand   [bandKey] = new SgVector(numOfObs);
      geocenterDelayRatesByBand [bandKey] = new SgVector(numOfObs);
      probOfFalsesByBand        [bandKey] = new SgVector(numOfObs);
      corrAprioriDelaysByBand   [bandKey] = new SgVector(numOfObs);
      corrAprioriRatesByBand    [bandKey] = new SgVector(numOfObs);
      corrAprioriAccsByBand     [bandKey] = new SgVector(numOfObs);
      incohAmpsByBand           [bandKey] = new SgVector(numOfObs);
      incohAmps2ByBand          [bandKey] = new SgVector(numOfObs);
      delayResidsByBand         [bandKey] = new SgVector(numOfObs);

      if (originType_==OT_KOMB)
        corrRootFileNamesByBand[bandKey].resize(numOfObs);

      tapeIds_1ByBand           [bandKey].resize(numOfObs);
      tapeIds_2ByBand           [bandKey].resize(numOfObs);
      channelIdsByBand          [bandKey].resize(numOfObs);
      polarizationsByBand       [bandKey].resize(numOfObs);
      epochsOfCorrelationsByBand[bandKey].resize(numOfObs);
      epochsOfCentralsByBand    [bandKey].resize(numOfObs);
      epochsOfFourfitingsByBand [bandKey].resize(numOfObs);
      epochsOfNominalScanTimeByBand[bandKey].resize(numOfObs);
      bbcIdxs_1ByBand           [bandKey].resize(numOfObs);
      bbcIdxs_2ByBand           [bandKey].resize(numOfObs);
      corelIdxNumbersUsbByBand  [bandKey].resize(numOfObs);
      corelIdxNumbersLsbByBand  [bandKey].resize(numOfObs);
      errorRates_1ByBand        [bandKey].resize(numOfObs);
      errorRates_2ByBand        [bandKey].resize(numOfObs);

      corrClocksByBand          [bandKey] = new SgMatrix(numOfObs, 4);
      loFreqs_1ByBand           [bandKey] = new SgMatrix(numOfObs, numOfChans);
      loFreqs_2ByBand           [bandKey] = new SgMatrix(numOfObs, numOfChans);
      channelBandwidthByBand    [bandKey] = new SgMatrix(numOfObs, numOfChans);

      matrices2del
        << sbDelaysByBand[bandKey] << grDelaysByBand[bandKey] << phRatesByBand [bandKey]
        << grDelaysAltByBand[bandKey] << phRatesAltByBand [bandKey]
        << phasesByBand[bandKey]
        << phaseCal_1ByBand[bandKey] << phaseCal_2ByBand[bandKey]
        << numOfAccPeriodsByChan_USBByBand[bandKey]
        << numOfAccPeriodsByChan_LSBByBand[bandKey] 
        << refFreqByChanByBand[bandKey] 
        << phCalAmps_1ByChanByBand[bandKey]
        << phCalAmps_2ByChanByBand[bandKey]
        << phCalPhss_1ByChanByBand[bandKey]
        << phCalPhss_2ByChanByBand[bandKey]
        << phCalFrqs_1ByChanByBand[bandKey]
        << phCalFrqs_2ByChanByBand[bandKey]
        << phCalOffs_1ByChanByBand[bandKey]
        << phCalOffs_2ByChanByBand[bandKey]
        << numOfSamplesByChan_USBByBand[bandKey] << numOfSamplesByChan_LSBByBand[bandKey]
        << residualFringeAmplByChanByBand[bandKey] << residualFringePhseByChanByBand[bandKey]
        << urvrsByBand[bandKey] << instrDelaysByBand[bandKey] 
        << starElevByBand[bandKey] << zenithDelaysByBand[bandKey] << searchParamsByBand[bandKey]
        << mDlysByBand[bandKey] << mAuxByBand[bandKey]
        << corrClocksByBand[bandKey] << loFreqs_1ByBand[bandKey]
        << loFreqs_2ByBand[bandKey] << channelBandwidthByBand[bandKey]
        ;

      vectors2del
        << refFreqsByBand[bandKey] << ambigsByBand[bandKey] << correlationByBand[bandKey]
        << snrByBand[bandKey] << sampleRateByBand[bandKey]
        << deltaEpochsByBand[bandKey] << sbdResidsByBand[bandKey] << rateResidsByBand[bandKey]
        << effDurationsByBand[bandKey] << startSecondsByBand[bandKey] << stopSecondsByBand[bandKey]
        << percentOfDiscardsByBand[bandKey] << uniformityFactorsByBand[bandKey] 
        << geocenterPhaseResidsByBand[bandKey] << geocenterPhaseTotalsByBand[bandKey]
        << geocenterSbDelaysByBand[bandKey] << geocenterGrDelaysByBand[bandKey]
        << geocenterDelayRatesByBand[bandKey] << probOfFalsesByBand[bandKey] 
        << corrAprioriDelaysByBand[bandKey] << corrAprioriRatesByBand[bandKey]
        << corrAprioriAccsByBand[bandKey] << incohAmpsByBand[bandKey] << incohAmps2ByBand[bandKey]
        << delayResidsByBand[bandKey] << phaseCalRates_1ByBand[bandKey] << phaseCalRates_2ByBand[bandKey]
//      << refClkErrByBand[bandKey]
        ;
    };
    dTec            = new SgVector(numOfObs);
    dTecStdDev      = new SgVector(numOfObs);
    vectors2del << dTec << dTecStdDev;

    bands = bandByKey_.keys();

    //
    QMap<QString, bool>         stnsPresent;
    QMap<QString, int>          stnsIdxs4scns;
    QMap<QString, int>          idxByScan;
    n = 1;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      stnIdxByName[it.value()->getKey()] = n++;
      stnsIdxs4scns[it.value()->getKey()] = 0;
    };
    n = 1;
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
      srcIdxByName[it.value()->getKey()] = n++;

    // fill the lists:
    for (int obsIdx=0; obsIdx<numOfObs; obsIdx++)
    {
      int                       nYear, nMonth, nDay, nHour, nMin;
      double                    dSec;
      SgVlbiObservation        *obs=observations_.at(obsIdx);
      epochs4Obs << *obs;
      stations_1 << obs->stn_1()->getKey();
      stations_2 << obs->stn_2()->getKey();
      sources << obs->src()->getKey();
      //
//4KOMB:
//std::cout << " -- - - " << qPrintable(QString("").sprintf("%3d(%3d)", obsIdx, epochByScanId.size()))
//          << qPrintable(obs->getScanId()) << "  :  "
//          << qPrintable(obs->getKey()) << "  :  " << qPrintable(obs->getScanName())
//          << qPrintable(obs->toString())
//          << " \n";
      if (!epochByScanId.contains(obs->getScanId())) // new scan:
      {
        epochByScanId.insert(obs->getScanId(), *obs);
        scanNames << obs->getScanName();
        crootFnames << obs->getCorrRootFileName();
        idxByScan[obs->getScanId()] = idxByScan.size();
        str.sprintf("%s %s", 
          qPrintable(obs->toString(SgMJD::F_YYYYMMDDHHMMSSSS)), qPrintable(obs->src()->getKey()));
        if (str.at(10) == ' ')
          str[10] = '-';
        scanFullNames << str;
        scan2src << srcIdxByName[obs->src()->getKey()];
        //
        if (stnsPresent.size())
          for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
          {
            if (stnsPresent.contains(it.key()))
              scan2Station[it.key()] << ++stnsIdxs4scns[it.key()];
            else
              scan2Station[it.key()] << 0;
          };
        stnsPresent.clear();
      };
      // crossrefs:
      obs2scan << epochByScanId.size();
      obs2stn_1 << stnIdxByName[obs->stn_1()->getKey()];
      obs2stn_2 << stnIdxByName[obs->stn_2()->getKey()];
      stnsPresent[obs->stn_1()->getKey()] = true;
      stnsPresent[obs->stn_2()->getKey()] = true;
      //
      //
      for (int bandIdx=0; bandIdx<bands.size(); bandIdx++)
      {
        const QString&          bandKey=bands.at(bandIdx);
        SgVlbiObservable       *o=obs->observable(bandKey);
        numOfChans = maxNumOfChanByBand[bandKey];
        epochsOfCorrelationsByBand[bandKey][obsIdx].resize(6);
        epochsOfCentralsByBand[bandKey][obsIdx].resize(6);
        epochsOfFourfitingsByBand[bandKey][obsIdx].resize(6);
        epochsOfNominalScanTimeByBand[bandKey][obsIdx].resize(6);
        bbcIdxs_1ByBand[bandKey][obsIdx].resize(numOfChans);
        bbcIdxs_2ByBand[bandKey][obsIdx].resize(numOfChans);
        corelIdxNumbersUsbByBand[bandKey][obsIdx].resize(numOfChans);
        corelIdxNumbersLsbByBand[bandKey][obsIdx].resize(numOfChans);
        errorRates_1ByBand[bandKey][obsIdx].resize(numOfChans);
        errorRates_2ByBand[bandKey][obsIdx].resize(numOfChans);
        fourfitVersionsByBand   [bandKey][obsIdx].resize(2);
        dbeditVersionByBand     [bandKey][obsIdx].resize(3);
        
        if (o)
        {
          //
          sbDelaysByBand[bandKey] ->setElement(obsIdx,0,  o->sbDelay().getValue());
          sbDelaysByBand[bandKey] ->setElement(obsIdx,1,  o->sbDelay().getSigma());
          //
          grDelaysByBand[bandKey] ->setElement(obsIdx,0,  o->grDelay().getValue());
          grDelaysByBand[bandKey] ->setElement(obsIdx,1,  o->grDelay().getSigma());
          phRatesByBand [bandKey] ->setElement(obsIdx,0,  o->phDRate().getValue());
          phRatesByBand [bandKey] ->setElement(obsIdx,1,  o->phDRate().getSigma());
          //
          if (originType_==OT_KOMB && isAttr(Attr_REF_CLOCKS_ADJUSTED))
          {
            grDelaysAltByBand[bandKey] ->setElement(obsIdx,0,  o->grDelay().getAlternateValue());
            grDelaysAltByBand[bandKey] ->setElement(obsIdx,1,  o->grDelay().getSigma());
            phRatesAltByBand [bandKey] ->setElement(obsIdx,0,  o->phDRate().getAlternateValue());
            phRatesAltByBand [bandKey] ->setElement(obsIdx,1,  o->phDRate().getSigma());
          };
          //
          phasesByBand  [bandKey] ->setElement(obsIdx,0,  o->getTotalPhase());
          phasesByBand  [bandKey] ->setElement(obsIdx,1,  o->getSnr()==0.0?1.0:1.0/o->getSnr());
          refFreqsByBand[bandKey] ->setElement(obsIdx,    o->getReferenceFrequency());
        
         
          phaseCal_1ByBand[bandKey]->setElement(obsIdx,0,  o->getPhaseCalGrDelays(0));
          phaseCal_2ByBand[bandKey]->setElement(obsIdx,0,  o->getPhaseCalGrDelays(1));
          phaseCal_1ByBand[bandKey]->setElement(obsIdx,1,  o->getPhaseCalRates(0));
          phaseCal_2ByBand[bandKey]->setElement(obsIdx,1,  o->getPhaseCalRates(1));
          ambigsByBand    [bandKey]->setElement(obsIdx,    o->grDelay().getAmbiguitySpacing());

          sprintf(buff, "%1d", o->getQualityFactor());
          qualityCodesByBand  [bandKey][obsIdx] = buff[0];
          dataFlagsByBand     [bandKey][obsIdx] = 0;
          correlationByBand   [bandKey]->setElement(obsIdx, o->getCorrCoeff());
          snrByBand           [bandKey]->setElement(obsIdx, o->getSnr());
          numOfChannelsByBand [bandKey][obsIdx] = o->getNumOfChannels();
          sampleRateByBand    [bandKey]->setElement(obsIdx, o->getSampleRate());
        
          fourfitOutputFNameByBand[bandKey][obsIdx] = o->getFourfitOutputFName();

          // epochs:
          //
          o->getEpochOfCorrelation().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          epochsOfCorrelationsByBand[bandKey][obsIdx][0] = nYear;
          epochsOfCorrelationsByBand[bandKey][obsIdx][1] = nMonth;
          epochsOfCorrelationsByBand[bandKey][obsIdx][2] = nDay;
          epochsOfCorrelationsByBand[bandKey][obsIdx][3] = nHour;
          epochsOfCorrelationsByBand[bandKey][obsIdx][4] = nMin; 
          epochsOfCorrelationsByBand[bandKey][obsIdx][5] = (int)round(dSec);
          //
          o->getEpochCentral().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          epochsOfCentralsByBand[bandKey][obsIdx][0] = nYear;
          epochsOfCentralsByBand[bandKey][obsIdx][1] = nMonth;
          epochsOfCentralsByBand[bandKey][obsIdx][2] = nDay;
          epochsOfCentralsByBand[bandKey][obsIdx][3] = nHour;
          epochsOfCentralsByBand[bandKey][obsIdx][4] = nMin;
          epochsOfCentralsByBand[bandKey][obsIdx][5] = (int)round(dSec);
          //
          o->getEpochOfFourfitting().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          epochsOfFourfitingsByBand[bandKey][obsIdx][0] = nYear;
          epochsOfFourfitingsByBand[bandKey][obsIdx][1] = nMonth;
          epochsOfFourfitingsByBand[bandKey][obsIdx][2] = nDay;
          epochsOfFourfitingsByBand[bandKey][obsIdx][3] = nHour;
          epochsOfFourfitingsByBand[bandKey][obsIdx][4] = nMin;
          epochsOfFourfitingsByBand[bandKey][obsIdx][5] = (int)round(dSec);
          //
          o->getEpochOfScan().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][0] = nYear;
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][1] = nMonth;
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][2] = nDay;
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][3] = nHour;
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][4] = nMin;
          epochsOfNominalScanTimeByBand[bandKey][obsIdx][5] = (int)round(dSec);
          //
          //
          if (originType_==OT_KOMB)
            corrRootFileNamesByBand[bandKey][obsIdx] = obs->getCorrRootFileName();
          //
          tapeIds_1ByBand[bandKey][obsIdx] = obs->auxObs_1()?obs->auxObs_1()->getTapeId():"";
          tapeIds_2ByBand[bandKey][obsIdx] = obs->auxObs_2()?obs->auxObs_2()->getTapeId():"";
          channelIdsByBand[bandKey][obsIdx] = "";
          polarizationsByBand[bandKey][obsIdx] = "";

          for (int k=0; k<o->getNumOfChannels(); k++)
          {
            refFreqByChanByBand[bandKey]->setElement(obsIdx,k, 
              o->refFreqByChan()->getElement(k));
            numOfAccPeriodsByChan_USBByBand[bandKey]->setElement(obsIdx,k,  
              o->numOfAccPeriodsByChan_USB()->getElement(k));
            numOfAccPeriodsByChan_LSBByBand[bandKey]->setElement(obsIdx,k,  
              o->numOfAccPeriodsByChan_LSB()->getElement(k));

            phCalAmps_1ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_1ByChan()->getElement(0, k));
            phCalAmps_2ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_2ByChan()->getElement(0, k));
            phCalPhss_1ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_1ByChan()->getElement(1, k)*RAD2DEG*100.0);
            phCalPhss_2ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_2ByChan()->getElement(1, k)*RAD2DEG*100.0);
            phCalFrqs_1ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_1ByChan()->getElement(2, k));
            phCalFrqs_2ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_2ByChan()->getElement(2, k));
            phCalOffs_1ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_1ByChan()->getElement(3, k)*RAD2DEG*100.0);
            phCalOffs_2ByChanByBand[bandKey]->setElement(obsIdx,k,
              o->phaseCalData_2ByChan()->getElement(3, k)*RAD2DEG*100.0);
            errorRates_1ByBand[bandKey][obsIdx][k] = 
              o->phaseCalData_1ByChan()->getElement(4, k);
            errorRates_2ByBand[bandKey][obsIdx][k] = 
              o->phaseCalData_2ByChan()->getElement(4, k);

            numOfSamplesByChan_USBByBand[bandKey]->setElement(obsIdx,k, 
              o->numOfSamplesByChan_USB()->getElement(k));
            numOfSamplesByChan_LSBByBand[bandKey]->setElement(obsIdx,k, 
              o->numOfSamplesByChan_LSB()->getElement(k));
            residualFringeAmplByChanByBand[bandKey]->setElement(obsIdx,k, 
              o->fringeAmplitudeByChan()->getElement(k));
            residualFringePhseByChanByBand[bandKey]->setElement(obsIdx,k, 
              o->fringePhaseByChan()->getElement(k)*RAD2DEG);

            bbcIdxs_1ByBand[bandKey][obsIdx][k] = o->bbcIdxByChan_1()->at(k);
            bbcIdxs_2ByBand[bandKey][obsIdx][k] = o->bbcIdxByChan_2()->at(k);
            corelIdxNumbersUsbByBand[bandKey][obsIdx][k] = o->corelIndexNumUSB()->at(k);
            corelIdxNumbersLsbByBand[bandKey][obsIdx][k] = o->corelIndexNumLSB()->at(k);
            loFreqs_1ByBand[bandKey]->setElement(obsIdx,k, o->loFreqByChan_1()->getElement(k));
            loFreqs_2ByBand[bandKey]->setElement(obsIdx,k, o->loFreqByChan_2()->getElement(k));
            channelBandwidthByBand[bandKey]->setElement(obsIdx,k, o->channelBandwidth()->getElement(k));
            channelIdsByBand    [bandKey][obsIdx] += str.sprintf("%-2c", o->chanIdByChan()->at(k));
            polarizationsByBand [bandKey][obsIdx] += str.sprintf("%-2c%-2c", 
              o->polarization_1ByChan()->at(k), o->polarization_2ByChan()->at(k));
          };
          //
          fourfitErrorCodesByBand   [bandKey][obsIdx].sprintf("%-2s", qPrintable(o->getErrorCode()));
          frqGrpCodesByBand         [bandKey][obsIdx].sprintf("%-2s", qPrintable(bandKey));
          fourfitControlFileByBand  [bandKey][obsIdx].sprintf("%s", 
                                                            qPrintable(o->getFourfitControlFile()));
          fourfitCommandByBand      [bandKey][obsIdx].sprintf("%s", 
                                                            qPrintable(o->getFourfitCommandOverride()));
          baselineCodesByBand       [bandKey][obsIdx].sprintf("%c%c", 
                                                        obs->stn_1()->getCid(), obs->stn_2()->getCid());
          tapeQualityCodesByBand    [bandKey][obsIdx].sprintf("%-6s", qPrintable(o->getTapeQualityCode()));
          corelVersionsByBand       [bandKey][obsIdx] = -1; // mimic dbedit
          startOffsetsByBand        [bandKey][obsIdx] = o->getStartOffset();
          bitsPerSamplesByBand      [bandKey][obsIdx] = o->getBitsPerSample();
          numLagsByBand             [bandKey][obsIdx] = o->getNlags();
          apLengthByBand            [bandKey][obsIdx] = obs->getApLength();
          stopOffsetsByBand         [bandKey][obsIdx] = o->getStopOffset();
          hopsRevisionNumbersByBand [bandKey][obsIdx] = o->getHopsRevisionNumber();
          fourfitVersionsByBand     [bandKey][obsIdx][0] = o->getFourfitVersion(0);
          fourfitVersionsByBand     [bandKey][obsIdx][1] = o->getFourfitVersion(1);
          dbeditVersionByBand       [bandKey][obsIdx][0] = 2007;
          dbeditVersionByBand       [bandKey][obsIdx][1] = 10;
          dbeditVersionByBand       [bandKey][obsIdx][2] = 27;
//        refClkErrByBand           [bandKey]->setElement(obsIdx, 0.0);                   // what is it?
          deltaEpochsByBand         [bandKey]->setElement(obsIdx, o->getCentrOffset());
          //
          urvrsByBand               [bandKey]->setElement(obsIdx,0,  o->getUrVr(0));
          urvrsByBand               [bandKey]->setElement(obsIdx,1,  o->getUrVr(1));
          instrDelaysByBand         [bandKey]->setElement(obsIdx,0,  o->getInstrDelay(0));
          instrDelaysByBand         [bandKey]->setElement(obsIdx,1,  o->getInstrDelay(1));
          starElevByBand            [bandKey]->setElement(obsIdx,0,  o->getCorrelStarElev_1()*RAD2DEG);
          starElevByBand            [bandKey]->setElement(obsIdx,1,  o->getCorrelStarElev_2()*RAD2DEG);
          zenithDelaysByBand        [bandKey]->setElement(obsIdx,0,  o->getCorrelZdelay_1());
          zenithDelaysByBand        [bandKey]->setElement(obsIdx,1,  o->getCorrelZdelay_2());
          //
          for (int j=0; j<6; j++)
            searchParamsByBand      [bandKey]->setElement(obsIdx,j,  o->getFourfitSearchParameters(j));

          sbdResidsByBand           [bandKey]->setElement(obsIdx, o->sbDelay().getResidualFringeFitting());
          rateResidsByBand          [bandKey]->setElement(obsIdx, o->phDRate().getResidualFringeFitting());
          effDurationsByBand        [bandKey]->setElement(obsIdx, o->getEffIntegrationTime());

          o->getTstart().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          startSecondsByBand        [bandKey]->setElement(obsIdx, 60.0*nMin + dSec);
          o->getTstop().toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
          stopSecondsByBand         [bandKey]->setElement(obsIdx, 60.0*nMin + dSec);
          percentOfDiscardsByBand   [bandKey]->setElement(obsIdx, o->getDiscardRatio());
          uniformityFactorsByBand   [bandKey]->setElement(obsIdx, o->getAcceptedRatio());
          geocenterPhaseResidsByBand[bandKey]->setElement(obsIdx, o->getGeocenterResidPhase()*RAD2DEG);
          geocenterPhaseTotalsByBand[bandKey]->setElement(obsIdx, o->getGeocenterTotalPhase()*RAD2DEG);
          geocenterSbDelaysByBand   [bandKey]->setElement(obsIdx, o->sbDelay().getGeocenterValue());
          geocenterGrDelaysByBand   [bandKey]->setElement(obsIdx, o->grDelay().getGeocenterValue());
          geocenterDelayRatesByBand [bandKey]->setElement(obsIdx, o->phDRate().getGeocenterValue());
          probOfFalsesByBand        [bandKey]->setElement(obsIdx, o->getProbabOfFalseDetection());
          corrAprioriDelaysByBand   [bandKey]->setElement(obsIdx, o->getAprioriDra(0));
          corrAprioriRatesByBand    [bandKey]->setElement(obsIdx, o->getAprioriDra(1));
          corrAprioriAccsByBand     [bandKey]->setElement(obsIdx, o->getAprioriDra(2));
          incohAmpsByBand           [bandKey]->setElement(obsIdx, o->getIncohChanAddAmp());
          incohAmps2ByBand          [bandKey]->setElement(obsIdx, o->getIncohSegmAddAmp());
          delayResidsByBand         [bandKey]->setElement(obsIdx, o->grDelay().getResidualFringeFitting());
          phaseCalRates_1ByBand     [bandKey]->setElement(obsIdx, o->getPhaseCalRates(0));
          phaseCalRates_2ByBand     [bandKey]->setElement(obsIdx, o->getPhaseCalRates(1));
          corrClocksByBand          [bandKey]->setElement(obsIdx,0, o->getCorrClocks(0, 0));
          corrClocksByBand          [bandKey]->setElement(obsIdx,1, o->getCorrClocks(0, 1));
          corrClocksByBand          [bandKey]->setElement(obsIdx,2, o->getCorrClocks(1, 0));
          corrClocksByBand          [bandKey]->setElement(obsIdx,3, o->getCorrClocks(1, 1));

          mDlysByBand[bandKey]->setElement(obsIdx,0,  o->vDlys()->getElement(0));
          mDlysByBand[bandKey]->setElement(obsIdx,1,  o->vDlys()->getElement(1));
          mDlysByBand[bandKey]->setElement(obsIdx,2,  o->vDlys()->getElement(2));
          mAuxByBand[bandKey]->setElement(obsIdx,0,  o->vAuxData()->getElement(0));
          mAuxByBand[bandKey]->setElement(obsIdx,1,  o->vAuxData()->getElement(1)*RAD2DEG);
          mAuxByBand[bandKey]->setElement(obsIdx,2,  o->vAuxData()->getElement(2));
          mAuxByBand[bandKey]->setElement(obsIdx,3,  o->vAuxData()->getElement(3));
        };
      };
      dTec          ->setElement(obsIdx, obs->getDTec());
      dTecStdDev    ->setElement(obsIdx, obs->getDTecStdDev());
    };
    //
    //
    //
    if (originType_==OT_AGV && primaryBand_->history().size())
      vgosDb->setCurrentVersion(primaryBand_->history().last()->getVersion());
    //
    //
    if (stnsPresent.size())
      for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      {
        if (stnsPresent.contains(it.key()))
          scan2Station[it.key()] << ++stnsIdxs4scns[it.key()];
        else
          scan2Station[it.key()] << 0;
      };
    stnsPresent.clear();
    stnsIdxs4scns.clear();
    //
    epochs4Scans = epochByScanId.values();
    
    // first attempt to write:
    if (isAttr(Attr_FF_CREATED) && !(isOk=vgosDb->storeEpochs4Scans(epochs4Scans)) )
      return isOk;

    if (isAttr(Attr_FF_CREATED))
    {    
      isOk = isOk && vgosDb->storeScanTimeMjd(epochs4Scans);
      isOk = isOk && vgosDb->storeEpochs4Obs(epochs4Obs);
      isOk = isOk && vgosDb->storeObsObjectNames(stations_1, stations_2, sources);
    };
    
    // create head:
    stations_1.clear();
    sources.clear();
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      stations_1 << stn->getKey();
      numScansPerStation << stn->auxObservationByScanId()->size();
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt)
        station2Scan[stn->getKey()] << idxByScan[jt.key()];
    };
    // fill zeros:
    int                         maxSize=0;
    for (QMap<QString, QList<int> >::iterator it=station2Scan.begin(); it!=station2Scan.end(); ++it)
      if (maxSize<it.value().size())
        maxSize=it.value().size();
    for (QMap<QString, QList<int> >::iterator it=station2Scan.begin(); it!=station2Scan.end(); ++it)
      if (it.value().size()<maxSize)
        for (int i=it.value().size(); i<maxSize; i++)
          it.value() << 0;
    //
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
      sources << it.value()->getKey();

    if (isAttr(Attr_FF_CREATED))
    {
      isOk = isOk && vgosDb->storeSessionHead(correlatorType_, correlatorName_, piAgencyName_,
        sessionCode_,  experimentSerialNumber_, description_, recordingMode_, 
        stations_1, sources, tStart_, tFinis_, (int)cppsSoft_);
      //
      // scan:
      isOk = isOk && vgosDb->storeScanName(scanNames, scanFullNames);

      if (originType_==OT_MK4 || originType_==OT_AGV)
        isOk = isOk && vgosDb->storeScanCrootFname(crootFnames);
      //
      // crossRefs:
      isOk = isOk && vgosDb->storeObsCrossRefs(obs2scan, obs2stn_1, obs2stn_2);
      isOk = isOk && vgosDb->storeSourceCrossRefs(sources, scan2src);
      isOk = isOk && vgosDb->storeStationCrossRefs(numScansPerStation, stations_1, 
        station2Scan, scan2Station);
    };
    //
    // output of band-dependent values:
    for (int iB=0; iB<bands_.size(); iB++)
    {
      //
      const QString&            bandKey=bands_.at(iB)->getKey();
      //
      vgosDb->setNumOfChannels(maxNumOfChanByBand[bandKey]);
      //
      // obs:
      if (isAttr(Attr_FF_CREATED))
      {
        isOk = isOk && vgosDb->storeObsSingleBandDelays(bandKey, sbDelaysByBand[bandKey]);
        //
        // GSI input: original and corrected gr.delays and rates:
        if (originType_==OT_KOMB && isAttr(Attr_REF_CLOCKS_ADJUSTED))
        {
          isOk = isOk && vgosDb->storeObsGroupDelays(bandKey, grDelaysAltByBand[bandKey], "Original");
          isOk = isOk && vgosDb->storeObsRates(bandKey, phRatesAltByBand[bandKey], "Original");
        };
        //
        isOk = isOk && vgosDb->storeObsGroupDelays(bandKey, grDelaysByBand[bandKey], "");
        isOk = isOk && vgosDb->storeObsRates(bandKey, phRatesByBand[bandKey], "");

        isOk = isOk && vgosDb->storeObsPhase(bandKey, phasesByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsRefFreqs(bandKey, refFreqsByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsCalUnphase(bandKey, phaseCal_1ByBand[bandKey], 
          phaseCal_2ByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsAmbigSpacing(bandKey, ambigsByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsQualityCodes(bandKey, qualityCodesByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsDataFlag(bandKey, dataFlagsByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsCorrelation(bandKey, correlationByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsSNR(bandKey, snrByBand[bandKey]);
      };
      //
      //
      if (isAttr(Attr_FF_CREATED) || isAttr(Attr_FF_CHANNEL_INFO_MODIFIED))
        isOk = isOk && vgosDb->storeObsChannelInfo(bandKey, originType_,
          channelIdsByBand[bandKey],
          polarizationsByBand[bandKey],
          numOfChannelsByBand[bandKey], 
          bitsPerSamplesByBand[bandKey],
          errorRates_1ByBand[bandKey],
          errorRates_2ByBand[bandKey],
          bbcIdxs_1ByBand[bandKey],
          bbcIdxs_2ByBand[bandKey],
          corelIdxNumbersUsbByBand[bandKey],
          corelIdxNumbersLsbByBand[bandKey],
          sampleRateByBand[bandKey], 
          residualFringeAmplByChanByBand[bandKey],
          residualFringePhseByChanByBand[bandKey],
          refFreqByChanByBand[bandKey],
          numOfSamplesByChan_USBByBand[bandKey], 
          numOfSamplesByChan_LSBByBand[bandKey], 
          numOfAccPeriodsByChan_USBByBand[bandKey], 
          numOfAccPeriodsByChan_LSBByBand[bandKey],
          loFreqs_1ByBand[bandKey],
          loFreqs_2ByBand[bandKey],
          channelBandwidthByBand[bandKey],
          getSidebandOrder());
      //
      if (isAttr(Attr_FF_CREATED))
        isOk = isOk && vgosDb->storeObsPhaseCalInfo(bandKey, originType_,
          phCalFrqs_1ByChanByBand[bandKey],
          phCalAmps_1ByChanByBand[bandKey],
          phCalPhss_1ByChanByBand[bandKey],
          phCalOffs_1ByChanByBand[bandKey],
          phaseCalRates_1ByBand[bandKey],
          phCalFrqs_2ByChanByBand[bandKey],
          phCalAmps_2ByChanByBand[bandKey],
          phCalPhss_2ByChanByBand[bandKey],
          phCalOffs_2ByChanByBand[bandKey],
          phaseCalRates_2ByBand[bandKey]);
      //
      if (isAttr(Attr_FF_CREATED))
        isOk = isOk && vgosDb->storeObsCorrelatorInfo(bandKey, originType_,
          fourfitOutputFNameByBand[bandKey],
          fourfitErrorCodesByBand[bandKey],
          frqGrpCodesByBand[bandKey],
          corelVersionsByBand[bandKey],
          startOffsetsByBand[bandKey],
          fourfitVersionsByBand[bandKey],
          dbeditVersionByBand[bandKey],
          deltaEpochsByBand[bandKey],
          urvrsByBand[bandKey], 
          instrDelaysByBand[bandKey], 
          starElevByBand[bandKey], 
          zenithDelaysByBand[bandKey], 
          searchParamsByBand[bandKey],
          baselineCodesByBand[bandKey], 
          tapeQualityCodesByBand[bandKey], 
          stopOffsetsByBand[bandKey],
          hopsRevisionNumbersByBand[bandKey],
          sampleRateByBand[bandKey], 
          sbdResidsByBand[bandKey],
          rateResidsByBand[bandKey],
          effDurationsByBand[bandKey],
          startSecondsByBand[bandKey], 
          stopSecondsByBand[bandKey], 
          percentOfDiscardsByBand[bandKey],
          uniformityFactorsByBand[bandKey], 
          geocenterPhaseResidsByBand[bandKey],
          geocenterPhaseTotalsByBand[bandKey], 
          geocenterSbDelaysByBand[bandKey],
          geocenterGrDelaysByBand[bandKey], 
          geocenterDelayRatesByBand[bandKey],
          probOfFalsesByBand[bandKey], 
          corrAprioriDelaysByBand[bandKey], 
          corrAprioriRatesByBand[bandKey],
          corrAprioriAccsByBand[bandKey], 
          incohAmpsByBand[bandKey], 
          incohAmps2ByBand[bandKey],
          delayResidsByBand[bandKey],
          corrRootFileNamesByBand[bandKey],
          tapeIds_1ByBand[bandKey],
          tapeIds_2ByBand[bandKey],
          epochsOfCorrelationsByBand[bandKey],
          epochsOfCentralsByBand[bandKey],
          epochsOfFourfitingsByBand[bandKey],
          epochsOfNominalScanTimeByBand[bandKey],
          corrClocksByBand[bandKey],
          mDlysByBand[bandKey],
          mAuxByBand[bandKey]);
      //
/*
//  store data for each band:
      if (originType_ == OT_MK4) // right now nothing to output for KOMB
        isOk = isOk && vgosDb->storeMiscFourFit(bandKey, originType_,
          fourfitControlFileByBand[bandKey], fourfitCommandByBand[bandKey],
          numLagsByBand[bandKey], apLengthByBand[bandKey]);
*/
//  store data only from the primary:
/* */
      if (isAttr(Attr_FF_CREATED)                       && 
         (originType_==OT_MK4 || originType_==OT_AGV)   && // right now nothing to output for KOMB
          primaryBand_                                  && 
          primaryBand_->getKey() == bandKey)
        isOk = isOk && vgosDb->storeMiscFourFit("", originType_,
          fourfitControlFileByBand[bandKey], fourfitCommandByBand[bandKey],
          numLagsByBand[bandKey], apLengthByBand[bandKey]);
    }; // end of band iteration
    //
    // VGOS:
    if (isAttr(Attr_FF_CREATED) && isAttr(Attr_HAS_DTEC))
      isOk = isOk && vgosDb->storeObsDtec(dTec, dTecStdDev);
    //
    //
    if (isAttr(Attr_FF_CREATED) && (originType_==OT_MK4 || originType_==OT_KOMB) )
      isOk = isOk && vgosDb->saveForeignHistory(primaryBand_->history(),
        "Correlator", primaryBand_->history().size()?primaryBand_->history().last()->getEpoch():tZero,
        "correlator operator", "History", name_, "1");
    else if (isAttr(Attr_FF_CREATED) && originType_==OT_AGV)
      isOk = isOk && vgosDb->saveForeignHistory(primaryBand_->history(),
        libraryVersion.getSoftwareName() + "/" + vgosDb->getCurrentDriverVersion()->getSoftwareName(),
        primaryBand_->history().size()?primaryBand_->history().last()->getEpoch():tZero,
        vgosDb->getCurrentIdentities()->getUserName() + ", " +
        vgosDb->getCurrentIdentities()->getAcFullName(), "History", name_, 
        libraryVersion.toString() + "/" + vgosDb->getCurrentDriverVersion()->toString());

    //    contemporaryHistory_.addHistoryRecord("Data were extracted from the correlator output.");

    //
    // clear resources:
    qualityCodesByBand.clear();
    dataFlagsByBand.clear();
    
    srcIdxByName.clear();
    stnIdxByName.clear();
    scan2src.clear();
    obs2scan.clear();
    obs2stn_1.clear();
    obs2stn_2.clear();
    //
    // obs per band:
    // clear temporary storages:
    for (int i=0; i<vectors2del.size(); i++)
      if (vectors2del.at(i))
      {
        delete vectors2del[i];
        vectors2del[i] = NULL; 
      };
    for (int i=0; i<matrices2del.size(); i++)
      if (matrices2del.at(i))
      {
        delete matrices2del[i];
        matrices2del[i] = NULL;
      };
    //
    sbDelaysByBand.clear();
    grDelaysByBand.clear();
    phRatesByBand.clear();
    phasesByBand.clear();
    ambigsByBand.clear();
    refFreqsByBand.clear();
//  effFreqsByBand.clear();
//  effFreqsEqWgtByBand.clear();
    phaseCal_1ByBand.clear();
    phaseCal_2ByBand.clear();
    correlationByBand.clear();
    snrByBand.clear();
    //
    //
    bands.clear();
    scanNames.clear();
    scanFullNames.clear();
    stations_1.clear();
    stations_2.clear();
    sources.clear();
    epochs4Scans.clear();
    epochs4Obs.clear();
    epochByScanId.clear();
    numScansPerStation.clear();
    //
    for (QMap<QString, QList<int> >::iterator it=station2Scan.begin(); it!=station2Scan.end(); ++it)
      it.value().clear();
    for (QMap<QString, QList<int> >::iterator it=scan2Station.begin(); it!=scan2Station.end(); ++it)
      it.value().clear();
    station2Scan.clear();
    scan2Station.clear();

    //
    //
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      const QString             stnName(stn->getKey());
      QList<SgMJD>              stnEpochs;
      SgVector                 *refClockOffset;
      int                       idx;
      
      refClockOffset = new SgVector(stn->auxObservationByScanId()->size());
      idx = 0;
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt)
      {
        SgVlbiAuxObservation   *auxObs=jt.value();
        stnEpochs << *auxObs;
        refClockOffset->setElement(idx, auxObs->getRefClockOffset());
        idx++;
      };
      if (isAttr(Attr_FF_CREATED))
        isOk = isOk && vgosDb->storeStationEpochs(stnName, stnEpochs);
      // KOMB:
      if (isAttr(Attr_FF_CREATED) && originType_ == OT_KOMB)
        isOk = isOk && vgosDb->storeStationRefClockOffset(stnName, refClockOffset);
      //
      stnEpochs.clear();
      delete refClockOffset;
    };
  };
  //-- end of if (isAttr(Attr_FF_CREATED))  // it was a new session, no vgosDb tree existed yet
  //
  //
  //
  //
  //
  //
  //
  //
  // special case to ressurect some databases:
  bool                          isSpecialCase;
  isSpecialCase = false;
  //isSpecialCase = true;
  if (isSpecialCase)
  {
    QList<QString>              sources;
    QMap<QString, SgMJD>        ebsi;
    QMap<QString, int>          srcIdxByName;
    QList<int>                  scan2src;
    int                         n=1;
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
    {
      sources << it.value()->getKey();
      srcIdxByName[it.value()->getKey()] = n++;
    };
    for (int obsIdx=0; obsIdx<numOfObs; obsIdx++)
    {
      SgVlbiObservation        *obs=observations_.at(obsIdx);
      if (!ebsi.contains(obs->getScanId())) // new scan:
      {
        ebsi.insert(obs->getScanId(), *obs);
        scan2src << srcIdxByName[obs->src()->getKey()];
      };
    };
    isOk = isOk && vgosDb->storeSourceCrossRefs(sources, scan2src);
    srcIdxByName.clear();
    scan2src.clear();
    ebsi.clear();
  };
  // EoSC
  //
  
  
  //
  //
  //
  if (config_)
  {
    if (originType_ == OT_AGV)
    {
      const SgIdentities       *ids=vgosDb->getCurrentIdentities();
      contemporaryHistory_.addHistoryRecord("Data were converted from ASCII vgosDa format to "
        "vgosDb tree by " + ids->getUserName() + " at " + ids->getAcFullName() + ". Contact info: <" + 
        ids->getUserEmailAddress() + ">", SgMJD::currentMJD().toUtc());
    }
    else
      makeHistoryIntro(contemporaryHistory_);
  };
  //
  if (vgosDb->getHave2redoLeapSecond())
    isOk = isOk && vgosDb->storeLeapSecond(getLeapSeconds());


  //
  //
  // theoreticals and partials:
  if (isAttr(Attr_FF_CALC_DATA_MODIFIED))
  {
//  QString                     kind4Ephemeris("DE405JPL");
    QString                     kind4Ephemeris("DE421JPL");
    QString                     kind4Nutation ("IAU2006");
    QString                     kind4HiFreqLibration("IERS2010");
    QString                     kind4HiFreqErp ("IERS2003");
    QString                     kind4EarthTide ("IERS2003");
    QString                     kind4PoleTide  ("IERS2003");

    QString                     kind4PoleTidePart("IERS2006");
    
    QMap<QString, SgMJD>        epochByScanId;
    int                         scanIdx, numOfScans;
    // 
    // calculate number of scans:
    for (int idx=0; idx<numOfObs; idx++)
    {
      SgVlbiObservation        *obs=observations_.at(idx);
      if (!epochByScanId.contains(obs->getScanId())) // new scan
        epochByScanId.insert(obs->getScanId(), *obs);
    };
    numOfScans = epochByScanId.size();
    epochByScanId.clear();
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF | SgLogger::DATA, className() +
      "::putDataIntoVgosDb(): counted " + QString("").setNum(numOfScans) + 
      " scans for the session " + name_);

    //
    // obs (theoreticals and partials):
    //
    SgVector                   *vCalcDelay, *vCalcRate, *vFractC;

    SgMatrix                   *cEarthTide, *cOceanTide, *cOceanTideOld;
    SgMatrix                   *cOceanPoleTide, *cPoleTide, *cPoleTideOld, *cTiltRmvr;
    SgMatrix                   *mHiFyUt1, *mHiFyPxy, *mLibrUt1, *mLibrPxy, *mPx, *mPy;
    SgMatrix                   *cBend, *cBendSun, *cBendSunHigher;
    SgMatrix                   *mPlxRev;
    SgMatrix                   *t2c_val, *t2c_rat, *t2c_acc;
    
    SgMatrix                   *partBend;
    SgMatrix                   *dV_dPx, *dV_dPy, *dV_dUT1;
    SgMatrix                   *dV_dCipX, *dV_dCipY;
    SgMatrix                   *dV_dRA, *dV_dDN;
    SgMatrix                   *dDel_dR_1, *dRat_dR_1;
    SgMatrix                   *dV_dGamma, *dV_dParallax;
    SgMatrix                   *dV_dPtdX, *dV_dPtdY;
    //
    // for scan dependent stuff:
    SgMatrix                   *rSun, *rMoon, *rEarth, *vSun, *vMoon, *vEarth, *aEarth;
    SgMatrix                   *pPsiEpsWahr, *pPsiEpsWahr_rates, *pPsiEps, *pPsiEps_rates;
    SgMatrix                   *pXys, *pXys_rates, *pmXy;
    SgVector                   *ut1_tai;


    //
    vCalcDelay    = new SgVector(numOfObs);
    vCalcRate     = new SgVector(numOfObs);
    vFractC       = new SgVector(numOfObs);

    cEarthTide    = new SgMatrix(numOfObs, 2);
//    cFeedCorr     = new SgMatrix(numOfObs, 2);
    cOceanTide    = new SgMatrix(numOfObs, 2);
    cOceanTideOld = new SgMatrix(numOfObs, 2);
    cOceanPoleTide= new SgMatrix(numOfObs, 2);
    cPoleTide     = new SgMatrix(numOfObs, 2);
    cPoleTideOld  = new SgMatrix(numOfObs, 2);
    cTiltRmvr     = new SgMatrix(numOfObs, 2);
    mHiFyUt1      = new SgMatrix(numOfObs, 2);
    mHiFyPxy      = new SgMatrix(numOfObs, 2);
    mLibrUt1      = new SgMatrix(numOfObs, 2);
    mLibrPxy      = new SgMatrix(numOfObs, 2);
    mPx           = new SgMatrix(numOfObs, 2);
    mPy           = new SgMatrix(numOfObs, 2);
    cBend         = new SgMatrix(numOfObs, 2);
    cBendSun      = new SgMatrix(numOfObs, 2);
    cBendSunHigher= new SgMatrix(numOfObs, 2);
    
    partBend      = new SgMatrix(numOfObs, 2);
    dV_dPx        = new SgMatrix(numOfObs, 2);
    dV_dPy        = new SgMatrix(numOfObs, 2);
    dV_dUT1       = new SgMatrix(numOfObs, 4);
    dV_dCipX      = new SgMatrix(numOfObs, 2);
    dV_dCipY      = new SgMatrix(numOfObs, 2);
    dV_dRA        = new SgMatrix(numOfObs, 2);
    dV_dDN        = new SgMatrix(numOfObs, 2);
    dDel_dR_1     = new SgMatrix(numOfObs, 3);
    dRat_dR_1     = new SgMatrix(numOfObs, 3);
    dV_dGamma     = new SgMatrix(numOfObs, 2);
    dV_dParallax  = new SgMatrix(numOfObs, 2);
    dV_dPtdX      = new SgMatrix(numOfObs, 2);
    dV_dPtdY      = new SgMatrix(numOfObs, 2);
    mPlxRev       = new SgMatrix(numOfObs, 2);
    rSun          = new SgMatrix(numOfScans, 3);
    rMoon         = new SgMatrix(numOfScans, 3);
    rEarth        = new SgMatrix(numOfScans, 3);
    vSun          = new SgMatrix(numOfScans, 3);
    vMoon         = new SgMatrix(numOfScans, 3);
    vEarth        = new SgMatrix(numOfScans, 3);
    aEarth        = new SgMatrix(numOfScans, 3);

    pPsiEpsWahr   = new SgMatrix(numOfScans, 2);
    pPsiEpsWahr_rates
                  = new SgMatrix(numOfScans, 2);
    pPsiEps       = new SgMatrix(numOfScans, 2);
    pPsiEps_rates = new SgMatrix(numOfScans, 2);
    pXys          = new SgMatrix(numOfScans, 3);
    pXys_rates    = new SgMatrix(numOfScans, 3);

    t2c_val       = new SgMatrix(numOfScans, 9);
    t2c_rat       = new SgMatrix(numOfScans, 9);
    t2c_acc       = new SgMatrix(numOfScans, 9);
    pmXy          = new SgMatrix(numOfScans, 2);
    ut1_tai       = new SgVector(numOfScans);
    
    scanIdx = 0;
    for (int obsIdx=0; obsIdx<numOfObs; obsIdx++)
    {
      SgVlbiObservation        *obs=observations_.at(obsIdx);
      int                       idx=obs->getMediaIdx();

      vCalcDelay  ->setElement(idx,           obs->getCalcConsensusDelay());
      vCalcRate   ->setElement(idx,           obs->getCalcConsensusRate());
      vFractC     ->setElement(idx,           obs->getFractC());
      // contributions:
      cEarthTide    ->setElement(idx, 0,      obs->getCalcEarthTideDelay());
      cEarthTide    ->setElement(idx, 1,      obs->getCalcEarthTideRate());
//      cFeedCorr     ->setElement(idx, 0,      obs->getCalcFeedCorrDelay());
//      cFeedCorr     ->setElement(idx, 1,      obs->getCalcFeedCorrRate());
      cOceanTide    ->setElement(idx, 0,      obs->getCalcOceanTideDelay());
      cOceanTide    ->setElement(idx, 1,      obs->getCalcOceanTideRate());
      cOceanTideOld ->setElement(idx, 0,      obs->getCalcOceanTideOldDelay());
      cOceanTideOld ->setElement(idx, 1,      obs->getCalcOceanTideOldRate());
      cOceanPoleTide->setElement(idx, 0,      obs->getCalcOceanPoleTideLdDelay());
      cOceanPoleTide->setElement(idx, 1,      obs->getCalcOceanPoleTideLdRate());
      cPoleTide     ->setElement(idx, 0,      obs->getCalcPoleTideDelay());
      cPoleTide     ->setElement(idx, 1,      obs->getCalcPoleTideRate());
      cPoleTideOld  ->setElement(idx, 0,      obs->getCalcPoleTideOldDelay());
      cPoleTideOld  ->setElement(idx, 1,      obs->getCalcPoleTideOldRate());
      cTiltRmvr     ->setElement(idx, 0,      obs->getCalcTiltRemvrDelay());
      cTiltRmvr     ->setElement(idx, 1,      obs->getCalcTiltRemvrRate());
      mHiFyUt1      ->setElement(idx, 0,      obs->getCalcHiFyUt1Delay());
      mHiFyUt1      ->setElement(idx, 1,      obs->getCalcHiFyUt1Rate());
      mHiFyPxy      ->setElement(idx, 0,      obs->getCalcHiFyPxyDelay());
      mHiFyPxy      ->setElement(idx, 1,      obs->getCalcHiFyPxyRate());
      mLibrUt1      ->setElement(idx, 0,      obs->getCalcHiFyUt1LibrationDelay());
      mLibrUt1      ->setElement(idx, 1,      obs->getCalcHiFyUt1LibrationRate());
      mLibrPxy      ->setElement(idx, 0,      obs->getCalcHiFyPxyLibrationDelay());
      mLibrPxy      ->setElement(idx, 1,      obs->getCalcHiFyPxyLibrationRate());
      mPx           ->setElement(idx, 0,      obs->getCalcPxDelay());
      mPx           ->setElement(idx, 1,      obs->getCalcPxRate());
      mPy           ->setElement(idx, 0,      obs->getCalcPyDelay());
      mPy           ->setElement(idx, 1,      obs->getCalcPyRate());
      cBend         ->setElement(idx, 0,      obs->getCalcConsBendingDelay());
      cBend         ->setElement(idx, 1,      obs->getCalcConsBendingRate());
      cBendSun      ->setElement(idx, 0,      obs->getCalcConsBendingSunDelay());
      cBendSun      ->setElement(idx, 1,      obs->getCalcConsBendingSunRate());
      cBendSunHigher->setElement(idx, 0,      obs->getCalcConsBendingSunHigherDelay());
      cBendSunHigher->setElement(idx, 1,      obs->getCalcConsBendingSunHigherRate());
      //
      mPlxRev       ->setElement(idx, 0,      obs->getDdel_dParallaxRev());
      mPlxRev       ->setElement(idx, 1,      obs->getDrat_dParallaxRev());
      // 
      // partials:
      partBend    ->setElement(idx, 0,        obs->getDdel_dBend());
      partBend    ->setElement(idx, 1,        obs->getDrat_dBend());
      dV_dPx      ->setElement(idx, 0,        obs->getDdel_dPx());
      dV_dPx      ->setElement(idx, 1,        obs->getDrat_dPx());
      dV_dPy      ->setElement(idx, 0,        obs->getDdel_dPy());
      dV_dPy      ->setElement(idx, 1,        obs->getDrat_dPy());
//    dV_dUT1     ->setElement(idx, 0,        obs->getDdel_dUT1());
//    dV_dUT1     ->setElement(idx, 1,        obs->getDrat_dUT1());
      dV_dUT1     ->setElement(idx, 0,        obs->getDdel_dUT1()/86400.0);
      dV_dUT1     ->setElement(idx, 1,        obs->getDrat_dUT1()/86400.0);

      dV_dUT1     ->setElement(idx, 2,        obs->getD2del_dUT12());
      dV_dUT1     ->setElement(idx, 3,        obs->getD2rat_dUT12());
      dV_dCipX    ->setElement(idx, 0,        obs->getDdel_dCipX());
      dV_dCipX    ->setElement(idx, 1,        obs->getDrat_dCipX());
      dV_dCipY    ->setElement(idx, 0,        obs->getDdel_dCipY());
      dV_dCipY    ->setElement(idx, 1,        obs->getDrat_dCipY());
      dV_dRA      ->setElement(idx, 0,        obs->getDdel_dRA());
      dV_dRA      ->setElement(idx, 1,        obs->getDrat_dRA());
      dV_dDN      ->setElement(idx, 0,        obs->getDdel_dDN());
      dV_dDN      ->setElement(idx, 1,        obs->getDrat_dDN());
      dDel_dR_1   ->setElement(idx, 0,        obs->getDdel_dR_1().at(X_AXIS));
      dDel_dR_1   ->setElement(idx, 1,        obs->getDdel_dR_1().at(Y_AXIS));
      dDel_dR_1   ->setElement(idx, 2,        obs->getDdel_dR_1().at(Z_AXIS));
      dRat_dR_1   ->setElement(idx, 0,        obs->getDrat_dR_1().at(X_AXIS));
      dRat_dR_1   ->setElement(idx, 1,        obs->getDrat_dR_1().at(Y_AXIS));
      dRat_dR_1   ->setElement(idx, 2,        obs->getDrat_dR_1().at(Z_AXIS));
      dV_dGamma   ->setElement(idx, 0,        obs->getDdel_dGamma());
      dV_dGamma   ->setElement(idx, 1,        obs->getDrat_dGamma());
      dV_dParallax->setElement(idx, 0,        obs->getDdel_dParallax());
      dV_dParallax->setElement(idx, 1,        obs->getDrat_dParallax());
      dV_dPtdX    ->setElement(idx, 0,        obs->getDdel_dPolTideX());
      dV_dPtdX    ->setElement(idx, 1,        obs->getDrat_dPolTideX());
      dV_dPtdY    ->setElement(idx, 0,        obs->getDdel_dPolTideY());
      dV_dPtdY    ->setElement(idx, 1,        obs->getDrat_dPolTideY());
      //
      //
      // scan-based stuff:
      if (!epochByScanId.contains(obs->getScanId())) // new scan:
      {
/*
std::cout << " ++ filling the matrix: obsIdx= " << idx << "  "
<< " scanIdx= " << scanIdx
<< " obs->getScanId()=[" << qPrintable(obs->getScanId()) << "]"
<< "\n";
*/
        epochByScanId.insert(obs->getScanId(), *obs);
        const Sg3dVector       *r=&obs->getRsun();
        rSun->setElement(scanIdx, 0,   r->at(X_AXIS));
        rSun->setElement(scanIdx, 1,   r->at(Y_AXIS));
        rSun->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getVsun();
        vSun->setElement(scanIdx, 0,   r->at(X_AXIS));
        vSun->setElement(scanIdx, 1,   r->at(Y_AXIS));
        vSun->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getRmoon();
        rMoon->setElement(scanIdx, 0,   r->at(X_AXIS));
        rMoon->setElement(scanIdx, 1,   r->at(Y_AXIS));
        rMoon->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getVmoon();
        vMoon->setElement(scanIdx, 0,   r->at(X_AXIS));
        vMoon->setElement(scanIdx, 1,   r->at(Y_AXIS));
        vMoon->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getRearth();
        rEarth->setElement(scanIdx, 0,   r->at(X_AXIS));
        rEarth->setElement(scanIdx, 1,   r->at(Y_AXIS));
        rEarth->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getVearth();
        vEarth->setElement(scanIdx, 0,   r->at(X_AXIS));
        vEarth->setElement(scanIdx, 1,   r->at(Y_AXIS));
        vEarth->setElement(scanIdx, 2,   r->at(Z_AXIS));
        r = &obs->getAearth();
        aEarth->setElement(scanIdx, 0,   r->at(X_AXIS));
        aEarth->setElement(scanIdx, 1,   r->at(Y_AXIS));
        aEarth->setElement(scanIdx, 2,   r->at(Z_AXIS));

        pPsiEpsWahr       ->setElement(scanIdx, 0,    obs->getCalcNutWahr_dPsiV());
        pPsiEpsWahr       ->setElement(scanIdx, 1,    obs->getCalcNutWahr_dEpsV());
        pPsiEpsWahr_rates ->setElement(scanIdx, 0,    obs->getCalcNutWahr_dPsiR());
        pPsiEpsWahr_rates ->setElement(scanIdx, 1,    obs->getCalcNutWahr_dEpsR());
        pPsiEps           ->setElement(scanIdx, 0,    obs->getCalcNut2006_dPsiV());
        pPsiEps           ->setElement(scanIdx, 1,    obs->getCalcNut2006_dEpsV());
        pPsiEps_rates     ->setElement(scanIdx, 0,    obs->getCalcNut2006_dPsiR());
        pPsiEps_rates     ->setElement(scanIdx, 1,    obs->getCalcNut2006_dEpsR());
        pXys              ->setElement(scanIdx, 0,    obs->getCalcCipXv());
        pXys              ->setElement(scanIdx, 1,    obs->getCalcCipYv());
        pXys              ->setElement(scanIdx, 2,    obs->getCalcCipSv());
        pXys_rates        ->setElement(scanIdx, 0,    obs->getCalcCipXr());
        pXys_rates        ->setElement(scanIdx, 1,    obs->getCalcCipYr());
        pXys_rates        ->setElement(scanIdx, 2,    obs->getCalcCipSr());

        const Sg3dMatrix       &t2c_v=obs->getTrf2crfVal();
        const Sg3dMatrix       &t2c_r=obs->getTrf2crfRat();
        const Sg3dMatrix       &t2c_a=obs->getTrf2crfAcc();
        for (int k=0; k<3; k++)
          for (int l=0; l<3; l++)
          {
            t2c_val       ->setElement(scanIdx, 3*k+l,  t2c_v.at((DIRECTION)k, (DIRECTION)l));
            t2c_rat       ->setElement(scanIdx, 3*k+l,  t2c_r.at((DIRECTION)k, (DIRECTION)l));
            t2c_acc       ->setElement(scanIdx, 3*k+l,  t2c_a.at((DIRECTION)k, (DIRECTION)l));
          };
        ut1_tai           ->setElement(scanIdx,       obs->getCalcUt1_Tai());
        pmXy              ->setElement(scanIdx, 0,    obs->getCalcPmX());
        pmXy              ->setElement(scanIdx, 1,    obs->getCalcPmY());
        //
        scanIdx++;
      };
    };
    isOk = isOk && vgosDb->storeObsDelayTheoretical(vCalcDelay);

    isOk = isOk && vgosDb->storeObsRateTheoretical (vCalcRate);
    isOk = isOk && vgosDb->storeObsFractC(vFractC);
    //

    isOk = isOk && vgosDb->storeObsCalEarthTide(cEarthTide, kind4EarthTide);
    isOk = isOk && vgosDb->storeObsCalOcean(cOceanTide);
    isOk = isOk && vgosDb->storeObsCalOceanOld(cOceanTideOld);
    isOk = isOk && vgosDb->storeObsCalOceanPoleTideLoad(cOceanPoleTide);
    isOk = isOk && vgosDb->storeObsCalPoleTide(cPoleTide, kind4PoleTide);
    isOk = isOk && vgosDb->storeObsCalPoleTideOldRestore(cPoleTideOld);
    isOk = isOk && vgosDb->storeObsCalTiltRmvr(cTiltRmvr);
    
    isOk = isOk && vgosDb->storeObsCalHiFyErp(mHiFyUt1, mHiFyPxy, kind4HiFreqErp);
    isOk = isOk && vgosDb->storeObsCalHiFyLibration(mLibrUt1, mLibrPxy, kind4HiFreqLibration);
    isOk = isOk && vgosDb->storeObsCalWobble(mPx, mPy);
    isOk = isOk && vgosDb->storeObsCalParallax(mPlxRev);
    isOk = isOk && vgosDb->storeObsCalBend(cBend);
    isOk = isOk && vgosDb->storeObsCalBendSun(cBendSun);
    isOk = isOk && vgosDb->storeObsCalBendSunHigher(cBendSunHigher);
    //
    isOk = isOk && vgosDb->storeObsPartBend(partBend);
    isOk = isOk && vgosDb->storeObsPartEOP(dV_dPx, dV_dPy, dV_dUT1);
    isOk = isOk && vgosDb->storeObsPartNut2KXY(dV_dCipX, dV_dCipY, kind4Nutation);
    isOk = isOk && vgosDb->storeObsPartRaDec(dV_dRA, dV_dDN);
    isOk = isOk && vgosDb->storeObsPartXYZ(dDel_dR_1, dRat_dR_1);
    isOk = isOk && vgosDb->storeObsPartGamma(dV_dGamma);
    isOk = isOk && vgosDb->storeObsPartParallax(dV_dParallax);
    isOk = isOk && vgosDb->storeObsPartPoleTides(dV_dPtdX, dV_dPtdY, kind4PoleTidePart);
    //
    //
    // scan dependent data:
    isOk = isOk && vgosDb->storeScanEphemeris(rSun, rMoon, rEarth, vSun, vMoon, vEarth, aEarth, kind4Ephemeris);
    isOk = isOk && vgosDb->storeScanNutationEqxWahr(pPsiEpsWahr, pPsiEpsWahr_rates);
    isOk = isOk && vgosDb->storeScanNutationEqx(pPsiEps, pPsiEps_rates, kind4Nutation);
    isOk = isOk && vgosDb->storeScanNutationNro(pXys, pXys_rates, kind4Nutation);
    isOk = isOk && vgosDb->storeScanTrf2crf(t2c_val, t2c_rat, t2c_acc);
    isOk = isOk && vgosDb->storeErpApriori(ut1_tai, pmXy);
    
    
    delete pmXy;
    delete ut1_tai;
    delete t2c_val;
    delete t2c_rat;
    delete t2c_acc;
    delete pPsiEpsWahr;
    delete pPsiEpsWahr_rates;
    delete pPsiEps;
    delete pPsiEps_rates;
    delete pXys;
    delete pXys_rates;

    delete rSun;
    delete rMoon;
    delete rEarth;
    delete vSun;
    delete vMoon;
    delete vEarth;
    delete aEarth;
    delete mPlxRev;
    delete dV_dGamma;
    delete dV_dParallax;
    delete dV_dPtdX;
    delete dV_dPtdY;
    delete dDel_dR_1;
    delete dRat_dR_1;
    delete dV_dRA;
    delete dV_dDN;
    delete dV_dCipX;
    delete dV_dCipY;
    delete dV_dPx;
    delete dV_dPy;
    delete dV_dUT1;
    delete partBend;
    delete mHiFyUt1;
    delete mHiFyPxy;
    delete mLibrUt1;
    delete mLibrPxy;
    delete mPx;
    delete mPy;
    delete cBend;
    delete cBendSun;
    delete cBendSunHigher;

    delete cEarthTide;
    delete cOceanTide;
    delete cOceanTideOld;
    delete cOceanPoleTide;
    delete cPoleTide;
    delete cPoleTideOld;
    delete cTiltRmvr;

    delete vFractC;
    delete vCalcDelay;
    delete vCalcRate;
    //
    //
    // per station (theoreticals and partials):
    QString                     kind4TroposphereModel("NMF");
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      const QString             stnName(stn->getKey());
      SgVector                 *parAngle;
      SgMatrix                 *azTheo, *elTheo;
      SgMatrix                 *calAxisOffset;
      SgMatrix                 *calNmfDry, *calNmfWet;
      SgMatrix                 *calHorzOLoad, *calVertOLoad;
      SgMatrix                 *ocnLdR, *ocnLdV;
      SgMatrix                 *partAxisOffset;
      SgMatrix                 *partTropZenithDry, *partTropZenithWet, *partTropGrad;
      
      int                       idx, num=stn->auxObservationByScanId()->size();

      parAngle          = new SgVector(num);
      azTheo            = new SgMatrix(num, 2);
      elTheo            = new SgMatrix(num, 2);
      ocnLdR            = new SgMatrix(num, 3);
      ocnLdV            = new SgMatrix(num, 3);
      calAxisOffset     = new SgMatrix(num, 2);
      calNmfDry         = new SgMatrix(num, 2);
      calNmfWet         = new SgMatrix(num, 2);
      calHorzOLoad      = new SgMatrix(num, 2);
      calVertOLoad      = new SgMatrix(num, 2);
      partAxisOffset    = new SgMatrix(num, 2);
      partTropZenithDry = new SgMatrix(num, 2);
      partTropZenithWet = new SgMatrix(num, 2);
      partTropGrad      = new SgMatrix(num, 4);
      
      idx = 0;
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt, idx++)
      {
        SgVlbiAuxObservation   *aux=jt.value();
        parAngle->setElement(idx,           aux->getParallacticAngle());
        azTheo->setElement(idx, 0,          aux->getAzimuthAngle());
        azTheo->setElement(idx, 1,          aux->getAzimuthAngleRate());
        elTheo->setElement(idx, 0,          aux->getElevationAngle());
        elTheo->setElement(idx, 1,          aux->getElevationAngleRate());
        ocnLdR->setElement(idx, 0,          aux->getOcnLdR().at(X_AXIS));
        ocnLdR->setElement(idx, 1,          aux->getOcnLdR().at(Y_AXIS));
        ocnLdR->setElement(idx, 2,          aux->getOcnLdR().at(Z_AXIS));
        ocnLdV->setElement(idx, 0,          aux->getOcnLdV().at(X_AXIS));
        ocnLdV->setElement(idx, 1,          aux->getOcnLdV().at(Y_AXIS));
        ocnLdV->setElement(idx, 2,          aux->getOcnLdV().at(Z_AXIS));
        //
        calAxisOffset->setElement (idx, 0,  aux->getCalcAxisOffset4Delay());
        calAxisOffset->setElement (idx, 1,  aux->getCalcAxisOffset4Rate());
        calNmfDry->setElement     (idx, 0,  aux->getCalcNdryCont4Delay());
        calNmfDry->setElement     (idx, 1,  aux->getCalcNdryCont4Rate());
        calNmfWet->setElement     (idx, 0,  aux->getCalcNwetCont4Delay());
        calNmfWet->setElement     (idx, 1,  aux->getCalcNwetCont4Rate());
        calHorzOLoad->setElement  (idx, 0,  aux->getCalcOLoadHorz4Delay());
        calHorzOLoad->setElement  (idx, 1,  aux->getCalcOLoadHorz4Rate());
        calVertOLoad->setElement  (idx, 0,  aux->getCalcOLoadVert4Delay());
        calVertOLoad->setElement  (idx, 1,  aux->getCalcOLoadVert4Rate());
        //
        partAxisOffset->setElement(idx, 0,  aux->getDdel_dAxsOfs());
        partAxisOffset->setElement(idx, 1,  aux->getDrat_dAxsOfs());
        
        partTropZenithDry->setElement(idx, 0,  aux->getDdel_dTzdDry());
        partTropZenithDry->setElement(idx, 1,  aux->getDrat_dTzdDry());
        partTropZenithWet->setElement(idx, 0,  aux->getDdel_dTzdWet());
        partTropZenithWet->setElement(idx, 1,  aux->getDrat_dTzdWet());
        partTropGrad->setElement(idx, 0,  aux->getDdel_dTzdGrdN());
        partTropGrad->setElement(idx, 1,  aux->getDdel_dTzdGrdE());
        partTropGrad->setElement(idx, 2,  aux->getDrat_dTzdGrdN());
        partTropGrad->setElement(idx, 3,  aux->getDrat_dTzdGrdE());
      };

      isOk = isOk && vgosDb->storeStationParAngle(stnName, parAngle);
      isOk = isOk && vgosDb->storeStationAzEl(stnName, azTheo, elTheo);
      isOk = isOk && vgosDb->storeStationOceanLdDisp(stnName, ocnLdR, ocnLdV);
      // contributions:
      isOk = isOk && vgosDb->storeStationCalAxisOffset(stnName, calAxisOffset);
      isOk = isOk && vgosDb->storeStationCalSlantPathTropDry(stnName, calNmfDry, kind4TroposphereModel);
      isOk = isOk && vgosDb->storeStationCalSlantPathTropWet(stnName, calNmfWet, kind4TroposphereModel);
      isOk = isOk && vgosDb->storeStationCalOceanLoad(stnName, calHorzOLoad, calVertOLoad);
      // partials:
      isOk = isOk && vgosDb->storeStationPartAxisOffset(stnName, partAxisOffset);
      isOk = isOk && vgosDb->storeStationPartZenithDelayDry(stnName, partTropZenithDry, 
        kind4TroposphereModel);
      isOk = isOk && vgosDb->storeStationPartZenithDelayWet(stnName, partTropZenithWet, 
        kind4TroposphereModel);
      isOk = isOk && vgosDb->storeStationPartHorizonGrad(stnName, partTropGrad, kind4TroposphereModel);
      
      
      //
      delete partTropZenithDry;
      delete partTropZenithWet;
      delete partTropGrad;
      delete ocnLdR;
      delete ocnLdV;
      delete calHorzOLoad;
      delete calVertOLoad;
      delete calNmfDry;
      delete calNmfWet;
      delete parAngle;
      delete calAxisOffset;
      delete partAxisOffset;
      delete azTheo;
      delete elTheo;
    };
    //
    //
    // the session-wide data:
    // Calc Info:
    isOk = isOk && vgosDb->storeCalcInfo(calcInfo_);
    //
    // ERP (if exist):
    if (args4Ut1Interpolation_ && args4Ut1Interpolation_->n()>0     &&
        tabs4Ut1Interpolation_ && tabs4Ut1Interpolation_->nRow()>0  &&
        args4PxyInterpolation_ && args4PxyInterpolation_->n()>0     &&
        tabs4PxyInterpolation_ && tabs4PxyInterpolation_->nRow()>0   )
    {
      SgVector                   *utArrayInfo, *pmArrayInfo, *utValues;
      SgMatrix                   *pmValues;
      double                      d;
      utArrayInfo = new SgVector(4);
      pmArrayInfo = new SgVector(3);
      utValues = new SgVector(tabs4Ut1Interpolation_->nRow());
      pmValues = new SgMatrix(tabs4PxyInterpolation_->nRow(), 2);
      // 0:
      d = args4Ut1Interpolation_->getElement(0);
      utArrayInfo->setElement(0, d>2390000.0?d:d+2400000.5);
      d = args4PxyInterpolation_->getElement(0);
      pmArrayInfo->setElement(0, d>2390000.0?d:d+2400000.5);
      // 1:
      utArrayInfo->setElement(1, args4Ut1Interpolation_->n()>1?
        (args4Ut1Interpolation_->getElement(1) - args4Ut1Interpolation_->getElement(0)):1.0);
      pmArrayInfo->setElement(1, args4PxyInterpolation_->n()>1?
        (args4PxyInterpolation_->getElement(1) - args4PxyInterpolation_->getElement(0)):1.0);
      // 2:
      utArrayInfo->setElement(2, args4Ut1Interpolation_->n());
      pmArrayInfo->setElement(2, args4PxyInterpolation_->n());
      // 3:
      utArrayInfo->setElement(3, 1.0);
      //
      for (unsigned int i=0; i<tabs4Ut1Interpolation_->nRow(); i++)
        utValues->setElement(i, tabs4Ut1Interpolation_->getElement(i, 0));
      for (unsigned int i=0; i<tabs4PxyInterpolation_->nRow(); i++)
      {
        pmValues->setElement(i, 0,  tabs4PxyInterpolation_->getElement(i, 0));
        pmValues->setElement(i, 1,  tabs4PxyInterpolation_->getElement(i, 1));
      };
      //
      isOk = isOk && vgosDb->storeCalcEop(pmArrayInfo, utArrayInfo, pmValues, utValues, calcInfo_);
      //
      delete utArrayInfo;
      delete pmArrayInfo;
      delete utValues;
      delete pmValues;
    };
  }; // end of CALC's first part
  //
  //
  // the second part, a priori:

  if (isAttr(Attr_FF_CALC_DATA_MODIFIED) || 
      getOriginType()==OT_AGV             )
  {
    //                                   a priori:
    QList<QString>              sourcesNames, sourcesRefs, stationsNames, tectonicPlateNames;
    QList<int>                  axisTypes;
    SgVector                   *axisOffsets;
    SgMatrix                   *cooRaDe, *cooXYZ, *axisTilts;
    
    cooRaDe     = new SgMatrix(sourcesByName_.size(),  2);
    cooXYZ      = new SgMatrix(stationsByName_.size(), 3);
    axisTilts   = new SgMatrix(stationsByName_.size(), 2);
    axisOffsets = new SgVector(stationsByName_.size());
    // collect info:
    int                         idx=0;
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
    {
      SgVlbiSourceInfo         *src=it.value();
      sourcesNames.append(src->getKey());
      sourcesRefs .append(src->getAprioriReference());
      cooRaDe->setElement(idx, 0, src->getRA());
      cooRaDe->setElement(idx, 1, src->getDN());
      idx++;
    };
    idx = 0;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      const Sg3dVector         &r=stn->getR();
      stationsNames     .append(stn->getKey());
      tectonicPlateNames.append(stn->getTectonicPlateName());
      //
      axisTypes.append(SgVlbiStationInfo::mntType2int(stn->getMntType()));
      axisOffsets->setElement(idx, stn->getAxisOffset());
      axisTilts->setElement(idx, 0, stn->getTilt(0));
      axisTilts->setElement(idx, 1, stn->getTilt(1));
      cooXYZ->setElement(idx, 0, r.at(X_AXIS));
      cooXYZ->setElement(idx, 1, r.at(Y_AXIS));
      cooXYZ->setElement(idx, 2, r.at(Z_AXIS));
      idx++;
    };
    
    // save the data:
    isOk = isOk && vgosDb->storeSourcesApriories(sourcesNames, sourcesRefs, cooRaDe);
    isOk = isOk && vgosDb->storeStationsApriories(stationsNames, cooXYZ, tectonicPlateNames);
//    // these data are not stored in agv files:
//    if (isAttr(Attr_FF_CALC_DATA_MODIFIED))
    isOk = isOk && vgosDb->storeAntennaApriories(stationsNames, axisTypes, axisOffsets, axisTilts);

    //
    if (getLeapSeconds() > 0)
      isOk = isOk && vgosDb->storeLeapSecond(getLeapSeconds());

    sourcesNames.clear();
    sourcesRefs.clear();
    stationsNames.clear();
    tectonicPlateNames.clear();

    delete axisOffsets;
    delete axisTilts;
    delete cooXYZ;
    delete cooRaDe;
  }; // end of the second part of CALC
  //
  //
  // the last part of CALC:
  if (isAttr(Attr_FF_CALC_DATA_MODIFIED))
  {
    //                                  band dependent data:
    // create carriers:
    QMap<QString, SgMatrix*>    uvfPerAsecByBand, feedCorrByBand;
    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey=bands_.at(iBand)->getKey();
      uvfPerAsecByBand.insert(bandKey, new SgMatrix(numOfObs, 2));
      feedCorrByBand.insert  (bandKey, new SgMatrix(numOfObs, 2));
    };
    // fill them:
    for (int iObs=0; iObs<numOfObs; iObs++)
    {
      SgVlbiObservation        *obs=observations_.at(iObs);
      for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
        it!=obs->observableByKey().end(); ++it)
      {
        SgVlbiObservable       *o=it.value();
        uvfPerAsecByBand[o->getBandKey()]->setElement(o->getMediaIdx(),0,   o->getUvFrPerAsec(0));
        uvfPerAsecByBand[o->getBandKey()]->setElement(o->getMediaIdx(),1,   o->getUvFrPerAsec(1));
        feedCorrByBand  [o->getBandKey()]->setElement(o->getMediaIdx(),0,   o->getCalcFeedCorrDelay());
        feedCorrByBand  [o->getBandKey()]->setElement(o->getMediaIdx(),1,   o->getCalcFeedCorrRate ());
      };
    };
    // write data into netCDF files:
    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey=bands_.at(iBand)->getKey();
      isOk = isOk && vgosDb->storeObsUVFperAsec (bandKey, uvfPerAsecByBand[bandKey]);
      isOk = isOk && vgosDb->storeObsCalFeedCorr(bandKey, feedCorrByBand  [bandKey]);
    };
    // free memmory:
    for (QMap<QString, SgMatrix*>::iterator it=uvfPerAsecByBand.begin();
      it!=uvfPerAsecByBand.end(); ++it)
      delete it.value();
    for (QMap<QString, SgMatrix*>::iterator it=feedCorrByBand.begin(); 
      it!=feedCorrByBand.end(); ++it)
      delete it.value();
    //
    //
  };
  //
  //
  //                                    end of CALC_MODIFIED
  //
  //
  //
  // stations (data from log files):
  if (isAttr(Attr_FF_AUX_OBS_MODIFIED) ||
      getOriginType()==OT_AGV           )
  {
    QString                     kind("");
    QMap<QString, QString>      cableSignByKey;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      const QString             stnName(stn->getKey());
      SgVector                 *metAtmPres, *metAtmRh, *metAtmTemp;
      SgMatrix                 *cableCal, *cblCorrections;
      SgMatrix                 *tSyses;
      int                       idx, num=stn->auxObservationByScanId()->size();
      int                       numTsChans=stn->tsysIfFreqs().size();
      QVector<double>           tsysFreqs(numTsChans);
      QVector<QString>          tsysIds(numTsChans), tsysSbs(numTsChans), tsysPzs(numTsChans);


      metAtmPres        = new SgVector(num);
      metAtmRh          = new SgVector(num);
      metAtmTemp        = new SgVector(num);
      cableCal          = new SgMatrix(num, 1);
      cblCorrections    = new SgMatrix(num, 3);
      tSyses    = new SgMatrix(num, numTsChans);
      for (int j=0; j<numTsChans; j++)
      {
        tsysFreqs[j] = stn->tsysIfFreqs()[j];
        tsysIds[j] = stn->tsysIfIds()[j];
        tsysSbs[j] = stn->tsysIfSideBands()[j];
        tsysPzs[j] = stn->tsysIfPolarizations()[j];
      };
      idx = 0;
      for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=stn->auxObservationByScanId()->begin();
        jt!=stn->auxObservationByScanId()->end(); ++jt, idx++)
      {
        SgVlbiAuxObservation   *aux=jt.value();
        
        metAtmPres    ->setElement(idx, aux->meteoData().getPressure());
        metAtmRh      ->setElement(idx, aux->meteoData().getRelativeHumidity());
        metAtmTemp    ->setElement(idx, aux->meteoData().getTemperature());
        
        cableCal      ->setElement(idx, 0,  aux->getCableCalibration());
        cblCorrections->setElement(idx, SgVlbiAuxObservation::CCT_FSLG,
          aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_FSLG));
        cblCorrections->setElement(idx, SgVlbiAuxObservation::CCT_CDMS,
          aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_CDMS));
        cblCorrections->setElement(idx, SgVlbiAuxObservation::CCT_PCMT,
          aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_PCMT));
        for (int j=0; j<numTsChans; j++)
          tSyses->setElement(idx, j, aux->tSyses()->getElement(j));
      };
      //
      if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL)    && 
          stn->isAttr(SgVlbiStationInfo::Attr_CABLE_CAL_MODIFIED))
      {
        if (stn->getCableCalsOrigin() == SgVlbiStationInfo::CCO_FS_LOG)
          kind = "";
        else if (stn->getCableCalsOrigin() == SgVlbiStationInfo::CCO_CDMS)
          kind = "Cdms";
        else if (stn->getCableCalsOrigin() == SgVlbiStationInfo::CCO_PCMT)
          kind = "Pcmt";
        else if (stn->getCableCalsOrigin() == SgVlbiStationInfo::CCO_CDMS_FS_LOG)
          kind = "CDMS";
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::putDataIntoVgosDb(): the station " + stnName + 
            " has an unknown kind of cable calibration data");
        isOk = isOk && vgosDb->storeStationCalCable(stnName, cableCal, stn->getCableCalsOriginTxt(), 
          kind);
        isOk = isOk && vgosDb->storeStationCalCblCorrections(stnName, cblCorrections, 
          "Various input files (e.g, FS log, CDMS, PCMT)");
        if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
          cableSignByKey[stnName] = stn->getCableCalMultiplierDBCal()==-1.0?"-":
            (stn->getCableCalMultiplierDBCal()==1.0?"+":"?");
        else
          cableSignByKey[stnName] = " ";
      };
      if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO)    &&
          stn->isAttr(SgVlbiStationInfo::Attr_METEO_MODIFIED))
      {

        if (stn->getMeteoDataOrigin() == SgVlbiStationInfo::MDO_FS_LOG)
          kind = "";
        else if (stn->getMeteoDataOrigin() == SgVlbiStationInfo::MDO_EXTERNAL)
          kind = "External";
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::putDataIntoVgosDb(): the station " + stnName + 
            " has an unknown kind of meteo data");
        isOk = isOk && vgosDb->storeStationMet(stnName, metAtmPres, metAtmRh, metAtmTemp, 
          stn->getMeteoDataOriginTxt(), kind);
      };
      if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_TSYS)    &&
          stn->isAttr(SgVlbiStationInfo::Attr_TSYS_MODIFIED))
      {
        isOk = isOk && vgosDb->storeStationTsys(stnName, tSyses, tsysFreqs, tsysIds, tsysSbs, tsysPzs);
      };

      delete metAtmPres;
      delete metAtmRh;
      delete metAtmTemp;
      delete cableCal;
      delete cblCorrections;      
    };
    //
    // save cable cal signs that were applied to stations' data:
    if (cableSignByKey.size())
      isOk = isOk && vgosDb->storeMiscCable(cableSignByKey);
    cableSignByKey.clear();
  };
  //



  // ionospheric corrections:
  if (isAttr(Attr_FF_ION_C_CALCULATED) || 
      getOriginType()==OT_AGV           )
//    cppsSoft_ == CPPS_PIMA            )
  {
    // create carriers:
    QMap<QString, SgMatrix*>    ionCalsByBand, ionSigsByBand;
    QMap<QString, SgMatrix*>    effFreqsByBand, effFreqsEqWgtByBand;
    QMap<QString, QVector<int> >ionDataFlagByBand;
    QVector<int>                ionBits;
    QString                     primeBandKey(primaryBand_?primaryBand_->getKey():"X");

    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey = bands_.at(iBand)->getKey();
      ionCalsByBand       .insert(bandKey, new SgMatrix(numOfObs, 2));
      ionSigsByBand       .insert(bandKey, new SgMatrix(numOfObs, 2));
      effFreqsByBand      .insert(bandKey, new SgMatrix(numOfObs, 3));
      effFreqsEqWgtByBand .insert(bandKey, new SgMatrix(numOfObs, 3));
      ionDataFlagByBand   .insert(bandKey, QVector<int>(numOfObs));
      for (int i=0; i<numOfObs; i++)
        ionDataFlagByBand[bandKey][i] = -1;
    };
    ionBits.resize(numOfObs);
    // fill them:
    for (int iObs=0; iObs<numOfObs; iObs++)
    {
      SgVlbiObservation        *obs=observations_.at(iObs);
      //
      for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
        it!=obs->observableByKey().end(); ++it)
      {
        SgVlbiObservable       *o=it.value();
        const QString&          bandKey=o->getBandKey();
        int                     mediaIdx=o->getMediaIdx();
        ionCalsByBand       [bandKey]->setElement(mediaIdx, 0,  o->grDelay().getIonoValue());
        ionCalsByBand       [bandKey]->setElement(mediaIdx, 1,  o->phDRate().getIonoValue());
        ionSigsByBand       [bandKey]->setElement(mediaIdx, 0,  o->grDelay().getIonoSigma());
        ionSigsByBand       [bandKey]->setElement(mediaIdx, 1,  o->phDRate().getIonoSigma());
        //
        effFreqsByBand      [bandKey]->setElement(mediaIdx, 0,  o->grDelay().getEffFreq());
        effFreqsByBand      [bandKey]->setElement(mediaIdx, 1,  o->phDelay().getEffFreq());
        effFreqsByBand      [bandKey]->setElement(mediaIdx, 2,  o->phDRate().getEffFreq());
        effFreqsEqWgtByBand [bandKey]->setElement(mediaIdx, 0,  o->grDelay().getEffFreqEqWgt());
        effFreqsEqWgtByBand [bandKey]->setElement(mediaIdx, 1,  o->phDelay().getEffFreqEqWgt());
        effFreqsEqWgtByBand [bandKey]->setElement(mediaIdx, 2,  o->phDRate().getEffFreqEqWgt());
        //
        if (obs->observableByKey().size() == 1)
          ionDataFlagByBand[o->getBandKey()][o->getMediaIdx()] = -1;
        else
        {
          ionDataFlagByBand[o->getBandKey()][o->getMediaIdx()] = 0;
          //The -3 flag is set if
          //ION_DELAY_SIGMA  < 1.d-14    or ION_RATE_SIGMA  < 1.d-17
          if (o->grDelay().getIonoSigma()<1.0e-14 || o->phDRate().getIonoSigma()<1.0e-17)
            ionDataFlagByBand[o->getBandKey()][o->getMediaIdx()] = -3;
          //The -4 flag is set if
          //ION_DELAY_SIGMA >1.d-8  or ION_RATE_SIGMA > 1.d-8
          if (o->grDelay().getIonoSigma()>1.0e-8 || o->phDRate().getIonoSigma()>1.0e-8)
            ionDataFlagByBand[o->getBandKey()][o->getMediaIdx()] = -4;
        };
      };
      ionBits[iObs] = obs->calculateIonoBits();
    };
    // write data into netCDF files:
    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey = bands_.at(iBand)->getKey();
//    if (isAttr(Attr_FF_ION_C_CALCULATED))
      isOk = isOk && vgosDb->storeObsCalIonGroup(bandKey, ionCalsByBand[bandKey], 
                                                  ionSigsByBand[bandKey], ionDataFlagByBand[bandKey]);
      isOk = isOk && vgosDb->storeObsEffFreqs(bandKey, effFreqsByBand[bandKey]);
      isOk = isOk && vgosDb->storeObsEffFreqs(bandKey, effFreqsEqWgtByBand[bandKey], true);
    };
//    if (isAttr(Attr_FF_ION_C_CALCULATED))
    isOk = isOk && vgosDb->storeIonoBits(ionBits);
    // free memmory:
    for (QMap<QString, SgMatrix*>::iterator it=ionCalsByBand.begin(); it!=ionCalsByBand.end(); ++it)
      delete it.value();
    for (QMap<QString, SgMatrix*>::iterator it=ionSigsByBand.begin(); it!=ionSigsByBand.end(); ++it)
      delete it.value();
    for (QMap<QString, SgMatrix*>::iterator it=effFreqsByBand.begin(); it!=effFreqsByBand.end(); ++it)
      delete it.value();
    for (QMap<QString, SgMatrix*>::iterator it=effFreqsEqWgtByBand.begin(); 
      it!=effFreqsEqWgtByBand.end(); ++it)
      delete it.value();
    effFreqsByBand.clear();
    effFreqsEqWgtByBand.clear();
    ionCalsByBand.clear();
    ionSigsByBand.clear();
    ionDataFlagByBand.clear();
    ionBits.clear();
  };
  // end of ionocorrections
  //
  //
  //
  if (isAttr(Attr_FF_OUTLIERS_PROCESSED) || 
      isAttr(Attr_FF_EDIT_INFO_MODIFIED) ||
      getOriginType()==OT_AGV             )
//    cppsSoft_ == CPPS_PIMA              )
  {
    QVector<int>                delUFlag, phsUFlag, ratUFlag, uAcSup;
    delUFlag.resize(numOfObs);
    phsUFlag.resize(numOfObs);
    ratUFlag.resize(numOfObs);
    uAcSup.resize(numOfObs);
    QString                     pbKey=primaryBand_->getKey();
    QString                     sbKey("");
    bool                        has2ndBand;
    has2ndBand = bands_.size() > 1;
    if (has2ndBand)
    {
      int                       bandIdx=0;
      sbKey = bands_.at(bandIdx++)->getKey();
      while (sbKey==pbKey && bandIdx<bands_.size())  // pick up a first non-prime band:
        sbKey = bands_.at(bandIdx++)->getKey();
    };
    for (int iObs=0; iObs<numOfObs; iObs++)
    {
      SgVlbiObservation        *obs=observations_.at(iObs);
      SgVlbiObservable         *pbO=obs->observable(pbKey);
      SgVlbiObservable         *sbO=has2ndBand?obs->observable(sbKey):NULL;
      int                       mediaIdx=obs->getMediaIdx();
      
      if (!pbO)
        std::cout << "Error: pbO is NULL\n";
      //
      // 
      if (getOriginType() != OT_AGV)
      {
        if (pbO->grDelay().getUnweightFlag()==0)
        {
          if (pbO->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
            pbO->grDelay().setUnweightFlag(1);
          else if (!pbO->grDelay().isAttr(SgVlbiMeasurement::Attr_PROCESSED))
            pbO->grDelay().setUnweightFlag(2);
          //
          if (sbO) // can be NULL
          {
            int                   qCodeSb=sbO->getQualityFactor();
            if (qCodeSb==0 && !obs->isAttr(SgObservation::Attr_PROCESSED))
              pbO->grDelay().setUnweightFlag(4);
          };
          //
          if (has2ndBand && !sbO)
            pbO->grDelay().setUnweightFlag(8);
        }
        else if (!pbO->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && 
                  obs->isAttr(SgObservation::Attr_PROCESSED))
          pbO->grDelay().setUnweightFlag(0);
      }
      else // cannot relay on Attr_PROCESSED flag:
      {
        if (pbO->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
          pbO->grDelay().setUnweightFlag(1);
        else if (sbO)
        {
          if (sbO->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
            pbO->grDelay().setUnweightFlag(2);
          if (sbO->getQualityFactor() == 0)
            pbO->grDelay().setUnweightFlag(4);
        }
        else if (has2ndBand)
          pbO->grDelay().setUnweightFlag(8);

        if (pbO->phDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
          pbO->phDelay().setUnweightFlag(1);
        if (pbO->phDRate().isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
          pbO->phDRate().setUnweightFlag(1);
      }; // end of vgosDa checking
      //
      //
      if (pbO->grDelay().getUnweightFlag()==0 && (sbO || !has2ndBand))
        uAcSup[mediaIdx] = -32768;
      else if (pbO->grDelay().getUnweightFlag()==1)
        uAcSup[mediaIdx] = -32767;
      else
        uAcSup[mediaIdx] = -32763;

      delUFlag[mediaIdx] = pbO->grDelay().getUnweightFlag();
      phsUFlag[mediaIdx] = pbO->phDelay().getUnweightFlag();
      ratUFlag[mediaIdx] = pbO->phDRate().getUnweightFlag();
    };
    isOk = isOk && vgosDb->storeObsEditData(delUFlag, phsUFlag, ratUFlag, uAcSup);
    delUFlag.clear();
    phsUFlag.clear();
    ratUFlag.clear();
    uAcSup.clear();
  };

  // numbers of ambiguities:
  if (isAttr(Attr_FF_AMBIGS_RESOLVED)    || 
      isAttr(Attr_FF_EDIT_INFO_MODIFIED) || 
      isAttr(Attr_FF_ION_C_CALCULATED)   ||
      getOriginType()==OT_AGV             )
//    cppsSoft_ == CPPS_PIMA              )
  {
    // create carriers:
    QMap<QString, QVector<int> >numOfAmbigsByBand, numOfSubAmbigsByBand, numOfPhsAmbigsByBand;
    QVector<int>                stub;
    QMap<QString, SgVector*>    grDelaysFullByBand, phDelaysFullByBand, phDelaysSigFullByBand;
//  bool                        hasPhaseAmbigs=false;
    bool                        hasPhaseAmbigs=isAttr(SgVlbiSessionInfo::Attr_FF_PHASE_DEL_USED);
    stub.clear();
    
    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey=bands_.at(iBand)->getKey();
      numOfAmbigsByBand   .insert(bandKey, QVector<int>(numOfObs));
      numOfSubAmbigsByBand.insert(bandKey, QVector<int>(numOfObs));
      numOfPhsAmbigsByBand.insert(bandKey, QVector<int>(numOfObs));
      grDelaysFullByBand[bandKey] = new SgVector(numOfObs);
      phDelaysFullByBand[bandKey] = new SgVector(numOfObs);
      phDelaysSigFullByBand[bandKey] = new SgVector(numOfObs);
      for (int i=0; i<numOfObs; i++)
      {
        numOfAmbigsByBand   [bandKey][i] = 0;
        numOfSubAmbigsByBand[bandKey][i] = 0;
        numOfPhsAmbigsByBand[bandKey][i] = 0;
      };
    };
    // fill them:
    for (int iObs=0; iObs<numOfObs; iObs++)
    {
      SgVlbiObservation        *obs=observations_.at(iObs);
      int                       mediaIdx=obs->getMediaIdx();
      for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
        it!=obs->observableByKey().end(); ++it)
      {
        SgVlbiObservable       *o=it.value();
//        numOfAmbigsByBand[o->getBandKey()][o->getMediaIdx()] = o->getNumOfAmbiguities();
        numOfAmbigsByBand   [o->getBandKey()][mediaIdx] = o->grDelay().getNumOfAmbiguities();
        numOfSubAmbigsByBand[o->getBandKey()][mediaIdx] = o->grDelay().getNumOfSubAmbigs();
        numOfPhsAmbigsByBand[o->getBandKey()][mediaIdx] = o->phDelay().getNumOfAmbiguities();
//      if (!hasPhaseAmbigs && o->phDelay().getNumOfAmbiguities()!=0)
//        hasPhaseAmbigs = true;
        grDelaysFullByBand[o->getBandKey()]->setElement(mediaIdx,
                                        o->grDelay().getValue() + o->grDelay().ambiguity());
        phDelaysFullByBand[o->getBandKey()]->setElement(mediaIdx,
                                        o->phDelay().getValue() + o->phDelay().ambiguity());
        phDelaysSigFullByBand[o->getBandKey()]->setElement(mediaIdx,
                                        o->phDelay().getSigma());
      };
    };
    // write data into netCDF files:
    for (int iBand=0; iBand<bands_.size(); iBand++)
    {
      const QString&            bandKey = bands_.at(iBand)->getKey();
      if (!true)
        isOk = isOk && vgosDb->storeObsNumGroupAmbigs(bandKey, numOfAmbigsByBand[bandKey], 
          numOfSubAmbigsByBand[bandKey]);
      else
        isOk = isOk && vgosDb->storeObsNumGroupAmbigs(bandKey, numOfAmbigsByBand[bandKey], stub);      
      isOk = isOk && vgosDb->storeObsGroupDelaysFull(bandKey, grDelaysFullByBand[bandKey]);
      if (hasPhaseAmbigs)
      {
        isOk = isOk && vgosDb->storeObsNumPhaseAmbigs(bandKey, numOfPhsAmbigsByBand[bandKey]);
        isOk = isOk && vgosDb->storeObsPhaseDelaysFull(bandKey, phDelaysFullByBand[bandKey], 
          phDelaysSigFullByBand[bandKey]);
      };
    };
    // free memmory:
    numOfAmbigsByBand   .clear();
    numOfSubAmbigsByBand.clear();
    numOfPhsAmbigsByBand.clear();
    for (QMap<QString, SgVector*>::iterator it=grDelaysFullByBand.begin();
      it!=grDelaysFullByBand.end(); ++it)
      delete it.value();
    grDelaysFullByBand.clear();
  };
  // end of numbers of ambiguities:

  //
  //
  // save solve control parameters:
  //
  if (parametersDescriptor_  && 
    getOriginType()!=OT_MK4  && 
    getOriginType()!=OT_KOMB  )
  {
    SgVector                   *vIntervals, *vConstraints;
    QList<QString>              sites, refSites;
    QList<int>                  ionoFlags;
    int                         numOfSites=stationsByName_.size();
    // mimic current SOLVE behavior:
    vIntervals = new SgVector(1);
    vConstraints = new SgVector(numOfSites);
    //
    // save troposhere parameters setup:
    int idx=0;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      sites << si->getKey();
      ionoFlags << 9; // what else?
      vConstraints->setElement(idx,
        (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH) ?
          si->getPcZenith().getPwlAPriori() : parametersDescriptor_->getZenith().getPwlAPriori())
            /(1.0e-12*24.0*vLight*100.0));
      idx++;
    };
    vIntervals->setElement(0, parametersDescriptor_->getZenith().getPwlStep()*24.0);
    isOk = isOk && vgosDb->storeAtmSetup(vIntervals, vConstraints, sites);
    //
    // save clock parameters setup:
    idx = 0;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      if (si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
        refSites << si->getKey();
      vConstraints->setElement(idx,
        (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS) ?
          si->getPcClocks().getPwlAPriori() : parametersDescriptor_->getClock0().getPwlAPriori())/864.0);
      idx++;
    };
    vIntervals->setElement(0, parametersDescriptor_->getClock0().getPwlStep()*24.0);
    isOk = isOk && vgosDb->storeClockSetup(vIntervals, vConstraints, sites, refSites);
    
    // just a placeholder for future:
    isOk = isOk && vgosDb->storeErpSetup(10.0, 0.67);
    //
    isOk = isOk && vgosDb->storeIonoSetup(ionoFlags, sites);
    //
    sites.clear();
    ionoFlags.clear();
    delete vConstraints;
    delete vIntervals;
  };
  //
  //
  // baseline weight corrections:
  if (isAttr(Attr_FF_WEIGHTS_CORRECTED))
  {
    QVector<QString>            baselineNames;
    SgMatrix                   *groupBlWeights;
    int                         numOfBaselines, idx;
    numOfBaselines = baselinesByName_.size();
    groupBlWeights = new SgMatrix(2, numOfBaselines);
    baselineNames.resize(numOfBaselines);
    idx = 0;
    for (BaselinesByName_it it=baselinesByName_.begin(); it!=baselinesByName_.end(); ++it)
    {
      SgVlbiBaselineInfo          *bi=it.value();
      baselineNames[idx] = bi->getKey();
      groupBlWeights->setElement(0, idx,  bi->getSigma2add(DT_DELAY));
      groupBlWeights->setElement(1, idx,  bi->getSigma2add(DT_RATE ));
      idx++;
    };
    isOk = isOk && vgosDb->storeSolveWeights(baselineNames, groupBlWeights);
    baselineNames.clear();
    delete groupBlWeights;
  };
  //
  //
  // source and baseline selection statuses:
  if (parametersDescriptor_    &&
      getOriginType()!=OT_MK4  &&
      getOriginType()!=OT_KOMB  )
  {
    int                         numOfStns, idx, jdx;
    QList<QString>              sources, stations;
    QVector<int>                sourcesSelection;
    QVector< QVector<int> >     baselinesSelection;
    sourcesSelection.resize(sourcesByName_.size());
    idx = 0;
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
    {
      SgVlbiSourceInfo           *si=it.value();
      sourcesSelection[idx] = si->calculateStatusBits();
      sources.append(si->getKey());
      idx++;
    };
    //
    numOfStns = stationsByName_.size();
    baselinesSelection.resize(stationsByName_.size());
    idx = 0;
    for (StationsByName_it it_i=stationsByName_.begin(); it_i!=stationsByName_.end(); ++it_i)
    {
      jdx = 0;
      baselinesSelection[idx].resize(numOfStns);
      for (StationsByName_it it_j=stationsByName_.begin(); it_j!=stationsByName_.end(); ++it_j)
      {
        QString                   st1Name(it_i.value()->getKey()), st2Name(it_j.value()->getKey());
        int                       bitArray;
        bitArray = 0;
        SgVlbiBaselineInfo       *bi, *biR;
        if (baselinesByName_.contains(st1Name + ":" + st2Name))
          bi = baselinesByName_.value(st1Name + ":" + st2Name);
        else
          bi = NULL;
        if (baselinesByName_.contains (st2Name + ":" + st1Name))
          biR = baselinesByName_.value(st2Name + ":" + st1Name);
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
        SgVlbiStationInfo       *s1i=NULL, *s2i=NULL;
        if (stationsByName_.contains(st1Name))
          s1i = stationsByName_.value(st1Name);
        if (stationsByName_.contains(st2Name))
          s2i = stationsByName_.value(st2Name);
        if ( ((bi  && !bi ->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)) ||
              (biR && !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)))  && 
             (s1i && !s1i->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))     && 
             (s2i && !s2i->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))  )
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
        baselinesSelection[idx][jdx] = bitArray;
        jdx++;
      };
      stations.append(it_i.value()->getKey());
      idx++;
    };
    isOk = isOk && vgosDb->storeSelectionStatus(sourcesSelection, baselinesSelection);
    sources.clear();
    stations.clear();
    sourcesSelection.clear();
    for (int i=0; i<baselinesSelection.size(); i++)
      baselinesSelection[i].clear();
    baselinesSelection.clear();
    //
    //
    QList<QString>              baselines;
    for (BaselinesByName_it it_b=baselinesByName_.begin(); it_b!=baselinesByName_.end(); ++it_b)
    {
      SgVlbiBaselineInfo *bi=it_b.value();
      if (bi->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) && 
         !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)         )
        baselines << bi->getKey();
    };
    isOk = isOk && vgosDb->storeBaselineClockSetup(baselines);
    baselines.clear();
  };
  //
  //
  if (config_ && 
      config_->getEccentricitiesFileName().size() && 
      getOriginType()!=OT_MK4  &&
      getOriginType()!=OT_KOMB  )
  {
    int                         numOfStns, idx;
    bool                        hasEccData;
    QVector<QString>            stationsNames, eccTypes, eccNums;
    SgMatrix                   *eccVals;
    
    numOfStns = stationsByName_.size();
    stationsNames.resize(numOfStns);
    eccTypes.resize(numOfStns);
    eccNums.resize(numOfStns);
    eccVals = new SgMatrix(numOfStns, 3);
    idx = 0;
    hasEccData = false;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo *si=it.value();
      if (si->eccRec())
      {
        if (!hasEccData && si->eccRec()->getEccType()!=SgEccRec::ET_N_A)
          hasEccData = true;
      
        stationsNames[idx] = si->getKey();
        eccTypes[idx] = si->eccRec()->getEccType()==SgEccRec::ET_XYZ ? "XY" : "NE";
        eccNums[idx] = si->eccRec()->getCdpNumber().leftJustified(10, ' ');
        eccVals->setElement(idx, 0, si->eccRec()->getDR().at(X_AXIS));
        eccVals->setElement(idx, 1, si->eccRec()->getDR().at(Y_AXIS));
        eccVals->setElement(idx, 2, si->eccRec()->getDR().at(Z_AXIS));
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF | SgLogger::DATA, className() +
          "::putDataIntoVgosDb(): no ecc for station " + si->getKey());
      idx++;
    };
    if (hasEccData)
      isOk = isOk && vgosDb->storeEccentricities(stationsNames, eccTypes, eccNums, eccVals, 
        config_->getEccentricitiesFileName());
    else
      logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): no eccentricities found");
      
    stationsNames.clear();
    eccTypes.clear();
    eccNums.clear();
    delete eccVals;
  };
  //
  //
  if (config_                  &&
      getOriginType()!=OT_MK4  &&
      getOriginType()!=OT_KOMB  )
  {
    int                         obsCalFlags;
    QList<QString>              calList;
    QVector<int>                statCalFlags;
    QVector<QString>            statCalNames, flybyNames, calSiteNames, obsCalNames;
    QVector< QVector<int> >     flybyFlags;
    
    statCalNames.resize(6);
    for (int i=0; i<6; i++)
      statCalNames[i] = sCalList[i];
    //
    flybyNames.resize(8);
    for (int i=0; i<8; i++)
      flybyNames[i] = sFclList[i];
    //
    calSiteNames.resize(stationsByName_.size());
    statCalFlags.resize(stationsByName_.size());
    flybyFlags.resize(stationsByName_.size());
    int                         idx=0;
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      calSiteNames[idx] = si->getKey();
      // currently, we use only cable cal from the list:
      statCalFlags[idx] = si->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)?0:1;
      flybyFlags[idx].resize(7);
      flybyFlags[idx][0] = 1<<(8-1);
      for (int j=1; j<7; j++)
        flybyFlags[idx][j] = 0;
      idx++;
    };
    //
    formObsCalibrationSetup(obsCalFlags, calList);
    obsCalNames.resize(calList.size());
    for (int i=0; i<calList.size(); i++)
      obsCalNames[i] = calList.at(i);
    //
    isOk = isOk && vgosDb->storeCalibrations(obsCalFlags, statCalFlags, flybyFlags, statCalNames, flybyNames, 
      calSiteNames, obsCalNames);
    //
    calList.clear();
    statCalFlags.clear();
    statCalNames.clear();
    flybyNames.clear();
    calSiteNames.clear();
    obsCalNames.clear();
    for (int i=0; i<flybyFlags.size(); i++)
      flybyFlags[i].clear();
    flybyFlags.clear();
  };
  //
  //
  // clock breaks: (should add FF_CLOCK_BREAK_ADDED to check?)
  if (  getOriginType()!=OT_MK4  && 
        getOriginType()!=OT_KOMB && 
       !isAttr(Attr_FF_AUX_OBS_MODIFIED) )
  {
    // another attempt:
    int                         numCbs;
    QVector<QString>            cbNames;
    QVector<SgMJD>              cbEpochs;
    QVector<int>                cbFlags;
    //
    QMap<QString, SgParameterBreak*>  
                                breakParameterByBreak;
    QMap<QString, QString>      breakNameByBreak;
    QList<SgParameterBreak*>    cBreaks;
    QList<QString>              cBreakNames, acmSites;
    QString                     sBreak("");
    QList<bool>                 acm_offsets, acm_rates;
    bool                        need2updateAprioriClocks=false;
    //
    // session-wide clock breaks:
    // collect the clock breaks:
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      if (si->clockBreaks().size())
      {
        si->calcCBEpochs4Export(*si->auxObservationByScanId());
        for (int j=0; j<si->clockBreaks().size(); j++)
        {
          SgParameterBreak     *pb=si->clockBreaks().at(j);
          sBreak = pb->getEpoch4Export().toString(SgMJD::F_INTERNAL) + "@" + si->getKey();
          breakParameterByBreak.insert(sBreak, pb);
          breakNameByBreak.insert(sBreak, si->getKey());
        };
      };
      if (si->isAttr(SgVlbiStationInfo::Attr_APRIORI_CLOCK_MODIFIED))
        need2updateAprioriClocks = true;
      if (si->getNeed2useAPrioriClocks())
      {
        acmSites    << si->getKey();
        acm_offsets << si->getAPrioriClockTerm_0();
        acm_rates   << si->getAPrioriClockTerm_1();
      };
    };
    cBreaks = breakParameterByBreak.values();
    cBreakNames = breakNameByBreak.values();
    if ((numCbs=cBreaks.size()))
    {
      contemporaryHistory_.addHistoryRecord("Detected " + QString().setNum(numCbs) +
        " clock break" + (numCbs==1?"":"s"), SgMJD::currentMJD().toUtc());
      cbNames.resize(numCbs);
      cbEpochs.resize(numCbs);
      cbFlags.resize(numCbs);
      for (int i=0; i<numCbs; i++)
      {
        cbNames[i]  = cBreakNames.at(i);
        cbEpochs[i] = cBreaks.at(i)->getEpoch4Export();
        cbFlags[i]  = 7;
        contemporaryHistory_.addHistoryRecord("A clock break marker has been set for " + 
          cBreakNames.at(i) + " at " + 
          cBreaks.at(i)->getEpoch4Export().toString(), SgMJD::currentMJD().toUtc());
      };
      //
      // check already saved info:
      bool                      need2store;
      bool                      hasClockBreaks;
      QVector<QString>          cbNames_old;
      QVector<SgMJD>            cbEpochs_old;
      QVector<int>              cbFlags_old;
      need2store = false;
      if ((hasClockBreaks=vgosDb->loadClockBreaks(cbNames_old, cbEpochs_old, cbFlags_old)))
      {
        if (cbNames_old != cbNames)
        {
          need2store = true;
          logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
            "::putDataIntoVgosDb(): the existed clock breaks names are different");
        };
        if (!need2store) // ok, the sizes and the station order too are the same
        {
          double                d;
          for (int i=0; i<cbEpochs.size(); i++)
            if (!need2store && (d=fabs(cbEpochs.at(i) - cbEpochs_old.at(i)))>1.0e-2/DAY2SEC)
            {
              need2store = true;
              logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
                "::putDataIntoVgosDb(): the existed clock breaks epochs are different: " +
                cbEpochs.at(i).toString() + " vs " + cbEpochs_old.at(i).toString() +
                " (diff: " + interval2Str(d) + ") for the station " + cbNames.at(i));
            };
        };
        if (!need2store && cbFlags_old != cbFlags)
        {
          need2store = true;
          logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
            "::putDataIntoVgosDb(): the existed clock breaks flags are different");
        };
        cbNames_old.clear();
        cbEpochs_old.clear();
        cbFlags_old.clear();
      }
      else
        need2store = true;
      //
      if (need2store)      
      {
        isOk = isOk && vgosDb->storeClockBreaks(cbNames, cbEpochs, cbFlags);
        logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
          "::putDataIntoVgosDb(): the existed clock breaks of the session were refereshed");
      }
      else
        logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
          "::putDataIntoVgosDb(): the session has clock breaks, the stored data were not modified");
      //
      cbNames.clear();
      cbEpochs.clear();
      cbFlags.clear();
    }
    else if (!vgosDb->vClockBreak().isEmpty()) // all previously defined clock breaks were removed:
    {
      vgosDb->vClockBreak().empty();
      logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): the vgosDb data tree was cleared from existed clock break(s)");
    }
    else
      logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): no clock break information were added to the vgosDb data tree");
    //
    cBreaks.clear();
    cBreakNames.clear();
    breakParameterByBreak.clear();
    breakNameByBreak.clear();
    //
    // check band's clock breaks, if they are exist, complain:
    for (QMap<QString, SgVlbiBand*>::iterator itb=bandByKey_.begin(); itb!=bandByKey_.end(); ++itb)
    {
      QString                   bandKey=itb.key();
      SgVlbiBand               *band=itb.value();
      for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
      {
        SgVlbiStationInfo      *si=it.value();
        if (si->clockBreaks().size())
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF | SgLogger::DATA, className() +
            "::putDataIntoVgosDb(): the station " + si->getKey() + " has " + 
            QString("").setNum(si->clockBreaks().size()) + " clock break(s) at the " + bandKey + 
            "-band. Saving the band dependent clock breaks is not implemented yet.");
      };
    };
    //
    // a priori clock model:
    if (need2updateAprioriClocks)
    {
      int                       numAcm=acmSites.size();
      if (numAcm)
      {
        SgMatrix               *offsets_n_rates=new SgMatrix(numAcm, 2);
        for (int i=0; i<numAcm; i++)
        {
          offsets_n_rates->setElement(i, 0,  acm_offsets.at(i));
          offsets_n_rates->setElement(i, 1,  acm_rates.at(i));
        };
        isOk = isOk && vgosDb->storeClockApriories(acmSites, offsets_n_rates);
        delete offsets_n_rates;
      }
      else // special case: a user could remove/turn off a priori clocks:
      {
        vgosDb->vClockApriori().empty();
        logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
          "::putDataIntoVgosDb(): a set of a priori clocks has been removed from vgosDb tree");
      };
    };





//----------------------------------


  /*
    QVector<QString>            cbNames;
    QVector<SgMJD>              cbEpochs;
    QVector<int>                cbFlags;
    int                         numCbs;
    QMap<QString, SgParameterBreak*>  
                                breakParameterByBreak;
    QMap<QString, QString>      breakNameByBreak;
    QList<SgParameterBreak*>    cBreaks;
    QList<QString>              cBreakNames;
    QString                     sBreak("");
    bool                        isClockBreaksModified;
    
    isClockBreaksModified = false;
    // session-wide clock breaks:
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      if (si->clockBreaks().size())
      {
        if (si->clockBreaks().getIsModified())
        {
          isClockBreaksModified = true;
          logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
            "::putDataIntoVgosDb(): the station " + si->getKey() + " has new clock break(s)");
        }
        else
          logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
            "::putDataIntoVgosDb(): the station " + si->getKey() +
            " has clock break(s) but they are not new");

        si->calcCBEpochs4Export(*si->auxObservationByScanId());
        for (int j=0; j<si->clockBreaks().size(); j++)
        {
          SgParameterBreak *pb=si->clockBreaks().at(j);
          sBreak = pb->getEpoch4Export().toString(SgMJD::F_INTERNAL) + "@" + si->getKey();
          breakParameterByBreak.insert(sBreak, pb);
          breakNameByBreak.insert(sBreak, si->getKey());
        };
      };
    };
    cBreaks = breakParameterByBreak.values();
    cBreakNames = breakNameByBreak.values();
    if ((numCbs=cBreaks.size()) && isClockBreaksModified)
    {
      contemporaryHistory_.addHistoryRecord("Detected " + QString().setNum(numCbs) +
                                            " clock break" + (numCbs==1?"":"s"));
      cbNames.resize(numCbs);
      cbEpochs.resize(numCbs);
      cbFlags.resize(numCbs);
      for (int i=0; i<numCbs; i++)
      {
        cbNames[i]  = cBreakNames.at(i);
        cbEpochs[i] = cBreaks.at(i)->getEpoch4Export();
        cbFlags[i]  = 7;
        contemporaryHistory_.addHistoryRecord("A clock break marker has been set for " + 
                                                cBreakNames.at(i) + " at " + 
                                                cBreaks.at(i)->getEpoch4Export().toString());
      };
      isOk = isOk && vgosDb->storeClockBreaks(cbNames, cbEpochs, cbFlags);
      cbNames.clear();
      cbEpochs.clear();
      cbFlags.clear();
    }
    else
      logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): no clock break information were added to the vgosDb data tree");
    
    cBreaks.clear();
    cBreakNames.clear();
    breakParameterByBreak.clear();
    breakNameByBreak.clear();
    //
    // band-specific clock breaks:
    for (QMap<QString, SgVlbiBand*>::iterator itb=bandByKey_.begin(); itb!=bandByKey_.end(); ++itb)
    {
      QString                   bandKey=itb.key();
      SgVlbiBand               *band=itb.value();
      for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
      {
        SgVlbiStationInfo        *si=it.value();
        if (si->clockBreaks().size())
        {
          si->calcCBEpochs4Export(*stationsByName_.value(si->getKey())->auxObservationByScanId());
          for (int j=0; j<si->clockBreaks().size(); j++)
          {
            SgParameterBreak *pb=si->clockBreaks().at(j);
            sBreak = pb->getEpoch4Export().toString(SgMJD::F_INTERNAL) + "@" + si->getKey();
            breakParameterByBreak.insert(sBreak, pb);
            breakNameByBreak.insert(sBreak, si->getKey());
          };
        };
      };
      cBreaks = breakParameterByBreak.values();
      cBreakNames = breakNameByBreak.values();
      if ((numCbs=cBreaks.size()))
      {
        cbNames.resize(numCbs);
        cbEpochs.resize(numCbs);
        cbFlags.resize(numCbs);
        for (int i=0; i<numCbs; i++)
        {
          cbNames [i] = cBreakNames.at(i);
          cbEpochs[i] = cBreaks.at(i)->getEpoch4Export();
          cbFlags [i] = 7;
        };
        isOk = isOk && vgosDb->storeClockBreaks(cbNames, cbEpochs, cbFlags, bandKey);
        cbNames.clear();
        cbEpochs.clear();
        cbFlags.clear();      
      };
      cBreaks.clear();
      cBreakNames.clear();
      breakParameterByBreak.clear();
      breakNameByBreak.clear();
    };
  */


  };
  //
  //
  //
  // at last, create new version of the wrapper file:
  vgosDb->composeWrapperFile();
  if (config_ && originType_ != OT_AGV)
    makeHistory(contemporaryHistory_);
  // collect everything interesting from the log:
  SgLogger                     *auxLogger=logger->lookupSupplementLog("History");
  if (auxLogger)
  {
    // vgosDbProcLogs-0.0.1 (Solstice): the sessio
    QRegExp                     reFltDriveName
      ("([\\w\\d]+)-([\\d\\.]+)\\s+\\(([\\w\\d\\s]+)\\):\\s+(.+)");
    QRegExp                     reFltNormMessg("([\\w\\d]+)::([\\w\\d]+)\\(\\):\\s+(.+)");
    QRegExp                     reFltWarnMessg("^Warning: ([\\w\\d]+)::([\\w\\d]+)\\(\\):\\s+(.+)");
    QRegExp                     reFltErrMessg("^ERROR: ([\\w\\d]+)::([\\w\\d]+)\\(\\):\\s+(.+)");
    QString                     sAttr(""), sSubj(""), sEvent("");
    const QList<QString*>      &histSpool=auxLogger->getSpool();
    for (int i=0; i<histSpool.size(); i++)
    {
      const QString            *pS=histSpool.at(i);
      if (reFltDriveName.indexIn(*pS) != -1)
      {
        sAttr = ".";
        sSubj = reFltDriveName.cap(1); 
        sEvent= reFltDriveName.cap(4);
      };
      if (reFltNormMessg.indexIn(*pS) != -1)
      {
        sAttr = ".";
        sSubj = reFltNormMessg.cap(2); 
        sEvent= reFltNormMessg.cap(3);
      };
      if (reFltWarnMessg.indexIn(*pS) != -1)
      {
        sAttr = "w";
        sSubj = reFltWarnMessg.cap(2); 
        sEvent= reFltWarnMessg.cap(3);
      };
      if (reFltErrMessg.indexIn(*pS) != -1)
      {
        sAttr = "E";
        sSubj = reFltErrMessg.cap(2); 
        sEvent= reFltErrMessg.cap(3);
      };
      if (sAttr.size() && sSubj.size() && sEvent.size())
        contemporaryHistory_.addHistoryRecord(sAttr + " " + sSubj + ": " + sEvent, 
          SgMJD::currentMJD().toUtc());
//        contemporaryHistory_.addHistoryRecord(sSubj + ": " + sEvent);
      else
        contemporaryHistory_.addHistoryRecord("  " + *pS, SgMJD::currentMJD().toUtc());
      sAttr = sSubj = sEvent = "";
    };
  };
  // save the history:
  isOk = isOk && vgosDb->saveLocalHistory(contemporaryHistory_);


  if (isOk)
  {
    if (vgosDb->getOperationMode() == SgNetCdf::OM_DRY_RUN)
      std::cout << "\nEnd of DRY RUN mode.\n";
    else
    {
      SgMJD                       finisEpoch(SgMJD::currentMJD());
      logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
        "::putDataIntoVgosDb(): session " + name_ + " has been updated"
        ", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
    };
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF | SgLogger::DATA, className() +
      "::putDataIntoVgosDb(): storing session " + name_ + " as a vgosDb database has failed");

  return isOk;
};







/*=====================================================================================================*/





/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/

/*=====================================================================================================*/
