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

#ifndef SG_GUI_TASK_CONFIG_H
#define SG_GUI_TASK_CONFIG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include <SgParameterCfg.h>
#include <SgParametersDescriptor.h>
#include <SgTaskConfig.h>
#include <SgVlbiSession.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#else
#   include <QtGui/QDialog>
#endif

#include <QtWidgets/QTreeWidgetItem>

#include <SgGuiQTreeWidgetExt.h>


class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDialog;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;






/*=====================================================================================================*/
class SgGuiTcUserCorrectionItem : public QTreeWidgetItem
{
public:
   SgGuiTcUserCorrectionItem(QTreeWidget *parent, int idx, int type=Type) : 
    QTreeWidgetItem(parent, type) {idx_=idx;};
  virtual ~SgGuiTcUserCorrectionItem() {};
  void setIdx(int idx) {idx_ = idx;};
  int getIdx() const {return idx_;};
  //  virtual bool operator<(const QTreeWidgetItem &other) const;
private:
  int                           idx_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class SgGuiTaskConfig : public QWidget
{
  Q_OBJECT

public:
   SgGuiTaskConfig(SgTaskConfig*, SgParametersDescriptor*, SgVlbiSession*, QWidget*,
                  Qt::WindowFlags=Qt::Widget);
  virtual inline ~SgGuiTaskConfig()
    {config_=NULL; parametersDescriptor_=NULL; session_=NULL; stcParsButtons_.clear();
      if (tweUserCorrections_) {delete tweUserCorrections_;}; tweUserCorrections_=NULL;};

  inline QString className() const {return "SgGuiTaskConfig";};



signals:
  void delayTypeModified(SgTaskConfig::VlbiDelayType);
  void rateTypeModified(SgTaskConfig::VlbiRateType);
  void activeBandModified(int);
  void clocksModelChanged(SgParameterCfg::PMode);
  void zenithModelChanged(SgParameterCfg::PMode);



public slots:
  void updateDelayTypeSelection(int);
  void updateRateTypeSelection(int);
  void updateActiveBandSelection(int);
  void displayParametersDesriptor();
  void displayConfig();



private slots:
  void toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void modifyDelayType(int);
  void modifyRateType(int);
  void modifyReweightingMode(int);
  void band2UseModified(int);
  void changeEF_WeightsFileName(const QString&);
  void changeAS_ini4del(const QString&);
  void changeAS_ini4rat(const QString&);
  void changeAS_min4del(const QString&);
  void changeAS_min4rat(const QString&);
  // station's:
  void paramaters4ClocksModified(int);
  void paramaters4ZenithModified(int);
  void paramaters4AtmGrdModified(int);
  void paramaters4StnPosModified(int);
  void paramaters4AxsOfsModified(int);
  void editParameterCfg4Clocks();
  void editParameterCfg4Zenith();
  void editParameterCfg4AtmGrd();
  void editParameterCfg4StnPos();
  void editParameterCfg4AxsOfs();
  // source's:
  void paramaters4SrcPosModified(int);
  void editParameterCfg4SrcPos();
  void paramaters4SrcSsmModified(int);
  void editParameterCfg4SrcSsm();
  // EOP:
  void paramaters4EopUt1Modified(int);
  void paramaters4EopUt1RateModified(int);
  void paramaters4EopPolarMotionModified(int);
  void paramaters4EopPolarMotionRateModified(int);
  void paramaters4EopNutationModified(int);
  void paramaters4EopNutationRateModified(int);
  void editParameterCfg4EopUt1();
  void editParameterCfg4EopUt1Rate();
  void editParameterCfg4EopPolar();
  void editParameterCfg4EopPolarRate();
  void editParameterCfg4EopNutation();
  void editParameterCfg4EopNutationRate();
  // baseline's:
  void paramaters4BlnClockModified(int);
  void paramaters4BlnLengthModified(int);
  void editParameterCfg4BlnClock();
  void editParameterCfg4BlnLength();
  // Test:
  void paramaters4TestModified(int);
  void editParameterCfg4Test();

  void modifyEstimatorPwlMode(int);
  void changeDoDownWeight(int);

  void changePRA_Iono4SBD(int);
  void changePRA_CBDetectionMode1(int);
  void changePRA_OutliersEliminationMode1(int);

  void changeGUI_activeBand(int);
  void changeGUI_observable(int);
  void changeIsNoviceUser(int);
  void changeHave2outputCovarMatrix(int);
  void changeIsIonosphereFreeUncorrelated(int);
  void changeUseOldMode4IonosphereSigma(int);
  void changeIsTesting(int);
  
  void changeUseQualityCodeG(int);
  void changeUseQualityCodeH(int);
  void changeCompatibility(int);
  void changeDynamicClockBreaks(int);
  void changeCompat_UseSolveElimFlags(int);
  void changeUseGoodQualityCodeAtStartup(int);

  void changeRTA_WeightCorrection(int);
  void changeRTA_UseExternalWeights(int);

  void checkStcParChackBoxesStatus(bool);

  void outliersProcessingActionModified(int);
  void outliersProcessingModeModified(int);
  void outliersProcessingThresholdModified(const QString&);
  void outliersProcessingIterationsLimitModified(const QString&);
  void changeOP_WcSuppresion(int);
  void changeOP_IsSolveCompatible(int);
  void changeOP_DoNotNormalize(int);
  
  void changeEF_SitePositions(int);
  void changeEF_SitePositionsFileName(const QString&);
  void changeEF_SiteVelocities(int);
  void changeEF_SiteVelocitiesFileName(const QString&);
  void changeEF_SourcePositions(int);
  void changeEF_SourcePositionsFileName(const QString&);
  void changeEF_SourceSsm(int);
  void changeEF_SourceSsmFileName(const QString&);
  void changeEF_Eop(int);
  void changeEF_EopFileName(const QString&);
  void changeEF_AxisOffset(int);
  void changeEF_AxisOffsetFileName(const QString&);
  void changeEF_HiFyEop(int);
  void changeEF_HiFyEopFileName(const QString&);
  void changeEF_MeanGradients(int);
  void changeEF_MeanGradientsFileName(const QString&);
  void changeEF_EccentricitiesFileName(const QString&);
  
  //
  void modifyRefractionModel(int);
  void flybySourceCccFModified(int n);
  
  void changeODC_SourceSsm(int);
  void changeODC_PolarMotion(int);
  void changeODC_SolidEarthTide(int);
  void changeODC_OceanTide(int);
  void changeODC_PoleTide(int);
  void changeODC_HiFqTidalPxy(int);
  void changeODC_HiFqTidalUt1(int);
  void changeODC_HiFqLibrationPxy(int);
  void changeODC_HiFqLibrationUt1(int);
  void changeODC_HiFqNutation(int);
  void changeODC_OceanPoleTide(int);
  void changeODC_GpsIono(int);
  void changeODC_FeedCorr(int);
  void changeODC_TiltRmvr(int);
  void changeODC_OldOceanTide(int);
  void changeODC_OldPoleTide(int);
  void changeODC_AxisOffset(int);
//  void changeODC_TropRefractionDry(int);
//  void changeODC_TropRefractionWet(int);
  
  void qualityCodeThresholdModified(int);
  void goodQualityCodeAtStartupModified(int);
  
  //
  void netIdCurrentIndexChanged(const QString&);
  void changeAA_SessionSetup(int);
  void changeAA_Iono4SBD(int);
  void changeAA_Ambigs(int);
  void changeAA_ClockBreaks(int);
  void changeAA_Iono4All(int);
  void changeAA_Outliers(int);
  void changeAA_Reweighting(int);
  void changeAA_ReportNotUsed(int);
  void createAutomaticProcessing4NetId();
  void destroyAutomaticProcessing4NetId();
  void changeAA_FinalSolution(int);  
 
  
private:
  SgTaskConfig               *config_;
  SgParametersDescriptor     *parametersDescriptor_;
  SgVlbiSession              *session_;

  //
  QWidget                    *tabGeneralOptions_;
  QWidget                    *tabOperations_;
  QWidget                    *tabModels_;
  QWidget                    *tabPia_;
  QWidget                    *makeTab4GeneralOptions();
  QWidget                    *makeTab4Operations();
  QWidget                    *makeTab4models();
  QWidget                    *makeTab4pia();


  // saved objects (to change state):
  QButtonGroup               *bgDelTypes_;
  QButtonGroup               *bgRatTypes_;
  QList<QRadioButton*>        ratTypeButtons_;
  QButtonGroup               *bgBands_;
  QButtonGroup               *bgOPModes_;
  QCheckBox                  *cbIsActiveBandFollowsTab_;
  QCheckBox                  *cbIsObservableFollowsPlot_;
  
  QCheckBox                  *cbUseQualityCodeG_;
  QCheckBox                  *cbUseQualityCodeH_;
  QCheckBox                  *cbIsSolveCompatible_;
  QCheckBox                  *cbUseDynamicClockBreaks_;
  QCheckBox                  *cbUseSolveObsSuppresionFlags_;
  QCheckBox                  *cbUseGoodQualityCodeAtStartup_;
  QCheckBox                  *cbIsNoviceUser_;
  QCheckBox                  *cbHave2outputCovarMatrix_;
  QCheckBox                  *cbIsIonosphereFreeUncorrelated_;
  QCheckBox                  *cbUseOldMode4IonosphereSigma_;
  
  QCheckBox                  *cbIsTesting_;
  //
  QButtonGroup               *bgPwlModes_;
  QCheckBox                  *cbDoDownWeight_;
  QButtonGroup               *bgOPActions_;
  QButtonGroup               *bgRWModes_;
  
  
  // for updating parameterization:
  QButtonGroup               *bgClocks_;
  QButtonGroup               *bgZenith_;
  QButtonGroup               *bgAtmGrads_;
  QButtonGroup               *bgStnCoord_;
  QButtonGroup               *bgAxsOffset_;
  QButtonGroup               *bgSrcCoord_;
  QButtonGroup               *bgSrcSsm_;
  QButtonGroup               *bgEopPm_;
  QButtonGroup               *bgEopPmRate_;
  QButtonGroup               *bgEopUt_;
  QButtonGroup               *bgEopUtRate_;
  QButtonGroup               *bgEopNut_;
  QButtonGroup               *bgBlnClock_;
  QButtonGroup               *bgBlnVector_;
  QButtonGroup               *bgTest_;
  

  // for mutexing reweighting and stochastic estimation:
  QList<QRadioButton*>        stcParsButtons_;
  QCheckBox                  *cbDoReweighting_;
  QCheckBox                  *cbUseExtWeights_;
  QLineEdit                  *leUseExtWeights_;
  QLineEdit                  *leIniSigma4Del_;
  QLineEdit                  *leIniSigma4Rat_;
  QLineEdit                  *leMinSigma4Del_;
  QLineEdit                  *leMinSigma4Rat_;
  //
  QLineEdit                  *leOPThreshold_;
  QLineEdit                  *leOPIterationsLimit_;
  QCheckBox                  *cbSuppressWCinOP_;
  QCheckBox                  *cbOPSolveCompatible_;
  QCheckBox                  *cbOPDoNotNormalize_;
  // a priori files:
  QCheckBox                  *cbExtAPrioriSitesPositions_;
  QLineEdit                  *leExtAPrioriSitesPositions_;
  QCheckBox                  *cbExtAPrioriSitesVelocities_;
  QLineEdit                  *leExtAPrioriSitesVelocities_;
  QCheckBox                  *cbExtAPrioriSourcesPositions_;
  QLineEdit                  *leExtAPrioriSourcesPositions_;
  QCheckBox                  *cbExtAPrioriSourcesSsm_;
  QLineEdit                  *leExtAPrioriSourcesSsm_;
  QCheckBox                  *cbExtAPrioriErp_;
  QLineEdit                  *leExtAPrioriErp_;
  QCheckBox                  *cbExtAPrioriAxisOffsets_;
  QLineEdit                  *leExtAPrioriAxisOffsets_;
  QCheckBox                  *cbExtAPrioriHiFyEop_;
  QLineEdit                  *leExtAPrioriHiFyEop_;
  QCheckBox                  *cbExtAPrioriMeanGradients_;
  QLineEdit                  *leExtAPrioriMeanGradients_;
  QLineEdit                  *leEccentricities_;
  //
  // fly-bies:
  QCheckBox                  *cbH2AppSourceSsm_;
  QButtonGroup               *bgFlybyRefrMapF_;
  QButtonGroup               *bgFlybySrcCcc_;
  
  // contributions:
  QCheckBox                  *cbH2AppOceanTide_;
  QCheckBox                  *cbH2AppPoleTide_;
  QCheckBox                  *cbH2AppEarthTide_;
  QCheckBox                  *cbH2AppPolarMotion_;
  QCheckBox                  *cbH2AppAxisOffsetContrib_;
//  QCheckBox                  *cbH2AppNdryContrib_;
//  QCheckBox                  *cbH2AppNwetContrib_;
  QCheckBox                  *cbH2AppHfTidalPxy_;
  QCheckBox                  *cbH2AppHfTidalUt1_;
  QCheckBox                  *cbH2AppHfLibrationPxy_;
  QCheckBox                  *cbH2AppHfLibrationUt1_;
  QCheckBox                  *cbH2AppHfNutation_;
  QCheckBox                  *cbH2AppOceanPoleTideLd_;
  QCheckBox                  *cbH2AppGpsIono_;
  QCheckBox                  *cbH2AppFeedCorr_;
  QCheckBox                  *cbH2AppTiltRmvr_;
  QCheckBox                  *cbH2AppOldOceanTide_;
  QCheckBox                  *cbH2AppOldPoleTide_;
  /*
  QCheckBox                  *cbH2AppTropContrib_;
  */
  QComboBox                  *cbRefractionModel_;
  
  QSpinBox                   *sbQualityCodeThreshold_;
  QSpinBox                   *sbGoodQualityCodeAtStartup_;
  bool                        isOutliersProcessingThresholdModified_;
  //
  QGroupBox                  *gbAutoAction_;
  QPushButton                *pbApCreate_;
  QPushButton                *pbApDestroy_;
  QComboBox                  *cbNetID_;
  QCheckBox                  *cbApDoSessionSetup_;
  QCheckBox                  *cbApDoIonoCorrection4SBD_;
  QCheckBox                  *cbApDoAmbigResolution_;
  QCheckBox                  *cbApDoClockBreaksDetection_;
  QCheckBox                  *cbApDoIonoCorrection4All_;
  QCheckBox                  *cbApDoOutliers_;
  QCheckBox                  *cbApDoWeights_;
  QCheckBox                  *cbApDoReportNotUsedData_;
  QGroupBox                  *gbApFinalSolution_;
  QButtonGroup               *bgApFinalSolution_;
  //
  //
  SgGuiQTreeWidgetExt        *tweUserCorrections_;

};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgGuiTaskConfigDialog : public QDialog
{
  Q_OBJECT

public:
  SgGuiTaskConfigDialog(SgTaskConfig*, SgParametersDescriptor*, SgVlbiSession*,
    QWidget* =NULL, Qt::WindowFlags =0);
  virtual ~SgGuiTaskConfigDialog();
  inline QString className() const {return "SgGuiTaskConfigDialog";};

signals:
  void windowClosed();
  
private slots:
  void accept();

protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};

private:
  SgTaskConfig               *config_;
  SgParametersDescriptor     *parametersDescriptor_;
  SgVlbiSession              *session_;
  SgGuiTaskConfig            *configWidget_;
};
/*=====================================================================================================*/

/*=====================================================================================================*/
#endif // SG_GUI_TASK_CONFIG_H
