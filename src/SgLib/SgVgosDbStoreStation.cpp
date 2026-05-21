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

#include <SgLogger.h>
#include <SgNetCdf.h>
#include <SgVgosDb.h>












/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- loadStation part of vgosDb data tree)                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
bool SgVgosDb::storeStationEpochs(const QString& stnName, const QList<SgMJD>& epochs)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationEpochs(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vTimeUTC_;
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != epochs.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationEpochs(): size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, epochs.size()));
    return false;
  };
  //
  fcSecond.alternateDimension(0, SD_NumStnPts);
  fcYmdhm .alternateDimension(0, SD_NumStnPts);
  fcSecond.setHave2hideLCode(true);
  fcYmdhm .setHave2hideLCode(true);
  if (!setupFormat(fcfTimeUTC, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationEpochs(): format description failed");
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
  for (int i=0; i<dscr->numOfPts_; i++)
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
      "::storeStationEpochs(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationEpochs(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalCable(const QString& stnName, const SgMatrix* cal,
  const QString& origin, const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalCable(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_Cable_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)cal->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCable(): cable cal size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, cal->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalCable, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCable(): format description failed for " + stnName);
    return false;
  };
  ncdf.setServiceVars(var.getStub(), origin, "StationScan", "TimeUTC.nc");
  //
  double                       *pC=ncdf.lookupVar(fcCalCable.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
    pC[i] = cal->getElement(i, 0);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCable(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCable(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalCblCorrections(const QString& stnName, const SgMatrix* cal,
  const QString& origin /*, const QString& kind*/)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalCblCorrections(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_CblCorrections_;
  //
  //  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)cal->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCblCorrections(): cable cal size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, cal->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalCblCorrections, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCblCorrections(): format description failed for " + stnName);
    return false;
  };
  ncdf.setServiceVars(var.getStub(), origin, "StationScan", "TimeUTC.nc");
  //
  double                       *pC=ncdf.lookupVar(fcCalCblCorrections.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    pC[3*i    ] = cal->getElement(i, 0);
    pC[3*i + 1] = cal->getElement(i, 1);
    pC[3*i + 2] = cal->getElement(i, 2);
  
  };
  //
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCblCorrections(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalCblCorrections(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationMet(const QString& stnName,
  SgVector*const metAtmPres, SgVector*const metAtmRH, SgVector*const metAtmTemp, 
  const QString& origin, const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationMet(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vMet_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)metAtmPres->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): atm.pressure size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, metAtmPres->n()));
    return false;
  };
  if (dscr->numOfPts_ != (int)metAtmRH->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): atm.rel.humidity size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, metAtmRH->n()));
    return false;
  };
  if (dscr->numOfPts_ != (int)metAtmTemp->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): atm.temperature size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, metAtmTemp->n()));
    return false;
  };
  //
  if (!setupFormat(fcfMet, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): format description failed for " + stnName);
    return false;
  };
  ncdf.setServiceVars(var.getStub(), origin, "StationScan", "TimeUTC.nc");
  //
  double                       *pT=ncdf.lookupVar(fcTempC  .name())->data2double();
  double                       *pP=ncdf.lookupVar(fcAtmPres.name())->data2double();
  double                       *pH=ncdf.lookupVar(fcRelHum .name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    pP[i] = metAtmPres->getElement(i);
    pH[i] = metAtmRH  ->getElement(i);
    pT[i] = metAtmTemp->getElement(i);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationMet(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationAzEl(const QString& stnName, SgMatrix*const azTheo, SgMatrix*const elTheo)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationAzEl(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vAzEl_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)azTheo->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationAzEl(): azimuth size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, azTheo->nRow()));
    return false;
  };
  if (dscr->numOfPts_ != (int)elTheo->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationAzEl(): elevation size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, elTheo->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfAzEl, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationAzEl(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *pAz=ncdf.lookupVar(fcAzTheo.name())->data2double();
  double                       *pEl=ncdf.lookupVar(fcElTheo.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    pAz[2*i    ] = azTheo->getElement(i, 0);
    pAz[2*i + 1] = azTheo->getElement(i, 1);
    pEl[2*i    ] = elTheo->getElement(i, 0);
    pEl[2*i + 1] = elTheo->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationAzEl(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationAzEl(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationPartAxisOffset(const QString& stnName, const SgMatrix* partAxisOffset)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationPartAxisOffset(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vPart_AxisOffset_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)partAxisOffset->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartAxisOffset(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, partAxisOffset->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPartAxisOffset, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartAxisOffset(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartAxisOffset.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = partAxisOffset->getElement(i, 0);
    p[2*i + 1] = partAxisOffset->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartAxisOffset(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationPartAxisOffset(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationParAngle(const QString& stnName, const SgVector* parAngle)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationParAngle(): unknown station [" + stnName + "]");
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vFeedRotation_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)parAngle->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationParAngle(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, parAngle->n()));
    return false;
  };
  //
  if (!setupFormat(fcfFeedRotation, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationParAngle(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcFeedRotation.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
    p[i] = parAngle->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationParAngle(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationParAngle(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalAxisOffset(const QString& stnName, const SgMatrix* cal)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalAxisOffset(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_AxisOffset_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)cal->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalAxisOffset(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, cal->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalAxisOffset, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalAxisOffset(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCalAxisOffset.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = cal->getElement(i, 0);
    p[2*i + 1] = cal->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalAxisOffset(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalAxisOffset(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalSlantPathTropDry(const QString& stnName, const SgMatrix* cal, 
  const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalSlantPathTropDry(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_SlantPathTropDry_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)cal->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropDry(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, cal->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalSlantPathTropDry, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropDry(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCalSlantPathTropDry.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = cal->getElement(i, 0);
    p[2*i + 1] = cal->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropDry(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropDry(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalSlantPathTropWet(const QString& stnName, const SgMatrix* cal, 
  const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalSlantPathTropWet(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_SlantPathTropWet_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)cal->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropWet(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, cal->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalSlantPathTropWet, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropWet(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcCalSlantPathTropWet.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = cal->getElement(i, 0);
    p[2*i + 1] = cal->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropWet(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalSlantPathTropWet(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationCalOceanLoad(const QString& stnName,
  const SgMatrix* calHorzOLoad, const SgMatrix* calVertOLoad)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationCalOceanLoad(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vCal_OceanLoad_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)calHorzOLoad->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalOceanLoad(): the matrix HorzOLoad size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, calHorzOLoad->nRow()));
    return false;
  };
  //
  if (dscr->numOfPts_ != (int)calVertOLoad->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalOceanLoad(): the matrix VertOLoad size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, calVertOLoad->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfCalStationOceanLoad, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalOceanLoad(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *pH=ncdf.lookupVar(fcCalStationOceanLoadHoriz.name())->data2double();
  double                       *pV=ncdf.lookupVar(fcCalStationOceanLoadVert .name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    pH[2*i    ] = calHorzOLoad->getElement(i, 0);
    pH[2*i + 1] = calHorzOLoad->getElement(i, 1);
    pV[2*i    ] = calVertOLoad->getElement(i, 0);
    pV[2*i + 1] = calVertOLoad->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationCalOceanLoad(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationCalOceanLoad(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationOceanLdDisp(const QString& stnName, const SgMatrix* dis, const SgMatrix* vel)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationOceanLdDisp(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vDis_OceanLoad_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)dis->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationOceanLdDisp(): the matrix dis size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, dis->nRow()));
    return false;
  };
  //
  if (dscr->numOfPts_ != (int)vel->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationOceanLdDisp(): the matrix vel size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, vel->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfDisOceanLoad, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationOceanLdDisp(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcDisOceanLoad.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[6*i    ] = dis->getElement(i, 0);
    p[6*i + 1] = dis->getElement(i, 1);
    p[6*i + 2] = dis->getElement(i, 2);
    p[6*i + 3] = vel->getElement(i, 0);
    p[6*i + 4] = vel->getElement(i, 1);
    p[6*i + 5] = vel->getElement(i, 2);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationOceanLdDisp(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationOceanLdDisp(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationPartZenithDelayDry(const QString& stnName, const SgMatrix* part,
  const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationPartZenithDelayDry(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vPart_ZenithPathTropDry_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)part->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayDry(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, part->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPartZenithPathTropDry, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayDry(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartZenithPathTropDry.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = part->getElement(i, 0);
    p[2*i + 1] = part->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayDry(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayDry(): the data successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationPartZenithDelayWet(const QString& stnName, const SgMatrix* part,
  const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationPartZenithDelayWet(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vPart_ZenithPathTropWet_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)part->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayWet(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, part->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPartZenithPathTropWet, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayWet(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartZenithPathTropWet.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[2*i    ] = part->getElement(i, 0);
    p[2*i + 1] = part->getElement(i, 1);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayWet(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationPartZenithDelayWet(): the data successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationPartHorizonGrad(const QString& stnName, const SgMatrix* part, 
  const QString& kind)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationPartHorizonGrad(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vPart_HorizonGrad_;
  //
  var.setKind(kind); // empty is ok too
  //
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)part->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartHorizonGrad(): the size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, part->nRow()));
    return false;
  };
  //
  if (!setupFormat(fcfPartHorizonGrad, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartHorizonGrad(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "Data are provided by CALC or similar software",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcPartHorizonGrad.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    p[4*i    ] = part->getElement(i, 0);
    p[4*i + 1] = part->getElement(i, 1);
    p[4*i + 2] = part->getElement(i, 2);
    p[4*i + 3] = part->getElement(i, 3);
  };
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationPartHorizonGrad(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationPartHorizonGrad(): the data successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationRefClockOffset(const QString& stnName, const SgVector* v)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationRefClockOffset(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vRefClockOffset_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)v->n())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationRefClockOffset(): vector v size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, v->n()));
    return false;
  };
  //
  if (!setupFormat(fcfRefClockOffset, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationRefClockOffset(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "KOMB correlator files",
    "StationScan", "TimeUTC.nc");
  //
  double                       *p=ncdf.lookupVar(fcRefClockOffset.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
    p[i] = v->getElement(i);
  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationRefClockOffset(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationRefClockOffset(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::storeStationTsys(const QString& stnName, const SgMatrix* tsyses,
  const QVector<double>& frqs, const QVector<QString>& ids, const QVector<QString>& sbs,
  const QVector<QString>& polzs)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::storeStationTsys(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  SgVdbVariable                &var=dscr->vTsys_;
  // !!!
//  var.setSubDir(stnName.simplified());
  //
  SgNetCdf                      ncdf(path2RootDir_ + "/" +
    var.name4export(path2RootDir_, operationMode_), currentIdentities_, outputFormatId_,
    sessionCode_, stnName, "");
  //
  if (dscr->numOfPts_ != (int)tsyses->nRow())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): matrix tsyses size mismatch: " + 
      QString("").sprintf("%d vs %d", dscr->numOfPts_, tsyses->nRow()));
    return false;
  };
  //
  //
  int                           numChans;
  numChans = tsyses->nCol();
  if (numChans != frqs.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): array of frqs size mismatch: " + 
      QString("").sprintf("%d vs %d", numChans, frqs.size()));
    return false;
  };
  if (numChans != ids.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): array of ids size mismatch: " + 
      QString("").sprintf("%d vs %d", numChans, ids.size()));
    return false;
  };

  int                           lIds, lSbs, lPzs;
  lIds = lSbs = lPzs = 0;
  for (int i=0; i<numChans; i++)
  {
    if (lIds < ids[i].size())
      lIds = ids[i].size();
    if (lSbs < sbs[i].size())
      lSbs = sbs[i].size();
    if (lPzs < polzs[i].size())
      lPzs = polzs[i].size();
  };
  
  fcTsysData.alternateDimension(1, numChans);
  fcTsysIfFreq.alternateDimension(0, numChans);
  fcTsysIfId.alternateDimension(0, numChans);
  fcTsysIfId.alternateDimension(1, lIds);
  fcTsysIfSideBand.alternateDimension(0, numChans);
  fcTsysIfSideBand.alternateDimension(1, lSbs);
  fcTsysIfPolarization.alternateDimension(0, numChans);
  fcTsysIfPolarization.alternateDimension(1, lPzs);

  if (!setupFormat(fcfTsys, ncdf, stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): format description failed");
    return false;
  };
  ncdf.setServiceVars(var.getStub(), "TSYS data",
    "StationScan", "TimeUTC.nc");
  //
  fcTsysData.alternateDimension(1, SD_Any);
  fcTsysIfFreq.alternateDimension(0, SD_Any);
  fcTsysIfId.alternateDimension(0, SD_Any);
  fcTsysIfId.alternateDimension(1, SD_Any);
  fcTsysIfSideBand.alternateDimension(0, SD_Any);
  fcTsysIfSideBand.alternateDimension(1, SD_Any);
  fcTsysIfPolarization.alternateDimension(0, SD_Any);
  fcTsysIfPolarization.alternateDimension(1, SD_Any);

  double                       *pTsys=ncdf.lookupVar(fcTsysData.name())->data2double();
  double                       *pFreqs=ncdf.lookupVar(fcTsysIfFreq.name())->data2double();
  char                         *pIds=ncdf.lookupVar(fcTsysIfId.name())->data2char();
  char                         *pSbs=ncdf.lookupVar(fcTsysIfSideBand.name())->data2char();
  char                         *pPzs=ncdf.lookupVar(fcTsysIfPolarization.name())->data2char();

  for (int i=0; i<dscr->numOfPts_; i++)
    for (int j=0; j<numChans; j++)
      pTsys[i*numChans + j] = tsyses->getElement(i, j);
  for (int j=0; j<numChans; j++)
  {
    pFreqs[j] = frqs[j];
    strncpy(pIds + lIds*j, qPrintable(ids[j]), lIds);
    strncpy(pSbs + lSbs*j, qPrintable(sbs[j]), lSbs);
    strncpy(pPzs + lPzs*j, qPrintable(polzs[j]), lPzs);
  };

  // write data:
  ncdf.setOperationMode(operationMode_);
  if (!ncdf.putData())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): cannot put data into " + ncdf.getFileName());
    return false;
  };
  if (operationMode_ == SgNetCdf::OM_REGULAR)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::storeStationTsys(): the data were successfully stored in " + ncdf.getFileName());
  return true;
};







/*=====================================================================================================*/

