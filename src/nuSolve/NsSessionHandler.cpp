/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
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

#include "NsSessionHandler.h"
#include "nuSolve.h"

#include <iostream>
#include <stdlib.h>



#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>



#include <SgConstants.h>
#include <SgDbhImage.h>
#include <SgGuiPiaReport.h>
#include <SgGuiPlotter.h>
#include <SgGuiTaskConfig.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgParameterCfg.h>
#include <SgSingleSessionTaskManager.h>
#include <SgSolutionReporter.h>
#include <SgTaskManager.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>
#include <SgVlbiObservation.h>






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsSessionHandler::NsSessionHandler(const QString& fileName, const QString& fileNameAux, 
  SgTaskConfig* config, SgParametersDescriptor* parametersDescriptor, 
  SgVlbiSessionInfo::OriginType oType)
  : fileName_(fileName),
    fileNameAux_(fileNameAux),
    path2DbhFiles_(""),
    dbhImageByKey_(), 
    dbhNameByKey_(), 
    dbhPathByKey_(),
    path2VdbFiles_(""),
    vdbWrapperFileName_(""),
    path2VdaFiles_("")
{
  oType_ = oType;
  session_ = NULL;
  reporter_ = NULL;
  vgosDb_ = NULL;

  // make a copy:
  config_ = new SgTaskConfig(*config);
  parametersDescriptor_ = new SgParametersDescriptor(*parametersDescriptor);
  //
  //
  initData();
};



//
NsSessionHandler::NsSessionHandler(SgTaskConfig* config, SgParametersDescriptor* parametersDescriptor)
  : fileName_(""),
    fileNameAux_(""),
    path2DbhFiles_(""),
    dbhImageByKey_(), 
    dbhNameByKey_(), 
    dbhPathByKey_(),
    path2VdbFiles_(""),
    vdbWrapperFileName_("")
{
  oType_ = SgVlbiSessionInfo::OT_UNKNOWN;
  session_ = NULL;
  reporter_ = NULL;
  vgosDb_ = NULL;

  // make a copy:
  config_ = new SgTaskConfig(*config);
  parametersDescriptor_ = new SgParametersDescriptor(*parametersDescriptor);
  //
  initData();
};



//
void NsSessionHandler::initData()
{
  //
  isImportOk_ = false;
  saveStatus_ = SS_UNDEF;
  // overwrite the saved set up:
  config_->setActiveBandIdx(0);
  config_->setUseDelayType(SgTaskConfig::VD_SB_DELAY);
  config_->setOpAction(SgTaskConfig::OPA_RESTORE);
  config_->setDoWeightCorrection(false);
  //
  // create the session and set up its config:
  session_ = new SgVlbiSession;
  session_->setConfig(config_);
  session_->setParametersDescriptor(parametersDescriptor_);
  session_->setOriginType(oType_);
  session_->setPath2Masterfile(setup.path2(setup.getPath2MasterFiles()));
  session_->setPath2APrioriFiles(setup.path2(setup.getPath2APrioriFiles()));
  session_->setPath2TrpFiles(setup.path2(setup.getPath2TrpFiles()));
  //
  reporter_ = new SgSolutionReporter(session_, setup.identities());
  //
  activeBand_ = 0;
  //
};



//
NsSessionHandler::~NsSessionHandler()
{
  //
  dbhNameByKey_.clear();
  dbhPathByKey_.clear();
  for (QMap<QString, SgDbhImage*>::iterator it=dbhImageByKey_.begin(); it!=dbhImageByKey_.end(); ++it)
    delete it.value();
  dbhImageByKey_.clear();
  //
  if (vgosDb_)
  {
    delete vgosDb_;
    vgosDb_ = NULL;
  };
  //
  if (reporter_)
  {
    delete reporter_;
    reporter_ = NULL;
  };
  //
  if (session_)
  {
    delete session_;
    session_ = NULL;
  };
	//	config = *config_;
  delete config_;
  config_ = NULL;

  delete parametersDescriptor_;
  parametersDescriptor_ = NULL;
};



//
bool NsSessionHandler::importSession(bool guiExpected)
{
  isImportOk_ = false;
  saveStatus_ = SS_UNDEF;
  //
  if (oType_==SgVlbiSessionInfo::OT_DBH)
  {
    if (setup.getHave2UpdateCatalog())
      setupCatsDbhFileNames(fileName_, fileNameAux_);
    else
      setupDogsDbhFileNames(fileName_);
  }
  else if (oType_==SgVlbiSessionInfo::OT_VDB)
  {
    setupVgosDbFileNames(fileName_);
  }
  else if (oType_==SgVlbiSessionInfo::OT_AGV)
  {
    setupVgosDaFileNames(fileName_);
  };
  //
  if ((oType_==SgVlbiSessionInfo::OT_DBH && dbhNameByKey_.size()==0)      ||
      (oType_==SgVlbiSessionInfo::OT_VDB && vdbWrapperFileName_.size()==0)||
      (oType_==SgVlbiSessionInfo::OT_AGV && fileName_.size()==0)
     )
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::importSession(): cannot find anything associated with the name \"" + fileName_ + 
      "\". Please, check and do it again.");
    return false;
  };
  //
  // read the files:
  if (oType_==SgVlbiSessionInfo::OT_DBH)
    isImportOk_ = initSessionFromDbh(guiExpected);
  else if (oType_==SgVlbiSessionInfo::OT_VDB)
    isImportOk_ = initSessionFromOdb(guiExpected);
  else if (oType_==SgVlbiSessionInfo::OT_AGV)
    isImportOk_ = initSessionFromVda(guiExpected);
  //
  return isImportOk_;
};



//
bool NsSessionHandler::performPia()
{
  bool                           isOk(false);
  //
  if (setup.getHave2SkipAutomaticProcessing())
  {
    logger->write(SgLogger::WRN, SgLogger::SESSION, className() + 
      "::performPia(): the automatic analysis is disabled by the command line argument");
    return isOk;
  };

  if (isImportOk_)
  {

    isOk = session_->doPostReadActions();
    
  };
  //
  return isOk;
};



//
//
void NsSessionHandler::process()
{
  session_->process(true, false);
};



//
void NsSessionHandler::calcIono()
{
  session_->calculateIonoCorrections(config_);
};



//
void NsSessionHandler::zeroIono()
{
  session_->zerofyIonoCorrections(config_);
};


void NsSessionHandler::changeActiveBand(int idx)
{
  activeBand_ = idx;
  config_->setActiveBandIdx(activeBand_);
};




//
bool NsSessionHandler::saveResults()
{
  //
  bool                           isOk(false);
  if (session_->isAttr(SgVlbiSessionInfo::Attr_FF_AUTOPROCESSED))
  {
    if (oType_ == SgVlbiSessionInfo::OT_DBH)
      isOk = exportDataIntoDbh();
    else if (oType_ == SgVlbiSessionInfo::OT_VDB)
      isOk = exportDataIntoVgosDb();
    else if (oType_ == SgVlbiSessionInfo::OT_AGV) // export data in vgosDb format:
      isOk = exportDataIntoVgosDa();
    //
    if (isOk && 
        oType_ != SgVlbiSessionInfo::OT_AGV)
      generateReport();
  };
  //
  return isOk;
};



//
// Sets up elements of dbhNameByKey_ map in a catalog-aware mode:
void NsSessionHandler::setupCatsDbhFileNames(const QString& fName, const QString& fName2)
{
  QString                       fileName(fName);
  QString                       fileName2(fName2);
  QTextStream                   s;
  QFile                         f;
  QString                       baseName;         // "YYMMMDD" part of databese's file name  
  QString                       currentBand;      // "S", "X", ...
  QString                       baseNetID;        // "A", "E", ...
  QString                       currentVersion;   // e.g., "V001"
  QString                       str1, str2;
  QString                       key;
  int                           version, v, l;
  bool                          isOk;

  // user's input, can contain $-char at the begin:
  if (fileName.at(0) == '$')
    fileName = fileName.mid(1);
  l = fileName.length();
  if (fileName2.size()>0 && fileName2.at(0) == '$')
    fileName2 = fileName2.mid(1);
  
  if (l==9)
  {
    // find out the first available version of the file:
    v = 1;
    while (inquireCat(fileName, ++v,  str1, str2) != 0 && v<70)
      ;
    v--;
    if (v==69)
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::setupCatsDbhFileNames(): the file " + fileName + " does not exist in the catalog");
      return;
    };
    // find out the latest version of the file:
    while (inquireCat(fileName, ++v,  str1, str2) == 0)
      ;
    v--;
    if (v<3)
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::setupCatsDbhFileNames(): the file " + fileName + " does not exist in the catalog");
      return;
    };
    fileName += QString("").sprintf("_V%03d", v);
    l += 5;
  }
  else if (!(fileName.mid(l-5, 2) == "_V")) // is it full name?
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::setupCatsDbhFileNames(): the file " + fileName + " is not a standard database name, "
      "expecting: YYMMMDDBC_Vxxx");
    return;
  };

  baseName = fileName.mid(l-14, 7);
  currentBand = fileName.mid(l-7, 1);
  baseNetID = fileName.mid(l-6, 1);
  currentVersion = fileName.mid(l-4, 4);
  version = currentVersion.mid(1).toInt(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::setupCatsDbhFileNames(): cannot determine proper DBH version for the file " + fileName);
    return;
  };
  
  // enquire about user provided database:
  if (inquireCat(baseName + currentBand + baseNetID, version,  str1, str2) != 0)
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::setupCatsDbhFileNames(): got an error on the request to " + 
      setup.catnuGetDbInfo() + ": " + str1);
    return;
  };
  // set up path to databases:
  l = str2.lastIndexOf("/");
  path2DbhFiles_ = str2.left(l+1);
  dbhNameByKey_[currentBand] = str2.mid(l+1);
  dbhPathByKey_[currentBand] = path2DbhFiles_;
  //
  //
  if (setup.getHave2AutoloadAllBands() && fileName2 != "/dev/null")
  {
    l = fileName2.size();
    if (l >= 9) // user explicitly provided a filename of the second database:
    {
      isOk = true;
      if (l==9) // no version info:
      {
        // find out the first available version of the file:
        v = 1;
        while (inquireCat(fileName2, ++v,  str1, str2) != 0 && v<70)
          ;
        v--;
        if (v==69)
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
            "::setupCatsDbhFileNames(): cannot find the second file " + fileName2 + " in the catalog");
          isOk = false;
        };
        // find out the latest version of the file:
        while (inquireCat(fileName2, ++v,  str1, str2) == 0)
          ;
        v--;
        if (v<2)
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
            "::setupCatsDbhFileNames(): cannot find the second file " + fileName2 + " in the catalog");
          isOk = false;
        };
        fileName2 += QString("").sprintf("_V%03d", v);
        l += 5;
      }
      else
        v = fileName2.mid(l-3, 3).toInt(&isOk);
      //
      if (isOk)
      {
        isOk = inquireCat(fileName2, v,  str1, str2) == 0;
        //
        QString                    aBand("");
        aBand = fileName2.mid(l-7, 1);
        l = str2.lastIndexOf("/");
        path2DbhFiles_ = str2.left(l+1);
        dbhNameByKey_[aBand] = str2.mid(l+1);
        dbhPathByKey_[aBand] = path2DbhFiles_;
        logger->write(SgLogger::INF, SgLogger::SESSION, className() +
          "::setupCatsDbhFileNames(): the second database file " + fileName2 + 
          " has been found in the catalog, band=[" + currentBand + "], version=" + 
          QString("").setNum(v));
      };
    }
    else // guess a file name of the second database:
    {
      // find a database with opposite band:
      v = version;
      isOk = false;
      if (currentBand == "X")
      {
        key = "S";
        while (v>1 && !isOk)
          isOk = inquireCat(baseName + key + baseNetID, v--,  str1, str2) == 0;
      }
      else
      {
        key = "X";
        while (v<31 && !isOk) // ver 30 -- is it enouhg?
          isOk = inquireCat(baseName + key + baseNetID, v++,  str1, str2) == 0;
      };
      if (isOk)
      {
        l = str2.lastIndexOf("/");
        dbhNameByKey_[key] = str2.mid(l+1);
        dbhPathByKey_[key] = str2.left(l+1);
      }
      else
        logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
          "::setupCatsDbhFileNames(): cannot find an opposite band, " + key + ", : " + str1);
    };
  };
  //
  // check if it is possible to submit a next version database:
  if (inquireCat(baseName + (currentBand=="S"?"X":currentBand) + baseNetID, version+1,  str1, str2) == 0)
  {
    if (!setup.getHave2MaskSessionCode())
    {
      saveStatus_ = SS_FILE_EXISTS;
      logger->write(SgLogger::WRN, SgLogger::SESSION, className() + 
        "::setupCatsDbhFileNames(): a database with the next version already exists in the catalog");
    };
  };
};



//
// Sets up elements of dbhNameByKey_ map in a standalone (non catalog) mode:
void NsSessionHandler::setupDogsDbhFileNames(const QString& fileName)
{
  QString baseName;         // "YYMMMDD" part of databese's file name  
  QString currentBand;      // "S", "X", ...
  QString baseNetID;        // "A", "E", ...
  QString currentVersion;   // e.g., "V001"
  QString str;
  int l = fileName.length();
  QString key;

  if (fileName.mid(l-5, 2) == "_V") // ok, we asuming it is standard IVS naming convention:
  {
    //
    currentBand = fileName.mid(l-7, 1);
    baseNetID = fileName.mid(l-6, 1);
    currentVersion = fileName.mid(l-4, 4);

    l = fileName.lastIndexOf("/");
    // we figure out it from the fileName:
    if (fileName.at(0) == '/') // user provided absolute path:
    {
      path2DbhFiles_ = fileName.left(l+1);
      str = fileName.mid(l+1);
    }
    else
    {
      path2DbhFiles_ =  setup.getPath2DbhFiles() + "/" + fileName.left(l+1);
      str = fileName.mid(l+1);
    };
    //
    baseName = str.left(7);
    QDir dir(path2DbhFiles_);
    QStringList nameFilters;
    if (setup.getHave2AutoloadAllBands())
      nameFilters << baseName + "?" + baseNetID + "*";
    else
      nameFilters << baseName + currentBand + baseNetID + "*";
    QStringList nameList = dir.entryList(nameFilters, 
                                QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase);
    if (!nameList.size())
    {
      // ??
    };
    for (int i=0; i<nameList.size(); i++)
    {
      l = nameList.at(i).length();
      if (l==14)
      {
        key = nameList.at(i).mid(l-7, 1);
        if (key == currentBand && !dbhNameByKey_.contains(key))
          dbhNameByKey_[key] = str;
        else
        {
          if (!dbhNameByKey_.contains(key))
            dbhNameByKey_[key] = nameList.at(i);
          else if (nameList.at(i).mid(l-4, 4) <= currentVersion)
            dbhNameByKey_[key] = nameList.at(i);
        };
      };
    };
  }
  else // perhaps, it is new naming convention, pass it as is:
    dbhNameByKey_["A"] = fileName;
};



//
// Sets up path to and name of the wrapper file of vgosDb data tree:
void NsSessionHandler::setupVgosDbFileNames(const QString& inpt)
{
  QString                       sessionName("");
  int                           version(0);
  
  bool                          isOk;

  QRegularExpression            reWrapperFileName(".*\\.(wrp|wrap)$",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reOldDbName("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName("^(\\d{8})-([\\S]{2,12}).*");

  QRegularExpression            rePath2DatabaseV1("(.*)/(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2})/?$");
  QRegularExpression            rePath2DatabaseV2("(.*)/(\\d{8}-[\\S]{2,12})/?$");

  //
  // ok, this is a wrapper file name:
  if (reWrapperFileName.match(inpt).hasMatch())
  {
    isOk = SgVlbiSession::guessSessionByWrapperFileName(inpt,
      setup.path2(setup.getPath2VgosDbFiles()), path2VdbFiles_, vdbWrapperFileName_,
        sessionName, version);
    if (isOk)
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::setupVgosDbFileNames(): got a session " + sessionName + " of version " + 
        QString("").setNum(version) + " from \"" + path2VdbFiles_  + "\":\"" + 
        vdbWrapperFileName_ + "\"");
    else
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::setupVgosDbFileNames(): cannot figure out a session from the input \"" + inpt + "\"");
  }
  // a database or a database with version:
  else if ( reOldDbName.match(inpt).hasMatch() ||
            reNewDbName.match(inpt).hasMatch()  )
  {
    isOk = SgVlbiSession::guessWrapperFileNameBySession(inpt,
      setup.path2(setup.getPath2VgosDbFiles()), 
      setup.identities().getAcAbbName(), setup.getUseOwnOnlyDatabase(),
      path2VdbFiles_, vdbWrapperFileName_, sessionName, version);
    if (isOk)
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::setupVgosDbFileNames(): got a wrapper file \"" + vdbWrapperFileName_ + 
        "\" from \"" + path2VdbFiles_ + "\" for the session " + sessionName + 
        " of version " + QString("").setNum(version));
    else
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::setupVgosDbFileNames(): cannot figure out a wrapper file from the input \"" + inpt + "\"");
  }
  else if (rePath2DatabaseV1.match(inpt).hasMatch() ||
           rePath2DatabaseV2.match(inpt).hasMatch()  )
  {
    // a directory with a database:
    QDir                        dir(inpt);
    if (dir.exists())
    {
      QRegularExpressionMatch   match=rePath2DatabaseV1.match(inpt);
      if (!match.hasMatch())
        match=rePath2DatabaseV2.match(inpt);
      QString                   sDir(match.captured(2));
      QString                   sPath(match.captured(1));

      isOk = SgVlbiSession::guessWrapperFileNameBySession(sDir, sPath, 
      setup.identities().getAcAbbName(), setup.getUseOwnOnlyDatabase(),
        path2VdbFiles_, vdbWrapperFileName_, sessionName, version, true);
      if (isOk)
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::setupVgosDbFileNames(): got a wrapper file \"" + vdbWrapperFileName_ + 
          "\" from \"" + path2VdbFiles_ + "\" for the session " + sessionName + 
          " of version " + QString("").setNum(version));
      else
        logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
          "::setupVgosDbFileNames(): cannot figure out a wrapper file from the directory \"" + 
          inpt + "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
        "::setupVgosDbFileNames(): the directory \"" + inpt + "\" does not exist");
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::setupVgosDbFileNames(): cannot guess input data from \"" + inpt + "\"");
  };
};



//
void NsSessionHandler::setupVgosDaFileNames(const QString& /*input*/)
{
  // just a file name:
  QFile                         file(fileName_);
  if (file.exists())
    return;
  QString                       str(setup.getPath2VgosDaFiles() + "/" + fileName_);
  file.setFileName(str);
  // if a user provided .vda suffix:
  if (file.exists())
  {
    fileName_ = str;
    return;
  };
  // if a user provided just a database name:
  str += ".vda";
  file.setFileName(str);
  if (file.exists())
    fileName_ = str;
};



// functions:
bool NsSessionHandler::initSessionFromDbh(bool guiExpected)
{
  SgDbhStream                   dbhStream;
  QFile                         dbhFile;
  SgDbhImage                   *dbhImage;
  QString                       path;
  QList<QString>                lst;
  //
  if (setup.getUseAltMasterfileSuffixes())
    lst = setup.getMasterfileSuffixes();
  //

  for (QMap<QString, QString>::const_iterator i=dbhNameByKey_.constBegin(); 
    i!=dbhNameByKey_.constEnd(); ++i)
  {
    path = dbhPathByKey_.contains(i.key()) ? dbhPathByKey_[i.key()] : path2DbhFiles_;
    dbhImage = new SgDbhImage;
    dbhFile.setFileName(path + i.value());
    logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() + 
      "::initSessionFromDbh(): reading file " + i.value() + " in the directory \"" +
      path + "\"", true);

    dbhFile.open(QIODevice::ReadOnly);
    dbhStream.setDevice(&dbhFile);
    dbhStream >> *dbhImage;
    dbhStream.setDevice(NULL);
    dbhFile.close();
    dbhImageByKey_[i.key()] = dbhImage;
  };
 
  session_->setOriginType( (oType_=SgVlbiSessionInfo::OT_DBH) );
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::initSessionFromDbh(): absorbing data from the set of DBH images", true);
  return session_->getDataFromDbhImages(dbhImageByKey_, setup.getHave2LoadImmatureSession(),
    guiExpected, lst);
};




//
bool NsSessionHandler::initSessionFromOdb(bool guiExpected)
{
  bool                          isOk;
  QList<QString>								lst;
	//
	if (setup.getUseAltMasterfileSuffixes())
		lst = setup.getMasterfileSuffixes();
	//
  //
  vgosDb_ = new SgVgosDb(&setup.identities(), &nuSolveVersion);
  isOk = vgosDb_->init(path2VdbFiles_, vdbWrapperFileName_);
  logger->write(isOk?SgLogger::DBG:SgLogger::ERR, SgLogger::GUI | SgLogger::SESSION, className() + 
    "::initSessionFromOdb(): parsing of wrapper file " + vdbWrapperFileName_ + 
    (isOk?" complete":" failed"));

  session_->setOriginType( (oType_=SgVlbiSessionInfo::OT_VDB) );

  if (isOk && session_->getDataFromVgosDb(vgosDb_, setup.getHave2LoadImmatureSession(), guiExpected, 
		lst))
  {
    logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() + 
      "::initSessionFromOdb(): the session " + session_->getSessionCode() + " (" + session_->getName() +
      ") has been imported from " + path2VdbFiles_);
    return true;
  };
  logger->write(SgLogger::ERR, SgLogger::GUI | SgLogger::SESSION, className() + 
    "::initSessionFromOdb(): reading of the session " + session_->getSessionCode() + 
    " (" + session_->getName() + ") failed");
  return false;
};



//
bool NsSessionHandler::initSessionFromVda(bool /*guiExpected*/)
{
  SgIdentities                  ids;
  SgVersion                     driverVersion;
  QList<QString>								lst;
	//
	if (setup.getUseAltMasterfileSuffixes())
		lst = setup.getMasterfileSuffixes();
	//

  session_->setOriginType( (oType_=SgVlbiSessionInfo::OT_AGV) );

  if (session_->getDataFromAgvFile(fileName_, &setup.identities(), &nuSolveVersion, lst))
  {
    logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() + 
      "::initSessionFromVda(): the session " + session_->getSessionCode() + " (" + session_->getName() +
      ") has been imported from " + fileName_);
    return true;
  };
  logger->write(SgLogger::ERR, SgLogger::GUI | SgLogger::SESSION, className() + 
    "::initSessionFromVda(): reading data from vgosDa file \"" + fileName_ + "\" failed");
  return false;
};



//
bool NsSessionHandler::exportDataIntoDbh()
{
  if (!canSave())
  {
    saveStatus_ = SS_NOT_READY;
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": exportDataIntoDbh(): saving data in DBH format has been canceled");
    return false;
  };
  //
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::exportDataIntoDbh(): saving data in DBH format");

  if (!session_)
  {
    saveStatus_ = SS_INTERNAL_ERROR;
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::exportDataIntoDbh(): the session pointer is NULL");
    return false;
  }
  session_->addAttr(SgVlbiSessionInfo::Attr_PRE_PROCESSED);
  SgVlbiBand                   *primaryBand=session_->primaryBand();
  if (!primaryBand)
  {
    saveStatus_ = SS_INTERNAL_ERROR;
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::exportDataIntoDbh(): the primary band pointer is NULL");
    return false;
  };
  SgDbhImage                   *dbhImage=dbhImageByKey_[primaryBand->getKey()];
  if (!dbhImage)
  {
    saveStatus_ = SS_INTERNAL_ERROR;
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      "::exportDataIntoDbh(): the dbhImage pointer is NULL");
    return false;
  };

  bool                          have2NotifyCatalog(setup.getHave2UpdateCatalog());
  bool                          isNewChain(false);
  // determine a name of the output file:
  QString           sessionFileName, dbhFileName;
  if (setup.getHave2MaskSessionCode())
  {
    //  QString alterCode(networkSuffix=="U"?"1":"8");
    QString                     alterCode(session_->getNetworkSuffix()=="U"?"1":"7");
    dbhImage->alterCode(alterCode);
    session_->setNetworkSuffix(alterCode);
    isNewChain = true;
  };

  sessionFileName = session_->getName() + QString().sprintf("_V%03d", dbhImage->currentVersion());
  dbhFileName = path2DbhFiles_ + sessionFileName;
  // check for existing file:
  if (QFile::exists(dbhFileName))
  {
    if (have2NotifyCatalog) // notify a user about refusing to save:
    {
      saveStatus_ = SS_FILE_EXISTS;
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::exportDataIntoDbh(): the file " + dbhFileName + " is already exist; remove it and try again");
      return false;
    };
    // alternate the name and save the data:
    QString                     suffix="_" + SgMJD::currentMJD().toString(SgMJD::F_INTERNAL);
    dbhFileName += suffix;
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      "::exportDataIntoDbh(): the new version file is already exist; the name is altered to " +
      sessionFileName + suffix);
  };
  // adjust data:
  session_->putDataIntoDbhImage(dbhImage);

  // before writing, verify catalog:
  if (have2NotifyCatalog)
  {
    QString         str1, str2;
    if (inquireCat(session_->getName(), dbhImage->currentVersion(),  str1, str2) == 0)
    {
      saveStatus_ = SS_CATALOG_ERROR;
      logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
        "::exportDataIntoDbh(): the new version already exists in the catalog, remove it from the "
        "catalog and try again");
      return false;
    };
  };
  //
  // write into the file:
  SgDbhStream   dbhStream;
  QFile         dbhFile(dbhFileName);
  dbhFile.open(QIODevice::WriteOnly);
  dbhStream.setDevice(&dbhFile);
  dbhStream << *dbhImage;
  dbhStream.setDevice(NULL);
  dbhFile.close();
  logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
    "::exportDataIntoDbh(): the " + primaryBand->getKey() + "-band of the session " + 
    session_->getName() + " has been saved into the file " + dbhFileName);

  if (have2NotifyCatalog)
  {
    QString         str1;
    if (notifyCat(dbhFileName, isNewChain, str1) != 0)
    {
      saveStatus_ = SS_CATALOG_ERROR;
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::exportDataIntoDbh(): updating the catalog has failed; error: " + str1);
      return false;
    }
    else
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
        "::exportDataIntoDbh(): the catalog has been updated successfuly");
  };
  //
  saveStatus_ = SS_OK;
  return true;
};



//
bool NsSessionHandler::exportDataIntoVgosDb(QString output)
{
  if (!canSave())
  {
    saveStatus_ = SS_NOT_READY;
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": exportDataIntoVgosDb(): exporting of data into vgosDb tree has been canceled");
    return false;
  };
  //
  bool                          isOk;
  
  // data were imported in vgosDb format:
  if (vgosDb_)
  {
    if ((isOk=session_->putDataIntoVgosDb(vgosDb_)))
      saveStatus_ = SS_OK;
    else
      saveStatus_ = SS_SAVING_ERROR;
  }
  // data were imported in another format:
  else
  {
    QString                     dir2Save("");
    QDir                        dir;
    if (output.size()) // for vgosDb output is a directory name:
    {
      dir2Save = output;
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::exportDataIntoVgosDb(): got a user specified path: \"" + dir2Save + "\"");
    }
    else
      dir2Save = setup.path2(setup.getPath2VgosDbFiles()) + "/" + 
        QString("").sprintf("%04d", session_->getTStart().calcYear()) + "/" + session_->getName();
    
    vgosDb_ = new SgVgosDb(&setup.identities(), &nuSolveVersion);
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::exportDataIntoVgosDb(): vgosDb object has been created");

    vgosDb_->init(session_);
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::exportDataIntoVgosDb(): the vgosDb object has been prepared to save the new session in "
      "vgosDb format");
    //
    // check if a dir (or a file) exists, adjust the name:
    if (dir.exists(dir2Save))
    {
      SgMJD                     epoch=SgMJD::currentMJD();
      dir2Save += "-" + epoch.toString(SgMJD::F_yyyymmdd);
      logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
        "::exportDataIntoVgosDb(): the output path has been adjusted to \"" + dir2Save + "\"");
      if (dir.exists(dir2Save))
      {
        epoch = SgMJD::currentMJD();
        dir2Save = setup.path2(setup.getPath2VgosDbFiles()) + "/" + 
          QString("").sprintf("%04d", session_->getTStart().calcYear()) + "/" + session_->getName() +
          "-" + epoch.toString(SgMJD::F_SOLVE_SPLFL_LONG);
        logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
          "::exportDataIntoVgosDb(): the output path has been readjusted to \"" + dir2Save + "\"");
      };
    };
    vgosDb_->setPath2RootDir(dir2Save);
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::exportDataIntoVgosDb(): the path was set to " + vgosDb_->getPath2RootDir());

    isOk = session_->putDataIntoVgosDb(vgosDb_);
    logger->write(SgLogger::INF, SgLogger::SESSION, className() +
      "::exportDataIntoVgosDb(): export of data into vgosDb tree has been " + (isOk?"complete":"failed"));
    
    delete vgosDb_;
    vgosDb_ = NULL;
    logger->write(SgLogger::DBG, SgLogger::SESSION, className() +
      "::exportDataIntoVgosDb(): vgosDb object has been destroyed.");
  };

  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() + 
    "::exportDataIntoVgosDb(): exporting of data into vgosDb tree has been" +
    (isOk?" complete":" failed"));
  
  return isOk;
};



//
int NsSessionHandler::inquireCat(const QString& databaseName, int version,
  QString& str1, QString& str2)
{
  int                     rc;
  QTextStream             s;
  QFile                   f;
  QString                 execStr;
  QString                 tmpFileName("/tmp/nuSolve-Catalog.");
  QString                 complain;
  tmpFileName += SgMJD::currentMJD().toString(SgMJD::F_INTERNAL);

  execStr = setup.getPath2CatNuInterfaceExec() + "/" + setup.catnuGetDbInfo() + " " +
            databaseName + " " + QString("").setNum(version) + " " + tmpFileName;
  rc = system(qPrintable(execStr));
  if (rc!=0) // complain
  {
    complain = "An error occured executing the system call [" + execStr + "]; RC is " +
                QString("").setNum(rc);
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::inquireCat(): " + complain, true);
    return -2;
  };
  f.setFileName(tmpFileName);
  f.open(QIODevice::ReadOnly);
  s.setDevice(&f);
  str1 = s.readLine();
  if (str1 != "0 success")
  {
    f.close();
    f.remove();
    str2 = "";
    return -1;
  };
  str2 = s.readLine();
  if (str2.length() < 14)
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::inquireCat(): the length of the received string, " + str2 + 
      ", is less than a properly formed database name");
  s.setDevice(NULL);
  f.close();
  f.remove();
  return 0;
};



//
int NsSessionHandler::notifyCat(const QString& fileName, bool isNew, QString& str1)
{
  int                     rc;
  QTextStream             s;
  QFile                   f;
  QString                 execStr;
  QString                 tmpFileName("/tmp/nuSolve-Catalog.");
  QString                 complain;
  QString                 descriptionOfKey("1234"), descriptionOfEntry("");
  tmpFileName += SgMJD::currentMJD().toString(SgMJD::F_INTERNAL);

  // put info in the comm.file:
  // for David:
  if (session_->primaryBand()->history().getFirstRecordFromUser().size())
  {
    if (isNew)
      descriptionOfKey = session_->primaryBand()->history().getFirstRecordFromUser();
    descriptionOfEntry = session_->primaryBand()->history().getFirstRecordFromUser();
  }
  else
  {
    // describe the key (if necessary):
    if (isNew)
      descriptionOfKey = session_->getOfficialName() + " | " + 
        libraryVersion.getSoftwareName() + "/" + nuSolveVersion.getSoftwareName();
    // describe the entry:
    descriptionOfEntry = session_->getOfficialName() + "/" + setup.identities().getAcAbbrevName() +
      " " + nuSolveVersion.getSoftwareName() +
      (isNew?" alt. ":" update. ") + setup.identities().getUserName();
  };
  //
  // prepare the file:
  f.setFileName(tmpFileName);
  if (f.open(QIODevice::WriteOnly))
  {
    s.setDevice(&f);
    s << descriptionOfKey  .left(36) << endl;
    s << descriptionOfEntry.left(80) << endl;
    s.setDevice(NULL);
    f.close();
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::notifyCat(): cannot open for writting the communication file");
    return -2;
  };
  //
  // form a string:
  execStr = setup.getPath2CatNuInterfaceExec() + "/" + setup.catnuSetDbInfo() + " " +
            fileName + " " + (isNew?"create":"update") + " " + tmpFileName;
//  std::cout << "calling string: [" << qPrintable(execStr) << "]\n";
  // and exec it:
  rc = system(qPrintable(execStr));
  if (rc!=0) // complain
  {
    complain = "An error occured executing the system call [" + execStr + "]; RC is " +
                QString("").setNum(rc);
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::notifyCat(): " + complain, true);
    return -2;
  };
  // read results:
  f.setFileName(tmpFileName);
  if (f.open(QIODevice::ReadOnly))
  {
    s.setDevice(&f);
    str1 = s.readLine();
    s.setDevice(NULL);
    f.close();
    f.remove();
    if (str1 != "0 success")
      return -1;
    return 0;
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() + 
      "::notifyCat(): cannot open for reading the communication file");
    return -2;
  };
};




//
void NsSessionHandler::generateReport(bool isExtended)
{
  bool                          isOk(true);
  QString                       splflDirName=setup.path2(setup.getPath2SpoolFileOutput());
  QString                       obsStatDirName=setup.path2(setup.getPath2NotUsedObsFileOutput());
  QDir                          d(splflDirName);
  if (!d.exists())
//  isOk = d.mkpath("./"); // Qt, wtf?
    isOk = d.mkpath(d.absolutePath());
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateReport(): cannot create directory \"" + splflDirName + "\"; report failed");
    return;
  };
  d.setPath(obsStatDirName);
  if (!d.exists())
//  isOk = d.mkpath("./");
    isOk = d.mkpath(d.absolutePath());
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateReport(): cannot create directory \"" + obsStatDirName + "\"; saving report failed");
    return;
  };
  //
  // create the report:
  QString         							fileName("SPLF" + setup.identities().getUserDefaultInitials());
  QString                     	rptrDirName = setup.path2(setup.getPath2ReportOutput());

  reporter_->setPath2APrioriFiles(setup.path2(setup.getPath2APrioriFiles()));
  reporter_->report2spoolFile(splflDirName,	obsStatDirName, fileName, rptrDirName,
		isExtended, setup.getHave2KeepUnusedObsList());
	logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
		"::generateReport(): spoolfile output has been saved in the " + fileName + " file");
  //
  // save it in a separate file (if necessary):
  if (isOk && setup.getHave2KeepSpoolFileReports())
  {
    d.setPath(rptrDirName);
    if (!d.exists())
//    isOk = d.mkpath("./"); // Qt, wtf?
      isOk = d.mkpath(d.absolutePath());
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::generateReport(): cannot create directory " + rptrDirName + "; saving report failed");
      return;
    };
    
    QString                 str(session_->getName() + ".SFF");
    if (QFile::exists(rptrDirName + "/" + str) && !QFile(rptrDirName + "/" + str).remove())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::generateReport(): cannot remove the file " + rptrDirName + "/" + str + 
        "; saving report failed");
      return;
    };
    if (QFile::copy(splflDirName + "/" + fileName, rptrDirName + "/" + str))
      logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::generateReport(): spoolfile output has been copied as " + rptrDirName + "/" + str + 
        " file too");
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::generateReport(): copying of " + splflDirName + "/" + fileName + " as " + rptrDirName + 
        "/" + str + " filed");
  };
};




//
void NsSessionHandler::generateMyReport()
{
  bool                          isOk(true);
  QString                       splflDirName=setup.path2(setup.getPath2SpoolFileOutput());
  QString                       obsStatDirName=setup.path2(setup.getPath2NotUsedObsFileOutput());
  QDir                          d(splflDirName);
  if (!d.exists())
//  isOk = d.mkpath("./"); // Qt, wtf?
    isOk = d.mkpath(d.absolutePath());
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateMyReport(): cannot create directory \"" + splflDirName + "\"; report failed");
    return;
  };
  d.setPath(obsStatDirName);
  if (!d.exists())
//  isOk = d.mkpath("./");
    isOk = d.mkpath(d.absolutePath());
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateMyReport(): cannot create directory \"" + obsStatDirName + "\"; saving report failed");
    return;
  };
  //
  // create the report:
  QString         fileName("Report." + session_->getName() + ".dat");
  reporter_->setPath2APrioriFiles(setup.path2(setup.getPath2APrioriFiles()));
  reporter_->report2MyFile(splflDirName, fileName);
  logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
    "::generateMyReport(): report has been saved in the " + fileName + " file");
  //
};



//
void NsSessionHandler::generateReport4StcPars()
{
  QString                       outputDirName=setup.path2(setup.getPath2ReportOutput());
  QString                       dbName=session_->getName();

  if (dbName.at(0) == '$')
    dbName.remove(0, 1);

  outputDirName += "/" + dbName;
  reporter_->reportStochasticEstimations(outputDirName);
  logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
    "::generateReport4StcPars(): stochastic parameter estimaitons were saved in \"" + outputDirName +
    "\" direcory");
};



//
void NsSessionHandler::generateReport4Tzds()
{
  QString                       outputDirName=setup.path2(setup.getPath2ReportOutput());
  QString                       dbName=session_->getName();

  if (dbName.at(0) == '$')
    dbName.remove(0, 1);
  outputDirName += "/" + dbName;

  if (reporter_->reportTotalZenithDelays(outputDirName))
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateReport4Tzds(): total zenith delays were saved in \"" + outputDirName +
      "\" direcory");
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateReport4Tzds(): saving total zenith delays in \"" + outputDirName +
      "\" direcory has been failed");
  
};



//
//
void NsSessionHandler::generateAposterioriFiles()
{
  bool                          isOk(true);
  QString                       splflDirName=setup.path2(setup.getPath2SpoolFileOutput());
  QDir                          d(splflDirName);
  if (!d.exists())
//  isOk = d.mkpath("./"); // Qt, wtf?
    isOk = d.mkpath(d.absolutePath());
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
      "::generateAposterioriFiles(): cannot create directory " + splflDirName + "; report failed");
    return;
  };
  //
  // create the report:
  QString         fileName(session_->getName());
  if (fileName.at(0) == '$')
    fileName.remove(0, 1);
  reporter_->report2aposterioriFiles(splflDirName, fileName);
  logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
    "::generateAposterioriFiles(): a posteriori files were saved with \"" + fileName + "\" basename");
  //
};



//
void NsSessionHandler::exportDataToNgs()
{
  if (!canSave())
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": exportDataToNgs(): exporting of data into NGS card format has been canceled");
    return;
  };
  //
  QString         dirName=setup.path2(setup.getPath2NgsOutput());
  bool            isOk(true);
  // check for existance:
  QDir            d(dirName);
  if (!d.exists())
//  isOk = d.mkpath("./"); // Qt, wtf?
    isOk = d.mkpath(d.absolutePath());
  if (isOk) 
  {
    session_->exportDataIntoNgsFile(dirName);
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::exportDataToNgs(): the session has been exported as NGS file in the directory " + dirName);
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::exportDataToNgs(): cannot create directory " + dirName + "; NGS export failed");
};



//
bool NsSessionHandler::exportDataIntoVgosDa(QString output)
{
  if (!canSave())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      ": exportDataIntoVgosDa(): saving data in vgosDa format has been canceled");
    return false;
  };
  //
  QString                       dir2Save("");
  QString                       file2Save("");
  QDir                          dir;
  if (output.size())
  {
    // first, check is it a direcory:
    dir.setPath(output);
    if (dir.exists())
    {
      dir2Save = output;
    }
    else
    {
      // split input name for directory and file parts:
      int                         idx=-1;
      if (-1 < (idx=output.lastIndexOf('/')))
      {
        dir2Save   = output.left(idx);
        file2Save = output.mid(idx + 1);
      }
      else
        file2Save = output;
    };
  }
  else
    dir2Save = setup.path2(setup.getPath2VgosDaFiles());
  //
  //
  bool            isOk(true);
  // check for existance:
  dir.setPath(dir2Save);
  if (!dir.exists())
//  isOk = dir.mkpath("./"); // Qt, wtf?
    isOk = dir.mkpath(dir.absolutePath());
  if (isOk)
  {
    if ((isOk=session_->putDataIntoAgvFile(dir2Save, file2Save, &setup.identities(), &nuSolveVersion)))
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::exportDataIntoVgosDa(): the session has been saved as an vgosDa file in the directory \"" + 
        dir2Save + "\"");
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::exportDataIntoVgosDa(): export data in a vgosDa file failed; directory to store: \"" + 
        dir2Save + "\", file to store: \"" + file2Save + "\"");
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::exportDataIntoVgosDa(): cannot create directory " + dir2Save + 
      "; storing data in vgosDa format has failed");
  return isOk;
};



//
void NsSessionHandler::addUserComment(const QString& comment)
{
  reporter_->addUserComment(comment);
};
/*=====================================================================================================*/











/*=====================================================================================================*/
