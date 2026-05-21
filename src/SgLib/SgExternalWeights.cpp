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
#include <iostream>


#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <SgExternalWeights.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           SgBaselineExternalWeight's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgBaselineExternalWeight::className()
{
  return "SgBaselineExternalWeight";
};



//
bool SgBaselineExternalWeight::parseString(const QString& str)
{
  //          1         2         3         4         5         6         7 
  //01234567890123456789012345678901234567890123456789012345678901234567890123456789
  //12MAY03XE   4 WETTZELL/YEBES40M     45.63    221.45      0.10      0.00

  // ok, we do not know exactly the format specifications, guess, it is a space separated
  // fileds:
  bool        isOk;
  QStringList l = str.mid(9).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 4)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "parseString(): cannot parse external weights string: [" + str + "]");
    return false;
  };

  dbhVersionNumber_ = l.at(0).toInt(&isOk);
  if (!isOk)
  {
    dbhVersionNumber_ = 0;
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "parseString(): cannot get DBH version number from external weights string: [" + str + "]");
    return false;
  };
  
  baselineName_ = l.at(1);
  // adjust notation:
  baselineName_.replace(8, 1, ':');
  baselineName_.replace('_', ' ');
  
  delayWeight_ = l.at(2).toDouble(&isOk);
  if (!isOk)
  {
    delayWeight_ = 0.0;
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "parseString(): cannot get delay weight from external weights string: [" + str + "]");
    return false;
  };
  
  rateWeight_ = l.at(3).toDouble(&isOk);
  if (!isOk)
  {
    rateWeight_ = 0.0;
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "parseString(): cannot get rate weight from external weights string: [" + str + "]");
    return false;
  };
 
  return true;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgExternalWeights's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgExternalWeights::className()
{
  return "SgExternalWeights";
};



//
bool SgExternalWeights::readFile(const QString& fileName)
{
  QString             str;
  QFile               f((fileName_=fileName));
  isOk_ = false;
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readFile(): the file [" + fileName + "] with external weights does not exist");
    return isOk_;
  };
  //
  // clear the stuff:
  for (QMap<QString, SgBaselineExternalWeight*>::iterator it=weights_.begin(); it!=weights_.end(); ++it)
    delete it.value();
  weights_.clear();
  //
  SgBaselineExternalWeight      w;
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.mid(0, 9) == sessionName_)
      {
        if (w.parseString(str))
        {
          // check for duplicate names:
          if (!weights_.contains(w.getBaselineName()))
            weights_.insert(w.getBaselineName(), new SgBaselineExternalWeight(w));
          else if (weights_.value(w.getBaselineName())->getDbhVersionNumber() != w.getDbhVersionNumber())
          {
            // pick up just latest version:
            if (weights_.value(w.getBaselineName())->getDbhVersionNumber() < w.getDbhVersionNumber())
            {
              SgBaselineExternalWeight  *bew=weights_.value(w.getBaselineName());
              bew->setDelayWeight(w.getDelayWeight());
              bew->setRateWeight(w.getRateWeight());
              bew->setDbhVersionNumber(w.getDbhVersionNumber());
            };
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              ": readFile(): got a duplicate record for " + w.getBaselineName() + "'s weight of the" + 
              " session " + sessionName_ + " in the external weights file " + fileName_);
        };
      };
    };
    f.close();
    s.setDevice(NULL);
  };
  if ((isOk_=(weights_.size()!=0)))
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      ": readFile(): " + QString().setNum(weights_.size()) + " weight records have been acquired from" + 
      " the external weightsfile " + fileName_);
  return isOk_;
};



//
void SgExternalWeights::setupExternalWeights(BaselinesByName& baselines)
{
  for (BaselinesByName_it it=baselines.begin(); it!=baselines.end(); ++it)
  {
    SgVlbiBaselineInfo         *bi=it.value();
    if (weights_.contains(bi->getKey()))
    {
      SgBaselineExternalWeight *w=weights_.find(bi->getKey()).value();
      bi->setSigma2add(DT_DELAY, w->getDelayWeight()*1.0e-12);
      bi->setSigma2add(DT_RATE,  w->getRateWeight() *1.0e-15); // fs/s?
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": setupExternalWeights(): cannot find a weight record for the baseline " +
        bi->getKey());
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
