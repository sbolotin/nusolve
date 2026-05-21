/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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
#include <sys/types.h>
#include <pwd.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif



#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiSession.h>



#include "vgosDbCalc.h"
#include "VcCalc2SessionIfc.h"
#include "VcStartupWizard.h"




void loadSettings(QSettings&);
void saveSettings(QSettings&, bool shouldInvokeSystemWideWizard);



VcSetup                         setup;

const QString                   origOrgName("NASA GSFC");
const QString                   origDmnName("gsfc.nasa.gov");
const QString                   origAppName("vgosDbCalc");

// HOPS's whims:
#ifdef OLD_HOPS
char                            progname[80];
int                             msglev = 2;
#endif


// for ARGP parser:
const char                     *argp_program_bug_address = "Sergei Bolotin <sergei.bolotin@nasa.gov>";

struct vdbcOptions
{
  QSettings                    *settings;
  QString                       altSetupName;
  QString                       altSetupAppName;
  QString                       inputArg;
  bool                          have2UseAltSetup;
  bool                          have2ForceWizard;
  bool                          shouldInvokeSystemWideWizard;
  bool                          isDryRun;
  bool                          useStdLocale;
};


//
// a parser for ARGP:
static int parse_opt(int key, char *arg, struct argp_state *state)
{
//  int                           n;
//  bool                          isOk;
  QString                       str("");
  struct vdbcOptions           *options=(struct vdbcOptions*)state->input;
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
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbCalcVersion.name() +
          ": parse_opt(): it is dangerous to use a string \"" + options->altSetupName +
          "\" as an alternative config name");
        logger->write(SgLogger::WRN, SgLogger::IO, vgosDbCalcVersion.name() +
          ": parse_opt(): you can overwrite another file (un)intentionally");
        delete options->settings;
        exit(22);
      };
      break;
    case 'l':
      options->useStdLocale = true;
      break;
    case 'p':
      loadSettings(*options->settings);
      setup.print2stdOut();
      exit(0);
      break;
    case 'q':
      options->isDryRun = true;
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
      std::cout << qPrintable(vgosDbCalcVersion.name(SgVersion::NF_Petrov)) << "\n";
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




bool createCalcExtFile(const QString& fileName, int fd);
bool createCalcOnnFile(const QString& calcOnnFileName, int fd, const QString& sessionName, int version,
  const QString& acName, const QString& userName);
bool checkAprioriFiles();





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
  struct vdbcOptions            options;
#ifdef SWCONFIG
  const QString                 path2SystemWideConfig(SWCONFIG "/xdg");
#else
  const QString                 path2SystemWideConfig("");
#endif
  QString                       userCommand("");
  bool                          isFirstRun;
  bool                          isOk;
  SgLogger                     *alHistory;
  SgLogger                     *alDriver;
  const char                   *calcMode = "mark3 ";
  int                           rc;

  rc = 0;
  options.settings = NULL;
  options.altSetupAppName = QString("");
  options.altSetupName = QString("");
  options.inputArg = QString("");
  options.have2UseAltSetup = false;
  options.have2ForceWizard = false;
  options.shouldInvokeSystemWideWizard = false;
  options.isDryRun = false;
  options.useStdLocale = false;

  QCoreApplication::setOrganizationName(origOrgName);
  QCoreApplication::setOrganizationDomain(origDmnName);
  QCoreApplication::setApplicationName(origAppName);

  // set up path to the system wide settings:
  QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, path2SystemWideConfig);
  //
  //
  options.settings = new QSettings;
  //
  // init the external structures:
  // strncpy(mode_.c_mode, "mark3 ", 6);
  for (int i=0; i<6; i++)
    mode_.c_mode[i] = *(calcMode + i);
  // clear the records:
  memset(extrnl_.External_inputs, 0, sizeof(extrnl_.External_inputs));
  memset(extrnl_.Ex_sites,        0, sizeof(extrnl_.Ex_sites));
  memset(extrnl_.Ex_stars,        0, sizeof(extrnl_.Ex_stars));
  memset(extrnl_.Ex_ocean,        0, sizeof(extrnl_.Ex_ocean));
  memset(extrnl_.Ex_EOP,          0, sizeof(extrnl_.Ex_EOP));
  memset(extrnl_.Ex_tilts,        0, sizeof(extrnl_.Ex_tilts));
  memset(extrnl_.Ex_OPTL,         0, sizeof(extrnl_.Ex_OPTL));
  extrnl_.External_aprioris = f_false;
  extrnl_.Input_sites = f_false;
  extrnl_.Input_stars = f_false;
  extrnl_.Input_ocean = f_false;
  extrnl_.Input_EOP = f_false;
  extrnl_.Input_tilts = f_false;
  extrnl_.Input_OPTL = f_false;
  extrnl_.le4 = f_false;

  // check for the legacy-style invocation:
  if (argc==5)
  {
    QString                     sArg1(argv[1]), sArg2(argv[2]), sArg3(argv[3]), sArg4(argv[4]);
    int                         nArg1, nArg2;
    isOk = true;
    nArg1 = nArg2 = 0;
    nArg1 = sArg1.toInt(&isOk);
    if (isOk)
      nArg2 = sArg2.toInt(&isOk);
    if (isOk) // check for calcOn file:
    {
      QFileInfo                 fi(sArg3);
      if (!(fi.exists() && fi.isReadable()))
        isOk = false;
    };
    if (isOk) // check for "external a priori" file:
    {
      QFileInfo                 fi(sArg4);
      if (!(fi.exists() && fi.isReadable()))
        isOk = false;
    };
    if (isOk) // looks like correct invocation
    {
      loadSettings(*options.settings);
      if (!checkAprioriFiles())
      {
        rc = 1;
        logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
          ": one of the external a priori files is missed, exiting");
        return rc;
      }
      else
      {
        c2s.setCalcCallingMode(VcCalc2SessionIfc::CCM_Legacy);
        mimicCalcMain(nArg1, nArg2, argv[3], argv[4]);
      };
    };
  };
  //
  // ok, normal calling:
  bool                          isGuiEnabled;
  const char*                   envDisplay=NULL;
  //
  isGuiEnabled = (envDisplay=getenv("DISPLAY"))!=NULL && 0<strlen(envDisplay);

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
  strcpy(progname, qPrintable("HOPS (on behalf of " + vgosDbCalcVersion.getSoftwareName() + ")"));
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
    {0, 0, 0, 0, "General options:", 10},
    {"std-locale",          'l', 0,         0,
      "Use the standard locale"},

    {0, 0, 0, 0, "Configuration control:", 11},
    {"alt",                 'a', "STRING",  0,
      "Use an alternative configuration STRING"},

    {0, 0, 0, 0, "Invocation of startup wizard:", 15},
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
  QString                       salute("vgosDbCalc is a program that reads vgosDb database, "
    "calculates theoretical values of delays and rates as well as partials for many parameters and "
    "stores them in a new version of the database. The mandatory argument DATABASE is a name of a "
    "database (with or without version part) or a name of wrapper file (absolute or relative to the "
    "default path of the vgosDb files).\v");

  salute += "The current version is:\n\t" + vgosDbCalcVersion.name() + " released on " +
            vgosDbCalcVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " +
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe utility vgosDbCalc is a part of nuSolve package. See the datails in "
    "\"vgosDbCalc User Guide\", a part of nuSolve distribution. You can get the latest version of "
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");

  struct argp                   argp={argp_options, parse_opt, "DATABASE", salute.toLatin1()};

  argp_parse (&argp, argc, argv, 0, 0, &options);

  //
  //
  isFirstRun = options.settings->allKeys().size()>0 ? false : true;
  //
  //

  //
  if (options.have2UseAltSetup)
  {
    logger->write(SgLogger::INF, SgLogger::IO, vgosDbCalcVersion.name() +
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
  if (isGuiEnabled)
  {
    if (options.have2ForceWizard || options.shouldInvokeSystemWideWizard ||
        options.settings->value("Version/StartUpWizard", 0).toInt() < VcStartupWizard::serialNumber())
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
            logger->write(SgLogger::ERR, SgLogger::IO, vgosDbCalcVersion.name() +
              ": cannot write system wide config");
            delete swSettings;
            return rc;
          };
          delete options.settings;
          options.settings = swSettings;
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, vgosDbCalcVersion.name() +
            ": using an alternative config name, system wide config edit request ignored");
      };
      //
      VcStartupWizard           startup(isFirstRun, options.have2ForceWizard, 
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
    logger->write(SgLogger::ERR, SgLogger::GUI, vgosDbCalcVersion.name() +
      ": cannot run graphical application.");
    delete options.settings;
    logger->clearSpool();
    return rc;
  };
  //
  if (!options.inputArg.size())
  {
    rc = 1;
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": no input files provided.");
    logger->clearSpool();
    return rc;
  };
  //
  if (!checkAprioriFiles())
  {
    rc = 1;
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": one of the external a priori files is missed, exiting");
    return rc;
  };
  //
  // determine a name of a wrapper file:
  QString                       path2wrapperFile("");
  QString                       wrapperFileName("");
  QString                       sessionName("");
  int                           version(0);
//QRegExp                       reDatabaseName("^[0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}$");
//QRegExp                       reDatabaseVersionName("^[0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}_V[0-9]{3}$");
  QRegularExpression            reOldDbName("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName("^(\\d{8})-([\\S]{2,12}).*");
//QRegularExpressionMatch       match;

  
  // ok, this is a wrapper file name:
  if (options.inputArg.right(4) == ".wrp")
  {
    isOk = SgVlbiSession::guessSessionByWrapperFileName(options.inputArg,
      setup.path2(setup.getPath2VgosDbFiles()), path2wrapperFile, wrapperFileName, sessionName, version);
  }
  else if ( reOldDbName.match(options.inputArg).hasMatch() ||
            reNewDbName.match(options.inputArg).hasMatch()  )
//  else if (reDatabaseName.exactMatch(options.inputArg) || 
//          reDatabaseVersionName.exactMatch(options.inputArg))
  {
    isOk = SgVlbiSession::guessWrapperFileNameBySession(options.inputArg, 
      setup.path2(setup.getPath2VgosDbFiles()), 
      setup.identities().getAcAbbName(), setup.getUseOwnOnlyDatabase(),
      path2wrapperFile, wrapperFileName, sessionName, version);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": cannot guess input data.");
    isOk = false;
    rc = 1;
  };

  char                          buff[180];
  QString                       calcExtFileName("");
  QString                       calcOnnFileName("");
  QString                       userLoginName("unkn");
  QString                       path2tmpDir("/tmp");
  int                           fd;
  QStringList                   lst;
  //
  if (setup.getUseAltMasterfileSuffixes())
    lst = setup.getMasterfileSuffixes();


  if (getenv("TMPDIR") && strlen(getenv("TMPDIR"))>0)
  {
    path2tmpDir = QString(getenv("TMPDIR"));
    logger->write(SgLogger::DBG, SgLogger::IO, vgosDbCalcVersion.name() +
      ": the path to tmp dir has been alternated: \"" + path2tmpDir + "\"");
  };

  // guess user login name:
  uid_t                         uid=geteuid();
  struct passwd                *pw=getpwuid(uid);
  if (!pw)
    perror("getpwuid: ");
  else
    userLoginName = pw->pw_name;

  if (isOk)
  {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%s/%s.calcExt.%s-XXXXXX",
      qPrintable(path2tmpDir),
      qPrintable(vgosDbCalcVersion.getSoftwareName()), qPrintable(userLoginName));
    if ((fd=mkstemp(buff)) == -1)
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": cannot create calcExt file.");
      isOk = false;
      rc = 1;
    };
  };
  if (isOk)
  {
    calcExtFileName = buff;
    isOk = createCalcExtFile(calcExtFileName, fd);
  };
  //
  if (isOk)
  {
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%s/%s.calcOn.%s-XXXXXX", 
      qPrintable(path2tmpDir),
      qPrintable(vgosDbCalcVersion.getSoftwareName()), qPrintable(userLoginName));
    if ((fd=mkstemp(buff)) == -1)
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": cannot create calcOn file.");
      isOk = false;
      rc = 1;
    };
  };
  if (isOk)
  {
    calcOnnFileName = buff;
    isOk = createCalcOnnFile(calcOnnFileName, fd, sessionName, version, 
      setup.identities().getAcAbbrevName(), setup.identities().getUserDefaultInitials());
  };
  //
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
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the env.variable LANG was set to \"C\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the env.variable LC_ALL was set to \"C\"");
  };
  //
  //
  if (isOk)
  {
    SgMJD                       startEpoch=SgMJD::currentMJD();
    logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() + 
      ": starting");

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
    //
    alHistory = new SgLogger(0, false, "");
    alHistory->setIsNeedTimeMark(false);
    alHistory->setIsMute(true);
    alHistory->setLogFacility(SgLogger::ERR, 0xFFFFFFFF);
    alHistory->setLogFacility(SgLogger::WRN, 0xFFFFFFFF);
    alHistory->setLogFacility(SgLogger::INF, 0xFFFFFFFF);
    alHistory->setLogFacility(SgLogger::DBG, 0);
    //    commented out: want logs from calc only
    //    logger->addSupplementLog("History", alHistory);
    //
    //
    SgVgosDb                   *vgosDb;
    SgVlbiSession               session;
    //
    session.setPath2Masterfile(setup.path2(setup.getPath2MasterFiles()));
    //
    //
    //------------------------------------------------------------------------------------------
    vgosDb = new SgVgosDb(&setup.identities(), &vgosDbCalcVersion);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the vgosDb object has been created");

    isOk = vgosDb->init(path2wrapperFile, wrapperFileName);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() + 
      ": parsing of wrapper file " + path2wrapperFile + "/" + wrapperFileName + 
      (isOk?" complete":" failed"));

    if (isOk && session.getDataFromVgosDb(vgosDb, true, isGuiEnabled, lst))
    {
      // collect log entries to save them into the history block of the session:
      logger->attachSupplementLog("History", alHistory);

      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Library ID: " + libraryVersion.name() + " released on " + 
        libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Driver  ID: " + vgosDbCalcVersion.name() + " released on " + 
        vgosDbCalcVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));

      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Host    ID: " + setup.identities().getMachineNodeName() + 
        " (Hw: " + setup.identities().getMachineMachineName() + 
        "; Sw: " + setup.identities().getMachineRelease() + 
        " version of " + setup.identities().getMachineSysName() + ")");

      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": User    ID: " + setup.identities().getUserName() + 
        " <" + setup.identities().getUserEmailAddress() + ">, " + setup.identities().getAcFullName());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": User command: \"" + userCommand + "\"");
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Input data  : " + path2wrapperFile + "/" + wrapperFileName);

      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": External a priori setup: ");
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": SITES    : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sites());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Sources  : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sources());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Ocean    : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4OceanLd());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": EOPs     : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Eops());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": Tilts    : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Tilts());
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": OPTL     : " + 
        setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4OceanPoleTideLd());

      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": the session " + session.getSessionCode() + " (" + session.getName() + 
        ") has been imported from vgosDb data set");

      // set up calc2session interface:
      c2s.setCalcCallingMode(VcCalc2SessionIfc::CCM_Modern);
      c2s.session() = &session;
      c2s.vgosDb() = vgosDb;
      if (sessionName.size()==8)
        sessionName = sessionName + ' ';
      c2s.setInputDbName(sessionName);
      c2s.setPath2Data(path2wrapperFile);
      c2s.setWrapperFileName(wrapperFileName);

      // run calc:
      QList<QString>            bands;
      QString                   bandKey(""), bandList("");
      if (session.primaryBand())
      {
        bandKey = session.primaryBand()->getKey();
        bands.append(bandKey);
        bandList = bandKey + ", ";
      };
      for (int i=0; i<session.bands().size(); i++)
        if (session.bands().at(i)->getKey() != bandKey)
        {
          bands.append(session.bands().at(i)->getKey());
          bandList += session.bands().at(i)->getKey() + ", ";
        };
      if (bandList.size() > 2)
        bandList.chop(2);
      logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": collected " + QString("").setNum(bands.size()) + " bands: " + bandList);
      //
      //                                call CALC:
      //
      mimicCalcMain2(0, -1, qPrintable(calcOnnFileName), qPrintable(calcExtFileName), bands);
      //
      logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": running observations through CALC has " + (isOk?"been complete":"failed"));

      // save the session:
      if (isOk)
      {
        session.addAttr(SgVlbiSessionInfo::Attr_FF_CALC_DATA_MODIFIED);
        if (options.isDryRun)
          vgosDb->setOperationMode(SgNetCdf::OM_DRY_RUN);

        isOk = session.putDataIntoVgosDb(vgosDb);
        logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
          ": export of data into vgosDB tree has " + (isOk?"been complete":"failed"));
        if (!isOk)
          rc = 1;
      }
      else
        logger->write(SgLogger::INF, SgLogger::SESSION, vgosDbCalcVersion.name() +
          ": nothing to write");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::SESSION, vgosDbCalcVersion.name() +
        ": import data of " + path2wrapperFile + "/" + wrapperFileName + " failed");
      rc = 1;
    };
    //
    alDriver->clearSpool();
    logger->detachSupplementLog("History");
    logger->detachSupplementLog("Driver");
    delete alHistory;
    delete alDriver;
    //
    delete vgosDb;
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": vgosDb object has been destroyed.");

    // report elapsed time:
    SgMJD                       finisEpoch=SgMJD::currentMJD();
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the elapsed time to process " + QString("").setNum(session.observations().size()) + 
      " observations is: " + interval2Str(finisEpoch - startEpoch) + 
      " (" + QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400.0) + " sec)", true);
  }
  else
    rc = 1;
  //
  if (!setup.getUseLocalLocale() || options.useStdLocale)
  {
    setenv("LANG", qPrintable(sLang), 1);
    setenv("LC_ALL", qPrintable(sLcAll), 1);
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the env.variable LANG was set to \"" + sLang + "\"");
    logger->write(SgLogger::DBG, SgLogger::SESSION, vgosDbCalcVersion.name() +
      ": the env.variable LC_ALL was set to \"" + sLcAll + "\"");
  };
  //
  if (QFile::exists(calcExtFileName))
    QFile::remove(calcExtFileName);
  if (QFile::exists(calcOnnFileName))
    QFile::remove(calcOnnFileName);
  
  delete options.settings;
  logger->clearSpool();
  return rc;
};



//
bool createCalcExtFile(const QString& fileName, int fd)
{
  QFile                         f(fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "createCalcExtFile(): error opening calcExt file: " + fileName);
    return false;
  };
  QTextStream                   ts(&f);
  close(fd);
  QString                       str;
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sites();
  ts << "SITES    " << str << "\n";
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sources();
  ts << "Sources  " << str << "\n";
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4OceanLd();
  ts << "Ocean    " << str << "\n";
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Eops();
  ts << "EOPs     " << str << "\n";
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Tilts();
  ts << "Tilts    " << str << "\n";
  str = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4OceanPoleTideLd();
  ts << "OPTL     " << str << "\n";
  ts.setDevice(NULL);
  f.close();
  logger->write(SgLogger::DBG, SgLogger::SESSION,
    "createCalcExtFile(): the external a priori parameters were saved in the file \"" + fileName + "\"");
  return true;
};



//
bool createCalcOnnFile(const QString& calcOnnFileName, int fd, const QString& sessionName, int version,
  const QString& acName, const QString& userName)
{
  QFile                         f(calcOnnFileName);
  QString                       str("");
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "createCalcOnnFile(): error opening calcExt file: " + calcOnnFileName);
    return false;
  };
  QTextStream ts(&f);
  close (fd);
  // to override the CALC's bug:
  histIntroRec = "Calc 11 at " + acName + "  -" + userName + "-";
  ts  << qPrintable(histIntroRec) << "\n";
  str.sprintf("$%-22s %3d     $%-22s", qPrintable(sessionName), version, qPrintable(sessionName));
//str.sprintf("$%-9s %8d      $%-9s", qPrintable(sessionName), version, qPrintable(sessionName));
  ts  << qPrintable(str) << "\n"
      << "00000000000000000000000000000000000000000000000000000000000000\n";

  ts.setDevice(NULL);
  f.close();
  logger->write(SgLogger::DBG, SgLogger::SESSION,
    "createCalcOnnFile(): the CALC control data were saved in the file \"" + 
    calcOnnFileName + "\"");
  return true;
};



//
bool checkAprioriFiles()
{
  bool                          isOk;
  QString                       fileName;
  isOk = true;
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sites();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for sites \"" + fileName + "\" does not exist");
    isOk = false;
  };
  
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Sources();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for sources \"" + fileName + "\" does not exist");
    isOk = false;
  };
  
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4OceanLd();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for ocean loading \"" + fileName + 
      "\" does not exist");
    isOk = false;
  };
  
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Eops();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for EOPs \"" + fileName + "\" does not exist");
    isOk = false;
  };
  
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + setup.getAprioriFile4Tilts();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for tilts \"" + fileName + "\" does not exist");
    isOk = false;
  };
  
  fileName = setup.path2(setup.getPath2CalcAprioriFiles()) + "/" + 
    setup.getAprioriFile4OceanPoleTideLd();
  if (!QFile::exists(fileName))
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION,
      "checkAprioriFiles(): the external a priori file for ocean pole tide loading \"" + fileName + 
      "\" does not exist");
    isOk = false;
  };
  return isOk;
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
  
  setup.setPath2CalcAprioriFiles(
    settings.value("Setup/Path2CalcAPrioriFiles",
      settings.value("Setup/Path2SolveAPrioriFiles", setup.getPath2CalcAprioriFiles())).toString());
  
  setup.setPath2VgosDbFiles(
    settings.value("Setup/Path2VgosDbFiles",
    setup.getPath2VgosDbFiles()).toString());
  setup.setPath2MasterFiles(
    settings.value("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles()).toString());
  setup.setAprioriFile4Sites(
    settings.value("Setup/CalcAprioriFileSites",
    setup.getAprioriFile4Sites()).toString());
  setup.setAprioriFile4Sources(
    settings.value("Setup/CalcAprioriFileSources",
    setup.getAprioriFile4Sources()).toString());
  setup.setAprioriFile4OceanLd(
    settings.value("Setup/CalcAprioriFileOceanLd",
    setup.getAprioriFile4OceanLd()).toString());
  setup.setAprioriFile4Eops(
    settings.value("Setup/CalcAprioriFileEops",
    setup.getAprioriFile4Eops()).toString());
  setup.setAprioriFile4Tilts(
    settings.value("Setup/CalcAprioriFileTilts",
    setup.getAprioriFile4Tilts()).toString());
  setup.setAprioriFile4OceanPoleTideLd(
    settings.value("Setup/CalcAprioriFileOceanPoleTideLd",
    setup.getAprioriFile4OceanPoleTideLd()).toString());
  setup.setPath2AuxLogs(
    settings.value("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs()).toString());
  setup.setHave2SavePerSessionLog(
    settings.value("Setup/Have2SavePerSessionLog",
    setup.getHave2SavePerSessionLog()).toBool());
  setup.setUseLocalLocale(
    settings.value("Setup/UseLocalLocale",
    setup.getUseLocalLocale()).toBool());
  setup.setUseOwnOnlyDatabase(
    settings.value("Setup/UseOwnOnlyDatabase",
    setup.getUseOwnOnlyDatabase()).toBool());
  //
  // adjust the logger:
  logger->setFileName(
    settings.value("Logger/FileName", 
    "vgosDbCalc.log").toString());
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
  /*
  QString                       str("");
  for (int i=0; i<setup.getMasterfileSuffixes().size(); i++)
    str += setup.getMasterfileSuffixes().at(i) + ",";
  str.chop(1);
  */
  QString                       str=setup.getMasterfileSuffixes().join(",");
  str = settings.value("Setup/MasterFileExtensions", str).toString();
  setup.setMasterfileSuffixes(str.split(QRegularExpression("[,;:]"), QString::SkipEmptyParts));
  //
  // clear old stuff:
  if (settings.contains("Setup/Path2SolveAPrioriFiles"))
  {
    settings.setValue("Setup/Path2CalcAPrioriFiles", setup.getPath2CalcAprioriFiles());
    settings.remove("Setup/Path2SolveAPrioriFiles");
    logger->write(SgLogger::DBG, SgLogger::CONFIG, vgosDbCalcVersion.name() +
        ": an obsolete setting key was removed.");
  };
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
  settings.setValue("Setup/Path2CalcAPrioriFiles",
    setup.getPath2CalcAprioriFiles());
  settings.setValue("Setup/Path2VgosDbFiles",
    setup.getPath2VgosDbFiles());
  settings.setValue("Setup/Path2MasterFiles",
    setup.getPath2MasterFiles());
  settings.setValue("Setup/CalcAprioriFileSites",
    setup.getAprioriFile4Sites());
  settings.setValue("Setup/CalcAprioriFileSources",
    setup.getAprioriFile4Sources());
  settings.setValue("Setup/CalcAprioriFileOceanLd",
    setup.getAprioriFile4OceanLd());
  settings.setValue("Setup/CalcAprioriFileEops",
    setup.getAprioriFile4Eops());
  settings.setValue("Setup/CalcAprioriFileTilts",
    setup.getAprioriFile4Tilts());
  settings.setValue("Setup/CalcAprioriFileOceanPoleTideLd",
    setup.getAprioriFile4OceanPoleTideLd());
  settings.setValue("Setup/Path2AuxLogs",
    setup.getPath2AuxLogs());
  settings.setValue("Setup/Have2SavePerSessionLog",
    setup.getHave2SavePerSessionLog());
  settings.setValue("Setup/UseLocalLocale",
    setup.getUseLocalLocale());
  settings.setValue("Setup/UseOwnOnlyDatabase",
    setup.getUseOwnOnlyDatabase());
  //
  // masterfile extensions:
  settings.setValue("Setup/UseAltMasterFileExtensions",
    setup.getUseAltMasterfileSuffixes());
  //
  /*
  QString                       str("");
  for (int i=0; i<setup.getMasterfileSuffixes().size(); i++)
    str += setup.getMasterfileSuffixes().at(i) + ",";
  str.chop(1);
  */
  settings.setValue("Setup/MasterFileExtensions", setup.getMasterfileSuffixes().join(","));
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
/*=====================================================================================================*/



/*=====================================================================================================*/
