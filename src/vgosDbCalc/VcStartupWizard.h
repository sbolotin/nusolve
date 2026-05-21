/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#ifndef VC_STARTUP_WIZARD_H
#define VC_STARTUP_WIZARD_H


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
class VcWizardPage : public QWizardPage
{
public:
  inline VcWizardPage(QWidget* parent=0) : QWizardPage(parent) {};
  inline void registerField(const QString& name, QWidget* widget, const char* property=0, 
                      const char* changedSignal=0)
  {QWizardPage::registerField(name, widget, property, changedSignal);};
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class VcStartupWizard : public QWizard
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VcStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun,
    QSettings *settings, QWidget *parent=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VcStartupWizard();



  //
  // Interfaces:
  //
  static int serialNumber();
  
  //
  // Functions:
  //
  inline QString className() const {return "VcStartupWizard";};
  
  VcWizardPage *createIntroPage();
  
  VcWizardPage *createPageHomeDir();

  VcWizardPage *createPageOne();

  VcWizardPage *createPageTwo();

  VcWizardPage *createPageThree();
  
  VcWizardPage *createConclusionPage();
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
  QLineEdit                    *leCalcAPrioriDirName_;
  QLineEdit                    *leMasterFilesDirName_;
  QCheckBox                    *cbUseAltMasterfileSuffixes_;
  QLineEdit                    *leAltMasterfileSuffixes_;
  QLineEdit                    *leFile4Sites_;
  QLineEdit                    *leFile4Sources_;
  QLineEdit                    *leFile4OceanLd_;
  QLineEdit                    *leFile4Eops_;
  QLineEdit                    *leFile4Tilts_;
  QLineEdit                    *leFile4OceanPoleTideLd_;
  QLineEdit                    *leAuxLogsDirName_;
  QCheckBox                    *cbMakeAuxLog_;
  QCheckBox                    *cbUseLocalLocale_;
  QCheckBox                    *cbUseOwnOnlyDatabase_;
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
#endif // VC_STARTUP_WIZARD_H
