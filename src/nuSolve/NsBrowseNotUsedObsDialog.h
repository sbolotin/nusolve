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

#ifndef NS_BROWSE_NOT_USED_OBS_DIALOG_H
#define NS_BROWSE_NOT_USED_OBS_DIALOG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QList>
#include <QtCore/QString>



#if QT_VERSION >= 0x050000
#    include <QtWidgets/QDialog>
#    include <QtWidgets/QTreeWidgetItem>
#else
#    include <QtGui/QDialog>
#    include <QtGui/QTreeWidgetItem>
#endif


class QTreeWidget;


class SgSolutionReporter;



/*=====================================================================================================*/
class NsNotUsedObsInfo
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsNotUsedObsInfo() : stn_1Name_(""), stn_2Name_(""), srcName_(""), strEpoch_(""), strQFs_(""), 
    reasons_("") {mediaIdx_ = -1;};

  NsNotUsedObsInfo(int mediaIdx, const QString&  stn_1Name, const QString&  stn_2Name, 
    const QString&  srcName, const QString&  strEpoch, const QString&  strQFs,  const QString&  reasons)
  : stn_1Name_(stn_1Name), stn_2Name_(stn_2Name), srcName_(srcName), strEpoch_(strEpoch), 
    strQFs_(strQFs), reasons_(reasons)
    {mediaIdx_ = mediaIdx;};
  
  inline NsNotUsedObsInfo(const NsNotUsedObsInfo& i)
  : stn_1Name_(i.getStn_1Name()), stn_2Name_(i.getStn_2Name()), srcName_(i.getSrcName()), 
    strEpoch_(i.getStrEpoch()), strQFs_(i.getStrQFs()), reasons_(i.getReasons())
    {mediaIdx_ = i.getMediaIdx();};

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsNotUsedObsInfo() {};

  inline NsNotUsedObsInfo& operator=(const NsNotUsedObsInfo& i)
    {setMediaIdx(i.getMediaIdx()); setStn_1Name(i.getStn_1Name()); setStn_2Name(i.getStn_2Name());
      setSrcName(i.getSrcName()); setStrEpoch(i.getStrEpoch()); setStrQFs(i.getStrQFs()); 
      setReasons(i.getReasons()); return *this;};

  //
  // Interfaces:
  //
  inline int getMediaIdx() const {return mediaIdx_;};
  inline const QString& getStn_1Name() const {return stn_1Name_;};
  inline const QString& getStn_2Name() const {return stn_2Name_;};
  inline const QString& getSrcName() const {return srcName_;};
  inline const QString&  getStrEpoch() const {return strEpoch_;};
  inline const QString&  getStrQFs() const {return strQFs_;};
  inline const QString& getReasons() const {return reasons_;};
  //
  inline void setMediaIdx(int idx) {mediaIdx_ = idx;};
  inline void setStn_1Name(const QString& str) {stn_1Name_ = str;};
  inline void setStn_2Name(const QString& str) {stn_2Name_= str;};
  inline void setSrcName(const QString& str) {srcName_= str;};
  inline void  setStrEpoch(const QString& str) {strEpoch_= str;};
  inline void  setStrQFs(const QString& str) {strQFs_= str;};
  inline void setReasons(const QString& str) {reasons_= str;};
  
  //
  // Functions:
  //
  inline QString className() const {return "NsNotUsedObsInfo";};
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  //
private:
  int                           mediaIdx_;
  QString                        stn_1Name_;
  QString                        stn_2Name_;
  QString                        srcName_;
  QString                        strEpoch_;
  QString                        strQFs_;
  QString                        reasons_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class NsDeselectedObsInfo
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsDeselectedObsInfo() : stn_1Name_(""), stn_2Name_(""), srcName_(""), strEpoch_(""), strQFs_("")
    {mediaIdx_ = -1; residual_ = 0.0;  stdDev_ = 0.0; normResid_ = 0.0;};

  NsDeselectedObsInfo(int mediaIdx, const QString&  stn_1Name, const QString&  stn_2Name, 
    const QString&  srcName, const QString&  strEpoch, const QString&  strQFs,  double residual,
    double stdDev, double normResid)
  : stn_1Name_(stn_1Name), stn_2Name_(stn_2Name), srcName_(srcName), strEpoch_(strEpoch), 
    strQFs_(strQFs)
    {mediaIdx_=mediaIdx; residual_=residual; stdDev_=stdDev; normResid_=normResid;};
  
  inline NsDeselectedObsInfo(const NsDeselectedObsInfo& i)
  : stn_1Name_(i.getStn_1Name()), stn_2Name_(i.getStn_2Name()), srcName_(i.getSrcName()), 
    strEpoch_(i.getStrEpoch()), strQFs_(i.getStrQFs())
    {mediaIdx_ = i.getMediaIdx(); residual_ = i.getResidual(); 
      stdDev_ = i.getStdDev(); normResid_ = i.getNormResid();};

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsDeselectedObsInfo() {};

  inline NsDeselectedObsInfo& operator=(const NsDeselectedObsInfo& i)
    {setMediaIdx(i.getMediaIdx()); setStn_1Name(i.getStn_1Name()); setStn_2Name(i.getStn_2Name());
      setSrcName(i.getSrcName()); setStrEpoch(i.getStrEpoch()); setStrQFs(i.getStrQFs()); 
      setResidual(i.getResidual()); setStdDev(i.getStdDev()); setNormResid(i.getNormResid());
      return *this;};

  //
  // Interfaces:
  //
  inline int getMediaIdx() const {return mediaIdx_;};
  inline const QString& getStn_1Name() const {return stn_1Name_;};
  inline const QString& getStn_2Name() const {return stn_2Name_;};
  inline const QString& getSrcName() const {return srcName_;};
  inline const QString&  getStrEpoch() const {return strEpoch_;};
  inline const QString&  getStrQFs() const {return strQFs_;};
  inline double getResidual() const {return residual_;};
  inline double getStdDev() const {return stdDev_;};
  inline double getNormResid() const {return normResid_;};
  //
  inline void setMediaIdx(int idx) {mediaIdx_ = idx;};
  inline void setStn_1Name(const QString& str) {stn_1Name_ = str;};
  inline void setStn_2Name(const QString& str) {stn_2Name_= str;};
  inline void setSrcName(const QString& str) {srcName_= str;};
  inline void  setStrEpoch(const QString& str) {strEpoch_= str;};
  inline void  setStrQFs(const QString& str) {strQFs_= str;};
  inline void setResidual(double d) {residual_=d;};
  inline void setStdDev(double d) {stdDev_=d;};
  inline void setNormResid(double d) {normResid_=d;};
  
  //
  // Functions:
  //
  inline QString className() const {return "NsDeselectedObsInfo";};
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  //
private:
  int                           mediaIdx_;
  QString                        stn_1Name_;
  QString                        stn_2Name_;
  QString                        srcName_;
  QString                        strEpoch_;
  QString                        strQFs_;
  double                        residual_;
  double                        stdDev_;
  double                        normResid_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
class NsQTreeWidgetItem : public QTreeWidgetItem
{
public:
   NsQTreeWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {};
  virtual ~NsQTreeWidgetItem() {};

  virtual bool operator<(const QTreeWidgetItem &other) const;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class NsBrowseNotUsedObsDialog : public QDialog
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsBrowseNotUsedObsDialog(const QList<NsNotUsedObsInfo*>& nonUsableObservations,
    const QList<NsDeselectedObsInfo*>& deselectedObservations, SgSolutionReporter *reporter,
    QWidget *parent=0, Qt::WindowFlags flags=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsBrowseNotUsedObsDialog();



  //
  // Interfaces:
  //



  //
  // Functions:
  //
  inline QString className() const {return "NsBrowseNotUsedObsDialog";};
  

  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void accept();
  void reject();
  void export2file();

private:
  QList<NsNotUsedObsInfo*>      nonUsableObservations_;
  QList<NsDeselectedObsInfo*>    deselectedObservations_;
  QTreeWidget                  *twUnusable_;
  QTreeWidget                  *twDeselected_;
  SgSolutionReporter           *reporter_;
  
  

protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};
};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // NS_BROWSE_NOT_USED_OBS_DIALOG_H
