/*
 *    This file is a part of vgosDbProcLogs. vgosDbProcLogs is a part of
 *    CALC/SOLVE system and is designed to extract data, meteo parameters
 *    and cable calibrations, from stations log files and store them in
 *    the vgosDb format.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
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

#ifndef VP_STARTUP_WIZARD_H
#define VP_STARTUP_WIZARD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QtGlobal>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QVBoxLayout>
#   include <QtWidgets/QWizard>
#   include <QtWidgets/QWizardPage>
#else
#   include <QtGui/QLabel>
#   include <QtGui/QVBoxLayout>
#   include <QtGui/QWizard>
#   include <QtGui/QWizardPage>
#endif






class QButtonGroup;
class QCheckBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QSettings;
class QTreeWidget;
class QTreeWidgetItem;




/*=====================================================================================================*/
class VpWizardPage : public QWizardPage
{
public:
  inline VpWizardPage(QWidget* parent=0) : QWizardPage(parent) {};
  inline void registerField(const QString& name, QWidget* widget, const char* property=0, 
                      const char* changedSignal=0)
  {QWizardPage::registerField(name, widget, property, changedSignal);};
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class VpStartupWizard : public QWizard
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VpStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun,
    QSettings *settings, QWidget *parent=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VpStartupWizard();



  //
  // Interfaces:
  //
  static int serialNumber();

  //
  // Functions:
  //
  inline QString className() const {return "VpStartupWizard";};
  
  VpWizardPage *createIntroPage();
  
  VpWizardPage *createPageHomeDir();

  VpWizardPage *createPageOne();

  VpWizardPage *createPageTwo();

  VpWizardPage *createPageThree();
  
  VpWizardPage *createPageDefaultCableSigns();

  VpWizardPage *createPageRinexData();

  VpWizardPage *createConclusionPage();
  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void accept();
  void editDefaultCableSign();
  void deleteDefaultCableSign();
  void insertDefaultCableSign();
  void editRinexData();
  void deleteRinexData();
  void insertRinexData();
  void changeAltMasterfileSuffixes(int chkState);


private:
  // other variables:
  bool                          isFirstRun_;
  bool                          isForcedRun_;
  bool                          isSystemWideRun_;

  // user identities:
  QLineEdit                    *leUserName_;
  QLineEdit                    *leUserEmail_;
  QLineEdit                    *leUserInitials_;
  QLineEdit                    *leAcFullName_;
  QLineEdit                    *leAcAbbrevName_;
  QLineEdit                    *leAcAbbName_;
  QLineEdit                    *leHomeDirName_;
  QLineEdit                    *leVgosDbDirName_;
  QLineEdit                    *leSessionDirName_;
  QLineEdit                    *leMasterFilesDirName_;
  QCheckBox                    *cbUseAltMasterfileSuffixes_;
  QLineEdit                    *leAltMasterfileSuffixes_;
  QCheckBox                    *cbUseLocalLocale_;
  QCheckBox                    *cbUseOwnOnlyDatabase_;
  QLineEdit                    *leAuxLogsDirName_;
  QCheckBox                    *cbMakeAuxLog_;
  // for logger:
  QCheckBox                    *cbLogStoreInFile_;
  QCheckBox                    *cbLogTimeLabel_;
  QLineEdit                    *eLogFileName_;
  QSpinBox                     *sLogCapacity_;
  QButtonGroup                 *bgLogLevels_;
  // 
  // default cable cal signs:
  QTreeWidget                  *twDefaultCableSigns_;
  // RINEX file name and pressure offset:
  QTreeWidget                  *twRinexData_;
  
  static const int              serialNumber_;
  QSettings                    *settings_;

protected:
};
/*=====================================================================================================*/






/*=====================================================================================================*/
class VpDefaultCableCalSignEditor : public QDialog
{
  Q_OBJECT
public:
  VpDefaultCableCalSignEditor(QMap<QString, int>* signByStation, const QString& stnName, int cblSign,
    QTreeWidget *twDefaultCableSigns, QTreeWidgetItem *wtItem, QWidget* =0, Qt::WindowFlags=0);
  inline ~VpDefaultCableCalSignEditor() {signByStation_=NULL; twDefaultCableSigns_=NULL; wtItem_=NULL;};
  inline QString className() const {return "VpDefaultCableCalSignEditor";};

  inline void setIsModified(bool is) {isModified_ = is;};

  private 
  slots:
  void accept();
  void reject() {QDialog::reject(); deleteLater();};

protected:
  QMap<QString, int>           *signByStation_;
  QString                       stnName_;
  int                           cblSign_;
  //
  QTreeWidget                  *twDefaultCableSigns_;
  QTreeWidgetItem              *wtItem_;
  bool                          isModified_;
  // widgets:
  QLineEdit                    *leStationName_;
  QLineEdit                    *leSign_;
  // functions:
  void                          acquireData();
};
/*=====================================================================================================*/







/*=====================================================================================================*/
class VpRinexDataEditor : public QDialog
{
  Q_OBJECT
public:
  VpRinexDataEditor(QMap<QString, QString>* fnameByStn, QMap<QString, double>* pressOffsetByStn,
    const QString& stnName, const QString rinexFname,
    double pressureOffset, QTreeWidget *twRinexData, QTreeWidgetItem *wtItem, 
    QWidget* =0, Qt::WindowFlags=0);
  inline ~VpRinexDataEditor() {rinexFileNameByStn_=NULL; rinexPressureOffsetByStn_=NULL; 
    twRinexData_=NULL; wtItem_=NULL;};
  inline QString className() const {return "VpRinexDataEditor";};

  inline void setIsModified(bool is) {isModified_ = is;};

  private 
  slots:
  void accept();
  void reject() {QDialog::reject(); deleteLater();};

protected:
  QMap<QString, QString>       *rinexFileNameByStn_;
  QMap<QString, double>        *rinexPressureOffsetByStn_;
  QString                       stnName_;
  QString                       rinexFileName_;
  double                        pressureOffset_;
//
  QTreeWidget                  *twRinexData_;
  QTreeWidgetItem              *wtItem_;
  bool                          isModified_;
  // widgets:
  QLineEdit                    *leStationKey_;
  QLineEdit                    *leRinexFileName_;
  QLineEdit                    *leRinexPressureOffset_;
  // functions:
  void                          acquireData();
};
/*=====================================================================================================*/








/*=====================================================================================================*/
#endif // VP_STARTUP_WIZARD_H
