/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>


#include <SgNetworkStnRecord.h>
#include <SgLogger.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgNetworkStnRecord::className()
{
  return "SgNetworkStnRecord";
};


SgNetworkStnRecord::SgNetworkStnRecord(const SgNetworkStnRecord& r)
{
  isValid_ = r.isValid_;
  name_ = r.name_;
  code_ = r.code_;
  domesId_ = r.domesId_;
  cdpId_ = r.cdpId_;
  description_ = r.description_;
};



//
SgNetworkStnRecord& SgNetworkStnRecord::operator=(const SgNetworkStnRecord& r)
{
  isValid_ = r.isValid_;
  name_ = r.name_;
  code_ = r.code_;
  domesId_ = r.domesId_;
  cdpId_ = r.cdpId_;
  description_ = r.description_;
  return *this;
};



//
bool SgNetworkStnRecord::parseString(const QString& str)
{
//          1         2         3         4         5
//012345678901234567890123456789012345678901234567890123456789
// Dm DEADMANL 40431M001 7267 mobile Deadman Lake, CA, USA
  code_ = str.mid(1, 2);
  name_ = str.mid(4, 8).simplified();
  name_.replace('_', ' ');
  name_ = name_.leftJustified(8, ' ');
  domesId_ = str.mid(13, 9);
  cdpId_ = str.mid(23, 4);
  description_ = str.mid(28);
  isValid_ = (name_ != QString("--------"));
  return isValid_;
};
/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgNetworkStations::className()
{
  return "SgNetworkStations";
};



//
bool SgNetworkStations::readFile()
{
  QString                       fName(path2file_ + "/" + fileName_);
  QString                       str;
  QFile                         f(fName);
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::readFile(): the ns-codes file [" + fName + 
      "] does not exist; ns-code records are unavailable");
    return (isOk_=false);
  };
  //
  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    SgNetworkStnRecord          rec;
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.size()>29 && str.at(0)==' ' && rec.parseString(str))
      {
        if (recByName_.contains(rec.getName()))
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::readFile(): got a duplicate record \"" + str + 
            "\" for the station name \"" + rec.getName() + "\" in the ns-codes file [" + fName + "]");
        else
          recByName_.insert(rec.getName(), rec);
        //
        if (recById_.contains(rec.getCode()))
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::readFile(): got a duplicate record \"" + str + 
            "\" for the station code \"" + rec.getCode() + "\" in the ns-codes file [" + fName + "]");
        else
          recById_.insert(rec.getCode(), rec);
      };
    };
    f.close();
    s.setDevice(NULL);
    isOk_ = (recByName_.size()>0);
  }
  else
    isOk_ = false;
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    "::readFile(): extracted " + QString("").setNum(recByName_.size()) + 
    " records from the ns-codes file [" + fName + "]");

  return isOk_;
};



//
const SgNetworkStnRecord& SgNetworkStations::lookUp(const QString& key)
{
  return recByName_.find(key).value();
};
/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
//

/*=====================================================================================================*/
