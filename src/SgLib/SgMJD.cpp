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


#include <QtCore/QDataStream>

#include <SgLogger.h>
#include <SgMJD.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
double SgMJD::calcPartOfDay(int hour, int min, double sec)
{
  return sec/DAY2SEC + (double)min/1440.0 + (double)hour/24.0;
};



// static:
// where did I get this code?
// TODO: check & upgrade
int SgMJD::calcDayNumber(int year, int month, int day)
{
  unsigned int                  century, yrsInCent;
  if (year<=57) //? should we get rid of this?
    year += 2000;
  if (year<=99) //? should we get rid of this too?
    year += 1900;
  if (month>2)
    month -= 3;
  else
  {
    month += 9;
    year--;
  };
  century   = year/100;
  yrsInCent = year - 100*century;
  return -678882 + day + (146097*century)/4 + (1461*yrsInCent)/4 + (153*month + 2)/5;
};



// the reverse procedure: 
// TODO: check & upgrade
void SgMJD::MJD_reverse(int date, double time, 
                        int& nYear, int& nMonth, int& nDay, int& nHour, int& nMin, double& dSec)
{
  if (time>=1.0)
  {
    time--;
    date++;
  };
  // integer days:
  unsigned int                  x;
  unsigned int                  j=date + 678882;
  nYear = (j*4 - 1)/146097;
  j = j*4 - 146097*nYear - 1;
  x = j/4;
  j = (x*4 + 3) / 1461;
  nYear = 100*nYear + j;
  x = x*4 + 3 - 1461*j;
  x = (x + 4)/4;
  nMonth = (5*x - 3)/153;
  x = 5*x - 3 - 153*nMonth;
  nDay = (x + 5)/5;
  if (nMonth < 10) 
    nMonth += 3; 
  else 
  {
    nMonth -= 9;
    nYear++;
  };

  // part of day:
  double                        dTmp=time*DAY2SEC + 1.0E-7;
  nHour = (int)floor(dTmp/3600.0);
  nMin  = (int)floor((dTmp - 3600.0*nHour)/60.0);
  dSec  = dTmp - 3600.0*nHour - 60.0*nMin;
  if (dSec >= 1.0E-7)
    dSec -= 1.0E-7;
  else
    dSec = 0.0;
};



//
// here we use Qt library (should we roll back to LibC?):
SgMJD SgMJD::currentMJD()
{
  QDateTime         d(QDate::currentDate(), QTime::currentTime());
  SgMJD             currentEpoch( d.date().year(), d.date().month(), d.date().day(),
                                  d.time().hour(), d.time().minute(),
                                  d.time().second()+d.time().msec()*0.001);
  return currentEpoch;
};
//
// end of statics.
//



//
//
void SgMJD::normalize()
{
  while(time_>=1.0) 
  {
    time_--; 
    date_++; 
  }; 
  while(time_<0.0) 
  {
    time_++; 
    date_--;
  };
};



//
QDateTime SgMJD::toQDateTime() const
{
  int                           nYr, nMo, nDy, nHr, nMi, nSec, nMsec;
  double                         dSec;

  SgMJD::MJD_reverse(date_, time_, nYr, nMo, nDy, nHr, nMi, dSec);
  nSec  = (int)floor(dSec);
  nMsec = (int)floor((dSec - nSec)*1000.0);
  return QDateTime(QDate(nYr, nMo, nDy), QTime(nHr, nMi, nSec, nMsec), Qt::UTC);
};



//
void SgMJD::setTime(double dTime)
{
  time_ = dTime; 
  normalize();
};



//
SgMJD& SgMJD::operator+=(double days)
{
  // split input on two parts:
  int                           nDays=(int)trunc(days);
  double                        dPart=days - nDays;
  // make operation:
  date_ += nDays;
  time_ += dPart;
  normalize();
  return *this;
};



//
SgMJD& SgMJD::operator-=(double days)
{
  // split input on two parts:
  int                           nDays=(int)trunc(days);
  double                        dPart=days - nDays;
  // make operation:
  date_ -= nDays;
  time_ -= dPart;
  normalize();
  return *this;
};



//
int SgMJD::calcYear() const
{
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;
  MJD_reverse(date_, time_,  nYear, nMonth, nDay, nHour, nMin, dSec);
  return nYear;
};



//
int SgMJD::calcMonth() const
{
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;
  MJD_reverse(date_, time_,  nYear, nMonth, nDay, nHour, nMin, dSec);
  return nMonth;
};



//
int SgMJD::calcDay() const
{
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;
  MJD_reverse(date_, time_,  nYear, nMonth, nDay, nHour, nMin, dSec);
  return nDay;
};



//
int SgMJD::calcDayOfYear() const
{
  return date_ - calcDayNumber(calcYear(), 1, 1) + 1;
};



// similar to month number, it is in the range [1..7]:
int SgMJD::calcDayOfWeek() const
{
  int                           i = (date_ - 55291)%7;//today is Monday, 5th of April 2010, MJD is 55291.
  return (i<0?i+7:i)+1;
};



// sets up epoch:
void SgMJD::setUpEpoch(int nYear, int nMonth, int nDay, int nHour, int nMin, double dSec)
{
  if (nMonth != 0)
    date_ = calcDayNumber(nYear, nMonth, nDay);
  else // month==0 and day is DOY like in sinex
    date_ = calcDayNumber(nYear, 1, 1) + nDay - 1;
  time_ = calcPartOfDay(nHour, nMin, dSec);
  normalize();
};



// parses input string and sets epoch:
bool SgMJD::fromString(Format F, const QString& s, bool isReset)
{
  int                           i, nYear, nMonth, nDay, nHour, nMin;
  double                        dSec, d;
  bool                          isOk;
  QString                       tmp;
  isOk = false;

  if (isReset) 
    *this = tZero;
  
  if (s=="UNKNOWN") 
  {
    // complain?
    return isOk;
  };

  switch(F)
  {      
  case F_Verbose:
    // Verbose representation of an epoch:
    // 0123456789012345678901234567
    // 12 Apr, 2010; 01:03:25.2000
    if (s.length()<27)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(0,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nMonth = 0;
    tmp = s.mid(3,3);
    for (i=0; i<12; i++) 
      if (tmp.startsWith(shortMonthNames_[i]))
        nMonth = i + 1;
    if (nMonth==0) // month not found
    {
      // complain
      return isOk;
    };
    nYear = s.mid(8,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(14,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(17,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(20,7).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_VerboseLong: // is there a sence to make parsering for it?
    // Mon, the 12th of Apr, 2010; 01hr 03min 25.2000sec
    break;

  case F_Simple:
  case F_YYYYMMDDHHMMSSSS:
    // 0123456789012345678901
    // 2010/04/12 01:03:25.2
    if (s.length()<19)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(8,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(5,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(11,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(14,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(17,4).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_YYYYMMDDSSSSSS:
    // 012345678901234
    // 20100412038051
    if (s.length()<14)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(6,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(4,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(8,6).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, dSec/10.0);
    break;


  case F_YYYYMMDDDD:
    // 012345678901234
    // 20100412.04
    if (s.length()<11)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(6,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(4,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(9).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, dSec*86400.0);
    break;


  case F_INTERNAL: //MJDate:TimeInSeconds
    // 01234567890123456789
    // 055298:03805.200000
    if (s.length()<19)
    {
      // complain
      return isOk;
    };
    if (s.mid(6,1)!=":")
    {
      // complain
      return isOk;
    };
    nDay = s.mid(0,6).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(7,12).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    date_ = nDay;
    time_ = dSec/DAY2SEC;
    break;

  case F_ECCDAT:
    // 01234567890123456
    // 2010.04.12-01.03
    if (s.length()<16)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(5,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(8,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nHour = s.mid(11,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nMin = s.mid(14,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, 0.0);
    break;

  case F_SOLVE_SPLFL:
  case F_SOLVE_SPLFL_V2:
  case F_SOLVE_SPLFL_V3:
    // 01234567890123456789
    // 2012.01.20-09:14:28
    if (s.length()<19)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(5,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(8,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(11,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(14,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(17).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_SOLVE_SPLFL_LONG:
    // 012345678901234567890123
    // 2012.01.20-09:32:00.960
    if (s.length()<23)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(5,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(8,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(11,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(14,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(17,6).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_FS_LOG:
  // 012345678901234567890123
  // 2020.195.11:15:34.11
    if (s.length()<17)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(5,3).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(9,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(12,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(15,5).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, 0, nDay, nHour, nMin, dSec);
  break;
  
  case F_SOLVE_SPLFL_SHORT:
    // 01234567890123456789
    // 12/01/20 00:02
    if (s.length()<14)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,2).toInt(&isOk);
    nYear += (nYear<65)?2000:1900;
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(3,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(6,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nHour = s.mid(9,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMin = s.mid(12,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, nHour, nMin, 0.0);
    break;

  case F_MJD:
    // 01234567890
    // 55298.0440
    if (s.length()<10)
    {
      // complain
      return isOk;
    };
    d = s.toDouble(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    date_ = (int)floor(d);
    time_ = d - (double)date_;
    break;

  case F_UNIX:
    // 0123456789012345
    // 1271034205.2000
    if (s.length()<15)
    {
      // complain
      return isOk;
    };
    d = s.toDouble(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    *this = SgMJD(tUnix0 + d/86400.0);
    break;

  case F_SINEX:
    // 0123456789012
    // 10:102:03805
    if (s.length()<12)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(3,3).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    dSec = s.mid(7,5).toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear<=50? nYear+2000 : nYear+1900, 0, nDay,  0, 0, dSec);
    break;


  case F_ISO:                 // e.g., 
      *this = SgMJD(QDateTime::fromString(s, Qt::ISODate));
    break;

  case F_RFC2822:             // e.g., 
      *this = SgMJD(QDateTime::fromString(s, Qt::RFC2822Date));
    break;


  // date only:
  case F_Date:
    // 012345678901
    // 2010 Apr 12
    if (s.length()<11)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(9,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = 0;
    tmp = s.mid(5,3);
    for (i=0; i<12; i++) 
      if (tmp.startsWith(shortMonthNames_[i]))
        nMonth = i + 1;
    if (nMonth==0) // month not found
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, 0.0);
    break;

  case F_DDMonYYYY:
    // 0123456789012
    // 12 Apr, 2010
    if (s.length()<12)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(0,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = 0;
    tmp = s.mid(3,3);
    for (i=0; i<12; i++) 
      if (tmp.startsWith(shortMonthNames_[i]))
        nMonth = i + 1;
    if (nMonth==0) // month not found
    {
      // complain
      return isOk;
    };
    nYear = s.mid(8,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, 0.0);
    break;

  case F_YYYYMMDD:
  case F_yyyymmdd:
    // 01234567890
    // 2010 04 12
    if (s.length()<10)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = s.mid(5,2).toInt(&isOk);
    if (!isOk)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(8,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, 0.0);
    break;

  case F_YYMonDD:
    // 01234567
    // 10Apr12
    if (s.length()<7)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(5,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = 0;
    tmp = s.mid(2,3);
    for (i=0; i<12; i++) 
      if (tmp.startsWith(shortMonthNames_[i], Qt::CaseInsensitive  ))
        nMonth = i + 1;
    if (nMonth==0) // month not found
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear<=50? nYear+2000 : nYear+1900, nMonth, nDay, 0, 0, 0.0);
    break;

  case F_YYYYMonDD:
    // 0123456789
    // 2016Mar01
    if (s.length()<9)
    {
      // complain
      return isOk;
    };
    nDay = s.mid(7,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nMonth = 0;
    tmp = s.mid(4,3);
    for (i=0; i<12; i++) 
      if (tmp.startsWith(shortMonthNames_[i], Qt::CaseInsensitive  ))
        nMonth = i + 1;
    if (nMonth==0) // month not found
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,4).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear, nMonth, nDay, 0, 0, 0.0);
    break;

  case F_Year:
    // 01234567
    // 2010.28
    if (s.length()<7)
    {
      // complain
      return isOk;
    };
    d = s.toDouble(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    date_ = calcDayNumber((int)floor(d), 1, 1); // MJD on the begin of the year
    time_ = 365.25*fmod(d, 1.0);
    normalize();
    break;

  case F_SINEX_S:
    // 0123456
    // 10:102
    if (s.length()<6)
    {
      // complain
      return isOk;
    };
    nYear = s.mid(0,2).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    nDay = s.mid(3,3).toInt(&isOk);
    if (!isOk) 
    {
      // complain
      return isOk;
    };
    setUpEpoch(nYear<=50? nYear+2000 : nYear+1900, 0, nDay,  0, 0, 0.0);
    break;
    
  // unsupported formats (just time):
  case F_Time:
    return isOk;
    break;
  case F_HHMMSS:
    return isOk;
    break;
  case F_TimeShort:
    return isOk;
    break;
  };
  return true;
};



// makes formatted output:
QString SgMJD::toString(Format F) const
{
  QString                       s("UNKNOWN");
  QString                       th;
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;

  if (date_==0)
    return s;
  
  MJD_reverse(date_, time_,  nYear, nMonth, nDay, nHour, nMin, dSec);
  switch(F)
  {
  // date and time:
  case F_Verbose:             // e.g., 12 Apr, 2010; 01:03:25.2000
    s.sprintf("%02d %s, %04d; %02d:%02d:%07.4f",
      nDay, shortMonthNames_[nMonth-1], nYear, 
      nHour, nMin, dSec);
    break;

  case F_VerboseLong:         // e.g., Mon, the 12th of Apr, 2010; 01hr 03min 25.2000sec
    switch (nDay)
    {
    case  1:
    case 21:
    case 31:
      th="st"; 
      break;
    case  2:  
    case 22:  
      th="nd"; 
      break;
    case  3:
    case 23:
      th="rd"; 
      break;
    default: 
      th="th";
    };
    s.sprintf("%s, the %d%s of %s, %04d; %02dhr %02dmin %07.4fsec",
      qPrintable(dayOfWeek2Str()), nDay, qPrintable(th), shortMonthNames_[nMonth-1], nYear, 
      nHour, nMin, dSec);
    break;
  
  case F_YYYYMMDDHHMMSSSS:    // e.g., 2010/04/12 01:03:25.2
    s.sprintf("%04d/%02d/%02d %02d:%02d:%04.1f", nYear, nMonth, nDay, nHour, nMin, dSec);
    break;
  case F_Simple:             // e.g., 2010/04/12 01:03:25
    s.sprintf("%04d/%02d/%02d %02d:%02d:%02d", nYear, nMonth, nDay, nHour, nMin, (int)(round(dSec)));
    break;

  case F_YYYYMMDDSSSSSS:      // e.g., 20100412038051
    s.sprintf("%04d%02d%02d%06d", nYear, nMonth, nDay, nHour*36000 + nMin*600 + (int)(round(dSec*10.0)));
    break;

  case F_YYYYMMDDDD:      // e.g., 20100412.04
    s.sprintf("%04d%02d%02d.%02d", nYear, nMonth, nDay, (int)round((nHour*3600 + nMin*60 + dSec)/864.0));
    break;

  case F_INTERNAL:            // e.g., 055298:03805.200000
    s.sprintf("%06d:%012.6f", date_, time_*DAY2SEC);
    break;

  case F_ECCDAT:              // e.g., 2010.04.12-01.03
    s.sprintf("%04d.%02d.%02d-%02d.%02d", nYear, nMonth, nDay, nHour, nMin);
    break;

  case F_SOLVE_SPLFL:         // e.g., 2012.01.20-09:14:28
    s.sprintf("%04d.%02d.%02d-%02d:%02d:%02d", nYear, nMonth, nDay, nHour, nMin, (int)(round(dSec)));
    break;

  case F_SOLVE_SPLFL_V2:      // e.g., 2014.11.18-18:30:31.0
    s.sprintf("%04d.%02d.%02d-%02d:%02d:%04.1f", nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_SOLVE_SPLFL_V3:      // e.g., 2014.11.18-18:30:31.05
    s.sprintf("%04d.%02d.%02d-%02d:%02d:%05.2f", nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_SOLVE_SPLFL_SHORT:   // e.g., 12/01/20 00:02
    nYear -= (nYear/100)*100;
    s.sprintf("%02d/%02d/%02d %02d:%02d", nYear, nMonth, nDay, nHour, nMin);
    break;

  case F_SOLVE_SPLFL_LONG:   // e.g., 2012.01.20-09:32:00.960
    s.sprintf("%04d.%02d.%02d-%02d:%02d:%06.3f", nYear, nMonth, nDay, nHour, nMin, dSec);
    break;

  case F_FS_LOG:
  // 012345678901234567890123
  // 2020.195.11:15:34.11
    s.sprintf("%04d.%03d.%02d:%02d:%05.2f", nYear, calcDayOfYear(), nHour, nMin, dSec);
    break;

  case F_MJD:                 // e.g., 55298.0440
    s.sprintf("%10.4f", date_ + time_);
    break;

  case F_UNIX:                // e.g., 1271034205.2000
    s.sprintf("%.4f", DAY2SEC*(*this - tUnix0));
    break;

  case F_SINEX:               // e.g., 10:102:03805
    if (*this==tZero)
      s="00:000:00000";
    else
      s.sprintf("%02d:%03d:%05d", 
                nYear-100*(nYear/100), calcDayOfYear(), (int)round(time_*DAY2SEC));
    break;

  case F_ISO:                 // e.g., 
      s = toQDateTime().toString(Qt::ISODate); 
    break;

  case F_RFC2822:             // e.g., 
      s = toQDateTime().toString(Qt::RFC2822Date);
    break;

  // date only:
  case F_Date:                // e.g., 2010 Apr 12
    s.sprintf("%04d %s %02d", nYear, shortMonthNames_[nMonth-1], nDay);
    break;
  
  case F_DDMonYYYY:           // e.g., 12 Apr, 2010
    s.sprintf("%02d %s, %04d", nDay, shortMonthNames_[nMonth-1], nYear);
    break;

  case F_YYYYMMDD:            // e.g., 2010 04 12
  default:
    s.sprintf("%04d %02d %02d", nYear, nMonth, nDay);
    break;

  case F_yyyymmdd:
    s.sprintf("%04d.%02d.%02d", nYear, nMonth, nDay);
    break;

  case F_YYMonDD:             // e.g., 10Apr12
    s.sprintf("%02d%s%02d", nYear-100*(nYear/100), shortMonthNames_[nMonth-1], nDay);
    break;

  case F_YYYYMonDD:           // e.g., 2016Mar01
    s.sprintf("%04d%s%02d", nYear, shortMonthNames_[nMonth-1], nDay);
    break;

  case F_Year:                // e.g., 2010.28
    s.sprintf("%7.2f", nYear + calcDayOfYear()/365.25);
    break;

  case F_SINEX_S:             // e.g., 10:102
    s.sprintf("%02d:%03d", nYear-100*(nYear/100), calcDayOfYear());
    break;

  // time only:
  case F_Time:                // e.g., 01:03:25.2
    if (dSec>59.9)
    {
      dSec = 0.0;
      nMin++;
    };
    if (nMin==60)
    {
      nHour++;
      nMin = 0;
    };
    s.sprintf("%02d:%02d:%04.1f", nHour, nMin, dSec);
    break;

  case F_HHMMSS:              // e.g., 01:03:25
    s.sprintf("%02d:%02d:%02d", nHour, nMin, (int)floor(dSec));
    break;

  case F_TimeShort:           // e.g., 01:03
    if (dSec>30.0)
      nMin++;
    if (nMin==60)
    {
      nHour++;
      nMin = 0;
    };
    s.sprintf("%02d:%02d", nHour, nMin);
    break;
  };
  return s;
};



// converts week's idx to string:
QString SgMJD::dayOfWeek2Str() const
{
  int                           dayOfWeek=calcDayOfWeek();
  if (1<=dayOfWeek && dayOfWeek<=7)
    return shortWeekdayNames_[dayOfWeek-1];
  else
  {
    logger->write(SgLogger::ERR, SgLogger::TIME, className() +
      QString().sprintf(": dayOfWeek2Str(): for the date %d dayOfWeek (%d) is out of ranges",
      date_, dayOfWeek));
    exit(1);
  };
};



// converts week's idx to string:
QString SgMJD::dayOfWeek2LongStr() const
{
  int                           dayOfWeek=calcDayOfWeek();
  if (1<=dayOfWeek && dayOfWeek<=7)
    return longWeekdayNames_[dayOfWeek-1];
  else
  {
    logger->write(SgLogger::ERR, SgLogger::TIME, className() +
      QString().sprintf(": dayOfWeek2LongStr(): for the date %d dayOfWeek (%d) is out of ranges",
      date_, dayOfWeek));
    exit(1);
  };
};



//
SgMJD SgMJD::toUtc() const
{
  int                           yr, mn, dy;
  int                           hr, mi;
  double                        sc;
  MJD_reverse(date_, time_,  yr, mn, dy, hr, mi, sc);
  QDateTime                     d(QDate(yr, mn, dy), QTime(hr, mi, (int)(round(sc))), Qt::LocalTime);
  QDateTime                     dUtc = d.toUTC();
  SgMJD                         epoch(dUtc.date().year(), dUtc.date().month(), dUtc.date().day(),
                                  dUtc.time().hour(), dUtc.time().minute(),
                                  dUtc.time().second()+dUtc.time().msec()*0.001);
  return epoch;
};



//
SgMJD SgMJD::toLocal() const
{
  int                           yr, mn, dy;
  int                           hr, mi;
  double                        sc;
  MJD_reverse(date_, time_,  yr, mn, dy, hr, mi, sc);
  QDateTime                     d(QDate(yr, mn, dy), QTime(hr, mi, (int)(round(sc))), Qt::UTC);
  QDateTime                     dLoc = d.toLocalTime();
  SgMJD                         epoch(dLoc.date().year(), dLoc.date().month(), dLoc.date().day(),
                                  dLoc.time().hour(), dLoc.time().minute(),
                                  dLoc.time().second()+dLoc.time().msec()*0.001);
  return epoch;
};



//
double SgMJD::gmst() const
{
  double                        dGmst, dGmst0h=0.0, dT_u=0.0, dT_u2=0.0, r;
  
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 21 says:
  
    "Apparent Geenwich Sidereal Time GST at the date t of observations, must be derived
    from the following expressions:
  
    (i) the relationship between Greenwich Mean Sidereal Time (GMST) and Universal.
    Time as given by Aoki et al. (1982):

    with T'_u = d'_u/36525, d'_u being the number of days elapsed since.
    2000 January 1, 12h UT1, takin on values +/- 0.5, +/- 1.5, ...,"
  */
  dT_u = (date_ - tEphem)/36525.0;
  dT_u2 = dT_u*dT_u;
  dGmst0h = (24110.54841 + 8640184.812866*dT_u + 0.093104*dT_u2 - 6.2e-6*dT_u*dT_u2)/DAY2SEC; //days

  /*
     "where r is the ratio of universal to sidereal time as given by Aoki et al. (1992),"
  */
  r = 1.002737909350795 + 5.9006e-11*dT_u - 5.9e-15*dT_u2;

  /*
    "(ii) the interval of GMST from 0h UT1 to the hour of the observation in UT1,"
  */
  dGmst = dGmst0h + r*time_;              // days
  dGmst = fmod(dGmst, 1.0);               // days
  if (dGmst < 0.0)
    dGmst += 1.0;

  dGmst *= 2.0*M_PI;                      // rad
  return dGmst;
};
  


//
void SgMJD::toYMDHMS_tr(int& nYear, int& nMonth, int& nDay, int& nHour, int& nMin, double& dSec) const
{
  MJD_reverse(date_, time_, nYear, nMonth, nDay, nHour, nMin, dSec);
  dSec = 1.0e-8*round(dSec*1.0e8);
};



//
bool SgMJD::saveIntermediateResults(QDataStream& s) const
{
  s << date_ << time_;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgMJD::loadIntermediateResults(QDataStream& s)
{
  int                           date;
  double                        time;
  //
  s >> date >> time;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  date_ = date;
  time_ = time;
  //
  return s.status()==QDataStream::Ok;
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
QString interval2Str(double days)
{
  int                           nYears=0, nMonths=0, nDays=0, nHours=0, nMins=0;
  double                        dSecs=0.0;
  QString                       str("");
  bool                          isSignPositive=(days>=0.0);
  
  if (!isSignPositive) 
  {
    days = -days;
    str = "-(";
  };
  
  if (days>=366.0)
  {
    nYears = (int)floor(days/365.25);
    days -= 365.25*nYears;
  };
  if (days>=31.0) // here mean month, 365.25/12, is 30 days 10hr and 30 min
  {
    nMonths = (int)floor(days*12.0/365.25);
    days -= nMonths*365.25/12.0;
  };
  if (days>=1.0)
  {
    nDays = (int)floor(days);
    days -= (double)nDays;
  };
 
  // ok, here days < 1.0, convert it to seconds:
  days *= DAY2SEC;
  nHours = (int)(days/3600.0);
  nMins  = (int)((days - 3600.0*nHours)/60.0);
  dSecs  = days - 3600.0*nHours - 60.0*nMins;

  if (nYears==0 && nMonths==0 && nDays==0 && nHours==0 && nMins==0 && dSecs<1.0)
    str += QString().sprintf("%.6g sec", dSecs); // special case of intervals, dt<1sec
  else
  {
    if (nYears)
      str += QString().sprintf("%d year%s ", nYears, nYears==1?"":"s");
    if (nMonths)
      str += QString().sprintf("%d month%s ", nMonths, nMonths==1?"":"s");
    if (nDays)
      str += QString().sprintf("%d day%s ", nDays, nDays==1?"":"s");
    if (nHours)
      str += QString().sprintf("%02dhr ", nHours);
    if (nMins)
      str += QString().sprintf("%02dmin ", nMins);
    str += QString().sprintf("%05.2fsec", dSecs);
    
//    Str += QString().sprintf("%02dhr %02dmin %05.2fsec", nHours, nMins, dSecs);
  };
  if (!isSignPositive)
    str += ")";
  return str;
};



// i/o:
//


/*=====================================================================================================*/
//
// constants:
//
const char                     *SgMJD::shortMonthNames_[] = 
{
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char                     *SgMJD::longMonthNames_[] = 
{
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};

const char                     *SgMJD::shortWeekdayNames_[] =
{
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

const char                     *SgMJD::longWeekdayNames_[] =
{
  "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
};


const SgMJD tEphem(51544.5);    // J2000 1Jan 12h 0m 0.0s
const SgMJD tZero (1957,10, 4); // Sputnik 1 was launched on October 4, 1957
const SgMJD tInf  (2100, 1, 1); // 
const SgMJD tUnix0(1970, 1, 1); // Unix zero epoch
/*=====================================================================================================*/
