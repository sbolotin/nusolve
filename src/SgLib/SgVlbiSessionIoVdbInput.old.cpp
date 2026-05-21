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


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgEccRec.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>



// to do: add ACM





/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/


/*=======================================================================================================
*
*                           I/O utilities:
* 
*======================================================================================================*/
//
bool SgVlbiSession::getDataFromVgosDb(SgVgosDb* vgosDb, bool have2LoadImmatureSession, bool guiExpected,
  const QStringList& masterfileSuffixes)
{
  SgMJD                         startEpoch(SgMJD::currentMJD());
  bool                          isOk(false);
  QString                       kind("");
  //
  const QString                 sMntTypes[]={"AZEL", "EQUA", "X_YN", "X_YE", "RICHMOND", "UNKN"};
  int                           numOfObs;
  int                           numOfUserCorrections;
  // 4 cleaning:
  QList<SgVector*>              vectors2del;
  QList<SgMatrix*>              matrices2del;
  //
  QList<SgMJD>                  epochs4Scans;
  QList<SgMJD>                  epochs4Obs;
  QList<QString>                stations_1, stations_2, sources;
  SgVector                     *calcDelays=NULL, *calcRates=NULL, *fractC=NULL;

  SgVector                     *dTec, *dTecStdDev;
  
  QMap<QString, SgMatrix*>      singleBandDelaysByBand, groupDelaysByBand, ratesByBand, phasesByBand, 
                                uvByBand, calFeedCorrByBand, effFreqsByBand, 
                                ionCalsByBand, ionSigmasByBand,
                                phaseCals_1ByBand, phaseCals_2ByBand;
  QMap<QString, SgMatrix*>      phaseCalFreqs_1ByBand, phaseCalAmps_1ByBand, 
                                phaseCalPhases_1ByBand, phaseCalOffsets_1ByBand,
                                phaseCalFreqs_2ByBand, phaseCalAmps_2ByBand, 
                                phaseCalPhases_2ByBand, phaseCalOffsets_2ByBand;

  QMap<QString, SgVector*>      ambigsByBand, refFreqsByBand, correlationByBand, snrByBand, 
                                phaseCalRates_1ByBand, phaseCalRates_2ByBand;
  QMap<QString, QVector<QString> >
                                qCodesByBand;
  QMap<QString, QVector<int> >  numAmbigsByBand, numSubAmbigsByBand, numPhsAmbigsByBand, 
                                ionDataFlagByBand, dataFlagByBand;
  //
  // correlator info stuff:
  QMap<QString, QVector<QString> >
                                fourfitOutputFNameByBand, fourfitErrorCodesByBand, 
                                baselineCodesByBand, 
                                tapeQualityCodesByBand, vlb1FileNamesByBand, tapeIds_1ByBand,
                                tapeIds_2ByBand;
  QMap<QString, QVector<int> >  startOffsetsByBand, stopOffsetsByBand,
                                hopsRevisionNumbersByBand;
  QMap<QString, QVector< QVector<int> > >
                                fourfitVersionsByBand, epochsOfCorrelationsByBand,
                                epochsOfCentralsByBand, epochsOfFourfitingsByBand, 
                                epochsOfNominalScanTimeByBand;
  QMap<QString, SgVector*>      deltaEpochsByBand, smplRateByBand, sbdResidsByBand, rateResidsByBand,
                                effDurationsByBand, startSecondsByBand, stopSecondsByBand, 
                                percentOfDiscardsByBand, uniformityFactorsByBand, 
                                geocenterPhaseResidsByBand, geocenterPhaseTotalsByBand,
                                geocenterSbDelaysByBand, geocenterGrDelaysByBand, 
                                geocenterDelayRatesByBand, probOfFalsesByBand, corrAprioriDelaysByBand,
                                corrAprioriRatesByBand, corrAprioriAccsByBand, incohAmpsByBand,
                                incohAmps2ByBand, delayResidsByBand;
  QMap<QString, SgMatrix*>      urvrsByBand, instrDelaysByBand, searchParamsByBand, corrClocksByBand,
                                mDlysByBand, mAuxByBand, starElevByBand, zenithDelaysByBand;
  // eocis
  //
  QMap<QString, QVector<QString> >
                                fourfitControlFileByBand, fourfitCommandByBand;
  QMap<QString, QVector<int> >  numLagsByBand;
  QMap<QString, QVector<double> >
                                apLengthByBand;

  // channel infos:
  QMap<QString, QVector<QString> >
                                channelIdsByBand, polarizationsByBand;
  QMap<QString, QVector<int> >  numOfChannelsByBand, bitsPerSamplesByBand;

  QMap<QString, QVector< QVector<int> > >
                                errorRates_1ByBand, errorRates_2ByBand, bbcIdxs_1ByBand, bbcIdxs_2ByBand,
                                corelIdxNumbersUsbByBand, corelIdxNumbersLsbByBand;
  QMap<QString, SgVector* >     sampleRateByBand;
  QMap<QString, SgMatrix* >     residFringeAmplByChanByBand, residFringePhseByChanByBand,
                                refFreqByChanByBand,
                                numOfSamplesByChan_USBByBand, numOfSamplesByChan_LSBByBand,
                                numOfAccPeriodsByChan_USBByBand, numOfAccPeriodsByChan_LSBByBand,
                                loFreqs_1ByBand, loFreqs_2ByBand;
  // eoci
  
  QList<QString>                listOfBands;
  
  cppsSoft_ = CPPS_UNKNOWN;

  dTec = dTecStdDev = NULL;

  setName(vgosDb->getSessionName());
  setSessionCode(vgosDb->getSessionCode());
  setInputDriver(vgosDb);

  //
  networkSuffix_ = name_.mid(8, 1);   // need to make proper name handling..

  listOfBands = vgosDb->getListOfBands();

  // fix it later:
  if (vgosDb->hasBand("X"))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): found data for X-band");
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): no data for X-band have been found: " + name_);
  if (vgosDb->hasBand("S"))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): found data for S-band");
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): no data for S-band have been found: " + name_);
  //
  for (int i=0; i<listOfBands.size(); i++)
  {
    const QString              &bandKey=listOfBands.at(i);
    singleBandDelaysByBand          .insert(bandKey, NULL);
    groupDelaysByBand               .insert(bandKey, NULL);
    ratesByBand                     .insert(bandKey, NULL);
    phasesByBand                    .insert(bandKey, NULL);
    ionCalsByBand                   .insert(bandKey, NULL);
    ionSigmasByBand                 .insert(bandKey, NULL);
    phaseCals_1ByBand               .insert(bandKey, NULL);
    phaseCals_2ByBand               .insert(bandKey, NULL);
    phaseCalFreqs_1ByBand           .insert(bandKey, NULL);
    phaseCalFreqs_2ByBand           .insert(bandKey, NULL);
    phaseCalAmps_1ByBand            .insert(bandKey, NULL);
    phaseCalAmps_2ByBand            .insert(bandKey, NULL);
    phaseCalPhases_1ByBand          .insert(bandKey, NULL);
    phaseCalPhases_2ByBand          .insert(bandKey, NULL);
    phaseCalOffsets_1ByBand         .insert(bandKey, NULL);
    phaseCalOffsets_2ByBand         .insert(bandKey, NULL);
    phaseCalRates_1ByBand           .insert(bandKey, NULL);
    phaseCalRates_2ByBand           .insert(bandKey, NULL);

    effFreqsByBand                  .insert(bandKey, NULL);
    refFreqsByBand                  .insert(bandKey, NULL);
    correlationByBand               .insert(bandKey, NULL);
    uvByBand                        .insert(bandKey, NULL);
    calFeedCorrByBand               .insert(bandKey, NULL);
    snrByBand                       .insert(bandKey, NULL);
    ambigsByBand                    .insert(bandKey, NULL);

    qCodesByBand                    .insert(bandKey, QVector<QString>());
    // correlator info stuff:
    fourfitOutputFNameByBand        .insert(bandKey, QVector<QString>());
    fourfitErrorCodesByBand         .insert(bandKey, QVector<QString>());
    baselineCodesByBand             .insert(bandKey, QVector<QString>());
    tapeQualityCodesByBand          .insert(bandKey, QVector<QString>());
    vlb1FileNamesByBand             .insert(bandKey, QVector<QString>());
    tapeIds_1ByBand                 .insert(bandKey, QVector<QString>());
    tapeIds_2ByBand                 .insert(bandKey, QVector<QString>());
//  corelVersionsByBand             .insert(bandKey, QVector<int>());
    startOffsetsByBand              .insert(bandKey, QVector<int>());
    stopOffsetsByBand               .insert(bandKey, QVector<int>());
    hopsRevisionNumbersByBand       .insert(bandKey, QVector<int>());
    fourfitVersionsByBand           .insert(bandKey, QVector< QVector<int> >());
//  dbeditVersionByBand             .insert(bandKey, QVector< QVector<int> >());
    epochsOfCorrelationsByBand      .insert(bandKey, QVector< QVector<int> >());
    epochsOfCentralsByBand          .insert(bandKey, QVector< QVector<int> >());
    epochsOfFourfitingsByBand       .insert(bandKey, QVector< QVector<int> >());
    epochsOfNominalScanTimeByBand   .insert(bandKey, QVector< QVector<int> >());
    deltaEpochsByBand               .insert(bandKey, NULL);
    smplRateByBand                  .insert(bandKey, NULL);
    sbdResidsByBand                 .insert(bandKey, NULL);
    rateResidsByBand                .insert(bandKey, NULL);
    effDurationsByBand              .insert(bandKey, NULL);
    startSecondsByBand              .insert(bandKey, NULL);
    stopSecondsByBand               .insert(bandKey, NULL);
    percentOfDiscardsByBand         .insert(bandKey, NULL);
    uniformityFactorsByBand         .insert(bandKey, NULL);
    geocenterPhaseResidsByBand      .insert(bandKey, NULL);
    geocenterPhaseTotalsByBand      .insert(bandKey, NULL);
    geocenterSbDelaysByBand         .insert(bandKey, NULL);
    geocenterGrDelaysByBand         .insert(bandKey, NULL);
    geocenterDelayRatesByBand       .insert(bandKey, NULL);
    probOfFalsesByBand              .insert(bandKey, NULL);
    corrAprioriDelaysByBand         .insert(bandKey, NULL);
    corrAprioriRatesByBand          .insert(bandKey, NULL);
    corrAprioriAccsByBand           .insert(bandKey, NULL);
    incohAmpsByBand                 .insert(bandKey, NULL);
    incohAmps2ByBand                .insert(bandKey, NULL);
    delayResidsByBand               .insert(bandKey, NULL);
    urvrsByBand                     .insert(bandKey, NULL);
    instrDelaysByBand               .insert(bandKey, NULL);
    starElevByBand                  .insert(bandKey, NULL);
    zenithDelaysByBand              .insert(bandKey, NULL);
    searchParamsByBand              .insert(bandKey, NULL);
    corrClocksByBand                .insert(bandKey, NULL);
    mDlysByBand                     .insert(bandKey, NULL);
    mAuxByBand                      .insert(bandKey, NULL);
    // EoCIS
    
    fourfitControlFileByBand        .insert(bandKey, QVector<QString>());
    fourfitCommandByBand            .insert(bandKey, QVector<QString>());
    numLagsByBand                   .insert(bandKey, QVector<int>());
    apLengthByBand                  .insert(bandKey, QVector<double>());
    
    numAmbigsByBand                 .insert(bandKey, QVector<int>());
    numSubAmbigsByBand              .insert(bandKey, QVector<int>());
    numPhsAmbigsByBand              .insert(bandKey, QVector<int>());
    dataFlagByBand                  .insert(bandKey, QVector<int>());
    ionDataFlagByBand               .insert(bandKey, QVector<int>());
    
    // channel infos:
    channelIdsByBand                .insert(bandKey, QVector<QString>());
    polarizationsByBand             .insert(bandKey, QVector<QString>());
    numOfChannelsByBand             .insert(bandKey, QVector<int>());
    bitsPerSamplesByBand            .insert(bandKey, QVector<int>());

    errorRates_1ByBand              .insert(bandKey, QVector< QVector<int> >());
    errorRates_2ByBand              .insert(bandKey, QVector< QVector<int> >());
    bbcIdxs_1ByBand                 .insert(bandKey, QVector< QVector<int> >());
    bbcIdxs_2ByBand                 .insert(bandKey, QVector< QVector<int> >());
    corelIdxNumbersUsbByBand        .insert(bandKey, QVector< QVector<int> >());
    corelIdxNumbersLsbByBand        .insert(bandKey, QVector< QVector<int> >());
                                                                
    sampleRateByBand                .insert(bandKey, NULL);
    residFringeAmplByChanByBand     .insert(bandKey, NULL);
    residFringePhseByChanByBand     .insert(bandKey, NULL);
    refFreqByChanByBand             .insert(bandKey, NULL);
    numOfSamplesByChan_USBByBand    .insert(bandKey, NULL);
    numOfSamplesByChan_LSBByBand    .insert(bandKey, NULL);
    numOfAccPeriodsByChan_USBByBand .insert(bandKey, NULL);
    numOfAccPeriodsByChan_LSBByBand .insert(bandKey, NULL);
    loFreqs_1ByBand                 .insert(bandKey, NULL);
    loFreqs_2ByBand                 .insert(bandKey, NULL);
    
  };
  
  if (!vgosDb->loadEpochs4Scans(epochs4Scans))
    return false;
  if (!vgosDb->loadEpochs4Obs(epochs4Obs))
    return false;
  
  numOfObs = epochs4Obs.size();
  
  // get general info:
  if (!vgosDb->loadObsObjectNames(stations_1, stations_2, sources))
    return false;
  if (!vgosDb->loadObsTheoreticals(calcDelays, calcRates))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): cannot find precalculated delays and rates");
    delAttr(Attr_HAS_CALC_DATA);
  }
  else
    addAttr(Attr_HAS_CALC_DATA);

  //
  if (!vgosDb->loadObsFractC(fractC))
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): cannot find fractC");
  };
  //

  vectors2del << calcDelays << calcRates << fractC;

  isOk = true;

  //
  //
  //
  // deal with Header:
  QString                       correlatorType(""), correlatorName(""), piName(""), experimentName(""),
                                experimentDescription(""), recordingMode("");
  QList<QString>                declStations, declSources;
  SgMJD                         tStart, tFinis;
  int                           sftIdx(-1), experimentSerialNumber(-1);
  if (!vgosDb->loadSessionHead(correlatorType, correlatorName, piName, experimentName,
    experimentSerialNumber, experimentDescription, recordingMode, declStations, declSources, 
    tStart, tFinis, sftIdx))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): cannot use the data because of missed info: Head");
    isOk = false;
  };
  setCorrelatorType(correlatorType);
  setCorrelatorName(correlatorName);
  setPiAgencyName(piName);
  setDescription(experimentDescription);
  if (0 < experimentSerialNumber)
    setExperimentSerialNumber(experimentSerialNumber);
  setRecordingMode(recordingMode);
  declStations.clear();
  declSources.clear();
  if (0 <= sftIdx)
    cppsSoft_ = (CorrelatorPostProcSoftware)sftIdx;
  // end of Header


  // "Obs" dir:
  for (int i=0; i<listOfBands.size(); i++)
  {
    const QString              &bandKey=listOfBands.at(i);
    if (!vgosDb->loadObsSingleBandDelays(bandKey, singleBandDelaysByBand[bandKey]))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): the data are hardly usable because of missed info: SB Delays for " + 
        bandKey + "-band");
      /*
      if (vgosDb->getCorrelatorType() != SgVgosDb::CT_Unknown)
        isOk = false;
      */
    };
    if (!vgosDb->loadObsGroupDelays(bandKey, groupDelaysByBand[bandKey]))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Group Delays for " + 
        bandKey + "-band");
      isOk = false;
    };
    if (!vgosDb->loadObsRates(bandKey, ratesByBand[bandKey]))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Rates for " + 
        bandKey + "-band");
      // isOk = false;
    };
    vgosDb->loadObsPhase(bandKey, phasesByBand[bandKey]);
    if (!vgosDb->loadObsRefFreqs(bandKey, refFreqsByBand[bandKey]))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Ref Freqs for " + 
        bandKey + "-band");
      isOk = false;
    };
    vgosDb->loadObsEffFreqs(bandKey, effFreqsByBand[bandKey]);

    if (!vgosDb->loadObsAmbigSpacing(bandKey, ambigsByBand[bandKey]))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Ambig Spacing for " + 
        bandKey + "-band");
      // isOk = false;
    };
    if (!vgosDb->loadObsQualityCodes(bandKey, qCodesByBand[bandKey]))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Quality Codes for " + 
        bandKey + "-band");
      // isOk = false;
    };
    if (!vgosDb->loadObsCorrelation(bandKey, correlationByBand[bandKey]) && !have2LoadImmatureSession)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: Correlation Coeffs for " + 
        bandKey + "-band");
    };
    if (!vgosDb->loadObsUVFperAsec(bandKey, uvByBand[bandKey]))
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: UV for " + 
        bandKey + "-band");
    };
    if (!vgosDb->loadObsCalFeedCorr(bandKey, calFeedCorrByBand[bandKey]))
    {
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: FeedCorr for " + 
        bandKey + "-band");
    }
    else
      hasFeedCorrContrib_= true;
    if (!vgosDb->loadObsSNR(bandKey, snrByBand[bandKey]) && !have2LoadImmatureSession)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): cannot use data because of missed info: SNR for " + 
        bandKey + "-band");
        // isOk = false;
    };

    if (!vgosDb->loadObsCorrelatorInfo(bandKey,
          fourfitOutputFNameByBand[bandKey],
          fourfitErrorCodesByBand[bandKey],
//        corelVersionsByBand[bandKey],
          startOffsetsByBand[bandKey],
          fourfitVersionsByBand[bandKey],
//        dbeditVersionByBand[bandKey],
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
          smplRateByBand[bandKey],
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
          vlb1FileNamesByBand[bandKey],
          tapeIds_1ByBand[bandKey],
          tapeIds_2ByBand[bandKey],
          epochsOfCorrelationsByBand[bandKey],
          epochsOfCentralsByBand[bandKey],
          epochsOfFourfitingsByBand[bandKey],
          epochsOfNominalScanTimeByBand[bandKey],
          corrClocksByBand[bandKey],
          mDlysByBand[bandKey],
          mAuxByBand[bandKey]))
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): reading Correlator Info for the " + bandKey + "-band has failed");


    if (!vgosDb->loadMiscFourFit(bandKey,
      fourfitControlFileByBand[bandKey], fourfitCommandByBand[bandKey],
        numLagsByBand[bandKey], apLengthByBand[bandKey]))
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): reading Fourfit Info for the " + bandKey + "-band has failed");
      
    vgosDb->loadObsNumGroupAmbigs(bandKey, numAmbigsByBand[bandKey], numSubAmbigsByBand[bandKey]);
    vgosDb->loadObsNumPhaseAmbigs(bandKey, numPhsAmbigsByBand[bandKey]);
    vgosDb->loadObsDataFlag(bandKey, dataFlagByBand[bandKey]);
    vgosDb->loadObsCalIonGroup(bandKey, ionCalsByBand[bandKey], ionSigmasByBand[bandKey], 
                                ionDataFlagByBand[bandKey]);

    //
    vgosDb->loadObsCalUnphase(bandKey, phaseCals_1ByBand[bandKey], phaseCals_2ByBand[bandKey]);
    //
    // channels setup:
    if (!vgosDb->loadObsChannelInfo(bandKey,
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
      residFringeAmplByChanByBand[bandKey],
      residFringePhseByChanByBand[bandKey],
      refFreqByChanByBand[bandKey],
      numOfSamplesByChan_USBByBand[bandKey],
      numOfSamplesByChan_LSBByBand[bandKey],
      numOfAccPeriodsByChan_USBByBand[bandKey],
      numOfAccPeriodsByChan_LSBByBand[bandKey],
      loFreqs_1ByBand[bandKey],
      loFreqs_2ByBand[bandKey]))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): missed info: channel info for " + bandKey + "-band");
    };
    //
    // phase cal data:
    if (!vgosDb->loadObsPhaseCalInfo(bandKey,
        phaseCalFreqs_1ByBand[bandKey], phaseCalAmps_1ByBand[bandKey], 
        phaseCalPhases_1ByBand[bandKey], phaseCalOffsets_1ByBand[bandKey], 
        phaseCalRates_1ByBand[bandKey],
        phaseCalFreqs_2ByBand[bandKey], phaseCalAmps_2ByBand[bandKey],
        phaseCalPhases_2ByBand[bandKey], phaseCalOffsets_2ByBand[bandKey],
        phaseCalRates_2ByBand[bandKey]))
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): missed info: phase calibration data for " + bandKey + "-band");
  };

  // VGOS:
  if (!vgosDb->loadObsDtec(dTec, dTecStdDev))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): no dTec data found"); // it is ok
  };

  // "Cals" dir:
  SgMatrix                     *calEarthTide, *calEopHiFiUt;
  SgMatrix                     *calPlxRev;
  SgMatrix                     *calBend, *calBendSun, *calBendSunHigher;
  SgMatrix                     *calEopHiFiXY, *calOcean, *calOceanOld;
  SgMatrix                     *calOceanPoleTideLd;
  SgMatrix                     *calPoleTide, *calPoleTideOldRestore, *calTiltRmvr;
  SgMatrix                     *calWobbleX, *calWobbleY;
  SgMatrix                     *calWobNut, *calHiFreqLibrationPm, *calHiFreqLibrationUt;
  SgMatrix                     *valNutNro, *valNutNro_rates;
  SgMatrix                     *valNutEqx, *valNutEqx_rates;
  SgMatrix                     *valNutEqxWahr, *valNutEqxWahr_rates;
  SgMatrix                     *rSun, *rMoon, *rEarth, *vSun, *vMoon, *vEarth, *aEarth;
  SgMatrix                     *t2c_val, *t2c_rat, *t2c_acc, *pmXy;
  SgVector                     *ut1_tai;

  calEarthTide = calEopHiFiUt = calEopHiFiXY = calOcean = calOceanOld = calPlxRev = calBend
    = calBendSun = calBendSunHigher = calPoleTide = calPoleTideOldRestore = calTiltRmvr
    = calWobNut = calWobbleX = calWobbleY = calOceanPoleTideLd
    = calHiFreqLibrationPm = calHiFreqLibrationUt = valNutNro = valNutNro_rates
    = valNutEqxWahr = valNutEqxWahr_rates = valNutEqx = valNutEqx_rates
    = rSun = rMoon = rEarth = vSun = vMoon = vEarth = aEarth
    = t2c_val = t2c_rat = t2c_acc = pmXy
    = NULL;
  ut1_tai = NULL;

  if (!vgosDb->loadObsCalEarthTide(calEarthTide, kind))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Earth Tides");
  }
  else if (!kind.isEmpty() && kind.toUpper() != "IERS2003")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of Earth tide calibration");
  //
  if (!vgosDb->loadObsCalHiFyErp(calEopHiFiUt, calEopHiFiXY, kind))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: HiFq EOP");
  }
  else if (!kind.isEmpty() && kind.toUpper() != "IERS2003")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of high frequency ERP calibration");
  //
  if (!vgosDb->loadObsCalOcean(calOcean))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Ocean Load");
  };
  vgosDb->loadObsCalOceanOld(calOceanOld);
  //
  vgosDb->loadObsCalOceanPoleTideLoad(calOceanPoleTideLd);

  if (!vgosDb->loadObsCalPoleTide(calPoleTide, kind))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Pole Tide");
  }
  else if (!kind.isEmpty() && kind.toUpper() != "IERS2003")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of pole tide calibration");
  vgosDb->loadObsCalPoleTideOldRestore(calPoleTideOldRestore);

  if (!vgosDb->loadObsCalTiltRmvr(calTiltRmvr))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: TiltRemover");
  };
  //
  if (!vgosDb->loadObsCalHiFyLibration(calHiFreqLibrationPm, calHiFreqLibrationUt, kind))
  {
    if (!vgosDb->loadObsCalHiFyLibration(calWobNut, kind))
    {
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): limited use data, missed info: Libration");
    };
  }
  else if (!kind.isEmpty() && kind.toUpper() != "IERS2010")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of high frequency libration "
      "calibration");
  //
  if (!vgosDb->loadObsCalWobble(calWobbleX, calWobbleY))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Pole Wobble");
  };
  if (!vgosDb->loadObsCalParallax(calPlxRev))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Parallax");
  };
  if (!vgosDb->loadObsCalBend(calBend))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Bending");
  };
  if (!vgosDb->loadObsCalBendSun(calBendSun))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Bending II");
  };
  if (!vgosDb->loadObsCalBendSunHigher(calBendSunHigher))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Bending III");
  };
  
  QVector<int>                  delUFlags, phsUFlags, ratUFlags, uAcSups, 
                                ionsBits, obs2Scan;
  QVector<QString>              scanNames, scanFullNames;
  QVector<QString>              corrRootFnames;
  SgMatrix                     *dV_dPx, *dV_dPy, *dV_dUT1, *dV_dCipX, *dV_dCipY, *dV_dRA, *dV_dDN;
  SgMatrix                     *dTau_dR_1,*dRat_dR_1;
  SgMatrix                     *dV_dBend, *dV_dGamma, *dV_dParallax, *dV_dPtdX, *dV_dPtdY;
  SgMatrix                     *userCorrections;
  
  dV_dRA = dV_dDN = dV_dCipX = dV_dCipY = dV_dPx = dV_dPy = dV_dUT1 = dTau_dR_1 
         = dRat_dR_1 = dV_dBend = dV_dGamma = dV_dParallax = dV_dPtdX = dV_dPtdY = NULL;
  userCorrections = NULL;
  numOfUserCorrections  = vgosDb->getNumOfUserCorrections();
  if (numOfUserCorrections)
  {
    QString                     str("");
    userCorrections = new SgMatrix(numOfObs, numOfUserCorrections);
    userCorrectionsUse_.resize(numOfUserCorrections);
    userCorrectionsName_.resize(numOfUserCorrections);
    for (int i=0; i<vgosDb->getNumOfUserCorrections(); i++)
    {
      vgosDb->loadObsUserCorrections(i, userCorrections, str);
      userCorrectionsUse_[i] = false; // true;
      userCorrectionsName_[i] = str;
    };
  };
  //
  //
  // edit's dir:
  vgosDb->loadObsEditData(delUFlags, phsUFlags, ratUFlags, uAcSups);

  // partial's dir:
  if (!vgosDb->loadObsPartEOP(dV_dPx, dV_dPy, dV_dUT1))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: EOP");

  if (!vgosDb->loadObsPartBend(dV_dBend))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: bend");

  if (!vgosDb->loadObsPartGamma(dV_dGamma))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: gamma");

  if (!vgosDb->loadObsPartParallax(dV_dParallax))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: parallax");

  if (!vgosDb->loadObsPartPoleTides(dV_dPtdX, dV_dPtdY, kind))
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: pole tide");
  else if (!kind.isEmpty() && kind.toUpper() != "IERS2006")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of pole tide partials");

  //
  if (!vgosDb->loadObsPartNut2KXY(dV_dCipX, dV_dCipY, kind))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: CIP/Nutation");
//    isOk = false;
  }
  else if (!kind.isEmpty() && kind.toUpper() != "IAU2006")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of Nutation partials");
  //
  if (!vgosDb->loadObsPartRaDec(dV_dRA, dV_dDN))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: source position");
//    isOk = false;
  };
  if (!vgosDb->loadObsPartXYZ(dTau_dR_1, dRat_dR_1))  //, here dTau_dR_2 = -dTau_dR_1
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: Partials: station coordinates");
//    isOk = false;
  };
  //
  // observation-to-scan mapping:
  if (!vgosDb->loadObsCrossRefs(obs2Scan))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): cannot use data because of missed info: CrossRefers");
    isOk = false;
  };
  //
  // scans:
  if (!vgosDb->loadScanName(scanNames, scanFullNames))
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): missed info: Scan Names");
  
  if (!vgosDb->loadScanCrootFname(corrRootFnames))
    logger->write(
      (vgosDb->getCorrelatorType()==SgVgosDb::CT_GSI||vgosDb->getCorrelatorType()==SgVgosDb::CT_CRL?
        SgLogger::DBG : SgLogger::WRN), SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): missed info: correlator root file names");
  //
  vgosDb->loadScanNutationEqxWahr(valNutEqxWahr, valNutEqxWahr_rates);
  vgosDb->loadScanNutationEqx(valNutEqx, valNutEqx_rates, kind);
  if (!kind.isEmpty() && kind.toUpper() != "IAU2006")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of Nutation contribution, "
      "Equinox mode");
  vgosDb->loadScanNutationNro(valNutNro, valNutNro_rates, kind);
  if (!kind.isEmpty() && kind.toUpper() != "IAU2006")
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of Nutation contribution, "
      "NRO mode");

  vgosDb->loadScanEphemeris(rSun, rMoon, rEarth, vSun, vMoon, vEarth, aEarth);
  vgosDb->loadScanTrf2crf(t2c_val, t2c_rat, t2c_acc);
  vgosDb->loadErpApriori(ut1_tai, pmXy);

  // collect pointers on vectors:
  vectors2del << dTec << dTecStdDev << ut1_tai
              ;

  // collect pointers on matrices:
  matrices2del  << calEarthTide << calEopHiFiUt << calEopHiFiXY << calOcean << calOceanOld
                << calPlxRev << calBend 
                << calBendSun << calBendSunHigher << calPoleTide << calPoleTideOldRestore
                << calTiltRmvr << calWobNut 
                << valNutEqxWahr << valNutEqxWahr_rates
                << valNutNro << valNutNro_rates
                << valNutEqx << valNutEqx_rates
                << rSun << rMoon << rEarth << vSun << vMoon << vEarth << aEarth
                << t2c_val << t2c_rat << t2c_acc << pmXy
                << calWobbleX << calWobbleY
                << calOceanPoleTideLd << calHiFreqLibrationPm << calHiFreqLibrationUt
                << dTau_dR_1 << dRat_dR_1
                << dV_dPx << dV_dPy << dV_dUT1 << dV_dCipX << dV_dCipY << dV_dRA << dV_dDN
                << dV_dBend << dV_dGamma << dV_dParallax << dV_dPtdX << dV_dPtdY
                ;
  if (userCorrections)
    matrices2del << userCorrections;
  
  // phase 1:
  //
  // parse load info:
  QString                               station1Name, station2Name, sourceName, baselineName;
  QString                               obsKey, scanName, scanId;
  SgVlbiStationInfo                    *station1Info, *station2Info;
  SgVlbiSourceInfo                     *sourceInfo;
  SgVlbiBaselineInfo                   *baselineInfo;
  SgVlbiObservation                    *obs=NULL;
  SgMJD                                 epoch;
  QMap<QString, SgVlbiAuxObservation*> *auxObsByScan=NULL;
  int                                   obs2ScanIdx;
  //
  // first, read observation, set up identities
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
    "::getDataFromVgosDb(): processing observations of the session " + vgosDb->getSessionName());
  for (int obsIdx=0; obsIdx<numOfObs; obsIdx++)
  {
    obs = NULL;
    // stations and source names:
    station1Name = stations_1.at(obsIdx);
    station2Name = stations_2.at(obsIdx);
    sourceName   = sources.at(obsIdx);
    baselineName = station1Name + ":" + station2Name;
    epoch = epochs4Obs.at(obsIdx);
    obsKey.sprintf("%s", 
      qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));
    // scanId and scanName:
    scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
    obs2ScanIdx = obs2Scan.at(obsIdx) - 1;
    if (scanNames.size())
      scanName = scanNames.at(obs2ScanIdx);
    else if (scanFullNames.size())
      scanName = scanFullNames.at(obs2ScanIdx);
    else
      scanName.sprintf("%03d-%02d:%02d:%04.1f@%s",
        epoch.calcDayOfYear(), epoch.calcHour(), epoch.calcMin(), epoch.calcSec(), 
        qPrintable(sourceName));
    //
    if (observationByKey_.contains(obsKey))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): the observation #" + QString("").setNum(obsIdx) + " at " + 
        epoch.toString() + " with a key \'" + obsKey + "\' already exists in the container");
      obs = observationByKey_.value(obsKey); // just to prevent the segfault
    }
    else
    {
      obs = new SgVlbiObservation(this);
      obs->setMJD(epoch);
      obs->setScanName(scanName);
      obs->setScanId(scanId);
      obs->setKey(obsKey);
      obs->setMediaIdx(obsIdx);
      if (scanFullNames.size() && scanFullNames.at(obs2ScanIdx).size())
        obs->setScanFullName(scanFullNames.at(obs2ScanIdx));
      if (corrRootFnames.size() && corrRootFnames.at(obs2ScanIdx).size())
        obs->setCorrRootFileName(corrRootFnames.at(obs2ScanIdx));
      observations_.append(obs);
      observationByKey_.insert(obsKey, obs);
    };
    //
    // station #1:
    if (stationsByName_.contains(station1Name))
      station1Info = stationsByName_.value(station1Name);
    else // new station, add it to the container and register its index:
    {
      station1Info = new SgVlbiStationInfo(stationsByName_.size(), station1Name);
      stationsByName_.insert(station1Info->getKey(), station1Info);
      stationsByIdx_.insert(station1Info->getIdx(), station1Info);
    };
    // station #2:
    if (stationsByName_.contains(station2Name))
      station2Info = stationsByName_.value(station2Name);
    else // new station, add it to the container and register its index:
    {
      station2Info = new SgVlbiStationInfo(stationsByName_.size(), station2Name);
      stationsByName_.insert(station2Info->getKey(), station2Info);
      stationsByIdx_.insert(station2Info->getIdx(), station2Info);
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
    obs->setStation1Idx(station1Info->getIdx());
    obs->setStation2Idx(station2Info->getIdx()); 
    obs->setSourceIdx  (sourceInfo  ->getIdx());
    obs->setBaselineIdx(baselineInfo->getIdx());
    //
    // collect data:
    // VGOS:
    if (dTec && dTecStdDev)
    {
      obs->setDTec      (dTec       ->getElement(obsIdx));
      obs->setDTecStdDev(dTecStdDev ->getElement(obsIdx));
    };
    // theo consensus delay:
    if (calcDelays)
      obs->setCalcConsensusDelay  (calcDelays->getElement(obsIdx));
    if (calcRates)
      obs->setCalcConsensusRate   (calcRates->getElement(obsIdx));
    if (fractC)
      obs->setFractC(fractC->getElement(obsIdx));

    if (calPlxRev)
    {
      obs->setDdel_dParallaxRev(calPlxRev->getElement(obsIdx, 0));
      obs->setDrat_dParallaxRev(calPlxRev->getElement(obsIdx, 1));
    };
    if (calBend)
    {
      obs->setCalcConsBendingDelay(calBend->getElement(obsIdx, 0));
      obs->setCalcConsBendingRate (calBend->getElement(obsIdx, 1));
    };
    if (calBendSun)
    {
      obs->setCalcConsBendingSunDelay(calBendSun->getElement(obsIdx, 0));
      obs->setCalcConsBendingSunRate (calBendSun->getElement(obsIdx, 1));
    };
    if (calBendSunHigher)
    {
      obs->setCalcConsBendingSunHigherDelay(calBendSunHigher->getElement(obsIdx, 0));
      obs->setCalcConsBendingSunHigherRate (calBendSunHigher->getElement(obsIdx, 1));
    };
    //
    // contributions:
    if (calOcean)
    {
      obs->setCalcOceanTideDelay(calOcean->getElement(obsIdx, 0));
      obs->setCalcOceanTideRate (calOcean->getElement(obsIdx, 1));
    };
    if (calEopHiFiUt)
    {
      obs->setCalcHiFyUt1Delay(calEopHiFiUt->getElement(obsIdx, 0));
      obs->setCalcHiFyUt1Rate (calEopHiFiUt->getElement(obsIdx, 1));
    };
    if (calEopHiFiXY)
    {
      obs->setCalcHiFyPxyDelay(calEopHiFiXY->getElement(obsIdx, 0));
      obs->setCalcHiFyPxyRate (calEopHiFiXY->getElement(obsIdx, 1));
    };
    if (calPoleTide)
    {
      obs->setCalcPoleTideDelay(calPoleTide->getElement(obsIdx, 0));
      obs->setCalcPoleTideRate (calPoleTide->getElement(obsIdx, 1));
    };
    if (calEarthTide)
    {
      obs->setCalcEarthTideDelay(calEarthTide->getElement(obsIdx, 0));
      obs->setCalcEarthTideRate (calEarthTide->getElement(obsIdx, 1));
    };
    if (calOceanPoleTideLd)
    {
      obs->setCalcOceanPoleTideLdDelay(calOceanPoleTideLd->getElement(obsIdx, 0));
      obs->setCalcOceanPoleTideLdRate (calOceanPoleTideLd->getElement(obsIdx, 1));
    };
    if (calWobbleX)
    {
      obs->setCalcPxDelay(calWobbleX->getElement(obsIdx, 0));
      obs->setCalcPxRate (calWobbleX->getElement(obsIdx, 1));
    };
    if (calWobbleY)
    {
      obs->setCalcPyDelay(calWobbleY->getElement(obsIdx, 0));
      obs->setCalcPyRate (calWobbleY->getElement(obsIdx, 1));
    };
    if (calWobNut)
    {
      obs->setCalcWobNutatContrib(calWobNut->getElement(obsIdx, 0));
      //obs->setCalcWobNutatContrib(calWobNut->getElement(obsIdx, 1));
    };
    if (calHiFreqLibrationPm)
    {
      obs->setCalcHiFyPxyLibrationDelay(calHiFreqLibrationPm->getElement(obsIdx, 0));
      obs->setCalcHiFyPxyLibrationRate (calHiFreqLibrationPm->getElement(obsIdx, 1));
    };
    if (calHiFreqLibrationUt)
    {
      obs->setCalcHiFyUt1LibrationDelay(calHiFreqLibrationUt->getElement(obsIdx, 0));
      obs->setCalcHiFyUt1LibrationRate (calHiFreqLibrationUt->getElement(obsIdx, 1));
    };
    
    if (rSun)
      obs->setRsun(Sg3dVector(rSun->getElement(obs2ScanIdx, 0), 
                              rSun->getElement(obs2ScanIdx, 1), 
                              rSun->getElement(obs2ScanIdx, 2)));
    if (vSun)
      obs->setVsun(Sg3dVector(vSun->getElement(obs2ScanIdx, 0), 
                              vSun->getElement(obs2ScanIdx, 1), 
                              vSun->getElement(obs2ScanIdx, 2)));
    if (rMoon)
      obs->setRmoon(Sg3dVector(rMoon->getElement(obs2ScanIdx, 0), 
                               rMoon->getElement(obs2ScanIdx, 1), 
                               rMoon->getElement(obs2ScanIdx, 2)));
    if (vMoon)
      obs->setVmoon(Sg3dVector(vMoon->getElement(obs2ScanIdx, 0), 
                               vMoon->getElement(obs2ScanIdx, 1), 
                               vMoon->getElement(obs2ScanIdx, 2)));
   
    if (rEarth)
      obs->setRearth(Sg3dVector(rEarth->getElement(obs2ScanIdx, 0), 
                                rEarth->getElement(obs2ScanIdx, 1), 
                                rEarth->getElement(obs2ScanIdx, 2)));
    if (vEarth)
      obs->setVearth(Sg3dVector(vEarth->getElement(obs2ScanIdx, 0), 
                                vEarth->getElement(obs2ScanIdx, 1), 
                                vEarth->getElement(obs2ScanIdx, 2)));
    if (aEarth)
      obs->setAearth(Sg3dVector(aEarth->getElement(obs2ScanIdx, 0), 
                                aEarth->getElement(obs2ScanIdx, 1), 
                                aEarth->getElement(obs2ScanIdx, 2)));
    if (t2c_val)
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfVal()((DIRECTION)k, (DIRECTION)l) = t2c_val->getElement(obs2ScanIdx, 3*k+l);
    if (t2c_rat)
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfRat()((DIRECTION)k, (DIRECTION)l) = t2c_rat->getElement(obs2ScanIdx, 3*k+l);
    if (t2c_acc)
      for (int k=0; k<3; k++)
        for (int l=0; l<3; l++)
          obs->trf2crfAcc()((DIRECTION)k, (DIRECTION)l) = t2c_acc->getElement(obs2ScanIdx, 3*k+l);

    if (ut1_tai)
      obs->setCalcUt1_Tai(ut1_tai->getElement(obs2ScanIdx));
    
    if (pmXy)
    {
      obs->setCalcPmX(pmXy->getElement(obs2ScanIdx, 0));
      obs->setCalcPmY(pmXy->getElement(obs2ScanIdx, 1));
    };

    if (valNutNro)
    {
      obs->setCalcCipXv(valNutNro->getElement(obs2ScanIdx, 0));
      obs->setCalcCipYv(valNutNro->getElement(obs2ScanIdx, 1));
      obs->setCalcCipSv(valNutNro->getElement(obs2ScanIdx, 2));
    };
    if (valNutNro_rates)
    {
      obs->setCalcCipXr(valNutNro_rates->getElement(obs2ScanIdx, 0));
      obs->setCalcCipYr(valNutNro_rates->getElement(obs2ScanIdx, 1));
      obs->setCalcCipSr(valNutNro_rates->getElement(obs2ScanIdx, 2));
    };
    if (valNutEqxWahr)
    {
      obs->setCalcNutWahr_dPsiV(valNutEqxWahr->getElement(obs2ScanIdx, 0));
      obs->setCalcNutWahr_dEpsV(valNutEqxWahr->getElement(obs2ScanIdx, 1));
    };
    if (valNutEqxWahr_rates)
    {
      obs->setCalcNutWahr_dPsiR(valNutEqxWahr_rates->getElement(obs2ScanIdx, 0));
      obs->setCalcNutWahr_dEpsR(valNutEqxWahr_rates->getElement(obs2ScanIdx, 1));
    };
    if (valNutEqx)
    {
      obs->setCalcNut2006_dPsiV(valNutEqx->getElement(obs2ScanIdx, 0));
      obs->setCalcNut2006_dEpsV(valNutEqx->getElement(obs2ScanIdx, 1));
    };
    if (valNutEqx_rates)
    {
      obs->setCalcNut2006_dPsiR(valNutEqx_rates->getElement(obs2ScanIdx, 0));
      obs->setCalcNut2006_dEpsR(valNutEqx_rates->getElement(obs2ScanIdx, 1));
    };
    if (calOceanOld)
    {
      obs->setCalcOceanTideOldDelay(calOceanOld->getElement(obsIdx, 0));
      obs->setCalcOceanTideOldRate (calOceanOld->getElement(obsIdx, 1));
    };
    if (calPoleTideOldRestore)
    {
      obs->setCalcPoleTideOldDelay(calPoleTideOldRestore->getElement(obsIdx, 0));
      obs->setCalcPoleTideOldRate (calPoleTideOldRestore->getElement(obsIdx, 1));
    };
    if (calTiltRmvr)
    {
      obs->setCalcTiltRemvrDelay(calTiltRmvr->getElement(obsIdx, 0));
      obs->setCalcTiltRemvrRate (calTiltRmvr->getElement(obsIdx, 1));
    };
    //
    // partials:
    // stations:
    if (dTau_dR_1)
    {
      Sg3dVector                r(dTau_dR_1->getElement(obsIdx, 0), 
                                  dTau_dR_1->getElement(obsIdx, 1), 
                                  dTau_dR_1->getElement(obsIdx, 2));
      obs->setDdel_dR_1( r);
      obs->setDdel_dR_2(-r);
    };
    if (dRat_dR_1)
    {
      Sg3dVector                r(dRat_dR_1->getElement(obsIdx, 0), 
                                  dRat_dR_1->getElement(obsIdx, 1), 
                                  dRat_dR_1->getElement(obsIdx, 2));
      obs->setDrat_dR_1( r);
      obs->setDrat_dR_2(-r);
    };
    // sources:
    if (dV_dRA && dV_dDN)
    {
      obs->setDdel_dRA( dV_dRA->getElement(obsIdx, 0) );
      obs->setDdel_dDN( dV_dDN->getElement(obsIdx, 0) );
      obs->setDrat_dRA( dV_dRA->getElement(obsIdx, 1) );
      obs->setDrat_dDN( dV_dDN->getElement(obsIdx, 1) );
    };
    // dUT1:
    if (dV_dUT1)
    {
      obs->setDdel_dUT1( dV_dUT1->getElement(obsIdx, 0)*86400.0 ); // the sign has been already inverted
      obs->setDrat_dUT1( dV_dUT1->getElement(obsIdx, 1)*86400.0 ); // the sign has been already inverted
      obs->setD2del_dUT12(dV_dUT1->getElement(obsIdx, 2));
      obs->setD2rat_dUT12(dV_dUT1->getElement(obsIdx, 3));
    };
    // Polar motion:
    if (dV_dPx && dV_dPy)
    {
      obs->setDdel_dPx(dV_dPx->getElement(obsIdx, 0));
      obs->setDdel_dPy(dV_dPy->getElement(obsIdx, 0));
      obs->setDrat_dPx(dV_dPx->getElement(obsIdx, 1));
      obs->setDrat_dPy(dV_dPy->getElement(obsIdx, 1));
    };
    // Angles of nutation:
    if (dV_dCipX && dV_dCipY)
    {
      obs->setDdel_dCipX( dV_dCipX->getElement(obsIdx, 0) );
      obs->setDdel_dCipY( dV_dCipY->getElement(obsIdx, 0) );
      obs->setDrat_dCipX( dV_dCipX->getElement(obsIdx, 1) );
      obs->setDrat_dCipY( dV_dCipY->getElement(obsIdx, 1) );
    };
    if (dV_dBend)
    {
      obs->setDdel_dBend(dV_dBend->getElement(obsIdx, 0));
      obs->setDrat_dBend(dV_dBend->getElement(obsIdx, 1));
    };
    if (dV_dGamma)
    {
      obs->setDdel_dGamma(dV_dGamma->getElement(obsIdx, 0));
      obs->setDrat_dGamma(dV_dGamma->getElement(obsIdx, 1));
    };
    if (dV_dParallax)
    {
      obs->setDdel_dParallax(dV_dParallax->getElement(obsIdx, 0));
      obs->setDrat_dParallax(dV_dParallax->getElement(obsIdx, 1));
    };
    if (dV_dPtdX && dV_dPtdY)
    {
      obs->setDdel_dPolTideX(dV_dPtdX->getElement(obsIdx, 0));
      obs->setDrat_dPolTideX(dV_dPtdX->getElement(obsIdx, 1));
      obs->setDdel_dPolTideY(dV_dPtdY->getElement(obsIdx, 0));
      obs->setDrat_dPolTideY(dV_dPtdY->getElement(obsIdx, 1));
    };
    //
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
      obs->setAuxObs_1(auxObs);
//      if (calUnphaseCal_1)
//        auxObs->setCalcUnPhaseCal(calUnphaseCal_1->getElement(obsIdx, 0));
    }
    else
      obs->setAuxObs_1(auxObsByScan->value(scanId));
    // the second station:
    auxObsByScan = station2Info->auxObservationByScanId();
    if (!auxObsByScan->contains(scanId)) // new scan, insert data:
    {
      SgVlbiAuxObservation     *auxObs=new SgVlbiAuxObservation;
      auxObs->setMJD(*obs);
      auxObsByScan->insert(scanId, auxObs);
      obs->setAuxObs_2(auxObs);
//      if (calUnphaseCal_2)
//        auxObs->setCalcUnPhaseCal(calUnphaseCal_2->getElement(obsIdx, 0));
    }
    else
      obs->setAuxObs_2(auxObsByScan->value(scanId));
    // cumulative statistics (do we need it?):
    station1Info->incNumTotal(DT_DELAY);
    station2Info->incNumTotal(DT_DELAY);
    sourceInfo  ->incNumTotal(DT_DELAY);
    baselineInfo->incNumTotal(DT_DELAY);

    station1Info->incNumTotal(DT_RATE);
    station2Info->incNumTotal(DT_RATE);
    sourceInfo  ->incNumTotal(DT_RATE);
    baselineInfo->incNumTotal(DT_RATE);
    //
    // user corrections:
    if (numOfUserCorrections)
    {
      obs->userCorrections().resize(numOfUserCorrections);
      for (int i=0; i<numOfUserCorrections; i++)
        obs->userCorrections()[i] = userCorrections->getElement(obsIdx, i);
    };
  };
  //
  //
  // collect observables:
  SgVlbiStationInfo                    *bandStation1Info, *bandStation2Info;
  SgVlbiSourceInfo                     *bandSourceInfo;
  SgVlbiBaselineInfo                   *bandBaselineInfo;
  SgVlbiObservable                     *o=NULL;
  // for calculation of effective frequencies:
  double                                effFreq4GR=0.0, effFreq4PH=0.0, effFreq4RT=0.0;
  //
  skyFreqByIfId_.clear();
  //
  for (int bandIdx=0; bandIdx<listOfBands.size(); bandIdx++)
  {
    const QString              &bandKey=listOfBands.at(bandIdx);
    SgVlbiBand                 *band=new SgVlbiBand;
    band->setKey(bandKey);
    bands_.append(band);
    bandByKey_.insert(bandKey, band);
    //
    SgMatrix                   *singleBandDelays            = singleBandDelaysByBand[bandKey];
    SgMatrix                   *groupDelays                 = groupDelaysByBand[bandKey];
    SgMatrix                   *rates                       = ratesByBand[bandKey];
    SgMatrix                   *phases                      = phasesByBand[bandKey];
    SgMatrix                   *ionCals                     = ionCalsByBand[bandKey];
    SgMatrix                   *ionSigmas                   = ionSigmasByBand[bandKey];
    SgMatrix                   *phaseCals_1                 = phaseCals_1ByBand[bandKey];
    SgMatrix                   *phaseCals_2                 = phaseCals_2ByBand[bandKey];
    SgMatrix                   *phaseCalFreqs_1             = phaseCalFreqs_1ByBand[bandKey];
    SgMatrix                   *phaseCalFreqs_2             = phaseCalFreqs_2ByBand[bandKey];
    SgMatrix                   *phaseCalAmps_1              = phaseCalAmps_1ByBand[bandKey];
    SgMatrix                   *phaseCalAmps_2              = phaseCalAmps_2ByBand[bandKey];
    SgMatrix                   *phaseCalPhases_1            = phaseCalPhases_1ByBand[bandKey];
    SgMatrix                   *phaseCalPhases_2            = phaseCalPhases_2ByBand[bandKey];
    SgMatrix                   *phaseCalOffsets_1           = phaseCalOffsets_1ByBand[bandKey];
    SgMatrix                   *phaseCalOffsets_2           = phaseCalOffsets_2ByBand[bandKey];
    SgMatrix                   *effFreqs                    = effFreqsByBand[bandKey];
    SgMatrix                   *uv                          = uvByBand[bandKey];
    SgMatrix                   *calFeedCorr                 = calFeedCorrByBand[bandKey];

    SgVector                   *refFreqs                    = refFreqsByBand[bandKey];
    SgVector                   *correlation                 = correlationByBand[bandKey];
    SgVector                   *snr                         = snrByBand[bandKey];
    SgVector                   *ambigs                      = ambigsByBand[bandKey];

    const QVector<QString>     &qCodes=qCodesByBand[bandKey];
    const QVector<int>         &numAmbigs=numAmbigsByBand[bandKey];
    const QVector<int>         &numSubAmbigs=numSubAmbigsByBand[bandKey];
    const QVector<int>         &numPhsAmbigs=numPhsAmbigsByBand[bandKey];
    const QVector<int>         &dataFlag=dataFlagByBand[bandKey];

    // correlator info stuff:
    const QVector<QString>     &fourfitOutputFName=fourfitOutputFNameByBand[bandKey];
    const QVector<QString>     &fourfitErrorCodes=fourfitErrorCodesByBand[bandKey];
    const QVector<QString>     &baselineCodes=baselineCodesByBand[bandKey];
    const QVector<QString>     &tapeQualityCodes=tapeQualityCodesByBand[bandKey];
    const QVector<QString>     &vlb1FileNames=vlb1FileNamesByBand[bandKey];
    const QVector<QString>     &tapeIds_1=tapeIds_1ByBand[bandKey];
    const QVector<QString>     &tapeIds_2=tapeIds_2ByBand[bandKey];
//  const QVector<int>         &corelVersions=corelVersionsByBand[bandKey];
    const QVector<int>         &startOffsets=startOffsetsByBand[bandKey];
    const QVector<int>         &stopOffsets=stopOffsetsByBand[bandKey];
    const QVector<int>         &hopsRevisionNumbers=hopsRevisionNumbersByBand[bandKey];
    const QVector< QVector<int> >
                               &fourfitVersions=fourfitVersionsByBand[bandKey];
//  const QVector< QVector<int> >
//                             &dbeditVersion=dbeditVersionByBand[bandKey];
    const QVector< QVector<int> >
                               &epochsOfCorrelations=epochsOfCorrelationsByBand[bandKey];
    const QVector< QVector<int> >
                               &epochsOfCentrals=epochsOfCentralsByBand[bandKey];
    const QVector< QVector<int> >
                               &epochsOfFourfitings=epochsOfFourfitingsByBand[bandKey];
    const QVector< QVector<int> >
                               &epochsOfNominalScanTime=epochsOfNominalScanTimeByBand[bandKey];
    SgVector                   *deltaEpochs=deltaEpochsByBand[bandKey];
    SgVector                   *smplRate=smplRateByBand[bandKey];
    SgVector                   *sbdResids=sbdResidsByBand[bandKey];
    SgVector                   *rateResids=rateResidsByBand[bandKey];
    SgVector                   *effDurations=effDurationsByBand[bandKey];
    SgVector                   *startSeconds=startSecondsByBand[bandKey];
    SgVector                   *stopSeconds=stopSecondsByBand[bandKey];
    SgVector                   *percentOfDiscards=percentOfDiscardsByBand[bandKey];
    SgVector                   *uniformityFactors=uniformityFactorsByBand[bandKey];
    SgVector                   *geocenterPhaseResids=geocenterPhaseResidsByBand[bandKey];
    SgVector                   *geocenterPhaseTotals=geocenterPhaseTotalsByBand[bandKey];
    SgVector                   *geocenterSbDelays=geocenterSbDelaysByBand[bandKey];
    SgVector                   *geocenterGrDelays=geocenterGrDelaysByBand[bandKey];
    SgVector                   *geocenterDelayRates=geocenterDelayRatesByBand[bandKey];
    SgVector                   *probOfFalses=probOfFalsesByBand[bandKey];
    SgVector                   *corrAprioriDelays=corrAprioriDelaysByBand[bandKey];
    SgVector                   *corrAprioriRates=corrAprioriRatesByBand[bandKey];
    SgVector                   *corrAprioriAccs=corrAprioriAccsByBand[bandKey];
    SgVector                   *incohAmps=incohAmpsByBand[bandKey];
    SgVector                   *incohAmps2=incohAmps2ByBand[bandKey];
    SgVector                   *delayResids=delayResidsByBand[bandKey];
    SgMatrix                   *urvrs=urvrsByBand[bandKey];
    SgMatrix                   *instrDelays=instrDelaysByBand[bandKey];
    SgMatrix                   *starElev=starElevByBand[bandKey];
    SgMatrix                   *zenithDelays=zenithDelaysByBand[bandKey];
    SgMatrix                   *searchParams=searchParamsByBand[bandKey];
    SgMatrix                   *corrClocks=corrClocksByBand[bandKey];
    SgMatrix                   *mDlys=mDlysByBand[bandKey];
    SgMatrix                   *mAux=mAuxByBand[bandKey];
    // EoCIS
    const QVector<QString>     &fourfitControlFile=fourfitControlFileByBand[bandKey];
    const QVector<QString>     &fourfitCommand    =fourfitCommandByBand[bandKey];
    const QVector<int>         &numLags           =numLagsByBand[bandKey];
    const QVector<double>      &apLength          =apLengthByBand[bandKey];
    // channel info stuff:
    const QVector<QString>     &channelIds=channelIdsByBand[bandKey];
    const QVector<QString>     &polarizations=polarizationsByBand[bandKey];

    const QVector<int>         &numOfChannels=numOfChannelsByBand[bandKey];
    const QVector<int>         &bitsPerSamples=bitsPerSamplesByBand[bandKey];
                                
    const QVector< QVector<int> >
                               &errorRates_1=errorRates_1ByBand[bandKey],
                               &errorRates_2=errorRates_2ByBand[bandKey],
                               &bbcIdxs_1=bbcIdxs_1ByBand[bandKey],
                               &bbcIdxs_2=bbcIdxs_2ByBand[bandKey],
                               &corelIdxNumbersUsb=corelIdxNumbersUsbByBand[bandKey],
                               &corelIdxNumbersLsb=corelIdxNumbersLsbByBand[bandKey];
    const SgVector             *sampleRate=sampleRateByBand[bandKey];
    const SgMatrix             *residFringeAmplByChan=residFringeAmplByChanByBand[bandKey],
                               *residFringePhseByChan=residFringePhseByChanByBand[bandKey],
                               *refFreqByChan=refFreqByChanByBand[bandKey],
                               *numOfSamplesByChan_USB=numOfSamplesByChan_USBByBand[bandKey],
                               *numOfSamplesByChan_LSB=numOfSamplesByChan_LSBByBand[bandKey],
                               *numOfAccPeriodsByChan_USB=numOfAccPeriodsByChan_USBByBand[bandKey],
                               *numOfAccPeriodsByChan_LSB=numOfAccPeriodsByChan_LSBByBand[bandKey],
                               *loFreqs_1=loFreqs_1ByBand[bandKey],
                               *loFreqs_2=loFreqs_2ByBand[bandKey];

    //
    // complain:
    //
    if (numOfAccPeriodsByChan_USB == NULL || numOfAccPeriodsByChan_LSB == NULL)
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): numsOfAccPeriodsByChan_* == NULL");
    if (refFreqByChan == NULL)
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): refFreqsByChan == NULL");
    if (residFringeAmplByChan == NULL)
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): residFringeAmplByChan == NULL");
    //
    if (numOfChannels.size())
    {
      band->addAttr(SgVlbiBand::Attr_MISSING_CHANNEL_INFO);
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): channel info is missing");
    };
    //

    
//  QVector<int>     &ionDataFlags = ionDataFlagByBand[bandKey];
    // import observations:
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): processing " + bandKey + 
      "-band of the session " + vgosDb->getSessionName());
    int                         nYr, nMo, nDy, nHr, nMi;
    double                      dSec;
    SgMJD                       tStart(tZero), tStop(tZero);
 
    for (int obsIdx=0; obsIdx<numOfObs; obsIdx++)
    {
      bool                      hasPhaseCal;
      o = NULL;
      if ((dataFlag.size()>0 && dataFlag.at(obsIdx) != -1) ||
          ( ambigs && 0.0<ambigs->getElement(obsIdx) && 
            //snr && 0.0<snr->getElement(obsIdx) &&     // SNR is missing in early databases
            groupDelays)
         )
      {
        station1Name = stations_1.at(obsIdx);
        station2Name = stations_2.at(obsIdx);
        sourceName   = sources.at(obsIdx);
        epoch        = epochs4Obs.at(obsIdx);
        baselineName = station1Name + ":" + station2Name;
        obsKey.sprintf("%s", 
          qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));
        // it have to be inserted already:
        if (observationByKey_.contains(obsKey))
          obs = observationByKey_.value(obsKey);
        else
        {
          logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): the observation #" + QString("").setNum(obsIdx) + ", " +
            obsKey + ", does not exist in the container");
          return false;
        };
        //
        // need more robust checking for missed observables here
        o = new SgVlbiObservable(obs, band);
        o->setMediaIdx(obsIdx);
        obs->addObservable(bandKey, o);
        //
        // band's station #1:
        if (band->stationsByName().contains(station1Name))
          bandStation1Info = band->stationsByName().value(station1Name);
        else // new station, add it to the container:
        {
          bandStation1Info = new SgVlbiStationInfo(obs->getStation1Idx(), station1Name);
          band->stationsByName().insert(bandStation1Info->getKey(), bandStation1Info);
          band->stationsByIdx().insert(bandStation1Info->getIdx(), bandStation1Info);
        };
        // band's station #2:
        if (band->stationsByName().contains(station2Name))
          bandStation2Info = band->stationsByName().value(station2Name);
        else // new station, add it to the container:
        {
          bandStation2Info = new SgVlbiStationInfo(obs->getStation2Idx(), station2Name);
          band->stationsByName().insert(bandStation2Info->getKey(), bandStation2Info);
          band->stationsByIdx().insert(bandStation2Info->getIdx(), bandStation2Info);
        };
        // band's source:
        if (band->sourcesByName().contains(sourceName))
          bandSourceInfo = band->sourcesByName().value(sourceName);
        else // new source, add it to the container:
        {
          bandSourceInfo = new SgVlbiSourceInfo(obs->getSourceIdx(), sourceName);
          band->sourcesByName().insert(bandSourceInfo->getKey(), bandSourceInfo);
          band->sourcesByIdx().insert(bandSourceInfo->getIdx(), bandSourceInfo);
        };
        // band's baselines:
        if (band->baselinesByName().contains(baselineName))
          bandBaselineInfo = band->baselinesByName().value(baselineName);
        else // new baseline, add it to the container and register its index:
        {
          bandBaselineInfo = new SgVlbiBaselineInfo(obs->getBaselineIdx(), baselineName);
          band->baselinesByName().insert(bandBaselineInfo->getKey(), bandBaselineInfo);
          band->baselinesByIdx().insert(bandBaselineInfo->getIdx(), bandBaselineInfo);
        };
        // collect data:
        // single band delay:
        if (singleBandDelays)
        {
          o->sbDelay().setValue(singleBandDelays->getElement(obsIdx, 0));
          o->sbDelay().setSigma(singleBandDelays->getElement(obsIdx, 1));
          if (o->sbDelay().getSigma() < 5.0e-12)
            o->owner()->addAttr(SgVlbiObservation::Attr_SBD_NOT_VALID);
        };
        // group delay:
        if (groupDelays)
        {
          o->grDelay().setValue(groupDelays->getElement(obsIdx, 0));
          o->grDelay().setSigma(groupDelays->getElement(obsIdx, 1));
          if (o->grDelay().getSigma() < 1.0e-15)
            o->owner()->addAttr(SgVlbiObservation::Attr_NOT_VALID);
        };
        // delay rate:
        if (rates)
        {
          o->phDRate().setValue(rates->getElement(obsIdx, 0));
          o->phDRate().setSigma(rates->getElement(obsIdx, 1));
        };
        // ambiguity spacing:
        if (ambigs)
          o->grDelay().setAmbiguitySpacing(ambigs->getElement(obsIdx));
        // number of ambiguities:
        if (numAmbigs.size())
          o->grDelay().setNumOfAmbiguities(numAmbigs.at(obsIdx));
        // number of ambiguities:
        if (numSubAmbigs.size())
          o->grDelay().setNumOfSubAmbigs(numSubAmbigs.at(obsIdx));
        // number of ambiguities, phase delays:
        if (numPhsAmbigs.size())
          o->phDelay().setNumOfAmbiguities(numPhsAmbigs.at(obsIdx));
        // quality code:
        if (qCodes.size())
        {
          int                 d;
          bool                isTmp;
          d = qCodes.at(obsIdx).toInt(&isTmp);
          if (!isTmp)
            d = -1;
          o->setQualityFactor(d);
        }
        else // old data, no quality code:
          o->setQualityFactor(9);
        // frequencies:
        if (refFreqs)
          o->setReferenceFrequency(refFreqs->getElement(obsIdx));
        if (effFreqs)
        {
          o->grDelay().setEffFreq(effFreqs->getElement(obsIdx, 0));
          o->phDelay().setEffFreq(effFreqs->getElement(obsIdx, 1));
          o->phDRate().setEffFreq(effFreqs->getElement(obsIdx, 2));
        };
        // correlation and SNR:
        if (correlation)
          o->setCorrCoeff(correlation->getElement(obsIdx));
        if (uv)
        {
          o->setUvFrPerAsec(0, uv->getElement(obsIdx, 0));
          o->setUvFrPerAsec(1, uv->getElement(obsIdx, 1));
        };
        if (calFeedCorr)
        {
          o->setCalcFeedCorrDelay(calFeedCorr->getElement(obsIdx, 0));
          o->setCalcFeedCorrRate (calFeedCorr->getElement(obsIdx, 1));
        };
        if (snr)
          o->setSnr(snr->getElement(obsIdx));
        // total phase:
        if (phases)
          o->setTotalPhase(phases->getElement(obsIdx, 0));
        //
        // Edits:
        // check for ionospheric correction, if it is already calculated, get it:
        if (ionCals && ionSigmas)
        {
          o->grDelay().setIonoValue(ionCals -> getElement(obsIdx, 0));
          o->phDRate().setIonoValue(ionCals -> getElement(obsIdx, 1));
          o->grDelay().setIonoSigma(ionSigmas->getElement(obsIdx, 0));
          o->phDRate().setIonoSigma(ionSigmas->getElement(obsIdx, 1));
        };
        if (phaseCals_1 && phaseCals_2)
        {
          o->setPhaseCalGrDelays(0, phaseCals_1->getElement(obsIdx, 0));
          o->setPhaseCalGrDelays(1, phaseCals_2->getElement(obsIdx, 0));
          o->setPhaseCalRates   (0, phaseCals_1->getElement(obsIdx, 1));
          o->setPhaseCalRates   (1, phaseCals_2->getElement(obsIdx, 1));
          };
        //
        // channel set up:
        if (bitsPerSamples.size())
          o->setBitsPerSample(bitsPerSamples.at(obsIdx));
        //
        // channel dependent stuff:
        //
        int                     nOfC=numOfChannels.size()?numOfChannels.at(obsIdx):0;
        if (nOfC>0)
        {
          o->allocateChannelsSetupStorages(nOfC);
          int                   numOfActualChannels=0;
          // set up data:
          for (int j=0; j<nOfC; j++)
          {
            // channel setup:
            o->numOfAccPeriodsByChan_USB()->setElement(j,
              numOfAccPeriodsByChan_USB?numOfAccPeriodsByChan_USB->getElement(obsIdx, j):0.0);
            o->numOfAccPeriodsByChan_LSB()->setElement(j,
              numOfAccPeriodsByChan_LSB?numOfAccPeriodsByChan_LSB->getElement(obsIdx, j):0.0);
            o->numOfSamplesByChan_USB()->setElement   (j,
              numOfSamplesByChan_USB?numOfSamplesByChan_USB->getElement(obsIdx, j):0.0);
            o->numOfSamplesByChan_LSB()->setElement   (j, 
              numOfSamplesByChan_LSB?numOfSamplesByChan_LSB->getElement(obsIdx, j):0.0);
            o->refFreqByChan()->setElement          (j, 
              refFreqByChan?refFreqByChan->getElement(obsIdx, j):0.0);
            if (0.0001 < o->refFreqByChan()->getElement(j))
              numOfActualChannels++;
            o->fringeAmplitudeByChan()->setElement    (j, 
              residFringeAmplByChan?residFringeAmplByChan->getElement(obsIdx, j):0.0);
            o->fringePhaseByChan()->setElement        (j,
              residFringePhseByChan?residFringePhseByChan->getElement(obsIdx, j)/RAD2DEG:0.0);
            if (loFreqs_1 && loFreqs_1->nRow()>0 && loFreqs_1->nCol()>0)
              o->loFreqByChan_1()->setElement(j, loFreqs_1->getElement(obsIdx, j));
            if (loFreqs_2 && loFreqs_2->nRow()>0 && loFreqs_2->nCol()>0)
              o->loFreqByChan_2()->setElement(j, loFreqs_2->getElement(obsIdx, j));

            (*o->chanIdByChan())[j] = channelIds.size()?channelIds.at(obsIdx).at(2*j).toLatin1():' ';
            (*o->polarization_1ByChan())[j] = polarizations.size()?
              polarizations.at(obsIdx).at(4*j    ).toLatin1():' ';
            (*o->polarization_2ByChan())[j] = polarizations.size()?
              polarizations.at(obsIdx).at(4*j + 2).toLatin1():' ';
            (*o->corelIndexNumUSB())[j] = corelIdxNumbersUsb.size()?corelIdxNumbersUsb.at(obsIdx).at(j):0;
            (*o->corelIndexNumLSB())[j] = corelIdxNumbersLsb.size()?corelIdxNumbersLsb.at(obsIdx).at(j):0;
            (*o->bbcIdxByChan_1())[j] = bbcIdxs_1.size()?bbcIdxs_1.at(obsIdx).at(j):0;
            (*o->bbcIdxByChan_2())[j] = bbcIdxs_2.size()?bbcIdxs_2.at(obsIdx).at(j):0;
            //
            //
            // phase calibration data:
            if (phaseCalAmps_1 && phaseCalAmps_2 && phaseCalPhases_1 && phaseCalPhases_2 &&
                phaseCalFreqs_1 && phaseCalFreqs_2 && phaseCalOffsets_1 && phaseCalOffsets_2)
            {
              o->phaseCalData_1ByChan()->setElement (SgVlbiObservable::PCCI_AMPLITUDE,j, 
                phaseCalAmps_1->getElement(obsIdx, j));
              o->phaseCalData_2ByChan()->setElement (SgVlbiObservable::PCCI_AMPLITUDE,j,
                phaseCalAmps_2->getElement(obsIdx, j));
              o->phaseCalData_1ByChan()->setElement (SgVlbiObservable::PCCI_PHASE,j,
                phaseCalPhases_1->getElement(obsIdx, j)*DEG2RAD/100.0);
              o->phaseCalData_2ByChan()->setElement (SgVlbiObservable::PCCI_PHASE,j,
                phaseCalPhases_2->getElement(obsIdx, j)*DEG2RAD/100.0);
              o->phaseCalData_1ByChan()->setElement (SgVlbiObservable::PCCI_FREQUENCY,j,
                phaseCalFreqs_1->getElement(obsIdx, j));
              o->phaseCalData_2ByChan()->setElement (SgVlbiObservable::PCCI_FREQUENCY,j,
                phaseCalFreqs_2->getElement(obsIdx, j));
              o->phaseCalData_1ByChan()->setElement (SgVlbiObservable::PCCI_OFFSET,j,
                phaseCalOffsets_1->getElement(obsIdx, j)*DEG2RAD/100.0);
              o->phaseCalData_2ByChan()->setElement (SgVlbiObservable::PCCI_OFFSET,j,
                phaseCalOffsets_2->getElement(obsIdx, j)*DEG2RAD/100.0);
              hasPhaseCal = true;
            }
            else
              hasPhaseCal = false;
            //
            //
            if (errorRates_1.size())
              o->phaseCalData_1ByChan()->setElement (4,j, errorRates_1.at(obsIdx).at(j));
            if (errorRates_2.size())
              o->phaseCalData_2ByChan()->setElement (4,j, errorRates_2.at(obsIdx).at(j));
            //
            //
            QString           ifId((*o->chanIdByChan())[j]);
            if (ifId.size() && ifId!=QString(" "))
            {
              if (skyFreqByIfId_.contains(ifId) &&
                  skyFreqByIfId_.value(ifId) != o->refFreqByChan()->getElement(j))
                logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
                  "::getDataFromVgosDb(): the IF \"" + ifId + "\" got new sky frequency " + 
                    QString("").sprintf("%.4f (old=%.4f) obs:", 
                    o->refFreqByChan()->getElement(j), skyFreqByIfId_.value(ifId)) +
                    o->strId());
              skyFreqByIfId_[ifId] = o->refFreqByChan()->getElement(j);
            };
          };
          //
          if (cppsSoft_ != CPPS_PIMA && hasPhaseCal)
            o->calcPhaseCalDelay();
          //
          if (sampleRate && numOfActualChannels && 
              cppsSoft_ != CPPS_PIMA &&
              vgosDb->getCorrelatorType() != SgVgosDb::CT_S2)
          {
            o->setSampleRate(sampleRate->getElement(obsIdx));
            // calculate the effective frequencies:
            evaluateEffectiveFreqs(*o->numOfAccPeriodsByChan_USB(), *o->numOfAccPeriodsByChan_LSB(),
              *o->refFreqByChan(), *o->fringeAmplitudeByChan(),
              *o->numOfSamplesByChan_USB(), *o->numOfSamplesByChan_LSB(),
              sampleRate->getElement(obsIdx), o->getReferenceFrequency(), 
              nOfC, effFreq4GR, effFreq4PH, effFreq4RT, correlatorType, o->strId());
            o->grDelay().setEffFreq(effFreq4GR);
            o->phDelay().setEffFreq(effFreq4PH);
            o->phDRate().setEffFreq(effFreq4RT);
            // calculate the effective frequencies with equal weights:
            evaluateEffectiveFreqs(*o->numOfAccPeriodsByChan_USB(), *o->numOfAccPeriodsByChan_LSB(),
              *o->refFreqByChan(), *o->fringeAmplitudeByChan(),
              *o->numOfSamplesByChan_USB(), *o->numOfSamplesByChan_LSB(),
              sampleRate->getElement(obsIdx), o->getReferenceFrequency(),
              nOfC, effFreq4GR, effFreq4PH, effFreq4RT, correlatorType, o->strId(), true);
            o->grDelay().setEffFreqEqWgt(effFreq4GR);
            o->phDelay().setEffFreqEqWgt(effFreq4PH);
            o->phDRate().setEffFreqEqWgt(effFreq4RT);
          };
          //
          if (mDlys && o->vDlys())
          {
            o->vDlys()->setElement(0, mDlys->getElement(obsIdx,0));
            o->vDlys()->setElement(1, mDlys->getElement(obsIdx,1));
            o->vDlys()->setElement(2, mDlys->getElement(obsIdx,2));
          };
          if (mAux && o->vAuxData())
          {
            o->vAuxData()->setElement(0, mAux->getElement(obsIdx,0));
            o->vAuxData()->setElement(1, mAux->getElement(obsIdx,1)/RAD2DEG);
            o->vAuxData()->setElement(2, mAux->getElement(obsIdx,2));
            o->vAuxData()->setElement(3, mAux->getElement(obsIdx,3));
          };
        }
        else if (numOfChannels.size())
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): get zero number of channels: " + o->strId());
        //
        //
        // 
        // correlator info stuff:
        if (fourfitOutputFName.size())
          o->setFourfitOutputFName(fourfitOutputFName.at(obsIdx));
        if (fourfitErrorCodes.size())
          o->setErrorCode(fourfitErrorCodes.at(obsIdx));
        //
        if (baselineCodes.size() && cppsSoft_ != CPPS_PIMA)
        {
          char                  cid;
          cid = baselineCodes.at(obsIdx).at(0).toLatin1();
          station1Info = stationsByName_.value(station1Name);
          if (station1Info->getCid() == ' ')
          {
            station1Info->setCid(cid);
            logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): station#1 " + station1Info->getKey() + " set station CID to \"" + 
              cid + "\" at " + o->strId());
          }
          else if (station1Info->getCid() != cid)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): station#1 " + station1Info->getKey() + 
              " station CID missmatch: \"" + station2Info->getCid() + "\" vs \"" + cid + 
              "\" at " + o->strId());
          };
          cid = baselineCodes.at(obsIdx).at(1).toLatin1();
          station2Info = stationsByName_.value(station2Name);
          if (station2Info->getCid() == ' ')
          {
            station2Info->setCid(cid);
            logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): station#2 " + station2Info->getKey() + " set station CID to \"" + 
              cid + "\" at " + o->strId());
          }
          else if (station2Info->getCid() != cid)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): station#2 " + station2Info->getKey() + 
              " station CID missmatch: \"" + station2Info->getCid() + "\" vs \"" + cid + 
              "\" at " + o->strId());
          };
        };
        //
        if (tapeQualityCodes.size())
          o->setTapeQualityCode(tapeQualityCodes.at(obsIdx));
        if (vlb1FileNames.size())
          obs->setCorrRootFileName(vlb1FileNames.at(obsIdx));
        if (tapeIds_1.size() && obs->auxObs_1())
          obs->auxObs_1()->setTapeId(tapeIds_1.at(obsIdx));
        if (tapeIds_2.size() && obs->auxObs_2())
          obs->auxObs_2()->setTapeId(tapeIds_2.at(obsIdx));
        if (startOffsets.size())
          o->setStartOffset(startOffsets.at(obsIdx));
        if (stopOffsets.size())
          o->setStopOffset(stopOffsets.at(obsIdx));
        if (hopsRevisionNumbers.size())
          o->setHopsRevisionNumber(hopsRevisionNumbers.at(obsIdx));
        if (fourfitVersions.size() && 2<=fourfitVersions.at(obsIdx).size())
        {
          o->setFourfitVersion(0, fourfitVersions.at(obsIdx).at(0));
          o->setFourfitVersion(1, fourfitVersions.at(obsIdx).at(1));
        };
        //
        // epochs:
        if (epochsOfCorrelations.size() && 6<=epochsOfCorrelations.at(obsIdx).size())
        {
          epoch.setUpEpoch(epochsOfCorrelations.at(obsIdx).at(0), epochsOfCorrelations.at(obsIdx).at(1),
            epochsOfCorrelations.at(obsIdx).at(2), epochsOfCorrelations.at(obsIdx).at(3), 
            epochsOfCorrelations.at(obsIdx).at(4), (double)epochsOfCorrelations.at(obsIdx).at(5));
          o->setEpochOfCorrelation(epoch);
        };
        if (epochsOfCentrals.size() && 6<=epochsOfCentrals.at(obsIdx).size())
        {
          epoch.setUpEpoch(epochsOfCentrals.at(obsIdx).at(0), epochsOfCentrals.at(obsIdx).at(1),
            epochsOfCentrals.at(obsIdx).at(2), epochsOfCentrals.at(obsIdx).at(3), 
            epochsOfCentrals.at(obsIdx).at(4), (double)epochsOfCentrals.at(obsIdx).at(5));
          o->setEpochCentral(epoch);
        };
        if (epochsOfFourfitings.size() && 6<=epochsOfFourfitings.at(obsIdx).size())
        {
          epoch.setUpEpoch(epochsOfFourfitings.at(obsIdx).at(0), epochsOfFourfitings.at(obsIdx).at(1),
            epochsOfFourfitings.at(obsIdx).at(2), epochsOfFourfitings.at(obsIdx).at(3), 
            epochsOfFourfitings.at(obsIdx).at(4), (double)epochsOfFourfitings.at(obsIdx).at(5));
          o->setEpochOfFourfitting(epoch);
        };
        if (epochsOfNominalScanTime.size() && 6<=epochsOfNominalScanTime.at(obsIdx).size())
        {
          epoch.setUpEpoch(epochsOfNominalScanTime.at(obsIdx).at(0), 
            epochsOfNominalScanTime.at(obsIdx).at(1),
            epochsOfNominalScanTime.at(obsIdx).at(2), epochsOfNominalScanTime.at(obsIdx).at(3), 
            epochsOfNominalScanTime.at(obsIdx).at(4), (double)epochsOfNominalScanTime.at(obsIdx).at(5));
          o->setEpochOfScan(epoch);
        };
        //
        if (deltaEpochs)
          o->setCentrOffset(deltaEpochs->getElement(obsIdx));
        if (smplRate)
          o->setSampleRate(smplRate->getElement(obsIdx));
        if (sbdResids)
          o->sbDelay().setResidualFringeFitting(sbdResids->getElement(obsIdx));
        if (rateResids)
          o->phDRate().setResidualFringeFitting(rateResids->getElement(obsIdx));
        if (effDurations)
          o->setEffIntegrationTime(effDurations->getElement(obsIdx));
        //
        if (startSeconds && stopSeconds)
        {
          SgMJD::MJD_reverse(o->owner()->getDate(), o->owner()->getTime(),
            nYr, nMo, nDy, nHr, nMi, dSec);
          tStart = SgMJD(nYr, nMo, nDy, nHr, 0, startSeconds->getElement(obsIdx));
          tStop  = SgMJD(nYr, nMo, nDy, nHr, 0, stopSeconds->getElement(obsIdx));
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
        if (percentOfDiscards)
          o->setDiscardRatio(percentOfDiscards->getElement(obsIdx));
        if (uniformityFactors)
          o->setAcceptedRatio(uniformityFactors->getElement(obsIdx));
        if (geocenterPhaseResids)
          o->setGeocenterResidPhase(geocenterPhaseResids->getElement(obsIdx)/RAD2DEG);
        if (geocenterPhaseTotals)
          o->setGeocenterTotalPhase(geocenterPhaseTotals->getElement(obsIdx)/RAD2DEG);
        if (geocenterSbDelays)
          o->sbDelay().setGeocenterValue(geocenterSbDelays->getElement(obsIdx));
        if (geocenterGrDelays)
          o->grDelay().setGeocenterValue(geocenterGrDelays->getElement(obsIdx));
        if (geocenterDelayRates)
          o->phDRate().setGeocenterValue(geocenterDelayRates->getElement(obsIdx));
        if (delayResids)
          o->grDelay().setResidualFringeFitting(delayResids->getElement(obsIdx));
        if (probOfFalses)
          o->setProbabOfFalseDetection(probOfFalses->getElement(obsIdx));
        if (corrAprioriDelays)
          o->setAprioriDra(0, corrAprioriDelays->getElement(obsIdx));
        if (corrAprioriRates)
          o->setAprioriDra(1, corrAprioriRates->getElement(obsIdx));
        if (corrAprioriAccs)
          o->setAprioriDra(2, corrAprioriAccs->getElement(obsIdx));
        if (incohAmps)
          o->setIncohChanAddAmp(incohAmps->getElement(obsIdx));
        if (incohAmps2)
          o->setIncohSegmAddAmp(incohAmps2->getElement(obsIdx));

        // matrices:
        if (urvrs)
        {
          o->setUrVr(0, urvrs->getElement(obsIdx,0));
          o->setUrVr(1, urvrs->getElement(obsIdx,1));
        };
        if (instrDelays)
        {
          o->setInstrDelay(0, instrDelays->getElement(obsIdx,0));
          o->setInstrDelay(1, instrDelays->getElement(obsIdx,1));
        };
        //
        if (starElev)
        {
          o->setCorrelStarElev_1(starElev->getElement(obsIdx, 0)/RAD2DEG);
          o->setCorrelStarElev_2(starElev->getElement(obsIdx, 1)/RAD2DEG);
        };
        if (zenithDelays)
        {
          o->setCorrelZdelay_1(zenithDelays->getElement(obsIdx, 0));
          o->setCorrelZdelay_2(zenithDelays->getElement(obsIdx, 1));
        };
        //
        if (searchParams)
          for (int j=0; j<6; j++)
            o->setFourfitSearchParameters(j, searchParams->getElement(obsIdx,j));
        if (corrClocks)
        {
          o->setCorrClocks(0, 0,  corrClocks->getElement(obsIdx,0));
          o->setCorrClocks(0, 1,  corrClocks->getElement(obsIdx,1));
          o->setCorrClocks(1, 0,  corrClocks->getElement(obsIdx,2));
          o->setCorrClocks(1, 1,  corrClocks->getElement(obsIdx,3));
        };
        // EoCIS
        //
        if (fourfitControlFile.size())
          o->setFourfitControlFile(fourfitControlFile.at(obsIdx));
        if (fourfitCommand.size())
          o->setFourfitCommandOverride(fourfitCommand.at(obsIdx));
        if (numLags.size())
          o->setNlags(numLags.at(obsIdx));
        if (apLength.size())
          obs->setApLength(apLength.at(obsIdx));
        if (delUFlags.size())
          o->grDelay().setUnweightFlag(delUFlags.at(obsIdx));
        // config_ can be NULL:
        if (config_ && config_->getUseSolveObsSuppresionFlags() && 
          (o->grDelay().getUnweightFlag()==1 || o->grDelay().getUnweightFlag()==2))
          o->owner()->addAttr(SgObservation::Attr_NOT_VALID);
        if (ratUFlags.size())
          o->phDRate().setUnweightFlag(ratUFlags.at(obsIdx));
        if (phsUFlags.size())
          o->phDelay().setUnweightFlag(phsUFlags.at(obsIdx));
        // + uAcSup, where to put it?
        //
        if (ambigs && ambigs->getElement(obsIdx)<=0.0 &&             // no ambigs for
            groupDelays && groupDelays->getElement(obsIdx, 1) > 0.0) // good observation
        {
          o->setErrorCode("Z");
          if (vgosDb->getCorrelatorType()!=SgVgosDb::CT_Unknown || ambigs->getElement(obsIdx)<0.0)
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): wrong ambiguity spacing (" + 
              QString("").setNum(ambigs->getElement(obsIdx)) + ") for the observable #" + 
              QString("").setNum(obsIdx) + " at the " + bandKey + "-band of " + obs->getKey() +
              " on " + name_);
        };
        if (qCodes.size() && 
            qCodes.at(obsIdx).simplified().size() == 0 && // it is ok, nothing to complain
            o->getErrorCode().simplified().size() == 0)
          o->setErrorCode("Y");
        //
        // per band statistics:
        band            ->incNumTotal(DT_DELAY);
        bandStation1Info->incNumTotal(DT_DELAY);
        bandStation2Info->incNumTotal(DT_DELAY);
        bandSourceInfo  ->incNumTotal(DT_DELAY);
        bandBaselineInfo->incNumTotal(DT_DELAY);

        band            ->incNumTotal(DT_RATE);
        bandStation1Info->incNumTotal(DT_RATE);
        bandStation2Info->incNumTotal(DT_RATE);
        bandSourceInfo  ->incNumTotal(DT_RATE);
        bandBaselineInfo->incNumTotal(DT_RATE);
      }
      else
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): skipped observable #" + QString("").setNum(obsIdx) + 
          " at the " + bandKey + "-band: missed data");
    };
    //
    // set up band's attributes:
    band->setTCreation(vgosDb->getDateOfCreation());
    band->setInputFileName(vgosDb->getWrapperFileName());
/*
    if (hasCorrelatorType_)
      band->setCorrelatorType(image->getStr(dCorrelatorType_, 0,0));
*/

    if ((ionCals && ionSigmas) || (dTec && dTecStdDev))
    {
      band->addAttr(SgVlbiBand::Attr_HAS_IONO_SOLUTION);
      addAttr(Attr_HAS_IONO_CORR);
    };
    //
    // quick'n'dirty:
    int                           idx(0);
    o = observations_.at(idx)->observable(bandKey);
    while (!o && idx<observations_.size())
      o = observations_.at(idx++)->observable(bandKey);
    if (o)
      band->setFrequency(o->getReferenceFrequency());
  };

  //
  //
  logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
    "::getDataFromVgosDb(): collected " + QString().setNum(skyFreqByIfId_.size()) + 
    " ifId<->skyFrq pairs");
 
  //
  // check for fourfit info:
  if (fourfitControlFileByBand.size() && fourfitControlFileByBand.begin().value().size()==0)
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): no fourfit info found for each band, trying to check the data for the "
      "X-band only");

    QVector<QString>            fourfitControlFile, fourfitCommand;
    QVector<int>                numLags;
    QVector<double>             apL;

    if (!vgosDb->loadMiscFourFit("", fourfitControlFile, fourfitCommand, numLags, apL))
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
        "::getDataFromVgosDb(): reading Fourfit Info has failed");
    else
    {
      for (int i=0; i<numOfObs; i++)
      {
        SgVlbiObservation      *obs=observations_.at(i);
        for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
          it!=obs->observableByKey().end(); ++it)
        {
//          SgVlbiObservable       *o=obs->observable("X");
          SgVlbiObservable       *o=it.value();
          if (o)
          {
            o->setFourfitControlFile(fourfitControlFile.at(i));
            o->setFourfitCommandOverride(fourfitCommand.at(i));
            o->setNlags(numLags.at(i));
          };
        };
        obs->setApLength(apL.at(i));
      };
    };
    fourfitControlFile.clear();
    fourfitCommand.clear();
    numLags.clear();
    apL.clear();
  };
  //
  //
  //
  if (dTec && dTecStdDev)
    addAttr(Attr_HAS_DTEC);
  //
  //
  //
  // end of phase 1
  //
  //
  //
  // checks for available "contributions":
  if (calOcean)
    hasOceanTideContrib_ = true;
  if (calPoleTide)
    hasPoleTideContrib_= true;
  if (calEarthTide)
    hasEarthTideContrib_= true;
  if (calWobbleX)
    hasWobblePxContrib_= true;
  if (calWobbleY)
    hasWobblePyContrib_= true;
  if (calWobNut)
    hasWobbleNutContrib_= true;
  if (calEopHiFiXY)
    hasWobbleHighFreqContrib_ = true;
  if (calEopHiFiUt)
    hasUt1HighFreqContrib_ = true;
  if (calTiltRmvr)
    hasTiltRemvrContrib_= true;
  if (calHiFreqLibrationPm)
    hasPxyLibrationContrib_ = true;
  if (calHiFreqLibrationUt)
    hasUt1LibrationContrib_ = true;
  if (calOceanPoleTideLd)
    hasOceanPoleTideContrib_ = true;
  if (calPoleTideOldRestore)
    hasPoleTideOldContrib_= true;
  if (calOceanOld)
    hasOceanTideOldContrib_ = true;
  hasUnPhaseCalContrib_= true;
  for (int i=0; i<listOfBands.size(); i++)
  {
    const QString              &bandKey=listOfBands.at(i);
    hasUnPhaseCalContrib_ = hasUnPhaseCalContrib_ && 
      phaseCals_1ByBand[bandKey]!=NULL && phaseCals_2ByBand[bandKey]!=NULL;
  };
  if (dV_dCipX && dV_dCipY)
    hasCipPartials_ = true;
  else
    hasCipPartials_ = false;
  //
  //
  // clear temporary storages:
  for (int i=0; i<vectors2del.size(); i++)
    if (vectors2del.at(i))
      delete vectors2del.at(i);

  for (int i=0; i<matrices2del.size(); i++)
    if (matrices2del.at(i))
      delete matrices2del.at(i);
  // 
  scanNames.clear();
  scanFullNames.clear();
  ionsBits.clear();
  obs2Scan.clear();
  delUFlags.clear();
  phsUFlags.clear();
  ratUFlags.clear();
  uAcSups.clear();
  
  for (int i=0; i<listOfBands.size(); i++)
  {
    const QString              &bandKey=listOfBands.at(i);
    qCodesByBand[bandKey].clear();
    
    // correlator info stuff:
    fourfitOutputFNameByBand[bandKey].clear();
    fourfitErrorCodesByBand[bandKey].clear();
    baselineCodesByBand[bandKey].clear();
    tapeQualityCodesByBand[bandKey].clear();
    vlb1FileNamesByBand[bandKey].clear();
    tapeIds_1ByBand[bandKey].clear();
    tapeIds_2ByBand[bandKey].clear();
//  corelVersionsByBand[bandKey].clear();
    startOffsetsByBand[bandKey].clear();
    stopOffsetsByBand[bandKey].clear();
    hopsRevisionNumbersByBand[bandKey].clear();
    fourfitVersionsByBand[bandKey].clear();
//  dbeditVersionByBand[bandKey].clear();
    epochsOfCorrelationsByBand[bandKey].clear();
    epochsOfCentralsByBand[bandKey].clear();
    epochsOfFourfitingsByBand[bandKey].clear();
    epochsOfNominalScanTimeByBand[bandKey].clear();
    if (deltaEpochsByBand[bandKey])
      delete deltaEpochsByBand[bandKey];
    if (smplRateByBand[bandKey])
      delete smplRateByBand[bandKey];
    if (sbdResidsByBand[bandKey])
      delete sbdResidsByBand[bandKey];
    if (rateResidsByBand[bandKey])
      delete rateResidsByBand[bandKey];
    if (effDurationsByBand[bandKey])
      delete effDurationsByBand[bandKey];
    if (startSecondsByBand[bandKey])
      delete startSecondsByBand[bandKey];
    if (stopSecondsByBand[bandKey])
      delete stopSecondsByBand[bandKey];
    if (percentOfDiscardsByBand[bandKey])
      delete percentOfDiscardsByBand[bandKey];

    if (uniformityFactorsByBand[bandKey])
      delete uniformityFactorsByBand[bandKey];
    if (geocenterPhaseResidsByBand[bandKey])
      delete geocenterPhaseResidsByBand[bandKey];
    if (geocenterPhaseTotalsByBand[bandKey])
      delete geocenterPhaseTotalsByBand[bandKey];
    if (geocenterSbDelaysByBand[bandKey])
      delete geocenterSbDelaysByBand[bandKey];
    if (geocenterGrDelaysByBand[bandKey])
      delete geocenterGrDelaysByBand[bandKey];
    if (geocenterDelayRatesByBand[bandKey])
      delete geocenterDelayRatesByBand[bandKey];
    if (probOfFalsesByBand[bandKey])
      delete probOfFalsesByBand[bandKey];
    if (corrAprioriDelaysByBand[bandKey])
      delete corrAprioriDelaysByBand[bandKey];
    if (corrAprioriRatesByBand[bandKey])
      delete corrAprioriRatesByBand[bandKey];
    if (corrAprioriAccsByBand[bandKey])
      delete corrAprioriAccsByBand[bandKey];
    if (incohAmpsByBand[bandKey])
      delete incohAmpsByBand[bandKey];
    if (incohAmps2ByBand[bandKey])
      delete incohAmps2ByBand[bandKey];
    if (delayResidsByBand[bandKey])
      delete delayResidsByBand[bandKey];
    if (urvrsByBand[bandKey])
      delete urvrsByBand[bandKey];
    if (instrDelaysByBand[bandKey])
      delete instrDelaysByBand[bandKey];
    if (starElevByBand[bandKey])
      delete starElevByBand[bandKey];
    if (zenithDelaysByBand[bandKey])
      delete zenithDelaysByBand[bandKey];
    if (searchParamsByBand[bandKey])
      delete searchParamsByBand[bandKey];
    if (corrClocksByBand[bandKey])
      delete corrClocksByBand[bandKey];
    if (mDlysByBand[bandKey])
      delete mDlysByBand[bandKey];
    if (mAuxByBand[bandKey])
      delete mAuxByBand[bandKey];
    // EoCIS
    
    fourfitControlFileByBand[bandKey].clear();
    fourfitCommandByBand[bandKey].clear();
    numLagsByBand[bandKey].clear();
    apLengthByBand[bandKey].clear();
    
    numAmbigsByBand[bandKey].clear();
    numSubAmbigsByBand[bandKey].clear();
    numPhsAmbigsByBand[bandKey].clear();
    dataFlagByBand[bandKey].clear();
    ionDataFlagByBand[bandKey].clear();
    if (ambigsByBand[bandKey])
      delete ambigsByBand[bandKey];
    if (correlationByBand[bandKey])
      delete correlationByBand[bandKey];
    if (uvByBand[bandKey])
      delete uvByBand[bandKey];
    if (calFeedCorrByBand[bandKey])
      delete calFeedCorrByBand[bandKey];
    if (snrByBand[bandKey])
      delete snrByBand[bandKey];
    if (sampleRateByBand[bandKey])
      delete sampleRateByBand[bandKey];
    if (residFringeAmplByChanByBand[bandKey])
      delete residFringeAmplByChanByBand[bandKey];
    if (residFringePhseByChanByBand[bandKey])
      delete residFringePhseByChanByBand[bandKey];
    if (refFreqByChanByBand[bandKey])
      delete refFreqByChanByBand[bandKey];
    if (numOfSamplesByChan_USBByBand[bandKey])
      delete numOfSamplesByChan_USBByBand[bandKey];
    if (numOfSamplesByChan_LSBByBand[bandKey])
      delete numOfSamplesByChan_LSBByBand[bandKey];
    if (numOfAccPeriodsByChan_USBByBand[bandKey])
      delete numOfAccPeriodsByChan_USBByBand[bandKey];
    if (numOfAccPeriodsByChan_LSBByBand[bandKey])
      delete numOfAccPeriodsByChan_LSBByBand[bandKey];
    if (loFreqs_1ByBand[bandKey])
      delete loFreqs_1ByBand[bandKey];
    if (loFreqs_2ByBand[bandKey])
      delete loFreqs_2ByBand[bandKey];
    
    if (refFreqsByBand[bandKey])
      delete refFreqsByBand[bandKey];
    if (phasesByBand[bandKey])
      delete phasesByBand[bandKey];
    if (ionCalsByBand[bandKey])
      delete ionCalsByBand[bandKey];
    if (ionSigmasByBand[bandKey])
      delete ionSigmasByBand[bandKey];
    if (phaseCals_1ByBand[bandKey])
      delete phaseCals_1ByBand[bandKey];
    if (phaseCals_2ByBand[bandKey])
      delete phaseCals_2ByBand[bandKey];
    if (ratesByBand[bandKey])
      delete ratesByBand[bandKey];
    if (groupDelaysByBand[bandKey])
      delete groupDelaysByBand[bandKey];
    if (singleBandDelaysByBand[bandKey])
      delete singleBandDelaysByBand[bandKey];
    if (phaseCalFreqs_1ByBand[bandKey])
      delete phaseCalFreqs_1ByBand[bandKey];
    if (phaseCalAmps_1ByBand[bandKey])
      delete phaseCalAmps_1ByBand[bandKey];
    if (phaseCalPhases_1ByBand[bandKey])
      delete phaseCalPhases_1ByBand[bandKey];
    if (phaseCalOffsets_1ByBand[bandKey])
      delete phaseCalOffsets_1ByBand[bandKey];
    if (phaseCalRates_1ByBand[bandKey])
      delete phaseCalRates_1ByBand[bandKey];
    if (phaseCalFreqs_2ByBand[bandKey])
      delete phaseCalFreqs_2ByBand[bandKey];
    if (phaseCalAmps_2ByBand[bandKey])
      delete phaseCalAmps_2ByBand[bandKey];
    if (phaseCalPhases_2ByBand[bandKey])
      delete phaseCalPhases_2ByBand[bandKey];
    if (phaseCalOffsets_2ByBand[bandKey])
      delete phaseCalOffsets_2ByBand[bandKey];
    if (phaseCalRates_2ByBand[bandKey])
      delete phaseCalRates_2ByBand[bandKey];
  };
  listOfBands.clear();
  epochs4Scans.clear();
  epochs4Obs.clear();
  stations_1.clear();
  stations_2.clear();
  sources.clear();
  //
  //
  //
  //
  // Phase two. Collecting station infos:
  bool                          isCalced=isAttr(Attr_HAS_CALC_DATA);
  bool                          hasAuxObs=false;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stInfo = it.value();
    QString                     origin("");
    //
    if (parametersDescriptor_) // can be NULL
    {
      stInfo->setPcClocks(parametersDescriptor_->getClock0());
      stInfo->setPcZenith(parametersDescriptor_->getZenith());
    };
    //
    const QString               stnName(stInfo->getKey());
    QList<SgMJD>                stEpochs;
    if (vgosDb->loadStationEpochs(stnName, stEpochs) && stEpochs.size())
    {
      int                       numOfPts=stEpochs.size();
      int                       numOfTsysIFs=0;
//    QMap<SgMJD, int>          idxByMjd;
      QMap<QString, int>        idxByMjd;
      for (int i=0; i<numOfPts; i++)
        idxByMjd.insert(stEpochs.at(i).toString(SgMJD::F_YYYYMMDDHHMMSSSS), i);
      
      SgVector                 *parAngle=NULL, *refClockOffset=NULL;
      SgMatrix                 *azTheo=NULL, *elTheo=NULL;
      SgMatrix                 *calAxisOffset=NULL;
      SgMatrix                 *calCable=NULL;
      SgMatrix                 *calCblCorrections=NULL;
      SgMatrix                 *calNmfDry=NULL;
      SgMatrix                 *calNmfWet=NULL;
      SgMatrix                 *calHorzOLoad=NULL, *calVertOLoad=NULL;
      SgVector                 *metAtmPres=NULL, *metAtmRH=NULL, *metAtmTemp=NULL;
      SgMatrix                 *partAxisOffset=NULL;
      SgMatrix                 *partTropZenithDry=NULL, *partTropZenithWet=NULL, *partTropGrad=NULL;
      SgMatrix                 *ocnLdR=NULL, *ocnLdV=NULL;
      SgMatrix                 *tSyses=NULL;
      //
      QVector<double>           tsysFreqs;
      QVector<QString>          tsysIds, tsysSbs, tsysPzs;

      // KOMB:
      if (!vgosDb->loadStationRefClockOffset(stnName, refClockOffset))
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
          "::getDataFromVgosDb(): no reference station clock offset data found for the station " + 
          stnName); // it is ok
      // data from CALC:
      if (isCalced && !vgosDb->loadStationAzEl(stnName, azTheo, elTheo))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of azimuth-elevation file failed for the station " + stnName);
      if (isCalced && !vgosDb->loadStationParAngle(stnName, parAngle))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of parallactic angle file failed for the station " + stnName);
      if (isCalced && !vgosDb->loadStationOceanLdDisp(stnName, ocnLdR, ocnLdV))
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of Ocean Loading displacements file failed for the station " + 
          stnName);
      if (isCalced && !vgosDb->loadStationCalAxisOffset(stnName, calAxisOffset))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of axis offset calibrations file failed for the station " + 
          stnName);
      //
      if (isCalced && !vgosDb->loadStationCalSlantPathTropDry(stnName, calNmfDry, kind))
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of hydrostatic troposphere calibrations file failed for the "
          "station " + stnName);
      else if (!kind.isEmpty() && kind.toUpper() != "NMF")
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of hydrostatic troposphere "
          "calibrations for the station " + stnName);
      //
      if (isCalced && !vgosDb->loadStationCalSlantPathTropWet(stnName, calNmfWet, kind))
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of wet troposphere calibrations file failed for the "
          "station " + stnName);
      else if (!kind.isEmpty() && kind.toUpper() != "NMF")
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of wet troposphere "
          "calibrations for the station " + stnName);
      //
      if (isCalced && !vgosDb->loadStationCalOceanLoad(stnName, calHorzOLoad, calVertOLoad))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of ocean loading calibrations file failed for the station " + 
          stnName);
      if (isCalced && !vgosDb->loadStationPartAxisOffset(stnName, partAxisOffset))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of axis offset partials file failed for the station " + 
          stnName);
      //
      if (isCalced && !vgosDb->loadStationPartZenithDelayDry(stnName, partTropZenithDry, kind))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of troposheric zenith delay (hydrostatic component) partials "
          "file failed for the station " + stnName);
      else if (!kind.isEmpty() && kind.toUpper() != "NMF")
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of troposheric zenith delay "
          "(hydrostatic component) partials for the station " + stnName);
      //
      if (isCalced && !vgosDb->loadStationPartZenithDelayWet(stnName, partTropZenithWet, kind))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of troposheric zenith delay (wet component) partials "
          "file failed for the station " + stnName);
      else if (!kind.isEmpty() && kind.toUpper() != "NMF")
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of troposheric zenith delay "
          "(wet component) partials for the station " + stnName);
      //
      if (isCalced && !vgosDb->loadStationPartHorizonGrad(stnName, partTropGrad, kind))
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of troposheric horizontal gradients partials "
          "file failed for the station " + stnName);
      else if (!kind.isEmpty() && kind.toUpper() != "NMF")
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got an unknown, (" + kind + "), kind of troposheric horizontal "
          "gradients partials for the station " + stnName);
      //
      //
      if (calNmfDry)
        hasNdryContrib_= true;
      if (calNmfWet)
        hasNwetContrib_= true;
      if (partTropGrad)
        hasGradPartials_ = true;
      //
      // from logs:
      // cable calibration corrections:
      if (vgosDb->loadStationCalCable(stnName, calCable, origin, kind))
      {
        stInfo->setCableCalsOriginTxt(origin);
        stInfo->addAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL);
        if (kind.isEmpty())
          stInfo->setCableCalsOrigin(SgVlbiStationInfo::CCO_FS_LOG);
        else if (kind.toUpper() == "CDMS")
          stInfo->setCableCalsOrigin(SgVlbiStationInfo::CCO_CDMS);
        else if (kind.toUpper() == "PCMT")
          stInfo->setCableCalsOrigin(SgVlbiStationInfo::CCO_PCMT);
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): the station " + stnName + 
            " has an unknown kind of cable calibration data");
          stInfo->setCableCalsOrigin(SgVlbiStationInfo::CCO_UNDEF);
        };
      }
      else
      {
        stInfo->delAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL);
        stInfo->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of cable calibrations file failed for the station " + stnName);
      };
      //
      // a set of cable cals: from FS log, CDMS and PCMT:
      if (vgosDb->loadStationCalCblCorrections(stnName, calCblCorrections, origin, kind))
      {
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): read a set of cable calibration corrections for the station " + 
          stnName);
      };
/* it is ok:
      else
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of a set of cable calibration corrections failed for "
          "the station " + stnName);
*/
      //
      //
      // meteo data:
      if (vgosDb->loadStationMet(stnName, metAtmPres, metAtmRH, metAtmTemp, origin, kind))
      {
        stInfo->setMeteoDataOriginTxt(origin);
        stInfo->addAttr(SgVlbiStationInfo::Attr_HAS_METEO);
        if (kind.isEmpty())
          stInfo->setMeteoDataOrigin(SgVlbiStationInfo::MDO_FS_LOG);
        else if (kind.toUpper() == "EXTERNAL")
          stInfo->setMeteoDataOrigin(SgVlbiStationInfo::MDO_EXTERNAL);
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): the station " + stnName + 
            " has an unknown kind of meteorological data");
          stInfo->setMeteoDataOrigin(SgVlbiStationInfo::MDO_UNDEF);
        };
      }
      else
      {
        stInfo->delAttr(SgVlbiStationInfo::Attr_HAS_METEO);
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of Meteo file failed for the station " + stnName);
      };
      // tsyses:
      if (vgosDb->loadStationTsys(stnName, tSyses, tsysFreqs, tsysIds, tsysSbs, tsysPzs))
      {
        numOfTsysIFs = tSyses->nCol();
        if (numOfPts != (int)tSyses->nRow())
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): tsys data dimension mismatch for the station " + stnName);
          numOfTsysIFs = 0;
        };
        if (numOfTsysIFs != tsysFreqs.size())
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): tsys freqs dimension mismatch for the station " + stnName);
          numOfTsysIFs = 0;
        };
        if (numOfTsysIFs != tsysIds.size())
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): tsys ids dimension mismatch for the station " + stnName);
          numOfTsysIFs = 0;
        };
        if (numOfTsysIFs != tsysSbs.size())
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): tsys isbs dimension mismatch for the station " + stnName);
          numOfTsysIFs = 0;
        };
        if (numOfTsysIFs != tsysPzs.size())
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): tsys pzs dimension mismatch for the station " + stnName);
          numOfTsysIFs = 0;
        };
        if (numOfTsysIFs)
        {
          stInfo->tsysIfFreqs().resize(numOfTsysIFs);
          stInfo->tsysIfIds().resize(numOfTsysIFs);
          stInfo->tsysIfSideBands().resize(numOfTsysIFs);
          stInfo->tsysIfPolarizations().resize(numOfTsysIFs);
          for (int j=0; j<numOfTsysIFs; j++)
          {
            stInfo->tsysIfFreqs()[j] = tsysFreqs[j];
            stInfo->tsysIfIds()[j] = tsysIds[j];
            stInfo->tsysIfSideBands()[j] = tsysSbs[j];
            stInfo->tsysIfPolarizations()[j] = tsysPzs[j];
          }
          stInfo->addAttr(SgVlbiStationInfo::Attr_HAS_TSYS);
        };
      }
      else
      {
        stInfo->delAttr(SgVlbiStationInfo::Attr_HAS_TSYS);
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): reading of tsys file failed for the station " + stnName);
      };
      //
      // clear this attributes:
      stInfo->delAttr(SgVlbiStationInfo::Attr_CABLE_CAL_MODIFIED);
      stInfo->delAttr(SgVlbiStationInfo::Attr_METEO_MODIFIED);
      stInfo->delAttr(SgVlbiStationInfo::Attr_TSYS_MODIFIED);
      //
      QMap<QString, SgVlbiAuxObservation*> 
                               *auxObsByScan=stInfo->auxObservationByScanId();
      QMap<QString, SgVlbiAuxObservation*>::iterator
                                jt=auxObsByScan->begin();
      for (; jt!=auxObsByScan->end(); ++jt)
      {
        SgVlbiAuxObservation   *auxObs = jt.value();
//      if (idxByMjd.contains(*auxObs))
        if (idxByMjd.contains(auxObs->toString(SgMJD::F_YYYYMMDDHHMMSSSS)))
        {
//        int                   idx=idxByMjd.value(*auxObs);
          int                   idx=idxByMjd.value(auxObs->toString(SgMJD::F_YYYYMMDDHHMMSSSS));
          if (azTheo && elTheo)
          {
            auxObs->setAzimuthAngle       (azTheo->getElement(idx, 0));
            auxObs->setAzimuthAngleRate   (azTheo->getElement(idx, 1));
            auxObs->setElevationAngle     (elTheo->getElement(idx, 0));
            auxObs->setElevationAngleRate (elTheo->getElement(idx, 1));
          };
          if (parAngle)
            auxObs->setParallacticAngle(parAngle->getElement(idx));
          if (refClockOffset)
            auxObs->setRefClockOffset(refClockOffset->getElement(idx));
          if (calAxisOffset)
          {
            auxObs->setCalcAxisOffset4Delay(calAxisOffset->getElement(idx, 0));
            auxObs->setCalcAxisOffset4Rate (calAxisOffset->getElement(idx, 1));
          };
          if (calNmfDry)
          {
            auxObs->setCalcNdryCont4Delay(calNmfDry->getElement(idx, 0));
            auxObs->setCalcNdryCont4Rate (calNmfDry->getElement(idx, 1));
          };
          if (calNmfWet)
          {
            auxObs->setCalcNwetCont4Delay(calNmfWet->getElement(idx, 0));
            auxObs->setCalcNwetCont4Rate (calNmfWet->getElement(idx, 1));
          };
          if (calHorzOLoad && calVertOLoad)
          {
            auxObs->setCalcOLoadHorz4Delay(calHorzOLoad->getElement(idx, 0));
            auxObs->setCalcOLoadHorz4Rate (calHorzOLoad->getElement(idx, 1));
            auxObs->setCalcOLoadVert4Delay(calVertOLoad->getElement(idx, 0));
            auxObs->setCalcOLoadVert4Rate (calVertOLoad->getElement(idx, 1));
          };
          if (partAxisOffset)
          {
            auxObs->setDdel_dAxsOfs(partAxisOffset->getElement(idx, 0));
            auxObs->setDrat_dAxsOfs(partAxisOffset->getElement(idx, 1));
          };
          if (partTropZenithDry)
          {
            auxObs->setDdel_dTzdDry(partTropZenithDry->getElement(idx, 0));
            auxObs->setDrat_dTzdDry(partTropZenithDry->getElement(idx, 1));
          };
          if (partTropZenithWet)
          {
            auxObs->setDdel_dTzdWet(partTropZenithWet->getElement(idx, 0));
            auxObs->setDrat_dTzdWet(partTropZenithWet->getElement(idx, 1));
          };
          if (partTropGrad)
          {
            auxObs->setDdel_dTzdGrdN (partTropGrad->getElement(idx, 0));
            auxObs->setDdel_dTzdGrdE (partTropGrad->getElement(idx, 1));
            auxObs->setDrat_dTzdGrdN (partTropGrad->getElement(idx, 2));
            auxObs->setDrat_dTzdGrdE (partTropGrad->getElement(idx, 3));
          };
          //
          if (calCable)
          {
            auxObs->setCableCalibration(calCable->getElement(idx, 0));
            auxObs->cableCorrections().setElement(SgVlbiAuxObservation::CCT_DFLT,
              calCable->getElement(idx, 0));
            hasAuxObs = true;
          }
          else
            auxObs->addAttr(SgVlbiAuxObservation::Attr_CABLE_CAL_BAD);
          if (calCblCorrections)
          {
            auxObs->cableCorrections().setElement(SgVlbiAuxObservation::CCT_FSLG,
              calCblCorrections->getElement(idx, SgVlbiAuxObservation::CCT_FSLG));
            auxObs->cableCorrections().setElement(SgVlbiAuxObservation::CCT_CDMS,
              calCblCorrections->getElement(idx, SgVlbiAuxObservation::CCT_CDMS));
            auxObs->cableCorrections().setElement(SgVlbiAuxObservation::CCT_PCMT,
              calCblCorrections->getElement(idx, SgVlbiAuxObservation::CCT_PCMT));
          };
          if (metAtmPres && metAtmRH && metAtmTemp)
          {
            auxObs->setMeteoData(SgMeteoData(metAtmTemp->getElement(idx),
                                              metAtmPres->getElement(idx), metAtmRH->getElement(idx)));
            auxObs->meteoData().delAttr(SgMeteoData::Attr_ARTIFICIAL_DATA);
            hasAuxObs = true;
          };
          if (tSyses && numOfTsysIFs)
          {
            auxObs->setTsysesSize(numOfTsysIFs);
            for (int j=0; j<numOfTsysIFs; j++) 
              auxObs->tSyses()->setElement(j, tSyses->getElement(idx, j));
          };
          if (ocnLdR && ocnLdV)
          {
            auxObs->setOcnLdR( Sg3dVector(ocnLdR->getElement(idx,0), 
                                          ocnLdR->getElement(idx,1), 
                                          ocnLdR->getElement(idx,2)));
            auxObs->setOcnLdV( Sg3dVector(ocnLdV->getElement(idx,0), 
                                          ocnLdV->getElement(idx,1), 
                                          ocnLdV->getElement(idx,2)));
          };
          //
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): cannot find epoch " + auxObs->toString() + 
            " in the block of station info for the station " + stnName);
      };
      stInfo->checkPresenceOfCableCalibratioCorrections();
      // free allocated spaces:
      if (azTheo) 
        delete azTheo;
      if (elTheo)
        delete elTheo;
      if (parAngle)
        delete parAngle;
      if (refClockOffset)
        delete refClockOffset;
      if (calAxisOffset)
        delete calAxisOffset;
      if (calCblCorrections)
        delete calCblCorrections;
      if (calNmfDry)
        delete calNmfDry;
      if (calNmfWet)
        delete calNmfWet;
      if (calHorzOLoad)
        delete calHorzOLoad;
      if (calVertOLoad)
        delete calVertOLoad;
      if (metAtmPres)
        delete metAtmPres;
      if (metAtmRH)
        delete metAtmRH;
      if (metAtmTemp)
        delete metAtmTemp;
      if (partAxisOffset)
        delete partAxisOffset;
      if (ocnLdR)
        delete ocnLdR;
      if (ocnLdV)
        delete ocnLdV;
      if (partTropZenithDry)
        delete partTropZenithDry;
      if (partTropZenithWet)
        delete partTropZenithWet;
      if (partTropGrad)
        delete partTropGrad;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): cannot load time table for the station " + stnName + ", skipped");
  };
  if (hasAuxObs)
    addAttr(Attr_HAS_AUX_OBS);
  //
  //
  //
  //
  // Phase three. Collecting a priori for sourses, stations:
  QVector<QString>              sourcesNames, sourcesApRefs, eccTypes, eccNums;
  QVector<QString>              stationsNames4Coo, stationsNames4Ant, stationsNames4Ecc, 
                                stationsNames4AprioriClocks, tectonicPlateNames;
  QVector<int>                  axisTypes;
  SgVector                     *axisOffsets=NULL;
  SgMatrix                     *srcCoo=NULL, *axisTilts=NULL, *eccVals=NULL, *stnCoo=NULL, 
                               *offsets_n_rates=NULL;


  if (!vgosDb->loadSourcesApriories(sourcesNames, srcCoo, sourcesApRefs))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: a priori: sources");
//    isOk = false;
  };
  if (!vgosDb->loadStationsApriories(stationsNames4Coo, stnCoo, tectonicPlateNames))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: a priori: stations");
//    isOk = false;
  };
  if (!vgosDb->loadAntennaApriories(stationsNames4Ant, axisTypes, axisOffsets, axisTilts))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::getDataFromVgosDb(): limited use data, missed info: a priori: antennae");
//    isOk = false;
  };
  vgosDb->loadEccentricities(stationsNames4Ecc, eccTypes, eccNums, eccVals);
  vgosDb->loadClockApriories(stationsNames4AprioriClocks, offsets_n_rates);

 
  //
  // sources:
  //
  if (sourcesNames.size() && srcCoo)
  {
    if (sourcesNames.size()!=sourcesByName_.size())
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): the number of a priori source positions " +
        QString().sprintf("(%d) is not equal to the actual number of sources (%d)",
          sourcesNames.size(), sourcesByName_.size()));
    for (int i=0; i<sourcesNames.size(); i++)
    {
      sourceName = sourcesNames.at(i);
      if (sourcesByName_.contains(sourceName))
      {
        sourceInfo = sourcesByName_.value(sourceName);
        double ra=srcCoo->getElement(i, 0);
        double dn=srcCoo->getElement(i, 1);
        sourceInfo->setRA(ra);
        sourceInfo->setDN(dn);
        if (sourcesApRefs.size() == sourcesNames.size())
          sourceInfo->setAprioriReference(sourcesApRefs.at(i));
      }
      else // complain:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): cannot find a priori entry '" + sourceName + 
          "' in the list of session sources");
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): cannot find sources a priori coordinates in the vgosDb tree");
  //
  // stations:
  // here we have three lists and the order of stations could be different
  // first, coordinates:
  if (stationsNames4Coo.size() && stnCoo && tectonicPlateNames.size())
  {
    if (stationsNames4Coo.size()!=stationsByName_.size())
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): the number of a priori stations positions " +
        QString().sprintf("(%d) is not equal to the actual number of stations (%d)",
          stationsNames4Coo.size(), stationsByName_.size()));
    calcInfo_.stations().clear();
    for (int i=0; i<stationsNames4Coo.size(); i++)
    {
      station1Name = stationsNames4Coo.at(i);
      if (stationsByName_.contains(station1Name))
      {
        station1Info = stationsByName_.value(station1Name);
        Sg3dVector              r(v3Zero);
        r(X_AXIS) = stnCoo->getElement(i, 0);
        r(Y_AXIS) = stnCoo->getElement(i, 1);
        r(Z_AXIS) = stnCoo->getElement(i, 2);
        station1Info->setR(r);
        station1Info->setTectonicPlateName(tectonicPlateNames.at(i));
        // set up the order:
        calcInfo_.stations().append(station1Info);
      }
      else // complain:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): cannot find station '" + station1Name + "' in the list of stations");
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): cannot find stations a priori coordinates in the vgosDb tree");
  //
  // then, antenna parameters:
  if (stationsNames4Ant.size() && axisTypes.size() && axisOffsets)
  {
    if (stationsNames4Ant.size()!=stationsByName_.size())
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): the number of a priori antenna parameters " +
        QString().sprintf("(%d) is not equal to the actual number of stations (%d)",
          stationsNames4Ant.size(), stationsByName_.size()));
    for (int i=0; i<stationsNames4Ant.size(); i++)
    {
      station1Name = stationsNames4Ant.at(i);
      if (stationsByName_.contains(station1Name))
      {
        station1Info = stationsByName_.value(station1Name);
        SgVlbiStationInfo::MountingType mnt = SgVlbiStationInfo::MT_UNKN;
        Sg3dVector                    r(v3Zero);
        //
        // axis offset type:
        if (axisTypes.size())
        {
          mnt = SgVlbiStationInfo::int2mntType(axisTypes.at(i));
          if (mnt != SgVlbiStationInfo::MT_UNKN)
          {
            station1Info->setMntType(mnt);
            logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): set up mounting type [" + sMntTypes[mnt] + "] for the antenna " + 
              station1Info->getKey());
          };
        };
        // axis offset:
        if (axisOffsets)
        {
          station1Info->setAxisOffset(axisOffsets->getElement(i));
          logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): the value of axis offset for the antenna " +
            station1Info->getKey() + " has been set to " + 
            QString("").setNum(station1Info->getAxisOffset()));
        };
      }
      else // complain:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): cannot find station '" + station1Name + "' in the list of stations");
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): cannot find antenna a priori parameters in the vgosDb tree");
  //
  //
  // and eccentricities:
  if (stationsNames4Ecc.size() && eccTypes.size() && eccNums.size() && eccVals)
  {
    for (int i=0; i<stationsNames4Ecc.size(); i++)
    {
      station1Name = stationsNames4Ecc.at(i);
      if (stationsByName_.contains(station1Name))
      {
        station1Info = stationsByName_.value(station1Name);
        if (station1Info)
        {
          station1Info->eccRec() = new SgEccRec();
          if (eccTypes.at(i) == "XY")
            station1Info->eccRec()->setEccType(SgEccRec::ET_XYZ);
          else if (eccTypes.at(i) == "NE")
            station1Info->eccRec()->setEccType(SgEccRec::ET_NEU);
          else
          {
            station1Info->eccRec()->setEccType(SgEccRec::ET_N_A);
            logger->write(SgLogger::ERR, SgLogger::IO, className() +
              "::getDataFromVgosDb(): cannot figure out Ecc type from the string \"" + 
              eccTypes.at(i) +  "\" for the station \"" +  station1Info->getKey() + "\"");
          };
          //
          station1Info->eccRec()->setDR(
            Sg3dVector(eccVals->getElement(i, 0), eccVals->getElement(i, 1), eccVals->getElement(i, 2)));
          station1Info->eccRec()->setCdpNumber(eccNums.at(i));
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::getDataFromVgosDb(): parsing ecc: the station # " + QString("").setNum(i) + " is NULL");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::getDataFromVgosDb(): cannot find a station \"" + station1Name + "\" in the map");
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): cannot find eccentricities in the vgosDb tree");
  //
  // a priori clocks:
  if (stationsNames4AprioriClocks.size() && offsets_n_rates)
  {
    for (int i=0; i<stationsNames4AprioriClocks.size(); i++)
    {
      station1Name = stationsNames4AprioriClocks.at(i);
      if (stationsByName_.contains(station1Name))
      {
        station1Info = stationsByName_.value(station1Name);
        station1Info->setAPrioriClockTerm_0(offsets_n_rates->getElement(i, 0));
        station1Info->setAPrioriClockTerm_1(offsets_n_rates->getElement(i, 1));
        station1Info->setNeed2useAPrioriClocks(true);
      }
      else // complain:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): cannot find station \"" + station1Name + 
          "\" in the list of stations for a priori clocks");
    };
  };


  bool                          hasTidalUt1;
//int                           tidalUt1(0);
//hasTidalUt1 = vgosDb->loadCalcInfo(tidalUt1, calcVersionValue);
//calcInfo_.setDversion(calcVersionValue);
  hasTidalUt1 = vgosDb->loadCalcInfo(calcInfo_);
  //
  //
  //
  eccTypes.clear();
  eccNums.clear();
  axisTypes.clear();
  stationsNames4AprioriClocks.clear();
  if (axisOffsets)
    delete axisOffsets;
  if (srcCoo)
    delete srcCoo;
  if (axisTilts)
    delete axisTilts;
  if (eccVals)
    delete eccVals;
  if (stnCoo)
    delete stnCoo;
  if (offsets_n_rates)
    delete offsets_n_rates;
  //
  //
  //
  //
  // Phase four. Solve-related parameters:
  // atm setup:
  SgVector                     *atmInterval=NULL, *atmRateConstraint=NULL;
  QVector<QString>              atmStationsNames;
  // clock setup:
  SgVector                     *clkInterval=NULL, *clkRateConstraint=NULL;
  QList<QString>                refClocks;
  QVector<QString>              clockStationsNames;
  // EOP:
  SgVector                     *pmArrayInfo=NULL, *utArrayInfo=NULL, *utValues=NULL;
  SgMatrix                     *pmValues=NULL;
  QString                       sUtMode, sUtModule, sUtOrigin, sPmMode, sPmModule, sPmOrigin;
  // calibrations:
  int                           obsCalFlags(31);
  QVector<int>                  statCalFlags;
  QVector< QVector<int> >       flybyFlags;
  QVector<QString>              statCalNames, flybyNames, calSiteNames, obsCalNames, blwNames;
  // selection status:
  QVector<int>                  sourcesSelection;
  QVector< QVector<int> >       baselinesSelection;
  // additional weights:
  SgMatrix                     *groupBlWeights=NULL;
  // calc info:
  int                           leapSecond=0;
  // clock breaks:
  QVector<QString>              cbNames;
  QVector<SgMJD>                cbEpochs;
  QVector<int>                  cbFlags;
  // cable signs:
  QMap<QString, QString>        cableSignByKey;
  bool                          hasLeapSecond;
  //
  // load data:
  // 
  vgosDb->loadAtmSetup(atmInterval, atmRateConstraint, atmStationsNames);
  vgosDb->loadClockSetup(refClocks, clkInterval, clkRateConstraint, clockStationsNames);
  vgosDb->loadCalcEop(pmArrayInfo, utArrayInfo, pmValues, utValues, sUtMode, sUtModule, sUtOrigin,
                      sPmMode, sPmModule, sPmOrigin);
  vgosDb->loadCalibrations(obsCalFlags, statCalFlags, flybyFlags, statCalNames, flybyNames, 
                            calSiteNames, obsCalNames);
  vgosDb->loadSelectionStatus(sourcesSelection, baselinesSelection);
  vgosDb->loadSolveWeights(blwNames, groupBlWeights);
  vgosDb->loadClockBreaks(cbNames, cbEpochs, cbFlags);
  hasLeapSecond = vgosDb->loadLeapSecond(leapSecond);
  vgosDb->loadMiscCable(cableSignByKey);
  
  
  if (atmInterval && atmRateConstraint)
    addAttr(Attr_PRE_PROCESSED);
    
  //
  // update the session:
  //
  if (parametersDescriptor_)
  {
    QString                   siteName("");
    //
    // clocks:
    if (clkInterval && clkRateConstraint)
    {
      SgParameterCfg            pCfg=parametersDescriptor_->getClock0();
      if (clkInterval->n()==1)
      {
        pCfg.setPwlStep(clkInterval->getElement(0)/24.0); // hr->day
        parametersDescriptor_->setClock0(pCfg);
      }
      else 
        for (int i=0; i<clockStationsNames.size(); i++)
        {
          siteName = clockStationsNames.at(i);
          if (siteName.size() < 8)
            siteName.sprintf("%-8s", qPrintable(siteName));
          if (stationsByName_.contains(siteName))
          {
            SgVlbiStationInfo  *si=stationsByName_.value(siteName);
            si->pcClocks()->setPwlStep(clkInterval->getElement(i)/24.0); // hr->day
            if (fabs(parametersDescriptor_->getClock0().getPwlStep() - 
              si->pcClocks()->getPwlStep()) > 1.0e-3)
              si->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): cannot find siteName \"" + siteName + 
              "\" in the map of stations");
        };
      //
      if (clkRateConstraint->n()==1)
      {
        pCfg.setPwlAPriori(clkRateConstraint->getElement(0)*864.0);
        parametersDescriptor_->setClock0(pCfg);
      }
      else 
        for (int i=0; i<clockStationsNames.size(); i++)
        {
          siteName = clockStationsNames.at(i);
          if (siteName.size() < 8)
            siteName.sprintf("%-8s", qPrintable(siteName));
          if (stationsByName_.contains(siteName))
          {
            SgVlbiStationInfo  *si=stationsByName_.value(siteName);
            si->pcClocks()->setPwlAPriori(clkRateConstraint->getElement(i)*864.0);
            if (fabs(parametersDescriptor_->getClock0().getPwlAPriori() - 
              si->pcClocks()->getPwlAPriori()) > 1.0e-3)
              si->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): cannot find siteName \"" + siteName + 
              "\" in the map of stations");
        };
    }
    else
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): no clock interval or clock rate constraint found");
    //
    // zenith:
    if (atmInterval && atmRateConstraint)
    {
      SgParameterCfg            pCfg=parametersDescriptor_->getZenith();
      if (atmInterval->n()==1)
      {
        pCfg.setPwlStep(atmInterval->getElement(0)/24.0); // hr->day
        parametersDescriptor_->setZenith(pCfg);
      }
      else 
        for (int i=0; i<atmStationsNames.size(); i++)
        {
          siteName = atmStationsNames.at(i);
          if (siteName.size() < 8)
            siteName.sprintf("%-8s", qPrintable(siteName));
          if (stationsByName_.contains(siteName))
          {
            SgVlbiStationInfo  *si=stationsByName_.value(siteName);
            si->pcZenith()->setPwlStep(atmInterval->getElement(i)/24.0); // hr->day
            if (fabs(parametersDescriptor_->getZenith().getPwlStep() - 
              si->pcZenith()->getPwlStep()) > 1.0e-3)
              si->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): cannot find siteName \"" + siteName + 
              "\" in the map of stations");
        };
      //
      if (atmRateConstraint->n()==1)
      {
        pCfg.setPwlAPriori(atmRateConstraint->getElement(0)*1.0e-12*24.0*vLight*100.0);
        parametersDescriptor_->setZenith(pCfg);
      }
      else 
        for (int i=0; i<atmStationsNames.size(); i++)
        {
          siteName = atmStationsNames.at(i);
          if (siteName.size() < 8)
            siteName.sprintf("%-8s", qPrintable(siteName));
          if (stationsByName_.contains(siteName))
          {
            SgVlbiStationInfo  *si=stationsByName_.value(siteName);
            si->pcZenith()->setPwlAPriori(atmRateConstraint->getElement(i)*1.0e-12*24.0*vLight*100.0);
            if (fabs(parametersDescriptor_->getZenith().getPwlAPriori() - 
              si->pcZenith()->getPwlAPriori()) > 1.0e-3)
              si->addAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): cannot find siteName \"" + siteName + 
              "\" in the map of stations");
        };
    }
    else
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): no atm interval or atm rate constraint found");
  };

  
/*
  if (atmInterval && atmRateConstraint && atmInterval->n()==atmRateConstraint->n() 
                  && atmInterval->n()==1)
  {
    if (parametersDescriptor_)
    {
      SgParameterCfg            pCfg=parametersDescriptor_->getZenith();
      pCfg.setPwlAPriori(atmRateConstraint->getElement(0)*(1.0e-12*24.0*vLight*100.0));
      pCfg.setPwlStep(atmInterval->getElement(0)/24.0); // hr->day
      parametersDescriptor_->setZenith(pCfg);
    };
  };
*/
/*
  if (parametersDescriptor_ && 
      clkInterval && clkRateConstraint && 
      clkInterval->n()==clkRateConstraint->n() && 
      clkInterval->n()==1)
  {
    SgParameterCfg              pCfg=parametersDescriptor_->getClock0();
    pCfg.setPwlAPriori(clkRateConstraint->getElement(0)*864.0);
    pCfg.setPwlStep(clkInterval->getElement(0)/24.0); // hr->day
    parametersDescriptor_->setClock0(pCfg);
  };
*/
  
  //
  // set up a reference clock site, if available:
  bool                          hasRC(false);
  if (refClocks.size())
  {
    for (int i=0; i<refClocks.size(); i++)
    {
      QString                   clockSiteName(refClocks.at(i));
      if (clockSiteName.size()<8)
        clockSiteName.sprintf("%-8s", qPrintable(refClocks.at(i)));
      if (stationsByName_.contains(clockSiteName))
      {
        stationsByName_.value(clockSiteName)->addAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
        hasRC = true;
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): Reference Clock attribute has been assigned to the station " +
          stationsByName_.value(clockSiteName)->getKey());
      };
    };
  };
  if (!hasRC)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): no Reference Clock attribute has been found for the session");
  // reference clock site finished.
  //
  // EOP, UT1:
  if (utArrayInfo && utValues)
  {
    double                      t0=utArrayInfo->getElement(0);
    double                      dt=utArrayInfo->getElement(1);
    unsigned int                numOfPts=round(utArrayInfo->getElement(2));
    if (t0 > 2390000.0)
      t0 -= 2400000.5;
    if (utValues->n() < numOfPts)
      numOfPts = utValues->n();
    allocUt1InterpolEpochs(t0, dt, numOfPts);
    *utValues *= -1.0;
    allocUt1InterpolValues(utValues);
    hasUt1Interpolation_ = true;
    calcInfo_.ut1Interpolation().setControlFlag(sUtMode);
    calcInfo_.ut1Interpolation().setDefinition(sUtModule);
    calcInfo_.ut1Interpolation().setOrigin(sUtOrigin);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): loaded " + QString().setNum(numOfPts) + 
      " records for UT1 interpolation");
  }
  else
  {
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
    hasUt1Interpolation_ = false;
  };
  // EOP, PM:
  if (pmArrayInfo && pmValues)
  {
    double                      t0=pmArrayInfo->getElement(0);
    double                      dt=pmArrayInfo->getElement(1);
    unsigned int                numOfPts=round(pmArrayInfo->getElement(2));
    if (t0 > 2390000.0)
      t0 -= 2400000.5;
    if (pmValues->nRow() < numOfPts)
      numOfPts = pmValues->nRow();
    allocPxyInterpolEpochs(t0, dt, numOfPts);
    allocPxyInterpolValues(pmValues);
    hasPxyInterpolation_ = true;
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): loaded " + QString().setNum(numOfPts) + 
      " records for polar motion interpolation");

    calcInfo_.polarMotionInterpolation().setControlFlag(sPmMode);
    calcInfo_.polarMotionInterpolation().setDefinition(sPmModule);
    calcInfo_.polarMotionInterpolation().setOrigin(sPmOrigin);
  }
  else
  {
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
    hasPxyInterpolation_ = false;
  };
  
  if (hasTidalUt1)
  {
    QString ut1Type("");
    switch (calcInfo_.getFlagTidalUt1())
    {
      case 1:
        tabsUt1Type_ = SgTidalUt1::CT_FULL;
        ut1Type = "Seriesfull of tides";
      break;
      case -1: // UT1R
        tabsUt1Type_ = SgTidalUt1::CT_SHORT_TERMS_REMOVED;
        ut1Type = "Only short period tidal terms are removed";
      break;
      case -2: // UT1S
        tabsUt1Type_ = SgTidalUt1::CT_ALL_TERMS_REMOVED;
        ut1Type = "All tidal terms are removed";
      break;
      default:
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got undocumented value for the TIDALUT1 value: " + 
          QString().setNum(calcInfo_.getFlagTidalUt1()) + "; the UT1 type has been reverted to default");
      break;
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): the type of the UT1 inner series has been set to " + ut1Type);
  };
  //
  // calibrations:
  if (obsCalNames.size() && config_)
  {
    QList<QString>            calList;
    for (int i=0; i<obsCalNames.size(); i++)
      calList << obsCalNames.at(i);
    applyObsCalibrationSetup(obsCalFlags, calList);
    calList.clear();
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): the observation calibration list is empty");
  //
  // flags for cable calibrations (is available):
  if (statCalNames.size() && calSiteNames.size() && statCalFlags.size())
  {
    bool                        isCableCalMatched(false);
    int                         cableCalFlagIdx(-1);
    for (int i=0; i<statCalNames.size(); i++)
      if (statCalNames.at(i) == "CABL DEL")
      {
        isCableCalMatched = true;
        cableCalFlagIdx = i;
      };
    if (isCableCalMatched)
    {
      QString         str;
      // use order of stations from calSiteNames list:
      for (int i=0; i<calSiteNames.size(); i++)
      {
        str = calSiteNames.at(i);
        if (stationsByName_.contains(str))
        {
          SgVlbiStationInfo    *si=stationsByName_.value(str);
          // just alternate cable cal flag:
          int                   nFlag=statCalFlags.at(i);
          if (nFlag & 1<<cableCalFlagIdx)
            si->delAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
          else
            si->addAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): cannot find " + str + 
            " (an entry from CalSiteName variable) in the list of stations");
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): there is no \"CABL DEL\" flag in the list of calibrations");
  };
  //
  //
  // sources selection status:
  if (sourcesSelection.size() && sourcesNames.size())
  {
    if (sourcesSelection.size() == sourcesNames.size())
    {
      int                         bitArray;
      SgVlbiSourceInfo           *si;
      for (int i=0; i<sourcesNames.size(); i++)
      {
        sourceName = sourcesNames.at(i);
        if (sourcesByName_.contains(sourceName))
        {
          si = sourcesByName_.value(sourceName);
          bitArray = sourcesSelection.at(i);
          si->applyStatusBits(bitArray);
        };
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): sourcesSelection and sourcesNames size mismatch");
  };
  //
  // baselines selection status:
  if (baselinesSelection.size() && stationsNames4Coo.size())
  {
    if (baselinesSelection.size() == stationsNames4Coo.size())
    {
      int                         bitArray;
      SgVlbiBaselineInfo         *bi, *biR;
      for (int i=0; i<stationsNames4Coo.size(); i++)
      {
        station1Name = stationsNames4Coo.at(i);
        if (stationsByName_.contains(station1Name))
        {
          for (int j=0; j<stationsNames4Coo.size(); j++)
          {
            station2Name = stationsNames4Coo.at(j);
            if (stationsByName_.contains(station2Name))
            {
// ???????????
//              bitArray = baselinesSelection.at(j).at(i);
              bitArray = baselinesSelection.at(i).at(j);
              //                image->getI2(dBaselinesSelStatus_, dSiteNames_->dim2()*j + i,0,0);
              if (baselinesByName_.contains(station1Name + ":" + station2Name))
                bi = baselinesByName_.value(station1Name + ":" + station2Name);
              else
                bi = NULL;
              if (baselinesByName_.contains (station2Name + ":" + station1Name))
                biR = baselinesByName_.value(station2Name + ":" + station1Name);
              else
                biR = NULL;
              // check for deselect flag:
              if (!(bitArray & (1<<1)))
              {
                if (bi && !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
                {
                  bi ->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
                  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                    "::getDataFromVgosDb(): the baseline " + bi->getKey() + " has been deselected");
                };
                if (biR && !biR->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
                {
                  biR->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
                  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                    "::getDataFromVgosDb(): the baseline " + biR->getKey() + " has been deselected");
                };
              };
              // check for baseline clocks flag:
              if ((bitArray & (1<<3)))
              {
                if (bi)
                {
                  bi ->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
                  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                    "::getDataFromVgosDb(): clock estimation has been assigned to the baseline " + 
                    bi->getKey());
                };
                if (biR)
                {
                  biR->addAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
                  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
                    "::getDataFromVgosDb(): clock estimation has been assigned to the baseline " + 
                    biR->getKey());
                };
              };
            };
          };
        };
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): baselinesSelection and stationsNames size mismatch");
  };
  //
  // weight corrections:
  if (groupBlWeights && blwNames.size() && groupBlWeights->nCol()==(unsigned)blwNames.size())
  {
    SgVlbiBaselineInfo         *bi;
    for (int i=0; i<blwNames.size(); i++)
    {
      const QString            &st=blwNames.at(i);
      int                       idx=st.indexOf(':');
      if (idx>0)
      {
        QString st1 = st.left(idx);
        QString st2 = st.mid(idx + 1);
        if (baselinesByName_.contains(blwNames.at(i)))
        {
          bi = baselinesByName_.value(blwNames.at(i));
          bi->setSigma2add(DT_DELAY, groupBlWeights->getElement(0, i));
          bi->setSigma2add(DT_RATE,  groupBlWeights->getElement(1, i));
        }
        else if (baselinesByName_.contains(st2 + ":" + st1)) // reverted?
        {
          bi = baselinesByName_.value(st2 + ":" + st1);
          bi->setSigma2add(DT_DELAY, groupBlWeights->getElement(0, i));
          bi->setSigma2add(DT_RATE,  groupBlWeights->getElement(1, i));
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): the list of baseline weights contains a reverted name: [" + 
            blwNames.at(i) + "]");
        }
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): cannot find baseline [" + blwNames.at(i) +
            "] in the map of baselines; supposed weight corrections are " +
            QString("").sprintf("%.1fps and %.1fE-14s/s", 
            groupBlWeights->getElement(0, i)*1.0e12, groupBlWeights->getElement(1, i)*1.0e14));
        }
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): malformed name of a baseline: \"" + st + "\",  skipped");
    };
    addAttr(Attr_HAS_WEIGHTS);
  }
  else if (config_)
  {
    for (BaselinesByName_it ij=baselinesByName_.begin(); ij!=baselinesByName_.end(); ++ij)
    {
      ij.value()->setSigma2add(DT_DELAY, config_->getInitAuxSigma4Delay());
      ij.value()->setSigma2add(DT_RATE,  config_->getInitAuxSigma4Rate());
    };
  };
  //
  // clock breaks:
  if (cbNames.size() && cbEpochs.size() && cbFlags.size())
  {
    int                         numOfClockBreaks;
    numOfClockBreaks = cbNames.size();
    if (numOfClockBreaks != cbEpochs.size())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::getDataFromVgosDb(): the number of clock breaks sites (" + 
        QString("").setNum(numOfClockBreaks) + ") is not equal to the number of clock break epochs " +
        QString("").setNum(cbEpochs.size()));
      numOfClockBreaks = std::min(numOfClockBreaks, cbEpochs.size());
    };
    QString                     stnName("");
    for (int i=0; i<numOfClockBreaks; i++)
    {
      const SgMJD              &tBreak=cbEpochs.at(i);
      SgVlbiStationInfo        *stn2Correct=NULL;
      stnName = cbNames.at(i);
      if (stnName.size() < 8)
      {
        stnName = cbNames.at(i).leftJustified(8, ' ');
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): a name of station \"" + cbNames.at(i) + 
          "\" from the ClockBreak file has been adjusted to \"" + stnName + "\"");
      };
      // add a clock break, set it to estimate in a common solution:
      if (stationsByName_.contains(stnName))
      {
        stn2Correct = stationsByName_.value(stnName);
        SgParameterBreak         *clockBreak=new SgParameterBreak(tBreak, 0.0);
        clockBreak->addAttr(SgParameterBreak::Attr_DYNAMIC);
        if (stn2Correct->clockBreaks().addBreak(clockBreak))
          logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): a clock break at station " + stn2Correct->getKey() + 
            " that occurred on " + tBreak.toString() + " has been added");
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
            "::getDataFromVgosDb(): adding new clock break at " + stn2Correct->getKey() +
            " station on " + tBreak.toString() + " has failed");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): cannot find station \"" + stnName +
          "\" in the map of stations, adding a clock break has failed");
      /*
      // ok, we add a clock break to each band:
      SgVlbiBand               *band=NULL;
      for (int iBand=0; iBand<bands_.size(); iBand++)
      {
        band = bands_.at(iBand);
        if (band->stationsByName().contains(stnName))
        {
          stn2Correct = band->stationsByName().value(stnName);
          SgParameterBreak         *clockBreak=new SgParameterBreak(tBreak, 0.0);
          if (stn2Correct->clockBreaks().addBreak(clockBreak))
            logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): a clock break at station " + stn2Correct->getKey() + 
              " that occurred on " + tBreak.toString() + " has been added to the " + 
              band->getKey() + "-band");
          else
            logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
              "::getDataFromVgosDb(): adding new clock break at " + stn2Correct->getKey() +
              " station on " + tBreak.toString() + " has failed");
        };
      };
      */
    };
    addAttr(Attr_HAS_CLOCK_BREAKS);
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): no clock breaks found");
  // end of clock breaks.
  //
  // check for leap seconds:
  if (hasLeapSecond)
  {
    setLeapSeconds(leapSecond);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): a number of leap seconds has been found; " + QString().setNum(leapSecond) +
      " leap seconds were assigned");
  };
  //
  // check for the applied cable cal signs:
  if (cableSignByKey.size())
  {
    for (QMap<QString, QString>::const_iterator it=cableSignByKey.begin(); 
      it!=cableSignByKey.end(); ++it)
    {
      const QString            &stnNme=it.key();
      const QString            &stnSgn=it.value();
      if (stationsByName_.contains(stnNme))
      {
        double                  d;
        if (stnSgn.contains("-"))
          d = -1.0;
        else if (stnSgn.contains("+"))
          d =  1.0;
        else 
          d =  0.0;
        stationsByName_[stnNme]->setCableCalMultiplierDBCal(d);
        logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): an applied cable calibration sign " + QString().sprintf("%+g", d) + 
          " has been set for the station [" + stnNme + "] on the basis of string: \"" + stnSgn +
          "\"");
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
          "::getDataFromVgosDb(): got cable cal sign for a station that is not in the session: " + 
          stnNme);
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
      "::getDataFromVgosDb(): " + QString().setNum(cableSignByKey.size()) + 
      " cable calibration signs were applied to stations");
  };

  if (cppsSoft_ == CPPS_UNKNOWN)
    cppsSoft_ = CPPS_HOPS;
  
  //
  //
  // clear temporary storages:
  if (atmInterval)
    delete atmInterval;
  if (atmRateConstraint)
    delete atmRateConstraint;
  if (clkInterval)
    delete clkInterval;
  if (clkRateConstraint)
    delete clkRateConstraint;
  
  if (pmArrayInfo)
    delete pmArrayInfo;
  if (utArrayInfo)
    delete utArrayInfo;
  if (utValues)
    delete utValues;
  if (pmValues)
    delete pmValues;
  if (groupBlWeights)
    delete groupBlWeights;
  
  sourcesNames.clear();
  stationsNames4Coo.clear();
  tectonicPlateNames.clear();
  stationsNames4Ant.clear();
  stationsNames4Ecc.clear();
  
  cbNames.clear();
  cbEpochs.clear();
  cbFlags.clear();
  cableSignByKey.clear();

  // pick up history, put it in every band:
  for (int iBand=0; iBand<numberOfBands(); iBand++)
  {
    SgVlbiBand                 *band=bands_.at(iBand);
    vgosDb->loadHistory(band->history());
    if (lastProcessed_ < vgosDb->getLastModified() )
      lastProcessed_ = vgosDb->getLastModified();
    band->setInputFileVersion(vgosDb->getCurrentVersion());
    band->setCorrelatorType(correlatorType);
  };
  //
  restoreIonCorrections();
  //
  //
  isOk = isOk && selfCheck(guiExpected, masterfileSuffixes);
  //
  SgMJD                         finisEpoch(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    ": getDataFromVgosDb(): the session " + getName() + "  has been read from vgosDb data set" + 
    ", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");


  //
  return isOk;
};

/*=====================================================================================================*/





/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/

/*=====================================================================================================*/
