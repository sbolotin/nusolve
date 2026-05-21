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
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <SgModelEop_JMG_96_hf.h>

#include <SgLogger.h>
#include <SgMatrix.h>
#include <SgVector.h>



/*=======================================================================================================
*
*                          SgModelEop_JMG_96_hf's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgModelEop_JMG_96_hf::className()
{
  return "SgModelEop_JMG_96_hf";
};



//
bool SgModelEop_JMG_96_hf::readFile(const QString& fileName)
{
  QString             str;
  QFile               f((fileName_=fileName));
  bool                isOk;
  isOk_ = false;
  if (!f.exists())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      ": the file [" + fileName + "] with a priori data does not exist");
    return false;
  };
  //
  // clear the stuff:
  numUt_ = 0;
  numPm_ = 0;
  if (baseModel_)
  {
    delete baseModel_;
    baseModel_ = NULL;
  };
  utModel_ = NULL;
  pmModel_ = NULL;
  //
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    int               idx=0;
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.at(0)==' ')
      {
        if (numUt_*numPm_ == 0) // first non-comments string, determine number of arrays:
        {
          QStringList l = str.simplified().split(' ', QString::SkipEmptyParts);
          if (l.size() != 2)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: cannot guess numbers of arrays from the string: [" +
              str + "]");
            return false;
          };
          numUt_ = l.at(0).toInt(&isOk);
          if (!isOk)
          {
            numUt_ = 0;
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: failed to get the array size for UT1, the string: [" +
              str + "]");
            return false;
          };
          numPm_ = l.at(1).toInt(&isOk);
          if (!isOk)
          {
            numUt_ = 0;
            numPm_ = 0;
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: failed to get the array size for PM, the string: [" +
              str + "]");
            return false;
          };
          // arrange some space for the data:
          baseModel_ = new HfEopRec[numUt_ + numPm_];
          utModel_ = baseModel_;
          pmModel_ = baseModel_ + numUt_;
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            ": the file [" + fileName + "]: allocated " + QString("").setNum(numUt_) + 
            " records for UT1 and " + QString("").setNum(numPm_) + " records for PM");
        }
        else
        {
          int           n[6];
          double        a_c, a_s;
          QStringList l = str.simplified().split(' ', QString::SkipEmptyParts);
          if (l.size() != 10)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: cannot guess format of the string: [" +
              str + "]");
            return false;
          };
          for (int i=0; i<6; i++)
          {
            n[i] = l.at(i).toInt(&isOk);
            if (!isOk)
            {
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                ": the file [" + fileName + "]: failed to acquire info (n_" + QString("").setNum(i) +
                "), the string: [" + str + "]");
              return false;
            };
          };
          a_c = l.at(6).toDouble(&isOk);
          if (!isOk)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: failed to acquire info (a_c), the string: [" + str + "]");
            return false;
          };
          a_s = l.at(7).toDouble(&isOk);
          if (!isOk)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": the file [" + fileName + "]: failed to acquire info (a_s), the string: [" + str + "]");
            return false;
          };
          for (int i=0; i<6; i++)
            baseModel_[idx].n_[i] = n[i];
          baseModel_[idx].a_cos_ = a_c;
          baseModel_[idx].a_sin_ = a_s;
          idx++;
        };
      };
    };
    f.close();
    s.setDevice(NULL);
  };

  if (0<numUt_ && 0<numPm_)
    isOk_ = true;

  return isOk_;
};



//
// The model is from:
// Gipson, John M. (1996) Very long baseline interferometry determination of neglected tidal terms 
// in high-frequency Earth orientation variation. Journal of Geophysical Research: Solid Earth, 101.
// 28051-28064 doi:10.1029/96jb02292
//
// note: input t is in TT timescale
void SgModelEop_JMG_96_hf::calcCorrections(const SgMJD& t, double& dUt1, double& dPx, double& dPy)
{
  double                        fundArgs[6];
  double                        arg, sarg, carg;
  //
  // zerofy values:
  fundArgs[0] = fundArgs[1] = fundArgs[2] = fundArgs[3] = fundArgs[4] = fundArgs[5] = 0.0;
  dUt1 = dPx = dPy = 0.0;
  
  if (!isOk_)
    return;

  calcNutationFundArgs_IersConv2003(t, fundArgs);
  fundArgs[5] = t.gmst() + M_PI;
  if (2.0*M_PI < fundArgs[5])
    fundArgs[5] -= 2.0*M_PI;
  
  // Eq. (2) from the paper:
  // dUT1:
  for (int i=0; i<numUt_; i++)
  {
    arg = 0.0;
    for (int j=0; j<6; j++)
      arg += utModel_[i].n_[j]*fundArgs[j];
    sincos(arg, &sarg, &carg);
    dUt1 += utModel_[i].a_cos_*carg + utModel_[i].a_sin_*sarg;
  };

  // Eq. (2) from the paper:
  // polar motion:
  for (int i=0; i<numPm_; i++)
  {
    arg = 0.0;
    for (int j=0; j<6; j++)
      arg += pmModel_[i].n_[j]*fundArgs[j];
    sincos(arg, &sarg, &carg);
    dPx += -pmModel_[i].a_cos_*carg + pmModel_[i].a_sin_*sarg;
    dPy +=  pmModel_[i].a_cos_*sarg + pmModel_[i].a_sin_*carg;
  };
};
/*=====================================================================================================*/





/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/








/*=====================================================================================================*/
