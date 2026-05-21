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

#ifndef SG_GUI_VLBI_SOURCE_LIST_H
#define SG_GUI_VLBI_SOURCE_LIST_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <SgVlbiSourceInfo.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QDialog>
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QCheckBox>
#   include <QtGui/QDialog>
#   include <QtGui/QHeaderView>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QTreeWidgetItem>
#endif

#include <SgGuiQTreeWidgetExt.h>










/*=====================================================================================================*/
class SgGuiVlbiSourceItem : public QTreeWidgetItem
{
public:
   SgGuiVlbiSourceItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    {srInfo_=NULL;};
  virtual ~SgGuiVlbiSourceItem() {srInfo_=NULL;};
  void setSrInfo(SgVlbiSourceInfo* info) {srInfo_ = info;};
  SgVlbiSourceInfo* getSrInfo() {return srInfo_;};
  virtual bool operator<(const QTreeWidgetItem &other) const;
  
private:
  SgVlbiSourceInfo *srInfo_;
};
/*=====================================================================================================*/



/*=====================================================================================================*/
class SgGuiVlbiSourceList : public QWidget
{
  Q_OBJECT
public:

  SgGuiVlbiSourceList(SgObjectBrowseMode, const QString&, QMap<QString, SgVlbiSourceInfo*>*,
    QWidget* =0, Qt::WindowFlags=0);
  virtual ~SgGuiVlbiSourceList() {sourcesByName_=NULL;};

  private 
  slots:
  void toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void entryClicked(QTreeWidgetItem*, int);
  void entryDoubleClicked(QTreeWidgetItem*, int);
  void updateContent();
  void updateContent4Nums();
  void modifySourceInfo(bool);

protected:
  QString                       ownerName_;
  SgObjectBrowseMode            browseMode_;
  int                           constColumns_;
  QMap<QString, SgVlbiSourceInfo*>
                               *sourcesByName_;
  SgGuiQTreeWidgetExt          *tweSources_;
  double                        scl4delay_;
  double                        scl4rate_;
  int                           numbersBrowseMode_;

  virtual inline QString className() const {return "SgGuiVlbiSourceList";};
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgGuiVlbiSrcInfoEditor : public QDialog
{
  Q_OBJECT
public:
  SgGuiVlbiSrcInfoEditor(SgVlbiSourceInfo*, const QString&, QWidget* =0, Qt::WindowFlags=0);
  ~SgGuiVlbiSrcInfoEditor();
  inline QString className() const {return "SgGuiVlbiSrcInfoEditor";};

  signals:
  void contentModified(bool);
  
  private
  slots:
  void toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void accept();
  void reject() {QDialog::reject(); isModified_=false; deleteLater();};
  void editSsmPoint();
  void deleteSsmPoint();
  void insertSsmPoint();
  void editSrcStModel(QTreeWidgetItem*, int);
  void updateModifyStatus(bool);
  void addNewSsmPoint(SgVlbiSourceInfo::StructModelMp*);

protected:
  SgVlbiSourceInfo             *sourceInfo_;
  bool                          isModified_;
  // widgets:
  QCheckBox                    *cbAttributes_[7];
  SgGuiQTreeWidgetExt          *twSrcStModels_;
  //
  // functions:
  void                          acquireData();
};
/*=====================================================================================================*/





/*=====================================================================================================*/
// Aux objects (options and SS model parameter editing):
//
/*=====================================================================================================*/
class SgGuiVlbiSrcStrModelItem : public QTreeWidgetItem
{
public:
   inline SgGuiVlbiSrcStrModelItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    {point_=NULL; modelIdx_=0;};
  virtual ~SgGuiVlbiSrcStrModelItem() {point_=NULL;};
  inline SgVlbiSourceInfo::StructModelMp* getPoint() {return point_;};
  inline int getModelIdx() {return modelIdx_;};
  inline void setPoint(SgVlbiSourceInfo::StructModelMp* p) {point_ = p;};
  inline void setModelIdx(int idx) {modelIdx_ = idx;};
private:
  SgVlbiSourceInfo::StructModelMp 
                               *point_;
  int                           modelIdx_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class QLineEdit;
class SgGuiVlbiSrcStrModelEditor : public QDialog
{
  Q_OBJECT
public:
  SgGuiVlbiSrcStrModelEditor(SgVlbiSourceInfo*, SgGuiVlbiSrcStrModelItem*,
    QWidget* =0, Qt::WindowFlags=0);
  inline ~SgGuiVlbiSrcStrModelEditor() {src_=NULL; twItem_=NULL;};
  inline QString className() const {return "SgGuiVlbiSrcStrModelEditor";};

  inline void setIsModified(bool is) {isModified_ = is;};

  signals:
  void ssmPointModified(bool);
  void ssmPointCreated(SgVlbiSourceInfo::StructModelMp*);

  private 
  slots:
  void accept();
  void reject();

protected:
  SgVlbiSourceInfo             *src_;
  SgVlbiSourceInfo::StructModelMp
                               *ssmPoint_;
  SgGuiVlbiSrcStrModelItem     *twItem_;
  bool                          isModified_;
  bool                          isNewPoint_;
  // widgets:
  QLineEdit                    *leX_;
  QLineEdit                    *leY_;
  QLineEdit                    *leK_;
  QLineEdit                    *leB_;
  QCheckBox                    *cbEstPosition_;
  QCheckBox                    *cbEstK_;
  QCheckBox                    *cbEstB_;
  // functions:
  void                          acquireData();
};





/*=====================================================================================================*/
#endif // SG_GUI_VLBI_SOURCE_LIST_H
