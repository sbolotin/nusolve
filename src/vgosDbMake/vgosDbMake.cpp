/*
 *
 *    This file is a part of vgosDbMake. vgosDbMake is a part of CALC/SOLVE
 *    system and is designed to convert correlator output data into VgosDb format.
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
#include <signal.h>
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
#    include <QtWidgets/QApplication>
#else
#    include <QtGui/QApplication>
#endif


#include <SgIdentities.h>
#include <SgKombFormat.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiSession.h>



#include "vgosDbMake.h"
#include "VmStartupWizard.h"



void loadSettings(QSettings&);
void saveSettings(QSettings&, bool shouldInvokeSystemWideWizard);



VmSetup                         setup;

const QString                   origOrgName("NASA GSFC");
const QString                   origDmnName("gsfc.nasa.gov");
const QString                   origAppName("vgosDbMake");

// HOPS's whims:
#ifdef OLD_HOPS
char                            progname[80];
int                             msglev = 2;
#endif

const int                       defaultMasterfileVersion(1);

// for ARGP parser:
const char                     *argp_program_bug_address = "Sergei Bolotin <sergei.bolotin@nasa.gov>";

struct vdbmOptions
{
  QSettings                    *settings;
  QString                       altSetupName;
  QString                       altSetupAppName;
  QString                       inputArg;
  QString                       mapFileName;
  QString                       altOutputDir;
  QString                       altDatabaseName;
  QString                       altCorrelatorName;
  QString                       correlatorReportFileName;
  QList<QString>                fringeErrorCodes2Skip;
  int                           altExpSerialNumber;
  int                           expectedMasterfileVersion;
  bool                          have2UseAltSetup;
  bool                          have2ForceWizard;
  bool                          shouldInvokeSystemWideWizard;
  bool                          isDryRun;
  bool                          useStdLocale;
  bool                          need2correctRefClocks;
  bool                          have2SaveAltOutputDir;
  bool 													acceptAllRootFiles;
};


//    
// a parser for ARGP:
static int parse_opt(int key, char *arg, struct argp_state *state)
{
  int                           n;
  bool                          is;
  QString                       str("");
  struct vdbmOptions           *options=(struct vdbmOptions*)state->input;
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
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbMakeVersion.name() +
          ": parse_opt(): it is dangerous to use a string \"" + options->altSetupName +
          "\" as an alternative config name");
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbMakeVersion.name() +
          ": parse_opt(): you can overwrite another file (un)intentionally");
        delete options->settings;
        exit(22);
      };
      break;
    case 'd':
      options->altDatabaseName = QString(arg);
      break;
    case 'e':
      options->fringeErrorCodes2Skip << QString(arg).toUpper();
      break;
    case 'f':
      n = QString(arg).toInt(&is);
      if (is)
      {
        if (n==1 || n==2)
          options->expectedMasterfileVersion = n;
        else
          logger->write(SgLogger::ERR, SgLogger::IO, vgosDbMakeVersion.name() +
            ": parse_opt(): masterfile format of version " + arg + " is not supported");
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, vgosDbMakeVersion.name() +
          ": parse_opt(): cannot convert \"" + arg + "\" to int");
        options->expectedMasterfileVersion = defaultMasterfileVersion;
      };
      break;
    case 'g':
      options->acceptAllRootFiles = true;
      break;
    case 'l':
      options->useStdLocale = true;
      break;
    case 'm':
      options->mapFileName = QString(arg);
      break;
    case 'O':
      options->have2SaveAltOutputDir = true;
      options->altOutputDir = QString(arg);
      break;
    case 'o':
      options->altOutputDir = QString(arg);
      break;
    case 'p':
      loadSettings(*options->settings);
      setup.print2stdOut();
      exit(0);
      break;
    case 'q':
      options->isDryRun = true;
      break;
    case 'r':
      options->altCorrelatorName = QString(arg);
      break;
    case 's':
      n = QString(arg).toInt(&is);
      if (is)
        options->altExpSerialNumber = n;
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, vgosDbMakeVersion.name() +
          ": parse_opt(): cannot convert \"" + arg + "\" to int");
        options->altExpSerialNumber = 0;
      };
      break;
    case 't':
      options->correlatorReportFileName = QString(arg);
      break;
    case 'x':
      options->need2correctRefClocks = true;
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
      std::cout << qPrintable(vgosDbMakeVersion.name(SgVersion::NF_Petrov)) << "\n";
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





SgVlbiSessionInfo::OriginType determineInputType(const QString& path2data);


/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
int main(int argc, char** argv)
{
  struct vdbmOptions            options;
  SgVlbiSessionInfo::OriginType inputType;
  QString                       userCommand("");
  SgLogger                     *alHistory;
  SgLogger                     *alDriver;
  QList<QString>                lst;
  //
  if (setup.getUseAltMasterfileSuffixes())
    lst = setup.getMasterfileSuffixes();


#ifdef SWCONFIG
  const QString                 path2SystemWideConfig(SWCONFIG "/xdg");
#else
  const QString                 path2SystemWideConfig("");
#endif
  int                           rc;
  bool                          isGuiEnabled;
  bool                          isFirstRun;
  const char*                   envDisplay=NULL;

  rc = 0;
  options.settings = NULL;
  options.altSetupAppName = QString("");
  options.altSetupName = QString("");
  options.inputArg = QString("");
  options.mapFileName = QString("");
  options.altOutputDir = QString("");
  options.altDatabaseName = QString("");
  options.altCorrelatorName = QString("");
  options.correlatorReportFileName = QString("");
  options.fringeErrorCodes2Skip.clear();
  options.altExpSerialNumber = 0;
  options.expectedMasterfileVersion = defaultMasterfileVersion;
  options.have2UseAltSetup = false;
  options.have2ForceWizard = false;
  options.shouldInvokeSystemWideWizard = false;
  options.isDryRun = false;
  options.useStdLocale = false;
  options.need2correctRefClocks = false;
  options.have2SaveAltOutputDir = false;
  options.acceptAllRootFiles = false;


  //
  // init:
  QCoreApplication::setOrganizationName(origOrgName);
  QCoreApplication::setOrganizationDomain(origDmnName);
  QCoreApplication::setApplicationName(origAppName);
  //
  // set up path to the system wide settings:
  QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, path2SystemWideConfig);
  //
  options.settings = new QSettings;
  //
  isGuiEnabled = (envDisplay=getenv("DISPLAY"))!=NULL && strlen(envDisplay)>0;
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
  strcpy(progname, qPrintable("HOPS (on behalf of " + vgosDbMakeVersion.getSoftwareName() + ")"));
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
    {0, 0, 0, 0, "General options:",         10},
    {"std-locale",          'l', 0,           0,
      "Use the standard locale."},
    {"output-dir",          'o', "STRING",    0,
      "Use an alternative path STRING to save files in vgosDb format."},
    {"mf-version",          'f', "NUM",       0,
      "Set the expected masterfile format version to NUM. The possible format versions are 1 and 2."},
    
    {0, 0, 0, 0, "Configuration control:",   11},
    {"alt",                 'a', "STRING",    0,
      "Use an alternative configuration STRING."},

    {0, 0, 0, 0, "Database edit options:",   12},
    {"database",            'd', "STRING",    0,
      "Set database name to STRING."},
    {"correlator",          'r', "STRING",    0,
      "Set correlator name to STRING."},
    {"exp-sn",              's', "NUM",       0,
      "Set experiment serial number to NUM."},

    {0, 0, 0, 0, "Data extraction control:", 13},
    {"exclude",             'e', "CHAR",      0,
      "exclude observations with fringe error code CHAR. If CHAR is \"*\" only observations that have "
      "no fringe error code will be extracted. There can be more than one \"-e\" option, "
      "e.g.: -eA -eB."},
    {"all-rootfile-names",  'g', 0,           0,
      "Assume root file names are in arbitrary form."},
    {"map",                 'm', "STRING",    0,
      "Set a name map file to STRING."},
    {"report",              't', "STRING",    0,
      "Set a correlator report file to STRING."},
    {"adjust-ref-stn",      'x', 0,  0,
      "KOMB files input only: adjust delays and rates for a reference station clock offset "
      "(experimental mode)."},

    {0, 0, 0, 0, "Invocation of startup wizard:", 25},
    {"sys-wide-wizard",     'W', 0,           0,
      "Run startup wizard for the system-wide settings."},
    {"wizard",              'w', 0,           0,
      "Force call of the startup wizard."},
     
    {0, 0, 0, 0, "Operation modes:", -1},
    {"print-setup",         'p', 0,           0,
      "Print set up and exit."},
    {"dry-mode",            'q', 0,           0,
      "Process in a \"dry run\" mode: files will not be created, instead names of the files "
      "will be printed."},
    {"version",             'V', 0,           0,
      "Print program version."},
    //
    {0}
  };
  QString                       salute("vgosDbMake is a program that extracts data from fringe or "
    "KOMB files and stores obtained info in vgosDb database. The mandatory argument INPUT_DIR is a "
    "name of a directory where the software searches the correlator files.\v");

  salute += "The current version is:\n\t" + vgosDbMakeVersion.name() + " released on " +
            vgosDbMakeVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " +
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe utility vgosDbMake is a part of nuSolve package. See the datails in "
    "\"vgosDbMake User Guide\", a part of nuSolve distribution. You can get the latest version of "
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");

  struct argp                   argp={argp_options, parse_opt, "INPUT_DIR", salute.toLatin1()};

  argp_parse (&argp, argc, argv, 0, 0, &options);

  isFirstRun = options.settings->allKeys().size()>0 ? false : true;
  //
  //
  if (options.have2UseAltSetup)
  {
    logger->write(SgLogger::INF, SgLogger::IO, vgosDbMakeVersion.name() +
      ": using alternative config name \"" + options.altSetupName + "\"");
    QSettings                  *altSettings=new QSettings(origOrgName, options.altSetupAppName);
    loadSettings(*altSettings);
    delete options.settings;
    options.settings = altSettings;
  }
  else
    loadSettings(*options.settings);
  //
  //
  //
  if (isGuiEnabled)
  {
    if (options.have2ForceWizard || options.shouldInvokeSystemWideWizard ||
        options.settings->value("Version/StartUpWizard", 0).toInt() < VmStartupWizard::serialNumber())
    {
      if (options.shouldInvokeSystemWideWizard)
      {
        if (!options.have2UseAltSetup)
        {
          QSettings            *swSettings = 
                                  new QSettings(QSettings::SystemScope, origOrgName, origAppName);
          if (!swSettings->isWritable())
          {
            logger->write(SgLogger::ERR, SgLogger::IO, vgosDbMakeVersion.name() +
              ": Cannot write system wide config");
            delete swSettings;
            return 1;
          };
          delete options.settings;
          options.settings = swSettings;
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO, vgosDbMakeVersion.name() +
            ": Using an alternative config name, system wide config edit request ignored");
      };
      //
      VmStartupWizard           startup(isFirstRun, options.have2ForceWizard, 
        options.shouldInvokeSystemWideWizard, options.settings);
      if (startup.exec() == QDialog::Rejected)
      {
        delete options.settings;
        logger->clearSpool();
        return rc; // rc == 0, it is ok
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
    logger->write(SgLogger::ERR, SgLogger::GUI, vgosDbMakeVersion.name() +
      ": cannot run graphical application.");
    delete options.settings;
    logger->clearSpool();
    return rc;
  };
 
  // alternate output directory:
  if (options.altOutputDir.size())
  {
    setup.setPath2VgosDbFiles(options.altOutputDir);
    logger->write(SgLogger::DBG, SgLogger::GUI, vgosDbMakeVersion.name() +
      ": using the directory \"" + setup.getPath2VgosDbFiles() + "\" for output");
    if (options.have2SaveAltOutputDir)
    {
      options.settings->setValue("Setup/Path2VgosDbFiles",
        setup.getPath2VgosDbFiles());
      logger->write(SgLogger::INF, SgLogger::GUI, vgosDbMakeVersion.name() +
        ": the default output direcory was changed to \"" + setup.getPath2VgosDbFiles() + "\"");
    };
  };
  //
  //===================== begin here: =====================
  // 
  // if input is not an absolute directory:
  if (options.inputArg.at(0) != QChar('/'))
    options.inputArg = setup.path2(setup.getPath2InputFiles()) + "/" + options.inputArg;

  QFileInfo                     fInfo(options.inputArg);
  if (!fInfo.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the provided input direcory, " + options.inputArg + ", does not exist");
    logger->clearSpool();
    return 1;
  };
  if (!fInfo.isDir())
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the provided input is a file, not a directory.");
    logger->clearSpool();
    return 1;
  };
  if (!fInfo.isReadable())
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the provided input direcory is unreadable.");
    logger->clearSpool();
    return 1;
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
  bool                          isOk(false);
  SgVgosDb                     *vgosDb;
  SgVlbiSession                 session;
  //
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() + 
    ": starting");
  session.setPath2Masterfile(setup.path2(setup.getPath2MasterFiles()));
  //
  //
  inputType = determineInputType(options.inputArg);
  if (inputType == SgVlbiSessionInfo::OT_UNKNOWN)
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": directory \"" + options.inputArg + "\": unable to determine the type of input files");
    logger->clearSpool();
    return 1;
  }
  else if (inputType!=SgVlbiSessionInfo::OT_MK4 && inputType!=SgVlbiSessionInfo::OT_KOMB)
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": directory \"" + options.inputArg + "\": unsupported type of input files");
    logger->clearSpool();
    return 1;
  };
  //
  // adjust correlator input file name (if it is not an absolute path):
  if (options.correlatorReportFileName.size() && options.correlatorReportFileName.at(0) != QChar('/'))
    options.correlatorReportFileName = setup.path2(setup.getPath2InputFiles()) + "/" + 
      options.correlatorReportFileName;
  //
  //------------------------------------------------------------------------------------------
  logger->attachSupplementLog("History", alHistory);
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": Library ID: " + libraryVersion.name() + " released on " +
    libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": Driver  ID: " + vgosDbMakeVersion.name() + " released on " + 
    vgosDbMakeVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));

  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": Host    ID: " + setup.identities().getMachineNodeName() + 
    " (Hw: " + setup.identities().getMachineMachineName() + 
    "; Sw: " + setup.identities().getMachineRelease() + 
    " version of " + setup.identities().getMachineSysName() + ")");

  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": User    ID: " + setup.identities().getUserName() + 
    " <" + setup.identities().getUserEmailAddress() + ">, " + setup.identities().getAcFullName());
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": User command: \"" + userCommand + "\"");
  logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
    ": Input data  : \"" + options.inputArg + "\"");
  //
  if (options.altExpSerialNumber)
  {
    session.setExperimentSerialNumber(options.altExpSerialNumber);
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": session serial number was mannualy set to " + 
      QString("").sprintf("%d", session.getExperimentSerialNumber()));
  };
  session.setExpectedMasterfileVersion(options.expectedMasterfileVersion);

  QString                       sLang(""), sLcAll("");
  if (!setup.getUseLocalLocale() || options.useStdLocale)
  {
    if (getenv("LANG"))
      sLang = QString(getenv("LANG"));
    setenv("LANG", "C", 1);
    if (getenv("LC_ALL"))
      sLcAll = QString(getenv("LC_ALL"));
    setenv("LC_ALL", "C", 1);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the env.variable LANG was set to \"C\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the env.variable LC_ALL was set to \"C\"");
  };

  SgMJD                         startEpoch=SgMJD::currentMJD();
  if ((isOk=inputType==SgVlbiSessionInfo::OT_MK4?
        session.getDataFromFringeFiles(options.inputArg, options.altDatabaseName, 
          options.altCorrelatorName, options.correlatorReportFileName, options.mapFileName, 
          options.fringeErrorCodes2Skip, lst, options.acceptAllRootFiles) :
        session.getDataFromKombFiles(options.inputArg, options.altDatabaseName,
          options.altCorrelatorName, options.correlatorReportFileName, options.mapFileName,
          options.need2correctRefClocks, lst)))
  {
    alDriver->setFileName(session.getName() + ".log");
    alDriver->rmLogFile();

    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the session " + session.getSessionCode() + " (" + session.getName() + 
      ") has been imported from fringes files");
    vgosDb = new SgVgosDb(&setup.identities(), &vgosDbMakeVersion);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": vgosDb object has been created");
    // udjust a correlator name:
    if (session.getCorrelatorName().size() == 0)
    {
      session.setCorrelatorName(setup.identities().getAcAbbName());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
        ": the empty correlator name was adjusted to \"" + session.getCorrelatorName() + "\"");
    };

    if ((isOk=vgosDb->init(&session)))
    {
      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
        ": the vgosDb object has been prepared to save the new session in vgosDb format");

      vgosDb->setPath2RootDir(setup.path2(setup.getPath2VgosDbFiles()) + "/" + 
        QString("").sprintf("%04d", session.getTStart().calcYear()) + "/" + session.getName());
      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
        ": the path was set to " + vgosDb->getPath2RootDir());
  
      if (options.isDryRun)
        vgosDb->setOperationMode(SgNetCdf::OM_DRY_RUN);

      // strore it in vgosDb format:
      isOk = session.putDataIntoVgosDb(vgosDb);
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
        ": export of data into vgosDb tree has been " + (isOk?"complete":"failed"));
      if (!isOk)
        rc = 1;
      else
      {
        SgMJD                   finisEpoch=SgMJD::currentMJD();
        logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbMakeVersion.name() +
          ": the elapsed time to process " + QString("").setNum(session.observations().size()) + 
          " observations is: " + interval2Str(finisEpoch - startEpoch) + 
          " (" + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400.0) + " sec)", true);
      };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
        ": initializing of the vgosDb object has failed");
      rc = 1;
    };
  
    delete vgosDb;
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": vgosDb object has been destroyed.");
  }
  else
  {
    rc = 1;
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": import data from \"" + options.inputArg + "\" failed");
  };

  if (!setup.getUseLocalLocale() || options.useStdLocale)
  {
    setenv("LANG", qPrintable(sLang), 1);
    setenv("LC_ALL", qPrintable(sLcAll), 1);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
      ": the env.variable LANG was set to \"" + sLang + "\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbMakeVersion.name() +
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
  setup.setPath2InputFiles(
    settings.value("Setup/Path2InputFiles",
    setup.getPath2InputFiles()).toString());
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
    "vgosDbMake.log").toString());
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
  settings.setValue("Setup/Path2InputFiles",
    setup.getPath2InputFiles());
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
};



//
SgVlbiSessionInfo::OriginType determineInputType(const QString& path2data)
{
  QDir                          dir(path2data);
  QList<QString>                files2read;
  QStringList                   entryList;
  QRegExp                       reKombFileName("B[0-9]{2,}");
  QRegExp                       reScanDirName("[0-9]{3}-[0-9]{4}[a-zA-Z]{0,1}");
  //
  // first, test for typical KOMB data set:
  files2read = dir.entryList(QDir::Files | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
  for (int i=0; i<files2read.size(); i++)
    if (files2read.at(i).contains(reKombFileName))
      entryList << files2read.at(i);
  if (entryList.size() >= 3)
  {
    logger->write(SgLogger::DBG, SgLogger::SESSION,
      "determineInputType(): directory " + path2data + ": looks like KOMB output");
    return SgVlbiSessionInfo::OT_KOMB;
  };
  //
  // then, test for typical MK4 data set:
  files2read.clear();
  entryList.clear();
  files2read = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
  for (int i=0; i<files2read.size(); i++)
//  if (dirList.at(i).contains(reScanDirName)) // sometimes, they send nonstandard names
      entryList << files2read.at(i);
  if (entryList.size() >= 0)
  {
    logger->write(SgLogger::DBG, SgLogger::SESSION,
      "determineInputType(): directory " + path2data + ": looks like FRINGE output");
    return SgVlbiSessionInfo::OT_MK4;
  };
  //
  // else...
  return SgVlbiSessionInfo::OT_UNKNOWN;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
