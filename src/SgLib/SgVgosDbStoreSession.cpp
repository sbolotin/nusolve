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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>


#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgModelsInfo.h>
#include <SgVgosDb.h>
#include <SgVlbiHistory.h>
#include <SgVlbiStationInfo.h>





/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- storeSession part of vgosDb data tree)                         */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
bool SgVgosDb::storeAtmSetup(const SgVector* interval, const SgVector* rateConstraint, 
  const QList<QString> &sites)
{
  SgVdbVariable                &var=progSolveDescriptor_.vAtmSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  // adjust dimensions:
  fcAtmInterval       .alternateDimension(0, interval->n());
  fcAtmRateConstraint .alternateDimension(0, rateConstraint->n());
  fcAtmRateSite       .alternateDimension(0, sites.size());
  if (!setupFormat(fcfAtmSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAtmSetup(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcAtmInterval       .alternateDimension(0, SD_Any);
  fcAtmRateConstraint .alternateDimension(0, SD_Any);
  fcAtmRateSite       .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are specified by user", "", "");
  //
  double                       *pT=ncdf.lookupVar(fcAtmInterval      .name())->data2double();
  double                       *pC=ncdf.lookupVar(fcAtmRateConstraint.name())->data2double();
  char                         *pS=ncdf.lookupVar(fcAtmRateSite      .name())->data2char  ();
  int                           len=fcAtmRateSite.dims().at(1);
  for (unsigned int i=0; i<interval->n(); i++)
    pT[i] = interval->getElement(i);
  for (unsigned int i=0; i<rateConstraint->n(); i++)
    pC[i] = rateConstraint->getElement(i);
  for (int i=0; i<sites.size(); i++)
    strncpy(pS + len*i, qPrintable(sites.at(i)), len);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAtmSetup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeAtmSetup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeClockSetup(const SgVector* interval, const SgVector* rateConstraint,
  const QList<QString> &sites, const QList<QString> &refSites)
{
  SgVdbVariable                &var=progSolveDescriptor_.vClockSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  FmtChkVar                     fcRefClocks("ReferenceClock", NC_CHAR,  false, QList<int>(),
                                            "CLK_SITS", "List of clock reference stations");
  QList<SgVgosDb::FmtChkVar*>  fcf(fcfClockSetup);
  //
  // adjust dimensions:
  if (refSites.size()==1)
    fcRefClocks.addDimension(8);
  else if (refSites.size()>1)
  {
    fcRefClocks.addDimension(refSites.size());
    fcRefClocks.addDimension(8);
  };
  if (refSites.size()>0)
    fcf << &fcRefClocks;
  fcClockInterval       .alternateDimension(0, interval->n());
  fcClockRateConstraint .alternateDimension(0, rateConstraint->n());
  fcClockRateName       .alternateDimension(0, sites.size());
  if (!setupFormat(fcf, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockSetup(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcClockInterval       .alternateDimension(0, SD_Any);
  fcClockRateConstraint .alternateDimension(0, SD_Any);
  fcClockRateName       .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are specified by user", "", "");
  //
  double                       *pT=ncdf.lookupData2double(fcClockInterval      .name());
  double                       *pC=ncdf.lookupData2double(fcClockRateConstraint.name());
  char                         *pS=ncdf.lookupData2char  (fcClockRateName      .name());
  char                         *pR=ncdf.lookupData2char  (fcRefClocks          .name());
  int                           len=fcClockRateName.dims().at(1);
  for (unsigned int i=0; i<interval->n(); i++)
    pT[i] = interval->getElement(i);
  for (unsigned int i=0; i<rateConstraint->n(); i++)
    pC[i] = rateConstraint->getElement(i);
  for (int i=0; i<sites.size(); i++)
    strncpy(pS + len*i, qPrintable(sites.at(i)), len);
  for (int i=0; i<refSites.size(); i++)
    strncpy(pR + len*i, qPrintable(refSites.at(i)), len);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockSetup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeClockSetup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeErpSetup(double pmOffsetConstraint, double utOffsetConstraint)
{
  SgVdbVariable                &var=progSolveDescriptor_.vErpSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  // adjust dimensions:
  if (!setupFormat(fcfErpSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpSetup(): format description failed");
    return false;
  };
  // restore the dimensions:
  ncdf.setServiceVars(var.getStub(), "The data are specified by user", "", "");
  //
  *ncdf.lookupVar(fcPmOffsetConstraint.name())->data2double() = pmOffsetConstraint;
  *ncdf.lookupVar(fcUtOffsetConstraint.name())->data2double() = utOffsetConstraint;
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpSetup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeErpSetup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeIonoSetup(const QList<int> &flags, const QList<QString> &sites)
{
  SgVdbVariable                &var=progSolveDescriptor_.vIonoSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  // adjust dimensions:
  if (!setupFormat(fcfIonoSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeIonoSetup(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The data are set by software", "", "");
  //
  char                         *pS=ncdf.lookupVar(fcIonoStationList.name())->data2char ();
  short                        *pF=ncdf.lookupVar(fcIonoSolveFlag  .name())->data2short();
  short                         len=8;
  for (int i=0; i<sites.size(); i++)
  {
    strncpy(pS + len*i, qPrintable(sites.at(i)), len);
    pF[i] = flags.at(i);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeIonoSetup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeIonoSetup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeSolveWeights(const QVector<QString> &baselineNames, SgMatrix* groupBlWeights)
{
  SgVdbVariable                &var=vGroupBLWeights_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  int                           numOfBaselines=baselineNames.size();
  // checking:
  if (!groupBlWeights)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSolveWeights(): cannot put data into " + ncdf.getFileName() + ": groupBlWeights is NULL");
    return false;
  };
  if (numOfBaselines != (int)groupBlWeights->nCol())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSolveWeights(): cannot put data into " + ncdf.getFileName() + 
      ": baselineNames (" + QString("").setNum(baselineNames.size()) + ") and groupBlWeights (" + 
      QString("").setNum((int)groupBlWeights->nCol()) + ") dimensions missmatch");
    return false;
  };
  //
  // adjust dimensions:
  fcGroupBLWeights    .alternateDimension(1, numOfBaselines);
  fcGroupBLWeightName .alternateDimension(0, numOfBaselines);
  if (!setupFormat(fcfGroupBLWeights, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSolveWeights(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcGroupBLWeights    .alternateDimension(1, SD_Any);
  fcGroupBLWeightName .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are generated by the software", "", "");
  //
  double                       *pW=ncdf.lookupData2double(fcGroupBLWeights   .name());
  char                         *pS=ncdf.lookupData2char  (fcGroupBLWeightName.name());
  int                           len=fcGroupBLWeightName.dims().at(2);
  for (int i=0; i<numOfBaselines; i++)
  {
    pW[                 i] = groupBlWeights->getElement(0, i);
    pW[numOfBaselines + i] = groupBlWeights->getElement(1, i);
    strncpy(pS + 2*len*i      , qPrintable(baselineNames.at(i).mid(0,8)), len);
    strncpy(pS + 2*len*i + len, qPrintable(baselineNames.at(i).mid(9,8)), len);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSolveWeights(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeSolveWeights(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeSelectionStatus(const QVector<int> &sourcesSelection,
  const QVector< QVector<int> > &stationsSelection)
{
  SgVdbVariable                &var=progSolveDescriptor_.vSelectionStatus_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (!setupFormat(fcfSelectionStatus, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSelectionStatus(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The data are assigned by the user", "", "");
  //
  short                        *pS=ncdf.lookupData2short(fcSrcSelectionFlag.name());
  short                        *pB=ncdf.lookupData2short(fcBlnSelectionFlag.name());
  for (int i=0; i<numOfSrc_; i++)
  {
    pS[i] = sourcesSelection.at(i);
  };
  for (int i=0; i<numOfStn_; i++)
  {
    for (int j=0; j<numOfStn_; j++)
      pB[numOfStn_*j + i] = stationsSelection.at(i).at(j);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSelectionStatus(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeSelectionStatus(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeBaselineClockSetup(const QList<QString>& baselines)
{
  if (!baselines.size())
    return true;
  SgVdbVariable                &var=progSolveDescriptor_.vBaselineClockSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  // adjust dimension:
  fcBaselineClock.alternateDimension(0, baselines.size());
  if (!setupFormat(fcfBlnClockSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeBaselineClockSetup(): format description failed");
    return false;
  };
  // restore the dimension:
  fcBaselineClock.alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are assigned by the user", "", "");
  //
  char                         *pS=ncdf.lookupData2char (fcBaselineClock.name());
  int                           len=fcBaselineClock.dims().at(2);
  for (int i=0; i<baselines.size(); i++)
  {
    strncpy(pS + 2*len*i      , qPrintable(baselines.at(i).mid(0,8)), len);
    strncpy(pS + 2*len*i + len, qPrintable(baselines.at(i).mid(9,8)), len);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeBaselineClockSetup(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeBaselineClockSetup(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeEccentricities(const QVector<QString> &stationsNames,
  const QVector<QString> &eccTypes, const QVector<QString> &eccNums, SgMatrix* eccVals, 
  const QString &eccFileName)
{
  SgVdbVariable                &var=vEccentricity_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (!setupFormat(fcfEccentricity, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEccentricities(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The data are extracted from the file " + eccFileName, "", "");
  //
  char                         *pT=ncdf.lookupData2char  (fcEccentricityType    .name());
  char                         *pS=ncdf.lookupData2char  (fcEccentricityName    .name());
  char                         *pM=ncdf.lookupData2char  (fcEccentricityMonument.name());
  double                       *pV=ncdf.lookupData2double(fcEccentricityVector  .name());
  int                           lT=fcEccentricityType     .dims().at(1);
  int                           lS=fcEccentricityName     .dims().at(1);
  int                           lM=fcEccentricityMonument .dims().at(1);
  for (int i=0; i<numOfStn_; i++)
  {
    strncpy(pT + lT*i, qPrintable(eccTypes.at(i)),      lT);
    strncpy(pS + lS*i, qPrintable(stationsNames.at(i)), lS);
    strncpy(pM + lM*i, qPrintable(eccNums.at(i)),       lM);
    pV[3*i    ] = eccVals->getElement(i, 0);
    pV[3*i + 1] = eccVals->getElement(i, 1);
    pV[3*i + 2] = eccVals->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEccentricities(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeEccentricities(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeCalibrations(int obsCalFlags, const QVector<int> &statCalFlags,
  const QVector< QVector<int> > &flybyFlags, const QVector<QString> &statCalNames,
  const QVector<QString> &flybyNames, const QVector<QString> &calSiteNames, 
  const QVector<QString> &obsCalNames)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalibrationSetup_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  // adjust dimension:
  fcObsCalName  .alternateDimension(0, obsCalNames.size());
  fcStatCalName .alternateDimension(0, statCalNames.size());
  fcFlybyName   .alternateDimension(0, flybyNames.size());
  //
  if (!setupFormat(fcfCalibrationSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalibrations(): format description failed");
    return false;
  };
  fcObsCalName  .alternateDimension(0, SD_Any);
  fcStatCalName .alternateDimension(0, SD_Any);
  fcFlybyName   .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are the results of the user activity", "", "");
  //
  int                           n, l;
  short                        *p;
  char                         *c;
  //
  *ncdf.lookupData2short(fcObsCalFlag.name()) = obsCalFlags;
  //  fcStatCalFlag:    SD_NumStn,
  p = ncdf.lookupData2short(fcStatCalFlag.name());
  for (int i=0; i<numOfStn_; i++)
    p[i] = statCalFlags[i];
  //  fcFlybyFlag:      SD_NumStn <<  7,
  p = ncdf.lookupData2short(fcFlybyFlag.name());
  l = fcFlybyFlag.dims().at(1);
  for (int iStn=0; iStn<numOfStn_; iStn++)
    for (int j=0; j<l; j++)
      p[l*iStn + j] = (flybyFlags[iStn])[j];
  //  fcStatCalName:    SD_Any << 8,
  c = ncdf.lookupData2char(fcStatCalName.name());
  n = statCalNames.size();
  l = fcStatCalName.dims().at(1);
  for (int i=0; i<n; i++)
    strncpy(c + l*i, qPrintable(statCalNames.at(i)), l);
  //  fcFlybyName:      SD_Any << 8,
  c = ncdf.lookupData2char(fcFlybyName.name());
  n = flybyNames.size();
  l = fcFlybyName.dims().at(1);
  for (int i=0; i<n; i++)
    strncpy(c + l*i, qPrintable(flybyNames.at(i)), l);
  //  fcCalStationName: SD_NumStn <<  8,
  c = ncdf.lookupData2char(fcCalStationName.name());
  l = fcCalStationName.dims().at(1);
  for (int i=0; i<numOfStn_; i++)
    strncpy(c + l*i, qPrintable(calSiteNames.at(i)), l);
  //  fcObsCalName:     SD_Any <<  8,
  c = ncdf.lookupData2char(fcObsCalName.name());
  n = obsCalNames.size();
  l = fcObsCalName.dims().at(1);
  for (int i=0; i<n; i++)
    strncpy(c + l*i, qPrintable(obsCalNames.at(i)), l);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalibrations(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeCalibrations(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeClockBreaks(const QVector<QString> &cbNames, const QVector<SgMJD> &cbEpochs, 
                                const QVector<int> &cbFlags, const QString& band)
{
  SgVdbVariable                &var=vClockBreak_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  // adjust dimensions:
  int                           numCB, stnNameLength;
  numCB = cbNames.size();
  if (!numCB)
    return true;
  stnNameLength = cbNames.at(0).size();
  fcClockBreakFlag  .alternateDimension(0, numCB);
  if (numCB > 1)
    fcClockBreakSite.addDimension(numCB);
  fcClockBreakSite.addDimension(stnNameLength);
  fcClockBreakEpoch .alternateDimension(0, numCB);
  if (!setupFormat(fcfClockBreak, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockBreaks(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcClockBreakFlag  .alternateDimension(0, SD_Any);
  fcClockBreakSite  .clearDimensions();
  fcClockBreakEpoch .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are the results of the user activity", "", "");
  //
  *ncdf.lookupData2short(fcClockBreakNumber.name()) = numCB;
  strncpy(ncdf.lookupData2char(fcClockBreakStatus.name()), "YE", 3);
  //
  short                        *pF=ncdf.lookupData2short (fcClockBreakFlag .name());
  double                       *pT=ncdf.lookupData2double(fcClockBreakEpoch.name());
  char                         *pS=ncdf.lookupData2char  (fcClockBreakSite .name());
  for (int i=0; i<numCB; i++)
  {
    pF[i] = cbFlags.at(i);
    pT[i] = cbEpochs.at(i).toDouble();
    strncpy(pS + stnNameLength*i, qPrintable(cbNames.at(i)), stnNameLength);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockBreaks(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeClockBreaks(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeSessionHead(const QString& corrType, const QString& corrName, const QString& piName,
  const QString& experimentName, int experimentSerialNumber, 
  const QString& experimentDescription, const QString& recordingMode,
  const QList<QString>& stations, const QList<QString>& sources,
  const SgMJD& tStart, const SgMJD& tFinis, int cppsIdx)
{
  SgVdbVariable                &var=vHead_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  // checking:
  if (numOfStn_ != stations.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSessionHead(): station list size mismatch: " + QString("").setNum(numOfStn_) + 
      " vs " + QString("").setNum(stations.size()));
    return false;
  };
  if (numOfSrc_ != sources.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSessionHead(): source list size mismatch: " + QString("").setNum(numOfSrc_) + 
      " vs " + QString("").setNum(sources.size()));
    return false;
  };

  //
  // set up dimensions:
  fcCorrelatorType        .alternateDimension(0, corrType.size());
  fcCorrelator            .alternateDimension(0, corrName.size());
  fcPrincipalInvestigator .alternateDimension(0, piName.size());
  fcExpName               .alternateDimension(0, experimentName.size());
  fcExpDescription        .alternateDimension(0, experimentDescription.size());
  fcRecordingMode         .alternateDimension(0, recordingMode.size());
  //
  if (!setupFormat(fcfHead, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSessionHead(): format description failed");
    return false;
  };
  fcCorrelatorType        .alternateDimension(0, SD_Any);
  fcCorrelator            .alternateDimension(0, SD_Any);
  fcPrincipalInvestigator .alternateDimension(0, SD_Any);
  fcExpName               .alternateDimension(0, SD_Any);
  fcExpDescription        .alternateDimension(0, SD_Any);
  fcRecordingMode         .alternateDimension(0, SD_Any);

  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator output", "", "");
  //
  short                        *pN;
  char                         *pS;
  int                           ep[5], len;
  double                        f;
  
  //
  *ncdf.lookupData2int(fcNumObs       .name())  = numOfObs_;
  *ncdf.lookupData2int(fcNumScan      .name())  = numOfScans_;
  *ncdf.lookupData2short(fcNumSource  .name())  = numOfSrc_;
  *ncdf.lookupData2short(fcNumStation .name())  = numOfStn_;
  
  *ncdf.lookupData2int(fcCorrPostProcSoftware.name()) = cppsIdx;
  //
  pN = ncdf.lookupData2short(fciUTCInterval.name());
  //
  tStart.toYMDHMS_tr(ep[0], ep[1], ep[2], ep[3], ep[4], f);
  for (int i=0; i<5; i++)
    pN[i] = ep[i];
  tFinis.toYMDHMS_tr(ep[0], ep[1], ep[2], ep[3], ep[4], f);
  for (int i=0; i<5; i++)
    pN[i + 5] = ep[i];
  //
  if (stations.size())
  {
    len = stations.at(0).size();
    pS = ncdf.lookupData2char(fcStationList.name());
    for (int i=0; i<stations.size(); i++)
      strncpy(pS + len*i, qPrintable(stations.at(i)), len);
  };
  if (sources.size())
  {
    len = sources.at(0).size();
    pS = ncdf.lookupData2char(fcSourceList.name());
    for (int i=0; i<sources.size(); i++)
      strncpy(pS + len*i, qPrintable(sources.at(i)), len);
  };
  //
  pS = ncdf.lookupData2char(fcCorrelatorType.name());
  strncpy(pS, qPrintable(corrType), corrType.size());
  //
  pS = ncdf.lookupData2char(fcCorrelator.name());
  strncpy(pS, qPrintable(corrName), corrName.size());
  //
  pS = ncdf.lookupData2char(fcPrincipalInvestigator.name());
  strncpy(pS, qPrintable(piName), piName.size());
  //
  pS = ncdf.lookupData2char(fcExpName.name());
  strncpy(pS, qPrintable(experimentName), experimentName.size());
  //
  *ncdf.lookupData2short(fcExpSerialNumber.name()) = experimentSerialNumber;
  //
  pS = ncdf.lookupData2char(fcExpDescription.name());
  strncpy(pS, qPrintable(experimentDescription), experimentDescription.size());
  //
  pS = ncdf.lookupData2char(fcRecordingMode.name());
  strncpy(pS, qPrintable(recordingMode), recordingMode.size());
  //
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSessionHead(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeSessionHead(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeLeapSecond(int leapSeconds)
{
  SgVdbVariable                &var=vLeapSecond_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (!setupFormat(fcfLeapSecond, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeLeapSecond(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "The data are provided by CALC software ", "", "");
  //
  *ncdf.lookupVar(fcLeapSecond.name())->data2short() = leapSeconds;
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeLeapSecond(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  have2redoLeapSecond_ = false;
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeLeapSecond(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeMiscCable(const QMap<QString, QString>& cableSignByKey)
{
  SgVdbVariable                &var=vMiscCable_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  QString                       signs("");
  int                           num=cableSignByKey.size(), l;
  if (num == 0)
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::storeMiscCable(): nothing to write");
    return false;
  };
  //
  // adjust dimensions (mimic db2vgosDB):
  l = 0;
  for (QMap<QString, QString>::const_iterator it=cableSignByKey.begin(); it!=cableSignByKey.end(); ++it)
  {
    if (l < it.key().size())
      l = it.key().size();
    signs += it.value() + " ";
  };
  signs.chop(1);
  if (signs.size() != 2*num - 1)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscCable(): signs size mismatch: " + QString("").setNum(signs.size()) + 
      " vs " + QString("").setNum(2*num - 1));
    return false;
  };
  fcCableStnName.alternateDimension(0, num);
  fcCableStnName.alternateDimension(1, l);
  fcCableSign   .alternateDimension(0, 2*num - 1);
  if (!setupFormat(fcfMiscCable, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscCable(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcCableStnName.alternateDimension(0, SD_Any);
  fcCableStnName.alternateDimension(1, SD_Any);
  fcCableSign   .alternateDimension(0, SD_Any);
  ncdf.setServiceVars(var.getStub(), "The data are exctracted from station log files", "", "");
  //
  char                         *pSgn=ncdf.lookupData2char(fcCableSign.name());
  char                         *pStn=ncdf.lookupData2char(fcCableStnName.name());
  int                           idx;
  idx = 0;
  for (QMap<QString, QString>::const_iterator it=cableSignByKey.begin(); 
    it!=cableSignByKey.end(); ++it, idx++)
    strncpy(pStn + l*idx, qPrintable(it.key()), l);
  strncpy(pSgn, qPrintable(signs), signs.size());
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscCable(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeMiscCable(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeMiscFourFit(const QString& band, SgVlbiSessionInfo::OriginType /*originType*/,
  const QVector<QString>& fourfitControlFile, const QVector<QString>& fourfitCommand,
  const QVector<int>& numLags, const QVector<double>& apLength)
{
  SgVdbVariable                *var=NULL;
  if (band.size())
  {
    if (!bandDataByName_.contains(band))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::storeMiscFourFit(): the band \"" + band + "\" is not registered");
      return false;
    };
    var = &bandDataByName_[band].vMiscFourFit_;
  }
  else
    var = &vMiscFourFit_;

  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var->name4export(path2RootDir_, operationMode_, band), currentIdentities_, outputFormatId_,
    sessionCode_, "", band);
  //
  if (numOfObs_ != fourfitControlFile.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscFourFit(): fourfitControlFile vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfObs_, fourfitControlFile.size()));
    return false;
  };
  //
  // set up the dimensions:
  int                           lCmd=0, lCnt=0;
  for (int i=0; i<numOfObs_; i++)
  {
    if (lCmd < fourfitCommand.at(i).size())
      lCmd = fourfitCommand.at(i).size();
    if (lCnt < fourfitControlFile.at(i).size())
      lCnt = fourfitControlFile.at(i).size();
  };
  fcFourFitCmdCString_v1002 .alternateDimension(1, lCmd);
  fcFourfitControlFile_v1002.alternateDimension(1, lCnt);
  //
  if (!setupFormat(fcfMiscFourFit_v1002, ncdf, "", band))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscFourFit(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcFourFitCmdCString_v1002 .alternateDimension(1, SD_Any);
  fcFourfitControlFile_v1002.alternateDimension(1, SD_Any);
  //
  ncdf.setServiceVars(var->getStub(), "Data are extracted from correlator output", "Obs", "TimeUTC.nc");
  //
  char                         *pCmd=ncdf.lookupVar(fcFourFitCmdCString_v1002 .name())->data2char();
  char                         *pCnt=ncdf.lookupVar(fcFourfitControlFile_v1002.name())->data2char();
  short                        *pNum=ncdf.lookupVar(fcNumLagsUsed_v1002       .name())->data2short();
  double                       *pApL=ncdf.lookupVar(fcAplength_v1002          .name())->data2double();
  //
  //    fill data structures:
  //
  for (int i=0; i<numOfObs_; i++)
  {
    // common for all types of correlator:
    pNum[i] = numLags.at(i);
    pApL[i] = apLength.at(i);
    //
    strncpy(pCmd + lCmd*i, qPrintable(fourfitCommand.at(i)), lCmd);
    strncpy(pCnt + lCnt*i, qPrintable(fourfitControlFile.at(i)), lCnt);
  };
  
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeMiscFourFit(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeMiscFourFit(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeCalcInfo(const SgModelsInfo& calcInfo)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalcInfo_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  // set up the dimensions:
  int                           lCtrlName=0, nCtrl=calcInfo.controlFlagNames().size(), numStn;
  for (int i=0; i<nCtrl; i++)
  {
    if (lCtrlName < calcInfo.controlFlagNames().at(i).size())
      lCtrlName = calcInfo.controlFlagNames().at(i).size();
  };
  if (calcInfo.controlFlagValues().size() < nCtrl)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): the sizes of control names and values mismatch");
    nCtrl = calcInfo.controlFlagValues().size();
  };
  int                           lOcnLdSts=0, nOcnLdSts=calcInfo.oLoadStationStatus().size();
  for (int i=0; i<nOcnLdSts; i++)
  {
    if (lOcnLdSts < calcInfo.oLoadStationStatus().at(i).size())
      lOcnLdSts = calcInfo.oLoadStationStatus().at(i).size();
  };
  
  fcCalcControlNames  .alternateDimension(0, nCtrl);
  fcCalcControlNames  .alternateDimension(1, lCtrlName);
  fcCalcControlValues .alternateDimension(0, nCtrl);
  fcATMMessage        .alternateDimension(0, calcInfo.troposphere().getDefinition().size());
  fcATMControl        .alternateDimension(0, calcInfo.troposphere().getControlFlag().size());
  fcAxisOffsetMessage .alternateDimension(0, calcInfo.axisOffset().getDefinition().size());
  fcAxisOffsetControl .alternateDimension(0, calcInfo.axisOffset().getControlFlag().size());
  fcEarthTideMessage  .alternateDimension(0, calcInfo.earthTide().getDefinition().size());
  fcEarthTideControl  .alternateDimension(0, calcInfo.earthTide().getControlFlag().size());
  fcPoleTideMessage   .alternateDimension(0, calcInfo.poleTide().getDefinition().size());
  fcPoleTideControl   .alternateDimension(0, calcInfo.poleTide().getControlFlag().size());
  fcNutationMessage   .alternateDimension(0, calcInfo.nutation().getDefinition().size());
  fcNutationControl   .alternateDimension(0, calcInfo.nutation().getControlFlag().size());
  fcOceanMessage      .alternateDimension(0, calcInfo.oceanLoading().getDefinition().size());
  fcOceanControl      .alternateDimension(0, calcInfo.oceanLoading().getControlFlag().size());
  fcATIMessage        .alternateDimension(0, calcInfo.atomicTime().getDefinition().size());
  fcATIControl        .alternateDimension(0, calcInfo.atomicTime().getControlFlag().size());
  fcCTIMessage        .alternateDimension(0, calcInfo.coordinateTime().getDefinition().size());
  fcCTIControl        .alternateDimension(0, calcInfo.coordinateTime().getControlFlag().size());
  fcParallaxMessage   .alternateDimension(0, calcInfo.parallax().getDefinition().size());
  fcParallaxControl   .alternateDimension(0, calcInfo.parallax().getControlFlag().size());
  fcStarMessage       .alternateDimension(0, calcInfo.star().getDefinition().size());
  fcStarControl       .alternateDimension(0, calcInfo.star().getControlFlag().size());
  fcTheoryMessage     .alternateDimension(0, calcInfo.relativity().getDefinition().size());
  fcRelativityControl .alternateDimension(0, calcInfo.relativity().getControlFlag().size());
  fcSiteMessage       .alternateDimension(0, calcInfo.site().getDefinition().size());
  fcFeedhornMessage   .alternateDimension(0, calcInfo.feedHorn().getDefinition().size());
  fcPepMessage        .alternateDimension(0, calcInfo.ephemeris().getDefinition().size());
  fcWobbleControl     .alternateDimension(0, calcInfo.polarMotion().getControlFlag().size());
  fcUT1Control        .alternateDimension(0, calcInfo.ut1().getControlFlag().size());
  fcOceanStationsFlag .alternateDimension(0, nOcnLdSts);
  fcOceanStationsFlag .alternateDimension(1, lOcnLdSts);
  fcUT1EPOCH          .alternateDimension(1, calcInfo.ut1InterpData()->nRow());
  fcWOBEPOCH          .alternateDimension(1, calcInfo.wobInterpData()->nRow());
  fcSiteZenithDelay   .alternateDimension(0, calcInfo.siteZenDelays().size());

  numStn = calcInfo.stations().size();
  fcOceanPoleTideCoef   .alternateDimension(0, numStn);
  fcOceanUpPhase        .alternateDimension(0, numStn);
  fcOceanHorizontalPhase.alternateDimension(0, numStn);
  fcOceanUpAmp          .alternateDimension(0, numStn);
  fcOceanHorizontalAmp  .alternateDimension(0, numStn);

  //
  if (!setupFormat(fcfCalcInfo, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): format description failed");
    return false;
  };
  // check for allocated data:
  if (!calcInfo.ut1InterpData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): the UT1 interpolation data are not defined");
    return false;
  };
  if (!calcInfo.wobInterpData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): the PM interpolation data are not defined");
    return false;
  };
  //
  // restore the dimensions:
  fcCalcControlNames    .alternateDimension(0, SD_Any);
  fcCalcControlNames    .alternateDimension(1, SD_Any);
  fcCalcControlValues   .alternateDimension(0, SD_Any);
  fcATMMessage          .alternateDimension(0, SD_Any);
  fcATMControl          .alternateDimension(0, SD_Any);
  fcAxisOffsetMessage   .alternateDimension(0, SD_Any);
  fcAxisOffsetControl   .alternateDimension(0, SD_Any);
  fcEarthTideMessage    .alternateDimension(0, SD_Any);
  fcEarthTideControl    .alternateDimension(0, SD_Any);
  fcPoleTideMessage     .alternateDimension(0, SD_Any);
  fcPoleTideControl     .alternateDimension(0, SD_Any);
  fcNutationMessage     .alternateDimension(0, SD_Any);
  fcNutationControl     .alternateDimension(0, SD_Any);
  fcOceanMessage        .alternateDimension(0, SD_Any);
  fcOceanControl        .alternateDimension(0, SD_Any);
  fcATIMessage          .alternateDimension(0, SD_Any);
  fcATIControl          .alternateDimension(0, SD_Any);
  fcCTIMessage          .alternateDimension(0, SD_Any);
  fcCTIControl          .alternateDimension(0, SD_Any);
  fcParallaxMessage     .alternateDimension(0, SD_Any);
  fcParallaxControl     .alternateDimension(0, SD_Any);
  fcStarMessage         .alternateDimension(0, SD_Any);
  fcStarControl         .alternateDimension(0, SD_Any);
  fcTheoryMessage       .alternateDimension(0, SD_Any);
  fcRelativityControl   .alternateDimension(0, SD_Any);
  fcSiteMessage         .alternateDimension(0, SD_Any);
  fcFeedhornMessage     .alternateDimension(0, SD_Any);
  fcPepMessage          .alternateDimension(0, SD_Any);
  fcWobbleControl       .alternateDimension(0, SD_Any);
  fcUT1Control          .alternateDimension(0, SD_Any);
  fcOceanStationsFlag   .alternateDimension(0, SD_NumStn);
  fcOceanStationsFlag   .alternateDimension(1, 4);
  fcUT1EPOCH            .alternateDimension(1, SD_Any);
  fcWOBEPOCH            .alternateDimension(1, SD_Any);
  fcSiteZenithDelay     .alternateDimension(0, SD_NumStn);
  fcOceanPoleTideCoef   .alternateDimension(0, SD_NumStn);
  fcOceanUpPhase        .alternateDimension(0, SD_NumStn);
  fcOceanHorizontalPhase.alternateDimension(0, SD_NumStn);
  fcOceanUpAmp          .alternateDimension(0, SD_NumStn);
  fcOceanHorizontalAmp  .alternateDimension(0, SD_NumStn);

  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software", "", "");
  //
  char                         *pCtrlN=ncdf.lookupVar(fcCalcControlNames    .name())->data2char();
  short                        *pCtrlF=ncdf.lookupVar(fcCalcControlValues   .name())->data2short();
  short                        *pTUt1=ncdf.lookupVar(fcTidalUt1Control      .name())->data2short();
  double                       *pClcV=ncdf.lookupVar(fcCalcVersion          .name())->data2double();
  char                         *pAtmM=ncdf.lookupVar(fcATMMessage           .name())->data2char();
  char                         *pAtmC=ncdf.lookupVar(fcATMControl           .name())->data2char();
  char                         *pAxoM=ncdf.lookupVar(fcAxisOffsetMessage    .name())->data2char();
  char                         *pAxoC=ncdf.lookupVar(fcAxisOffsetControl    .name())->data2char();
  char                         *pEtdM=ncdf.lookupVar(fcEarthTideMessage     .name())->data2char();
  char                         *pEtdC=ncdf.lookupVar(fcEarthTideControl     .name())->data2char();
  char                         *pPtdM=ncdf.lookupVar(fcPoleTideMessage      .name())->data2char();
  char                         *pPtdC=ncdf.lookupVar(fcPoleTideControl      .name())->data2char();
  char                         *pNutM=ncdf.lookupVar(fcNutationMessage      .name())->data2char();
  char                         *pNutC=ncdf.lookupVar(fcNutationControl      .name())->data2char();
  char                         *pOtdM=ncdf.lookupVar(fcOceanMessage         .name())->data2char();
  char                         *pOtdC=ncdf.lookupVar(fcOceanControl         .name())->data2char();
  char                         *pAtiM=ncdf.lookupVar(fcATIMessage           .name())->data2char();
  char                         *pAtiC=ncdf.lookupVar(fcATIControl           .name())->data2char();
  char                         *pCtiM=ncdf.lookupVar(fcCTIMessage           .name())->data2char();
  char                         *pCtiC=ncdf.lookupVar(fcCTIControl           .name())->data2char();
  char                         *pPlxM=ncdf.lookupVar(fcParallaxMessage      .name())->data2char();
  char                         *pPlxC=ncdf.lookupVar(fcParallaxControl      .name())->data2char();
  char                         *pStrM=ncdf.lookupVar(fcStarMessage          .name())->data2char();
  char                         *pStrC=ncdf.lookupVar(fcStarControl          .name())->data2char();
  char                         *pRelM=ncdf.lookupVar(fcTheoryMessage        .name())->data2char();
  char                         *pRelC=ncdf.lookupVar(fcRelativityControl    .name())->data2char();
  char                         *pSitM=ncdf.lookupVar(fcSiteMessage          .name())->data2char();
  char                         *pFdHM=ncdf.lookupVar(fcFeedhornMessage      .name())->data2char();
  char                         *pEphM=ncdf.lookupVar(fcPepMessage           .name())->data2char();
  char                         *pPxyC=ncdf.lookupVar(fcWobbleControl        .name())->data2char();
  char                         *pUt1C=ncdf.lookupVar(fcUT1Control           .name())->data2char();
  char                         *pOSts=ncdf.lookupVar(fcOceanStationsFlag    .name())->data2char();
  double                       *pRelD=ncdf.lookupVar(fcRelativityData       .name())->data2double();
  double                       *pPrcD=ncdf.lookupVar(fcPrecessionData       .name())->data2double();
  double                       *pEtdD=ncdf.lookupVar(fcEarthTideData        .name())->data2double();
  double                       *pUt1E=ncdf.lookupVar(fcUT1EPOCH             .name())->data2double();
  double                       *pWobE=ncdf.lookupVar(fcWOBEPOCH             .name())->data2double();
  double                       *pSitZ=ncdf.lookupVar(fcSiteZenithDelay      .name())->data2double();

  double                       *pOptc=ncdf.lookupVar(fcOceanPoleTideCoef    .name())->data2double();
  double                       *pOvPh=ncdf.lookupVar(fcOceanUpPhase         .name())->data2double();
  double                       *pOhPh=ncdf.lookupVar(fcOceanHorizontalPhase .name())->data2double();
  double                       *pOvAm=ncdf.lookupVar(fcOceanUpAmp           .name())->data2double();
  double                       *pOhAm=ncdf.lookupVar(fcOceanHorizontalAmp   .name())->data2double();
  //
  //    fill data structures:
  //
  for (int i=0; i<nCtrl; i++)
  {
    pCtrlF[i] = calcInfo.controlFlagValues().at(i);
    strncpy(pCtrlN + lCtrlName*i, qPrintable(calcInfo.controlFlagNames().at(i)), lCtrlName);
  };
  for (int i=0; i<nOcnLdSts; i++)
    strncpy(pOSts + lOcnLdSts*i, qPrintable(calcInfo.oLoadStationStatus().at(i)), lOcnLdSts);
  //
  strncpy(pAtmM, qPrintable(calcInfo.troposphere().getDefinition()), 
                            calcInfo.troposphere().getDefinition().size());
  strncpy(pAtmC, qPrintable(calcInfo.troposphere().getControlFlag()), 
                            calcInfo.troposphere().getControlFlag().size());
  strncpy(pAxoM, qPrintable(calcInfo.axisOffset().getDefinition()), 
                            calcInfo.axisOffset().getDefinition().size());
  strncpy(pAxoC, qPrintable(calcInfo.axisOffset().getControlFlag()), 
                            calcInfo.axisOffset().getControlFlag().size());
  strncpy(pEtdM, qPrintable(calcInfo.earthTide().getDefinition()), 
                            calcInfo.earthTide().getDefinition().size());
  strncpy(pEtdC, qPrintable(calcInfo.earthTide().getControlFlag()), 
                            calcInfo.earthTide().getControlFlag().size());
  strncpy(pPtdM, qPrintable(calcInfo.poleTide().getDefinition()), 
                            calcInfo.poleTide().getDefinition().size());
  strncpy(pPtdC, qPrintable(calcInfo.poleTide().getControlFlag()), 
                            calcInfo.poleTide().getControlFlag().size());
  strncpy(pNutM, qPrintable(calcInfo.nutation().getDefinition()), 
                            calcInfo.nutation().getDefinition().size());
  strncpy(pNutC, qPrintable(calcInfo.nutation().getControlFlag()), 
                            calcInfo.nutation().getControlFlag().size());
  strncpy(pOtdM, qPrintable(calcInfo.oceanLoading().getDefinition()), 
                            calcInfo.oceanLoading().getDefinition().size());
  strncpy(pOtdC, qPrintable(calcInfo.oceanLoading().getControlFlag()), 
                            calcInfo.oceanLoading().getControlFlag().size());
  strncpy(pAtiM, qPrintable(calcInfo.atomicTime().getDefinition()), 
                            calcInfo.atomicTime().getDefinition().size());
  strncpy(pAtiC, qPrintable(calcInfo.atomicTime().getControlFlag()), 
                            calcInfo.atomicTime().getControlFlag().size());
  strncpy(pCtiM, qPrintable(calcInfo.coordinateTime().getDefinition()), 
                            calcInfo.coordinateTime().getDefinition().size());
  strncpy(pCtiC, qPrintable(calcInfo.coordinateTime().getControlFlag()), 
                            calcInfo.coordinateTime().getControlFlag().size());
  strncpy(pPlxM, qPrintable(calcInfo.parallax().getDefinition()), 
                            calcInfo.parallax().getDefinition().size());
  strncpy(pPlxC, qPrintable(calcInfo.parallax().getControlFlag()), 
                            calcInfo.parallax().getControlFlag().size());
  strncpy(pStrM, qPrintable(calcInfo.star().getDefinition()),
                            calcInfo.star().getDefinition().size());
  strncpy(pStrC, qPrintable(calcInfo.star().getControlFlag()), 
                            calcInfo.star().getControlFlag().size());
  strncpy(pRelM, qPrintable(calcInfo.relativity().getDefinition()), 
                            calcInfo.relativity().getDefinition().size());
  strncpy(pRelC, qPrintable(calcInfo.relativity().getControlFlag()), 
                            calcInfo.relativity().getControlFlag().size());
  strncpy(pSitM, qPrintable(calcInfo.site().getDefinition()), 
                            calcInfo.site().getDefinition().size());
  strncpy(pFdHM, qPrintable(calcInfo.feedHorn().getDefinition()), 
                            calcInfo.feedHorn().getDefinition().size());
  strncpy(pEphM, qPrintable(calcInfo.ephemeris().getDefinition()), 
                            calcInfo.ephemeris().getDefinition().size());
  strncpy(pPxyC, qPrintable(calcInfo.polarMotion().getControlFlag()), 
                            calcInfo.polarMotion().getControlFlag().size());
  strncpy(pUt1C, qPrintable(calcInfo.ut1().getControlFlag()), 
                            calcInfo.ut1().getControlFlag().size());

  *pTUt1 = calcInfo.getFlagTidalUt1();
  *pClcV = calcInfo.getDversion();

  *pRelD = calcInfo.getRelativityData();
  *pPrcD = calcInfo.getPrecessionData();
  pEtdD[0] = calcInfo.getEarthTideData(0);
  pEtdD[1] = calcInfo.getEarthTideData(1);
  pEtdD[2] = calcInfo.getEarthTideData(2);

  for (unsigned int i=0; i<calcInfo.ut1InterpData()->nRow(); i++)
    for (unsigned int j=0; j<calcInfo.ut1InterpData()->nCol(); j++)
      pUt1E[calcInfo.ut1InterpData()->nCol()*i + j] = calcInfo.ut1InterpData()->getElement(i, j);
  for (unsigned int i=0; i<calcInfo.wobInterpData()->nRow(); i++)
    for (unsigned int j=0; j<calcInfo.wobInterpData()->nCol(); j++)
      pWobE[calcInfo.wobInterpData()->nCol()*i + j] = calcInfo.wobInterpData()->getElement(i, j);

  for (int i=0; i<calcInfo.siteZenDelays().size(); i++)
    pSitZ[i] = calcInfo.siteZenDelays().at(i);
  //
  for (int i=0; i<numStn; i++)
  {
    SgVlbiStationInfo          *si=calcInfo.stations().at(i);
    if (si)
    {
      for (int j=0; j<11; j++)
      {
        pOvPh[11*i      + j] = si->getOLoadPhase(j, 0);
        pOhPh[22*i      + j] = si->getOLoadPhase(j, 1);
        pOhPh[22*i + 11 + j] = si->getOLoadPhase(j, 2);
        pOvAm[11*i      + j] = si->getOLoadAmplitude(j, 0);
        pOhAm[22*i      + j] = si->getOLoadAmplitude(j, 1);
        pOhAm[22*i + 11 + j] = si->getOLoadAmplitude(j, 2);
      };
      //
      for (int j=0; j<6; j++)
        pOptc[6*i + j] = si->getOptLoadCoeff(j);
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::storeCalcInfo(): the stnInfo is NULL for idx=" + QString("").setNum(i));
  };

  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeCalcInfo(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeCalcEop(const SgVector* pmArrayInfo, const SgVector* utArrayInfo, 
  const SgMatrix* pmValues, const SgVector* utValues, 
  const SgModelsInfo& calcInfo)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalcErp_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  // set up the dimensions:
  int                           nPmArrayInfo, nUtArrayInfo, nPmValues, nUtValues;
  nPmArrayInfo = pmArrayInfo->n();
  nUtArrayInfo = utArrayInfo->n();
  nPmValues = pmValues->nRow();
  nUtValues = utValues->n();
  
  if (fcUT1ArrayInfo.dims().at(0) != nUtArrayInfo)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcEop(): size mismatch for UT1ArrayInfo: " + 
      QString("").sprintf("%d vs %d", fcUT1ArrayInfo.dims().at(0), nUtArrayInfo));
    return false;
  };
  if (fcWobArrayInfo.dims().at(0) != nPmArrayInfo)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcEop(): size mismatch for WobArrayInfo: " + 
      QString("").sprintf("%d vs %d", fcWobArrayInfo.dims().at(0), nPmArrayInfo));
    return false;
  };
  fcUT1Values     .alternateDimension(0, nUtValues);
  fcWobValues     .alternateDimension(0, nPmValues);
  fcUT1IntrpMode  .alternateDimension(0, calcInfo.ut1Interpolation().getControlFlag().size());
  fcCalcUt1Module .alternateDimension(0, calcInfo.ut1Interpolation().getDefinition().size());
  fcUT1Origin     .alternateDimension(0, calcInfo.ut1Interpolation().getOrigin().size());
  fcWobIntrpMode  .alternateDimension(0, calcInfo.polarMotionInterpolation().getControlFlag().size());
  fcCalcWobModule .alternateDimension(0, calcInfo.polarMotionInterpolation().getDefinition().size());
  fcWobbleOrigin  .alternateDimension(0, calcInfo.polarMotionInterpolation().getOrigin().size());
  //
  if (!setupFormat(fcfCalcEop, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcEop(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcUT1Values     .alternateDimension(0, SD_Any);
  fcWobValues     .alternateDimension(0, SD_Any);
  fcUT1IntrpMode  .alternateDimension(0, SD_Any);
  fcCalcUt1Module .alternateDimension(0, SD_Any);
  fcUT1Origin     .alternateDimension(0, SD_Any);
  fcWobIntrpMode  .alternateDimension(0, SD_Any);
  fcCalcWobModule .alternateDimension(0, SD_Any);
  fcWobbleOrigin  .alternateDimension(0, SD_Any);
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software", "", "");
  //
  double                       *pUtAi=ncdf.lookupVar(fcUT1ArrayInfo .name())->data2double();
  double                       *pPmAi=ncdf.lookupVar(fcWobArrayInfo .name())->data2double();
  double                       *pUtVa=ncdf.lookupVar(fcUT1Values    .name())->data2double();
  double                       *pPmVa=ncdf.lookupVar(fcWobValues    .name())->data2double();
  char                         *pUtMd=ncdf.lookupVar(fcUT1IntrpMode .name())->data2char();
  char                         *pUtMo=ncdf.lookupVar(fcCalcUt1Module.name())->data2char();
  char                         *pUtOr=ncdf.lookupVar(fcUT1Origin    .name())->data2char();
  char                         *pPmMd=ncdf.lookupVar(fcWobIntrpMode .name())->data2char();
  char                         *pPmMo=ncdf.lookupVar(fcCalcWobModule.name())->data2char();
  char                         *pPmOr=ncdf.lookupVar(fcWobbleOrigin .name())->data2char();
  //
  //    fill data structures:
  //
  for (int i=0; i<nPmArrayInfo; i++)
    pPmAi[i] = pmArrayInfo->getElement(i);
  for (int i=0; i<nUtArrayInfo; i++)
    pUtAi[i] = utArrayInfo->getElement(i);
  for (int i=0; i<nPmValues; i++)
  {
    pPmVa[2*i    ] = pmValues->getElement(i, 0);
    pPmVa[2*i + 1] = pmValues->getElement(i, 1);
  };
  for (int i=0; i<nUtValues; i++)
    pUtVa[i] = utValues->getElement(i);
  //
  strncpy(pUtMd, qPrintable(calcInfo.ut1Interpolation().getControlFlag()), 
                            calcInfo.ut1Interpolation().getControlFlag().size());
  strncpy(pUtMo, qPrintable(calcInfo.ut1Interpolation().getDefinition()), 
                            calcInfo.ut1Interpolation().getDefinition().size());
  strncpy(pUtOr, qPrintable(calcInfo.ut1Interpolation().getOrigin()), 
                            calcInfo.ut1Interpolation().getOrigin().size());
  strncpy(pPmMd, qPrintable(calcInfo.polarMotionInterpolation().getControlFlag()), 
                            calcInfo.polarMotionInterpolation().getControlFlag().size());
  strncpy(pPmMo, qPrintable(calcInfo.polarMotionInterpolation().getDefinition()), 
                            calcInfo.polarMotionInterpolation().getDefinition().size());
  strncpy(pPmOr, qPrintable(calcInfo.polarMotionInterpolation().getOrigin()), 
                            calcInfo.polarMotionInterpolation().getOrigin().size());
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeCalcEop(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeCalcEop(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//--
//
bool SgVgosDb::storeSourcesApriories(const QList<QString> &sourcesNames, 
  const QList<QString> &sourcesRefs, const SgMatrix* cooRaDe)
{
  SgVdbVariable                &var=vSourceApriori_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfSrc_ == 0)
    numOfSrc_ = sourcesNames.size();
  else if (sourcesNames.size() != numOfSrc_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): size mismatch for sourcesNames: " + 
      QString("").sprintf("%d vs %d", sourcesNames.size(), numOfSrc_));
    return false;
  };
  if (sourcesRefs.size() != numOfSrc_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): size mismatch for sourcesRefs: " + 
      QString("").sprintf("%d vs %d", sourcesRefs.size(), numOfSrc_));
    return false;
  };
  if (cooRaDe->nRow() != (unsigned int)numOfSrc_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): size mismatch for sourcesCoords: " + 
      QString("").sprintf("%d vs %d", cooRaDe->nRow(), numOfSrc_));
    return false;
  };
  //
  // set up the dimensions:
  int                           lRef=0, lNam=0;
  for (int i=0; i<numOfSrc_; i++)
  {
    if (lRef < sourcesRefs.at(i).size())
      lRef = sourcesRefs.at(i).size();
    if (lNam < sourcesNames.at(i).size())
      lNam = sourcesNames.at(i).size();
  };
  fcSourceNameApriori .alternateDimension(1, lNam);
  fcSourceReference   .alternateDimension(1, lRef);
  //
  if (!setupFormat(fcfSourceApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcSourceNameApriori .alternateDimension(1, 8);
  fcSourceReference   .alternateDimension(1, SD_Any);
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software", "", "");
  //
  double                       *pCoor=ncdf.lookupVar(fcSource2000RaDec  .name())->data2double();
  char                         *pName=ncdf.lookupVar(fcSourceNameApriori.name())->data2char();
  char                         *pRefr=ncdf.lookupVar(fcSourceReference  .name())->data2char();
  //
  //    fill data structures:
  //
  for (int i=0; i<numOfSrc_; i++)
  {
    pCoor[2*i    ] = cooRaDe->getElement(i, 0);
    pCoor[2*i + 1] = cooRaDe->getElement(i, 1);
    strncpy(pName + lNam*i, qPrintable(sourcesNames.at(i)), lNam);
    strncpy(pRefr + lRef*i, qPrintable(sourcesRefs .at(i)), lRef);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeSourcesApriories(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeClockApriories(const QList<QString> &siteNames, const SgMatrix* offsets_n_rates)
{
  SgVdbVariable                &var=vClockApriori_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (siteNames.size() != (int)offsets_n_rates->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockApriories(): size of siteNames (" + QString("").setNum(siteNames.size()) + 
      ") and offset/rates (" + QString("").setNum(offsets_n_rates->nRow()) + ") mismatch"); 
    return false;
  };
  //
  // set up the dimensions:
  int                           nS=siteNames.size();
  int                           lNam=0;
  for (int i=0; i<nS; i++)
    if (lNam < siteNames.at(i).size())
      lNam = siteNames.at(i).size();

  fcClockAprioriSite  .alternateDimension(0, nS);
  fcClockAprioriSite  .alternateDimension(1, lNam);
  fcClockAprioriOffset.alternateDimension(0, nS);
  fcClockAprioriRate  .alternateDimension(0, nS);
  //
  if (!setupFormat(fcfClockApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockApriories(): format description failed");
    return false;
  };
  // restore the dimensions:
  fcClockAprioriSite  .alternateDimension(0, SD_Any);
  fcClockAprioriSite  .alternateDimension(1, 8);
  fcClockAprioriOffset.alternateDimension(0, SD_Any);
  fcClockAprioriRate  .alternateDimension(0, SD_Any);
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by a user or similar device", "", "");
  //
  char                         *pS=ncdf.lookupVar(fcClockAprioriSite  .name())->data2char();
  double                       *pO=ncdf.lookupVar(fcClockAprioriOffset.name())->data2double();
  double                       *pR=ncdf.lookupVar(fcClockAprioriRate  .name())->data2double();
  //
  //    fill data structures:
  //
  for (int i=0; i<nS; i++)
  {
    pO[i] = offsets_n_rates->getElement(i, 0);
    pR[i] = offsets_n_rates->getElement(i, 1);
    strncpy(pS + lNam*i, qPrintable(siteNames.at(i)), lNam);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeClockApriories(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeClockApriories(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationsApriories(const QList<QString> &stationsNames,
  const SgMatrix* cooXYZ, const QList<QString> &tectonicPlateNames)
{
  SgVdbVariable                &var=vStationApriori_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (numOfStn_ == 0)
    numOfStn_ = stationsNames.size();
  else if (stationsNames.size() != numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): size mismatch for sourcesNames: " + 
      QString("").sprintf("%d vs %d", stationsNames.size(), numOfStn_));
    return false;
  };
  if (tectonicPlateNames.size() != numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): size mismatch for the list of tectonic plate names: " + 
      QString("").sprintf("%d vs %d", tectonicPlateNames.size(), numOfStn_));
    return false;
  };
  if (cooXYZ->nRow() != (unsigned int)numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): size mismatch for cooXYZ: " + 
      QString("").sprintf("%d vs %d", cooXYZ->nRow(), numOfStn_));
    return false;
  };
  //
  // set up the dimensions:
  int                           lNam = fcStationNameApriori.dims().at(1);
  int                           lTpl = fcStationPlateName.dims().at(1);
  if (!setupFormat(fcfStationApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software", "", "");
  //
  double                       *pCoor=ncdf.lookupVar(fcStationXYZ         .name())->data2double();
  char                         *pName=ncdf.lookupVar(fcStationNameApriori .name())->data2char();
  char                         *pTplt=ncdf.lookupVar(fcStationPlateName   .name())->data2char();
  //
  //    fill data structures:
  //
  for (int i=0; i<numOfStn_; i++)
  {
    pCoor[3*i    ] = cooXYZ->getElement(i, 0);
    pCoor[3*i + 1] = cooXYZ->getElement(i, 1);
    pCoor[3*i + 2] = cooXYZ->getElement(i, 2);
    strncpy(pName + lNam*i, qPrintable(stationsNames      .at(i)), lNam);
    strncpy(pTplt + lTpl*i, qPrintable(tectonicPlateNames .at(i)), lTpl);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationsApriories(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeAntennaApriories(const QList<QString> &stationsNames,
  const QList<int> &axisTypes, const SgVector* axisOffsets, const SgMatrix* axisTilts)
{
  SgVdbVariable                &var=vAntennaApriori_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_);
  //
  if (stationsNames.size() != numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): size mismatch for sourcesNames: " + 
      QString("").sprintf("%d vs %d", stationsNames.size(), numOfStn_));
    return false;
  };
  if (axisTypes.size() != numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): size mismatch for the list of axis types: " + 
      QString("").sprintf("%d vs %d", axisTypes.size(), numOfStn_));
    return false;
  };
  if (axisOffsets->n() != (unsigned int)numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): size mismatch for axisOffsets: " + 
      QString("").sprintf("%d vs %d", axisOffsets->n(), numOfStn_));
    return false;
  };
  if (axisTilts->nRow() != (unsigned int)numOfStn_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): size mismatch for axisTilts: " + 
      QString("").sprintf("%d vs %d", axisTilts->nRow(), numOfStn_));
    return false;
  };
  //
  // set up the dimensions:
  int                           lNam = fcAntennaName.dims().at(1);
  if (!setupFormat(fcfAntennaApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software", "", "");
  //
  double                       *pOffs=ncdf.lookupVar(fcAxisOffset .name())->data2double();
  double                       *pTilt=ncdf.lookupVar(fcAxisTilt   .name())->data2double();
  char                         *pName=ncdf.lookupVar(fcAntennaName.name())->data2char();
  short                        *pType=ncdf.lookupVar(fcAxisType   .name())->data2short();
  //
  //    fill data structures:
  //
  for (int i=0; i<numOfStn_; i++)
  {
    pOffs[i      ] = axisOffsets->getElement(i);
    pTilt[2*i    ] = axisTilts->getElement(i, 0);
    pTilt[2*i + 1] = axisTilts->getElement(i, 1);
    pType[i      ] = axisTypes.at(i);
    strncpy(pName + lNam*i, qPrintable(stationsNames      .at(i)), lNam);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeAntennaApriories(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};





//
//
// ------------------ wrapper file:
// 
//
bool SgVgosDb::composeWrapperFile()
{
  QString                       str;
  QString                       outputFileName("");
  
  createWrapperFileName(outputFileName);
  
  if (operationMode_ == SgNetCdf::OM_REGULAR) // regular output:
  {
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::composeWrapperFile(): the name for the new wrapper file: " + outputFileName);
    if (isNewFile_)
    {
      QDir                      dir(path2RootDir_);
      if (!dir.exists())
      {
//      if (!dir.mkpath(path2RootDir_))
        if (!dir.mkpath(dir.absolutePath()))
        {
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::composeWrapperFile(): cannot create a path to the root directory, " + path2RootDir_);
          return false;
        }
        else
          logger->write(SgLogger::DBG, SgLogger::IO, className() + 
            "::composeWrapperFile(): a path to the root directory, " + path2RootDir_ + 
            ", has been created");
      };
    };
    //
    //
    if (have2adjustPermissions_)
    {
      QFile::Permissions        perm=QFile::permissions(path2RootDir_);
      if (!(perm & QFile::WriteGroup && perm & QFile::ExeGroup))
      {
        if (!QFile::setPermissions(path2RootDir_, perm | QFile::WriteGroup | QFile::ExeGroup))
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
            "::composeWrapperFile(): cannot change permissions of the directory \"" + 
            path2RootDir_ + "\"");
      }
      else
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::composeWrapperFile(): permissions of the directory \"" + path2RootDir_ + "\" looks fine");
    };
    //
    //
    QFile                       f(path2RootDir_ + "/" + outputFileName);
    if (f.open(QFile::WriteOnly))
    {
      QTextStream               s(&f);
      composeVersionBlock(s);
      s << "!\n";
      composeHistoryBlock(s);
      s << "!\n";
      composeSessionBlock(s);
      s << "!\n";
      for (QMap<QString, StationDescriptor>::iterator it=stnDescriptorByKey_.begin();
                                                                    it!=stnDescriptorByKey_.end(); ++it)
      {
        composeStationBlock(s, it.value());
        s << "!\n";
      };    
      composeScanBlock(s);
      s << "!\n";
      composeObservationBlock(s);
      s << "!\n";
      if (progSolveDescriptor_.hasSomething4output())
        composeProgramSolveBlock(s);
      for (QMap<QString, ProgramGenericDescriptor>::iterator it=progDescriptorByName_.begin(); 
                                                                  it!=progDescriptorByName_.end(); ++it)
      {
        composeProgramGenericBlock(s, it.value());
        s << "!\n";
      };
      //
      //
      f.close();
      s.setDevice(NULL);
      if (have2adjustPermissions_)
      {
        QFile::Permissions      perm=f.permissions();
        if (!f.setPermissions(perm | QFile::WriteGroup))
          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
            "::composeWrapperFile(): cannot change permissions of the file \"" + f.fileName() + "\"");
      };
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::composeWrapperFile(): the wrapper file \"" + f.fileName() + "\" has been composed");
      return true;
    }
    else 
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::composeWrapperFile(): cannot open with write access the wrapper file \"" +
        f.fileName() + "\"");
    return false;
  }
  else // dry run mode:
  {
    std::cout << "\nDRY RUN: The wrapper file will be created: \"" 
              << qPrintable(path2RootDir_ + "/" + outputFileName) << "\"\n";
    return true;
  };
};



//
void SgVgosDb::createWrapperFileName(QString &newName)
{
  QRegExp                       rx;
  int                           ver=currentVersion_;
  QString                       sPrefix(""), sVersion(""), sSuffix(""), sExtension("");
  QString                       sKind(""), sOrganization(""), str("");
  rx.setMinimal(true);

  newName = wrapperFileName_.size() ? wrapperFileName_ : sessionName_;


  // pick up the prefix:
  // new session:
  if (!newName.contains('_'))
  {
    sPrefix = newName;
    sKind = "all";      // add the kind too
  };
  // already existed session:
  rx.setPattern("^([-a-zA-Z0-9]+)_(\\S+)$");
  if (newName.contains(rx))
  {
    sPrefix = rx.cap(1);
  };
  // pick up the version:
  rx.setPattern("^(\\S+)_V(\\d+)([_\\.]+)(\\S+)$");
  if (newName.contains(rx))
  {
    sVersion = rx.cap(2);
    ver = sVersion.toInt();
  };
  // pick up the kind:
  rx.setPattern("^(\\S+)_k([A-Za-z0-9]+)([_\\.]+)(\\S+)$");
  if (newName.contains(rx))
  {
    sKind = rx.cap(2);
  };
  // pick up the institution:
  rx.setPattern("^(\\S+)_i([A-Za-z0-9]+)([_\\.]+)(\\S+)$");
  rx.setMinimal(true);
  if (newName.contains(rx))
  {
    str = rx.cap(2);
  };
  sOrganization = currentIdentities_->getAcAbbName(); // three chars
  if (str.size() && str != sOrganization) // just notify a user:
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::createWrapperFileName(): the organization name has been adjusted: " + str + 
      " => " + sOrganization);
  // pick up the extension:
  rx.setPattern("^(\\S+)\\.(\\S+)$");
  if (newName.contains(rx))
  {
    sExtension = rx.cap(2);
  };

  if (sExtension.size()==0)
    sExtension = "wrp";

  sSuffix.sprintf("%s%s.%s",
    qPrintable(sOrganization.size()?("_i" + sOrganization):""), 
    qPrintable(sKind.size()?("_k" + sKind):""), 
    qPrintable(sExtension));

  newName.sprintf("%s_V%03d%s", 
    qPrintable(sPrefix), ++ver, qPrintable(sSuffix));

  while (QFile(path2RootDir_ + "/" + newName).exists() && ver<1000)
  {
    newName.sprintf("%s_V%03d%s", 
      qPrintable(sPrefix), ++ver, qPrintable(sSuffix));
  };
  //
  if (ver>=1000)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::createWrapperFileName(): the version number is too big");
  //
  //
  sSuffix = currentDriverVersion_->getSoftwareName();
  localHistory_.historyFileName_.sprintf("%s_V%03d_k%s.hist", 
    qPrintable(sPrefix), ver, qPrintable(sSuffix));
  
  if (QFile(path2RootDir_ + "/" + localHistory_.defaultDir_ + "/" + 
    localHistory_.historyFileName_).exists())
  {
    sSuffix = "_i" + sOrganization + "_k" + currentDriverVersion_->getSoftwareName();
    localHistory_.historyFileName_.sprintf("%s_V%03d%s.hist", 
      qPrintable(sPrefix), ver, qPrintable(sSuffix));
  };
};



//
void SgVgosDb::composeVersionBlock(QTextStream& s)
{
  s << "VERSION " << outputFormatId_
    << "\n!\n!\n";
};



//
void SgVgosDb::composeHistoryBlock(QTextStream& s)
{
  //localHistory_.softwareName_ = currentIdentities_->getExecBinaryName();
  //
  localHistory_.processName_ = libraryVersion.getSoftwareName() + "/" + 
    currentDriverVersion_->getSoftwareName();
  localHistory_.epochOfCreation_ = SgMJD::currentMJD().toUtc();
  localHistory_.creator_ = currentIdentities_->getUserName() + ", " + 
    currentIdentities_->getAcFullName();
  localHistory_.version_ = libraryVersion.toString() + "/" + currentDriverVersion_->toString();
  if (wrapperFileName_.size())
    localHistory_.inputWrapperFileName_ = wrapperFileName_;

  QString                       str("");
  s << "!\n";
  s << "Begin History\n";
  // write previous histories:
//QMap<QString, HistoryDescriptor>::iterator  ith;
//for (ith=historyDescriptorByName_.begin(); ith!=historyDescriptorByName_.end(); ++ith)
  for (int i=0; i<historyDescriptors_.size(); i++)
  {
    const HistoryDescriptor&      hd=historyDescriptors_.at(i);
    s << "!\nBegin Process "<< hd.processName_ << "\n";
    s << "Version "         << hd.version_ << "\n";
    s << "CreatedBy "       << hd.creator_ << "\n";
    s << "Default_dir "     << hd.defaultDir_ << "\n";
    str = hd.defaultDir_;
    s << "RunTimeTag "      << hd.epochOfCreation_.toString(SgMJD::F_Simple)
      << " UTC\n";
    s << "History "         << hd.historyFileName_ << "\n";
    if (hd.inputWrapperFileName_.size())
      s << "InputWrapper "  << hd.inputWrapperFileName_ << "\n";
    s << "End Process "     << hd.processName_ << "\n";
  };
  if (!str.isEmpty())
    localHistory_.defaultDir_ = str;
  // the local history:
  s << "!\nBegin Process "<< localHistory_.processName_ << "\n";
  s << "Version "         << localHistory_.version_ << "\n";
  s << "CreatedBy "       << localHistory_.creator_ << "\n";
  s << "Default_dir "     << localHistory_.defaultDir_ << "\n";
  s << "RunTimeTag "      << localHistory_.epochOfCreation_.toString(SgMJD::F_Simple)
    << " UTC\n";
  s << "History "         << localHistory_.historyFileName_ << "\n";
  if (localHistory_.inputWrapperFileName_.size())
    s << "InputWrapper "  << localHistory_.inputWrapperFileName_ << "\n";
  s << "End Process "     << localHistory_.processName_ << "\n";
  s << "!\nEnd History\n";
};



//
void SgVgosDb::writeNcFileName(QTextStream& s, QString& currentSubDir, const SgVdbVariable& var)
{
  if (var.isEmpty())
    return;
  QString                       subDir(""), baseName("");
  //splitNcFileName(fileName, subDir, baseName);
  if (currentSubDir != var.getSubDir())
    s << "!\nDefault_Dir " << (currentSubDir=var.getSubDir()) << "\n";
  s << var.getFileName4Output() << "\n";
};



//
void SgVgosDb::composeSessionBlock(QTextStream& s)
{
  QString                       str, subDir("");
  QMap<QString, BandData>::iterator
                                it;

  s << "!\n";
  s << "Begin Session\n";
  s << "Session " << sessionCode_ << "\n";
  s << "AltSessionId " << sessionName_ << "\n";
  s << "!\n";
  writeNcFileName(s, subDir, vHead_);
  //
  //
  writeNcFileName(s, subDir, vAntennaApriori_);
  writeNcFileName(s, subDir, vStationApriori_);
  writeNcFileName(s, subDir, vSourceApriori_);
  writeNcFileName(s, subDir, vClockApriori_);
  writeNcFileName(s, subDir, vEccentricity_);
  //
  writeNcFileName(s, subDir, vStationCrossRef_);
  writeNcFileName(s, subDir, vSourceCrossRef_);
  writeNcFileName(s, subDir, vGroupBLWeights_);
  writeNcFileName(s, subDir, vClockBreak_);
  writeNcFileName(s, subDir, vLeapSecond_);
  writeNcFileName(s, subDir, vMiscCable_);
  writeNcFileName(s, subDir, vMiscFourFit_);
  for (it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
  {
    const BandData&             bd=it.value();
    writeNcFileName(s, subDir, bd.vMiscFourFit_);
    //...
  };
  //
  s << "!\nEnd Session\n";
};



//
void SgVgosDb::composeStationBlock(QTextStream&s, const StationDescriptor& sd)
{
  QString                       str, subDir("");
  s << "Begin Station " << sd.stationKey_ << "\n";
  //
  writeNcFileName(s, subDir, sd.vTimeUTC_);
  writeNcFileName(s, subDir, sd.vRefClockOffset_);
  writeNcFileName(s, subDir, sd.vTsys_);
  writeNcFileName(s, subDir, sd.vMet_);
  writeNcFileName(s, subDir, sd.vAzEl_);
  writeNcFileName(s, subDir, sd.vFeedRotation_);
  writeNcFileName(s, subDir, sd.vCal_AxisOffset_);
  writeNcFileName(s, subDir, sd.vCal_Cable_);
  writeNcFileName(s, subDir, sd.vCal_CblCorrections_);
  writeNcFileName(s, subDir, sd.vCal_SlantPathTropDry_);
  writeNcFileName(s, subDir, sd.vCal_SlantPathTropWet_);
  writeNcFileName(s, subDir, sd.vCal_OceanLoad_);
  writeNcFileName(s, subDir, sd.vPart_AxisOffset_);
  writeNcFileName(s, subDir, sd.vPart_ZenithPathTropDry_);
  writeNcFileName(s, subDir, sd.vPart_ZenithPathTropWet_);
  writeNcFileName(s, subDir, sd.vPart_HorizonGrad_);
  writeNcFileName(s, subDir, sd.vDis_OceanLoad_);
  s << "End Station " << sd.stationKey_ << "\n";
};



//
void SgVgosDb::composeScanBlock(QTextStream& s)
{
  QString                       str, subDir("");
  s << "!\nBegin Scan\n";
  //
  writeNcFileName(s, subDir, vScanTimeUTC_);
  writeNcFileName(s, subDir, vCorrRootFile_);
  writeNcFileName(s, subDir, vErpApriori_);
  writeNcFileName(s, subDir, vScanName_);
  writeNcFileName(s, subDir, vNutationEqx_kWahr_);
  writeNcFileName(s, subDir, vNutationEqx_);
  writeNcFileName(s, subDir, vNutationNro_);
  writeNcFileName(s, subDir, vEphemeris_);
  writeNcFileName(s, subDir, vRot_CF2J2K_);
  //
  s << "!\nEnd Scan\n";
};



//
void SgVgosDb::composeObservationBlock(QTextStream& s)
{
  QString                       str, subDir("");
  QMap<QString, BandData>::iterator
                                it;
  s << "!\nBegin Observation\n";
  //
  s << "!\n";
  writeNcFileName(s, subDir, vObservationTimeUTC_);
  writeNcFileName(s, subDir, vBaseline_);
  writeNcFileName(s, subDir, vSource_);
  //  writeNcFileName(s, subDir, vURVR_);
  // bands:
  for (it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
  {
    const BandData&             bd=it.value();
    writeNcFileName(s, subDir, bd.vChannelInfo_);
    writeNcFileName(s, subDir, bd.vPhaseCalInfo_);
    writeNcFileName(s, subDir, bd.vCorrInfo_);
    writeNcFileName(s, subDir, bd.vRefFreq_);
    writeNcFileName(s, subDir, bd.vAmbigSize_);
    writeNcFileName(s, subDir, bd.vQualityCode_);
    writeNcFileName(s, subDir, bd.vSBDelay_);
    writeNcFileName(s, subDir, bd.vGroupDelay_);
    writeNcFileName(s, subDir, bd.vGroupRate_);
    writeNcFileName(s, subDir, bd.vPhase_);
    writeNcFileName(s, subDir, bd.vDelayDataFlag_);
    writeNcFileName(s, subDir, bd.vDataFlag_);
    writeNcFileName(s, subDir, bd.vSNR_);
    writeNcFileName(s, subDir, bd.vCorrelation_);
    s << "!\n";
    writeNcFileName(s, subDir, bd.vCal_SlantPathIonoGroup_);
    writeNcFileName(s, subDir, bd.vEffFreq_);
    writeNcFileName(s, subDir, bd.vEffFreq_EqWt_);
    s << "!\n";
  };
  //
  writeNcFileName(s, subDir, vFeedRotNet_);
  //
  s << "!\n";
  writeNcFileName(s, subDir, vEdit_);
  for (it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
  {
    const BandData&             bd=it.value();
    writeNcFileName(s, subDir, bd.vNumGroupAmbig_);
    writeNcFileName(s, subDir, bd.vNumPhaseAmbig_);
    writeNcFileName(s, subDir, bd.vGroupDelayFull_);
    writeNcFileName(s, subDir, bd.vPhaseDelayFull_);
  };
  writeNcFileName(s, subDir, vNGSQualityFlag_);
  //
  s << "!\n";
  writeNcFileName(s, subDir, vObsCrossRef_);
  //
  s << "!\n";
  writeNcFileName(s, subDir, vDelayTheoretical_);
  writeNcFileName(s, subDir, vRateTheoretical_);
  //
  s << "!\n";
  writeNcFileName(s, subDir, vDiffTec_);
  //
  s << "!\n";
  writeNcFileName(s, subDir, vCal_BendSunHigher_);
  writeNcFileName(s, subDir, vCal_BendSun_);
  writeNcFileName(s, subDir, vCal_Bend_);
  writeNcFileName(s, subDir, vCal_EarthTide_);
  writeNcFileName(s, subDir, vCal_FeedCorrection_);
  writeNcFileName(s, subDir, vCal_HfErp_);
  writeNcFileName(s, subDir, vCal_OceanLoad_);
  writeNcFileName(s, subDir, vCal_Parallax_);
  writeNcFileName(s, subDir, vCal_PoleTideOldRestore_);
  writeNcFileName(s, subDir, vCal_PoleTide_);
  writeNcFileName(s, subDir, vCal_TiltRemover_);
  for (it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
  {
    writeNcFileName(s, subDir, it.value().vCal_Unphase_);
    writeNcFileName(s, subDir, it.value().vCal_FeedCorrection_);
    writeNcFileName(s, subDir, it.value().vUVFperAsec_);
  };
  writeNcFileName(s, subDir, vCal_Wobble_);
  writeNcFileName(s, subDir, vCal_HfLibration_);
  writeNcFileName(s, subDir, vCal_OceanLoadOld_);
  writeNcFileName(s, subDir, vCal_OceanPoleTideLoad_);
  writeNcFileName(s, subDir, vCal_HiFreqLibration_);
  s << "!\n";
  writeNcFileName(s, subDir, vPart_Bend_);
  writeNcFileName(s, subDir, vPart_Gamma_);
  writeNcFileName(s, subDir, vPart_Erp_);
  writeNcFileName(s, subDir, vPart_NutationEqx_);
  writeNcFileName(s, subDir, vPart_NutationNro_);
  writeNcFileName(s, subDir, vPart_Parallax_);
  writeNcFileName(s, subDir, vPart_PoleTide_);
  writeNcFileName(s, subDir, vPart_Precession_);
  writeNcFileName(s, subDir, vPart_RaDec_);
  writeNcFileName(s, subDir, vPart_Xyz_);
  //
  s << "!\nEnd Observation\n";
};



//
void SgVgosDb::composeProgramSolveBlock(QTextStream& s)
{
  // need to rework this part later:
  QString                       str, subDir(progSolveDescriptor_.vScanTimeMJD_.getSubDir());
  s << "!\n";
  s << "Begin Program " << progSolveDescriptor_.programName_ << "\n";
  s << "Default_dir "   << subDir << "\n";
  s << "!\n";
  s << "Begin Session\n";
  //  writeNcFileName(s, subDir, progSolveDescriptor_.vMisc_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vCalcInfo_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vCalibrationSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vClockSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vAtmSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vErpSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vIonoSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vCalcErp_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vBaselineClockSetup_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vSelectionStatus_);
  s << "End Session\n";
  s << "!\n";
  //
  s << "Begin Scan\n";
  writeNcFileName(s, subDir, progSolveDescriptor_.vScanTimeMJD_);
  s << "End Scan\n";
  s << "!\n";
  //
  s << "Begin Observation\n";
  writeNcFileName(s, subDir, progSolveDescriptor_.vUnPhaseCalFlag_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vIonoBits_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vFractC_);
  writeNcFileName(s, subDir, progSolveDescriptor_.vUserSup_);
  s << "End Observation\n";
  s << "!\n";
  s << "End Program " << progSolveDescriptor_.programName_ << "\n";
  s << "!\n";
};



//
void SgVgosDb::composeProgramGenericBlock(QTextStream& s, const ProgramGenericDescriptor& pgd)
{
  s << "!\n";
  s << "Begin Program " << pgd.programName_ << "\n";
  for (int i=0; i<pgd.content_.size(); i++)
    s << pgd.content_.at(i) << "\n";
  s << "End Program " << pgd.programName_ << "\n";
  s << "!\n";
};



//
bool SgVgosDb::saveLocalHistory(const SgVlbiHistory& history)
{
  QString                       str;
  if (!history.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::saveLocalHistory(): there is no history to save");
    return false;
  };
  str = path2RootDir_;
  if (localHistory_.defaultDir_.size())
    str += "/" + localHistory_.defaultDir_;

  if (operationMode_ == SgNetCdf::OM_REGULAR) // regular output:
  {
    QDir                          dir(str);
    if (!dir.exists())
    {
//    if (dir.mkpath(str))
      if (dir.mkpath(dir.absolutePath()))
      {
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::saveLocalHistory(): the directory \"" + str + "\" has been created");
        if (have2adjustPermissions_)
        {
          QFile::Permissions      perm=QFile::permissions(str);
          if (!QFile::setPermissions(str, perm | QFile::WriteGroup | QFile::ExeGroup))
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::saveLocalHistory(): cannot change permissions of the directory \"" + str + "\"");
        };
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::saveLocalHistory(): cannot create the directory \"" + str + "\"; saving history failed");
        return false;      
      };
    };
    str += "/" + localHistory_.historyFileName_; 
    if (QFile(str).exists())
    {
      QFile::remove(str);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::saveLocalHistory(): supposed to be a new history file \"" + str + 
        "] already exists; the file has been removed");
    };
    //
    QFile                         f(str);
    currentVersion_++;
    //
    const QString                 timeTag("TIMETAG"), versionTag("MK3DB_VERSION");
    const SgVlbiHistoryRecord    *rec=history.at(0);
    SgMJD                         t(rec->getEpoch());
    if (f.open(QFile::WriteOnly | QFile::Append))
    {
      QTextStream                 s(&f);
      // first label:
      s << "\n" << timeTag << " " << t.toString(SgMJD::F_Simple) << " UTC\n";
      s << versionTag << " " << str.setNum(currentVersion_) << "\n";
      for (int i=0; i<history.size(); i++)
      {
        rec = history.at(i);
        if (rec->getEpoch() != t)
        {
          t = rec->getEpoch();
          s << "\n" << timeTag << " " << t.toString(SgMJD::F_Simple) << " UTC\n";
        };
        s << rec->getText() << "\n";
      };
      f.close();
      s.setDevice(NULL);
      //
      // adjust permissions:
      QFile::Permissions          perm=f.permissions();
      if (!f.setPermissions(perm | QFile::WriteGroup))
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::saveLocalHistory(): cannot adjust the permissions of the file \"" + f.fileName() + "\"");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::saveLocalHistory(): cannot open the history file \"" + f.fileName() + "\" for write access");
      return false;
    };
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::saveLocalHistory(): saved " + str.setNum(history.size()) + 
      " historical records into the file \"" + f.fileName() + "\"");
    return true;
  }
  else // dry run mode:
  {
    std::cout << "DRY RUN: The file with local history will be created: \""
              << qPrintable(str + "/" + localHistory_.historyFileName_) << "\"\n";
    
    return true;
  };
};



//
bool SgVgosDb::saveForeignHistory(const SgVlbiHistory& history,
  const QString& softwareName,
  const SgMJD& epochOfCreation,
  const QString& creator,
  const QString& defaultDir,
  const QString& historyFileName,
  const QString& version,
  bool isMk3Compatible)
{
  QString                       str, key(softwareName);
  if (!history.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::saveForeignHistory(): there is no history to save");
    return false;
  };

  HistoryDescriptor             hd;
  hd.processName_ = softwareName;
  hd.epochOfCreation_ = epochOfCreation;
  hd.creator_ = creator;
  hd.defaultDir_ = defaultDir;
  hd.historyFileName_ = 
    historyFileName +
    QString("").sprintf("_V%03d", currentVersion_) +
    "_k" + corrTypeId_ + ".hist";

  hd.version_ = version;
  hd.isMk3Compatible_ = isMk3Compatible;
  historyDescriptors_ << hd;
  //
  str = path2RootDir_;
  if (hd.defaultDir_.size())
    str += "/" + hd.defaultDir_;

  if (operationMode_ == SgNetCdf::OM_REGULAR) // regular output:
  {
    QDir                          dir(str);
    if (!dir.exists())
    {
//    if (dir.mkpath(str))
      if (dir.mkpath(dir.absolutePath()))
      {
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::saveForeignHistory(): the directory \"" + str + "\" has been created");
        if (have2adjustPermissions_)
        {
          QFile::Permissions      perm=QFile::permissions(str);
          if (!QFile::setPermissions(str, perm | QFile::WriteGroup | QFile::ExeGroup))
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::saveForeignHistory(): cannot change permissions of the directory \"" + str + "\"");
        };
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::saveForeignHistory(): cannot create the directory \"" + str + "\"; saving history failed");
        return false;      
      };
    };
    str += "/" + hd.historyFileName_; 
    if (QFile(str).exists())
    {
      QFile::remove(str);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::saveForeignHistory(): supposed to be a new history file \"" + str + 
        "\" already exists; the file has been removed");
    };
    //
    QFile                         f(str);
    //
    const QString                 timeTag("TIMETAG"), versionTag("MK3DB_VERSION");
    const SgVlbiHistoryRecord    *rec=history.at(0);
    SgMJD                         t(rec->getEpoch());
    int                           v(rec->getVersion());
    if (f.open(QFile::WriteOnly | QFile::Append))
    {
      QTextStream                 s(&f);
      // first label:
      s << "\n" << timeTag << " " << t.toString(SgMJD::F_Simple) << " UTC\n";
      s << versionTag << " " << str.setNum(v) << "\n";
      for (int i=0; i<history.size(); i++)
      {
        rec = history.at(i);
        if (rec->getEpoch() != t)
        {
          t = rec->getEpoch();
          s << "\n" << timeTag << " " << t.toString(SgMJD::F_Simple) << " UTC\n";
          if (rec->getVersion() != v)
          {
            v = rec->getVersion();
            s << versionTag << " " << str.setNum(v) << "\n";
          };
        };
        s << rec->getText() << "\n";
      };
      f.close();
      s.setDevice(NULL);
      // adjust permissions:
      QFile::Permissions          perm=f.permissions();
      if (!f.setPermissions(perm | QFile::WriteGroup))
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::saveForeignHistory(): cannot adjust the permissions of the file \"" + f.fileName() + "\"");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::saveForeignHistory(): cannot open the history file \"" + f.fileName() + "\" for write access");
      return false;
    };
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::saveForeignHistory(): saved " + str.setNum(history.size()) + 
      " historical records into the file \"" + f.fileName() + "\"");
    return true;
  }
  else // dry run mode:
  {
    std::cout << "DRY RUN: The file with foreign history will be created: \""
              << qPrintable(str + "/" + hd.historyFileName_) << "\"\n";
    return true;
  };
};
/*=====================================================================================================*/




/*=====================================================================================================*/
//
// Constants:
//





 



/*=====================================================================================================*/
