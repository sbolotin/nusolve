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

#include <SgGuiVlbiBaselineList.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QGridLayout>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QPushButton>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QCheckBox>
#   include <QtGui/QGridLayout>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QPushButton>
#endif




#include <SgLogger.h>
#include <SgTaskConfig.h>
#include <SgVlbiBand.h>
#include <SgVlbiSession.h>





enum BaselineColumnIndex
{
  BCI_NUMBER      =  0,
  BCI_NAME        =  1,
  BCI_TOT_OBS     =  2,
  BCI_USB_OBS     =  3,
  BCI_PRC_OBS     =  4,
  //
  BCI_S_WRMS_DEL  =  5,
  BCI_S_IGNORE    =  6,
  BCI_S_LENGTH    =  7,
  BCI_S_SIG0_DEL  =  8,
  BCI_S_CLK_EST   =  9,
  BCI_S_CLK_VAL   = 10,
  BCI_S_CLK_SIG   = 11,
  BCI_S_IONO4GRD  = 12,
  BCI_S_IONO4PHD  = 13,
  BCI_S_BIND_TRP  = 14,

  //
  BCI_B_DISP_DEL  =  5,
  BCI_B_DISP_RAT  =  6,
  BCI_B_SIG0_DEL  =  7,
  BCI_B_SIG0_RAT  =  8,
  BCI_B_WRMS_DEL  =  9,
  BCI_B_WRMS_RAT  = 10,
  BCI_B_AMBIG_SP  = 11,
  BCI_B_NUM_CHAN  = 12,
};




/*=======================================================================================================
*
*                   SgGuiVlbiBaselineItem METHODS:
* 
*======================================================================================================*/
bool SgGuiVlbiBaselineItem::operator<(const QTreeWidgetItem &other) const
{
  int                           column=treeWidget()->sortColumn();
  double                        dL, dR;
  bool                          isOk;
  const SgVlbiBaselineInfo     *oBl=((const SgGuiVlbiBaselineItem&)other).getBlInfo();
  //
  // sort by normalized value of the estimated clock offset:
  if (column == BCI_S_CLK_EST && blInfo_ && oBl)
  {
    if (blInfo_->dClockSigma()==0.0 && oBl->dClockSigma()!=0.0)
      return true;
    else if (blInfo_->dClockSigma()!=0.0 && oBl->dClockSigma()==0.0)
      return false;
    else if (blInfo_->dClockSigma()!=0.0 && oBl->dClockSigma()!=0.0)
      return fabs(blInfo_->dClock()/blInfo_->dClockSigma()) < fabs(oBl->dClock()/oBl->dClockSigma());
  };
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
*                   SgGuiVlbiBaselineList METHODS:
* 
*======================================================================================================*/
SgGuiVlbiBaselineList::SgGuiVlbiBaselineList(SgVlbiSession *session, SgTaskConfig *cfg,
  SgObjectBrowseMode mode, QMap<QString, SgVlbiBaselineInfo*>* aMap, 
  QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f), 
    ownerName_(session->getName()),
    refClockStations_(),
    bClockShiftStrong_(QColor(  0,105,205)),
    bClockShiftOk_    (QColor(  0,  0,  0)),
    bClockShiftWeak_  (QColor(240,160, 20)),
    bClockShiftBad_   (QColor(240, 15, 90))
//    bClockShiftStrong_(QColor(  0,160, 20)),
{
  session_ = session;
  cfg_ = cfg;
  QString                       str;
  QBoxLayout                   *layout=new QVBoxLayout(this);
  double                        d;
  scl4delay_ = 1.0e12;
  scl4rate_  = 1.0e15;
  browseMode_ = mode;
  baselinesByName_ = aMap;
  constColumns_ = -1;
  numbersBrowseMode_ = 0;

  QStringList                   headerLabels;
  headerLabels 
    << "Idx" 
    << "Name" 
    << "TotObs"
    << "GoodObs"
    << "PrcdObs"
    ;
  switch (browseMode_)
  {
  case OBM_BAND: 
    headerLabels 
      << "Disp(ps)"
      << "Disp(fs/s)"
      << "Sig0(ps)"
      << "Sig0(fs/s)"
      << "WRMS(ps)"
      << "WRMS(fs/s)"
      << "Ambig.Spc"
      << "#Channels"
      ;
    break;
  case OBM_SESSION: 
    constColumns_ =  9;
    headerLabels 
      << "WRMS(ps)"
      << " Omit "
      << "length(m)"
      << "Sig0(ps)"
      << "Est.Clk"
      << "ClkVal"
      << "ClkSig"
      << "IonGrd"
      << "IonPhd"
    ;
    break;
  case OBM_PROJECT: 
    break;
  };
  
  tweBaselines_ = new SgGuiQTreeWidgetExt(this);
  tweBaselines_->setColumnCount(headerLabels.count());
  tweBaselines_->setHeaderLabels(headerLabels);

  QMap<QString, SgVlbiBaselineInfo*>::const_iterator i = baselinesByName_->constBegin();
  for (; i!=baselinesByName_->constEnd(); ++i)
  {
    SgVlbiBaselineInfo* blInfo = i.value();

    SgGuiVlbiBaselineItem *item = new SgGuiVlbiBaselineItem(tweBaselines_);
    item->setBlInfo(blInfo);
    item->setText(BCI_NUMBER,  str.sprintf("%4d", blInfo->getIdx()));
    item->setData(BCI_NUMBER,  Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(BCI_NUMBER,  Qt::UserRole, true);
    //
    item->setText(BCI_NAME,    blInfo->getKey());
    item->setData(BCI_NAME,    Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setData(BCI_NAME,    Qt::UserRole, true);
    //
    item->setText(BCI_TOT_OBS, str.sprintf("%7d", blInfo->numTotal(DT_DELAY)));
    item->setData(BCI_TOT_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(BCI_TOT_OBS, Qt::UserRole, true);
    //
    item->setText(BCI_USB_OBS, str.sprintf("%7d", blInfo->numUsable(DT_DELAY)));
    item->setData(BCI_USB_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(BCI_USB_OBS, Qt::UserRole, true);
    //
    item->setText(BCI_PRC_OBS, str.sprintf("%7d", blInfo->numProcessed(DT_DELAY)));
    item->setData(BCI_PRC_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(BCI_PRC_OBS, Qt::UserRole, true);

    if (browseMode_==OBM_SESSION)
    {
      item->setText(BCI_S_WRMS_DEL, str.sprintf("%.1f", blInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(BCI_S_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_S_WRMS_DEL, Qt::UserRole, true);
      //
      item->setText(BCI_S_IGNORE,   blInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)? "X" : "");
      item->setData(BCI_S_IGNORE,   Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(BCI_S_IGNORE,   Qt::UserRole, false);
      //
      item->setText(BCI_S_LENGTH,   str.sprintf("%.1f", blInfo->getLength()));
      item->setData(BCI_S_LENGTH,   Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_S_LENGTH,   Qt::UserRole, true);
      //
      item->setText(BCI_S_SIG0_DEL, str.sprintf("%.1f", blInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setData(BCI_S_SIG0_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_S_SIG0_DEL, Qt::UserRole, true);
      //
      item->setText(BCI_S_CLK_VAL,  str.sprintf("%.1f", blInfo->dClock()*scl4delay_));
      item->setData(BCI_S_CLK_VAL,  Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_S_CLK_VAL,  Qt::UserRole, true);
      //
      item->setText(BCI_S_CLK_SIG,  str.sprintf("%.1f", blInfo->dClockSigma()*scl4delay_));
      item->setData(BCI_S_CLK_SIG,  Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_S_CLK_SIG,  Qt::UserRole, true);

      d = blInfo->dClockSigma()==0.0?1.0:fabs(blInfo->dClock()/blInfo->dClockSigma());
      if (blInfo->dClockSigma()==0.0 || d>=3.0)
      {
        if (fabs(blInfo->dClock())>10.0e-12)
          item->setForeground(BCI_S_CLK_EST, bClockShiftStrong_);
        else
          item->setForeground(BCI_S_CLK_EST, bClockShiftOk_);
      }
      else if (1.0<=d && d<3.0)
        item->setForeground(BCI_S_CLK_EST, bClockShiftWeak_);
      else
        item->setForeground(BCI_S_CLK_EST, bClockShiftBad_);
      QFont                     font(item->font(7));
      font.setBold(true);

      item->setFont(BCI_S_CLK_EST,  font);
      item->setText(BCI_S_CLK_EST,  blInfo->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS)? "Y" : "");
      item->setData(BCI_S_CLK_EST,  Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(BCI_S_CLK_EST,  Qt::UserRole, false);

      item->setText(BCI_S_IONO4GRD, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD)? "Y" : "");
      item->setData(BCI_S_IONO4GRD, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(BCI_S_IONO4GRD, Qt::UserRole, false);

      item->setText(BCI_S_IONO4PHD, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD)? "Y" : "");
      item->setData(BCI_S_IONO4PHD, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(BCI_S_IONO4PHD, Qt::UserRole, false);
    }
    else if (browseMode_==OBM_BAND)
    {
      item->setText(BCI_B_DISP_DEL, str.sprintf("%.1f", blInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setData(BCI_B_DISP_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_DISP_DEL, Qt::UserRole, false);

      item->setText(BCI_B_DISP_RAT, str.sprintf("%.1f", blInfo->dispersion(DT_RATE)*scl4rate_));
      item->setData(BCI_B_DISP_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_DISP_RAT, Qt::UserRole, false);

      item->setText(BCI_B_SIG0_DEL, str.sprintf("%.1f", blInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setData(BCI_B_SIG0_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_SIG0_DEL, Qt::UserRole, false);

      item->setText(BCI_B_SIG0_RAT, str.sprintf("%.1f", blInfo->getSigma2add(DT_RATE)*scl4rate_));
      item->setData(BCI_B_SIG0_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_SIG0_RAT, Qt::UserRole, false);

      item->setText(BCI_B_WRMS_DEL, str.sprintf("%.1f", blInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(BCI_B_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_WRMS_DEL, Qt::UserRole, false);

      item->setText(BCI_B_WRMS_RAT, str.sprintf("%.1f", blInfo->wrms(DT_RATE)*scl4rate_));
      item->setData(BCI_B_WRMS_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_WRMS_RAT, Qt::UserRole, false);

      item->setText(BCI_B_AMBIG_SP, str.sprintf("%.2f", blInfo->getTypicalGrdAmbigSpacing()*1.0e9));
      item->setData(BCI_B_AMBIG_SP, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_AMBIG_SP, Qt::UserRole, false);

      item->setText(BCI_B_NUM_CHAN, str.sprintf("%d", blInfo->getTypicalNumOfChannels()));
      item->setData(BCI_B_NUM_CHAN, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(BCI_B_NUM_CHAN, Qt::UserRole, false);
    };
  };
  
  tweBaselines_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  tweBaselines_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  tweBaselines_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  tweBaselines_->header()->setStretchLastSection(false);
  
  tweBaselines_->setSortingEnabled(true);
  tweBaselines_->setUniformRowHeights(true);
  tweBaselines_->sortByColumn(1, Qt::AscendingOrder);
  tweBaselines_->setFocus();
  tweBaselines_->setItemsExpandable(false);
  tweBaselines_->setAllColumnsShowFocus(true);

  layout->addWidget(tweBaselines_);
  
  if (browseMode_!=OBM_BAND)
    connect(tweBaselines_,
      SIGNAL(moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)), 
      SLOT  (toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)));
  connect(tweBaselines_,
    SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
    SLOT  (entryDoubleClicked(QTreeWidgetItem*, int)));
  connect(tweBaselines_,
    SIGNAL(itemClicked (QTreeWidgetItem*, int)),
    SLOT  (entryClicked(QTreeWidgetItem*, int))
    );
};



//
void SgGuiVlbiBaselineList::addRefClockStation(const QString& stName)
{
  bool                          isContains=false;
  for (int i=0; i<refClockStations_.size(); i++)
    if (refClockStations_.at(i) == stName)
      isContains = true;
  if (isContains)
  {
    logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
      "addRefClockStation(): the station " + stName + 
      " already is in the reference clock stations list");
    return;
  };
  refClockStations_.append(stName);
};



//
void SgGuiVlbiBaselineList::delRefClockStation(const QString& stName)
{
  bool                          isContains=false;
  isContains = refClockStations_.removeOne(stName);
  if (!isContains)
  {
    logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
      "delRefClockStation(): cannot remove the station " + stName + 
      " from the reference clock stations list");
    return;
  };
};



//
void SgGuiVlbiBaselineList::toggleEntryMoveEnable(QTreeWidgetItem* item, int n, 
  Qt::MouseButton /* mouseButtonState */, Qt::KeyboardModifiers /* modifiers */)
{
  if (!item) // may be NULL
    return;

  bool                          isTmp;
  SgVlbiBaselineInfo*           blInfo=((SgGuiVlbiBaselineItem*)item)->getBlInfo();

  switch (n)
  {
  case BCI_S_IGNORE:
    blInfo->xorAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
    item->setText(n, blInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)? "X" : "");
    break;
  case BCI_S_CLK_EST:
    isTmp = false;
    for (int i=0; i<refClockStations_.size(); i++)
      if (blInfo->getKey().contains(refClockStations_.at(i)))
        isTmp = true;
    if (!isTmp || blInfo->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS)) // we are able to deselect
    {
      blInfo->xorAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
      item->setText(n, blInfo->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS)? "Y" : "");
    };
    break;
  case BCI_S_IONO4GRD:
    blInfo->xorAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD);
    item->setText(n, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD)? "Y" : "");
    break;
  case BCI_S_IONO4PHD:
    blInfo->xorAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD);
    item->setText(n, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD)? "Y" : "");
    break;
//  case BCI_S_BIND_TRP:
//    blInfo->xorAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE);
//    item->setText(n, blInfo->isAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE)? "Y" : "");
//    break;
  default:
    //std::cout << "default, n=" << n << "\n";
    break;
  };
};



//
void SgGuiVlbiBaselineList::entryClicked(QTreeWidgetItem* item, int column)
{
  if (!item)
    return;
  if (column != BCI_USB_OBS && column != BCI_PRC_OBS)
    return;
  if (numbersBrowseMode_ == 0)
    numbersBrowseMode_ = 1;
  else 
    numbersBrowseMode_ = 0;
  
  QTreeWidgetItem              *headerItem = tweBaselines_->headerItem();
  headerItem->setText(BCI_USB_OBS, numbersBrowseMode_==0?"GoodObs":"G/T, %");
  headerItem->setText(BCI_PRC_OBS, numbersBrowseMode_==0?"PrcdObs":"P/G, %");
  
  updateContent4Nums();
};



//
void SgGuiVlbiBaselineList::entryDoubleClicked(QTreeWidgetItem* item, int column)
{
  if (!item || !item->data(column, Qt::UserRole).toBool())
    return;
  //
  SgVlbiBaselineInfo           *blInfo=((SgGuiVlbiBaselineItem*)item)->getBlInfo();
  if (blInfo)
  {
    SgGuiVlbiBlnInfoEditor     *biEditor=new SgGuiVlbiBlnInfoEditor(session_, cfg_, blInfo, browseMode_,
      ownerName_, this);
    connect(biEditor, SIGNAL(contentModified(bool)), SLOT(modifyBaselineInfo(bool)));
    biEditor->show();
  };
};



//
void SgGuiVlbiBaselineList::modifyBaselineInfo(bool isModified)
{
  if (isModified)
    updateContent();
};



//
void SgGuiVlbiBaselineList::updateContent()
{
  QString                       str;
  SgVlbiBaselineInfo           *blInfo=NULL;
  SgGuiVlbiBaselineItem        *item=NULL;
  QTreeWidgetItemIterator       it(tweBaselines_);
  double                        d;
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiBaselineItem*)(*it);
    blInfo = item->getBlInfo();
    numTotal = blInfo->numTotal(DT_DELAY);
    numUsable = blInfo->numUsable(DT_DELAY);
    item->setText(BCI_NUMBER, str.sprintf("%4d", blInfo->getIdx()));
    item->setText(BCI_NAME, blInfo->getKey());
    item->setText(BCI_TOT_OBS, str.sprintf("%7d", blInfo->numTotal(DT_DELAY)));
    /*
    item->setText(BCI_USB_OBS, str.sprintf("%7d", blInfo->numUsable(DT_DELAY)));
    item->setText(BCI_PRC_OBS, str.sprintf("%7d", blInfo->numProcessed(DT_DELAY)));
    */
    //
    if (numbersBrowseMode_ == 0)
    {
      item->setText(BCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(BCI_PRC_OBS,  str.sprintf("%7d", blInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(BCI_USB_OBS, "N/A");
      else
        item->setText(BCI_USB_OBS, str.sprintf("%.2f", 100.0*blInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(BCI_PRC_OBS, "N/A");
      else
        item->setText(BCI_PRC_OBS, str.sprintf("%.2f", 100.0*blInfo->numProcessed(DT_DELAY)/numUsable));
    };
    //
    if (browseMode_==OBM_SESSION)
    {
      item->setText(BCI_S_WRMS_DEL, str.sprintf("%.1f", blInfo->wrms(DT_DELAY)*scl4delay_));
      item->setText(BCI_S_IGNORE, blInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID)? "X" : "");

      item->setText(BCI_S_SIG0_DEL, str.sprintf("%.1f", blInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setText(BCI_S_CLK_VAL, str.sprintf("%.1f", blInfo->dClock()*scl4delay_));
      item->setText(BCI_S_CLK_SIG, str.sprintf("%.1f", blInfo->dClockSigma()*scl4delay_));
      //
      d = blInfo->dClockSigma()==0.0?1.0:fabs(blInfo->dClock()/blInfo->dClockSigma());
      if (blInfo->dClockSigma()==0.0 || d>=3.0)
      {
        if (fabs(blInfo->dClock())>10.0e-12)
          item->setForeground(BCI_S_CLK_EST, bClockShiftStrong_);
        else
          item->setForeground(BCI_S_CLK_EST, bClockShiftOk_);
      }
      else if (1.0<=d && d<3.0)
        item->setForeground(BCI_S_CLK_EST, bClockShiftWeak_);
      else
        item->setForeground(BCI_S_CLK_EST, bClockShiftBad_);
      item->setText(BCI_S_CLK_EST, blInfo->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS)? "Y" : "");
      item->setText(BCI_S_IONO4GRD, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD)? "Y" : "");
      item->setText(BCI_S_IONO4PHD, blInfo->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD)? "Y" : "");
    }
    else if (browseMode_==OBM_BAND)
    {
      item->setText   (BCI_B_DISP_DEL, str.sprintf("%.1f", blInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setText   (BCI_B_DISP_RAT, str.sprintf("%.1f", blInfo->dispersion(DT_RATE)*scl4rate_));
      item->setText   (BCI_B_SIG0_DEL, str.sprintf("%.1f", blInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setText   (BCI_B_SIG0_RAT, str.sprintf("%.1f", blInfo->getSigma2add(DT_RATE)*scl4rate_));
      item->setText   (BCI_B_WRMS_DEL, str.sprintf("%.1f", blInfo->wrms(DT_DELAY)*scl4delay_));
      item->setText   (BCI_B_WRMS_RAT, str.sprintf("%.1f", blInfo->wrms(DT_RATE)*scl4rate_));
    };
    ++it;
  };
};



//
void SgGuiVlbiBaselineList::updateContent4Nums()
{
  QString                       str;
  SgVlbiBaselineInfo           *blInfo=NULL;
  SgGuiVlbiBaselineItem        *item=NULL;
  QTreeWidgetItemIterator       it(tweBaselines_);
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiBaselineItem*)(*it);
    blInfo = item->getBlInfo();
    numTotal = blInfo->numTotal(DT_DELAY);
    numUsable = blInfo->numUsable(DT_DELAY);
    if (numbersBrowseMode_ == 0)
    {
      item->setText(BCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(BCI_PRC_OBS,  str.sprintf("%7d", blInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(BCI_USB_OBS, "N/A");
      else
        item->setText(BCI_USB_OBS, str.sprintf("%.2f", 100.0*blInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(BCI_PRC_OBS, "N/A");
      else
        item->setText(BCI_PRC_OBS, str.sprintf("%.2f", 100.0*blInfo->numProcessed(DT_DELAY)/numUsable));
    };
    ++it;
  };
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*                   SgGuiVlbiBlnInfoEditor METHODS:
* 
*======================================================================================================*/
SgGuiVlbiBlnInfoEditor::SgGuiVlbiBlnInfoEditor(SgVlbiSession *session, SgTaskConfig *cfg,
  SgVlbiBaselineInfo* blnInfo, SgObjectBrowseMode mode, const QString& ownerName, 
  QWidget* parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  static const QString          attrNames[] = 
  {
    QString("Ignore the baseline"),
    QString("Estimate the clock shift for the baseline"),
    QString("Use ionosphere correction for group delay (if available)"),
    QString("Use ionosphere correction for phase delay (if available)"),
    QString("Bind the zenith delays of the two stations"),
    QString("Do not perform weight corrections for this baseline"),
  };
  QSize                         btnSize;
  QBoxLayout                   *mainLayout, *vLayout, *hLayout;
  QGroupBox                    *gbox;

  session_ = session;
  cfg_ = cfg;
  baselineInfo_ = blnInfo;
  browseMode_ = mode;
  isModified_ = false;

  switch (browseMode_)
  {
  case OBM_BAND:
    setWindowTitle("The baseline " + baselineInfo_->getKey() + " at the " + ownerName + "-Band");
    break;
  case OBM_SESSION:
    setWindowTitle("The baseline " + baselineInfo_->getKey() + " at the  " + ownerName + " session");
    break;
  case OBM_PROJECT:
    setWindowTitle("The baseline " + baselineInfo_->getKey() + " in the " + ownerName + " project");
    break;
  };

  //
  mainLayout = new QVBoxLayout(this);
  //
  // attributes:
  gbox = new QGroupBox("Attributes of Baseline", this);
  vLayout = new QVBoxLayout(gbox);
  for (int i=0; i<6; i++)
  {
    cbAttributes_[i] = new QCheckBox(attrNames[i], gbox);
    cbAttributes_[i]->setMinimumSize(cbAttributes_[i]->sizeHint());
    vLayout->addWidget(cbAttributes_[i]);
  };
  cbAttributes_[0]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID));
  cbAttributes_[1]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS));
  cbAttributes_[2]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD));
  cbAttributes_[3]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD));
  cbAttributes_[4]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE));
  cbAttributes_[5]->setChecked(baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_SKIP_WC));
  mainLayout->addWidget(gbox, 10);
  //
  // aux sigmas:
  gbox = new QGroupBox("Sigmas for rewighting", this);
  QGridLayout *grid = new QGridLayout(gbox);

  grid->addWidget(new QLabel("Additional sigma for delays (ps):", gbox), 0, 0);
  leAuxSigma4Delay_ = new QLineEdit(gbox);
  leAuxSigma4Delay_->setText(QString("").sprintf("%.2f", baselineInfo_->getSigma2add(DT_DELAY)*1.0E12));
  grid->addWidget(leAuxSigma4Delay_, 0, 1);

  grid->addWidget(new QLabel("Additional sigma for rates (fs/s):", gbox), 1, 0);
  leAuxSigma4Rate_ = new QLineEdit(gbox);
  leAuxSigma4Rate_->setText(QString("").sprintf("%.2f", baselineInfo_->getSigma2add(DT_RATE)*1.0E15));
  grid->addWidget(leAuxSigma4Rate_, 1, 1);
  
  mainLayout->addWidget(gbox, 10);
  //
  // estimation of baseline clock shift:
  gbox = new QGroupBox("Esimtation of baseline clock shift", this);
  grid = new QGridLayout(gbox);
  grid->addWidget(new QLabel("Value of clocks offset (ps):", gbox), 0, 0);
  grid->addWidget(
    new QLabel(QString("").sprintf("%.2f", baselineInfo_->dClock()*1.0e12), gbox), 0, 1);
  grid->addWidget(new QLabel("Std.dev of clocks offset (ps):", gbox), 1, 0);
  grid->addWidget(
    new QLabel(QString("").sprintf("%.2f", baselineInfo_->dClockSigma()*1.0e12), gbox), 1, 1);
  
  mainLayout->addWidget(gbox, 10);
  
  //
  //
  QPushButton *bOk       = new QPushButton("OK", this);
  QPushButton *bCancel   = new QPushButton("Cancel", this);
  bOk->setDefault(true);

  bCancel->setMinimumSize((btnSize=bCancel->sizeHint()));
  bOk->setMinimumSize(btnSize);
  //

  hLayout = new QHBoxLayout();
  mainLayout->addSpacing(15);
  mainLayout->addStretch(1);
  mainLayout->addLayout(hLayout);

  hLayout->addStretch(1);
  hLayout->addWidget(bOk);
  hLayout->addWidget(bCancel);

  connect(bOk,     SIGNAL(clicked()), SLOT(accept()));
  connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
  mainLayout->activate();
};



//
SgGuiVlbiBlnInfoEditor::~SgGuiVlbiBlnInfoEditor()
{
  session_ = NULL;
  cfg_ = NULL;
  baselineInfo_ = NULL;
  emit contentModified(isModified_);
};



//
void SgGuiVlbiBlnInfoEditor::accept()
{
  acquireData();
  QDialog::accept();
  deleteLater();
};



//
void SgGuiVlbiBlnInfoEditor::acquireData()
{
  // Attributes:
  if (cbAttributes_[0]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
    isModified_ = true;
  };
  if (cbAttributes_[1]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);
    isModified_ = true;
  };
  if (cbAttributes_[2]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD);
    isModified_ = true;
  };
  if (cbAttributes_[3]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD);
    isModified_ = true;
  };
  if (cbAttributes_[4]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE);
    isModified_ = true;
  };
  if (cbAttributes_[5]->isChecked() != baselineInfo_->isAttr(SgVlbiBaselineInfo::Attr_SKIP_WC))
  {
    baselineInfo_->xorAttr(SgVlbiBaselineInfo::Attr_SKIP_WC);
    isModified_ = true;
  };
  //
  QString                       str;
  bool                          isOk;
  double                        d;
  str = leAuxSigma4Delay_->text();
  d = str.toDouble(&isOk);
  if (isOk && d!=baselineInfo_->getSigma2add(DT_DELAY)*1.0E12)
  {
    baselineInfo_->setSigma2add(DT_DELAY, d*1.0E-12);
    isModified_ = true;
    // propagate the value to its right place, @band:
    if (session_ && cfg_)
    {
      SgVlbiBand               *band=session_->bands().at(cfg_->getActiveBandIdx());
      if (band && band->baselinesByName().contains(baselineInfo_->getKey()))
      {
        SgVlbiBaselineInfo     *bandBi=band->baselinesByName().value(baselineInfo_->getKey());
        bandBi->setSigma2add(DT_DELAY, d*1.0E-12);
        logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::STATION, className() + 
          "::acquireData(): the additional delay std.devs were propagated to the " + band->getKey() +
          "-band for the baseline [" + baselineInfo_->getKey() + "]");
      }
      else
      {
        logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
          "::acquireData(): cannot find baseline [" + baselineInfo_->getKey() + "] for the " +
          (band?band->getKey():"NULL") + "-band");
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
        "::acquireData(): session or config is NULL for the baseline [" + 
        baselineInfo_->getKey() + "]");
  };
  str = leAuxSigma4Rate_->text();
  d = str.toDouble(&isOk);
  if (isOk && d!=baselineInfo_->getSigma2add(DT_RATE)*1.0E15)
  {
    baselineInfo_->setSigma2add(DT_RATE, d*1.0E-15);
    isModified_ = true;
    // propagate the value to its right place, @band:
    if (session_ && cfg_)
    {
      SgVlbiBand               *band=session_->bands().at(cfg_->getActiveBandIdx());
      if (band && band->baselinesByName().contains(baselineInfo_->getKey()))
      {
        SgVlbiBaselineInfo     *bandBi=band->baselinesByName().value(baselineInfo_->getKey());
        bandBi->setSigma2add(DT_RATE, d*1.0E-15);
        logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::STATION, className() + 
          "::acquireData(): the additional rate std.devs were propagated to the " + band->getKey() +
          "-band for the baseline [" + baselineInfo_->getKey() + "]");
      }
      else
      {
        logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
          "::acquireData(): cannot find baseline [" + baselineInfo_->getKey() + "] for the " +
          (band?band->getKey():"NULL") + "-band");
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::STATION, className() + 
        "::acquireData(): session or config is NULL for the baseline [" + 
        baselineInfo_->getKey() + "]");
  };
};



//
void SgGuiVlbiBlnInfoEditor::updateModifyStatus(bool isModified)
{
  isModified_ = isModified_ || isModified;
};
/*=====================================================================================================*/







/*=====================================================================================================*/
