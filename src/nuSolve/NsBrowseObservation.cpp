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

#include "NsBrowseObservation.h"
#include "nuSolve.h"


#include <iostream>
#include <stdlib.h>


#include <QtCore/QDir>

#if QT_VERSION >= 0x050000
#    include <QtWidgets/QBoxLayout>
#    include <QtWidgets/QGroupBox>
#    include <QtWidgets/QHeaderView>
#    include <QtWidgets/QFrame>
#    include <QtWidgets/QLabel>
#    include <QtWidgets/QPlainTextEdit>
#    include <QtWidgets/QPushButton>
#    include <QtWidgets/QTabWidget>
#    include <QtWidgets/QTextEdit>
#    include <QtWidgets/QTreeWidget>
#else
#    include <QtGui/QBoxLayout>
#    include <QtGui/QGroupBox>
#    include <QtGui/QHeaderView>
#    include <QtGui/QFrame>
#    include <QtGui/QLabel>
#    include <QtGui/QPlainTextEdit>
#    include <QtGui/QPushButton>
#    include <QtGui/QTabWidget>
#    include <QtGui/QTextEdit>
#    include <QtGui/QTreeWidget>
#endif



#include <SgLogger.h>
#include <SgMJD.h>
#include <SgSolutionReporter.h>

#include <SgVlbiBaselineInfo.h>
#include <SgVlbiStationInfo.h>
#include <SgVlbiSourceInfo.h>
#include <SgConstants.h>

#include "NsBrowseNotUsedObsDialog.h"





/////*=======================================================================================================
////*
////*                   NsQTreeWidgetItem METHODS:
////* 
////*======================================================================================================*/
////bool NsQTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
////{
////  int                           column=treeWidget()->sortColumn();
////  double                        dL, dR;
////  bool                          isOk;
////  //
////  // if can be converted to double sort by value:
////  dL = text(column).toDouble(&isOk);
////  if (isOk)
////  {
////    dR = other.text(column).toDouble(&isOk);
////    if (isOk)
////      return dL < dR;
////  };
////  // otherwice, do as usual:
////  return QTreeWidgetItem::operator<(other);
////};
/////*=====================================================================================================*/






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsBrowseObservation::NsBrowseObservation(SgVlbiObservation *obs,
  QWidget *parent, Qt::WindowFlags flags)
  :
    QDialog(parent, flags)
{
  obs_ = obs;

  QBoxLayout                   *layout, *subLayout;
  QSize                         btnSize;
  QTabWidget                   *topTabs;
  
  layout  = new QVBoxLayout(this);
  topTabs = new QTabWidget (this);

  topTabs->addTab(tab4GeneralInfo(), "&General Info");
  topTabs->addTab(tab4Bands(),       "&Bands");
  //
  layout->addWidget(topTabs, 10);

  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  
// buttons  
  QPushButton                   *ok;
  ok = new QPushButton("Close", this);
    
  ok->setMinimumSize((btnSize=ok->sizeHint()));
  subLayout->addWidget(ok);

  subLayout->addStretch(1);
  subLayout->addWidget(ok);

  setSizeGripEnabled(true);

  //
  connect(ok,     SIGNAL(clicked()), SLOT(accept()));

  setWindowTitle("Observation: " + obs_->strId());
};



//
NsBrowseObservation::~NsBrowseObservation()
{
  obs_ = NULL;
};



//
void NsBrowseObservation::accept()
{
  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::accept(): accept selected");
  deleteLater();
};



//
void NsBrowseObservation::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::reject(): reject selected");
  deleteLater();
};



//
QWidget* NsBrowseObservation::tab4GeneralInfo()
{
  QString     									str("");
  QWidget     								 *w=new QWidget(this);
//  QBoxLayout  								 *layout=new QHBoxLayout(w);
  QLabel       								 *label;
  QGridLayout 								 *grid;
  QGridLayout 								 *mainGrid;
  QGroupBox   							   *gbox;
//  QBoxLayout  								 *subLayout;
	int														rowIdx;

  mainGrid = new QGridLayout(w);

//  subLayout = new QVBoxLayout;
//  layout->addLayout(subLayout);

  // attributes:
  gbox = new QGroupBox("Attributes of the observation", w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  //
  label = new QLabel("Epoch:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(obs_->toString(SgMJD::F_YYYYMMDDHHMMSSSS), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Observation #:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d", obs_->getMediaIdx() + 1), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Baseline:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(obs_->baseline()->getKey(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Source:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(obs_->src()->getKey(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Scan:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(obs_->getScanName() + "  " + obs_->getScanFullName(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Number of bands:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.setNum(obs_->observableByKey().size()), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  if (obs_->activeObs())
  {
    label = new QLabel(obs_->activeObs()->activeDelay()->getName() + " residuals [ps]:", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);

    label = new QLabel(str.sprintf("%.2f +/- %.2f", 
                obs_->activeObs()->activeDelay()->getResidual()*1.0e12,
                obs_->activeObs()->activeDelay()->sigma2Apply()*1.0e12), gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++, 1);

    label = new QLabel(obs_->activeObs()->phDRate().getName() + " residuals [fs]:", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);
    
    label = new QLabel(str.sprintf("%.2f +/- %.2f", 
                obs_->activeObs()->phDRate().getResidual()*1.0e15,
                obs_->activeObs()->phDRate().sigma2Apply()*1.0e15), gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++, 1);
  };

  //
  label = new QLabel("In solution:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    obs_->isAttr(SgObservation::Attr_PROCESSED)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  /*
  label = new QLabel("Turned off:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    obs_->isAttr(SgObservation::Attr_NOT_VALID)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
  */

  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);


  mainGrid->addWidget(gbox, 0, 0, 4, 1);
  //


  // new column:
  // source attributes:
  gbox = new QGroupBox("Attributes of the radio source " + obs_->src()->getKey(), w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  //
  label = new QLabel("T/U/P numbers:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d/%d/%d",
            obs_->src()->numTotal(DT_DELAY),
            obs_->src()->numUsable(DT_DELAY),
            obs_->src()->numProcessed(DT_DELAY)), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Delay WRMS [ps]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->src()->wrms(DT_DELAY)*1.0e12), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 0, 1);


  // baseline attributes:
  gbox = new QGroupBox("Attributes of the baseline " + obs_->baseline()->getKey(), w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  //
  label = new QLabel("T/U/P numbers:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d/%d/%d",
            obs_->baseline()->numTotal(DT_DELAY),
            obs_->baseline()->numUsable(DT_DELAY),
            obs_->baseline()->numProcessed(DT_DELAY)), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Delay WRMS [ps]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->baseline()->wrms(DT_DELAY)*1.0e12), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 1, 1);


  // station #1 attributes:
  gbox = new QGroupBox("Attributes of the reference station " + obs_->stn_1()->getKey(), w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  //
  label = new QLabel("T/U/P numbers:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d/%d/%d",
            obs_->stn_1()->numTotal(DT_DELAY),
            obs_->stn_1()->numUsable(DT_DELAY),
            obs_->stn_1()->numProcessed(DT_DELAY)), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Delay WRMS [ps]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->stn_1()->wrms(DT_DELAY)*1.0e12), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Azimuth [deg]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->auxObs_1()->getAzimuthAngle()*RAD2DEG), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
  
  //
  label = new QLabel("Elevation [deg]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->auxObs_1()->getElevationAngle()*RAD2DEG), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 2, 1);



  // station #2 attributes:
  gbox = new QGroupBox("Attributes of the remote station " + obs_->stn_2()->getKey(), w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

  //
  label = new QLabel("T/U/P numbers:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d/%d/%d",
            obs_->stn_2()->numTotal(DT_DELAY),
            obs_->stn_2()->numUsable(DT_DELAY),
            obs_->stn_2()->numProcessed(DT_DELAY)), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Delay WRMS [ps]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->stn_2()->wrms(DT_DELAY)*1.0e12), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Azimuth [deg]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->auxObs_2()->getAzimuthAngle()*RAD2DEG), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
  
  //
  label = new QLabel("Elevation [deg]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f",
            obs_->auxObs_2()->getElevationAngle()*RAD2DEG), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 3, 1);

  
  mainGrid->setHorizontalSpacing(mainGrid->horizontalSpacing() + 20);
  mainGrid->setVerticalSpacing  (mainGrid->verticalSpacing  () + 20);

  //
  return w;
};



//
QWidget* NsBrowseObservation::tab4Bands()
{
  QTabWidget                   *w=new QTabWidget(this);
  w->setTabPosition(QTabWidget::East);

/*  
  for (QMap<QString, SgVlbiObservable*>::iterator it=obs_->observableByKey().begin(); 
    it!=obs_->observableByKey().end(); ++it)
    w->addTab(tab4Band(it.value()), it.key() + "-Band");
*/
  // keep the same order as in the session edit window:
  
  QMap<QString, SgVlbiObservable*>::iterator 
                                it=obs_->observableByKey().end();
  do
  {
    --it;
    w->addTab(tab4Band(it.value()), it.key() + "-Band");
  }
  while (it != obs_->observableByKey().begin());
  //
  // set the tab to point on the active obs:
  int                           idx=0;
  it = obs_->observableByKey().end();
  do
  {
    --it;
    if (it.key() == obs_->activeObs()->getBandKey() )
      w->setCurrentIndex(idx);
    idx++;
  }
  while (it != obs_->observableByKey().begin());

  return w;
};



//
QWidget* NsBrowseObservation::tab4Band(SgVlbiObservable* o)
{
  QTabWidget                   *w=new QTabWidget(this);
  w->setTabPosition(QTabWidget::South);

  w->addTab(tab4EditingInfo (o), "Editing Info");
  w->addTab(tab4ChannelList (o), "Channel Info list");
  w->addTab(tab4ChannelPlots(o), "Channel Info plot");
  w->addTab(tab4CPcalPlots  (o), "Phase cal plot");

//  connect(this, SIGNAL(dataChanged()), stnInfoList, SLOT(updateContent()));
//  connect(this, SIGNAL(dataChanged()), srcInfoList, SLOT(updateContent()));
//  connect(this, SIGNAL(dataChanged()), baselineInfoList, SLOT(updateContent()));
  return w;
};



//
QWidget* NsBrowseObservation::tab4EditingInfo(SgVlbiObservable* o)
{
  QWidget     								 *w=new QWidget(this);
  QString     									str("");
//QBoxLayout  								 *layout=new QHBoxLayout(w);
//QBoxLayout  								 *subLayout;
  QLabel       								 *label;
  QGridLayout 								 *grid;
  QGridLayout 								 *mainGrid;
  QGroupBox   							   *gbox;
	int														rowIdx;

  mainGrid = new QGridLayout(w);


  QLabel       								 *emptyLabel;

//  subLayout = new QVBoxLayout;
//  layout->addLayout(subLayout);

  // attributes:
  gbox = new QGroupBox("Parameters of " + obs_->strId(), w);
  grid = new QGridLayout(gbox);
  rowIdx = 0;

/*
  //
  label = new QLabel("Obs ID:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(obs_->strId(), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
*/
  //
  label = new QLabel("Quality factor:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.setNum(o->getQualityFactor()), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Error code:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel("\"" + o->getErrorCode() + "\"", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Fourfit file name:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel("\"" + o->getFourfitOutputFName() + "\"", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("SNR:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2g", o->getSnr()), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Number of channels:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%d", o->getNumOfChannels()), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Sample rate [MHz]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.1f", o->getSampleRate()*1.0e-6), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Group delay ambiguity spacing [ns]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.1f", o->grDelay().getAmbiguitySpacing()*1.0e9), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  //
  label = new QLabel("Phase delay ambiguity spacing [ps]:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(str.sprintf("%.2f", o->phDelay().getAmbiguitySpacing()*1.0e12), gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);


  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 0, 0);


  // attributes of the observable:
  gbox = new QGroupBox("Attributes", w);
  grid = new QGridLayout(gbox);

  rowIdx = 0;

  //
  label = new QLabel(o->activeDelay()->getName() + " processed:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    o->sbDelay().isAttr(SgVlbiMeasurement::Attr_PROCESSED)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  // a "separator":
  emptyLabel = new QLabel(QString(""), gbox);
  emptyLabel->setMinimumSize(emptyLabel->sizeHint());
  grid->addWidget(emptyLabel, rowIdx++, 1);

      
  //
  label = new QLabel(o->sbDelay().getName() + " turned off:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    o->sbDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

/*
  //
  label = new QLabel(o->grDelay().getName() + " processed:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    o->grDelay().isAttr(SgVlbiMeasurement::Attr_PROCESSED)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);
*/
  //
  label = new QLabel(o->grDelay().getName() + " turned off:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(
    o->grDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);


  //
  label = new QLabel(o->phDelay().getName() + " turned off:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx, 0);

  label = new QLabel(o->phDelay().isAttr(SgVlbiMeasurement::Attr_NOT_VALID)?"<b>Yes</b>":"No", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, rowIdx++, 1);

  // a "separator":
  emptyLabel = new QLabel(QString(""), gbox);
  emptyLabel->setMinimumSize(emptyLabel->sizeHint());
  grid->addWidget(emptyLabel, rowIdx++, 1);

  if (o->getIsActive())
  {
    str = "None";
    if (!obs_->isAttr(SgObservation::Attr_PROCESSED))
//  if (!o->isUsable() )
    {
      str = "";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_LOW_QF))
        str += "Low quality factor. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_HAS_ERROR_CODE))
        str += "Error code is set. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_SOURCE))
        str += "Deselected source. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_STATION))
        str += "Deselected station. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_BASELINE))
        str += "Deselected baseline. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_UNMATED))
        str += "Not mated. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_ONE_CHANNEL))
        str += "Not enough used channels. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_LOW_QF))
        str += "Low quality factor on another band. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_HAS_ERROR_CODE))
        str += "Error code is set on another band. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_ONE_CHANNEL))
        str += "Not enough used channels on another band. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_TOO_EARLY))
        str += "The observation is before the specified time to start. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_TOO_LATE))
        str += "The observation is after the specified time to end. ";
      str.chop(1);
    };
    //
    label = new QLabel(o->activeDelay()->getName() + " reason(s) of non-usability: ", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);
    
    label = new QLabel(str, gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++, 1);
    
    str = "None";
    if (!o->isUsable())
      str = "Not usable";
    else if (!o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      str = "";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_DESELECTED_OBS))
        str += "Deselected observation. ";
      if (o->nonUsableReason().isAttr(SgVlbiObservable::NUR_MATE_DESELECTED_OBS))
        str += "Deselected observation on another band. ";
      str.chop(1);
    };
    //
    label = new QLabel(o->activeDelay()->getName() + " reason(s) for non-processing: ", gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx, 0);

    label = new QLabel(str, gbox);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, rowIdx++, 1);
  };


  grid->setRowStretch(rowIdx, 1);
  grid->setHorizontalSpacing(grid->horizontalSpacing() + 20);

  mainGrid->addWidget(gbox, 0, 1);

  
  mainGrid->setHorizontalSpacing(mainGrid->horizontalSpacing() + 20);
  mainGrid->setVerticalSpacing  (mainGrid->verticalSpacing  () + 20);

  //
  return w;
};



//
QWidget* NsBrowseObservation::tab4ChannelList(SgVlbiObservable* o)
{
  QString                       str("");
  QStringList                   headerLabels;
  QTreeWidget                  *twChannels;
  QWidget                      *w=new QWidget(this);
  QBoxLayout                   *layout=new QVBoxLayout(w);

  headerLabels 
    << "Idx" 
    << "Chan Id" 
    << "Freq, MHz  "
    << "BW, MHz"
    << "Plrz @1"
    << "Plrz @2"
    << "Num AP, USB"
    << "Num AP, LSB"
    << "Num MSmp, USB"
    << "Num MSmp, LSB"
    << "FrngAmpl"
    << "FrngPhase, degree"
    ;

  //
  twChannels = new QTreeWidget(w);
  twChannels->setColumnCount(headerLabels.count());
  twChannels->setHeaderLabels(headerLabels);


  for (int i=0; i<o->getNumOfChannels(); i++)
  {
    QTreeWidgetItem            *item=new QTreeWidgetItem(twChannels);

    item->setText( 0, str.sprintf("%2d", i));
    item->setData( 0, Qt::TextAlignmentRole, Qt::AlignRight);
    
    item->setText( 1, QString(o->chanIdByChan()->at(i)));
    item->setData( 1, Qt::TextAlignmentRole, Qt::AlignCenter);

    item->setText( 2, str.sprintf("%.2f", o->refFreqByChan()->getElement(i)));
    item->setData( 2, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText( 3, str.sprintf("%.2f", o->channelBandwidth()->getElement(i)));
    item->setData( 3, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText( 4, QString(o->polarization_1ByChan()->at(i)));
    item->setData( 4, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText( 5, QString(o->polarization_2ByChan()->at(i)));
    item->setData( 5, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText( 6, str.sprintf("%.2f", o->numOfAccPeriodsByChan_USB()->getElement(i)));
    item->setData( 6, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText( 7, str.sprintf("%.2f", o->numOfAccPeriodsByChan_LSB()->getElement(i)));
    item->setData( 7, Qt::TextAlignmentRole, Qt::AlignRight);


    item->setText( 8, str.sprintf("%.2f", o->numOfSamplesByChan_USB()->getElement(i)*1.0e-6));
    item->setData( 8, Qt::TextAlignmentRole, Qt::AlignRight);
    
    item->setText( 9, str.sprintf("%.2f", o->numOfSamplesByChan_LSB()->getElement(i)*1.0e-6));
    item->setData( 9, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(10, str.sprintf("%.2g", o->fringeAmplitudeByChan()->getElement(i)));
    item->setData(10, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(11, str.sprintf("%.1f", o->fringePhaseByChan()->getElement(i)*RAD2DEG));
    item->setData(11, Qt::TextAlignmentRole, Qt::AlignRight);
   
  };
    
  twChannels->header()->resizeSections(QHeaderView::ResizeToContents);
  twChannels->header()->setSectionResizeMode(QHeaderView::Interactive);
  twChannels->header()->setStretchLastSection(false);
  //twChannels->header()->setStretchLastSection(true);
  
  twChannels->setSortingEnabled(true);
  twChannels->setUniformRowHeights(true);
  twChannels->sortByColumn(2, Qt::AscendingOrder);
  twChannels->setFocus();
  twChannels->setItemsExpandable(false);
  twChannels->setAllColumnsShowFocus(true);

  layout->addWidget(twChannels);
  
/*
  connect(tweSources_,
    SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
    SLOT  (entryDoubleClicked(QTreeWidgetItem*, int))
    );
  connect(tweSources_,
    SIGNAL(itemClicked (QTreeWidgetItem*, int)),
    SLOT  (entryClicked(QTreeWidgetItem*, int))
    );
*/
  return w;
};



//
QWidget* NsBrowseObservation::tab4ChannelPlots(SgVlbiObservable* o)
{
  QString                       title("");
  QString                       str("");
  title = o->getBandKey() + "-band of the observation " + o->strId();

  SgPlotCarrier* carrier = new SgPlotCarrier( 8, 0, title);
  plotCarriers4ChannelData_.append(carrier);

  // names of columns:
  // values:
  carrier->setNameOfColumn( 0, "Ref.Freq, MHz");
  carrier->setNameOfColumn( 1, "Fringe Amplitude");
  carrier->setNameOfColumn( 2, "Fringe Phase");
  carrier->setNameOfColumn( 3, "Bandwidth");
  carrier->setNameOfColumn( 4, "Num Of Acc Periods, USB");
  carrier->setNameOfColumn( 5, "Num Of Acc Periods, LSB");
  carrier->setNameOfColumn( 6, "Num Of MSamples, USB");
  carrier->setNameOfColumn( 7, "Num Of MSamples, LSB");

  // create branches and fill data:
  carrier->listOfBranches()->clear();
  carrier->createBranch(o->getNumOfChannels(), "Per channel data", true);
  
  SgPlotBranch                 *branch=carrier->listOfBranches()->last();
  for (int i=0; i<o->getNumOfChannels(); i++)
  {
    branch->data()->setElement(i, 0,   o->refFreqByChan()->getElement(i));
    branch->data()->setElement(i, 1,   o->fringeAmplitudeByChan()->getElement(i));
    branch->data()->setElement(i, 2,   o->fringePhaseByChan()->getElement(i)*RAD2DEG);
    branch->data()->setElement(i, 3,   o->channelBandwidth()->getElement(i));
    branch->data()->setElement(i, 4,   o->numOfAccPeriodsByChan_USB()->getElement(i));
    branch->data()->setElement(i, 5,   o->numOfAccPeriodsByChan_LSB()->getElement(i));
    branch->data()->setElement(i, 6,   o->numOfSamplesByChan_USB()->getElement(i)*1.0e-6);
    branch->data()->setElement(i, 7,   o->numOfSamplesByChan_LSB()->getElement(i)*1.0e-6);
  };

  // widgets:
  QWidget                      *w=new QWidget(this);
  unsigned int                  plotterModes;
  plotterModes = SgPlot::PM_ERRBARS | SgPlot::PM_Q_PNT_EXT_PROC | SgPlot::PM_FILTERS_ENABLED;
  
  SgPlot                       *plot=new SgPlot(carrier, setup.path2(setup.getPath2PlotterOutput()), 
                                                  w, plotterModes);
  plot->setOutputFormat(setup.getPlotterOutputFormat());

  //
  plots4ChannelData_.append(plot);

  QBoxLayout *layout = new QVBoxLayout(w);
  layout->addWidget(plot);
  /*
  connect(plot, SIGNAL(userPressedAKey(SgPlot*, Qt::KeyboardModifiers, int)), 
                SLOT  (dispatchUserKey(SgPlot*, Qt::KeyboardModifiers, int)));
  connect(plot, SIGNAL(yAxisChanged(int)), SLOT(dispatchChangeOfYAxis(int)));
  connect(plot, SIGNAL(pointInfoRequested(SgPlot*, SgPlotBranch*, int, int, int)), 
                SLOT  (displayPointInfo(SgPlot*, SgPlotBranch*, int, int, int)));
  connect(plot, SIGNAL(pointInfoRequested_mode1(SgPlot*, SgPlotBranch*, int, int, int)), 
                SLOT  (displayObservableInfo(SgPlot*, SgPlotBranch*, int, int, int)));
  */
  return w;
};



//
QWidget* NsBrowseObservation::tab4CPcalPlots(SgVlbiObservable* o)
{
  QString                       title("");
  QString                       str("");
  SgPlotBranch                 *branch;
  title = o->getBandKey() + "-band of the observation " + o->strId();

  SgPlotCarrier* carrier = new SgPlotCarrier( 6, 0, title);
  plotCarriers4ChannelData_.append(carrier);

  // names of columns:
  // values:
  carrier->setNameOfColumn( 0, "Channel Freq, MHz");
  carrier->setNameOfColumn( 1, "Frequency, MHz");
  carrier->setNameOfColumn( 2, "Amplitude");
  carrier->setNameOfColumn( 3, "Phase");
  carrier->setNameOfColumn( 4, "Offset");
  carrier->setNameOfColumn( 5, "Rate");

  // create branches and fill data:
  carrier->listOfBranches()->clear();
  
  // station #0:
  carrier->createBranch(o->getNumOfChannels(), o->stn_1()->getKey(), true);
  branch = carrier->listOfBranches()->last();
  for (int i=0; i<o->getNumOfChannels(); i++)
  {
    branch->data()->setElement(i, 0, o->refFreqByChan()->getElement(i));
    branch->data()->setElement(i, 1, 
      o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_FREQUENCY, i));
    branch->data()->setElement(i, 2, 
      o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_AMPLITUDE, i));
    branch->data()->setElement(i, 3, 
      o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_PHASE,     i)*RAD2DEG);
    branch->data()->setElement(i, 4, 
      o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_OFFSET,    i));
    branch->data()->setElement(i, 5, 
      o->phaseCalData_1ByChan()->getElement(SgVlbiObservable::PCCI_RATE,      i));
  };

  // station #1:
  carrier->createBranch(o->getNumOfChannels(), o->stn_2()->getKey(), true);
  branch = carrier->listOfBranches()->last();
  for (int i=0; i<o->getNumOfChannels(); i++)
  {
    branch->data()->setElement(i, 0, o->refFreqByChan()->getElement(i));
    branch->data()->setElement(i, 1, 
      o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_FREQUENCY, i));
    branch->data()->setElement(i, 2, 
      o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_AMPLITUDE, i));
    branch->data()->setElement(i, 3, 
      o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_PHASE,     i)*RAD2DEG);
    branch->data()->setElement(i, 4, 
      o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_OFFSET,    i));
    branch->data()->setElement(i, 5, 
      o->phaseCalData_2ByChan()->getElement(SgVlbiObservable::PCCI_RATE,      i));
  };

  // widgets:
  QWidget                      *w=new QWidget(this);
  unsigned int                  plotterModes;
  plotterModes = SgPlot::PM_ERRBARS;
  
  SgPlot *plot = new SgPlot(carrier, setup.path2(setup.getPath2PlotterOutput()), w, plotterModes);
  plot->setOutputFormat(setup.getPlotterOutputFormat());

  //
  plots4ChannelData_.append(plot);

  QBoxLayout *layout = new QVBoxLayout(w);
  layout->addWidget(plot);
  /*
  connect(plot, SIGNAL(userPressedAKey(SgPlot*, Qt::KeyboardModifiers, int)), 
                SLOT  (dispatchUserKey(SgPlot*, Qt::KeyboardModifiers, int)));
  connect(plot, SIGNAL(yAxisChanged(int)), SLOT(dispatchChangeOfYAxis(int)));
  connect(plot, SIGNAL(pointInfoRequested(SgPlot*, SgPlotBranch*, int, int, int)), 
                SLOT  (displayPointInfo(SgPlot*, SgPlotBranch*, int, int, int)));
  connect(plot, SIGNAL(pointInfoRequested_mode1(SgPlot*, SgPlotBranch*, int, int, int)), 
                SLOT  (displayObservableInfo(SgPlot*, SgPlotBranch*, int, int, int)));
  */
  return w;
};

/*=====================================================================================================*/
