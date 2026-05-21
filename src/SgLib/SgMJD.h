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

#ifndef SG_MJD_H
#define SG_MJD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QDateTime>

class QDataStream;


//!< seconds in one day:
#ifndef DAY2SEC
#define DAY2SEC         (86400.0)
#endif



class SgMJD;
extern const SgMJD  tEphem;
extern const SgMJD  tZero;
extern const SgMJD  tInf;
extern const SgMJD  tUnix0;



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgMJD
{
public:
  enum Format
  {
    // date and time:
    // for humans:
    F_Verbose,              //! Verbose output: 02 Apr, 2010; 17:02:43.6400
    F_VerboseLong,          //! Long verbose: Fri, the 2nd of Apr, 2010; 17hr 02min 43.6400sec
    F_YYYYMMDDHHMMSSSS,     //! Digits: 2010/04/02 17:02:43.6
    F_Simple,               //! Digits: 2010/04/02 17:02:43
    // for cylons:
    F_YYYYMMDDSSSSSS,       //! Digits, date and seconds: 20100402613636
    F_YYYYMMDDDD,           //! Digits, date and time: 20100402.71
    F_INTERNAL,             //! Digits, MJD and seconds : 055288:61363.6400
    F_ECCDAT,               //! That was used in ECC.dat files: 2010.04.02-17.02
    F_SOLVE_SPLFL,          //! A spoolfile represenation of epoch: 2012.01.20-09:14:28
    F_SOLVE_SPLFL_V2,       //! Another spoolfile represenation of epoch: 2012.01.20-09:14:28.0
    F_SOLVE_SPLFL_V3,       //! Another spoolfile represenation of epoch: 2012.01.20-09:14:28.05
    F_SOLVE_SPLFL_SHORT,    //! Another version from spoolfile format: 12/01/20 00:02
    F_SOLVE_SPLFL_LONG,     //! Another version from spoolfile format: 2012.01.20-09:32:00.960
    F_FS_LOG,               //! Field System logs: 2020.195.11:15:34.11
    F_MJD,                  //! Just MJD: 55288.7102
    F_UNIX,                 //! UNUX seconds: 1270227763.6400
    F_SINEX,                //! SINEX format: 10:092:61364
    F_ISO,                  //! ISO date format realized by Qt (Qt::ISODate)
    F_RFC2822,              //! RFC2822 date format realized by Qt (Qt::RFC2822Date)
    // date only:
    F_Date,                 //! Date: 2010 Apr 02
    F_DDMonYYYY,            //! Another format for a date: 02 Apr, 2010
    F_YYYYMMDD,             //! Date in digits: 2010 04 02
    F_yyyymmdd,             //! Date in digits: 2010.04.02
    F_YYYYMonDD,            //! Date, short: 2016Mar01
    F_YYMonDD,              //! Date, more shortly: 10Apr02
    F_Year,                 //! Year: 2010.25
    F_SINEX_S,              //! SINEX, short version: 10:092
    // time only:
    F_Time,                 //! Just time: 17:02:43.6
    F_HHMMSS,               //! Time, seconds are integer: 17:02:43
    F_TimeShort,            //! Just hours and minutes: 17:02
  };

  
  // Statics:
  /**Calculates MJDay number for particular date.
   * \param year -- a year of the epoch;
   * \param month -- a month of the epoch;
   * \param day -- a day of the epoch.
   */
  static int calcDayNumber(int year, int month, int day);
  
  /**Calculates part of the day that is corresponding to given hour, min and sec.
   * \param hour -- an hour;
   * \param min -- a number of minutes;
   * \param sec -- seconds.
   */
  static double calcPartOfDay(int hour, int min, double sec);

  /**Converts date and time into human readable representation of the epoch for the given MJD.
   * \param date -- MJD number (input);
   * \param time -- part of the day (input);
   * \param nYear -- year (output);
   * \param nMonth -- month (output);
   * \param nDay -- day (output);
   * \param nHour -- hours (output);
   * \param nMin -- minutes (output);
   * \param dSec -- seconds (output);
   */
  static void MJD_reverse(int date, double time,
                          int& nYear, int& nMonth, int& nDay, int& nHour, int& nMin, double& dSec);

  /**Retuns an object that contains current epoch.
   */
  static SgMJD currentMJD();



  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes or later assignments).
   */
  inline SgMJD();

 /**A constructor.
  * Creates a copy of an object.
  * \param epoch -- a MJD epoch.
  */
  inline SgMJD(double epoch);

  /**A constructor.
   * Creates a copy of an object.
   * \param MJD -- a MJD epoch.
   */
  inline SgMJD(const SgMJD& MJD);
  
  /**A constructor.
   * Creates a copy of an object.
   * \param nDay -- number of MJD days;
   * \param dTime -- fraction part of a day, in days.
   */
  inline SgMJD(int nDay, double dTime);

  /**A constructor.
   * Creates a copy of an object.
   * \param nYear -- year of the date;
   * \param nMonth -- month of the date;
   * \param nDay -- day of the date;
   * \param nHour -- hours;
   * \param nMin -- minutes;
   * \param dSec -- seconds.
   */
  inline SgMJD(int nYear, int nMonth, int nDay, int nHour=0, int nMin=0, double dSec=0.0);

  inline SgMJD(const QDateTime&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgMJD();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgMJD& operator=(const SgMJD& T);

  /**Returns number of MJDays.
   */
  inline int getDate() const;

  /**Returns a fractional part of the day.
   */
  inline double getTime() const;

  /**Sets the number of MJDays.
   * \param nDays -- integer MJD;
   */
  inline void setDate(int nDays);

  /**Sets a fractional part of the day.
   * \param dTime -- fractional part of the day;
   */
  void setTime(double dTime);

  QDateTime toQDateTime() const;


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  inline QString className() const;
  
  /**Calculates and returns a year of the epoch.
   */
  int calcYear() const;

  /**Calculates and returns a month of the epoch.
   */
  int calcMonth() const;

  /**Calculates and returns a day of the epoch.
   */
  int calcDay() const;

  /**Calculates and returns day number in a year.
   */
  int calcDayOfYear() const;

  /**Calculates and returns day of a week.
   */
  int calcDayOfWeek() const;

  /**Calculates and returns an hour of the time.
   */
  inline int calcHour() const;

  /**Calculates and returns a minute of the time.
   */
  inline int calcMin() const;

  /**Calculates and returns a second of the time.
   */
  inline double calcSec() const;

  void toYMDHMS_tr(int& nYear, int& nMonth, int& nDay, int& nHour, int& nMin, double& dSec) const;

  
//  QString       month2Str()     const;

  /**Returns a day of a week in a literal form (3 chars abbr).
   */
  QString dayOfWeek2Str() const;

  /**Returns a day of a week in a (long) literal form.
   */
  QString dayOfWeek2LongStr() const;

  /**Sets up proper value of the epoch.
   * \param year -- a year;
   * \param month -- a month;
   * \param day -- a day of the month;
   * \param hour -- a hour;
   * \param min -- a minute;
   * \param sec -- a second.
   */
  void setUpEpoch(int year, int month, int day, int hour, int min, double sec);

  /**Returns a string that represents epoch, or only date or time part of it.
   * Output depend on Format parameter.
   * \param format -- a format of output string.
   */
  QString toString(Format format=F_Verbose) const; 

  /**Sets up proper value of the epoch parsing input string.
   * \param format -- a format of the string;
   * \param str -- input string to parse;
   * \param isReset -- if true, the value of *this is reseted to TZero before
   *        parsing the input string.
   */
  bool fromString(Format format, const QString& str, bool isReset=true);

 
  
  /** Compares the object with another one.
   */
  inline bool operator==(const SgMJD& T) const;
  
  /** Compares the object with another one.
   */
  inline bool operator!=(const SgMJD& T) const;
  
  /**Adds a double to the object.
   */
  SgMJD& operator+=(double days);

  /**Substract a double from the object.
   */
  SgMJD& operator-=(double days);
  
  /**Converts SgMJD type to double.
   */
  inline double toDouble() const;
  
  SgMJD toUtc() const;
  SgMJD toLocal() const;
  
  double gmst() const;
  

  //
  // Friends:
  //
  /** Returns a difference of two epochs.
   */
  inline friend double operator-(const SgMJD& T1, const SgMJD& T2);

  /** Compares T1 and T2.
   */
  inline friend bool operator<(const SgMJD& T1, const SgMJD& T2);

  /** Compares T1 and T2.
   */
  inline friend bool operator>(const SgMJD& T1, const SgMJD& T2);

  /** Compares T1 and T2.
   */
  inline friend bool operator<=(const SgMJD& T1, const SgMJD& T2);

  /** Compares T1 and T2.
   */
  inline friend bool operator>=(const SgMJD& T1, const SgMJD& T2);

  /** Returns an epoch shifted by dT in the future.
   */
  inline friend SgMJD operator+(const SgMJD& T, double dT);

  /** Returns an epoch shifted by dT in the past.
   */
  inline friend SgMJD operator-(const SgMJD& T, double dT);

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);


private:
  int                           date_;          //! integer part of the epoch;
  double                        time_;          //! part of the day, in days.
    
  /**Adjust time_ and date_ parts to keep 0<=time_<=1.
   */
  void                          normalize();

protected:
  static const char            *shortMonthNames_[];
  static const char            *longMonthNames_[];
  static const char            *shortWeekdayNames_[];
  static const char            *longWeekdayNames_[];
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgMJD inline members:                                                                               */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgMJD::SgMJD()
{
  date_ = 0;
  time_ = 0.0;
};



// A regular constructor:
inline SgMJD::SgMJD(double epoch)
{
  date_ = (int)floor(epoch); 
  time_ = epoch - (double) date_;
};



// Another constructor:
inline SgMJD::SgMJD(int nDay, double dTime)
{
  date_ = nDay + (int)trunc(dTime);
  time_ = dTime>=1.0? dTime-trunc(dTime) : dTime;
};



// A copying constructor:
inline SgMJD::SgMJD(const SgMJD& T)
{
  date_ = T.date_; 
  time_ = T.time_;
};



// Constructor too:
inline SgMJD::SgMJD(int nYear, int nMonth, int nDay, int nHour, int nMin, double dSec)
{
  setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSec);
};



inline SgMJD::SgMJD(const QDateTime& d)
{
  setUpEpoch(d.date().year(), d.date().month(), d.date().day(),
    d.time().hour(), d.time().minute(), d.time().second() + d.time().msec()/1000.0);
};



// A destructor:
inline SgMJD::~SgMJD()
{
  // nothing to do
};



//
// INTERFACES:
//
// returns number of MJDays
inline int SgMJD::getDate() const 
{
  return date_;
};



// returns fractional part of a day:
inline double SgMJD::getTime() const 
{
  return time_;
};



//
inline void SgMJD::setDate(int nDays)
{
  date_ = nDays;
};



//
inline SgMJD& SgMJD::operator=(const SgMJD& T)
{
  date_ = T.date_; 
  time_ = T.time_; 
  return *this;
};




//
// FUNCTIONS:
//
//
//
inline QString SgMJD::className() const
{
  return "SgMJD";
};


//
inline int SgMJD::calcHour() const
{
  return (int)(time_*DAY2SEC/3600.0);
};


//
inline int SgMJD::calcMin() const
{
  return (int)((time_*DAY2SEC - 3600.0*calcHour())/60.0);
};


//
inline double SgMJD::calcSec() const
{
  return time_*DAY2SEC - 3600.0*calcHour() - 60.0*calcMin();
};



//
inline bool SgMJD::operator==(const SgMJD& T) const
{
  return T.date_==date_ && T.time_==time_;
};



//
inline bool SgMJD::operator!=(const SgMJD& T) const
{
  return T.date_!=date_ || T.time_!=time_;
};



// explicit type conversion:
inline double SgMJD::toDouble() const
{
  return date_ + time_;
};




//
// FRIENDS:
//
//
//
inline double operator-(const SgMJD& T1, const SgMJD& T2)
{
  return (T1.date_-T2.date_) + (T1.time_-T2.time_);
};



//
inline bool operator<(const SgMJD& T1, const SgMJD& T2)
{
  if (T1.date_ < T2.date_) 
    return true;
  return T1.date_==T2.date_? T1.time_<T2.time_ : false;
};



//
inline bool operator<=(const SgMJD& T1, const SgMJD& T2)
{
  if (T1.date_ < T2.date_) 
    return true;
  return T1.date_==T2.date_? T1.time_<=T2.time_ : false;
};



//
inline bool operator>(const SgMJD& T1, const SgMJD& T2)
{
  if (T1.date_ > T2.date_) 
    return true;
  return T1.date_==T2.date_? T1.time_>T2.time_ : false;
};



//
inline bool operator>=(const SgMJD& T1, const SgMJD& T2)
{
  if (T1.date_ > T2.date_) 
    return true;
  return T1.date_==T2.date_? T1.time_>=T2.time_ : false;
};



//
inline SgMJD operator+(const SgMJD& T, double dT)
{
  return SgMJD(T)+=dT;
};



//
inline SgMJD operator-(const SgMJD& T, double dT)
{
  return SgMJD(T)-=dT;
};


/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//
/**Returns a string for time interval
 * \param days -- time interval (usually, difference of two SgMJD epochs) in days.
 */
QString interval2Str(double days);


/*=====================================================================================================*/
#endif //SG_MJD_H
