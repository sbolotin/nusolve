/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
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





#include <SgGuiTaskConfig.h>
#include <SgGuiParameterCfg.h>
#include <SgLogger.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QComboBox>
#   include <QtWidgets/QFrame>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#   include <QtWidgets/QSpinBox>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QCheckBox>
#   include <QtGui/QComboBox>
#   include <QtGui/QFrame>
#   include <QtGui/QGroupBox>
#   include <QtGui/QHeaderView>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#   include <QtGui/QRadioButton>
#   include <QtGui/QSpinBox>
#endif




#include <QtGui/QStandardItemModel>



/*=======================================================================================================
*
*              SgGuiTaskConfig's METHODS:
* 
*======================================================================================================*/
//
SgGuiTaskConfig::SgGuiTaskConfig(SgTaskConfig* config, SgParametersDescriptor* parametersDescriptor,
  SgVlbiSession* session, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    ratTypeButtons_()
{
  //
  config_ = config;
  parametersDescriptor_ = parametersDescriptor;
  session_ = session;

  isOutliersProcessingThresholdModified_ = config_->getOpThreshold() != 3.0;
  
  QTabWidget                   *tabs = new QTabWidget (this);
  QBoxLayout                   *layout=new QVBoxLayout(this);
  tabs->addTab((tabGeneralOptions_=makeTab4GeneralOptions()), "General");
  tabs->addTab((tabOperations_=makeTab4Operations()),         "Operation");
  tabs->addTab((tabModels_=makeTab4models()),                 "External a priori and models");
  if (session_==NULL)
    tabs->addTab((tabPia_=makeTab4pia()),                     "Post import actions");
  else
  {
    tabs->setCurrentWidget(tabOperations_);
    tabs->setTabPosition(QTabWidget::North);
//  tabs->setTabPosition(QTabWidget::South);
  };
  layout->addWidget(tabs);
  // adjust status of stoch.parameters:
  if (config_->getDoWeightCorrection() || config_->getUseRateType()!=SgTaskConfig::VR_NONE)
    for (int i=0; i<stcParsButtons_.size(); i++)
      stcParsButtons_.at(i)->setEnabled(false);
  //
  tweUserCorrections_ = NULL;
};



//
QWidget* SgGuiTaskConfig::makeTab4GeneralOptions()
{
  // consts:
  // observables:
  static const QString delayTypeNames[] =
    { QString("None"),
      QString("Single band"),
      QString("Group"),
      QString("Phase")};
  static const QString rateTypeNames[] =
    { QString("None"),
      QString("Phase")};
  static const int numOfDelayTypes = sizeof(delayTypeNames)/sizeof(QString);
  static const int numOfRateTypes  = sizeof(rateTypeNames)/sizeof(QString);

  QWidget                      *w=new QWidget(this);
  QGroupBox                    *gbox;
  QCheckBox                    *cb;
  QBoxLayout                   *mainLayout;
  QBoxLayout                   *layout;
  QBoxLayout                   *aLayout;

  mainLayout = new QVBoxLayout(w);
  layout = new QHBoxLayout;
  mainLayout->addLayout(layout);
  //
  // delays:
  QRadioButton                 *rbDelTypes[numOfDelayTypes];
  gbox = new QGroupBox("Delay type:", w);
  aLayout = new QVBoxLayout(gbox);
  bgDelTypes_ = new QButtonGroup(gbox);
  for (int i=0; i<numOfDelayTypes; i++)
  {
    rbDelTypes[i] = new QRadioButton(delayTypeNames[i], gbox);
    rbDelTypes[i]-> setMinimumSize(rbDelTypes[i]->sizeHint());
    bgDelTypes_->addButton(rbDelTypes[i], i);
    aLayout->addWidget(rbDelTypes[i]);
  };
  rbDelTypes[config_->getUseDelayType()]->setChecked(true);
  connect(bgDelTypes_, SIGNAL(buttonClicked(int)), SLOT(modifyDelayType(int)));
  layout->addWidget(gbox);
  //
  // rates:
  QRadioButton                 *rbRatTypes[numOfRateTypes];
  gbox = new QGroupBox("Rate type:", w);
  aLayout = new QVBoxLayout(gbox);
  aLayout->addStretch(1);
  bgRatTypes_ = new QButtonGroup(gbox);
  for (int i=0; i<numOfRateTypes; i++)
  {
    rbRatTypes[i] = new QRadioButton(rateTypeNames[i], gbox);
    rbRatTypes[i]-> setMinimumSize(rbRatTypes[i]->sizeHint());
    bgRatTypes_->addButton(rbRatTypes[i], i);
    aLayout->addWidget(rbRatTypes[i]);
    ratTypeButtons_ << rbRatTypes[i];
  };
  aLayout->addStretch(1);
  rbRatTypes[config_->getUseRateType()]->setChecked(true);
  connect(bgRatTypes_, SIGNAL(buttonClicked(int)), SLOT(modifyRateType(int)));
  layout->addWidget(gbox);
  //
  // bands:
  if (session_ && session_->bands().size())
  {
    QRadioButton               *rbBands[session_->bands().size()];
    gbox = new QGroupBox("Band:", w);
    aLayout = new QVBoxLayout(gbox);
    bgBands_ = new QButtonGroup(gbox);
    for (int i=0; i<session_->bands().size(); i++)
    {
      rbBands[i] = new QRadioButton(session_->bands().at(i)->getKey() + "-Band", gbox);
      rbBands[i]-> setMinimumSize(rbBands[i]->sizeHint());
      bgBands_->addButton(rbBands[i], i);
      aLayout->addWidget(rbBands[i]);
    };
    rbBands[0]->setChecked(true);
    connect(bgBands_, SIGNAL(buttonClicked(int)), SLOT(band2UseModified(int)));
    layout->addWidget(gbox);
  };
  //
  //
  // GUI interaction:
  gbox = new QGroupBox("Interactions with GUI:", w);
  aLayout = new QVBoxLayout(gbox);
  // Active band:
  cb = new QCheckBox("Active band follows tab", gbox);
  cb->setCheckState(config_->getIsActiveBandFollowsTab()?Qt::Checked:Qt::Unchecked);
  aLayout->addWidget(cb);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeGUI_activeBand(int)));
  cbIsActiveBandFollowsTab_ = cb;
  // Active Observable:
  cb = new QCheckBox("Observable follows plot", gbox);
  cb->setCheckState(config_->getIsObservableFollowsPlot()?Qt::Checked:Qt::Unchecked);
  aLayout->addWidget(cb);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeGUI_observable(int)));
  cbIsObservableFollowsPlot_ = cb;
  //
  mainLayout->addWidget(gbox);
  //
  //
  // QC threshold:
  aLayout = new QHBoxLayout;
  mainLayout->addLayout(aLayout);
  aLayout->addWidget(new QLabel("Observation Quality Code threshold (use obs of this code or higher):", 
    w));
  sbQualityCodeThreshold_ = new QSpinBox(w);
  sbQualityCodeThreshold_->setValue(config_->getQualityCodeThreshold());
  sbQualityCodeThreshold_->setRange(0, 10);
//  sbQualityCodeThreshold_->setMaximum(10);
  connect(sbQualityCodeThreshold_,  SIGNAL(valueChanged(int)),
                                    SLOT  (qualityCodeThresholdModified(int)));
  aLayout->addStretch(1);
  aLayout->addWidget(sbQualityCodeThreshold_);
  //
  // G-codes:
  cb = new QCheckBox("Use observations with `G' error code", w);
  cb->setCheckState(config_->getUseQualityCodeG()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeUseQualityCodeG(int)));
  mainLayout->addWidget(cb);
  cbUseQualityCodeG_ = cb;
  //
  // H-codes:
  cb = new QCheckBox("Use observations with `H' error code", w);
  cb->setCheckState(config_->getUseQualityCodeH()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeUseQualityCodeH(int)));
  mainLayout->addWidget(cb);
  cbUseQualityCodeH_ = cb;
  //
  //
  // Compatibility:
  cb = new QCheckBox("Interactive SOLVE compatible mode", w);
  cb->setCheckState(config_->getIsSolveCompatible()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeCompatibility(int)));
  mainLayout->addWidget(cb);
  cbIsSolveCompatible_ = cb;
  // dynamic clock breaks:
  cb = new QCheckBox("Estimate clock break parameters in common solution", w);
  cb->setCheckState(config_->getUseDynamicClockBreaks()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeDynamicClockBreaks(int)));
  mainLayout->addWidget(cb);
  cbUseDynamicClockBreaks_ = cb;
  //
  //
  cb = new QCheckBox("Use SOLVE's observation elimination flags", w);
  cb->setCheckState(config_->getUseSolveObsSuppresionFlags()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeCompat_UseSolveElimFlags(int)));
  mainLayout->addWidget(cb);
  cbUseSolveObsSuppresionFlags_ = cb;
  //
  //  
  aLayout = new QHBoxLayout;
  mainLayout->addLayout(aLayout);
  cb = new QCheckBox("Initially use observations with Quality Code of or higher: ", w);
  cb->setCheckState(config_->getUseGoodQualityCodeAtStartup()?Qt::Checked:Qt::Unchecked);
  aLayout->addWidget(cb);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeUseGoodQualityCodeAtStartup(int)));
  cbUseGoodQualityCodeAtStartup_ = cb;
  // QC threshold:
  sbGoodQualityCodeAtStartup_ = new QSpinBox(w);
  sbGoodQualityCodeAtStartup_->setValue(config_->getGoodQualityCodeAtStartup());
  sbGoodQualityCodeAtStartup_->setRange(0, 10);
//  sbGoodQualityCodeAtStartup_->setMaximum(10);
  //
  connect(sbGoodQualityCodeAtStartup_,  SIGNAL(valueChanged(int)),
                                        SLOT  (goodQualityCodeAtStartupModified(int)));
  aLayout->addStretch(1);
  aLayout->addWidget(sbGoodQualityCodeAtStartup_);
  //
  //
  // Novice mode:
  cb = new QCheckBox("Novice user mode", w);
  cb->setCheckState(config_->getIsNoviceUser()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeIsNoviceUser(int)));
  cbIsNoviceUser_ = cb;
  mainLayout->addWidget(cb);
  //
  // Have to make output of a covariance matrix:
  cb = new QCheckBox("Make output of a covariance matrix in an ASCII file", w);
  cb->setCheckState(config_->getHave2outputCovarMatrix()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeHave2outputCovarMatrix(int)));
  cbHave2outputCovarMatrix_ = cb;
  mainLayout->addWidget(cb);
  //
  // Have to make output of a covariance matrix:
  cb = new QCheckBox("Ionosphere free delay and rate are from external sources", w);
  cb->setCheckState(config_->getIsIonosphereFreeUncorrelated()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeIsIonosphereFreeUncorrelated(int)));
  cbIsIonosphereFreeUncorrelated_ = cb;
  mainLayout->addWidget(cb);
  //
  // Have to make output of a covariance matrix:
  cb = new QCheckBox("Use old mode for calculation of ionosphere std.dev (test purposes)", w);
  cb->setCheckState(config_->getUseOldMode4IonosphereSigma()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeUseOldMode4IonosphereSigma(int)));
  cbUseOldMode4IonosphereSigma_ = cb;
  mainLayout->addWidget(cb);
  //
  //
  mainLayout->addStretch(1);
  //
  // 4tests:
  cb = new QCheckBox("Testing (for test purposes only)", w);
  cb->setCheckState(config_->getIsTesting()?Qt::Checked:Qt::Unchecked);
  connect(cb, SIGNAL(stateChanged(int)), SLOT(changeIsTesting(int)));
  cbIsTesting_ = cb;
  mainLayout->addWidget(cb);
  mainLayout->addStretch(1);
  //
  return w;
};



//
QWidget* SgGuiTaskConfig::makeTab4Operations()
{
  static const QString parTypeName[] =
  { QString("No"),
    QString("Lcl"),
    QString("Arc"),
    QString("Pwl"),
    QString("Stc") };
  static const int numOfParTypeNames = sizeof(parTypeName)/sizeof(QString);
  static const QString parKindName[] =
  { QString("Clocks"),
    QString("Zenith delay"),
    QString("Atm gradients"),
    QString("Station coords"),
    QString("Axis offsets"),
    QString("Source coords"),
    QString("Source structure model"),
    QString("PM"),
    QString("PM rates"),
    QString("dUT1"),
    QString("dUT1 rate"),
    QString("Nutation angles"),
    QString("Baseline clocks"),
    QString("Baseline vector"),
    QString("Test") };
  static const int numOfParKindNames = sizeof(parKindName)/sizeof(QString);
  static const QString estimatorPwlModeNames[] =
    { QString("Incremental rates"),
      QString("B-Splines: linear"),
      QString("B-Splines: quadratic") };
  static const int numOfEstimatorPwlModes = sizeof(estimatorPwlModeNames)/sizeof(QString);
  static const QString reweightingModeNames[] =
    { QString("Band-wide"),
      QString("Baseline dependent") };
  static const int numOfReweightingModes = sizeof(reweightingModeNames)/sizeof(QString);
  // outliers processing:
  static const QString outliersProcessingModeNames[] =
    { QString("Band-wide"),
      QString("Baseline dependent") };
  static const int numOfOutliersProcessingModes = sizeof(outliersProcessingModeNames)/sizeof(QString);
  static const QString outliersProcessingActionNames[] =
    { QString("Elimination"),
      QString("Restoration") };
  static const int numOfOutliersProcessingActions = sizeof(outliersProcessingActionNames)/sizeof(QString);
  //
  //

  QGroupBox                    *gbox;
  QWidget                      *w=new QWidget(this);
  QBoxLayout                   *mainLayout=new QHBoxLayout(w);
  QBoxLayout                   *layout;
  QBoxLayout                   *aLayout;
  QGridLayout                  *grid;

  layout = new QVBoxLayout;
  mainLayout->addLayout(layout);
  
  //
  // left part:
  gbox = new QGroupBox("Parameters to estimate:", w);
  grid = new QGridLayout(gbox);
  // Qt::AlignHCenter
  // Qt::AlignCenter
  // parameter types:
  for (int i=0; i<numOfParTypeNames; i++)
    grid->addWidget(new QLabel(parTypeName[i], gbox),    0, i+1, Qt::AlignHCenter);
  for (int i=0; i<numOfParKindNames; i++)
    grid->addWidget(new QLabel(parKindName[i], gbox),  i+1,   0, Qt::AlignLeft | Qt::AlignVCenter);
  //
  QRadioButton                 *rbParameters[5];
  //
  // clocks:
  bgClocks_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgClocks_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 1, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgClocks_, SIGNAL(buttonClicked(int)), SLOT(paramaters4ClocksModified(int)));
  
  // zenith delays:
  bgZenith_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgZenith_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 2, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgZenith_, SIGNAL(buttonClicked(int)), SLOT(paramaters4ZenithModified(int)));

  // atmospheric gradients::
  bgAtmGrads_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgAtmGrads_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 3, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgAtmGrads_, SIGNAL(buttonClicked(int)), SLOT(paramaters4AtmGrdModified(int)));

  // station coords:
  bgStnCoord_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgStnCoord_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 4, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgStnCoord_, SIGNAL(buttonClicked(int)), SLOT(paramaters4StnPosModified(int)));
  
  // axis offset:
  bgAxsOffset_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgAxsOffset_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 5, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgAxsOffset_, SIGNAL(buttonClicked(int)), SLOT(paramaters4AxsOfsModified(int)));

  // source coords:
  bgSrcCoord_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgSrcCoord_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 6, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgSrcCoord_, SIGNAL(buttonClicked(int)), SLOT(paramaters4SrcPosModified(int)));

  // source structure model:
  bgSrcSsm_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgSrcSsm_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 7, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgSrcSsm_, SIGNAL(buttonClicked(int)), SLOT(paramaters4SrcSsmModified(int)));

  // Polar Motion:
  bgEopPm_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgEopPm_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 8, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgEopPm_, SIGNAL(buttonClicked(int)), SLOT(paramaters4EopPolarMotionModified(int)));

  // Polar Motion Rate:
  bgEopPmRate_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgEopPmRate_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 9, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgEopPmRate_, SIGNAL(buttonClicked(int)), SLOT(paramaters4EopPolarMotionRateModified(int)));

  // Earth rotation:
  bgEopUt_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgEopUt_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 10, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgEopUt_, SIGNAL(buttonClicked(int)), SLOT(paramaters4EopUt1Modified(int)));

  // Earth rotation rate:
  bgEopUtRate_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgEopUtRate_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 11, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgEopUtRate_, SIGNAL(buttonClicked(int)), SLOT(paramaters4EopUt1RateModified(int)));

  // Nutation angles:
  bgEopNut_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgEopNut_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 12, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgEopNut_, SIGNAL(buttonClicked(int)), SLOT(paramaters4EopNutationModified(int)));
  
  // Baseline clock offsets:
  bgBlnClock_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgBlnClock_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 13, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgBlnClock_, SIGNAL(buttonClicked(int)), SLOT(paramaters4BlnClockModified(int)));

  // Baseline vector:
  bgBlnVector_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgBlnVector_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 14, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgBlnVector_, SIGNAL(buttonClicked(int)), SLOT(paramaters4BlnLengthModified(int)));

  // Test purposes:
  bgTest_ = new QButtonGroup(gbox);
  for (int i=0; i<5; i++)
  {
    rbParameters[i] = new QRadioButton(gbox);
    bgTest_->addButton(rbParameters[i], i);
    grid->addWidget(rbParameters[i], 15, 1+i, Qt::AlignHCenter);
  };
  stcParsButtons_ << rbParameters[4];
  connect(bgTest_, SIGNAL(buttonClicked(int)), SLOT(paramaters4TestModified(int)));
  //
  //
  // end of initializations, display the parameterization:
  displayParametersDesriptor();
  //
  for (int i=0; i<stcParsButtons_.size(); i++)
    connect(stcParsButtons_.at(i), SIGNAL(toggled(bool)), SLOT(checkStcParChackBoxesStatus(bool)));
  //
  // 
  QPushButton                  *pushButton;
  int                           l;
  // clocks:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth((l=3*pushButton->fontMetrics().width("Edit")));
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4Clocks()));
  grid->addWidget(pushButton, 1, 6, Qt::AlignHCenter);
  // zenith delay:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4Zenith()));
  grid->addWidget(pushButton, 2, 6, Qt::AlignHCenter);
  // atm grads:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4AtmGrd()));
  grid->addWidget(pushButton, 3, 6, Qt::AlignHCenter);
  // stations positions:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4StnPos()));
  grid->addWidget(pushButton, 4, 6, Qt::AlignHCenter);
  // axis offset:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4AxsOfs()));
  grid->addWidget(pushButton, 5, 6, Qt::AlignHCenter);
  // sources positions:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4SrcPos()));
  grid->addWidget(pushButton, 6, 6, Qt::AlignHCenter);
  // sources structure model:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4SrcSsm()));
  grid->addWidget(pushButton, 7, 6, Qt::AlignHCenter);
  // EOP, polar motion:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4EopPolar()));
  grid->addWidget(pushButton, 8, 6, Qt::AlignHCenter);
  // EOP, polar motion rates:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4EopPolarRate()));
  grid->addWidget(pushButton, 9, 6, Qt::AlignHCenter);
  // EOP, Earth rotation:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4EopUt1()));
  grid->addWidget(pushButton,10, 6, Qt::AlignHCenter);
  // EOP, Earth rotation rate:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4EopUt1Rate()));
  grid->addWidget(pushButton,11, 6, Qt::AlignHCenter);
  // EOP, nutation angles:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4EopNutation()));
  grid->addWidget(pushButton,12, 6, Qt::AlignHCenter);
  // Baselines, clock offsets:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4BlnClock()));
  grid->addWidget(pushButton,13, 6, Qt::AlignHCenter);
  // Baselines, vector of the baseline:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4BlnLength()));
  grid->addWidget(pushButton,14, 6, Qt::AlignHCenter);
  // Test purposes:
  pushButton = new QPushButton("Edit", gbox);
  pushButton->setFixedWidth(l);
  connect(pushButton, SIGNAL(clicked()), SLOT(editParameterCfg4Test()));
  grid->addWidget(pushButton,15, 6, Qt::AlignHCenter);
  //
  // end of buttons
  //
  layout->addWidget(gbox); // end of parameter selecion
  //
  //
  // PWL modes:
  QRadioButton                 *rbPwlMode[numOfEstimatorPwlModes];
  gbox = new QGroupBox("Piecewise parameter model:", w);
  aLayout = new QVBoxLayout(gbox);
  bgPwlModes_ = new QButtonGroup(gbox);
  for (int i=0; i<numOfEstimatorPwlModes; i++)
  {
    rbPwlMode[i] = new QRadioButton(estimatorPwlModeNames[i], gbox);
    rbPwlMode[i]-> setMinimumSize(rbPwlMode[i]->sizeHint());
    bgPwlModes_->addButton(rbPwlMode[i], i);
    aLayout->addWidget(rbPwlMode[i]);
  };
  rbPwlMode[config_->getPwlMode()]->setChecked(true);
  connect(bgPwlModes_, SIGNAL(buttonClicked(int)), SLOT(modifyEstimatorPwlMode(int)));
  layout->addWidget(gbox);
  //
  cbDoDownWeight_ = new QCheckBox("Downweight delays by 1.D9", gbox);
  cbDoDownWeight_->setCheckState(config_->getDoDownWeight()?Qt::Checked:Qt::Unchecked);
  layout->addWidget(cbDoDownWeight_);
  connect(cbDoDownWeight_, SIGNAL(stateChanged(int)), SLOT(changeDoDownWeight(int)));

  layout->addStretch(1);
  //
  //
  // right part:
  //
  layout = new QVBoxLayout;
  mainLayout->addLayout(layout);
  //
  // Outliers Processing:
  QGroupBox                    *gboxOP=new QGroupBox("Outliers Processing", w);
  QBoxLayout                   *layoutOP=new QVBoxLayout(gboxOP);
  QBoxLayout                   *layoutOPAM;
  layoutOPAM = new QHBoxLayout;
  /*
  if (session_)
    layoutOPAM = new QHBoxLayout;
  else
    layoutOPAM = new QVBoxLayout;
  */
  layoutOP->addLayout(layoutOPAM);
  // Action to do:
  QRadioButton                 *rbOPAction[numOfOutliersProcessingActions];
  QGroupBox                    *gboxOPA=new QGroupBox("Outliers Action:", gboxOP);
  bgOPActions_ = new QButtonGroup(gboxOPA);
  aLayout = new QVBoxLayout(gboxOPA);
  for (int i=0; i<numOfOutliersProcessingActions; i++)
  {
    rbOPAction[i] = new QRadioButton(outliersProcessingActionNames[i], gboxOPA);
    rbOPAction[i]-> setMinimumSize(rbOPAction[i]->sizeHint());
    bgOPActions_->addButton(rbOPAction[i], i);
    aLayout->addWidget(rbOPAction[i]);
  };
  rbOPAction[config_->getOpAction()]->setChecked(true);
  connect(bgOPActions_, SIGNAL(buttonClicked(int)), SLOT(outliersProcessingActionModified(int)));
  layoutOPAM->addWidget(gboxOPA);
  // Mode of the action:
  QRadioButton         *rbOPMode[numOfOutliersProcessingModes];
  gbox = new QGroupBox("Processing Mode:", gboxOP);
  bgOPModes_ = new QButtonGroup(gbox);
  aLayout = new QVBoxLayout(gbox);
  for (int i=0; i<numOfOutliersProcessingModes; i++)
  {
    rbOPMode[i] = new QRadioButton(outliersProcessingModeNames[i], gbox);
    rbOPMode[i]-> setMinimumSize(rbOPMode[i]->sizeHint());
    bgOPModes_->addButton(rbOPMode[i], i);
    aLayout->addWidget(rbOPMode[i]);
  };
  rbOPMode[config_->getOpMode()]->setChecked(true);
  connect(bgOPModes_, SIGNAL(buttonClicked(int)), SLOT(outliersProcessingModeModified(int)));
  layoutOPAM->addWidget(gbox);
  // Threshold for outliers:
  aLayout = new QHBoxLayout;
  layoutOP->addLayout(aLayout);
  aLayout->addWidget(new QLabel("Threshold for outliers (in sigmas):", gboxOP));
  leOPThreshold_ = new QLineEdit(gboxOP);
  QDoubleValidator *val=new QDoubleValidator(leOPThreshold_);
  val->setBottom(0.0);
  leOPThreshold_->setValidator(val);
  leOPThreshold_->setText(QString("").sprintf("%.2f", config_->getOpThreshold()));
  leOPThreshold_->setFixedWidth((l=leOPThreshold_->fontMetrics().width("WW.WW")));
  connect(leOPThreshold_, SIGNAL(textChanged(const QString &)),
                                            SLOT(outliersProcessingThresholdModified(const QString&)));
  aLayout->addStretch(1);
  aLayout->addWidget(leOPThreshold_);
  // Limit for iterations:
  aLayout = new QHBoxLayout;
  layoutOP->addLayout(aLayout);
  aLayout->addWidget(new QLabel("Number of iterations limit:", gboxOP));
  leOPIterationsLimit_ = new QLineEdit(gboxOP);
  QIntValidator *intVal=new QIntValidator(leOPIterationsLimit_);
  intVal->setBottom(1);
  leOPIterationsLimit_->setValidator(intVal);
  leOPIterationsLimit_->setText(QString("").sprintf("%d", config_->getOpIterationsLimit()));
  leOPIterationsLimit_->setFixedWidth(l);
  connect(leOPIterationsLimit_, SIGNAL(textChanged(const QString &)),
                                      SLOT(outliersProcessingIterationsLimitModified(const QString&)));
  aLayout->addStretch(1);
  aLayout->addWidget(leOPIterationsLimit_);
  // Suppresing weight correction in iterations:
  cbSuppressWCinOP_ = new QCheckBox("Do not reweight while process outliers", gboxOP);
  cbSuppressWCinOP_->setCheckState(
                              config_->getOpHave2SuppressWeightCorrection()?Qt::Checked:Qt::Unchecked);
  layoutOP->addWidget(cbSuppressWCinOP_);
  connect(cbSuppressWCinOP_, SIGNAL(stateChanged(int)), SLOT(changeOP_WcSuppresion(int)));
  //
  // Process outliers in SOLVE compatible mode:
  cbOPSolveCompatible_ = new QCheckBox("Use SOLVE compatible mode", gboxOP);
  cbOPSolveCompatible_->setCheckState(config_->getOpIsSolveCompatible()?Qt::Checked:Qt::Unchecked);
  layoutOP->addWidget(cbOPSolveCompatible_);
  connect(cbOPSolveCompatible_, SIGNAL(stateChanged(int)), SLOT(changeOP_IsSolveCompatible(int)));
  //
  // Do not normalize residuals:
  cbOPDoNotNormalize_ = new QCheckBox("Do not normalize residuals", gboxOP);
  cbOPDoNotNormalize_->setCheckState(config_->getOpHave2NormalizeResiduals()?Qt::Unchecked:Qt::Checked);
  layoutOP->addWidget(cbOPDoNotNormalize_);
  connect(cbOPDoNotNormalize_, SIGNAL(stateChanged(int)), SLOT(changeOP_DoNotNormalize(int)));
  //
  layout->addWidget(gboxOP);
  // end of Outliers Processing.
  //
  //
  // Reweighting:
  //
  QGroupBox                    *gboxRW=new QGroupBox("Reweighting", w);
  QVBoxLayout                  *layoutRW=new QVBoxLayout(gboxRW);
  gbox = new QGroupBox("Reweighting Action:", gboxRW);
  aLayout = new QVBoxLayout(gbox);
  // do action:
  cbDoReweighting_ = new QCheckBox("Evaluate weight correction", gbox);
  cbDoReweighting_->setCheckState(config_->getDoWeightCorrection()?Qt::Checked:Qt::Unchecked);
  aLayout->addWidget(cbDoReweighting_);
  connect(cbDoReweighting_, SIGNAL(stateChanged(int)), SLOT(changeRTA_WeightCorrection(int)));
  // use action:
  cbUseExtWeights_ = new QCheckBox("Use external weights", gbox);
  cbUseExtWeights_->setCheckState(config_->getUseExternalWeights()?Qt::Checked:Qt::Unchecked);
  aLayout->addWidget(cbUseExtWeights_);
  connect(cbUseExtWeights_, SIGNAL(stateChanged(int)), SLOT(changeRTA_UseExternalWeights(int)));
  //
  QBoxLayout                   *bLayout;
  bLayout = new QHBoxLayout;
  /*
  if (session_)
    bLayout = new QHBoxLayout;
  else
    bLayout = new QVBoxLayout;
  */
  layoutRW->addLayout(bLayout);
  bLayout->addWidget(gbox);
  //
  // Reweighting modes:
  QRadioButton *rbRWMode[numOfReweightingModes];
  gbox = new QGroupBox("Reweighting mode:", gboxRW);
  aLayout = new QVBoxLayout(gbox);
  bgRWModes_ = new QButtonGroup(gbox);
  for (int i=0; i<numOfReweightingModes; i++)
  {
    rbRWMode[i] = new QRadioButton(reweightingModeNames[i], gbox);
    rbRWMode[i]-> setMinimumSize(rbRWMode[i]->sizeHint());
    bgRWModes_->addButton(rbRWMode[i], i);
    aLayout->addWidget(rbRWMode[i]);
  };
  rbRWMode[config_->getWcMode()]->setChecked(true);
  connect(bgRWModes_, SIGNAL(buttonClicked(int)), SLOT(modifyReweightingMode(int)));
  bLayout->addWidget(gbox);
  //
  grid = new QGridLayout();
  layoutRW->addLayout(grid);
  grid->addWidget(new QLabel("External weights file name:", gboxRW), 0, 0);
  leUseExtWeights_ = new QLineEdit(gboxRW);
  leUseExtWeights_->setText(config_->getExtWeightsFileName());
  connect(leUseExtWeights_, SIGNAL(textChanged(const QString &)),
                                      SLOT(changeEF_WeightsFileName(const QString&)));
  leUseExtWeights_->setEnabled(config_->getUseExternalWeights());
  grid->addWidget(leUseExtWeights_, 0, 1);
  //
  // initial aux sigmas:
  grid->addWidget(new QLabel("Initial aux sigmas for delays (ps):", gboxRW), 1, 0);
  leIniSigma4Del_ = new QLineEdit(gboxRW);
  leIniSigma4Del_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Delay()*1.0e12));
  leIniSigma4Del_->setMinimumWidth(leIniSigma4Del_->fontMetrics().width("100.0") + 10);
  connect(leIniSigma4Del_, SIGNAL(textChanged(const QString &)), SLOT(changeAS_ini4del(const QString&)));
  grid->addWidget(leIniSigma4Del_, 1, 1);
  //
  grid->addWidget(new QLabel("Initial aux sigmas for rates (fs/s):", gboxRW), 2, 0);
  leIniSigma4Rat_ = new QLineEdit(gboxRW);
  leIniSigma4Rat_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Rate()*1.0e15));
  leIniSigma4Rat_->setMinimumWidth(leIniSigma4Rat_->fontMetrics().width("100.0") + 10);
  connect(leIniSigma4Rat_, SIGNAL(textChanged(const QString &)), SLOT(changeAS_ini4rat(const QString&)));
  grid->addWidget(leIniSigma4Rat_, 2, 1);


  // minimal aux sigmas:
  if (session_==NULL)
    grid->addWidget(new QLabel("Minimal aux sigmas for delays (ps):", gboxRW), 3, 0);
  else
    grid->addWidget(new QLabel("Minimal aux sigmas for delays (ps):", gboxRW), 1, 2);
  leMinSigma4Del_ = new QLineEdit(gboxRW);
  leMinSigma4Del_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Delay()*1.0e12));
  leMinSigma4Del_->setMinimumWidth(leMinSigma4Del_->fontMetrics().width("100.0"));
  connect(leMinSigma4Del_, SIGNAL(textChanged(const QString &)), SLOT(changeAS_min4del(const QString&)));
  if (session_==NULL)
    grid->addWidget(leMinSigma4Del_, 3, 1);
  else
    grid->addWidget(leMinSigma4Del_, 1, 3);
  //
  if (session_==NULL)
    grid->addWidget(new QLabel("Minimal aux sigmas for rates (fs/s):", gboxRW), 4, 0);
  else
    grid->addWidget(new QLabel("Minimal aux sigmas for rates (fs/s):", gboxRW), 2, 2);
  leMinSigma4Rat_ = new QLineEdit(gboxRW);
  leMinSigma4Rat_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Rate()*1.0e15));
  leMinSigma4Rat_->setMinimumWidth(leMinSigma4Rat_->fontMetrics().width("100.0"));
  connect(leMinSigma4Rat_, SIGNAL(textChanged(const QString &)), SLOT(changeAS_min4rat(const QString&)));

  if (session_ == NULL)
    grid->addWidget(leMinSigma4Rat_, 4, 1);
  else
    grid->addWidget(leMinSigma4Rat_, 2, 3);

  if (session_ == NULL)
    grid->setColumnStretch(2, 1);
  else
    grid->setColumnStretch(4, 1);
  //
  layout->addWidget(gboxRW);
  //
  // end of Reweighting.
  //
  //
  if (session_ == NULL)
    layout->addStretch(1);
  else
  {
    // Scope of data:
    //
    QGroupBox                    *gboxTSO=new QGroupBox("Time span of observations to process", w);
    //
    grid = new QGridLayout(gboxTSO);

    grid->addWidget(new QLabel("First observation:", gboxTSO), 0, 0);
    grid->addWidget(new QLabel("Last observation:",  gboxTSO), 1, 0);
    //
    QRadioButton                 *rbFirstObs[2];
    QRadioButton                 *rbLastObs[2];
    
    bgFirstObs_ = new QButtonGroup(gboxTSO);
    bgLastObs_  = new QButtonGroup(gboxTSO);

    rbFirstObs[0] = new QRadioButton("First observation of the session", gboxTSO);
    rbFirstObs[1] = new QRadioButton("Manual", gboxTSO);
    rbFirstObs[0]-> setMinimumSize(rbFirstObs[0]->sizeHint());
    rbFirstObs[1]-> setMinimumSize(rbFirstObs[1]->sizeHint());
    bgFirstObs_->addButton(rbFirstObs[0], 0);
    bgFirstObs_->addButton(rbFirstObs[1], 1);
    
    rbLastObs[0] = new QRadioButton("Last observation of the session", gboxTSO);
    rbLastObs[1] = new QRadioButton("Manual", gboxTSO);
    rbLastObs[0]-> setMinimumSize(rbLastObs[0]->sizeHint());
    rbLastObs[1]-> setMinimumSize(rbLastObs[1]->sizeHint());
    bgLastObs_->addButton(rbLastObs[0], 0);
    bgLastObs_->addButton(rbLastObs[1], 1);
  
    grid->addWidget(rbFirstObs[0], 0, 1);
    grid->addWidget(rbFirstObs[1], 0, 2);
    grid->addWidget(rbLastObs[0], 1, 1);
    grid->addWidget(rbLastObs[1], 1, 2);
   
    leFirstObs2Process_ = new QLineEdit(gboxTSO);
    leFirstObs2Process_->setText(session_->getTStart().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    leFirstObs2Process_->setMinimumWidth(leFirstObs2Process_->fontMetrics().width(leFirstObs2Process_->text()));
    grid->addWidget(leFirstObs2Process_, 0, 3);
        
    leLastObs2Process_ = new QLineEdit(gboxTSO);
    leLastObs2Process_->setText(session_->getTFinis().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    leLastObs2Process_->setMinimumWidth(leLastObs2Process_->fontMetrics().width(leLastObs2Process_->text()));
    grid->addWidget(leLastObs2Process_, 1, 3);

    connect(bgFirstObs_, SIGNAL(buttonClicked(int)), SLOT(firstObsChanged(int)));
    connect(bgLastObs_,  SIGNAL(buttonClicked(int)), SLOT(lastObsChanged(int)));
    connect(leFirstObs2Process_, SIGNAL(editingFinished()),  SLOT(changeFirstObs()));
    connect(leLastObs2Process_,  SIGNAL(editingFinished()),  SLOT(changeLastObs()));

    leFirstObs2Process_->setEnabled(false);
    leLastObs2Process_ ->setEnabled(false);

    rbFirstObs[0]->setChecked(true);
    rbLastObs[0] ->setChecked(true);

    layout->addWidget(gboxTSO);
    layout->addStretch(1);
  };
  //
  return w;
};



//
QWidget* SgGuiTaskConfig::makeTab4models()
{
/*
  static const QString flybyRefrMapFNames[] =
    { QString("None"),
      QString("NMF, Niell"),
      QString("MTT, Herring") };
  static const int numOfFlybyRefrMapFs = sizeof(flybyRefrMapFNames)/sizeof(QString);
*/
  //
  //
  QWidget                      *w=new QWidget(this);
  QBoxLayout                   *mainLayout=new QHBoxLayout(w);
  QBoxLayout                   *layoutA=new QVBoxLayout;
  QBoxLayout                   *layoutB=new QVBoxLayout;
  mainLayout->addLayout(layoutA);
  mainLayout->addLayout(layoutB);

  QGridLayout                  *grid;

  //
  // external files with a priori information:
  QGroupBox                    *gboxEF=new QGroupBox("Use external files with a priori info", w);
  grid = new QGridLayout(gboxEF);
  //
  // sites positions:
  cbExtAPrioriSitesPositions_ = new QCheckBox("Sites positions:", gboxEF);
  grid->addWidget(cbExtAPrioriSitesPositions_, 0, 0);
  connect(cbExtAPrioriSitesPositions_, SIGNAL(stateChanged(int)), SLOT(changeEF_SitePositions(int)));
  //
  leExtAPrioriSitesPositions_ = new QLineEdit(gboxEF);
  leExtAPrioriSitesPositions_->setText(config_->getExtAPrioriSitesPositionsFileName());
  connect(leExtAPrioriSitesPositions_, SIGNAL(textChanged(const QString &)),
                                       SLOT(changeEF_SitePositionsFileName(const QString&)));
  leExtAPrioriSitesPositions_->setMinimumWidth(6*leExtAPrioriSitesPositions_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriSitesPositions_, 0, 1);
  //
  // sites velocities:
  cbExtAPrioriSitesVelocities_ = new QCheckBox("Sites velocities:", gboxEF);
  grid->addWidget(cbExtAPrioriSitesVelocities_, 1, 0);
  connect(cbExtAPrioriSitesVelocities_, SIGNAL(stateChanged(int)), SLOT(changeEF_SiteVelocities(int)));
  //
  leExtAPrioriSitesVelocities_ = new QLineEdit(gboxEF);
  leExtAPrioriSitesVelocities_->setText(config_->getExtAPrioriSitesVelocitiesFileName());
  connect(leExtAPrioriSitesVelocities_, SIGNAL(textChanged(const QString &)),
                                        SLOT(changeEF_SiteVelocitiesFileName(const QString&)));
  leExtAPrioriSitesVelocities_->setMinimumWidth(6*leExtAPrioriSitesVelocities_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriSitesVelocities_, 1, 1);
  //
  // sources positions:
  cbExtAPrioriSourcesPositions_ = new QCheckBox("Sources positions:", gboxEF);
  cbExtAPrioriSourcesPositions_->setToolTip("The Sky Is a Neighborhood");
  grid->addWidget(cbExtAPrioriSourcesPositions_, 2, 0);
  connect(cbExtAPrioriSourcesPositions_, SIGNAL(stateChanged(int)), 
                                         SLOT(changeEF_SourcePositions(int)));
  //
  leExtAPrioriSourcesPositions_ = new QLineEdit(gboxEF);
  leExtAPrioriSourcesPositions_->setText(config_->getExtAPrioriSourcesPositionsFileName());
  connect(leExtAPrioriSourcesPositions_, SIGNAL(textChanged(const QString &)),
                                         SLOT(changeEF_SourcePositionsFileName(const QString&)));
  leExtAPrioriSourcesPositions_->setMinimumWidth(6*leExtAPrioriSourcesPositions_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriSourcesPositions_, 2, 1);
  //
  // sources SM:
  cbExtAPrioriSourcesSsm_ = new QCheckBox("Source structure model:", gboxEF);
  cbExtAPrioriSourcesSsm_->setToolTip("So keep it down");
  grid->addWidget(cbExtAPrioriSourcesSsm_, 3, 0);
  connect(cbExtAPrioriSourcesSsm_, SIGNAL(stateChanged(int)), 
                                   SLOT(changeEF_SourceSsm(int)));
  //
  leExtAPrioriSourcesSsm_ = new QLineEdit(gboxEF);
  leExtAPrioriSourcesSsm_->setText(config_->getExtAPrioriSourceSsmFileName());
  connect(leExtAPrioriSourcesSsm_, SIGNAL(textChanged(const QString &)),
                                         SLOT(changeEF_SourceSsmFileName(const QString&)));
  leExtAPrioriSourcesSsm_->setMinimumWidth(6*leExtAPrioriSourcesSsm_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriSourcesSsm_, 3, 1);
  //
  // EOP:
  cbExtAPrioriErp_ = new QCheckBox("Earth rotation parameters:", gboxEF);
  grid->addWidget(cbExtAPrioriErp_, 4, 0);
  connect(cbExtAPrioriErp_, SIGNAL(stateChanged(int)), SLOT(changeEF_Eop(int)));
  //
  leExtAPrioriErp_ = new QLineEdit(gboxEF);
  leExtAPrioriErp_->setText(config_->getExtAPrioriErpFileName());
  connect(leExtAPrioriErp_, SIGNAL(textChanged(const QString &)),
                            SLOT(changeEF_EopFileName(const QString&)));
  leExtAPrioriErp_->setMinimumWidth(6*leExtAPrioriErp_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriErp_, 4, 1);
  //
  // new column:
  // axis offsets:
  cbExtAPrioriAxisOffsets_ = new QCheckBox("Axis offsets:", gboxEF);
  grid->addWidget(cbExtAPrioriAxisOffsets_, 5, 0);
  connect(cbExtAPrioriAxisOffsets_, SIGNAL(stateChanged(int)), SLOT(changeEF_AxisOffset(int)));
  //
  leExtAPrioriAxisOffsets_ = new QLineEdit(gboxEF);
  leExtAPrioriAxisOffsets_->setText(config_->getExtAPrioriAxisOffsetsFileName());
  connect(leExtAPrioriAxisOffsets_, SIGNAL(textChanged(const QString &)),
                                    SLOT(changeEF_AxisOffsetFileName(const QString&)));
  leExtAPrioriAxisOffsets_->setMinimumWidth(6*leExtAPrioriAxisOffsets_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriAxisOffsets_, 5, 1);
  //
  // mean site gradients:
  cbExtAPrioriMeanGradients_ = new QCheckBox("Mean site gradients:", gboxEF);
  grid->addWidget(cbExtAPrioriMeanGradients_, 6, 0);
  connect(cbExtAPrioriMeanGradients_, SIGNAL(stateChanged(int)), SLOT(changeEF_MeanGradients(int)));
  //
  leExtAPrioriMeanGradients_ = new QLineEdit(gboxEF);
  leExtAPrioriMeanGradients_->setText(config_->getExtAPrioriMeanGradientsFileName());
  connect(leExtAPrioriMeanGradients_, SIGNAL(textChanged(const QString &)),
                                      SLOT(changeEF_MeanGradientsFileName(const QString&)));
  leExtAPrioriMeanGradients_->setMinimumWidth(6*leExtAPrioriMeanGradients_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriMeanGradients_, 6, 1);
  //
  // high frequency ERP:
  if (config_->getHave2ApplyPxyOceanTideHFContrib())
    config_->setUseExtAPrioriHiFyErp(false);
  cbExtAPrioriHiFyEop_ = new QCheckBox("External model of subdiurnal ERP variations:", gboxEF);
  grid->addWidget(cbExtAPrioriHiFyEop_, 7, 0);
  connect(cbExtAPrioriHiFyEop_, SIGNAL(stateChanged(int)), SLOT(changeEF_HiFyEop(int)));
  //
  leExtAPrioriHiFyEop_ = new QLineEdit(gboxEF);
  leExtAPrioriHiFyEop_->setText(config_->getExtAPrioriHiFyErpFileName());
  connect(leExtAPrioriHiFyEop_, SIGNAL(textChanged(const QString &)),
                                SLOT(changeEF_HiFyEopFileName(const QString&)));
  leExtAPrioriHiFyEop_->setMinimumWidth(6*leExtAPrioriHiFyEop_->fontMetrics().width("WWW"));
  grid->addWidget(leExtAPrioriHiFyEop_, 7, 1);
  //
  // eccentricities:
  grid->addWidget(new QLabel("Eccentricities file name:", gboxEF), 8, 0);
  //
  leEccentricities_ = new QLineEdit(gboxEF);
  leEccentricities_->setText(config_->getEccentricitiesFileName());
  connect(leEccentricities_,  SIGNAL(textChanged(const QString &)),
                              SLOT(changeEF_EccentricitiesFileName(const QString&)));
  leEccentricities_->setMinimumWidth(6*leEccentricities_->fontMetrics().width("WWW"));
  grid->addWidget(leEccentricities_, 8, 1);
  //

  // attach the widget:
  layoutA->addWidget(gboxEF);
  layoutA->addStretch(1);
  //
  // contributions from CALC:
  int                           rIdx, cIdx;
  QGroupBox                    *gboxODC=new QGroupBox("Apply the following contributions", w);
  grid = new QGridLayout(gboxODC);
  rIdx = 0;
  cIdx = 0;
  //
  // have2ApplyPxContrib_ && have2ApplyPyContrib_:
  cbH2AppPolarMotion_ = new QCheckBox("Polar motion (x,y)", gboxODC);
  grid->addWidget(cbH2AppPolarMotion_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasWobblePxContrib() && session_->hasWobblePyContrib()))
  {
    cbH2AppPolarMotion_->setCheckState(config_->getHave2ApplyPxContrib()? Qt::Checked:Qt::Unchecked);
    connect(cbH2AppPolarMotion_, SIGNAL(stateChanged(int)), SLOT(changeODC_PolarMotion(int)));
  }
  else
    cbH2AppPolarMotion_->setEnabled(false);
  //
  // tides:
  // have2ApplyEarthTideContrib_:
  cbH2AppEarthTide_ = new QCheckBox("Solid Earth tides", gboxODC);
  grid->addWidget(cbH2AppEarthTide_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasEarthTideContrib()))
  {
    cbH2AppEarthTide_->setCheckState(config_->getHave2ApplyEarthTideContrib()? Qt::Checked:Qt::Unchecked);
    connect(cbH2AppEarthTide_, SIGNAL(stateChanged(int)), SLOT(changeODC_SolidEarthTide(int)));
  }
  else
    cbH2AppEarthTide_->setEnabled(false);
  // have2ApplyOceanTideContrib_:
  cbH2AppOceanTide_ = new QCheckBox("Ocean tides loading", gboxODC);
  grid->addWidget(cbH2AppOceanTide_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasOceanTideContrib()))
  {
    cbH2AppOceanTide_->setCheckState(config_->getHave2ApplyOceanTideContrib()? Qt::Checked:Qt::Unchecked);
    connect(cbH2AppOceanTide_, SIGNAL(stateChanged(int)), SLOT(changeODC_OceanTide(int)));
  }
  else
    cbH2AppOceanTide_->setEnabled(false);
  //
  // have2ApplyPoleTideContrib_:
  cbH2AppPoleTide_ = new QCheckBox("Pole tide deformations", gboxODC);
  grid->addWidget(cbH2AppPoleTide_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasPoleTideContrib()))
  {
    cbH2AppPoleTide_->setCheckState(config_->getHave2ApplyPoleTideContrib()? Qt::Checked:Qt::Unchecked);
    connect(cbH2AppPoleTide_, SIGNAL(stateChanged(int)), SLOT(changeODC_PoleTide(int)));
  }
  else
    cbH2AppPoleTide_->setEnabled(false);
  //
  // have2ApplyOceanPoleTideContrib_:
  cbH2AppOceanPoleTideLd_ = new QCheckBox("Ocean Pole Tide Loading", gboxODC);
  grid->addWidget(cbH2AppOceanPoleTideLd_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasOceanPoleTideContrib()))
  {
    cbH2AppOceanPoleTideLd_->setCheckState(config_->getHave2ApplyOceanPoleTideContrib()? 
                                                                              Qt::Checked:Qt::Unchecked);
    connect(cbH2AppOceanPoleTideLd_, SIGNAL(stateChanged(int)), SLOT(changeODC_OceanPoleTide(int)));
  }
  else
    cbH2AppOceanPoleTideLd_->setEnabled(false);
  // have2ApplyGpsIonoContrib_:
  cbH2AppGpsIono_ = new QCheckBox("GPS Ionosphere corrections", gboxODC);
  grid->addWidget(cbH2AppGpsIono_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasGpsIonoContrib()))
  {
    cbH2AppGpsIono_->setCheckState(config_->getHave2ApplyGpsIonoContrib()? Qt::Checked:Qt::Unchecked);
    connect(cbH2AppGpsIono_, SIGNAL(stateChanged(int)), SLOT(changeODC_GpsIono(int)));
  }
  else
    cbH2AppGpsIono_->setEnabled(false);

/*
  //
  // have2ApplyNdryContrib_:
  cbH2AppNdryContrib_ = new QCheckBox("Refraction: hydrostatic troposphere", gboxODC);
  grid->addWidget(cbH2AppNdryContrib_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasNdryContrib()))
  {
    cbH2AppNdryContrib_->setCheckState(config_->getHave2ApplyNdryContrib()?Qt::Checked:Qt::Unchecked);
    connect(cbH2AppNdryContrib_, SIGNAL(stateChanged(int)), SLOT(changeODC_TropRefractionDry(int)));
  }
  else
    cbH2AppNdryContrib_->setEnabled(false);

  // have2ApplyNwetContrib_:
  cbH2AppNwetContrib_ = new QCheckBox("Refraction: wet troposphere", gboxODC);
  grid->addWidget(cbH2AppNwetContrib_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasNwetContrib()))
  {
    cbH2AppNwetContrib_->setCheckState(config_->getHave2ApplyNwetContrib()?Qt::Checked:Qt::Unchecked);
    connect(cbH2AppNwetContrib_, SIGNAL(stateChanged(int)), SLOT(changeODC_TropRefractionWet(int)));
  }
  else
    cbH2AppNwetContrib_->setEnabled(false);
*/


  //
  // have2ApplyFeedCorrContrib_:
  cbH2AppFeedCorr_ = new QCheckBox("Feed horn rotation", gboxODC);
  grid->addWidget(cbH2AppFeedCorr_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasFeedCorrContrib()))
  {
    cbH2AppFeedCorr_->setCheckState(config_->getHave2ApplyFeedCorrContrib()?Qt::Checked:Qt::Unchecked);
    connect(cbH2AppFeedCorr_, SIGNAL(stateChanged(int)), SLOT(changeODC_FeedCorr(int)));
  }
  else
    cbH2AppFeedCorr_->setEnabled(false);
  //
  //  new column:
  //
  rIdx = 0;
  cIdx = 1;
  //
  //
  //
  // have2ApplyPxyOceanTideHFContrib_:
  cbH2AppHfTidalPxy_ = new QCheckBox("Subdiurnal tidal polar motion", gboxODC);
  grid->addWidget(cbH2AppHfTidalPxy_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasWobbleHighFreqContrib()))
  {
    connect(cbH2AppHfTidalPxy_, SIGNAL(stateChanged(int)), SLOT(changeODC_HiFqTidalPxy(int)));
    cbH2AppHfTidalPxy_->setCheckState(config_->getHave2ApplyPxyOceanTideHFContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
  }
  else
    cbH2AppHfTidalPxy_->setEnabled(false);
  //
  // have2ApplyUt1OceanTideHFContrib_:
  cbH2AppHfTidalUt1_ = new QCheckBox("Subdiurnal tidal UT1", gboxODC);
  grid->addWidget(cbH2AppHfTidalUt1_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasUt1HighFreqContrib()))
  {
    connect(cbH2AppHfTidalUt1_, SIGNAL(stateChanged(int)), SLOT(changeODC_HiFqTidalUt1(int)));
    cbH2AppHfTidalUt1_->setCheckState(config_->getHave2ApplyUt1OceanTideHFContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
  }
  else
    cbH2AppHfTidalUt1_->setEnabled(false);
  //
  if (session_==NULL || (session_ && session_->calcInfo().getDversion()>=11.0))
  {
    // have2ApplyPxyLibrationContrib_:
    cbH2AppHfLibrationPxy_ = new QCheckBox("Libration in polar motion", gboxODC);
    grid->addWidget(cbH2AppHfLibrationPxy_, rIdx++, cIdx);
    if (session_==NULL || 
       (session_ && session_->hasPxyLibrationContrib()))
    {
      connect(cbH2AppHfLibrationPxy_, SIGNAL(stateChanged(int)), SLOT(changeODC_HiFqLibrationPxy(int)));
      cbH2AppHfLibrationPxy_->setCheckState(config_->getHave2ApplyPxyLibrationContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
    }
    else
      cbH2AppHfLibrationPxy_->setEnabled(false);
    //
    // have2ApplyUt1LibrationContrib_:
    cbH2AppHfLibrationUt1_ = new QCheckBox("Libration in UT1", gboxODC);
    grid->addWidget(cbH2AppHfLibrationUt1_, rIdx++, cIdx);
    if (session_==NULL || 
       (session_ && session_->hasUt1LibrationContrib()))
    {
      connect(cbH2AppHfLibrationUt1_, SIGNAL(stateChanged(int)), SLOT(changeODC_HiFqLibrationUt1(int)));
      cbH2AppHfLibrationUt1_->setCheckState(config_->getHave2ApplyUt1LibrationContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
    }
    else
      cbH2AppHfLibrationUt1_->setEnabled(false);
    cbH2AppHfNutation_ = NULL;
  }
  else
  {
    // have2ApplyNutationHFContrib_:
    cbH2AppHfNutation_ = new QCheckBox("Subdiurnal nutation in ERP", gboxODC);
    grid->addWidget(cbH2AppHfNutation_, rIdx++, cIdx);
    if (session_==NULL || 
       (session_ && session_->hasWobbleNutContrib()))
    {
      connect(cbH2AppHfNutation_, SIGNAL(stateChanged(int)), SLOT(changeODC_HiFqNutation(int)));
      cbH2AppHfNutation_->setCheckState(config_->getHave2ApplyNutationHFContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
    }
    else
      cbH2AppHfNutation_->setEnabled(false);
    cbH2AppHfLibrationPxy_ = NULL;
    cbH2AppHfLibrationUt1_ = NULL;
  };
  //
  // have2ApplyAxisOffsetContrib_:
  cbH2AppAxisOffsetContrib_ = new QCheckBox("Axis offset", gboxODC);
  grid->addWidget(cbH2AppAxisOffsetContrib_, rIdx++, cIdx);
  cbH2AppAxisOffsetContrib_->setCheckState(config_->getHave2ApplyAxisOffsetContrib()?
                                                                              Qt::Checked:Qt::Unchecked);
  connect(cbH2AppAxisOffsetContrib_, SIGNAL(stateChanged(int)), SLOT(changeODC_AxisOffset(int)));
  //
  //
  cbH2AppTiltRmvr_ = new QCheckBox("Axis Tilt Remover", gboxODC);
  grid->addWidget(cbH2AppTiltRmvr_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasTiltRemvrContrib()))
  {
    cbH2AppTiltRmvr_->setCheckState(config_->getHave2ApplyTiltRemvrContrib()?Qt::Checked:Qt::Unchecked);
    connect(cbH2AppTiltRmvr_, SIGNAL(stateChanged(int)), SLOT(changeODC_TiltRmvr(int)));
  }
  else
    cbH2AppTiltRmvr_->setEnabled(false);
  //
  // have2ApplyOldOceanTideContrib_:
  cbH2AppOldOceanTide_ = new QCheckBox("Old ocean tides loading", gboxODC);
  grid->addWidget(cbH2AppOldOceanTide_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasOceanTideOldContrib()))
  {
    cbH2AppOldOceanTide_->setCheckState(config_->getHave2ApplyOldOceanTideContrib()? 
                                                                              Qt::Checked:Qt::Unchecked);
    connect(cbH2AppOldOceanTide_, SIGNAL(stateChanged(int)), SLOT(changeODC_OldOceanTide(int)));
  }
  else
    cbH2AppOldOceanTide_->setEnabled(false);
  //
  // have2ApplyOldPoleTideContrib_:
  cbH2AppOldPoleTide_ = new QCheckBox("Old pole tide deformations", gboxODC);
  grid->addWidget(cbH2AppOldPoleTide_, rIdx++, cIdx);
  if (session_==NULL || 
     (session_ && session_->hasPoleTideOldContrib()))
  {
    cbH2AppOldPoleTide_->setCheckState(config_->getHave2ApplyOldPoleTideContrib()? 
                                                                              Qt::Checked:Qt::Unchecked);
    connect(cbH2AppOldPoleTide_, SIGNAL(stateChanged(int)), SLOT(changeODC_OldPoleTide(int)));
  }
  else
    cbH2AppOldPoleTide_->setEnabled(false);
  //
  QBoxLayout                   *hLayout=new QHBoxLayout;

  layoutA->addLayout(hLayout);
  hLayout->addWidget(gboxODC);
  layoutA->addStretch(1);

  //
  //

  QGroupBox                    *gboxFBM=new QGroupBox("Apply the following models/options:", w);
  QVBoxLayout                  *layout=new QVBoxLayout(gboxFBM);
/*
  // Refraction, hydrostatic:
  QRadioButton                 *rbRefrMapF[numOfFlybyRefrMapFs];
  QGroupBox                    *gbox=new QGroupBox("Refraction: zenith delay mapping function", gboxFBM);
  bgFlybyRefrMapF_ = new QButtonGroup(gbox);
  QVBoxLayout                  *aLayout=new QVBoxLayout(gbox);
  for (int i=0; i<numOfFlybyRefrMapFs; i++)
  {
    rbRefrMapF[i] = new QRadioButton(flybyRefrMapFNames[i], gbox);
    rbRefrMapF[i]-> setMinimumSize(rbRefrMapF[i]->sizeHint());
    bgFlybyRefrMapF_->addButton(rbRefrMapF[i], i);
    aLayout->addWidget(rbRefrMapF[i]);
  };
  rbRefrMapF[config_->getFlybyTropZenithMap()]->setChecked(true);
  connect(bgFlybyRefrMapF_, SIGNAL(buttonClicked(int)), SLOT(flybyRefrMapFModified(int)));

  layout->addWidget(gbox);
*/
  //
  // Source structure corrections:
  //
  cbH2AppSourceSsm_ = new QCheckBox("Source structure model", gboxFBM);
  layout->addWidget(cbH2AppSourceSsm_);
  cbH2AppSourceSsm_->setCheckState(config_->getHave2ApplySourceSsm()? Qt::Checked:Qt::Unchecked);
  connect(cbH2AppSourceSsm_, SIGNAL(stateChanged(int)), SLOT(changeODC_SourceSsm(int)));
  //
  // Refrection models:
  //
  QBoxLayout                   *lyt=new QHBoxLayout();
  layout->addLayout(lyt);
  QLabel                       *label = new QLabel("Refraction model: ", gboxFBM);
  label->setMinimumSize(label->sizeHint());
  lyt->addWidget(label);
  cbRefractionModel_ = new QComboBox(gboxFBM);
  cbRefractionModel_->setInsertPolicy(QComboBox::InsertAtBottom);
  //
  cbRefractionModel_->addItem("None", SgTaskConfig::RM_NONE);
  cbRefractionModel_->addItem("Contribution from the database", SgTaskConfig::RM_CONTRIB_INTERNAL);
  if (!session_)
    cbRefractionModel_->addItem("Contribution from an external file", SgTaskConfig::RM_CONTRIB_EXTERNAL);
  else
    cbRefractionModel_->addItem("Contribution from an external file: " + 
      session_->externalTrpFile().modelName(), SgTaskConfig::RM_CONTRIB_EXTERNAL);
  cbRefractionModel_->addItem("FlyBy: NMF", SgTaskConfig::RM_FLYBY_NMF);
  cbRefractionModel_->addItem("FlyBy: MTT", SgTaskConfig::RM_FLYBY_MTT);
  // disable it if the file is missing:
  if (session_ && !session_->externalTrpFile().isOk())
  {
    QStandardItemModel         *model=dynamic_cast<QStandardItemModel*>(cbRefractionModel_->model());
    QStandardItem              *item=model->item((int)SgTaskConfig::RM_CONTRIB_EXTERNAL);
    item->setEnabled(false);
  };
  cbRefractionModel_->setCurrentIndex((int)config_->getRefractionModel());
  cbRefractionModel_->setMinimumSize(cbRefractionModel_->sizeHint());
  lyt->addWidget(cbRefractionModel_);
  connect(cbRefractionModel_, SIGNAL(currentIndexChanged(int)), SLOT(modifyRefractionModel(int)));
  // end of Refraction models
  

  // 
  // 
  if (session_)
  {
    bool                        hasCdma, hasPcmt;
    hasCdma = hasPcmt = false;
    for (QMap<QString, SgVlbiStationInfo*>::iterator it=session_->stationsByName().begin(); 
      it!=session_->stationsByName().end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS))
        hasCdma = true;
      if (si->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT))
        hasPcmt = true;
    };
    if (hasCdma || hasPcmt)
    {
      // source of cable calibration corrections:
      QRadioButton             *rbCccSource[4];
      QGroupBox                *gbox=new QGroupBox("Chose a source of cable calibration correction "
                                                    "(for all stations):",
        gboxFBM);
      bgFlybySrcCcc_ = new QButtonGroup(gbox);
      QVBoxLayout              *aLayout=new QVBoxLayout(gbox);
      aLayout->setSpacing(aLayout->spacing()/2);
      for (int i=0; i<4; i++)
      {
        rbCccSource[i] = new QRadioButton("Use cable calibration correction from: " + 
          sCableCalSourceNames[i], gbox);
        rbCccSource[i]->setMinimumSize(rbCccSource[i]->sizeHint());
        bgFlybySrcCcc_->addButton(rbCccSource[i], i);
        aLayout->addWidget(rbCccSource[i]);
      };
      rbCccSource[config_->getFlybyCableCalSource()]->setChecked(true);
      connect(bgFlybySrcCcc_, SIGNAL(buttonClicked(int)), SLOT(flybySourceCccFModified(int)));
      layout->addWidget(gbox);
    };
  };


  
  //
  //
  layout->addStretch(1);
  //
  // end of fly by models...
  hLayout->addWidget(gboxFBM);
  //
  //
  //
  //  right column:
  if (session_ && session_->getUserCorrectionsName().size()) // can be NULL:
  {
    //
    // external files with a priori information:
    QGroupBox                    *gboxUC=new QGroupBox("Apply user provided corrections", w);
    QBoxLayout                   *layout=new QVBoxLayout(gboxUC);
    //
    // attach the widget:
    layoutB->addWidget(gboxUC);

    QStringList                   headerLabels;
    headerLabels << "User correction" << "Apply?";
    //
    tweUserCorrections_ = new SgGuiQTreeWidgetExt(gboxUC);
    tweUserCorrections_->setColumnCount(headerLabels.count());
    tweUserCorrections_->setHeaderLabels(headerLabels);
    for (int i=0; i<session_->getUserCorrectionsName().size(); i++)
    {
      SgGuiTcUserCorrectionItem 
                               *item=new SgGuiTcUserCorrectionItem(tweUserCorrections_, i);
      item->setText(0, session_->getUserCorrectionsName().at(i));
      item->setData(0, Qt::TextAlignmentRole, Qt::AlignLeft);
      item->setText(1, session_->getUserCorrectionsUse().at(i)?"Y":"");
      item->setData(1, Qt::TextAlignmentRole, Qt::AlignCenter);
    };
    tweUserCorrections_->header()->resizeSections(QHeaderView::ResizeToContents);
    tweUserCorrections_->header()->setSectionResizeMode(QHeaderView::Interactive);
    tweUserCorrections_->header()->setStretchLastSection(false);
    tweUserCorrections_->setSortingEnabled(true);
    tweUserCorrections_->setUniformRowHeights(true);
    tweUserCorrections_->sortByColumn(1, Qt::AscendingOrder);
    tweUserCorrections_->setFocus();
    tweUserCorrections_->setItemsExpandable(false);
    tweUserCorrections_->setAllColumnsShowFocus(true);

    layout->addWidget(tweUserCorrections_);
  
    connect(tweUserCorrections_,
      SIGNAL(moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)), 
      SLOT  (toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)));
  };
  //
  //
  //
  // set check states after creation of QLineEditors and other objects:
  cbExtAPrioriSitesPositions_->setCheckState(
                                  config_->getUseExtAPrioriSitesPositions()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSitesVelocities_->setCheckState(
                                  config_->getUseExtAPrioriSitesVelocities()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSourcesPositions_->setCheckState(
                                  config_->getUseExtAPrioriSourcesPositions()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSourcesSsm_->setCheckState(config_->getUseExtAPrioriSourceSsm()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriErp_->setCheckState(config_->getUseExtAPrioriErp()? Qt::Checked:Qt::Unchecked);
  cbExtAPrioriAxisOffsets_->setCheckState(
                                  config_->getUseExtAPrioriAxisOffsets()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriHiFyEop_->setCheckState(config_->getUseExtAPrioriHiFyErp()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriMeanGradients_->setCheckState(
                                  config_->getUseExtAPrioriMeanGradients()?Qt::Checked:Qt::Unchecked);
  leExtAPrioriSitesPositions_->setEnabled(config_->getUseExtAPrioriSitesPositions());
  leExtAPrioriSitesVelocities_->setEnabled(config_->getUseExtAPrioriSitesVelocities());
  leExtAPrioriSourcesPositions_->setEnabled(config_->getUseExtAPrioriSourcesPositions());
  leExtAPrioriSourcesSsm_->setEnabled(config_->getUseExtAPrioriSourceSsm());
  leExtAPrioriErp_->setEnabled(config_->getUseExtAPrioriErp());
  leExtAPrioriAxisOffsets_->setEnabled(config_->getUseExtAPrioriAxisOffsets());
  leExtAPrioriHiFyEop_->setEnabled(config_->getUseExtAPrioriHiFyErp());
  leExtAPrioriMeanGradients_->setEnabled(config_->getUseExtAPrioriMeanGradients());
  //
  //
  //
/*
  if (cbH2AppNdryContrib_->checkState() == Qt::Checked ||
      cbH2AppNwetContrib_->checkState() == Qt::Checked  )
    for (int i=0; i<bgFlybyRefrMapF_->buttons().size(); i++)  
      bgFlybyRefrMapF_->buttons()[i]->setEnabled(false);
  //
  if (bgFlybyRefrMapF_->checkedId() != 0)
  {
    cbH2AppNdryContrib_->setEnabled(false);
    cbH2AppNwetContrib_->setEnabled(false);
  };
*/
  //
  //
//-->>  mainLayout->addStretch(1);
  
  return w;
};



//
QWidget* SgGuiTaskConfig::makeTab4pia()
{
  static const QString automaticFinalSolutionName[] =
    { QString("Baselines"),
      QString("dUT1") };
  static const int numOfAutomaticFinalSolutionNames = sizeof(automaticFinalSolutionName)/sizeof(QString);
  //
  QWidget                      *w=new QWidget(this);
  QBoxLayout                   *mainLayout=new QVBoxLayout(w);
  QBoxLayout                   *layout;
  QLabel                       *label;

  // Automatic actions setup:
  //
  label = new QLabel("Set up for network ID:", w);
  label->setMinimumSize(label->sizeHint());
  layout = new QHBoxLayout;
  mainLayout->addLayout(layout);
  layout->addWidget(label);
  //
  cbNetID_ = new QComboBox(w);
  cbNetID_->setMinimumSize(cbNetID_->sizeHint());
  layout->addStretch(1);
  layout->addWidget(cbNetID_);
  //
  gbAutoAction_ = new QGroupBox("Actions to perform:", w);
  mainLayout->addWidget(gbAutoAction_);
  layout = new QVBoxLayout(gbAutoAction_);
  //
  cbApDoSessionSetup_ = new QCheckBox("Perform set up of the session (reference clock station, etc.)",
                                                                                          gbAutoAction_);
  layout->addWidget(cbApDoSessionSetup_);
  connect(cbApDoSessionSetup_, SIGNAL(stateChanged(int)), SLOT(changeAA_SessionSetup(int)));
  //
  cbApDoIonoCorrection4SBD_ = new QCheckBox("Evaluate ionospheric correction for single band delay", 
                                                                                          gbAutoAction_);
  layout->addWidget(cbApDoIonoCorrection4SBD_);
  connect(cbApDoIonoCorrection4SBD_, SIGNAL(stateChanged(int)), SLOT(changeAA_Iono4SBD(int)));
  //
  cbApDoAmbigResolution_ = new QCheckBox("Resolve group delay ambiguities", gbAutoAction_);
  layout->addWidget(cbApDoAmbigResolution_);
  connect(cbApDoAmbigResolution_, SIGNAL(stateChanged(int)), SLOT(changeAA_Ambigs(int)));
  //
  cbApDoClockBreaksDetection_ = new QCheckBox("Detect clock breaks", gbAutoAction_);
  layout->addWidget(cbApDoClockBreaksDetection_);
  connect(cbApDoClockBreaksDetection_, SIGNAL(stateChanged(int)), SLOT(changeAA_ClockBreaks(int)));
  //
  cbApDoIonoCorrection4All_ = new QCheckBox("Evaluate ionospheric correction for all observables",
                                                                                          gbAutoAction_);
  layout->addWidget(cbApDoIonoCorrection4All_);
  connect(cbApDoIonoCorrection4All_, SIGNAL(stateChanged(int)), SLOT(changeAA_Iono4All(int)));
  //
  cbApDoOutliers_ = new QCheckBox("Process outliers", gbAutoAction_);
  layout->addWidget(cbApDoOutliers_);
  connect(cbApDoOutliers_, SIGNAL(stateChanged(int)), SLOT(changeAA_Outliers(int)));
  //
  cbApDoWeights_ = new QCheckBox("Perform weight corrections", gbAutoAction_);
  layout->addWidget(cbApDoWeights_);
  connect(cbApDoWeights_, SIGNAL(stateChanged(int)), SLOT(changeAA_Reweighting(int)));
  //
  cbApDoReportNotUsedData_ = new QCheckBox("Print a list of not used observations in a file", 
                                                                                          gbAutoAction_);
  layout->addWidget(cbApDoReportNotUsedData_);
  connect(cbApDoReportNotUsedData_, SIGNAL(stateChanged(int)), SLOT(changeAA_ReportNotUsed(int)));
  //
  // options:
  QRadioButton                 *rbFinSolTypes[numOfAutomaticFinalSolutionNames];
  gbApFinalSolution_ = new QGroupBox("Make Final Solution:", this);
  layout = new QVBoxLayout(gbApFinalSolution_);
  bgApFinalSolution_ = new QButtonGroup(gbApFinalSolution_);
  for (int i=0; i<numOfAutomaticFinalSolutionNames; i++)
  {
    rbFinSolTypes[i] = new QRadioButton(automaticFinalSolutionName[i], gbApFinalSolution_);
    rbFinSolTypes[i]-> setMinimumSize(rbFinSolTypes[i]->sizeHint());
    bgApFinalSolution_->addButton(rbFinSolTypes[i], i);
    layout->addWidget(rbFinSolTypes[i]);
  };
  connect(bgApFinalSolution_, SIGNAL(buttonClicked(int)), SLOT(changeAA_FinalSolution(int)));
  mainLayout->addWidget(gbApFinalSolution_); 
  //
  //
  layout = new QHBoxLayout;
  mainLayout->addLayout(layout);
  pbApCreate_ = new QPushButton("Create", w);
  layout->addStretch(1);
  layout->addWidget(pbApCreate_);
  connect(pbApCreate_, SIGNAL(clicked()), SLOT(createAutomaticProcessing4NetId()));
  //
  pbApDestroy_ = new QPushButton("Destroy", w);
  layout->addWidget(pbApDestroy_);
  connect(pbApDestroy_, SIGNAL(clicked()), SLOT(destroyAutomaticProcessing4NetId()));
  //
  //
  // feed data into the ComboBox, all the checkboxes have to exist at this stage:
  bool                        isTmp(false);
  cbNetID_->setInsertPolicy(QComboBox::InsertAlphabetically);
  for (int i=0; i<networks.size(); i++)
    cbNetID_->addItem(networks.at(i).getName());
  connect(cbNetID_, 
    SIGNAL(currentIndexChanged(const QString&)), SLOT(netIdCurrentIndexChanged(const QString&)));

  for (int i=0; i<networks.size(); i++)
  {
    if (networks.at(i).getName() == config_->getLastModifiedNetId())
    {
      cbNetID_->setCurrentIndex(i);
      if (i==0)
        netIdCurrentIndexChanged(config_->getLastModifiedNetId());
      isTmp = true;
    };
  };
  if (!isTmp)
    for (int i=0; i<networks.size(); i++)
    {
      if (networks.at(i).getName() == defaultNetIdName)
        cbNetID_->setCurrentIndex(i);
    };
  // end of automatic procession setup:
  mainLayout->addStretch(1);

  return w;
};



//
void SgGuiTaskConfig::displayParametersDesriptor()
{
  QMap<SgParameterCfg::PMode, int>        idxByMode;
  idxByMode[SgParameterCfg::PM_NONE] = 0;
  idxByMode[SgParameterCfg::PM_GLB]  = 1;
  idxByMode[SgParameterCfg::PM_LOC]  = 1;
  idxByMode[SgParameterCfg::PM_ARC]  = 2;
  idxByMode[SgParameterCfg::PM_PWL]  = 3;
  idxByMode[SgParameterCfg::PM_STC]  = 4;
  
  bgClocks_   ->button(idxByMode[parametersDescriptor_->getClock0()     .getPMode()])->setChecked(true);
  bgZenith_   ->button(idxByMode[parametersDescriptor_->getZenith()     .getPMode()])->setChecked(true);
  bgAtmGrads_ ->button(idxByMode[parametersDescriptor_->getAtmGrad()    .getPMode()])->setChecked(true);
  bgStnCoord_ ->button(idxByMode[parametersDescriptor_->getStnCoo()     .getPMode()])->setChecked(true);
  bgAxsOffset_->button(idxByMode[parametersDescriptor_->getAxisOffset() .getPMode()])->setChecked(true);
  bgSrcCoord_ ->button(idxByMode[parametersDescriptor_->getSrcCoo()     .getPMode()])->setChecked(true);
  bgSrcSsm_   ->button(idxByMode[parametersDescriptor_->getSrcSsm()     .getPMode()])->setChecked(true);
  bgEopPm_    ->button(idxByMode[parametersDescriptor_->getPolusXY()    .getPMode()])->setChecked(true);
  bgEopPmRate_->button(idxByMode[parametersDescriptor_->getPolusXYRate().getPMode()])->setChecked(true);
  bgEopUt_    ->button(idxByMode[parametersDescriptor_->getPolusUT1()   .getPMode()])->setChecked(true);
  bgEopUtRate_->button(idxByMode[parametersDescriptor_->getPolusUT1Rate().getPMode()])->setChecked(true);
  bgEopNut_   ->button(idxByMode[parametersDescriptor_->getPolusNut()   .getPMode()])->setChecked(true);
  bgBlnClock_ ->button(idxByMode[parametersDescriptor_->getBlClock()    .getPMode()])->setChecked(true);
  bgBlnVector_->button(idxByMode[parametersDescriptor_->getBlLength()   .getPMode()])->setChecked(true);
  bgTest_     ->button(idxByMode[parametersDescriptor_->getTest()       .getPMode()])->setChecked(true);
};



//
void SgGuiTaskConfig::displayConfig()
{
  if (session_==NULL)
    return;
  
  QAbstractButton              *b=NULL;

  // page one (general):
  b = bgDelTypes_->button(config_->getUseDelayType());
  if (b)
  {
    b->setChecked(true);
    emit delayTypeModified(config_->getUseDelayType());
  };
  b = bgRatTypes_->button(config_->getUseRateType());
  if (b)
  {
    b->setChecked(true);
    emit rateTypeModified(config_->getUseRateType());
  };
  b = bgBands_->button(config_->getActiveBandIdx());
  if (b)
  {
    b->setChecked(true);
    emit activeBandModified(config_->getActiveBandIdx());
  };
  
  cbIsActiveBandFollowsTab_->
    setCheckState(config_->getIsActiveBandFollowsTab()?Qt::Checked:Qt::Unchecked);
  cbIsObservableFollowsPlot_->
    setCheckState(config_->getIsObservableFollowsPlot()?Qt::Checked:Qt::Unchecked);
  sbQualityCodeThreshold_->setValue(config_->getQualityCodeThreshold());
  //
  cbUseQualityCodeG_->setCheckState(config_->getUseQualityCodeG()?Qt::Checked:Qt::Unchecked);
  cbUseQualityCodeH_->setCheckState(config_->getUseQualityCodeH()?Qt::Checked:Qt::Unchecked);
  cbIsSolveCompatible_->setCheckState(config_->getIsSolveCompatible()?Qt::Checked:Qt::Unchecked);
  cbUseDynamicClockBreaks_->setCheckState(config_->getUseDynamicClockBreaks()?Qt::Checked:Qt::Unchecked);
  cbUseSolveObsSuppresionFlags_->
    setCheckState(config_->getUseSolveObsSuppresionFlags()?Qt::Checked:Qt::Unchecked);
  cbUseGoodQualityCodeAtStartup_->
    setCheckState(config_->getUseGoodQualityCodeAtStartup()?Qt::Checked:Qt::Unchecked);
  sbGoodQualityCodeAtStartup_->setValue(config_->getGoodQualityCodeAtStartup());
  cbIsNoviceUser_->setCheckState(config_->getIsNoviceUser()?Qt::Checked:Qt::Unchecked);
  cbHave2outputCovarMatrix_->setCheckState(
    config_->getHave2outputCovarMatrix()? Qt::Checked : Qt::Unchecked);
  cbIsIonosphereFreeUncorrelated_->setCheckState(
    config_->getIsIonosphereFreeUncorrelated()? Qt::Checked : Qt::Unchecked);
  cbUseOldMode4IonosphereSigma_->setCheckState(
    config_->getUseOldMode4IonosphereSigma()? Qt::Checked : Qt::Unchecked);
  cbIsTesting_->setCheckState(config_->getIsTesting()? Qt::Checked : Qt::Unchecked);
  //
  // page two (operations), except estimated parameters:
  b = bgPwlModes_->button(config_->getPwlMode());
  if (b)
    b->setChecked(true);
  b = bgOPActions_->button(config_->getOpAction());
  if (b)
    b->setChecked(true);
  b = bgOPModes_->button(config_->getOpMode());
  if (b)
    b->setChecked(true);

  leOPThreshold_->setText(QString("").sprintf("%.2f", config_->getOpThreshold()));
  leOPIterationsLimit_->setText(QString("").sprintf("%d", config_->getOpIterationsLimit()));
  cbSuppressWCinOP_->
    setCheckState(config_->getOpHave2SuppressWeightCorrection()?Qt::Checked:Qt::Unchecked);
  cbOPSolveCompatible_->setCheckState(config_->getOpIsSolveCompatible()?Qt::Checked:Qt::Unchecked);
  cbOPDoNotNormalize_->setCheckState(config_->getOpHave2NormalizeResiduals()?Qt::Unchecked:Qt::Checked);
  cbDoReweighting_->setCheckState(config_->getDoWeightCorrection()?Qt::Checked:Qt::Unchecked);
  cbUseExtWeights_->setCheckState(config_->getUseExternalWeights()?Qt::Checked:Qt::Unchecked);
  b = bgRWModes_->button(config_->getWcMode());
  if (b)
    b->setChecked(true);
  leUseExtWeights_->setText(config_->getExtWeightsFileName());
  leUseExtWeights_->setEnabled(config_->getUseExternalWeights());
  leIniSigma4Del_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Delay()*1.0e12));
  leIniSigma4Rat_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Rate()*1.0e15));
  leMinSigma4Del_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Delay()*1.0e12));
  leMinSigma4Rat_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Rate()*1.0e15));
  //
  // page three (models):
  leExtAPrioriSitesPositions_->setText(config_->getExtAPrioriSitesPositionsFileName());
  leExtAPrioriSitesVelocities_->setText(config_->getExtAPrioriSitesVelocitiesFileName());
  leExtAPrioriSourcesPositions_->setText(config_->getExtAPrioriSourcesPositionsFileName());
  leExtAPrioriSourcesSsm_->setText(config_->getExtAPrioriSourceSsmFileName());
  leExtAPrioriErp_->setText(config_->getExtAPrioriErpFileName());
  leExtAPrioriAxisOffsets_->setText(config_->getExtAPrioriAxisOffsetsFileName());
  leExtAPrioriMeanGradients_->setText(config_->getExtAPrioriMeanGradientsFileName());
  
  if (config_->getHave2ApplyPxyOceanTideHFContrib())
    config_->setUseExtAPrioriHiFyErp(false);
  leExtAPrioriHiFyEop_->setText(config_->getExtAPrioriHiFyErpFileName());
  leEccentricities_->setText(config_->getEccentricitiesFileName());

  if (cbH2AppSourceSsm_->isEnabled())
    cbH2AppSourceSsm_->setCheckState(config_->getHave2ApplySourceSsm()?Qt::Checked:Qt::Unchecked);

  if (cbH2AppPolarMotion_->isEnabled())
    cbH2AppPolarMotion_->setCheckState(config_->getHave2ApplyPxContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppEarthTide_->isEnabled())
    cbH2AppEarthTide_->setCheckState(config_->getHave2ApplyEarthTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppOceanTide_->isEnabled())
    cbH2AppOceanTide_->setCheckState(config_->getHave2ApplyOceanTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppPoleTide_->isEnabled())
    cbH2AppPoleTide_->setCheckState(config_->getHave2ApplyPoleTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppOceanPoleTideLd_->isEnabled())
    cbH2AppOceanPoleTideLd_->
      setCheckState(config_->getHave2ApplyOceanPoleTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppGpsIono_->isEnabled())
    cbH2AppGpsIono_->
      setCheckState(config_->getHave2ApplyGpsIonoContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppOldOceanTide_->isEnabled())
    cbH2AppOldOceanTide_->
      setCheckState(config_->getHave2ApplyOldOceanTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppOldPoleTide_->isEnabled())
    cbH2AppOldPoleTide_->
      setCheckState(config_->getHave2ApplyOldPoleTideContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppHfTidalPxy_->isEnabled())
    cbH2AppHfTidalPxy_->
      setCheckState(config_->getHave2ApplyPxyOceanTideHFContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppHfTidalUt1_->isEnabled())
    cbH2AppHfTidalUt1_->
      setCheckState(config_->getHave2ApplyUt1OceanTideHFContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppHfLibrationPxy_ && cbH2AppHfLibrationPxy_->isEnabled())
    cbH2AppHfLibrationPxy_->
      setCheckState(config_->getHave2ApplyPxyLibrationContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppHfLibrationUt1_ && cbH2AppHfLibrationUt1_->isEnabled())
    cbH2AppHfLibrationUt1_->setCheckState(
      config_->getHave2ApplyUt1LibrationContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppHfNutation_ && cbH2AppHfNutation_->isEnabled())
    cbH2AppHfNutation_->
      setCheckState(config_->getHave2ApplyNutationHFContrib()?Qt::Checked:Qt::Unchecked);

  if (cbH2AppFeedCorr_->isEnabled())
    cbH2AppFeedCorr_->setCheckState(config_->getHave2ApplyFeedCorrContrib()?Qt::Checked:Qt::Unchecked);
  if (cbH2AppTiltRmvr_->isEnabled())
    cbH2AppTiltRmvr_->setCheckState(config_->getHave2ApplyTiltRemvrContrib()?Qt::Checked:Qt::Unchecked);
  //
  cbExtAPrioriSitesPositions_->
    setCheckState(config_->getUseExtAPrioriSitesPositions()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSitesVelocities_->
    setCheckState(config_->getUseExtAPrioriSitesVelocities()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSourcesPositions_->
    setCheckState(config_->getUseExtAPrioriSourcesPositions()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriSourcesSsm_->
    setCheckState(config_->getUseExtAPrioriSourceSsm()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriErp_->setCheckState(config_->getUseExtAPrioriErp()? Qt::Checked:Qt::Unchecked);
  cbExtAPrioriAxisOffsets_->
    setCheckState(config_->getUseExtAPrioriAxisOffsets()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriHiFyEop_->setCheckState(config_->getUseExtAPrioriHiFyErp()?Qt::Checked:Qt::Unchecked);
  cbExtAPrioriMeanGradients_->
    setCheckState(config_->getUseExtAPrioriMeanGradients()?Qt::Checked:Qt::Unchecked);
  //
  leExtAPrioriSitesPositions_->setEnabled(config_->getUseExtAPrioriSitesPositions());
  leExtAPrioriSitesVelocities_->setEnabled(config_->getUseExtAPrioriSitesVelocities());
  leExtAPrioriSourcesPositions_->setEnabled(config_->getUseExtAPrioriSourcesPositions());
  leExtAPrioriSourcesSsm_->setEnabled(config_->getUseExtAPrioriSourceSsm());
  leExtAPrioriErp_->setEnabled(config_->getUseExtAPrioriErp());
  leExtAPrioriAxisOffsets_->setEnabled(config_->getUseExtAPrioriAxisOffsets());
  leExtAPrioriHiFyEop_->setEnabled(config_->getUseExtAPrioriHiFyErp());
  leExtAPrioriMeanGradients_->setEnabled(config_->getUseExtAPrioriMeanGradients());
};



//
void SgGuiTaskConfig::netIdCurrentIndexChanged(const QString& str)
{
  SgTaskConfig::AutomaticProcessing ap;
  if (config_->apByNetId().contains(str))
  {
    ap = config_->apByNetId().value(str);
    gbAutoAction_->setEnabled(true);
    gbApFinalSolution_->setEnabled(true);
    pbApCreate_->setEnabled(false);
    pbApDestroy_->setEnabled(str!=defaultNetIdName);
    config_->setLastModifiedNetId(str);
  }
  else
  {
    gbAutoAction_->setEnabled(false);
    gbApFinalSolution_->setEnabled(false);
    pbApCreate_->setEnabled(true);
    pbApDestroy_->setEnabled(false);
  };
  cbApDoSessionSetup_->setCheckState(ap.doSessionSetup_?Qt::Checked:Qt::Unchecked);
  cbApDoIonoCorrection4SBD_->setCheckState(ap.doIonoCorrection4SBD_?Qt::Checked:Qt::Unchecked);
  cbApDoAmbigResolution_->setCheckState(ap.doAmbigResolution_?Qt::Checked:Qt::Unchecked);
  cbApDoClockBreaksDetection_->setCheckState(ap.doClockBreaksDetection_?Qt::Checked:Qt::Unchecked);
  cbApDoIonoCorrection4All_->setCheckState(ap.doIonoCorrection4All_?Qt::Checked:Qt::Unchecked);
  cbApDoOutliers_->setCheckState(ap.doOutliers_?Qt::Checked:Qt::Unchecked);
  cbApDoWeights_->setCheckState(ap.doWeights_?Qt::Checked:Qt::Unchecked);
  cbApDoReportNotUsedData_->setCheckState(ap.doReportNotUsedData_?Qt::Checked:Qt::Unchecked);
  bgApFinalSolution_->button(ap.finalSolution_)->setChecked(true);
};



//
void SgGuiTaskConfig::changeAA_SessionSetup(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doSessionSetup_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_Iono4SBD(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doIonoCorrection4SBD_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_Ambigs(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doAmbigResolution_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_ClockBreaks(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doClockBreaksDetection_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_Iono4All(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doIonoCorrection4All_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_Outliers(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doOutliers_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_Reweighting(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doWeights_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_ReportNotUsed(int state)
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
    config_->apByNetId()[str].doReportNotUsedData_ = state==Qt::Checked;
};



//
void SgGuiTaskConfig::changeAA_FinalSolution(int n)
{
  QString                       str=cbNetID_->currentText();
  if (!config_->apByNetId().contains(str))
    return;
  switch (n)
  {
  default:
  case 0:
    config_->apByNetId()[str].finalSolution_ = SgTaskConfig::AutomaticProcessing::FS_BASELINE;
    break;
  case 1:
    config_->apByNetId()[str].finalSolution_ = SgTaskConfig::AutomaticProcessing::FS_UT1;
    break;
  };
};



//
void SgGuiTaskConfig::createAutomaticProcessing4NetId()
{
  QString                       str=cbNetID_->currentText();
  if (!config_->apByNetId().contains(str))
  {
    SgTaskConfig::AutomaticProcessing ap;
    config_->apByNetId()[str] = ap;
    gbAutoAction_->setEnabled(true);
    gbApFinalSolution_->setEnabled(true);
    pbApCreate_->setEnabled(false);
    pbApDestroy_->setEnabled(true);
    cbApDoSessionSetup_->setCheckState(ap.doSessionSetup_?Qt::Checked:Qt::Unchecked);
    cbApDoIonoCorrection4SBD_->setCheckState(ap.doIonoCorrection4SBD_?Qt::Checked:Qt::Unchecked);
    cbApDoAmbigResolution_->setCheckState(ap.doAmbigResolution_?Qt::Checked:Qt::Unchecked);
    cbApDoClockBreaksDetection_->setCheckState(ap.doClockBreaksDetection_?Qt::Checked:Qt::Unchecked);
    cbApDoIonoCorrection4All_->setCheckState(ap.doIonoCorrection4All_?Qt::Checked:Qt::Unchecked);
    cbApDoOutliers_->setCheckState(ap.doOutliers_?Qt::Checked:Qt::Unchecked);
    cbApDoWeights_->setCheckState(ap.doWeights_?Qt::Checked:Qt::Unchecked);
    cbApDoReportNotUsedData_->setCheckState(ap.doReportNotUsedData_?Qt::Checked:Qt::Unchecked);
    bgApFinalSolution_->button(ap.finalSolution_)->setChecked(true);
    config_->setLastModifiedNetId(str);
  };
};



//
void SgGuiTaskConfig::destroyAutomaticProcessing4NetId()
{
  QString                       str=cbNetID_->currentText();
  if (config_->apByNetId().contains(str))
  {
    SgTaskConfig::AutomaticProcessing ap;
    cbApDoSessionSetup_->setCheckState(ap.doSessionSetup_?Qt::Checked:Qt::Unchecked);
    cbApDoIonoCorrection4SBD_->setCheckState(ap.doIonoCorrection4SBD_?Qt::Checked:Qt::Unchecked);
    cbApDoAmbigResolution_->setCheckState(ap.doAmbigResolution_?Qt::Checked:Qt::Unchecked);
    cbApDoClockBreaksDetection_->setCheckState(ap.doClockBreaksDetection_?Qt::Checked:Qt::Unchecked);
    cbApDoIonoCorrection4All_->setCheckState(ap.doIonoCorrection4All_?Qt::Checked:Qt::Unchecked);
    cbApDoOutliers_->setCheckState(ap.doOutliers_?Qt::Checked:Qt::Unchecked);
    cbApDoWeights_->setCheckState(ap.doWeights_?Qt::Checked:Qt::Unchecked);
    cbApDoReportNotUsedData_->setCheckState(ap.doReportNotUsedData_?Qt::Checked:Qt::Unchecked);
    bgApFinalSolution_->button(ap.finalSolution_)->setChecked(true);
    config_->apByNetId().remove(str);
    gbAutoAction_->setEnabled(false);
    gbApFinalSolution_->setEnabled(false);
    pbApCreate_->setEnabled(true);
    pbApDestroy_->setEnabled(false);
    config_->setLastModifiedNetId(defaultNetIdName);
  };
};



//
void SgGuiTaskConfig::editParameterCfg4Clocks()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_CLOCK_0)),
    SgParametersDescriptor::Idx_CLOCK_0);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4Zenith()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_ZENITH)),
    SgParametersDescriptor::Idx_ZENITH);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4AtmGrd()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_ATMGRAD)),
    SgParametersDescriptor::Idx_ATMGRAD);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4StnPos()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_STNCOO)),
    SgParametersDescriptor::Idx_STNCOO);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4AxsOfs()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_AXISOFFSET)),
    SgParametersDescriptor::Idx_AXISOFFSET);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4SrcPos()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_SRCCOO)),
    SgParametersDescriptor::Idx_SRCCOO);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4SrcSsm()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_SRCSSM)),
    SgParametersDescriptor::Idx_SRCSSM);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopUt1()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSUT1)),
    SgParametersDescriptor::Idx_POLUSUT1);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopUt1Rate()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSUT1R)),
    SgParametersDescriptor::Idx_POLUSUT1R);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopPolar()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSXY)),
    SgParametersDescriptor::Idx_POLUSXY);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopPolarRate()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSXYR)),
    SgParametersDescriptor::Idx_POLUSXYR);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopNutation()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSNUT)),
    SgParametersDescriptor::Idx_POLUSNUT);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4EopNutationRate()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_POLUSNUTR)),
    SgParametersDescriptor::Idx_POLUSNUTR);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4BlnClock()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_BL_CLK)),
    SgParametersDescriptor::Idx_BL_CLK);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4BlnLength()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_BL_LENGTH)),
    SgParametersDescriptor::Idx_BL_LENGTH);
  e->show();
};



//
void SgGuiTaskConfig::editParameterCfg4Test()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(
    &(parametersDescriptor_->parameter(SgParametersDescriptor::Idx_TEST)),
    SgParametersDescriptor::Idx_TEST);
  e->show();
};



//
void SgGuiTaskConfig::modifyDelayType(int n)
{
  SgTaskConfig::VlbiDelayType   dType;
  switch (n)
  {
  default:
  case 0:
    dType = SgTaskConfig::VD_NONE;
    break;
  case 1:
    dType = SgTaskConfig::VD_SB_DELAY;
    break;
  case 2:
    dType = SgTaskConfig::VD_GRP_DELAY;
    break;
  case 3:
    dType = SgTaskConfig::VD_PHS_DELAY;
    break;
  };
  config_->setUseDelayType(dType);
  emit delayTypeModified(dType);
};



//
void SgGuiTaskConfig::modifyRateType(int n)
{
  SgTaskConfig::VlbiRateType    rType;
  switch (n)
  {
  default:
  case 0:
    rType = SgTaskConfig::VR_NONE;
    for (int i=0; i<stcParsButtons_.size(); i++)
      stcParsButtons_.at(i)->setEnabled(true);
    break;
  case 1:
    rType = SgTaskConfig::VR_PHS_RATE;
    for (int i=0; i<stcParsButtons_.size(); i++)
      stcParsButtons_.at(i)->setEnabled(false);
    break;
  };
  config_->setUseRateType(rType);
  emit rateTypeModified(rType);
};



//
void SgGuiTaskConfig::modifyReweightingMode(int n)
{
  switch (n)
  {
  default:
  case 0:
    config_->setWcMode(SgTaskConfig::WCM_BAND);
    break;
  case 1:
    config_->setWcMode(SgTaskConfig::WCM_BASELINE);
    break;
  };
};



//
void SgGuiTaskConfig::modifyEstimatorPwlMode(int n)
{
  switch (n)
  {
  default:
  case 0:
    config_->setPwlMode(SgTaskConfig::EPM_INCRATE);
    break;
  case 1:
    config_->setPwlMode(SgTaskConfig::EPM_BSPLINE_LINEA);
    break;
  case 2:
    config_->setPwlMode(SgTaskConfig::EPM_BSPLINE_QUADR);
    break;
  };
};



//
void SgGuiTaskConfig::outliersProcessingActionModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    config_->setOpAction(SgTaskConfig::OPA_ELIMINATE);
    break;
  case 1:
    config_->setOpAction(SgTaskConfig::OPA_RESTORE);
    break;
  };
};



//
void SgGuiTaskConfig::outliersProcessingModeModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    config_->setOpMode(SgTaskConfig::OPM_BAND);
    break;
  case 1:
    config_->setOpMode(SgTaskConfig::OPM_BASELINE);
    break;
  };
};



//
void SgGuiTaskConfig::outliersProcessingThresholdModified(const QString& str)
{
  bool                          isOK;
  double                        d;
  d = str.toDouble(&isOK);
  if (isOK && d!=config_->getOpThreshold())
  {
    config_->setOpThreshold(d);
    isOutliersProcessingThresholdModified_ = true;
  };
};



//
void SgGuiTaskConfig::outliersProcessingIterationsLimitModified(const QString& str)
{
  bool                          isOK;
  int                           n;
  n = str.toInt(&isOK);
  if (isOK && n!=config_->getOpIterationsLimit())
    config_->setOpIterationsLimit(n);
};



//
void SgGuiTaskConfig::qualityCodeThresholdModified(int q)
{
  config_->setQualityCodeThreshold(q);
};



//
void SgGuiTaskConfig::goodQualityCodeAtStartupModified(int q)
{
  config_->setGoodQualityCodeAtStartup(q);
};



//
void SgGuiTaskConfig::updateDelayTypeSelection(int n)
{
  QAbstractButton              *b=bgDelTypes_->button(n);
  if (b)
    b->setChecked(true);
  //
  if (false && !isOutliersProcessingThresholdModified_) // if user changed it once, do not interfere!
  {
    double          d(5.0);
    if (n==SgTaskConfig::VD_GRP_DELAY)
      d = 3.0;
    config_->setOpThreshold(d);
    leOPThreshold_->setText(QString("").sprintf("%.2f", config_->getOpThreshold()));
  };
};



//
void SgGuiTaskConfig::updateRateTypeSelection(int n)
{
  QAbstractButton              *b=bgRatTypes_->button(n);
  if (b)
    b->setChecked(true);
};



//
void SgGuiTaskConfig::band2UseModified(int n)
{
  config_->setActiveBandIdx(n);
  if (config_->getIsActiveBandFollowsTab())
    emit activeBandModified(n);
};



//
void SgGuiTaskConfig::updateActiveBandSelection(int n)
{
  QAbstractButton *b = bgBands_->button(n);
  if (b)
    b->setChecked(true);
};
  


//
void SgGuiTaskConfig::paramaters4ClocksModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setClock0Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock1Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock2Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock3Mode(SgParameterCfg::PM_NONE);
    emit clocksModelChanged(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setClock0Mode(SgParameterCfg::PM_LOC);
    parametersDescriptor_->setClock1Mode(SgParameterCfg::PM_LOC);
    parametersDescriptor_->setClock2Mode(SgParameterCfg::PM_LOC);
    parametersDescriptor_->setClock3Mode(SgParameterCfg::PM_LOC);
    emit clocksModelChanged(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setClock0Mode(SgParameterCfg::PM_ARC);
    parametersDescriptor_->setClock1Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock2Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock3Mode(SgParameterCfg::PM_NONE);
    emit clocksModelChanged(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setClock0Mode(SgParameterCfg::PM_PWL);
    parametersDescriptor_->setClock1Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock2Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock3Mode(SgParameterCfg::PM_NONE);
    emit clocksModelChanged(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setClock0Mode(SgParameterCfg::PM_STC);
    parametersDescriptor_->setClock1Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock2Mode(SgParameterCfg::PM_NONE);
    parametersDescriptor_->setClock3Mode(SgParameterCfg::PM_NONE);
    emit clocksModelChanged(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4ZenithModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setZenithMode(SgParameterCfg::PM_NONE);
    emit zenithModelChanged(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setZenithMode(SgParameterCfg::PM_LOC);
    emit zenithModelChanged(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setZenithMode(SgParameterCfg::PM_ARC);
    emit zenithModelChanged(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setZenithMode(SgParameterCfg::PM_PWL);
    emit zenithModelChanged(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setZenithMode(SgParameterCfg::PM_STC);
    emit zenithModelChanged(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4AtmGrdModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setAtmGradMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setAtmGradMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setAtmGradMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setAtmGradMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setAtmGradMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4StnPosModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setStnCooMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setStnCooMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setStnCooMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setStnCooMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setStnCooMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4AxsOfsModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setAxisOffsetMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setAxisOffsetMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setAxisOffsetMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setAxisOffsetMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setAxisOffsetMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4SrcPosModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setSrcCooMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setSrcCooMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setSrcCooMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setSrcCooMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setSrcCooMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4SrcSsmModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setSrcSsmMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setSrcSsmMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setSrcSsmMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setSrcSsmMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setSrcSsmMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopUt1Modified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusUT1Mode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusUT1Mode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusUT1Mode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusUT1Mode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusUT1Mode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopUt1RateModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusUT1RateMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusUT1RateMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusUT1RateMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusUT1RateMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusUT1RateMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopPolarMotionModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusXYMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusXYMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusXYMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusXYMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusXYMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopPolarMotionRateModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusXYRateMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusXYRateMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusXYRateMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusXYRateMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusXYRateMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopNutationModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusNutMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusNutMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusNutMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusNutMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusNutMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4EopNutationRateModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setPolusNutRateMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setPolusNutRateMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setPolusNutRateMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setPolusNutRateMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setPolusNutRateMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4BlnClockModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setBlClockMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setBlClockMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setBlClockMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setBlClockMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setBlClockMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4BlnLengthModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setBlLengthMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setBlLengthMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setBlLengthMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setBlLengthMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setBlLengthMode(SgParameterCfg::PM_STC);
    break;
  };
};



//
void SgGuiTaskConfig::paramaters4TestModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    parametersDescriptor_->setTestMode(SgParameterCfg::PM_NONE);
    break;
  case 1:
    parametersDescriptor_->setTestMode(SgParameterCfg::PM_LOC);
    break;
  case 2:
    parametersDescriptor_->setTestMode(SgParameterCfg::PM_ARC);
    break;
  case 3:
    parametersDescriptor_->setTestMode(SgParameterCfg::PM_PWL);
    break;
  case 4:
    parametersDescriptor_->setTestMode(SgParameterCfg::PM_STC);
    break;
  };
};


//
void SgGuiTaskConfig::checkStcParChackBoxesStatus(bool isChecked)
{
  if (isChecked && cbDoReweighting_->isEnabled())
  {
    cbDoReweighting_->setEnabled(false);
    for (int i=0; i<ratTypeButtons_.size(); i++)
      ratTypeButtons_.at(i)->setEnabled(false);
  }
  else
  {
    bool                        canBeEnabled(true);
    for (int i=0; i<stcParsButtons_.size(); i++)
      if (stcParsButtons_.at(i)->isChecked())
        canBeEnabled = false;
    if (canBeEnabled)
    {
      cbDoReweighting_->setEnabled(true);
      for (int i=0; i<ratTypeButtons_.size(); i++)
        ratTypeButtons_.at(i)->setEnabled(true);
    };
  };
};



//
void SgGuiTaskConfig::changePRA_Iono4SBD(int chkState)
{
  config_->setDoIonoCorrection4SBD(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changePRA_CBDetectionMode1(int chkState)
{
  config_->setDoClockBreakDetectionMode1(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changePRA_OutliersEliminationMode1(int chkState)
{
  config_->setDoOutliersEliminationMode1(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeGUI_activeBand(int chkState)
{
  config_->setIsActiveBandFollowsTab(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeGUI_observable(int chkState)
{
  config_->setIsObservableFollowsPlot(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeIsNoviceUser(int chkState)
{
  config_->setIsNoviceUser(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeHave2outputCovarMatrix(int chkState)
{
  config_->setHave2outputCovarMatrix(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeIsIonosphereFreeUncorrelated(int chkState)
{
  config_->setIsIonosphereFreeUncorrelated(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeUseOldMode4IonosphereSigma(int chkState)
{
  config_->setUseOldMode4IonosphereSigma(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeIsTesting(int chkState)
{
  config_->setIsTesting(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeUseQualityCodeG(int chkState)
{
  config_->setUseQualityCodeG(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeUseQualityCodeH(int chkState)
{
  config_->setUseQualityCodeH(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeCompatibility(int chkState)
{
  config_->setIsSolveCompatible(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeDynamicClockBreaks(int chkState)
{
  config_->setUseDynamicClockBreaks(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeUseGoodQualityCodeAtStartup(int chkState)
{
  config_->setUseGoodQualityCodeAtStartup(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeCompat_UseSolveElimFlags(int chkState)
{
  config_->setUseSolveObsSuppresionFlags(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeRTA_WeightCorrection(int chkState)
{
  bool                          have2=chkState==Qt::Checked;
  config_->setDoWeightCorrection(have2);
  for (int i=0; i<stcParsButtons_.size(); i++)
    stcParsButtons_.at(i)->setEnabled(!have2 && config_->getUseRateType()==SgTaskConfig::VR_NONE);
  cbUseExtWeights_->setEnabled(!have2);
};



//
void SgGuiTaskConfig::changeRTA_UseExternalWeights(int chkState)
{
  bool                          have2=chkState==Qt::Checked;
  config_->setUseExternalWeights(have2);
  cbDoReweighting_->setEnabled(!have2);
  leUseExtWeights_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_WeightsFileName(const QString& str)
{
  config_->setExtWeightsFileName(str);
};



//
void SgGuiTaskConfig::changeAS_ini4del(const QString& str)
{
  bool                          isOk;
  double                        s;
  s = str.toDouble(&isOk);
  if (isOk)
    config_->setInitAuxSigma4Delay(s*1.0e-12);
  else
    leIniSigma4Del_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Delay()*1.0e12));
};



//
void SgGuiTaskConfig::changeAS_ini4rat(const QString& str)
{
  bool                          isOk;
  double                        s;
  s = str.toDouble(&isOk);
  if (isOk)
    config_->setInitAuxSigma4Rate(s*1.0e-15);
  else
    leIniSigma4Rat_->setText(QString("").sprintf("%g", config_->getInitAuxSigma4Rate()*1.0e15));
};



//
void SgGuiTaskConfig::changeAS_min4del(const QString& str)
{
  bool                          isOk;
  double                        s;
  s = str.toDouble(&isOk);
  if (isOk)
    config_->setMinAuxSigma4Delay(s*1.0e-12);
  else
    leMinSigma4Del_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Delay()*1.0e12));
};



void SgGuiTaskConfig::changeAS_min4rat(const QString& str)
{
  bool                          isOk;
  double                        s;
  s = str.toDouble(&isOk);
  if (isOk)
    config_->setMinAuxSigma4Rate(s*1.0e-15);
  else
    leMinSigma4Rat_->setText(QString("").sprintf("%g", config_->getMinAuxSigma4Rate()*1.0e15));
};



//
void SgGuiTaskConfig::changeOP_WcSuppresion(int chkState)
{
  config_->setOpHave2SuppressWeightCorrection(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeOP_IsSolveCompatible(int chkState)
{
  config_->setOpIsSolveCompatible(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeOP_DoNotNormalize(int chkState)
{
  config_->setOpHave2NormalizeResiduals(chkState==Qt::Unchecked);
};



//
void SgGuiTaskConfig::firstObsChanged(int idx)
{
  leFirstObs2Process_->setEnabled(idx != 0);
  if (idx == 0)
    config_->setT2Bgn(tZero);
  else
    changeFirstObs();
};



//
void SgGuiTaskConfig::lastObsChanged(int idx)
{
  leLastObs2Process_->setEnabled(idx != 0);
  if (idx == 0)
    config_->setT2End(tInf);
  else
    changeLastObs();
};



//
void SgGuiTaskConfig::changeFirstObs()
{
  QString                       txt=leFirstObs2Process_->text();
  SgMJD                         t;
  if (t.fromString(SgMJD::F_YYYYMMDDHHMMSSSS, txt))
    config_->setT2Bgn(t);
  else
  {
    config_->setT2Bgn(tZero);
    leFirstObs2Process_->setText(session_->getTStart().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    bgFirstObs_->button(0)->setChecked(true);
    leFirstObs2Process_->setEnabled(false);
  };
};



//
void SgGuiTaskConfig::changeLastObs()
{
  QString                       txt=leLastObs2Process_->text();
  SgMJD                         t;
  if (t.fromString(SgMJD::F_YYYYMMDDHHMMSSSS, txt))
    config_->setT2End(t);
  else
  {
    config_->setT2End(tInf);
    leLastObs2Process_->setText(session_->getTFinis().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    bgLastObs_->button(0)->setChecked(true);
    leLastObs2Process_->setEnabled(false);
  };
};



//
void SgGuiTaskConfig::changeEF_SitePositions(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriSitesPositions(have2);
  leExtAPrioriSitesPositions_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_SitePositionsFileName(const QString& str)
{
  config_->setExtAPrioriSitesPositionsFileName(str);
};



//
void SgGuiTaskConfig::changeEF_SiteVelocities(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriSitesVelocities(have2);
  leExtAPrioriSitesVelocities_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_SiteVelocitiesFileName(const QString& str)
{
  config_->setExtAPrioriSitesVelocitiesFileName(str);
};



//
void SgGuiTaskConfig::changeEF_SourcePositions(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriSourcesPositions(have2);
  leExtAPrioriSourcesPositions_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_SourcePositionsFileName(const QString& str)
{
  config_->setExtAPrioriSourcesPositionsFileName(str);
};



//
void SgGuiTaskConfig::changeEF_SourceSsm(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriSourceSsm(have2);
  leExtAPrioriSourcesSsm_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_SourceSsmFileName(const QString& str)
{
  config_->setExtAPrioriSourceSsmFileName(str);
};



//
void SgGuiTaskConfig::changeEF_Eop(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriErp(have2);
  leExtAPrioriErp_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_EopFileName(const QString& str)
{
  config_->setExtAPrioriErpFileName(str);
};



//
void SgGuiTaskConfig::changeEF_AxisOffset(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriAxisOffsets(have2);
  leExtAPrioriAxisOffsets_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_AxisOffsetFileName(const QString& str)
{
  config_->setExtAPrioriAxisOffsetsFileName(str);
};



//
void SgGuiTaskConfig::changeEF_HiFyEop(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriHiFyErp(have2);
  leExtAPrioriHiFyEop_->setEnabled(have2);
  cbH2AppHfTidalPxy_->setEnabled(!have2);
  cbH2AppHfTidalUt1_->setEnabled(!have2);
};



//
void SgGuiTaskConfig::changeEF_HiFyEopFileName(const QString& str)
{
  config_->setExtAPrioriHiFyErpFileName(str);
};



//
void SgGuiTaskConfig::changeEF_MeanGradients(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setUseExtAPrioriMeanGradients(have2);
  leExtAPrioriMeanGradients_->setEnabled(have2);
};



//
void SgGuiTaskConfig::changeEF_MeanGradientsFileName(const QString& str)
{
  config_->setExtAPrioriMeanGradientsFileName(str);
};



//
void SgGuiTaskConfig::changeEF_EccentricitiesFileName(const QString& str)
{
  config_->setEccentricitiesFileName(str);
};




//
void SgGuiTaskConfig::changeODC_SourceSsm(int chkState)
{
  config_->setHave2ApplySourceSsm(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::modifyRefractionModel(int idx)
{
  config_->setRefractionModel((SgTaskConfig::RefractionModel)idx);
  switch (idx)
  {
  default:
  case SgTaskConfig::RM_NONE:
    //config_->setRefractionModel(SgTaskConfig::RM_NONE);
    break;
  case SgTaskConfig::RM_CONTRIB_EXTERNAL:
    break;
  case SgTaskConfig::RM_CONTRIB_INTERNAL:
    break;
  case SgTaskConfig::RM_FLYBY_NMF:
    break;
  case SgTaskConfig::RM_FLYBY_MTT:
    break;
  };
};



//
void SgGuiTaskConfig::flybySourceCccFModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    config_->setFlybyCableCalSource(SgTaskConfig::CCS_DEFAULT);
    break;
  case 1:
    config_->setFlybyCableCalSource(SgTaskConfig::CCS_FSLG);
    break;
  case 2:
    config_->setFlybyCableCalSource(SgTaskConfig::CCS_CDMS);
    break;
  case 3:
    config_->setFlybyCableCalSource(SgTaskConfig::CCS_PCMT);
    break;
  };
};



//
void SgGuiTaskConfig::changeODC_PolarMotion(int chkState)
{
  bool have=chkState==Qt::Checked;
  config_->setHave2ApplyPxContrib(have);
  config_->setHave2ApplyPyContrib(have);
};



//
void SgGuiTaskConfig::changeODC_SolidEarthTide(int chkState)
{
  config_->setHave2ApplyEarthTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_OceanTide(int chkState)
{
  config_->setHave2ApplyOceanTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_PoleTide(int chkState)
{
  config_->setHave2ApplyPoleTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_HiFqTidalPxy(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setHave2ApplyPxyOceanTideHFContrib(have2);
  cbExtAPrioriHiFyEop_->setEnabled(!have2);
};



//
void SgGuiTaskConfig::changeODC_HiFqTidalUt1(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setHave2ApplyUt1OceanTideHFContrib(have2);
  cbExtAPrioriHiFyEop_->setEnabled(!have2);
};



//
void SgGuiTaskConfig::changeODC_HiFqLibrationPxy(int chkState)
{
  bool have2=chkState==Qt::Checked;
  config_->setHave2ApplyPxyLibrationContrib(have2);
};



//
void SgGuiTaskConfig::changeODC_HiFqLibrationUt1(int chkState)
{
  config_->setHave2ApplyUt1LibrationContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_HiFqNutation(int chkState)
{
  config_->setHave2ApplyNutationHFContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_OceanPoleTide(int chkState)
{
  config_->setHave2ApplyOceanPoleTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_GpsIono(int chkState)
{
  config_->setHave2ApplyGpsIonoContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_FeedCorr(int chkState)
{
  config_->setHave2ApplyFeedCorrContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_TiltRmvr(int chkState)
{
  config_->setHave2ApplyTiltRemvrContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_OldOceanTide(int chkState)
{
  config_->setHave2ApplyOldOceanTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_OldPoleTide(int chkState)
{
  config_->setHave2ApplyOldPoleTideContrib(chkState==Qt::Checked);
};



//
void SgGuiTaskConfig::changeODC_AxisOffset(int chkState)
{
  config_->setHave2ApplyAxisOffsetContrib(chkState==Qt::Checked);
};



/*
//
void SgGuiTaskConfig::changeODC_TropRefractionDry(int chkState)
{
  bool                          isOn=chkState==Qt::Checked;
  config_->setHave2ApplyNdryContrib(isOn);
  isOn = isOn || config_->getHave2ApplyNwetContrib();
  for (int i=0; i<bgFlybyRefrMapF_->buttons().size(); i++)  
    bgFlybyRefrMapF_->buttons()[i]->setEnabled(!isOn);
};



//
void SgGuiTaskConfig::changeODC_TropRefractionWet(int chkState)
{
  bool                          isOn=chkState==Qt::Checked;
  config_->setHave2ApplyNwetContrib(isOn);
  isOn = isOn || config_->getHave2ApplyNdryContrib();
  for (int i=0; i<bgFlybyRefrMapF_->buttons().size(); i++)  
    bgFlybyRefrMapF_->buttons()[i]->setEnabled(!isOn);
};
*/


//
void SgGuiTaskConfig::changeDoDownWeight(int chkState)
{
  config_->setDoDownWeight(chkState==Qt::Checked);
};



//
//
void SgGuiTaskConfig::toggleEntryMoveEnable(QTreeWidgetItem* item, int n, 
  Qt::MouseButton /* mouseButtonState */, Qt::KeyboardModifiers /* modifiers */)
{
  if (!item) // may be NULL
    return;

  int                           idx=((SgGuiTcUserCorrectionItem*)item)->getIdx();
  QString                       str("");
  switch (n)
  {
  case 0:
    break;
  case 1:
    session_->userCorrectionsUse()[idx] = !session_->userCorrectionsUse()[idx];
    item->setText(1, session_->getUserCorrectionsUse().at(idx)?"Y":"");
    break;
  default:
    //std::cout << "default, n=" << n << "\n";
    break;
  };
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*              SgGuiTaskConfigDialog's METHODS:
* 
*======================================================================================================*/
//
SgGuiTaskConfigDialog::SgGuiTaskConfigDialog(SgTaskConfig* config, SgParametersDescriptor* parDescriptor,
  SgVlbiSession* session, QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  config_ = config;
  parametersDescriptor_ = parDescriptor;
  session_ = session;

  QPushButton   *button;
  QBoxLayout    *layout, *subLayout;
  QFrame        *bar;

  layout = new QVBoxLayout(this);
  configWidget_ = new SgGuiTaskConfig(config_, parametersDescriptor_, session_, this);
  layout->addWidget(configWidget_);

  bar = new QFrame(this);
  bar->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  layout->addWidget(bar);

  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);
  
  subLayout->addWidget(button=new QPushButton("Close", this));
  button->setMinimumSize(button->sizeHint());
  connect(button, SIGNAL(clicked()), SLOT(accept()));

  setWindowTitle("General Configure Editor");
};



//
SgGuiTaskConfigDialog::~SgGuiTaskConfigDialog()
{
  delete configWidget_;
  configWidget_ = NULL;
  config_ = NULL;
  parametersDescriptor_ = NULL;
  session_ = NULL;
  emit windowClosed();
};



//
void SgGuiTaskConfigDialog::accept()
{
  QDialog::accept();
  deleteLater();
};
/*=====================================================================================================*/
