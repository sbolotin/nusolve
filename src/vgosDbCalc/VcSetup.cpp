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

#include <iostream>
#include <stdlib.h>
#include <sys/utsname.h>


#include <QtCore/QDir>
#include <QtCore/QFileInfo>


#include <SgMasterRecord.h>
#include <SgLogger.h>


#include "VcSetup.h"
#include "vgosDbCalc.h"




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
VcSetup::VcSetup() :
  path2Home_                  (""),
  path2VgosDbFiles_           ("/500/vgosDb"),
  path2MasterFiles_           ("/500/oper/master"),
  path2CalcAprioriFiles_      ("/500/oper/calc_apriori_files"),
  aPrioriFile4Sites_          ("blokq.c11.dat"),
  aPrioriFile4Sources_        ("blokq.c11.dat"),
  aPrioriFile4OceanLd_        ("blokq.c11.dat"),
  aPrioriFile4Eops_           ("usno_finals.erp"),
  aPrioriFile4Tilts_          ("tilt.dat"),
  aPrioriFile4OceanPoleTideLd_("blokq.c11.dat"),
  path2AuxLogs_               ("Logs"),
  identities_(),
  masterfileSuffixes_(defaultMasterfileSuffixes)
{
  have2SavePerSessionLog_ = true;
  useLocalLocale_ = false;
  useAltMasterfileSuffixes_ = false;
  useOwnOnlyDatabase_ = true;
  //
};



//
VcSetup::~VcSetup()
{
};



//
void VcSetup::setUpBinaryIdentities(const QString& argv0)
{
  QFileInfo                     fi(argv0);
  identities_.setExecBinaryName(fi.fileName());
  identities_.setExecDir(fi.canonicalPath());
  identities_.setCurrentDir(QDir::currentPath());
  identities_.setDriverVersion(vgosDbCalcVersion);
};



//
void VcSetup::print2stdOut()
{
  std::cout << "The setup of the software " << qPrintable(vgosDbCalcVersion.name()) << ":\n";

  std::cout
    << "   The Home Direcory:\t\t\t\"" << qPrintable(path2Home_) << "\"\n"
    << "   Path to CALC a priori files:\t\"" << qPrintable(path2CalcAprioriFiles_) << "\"\n"
    << "   Path to VgosDb files:\t\t\"" << qPrintable(path2VgosDbFiles_) << "\"\n"
    << "   Path to Master files:\t\t\"" << qPrintable(path2MasterFiles_) << "\"\n"
    << "   A priori file names:\n"

    << "   Site positions and velociities:\t\"" << qPrintable(aPrioriFile4Sites_) << "\"\n"
    << "   Source coordinates:\t\t\t\"" << qPrintable(aPrioriFile4Sources_) << "\"\n"
    << "   Ocean loading parameters:\t\t\"" << qPrintable(aPrioriFile4OceanLd_) << "\"\n"
    << "   EOP:\t\t\t\t\t\"" << qPrintable(aPrioriFile4Eops_) << "\"\n"
    << "   Antenna tilts:\t\t\t\"" << qPrintable(aPrioriFile4Tilts_) << "\"\n"
    << "   Old ocean loading parameters:\t\"" << qPrintable(aPrioriFile4OceanPoleTideLd_) << "\"\n"
    << "   Save log file for each session:\t" << (have2SavePerSessionLog_?"Yes":"No") << "\n"
    << "   Do not alternate locale set up:\t" << (useLocalLocale_?"Yes":"No") << "\n"
    << "   Use alternate names of masterfiles:\t" << (useAltMasterfileSuffixes_?"Yes":"No") << "\n"
    ;
  if (useAltMasterfileSuffixes_)
  {
    std::cout << "   The alternate names of masterfiles:\t"
              << qPrintable(masterfileSuffixes_.join(",")) << "\n";
  };
  std::cout
    << "   Use local only databases:\t\t" << (useOwnOnlyDatabase_?"Yes":"No") << "\n"
    ;

  if (have2SavePerSessionLog_)
    std::cout
      << "   Path to logs for each session:\t\"" << qPrintable(path2AuxLogs_) << "\"\n"
      ;

  identities_.print2stdOut();
};

/*=====================================================================================================*/
