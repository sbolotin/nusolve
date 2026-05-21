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
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>


#include <SgVlbiSession.h>

#include <SgLogger.h>




/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/
extern const QString            sStationCalibrationList[]= 
  { "CABL DEL", "UNPHASCL", "NDRYCONT", "NWETCONT", "        ", "WVR DELY"  };

extern const QString            sFlybyCalibrationList[]=
  { "CFAKBDRY", "CFAKBWET", "CFAJJDRY", "IFADRFLY", "IFADRYSS", "MTTDRFLY", "MTTDRYSS", "NMFDRFLY"  };

extern const QString            sObsCalibrationList_v10[]=
  { "PTD CONT", "WOBXCONT", "WOBYCONT", "ETD CONT", "OCE CONT", "PTOLDCON", 
    "UT1ORTHO", "WOBORTHO", "WOBNUTAT", "FEED.COR", "WAHRCONT", "TILTRMVR"  };

extern const QString            sObsCalibrationList_v11[]=
  { "PTD CONT", "WOBXCONT", "WOBYCONT", "ETD CONT", "OCE CONT", "UT1ORTHO", "WOBORTHO", 
    "WOBLIBRA", "UT1LIBRA", "OPTLCONT", "FEED.COR", "OCE_OLD ", "TILTRMVR", "PTOLDCON"  };

extern const QString            sAntennaMountTypes[]=
  {"AZEL", "EQUA", "X_YN", "X_YE", "RICHMOND", "UNKN"};

enum ObsCal_v10_index
{
  O10_POLE_TIDE     =  0,
  O10_WOBBLE_X      =  1,
  O10_WOBBLE_Y      =  2,
  O10_EARTH_TIDE    =  3,
  O10_OCEAN_TIDE    =  4,
  O10_POLE_TIDE_OLD =  5,
  O10_UT1_ORTHO     =  6,
  O10_WOB_ORTHO     =  7,
  O10_WOB_NUTAT     =  8,
  O10_FEED_ROT      =  9,
  O10_WAHR_NUTAT    = 10,
  O10_TILT_RMVR     = 11,
};
enum ObsCal_v11_index
{
  O11_POLE_TIDE     =  0,
  O11_WOBBLE_X      =  1,
  O11_WOBBLE_Y      =  2,
  O11_EARTH_TIDE    =  3,
  O11_OCEAN_TIDE    =  4,
  O11_UT1_ORTHO     =  5,
  O11_WOB_ORTHO     =  6,
  O11_WOB_LIBRA     =  7,
  O11_UT1_LIBRA     =  8,
  O11_OCN_POLE_TIDE =  9,
  O11_FEED_ROT      = 10,
  O11_OCEAN_TIDE_OLD= 11,
  O11_TILT_RMVR     = 12,
  O11_POLE_TIDE_OLD = 13,
};



/*=======================================================================================================
*
*                           I/O support:
* 
*======================================================================================================*/
//
bool SgVlbiSession::guessWrapperFileNameBySession(const QString& inputArg, const QString& path2VgosDb,
  const QString& acAbbName, bool useOwnOnly,
  QString& path2wrapperFile, QString& wrapperFileName, QString& sessionName, int& version, bool noYears)
{
  // here inputArg is a session name with or without version part, e.g.:
  // 15AUG01XK_V004
  // 15AUG01XK
  //
  logger->write(SgLogger::DBG, SgLogger::SESSION, SgVlbiSession::className() +
    "::guessWrapperFileNameBySession(): processing: " + inputArg);
  bool                          isOk;
  int                           nn;
  QString                       vVer("");
  QString                       kKind("");
  QString                       iInst("");
  QString                       sYr("");
  //QRegExp                     reVer(".*_V([0-9]{3}).*");
  QRegularExpression            reVer(".*_V([0-9]{3}).*");
  QRegularExpression            reOldDbName("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName("^(\\d{8})-([^\\s_]{2,12}).*");
  QRegularExpressionMatch       match;
  version = -1;
  //
  // guess version number, if provided:
  if ((match=reVer.match(inputArg)).hasMatch())
  {
    nn = match.captured(1).toInt(&isOk);
    if (isOk)
    {
      version = nn;
      vVer.sprintf("_V%03d", version);
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): user provided version is: " + QString("").setNum(version));
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): cannot guess the version number form: \"" + inputArg + "\"");
      return false;
    };
  };

  if ((match=reOldDbName.match(inputArg)).hasMatch())         // dbName: 12JUL16XA
  {
    sessionName = match.captured(1);
    sYr = sessionName.left(2);
  }
  else if ((match=reNewDbName.match(inputArg)).hasMatch())
  {
    sessionName = match.captured(1) + "-" + match.captured(2);
    sYr = sessionName.left(4);
  };
  // 
  //
  path2wrapperFile = path2VgosDb;
  if (path2wrapperFile.right(1) == '/')
    path2wrapperFile.chop(1);
  //
  //
  if (!noYears)
  {
    nn = sYr.toInt();
    if (77 < nn && nn < 100)
      nn += 1900;
    else if (nn < 40)
      nn += 2000;
    sYr.sprintf("%4d", nn);
    if (path2wrapperFile.right(4) != sYr)
      path2wrapperFile += "/" + sYr;
  };
  path2wrapperFile += "/" + sessionName;
  //
  //
  if (!QDir(path2wrapperFile).exists())
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::guessWrapperFileNameBySession(): the input directory \"" + path2wrapperFile + 
      "\" does not exist");
    return false;
  }
  else
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::guessWrapperFileNameBySession(): the path to the wrapper file is: \"" + path2wrapperFile + 
      "\"");


  // look up the file:
  QStringList                   fltList;
  fltList << sessionName + vVer + "*.wrp";
  QStringList                   fList=QDir(path2wrapperFile).entryList(fltList, 
                                      QDir::Files | QDir::Readable, QDir::Name);
  if (fList.size() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::guessWrapperFileNameBySession(): no any wrapper file was found in \"" + path2wrapperFile + 
      "\"");
    return false;
  };
  //
  //
  if (useOwnOnly) // check for the institution:
  {       
    bool                        hasOursAc=false;
    QRegularExpression          reI(".*_i(" + acAbbName + ")(?:_|\\.).*");
    for (int i=0; i<fList.size(); i++)
      if ( (match=reI.match(fList.at(i))).hasMatch() )
        hasOursAc = true;
    if (hasOursAc) //
    {
      QStringList               tmpList;
      for (int i=0; i<fList.size(); i++)
        if ( (match=reI.match(fList.at(i))).hasMatch() )
          tmpList << fList.at(i);
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): selected " + QString("").setNum(tmpList.size()) +
        " entries of our own AC (" + acAbbName + ") from " + QString("").setNum(fList.size()) + " total");
      fList = tmpList;
    };
  };
  //
  // we have at least one entry:
  if (fList.size() == 1)
  {
    wrapperFileName = fList.at(0);
    if (version == -1)
    {
      if ((match=reVer.match(wrapperFileName)).hasMatch())
      {
        nn = match.captured(1).toInt(&isOk);
        if (isOk)
        {
          version = nn;
          logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
            "::guessWrapperFileNameBySession(): guessed version is: " + QString("").setNum(version));
        }
        else
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
            "::guessWrapperFileNameBySession(): cannot guess the version number form: \"" + 
            wrapperFileName + "\"");
          return false;
        };
      };
    };
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::guessWrapperFileNameBySession(): the wrapper file name is: \"" + wrapperFileName + "\"");
    return true;
  }
  else
  {
    if (version == -1)
    {
      // find out the largest version number (the list is sorted by name):
      QString                   str=fList.at(fList.size() - 1);
//    QRegExp                   reSn(".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2})_V([0-9]{3}).*");
      QRegularExpression        reSn("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}|\\d{8}-[\\S]{2,12})_V([0-9]{3}).*");

      QString                   sVer("");
//      if (-1 < reSn.indexIn(str))
      if ((match=reSn.match(str)).hasMatch())
      {
//std::cout << " +++ 3: "
//<< "match.capture(1) = [" << qPrintable(match.captured(1)) << "]  "
//<< "match.capture(2) = [" << qPrintable(match.captured(2)) << "]  "
//<< "\n";
        vVer = match.captured(2);
        bool                    isOk;
        int                     nn;
        nn = vVer.toInt(&isOk);
        if (isOk)
        {
          version = nn;
          vVer.sprintf("_V%03d", version);
          logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
            "::guessWrapperFileNameBySession(): guessed version is: " + QString("").setNum(version));
        }
        else
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
            "::guessWrapperFileNameBySession(): cannot guess a database version from the file \"" + 
            str + "\"");
          return false;
        };
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): cannot parse the string \"" + str + 
          "\", need to update the template");
        return false;
      };
      //
      fList.clear();
      fltList.clear();
      fltList << sessionName + vVer + "*.wrp";
      fList = QDir(path2wrapperFile).entryList(fltList, QDir::Files | QDir::Readable, QDir::Name);
      if (fList.size() == 1) // if only one file, stop searching:
      {
        wrapperFileName = fList.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + wrapperFileName + 
          "\"");
        return true;
      };
    };
    // ok, at this point all files have the same version.
    QStringList                 noK, kAll;
    for (int i=0; i<fList.size(); i++)
    {
      if (fList.at(i).contains("_kall", Qt::CaseInsensitive))
        kAll << fList.at(i);
      if (!fList.at(i).contains("_k", Qt::CaseInsensitive))
        noK << fList.at(i);
    };
    if (noK.size() == 1)
    {
      wrapperFileName = noK.at(0);
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + wrapperFileName + "\"");
      return true;
    }
    else if (kAll.size() == 1)
    {
      wrapperFileName = kAll.at(0);
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + wrapperFileName + "\"");
      return true;
    }
    else if (/*noK.size() == 0 &&*/ 1 < kAll.size()) // more than one _kall:
    {
      QStringList               noI, iUs, iIvs;
      QString                   usId("_i"), ivsId("_iIVS");
      usId += acAbbName;
      for (int i=0; i<kAll.size(); i++)
      {
        if (kAll.at(i).contains(usId, Qt::CaseInsensitive))
          iUs << kAll.at(i);
        if (kAll.at(i).contains(ivsId, Qt::CaseInsensitive))
          iIvs << kAll.at(i);

        if (!kAll.at(i).contains("_i", Qt::CaseInsensitive))
          noI << kAll.at(i);
      };
      if (iUs.size() == 1)
      {
        wrapperFileName = iUs.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      if (iIvs.size() == 1)
      {
        wrapperFileName = iIvs.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      else if (noI.size() == 1)
      {
        wrapperFileName = noI.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      else if (1 < iUs.size())
      {
        wrapperFileName = iUs.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      else if (1 < iIvs.size())
      {
        wrapperFileName = iIvs.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      else if (1 < noI.size())
      {
        wrapperFileName = noI.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name: \"" + 
          wrapperFileName + "\"");
        return true;
      }
      else
      {
        wrapperFileName = kAll.at(0);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::guessWrapperFileNameBySession(): guessed the wrapper file name (last resort): \"" + 
          wrapperFileName + "\"");
        return true;        
      };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::guessWrapperFileNameBySession(): cannot guess wrapper file name: noK.size()=" + 
        QString("").setNum(noK.size()) + ", kAll.size()=" + QString("").setNum(kAll.size()) );
      return false;
    };
  };
  //
  return true;
};



//
bool SgVlbiSession::guessSessionByWrapperFileName(const QString& inputArg, const QString& path2VgosDb,
    QString& path2wrapperFile, QString& wrapperFileName, QString& sessionName, int& version)
{
  logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
    "::guessSessionByWrapperFileName(): processing: " + inputArg);
  //
  // split input on path and filename:
  int                         idx=inputArg.lastIndexOf("/");
  if (idx == -1)
  {
    path2wrapperFile = ".";
    wrapperFileName = inputArg;
  }
  else if (idx == 0) // ?
  {
    path2wrapperFile = "/";
    wrapperFileName = inputArg.mid(1);
  }
  else
  {
    path2wrapperFile = inputArg.left(idx);
    wrapperFileName = inputArg.mid(idx + 1);
  };
  // add the default path:
  if (path2wrapperFile.at(0) != QChar('/') && path2wrapperFile.at(0) != QChar('.'))
  {
    path2wrapperFile = path2VgosDb + "/" + path2wrapperFile;
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::guessSessionByWrapperFileName(): the path to wrapper file has been modified to \"" + 
      path2wrapperFile + "\"");
  };
  //
  // check for existance:
  if (!QFile::exists((path2wrapperFile.size()?path2wrapperFile + "/" : "") + wrapperFileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::guessSessionByWrapperFileName(): the provided file, " + path2wrapperFile + "/" + 
      wrapperFileName + ", does not exist");
    return false;
  };
  //
  //
////QRegExp                     reSn(".*/([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z]{2})_V([0-9]{3}).*");
//  QRegExp                     reSn(".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2})_V([0-9]{3}).*");
  QString                       sVer("");
  QRegularExpression            reOldDbName(".*(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2})_V([0-9]{3}).*");
  QRegularExpression            reNewDbName(".*(\\d{8})-([\\S]{2,12})_V([0-9]{3}).*");
  QRegularExpressionMatch       match;
  
  if ((match=reOldDbName.match(inputArg)).hasMatch())         // dbName: 12JUL16XA
  {
    sessionName = match.captured(1);
    sVer        = match.captured(2);
  }
  else if ((match=reNewDbName.match(inputArg)).hasMatch())    // dbName: 20120716-r1542
  {
    sessionName = match.captured(1) + "-" + match.captured(2);
    sVer        = match.captured(3);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::guessSessionByWrapperFileName(): cannot guess a database name from the string \"" + 
      inputArg + "\"");
    return false;
  };
  bool                          isOk;
  int                           v;
  v = sVer.toInt(&isOk);
  if (isOk)
    version = v;
  else
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::guessSessionByWrapperFileName(): cannot guess a database version from the string \"" + 
      inputArg + "\"");
  //
  return true;
};



//
void SgVlbiSession::applyObsCalibrationSetup(int bitFlags, const QList<QString> &calList)
{
  int                           num=calList.size();
  if ((calcInfo_.getDversion()<11.0 && num!=12) ||
      (calcInfo_.getDversion()>=11.0 && num!=14)  )
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::applyObsCalibrationSetup(): for the version " + QString("").setNum(calcInfo_.getDversion()) + 
      " of CALC the size of calibration list is unexpected, " + QString("").setNum(num) +
      "; it have to be " + QString("").setNum(calcInfo_.getDversion()<11.0?12:14));
    // return;
  };
  if (bitFlags == 0)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::applyObsCalibrationSetup(): all the bit flags are zeros, nothing to apply");
    return;
  };
  
  //
  // clear all flags that can be overwritten from database setup:
  config_->setHave2ApplyPoleTideContrib(false);
  config_->setHave2ApplyPxContrib(false);
  config_->setHave2ApplyPyContrib(false);
  config_->setHave2ApplyEarthTideContrib(false);
  config_->setHave2ApplyOceanTideContrib(false);
  config_->setHave2ApplyUt1OceanTideHFContrib(false);
  config_->setHave2ApplyPxyOceanTideHFContrib(false);
  config_->setHave2ApplyNutationHFContrib(false);
  config_->setHave2ApplyFeedCorrContrib(false);
  config_->setHave2ApplyTiltRemvrContrib(false);
  config_->setHave2ApplyPxyLibrationContrib(false);
  config_->setHave2ApplyUt1LibrationContrib(false);
  config_->setHave2ApplyOceanPoleTideContrib(false);
  config_->setHave2ApplyOldOceanTideContrib(false);
  config_->setHave2ApplyOldPoleTideContrib(false);
  bool                          isOn;
  for (int i=0; i<num; i++)
  {
    const QString              &str=calList.at(i);
    isOn = bitFlags & (1<<i);
    if (str == "PTD CONT")
      config_->setHave2ApplyPoleTideContrib(isOn);
    else if (str == "WOBXCONT")
      config_->setHave2ApplyPxContrib(isOn);
    else if (str == "WOBYCONT")
      config_->setHave2ApplyPyContrib(isOn);
    else if (str == "ETD CONT")
      config_->setHave2ApplyEarthTideContrib(isOn);
    else if (str == "OCE CONT")
      config_->setHave2ApplyOceanTideContrib(isOn);
    else if (str == "UT1ORTHO")
      config_->setHave2ApplyUt1OceanTideHFContrib(isOn);
    else if (str == "WOBORTHO")
      config_->setHave2ApplyPxyOceanTideHFContrib(isOn);
    else if (str == "WOBNUTAT")
      config_->setHave2ApplyNutationHFContrib(isOn);
    else if (str == "FEED.COR")
      config_->setHave2ApplyFeedCorrContrib(isOn);
    else if (str == "TILTRMVR")
      config_->setHave2ApplyTiltRemvrContrib(isOn);
    else if (str == "WOBLIBRA")
      config_->setHave2ApplyPxyLibrationContrib(isOn);
    else if (str == "UT1LIBRA")
      config_->setHave2ApplyUt1LibrationContrib(isOn);
    else if (str == "OPTLCONT")
      config_->setHave2ApplyOceanPoleTideContrib(isOn);
    else if (str == "OCE_OLD ")
      config_->setHave2ApplyOldOceanTideContrib(isOn);
    else if (str == "PTOLDCON")
      config_->setHave2ApplyOldPoleTideContrib(isOn);
    else if (str == "WAHRCONT")
      {}
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::applyObsCalibrationSetup(): got an unexpected contribution code: " + str + "; ignored");
  };
  config_->setContribsAreFromDatabase(true);
  logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
    "::applyObsCalibrationSetup(): calibration set up has been applied assuming version #" +
    QString("").setNum(calcInfo_.getDversion()) + " of CALC");
};



//
void SgVlbiSession::formObsCalibrationSetup(int& bitFlags, QList<QString> &calList)
{
  bitFlags = 0;
  calList.clear();
  if (calcInfo_.getDversion() < 11.0)
  {
    for (int i=0; i<12; i++)
      calList << sObsCalibrationList_v10[i];
    //
    if (config_->getHave2ApplyOceanTideContrib())
      bitFlags += 1 << O10_OCEAN_TIDE;
    if (config_->getHave2ApplyPoleTideContrib())
      bitFlags += 1 << O10_POLE_TIDE;
    if (config_->getHave2ApplyEarthTideContrib())
      bitFlags += 1 << O10_EARTH_TIDE;
    if (config_->getHave2ApplyPxContrib())
      bitFlags += 1 << O10_WOBBLE_X;
    if (config_->getHave2ApplyPyContrib())
      bitFlags += 1 << O10_WOBBLE_Y;
    if (config_->getHave2ApplyNutationHFContrib())
      bitFlags += 1 << O10_WOB_NUTAT;
    if (config_->getHave2ApplyPxyOceanTideHFContrib())
      bitFlags += 1 << O10_WOB_ORTHO;
    if (config_->getHave2ApplyUt1OceanTideHFContrib())
      bitFlags += 1 << O10_UT1_ORTHO;
    if (config_->getHave2ApplyFeedCorrContrib())
      bitFlags += 1 << O10_FEED_ROT;
    if (config_->getHave2ApplyTiltRemvrContrib())
      bitFlags += 1 << O10_TILT_RMVR;
    if (config_->getHave2ApplyOldPoleTideContrib())
      bitFlags += 1 << O10_POLE_TIDE_OLD;
  }
  else
  {
    for (int i=0; i<14; i++)
      calList << sObsCalibrationList_v11[i];
    //
    if (config_->getHave2ApplyOceanTideContrib())
      bitFlags += 1 << O11_OCEAN_TIDE;
    if (config_->getHave2ApplyPoleTideContrib())
      bitFlags += 1 << O11_POLE_TIDE;
    if (config_->getHave2ApplyEarthTideContrib())
      bitFlags += 1 << O11_EARTH_TIDE;
    if (config_->getHave2ApplyPxContrib())
      bitFlags += 1 << O11_WOBBLE_X;
    if (config_->getHave2ApplyPyContrib())
      bitFlags += 1 << O11_WOBBLE_Y;
    if (config_->getHave2ApplyPoleTideContrib())
      bitFlags += 1 << O11_OCN_POLE_TIDE;
    if (config_->getHave2ApplyPxyLibrationContrib())
      bitFlags += 1 << O11_WOB_LIBRA;
    if (config_->getHave2ApplyUt1LibrationContrib())
      bitFlags += 1 << O11_UT1_LIBRA;
    if (config_->getHave2ApplyPxyOceanTideHFContrib())
      bitFlags += 1 << O11_WOB_ORTHO;
    if (config_->getHave2ApplyUt1OceanTideHFContrib())
      bitFlags += 1 << O11_UT1_ORTHO;
    if (config_->getHave2ApplyFeedCorrContrib())
      bitFlags += 1 << O11_FEED_ROT;
    if (config_->getHave2ApplyTiltRemvrContrib())
      bitFlags += 1 << O11_TILT_RMVR;
    if (config_->getHave2ApplyOldOceanTideContrib())
      bitFlags += 1 << O11_OCEAN_TIDE_OLD;
    if (config_->getHave2ApplyOldPoleTideContrib())
      bitFlags += 1 << O11_POLE_TIDE_OLD;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
    "::formObsCalibrationSetup(): calibration set up has been formed assuming version #" +
    QString("").setNum(calcInfo_.getDversion()) + " of CALC");
};



//
void SgVlbiSession::importMapFile(const QString& mapFileName, QMap<QString, QString>& map4Stn,
  QMap<QString, QString>& map4Src, QMap<QString, QString>& map4Bnd)
{
  if (map4Stn.size())
    map4Stn.clear();
  if (map4Src.size())
    map4Src.clear();
  if (!mapFileName.size()) // empty string, nothing to do (even complain)
    return;
  //
  //
  QFile                         f(mapFileName);
  if (!f.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::importMapFile(): the map file [" + f.fileName() + "] does not exist");
    return;
  }
  else
  {
    if (f.open(QFile::ReadOnly))
    {
      QTextStream               s(&f);
      QString                   str(""), from(""), to("");
      int                       numOfStrs, numOfSrc, numOfStn, numOfBnd;
      QRegExp                   reStn("^stn:\\s*([0-9A-Z]{1}[\\.0-9A-Z\\s_+-]{1,6}[0-9A-Z_]{1})\\s*=>"
                                      "\\s*([0-9A-Z-]{1}[0-9A-Z\\s_+-]{1,6}[0-9A-Z_-]{1}).*");
      QRegExp                   reSrc("^src:\\s*([0-9A-Z]{1}[\\.0-9A-Z\\s_+-]{1,6}[0-9A-Z_]{1})\\s*=>"
                                      "\\s*([0-9A-Z-]{1}[0-9A-Z\\s_+-]{1,6}[0-9A-Z_-]{1}).*");
      QRegExp                   reBnd("^bnd:\\s*([0-9A-Za-z]{1,2})\\s*=>\\s*([0-9A-Za-z-]{1,3}).*");
      numOfStrs = numOfSrc = numOfStn = numOfBnd = 0;
      while (!s.atEnd())
      {
        str = s.readLine();
        numOfStrs++;
        if (str.size())
        {
          if (-1 < reStn.indexIn(str))
          {
            if (reStn.cap(2) == sSkipCode_)
            {
              from = reStn.cap(1).leftJustified(8, ' ', true);
              if (!map4Stn.contains(from))
                map4Stn.insert(from, sSkipCode_);
              else
                map4Stn[from] = sSkipCode_;
              logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
                "::importMapFile(): the station \"" + from + "\" will be excluded from the session");
            }
            else
            {
              from = reStn.cap(1).leftJustified(8, ' ', true);
              to   = reStn.cap(2).leftJustified(8, ' ', true);
              if (!map4Stn.contains(from))
              {
                map4Stn.insert(from, to);
                logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a station map record added: \"" + from + "\" => \"" + to + "\"");
              }
              else if (map4Stn.value(from) == to)
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a station map record \"" + from + "\" => \"" + to +
                  "\" is already in the map");
              else
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): an attempt to change a station map record \"" + from +
                  "\" => \"" + to + "\"; ignored");
            };
          }
          else if (str.left(4) == "stn:")
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::importMapFile(): cannot parse a station map string: \"" + str + "\"");
          //
          // sources:
          if (-1 < reSrc.indexIn(str))
          {
            if (reSrc.cap(2) == sSkipCode_)
            {
              from = reSrc.cap(1).leftJustified(8, ' ', true);
              if (!map4Src.contains(from))
                map4Src.insert(from, sSkipCode_);
              else
                map4Src[from] = sSkipCode_;
              logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
                "::importMapFile(): the source \"" + from + "\" will be excluded from the session");
            }
            else
            {
              from = reSrc.cap(1).leftJustified(8, ' ', true);
              to   = reSrc.cap(2).leftJustified(8, ' ', true);;
              if (!map4Src.contains(from))
              {
                map4Src.insert(from, to);
                logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a source map record added: \"" + from + "\" => \"" + to + "\"");
              }
              else if (map4Src.value(from) == to)
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a source map record \"" + from + "\" => \"" + to +
                  "\" is already in the map");
              else
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): an attempt to change a source map record \"" + from +
                  "\" => \"" + to + "\"; ignored");
            };
          }
          else if (str.left(4) == "src:")
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::importMapFile(): cannot parse a source map string: \"" + str + "\"");
          //
          // bands:
          if (-1 < reBnd.indexIn(str))
          {
            if (reBnd.cap(2) == sSkipCode_)
            {
              from = reBnd.cap(1).trimmed();
              if (!map4Bnd.contains(from))
                map4Bnd.insert(from, sSkipCode_);
              else
                map4Bnd[from] = sSkipCode_;
              logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
                "::importMapFile(): the band \"" + from + "\" will be excluded from the session");
            }
            else
            {
              from = reBnd.cap(1).trimmed();
              to   = reBnd.cap(2).trimmed();
              if (!map4Bnd.contains(from))
              {
                map4Bnd.insert(from, to);
                logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a band map record added: \"" + from + "\" => \"" + to + "\"");
              }
              else if (map4Bnd.value(from) == to)
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): a band map record \"" + from + "\" => \"" + to +
                  "\" is already in the map");
              else
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importMapFile(): an attempt to change a band map record \"" + from +
                  "\" => \"" + to + "\"; ignored");
            };
          }
          else if (str.left(4) == "bnd:")
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::importMapFile(): cannot parse a band map string: \"" + str + "\"");
        };
      };
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::importMapFile(): read " + QString("").setNum(numOfStrs) + 
        " strings from the map file [" + f.fileName() + "]");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::importMapFile(): cannot open the map file [" + f.fileName() + "] for read access");
      return;
    };
  };
};




//
bool SgVlbiSession::check4NameMap(const QMap<QString, QString> &map, QString& name)
{
  if (!map.size()) // nothing to do
    return true;
  if (map.contains(name))
  {
    if (map.value(name) == sSkipCode_)
    {
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::check4stationNameMap(): the object \"" + name + "\" have to be skipped");
      return false;
    };
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::check4stationNameMap(): the input object name \"" + name + "\" is mapping to \"" +
      map.value(name) + "\"");
    name = map.value(name);
  };
  return true;
};




/*=====================================================================================================*/

/*=====================================================================================================*/
