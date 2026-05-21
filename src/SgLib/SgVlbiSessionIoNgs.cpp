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

#include <QtCore/QFile>
#include <QtCore/QTextStream>


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgLogger.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>


/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/




//
bool SgVlbiSession::exportDataIntoNgsFile(const QString& dirName)
{
  SgMJD                         startEpoch(SgMJD::currentMJD());
  static const QString mntTypeNames[] = 
  {
    "AZEL",
    "EQUA",
    "X-YN",
    "X-YE",
    "RCHM",
    "UNKN"
    };
  QString                 fileName(getName());
  QString                 str(fileName);

  fileName += QString().sprintf("_N%03d", primaryBand_->getInputFileVersion() + 1);
  
  QFile           f(dirName + "/" + fileName); 
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
      ": error opening output file: " + dirName + "/" + fileName);
    return false;
  };
  QTextStream             ts(&f);
  SgVlbiStationInfo      *sti;
//SgVlbiStationInfo      *st1i, *st2i;
  SgVlbiSourceInfo       *sri;
  
  // start output operations:
  // header:
  str += QString().sprintf("_V%03d", primaryBand_->getInputFileVersion() + 1);
  //  DATA IN NGS FORMAT FROM DATABASE 12JAN26XE_V004
  ts << "DATA IN NGS FORMAT FROM DATABASE " << str << "\r\n";
  //Observed delays and rates in card #2, modified errors in card #9
  ts << "Observed delays and rates in card #2, modified errors in card #9\r\n";
  
  // station block:
  StationsByName_it     itSt;
  itSt = stationsByName_.begin();
  for (itSt=stationsByName_.begin(); itSt!=stationsByName_.end(); ++itSt)
  {
    sti = itSt.value();
    //YEBES40M    4848762.10000  -261484.50000  4123084.90000 AZEL   2.06500
    str.sprintf("%-8s   %14.5f %14.5f %14.5f %4s %9.5f",
      qPrintable(sti->getKey()),
      sti->getR().at(X_AXIS),
      sti->getR().at(Y_AXIS),
      sti->getR().at(Z_AXIS),
      qPrintable(mntTypeNames[sti->getMntType()]),
      sti->getAxisOffset() );
    ts << str << "\r\n";
  };
  ts << "$END\r\n";
  // sources block:
  SourcesByName_it     itSr;
  itSr = sourcesByName_.begin();
  for (itSr=sourcesByName_.begin(); itSr!=sourcesByName_.end(); ++itSr)
  {
    sri = itSr.value();
    char      sgn;
    int       ra_hr, ra_min, de_deg, de_min;
    double    ra_sec, de_sec;
    ra_sec = sri->getRA();
    de_sec = sri->getDN();
    sgn = ' ';
    if (de_sec<0.0)
    {
      sgn = '-';
      de_sec = - de_sec;
    };
    ra_sec*= 43200.0/M_PI;
    ra_hr  = trunc(ra_sec/60.0/60.0);
    ra_sec-= ra_hr*60.0*60.0;
    ra_min = trunc(ra_sec/60.0);
    ra_sec-= ra_min*60.0;

    de_sec*= 648000.0/M_PI;
    de_deg = trunc(de_sec/60.0/60.0);
    de_sec-= de_deg*60.0*60.0;
    de_min = trunc(de_sec/60.0);
    de_sec-= de_min*60.0;
    //3C446     22 25    47.259295 - 4 57     1.390820
    str.sprintf("%-8s  %2d %2d    %9.6f %c%2d %2d    %9.6f",
      qPrintable(sri->getKey()),
      ra_hr, ra_min, ra_sec,
      sgn,
      de_deg, de_min, de_sec );    
    ts << str << "\r\n";
  };
  ts << "$END\r\n";
  // aux parameters:
  //  .8212990000000D+04           GR PH
  str.sprintf("%20.13E           GR PH",
    primaryBand_->getFrequency());
  ts << str << "\r\n$END\r\n";

  // observations:
  QString             strQual, strIonoQual;
  int                 yr, month, day, hr, min;
  double              sec;
  double              effectiveGDSigma, effectivePRSigma, sigmaGD2add, sigmaPR2add;
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation  *obs = observations_.at(i);
    SgVlbiObservable   *o = obs->primeObs();
    if (o)
    {
      SgMJD::MJD_reverse(obs->getDate(), obs->getTime(), yr, month, day, hr, min, sec);
//    st1i = obs->stn_1();
//    st2i = obs->stn_2();
      strQual.sprintf("%2d", o->grDelay().getUnweightFlag());
      if (o->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && o->grDelay().getUnweightFlag()==0)
        strQual = " 1";
      strIonoQual = " 0";
      if (obs->isAttr(SgVlbiObservation::Attr_NOT_MATCHED))
        strIonoQual = "-1";
    
      sigmaGD2add = obs->baseline()->getSigma2add(DT_DELAY);
      sigmaPR2add = 0.0;
      effectiveGDSigma = sqrt(
        o->grDelay().getSigma()*o->grDelay().getSigma() +
        o->grDelay().getIonoSigma()*o->grDelay().getIonoSigma() +
        sigmaGD2add*sigmaGD2add);
    
      effectivePRSigma = sqrt(
        o->phDRate().getSigma()*o->phDRate().getSigma() +
        o->phDRate().getIonoSigma()*o->phDRate().getIonoSigma() + 
        sigmaPR2add);
    
      // first record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //YEBES40M  ZELENCHK  1803+784 2012 01 26 18 30  30.0000000000                 101
      str.sprintf("%-8s  %-8s  %-8s %04d %02d %02d %02d %02d %14.10f          %8d01",
        qPrintable(o->stn_1()->getKey()), qPrintable(o->stn_2()->getKey()),
        qPrintable(o->src()->getKey()),
        yr, month, day, hr, min, sec, 
        i+1);
      ts << str << "\r\n";
    
      // second record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //     311082.58351570    .00174  -160030.5984109248    .00382 0      I        102
      str.sprintf("%20.8f%10.5f%20.10f%10.5f%2s      I %8d02",
        (o->grDelay().getValue() + o->grDelay().ambiguity())*1.0e9,
        o->grDelay().getSigma()*1.0e9,
        o->phDRate().getValue()*1.0e12, o->phDRate().getSigma()*1.0e12,
        qPrintable(strQual),
        i+1);
      ts << str << "\r\n";

      // third record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //    .00612    .00000    .00000    .00000  -1.665895811950809        0.       103
      str.sprintf("%10.5f    .00000    .00000    .00000 %19.15f        0.%8d03",
        o->getCorrCoeff(),
        o->getTotalPhase(),
        i+1);
      ts << str << "\r\n";

      // fourth record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //       .00   .0       .00   .0       .00   .0       .00   .0                 104
      str.sprintf("       .00   .0       .00   .0       .00   .0       .00   .0          %8d04",
        i+1);
      ts << str << "\r\n";

      // fifth record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //    .00343   -.01793    .00000    .00000    .00000    .00000                 105
      str.sprintf("%10.5f%10.5f    .00000    .00000    .00000    .00000          %8d05",
        obs->auxObs_1()->getCableCalibration()*1.0e9,
        obs->auxObs_2()->getCableCalibration()*1.0e9,
// ???
//        obs->auxObs_1()->getCableCalibration()*st1i->getCableCalMultiplier()*1.0e9,
//        obs->auxObs_2()->getCableCalibration()*st2i->getCableCalMultiplier()*1.0e9,
        i+1);
      ts << str << "\r\n";

      // sixth record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //     4.085    -8.990   911.093   882.400    78.146    74.630 0 0             106
      str.sprintf("%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f 0 0      %8d06",
        obs->auxObs_1()->getMeteoData().getTemperature(),
        obs->auxObs_2()->getMeteoData().getTemperature(),
        obs->auxObs_1()->getMeteoData().getPressure(),
        obs->auxObs_2()->getMeteoData().getPressure(),
        obs->auxObs_1()->getMeteoData().getRelativeHumidity()*100.0,
        obs->auxObs_2()->getMeteoData().getRelativeHumidity()*100.0,
        i+1);
      ts << str << "\r\n";

      // eigth record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //        1.2235335897    .00321        -.0311519821    .00530  0              108
      str.sprintf("%20.10f%10.5f%20.10f%10.5f %2s       %8d08",
        o->grDelay().getIonoValue()*1.0e9,
        o->grDelay().getIonoSigma()*1.0e9,
        o->phDRate().getIonoValue()*1.0e12,
        o->phDRate().getIonoSigma()*1.0e12,
        qPrintable(strIonoQual),
        i+1);
      ts << str << "\r\n";

      // ninth record:
      //          1         2         3         4         5         6         7        
      //01234567890123456789012345678901234567890123456789012345678901234567890123456789
      //     311082.58351570    .05262  -160030.5984109248    .10084 0      I        109
      str.sprintf("%20.8f%10.5f%20.10f%10.5f%2s      I %8d09",
        (o->grDelay().getValue() + o->grDelay().ambiguity())*1.0e9,
        effectiveGDSigma*1.0e9,
        o->phDRate().getValue()*1.0e12, 
        effectivePRSigma*1.0e12,
        qPrintable(strQual),
        i+1);
      ts << str << "\r\n";
    };
  };

  ts.setDevice(NULL);
  f.close();


  SgMJD                         finisEpoch(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::exportDataIntoNgsFile(): data have been exported into the NGS cards format file \"" + fileName +
    "\", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");

  return true;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/

/*=====================================================================================================*/
