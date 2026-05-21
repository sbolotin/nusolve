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

#include <math.h>


#include <SgGuiPlotter.h>
#include <SgMJD.h>
#include <SgVersion.h>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>




#if QT_VERSION >= 0x050000
#   include <QtWidgets/QApplication>
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QListView>
#   include <QtWidgets/QMenu>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QRadioButton>
#   include <QtCore/QStringListModel>
#   include <QtWidgets/QToolTip>
#else
#   include <QtGui/QApplication>
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QListView>
#   include <QtGui/QMenu>
#   include <QtGui/QMessageBox>
#   include <QtGui/QRadioButton>
#   include <QtGui/QStringListModel>
#   include <QtGui/QToolTip>
#endif

#include <QtGui/QFontMetrics>
#include <QtGui/QImage>
#include <QtGui/QPaintDevice>
#include <QtGui/QPainter>
#include <QtGui/QPdfWriter>





/*=======================================================================================================
*
*                           SgPlotBranch:
* 
*======================================================================================================*/
SgPlotBranch::SgPlotBranch(unsigned int numOfRows, 
                          unsigned int numOfValuesColumns, unsigned int numOfSigmasColumns,
                          const QString& name, bool hasExtKeys)
  :
    name_(name),
    alternativeTitleName_(),
    extKeys_()
{
  alternativeTitleName_[-1] = name;
  numOfRows_ = numOfRows;
  data_ = new SgMatrix(numOfRows_, numOfValuesColumns + numOfSigmasColumns + 1);
  isBrowsable_ = true;
  if ((hasExtKeys_=hasExtKeys))
  {
    extKeys_.resize(numOfRows_);
    extKeysVisible_.resize(numOfRows_);
    for (unsigned int i=0; i<numOfRows_; i++)
      extKeysVisible_[i] = true;
  };
//  idx_ = 0;
};



//
SgPlotBranch::~SgPlotBranch()
{
  if (data_)
  {
    delete data_;
    data_ = NULL;
  };
};



//
void SgPlotBranch::setDataAttr(unsigned int i, unsigned int attr)
{
  data_->setElement(i, data_->nCol()-1, (double) attr);
};



//
void SgPlotBranch::addDataAttr(unsigned int i, unsigned int attr)
{
  unsigned int a = getDataAttr(i);
  data_->setElement(i, data_->nCol()-1, (double) (a | attr));
};



//
void SgPlotBranch::delDataAttr(unsigned int i, unsigned int attr)
{
  unsigned int a = getDataAttr(i);
  data_->setElement(i, data_->nCol()-1, (double) (a & ~attr));
};



//
void SgPlotBranch::xorDataAttr(unsigned int i, unsigned int attr)
{
  unsigned int a = getDataAttr(i);
  data_->setElement(i, data_->nCol()-1, (double) (a ^ attr));
};



//
unsigned int SgPlotBranch::getDataAttr(unsigned int i) const
{
  return (unsigned int) data_->getElement(i, data_->nCol()-1);
};



//
bool SgPlotBranch::isPointInRanges(int idx, unsigned int limits) const
{
  SgPlotArea::SetsOfData        sod=(SgPlotArea::SetsOfData)limits;
  bool                          isInRanges(false);

  if (sod == SgPlotArea::SOD_ALL)
    isInRanges = true;
  else if (sod == SgPlotArea::SOD_USABLE)
    isInRanges = !(getDataAttr(idx) & SgPlotCarrier::DA_NONUSABLE);
  else if (sod == SgPlotArea::SOD_PROCESSED)
    isInRanges = !(getDataAttr(idx) & SgPlotCarrier::DA_NONUSABLE) && 
                 !(getDataAttr(idx) & SgPlotCarrier::DA_REJECTED);
  return isInRanges;
};
  


//
bool SgPlotBranch::isPointVisible(int idx, unsigned int limits) const
{
  SgPlotArea::SetsOfData        sod=(SgPlotArea::SetsOfData)limits;
  bool                          isVisible(false);

  if (hasExtKeys_ && !extKeysVisible_.at(idx))
    return false;
  
  if (sod == SgPlotArea::SOD_ALL)
    isVisible = true;
  else if (sod == SgPlotArea::SOD_USABLE)
    isVisible = !(getDataAttr(idx) & SgPlotCarrier::DA_NONUSABLE);
  else if (sod == SgPlotArea::SOD_PROCESSED)
  {
    isVisible = !(getDataAttr(idx) & SgPlotCarrier::DA_NONUSABLE) && 
                !(getDataAttr(idx) & SgPlotCarrier::DA_REJECTED);
  };
  return isVisible;
};



//
void SgPlotBranch::flagExtKey(const QString& eKey, bool on)
{
  for (unsigned int i=0; i<numOfRows_; i++)
    if (extKeys_.at(i).contains(eKey))
      extKeysVisible_[i] = on;
};
/*=====================================================================================================*/




/*=======================================================================================================
*
*                           SgPlotCarrier:
* 
*======================================================================================================*/
SgPlotCarrier::SgPlotCarrier(unsigned int numOfValuesColumns, 
  unsigned int numOfSigmasColumns,
  const QString& name) : 
  name_(),
  columnNames_(numOfValuesColumns + numOfSigmasColumns, NULL)
{
  numOfValuesColumns_ = numOfValuesColumns;
  numOfSigmasColumns_ = numOfSigmasColumns;

  name_[-1] = name;
  file2SaveBaseName_ = "unnamed_data";

  dataTypes_      = new int[numOfColumns()-1];
  dataStdVarIdx_  = new int[numOfColumns()-1];
  //set default values:
  for (int i=0; i<numOfColumns()-1; i++)
  {
    *(dataTypes_ + i) = AxisType_DATA;
    *(dataStdVarIdx_ + i) = -1;
  };
  isOK_ = false;
};



//
SgPlotCarrier::~SgPlotCarrier()
{
  if (dataTypes_)
  {
    delete[] dataTypes_;
    dataTypes_ = NULL;
  };
  if (dataStdVarIdx_)
  {
    delete[] dataStdVarIdx_;
    dataStdVarIdx_ = NULL;
  };
  for (unsigned int i=0; i<numOfValuesColumns_+numOfSigmasColumns_; i++)
    delete columnNames_.at(i);
  while (!listOfBranches_.isEmpty())
    delete listOfBranches_.takeFirst();
};



//
void SgPlotCarrier::createBranch(unsigned int numberOfRows, const QString& branchName,
  bool hasExtKeys)
{
  listOfBranches_.append(
    new SgPlotBranch(numberOfRows, numOfValuesColumns_, numOfSigmasColumns_, branchName, hasExtKeys));
};



//
bool SgPlotCarrier::selfCheck()
{
  isOK_ = false;
  // check sizes:
  if (!listOfBranches_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::GUI, className() +
      ": selfCheck(): the list of branches is empty, nothing to plot");
    return isOK_; // nothing to plot
  };

  // check names of columns, fill empty slots:
  for (unsigned int i=0; i<numOfValuesColumns_+numOfSigmasColumns_; i++)
    if (columnNames_.at(i) == NULL)
      columnNames_[i] = new QString("");

  // check sizes of each branch:
  for (int i=0; i<listOfBranches_.size(); i++)
    if (!listOfBranches_.at(i)->data()->nRow())
    {
      logger->write(SgLogger::WRN, SgLogger::GUI, className() +
        ": selfCheck(): the branch " + listOfBranches_.at(i)->getName() + " has no points");
    }
  // it's ok:
  return (isOK_=true);
};



//
void SgPlotCarrier::setNameOfColumn(unsigned int idx, const QString& name)
{
  if (idx<numOfValuesColumns_+numOfSigmasColumns_)
    columnNames_[idx] = new QString(name);
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgPlotArea:
* 
*======================================================================================================*/
QString SgPlotArea::xLabel4Unknown_("Unnamed X axis");
QString SgPlotArea::yLabel4Unknown_("Unnamed Y axis");
SgPlotArea::SgPlotArea(SgPlotCarrier* plotCarrier, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  plotCarrier_ = plotCarrier;

  width_  = 980;
  height_ = 510;
  visibleWidth_ = width_;
  visibleHeight_ = height_;

  xColumn_ = yColumn_ = 0;

  radius_ = 3;
  ddr_ = 1;

  isXTicsMJD_         = false;
  isPlotPoints_       = true;
  isPlotLines_        = false;
  isPlotErrBars_      = false;
  isPlotImpulses_     = false;
  have2HasZero_       = false;
  isLimitsOnVisible_  = false;
  rangeLimits_        = SOD_PROCESSED;
  sets2plot_          = SOD_PROCESSED;
  isStdVar_           = true;
  isRangeSymmetrical_ = false;

  xLabel_ = &xLabel4Unknown_;
  yLabel_ = &yLabel4Unknown_;

  setXColumn(0);
  setYColumn(1);

  resize(width_, height_);
  setBackgroundRole(QPalette::Base);
  framePen_ = new QPen(QColor(0,0,0), 1);
  zeroPen_  = new QPen(QColor(120,120,120), 1);
  barPen_   = new QPen(QColor(180,0,0), 1);
  rulerPen_ = new QPen(QColor(150,150,150), 1);
  ticLinesPen_ = new QPen(QColor(225,225,225), 1);
  rulerBrush_ = new QBrush(QColor(255, 230, 10, 200));
  ignoredPen_ = new QPen(QColor(140,140,140), 1);
  ignoredBrush_ = new QBrush(QColor(200,200,200));


  bpHuePhase_     =   0;
  bpSaturation_   = 255;
  bpValue_        = 230;

  branchPens_ = NULL;
  branchBrushes_ = NULL;
  branchSelectedPens_ = NULL;
  branchSelectedBrushes_ = NULL;
  initBranchPens();
  
  maxX_ = 20.0;
  maxY_ = 20.0;
  minX_ =-20.0;
  minY_ =-20.0;
  unsetUserDefinedRanges();

  //
  upMargin_ = 10;
  rightMargin_ = 10;
  xLabelWidth_ = 0;
  labelsHeight_= 0;
  yLabelWidth_ = 0;
  yLabelHeight_= 0;
  xTicsWidth_  = 0;
  yTicsWidth_  = 0;

  titleWidth_  = 0;
  // XTics:
  numOfXTics_  = 0;
  xStepP_      = 0;
  xTicsStep_   = 0.0;
  xTicsBias_   = 0.0;
  isXTicsBiased_= false;

  // YTics:
  numOfYTics_  = 0;
  yStepP_      = 0;
  yTicsStep_   = 0.0;
  yTicsBias_   = 0.0;
  isYTicsBiased_= false;

  xFrameBegin_ = 0;
  xFrameEnd_   = 0;
  yFrameBegin_ = 0;
  yFrameEnd_   = 0;

  xDataBegin_  = 0;
  xDataEnd_    = 0;
  xMargins_    = 0;
  yDataBegin_  = 0;
  yDataEnd_    = 0;
  yMargins_    = 0;

  f_Ax_ = f_Bx_ = f_Ay_ = f_By_ = 0.0;

  xTicsMJD_    = 0;
  
  rulerFromPoint_.setX(-1);
  rulerFromPoint_.setY(-1);
  rulerToPoint_.setX(-1);
  rulerToPoint_.setY(-1);
  rulerToPointPrev_.setX(-1);
  rulerToPointPrev_.setY(-1);

  cursorDefault_.setShape(Qt::ArrowCursor);
  cursorScrolling_.setShape(Qt::ClosedHandCursor);
  cursorMeasuring_.setShape(Qt::CrossCursor);
//  cursorMeasuring_.setShape(Qt::ArrowCursor);

  setUserMode(UserMode_DEFAULT);
};



//
SgPlotArea::~SgPlotArea()
{
  if (ignoredPen_) 
  {
    delete ignoredPen_;
    ignoredPen_ = NULL;
  };
  if (ignoredBrush_) 
  {
    delete ignoredBrush_;
    ignoredBrush_ = NULL;
  };
  if (rulerBrush_) 
  {
    delete rulerBrush_;
    rulerBrush_ = NULL;
  };
  if (framePen_) 
  {
    delete framePen_;
    framePen_ = NULL;
  };
  if (zeroPen_) 
  {
    delete zeroPen_;
    zeroPen_ = NULL;
  };
  if (barPen_) 
  {
    delete barPen_;
    barPen_ = NULL;
  };
  if (branchPens_)
  {
    delete[] branchPens_;
    branchPens_ = NULL;
  };  
  if (branchBrushes_)
  {
    delete[] branchBrushes_;
    branchBrushes_ = NULL;
  };
  if (branchSelectedPens_)
  {
    delete[] branchSelectedPens_;
    branchSelectedPens_ = NULL;
  };
  if (branchSelectedBrushes_)
  {
    delete[] branchSelectedBrushes_;
    branchSelectedBrushes_ = NULL;
  };
  if (ticLinesPen_)
  {
    delete ticLinesPen_;
    ticLinesPen_ = NULL;
  };
  if (rulerPen_)
  {
    delete rulerPen_;
    rulerPen_ = NULL;
  };
};



//
void SgPlotArea::setUserDefinedRanges(double minX, double maxX, double minY, double maxY)
{
  userDefinedMinX_ = minX;
  userDefinedMaxX_ = maxX;

  userDefinedMinY_ = minY;
  userDefinedMaxY_ = maxY;

  useUserDefinedRanges_ = true;
};



//
void SgPlotArea::unsetUserDefinedRanges()
{
  userDefinedMinX_ = 0.0;
  userDefinedMaxX_ = 0.0;

  userDefinedMinY_ = 0.0;
  userDefinedMaxY_ = 0.0;

  useUserDefinedRanges_ = false;
};



//
void SgPlotArea::setUserMode(UserMode mode)
{
  if (userMode_ != mode)
  {
    userMode_ = mode;
    switch (userMode_)
    {
    case UserMode_INQUIRING:
    case UserMode_QUERYING:
    case UserMode_SELECTING:
    case UserMode_DESELECTING:
      setCursor(cursorDefault_);
      break;

    case UserMode_SCROLLING:
      setCursor(cursorScrolling_);
      break;

    case UserMode_MEASURING:
      setCursor(cursorMeasuring_);
      break;

    case UserMode_RERANGING:
      setCursor(cursorMeasuring_);
      break;

    default:
    case UserMode_DEFAULT:
      setCursor(cursorDefault_);
      // zerofy ruler's points:
      rulerFromPoint_.setX(-1);
      rulerFromPoint_.setY(-1);
      rulerToPoint_.setX(-1);
      rulerToPoint_.setY(-1);
      rulerToPointPrev_.setX(-1);
      rulerToPointPrev_.setY(-1);
      break;
    };
  };
};



//
void SgPlotArea::initBranchPens()
{
  if (branchPens_)
    delete[] branchPens_;
  if (branchBrushes_)
    delete[] branchBrushes_;
  if (branchSelectedPens_)
    delete[] branchSelectedPens_;
  if (branchSelectedBrushes_)
    delete[] branchSelectedBrushes_;
  int numOfBranches = plotCarrier_->listOfBranches()->size();
  branchPens_ = new QPen[numOfBranches];
  branchBrushes_ = new QBrush[numOfBranches];
  branchSelectedPens_ = new QPen[numOfBranches];
  branchSelectedBrushes_ = new QBrush[numOfBranches];
  setBranchColors();
};



//
void SgPlotArea::setBranchColors()
{
  int numOfBranches = plotCarrier_->listOfBranches()->size();
  QPen   *pen = branchPens_;
  QBrush *brush = branchBrushes_;
  QPen   *pen4Selection = branchSelectedPens_;
  QBrush *brush4Selection = branchSelectedBrushes_;
  QColor  color, color4Selection;
  for (int i=0; i<numOfBranches; i++, pen++, brush++, pen4Selection++, brush4Selection++)
  {
    color.setHsv(bpHuePhase_ + (int)(360*i/numOfBranches), bpSaturation_, bpValue_);
    pen->setColor(color);
    pen->setWidth(1);
    brush->setColor(color);
    brush->setStyle(Qt::SolidPattern);

    color4Selection.setHsv((int)(360*i/numOfBranches), 255, 175);
    pen4Selection->setColor(color4Selection);
    pen4Selection->setWidth(3);
    brush4Selection->setColor(QColor(255,255,255));
    brush4Selection->setStyle(Qt::SolidPattern);
  };
};



//
void SgPlotArea::setBPHuePhase(int phase) 
{
  bpHuePhase_ = phase;
  setBranchColors();
  update();
};



//
void SgPlotArea::setBPSaturation(int saturation) 
{
  bpSaturation_ = saturation;
  setBranchColors();
  update();
};



//
void SgPlotArea::setBPValue(int value)
{
  bpValue_ = value;
  setBranchColors();
  update();
};



//
void SgPlotArea::setXColumn(unsigned int xColumn)
{
  if (plotCarrier_->isOK() && xColumn < (unsigned int)plotCarrier_->numOfColumns())
  {
    xColumn_ = xColumn;
    if (plotCarrier_->getAxisType(xColumn_) == SgPlotCarrier::AxisType_MJD) 
      isXTicsMJD_ = true;
    else 
      isXTicsMJD_ = false;
    if (*plotCarrier_->columnNames()->at(xColumn_) != "")
      xLabel_ = plotCarrier_->columnNames()->at(xColumn_);
    else 
      xLabel_ = &xLabel4Unknown_;
  };
};



//
void SgPlotArea::setYColumn(unsigned int yColumn)
{
  if (plotCarrier_->isOK() && yColumn < (unsigned int)plotCarrier_->numOfColumns())
  {
    yColumn_ = yColumn;
    if (*plotCarrier_->columnNames()->at(yColumn_) != "")
      yLabel_ = plotCarrier_->columnNames()->at(yColumn_);
    else 
      yLabel_ = &yLabel4Unknown_;
  };
};



//
void SgPlotArea::calcLimits()
{
  if (!plotCarrier_->isOK())
    return; // nothing to do
  
  if (useUserDefinedRanges_)
  {
    minX_ = userDefinedMinX_;
    maxX_ = userDefinedMaxX_;

    minY_ = userDefinedMinY_;
    maxY_ = userDefinedMaxY_;
  }
  else
  {
    // find out first browsable point:
    int                         iBranch=0;
    int                         ei=isStdVar_ ? plotCarrier_->getStdVarIdx(yColumn_) : -1;
    SgPlotBranch               *branch=plotCarrier_->listOfBranches()->at(iBranch);
    SetsOfData                  rangeLimits=rangeLimits_;
    // first, skip branches with zero data:
    while (iBranch<plotCarrier_->listOfBranches()->size() &&
      (branch=plotCarrier_->listOfBranches()->at(iBranch))->data()->nRow()<=0)
      iBranch++;
    if (iBranch==plotCarrier_->listOfBranches()->size() && branch->data()->nRow()<=0)
      return;         // all are turned off, nothing to calc, use previous limits
    //
    if (isLimitsOnVisible_)
    {
      while (iBranch<plotCarrier_->listOfBranches()->size() &&
        !(branch=plotCarrier_->listOfBranches()->at(iBranch))->getIsBrowsable())
        iBranch++;
      if (iBranch==plotCarrier_->listOfBranches()->size() && !branch->getIsBrowsable()) 
        return;         // all are turned off, nothing to calc, use previous limits
    };

    unsigned                    idx=0;
    if (rangeLimits != SOD_ALL)
    {
      bool                      isInvisible=true;
      while (isInvisible && iBranch<plotCarrier_->listOfBranches()->size())
      {
        idx = 0;
        while (idx < branch->data()->nRow() && 
//        (isInvisible = branch->getDataAttr(idx) & SgPlotCarrier::DA_REJECTED))
          (isInvisible = !(isLimitsOnVisible_?
            branch->isPointVisible(idx, rangeLimits):
              branch->isPointInRanges(idx, rangeLimits))
          ))
          idx++;
        if (isInvisible)
        {
          iBranch++;
          if (iBranch < plotCarrier_->listOfBranches()->size())
            branch = plotCarrier_->listOfBranches()->at(iBranch);
        };
      };
      if (isInvisible)  // all points are in DATA_IGNORE mode, calc limits using all data
      {
        rangeLimits = SOD_ALL;
        idx = 0;
      };
    };

    minX_ = branch->data()->getElement(idx, xColumn_);
    maxX_ = branch->data()->getElement(idx, xColumn_);
    minY_ = (ei>=0) ? branch->data()->getElement(idx, yColumn_) - branch->data()->getElement(idx, ei) :
                      branch->data()->getElement(idx, yColumn_);
    maxY_ = (ei>=0) ? branch->data()->getElement(idx, yColumn_) + branch->data()->getElement(idx, ei) :
                      branch->data()->getElement(idx, yColumn_);
    //
    for (int j=0; j<plotCarrier_->listOfBranches()->size(); j++)
    {
      branch = plotCarrier_->listOfBranches()->at(j);
      if (branch->getIsBrowsable() || !isLimitsOnVisible_)
        for (unsigned i=0; i<branch->data()->nRow(); i++)
        {
          if (isLimitsOnVisible_?
            branch->isPointVisible(i, rangeLimits):
              branch->isPointInRanges(i, rangeLimits))
          {
            minX_ = std::min(minX_, branch->data()->getElement(i, xColumn_));
            maxX_ = std::max(maxX_, branch->data()->getElement(i, xColumn_));
            minY_ = std::min(minY_, (ei>=0) ? branch->data()->getElement(i, yColumn_) - 
                      branch->data()->getElement(i, ei) : branch->data()->getElement(i, yColumn_) );
            maxY_ = std::max(maxY_, (ei>=0) ? branch->data()->getElement(i, yColumn_) + 
                      branch->data()->getElement(i, ei) : branch->data()->getElement(i, yColumn_) );
          };
        };
    };
  };
  //
  // make final arrangements:
  if (maxX_ == minX_)
  {
    minX_ -= 0.5;
    maxX_ += 0.5;
  };
  if (maxY_ == minY_)
  {
    minY_ -= 0.5;
    maxY_ += 0.5;
  };
  // adjust the limits:
  double                        d;
  d = (maxY_ + minY_)/2.0;
  if ( (maxY_-minY_)<d && (maxY_-minY_)/d<1.0e-12 )
  {
    
    minY_ -= d*1.0e-12;
    maxY_ += d*1.0e-12;
  };

  if (have2HasZero_)
  {
    if (maxX_*minX_ > 0.0 && plotCarrier_->getAxisType(xColumn_) != SgPlotCarrier::AxisType_MJD)
    {
      if (minX_>0.0) 
        minX_ = 0.0;
      else 
        maxX_ = 0.0;
    };
    if (maxY_*minY_ > 0.0 && plotCarrier_->getAxisType(yColumn_) != SgPlotCarrier::AxisType_MJD)
    {
      if (minY_ > 0.0) 
        minY_ = 0.0;
      else 
        maxY_ = 0.0;
    };
  };
  //
  if (isRangeSymmetrical_) // for nonMJD-axis only:
  {
    double                    max;
    if (plotCarrier_->getAxisType(xColumn_) != SgPlotCarrier::AxisType_MJD)
    {
      max = std::max(fabs(maxX_), fabs(minX_));
      maxX_ = max;
      minX_ =-max;
    };
    if (plotCarrier_->getAxisType(yColumn_) != SgPlotCarrier::AxisType_MJD)
    {
      max = std::max(fabs(maxY_), fabs(minY_));
      maxY_ = max;
      minY_ =-max;
    };
  };
  //
};



//
void SgPlotArea::calcTransforms()
{
  f_Bx_ = (xDataEnd_ - xDataBegin_)/(maxX_ - minX_);
  f_Ax_ = xDataEnd_ - f_Bx_*maxX_;

  f_By_ = (yDataEnd_ - yDataBegin_)/(minY_ - maxY_);
  f_Ay_ = yDataBegin_ - f_By_*maxY_;
};



//
void SgPlotArea::defineAreas(QPainter* painter)
{
  double dy = 0.0;
 
  isXTicsBiased_ = false;
  isYTicsBiased_ = false;

  QFontMetrics  fm(painter->fontMetrics());
  labelsHeight_ = fm.height();

  xLabelWidth_  = fm.width(*xLabel_);
  yLabelWidth_  = fm.width(*yLabel_);
  yLabelHeight_ = fm.height();
  titleWidth_   = fm.width(plotCarrier_->getName(yColumn_));

  // should be smarter a bit
  xTicsWidth_ = fm.width("800000000");
  if (isXTicsMJD_)
    xTicsWidth_ = fm.width(" " + SgMJD(minX_).toString(SgMJD::F_Date) + " ");

  upMargin_    = 3*labelsHeight_/2;
  yFrameBegin_ = upMargin_;
  yFrameEnd_   = height_ - 7*labelsHeight_/2;
  // Define Width of Y Tics:
  numOfYTics_ = (yFrameEnd_ - yFrameBegin_)/(5*labelsHeight_);
  yTicsStep_ = (maxY_ - minY_)/numOfYTics_;
  yStepP_ = (int)floor(log10(yTicsStep_));

  yTicsStep_ = trunc( yTicsStep_/exp10(yStepP_) );
  if (yTicsStep_ == 3)
    yTicsStep_ = 2;
  else if (yTicsStep_==4 || yTicsStep_==6 || yTicsStep_==7) 
    yTicsStep_ = 5;
  else if (yTicsStep_ == 8 || yTicsStep_ == 9)
    yTicsStep_ = 10;
  yTicsStep_ = yTicsStep_*exp10(yStepP_);
  yTicsWidth_ = 10;
  if ((maxY_ + yTicsStep_/3.0)*(minY_ - yTicsStep_/3.0)<0.0) // here the "0" is the origin
  {
    dy = 0.0;
    while (dy <= maxY_ + yTicsStep_/3.0)
    {
      yTicsWidth_ = std::max(yTicsWidth_, fm.width(QString().sprintf("%.8g", dy)));
      dy += yTicsStep_;
    };
    dy = 0.0;
    while (dy >= minY_ - yTicsStep_/3.0)
    {
      yTicsWidth_ = std::max(yTicsWidth_, fm.width(QString().sprintf("%.8g", dy)));
      dy -= yTicsStep_;
    };
  }
  else
  {
    if (log10(fabs(minY_)) - yStepP_ > 7.0) // need a bias
    {
      isYTicsBiased_ = true;
      yTicsBias_ = trunc( minY_/exp10(yStepP_ + 1.0) )*exp10(yStepP_ + 1.0);
      yLabelWidth_ += fm.width(QString().sprintf(", %.16g +", yTicsBias_));
    }
    else
    {
      yTicsBias_ = 0.0;
      isYTicsBiased_ = false;
    };
    // define the origin:
    dy = yTicsStep_*floor(maxY_/yTicsStep_);
    if (!isYTicsBiased_) //?
      while (dy > minY_) // down from the origin:
      {
        yTicsWidth_ = std::max(yTicsWidth_, 
                  fm.width(QString().sprintf("%.8g", isYTicsBiased_ ? dy-yTicsBias_ : dy)));
        dy -= yTicsStep_;
      };
  };
  // end of Y-Tics calculations
  
  xFrameBegin_ = yTicsWidth_ + 5*yLabelHeight_/2 + 10;
  xFrameEnd_   = width_ - rightMargin_;

  // Define X Tics parameters:
  if (isXTicsMJD_)
  {
    numOfXTics_ = (xFrameEnd_ - xFrameBegin_)/(7*xTicsWidth_/4);
    xTicsStep_ = 86400.0*(maxX_ - minX_)/numOfXTics_; // here xTicsStep_ in sec
    if (xTicsStep_ <= 60.0) // sec.s
    {
      xStepP_ = (int)floor(log10(xTicsStep_));
      xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) )*exp10(xStepP_);
      xTicsMJD_ = 0;
    }
    else if (xTicsStep_/60.0 <= 45.0) // min.s
    {
      xTicsStep_ /= 60.0;
      xStepP_ = (int)floor(log10(xTicsStep_));
      xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) )*exp10(xStepP_);
      if (10.0>xTicsStep_ && xTicsStep_>=8.0)
        xTicsStep_ = 10.0;
      else if (8.0>xTicsStep_ && xTicsStep_>=5.0)
        xTicsStep_ = 5.0;
      else if (5.0>xTicsStep_ && xTicsStep_>=2.0)
        xTicsStep_ = 2.0;
      xTicsMJD_ = 1;
    }
    else if (xTicsStep_/3600.0 <= 18.0) // hrs
    {
      xTicsStep_ /= 3600.0;
      xStepP_ = (int)floor(log10(xTicsStep_));
      xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) )*exp10(xStepP_);
      if (xTicsStep_ >= 10.0) 
        xTicsStep_ = 12.0;
      else if (xTicsStep_ >= 8.0) 
        xTicsStep_ = 8.0;
      else if (xTicsStep_ >= 6.0) 
        xTicsStep_ = 6.0;
      else if (xTicsStep_ >= 4.0) 
        xTicsStep_ = 4.0;
      if (xTicsStep_ < 1.0) 
        xTicsStep_ = 1.0;
      xTicsMJD_ = 2;
    }
    else if (xTicsStep_/86400.0 <= 20.0) // days
    {
      xTicsStep_ /= 86400.0;
      if (xTicsStep_ < 1.0) 
        xTicsStep_ = 0.5;
      xStepP_ = (int)floor(log10(xTicsStep_));
      xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) )*exp10(xStepP_);
      xTicsMJD_ = 3; //days
    }
    else
    {
      numOfXTics_ = (xFrameEnd_ - xFrameBegin_)/(2*xTicsWidth_);
      xTicsStep_ = 86400.0*(maxX_ - minX_)/numOfXTics_;

      xTicsStep_ /= 86400.0;
      xStepP_ = (int)floor(log10(xTicsStep_));
      xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) )*exp10(xStepP_);
      xTicsMJD_ = 4; //days and more
    };
  }
  else
  {
    numOfXTics_ = (xFrameEnd_ - xFrameBegin_)/(2*xTicsWidth_);
    xTicsStep_ = (maxX_ - minX_)/numOfXTics_;
    xStepP_ = (int)floor(log10(xTicsStep_));
    xTicsStep_ = trunc( xTicsStep_/exp10(xStepP_) );
    if (xTicsStep_ == 3)
      xTicsStep_ = 2;
    else if (xTicsStep_ == 4 || xTicsStep_ == 6 || xTicsStep_ == 7)
      xTicsStep_ = 5;
    else if (xTicsStep_==8 || xTicsStep_==9)
      xTicsStep_ = 10;
    xTicsStep_ = xTicsStep_*exp10(xStepP_);
    if (!isXTicsMJD_ && log10(fabs(minX_)) - xStepP_ > 7.0) // need a bias
    {
      isXTicsBiased_ = true;
      xTicsBias_ = trunc( minX_/exp10(xStepP_ + 1.0) )*exp10(xStepP_ + 1.0);
      xLabelWidth_ += fm.width(QString().sprintf(", %.16g +", xTicsBias_));
    }
    else
    {
      xTicsBias_ = 0.0;
      isXTicsBiased_ = false;
    };
  };
  // end of XTics calculations
  xDataBegin_ = xFrameBegin_ + 20;
  xDataEnd_   = xFrameEnd_   - 20;
  xMargins_   = xDataBegin_ + width_ - xDataEnd_;

  yDataBegin_ = yFrameBegin_ + 20;
  yDataEnd_   = yFrameEnd_   - 20;
  yMargins_   = yDataBegin_ + height_ - yDataEnd_;
};



//
void SgPlotArea::resizeEvent(QResizeEvent* ev)
{
  height_ = ev->size().height();
  width_  = ev->size().width();
  QWidget::resizeEvent(ev);
};



//
void SgPlotArea::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  QRect rect = e->rect();
  drawWholePlot(&painter, rect);
};



//
void SgPlotArea::drawWholePlot(QPainter* painter, const QRect& rect)
{
//  painter->setRenderHint(QPainter::Antialiasing);
//  painter->setRenderHint(QPainter::TextAntialiasing);
//  painter->setRenderHint(QPainter::HighQualityAntialiasing);

  calcLimits();
  defineAreas(painter);
  calcTransforms();
  drawFrames(painter);
  drawYTics(painter);

  if (isXTicsMJD_)
    drawXmjdTics(painter);
  else
    drawXTics(painter);
  
  //painter->setRenderHint(QPainter::Antialiasing);
  drawData(painter, rect);

  if (userMode_==UserMode_INQUIRING)
    drawPointInfo(painter);
  else if (userMode_==UserMode_MEASURING)
    drawRuler(painter);
  else if (userMode_==UserMode_RERANGING)
    drawRangeSelector(painter);
  else if (userMode_==UserMode_SELECTING || userMode_==UserMode_DESELECTING)
    drawPointSelector(painter);
};



//
void SgPlotArea::drawPointInfo(QPainter* painter)
{
  if (rulerToPoint_.x()>=0 && rulerToPoint_.y()>=0)
  {
    painter->setPen(*rulerPen_);
    painter->drawEllipse(rulerFromPoint_, 2, 2);
    painter->drawEllipse(rulerToPoint_, 2, 2);

    double x   = reverseCalcX(rulerToPoint_.x());
    double y   = reverseCalcY(rulerToPoint_.y());

    QString xInfo, yInfo;
    QFontMetrics  fm(painter->fontMetrics());

    if (isXTicsMJD_)
    {
      xInfo = "t: " + SgMJD(x).toString(SgMJD::F_YYYYMMDDHHMMSSSS);
      yInfo.sprintf("f: %.4g", y);
    }
    else
    {
      xInfo.sprintf("x: %.4g", x);
      yInfo.sprintf("y: %.4g", y);
    };

    int biasX = 5, biasY = 5;
    int widthDX = fm.width(xInfo);
    int widthDY = fm.width(yInfo);
    int rectHeight = 2*fm.height() + 3*biasY;
    int rectWidth = std::max(widthDX, widthDY) + 2*biasX;

    QRect infoRect(rulerToPoint_.x()+10, rulerToPoint_.y()-10 - rectHeight, rectWidth, rectHeight);

    painter->setPen(*framePen_);
    painter->fillRect(infoRect, *rulerBrush_);
    painter->setBrush(Qt::NoBrush);
      
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + biasY + fm.height(), xInfo);
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 2*(biasY + fm.height()), yInfo);
  };
};



//
void SgPlotArea::drawRuler(QPainter* painter)
{
  if (rulerFromPoint_.x()>=0 && rulerFromPoint_.y()>=0)
  {
    painter->setPen(*rulerPen_);
    painter->drawEllipse(rulerFromPoint_, 2, 2);
    painter->drawEllipse(rulerToPoint_, 2, 2);
    painter->drawLine(rulerFromPoint_, rulerToPoint_);
      
    double dx = reverseCalcX(rulerToPoint_.x()) - reverseCalcX(rulerFromPoint_.x());
    double dy = reverseCalcY(rulerToPoint_.y()) - reverseCalcY(rulerFromPoint_.y());
    QString dXInfo, dYInfo;
    QFontMetrics  fm(painter->fontMetrics());

    if (isXTicsMJD_)
    {
      dXInfo = "dt: " + interval2Str(dx);
      dYInfo.sprintf("df: %.4g", dy);
    }
    else
    {
      dXInfo.sprintf("dx: %.4g", dx);
      dYInfo.sprintf("dy: %.4g", dy);
    };

    int biasX = 5, biasY = 5;
    int widthDX = fm.width(dXInfo);
    int widthDY = fm.width(dYInfo);
    int rectHeight = 2*fm.height() + 3*biasY;
    int rectWidth = std::max(widthDX, widthDY) + 2*biasX;

    QRect infoRect(rulerToPoint_.x()+10, rulerToPoint_.y()-10 - rectHeight, rectWidth, rectHeight);

    painter->setPen(*framePen_);
    painter->fillRect(infoRect, *rulerBrush_);
    painter->setBrush(Qt::NoBrush);
      
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + biasY + fm.height(), dXInfo);
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 2*(biasY + fm.height()), dYInfo);
  };
};



//
void SgPlotArea::drawRangeSelector(QPainter* painter)
{
  if (rulerFromPoint_.x()>=0 && rulerFromPoint_.y()>=0)
  {
    painter->setPen(*rulerPen_);
    QRect rect(rulerFromPoint_, (rulerToPoint_ -QPoint(1,1)));
    painter->drawRect(rect);
      
    double x_to   = reverseCalcX(rulerToPoint_.x());
    double x_from = reverseCalcX(rulerFromPoint_.x());
    double y_to   = reverseCalcY(rulerToPoint_.y());
    double y_from = reverseCalcY(rulerFromPoint_.y());
    double dx = x_to - x_from;
    double dy = y_to - y_from;

    QString xRangesInfo, yRangesInfo;
    QString dXInfo, dYInfo;
    QFontMetrics  fm(painter->fontMetrics());

    if (isXTicsMJD_)
    {
      xRangesInfo = "t: from <" + SgMJD(x_from).toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
                    "> to <" + SgMJD(x_to).toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ">";
      yRangesInfo.sprintf("f: from: <%.4g> to <%4.g>", y_from, y_to);
      dXInfo = "dt: " + interval2Str(dx);
      dYInfo.sprintf("df: %.4g", dy);
    }
    else
    {
      xRangesInfo.sprintf("f: from: <%.4g> to <%4.g>", x_from, x_to);
      yRangesInfo.sprintf("f: from: <%.4g> to <%4.g>", y_from, y_to);
      dXInfo.sprintf("dx: %.4g", dx);
      dYInfo.sprintf("dy: %.4g", dy);
    };

    int biasX = 5, biasY = 5;
    int rectWidth = std::max(fm.width(xRangesInfo), fm.width(yRangesInfo));
    rectWidth = std::max(rectWidth, fm.width(dXInfo));
    rectWidth = std::max(rectWidth, fm.width(dYInfo));
    rectWidth += 2*biasX;
    int rectHeight = 5*fm.height() + 6*biasY;

    QRect infoRect(rulerToPoint_.x()+10, rulerToPoint_.y()-10 - rectHeight, rectWidth, rectHeight);

    painter->setPen(*framePen_);
    painter->fillRect(infoRect, *rulerBrush_);
    painter->setBrush(Qt::NoBrush);
      
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + biasY + fm.height(), "New ranges:");
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 2*(biasY + fm.height()), xRangesInfo);
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 3*(biasY + fm.height()), yRangesInfo);
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 4*(biasY + fm.height()), dXInfo);
    painter->drawText(rulerToPoint_.x()+10 + biasX, 
                      rulerToPoint_.y()-10 - rectHeight + 5*(biasY + fm.height()), dYInfo);    
  };
};



//
void SgPlotArea::drawPointSelector(QPainter* painter)
{
  if (rulerFromPoint_.x()>=0 && rulerFromPoint_.y()>=0)
  {
    painter->setPen(*rulerPen_);
//  painter->setBrush(QBrush(QColor(200, 180, 50), Qt::Dense6Pattern));
    painter->setBrush(QBrush(QColor(144, 238, 144, 100)));
    QRect rect(rulerFromPoint_, (rulerToPoint_ - QPoint(1,1)));
//    painter->drawEllipse(rect);
    painter->drawRect(rect);
    painter->setBrush(Qt::NoBrush);
  };
};



//
void SgPlotArea::drawFrames(QPainter* painter)
{
  // Title:
  SgPlotBranch                 *branch=NULL;
  int                           numOfVisibleBranches=0;
  for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
    if (plotCarrier_->listOfBranches()->at(i)->getIsBrowsable())
    {
      numOfVisibleBranches++;
      branch = plotCarrier_->listOfBranches()->at(i);
    };
  QFontMetrics                  fm(painter->fontMetrics());
  QString                       str(plotCarrier_->getName(yColumn_));
  if (numOfVisibleBranches==1)
    str = branch->getAlternativeTitleName(yColumn_);
  titleWidth_ = fm.width(str);
  painter->drawText((xFrameEnd_ + xFrameBegin_ - titleWidth_)/2, labelsHeight_, str);
  //
  // Labels:
  // X Label:
  if (isXTicsMJD_)
    painter->drawText((xFrameBegin_+xFrameEnd_-xLabelWidth_)/2, height_-labelsHeight_/2, 
                      (xTicsMJD_<=2 && maxX_-minX_<1.0) ?
                      (*xLabel_+"; "+SgMJD((int)trunc(minX_),0.0).toString(SgMJD::F_Date)) :
                       *xLabel_);
  else
    painter->drawText((xFrameBegin_+xFrameEnd_-xLabelWidth_)/2, height_-labelsHeight_/2, 
                      isXTicsBiased_ ? (*xLabel_+QString().sprintf(", %.16g +",xTicsBias_)) :
                      *xLabel_);
  // Y Label:
  painter->save();
  painter->rotate(270);
  painter->drawText(-(yFrameEnd_+yFrameBegin_+yLabelWidth_)/2, 3*labelsHeight_/2, 
                  isYTicsBiased_ ? (*yLabel_+QString().sprintf(", %.16g +",yTicsBias_)) : *yLabel_);
  painter->restore();

  // frames:
  painter->setPen(*framePen_);
  painter->drawLine(xFrameBegin_, yFrameEnd_,   xFrameEnd_,   yFrameEnd_);
  painter->drawLine(xFrameEnd_,   yFrameEnd_,   xFrameEnd_,   yFrameBegin_);
  painter->drawLine(xFrameEnd_,   yFrameBegin_, xFrameBegin_, yFrameBegin_);
  painter->drawLine(xFrameBegin_, yFrameBegin_, xFrameBegin_, yFrameEnd_);
};



//
void SgPlotArea::drawYTics(QPainter* painter)
{
  QString       Str;
  int           ticAlignFlag = (yStepP_<0) ? Qt::AlignLeft : Qt::AlignRight;
  double        dy = 0.0;
  int           yTic = 0, i;
  //if (MaxY*MinY<0.0) // there is "0":
  if ((maxY_+yTicsStep_/3.0)*(minY_-yTicsStep_/3.0) < 0.0) // there is "0":
  {
    painter->setPen(*zeroPen_);
    dy = 0.0;
    yTic = calcY(dy);
    painter->drawLine(xFrameBegin_+1, yTic, xFrameEnd_-1, yTic);
    painter->setPen(*framePen_);
    while (dy <= maxY_)
    {
      if ((yTic=calcY(dy))>yFrameBegin_+3 && yTic<yFrameEnd_-3)
      {
        Str.sprintf("%.8g", dy);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(xFrameBegin_, yTic, xFrameEnd_, yTic);
        painter->setPen(*framePen_);
        painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_-10,   yTic);
        painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+10, yTic);
        painter->drawText(xFrameBegin_-yTicsWidth_-10, yTic-labelsHeight_/2, 
                          yTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      };
      for (i=0; i<9; i++)
        if ((yTic = calcY(dy + yTicsStep_*(i+1)/10.0))>yFrameBegin_+3 && yTic<yFrameEnd_-3)
        {
          painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_  - ((i!=4) ? 3 : 6), yTic);
          painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+ ((i!=4) ? 4 : 7), yTic);
        };
      dy += yTicsStep_;
    };
    for (i=0; i<9; i++)
      if ((yTic = calcY(-yTicsStep_*(i+1)/10.0))<yFrameEnd_-3 && yTic>yFrameBegin_+3)
      {
        painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_  - ((i!=4) ? 3 : 6), yTic);
        painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+ ((i!=4) ? 4 : 7), yTic);
      };
    dy =- yTicsStep_;
    while (dy >= minY_)
    {
      if ((yTic=calcY(dy))<yFrameEnd_-3 && yTic>yFrameBegin_+3)
      {
        Str.sprintf("%.8g", dy);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(xFrameBegin_, yTic, xFrameEnd_, yTic);
        painter->setPen(*framePen_);
        painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_-10,   yTic);
        painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+10, yTic);
        painter->drawText(xFrameBegin_-yTicsWidth_-10, yTic-labelsHeight_/2, 
                          yTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      };
      for (i=0; i<9; i++)
        if ((yTic = calcY(dy - yTicsStep_*(i+1)/10.0))<yFrameEnd_-3 && yTic>yFrameBegin_+3)
        {
          painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_  - ((i!=4) ? 3 : 6), yTic);
          painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+ ((i!=4) ? 4 : 7), yTic);
        };
      dy -= yTicsStep_;
    };
  }
  else
  {
    // define the origin:
    double y_0 = yTicsStep_*floor((yFrameBegin_+3-f_Ay_)/f_By_/yTicsStep_);
    dy = y_0;
    while (dy > (yFrameEnd_-3-f_Ay_)/f_By_) // down from the origin:
    {
      yTic = calcY(dy);
      Str.sprintf("%.8g", isYTicsBiased_ ? dy-yTicsBias_ : dy);
      painter->setPen(*ticLinesPen_);
      painter->drawLine(xFrameBegin_, yTic, xFrameEnd_, yTic);
      painter->setPen(*framePen_);
      painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_-10,   yTic);
      painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+10, yTic);
      painter->drawText(xFrameBegin_-yTicsWidth_-10, yTic-labelsHeight_/2, 
                        yTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<9; i++)
        if ((yTic = calcY(dy - yTicsStep_*(i+1)/10.0)) < yFrameEnd_-3)
        {
          painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_  - ((i!=4) ? 3 : 6), yTic);
          painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+ ((i!=4) ? 4 : 7), yTic);
        };
      dy -= yTicsStep_;
    };
    for (i=0; i<9; i++) // draw upward from the origin:
      if ((yTic = calcY(y_0 + yTicsStep_*(i+1)/10.0)) > yFrameBegin_+3)
      {
        painter->drawLine(xFrameEnd_,   yTic, xFrameEnd_  - ((i!=4) ? 3 : 6), yTic);
        painter->drawLine(xFrameBegin_, yTic, xFrameBegin_+ ((i!=4) ? 4 : 7), yTic);
      };
  };
};



//
void SgPlotArea::drawXTics(QPainter* painter)
{
  QString       Str;
  double        dx = 0.0;
  int           xTic = 0, i;
  int           ticAlignFlag = Qt::AlignCenter;

  if (maxX_*minX_<0.0) // there is "0":
  {
    dx = 0.0;
    painter->setPen(*zeroPen_);
    xTic = calcX(dx);
    painter->drawLine(xTic, yFrameBegin_+1, xTic, yFrameEnd_-1);
    painter->setPen(*framePen_);
    while (dx <= maxX_)
    {
      xTic = calcX(dx);
      Str.sprintf("%.8g", dx);
      painter->setPen(*ticLinesPen_);
      painter->drawLine(xTic, yFrameBegin_,   xTic, yFrameEnd_);
      painter->setPen(*framePen_);
      painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  -10);
      painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+10);
      painter->drawText(xTic-xTicsWidth_/2, yFrameEnd_+labelsHeight_/2, 
                            xTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<9; i++)
        if ((xTic=calcX(dx + xTicsStep_*(i+1)/10.0)) < xFrameEnd_-3)
        {
          painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  - ((i!=4) ? 3 : 6));
          painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+ ((i!=4) ? 4 : 7));
        };
      dx += xTicsStep_;
    };
    for (i=0; i<9; i++)
      if ((xTic=calcX(-xTicsStep_*(i+1)/10.0)) > xFrameBegin_+3)
      {
            painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  - ((i!=4) ? 3 : 6));
            painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+ ((i!=4) ? 4 : 7));
      };
    dx =- xTicsStep_;
    while (dx >= minX_)
    {
      xTic = calcX(dx);
      Str.sprintf("%.8g", dx);
      painter->setPen(*ticLinesPen_);
      painter->drawLine(xTic, yFrameBegin_,   xTic, yFrameEnd_);
      painter->setPen(*framePen_);
      painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  -10);
      painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+10);
      painter->drawText(xTic-xTicsWidth_/2, yFrameEnd_+labelsHeight_/2, 
                        xTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<9; i++)
        if ((xTic=calcX(dx-xTicsStep_*(i+1)/10.0)) > xFrameBegin_+3)
        {
          painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  - ((i!=4) ? 3 : 6));
          painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+ ((i!=4) ? 4 : 7));
        };
      dx -= xTicsStep_;
    };
  }
  else
  {
    // define the origin:
    double x_0=xTicsStep_*floor((xFrameEnd_-3-f_Ax_)/f_Bx_/xTicsStep_);
    dx = x_0;
    while (dx > (xFrameBegin_-f_Ax_)/f_Bx_) // down from the origin:
    {
      xTic = calcX(dx);
      Str.sprintf("%.8g", isXTicsBiased_? dx-xTicsBias_ : dx);
      painter->setPen(*ticLinesPen_);
      painter->drawLine(xTic, yFrameBegin_,   xTic, yFrameEnd_);
      painter->setPen(*framePen_);
      painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  -10);
      painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+10);
      painter->drawText(xTic-xTicsWidth_/2, yFrameEnd_+labelsHeight_/2, 
                        xTicsWidth_, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<9; i++)
        if ((xTic=calcX(dx-xTicsStep_*(i+1)/10.0)) > xFrameBegin_+3)
        {
          painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  - ((i!=4) ? 3 : 6));
          painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+ ((i!=4) ? 4 : 7));
        };
      dx -= xTicsStep_;
    };
    for (i=0; i<9; i++) // draw upward from the origin:
      if ((xTic=calcX(x_0+xTicsStep_*(i+1)/10.0)) < xFrameEnd_-3)
      {
        painter->drawLine(xTic, yFrameEnd_,   xTic, yFrameEnd_  - ((i!=4) ? 3 : 6));
        painter->drawLine(xTic, yFrameBegin_, xTic, yFrameBegin_+ ((i!=4) ? 4 : 7));
      };
  };
};



//
void SgPlotArea::drawXmjdTics(QPainter* painter)
{
  QString       Str;
  SgMJD         dt;
  SgMJD         D0((int)trunc(minX_)+1, 0.0); // here is the next midnight of the epoch of the MinX
  QFontMetrics  fm(painter->fontMetrics());
  double        tmp;
  int           tTic=0, i;
  int           ss=2, tw=0;
  int           ticAlignFlag=Qt::AlignCenter;
  int           n4SubTic;
  
  switch (xTicsMJD_)
  {
  case 0: // XTicsStep in seconds:
    D0 = SgMJD((int)trunc(minX_), 0.0);
    D0 += xTicsStep_/86400.0*
              trunc((SgMJD((xFrameBegin_ + 3 - f_Ax_)/f_Bx_) - D0)/(xTicsStep_/86400.0));
    dt = D0;
    ss = 5;
    n4SubTic = 20;
    while (dt.toDouble() < (xFrameEnd_-3-f_Ax_)/f_Bx_) // upward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if ( modf(dt.toDouble(), &tmp)*86400.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_Time);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };

      if (tTic > xFrameBegin_+3)
      {
        tw = fm.width(Str);
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
        painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                                  tw, labelsHeight_, ticAlignFlag, Str);
      };
      for (i=0; i<n4SubTic-1; i++)
        if ((tTic = calcX(dt.toDouble() + xTicsStep_/86400.0*(i+1)/n4SubTic))<xFrameEnd_-3 && 
            tTic>xFrameBegin_+3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt += xTicsStep_/86400.0;
    };
    for (i=0; i<n4SubTic-1; i++)
      if ((tTic = calcX(D0.toDouble() - xTicsStep_/86400.0*(i+1)/n4SubTic))>xFrameBegin_+3  && 
            tTic>xFrameBegin_+3)
      {
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
      };
    dt = D0 - xTicsStep_/86400.0;
    while (dt.toDouble() > (xFrameBegin_+3-f_Ax_)/f_Bx_) // downward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if ( modf(dt.toDouble(), &tmp)*86400.0 < 0.3)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_Time);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, tw, labelsHeight_, 
                            ticAlignFlag, Str);
      for (i=0; i<n4SubTic-1; i++)
        if ((tTic = calcX(dt.toDouble() - xTicsStep_/86400.0*(i+1)/n4SubTic))>xFrameBegin_+3 && 
              tTic>xFrameBegin_+3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt -= xTicsStep_/86400.0;
    };
    break;
      
  case 1: // XTicsStep in minutes:
    D0 = SgMJD((int)trunc(minX_), 0.0);
    D0+= xTicsStep_/1440.0*trunc((SgMJD((xFrameBegin_ + 3 - f_Ax_)/f_Bx_) - D0)/(xTicsStep_/1440.0));

    dt = D0;
    ss = 1;
    if (xTicsStep_ >= 50) 
    {
      ss = 5;
      n4SubTic = 10;
    }
    else if (xTicsStep_ >= 30)
    {
      ss = 10;
      n4SubTic = 20;
    }
    else if (xTicsStep_ == 20) 
    {
      ss = 5;
      n4SubTic = 20;
    }
    else if (xTicsStep_ == 10) 
    {
      ss = 5;
      n4SubTic = 10;
    }
    else if (xTicsStep_ >= 8) 
    {
      ss = 2;
      n4SubTic = (int)(ss*xTicsStep_);
    }
    else
    {
      ss = 5;
      n4SubTic = (int)(ss*xTicsStep_);
    };

    while (dt.toDouble() < (xFrameEnd_-3-f_Ax_)/f_Bx_) // upward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if (modf(dt.toDouble(), &tmp)*1440.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };

      if (tTic > xFrameBegin_+3)
      {
        tw = fm.width(Str);
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
        painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                                  tw, labelsHeight_, ticAlignFlag, Str);
      };
      for (i=0; i<n4SubTic-1; i++)
        if ((tTic = calcX(dt.toDouble() + xTicsStep_/1440.0*(i+1)/n4SubTic))<xFrameEnd_-3 && 
              tTic>xFrameBegin_+3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt += xTicsStep_/1440.0;
    };
    for (i=0; i<n4SubTic-1; i++)
      if ((tTic = calcX(D0.toDouble() - xTicsStep_/1440.0*(i+1)/n4SubTic))>xFrameBegin_+3  && 
            tTic>xFrameBegin_+3)
      {
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
      };
    dt = D0 - xTicsStep_/1440.0;
    while (dt.toDouble() > (xFrameBegin_+3-f_Ax_)/f_Bx_) // downward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if ( modf(dt.toDouble(), &tmp)*1440.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                          tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<n4SubTic-1; i++)
        if ((tTic = calcX(dt.toDouble() - xTicsStep_/1440.0*(i+1)/n4SubTic))>xFrameBegin_+3 && 
              tTic>xFrameBegin_+3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt -= xTicsStep_/1440.0;
    };
    break;
      
  case 2: // XTicsStep in hours:
    dt = D0;
    ss = 2;
    if (xTicsStep_ < 4) 
      ss = 3;
    if (xTicsStep_ < 3) 
      ss = 4;
    if (xTicsStep_ < 2) 
      ss = 6;
    while (dt.toDouble() < (xFrameEnd_-3-f_Ax_)/f_Bx_) // upward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if ( modf(dt.toDouble(), &tmp)*24.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                            tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<ss*xTicsStep_-1; i++)
        if ((tTic=calcX(dt.toDouble()+xTicsStep_/24.0*(i+1)/(ss*xTicsStep_))) < xFrameEnd_-3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt += xTicsStep_/24.0;
    };
    for (i=0; i<ss*xTicsStep_-1; i++)
      if ((tTic=calcX(D0.toDouble()-xTicsStep_/24.0*(i+1)/(ss*xTicsStep_)))>xFrameBegin_+3  && 
            tTic<xFrameEnd_-3)
      {
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
      };
    dt = D0 - xTicsStep_/24.0;
    while (dt.toDouble() > (xFrameBegin_+3-f_Ax_)/f_Bx_) // downward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if ( modf(dt.toDouble(), &tmp)*24.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };

      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                          tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<ss*xTicsStep_-1; i++)
        if ((tTic=calcX(dt.toDouble()-xTicsStep_/24.0*(i+1)/(ss*xTicsStep_)))>xFrameBegin_+3 && 
              tTic<xFrameEnd_-3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt -= xTicsStep_/24.0;
    };
    break;

  case 3: // XTicsStep in days:
    dt = D0;
    ss = 6;
    if (xTicsStep_ < 1.0) 
      ss = 24;
    else if (xTicsStep_ < 2) 
      ss = 12;
    else if (xTicsStep_ < 4) 
      ss = 6;
    else if (xTicsStep_ < 6) 
      ss = 4;
    else if (xTicsStep_ < 12) 
      ss = 2;
    else 
      ss = 1;
    while (dt.toDouble() < (xFrameEnd_-3-f_Ax_)/f_Bx_) // upward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if (modf(dt.toDouble(), &tmp)*24.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                          tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<ss*xTicsStep_-1; i++)
        if ((tTic=calcX(dt.toDouble()+xTicsStep_*(i+1)/(ss*xTicsStep_))) < xFrameEnd_-3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt += xTicsStep_;
    };
    for (i=0; i<ss*xTicsStep_-1; i++)
      if ((tTic=calcX(D0.toDouble()-xTicsStep_*(i+1)/(ss*xTicsStep_))) > xFrameBegin_+3)
      {
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
      };
    dt = D0 - xTicsStep_;
    while (dt.toDouble() > (xFrameBegin_+3-f_Ax_)/f_Bx_) // downward from the origin:
    {
      tTic = calcX(dt.toDouble());
      if (modf(dt.toDouble(), &tmp)*24.0 < 0.1)
      {
        Str = SgMJD(tmp).toString(SgMJD::F_Date);
        painter->setPen(*zeroPen_);
        painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      }
      else
      {
        Str = SgMJD(dt).toString(SgMJD::F_TimeShort);
        painter->setPen(*ticLinesPen_);
        painter->drawLine(tTic, yFrameBegin_+1,   tTic, yFrameEnd_-1);
        painter->setPen(*framePen_);
      };
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                                  tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<ss*xTicsStep_-1; i++)
        if ((tTic=calcX(dt.toDouble()-xTicsStep_*(i+1)/(ss*xTicsStep_))) > xFrameBegin_+3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%ss)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%ss)?4:7));
        };
      dt -= xTicsStep_;
    };
    break;

  case 4: // XTicsStep in days:
    dt = D0;
    while (dt.toDouble() < (xFrameEnd_-3-f_Ax_)/f_Bx_) // upward from the origin:
    {
      tTic = calcX(dt.toDouble());
      Str = SgMJD(dt).toString(SgMJD::F_Date);
      painter->setPen(*zeroPen_);
      painter->drawLine(tTic, yFrameBegin_+1, tTic, yFrameEnd_-1);
      painter->setPen(*framePen_);
          
      tw = fm.width(Str);
      painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  -10);
      painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+10);
      painter->drawText(tTic-tw/2, yFrameEnd_+labelsHeight_/2, 
                                  tw, labelsHeight_, ticAlignFlag, Str);
      for (i=0; i<9; i++)
        if ((tTic = calcX(dt.toDouble() + xTicsStep_*(i+1)/(10)))<xFrameEnd_-3)
        {
          painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%5)?3:6));
          painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%5)?4:7));
        };
      dt += xTicsStep_;
    };
    for (i=0; i<9; i++)
      if ((tTic = calcX(D0.toDouble() - xTicsStep_*(i+1)/10))>xFrameBegin_+3)
      {
        painter->drawLine(tTic, yFrameEnd_,   tTic, yFrameEnd_  - (((i+1)%5)?3:6));
        painter->drawLine(tTic, yFrameBegin_, tTic, yFrameBegin_+ (((i+1)%5)?4:7));
      };
    break;
  }; // end of cases

};



//
void SgPlotArea::drawData(QPainter* painter, const QRect& rect)
{
  if (!plotCarrier_->isOK())
    return;

  int               i, ei;
  int               dr = 0;

  int limitLeft  = std::max(rect.left() - visibleWidth_/2, xFrameBegin_);
  int limitRight = std::min(rect.right() + visibleWidth_/2, xFrameEnd_);
  int limitTop   = std::max(rect.top() - visibleHeight_/2, yFrameBegin_);
  int limitBottom= std::min(rect.bottom() + visibleHeight_/2, yFrameEnd_);
  
  // selection/deselection stuff:
  QPoint            pointLT;                                      // left top
  QPoint            pointRB;                                      // right bottom
  pointLT.setX(std::min(rulerFromPoint_.x(), rulerToPoint_.x()));
  pointLT.setY(std::min(rulerFromPoint_.y(), rulerToPoint_.y()));
  pointRB.setX(std::max(rulerFromPoint_.x(), rulerToPoint_.x()));
  pointRB.setY(std::max(rulerFromPoint_.y(), rulerToPoint_.y()));
//  QRegion selectionRegion(QRect(pointLT, pointRB-QPoint(1,1)), QRegion::Ellipse);
  QRegion selectionRegion(QRect(pointLT, pointRB-QPoint(1,1)));
  bool              selectionRegionIsAPoint = rulerFromPoint_==rulerToPoint_;
 
  for (i=0; i<plotCarrier_->listOfBranches()->size(); i++)
  {
    SgPlotBranch *branch = plotCarrier_->listOfBranches()->at(i);
    if (branch->getIsBrowsable())
    {
      painter->setPen(*(branchPens_+i));
      painter->setBrush(*(branchBrushes_+i));
      bool isFirstPointDrawn = false;
      int x=0, y=0;
      int prevX=0, prevY=0;
      QPoint point;
      for (unsigned int j=0; j<branch->data()->nRow(); j++)
        if (branch->isPointVisible(j, sets2plot_))
        {
          int y_u=0;
          int y_d=0;
          prevX = x;
          prevY = y;
          x = calcX(branch->data()->getElement(j, xColumn_));
          y = calcY(branch->data()->getElement(j, yColumn_));
          point.setX(x);
          point.setY(y);
//          if (xFrameBegin_<=x && x<=xFrameEnd_ && yFrameBegin_<=y && y<=yFrameEnd_)
          if (limitLeft<=x && x<=limitRight && limitTop<=y && y<=limitBottom)
          {
            QRect rect(x-radius_, y-radius_, 2*radius_, 2*radius_);
            if (selectionRegionIsAPoint) // special case:
            {
              selectionRegion.setRects(&rect, 1);
              if (selectionRegion.contains(rulerToPoint_))
                branch->xorDataAttr(j, SgPlotCarrier::DA_SELECTED);
            }
            else
            {
              if (userMode_ == UserMode_SELECTING && selectionRegion.contains(rect))
                branch->addDataAttr(j, SgPlotCarrier::DA_SELECTED);
              else if (userMode_ == UserMode_DESELECTING && selectionRegion.contains(rect))
                branch->delDataAttr(j, SgPlotCarrier::DA_SELECTED);
            };
            
            // drawing data with primitives:
            // lines:
            if (isPlotLines_)
            {
              if (!isFirstPointDrawn)
                isFirstPointDrawn = true;
              else
                painter->drawLine(prevX, prevY, x, y);
            };
            // points:
            if (isPlotPoints_)
            {
              dr = 0;
              if ((branch->getDataAttr(j) & SgPlotCarrier::DA_SELECTED))
              {
//                painter->setBrush(Qt::NoBrush);
//                painter->setPen(*framePen_);
                dr = ddr_;
                painter->setBrush(*(branchSelectedBrushes_+i));
                painter->setPen(*(branchSelectedPens_+i));
              }
              else if ( (branch->getDataAttr(j) & SgPlotCarrier::DA_REJECTED) ||
                        (branch->getDataAttr(j) & SgPlotCarrier::DA_NONUSABLE) )
              {
                painter->setBrush(*ignoredBrush_);
                painter->setPen(*ignoredPen_);
              }
              else // regular points:
              {
                painter->setBrush(*(branchBrushes_+i));
                painter->setPen(*framePen_);
              };
              // paint:
              if (branch->getDataAttr(j) & SgPlotCarrier::DA_NONUSABLE)
                painter->drawRect(x-radius_-dr, y-radius_-dr, 2*(radius_+dr), 2*(radius_+dr));
              else
              {
                if (branch->getDataAttr(j) & SgPlotCarrier::DA_SPECIAL_01)
                {
                  painter->drawLine(x-2*(dr+2), y-2*(dr+2), x+2*(dr+2), y+2*(dr+2));
                  painter->drawLine(x-2*(dr+2), y+2*(dr+2), x+2*(dr+2), y-2*(dr+2));
                  painter->drawLine(x-3*(dr+2), y,          x+3*(dr+2), y);
                };
                painter->drawEllipse(QPoint(x,y), radius_+dr,radius_+dr);
              };
              painter->setPen(*(branchPens_+i));
            };
            // error bars (if available):
            if (isPlotErrBars_ && (ei=plotCarrier_->getStdVarIdx(yColumn_))!=-1)
            {
              y_u = std::max( calcY(branch->data()->getElement(j, yColumn_) + 
                                    branch->data()->getElement(j, ei)), yFrameBegin_);
              y_d = std::min( calcY(branch->data()->getElement(j, yColumn_) - 
                                    branch->data()->getElement(j, ei)), yFrameEnd_);
              painter->drawLine(x, y_u, x, y_d);
            };
            // impulses:
            if (isPlotImpulses_)
            {
              if (branch->data()->getElement(j, yColumn_) > 0.0)
              {
                y_u = calcY(branch->data()->getElement(j, yColumn_));
                y_d = std::min(calcY(0.0), yDataEnd_);
              }
              else
              {
                y_u = std::max(calcY(0.0), yDataBegin_);
                y_d = calcY(branch->data()->getElement(j, yColumn_));
              };
              painter->drawLine(x, y_u, x, y_d);
            };
          };
          
          // draw bars:
          if (branch->getDataAttr(j) & SgPlotCarrier::DA_BAR && 
                yFrameBegin_<=y && y<=yFrameEnd_)
          {
            painter->setPen(*barPen_);
            painter->drawLine(x, yDataBegin_, x, yDataEnd_);
            painter->setPen(*(branchPens_+i));
          };
        };
    };
  };
  painter->setBrush(Qt::NoBrush);
};



//
void SgPlotArea::queryPoint(const QPoint& queryPoint, SgPlotBranch*& queryBranch, int &queryIdx)
{
  double            d, minD;
  double            leftLimit, rightLimit, topLimit, bottomLimit;
  double            queryX, queryY;
  double            x, y;

  queryX = queryPoint.x();
  queryY = queryPoint.y();
  
  // a window for lookup:
  leftLimit   = queryPoint.x() - 5*radius_;
  rightLimit  = queryPoint.x() + 5*radius_;
  topLimit    = queryPoint.y() - 5*radius_;
  bottomLimit = queryPoint.y() + 5*radius_;

  // initial values:
  minD = sqrt((rightLimit-leftLimit)*(rightLimit-leftLimit) + 
              (bottomLimit-topLimit)*(bottomLimit-topLimit));
  queryBranch = NULL;
  queryIdx = -1;

  if (!plotCarrier_->isOK())
    return;
  
//for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
  for (int i=plotCarrier_->listOfBranches()->size()-1; 0<=i; i--) // search from the back
  {
    SgPlotBranch *branch = plotCarrier_->listOfBranches()->at(i);
    if (branch->getIsBrowsable())
      for (unsigned int j=0; j<branch->data()->nRow(); j++)
        if (branch->isPointVisible(j, sets2plot_))
        {
          x = calcX(branch->data()->getElement(j, xColumn_));
          y = calcY(branch->data()->getElement(j, yColumn_));
          if (leftLimit<=x && x<=rightLimit && topLimit<=y && y<=bottomLimit)
          {
            d = sqrt( (x-queryX)*(x-queryX) + (y-queryY)*(y-queryY) );
            if (minD > d)
            {
              queryBranch = branch;
              queryIdx = j;
              minD = d;
            };
          };
        };
  };
  if (queryBranch)
    logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
      ": query successful for [" + queryBranch->getName() + "] branch, point# " + 
      QString().setNum(queryIdx));
};



//
void SgPlotArea::output4Files(const QString& path)
{
  bool isOutputDirExists(false);
  if (!path.isEmpty())
  {
    bool                        isOk(true);
    QDir                        d(path);
    if (!d.exists())
//    isOk = d.mkpath("./"); // Qt, wtf?
      isOk = d.mkpath(d.absolutePath());
    else
      isOutputDirExists = true;
    if (!isOk)
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::GUI, className() + 
      ": cannot create directory " + path);
    else
      isOutputDirExists = true;
  };
  
  QString         fileName = plotCarrier_->getFile2SaveBaseName()
    + QString("").sprintf("[%u:%u]", xColumn_, yColumn_);
  
  if (isOutputDirExists)
    fileName = path + "/" + fileName;
  
  QString         buff;
  QFile           f;
  QTextStream     s;
  int             ei;
  for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
  {
    SgPlotBranch *branch = plotCarrier_->listOfBranches()->at(i);
    if (branch->getIsBrowsable())
    {
      f.setFileName(fileName + QString("").sprintf("_%03d.dat", i));
      if (f.open(QIODevice::WriteOnly))
      {      
        s.setDevice(&f);
        s << "# " << branch->getName() << "\n";
        s << "# X: [" << qPrintable(*xLabel_) << "]\n";
        s << "# Y: [" << qPrintable(*yLabel_) << "]\n";

        for (unsigned int j=0; j<branch->data()->nRow(); j++)
          if (branch->isPointVisible(j, sets2plot_))
          {
            s << QString().sprintf("%24.12f  %24.12f ", 
              branch->data()->getElement(j, xColumn_), branch->data()->getElement(j, yColumn_));
            if ((ei=plotCarrier_->getStdVarIdx(yColumn_)) != -1)
              s << QString().sprintf(" %24.12f ", branch->data()->getElement(j, ei));
            if (isXTicsMJD_)
              s << "     "
                << SgMJD(branch->data()->getElement(j, xColumn_)).toString(SgMJD::F_YYYYMMDDHHMMSSSS);
            s << "\n";
          };
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + " output4Files(): the data '" +
          plotCarrier_->getName(yColumn_) + "' has been saved to the file " + f.fileName());
        f.close();
        s.setDevice(NULL);
      }
      else    
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        ": output4Files(): cannot open the file " + f.fileName() + " for writing");
    };
  };
};



void SgPlotArea::output4Print(QPainter *prnPainter, int width, int height, int radius, int ddr, 
  int fontSize)
{
  int                           widthSaved(width_);
  int                           heightSaved(height_);
  int                           radiusSaved(radius_);
  int                           ddrSaved(ddr_);
  QFont                         font=QApplication::font();

//font.setPointSize(12);
  font.setPointSize(fontSize);
  prnPainter->setFont(font);

  width_ = width;
  height_ = height;
  
  radius_ = radius;
  ddr_ = ddr;
  drawWholePlot(prnPainter, QRect(0, 0, width_, height_));

  width_ = widthSaved;
  height_ = heightSaved;
  radius_ = radiusSaved;
  ddr_ = ddrSaved;
};
/*=====================================================================================================*/





/*=======================================================================================================
*
*                           SgPlot:
* 
*======================================================================================================*/
const double SgPlot::scaleFactor_ = M_2_SQRTPI;  // it's a "right" scale factor: 2/sqrt(pi)
SgPlot::SgPlot(SgPlotCarrier* plotCarrier, const QString& pth2outpt,
  QWidget *parent, unsigned int modes)
  : QWidget(parent),
    path2Outputs_(pth2outpt),
    filterActions_(),
    filterAuxActions_(),
    filterExtActions_(),
    extKeys_()
{
  lvExtKeys_ = NULL;

  scaleX_ = 1.0;
  scaleY_ = 1.0;
  
  outputFormat_ = OF_PDF;
  
  // limits for zoomin and zoomout; could be changed later
  maxZoomX_ = 40.0;
  minZoomX_ = 0.5;
  maxZoomY_ = 40.0;
  minZoomY_ = 0.5;
 
  modes_  = modes;
 
  // user actions:
  //  isScrolling_ = false;
  
  plotCarrier_ = plotCarrier;
  if (!plotCarrier_->selfCheck())
    logger->write(SgLogger::ERR, SgLogger::GUI, className() + 
      "::selfCheck() for [" + plotCarrier_->getName() + "] failed");


  if (modes_ & PM_EXT_KEY_SELECT)
  {
    extKeys_.clear();
    QMap<QString, QString>      keyByKey;
    for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
    {
      SgPlotBranch             *pb=plotCarrier_->listOfBranches()->at(i);
      if (pb->hasExtKeys())
        for (uint j=0; j<pb->numOfRows(); j++)
          if (!keyByKey.contains(pb->getExtKey(j)))
            keyByKey.insert(pb->getExtKey(j), pb->getExtKey(j));
    };
    extKeys_ = keyByKey.keys();
    keyByKey.clear();
  };


  
  area_ = new SgPlotArea(plotCarrier_, this);
  area_->setBackgroundRole(QPalette::Base);
  area_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  plotScroller_ = new SgPlotScroller(this);
  plotScroller_->setBackgroundRole(QPalette::Dark);
  plotScroller_->setWidget(area_);
  plotScroller_->setAlignment(Qt::AlignCenter);
  plotScroller_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  plotScroller_->setLineWidth(2);

  // interaction with a user:
  connect(plotScroller_, SIGNAL(mouseWheelRotated(QWheelEvent*)), 
          SLOT(processWheelEvent(QWheelEvent*)));
  connect(plotScroller_, SIGNAL(mousePressed(QMouseEvent*)), 
          SLOT(processMousePressEvent(QMouseEvent*)));
  connect(plotScroller_, SIGNAL(mouseMoved(QMouseEvent*)), 
          SLOT(processMouseMoveEvent(QMouseEvent*)));
  connect(plotScroller_, SIGNAL(mouseReleased(QMouseEvent*)), 
          SLOT(processMouseReleaseEvent(QMouseEvent*)));
  connect(plotScroller_, SIGNAL(mouseDoubleClicked(QMouseEvent*)), 
          SLOT(processMouseDoubleClickEvent(QMouseEvent*)));
/*
  connect(plotScroller_, SIGNAL(keyPressed(QKeyEvent*)),
          SLOT(processKeyPressEvent(QKeyEvent*)));
  connect(plotScroller_, SIGNAL(keyReleased(QKeyEvent*)), 
          SLOT(processKeyReleaseEvent(QKeyEvent*)));
*/
  zoomInAction_ = new QAction(tr("Zoom &In"), this);
  zoomInAction_->setShortcut(tr("Ctrl++"));
  connect(zoomInAction_, SIGNAL(triggered()), SLOT(zoomIn()));
  addAction(zoomInAction_);

  zoomOutAction_ = new QAction(tr("Zoom &Out"), this);
  zoomOutAction_->setShortcut(tr("Ctrl+-"));
  connect(zoomOutAction_, SIGNAL(triggered()), this, SLOT(zoomOut()));
  addAction(zoomOutAction_);
  
  zoomXInAction_ = new QAction(tr("Zoom In X-axis"), this);
  zoomXInAction_->setShortcut(Qt::CTRL + Qt::Key_Right);
  connect(zoomXInAction_, SIGNAL(triggered()), this, SLOT(zoomXIn()));
  addAction(zoomXInAction_);

  zoomYInAction_ = new QAction(tr("Zoom In Y-axis"), this);
  zoomYInAction_->setShortcut(Qt::CTRL + Qt::Key_Up);
  connect(zoomYInAction_, SIGNAL(triggered()), this, SLOT(zoomYIn()));
  addAction(zoomYInAction_);

  zoomXOutAction_ = new QAction(tr("Zoom Out X-axis"), this);
  zoomXOutAction_->setShortcut(Qt::CTRL + Qt::Key_Left);
  connect(zoomXOutAction_, SIGNAL(triggered()), this, SLOT(zoomXOut()));
  addAction(zoomXOutAction_);

  zoomYOutAction_ = new QAction(tr("Zoom Out Y-axis"), this);
  zoomYOutAction_->setShortcut(Qt::CTRL + Qt::Key_Down);
  connect(zoomYOutAction_, SIGNAL(triggered()), this, SLOT(zoomYOut()));
  addAction(zoomYOutAction_);
  
  normalSizeAction_= new QAction(tr("Set normal size"), this);
  normalSizeAction_->setShortcut(tr("Ctrl+*"));
  connect(normalSizeAction_, SIGNAL(triggered()), this, SLOT(zoomNormalView()));
  addAction(normalSizeAction_);

  if (modes_ & PM_HAS_HAVE_ZERO)
    area_->setHave2HasZero(true);

  QBoxLayout *mainLayout = new QHBoxLayout(this);
  
  if (modes_ & PM_EXT_KEY_SELECT)
  {
    QStringListModel           *model=new QStringListModel();
    QStringList                 list;
    lvExtKeys_ = new QListView(this);
    for (int i=0; i<extKeys_.size(); i++)
      list << extKeys_.at(i);
    model->setStringList(list);
    lvExtKeys_->setModel(model);

    lvExtKeys_->setSelectionMode(QAbstractItemView::MultiSelection);
    lvExtKeys_->setEditTriggers (QAbstractItemView::NoEditTriggers);
    lvExtKeys_->setUniformItemSizes(true);
    lvExtKeys_->selectAll();
    isExtKeyViewInSpecialMode_ = false;

    connect(lvExtKeys_->selectionModel(), 
      SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)), SLOT(extKeyChanged()));
      
    //lvExtKeys_->setMinimumWidth(24 + lvExtKeys_->sizeHintForColumn(0));
    //lvExtKeys_->setMaximumHeight(9*lvExtKeys_->sizeHintForRow(0));

    QHBoxLayout *layoutH = new QHBoxLayout;
    layoutH->setSpacing(0);
    QPushButton *allPlus = new QPushButton("*+", this);
    QPushButton *prevKey = new QPushButton("<<", this);
    QPushButton *nextKey = new QPushButton(">>", this);
    QPushButton *allMinus= new QPushButton("-*", this);
    QSize        btnSize = allPlus->fontMetrics().size(Qt::TextSingleLine, ">>");

    //btnSize += QSize(12,8);
    btnSize += QSize(8,4);
    allPlus ->setFixedSize(btnSize);
    allMinus->setFixedSize(btnSize);
    prevKey ->setFixedSize(btnSize);
    nextKey ->setFixedSize(btnSize);

    layoutH->addWidget(allMinus);
    layoutH->addStretch(1);
    layoutH->addWidget(prevKey);
    layoutH->addWidget(nextKey);
    layoutH->addStretch(1);
    layoutH->addWidget(allPlus);

    connect(allPlus,  SIGNAL(clicked()), SLOT(markAllExtKeysAsSelected()));
    connect(allMinus, SIGNAL(clicked()), SLOT(markAllExtKeysAsDeselected()));
    connect(prevKey,  SIGNAL(clicked()), SLOT(markPrevExtKey()));
    connect(nextKey,  SIGNAL(clicked()), SLOT(markNextExtKey()));
    QVBoxLayout *layoutV = new QVBoxLayout;
    layoutV->setSpacing(0);
    layoutV->addLayout(layoutH);
    layoutV->addWidget(lvExtKeys_, 1);
    mainLayout->addLayout(layoutV, 0);
  };
  //
  QBoxLayout *layout = new QVBoxLayout;
  mainLayout->addLayout(layout, 5);

  layout->addWidget(plotScroller_, 5);
  layout->addWidget(control());
  mainLayout->activate();
  setLayout(mainLayout);
 
  currentKeyModifier_ = 0;
};



//
SgPlot::~SgPlot()
{
  if (zoomOutAction_)
  {
    delete zoomOutAction_;
    zoomOutAction_ = NULL;
  };

  if (zoomInAction_)
  {
    delete zoomInAction_;
    zoomInAction_ = NULL;
  };

  if (area_)
  {
    delete area_;
    area_ = NULL;
  };
  filterActions_.clear();
  filterAuxActions_.clear();
  filterExtActions_.clear();
  extKeys_.clear();
};



//
QWidget* SgPlot::control()
{
  QGridLayout                  *grid;
  QLabel                       *label;
  QGroupBox                    *gBox;
  QBoxLayout                   *layout, *aLayout;
  QPushButton                  *button;
  QSlider                      *slider;
  unsigned int                  i;

  controls_ = new QFrame(this);
  layout = new QHBoxLayout(controls_);

  // axis selection:
  if (!(modes_ & PM_WO_AXIS_NAMES))
  {
    aLayout = new QVBoxLayout;
    layout->addLayout(aLayout);
    
    gBox = new QGroupBox("Axis to plot", controls_);
    grid = new QGridLayout(gBox);
      
    label  = new QLabel("X:", gBox);
    label -> setMinimumSize(label->sizeHint());
    grid  -> addWidget(label, 0,0,  Qt::AlignLeft | Qt::AlignVCenter);
      
    label  = new QLabel("Y:", gBox);
    label -> setMinimumSize(label->sizeHint());
    grid  -> addWidget(label, 1,0,  Qt::AlignLeft | Qt::AlignVCenter);
      
    cbXAxis_ = new QComboBox(gBox);
    cbXAxis_->setInsertPolicy(QComboBox::InsertAtBottom);
    cbYAxis_ = new QComboBox(gBox);
    cbYAxis_->setInsertPolicy(QComboBox::InsertAtBottom);
  
    for (i=0; i<(unsigned int)(plotCarrier_->numOfColumns()-1); i++)
    {
      cbXAxis_->insertItem(i, *plotCarrier_->columnNames()->at(i));
      cbYAxis_->insertItem(i, *plotCarrier_->columnNames()->at(i));
    };
    // set defaults:
    cbXAxis_->setCurrentIndex(0);
    cbYAxis_->setCurrentIndex(plotCarrier_->numOfColumns()>1?1:0);
    connect(cbXAxis_, SIGNAL(highlighted(int)),         SLOT(changeXaxisTemp(int)));
    connect(cbXAxis_, SIGNAL(currentIndexChanged(int)), SLOT(changeXaxis(int)));
    connect(cbYAxis_, SIGNAL(highlighted(int)),         SLOT(changeYaxisTemp(int)));
    connect(cbYAxis_, SIGNAL(currentIndexChanged(int)), SLOT(changeYaxis(int)));

    cbXAxis_->setMinimumSize(cbXAxis_->sizeHint());
    grid->addWidget(cbXAxis_, 0,1);
    cbYAxis_->setMinimumSize(cbYAxis_->sizeHint());
    grid->addWidget(cbYAxis_, 1,1);
      
    gBox->setLayout(grid);
    aLayout->addWidget(gBox);
//  layout->addWidget(gBox);

    //--
    const char*                 sets2plotNames[3] = {"A", "U", "G"};
    QRadioButton               *rbButton[3];
    gBox = new QGroupBox("Data to plot", controls_);
    QBoxLayout                 *bLayout=new QHBoxLayout(gBox);
    bgSets2plot_ = new QButtonGroup(gBox);
    for (int i=0; i<3; i++)
    {
      rbButton[i] = new QRadioButton(sets2plotNames[i], gBox);
      rbButton[i]-> setMinimumSize(rbButton[i]->sizeHint());
      bgSets2plot_->addButton(rbButton[i], i);
      bLayout->addWidget(rbButton[i]);
    };
    rbButton[area_->getSets2plot()]->setChecked(true);
    connect(bgSets2plot_, SIGNAL(buttonClicked(int)), SLOT(modifySets2plot(int)));
    aLayout->addWidget(gBox);
    //--
  };
  //
  // branches:
  if (!(modes_ & PM_WO_BRANCH_NAMES))
  {
    QStringListModel *model = new QStringListModel();
    QStringList list;
    lvBranches_ = new QListView(controls_);
    for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
      list << plotCarrier_->listOfBranches()->at(i)->getName();
    model -> setStringList(list);
    lvBranches_->setModel(model);
      
    lvBranches_->setSelectionMode(QAbstractItemView::MultiSelection);
    lvBranches_->setEditTriggers (QAbstractItemView::NoEditTriggers);
    lvBranches_->setUniformItemSizes(true);
    lvBranches_->selectAll();
    isBranchViewInSpecialMode_ = false;

    connect(lvBranches_->selectionModel(), 
      SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)), SLOT(branchChanged()));
      
    lvBranches_->setMinimumWidth(24 + lvBranches_->sizeHintForColumn(0));
    lvBranches_->setMaximumHeight(9*lvBranches_->sizeHintForRow(0));

    QHBoxLayout *layoutH = new QHBoxLayout;
    layoutH->setSpacing(0);
    QPushButton *allPlus = new QPushButton("*+", this);
    QPushButton *prevBrn = new QPushButton("<<", this);
    QPushButton *nextBrn = new QPushButton(">>", this);
    QPushButton *allMinus= new QPushButton("-*", this);
    QSize        btnSize = allPlus->fontMetrics().size(Qt::TextSingleLine, ">>");

    //btnSize+=QSize(12,8);
    btnSize+=QSize(8,4);
    allPlus ->setFixedSize(btnSize);
    allMinus->setFixedSize(btnSize);
    prevBrn ->setFixedSize(btnSize);
    nextBrn ->setFixedSize(btnSize);

    QPushButton                *filterMinus=NULL, *filterPlus=NULL;
    if (modes_ & PM_FILTERS_ENABLED)
    {
      filterMinus = new QPushButton("-F", this);
      filterPlus  = new QPushButton("F+", this);
      filterMinus->setFixedHeight (btnSize.height());
      filterMinus->setMinimumWidth(btnSize.width());
      filterPlus ->setFixedHeight (btnSize.height());
      filterPlus ->setMinimumWidth(btnSize.width());
      filterPlus ->setContextMenuPolicy(Qt::CustomContextMenu);
      filterMinus->setContextMenuPolicy(Qt::CustomContextMenu);
    };
    layoutH->addWidget(allMinus);
    if (modes_ & PM_FILTERS_ENABLED)
      layoutH->addWidget(filterMinus, 1);
    else
      layoutH->addStretch(1);
    layoutH->addWidget(prevBrn);
    layoutH->addWidget(nextBrn);
    if (modes_ & PM_FILTERS_ENABLED)
      layoutH->addWidget(filterPlus, 1);
    else
      layoutH->addStretch(1);
    layoutH->addWidget(allPlus);

    connect(allPlus,  SIGNAL(clicked()), SLOT(markAllBranchesAsSelected()));
    connect(allMinus, SIGNAL(clicked()), SLOT(markAllBranchesAsDeselected()));
    connect(prevBrn,  SIGNAL(clicked()), SLOT(markPrevBranch()));
    connect(nextBrn,  SIGNAL(clicked()), SLOT(markNextBranch()));
    if (modes_ & PM_FILTERS_ENABLED)
    {
      connect(filterMinus,  SIGNAL(clicked()), SLOT(processFilterMinus()));
      connect(filterPlus,   SIGNAL(clicked()), SLOT(processFilterPlus ()));
      connect(filterMinus,  SIGNAL(customContextMenuRequested(const QPoint&)),
                            SLOT  (processFilterAuxMinus(const QPoint&)));
      connect(filterPlus,   SIGNAL(customContextMenuRequested(const QPoint&)),
                            SLOT  (processFilterAuxPlus (const QPoint&)));
    };
    QVBoxLayout *layoutV = new QVBoxLayout;
    layoutV->setSpacing(0);
    layoutV->addLayout(layoutH);
    layoutV->addWidget(lvBranches_);
    layout->addLayout(layoutV, 5);
  };
  //
  // draw modes:
  gBox = new QGroupBox("Plot data as:", controls_);
  aLayout = new QVBoxLayout(gBox);
  
  aLayout->addStretch(1);
  cbPoints_ = new QCheckBox("Points", gBox);
  cbPoints_->setMinimumSize(cbPoints_->sizeHint());
  // defaults:
  cbPoints_->setChecked(true);
  connect(cbPoints_, SIGNAL(toggled(bool)), SLOT(dmPointsChanged(bool)));
  if (modes_ & PM_WO_DOTS)
    cbPoints_->setChecked(false);
  aLayout->addWidget(cbPoints_);

  cbLines_ = new QCheckBox("Lines", gBox);
  cbLines_->setMinimumSize(cbLines_->sizeHint());
  connect(cbLines_, SIGNAL(toggled(bool)), SLOT(dmLinesChanged(bool)));
  if (modes_ & PM_LINES)
    cbLines_->setChecked(true);
  aLayout->addWidget(cbLines_);

  cbErrBars_ = new QCheckBox("Error bars", gBox);
  cbErrBars_->setMinimumSize(cbErrBars_->sizeHint());
  connect(cbErrBars_, SIGNAL(toggled(bool)), SLOT(dmErrBarsChanged(bool)));
  if (modes_ & PM_ERRBARS)
    cbErrBars_->setChecked(true);
  aLayout->addWidget(cbErrBars_);

  cbImpulses_ = new QCheckBox("Impulses", gBox);
  cbImpulses_->setMinimumSize(cbImpulses_->sizeHint());
  connect(cbImpulses_, SIGNAL(toggled(bool)), SLOT(dmImpulsesChanged(bool)));
  aLayout->addWidget(cbImpulses_);
  if (modes_ & PM_IMPULSE)
    cbImpulses_->setChecked(true);
  aLayout->addStretch(1);

  gBox->setLayout(aLayout);
  layout->addWidget(gBox);
  //

  // colors:
  gBox = new QGroupBox("Colors (H,S,V)", controls_);
  grid = new QGridLayout(gBox);

  slider = new QSlider(Qt::Vertical, gBox);
  slider->setTickPosition(QSlider::TicksLeft);
//  slider->setTickPosition(QSlider::NoTicks);
  slider->setRange(0, 360);
  slider->setPageStep(30);
  slider->setValue(0);
  slider->setMinimumSize(slider->sizeHint());
  connect(slider, SIGNAL(valueChanged(int)), SLOT(colorHChanged(int)));
  grid->addWidget(slider, 1,0);

  slider = new QSlider(Qt::Vertical, gBox);
//  slider->setTickPosition(QSlider::TicksBothSides);
  slider->setTickPosition(QSlider::NoTicks);
  slider->setRange(0, 255);
  slider->setPageStep(24);
  slider->setValue(255);
  slider->setMinimumSize(slider->sizeHint());
  connect(slider, SIGNAL(valueChanged(int)), SLOT(colorSChanged(int)));
  grid->addWidget(slider, 1,1);

  slider = new QSlider(Qt::Vertical, gBox);
  slider->setTickPosition(QSlider::TicksRight);
//  slider->setTickPosition(QSlider::NoTicks);
  slider->setRange(0, 255);
  slider->setPageStep(50);
  slider->setValue(200);
  slider->setMinimumSize(slider->sizeHint());
  connect(slider, SIGNAL(valueChanged(int)), SLOT(colorVChanged(int)));
  grid->addWidget(slider, 1,2);

//  grid->activate();
  gBox->setLayout(grid);
  layout->addWidget(gBox);
  //

  // options:
  gBox = new QGroupBox("Plot Ranges", controls_);
  aLayout = new QVBoxLayout(gBox);
  
  aLayout->addStretch(1);
  cbUserDefined_ = new QCheckBox("User's", gBox);
  cbUserDefined_->setMinimumSize(cbUserDefined_->sizeHint());
  cbUserDefined_->setEnabled(false);
  connect(cbUserDefined_, SIGNAL(toggled(bool)), SLOT(oUserDefinedChanged(bool)));
  aLayout->addWidget(cbUserDefined_);

  cbRangeVisible_ = new QCheckBox("Visible only", gBox);
  cbRangeVisible_->setMinimumSize(cbRangeVisible_->sizeHint());
  cbRangeVisible_->setChecked(false);
  connect(cbRangeVisible_, SIGNAL(toggled(bool)), SLOT(oVisRang(bool)));
  aLayout->addWidget(cbRangeVisible_);

  cbWStdVar_ = new QCheckBox("w/ StdVar", gBox);
  cbWStdVar_->setMinimumSize(cbWStdVar_->sizeHint());
  cbWStdVar_->setChecked(true);
  connect(cbWStdVar_, SIGNAL(toggled(bool)), SLOT(oWStdVar(bool)));
  aLayout->addWidget(cbWStdVar_);

  cbSymmetrical_ = new QCheckBox("Symmetrical", gBox);
  cbSymmetrical_->setMinimumSize(cbSymmetrical_->sizeHint());
  cbSymmetrical_->setChecked(false);
  connect(cbSymmetrical_, SIGNAL(toggled(bool)), SLOT(setRangeSymmetrical(bool)));
  aLayout->addWidget(cbSymmetrical_);

  //--
  const char*                   rangeLimitsNames[3] = {"A", "U", "G"};
  QRadioButton                 *rbButton[3];
  QBoxLayout                   *bLayout=new QHBoxLayout;
  aLayout->addLayout(bLayout);
  bgRangeLimits_ = new QButtonGroup(gBox);
  for (int i=0; i<3; i++)
  {
    rbButton[i] = new QRadioButton(rangeLimitsNames[i], gBox);
    rbButton[i]-> setMinimumSize(rbButton[i]->sizeHint());
    bgRangeLimits_->addButton(rbButton[i], i);
    bLayout->addWidget(rbButton[i]);
  };
  rbButton[area_->getRangeLimits()]->setChecked(true);
  connect(bgRangeLimits_, SIGNAL(buttonClicked(int)), SLOT(modifyRangeLimits(int)));
  //--

  aLayout->addStretch(1);

  gBox->setLayout(aLayout);
  layout->addWidget(gBox);

  //..
  // plot size:
  layout->addStretch(1);
  grid = new QGridLayout();
  layout->addLayout(grid, 0);

  button = new QPushButton(">file", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(save2File()));
  grid->addWidget(button, 0,0);

  button = pbZommXOut_ = new QPushButton("Z'X-", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomXOut()));
  grid->addWidget(button, 1,0);

  button = pbZommOut_ = new QPushButton("Z'-", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  button->addAction(zoomOutAction_);
  connect (button, SIGNAL(clicked()), SLOT(zoomOut()));
  grid->addWidget(button, 2,0);

  button = pbZommYIn_ = new QPushButton("Z'Y+", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomYIn()));
  grid->addWidget(button, 0,1);

  button = new QPushButton("O", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomNormalView()));
  grid->addWidget(button, 1,1);

  button = pbZommYOut_ = new QPushButton("Z'Y-", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomYOut()));
  grid->addWidget(button, 2,1);

  button = pbZommIn_ = new QPushButton("Z'+", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  button->addAction(zoomInAction_);
  connect (button, SIGNAL(clicked()), SLOT(zoomIn()));
  grid->addWidget(button, 0,2);

  button = pbZommXIn_ = new QPushButton("Z'X+", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomXIn()));
  grid->addWidget(button, 1,2);

  button = new QPushButton(">Img", controls_);
  button->setMinimumSize(2*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(save2Image()));
  grid->addWidget(button, 2,2);
  //
//  layout->activate();

  return controls_;
};






//
void SgPlot::markAllBranchesAsSelected()
{
  lvBranches_->selectAll();
};



//
void SgPlot::markAllBranchesAsDeselected()
{
  lvBranches_->clearSelection();
};



//
void SgPlot::markAllExtKeysAsSelected()
{
  lvExtKeys_->selectAll();
};



//
void SgPlot::markAllExtKeysAsDeselected()
{
  lvExtKeys_->clearSelection();
};



//
void SgPlot::fillAxisNames()
{
  if (modes_ & PM_WO_AXIS_NAMES)
    return;

  int           currentX = cbXAxis_->currentIndex();
  int           currentY = cbYAxis_->currentIndex();
  cbXAxis_->clear();
  cbYAxis_->clear();
  if (plotCarrier_->numOfColumns())
  {
    QString   *s;
    for (int i=0; i<plotCarrier_->numOfColumns()-1; i++)
    {
      s = plotCarrier_->columnNames()->at(i);
      cbXAxis_->insertItem(i, *s);
      cbYAxis_->insertItem(i, *s);
    };
    if (cbXAxis_->count() > currentX)
      cbXAxis_->setCurrentIndex(currentX);
    else
      cbXAxis_->setCurrentIndex(0);
    if (cbYAxis_->count() > currentY)
      cbYAxis_->setCurrentIndex(currentY);
    else
      cbYAxis_->setCurrentIndex(plotCarrier_->numOfColumns()>=1?1:0);
  };
  cbXAxis_->setMinimumSize(cbXAxis_->sizeHint());
  cbYAxis_->setMinimumSize(cbYAxis_->sizeHint());
};



//
void SgPlot::fillBranchesNames()
{
  if (modes_ & PM_WO_BRANCH_NAMES)
    return;

  if (plotCarrier_->listOfBranches()->count())
  {
    QAbstractItemModel *oldModel = lvBranches_->model();
    QStringList list;
    for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
      list << plotCarrier_->listOfBranches()->at(i)->getName();
    lvBranches_->setModel(new QStringListModel(list));
    delete oldModel;
      
    lvBranches_->setSelectionMode(QAbstractItemView::MultiSelection);
//    lvBranches_->selectAll();
    lvBranches_->setMinimumWidth(20 + lvBranches_->sizeHintForColumn(0));
    lvBranches_->setMaximumHeight(6*lvBranches_->sizeHintForRow(0));

    connect(lvBranches_->selectionModel(), 
      SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)), SLOT(branchChanged()));
    lvBranches_->selectAll();
  };
};



//
void SgPlot::dataStructureChanged()
{
  area_->dataChanged();
  fillAxisNames(); 
  fillBranchesNames();
  area_->update();
};



//
void SgPlot::dataContentChanged()
{
  //  area_->dataChanged();
  //  fillAxisNames(); 
  // fillBranchesNames(); 
  area_->update();
};



//
void SgPlot::changeXaxis(int n)
{
  if (n>-1)
    area_->setXColumn(n);
  area_->update();
  emit xAxisChanged(n);
};



//
void SgPlot::changeXaxisTemp(int n)
{
  if (n>-1)
    area_->setXColumn(n);
  area_->update();
};



//
void SgPlot::changeXaxisTemp2(int n)
{
  cbYAxis_->setCurrentIndex(n);
  if (n>-1)
    area_->setXColumn(n);
  area_->update();
};



//
void SgPlot::changeYaxis(int n)
{
  if (n>-1)
    area_->setYColumn(n);
  area_->update();
  emit yAxisChanged(n);
};



//
void SgPlot::changeYaxisTemp(int n)
{
  if (n>-1)
    area_->setYColumn(n);
  area_->update();
};



//
void SgPlot::changeYaxisTemp2(int n)
{
  cbYAxis_->setCurrentIndex(n);
  if (n>-1)
    area_->setYColumn(n);
  area_->update();
};



//
void SgPlot::colorHChanged(int n)
{
  area_->setBPHuePhase(n);
};



//
void SgPlot::colorSChanged(int n)
{
  area_->setBPSaturation(n);
};



//
void SgPlot::colorVChanged(int n)
{
  area_->setBPValue(n);
};



//
void SgPlot::markPrevBranch()
{
  if (!isBranchViewInSpecialMode_) // adjust viewings:
  {
    lvBranches_->clearSelection();
    isBranchViewInSpecialMode_ = true;
  };
  QStringListModel *model = (QStringListModel*) lvBranches_->model();
  QModelIndex mIdx = lvBranches_->currentIndex();
  int row = mIdx.row();
  row--;
  if (row<0)
    row = model->rowCount() - 1;
  mIdx = model->index(row);
  lvBranches_->setCurrentIndex(mIdx);
};



//
void SgPlot::markNextBranch()
{
  if (!isBranchViewInSpecialMode_) // adjust viewings:
  {
    lvBranches_->clearSelection();
    isBranchViewInSpecialMode_ = true;
  };
  QStringListModel *model = (QStringListModel*) lvBranches_->model();
  QModelIndex mIdx = lvBranches_->currentIndex();
  int row = mIdx.row();
  row++;
  if (row > model->rowCount()-1)
    row = 0;
  mIdx = model->index(row);
  lvBranches_->setCurrentIndex(mIdx);
};



//
void SgPlot::markPrevExtKey()
{
  if (!isExtKeyViewInSpecialMode_) // adjust viewings:
  {
    lvExtKeys_->clearSelection();
    isExtKeyViewInSpecialMode_ = true;
  };
  QStringListModel *model = (QStringListModel*) lvExtKeys_->model();
  QModelIndex mIdx = lvExtKeys_->currentIndex();
  int row = mIdx.row();
  row--;
  if (row<0)
    row = model->rowCount() - 1;
  mIdx = model->index(row);
  lvExtKeys_->setCurrentIndex(mIdx);
};



//
void SgPlot::markNextExtKey()
{
  if (!isExtKeyViewInSpecialMode_) // adjust viewings:
  {
    lvExtKeys_->clearSelection();
    isExtKeyViewInSpecialMode_ = true;
  };
  QStringListModel *model = (QStringListModel*) lvExtKeys_->model();
  QModelIndex mIdx = lvExtKeys_->currentIndex();
  int row = mIdx.row();
  row++;
  if (row > model->rowCount()-1)
    row = 0;
  mIdx = model->index(row);
  lvExtKeys_->setCurrentIndex(mIdx);
};



//
void SgPlot::processFilterMinus()
{
  if (currentKeyModifier_ & Qt::ShiftModifier)
  {
    processFilterExtMinus();
    currentKeyModifier_ = 0;
    return;
  };
  //
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    QItemSelectionModel        *selectionModel=lvBranches_->selectionModel();
    QStringListModel           *model=(QStringListModel*)lvBranches_->model();
    for (int i=0; i<model->rowCount(); i++)
      if (plotCarrier_->listOfBranches()->at(i)->getName().contains(txt))
        selectionModel->select(model->index(i), QItemSelectionModel::Deselect);
  };
};



//
void SgPlot::processFilterPlus()
{
  if (currentKeyModifier_ & Qt::ShiftModifier)
  {
    processFilterExtPlus();
    currentKeyModifier_ = 0;
    return;
  };
  //
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    QItemSelectionModel        *selectionModel=lvBranches_->selectionModel();
    QStringListModel           *model=(QStringListModel*)lvBranches_->model();
    for (int i=0; i<model->rowCount(); i++)
      if (plotCarrier_->listOfBranches()->at(i)->getName().contains(txt))
        selectionModel->select(model->index(i), QItemSelectionModel::Select);
  };
};



//
void SgPlot::processFilterAuxMinus(const QPoint&)
{
  if (!filterAuxActions_.size())
    return;
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterAuxActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    QItemSelectionModel        *selectionModel=lvBranches_->selectionModel();
    QStringListModel           *model=(QStringListModel*)lvBranches_->model();
    for (int i=0; i<model->rowCount(); i++)
      if (plotCarrier_->listOfBranches()->at(i)->getName().contains(txt))
        selectionModel->select(model->index(i), QItemSelectionModel::Deselect);
  };
};



//
void SgPlot::processFilterAuxPlus(const QPoint&)
{
  if (!filterAuxActions_.size())
    return;
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterAuxActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    QItemSelectionModel        *selectionModel=lvBranches_->selectionModel();
    QStringListModel           *model=(QStringListModel*)lvBranches_->model();
    for (int i=0; i<model->rowCount(); i++)
      if (plotCarrier_->listOfBranches()->at(i)->getName().contains(txt))
        selectionModel->select(model->index(i), QItemSelectionModel::Select);
  };
};



//
void SgPlot::processFilterExtMinus()
{
  if (!filterExtActions_.size())
    return;
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterExtActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
    {
      SgPlotBranch             *branch=plotCarrier_->listOfBranches()->at(i);
      if (branch->hasExtKeys())
        for (unsigned int j=0; j<branch->numOfRows(); j++)
          if (branch->getExtKey(j)==txt &&
             (branch->getDataAttr(j) & SgPlotCarrier::DA_SELECTED))
            branch->delDataAttr(j, SgPlotCarrier::DA_SELECTED);
    };
  };
  area_->update();
};



//
void SgPlot::processFilterExtPlus()
{
  if (!filterExtActions_.size())
    return;
  QAction                      *ret=NULL;
  if ((ret=QMenu::exec(filterExtActions_, QCursor::pos())))
  {
    QString                     txt=ret->text();
    for (int i=0; i<plotCarrier_->listOfBranches()->size(); i++)
    {
      SgPlotBranch             *branch=plotCarrier_->listOfBranches()->at(i);
      if (branch->hasExtKeys())
        for (unsigned int j=0; j<branch->numOfRows(); j++)
          if (branch->getExtKey(j)==txt &&
            !(branch->getDataAttr(j) & SgPlotCarrier::DA_SELECTED))
            branch->addDataAttr(j, SgPlotCarrier::DA_SELECTED);
    };
  };
  area_->update();
};



//
void SgPlot::branchChanged()
{
  QItemSelectionModel *selectionModel = lvBranches_->selectionModel();
  QStringListModel *model = (QStringListModel*)lvBranches_->model();
  for (int i=0; i<model->rowCount(); i++)
  {
    if (selectionModel->isSelected(model->index(i)))
      plotCarrier_->listOfBranches()->at(i)->setIsBrowsable(true);
    else
      plotCarrier_->listOfBranches()->at(i)->setIsBrowsable(false);
  };
  area_->update();
  isBranchViewInSpecialMode_ = false;
};



//
void SgPlot::extKeyChanged()
{
  QItemSelectionModel          *selectionModel=lvExtKeys_->selectionModel();
  QStringListModel             *model=(QStringListModel*)lvExtKeys_->model();
  for (int i=0; i<model->rowCount(); i++)
  {
    QString str("");
    str = model->data(model->index(i), Qt::DisplayRole).toString();

    if (selectionModel->isSelected(model->index(i)))
    {
      for (int j=0; j<plotCarrier_->listOfBranches()->size(); j++)
        plotCarrier_->listOfBranches()->at(j)->flagExtKey(str, true);
      //std::cout << "  :" << qPrintable(str) << " is selected\n";
    }
//      plotCarrier_->listOfBranches()->at(i)->setIsBrowsable(true);
    else
    {
      for (int j=0; j<plotCarrier_->listOfBranches()->size(); j++)
        plotCarrier_->listOfBranches()->at(j)->flagExtKey(str, false);
      //std::cout << "  :" << qPrintable(str) << " is unselected\n";
    };
//      plotCarrier_->listOfBranches()->at(i)->setIsBrowsable(false);
  };
  area_->update();
  isExtKeyViewInSpecialMode_ = false;
};



//
void SgPlot::setFilterNames(const QList<QString>& l)
{
  if (filterActions_.size())
    filterActions_.clear();
  for (int i=0; i<l.size(); i++)
    filterActions_.append(new QAction(l.at(i), this));
};



//
void SgPlot::setFilterAuxNames(const QList<QString>& l)
{
  if (filterAuxActions_.size())
    filterAuxActions_.clear();
  for (int i=0; i<l.size(); i++)
    filterAuxActions_.append(new QAction(l.at(i), this));
};



//
void SgPlot::setFilterExtNames(const QList<QString>& l)
{
  if (filterExtActions_.size())
    filterExtActions_.clear();
  for (int i=0; i<l.size(); i++)
    filterExtActions_.append(new QAction(l.at(i), this));
};



//
void SgPlot::dmPointsChanged(bool Is)
{
  area_->setIsPlotPoints(Is);
  area_->update();
};



//
void SgPlot::dmLinesChanged(bool Is)
{
  area_->setIsPlotLines(Is);
  area_->update();
};



//
void SgPlot::dmErrBarsChanged(bool Is)
{
  area_->setIsPlotErrBars(Is);
  area_->update();
};



//
void SgPlot::dmImpulsesChanged(bool Is)
{
  area_->setIsPlotImpulses(Is);
  area_->update();
};



//
void SgPlot::oUserDefinedChanged(bool is)
{
  if (!is)
  {
    area_->unsetUserDefinedRanges();
    cbUserDefined_->setEnabled(false);
    for (int i=0; i<bgRangeLimits_->buttons().size(); i++)
      bgRangeLimits_->buttons().at(i)->setEnabled(true);
    cbRangeVisible_->setEnabled(true);
    cbWStdVar_->setEnabled(true);
    area_->update();
  };
};



//
void SgPlot::modifySets2plot(int idx)
{
  SgPlotArea::SetsOfData        sod;
  sod = (SgPlotArea::SetsOfData)idx;
  area_->setSets2plot(sod);
  area_->update();
};



//
void SgPlot::modifyRangeLimits(int idx)
{
//  int                           sets2plotIdx=bgSets2plot_->checkedId();
  SgPlotArea::SetsOfData        rl=(SgPlotArea::SetsOfData)idx;
  area_->setRangeLimits(rl);
  //
  if (true /*-1<sets2plotIdx && idx<sets2plotIdx*/)
  {
    bgSets2plot_->buttons().at(idx)->setChecked(true);
    modifySets2plot(idx);
  }
  else
    area_->update();
};



//
void SgPlot::oVisRang(bool is)
{
  area_->setIsLimitsOnVisible(is);
  area_->update();
};



//
void SgPlot::oWStdVar(bool is)
{
  area_->setIsStdVar(is);
  area_->update();
};



//
void SgPlot::setRangeSymmetrical(bool is)
{
  area_->setIsRangeSymmetrical(is);
  area_->update();
};



//
void SgPlot::zoomNormalView()
{
  scaleX_ = scaleY_ = 1.0;
  area_->resize(plotScroller_->maximumViewportSize());
  area_->setVisibleWidth(plotScroller_->maximumViewportSize().width());
  area_->setVisibleHeight(plotScroller_->maximumViewportSize().height());
  // enable actions:
  zoomInAction_->setEnabled(true);
  zoomOutAction_->setEnabled(true);
  // enable buttons:
  pbZommOut_->setEnabled(true);
  pbZommIn_->setEnabled(true);
  pbZommXOut_->setEnabled(true);
  pbZommXIn_->setEnabled(true);
  pbZommYOut_->setEnabled(true);
  pbZommYIn_->setEnabled(true);
};



//
void SgPlot::resizeEvent(QResizeEvent *ev)
{
  QSize size = plotScroller_->maximumViewportSize();
  area_->resize((int)(scaleX_*size.width()), (int)(scaleY_*size.height()));
  area_->setVisibleWidth(plotScroller_->maximumViewportSize().width());
  area_->setVisibleHeight(plotScroller_->maximumViewportSize().height());
  QWidget::resizeEvent(ev);
};



//
void SgPlot::processMousePressEvent(QMouseEvent* e)
{
  Qt::MouseButton button = e->button();
  Qt::KeyboardModifiers modifiers = e->modifiers();
  
  // here we process user input from the mouse:
  switch (button)
  {
  case Qt::LeftButton:      // left eye
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
      case Qt::ShiftModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT)
          startSelecting(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()), modifiers & Qt::ShiftModifier);
        break;
      case Qt::ControlModifier:
        break;
    };
    break;

  case Qt::MidButton:       // wheel pressed
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT)
          startScrollViewport(e->globalPos());
        break;
      case Qt::ControlModifier:
        break;
    };
    break;

  case Qt::RightButton:     // right eye
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT)
          startInquire(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()));
        break;
      case Qt::ShiftModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT)
          startMeasuring(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()));
        break;
      case Qt::ControlModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT)
          startReRanging(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()));
        break;
    };
    break;

  default:                  // something else:
    logger->write(SgLogger::DBG, SgLogger::GUI, className() +
      ": processMousePressEvent(): mouse buttons pressed: " +
      ((button & Qt::LeftButton)  ? "Left ": "") +
      ((button & Qt::RightButton) ? "Right ": "") +
      ((button & Qt::MidButton)   ? "Middle ": "") +
      ((button & Qt::XButton1)    ? "XButton1 ": "") +
      ((button & Qt::XButton2)    ? "XButton2": "")
      );
    break;
  };
  e->accept();
};



//
void SgPlot::processMouseMoveEvent(QMouseEvent* e)
{
  // here we process user input from the mouse:
  switch (area_->getUserMode())
  {
    case SgPlotArea::UserMode_SCROLLING:
      doScrollViewport(e->globalPos());
      break;

    case SgPlotArea::UserMode_INQUIRING:
      doInquire(e->pos() +
        QPoint (plotScroller_->horizontalScrollBar()->value(), 
                plotScroller_->verticalScrollBar()->value()));
      break;

    case SgPlotArea::UserMode_MEASURING:
      doMeasuring(e->pos() +
        QPoint (plotScroller_->horizontalScrollBar()->value(), 
                plotScroller_->verticalScrollBar()->value()));
      break;

    case SgPlotArea::UserMode_RERANGING:
      doReRanging(e->pos() +
        QPoint (plotScroller_->horizontalScrollBar()->value(), 
                plotScroller_->verticalScrollBar()->value()));
      break;

    case SgPlotArea::UserMode_SELECTING:
    case SgPlotArea::UserMode_DESELECTING:
      doSelecting(e->pos() +
        QPoint (plotScroller_->horizontalScrollBar()->value(), 
                plotScroller_->verticalScrollBar()->value()), e->modifiers() & Qt::ShiftModifier);
      break;

    default:
      break;
  };
  e->accept();
};



//
void SgPlot::processMouseReleaseEvent(QMouseEvent* e)
{
  Qt::MouseButton button = e->button();
  Qt::KeyboardModifiers modifiers = e->modifiers();
  // here we process user input from the mouse:
  switch (button)
  {
  case Qt::LeftButton:      // left eye
    if (area_->getUserMode() == SgPlotArea::UserMode_SELECTING || 
        area_->getUserMode() == SgPlotArea::UserMode_DESELECTING)
      stopSelecting(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(),
                    plotScroller_->verticalScrollBar()->value()), modifiers & Qt::ShiftModifier);
    break;

  case Qt::MidButton:       // wheel pressed
    if (area_->getUserMode() == SgPlotArea::UserMode_SCROLLING)
      stopScrollViewport();
    break;

  case Qt::RightButton:     // right eye
    if (area_->getUserMode() == SgPlotArea::UserMode_INQUIRING)
      stopInquire();
    if (area_->getUserMode() == SgPlotArea::UserMode_MEASURING)
      stopMeasuring();
    if (area_->getUserMode() == SgPlotArea::UserMode_RERANGING)
      stopReRanging(modifiers & Qt::ControlModifier);
    break;

  default:                  // something else:
    logger->write(SgLogger::DBG, SgLogger::GUI, className() +
      ": processMousePressEvent(): mouse buttons released: " +
      ((button & Qt::LeftButton)  ? "Left ": "") +
      ((button & Qt::RightButton) ? "Right ": "") +
      ((button & Qt::MidButton)   ? "Middle ": "") +
      ((button & Qt::XButton1)    ? "XButton1 ": "") +
      ((button & Qt::XButton2)    ? "XButton2": "")
      );
    break;
  };
  e->accept();
};



//
void SgPlot::processMouseDoubleClickEvent(QMouseEvent *e)
{
  Qt::MouseButton button = e->button();
  Qt::KeyboardModifiers modifiers = e->modifiers();
  
  // here we process user input from the mouse:
  switch (button)
  {
  case Qt::LeftButton:      // left eye
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT ||
            area_->getUserMode() == SgPlotArea::UserMode_SELECTING)
          queryData(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()), QM_Mode0);
        break;
      case Qt::ControlModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT ||
            area_->getUserMode() == SgPlotArea::UserMode_SELECTING)
          queryData(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()), QM_Mode1);
        break;
      case Qt::AltModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT ||
            area_->getUserMode() == SgPlotArea::UserMode_SELECTING)
          queryData(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()), QM_Mode2);
        break;
      case Qt::ShiftModifier:
        if (area_->getUserMode() == SgPlotArea::UserMode_DEFAULT ||
            area_->getUserMode() == SgPlotArea::UserMode_SELECTING)
          queryData(e->pos() + 
            QPoint (plotScroller_->horizontalScrollBar()->value(), 
                    plotScroller_->verticalScrollBar()->value()), QM_Mode3);
        break;
    };
    break;

  case Qt::MidButton:       // wheel pressed
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
        break;
      case Qt::ControlModifier:
        break;
    };
    break;

  case Qt::RightButton:     // right eye
    switch (modifiers)
    {
      default:
      case Qt::NoModifier:
        break;
      case Qt::ShiftModifier:
        break;
      case Qt::ControlModifier:
        break;
    };
    break;

  default:                  // something else:
    break;
  };
  e->accept();
};



//
void SgPlot::processWheelEvent(QWheelEvent *e)
{
  QPoint   p = e->pos();
  int  delta = e->delta();

  if (delta>0 && zoomInAction_->isEnabled())
    rescaleArea(scaleFactor_, scaleFactor_, p.x(), p.y());
  else if (delta<0  && zoomOutAction_->isEnabled())
    rescaleArea(1.0/scaleFactor_, 1.0/scaleFactor_, p.x(), p.y());
};



//
void SgPlot::processKeyPressEvent(QKeyEvent* e)
{
  Qt::KeyboardModifiers         modifiers=e->modifiers();
  int                           key=e->key();
  currentKeyModifier_ = modifiers;
  
  switch (key)
  {
  case Qt::Key_Control:
    break;
    
  default:
    //std::cout << "Press: key= " << key << ", modifiers=" << modifiers << "\n";
    break;
  };
  emit userPressedAKey(this, modifiers, key);
  e->accept();
};



//
void SgPlot::processKeyReleaseEvent(QKeyEvent* e)
{
  //Qt::KeyboardModifiers       modifiers=e->modifiers();
  int                           key=e->key();
  currentKeyModifier_ = 0; 

  switch (key)
  {
  case Qt::Key_Control:
    break;
    
  default:
    //std::cout << "Release: key= " << key << ", modifiers=" << modifiers << "\n";
    break;
  };
  //emit userPressedAKey(this, modifiers, key);
  e->accept();
};



//
void SgPlot::rescaleArea(double xFactor, double yFactor, int posX, int posY)
{
  scaleX_ *= xFactor;
  scaleY_ *= yFactor;
  
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();

  if (posX<0)
    posX = xScrollBar->pageStep()/2;
  if (posY<0)
    posY = yScrollBar->pageStep()/2;
  int x_old = xScrollBar->value() + posX;
  int y_old = yScrollBar->value() + posY;

  int xL = area_->width_;
  int yL = area_->height_;
  double xFprime = xFactor + (xFactor - 1.0)*(area_->xMargins_ - 1)/xL;
  double yFprime = yFactor + (yFactor - 1.0)*(area_->yMargins_ - 1)/yL;
 
  area_->resize(scaleX_*plotScroller_->maximumViewportSize().width(), 
                scaleY_*plotScroller_->maximumViewportSize().height());
  area_->setVisibleWidth(plotScroller_->maximumViewportSize().width());
  area_->setVisibleHeight(plotScroller_->maximumViewportSize().height());


  xScrollBar->setValue(area_->xDataBegin_ + (int)round(xFprime*(x_old - area_->xDataBegin_)) - posX);
  yScrollBar->setValue(area_->yDataBegin_ + (int)round(yFprime*(y_old - area_->yDataBegin_)) - posY);
 
  // adjust enabilities of actions:
  zoomInAction_  ->setEnabled(scaleX_ <= maxZoomX_ && scaleY_ <= maxZoomY_);
  zoomOutAction_ ->setEnabled(scaleX_ >= minZoomX_ && scaleY_ >= minZoomY_);
  zoomXInAction_ ->setEnabled(scaleX_ <= maxZoomX_);
  zoomXOutAction_->setEnabled(scaleX_ >= minZoomX_);
  zoomYInAction_ ->setEnabled(scaleY_ <= maxZoomY_);
  zoomYOutAction_->setEnabled(scaleY_ >= minZoomY_);

  // and buttons:
  pbZommIn_  ->setEnabled(scaleX_ <= maxZoomX_ && scaleY_ <= maxZoomY_);
  pbZommOut_ ->setEnabled(scaleX_ >= minZoomX_ && scaleY_ >= minZoomY_);
  pbZommXIn_ ->setEnabled(scaleX_ <= maxZoomX_);
  pbZommXOut_->setEnabled(scaleX_ >= minZoomX_);
  pbZommYIn_ ->setEnabled(scaleY_ <= maxZoomY_);
  pbZommYOut_->setEnabled(scaleY_ >= minZoomY_);
};



//
void SgPlot::zoomIn()
{
  rescaleArea(scaleFactor_, scaleFactor_);
};



//
void SgPlot::zoomOut()
{
  rescaleArea(1.0/scaleFactor_, 1.0/scaleFactor_);
};



//
void SgPlot::zoomXIn()
{
  rescaleArea(scaleFactor_, 1.0);
};



//
void SgPlot::zoomXOut()
{
  rescaleArea(1.0/scaleFactor_, 1.0);
};



//
void SgPlot::zoomYIn()
{
  rescaleArea(1.0, scaleFactor_);
};



//
void SgPlot::zoomYOut()
{
  rescaleArea(1.0, 1.0/scaleFactor_);
};



// User actions:
// scrolling:
void SgPlot::startScrollViewport(const QPoint& newCursorPosition)
{
  area_->setUserMode(SgPlotArea::UserMode_SCROLLING);
  oldCursorPosition_ = newCursorPosition;
};



//
void SgPlot::doScrollViewport(const QPoint& newCursorPosition)
{
  int dX = oldCursorPosition_.x() - newCursorPosition.x();
  int dY = oldCursorPosition_.y() - newCursorPosition.y();

  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  xScrollBar->setValue(xScrollBar->value() + dX);
  yScrollBar->setValue(yScrollBar->value() + dY);

  oldCursorPosition_ = newCursorPosition;
};



//
void SgPlot::stopScrollViewport()
{
  area_->setUserMode(SgPlotArea::UserMode_DEFAULT);
};



// inquiring:
void SgPlot::startInquire(const QPoint& point)
{
  area_->setUserMode(SgPlotArea::UserMode_INQUIRING);

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));

  area_->setRulerToPoint(point + QPoint(dx, dy));
  area_->update();
};



//
void SgPlot::doInquire(const QPoint& point)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());
  
  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));
  
  area_->setRulerToPoint(point + QPoint(dx, dy));
  area_->update(rect);
};



//
void SgPlot::stopInquire()
{
  area_->setUserMode(SgPlotArea::UserMode_DEFAULT);
  area_->update();
};



// measuring:
void SgPlot::startMeasuring(const QPoint& point)
{
  area_->setUserMode(SgPlotArea::UserMode_MEASURING);

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));

  area_->setRulerFromPoint(point + QPoint(dx, dy));
  area_->setRulerToPoint(point + QPoint(dx, dy));
};



//
void SgPlot::doMeasuring(const QPoint& point)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());
  
  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));
  
  area_->setRulerToPoint(point + QPoint(dx, dy));
  area_->update(rect);
};



//
void SgPlot::stopMeasuring()
{
  area_->setUserMode(SgPlotArea::UserMode_DEFAULT);
  area_->update();
};



// changing ranges:
void SgPlot::startReRanging(const QPoint& point)
{
  area_->setUserMode(SgPlotArea::UserMode_RERANGING);

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));

  area_->setRulerFromPoint(point + QPoint(dx, dy));
  area_->setRulerToPoint(point + QPoint(dx, dy));
};



//
void SgPlot::doReRanging(const QPoint& point)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0-scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0-scaleY_));

  area_->setRulerToPoint(point + QPoint(dx, dy));
  area_->update(rect);
};



//
void SgPlot::stopReRanging(bool performAction)
{
  if (performAction)
  {
    double x_to   = area_->reverseCalcX(area_->getRulerToPoint().x());
    double x_from = area_->reverseCalcX(area_->getRulerFromPoint().x());
    double y_to   = area_->reverseCalcY(area_->getRulerToPoint().y());
    double y_from = area_->reverseCalcY(area_->getRulerFromPoint().y());

    if (x_to != x_from && y_to != y_from)
    {
      double minX = std::min(x_to, x_from);
      double maxX = std::max(x_to, x_from);
      double minY = std::min(y_to, y_from);
      double maxY = std::max(y_to, y_from);
      area_->setUserDefinedRanges(minX, maxX, minY, maxY);
      cbUserDefined_->setEnabled(true);
      cbUserDefined_->setChecked(true);
      cbRangeVisible_->setEnabled(false);
      for (int i=0; i<bgRangeLimits_->buttons().size(); i++)
        bgRangeLimits_->buttons().at(i)->setEnabled(false);
      cbWStdVar_->setEnabled(false);
    };
  };
//  else
//    area_->unsetUserDefinedRanges();
  
  area_->setUserMode(SgPlotArea::UserMode_DEFAULT);
  area_->update();
};



// querying data:
void SgPlot::queryData(const QPoint& point, QueryMode qm)
{
  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0 - scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0 - scaleY_));

  SgPlotBranch   *branch = NULL;
  int            idx = -1;
  area_->queryPoint(point + QPoint(dx, dy), branch, idx);
  if (branch && idx>-1)
  {
    if (modes_ & PM_Q_PNT_EXT_PROC && qm != QM_Mode3) // Mode3 (==Shift) is for the internal processing:
      emit pointInfoRequested(this, branch, idx, area_->getXColumn(), area_->getYColumn(), qm);
    else
    {
      QString xStr, yStr;
      double x = branch->data()->getElement(idx, area_->getXColumn());
      double y = branch->data()->getElement(idx, area_->getYColumn());
    
      if (!(branch->getDataAttr(idx) & SgPlotCarrier::DA_SELECTED))
      {
        branch->addDataAttr(idx, SgPlotCarrier::DA_SELECTED);
        area_->update();
      };

      if (area_->isXTicsMJD())
        xStr = " (time axis): " + SgMJD(x).toString();
      else
        xStr.sprintf(" (argument axis): %.6g", x);
      yStr.sprintf(" (value axis): %.6g", y);
   
      QMessageBox msgBox(this);
      msgBox.setIcon(QMessageBox::Information);
      msgBox.setText("<b>Inquired point</b>");
      msgBox.setInformativeText("<p>You have selected a point from the branch <b>" + 
        branch->getName() + "</b>. Its attributes are:</p>" + 
        "<p>index #" + QString().setNum(idx) + ",</p>" +
        "<p>" + xStr + "</p>" +
        "<p>" + yStr + "</p>" );
      //    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      //    msgBox.setDefaultButton(QMessageBox::Save);
      msgBox.exec();
    };
  };
};



// marking points:
void SgPlot::startSelecting(const QPoint& point, bool isInverse)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());

  area_->setUserMode(isInverse ? SgPlotArea::UserMode_DESELECTING : SgPlotArea::UserMode_SELECTING);

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0 - scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0 - scaleY_));

  area_->setRulerFromPoint(point + QPoint(dx, dy));
  area_->setRulerToPoint(point + QPoint(dx, dy));
  area_->update(rect);
};



//
void SgPlot::doSelecting(const QPoint& point, bool isInverse)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());

  int l, dx=0, dy=0;
  l = plotScroller_->maximumViewportSize().width();
  if (area_->width() < l)
    dx = -(int)round(0.5*l*(1.0 - scaleX_));
  l = plotScroller_->maximumViewportSize().height();
  if (area_->height() < l)
    dy = -(int)round(0.5*l*(1.0 - scaleY_));

  area_->setUserMode(isInverse ? SgPlotArea::UserMode_DESELECTING : SgPlotArea::UserMode_SELECTING);
  area_->setRulerToPoint(point + QPoint(dx, dy));
  QPoint            pointLT;                                      // left top
  QPoint            pointRB;                                      // right bottom
  pointLT.setX(std::min(area_->getRulerFromPoint().x(),
    std::min(area_->getRulerToPoint().x(), area_->getRulerToPointPrev().x())));
  pointLT.setY(std::min(area_->getRulerFromPoint().y(),
    std::min(area_->getRulerToPoint().y(), area_->getRulerToPointPrev().y())));
  pointRB.setX(std::max(area_->getRulerFromPoint().x(),
    std::max(area_->getRulerToPoint().x(), area_->getRulerToPointPrev().x())));
  pointRB.setY(std::max(area_->getRulerFromPoint().y(),
    std::max(area_->getRulerToPoint().y(), area_->getRulerToPointPrev().y())));
  area_->update(QRect(pointLT-QPoint(5,5), pointRB+QPoint(5,5)));
};



//
void SgPlot::stopSelecting(const QPoint&, bool)
{
  QScrollBar *xScrollBar = plotScroller_->horizontalScrollBar();
  QScrollBar *yScrollBar = plotScroller_->verticalScrollBar();
  QRect rect(xScrollBar->value(), yScrollBar->value(),
                xScrollBar->pageStep(), yScrollBar->pageStep());

  area_->setUserMode(SgPlotArea::UserMode_DEFAULT);
  area_->update(rect);
};



//
void SgPlot::save2File()
{
  area_->output4Files(path2Outputs_);
};



//
void SgPlot::save2Image()
{
  bool                          isOutputDirExists(false);
  if (!path2Outputs_.isEmpty())
  {
    bool                        isOk(true);
    QDir                        d(path2Outputs_);
    if (!d.exists())
      isOk = d.mkpath(d.absolutePath());
    else
      isOutputDirExists = true;
    if (!isOk)
      logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::GUI, className() + 
      "::save2Image(): cannot create directory " + path2Outputs_);
    else
      isOutputDirExists = true;
  };
  
  QString         fileName = plotCarrier_->getFile2SaveBaseName()
    + QString("").sprintf("[%u:%u]", area_->getXColumn(), area_->getYColumn());
  if (isOutputDirExists)
    fileName = path2Outputs_ + "/" + fileName;

  QString                       fileSuffix("");
  
  switch (outputFormat_)
  {
    case OF_PS:
    case OF_PDF:
      fileSuffix = ".pdf";
    break;
    case OF_JPG:
      fileSuffix = ".jpg";
    break;
    case OF_PNG:
      fileSuffix = ".png";
    break;
    case OF_PPM:
      fileSuffix = ".ppm";
    break;
    default:
      logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::GUI, className() + 
        "::save2Image(): unknown image type " + QString("").setNum(outputFormat_));
      fileSuffix = ".pdf";
    break;
  };
  //
  QPainter                      painter;

  if (outputFormat_==OF_PS || outputFormat_==OF_PDF)
  {
// PageOrientation
// Resolution
// PageSize
//
// pointSize
// openPointIncrement
// fontSize
//
    QPdfWriter                  pdfWriter(fileName + ".pdf");
    pdfWriter.setPageOrientation(QPageLayout::Landscape);
    pdfWriter.setResolution(300);
    pdfWriter.setPageSize(QPageSize(QPageSize::Letter));
    pdfWriter.setCreator(libraryVersion.name());

    // make an output:
    painter.begin(&pdfWriter);
    area_->output4Print(&painter, pdfWriter.width(), pdfWriter.height(), 12, 6, 12);
    painter.end();
  }
  else
  {
// width
// height
// or (from plot) * scale
//
// pointSize
// openPointIncrement
// fontSize
//
    QImage                      image(2*QSize(area_->width(), area_->height()),
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
      QImage::Format_RGBA64
#else
      QImage::Format_ARGB32
#endif
    );
    image.fill(QColor(255, 255, 255));

    painter.begin(&image);
    area_->output4Print(&painter, image.width(), image.height(), 5, 2, 16);
    painter.end();
    image.save(fileName + fileSuffix);
  };
  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::IO, className() + 
    "::save2Image(): the plot has been saved into the file " + fileName + fileSuffix);
};
/*=====================================================================================================*/

















/*=====================================================================================================*/
