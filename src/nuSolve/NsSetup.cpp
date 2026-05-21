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

#include <iostream>
#include <stdlib.h>
#include <sys/utsname.h>


#include <QtCore/QDir>
#include <QtCore/QFileInfo>


#include <SgMasterRecord.h>
#include <SgLogger.h>


#include "NsSetup.h"
#include "nuSolve.h"


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsSetup::NsSetup() :
  catnuGetDbInfo_("catnu_find_db"),
  catnuSetDbInfo_("catnu_update_cat"),
  path2Home_(""),
  path2CatNuInterfaceExec_  ("/mk5/bin"),
  path2DbhFiles_            ("/500/databases"),
  path2VgosDbFiles_         ("/500/vgosDb"),
  path2VgosDaFiles_         ("/500/vgosDa"),
  path2APrioriFiles_        ("/500/oper/solve_save_files"),
  path2TrpFiles_            ("/sgpvlbi/trop/vmf3_tot"),
  path2MasterFiles_         ("/500/oper/master"),
  path2SpoolFileOutput_     ("/space/solve/spool_files"),
//path2SpoolFileOutput_     ("spool_files"),
  path2NotUsedObsFileOutput_("/space/solve/work_files"),
  path2ReportOutput_        ("Reports"),
  path2NgsOutput_           ("NGS"),
  path2PlotterOutput_       ("PlotterOutputs"),
  path2IntermediateResults_ ("IntermediateResults"),
  path2AuxLogs_             ("Logs"),
  identities_(),
  externalCommand_(""),
  masterfileSuffixes_(defaultMasterfileSuffixes)
{
  autoSavingMode_ = AS_ON_EXIT;
  have2SavePerSessionLog_ = true;

  have2UpdateCatalog_ = false;
  have2MaskSessionCode_ = false;
  have2KeepSpoolFileReports_ = true;
  have2KeepUnusedObsList_ = false;
  have2LoadImmatureSession_ = false;
  //
  have2WarnCloseWindow_ = true;
  isShortScreen_  = false;
  mainWinWidth_ = 0;
  mainWinHeight_ = 0;
  mainWinPosX_ = 0;
  mainWinPosY_ = 0;
  seWinWidth_ = 0;
  seWinHeight_ = 0;
  have2AutoloadAllBands_ = true;
//isBandPlotPerSrcView_ = false;
  isBandPlotPerSrcView_ = true;
  useOwnOnlyDatabase_ = true;
  
  plotterOutputFormat_ = SgPlot::OF_PDF;
  //
  lnfsFileName_ = "";
  lnfsOriginType_ = SgVlbiSessionInfo::OT_UNKNOWN;
  lnfsIsThroughCatalog_ = true;
  have2SkipAutomaticProcessing_ = false;
  have2ForceAutomaticProcessing_ = false;
  execExternalCommand_        = false;

  useAltMasterfileSuffixes_ = false;
};



//
NsSetup::~NsSetup()
{
};



//
void NsSetup::setUpBinaryIdentities(const QString& argv0)
{
  QFileInfo                     fi(argv0);
  identities_.setExecBinaryName(fi.fileName());
//identities_.setExecDir(fi.canonicalPath());
  identities_.setExecDir(fi.absolutePath());
  identities_.setCurrentDir(QDir::currentPath());
  identities_.setDriverVersion(nuSolveVersion);
};
/*=====================================================================================================*/
