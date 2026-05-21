/*
 *    This file is a part of log2ant utility. The utility is a part of
 *    nuSolve distribution and is designed to extract meteo parameters,
 *    cable calibrations, tsys data from stations log files and store them
 *    in ANTCAL or/and ANTAB format.
 *    Copyright (C) 2020 Sergei Bolotin.
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
#include "log2ant.h"

#include <QtCore/QFileInfo>
#include <QtCore/QString>

#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgStnLogCollector.h>


//#define HAVE_ARGP_H 1
//#undef HAVE_ARGP_H


#ifdef HAVE_ARGP_H
#   include <argp.h>
#else
#   include <unistd.h>
#endif




// HOPS's whims:
#ifdef OLD_HOPS
char                            progname[80];
int                             msglev = 2;
#endif

#ifdef HAVE_ARGP_H
const char                     *argp_program_bug_address = "Sergei Bolotin <sergei.bolotin@nasa.gov>";
#endif



struct l2aOptions
{
  QString                       inputFileName;
  QString                       outputFileName;
  QString                       dbbc3InputFileName;
  QString                       stationKey;
  QString                       compressExt;
  QString                       logFileName;
  SgMJD                         tBegin;
  SgMJD                         tEnd;
  QMap<QString, AntcalOutputData>
                                knownWishedData;
  int                           antcalOutputData;
  int                           reportAllTsysData;
  int                           logLevel;
  bool                          supressNonUsedSensors;
};





#ifdef HAVE_ARGP_H
static int parse_opt(int key, char *arg, struct argp_state *state)
{
  int                           n;
  bool                          isOk;
  QString                       str("");
  struct l2aOptions            *options=(struct l2aOptions*)state->input;

  switch (key)
  {
    case '3':
      options->dbbc3InputFileName = QString(arg);
      break;
    case 'a':
      options->reportAllTsysData = true;
      break;
    case 'b':
      str = QString(arg);
      options->tBegin.fromString(SgMJD::F_FS_LOG, str);
      if (options->tBegin == tZero)
      {
        // try another format:
        options->tBegin.fromString(SgMJD::F_SOLVE_SPLFL_V3, str);
        if (options->tBegin == tZero)
          logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
            ": parse_opt(): option \"-b\": cannot convert \"" + str + "\" to epoch");
      };
      break;
    case 'c':
      options->compressExt = QString(arg);
      break;
    case 'e':
      str = QString(arg);
      options->tEnd.fromString(SgMJD::F_FS_LOG, str);
      if (options->tEnd == tZero)
      {
        // try another format:
        options->tEnd.fromString(SgMJD::F_SOLVE_SPLFL_V3, str);
        if (options->tEnd == tZero)
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
            ": option \"-e\": cannot convert \"" + str + "\" to epoch");
          options->tEnd = tInf;
        };
      };
      break;
    case 'l':
      options->logFileName = QString(arg);
      break;
    case 'o':
      options->outputFileName = QString(arg);
      break;
    case 's':
      options->stationKey = QString(arg).leftJustified(8, ' ');
      break;
    case 't':
      str = QString(arg);
      if (options->knownWishedData.contains(str))
      {
        options->antcalOutputData |= options->knownWishedData.value(str);
        logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-t\": added type of data: \"" + str + "\"");
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-t\": unknown type of data: \"" + str + "\"");
        argp_usage(state);
        return 0;
      };
      break;
    case 'u':
      options->supressNonUsedSensors = true;
      break;
    case 'v':
      str = QString(arg);
      n = str.toInt(&isOk);
      if (isOk)
      {
        if (-1 < n && n < 4)
        {
          options->logLevel = n;
          if (2 < options->logLevel)
            logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
              ": option \"-v\": the log level is set to " + QString("").setNum(options->logLevel));
        }
        else
          logger->write(SgLogger::WRN, SgLogger::SESSION, log2antVersion.name() +
            ": option \"-v\": the level " + str + " is out of range [0..3]");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-v\": cannot convert \"" + str + "\" to int");
      break;
    case 'V':
      std::cout << qPrintable(log2antVersion.name(SgVersion::NF_Petrov)) << "\n";
      exit(0);
      break;

    //
    //
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
      {
        argp_usage(state);
      };
      options->inputFileName = QString(arg);
      break;  
    case ARGP_KEY_END:
      if (state->arg_num < 1)
      argp_usage (state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
      break;
  };
  return 0;
};
#endif








/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
int main(int argc, char** argv)
{
  struct l2aOptions             options;
  int                           rc;

  QString                       userCommand("");
  QString                       logDirName("./");
  QString                       str("");
  SgIdentities                  identities;

  options.inputFileName = QString("");
  options.stationKey = QString("");
  options.dbbc3InputFileName = QString("");
  options.outputFileName = QString("");
  options.compressExt = QString("");
  options.logFileName = QString("log2ant.log");
  options.tBegin = tZero;
  options.tEnd = tInf;
  options.reportAllTsysData = false;
  options.logLevel = 1;
  options.supressNonUsedSensors = false;

  options.knownWishedData.insert("dat", AOD_DATA_ON);
  options.knownWishedData.insert("cbl", AOD_CABLE_CAL);
  options.knownWishedData.insert("met", AOD_METEO);
  options.knownWishedData.insert("tsys",AOD_TSYS);
  options.knownWishedData.insert("tpi", AOD_TPI);
  options.knownWishedData.insert("phc", AOD_PCAL);
  options.knownWishedData.insert("fmt", AOD_FMTGPS);
  options.knownWishedData.insert("tpc", AOD_DBBC3TP);
  options.knownWishedData.insert("sefd",AOD_SEFD);
  options.antcalOutputData = 0;
  
  rc = 0;


  for (int i=0; i<argc; i++)
    userCommand += QString(argv[i]) + " ";
  userCommand.chop(1);



#ifdef HAVE_ARGP_H
  //
  // ARGP setup:
  //
  struct argp_option            argp_options[] =
  {
    {0, 0, 0, 0, "General options:", 10},
    {"station-name",        's', "STRING",  0, 
      "Set a name of a station to STRING"},

    {0, 0, 0, 0, "Input control:",   11},
    {"DBBC3-dump",          '3', "STRING",  0, 
      "Use a file STRING as a DBBC3 dump file"},

    {0, 0, 0, 0, "Output control:",  12},
    {"compress",            'c', "STRING",  0, 
      "Use a compressor STRING (gzip or bzip2) to squeeze output ANTCAL file"},
    {"output",              'o', "STRING",  0, 
      "Set a name of output ANTCAL file to STRING"},

    {0, 0, 0, 0, "Time interval options:", 13},
    {"t-begin",             'b', "STRING",  0, 
      "Set an epoch of the first observation to STRING, data before STRING will be ignored"},
    {"t-end",               'e', "STRING",  0, 
      "Set an epoch of the last observation to STRING, data after STRING will be ignored"},

    {0, 0, 0, 0, "Data filter:", 14},
    {"all",                 'a', 0,         0, 
      "Report all collected data (i.e., both data_valid on and off intervals)"},
    {"data-type",           't', "STRING",  0, 
      "Extract only the specified type of data. STRING can be: "
      "cbl (cable calibration), "
      "dat (data=on/off), "
      "fmt (fmt2gps), "
      "met (meteorological parameters), "
      "phc (phase calibration), "
      "sefd (SEFD evaluation), "
      "tpc (DBBC3 TPC), "
      "tpi (TPI), "
      "tsys (TSYS). "
      "There can be more than one \"-t\" option, e.g.: -t dat -t tsys"},
    {"strip-unused-sensors",'u', 0,         0, 
      "Do not output sensors that are not in a log file for some particular time"},

    {0, 0, 0, 0, "Debug output:", -2},
    {"verbose",             'v', "NUM",     0, 
      "Set a level of log output to NUM: 0 (errors only), 1 (+warnings), 2 (+info) and "
      "3 (+debug output). Default is 1"},
    {"log-file",            'l', "STRING",  0, 
      "Store log2ant's output in a file STRING, the default is \"log2ant.log\""},

    {0, 0, 0, 0, "Operation modes:", -1},
    {"version",             'V', 0,         0, 
      "Print program version"},
    //
    {0}
  };
  QString                       salute("A program that extracts various data from a Field System "
    "log file, FS_LOG_FILE, and stores them in an ANTCAL file.\v");

  salute += "The current version is:\n\t" + log2antVersion.name() + " released on " +
            log2antVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " +
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe utility log2ant is a part of nuSolve package. See datails in "
    "\"log2ant User Guide\", a part of nuSolve distribution. You can get the latest version of "
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");

  struct argp                   argp={argp_options, parse_opt, "FS_LOG_FILE", salute.toLatin1()};

  argp_parse (&argp, argc, argv, 0, 0, &options);
  
  //
  //
  //
  //
#else
  //                      getopt version for the poor ARGPless people:
  //
  QString                       salute("");
  salute += "The current version is:\n\t" + log2antVersion.name() + " released on " +
            log2antVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) +
            "\n\t" + libraryVersion.name() + " released on " +
            libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY);
  salute +=
    QString("\n\nThe utility log2ant is a part of nuSolve package. See datails in "
    "\"log2ant User \nGuide\", a part of nuSolve distribution. You can get the latest version of\n"
    "nuSolve at\n\t\thttps://sourceforge.net/projects/nusolve");


  QString                       help(
    "Usage: log2ant [OPTION...] FS_LOG_FILE\n"
    "A program that extracts various data from a Field System log file, FS_LOG_FILE,\n"
    "and stores them in an ANTCAL file.\n\n"
    " General options:\n"
    "  -s=STRING    Set a name of a station to STRING\n\n"
    " Input control:\n"
    "  -3=STRING    Use a file STRING as a DBBC3 dump file\n\n"
    " Output control:\n"
    "  -c=STRING    Use a compressor STRING (gzip or bzip2) to squeeze\n"
    "               output ANTCAL file\n"
    "  -o=STRING    Set a name of output ANTCAL file to STRING\n\n"
    " Time interval options:\n"
    "  -b=STRING    Set an epoch of the first observation to STRING,\n"
    "               data before STRING will be ignored\n"
    "  -e=STRING    Set an epoch of the last observation to STRING,\n"
    "               data after STRING will be ignored\n\n"
    " Data filter:\n"
    "  -a           Report all collected data (i.e., both data_valid\n"
    "               on and off intervals)\n"
    "  -t=STRING    Extract only the specified type of data. STRING\n"
    "               can be: cbl (cable calibration), dat\n"
    "               (data=on/off), fmt (fmt2gps), met (meteorological\n"
    "               parameters), phc (phase calibration), sefd (SEFD\n"
    "               evaluation), tpc (DBBC3 TPC), tpi (TPI), tsys\n"
    "               (TSYS). There can be more than one \"-t\" option,\n"
    "               e.g.: -t dat -t tsys\n"
    "  -u           Do not output sensors that are not in a log file\n"
    "               for some particular time\n\n"
    " Debug output:\n"
    "  -l=STRING    Store log2ant's output in a file STRING, the\n"
    "               default is \"log2ant.log\"\n"
    "  -v=NUM       Set a level of log output to NUM: 0 (errors only),\n"
    "               1 (+warnings), 2 (+info) and 3 (+debug output).\n"
    "               Default is 1\n\n"
    " Operation modes:\n"
    "  -?,h         Give this help list\n"
    "  -V           Print program version\n\n"
    "Mandatory or optional arguments to long options are also mandatory or optional\n"
    "for any corresponding short options.\n\n");

  help += salute;

//    "The current version is:\n"
//    "	log2ant-0.1.2 (Lands Run Falls) released on 15 May, 2024\n"
//    "	SgLib-0.8.2 (Compton Peak) released on 25 Sep, 2024\n\n"
//    "The utility log2ant is a part of nuSolve package. See datails in \"log2ant User\n"
//    "Guide\", a part of nuSolve distribution. You can get the latest version of\n"
//    "nuSolve at\n"
//    "		https://sourceforge.net/projects/nusolve\n\n"
//    "Report bugs to Sergei Bolotin <sergei.bolotin@nasa.gov>.\n";
//

  const char                   *getopt_options = "3:ab:c:e:l:ho:s:t:uv:V?";
  bool                          isOk;
  int                           c, n;
  while ((c=getopt(argc, argv, getopt_options))!=-1)
    switch (c)
    {
    case '3':
      options.dbbc3InputFileName = QString(optarg);
      break;
    case 'a':
      options.reportAllTsysData = true;
      break;
    case 'b':
      str = QString(optarg);
      options.tBegin.fromString(SgMJD::F_FS_LOG, str);
      if (options.tBegin == tZero)
      {
        // try another format:
        options.tBegin.fromString(SgMJD::F_SOLVE_SPLFL_V3, str);
        if (options.tBegin == tZero)
          logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
            ": option \"-b\": cannot convert \"" + str + "\" to epoch");
      };
      break;
    case 'c':
      options.compressExt = QString(optarg);
      break;
    case 'e':
      str = QString(optarg);
      options.tEnd.fromString(SgMJD::F_FS_LOG, str);
      if (options.tEnd == tZero)
      {
        // try another format:
        options.tEnd.fromString(SgMJD::F_SOLVE_SPLFL_V3, str);
        if (options.tEnd == tZero)
        {
          logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
            ": option \"-e\": cannot convert \"" + str + "\" to epoch");
          options.tEnd = tInf;
        };
      };
      break;
    case 'l':
      options.logFileName = QString(optarg);
      break;
    case 'o':
      options.outputFileName = QString(optarg);
      break;
    case 's':
      options.stationKey = QString(optarg).leftJustified(8, ' ');
      break;
    case 't':
      str = QString(optarg);
      if (options.knownWishedData.contains(str))
      {
        options.antcalOutputData |= options.knownWishedData.value(str);
        logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-t\": added type of data: \"" + str + "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-t\": unknown type of data: \"" + str + "\"");
      break;
    case 'u':
      options.supressNonUsedSensors = true;
      break;
    case 'v':
      str = QString(optarg);
      n = str.toInt(&isOk);
      if (isOk)
      {
        if (-1 < n && n < 4)
        {
          options.logLevel = n;
          if (2 < options.logLevel)
            logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
              ": option \"-v\": the log level is set to " + QString("").setNum(options.logLevel));
        }
        else
          logger->write(SgLogger::WRN, SgLogger::SESSION, log2antVersion.name() +
            ": option \"-v\": the level " + str + " is out of range [0..3]; the log level is unchanged");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::SESSION, log2antVersion.name() +
          ": option \"-v\": cannot convert \"" + str + "\" to int; the log level is unchanged");
      break;
    case 'V':
      std::cout << qPrintable(log2antVersion.name(SgVersion::NF_Petrov)) << "\n";
      exit(0);
      break;
    //
    default:
    case '?':
    case 'h':
      std::cout << qPrintable(help) << "\n";
      return 0;
      break;
    };
  //
  //
  if (optind >= argc)
  {
    std::cout << qPrintable(help) << "\n";
    return 0;
  }
  else
  {
    options.inputFileName = QString(argv[optind]);
  };
  //
  if (options.antcalOutputData == 0)
    options.antcalOutputData = AOD_ALL;
#endif


















  SgMJD                         startEpoch=SgMJD::currentMJD();
  //
  //
  if (options.antcalOutputData == 0)
    options.antcalOutputData = AOD_ALL;

  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() + 
    ": starting");
  //
  // check log file name:
  if (options.logFileName.contains('/'))
  {
    QFileInfo                   fi(options.logFileName);
    options.logFileName = fi.fileName();
    logDirName = fi.absolutePath();
  };
  //
  // setup logger:
  logger->setFileName(options.logFileName);
  logger->setDirName(logDirName);
  logger->setIsStoreInFile(true);
  logger->setIsNeedTimeMark(true);
  logger->setLogFacility(SgLogger::ERR, options.logLevel>=0?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::WRN, options.logLevel>=1?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::INF, options.logLevel>=2?0xFFFFFFFF:0);
  logger->setLogFacility(SgLogger::DBG, options.logLevel==3?0xFFFFFFFF:0);
  // reset the file:
  logger->rmLogFile();

  // put fingerprints:
  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
    ": Library ID: " + libraryVersion.name() + " released on " +
    libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
    ": Driver  ID: " + log2antVersion.name() + " released on " +
    log2antVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
    ": Host    ID: " + identities.getMachineNodeName() +
    " (Hw: " + identities.getMachineMachineName() +
    "; Sw: " + identities.getMachineRelease() +
    " version of " + identities.getMachineSysName() + ")");  
  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
    ": User    ID: " + identities.getUserName() +
    " <" + identities.getUserEmailAddress() + ">");
  logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
    ": User command: \"" + userCommand + "\"");

  if (tZero < options.tBegin)
    logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
      ": expected start of a session is set to " + options.tBegin.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
  if (options.tEnd < tInf)
    logger->write(SgLogger::DBG, SgLogger::SESSION, log2antVersion.name() +
      ": expected end of a session is set to " + options.tEnd.toString(SgMJD::F_YYYYMMDDHHMMSSSS));


  SgStnLogCollector             logCollector;
  bool                          have2owrt=true;

  //
  logCollector.setUserCommand(userCommand);
  logCollector.setAntcalOutputData(options.antcalOutputData);
  //
  if (logCollector.readLogFile(options.inputFileName, 
    options.stationKey, options.tBegin, options.tEnd, ""))
  {
    if (!options.stationKey.size())
      options.stationKey = logCollector.getStationName();

    if (options.dbbc3InputFileName.size())
    {
      if (logCollector.readDbbc3DumpFile(options.dbbc3InputFileName))
        logger->write(SgLogger::DBG, SgLogger::PREPROC, log2antVersion.name() +
          ": the DBBC3 dump file \"" + options.dbbc3InputFileName + "\" have been read"); 
      else
        logger->write(SgLogger::ERR, SgLogger::PREPROC, log2antVersion.name() +
          ": reading of the DBBC3 dump file \"" + options.dbbc3InputFileName + "\" failed for station"); 
    };
    //
    // export data:
    if (logCollector.createAntCalFile(options.stationKey, options.outputFileName, have2owrt,
      options.reportAllTsysData, options.supressNonUsedSensors, options.compressExt))
      logger->write(SgLogger::DBG, SgLogger::PREPROC, log2antVersion.name() +
        ": extracted data were successfully stored in an ANTCAL file for station \"" + 
        options.stationKey + "\"");
    else
    {
      rc = 1;
      logger->write(SgLogger::WRN, SgLogger::PREPROC, log2antVersion.name() +
        ": data storing in an ANTCAL file has failed for station \"" + options.stationKey + "\"");
    };
  }
  else
  {
    rc = 1;
    logger->write(SgLogger::WRN, SgLogger::PREPROC, log2antVersion.name() +
      ": extracting data from the log file \"" + options.inputFileName + "\" failed");
  };
  //
  if (rc == 0)
  {
    SgMJD                       finisEpoch=SgMJD::currentMJD();
    logger->write(SgLogger::INF, SgLogger::SESSION, log2antVersion.name() +
      ": the elapsed time to process log file \"" + options.inputFileName + "\" is: " + 
      interval2Str(finisEpoch - startEpoch) + " (" + 
      QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400.0) + " sec)", true);
  };
  //
  return rc;
};

/*=====================================================================================================*/





/*=====================================================================================================*/
