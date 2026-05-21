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


#include <SgGuiVlbiStationList.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#endif




#include <SgGuiParameterCfg.h>
#include <SgLogger.h>
#include <SgParametersDescriptor.h>





enum StationColumnIndex
{
  SCI_NUMBER      =  0,
  SCI_NAME        =  1,
  SCI_SCANS       =  2,
  SCI_TOT_OBS     =  3,
  SCI_USB_OBS     =  4,
  SCI_PRC_OBS     =  5,
  //
  SCI_S_WRMS_DEL  =  6,
  SCI_S_IGNORE    =  7,
  SCI_S_ACM       =  8,
  SCI_S_CLK_BRKS  =  9,
  SCI_S_CLK_TERMS = 10,
  SCI_S_REF_CLK   = 11,
  
  SCI_S_DFLT_CO   = 12,
  SCI_S_CBL_ORI   = 13,
  SCI_S_CBL_SGN   = 14,

  SCI_S_CBL_CAL   = 15,
  SCI_S_FLAGS     = 16,
  SCI_S_LC        = 17,
  SCI_S_LZ        = 18,
  SCI_S_COO_EST   = 19,
  SCI_S_COO_CON   = 20,
  SCI_S_AXO_EST   = 21,
  //
  SCI_B_DISP_DEL  =  6,
  SCI_B_DISP_RAT  =  7,
  SCI_B_SIG0_DEL  =  8,
  SCI_B_SIG0_RAT  =  9,
  SCI_B_WRMS_DEL  = 10,
  SCI_B_WRMS_RAT  = 11,
  SCI_B_CLK_BRKS  = 12,
};

const QString                 sCableCalOrigs[] = {"Default", "FSLog", "CDMS", "PCMT"};
const QString                 sDefaultCableCalOrigs[] = {"N/A", "FSLog", "CDMS", "PCMT", 
                                "CDMS from FSLog"};



/*=======================================================================================================
*
*                   SgGuiVlbiStationItem METHODS:
* 
*======================================================================================================*/
bool SgGuiVlbiStationItem::operator<(const QTreeWidgetItem &other) const
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
*                   SgGuiVlbiStationList METHODS:
* 
*======================================================================================================*/
SgGuiVlbiStationList::SgGuiVlbiStationList(SgObjectBrowseMode mode, const QString& ownerName,
  QMap<QString, SgVlbiStationInfo*>* map, const SgMJD& t0, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  scl4delay_ = 1.0e12;
  scl4rate_  = 1.0e15;
  t0_ = t0;
  browseMode_ = mode;
  stationsByName_ = map;
  ownerName_ = ownerName;
  numbersBrowseMode_ = 0;
  QString                       str;
  QBoxLayout                   *layout = new QVBoxLayout(this);
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
    headerLabels 
      << "Disp(ps)"
      << "Disp(fs/s)"
      << "Sig0(ps)"
      << "Sig0(fs/s)"
      << "WRMS(ps)"
      << "WRMS(fs/s)"
      << "Clk:Brk"
      ;
    break;
  case OBM_SESSION: 
    headerLabels 
      << "WRMS(ps)"
      << " Omit "
      << "ACM"
      << "Clk:Brk"
      << "Clk:n"
      << "Clk:Ref"
      << "Cbl:Dflt"
      << "Cbl:Orig"
      << "Cbl:Sgn"
      << "Cbl:Cal"
      << "Flg"
      << "LC"
      << "LZ"
      << "R:Est"
      << "R:C"
      << "Ax:Est"
    ;
    break;
  case OBM_PROJECT: 
    break;
  };
  //
  tweStations_ = new SgGuiQTreeWidgetExt(this);
  tweStations_->setColumnCount(headerLabels.count());
  tweStations_->setHeaderLabels(headerLabels);
  //
  QMap<QString, SgVlbiStationInfo*>::const_iterator i = stationsByName_->constBegin();
  for (; i!=stationsByName_->constEnd(); ++i)
  {
    SgVlbiStationInfo* stInfo = i.value();

    SgGuiVlbiStationItem *item = new SgGuiVlbiStationItem(tweStations_);
    item->setStInfo(stInfo);
    item->setText(SCI_NUMBER,  str.sprintf("%4d", stInfo->getIdx()));
    item->setData(SCI_NUMBER,  Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(SCI_NUMBER,  Qt::UserRole, true);
    item->setToolTip(SCI_NUMBER, "Index of a station");
    
    item->setText(SCI_NAME,    stInfo->getKey());
    item->setData(SCI_NAME,    Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setData(SCI_NAME,    Qt::UserRole, true);

    item->setText(SCI_SCANS,   str.sprintf("%7d", stInfo->auxObservationByScanId()->size()));
    item->setData(SCI_SCANS,   Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(SCI_SCANS,   Qt::UserRole, true);
    
    item->setText(SCI_TOT_OBS, str.sprintf("%7d", stInfo->numTotal(DT_DELAY)));
    item->setData(SCI_TOT_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(SCI_TOT_OBS, Qt::UserRole, true);

    item->setText(SCI_USB_OBS, str.sprintf("%7d", stInfo->numUsable(DT_DELAY)));
    item->setData(SCI_USB_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(SCI_USB_OBS, Qt::UserRole, true);

    item->setText(SCI_PRC_OBS, str.sprintf("%7d", stInfo->numProcessed(DT_DELAY)));
    item->setData(SCI_PRC_OBS, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(SCI_PRC_OBS, Qt::UserRole, true);

    if (browseMode_==OBM_BAND)
    {
      item->setText(SCI_B_DISP_DEL, str.sprintf("%.1f", stInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_DISP_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_DISP_DEL, Qt::UserRole, true);

      item->setText(SCI_B_DISP_RAT, str.sprintf("%.1f", stInfo->dispersion(DT_RATE)*scl4rate_));
      item->setData(SCI_B_DISP_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_DISP_RAT, Qt::UserRole, true);

      item->setText(SCI_B_SIG0_DEL, str.sprintf("%.1f", stInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_SIG0_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_SIG0_DEL, Qt::UserRole, true);

      item->setText(SCI_B_SIG0_RAT, str.sprintf("%.1f", stInfo->getSigma2add(DT_RATE)*scl4rate_));
      item->setData(SCI_B_SIG0_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_SIG0_RAT, Qt::UserRole, true);

      item->setText(SCI_B_WRMS_DEL, str.sprintf("%.1f", stInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(SCI_B_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_WRMS_DEL, Qt::UserRole, true);

      item->setText(SCI_B_WRMS_RAT, str.sprintf("%.1f", stInfo->wrms(DT_RATE)*scl4rate_));
      item->setData(SCI_B_WRMS_RAT, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_B_WRMS_RAT, Qt::UserRole, true);

      item->setText(SCI_B_CLK_BRKS, str.sprintf("%d", stInfo->clockBreaks().size()));
      item->setData(SCI_B_CLK_BRKS, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_B_CLK_BRKS, Qt::UserRole, true);
      //
    };
    if (browseMode_==OBM_SESSION)
    {
      item->setText(SCI_S_WRMS_DEL, str.sprintf("%.1f", stInfo->wrms(DT_DELAY)*scl4delay_));
      item->setData(SCI_S_WRMS_DEL, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_S_WRMS_DEL, Qt::UserRole, true);
      //
      item->setText(SCI_S_IGNORE, stInfo->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)? "X" : "");
      item->setData(SCI_S_IGNORE, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_IGNORE, Qt::UserRole, false);
      //
      item->setText(SCI_S_CLK_BRKS, str.sprintf("%d", stInfo->clockBreaks().size()));
      item->setData(SCI_S_CLK_BRKS, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_CLK_BRKS, Qt::UserRole, true);
      //
      item->setText(SCI_S_ACM, stInfo->getNeed2useAPrioriClocks()? "Y" : "");
      item->setData(SCI_S_ACM, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_ACM, Qt::UserRole, true);
      //
      item->setText(SCI_S_DFLT_CO, sDefaultCableCalOrigs[stInfo->getCableCalsOrigin()]);
      item->setData(SCI_S_DFLT_CO, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_DFLT_CO, Qt::UserRole, true);
      //
      item->setText(SCI_S_CBL_ORI, sCableCalOrigs[stInfo->getFlybyCableCalSource()]);
      item->setData(SCI_S_CBL_ORI, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_CBL_ORI, Qt::UserRole, true);
      //
      item->setText(SCI_S_CBL_SGN, stInfo->getCableCalMultiplierDBCal()==-1.0?"-":
        (stInfo->getCableCalMultiplierDBCal()==1.0?"+":" "));
      item->setData(SCI_S_CBL_SGN, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_CBL_SGN, Qt::UserRole, true);
      //
      item->setText(SCI_S_CLK_TERMS, str.sprintf("%2d", stInfo->getClocksModelOrder()));
      item->setData(SCI_S_CLK_TERMS, Qt::TextAlignmentRole, Qt::AlignRight);
      item->setData(SCI_S_CLK_TERMS, Qt::UserRole, false);
      //
      item->setText(SCI_S_REF_CLK, stInfo->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS)? "R" : "");
      item->setData(SCI_S_REF_CLK, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_REF_CLK, Qt::UserRole, false);
      //
      item->setText(SCI_S_CBL_CAL, stInfo->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)? "" : 
        (stInfo->isAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE)? "-Y" : "Y"));
      item->setData(SCI_S_CBL_CAL, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_CBL_CAL, Qt::UserRole, false);
      //
      item->setText(SCI_S_FLAGS, 
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO)? "m" : "") +
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)? "a" : "") +
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS)? "-Z" : "")
        );
      item->setData(SCI_S_FLAGS, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_FLAGS, Qt::UserRole, false);
      //
      item->setText(SCI_S_LC, stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS)? "Y" : "");
      item->setData(SCI_S_LC, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_LC, Qt::UserRole, false);
      //
      item->setText(SCI_S_LZ, stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH)? "Y" : "");
      item->setData(SCI_S_LZ, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_LZ, Qt::UserRole, false);
      //
      item->setText(SCI_S_COO_EST, stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO)? "Y" : "");
      item->setData(SCI_S_COO_EST, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_COO_EST, Qt::UserRole, false);
      //
      item->setText(SCI_S_COO_CON, stInfo->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO)? "*" : "");
      item->setData(SCI_S_COO_CON, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_COO_CON, Qt::UserRole, false);
      //
      item->setText(SCI_S_AXO_EST, stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_AXO)? "Y" : "");
      item->setData(SCI_S_AXO_EST, Qt::TextAlignmentRole, Qt::AlignCenter);
      item->setData(SCI_S_AXO_EST, Qt::UserRole, false);
    };
  };
  
  //
  // adjust the header:
  /*
  QTreeWidgetItem              *headerItem = tweStations_->headerItem();
  headerItem->setData(SCI_USB_OBS, Qt::TextAlignmentRole, Qt::AlignCenter);
  headerItem->setData(SCI_PRC_OBS, Qt::TextAlignmentRole, Qt::AlignCenter);
  */
  
  tweStations_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  tweStations_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  tweStations_->header()->setResizeMode(QHeaderView::Interactive);
#endif

  tweStations_->header()->setStretchLastSection(false);
  
  tweStations_->setSortingEnabled(true);
  tweStations_->setUniformRowHeights(true);
  tweStations_->sortByColumn(1, Qt::AscendingOrder);
  tweStations_->setFocus();
  tweStations_->setItemsExpandable(false);
  tweStations_->setAllColumnsShowFocus(true);
  layout->addWidget(tweStations_);
  
  if (browseMode_!=OBM_BAND)
  {
    connect(tweStations_,
      SIGNAL(moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers)), 
      SLOT  (toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers))
      );
  };
  connect(tweStations_,
    SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
    SLOT  (entryDoubleClicked(QTreeWidgetItem*, int))
    );
  connect(tweStations_,
    SIGNAL(itemClickedWithButton(QTreeWidgetItem*, int, Qt::MouseButton)),
    SLOT  (entryClickedWithButton(QTreeWidgetItem*, int, Qt::MouseButton))
    );
};




//
void SgGuiVlbiStationList::toggleEntryMoveEnable(QTreeWidgetItem* item, int column, 
  Qt::MouseButton mouseButtonState, Qt::KeyboardModifiers modifiers)
{
  if (!item) // may be NULL
    return;
  
  int                           n;
  QString                       str;
  SgVlbiStationInfo            *stInfo=((SgGuiVlbiStationItem*)item)->getStInfo();
  switch (column)
  {
  case SCI_S_IGNORE:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_NOT_VALID);
    item->setText(SCI_S_IGNORE, stInfo->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)? "X" : "");
    break;
  case SCI_S_CLK_TERMS:
    n = stInfo->getClocksModelOrder();
    switch (mouseButtonState)
    {
    case Qt::LeftButton:
      if (n>0)
        n--;
      break;
    case Qt::RightButton:
      if (n<10)
        n++;
      break;
    default:
      n = 3;
      break;
    };
    stInfo->setClocksModelOrder(n);
    item->setText(SCI_S_CLK_TERMS, str.sprintf("%2d", stInfo->getClocksModelOrder()));
    break;
  case SCI_S_REF_CLK:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
    if (stInfo->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
    {
      item->setText(SCI_S_REF_CLK, "R");
      emit refClockStationSelected(stInfo->getKey());
    }
    else
    {
      item->setText(SCI_S_REF_CLK, "");
      emit refClockStationDeselected(stInfo->getKey());
    };
    break;
  case SCI_S_CBL_CAL:
    if (modifiers == Qt::ControlModifier)
      stInfo->xorAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE);
    else
      stInfo->xorAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
    item->setText(SCI_S_CBL_CAL, stInfo->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)? "" :
      (stInfo->isAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE)? "-Y" : "Y"));
    break;
  case SCI_S_FLAGS:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_BAD_METEO);
    stInfo->resetRefractionModel();
    item->setText(SCI_S_FLAGS, 
      QString(stInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO)? "m" : "") +
      QString(stInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)? "a" : "") +
      QString(stInfo->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS)? "-Z" : "")
      );
    break;
  case SCI_S_LC:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
    item->setText(SCI_S_LC, stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS)? "Y" : "");
    break;
  case SCI_S_LZ:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
    item->setText(SCI_S_LZ, stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH)? "Y" : "");
    break;
  case SCI_S_COO_EST:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO);
    item->setText(SCI_S_COO_EST, stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO)? "Y" : "");
    break;
  case SCI_S_COO_CON:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO);
    item->setText(SCI_S_COO_CON, stInfo->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO)? "*" : "");
    break;
  case SCI_S_AXO_EST:
    stInfo->xorAttr(SgVlbiStationInfo::Attr_ESTIMATE_AXO);
    item->setText(SCI_S_AXO_EST, stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_AXO)? "Y" : "");
    break;
  default:
    // std::cout << "default, n=" << n << "\n";
    break;
  };
};



//
void SgGuiVlbiStationList::entryClickedWithButton(QTreeWidgetItem* item, int column, Qt::MouseButton button)
{
  if (!item || (column != SCI_USB_OBS && column != SCI_PRC_OBS) || button != Qt::RightButton)
    return;
  
  if (numbersBrowseMode_ == 0)
    numbersBrowseMode_ = 1;
  else 
    numbersBrowseMode_ = 0;
  
  QTreeWidgetItem              *headerItem = tweStations_->headerItem();
  headerItem->setText(SCI_USB_OBS, numbersBrowseMode_==0?"GoodObs":"G/T, %");
  headerItem->setText(SCI_PRC_OBS, numbersBrowseMode_==0?"PrcdObs":"P/G, %");
  
  updateContent4Nums();
};



//
void SgGuiVlbiStationList::entryDoubleClicked(QTreeWidgetItem* item, int column)
{
  if (!item || !item->data(column, Qt::UserRole).toBool())
    return;
  
  SgVlbiStationInfo            *stInfo=((SgGuiVlbiStationItem*)item)->getStInfo();
  if (stInfo)
  {
    SgGuiVlbiStnInfoEditor     *siEditor=new SgGuiVlbiStnInfoEditor(stInfo, browseMode_, ownerName_,
      t0_, this);
    connect(siEditor, SIGNAL(contentModified(bool)), SLOT(modifyStationInfo(bool)));
    siEditor->show();
  };
};



//
void SgGuiVlbiStationList::modifyStationInfo(bool isModified)
{
  if (isModified)
    updateContent();
};



//
void SgGuiVlbiStationList::updateContent()
{
  QString                       str;
  SgVlbiStationInfo            *stInfo=NULL;
  SgGuiVlbiStationItem         *item=NULL;
  QTreeWidgetItemIterator       it(tweStations_);
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiStationItem*)(*it);
    stInfo = item->getStInfo();
    numTotal = stInfo->numTotal(DT_DELAY);
    numUsable = stInfo->numUsable(DT_DELAY);
    //
    item->setText(SCI_NUMBER,   str.sprintf("%4d", stInfo->getIdx()));
    item->setText(SCI_NAME,     stInfo->getKey());
    item->setText(SCI_SCANS,    str.sprintf("%7d", stInfo->auxObservationByScanId()->size()));
    item->setText(SCI_TOT_OBS,  str.sprintf("%7d", stInfo->numTotal(DT_DELAY)));
    /*
    item->setText(SCI_USB_OBS,  str.sprintf("%7d", stInfo->numUsable(DT_DELAY)));
    item->setText(SCI_PRC_OBS,  str.sprintf("%7d", stInfo->numProcessed(DT_DELAY)));
    */
    //
    if (numbersBrowseMode_ == 0)
    {
      item->setText(SCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(SCI_PRC_OBS,  str.sprintf("%7d", stInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(SCI_USB_OBS, "N/A");
      else
        item->setText(SCI_USB_OBS, str.sprintf("%.2f", 100.0*stInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(SCI_PRC_OBS, "N/A");
      else
        item->setText(SCI_PRC_OBS, str.sprintf("%.2f", 100.0*stInfo->numProcessed(DT_DELAY)/numUsable));
    };
    //
    if (browseMode_==OBM_BAND)
    {
      item->setText(SCI_B_DISP_DEL, str.sprintf("%.1f", stInfo->dispersion(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_DISP_RAT, str.sprintf("%.1f", stInfo->dispersion(DT_RATE )*scl4rate_));
      item->setText(SCI_B_SIG0_DEL, str.sprintf("%.1f", stInfo->getSigma2add(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_SIG0_RAT, str.sprintf("%.1f", stInfo->getSigma2add(DT_RATE )*scl4rate_));
      item->setText(SCI_B_WRMS_DEL, str.sprintf("%.1f", stInfo->wrms(DT_DELAY)*scl4delay_));
      item->setText(SCI_B_WRMS_RAT, str.sprintf("%.1f", stInfo->wrms(DT_RATE )*scl4rate_));
      item->setText(SCI_B_CLK_BRKS, str.sprintf("%d", stInfo->clockBreaks().size()));
    };
    if (browseMode_==OBM_SESSION)
    {
      item->setText(SCI_S_WRMS_DEL, str.sprintf("%.1f", stInfo->wrms(DT_DELAY)*scl4delay_));
      item->setText(SCI_S_IGNORE,   stInfo->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)? "X" : "");
      item->setText(SCI_S_CLK_BRKS, str.sprintf("%d", stInfo->clockBreaks().size()));
      item->setText(SCI_S_ACM,      stInfo->getNeed2useAPrioriClocks()? "Y" : "");
      item->setText(SCI_S_DFLT_CO,  sDefaultCableCalOrigs[stInfo->getCableCalsOrigin()]);
      item->setText(SCI_S_CBL_ORI,  sCableCalOrigs[stInfo->getFlybyCableCalSource()]);
      item->setText(SCI_S_CLK_TERMS,str.sprintf("%2d", stInfo->getClocksModelOrder()));
      item->setText(SCI_S_REF_CLK,  stInfo->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS)? "R" : "");
      item->setText(SCI_S_CBL_CAL,  stInfo->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)? "" : 
        (stInfo->isAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE)? "-Y" : "Y"));
      item->setText(SCI_S_FLAGS, 
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_BAD_METEO)? "m" : "") +
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_ART_METEO)? "a" : "") +
        QString(stInfo->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS)? "-Z" : "")
        );
      item->setText(SCI_S_LC,       stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS)? "Y" : "");
      item->setText(SCI_S_LZ,       stInfo->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH)? "Y" : "");
      item->setText(SCI_S_COO_EST,  stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO)? "Y" : "");
      item->setText(SCI_S_COO_CON,  stInfo->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO)? "*" : "");
      item->setText(SCI_S_AXO_EST,  stInfo->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_AXO)? "Y" : "");
    };
    ++it;
  };
};



//
void SgGuiVlbiStationList::updateContent4Nums()
{
  QString                       str;
  SgVlbiStationInfo            *stInfo=NULL;
  SgGuiVlbiStationItem         *item=NULL;
  QTreeWidgetItemIterator       it(tweStations_);
  int                           numTotal;
  int                           numUsable;
  while (*it)
  {
    item = (SgGuiVlbiStationItem*)(*it);
    stInfo = item->getStInfo();
    numTotal = stInfo->numTotal(DT_DELAY);
    numUsable = stInfo->numUsable(DT_DELAY);
    if (numbersBrowseMode_ == 0)
    {
      item->setText(SCI_USB_OBS,  str.sprintf("%7d", numUsable));
      item->setText(SCI_PRC_OBS,  str.sprintf("%7d", stInfo->numProcessed(DT_DELAY)));
    }
    else
    {
      if (numTotal == 0)
        item->setText(SCI_USB_OBS, "N/A");
      else
        item->setText(SCI_USB_OBS, str.sprintf("%.2f", 100.0*stInfo->numUsable(DT_DELAY)/numTotal));
      if (numUsable == 0)
        item->setText(SCI_PRC_OBS, "N/A");
      else
        item->setText(SCI_PRC_OBS, str.sprintf("%.2f", 100.0*stInfo->numProcessed(DT_DELAY)/numUsable));
    };
    ++it;
  };
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                   SgGuiVlbiStnInfoEditor METHODS:
* 
*======================================================================================================*/
SgGuiVlbiStnInfoEditor::SgGuiVlbiStnInfoEditor(SgVlbiStationInfo* stnInfo, SgObjectBrowseMode mode,
  const QString& ownerName, const SgMJD& t0, QWidget* parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  static const QString          attrNames[] = 
  {
    QString("Ignore the station"),
    QString("Clocks of the station are the reference ones"),
    QString("The station has a bad cable calibration"),
    QString("The station has an inverted cable calibration"),
    QString("Metheo data on the station are wrong"),
    QString("Metheo data on the station are artificial or suspicious"),
    QString("Do not estimate tropospheric refraction parameters"),
  };
  QString                       str;
  QSize                         btnSize;
  stationInfo_ = stnInfo;
  browseMode_ = mode;
  flybyCableCalSource_ = stationInfo_->getFlybyCableCalSource();  
  t0_ = t0;
  bModel_ = stationInfo_->clockBreaks();
  bModel_.setT0(t0_);
  
  isModified_ = false;

  switch (browseMode_)
  {
  case OBM_BAND:
    setWindowTitle("The station " + stationInfo_->getKey() + " on the " + ownerName + "-Band");
    break;
  case OBM_SESSION:
    setWindowTitle("The station " + stationInfo_->getKey() + " at " + ownerName + " session");
    break;
  case OBM_PROJECT:
    setWindowTitle("The station " + stationInfo_->getKey() + " of " + ownerName + " project");
    break;
  };

  //
  QVBoxLayout                  *mainLayout=new QVBoxLayout(this);

  // clock breaks:
  QGroupBox                    *gBox=new QGroupBox("List of Clock Break Events", this);
  QBoxLayout                   *layout=new QVBoxLayout(gBox);

  QStringList headerLabels;
  headerLabels 
    << "Epoch of a clock break   " 
    << "Jump (ns)"
    << "Sigma (ns)"
    << "Dynamic"
    ;
  twClockBreaks_ = new QTreeWidget(gBox);
  twClockBreaks_->setColumnCount(2);
  twClockBreaks_->setHeaderLabels(headerLabels);
  //
  //
  for (int i=0; i<bModel_.size(); i++)
  {
    SgParameterBreak           *b=bModel_.at(i);
    SgGuiVlbiClockBreakItem    *item=new SgGuiVlbiClockBreakItem(twClockBreaks_);
    item->setClockBreak(b);
    item->setText( 0, b->toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    item->setData( 0, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 1, QString("").sprintf("%.1f", b->getA0()));
    item->setData( 1, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 2, QString("").sprintf("%.1f", b->getS0()));
    item->setData( 2, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText( 3, b->isAttr(SgParameterBreak::Attr_DYNAMIC)?"Y":"N");
    item->setData( 3, Qt::TextAlignmentRole, Qt::AlignCenter);
  };
  twClockBreaks_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  twClockBreaks_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  twClockBreaks_->header()->setResizeMode(QHeaderView::Interactive);
#endif

  twClockBreaks_->header()->setStretchLastSection(false);
  twClockBreaks_->setSortingEnabled(true);
  twClockBreaks_->setUniformRowHeights(true);
  twClockBreaks_->sortByColumn(0, Qt::AscendingOrder);
  twClockBreaks_->setFocus();
  twClockBreaks_->setItemsExpandable(false);
  twClockBreaks_->setAllColumnsShowFocus(true);
  twClockBreaks_->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  twClockBreaks_->setMinimumSize(twClockBreaks_->sizeHint());  
  layout->addWidget(twClockBreaks_);
  
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
  if (browseMode_ == OBM_SESSION)
  {
    QPushButton                *bLClocks=new QPushButton("Edit Clocks", this);
    QPushButton                *bLZenith=new QPushButton("Edit Zenith", this);
    QGroupBox                  *gboxAttr=new QGroupBox("Attributes of Station", this);
    layout = new QVBoxLayout(gboxAttr);
    layout->setSpacing(layout->spacing()/2);
    for (int i=0; i<7; i++)
    {
      cbAttributes_[i] = new QCheckBox(attrNames[i], gboxAttr);
      cbAttributes_[i]->setMinimumSize(cbAttributes_[i]->sizeHint());
      layout->addWidget(cbAttributes_[i]);
    };
    cbAttributes_[0]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_NOT_VALID));
    cbAttributes_[1]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS));
    cbAttributes_[2]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL));
    cbAttributes_[3]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE));
    cbAttributes_[4]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_BAD_METEO));
    cbAttributes_[5]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_ART_METEO));
    cbAttributes_[6]->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS));

    // options:
    mainLayout->addSpacing(15);
    mainLayout->addWidget(gboxAttr, 10);

    QRadioButton               *rbCccSource[4];
    QGroupBox                  *gbox=new QGroupBox("Source of cable calibration correction:", this);
    QButtonGroup               *bgFlybySrcCcc = new QButtonGroup(gbox);
    layout = new QVBoxLayout(gbox);
    layout->setSpacing(layout->spacing()/2);
    for (int i=0; i<4; i++)
    {
      str = sCableCalSourceNames[i];
      if (i==0)
        str += " (" + sDefaultCableCalOrigs[stnInfo->getCableCalsOrigin()] + ")";
      rbCccSource[i] = new QRadioButton("Use cable calibration correction from: " + str, gbox);
      rbCccSource[i]->setMinimumSize(rbCccSource[i]->sizeHint());
      bgFlybySrcCcc->addButton(rbCccSource[i], i);
      layout->addWidget(rbCccSource[i]);
    };
    if (!stationInfo_->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_FSLG))
      bgFlybySrcCcc->buttons()[1]->setEnabled(false);
    if (!stationInfo_->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS))
      bgFlybySrcCcc->buttons()[2]->setEnabled(false);
    if (!stationInfo_->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT))
      bgFlybySrcCcc->buttons()[3]->setEnabled(false);
    rbCccSource[stationInfo_->getFlybyCableCalSource()]->setChecked(true);
    connect(bgFlybySrcCcc, SIGNAL(buttonClicked(int)), SLOT(flybySourceCccFModified(int)));
    mainLayout->addWidget(gbox, 10);


    QGridLayout                *grid=new QGridLayout();
    QLabel                     *label;
    mainLayout->addLayout(grid);
    //
    cbUseAPrioriClocks_ = new QCheckBox("Use user specified a priori clocks", this);
    cbUseAPrioriClocks_->setChecked(stationInfo_->getNeed2useAPrioriClocks());
    cbUseAPrioriClocks_->setMinimumSize(cbUseAPrioriClocks_->sizeHint());
    grid->addWidget(cbUseAPrioriClocks_, 0,0);

    label = new QLabel("Offset (s):", this);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, 0, 1);
    
    leAPrioriClockTerm_0_ = new QLineEdit(str.sprintf("%g", stationInfo_->getAPrioriClockTerm_0()), this);
    leAPrioriClockTerm_0_->setMinimumWidth(leAPrioriClockTerm_0_->fontMetrics().width("WWWWW"));
    grid->addWidget(leAPrioriClockTerm_0_, 0,2);

    label = new QLabel("Rate (s/s):", this);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, 1, 1);
  
    leAPrioriClockTerm_1_ = new QLineEdit(str.sprintf("%g", stationInfo_->getAPrioriClockTerm_1()), this);
    leAPrioriClockTerm_1_->setMinimumWidth(leAPrioriClockTerm_1_->fontMetrics().width("WWWWW"));
    grid->addWidget(leAPrioriClockTerm_1_, 1,2);
    //
    //
    label = new QLabel("Number of polynomials for a clock model:", this);
    label->setMinimumSize(label->sizeHint());
    grid->addWidget(label, 2, 0);
  
    sbOrder_ = new QSpinBox(this);
    sbOrder_->setRange( 0, 10);
    sbOrder_->setValue(stationInfo_->getClocksModelOrder());
    sbOrder_->setMinimumSize(sbOrder_->sizeHint());
    grid->addWidget(sbOrder_, 2,1, 1,2);

    switch (stationInfo_->pcClocks()->getPMode())
    {
    default:
    case SgParameterCfg::PM_NONE:
      str="No";
      break;
    case SgParameterCfg::PM_ARC:
      str="Arc";
      break;
    case SgParameterCfg::PM_PWL:
      str="PWL";
      break;
    case SgParameterCfg::PM_STC:
      str="Stc";
      break;
    };
    cbUseLocalClocks_ = new QCheckBox("Use local setup of parameters for clocks (" + str + ")", this); 
    cbUseLocalClocks_->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS));
    cbUseLocalClocks_->setMinimumSize(cbUseLocalClocks_->sizeHint());
    grid->addWidget(cbUseLocalClocks_, 3,0);

    switch (stationInfo_->pcZenith()->getPMode())
    {
    default:
    case SgParameterCfg::PM_NONE:
      str="No";
      break;
    case SgParameterCfg::PM_ARC:
      str="Arc";
      break;
    case SgParameterCfg::PM_PWL:
      str="PWL";
      break;
    case SgParameterCfg::PM_STC:
      str="Stc";
      break;
    };
    cbUseLocalZenith_ = new QCheckBox("Use local setup of parameters for zenith delays (" + str + ")", 
      this);
    cbUseLocalZenith_->setChecked(stationInfo_->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH));
    cbUseLocalZenith_->setMinimumSize(cbUseLocalZenith_->sizeHint());
    grid->addWidget(cbUseLocalZenith_, 4,0);
  
    bLClocks->setMinimumSize(bLClocks->sizeHint());
    grid->addWidget(bLClocks, 3,1, 1,2);

    bLZenith->setMinimumSize(bLZenith->sizeHint());
    grid->addWidget(bLZenith, 4,1, 1,2);

    connect(bLClocks,SIGNAL(clicked()), SLOT(editLocalClocks()));
    connect(bLZenith,SIGNAL(clicked()), SLOT(editLocalZenith()));
  };
  
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


  connect(twClockBreaks_, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                          SLOT(editClockBreakRecordItem(QTreeWidgetItem*, int)));

  connect(bOk,     SIGNAL(clicked()), SLOT(accept()));
  connect(bAdd,    SIGNAL(clicked()), SLOT(insertClockBreakRecord()));
  connect(bEdit,   SIGNAL(clicked()), SLOT(editClockBreakRecord()));
  connect(bDelete, SIGNAL(clicked()), SLOT(deleteClockBreakRecord()));
  connect(bCancel, SIGNAL(clicked()), SLOT(reject()));

  mainLayout->activate();
};



//
SgGuiVlbiStnInfoEditor::~SgGuiVlbiStnInfoEditor()
{
  stationInfo_ = NULL;
  emit contentModified(isModified_);
};



//
void SgGuiVlbiStnInfoEditor::accept()
{
  acquireData();
  QDialog::accept();
  deleteLater();
};



//
void SgGuiVlbiStnInfoEditor::flybySourceCccFModified(int n)
{
  switch (n)
  {
  default:
  case 0:
    flybyCableCalSource_ = SgTaskConfig::CCS_DEFAULT;
    break;
  case 1:
    flybyCableCalSource_ = SgTaskConfig::CCS_FSLG;
    break;
  case 2:
    flybyCableCalSource_ = SgTaskConfig::CCS_CDMS;
    break;
  case 3:
    flybyCableCalSource_ = SgTaskConfig::CCS_PCMT;
    break;
  };
};



//
void SgGuiVlbiStnInfoEditor::acquireData()
{
  bool                          isOk;
  if (isModified_) // now, the flag could be changed only by clock break editor
    stationInfo_->clockBreaks() = bModel_;
  
  int           l=0;
  if (browseMode_ == OBM_SESSION)
  {
    // Attributes:
    if (cbAttributes_[0]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_NOT_VALID);
      isModified_ = true;
    };
    if (cbAttributes_[1]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);
      isModified_ = true;
    };
    if (cbAttributes_[2]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);
      isModified_ = true;
    };
    if (cbAttributes_[3]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_INVERSE_CABLE);
      isModified_ = true;
    };
    if (cbAttributes_[4]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_BAD_METEO))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_BAD_METEO);
      stationInfo_->resetRefractionModel();
      isModified_ = true;
    };
    if (cbAttributes_[5]->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_ART_METEO))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_ART_METEO);
      stationInfo_->resetRefractionModel();
      isModified_ = true;
    };
    if (cbAttributes_[6]->isChecked() != 
      stationInfo_->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS);
      isModified_ = true;
    };

    if (cbUseAPrioriClocks_->isChecked() != stationInfo_->getNeed2useAPrioriClocks())
    {
      stationInfo_->addAttr(SgVlbiStationInfo::Attr_APRIORI_CLOCK_MODIFIED);
      stationInfo_->setNeed2useAPrioriClocks(cbUseAPrioriClocks_->isChecked());
      isModified_ = true;
    };
    if (stationInfo_->getNeed2useAPrioriClocks())
    {
      double                      d;
      d = leAPrioriClockTerm_0_->text().toDouble(&isOk);
      if (isOk && d!=stationInfo_->getAPrioriClockTerm_0())
      {
        stationInfo_->addAttr(SgVlbiStationInfo::Attr_APRIORI_CLOCK_MODIFIED);
        stationInfo_->setAPrioriClockTerm_0(d);
        isModified_ = true;
      };
      d = leAPrioriClockTerm_1_->text().toDouble(&isOk);
      if (isOk && d!=stationInfo_->getAPrioriClockTerm_1())
      {
        stationInfo_->addAttr(SgVlbiStationInfo::Attr_APRIORI_CLOCK_MODIFIED);
        stationInfo_->setAPrioriClockTerm_1(d);
        isModified_ = true;
      };
    };
    // Peculiar parameters:
    if (cbUseLocalClocks_->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS);
      isModified_ = true;
    };
    if (cbUseLocalZenith_->isChecked() != stationInfo_->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH))
    {
      stationInfo_->xorAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH);
      isModified_ = true;
    };
    if ((l=sbOrder_->value()) != stationInfo_->getClocksModelOrder())
    {
      stationInfo_->setClocksModelOrder(l);
      isModified_ = true;
    };
    if (flybyCableCalSource_ != stationInfo_->getFlybyCableCalSource())
    {
      stationInfo_->setFlybyCableCalSource(flybyCableCalSource_);
      isModified_ = true;
    };
  };
};



//
void SgGuiVlbiStnInfoEditor::editClockBreakRecordItem(QTreeWidgetItem* item, int column)
{
  if (item && column>-1)
  {
    SgGuiVlbiStnClockBreakEditor *cbEditor=
      new SgGuiVlbiStnClockBreakEditor(&bModel_, browseMode_, (SgGuiVlbiClockBreakItem*)item, 
        stationInfo_, this);
    connect (cbEditor, SIGNAL(clockBreakModified(bool)), SLOT(updateModifyStatus(bool)));
    cbEditor->show();
  };
};



//
void SgGuiVlbiStnInfoEditor::editLocalClocks()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(stationInfo_->pcClocks(), 
    SgParametersDescriptor::Idx_CLOCK_0, true, this);
  connect (e, SIGNAL(valueModified(bool)), SLOT(updateModifyStatus(bool)));
  connect (e, SIGNAL(valueModified(bool)), SLOT(updateLClocksMode(bool)));
  e->show();
};



//
void SgGuiVlbiStnInfoEditor::editLocalZenith()
{
  SgGuiParameterCfg            *e=new SgGuiParameterCfg(stationInfo_->pcZenith(),
    SgParametersDescriptor::Idx_ZENITH, true, this);
  connect (e, SIGNAL(valueModified(bool)), SLOT(updateModifyStatus(bool)));
  connect (e, SIGNAL(valueModified(bool)), SLOT(updateLZenithMode(bool)));
  e->show();
};



//
void SgGuiVlbiStnInfoEditor::updateModifyStatus(bool isModified)
{
  isModified_ = isModified_ || isModified;
};



//
void SgGuiVlbiStnInfoEditor::updateLClocksMode(bool isModified)
{
  if (isModified)
  {
    QString                     str;
    switch (stationInfo_->pcClocks()->getPMode())
    {
    default:
    case SgParameterCfg::PM_NONE:
      str="No";
      break;
    case SgParameterCfg::PM_ARC:
      str="Arc";
      break;
    case SgParameterCfg::PM_PWL:
      str="PWL";
      break;
    case SgParameterCfg::PM_STC:
      str="Stc";
      break;
    };
    cbUseLocalClocks_->setText("Use local setup of parameters for clocks (" + str + ")");
  };
};



//
void SgGuiVlbiStnInfoEditor::updateLZenithMode(bool isModified)
{
  if (isModified)
  {
    QString                     str;
    switch (stationInfo_->pcZenith()->getPMode())
    {
    default:
    case SgParameterCfg::PM_NONE:
      str="No";
      break;
    case SgParameterCfg::PM_ARC:
      str="Arc";
      break;
    case SgParameterCfg::PM_PWL:
      str="PWL";
      break;
    case SgParameterCfg::PM_STC:
      str="Stc";
      break;
    };
    cbUseLocalZenith_->setText("Use local setup of parameters for zenith delays (" + str + ")");
  };
};



//
void SgGuiVlbiStnInfoEditor::addNewClockBreak(SgParameterBreak* b)
{
  if (b && bModel_.addBreak(b))
  {
    SgGuiVlbiClockBreakItem    *cbItem=new SgGuiVlbiClockBreakItem(twClockBreaks_);
    cbItem->setClockBreak(b);
    cbItem->setText( 0, b->toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    cbItem->setData( 0, Qt::TextAlignmentRole, Qt::AlignRight);
    cbItem->setText( 1, QString("").sprintf("%.1f", b->getA0()));
    cbItem->setData( 1, Qt::TextAlignmentRole, Qt::AlignRight);
    cbItem->setText( 2, QString("").sprintf("%.1f", b->getS0()));
    cbItem->setData( 2, Qt::TextAlignmentRole, Qt::AlignRight);
    cbItem->setText( 3, b->isAttr(SgParameterBreak::Attr_DYNAMIC)?"Y":"N");
    cbItem->setData( 3, Qt::TextAlignmentRole, Qt::AlignCenter);
    twClockBreaks_->resizeColumnToContents(0);
    twClockBreaks_->resizeColumnToContents(1);
    twClockBreaks_->resizeColumnToContents(2);
    twClockBreaks_->resizeColumnToContents(3);
  };
};



//
void SgGuiVlbiStnInfoEditor::editClockBreakRecord()
{
  if (twClockBreaks_->selectedItems().size())
  {
    SgGuiVlbiStnClockBreakEditor *cbEditor=
      new SgGuiVlbiStnClockBreakEditor(&bModel_, browseMode_,
      (SgGuiVlbiClockBreakItem*)(twClockBreaks_->selectedItems().at(0)), stationInfo_, this);
    connect (cbEditor, SIGNAL(clockBreakModified(bool)), SLOT(updateModifyStatus(bool)));
    cbEditor->show();
  };
};



//
void SgGuiVlbiStnInfoEditor::deleteClockBreakRecord()
{
  if (twClockBreaks_->selectedItems().size())
  {
    SgGuiVlbiClockBreakItem    *cbItem=(SgGuiVlbiClockBreakItem*)(twClockBreaks_->selectedItems().at(0));
    SgParameterBreak           *b=cbItem->getClockBreak();
    if (QMessageBox::warning(this, "Delete?",
        "Are you sure to delete clock break happened on\n \"" +
        b->toString() + "\"?\n",
        QMessageBox::No | QMessageBox::Yes, QMessageBox::No)==QMessageBox::Yes)
    {
      delete cbItem;
      bModel_.delBreak(*b);
      isModified_ = true;
    };
  };
};



//
void SgGuiVlbiStnInfoEditor::insertClockBreakRecord()
{
  SgGuiVlbiStnClockBreakEditor *cbEditor=new SgGuiVlbiStnClockBreakEditor(&bModel_, browseMode_, 
                                              NULL, stationInfo_, this);
  connect (cbEditor, SIGNAL(clockBreakModified(bool)), SLOT(updateModifyStatus(bool)));
  connect (cbEditor, SIGNAL(clockBreakCreated(SgParameterBreak*)),
                       SLOT(addNewClockBreak(SgParameterBreak*)));
  cbEditor->show();
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                  SgGuiVlbiStnClockBreakEditor  METHODS:
* 
*======================================================================================================*/
SgGuiVlbiStnClockBreakEditor::SgGuiVlbiStnClockBreakEditor(SgBreakModel* model, SgObjectBrowseMode mode,
                                SgGuiVlbiClockBreakItem* cbItem, SgVlbiStationInfo* si,
                                QWidget *parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  browseMode_ = mode;
  cbModel_ = model;
  cbItem_ = cbItem;
  clockBreak_ = cbItem?cbItem->getClockBreak():NULL;
  stationInfo_ = si;
//  twClockBreaks_ = cbItem_?cbItem_->treeWidget():NULL;

  isModified_ = false;
  isNewClockBreak_ = false;
  setWindowTitle("Clock Break Editor");

  QLabel                       *label;
  QGroupBox                    *gbox;
  QBoxLayout                   *layout, *subLayout;
  QGridLayout                  *grid;
  
  QString                       str;
  int                           yr=0, mo=0, dy=0, hr=0, mi=0;
  double                        se=0.0, valA0=0.0, sigA0=0.0;
  
  if (!clockBreak_)
  {
    clockBreak_ = new SgParameterBreak(cbModel_->getT0());
    clockBreak_->addAttr(SgParameterBreak::Attr_MODIFIED);
    isModified_ = true;
    isNewClockBreak_ = true;
  };
  
  SgMJD::MJD_reverse(clockBreak_->getDate(), clockBreak_->getTime(),  yr, mo, dy, hr, mi, se);

  valA0 = clockBreak_->getA0();
  sigA0 = clockBreak_->getS0();

  gbox = new QGroupBox("Clock Break", this);
  grid = new QGridLayout(gbox);

  label= new QLabel("Epoch:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 0, Qt::AlignLeft | Qt::AlignVCenter);

  label= new QLabel("Value (ns):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);
  
  label= new QLabel("Sigma (ns):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 3, 0, Qt::AlignLeft | Qt::AlignVCenter);
 
  //
  label= new QLabel("YYYY", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 1);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 2);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 3);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 4);

  label= new QLabel("DD", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 5);

  label= new QLabel("HH", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 7);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 8);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 9);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,10);

  label= new QLabel("SS.SS", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,11);

  //
  leYr_ = new QLineEdit(str.sprintf("%04d", yr), gbox);
  leYr_->setFixedWidth(leYr_->fontMetrics().width("YYYY") + 10);
  grid ->addWidget(leYr_, 1, 1);

  leMn_ = new QLineEdit(str.sprintf("%02d", mo), gbox);
  leMn_->setFixedWidth(leMn_->fontMetrics().width("MM") + 10);
  grid ->addWidget(leMn_, 1, 3);

  leDy_ = new QLineEdit(str.sprintf("%02d", dy), gbox);
  leDy_->setFixedWidth(leDy_->fontMetrics().width("MM") + 10);
  grid ->addWidget(leDy_, 1, 5);

  leHr_ = new QLineEdit(str.sprintf("%02d", hr), gbox);
  leHr_->setFixedWidth(leHr_->fontMetrics().width("MM") + 10);
  grid ->addWidget(leHr_, 1, 7);

  leMi_ = new QLineEdit(str.sprintf("%02d", mi), gbox);
  leMi_->setFixedWidth(leMi_->fontMetrics().width("MM") + 10);
  grid ->addWidget(leMi_, 1, 9);

  leSc_ = new QLineEdit(str.sprintf("%5.2f", se), gbox);
  leSc_->setFixedWidth(leSc_->fontMetrics().width("SS.SS") + 10);
  grid ->addWidget(leSc_, 1,11);

  //
  leA0_ = new QLineEdit(str.sprintf("%.2f", valA0), gbox);
  leA0_->setMinimumSize(leA0_->sizeHint());
  grid ->addWidget(leA0_, 2,1,  1,5);
  //grid->addColSpacing(6, 15);

  //
  leS0_ = new QLineEdit(str.sprintf("%.2f", sigA0), gbox);
  leS0_->setMinimumSize(leS0_->sizeHint());
  grid ->addWidget(leS0_, 3,1,  1,5);

  grid->addItem(new QSpacerItem(15, 0), 0, 6);
  

  //---
  layout = new QVBoxLayout(this);
  layout -> addWidget(gbox);
  
  cbIsDynamic_ = new QCheckBox("Estimate clock break parameters in a common solution", this); 
  if (browseMode_ == OBM_SESSION) // for session-wide only:
    cbIsDynamic_->setChecked(clockBreak_->isAttr(SgParameterBreak::Attr_DYNAMIC));
  cbIsDynamic_->setMinimumSize(cbIsDynamic_->sizeHint());
  layout->addWidget(cbIsDynamic_);
  layout->addSpacing(15);
  // prevent a user from modification of this attribute for band-dependent clock breaks:
  if (browseMode_ != OBM_SESSION)
    cbIsDynamic_->setEnabled(false);
  
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
void SgGuiVlbiStnClockBreakEditor::accept()
{
  if (acquireData())
  {
    QDialog::accept();
    if (isNewClockBreak_)
      emit clockBreakCreated(clockBreak_);
    else if (isModified_)
    {
      cbItem_->setText( 0, clockBreak_->toString(SgMJD::F_YYYYMMDDHHMMSSSS));
      cbItem_->setText( 1, QString("").sprintf("%.1f", clockBreak_->getA0()));
      cbItem_->setText( 2, QString("").sprintf("%.1f", clockBreak_->getS0()));
      cbItem_->setText( 3, clockBreak_->isAttr(SgParameterBreak::Attr_DYNAMIC)?"Y":"N");
    };
    emit clockBreakModified(isModified_);
  }
  else if (clockBreak_ && isNewClockBreak_)
    delete clockBreak_;
  deleteLater();
};



//
void SgGuiVlbiStnClockBreakEditor::reject()
{
  emit clockBreakModified(false);
  QDialog::reject(); 
  if (clockBreak_ && isNewClockBreak_)
    delete clockBreak_;
  deleteLater();
};



//
bool SgGuiVlbiStnClockBreakEditor::acquireData()
{
  if (!clockBreak_)
    return false;

  bool                          isOK=true;
  int                           yr=0;
  int                           mn=0;
  int                           dy=0;
  int                           hr=0;
  int                           mi=0;
  double                        d=0.0;

  if (isOK)
    yr = leYr_->text().toInt(&isOK);
  if (isOK)
    mn = leMn_->text().toInt(&isOK);
  if (isOK)
    dy = leDy_->text().toInt(&isOK);
  if (isOK)
    hr = leHr_->text().toInt(&isOK);
  if (isOK)
    mi = leMi_->text().toInt(&isOK);
  if (isOK)
    d = leSc_->text().toDouble(&isOK);

  SgMJD                         t(yr, mn, dy, hr, mi, d);
  //
  // check the provided epoch:
  SgMJD                         tStart=*stationInfo_->auxObservationByScanId()->first();
  SgMJD                         tFinis=*stationInfo_->auxObservationByScanId()->last();
  if (t - tStart < 10.0*60.0/DAY2SEC) // epoch of the break is less than 10 min from the first obs at
  {                                   // th estation
    isOK = false;
    logger->write(SgLogger::WRN, SgLogger::GUI, className() + 
      "::acquireData(): station " + stationInfo_->getKey() + 
      ": the clock epoch (" + t.toString(SgMJD::F_Simple) + 
      ") is withing 10 min from the first observation (" + tStart.toString(SgMJD::F_Simple) + 
      "); ignored");
  }
  else if (tFinis - t < 10.0*60.0/DAY2SEC) // epoch of the break is less than 10 min from the last obs at
  {                                        // the station
    isOK = false;
    logger->write(SgLogger::WRN, SgLogger::GUI, className() + 
      "::acquireData(): station " + stationInfo_->getKey() + 
      ": the clock epoch (" + t.toString(SgMJD::F_Simple) + 
      ") is withing 10 min from the last observation (" + tStart.toString(SgMJD::F_Simple) + 
      "); ignored");
  };
  //
  
  if (isOK && t!=*clockBreak_)
  {
    isModified_ = true;
    *(SgMJD*)clockBreak_ = t;
  };
  if (isOK)
    d = leA0_->text().toDouble(&isOK);
  if (isOK && d!=clockBreak_->getA0())
  {
    isModified_ = true;
    clockBreak_->setA0(d);
  };
  if (isOK)
    d = leS0_->text().toDouble(&isOK);
  if (isOK && d!=clockBreak_->getS0() && d>=0.0)
  {
    isModified_ = true;
    clockBreak_->setS0(d);
  };
  if (isOK && cbIsDynamic_->isChecked()!=clockBreak_->isAttr(SgParameterBreak::Attr_DYNAMIC))
  {
    if (cbIsDynamic_->isChecked())
      clockBreak_->addAttr(SgParameterBreak::Attr_DYNAMIC);
    else
      clockBreak_->delAttr(SgParameterBreak::Attr_DYNAMIC);
    isModified_ = true;
  };
  return isOK;
};
/*=====================================================================================================*/



/*=====================================================================================================*/
