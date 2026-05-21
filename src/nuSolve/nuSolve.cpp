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

#include "nuSolve.h"

#include <argp.h>
#include <signal.h>
#include <unistd.h>

#include <stdio.h>
#include <omp.h>



#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QSettings>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QApplication>
#   include <QtWidgets/QMessageBox>
#else
#   include <QtGui/QApplication>
#   include <QtGui/QMessageBox>
#endif

#include <QtGui/QFont>
#include <QtGui/QIcon>




#include "NsMainWindow.h"
#include "NsStartupWizard.h"
#include "NsSessionHandler.h"
#include "NsScrSupport.h"



SgTaskConfig                    config;
SgParametersDescriptor          parametersDescriptor;
NsSetup                         setup;

SgVersion                       storedLibraryVersion;
SgVersion                       storedNuSolveVersion;
const int                       currentSettingsVersion(20170328);

const QString                   origOrgName("NASA GSFC");
const QString                   origDmnName("gsfc.nasa.gov");
const QString                   origAppName("nuSolve");

// HOPS's whims:
#ifdef OLD_HOPS
char                            progname[80];
int                             msglev=2;
#endif

// for ARGP parser:
const char                     *argp_program_bug_address = "Sergei Bolotin <sergei.bolotin@nasa.gov>";

struct nsOptions
{
  QSettings                    *settings;
  QString                       altSetupName;
  QString                       altSetupAppName;
  bool                          have2UseAltSetup;
  
  bool                          isForcedCatalogMode;
  bool                          isForcedStandaloneMode;
  bool                          useDefaultSetup;
  SgVlbiSessionInfo::OriginType ofFmt;
  bool                          have2SkipAutomaticProcessing;
  bool                          have2ForceAutomaticProcessing;
  bool                          isNeedSignalHandler;

  bool                          have2ForceWizard;
  bool                          shouldInvokeSystemWideWizard;
  bool                          have2LoadImmatureSession;
  QString                       scriptFileName;
  QString                       databaseName;
  QList<QString>                args;

};

//
// a parser for ARGP:
static int parse_opt(int key, char *arg, struct argp_state *state)
{
  QString                       str("");
  struct nsOptions             *options=(struct nsOptions*)state->input;
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
        logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
          ": parse_opt(): it is dangerous to use a string \"" + options->altSetupName +
          "\" as an alternative config name");
        logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
          ": parse_opt(): you can overwrite another file (un)intentionally");
        delete options->settings;
        exit(22);
      };
      break;
    case 'c':
      options->isForcedCatalogMode = true;
      break;
    case 'd':
      options->useDefaultSetup = true;
      logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
        ": parse_opt(): using default setup");
     break;
    case 'f':
      if (strcmp(arg, "dbh") == 0)
        options->ofFmt = SgVlbiSessionInfo::OT_DBH;
      else if (strcmp(arg, "vgos") == 0)
        options->ofFmt = SgVlbiSessionInfo::OT_VDB;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
          ": parse_opt(): the data storage format \"" + arg + "\" is unrecognized");
        delete options->settings;
        exit(1);
      };
      break;
    case 'i':
      options->isNeedSignalHandler = false;
      logger->write(SgLogger::INF, SgLogger::IO, nuSolveVersion.name() +
        ": parse_opt(): will not interrupt signals");
      break;
    case 'l':
      options->have2LoadImmatureSession = true;
      break;
    case 'm':
      options->have2ForceAutomaticProcessing = true;
      break;
    case 'n':
      options->have2SkipAutomaticProcessing = true;
      break;
    case 'q':
      break;
    case 's':
      options->isForcedStandaloneMode = true;
      break;
    case 't':
      options->scriptFileName = QString(arg);
      break;
    case 'W':
      options->shouldInvokeSystemWideWizard = true;
      break;
    case 'w':
      options->have2ForceWizard = true;
      break;

    //
   case 'V':
      std::cout << qPrintable(nuSolveVersion.name(SgVersion::NF_Petrov)) << "\n";
      exit(0);
      break;
    //
    case ARGP_KEY_ARG:
      if (options->scriptFileName.size())
        options->args << QString(arg);
      else
        options->databaseName = QString(arg);
      break;
    case ARGP_KEY_END:
      //if (state->arg_num < 1)
      //  argp_usage (state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
      break;
  };
  return 0;
};
//
//








void loadSettings(QSettings&);
void loadSettingsOldStyle(QSettings&);
void saveSettings(QSettings&, bool shouldInvokeSystemWideWizard);
void checkSettings(QSettings&);
void rmOldStyleSettings(QSettings&);
int processSession(const QString& databaseName, SgVlbiSessionInfo::OriginType fmt);


//      saved actions for signals:
//      RT errors:
struct sigaction                saveSIGFPE;
struct sigaction                saveSIGILL;
struct sigaction                saveSIGSEGV;
struct sigaction                saveSIGABRT;
struct sigaction                saveSIGBUS;

//      termination signals:
struct sigaction                saveSIGINT;
struct sigaction                saveSIGHUP;
struct sigaction                saveSIGTERM;

//      default:
struct sigaction                saveSIGDFL;



//
void handlerSIGs(int signum)
{
  QString str("Signal handler: catched the \"");
  str += strsignal(signum);
  str += "\" signal";
  std::cout << qPrintable(str) << "; saving the Log...\n";
  logger->write(SgLogger::ERR, SgLogger::DATA, str);
  logger->clearSpool();

  QMessageBox::critical(0, nuSolveVersion.name() + ": signal handler",
                        QString("An internal error occurred.\n\n") + str +
                        ".\n\nThe application will now exit.");

  // special handling:
  switch(signum)
  {
  //run-time errors:
  case SIGFPE:  
    sigaction(signum, &saveSIGFPE, NULL); 
    break;
  case SIGILL:  
    sigaction(signum, &saveSIGILL, NULL); 
    break;
  case SIGSEGV: 
    sigaction(signum, &saveSIGSEGV,NULL); 
    break;
  case SIGABRT: 
    sigaction(signum, &saveSIGABRT,NULL); 
    break;
  case SIGBUS:  
    sigaction(signum, &saveSIGBUS, NULL); 
    break;
  //interups:
  case SIGINT:  
    sigaction(signum, &saveSIGINT, NULL); 
    break;
  case SIGHUP:  
    sigaction(signum, &saveSIGHUP, NULL); 
    break;
  case SIGTERM: 
    sigaction(signum, &saveSIGTERM,NULL); 
    break;
  default:
    std::cout << "got an unexpected signal, setting handler to default.\n";
    sigaction(signum, &saveSIGDFL, NULL);
  break;
  };
  std::cout << "Signal handler: processing default handler..\n";
  raise(signum);
};



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
int main(int argc, char** argv)
{
  struct sigaction              act;
  struct nsOptions              options;
  int                           rc;
  bool                          isFirstRun;
#ifdef SWCONFIG
  const QString                 path2SystemWideConfig(SWCONFIG "/xdg");
#else
  const QString                 path2SystemWideConfig("");
#endif
  
  options.altSetupAppName = QString("");
  options.altSetupName = QString("");
  options.have2UseAltSetup = false;
  options.isForcedCatalogMode = false;
  options.isForcedStandaloneMode=false;
  options.useDefaultSetup = false;
  options.ofFmt = SgVlbiSessionInfo::OT_VDB;
  options.have2SkipAutomaticProcessing = false;
  options.have2ForceAutomaticProcessing = false;
  options.isNeedSignalHandler = true;
  options.have2ForceWizard = false;
  options.shouldInvokeSystemWideWizard = false;
  options.have2LoadImmatureSession = false;
  options.scriptFileName = QString("");
  options.databaseName = QString("");
  options.args.clear();

  //
  saveSIGDFL.sa_handler=SIG_DFL;
  sigemptyset(&saveSIGDFL.sa_mask);
  saveSIGDFL.sa_flags = 0;

  // init:
  QCoreApplication::setOrganizationName(origOrgName);
  QCoreApplication::setOrganizationDomain(origDmnName);
  QCoreApplication::setApplicationName(origAppName);
  //
  // set up path to the system wide settings:
  QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, path2SystemWideConfig);
  //
  // HOPS's whims:
#ifdef OLD_HOPS
  strcpy(progname, qPrintable("HOPS (on behalf of " + nuSolveVersion.getSoftwareName() + ")"));
#endif
//
//    "The rest of arguments are treating as arguments of a script if \"-t\" option is present,\n"
//    "otherwice turns the software into a command line mode and use the argument as a name\n"
//    "of a database to process. The command line mode supposed to be applied to the INT type\n"
//    "of sessions only.\n";
  //
  QString                       command("");
  for (int i=0; i<argc; i++)
    command += QString(argv[i]) + " ";
  command.chop(1);
  //
  setup.setUpBinaryIdentities(QString(argv[0]));
  setup.identities().setUserCommand(command);
  //
  options.settings = new QSettings;

  //
  // ARGP setup:
  //
  struct argp_option            argp_options[] =
  {
    {0, 0, 0, 0, "Configuration control:", 10},
    {"alt",                 'a', "STRING",  0,
      "Use an alternative configuration STRING"},
    {"default-setup",       'd', 0,         0,
      "Use default setup (WARNING: existing configuration will be overwriten)"},

    {0, 0, 0, 0, "Script mode:", 11},
    {"script",              't', "STRING",  0,
      "Execute a script STRING. "},

    {0, 0, 0, 0, "Automatic processing (GUI mode):", 12},
    {"force-automatic",     'm', 0,         0,
      "Force executing of automatic processing"},
    {"no-automatic",        'n', 0,         0,
      "Do not run automatic processing even if a session is eligible for it"},

    {0, 0, 0, 0, "Input control:", 13},
    {"catalog",             'c', 0,         0,
      "Force run in the catalog aware mode (opposite to '-s')"},
    {"format",              'f', "STRING",  0,
      "Set the data storage format of the provided session to STRING (either \"dbh\" or \"vgos\")"},
    {"read-all",            'l', 0,         0,
      "Read all databases, even that that lack of essential information"},
    {"standalone",          's', 0,         0,
      "Force run in the standalone mode (opposite to '-c')"},

    {0, 0, 0, 0, "Invocation of startup wizard:", 14},
    {"sys-wide-wizard",     'W', 0,         0,
      "Run startup wizard for the system-wide settings"},
    {"wizard",              'w', 0,         0,
      "Force call of the startup wizard"},

    {0, 0, 0, 0, "Execution control:", 15},
    {"no-interruptions",    'i', 0,         0,
      "Do not catch interruptions"},

    {0, 0, 0, 0, "Operation modes:", -1},
    {"version",             'V', 0,         0,
      "Print program version"},
    //
    {0}
  };

  QString                       salute("A program to make preliminary analysis of a geodetic "
    "VLBI session.\v");

  salute += "The current version is:\n\t" + nuSolveVersion.name() + " released on " + 
            nuSolveVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " + 
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe program nuSolve is a part of nuSolve package. For datails see "
    "\"nuSolve User Guide\", a part of nuSolve distribution. You can get the latest version of "
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");

  struct argp                   argp={argp_options, parse_opt, 
    "\n\nDATABASE\nSCRIPT [SCRIPT OPTIONS]", salute.toLatin1()};

  argp_parse (&argp, argc, argv, 0, 0, &options);
  //
  //
  //
  //
  if (options.isNeedSignalHandler && !options.scriptFileName.size()) // do not catch signals in a script mode
  {
    act.sa_handler = handlerSIGs;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGFPE, &act, &saveSIGFPE);
    sigaction(SIGILL, &act, &saveSIGILL);
    sigaction(SIGSEGV,&act, &saveSIGSEGV);
    sigaction(SIGABRT,&act, &saveSIGABRT);
    sigaction(SIGBUS, &act, &saveSIGBUS);
    sigaction(SIGINT, &act, &saveSIGINT);
    sigaction(SIGHUP, &act, &saveSIGHUP);
    sigaction(SIGTERM,&act, &saveSIGTERM);
  };
  //
  //
  isFirstRun = options.settings->allKeys().size()>0 ? false : true;
  //
  // load user saved values:
  if (!options.useDefaultSetup)
  {
    // first, use "standard" set up:
    checkSettings(*options.settings);
    if (options.have2UseAltSetup)
    {
      logger->write(SgLogger::INF, SgLogger::IO, nuSolveVersion.name() +
        ": using alternative config name \"" + options.altSetupName + "\"");
      QSettings                *altSettings = new QSettings(origOrgName, options.altSetupAppName);
      loadSettings(*altSettings);
      delete options.settings;
      options.settings = altSettings;
      config.setName(options.altSetupName);
    }
    else
      config.setName("");
  };
  //
  // check for the special needs:
  // "User bad. Replace and press Enter":
  if (options.isForcedCatalogMode && options.isForcedStandaloneMode)
  {
    options.isForcedCatalogMode = options.isForcedStandaloneMode = false;
    logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
      ": combination of -c and -s options has no effect");
  };
  if (options.have2ForceAutomaticProcessing && options.have2SkipAutomaticProcessing)
  {
    options.have2ForceAutomaticProcessing = options.have2SkipAutomaticProcessing = false;
    logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
      ": combination of -m and -n options has no effect");
  };
  //
  //
  if (options.isForcedCatalogMode && setup.getHave2UpdateCatalog())
    options.isForcedCatalogMode = false;
  else if (options.isForcedCatalogMode)
  {
    setup.setHave2UpdateCatalog(true);
    logger->write(SgLogger::DBG, SgLogger::IO, nuSolveVersion.name() +
      ": forced run in the catalog aware mode");
  };
  if (options.isForcedStandaloneMode && !setup.getHave2UpdateCatalog())
    options.isForcedStandaloneMode = false;
  else if (options.isForcedStandaloneMode)
  {
    setup.setHave2UpdateCatalog(false);
    logger->write(SgLogger::DBG, SgLogger::IO, nuSolveVersion.name() +
      ": forced run in the standalone mode");
  };
  if (options.have2SkipAutomaticProcessing)
  {
    setup.setHave2SkipAutomaticProcessing(options.have2SkipAutomaticProcessing);
    logger->write(SgLogger::DBG, SgLogger::IO, nuSolveVersion.name() +
      ": the automatic process mode is supressed");
  };
  if (options.have2ForceAutomaticProcessing)
  {
    setup.setHave2ForceAutomaticProcessing(options.have2ForceAutomaticProcessing);
    logger->write(SgLogger::DBG, SgLogger::IO, nuSolveVersion.name() +
      ": forced call the automatic processing");
  };
  //
  if (options.have2LoadImmatureSession)
    setup.setHave2LoadImmatureSession(options.have2LoadImmatureSession);
  //
  //
  //
  // adjust the logger:
  if (options.settings->contains("Version/LibraryName"))
  {
    logger->setLogFacility(SgLogger::ERR, 
      options.settings->value("Logger/FacilityERR", logger->getLogFacility(SgLogger::ERR)).toUInt());
    logger->setLogFacility(SgLogger::WRN, 
      options.settings->value("Logger/FacilityWRN", logger->getLogFacility(SgLogger::WRN)).toUInt());
    logger->setLogFacility(SgLogger::INF, 
      options.settings->value("Logger/FacilityINF", logger->getLogFacility(SgLogger::INF)).toUInt());
    logger->setLogFacility(SgLogger::DBG, 
      options.settings->value("Logger/FacilityDBG", logger->getLogFacility(SgLogger::DBG)).toUInt());
  };
  logger->setFileName(options.settings->value("Logger/FileName", "nuSolve.log").toString());
  logger->setDirName(setup.getPath2Home());
  logger->setCapacity(options.settings->value("Logger/Capacity", logger->getCapacity()).toInt());
  logger->setIsStoreInFile(
    options.settings->value("Logger/IsStoreInFile", logger->getIsStoreInFile()).toBool());
  logger->setIsNeedTimeMark(
    options.settings->value("Logger/IsNeedTimeMark", logger->getIsNeedTimeMark()).toBool());
  logger->setUseFullDateFormat(
    options.settings->value("Logger/UseFullDate", logger->getUseFullDateFormat()).toBool());
  // 
  //
  //
  // exec the application:
  //
  rc = 0;
  if (options.scriptFileName.size())
  {
    bool                        hasDisplay=false;
    const char*                 envDisplay=NULL;
    if ((envDisplay=getenv("DISPLAY")) && strlen(envDisplay))
      hasDisplay = true;

    if (hasDisplay)
    {
      QApplication              app(argc, argv);
      rc = executeScript(options.scriptFileName, options.args, hasDisplay);
    }
    else
    {
      QCoreApplication          app(argc, argv);
      rc = executeScript(options.scriptFileName, options.args, hasDisplay);
    };

    logger->clearSpool();
    logger->setIsStoreInFile(false);
    // and exit:
    delete options.settings;
    return rc;
  }
  else if (options.databaseName.size() && !options.have2ForceWizard)
  {
    if (isFirstRun)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
        ": cannot proccess the database \"" + options.databaseName + "\": no config found");
      return 1;
    };
    //
    //
    // check the input argument:
    //
    QString                     sessionName("");
    if (options.ofFmt==SgVlbiSessionInfo::OT_DBH)
    {
      // standalone mode, check for existing file:
      if ((options.isForcedStandaloneMode || !setup.getHave2UpdateCatalog()) && !QFile::exists(options.databaseName))
      {
        QString                 sTmp(setup.path2(setup.getPath2DbhFiles()) + "/" + options.databaseName);
        if (QFile::exists(sTmp))
          options.databaseName = sTmp;
        else
        {
          logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
            ": cannot proccess the database \"" + options.databaseName + "\": file does not exist");
          return 1;
        };
      };
      //
      // extract the session name:
      QRegExp                   reDbName(".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}).*");
      QRegExp                   reDbVerName(".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2})_V([0-9]{3}).*");
      QString                   sVer("");
      if (-1 < reDbVerName.indexIn(options.databaseName))
      {
        sessionName = reDbVerName.cap(1);
        sVer = reDbVerName.cap(2);
      }
      else if (-1 < reDbName.indexIn(options.databaseName))
        sessionName = reDbName.cap(1);
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
          ": cannot proccess the database \"" + options.databaseName + "\": does not look like a "
          "database name");
        return 1;
      };
    }
    else if (options.ofFmt==SgVlbiSessionInfo::OT_VDB)
    {
      // determine a name of a wrapper file:
      QString                   path2wrapperFile("");
      QString                   wrapperFileName("");
      int                       version(0);
      QRegExp                   reDatabaseName("^[0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}$");
      QRegExp                   reDatabaseVersionName("^[0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}_V[0-9]{3}$");
      bool                      isOk;
      // ok, this is a wrapper file name:
      if (options.databaseName.right(4) == ".wrp")
      {
        isOk = SgVlbiSession::guessSessionByWrapperFileName(options.databaseName,
          setup.path2(setup.getPath2VgosDbFiles()), 
          path2wrapperFile, wrapperFileName, sessionName, version);
      }
      else if (reDatabaseName.exactMatch(options.databaseName) || reDatabaseVersionName.exactMatch(options.databaseName))
      {
        isOk = SgVlbiSession::guessWrapperFileNameBySession(options.databaseName,
          setup.path2(setup.getPath2VgosDbFiles()), 
          setup.identities().getAcAbbName(), setup.getUseOwnOnlyDatabase(),
          path2wrapperFile, wrapperFileName, sessionName, version);
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
          ": cannot proccess the database \"" + options.databaseName + "\": file does not exist");
        return 1;
      };
      if (isOk)
        options.databaseName = path2wrapperFile + "/" + wrapperFileName;
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
          ": cannot proccess the database \"" + options.databaseName + "\": file does not exist");
        return 1;
      };
    };
    //
    //
    SgMJD                         startEpoch(SgMJD::currentMJD());
    QApplication                  app(argc, argv, false);
    SgLogger                     *alDriver;
    //
/*
    // adjust the logger:
    if (settings->contains("Version/LibraryName"))
    {
      logger->setLogFacility(SgLogger::ERR, 
        settings->value("Logger/FacilityERR", logger->getLogFacility(SgLogger::ERR)).toUInt());
      logger->setLogFacility(SgLogger::WRN, 
        settings->value("Logger/FacilityWRN", logger->getLogFacility(SgLogger::WRN)).toUInt());
      logger->setLogFacility(SgLogger::INF, 
        settings->value("Logger/FacilityINF", logger->getLogFacility(SgLogger::INF)).toUInt());
      logger->setLogFacility(SgLogger::DBG, 
        settings->value("Logger/FacilityDBG", logger->getLogFacility(SgLogger::DBG)).toUInt());
    };
    logger->setFileName(settings->value("Logger/FileName", "nuSolve.log").toString());
    logger->setDirName(setup.getPath2Home());
    logger->setCapacity(settings->value("Logger/Capacity", logger->getCapacity()).toInt());
    logger->setIsStoreInFile(
      settings->value("Logger/IsStoreInFile", logger->getIsStoreInFile()).toBool());
    logger->setIsNeedTimeMark(
      settings->value("Logger/IsNeedTimeMark", logger->getIsNeedTimeMark()).toBool());
    // 
*/
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
    alDriver->setFileName(sessionName + ".log");
    alDriver->rmLogFile();

    logger->write(SgLogger::DBG, SgLogger::PREPROC, nuSolveVersion.name() + 
      ": starting command line version");

    logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
      ": Library ID: " + libraryVersion.name());
    logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
      ": Driver  ID: " + nuSolveVersion.name());

    logger->write(SgLogger::INF, SgLogger::SESSION, nuSolveVersion.name() +
      ": Host    ID: " + setup.identities().getMachineNodeName() + 
      " (Hw: " + setup.identities().getMachineMachineName() + 
      "; Sw: " + setup.identities().getMachineRelease() + 
      " version of " + setup.identities().getMachineSysName() + ")");

    logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
      ": User    ID: " + setup.identities().getUserName() + 
      " <" + setup.identities().getUserEmailAddress() + ">, " + setup.identities().getAcFullName());
    logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
      ": User command: \"" + command + "\"");
    logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
      ": Input data: " + options.databaseName);
  
    rc = processSession(options.databaseName, options.ofFmt);

    if (rc==0)
    {
      // report elapsed time:
      SgMJD                       finisEpoch=SgMJD::currentMJD();
      logger->write(SgLogger::INF, SgLogger::PREPROC, nuSolveVersion.name() +
        ": the elapsed time to process the session " + sessionName + " is: " + 
        interval2Str(finisEpoch - startEpoch) + " (" + 
        QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400.0) + " sec)");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::PREPROC, nuSolveVersion.name() +
        ": processing of the database " + options.databaseName + " has failed");
      
    alDriver->clearSpool();
    logger->detachSupplementLog("Driver");
    delete alDriver;
    //
    logger->clearSpool();
    logger->setIsStoreInFile(false);
    //
    //
  }
  else //  ===================  GUI part starts here:  ===================
  {
    //
    //
    QApplication                  app(argc, argv);
    QIcon                         appIcon(QPixmap(":/images/icon_2.5.png"));
    app.setWindowIcon(appIcon);
    //
    if (options.settings->contains("Setup/GuiStyle"))
    {
      QString                     appStyle(options.settings->value("Setup/GuiStyle").toString());
      if (!appStyle.contains("GTK")) // it crashes with "GTK+" style
      {
        QApplication::setStyle(appStyle);
        QFont                     font("Sans Serif", 10);
        if (options.settings->contains("Setup/GuiFont"))
          font = options.settings->value("Setup/GuiFont").value<QFont>();
        QApplication::setFont(font);
      };
    };
    //
    //--
    if (options.have2ForceWizard || options.shouldInvokeSystemWideWizard ||
        options.settings->value("Version/StartUpWizard", 0).toInt() < NsStartupWizard::serialNumber())
    {
      if (options.shouldInvokeSystemWideWizard)
      {
        if (!options.have2UseAltSetup)
        {
          QSettings            *swSettings = 
                                  new QSettings(QSettings::SystemScope, origOrgName, origAppName);
          if (!swSettings->isWritable())
          {
            logger->write(SgLogger::ERR, SgLogger::IO, nuSolveVersion.name() +
              ": cannot write system wide config");
            delete swSettings;
            return 1;
          };
          delete options.settings;
          options.settings = swSettings;
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO, nuSolveVersion.name() +
            ": using an alternative config name, system wide config edit request ignored");
      };
      //
      NsStartupWizard           startup(isFirstRun, options.have2ForceWizard, options.shouldInvokeSystemWideWizard);
      if ((rc=startup.exec())==0)
      {
        delete options.settings;
        logger->clearSpool();
        return rc;
      };
      //
      // save if user pressed "Finish":
      options.settings->setValue("Version/StartUpWizard", startup.serialNumber());
      saveSettings(*options.settings, options.shouldInvokeSystemWideWizard);
    };  
    //
    mainWindow = new NsMainWindow;
    mainWindow->show();
    rc = app.exec();
    delete mainWindow;
  };
  //
  //
  //
  //
  // roll back to previous values:
  if (options.isForcedCatalogMode)
    setup.setHave2UpdateCatalog(false);
  if (options.isForcedStandaloneMode)
    setup.setHave2UpdateCatalog(true);

  // save user values:
  saveSettings(*options.settings, options.shouldInvokeSystemWideWizard);

  delete options.settings;
  return rc;
};



//
void checkSettings(QSettings& settings)
{
  if (settings.contains("Version/LibraryName"))
  {
    QString                     savedSoftName;
    QString                     savedCodeName;
    int                         savedMajorNum, savedMinorNum, savedTeenyNum, savedSettingsVersion;
    
    savedSoftName = settings.value("Version/LibraryName", "N/A").toString();
    savedCodeName = settings.value("Version/LibraryComments", "N/A").toString();
    savedMajorNum = settings.value("Version/LibraryMajor", 0).toInt();
    savedMinorNum = settings.value("Version/LibraryMinor", 0).toInt();
    savedTeenyNum = settings.value("Version/LibraryTeeny", 0).toInt();
    storedLibraryVersion.setSoftwareName(savedSoftName);
    storedLibraryVersion.setMajorNumber(savedMajorNum);
    storedLibraryVersion.setMinorNumber(savedMinorNum);
    storedLibraryVersion.setTeenyNumber(savedTeenyNum);
    storedLibraryVersion.setCodeName(savedCodeName);

    savedSoftName = settings.value("Version/nuSolveName", "N/A").toString();
    savedCodeName = settings.value("Version/nuSolveComments", "N/A").toString();
    savedMajorNum = settings.value("Version/nuSolveMajor", 0).toInt();
    savedMinorNum = settings.value("Version/nuSolveMinor", 0).toInt();
    savedTeenyNum = settings.value("Version/nuSolveTeeny", 0).toInt();
    storedNuSolveVersion.setSoftwareName(savedSoftName);
    storedNuSolveVersion.setMajorNumber(savedMajorNum);
    storedNuSolveVersion.setMinorNumber(savedMinorNum);
    storedNuSolveVersion.setTeenyNumber(savedTeenyNum);
    storedNuSolveVersion.setCodeName(savedCodeName);
    
    if (storedLibraryVersion>libraryVersion)
      logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::RUN, nuSolveVersion.name() +
        ": you are using an obsolete version, " + libraryVersion.name() + ", of the Library "
        "than it was before: " + storedLibraryVersion.name());
    if (storedNuSolveVersion>nuSolveVersion)
      logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::RUN, nuSolveVersion.name() +
        ": you are using an obsolete version, " + nuSolveVersion.name() + ", of the software "
        "than it was before: " + storedNuSolveVersion.name());
    
    savedSettingsVersion = settings.value("Version/nuSolveSettings", 0).toInt();
   
    loadSettings(settings);
    if (savedSettingsVersion < currentSettingsVersion) // do something:
    {
      // correct PWL model:
      if (savedSettingsVersion == 0)
      {
        int                     iMode;
        iMode = settings.value("Config/EstimatorPwlMode", config.getPwlMode()).toInt();
        switch (iMode)
        {
          case 0:
            config.setPwlMode(SgTaskConfig::EPM_BSPLINE_LINEA);
            break;
          case 1:
            config.setPwlMode(SgTaskConfig::EPM_INCRATE);
            break;
          default:
            logger->write(SgLogger::WRN, SgLogger::RUN, nuSolveVersion.name() +
              ": unrecognized PWL mode: " + QString("").setNum(iMode));
        };
        logger->write(SgLogger::DBG, SgLogger::RUN, nuSolveVersion.name() +
          ": the PWL mode has been adjusted");
      };
      // something else:
    
    };

  }
  else
  {
    loadSettingsOldStyle(settings);
    rmOldStyleSettings(settings);
  };
};



//
void loadSettings(QSettings& settings)
{
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
  setup.setPath2CatNuInterfaceExec(
    settings.value("Setup/Path2CatalogInterfaceExec", 
    setup.getPath2CatNuInterfaceExec()).toString());
  setup.setPath2DbhFiles(
    settings.value("Setup/Path2DbhFiles",
    setup.getPath2DbhFiles()).toString());
  setup.setPath2VgosDbFiles(
    settings.value("Setup/Path2OpenDbFiles",
    setup.getPath2VgosDbFiles()).toString());
  setup.setPath2VgosDaFiles(
    settings.value("Setup/Path2VgosDaFiles",
    setup.getPath2VgosDaFiles()).toString());
  setup.setPath2APrioriFiles(
    settings.value("Setup/Path2APrioriFiles",
    setup.getPath2APrioriFiles()).toString());
  setup.setPath2TrpFiles(
    settings.value("Setup/Path2TrpFiles",
    setup.getPath2TrpFiles()).toString());
  setup.setPath2MasterFiles(
    settings.value("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles()).toString());
  setup.setPath2SpoolFileOutput(
    settings.value("Setup/Path2SpoolfileOutput",
    setup.getPath2SpoolFileOutput()).toString());
  setup.setPath2NotUsedObsFileOutput(
    settings.value("Setup/Path2NotUsedObsFileOutput",
    setup.getPath2NotUsedObsFileOutput()).toString());
  setup.setPath2ReportOutput(
    settings.value("Setup/Path2ReportsOutput",
    setup.getPath2ReportOutput()).toString());
  setup.setPath2NgsOutput(
    settings.value("Setup/Path2NgsFilesOutput",
    setup.getPath2NgsOutput()).toString());
  setup.setPath2PlotterOutput(
    settings.value("Setup/Path2PlotterFilesOutput",
    setup.getPath2PlotterOutput()).toString());
  setup.setPath2IntermediateResults(
    settings.value("Setup/Path2IntermediateResults",
    setup.getPath2IntermediateResults()).toString());
  setup.setAutoSavingMode(NsSetup::AutoSaving(
    settings.value("Setup/AutoSavingMode",
    setup.getAutoSavingMode()).toInt()));
  setup.setHave2UpdateCatalog(
    settings.value("Setup/Have2UpdateCatalog", 
    setup.getHave2UpdateCatalog()).toBool());
  setup.setHave2MaskSessionCode(
    settings.value("Setup/Have2AlterSessionCode", 
    setup.getHave2MaskSessionCode()).toBool());
  setup.setHave2KeepSpoolFileReports(
    settings.value("Setup/Have2KeepSpoolfileReports", 
    setup.getHave2KeepSpoolFileReports()).toBool());
  setup.setHave2KeepUnusedObsList(
    settings.value("Setup/Have2KeepUnusedObsList", 
    setup.getHave2KeepUnusedObsList()).toBool());
  setup.setHave2WarnCloseWindow(
    settings.value("Setup/Have2WarnClosingWindow",
    setup.getHave2WarnCloseWindow()).toBool());
  setup.setIsShortScreen(
    settings.value("Setup/IsShortScreen",
    setup.getIsShortScreen()).toBool());
  setup.setMainWinWidth(
    settings.value("Setup/MainWinWidth",
    setup.getMainWinWidth()).toInt());
  setup.setMainWinHeight(
    settings.value("Setup/MainWinHeight",
    setup.getMainWinHeight()).toInt());
  setup.setMainWinPosX(
    settings.value("Setup/MainWinPosX",
    setup.getMainWinPosX()).toInt());
  setup.setMainWinPosY(
    settings.value("Setup/MainWinPosY",
    setup.getMainWinPosY()).toInt());

  setup.setSeWinWidth(
    settings.value("Setup/SeWinWidth",
    setup.getSeWinWidth()).toInt());
  setup.setSeWinHeight(
    settings.value("Setup/SeWinHeight",
    setup.getSeWinHeight()).toInt());
  setup.setHave2AutoloadAllBands(
    settings.value("Setup/Have2AutoloadAllBands",
    setup.getHave2AutoloadAllBands()).toBool());
  setup.setIsBandPlotPerSrcView(
    settings.value("Setup/IsBandPlotPerSrcView",
    setup.getIsBandPlotPerSrcView()).toBool());
  setup.setUseOwnOnlyDatabase(
    settings.value("Setup/UseOwnOnlyDatabase",
    setup.getUseOwnOnlyDatabase()).toBool());
  setup.setPlotterOutputFormat(SgPlot::OutputFormat(
    settings.value("Setup/PlotterOutputFormat",
    setup.getPlotterOutputFormat()).toInt()));
  setup.setLnfsFileName(
    settings.value("Setup/LnfsFileName",
    setup.getLnfsFileName()).toString());
  setup.setLnfsOriginType(SgVlbiSessionInfo::OriginType(
    settings.value("Setup/LnfsOriginType",
    setup.getLnfsOriginType()).toInt()));
  setup.setLnfsIsThroughCatalog(
    settings.value("Setup/LnfsIsThroughCatalog",
    setup.getLnfsIsThroughCatalog()).toBool());
  setup.setPath2AuxLogs(
    settings.value("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs()).toString());
  setup.setHave2SavePerSessionLog(
    settings.value("Setup/Have2SavePerSessionLog",
    setup.getHave2SavePerSessionLog()).toBool());
  setup.setExecExternalCommand(
    settings.value("Setup/ExecExternalCommand",
    setup.getExecExternalCommand()).toBool());
  setup.setExternalCommand(
    settings.value("Setup/ExternalCommand",
    setup.getExternalCommand()).toString());


  // config:
  config.setQualityCodeThreshold(
    settings.value("Config/QualityCodeThreshold",
    config.getQualityCodeThreshold()).toInt());
  config.setGoodQualityCodeAtStartup(
    settings.value("Config/GoodQualityCodeAtStartup",
    config.getGoodQualityCodeAtStartup()).toInt());
  config.setUseGoodQualityCodeAtStartup(
    settings.value("Config/UseGoodQualityCodeAtStartup",
    config.getUseGoodQualityCodeAtStartup()).toBool());
  config.setDoWeightCorrection(
    settings.value("Config/DoWeightCorrection",
    config.getDoWeightCorrection()).toBool());
  config.setUseQualityCodeG(
    settings.value("Config/UseQualityCodeG",
    config.getUseQualityCodeG()).toBool());
  config.setUseQualityCodeH(
    settings.value("Config/UseQualityCodeH",
    config.getUseQualityCodeH()).toBool());
  config.setIsSolveCompatible(
    settings.value("Config/IsSolveCompatible",
    config.getIsSolveCompatible()).toBool());
  config.setUseDynamicClockBreaks(
    settings.value("Config/UseDynamicClockBreaks",
    config.getUseDynamicClockBreaks()).toBool());
  config.setUseSolveObsSuppresionFlags(
    settings.value("Config/UseSolveObsSuppresionFlags",
    config.getUseSolveObsSuppresionFlags()).toBool());
  config.setUseExternalWeights(
    settings.value("Config/UseExternalWeights",
    config.getUseExternalWeights()).toBool());
  config.setWcMode(SgTaskConfig::WeightCorrectionMode(
    settings.value("Config/WeightCorrectionMode",
    config.getWcMode()).toInt()));
  config.setExtWeightsFileName(
    settings.value("Config/ExternalWeightsFileName",
    config.getExtWeightsFileName()).toString());
  config.setInitAuxSigma4Delay(
    settings.value("Config/InitAuxSigma4Delay",
    config.getInitAuxSigma4Delay()).toDouble());
  config.setInitAuxSigma4Rate(
    settings.value("Config/InitAuxSigma4Rate",
    config.getInitAuxSigma4Rate()).toDouble());
  config.setMinAuxSigma4Delay(
    settings.value("Config/MinAuxSigma4Delay",
    config.getMinAuxSigma4Delay()).toDouble());
  config.setMinAuxSigma4Rate(
    settings.value("Config/MinAuxSigma4Rate",
    config.getMinAuxSigma4Rate()).toDouble());
  config.setPwlMode(SgTaskConfig::EstimatorPwlMode(
    settings.value("Config/EstimatorPwlMode",
    config.getPwlMode()).toInt()));
  config.setOpMode(SgTaskConfig::OutliersProcessingMode(
    settings.value("Config/OutlierProcessingMode",
    config.getOpMode()).toInt()));
  config.setOpAction(SgTaskConfig::OutliersProcessingAction(
    settings.value("Config/OutlierProcessingAction",
    config.getOpAction()).toInt()));
  config.setOpThreshold(
    settings.value("Config/OutlierProcessingThreshold",
    config.getOpThreshold()).toDouble());
  config.setOpIterationsLimit(
    settings.value("Config/OutlierProcessingIterationsLimit",
    config.getOpIterationsLimit()).toInt());
  config.setOpHave2SuppressWeightCorrection(
    settings.value("Config/OutlierProcessingSuppressWC",
    config.getOpHave2SuppressWeightCorrection()).toBool());
  config.setOpIsSolveCompatible(
    settings.value("Config/OutlierProcessingIsSolveCompatible",
    config.getOpIsSolveCompatible()).toBool());
  config.setOpHave2NormalizeResiduals(
    settings.value("Config/OutlierProcessingHave2NormalizeResiduals",
    config.getOpHave2NormalizeResiduals()).toBool());
  config.setDoIonoCorrection4SBD(
    settings.value("Config/DoIono4Sbd",
    config.getDoIonoCorrection4SBD()).toBool());
  config.setDoClockBreakDetectionMode1(
    settings.value("Config/DoClockBreakDetectionMode_1",
    config.getDoClockBreakDetectionMode1()).toBool());
  config.setDoOutliersEliminationMode1(
    settings.value("Config/DoOutlierEliminationMode_1",
    config.getDoOutliersEliminationMode1()).toBool());
  config.setIsActiveBandFollowsTab(
    settings.value("Config/IsActiveBandFollowsTab",
    config.getIsActiveBandFollowsTab()).toBool());
  config.setIsObservableFollowsPlot(
    settings.value("Config/IsObservableFollowsPlot",
    config.getIsObservableFollowsPlot()).toBool());
  config.setUseExtAPrioriSitesPositions(
    settings.value("Config/UseExternalAPrioriSitePostions",
    config.getUseExtAPrioriSitesPositions()).toBool());
  config.setUseExtAPrioriSitesVelocities(
    settings.value("Config/UseExternalAPrioriSiteVelocities",
    config.getUseExtAPrioriSitesVelocities()).toBool());
  config.setUseExtAPrioriSourcesPositions(
    settings.value("Config/UseExternalAPrioriSourcePostions",
    config.getUseExtAPrioriSourcesPositions()).toBool());
  config.setUseExtAPrioriSourceSsm(
    settings.value("Config/UseExternalAPrioriSourceSsm",
    config.getUseExtAPrioriSourceSsm()).toBool());
  config.setUseExtAPrioriErp(
    settings.value("Config/UseExternalAPrioriEop",
    config.getUseExtAPrioriErp()).toBool());
  config.setUseExtAPrioriAxisOffsets(
    settings.value("Config/UseExternalAPrioriAxisOffsets",
    config.getUseExtAPrioriAxisOffsets()).toBool());
  config.setUseExtAPrioriHiFyErp(
    settings.value("Config/UseExternalAPrioriHighFrequencyErp",
    config.getUseExtAPrioriHiFyErp()).toBool());
  config.setUseExtAPrioriMeanGradients(
    settings.value("Config/UseExternalAPrioriMeanGradients",
    config.getUseExtAPrioriMeanGradients()).toBool());
  config.setExtAPrioriSitesPositionsFileName(
    settings.value("Config/ExternalAPrioriSitePostionsFileName",
    config.getExtAPrioriSitesPositionsFileName()).toString());
  config.setExtAPrioriSitesVelocitiesFileName(
    settings.value("Config/ExternalAPrioriSiteVelocitiesFileName",
    config.getExtAPrioriSitesVelocitiesFileName()).toString());
  config.setExtAPrioriSourcesPositionsFileName(
    settings.value("Config/ExternalAPrioriSourcePostionsFileName",
    config.getExtAPrioriSourcesPositionsFileName()).toString());
  config.setExtAPrioriSourceSsmFileName(
    settings.value("Config/ExternalAPrioriSourceSsmFileName",
    config.getExtAPrioriSourceSsmFileName()).toString());
  config.setExtAPrioriErpFileName(
    settings.value("Config/ExternalAPrioriEopFileName",
    config.getExtAPrioriErpFileName()).toString());
  config.setExtAPrioriAxisOffsetsFileName(
    settings.value("Config/ExternalAPrioriAxisOffsetFileName",
    config.getExtAPrioriAxisOffsetsFileName()).toString());
  config.setExtAPrioriHiFyErpFileName(
    settings.value("Config/ExternalAPrioriHighFrequencyErpFileName",
    config.getExtAPrioriHiFyErpFileName()).toString());
  config.setExtAPrioriMeanGradientsFileName(
    settings.value("Config/ExternalAPrioriMeanGradientsFileName",
    config.getExtAPrioriMeanGradientsFileName()).toString());
  config.setEccentricitiesFileName(
    settings.value("Config/EccentricitiesFileName",
    config.getEccentricitiesFileName()).toString());
  config.setHave2ApplySourceSsm(
    settings.value("Config/Have2ApplySourceSsm",
    config.getHave2ApplySourceSsm()).toBool());

  config.setRefractionModel(SgTaskConfig::RefractionModel(
    settings.value("Config/RefractionModel",
    config.getRefractionModel()).toInt()));

  config.setHave2ApplyOceanTideContrib(
    settings.value("Config/Have2ApplyOceanTidesContribution",
    config.getHave2ApplyOceanTideContrib()).toBool());
  config.setHave2ApplyPoleTideContrib(
    settings.value("Config/Have2ApplyPoleTideContribution",
    config.getHave2ApplyPoleTideContrib()).toBool());
  config.setHave2ApplyEarthTideContrib(
    settings.value("Config/Have2ApplyEarthTideContribution",
    config.getHave2ApplyEarthTideContrib()).toBool());
  config.setHave2ApplyPxContrib(
    settings.value("Config/Have2ApplyWobblePxContribution",
    config.getHave2ApplyPxContrib()).toBool());
  config.setHave2ApplyPyContrib(
    settings.value("Config/Have2ApplyWobblePyContribution",
    config.getHave2ApplyPyContrib()).toBool());
  config.setHave2ApplyAxisOffsetContrib(
    settings.value("Config/Have2ApplyAxisOffsetContribution",
    config.getHave2ApplyAxisOffsetContrib()).toBool());
  config.setHave2ApplyNutationHFContrib(
    settings.value("Config/Have2ApplyNutationHighFrequencyContribution",
    config.getHave2ApplyNutationHFContrib()).toBool());
  config.setHave2ApplyPxyOceanTideHFContrib(
    settings.value("Config/Have2ApplyPolarMotionHighFrequencyContribution",
    config.getHave2ApplyPxyOceanTideHFContrib()).toBool());
  config.setHave2ApplyUt1OceanTideHFContrib(
    settings.value("Config/Have2ApplyUT1HighFrequencyContribution",
    config.getHave2ApplyUt1OceanTideHFContrib()).toBool());
  config.setHave2ApplyFeedCorrContrib(
    settings.value("Config/Have2ApplyFeedCorrectionContribution",
    config.getHave2ApplyFeedCorrContrib()).toBool());
  config.setHave2ApplyTiltRemvrContrib(
    settings.value("Config/Have2ApplyTiltRemvrContribution",
    config.getHave2ApplyTiltRemvrContrib()).toBool());
  config.setHave2ApplyUnPhaseCalContrib(
    settings.value("Config/Have2ApplyUnphaseCalibrationContribution",
    config.getHave2ApplyUnPhaseCalContrib()).toBool());
  config.setHave2ApplyOceanPoleTideContrib(
    settings.value("Config/Have2ApplyOceanPoleTideContribution",
    config.getHave2ApplyOceanPoleTideContrib()).toBool());
  config.setHave2ApplyGpsIonoContrib(
    settings.value("Config/Have2ApplyGpsIonoContrib",
    config.getHave2ApplyGpsIonoContrib()).toBool());
  config.setHave2ApplyPxyLibrationContrib(
    settings.value("Config/Have2ApplyPolarMotionLibrationContribution",
    config.getHave2ApplyPxyLibrationContrib()).toBool());
  config.setHave2ApplyUt1LibrationContrib(
    settings.value("Config/Have2ApplyUT1LibrationContribution",
    config.getHave2ApplyUt1LibrationContrib()).toBool());
  config.setHave2ApplyOldOceanTideContrib(
    settings.value("Config/Have2ApplyOldOceanTidesContribution",
    config.getHave2ApplyOldOceanTideContrib()).toBool());
  config.setHave2ApplyOldPoleTideContrib(
    settings.value("Config/Have2ApplyOldPoleTideContribution",
    config.getHave2ApplyOldPoleTideContrib()).toBool());
  config.setIsNoviceUser(
    settings.value("Config/IsNoviceUser",
    config.getIsNoviceUser()).toBool());
  config.setLastModifiedNetId(
    settings.value("Config/LastModifiedNetId",
    config.getLastModifiedNetId()).toString());
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
  //
  // 
  int                           size=settings.beginReadArray("Config/AutomaticProcessingByNetId");
  // remove default set up:
  if (size)
    config.apByNetId().clear();
  // load saved data:
  for (int i=0; i<size; ++i)
  {
    settings.setArrayIndex(i);
    SgTaskConfig::AutomaticProcessing 
                                ap;
    QString                     netId;
    netId = settings.value("networkId").toString();
    ap.doSessionSetup_        = settings.value("doSessionSetup",          ap.doSessionSetup_)
      .toBool();
    ap.doIonoCorrection4SBD_  = settings.value("doIonoCorrection4SBD",    ap.doIonoCorrection4SBD_)
      .toBool();
    ap.doAmbigResolution_     = settings.value("doAmbigResolution",       ap.doAmbigResolution_)
      .toBool();
    ap.doClockBreaksDetection_= settings.value("doClockBreaksDetection",  ap.doClockBreaksDetection_)
      .toBool();
    ap.doIonoCorrection4All_  = settings.value("doIonoCorrection4All",    ap.doIonoCorrection4All_)
      .toBool();
    ap.doOutliers_            = settings.value("doOutliers",              ap.doOutliers_)
      .toBool();
    ap.doWeights_             = settings.value("doWeights",               ap.doWeights_)
      .toBool();
    ap.doReportNotUsedData_   = settings.value("doReportNotUsedData",     ap.doReportNotUsedData_)
      .toBool();
    ap.finalSolution_         = SgTaskConfig::AutomaticProcessing::FinalSolution(
                                settings.value("FinalSolution",           ap.finalSolution_)
      .toInt());
    config.apByNetId()[netId] = ap;
  };
  settings.endArray();
  //
};



//
void loadSettingsOldStyle(QSettings& settings)
{
  // setup.identities:
  setup.identities().setUserName( 
    settings.value("Identities/User name", 
    setup.identities().getUserName()).toString());
  setup.identities().setUserEmailAddress( 
    settings.value("Identities/User email address", 
    setup.identities().getUserEmailAddress()).toString());
  setup.identities().setUserDefaultInitials( 
    settings.value("Identities/User default initials", 
    setup.identities().getUserDefaultInitials()).toString());
  setup.identities().setAcFullName( 
    settings.value("Identities/Analysis center full name", 
    setup.identities().getAcFullName()).toString());
  setup.identities().setAcAbbrevName( 
    settings.value("Identities/Analysis center abbreviated name", 
    setup.identities().getAcAbbrevName()).toString());
  setup.identities().setAcAbbName( 
    settings.value("Identities/Analysis center short abbreviated name", 
    setup.identities().getAcAbbName()).toString());

  // setup:
  // Pathes:
  setup.setPath2CatNuInterfaceExec(
    settings.value("Setup/Path to catalog interface exec", 
    setup.getPath2CatNuInterfaceExec()).toString());
  setup.setPath2DbhFiles(
    settings.value("Setup/Path to DBH files", 
    setup.getPath2DbhFiles()).toString());
  setup.setPath2APrioriFiles(
    settings.value("Setup/Path to a priori files", 
    setup.getPath2APrioriFiles()).toString());
  setup.setPath2MasterFiles(
    settings.value("Setup/Path to master files", 
    setup.getPath2MasterFiles()).toString());
  setup.setPath2SpoolFileOutput(
    settings.value("Setup/Path to spoolfile output", 
    setup.getPath2SpoolFileOutput()).toString());
  setup.setPath2ReportOutput(
    settings.value("Setup/Path to reports output", 
    setup.getPath2ReportOutput()).toString());
  setup.setPath2NgsOutput(
    settings.value("Setup/Path to NGS files output", 
    setup.getPath2NgsOutput()).toString());
  setup.setHave2UpdateCatalog(
    settings.value("Setup/Have to update catalog", 
    setup.getHave2UpdateCatalog()).toBool());
  setup.setHave2MaskSessionCode(
    settings.value("Setup/Have to alter session code", 
    setup.getHave2MaskSessionCode()).toBool());
  setup.setHave2KeepSpoolFileReports(
    settings.value("Setup/Have to keep spoolfile reports", 
    setup.getHave2KeepSpoolFileReports()).toBool());
  // GUI:
  setup.setHave2WarnCloseWindow(
    settings.value("Setup/Have to warn closing window",
    setup.getHave2WarnCloseWindow()).toBool());

  // config:
  config.setDoWeightCorrection(
    settings.value("Config/Do weight correction",
    config.getDoWeightCorrection()).toBool());
  config.setIsSolveCompatible(
    settings.value("Config/Is SOLVE compatible",
    config.getIsSolveCompatible()).toBool());
  config.setUseExternalWeights(
    settings.value("Config/Use external weights",
    config.getUseExternalWeights()).toBool());
  config.setWcMode(SgTaskConfig::WeightCorrectionMode(
    settings.value("Config/Weight correction mode",
    config.getWcMode()).toInt()));
  config.setExtWeightsFileName(
    settings.value("Config/External weights file name",
    config.getExtWeightsFileName()).toString());
  config.setPwlMode(SgTaskConfig::EstimatorPwlMode(
    settings.value("Config/EstimatorPwlMode",
    config.getPwlMode()).toInt()));
  config.setOpMode(SgTaskConfig::OutliersProcessingMode(
    settings.value("Config/Outlier processing mode",
    config.getOpMode()).toInt()));
  config.setOpAction(SgTaskConfig::OutliersProcessingAction(
    settings.value("Config/Outlier processing action",
    config.getOpAction()).toInt()));
  config.setOpThreshold(
    settings.value("Config/Outlier processing threshold",
    config.getOpThreshold()).toDouble());
  config.setOpIterationsLimit(
    settings.value("Config/Outlier processing iterations limit",
    config.getOpIterationsLimit()).toInt());
  config.setOpHave2SuppressWeightCorrection(
    settings.value("Config/Outlier processing WC suppresion",
    config.getOpHave2SuppressWeightCorrection()).toBool());
  config.setDoIonoCorrection4SBD(
    settings.value("Config/Do iono 4SBD",
    config.getDoIonoCorrection4SBD()).toBool());
  config.setDoClockBreakDetectionMode1(
    settings.value("Config/Do CB Mode 1",
    config.getDoClockBreakDetectionMode1()).toBool());
  config.setDoOutliersEliminationMode1(
    settings.value("Config/Do OP Mode 1",
    config.getDoOutliersEliminationMode1()).toBool());
  config.setIsActiveBandFollowsTab(
    settings.value("Config/Is active band follows tab",
    config.getIsActiveBandFollowsTab()).toBool());
  config.setIsObservableFollowsPlot(
    settings.value("Config/Is observable follows plotter",
    config.getIsObservableFollowsPlot()).toBool());
  config.setUseExtAPrioriSitesPositions(
    settings.value("Config/Use external a priori sites postions",
    config.getUseExtAPrioriSitesPositions()).toBool());
  config.setUseExtAPrioriSitesVelocities(
    settings.value("Config/Use external a priori sites velocities",
    config.getUseExtAPrioriSitesVelocities()).toBool());
  config.setUseExtAPrioriSourcesPositions(
    settings.value("Config/Use external a priori sources positions",
    config.getUseExtAPrioriSourcesPositions()).toBool());
  config.setUseExtAPrioriAxisOffsets(
    settings.value("Config/Use external a priori axis offsets",
    config.getUseExtAPrioriAxisOffsets()).toBool());
  config.setUseExtAPrioriHiFyErp(
    settings.value("Config/Use external a priori high frequency ERP",
    config.getUseExtAPrioriHiFyErp()).toBool());
  config.setUseExtAPrioriMeanGradients(
    settings.value("Config/Use external a priori mean gradients",
    config.getUseExtAPrioriMeanGradients()).toBool());
  config.setExtAPrioriSitesPositionsFileName(
    settings.value("Config/External a priori sites postions file name",
    config.getExtAPrioriSitesPositionsFileName()).toString());
  config.setExtAPrioriSitesVelocitiesFileName(
    settings.value("Config/External a priori sites velocities file name",
    config.getExtAPrioriSitesVelocitiesFileName()).toString());
  config.setExtAPrioriSourcesPositionsFileName(
    settings.value("Config/External a priori sources positions file name",
    config.getExtAPrioriSourcesPositionsFileName()).toString());
  config.setExtAPrioriAxisOffsetsFileName(
    settings.value("Config/External a priori axis offsets file name",
    config.getExtAPrioriAxisOffsetsFileName()).toString());
  config.setExtAPrioriHiFyErpFileName(
    settings.value("Config/External a priori high frequency ERP file name",
    config.getExtAPrioriHiFyErpFileName()).toString());
  config.setExtAPrioriMeanGradientsFileName(
    settings.value("Config/External a priori mean gradients file name",
    config.getExtAPrioriMeanGradientsFileName()).toString());
  config.setHave2ApplyOceanTideContrib(
    settings.value("Config/Have to apply ocean tides contribution",
    config.getHave2ApplyOceanTideContrib()).toBool());
  config.setHave2ApplyPoleTideContrib(
    settings.value("Config/Have to apply pole tide contribution",
    config.getHave2ApplyPoleTideContrib()).toBool());
  config.setHave2ApplyEarthTideContrib(
    settings.value("Config/Have to apply earth tide contribution",
    config.getHave2ApplyEarthTideContrib()).toBool());
  config.setHave2ApplyPxContrib(
    settings.value("Config/Have to apply wobble Px contribution",
    config.getHave2ApplyPxContrib()).toBool());
  config.setHave2ApplyPyContrib(
    settings.value("Config/Have to apply wobble Py contribution",
    config.getHave2ApplyPyContrib()).toBool());
  config.setHave2ApplyAxisOffsetContrib(
    settings.value("Config/Have to apply axis offset contribution",
    config.getHave2ApplyAxisOffsetContrib()).toBool());
  config.setHave2ApplyNutationHFContrib(
    settings.value("Config/Have to apply nutation high frequency contribution",
    config.getHave2ApplyNutationHFContrib()).toBool());
  config.setHave2ApplyPxyOceanTideHFContrib(
    settings.value("Config/Have to apply polar motion high frequency contribution",
    config.getHave2ApplyPxyOceanTideHFContrib()).toBool());
  config.setHave2ApplyUt1OceanTideHFContrib(
    settings.value("Config/Have to apply UT1 high frequency contribution",
    config.getHave2ApplyUt1OceanTideHFContrib()).toBool());
  config.setHave2ApplyFeedCorrContrib(
    settings.value("Config/Have to apply feed correction contribution",
    config.getHave2ApplyFeedCorrContrib()).toBool());
  config.setHave2ApplyTiltRemvrContrib(
    settings.value("Config/Have to apply tilt remvr contribution",
    config.getHave2ApplyTiltRemvrContrib()).toBool());
  config.setHave2ApplyUnPhaseCalContrib(
    settings.value("Config/Have to apply unphase calibration contribution",
    config.getHave2ApplyUnPhaseCalContrib()).toBool());
};



//
void rmOldStyleSettings(QSettings& settings)
{
  // setup.identities:
  settings.remove("Identities/User name"); 
  settings.remove("Identities/User email address"); 
  settings.remove("Identities/User default initials"); 
  settings.remove("Identities/Analysis center full name"); 
  settings.remove("Identities/Analysis center abbreviated name"); 
  settings.remove("Identities/Analysis center short abbreviated name"); 

  // setup:
  // Pathes:
  settings.remove("Setup/Path to catalog interface exec"); 
  settings.remove("Setup/Path to DBH files"); 
  settings.remove("Setup/Path to a priori files"); 
  settings.remove("Setup/Path to master files"); 
  settings.remove("Setup/Path to spoolfile output"); 
  settings.remove("Setup/Path to reports output"); 
  settings.remove("Setup/Path to NGS files output"); 
  settings.remove("Setup/Have to update catalog"); 
  settings.remove("Setup/Have to alter session code"); 
  settings.remove("Setup/Have to keep spoolfile reports"); 
  // GUI:
  settings.remove("Setup/Have to warn closing window");

  // config:
  settings.remove("Config/Do weight correction");
  settings.remove("Config/Is SOLVE compatible");
  settings.remove("Config/Use external weights");
  settings.remove("Config/Weight correction mode");
  settings.remove("Config/External weights file name");
  settings.remove("Config/Outlier processing mode");
  settings.remove("Config/Outlier processing action");
  settings.remove("Config/Outlier processing threshold");
  settings.remove("Config/Outlier processing iterations limit");
  settings.remove("Config/Outlier processing WC suppresion");
  settings.remove("Config/Do iono 4SBD");
  settings.remove("Config/Do CB Mode 1");
  settings.remove("Config/Do OP Mode 1");
  settings.remove("Config/Is active band follows tab");
  settings.remove("Config/Is observable follows plotter");
  settings.remove("Config/Use external a priori sites postions");
  settings.remove("Config/Use external a priori sites velocities");
  settings.remove("Config/Use external a priori sources positions");
  settings.remove("Config/Use external a priori axis offsets");
  settings.remove("Config/Use external a priori high frequency ERP");
  settings.remove("Config/Use external a priori mean gradients");
  settings.remove("Config/External a priori sites postions file name");
  settings.remove("Config/External a priori sites velocities file name");
  settings.remove("Config/External a priori sources positions file name");
  settings.remove("Config/External a priori axis offsets file name");
  settings.remove("Config/External a priori high frequency ERP file name");
  settings.remove("Config/External a priori mean gradients file name");
  settings.remove("Config/Have to apply ocean tides contribution");
  settings.remove("Config/Have to apply pole tide contribution");
  settings.remove("Config/Have to apply earth tide contribution");
  settings.remove("Config/Have to apply wobble Px contribution");
  settings.remove("Config/Have to apply wobble Py contribution");
  settings.remove("Config/Have to apply axis offset contribution");
  settings.remove("Config/Have to apply nutation high frequency contribution");
  settings.remove("Config/Have to apply polar motion high frequency contribution");
  settings.remove("Config/Have to apply UT1 high frequency contribution");
  settings.remove("Config/Have to apply feed correction contribution");
  settings.remove("Config/Have to apply tilt remvr contribution");
  settings.remove("Config/Have to apply Niel hydrostatic troposphere contribution");
  settings.remove("Config/Have to apply Niel wet troposphere contribution");
  settings.remove("Config/Have to apply unphase calibration contribution");

  //
  settings.remove("Config/FlybyTropZenithMap");
  settings.remove("Config/Have2ApplyNielHydrostaticTroposphereContribution");
  settings.remove("Config/Have2ApplyNielWetTroposphereContribution");
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
  settings.setValue("Setup/Path2CatalogInterfaceExec",
    setup.getPath2CatNuInterfaceExec());
  settings.setValue("Setup/Path2DbhFiles",
    setup.getPath2DbhFiles());
  settings.setValue("Setup/Path2OpenDbFiles",
    setup.getPath2VgosDbFiles());
  settings.setValue("Setup/Path2VgosDaFiles",
    setup.getPath2VgosDaFiles());
  settings.setValue("Setup/Path2APrioriFiles",
    setup.getPath2APrioriFiles());
  settings.setValue("Setup/Path2TrpFiles",
    setup.getPath2TrpFiles());
  settings.setValue("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles());
  settings.setValue("Setup/Path2SpoolfileOutput",
    setup.getPath2SpoolFileOutput());
  settings.setValue("Setup/Path2NotUsedObsFileOutput",
    setup.getPath2NotUsedObsFileOutput());
  settings.setValue("Setup/Path2ReportsOutput",
    setup.getPath2ReportOutput());
  settings.setValue("Setup/Path2NgsFilesOutput",
    setup.getPath2NgsOutput());
  settings.setValue("Setup/Path2PlotterFilesOutput",
    setup.getPath2PlotterOutput());
  settings.setValue("Setup/Path2IntermediateResults",
    setup.getPath2IntermediateResults());
  // masterfile extensions:
  settings.setValue("Setup/UseAltMasterFileExtensions",
    setup.getUseAltMasterfileSuffixes());
  settings.setValue("Setup/MasterFileExtensions", 
    setup.getMasterfileSuffixes().join(","));
  //
  //
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Setup/AutoSavingMode",
      setup.getAutoSavingMode());
    settings.setValue("Setup/Have2UpdateCatalog",
      setup.getHave2UpdateCatalog());
    settings.setValue("Setup/Have2AlterSessionCode",
      setup.getHave2MaskSessionCode());
    settings.setValue("Setup/Have2KeepSpoolfileReports",
      setup.getHave2KeepSpoolFileReports());
    settings.setValue("Setup/Have2KeepUnusedObsList",
      setup.getHave2KeepUnusedObsList());
    settings.setValue("Setup/Have2WarnClosingWindow",
      setup.getHave2WarnCloseWindow());
    settings.setValue("Setup/IsShortScreen",
      setup.getIsShortScreen());
    settings.setValue("Setup/MainWinWidth",
      setup.getMainWinWidth());
    settings.setValue("Setup/MainWinHeight",
      setup.getMainWinHeight());
    settings.setValue("Setup/MainWinPosX",
      setup.getMainWinPosX());
    settings.setValue("Setup/MainWinPosY",
      setup.getMainWinPosY());

    settings.setValue("Setup/SeWinWidth",
      setup.getSeWinWidth());
    settings.setValue("Setup/SeWinHeight",
      setup.getSeWinHeight());
    settings.setValue("Setup/Have2AutoloadAllBands",
      setup.getHave2AutoloadAllBands());
    settings.setValue("Setup/IsBandPlotPerSrcView",
      setup.getIsBandPlotPerSrcView());
    settings.setValue("Setup/UseOwnOnlyDatabase",
      setup.getUseOwnOnlyDatabase());
    settings.setValue("Setup/PlotterOutputFormat",
      setup.getPlotterOutputFormat());
    settings.setValue("Setup/LnfsFileName",
      setup.getLnfsFileName());
    settings.setValue("Setup/LnfsOriginType",
      setup.getLnfsOriginType());
    settings.setValue("Setup/LnfsIsThroughCatalog",
      setup.getLnfsIsThroughCatalog());
  };
  settings.setValue("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs());
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Setup/Have2SavePerSessionLog",
      setup.getHave2SavePerSessionLog());
    settings.setValue("Setup/ExecExternalCommand",
      setup.getExecExternalCommand());
    settings.setValue("Setup/ExternalCommand",
      setup.getExternalCommand());
  };

  // config:
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Config/QualityCodeThreshold",
      config.getQualityCodeThreshold());
    settings.setValue("Config/GoodQualityCodeAtStartup",
      config.getGoodQualityCodeAtStartup());
    settings.setValue("Config/UseGoodQualityCodeAtStartup",
      config.getUseGoodQualityCodeAtStartup());
    settings.setValue("Config/DoWeightCorrection",
      config.getDoWeightCorrection());
    settings.setValue("Config/UseQualityCodeG",
      config.getUseQualityCodeG());
    settings.setValue("Config/UseQualityCodeH",
      config.getUseQualityCodeH());
    settings.setValue("Config/IsSolveCompatible",
      config.getIsSolveCompatible());
    settings.setValue("Config/UseDynamicClockBreaks",
      config.getUseDynamicClockBreaks());
    settings.setValue("Config/UseSolveObsSuppresionFlags",
      config.getUseSolveObsSuppresionFlags());
    settings.setValue("Config/UseExternalWeights",
      config.getUseExternalWeights());
    settings.setValue("Config/WeightCorrectionMode",
      config.getWcMode());
    settings.setValue("Config/ExternalWeightsFileName",
      config.getExtWeightsFileName());
    settings.setValue("Config/InitAuxSigma4Delay",
      config.getInitAuxSigma4Delay());
    settings.setValue("Config/InitAuxSigma4Rate",
      config.getInitAuxSigma4Rate());
    settings.setValue("Config/MinAuxSigma4Delay",
      config.getMinAuxSigma4Delay());
    settings.setValue("Config/MinAuxSigma4Rate",
      config.getMinAuxSigma4Rate());
    settings.setValue("Config/EstimatorPwlMode",
      config.getPwlMode());
    settings.setValue("Config/OutlierProcessingMode",
      config.getOpMode());
    settings.setValue("Config/OutlierProcessingAction",
      config.getOpAction());
    settings.setValue("Config/OutlierProcessingThreshold",
      config.getOpThreshold());
    settings.setValue("Config/OutlierProcessingIterationsLimit",
      config.getOpIterationsLimit());
    settings.setValue("Config/OutlierProcessingSuppressWC",
      config.getOpHave2SuppressWeightCorrection());
    settings.setValue("Config/OutlierProcessingIsSolveCompatible",
      config.getOpIsSolveCompatible());
    settings.setValue("Config/OutlierProcessingHave2NormalizeResiduals",
      config.getOpHave2NormalizeResiduals());
    settings.setValue("Config/DoIono4Sbd",
      config.getDoIonoCorrection4SBD());
    settings.setValue("Config/DoClockBreakDetectionMode_1",
      config.getDoClockBreakDetectionMode1());
    settings.setValue("Config/DoOutlierEliminationMode_1",
      config.getDoOutliersEliminationMode1());
    settings.setValue("Config/IsActiveBandFollowsTab",
      config.getIsActiveBandFollowsTab());
    settings.setValue("Config/IsObservableFollowsPlot",
      config.getIsObservableFollowsPlot());
    settings.setValue("Config/UseExternalAPrioriSitePostions",
      config.getUseExtAPrioriSitesPositions());
    settings.setValue("Config/UseExternalAPrioriSiteVelocities",
      config.getUseExtAPrioriSitesVelocities());
    settings.setValue("Config/UseExternalAPrioriSourcePostions",
      config.getUseExtAPrioriSourcesPositions());
    settings.setValue("Config/UseExternalAPrioriSourceSsm",
      config.getUseExtAPrioriSourceSsm());
    settings.setValue("Config/UseExternalAPrioriEop",
      config.getUseExtAPrioriErp());
    settings.setValue("Config/UseExternalAPrioriAxisOffsets",
      config.getUseExtAPrioriAxisOffsets());
    settings.setValue("Config/UseExternalAPrioriHighFrequencyErp",
      config.getUseExtAPrioriHiFyErp());
    settings.setValue("Config/UseExternalAPrioriMeanGradients",
      config.getUseExtAPrioriMeanGradients());
    settings.setValue("Config/ExternalAPrioriSitePostionsFileName",
      config.getExtAPrioriSitesPositionsFileName());
    settings.setValue("Config/ExternalAPrioriSiteVelocitiesFileName",
      config.getExtAPrioriSitesVelocitiesFileName());
    settings.setValue("Config/ExternalAPrioriSourcePostionsFileName",
      config.getExtAPrioriSourcesPositionsFileName());
    settings.setValue("Config/ExternalAPrioriSourceSsmFileName",
      config.getExtAPrioriSourceSsmFileName());
    settings.setValue("Config/ExternalAPrioriEopFileName",
      config.getExtAPrioriErpFileName());
    settings.setValue("Config/ExternalAPrioriAxisOffsetFileName",
      config.getExtAPrioriAxisOffsetsFileName());
    settings.setValue("Config/ExternalAPrioriHighFrequencyErpFileName",
      config.getExtAPrioriHiFyErpFileName());
    settings.setValue("Config/ExternalAPrioriMeanGradientsFileName",
      config.getExtAPrioriMeanGradientsFileName());
    settings.setValue("Config/EccentricitiesFileName",
      config.getEccentricitiesFileName());
    settings.setValue("Config/Have2ApplySourceSsm",
      config.getHave2ApplySourceSsm());
      

    settings.setValue("Config/RefractionModel",
      config.getRefractionModel());
   
    settings.setValue("Config/Have2ApplyOceanTidesContribution",
      config.getHave2ApplyOceanTideContrib());
    settings.setValue("Config/Have2ApplyPoleTideContribution",
      config.getHave2ApplyPoleTideContrib());
    settings.setValue("Config/Have2ApplyEarthTideContribution",
      config.getHave2ApplyEarthTideContrib());
    settings.setValue("Config/Have2ApplyWobblePxContribution",
      config.getHave2ApplyPxContrib());
    settings.setValue("Config/Have2ApplyWobblePyContribution",
      config.getHave2ApplyPyContrib());
    settings.setValue("Config/Have2ApplyAxisOffsetContribution",
      config.getHave2ApplyAxisOffsetContrib());
    settings.setValue("Config/Have2ApplyNutationHighFrequencyContribution",
      config.getHave2ApplyNutationHFContrib());
    settings.setValue("Config/Have2ApplyPolarMotionHighFrequencyContribution",
      config.getHave2ApplyPxyOceanTideHFContrib());
    settings.setValue("Config/Have2ApplyUT1HighFrequencyContribution",
      config.getHave2ApplyUt1OceanTideHFContrib());
    settings.setValue("Config/Have2ApplyFeedCorrectionContribution",
      config.getHave2ApplyFeedCorrContrib());
    settings.setValue("Config/Have2ApplyTiltRemvrContribution",
      config.getHave2ApplyTiltRemvrContrib());
    settings.setValue("Config/Have2ApplyUnphaseCalibrationContribution",
      config.getHave2ApplyUnPhaseCalContrib());
    settings.setValue("Config/Have2ApplyOceanPoleTideContribution",
      config.getHave2ApplyOceanPoleTideContrib());
    settings.setValue("Config/Have2ApplyGpsIonoContrib",
      config.getHave2ApplyGpsIonoContrib());
    settings.setValue("Config/Have2ApplyPolarMotionLibrationContribution",
      config.getHave2ApplyPxyLibrationContrib());
    settings.setValue("Config/Have2ApplyUT1LibrationContribution",
      config.getHave2ApplyUt1LibrationContrib());
    settings.setValue("Config/Have2ApplyOldOceanTidesContribution",
      config.getHave2ApplyOldOceanTideContrib());
    settings.setValue("Config/Have2ApplyOldPoleTideContribution",
      config.getHave2ApplyOldPoleTideContrib());
    settings.setValue("Config/IsNoviceUser",
      config.getIsNoviceUser());
  };
  // versions:
  settings.setValue("Version/LibraryName",        libraryVersion.getSoftwareName());
  settings.setValue("Version/LibraryMajor",       libraryVersion.getMajorNumber());
  settings.setValue("Version/LibraryMinor",       libraryVersion.getMinorNumber());
  settings.setValue("Version/LibraryTeeny",       libraryVersion.getTeenyNumber());
  settings.setValue("Version/LibraryComments",    libraryVersion.getCodeName());
  settings.setValue("Version/nuSolveName",        nuSolveVersion.getSoftwareName());
  settings.setValue("Version/nuSolveMajor",       nuSolveVersion.getMajorNumber());
  settings.setValue("Version/nuSolveMinor",       nuSolveVersion.getMinorNumber());
  settings.setValue("Version/nuSolveTeeny",       nuSolveVersion.getTeenyNumber());
  settings.setValue("Version/nuSolveComments",    nuSolveVersion.getCodeName());
  settings.setValue("Version/nuSolveSettings",    currentSettingsVersion);
  //
  //
  if (!shouldInvokeSystemWideWizard)
  {
    settings.setValue("Config/LastModifiedNetId",   config.getLastModifiedNetId());
  };
  // clear it:
  if (!shouldInvokeSystemWideWizard)
  {
    settings.remove("Config/AutomaticProcessingByNetId");
    settings.beginWriteArray("Config/AutomaticProcessingByNetId", config.apByNetId().size());
    int                           idx(0);
    for (QMap<QString, SgTaskConfig::AutomaticProcessing>::const_iterator it=config.apByNetId().begin();
      it!=config.apByNetId().end(); ++it, ++idx)
    {
      settings.setArrayIndex(idx);
      const SgTaskConfig::AutomaticProcessing
                               &ap=it.value();
      const QString            &id=it.key();
      settings.setValue("networkId", id);
      settings.setValue("doSessionSetup", ap.doSessionSetup_);
      settings.setValue("doIonoCorrection4SBD", ap.doIonoCorrection4SBD_);
      settings.setValue("doAmbigResolution", ap.doAmbigResolution_);
      settings.setValue("doClockBreaksDetection", ap.doClockBreaksDetection_);
      settings.setValue("doIonoCorrection4All", ap.doIonoCorrection4All_);
      settings.setValue("doOutliers", ap.doOutliers_);
      settings.setValue("doWeights", ap.doWeights_);
      settings.setValue("doReportNotUsedData", ap.doReportNotUsedData_);
      settings.setValue("FinalSolution", ap.finalSolution_);
    };
    settings.endArray();
  //
  };
};



//
int processSession(const QString& databaseName, SgVlbiSessionInfo::OriginType fmt)
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, nuSolveVersion.name() + 
    ": processing " + databaseName + " session");

  bool                          isOk(false);
  NsSessionHandler             *handler;
  handler = new NsSessionHandler(databaseName, "", &config, &parametersDescriptor, fmt);
  if (handler->importSession(false) && handler->performPia())
    isOk = handler->saveResults();
  delete handler;

  return isOk?0:-1;
};
/*=====================================================================================================*/













/*=====================================================================================================*/

