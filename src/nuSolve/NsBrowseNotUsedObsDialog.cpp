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

#include "NsBrowseNotUsedObsDialog.h"
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





/*=======================================================================================================
*
*                   NsQTreeWidgetItem METHODS:
* 
*======================================================================================================*/
bool NsQTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
{
  int                           column=treeWidget()->sortColumn();
  double                        dL, dR;
  bool                          isOk;
  //
  // if can be converted to double sort by value:
  dL = text(column).toDouble(&isOk);
  if (isOk)
  {
    dR = other.text(column).toDouble(&isOk);
    if (isOk)
      return dL < dR;
  };
  // otherwice, do as usual:
  return QTreeWidgetItem::operator<(other);
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsBrowseNotUsedObsDialog::NsBrowseNotUsedObsDialog(const QList<NsNotUsedObsInfo*>& nonUsableObservations,
  const QList<NsDeselectedObsInfo*>& deselectedObservations, SgSolutionReporter *reporter,
  QWidget *parent, Qt::WindowFlags flags)
  :
    QDialog(parent, flags),
    nonUsableObservations_(nonUsableObservations),
    deselectedObservations_(deselectedObservations)
{
  reporter_ = reporter;
  
  QBoxLayout                    *layout, *subLayout;
  QSize                          btnSize;
  QString                       str("");

  layout = new QVBoxLayout(this);
  
  QStringList                   headerLabels;
  headerLabels 
    << "Media index" 
    << "Station#1" 
    << "Station#2" 
    << "Source"
    << "Time"
    << "QF(s)"
    << "Reason(s)"
    ;

  twUnusable_ = new QTreeWidget(this);
  twUnusable_->setColumnCount(headerLabels.count());
  twUnusable_->setHeaderLabels(headerLabels);

  for (int i=0; i<nonUsableObservations_.size(); i++)
  {
    NsNotUsedObsInfo          *obsInfo=nonUsableObservations_.at(i);
    NsQTreeWidgetItem         *item = new NsQTreeWidgetItem(twUnusable_);

    item->setText(0, str.sprintf("%6d", obsInfo->getMediaIdx()));
    item->setData(0, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(1, str.sprintf("%s", qPrintable(obsInfo->getStn_1Name())));
    item->setData(1, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(2, str.sprintf("%s", qPrintable(obsInfo->getStn_2Name())));
    item->setData(2, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(3, str.sprintf("%s", qPrintable(obsInfo->getSrcName())));
    item->setData(3, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(4, str.sprintf("%s", qPrintable(obsInfo->getStrEpoch())));
    item->setData(4, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(5, str.sprintf("%s", qPrintable(obsInfo->getStrQFs())));
    item->setData(5, Qt::TextAlignmentRole, Qt::AlignCenter);
    item->setText(6, str.sprintf("%s", qPrintable(obsInfo->getReasons())));
    item->setData(6, Qt::TextAlignmentRole, Qt::AlignLeft);
  };
  
  twUnusable_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  twUnusable_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  twUnusable_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  twUnusable_->header()->setStretchLastSection(true);
  
  twUnusable_->setSortingEnabled(true);
  twUnusable_->setUniformRowHeights(true);
  twUnusable_->sortByColumn(0, Qt::AscendingOrder);
  twUnusable_->setFocus();
  twUnusable_->setItemsExpandable(false);
  twUnusable_->setAllColumnsShowFocus(true);
  twUnusable_->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  twUnusable_->setMinimumSize(twUnusable_->sizeHint());  
  
  //
  //
  //----------
  headerLabels.clear();
  headerLabels 
    << "Media index" 
    << "Station#1" 
    << "Station#2" 
    << "Source"
    << "Time"
    << "QF(s)"
    << "Residual(ps)"
    << "Std.Dev(ps)"
    << "NormResid"
    ;

  twDeselected_ = new QTreeWidget(this);
  twDeselected_->setColumnCount(headerLabels.count());
  twDeselected_->setHeaderLabels(headerLabels);

  for (int i=0; i<deselectedObservations_.size(); i++)
  {
    NsDeselectedObsInfo       *obsInfo=deselectedObservations_.at(i);
    NsQTreeWidgetItem          *item = new NsQTreeWidgetItem(twDeselected_);

    item->setText(0, str.sprintf("%6d", obsInfo->getMediaIdx()));
    item->setData(0, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(1, str.sprintf("%s", qPrintable(obsInfo->getStn_1Name())));
    item->setData(1, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(2, str.sprintf("%s", qPrintable(obsInfo->getStn_2Name())));
    item->setData(2, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(3, str.sprintf("%s", qPrintable(obsInfo->getSrcName())));
    item->setData(3, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(4, str.sprintf("%s", qPrintable(obsInfo->getStrEpoch())));
    item->setData(4, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(5, str.sprintf("%s", qPrintable(obsInfo->getStrQFs())));
    item->setData(5, Qt::TextAlignmentRole, Qt::AlignCenter);

    item->setText(6, str.sprintf("%.1f", obsInfo->getResidual()));
    item->setData(6, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(7, str.sprintf("%.1f", obsInfo->getStdDev()));
    item->setData(7, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(8, str.sprintf("%.1f", obsInfo->getNormResid()));
    item->setData(8, Qt::TextAlignmentRole, Qt::AlignRight);
  };
  
  twDeselected_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  twDeselected_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  twDeselected_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  twDeselected_->header()->setStretchLastSection(false);
  
  twDeselected_->setSortingEnabled(true);
  twDeselected_->setUniformRowHeights(true);
  twDeselected_->sortByColumn(0, Qt::AscendingOrder);
  twDeselected_->setFocus();
  twDeselected_->setItemsExpandable(false);
  twDeselected_->setAllColumnsShowFocus(true);
  twDeselected_->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  twDeselected_->setMinimumSize(twDeselected_->sizeHint());  
  //
  //
  //----------
  QTabWidget                   *aTabs=new QTabWidget(this);

  aTabs->addTab(twUnusable_,
    QString("").setNum(nonUsableObservations_.size())  + " &Unusable Observations");
  aTabs->addTab(twDeselected_,
    QString("").setNum(deselectedObservations_.size()) + " &Deselected Observations");
  layout->addWidget(aTabs);
  
  // buttons:
  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);
 
  QPushButton          *bClose=new QPushButton("Close", this);
  QPushButton          *bExport=new QPushButton("Export", this);
  bClose->setDefault(true);

  bExport->setMinimumSize((btnSize=bExport->sizeHint()));
  bClose ->setMinimumSize(btnSize);

  subLayout->addWidget(bExport);
  subLayout->addWidget(bClose);
  
  layout->activate();
  setMinimumSize(sizeHint());
  
  connect(bClose,  SIGNAL(clicked()), SLOT(accept()));
  connect(bExport, SIGNAL(clicked()), SLOT(export2file()));
  setWindowTitle("The lists of not used observations");
};



//
NsBrowseNotUsedObsDialog::~NsBrowseNotUsedObsDialog()
{
  for (int i=0; i<nonUsableObservations_.size(); i++)
    delete nonUsableObservations_.at(i);
  nonUsableObservations_.clear();
  
  deselectedObservations_.clear();
};



//
void NsBrowseNotUsedObsDialog::accept()
{
  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::accept(): accept selected");
  deleteLater();
};



//
void NsBrowseNotUsedObsDialog::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::reject(): reject selected");
  deleteLater();
};



//
void NsBrowseNotUsedObsDialog::export2file()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::export2file(): export observations status selected");
  if (reporter_)
  {
    bool                          isOk(true);
    QString                       nuobsDirName=setup.path2(setup.getPath2NotUsedObsFileOutput());
    QDir                          d(nuobsDirName);
    if (!d.exists())
//    isOk = d.mkpath("./"); // Qt, wtf?
      isOk = d.mkpath(d.absolutePath());
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::export2file(): cannot create directory " + nuobsDirName + "; report failed");
      return;
    };
    //
    // create the report:
    QString         fName("nuSolve_unused_observations_" + setup.identities().getUserDefaultInitials());
    
    if (reporter_->reportNotUsedObs(nuobsDirName, fName))
      logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::export2file(): observations status have been saved in the \"" + fName + "\" file of \"" +
        nuobsDirName + "\" directory");
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
        "::export2file(): writting observations status the " + fName + " file has been failed");
  }
  else
    logger->write(SgLogger::ERR, SgLogger::GUI, className() + 
      "::export2file(): export has been failed, the reporter is NULL");
};
/*=====================================================================================================*/
