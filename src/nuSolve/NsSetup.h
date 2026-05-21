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

#ifndef NS_SETUP_H
#define NS_SETUP_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgGuiPlotter.h>
#include <SgIdentities.h>
#include <SgVlbiSessionInfo.h>


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsSetup
{
public:
  enum AutoSaving
  {
    AS_NONE                   = 0,
    AS_ON_EXIT                = 1,
    AS_ALWAYS                 = 2,
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsSetup();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsSetup();



  //
  // Interfaces:
  //
  // gets:
  /**Returns a path to nuSolve's home.
   */
  inline const QString& getPath2Home() const;

  /**Returns a path to 'Catalog<=>nuSolve Interface' executables.
   */
  inline const QString& getPath2CatNuInterfaceExec() const;

  /**Returns a path to DBH files (observations).
   */
  inline const QString& getPath2DbhFiles() const;

  /**Returns a path to vgosDb files (observations).
   */
  inline const QString& getPath2VgosDbFiles() const;
  inline const QString& getPath2VgosDaFiles() const {return path2VgosDaFiles_;};

  /**Returns a path to files with a priori information.
   */
  inline const QString& getPath2APrioriFiles() const;

  inline const QString& getPath2TrpFiles() const {return path2TrpFiles_;};

  /**Returns a path to masterfiles.
   */
  inline const QString& getPath2MasterFiles() const;

  /**Returns a path where to write a report in "spoolfile" format.
   */
  inline const QString& getPath2SpoolFileOutput() const;

  inline const QString& getPath2NotUsedObsFileOutput() const;

  /**Returns a path where to write a report in "spoolfile" format (aux copy).
   */
  inline const QString& getPath2ReportOutput() const;

  /**Returns a path where to save observations in NGS format.
   */
  inline const QString& getPath2NgsOutput() const;

  /**Returns a path where the plotter will save its pictures.
   */
  inline const QString& getPath2PlotterOutput() const;

  /**Returns a path where the itermediate results will be saved.
   */
  inline const QString& getPath2IntermediateResults() const;
  
  inline AutoSaving getAutoSavingMode() const;

  /**Returns a path to auxiliary logs.
   */
  inline const QString& getPath2AuxLogs() const;

  /**Returns true if the software have to save an auxiliary log.
   */
  inline bool getHave2SavePerSessionLog() const;

  /**Returns true if export of session have to update the catalog.
   */
  inline bool getHave2UpdateCatalog() const;

  /**Returns true if in export session the session code have to be modified (for test purposes).
   */
  inline bool getHave2MaskSessionCode() const;

  /**Returns true if "spool file" report should be copyed into "report" directory.
   */
  inline bool getHave2KeepSpoolFileReports() const;

  inline bool getHave2KeepUnusedObsList() const {return have2KeepUnusedObsList_;};

  /**Returns true if nuSolve supposed to read a session that contains only a part of essential data.
   */
  inline bool getHave2LoadImmatureSession() const;

  /**Returns identities of user, analysis center, computer, etc.
   */
  inline const SgIdentities& getIdentities() const;

  /**Returns a reference on identities.
   */
  inline SgIdentities& identities();
  
  inline bool getHave2WarnCloseWindow() const;

  inline bool getIsShortScreen() const;

  inline int getMainWinWidth() const;
  
  inline int getMainWinHeight() const;

  inline int getMainWinPosX() const;
  
  inline int getMainWinPosY() const;

  inline int getSeWinWidth() const;

  inline int getSeWinHeight() const;

  inline bool getHave2AutoloadAllBands() const;
  
  inline bool getIsBandPlotPerSrcView() const;

  inline bool getUseOwnOnlyDatabase() const;
  
  inline SgPlot::OutputFormat getPlotterOutputFormat() const;

  inline const QString& getLnfsFileName() const;
  
  inline SgVlbiSessionInfo::OriginType getLnfsOriginType() const;
  
  inline bool getLnfsIsThroughCatalog() const;

  inline bool getHave2SkipAutomaticProcessing() const;

  inline bool getHave2ForceAutomaticProcessing() const;

  inline bool getExecExternalCommand() const {return execExternalCommand_;};

  inline const QString& getExternalCommand() const {return externalCommand_;};


  inline const QStringList& getMasterfileSuffixes() const {return masterfileSuffixes_;};
  inline bool getUseAltMasterfileSuffixes() {return useAltMasterfileSuffixes_;};


  //
  // sets:
  /**Sets up a path to nuSolve's home.
   */
  inline void setPath2Home(const QString&);

  /**Sets up a path to 'Catalog<=>nuSolve Interface' executables.
   */
  inline void setPath2CatNuInterfaceExec(const QString&);

  /**Sets up a path to DBH files (observations).
   */
  inline void setPath2DbhFiles(const QString&);

  /**Sets up a path to vgosDb files (observations).
   */
  inline void setPath2VgosDbFiles(const QString&);
  inline void setPath2VgosDaFiles(const QString& path) {path2VgosDaFiles_ = path;};

  /**Sets up a path to files with a priori information.
   */
  inline void setPath2APrioriFiles(const QString&);

  inline void setPath2TrpFiles(const QString& str) {path2TrpFiles_=str;};

  /**Sets up a path to masterfiles.
   */
  inline void setPath2MasterFiles(const QString&);

  /**Sets up a path where to write a report in "spoolfile" format.
   */
  inline void setPath2SpoolFileOutput(const QString&);

  inline void setPath2NotUsedObsFileOutput(const QString&);

  /**Sets up a path where to write a report in "spoolfile" format (aux copy).
   */
  inline void setPath2ReportOutput(const QString&);

  /**Sets up a path where to save observations in NGS format.
   */
  inline void setPath2NgsOutput(const QString&);

  /**Sets up a path where save plotter's files.
   */
  inline void setPath2PlotterOutput(const QString&);

  /**Sets up a path where the itermediate results will be saved.
   */
  inline void setPath2IntermediateResults(const QString&);
  
  inline void setAutoSavingMode(AutoSaving);

  /**Sets up a path to auxiliary logs.
   */
  inline void setPath2AuxLogs(const QString&);

  /**Sets up an option to save an auxiliary log.
   */
  inline void setHave2SavePerSessionLog(bool);

  /**Sets up necessity to update the catalog in the export process.
   */
  inline void setHave2UpdateCatalog(bool);

  /**Sets up modification of the session code in the export process (for test purposes).
   */
  inline void setHave2MaskSessionCode(bool);
  
  /**Sets up modification of copying "spool file" report into "report" directory.
   */
  inline void setHave2KeepSpoolFileReports(bool);
  
  inline void setHave2KeepUnusedObsList(bool have) {have2KeepUnusedObsList_ = have;};

  /**Sets up LoadImmatureSession flag.
   */
  inline void setHave2LoadImmatureSession(bool);

  /**Sets up identities of user, analysis center, computer, etc.
   */
  inline void setIdentities(const SgIdentities&);

  inline void setHave2WarnCloseWindow(bool);

  inline void setIsShortScreen(bool);
  
  inline void setMainWinWidth(int);

  inline void setMainWinHeight(int);

  inline void setMainWinPosX(int);

  inline void setMainWinPosY(int);

  inline void setSeWinWidth(int);
  
  inline void setSeWinHeight(int);

  inline void setHave2AutoloadAllBands(bool);

  inline void setIsBandPlotPerSrcView(bool);

  inline void setUseOwnOnlyDatabase(bool);
  
  inline void setPlotterOutputFormat(SgPlot::OutputFormat);

  inline void setLnfsFileName(const QString&);
  
  inline void setLnfsOriginType(SgVlbiSessionInfo::OriginType);
  
  inline void setLnfsIsThroughCatalog(bool);
  
  inline void setHave2SkipAutomaticProcessing(bool);
  
  inline void setHave2ForceAutomaticProcessing(bool);

  inline void setExecExternalCommand(bool b) {execExternalCommand_ = b;};

  inline void setExternalCommand(const QString& command) {externalCommand_ = command;};


  inline const QString& catnuGetDbInfo() const;

  inline const QString& catnuSetDbInfo() const;


  inline void setMasterfileSuffixes(const QStringList& lst) {masterfileSuffixes_ = lst;};
  inline void setUseAltMasterfileSuffixes(bool use) {useAltMasterfileSuffixes_ = use;};



  //
  // Functions:
  //
  inline QString className() const {return "NsSetup";};
  
  void setUpBinaryIdentities(const QString&);

  inline QString path2(const QString&) const;

  //
  // Friends:
  //



  //
  // I/O:
  //
  //


private:
  const QString                 catnuGetDbInfo_;
  const QString                 catnuSetDbInfo_;
  // pathes:
  QString                       path2Home_;
  QString                       path2CatNuInterfaceExec_;
  QString                       path2DbhFiles_;
  QString                       path2VgosDbFiles_;
  QString                       path2VgosDaFiles_;
  QString                       path2APrioriFiles_;
  QString                       path2TrpFiles_;
  QString                       path2MasterFiles_;
  QString                       path2SpoolFileOutput_;
  QString                       path2NotUsedObsFileOutput_;
  QString                       path2ReportOutput_;
  QString                       path2NgsOutput_;
  QString                       path2PlotterOutput_;
  QString                       path2IntermediateResults_;
  AutoSaving                    autoSavingMode_;
  // aux log:
  QString                       path2AuxLogs_;
  bool                          have2SavePerSessionLog_;

  bool                          have2UpdateCatalog_;
  bool                          have2MaskSessionCode_;
  bool                          have2KeepSpoolFileReports_;
  bool                          have2KeepUnusedObsList_;
  // transients:
  bool                          have2LoadImmatureSession_;
  
  // look & feel:
  bool                          have2WarnCloseWindow_;
  bool                          isShortScreen_;
  int                           mainWinWidth_;
  int                           mainWinHeight_;
  int                           mainWinPosX_;
  int                           mainWinPosY_;
  int                           seWinWidth_;
  int                           seWinHeight_;
  // options:
  bool                          have2AutoloadAllBands_;
  SgPlot::OutputFormat          plotterOutputFormat_;
  bool                          isBandPlotPerSrcView_;
  bool                          useOwnOnlyDatabase_;

  // identities:
  SgIdentities                  identities_;

  // the last non-finished session:
  QString                       lnfsFileName_;
  SgVlbiSessionInfo::OriginType lnfsOriginType_;
  bool                          lnfsIsThroughCatalog_;
  // post save action:
  bool                          execExternalCommand_;
  QString                       externalCommand_;

  // temporaries (do not need to save it):
  bool                          have2SkipAutomaticProcessing_;
  bool                          have2ForceAutomaticProcessing_;

  //
  QStringList                   masterfileSuffixes_;
  bool                          useAltMasterfileSuffixes_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* NsSetup inline members:                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// INTERFACES:
//
//
inline const QString& NsSetup::getPath2Home() const
{
  return path2Home_;
};



//
inline const QString& NsSetup::getPath2CatNuInterfaceExec() const
{
  return path2CatNuInterfaceExec_;
};



//
inline const QString& NsSetup::getPath2DbhFiles() const
{
  return path2DbhFiles_;
};



//
inline const QString& NsSetup::getPath2VgosDbFiles() const
{
  return path2VgosDbFiles_;
};



//
inline const QString& NsSetup::getPath2APrioriFiles() const
{
  return path2APrioriFiles_;
};



//
inline const QString& NsSetup::getPath2MasterFiles() const
{
  return path2MasterFiles_;
};



//
inline const QString& NsSetup::getPath2SpoolFileOutput() const
{
  return path2SpoolFileOutput_;
};



//
inline const QString& NsSetup::getPath2NotUsedObsFileOutput() const
{
  return path2NotUsedObsFileOutput_;
};



//
inline const QString& NsSetup::getPath2ReportOutput() const
{
  return path2ReportOutput_;
};



//
inline const QString& NsSetup::getPath2NgsOutput() const
{
  return path2NgsOutput_;
};



//
inline const QString& NsSetup::getPath2PlotterOutput() const
{
  return path2PlotterOutput_;
};



//
inline const QString& NsSetup::getPath2IntermediateResults() const
{
  return path2IntermediateResults_;
};



//
inline NsSetup::AutoSaving NsSetup::getAutoSavingMode() const
{
  return autoSavingMode_;
};



//
inline const QString& NsSetup::getPath2AuxLogs() const
{
  return path2AuxLogs_;
};



//
inline bool NsSetup::getHave2SavePerSessionLog() const
{
  return have2SavePerSessionLog_;
};



//
inline bool NsSetup::getHave2UpdateCatalog() const
{
  return have2UpdateCatalog_;
};



//
inline bool NsSetup::getHave2MaskSessionCode() const
{
  return have2MaskSessionCode_;
};



//
inline bool NsSetup::getHave2KeepSpoolFileReports() const
{
  return have2KeepSpoolFileReports_;
};



//
inline bool NsSetup::getHave2LoadImmatureSession() const
{
  return have2LoadImmatureSession_;
};



//
inline const SgIdentities& NsSetup::getIdentities() const
{
  return identities_;
};



//
inline SgIdentities& NsSetup::identities()
{
  return identities_;
};



//
inline bool NsSetup::getHave2WarnCloseWindow() const
{
  return have2WarnCloseWindow_;
};



//
inline bool NsSetup::getIsShortScreen() const
{
  return isShortScreen_;
};



//
inline int NsSetup::getMainWinWidth() const
{
  return mainWinWidth_;
};



//
inline int NsSetup::getMainWinHeight() const
{
  return mainWinHeight_;
};



//
inline int NsSetup::getMainWinPosX() const
{
  return mainWinPosX_;
};



//
inline int NsSetup::getMainWinPosY() const
{
  return mainWinPosY_;
};



//
inline int NsSetup::getSeWinWidth() const
{
  return seWinWidth_;
};



//
inline int NsSetup::getSeWinHeight() const
{
  return seWinHeight_;
};



//
inline bool NsSetup::getHave2AutoloadAllBands() const
{
  return have2AutoloadAllBands_;
};



//
inline bool NsSetup::getIsBandPlotPerSrcView() const
{
  return isBandPlotPerSrcView_;
};



//
inline bool NsSetup::getUseOwnOnlyDatabase() const
{
  return useOwnOnlyDatabase_;;
};



//
inline SgPlot::OutputFormat NsSetup::getPlotterOutputFormat() const
{
  return plotterOutputFormat_==SgPlot::OF_PS?SgPlot::OF_PDF:plotterOutputFormat_;
};



//  
inline const QString& NsSetup::getLnfsFileName() const
{
  return lnfsFileName_;
};



//
inline SgVlbiSessionInfo::OriginType NsSetup::getLnfsOriginType() const
{
  return lnfsOriginType_;
};



//
inline bool NsSetup::getLnfsIsThroughCatalog() const
{
  return lnfsIsThroughCatalog_;
};



//
inline bool NsSetup::getHave2SkipAutomaticProcessing() const
{
  return have2SkipAutomaticProcessing_;
};



//
inline bool NsSetup::getHave2ForceAutomaticProcessing() const
{
  return have2ForceAutomaticProcessing_;
};



//
// sets:
//
inline void NsSetup::setPath2Home(const QString& str)
{
  path2Home_ = str;
};



//
inline void NsSetup::setPath2CatNuInterfaceExec(const QString& str)
{
  path2CatNuInterfaceExec_ = str;
};



//
inline void NsSetup::setPath2DbhFiles(const QString& str)
{
  path2DbhFiles_ = str;
};



//
inline void NsSetup::setPath2VgosDbFiles(const QString& str)
{
  path2VgosDbFiles_ = str;
};



//
inline void NsSetup::setPath2APrioriFiles(const QString& str)
{
  path2APrioriFiles_ = str;
};



//
inline void NsSetup::setPath2MasterFiles(const QString& str)
{
  path2MasterFiles_ = str;
};



//
inline void NsSetup::setPath2SpoolFileOutput(const QString& str)
{
  path2SpoolFileOutput_ = str;
};



//
inline void NsSetup::setPath2NotUsedObsFileOutput(const QString& str)
{
  path2NotUsedObsFileOutput_ = str;
};



//
inline void NsSetup::setPath2ReportOutput(const QString& str)
{
  path2ReportOutput_ = str;
};



//
inline void NsSetup::setPath2NgsOutput(const QString& str)
{
  path2NgsOutput_ = str;
};



//
inline void NsSetup::setPath2PlotterOutput(const QString& str)
{
  path2PlotterOutput_ = str;
};



//
inline void NsSetup::setPath2IntermediateResults(const QString& str)
{
  path2IntermediateResults_ = str;
};



//
inline void NsSetup::setAutoSavingMode(NsSetup::AutoSaving mode)
{
  autoSavingMode_ = mode;
};



//
inline void NsSetup::setPath2AuxLogs(const QString& str)
{
  path2AuxLogs_ = str;
};



//
inline void NsSetup::setHave2SavePerSessionLog(bool have2)
{
  have2SavePerSessionLog_ = have2;
};



//
inline void NsSetup::setHave2UpdateCatalog(bool have2)
{
  have2UpdateCatalog_ = have2;
};



//
inline void NsSetup::setHave2MaskSessionCode(bool have2)
{
  have2MaskSessionCode_ = have2;
};



//
inline void NsSetup::setHave2KeepSpoolFileReports(bool have2)
{
  have2KeepSpoolFileReports_ = have2;
};



//
inline void NsSetup::setHave2LoadImmatureSession(bool have2)
{
  have2LoadImmatureSession_ = have2;
};



//
inline void NsSetup::setIdentities(const SgIdentities& id)
{
  identities_ = id;
};



//
inline void NsSetup::setHave2WarnCloseWindow(bool have2)
{
  have2WarnCloseWindow_ = have2;
};



//
inline void NsSetup::setIsShortScreen(bool is)
{
  isShortScreen_ = is;
};



//
inline void NsSetup::setMainWinWidth(int w)
{
  mainWinWidth_ = w;
};



//
inline void NsSetup::setMainWinHeight(int h)
{
  mainWinHeight_ = h;
};



//
inline void NsSetup::setMainWinPosX(int x)
{
  mainWinPosX_ = x;
};



//
inline void NsSetup::setMainWinPosY(int y)
{
  mainWinPosY_ = y;
};



//
inline void NsSetup::setSeWinWidth(int w)
{
  seWinWidth_ = w;
};



//
inline void NsSetup::setSeWinHeight(int h)
{
  seWinHeight_ = h;
};



//
inline void NsSetup::setHave2AutoloadAllBands(bool have2)
{
  have2AutoloadAllBands_ = have2;
};



//
inline void NsSetup::setIsBandPlotPerSrcView(bool is)
{
  isBandPlotPerSrcView_ = is;
};



//
inline void NsSetup::setUseOwnOnlyDatabase(bool use)
{
  useOwnOnlyDatabase_ = use;
};



//  
inline void NsSetup::setPlotterOutputFormat(SgPlot::OutputFormat fmt)
{
  plotterOutputFormat_ = (fmt==SgPlot::OF_PS?SgPlot::OF_PDF:fmt);
};



//
inline void NsSetup::setLnfsFileName(const QString& str)
{
  lnfsFileName_ = str;
};



//
inline void NsSetup::setLnfsOriginType(SgVlbiSessionInfo::OriginType o)
{
  lnfsOriginType_ = o;
};



//
inline void NsSetup::setLnfsIsThroughCatalog(bool is)
{
  lnfsIsThroughCatalog_ = is;
};



//
inline void NsSetup::setHave2SkipAutomaticProcessing(bool is)
{
  have2SkipAutomaticProcessing_ = is;
};



//
inline void NsSetup::setHave2ForceAutomaticProcessing(bool is)
{
  have2ForceAutomaticProcessing_ = is;
};



//
inline const QString& NsSetup::catnuGetDbInfo() const
{
  return catnuGetDbInfo_;
};



//
inline const QString& NsSetup::catnuSetDbInfo() const
{
  return catnuSetDbInfo_;
};



//
inline QString NsSetup::path2(const QString& path) const
{
  return 
    path.size()==0 ? path2Home_ :
      (path.at(0)==QChar('/') ? path : path2Home_ + "/" + path);
};






/*=====================================================================================================*/
#endif // NS_SETUP_H
