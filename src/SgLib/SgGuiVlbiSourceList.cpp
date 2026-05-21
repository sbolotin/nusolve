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



#include <SgLogger.h>

#include <SgGuiVlbiSourceList.h>
#include <SgConstants.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#endif

#include <QtGui/QFontMetrics>


enum SourceColumnIndex
{
  SCI_NUMBER      =  0,
  SCI_NAME        =  1,
  SCI_TOT_SCANS   =  2,
  SCI_TOT_OBS     =  3,
  SCI_USB_OBS     =  4,
  SCI_PRC_OBS     =  5,
  //
  SCI_S_WRMS_DEL  =  6,
  SCI_S_IGNORE    =  7,
  SCI_S_COO_EST   =  8,
  SCI_S_COO_CON   =  9,
  SCI_S_AP_EXIST  = 10,
  SCI_S_AP_COMMENT= 11,
  SCI_S_AL_2EXT   = 12,
  SCI_S_AL_2EST   = 13,
  SCI_S_USE_SSM   = 14,
  SCI_S_SSM_PTS   = 15,
  //
  SCI_B_DISP_DEL  =  6,
  SCI_B_DISP_RAT  =  7,
  SCI_B_SIG0_DEL  =  8,
  SCI_B_SIG0_RAT  =  9,
  SCI_B_WRMS_DEL  = 10,
  SCI_B_WRMS_RAT  = 11,
};


enum SsmColumnIndex
{
  MCI_IDX         =  0,
  MCI_X           =  1,
  MCI_Y           =  2,
  MCI_ER          =  3,
  MCI_K           =  4,
  MCI_EK          =  5,
  MCI_B           =  6,
  MCI_EB          =  7,
};




/*=======================================================================================================
*
*                   SgGuiVlbiSourceItem METHODS:
* 
*======================================================================================================*/
bool SgGuiVlbiSourceItem::operator<(const QTreeWidgetItem &other) const
{
  int                           column=treeWidget()->sortColumn();
  double                        dL, dR;
  bool                          isOk;
  dL = text(column).toDouble(&isOk);
  if (isOk)
  {
    dR = other.text(column).toDouble(&isOk);
    if (isOk)
      return dL < dR;
  };
  return QTreeWidgetItem::operator<(other);
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*                   SgGuiVlbiSourceList METHODS:
* 
*======================================================================================================*/
SgGuiVlbiSourceList::SgGuiVlbiSourceList(SgObjectBrowseMode mode, const QString& ownerName,
  QMap<QString, SgVlbiSourceInfo*>* map, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f),
    ownerName_(ownerName)
{
  browseMode_ = mode;
  constColumns_ = -1;
  sourcesByName_ = map;
  scl4delay_ = 1.0e12;
  scl4rate_  = 1.0e15;
  numbersBrowseMode_ = 0;
  QString                       str;
  QBoxLayout                   *layout=new QVBoxLayout(this);
  QStringList                   headerLabels;
  headerLabels 
    << "Idx" 
    << "Name" 
    << "Scans"
    << "TotObs"
    << "GoodObs"
    << "PrcdObs"
    ;
  switch (browseMode_)
  {
  case OBM_BAND: 
    constColumns_ = 12;
    headerLabels 
      << "Disp(ps)"
      << "Disp(fs/s)"
      << "Sig0(ps)"
      << "Sig0(fs/s)"
      << "WRMS(ps)"
      << "WRMS(fs/s)"
      ;
    break;
  case OBM_SESSION: 
    constColumns_ = 7;
    headerLabels 
      << "WRMS(ps)"
      << " Omit "
      << "k:Est"
      << "k:C"
      << "E.A."
      << "E.A.Status"
      << "2E.A."
      << "2Estd"
      << "UseSSM"
      << "#SSMpts"
    ;
    break;
  case OBM_PROJECT: 
    break;
  };
  //
  tweSources_ = new SgGuiQTreeWidgetExt(this);
  tweSources_->setColumnCount(headerLabels.count());
  tweSources_->setHeaderLabels(headerLabels);
  
  QMap<QString, SgVlbiSourceInfo*>::const_iterator i = sourcesByName_->constBegin();
  for (; i!=sourcesByName_->constEnd(); ++i)
  {
    SgVlbiSourceInfo* srInfo = i.value();

    SgGuiVlbiSourceItem *item = new SgGuiVlbiSourceItem(tweSources_);
    item->setSrInfo(srInfo);
    item->setText(SCI_NUMBER, str.sprintf("%4d", srInfo->getIdx()));
    item->setData(SCI_NUMBER, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(SCI_NAME, srInfo->getKey());
    item->setData(SCI_NAME, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setText(SCI_TOT_SCANS, str.sprintf("%7d", srInfo->getTotalScanNum()));
    item->setData(SCI_TOT_SCANS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(SCI_TOT_OBS, str.sprintf("%7d", srInfo->numTotal(DT_DELAY)));
    item->setData(SCI_TOT_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(SCI_USB_OBS, str.sprintf("%7d", srInfo->numUsable(DT_DELAY)));
    item->setData(SCI_USB_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(SCI_PRC_OBS, str.sprintf("%7d", srInfo->numProcessed(DT_DELAY)));
    item->setData(SCI_PRC_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    
    if (browseMode_==OBM_BAND)
    {
      item->setText(SCI_B_DISP_DEL, str.sprintf("%.1f", srInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_DISP_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_B_DISP_RAT, str.sprintf("%.1f", srInfo->dispersion(DT_RATE )*scl4rate_));
      item->setData(SCI_B_DISP_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_B_SIG0_DEL, str.sprintf("%.1f", srInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_SIG0_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_B_SIG0_RAT, str.sprintf("%.1f", srInfo->getSigma2add(DT_RATE )*scl4rate_));
      item->setData(SCI_B_SIG0_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_B_WRMS_DEL, str.sprintf("%.1f", srInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_B_WRMS_RAT, str.sprintf("%.1f", srInfo->wrms(DT_RATE )*scl4rate_));
      item->setData(SCI_B_WRMS_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
    }
    else if (browseMode_==OBM_SESSION)
    {
      item->setText(SCI_S_WRMS_DEL, str.sprintf("%.1f", srInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(SCI_S_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_S_IGNORE, srInfo->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID)? "X" : "");
      item->setData(SCI_S_IGNORE, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setText(SCI_S_COO_EST, srInfo->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO)? "Y" : "");
      item->setData(SCI_S_COO_EST, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setText(SCI_S_COO_CON, srInfo->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO)? "*" : "");
      item->setData(SCI_S_COO_CON, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setText(SCI_S_AP_EXIST, srInfo->isAttr(SgVlbiSourceInfo::Attr_HAS_A_PRIORI_POS)?"Y":"-");
      item->setData(SCI_S_AP_EXIST, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setText(SCI_S_AP_COMMENT, srInfo->getAprioriComments());
      item->setData(SCI_S_AP_COMMENT, Qt::TextAlignmentRole, Qt::AlignLeft);
      item->setText(SCI_S_AL_2EXT, str.sprintf("%.3f", srInfo->getAl2ExtA()*RAD2MAS));
      item->setData(SCI_S_AL_2EXT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setText(SCI_S_AL_2EST, str.sprintf("%.3f", srInfo->getAl2Estd()*RAD2MAS));
      item->setData(SCI_S_AL_2EST, Qt::TextAlignmentRole, Qt::AlignRight);
      str = "";
      if (srInfo->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM))
      {
        str = "Y";
        if (srInfo->calcNumOfEstimatedSsmParameters())
          str += QString("").sprintf("(%d)", srInfo->calcNumOfEstimatedSsmParameters());
      };
      item->setText(SCI_S_USE_SSM, str);
      item->setData(SCI_S_USE_SSM, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setText(SCI_S_SSM_PTS, srInfo->sModel().size()?str.sprintf("%d", srInfo->sModel().size()):"");
      item->setData(SCI_S_SSM_PTS, Qt::TextAlignmentRole, Qt::AlignRight);
    };
  };
  
  tweSources_->header()->resizeSections(QHeaderView::ResizeToContents);
 #if QT_VERSION >= 0x050000
  tweSources_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  tweSources_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  tweSources_->header()->setStretchLastSection(false);
  
  tweSources_->setSortingEnabled(true);
  tweSources_->setUniformRowHeights(true);
  tweSources_->sortByColumn(1, Qt::AscendingOrder);
  tweSources_->setFocus();
  tweSources_->setItemsExpandable(false);
  tweSources_->setAllColumnsShowFocus(true);

  layout->addWidget(tweSources_);
  
  if (browseMode_!=OBM_BAND)
    connect(tweSources_,
      SIGNAL(moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)), 
      SLOT  (toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)));

  connect(tweSources_,
    SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
    SLOT  (entryDoubleClicked(QTreeWidgetItem*, int))
    );
  connect(tweSources_,
    SIGNAL(itemClicked (QTreeWidgetItem*, int)),
    SLOT  (entryClicked(QTreeWidgetItem*, int))
    );
};



//
void SgGuiVlbiSourceList::toggleEntryMoveEnable(QTreeWidgetItem* item, int n, 
  Qt::MouseButton /* mouseButtonState */, Qt::KeyboardModifiers /* modifiers */)
{
  if (!item) // may be NULL
    return;

  SgVlbiSourceInfo*             srInfo=((SgGuiVlbiSourceItem*)item)->getSrInfo();
  QString                       str("");
  switch (n)
  {
  case SCI_S_IGNORE:
    srInfo->xorAttr(SgVlbiSourceInfo::Attr_NOT_VALID);
    item->setText(SCI_S_IGNORE, srInfo->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID)? "X" : "");
    break;
  case SCI_S_COO_EST:
    srInfo->xorAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO);
    item->setText(SCI_S_COO_EST, srInfo->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO)? "Y" : "");
    break;
  case SCI_S_COO_CON:
    srInfo->xorAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO);
    item->setText(SCI_S_COO_CON, srInfo->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO)? "*" : "");
    break;
  case SCI_S_USE_SSM:
    srInfo->xorAttr(SgVlbiSourceInfo::Attr_APPLY_SSM);
    str = "";
    if (srInfo->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM))
    {
      str = "Y";
      if (srInfo->calcNumOfEstimatedSsmParameters())
        str += QString("").sprintf("(%d)", srInfo->calcNumOfEstimatedSsmParameters());
    };
    item->setText(SCI_S_USE_SSM, str);
    break;
  default:
    //std::cout << "default, n=" << n << "\n";
    break;
  };
};



//
void SgGuiVlbiSourceList::entryClicked(QTreeWidgetItem* item, int column)
{
  if (!item)
    return;
  if (column != SCI_USB_OBS && column != SCI_PRC_OBS)
    return;
  if (numbersBrowseMode_ == 0)
    numbersBrowseMode_ = 1;
  else 
    numbersBrowseMode_ = 0;
  
  QTreeWidgetItem              *headerItem = tweSources_->headerItem();
  headerItem->setText(SCI_USB_OBS, numbersBrowseMode_==0?"GoodObs":"G/T, %");
  headerItem->setText(SCI_PRC_OBS, numbersBrowseMode_==0?"PrcdObs":"P/G, %");
  
  updateContent4Nums();
};



//
void SgGuiVlbiSourceList::entryDoubleClicked(QTreeWidgetItem* item, int column)
{
  if (!item || column>=constColumns_)
    return;
  SgVlbiSourceInfo *src = ((SgGuiVlbiSourceItem*)item)->getSrInfo();
  if (src)
  {
    if (browseMode_ == OBM_SESSION)
    {
      SgGuiVlbiSrcInfoEditor   *siEditor=new SgGuiVlbiSrcInfoEditor(src, ownerName_, this);
      connect(siEditor, SIGNAL(contentModified(bool)), SLOT(modifySourceInfo(bool)));
      siEditor->show();
    };
  };
};



//
void SgGuiVlbiSourceList::modifySourceInfo(bool isModified)
{
  if (isModified)
    updateContent();
};



//
void SgGuiVlbiSourceList::updateContent()
{
  QString                       str("");
  SgVlbiSourceInfo             *srInfo=NULL;
  SgGuiVlbiSourceItem          *item=NULL;
  QTreeWidgetItemIterator       it(tweSources_);
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiSourceItem*)(*it);
    srInfo = item->getSrInfo();
    numTotal = srInfo->numTotal(DT_DELAY);
    numUsable = srInfo->numUsable(DT_DELAY);
    item->setText(SCI_NUMBER, str.sprintf("%4d", srInfo->getIdx()));
    item->setText(SCI_NAME, srInfo->getKey());
    item->setText(SCI_TOT_SCANS, str.sprintf("%7d", srInfo->getTotalScanNum()));
    item->setText(SCI_TOT_OBS, str.sprintf("%7d", srInfo->numTotal(DT_DELAY)));
    /*
    item->setText(SCI_USB_OBS, str.sprintf("%7d", srInfo->numUsable(DT_DELAY)));
    item->setText(SCI_PRC_OBS, str.sprintf("%7d", srInfo->numProcessed(DT_DELAY)));
    */
    //
    if (numbersBrowseMode_ == 0)
    {
      item->setText(SCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(SCI_PRC_OBS,  str.sprintf("%7d", srInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(SCI_USB_OBS, "N/A");
      else
        item->setText(SCI_USB_OBS, str.sprintf("%.2f", 100.0*srInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(SCI_PRC_OBS, "N/A");
      else
        item->setText(SCI_PRC_OBS, str.sprintf("%.2f", 100.0*srInfo->numProcessed(DT_DELAY)/numUsable));
    };
    //
    if (browseMode_==OBM_BAND)
    {
      item->setText(SCI_B_DISP_DEL, str.sprintf("%.1f", srInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_DISP_RAT, str.sprintf("%.1f", srInfo->dispersion(DT_RATE )*scl4rate_));
      item->setText(SCI_B_SIG0_DEL, str.sprintf("%.1f", srInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_SIG0_RAT, str.sprintf("%.1f", srInfo->getSigma2add(DT_RATE )*scl4rate_));
      item->setText(SCI_B_WRMS_DEL, str.sprintf("%.1f", srInfo->wrms(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_WRMS_RAT, str.sprintf("%.1f", srInfo->wrms(DT_RATE )*scl4rate_));
    }
    else if (browseMode_==OBM_SESSION)
    {
      item->setText(SCI_S_WRMS_DEL, str.sprintf("%.1f", srInfo->wrms(DT_DELAY)*scl4delay_));
//    item->setText(SCI_S_WRMS_RAT, str.sprintf("%.1f", srInfo->wrms(DT_RATE )*scl4rate_));
      item->setText(SCI_S_IGNORE, srInfo->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID)? "X" : "");
      item->setText(SCI_S_COO_EST, srInfo->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO)? "Y" : "");
      item->setText(SCI_S_COO_CON, srInfo->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO)? "*" : "");
      item->setText(SCI_S_AP_EXIST, srInfo->isAttr(SgVlbiSourceInfo::Attr_HAS_A_PRIORI_POS)?"Y":"-");
      item->setText(SCI_S_AP_COMMENT, srInfo->getAprioriComments());
      item->setText(SCI_S_AL_2EXT, str.sprintf("%.3f", srInfo->getAl2ExtA()*RAD2MAS));
      item->setText(SCI_S_AL_2EST, str.sprintf("%.3f", srInfo->getAl2Estd()*RAD2MAS));
      str = "";
      if (srInfo->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM))
      {
        str = "Y";
        if (srInfo->calcNumOfEstimatedSsmParameters())
          str += QString("").sprintf("(%d)", srInfo->calcNumOfEstimatedSsmParameters());
      };
      item->setText(SCI_S_USE_SSM, str);
      item->setText(SCI_S_SSM_PTS, srInfo->sModel().size()?str.sprintf("%d", srInfo->sModel().size()):"");
    };
    ++it;
  };
};



//
void SgGuiVlbiSourceList::updateContent4Nums()
{
  QString                       str;
  SgVlbiSourceInfo             *srInfo=NULL;
  SgGuiVlbiSourceItem          *item=NULL;
  QTreeWidgetItemIterator       it(tweSources_);
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiSourceItem*)(*it);
    srInfo = item->getSrInfo();
    numTotal = srInfo->numTotal(DT_DELAY);
    numUsable = srInfo->numUsable(DT_DELAY);
    if (numbersBrowseMode_ == 0)
    {
      item->setText(SCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(SCI_PRC_OBS,  str.sprintf("%7d", srInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(SCI_USB_OBS, "N/A");
      else
        item->setText(SCI_USB_OBS, str.sprintf("%.2f", 100.0*srInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(SCI_PRC_OBS, "N/A");
      else
        item->setText(SCI_PRC_OBS, str.sprintf("%.2f", 100.0*srInfo->numProcessed(DT_DELAY)/numUsable));
    };
    ++it;
  };
};
/*=====================================================================================================*/







/*=======================================================================================================
*
*                   SgGuiVlbiSrcInfoEditor METHODS:
* 
*======================================================================================================*/
SgGuiVlbiSrcInfoEditor::SgGuiVlbiSrcInfoEditor(SgVlbiSourceInfo* srcInfo, const QString& ownerName, 
  QWidget* parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  static const QString          attrNames[] = 
  {
    QString("Ignore the source"),
    QString("Estimate source positions"),
    QString("Constrain source position in estimation"),
    QString("Apply the multi point source structure model"),
    QString("Test"),
  };
  const int                     numOfAttr=5;
  QString                       str;
  QSize                         btnSize;
  sourceInfo_ = srcInfo;
  isModified_ = false;
  
  setWindowTitle("The source " + sourceInfo_->getKey() + " at " + ownerName + " session");

  //
  QVBoxLayout                  *mainLayout=new QVBoxLayout(this);

  // clock breaks:
  QGroupBox                    *gBox=new QGroupBox("Components of the multi point source structure model",
                                                    this);
  QBoxLayout                   *layout=new QVBoxLayout(gBox);
  QStringList                   headerLabels;
  headerLabels 
    << "idx" 
    << "x (mas)" 
    << "y (mas)"
    << "Est P?"
    << "k"
    << "Est K?"
    << "B"
    << "Est B?"
    ;

  twSrcStModels_ = new SgGuiQTreeWidgetExt(gBox);
  twSrcStModels_->setColumnCount(8);
  twSrcStModels_->setHeaderLabels(headerLabels);
  //
  //
  for (int i=0; i<sourceInfo_->sModel().size(); i++)
  {
    SgVlbiSourceInfo::StructModelMp
                               *ss=&sourceInfo_->sModel()[i];
    SgGuiVlbiSrcStrModelItem    *item=new SgGuiVlbiSrcStrModelItem(twSrcStModels_);
    item->setPoint(ss);
    item->setModelIdx(i);

    item->setText(MCI_IDX, QString("").sprintf("%d", i));
    item->setData(MCI_IDX, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(MCI_X, QString("").sprintf("%.4f", ss->getX()*RAD2MAS));
    item->setData(MCI_X, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(MCI_Y, QString("").sprintf("%.4f", ss->getY()*RAD2MAS));
    item->setData(MCI_Y, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(MCI_ER, ss->getEstimatePosition()?"Y":"N");
    item->setData(MCI_ER, Qt::TextAlignmentRole, Qt::AlignCenter);

    item->setText(MCI_K, QString("").sprintf("%.4f", ss->getK()));
    item->setData(MCI_K, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(MCI_EK, ss->getEstimateRatio()?"Y":"N");
    item->setData(MCI_EK, Qt::TextAlignmentRole, Qt::AlignCenter);

    item->setText(MCI_B, QString("").sprintf("%.4f", ss->getB()));
    item->setData(MCI_B, Qt::TextAlignmentRole, Qt::AlignRight);

    item->setText(MCI_EB, ss->getEstimateSpIdx()?"Y":"N");
    item->setData(MCI_EB, Qt::TextAlignmentRole, Qt::AlignCenter);
  };
  twSrcStModels_->resizeColumnToContents(0);
  twSrcStModels_->resizeColumnToContents(1);
  twSrcStModels_->resizeColumnToContents(2);
  twSrcStModels_->resizeColumnToContents(3);
  twSrcStModels_->resizeColumnToContents(4);
  twSrcStModels_->resizeColumnToContents(5);
  twSrcStModels_->resizeColumnToContents(6);
  twSrcStModels_->resizeColumnToContents(7);

  twSrcStModels_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  twSrcStModels_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  twSrcStModels_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  twSrcStModels_->header()->setStretchLastSection(false);
  twSrcStModels_->setSortingEnabled(true);
  twSrcStModels_->setUniformRowHeights(true);
  twSrcStModels_->sortByColumn(0, Qt::AscendingOrder);
  twSrcStModels_->setFocus();
  twSrcStModels_->setItemsExpandable(false);
  twSrcStModels_->setAllColumnsShowFocus(true);
  twSrcStModels_->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  twSrcStModels_->setMinimumSize(twSrcStModels_->sizeHint());
  layout->addWidget(twSrcStModels_);
  
  QPushButton                  *bAdd   =new QPushButton("Add", this);
  QPushButton                  *bEdit  =new QPushButton("Edit", this);
  QPushButton                  *bDelete=new QPushButton("Delete", this);

  bDelete->setMinimumSize((btnSize=bDelete->sizeHint()));
  bAdd->setMinimumSize(btnSize);
  bEdit->setMinimumSize(btnSize);

  QBoxLayout                   *subLayout=new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);
  subLayout->addSpacing(3*btnSize.width());
  subLayout->addWidget(bAdd);
  subLayout->addWidget(bEdit);
  subLayout->addWidget(bDelete);

  mainLayout->addWidget(gBox);

  // attributes:
  QGroupBox                  *gboxAttr=new QGroupBox("Attributes of Source", this);
  layout = new QVBoxLayout(gboxAttr);
  for (int i=0; i<numOfAttr; i++)
  {
    cbAttributes_[i] = new QCheckBox(attrNames[i], gboxAttr);
    cbAttributes_[i]->setMinimumSize(cbAttributes_[i]->sizeHint());
    layout->addWidget(cbAttributes_[i]);
  };
  cbAttributes_[0]->setChecked(sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID));
  cbAttributes_[1]->setChecked(sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO));
  cbAttributes_[2]->setChecked(sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO));
  cbAttributes_[3]->setChecked(sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM));
  cbAttributes_[4]->setChecked(sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_TEST));
  //
  // options:
  mainLayout->addSpacing(15);
  mainLayout->addWidget(gboxAttr, 10);
  //
  //
  //---
  subLayout = new QHBoxLayout();
  mainLayout->addSpacing(15);
  mainLayout->addStretch(1);
  mainLayout->addLayout(subLayout);

  QPushButton                  *bOk=new QPushButton("OK", this);
  QPushButton                  *bCancel=new QPushButton("Cancel", this);
  bOk->setDefault(true);

  bCancel->setMinimumSize((btnSize=bCancel->sizeHint()));
  bOk->setMinimumSize(btnSize);

  subLayout->addStretch(1);
  subLayout->addWidget(bOk);
  subLayout->addWidget(bCancel);


  connect(twSrcStModels_,
    SIGNAL(moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)), 
    SLOT  (toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)));

  connect(twSrcStModels_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                          SLOT(editSrcStModel(QTreeWidgetItem*, int)));

  connect(bOk,     SIGNAL(clicked()), SLOT(accept()));
  connect(bAdd,    SIGNAL(clicked()), SLOT(insertSsmPoint()));
  connect(bEdit,   SIGNAL(clicked()), SLOT(editSsmPoint()));
  connect(bDelete, SIGNAL(clicked()), SLOT(deleteSsmPoint()));
  connect(bCancel, SIGNAL(clicked()), SLOT(reject()));

  mainLayout->activate();
};



//
SgGuiVlbiSrcInfoEditor::~SgGuiVlbiSrcInfoEditor()
{
  sourceInfo_ = NULL;
  emit contentModified(isModified_);
};



//
void SgGuiVlbiSrcInfoEditor::toggleEntryMoveEnable(QTreeWidgetItem* item, int n, 
  Qt::MouseButton /* mouseButtonState */, Qt::KeyboardModifiers /* modifiers */)
{
  if (!item) // may be NULL
    return;

  SgVlbiSourceInfo::StructModelMp*
                                ss=((SgGuiVlbiSrcStrModelItem*)item)->getPoint();
  QString                       str("");
  switch (n)
  {
  case MCI_ER:
    ss->setEstimatePosition(!ss->getEstimatePosition());
    item->setText(MCI_ER, ss->getEstimatePosition()?"Y":"N");
    isModified_ = true;
    break;
  case MCI_EK:
    ss->setEstimateRatio(!ss->getEstimateRatio());
    item->setText(MCI_EK, ss->getEstimateRatio()?"Y":"N");
    isModified_ = true;
    break;
  case MCI_EB:
    ss->setEstimateSpIdx(!ss->getEstimateSpIdx());
    item->setText(MCI_EB, ss->getEstimateSpIdx()?"Y":"N");
    isModified_ = true;
    break;
  default:
    //std::cout << "default, n=" << n << "\n";
    break;
  };
};



//
void SgGuiVlbiSrcInfoEditor::accept()
{
  acquireData();
  emit contentModified(isModified_);
  QDialog::accept();
  deleteLater();
};



//
void SgGuiVlbiSrcInfoEditor::acquireData()
{
  // Attributes:
  if (cbAttributes_[0]->isChecked() != sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
  {
    sourceInfo_->xorAttr(SgVlbiSourceInfo::Attr_NOT_VALID);
    isModified_ = true;
  };
  if (cbAttributes_[1]->isChecked() != sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO))
  {
    sourceInfo_->xorAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO);
    isModified_ = true;
  };
  if (cbAttributes_[2]->isChecked() != sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO))
  {
    sourceInfo_->xorAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO);
    isModified_ = true;
  };
  if (cbAttributes_[3]->isChecked() != sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM))
  {
    sourceInfo_->xorAttr(SgVlbiSourceInfo::Attr_APPLY_SSM);
    isModified_ = true;
  };
  if (cbAttributes_[4]->isChecked() != sourceInfo_->isAttr(SgVlbiSourceInfo::Attr_TEST))
  {
    sourceInfo_->xorAttr(SgVlbiSourceInfo::Attr_TEST);
    isModified_ = true;
  };
};



//
void SgGuiVlbiSrcInfoEditor::editSrcStModel(QTreeWidgetItem* item, int column)
{
  if (item && column>-1)
  {
    SgGuiVlbiSrcStrModelEditor *ssmEditor=
      new SgGuiVlbiSrcStrModelEditor(sourceInfo_, (SgGuiVlbiSrcStrModelItem*)item, this);
    connect(ssmEditor, SIGNAL(ssmPointModified(bool)), SLOT(updateModifyStatus(bool)));
    ssmEditor->show();
  };
};



//
void SgGuiVlbiSrcInfoEditor::updateModifyStatus(bool isModified)
{
  isModified_ = isModified_ || isModified;
};



//
void SgGuiVlbiSrcInfoEditor::editSsmPoint()
{
  if (twSrcStModels_->selectedItems().size())
  {
    SgGuiVlbiSrcStrModelEditor *ssmEditor=
      new SgGuiVlbiSrcStrModelEditor(sourceInfo_,
      (SgGuiVlbiSrcStrModelItem*)(twSrcStModels_->selectedItems().at(0)), this);
    connect (ssmEditor, SIGNAL(ssmPointModified(bool)), SLOT(updateModifyStatus(bool)));
    ssmEditor->show();
  };
};



//
void SgGuiVlbiSrcInfoEditor::deleteSsmPoint()
{
  if (twSrcStModels_->selectedItems().size())
  {
    SgGuiVlbiSrcStrModelItem   *item=(SgGuiVlbiSrcStrModelItem*)(twSrcStModels_->selectedItems().at(0));
    SgVlbiSourceInfo::StructModelMp
                               *p=item->getPoint();
    int                         idx=item->getModelIdx();
    if (QMessageBox::warning(this, "Delete?",
        "Are you sure to delete a component #" + QString("").setNum(idx) +
        " of a source structure model at " +
        QString("").sprintf("(x=%.4f, y=%.4f)?\n", p->getX()*RAD2MAS, p->getY()*RAD2MAS),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
    {
      delete item;
      sourceInfo_->sModel().removeAt(idx);
      isModified_ = true;
    };
  };
};



//
void SgGuiVlbiSrcInfoEditor::insertSsmPoint()
{
  SgGuiVlbiSrcStrModelEditor *ssmEditor=new SgGuiVlbiSrcStrModelEditor(sourceInfo_, NULL, this);
  connect (ssmEditor, SIGNAL(ssmPointModified(bool)), SLOT(updateModifyStatus(bool)));
  connect (ssmEditor, SIGNAL(ssmPointCreated(SgVlbiSourceInfo::StructModelMp*)),
                        SLOT(addNewSsmPoint(SgVlbiSourceInfo::StructModelMp*)));
  ssmEditor->show();
};



//
void SgGuiVlbiSrcInfoEditor::addNewSsmPoint(SgVlbiSourceInfo::StructModelMp* ss)
{
  if (ss)
  {
    int                         idx=sourceInfo_->sModel().size();
    sourceInfo_->sModel().append(*ss);
    SgGuiVlbiSrcStrModelItem   *item=new SgGuiVlbiSrcStrModelItem(twSrcStModels_);
    item->setPoint(ss);
    item->setModelIdx(idx);
/*
    item->setText( 0, QString("").sprintf("%d", idx));
    item->setData( 0, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 1, QString("").sprintf("%.4f", ss->getX()*RAD2MAS));
    item->setData( 1, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 2, QString("").sprintf("%.4f", ss->getY()*RAD2MAS));
    item->setData( 2, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 3, ss->getEstimatePosition()?"Y":"N");
    item->setData( 3, Qt::TextAlignmentRole, Qt::AlignCenter);
    item->setText( 4, QString("").sprintf("%.4f", ss->getK()));
    item->setData( 4, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 5, ss->getEstimateRatio()?"Y":"N");
    item->setData( 5, Qt::TextAlignmentRole, Qt::AlignCenter);
    item->setText( 6, QString("").sprintf("%.4f", ss->getB()));
    item->setData( 6, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 7, ss->getEstimateSpIdx()?"Y":"N");
    item->setData( 7, Qt::TextAlignmentRole, Qt::AlignCenter);
*/
    item->setText(MCI_IDX, QString("").sprintf("%d", idx));
    item->setData(MCI_IDX, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(MCI_X, QString("").sprintf("%.4f", ss->getX()*RAD2MAS));
    item->setData(MCI_X, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(MCI_Y, QString("").sprintf("%.4f", ss->getY()*RAD2MAS));
    item->setData(MCI_Y, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(MCI_ER, ss->getEstimatePosition()?"Y":"N");
    item->setData(MCI_ER, Qt::TextAlignmentRole, Qt::AlignCenter);
    item->setText(MCI_K, QString("").sprintf("%.4f", ss->getK()));
    item->setData(MCI_K, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(MCI_EK, ss->getEstimateRatio()?"Y":"N");
    item->setData(MCI_EK, Qt::TextAlignmentRole, Qt::AlignCenter);
    item->setText(MCI_B, QString("").sprintf("%.4f", ss->getB()));
    item->setData(MCI_B, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(MCI_EB, ss->getEstimateSpIdx()?"Y":"N");
    item->setData(MCI_EB, Qt::TextAlignmentRole, Qt::AlignCenter);
//    twSrcStModels_->resizeColumnToContents(0);
    twSrcStModels_->resizeColumnToContents(1);
    twSrcStModels_->resizeColumnToContents(2);
//    twSrcStModels_->resizeColumnToContents(3);
    twSrcStModels_->resizeColumnToContents(4);
//    twSrcStModels_->resizeColumnToContents(5);
    twSrcStModels_->resizeColumnToContents(6);
//    twSrcStModels_->resizeColumnToContents(7);
  };
};
/*=====================================================================================================*/









/*=======================================================================================================
*
*                  SgGuiVlbiSrcStrModelEditor  METHODS:
* 
*======================================================================================================*/
SgGuiVlbiSrcStrModelEditor::SgGuiVlbiSrcStrModelEditor(SgVlbiSourceInfo* src, 
    SgGuiVlbiSrcStrModelItem* twItem, QWidget* parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  src_ = src;
  twItem_ = twItem;
  ssmPoint_ = twItem_?twItem_->getPoint():NULL;
  isModified_ = false;
  isNewPoint_ = false;
  setWindowTitle("SS Model Editor");

  QLabel                       *label;
  QGroupBox                    *gbox;
  QBoxLayout                   *layout, *subLayout;
  QGridLayout                  *grid;
  
  QString                       str;
  
  if (!ssmPoint_)
  {
    ssmPoint_ = new SgVlbiSourceInfo::StructModelMp(0.0, 0.0, 0.0, 0.0, false, false, false);
    isModified_ = true;
    isNewPoint_ = true;
  };
  
  gbox = new QGroupBox("SS Model", this);
  grid = new QGridLayout(gbox);

  label= new QLabel("x (mas):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
  label= new QLabel("y (mas):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 0, Qt::AlignLeft | Qt::AlignVCenter);
  label= new QLabel("k: ", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);
  label= new QLabel("b: ", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 3, 0, Qt::AlignLeft | Qt::AlignVCenter);


  //
  leX_ = new QLineEdit(str.sprintf("%.4f", ssmPoint_->getX()*RAD2MAS), gbox);
  leX_->setFixedWidth(leX_->fontMetrics().width("YYYYYYYY") + 10);
  grid ->addWidget(leX_, 0, 1);
  
  leY_ = new QLineEdit(str.sprintf("%.4f", ssmPoint_->getY()*RAD2MAS), gbox);
  leY_->setFixedWidth(leY_->fontMetrics().width("YYYYYYYY") + 10);
  grid ->addWidget(leY_, 1, 1);

  leK_ = new QLineEdit(str.sprintf("%.4f", ssmPoint_->getK()), gbox);
  leK_->setFixedWidth(leK_->fontMetrics().width("YYYYYYYY") + 10);
  grid ->addWidget(leK_, 2, 1);

  leB_ = new QLineEdit(str.sprintf("%.4f", ssmPoint_->getB()), gbox);
  leB_->setFixedWidth(leB_->fontMetrics().width("YYYYYYYY") + 10);
  grid ->addWidget(leB_, 3, 1);


  grid->addItem(new QSpacerItem(15, 0), 0, 6);
 

  //---
  layout = new QVBoxLayout(this);
  layout -> addWidget(gbox);
  
  cbEstPosition_ = new QCheckBox("Estimate position of the component", this); 
  cbEstPosition_->setChecked(ssmPoint_->getEstimatePosition());
  cbEstPosition_->setMinimumSize(cbEstPosition_->sizeHint());
  layout->addWidget(cbEstPosition_);

  cbEstK_ = new QCheckBox("Estimate brightness ratio", this); 
  cbEstK_->setChecked(ssmPoint_->getEstimateRatio());
  cbEstK_->setMinimumSize(cbEstK_->sizeHint());
  layout->addWidget(cbEstK_);
  
  cbEstB_ = new QCheckBox("Estimate a difference of spectral indices", this); 
  cbEstB_->setChecked(ssmPoint_->getEstimateSpIdx());
  cbEstB_->setMinimumSize(cbEstB_->sizeHint());
  layout->addWidget(cbEstB_);
 
  layout->addSpacing(15);
  
  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);

  QPushButton                  *bOk=new QPushButton("OK", this);
  QPushButton                  *bCancel=new QPushButton("Cancel", this);
  bOk->setDefault(true);
  QSize                         btnSize;

  bCancel->setMinimumSize((btnSize=bCancel->sizeHint()));
  bOk->setMinimumSize(btnSize);
  subLayout->addWidget(bOk);
  subLayout->addWidget(bCancel);
  connect(bOk, SIGNAL(clicked()), SLOT(accept()));
  connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
};



//
void SgGuiVlbiSrcStrModelEditor::accept()
{
  acquireData();
  QDialog::accept();
  if (isNewPoint_)
    emit ssmPointCreated(ssmPoint_);
  else if (isModified_)
  {
    twItem_->setText( 1, QString("").sprintf("%.4f", ssmPoint_->getX()*RAD2MAS));
    twItem_->setText( 2, QString("").sprintf("%.4f", ssmPoint_->getY()*RAD2MAS));
    twItem_->setText( 3, ssmPoint_->getEstimatePosition()?"Y":"N");
    twItem_->setText( 4, QString("").sprintf("%.4f", ssmPoint_->getK()));
    twItem_->setText( 5, ssmPoint_->getEstimateRatio()?"Y":"N");
    twItem_->setText( 6, QString("").sprintf("%.4f", ssmPoint_->getB()));
    twItem_->setText( 7, ssmPoint_->getEstimateSpIdx()?"Y":"N");
  };
  emit ssmPointModified(isModified_);
  deleteLater();
};



//
void SgGuiVlbiSrcStrModelEditor::reject()
{
  emit ssmPointModified(false);
  QDialog::reject(); 
  if (ssmPoint_ && isNewPoint_)
    delete ssmPoint_;
  deleteLater();
};



//
void SgGuiVlbiSrcStrModelEditor::acquireData()
{
  if (!src_)
    return;

  bool                          isOK=true;
  double                        d=0.0;

  if (isOK)
    d = leX_->text().toDouble(&isOK);
  if (isOK && d!=ssmPoint_->getX()*RAD2MAS)
  {
    isModified_ = true;
    ssmPoint_->setX(d/RAD2MAS);
  };
  if (isOK)
    d = leY_->text().toDouble(&isOK);
  if (isOK && d!=ssmPoint_->getY()*RAD2MAS)
  {
    isModified_ = true;
    ssmPoint_->setY(d/RAD2MAS);
  };

  if (isOK)
    d = leK_->text().toDouble(&isOK);
  if (isOK && d!=ssmPoint_->getK())
  {
    isModified_ = true;
    ssmPoint_->setK(d);
  };
  if (isOK)
    d = leB_->text().toDouble(&isOK);
  if (isOK && d!=ssmPoint_->getB())
  {
    isModified_ = true;
    ssmPoint_->setB(d);
  };

  if (isOK && cbEstPosition_->isChecked()!=ssmPoint_->getEstimatePosition())
  {
    ssmPoint_->setEstimatePosition(cbEstPosition_->isChecked());
    isModified_ = true;
  };
  if (isOK && cbEstK_->isChecked()!=ssmPoint_->getEstimateRatio())
  {
    ssmPoint_->setEstimateRatio(cbEstK_->isChecked());
    isModified_ = true;
  };
  if (isOK && cbEstB_->isChecked()!=ssmPoint_->getEstimateSpIdx())
  {
    ssmPoint_->setEstimateSpIdx(cbEstB_->isChecked());
    isModified_ = true;
  };
};
/*=====================================================================================================*/




/*=====================================================================================================*/






