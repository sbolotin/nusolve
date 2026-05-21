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
#include "NsSetupDialog.h"


#include <SgGuiLoggerConfig.h>

#include <iostream>
#include <stdlib.h>


#include <SgLogger.h>
#include <SgMJD.h>
#include <SgGuiPlotter.h>



#include <QtCore/QDir>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#   include <QtWidgets/QTabWidget>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#   include <QtGui/QRadioButton>
#   include <QtGui/QTabWidget>
#endif






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsSetupDialog::NsSetupDialog(QWidget *parent, Qt::WindowFlags flags) 
  : QDialog(parent, flags)
{
  setWindowTitle("nuSolve Preferences");

  QBoxLayout                   *layout, *subLayout;
  QSize                         btnSize;

  layout = new QVBoxLayout(this);
  
  tabs = new QTabWidget(this);
  tabs->addTab(tab4Pathes(), "&Directories");
  tabs->addTab(tab4Options(), "&Options");
  tabs->addTab(tab4Identities(), "&User Identities");
  tabs->addTab((logConfigWidget_ = new SgGuiLoggerConfig(this)), "&Logger Options");
  
  layout->addWidget(tabs, 10);
  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  
  QPushButton *ok = new QPushButton("OK", this);
  QPushButton *cancel = new QPushButton("Cancel", this);
  QPushButton *apply = new QPushButton("Apply", this);
  ok->setDefault(true);

  cancel->setMinimumSize((btnSize=cancel->sizeHint()));
  apply ->setMinimumSize( btnSize );
  ok    ->setMinimumSize( btnSize );

  subLayout->addStretch(1);
  subLayout->addWidget(apply);
  subLayout->addWidget(ok);
  subLayout->addWidget(cancel);
  setSizeGripEnabled(true);

  connect(apply,  SIGNAL(clicked()), SLOT(apply()));
  connect(ok,     SIGNAL(clicked()), SLOT(accept()));
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
};



NsSetupDialog::~NsSetupDialog()
{
};



//
void NsSetupDialog::apply()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": apply selected");
  acquireData();
};



//
void NsSetupDialog::accept()
{
  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": accept selected");
  acquireData();
  deleteLater();
};



//
void NsSetupDialog::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": reject selected");
  deleteLater();
};



//
QWidget* NsSetupDialog::tab4Pathes()
{
  QGroupBox                    *gBox;
  QBoxLayout                   *mainLayout;
  QLabel                       *label;
  QGridLayout                  *gridLayout;
  QWidget                      *widget=new QWidget(this);
  QString                       str("");

  mainLayout = new QVBoxLayout(widget);
  mainLayout -> addStretch(1);

  gBox = new QGroupBox("Essential paths", widget);
  gridLayout = new QGridLayout(gBox);
  // 
  label = new QLabel("Home (non-absolute paths count from it):", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Home_ = new QLineEdit(gBox);
  lePath2Home_->setText(setup.getPath2Home());
  lePath2Home_->setMinimumSize(lePath2Home_->sizeHint());
  lePath2Home_->setMinimumWidth(lePath2Home_->fontMetrics().width(setup.getPath2Home()));
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(lePath2Home_, 0, 1);
  // 
  label = new QLabel("Executables of Catalog<->nuSolve Interface:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2CatNu_ = new QLineEdit(gBox);
  lePath2CatNu_->setText(setup.getPath2CatNuInterfaceExec());
  lePath2CatNu_->setMinimumSize(lePath2CatNu_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(lePath2CatNu_, 1, 1);
  //
  gridLayout->activate();
  mainLayout->addWidget(gBox);
  //
  gBox = new QGroupBox("Data", widget);
  gridLayout = new QGridLayout(gBox);
  //
  label = new QLabel("Observations (DBH) files:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Dbh_ = new QLineEdit(gBox);
  lePath2Dbh_->setText(setup.getPath2DbhFiles());
  lePath2Dbh_->setMinimumSize(lePath2Dbh_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(lePath2Dbh_, 0, 1);
  //
  label = new QLabel("Observations (vgosDb) files:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2VgosDb_ = new QLineEdit(gBox);
  lePath2VgosDb_->setText(setup.getPath2VgosDbFiles());
  lePath2VgosDb_->setMinimumSize(lePath2VgosDb_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(lePath2VgosDb_, 1, 1);
  //
  label = new QLabel("Observations (vgosDa) files:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2VgosDa_ = new QLineEdit(gBox);
  lePath2VgosDa_->setText(setup.getPath2VgosDaFiles());
  lePath2VgosDa_->setMinimumSize(lePath2VgosDa_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(lePath2VgosDa_, 2, 1);
  //
  label = new QLabel("A priori files:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Apriori_ = new QLineEdit(gBox);
  lePath2Apriori_->setText(setup.getPath2APrioriFiles());
  lePath2Apriori_->setMinimumSize(lePath2Apriori_->sizeHint());
  gridLayout->addWidget(label, 3, 0);
  gridLayout->addWidget(lePath2Apriori_, 3, 1);
  //
  label = new QLabel("TRP files (VMF and other models):", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2TrpFiles_ = new QLineEdit(gBox);
  lePath2TrpFiles_->setText(setup.getPath2TrpFiles());
  lePath2TrpFiles_->setMinimumSize(lePath2TrpFiles_->sizeHint());
  gridLayout->addWidget(label, 4, 0);
  gridLayout->addWidget(lePath2TrpFiles_, 4, 1);
  //
  label = new QLabel("Masterfiles:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Masterfiles_ = new QLineEdit(gBox);
  lePath2Masterfiles_->setText(setup.getPath2MasterFiles());
  lePath2Masterfiles_->setMinimumSize(lePath2Masterfiles_->sizeHint());
  gridLayout->addWidget(label, 5, 0);
  gridLayout->addWidget(lePath2Masterfiles_, 5, 1);

  //
  // compose a string:
  for (int i=0; i<setup.getMasterfileSuffixes().size(); i++)
    str += setup.getMasterfileSuffixes().at(i) + ",";
  str.chop(1);
  cbUseAltMasterfileSuffixes_ = new QCheckBox("Use alternative masterfile extensions", gBox);
  cbUseAltMasterfileSuffixes_->setMinimumSize(cbUseAltMasterfileSuffixes_->sizeHint());
  cbUseAltMasterfileSuffixes_->setCheckState(setup.getUseAltMasterfileSuffixes()?Qt::Checked:Qt::Unchecked);
  //
  leAltMasterfileSuffixes_ = new QLineEdit(gBox);
  leAltMasterfileSuffixes_->setText(str);
  leAltMasterfileSuffixes_->setMinimumSize(leAltMasterfileSuffixes_->sizeHint());
  gridLayout->addWidget(cbUseAltMasterfileSuffixes_,  6, 0);
  gridLayout->addWidget(leAltMasterfileSuffixes_,     6, 1);
  //
  leAltMasterfileSuffixes_->setEnabled(setup.getUseAltMasterfileSuffixes());
  connect(cbUseAltMasterfileSuffixes_, SIGNAL(stateChanged(int)), SLOT(changeAltMasterfileSuffixes(int)));

  //
  gridLayout->activate();
  mainLayout->addWidget(gBox);
  //
  gBox = new QGroupBox("Miscellaneous output", widget);
  gridLayout = new QGridLayout(gBox);
  //
  label = new QLabel("Spoolfile output:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Spoolfiles_ = new QLineEdit(gBox);
  lePath2Spoolfiles_->setText(setup.getPath2SpoolFileOutput());
  lePath2Spoolfiles_->setMinimumSize(lePath2Spoolfiles_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(lePath2Spoolfiles_, 0, 1);
  //
  label = new QLabel("List of not used observations output:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2NotUsedObs_ = new QLineEdit(gBox);
  lePath2NotUsedObs_->setText(setup.getPath2NotUsedObsFileOutput());
  lePath2NotUsedObs_->setMinimumSize(lePath2NotUsedObs_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(lePath2NotUsedObs_, 1, 1);
  //
  label = new QLabel("Reports output:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2Reports_ = new QLineEdit(gBox);
  lePath2Reports_->setText(setup.getPath2ReportOutput());
  lePath2Reports_->setMinimumSize(lePath2Reports_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(lePath2Reports_, 2, 1);
  //
  label = new QLabel("Save observations in NGS format:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2NgsOutput_ = new QLineEdit(gBox);
  lePath2NgsOutput_->setText(setup.getPath2NgsOutput());
  lePath2NgsOutput_->setMinimumSize(lePath2NgsOutput_->sizeHint());
  gridLayout->addWidget(label, 3, 0);
  gridLayout->addWidget(lePath2NgsOutput_, 3, 1);
  //
  label = new QLabel("Plotter output (images and ASCII files):", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2PlotterOutput_ = new QLineEdit(gBox);
  lePath2PlotterOutput_->setText(setup.getPath2PlotterOutput());
  lePath2PlotterOutput_->setMinimumSize(lePath2PlotterOutput_->sizeHint());
  gridLayout->addWidget(label, 4, 0);
  gridLayout->addWidget(lePath2PlotterOutput_, 4, 1);
  //
  label = new QLabel("State and intermediate results:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2IntermediateResults_ = new QLineEdit(gBox);
  lePath2IntermediateResults_->setText(setup.getPath2IntermediateResults());
  lePath2IntermediateResults_->setMinimumSize(lePath2IntermediateResults_->sizeHint());
  gridLayout->addWidget(label, 5, 0);
  gridLayout->addWidget(lePath2IntermediateResults_, 5, 1);
  //
  label = new QLabel("Path to the auxiliary logs:", gBox);
  label->setMinimumSize(label->sizeHint());
  lePath2AuxLogs_ = new QLineEdit(gBox);
  lePath2AuxLogs_->setText(setup.getPath2AuxLogs());
  lePath2AuxLogs_->setMinimumSize(lePath2AuxLogs_->sizeHint());
  gridLayout->addWidget(label, 6, 0);
  gridLayout->addWidget(lePath2AuxLogs_, 6, 1);

  gridLayout->activate();

  mainLayout->addWidget(gBox);
  mainLayout->addStretch(1);
  //---
 
  return widget;
};



//
QWidget* NsSetupDialog::tab4Options()
{
  QGroupBox                    *gBox;
  QBoxLayout                   *mainLayout;
  QLabel                       *label;
//QGridLayout                  *gridLayout;
  QWidget                      *widget=new QWidget(this);
  QBoxLayout                   *layout;
  
  mainLayout = new QVBoxLayout(widget);
  mainLayout -> addStretch(1);

  gBox = new QGroupBox("Options", widget);
  QBoxLayout                   *aLayout=new QVBoxLayout(gBox);
  // 
  cbHave2UpdateCatalog_ = new QCheckBox("Database operations are going through the catalog", gBox);
  cbHave2MaskSessionCode_ =
    new QCheckBox("Saving a database have to alternate the session code (for tests purposes)", gBox);
  cbHave2KeepSpoolFileReports_ = new QCheckBox("Copy spoolfile reports into \"Report\" directory", gBox);
  cbHave2KeepUnusedObsList_ = new QCheckBox("Copy a list of unused obs into \"Report\" directory", gBox);
  cbHave2WarnCloseWindow_ = new QCheckBox("Warn me when closing Session Editor Window", gBox);
  cbIsShortScreen_ = new QCheckBox("Make horizontal layout in the Session Editor", gBox);
  cbHave2AutoloadAllBands_ = new QCheckBox("Autoload all bands", gBox);
  cbMakeAuxLog_ = new QCheckBox("Save a log file for each session (command line mode only)", gBox);
  cbExecExternalCommand_ = new QCheckBox("After saving execute a command:", gBox);
  
  cbIsBandPlotPerSrcView_ = new QCheckBox("Per source view of band plot", gBox);
  cbUseOwnOnlyDatabase_   = new QCheckBox("Load databases produced by " + 
    setup.identities().getAcAbbName() + " VLBI Analysis Center only", gBox);
    
  cbHave2UpdateCatalog_->setMinimumSize(cbHave2UpdateCatalog_->sizeHint());
  cbHave2MaskSessionCode_->setMinimumSize(cbHave2MaskSessionCode_->sizeHint());
  cbHave2KeepSpoolFileReports_->setMinimumSize(cbHave2KeepSpoolFileReports_->sizeHint());
  cbHave2KeepUnusedObsList_->setMinimumSize(cbHave2KeepUnusedObsList_->sizeHint());
  cbHave2WarnCloseWindow_->setMinimumSize(cbHave2WarnCloseWindow_->sizeHint());
  cbIsShortScreen_->setMinimumSize(cbIsShortScreen_->sizeHint());
  cbHave2AutoloadAllBands_->setMinimumSize(cbHave2AutoloadAllBands_->sizeHint());
  cbMakeAuxLog_->setMinimumSize(cbMakeAuxLog_->sizeHint());
  cbExecExternalCommand_->setMinimumSize(cbExecExternalCommand_->sizeHint());
  cbIsBandPlotPerSrcView_->setMinimumSize(cbIsBandPlotPerSrcView_->sizeHint());
  cbUseOwnOnlyDatabase_->setMinimumSize(cbUseOwnOnlyDatabase_->sizeHint());
  
  cbHave2UpdateCatalog_->setChecked(setup.getHave2UpdateCatalog());
  cbHave2MaskSessionCode_->setChecked(setup.getHave2MaskSessionCode());
  cbHave2KeepSpoolFileReports_->setChecked(setup.getHave2KeepSpoolFileReports());
  cbHave2KeepUnusedObsList_->setChecked(setup.getHave2KeepUnusedObsList());
  cbHave2WarnCloseWindow_->setChecked(setup.getHave2WarnCloseWindow());
  cbIsShortScreen_->setChecked(setup.getIsShortScreen());
  cbHave2AutoloadAllBands_->setChecked(setup.getHave2AutoloadAllBands());
  cbMakeAuxLog_->setChecked(setup.getHave2SavePerSessionLog());
  cbExecExternalCommand_->setChecked(setup.getExecExternalCommand());
  cbIsBandPlotPerSrcView_->setChecked(setup.getIsBandPlotPerSrcView());
  cbUseOwnOnlyDatabase_->setChecked(setup.getUseOwnOnlyDatabase());

  aLayout->addWidget(cbHave2UpdateCatalog_);
  aLayout->addWidget(cbHave2MaskSessionCode_);
  aLayout->addWidget(cbHave2KeepSpoolFileReports_);
  aLayout->addWidget(cbHave2KeepUnusedObsList_);
  aLayout->addWidget(cbHave2WarnCloseWindow_);
  aLayout->addWidget(cbIsShortScreen_);
  aLayout->addWidget(cbHave2AutoloadAllBands_);
  aLayout->addWidget(cbMakeAuxLog_);

  layout = new QHBoxLayout;
  aLayout->addLayout(layout);
  layout->addWidget(cbExecExternalCommand_);
  //
  aLayout->addWidget(cbIsBandPlotPerSrcView_);
  aLayout->addWidget(cbUseOwnOnlyDatabase_);
  //
  leExternalCommand_ = new QLineEdit(gBox);
  leExternalCommand_->setText(setup.getExternalCommand());
  layout->addWidget(leExternalCommand_);

  aLayout->activate();
  mainLayout->addWidget(gBox);
  
  //
  QGroupBox                    *gb=new QGroupBox("Output file format for plots:", gBox);
  layout = new QVBoxLayout(gb);
  QButtonGroup                 *bg=new QButtonGroup(gb);
  QRadioButton                 *rb;
  rb = new QRadioButton("PDF", gb);
  rb->setMinimumSize(rb->sizeHint());
  bg->addButton(rb, 0);
  layout->addWidget(rb);
  if (setup.getPlotterOutputFormat() == SgPlot::OF_PDF)
    rb->setChecked(true);
  //
  rb = new QRadioButton("JPG", gb);
  rb->setMinimumSize(rb->sizeHint());
  bg->addButton(rb, 1);
  layout->addWidget(rb);
  if (setup.getPlotterOutputFormat() == SgPlot::OF_JPG)
    rb->setChecked(true);
  //
  rb = new QRadioButton("PNG", gb);
  rb->setMinimumSize(rb->sizeHint());
  bg->addButton(rb, 2);
  layout->addWidget(rb);
  if (setup.getPlotterOutputFormat() == SgPlot::OF_PNG)
    rb->setChecked(true);
  //
  rb = new QRadioButton("PPM", gb);
  rb->setMinimumSize(rb->sizeHint());
  bg->addButton(rb, 3);
  layout->addWidget(rb);
  if (setup.getPlotterOutputFormat() == SgPlot::OF_PPM)
    rb->setChecked(true);

  //
  mainLayout->addWidget(gb);
  connect(bg, SIGNAL(buttonClicked(int)), SLOT(modifyPlotterOutputFormat(int)));






  //
  layout = new QHBoxLayout();
  mainLayout->addLayout(layout);
  label = new QLabel("State and the intermediate results autosave mode: ", gBox);
  label->setMinimumSize(label->sizeHint());
  layout->addWidget(label);
  
  cbAutosaveMode_ = new QComboBox(gBox);
  cbAutosaveMode_->setInsertPolicy(QComboBox::InsertAtBottom);
  const char                   *sAsmFlags[] = 
  {
    "None",
    "On Exit",
    "Always",
  };
  for (int i=0; i<(int)(sizeof(sAsmFlags)/sizeof(const char*)); i++)
    cbAutosaveMode_->addItem(sAsmFlags[i]);
  cbAutosaveMode_->setCurrentIndex((int)setup.getAutoSavingMode());
  cbAutosaveMode_->setMinimumSize(cbAutosaveMode_->sizeHint());
  layout->addWidget(cbAutosaveMode_);
  connect(cbAutosaveMode_, SIGNAL(currentIndexChanged(int)), SLOT(modifyAutosaveMode(int)));
  
  mainLayout->addStretch(1);
  
  return widget;
};



//
QWidget* NsSetupDialog::tab4Identities()
{
  QGroupBox                    *gBox;
  QBoxLayout                   *mainLayout;
  QLabel                       *label;
  QGridLayout                  *gridLayout;
  QWidget                      *widget=new QWidget(this);
  int                           labelWidth;

  mainLayout = new QVBoxLayout(widget);
  mainLayout -> addStretch(1);

  gBox = new QGroupBox("User", widget);
  gridLayout = new QGridLayout(gBox);
  // 
  label = new QLabel("Name:", gBox);
  label->setMinimumSize(label->sizeHint());
  leUserName_ = new QLineEdit(gBox);
  leUserName_->setText(setup.getIdentities().getUserName());
  leUserName_->setMinimumSize(leUserName_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leUserName_, 0, 1);
  //
  label = new QLabel("E-mail address:", gBox);
  label->setMinimumSize(label->sizeHint());
  leUserEmail_ = new QLineEdit(gBox);
  leUserEmail_->setText(setup.getIdentities().getUserEmailAddress());
  leUserEmail_->setMinimumSize(leUserEmail_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(leUserEmail_, 1, 1);
  //
  label = new QLabel("User's default initials:", gBox);
  label->setMinimumSize(label->sizeHint());
  labelWidth = label->sizeHint().width();
  leUserInitials_ = new QLineEdit(gBox);
  leUserInitials_->setText(setup.getIdentities().getUserDefaultInitials());
  leUserInitials_->setMinimumSize(leUserInitials_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(leUserInitials_, 2, 1);

  gridLayout->activate();

  mainLayout->addWidget(gBox);
  mainLayout->addStretch(1);
  //---

  gBox = new QGroupBox("Analysis Center", widget);
  gridLayout = new QGridLayout(gBox);
  // 
  label = new QLabel("Full Name:", gBox);
  label->setMinimumSize(label->sizeHint());
  label->setMinimumWidth(labelWidth);
  leAcFullName_ = new QLineEdit(gBox);
  leAcFullName_->setText(setup.getIdentities().getAcFullName());
  leAcFullName_->setMinimumSize(leAcFullName_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leAcFullName_, 0, 1);
  // 
  label = new QLabel("Abbreviation:", gBox);
  label->setMinimumSize(label->sizeHint());
  label->setMinimumWidth(labelWidth);
  leAcAbbrevName_ = new QLineEdit(gBox);
  leAcAbbrevName_->setText(setup.getIdentities().getAcAbbrevName());
  leAcAbbrevName_->setMinimumSize(leAcAbbrevName_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(leAcAbbrevName_, 1, 1);
  // 
  label = new QLabel("3-chars Abbrev.:", gBox);
  label->setMinimumSize(label->sizeHint());
  label->setMinimumWidth(labelWidth);
  leAcAbbName_ = new QLineEdit(gBox);
  leAcAbbName_->setText(setup.getIdentities().getAcAbbName());
  leAcAbbName_->setMinimumSize(leAcAbbName_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(leAcAbbName_, 2, 1);

  gridLayout->activate();
  
  mainLayout->addWidget(gBox);
  mainLayout->addStretch(2);
  
  return widget;
};



//
void NsSetupDialog::acquireData()
{
  QString                       str("");
  QList<QString>                lst;
  //
  setup.setPath2Home(lePath2Home_->text());
  setup.setPath2CatNuInterfaceExec(lePath2CatNu_->text());
  setup.setPath2DbhFiles(lePath2Dbh_->text());
  setup.setPath2VgosDbFiles(lePath2VgosDb_->text());
  setup.setPath2VgosDaFiles(lePath2VgosDa_->text());
  setup.setPath2APrioriFiles(lePath2Apriori_->text());
  setup.setPath2TrpFiles(lePath2TrpFiles_->text());
  setup.setPath2MasterFiles(lePath2Masterfiles_->text());

  //
  setup.setUseAltMasterfileSuffixes(cbUseAltMasterfileSuffixes_->isChecked());
  //
  str = leAltMasterfileSuffixes_->text();
  if (str.size())
  {
    lst = str.split(QRegularExpression("[,;:]"), QString::SkipEmptyParts);
    if (lst.size())
      setup.setMasterfileSuffixes(lst);
    else
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::acquireData(): empty list for masterfile extensions, the string: \"" + str + "\"");
  };

  //
  setup.setPath2SpoolFileOutput(lePath2Spoolfiles_->text());
  setup.setPath2NotUsedObsFileOutput(lePath2NotUsedObs_->text());
  setup.setPath2ReportOutput(lePath2Reports_->text());
  setup.setPath2NgsOutput(lePath2NgsOutput_->text());
  setup.setPath2PlotterOutput(lePath2PlotterOutput_->text());
  setup.setPath2IntermediateResults(lePath2IntermediateResults_->text());
  setup.setPath2AuxLogs(lePath2AuxLogs_->text());
  setup.setHave2UpdateCatalog(cbHave2UpdateCatalog_->isChecked());
  setup.setHave2MaskSessionCode(cbHave2MaskSessionCode_->isChecked());
  setup.setHave2KeepSpoolFileReports(cbHave2KeepSpoolFileReports_->isChecked());
  setup.setHave2KeepUnusedObsList(cbHave2KeepUnusedObsList_->isChecked());
  setup.setHave2WarnCloseWindow(cbHave2WarnCloseWindow_->isChecked());
  setup.setIsShortScreen(cbIsShortScreen_->isChecked());
  setup.setHave2AutoloadAllBands(cbHave2AutoloadAllBands_->isChecked());
  setup.setHave2SavePerSessionLog(cbMakeAuxLog_->isChecked());
  setup.setExecExternalCommand(cbExecExternalCommand_->isChecked());
  setup.setExternalCommand(leExternalCommand_->text());
  setup.setIsBandPlotPerSrcView(cbIsBandPlotPerSrcView_->isChecked());
  setup.setUseOwnOnlyDatabase(cbUseOwnOnlyDatabase_->isChecked());

  // 
  // identities:
  setup.identities().setUserName(leUserName_->text());
  setup.identities().setUserEmailAddress(leUserEmail_->text());
  setup.identities().setUserDefaultInitials(leUserInitials_->text());
  setup.identities().setAcFullName(leAcFullName_->text());
  setup.identities().setAcAbbrevName(leAcAbbrevName_->text());
  setup.identities().setAcAbbName(leAcAbbName_->text());
  //
  logConfigWidget_->acquireData();
  //
  // check home dir:
  QDir                          dir(setup.getPath2Home());
  if (!dir.exists())
  {
//  if (!dir.mkpath(setup.getPath2Home()))
    if (!dir.mkpath(dir.absolutePath()))
      QMessageBox::warning(this, "Warning", "Cannot create Home directory " + 
        setup.getPath2Home());
    else
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        ": the directory " + setup.getPath2Home() + " has been created");
  };
};



//
void NsSetupDialog::modifyPlotterOutputFormat(int idx)
{
  switch (idx)
  {
  case 0:
    setup.setPlotterOutputFormat(SgPlot::OF_PDF);
    break;
  case 1:
    setup.setPlotterOutputFormat(SgPlot::OF_JPG);
    break;
  case 2:
    setup.setPlotterOutputFormat(SgPlot::OF_PNG);
    break;
  case 3:
    setup.setPlotterOutputFormat(SgPlot::OF_PPM);
    break;
  default:
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::modifyPlotterOutputFormat(): unknown id of output format: " + QString("").setNum(idx));
  };
};



//
void NsSetupDialog::modifyAutosaveMode(int idx)
{
  switch (idx)
  {
  default:
  case 0:
    setup.setAutoSavingMode(NsSetup::AS_NONE);
    break;
  case 1:
    setup.setAutoSavingMode(NsSetup::AS_ON_EXIT);
    break;
  case 2:
    setup.setAutoSavingMode(NsSetup::AS_ALWAYS);
    break;
  };
};



//
void NsSetupDialog::changeAltMasterfileSuffixes(int chkState)
{
  bool                          have2=chkState==Qt::Checked;
  setup.setUseAltMasterfileSuffixes(have2);
  leAltMasterfileSuffixes_->setEnabled(have2);
};




/*=====================================================================================================*/
