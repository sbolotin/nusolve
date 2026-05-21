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

#include <iostream>
#include <stdlib.h>
#include <sys/utsname.h>


#include <QtCore/QDir>
#include <QtCore/QFileInfo>



#include <SgMasterRecord.h>
#include <SgLogger.h>


#include "VpSetup.h"
#include "vgosDbProcLogs.h"




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
VpSetup::VpSetup() :
  path2Home_        (""),
  path2VgosDbFiles_ ("/500/vgosDb"),
  path2MasterFiles_ ("/500/oper/master"),
  path2SessionFiles_("/500/sessions"),
  path2AuxLogs_     ("Logs"),
  identities_(),
  defaultCableSignByStn_(),
  rinexFileNameByStn_(),
  rinexPressureOffsetByStn_(),
  masterfileSuffixes_(defaultMasterfileSuffixes)
{
  have2SavePerSessionLog_ = true;
  useLocalLocale_ = false;
  //
  defaultCableSignByStn_.insert("WETTZELL", 1);

  rinexFileNameByStn_.insert("WESTFORD", "sa01");
  rinexFileNameByStn_.insert("FORTLEZA", "ceeu");
  rinexPressureOffsetByStn_.insert("WESTFORD", 1.9); //mbar
  useAltMasterfileSuffixes_ = false;
  useOwnOnlyDatabase_ = true;
};



//
VpSetup::~VpSetup()
{
};



//
void VpSetup::setUpBinaryIdentities(const QString& argv0)
{
  QFileInfo                     fi(argv0);
  identities_.setExecBinaryName(fi.fileName());
  identities_.setExecDir(fi.canonicalPath());
  identities_.setCurrentDir(QDir::currentPath());
  identities_.setDriverVersion(vgosDbProcLogsVersion);
};



//
void VpSetup::print2stdOut()
{
  std::cout << "The setup of the software " << qPrintable(vgosDbProcLogsVersion.name()) << ":\n";
  
  std::cout 
    << "   The Home Direcory:\t\t\t\"" << qPrintable(path2Home_) << "\"\n"
    << "   Path to session files:\t\t\"" << qPrintable(path2SessionFiles_) << "\"\n"
    << "   Path to VgosDb files:\t\t\"" << qPrintable(path2VgosDbFiles_) << "\"\n"
    << "   Path to Master files:\t\t\"" << qPrintable(path2MasterFiles_) << "\"\n"
    << "   Save log file for each session:\t" << (have2SavePerSessionLog_?"Yes":"No") << "\n"
    << "   Do not alternate locale set up:\t" << (useLocalLocale_?"Yes":"No") << "\n"
    << "   Use alternate names of masterfiles:\t" << (useAltMasterfileSuffixes_?"Yes":"No") << "\n"
    ;
  if (useAltMasterfileSuffixes_)
    std::cout << "   The alternate names of masterfiles:\t"
              << qPrintable(masterfileSuffixes_.join(",")) << "\n";
  std::cout
    << "   Use local only databases:\t\t" << (useOwnOnlyDatabase_?"Yes":"No") << "\n"
    ;
  if (have2SavePerSessionLog_)
    std::cout 
      << "   Path to logs for each session:\t\"" << qPrintable(path2AuxLogs_) << "\"\n";
  
  identities_.print2stdOut();

};
/*=====================================================================================================*/
