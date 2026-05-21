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

#ifndef SG_LOGGER_H
#define SG_LOGGER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <iostream>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>




class SgLogger;
extern SgLogger *logger;



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgLogger
{
public:
  enum LogLevel
  {
    ERR,                                  //<! Log level: error
    WRN,                                  //<! Log level: normal
    INF,                                  //<! Log level: info
    DBG                                   //<! Log level: debug
  };
  enum LogFacility
  {
    IO_BIN      = 1<< 0,                  //<! Log facility:
    IO_TXT      = 1<< 1,                  //<! Log facility:
    IO_NCDF     = 1<< 2,                  //<! Log facility: NetCDF I/O manipulations
    IO_DBH      = 1<< 3,                  //<! Log facility: DBH I/O manipulations
    IO          = IO_BIN | IO_TXT | IO_NCDF | IO_DBH, 
    MATRIX      = 1<< 4,                  //<! Log facility: matrix/vector
    MATRIX3D    = 1<< 5,                  //<! Log facility: 3d matrix/3d vector
    INTERP      = 1<< 6,                  //<! Log facility: interpolations
    MATH_RES_1  = 1<< 7,                  //<! Log facility:
    MATH        = MATRIX | MATRIX3D | INTERP | MATH_RES_1,
    OBS         = 1<< 8,                  //<! Log facility:
    STATION     = 1<< 9,                  //<! Log facility: station
    SOURCE      = 1<<10,                  //<! Log facility: source
    SESSION     = 1<<11,                  //<! Log facility:
    DATA        = OBS | STATION | SOURCE | SESSION,
    REF_FRAME   = 1<<12,                  //<! Log facility:
    TIME        = 1<<13,                  //<! Log facility:
    IONO        = 1<<14,                  //<! Log facility: ionospheric corrections
    REFRACTION  = 1<<15,                  //<! Log facility: refraction delay
    DELAY       = 1<<16,                  //<! Log facility: grav. delay
    RATE        = 1<<17,                  //<! Log facility:
    FLY_BY      = 1<<18,                  //<! Log facility:
    DISPLACEMENT= 1<<19,                  //<! Log facility:
    GEO_RES_1   = 1<<20,                  //<! Log facility:
    GEO         = REF_FRAME | TIME | IONO | REFRACTION | DELAY | RATE | FLY_BY |
                    DISPLACEMENT | GEO_RES_1,
    ESTIMATOR   = 1<<21,                  //<! Log facility: estimator
    PWL         = 1<<22,                  //<! Log facility: piecewise linear parameters
    STOCH       = 1<<23,                  //<! Log facility: stochastic parameters
    CONFIG      = 1<<24,                  //<! Log facility: config
    GUI         = 1<<25,                  //<! Log facility: GUI
    REPORT      = 1<<26,                  //<! Log facility: report generator
    RESOURCE    = 1<<27,                  //<! Log facility: resource manager
    RUN         = 1<<28,                  //<! Log facility: project manager
    PREPROC     = 1<<29,                  //<! Log facility: preprocessing
    RESERVED_1  = 1<<30,                  //<! Log facility: preprocessing
    RESERVED_2  = 1<<31,                  //<! Log facility: preprocessing
    ALL         = IO | MATH | DATA | GEO | ESTIMATOR | PWL | STOCH | CONFIG | GUI | REPORT | RESOURCE |
                    RUN | PREPROC | RESERVED_1 | RESERVED_2,
  };

  // Statics:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  SgLogger(int capacity=100, bool isStoreInFile=false, const QString& fileName="logger.log");


  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgLogger();


  //
  // Interfaces:
  //
  inline void setDirName(const QString &dirName) {dirName_ = dirName;};
  inline void setFileName(const QString &fileName) {fileName_ = fileName;};
  inline void setCapacity(int capacity) {capacity_ = capacity;};
  inline void setIsStoreInFile(bool isStoreInFile) {isStoreInFile_ = isStoreInFile;};
  inline void setUseFullDateFormat(bool useFullDateFormat) {useFullDateFormat_ = useFullDateFormat;};
  inline void setIsNeedTimeMark(bool isNeedTimeMark) {isNeedTimeMark_ = isNeedTimeMark;};
  inline void setIsMute(bool is) {isMute_ = is;};
  inline void setLogFacility(LogLevel lvl, quint32 f) {logFacilities_[lvl] = f;};
  inline void setFacilitiesSerialNumber(double sn) {facilitiesSerialNumber_ = sn;};
  
  inline const QString& getDirName() const {return dirName_;};
  inline const QString& getFileName() const {return fileName_;};
  inline int getCapacity() const {return capacity_;};
  inline bool getIsStoreInFile() const {return isStoreInFile_;};
  inline bool getUseFullDateFormat() const {return useFullDateFormat_;};
  inline bool getIsNeedTimeMark() const {return isNeedTimeMark_;};
  inline bool getIsMute() const {return isMute_;};
  inline quint32 getLogFacility(LogLevel lvl) const {return logFacilities_[lvl];};
  inline double getFacilitiesSerialNumber() const {return facilitiesSerialNumber_;};
  inline const QList<QString*>& getSpool() const {return spool_;};

  //
  // Functions:
  //
  inline QString className() const {return "SgLogger";};
  inline void addLogFacility(LogLevel lvl, quint32 f) {logFacilities_[lvl] |= f;};
  inline void delLogFacility(LogLevel lvl, quint32 f) {logFacilities_[lvl] &= ~f;};
  inline bool isEligible(LogLevel lvl, quint32 f) const {return logFacilities_[lvl]&f;};
  virtual void startUp();
  virtual void clearSpool();
  virtual void write(LogLevel, quint32, const QString &, bool=false);
  //
  void attachSupplementLog(const QString& name, SgLogger *auxLogger);
  void detachSupplementLog(const QString& name);
  SgLogger* lookupSupplementLog(const QString& name);
  void rmLogFile();



  //
  // Friends:
  //


  //
  // I/O:
  //
  // ...


protected:
  bool                          isStoreInFile_;
  bool                          isNeedTimeMark_;
  bool                          useFullDateFormat_;
  bool                          isMute_;
  QString                       dirName_;
  QString                       fileName_;
  QList<QString*>               spool_;
  quint32                       logFacilities_[4];
  int                           capacity_;
  double                        facilitiesSerialNumber_;
  QMap<QString, SgLogger*>      logSupplements_;

  virtual void makeOutput(LogLevel level, const QString &s, bool isAsync);
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* geo_MJD inline members:                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:






// A destructor:



//
// INTERFACES:
//




//
// FUNCTIONS:
//
//
//




//
// FRIENDS:
//
//
//



/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif //SG_LOGGER_H

