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

#ifndef NS_SESSION_EDIT_DIALOG_H
#define NS_SESSION_EDIT_DIALOG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QAbstractItemModel>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QTreeWidget>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QDialog>
#   include <QtGui/QLabel>
#   include <QtGui/QTreeWidget>
#   include <QtGui/QTreeWidgetItem>
#endif




#include <SgGuiPlotter.h>
#include <SgGuiVlbiStationList.h>
#include <SgGuiVlbiBaselineList.h>
#include <SgGuiVlbiSourceList.h>
#include <SgGuiVlbiHistory.h>

#include <SgVlbiSession.h>
#include <SgTaskConfig.h>
#include <SgParametersDescriptor.h>


#include "NsSessionHandler.h"



class QPushButton;

class SgDbhImage;
class SgGuiTaskConfig;
class SgVgosDb;
class SgSolutionReporter;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsSessionEditDialog : public QDialog, public NsSessionHandler
{
Q_OBJECT
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsSessionEditDialog(const QString& fileName, const QString& fileNameAux, 
    SgTaskConfig*, SgParametersDescriptor*, SgVlbiSessionInfo::OriginType, bool isLnfs,
    QWidget *parent=0, Qt::WindowFlags flags=Qt::Dialog);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsSessionEditDialog();


  //
  // Interfaces:
  //


  //
  // Functions:
  //
  inline QString className() const {return "NsSessionEditDialog";};

 
  //
  // Friends:
  //


  //
  // I/O:
  //
  //

signals:
  void dataChanged();
  void activeBandChanged(int);
  void delayTypeChanged(int);


private slots:
  void accept();
  void reject();

  void dispatchUserKey(SgPlot*, Qt::KeyboardModifiers, int);
  void dispatchUserKeyOld(SgPlot*, Qt::KeyboardModifiers, int);
  void displayPointInfo(SgPlot*, SgPlotBranch*, int, int, int, SgPlot::QueryMode); 
//void displayObservableInfo(SgPlot*, SgPlotBranch*, int, int, int);
  void dispatchChangeOfYAxis(int);
//void dispatchChangeOfYAxisExt(SgTaskConfig::VlbiObservables);
  void dispatchChangeOfYAxis(SgTaskConfig::VlbiDelayType);
  void dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode);
  void dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode);
  //
  void process();
  void process_2times();
  void process_3times();
  void process_4macro();
  //
  void calcIono();
  void zeroIono();
  void calcClcF1();
  void saveData();
  void detectAndProcessClockBreaks();
  void processRmOutButton();
  void changeActiveBand(int idx);
  void scanAmbiguityMultipliers();
  void resetAmbiguityMultipliers();
  void changePrimaryBand(int);
  void clearAuxSigmas();
  void clearAllEditings();
  void generateAposterioriFiles();
  void generateReport();
  void generateReport4StcPars();
  void generateReport4Tzds();
  void generateReport_Extended();
  void exportDataToNgs();
  void putDataToAgv();
  void makeMyAction();
  void executeTestAction();
  void netIdCurrentIndexChanged(const QString&);
  void displayExcludedObs();
  void displayExcludedObs_old();
  void saveIntermediateResults();

private:
  SgGuiTaskConfig              *configWidget_;
  // plotting:
  QList<SgPlotCarrier*>         plotCarriers4Baselines_;
  QList<SgPlot*>                plots4Baselines_;
  SgPlotCarrier                *plotCarrier4Stations_;
  SgPlot                       *plot4Stations_;
  int                           cableCalsOffset_;
  SgPlotCarrier                *plotCarrier4Session_;
  SgPlot                       *plot4Session_;
 
  SgGuiVlbiStationList         *stationsList_;
  SgGuiVlbiBaselineList        *baselinesList_;
  SgGuiVlbiSourceList          *sourcesList_;
  
  double                        scale4Delay_;
  double                        scale4Rate_;
  // aux:
  bool                          isImpossibleUpdateCat_;
  // for "beautification":
  bool                          processHasBeenCalled_;
  QList<QLabel*>                labelsPrc_;
  QList<QLabel*>                labelsPar_;
  QList<QLabel*>                labelsWRMS_;
  QList<QLabel*>                labelsSig0_;
  QList<QLabel*>                labelsChi2_;
  //
  QAction                      *generateReportAct_;
  // 
  bool                          isWidgetsBlocked_;
  QList<QWidget*>               widgets2Block_;
  //
  QPushButton                  *saveButton_;
  QPushButton                  *ionoC_;
  QPushButton                  *iono0_;
  QPushButton                  *scanAmbigs_;
  QPushButton                  *resetAmbigs_;
  bool                          isNeed2SaveSir_;
  // saved:
  bool                          h2aPxContrib_;
  bool                          h2aPyContrib_;
  bool                          h2aEarthTideContrib_;
  bool                          h2aOceanTideContrib_;
  bool                          h2aPoleTideContrib_;
  bool                          h2aUt1OceanTideHFContrib_; 
  bool                          h2aPxyOceanTideHFContrib_;
  bool                          h2aNutationHFContrib_;
  bool                          h2aUt1LibrationContrib_;
  bool                          h2aPxyLibrationContrib_;
  bool                          h2aOceanPoleTideContrib_;
  bool                          h2aFeedCorrContrib_;
  bool                          h2aTiltRemvrContrib_;
  bool                          h2aOldOceanTideContrib_;
  bool                          h2aOldPoleTideContrib_;
  
  // functions:
  QWidget*                      tab4GeneralInfo();
  QWidget*                      tab4Options();
  QWidget*                      tab4BaselinesPlot();
  QWidget*                      tab4StationsPlot();
  QWidget*                      tab4SessionPlot();
  QWidget*                      tab4StationsInfo();
  QWidget*                      tab4BaselinesInfo();
  QWidget*                      tab4SourcesInfo();
  QWidget*                      tab4Band(SgVlbiBand*);
  QWidget*                      tab4BandPlots(SgVlbiBand*);
  bool                          exportDataIntoDbh();
  bool                          exportDataIntoVgosDb();
  void                          blockInput();
  void                          restoreInput();
  bool                          loadIntermediateResults(bool);
  void                          runExternalCommand();
  //
  
protected:
  virtual void                  closeEvent(QCloseEvent *) {deleteLater();};
  void                          updateResiduals();
  void                          updateSessionWideSolutions();
  void                          excludeMarkedPoints(SgPlot*);
  void                          includeMarkedPoints(SgPlot*);
  void                          toggleMarkedPoints(SgPlot*);

  void                          unimmunizeMarkedPoints(SgPlot*);
  void                          immunizeMarkedPoints(SgPlot*);
  void                          excludeAndImmunizeMarkedPoints(SgPlot*);
  void                          includeAndUnimmunizeMarkedPoints(SgPlot*);
  
  void                          generateRequest4Refringing(SgPlot*);
  void                          changeNumOfAmbigSpacing4MarkedPoints(SgPlot*, int);
  void                          changeNumOfSubAmbig4MarkedPoints(SgPlot*, int);
  bool                          findClockBreakEvent(SgPlot*, QList<SgVlbiBaselineInfo*>&, SgVlbiBand*&,
                                  SgMJD&, SgVlbiStationInfo*&);
  void                          processClockBreakMarks(SgPlot*);
  void                          processClockBreakMarksNew(SgPlot*);

  void                          processTestFunctionN1(SgPlot*);
  void                          adjustAmbiguityMultipliers();
  virtual bool                  canSave();
  void                          postSave();
  void                          displayPointInfo_old(SgPlotBranch* branch, int idx);
  void                          displayPointInfo_new(SgPlotBranch* branch, int idx);
};
/*=====================================================================================================*/


/*=====================================================================================================*/
#endif // NS_SESSION_EDIT_DIALOG_H
