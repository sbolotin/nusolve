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
bool SgVgosDb::loadStationName(const QString& stnKey)
{
  if (!stnDescriptorByKey_.contains(stnKey))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationName(): unknown station " + stnKey);
    return false;
  };
  StationDescriptor            &dscr=stnDescriptorByKey_[stnKey];
  if (dscr.vTimeUTC_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationName(): there is no epochs for the station " + stnKey + 
      ", the vgosDb variable TimeUTC is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + dscr.vTimeUTC_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfTimeUTC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationName(): format check failed");
    return false;
  };
  //
  SgNcdfVariable               *svcV;
  if ( (svcV=ncdf.lookupServiceVar(SgNetCdf::svcStation)) )
  {
    const char                 *c=svcV->data2char();
    int                         l=svcV->dimensions().at(0)->getN();
    dscr.stationName_ = QString::fromLatin1(c, l);
    if (dscr.stationName_.size() < 8)
    {
      dscr.stationName_ = dscr.stationName_.leftJustified(8, ' '); // grrr..
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationName(): there is no service variable \"" + SgNetCdf::svcStation + 
      "\" for the station " + stnKey);
    return false;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationName(): found station name " + dscr.stationName_ + 
    " for the station key " + stnKey + " from the file " + ncdf.getFileName());
  return true;
};






//
bool SgVgosDb::loadStationEpochs(const QString& stnName, QList<SgMJD>& epochs)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationEpochs(): unknown station [" + stnName + "]");
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vTimeUTC_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationEpochs(): there is no epochs for the station " + stnName + 
      ", the vgosDb variable TimeUTC is empty");
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vTimeUTC_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfTimeUTC, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationEpochs(): format check failed");
    return false;
  };
  const double                 *pSeconds=ncdf2.lookupVar(fcSecond.name())->data2double();
  const short                  *pYMDHM  =ncdf2.lookupVar(fcYmdhm .name())->data2short();
  dscr->numOfPts_ = ncdf2.lookupVar(fcSecond.name())->dimensions().at(0)->getN();
  epochs.clear();
  for (int i=0; i<dscr->numOfPts_; i++)
    epochs.append( SgMJD(pYMDHM[5*i  ], pYMDHM[5*i+1], pYMDHM[5*i+2], pYMDHM[5*i+3], 
                                        pYMDHM[5*i+4], pSeconds[i]));
  if (epochs.size())
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadStationEpochs(): read " + QString("").setNum(epochs.size()) + 
      " epochs for the station " + stnName + " from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationAzEl(const QString& stnName, SgMatrix* &azTheo, SgMatrix* &elTheo)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationAzEl(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vAzEl_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationAzEl(): there is no vgosDb variable AzEl for the station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vAzEl_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfAzEl, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationAzEl(): format check failed");
    return false;
  };
  azTheo = new SgMatrix(dscr->numOfPts_, 2);
  elTheo = new SgMatrix(dscr->numOfPts_, 2);
  const double                 *pAz=ncdf2.lookupVar(fcAzTheo.name())->data2double();
  const double                 *pEl=ncdf2.lookupVar(fcElTheo.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    azTheo->setElement(i, 0,  pAz[2*i    ]);
    azTheo->setElement(i, 1,  pAz[2*i + 1]);
    elTheo->setElement(i, 0,  pEl[2*i    ]);
    elTheo->setElement(i, 1,  pEl[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationAzEl(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationParAngle(const QString& stnName, SgVector* &parAngle)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationParAngle(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vFeedRotation_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationParAngle(): there is no Vdb variable FeedRotation for the station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vFeedRotation_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfFeedRotation, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationParAngle(): format check failed");
    return false;
  };
  parAngle = new SgVector(dscr->numOfPts_);
  const double                 *pAngle=ncdf2.lookupVar(fcFeedRotation.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
    parAngle->setElement(i,  pAngle[i]);
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationParAngle(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationCalAxisOffset(const QString& stnName, SgMatrix* &cal)
{
  QString                       origin;
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalAxisOffset(): unknown station " + stnName);
    return false;
  };
  return loadStdObsCalibration(cal, origin, "Cal-AxisOffset",
    stnDescriptorByName_[stnName]->vCal_AxisOffset_, fcCalAxisOffset, fcfCalAxisOffset);
};




//
bool SgVgosDb::loadStationCalCable(const QString& stnName, SgMatrix* &cal, 
  QString& origin, QString& kind)
{
  bool                          isOk;
  origin = "";
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalCable(): unknown station " + stnName);
    return false;
  };
  isOk = loadStdObsCalibration(cal, origin, "Cal-Cable",
    stnDescriptorByName_[stnName]->vCal_Cable_, fcCalCable, fcfCalCable, false);
  kind = stnDescriptorByName_[stnName]->vCal_Cable_.getKind();
  return isOk;
};



//
bool SgVgosDb::loadStationCalCblCorrections(const QString& stnName, SgMatrix* &cal, 
  QString& origin, QString& kind)
{
  bool                          isOk=false;
  origin = "";
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalCblCorrections(): unknown station " + stnName);
    return isOk;
  };
  //
  if (!stnDescriptorByName_[stnName]->vCal_CblCorrections_.isEmpty())
  {
    isOk = loadStdObsCalibration(cal, origin, "Cal-CblCorrections",
      stnDescriptorByName_[stnName]->vCal_CblCorrections_, fcCalCblCorrections, 
      fcfCalCblCorrections, false);
    kind = stnDescriptorByName_[stnName]->vCal_CblCorrections_.getKind();
  }
  else if (!stnDescriptorByName_[stnName]->vCal_CableCorrections_.isEmpty())
  {
    isOk = loadStdObsCalibration(cal, origin, "Cal-CableCorrections",
      stnDescriptorByName_[stnName]->vCal_CableCorrections_, fcCalCableCorrections, 
      fcfCalCableCorrections, false);
    kind = stnDescriptorByName_[stnName]->vCal_CableCorrections_.getKind();
  };
  return isOk;
};



//
bool SgVgosDb::loadStationMet(const QString& stnName,
  SgVector* &metAtmPres, SgVector* &metAtmRH, SgVector* &metAtmTemp, QString& origin, QString& kind)
{
  origin = "";
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationMet(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vMet_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
      "::loadStationMet(): there is no vgosDb variable Met for the station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vMet_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfMet, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationMet(): format check failed");
    return false;
  };
  metAtmPres = new SgVector(dscr->numOfPts_);
  metAtmRH   = new SgVector(dscr->numOfPts_);
  metAtmTemp = new SgVector(dscr->numOfPts_);
  const double                 *pT=ncdf2.lookupVar(fcTempC  .name())->data2double();
  const double                 *pP=ncdf2.lookupVar(fcAtmPres.name())->data2double();
  const double                 *pH=ncdf2.lookupVar(fcRelHum .name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    metAtmTemp->setElement(i,   pT[i]);
    metAtmPres->setElement(i,   pP[i]);
    metAtmRH  ->setElement(i,   pH[i]);
  };
  //
  //
  SgNcdfVariable               *svcV;
  if ( (svcV=ncdf2.lookupServiceVar(SgNetCdf::svcDataOrigin)) )
  {
    const char                 *c=svcV->data2char();
    int                         l=svcV->dimensions().at(0)->getN();
    origin = QString::fromLatin1(c, l);
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadStationMet(): cannot find service variable \"" + SgNetCdf::svcStation + 
      "\" in " + ncdf2.getFileName());
  //
  kind = dscr->vMet_.getKind();
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationMet(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationCalSlantPathTropDry(const QString& stnName, SgMatrix* &cal, QString &kind)
{
  bool                          isOk;
  kind = "";
  QString                       origin;
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalSlantPathTropDry(): unknown station " + stnName);
    return false;
  };
  isOk = loadStdObsCalibration(cal, origin, "Cal-SlantPathTropDry",
    stnDescriptorByName_[stnName]->vCal_SlantPathTropDry_, fcCalSlantPathTropDry, fcfCalSlantPathTropDry,
    false);
  kind = stnDescriptorByName_[stnName]->vCal_SlantPathTropDry_.getKind();
  return isOk;
};



//
bool SgVgosDb::loadStationCalSlantPathTropWet(const QString& stnName, SgMatrix* &cal, QString &kind)
{
  bool                          isOk;
  kind = "";
  QString                       origin;
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalSlantPathTropWet(): unknown station " + stnName);
    return false;
  };
  isOk = loadStdObsCalibration(cal, origin, "Cal-SlantPathTropWet",
    stnDescriptorByName_[stnName]->vCal_SlantPathTropWet_, fcCalSlantPathTropWet, fcfCalSlantPathTropWet,
    false);
  kind = stnDescriptorByName_[stnName]->vCal_SlantPathTropWet_.getKind();
  return isOk;
};



//
bool SgVgosDb::loadStationCalOceanLoad(const QString& stnName,
  SgMatrix* &calHorzOLoad, SgMatrix* &calVertOLoad)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationCalOceanLoad(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vCal_OceanLoad_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
      "::loadStationCalOceanLoad(): there is no vgosDb variable Cal-StationOceanLoad for the station "
      + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vCal_OceanLoad_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfCalStationOceanLoad, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationCalOceanLoad(): format check failed");
    return false;
  };
  calVertOLoad = new SgMatrix(dscr->numOfPts_, 2);
  calHorzOLoad = new SgMatrix(dscr->numOfPts_, 2);
  const double                 *pV=ncdf2.lookupVar(fcCalStationOceanLoadVert .name())->data2double();
  const double                 *pH=ncdf2.lookupVar(fcCalStationOceanLoadHoriz.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    calVertOLoad->setElement(i, 0,  pV[2*i    ]);
    calVertOLoad->setElement(i, 1,  pV[2*i + 1]);
    calHorzOLoad->setElement(i, 0,  pH[2*i    ]);
    calHorzOLoad->setElement(i, 1,  pH[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationCalOceanLoad(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationPartAxisOffset(const QString& stnName, SgMatrix* &partAxisOffset)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartAxisOffset(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vPart_AxisOffset_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartAxisOffset(): there is no vgosDb variable Part-AxisOffset for the station " + 
      stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vPart_AxisOffset_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfPartAxisOffset, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationPartAxisOffset(): format check failed");
    return false;
  };
  const double                 *p=ncdf2.lookupVar(fcPartAxisOffset.name())->data2double();
  partAxisOffset = new SgMatrix(dscr->numOfPts_, 2);
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    partAxisOffset->setElement(i, 0,   p[2*i    ]);
    partAxisOffset->setElement(i, 1,   p[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationPartAxisOffset(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationOceanLdDisp(const QString& stnName, SgMatrix* &dis, SgMatrix* &vel)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationOceanLdDisp(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vDis_OceanLoad_.isEmpty())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::loadStationOceanLdDisp(): there is no vgosDb variable Dis-OceanLoad for the station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vDis_OceanLoad_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfDisOceanLoad, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationOceanLdDisp(): format check failed");
    return false;
  };
  dis = new SgMatrix(dscr->numOfPts_, 3);
  vel = new SgMatrix(dscr->numOfPts_, 3);
  const double                 *p=ncdf2.lookupVar(fcDisOceanLoad.name())->data2double();
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    dis->setElement(i, 0,  p[6*i    ]);
    dis->setElement(i, 1,  p[6*i + 1]);
    dis->setElement(i, 2,  p[6*i + 2]);
    vel->setElement(i, 0,  p[6*i + 3]);
    vel->setElement(i, 1,  p[6*i + 4]);
    vel->setElement(i, 2,  p[6*i + 5]);
   };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationOceanLdDisp(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationPartZenithDelayDry(const QString& stnName, SgMatrix* &part, QString &kind)
{
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartZenithDelayDry(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vPart_ZenithPathTropDry_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartZenithDelayDry(): there is no vgosDb variable Part-ZenithPathTropDry for the "
      "station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf2(path2RootDir_ + "/" + dscr->vPart_ZenithPathTropDry_.getFileName());
  ncdf2.getData();
  if (!checkFormat(fcfPartZenithPathTropDry, ncdf2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationPartZenithDelayDry(): format check failed");
    return false;
  };
  const double                 *p=ncdf2.lookupVar(fcPartZenithPathTropDry.name())->data2double();
  part = new SgMatrix(dscr->numOfPts_, 2);
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    part->setElement(i, 0,   p[2*i    ]);
    part->setElement(i, 1,   p[2*i + 1]);
  };
  kind = dscr->vPart_ZenithPathTropDry_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationPartZenithDelayDry(): data successfully loaded from " + ncdf2.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationPartZenithDelayWet(const QString& stnName, SgMatrix* &part, QString &kind)
{
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartZenithDelayWet(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vPart_ZenithPathTropWet_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartZenithDelayWet(): there is no vgosDb variable Part-ZenithPathTropWet for the "
      "station " + stnName);
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + dscr->vPart_ZenithPathTropWet_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPartZenithPathTropWet, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationPartZenithDelayWet(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartZenithPathTropWet.name())->data2double();
  part = new SgMatrix(dscr->numOfPts_, 2);
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    part->setElement(i, 0,   p[2*i    ]);
    part->setElement(i, 1,   p[2*i + 1]);
  };
  kind = dscr->vPart_ZenithPathTropWet_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationPartZenithDelayWet(): data successfully loaded from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationPartHorizonGrad(const QString& stnName, SgMatrix* &part, QString &kind)
{
  kind = "";
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartHorizonGrad(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vPart_HorizonGrad_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationPartHorizonGrad(): there is no vgosDb variable Part-HorizonGrad for the station " +
      stnName);
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + dscr->vPart_HorizonGrad_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfPartHorizonGrad, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationPartHorizonGrad(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcPartHorizonGrad.name())->data2double();
  part = new SgMatrix(dscr->numOfPts_, 4);
  for (int i=0; i<dscr->numOfPts_; i++)
  {
    part->setElement(i, 0,   p[4*i    ]);
    part->setElement(i, 1,   p[4*i + 1]);
    part->setElement(i, 2,   p[4*i + 2]);
    part->setElement(i, 3,   p[4*i + 3]);
  };
  kind = dscr->vPart_HorizonGrad_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationPartHorizonGrad(): data successfully loaded from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationRefClockOffset(const QString& stnName, SgVector* &refClockOffset)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationRefClockOffset(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vRefClockOffset_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
      "::loadStationRefClockOffset(): the vgosDb variable RefClockOffset for the station " +
      stnName + " is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + dscr->vRefClockOffset_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfRefClockOffset, ncdf))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::loadStationRefClockOffset(): format check failed");
    return false;
  };
  const double                 *d=ncdf.lookupVar(fcRefClockOffset.name())->data2double();
  refClockOffset = new SgVector(dscr->numOfPts_);
  for (int i=0; i<dscr->numOfPts_; i++)
    refClockOffset->setElement(i, d[i]);
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationRefClockOffset(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationTsys(const QString& stnName, SgMatrix* &tsyses, QVector<double>& frqs, 
  QVector<QString>& ids, QVector<QString>& sbs, QVector<QString>& polzs)
{
  if (!stnDescriptorByName_.contains(stnName))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::loadStationTsys(): unknown station " + stnName);
    return false;
  };
  StationDescriptor            *dscr=stnDescriptorByName_[stnName];
  if (dscr->vTsys_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() +
      "::loadStationTsys(): the vgosDb variable Tsys for the station " +
      stnName + " is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + dscr->vTsys_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfTsys, ncdf))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::loadStationTsys(): format check failed");
    return false;
  };
  const double                 *tsys=ncdf.lookupVar(fcTsysData.name())->data2double();
  const double                 *freqs=ncdf.lookupVar(fcTsysIfFreq.name())->data2double();
  const char                   *cIds=ncdf.lookupVar(fcTsysIfId.name())->data2char();
  const char                   *cSbs=ncdf.lookupVar(fcTsysIfSideBand.name())->data2char();
  const char                   *cPzs=ncdf.lookupVar(fcTsysIfPolarization.name())->data2char();
  int                           nChans=ncdf.lookupVar(fcTsysIfFreq.name())->dimensions().at(0)->getN();
  int                           lIds=ncdf.lookupVar(fcTsysIfId.name())->dimensions().at(1)->getN();
  int                           lSbs=ncdf.lookupVar(fcTsysIfSideBand.name())->dimensions().at(1)->getN();
  int                           lPzs=ncdf.lookupVar(fcTsysIfPolarization.name())->dimensions().at(1)->getN();

  tsyses = new SgMatrix(dscr->numOfPts_, nChans);
  frqs.resize(nChans);
  ids.resize(nChans);
  sbs.resize(nChans);
  polzs.resize(nChans);

  for (int i=0; i<dscr->numOfPts_; i++)
    for (int j=0; j<nChans; j++)
      tsyses->setElement(i, j,  tsys[nChans*i + j]);
  for (int j=0; j<nChans; j++)
  {
    frqs [j] = freqs[j];
    ids  [j] = QString::fromLatin1(cIds + lIds*j, lIds);
    sbs  [j] = QString::fromLatin1(cSbs + lSbs*j, lSbs);
    polzs[j] = QString::fromLatin1(cPzs + lPzs*j, lPzs);
  };
  //  
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadStationTsys(): data loaded successfully from " + ncdf.getFileName());
  return true;
};
/*=====================================================================================================*/










/*=====================================================================================================*/
//
// Constants:
//







// Variable descriptions:
SgVgosDb::FmtChkVar
  // FmtChkVar(const char* name, nc_type type, bool isMandatory, const QList<int> l, 
  //  const QString& attLCode="", const QString& attDefinition="", const QString& attUnits="", 
  //  const QString& attBand="") 
  fcAzTheo              ("AzTheo",          NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "AZ-THEO ", "Azimuth array definition", "radian, radian/sec"),
  fcElTheo              ("ElTheo",          NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "EL-THEO ", "Elevation array definition", "radian, radian/sec"),
  fcFeedRotation        ("FeedRotation",    NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "PARANGLE", "Feedhorn rot. angle", "radian"),
  fcCalAxisOffset       ("Cal-AxisOffset",  NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "AXO CONT", "New Axis Offset Contributions", "second"),
  fcCalCable            ("Cal-Cable",       NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "CABL DEL", "Cable calibration data", "second"),
  fcCalCblCorrections ("Cal-CblCorrections",  
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 3,
    "", "A set of cable calibration corrections: (FS Logs, CDMS, PCMT)", "second"),
  // tmp:
  fcCalCableCorrections ("Cal-CableCorrections",  
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 3,
    "", "A set of cable calibration corrections: (FS Logs, CDMS, PCMT)", "second"),
  //
  fcCalSlantPathTropDry ("Cal-SlantPathTropDry",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "NDRYCONT", "Nhmf (dry) atm. contribution", "second"),
  fcCalSlantPathTropWet ("Cal-SlantPathTropWet",        
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "NWETCONT", "Whmf (wet) atm. contribution", "second"),
  fcCalStationOceanLoadVert 
                        ("Cal-StationOceanLoadVert",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "OCE VERT", "Site-dep ocean cont - vertical", "second"),
  fcCalStationOceanLoadHoriz
                        ("Cal-StationOceanLoadHoriz",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "OCE HORZ", "Site-dep ocean cont - horizontal", "second"),
  fcCalUnPhaseCal       ("Cal-UnPhaseCal",  NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2  ),
  fcTempC               ("TempC",           NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "TEMP C", "Temp in C at local WX station", "Celsius"),
  fcAtmPres             ("AtmPres",         NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "ATM PRES", "Pressure in hPa at site", "hPa"),
  fcRelHum              ("RelHum",          NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "REL.HUM.", "Rel.Hum. at local WX st; [0...1], 0.5=50%)", "unitless"),
  fcPartAxisOffset      ("Part-AxisOffset", NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "AXO PART", "Axis Offset partial deriv. def."),
  fcDisOceanLoad        ("Dis-OceanLoad",   NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2 << 3,
    "OCE DELD", "Ocean load site dependent displace", "meter"),
  fcPartZenithPathTropDry
                        ("Part-ZenithPathTropDry",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "NDRYPART", "Nhmf2 dry partial deriv. def."),
  fcPartZenithPathTropWet
                        ("Part-ZenithPathTropWet",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2,
    "NWETPART", "Whmf2 wet partial deriv. def."),
  fcPartHorizonGrad     ("Part-HorizonGrad",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << 2 << 2,
    "NGRADPAR", "Niell dry atm. gradient partials"),
  fcRefClockOffset      ("ReferenceClockOffset",
                                            NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts,
    "", "Difference between reference station clock and UTC at the reference epoch", "sec"),

  fcTsysData            ("TsysData",        NC_DOUBLE,  true, QList<int>()  << SD_NumStnPts << SD_Any,
    "", "System temperature"),
  fcTsysIfFreq           ("TsysIfreq",       NC_DOUBLE,  true, QList<int>()  << SD_Any,
    "", "Frequencies of IFs for TSYS data"),
  fcTsysIfId            ("TsysIfId",        NC_CHAR  ,  true, QList<int>()  << SD_Any << SD_Any,
    "", "Identifiers of IFs for TSYS data"),

  fcTsysIfSideBand      ("TsysIfSideBand",  NC_CHAR  ,  true, QList<int>()  << SD_Any << SD_Any,
    "", "Sidebands of IFs for TSYS data"),

  fcTsysIfPolarization  ("TsysIfPolarization",
                                            NC_CHAR  ,  true, QList<int>()  << SD_Any << SD_Any,
    "", "Polarizations of IFs for TSYS data"),





  fcStub  ("",  NC_CHAR,  true, QList<int>() << 1, "", "")
  ;




// netCDF files:
QList<SgVgosDb::FmtChkVar*>
  fcfAzEl         = QList<SgVgosDb::FmtChkVar*>() << &fcAzTheo << &fcElTheo,
  fcfFeedRotation = QList<SgVgosDb::FmtChkVar*>() << &fcFeedRotation,
  fcfCalAxisOffset= QList<SgVgosDb::FmtChkVar*>() << &fcCalAxisOffset,
  fcfCalCable     = QList<SgVgosDb::FmtChkVar*>() << &fcCalCable,
  fcfCalCblCorrections     
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalCblCorrections,
  fcfCalCableCorrections     
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalCableCorrections,
  fcfCalSlantPathTropDry
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalSlantPathTropDry,
  fcfCalSlantPathTropWet
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalSlantPathTropWet,
  fcfCalStationOceanLoad
                  = QList<SgVgosDb::FmtChkVar*>() << &fcCalStationOceanLoadVert 
                                                  << &fcCalStationOceanLoadHoriz,
  fcfCalUnPhaseCal= QList<SgVgosDb::FmtChkVar*>() << &fcCalUnPhaseCal,
  fcfMet          = QList<SgVgosDb::FmtChkVar*>() << &fcTempC << &fcAtmPres << &fcRelHum,
  fcfPartAxisOffset         
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartAxisOffset,
  fcfDisOceanLoad = QList<SgVgosDb::FmtChkVar*>() << &fcDisOceanLoad,
  
  fcfPartZenithPathTropDry
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartZenithPathTropDry,
  fcfPartZenithPathTropWet
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartZenithPathTropWet,
  fcfPartHorizonGrad
                  = QList<SgVgosDb::FmtChkVar*>() << &fcPartHorizonGrad,
  fcfRefClockOffset
                  = QList<SgVgosDb::FmtChkVar*>() << &fcRefClockOffset,
  fcfTsys
                  = QList<SgVgosDb::FmtChkVar*>() << &fcTsysData << &fcTsysIfFreq << &fcTsysIfId
                  << &fcTsysIfSideBand << &fcTsysIfPolarization

  ;

/*=====================================================================================================*/

