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

#ifndef SG_GUI_PLOTTER_H
#define SG_GUI_PLOTTER_H

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QVector>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QAction>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QComboBox>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QScrollArea>
#   include <QtWidgets/QScrollBar>
#else
#   include <QtGui/QAction>
#   include <QtGui/QCheckBox>
#   include <QtGui/QComboBox>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QPushButton>
#   include <QtGui/QScrollArea>
#   include <QtGui/QScrollBar>
#endif


#include <QtGui/QPen>
#include <QtGui/QCursor>
#include <QtGui/QTextCharFormat>


#include <SgLogger.h>
#include <SgMatrix.h>


class QButtonGroup;


/***===================================================================================================*/
/**
 * One branch on a plot.
 *
 */
/**====================================================================================================*/
class SgPlotBranch
{
public:
  /**A constructor.
   */
  SgPlotBranch(unsigned int, unsigned int, unsigned int, const QString&, bool hasExtKeys=false);
  /**A destructor.
   * Frees allocated memory.
   */
  ~SgPlotBranch();
  
  inline unsigned int           numOfRows() const {return numOfRows_;};
  inline SgMatrix*              data() {return data_;};
  inline void                   setIsBrowsable(bool is) {isBrowsable_ = is;};
  void                          setDataAttr(unsigned int, unsigned int);
  void                          addDataAttr(unsigned int, unsigned int);
  void                          delDataAttr(unsigned int, unsigned int);
  void                          xorDataAttr(unsigned int, unsigned int);
  inline void                   setName(const QString& name) {name_=name;};
  inline bool                   getIsBrowsable() const {return isBrowsable_;};
  unsigned int                  getDataAttr(unsigned int) const;
  inline const QString&         getName() const {return name_;};
  inline QString&               getAlternativeTitleName(int idx=-1)
    {return alternativeTitleName_.contains(idx)?alternativeTitleName_[idx]:alternativeTitleName_[-1];};
  inline void                   setAlternativeTitleName(const QString& aName, int idx=-1)
    {alternativeTitleName_[idx] = aName;};
  
  bool                          isPointVisible(int idx, unsigned int limits) const;
  bool                          isPointInRanges(int idx, unsigned int limits) const;
  bool                          hasExtKeys() const {return hasExtKeys_;};
  void                          setExtKey(int idx, const QString& key)
    {if (hasExtKeys_) extKeys_[idx]=key;};
//  const QString&                getExtKey(int idx) {return hasExtKeys_?extKeys_.at(idx):QString("");};
  const QString&                getExtKey(int idx) {return extKeys_.at(idx);};
  void                          flagExtKey(const QString& eKey, bool on);
  
private:
  QString                       name_;
  QMap<int, QString>            alternativeTitleName_;
  SgMatrix                     *data_;
  unsigned int                  numOfRows_;             //<! Number of rows
  bool                          isBrowsable_;
  bool                          hasExtKeys_;
  QVector<QString>              extKeys_;
  QVector<bool>                 extKeysVisible_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * An object that pass information about data to plot and its format
 *
 */
/**====================================================================================================*/
class SgPlotCarrier
{
public:
  enum AxisType
  {
    AxisType_DATA = 0,
    AxisType_MJD  = 1
  };

  enum DataAttr
  {
    DA_NONUSABLE  = 1<<0,
    DA_REJECTED   = 1<<1,
    DA_BAR        = 1<<2,
    DA_SELECTED   = 1<<3,
    DA_SPECIAL_01 = 1<<4,
  };
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  SgPlotCarrier(unsigned int, unsigned int, const QString&);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgPlotCarrier();

  //
  // Interfaces:
  //
  // access:
  inline QList<SgPlotBranch*>*  listOfBranches() {return &listOfBranches_;};
  inline QVector<QString*>*     columnNames() {return &columnNames_;};
  inline unsigned int           numOfValuesColumns() const {return numOfValuesColumns_;};
  inline unsigned int           numOfSigmasColumns() const {return numOfSigmasColumns_;};
  inline bool                   isOK() const {return isOK_;};
  // gets:
  inline AxisType               getAxisType(int columnIdx) const;
  inline int                    getStdVarIdx(int columnIdx) const;
  inline QString&               getName(int idx=-1) {return name_.contains(idx)?name_[idx]:name_[-1];};

  inline const QString&         getFile2SaveBaseName() const {return file2SaveBaseName_;};
  // sets:
  inline void                   setAxisType(int columnIdx, AxisType axisType);
  inline void                   setStdVarIdx(int columnIdx, int sigmaColumnIdx);
  inline void                   setName(const QString& name, int idx=-1) {name_[idx]=name;};
  inline void                   setFile2SaveBaseName(const QString& name) {file2SaveBaseName_=name;};
  //
  // Functions:
  //
  inline QString                className() const {return "SgPlotCarrier";};
  inline int                    numOfColumns() const;
  void                          createBranch(unsigned int numberOfRows, const QString& branchName,
                                  bool hasExtKeys=false);
  bool                          selfCheck();
  void                          setNameOfColumn(unsigned int, const QString&);

private:
  QMap<int, QString>            name_;                  //<! name of the plot (title), could vary
  QList<SgPlotBranch*>          listOfBranches_;        //<! container for plot branches
  bool                          isOK_;
  QString                       file2SaveBaseName_;

  // branch format descriptor:
  unsigned int                  numOfValuesColumns_;    //<! Number of values columns 
  unsigned int                  numOfSigmasColumns_;    //<! Number of std errors columns
  QVector<QString*>             columnNames_;           //<! Vector of column names
  int*                          dataTypes_;             //<! Types of data per each column
  int*                          dataStdVarIdx_;         //<! Indexes of std. var. columns
};
/*=====================================================================================================*/



//
inline void SgPlotCarrier::setAxisType(int columnIdx, AxisType axisType)
{
  if (0<=columnIdx && columnIdx<numOfColumns()-1) 
    *(dataTypes_ + columnIdx) = axisType;
};



//
inline void SgPlotCarrier::setStdVarIdx(int columnIdx, int sigmaColumnIdx)
{
  if (0<=columnIdx && columnIdx<numOfColumns()-1 && sigmaColumnIdx<numOfColumns()-1)
    *(dataStdVarIdx_ + columnIdx) = sigmaColumnIdx;
};



//
inline SgPlotCarrier::AxisType SgPlotCarrier::getAxisType(int columnIdx) const
{
  return (0<=columnIdx && columnIdx<numOfColumns()-1) ? 
    (AxisType)(*(dataTypes_+columnIdx)) : AxisType_DATA;
};



//
inline int SgPlotCarrier::getStdVarIdx(int columnIdx) const
{
  return (0<=columnIdx && columnIdx<numOfColumns()-1) ? *(dataStdVarIdx_ + columnIdx) : -1;
};



//
inline int SgPlotCarrier::numOfColumns() const 
{
  return numOfValuesColumns_ + numOfSigmasColumns_ + 1;
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * Plot area itself
 *
 */
/**====================================================================================================*/
class SgPlotArea : public QWidget
{
  Q_OBJECT

friend class SgPlot;
public:
  enum UserMode
  {
    UserMode_DEFAULT   = 0,
    UserMode_SCROLLING = 1,
    UserMode_INQUIRING = 2,
    UserMode_MEASURING = 3,
    UserMode_RERANGING = 4,
    UserMode_QUERYING  = 5,
    UserMode_SELECTING = 6,
    UserMode_DESELECTING = 7,
  };
  enum SetsOfData
  {
    SOD_ALL        = 0,
    SOD_USABLE     = 1,
    SOD_PROCESSED  = 2,
  };
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  SgPlotArea(SgPlotCarrier*, QWidget* parent=0, Qt::WindowFlags f=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgPlotArea();


  //
  // Interfaces:
  //
  void                          setXColumn(unsigned int xColumn);
  void                          setYColumn(unsigned int yColumn);
  inline unsigned int           getXColumn() const {return xColumn_;};
  inline unsigned int           getYColumn() const {return yColumn_;};
  inline UserMode               getUserMode() const {return userMode_;};
  inline const QPoint&          getRulerFromPoint() const {return rulerFromPoint_;};
  inline const QPoint&          getRulerToPoint() const {return rulerToPoint_;};
  inline const QPoint&          getRulerToPointPrev() const {return rulerToPointPrev_;};
  inline bool                   isXTicsMJD() const {return isXTicsMJD_;};
  inline SetsOfData             getRangeLimits() const {return rangeLimits_;};
  inline SetsOfData             getSets2plot() const {return sets2plot_;};

  inline int                    width() const {return width_;};
  inline int                    height() const {return height_;};


  void                          setBPHuePhase(int);
  void                          setBPSaturation(int);
  void                          setBPValue(int);
  inline void                   setIsPlotPoints(bool is) {isPlotPoints_=is;};
  inline void                   setIsPlotLines(bool is)  {isPlotLines_=is;};
  inline void                   setIsPlotErrBars(bool is){isPlotErrBars_=is;};
  inline void                   setIsPlotImpulses(bool is){isPlotImpulses_=is;};
  void                          setUserMode(UserMode mode);
  inline void                   setRulerFromPoint(QPoint point) {rulerFromPoint_=point;};
  inline void                   setRulerToPoint(QPoint point)
    {rulerToPointPrev_=rulerToPoint_; rulerToPoint_=point;};
  inline void                   setVisibleWidth(int width) {visibleWidth_=width<width_?width:width_;};
  inline void                   setVisibleHeight(int height)
    {visibleHeight_=height<height_?height:height_;};

  // modes:
  inline void                   setHave2HasZero(bool is) {have2HasZero_=is;};
  inline void                   setIsLimitsOnVisible(bool is) {isLimitsOnVisible_=is;};
  inline void                   setIsStdVar(bool is) {isStdVar_=is;};
  inline void                   setIsRangeSymmetrical(bool is) {isRangeSymmetrical_=is;};
  inline void                   setRangeLimits(SetsOfData rl) {rangeLimits_=rl;};
  inline void                   setSets2plot(SetsOfData sod) {sets2plot_=sod;};


  //
  // Functions:
  //
  inline virtual QString        className() const {return "SgPlotArea";};
  void                          output4Print(QPainter*, int, int, int, int, int);
  void                          output4Files(const QString& path);
  void                          dataChanged() {initBranchPens();};
  void                          setUserDefinedRanges(double, double, double, double);
  void                          unsetUserDefinedRanges();
  void                          queryPoint(const QPoint&, SgPlotBranch*&, int&);

protected:

  // areas:
  int                           width_;
  int                           height_;
  int                           upMargin_;
  int                           rightMargin_;
  int                           visibleWidth_;
  int                           visibleHeight_;
  // labels & tics:
  int                           xLabelWidth_;
  int                           labelsHeight_;
  int                           yLabelWidth_;
  int                           yLabelHeight_;
  int                           xTicsWidth_;
  int                           yTicsWidth_;
  // frame area:
  int                           xFrameBegin_;
  int                           xFrameEnd_;
  int                           yFrameBegin_;
  int                           yFrameEnd_;
  // data area:
  int                           xDataBegin_;
  int                           xDataEnd_;
  int                           yDataBegin_;
  int                           yDataEnd_;
  // Title:
  int                           titleWidth_;
  // X Tics:
  int                           numOfXTics_;
  int                           xStepP_;
  double                        xTicsStep_;
  double                        xTicsBias_;
  bool                          isXTicsBiased_;
  bool                          isXTicsMJD_;
  int                           xTicsMJD_;
  // Y Tics:
  int                           numOfYTics_;
  int                           yStepP_;
  double                        yTicsStep_;
  double                        yTicsBias_;
  bool                          isYTicsBiased_;
  // labels:
  QString                      *xLabel_;
  QString                      *yLabel_;
  static QString                xLabel4Unknown_;
  static QString                yLabel4Unknown_;
  // points:
  int                           radius_;
  int                           ddr_;


  // 
  unsigned int                  xColumn_;
  unsigned int                  yColumn_;
  // natural ranges:
  double                        maxX_;
  double                        maxY_;
  double                        minX_;
  double                        minY_;
  // user defined ranges:
  double                        userDefinedMaxX_;
  double                        userDefinedMaxY_;
  double                        userDefinedMinX_;
  double                        userDefinedMinY_;
  bool                          useUserDefinedRanges_;
  int                           xMargins_;
  int                           yMargins_;

  // transform parameters:
  double                        f_Ax_;
  double                        f_Bx_;
  double                        f_Ay_;
  double                        f_By_;
  
  // pens:
  QPen                         *framePen_;
  QPen                         *branchPens_;
  QBrush                       *branchBrushes_;
  QPen                         *zeroPen_;
  QPen                         *barPen_;
  QPen                         *ticLinesPen_;
  QPen                         *rulerPen_;
  QBrush                       *rulerBrush_;
  QPen                         *branchSelectedPens_;
  QBrush                       *branchSelectedBrushes_;
  QPen                         *ignoredPen_;
  QBrush                       *ignoredBrush_;
  int                           bpHuePhase_;
  int                           bpSaturation_;
  int                           bpValue_;
    
  // draw mode:
  bool                          isPlotPoints_;
  bool                          isPlotLines_;
  bool                          isPlotErrBars_;
  bool                          isPlotImpulses_;
  
  // options:
  bool                          isLimitsOnVisible_;
  bool                          isStdVar_;
  bool                          isRangeSymmetrical_;
  SetsOfData                    rangeLimits_;
  SetsOfData                    sets2plot_;

  // browsables:
  SgPlotCarrier                *plotCarrier_;

  // misc
  bool                          have2HasZero_;

  // user modes (just to browse cursor):
  UserMode                      userMode_;
  QCursor                       cursorDefault_;
  QCursor                       cursorScrolling_;
  QCursor                       cursorMeasuring_;
  
  // ruler:
  QPoint                        rulerFromPoint_;
  QPoint                        rulerToPoint_;
  QPoint                        rulerToPointPrev_;


  void drawWholePlot(QPainter*, const QRect&);
  void drawPointInfo(QPainter*);
  void drawRuler(QPainter*);
  void drawRangeSelector(QPainter*);
  void drawPointSelector(QPainter*);
  void drawFrames(QPainter*);
  void drawYTics(QPainter*);
  void drawXTics(QPainter*);
  void drawXmjdTics(QPainter*);
  void drawData(QPainter*, const QRect&);
  void calcLimits();
  void calcTransforms();
  void defineAreas(QPainter*);
  inline int calcX(double x) const;
  inline int calcY(double y) const;
  inline double reverseCalcX(int x) const;
  inline double reverseCalcY(int y) const;
  void setBranchColors();
  void initBranchPens();

  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);
};
/*=====================================================================================================*/



inline int SgPlotArea::calcX(double x) const
{
  double f = f_Ax_ + f_Bx_*x; 

  if (f < 0.0) 
    return 0; 
  if (f > width_) 
    return width_ - 1; 
  
  return (int)f;
};



inline int SgPlotArea::calcY(double y) const
{
  double f = f_Ay_ + f_By_*y; 

  if (f < 0.0) 
    return 0;
  if (f > height_) 
    return height_ - 1; 

  return (int)f;
};



inline double SgPlotArea::reverseCalcX(int x) const
{
  return ((double)x - f_Ax_)/f_Bx_;
};



inline double SgPlotArea::reverseCalcY(int y) const
{
  return ((double)y - f_Ay_)/f_By_;
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * Scroller for the plot area
 * (we need it to intercept few signals)
 */
/**====================================================================================================*/
class SgPlotScroller : public QScrollArea 
{
  Q_OBJECT
  
public:
  SgPlotScroller(QWidget *parent=NULL) : QScrollArea(parent) {};

signals:
  void mouseWheelRotated(QWheelEvent *);
  void mousePressed(QMouseEvent *);
  void mouseMoved(QMouseEvent *);
  void mouseReleased(QMouseEvent *);
  void mouseDoubleClicked(QMouseEvent *);
  void keyPressed(QKeyEvent *);
  void keyReleased(QKeyEvent *);

protected:
  virtual void wheelEvent(QWheelEvent *e) {emit mouseWheelRotated(e); e->accept();};
  virtual void mousePressEvent(QMouseEvent *e) {emit mousePressed(e); e->accept();};
  virtual void mouseMoveEvent(QMouseEvent *e) {emit mouseMoved(e); e->accept();};
  virtual void mouseReleaseEvent(QMouseEvent *e) {emit mouseReleased(e); e->accept();};
  virtual void mouseDoubleClickEvent(QMouseEvent *e) {emit mouseDoubleClicked(e); e->accept();};
  //  virtual void keyPressEvent(QKeyEvent *e) {emit keyPressed(e); e->accept();};
  //  virtual void keyReleaseEvent(QKeyEvent *e) {emit keyReleased(e); e->accept();};
};
/*=====================================================================================================*/




class QListView;
/***===================================================================================================*/
/**
 * Plot area and aux widgets
 *
 */
/**====================================================================================================*/
class SgPlot : public QWidget
{
  Q_OBJECT
  
public:
  enum PlotMode
  {
    PM_HAS_HAVE_ZERO    = 1<<0,
    PM_IMPULSE          = 1<<1,
    PM_WO_BRANCH_NAMES  = 1<<2,
    PM_WO_AXIS_NAMES    = 1<<3,
    PM_WO_DOTS          = 1<<4,
    PM_LINES            = 1<<5,
    PM_ERRBARS          = 1<<6,
    PM_Q_PNT_EXT_PROC   = 1<<7,   // external point info display
    PM_FILTERS_ENABLED  = 1<<8,
    PM_EXT_KEY_SELECT   = 1<<9,   // extended key selector
  };
  enum OutputFormat
  {
    OF_PS               = 0,
    OF_PDF              = 1,
    OF_JPG              = 2,
    OF_PNG              = 3,
    OF_PPM              = 4,
  };
  enum QueryMode
  {
    QM_Mode0            = 0,
    QM_Mode1            = 1,
    QM_Mode2            = 2,
    QM_Mode3            = 3,
  };
  
  SgPlot(SgPlotCarrier*, const QString&, QWidget* =0, unsigned int =0);

  virtual ~SgPlot();

  SgPlotCarrier* carrier() {return plotCarrier_;};

  int getNumOfXColumn() const {return area_->getXColumn();};
  int getNumOfYColumn() const {return area_->getYColumn();};
  OutputFormat getOutputFormat() const {return outputFormat_;};
  const QString& getPath2Outputs() const {return path2Outputs_;};
  
  void setOutputFormat(OutputFormat fmt) {outputFormat_ = fmt;};
  void setPath2Outputs(const QString& pth) {path2Outputs_ = pth;};
  
  void setFilterNames(const QList<QString>&);
  void setFilterAuxNames(const QList<QString>&);
  void setFilterExtNames(const QList<QString>&);

  // some values of the plots are changed:
  void dataContentChanged();

  // the format of the plot is changed:
  void dataStructureChanged();

  // communication with a user
signals:
  void userPressedAKey(SgPlot*, Qt::KeyboardModifiers, int);
  void xAxisChanged(int);
  void yAxisChanged(int);
  void pointInfoRequested(SgPlot*, SgPlotBranch*, int,  int, int, SgPlot::QueryMode);
//void pointInfoRequested_mode1(SgPlot*, SgPlotBranch*, int,  int, int);
//void pointInfoRequested_mode2(SgPlot*, SgPlotBranch*, int,  int, int);


public slots:
  void changeXaxis(int);
  void changeYaxis(int);
  void changeXaxisTemp2(int);
  void changeYaxisTemp2(int);



protected:
  virtual QString className() const {return "SgPlot";};
  virtual void resizeEvent(QResizeEvent*);
  //  virtual void          keyPressEvent(QKeyEvent *e) {emit keyPressed(e); e->accept();};
  //  virtual void          keyReleaseEvent(QKeyEvent *e) {emit keyReleased(e); e->accept();};
  virtual void keyPressEvent  (QKeyEvent *e) {processKeyPressEvent(e);};
  virtual void keyReleaseEvent(QKeyEvent *e) {processKeyReleaseEvent(e);};
  QWidget* control();
  void fillAxisNames();
  void fillBranchesNames();
  void adjustScrollBar(QScrollBar*, double, double, double);
  void rescaleArea(double, double, int=-1, int=-1);
  
  // user actions:
  // scrolling:
  void startScrollViewport(const QPoint&);
  void doScrollViewport(const QPoint&);
  void stopScrollViewport();
  // inquiring:
  void startInquire(const QPoint&);
  void doInquire(const QPoint&);
  void stopInquire();
  // measuring:
  void startMeasuring(const QPoint&);
  void doMeasuring(const QPoint&);
  void stopMeasuring();
  // changing ranges:
  void startReRanging(const QPoint&);
  void doReRanging(const QPoint&);
  void stopReRanging(bool);
  // querying data:
  void queryData(const QPoint&, QueryMode);
  // selecting points:
  void startSelecting(const QPoint&, bool=false);
  void doSelecting(const QPoint&, bool=false);
  void stopSelecting(const QPoint&, bool=false);

  QWidget* branchesWidget();

  
  
protected slots:
  void changeXaxisTemp(int);
  void changeYaxisTemp(int);
  void colorHChanged(int);
  void colorSChanged(int);
  void colorVChanged(int);
  void branchChanged();
  void extKeyChanged();
  void dmPointsChanged(bool);
  void dmLinesChanged(bool);
  void dmErrBarsChanged(bool);
  void dmImpulsesChanged(bool);

  void oUserDefinedChanged(bool);
  void modifySets2plot(int);
  void setRangeSymmetrical(bool);
  void modifyRangeLimits(int);
  
  void oVisRang(bool);
  void oWStdVar(bool);

  // zooming:
  void zoomIn();
  void zoomOut();
  void zoomXIn();
  void zoomXOut();
  void zoomYIn();
  void zoomYOut();
  void zoomNormalView();
  // interaction with a user:
  void processWheelEvent(QWheelEvent*);
  void processMousePressEvent(QMouseEvent*);
  void processMouseMoveEvent(QMouseEvent*);
  void processMouseReleaseEvent(QMouseEvent*);
  void processMouseDoubleClickEvent(QMouseEvent*);
  void processKeyPressEvent(QKeyEvent*);
  void processKeyReleaseEvent(QKeyEvent*);
  void markAllBranchesAsSelected();
  void markAllBranchesAsDeselected();
  void markAllExtKeysAsSelected();
  void markAllExtKeysAsDeselected();
  void markPrevBranch();
  void markNextBranch();
  void markPrevExtKey();
  void markNextExtKey();
  //
  void processFilterMinus();
  void processFilterPlus ();
  void processFilterAuxMinus(const QPoint&);
  void processFilterAuxPlus (const QPoint&);
  void processFilterExtMinus();
  void processFilterExtPlus ();

  // saving:
  void save2File();
//  void save2PS();
  void save2Image();


private:
  static const double           scaleFactor_;
  SgPlotScroller               *plotScroller_;
  SgPlotArea                   *area_;
  QFrame                       *controls_;
  double                        scaleX_;
  double                        scaleY_;
  OutputFormat                  outputFormat_;
  QString                       path2Outputs_;

  unsigned int                  modes_;
  SgPlotCarrier                *plotCarrier_;
  
  double                        maxZoomX_;
  double                        minZoomX_;
  double                        maxZoomY_;
  double                        minZoomY_;
  
  // user actions:
  // scroll viewport:
  QPoint                        oldCursorPosition_;
  QPoint                        refPoint_;

  // controls:
  QComboBox                    *cbXAxis_;
  QComboBox                    *cbYAxis_;
  QButtonGroup                 *bgSets2plot_;
  QListView                    *lvBranches_;
  bool                          isBranchViewInSpecialMode_;
  QList<QAction*>               filterActions_;
  QList<QAction*>               filterAuxActions_;
  QList<QAction*>               filterExtActions_;
  
  QCheckBox                    *cbPoints_;
  QCheckBox                    *cbLines_;
  QCheckBox                    *cbErrBars_;
  QCheckBox                    *cbImpulses_;

  QCheckBox                    *cbUserDefined_;
  QCheckBox                    *cbRangeVisible_;
  QCheckBox                    *cbWStdVar_;
  QCheckBox                    *cbSymmetrical_;
  QButtonGroup                 *bgRangeLimits_;
  
  QPushButton                  *pbZommIn_;
  QPushButton                  *pbZommOut_;
  QPushButton                  *pbZommXIn_;
  QPushButton                  *pbZommXOut_;
  QPushButton                  *pbZommYIn_;
  QPushButton                  *pbZommYOut_;


  QAction                      *zoomInAction_;
  QAction                      *zoomXInAction_;
  QAction                      *zoomYInAction_;
  QAction                      *zoomOutAction_;
  QAction                      *zoomXOutAction_;
  QAction                      *zoomYOutAction_;
  QAction                      *normalSizeAction_;
  //
  unsigned int                  currentKeyModifier_;
  
  //
  QList<QString>                extKeys_;
  QListView                    *lvExtKeys_;
  bool                          isExtKeyViewInSpecialMode_;
};
/*=====================================================================================================*/
/*=====================================================================================================*/



/*=====================================================================================================*/
#endif // SG_GUI_PLOTTER_H
