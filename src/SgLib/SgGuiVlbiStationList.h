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


#ifndef SG_GUI_VLBI_STATION_LIST_H
#define SG_GUI_VLBI_STATION_LIST_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <SgVlbiStationInfo.h>
#include <SgGuiQTreeWidgetExt.h>



#if QT_VERSION >= 0x050000
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QDialog>
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QSpinBox>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QCheckBox>
#   include <QtGui/QDialog>
#   include <QtGui/QHeaderView>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QSpinBox>
#   include <QtGui/QTreeWidgetItem>
#endif

#include <QtGui/QFontMetrics>











/*=====================================================================================================*/
class SgGuiVlbiStationItem : public QTreeWidgetItem
{
public:
   SgGuiVlbiStationItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    {stInfo_=NULL;};
  virtual ~SgGuiVlbiStationItem() {stInfo_=NULL;};
  void setStInfo(SgVlbiStationInfo* info) {stInfo_ = info;};
  SgVlbiStationInfo* getStInfo() {return stInfo_;};
  virtual bool operator<(const QTreeWidgetItem &other) const;

private:
  SgVlbiStationInfo *stInfo_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class SgGuiVlbiStationList : public QWidget
{
  Q_OBJECT
public:
  SgGuiVlbiStationList(SgObjectBrowseMode, const QString&, QMap<QString, SgVlbiStationInfo*>*,
    const SgMJD& t=tZero, QWidget* =0, Qt::WindowFlags=0);
  virtual ~SgGuiVlbiStationList() {stationsByName_=NULL;};
  const SgMJD& getT0() const {return t0_;};
  void setT0(const SgMJD& t) {t0_=t;};
  virtual inline QString className() const {return "SgGuiVlbiStationList";};

signals:
  void refClockStationSelected(const QString&);
  void refClockStationDeselected(const QString&);

private 
slots:
  void toggleEntryMoveEnable(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void entryClickedWithButton(QTreeWidgetItem*, int, Qt::MouseButton);
  void entryDoubleClicked(QTreeWidgetItem*, int);
  void updateContent();
  void updateContent4Nums();
  void modifyStationInfo(bool);


protected:
  QString                       ownerName_;
  SgObjectBrowseMode            browseMode_;
  QMap<QString, SgVlbiStationInfo*>
                               *stationsByName_;
  SgGuiQTreeWidgetExt          *tweStations_;
  double                        scl4delay_;
  double                        scl4rate_;
  SgMJD                         t0_;
  int                           numbersBrowseMode_;
};
/*=====================================================================================================*/






// Aux objects (options and clock breaks editing):
//
/*=====================================================================================================*/
class SgGuiVlbiClockBreakItem : public QTreeWidgetItem
{
public:
   SgGuiVlbiClockBreakItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type)
    {clockBreak_=NULL;};
  virtual ~SgGuiVlbiClockBreakItem() {clockBreak_=NULL;};
  void setClockBreak(SgParameterBreak* clockBreak) {clockBreak_ = clockBreak;};
  SgParameterBreak* getClockBreak() {return clockBreak_;};
  
private:
  SgParameterBreak *clockBreak_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class SgGuiVlbiStnInfoEditor : public QDialog
{
  Q_OBJECT
public:
  SgGuiVlbiStnInfoEditor(SgVlbiStationInfo*, SgObjectBrowseMode, const QString&,
    const SgMJD&, QWidget* =0, Qt::WindowFlags=0);
  ~SgGuiVlbiStnInfoEditor();
  inline QString className() const {return "SgGuiVlbiStnInfoEditor";};

  signals:
  void          contentModified(bool);
  
  private
  slots:
  void          accept();
  void          reject() {QDialog::reject(); isModified_=false; deleteLater();};
  void          editClockBreakRecord();
  void          deleteClockBreakRecord();
  void          insertClockBreakRecord();
  void          editClockBreakRecordItem(QTreeWidgetItem*, int);
  void          updateModifyStatus(bool);
  void          updateLClocksMode(bool);
  void          updateLZenithMode(bool);
  void          addNewClockBreak(SgParameterBreak*);
  void          editLocalClocks();
  void          editLocalZenith();
  void          flybySourceCccFModified(int n);


protected:
  SgObjectBrowseMode            browseMode_;
  SgVlbiStationInfo            *stationInfo_;
  SgBreakModel                  bModel_;
  SgMJD                         t0_;
  bool                          isModified_;
  // widgets:
  QCheckBox                    *cbAttributes_[7];
  QTreeWidget                  *twClockBreaks_;
  QCheckBox                    *cbUseAPrioriClocks_;
  QLineEdit                    *leAPrioriClockTerm_0_;
  QLineEdit                    *leAPrioriClockTerm_1_;
  QSpinBox                     *sbOrder_;
  QCheckBox                    *cbUseLocalClocks_;
  QCheckBox                    *cbUseLocalZenith_;
  SgTaskConfig::CableCalSource  flybyCableCalSource_;
  //
  // functions:
  void                          acquireData();
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class QLineEdit;
class SgGuiVlbiStnClockBreakEditor : public QDialog
{
  Q_OBJECT
public:
//  SgGuiVlbiStnClockBreakEditor(SgGuiVlbiClockBreakItem*, QWidget* =0, Qt::WindowFlags=0);
  SgGuiVlbiStnClockBreakEditor(SgBreakModel*, SgObjectBrowseMode, SgGuiVlbiClockBreakItem*,
    SgVlbiStationInfo* si,
    QWidget* =0, Qt::WindowFlags=0);
  inline ~SgGuiVlbiStnClockBreakEditor() {clockBreak_=NULL; cbItem_=NULL;};
  inline QString className() const {return "SgGuiVlbiStnClockBreakEditor";};

  inline void setIsModified(bool is) {isModified_ = is;};

  signals:
  void clockBreakModified(bool);
  void clockBreakCreated(SgParameterBreak*);

  private 
  slots:
  void accept();
  void reject();

protected:
  SgObjectBrowseMode            browseMode_;
  SgBreakModel                 *cbModel_;
  SgParameterBreak             *clockBreak_;
  SgGuiVlbiClockBreakItem      *cbItem_;
  bool                          isModified_;
  bool                          isNewClockBreak_;
  SgVlbiStationInfo            *stationInfo_;
  // widgets:
  QLineEdit                    *leYr_;
  QLineEdit                    *leMn_;
  QLineEdit                    *leDy_;
  QLineEdit                    *leHr_;
  QLineEdit                    *leMi_;
  QLineEdit                    *leSc_;
  QLineEdit                    *leA0_;
  QLineEdit                    *leA1_;
  QLineEdit                    *leA2_;
  QLineEdit                    *leS0_;
  QLineEdit                    *leS1_;
  QLineEdit                    *leS2_;
  QCheckBox                    *cbIsDynamic_;
  // functions:
  bool                          acquireData();
};
/*=====================================================================================================*/


/*=====================================================================================================*/
#endif // SG_GUI_VLBI_STATION_LIST_H
