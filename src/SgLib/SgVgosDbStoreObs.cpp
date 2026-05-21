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
#include <netcdf.h>


#include <SgConstants.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiSession.h>



/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- storeObs part of vgosDb data tree)                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
bool SgVgosDb::storeEpochs4Obs(const QList<SgMJD>& epochs)
{
  SgVdbVariable                &var=vObservationTimeUTC_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_, 
    sessionCode_, "", "");
  //
  if (numOfObs_ != epochs.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Obs(): size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, epochs.size()));
    return false;
  };
  fcSecond  .alternateDimension(0, SD_NumObs);
  fcYmdhm   .alternateDimension(0, SD_NumObs);
//  fcUtc4    .alternateDimension(0, SD_NumObs);
  if (!setupFormat(fcfTimeUTC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Obs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator files", "", "");
  //
  //
  double                        dSec;
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                       *pS=ncdf.lookupVar(fcSecond.name())->data2double();
  short                        *pD=ncdf.lookupVar(fcYmdhm .name())->data2short();
//  short                        *pU=ncdf.lookupVar(fcUtc4  .name())->data2short();
  for (int i=0; i<numOfObs_; i++)
  {
    epochs.at(i).toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
#ifdef OLD_DB_COMPAT
    pD[5*i + 0] = nYear%100;
#else
    pD[5*i + 0] = nYear;
#endif
    pD[5*i + 1] = nMonth;
    pD[5*i + 2] = nDay;
    pD[5*i + 3] = nHour;
    pD[5*i + 4] = nMin;
    pS[i] = dSec;
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Obs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Obs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCalIonGroup(const QString& band, 
  const SgMatrix* ionCals, const SgMatrix* ionSigmas, const QVector<int> &ionDataFlag)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCalIonGroup(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_SlantPathIonoGroup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);

  if (!setupFormat(fcfCalSlantPathIonoGroup, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalIonGroup(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Evaluated from dual band observations", "Obs", "TimeUTC.nc");
  //
  //
  double                       *pV=ncdf.lookupVar(fcCalIonoGroup        .name())->data2double();
  double                       *pS=ncdf.lookupVar(fcCalIonoGroupSigma   .name())->data2double();
  short                        *pI=ncdf.lookupVar(fcCalIonoGroupDataFlag.name())->data2short();
  for (int i=0; i<ionDataFlag.size(); i++)
  {
    pV[2*i + 0] = ionCals  ->getElement(i, 0);
    pV[2*i + 1] = ionCals  ->getElement(i, 1);
    pS[2*i + 0] = ionSigmas->getElement(i, 0);
    pS[2*i + 1] = ionSigmas->getElement(i, 1);
    pI[i] = ionDataFlag[i];
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalIonGroup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCalIonGroup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
//
bool SgVgosDb::storeIonoBits(const QVector<int>& ionoBits)
{
  SgVdbVariable                &var=progSolveDescriptor_.vIonoBits_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");

  if (!setupFormat(fcfIonoBits, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeIonBits(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The results of calculations", "Obs", "TimeUTC.nc");
  //
  //
  short                        *pI=ncdf.lookupVar(fcIonoBits.name())->data2short();
  for (int i=0; i<ionoBits.size(); i++)
    pI[i] = ionoBits[i];
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeIonoBits(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeIonoBits(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsEditData(const QVector<int>& delUFlag, const QVector<int>& phsUFlag,
                                 const QVector<int>& ratUFlag, const QVector<int>& uAcSup)
{
  SgVdbVariable                &var=vEdit_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  
  if (!setupFormat(fcfEdit_v1002, ncdf)) // this is a current version
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEditData(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The results of calculations", "Obs", "TimeUTC.nc");
  //
  //
  short                        *pD=ncdf.lookupVar(fcDelayFlag.name())->data2short();
  short                        *pR=ncdf.lookupVar(fcRateFlag .name())->data2short();
  short                        *pP=ncdf.lookupVar(fcPhaseFlag.name())->data2short();
//  short                        *pU=ncdf.lookupVar(fcUserSup  .name())->data2short();
  for (int i=0; i<delUFlag.size(); i++)
  {
    pD[i] = delUFlag[i];
    pR[i] = ratUFlag[i];
    pP[i] = phsUFlag[i];
//    pU[i] = uAcSup  [i];
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEditData(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  //
  storeObsUserSup(uAcSup);
  //
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsEditData(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsUserSup(const QVector<int>& uAcSup)
{
  SgVdbVariable                &var=progSolveDescriptor_.vUserSup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  
  if (!setupFormat(fcfUserSup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsUserSup(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The results of user's actions", "Obs", "TimeUTC.nc");
  //
  //
  short                        *pU=ncdf.lookupVar(fcUserSup  .name())->data2short();
  for (int i=0; i<uAcSup.size(); i++)
    pU[i] = uAcSup[i];
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsUserSup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsUserSup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
//
bool SgVgosDb::storeObsNumGroupAmbigs(const QString &band, const QVector<int> &numAmbigs, 
  const QVector<int> &numSubAmbigs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsNumGroupAmbigs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vNumGroupAmbig_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numSubAmbigs.size() == numAmbigs.size())
  {
    if (!setupFormat(fcfNumGroupAmbigE, ncdf, "", band))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): format description (E) failed");
      return false;
    };
    ncdf.setServiceVars(var.getStub(), "The results of calculations", "Obs", "TimeUTC.nc");
    //
    short                        *pN=ncdf.lookupVar(fcNumGroupAmbig.name())->data2short();
    short                        *pS=ncdf.lookupVar(fcNumGroupSubAmbig.name())->data2short();
    for (int i=0; i<numAmbigs.size(); i++)
    {
      pN[i] = numAmbigs[i];
      pS[i] = numSubAmbigs[i];
    };
    // write data:
    ncdf.setOperationMode(operationMode_);
    if (!ncdf.putData())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): cannot put data (E) into " + ncdf.getFileName());
      return false;
    };  
    if (operationMode_ == SgNetCdf::OM_REGULAR)
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): the data (E) were successfully stored in " + ncdf.getFileName());
    return true;
  }
  else
  {
    if (!setupFormat(fcfNumGroupAmbig, ncdf, "", band))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): format description failed");
      return false;
    };
    ncdf.setServiceVars(var.getStub(), "The results of calculations", "Obs", "TimeUTC.nc");
    //
    short                        *pI=ncdf.lookupVar(fcNumGroupAmbig.name())->data2short();
    for (int i=0; i<numAmbigs.size(); i++)
      pI[i] = numAmbigs[i];
    // write data:
    ncdf.setOperationMode(operationMode_);
    if (!ncdf.putData())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): cannot put data into " + ncdf.getFileName());
      return false;
    };  
    if (operationMode_ == SgNetCdf::OM_REGULAR)
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
        "::storeObsNumGroupAmbigs(): the data were successfully stored in " + ncdf.getFileName());
    return true;
  };
};



//
//
bool SgVgosDb::storeObsNumPhaseAmbigs(const QString &band, const QVector<int> &numAmbigs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsNumPhaseAmbigs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vNumPhaseAmbig_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (!setupFormat(fcfNumPhaseAmbig, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsNumPhaseAmbigs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The results of calculations", "Obs", "TimeUTC.nc");
  //
  int                          *pI=ncdf.lookupVar(fcNumPhaseAmbig.name())->data2int();
  for (int i=0; i<numAmbigs.size(); i++)
    pI[i] = numAmbigs[i];
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsNumPhaseAmbigs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsNumPhaseAmbigs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsObjectNames(const QList<QString>& stations_1, const QList<QString>& stations_2, 
                                    const QList<QString>& sources)
{
  SgVdbVariable                &varB=vBaseline_;
  SgVdbVariable                &varS=vSource_;
  SgNetCdf                      ncdfB(path2RootDir_ + "/" + 
    varB.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  if (!setupFormat(fcfBaseline, ncdfB))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): format description of baselines failed");
    return false;
  };
  SgNetCdf                      ncdfS(path2RootDir_ + "/" + 
    varS.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  if (!setupFormat(fcfSource, ncdfS))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): format description of sources failed");
    return false;
  };
  ncdfB.setServiceVars(varB.getStub(), "Data are extracted from correlator files", "Obs", "TimeUTC.nc");
  ncdfS.setServiceVars(varS.getStub(), "Data are extracted from correlator files", "Obs", "TimeUTC.nc");
  //
  char                         *pSb=ncdfB.lookupVar(fcBaseline.name())->data2char();
  char                         *pSs=ncdfS.lookupVar(fcSource.name())->data2char();
  int                           lenB=fcBaseline.dims().at(2);
  int                           lenS=fcSource.dims().at(1);
  for (int i=0; i<numOfObs_; i++)
  {
    strncpy(pSb + lenB* 2*i   , qPrintable(stations_1.at(i)), lenB);
    strncpy(pSb + lenB*(2*i+1), qPrintable(stations_2.at(i)), lenB);
    strncpy(pSs + lenS*i      , qPrintable(sources.at(i)),    lenS);
  };
  // write data:
  ncdfB.setOperationMode(operationMode_);
  if (!ncdfB.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): cannot put baselines data into " + ncdfB.getFileName());
    return false;
  };
  ncdfS.setOperationMode(operationMode_);
  if (!ncdfS.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): cannot put sources data into " + ncdfS.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): the data were successfully stored in " + ncdfB.getFileName());
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsObjectNames(): the data were successfully stored in " + ncdfS.getFileName());
  };
  return true;
};



//
bool SgVgosDb::storeObsCrossRefs(const QList<int>& obs2scan,
  const QList<int>& obs2stn_1, const QList<int>& obs2stn_2)
{
  SgVdbVariable                &var=vObsCrossRef_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (!setupFormat(fcfObsCrossRef, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCrossRefs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Generated on the fly", "", "");
  //
  //
  int                          *pI=ncdf.lookupVar(fcObs2Scan    .name())->data2int();
  short                        *pB=ncdf.lookupVar(fcObs2Baseline.name())->data2short();
  for (int i=0; i<numOfObs_; i++)
  {
    pI[  i    ] = obs2scan.at(i);
    pB[2*i    ] = obs2stn_1.at(i);
    pB[2*i + 1] = obs2stn_2.at(i);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCrossRefs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCrossRefs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeSourceCrossRefs(const QList<QString>& sources, const QList<int>& scan2src)
{
  SgVdbVariable                &var=vSourceCrossRef_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  
  //
  if (numOfSrc_ != sources.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourceCrossRefs(): src list size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfSrc_, sources.size()));
    return false;
  };
  if (numOfScans_ != scan2src.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourceCrossRefs(): scan map size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, scan2src.size()));
    return false;
  };
  //
  if (!setupFormat(fcfSourceCrossRef, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourceCrossRefs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Generated on the fly", "", "");
  //
  //
  int                          *pI=ncdf.lookupVar(fcScan2Source       .name())->data2int();
  char                         *pS=ncdf.lookupVar(fcCrossRefSourceList.name())->data2char();
  int                           len=fcCrossRefSourceList.dims().at(1);
  for (int i=0; i<numOfSrc_; i++)
    strncpy(pS + len*i, qPrintable(sources.at(i)), len);
  for (int i=0; i<numOfScans_; i++)
    pI[i] = scan2src.at(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourceCrossRefs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeSourceCrossRefs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCrossRefs(const QList<int>& numScansPerStation, 
  const QList<QString>& stations, 
  const QMap<QString, QList<int> >& stations2scan, 
  const QMap<QString, QList<int> >& scans2station)
{
  SgVdbVariable                &var=vStationCrossRef_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  //
  if (numOfStn_ != numScansPerStation.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): numScansPerStation list size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfStn_, numScansPerStation.size()));
    return false;
  };
  if (numOfStn_ != stations.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): stn list size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfStn_, stations.size()));
    return false;
  };
  if (numOfStn_ != stations2scan.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): stations2scan map size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfStn_, stations2scan.size()));
    return false;
  };
  if (numOfStn_ != scans2station.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): scans2station map size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfStn_, scans2station.size()));
    return false;
  };
  if (numOfScans_ != scans2station.begin().value().size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): scans2station map's content size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, scans2station.begin().value().size()));
    return false;
  };
  // check the dimensions:
  int                           numOfEpochs=stations2scan.begin().value().size();
  //
  for (QMap<QString, QList<int> >::const_iterator it=stations2scan.begin(); it!=stations2scan.end();
    ++it)
    if (it.value().size() != numOfEpochs)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeStationCrossRefs(): stations2scan map content size mismatch for the station " + 
        it.key() + ": " + QString("").sprintf("%d vs %d", numOfEpochs, it.value().size()));
      return false;
    };
  for (QMap<QString, QList<int> >::const_iterator it=scans2station.begin(); it!=scans2station.end();
    ++it)
    if (it.value().size() != numOfScans_)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeStationCrossRefs(): scans2station map content size mismatch for the station " + 
        it.key() + ": " + QString("").sprintf("%d vs %d", numOfScans_, it.value().size()));
      return false;
    };
  //
  fcStation2Scan.alternateDimension(0, numOfEpochs);
  if (!setupFormat(fcfStationCrossRef, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): format description failed");
    return false;
  };
  fcStation2Scan.alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "Generated on the fly", "", "");
  //
  //
  int                          *pN=ncdf.lookupVar(fcNumScansPerStation  .name())->data2int();
  int                          *p2Scn=ncdf.lookupVar(fcStation2Scan     .name())->data2int();
  int                          *p2Stn=ncdf.lookupVar(fcScan2Station     .name())->data2int();
  char                         *pS=ncdf.lookupVar(fcCrossRefStationList .name())->data2char();
  int                           len=fcCrossRefStationList.dims().at(1);
  for (int i=0; i<numOfStn_; i++)
  {
    const QString              &stn=stations.at(i);
    strncpy(pS + len*i, qPrintable(stn), len);
    pN[i] = numScansPerStation.at(i);
    //
    if (stations2scan.contains(stn))
    {
      for (int j=0; j<numOfEpochs; j++)
        p2Scn[i + j*numOfStn_] = stations2scan[stn].at(j);
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeStationCrossRefs(): cannot find station " + stn + " in the stations2scan map");
      return false;
    };
    //
    if (scans2station.contains(stn))
    {
      for (int j=0; j<numOfScans_; j++)
        p2Stn[i + j*numOfStn_] = scans2station[stn].at(j);
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeStationCrossRefs(): cannot find station " + stn + " in the scans2station map");
      return false;
    };
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCrossRefs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsSingleBandDelays(const QString& band, const SgMatrix* singleBandDelays)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsSingleBandDelays(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vSBDelay_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)singleBandDelays->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSingleBandDelays(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, singleBandDelays->nRow()));
    return false;
  };
  if (2 != singleBandDelays->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSingleBandDelays(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", 2, singleBandDelays->nCol()));
    return false;
  };
  //
  if (!setupFormat(fcfSBDelay, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSingleBandDelays(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcSBDelay   .name())->data2double();
  double                       *pSig=ncdf.lookupVar(fcSBDelaySig.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pVal[i] = singleBandDelays->getElement(i,0);
    pSig[i] = singleBandDelays->getElement(i,1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSingleBandDelays(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsSingleBandDelays(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsGroupDelays(const QString& band, const SgMatrix* groupDelays, const QString& kind)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsGroupDelays(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vGroupDelay_;
  //
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)groupDelays->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelays(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, groupDelays->nRow()));
    return false;
  };
  if (2 != groupDelays->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelays(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", 2, groupDelays->nCol()));
    return false;
  };
  //
  if (!setupFormat(fcfGroupDelay, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelays(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcGroupDelay    .name())->data2double();
  double                       *pSig=ncdf.lookupVar(fcGroupDelaySig .name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pVal[i] = groupDelays->getElement(i,0);
    pSig[i] = groupDelays->getElement(i,1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelays(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelays(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsGroupDelaysFull(const QString& band, const SgVector* groupDelays)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsGroupDelaysFull(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vGroupDelayFull_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)groupDelays->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelaysFull(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, groupDelays->n()));
    return false;
  };
  //
  if (!setupFormat(fcfGroupDelayFull, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelaysFull(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Evaluated by the software", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcGroupDelayFull.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    pVal[i] = groupDelays->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelaysFull(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsGroupDelaysFull(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPhaseDelaysFull(const QString& band, const SgVector* phaseDelays, 
  const SgVector* phaseDelaySigs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsPhaseDelaysFull(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseDelayFull_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)phaseDelays->n() || numOfObs_ != (int)phaseDelaySigs->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseDelaysFull(): vector size mismatch: " + 
      QString("").sprintf("%d vs (%d or %d)", numOfObs_, phaseDelays->n(), phaseDelaySigs->n()));
    return false;
  };
  //
  if (!setupFormat(fcfPhaseDelayFull, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseDelaysFull(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Evaluated by the software", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcPhaseDelayFull.name())->data2double();
  double                       *pSig=ncdf.lookupVar(fcPhaseDelaySigFull.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pVal[i] = phaseDelays->getElement(i);
    pSig[i] = phaseDelaySigs->getElement(i);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseDelaysFull(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseDelaysFull(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};


//
bool SgVgosDb::storeObsRates(const QString& band, const SgMatrix* rates, const QString& kind)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsRates(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vGroupRate_;
  //
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  if (numOfObs_ != (int)rates->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRates(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, rates->nRow()));
    return false;
  };
  if (2 != rates->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRates(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", 2, rates->nCol()));
    return false;
  };
  //
  if (!setupFormat(fcfGroupRate, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRates(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcGroupRate   .name())->data2double();
  double                       *pSig=ncdf.lookupVar(fcGroupRateSig.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pVal[i] = rates->getElement(i,0);
    pSig[i] = rates->getElement(i,1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRates(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsRates(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPhase(const QString& band, const SgMatrix* phases)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsPhase(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhase_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)phases->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhase(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, phases->nRow()));
    return false;
  };
  if (2 != phases->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhase(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", 2, phases->nCol()));
    return false;
  };
  //
  if (!setupFormat(fcfPhase, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhase(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *pVal=ncdf.lookupVar(fcPhase   .name())->data2double();
  double                       *pSig=ncdf.lookupVar(fcPhaseSig.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pVal[i] = phases->getElement(i,0);
    pSig[i] = phases->getElement(i,1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhase(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPhase(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsRefFreqs(const QString& band, const SgVector* rf)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsRefFreqs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vRefFreq_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)rf->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRefFreqs(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, rf->n()));
    return false;
  };
  //
  if (!setupFormat(fcfRefFreq, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRefFreqs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcRefFreq.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = rf->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRefFreqs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsRefFreqs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsEffFreqs(const QString& band, const SgMatrix* freqs, bool areEqWgts)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsEffFreqs(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=areEqWgts?
                                    bandDataByName_[band].vEffFreq_EqWt_:bandDataByName_[band].vEffFreq_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)freqs->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEffFreqs(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, freqs->nRow()));
    return false;
  };
  if (3 != freqs->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEffFreqs(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", 2, freqs->nCol()));
    return false;
  };
  //
  QList<SgVgosDb::FmtChkVar*>  &fcf=areEqWgts?fcfEffFreqEqWgts:fcfEffFreq;
  if (!setupFormat(fcf, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEffFreqs(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data calculated using channel setup", "Obs", "TimeUTC.nc");
  //
  double                       *pG, *pP, *pR;
  if (areEqWgts)
  {
    pG=ncdf.lookupVar(fcFreqGroupIonEqWgts.name())->data2double();
    pP=ncdf.lookupVar(fcFreqPhaseIonEqWgts.name())->data2double();
    pR=ncdf.lookupVar(fcFreqRateIonEqWgts .name())->data2double();
  }
  else
  {
    pG=ncdf.lookupVar(fcFreqGroupIon.name())->data2double();
    pP=ncdf.lookupVar(fcFreqPhaseIon.name())->data2double();
    pR=ncdf.lookupVar(fcFreqRateIon .name())->data2double();
  };
  for (int i=0; i<numOfObs_; i++)
  {
    pG[i] = freqs->getElement(i, 0);
    pP[i] = freqs->getElement(i, 1);
    pR[i] = freqs->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsEffFreqs(): cannot put data into " + ncdf.getFileName() + 
      " using " + (areEqWgts?"equal weights":"weighted") + " mode");
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsEffFreqs(): the data were successfully stored in " + ncdf.getFileName() + 
      " using " + (areEqWgts?"equal weights":"weighted") + " mode");
  return true;
};



//
bool SgVgosDb::storeObsAmbigSpacing(const QString& band, const SgVector* ambigs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsAmbigSpacing(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vAmbigSize_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)ambigs->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsAmbigSpacing(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, ambigs->n()));
    return false;
  };
  //
  if (!setupFormat(fcfAmbigSize, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsAmbigSpacing(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcAmbigSize.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = ambigs->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsAmbigSpacing(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsAmbigSpacing(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsQualityCodes(const QString& band, const QVector<char>& qualityCodes)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsQualityCodes(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vQualityCode_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != qualityCodes.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsQualityCodes(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, qualityCodes.size()));
    return false;
  };
  //
  if (!setupFormat(fcfQualityCode, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsQualityCodes(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  char                         *p=ncdf.lookupVar(fcQualityCode.name())->data2char();
  for (int i=0; i<numOfObs_; i++)
    p[i] = qualityCodes.at(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsQualityCodes(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsAmbigSpacing(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCorrelation(const QString &band, const SgVector* correlations)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCorrelation(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCorrelation_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)correlations->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelation(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, correlations->n()));
    return false;
  };
  //
  if (!setupFormat(fcfCorrelation, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelation(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCorrelation.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = correlations->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelation(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelation(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsUVFperAsec(const QString &band, const SgMatrix* uvfPerAsec)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsUVFperAsec(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vUVFperAsec_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)uvfPerAsec->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsUVFperAsec(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, uvfPerAsec->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfUVFperAsec, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsUVFperAsec(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcUVFperAsec.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = uvfPerAsec->getElement(i, 0);
    p[2*i + 1] = uvfPerAsec->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsUVFperAsec(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsUVFperAsec(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCalFeedCorr(const QString &band, const SgMatrix* contrib)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCalFeedCorr(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_FeedCorrection_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)contrib->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalFeedCorr(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, contrib->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalFeedCorrection, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalFeedCorr(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCalFeedCorrection.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = contrib->getElement(i, 0);
    p[2*i + 1] = contrib->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalFeedCorr(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCalFeedCorr(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsSNR(const QString &band, const SgVector* snrs)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsSNR(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vSNR_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_, 
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)snrs->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSNR(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, snrs->n()));
    return false;
  };
  //
  if (!setupFormat(fcfSNR, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSNR(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcSNR.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = snrs->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsSNR(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsSNR(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsDtec(const SgVector* dTec, const SgVector* dTecStdDev)
{
  SgVdbVariable                &var=vDiffTec_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_, 
    sessionCode_);
  //
  if (numOfObs_ != (int)dTec->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDtec(): vector dTec size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dTec->n()));
    return false;
  };
  if (numOfObs_ != (int)dTecStdDev->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDtec(): vector dTecStdDev size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dTecStdDev->n()));
    return false;
  };
  //
  if (!setupFormat(fcfDTec, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDtec(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *d=ncdf.lookupVar(fcDtec       .name())->data2double();
  double                       *e=ncdf.lookupVar(fcDtecStdErr .name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    d[i] = dTec       ->getElement(i);
    e[i] = dTecStdDev ->getElement(i);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDtec(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsDtec(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsDataFlag(const QString &band, const QVector<int>& dataFlags)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsDataFlag(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vDataFlag_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" + 
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)dataFlags.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDataFlag(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dataFlags.size()));
    return false;
  };
  //
  if (!setupFormat(fcfDataFlag, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDataFlag(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Auxiliary data", "Obs", "TimeUTC.nc");
  //
  short                        *p=ncdf.lookupVar(fcDataFlag.name())->data2short();
  for (int i=0; i<numOfObs_; i++)
    p[i] = dataFlags.at(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDataFlag(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsDataFlag(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};




//
bool SgVgosDb::storeObsChannelInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
  const QVector<QString>& channelIds,                         // CHAN ID
  const QVector<QString>& polarizations,                      // POLARIZ
  const QVector<int>& numOfChannels,                          // #CHANELS
  const QVector<int>& bitsPerSamples,                         // BITSAMPL
  const QVector< QVector<int> >& errorRates_1,                // ERRORATE @1
  const QVector< QVector<int> >& errorRates_2,                // ERRORATE @2
  const QVector< QVector<int> >& bbcIdxs_1,                   // BBC IND @1
  const QVector< QVector<int> >& bbcIdxs_2,                   // BBC IND @2
  const QVector< QVector<int> >& corelIdxNumbersUsb,          // INDEXNUM usb
  const QVector< QVector<int> >& corelIdxNumbersLsb,          // INDEXNUM lsb
  const SgVector* sampleRate,                                 // SAMPLRAT
  const SgMatrix* residFringeAmplByChan,                      // AMPBYFRQ Amp
  const SgMatrix* residFringePhseByChan,                      // AMPBYFRQ Phs
  const SgMatrix* refFreqByChan,                              // RFREQ
  const SgMatrix* numOfSamplesByChan_USB,                     // #SAMPLES usb
  const SgMatrix* numOfSamplesByChan_LSB,                     // #SAMPLES lsb
  const SgMatrix* numOfAccPeriodsByChan_USB,                  // NO.OF AP usb
  const SgMatrix* numOfAccPeriodsByChan_LSB,                  // NO.OF AP lsb
  const SgMatrix* loFreqs_1,                                  // LO FREQ @1
  const SgMatrix* loFreqs_2,                                  // LO FREQ @2
  const SgMatrix* channelBandwidth,                           // bandwidth of channels
  SgSidebandOrder sbOrder
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsChannelInfo(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vChannelInfo_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != numOfChannels.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsChannelInfo(): numOfChannels vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, numOfChannels.size()));
    return false;
  };
  //
  bool                          hasExtendedData=true;
  //
  QList<SgVgosDb::FmtChkVar*>  &fcfChannelInfo=fcfChannelInfoMk4;
  if (originType == SgVlbiSessionInfo::OT_KOMB)
  {
    fcfChannelInfo = fcfChannelInfoKomb;
    hasExtendedData= false;
  };
  //
  if (!setupFormat(fcfChannelInfo, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsChannelInfo(): format description failed");
    return false;
  };
  //
  //
  /*
  if (originType == SgVlbiSessionInfo::OT_MK4 ||
      originType == SgVlbiSessionInfo::OT_VDB ||
      (originType == SgVlbiSessionInfo::OT_AGV && 
        (correlatorType_==CT_Mk4 || correlatorType_==CT_Difx || correlatorType_==CT_VLBA)))
    isNewMode = true;
  */
  
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  char                         *pChannelID=NULL;
  char                         *pPolarization=NULL;
  short                        *pNumChannels=ncdf.lookupVar(fcNumChannels.name())->data2short();
  short                        *pBITSAMPL=NULL;
  short                        *pERRORATE=NULL;
  short                        *pBBCIndex=NULL;
  short                        *pINDEXNUM=NULL;
  double                       *pSampleRate=ncdf.lookupVar(fcSampleRate.name())->data2double();
  double                       *pChanAmpPhase=ncdf.lookupVar(fcChanAmpPhase.name())->data2double();
  double                       *pChannelFreq=ncdf.lookupVar(fcChannelFreq.name())->data2double();
  double                       *pNumSamples=NULL;
  double                       *pLOFreq=NULL;
  double                       *pChannelBw=ncdf.lookupVar(fcChannelBandwidth.name())->data2double();
  short                        *pNumAp=ncdf.lookupVar(fcNumAp.name())->data2short();
  short                        *pSidebandOrder=ncdf.lookupVar(fcSidebandOrder.name())->data2short();


  *pSidebandOrder = sbOrder;
  int                           usbOffset, lsbOffset;
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

  if (sbOrder == SO_UNDEF)
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsChannelInfo(): the sideband order is not defined");

  //
  // Mk4 extension:
//  if (originType == SgVlbiSessionInfo::OT_MK4)
//      SgVgosDb::CT_Mk3 || SgVgosDb::CT_CRL || SgVgosDb::CT_GSI || SgVgosDb::CT_S2
  if (hasExtendedData)
  {
    pChannelID = ncdf.lookupVar(fcChannelID.name())->data2char();
    pPolarization = ncdf.lookupVar(fcPolarization.name())->data2char();
    pBITSAMPL = ncdf.lookupVar(fcBITSAMPL.name())->data2short();
    pERRORATE = ncdf.lookupVar(fcERRORATE.name())->data2short();
    pBBCIndex = ncdf.lookupVar(fcBBCIndex.name())->data2short();
    pINDEXNUM = ncdf.lookupVar(fcINDEXNUM.name())->data2short();
    pNumSamples = ncdf.lookupVar(fcNumSamples.name())->data2double();
    pLOFreq = ncdf.lookupVar(fcLOFreq.name())->data2double();
  }
  //
  // KOMB extension:
  else if (originType == SgVlbiSessionInfo::OT_KOMB)
  {
    // reserved
  };
  //
  //    fill data structures:
  //
  for (int i=0; i<numOfObs_; i++)
  {
    // common for all types of correlator:
    pNumChannels[i] = numOfChannels[i];
    pSampleRate[i] = sampleRate->getElement(i);
    //
    // Mk4 specific part:
//    if (originType == SgVlbiSessionInfo::OT_MK4)
    if (hasExtendedData)
    {
      strncpy(pChannelID    + 2*numOfChan_*i, qPrintable(channelIds.at(i)),    2*numOfChan_);
      strncpy(pPolarization + 4*numOfChan_*i, qPrintable(polarizations.at(i)), 4*numOfChan_);
    }
    // KOMB specific part:
    else if (originType == SgVlbiSessionInfo::OT_KOMB)
    {
      // reserved
    };
    //
    // per channel data:
    for (int j=0; j<numOfChan_; j++)
    {
      // common part:
      pNumAp[2*numOfChan_*i + 2*j + usbOffset]= round(numOfAccPeriodsByChan_USB->getElement(i,j));
      pNumAp[2*numOfChan_*i + 2*j + lsbOffset]= round(numOfAccPeriodsByChan_LSB->getElement(i,j));
      pChannelBw    [numOfChan_*i + j]        = channelBandwidth->getElement(i,j);
      pChannelFreq  [numOfChan_  *i + j]      = refFreqByChan->getElement(i,j);
      pChanAmpPhase [numOfChan_*2*i + 2*j + 0]= residFringeAmplByChan->getElement(i,j);
      pChanAmpPhase [numOfChan_*2*i + 2*j + 1]= residFringePhseByChan->getElement(i,j);
      //
      // Mk4 specific part:
//    if (originType == SgVlbiSessionInfo::OT_MK4)
      if (hasExtendedData)
      {
        //fcERRORATE:
        if (errorRates_1.at(i).size()==numOfChan_ && errorRates_2.at(i).size()==numOfChan_)
        {
          // LSB here is the same as USB:
          pERRORATE[4*numOfChan_*i + 4*j    ] = errorRates_1.at(i).at(j);
          pERRORATE[4*numOfChan_*i + 4*j + 1] = errorRates_2.at(i).at(j);
          pERRORATE[4*numOfChan_*i + 4*j + 2] = errorRates_1.at(i).at(j); 
          pERRORATE[4*numOfChan_*i + 4*j + 3] = errorRates_2.at(i).at(j);
        };
        //fcLOFreq:
        pLOFreq[2*numOfChan_*i + 2*j    ] = loFreqs_1->getElement(i,j);
        pLOFreq[2*numOfChan_*i + 2*j + 1] = loFreqs_2->getElement(i,j);
        //
        //fcNumSamples: 
        pNumSamples[2*numOfChan_*i + 2*j + usbOffset] = numOfSamplesByChan_USB->getElement(i,j);
        pNumSamples[2*numOfChan_*i + 2*j + lsbOffset] = numOfSamplesByChan_LSB->getElement(i,j);
        //fcBITSAMPL:
        pBITSAMPL[i] = bitsPerSamples.at(i);
        //fcBBCIndex:
        if (bbcIdxs_1.at(i).size()==numOfChan_ && bbcIdxs_2.at(i).size()==numOfChan_)
        {
          pBBCIndex[2*numOfChan_*i + 2*j    ] = bbcIdxs_1.at(i).at(j);
          pBBCIndex[2*numOfChan_*i + 2*j + 1] = bbcIdxs_2.at(i).at(j);
        };
        //fcINDEXNUM:
        if (corelIdxNumbersUsb.at(i).size()==numOfChan_ && corelIdxNumbersLsb.at(i).size()==numOfChan_)
        {
          pINDEXNUM[2*numOfChan_*i + 2*j + usbOffset] = corelIdxNumbersUsb.at(i).at(j);
          pINDEXNUM[2*numOfChan_*i + 2*j + lsbOffset] = corelIdxNumbersLsb.at(i).at(j);
        };
      }
      // KOMB specific part:
      else if (originType == SgVlbiSessionInfo::OT_KOMB)
      {
        // reserved
      };
    };
    //
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsChannelInfo(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsChannelInfo(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPhaseCalInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
  const SgMatrix* phCalFrqs_1ByChan, 
  const SgMatrix* phCalAmps_1ByChan, 
  const SgMatrix* phCalPhss_1ByChan, 
  const SgMatrix* phCalOffs_1ByChan, 
  const SgVector* phCalRates_1, 

  const SgMatrix* phCalFrqs_2ByChan, 
  const SgMatrix* phCalAmps_2ByChan, 
  const SgMatrix* phCalPhss_2ByChan, 
  const SgMatrix* phCalOffs_2ByChan, 
  const SgVector* phCalRates_2
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsPhaseCalInfo(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vPhaseCalInfo_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  QList<SgVgosDb::FmtChkVar*>  &fcfPhaseCalInfo=fcfPhaseCalInfoMk4;
  if (originType == SgVlbiSessionInfo::OT_KOMB)
    fcfPhaseCalInfo = fcfPhaseCalInfoMk3;
  //
  if (numOfObs_ != (int)phCalFrqs_1ByChan->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseCalInfo(): phCalFrqs_1ByChan matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, phCalFrqs_1ByChan->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPhaseCalInfo, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseCalInfo(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *pPhaseCalRate=ncdf.lookupVar(fcPhaseCalRate.name())->data2double();
  short                        *pPhaseCalFreq=ncdf.lookupVar(fcPhaseCalFreq.name())->data2short();
  short                        *pPhaseCalAmp=ncdf.lookupVar(fcPhaseCalAmp.name())->data2short();
  short                        *pPhaseCalPhase=ncdf.lookupVar(fcPhaseCalPhase.name())->data2short();
  short                        *pPhaseCalOffset=NULL;
  if (originType == SgVlbiSessionInfo::OT_MK4 ||
      originType == SgVlbiSessionInfo::OT_AGV)
    pPhaseCalOffset = ncdf.lookupVar(fcPhaseCalOffset.name())->data2short();
  //
  for (int i=0; i<numOfObs_; i++)
  {
    pPhaseCalRate[2*i     ] = phCalRates_1->getElement(i);
    pPhaseCalRate[2*i + 1 ] = phCalRates_2->getElement(i);
    // per channel data:
    for (int j=0; j<numOfChan_; j++)
    {
      // fcPhaseCalFreq:
      pPhaseCalFreq[2*numOfChan_*i + 2*j    ] = round(phCalFrqs_1ByChan->  getElement(i,j));
      pPhaseCalFreq[2*numOfChan_*i + 2*j + 1] = round(phCalFrqs_2ByChan->  getElement(i,j));
      // fcPhaseCalAmp:
      pPhaseCalAmp[2*numOfChan_*i + 2*j    ] = round(phCalAmps_1ByChan->  getElement(i,j));
      pPhaseCalAmp[2*numOfChan_*i + 2*j + 1] = round(phCalAmps_2ByChan->  getElement(i,j));
      // fcPhaseCalPhase:
      pPhaseCalPhase[2*numOfChan_*i + 2*j    ] = round(phCalPhss_1ByChan->  getElement(i,j));
      pPhaseCalPhase[2*numOfChan_*i + 2*j + 1] = round(phCalPhss_2ByChan->  getElement(i,j));
      // fcPhaseCalOffset:
      if (originType == SgVlbiSessionInfo::OT_MK4 ||
          originType == SgVlbiSessionInfo::OT_AGV)
      {
        pPhaseCalOffset[2*numOfChan_*i + 2*j    ] = round(phCalOffs_1ByChan->  getElement(i,j));
        pPhaseCalOffset[2*numOfChan_*i + 2*j + 1] = round(phCalOffs_2ByChan->  getElement(i,j));
      };
    };
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseCalInfo(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPhaseCalInfo(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCorrelatorInfo(const QString& band, SgVlbiSessionInfo::OriginType originType,
  const QVector<QString>& fourfitOutputFName,             // fcFOURFFIL
  const QVector<QString>& fourfitErrorCodes,              // fcFRNGERR
  const QVector<QString>& frqGrpCodes,                    // fcFRQGROUP 
  const QVector<int>& corelVersions,                      // fcCORELVER
  const QVector<int>& startOffsets,                       // fcStartOffset
  const QVector< QVector<int> >& fourfitVersions,         // fcFOURFVER 
  const QVector< QVector<int> >& dbeditVersion,           // fcDBEDITVE
  const SgVector* deltaEpochs,                            // fcDELTAEPO
  const SgMatrix* urvrs,                                  // fcURVR
  const SgMatrix* instrDelays,                            // fcIDELAY
  const SgMatrix* starElev,                               // fcSTARELEV
  const SgMatrix* zenithDelays,                           // fcZDELAY
  const SgMatrix* searchParams,                           // fcSRCHPAR
  const QVector<QString>&  baselineCodes,                 // fcCORBASCD
  const QVector<QString>&  tapeQualityCodes,              // fcTapeCode
  const QVector<int>& stopOffsets,                        // fcStopOffset
  const QVector<int>& hopsRevisionNumbers,                // fcHopsRevNum
  const SgVector* sampleRate,                             // fcRECSETUP
  const SgVector* sbdResids,                              // fcSBRESID
  const SgVector* rateResids,                             // fcRATRESID
  const SgVector* effDurations,                           // fcEffectiveDuration
  const SgVector* startSeconds,                           // fcStartSec
  const SgVector* stopSeconds,                            // fcStopSec
  const SgVector* percentOfDiscards,                      // fcDISCARD
  const SgVector* uniformityFactors,                      // fcQBFACTOR
  const SgVector* geocenterPhaseResids,                   // fcGeocResidPhase
  const SgVector* geocenterPhaseTotals,                   // fcGeocPhase
  const SgVector* geocenterSbDelays,                      // fcGeocSBD
  const SgVector* geocenterGrDelays,                      // fcGeocMBD
  const SgVector* geocenterDelayRates,                    // fcGeocRate
  const SgVector* probOfFalses,                           // fcProbFalseDetection
  const SgVector* corrAprioriDelays,                      // fcABASDEL
  const SgVector* corrAprioriRates,                       // fcABASRATE
  const SgVector* corrAprioriAccs,                        // fcABASACCE
  const SgVector* incohAmps,                              // fcINCOHAMP
  const SgVector* incohAmps2,                             // fcINCOH2
  const SgVector* delayResids,                            // fcDELRESID
  const QVector<QString>& vlb1FileNames,                  // fcVLB1FILE
  const QVector<QString>& tapeIds_1,                      // fcTAPEID
  const QVector<QString>& tapeIds_2,                      // fcTAPEID
  const QVector< QVector<int> >& epochsOfCorrelations,    // fcUTCCorr
  const QVector< QVector<int> >& epochsOfCentrals,        // fcUTCMidObs
  const QVector< QVector<int> >& epochsOfFourfitings,     // fcFOURFUTC
  const QVector< QVector<int> >& epochsOfNominalScanTime, // fcUTCScan
  const SgMatrix* corrClocks,                             // fcCORCLOCK
  const SgMatrix* mDlys,                                  // fcDLYEPOm1, fcDLYEPOCH, fcDLYEPOp1
  const SgMatrix* mAux                                    // fcAPCLOFST, fcTOTPCENT, fcRATOBSVM,
                                                          // fcUTCVLB2, fcDELOBSVM
  )
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCorrelatorInfo(): the band \"" + band + "\" is not registered");
    return false;
  };
  if (originType == SgVlbiSessionInfo::OT_UNKNOWN)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCorrelatorInfo(): the data origin type is unknown");
    return false;
  };

  SgVdbVariable                &var=bandDataByName_[band].vCorrInfo_;
  // adjust the stub:
  if (originType == SgVlbiSessionInfo::OT_KOMB)
    var.setStub("CorrInfo-GSI");
  else if ( originType == SgVlbiSessionInfo::OT_MK4 ||
            originType == SgVlbiSessionInfo::OT_AGV)
    var.setStub("CorrInfo-difx");
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != fourfitOutputFName.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelatorInfo(): fourfitOutputFName vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, fourfitOutputFName.size()));
    return false;
  };
  //
  int                           ffStrLength;
  ffStrLength = 0;
  for (int i=0; i<numOfObs_; i++)
    if (ffStrLength < fourfitOutputFName.at(i).size())
      ffStrLength = fourfitOutputFName.at(i).size();
  if (fcFOURFFIL.dims()[1] < ffStrLength)
    fcFOURFFIL.alternateDimension(1, ffStrLength);
  else if (fcFOURFFIL.dims()[1] > ffStrLength)
    ffStrLength = fcFOURFFIL.dims()[1];
  //
  //
  QList<SgVgosDb::FmtChkVar*>  &fcfCorrInfo=fcfCorrInfoMk4;
  if (originType == SgVlbiSessionInfo::OT_KOMB)
    fcfCorrInfo=fcfCorrInfoKomb;
  //
  //
  if (!setupFormat(fcfCorrInfo, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelatorInfo(): format description failed");
    return false;
  };
  fcFOURFFIL.alternateDimension(1, 16);
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  //
  // common:
  char                         *pFOURFFIL=ncdf.lookupVar(fcFOURFFIL.name())->data2char();
  char                         *pFRQGROUP=ncdf.lookupVar(fcFRQGROUP.name())->data2char();
  short                        *pDBEDITVE=ncdf.lookupVar(fcDBEDITVE.name())->data2short();
  double                       *pDELTAEPO=ncdf.lookupVar(fcDELTAEPO.name())->data2double();
  double                       *pSRCHPAR=ncdf.lookupVar(fcSRCHPAR.name())->data2double();
  char                         *pCORBASCD=ncdf.lookupVar(fcCORBASCD.name())->data2char();
  char                         *pTapeCode=ncdf.lookupVar(fcTapeCode.name())->data2char();
  double                       *pSBRESID=ncdf.lookupVar(fcSBRESID.name())->data2double();
  double                       *pRATRESID=ncdf.lookupVar(fcRATRESID.name())->data2double();
  double                       *pEffectiveDuration=
                                              ncdf.lookupVar(fcEffectiveDuration.name())->data2double();
  double                       *pStartSec=ncdf.lookupVar(fcStartSec.name())->data2double();
  double                       *pStopSec=ncdf.lookupVar(fcStopSec.name())->data2double();
  double                       *pDISCARD=ncdf.lookupVar(fcDISCARD.name())->data2double();
  
  double                       *pGeocResidPhase=ncdf.lookupVar(fcGeocResidPhase.name())->data2double();
  double                       *pProbFalseDetection=
                                              ncdf.lookupVar(fcProbFalseDetection.name())->data2double();
  double                       *pABASRATE=ncdf.lookupVar(fcABASRATE.name())->data2double();
  double                       *pABASDEL=ncdf.lookupVar(fcABASDEL.name())->data2double();
  double                       *pABASACCE=ncdf.lookupVar(fcABASACCE.name())->data2double();
  double                       *pGeocPhase=ncdf.lookupVar(fcGeocPhase.name())->data2double();
  double                       *pINCOHAMP=ncdf.lookupVar(fcINCOHAMP.name())->data2double();
  double                       *pINCOH2=ncdf.lookupVar(fcINCOH2.name())->data2double();
  double                       *pDELRESID=ncdf.lookupVar(fcDELRESID.name())->data2double();
  short                        *pUTCCorr=ncdf.lookupVar(fcUTCCorr.name())->data2short();
  short                        *pUTCMidObs=ncdf.lookupVar(fcUTCMidObs.name())->data2short();
  // Mk4 specific:
  char                         *pFRNGERR=NULL;
  short                        *pCORELVER=NULL;
  short                        *pStartOffset=NULL;
  short                        *pFOURFVER=NULL;
  double                       *pURVR=NULL;
  double                       *pIDELAY=NULL;
  double                       *pSTARELEV=NULL;
  double                       *pZDELAY=NULL;
  short                        *pStopOffset=NULL;
  short                        *pHopsRevNum=NULL;
  double                       *pQBFACTOR=NULL;
  double                       *pGeocSBD=NULL;
  double                       *pGeocRate=NULL;
  double                       *pGeocMBD=NULL;
  short                        *pFOURFUTC=NULL;
  short                        *pUTCScan=NULL;
  char                         *pTAPEID=NULL;
  double                       *pCORCLOCK=NULL;
  // KOMB specific:
  short                        *pFRNGERR4S2=NULL;
  char                         *pVLB1FILE=NULL;
  short                        *pRECSETUP=NULL;
  double                       *pDLYEPOp1=NULL;
  double                       *pDLYEPOCH=NULL;
  double                       *pDLYEPOm1=NULL;
  double                       *pAPCLOFST=NULL;
  double                       *pTOTPCENT=NULL;
  double                       *pRATOBSVM=NULL;
  double                       *pDELOBSVM=NULL;
  //
  //
  // Mk4 extension:
  if (originType == SgVlbiSessionInfo::OT_MK4 ||
      originType == SgVlbiSessionInfo::OT_AGV)
  {
    pFRNGERR = ncdf.lookupVar(fcFRNGERR.name())->data2char();
    pCORELVER = ncdf.lookupVar(fcCORELVER.name())->data2short();
    pStartOffset = ncdf.lookupVar(fcStartOffset.name())->data2short();
    pFOURFVER = ncdf.lookupVar(fcFOURFVER.name())->data2short();    
    pURVR = ncdf.lookupVar(fcURVR.name())->data2double();
    pIDELAY = ncdf.lookupVar(fcIDELAY.name())->data2double();
    pSTARELEV = ncdf.lookupVar(fcSTARELEV.name())->data2double();
    pZDELAY = ncdf.lookupVar(fcZDELAY.name())->data2double();
    pStopOffset = ncdf.lookupVar(fcStopOffset.name())->data2short();
    pHopsRevNum = ncdf.lookupVar(fcHopsRevNum.name())->data2short();
    pQBFACTOR = ncdf.lookupVar(fcQBFACTOR.name())->data2double();
    pGeocSBD = ncdf.lookupVar(fcGeocSBD.name())->data2double();
    pGeocRate = ncdf.lookupVar(fcGeocRate.name())->data2double();
    pGeocMBD = ncdf.lookupVar(fcGeocMBD.name())->data2double();
    pFOURFUTC = ncdf.lookupVar(fcFOURFUTC.name())->data2short();
    pUTCScan = ncdf.lookupVar(fcUTCScan.name())->data2short();
    pTAPEID = ncdf.lookupVar(fcTAPEID.name())->data2char();
    pCORCLOCK = ncdf.lookupVar(fcCORCLOCK.name())->data2double();
  }
  else if (originType == SgVlbiSessionInfo::OT_KOMB)
  {
    pFRNGERR4S2 = ncdf.lookupVar(fcFRNGERR4S2.name())->data2short();
    pVLB1FILE = ncdf.lookupVar(fcVLB1FILE.name())->data2char();
    pRECSETUP = ncdf.lookupVar(fcRECSETUP.name())->data2short();
    pDLYEPOp1 = ncdf.lookupVar(fcDLYEPOp1.name())->data2double();
    pDLYEPOCH = ncdf.lookupVar(fcDLYEPOCH.name())->data2double();
    pDLYEPOm1 = ncdf.lookupVar(fcDLYEPOm1.name())->data2double();
    pAPCLOFST = ncdf.lookupVar(fcAPCLOFST.name())->data2double();
    pTOTPCENT = ncdf.lookupVar(fcTOTPCENT.name())->data2double();
    pRATOBSVM = ncdf.lookupVar(fcRATOBSVM.name())->data2double();
    pDELOBSVM = ncdf.lookupVar(fcDELOBSVM.name())->data2double();
    // just use another vgosDb variable:
    pFOURFUTC = ncdf.lookupVar(fcUTCVLB2.name())->data2short();
  };
  //
  //
  for (int i=0; i<numOfObs_; i++)
  {
    strncpy(pFOURFFIL + ffStrLength*i, qPrintable(fourfitOutputFName.at(i)), ffStrLength);
    //
    if (dbeditVersion.at(i).size()==3)
    {
      pDBEDITVE   [3*i  ] = dbeditVersion.at(i).at(0);
      pDBEDITVE   [3*i+1] = dbeditVersion.at(i).at(1);
      pDBEDITVE   [3*i+2] = dbeditVersion.at(i).at(2);
    };
//  pREFCLKER   [i    ] = refClkErr->getElement(i);
    pSBRESID            [i] = sbdResids->getElement(i);
    pRATRESID           [i] = rateResids->getElement(i);
    pEffectiveDuration  [i] = effDurations->getElement(i);
    pStartSec           [i] = startSeconds->getElement(i);
    pStopSec            [i] = stopSeconds->getElement(i);
    pDISCARD            [i] = percentOfDiscards->getElement(i);
    pGeocResidPhase     [i] = geocenterPhaseResids->getElement(i);
    pGeocPhase          [i] = geocenterPhaseTotals->getElement(i);
    pProbFalseDetection [i] = probOfFalses->getElement(i);
    pABASRATE           [i] = corrAprioriRates->getElement(i);
    pABASDEL            [i] = corrAprioriDelays->getElement(i);
    pABASACCE           [i] = corrAprioriAccs->getElement(i);
    pINCOHAMP           [i] = incohAmps->getElement(i);
    pINCOH2             [i] = incohAmps2->getElement(i);
    pDELRESID           [i] = delayResids->getElement(i);
    //fcDELTAEPO:
    pDELTAEPO           [i] = deltaEpochs->getElement(i);
    //
    for (int j=0; j<6; j++)
      pSRCHPAR  [6*i + j] = searchParams->getElement(i,j);
    //
    if (epochsOfCorrelations.at(i).size()==6 &&
        epochsOfCentrals.at(i).size()==6     &&
        epochsOfFourfitings.at(i).size()==6   )
      for (int j=0; j<6; j++)
      {
        pUTCCorr  [6*i + j] = epochsOfCorrelations.at(i).at(j); 
        pUTCMidObs[6*i + j] = epochsOfCentrals.at(i).at(j);
        pFOURFUTC [6*i + j] = epochsOfFourfitings.at(i).at(j);
      }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::storeObsCorrelatorInfo(): Warning: one of epochs (Correlations, Centrals or Fourfitings} "
        "has wrong dimensions");
    //
    strncpy(pFRQGROUP + 2*i,        qPrintable(frqGrpCodes.at(i)), 2);
    strncpy(pCORBASCD + 2*i,        qPrintable(baselineCodes.at(i)), 2);
    strncpy(pTapeCode + 6*i,        qPrintable(tapeQualityCodes.at(i)), 6);
    //
    if (originType == SgVlbiSessionInfo::OT_MK4 ||
        originType == SgVlbiSessionInfo::OT_AGV)
    {
      strncpy(pFRNGERR + i, qPrintable(fourfitErrorCodes.at(i)), 1);
      //fcCORELVER:
      pCORELVER[i] = corelVersions.at(i);
      //fcFOURFVER:
      if (fourfitVersions.at(i).size()==2)
      {
        pFOURFVER[2*i  ] = fourfitVersions.at(i).at(0);
        pFOURFVER[2*i+1] = fourfitVersions.at(i).at(1);
      };
      //fcStartOffset:
      pStartOffset[i] = startOffsets.at(i);
      //fcStopOffset:
      pStopOffset [i] = stopOffsets[i];
      pHopsRevNum [i] = hopsRevisionNumbers[i];
      //fcGeocSBD:
      pGeocSBD    [i] = geocenterSbDelays->getElement(i);
      //fcGeocMBD:
//    pGeocMBD    [i] = geocenterDelayRates->getElement(i);
      pGeocMBD    [i] = geocenterGrDelays->getElement(i);
      //fcGeocRate:
//    pGeocRate   [i] = geocenterGrDelays->getElement(i);
      pGeocRate   [i] = geocenterDelayRates->getElement(i);
      //fcQBFACTOR:
      pQBFACTOR   [i] = uniformityFactors->getElement(i);
      //fcZDELAY:
      pZDELAY     [2*i  ] = zenithDelays->getElement(i,0);
      pZDELAY     [2*i+1] = zenithDelays->getElement(i,1);
      //fcIDELAY:
      pIDELAY     [2*i  ] = instrDelays->getElement(i,0);
      pIDELAY     [2*i+1] = instrDelays->getElement(i,1);
      //fcURVR:
      pURVR       [2*i  ] = urvrs->getElement(i,0);
      pURVR       [2*i+1] = urvrs->getElement(i,1);
      //fcSTARELEV:
      pSTARELEV   [2*i  ] = starElev->getElement(i,0);
      pSTARELEV   [2*i+1] = starElev->getElement(i,1);
      //fcCORCLOCK:
      pCORCLOCK[4*i     ] = corrClocks->getElement(i, 0);
      pCORCLOCK[4*i + 1 ] = corrClocks->getElement(i, 1);
      pCORCLOCK[4*i + 2 ] = corrClocks->getElement(i, 2);
      pCORCLOCK[4*i + 3 ] = corrClocks->getElement(i, 3);
      //fcUTCScan:
      if (epochsOfNominalScanTime.at(i).size() == 6)
        for (int j=0; j<6; j++)
          pUTCScan  [6*i + j] = epochsOfNominalScanTime.at(i).at(j);
      else
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
          "::storeObsCorrelatorInfo(): Warning: epochsOfNominalScanTime has wrong dimensions");
      //fcTAPEID:
      strncpy(pTAPEID   + 2*8*i,      qPrintable(tapeIds_1.at(i)), 8);
      strncpy(pTAPEID   + 2*8*i + 8,  qPrintable(tapeIds_2.at(i)), 8);
    }
    else if (originType == SgVlbiSessionInfo::OT_KOMB)
    {
      if (fourfitErrorCodes.at(i) == "" || fourfitErrorCodes.at(i).simplified() == "")
        pFRNGERR4S2[i] = 0;
      else
        pFRNGERR4S2[i] = 1;
      //fcRECSETUP:
      pRECSETUP[3*i+0] = round(sampleRate->getElement(i)*1.0e-3);
      pRECSETUP[3*i+1] = 1;
      pRECSETUP[3*i+2] = 1;
      pDLYEPOm1[i] = mDlys->getElement(i, 0);
      pDLYEPOCH[i] = mDlys->getElement(i, 1);
      pDLYEPOp1[i] = mDlys->getElement(i, 2);
      
      pAPCLOFST[i] = mAux->getElement(i, 0);
      pTOTPCENT[i] = mAux->getElement(i, 1);
      pRATOBSVM[i] = mAux->getElement(i, 2);
      pDELOBSVM[2*i + 1] = modf(mAux->getElement(i, 3)*1.0e6, &pDELOBSVM[2*i]);

      strncpy(pVLB1FILE + 6*i,       qPrintable(vlb1FileNames.at(i)), 6);
    };
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelatorInfo(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCorrelatorInfo(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsDelayTheoretical(const SgVector* v)
{
  SgVdbVariable                &var=vDelayTheoretical_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)v->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDelayTheoretical(): vector v size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, v->n()));
    return false;
  };
  //
  if (!setupFormat(fcfDelayTheoretical, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDelayTheoretical(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcDelayTheoretical.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = v->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsDelayTheoretical(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsDelayTheoretical(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsRateTheoretical(const SgVector* v)
{
  SgVdbVariable                &var=vRateTheoretical_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)v->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRateTheoretical(): vector v size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, v->n()));
    return false;
  };
  //
  if (!setupFormat(fcfRateTheoretical, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRateTheoretical(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcRateTheoretical.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = v->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsRateTheoretical(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsRateTheoretical(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsStd1Contribution(const QString& varName, const SgMatrix* contrib, 
  SgVdbVariable& var, const QList<SgVgosDb::FmtChkVar*>& fcf)
{
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)contrib->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": matrix contrib size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, contrib->nRow()));
    return false;
  };
  if (2 != (int)contrib->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": matrix contrib size is unexpected: " + 
      QString("").setNum(contrib->nCol()) + ", should be 2");
    return false;
  };
  if (fcf.size() != 1)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": FCF list size is unexpected: " +
      QString("").setNum(fcf.size()) + ", should be 1");
    return false;
  };
  const SgVgosDb::FmtChkVar    *fc=fcf.at(0);
  //
  if (!setupFormat(fcf, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fc->name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = contrib->getElement(i, 0);
    p[2*i + 1] = contrib->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsStd1Contribution(): " + varName + ": the data were successfully stored in " + 
      ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsStd2Contribution(const QString& varName, const QList<const SgMatrix*> &contribs,
  SgVdbVariable& var, const QList<SgVgosDb::FmtChkVar*>& fcf)
{
  //
  // order in contribs corresponds to the order in fcf
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  int                           numEl;
  //
  if ((numEl=contribs.size()) != fcf.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd2Contribution(): " + varName + ": sizes of the lists mismatch: " + 
      QString("").sprintf("%d vs %d", numEl, fcf.size()));
    return false;
  };
  // check consistency of the sizes:
  for (int i=0; i<contribs.size(); i++)
  {
    const SgMatrix             *m=contribs.at(i);
    FmtChkVar                  *fc=fcf.at(i);
    if (numOfObs_ != (int)m->nRow())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsStd2Contribution(): " + varName + ": matrix contribs[" + QString("").setNum(i) + 
        "] size mismatch: " + QString("").sprintf("%d vs %d", numOfObs_, m->nRow()));
      return false;
    };
    if (2 != (int)m->nCol())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsStd2Contribution(): " + varName + ": matrix contribs[" + QString("").setNum(i) + 
        "] size is unexpected: " + QString("").setNum(m->nCol()) + ", should be 2");
      return false;
    };
    if (SD_NumObs!=fc->dims().at(0) || 2!=fc->dims().at(1))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeObsStd2Contribution(): " + varName + ": FC[" + QString("").setNum(i) + 
        "] sizes are unexpected: " + QString("").sprintf("%d:%d", fc->dims().at(0), fc->dims().at(1)) + 
        ", should be " + QString("").setNum(numOfObs_) + ":2");
      return false;
    };
  };
  //
  if (!setupFormat(fcf, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd2Contribution(): " + varName + ": format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  for (int i=0; i<numEl; i++)
  {
    const FmtChkVar            *fc=fcf.at(i);
    const SgMatrix             *m=contribs.at(i);
    double                     *p=ncdf.lookupVar(fc->name())->data2double();
    for (int i=0; i<numOfObs_; i++)
    {
      p[2*i    ] = m->getElement(i, 0);
      p[2*i + 1] = m->getElement(i, 1);
    };
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsStd2Contribution(): " + varName + ": cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsStd2Contribution(): " + varName + ": the data were successfully stored in " + 
      ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCalEarthTide(const SgMatrix* contrib, const QString& kind)
{
  vCal_EarthTide_.setKind(kind);
  return storeObsStd1Contribution("Cal_EarthTide", contrib, vCal_EarthTide_, fcfCalEarthTide);
};



//
bool SgVgosDb::storeObsCalFeedCorr(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_FeedCorrection", contrib, vCal_FeedCorrection_, 
    fcfCalFeedCorrection);
};



//
bool SgVgosDb::storeObsCalOcean(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_OceanLoad", contrib, vCal_OceanLoad_, fcfCalOceanLoad);
};



//
bool SgVgosDb::storeObsCalOceanOld(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_OceanLoadOld", contrib, vCal_OceanLoadOld_, fcfCalOceanLoadOld);
};



//
bool SgVgosDb::storeObsCalOceanPoleTideLoad(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_OceanPoleTideLoad", contrib, vCal_OceanPoleTideLoad_,
    fcfCalOceanPoleTideLoad);
};



//
bool SgVgosDb::storeObsCalPoleTide(const SgMatrix* contrib, const QString& kind)
{
  vCal_PoleTide_.setKind(kind);
  return storeObsStd1Contribution("Cal_PoleTide", contrib, vCal_PoleTide_, fcfCalPoleTide);
};



//
bool SgVgosDb::storeObsCalPoleTideOldRestore(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_PoleTideOldRestore", contrib, vCal_PoleTideOldRestore_, 
    fcfCalPoleTideOldRestore);
};



//
bool SgVgosDb::storeObsCalTiltRmvr(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_TiltRemover", contrib, vCal_TiltRemover_, fcfCalTiltRemover);
};



//
bool SgVgosDb::storeObsCalBend(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_Bend", contrib, vCal_Bend_, fcfCalBend);
};



//
bool SgVgosDb::storeObsCalBendSun(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_BendSun", contrib, vCal_BendSun_, fcfCalBendSun);
};



//
bool SgVgosDb::storeObsCalBendSunHigher(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_BendSunHigher", contrib, vCal_BendSunHigher_, 
    fcfCalBendSunHigher);
};



//
bool SgVgosDb::storeObsCalParallax(const SgMatrix* contrib)
{
  return storeObsStd1Contribution("Cal_Parallax", contrib, vCal_Parallax_, fcfCalParallax);
};



//
bool SgVgosDb::storeObsCalHiFyErp(const SgMatrix* mUt1, const SgMatrix* mPxy, const QString& kind)
{
  vCal_HfErp_.setKind(kind);
  return storeObsStd2Contribution("Cal_HfErp", 
    QList<const SgMatrix*>() << mUt1 << mPxy, vCal_HfErp_, fcfCalHiFyErp);
};



//
bool SgVgosDb::storeObsCalHiFyLibration(const SgMatrix* mUt1, const SgMatrix* mPxy, const QString& kind)
{
  vCal_HiFreqLibration_.setKind(kind);
  return storeObsStd2Contribution("Cal_HiFreqLibration",
    QList<const SgMatrix*>() << mUt1 << mPxy, vCal_HiFreqLibration_, fcfCalHiFreqLibration);
};



//
bool SgVgosDb::storeObsCalWobble(const SgMatrix* mPx, const SgMatrix* mPy)
{
  return storeObsStd2Contribution("Cal_Wobble",
    QList<const SgMatrix*>() << mPx << mPy, vCal_Wobble_, fcfCalWobble);
};



//
bool SgVgosDb::storeObsFractC(const SgVector* v)
{
  SgVdbVariable                &var=progSolveDescriptor_.vFractC_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  if (numOfObs_ != (int)v->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalUnphase(): vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, v->n()));
    return false;
  };
  if (!setupFormat(fcfFractC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsFractC(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcFractC.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
    p[i] = v->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsFractC(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsFractC(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsCalUnphase(const QString &band,
  const SgMatrix* phaseCal_1, const SgMatrix* phaseCal_2)
{
  if (!bandDataByName_.contains(band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeObsCalUnphase(): the band \"" + band + "\" is not registered");
    return false;
  };
  SgVdbVariable                &var=bandDataByName_[band].vCal_Unphase_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != (int)phaseCal_1->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalUnphase(): matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, phaseCal_1->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalUnphase, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalUnphase(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are calculated from correlator output", "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCalUnPhase.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[4*i    ] = phaseCal_1->getElement(i, 0);
    p[4*i + 1] = phaseCal_2->getElement(i, 0);
    p[4*i + 2] = phaseCal_1->getElement(i, 1);
    p[4*i + 3] = phaseCal_2->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsCalUnphase(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsCalUnphase(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};




//
bool SgVgosDb::storeObsPartBend(const SgMatrix* partBend)
{
  SgVdbVariable                &var=vPart_Bend_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)partBend->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartBend(): matrix partBend size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, partBend->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_Bend, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartBend(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartBend.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = partBend->getElement(i, 0);
    p[2*i + 1] = partBend->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartBend(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartBend(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartEOP(const SgMatrix* dV_dPx, const SgMatrix* dV_dPy, const SgMatrix* dV_dUT1)
{
  SgVdbVariable                &var=vPart_Erp_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)dV_dPx->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): matrix dV_dPx size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dPx->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dV_dPy->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): matrix dV_dPy size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dPy->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dV_dUT1->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): matrix dV_dUT1 size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dUT1->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_Erp, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *pP=ncdf.lookupVar(fcPartWobble.name())->data2double();
  double                       *pU=ncdf.lookupVar(fcPartUt1.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    pP[4*i    ] = dV_dPx->getElement(i, 0);
    pP[4*i + 1] = dV_dPy->getElement(i, 0);
    pP[4*i + 2] = dV_dPx->getElement(i, 1);
    pP[4*i + 3] = dV_dPy->getElement(i, 1);
  
    pU[4*i    ] = dV_dUT1->getElement(i, 0);
    pU[4*i + 1] = dV_dUT1->getElement(i, 1);
    pU[4*i + 2] = dV_dUT1->getElement(i, 2);
    pU[4*i + 3] = dV_dUT1->getElement(i, 3);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartEOP(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartNut2KXY(const SgMatrix* dV_dCipX, const SgMatrix* dV_dCipY,
  const QString& kind)
{
  SgVdbVariable                &var=vPart_NutationNro_;
  //
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)dV_dCipX->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartNut2KXY(): matrix dV_dCipX size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dCipX->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dV_dCipY->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartNut2KXY(): matrix dV_dCipY size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dCipY->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_NutationNro, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartNut2KXY(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartNutationNro.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[4*i    ] = dV_dCipX->getElement(i, 0);
    p[4*i + 1] = dV_dCipY->getElement(i, 0);
    p[4*i + 2] = dV_dCipX->getElement(i, 1);
    p[4*i + 3] = dV_dCipY->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartNut2KXY(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartNut2KXY(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartRaDec(const SgMatrix* dV_dRA, const SgMatrix* dV_dDN)
{
  SgVdbVariable                &var=vPart_RaDec_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)dV_dRA->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartRaDec(): matrix dV_dRA size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dRA->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dV_dDN->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartRaDec(): matrix dV_dDN size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dDN->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_RaDec, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartRaDec(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartRaDec.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[4*i    ] = dV_dRA->getElement(i, 0);
    p[4*i + 1] = dV_dDN->getElement(i, 0);
    p[4*i + 2] = dV_dRA->getElement(i, 1);
    p[4*i + 3] = dV_dDN->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartRaDec(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartRaDec(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//dTau_dR_2 = -dTau_dR_1
bool SgVgosDb::storeObsPartXYZ(const SgMatrix* dDel_dR_1, const SgMatrix* dRat_dR_1)
{
  SgVdbVariable                &var=vPart_Xyz_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)dDel_dR_1->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartXYZ(): matrix dDel_dR_1 size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dDel_dR_1->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dRat_dR_1->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartXYZ(): matrix dRat_dR_1 size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dRat_dR_1->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_Xyz, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartXYZ(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartXyz.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[6*i    ] = dDel_dR_1->getElement(i, 0);
    p[6*i + 1] = dDel_dR_1->getElement(i, 1);
    p[6*i + 2] = dDel_dR_1->getElement(i, 2);
    p[6*i + 3] = dRat_dR_1->getElement(i, 0);
    p[6*i + 4] = dRat_dR_1->getElement(i, 1);
    p[6*i + 5] = dRat_dR_1->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartXYZ(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartXYZ(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartGamma(const SgMatrix* part)
{
  SgVdbVariable                &var=vPart_Gamma_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)part->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartGamma(): matrix part size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, part->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_Gamma, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartGamma(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartGamma.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = part->getElement(i, 0);
    p[2*i + 1] = part->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartGamma(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartGamma(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartParallax(const SgMatrix* part)
{
  SgVdbVariable                &var=vPart_Parallax_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)part->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartParallax(): matrix part size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, part->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_Parallax, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartParallax(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartParallax.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[2*i    ] = part->getElement(i, 0);
    p[2*i + 1] = part->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartParallax(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartParallax(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeObsPartPoleTides(const SgMatrix* dV_dPtdX, const SgMatrix* dV_dPtdY,
  const QString& kind)
{
  SgVdbVariable                &var=vPart_PoleTide_;
  //
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfObs_ != (int)dV_dPtdX->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartPoleTides(): matrix dV_dPtdX size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dPtdX->nRow()));
    return false;
  };
  if (numOfObs_ != (int)dV_dPtdY->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartPoleTides(): matrix dV_dPtdY size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, dV_dPtdY->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPart_PolarTide, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartPoleTides(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Obs", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartPolarTide.name())->data2double();
  for (int i=0; i<numOfObs_; i++)
  {
    p[4*i    ] = dV_dPtdX->getElement(i, 0);
    p[4*i + 1] = dV_dPtdY->getElement(i, 0);
    p[4*i + 2] = dV_dPtdX->getElement(i, 1);
    p[4*i + 3] = dV_dPtdY->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeObsPartPoleTides(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeObsPartPoleTides(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};






/*=====================================================================================================*/

