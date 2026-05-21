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

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTextStream>


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>



#include <SgIoAgvDriver.h>


/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/
//


/*=====================================================================================================*/
//
bool SgVlbiSession::putDataIntoAgvFile(const QString& dirName, const QString& fileName,
  SgIdentities *ids, SgVersion *driverVersion)
{
  bool                          isOk;
  SgMJD                         startEpoch(SgMJD::currentMJD());
  SgAgvDriver                   agvDriver(ids, driverVersion);

  agvDriver.setSession(this);
  isOk = agvDriver.exportData(dirName, fileName.size()?fileName:getName());

  SgMJD                         finisEpoch(SgMJD::currentMJD());
  
  if (isOk)
    logger->write(SgLogger::INF, SgLogger::IO_NCDF | SgLogger::DATA, className() +
      "::putDataIntoAgvFile(): the session " + name_ + " has been exported in an AGV file \"" +
      agvDriver.fileName() + "\" in directory \"" + agvDriver.path2File() +
      "\"; elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
  else
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF | SgLogger::DATA, className() +
      "::putDataIntoAgvFile(): export of the session \"" + name_ + 
      "\" in AGV format has failed; file: \"" + agvDriver.fileName() + 
      "\", directory: \"" + agvDriver.path2File() + "\"");

  return isOk;
};



//
bool SgVlbiSession::getDataFromAgvFile(const QString& fileName, SgIdentities *ids, 
  SgVersion *driverVersion, const QStringList& masterfileSuffixes)
{
  bool                           isOk;
  SgMJD                         startEpoch(SgMJD::currentMJD());
  SgAgvDriver                   agvDriver(ids, driverVersion);


  agvDriver.setSession(this);
  if ((isOk = agvDriver.importData(fileName)))
    addAttr(Attr_FF_CREATED);
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      ": getDataFromAgvFile(): import of AGV file \"" + fileName + "\" has failed");

  if (bandByKey_.size() == 0)
    isOk = false;
  if (observations_.size() == 0)
    isOk = false;
    
  //isOk = isOk && selfCheck(guiExpected);
  isOk = isOk && selfCheck(false, masterfileSuffixes);
  //
  SgMJD                         finisEpoch(SgMJD::currentMJD());
  if (isOk)
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      ": getDataFromAgvFile(): the session " + getName() + "  has been read from vgosDb data set" + 
      ", elapsed time: " + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
 
  return isOk;
};

/*=====================================================================================================*/





/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/

/*=====================================================================================================*/
