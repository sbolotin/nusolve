/*
 *
 *    This file is a part of vgosDbMake. vgosDbMake is a part of CALC/SOLVE
 *    system and is designed to convert correlator output data into VgosDb format.
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

#ifndef VM_STARTUP_WIZARD_H
#define VM_STARTUP_WIZARD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <QtCore/QtGlobal>

#if QT_VERSION >= 0x050000
#    include <QtWidgets/QLabel>
#    include <QtWidgets/QVBoxLayout>
#    include <QtWidgets/QWizard>
#    include <QtWidgets/QWizardPage>
#else
#    include <QtGui/QLabel>
#    include <QtGui/QVBoxLayout>
#    include <QtGui/QWizard>
#    include <QtGui/QWizardPage>
#endif





class QButtonGroup;
class QCheckBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QSettings;




/*=====================================================================================================*/
class VmWizardPage : public QWizardPage
{
public:
  inline VmWizardPage(QWidget* parent=0) : QWizardPage(parent) {};
  inline void registerField(const QString& name, QWidget* widget, const char* property=0, 
                            const char* changedSignal=0)
    {QWizardPage::registerField(name, widget, property, changedSignal);};
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class VmStartupWizard : public QWizard
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VmStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun,
    QSettings *settings, QWidget *parent=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VmStartupWizard();



  //
  // Interfaces:
  //
  static int serialNumber();

  //
  // Functions:
  //
  inline QString className() const {return "VmStartupWizard";};
  
  VmWizardPage *createIntroPage();
  
  VmWizardPage *createPageHomeDir();

  VmWizardPage *createPageOne();

  VmWizardPage *createPageTwo();

  VmWizardPage *createPageThree();
  
  VmWizardPage *createConclusionPage();
  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void accept();
  void changeAltMasterfileSuffixes(int chkState);


private:
  // aux classes :
  /*
  // pages:
  IntroPage                    *introPage_;
  UserIdentitiesPage           *userIdentitiesPage_;
  AcIdentitiesPage             *acIdentitiesPage_;
  FinalPage                    *finalPage_;
  */

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
  QLineEdit                    *leInputDirName_;
  QLineEdit                    *leMasterFilesDirName_;
  QCheckBox                    *cbUseAltMasterfileSuffixes_;
  QLineEdit                    *leAltMasterfileSuffixes_;
  QCheckBox                    *cbUseLocalLocale_;
  QLineEdit                    *leAuxLogsDirName_;
  QCheckBox                    *cbMakeAuxLog_;
  // for logger:
  QCheckBox                    *cbLogStoreInFile_;
  QCheckBox                    *cbLogTimeLabel_;
  QLineEdit                    *eLogFileName_;
  QSpinBox                     *sLogCapacity_;
  QButtonGroup                 *bgLogLevels_;
  //
  static const int              serialNumber_;
  QSettings                    *settings_;

protected:
};
/*=====================================================================================================*/








/*=====================================================================================================*/
#endif // VM_STARTUP_WIZARD_H
