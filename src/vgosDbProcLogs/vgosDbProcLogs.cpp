/*
 *    This file is a part of vgosDbProcLogs. vgosDbProcLogs is a part of
 *    CALC/SOLVE system and is designed to extract data, meteo parameters
 *    and cable calibrations, from stations log files and store them in
 *    the vgosDb format.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
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


#include <argp.h>
#include <unistd.h>



#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QRegularExpression>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif


#include <SgIdentities.h>
#include <SgKombFormat.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiSession.h>



#include "vgosDbProcLogs.h"
#include "VpStartupWizard.h"




void loadSettings(QSettings&);
void saveSettings(QSettings&, bool shouldInvokeSystemWideWizard);



VpSetup                         setup;

const QString                   origOrgName("NASA GSFC");
const QString                   origDmnName("gsfc.nasa.gov");
const QString                   origAppName("vgosDbProcLogs");


// HOPS's whims:
#ifdef OLD_HOPS
char                            progname[80];
int                             msglev = 2;
#endif

// for ARGP parser:
const char                     *argp_program_bug_address = "Sergei Bolotin <sergei.bolotin@nasa.gov>";

struct vdbpOptions
{
  QSettings                    *settings;
  QString                       altSetupName;
  QString                       altSetupAppName;
  QString                       inputArg;
  QString                       altPath2InputFiles;
  QList<QString>                stations;
  QList<QString>                kinds;
  QString                       orderOfMeteo;
  QMap<QString, int>            knownKinds;
  bool                          have2UseAltSetup;
  bool                          have2ForceWizard;
  bool                          shouldInvokeSystemWideWizard;
  bool                          isDryRun;
  bool                          useStdLocale;
  bool                          have2clearCableData;
  bool                          have2clearMeteoData;
  bool                          have2clearTsysData;
};


//
// a parser for ARGP:
static int parse_opt(int key, char *arg, struct argp_state *state)
{
  QString                       str("");
  struct vdbpOptions           *options=(struct vdbpOptions*)state->input;
  //
  switch (key)
  {
    case 'a':
      options->altSetupName = QString(arg);
      if (!options->altSetupName.contains("/..")) // are there any other elements of path that we should avoid?
      {
        options->altSetupAppName = origAppName + "-" + options->altSetupName;
        options->have2UseAltSetup = true;
      }
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): it is dangerous to use a string \"" + options->altSetupName +
          "\" as an alternative config name");
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): you can overwrite another file (un)intentionally");
        delete options->settings;
        exit(22);
      };
      break;
    case 'd':
      options->altPath2InputFiles = QString(arg);
      break;
    case 'k':
      str = QString(arg);
      if (options->knownKinds.contains(str))
        options->kinds.append(str);
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): got an unknown kind \"" + str + "\".");
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): the known kinds are: \"log\", \"cdms\", \"pcmt\" and \"met\".");
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): not sure what to do, quiting.");
        exit(1);
      };
      break;
    case 'l':
      options->useStdLocale = true;
      break;
    case 'o':
      options->orderOfMeteo = QString(arg).toUpper();
      break;
    case 'p':
      loadSettings(*options->settings);
      setup.print2stdOut();
      exit(0);
      break;
    case 'q':
      options->isDryRun = true;
      break;
    case 's':
      str = QString(arg);
      options->stations.append(str);
      logger->write(SgLogger::DBG, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": parse_opt(): the station \"" + str + "\" is added to the process list");
      break;
    case 'z':
      if (strcmp(arg, "c") == 0)
        options->have2clearCableData = true;
      else if (strcmp(arg, "m") == 0)
        options->have2clearMeteoData = true;
      else if (strcmp(arg, "t") == 0)
        options->have2clearTsysData = true;
      else
      {
       logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
          ": parse_opt(): unrecognized argument \"" + arg + "\" of \"-z\" option; "
          "expected: \"c\", \"m\" or \"t\"");
      };
      break;

    //
    case 'W':
      options->shouldInvokeSystemWideWizard = true;
      break;
    case 'w':
      options->have2ForceWizard = true;
      break;
    //
   case 'V':
      std::cout << qPrintable(vgosDbProcLogsVersion.name(SgVersion::NF_Petrov)) << "\n";
      exit(0);
      break;
    //
    case ARGP_KEY_ARG:
      if (1 < state->arg_num)
      {
        argp_usage(state);
      };
      options->inputArg = QString(arg);
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1 && 
        !(options->have2ForceWizard || options->shouldInvokeSystemWideWizard))
        argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
      break;
  };
  return 0;
};  
//    














//
QCoreApplication* createApplication(int &argc, char *argv[], bool isGuiEnabled)
{
  if (isGuiEnabled)
    return new QApplication(argc, argv);
  else
    return new QCoreApplication(argc, argv);
};



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
int main(int argc, char** argv)
{
  struct vdbpOptions            options;
  SgLogger                     *alHistory;
  SgLogger                     *alDriver;
  
  QString                       sessionName("");
  QString                       path2wrapperFile("");
  QString                       wrapperFileName("");
  QString                       userCommand("");
  QStringList                   lst;
  //
  if (setup.getUseAltMasterfileSuffixes())
    lst = setup.getMasterfileSuffixes();


#ifdef SWCONFIG
  const QString                 path2SystemWideConfig(SWCONFIG "/xdg");
#else
  const QString                 path2SystemWideConfig("");
#endif
  int                           rc;
  bool                          isOk;
  bool                          isFirstRun;
  bool                          isGuiEnabled;
  const char*                   envDisplay=NULL;

  rc = 0;
  options.settings = NULL;
  options.altSetupAppName = QString("");
  options.altSetupName = QString("");
  options.inputArg = QString("");
  options.altPath2InputFiles = QString("");
  options.orderOfMeteo = QString("");
  options.stations.clear();
  options.kinds.clear();
  options.knownKinds.clear();
  options.have2UseAltSetup = false;
  options.have2ForceWizard = false;
  options.shouldInvokeSystemWideWizard = false;
  options.isDryRun = false;
  options.useStdLocale = false;
  options.have2clearCableData = false;
  options.have2clearMeteoData = false;
  options.have2clearTsysData = false;

  options.knownKinds.insert("log",  0);
  options.knownKinds.insert("cdms", 1);
  options.knownKinds.insert("pcmt", 2);
  options.knownKinds.insert("met",  3);

  //
  // init:
  QCoreApplication::setOrganizationName(origOrgName);
  QCoreApplication::setOrganizationDomain(origDmnName);
  QCoreApplication::setApplicationName(origAppName);

  // set up path to the system wide settings:
  QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, path2SystemWideConfig);
  //
  options.settings = new QSettings;
  //
  isGuiEnabled = (envDisplay=getenv("DISPLAY"))!=NULL && 0<strlen(envDisplay);
  //
  QScopedPointer<QCoreApplication>
                                app(createApplication(argc, argv, isGuiEnabled));
  if (qobject_cast<QApplication *>(app.data()))
  {
    // do something with GUI
  }
  else
  {
    // do something for non-GUI
  };
  //
  // especially for HOPS:
#ifdef OLD_HOPS
  strcpy(progname, qPrintable("HOPS (on behalf of " + vgosDbProcLogsVersion.getSoftwareName() + ")"));
#endif
  //
  setup.setUpBinaryIdentities(QString(argv[0]));
  for (int i=0; i<argc; i++)
    userCommand += QString(argv[i]) + " ";
  userCommand.chop(1);


  //
  // ARGP setup:
  //
  struct argp_option            argp_options[] =
  {
    {0, 0, 0, 0, "General options:",      10},
    {"input-dir",           'd', "STRING",  0,
      "Use an alternative path STRING to search for input files"},
    {"std-locale",          'l', 0,         0,
      "Use the standard locale"},

    {0, 0, 0, 0, "Configuration control:",  11},
    {"alt",                 'a', "STRING",  0,
      "Use an alternative configuration STRING"},

    {0, 0, 0, 0, "Data processing control:", 13},
    {"kind",                'k', "STRING",  0,
      "Use only the specified by STRING kind of input. Known kinds are: log (FS log files), "
      "cdms (files with CDMS values), "
      "pcmt (files with PCMT values), "
      "met (files with meteorological data). "
      "There can be more than one \"-k\" option"},
    {"meteo-order",         'o', "STRING",  0,
      "An order of meteo parameters in /wx/ record of a log file. STRING is a string with at least 3 "
      "characters: T (temperature), P (pressure) and H (relative humidity). The order of the chars in "
      "STRING determines the order of data in a log file. The default is TPH. The values are case "
      "insensitive. The argument is supposed to be used with \"-s\" option, otherwise change of the "
      "order will affect all stations."},
    {"station",             's', "STRING",  0,
      "Use only station STRING (STRING can be either two-char station code or 8-chars station name). "
      "There can be more than one \"-s\" option"},
    {"zerofy",              'z', "CHAR",  0,
      "Reset data type CHAR. CHAR can be \"c\" (cable calibration), "
      "\"m\" (meteorological data) or \"t\" (tsys). There can be more than one \"-z\" option"},

    {0, 0, 0, 0, "Invocation of startup wizard:", 25},
    {"sys-wide-wizard",     'W', 0,         0,
      "Run startup wizard for the system-wide settings"},
    {"wizard",              'w', 0,         0,
      "Force call of the startup wizard"},

    {0, 0, 0, 0, "Operation modes:", -1},
    {"print-setup",         'p', 0,         0,
      "Print set up and exit"},
    {"dry-mode",            'q', 0,         0,
      "Process in a \"dry run\" mode: files will not be created, instead names of the files "
      "will be printed"},
    {"version",             'V', 0,         0,
      "Print program version"},
    //
    {0}
  };
  QString                       salute("vgosDbProcLogs is a program that extracts data from filed "
    "system log files and stores obtained info in vgosDb database. The mandatory argument DATABASE is "
    "a name of a database (with or without version part) or a name of wrapper file (absolute or "
    "relative to the default path of the vgosDb files).\v");
  
  salute += "The current version is:\n\t" + vgosDbProcLogsVersion.name() + " released on " +
            vgosDbProcLogsVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " +
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe utility vgosDbProcLogs is a part of nuSolve package. See the datails in "
    "\"vgosDbProcLogs User Guide\", a part of nuSolve distribution. You can get the latest version of "
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");
      
  struct argp                   argp={argp_options, parse_opt, "DATABASE", salute.toLatin1()};
    
  argp_parse (&argp, argc, argv, 0, 0, &options);
  //
  //
  isFirstRun = options.settings->allKeys().size()>0 ? false : true;
  //
  //
  if (options.have2UseAltSetup)
  {
    logger->write(SgLogger::INF, SgLogger::IO, vgosDbProcLogsVersion.name() +
      ": using alternative config name \"" + options.altSetupName + "\"");
    QSettings                  *altSettings=new QSettings(origOrgName, options.altSetupAppName);
    loadSettings(*altSettings);
    delete options.settings;
    options.settings = altSettings;
  }
  else
    loadSettings(*options.settings);
  //
  if (options.orderOfMeteo.size()) // check user input:
  {
    isOk = true;
    if (options.orderOfMeteo.size() < 3)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the size of meteo order string (" + QString("").setNum(options.orderOfMeteo.size()) + 
        ") is less than 3; ignoring");
      isOk = false;
    }
    else if (options.orderOfMeteo.count('T') < 1)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" does not contain temperature tag "
        "(T or t); ignoring");
      isOk = false;
    }
    else if (options.orderOfMeteo.count('P') < 1)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" does not contain pressure tag "
        "(P or p); ignoring");
      isOk = false;
    }
    else if (options.orderOfMeteo.count('H') < 1)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" does not contain relative humidity "
        "tag (H or h); ignoring");
      isOk = false;
    }
    else if (1 < options.orderOfMeteo.count('T'))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" contains more than one temperature "
        "tag (T or t); ignoring");
      isOk = false;
    }
    else if (1 < options.orderOfMeteo.count('P'))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" contains more than one pressure tag "
        "(P or p); ignoring");
      isOk = false;
    }
    else if (1 < options.orderOfMeteo.count('H'))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" contains more than one relative "
        "humidity tag (H or h); ignoring");
      isOk = false;
    };
    //
    if (!isOk) // reset the string:
    {
      logger->write(SgLogger::WRN, SgLogger::IO, vgosDbProcLogsVersion.name() +
        ": the meteo order string \"" + options.orderOfMeteo + "\" is invalid");
      options.orderOfMeteo = QString("");
    };
  };

  //
  //
  //
  if (isGuiEnabled)
  {
    if (options.have2ForceWizard || options.shouldInvokeSystemWideWizard ||
      options.settings->value("Version/StartUpWizard", 0).toInt() < VpStartupWizard::serialNumber())
    {
      if (options.shouldInvokeSystemWideWizard)
      {
        if (!options.have2UseAltSetup)
        {
          QSettings            *swSettings = 
                                  new QSettings(QSettings::SystemScope, origOrgName, origAppName);
          if (!swSettings->isWritable())
          {
            rc = 1;
            logger->write(SgLogger::ERR, SgLogger::IO, vgosDbProcLogsVersion.name() +
              ": cannot write system wide config");
            delete swSettings;
            return rc;
          };
          delete options.settings;
          options.settings = swSettings;
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, vgosDbProcLogsVersion.name() +
            ": using an alternative config name, system wide config edit request ignored");
      };
      //
      VpStartupWizard           startup(isFirstRun, options.have2ForceWizard, 
        options.shouldInvokeSystemWideWizard, options.settings);
      if (startup.exec() == QDialog::Rejected)
      {
        delete options.settings;
        logger->clearSpool();
        return rc;
      };
      //
      // save if user pressed "Finish":
      options.settings->setValue("Version/StartUpWizard", startup.serialNumber());
      saveSettings(*options.settings, options.shouldInvokeSystemWideWizard);
      //
      // it is ok, exitting:
      if (!options.inputArg.size())
      {
        delete options.settings;
        logger->clearSpool();
        return rc;
      };
    };
  }
  else if (options.have2ForceWizard || options.shouldInvokeSystemWideWizard)
  {
    rc = 1;
    logger->write(SgLogger::ERR, SgLogger::GUI, vgosDbProcLogsVersion.name() +
      ": cannot run graphical application.");
    delete options.settings;
    logger->clearSpool();
    return rc;
  };

  //
  // determine a name of a wrapper file:
  //
  int                           version(0);
  QRegularExpression            reWrapperFileName(".*\\.(wrp|wrap)$", 
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reOldDbName("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName("^(\\d{8})-([\\S]{2,12}).*");

  //
  // ok, this is a wrapper file name:
  if (reWrapperFileName.match(options.inputArg).hasMatch())
  {
    isOk = SgVlbiSession::guessSessionByWrapperFileName(options.inputArg,
      setup.path2(setup.getPath2VgosDbFiles()), path2wrapperFile, wrapperFileName,
        sessionName, version);
    if (isOk)
      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": got a session " + sessionName + " of version " + 
        QString("").setNum(version) + " from \"" + path2wrapperFile  + "\":\"" + 
        wrapperFileName + "\"");
    else
    {
      rc = 1;
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": cannot figure out a session from the input \"" + options.inputArg + "\"");
    };
  }
  else if ( reOldDbName.match(options.inputArg).hasMatch() ||
            reNewDbName.match(options.inputArg).hasMatch()  )
  {
    isOk = SgVlbiSession::guessWrapperFileNameBySession(options.inputArg,
      setup.path2(setup.getPath2VgosDbFiles()), 
      setup.identities().getAcAbbName(), setup.getUseOwnOnlyDatabase(),
      path2wrapperFile, wrapperFileName, sessionName, version);
    if (isOk)
      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": got a wrapper file \"" + wrapperFileName + 
        "\" from \"" + path2wrapperFile + "\" for the session " + sessionName + 
        " of version " + QString("").setNum(version));
    else
    {
      rc = 1;
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": cannot figure out a session from the input \"" + options.inputArg + "\"");
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": cannot guess input data from \"" + options.inputArg + "\"");
    rc = 1;
  };
  //
  //
  //
  alDriver = new SgLogger(800, setup.getHave2SavePerSessionLog(), "drv-unnamed.log");
  alDriver->setDirName(setup.path2(setup.getPath2AuxLogs()));
  alDriver->setIsNeedTimeMark(true);
  alDriver->setIsMute(true);
  alDriver->setLogFacility(SgLogger::ERR, 0xFFFFFFFF);
  alDriver->setLogFacility(SgLogger::WRN, 0xFFFFFFFF);
  alDriver->setLogFacility(SgLogger::INF, 0xFFFFFFFF);
  alDriver->setLogFacility(SgLogger::DBG, 0);
  if (setup.getHave2SavePerSessionLog())
    logger->attachSupplementLog("Driver", alDriver);
  //
  alHistory = new SgLogger(0, false, "");
  alHistory->setIsNeedTimeMark(false);
  alHistory->setIsMute(true);
  alHistory->setLogFacility(SgLogger::ERR, 0xFFFFFFFF);
  alHistory->setLogFacility(SgLogger::WRN, 0xFFFFFFFF);
  alHistory->setLogFacility(SgLogger::INF, 0xFFFFFFFF);
  alHistory->setLogFacility(SgLogger::DBG, 0);
  //
  //
  //
  isOk = false;
  SgVgosDb                     *vgosDb;
  SgVlbiSession                 session;
  //
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() + 
    ": starting");
  session.setPath2Masterfile(setup.path2(setup.getPath2MasterFiles()));
  //
  //
  //------------------------------------------------------------------------------------------
  //
  QString                       sLang(""), sLcAll("");
  if (!setup.getUseLocalLocale() || options.useStdLocale)
  {
    if (getenv("LANG"))
      sLang = QString(getenv("LANG"));
    setenv("LANG", "C", 1);
    if (getenv("LC_ALL"))
      sLcAll = QString(getenv("LC_ALL"));
    setenv("LC_ALL", "C", 1);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": the env.variable LANG was set to \"C\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": the env.variable LC_ALL was set to \"C\"");
  };
  //
  SgMJD                         startEpoch=SgMJD::currentMJD();

  vgosDb = new SgVgosDb(&setup.identities(), &vgosDbProcLogsVersion);
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
    ": the vgosDb object has been created");

  isOk = vgosDb->init(path2wrapperFile, wrapperFileName);
  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, vgosDbProcLogsVersion.name() + 
    ": initSessionFromOdb(): parsing of wrapper file " + path2wrapperFile + "/" + wrapperFileName + 
    (isOk?" complete":" failed"));
  
  if (vgosDb->getSessionName().size())
    alDriver->setFileName(vgosDb->getSessionName() + ".log");
  else
    alDriver->setFileName(sessionName + ".log");
  alDriver->rmLogFile();

  if (isOk && session.getDataFromVgosDb(vgosDb, false, isGuiEnabled, lst))
  {
    logger->attachSupplementLog("History", alHistory);

    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": Library ID: " + libraryVersion.name() + " released on " +
      libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": Driver  ID: " + vgosDbProcLogsVersion.name() + " released on " +
      vgosDbProcLogsVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));

    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": Host    ID: " + setup.identities().getMachineNodeName() + 
      " (Hw: " + setup.identities().getMachineMachineName() + 
      "; Sw: " + setup.identities().getMachineRelease() + 
      " version of " + setup.identities().getMachineSysName() + ")");

    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": User    ID: " + setup.identities().getUserName() + 
      " <" + setup.identities().getUserEmailAddress() + ">, " + setup.identities().getAcFullName());
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": User command: \"" + userCommand + "\"");
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": Input data  : " + path2wrapperFile + "/" + wrapperFileName);
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": the session " + session.getSessionCode() + " (" + session.getName() + 
      ") has been imported from vgosDb data set");

    //
    // clear data if necessary:
    if (options.have2clearCableData || options.have2clearMeteoData || options.have2clearTsysData)
    {
      QString                   sWhat("");
      QString                   sStns("all stations");

      if (options.have2clearCableData)
        sWhat += "cable cal";
      if (options.have2clearMeteoData)
        sWhat += ", meteorological";
      if (options.have2clearTsysData)
        sWhat += ", tsys";
      sWhat += " data";

      if (options.stations.size())
      {
        sStns = "selected station(s) ";
        for (int i=0; i<options.stations.size(); i++)
          sStns += options.stations.at(i) + ", ";
        sStns.chop(2);
      };
      //
      if (session.resetDataFromLogFiles(options.have2clearCableData, options.have2clearMeteoData, 
        options.have2clearTsysData, options.stations))
      {
        //
        logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
          ": the " + sWhat + " were reset for " + sStns + " of the session " + session.getSessionCode() + 
          " (" + session.getName() + ")");
        session.contemporaryHistory().addHistoryRecord("The " + sWhat + " were reset for " + sStns, 
          SgMJD::currentMJD().toUtc());
      }
      else
        logger->write(SgLogger::WRN, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
          ": clearing the " + sWhat + " for " + sStns + " of the session " + session.getSessionCode() + 
          " (" + session.getName() + ") failed");
    };
    //
    // parse log files and absorb the data:
    isOk = session.importDataFromLogFiles(setup.path2(setup.getPath2SessionFiles()),
      options.stations, options.kinds, &setup.getDefaultCableSignByStn(), 
      &setup.getRinexFileNameByStn(), &setup.getRinexPressureOffsetByStn(), 
      false, false, options.altPath2InputFiles, options.orderOfMeteo);
    //
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": import of data from station log files has " + (isOk?"been complete":"failed"));
    // save the session:
    if (isOk)
    {
      if (options.isDryRun)
        vgosDb->setOperationMode(SgNetCdf::OM_DRY_RUN);
      if (!(isOk=session.putDataIntoVgosDb(vgosDb)))
        rc = 1;
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": export of data into vgosDb set has " + (isOk?"been complete":"failed"));

      // report elapsed time:
      SgMJD                     finisEpoch=SgMJD::currentMJD();
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": the elapsed time to process " + QString("").setNum(session.observations().size()) +
        " observations is: " + interval2Str(finisEpoch - startEpoch) +
        " (" + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400.0) + " sec)", true);
    }
    else
    {
      rc = 1;
      logger->write(SgLogger::WRN, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
        ": no new data were acquired");
    };
    //
    delete vgosDb;
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": vgosDb object has been destroyed.");
  }
  else
  {
    rc = 1;
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": import data of " + path2wrapperFile + "/" + wrapperFileName + " failed");
  };

  //
  if (!setup.getUseLocalLocale() || options.useStdLocale)
  {
    setenv("LANG", qPrintable(sLang), 1);
    setenv("LC_ALL", qPrintable(sLcAll), 1);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": the env.variable LANG was set to \"" + sLang + "\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbProcLogsVersion.name() +
      ": the env.variable LC_ALL was set to \"" + sLcAll + "\"");
  };

  alDriver->clearSpool();
  logger->detachSupplementLog("Driver");
  logger->detachSupplementLog("History");
  delete alHistory;
  delete alDriver;
  delete options.settings;
  logger->clearSpool();
  return rc;
};


//
void loadSettings(QSettings& settings)
{
  int                           logLevel;
  //
  // setup.identities:
  setup.identities().setUserName( 
    settings.value("Identities/UserName", 
    setup.identities().getUserName()).toString());
  setup.identities().setUserEmailAddress( 
    settings.value("Identities/UserEmailAddress", 
    setup.identities().getUserEmailAddress()).toString());
  setup.identities().setUserDefaultInitials(
    settings.value("Identities/UserDefaultInitials",
    setup.identities().getUserDefaultInitials()).toString());
  setup.identities().setAcFullName(
    settings.value("Identities/AcFullName",
    setup.identities().getAcFullName()).toString());
  setup.identities().setAcAbbrevName(
    settings.value("Identities/AcAbbreviatedName",
    setup.identities().getAcAbbrevName()).toString());
  setup.identities().setAcAbbName( 
    settings.value("Identities/AcShortAbbreviatedName",
    setup.identities().getAcAbbName()).toString());

  // setup:
  // Pathes:
  setup.setPath2Home(
    settings.value("Setup/Path2Home", 
    setup.getPath2Home()).toString());
  setup.setPath2SessionFiles(
    settings.value("Setup/Path2SessionFiles",
    setup.getPath2SessionFiles()).toString());
  setup.setPath2VgosDbFiles(
    settings.value("Setup/Path2VgosDbFiles",
    setup.getPath2VgosDbFiles()).toString());
  setup.setPath2MasterFiles(
    settings.value("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles()).toString());
  setup.setPath2AuxLogs(
    settings.value("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs()).toString());
  setup.setHave2SavePerSessionLog(
    settings.value("Setup/Have2SavePerSessionLog",
    setup.getHave2SavePerSessionLog()).toBool());
  setup.setUseLocalLocale(
    settings.value("Setup/UseLocalLocale",
    setup.getUseLocalLocale()).toBool());
  //
  // adjust the logger:
  logger->setFileName(
    settings.value("Logger/FileName", 
    "vgosDbProcLogs.log").toString());
  logger->setDirName(setup.getPath2Home());
  logger->setCapacity(
    settings.value("Logger/Capacity", 400).toInt());
  logger->setIsStoreInFile(
    settings.value("Logger/IsStoreInFile", logger->getIsStoreInFile()).toBool());
  logger->setIsNeedTimeMark(
    settings.value("Logger/IsNeedTimeMark", logger->getIsNeedTimeMark()).toBool());
  logLevel = settings.value("Logger/LogLevel", 2).toInt();
  logger->setLogFacility(SgLogger::ERR, logLevel>=0?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::WRN, logLevel>=1?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::INF, logLevel>=2?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::DBG, logLevel==3?0xFFFFFFFF:0);
  //
  //
  // masterfile extensions:
  setup.setUseAltMasterfileSuffixes(
    settings.value("Setup/UseAltMasterFileExtensions",
    setup.getUseAltMasterfileSuffixes()).toBool());
  //
  QString                       str=setup.getMasterfileSuffixes().join(",");
  str = settings.value("Setup/MasterFileExtensions", str).toString();
  setup.setMasterfileSuffixes(str.split(QRegularExpression("[,;:]"), QString::SkipEmptyParts));
  //
  setup.setUseOwnOnlyDatabase(
    settings.value("Setup/UseOwnOnlyDatabase",
    setup.getUseOwnOnlyDatabase()).toBool());
  //
  int                           size, defaultCableSign;
  QString                       stationKey(""), rinexFileName("");
  double                        dP;
  if ( (size = settings.beginReadArray("Setup/DefaultCableSignByStn")) )
  {
    // remove default set up:
    setup.defaultCableSignByStn().clear();
    // load saved data:
    for (int i=0; i<size; ++i)
    {
      settings.setArrayIndex(i);
      stationKey = settings.value("StationKey").toString().leftJustified(8, ' ');
      defaultCableSign = settings.value("DefaultCableSign", 1).toInt();
      setup.defaultCableSignByStn().insert(stationKey, defaultCableSign);
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::CONFIG, vgosDbProcLogsVersion.name() +
      ": loadSettings(): no DefaultCableSignByStn found, using default set up");
  settings.endArray();
  //
  // RINEX stuff:
  // file names:
  if ( (size = settings.beginReadArray("Setup/RinexFileNameByStn")) )
  {
    // remove default set up:
    setup.rinexFileNameByStn().clear();
    // load saved data:
    for (int i=0; i<size; ++i)
    {
      settings.setArrayIndex(i);
      stationKey = settings.value("StationKey").toString().leftJustified(8, ' ');
      rinexFileName = settings.value("RinexFileName").toString();
      setup.rinexFileNameByStn().insert(stationKey, rinexFileName);
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::CONFIG, vgosDbProcLogsVersion.name() +
      ": loadSettings(): no RinexFileNameByStn found, using default set up");
  settings.endArray();
  // pressure offset:
  if ( (size = settings.beginReadArray("Setup/RinexPressureOffsetByStn")) )
  {
    // remove default set up:
    setup.rinexPressureOffsetByStn().clear();
    // load saved data:
    for (int i=0; i<size; ++i)
    {
      settings.setArrayIndex(i);
      stationKey = settings.value("StationKey").toString().leftJustified(8, ' ');
      dP = settings.value("RinexPressureOffset", 0.0).toDouble();
      setup.rinexPressureOffsetByStn().insert(stationKey, dP);
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::CONFIG, vgosDbProcLogsVersion.name() +
      ": loadSettings(): no RinexPressureOffsetByStn found, using default set up");
  settings.endArray();
};



//
void saveSettings(QSettings& settings, bool shouldInvokeSystemWideWizard)
{
  // setup.identities:
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Identities/UserName",
      setup.identities().getUserName());
    settings.setValue("Identities/UserEmailAddress",
      setup.identities().getUserEmailAddress());
    settings.setValue("Identities/UserDefaultInitials",
      setup.identities().getUserDefaultInitials());
  };
  settings.setValue("Identities/AcFullName",
    setup.identities().getAcFullName());
  settings.setValue("Identities/AcAbbreviatedName",
    setup.identities().getAcAbbrevName());
  settings.setValue("Identities/AcShortAbbreviatedName",
    setup.identities().getAcAbbName());
  // setup:
  // Pathes:
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Setup/Path2Home",
      setup.getPath2Home());
  };
  settings.setValue("Setup/Path2SessionFiles",
    setup.getPath2SessionFiles());
  settings.setValue("Setup/Path2VgosDbFiles",
    setup.getPath2VgosDbFiles());
  settings.setValue("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles());
  settings.setValue("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs());
  settings.setValue("Setup/Have2SavePerSessionLog",
    setup.getHave2SavePerSessionLog());
  settings.setValue("Setup/UseLocalLocale",
    setup.getUseLocalLocale());
  //
  // masterfile extensions:
  settings.setValue("Setup/UseAltMasterFileExtensions",
    setup.getUseAltMasterfileSuffixes());
  //
  settings.setValue("Setup/MasterFileExtensions", 
    setup.getMasterfileSuffixes().join(","));
  //
  settings.setValue("Setup/UseOwnOnlyDatabase",
    setup.getUseOwnOnlyDatabase());
  //
  // store logger's config:
  settings.setValue("Logger/FileName", 
    logger->getFileName());
  //  logger->getDirName();
  settings.setValue("Logger/Capacity", 
    logger->getCapacity());
  settings.setValue("Logger/IsStoreInFile", 
    logger->getIsStoreInFile());
  settings.setValue("Logger/IsNeedTimeMark", 
    logger->getIsNeedTimeMark());
  //
  //
  settings.remove("Setup/DefaultCableSignByStn");
  settings.beginWriteArray("Setup/DefaultCableSignByStn", setup.getDefaultCableSignByStn().size());
  int                           idx(0);
  for (QMap<QString, int>::const_iterator it=setup.getDefaultCableSignByStn().begin();
    it!=setup.getDefaultCableSignByStn().end(); ++it, ++idx)
  {
    settings.setArrayIndex(idx);
    settings.setValue("StationKey", it.key());
    settings.setValue("DefaultCableSign", it.value());
  };
  settings.endArray();
  //
  // RINEX stuff:
  // RINEX file names:
  settings.remove("Setup/RinexFileNameByStn");
  settings.beginWriteArray("Setup/RinexFileNameByStn", setup.getRinexFileNameByStn().size());
  idx = 0;
  for (QMap<QString, QString>::const_iterator it=setup.getRinexFileNameByStn().begin();
    it!=setup.getRinexFileNameByStn().end(); ++it, ++idx)
  {
    settings.setArrayIndex(idx);
    settings.setValue("StationKey", it.key());
    settings.setValue("RinexFileName", it.value());
  };
  settings.endArray();
  // pressure offset:
  settings.remove("Setup/RinexPressureOffsetByStn");
  settings.beginWriteArray("Setup/RinexPressureOffsetByStn", setup.getRinexPressureOffsetByStn().size());
  idx = 0;
  for (QMap<QString, double>::const_iterator it=setup.getRinexPressureOffsetByStn().begin();
    it!=setup.getRinexPressureOffsetByStn().end(); ++it, ++idx)
  {
    settings.setArrayIndex(idx);
    settings.setValue("StationKey", it.key());
    settings.setValue("RinexPressureOffset", it.value());
  };
  settings.endArray();
  
};
/*=====================================================================================================*/





/*=====================================================================================================*/
