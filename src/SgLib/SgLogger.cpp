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


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>



#include <SgLogger.h>
#include <SgMJD.h>


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
SgLogger::SgLogger(int capacity, bool isStoreInFile, const QString& fileName) :
  dirName_ (""),
  fileName_(fileName),
  logSupplements_()
{
  capacity_     = capacity;
  isStoreInFile_= isStoreInFile;
  
  logFacilities_[ERR] = 0xFFFFFFFF;
  logFacilities_[WRN] = 0xFFFFFFFF;
  logFacilities_[INF] = 0xFFFFFFFF;
  logFacilities_[DBG] = IO | DATA | ESTIMATOR | CONFIG | RUN | PREPROC;
  isNeedTimeMark_ = true;
  useFullDateFormat_ = false;
//  useFullDateFormat_ = true;
  isMute_ = false;
  facilitiesSerialNumber_ = 20120815.0;
};



//
SgLogger::~SgLogger() 
{
  if (spool_.size())
    clearSpool();
  if (logSupplements_.size())
    logSupplements_.clear(); // we are not an owner
  while (!spool_.isEmpty())
    delete spool_.takeFirst();
};



//
void SgLogger::startUp()
{
  //just explain:
  QString                       tmp;
  tmp.sprintf("Capacity: %d", capacity_);
  if (isStoreInFile_)
    tmp += "; log file name: \"" + fileName_ + "\"";
  write(DBG, IO_TXT, className() + ": started with parameters: " + tmp);
};



// all objects should use SgLogger::write(level, facility, const QString&)
// to log something
void SgLogger::write(LogLevel level, quint32 facility, const QString &s, bool isAsync)
{
  if (logSupplements_.size())
    for (QMap<QString, SgLogger*>::iterator it=logSupplements_.begin(); it!=logSupplements_.end(); ++it)
      it.value()->write(level, facility, s, isAsync);
  if (isEligible(level, facility))
  {
    QString                     str("");
    if (isNeedTimeMark_)
//      str = SgMJD::currentMJD().toString( useFullDateFormat_?SgMJD::F_RFC2822:SgMJD::F_HHMMSS ) + " ";
//    str = SgMJD::currentMJD().toString( useFullDateFormat_?SgMJD::F_ISO:SgMJD::F_HHMMSS ) + " ";
    str = SgMJD::currentMJD().toString(useFullDateFormat_?SgMJD::F_YYYYMMDDHHMMSSSS:SgMJD::F_HHMMSS) + 
      " ";
    if (level == ERR)
      str += "ERROR: ";
    if (level == WRN)
      str += "Warning: ";
    str += s;
    spool_.append(new QString(str));
    makeOutput(level, str, isAsync);
    if (spool_.count() == capacity_)
      clearSpool();
  };
};



//
void SgLogger::clearSpool() 
{
  if (isStoreInFile_)
  {
    QFile                       file2write(dirName_.isEmpty()?fileName_:dirName_ + "/" + fileName_);
    // check the path:
    if (!dirName_.isEmpty())
    {
      QDir                      dir(dirName_);
      if (!dir.exists())
//      if (!dir.mkpath(dirName_))
        if (!dir.mkpath(dir.absolutePath()))
          std::cout << "SgLogger::clearSpool(): cannot create direcory \"" << qPrintable(dirName_) 
                    << "\"\n";
    };
    if (file2write.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
    {
      QTextStream               ts(&file2write);
      for (int i=0; i<spool_.size(); i++)
        ts << *spool_.at(i) << endl;
      ts.setDevice(NULL);
      file2write.close();
    }
    else
      std::cout << "SgLogger::clearSpool(): cannot open log file \"" << file2write.error() << "\"\n";
  };
  while (!spool_.isEmpty())
    delete spool_.takeFirst();
};



//
void SgLogger::rmLogFile()
{
//  if (isStoreInFile_) //
//  {
  QString                     target(dirName_.isEmpty() ? fileName_ : dirName_ + "/" + fileName_);
  if (QFile::exists(target))
    if (!QFile::remove(target))
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::rmLogFile(): cannot remove the file \"" + target + "\"");
//  };
};



//
void SgLogger::makeOutput(LogLevel, const QString &s, bool)
{
  if (!isMute_)
    std::cerr << qPrintable(s) << "\n";
};



//
void SgLogger::attachSupplementLog(const QString& name, SgLogger *auxLogger)
{
  if (!auxLogger)
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::attachSupplementLog(): the log \"" + name + "\" is NULL");
  else if (logSupplements_.contains(name))
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::attachSupplementLog(): the log \"" + name + "\" was already added to the map");
  else
    logSupplements_.insert(name, auxLogger);
};



//
void SgLogger::detachSupplementLog(const QString& name)
{
  if (logSupplements_.contains(name))
    logSupplements_.remove(name);
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::detachSupplementLog(): the log \"" + name + "\" is not in the map");
};



//
SgLogger* SgLogger::lookupSupplementLog(const QString& name)
{
  return logSupplements_.contains(name)?logSupplements_[name]:NULL;
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
//
// constants:
//
SgLogger  basicLogger;
//SgLogger *logger=new SgLogger();
SgLogger *logger = &basicLogger;


/*=====================================================================================================*/
