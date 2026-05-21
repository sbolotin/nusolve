/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2017-2020 Sergei Bolotin.
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

#ifndef NS_SCR_PRX_4_LOGGER_H
#define NS_SCR_PRX_4_LOGGER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtCore/QString>

#if 0 < HAVE_SCRIPTS
#    include <QtScript/QScriptEngine>
#    include <QtScript/QScriptValue>
#endif

#include <SgLogger.h>

#include "nuSolve.h"



class QString;

extern const QString            str_WRONG;

extern const QString            str_LogErr;
extern const QString            str_LogWrn;
extern const QString            str_LogInf;
extern const QString            str_LogDbg;



extern const QString            str_LogIoBin;
extern const QString            str_LogIoTxt;
extern const QString            str_LogIoNcdf;
extern const QString            str_LogIoDbh;
extern const QString            str_LogIo;

extern const QString            str_LogMatrix;
extern const QString            str_LogMatrix3D;
extern const QString            str_LogInterp;
extern const QString            str_LogMath;

extern const QString            str_LogObs;
extern const QString            str_LogStation;
extern const QString            str_LogSource;
extern const QString            str_LogSession;
extern const QString            str_LogData;

extern const QString            str_LogRefFrame;
extern const QString            str_LogTime;
extern const QString            str_LogIono;
extern const QString            str_LogRefraction;
extern const QString            str_LogDelay;
extern const QString            str_LogRate;
extern const QString            str_LogFlyBy;
extern const QString            str_LogDisplacement;
extern const QString            str_LogGeo;

extern const QString            str_LogEstimator;
extern const QString            str_LogPwl;
extern const QString            str_LogStoch;
extern const QString            str_LogConfig;
extern const QString            str_LogGui;
extern const QString            str_LogReport;
extern const QString            str_LogRun;
extern const QString            str_LogPreproc;
extern const QString            str_LogAll;


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsScrPrx4Logger : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString fileName
    WRITE setFileName READ getFileName)
  Q_PROPERTY(QString dirName
    WRITE setDirName READ getDirName)
  Q_PROPERTY(bool have2store
    WRITE setHave2store READ getHave2store)
  Q_PROPERTY(bool isMute
    WRITE setIsMute READ getIsMute)
  Q_PROPERTY(int returnCode
    WRITE setReturnCode READ getReturnCode)
  
  Q_ENUMS (LogLevel LogFacility)


public:
  enum LogLevel
  {
    Err = SgLogger::ERR,
    Wrn = SgLogger::WRN,
    Inf = SgLogger::INF,
    Dbg = SgLogger::DBG
  };

  enum LogFacility
  {
    IoBin       = SgLogger::IO_BIN,
    IoTxt       = SgLogger::IO_TXT,
    IoNcdf      = SgLogger::IO_NCDF,
    IoDbh       = SgLogger::IO_DBH,
    Io          = SgLogger::IO,
    Matrix      = SgLogger::MATRIX,
    Matrix3d    = SgLogger::MATRIX3D,
    Interp      = SgLogger::INTERP,
    Math        = SgLogger::MATH,
    Obs         = SgLogger::OBS,
    Station     = SgLogger::STATION,
    Source      = SgLogger::SOURCE,
    Session     = SgLogger::SESSION,
    Data        = SgLogger::DATA,
    RefFrame    = SgLogger::REF_FRAME,
    Time        = SgLogger::TIME,
    Iono        = SgLogger::IONO,
    Refraction  = SgLogger::REFRACTION,
    Delay       = SgLogger::DELAY,
    Rate        = SgLogger::RATE,
    FlyBy       = SgLogger::FLY_BY,
    Displacement= SgLogger::DISPLACEMENT,
    Geo         = SgLogger::GEO,
    Estimator   = SgLogger::ESTIMATOR,
    Pwl         = SgLogger::PWL,
    Stoch       = SgLogger::STOCH,
    Config      = SgLogger::CONFIG,
    Gui         = SgLogger::GUI,
    Report      = SgLogger::REPORT,
    Run         = SgLogger::RUN,
    Preproc     = SgLogger::PREPROC,
    All         = SgLogger::ALL
  };


  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsScrPrx4Logger(SgLogger* logger);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsScrPrx4Logger();


  //
  // Interfaces:
  //
public slots:
  inline const QString& getFileName() const {return logger_->getFileName();}
  inline const QString& getDirName() const {return logger_->getDirName();}
  inline bool getHave2store() const {return logger_->getIsStoreInFile();}
  inline bool getIsMute() const {return logger_->getIsMute();}
  inline int getReturnCode() const {return returnCode_;}

  inline quint32 getLogFacility(LogLevel lvl) const 
    {return (logger_!=NULL)?logger_->getLogFacility((SgLogger::LogLevel)lvl):0;};

  inline void setFileName(const QString& fname) {logger_->setFileName(fname);};
  inline void setDirName(const QString& dname) {logger_->setDirName(dname);};
  inline void setHave2store(bool have2) {logger_->setIsStoreInFile(have2);};
  inline void setIsMute(bool is) {logger_->setIsMute(is);};
  inline void setReturnCode(int rc) {returnCode_ = rc;};

  //
  inline void write(LogLevel lvl, quint32 f, const QString &s) 
    {if (logger_) logger_->write((SgLogger::LogLevel)lvl, f, s);};
  inline void addLogFacility(LogLevel lvl, quint32 f)
    {if (logger_) logger_->addLogFacility((SgLogger::LogLevel)lvl, f);};
  inline void delLogFacility(LogLevel lvl, quint32 f)
    {if (logger_) logger_->delLogFacility((SgLogger::LogLevel)lvl, f);};
  inline void selLogFacility(LogLevel lvl, quint32 f)
    {if (logger_) logger_->setLogFacility((SgLogger::LogLevel)lvl, f);};
  inline bool isEligible(LogLevel lvl, quint32 f) const
    {return logger_?logger_->isEligible((SgLogger::LogLevel)lvl, f):false;};
  
  inline void rmLogFile() {if (logger_) logger_->rmLogFile();};
  inline void clearSpool() {if (logger_) logger_->clearSpool();};



public:
  //
  // Functions:
  //
  inline QString className() const {return "NsScrPrx4Logger";};


  //
  // Friends:
  //


  //
  // I/O:
  //
  //

protected:
  SgLogger                     *logger_;
  int                           returnCode_; 
};
/*=====================================================================================================*/



#if 0 < HAVE_SCRIPTS

Q_DECLARE_METATYPE(NsScrPrx4Logger::LogLevel);
Q_DECLARE_METATYPE(NsScrPrx4Logger::LogFacility);

#endif


/*=====================================================================================================*/
//
// aux functions:
//
#if 0 < HAVE_SCRIPTS

#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4LogLevel(QScriptEngine */*eng*/, const NsScrPrx4Logger::LogLevel &l)
{
  return
    l==NsScrPrx4Logger::Err?str_LogErr:
      (l==NsScrPrx4Logger::Wrn?str_LogWrn:
        (l==NsScrPrx4Logger::Inf?str_LogInf:
          (l==NsScrPrx4Logger::Dbg?str_LogDbg:str_WRONG)));
};
#else
inline QScriptValue toScriptValue4LogLevel(QScriptEngine *eng, const NsScrPrx4Logger::LogLevel &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4LogLevel(const QScriptValue &obj, NsScrPrx4Logger::LogLevel &l)
{
  l = (NsScrPrx4Logger::LogLevel) obj.toInt32();
};





#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4LogFacility(QScriptEngine */*eng*/,
  const NsScrPrx4Logger::LogFacility &f)
{
  return 
    f==NsScrPrx4Logger::IoBin?str_LogIoBin:
      (f==NsScrPrx4Logger::IoTxt?str_LogIoTxt:
        (f==NsScrPrx4Logger::IoNcdf?str_LogIoNcdf:
          (f==NsScrPrx4Logger::IoDbh?str_LogIoDbh:
            (f==NsScrPrx4Logger::Io?str_LogIo:
  (f==NsScrPrx4Logger::Matrix?str_LogMatrix:
    (f==NsScrPrx4Logger::Matrix3d?str_LogMatrix3D:
      (f==NsScrPrx4Logger::Interp?str_LogInterp:
        (f==NsScrPrx4Logger::Math?str_LogMath:
  (f==NsScrPrx4Logger::Obs?str_LogObs:
    (f==NsScrPrx4Logger::Station?str_LogStation:
      (f==NsScrPrx4Logger::Source?str_LogSource:
        (f==NsScrPrx4Logger::Session?str_LogSession:
          (f==NsScrPrx4Logger::Data?str_LogData:
  (f==NsScrPrx4Logger::RefFrame?str_LogRefFrame:
    (f==NsScrPrx4Logger::Iono?str_LogIo:
      (f==NsScrPrx4Logger::Refraction?str_LogRefraction:
        (f==NsScrPrx4Logger::Delay?str_LogDelay:
          (f==NsScrPrx4Logger::Rate?str_LogRate:
            (f==NsScrPrx4Logger::FlyBy?str_LogFlyBy:
              (f==NsScrPrx4Logger::Displacement?str_LogDisplacement:
                (f==NsScrPrx4Logger::Geo?str_LogGeo:
  (f==NsScrPrx4Logger::Estimator?str_LogEstimator:
    (f==NsScrPrx4Logger::Pwl?str_LogPwl:
      (f==NsScrPrx4Logger::Stoch?str_LogStoch:
        (f==NsScrPrx4Logger::Config?str_LogConfig:
          (f==NsScrPrx4Logger::Gui?str_LogGui:
            (f==NsScrPrx4Logger::Report?str_LogReport:
              (f==NsScrPrx4Logger::Run?str_LogRun:
                (f==NsScrPrx4Logger::Preproc?str_LogPreproc:
                  (f==NsScrPrx4Logger::All?str_LogAll:str_WRONG)
      )))))))))))))))))))))))))))));
};
#else
inline QScriptValue toScriptValue4LogFacility(QScriptEngine *eng,  const NsScrPrx4Logger::LogFacility &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4LogFacility(const QScriptValue &obj, NsScrPrx4Logger::LogFacility &f)
{
  f = (NsScrPrx4Logger::LogFacility) obj.toInt32();
};


#endif // SCRIPT_SUPPORT


/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_LOGGER_H
