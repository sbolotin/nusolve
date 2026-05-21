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

#ifndef NS_STARTUP_WIZARD_H
#define NS_STARTUP_WIZARD_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


//#include <QtCore/QVersionNumber>
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



class QCheckBox;
class QLineEdit;


class NsWizardPage : public QWizardPage
{
public:
  inline NsWizardPage(QWidget* parent=0) : QWizardPage(parent) {};
  inline void registerField(const QString& name, QWidget* widget, const char* property=0, 
                      const char* changedSignal=0)
  {QWizardPage::registerField(name, widget, property, changedSignal);};
};



/*=====================================================================================================*/
class NsStartupWizard : public QWizard
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun, QWidget *parent=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsStartupWizard();



  //
  // Interfaces:
  //
  static int serialNumber();


  //
  // Functions:
  //
  inline QString className() const {return "NsStartupWizard";};
  
  NsWizardPage *createIntroPage();
  
  NsWizardPage *createPageHomeDir();

  NsWizardPage *createPageOne();

  NsWizardPage *createPageTwo();
  
  NsWizardPage *createConclusionPage();
  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void accept();

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
  QLineEdit                    *leAuxLogsDirName_;
  QCheckBox                    *cbMakeAuxLog_;
  //
  QLineEdit                    *lePath2CatNu_;
  QLineEdit                    *lePath2Dbh_;
  QLineEdit                    *lePath2VgosDb_;
  QLineEdit                    *lePath2VgosDa_;
  QLineEdit                    *lePath2Apriori_;
  QLineEdit                    *lePath2Masterfiles_;
  QLineEdit                    *lePath2Spoolfiles_;
  QLineEdit                    *lePath2NotUsedObs_;
  QLineEdit                    *lePath2Reports_;
  QLineEdit                    *lePath2NgsOutput_;
  QLineEdit                    *lePath2PlotterOutput_;
  QLineEdit                    *lePath2IntermediateResults_;
 
  
  static const int              serialNumber_;

protected:
};
/*=====================================================================================================*/







/*=====================================================================================================*/
#endif // NS_STARTUP_WIZARD_H
