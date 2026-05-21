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
#include <QtCore/QList>


#include <SgVlbiSession.h>

#include <SgLogger.h>
#include <SgNetworkStnRecord.h>
#include <SgStnLogCollector.h>









/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
bool SgVlbiSession::importDataFromLogFiles(const QString& path2sessions,
  const QList<QString>& stns, const QList<QString>& kinds,
  const QMap<QString, int>* defaultCableSignByStn,
  const QMap<QString, QString>* rinexFileNameByStn,
  const QMap<QString, double>* rinexPressureOffsetByStn,
  bool have2overwriteAntabFile, bool reportAllTsysData,
  const QString& altPath2InputFiles,
  const QString& orderOfMeteo)
{
  QMap<QString, QString>        logFileByKey;
  QMap<QString, QString>        cdmsFileByKey;
  QMap<QString, QString>        pcmtFileByKey;
  QMap<QString, QString>        metFileByKey;
  QList<SgVlbiStationInfo*>     stations;
  QString                       path2logFiles("");
  QString                       sessCode(sessionCode_.toLower());
  QString                       str("");
  QString                       stnKey("");
  int                           nYear=tStart_.calcYear();
  bool                          haveFoundSomething=false;
  bool                          haveVlbaLog=false;
  bool                          doLogs=true, doCdms=true, doPcmt=true, doMets=true;
  SgNetworkStations             ns_codes(path2Masterfile_);
  ns_codes.readFile();
  //
  if (kinds.size())
  {
    doLogs = doCdms = doPcmt = doMets = false;
    for (int i=0; i<kinds.size(); i++)
      if (kinds.at(i) == "log")
        doLogs = true;
      else if (kinds.at(i) == "cdms")
        doCdms = true;
      else if (kinds.at(i) == "pcmt")
        doPcmt = true;
      else if (kinds.at(i) == "met")
        doMets = true;
  };
  //
  if (altPath2InputFiles.size())
    path2logFiles = altPath2InputFiles;
  else
    path2logFiles = path2sessions + "/" + QString("").sprintf("%04d", nYear) + "/" + sessCode + "/";
  //
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::importDataFromLogFiles(): checking [" + path2logFiles + "] directory for station log files");
  // reset the flag:
  delAttr(SgVlbiSession::Attr_FF_AUX_OBS_MODIFIED);
  //
  //
  if (stns.size())
  {
    for (int i=0; i<stns.size(); i++)
    {
      stnInpt2Key(stns.at(i), ns_codes, stnKey);
      if (stnKey.size())
      {
        if (stationsByName_.contains(stnKey))
          stations.append(stationsByName_.value(stnKey));
        else
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): cannot find station " + stnKey + " in the session");
      };
    };
  }
  else
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      stations.append(it.value());
  //
  //
  //
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::importDataFromLogFiles(): collected " + QString("").setNum(stations.size()) + 
    " stations to process");
  //
  QDir                          dir(path2logFiles);
  QStringList                   patterns;
  QStringList                   fileList;
  for (int i=0; i<stations.size(); i++)
  {
    SgVlbiStationInfo          *stn=stations.at(i);
    //
    if (ns_codes.recsByName().contains(stn->getKey()))
    {
      // can be more than one in the list, check it later:
      const SgNetworkStnRecord &rec=ns_codes.recsByName().find(stn->getKey()).value();
      if (rec.isValid())
      {
        const QString&          sCode=rec.getCode();
        stn->setSid(sCode.at(0).toLatin1(), sCode.at(1).toLatin1());
        //
        if (doLogs)
        {
          str = path2logFiles + sessCode + sCode.toLower() + ".log";
          if (QFile::exists(str))
            logFileByKey[stn->getKey()] = str;
        };
        //
        //
        // VGOS produced data:
        // CDMS:
        if (doCdms)
        {
          str = path2logFiles + sessCode + sCode.toLower() + ".cdms.dat";
          if (QFile::exists(str))
            cdmsFileByKey[stn->getKey()] = str;
        };
        //
        //
        // PCMT:
        if (doPcmt)
        {
          str = sessCode + sCode.toLower() + ".pcmt.*.*.dat";
          patterns.clear();
          patterns << str;
          fileList = dir.entryList(patterns,
            QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Time);
          if (fileList.size())
          {
            str = fileList.at(0);
            pcmtFileByKey[stn->getKey()] = path2logFiles + str;
            // complain:
            if (fileList.size() > 1)
              logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
                "::importDataFromLogFiles(): more than one PCMT file found for the station " + 
                stn->name() + ", the last modified will be used: " + path2logFiles + str);
          };
        };
        //
        //
        // external meteo data:
        if (doMets)
        {
          str = path2logFiles + sessCode + sCode.toLower() + ".met.dat";
          if (QFile::exists(str))
            metFileByKey[stn->getKey()] = str;
        };
      }
      else
        logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::importDataFromLogFiles(): the ns-codes.txt record for the station " + stn->name() +
          " is not valid");
    }
    else
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): cannot find station " + stn->name() +
        " in the ns-codes.txt file");
  };
  //
  //
  //
  // check for existance of a VLBA log file:
  SgVlbaLogCollector            vlbaLogCollector;
  str = path2logFiles + sessCode + "cal.vlba";
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::importDataFromLogFiles(): checking " + str + " log file for VLBA stations");
  if (QFile::exists(str))
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): found the log file for VLBA stations");
    haveVlbaLog = vlbaLogCollector.readLogFile(str, nYear);
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): the log file for VLBA stations was not found");
  //
  //
  //
  if (logFileByKey.size() != stationsByName_.size()) // something missed, make additional search:
    search4missedLogFiles(logFileByKey, ns_codes);
  //
  //
  //
  if (logFileByKey.size())
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): collected " + QString("").setNum(logFileByKey.size()) + 
      " log file" + QString(logFileByKey.size()>1?"s":""));
  if (cdmsFileByKey.size())
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): collected " + QString("").setNum(cdmsFileByKey.size()) + 
      " CDMS file" + QString(cdmsFileByKey.size()>1?"s":""));
  if (pcmtFileByKey.size())
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): collected " + QString("").setNum(pcmtFileByKey.size()) + 
      " PCMT file" + QString(pcmtFileByKey.size()>1?"s":""));
  if (metFileByKey.size())
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::importDataFromLogFiles(): collected " + QString("").setNum(metFileByKey.size()) + 
      " file" + QString(metFileByKey.size()>1?"s":"") + " with external meteorological data");
  //
  //
  //
  // First, check for aux (vgos) cal files:
  //
  // CDMS:
  for (QMap<QString, QString>::iterator it=cdmsFileByKey.begin(); it!=cdmsFileByKey.end(); ++it)
  {
    SgVlbiStationInfo          *stn=stationsByName_.value(it.key());
    if (!stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
    {
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): processing " + it.value() + 
        ", a CDMS file for " + stn->name() + " station");
      SgStnLogCollector         logCollector;
      if (logCollector.readLogFile(it.value(), stn->name(), tStart_, tFinis_, ""))
      {
        if (logCollector.propagateData(stn, false, have2overwriteAntabFile, false))
        {
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation successfully completed for " + stn->name());
          haveFoundSomething = true;
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
          {
            stn->setCableCalsOriginTxt("Data extracted from the file " + it.value());
            stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_CDMS);
          };
        }
        else
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation has failed for " + stn->name());
      }
      else
        logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::importDataFromLogFiles(): reading of " + it.value() + " failed");
    }
    else
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): the station " + stn->name() + 
        " already has cable calibration data, the file " + it.value() + " is skipped");
  };
  //
  // External meteo data:
  for (QMap<QString, QString>::iterator it=metFileByKey.begin(); it!=metFileByKey.end(); ++it)
  {
    SgVlbiStationInfo          *stn=stationsByName_.value(it.key());
    if (!stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO))
    {
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): processing " + it.value() + 
        " file with external meteo data for " + stn->name() + " station");
      SgStnLogCollector         logCollector;
      if (logCollector.readLogFile(it.value(), stn->name(), tStart_, tFinis_, ""))
      {
        if (logCollector.propagateData(stn, false, have2overwriteAntabFile, false))
        {
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation successfully completed for " + stn->name());
          haveFoundSomething = true;
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO))
          {
            stn->setMeteoDataOriginTxt("Data extracted from the file " + it.value());
            stn->setMeteoDataOrigin(SgVlbiStationInfo::MDO_EXTERNAL);
          };
        }
        else
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation has failed for " + stn->name());
      }
      else
        logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::importDataFromLogFiles(): reading of " + it.value() + " failed");
    }
    else
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): the station " + stn->name() + 
        " already has meteo data, the file " + it.value() + " is skipped");
  };
  //
  //
  SgChannelSkeded               psf;
  psf.parseSkdFile(path2logFiles + sessCode + ".skd");
  
  // FS Logs:  
  // Import data from the existing FS log files of regular stations:
  for (QMap<QString, QString>::iterator it=logFileByKey.begin(); it!=logFileByKey.end(); ++it)
  {
    SgVlbiStationInfo          *stn=stationsByName_.value(it.key());
    if (!stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL) || 
        !stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO)     ||
        !stn->isAttr(SgVlbiStationInfo::Attr_HAS_TSYS)        )
    {
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): processing " + it.value() + 
        ", a log file for " + stn->name() + " station");
      // clear the attributes:
      SgStnLogCollector         logCollector;
      // set deafult cable sign map:
      logCollector.setDefaultCableSignByStn(defaultCableSignByStn);
      logCollector.setRinexFileNameByStn(rinexFileNameByStn);
      logCollector.setRinexPressureOffsetByStn(rinexPressureOffsetByStn);
      logCollector.setChannelSkeded(&psf);
      // it should speed up a little bit:
//    logCollector.setAntcalOutputData(AOD_TRADITIONAL | AOD_TSYS);
      logCollector.setAntcalOutputData(AOD_TRADITIONAL);
//
//    4 debug:
//    logCollector.setUseSkededChannelSetup(true);
      //
      //
      if (logCollector.readLogFile(it.value(), stn->name(), tStart_, tFinis_, orderOfMeteo))
      {
        if (logCollector.propagateData(stn, 
            // true,
            have2overwriteAntabFile, // temporary
            have2overwriteAntabFile, reportAllTsysData))
        {
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation successfully completed for " + stn->name());
          haveFoundSomething = true;
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
          {
            stn->setCableCalsOriginTxt("Data extracted from the file " + it.value());
            if (stn->isAttr(SgVlbiStationInfo::Attr_CABLE_CAL_IS_CDMS))
              stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_CDMS_FS_LOG);
            else
              stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_FS_LOG);
          };
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO))
          {
            stn->setMeteoDataOriginTxt("Data extracted from the file " + it.value());
            stn->setMeteoDataOrigin(SgVlbiStationInfo::MDO_FS_LOG);
          };
        }
        else
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation has failed for " + stn->name());
      }
      else
        logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::importDataFromLogFiles(): reading of " + it.value() + " failed");
    }
    else
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): the station " + stn->name() + 
        " already has all data, the file " + it.value() + " is skipped");
  };
  //
  // PCMT:
  for (QMap<QString, QString>::iterator it=pcmtFileByKey.begin(); it!=pcmtFileByKey.end(); ++it)
  {
    SgVlbiStationInfo          *stn=stationsByName_.value(it.key());
    if (!stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
    {
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): processing " + it.value() + 
        ", a PCMT file for " + stn->name() + " station");
      SgStnLogCollector         logCollector;
      if (logCollector.readLogFile(it.value(), stn->name(), tStart_, tFinis_, ""))
      {
        if (logCollector.propagateData(stn, false, have2overwriteAntabFile, false))
        {
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation successfully completed for " + stn->name());
          haveFoundSomething = true;
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
          {
            stn->setCableCalsOriginTxt("Data extracted from the file " + it.value());
            stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_PCMT);
          };
        }
        else
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation has failed for " + stn->name());
      }
      else
        logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::importDataFromLogFiles(): reading of " + it.value() + " failed");
    }
    else
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::importDataFromLogFiles(): the station " + stn->name() + 
        " already has cable calibration data, the file " + it.value() + " is skipped");
  };
  //
  //
  //
  //
  //
  // import data from the VLBA log file, if it is exists:
  if (haveVlbaLog)
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *stn=it.value();
      if (vlbaLogCollector.isVlba(stn->getKey()))
      {
        if (vlbaLogCollector.propagateData(stn))
        {
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation successfully completed for " + stn->name());
          haveFoundSomething = true;
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
          {
            stn->setCableCalsOriginTxt("Data extracted from the file " +
              vlbaLogCollector.getLogFileName());
            stn->setCableCalsOrigin(SgVlbiStationInfo::CCO_FS_LOG);
          };
          if (stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO))
          {
            stn->setMeteoDataOriginTxt("Data extracted from the file " + 
              vlbaLogCollector.getLogFileName());
            stn->setMeteoDataOrigin(SgVlbiStationInfo::MDO_FS_LOG);
          };
        }
        else
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::importDataFromLogFiles(): data propagation has failed for " + stn->name());
      };
    };
  //
  if (haveFoundSomething)
    addAttr(SgVlbiSession::Attr_FF_AUX_OBS_MODIFIED);
  //
  //
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::importDataFromLogFiles(): import of data from station log files has been finished, " +
    (haveFoundSomething?"got some new information":"nothing useful found"));
  return haveFoundSomething;
};





//
void SgVlbiSession::search4missedLogFiles(QMap<QString, QString>& /*logFileByKey*/, 
  const SgNetworkStations &/*ns_codes*/)
{
  
};



//
bool SgVlbiSession::resetDataFromLogFiles(bool doCable, bool doMeteo, bool doTsys, 
  const QList<QString>& stns)
{
  if (!(doCable || doMeteo || doTsys))
    return false;
  //
  //
  QList<SgVlbiStationInfo*>     stations;
  SgNetworkStations             ns_codes(path2Masterfile_);
  bool                          ns_codes_read(false);
  QString                       stnKey("");
  //
  if (stns.size())
  {
    for (int i=0; i<stns.size(); i++)
    {
      if (stns.at(i).size()==2 && !ns_codes_read) // do not need to read the file if it is not a 2-char id
      {
        ns_codes.readFile();
        ns_codes_read = true;
      };
      stnInpt2Key(stns.at(i), ns_codes, stnKey);
      if (stnKey.size())
      {
        if (stationsByName_.contains(stnKey))
          stations.append(stationsByName_.value(stnKey));
        else
          logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
            "::resetDataFromLogFiles(): cannot find station " + stnKey + " in the session");
      };
    };
  }
  else
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
      stations.append(it.value());
  //
  //
  if (!stations.size())
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::resetDataFromLogFiles(): no station found to process");
    return false;
  }
  else 
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::resetDataFromLogFiles(): collected " + QString("").setNum(stations.size()) + 
      " stations to process");
  //
  //
  //
  for (int i=0; i<stations.size(); i++)
  {
    SgVlbiStationInfo          *stn=stations.at(i);
    if (doCable)
    {
      stn->resetCable();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::resetDataFromLogFiles(): the cable calibration corrections were set to zeros for station " +
        stn->getKey());
    };
    if (doMeteo)
    {
      stn->resetMeteo();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::resetDataFromLogFiles(): the meteorological data were set to zeros for station " + 
        stn->getKey());
    };
    if (doTsys)
    {
      stn->resetTsys();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::resetDataFromLogFiles(): the meteorological data were set to zeros for station " + 
        stn->getKey());
    };
  };
  addAttr(Attr_FF_AUX_OBS_MODIFIED);
  return true;
};



//
void SgVlbiSession::stnInpt2Key(const QString& stnNameInpt, SgNetworkStations& ns_codes, QString& key)
{
  QString                       str(stnNameInpt);
  key = "";
  if (str.size() == 2) // have to check ns-codes:
  {
    if (ns_codes.recsById().contains(str))
    {
      key = ns_codes.recsById().value(str).getName();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::stnInpt2Key(): found a station name " + key + " for the code \"" + str + "\"");
    }
    else if (str.at(0).isLower())
    {
      str[0] = str.at(0).toUpper();
      if (ns_codes.recsById().contains(str))
      {
        key = ns_codes.recsById().value(str).getName();
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::stnInpt2Key(): found a station name " + key + " for the code \"" + str + "\"");
      }
      else if (str.at(1).isUpper())
      {
        str[1] = str.at(1).toLower();
        if (ns_codes.recsById().contains(str))
        {
          key = ns_codes.recsById().value(str).getName();
          logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
            "::stnInpt2Key(): found a station name " + key + " for the code \"" + str + "\"");
        }
        else
          logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
            "::stnInpt2Key(): cannot find a station name for the code \"" + str + "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
          "::stnInpt2Key(): cannot find a station name for the code \"" + str + "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
        "::stnInpt2Key(): cannot find a station name for the code \"" + str + "\"");
  }
  else if (str.size() > 2)
    key = stnNameInpt.leftJustified(8, ' ', true);
};
/*=====================================================================================================*/


 


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

/*=====================================================================================================*/
