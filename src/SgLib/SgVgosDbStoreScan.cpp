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


#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>



/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- storeScan part of vgosDb data tree)                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
bool SgVgosDb::storeEpochs4Scans(const QList<SgMJD>& epochs)
{
  SgVdbVariable                &var=vScanTimeUTC_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != epochs.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Scans(): size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, epochs.size()));
    return false;
  };
  fcSecond.alternateDimension(0, SD_NumScans);
  fcYmdhm .alternateDimension(0, SD_NumScans);
  fcSecond.setHave2hideLCode(true);
  fcYmdhm .setHave2hideLCode(true);
  //
  if (!setupFormat(fcfTimeUTC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Scans(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator files", "", "");
  fcSecond.setHave2hideLCode(false);
  fcYmdhm .setHave2hideLCode(false);
  //
  //
  double                        dSec;
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                       *pS=ncdf.lookupVar(fcSecond.name())->data2double();
  short                        *pD=ncdf.lookupVar(fcYmdhm .name())->data2short();
//  short                        *pU=ncdf.lookupVar(fcUtc4  .name())->data2short();
  for (int i=0; i<numOfScans_; i++)
  {
    epochs.at(i).toYMDHMS_tr(nYear, nMonth, nDay, nHour, nMin, dSec);
    pD[5*i + 0] = nYear;
    pD[5*i + 1] = nMonth;
    pD[5*i + 2] = nDay;
    pD[5*i + 3] = nHour;
    pD[5*i + 4] = nMin;
    pS[i] = dSec;
/*
    pU[6*i + 0] = nYear;
    pU[6*i + 1] = nMonth;
    pU[6*i + 2] = nDay;
    pU[6*i + 3] = nHour;
    pU[6*i + 4] = nMin;
    pU[6*i + 5] = round(dSec);
*/
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Scans(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeEpochs4Scans(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanTimeMjd(const QList<SgMJD>& epochs)
{
  SgVdbVariable                &var=progSolveDescriptor_.vScanTimeMJD_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != epochs.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTimeMjd(): size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, epochs.size()));
    return false;
  };
  fcSecond.alternateDimension(0, SD_NumScans);
  fcYmdhm .alternateDimension(0, SD_NumScans);
  fcSecond.setHave2hideLCode(true);
  fcYmdhm .setHave2hideLCode(true);
  //
  if (!setupFormat(fcfScanTimeMjd, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTimeMjd(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator files", "", "");
  fcSecond.setHave2hideLCode(false);
  fcYmdhm .setHave2hideLCode(false);
  //
  //
  int                          *pD=ncdf.lookupVar(fcMjd     .name())->data2int();
  double                       *pF=ncdf.lookupVar(fcDayFrac .name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    pD[i] = epochs.at(i).getDate();
    pF[i] = epochs.at(i).getTime();
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTimeMjd(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanTimeMjd(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanName(const QList<QString>& scanNames, const QList<QString>& scanFullNames)
{
  SgVdbVariable                &var=vScanName_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != scanNames.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanName(): scanName size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, scanNames.size()));
    return false;
  };
  if (numOfScans_ != scanFullNames.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanName(): scanFullName size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, scanFullNames.size()));
    return false;
  };
  fcScanName    .alternateDimension(0, SD_NumScans);
  fcScanNameFull.alternateDimension(0, SD_NumScans);
  if (!setupFormat(fcfScanName, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanName(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator files", "Scan", "TimeUTC.nc");
  //
  //
  int                           lN, lF;
  char                         *pN=ncdf.lookupVar(fcScanName    .name())->data2char();
  char                         *pF=ncdf.lookupVar(fcScanNameFull.name())->data2char();
  lN = fcScanName     .dims().at(1);
  lF = fcScanNameFull .dims().at(1);
  for (int i=0; i<numOfScans_; i++)
  {
    strncpy(pN + lN*i, qPrintable(scanNames     .at(i)), lN);
    strncpy(pF + lF*i, qPrintable(scanFullNames .at(i)), lF);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanName(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanName(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanCrootFname(const QList<QString>& corrRootFnames)
{
  SgVdbVariable                &var=vCorrRootFile_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != corrRootFnames.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanCrootFname(): corrRootFnames size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, corrRootFnames.size()));
    return false;
  };
  if (!setupFormat(fcfCorrRootFile, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanCrootFname(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are extracted from correlator files", "Scan", "TimeUTC.nc");
  //
  //
  int                           lN;
  char                         *pN=ncdf.lookupVar(fcCROOTFIL.name())->data2char();
  lN = fcCROOTFIL.dims().at(1);
  for (int i=0; i<numOfScans_; i++)
    strncpy(pN + lN*i, qPrintable(corrRootFnames.at(i)), lN);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanCrootFname(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanCrootFname(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanEphemeris(const SgMatrix* rSun, const SgMatrix* rMoon, const SgMatrix* rEarth,
  const SgMatrix* vSun, const SgMatrix* vMoon, const SgMatrix* vEarth, const SgMatrix* aEarth,
  const QString& kind)
{
  SgVdbVariable                &var=vEphemeris_;
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)rSun->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanEphemeris(): r_sun's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, rSun->nRow()));
    return false;
  };
  if (!setupFormat(fcfEphemeris, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanEphemeris(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *pS=ncdf.lookupVar(fcSunXyz.name())->data2double();
  double                       *pM=ncdf.lookupVar(fcMoonXyz.name())->data2double();
  double                       *pE=ncdf.lookupVar(fcEarthXyz.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    pS[6*i    ] = rSun->getElement(i, 0);
    pS[6*i + 1] = rSun->getElement(i, 1);
    pS[6*i + 2] = rSun->getElement(i, 2);
    pS[6*i + 3] = vSun->getElement(i, 0);
    pS[6*i + 4] = vSun->getElement(i, 1);
    pS[6*i + 5] = vSun->getElement(i, 2);

    pM[6*i    ] = rMoon->getElement(i, 0);
    pM[6*i + 1] = rMoon->getElement(i, 1);
    pM[6*i + 2] = rMoon->getElement(i, 2);
    pM[6*i + 3] = vMoon->getElement(i, 0);
    pM[6*i + 4] = vMoon->getElement(i, 1);
    pM[6*i + 5] = vMoon->getElement(i, 2);

    pE[9*i    ] = rEarth->getElement(i, 0);
    pE[9*i + 1] = rEarth->getElement(i, 1);
    pE[9*i + 2] = rEarth->getElement(i, 2);
    pE[9*i + 3] = vEarth->getElement(i, 0);
    pE[9*i + 4] = vEarth->getElement(i, 1);
    pE[9*i + 5] = vEarth->getElement(i, 2);
    pE[9*i + 6] = aEarth->getElement(i, 0);
    pE[9*i + 7] = aEarth->getElement(i, 1);
    pE[9*i + 8] = aEarth->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanEphemeris(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanEphemeris(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanNutationEqxWahr(const SgMatrix* pPsiEps, const SgMatrix* pPsiEps_rates)
{
  SgVdbVariable                &var=vNutationEqx_kWahr_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)pPsiEps->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqxWahr(): val's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pPsiEps->nRow()));
    return false;
  };
  if (numOfScans_ != (int)pPsiEps_rates->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqxWahr(): rate's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pPsiEps_rates->nRow()));
    return false;
  };
  if (!setupFormat(fcfNutationEqxWahr, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqxWahr(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *p=ncdf.lookupVar(fcNutationEqxWahr.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    p[4*i    ] = pPsiEps->getElement(i, 0);
    p[4*i + 1] = pPsiEps->getElement(i, 1);
    p[4*i + 2] = pPsiEps_rates->getElement(i, 0);
    p[4*i + 3] = pPsiEps_rates->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqxWahr(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqxWahr(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanNutationEqx(const SgMatrix* pPsiEps, const SgMatrix* pPsiEps_rates,
  const QString& kind)
{
  SgVdbVariable                &var=vNutationEqx_;
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)pPsiEps->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqx(): val's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pPsiEps->nRow()));
    return false;
  };
  if (numOfScans_ != (int)pPsiEps_rates->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqx(): rate's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pPsiEps_rates->nRow()));
    return false;
  };
  if (!setupFormat(fcfNutationEqx, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqx(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *p=ncdf.lookupVar(fcNutationEqx.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    p[4*i    ] = pPsiEps->getElement(i, 0);
    p[4*i + 1] = pPsiEps->getElement(i, 1);
    p[4*i + 2] = pPsiEps_rates->getElement(i, 0);
    p[4*i + 3] = pPsiEps_rates->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqx(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationEqx(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanNutationNro(const SgMatrix* pXys, const SgMatrix* pXys_rates,
  const QString& kind)
{
  SgVdbVariable                &var=vNutationNro_;
  var.setKind(kind); // empty is ok too
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)pXys->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationNro(): val's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pXys->nRow()));
    return false;
  };
  if (numOfScans_ != (int)pXys_rates->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationNro(): rate's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pXys_rates->nRow()));
    return false;
  };
  if (!setupFormat(fcfNutationNro, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationNro(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *p=ncdf.lookupVar(fcNutationNro.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    p[6*i    ] = pXys->getElement(i, 0);
    p[6*i + 1] = pXys->getElement(i, 1);
    p[6*i + 2] = pXys->getElement(i, 2);
    p[6*i + 3] = pXys_rates->getElement(i, 0);
    p[6*i + 4] = pXys_rates->getElement(i, 1);
    p[6*i + 5] = pXys_rates->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationNro(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanNutationNro(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeScanTrf2crf(const SgMatrix* val, const SgMatrix* rat, const SgMatrix* acc)
{
  SgVdbVariable                &var=vRot_CF2J2K_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)val->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): val's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, val->nRow()));
    return false;
  };
  if (numOfScans_ != (int)rat->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): rate's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, rat->nRow()));
    return false;
  };
  if (numOfScans_ != (int)acc->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): acc's matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, acc->nRow()));
    return false;
  };
  if (!setupFormat(fcfRotCf2J2K, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *p=ncdf.lookupVar(fcRotCf2J2K.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
    for (int j=0; j<9; j++)
    {
      p[27*i     + j ] = val->getElement(i, j);
      p[27*i + 9 + j ] = rat->getElement(i, j);
      p[27*i +18 + j ] = acc->getElement(i, j);
    };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeScanTrf2crf(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeErpApriori(const SgVector* ut1_tai, const SgMatrix* pm)
{
  SgVdbVariable                &var=vErpApriori_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, "", "");
  //
  if (numOfScans_ != (int)ut1_tai->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpApriori(): ut1_tai vector size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, ut1_tai->n()));
    return false;
  };
  if (numOfScans_ != (int)pm->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpApriori(): PM matrix size mismatch: " + 
      QString("").sprintf("%d vs %d", numOfScans_, pm->nRow()));
    return false;
  };
  if (!setupFormat(fcfErpApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpApriori(): format description failed");
    return false;
  };
  //
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "Scan", "TimeUTC.nc");
  //
  //
  double                       *pU=ncdf.lookupVar(fcUt1_Tai.name())->data2double();
  double                       *pP=ncdf.lookupVar(fcPolarMotion.name())->data2double();
  for (int i=0; i<numOfScans_; i++)
  {
    pU[i] = ut1_tai->getElement(i);
    pP[2*i    ] = pm->getElement(i, 0);
    pP[2*i + 1] = pm->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeErpApriori(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeErpApriori(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};




/*=====================================================================================================*/

