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


#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgModelsInfo.h>
#include <SgNetCdf.h>
#include <SgVgosDb.h>
#include <SgVersion.h>
#include <SgVlbiHistory.h>
#include <SgVlbiStationInfo.h>










/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation (continue -- loadSession part of vgosDb data tree)                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
bool SgVgosDb::loadSourcesApriories(QVector<QString> &sourcesNames, SgMatrix* &cooRaDe, 
  QVector<QString> &sourcesApRefs)
{
  if (vSourceApriori_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadSourcesApriories(): the vgosDb variable SourceApriori is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vSourceApriori_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfSourceApriori, ncdf))
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadSourcesApriories(): format check failed");
    return false;
  };
  //
  //
  const char                 *pS=ncdf.lookupVar(fcSourceNameApriori.name())->data2char();
  const char                 *pR=ncdf.lookupVar(fcSourceReference.name())?
                                ncdf.lookupVar(fcSourceReference.name())->data2char():NULL;
  const double               *pD=ncdf.lookupVar(fcSource2000RaDec  .name())->data2double();
  int                         lN=ncdf.lookupVar(fcSourceNameApriori.name())->dimensions().at(1)->getN();
  int                         lR=pR?ncdf.lookupVar(fcSourceReference.name())->dimensions().at(1)->getN():
                                0;
  char                       *buff=NULL;
  numOfSrc_ = ncdf.lookupVar(fcSource2000RaDec  .name())->dimensions().at(0)->getN();
  sourcesNames.resize(numOfSrc_);
  sourcesApRefs.clear();
  if (pR)
  {
    sourcesApRefs.resize(numOfSrc_);
    buff = new char[lR + 1];
  };
  cooRaDe = new SgMatrix(numOfSrc_, 2);
  for (int i=0; i<numOfSrc_; i++)
  {
    sourcesNames[i] = QString::fromLatin1(pS + lN*i, lN);
    if (pR)
    {
      strncpy(buff, pR + lR*i, lR);
      sourcesApRefs[i] = QString::fromLatin1(buff);
    };
    cooRaDe->setElement(i, 0, pD[2*i    ]);
    cooRaDe->setElement(i, 1, pD[2*i + 1]);
  };
  if (buff)
    delete[] buff;
  if (numOfSrc_)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadSourcesApriories(): loaded a priori coordinates of " + QString("").setNum(numOfSrc_) + 
      " sources from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadClockApriories(QVector<QString> &stationNames, SgMatrix* &offsets_n_rates)
{
  if (vClockApriori_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadClockApriories(): the vgosDb variable ClockApriori is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vClockApriori_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfClockApriori, ncdf))
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadClockApriories(): format check failed");
    return false;
  };
  //
  //
  const char                 *pS=ncdf.lookupVar(fcClockAprioriSite.name())->data2char();
  const double               *pO=ncdf.lookupVar(fcClockAprioriOffset.name())->data2double();
  const double               *pR=ncdf.lookupVar(fcClockAprioriRate.name())->data2double();
  int                         nS=ncdf.lookupVar(fcClockAprioriSite.name())->dimensions().at(0)->getN();
  int                         lS=ncdf.lookupVar(fcClockAprioriSite.name())->dimensions().at(1)->getN();
  int                         nO=ncdf.lookupVar(fcClockAprioriOffset.name())->dimensions().at(0)->getN();
  int                         nR=ncdf.lookupVar(fcClockAprioriRate.name())->dimensions().at(0)->getN();
  if (nS != nO)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadClockApriories(): a number of a priori site names (" + QString("").setNum(nS) +
      ") and a number of a priori offsets ("  + QString("").setNum(nO) + ") mismatch");
    return false;
  };
  if (nS != nR)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadClockApriories(): a number of a priori site names (" + QString("").setNum(nS) +
      ") and a number of a priori rates ("  + QString("").setNum(nR) + ") mismatch");
    return false;
  };
  stationNames.resize(nS);
  offsets_n_rates = new SgMatrix(nS, 2);
  for (int i=0; i<nS; i++)
  {
    stationNames[i] = QString::fromLatin1(pS + lS*i, lS);
    offsets_n_rates->setElement(i, 0, pO[i]);
    offsets_n_rates->setElement(i, 1, pR[i]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadClockApriories(): loaded a priori parameters of " + QString("").setNum(nS) + 
    " clocks from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadStationsApriories(QVector<QString> &stationsNames, SgMatrix* &cooXYZ,
  QVector<QString> &tectonicPlateNames)
{
  if (vStationApriori_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadStationsApriories(): the vgosDb variable StationApriori is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vStationApriori_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfStationApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadStationsApriories(): format check failed");
    return false;
  };
  const double               *pD=ncdf.lookupVar(fcStationXYZ        .name())->data2double();
  const char                 *pS=ncdf.lookupVar(fcStationNameApriori.name())->data2char();
  int                         lS=ncdf.lookupVar(fcStationNameApriori.name())->dimensions().at(1)->getN();
  const char                 *pT=ncdf.lookupVar(fcStationPlateName  .name())->data2char();
  int                         lT=ncdf.lookupVar(fcStationPlateName  .name())->dimensions().at(1)->getN();
  numOfStn_ = ncdf.lookupVar(fcStationXYZ        .name())->dimensions().at(0)->getN();
  stationsNames.resize(numOfStn_);
  tectonicPlateNames.resize(numOfStn_);
  cooXYZ = new SgMatrix(numOfStn_, 3);
  for (int i=0; i<numOfStn_; i++)
  {
    stationsNames[i] = QString::fromLatin1(pS + lS*i, lS);
    tectonicPlateNames[i] = QString::fromLatin1(pT + lT*i, lT);
    cooXYZ->setElement(i, 0,    pD[3*i    ]);
    cooXYZ->setElement(i, 1,    pD[3*i + 1]);
    cooXYZ->setElement(i, 2,    pD[3*i + 2]);
  };
  if (numOfStn_)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadStationsApriories(): loaded a priori positions of " + QString("").setNum(numOfStn_) + 
      " stations from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadAntennaApriories(QVector<QString> &stationsNames, QVector<int> &axisTypes, 
  SgVector* &axisOffsets, SgMatrix* &axisTilts)
{
  if (vAntennaApriori_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadAntennaApriories(): the vgosDb variable AntennaApriori is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vAntennaApriori_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfAntennaApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadAntennaApriories(): format check failed");
    return false;
  };
  const short                  *pI=ncdf.lookupVar(fcAxisType   .name())->data2short();
  const char                   *pS=ncdf.lookupVar(fcAntennaName.name())->data2char();
  const double                 *pO=ncdf.lookupVar(fcAxisOffset .name())->data2double();
  const double                 *pT=ncdf.lookupVar(fcAxisTilt   .name())?
                                   ncdf.lookupVar(fcAxisTilt   .name())->data2double():NULL;
  int                           n =ncdf.lookupVar(fcAntennaName.name())->dimensions().at(0)->getN();
  int                           l =ncdf.lookupVar(fcAntennaName.name())->dimensions().at(1)->getN();
  stationsNames.resize(n);
  axisTypes.resize(n);
  axisOffsets = new SgVector(n);
  if (pT)
    axisTilts = new SgMatrix(n, 2);
  for (int i=0; i<n; i++)
  {
    stationsNames[i]  = QString::fromLatin1(pS + l*i, l);
    axisTypes[i]      = pI[i];
    axisOffsets->setElement(i, pO[i]);
    if (pT)
    {
      axisTilts->setElement(i, 0, pT[2*i    ]);
      axisTilts->setElement(i, 1, pT[2*i + 1]);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadAntennaApriories(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadEccentricities(QVector<QString> &stationsNames, QVector<QString> &eccTypes,
  QVector<QString> &eccNums, SgMatrix* &eccVals)
{
  if (vEccentricity_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadEccentricities(): the vgosDb variable Eccentricity is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vEccentricity_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfEccentricity, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadEccentricities(): format check failed");
    return false;
  };
  const char             *pType=ncdf.lookupVar(fcEccentricityType    .name())->data2char();
  const char             *pName=ncdf.lookupVar(fcEccentricityName    .name())->data2char();
  const char             *pMnmt=ncdf.lookupVar(fcEccentricityMonument.name())->data2char();
  const double           *pV=ncdf.lookupVar(fcEccentricityVector.name())->data2double();
  int                     n =ncdf.lookupVar(fcEccentricityType.name())->dimensions().at(0)->getN();
  int                     lT=ncdf.lookupVar(fcEccentricityType.name())->dimensions().at(1)->getN();
  int                     lN=ncdf.lookupVar(fcEccentricityName.name())->dimensions().at(1)->getN();
  int                     lM=ncdf.lookupVar(fcEccentricityMonument.name())->dimensions().at(1)->getN();
  stationsNames.resize(n);
  eccTypes.resize(n);
  eccNums.resize(n);
  eccVals = new SgMatrix(n, 3);
  for (int i=0; i<n; i++)
  {
    stationsNames[i]  = QString::fromLatin1(pName + lN*i, lN);
    eccTypes[i]       = QString::fromLatin1(pType + lT*i, lT);
    eccNums[i]        = QString::fromLatin1(pMnmt + lM*i, lM);
    eccVals->setElement(i, 0, pV[3*i    ]);
    eccVals->setElement(i, 1, pV[3*i + 1]);
    eccVals->setElement(i, 2, pV[3*i + 2]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadEccentricities(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadAtmSetup(SgVector* &interval, SgVector* &rateConstraint, 
  QVector<QString> &stationsNames)
{
  SgVdbVariable                &var=progSolveDescriptor_.vAtmSetup_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadAtmSetup(): the vgosDb variable AtmSetup is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfAtmSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadAtmSetup(): format check failed");
    return false;
  };
  const double           *pT= ncdf.lookupVar(fcAtmInterval      .name())?
                              ncdf.lookupVar(fcAtmInterval      .name())->data2double():NULL;
  const double           *pC= ncdf.lookupVar(fcAtmRateConstraint.name())?
                              ncdf.lookupVar(fcAtmRateConstraint.name())->data2double():NULL;
  const char             *pS= ncdf.lookupVar(fcAtmRateSite      .name())?
                              ncdf.lookupVar(fcAtmRateSite      .name())->data2char():NULL;
  int                     nI=pT?
                              ncdf.lookupVar(fcAtmInterval      .name())->dimensions().at(0)->getN():0;
  int                     nC=pC?
                              ncdf.lookupVar(fcAtmRateConstraint.name())->dimensions().at(0)->getN():0;
  int                     l =pS?
                              ncdf.lookupVar(fcAtmRateSite      .name())->dimensions().at(1)->getN():0;
  if (nI)
  {
    interval = new SgVector(nI);
    for (int i=0; i<nI; i++)
      interval->setElement(i, pT[i]);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadAtmSetup(): the atm intervals have been found");
  };
  if (nC)
  {
    rateConstraint = new SgVector(nC);
    stationsNames.resize(nC);
    for (int i=0; i<nC; i++)
    {
      rateConstraint->setElement(i, pC[i]);
      if (pS)
        stationsNames[i] = QString::fromLatin1(pS + l*i, l);
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadAtmSetup(): the atm rate constraints have been found");
  };
  if (nI || nC)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadAtmSetup(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadClockSetup(QList<QString> &refClocks, SgVector* &interval, SgVector* &rateConstraint,
  QVector<QString> &stationsNames)
{
  SgVdbVariable                &var=progSolveDescriptor_.vClockSetup_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): the vgosDb variable ClockSetup is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfClockSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): format check failed");
    return false;
  };
  // special case:
  refClocks.clear();
//  SgNcdfVariable               *vRefs=ncdf.lookupVar("RefClockStationList");
  SgNcdfVariable               *vRefs=ncdf.lookupVar("ReferenceClock");
  if (vRefs)
  {
    if (vRefs->getTypeOfData() == NC_CHAR)
    {
      if (vRefs->dimensions().size() == 1)
        refClocks.append(QString::fromLatin1(vRefs->data2char(), vRefs->dimensions().at(0)->getN()));
      else
      {
        int                     n=vRefs->dimensions().at(0)->getN();
        int                     l=vRefs->dimensions().at(1)->getN();
        for (int i=0; i<n; i++)
          refClocks.append(QString::fromLatin1(vRefs->data2char() + l*i, l));
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::loadClockSetup(): wrong data type of \"RefClockStationList\", ignoring");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): no reference clock found");
  //
  // end of special case
  //
  const double           *pT=ncdf.lookupVar(fcClockInterval      .name())?
                             ncdf.lookupVar(fcClockInterval      .name())->data2double():NULL;
  const double           *pC=ncdf.lookupVar(fcClockRateConstraint.name())?
                             ncdf.lookupVar(fcClockRateConstraint.name())->data2double():NULL;
  const char             *pS=ncdf.lookupVar(fcClockRateName      .name())?
                             ncdf.lookupVar(fcClockRateName      .name())->data2char():NULL;
  int                     nI=pT?
                            ncdf.lookupVar(fcClockInterval.name())->dimensions().at(0)->getN():0;
  int                     nC=pC?
                            ncdf.lookupVar(fcClockRateConstraint.name())->dimensions().at(0)->getN():0;
  int                     l =pS?
                            ncdf.lookupVar(fcClockRateName.name())->dimensions().at(1)->getN():0;
  if (nI)
  {
    interval = new SgVector(nI);
    for (int i=0; i<nI; i++)
      interval->setElement(i, pT[i]);
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): the clock intervals have been found");
  };
  if (nC)
  {
    rateConstraint = new SgVector(nC);
    stationsNames.resize(nC);
    for (int i=0; i<nC; i++)
    {
      rateConstraint->setElement(i, pC[i]);
      if (pS)
        stationsNames[i] = QString::fromLatin1(pS + l*i, l);
    };
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): the clock rate constraints have been found");
  };
  if (nI || nC)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadClockSetup(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadCalcEop(SgVector* &pmArrayInfo, SgVector* &utArrayInfo, SgMatrix* &pmValues, 
  SgVector* &utValues, QString &sUtMode, QString &sUtModule, QString &sUtOrigin, QString &sPmMode, 
  QString &sPmModule, QString &sPmOrigin)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalcErp_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadCalcEop(): the vgosDb variable CalcEop is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalcEop, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadCalcEop(): format check failed");
    return false;
  };
  int                           n, l;
  const double                 *p;
  const char                   *c;
  p = ncdf.lookupVar(fcWobArrayInfo.name())->data2double();
  pmArrayInfo = new SgVector(3);
  pmArrayInfo->setElement(0, p[0]);
  pmArrayInfo->setElement(1, p[1]);
  pmArrayInfo->setElement(2, p[2]);
  //
  p = ncdf.lookupVar(fcUT1ArrayInfo.name())->data2double();
  utArrayInfo = new SgVector(4);
  utArrayInfo->setElement(0, p[0]);
  utArrayInfo->setElement(1, p[1]);
  utArrayInfo->setElement(2, p[2]);
  utArrayInfo->setElement(3, p[3]);
  //
  p = ncdf.lookupVar(fcUT1Values.name())->data2double();
  n = ncdf.lookupVar(fcUT1Values.name())->dimensions().at(0)->getN();
  utValues = new SgVector(n);
  for (int i=0; i<n; i++)
    utValues->setElement(i, p[i]);
  //
  p = ncdf.lookupVar(fcWobValues.name())->data2double();
  n = ncdf.lookupVar(fcWobValues.name())->dimensions().at(0)->getN();
  pmValues = new SgMatrix(n, 2);
  for (int i=0; i<n; i++)
  {
    pmValues->setElement(i, 0, p[2*i    ]);
    pmValues->setElement(i, 1, p[2*i + 1]);
  };
  //
  c = ncdf.lookupVar(fcUT1IntrpMode.name())->data2char();
  l = ncdf.lookupVar(fcUT1IntrpMode.name())->dimensions().at(0)->getN();
  sUtMode = QString::fromLatin1(c, l);
  //
  c = ncdf.lookupVar(fcCalcUt1Module.name())->data2char();
  l = ncdf.lookupVar(fcCalcUt1Module.name())->dimensions().at(0)->getN();
  sUtModule = QString::fromLatin1(c, l);
  //
  c = ncdf.lookupVar(fcUT1Origin.name())->data2char();
  l = ncdf.lookupVar(fcUT1Origin.name())->dimensions().at(0)->getN();
  sUtOrigin = QString::fromLatin1(c, l);
  //
  c = ncdf.lookupVar(fcWobIntrpMode.name())->data2char();
  l = ncdf.lookupVar(fcWobIntrpMode.name())->dimensions().at(0)->getN();
  sPmMode = QString::fromLatin1(c, l);
  //
  c = ncdf.lookupVar(fcCalcWobModule.name())->data2char();
  l = ncdf.lookupVar(fcCalcWobModule.name())->dimensions().at(0)->getN();
  sPmModule = QString::fromLatin1(c, l);
  //
  c = ncdf.lookupVar(fcWobbleOrigin.name())->data2char();
  l = ncdf.lookupVar(fcWobbleOrigin.name())->dimensions().at(0)->getN();
  sPmOrigin = QString::fromLatin1(c, l);
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadCalcEop(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadCalibrations(int &obsCalFlags, QVector<int> &statCalFlags, 
  QVector< QVector<int> > &flybyFlags, QVector<QString> &statCalNames, QVector<QString> &flybyNames,
  QVector<QString> &calSiteNames, QVector<QString> &obsCalNames)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalibrationSetup_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): the vgosDb variable CalibrationSetup is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalibrationSetup, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): format check failed");
    return false;
  };
  int                           n, l;
  const short                  *p;
  const char                   *c;
  //
  if (ncdf.lookupVar(fcObsCalFlag.name()))
    obsCalFlags = *ncdf.lookupVar(fcObsCalFlag.name())->data2short();
  else
  {
    obsCalFlags = 0;
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): cannot find " + fcObsCalFlag.name() + 
      " variable in the file " + ncdf.getFileName());
  };
  //
  p = ncdf.lookupVar(fcStatCalFlag.name())->data2short();
  n = ncdf.lookupVar(fcStatCalFlag.name())->dimensions().at(0)->getN();
  statCalFlags.resize(n);
  for (int i=0; i<n; i++)
    statCalFlags[i] = p[i];
  //
  if (ncdf.lookupVar(fcFlybyFlag.name()))
  {
    p = ncdf.lookupVar(fcFlybyFlag.name())->data2short();
    n = ncdf.lookupVar(fcFlybyFlag.name())->dimensions().at(0)->getN();
    l = ncdf.lookupVar(fcFlybyFlag.name())->dimensions().at(1)->getN();
    flybyFlags.resize(n);
    for (int iStn=0; iStn<n; iStn++)
    {
      flybyFlags[iStn].resize(l);
      for (int j=0; j<l; j++)
        (flybyFlags[iStn])[j] = p[l*iStn + j];
    };
  }
  else
  {
    flybyFlags.clear();
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): cannot find " + fcFlybyFlag.name() + 
      " variable in the file " + ncdf.getFileName());
  };
  //
  c = ncdf.lookupVar(fcStatCalName.name())->data2char();
  n = ncdf.lookupVar(fcStatCalName.name())->dimensions().at(0)->getN();
  l = ncdf.lookupVar(fcStatCalName.name())->dimensions().at(1)->getN();
  statCalNames.resize(n);
  for (int i=0; i<n; i++)
    statCalNames[i] = QString::fromLatin1(c + l*i, l);
  //
  if (ncdf.lookupVar(fcFlybyName.name()))
  {
    c = ncdf.lookupVar(fcFlybyName.name())->data2char();
    n = ncdf.lookupVar(fcFlybyName.name())->dimensions().at(0)->getN();
    l = ncdf.lookupVar(fcFlybyName.name())->dimensions().at(1)->getN();
    flybyNames.resize(n);
    for (int i=0; i<n; i++)
      flybyNames[i] = QString::fromLatin1(c + l*i, l);
  }
  else
  {
    flybyFlags.clear();
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): cannot find " + fcFlybyName.name() + 
      " variable in the file " + ncdf.getFileName());
  };
  //
  c = ncdf.lookupVar(fcCalStationName.name())->data2char();
  n = ncdf.lookupVar(fcCalStationName.name())->dimensions().at(0)->getN();
  l = ncdf.lookupVar(fcCalStationName.name())->dimensions().at(1)->getN();
  calSiteNames.resize(n);
  for (int i=0; i<n; i++)
    calSiteNames[i] = QString::fromLatin1(c + l*i, l);
  //
  if (ncdf.lookupVar(fcObsCalName.name()))
  {
    c = ncdf.lookupVar(fcObsCalName.name())->data2char();
    n = ncdf.lookupVar(fcObsCalName.name())->dimensions().at(0)->getN();
    l = ncdf.lookupVar(fcObsCalName.name())->dimensions().at(1)->getN();
    obsCalNames.resize(n);
    for (int i=0; i<n; i++)
      obsCalNames[i] = QString::fromLatin1(c + l*i, l);
  }
  else
  {
    obsCalNames.clear();
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadCalibrations(): cannot find " + fcObsCalName.name() + 
      " variable in the file " + ncdf.getFileName());
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadCalibrations(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadSelectionStatus(QVector<int> &sourcesSelection, 
  QVector< QVector<int> > &baselinesSelection)
{
  SgVdbVariable                &var=progSolveDescriptor_.vSelectionStatus_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadSelectionStatus(): the vgosDb variable SelectionStatus is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfSelectionStatus, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadSelectionStatus(): format check failed");
    return false;
  };
  const short            *pS=ncdf.lookupVar(fcSrcSelectionFlag.name())->data2short();
  const short            *pB=ncdf.lookupVar(fcBlnSelectionFlag.name())->data2short();
  int                     nS=ncdf.lookupVar(fcSrcSelectionFlag.name())->dimensions().at(0)->getN();
  int                     nB=ncdf.lookupVar(fcBlnSelectionFlag.name())->dimensions().at(0)->getN();
  sourcesSelection.resize(nS);
  for (int i=0; i<nS; i++)
    sourcesSelection[i] = pS[i];
  baselinesSelection.resize(nB);
  for (int i=0; i<nB; i++)
  {
    baselinesSelection[i].resize(nB);
    for (int j=0; j<nB; j++)
      baselinesSelection[i][j] = pB[nB*i + j];
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadSelectionStatus(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadSolveWeights(QVector<QString> &baselineNames, SgMatrix* &groupBlWeights)
{
  if (vGroupBLWeights_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadSolveWeights(): the vgosDb variable GroupBLWeights is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vGroupBLWeights_.getFileName());
  ncdf.getData();
  // 
  if (!checkFormat(fcfGroupBLWeights, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadSolveWeights(): format check failed");
    return false;
  };
  // plus additional check:
  SgNcdfVariable               *vW, *vN;
  vW = ncdf.lookupVar(fcGroupBLWeights   .name());
  vN = ncdf.lookupVar(fcGroupBLWeightName.name());
  if (vW->dimensions().at(1)->getN() != vN->dimensions().at(0)->getN())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadSolveWeights(): dimensions mismatch");
    return false;
  };
  const double           *pW=vW->data2double();
  const char             *pN=vN->data2char();
  int                     n =vW->dimensions().at(1)->getN();
  int                     l =vN->dimensions().at(2)->getN();
  baselineNames.resize(n);
  for (int i=0; i<n; i++)
    baselineNames[i] = 
      QString::fromLatin1(pN + 2*l*i, l) + ":" + QString::fromLatin1(pN + 2*l*i + l, l);
  groupBlWeights = new SgMatrix(2, n);
  for (int i=0; i<n; i++)
  {
    groupBlWeights->setElement(0, i,  pW[    i]);
    groupBlWeights->setElement(1, i,  pW[n + i]);
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadSolveWeights(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadClockBreaks(QVector<QString> &cbNames, QVector<SgMJD> &cbEpochs, 
  QVector<int> &cbFlags)
{
  if (vClockBreak_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadClockBreaks(): the vgosDb variable ClockBreak is empty, no clock breaks yet");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vClockBreak_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfClockBreak, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadClockBreaks(): format check failed");
    return false;
  };
  // need checking for dimensions...
  const short            *pF=ncdf.lookupVar(fcClockBreakFlag .name())->data2short();
  const char             *pN=ncdf.lookupVar(fcClockBreakSite .name())->data2char();
  const double           *pT=ncdf.lookupVar(fcClockBreakEpoch.name())->data2double();
  int                     n =ncdf.lookupVar(fcClockBreakEpoch.name())->dimensions().at(0)->getN();
  int                     l =ncdf.lookupVar(fcClockBreakSite .name())->dimensions().last()->getN();
  cbNames.resize(n);
  cbEpochs.resize(n);
  cbFlags.resize(n);
  for (int i=0; i<n; i++)
  {
    cbNames[i]  = QString::fromLatin1(pN + l*i, l);
    cbEpochs[i] = SgMJD(pT[i]<2000000.0?pT[i]:pT[i]-2400000.5);
    cbFlags[i]  = pF[i];
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadClockBreaks(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadLeapSecond(int &leapSeconds)
{
  if (vLeapSecond_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadLeapSecond(): the vgosDb variable LeapSecond is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vLeapSecond_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfLeapSecondIn, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadLeapSecond(): format check failed");
    return false;
  };
  if (ncdf.lookupVar(fcTai_Utc.name()))
  {
    leapSeconds = *(ncdf.lookupVar(fcTai_Utc.name())->data2double() + 1);
    have2redoLeapSecond_ = true;
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadLeapSecond(): found data in the obsolete format, leapSeconds = " + 
      QString("").setNum(leapSeconds));
  }
  else if (ncdf.lookupVar(fcLeapSecond.name()))
  {
    leapSeconds = *(ncdf.lookupVar(fcLeapSecond.name())->data2short());
    have2redoLeapSecond_ = false;
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadLeapSecond(): found data in the current format, leapSeconds = " + 
      QString("").setNum(leapSeconds));
  }
  else
  {
    leapSeconds = 0;
    have2redoLeapSecond_ = true;
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadLeapSecond(): cannot find anything familiar");
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadLeapSecond(): data loaded successfully from " + ncdf.getFileName() );
  return true;
};



//
bool SgVgosDb::loadCalcInfo(int &tidalUt1, double &calcVersionValue)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalcInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadCalcInfo(): the vgosDb variable CalcInfo is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalcInfo, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadCalcInfo(): format check failed");
    return false;
  };
  const short                  *pUt1C=ncdf.lookupVar(fcTidalUt1Control.name())->data2short();
  const double                 *pCver=ncdf.lookupVar(fcCalcVersion.name())->data2double();
  tidalUt1 = *pUt1C;
  calcVersionValue = *pCver;
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadCalcInfo(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadCalcInfo(SgModelsInfo& calcInfo)
{
  SgVdbVariable                &var=progSolveDescriptor_.vCalcInfo_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadCalcInfo(): the vgosDb variable CalcInfo is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCalcInfo, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadCalcInfo(): format check failed");
    return false;
  };

  //
  int                           l, m;
  const short                  *pTidUt1C=ncdf.lookupVar(fcTidalUt1Control.name())?
                                  ncdf.lookupVar(fcTidalUt1Control.name())->data2short():NULL;
  const double                 *pCver=ncdf.lookupVar(fcCalcVersion.name())?
                                  ncdf.lookupVar(fcCalcVersion.name())->data2double():NULL;
  //
  const char                   *pCtrlN=ncdf.lookupVar(fcCalcControlNames    .name())?
                                  ncdf.lookupVar(fcCalcControlNames    .name())->data2char():NULL;
  const short                  *pCtrlF=ncdf.lookupVar(fcCalcControlValues   .name())?
                                  ncdf.lookupVar(fcCalcControlValues   .name())->data2short():NULL;
  const char                   *pAtmM=ncdf.lookupVar(fcATMMessage           .name())?
                                  ncdf.lookupVar(fcATMMessage           .name())->data2char():NULL;
  const char                   *pAtmC=ncdf.lookupVar(fcATMControl           .name())?
                                  ncdf.lookupVar(fcATMControl           .name())->data2char():NULL;
  const char                   *pAxoM=ncdf.lookupVar(fcAxisOffsetMessage    .name())?
                                  ncdf.lookupVar(fcAxisOffsetMessage    .name())->data2char():NULL;
  const char                   *pAxoC=ncdf.lookupVar(fcAxisOffsetControl    .name())?
                                  ncdf.lookupVar(fcAxisOffsetControl    .name())->data2char():NULL;
  const char                   *pEtdM=ncdf.lookupVar(fcEarthTideMessage     .name())?
                                  ncdf.lookupVar(fcEarthTideMessage     .name())->data2char():NULL;
  const char                   *pEtdC=ncdf.lookupVar(fcEarthTideControl     .name())?
                                  ncdf.lookupVar(fcEarthTideControl     .name())->data2char():NULL;
  const char                   *pPtdM=ncdf.lookupVar(fcPoleTideMessage      .name())?
                                  ncdf.lookupVar(fcPoleTideMessage      .name())->data2char():NULL;
  const char                   *pPtdC=ncdf.lookupVar(fcPoleTideControl      .name())?
                                  ncdf.lookupVar(fcPoleTideControl      .name())->data2char():NULL;
  const char                   *pNutM=ncdf.lookupVar(fcNutationMessage      .name())?
                                  ncdf.lookupVar(fcNutationMessage      .name())->data2char():NULL;
  const char                   *pNutC=ncdf.lookupVar(fcNutationControl      .name())?
                                  ncdf.lookupVar(fcNutationControl      .name())->data2char():NULL;
  const char                   *pOtdM=ncdf.lookupVar(fcOceanMessage         .name())?
                                  ncdf.lookupVar(fcOceanMessage         .name())->data2char():NULL;
  const char                   *pOtdC=ncdf.lookupVar(fcOceanControl         .name())?
                                  ncdf.lookupVar(fcOceanControl         .name())->data2char():NULL;
  const char                   *pAtiM=ncdf.lookupVar(fcATIMessage           .name())?
                                  ncdf.lookupVar(fcATIMessage           .name())->data2char():NULL;
  const char                   *pAtiC=ncdf.lookupVar(fcATIControl           .name())?
                                  ncdf.lookupVar(fcATIControl           .name())->data2char():NULL;
  const char                   *pCtiM=ncdf.lookupVar(fcCTIMessage           .name())?
                                  ncdf.lookupVar(fcCTIMessage           .name())->data2char():NULL;
  const char                   *pCtiC=ncdf.lookupVar(fcCTIControl           .name())?
                                  ncdf.lookupVar(fcCTIControl           .name())->data2char():NULL;
  const char                   *pPlxM=ncdf.lookupVar(fcParallaxMessage      .name())?
                                  ncdf.lookupVar(fcParallaxMessage      .name())->data2char():NULL;
  const char                   *pPlxC=ncdf.lookupVar(fcParallaxControl      .name())?
                                  ncdf.lookupVar(fcParallaxControl      .name())->data2char():NULL;
  const char                   *pStrM=ncdf.lookupVar(fcStarMessage          .name())?
                                  ncdf.lookupVar(fcStarMessage          .name())->data2char():NULL;
  const char                   *pStrC=ncdf.lookupVar(fcStarControl          .name())?
                                  ncdf.lookupVar(fcStarControl          .name())->data2char():NULL;
  const char                   *pRelM=ncdf.lookupVar(fcTheoryMessage        .name())?
                                  ncdf.lookupVar(fcTheoryMessage        .name())->data2char():NULL;
  const char                   *pRelC=ncdf.lookupVar(fcRelativityControl    .name())?
                                  ncdf.lookupVar(fcRelativityControl    .name())->data2char():NULL;
  const char                   *pSitM=ncdf.lookupVar(fcSiteMessage          .name())?
                                  ncdf.lookupVar(fcSiteMessage          .name())->data2char():NULL;
  const char                   *pFdHM=ncdf.lookupVar(fcFeedhornMessage      .name())?
                                  ncdf.lookupVar(fcFeedhornMessage      .name())->data2char():NULL;
  const char                   *pEphM=ncdf.lookupVar(fcPepMessage           .name())?
                                  ncdf.lookupVar(fcPepMessage           .name())->data2char():NULL;
  const char                   *pPxyC=ncdf.lookupVar(fcWobbleControl        .name())?
                                  ncdf.lookupVar(fcWobbleControl        .name())->data2char():NULL;
  const char                   *pUt1C=ncdf.lookupVar(fcUT1Control           .name())?
                                  ncdf.lookupVar(fcUT1Control           .name())->data2char():NULL;
  const char                   *pOSts=ncdf.lookupVar(fcOceanStationsFlag    .name())?
                                  ncdf.lookupVar(fcOceanStationsFlag    .name())->data2char():NULL;
  const double                 *pRelD=ncdf.lookupVar(fcRelativityData       .name())?
                                  ncdf.lookupVar(fcRelativityData       .name())->data2double():NULL;
  const double                 *pPrcD=ncdf.lookupVar(fcPrecessionData       .name())?
                                  ncdf.lookupVar(fcPrecessionData       .name())->data2double():NULL;
  const double                 *pEtdD=ncdf.lookupVar(fcEarthTideData        .name())?
                                  ncdf.lookupVar(fcEarthTideData        .name())->data2double():NULL;
  const double                 *pUt1E=ncdf.lookupVar(fcUT1EPOCH             .name())?
                                  ncdf.lookupVar(fcUT1EPOCH             .name())->data2double():NULL;
  const double                 *pWobE=ncdf.lookupVar(fcWOBEPOCH             .name())?
                                  ncdf.lookupVar(fcWOBEPOCH             .name())->data2double():NULL;
  const double                 *pSitZ=ncdf.lookupVar(fcSiteZenithDelay      .name())?
                                  ncdf.lookupVar(fcSiteZenithDelay      .name())->data2double():NULL;

  const double                 *pOptc=ncdf.lookupVar(fcOceanPoleTideCoef    .name())?
                                  ncdf.lookupVar(fcOceanPoleTideCoef    .name())->data2double():NULL;
  const double                 *pOvPh=ncdf.lookupVar(fcOceanUpPhase         .name())?
                                  ncdf.lookupVar(fcOceanUpPhase         .name())->data2double():NULL;
  const double                 *pOhPh=ncdf.lookupVar(fcOceanHorizontalPhase .name())?
                                  ncdf.lookupVar(fcOceanHorizontalPhase .name())->data2double():NULL;
  const double                 *pOvAm=ncdf.lookupVar(fcOceanUpAmp           .name())?
                                  ncdf.lookupVar(fcOceanUpAmp           .name())->data2double():NULL;
  const double                 *pOhAm=ncdf.lookupVar(fcOceanHorizontalAmp   .name())?
                                  ncdf.lookupVar(fcOceanHorizontalAmp   .name())->data2double():NULL;

  //
  //
  if (pTidUt1C)
    calcInfo.setFlagTidalUt1(*pTidUt1C);
  if (pCver)
    calcInfo.setDversion(*pCver);
  //
  if (pCtrlN && pCtrlF)
  {
    l = ncdf.lookupVar(fcCalcControlNames.name())->dimensions().at(0)->getN();
    m = ncdf.lookupVar(fcCalcControlNames.name())->dimensions().at(1)->getN();
    if (l != ncdf.lookupVar(fcCalcControlValues.name())->dimensions().at(0)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::loadCalcInfo(): the dimensions of CalcControlNames and CalcControlValues mismatch,"
        " the file is \"" + ncdf.getFileName() + "\"");
    else
    {
      QList<int>*                flagVals=&calcInfo.controlFlagValues();
      QList<QString>*            flagNames=&calcInfo.controlFlagNames();
      flagVals->clear();
      flagNames->clear();
      for (int i=0; i<l; i++)
      {
        flagVals->append(pCtrlF[i]);
        flagNames->append(QString::fromLatin1(pCtrlN + m*i, m) );
      };
    };
  };
  //
  if (pAtmM && pAtmC)
  {
    calcInfo.troposphere().setDefinition(QString::fromLatin1(pAtmM, 
      ncdf.lookupVar(fcATMMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.troposphere().setControlFlag(QString::fromLatin1(pAtmC, 
      ncdf.lookupVar(fcATMControl.name())->dimensions().at(0)->getN() ));
  };
  // 
  if (pAxoM && pAxoC)
  {
    calcInfo.axisOffset().setDefinition(QString::fromLatin1(pAxoM,
      ncdf.lookupVar(fcAxisOffsetMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.axisOffset().setControlFlag(QString::fromLatin1(pAxoC,
      ncdf.lookupVar(fcAxisOffsetControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pEtdM && pEtdC)
  {
    calcInfo.earthTide().setDefinition(QString::fromLatin1(pEtdM,
      ncdf.lookupVar(fcEarthTideMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.earthTide().setControlFlag(QString::fromLatin1(pEtdC,
      ncdf.lookupVar(fcEarthTideControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pPtdM && pPtdC)
  {
    calcInfo.poleTide().setDefinition(QString::fromLatin1(pPtdM,
      ncdf.lookupVar(fcPoleTideMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.poleTide().setControlFlag(QString::fromLatin1(pPtdC,
      ncdf.lookupVar(fcPoleTideControl.name())->dimensions().at(0)->getN() ));
  };

  //
  if (pNutM && pNutC)
  {
    calcInfo.nutation().setDefinition(QString::fromLatin1(pNutM,
      ncdf.lookupVar(fcNutationMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.nutation().setControlFlag(QString::fromLatin1(pNutC,
      ncdf.lookupVar(fcNutationControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pOtdM && pOtdC)
  {
    calcInfo.oceanLoading().setDefinition(QString::fromLatin1(pOtdM,
      ncdf.lookupVar(fcOceanMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.oceanLoading().setControlFlag(QString::fromLatin1(pOtdC,
      ncdf.lookupVar(fcOceanControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pAtiM && pAtiC)
  {
    calcInfo.atomicTime().setDefinition(QString::fromLatin1(pAtiM,
      ncdf.lookupVar(fcATIMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.atomicTime().setControlFlag(QString::fromLatin1(pAtiC,
      ncdf.lookupVar(fcATIControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pCtiM && pCtiC)
  {
    calcInfo.coordinateTime().setDefinition(QString::fromLatin1(pCtiM,
      ncdf.lookupVar(fcCTIMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.coordinateTime().setControlFlag(QString::fromLatin1(pCtiC,
      ncdf.lookupVar(fcCTIControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pPlxM && pPlxC)
  {
    calcInfo.parallax().setDefinition(QString::fromLatin1(pPlxM,
      ncdf.lookupVar(fcParallaxMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.parallax().setControlFlag(QString::fromLatin1(pPlxC,
      ncdf.lookupVar(fcParallaxControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pStrM && pStrC)
  {
    calcInfo.star().setDefinition(QString::fromLatin1(pStrM,
      ncdf.lookupVar(fcStarMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.star().setControlFlag(QString::fromLatin1(pStrC,
      ncdf.lookupVar(fcStarControl.name())->dimensions().at(0)->getN() ));
  };
  //
  if (pRelM && pRelC)
  {
    calcInfo.relativity().setDefinition(QString::fromLatin1(pRelM,
      ncdf.lookupVar(fcTheoryMessage.name())->dimensions().at(0)->getN() ));
    calcInfo.relativity().setControlFlag(QString::fromLatin1(pRelC,
      ncdf.lookupVar(fcRelativityControl.name())->dimensions().at(0)->getN() ));
  };
  if (pSitM)
    calcInfo.site().setDefinition(QString::fromLatin1(pSitM,
      ncdf.lookupVar(fcSiteMessage.name())->dimensions().at(0)->getN()));
  if (pFdHM)
    calcInfo.feedHorn().setDefinition(QString::fromLatin1(pFdHM,
      ncdf.lookupVar(fcFeedhornMessage.name())->dimensions().at(0)->getN()));
  if (pEphM)
    calcInfo.ephemeris().setDefinition(QString::fromLatin1(pEphM,
      ncdf.lookupVar(fcPepMessage.name())->dimensions().at(0)->getN()));
  if (pPxyC)
    calcInfo.polarMotion().setControlFlag(QString::fromLatin1(pPxyC,
      ncdf.lookupVar(fcWobbleControl.name())->dimensions().at(0)->getN()));
  if (pUt1C)
    calcInfo.ut1().setControlFlag(QString::fromLatin1(pUt1C,
      ncdf.lookupVar(fcUT1Control.name())->dimensions().at(0)->getN()));
  //
  if (pOSts)
  {
    int                         nOcnLdSts, lOcnLdSts;
    nOcnLdSts = ncdf.lookupVar(fcOceanStationsFlag.name())->dimensions().at(0)->getN();
    lOcnLdSts = ncdf.lookupVar(fcOceanStationsFlag.name())->dimensions().at(1)->getN();
    
    QList<QString>             *statuses=&calcInfo.oLoadStationStatus();
    statuses->clear();
    for (int i=0; i<nOcnLdSts; i++)
      statuses->append(QString::fromLatin1(pOSts + lOcnLdSts*i, lOcnLdSts));
  };
  //
  if (pRelD)
    calcInfo.setRelativityData(*pRelD);
  if (pPrcD)
    calcInfo.setPrecessionData(*pPrcD);
  if (pEtdD)
  {
    calcInfo.setEarthTideData(0, pEtdD[0]);
    calcInfo.setEarthTideData(1, pEtdD[1]);
    calcInfo.setEarthTideData(2, pEtdD[2]);
  };
  //
  if (pUt1E)
  {
    m = ncdf.lookupVar(fcUT1EPOCH.name())->dimensions().at(1)->getN();
    calcInfo.prepareUt1InterpData(m);
    for (int i=0; i<m; i++)
      for (int j=0; j<2; j++)
        calcInfo.ut1InterpData()->setElement(i, j,  pUt1E[2*i + j]);
  };
  //
  if (pWobE)
  {
    m = ncdf.lookupVar(fcWOBEPOCH.name())->dimensions().at(1)->getN();
    calcInfo.prepareWobInterpData(m);
    for (int i=0; i<m; i++)
      for (int j=0; j<2; j++)
        calcInfo.wobInterpData()->setElement(i, j,  pWobE[2*i + j]);
  };
  //
  if (pSitZ)
  {
    m = ncdf.lookupVar(fcSiteZenithDelay.name())->dimensions().at(0)->getN();
    calcInfo.siteZenDelays().clear();
    for (int i=0; i<m; i++)
      calcInfo.siteZenDelays().append(pSitZ[i]);
  };
  //
  if (pOvPh && pOhPh && pOvAm && pOhAm && pOptc)
  {
    m = ncdf.lookupVar(fcOceanPoleTideCoef.name())->dimensions().at(0)->getN();
    l = ncdf.lookupVar(fcOceanUpPhase.name())->dimensions().at(1)->getN();
    
    if (m != ncdf.lookupVar(fcOceanUpPhase.name())->dimensions().at(0)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): dimensions of OceanPoleTideCoef and OceanUpPhase mismatch");
    else if (m != ncdf.lookupVar(fcOceanHorizontalPhase.name())->dimensions().at(0)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): dimensions of OceanPoleTideCoef and OceanHorizontalPhase mismatch");
    else if (m != ncdf.lookupVar(fcOceanUpAmp.name())->dimensions().at(0)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): dimensions of OceanPoleTideCoef and OceanUpAmp mismatch");
    else if (m != ncdf.lookupVar(fcOceanHorizontalAmp.name())->dimensions().at(0)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): dimensions of OceanPoleTideCoef and OceanHorizontalAmp mismatch");
    else if (m != calcInfo.stations().size())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): dimensions of OceanPoleTideCoef and number of stations mismatch");
    else if (l != 11)
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension of OceanUpPhase");
    else if (l != ncdf.lookupVar(fcOceanHorizontalPhase.name())->dimensions().at(2)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension(2) of OceanHorizontalPhase");
    else if (l != ncdf.lookupVar(fcOceanUpAmp.name())->dimensions().at(1)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension of OceanUpAmp");
    else if (l != ncdf.lookupVar(fcOceanHorizontalAmp.name())->dimensions().at(2)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension(2) of OceanHorizontalAmp");
    else if (2 != ncdf.lookupVar(fcOceanHorizontalPhase.name())->dimensions().at(1)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension(1) of OceanHorizontalPhase");
    else if (2 != ncdf.lookupVar(fcOceanHorizontalAmp.name())->dimensions().at(1)->getN())
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::loadCalcInfo(): unexpected dimension(1) of OceanHorizontalAmp");
    else
    {
      for (int i=0; i<m; i++)
      {
        SgVlbiStationInfo       *si=calcInfo.stations().at(i);
        if (si)
        {
          for (int j=0; j<l; j++)
          {
            si->setOLoadPhase(j, 0,  pOvPh[l*i       + j]);
            si->setOLoadPhase(j, 1,  pOhPh[2*l*i     + j]);
            si->setOLoadPhase(j, 2,  pOhPh[2*l*i + l + j]);
            
            si->setOLoadAmplitude(j, 0, pOvAm[l*i       + j]);
            si->setOLoadAmplitude(j, 1, pOhAm[2*l*i     + j]);
            si->setOLoadAmplitude(j, 2, pOhAm[2*l*i + l + j]);
          };
          for (int j=0; j<6; j++)
            si->setOptLoadCoeff(j, pOptc[6*i + j]);
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::loadCalcInfo(): the station # " + QString("").setNum(i) + " is NULL");
      };
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadCalcInfo(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadMiscCable(QMap<QString, QString>& cableSignByKey)
{
  SgVdbVariable                &var=vMiscCable_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadMiscCable(): the vgosDb variable MiscCable is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfMiscCable, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadMiscCable(): format check failed");
    return false;
  };
  if (!ncdf.lookupVar(fcCableSign.name())) // it is possible, no signs at all..
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadMiscCable(): the vgosDb variable " + fcCableSign.name() + " missed in the file");
    return false;
  };
  QString                       sSign, sKey;
  int                           lSgns, nStns, lStns;
  const char                   *pSgn=ncdf.lookupVar(fcCableSign.name())->data2char();
  const char                   *pStn=ncdf.lookupVar(fcCableStnName.name())->data2char();
  lSgns = ncdf.lookupVar(fcCableSign.name())->dimensions().at(0)->getN();
  nStns = ncdf.lookupVar(fcCableStnName.name())->dimensions().at(0)->getN();
  lStns = ncdf.lookupVar(fcCableStnName.name())->dimensions().at(1)->getN();

  for (int i=0; i<nStns; i++)
  {
    if (2*i < lSgns)
      sSign = QString::fromLatin1(pSgn + 2*i, 1); // take only first char: CableSign = "+ - - + + +"
    else 
      sSign = "?";
    sKey = QString::fromLatin1(pStn + lStns*i, lStns);
    if (sKey.simplified().size()> 1 && 0<*(pStn + lStns*i))
    {
      if (!cableSignByKey.contains(sKey))
        cableSignByKey.insert(sKey, sSign);
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
          "::loadMiscCable(): got a duplicated stn name [" + sKey + "](" + sSign +
          "); previous value is [" + cableSignByKey.key(sKey) + "](" + cableSignByKey.value(sKey) + 
          ") from " + ncdf.getFileName());
      };
    };// everything else is just garbage
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadMiscCable(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadMiscFourFit(const QString& band,
  QVector<QString>& fourfitControlFile, QVector<QString>& fourfitCommand,
  QVector<int>& numLags, QVector<double>& apLength)
{
  SgVdbVariable                *var=NULL;
  double                        fmtVer=inputFormatVersion_;

  if (band.size())
  {
    if (!bandDataByName_.contains(band))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
        "::loadMiscFourFit(): the band \"" + band + "\" is not registered");
      return false;
    };
    var = &bandDataByName_[band].vMiscFourFit_;
  }
  else
    var = &vMiscFourFit_;

  //
  if (var->isEmpty())
  {
    if (band.size())
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadMiscFourFit(): the vgosDb variable MiscFourFit" +
      (band.size()?" for the band [" + band + "]" :"") + " is empty");
    else
      logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadMiscFourFit(): the vgosDb variable MiscFourFit is empty");
    return false;
  };

  FmtChkVar                    *fcNumLagsUsed, *fcFourFitCmdCString, *fcFourfitControlFile, *fcAplength;
  QList<SgVgosDb::FmtChkVar*>  *fcfMiscFourFit;
//  QList<SgVgosDb::FmtChkVar*>   fcfMiscFourFit;
  int                           strIdx;
  
  // get data and figure out the format :-/
/*
  // check content first:
  SgNcdfVariable               *svcV;
  if ( (svcV=ncdf.lookupServiceVar(SgNetCdf::svcVgosDbVersion.toUpper())) )
  {
    const char                 *c=svcV->data2char();
    int                         l=svcV->dimensions().at(0)->getN();
    QString                     fmtVersion(QString::fromLatin1(c, l));
    // quick and dirty:
    if (fmtVersion.left(5)==QString("1.004") && fmtVer<=1.002)
    {
      fcNumLagsUsed = &fcNumLagsUsed_v1004;
      fcFourFitCmdCString = &fcFourFitCmdCString_v1004;
      fcFourfitControlFile = &fcFourfitControlFile_v1004;
      fcAplength = &fcAplength_v1004;
      fcfMiscFourFit.clear();
      fcfMiscFourFit << fcFourFitCmdCString << fcNumLagsUsed << fcFourfitControlFile << fcAplength;
      strIdx = 0;
      fmtVer = 1.004;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::loadMiscFourFit(): format version has been corrected to 1.004");
    };
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadMiscFourFit(): cannot find service variable \"" + SgNetCdf::svcVgosDbVersion + 
      "\" in " + ncdf.getFileName());
  */

  SgNetCdf                      ncdf(path2RootDir_ + "/" + var->getFileName());
  ncdf.getData();

  SgNcdfVariable               *ncdfVar=ncdf.lookupVar(fcFourFitCmdCString_v1002.name());
  if (ncdfVar)
  {
    if (ncdfVar->dimensions().size() == 2) // ver 1.002:
    {
      fcNumLagsUsed = &fcNumLagsUsed_v1002;
      fcFourFitCmdCString = &fcFourFitCmdCString_v1002;
      fcFourfitControlFile = &fcFourfitControlFile_v1002;
      fcAplength = &fcAplength_v1002;
      fcfMiscFourFit = &fcfMiscFourFit_v1002;
      strIdx = 1;
      if (1.002 < fmtVer)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
          "::loadMiscFourFit(): the input format version has been adjusted to 1.002");
        fmtVer = 1.002;
      };
    }
    else if (ncdfVar->dimensions().size() == 1) // ver 1.004:
    {
      fcNumLagsUsed = &fcNumLagsUsed_v1004;
      fcFourFitCmdCString = &fcFourFitCmdCString_v1004;
      fcFourfitControlFile = &fcFourfitControlFile_v1004;
      fcAplength = &fcAplength_v1004;
      fcfMiscFourFit = &fcfMiscFourFit_v1004;
      strIdx = 0;
      if (fmtVer <= 1.002)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
          "::loadMiscFourFit(): the input format version has been adjusted to 1.004");
        fmtVer = 1.004;
      };
    }
    else
    {
      strIdx = -1;
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::loadMiscFourFit(): get unknown version of the file, number of dimensions is " +
        QString("").setNum(ncdfVar->dimensions().size()));
      return false;
    };
  }
  else
  {
    strIdx = -1;
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadMiscFourFit(): unexpected content of the file: the variable \"" +
      fcFourFitCmdCString_v1002.name() + "\" is not found");
    return false;
  };
  //
  //
  //
  //
  //
  if (!checkFormat(*fcfMiscFourFit, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadMiscFourFit(): format check failed");
    return false;
  };

  fourfitControlFile.resize(numOfObs_);
  fourfitCommand.resize(numOfObs_);
  numLags.resize(numOfObs_);
  apLength.resize(numOfObs_);

  int                           lFFcmd, lFFctrl;
/*
  const short                  *pNumLags=ncdf.lookupVar(fcNumLagsUsed_v1002.name())->data2short();
  const char                   *pFFcmd=ncdf.lookupVar(fcFourFitCmdCString_v1002.name())->data2char();
  const char                   *pFFctrl=ncdf.lookupVar(fcFourfitControlFile_v1002.name())->data2char();
  const double                 *pApLength=ncdf.lookupVar(fcAplength_v1002.name())->data2double();

  lFFcmd = ncdf.lookupVar(fcFourFitCmdCString_v1002.name())->dimensions().at(1)->getN();
  lFFctrl = ncdf.lookupVar(fcFourfitControlFile_v1002.name())->dimensions().at(1)->getN();
*/
  const short                  *pNumLags=ncdf.lookupVar(fcNumLagsUsed->name())->data2short();
  const char                   *pFFcmd=ncdf.lookupVar(fcFourFitCmdCString->name())->data2char();
  const char                   *pFFctrl=ncdf.lookupVar(fcFourfitControlFile->name())->data2char();
  const double                 *pApLength=ncdf.lookupVar(fcAplength->name())->data2double();

//  const int                    *pCppSoft=NULL;
//  if (ncdf.lookupVar(fcCorrPostProcSoftware.name()))
//    pCppSoft = ncdf.lookupVar(fcCorrPostProcSoftware.name())->data2int();
  
  lFFcmd = ncdf.lookupVar(fcFourFitCmdCString->name())->dimensions().at(strIdx)->getN();
  lFFctrl = ncdf.lookupVar(fcFourfitControlFile->name())->dimensions().at(strIdx)->getN();
  
  for (int i=0; i<numOfObs_; i++)
  {
    if (fmtVer <= 1.002)
    {
      fourfitCommand[i] = QString::fromLatin1(pFFcmd + lFFcmd*i, lFFcmd);
      fourfitControlFile[i] = QString::fromLatin1(pFFctrl + lFFctrl*i, lFFctrl);
      numLags[i] = *(pNumLags + i);
      apLength[i] = *(pApLength + i);
    }
    else // the same for all obs:
    {
      fourfitCommand[i] = QString::fromLatin1(pFFcmd, lFFcmd);
      fourfitControlFile[i] = QString::fromLatin1(pFFctrl, lFFctrl);
      numLags[i] = *pNumLags;
      apLength[i] = *pApLength;
    };
  };
  //
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadMiscFourFit(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadSessionHead(QString& corrType, QString& corrName, QString& piName,
  QString& experimentName, int& experimentSerialNumber, QString& experimentDescription, 
  QString& recordingMode, QList<QString>& stations, QList<QString>& sources, 
  SgMJD& tStart, SgMJD& tFinis, int& cppsIdx)
{
  int                           numObs=0, numScan=0;
  SgNcdfVariable               *ndcV=NULL;
  //
  if (vHead_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadSessionHead(): the vgosDb variable Head is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vHead_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfHead, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadSessionHead(): format check failed");
    return false;
  };
  
  // corrType:
  ndcV = ncdf.lookupVar(fcCorrelatorType.name());
  if (ndcV)
    corrType = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // corrName:
  ndcV = ncdf.lookupVar(fcCorrelator.name());
  if (ndcV)
    corrName = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // piName:
  ndcV = ncdf.lookupVar(fcPrincipalInvestigator.name());
  if (ndcV)
    piName = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // experimentName:
  ndcV = ncdf.lookupVar(fcExpName.name());
  if (ndcV)
    experimentName = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // experimentDesription:
  ndcV = ncdf.lookupVar(fcExpDescription.name());
  if (ndcV)
    experimentDescription = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // recordingMode:
  ndcV = ncdf.lookupVar(fcRecordingMode.name());
  if (ndcV)
    recordingMode = QString::fromLatin1(ndcV->data2char(), ndcV->dimensions().at(0)->getN());
  // CppSoftware:
  ndcV = ncdf.lookupVar(fcCorrPostProcSoftware.name());
  if (ndcV)
    cppsIdx = *ndcV->data2int();
  // numbers of *:
  ndcV = ncdf.lookupVar(fcNumObs.name());
  if (ndcV)
    numObs = *ndcV->data2int();
  ndcV = ncdf.lookupVar(fcNumScan.name());
  if (ndcV)
    numScan = *ndcV->data2int();
  // experimentSerialNumber:
  ndcV = ncdf.lookupVar(fcExpSerialNumber.name());
  if (ndcV)
    experimentSerialNumber = *ndcV->data2short();
  //
  //
  int                           l, n;
  const char                   *c;
 
  stations.clear();
  sources.clear();
  // stations:
  ndcV = ncdf.lookupVar(fcStationList.name());
  if (ndcV)
  {
    c = ndcV->data2char();
    n = ndcV->dimensions().at(0)->getN();
    l = ndcV->dimensions().at(1)->getN();
    for (int i=0; i<n; i++)
      stations << QString::fromLatin1(c + l*i, l);
  };
  // sources:
  ndcV = ncdf.lookupVar(fcSourceList.name());
  if (ndcV)
  {
    c = ndcV->data2char();
    n = ndcV->dimensions().at(0)->getN();
    l = ndcV->dimensions().at(1)->getN();
    for (int i=0; i<n; i++)
      sources << QString::fromLatin1(c + l*i, l);
  };
  //
  ndcV = ncdf.lookupVar(fciUTCInterval.name());
  if (ndcV)
  {
    const short                *pYMDHM=ndcV->data2short();
    tStart = SgMJD(pYMDHM[0], pYMDHM[1], pYMDHM[2], pYMDHM[3], pYMDHM[4], 0.0);
    tFinis = SgMJD(pYMDHM[5], pYMDHM[6], pYMDHM[7], pYMDHM[8], pYMDHM[9], 0.0);
  };
  //
  // check?
  if (numOfObs_ && numObs && numOfObs_ != numObs)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadSessionHead(): the declared number of observations, " + QString("").setNum(numObs) + 
      ", does not equal to the actual number, " + QString("").setNum(numOfObs_));
  if (numOfScans_ && numScan && numOfScans_ != numScan)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadSessionHead(): the declared number of scans, " + QString("").setNum(numScan) + 
      ", does not equal to the actual number, " + QString("").setNum(numOfScans_));
  //
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadSessionHead(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadHistory(QList<SgVlbiHistoryRecord*>& history)
{
  QString                       str, strAux;
 
//if (historyDescriptorByName_.size()==0)
  if (historyDescriptors_.size()==0)
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      "::loadHistory(): the history collector is empty");
    history.clear();
    return false;
  };
  //
  QMap<QString, const HistoryDescriptor*>           hdByEpoch;
  int                                               idx(0);
//for (QMap<QString, HistoryDescriptor>::iterator it=historyDescriptorByName_.begin(); 
//  it!=historyDescriptorByName_.end(); ++it)
  for (int i=0; i<historyDescriptors_.size(); i++)
  {
    const HistoryDescriptor    &hd=historyDescriptors_.at(i);
//
// unfortunately, nobody respects timekeeping
//    str = hd.epochOfCreation_.toString(SgMJD::F_INTERNAL) + QString("").sprintf("__%06d", idx++);
    str = QString("").sprintf("__%06d", idx++);
    hdByEpoch[str] = &hd;
  };
  // read them:
  const QString                 timeTag("TIMETAG"), versionTag("MK3DB_VERSION");
  int                           version(-1), latestVersion(0), numOfParsedStrings(0);
  SgMJD                         t(tZero);
  for (QMap<QString, const HistoryDescriptor*>::iterator it=hdByEpoch.begin(); 
    it!=hdByEpoch.end(); ++it)
  {
    const HistoryDescriptor    *hd=it.value();
    str = path2RootDir_;
    if (hd->defaultDir_.size())
      str += "/" + hd->defaultDir_;
    QFile                       f(str + "/" + hd->historyFileName_);
    if (!f.exists())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::loadHistory(): the history file [" + f.fileName() + "] does not exist");
    }
    else
    {
      //
      if (f.open(QFile::ReadOnly))
      {
        QTextStream             s(&f);
        bool                    isOk;
        int                     numOfStrs(0);
        t = tZero;
        while (!s.atEnd())
        {
          str = s.readLine();
          if (str.startsWith(timeTag, Qt::CaseInsensitive)) // parse time tag:
          {
            bool                isUtc=false;
            strAux = str.mid(timeTag.length() + 1).simplified();
            if (strAux.endsWith("UTC"))
            {
              strAux.chop(3);
              isUtc = true;
            };
            if (!t.fromString(SgMJD::F_YYYYMMDDHHMMSSSS, strAux))
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                "::loadHistory(): cannot guess an epoch from [" + strAux + "] string");
            else if (!isUtc)
              t = t.toUtc();
          }
          else if (str.startsWith(versionTag, Qt::CaseInsensitive)) // parse version tag:
          {
            strAux = str.mid(versionTag.length() + 1).simplified();
            version = strAux.toInt(&isOk);
            if (!isOk)
            {
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                "::loadHistory(): cannot guess a version number from [" + strAux + "] string");
              version = -1;
            }
            else
              latestVersion = version;
          }
          else if (tZero < t) // ignore all before the first time tag:
          {
            history.append(new SgVlbiHistoryRecord(t, version==-1?latestVersion+1:version, str));
            if (lastModified_ < t)
              lastModified_ = t;
          };
          numOfStrs++;
        };
        f.close();
        s.setDevice(NULL);
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::loadHistory(): parsed " + QString("").setNum(numOfStrs) + 
          " strings from the history file [" + f.fileName() + "]");
        numOfParsedStrings += numOfStrs;
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::loadHistory(): cannot open the history file [" + f.fileName() + "] for read access");
      };
    };
  };
  currentVersion_ = latestVersion;
  // put fingerprints: 
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": Library ID: " + libraryVersion.name() + " released on " +  
    libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY)));
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": Driver  ID: " + currentDriverVersion_->name() + " released on " + 
    currentDriverVersion_->getReleaseEpoch().toString(SgMJD::F_DDMonYYYY)));
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": Host    ID: " + currentIdentities_->getMachineNodeName() + 
    " (Hw: " + currentIdentities_->getMachineMachineName() + 
    "; Sw: " + currentIdentities_->getMachineRelease() + 
    " version of " + currentIdentities_->getMachineSysName() + ")"));
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": User    ID: " + currentIdentities_->getUserName() + 
    " <" + currentIdentities_->getUserEmailAddress() + ">, " + currentIdentities_->getAcFullName()));
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": Input data: " + path2RootDir_ + "/" + wrapperFileName_));
  history.append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), currentVersion_ + 1,
    currentDriverVersion_->getSoftwareName() + 
    ": Command   : " + currentIdentities_->getUserCommand()));
  return true;
};
/*=====================================================================================================*/







/*=====================================================================================================*/
//
// Constants:
//



// Variable descriptions:
SgVgosDb::FmtChkVar
  // Source Apriori:
  fcSourceNameApriori   ("AprioriSourceList",   NC_CHAR,    true, QList<int>() << SD_NumSrc <<  8,
    "STRNAMES", "Source names in RA order."),
  fcSource2000RaDec     ("AprioriSource2000RaDec",   
                                                NC_DOUBLE,  true, QList<int>() << SD_NumSrc <<  2,
    "STAR2000", "J2000 Source RA and Dec"),
  fcSourceReference     ("AprioriSourceReference",   
                                                NC_CHAR,    false,QList<int>() << SD_NumSrc << SD_Any,
    "STAR REF", "Source of coordinate values."),
  // Clock Apriori:
//fcClockAprioriSite    ("ClockAprioriSite",    NC_CHAR,    true, QList<int>() << SD_Any <<  8,
  fcClockAprioriSite    ("AprioriClockStationList",
                                                NC_CHAR,    true, QList<int>() << SD_Any <<  8,
    "STAT_ACM", "Stations with a priori clock mod"),
//fcClockAprioriOffset  ("ClockAprioriOffset",  NC_DOUBLE,  true, QList<int>() << SD_Any,
  fcClockAprioriOffset  ("AprioriClockOffset",  NC_DOUBLE,  true, QList<int>() << SD_Any,
    "CLOOFACM", "A priori clock offset (sec)"),
//fcClockAprioriRate    ("ClockAprioriRate",    NC_DOUBLE,  true, QList<int>() << SD_Any,
  fcClockAprioriRate    ("AprioriClockRate",    NC_DOUBLE,  true, QList<int>() << SD_Any,
    "CLODRACM", "A priori clock drift (sec/sec)"),

  // Station Apriori:
  fcStationNameApriori  ("AprioriStationList",  NC_CHAR,    true, QList<int>() << SD_NumStn <<  8,
    "SITNAMES", "Site names in alphabetical order"),
  fcStationXYZ          ("AprioriStationXYZ",   NC_DOUBLE,  true, QList<int>() << SD_NumStn <<  3,
    "SITERECS", "Site cartesian coords (m).", "meter"),
  fcStationPlateName    ("AprioriStationTectonicPlate",
                                                NC_CHAR,    true, QList<int>() << SD_NumStn <<  4,
    "TECTPLNM", "4-char tectonic plate names."),
  // Antenna Apriori:
  fcAxisType            ("AntennaAxisType",     NC_SHORT,   true, QList<int>() << SD_NumStn,
    "AXISTYPS", "Axis type (1-eq 2-xy 3-azel 4 5)"),
  fcAntennaName         ("AntennaStationList",  NC_CHAR,    true, QList<int>() << SD_NumStn <<  8,
    "", "List of Antennas"),
  fcAxisOffset          ("AntennaAxisOffset",   NC_DOUBLE,  true, QList<int>() << SD_NumStn,
    "AXISOFFS", "Axis offsets (m).", "meter"),
  fcAxisTilt            ("AntennaAxisTilt",     NC_DOUBLE,  false,QList<int>() << SD_NumStn <<  2,
    "AXISTILT", "Fixed axis tilt", "radian"),

  // CalcERP:
  fcUT1IntrpMode        ("UT1IntrpMode",        NC_CHAR,    true, QList<int>() << SD_Any,
    "UT1INTRP", "Message for UT1 interp. scheme"),
  fcWobIntrpMode        ("WobIntrpMode",        NC_CHAR,    true, QList<int>() << SD_Any,
    "WOBINTRP", "Interp. scheme for polar motion."),
  fcCalcUt1Module       ("CalcUt1Module",       NC_CHAR,    true, QList<int>() << SD_Any,
    "UT1 MESS", "UT1 Module message definition"),
  fcCalcWobModule       ("CalcWobModule",       NC_CHAR,    true, QList<int>() << SD_Any,
    "WOB MESS", "Wobble message definition."),
  fcUT1Origin           ("UT1Origin",           NC_CHAR,    true, QList<int>() << SD_Any,
    "FUT1TEXT", "Final Value TAI-UT1 origin text."),
  fcWobbleOrigin        ("WobbleOrigin",        NC_CHAR,    true, QList<int>() << SD_Any,
    "FWOBTEXT", "Final Value wobble origin text."),
  fcUT1ArrayInfo        ("UT1ArrayInfo",        NC_DOUBLE,  true, QList<int>() << 4,
    "FUT1 INF", "Array: (FJD of start, spacing in days, number points, Scaling (should be 1))"),
  fcWobArrayInfo        ("WobArrayInfo",        NC_DOUBLE,  true, QList<int>() << 3,
    "FWOB INF", "Array: (FJD of start, spacing in days, number points)"),
  fcUT1Values           ("UT1Values",           NC_DOUBLE,  true, QList<int>() << SD_Any,
    "FUT1 PTS", "Final Value TAI-UT1 data points."),
  fcWobValues           ("WobValues",           NC_DOUBLE,  true, QList<int>() << SD_Any << 2,
    "FWOBX&YT", "Final wobble X Y component value"),
  // CalcInfo:
  fcTidalUt1Control     ("TidalUT1Control",     NC_SHORT,   true, QList<int>() << 1,
    "TIDALUT1", "Flag for tidal terms in UT1 sers"),
  fcCalcVersion         ("CalcVersion",         NC_DOUBLE,  true, QList<int>() << 1,
    "CALC VER", "CALC version number"),
  fcCalcControlNames    ("CalcControlNames",    NC_CHAR,    true, QList<int>() << SD_Any << SD_Any,
    "CALCFLGN", "CALC flow control flags name def"),
  fcCalcControlValues   ("CalcControlValues",   NC_SHORT,   true, QList<int>() << SD_Any,
    "CALCFLGV", "CALC flow control flags valu def"),
  fcATMMessage          ("ATMMessage",          NC_CHAR,    true, QList<int>() << SD_Any,
    "ATM MESS", "Atmosphere message definition"),
  fcATMControl          ("ATMControl",          NC_CHAR,    true, QList<int>() << SD_Any,
    "ATM CFLG", "Atmosphere control flag mess def"),
  fcAxisOffsetMessage   ("AxisOffsetMessage",   NC_CHAR,    true, QList<int>() << SD_Any,
    "AXO MESS", "Axis Offset Message Definition"),
  fcAxisOffsetControl   ("AxisOffsetControl",   NC_CHAR,    true, QList<int>() << SD_Any,
    "AXO CFLG", "Axis Offset Control flag mes def"),
  fcEarthTideMessage    ("EarthTideMessage",    NC_CHAR,    true, QList<int>() << SD_Any,
    "ETD MESS", "Earth Tide message definition"),
  fcEarthTideControl    ("EarthTideControl",    NC_CHAR,    true, QList<int>() << SD_Any,
    "ETD CFLG", "Earth Tide flow control mess def"),
  fcPoleTideMessage     ("PoleTideMessage",     NC_CHAR,    true, QList<int>() << SD_Any,
    "PTD MESS", "Pole tide message definition"),
  fcPoleTideControl     ("PoleTideControl",     NC_CHAR,    true, QList<int>() << SD_Any,
    "PTD CFLG", "Pole tide flow control mess def"),
  fcNutationMessage     ("NutationMessage",     NC_CHAR,    true, QList<int>() << SD_Any,
    "NUT MESS", "Nutation message definition"),
  fcNutationControl     ("NutationControl",     NC_CHAR,    true, QList<int>() << SD_Any,
    "NUT CFLG", "Nutation flow control mess def."),
  fcOceanMessage        ("OceanMessage",        NC_CHAR,    true, QList<int>() << SD_Any,
    "OCE MESS", "Ocean loading message definition"),
  fcOceanControl        ("OceanControl",        NC_CHAR,    true, QList<int>() << SD_Any,
    "OCE CFLG", "Ocean load flow control mess def"),
  fcATIMessage          ("ATIMessage",          NC_CHAR,    true, QList<int>() << SD_Any,
    "ATI MESS", "ATIME Message Definition"),
  fcATIControl          ("ATIControl",          NC_CHAR,    true, QList<int>() << SD_Any,
    "ATI CFLG", "ATIME Flow Control Message Def."),
  fcCTIMessage          ("CTIMessage",          NC_CHAR,    true, QList<int>() << SD_Any,
    "CTI MESS", "CTIMG Message Definition"),
  fcCTIControl          ("CTIControl",          NC_CHAR,    true, QList<int>() << SD_Any,
    "CTI CFLG", "CTIMG Flow Control Message Def"),
  fcParallaxMessage     ("ParallaxMessage",     NC_CHAR,    true, QList<int>() << SD_Any,
    "PLX MESS", "Parallax message definition"),
  fcParallaxControl     ("ParallaxControl",     NC_CHAR,    true, QList<int>() << SD_Any,
    "PLX CFLG", "Parallax flow control mess def"),
  fcStarMessage         ("StarMessage",         NC_CHAR,    true, QList<int>() << SD_Any,
    "STR MESS", "Star module message definition"),
  fcStarControl         ("StarParallaxControl", NC_CHAR,    true, QList<int>() << SD_Any,
    "STR CFLG", "Parallax flow control mess def"),
  fcTheoryMessage       ("TheoryMessage",       NC_CHAR,    true, QList<int>() << SD_Any,
    "THE MESS", "Theory module identification"),
  fcRelativityControl   ("RelativityControl",   NC_CHAR,    true, QList<int>() << SD_Any,
    "REL CFLG", "Relativisitc bending use status"),
  fcSiteMessage         ("SiteMessage",         NC_CHAR,    true, QList<int>() << SD_Any,
    "SIT MESS", "Site Module Message Definition"),
  fcFeedhornMessage     ("FeedhornMessage",     NC_CHAR,    true, QList<int>() << SD_Any,
    "PAN MESS", "Feedhorn rot. angle mod. ident."),
  fcPepMessage          ("PepMessage",          NC_CHAR,    true, QList<int>() << SD_Any,
    "PEP MESS", "PEP Utility Message Definition"),
  fcWobbleControl       ("WobbleControl",       NC_CHAR,    true, QList<int>() << SD_Any,
    "WOB CFLG", "Wobble flow control mess def."),
  fcUT1Control          ("UT1Control",          NC_CHAR,    true, QList<int>() << SD_Any,
    "UT1 CFLG", "UT1 control flag message def."),
  fcOceanStationsFlag   ("OceanStationsFlag",   NC_CHAR,    true, QList<int>() << SD_NumStn << 4,
    "OCE STAT", "Ocean loading station status."),
  fcRelativityData      ("RelativityMessage",   NC_DOUBLE,  true, QList<int>() << 1,
    "REL DATA", "Relativity mod data (gamma)."),
  fcPrecessionData      ("PrecessionData",      NC_DOUBLE,  true, QList<int>() << 1,
    "PRE DATA", "Precession constant (asec/cent)."),
  fcEarthTideData       ("EarthTideData",       NC_DOUBLE,  true, QList<int>() << 3,
    "ETD DATA", "Earth tide module data (la. h l)"),
  fcUT1EPOCH            ("UT1EPOCH",            NC_DOUBLE,  true, QList<int>() << 2 << SD_Any,
    "UT1EPOCH", "TAI - UT1 epoch value definition"),
  fcWOBEPOCH            ("WOBEPOCH",            NC_DOUBLE,  true, QList<int>() << 2 << SD_Any,
    "WOBEPOCH", "Interpolated wobble array def"),
  fcSiteZenithDelay     ("SiteZenithDelay",     NC_DOUBLE,  true, QList<int>() << SD_NumStn,
    "SITEZENS", "Site zenith path delays (nsec)."),
  fcOceanPoleTideCoef   ("OceanPoleTideCoef",   NC_DOUBLE,  false,QList<int>() << SD_NumStn << 6,
    "OPTLCOEF", "Ocean Pole Tide loading Coefficients"),
  fcOceanUpPhase        ("OceanUpPhase",        NC_DOUBLE,  true, QList<int>() << SD_NumStn << 11,
    "SITOCPHS", "Vert ocean loading phases (rad).", "radians"),
  fcOceanHorizontalPhase("OceanHorizontalPhase",NC_DOUBLE,  true, QList<int>() << SD_NumStn << 2 << 11,
    "SITHOCPH", "Horz ocean loading phases (rad).", "radians"),
  fcOceanUpAmp          ("OceanUpAmp",          NC_DOUBLE,  true, QList<int>() << SD_NumStn << 11,
    "SITOCAMP", "Vert ocean loading ampltudes (m)", "meter"),
  fcOceanHorizontalAmp  ("OceanHorizontalAmp",  NC_DOUBLE,  true, QList<int>() << SD_NumStn << 2 << 11,
    "SITHOCAM", "Horz ocean loading ampltudes (m)", "meter"),

  //
  fcLeapSecond        ("LeapSecond",        NC_SHORT,   false,QList<int>() << 1,
    "LeapSeco", "Leap seconds", "second"),
  fcTai_Utc           ("TAI- UTC",          NC_DOUBLE,  false,QList<int>() << 3                   ),
  //
  // Head:
  fcCorrelatorType    ("CorrelatorType",    NC_CHAR,    true, QList<int>() << SD_Any),
  fcCorrelator        ("Correlator",        NC_CHAR,    true, QList<int>() << SD_Any,
    "CORPLACE", "Correlator name (from a master file)."),
  fcNumObs            ("NumObs",            NC_INT,     true, QList<int>() << 1,
    "NUM4 OBS", "Number of observations (I*4)"),
  fcNumScan           ("NumScan",           NC_INT,     true, QList<int>() << 1 ),
  fcNumSource         ("NumSource",         NC_SHORT,   false,QList<int>() << 1 ),
  fcNumStation        ("NumStation",        NC_SHORT,   false,QList<int>() << 1 ),
  fcPrincipalInvestigator
                      ("PrincipalInvestigator",
                                            NC_CHAR,    false,QList<int>() << SD_Any),
  fcExpName           ("ExpName",           NC_CHAR,    false,QList<int>() << SD_Any,
    "EXPCODE ", "Experiment name."),
  fcExpSerialNumber   ("ExpSerialNumber",   NC_SHORT,   false,QList<int>() << 1,
    "EXPSERNO", "Experiment Serial Number."),
  fcExpDescription    ("ExpDescription",    NC_CHAR,    false,QList<int>() << SD_Any,
    "EXPDESC ", "Experiment description."),
  fcRecordingMode    ("RecordingMode",      NC_CHAR,    false,QList<int>() << SD_Any,
    "RECMODE ", "Recoding mode"),
  fcStationList       ("StationList",       NC_CHAR,    true, QList<int>() << SD_NumStn << 8 ),
  fcSourceList        ("SourceList",        NC_CHAR,    true, QList<int>() << SD_NumSrc << 8 ),
  fciUTCInterval      ("iUTCInterval",      NC_SHORT,   true, QList<int>() << 2 << 5,
    "INTRVAL4", "First and last UTC time tag in input file."),
  //
  //
  fcCableSign         ("CableSign",         NC_CHAR,    false,QList<int>() << SD_Any,
    "CBL SIGN", "Sign applied to cable calibration"),
  fcCableStnName      ("CableStationList",  NC_CHAR,    true, QList<int>() << SD_Any << 8,
    "CBL STAT", "Stations for cable sign"),

  fcFourFitCmdCString_v1002
                      ("FourFitCmdCString", NC_CHAR,    false,QList<int>() << SD_NumObs << SD_Any,
    "FOURF CS", "Command string used for fourfit."),
  fcNumLagsUsed_v1002 ("NumLagsUsed",       NC_SHORT,   false,QList<int>() << SD_NumObs,
    "NLAGS   ", "Num of lags used for correlation"),
  fcFourfitControlFile_v1002
                      ("FourfitControlFile",NC_CHAR,    false,QList<int>() << SD_NumObs << SD_Any,
    "FOURF CF", "Control file name for fourfit."),
  fcAplength_v1002    ("Aplength",          NC_DOUBLE,  false,QList<int>() << SD_NumObs,
    "APLENGTH", "Length of accumul. period in sec", "second"),

  fcFourFitCmdCString_v1004
                      ("FourFitCmdCString", NC_CHAR,    false,QList<int>() << SD_Any,
    "FOURF CS", "Command string used for fourfit."),
  fcNumLagsUsed_v1004 ("NumLagsUsed",       NC_SHORT,   false,QList<int>() << 1,
    "NLAGS   ", "Num of lags used for correlation"),
  fcFourfitControlFile_v1004
                      ("FourfitControlFile",NC_CHAR,    false,QList<int>() << SD_Any,
    "FOURF CF", "Control file name for fourfit."),
  fcAplength_v1004    ("Aplength",          NC_DOUBLE,  false,QList<int>() << 1,
    "APLENGTH", "Length of accumul. period in sec", "second"),
  fcCorrPostProcSoftware("CorrPpSoftware",  NC_INT,     false,QList<int>() << 1,
    "", "Correlator post processing software: 0 - HOPS, 1 - PIMA", ""),



  fcBracket("N/A", NC_SHORT,  false, QList<int>() << 1,  "N/A", "N/A")
  ;


  

// netCDF files:
QList<SgVgosDb::FmtChkVar*>
  fcfSourceApriori= QList<SgVgosDb::FmtChkVar*>() << &fcSourceNameApriori << &fcSource2000RaDec
                                                  << &fcSourceReference,
  fcfClockApriori = QList<SgVgosDb::FmtChkVar*>() << &fcClockAprioriSite << &fcClockAprioriOffset
                                                  << &fcClockAprioriRate,
  fcfStationApriori
                  = QList<SgVgosDb::FmtChkVar*>() << &fcStationNameApriori << &fcStationXYZ 
                                                  << &fcStationPlateName,
  fcfAntennaApriori
                  = QList<SgVgosDb::FmtChkVar*>() << &fcAxisType << &fcAntennaName << &fcAxisOffset 
                                                  << &fcAxisTilt,
  fcfCalcEop      = QList<SgVgosDb::FmtChkVar*>() << &fcWobArrayInfo << &fcUT1ArrayInfo << &fcUT1Values
                                                  << &fcWobValues << &fcUT1IntrpMode 
                                                  << &fcCalcUt1Module << &fcUT1Origin 
                                                  << &fcCalcWobModule << &fcWobIntrpMode 
                                                  << &fcWobbleOrigin,
  fcfCalcInfo     = QList<SgVgosDb::FmtChkVar*>() << &fcTidalUt1Control << &fcCalcVersion 
                                                  << &fcCalcControlNames << &fcCalcControlValues 
                                                  << &fcATMMessage << &fcATMControl 
                                                  << &fcAxisOffsetMessage << &fcAxisOffsetControl
                                                  << &fcEarthTideMessage << &fcEarthTideControl 
                                                  << &fcPoleTideMessage << &fcPoleTideControl
                                                  << &fcNutationMessage << &fcNutationControl
                                                  << &fcOceanMessage << &fcOceanControl
                                                  << &fcATIMessage << &fcATIControl
                                                  << &fcCTIMessage << &fcCTIControl
                                                  << &fcParallaxMessage << &fcParallaxControl
                                                  << &fcStarMessage << &fcStarControl
                                                  << &fcTheoryMessage << &fcRelativityControl
                                                  << &fcSiteMessage << &fcFeedhornMessage 
                                                  << &fcPepMessage << &fcWobbleControl << &fcUT1Control
                                                  << &fcOceanStationsFlag 
                                                  << &fcRelativityData << &fcPrecessionData 
                                                  << &fcEarthTideData
                                                  << &fcUT1EPOCH << &fcWOBEPOCH << &fcSiteZenithDelay
                                                  << &fcOceanPoleTideCoef << &fcOceanUpPhase 
                                                  << &fcOceanHorizontalPhase << &fcOceanUpAmp 
                                                  << &fcOceanHorizontalAmp

  ,
  fcfLeapSecond   = QList<SgVgosDb::FmtChkVar*>() << &fcLeapSecond,
  fcfLeapSecondIn = QList<SgVgosDb::FmtChkVar*>() << &fcTai_Utc << &fcLeapSecond,
  fcfHead         = QList<SgVgosDb::FmtChkVar*>() << &fcNumObs << &fcNumScan << &fcNumSource
                                                  << &fcNumStation << &fcCorrelatorType 
                                                  << &fcCorrelator << &fcPrincipalInvestigator
                                                  << &fcExpName << &fcExpSerialNumber 
                                                  << &fcExpDescription << &fcRecordingMode 
                                                  << &fciUTCInterval << &fcStationList << &fcSourceList
                                                  << &fcCorrPostProcSoftware,
  fcfMiscCable    = QList<SgVgosDb::FmtChkVar*>() << &fcCableSign << &fcCableStnName,
  fcfMiscFourFit_v1002
                  = QList<SgVgosDb::FmtChkVar*>() << &fcFourFitCmdCString_v1002 << &fcNumLagsUsed_v1002
                                                  << &fcFourfitControlFile_v1002 << &fcAplength_v1002
                                                  ,
  fcfMiscFourFit_v1004
                  = QList<SgVgosDb::FmtChkVar*>() << &fcFourFitCmdCString_v1004 << &fcNumLagsUsed_v1004
                                                  << &fcFourfitControlFile_v1004 << &fcAplength_v1004
                                                  ,
 
  
  
  fcfBracket      = QList<SgVgosDb::FmtChkVar*>() << &fcBracket
  ;



/*=====================================================================================================*/

