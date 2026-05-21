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
/* SgVgosDb implementation (continue -- loadScan part of vgosDb data tree)                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
bool SgVgosDb::loadEpochs4Scans(QList<SgMJD>& epochs)
{
  if (vScanTimeUTC_.isEmpty())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Scans(): the vgosDb variable ScanTimeUTC is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vScanTimeUTC_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfTimeUTC, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Scans(): format check failed");
    return false;
  };
  const double                 *pSeconds=ncdf.lookupVar(fcSecond.name())->data2double();
  const short                  *pYMDHM  =ncdf.lookupVar(fcYmdhm .name())->data2short();
  epochs.clear();
  numOfScans_ = ncdf.lookupVar(fcSecond.name())->dimensions().at(0)->getN();
  for (int i=0; i<numOfScans_; i++)
    epochs.append( SgMJD(pYMDHM[5*i  ], pYMDHM[5*i+1], pYMDHM[5*i+2], pYMDHM[5*i+3], 
                                        pYMDHM[5*i+4], pSeconds[i]));
  if (epochs.size())
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadEpochs4Scans(): read " + QString("").setNum(epochs.size()) + 
      " scan epochs from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanName(QVector<QString>& scanNames, QVector<QString>& scanFullNames)
{
  if (vScanName_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): the vgosDb variable ScanName is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vScanName_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfScanNameInput, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): format check failed");
    return false;
  };
  // some sessions do not have scan names
  const char                   *pN=NULL, *pF=NULL;
  int                           lN=0, lF=0;
  //
  if (ncdf.lookupVar(fcScanNameInput.name()))
  {
    pN = ncdf.lookupVar(fcScanNameInput.name())->data2char();
    lN = ncdf.lookupVar(fcScanNameInput.name())->dimensions().at(1)->getN();
    scanNames.resize(numOfScans_);
  };
  if (ncdf.lookupVar(fcScanNameFull.name()))
  {
    pF = ncdf.lookupVar(fcScanNameFull.name())->data2char();
    lF = ncdf.lookupVar(fcScanNameFull.name())->dimensions().at(1)->getN();
    scanFullNames.resize(numOfScans_);
  };
  //
  for (int i=0; i<numOfScans_; i++)
  {
    if (pN)
      scanNames    [i] = QString::fromLatin1(pN + lN*i, lN);
    if (pF)
      scanFullNames[i] = QString::fromLatin1(pF + lF*i, lF);
  };
  if (pN && pF)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): data successfully loaded from " + ncdf.getFileName());
  else if (pN)
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): only scan names are available in " + ncdf.getFileName());
  else if (pF)
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): only scan full names are available in " + ncdf.getFileName());
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
      "::loadScanName(): nothing to load from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanEphemeris(SgMatrix*& rSun, SgMatrix*& rMoon, SgMatrix*& rEarth,
    SgMatrix*& vSun, SgMatrix*& vMoon, SgMatrix*& vEarth, SgMatrix*& aEarth)
{
  if (vEphemeris_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanEphemeris(): the vgosDb variable Ephemeris is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vEphemeris_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfEphemeris, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanEphemeris(): format check failed");
    return false;
  };
  const double                 *pS=ncdf.lookupVar(fcSunXyz.name())->data2double();
  const double                 *pM=ncdf.lookupVar(fcMoonXyz.name())->data2double();
  const double                 *pE=ncdf.lookupVar(fcEarthXyz.name())->data2double();

  rSun   = new SgMatrix(numOfScans_, 3);
  rMoon  = new SgMatrix(numOfScans_, 3);
  rEarth = new SgMatrix(numOfScans_, 3);
  vSun   = new SgMatrix(numOfScans_, 3);
  vMoon  = new SgMatrix(numOfScans_, 3);
  vEarth = new SgMatrix(numOfScans_, 3);
  aEarth = new SgMatrix(numOfScans_, 3);

  for (int i=0; i<numOfScans_; i++)
  {
    rSun->setElement(i, 0, pS[6*i    ]);
    rSun->setElement(i, 1, pS[6*i + 1]);
    rSun->setElement(i, 2, pS[6*i + 2]);
    vSun->setElement(i, 0, pS[6*i + 3]);
    vSun->setElement(i, 1, pS[6*i + 4]);
    vSun->setElement(i, 2, pS[6*i + 5]);

    rMoon->setElement(i, 0, pM[6*i    ]);
    rMoon->setElement(i, 1, pM[6*i + 1]);
    rMoon->setElement(i, 2, pM[6*i + 2]);
    vMoon->setElement(i, 0, pM[6*i + 3]);
    vMoon->setElement(i, 1, pM[6*i + 4]);
    vMoon->setElement(i, 2, pM[6*i + 5]);

    rEarth->setElement(i, 0, pE[9*i    ]);
    rEarth->setElement(i, 1, pE[9*i + 1]);
    rEarth->setElement(i, 2, pE[9*i + 2]);
    vEarth->setElement(i, 0, pE[9*i + 3]);
    vEarth->setElement(i, 1, pE[9*i + 4]);
    vEarth->setElement(i, 2, pE[9*i + 5]);
    aEarth->setElement(i, 0, pE[9*i + 6]);
    aEarth->setElement(i, 1, pE[9*i + 7]);
    aEarth->setElement(i, 2, pE[9*i + 8]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanEphemeris(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanNutationEqxWahr(SgMatrix*& pPsiEps, SgMatrix*& pPsiEps_rates)
{
  if (vNutationEqx_kWahr_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationEqxWahr(): the vgosDb variable NutationEqx_kWahr is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vNutationEqx_kWahr_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfNutationEqx, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationEqxWahr(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcNutationEqx.name())->data2double();
  pPsiEps         = new SgMatrix(numOfScans_, 2); // vals : dPsi and dEps
  pPsiEps_rates   = new SgMatrix(numOfScans_, 2); // rates: dPsi and dEps
  for (int i=0; i<numOfScans_; i++)
  {
    pPsiEps->setElement(i,0,  p[2*2*i    ]);
    pPsiEps->setElement(i,1,  p[2*2*i + 1]);

    pPsiEps_rates->setElement(i,0,  p[2*2*i + 2]);
    pPsiEps_rates->setElement(i,1,  p[2*2*i + 3]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanNutationEqxWahr(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanNutationEqx(SgMatrix*& pPsiEps, SgMatrix*& pPsiEps_rates, QString& kind)
{
  kind = "";
  if (vNutationEqx_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationEqx(): the vgosDb variable NutationEqx is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vNutationEqx_.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfNutationEqx, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationEqx(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcNutationEqx.name())->data2double();
  pPsiEps         = new SgMatrix(numOfScans_, 2); // vals : dPsi and dEps
  pPsiEps_rates   = new SgMatrix(numOfScans_, 2); // rates: dPsi and dEps
  for (int i=0; i<numOfScans_; i++)
  {
    pPsiEps->setElement(i,0,  p[2*2*i    ]);
    pPsiEps->setElement(i,1,  p[2*2*i + 1]);

    pPsiEps_rates->setElement(i,0,  p[2*2*i + 2]);
    pPsiEps_rates->setElement(i,1,  p[2*2*i + 3]);
  };
  kind = vNutationEqx_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanNutationEqx(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanNutationNro(SgMatrix*& pXys, SgMatrix*& pXys_rates, QString& kind)
{
  kind = "";
  if (vNutationNro_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationNRO(): the vgosDb variable NutationNro is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vNutationNro_.getFileName());
  ncdf.getData();
  
  if (!checkFormat(fcfNutationNro, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanNutationNRO(): format check failed");
    return false;
  };
  const double                 *p=ncdf.lookupVar(fcNutationNro.name())->data2double();
  pXys        = new SgMatrix(numOfScans_, 3); // vals : Cip_X, Cip_Y, Cip_S
  pXys_rates  = new SgMatrix(numOfScans_, 3); // rates: Cip_X, Cip_Y, Cip_S
  for (int i=0; i<numOfScans_; i++)
  {
    pXys->setElement(i,0,  p[2*3*i    ]);
    pXys->setElement(i,1,  p[2*3*i + 1]);
    pXys->setElement(i,2,  p[2*3*i + 2]);

    pXys_rates->setElement(i,0,  p[2*3*i + 3]);
    pXys_rates->setElement(i,1,  p[2*3*i + 4]);
    pXys_rates->setElement(i,2,  p[2*3*i + 5]);
  };
  kind = vNutationNro_.getKind();
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanNutationNRO(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanCrootFname(QVector<QString>& corrRootFnames)
{
  SgVdbVariable                &var=vCorrRootFile_;
  if (var.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanCrootFname(): the vgosDb variable CorrRootFile is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + var.getFileName());
  ncdf.getData();
  if (!checkFormat(fcfCorrRootFile, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanCrootFname(): format check failed");
    return false;
  };
  if (!ncdf.lookupVar(fcCROOTFIL.name()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanCrootFname(): cannot find the variable " + fcCROOTFIL.name() + " in the file");
    return false;
  };
  const char                   *pN=ncdf.lookupVar(fcCROOTFIL.name())->data2char();
  int                           lN=ncdf.lookupVar(fcCROOTFIL.name())->dimensions().at(1)->getN();
  corrRootFnames.resize(numOfScans_);
  for (int i=0; i<numOfScans_; i++)
    corrRootFnames[i] = QString::fromLatin1(pN + lN*i, lN);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanCrootFname(): data successfully loaded from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadScanTrf2crf(SgMatrix*& val, SgMatrix*& rat, SgMatrix*& acc)
{
  if (vRot_CF2J2K_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadScanTrf2crf(): the vgosDb variable Rot_CF2J2K is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vRot_CF2J2K_.getFileName());
  ncdf.getData();
  
  if (!checkFormat(fcfRotCf2J2K, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadScanTrf2crf(): format check failed");
    return false;
  };
  double                       *p=ncdf.lookupVar(fcRotCf2J2K.name())->data2double();
  val = new SgMatrix(numOfScans_, 9);
  rat = new SgMatrix(numOfScans_, 9);
  acc = new SgMatrix(numOfScans_, 9);
  for (int i=0; i<numOfScans_; i++)
    for (int j=0; j<9; j++)
    {
      val->setElement(i, j, p[27*i     + j ]);
      rat->setElement(i, j, p[27*i + 9 + j ]);
      acc->setElement(i, j, p[27*i +18 + j ]);
    };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadScanTrf2crf(): data loaded successfully from " + ncdf.getFileName());
  return true;
};



//
bool SgVgosDb::loadErpApriori(SgVector*& ut1_tai, SgMatrix*& pm)
{
  if (vErpApriori_.isEmpty())
  {
    logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
      "::loadErpApriori(): the vgosDb variable ErpApriori is empty");
    return false;
  };
  SgNetCdf                      ncdf(path2RootDir_ + "/" + vErpApriori_.getFileName());
  ncdf.getData();
  
  if (!checkFormat(fcfErpApriori, ncdf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::loadErpApriori(): format check failed");
    return false;
  };
  double                       *pU=ncdf.lookupVar(fcUt1_Tai.name())->data2double();
  double                       *pP=ncdf.lookupVar(fcPolarMotion.name())->data2double();

  ut1_tai = new SgVector(numOfScans_);
  pm      = new SgMatrix(numOfScans_, 2);
  for (int i=0; i<numOfScans_; i++)
  {
    ut1_tai->setElement(i, pU[i]);
    pm->setElement(i, 0, pP[2*i    ]);
    pm->setElement(i, 1, pP[2*i + 1]);
  };
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::loadErpApriori(): data loaded successfully from " + ncdf.getFileName());
  return true;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// Constants:
//

// const char* name, nc_type type, bool isMandatory, const QList<int> dims, 
//      const QString& attLCode="", const QString& attDefinition="", 
//      const QString& attUnits="", const QString& attBand=""

// Variable descriptions:
SgVgosDb::FmtChkVar
  fcScanName          ("ScanName",          NC_CHAR,    false,QList<int>() << SD_NumScans << 10     ),
  fcScanNameInput     ("ScanName",          NC_CHAR,    false,QList<int>() << SD_NumScans << SD_Any ),
  fcScanNameFull      ("ScanNameFull",      NC_CHAR,    false,QList<int>() << SD_NumScans << 30     ),
  fcNutationEqxWahr   ("NutationEQX",       NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2 << 2,
    "NUT WAHR", "Wahr nut vals  - Dpsi Deps&rates"),
  fcNutationEqx       ("NutationEQX",       NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2 << 2,
    "NUT2006A", "IAU2006A Nut. - Dpsi  Deps  Rates"),
  fcNutationNro       ("NutationNRO",       NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2 << 3,
    "NUT06XYS", "2000/2006 Nut/Prec X,Y, S & Rates"),
  fcCROOTFIL          ("CROOTFIL",          NC_CHAR,    false,QList<int>() << SD_NumScans << 16,
    "CROOTFIL", "Correlator root file name.......", "", "?"),
  fcMjd               ("MJD",               NC_INT,     true, QList<int>() << SD_NumScans,
    "", "Integer part of MJD time tag of scan", "day"),
  fcDayFrac           ("DayFrac",           NC_DOUBLE,  true, QList<int>() << SD_NumScans,
    "", "Fractional day part of time tag of scan", "day"),
  fcSunXyz            ("SunXYZ",            NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2 << 3,
    "SUN DATA", "Solar geocentric coordinates and velocities", "Meter|Meter/sec"),
  fcMoonXyz           ("MoonXYZ",           NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2 << 3,
    "MOONDATA", "Lunar geocentric coordinates and velocities", "Meter|Meter/sec"),
  fcEarthXyz          ("EarthXYZ",          NC_DOUBLE,  true, QList<int>() << SD_NumScans << 3 << 3,
    "EARTH CE", "Earth barycentric coordinates, vels and accs", "Meter|Meter/sec|Meter/sec/sec"),
  fcRotCf2J2K         ("Rot-CF2J2K",        NC_DOUBLE,  true, QList<int>() << SD_NumScans << 3 << 3 << 3,
    "CF2J2K  ", "Crust-fixed to J2000 Rot. Matrix and deriviatives"),
  
  fcUt1_Tai           ("UT1",               NC_DOUBLE,  true, QList<int>() << SD_NumScans,
    "UT1 -TAI", "UT1 time of day for this obsvr.", "second"),
  fcPolarMotion       ("PolarMotion",       NC_DOUBLE,  true, QList<int>() << SD_NumScans << 2,
    "POLAR XY", "Polar motion X & Y for obs (rad)", "radian")
  
  ;





// netCDF files:
QList<SgVgosDb::FmtChkVar*>
  fcfScanName     = QList<SgVgosDb::FmtChkVar*>() << &fcScanName << &fcScanNameFull,
  fcfScanNameInput= QList<SgVgosDb::FmtChkVar*>() << &fcScanNameInput << &fcScanNameFull,
  fcfNutationEqxWahr
                  = QList<SgVgosDb::FmtChkVar*>() << &fcNutationEqxWahr,
  fcfNutationEqx  = QList<SgVgosDb::FmtChkVar*>() << &fcNutationEqx,
  fcfNutationNro  = QList<SgVgosDb::FmtChkVar*>() << &fcNutationNro,
  fcfCorrRootFile = QList<SgVgosDb::FmtChkVar*>() << &fcCROOTFIL,
  fcfScanTimeMjd  = QList<SgVgosDb::FmtChkVar*>() << &fcMjd << &fcDayFrac,
  fcfEphemeris    = QList<SgVgosDb::FmtChkVar*>() << &fcSunXyz << &fcMoonXyz << &fcEarthXyz,
  fcfRotCf2J2K    = QList<SgVgosDb::FmtChkVar*>() << &fcRotCf2J2K,
  fcfErpApriori   = QList<SgVgosDb::FmtChkVar*>() << &fcUt1_Tai << &fcPolarMotion
  
  ;

 


/*=====================================================================================================*/

