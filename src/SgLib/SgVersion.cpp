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


#include <QtCore/QDataStream>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegExp>


#include <SgLogger.h>
#include <SgMJD.h>
#include <SgVersion.h>


/*=====================================================================================================*/
//
// common variable:
//
SgVersion                       libraryVersion( "SgLib",   0, 8, 3,
                                                "Stony Man",
                                                SgMJD( 2026,  5, 21,
                                                             15, 59));
/*=====================================================================================================*/




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
QString SgVersion::name(NameFormat fmt) const
{
  QString name;
  switch (fmt)
  {
    case NF_Human:
      name.sprintf("%s-%1d.%d.%d",
        qPrintable(softwareName_), majorNumber_, minorNumber_, teenyNumber_);
      if (codeName_.size())
        name += " (" + codeName_ + ")";
      break;
    case NF_Cylon:
      break;
    case NF_Petrov:
      name.sprintf("%s %s",
        qPrintable(softwareName_), qPrintable(releaseEpoch_.toString(SgMJD::F_YYYYMMDDDD)));
      break;
  };
  return name;
};



//
bool SgVersion::saveIntermediateResults(QDataStream& s) const
{
  s << softwareName_ << majorNumber_ << minorNumber_ << teenyNumber_ << codeName_;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  if (!releaseEpoch_.saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data for the epoch");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgVersion::loadIntermediateResults(QDataStream& s)
{
  QString                       softwareName, codeName;
  int                           majorNumber, minorNumber, teenyNumber;
  SgMJD                         releaseEpoch;
  //
  s >> softwareName >> majorNumber >> minorNumber >> teenyNumber >> codeName;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (!releaseEpoch_.loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data for the epoch: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (getSoftwareName() != softwareName)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: wrong order, name mismatch: got [" + 
      softwareName + "], expected [" + getSoftwareName() + "]");
    return false;
  };
  setSoftwareName(softwareName);
  setMajorNumber(majorNumber);
  setMinorNumber(minorNumber);
  setTeenyNumber(teenyNumber);
  setCodeName(codeName);
  setReleaseEpoch(releaseEpoch);
  //
  return s.status()==QDataStream::Ok;
};



//
bool SgVersion::parseString(const QString& str)
{
  bool                          isOk;
  QRegExp                       reIsStdName("(.+)-(\\d+\\.\\d+\\.\\d+)\\s+\\(\\s*(.+)\\s*\\).*");

  isOk = false;
  
  if (reIsStdName.indexIn(str) != -1)
  {
    QStringList                 lst=reIsStdName.cap(2).split(".");
    majorNumber_ = lst.at(0).toInt(&isOk);
    if (isOk)
    {
      minorNumber_ = lst.at(1).toInt(&isOk);
      if (isOk)
      {
        teenyNumber_ = lst.at(2).toInt(&isOk);
        softwareName_ = reIsStdName.cap(1);
        codeName_ = reIsStdName.cap(3);
      };
    };
  };
  return isOk;
};
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
