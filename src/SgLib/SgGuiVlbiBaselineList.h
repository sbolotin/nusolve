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

#ifndef SG_GUI_VLBI_BASELINE_LIST_H
#define SG_GUI_VLBI_BASELINE_LIST_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgGuiQTreeWidgetExt.h>
#include <SgVlbiBaselineInfo.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QDialog>
#   include <QtGui/QHeaderView>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QTreeWidgetItem>
#endif







class SgTaskConfig;
class SgVlbiBaselineInfo;
class SgVlbiSession;



/*=====================================================================================================*/
class SgGuiVlbiBaselineItem : public QTreeWidgetItem
{
public:
   SgGuiVlbiBaselineItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    {blInfo_=NULL;};
  virtual ~SgGuiVlbiBaselineItem() {blInfo_=NULL;};
  void setBlInfo(SgVlbiBaselineInfo* info) {blInfo_ = info;};
  SgVlbiBaselineInfo* getBlInfo() {return blInfo_;};
  const SgVlbiBaselineInfo* getBlInfo() const {return blInfo_;};

  virtual bool operator<(const QTreeWidgetItem &other) const;
  
private:
  SgVlbiBaselineInfo *blInfo_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgGuiVlbiBaselineList : public QWidget
{
  Q_OBJECT
  
public:
  SgGuiVlbiBaselineList(SgVlbiSession *session, SgTaskConfig *cfg, 
    SgObjectBrowseMode, QMap<QString, SgVlbiBaselineInfo*>*,
    QWidget* = 0,  Qt::WindowFlags = 0);
  virtual ~SgGuiVlbiBaselineList() {session_=NULL; cfg_=NULL; baselinesByName_=NULL;};


public
slots:
  void addRefClockStation(const QString&);
  void delRefClockStation(const QString&);

private 
slots:
  void toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void updateContent();
  void updateContent4Nums();
  void entryClicked(QTreeWidgetItem*, int);
  void entryDoubleClicked(QTreeWidgetItem*, int);
  void modifyBaselineInfo(bool);
  

protected:
  SgVlbiSession                *session_;
  SgTaskConfig                 *cfg_;
  QString                       ownerName_;
  SgObjectBrowseMode            browseMode_;
  QMap<QString, SgVlbiBaselineInfo*>   
                               *baselinesByName_;
  SgGuiQTreeWidgetExt          *tweBaselines_;
  double                        scl4delay_;
  double                        scl4rate_;
  int                           constColumns_;
  QList<QString>                refClockStations_;
  QBrush                        bClockShiftStrong_;
  QBrush                        bClockShiftOk_;
  QBrush                        bClockShiftWeak_;
  QBrush                        bClockShiftBad_;
  int                           numbersBrowseMode_;
  virtual inline QString className() const {return "SgGuiVlbiBaselineList";};
};
/*=====================================================================================================*/



class QLineEdit;
class QCheckBox;

/*=====================================================================================================*/
class SgGuiVlbiBlnInfoEditor : public QDialog
{
  Q_OBJECT
public:
  SgGuiVlbiBlnInfoEditor(SgVlbiSession *session, SgTaskConfig *cfg, 
    SgVlbiBaselineInfo*, SgObjectBrowseMode, const QString&,
    QWidget* =0, Qt::WindowFlags=0);
  ~SgGuiVlbiBlnInfoEditor();
  inline QString className() const {return "SgGuiVlbiBlnInfoEditor";};

  signals:
  void          contentModified(bool);
  
  private 
  slots:
  void          accept();
  void          reject() {QDialog::reject(); isModified_=false; deleteLater();};
  void          updateModifyStatus(bool);

protected:
  SgVlbiSession                        *session_;
  SgTaskConfig                         *cfg_;
  SgObjectBrowseMode                    browseMode_;
  SgVlbiBaselineInfo                   *baselineInfo_;
  bool                                  isModified_;
  // widgets:
  QCheckBox                            *cbAttributes_[6];
  QLineEdit                            *leAuxSigma4Delay_;
  QLineEdit                            *leAuxSigma4Rate_;
  // functions:
  void                                  acquireData();
};
/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_GUI_VLBI_BASELINE_LIST_H
