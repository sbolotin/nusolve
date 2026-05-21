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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>


#include <SgMasterRecord.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgMasterRecord::className()
{
  return "SgMasterRecord";
};



//
SgMasterRecord& SgMasterRecord::operator=(const SgMasterRecord& r)
{
  isValid_ = r.isValid_;
  name_ = r.name_;
  type_ = r.type_;
  code_ = r.code_;
  date_ = r.date_;
  stations_ = r.stations_;
  dbcCode_ = r.dbcCode_;
  scheduledBy_ = r.scheduledBy_;
  correlatedBy_ = r.correlatedBy_;
  submittedBy_ = r.submittedBy_;
  suffixes_ = r.suffixes_;
  return *this;
};



//
void SgMasterRecord::setSuffixes(const QStringList& lst)
{
  if (lst.size())
  {
    suffixes_.clear();
    for (int i=0; i<lst.size(); i++)
      suffixes_ << lst.at(i);
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::setSuffixes(): the list of suffixes is empty");
};



//
void SgMasterRecord::addSuffix(const QString& sfx)
{
  if (sfx.size())
      suffixes_ << sfx;
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::addSuffix(): the suffix is empty");
};



//
void SgMasterRecord::parseString_v1(const QString& str)
{
  isValid_ = false;
  if (str.at(0) != QChar('|') || 
      str.at(str.size()-1)!=QChar('|') ||
      str.count(QChar('|')) != 16)
  {
    //complain (?):
    //logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    //  ": got a not valid MR-string: [" + str + "]; skipped");
    return;
  };
  QStringList fields = str.split("|");
  name_ = fields.at(1).simplified();
  code_ = fields.at(2).simplified().toLower();
  date_ = fields.at(3).simplified();
  stations_ = fields.at(7).simplified();
  dbcCode_ = fields.at(12).simplified();
  scheduledBy_ = fields.at(8).simplified();
  correlatedBy_ = fields.at(9).simplified();
  submittedBy_ = fields.at(13).simplified();
  if (name_.size() && code_.size() && dbcCode_.size() && date_.size())
    isValid_ = true;
};



//
void SgMasterRecord::parseString_v2(const QString& str)
{
  isValid_ = false;
  if (str.at(0) != QChar('|') || 
      str.at(str.size()-1)!=QChar('|') ||
      str.count(QChar('|')) != 14)
  {
    //complain (?):
    //logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    //  ": got a not valid MR-string: [" + str + "]; skipped");
    return;
  };
  QStringList fields = str.split("|");
  //
  //     SESSION      DATE     SESSION    DOY TIME   DUR                         STATIONS                        SKED CORR  STATUS  DBC  SUBM DEL
  //     TYPE     yyyymmdd     CODE     ddd hh:mm  h:mm                                                                  yyyymmdd CODE      days
  //---------------------------------------------------------------------------------------------------------------------------------------------
  //|IVS-R1      |20220103|r11032      |  3|17:00|24:00|HtIsKvMaNsNtNyOnSaWzYg -Ag                            |NASA|BONN|20220118| XA |NASA|  14|
  //     1            2         3         4   5     6                  7                                         8   9     10      11   12    13
  type_ = fields.at(1).simplified();
  date_ = fields.at(2).simplified();
  code_ = fields.at(3).simplified();
  //
  stations_ = fields.at(7).simplified();
  scheduledBy_ = fields.at(8).simplified();
  correlatedBy_ = fields.at(9).simplified();
  //
  dbcCode_ = fields.at(11).simplified();
  submittedBy_ = fields.at(12).simplified();
  name_ = code_; // set name to the session code

  if (type_.size() && code_.size() && date_.size())
    isValid_ = true;
};



//
bool SgMasterRecord::lookupRecordByName(const QString& dbName, const QString& path)
{
  QRegularExpression            reOldDbName("(\\d{2})([A-Z]{3})(\\d{2})([A-Z0-9]{1,2})");
  QRegularExpression            reNewDbName("(\\d{8})-([\\S]{2,12})");
  QRegularExpressionMatch       match;
  QDir                          dir(path);

  if (!dir.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::lookupRecordByName(): the masterfile directory \"" + path + 
      "\" does not exist; master file records are unavailable");
    return false;
  };

  QString                       sYear("");
  QString                       sDate("");
  QString                       sCode("");
  QString                       str("");
  if ((match=reOldDbName.match(dbName)).hasMatch())           // dbName: 12JUL16XA
  {
    sYear = match.captured(1);
    sDate = match.captured(2) + match.captured(3);
    sCode = match.captured(4);
    mfVersion_ = MFV_V01;
  }
  else if ((match=reNewDbName.match(dbName)).hasMatch())      // dbName: 20120716-r1542
  {
    sYear = match.captured(1).left(4);
    sDate = "";
//  sCode = match.captured(2).toUpper();
    sCode = match.captured(2).toLower();
    mfVersion_ = MFV_V02;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::lookupRecordByName(): the database name, " + dbName + ", is of unknown pattern");


  str = path + "/" + "master" + sYear;
  for (int i=0; i<suffixes_.size(); i++)
  {
    if (findRecordByName(sDate, sCode, str + suffixes_.at(i)))
      return true;
  };
  return false;
};



//
bool SgMasterRecord::findRecordByName(const QString& date, const QString& dbcCode,
  const QString& fileName)
{
  if (mfVersion_ == MFV_UNDEF)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::findRecordByName(): cannot guess masterfile version");
    return false;
  };
  //
  SgMasterRecord                mr;
  QString                       str;
  QFile                         f(fileName);
  if (!f.exists())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::findRecordByName(): the masterfile \"" + fileName + "\" does not exist");
    return false;
  };
  //
  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.size()>16)
      {
        if (mfVersion_ == MFV_V01)
          mr.parseString_v1(str);
        else
          mr.parseString_v2(str);
        if (0 < date.size())
        {
          if (mr.getDate() == date && mr.getDbcCode() == dbcCode)
          {
            *this = mr;
            f.close();
            s.setDevice(NULL);
            return true;
          };
        }
        else
        {
          if (mr.getCode() == dbcCode)
          {
            *this = mr;
            f.close();
            s.setDevice(NULL);
            return true;
          };
        };
      };
    };
    f.close();
    s.setDevice(NULL);
  };
  return false;
};



//
bool SgMasterRecord::lookupRecordByCode(const QString& sessionCode, const QString& sYear,
  const QString& path)
{
  QString                       str("");
  
  if (sYear.size() == 2)
    mfVersion_ = MFV_V01;
  else if (sYear.size() == 4)
    mfVersion_ = MFV_V02;

  QDir                          dir(path);
  if (!dir.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::lookupRecordByCode(): the masterfile directory \"" + path + 
      "\" does not exist; master file records are unavailable");
    return false;
  };

  str = path + "/" + "master" + sYear;
  for (int i=0; i<suffixes_.size(); i++)
  {
    if (findRecordByCode(sessionCode.toLower(), str + suffixes_.at(i)))
      return true;
  };
  return false;
};



//
bool SgMasterRecord::findRecordByCode(const QString& sessionCode, const QString& fileName)
{
  if (mfVersion_ == MFV_UNDEF)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::findRecordByName(): cannot guess masterfile version");
    return false;
  };
  //
  SgMasterRecord                mr;
  QString                       str;
  QFile                         f(fileName);
  if (!f.exists())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::findRecordByCode(): the masterfile \"" + fileName + "\" does not exist");
    return false;
  };
  //
  if (f.open(QFile::ReadOnly))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::findRecordByCode(): processing the masterfile \"" + fileName + "\"");
    QTextStream                 s(&f);
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.size()>16)
      {
        if (mfVersion_ == MFV_V01)
          mr.parseString_v1(str);
        else
          mr.parseString_v2(str);
        if (mr.getCode() == sessionCode) // some correlators provide lower case here
        {
          *this = mr;
          f.close();
          s.setDevice(NULL);
          return true;
        };
      };
    };
    f.close();
    s.setDevice(NULL);
  };
  return false;
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
QStringList                     defaultMasterfileSuffixes = {"-loc.txt", ".txt", "-int.txt", "-vgos.txt"};

/*=====================================================================================================*/
