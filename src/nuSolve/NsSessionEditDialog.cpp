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

#include "NsSessionEditDialog.h"
#include "NsMainWindow.h"
#include "nuSolve.h"
#include "NsBrowseNotUsedObsDialog.h"
#include "NsBrowseObservation.h"

#include <iostream>
#include <stdlib.h>




//#define DEMO 1
//#undef DEMO


#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>



#if QT_VERSION >= 0x050000
#    include <QtWidgets/QApplication>
#    include <QtWidgets/QBoxLayout>
#    include <QtWidgets/QButtonGroup>
#    include <QtWidgets/QCheckBox>
#    include <QtWidgets/QComboBox>
#    include <QtWidgets/QFrame>
#    include <QtWidgets/QGroupBox>
#    include <QtWidgets/QHeaderView>
#    include <QtWidgets/QLabel>
#    include <QtWidgets/QMessageBox>
#    include <QtWidgets/QPushButton>
#    include <QtWidgets/QRadioButton>
#    include <QtWidgets/QShortcut>
#    include <QtWidgets/QTabWidget>
#else
#    include <QtGui/QApplication>
#    include <QtGui/QBoxLayout>
#    include <QtGui/QButtonGroup>
#    include <QtGui/QCheckBox>
#    include <QtGui/QComboBox>
#    include <QtGui/QFrame>
#    include <QtGui/QGroupBox>
#    include <QtGui/QHeaderView>
#    include <QtGui/QLabel>
#    include <QtGui/QMessageBox>
#    include <QtGui/QPushButton>
#    include <QtGui/QRadioButton>
#    include <QtGui/QShortcut>
#    include <QtGui/QTabWidget>
#endif


#include <QtGui/QIcon>


#include <SgArcStorage.h>
#include <SgConstants.h>
#include <SgDbhImage.h>
#include <SgGuiPiaReport.h>
#include <SgGuiPlotter.h>
#include <SgGuiTaskConfig.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgParameterCfg.h>
#include <SgPwlStorage.h>
#include <SgPwlStorageBSplineL.h>
#include <SgPwlStorageBSplineQ.h>
#include <SgPwlStorageIncRates.h>
#include <SgSingleSessionTaskManager.h>
#include <SgSolutionReporter.h>
#include <SgTaskManager.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>
#include <SgVlbiObservation.h>





enum BaselinePlotIndex
{
  BPI_EPOCH           =  0,
  BPI_GR_DELAY        =  1,
  BPI_PH_RATE         =  2,
  BPI_RES_NORM_RAT    =  3,
  BPI_RES_PHR         =  4,
  BPI_RES_SBD         =  5,
  BPI_RES_GRD         =  6,
  BPI_RES_NORM_DEL    =  7,
  BPI_RES_PHD         =  8,
  BPI_ION_SBD         =  9,
  BPI_ION_GRD         = 10,
  BPI_ION_PHD         = 11,
  BPI_ION_PHR         = 12,
  BPI_ION_DTEC        = 13,
  BPI_CORR_COEF       = 14,
  BPI_SNR             = 15,
  BPI_SIG_SBD         = 16,
  BPI_SIG_GRD         = 17,
  BPI_SIG_DEL_APPLIED = 18,
  BPI_SIG_PHD         = 19,
  BPI_SIG_PHR         = 20,
  BPI_SIG_RAT_APPLIED = 21,
  BPI_SIG_ION_SBD     = 22,
  BPI_SIG_ION_GRD     = 23,
  BPI_SIG_ION_PHD     = 24,
  BPI_SIG_ION_PHR     = 25,
  BPI_SIG_ION_DTEC    = 26,
  BPI_AMBIG_GR_SPACING= 27,
  BPI_AMBIG_GR_NUMBER = 28,
  BPI_AMBIG_PH_SPACING= 29,
  BPI_AMBIG_PH_NUMBER = 30,

  // test purposes:
  BPI_EFF_FRQ_GRD     = 31,
  BPI_EFF_FRQ_PHD     = 32,
  BPI_EFF_FRQ_PHR     = 33,
  //
  BPI_QF              = 34,
  BPI_FEC             = 35, // fringe error code
  // phasecals:
  BPI_PCAL_GR_DEL_1   = 36,
  BPI_PCAL_PH_DEL_1   = 37,
  BPI_PCAL_GR_DEL_2   = 38,
  BPI_PCAL_PH_DEL_2   = 39,
  BPI_U               = 40,
  BPI_V               = 41,
  BPI_EFF_INTGR_TIME  = 42,
  BPI_TAUS            = 43,
  BPI_TEST            = 44,
  BPI_TEST2           = 45,
  //
};



enum StationPlotIndex
{
  SPI_EPOCH           =  0,
  SPI_CABLE_CAL       =  1,
  SPI_MET_TEMP        =  2,
  SPI_MET_PRESS       =  3,
  SPI_MET_HUMID       =  4,
  SPI_AZIMUTH         =  5,
  SPI_ELEVATION       =  6,
  SPI_PARALLACTIC     =  7,
  SPI_EST_CLK         =  8,
  SPI_EST_WZD         =  9,
  SPI_EST_GRN         = 10,
  SPI_EST_GRE         = 11,
  SPI_TOT_ZDEL        = 12,
  SPI_SIG_EST_CLK     = 13,
  SPI_SIG_EST_WZD     = 14,
  SPI_SIG_EST_GRN     = 15,
  SPI_SIG_EST_GRE     = 16,
  SPI_SIG_TOT_ZDEL    = 17,
  // test purposes:
  SPI_RES_GRD         = 18,
  SPI_SIG_GRD         = 19,
};



enum SessionPlotIndex
{
  SNI_EPOCH           =  0,
  SNI_EST_UT1         =  1,
  SNI_EST_PMX         =  2,
  SNI_EST_PMY         =  3,
  SNI_SIG_UT1         =  4,
  SNI_SIG_PMX         =  5,
  SNI_SIG_PMY         =  6,
};



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsSessionEditDialog::NsSessionEditDialog(const QString& fileName, const QString& fileNameAux, 
  SgTaskConfig* config, SgParametersDescriptor* parametersDescriptor, 
  SgVlbiSessionInfo::OriginType oType, bool isLnfs, QWidget *parent, Qt::WindowFlags flags)
  : QDialog(parent, flags),
    NsSessionHandler(fileName, fileNameAux, config, parametersDescriptor, oType),
    plotCarriers4Baselines_(),
    plots4Baselines_(),
    labelsPrc_(),
    labelsPar_(),
    labelsWRMS_(),
    labelsSig0_(),
    labelsChi2_()
{
  stationsList_ = NULL;
  baselinesList_ = NULL;
  sourcesList_ = NULL;
  generateReportAct_ = NULL;
  plotCarrier4Stations_ = NULL;
  plotCarrier4Session_  = NULL;
  plot4Stations_        = NULL;
  plot4Session_         = NULL;
  
  cableCalsOffset_ = 0;

  scale4Delay_ = 1.0e9;  // this is ns
  scale4Rate_  = 1.0e15; // fs
  //  scale4Delay_ = vLight*100.0; // this is cm
  processHasBeenCalled_ = false;
  isImpossibleUpdateCat_ = false;
  isNeed2SaveSir_ = true;
  //
  // save some info:
  h2aPxContrib_             = config_->getHave2ApplyPxContrib();
  h2aPyContrib_             = config_->getHave2ApplyPyContrib();
  h2aEarthTideContrib_      = config_->getHave2ApplyEarthTideContrib();
  h2aOceanTideContrib_      = config_->getHave2ApplyOceanTideContrib();
  h2aPoleTideContrib_       = config_->getHave2ApplyPoleTideContrib();
  h2aUt1OceanTideHFContrib_ = config_->getHave2ApplyUt1OceanTideHFContrib();
  h2aPxyOceanTideHFContrib_ = config_->getHave2ApplyPxyOceanTideHFContrib();
  h2aNutationHFContrib_     = config_->getHave2ApplyNutationHFContrib();
  h2aUt1LibrationContrib_   = config_->getHave2ApplyUt1LibrationContrib();
  h2aPxyLibrationContrib_   = config_->getHave2ApplyPxyLibrationContrib();
  h2aOceanPoleTideContrib_  = config_->getHave2ApplyOceanPoleTideContrib();
  h2aFeedCorrContrib_       = config_->getHave2ApplyFeedCorrContrib();
  h2aTiltRemvrContrib_      = config_->getHave2ApplyTiltRemvrContrib();
  h2aOldOceanTideContrib_   = config_->getHave2ApplyOldOceanTideContrib();
  h2aOldPoleTideContrib_    = config_->getHave2ApplyOldPoleTideContrib();
  
  // add GUI:
  session_->setLongOperationStart(&longOperationStartDisplay);
  session_->setLongOperationProgress(&longOperationProgressDisplay);
  session_->setLongOperationStop(&longOperationStopDisplay);
  session_->setLongOperationMessage(&longOperationMessageDisplay);
  session_->setLongOperationShowStats(&longOperationShowStats);
  session_->setHave2InteractWithGui(true);

  blockInput();
  if (!importSession(true))
  {
    restoreInput();
    deleteLater();
    return;
  };
  restoreInput();
  // warn a user:
  if (saveStatus_ == SS_FILE_EXISTS)
  {
    QMessageBox::warning(this, "Warning", "A database with the next version "
      "already exists in the catalog.\nThe 'Save' procedure will not work if you do not remove it "
      "from the catalog first.");
    saveStatus_ = SS_UNDEF;
  };
  // if nothing to load:
  if (loadIntermediateResults(!isLnfs))
    processHasBeenCalled_ = true;
  else if (oType_ == SgVlbiSessionInfo::OT_AGV)
  {
    logger->write(SgLogger::INF, SgLogger::SESSION, className() + 
      ": the automatic analysis is disabled for vgosDa input type");
  }
  else if (setup.getHave2SkipAutomaticProcessing())
  {
    logger->write(SgLogger::WRN, SgLogger::SESSION, className() + 
      ": the automatic analysis is disabled by the command line argument");
    session_->pickupReferenceCoordinatesStation();
  }
  // unprocessed or foreign session:
  else if (setup.getHave2ForceAutomaticProcessing() || session_->need2runAutomaticDataProcessing())
  {
    if (session_->doPostReadActions() && session_->isAttr(SgVlbiSessionInfo::Attr_FF_AUTOPROCESSED))
      generateReport();
    processHasBeenCalled_ = true;
    if (config_->apByNetId().contains(session_->getNetworkID())       &&
        config_->apByNetId()[session_->getNetworkID()].doOutliers_    &&
        config_->apByNetId()[session_->getNetworkID()].doWeights_      )
    (new SgGuiPiaReport(config_, parametersDescriptor_, session_, this, Qt::Dialog))->show();
  }
  else
  {
    // the reference clock station(s) should be saved in a database
    // have to set up a coordinate reference station and adjust clock's polynomial model order:
    session_->pickupReferenceCoordinatesStation();
    session_->setClockModelOrder4Stations(3);
  };
  //
  //
  //                           set up the GUI stuff:
  //
  //
  if (oType_==SgVlbiSessionInfo::OT_DBH)
  {
    setWindowTitle(session_->getName() + " Session Editor " + 
      (setup.getHave2UpdateCatalog()?"(work through catalog":"(standalone mode") + " as " +
      setup.identities().getUserDefaultInitials() + ")");
  }
  else if (oType_==SgVlbiSessionInfo::OT_VDB)
  {
    setWindowTitle("Session Editor:  " + session_->getName() + "  " + 
      QString("").sprintf("V%03d", vgosDb_->getCurrentVersion()) + "  " +
      session_->getCorrelatorName() + "|" +
      session_->getSubmitterName() + 
      " (as " + setup.identities().getUserDefaultInitials() + ")");
  }
  else if (oType_==SgVlbiSessionInfo::OT_AGV)
  {
    setWindowTitle(session_->getName() + " Session Editor " + 
      (setup.getHave2UpdateCatalog()?"(work through catalog":"(standalone mode") + " as " +
      setup.identities().getUserDefaultInitials() + ")");
  }
  else
  {
    setWindowTitle(session_->getName() + " Session Editor " + 
      (setup.getHave2UpdateCatalog()?"(work through catalog":"(standalone mode") + " as " +
      setup.identities().getUserDefaultInitials() + ")");
  };


  if (0<setup.getSeWinWidth() && 0<setup.getSeWinHeight())
    resize(setup.getSeWinWidth(), setup.getSeWinHeight());

  QBoxLayout                   *layout, *subLayout;
  QSize                         btnSize;
  if (setup.getIsShortScreen())
    layout = new QHBoxLayout(this);
  else
    layout = new QVBoxLayout(this);
  QTabWidget                   *sessionTabs=new QTabWidget(this);

  sessionTabs->addTab(tab4GeneralInfo(),   "&General Info");
  sessionTabs->addTab(tab4Options(),       "&Options");
  sessionTabs->addTab(tab4BaselinesPlot(), "&Bands");
  sessionTabs->addTab(tab4StationsInfo(),  "S&tations (List)");
  sessionTabs->addTab(tab4SourcesInfo(),   "Sou&rces (List)");
  sessionTabs->addTab(tab4BaselinesInfo(), "Base&lines (List)");
  sessionTabs->addTab(tab4StationsPlot(),  "&Stations (Plots)");
  sessionTabs->addTab(tab4SessionPlot(),   "S&ession (Plots)");
  //
  dispatchChangeOfYAxis(config_->getUseDelayType());
  //
  layout->addWidget(sessionTabs, 10);
                                    // "too big, too small
  if (setup.getIsShortScreen())     //  size does matter after all."
    subLayout = new QVBoxLayout();
  else
    subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  
  
  QPushButton                   *ok, *process, *dapCB, *rmOut, *save, *s2add0, *reset;
  ok = new QPushButton("Close", this);
  ionoC_ = new QPushButton("&IonoC", this);
  iono0_ = new QPushButton("Iono&0", this);
  process= new QPushButton("&Process", this);
  dapCB  = new QPushButton("&CBreak", this);
  rmOut  = new QPushButton("O&utlr", this);
  scanAmbigs_  = new QPushButton("&Ambig", this);
  resetAmbigs_ = new QPushButton("Ambig0", this);
  save   = new QPushButton("Sa&ve", this);
  s2add0 = new QPushButton("Au&xSig0", this);
  reset  = new QPushButton("Reset", this);
  saveButton_ = save;
  //  QPushButton *rprt    = new QPushButton("Report", this);
  process->setDefault(true);

  process->setMinimumSize((btnSize=process->sizeHint()));
  ionoC_ ->setMinimumSize( btnSize );
  iono0_ ->setMinimumSize( btnSize );
  //  cancel ->setMinimumSize( btnSize );
  ok     			->setMinimumSize( btnSize );
  dapCB  			->setMinimumSize( btnSize );
  rmOut  			->setMinimumSize( btnSize );
  scanAmbigs_	->setMinimumSize( btnSize );
  resetAmbigs_->setMinimumSize( btnSize );
  save   			->setMinimumSize( btnSize );
  //  rprt   	->setMinimumSize( btnSize );
  s2add0 ->setMinimumSize( btnSize );
  reset  ->setMinimumSize( btnSize );
  if (setup.getIsShortScreen())
    subLayout->addSpacing(btnSize.height());
  subLayout->addWidget(process);
  subLayout->addWidget(ionoC_);
  subLayout->addWidget(iono0_);
  subLayout->addWidget(scanAmbigs_);
  subLayout->addWidget(resetAmbigs_);
  subLayout->addWidget(dapCB);
  subLayout->addWidget(s2add0);
  subLayout->addWidget(reset);
  subLayout->addWidget(rmOut);

  subLayout->addStretch(1);
  //  subLayout->addWidget(rprt);
  subLayout->addWidget(save);
  subLayout->addWidget(ok);
  //  subLayout->addWidget(cancel);
  if (setup.getIsShortScreen())
    subLayout->addSpacing(btnSize.height());
  setSizeGripEnabled(true);

//  stationsList_ ->setWrmsScale(scale4Delay_*1.0E3);
//  sourcesList_  ->setWrmsScale(scale4Delay_*1.0E3);
//  baselinesList_->setWrmsScale(scale4Delay_*1.0E3);

  connect(this, SIGNAL(dataChanged()), stationsList_,  SLOT(updateContent()));
  connect(this, SIGNAL(dataChanged()), sourcesList_,   SLOT(updateContent()));
  connect(this, SIGNAL(dataChanged()), baselinesList_, SLOT(updateContent()));

  connect(stationsList_, SIGNAL(refClockStationSelected  (const QString&)), 
          baselinesList_,SLOT  (addRefClockStation       (const QString&)));
  connect(stationsList_, SIGNAL(refClockStationDeselected(const QString&)), 
          baselinesList_,SLOT  (delRefClockStation       (const QString&)));
  // set up ref clocks if exist:
  QMap<QString, SgVlbiStationInfo*>::const_iterator it=session_->stationsByName().constBegin();
  for (; it!=session_->stationsByName().constEnd(); ++it)
  {
    SgVlbiStationInfo* stInfo = it.value();
    if (stInfo->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      baselinesList_->addRefClockStation(stInfo->getKey());
  };
  //
  connect(process,SIGNAL(clicked()), SLOT(process()));
  connect(ionoC_, SIGNAL(clicked()), SLOT(calcIono()));
  connect(iono0_, SIGNAL(clicked()), SLOT(zeroIono()));
  connect(dapCB,  SIGNAL(clicked()), SLOT(detectAndProcessClockBreaks()));
  connect(rmOut,  SIGNAL(clicked()), SLOT(processRmOutButton()));
  connect(scanAmbigs_,  SIGNAL(clicked()), SLOT(scanAmbiguityMultipliers()));
  connect(resetAmbigs_, SIGNAL(clicked()), SLOT(resetAmbiguityMultipliers()));
  connect(ok,     SIGNAL(clicked()), SLOT(accept()));
  connect(save,   SIGNAL(clicked()), SLOT(saveData()));
  connect(s2add0, SIGNAL(clicked()), SLOT(clearAuxSigmas()));
  connect(reset,  SIGNAL(clicked()), SLOT(clearAllEditings()));

  if (1 < session_->numberOfBands())
  {
    if (session_->isAttr(SgVlbiSessionInfo::Attr_HAS_IONO_CORR))
      ionoC_->setEnabled(false);
    else
      iono0_->setEnabled(false);
  }
  else
  {
    ionoC_->setEnabled(false);
    iono0_->setEnabled(false);
  };
  //
  scanAmbigs_ ->setEnabled(session_->bands().at(activeBand_)->isAttr(SgVlbiBand::Attr_HAS_AMBIGS));
  resetAmbigs_->setEnabled(session_->bands().at(activeBand_)->isAttr(SgVlbiBand::Attr_HAS_AMBIGS));
  //
//    if (isImpossibleUpdateCat_)
//      save->setEnabled(false);
  isWidgetsBlocked_ = false;
  widgets2Block_   << ok << process << ionoC_ << iono0_ << dapCB << rmOut << scanAmbigs_ << resetAmbigs_
									<< save << s2add0;
  //              << cancel << clcF1 << rprt;
  // set actual values:
  setup.setLnfsFileName(fileName);
  setup.setLnfsOriginType(oType);
  setup.setLnfsIsThroughCatalog(setup.getHave2UpdateCatalog());

  QShortcut *shCut;

  // These are still alt's:
  shCut = new QShortcut(QKeySequence(tr("Alt+2")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_2times()));
  shCut = new QShortcut(QKeySequence(tr("Ctrl+2")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_2times()));

  shCut = new QShortcut(QKeySequence(tr("Alt+3")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_3times()));
	shCut = new QShortcut(QKeySequence(tr("Ctrl+3")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_3times()));

  shCut = new QShortcut(QKeySequence(tr("Alt+4")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_4macro()));
  shCut = new QShortcut(QKeySequence(tr("Ctrl+4")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(process_4macro()));

  // 
  shCut = new QShortcut(QKeySequence(tr("Ctrl+a")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(generateAposterioriFiles()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+g")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(putDataToAgv()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+h")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(generateReport4StcPars()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+i")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(displayExcludedObs()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+m")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(makeMyAction()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+n")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(exportDataToNgs()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+r")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(generateReport()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+Shift+r")), this);              //
  connect(shCut, SIGNAL(activated()), this, SLOT(generateReport_Extended())); // mostly for tests

  shCut = new QShortcut(QKeySequence(tr("Ctrl+s")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(saveIntermediateResults()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+t")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(executeTestAction()));

  shCut = new QShortcut(QKeySequence(tr("Ctrl+z")), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(generateReport4Tzds()));
};



//
NsSessionEditDialog::~NsSessionEditDialog()
{
  // restore overwritten configs:
  if (config_->getContribsAreFromDatabase())
  {
    config_->setHave2ApplyPoleTideContrib(h2aPoleTideContrib_);
    config_->setHave2ApplyPxContrib(h2aPxContrib_);
    config_->setHave2ApplyPyContrib(h2aPyContrib_);
    config_->setHave2ApplyEarthTideContrib(h2aEarthTideContrib_);
    config_->setHave2ApplyOceanTideContrib(h2aOceanTideContrib_);
    config_->setHave2ApplyUt1OceanTideHFContrib(h2aUt1OceanTideHFContrib_);
    config_->setHave2ApplyPxyOceanTideHFContrib(h2aPxyOceanTideHFContrib_);
    config_->setHave2ApplyNutationHFContrib(h2aNutationHFContrib_);
    config_->setHave2ApplyFeedCorrContrib(h2aFeedCorrContrib_);
    config_->setHave2ApplyTiltRemvrContrib(h2aTiltRemvrContrib_);
    config_->setHave2ApplyPxyLibrationContrib(h2aPxyLibrationContrib_);
    config_->setHave2ApplyUt1LibrationContrib(h2aUt1LibrationContrib_);
    config_->setHave2ApplyOceanPoleTideContrib(h2aOceanPoleTideContrib_);
    config_->setHave2ApplyOldOceanTideContrib(h2aOldOceanTideContrib_);
    config_->setHave2ApplyOldPoleTideContrib(h2aOldPoleTideContrib_);
  };
  //
  
//  while (!plotCarriers4Baselines_.isEmpty())
//    delete plotCarriers4Baselines_.takeFirst();
  for (int i=0; i<plotCarriers4Baselines_.size(); i++)
    delete plotCarriers4Baselines_.at(i);
  plotCarriers4Baselines_.clear();

  if (plotCarrier4Stations_)
  {
    delete plotCarrier4Stations_;
    plotCarrier4Stations_ = NULL;
  };

  if (plotCarrier4Session_)
  {
    delete plotCarrier4Session_;
    plotCarrier4Session_ = NULL;
  };

  setup.setSeWinWidth(width());
  setup.setSeWinHeight(height());
};




//             non-GUI and NsSessionHandler's reloads:
//
//
//
void NsSessionEditDialog::generateAposterioriFiles()
{
  NsSessionHandler::generateAposterioriFiles();
};



//
void NsSessionEditDialog::generateReport()
{
  NsSessionHandler::generateReport();
};



//
void NsSessionEditDialog::makeMyAction()
{
  NsSessionHandler::generateMyReport();
};



//
void NsSessionEditDialog::generateReport4StcPars()
{
  NsSessionHandler::generateReport4StcPars();
};



//
void NsSessionEditDialog::generateReport4Tzds()
{
  NsSessionHandler::generateReport4Tzds();
};



//
void NsSessionEditDialog::generateReport_Extended()
{
  NsSessionHandler::generateReport(true);
};



//
void NsSessionEditDialog::exportDataToNgs()
{
  blockInput();
  NsSessionHandler::exportDataToNgs();
  restoreInput();
};



//
void NsSessionEditDialog::putDataToAgv()
{
  blockInput();
  NsSessionHandler::exportDataIntoVgosDa();
  restoreInput();
};



//
bool NsSessionEditDialog::exportDataIntoDbh()
{
  bool                           isOk;
  
  blockInput();
  isOk = NsSessionHandler::exportDataIntoDbh();
  if (isOk)
    postSave();
  restoreInput();

  if (!isOk && saveStatus_==SS_FILE_EXISTS)
    QMessageBox::warning(this, "Warning", "The database with the new version "
        "already exists in the catalog.\nRemove it from the catalog and try again.");

  return isOk;
};



//
bool NsSessionEditDialog::exportDataIntoVgosDb()
{
  //
  bool                           isOk;
 
  blockInput();
  isOk = NsSessionHandler::exportDataIntoVgosDb();
  if (isOk)
    postSave();
  restoreInput();

  return isOk;
};



//
void NsSessionEditDialog::saveData()
{
  if (config_->getIsNoviceUser() &&
      QMessageBox::warning(this, "Novice User Warning", "Do you want to save data?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
    return;

  bool                           isOk(false);
  //
  if (oType_ == SgVlbiSessionInfo::OT_DBH)
    isOk = exportDataIntoDbh();
  else if (oType_ == SgVlbiSessionInfo::OT_VDB)
    isOk = exportDataIntoVgosDb();
  else if (oType_ == SgVlbiSessionInfo::OT_AGV) // export data in vgosDb format:
    isOk = exportDataIntoVgosDb();
  else
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::saveData(): unrecognized type of data");
    return;
  };
  //
  if (isOk &&
      oType_ != SgVlbiSessionInfo::OT_AGV)
  {
    generateReport();
    if (setup.getExecExternalCommand())
      runExternalCommand();
  };
};



//
bool NsSessionEditDialog::canSave()
{
  if (saveStatus_ == SS_OK)  // was already saved:
  {
    if (QMessageBox::warning(this, "Warning",
        "This session already has been saved. "
        "Do you want to save the session as a new version?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return false;
  };
  //
  if (!config_->getIsNoviceUser())
    return true;
  //
  // help to new user:
//  if (!session_->isAttr(SgVlbiSessionInfo::Attr_FF_ION_C_CALCULATED))
  if (!session_->isAttr(SgVlbiSessionInfo::Attr_HAS_IONO_CORR))
  {
    if (QMessageBox::warning(this, "Novice User Warning", 
        "No ionosphere corrections evaluated for the session. "
        "Do you want to save the session anyway?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return false;
  };
  if (!session_->isAttr(SgVlbiSessionInfo::Attr_FF_WEIGHTS_CORRECTED))
  {
    if (QMessageBox::warning(this, "Novice User Warning",
        "No additional weights were calculated for the session. "
        "Do you want to save the session anyway?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return false;
  };
  return true;
};



//
void NsSessionEditDialog::postSave()
{
  isNeed2SaveSir_ = false;
  // remove the file with intermediate results:
  QString                       dirName=setup.path2(setup.getPath2IntermediateResults());
  QDir                          d(dirName);
  if (!d.exists())
    return;
  if (dirName.size()>0)
    dirName += "/";
  QFile                         file(dirName + session_->name4SirFile(setup.getHave2UpdateCatalog()));
  if (file.exists())
    file.remove();
  setup.setLnfsFileName("");
  setup.setLnfsOriginType(SgVlbiSessionInfo::OT_UNKNOWN);
  setup.setLnfsIsThroughCatalog(setup.getHave2UpdateCatalog());
};




//             sess edit dialog's and GUI stuff:
//
//
void NsSessionEditDialog::accept()
{
  if (setup.getHave2WarnCloseWindow() &&
      QMessageBox::warning(this, "Warning", "There are unsaved data. Are you sure to close the window?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
    return;

  //
  // save intermediate results:
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()!=NsSetup::AS_NONE)
    saveIntermediateResults();
  //

  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": accept selected");
  deleteLater();
};



//
void NsSessionEditDialog::reject() 
{
  if (setup.getHave2WarnCloseWindow() &&
      QMessageBox::warning(this, "Warning", "There are unsaved data. Are you sure to close the window?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
    return;
  //
  // save intermediate results:
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()!=NsSetup::AS_NONE)
    saveIntermediateResults();
  //
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": reject selected");
  deleteLater();
};



//
void NsSessionEditDialog::blockInput()
{
  if (!isWidgetsBlocked_)
  {
    isWidgetsBlocked_ = true;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    for (int i=0; i<widgets2Block_.size(); i++)
      widgets2Block_.at(i)->blockSignals(isWidgetsBlocked_);
  };
};



//
void NsSessionEditDialog::restoreInput()
{
  if (isWidgetsBlocked_)
  {
    isWidgetsBlocked_ = false;
    QApplication::restoreOverrideCursor();
    for (int i=0; i<widgets2Block_.size(); i++)
      widgets2Block_.at(i)->blockSignals(isWidgetsBlocked_);
  };
};



//
QWidget* NsSessionEditDialog::tab4GeneralInfo()
{
  QString     									str("");
  QWidget     								 *w=new QWidget(this);
  QBoxLayout  								 *layout=new QHBoxLayout(w);
  QLabel       								 *label;
  QGridLayout 								 *grid;
  QGroupBox   							   *gbox;
  QBoxLayout  								 *subLayout;
	int														rowIdx;

  subLayout = new QVBoxLayout;
  layout->addLayout(subLayout);

  // attributes:
  gbox = new QGroupBox("Attributes of the session", w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  label = new QLabel("Type of import file(s):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  switch(session_->getOriginType())
  {
  case SgVlbiSessionInfo::OT_DBH:
    str = "Mk-III database";
    break;
  case SgVlbiSessionInfo::OT_NGS:
    str = "NGS cards";
    break;
  case SgVlbiSessionInfo::OT_VDB:
    str = "vgosDB tree";
    break;
  case SgVlbiSessionInfo::OT_MK4:
    str = "Mk4 correlator files";
    break;
  case SgVlbiSessionInfo::OT_KOMB:
    str = "KOMB correlator files";
    break;
  case SgVlbiSessionInfo::OT_AGV:
    str = "VDA file";
    break;
  case SgVlbiSessionInfo::OT_UNKNOWN:
    str = "unknown";
    break;
  };
  label = new QLabel(str, gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

	//
  label = new QLabel("Input file:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  
  label = new QLabel(session_->primaryBand()->getInputFileName(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

	//
  label = new QLabel("Correlator postprocessing software:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  switch(session_->getCppsSoft())
  {
  case SgVlbiSessionInfo::CPPS_HOPS:
    str = "HOPS";
    break;
  case SgVlbiSessionInfo::CPPS_PIMA:
    str = "PIMA";
    break;
  case SgVlbiSessionInfo::CPPS_C5PP:
    str = "C5++";
    break;
  case SgVlbiSessionInfo::CPPS_UNKNOWN:
  default:
    str = "unknown";
    break;
  };
  label = new QLabel(str, gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);


  if (session_->getOfficialName().size()) // v01:
  {
    label = new QLabel("Official Name/Code:", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);
    label = new QLabel(session_->getOfficialName() + "/" + session_->getSessionCode(), gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++,1);
  }
  else //v02:
  {
    label = new QLabel("Network Type/Code:", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);
    label = new QLabel(session_->getSessionType() + "/" + session_->getSessionCode(), gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++, 1);
  };
  
  label = new QLabel("Experiment description:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  str = session_->getDescription().simplified();
  label = new QLabel(str!=""?str:QString(" "), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("The session was scheduled at:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getSchedulerName(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Correlated by:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getCorrelatorName(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Responsibility of:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getSubmitterName(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Latest version created on:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getTCreation().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Network ID:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  QComboBox                    *cbNetID=new QComboBox(gbox);
  //  cbNetID->setInsertPolicy(QComboBox::InsertAtBottom);
  cbNetID->setInsertPolicy(QComboBox::InsertAlphabetically);

  bool                          isTmp(false);
  for (int i=0; i<networks.size(); i++)
  {
    cbNetID->addItem(networks.at(i).getName());
    if (networks.at(i).getName() == session_->getNetworkID())
    {
      cbNetID->setCurrentIndex(i);
      isTmp = true;
    };
  };
  if (!isTmp) // a net ID from the session (default):
  {
    cbNetID->addItem(session_->getNetworkID());
    cbNetID->setCurrentIndex(cbNetID->count() - 1);
  };
  cbNetID->setMinimumSize(cbNetID->sizeHint());
  grid->addWidget(cbNetID, rowIdx++, 1);
  connect(cbNetID, 
    SIGNAL(currentIndexChanged(const QString&)), SLOT(netIdCurrentIndexChanged(const QString&)));
  //
  
  label = new QLabel("Epoch of the first observation:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getTStart().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Epoch of the last observation:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getTFinis().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Mean epoch of observations:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getTMean().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Reference epoch:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->tRefer().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Interval of observations:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(interval2Str(session_->getTFinis() - session_->getTStart()), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Last modified on:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
  label = new QLabel(session_->getLastProcessed().toString(SgMJD::F_Verbose), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  label = new QLabel("Record mode:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
//label = new QLabel(session_->primaryBand()->recordMode(), gbox);
  label = new QLabel(session_->getRecordingMode(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
  subLayout->addWidget(gbox);

  label = new QLabel("Sideband order:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);
//label = new QLabel(session_->primaryBand()->recordMode(), gbox);
  label = new QLabel(sidebandOrderByType[session_->getSidebandOrder()], gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
  subLayout->addWidget(gbox);



  // parameters:
  gbox = new QGroupBox("Parameters", w);
  grid = new QGridLayout(gbox);

  label = new QLabel("User Flag:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0,0);

  QComboBox *userFlag = new QComboBox(gbox);
  userFlag->setInsertPolicy(QComboBox::InsertAtBottom);

  const char* sUFlags[] = 
  {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
  };
  for (int i=0; i<(int)(sizeof(sUFlags)/sizeof(const char*)); i++)
  {
    userFlag->addItem(sUFlags[i]);
    if (sUFlags[i] == session_->getUserFlag()) 
      userFlag->setCurrentIndex(i);
  };
  userFlag->setMinimumSize(userFlag->sizeHint());
  grid->addWidget(userFlag, 0,1);
  subLayout->addWidget(gbox);

  // bands:
  gbox = new QGroupBox("Bands", w);
  grid = new QGridLayout(gbox);

  label = new QLabel("<b>ID</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 0);
  //
  label = new QLabel("<b>Freq</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 1);
  //
  label = new QLabel("<b>#Chan</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 2);
  //
  label = new QLabel("<b>SmplRate</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 3);
  //
  label = new QLabel("<b>Bit/smpl</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 4);
	//
  label = new QLabel("<b>I.Time</b> (s)", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 5);

  //
  label = new QLabel("<b>Ver</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 6);
  //
  /*
  label = new QLabel("<b>File</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 7);
  //
  label = new QLabel("<b>CALC ver</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 8);
  */
  //
  label = new QLabel("<b>#Total/Used</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 7);
  //
  label = new QLabel("<b>#Par/#C</b>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 8);

/**/
  //
  label = new QLabel("<b>WRMS</b>(ps)", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 9);
  //
  label = new QLabel("<p><b>&sigma;<sub>0</sub><b></p>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 10);
  //
  label = new QLabel("<p><b>&chi;<sup>2</sup><b></p>", gbox);
  label->setAlignment(Qt::AlignCenter);
  grid->addWidget(label, 0, 11);
/**/
/*
  QButtonGroup  *bgPrimaryBands = new QButtonGroup(gbox);
  QRadioButton  *rbPrimaryBand = NULL;
*/

//  QCheckBox     *cbIgnore = NULL;
  for (int i=0; i<session_->numberOfBands(); i++)
  {
    SgVlbiBand* band=session_->bands().at(i);

    label = new QLabel(band->getKey(), gbox);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i,  0);
    //
    label = new QLabel(str.sprintf("%6.1f", band->getFrequency()), gbox);
    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label, 1+i, 1);
    //
    label = new QLabel(str.sprintf("%2d", band->getMaxNumOfChannels()), gbox);
    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label, 1+i,  2);
    //
    label = new QLabel(str.sprintf("%6.1f", band->sampleRate()*1.0e-6), gbox);
    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label, 1+i,  3);
    //
    label = new QLabel(str.sprintf("%1d", band->bitsPerSample()), gbox);
    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label, 1+i,  4);
    //
    label = new QLabel(str.sprintf("%4.1f", band->meanEffectiveIntegrationTime()), gbox);
    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label, 1+i,  5);
    //
    //
    label = new QLabel(str.sprintf("%3d", band->getInputFileVersion()), gbox);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i,  6);
    //
    /*
    label = new QLabel(band->getInputFileName(), gbox);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i, 7);
    //
    label = new QLabel(str.sprintf("%g", session_->calcInfo().getDversion()), gbox);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i, 8);
    */
    //
    label = new QLabel(str.sprintf("%3d/%3d", 
      band->observables().count(), band->numProcessed(DT_DELAY)), gbox);
    labelsPrc_.append(label);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i,  7);
    //
    label = new QLabel(str.sprintf("%d/%d", 
      session_->getNumOfParameters(), session_->getNumOfConstraints()), gbox);
    labelsPar_.append(label);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i,  8);
    //
/**/
    label = new QLabel(str.sprintf("%.1f", band->wrms(DT_DELAY)*scale4Delay_*1.0E3), gbox);
    labelsWRMS_.append(label);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i,  9);
    //
    label = new QLabel(str.sprintf("%.1f", band->getSigma2add(DT_DELAY)*scale4Delay_*1.0E3), gbox);
    labelsSig0_.append(label);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i, 10);
    //
    label = new QLabel(str.sprintf("%.2f", 
      session_->getNumOfDOF()>0.0?band->chi2(DT_DELAY)/session_->getNumOfDOF():0.0), gbox);
    labelsChi2_.append(label);
    label->setAlignment(Qt::AlignCenter);
    grid->addWidget(label, 1+i, 11);
/**/
  };
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);
  subLayout->addWidget(gbox);
  subLayout->addStretch(1);
  layout->addStretch(5);
  //

/*
  connect(bgPrimaryBands, SIGNAL(buttonClicked(int)), SLOT(changePrimaryBand(int)));
*/

  //
  return w;
};



//
void NsSessionEditDialog::netIdCurrentIndexChanged(const QString& str)
{
  session_->setNetworkID(str);
  logger->write(SgLogger::DBG, SgLogger::SESSION, className() + 
    ": netIdCurrentIndexChanged(): the networkID of the session has been changed to `" + str + "'");
};



//
QWidget* NsSessionEditDialog::tab4Options()
{
  configWidget_ = new SgGuiTaskConfig(config_, parametersDescriptor_, session_, this);
  connect(this, SIGNAL(activeBandChanged(int)), configWidget_, SLOT(updateActiveBandSelection(int)));
  connect(this, SIGNAL(delayTypeChanged(int)), configWidget_, SLOT(updateDelayTypeSelection(int)));
  connect(configWidget_, SIGNAL(delayTypeModified(SgTaskConfig::VlbiDelayType)),
          this, SLOT(dispatchChangeOfYAxis(SgTaskConfig::VlbiDelayType)));

  connect(configWidget_, SIGNAL(clocksModelChanged(SgParameterCfg::PMode)),
          this, SLOT(dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode)));
  connect(configWidget_, SIGNAL(zenithModelChanged(SgParameterCfg::PMode)),
          this, SLOT(dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode)));

  return configWidget_;
};



//
void NsSessionEditDialog::dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode mode)
{
  for (QMap<QString, SgVlbiStationInfo*>::iterator it = session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->pcClocks()->setPMode(mode);
  };
};



//
void NsSessionEditDialog::dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode mode)
{
  for (QMap<QString, SgVlbiStationInfo*>::iterator it = session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->pcZenith()->setPMode(mode);
  };
};



//
QWidget* NsSessionEditDialog::tab4BaselinesPlot()
{
  QTabWidget *w = new QTabWidget(this);
//  w->setTabPosition(QTabWidget::South);
  w->setTabPosition(QTabWidget::East);
  for (int i=0; i<session_->numberOfBands(); i++)
  {
    w->addTab(tab4Band(session_->bands().at(i)), session_->bands().at(i)->getKey() + "-Band");
  };
  connect(w, SIGNAL(currentChanged(int)), this, SLOT(changeActiveBand(int)));
  connect(configWidget_, SIGNAL(activeBandModified(int)), w, SLOT(setCurrentIndex(int)));
  return w;
};



//
QWidget* NsSessionEditDialog::tab4Band(SgVlbiBand* band)
{
  SgGuiVlbiStationList   *stnInfoList;
  SgGuiVlbiSourceList    *srcInfoList;
  SgGuiVlbiBaselineList  *baselineInfoList;
  
  QTabWidget *w = new QTabWidget(this);
  w->setTabPosition(QTabWidget::South);
  w->addTab(tab4BandPlots(band), "Data");
  stnInfoList = new SgGuiVlbiStationList(OBM_BAND, band->getKey(), &band->stationsByName(), 
                                                                      session_->getTStart(), this);
  srcInfoList = new SgGuiVlbiSourceList(OBM_BAND, band->getKey(), &band->sourcesByName(), this);
  baselineInfoList = new SgGuiVlbiBaselineList(session_, config_, OBM_BAND, &band->baselinesByName(),
    this);
  w->addTab(stnInfoList, "Stations");
  w->addTab(srcInfoList, "Sources");
  w->addTab(baselineInfoList, "Baselines");
  w->addTab(new SgGuiVlbiHistory(&band->history(), this), "History");
//  stnInfoList->setWrmsScale(scale4Delay_*1.0E3);
//  srcInfoList->setWrmsScale(scale4Delay_*1.0E3);
//  baselineInfoList->setWrmsScale(scale4Delay_*1.0E3);
  connect(this, SIGNAL(dataChanged()), stnInfoList, SLOT(updateContent()));
  connect(this, SIGNAL(dataChanged()), srcInfoList, SLOT(updateContent()));
  connect(this, SIGNAL(dataChanged()), baselineInfoList, SLOT(updateContent()));
  return w;
};



//
QWidget* NsSessionEditDialog::tab4BandPlots(SgVlbiBand* band)
{
  QList<SgVlbiObservable*>     *observables=NULL;
  QString                       title("");
  QString                       str("");
  title = band->getKey() + "-band of the session " + session_->getName() + ", baselines";

//SgPlotCarrier* carrier = new SgPlotCarrier( 33, 11, title);
  SgPlotCarrier* carrier = new SgPlotCarrier( 35, 11, title);

  plotCarriers4Baselines_.append(carrier);
  str = title + " (group delay ambig.spacing: " + band->strGrdAmbigsStat() + " [ns])";
  carrier->setName(str, BPI_RES_GRD);
  carrier->setName(str, BPI_GR_DELAY);
  carrier->setName(str, BPI_ION_GRD);
  carrier->setName(str, BPI_AMBIG_GR_SPACING);
  carrier->setName(str, BPI_AMBIG_GR_NUMBER);
  str = title + " (phase delay ambig.spacing: " + band->strPhdAmbigsStat() + " [ns])";
  carrier->setName(str, BPI_RES_PHD);
  carrier->setName(str, BPI_ION_PHD);
  carrier->setName(str, BPI_AMBIG_PH_SPACING);
  carrier->setName(str, BPI_AMBIG_PH_NUMBER);

  carrier->setFile2SaveBaseName(session_->getName() + "_baselines_" + band->getKey() + "-band");

  // names of columns:
  // epoch:
  carrier->setNameOfColumn(BPI_EPOCH, "Time (UTC)");
  carrier->setAxisType(BPI_EPOCH, SgPlotCarrier::AxisType_MJD);
  // values:
  carrier->setNameOfColumn(BPI_RES_SBD,         "Res: SB delay, ns");
  carrier->setNameOfColumn(BPI_RES_GRD,         "Res: GR delay, ns");
  carrier->setNameOfColumn(BPI_RES_PHD,         "Res: PH delay, ns");
  carrier->setNameOfColumn(BPI_RES_PHR,         "Res: PH rate, fs/s");
  carrier->setNameOfColumn(BPI_GR_DELAY,        "Group delay, ns");
  carrier->setNameOfColumn(BPI_PH_RATE,         "Delay rate, fs/s");
  carrier->setNameOfColumn(BPI_ION_SBD,         "Iono SB delay, ns");
  carrier->setNameOfColumn(BPI_ION_GRD,         "Iono GR delay, ns");
  carrier->setNameOfColumn(BPI_ION_PHD,         "Iono PH delay, ns");
  carrier->setNameOfColumn(BPI_ION_PHR,         "Iono PH rate, fs/s");
  carrier->setNameOfColumn(BPI_ION_DTEC,        "Iono dTEK");
  carrier->setNameOfColumn(BPI_CORR_COEF,       "Corr.Coeff.");
  carrier->setNameOfColumn(BPI_SNR,             "SNR");
  // sigmas:
  carrier->setNameOfColumn(BPI_SIG_SBD,         "Sigma of SB delay, ns");
  carrier->setNameOfColumn(BPI_SIG_GRD,         "Sigma of GR delay, ns");
  carrier->setNameOfColumn(BPI_SIG_DEL_APPLIED, "Applied GR delay sigma, ns");
  carrier->setNameOfColumn(BPI_SIG_RAT_APPLIED, "Applied PH rate sigma, fs/s");
  carrier->setNameOfColumn(BPI_SIG_PHR,         "Sigma of delay rate, fs/s");
  carrier->setNameOfColumn(BPI_SIG_PHD,         "Sigma of PH delay, ns");
  carrier->setNameOfColumn(BPI_SIG_ION_SBD,     "Sigma of Iono SB delay, ns");
  carrier->setNameOfColumn(BPI_SIG_ION_GRD,     "Sigma of Iono GR delay, ns");
  carrier->setNameOfColumn(BPI_SIG_ION_PHD,     "Sigma of Iono PH delay, ns");
  carrier->setNameOfColumn(BPI_SIG_ION_PHR,     "Sigma of Iono PH rate, fs/s");
  carrier->setNameOfColumn(BPI_SIG_ION_DTEC,    "Sigma of Iono dTEK");
  //
  carrier->setNameOfColumn(BPI_RES_NORM_DEL,    "Normalized delay residuals");
  carrier->setNameOfColumn(BPI_RES_NORM_RAT,    "Normalized rate residuals");
  carrier->setNameOfColumn(BPI_AMBIG_GR_SPACING,"Ambig GR spacing, ns");
  carrier->setNameOfColumn(BPI_AMBIG_GR_NUMBER, "Ambig GR #");
  carrier->setNameOfColumn(BPI_AMBIG_PH_SPACING,"Ambig PH spacing, ns");
  carrier->setNameOfColumn(BPI_AMBIG_PH_NUMBER, "Ambig PH #");

  carrier->setNameOfColumn(BPI_EFF_FRQ_GRD,     "GR Eff Freq, MHZ");
  carrier->setNameOfColumn(BPI_EFF_FRQ_PHD,     "PH Eff Freq, MHZ");
  carrier->setNameOfColumn(BPI_EFF_FRQ_PHR,     "RT Eff Freq, MHZ");
  //
  //carrier->setNameOfColumn(BPI_S_DIFF,          "sDiff");
  //carrier->setNameOfColumn(BPI_X_DIFF,          "xDiff");
  //carrier->setNameOfColumn(BPI_R_DIFF,          "rDiff");
  //carrier->setNameOfColumn(BPI_GRD_SBD,         "GRD-SBD");
  //carrier->setNameOfColumn(BPI_Q_SBD,           "qSBD");
  //carrier->setNameOfColumn(BPI_Q_GRD,           "qGRD");
  //carrier->setNameOfColumn(BPI_Q_PHR,           "qPHR");
  //carrier->setNameOfColumn(BPI_CLK_1,           "ClcF#1");
  //carrier->setNameOfColumn(BPI_CLK_1L,          "ClcF#1_left");
  //carrier->setNameOfColumn(BPI_CLK_1R,          "ClcF#1_right");
  carrier->setNameOfColumn(BPI_QF,              "QF");
  carrier->setNameOfColumn(BPI_FEC,             "Frng Error Code");
  carrier->setNameOfColumn(BPI_PCAL_GR_DEL_1,   "PCal:GrDel@1");
  carrier->setNameOfColumn(BPI_PCAL_PH_DEL_1,   "PCal:PhDel@1");
  carrier->setNameOfColumn(BPI_PCAL_GR_DEL_2,   "PCal:GrDel@2");
  carrier->setNameOfColumn(BPI_PCAL_PH_DEL_2,   "PCal:PhDel@2");
  carrier->setNameOfColumn(BPI_U,               "U");
  carrier->setNameOfColumn(BPI_V,               "V");
  carrier->setNameOfColumn(BPI_EFF_INTGR_TIME,  "EffIntgrnTime, s");
  carrier->setNameOfColumn(BPI_TAUS,            "tauS, ns");
  carrier->setNameOfColumn(BPI_TEST,            "Test");
  carrier->setNameOfColumn(BPI_TEST2,           "Test2");

  // set up sigmas:
  carrier->setStdVarIdx(BPI_RES_SBD,  BPI_SIG_SBD);
  carrier->setStdVarIdx(BPI_RES_GRD,  BPI_SIG_DEL_APPLIED);
  carrier->setStdVarIdx(BPI_RES_PHD,  BPI_SIG_PHD);
  carrier->setStdVarIdx(BPI_RES_PHR,  BPI_SIG_RAT_APPLIED);
  carrier->setStdVarIdx(BPI_GR_DELAY, BPI_SIG_GRD);
  carrier->setStdVarIdx(BPI_PH_RATE,  BPI_SIG_PHR);
  carrier->setStdVarIdx(BPI_ION_SBD,  BPI_SIG_ION_SBD);
  carrier->setStdVarIdx(BPI_ION_GRD,  BPI_SIG_ION_GRD);
  carrier->setStdVarIdx(BPI_ION_PHD,  BPI_SIG_ION_PHD);
  carrier->setStdVarIdx(BPI_ION_PHR,  BPI_SIG_ION_PHR);
  carrier->setStdVarIdx(BPI_ION_DTEC, BPI_SIG_ION_DTEC);
    
  // create branches and fill data:
  carrier->listOfBranches()->clear();
  title = band->getKey() + "-band of the session " + session_->getName() + "; ";
  QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=band->baselinesByName().constBegin();
  for (; it!=band->baselinesByName().constEnd(); ++it)
  {
    SgVlbiBaselineInfo         *baselineInfo=it.value();
    observables = &baselineInfo->observables();
    carrier->createBranch(observables->size(), baselineInfo->getKey(), true);
    SgPlotBranch               *branch=carrier->listOfBranches()->last();
    //
    // group delays ambigs info:
    str = title + baselineInfo->getKey() + " (ambig. spacing: " + 
      baselineInfo->strGrdAmbigsStat() + " [ns])";
    branch->setAlternativeTitleName(str, BPI_RES_GRD);
    branch->setAlternativeTitleName(str, BPI_GR_DELAY);
    branch->setAlternativeTitleName(str, BPI_AMBIG_GR_SPACING);
    branch->setAlternativeTitleName(str, BPI_AMBIG_GR_NUMBER);
    // phase delays ambigs info:
    str = title + baselineInfo->getKey() + " (ambig. spacing: " + 
      baselineInfo->strPhdAmbigsStat() + " [ns])";
    branch->setAlternativeTitleName(str, BPI_RES_PHD);
    branch->setAlternativeTitleName(str, BPI_AMBIG_PH_SPACING);
    branch->setAlternativeTitleName(str, BPI_AMBIG_PH_NUMBER);

    //
    QList<SgVlbiObservable*>::const_iterator jt=observables->constBegin();
    for (int idx=0; jt!=observables->constEnd(); ++jt, idx++)
    {
      SgVlbiObservable       *o=*jt;
      //
      if (processHasBeenCalled_)
      {
//        if (!o->owner()->isAttr(SgObservation::Attr_PROCESSED))
        if (!o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
          branch->addDataAttr(idx, SgPlotCarrier::DA_REJECTED);
        else
          branch->delDataAttr(idx, SgPlotCarrier::DA_REJECTED);
        if (o->isUsable())
          branch->delDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
        else
          branch->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
				//
				if (o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL) ||
						o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL))
        	branch->addDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
      };
      //
      // epoch:
      branch->data()->setElement(idx, BPI_EPOCH,   o->epoch().toDouble());
      // residuals:
      branch->data()->setElement(idx, BPI_RES_SBD, o->sbDelay().getResidual()*scale4Delay_);
      branch->data()->setElement(idx, BPI_RES_GRD, o->grDelay().getResidual()*scale4Delay_);
      branch->data()->setElement(idx, BPI_RES_PHD, o->phDelay().getResidual()*scale4Delay_);
      branch->data()->setElement(idx, BPI_RES_PHR, o->phDRate().getResidual()*scale4Rate_);
      // values:
      branch->data()->setElement(idx, BPI_GR_DELAY,o->grDelay().getValue()   *scale4Delay_);
      branch->data()->setElement(idx, BPI_PH_RATE, o->phDRate().getValue()   *scale4Rate_);
      // ionospheric corrections:
      branch->data()->setElement(idx, BPI_ION_SBD, o->sbDelay().getIonoValue()*scale4Delay_);
      branch->data()->setElement(idx, BPI_ION_GRD, o->grDelay().getIonoValue()*scale4Delay_);
      branch->data()->setElement(idx, BPI_ION_PHD, o->phDelay().getIonoValue()*scale4Delay_);
      branch->data()->setElement(idx, BPI_ION_PHR, o->phDRate().getIonoValue()*scale4Rate_);
      branch->data()->setElement(idx, BPI_ION_DTEC,o->owner()->getDTec());
      // 
      branch->data()->setElement(idx, BPI_CORR_COEF, o->getCorrCoeff());
      branch->data()->setElement(idx, BPI_SNR, o->getSnr());
      // sigmas:
      branch->data()->setElement(idx, BPI_SIG_SBD,      o->sbDelay().getSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_GRD,      o->grDelay().getSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_PHD,      o->phDelay().getSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_DEL_APPLIED,
                                                        o->grDelay().sigma2Apply()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_RAT_APPLIED,
                                                        o->phDRate().sigma2Apply()*scale4Rate_);
      branch->data()->setElement(idx, BPI_SIG_PHR,      o->phDRate().getSigma()    *scale4Rate_);
      // iono's sigmas:
      branch->data()->setElement(idx, BPI_SIG_ION_SBD,  o->sbDelay().getIonoSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_ION_GRD,  o->grDelay().getIonoSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_ION_PHD,  o->phDelay().getIonoSigma()*scale4Delay_);
      branch->data()->setElement(idx, BPI_SIG_ION_PHR,  o->phDRate().getIonoSigma()*scale4Rate_);
      branch->data()->setElement(idx, BPI_SIG_ION_DTEC, o->owner()->getDTecStdDev());
        //
//      branch->data()->setElement(idx, BPI_NORM_RESID,   o->grDelay().getResidualNorm());
      branch->data()->setElement(idx, BPI_RES_NORM_DEL, o->activeDelay()?
                                                        o->activeDelay()->getResidualNorm():0.0);
      branch->data()->setElement(idx, BPI_RES_NORM_RAT, o->phDRate().getResidualNorm());
      branch->data()->setElement(idx, BPI_AMBIG_GR_SPACING,
                                                        o->grDelay().getAmbiguitySpacing()*scale4Delay_);
      branch->data()->setElement(idx, BPI_AMBIG_GR_NUMBER, 
                                                        o->grDelay().getNumOfAmbiguities());
      branch->data()->setElement(idx, BPI_AMBIG_PH_SPACING,
                                                        o->phDelay().getAmbiguitySpacing()*scale4Delay_);
      branch->data()->setElement(idx, BPI_AMBIG_PH_NUMBER, 
                                                        o->phDelay().getNumOfAmbiguities());
      branch->data()->setElement(idx, BPI_EFF_FRQ_GRD,  o->grDelay().getEffFreq());
      branch->data()->setElement(idx, BPI_EFF_FRQ_PHD,  o->phDelay().getEffFreq());
      branch->data()->setElement(idx, BPI_EFF_FRQ_PHR,  o->phDRate().getEffFreq());

      //branch->data()->setElement(idx, BPI_S_DIFF,       o->getSbdDiffBand()*scale4Delay_);
      //branch->data()->setElement(idx, BPI_X_DIFF,       o->getGrdDiffBand()*scale4Delay_);
      //branch->data()->setElement(idx, BPI_R_DIFF,       o->getPhrDiffBand());
      //branch->data()->setElement(idx, BPI_GRD_SBD,     (o->grDelay().getValue() +
      //  o->grDelay().ambiguity() +
      //  o->grDelay().getIonoValue() - o->sbDelay().getValue())*scale4Delay_);
      //branch->data()->setElement(idx, BPI_CLK_1,        o->owner()->getBaselineClock_F1()*scale4Delay_);
      //branch->data()->setElement(idx, BPI_CLK_1L,       o->owner()->getBaselineClock_F1l()*scale4Delay_);
      //branch->data()->setElement(idx, BPI_CLK_1R,       o->owner()->getBaselineClock_F1r()*scale4Delay_);
      branch->data()->setElement(idx, BPI_QF,           	o->getQualityFactor());
      branch->data()->setElement(idx, BPI_FEC,          	o->errorCode2Int());
      branch->data()->setElement(idx, BPI_PCAL_GR_DEL_1,	o->getPhaseCalGrDelays(0)*scale4Delay_);
      branch->data()->setElement(idx, BPI_PCAL_PH_DEL_1,	o->getPhaseCalPhDelays(0)*scale4Delay_);
      branch->data()->setElement(idx, BPI_PCAL_GR_DEL_2,	o->getPhaseCalGrDelays(1)*scale4Delay_);
      branch->data()->setElement(idx, BPI_PCAL_PH_DEL_2,	o->getPhaseCalPhDelays(1)*scale4Delay_);
      branch->data()->setElement(idx, BPI_U,            	o->getUvFrPerAsec(0));
      branch->data()->setElement(idx, BPI_V,            	o->getUvFrPerAsec(1));
      branch->data()->setElement(idx, BPI_EFF_INTGR_TIME, o->getEffIntegrationTime());
      branch->data()->setElement(idx, BPI_TAUS,         	o->getTauS()*scale4Delay_);
//    branch->data()->setElement(idx, BPI_TEST,         	o->getTest()*scale4Delay_);
      branch->data()->setElement(idx, BPI_TEST,         	o->getTest());
//      branch->data()->setElement(idx, BPI_TEST,         	o->owner()->getBaselineClock_F1()*scale4Delay_);
//    branch->data()->setElement(idx, BPI_TEST2,        	o->getTest2()*scale4Delay_);
      branch->data()->setElement(idx, BPI_TEST2,        	o->getTest2());
//      branch->data()->setElement(idx, BPI_TEST2,        	o->owner()->getBaselineClock_F2()*scale4Delay_);
      
      branch->setExtKey(idx, o->src()->getKey());
    };
  };

  // widgets:
  QWidget *w = new QWidget(this);
  unsigned int                  plotterModes;
  plotterModes = SgPlot::PM_ERRBARS | SgPlot::PM_Q_PNT_EXT_PROC | SgPlot::PM_FILTERS_ENABLED;
  if (setup.getIsBandPlotPerSrcView())
    plotterModes |= SgPlot::PM_EXT_KEY_SELECT;
  
  SgPlot *plot = new SgPlot(carrier, setup.path2(setup.getPath2PlotterOutput()), w, plotterModes);
  plot->setOutputFormat(setup.getPlotterOutputFormat());
  //
  QList<QString>                filters, filtersAux, filtersExt;
  QString                       blNames("");
  for (BaselinesByName_it it=session_->baselinesByName().begin(); 
    it!=session_->baselinesByName().end(); ++it)
      blNames += it.value()->getKey() + ",";
  for (StationsByName_it it=session_->stationsByName().begin(); 
    it!=session_->stationsByName().end(); ++it)
  {
    filters << it.value()->getKey();
    QString   key=it.value()->getKey() + ":";
    if (blNames.contains(key))
      filtersAux << key;
    key = ":" + it.value()->getKey();
    if (blNames.contains(key))
      filtersAux << key;
  };
  for (SourcesByName_it it=session_->sourcesByName().begin(); 
    it!=session_->sourcesByName().end(); ++it)
    filtersExt << it.value()->getKey();
  plot->setFilterNames(filters);
  plot->setFilterAuxNames(filtersAux);
  plot->setFilterExtNames(filtersExt);
  //
  plots4Baselines_.append(plot);
  QBoxLayout *layout = new QVBoxLayout(w);
  layout->addWidget(plot);
  connect(plot, SIGNAL(userPressedAKey(SgPlot*, Qt::KeyboardModifiers, int)), 
                SLOT  (dispatchUserKey(SgPlot*, Qt::KeyboardModifiers, int)));
  connect(plot, SIGNAL(yAxisChanged(int)), SLOT(dispatchChangeOfYAxis(int)));
	//
  connect(plot, SIGNAL(pointInfoRequested(SgPlot*, SgPlotBranch*, int, int, int, SgPlot::QueryMode)), 
                SLOT  (displayPointInfo(SgPlot*, SgPlotBranch*, int, int, int, SgPlot::QueryMode)));

/*

  connect(plot, SIGNAL(pointInfoRequested(SgPlot*, SgPlotBranch*, int, int, int, SgPlot::QueryMode)), 
                SLOT  (displayObservableInfo(SgPlot*, SgPlotBranch*, int, int, int, SgPlot::QueryMode)));
  connect(plot, SIGNAL(pointInfoRequested_mode1(SgPlot*, SgPlotBranch*, int, int, int)), 
                SLOT  (displayPointInfo(SgPlot*, SgPlotBranch*, int, int, int)));
*/
  return w;
};



//
QWidget* NsSessionEditDialog::tab4StationsPlot()
{
  QMap<QString, SgVlbiAuxObservation*> 
															 *auxObservationsByScan=NULL;
  int                           numOfIfs = session_->skyFreqByIfId().size();
  QList<QString>                ifIds=session_->skyFreqByIfId().keys();
  QList<double>                 ifFrqs=session_->skyFreqByIfId().values();

  cableCalsOffset_ = 0;
  // check for presence of some data:
  bool                          hasTsyses=false;
  bool                          hasCdms=false;
  bool                          hasPcmt=false;
  QMap<QString, SgVlbiStationInfo*>::const_iterator
                                it=session_->stationsByName().constBegin();
  for (; it!=session_->stationsByName().constEnd(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();

    if (si->isAttr(SgVlbiStationInfo::Attr_HAS_TSYS))
      hasTsyses = true;
    if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS))
      hasCdms = true;
    if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT))
      hasPcmt = true;
  };
  if (!hasTsyses)
    numOfIfs = 0;
  //
  //
  if (hasCdms || hasPcmt)
    cableCalsOffset_ = 3;
  //
  plotCarrier4Stations_ = new SgPlotCarrier( 14 + numOfIfs + cableCalsOffset_, 6, 
    "Plots for the session " + session_->getName() + ", station view");
  plotCarrier4Stations_->setFile2SaveBaseName(session_->getName() + "_stations_");

  // names of columns:
  plotCarrier4Stations_->setNameOfColumn(SPI_EPOCH,       "Time (UTC)");
  plotCarrier4Stations_->setAxisType    (SPI_EPOCH, SgPlotCarrier::AxisType_MJD);
  if (hasCdms || hasPcmt)
  {
    plotCarrier4Stations_->setNameOfColumn(SPI_CABLE_CAL,     "Default Cable cal., ps");
    plotCarrier4Stations_->setNameOfColumn(SPI_CABLE_CAL + 1, "FS Log Cable cal., ps");
    plotCarrier4Stations_->setNameOfColumn(SPI_CABLE_CAL + 2, "CDMS Cable cal., ps");
    plotCarrier4Stations_->setNameOfColumn(SPI_CABLE_CAL + 3, "PCMT Cable cal., ps");
  }
  else
  plotCarrier4Stations_->setNameOfColumn(SPI_CABLE_CAL,   "Cable cal., ps");
  plotCarrier4Stations_->setNameOfColumn(SPI_MET_TEMP + cableCalsOffset_,    "Met.: temp., C");
  plotCarrier4Stations_->setNameOfColumn(SPI_MET_PRESS + cableCalsOffset_,   "Met.: press., mb");
  plotCarrier4Stations_->setNameOfColumn(SPI_MET_HUMID + cableCalsOffset_,   "Met.: rel.hum., %");
  plotCarrier4Stations_->setNameOfColumn(SPI_AZIMUTH + cableCalsOffset_,     "Azimuth, deg");
  plotCarrier4Stations_->setNameOfColumn(SPI_ELEVATION + cableCalsOffset_,   "Elevation, deg");
  plotCarrier4Stations_->setNameOfColumn(SPI_PARALLACTIC + cableCalsOffset_, "Parallactic, deg");
  plotCarrier4Stations_->setNameOfColumn(SPI_EST_CLK + cableCalsOffset_,     "Est.Clocks, ps");
//  plotCarrier4Stations_->setNameOfColumn(SPI_EST_WZD, "Est.Zenith, cm");
  plotCarrier4Stations_->setNameOfColumn(SPI_EST_WZD + cableCalsOffset_,     "Est.Zenith, ps");
  plotCarrier4Stations_->setNameOfColumn(SPI_EST_GRN + cableCalsOffset_,     "Est.AtmGrd:N, cm");
  plotCarrier4Stations_->setNameOfColumn(SPI_EST_GRE + cableCalsOffset_,     "Est.AtmGrd:E, cm");
  plotCarrier4Stations_->setNameOfColumn(SPI_TOT_ZDEL + cableCalsOffset_,    "TZD, cm");
  // sigmas:
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_EST_CLK + cableCalsOffset_, "Sigma of clocks");
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_EST_WZD + cableCalsOffset_, "Sigma of zenith");
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_EST_GRN + cableCalsOffset_, "Sigma of atmGrd:N");
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_EST_GRE + cableCalsOffset_, "Sigma of atmGrd:E");
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_TOT_ZDEL + cableCalsOffset_,"Sigma of TZD");
  plotCarrier4Stations_->setStdVarIdx   (SPI_EST_CLK + cableCalsOffset_,
    SPI_SIG_EST_CLK + cableCalsOffset_);
  plotCarrier4Stations_->setStdVarIdx   (SPI_EST_WZD + cableCalsOffset_,
    SPI_SIG_EST_WZD + cableCalsOffset_);
  plotCarrier4Stations_->setStdVarIdx   (SPI_EST_GRN + cableCalsOffset_,
    SPI_SIG_EST_GRN + cableCalsOffset_);
  plotCarrier4Stations_->setStdVarIdx   (SPI_EST_GRE + cableCalsOffset_,
    SPI_SIG_EST_GRE + cableCalsOffset_);
  plotCarrier4Stations_->setStdVarIdx   (SPI_TOT_ZDEL + cableCalsOffset_,
    SPI_SIG_TOT_ZDEL + cableCalsOffset_);

  plotCarrier4Stations_->setNameOfColumn(SPI_RES_GRD + cableCalsOffset_,     "Res: GrDelay, ps");
  plotCarrier4Stations_->setNameOfColumn(SPI_SIG_GRD + cableCalsOffset_,     "Sigma of GrD resid");
  plotCarrier4Stations_->setStdVarIdx   (SPI_RES_GRD + cableCalsOffset_,  SPI_SIG_GRD + cableCalsOffset_);
  for (int i=0; i<numOfIfs; i++)
    plotCarrier4Stations_->setNameOfColumn(SPI_SIG_GRD + cableCalsOffset_ + 1 + i, 
      "Tsys @IF " + ifIds.at(i) + QString("").sprintf(", %.2fMHz", ifFrqs.at(i)));


  // create branches and fill data:
  plotCarrier4Stations_->listOfBranches()->clear();
  for (it=session_->stationsByName().constBegin(); it!=session_->stationsByName().constEnd(); ++it)
  {
    SgVlbiStationInfo*          stationInfo=it.value();
    QMap<int, int>              frq2if;
    for (int i=0; i<ifFrqs.size(); i++)
    {
      frq2if[i] = -1;
      for (int j=0; j<stationInfo->tsysIfFreqs().size(); j++)
        if (fabs(ifFrqs.at(i) - stationInfo->tsysIfFreqs().at(j)) < 0.01)
          frq2if[i] = j;
    };
    
    auxObservationsByScan = stationInfo->auxObservationByScanId();
    plotCarrier4Stations_->createBranch(auxObservationsByScan->size(), stationInfo->getKey());
    SgPlotBranch             *branch=plotCarrier4Stations_->listOfBranches()->last();
    //
    QMap<QString, SgVlbiAuxObservation*>::const_iterator jt=auxObservationsByScan->constBegin();
    for (int idx=0; jt!=auxObservationsByScan->constEnd(); ++jt, idx++)
    {
      SgVlbiAuxObservation   *auxObs=jt.value();
        
      branch->data()->setElement(idx, SPI_EPOCH,        auxObs->toDouble());
      branch->data()->setElement(idx, SPI_CABLE_CAL,    auxObs->getCableCalibration()*1.0e12);
      if (hasCdms || hasPcmt)
      {
        branch->data()->setElement(idx, SPI_CABLE_CAL,  auxObs->getCableCalibration()*1.0e12);
        branch->data()->setElement(idx, SPI_CABLE_CAL + 1,
          auxObs->cableCorrections().getElement(SgVlbiAuxObservation::CCT_FSLG)*1.0e12);
        branch->data()->setElement(idx, SPI_CABLE_CAL + 2,
          auxObs->cableCorrections().getElement(SgVlbiAuxObservation::CCT_CDMS)*1.0e12);
        branch->data()->setElement(idx, SPI_CABLE_CAL + 3,
          auxObs->cableCorrections().getElement(SgVlbiAuxObservation::CCT_PCMT)*1.0e12);
      }
      else
        branch->data()->setElement(idx, SPI_CABLE_CAL,
          auxObs->getCableCalibration()*1.0e12);

      branch->data()->setElement(idx, SPI_MET_TEMP + cableCalsOffset_,
        auxObs->getMeteoData().getTemperature());
      branch->data()->setElement(idx, SPI_MET_PRESS + cableCalsOffset_,
        auxObs->getMeteoData().getPressure());
      branch->data()->setElement(idx, SPI_MET_HUMID + cableCalsOffset_,
        auxObs->getMeteoData().getRelativeHumidity()*100.0);
      branch->data()->setElement(idx, SPI_AZIMUTH + cableCalsOffset_,
        auxObs->getAzimuthAngle()*RAD2DEG);
      branch->data()->setElement(idx, SPI_ELEVATION + cableCalsOffset_,
        auxObs->getElevationAngle()*RAD2DEG);
      branch->data()->setElement(idx, SPI_PARALLACTIC + cableCalsOffset_,
        auxObs->getParallacticAngle()*RAD2DEG);
      branch->data()->setElement(idx, SPI_EST_CLK + cableCalsOffset_,
        auxObs->getEstClocks());
      branch->data()->setElement(idx, SPI_EST_WZD + cableCalsOffset_,
        auxObs->getEstZenithDelay()*1.0e10/vLight);
      branch->data()->setElement(idx, SPI_EST_GRN + cableCalsOffset_,
        auxObs->getEstAtmGradN());
      branch->data()->setElement(idx, SPI_EST_GRE + cableCalsOffset_,
        auxObs->getEstAtmGradE());
      branch->data()->setElement(idx, SPI_TOT_ZDEL + cableCalsOffset_,
        (auxObs->getZenithDelayH() + auxObs->getZenithDelayW())*100.0 + auxObs->getEstZenithDelay());
      branch->data()->setElement(idx, SPI_SIG_EST_CLK + cableCalsOffset_,
        auxObs->getEstClocksSigma());
      branch->data()->setElement(idx, SPI_SIG_EST_WZD + cableCalsOffset_,
        auxObs->getEstZenithDelaySigma()*1.0e10/vLight);
      branch->data()->setElement(idx, SPI_SIG_EST_GRN + cableCalsOffset_,
        auxObs->getEstAtmGradNSigma());
      branch->data()->setElement(idx, SPI_SIG_EST_GRE + cableCalsOffset_,
        auxObs->getEstAtmGradESigma());
      branch->data()->setElement(idx, SPI_SIG_TOT_ZDEL + cableCalsOffset_,
        auxObs->getEstZenithDelaySigma());

      SgVlbiObservation      *obs=auxObs->getOwner();
      SgVlbiObservable       *o=obs?obs->activeObs():NULL;
      if (o)
      {
        branch->data()->setElement(idx, SPI_RES_GRD + cableCalsOffset_,
          o->grDelay().getResidual()*scale4Delay_*1.0e3);
        branch->data()->setElement(idx, SPI_SIG_GRD + cableCalsOffset_,
          o->grDelay().sigma2Apply()*scale4Delay_*1.0e3);
      };

      for (int i=0; i<numOfIfs; i++)
        if (-1 < frq2if[i])
          branch->data()->setElement(idx, SPI_SIG_GRD + cableCalsOffset_ + 1 + i,
            auxObs->tSyses()->getElement(frq2if[i]));
//      if (auxObs->getElevationAngle()<-9.0)
//        branch->addDataAttr(idx, SgPlotCarrier::DataAttr_IGNORE);
//    if (auxObs->meteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA))
//      branch->addDataAttr(idx, SgPlotCarrier::DataAttr_IGNORE);
    };
  };

  // widgets:
  QWidget *w = new QWidget(this);
  plot4Stations_ = new SgPlot(plotCarrier4Stations_, setup.path2(setup.getPath2PlotterOutput()), w);
  plot4Stations_->setOutputFormat(setup.getPlotterOutputFormat());
  QBoxLayout *layout = new QVBoxLayout(w);
  layout->addWidget(plot4Stations_);

  return w;
};



//
//
QWidget* NsSessionEditDialog::tab4SessionPlot()
{
  QWidget                      *w=new QWidget(this);

  plotCarrier4Session_ = new SgPlotCarrier( 4, /*+*/ 3, "Plots for the session " + 
    session_->getName() + ", session wide data");

  plotCarrier4Session_->setFile2SaveBaseName(session_->getName() + "_session_");

  // names of columns:
  plotCarrier4Session_->setNameOfColumn(SNI_EPOCH, "Time (UTC)");
  plotCarrier4Session_->setAxisType    (SNI_EPOCH, SgPlotCarrier::AxisType_MJD);

  plotCarrier4Session_->setNameOfColumn(SNI_EST_UT1, "dUT1, ms");
  plotCarrier4Session_->setNameOfColumn(SNI_EST_PMX, "dPMx, mas");
  plotCarrier4Session_->setNameOfColumn(SNI_EST_PMY, "dPMy, mas");
  plotCarrier4Session_->setNameOfColumn(SNI_SIG_UT1, "eUT1, ms");
  plotCarrier4Session_->setNameOfColumn(SNI_SIG_PMX, "ePMx, mas");
  plotCarrier4Session_->setNameOfColumn(SNI_SIG_PMY, "ePMy, mas");
  plotCarrier4Session_->setStdVarIdx   (SNI_EST_UT1, SNI_SIG_UT1);
  plotCarrier4Session_->setStdVarIdx   (SNI_EST_PMX, SNI_SIG_PMX);
  plotCarrier4Session_->setStdVarIdx   (SNI_EST_PMY, SNI_SIG_PMY);

  // create branches and fill data:
  plotCarrier4Session_->listOfBranches()->clear();

  int                           numOfScans=session_->scanEpochs().size(), idx;
  SgMJD                          t(tZero);

  plotCarrier4Session_->createBranch(numOfScans, "ERP: HF external model");
  SgPlotBranch                  *branch=plotCarrier4Session_->listOfBranches()->last();
  plotCarrier4Session_->createBranch(numOfScans, "ERP: HF CALC");
  SgPlotBranch                  *branchC=plotCarrier4Session_->listOfBranches()->last();
  plotCarrier4Session_->createBranch(numOfScans, "ERP: estimation ARC");
  SgPlotBranch                  *branchA=plotCarrier4Session_->listOfBranches()->last();
  plotCarrier4Session_->createBranch(numOfScans, "ERP: estimation PWL");
  SgPlotBranch                  *branchP=plotCarrier4Session_->listOfBranches()->last();
  plotCarrier4Session_->createBranch(numOfScans, "ERP: estimation stochastic");
  SgPlotBranch                  *branchS=plotCarrier4Session_->listOfBranches()->last();

  t = tZero;
  idx = 0;
  for (int i=0; i<session_->observations().size(); i++)
    if (t != session_->observations().at(i)->getMJD())
    {
      SgVlbiObservation         *obs=session_->observations().at(i);
      t = session_->observations().at(i)->getMJD();
      branch->data()->setElement(idx, SNI_EPOCH,   obs->toDouble());
      
      branch->data()->setElement(idx, SNI_EST_UT1, obs->getAprioriUt1HfContrib()*DAY2SEC*1.0e3);
      branch->data()->setElement(idx, SNI_SIG_UT1, 0.0);
      branch->data()->setElement(idx, SNI_EST_PMX, obs->getAprioriPxHfContrib()*RAD2MAS);
      branch->data()->setElement(idx, SNI_SIG_PMX, 0.0);
      branch->data()->setElement(idx, SNI_EST_PMY, obs->getAprioriPyHfContrib()*RAD2MAS);
      branch->data()->setElement(idx, SNI_SIG_PMY, 0.0);

      branchC->data()->setElement(idx, SNI_EPOCH,   obs->toDouble());
      branchC->data()->setElement(idx, SNI_EST_UT1, obs->getCalcHiFyUt1Delay()/obs->getDdel_dUT1() *DAY2SEC*1.0e3);
      // there are no LCODES from CALC with Px and Py corrections due to the model.

      branchA->data()->setElement(idx, SNI_EPOCH,   obs->toDouble());
      branchA->data()->setElement(idx, SNI_EST_UT1, 0.0);
      branchA->data()->setElement(idx, SNI_SIG_UT1, 0.0);
      branchA->data()->setElement(idx, SNI_EST_PMX, 0.0);
      branchA->data()->setElement(idx, SNI_SIG_PMX, 0.0);
      branchA->data()->setElement(idx, SNI_EST_PMY, 0.0);
      branchA->data()->setElement(idx, SNI_SIG_PMY, 0.0);
      branchA->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);

      branchP->data()->setElement(idx, SNI_EPOCH,   obs->toDouble());
      branchP->data()->setElement(idx, SNI_EST_UT1, 0.0);
      branchP->data()->setElement(idx, SNI_SIG_UT1, 0.0);
      branchP->data()->setElement(idx, SNI_EST_PMX, 0.0);
      branchP->data()->setElement(idx, SNI_SIG_PMX, 0.0);
      branchP->data()->setElement(idx, SNI_EST_PMY, 0.0);
      branchP->data()->setElement(idx, SNI_SIG_PMY, 0.0);
      branchP->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);

      branchS->data()->setElement(idx, SNI_EPOCH,   obs->toDouble());
      branchS->data()->setElement(idx, SNI_EST_UT1, 0.0);
      branchS->data()->setElement(idx, SNI_SIG_UT1, 0.0);
      branchS->data()->setElement(idx, SNI_EST_PMX, 0.0);
      branchS->data()->setElement(idx, SNI_SIG_PMX, 0.0);
      branchS->data()->setElement(idx, SNI_EST_PMY, 0.0);
      branchS->data()->setElement(idx, SNI_SIG_PMY, 0.0);
      branchS->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);

      idx++;
    };

  // widgets:
  plot4Session_ = new SgPlot(plotCarrier4Session_, setup.path2(setup.getPath2PlotterOutput()), w);
  plot4Session_->setOutputFormat(setup.getPlotterOutputFormat());
  QBoxLayout                    *layout=new QVBoxLayout(w);
  layout->addWidget(plot4Session_);

  return w;
};



//
void NsSessionEditDialog::changeActiveBand(int idx)
{
  activeBand_ = idx;
  scanAmbigs_ ->setEnabled(session_->bands().at(activeBand_)->isAttr(SgVlbiBand::Attr_HAS_AMBIGS));
  resetAmbigs_->setEnabled(session_->bands().at(activeBand_)->isAttr(SgVlbiBand::Attr_HAS_AMBIGS));
  if (config_->getIsActiveBandFollowsTab())
  {
    config_->setActiveBandIdx(activeBand_);
    emit activeBandChanged(activeBand_);
  };
};



//
void NsSessionEditDialog::changePrimaryBand(int idx)
{
  if (session_)
  {
    session_->setPrimaryBandByIdx(idx);
    logger->write(SgLogger::INF, SgLogger::SESSION, className() +
      ": changePrimaryBand(): the primary band switched to " + session_->primaryBand()->getKey() +
      "-band");
  };
};



//
void NsSessionEditDialog::dispatchChangeOfYAxis(int n)
{
  if (config_->getIsObservableFollowsPlot() && 
      config_->getUseRateType()==SgTaskConfig::VR_NONE &&       // do not switch if rates are "in"
      (n==BPI_RES_SBD || n==BPI_RES_GRD || n==BPI_RES_PHD)  )
  {
    switch (n)
    {
    default:
    case BPI_RES_SBD:
      config_->setUseDelayType(SgTaskConfig::VD_SB_DELAY);
      break;
    case BPI_RES_GRD:
      config_->setUseDelayType(SgTaskConfig::VD_GRP_DELAY);
      break;
    case BPI_RES_PHD:
      config_->setUseDelayType(SgTaskConfig::VD_PHS_DELAY);
      break;
    };

    for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    {
      SgPlot                   *plot=plots4Baselines_.at(iBand);
      if (iBand != activeBand_)
        plot->changeYaxisTemp2(n);
    };
    emit delayTypeChanged(config_->getUseDelayType());
  };
};



//
void NsSessionEditDialog::dispatchChangeOfYAxis(SgTaskConfig::VlbiDelayType delType)
{
  int                           n;
  switch (delType)
  {
  default:
  case SgTaskConfig::VD_NONE:
    n = -1;
    break;
  case SgTaskConfig::VD_SB_DELAY:
    n = BPI_RES_SBD;
    break;
  case SgTaskConfig::VD_GRP_DELAY:
    n = BPI_RES_GRD;
    break;
  case SgTaskConfig::VD_PHS_DELAY:
    n = BPI_RES_PHD;
    break;
  };
  if (n >= 0)
    for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    {
      SgPlot                   *plot=plots4Baselines_.at(iBand);
      plot->changeYaxisTemp2(n);
    };
};



//
QWidget* NsSessionEditDialog::tab4StationsInfo()
{
  return (stationsList_ = new SgGuiVlbiStationList(OBM_SESSION, session_->getName(),
    &session_->stationsByName(), session_->getTStart(), this));
};



//
QWidget* NsSessionEditDialog::tab4BaselinesInfo()
{
  return (baselinesList_ = new SgGuiVlbiBaselineList(session_, config_, OBM_SESSION,
    &session_->baselinesByName(), this));
};



//
QWidget* NsSessionEditDialog::tab4SourcesInfo()
{
  return (sourcesList_ = new SgGuiVlbiSourceList(OBM_SESSION, session_->getName(), 
    &session_->sourcesByName(), this));
};



//
void NsSessionEditDialog::process(/*bool isNeedUpdatePlots*/)
{
  bool                           isNeedUpdatePlots=true;
  //
  // for people from USNO:
  int                           numOfConstrains=0;
  for (StationsByName_it it=session_->stationsByName().begin();
        it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    if (  si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) &&
         !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)          )
      numOfConstrains++;
  };
  if (numOfConstrains==0)
    QMessageBox::warning(this, 
      "No reference clock station", "No station is selected as a reference clock one.");
  // end of p.f.u.
  //
  blockInput();
  // run it:
  session_->process(true, true);
  //
  processHasBeenCalled_ = true;
  if (isNeedUpdatePlots)
  {
    updateResiduals();
    emit dataChanged();
  };
  restoreInput();
  //
  QString               str;
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgVlbiBand                 *band=session_->bands().at(iBand);
    labelsPrc_ .at(iBand)->setText(str.sprintf("%3d/%3d",  
      band->observables().count(), band->numProcessed(DT_DELAY)));
    labelsPar_ .at(iBand)->setText(str.sprintf("%d/%d",
      session_->getNumOfParameters(), session_->getNumOfConstraints()));
    labelsWRMS_.at(iBand)->setText(str.sprintf("%.1f", band->wrms(DT_DELAY)*scale4Delay_*1.0E3));
    labelsSig0_.at(iBand)->setText(str.sprintf("%.1f", band->getSigma2add(DT_DELAY)*scale4Delay_*1.0E3));
    labelsChi2_.at(iBand)->setText(str.sprintf("%.2f", 
      session_->getNumOfDOF()>0.0?band->chi2(DT_DELAY)/session_->getNumOfDOF():0.0));
  };
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::process_2times()
{
  process();
  process();
};



//
void NsSessionEditDialog::process_3times()
{
  process();
  process();
  process();
};


//
void NsSessionEditDialog::process_4macro()
{
  processRmOutButton();
  process_3times();
};



// 
void NsSessionEditDialog::updateResiduals()
{
  SgPlot                       *plot=NULL;
  SgPlotCarrier                *carrier=NULL;
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgVlbiBand                 *band=session_->bands().at(iBand);
    //
    plot    = plots4Baselines_.at(iBand);
    carrier = plotCarriers4Baselines_.at(iBand);
    //
    QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=band->baselinesByName().constBegin();
    for (int iBaseline=0; it!=band->baselinesByName().constEnd(); ++it, iBaseline++)
    {
      SgVlbiBaselineInfo*         baselineInfo=it.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
//        if (iBand == config_->getActiveBandIdx())
//          baselineInfo->calculateClockF1(config_);

        QList<SgVlbiObservable*> *observables=&baselineInfo->observables();
        SgPlotBranch             *branch=carrier->listOfBranches()->at(iBaseline);
        
        QList<SgVlbiObservable*>::const_iterator jt=observables->constBegin();
        for (int idx=0; jt!=observables->constEnd(); ++jt, idx++)
        {
          SgVlbiObservable       *o=*jt;
//        if (!o->owner()->isAttr(SgObservation::Attr_PROCESSED))
          if (!o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
            branch->addDataAttr(idx, SgPlotCarrier::DA_REJECTED);
          else
            branch->delDataAttr(idx, SgPlotCarrier::DA_REJECTED);
          if (o->isUsable())
            branch->delDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
          else
            branch->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
          branch->data()->setElement(idx, BPI_RES_SBD, o->sbDelay().getResidual()*scale4Delay_);
          branch->data()->setElement(idx, BPI_RES_GRD, o->grDelay().getResidual()*scale4Delay_);
          branch->data()->setElement(idx, BPI_RES_PHD, o->phDelay().getResidual()*scale4Delay_);
          branch->data()->setElement(idx, BPI_RES_PHR, o->phDRate().getResidual()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_DEL_APPLIED,
                                                            o->grDelay().sigma2Apply()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_RAT_APPLIED,
                                                            o->phDRate().sigma2Apply()*scale4Rate_);
//        branch->data()->setElement(idx, BPI_NORM_RESID,   o->activeMeasurement()->getResidualNorm());
          branch->data()->setElement(idx, BPI_RES_NORM_DEL, o->activeDelay()?
                                                            o->activeDelay()->getResidualNorm():0.0);
          branch->data()->setElement(idx, BPI_RES_NORM_RAT, o->phDRate().getResidualNorm());
          branch->data()->setElement(idx, BPI_AMBIG_GR_NUMBER, 
                                                            o->grDelay().getNumOfAmbiguities());
          branch->data()->setElement(idx, BPI_AMBIG_PH_NUMBER, 
                                                            o->phDelay().getNumOfAmbiguities());
          //branch->data()->setElement(idx, BPI_GRD_SBD,     (o->grDelay().getValue() +
          //        o->grDelay().ambiguity() +
          //        o->grDelay().getIonoValue() - o->sbDelay().getValue())*scale4Delay_);

          branch->data()->setElement(idx, BPI_TAUS,          o->getTauS()*scale4Delay_);
//        branch->data()->setElement(idx, BPI_TEST,          o->getTest()*scale4Delay_);
//          branch->data()->setElement(idx, BPI_TEST,          o->owner()->getBaselineClock_F1()*scale4Delay_);
//          branch->data()->setElement(idx, BPI_TEST2,         o->getTest2()*scale4Delay_);
          branch->data()->setElement(idx, BPI_TEST2,         o->getTest2());
//          branch->data()->setElement(idx, BPI_TEST2,         o->owner()->getBaselineClock_F2()*scale4Delay_);
        };
      };
    };
    plot->dataContentChanged();
    labelsSig0_.at(iBand)->setText(QString("").
      sprintf("%.2f", band->getSigma2add(DT_DELAY)*scale4Delay_*1.0E3));
  };
  // plus estimated parameters:
  QMap<QString, SgVlbiAuxObservation*>       *auxObservationsByScan=NULL;
  int                                         stnIdx;
  plot = plot4Stations_;
  carrier = plotCarrier4Stations_;
  //
  QMap<QString, SgVlbiStationInfo*>::const_iterator it=session_->stationsByName().constBegin();
  for (stnIdx=0; it!=session_->stationsByName().constEnd(); ++it, stnIdx++)
  {
    SgVlbiStationInfo          *stationInfo=it.value();
    SgPlotBranch               *branch=carrier->listOfBranches()->at(stnIdx);
    auxObservationsByScan = stationInfo->auxObservationByScanId();
      
    QMap<QString, SgVlbiAuxObservation*>::const_iterator jt=auxObservationsByScan->constBegin();
    for (int idx=0; jt!=auxObservationsByScan->constEnd(); ++jt, idx++)
    {
      SgVlbiAuxObservation   *auxObs=jt.value();
      if (!auxObs->isAttr(SgVlbiAuxObservation::Attr_PROCESSED))
        branch->addDataAttr(idx, SgPlotCarrier::DA_REJECTED);
      else
        branch->delDataAttr(idx, SgPlotCarrier::DA_REJECTED);
/*
      if (auxObs->isUsable())
        branch->delDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
      else
        branch->addDataAttr(idx, SgPlotCarrier::DA_NONUSABLE);
*/
      branch->data()->setElement(idx, SPI_EST_CLK + cableCalsOffset_, 
        auxObs->getEstClocks());
      branch->data()->setElement(idx, SPI_EST_WZD + cableCalsOffset_, 
        auxObs->getEstZenithDelay()*1.0e10/vLight);
      branch->data()->setElement(idx, SPI_EST_GRN + cableCalsOffset_, 
        auxObs->getEstAtmGradN());
      branch->data()->setElement(idx, SPI_EST_GRE + cableCalsOffset_, 
        auxObs->getEstAtmGradE());
      branch->data()->setElement(idx, SPI_TOT_ZDEL + cableCalsOffset_,
        (auxObs->getZenithDelayH() + auxObs->getZenithDelayW())*100.0 + auxObs->getEstZenithDelay());
      branch->data()->setElement(idx, SPI_SIG_EST_CLK + cableCalsOffset_,
        auxObs->getEstClocksSigma());
      branch->data()->setElement(idx, SPI_SIG_EST_WZD + cableCalsOffset_, 
        auxObs->getEstZenithDelaySigma()*1.0e10/vLight);
      branch->data()->setElement(idx, SPI_SIG_EST_GRN + cableCalsOffset_,
        auxObs->getEstAtmGradNSigma());
      branch->data()->setElement(idx, SPI_SIG_EST_GRE + cableCalsOffset_,
        auxObs->getEstAtmGradESigma());
      branch->data()->setElement(idx, SPI_SIG_TOT_ZDEL + cableCalsOffset_,
        auxObs->getEstZenithDelaySigma());
      
      SgVlbiObservation      *obs=auxObs->getOwner();
      SgVlbiObservable       *o=obs?obs->activeObs():NULL;
//    if (o && obs->isAttr(SgObservation::Attr_PROCESSED))
      if (o && o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        branch->data()->setElement(idx, SPI_RES_GRD + cableCalsOffset_,
          o->grDelay().getResidual()*scale4Delay_*1.0e3);
        branch->data()->setElement(idx, SPI_SIG_GRD + cableCalsOffset_,
          o->grDelay().sigma2Apply()*scale4Delay_*1.0e3);
      }
      else
      {
        branch->data()->setElement(idx, SPI_RES_GRD + cableCalsOffset_,    0.0);
        branch->data()->setElement(idx, SPI_SIG_GRD + cableCalsOffset_,    0.0);
      };
    };
  };
  plot->dataContentChanged();
  //
  //
  updateSessionWideSolutions();
};



//
void NsSessionEditDialog::updateSessionWideSolutions()
{
  SgPlot                       *plot=plot4Session_;
  SgPlotCarrier                *carrier=plotCarrier4Session_;
  QString                       keyUt1("EOP: dUT1, ms");
  QString                       keyPmx("EOP: P_x, mas");
  QString                       keyPmy("EOP: P_y, mas");
  const double                  scale4Ut1(parametersDescriptor_->getPolusUT1().getScale());
  const double                  scale4Pm(parametersDescriptor_->getPolusXY().getScale());
  bool                          isTmp;

  SgPlotBranch                 *brArc=carrier->listOfBranches()->at(2);
  SgPlotBranch                 *brPwl=carrier->listOfBranches()->at(3);
  SgPlotBranch                 *brStc=carrier->listOfBranches()->at(4);

  
  // 4Arc:
  SgArcStorage                 *arcUt1=NULL, *arcPmx=NULL, *arcPmy=NULL;
  // 4PWL:
  SgPwlStorage                 *pwlUt1=NULL, *pwlPmx=NULL, *pwlPmy=NULL;
  // 4Stoch:
  const QMap<QString, SgParameter*>
                               *ut1ByEpoch=NULL, *pmxByEpoch=NULL, *pmyByEpoch=NULL;
  SgParameter                  *par;
  

  // Arc:
  if (reporter_->arcByName().contains(keyUt1))
    arcUt1 = reporter_->arcByName().value(keyUt1);
  if (reporter_->arcByName().contains(keyPmx))
    arcPmx = reporter_->arcByName().value(keyPmx);
  if (reporter_->arcByName().contains(keyPmy))
    arcPmy = reporter_->arcByName().value(keyPmy);

  // PWL:
  if (reporter_->pwlByName().contains(keyUt1))
    pwlUt1 = reporter_->pwlByName().value(keyUt1);
  if (reporter_->pwlByName().contains(keyPmx))
    pwlPmx = reporter_->pwlByName().value(keyPmx);
  if (reporter_->pwlByName().contains(keyPmy))
    pwlPmy = reporter_->pwlByName().value(keyPmy);

  // Stoch:
  if (reporter_->stcParByName()->contains(keyUt1))
    ut1ByEpoch = &(*reporter_->stcParByName())[keyUt1];
  if (reporter_->stcParByName()->contains(keyPmx))
    pmxByEpoch = &(*reporter_->stcParByName())[keyPmx];
  if (reporter_->stcParByName()->contains(keyPmy))
    pmyByEpoch = &(*reporter_->stcParByName())[keyPmy];
  

  for (int i=0; i<session_->scanEpochs().size(); i++)
  {
    const SgMJD                *pt=session_->scanEpochs().at(i);
    QString                     timeTag(pt->toString(SgMJD::F_INTERNAL));

    // ARC:
    isTmp = false;
    if (arcUt1)
    {
      brArc->data()->setElement(i, SNI_EST_UT1, arcUt1->calcSolution(*pt)*scale4Ut1);
      brArc->data()->setElement(i, SNI_SIG_UT1, arcUt1->calcSigma(*pt)*scale4Ut1);
      brArc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
      isTmp = true;
    }
    else
    {
      brArc->data()->setElement(i, SNI_EST_UT1, 0.0);
      brArc->data()->setElement(i, SNI_SIG_UT1, 0.0);
      brArc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (arcPmx)
    {
      brArc->data()->setElement(i, SNI_EST_PMX, arcPmx->calcSolution(*pt)*scale4Pm);
      brArc->data()->setElement(i, SNI_SIG_PMX, arcPmx->calcSigma(*pt)*scale4Pm);
      brArc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brArc->data()->setElement(i, SNI_EST_PMX, 0.0);
      brArc->data()->setElement(i, SNI_SIG_PMX, 0.0);
      brArc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (arcPmy)
    {
      brArc->data()->setElement(i, SNI_EST_PMY, arcPmy->calcSolution(*pt)*scale4Pm);
      brArc->data()->setElement(i, SNI_SIG_PMY, arcPmy->calcSigma(*pt)*scale4Pm);
      brArc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brArc->data()->setElement(i, SNI_EST_PMY, 0.0);
      brArc->data()->setElement(i, SNI_SIG_PMY, 0.0);
      brArc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    if (isTmp)
      brArc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);


    // PWL:
    isTmp = false;
    if (pwlUt1)
    {
      brPwl->data()->setElement(i, SNI_EST_UT1, pwlUt1->calcSolution (*pt)*scale4Ut1);
      brPwl->data()->setElement(i, SNI_SIG_UT1, pwlUt1->calcRateSigma(*pt)*scale4Ut1);
      brPwl->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
      isTmp = true;
    }
    else
    {
      brPwl->data()->setElement(i, SNI_EST_UT1, 0.0);
      brPwl->data()->setElement(i, SNI_SIG_UT1, 0.0);
      brPwl->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (pwlPmx)
    {
      brPwl->data()->setElement(i, SNI_EST_PMX, pwlPmx->calcSolution (*pt)*scale4Pm);
      brPwl->data()->setElement(i, SNI_SIG_PMX, pwlPmx->calcRateSigma(*pt)*scale4Pm);
      brPwl->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brPwl->data()->setElement(i, SNI_EST_PMX, 0.0);
      brPwl->data()->setElement(i, SNI_SIG_PMX, 0.0);
      brPwl->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (pwlPmy)
    {
      brPwl->data()->setElement(i, SNI_EST_PMY, pwlPmy->calcSolution (*pt)*scale4Pm);
      brPwl->data()->setElement(i, SNI_SIG_PMY, pwlPmy->calcRateSigma(*pt)*scale4Pm);
      brPwl->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brPwl->data()->setElement(i, SNI_EST_PMY, 0.0);
      brPwl->data()->setElement(i, SNI_SIG_PMY, 0.0);
      brPwl->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    if (isTmp)
      brPwl->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);


    // Stoch:
    isTmp = false;
    if (ut1ByEpoch && ut1ByEpoch->contains(timeTag) && (par=ut1ByEpoch->value(timeTag)))
    {
      brStc->data()->setElement(i, SNI_EST_UT1, par->getSolution()*scale4Ut1);
      brStc->data()->setElement(i, SNI_SIG_UT1, par->getSigma()*scale4Ut1);
      brStc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
      isTmp = true;
    }
    else
    {
      brStc->data()->setElement(i, SNI_EST_UT1, 0.0);
      brStc->data()->setElement(i, SNI_SIG_UT1, 0.0);
      brStc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (pmxByEpoch && pmxByEpoch->contains(timeTag) && (par=pmxByEpoch->value(timeTag)))
    {
      brStc->data()->setElement(i, SNI_EST_PMX, par->getSolution()*scale4Pm);
      brStc->data()->setElement(i, SNI_SIG_PMX, par->getSigma()*scale4Pm);
      brStc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brStc->data()->setElement(i, SNI_EST_PMX, 0.0);
      brStc->data()->setElement(i, SNI_SIG_PMX, 0.0);
      brStc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    //
    if (pmyByEpoch && pmyByEpoch->contains(timeTag) && (par=pmyByEpoch->value(timeTag)))
    {
      brStc->data()->setElement(i, SNI_EST_PMY, par->getSolution()*scale4Pm);
      brStc->data()->setElement(i, SNI_SIG_PMY, par->getSigma()*scale4Pm);
      brStc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    }
    else
    {
      brStc->data()->setElement(i, SNI_EST_PMY, 0.0);
      brStc->data()->setElement(i, SNI_SIG_PMY, 0.0);
      brStc->addDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
    };
    if (isTmp)
      brStc->delDataAttr(i, SgPlotCarrier::DA_NONUSABLE);
  };

  plot->dataContentChanged();
};



//
void NsSessionEditDialog::calcIono()
{
  if (config_->getIsNoviceUser() && !session_->isAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED))
  {
    if (QMessageBox::warning(this, "Novice User Warning",
        "It looks like ambiguities are not resolved for the session yet."
        "Do you want to evaluate ionosphere corrections?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return;
  };
  //
  session_->calculateIonoCorrections(config_);
  // replot:
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgPlot          *plot    = plots4Baselines_.at(iBand);
    SgPlotCarrier   *carrier = plotCarriers4Baselines_.at(iBand);
    SgVlbiBand      *band    = session_->bands().at(iBand);

    QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
    for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
    {
      SgVlbiBaselineInfo* baselineInfo = i.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
        SgPlotBranch *branch = carrier->listOfBranches()->at(iBaseline);
        
        QList<SgVlbiObservable*>::const_iterator j = observables->constBegin();
        for (int idx=0; j!=observables->constEnd(); ++j, idx++)
        {
          SgVlbiObservable    *o=*j;
          branch->data()->setElement(idx, BPI_ION_SBD, o->sbDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_GRD, o->grDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHD, o->phDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHR, o->phDRate().getIonoValue()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_DEL_APPLIED,
                                                            o->grDelay().sigma2Apply()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_RAT_APPLIED,
                                                            o->phDRate().sigma2Apply()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_ION_SBD,  o->sbDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_GRD,  o->grDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHD,  o->phDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHR,  o->phDRate().getIonoSigma()*scale4Rate_);
//        branch->data()->setElement(idx, BPI_NORM_RESID,   o->activeMeasurement()->getResidualNorm());
          branch->data()->setElement(idx, BPI_RES_NORM_DEL, o->activeDelay()?
                                                            o->activeDelay()->getResidualNorm():0.0);
          branch->data()->setElement(idx, BPI_RES_NORM_RAT, o->phDRate().getResidualNorm());
          //branch->data()->setElement(idx, BPI_S_DIFF, o->getSbdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_X_DIFF, o->getGrdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_R_DIFF, o->getPhrDiffBand());
        };
      };
    };
    plot->dataContentChanged();
  };
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
  ionoC_->setEnabled(false);
  iono0_->setEnabled(true);
};



//
void NsSessionEditDialog::zeroIono()
{
  //
  session_->zerofyIonoCorrections(config_);
  // replot:
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgPlot          *plot    = plots4Baselines_.at(iBand);
    SgPlotCarrier   *carrier = plotCarriers4Baselines_.at(iBand);
    SgVlbiBand      *band    = session_->bands().at(iBand);

    QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
    for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
    {
      SgVlbiBaselineInfo* baselineInfo = i.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
        SgPlotBranch *branch = carrier->listOfBranches()->at(iBaseline);
        
        QList<SgVlbiObservable*>::const_iterator j = observables->constBegin();
        for (int idx=0; j!=observables->constEnd(); ++j, idx++)
        {
          SgVlbiObservable     *o=*j;
          branch->data()->setElement(idx, BPI_ION_SBD, o->sbDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_GRD, o->grDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHD, o->phDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHR, o->phDRate().getIonoValue()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_DEL_APPLIED,
                                                            o->grDelay().sigma2Apply()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_RAT_APPLIED,
                                                            o->phDRate().sigma2Apply()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_ION_SBD,  o->sbDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_GRD,  o->grDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHD,  o->phDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHR,  o->phDRate().getIonoSigma()*scale4Rate_);
//        branch->data()->setElement(idx, BPI_NORM_RESID,   o->activeMeasurement()->getResidualNorm());
          branch->data()->setElement(idx, BPI_RES_NORM_DEL, o->activeDelay()?
                                                            o->activeDelay()->getResidualNorm():0.0);
          branch->data()->setElement(idx, BPI_RES_NORM_RAT, o->phDRate().getResidualNorm());
          //branch->data()->setElement(idx, BPI_S_DIFF, o->getSbdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_X_DIFF, o->getGrdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_R_DIFF, o->getPhrDiffBand());
        };
      };
    };
    plot->dataContentChanged();
  };
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
  ionoC_->setEnabled(true);
  iono0_->setEnabled(false);
};



//
void NsSessionEditDialog::calcClcF1()
{
  /*
  session_->calculateBaselineClock_F1();
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgPlot          *plot    = plots4Baselines_.at(iBand);
    SgPlotCarrier   *carrier = plotCarriers4Baselines_.at(iBand);
    SgVlbiBand      *band    = session_->bands().at(iBand);

    QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
    for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
    {
      SgVlbiBaselineInfo* baselineInfo = i.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
        SgPlotBranch *branch = carrier->listOfBranches()->at(iBaseline);
        
        QList<SgVlbiObservable*>::const_iterator j = observables->constBegin();
        for (int idx=0; j!=observables->constEnd(); ++j, idx++)
        {
          SgVlbiObservable     *o=*j;
          branch->data()->setElement(idx, BPI_CLK_1,  o->owner()->getBaselineClock_F1() *scale4Delay_);
          branch->data()->setElement(idx, BPI_CLK_1L, o->owner()->getBaselineClock_F1l()*scale4Delay_);
          branch->data()->setElement(idx, BPI_CLK_1R, o->owner()->getBaselineClock_F1r()*scale4Delay_);
        };
      };
    };
    plot->dataContentChanged();
  };
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
  */
};



//
void NsSessionEditDialog::dispatchUserKeyOld(SgPlot* plotter, Qt::KeyboardModifiers modifiers, int key)
{
  switch (modifiers)
  {
  //
  // Ctrl+
  case Qt::ControlModifier:
    switch (key)
    {
    case Qt::Key_B:
      processClockBreakMarksNew(plotter);
      // std::cout << "processed New\n";
      break;
    case Qt::Key_F:
      generateRequest4Refringing(plotter);
      // std::cout << "processed New\n";
      break;
//    case Qt::Key_M:
//      adjustAmbiguityMultipliers();
      break;
    case Qt::Key_T:
      processTestFunctionN1(plotter);
      break;
    case Qt::Key_U:
      toggleMarkedPoints(plotter);
      break;
    case Qt::Key_X:
      excludeMarkedPoints(plotter);
      break;
    case Qt::Key_Y:
      includeMarkedPoints(plotter);
      break;
    default:
      break;
    };
    break;
  //
  // Shift+
  case Qt::ShiftModifier:
    switch (key)
    {
    case Qt::Key_U:
      toggleMarkedPoints(plotter);
      break;
    case Qt::Key_X:
      unimmunizeMarkedPoints(plotter);
      break;
    case Qt::Key_Y:
      immunizeMarkedPoints(plotter);
      break;
    default:
      break;
    };
    break;
  //
  // Alt+
  case Qt::AltModifier:
    switch (key)
    {
    case Qt::Key_B:
      processClockBreakMarks(plotter);
      // std::cout << "processed Old\n";
      break;
    default:
      break;
    };
    break;

  case Qt::ControlModifier | Qt::ShiftModifier:
    switch (key)
    {
    case Qt::Key_X:
      excludeAndImmunizeMarkedPoints(plotter);
      break;
    case Qt::Key_Y:
      includeAndUnimmunizeMarkedPoints(plotter);
      break;
    default:
      break;
    };
    break;

  default:
  case Qt::NoModifier:
    switch (key)
    {
    case Qt::Key_Equal:
      changeNumOfAmbigSpacing4MarkedPoints(plotter,  1);
      break;
    case Qt::Key_Minus:
      changeNumOfAmbigSpacing4MarkedPoints(plotter, -1);
      break;
    case Qt::Key_Plus:
      changeNumOfSubAmbig4MarkedPoints(plotter,  1);
      break;
    case Qt::Key_Underscore:
      changeNumOfSubAmbig4MarkedPoints(plotter, -1);
      break;
    default:
      break;
    };
    break;
  };
  
  if (logger->isEligible(SgLogger::DBG, SgLogger::RESERVED_1))
		std::cout << "Dispatch: key= " << key << ", modifiers=" << modifiers << "\n";
};



//
void NsSessionEditDialog::dispatchUserKey(SgPlot* plotter, Qt::KeyboardModifiers modifiers, int key)
{

  switch (key)
  {
  case Qt::Key_B:
		if (modifiers == Qt::ControlModifier)
			processClockBreakMarksNew(plotter);
		else if (modifiers == Qt::AltModifier)
			processClockBreakMarks(plotter);
    break;

  case Qt::Key_F:
		if (modifiers == Qt::ControlModifier)
			generateRequest4Refringing(plotter);
    break;

  case Qt::Key_T:
    if (modifiers == Qt::ControlModifier)
			processTestFunctionN1(plotter);
    break;

  case Qt::Key_U:
		if (modifiers == Qt::ControlModifier ||
				modifiers == Qt::ShiftModifier   )
			toggleMarkedPoints(plotter);
    break;


  case Qt::Key_X:
		if (modifiers == Qt::ControlModifier)
			excludeMarkedPoints(plotter);
		else if (modifiers == Qt::ShiftModifier)
			unimmunizeMarkedPoints(plotter);
		else if (modifiers == (Qt::ControlModifier | Qt::ShiftModifier))
			excludeAndImmunizeMarkedPoints(plotter);
    break;


  case Qt::Key_Y:
		if (modifiers == Qt::ControlModifier)
			includeMarkedPoints(plotter);
		else if (modifiers == Qt::ShiftModifier)
			immunizeMarkedPoints(plotter);
		else if (modifiers == (Qt::ControlModifier | Qt::ShiftModifier))
			includeAndUnimmunizeMarkedPoints(plotter);
		break;

  case Qt::Key_Equal:
  case Qt::Key_Plus:
  case Qt::Key_AsciiCircum:
		changeNumOfAmbigSpacing4MarkedPoints(plotter,  1);
    break;

  case Qt::Key_Minus:
		if (modifiers == Qt::ShiftModifier)
			changeNumOfAmbigSpacing4MarkedPoints(plotter,  1);
		else 
			changeNumOfAmbigSpacing4MarkedPoints(plotter, -1);
		break;
  case Qt::Key_Underscore:
    changeNumOfAmbigSpacing4MarkedPoints(plotter, -1);
    break;

  default:
		break;
  };

  if (logger->isEligible(SgLogger::DBG, SgLogger::RESERVED_1))
		std::cout << "Dispatch: key= " << key << ", modifiers=" << modifiers << "\n";
};



//
void NsSessionEditDialog::excludeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier							   *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand *band = session_->bands().at(iBand);
      BaselinesByName_it i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo* baselineInfo = i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
          SgPlotBranch *branch = actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator j = observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
              // change observation attribute
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
							branch->addDataAttr(idx, SgPlotCarrier::DA_REJECTED);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::includeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand          	   *band=session_->bands().at(iBand);
      BaselinesByName_it 				i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo		 *baselineInfo=i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> 
															 *observables=&baselineInfo->observables();
          SgPlotBranch 			   *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator 
																j=observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
							// change observation attribute:
							SgVlbiObservation*obs=o->owner();
							obs->eradicateAttr(SgVlbiMeasurement::Attr_NOT_VALID, config_->getUseDelayType());
							// turn on baseline and station(s) if they were deselected:
							if (obs->baseline()->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
								obs->baseline()->delAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
							if (obs->stn_1()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
								obs->stn_1()->delAttr(SgVlbiStationInfo::Attr_NOT_VALID);
							if (obs->stn_2()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
								obs->stn_2()->delAttr(SgVlbiStationInfo::Attr_NOT_VALID);
							
							branch->delDataAttr(idx, SgPlotCarrier::DA_REJECTED);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::toggleMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand          	   *band=session_->bands().at(iBand);
      BaselinesByName_it 				i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo		 *baselineInfo=i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> 
															 *observables=&baselineInfo->observables();
          SgPlotBranch 			   *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator 
																j=observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
							if (o->isUsable())
							{
								if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SPECIAL_01)
								{
									o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
									o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
									branch->delDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
								}
								else
								{
									o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
									o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
									branch->addDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
								};
							};
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::unimmunizeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand          	   *band=session_->bands().at(iBand);
      BaselinesByName_it 				i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo		 *baselineInfo=i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> 
															 *observables=&baselineInfo->observables();
          SgPlotBranch 			   *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator 
																j=observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if ((branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED) && o->isUsable())
            {
							o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
							o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::immunizeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand          	   *band=session_->bands().at(iBand);
      BaselinesByName_it 				i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo		 *baselineInfo=i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> 
															 *observables=&baselineInfo->observables();
          SgPlotBranch 			   *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator 
																j=observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if ((branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED) && o->isUsable())
            {
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
							branch->addDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::excludeAndImmunizeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier							   *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand *band = session_->bands().at(iBand);
      BaselinesByName_it i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo* baselineInfo = i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
          SgPlotBranch *branch = actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator j = observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
              // change observation attribute
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
							branch->addDataAttr(idx, SgPlotCarrier::DA_REJECTED);
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
							o->activeMeasurement()->addAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
							branch->addDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
              branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::includeAndUnimmunizeMarkedPoints(SgPlot* plotter)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool 													isDataModified=false;
  
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      SgVlbiBand          	   *band=session_->bands().at(iBand);
      BaselinesByName_it 				i=band->baselinesByName().begin();
      for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
      {
        SgVlbiBaselineInfo		 *baselineInfo=i.value();
        if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          QList<SgVlbiObservable*> 
															 *observables=&baselineInfo->observables();
          SgPlotBranch 			   *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
          QList<SgVlbiObservable*>::iterator 
																j=observables->begin();
          for (int idx=0; j!=observables->end(); ++j, idx++)
          {
            SgVlbiObservable   *o=*j;
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
							// change observation attribute:
							SgVlbiObservation*obs=o->owner();
							obs->eradicateAttr(SgVlbiMeasurement::Attr_NOT_VALID, config_->getUseDelayType());
							// turn on baseline and station(s) if they were deselected:
							if (obs->baseline()->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
								obs->baseline()->delAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
							if (obs->stn_1()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
								obs->stn_1()->delAttr(SgVlbiStationInfo::Attr_NOT_VALID);
							if (obs->stn_2()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
								obs->stn_2()->delAttr(SgVlbiStationInfo::Attr_NOT_VALID);

							o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL);
							o->activeMeasurement()->delAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL);
							branch->delDataAttr(idx, SgPlotCarrier::DA_REJECTED);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SPECIAL_01);
							branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
							isDataModified = true;
            };
          };
        };
      };
      plotter->dataContentChanged();
      if (isDataModified)
				session_->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
    };
  //emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};






//
void NsSessionEditDialog::generateRequest4Refringing(SgPlot* plotter)
{
  if (plotter->getNumOfYColumn() != BPI_RES_GRD)
    return;
  if (session_->isAttr(SgVlbiSessionInfo::Attr_FF_ION_C_CALCULATED))
    QMessageBox::warning(this, 
      "Unable to proceed",
      "Cannot create the list of observations for refringing. Turn off ionosphere corrections and repeat");
  //
  //
  QList<SgVlbiObservable*>      obsList;
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  //
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier)       // points to the same object
    {
      SgVlbiBand               *band=session_->bands().at(iBand);
      QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=band->baselinesByName().constBegin();
      for (int iBaseline=0; it!=band->baselinesByName().constEnd(); ++it, iBaseline++)
      {
        SgVlbiBaselineInfo     *baselineInfo=it.value();
        if (baselineInfo)
        {
          QList<SgVlbiObservable*>
                               *observables=&baselineInfo->observables();
          SgPlotBranch          *branch=actualCarrier->listOfBranches()->at(iBaseline);
          for (int idx=0; idx<observables->size(); idx++)
          {
            SgVlbiObservable   *o=observables->at(idx);
            if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
            {
              obsList << o;
              branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
            };
          };
        };
      };
      plotter->dataContentChanged();
      //
    };
  
  if (!obsList.size())
    return;
  
  QString                       bandKey(obsList.at(0)->getBandKey());
  QString                       str(setup.path2(setup.getPath2SpoolFileOutput()));
  QDir                          d(str);
  bool                          isOk;
  if (!d.exists())
  {
//  isOk = d.mkpath("./");
    isOk = d.mkpath(d.absolutePath());
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::generateRequest4Refringing(): cannot create directory " + str);
      return;
    };
  };
  //
  QString         fileName(session_->getName());
  if (fileName.at(0) == '$')
    fileName.remove(0, 1);
  fileName +=  ".b_" + bandKey + ".4FourFit";
  QFile                         f(str + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::generateRequest4Refringing(): error opening output file: " + str + "/" + fileName);
    return;
  };

  double                         dInterval=1.5e-9; // 1.5 ns
  double                         wantedResid;
  if (bandKey == "S")
    dInterval = 3e-9; // was: 4ns
  
  QTextStream                   s(&f);
  QString 											sPol("");
  SgVlbiBand                   *band;
  SgVlbiStationInfo						 *stn1;
  SgVlbiStationInfo						 *stn2;
  for (int i=0; i<obsList.size(); i++)
  {
    SgVlbiObservable           *o=obsList.at(i);
    if (o && o->getBandKey() == bandKey)
    {
			sPol = "";
			if (session_->bandByKey().contains(bandKey))
			{
				band = session_->bandByKey().value(bandKey);
				stn1 = band->stationsByName().value(o->owner()->stn_1()->getKey());
				stn2 = band->stationsByName().value(o->owner()->stn_2()->getKey());
				if (stn1 && stn2)
				{
					if (stn1->getNetPolarizations()&SgVlbiStationInfo::NP_CircPolarizRight &&
							stn2->getNetPolarizations()&SgVlbiStationInfo::NP_CircPolarizRight  ) // nothing to do
						sPol = "";
					else if (stn1->getNetPolarizations()&SgVlbiStationInfo::NP_CircPolarizRight    &&
										 (stn2->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizX ||
											stn2->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizY  ) )
						sPol = "-PRX+RY";
					else if ((stn1->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizX ||
										stn1->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizY)    &&
					 				 stn2->getNetPolarizations()&SgVlbiStationInfo::NP_CircPolarizRight 	  )
						sPol = "-PXR+YR";
					else if ((stn1->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizX ||
										stn1->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizY)    &&
 									 (stn2->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizX ||
										stn2->getNetPolarizations()&SgVlbiStationInfo::NP_LinearPolarizY)     )
						sPol = "-PI";
					else
						logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
							"::generateRequest4Refringing(): unexpected polarization setup at " + bandKey + 
							"-band: station1= " + netPolarizations2string(stn1->getNetPolarizations()) +
							", station2= " + netPolarizations2string(stn2->getNetPolarizations()));
				}
				else
					logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
							"::generateRequest4Refringing(): stn1 or stn2 is NULL");
			}
			else
				logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
					"::generateRequest4Refringing(): cannot find band \"" + bandKey + "\" in the session");
			
			wantedResid = o->grDelay().getResidualFringeFitting() - 
				(o->grDelay().getResidual() /*+ o->grDelay().ambiguity()*/);
			if (wantedResid < -o->grDelay().getAmbiguitySpacing()/2.0)
				wantedResid += o->grDelay().getAmbiguitySpacing();
			if (o->grDelay().getAmbiguitySpacing()/2.0 < wantedResid)
				wantedResid -= o->grDelay().getAmbiguitySpacing();

			str = "fourfit -c " + o->getFourfitControlFile() + 
				" -b " + o->owner()->stn_1()->getCid() + o->owner()->stn_2()->getCid() + ":" + o->getBandKey() + 
				" " + sPol + (sPol.size()?" ":"") +
				o->owner()->getScanName() + " set mb_win " +
				QString("").sprintf("%.6f %.6f", (wantedResid - dInterval)*1.0e6, (wantedResid + dInterval)*1.0e6);

			s << str << "\n";
		};
  };
  //
  s.setDevice(NULL);
  f.close();
  obsList.clear();

  logger->write(SgLogger::INF, SgLogger::REPORT, className() +
    "::generateRequest4Refringing(): the file \"" + f.fileName() + "\" has been created");
};



//
void NsSessionEditDialog::changeNumOfAmbigSpacing4MarkedPoints(SgPlot* plotter, int inc)
{
  if (config_->getIsNoviceUser() && session_->isAttr(SgVlbiSessionInfo::Attr_HAS_IONO_CORR))
  {
    if (QMessageBox::warning(this, "Novice User Warning",
        "The ionosphere corrections are evaluated and taken into account."
        "Modifications of ambiguity multiplier numbers will lead to unexpected results."
        "Do you want to continue?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return;
  };

  SgPlotCarrier                *actualCarrier=plotter->carrier();
  bool                          isDataModified(false);
  //
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier)       // points to the same object
    {
      // group delays:
      if (plotter->getNumOfYColumn()==BPI_RES_GRD         ||      // this is a group delay residuals
          plotter->getNumOfYColumn()==BPI_AMBIG_GR_NUMBER         // or ambig numbers
          //|| plotter->getNumOfYColumn()==BPI_GRD_SBD
          )
      {
        SgVlbiBand             *band=session_->bands().at(iBand);
        QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
        for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
        {
          SgVlbiBaselineInfo* baselineInfo = i.value();
          if (baselineInfo)
          {
            QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
            SgPlotBranch *branch = actualCarrier->listOfBranches()->at(iBaseline);
        
            QList<SgVlbiObservable*>::iterator j = observables->begin();
            for (int idx=0; j!=observables->end(); ++j, idx++)
            {
              SgVlbiObservable   *o=*j;
              if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
              {
                // change observation attribute
                o->grDelay().setNumOfAmbiguities(o->grDelay().getNumOfAmbiguities() + inc);
//-->           // adjust residual (can be used somewhere else before a user will make another iteration)
                o->grDelay().setResidual(o->grDelay().getResidual() + 
                  o->grDelay().getAmbiguitySpacing()*inc);
                // adjust plot data
                branch->data()->setElement(idx, BPI_RES_GRD,
                                branch->data()->getElement(idx, BPI_RES_GRD) +
                                o->grDelay().getAmbiguitySpacing()*inc*scale4Delay_);
                branch->data()->setElement(idx, BPI_AMBIG_GR_NUMBER, o->grDelay().getNumOfAmbiguities());
                //branch->data()->setElement(idx, BPI_GRD_SBD,
                //              ( o->grDelay().getValue() +
                //                o->grDelay().ambiguity() +
                //                o->grDelay().getIonoValue() - o->sbDelay().getValue() )*scale4Delay_);
                branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
                isDataModified = true;
                o->owner()->session()->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
              };
            };
          };
        };
        plotter->dataContentChanged();
      };
      //
      // phase delays:
      if (plotter->getNumOfYColumn()==BPI_RES_PHD         ||      // this is a group delay residuals
          plotter->getNumOfYColumn()==BPI_AMBIG_PH_NUMBER  )      // or ambig numbers
      {
        SgVlbiBand             *band=session_->bands().at(iBand);
        QMap<QString, SgVlbiBaselineInfo*>::const_iterator it=band->baselinesByName().constBegin();
        for (int iBaseline=0; it!=band->baselinesByName().constEnd(); ++it, iBaseline++)
        {
          SgVlbiBaselineInfo   *baselineInfo=it.value();
          if (baselineInfo)
          {
            QList<SgVlbiObservable*> *observables=&baselineInfo->observables();
            SgPlotBranch       *branch=actualCarrier->listOfBranches()->at(iBaseline);
        
            QList<SgVlbiObservable*>::iterator j=observables->begin();
            for (int idx=0; j!=observables->end(); ++j, idx++)
            {
              SgVlbiObservable   *o=*j;
              if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
              {
                // change observation attribute
                o->phDelay().setNumOfAmbiguities(o->phDelay().getNumOfAmbiguities() + inc);
                // adjust plot data
                branch->data()->setElement(idx, BPI_RES_PHD,
                                branch->data()->getElement(idx, BPI_RES_PHD) +
                                o->phDelay().getAmbiguitySpacing()*inc*scale4Delay_);
                branch->data()->setElement(idx, BPI_AMBIG_PH_NUMBER, o->phDelay().getNumOfAmbiguities());
                branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
                isDataModified = true;
              };
            };
          };
        };
        plotter->dataContentChanged();
      };
      //
    };
  //emit dataChanged();
  if (isDataModified)
    session_->addAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED);
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::changeNumOfSubAmbig4MarkedPoints(SgPlot* plotter, int inc)
{
  SgPlotCarrier                *actualCarrier=plotter->carrier();
//  bool                          isDataModified(false);
  //
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier)       // points to the same object
    {
      // group delays only:
      if (plotter->getNumOfYColumn()==BPI_RES_GRD)
      {
        SgVlbiBand             *band=session_->bands().at(iBand);
        QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
        for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
        {
          SgVlbiBaselineInfo* baselineInfo = i.value();
          if (baselineInfo)
          {
            QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
            SgPlotBranch *branch = actualCarrier->listOfBranches()->at(iBaseline);
        
            QList<SgVlbiObservable*>::iterator j = observables->begin();
            for (int idx=0; j!=observables->end(); ++j, idx++)
            {
              SgVlbiObservable   *o=*j;
              if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
              {
                // change observation attribute
                o->grDelay().setNumOfSubAmbigs(o->grDelay().getNumOfSubAmbigs() + inc);
                // adjust plot data
                branch->data()->setElement(idx, BPI_RES_GRD,
                                branch->data()->getElement(idx, BPI_RES_GRD) +
                                o->grDelay().getSubAmbigSpacing()*inc*scale4Delay_);
                branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
//                isDataModified = true;
                o->owner()->session()->addAttr(SgVlbiSession::Attr_FF_EDIT_INFO_MODIFIED);
              };
            };
          };
        };
        plotter->dataContentChanged();
      };
    };
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
bool NsSessionEditDialog::findClockBreakEvent(SgPlot* plotter,
  QList<SgVlbiBaselineInfo*>& markedBaselines, SgVlbiBand*& band,
  SgMJD& tBreak, SgVlbiStationInfo*& stnBreak)
{
  SgPlotCarrier            *actualCarrier=plotter->carrier();
  QList<SgVlbiObservable*>  obsAtBreak;
  QList<SgVlbiObservable*>  obsNextToBreak;
  bool                      isOK=false;

  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
    {
      band = session_->bands().at(iBand);
      isOK = true;
    };

  if (!isOK)
    return isOK;
  
  BaselinesByName_it i=band->baselinesByName().begin();
  for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
  {
    SgVlbiBaselineInfo* baselineInfo = i.value();
    if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
    {
      //----------  collect marked points:
      QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
      SgPlotBranch *branch = actualCarrier->listOfBranches()->at(iBaseline);
      bool      hasMatched=false;
      for (int idx=observables->size()-2; idx>=0; idx--)
        if (branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED)
        {
          if (!hasMatched)
          {
            hasMatched = true;
            SgVlbiObservable   *o=observables->at(idx);
            markedBaselines.append(baselineInfo);
            int j=idx+1;
            while (j<observables->size() && branch->getDataAttr(j) & SgPlotCarrier::DA_REJECTED)
              j++;
            if (j<observables->size() && !(branch->getDataAttr(j) & SgPlotCarrier::DA_REJECTED))
            {
              SgVlbiObservable *obsNext=observables->at(j);
              obsAtBreak.append(o);
              obsNextToBreak.append(obsNext);
            };
          };
          // discard selection marks:
          branch->delDataAttr(idx, SgPlotCarrier::DA_SELECTED);
        };
    };
  };
  // perform some checks:
  if (!obsAtBreak.size()) // that's ok, user just occasionally pressed keys, nothing to do
    return false;
  if (obsAtBreak.size() != obsNextToBreak.size())
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": findClockBreakEvent(): number of points in the aux lists are not equal: " +
      QString("").sprintf("%d vs %d",  obsAtBreak.size(), obsNextToBreak.size()));
    return false;
  };
  if (obsAtBreak.size() == 1 && band->baselinesByName().size()>1)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      ": findClockBreakEvent(): not enough points to detect a clock break");
    QMessageBox::warning(this, "Clock Breaks detector", "Not enough points to detect a clock break");
    return false;
  };
  // guess a clock break:
  // count frequencies:
  QMap<QString, int>  numByName;
  for (int i=0; i<obsAtBreak.size(); i++)
  {
    numByName[obsAtBreak.at(i)->stn_1()->getKey()]++;
    numByName[obsAtBreak.at(i)->stn_2()->getKey()]++;
  };
  // get maximum:
  QMap<QString, int>::iterator  iAux=numByName.begin();
  int                           minN=iAux.value(), maxN=iAux.value();
  QString                       stnName=iAux.key();
  for (; iAux!=numByName.end(); ++iAux)
  {
    int                   n=iAux.value();
    if (minN>n)
      minN = n;
    if (maxN<n)
    {
      maxN = n;
      stnName = iAux.key();
    };
  };
  // determine proper station and epoch:
  if (maxN!=minN || (maxN==1 && minN==1))
  {
    stnBreak = NULL;
    tBreak = session_->getTStart();
    if (session_->stationsByName().contains(stnName))
    {
      SgVlbiObservable         *obsBreak=NULL;
      stnBreak = session_->stationsByName().value(stnName);
      for (int i=0; i<obsAtBreak.size(); i++)
        if (tBreak < obsAtBreak.at(i)->epoch())
        {
          obsBreak = obsAtBreak.at(i);
          tBreak = obsBreak->epoch();
        };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
        ": findClockBreakEvent(): stnName " + stnName + " is not in session_->stationsByName()");
      isOK = false;
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      ": findClockBreakEvent(): impossible to guess the station name: maxN==minN");
    isOK = false;
  };
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
  return isOK;
};



//
void NsSessionEditDialog::processClockBreakMarks(SgPlot* plotter)
{
  SgMJD                       tBreak=tZero;
  QList<SgVlbiBaselineInfo*>  markedBaselines;
  SgVlbiStationInfo          *stnBreak=NULL;
  SgVlbiBand                 *band=NULL;

  // parse user marks, determine station and epoch of the break (plus some aux variables):
  if (!findClockBreakEvent(plotter, markedBaselines, band, tBreak, stnBreak))
    return;
  if (!markedBaselines.size() || !band || tBreak==tZero || !stnBreak)
    return;

  // determine parameters of the break:
  double        shift, sigma;
  session_->calculateClockBreakParameter(tBreak, stnBreak, band, shift, sigma);
  if (config_->getUseDelayType()==SgTaskConfig::VD_SB_DELAY)
    shift  = round(1.0E9*shift);            // round to ns
  else if ( config_->getUseDelayType()==SgTaskConfig::VD_GRP_DELAY ||
            config_->getUseDelayType()==SgTaskConfig::VD_PHS_DELAY  )
    shift  = round(1.0E12*shift)*1.0E-3;    // round to ps
  sigma *= 1.0E9;

  if (fabs(shift) > 3.0*sigma) // 3sigma threshold (otherwise, do it by hands):
  {
    // ok, let user know
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": processClockBreakMarks(): detected clock break at " + stnBreak->getKey() +
      " station on " + tBreak.toString() + "; the shift is " +
      QString("").sprintf("%.4f ns", shift));
    // adjust station information concerning the clock break:
    session_->correctClockBreak(band, stnBreak, tBreak, shift, sigma, true);
    // replot residuals:
    updateResiduals();
    // emit dataChanged();
  }
  else // just print out what was found:
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": processClockBreakMarks(): here could be a clock break at " + stnBreak->getKey() +
      " station on " + tBreak.toString() + "; however, its value is small, about " +
      QString("").sprintf("%.4f (+/- %.4f) ns", shift, sigma));
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::processClockBreakMarksNew(SgPlot* plotter)
{
  SgMJD                       tBreak=tZero;
  QList<SgVlbiBaselineInfo*>  markedBaselines;
  SgVlbiStationInfo          *stnBreak=NULL;
  SgVlbiBand                 *band=NULL;
  SgClockBreakAgent          *cbAgent=NULL;
  QList<SgClockBreakAgent*>   cbAgents;

  // collect previously detected clock breaks:
  QMap<QString, SgVlbiStationInfo*>       *stationsByName;
  if (!config_->getUseDynamicClockBreaks() && 
    (plotter->getNumOfYColumn()==BPI_RES_GRD || plotter->getNumOfYColumn()==BPI_RES_PHD) )
    stationsByName = &session_->bands().at(activeBand_)->stationsByName();
  else
    stationsByName = &session_->stationsByName();
  StationsByName_it            it=stationsByName->begin();
  for (int idx=0; it!=stationsByName->end(); ++it, idx++)
  {
    SgVlbiStationInfo          *stn=it.value();
    SgBreakModel               &breakModel=stn->clockBreaks();
    for (int brkIdx=0; brkIdx<breakModel.size(); brkIdx++)
    {
      SgParameterBreak         *brk=breakModel.at(brkIdx);
      cbAgent = new SgClockBreakAgent;
      cbAgent->epoch_ = *brk;
      cbAgent->stn_   = stn;
      cbAgents.append(cbAgent);
    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": processClockBreakMarksNew(): got " + QString("").setNum(cbAgents.size()) + 
    " entries in cbAgents");

  // parse user marks, determine station and epoch of the break (plus some aux variables):
  if (findClockBreakEvent(plotter, markedBaselines, band, tBreak, stnBreak) &&
      markedBaselines.size() && band && tBreak!=tZero && stnBreak )
  {
    // first, check for clock break, it may be already in the list:
    bool                        doesExist=false;
    for (int i=0; i<cbAgents.size(); i++)
    {
      cbAgent = cbAgents.at(i);
      if (fabs(cbAgent->epoch_ - tBreak)<5.5E-6 && cbAgent->stn_->getKey()==stnBreak->getKey())
        doesExist = true;
    };
    // if it is a new entry, append it:
    if (!doesExist)
    {
      cbAgent = new SgClockBreakAgent;
      cbAgent->epoch_ = tBreak;
      cbAgent->stn_   = stnBreak;
      cbAgents.append(cbAgent);
    };
  }; // All in all it's just another break in the clocks.

  if (!cbAgents.size()) // nothing to evaluate, perhaps, a user pressed the keys by accident.
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": processClockBreakMarksNew(): the size of clock break agents list is zero, nothing to do");
    return;
  };
  
  // determine parameters of the breaks:
  session_->calculateClockBreaksParameters(cbAgents, band);
  
  //  std::cout << "Evaluated clock breaks:\n";
  for (int idx=0; idx<cbAgents.size(); idx++)
  {
    cbAgent = cbAgents.at(idx);
    double                      shift, sigma;
    tBreak = cbAgent->epoch_;
    stnBreak = cbAgent->stn_;
    shift = cbAgent->shift_;
    sigma = cbAgent->sigma_;

    if (config_->getUseDelayType()==SgTaskConfig::VD_SB_DELAY)
      shift  = round(1.0E9*shift);            // round to ns
    else if ( config_->getUseDelayType()==SgTaskConfig::VD_GRP_DELAY ||
              config_->getUseDelayType()==SgTaskConfig::VD_PHS_DELAY  )
      shift  = round(1.0E12*shift)*1.0E-3;    // round to ps
    sigma *= 1.0E9;
    //
    //    std::cout << qPrintable(stnBreak->getKey() + " " + tBreak.toString() ) << "  "
    //              << shift << "  +/-" << sigma << "\n";
    //
    if (fabs(shift) > 3.0*sigma) // 3sigma threshold (otherwise, do it by hands):
    {
      // ok, let user know
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        ": processClockBreakMarksNew(): detected clock break at " + stnBreak->getKey() +
        " station on " + tBreak.toString() + "; shift on " +
        QString("").sprintf("%.4f ns", shift));
      // adjust station information concerning the clock break:
      session_->correctClockBreak(band, stnBreak, tBreak, shift, sigma, true);
      // replot residuals:
      updateResiduals();
    }
    else // just print out what was found:
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        ": processClockBreakMarksNew(): here could be a clock break at " + stnBreak->getKey() +
        " station on " + tBreak.toString() + "; however, its value is small, about " +
        QString("").sprintf("%.4f (+/- %.4f) ns", shift, sigma));
    // free the allocated memory:
    delete cbAgent;
  };
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::processTestFunctionN1(SgPlot* plotter)
{
  SgVlbiBand                   *band=NULL;
  SgPlotCarrier                *actualCarrier=plotter->carrier();
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
    if (plotCarriers4Baselines_.at(iBand) == actualCarrier) // points to the same object
      band = session_->bands().at(iBand);
  if (!band)
    return;

  QString         							stnName("");
  QMultiMap<QString, SgMJD>     candidatesByBln;
  SgMJD           							tBreak(tZero);
  if (session_->detectClockBreaks_mode1(band, stnName, tBreak, candidatesByBln))
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      ": processTestFunctionN1(): detected clock break at " +
      stnName + " on " + tBreak.toString());
  };
};



//
void NsSessionEditDialog::adjustAmbiguityMultipliers()
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": adjustAmbiguityMultipliers(): starting adjusting of ambiguity multipliers");

  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgVlbiBand           *band=session_->bands().at(iBand);
    BaselinesByName_it    i=band->baselinesByName().begin();
    for (int iBaseline=0; i!=band->baselinesByName().end(); ++i, iBaseline++)
    {
      SgVlbiBaselineInfo* baselineInfo=i.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QList<SgVlbiObservable*> *observables=&baselineInfo->observables();
        for (int idx=0; idx<observables->size(); idx++)
        {
          SgVlbiObservable     *o=observables->at(idx);
          double                diff=o->sbDelay().getResidual() + o->sbDelay().getIonoValue() -
              ( o->grDelay().getResidual() + o->grDelay().ambiguity() +
                o->grDelay().getIonoValue());
          o->grDelay().setNumOfAmbiguities(
            o->grDelay().getNumOfAmbiguities() + rint(diff/o->grDelay().getAmbiguitySpacing()));
        };
      };
    };
  };
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();

};



//
void NsSessionEditDialog::scanAmbiguityMultipliers()
{
  if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
    return;

  if (config_->getIsNoviceUser() && session_->isAttr(SgVlbiSessionInfo::Attr_HAS_IONO_CORR))
  {
    if (QMessageBox::warning(this, "Novice User Warning",
        "The ionosphere corrections are evaluated and taken into account."
        "Modifications of ambiguity multiplier numbers will lead to unexpected results."
        "Do you want to continue?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
      return;
  };

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": scanAmbiguityMultipliers(): starting scanning of ambiguity multipliers");

  blockInput();
  session_->scanBaselines4GrDelayAmbiguities(activeBand_);
  updateResiduals();
  restoreInput();
  emit dataChanged();
  session_->addAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED);
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::resetAmbiguityMultipliers()
{
  if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
    return;

  blockInput();
  session_->resetDelayAmbiguities(activeBand_, config_->getUseDelayType());
  updateResiduals();
  restoreInput();
  emit dataChanged();
  session_->delAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED);
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::resetAmbiguityMultipliers(): ambiguity multipliers were reset for " +
    session_->bands().at(activeBand_)->getKey() + "-band");
};



//
void NsSessionEditDialog::detectAndProcessClockBreaks()
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": detectAndProcessClockBreaks(): clock break correction procedure initiated");
  
  session_->checkBandForClockBreaks_part1(activeBand_, true);
  
  session_->checkBandForClockBreaks_part2(activeBand_, true);

  updateResiduals();
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::processRmOutButton()
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": processRmOutButton(): outliers detect procedure initiated");

  // show "wait" cursor:
  blockInput();

  if (config_->getOpAction() == SgTaskConfig::OPA_ELIMINATE)
  {
    if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
      session_->eliminateOutliersSimpleMode(activeBand_, 1, 5);
    else
      session_->eliminateOutliers(activeBand_);
  }
  else
    session_->restoreOutliers(activeBand_);
  
  updateResiduals();
  emit dataChanged();
  // restore cursor:
  restoreInput();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::clearAllEditings()
{
  if (QMessageBox::warning(this, "Warning", 
    "Are you sure to wipe out all changes you made for the session?",  
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
    return;

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    ": clearAllEditings(): starting clear of all edit information");
  
  zeroIono();
  session_->resetAllEditings(true);

  updateResiduals();
  configWidget_->displayParametersDesriptor();
  configWidget_->displayConfig();
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::clearAuxSigmas()
{
  if (session_)
    session_->zerofySigma2add();
  emit dataChanged();
  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
    saveIntermediateResults();
};



//
void NsSessionEditDialog::displayPointInfo(SgPlot*, SgPlotBranch* branch, int idx, 
	int xColumn, int yColumn, SgPlot::QueryMode qm)
{
	if (qm == SgPlot::QM_Mode0)
		displayPointInfo_new(branch, idx);
	else if (qm == SgPlot::QM_Mode1)
		displayPointInfo_old(branch, idx);
};



//
void NsSessionEditDialog::displayPointInfo_old(SgPlotBranch* branch, int idx)
{
  SgVlbiBand                   *band=session_->bands().at(activeBand_);
  QString                       blName=branch->getName();
  SgVlbiBaselineInfo           *baseline=NULL;
  if (band->baselinesByName().contains(blName))
  {
    baseline = band->baselinesByName().find(blName).value();
    SgVlbiObservable          *o=baseline->observables().at(idx);
    if (o)
    {
      QString                   str2copy, str2copyBlName(""), strQCs(""), strSnrs(""), strECs("");
      QString                   str4usability(""), str4nonProc("");
      QString                   strChanInfoIds(""), strChanInfoFrq("");
      QString                   strChanInfoP1(""), strChanInfoP2("");
      QString                   blnStat(""), srcStat(""), stn1Stat(""), stn2Stat("");
      QString 									strHeader("");
      QString 									strBody("");

      if (session_->baselinesByName().size()>1)
        str2copyBlName = " " + o->baseline()->getKey() + ",";
      str2copy.sprintf("observation %d,%s %s, %s, which fits at  %.2f +/- %.2f ps",
        o->getMediaIdx()+1, qPrintable(str2copyBlName), 
        qPrintable(o->src()->getKey()), qPrintable(o->epoch().toString(SgMJD::F_HHMMSS)),
        o->activeDelay()->getResidual()*scale4Delay_*1.0e3,
        o->activeDelay()->sigma2Apply()*scale4Delay_*1.0e3);
 
      SgVlbiObservation        *obs=o->owner();
      for (QMap<QString, SgVlbiObservable*>::iterator it_obs=obs->observableByKey().begin();
        it_obs!=obs->observableByKey().end(); ++it_obs)
      {
        strQCs += it_obs.key() + ":" + QString("").setNum(it_obs.value()->getQualityFactor()) + ", ";
        strSnrs+= it_obs.key() + ":" + QString("").sprintf("%.2g", it_obs.value()->getSnr()) + ", ";
        strECs += it_obs.key() + ":\"" + it_obs.value()->getErrorCode() + "\", ";
      };
      if (strQCs.size()>1)
        strQCs = strQCs.left(strQCs.size() - 2);
      if (strSnrs.size()>1)
        strSnrs = strSnrs.left(strSnrs.size() - 2);
      if (strECs.size()>1)
        strECs = strECs.left(strECs.size() - 2);
      
      str4usability = "";
      if (!o->isUsable())
      {
				if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_LOW_QF))
          str4usability += "Low quality factor. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_HAS_ERROR_CODE))
          str4usability += "Error code is set. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_SOURCE))
          str4usability += "Deselected source. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_STATION))
          str4usability += "Deselected station. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_BASELINE))
          str4usability += "Deselected baseline. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_UNMATED))
          str4usability += "Not mated. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_ONE_CHANNEL))
          str4usability += "Not enough used channels. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_LOW_QF))
          str4usability += "Low quality factor on another band. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_HAS_ERROR_CODE))
          str4usability += "Error code is set on another band. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_ONE_CHANNEL))
          str4usability += "Not enough used channels on another band. ";
        str4usability.chop(1);
        str4usability = "<dt>Reason(s) of non-usability:</dt><dd>" + str4usability + "</dd>";
      };
      str4nonProc = "";
      if (!o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED) && o->isUsable())
      {
				if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_OBS))
          str4nonProc += "Deselected observation. ";
        if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_DESELECTED_OBS))
          str4nonProc += "Deselected observation on another band. ";
        str4nonProc.chop(1);
        str4nonProc = "<dt>Reason(s) for non-processing:</dt><dd>" + str4nonProc + "</dd>";
      };

      blnStat.sprintf("<small><ul><li>Numbers (T/U/P): %d/%d/%d<li>WRMS %.2fps</ul></small>", 
        o->baseline()->numTotal(DT_DELAY), 
        o->baseline()->numUsable(DT_DELAY), 
        o->baseline()->numProcessed(DT_DELAY),
        o->baseline()->wrms(DT_DELAY)*1.0e12);
      srcStat.sprintf("<small><ul><li>Numbers (T/U/P): %d/%d/%d<li>WRMS %.2fps</ul></small>", 
        o->src()->numTotal(DT_DELAY), 
        o->src()->numUsable(DT_DELAY), 
        o->src()->numProcessed(DT_DELAY),
        o->src()->wrms(DT_DELAY)*1.0e12);
      stn1Stat.sprintf("<small><ul><li>Numbers (T/U/P): %d/%d/%d<li>WRMS %.2fps"
        "<li>Azimuth %.1f(degree)<li>Elevation %.1f(degree)</ul></small>", 
        o->stn_1()->numTotal(DT_DELAY), 
        o->stn_1()->numUsable(DT_DELAY), 
        o->stn_1()->numProcessed(DT_DELAY),
        o->stn_1()->wrms(DT_DELAY)*1.0e12,
        obs->auxObs_1()->getAzimuthAngle()*RAD2DEG,
        obs->auxObs_1()->getElevationAngle()*RAD2DEG);
      stn2Stat.sprintf("<small><ul><li>Numbers (T/U/P): %d/%d/%d<li>WRMS %.2fps"
        "<li>Azimuth %.1f(degree)<li>Elevation %.1f(degree)</ul></small>",
        o->stn_2()->numTotal(DT_DELAY), 
        o->stn_2()->numUsable(DT_DELAY), 
        o->stn_2()->numProcessed(DT_DELAY),
        o->stn_2()->wrms(DT_DELAY)*1.0e12,
        obs->auxObs_2()->getAzimuthAngle()*RAD2DEG,
        obs->auxObs_2()->getElevationAngle()*RAD2DEG);
      
      for (int i=0; i<o->getNumOfChannels(); i++)
      {
				strChanInfoFrq += QString("").sprintf("<td align=\"center\">%.2f</td>", 
					o->refFreqByChan()->getElement(i));
				strChanInfoIds += QString("").sprintf("<td align=\"center\">%c</td>",  
					o->chanIdByChan()->at(i));
				strChanInfoP1 += QString("").sprintf("<td align=\"center\">%c</td>",   
					o->polarization_1ByChan()->at(i));
				strChanInfoP2 += QString("").sprintf("<td align=\"center\">%c</td>",  
					o->polarization_2ByChan()->at(i));
			};

      //
      strHeader = "Observation Info";
      strBody 	= "<p><b>Inquired observation</b></p><p>You have selected an observation of " +
        o->src()->getKey() + " radio source on the baseline " +
        o->baseline()->getKey() + ". Its attributes are:"
        "<ul>"
        "<li>baseline: <b>" + o->baseline()->getKey() + "</b>" + blnStat +
        "<li>radio source: <b>" + o->src()->getKey() + "</b>" + srcStat +
        "<li>reference station: <b>" + o->stn_1()->getKey() + "</b>" + stn1Stat +
        "<li>remote station: <b>" + o->stn_2()->getKey() + "</b>" + stn2Stat +
        "<li>band: <b>" + o->getBandKey() + "</b>" +
        "<li>observation # " + QString("").setNum(o->getMediaIdx()) +
        ", baseline idx: " + QString("").setNum(idx) + "</li>" +
        "<li>epoch: " + o->epoch().toString() + "</li>" +
        "<li>scan name: " + obs->getScanName() + "</li>" +
        "<li>fourfit file name: " + o->getFourfitOutputFName() + "</li>" +
        "<li>smaple rate: " + QString("").sprintf("%.1f MHz", o->getSampleRate()*1.0e-6) + "</li>" +
        "<li>num of channels: " + QString("").setNum(o->getNumOfChannels()) + "</li>" +
        "<li>" + o->activeDelay()->getName() + 
        QString("").sprintf(" residual: %.2f +/- %.2fps (applied: %.2fps) </li>",
          o->activeDelay()->getResidual()*scale4Delay_*1.0e3, 
          o->activeDelay()->getSigma()*scale4Delay_*1.0e3,
          o->activeDelay()->sigma2Apply()*scale4Delay_*1.0e3) + 
        "<li>Attributes:<dl>"
        "<dt>Quality code(s):</dt><dd>{" + strQCs + "}</dd>"
        "<dt>SNR(s):</dt><dd>{" + strSnrs + "}</dd>"
        "<dt>Channels (N=" + QString("").setNum(o->getNumOfChannels()) + "):</dt><dd>"
				"<table align=\"left\" border=\"1\">"
				"<tr>" + strChanInfoIds + "</tr>"
				"<tr>" + strChanInfoFrq + "</tr>"
				"<tr>" + strChanInfoP1  + "</tr>"
				"<tr>" + strChanInfoP2  + "</tr>"
				"</table></dd>" 
        "<dt>Ambiguity spacing:</dt><dd>" + 
					QString("").setNum(o->grDelay().getAmbiguitySpacing()*1.0e9) + " (ns)</dd>"
        "<dt>Usable:</dt><dd>" + (o->isUsable()?QString("<b>Yes</b>"):QString("No")) + "</dd>" + 
        str4usability + 
        "<dt>Excluded:</dt><dd>" + 
        (o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID)?
        "<b>Yes</b>":"No") + "</dd>" +
        "<dt>Processed:</dt><dd>" + 
        (o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)?
        //obs->isAttr(SgVlbiObservation::Attr_PROCESSED)?
        "<b>Yes</b>":"No") + "</dd>" +
        str4nonProc +
				//
        "<dt>Fringe error code(s):</dt><dd>{" + strECs + "}</dd></dl></ul></p><p>"
        "Cut and copy string for reports:<br>" + 
        str2copy + "</p>";


      QDialog               	 *dialog = new QDialog(this);
			QPushButton   					 *button;
			QBoxLayout    		 			 *layout, *subLayout;
			QLabel									 *label;
			layout = new QVBoxLayout(dialog);
			label  = new QLabel(strBody, dialog);
			layout->addWidget(label);

			subLayout = new QHBoxLayout();
			layout->addLayout(subLayout);
			subLayout->addStretch(1);
  
			subLayout->addWidget(button=new QPushButton("Close", dialog));
			button->setMinimumSize(button->sizeHint());
			connect(button, SIGNAL(clicked()), dialog, SLOT(accept()));

      dialog->setWindowModality(Qt::NonModal);
      dialog->setModal(false);

			dialog->setWindowTitle(strHeader);
      dialog->show();
  
/*      
      int 											pimaAutoSup=obs->getPimaAutoSup();
      int 											pimaUserSup=obs->getPimaUserSup();
//    int 											pimaUserRec=obs->getPimaUserRec();
			if (pimaAutoSup)
			{
				std::cout << "Obs: " << qPrintable(o->strId()) << "  pimaAutoSup=" << pimaAutoSup << ":\n";
				for (QMap<PimaAutoSuppress, QString>::const_iterator it=pasAbbrByType.constBegin(); 
					it!=pasAbbrByType.constEnd(); ++it)
					{
						if (it.key() & pimaAutoSup)
							std::cout << " " << qPrintable(it.value()) << " ";
					};
					std::cout << "\n";
			};
			if (pimaUserSup)
			{
				std::cout << "Obs: " << qPrintable(o->strId()) << "  pimaUserSup=" << pimaUserSup << ":\n";
				for (QMap<PimaSolutionType, QString>::const_iterator it=pstAbbrByType.constBegin(); 
					it!=pstAbbrByType.constEnd(); ++it)
					{
						if (it.key() & pimaUserSup)
							std::cout << " " << qPrintable(it.value()) << " ";
					};
					std::cout << "\n";
			};
*/
    };
  };
};



//
void NsSessionEditDialog::displayPointInfo_new(SgPlotBranch* branch, int idx)
{
  SgVlbiBand                   *band=session_->bands().at(activeBand_);
  QString                       blName=branch->getName();
  SgVlbiBaselineInfo           *baseline=NULL;
  if (band->baselinesByName().contains(blName))
  {
    baseline = band->baselinesByName().find(blName).value();
    SgVlbiObservable          *o=baseline->observables().at(idx);
    if (o && o->owner())
    {
			NsBrowseObservation			 *boWindow = new NsBrowseObservation(o->owner(), this);
			boWindow->show();
		}
    else
    {
      if (!o)
        std::cout << "displayPointInfo_new: o is NULL\n";
      else if (!o->owner())
        std::cout << "displayPointInfo_new: o->owner() is NULL\n";
    }
	}
  else 
    std::cout << "displayPointInfo_new:  !band->baselinesByName().contains(blName)\n";
};



//
void NsSessionEditDialog::saveIntermediateResults()
{
  QString                       dirName=setup.path2(setup.getPath2IntermediateResults());
  bool                          isOk(true);
  SgMJD                         startEpoch(SgMJD::currentMJD());
  // check for existance:
  QDir                          d(dirName);
  if (!d.exists())
  {
//  isOk = d.mkpath("./"); // Qt, wtf?
    isOk = d.mkpath(d.absolutePath());
    if (isOk)
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": saveIntermediateResults(): the directory " + dirName + " has been created");
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        ": saveIntermediateResults(): cannot create directory " + dirName + 
        "; saving the intermediate results has failed");
      setup.setLnfsFileName("");
      setup.setLnfsOriginType(SgVlbiSessionInfo::OT_UNKNOWN);
      return;
    };
  };
  if (dirName.size()>0)
    dirName += "/";
  QString                       fileName(session_->name4SirFile(setup.getHave2UpdateCatalog()));
  QFile                         file(dirName + fileName);
  if (!file.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      ": saveIntermediateResults(): cannot open file " + file.fileName() +
      "; saving the intermediate results has failed");
    setup.setLnfsFileName("");
    setup.setLnfsOriginType(SgVlbiSessionInfo::OT_UNKNOWN);
    return;
  };
  QDataStream                   s(&file);
  if ((isOk=session_->saveIntermediateResults(s)))
  {
    SgMJD                       finisEpoch(SgMJD::currentMJD());
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      ": saveIntermediateResults(): the intermediate results have been saved in the file " + 
      file.fileName() + ", elapsed time: " + 
      QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      ": saveIntermediateResults(): saving the intermediate results in the file " + 
      file.fileName() + " has failed");
    setup.setLnfsFileName("");
    setup.setLnfsOriginType(SgVlbiSessionInfo::OT_UNKNOWN);
  };
  s.setDevice(NULL);
  file.close();
  return;
};



//
bool NsSessionEditDialog::loadIntermediateResults(bool need2notify)
{
  QString                       dirName=setup.path2(setup.getPath2IntermediateResults());
  bool                          isOk(true);
  SgMJD                         startEpoch(SgMJD::currentMJD());
  // check for existance:
  QDir                          d(dirName);
  if (!d.exists())
    return false;
  if (dirName.size()>0)
    dirName += "/";
  QString                       fileName(session_->name4SirFile(setup.getHave2UpdateCatalog()));
  QFile                         file(dirName + fileName);
  if (!file.exists())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      ": loadIntermediateResults(): nothing to read, the file " + file.fileName() + " does not exist");
    return false;
  };
  if (!file.open(QIODevice::ReadOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      ": loadIntermediateResults(): cannot read file " + file.fileName() +
      "; loading the intermediate results has failed");
    return false;
  };
  // ok, it is exist and we can read the file. Ask user:
  if (need2notify &&
      QMessageBox::question(this, 
        "Found unsaved data",
        "There are intermediate results for this session, should we load them?\n"
        "If you say No, you'll start a session from the scratch, also the intermediate results "
        "will be overwritten.",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)
  {
    file.close();
    file.remove();
    return false;
  };
  QDataStream                   s(&file);
  if ((isOk=session_->loadIntermediateResults(s)))
  {
    SgMJD                         finisEpoch(SgMJD::currentMJD());
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      ": loadIntermediateResults(): the intermediate results have been loaded from the file " + 
      file.fileName() + ", elapsed time: " + 
      QString("").sprintf("%.2f", (finisEpoch - startEpoch)*86400000.0) + " ms");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      ": loadIntermediateResults(): loading the intermediate results from the file " + 
      file.fileName() + " has failed");
  s.setDevice(NULL);
  file.close();
  return isOk;
};



//
void NsSessionEditDialog::executeTestAction()
{
	session_->writeUserData2File("output.txt"); //--> 
  blockInput();
//-->  session_->eliminateLargeOutliers(activeBand_, 10, 3.0);

/*
  // put here an action to test:
  session_->calculateIonoCorrections(NULL);
  for (int iBand=0; iBand<session_->numberOfBands(); iBand++)
  {
    SgPlot          *plot    = plots4Baselines_.at(iBand);
    SgPlotCarrier   *carrier = plotCarriers4Baselines_.at(iBand);
    SgVlbiBand      *band    = session_->bands().at(iBand);

    QMap<QString, SgVlbiBaselineInfo*>::const_iterator i=band->baselinesByName().constBegin();
    for (int iBaseline=0; i!=band->baselinesByName().constEnd(); ++i, iBaseline++)
    {
      SgVlbiBaselineInfo* baselineInfo = i.value();
      if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QList<SgVlbiObservable*> *observables = &baselineInfo->observables();
        SgPlotBranch *branch = carrier->listOfBranches()->at(iBaseline);
        
        QList<SgVlbiObservable*>::const_iterator j = observables->constBegin();
        for (int idx=0; j!=observables->constEnd(); ++j, idx++)
        {
          SgVlbiObservable     *o=*j;
          branch->data()->setElement(idx, BPI_ION_SBD, o->sbDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_GRD, o->grDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHD, o->phDelay().getIonoValue()*scale4Delay_);
          branch->data()->setElement(idx, BPI_ION_PHR, o->phDRate().getIonoValue()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_DEL_APPLIED,
                                                            o->grDelay().sigma2Apply()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_RAT_APPLIED,
                                                            o->phDRate().sigma2Apply()*scale4Rate_);
          branch->data()->setElement(idx, BPI_SIG_ION_SBD,  o->sbDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_GRD,  o->grDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHD,  o->phDelay().getIonoSigma()*scale4Delay_);
          branch->data()->setElement(idx, BPI_SIG_ION_PHR,  o->phDRate().getIonoSigma()*scale4Rate_);
//        branch->data()->setElement(idx, BPI_NORM_RESID,   o->activeMeasurement()->getResidualNorm());
          branch->data()->setElement(idx, BPI_RES_NORM_DEL, o->activeDelay()?
                                                            o->activeDelay()->getResidualNorm():0.0);
          branch->data()->setElement(idx, BPI_RES_NORM_RAT, o->phDRate().getResidualNorm());
          //branch->data()->setElement(idx, BPI_S_DIFF, o->getSbdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_X_DIFF, o->getGrdDiffBand()*scale4Delay_);
          //branch->data()->setElement(idx, BPI_R_DIFF, o->getPhrDiffBand());
        };
      };
    };
    plot->dataContentChanged();
  };
*/ 


//-->
//-->  emit dataChanged();
//-->
//-->  if (isNeed2SaveSir_ && setup.getAutoSavingMode()==NsSetup::AS_ALWAYS)
//-->    saveIntermediateResults();
//-->
  logger->write(SgLogger::DBG, SgLogger::GUI, className() +
    ": executeTestAction(): done");
  restoreInput();
};



//
//
void NsSessionEditDialog::displayExcludedObs()
{
  // collect info and display it:
  QList<SgVlbiObservation*>     deselectedObs;
  QList<SgVlbiObservation*>     nonUsableObs;
  QList<NsDeselectedObsInfo*>   deselectedObservations;
  QList<NsNotUsedObsInfo*>      nonUsableObservations;
 
  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
		SgVlbiObservable           *o=obs?obs->activeObs():NULL;
		if (o)
		{
			if (!o->isUsable())
				nonUsableObs << obs;
			else if (o->isUsable() && o->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
				deselectedObs << obs;
		};

  };
  QString                       str("");
  for (int i=0; i<deselectedObs.size(); i++)
  {
    QString                     strQCs("");
    SgVlbiObservation          *obs=deselectedObs.at(i);
    SgVlbiObservable           *o=obs->activeObs(); // can be NULL
    if (o)
    {
      for (QMap<QString, SgVlbiObservable*>::iterator it_obs=obs->observableByKey().begin();
        it_obs!=obs->observableByKey().end(); ++it_obs)
        strQCs += it_obs.key() + ":" + QString("").setNum(it_obs.value()->getQualityFactor()) + ",";
      strQCs.chop(1);
      NsDeselectedObsInfo           *obsInfo=new NsDeselectedObsInfo(o->getMediaIdx(), 
        obs->stn_1()->getKey(),  obs->stn_2()->getKey(), obs->src()->getKey(), 
        obs->toString(SgMJD::F_HHMMSS), strQCs, 
        o->activeDelay()->getResidual()*1.0e12,
        o->activeDelay()->sigma2Apply()*1.0e12,
        o->activeDelay()->getResidualNorm()
        );
      deselectedObservations << obsInfo;
    };
  };
  for (int i=0; i<nonUsableObs.size(); i++)
  {
    QString                     strReasons("");
    QString                     strQCs("");
    SgVlbiObservation          *obs=nonUsableObs.at(i);
    SgVlbiObservable           *o=obs->activeObs(); // can be NULL
    if (o)
    {
      for (QMap<QString, SgVlbiObservable*>::iterator it_obs=obs->observableByKey().begin();
        it_obs!=obs->observableByKey().end(); ++it_obs)
        strQCs += it_obs.key() + ":" + QString("").setNum(it_obs.value()->getQualityFactor()) + ",";
      strQCs.chop(1);

      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_LOW_QF))
        strReasons += "Low quality factor. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_SOURCE))
        strReasons += "Deselected source. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_STATION))
        strReasons += "Deselected station. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_BASELINE))
        strReasons += "Deselected baseline. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_UNMATED))
        strReasons += "Not mated. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_ONE_CHANNEL))
        strReasons += "Not enough used channels. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_LOW_QF))
        strReasons += "Low quality factor on another band. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_ONE_CHANNEL))
        strReasons += "Not enough used channels on another band. ";
      strReasons.chop(1);
      
      NsNotUsedObsInfo           *obsInfo=new NsNotUsedObsInfo(o->getMediaIdx(), obs->stn_1()->getKey(),
        obs->stn_2()->getKey(), obs->src()->getKey(), obs->toString(SgMJD::F_HHMMSS), strQCs, 
        strReasons);
    
      nonUsableObservations << obsInfo;
    };
  };
  //
  NsBrowseNotUsedObsDialog     *browser;
  browser = new NsBrowseNotUsedObsDialog(nonUsableObservations, deselectedObservations, reporter_, this);
  browser->setModal(false);
  browser->setSizeGripEnabled(true);
  browser->setWindowModality(Qt::NonModal);
  browser->setWindowFlags(Qt::Window);
  browser->resize(0.8*size());
  browser->show();
  browser->raise();
  browser->activateWindow();
  
};



//
void NsSessionEditDialog::displayExcludedObs_old()
{
  // collect info and diplay it:
  QList<SgVlbiObservation*>     listOfExcludedObs;
  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    if (obs->activeObs() && 
				obs->activeObs()->isUsable() &&
        obs->activeObs()->activeMeasurement()     && 
        obs->activeObs()->activeMeasurement()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
      listOfExcludedObs << obs;
  };
  QString                       lstOfObs("");
  QString                       str2copy("");
  for (int i=0; i<listOfExcludedObs.size(); i++)
  {
    QString                     str2copyBlName(""), strQCs("");
    SgVlbiObservation          *obs=listOfExcludedObs.at(i);
    SgVlbiObservable           *o=obs->primeObs();
    // need to specify a baseline:
    if (session_->baselinesByName().size() > 1)
      str2copyBlName = " " + obs->baseline()->getKey() + ",";
    for (QMap<QString, SgVlbiObservable*>::iterator it_obs=obs->observableByKey().begin();
      it_obs!=obs->observableByKey().end(); ++it_obs)
      strQCs += it_obs.key() + ":" + QString("").setNum(it_obs.value()->getQualityFactor()) + ",";
    if (strQCs.size()>1)
      strQCs = strQCs.left(strQCs.size() - 1);
    str2copy.sprintf("observation %d,%s %s, %s, QC=(%s) which fits at  %.2f +/- %.2f ps",
      o->getMediaIdx()+1, qPrintable(str2copyBlName), 
      qPrintable(o->src()->getKey()), qPrintable(o->epoch().toString(SgMJD::F_HHMMSS)),
      qPrintable(strQCs),
      o->activeDelay()->getResidual()*1.0e12,
      o->activeDelay()->sigma2Apply()*1.0e12);
//    lstOfObs += str2copy + "<br>";
    lstOfObs += str2copy + "\n";
  };
//  QMessageBox                   msgBox(NULL);
  QMessageBox                   msgBox(QMessageBox::Information, "", "", 
    QMessageBox::Ok, NULL, Qt::Window);


  msgBox.setIcon(QMessageBox::Information);
  msgBox.setText("<b>The list of potentially good observations that were excluded from data analysis "
    "for the session " +  session_->getName() + ":</b>");
  msgBox.setInformativeText("Total amount: " + QString("").setNum(listOfExcludedObs.size()));
  msgBox.setDetailedText(
//    "<p>The observations are:</p>"
//    "<p>" + lstOfObs + "</p>");
    "The observations are:\n" + lstOfObs);
  msgBox.setSizeGripEnabled(true);
  msgBox.exec();
};






//
void NsSessionEditDialog::runExternalCommand()
{
  if (setup.getExternalCommand().size() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::runExternalCommand(): no command specified");
    return;
  };
/*
  QFileInfo                     fi(setup.getExternalCommand());
  if (!fi.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::runExternalCommand(): the file \"" + setup.getExternalCommand() + "\" does not exist");
    return;
  };
  if (!fi.isExecutable())
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::runExternalCommand(): the file \"" + setup.getExternalCommand() + "\" is not not executable");
    return;
  };
*/
  int                           rc;
  QString                        command(setup.getExternalCommand());
  command += " '" + session_->getName() + "' '" + 
    session_->getSessionCode() + "' '" + session_->getOfficialName() + "' '" + 
    session_->getNetworkID() + "' '" + setup.identities().getUserDefaultInitials() + "'";

  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::runExternalCommand(): executing the command \"" + command + "\"");

  rc = system(qPrintable(command));
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::runExternalCommand(): the command \"" + command + 
    "\" has been executed with rc=" + QString("").setNum(rc));
};
/*=====================================================================================================*/











/*=====================================================================================================*/
