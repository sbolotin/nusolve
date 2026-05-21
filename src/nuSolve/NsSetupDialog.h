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

#ifndef NS_SETUP_DIALOG_H
#define NS_SETUP_DIALOG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include "NsSetup.h"


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#else
#   include <QtGui/QDialog>
#endif



class QCheckBox;
class QComboBox;
class QLineEdit;
class QTabWidget;
class SgGuiLoggerConfig;





/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsSetupDialog : public QDialog
{
  Q_OBJECT

public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsSetupDialog(QWidget *parent=0, Qt::WindowFlags flags=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsSetupDialog();



  //
  // Interfaces:
  //



  //
  // Functions:
  //
  inline QString className() const {return "NsSetupDialog";};

 
  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void apply();
  void accept();
  void reject();
  void modifyPlotterOutputFormat(int);
  void modifyAutosaveMode(int);
  void changeAltMasterfileSuffixes(int chkState);
  


protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};
  void acquireData();



private:
  // functions:
  QWidget*                    tab4Pathes();
  QWidget*                    tab4Options();
  QWidget*                    tab4Identities();
  
  QTabWidget                 *tabs;
  SgGuiLoggerConfig          *logConfigWidget_;
  QLineEdit                  *lePath2Home_;
  QLineEdit                  *lePath2CatNu_;
  QLineEdit                  *lePath2Dbh_;
  QLineEdit                  *lePath2VgosDb_;
  QLineEdit                  *lePath2VgosDa_;
  QLineEdit                  *lePath2Apriori_;
  QLineEdit                  *lePath2TrpFiles_;
  QLineEdit                  *lePath2Masterfiles_;
  QCheckBox                  *cbUseAltMasterfileSuffixes_;
  QLineEdit                  *leAltMasterfileSuffixes_;
  QLineEdit                  *lePath2Spoolfiles_;
  QLineEdit                  *lePath2NotUsedObs_;
  QLineEdit                  *lePath2Reports_;
  QLineEdit                  *lePath2NgsOutput_;
  QLineEdit                  *lePath2PlotterOutput_;
  QLineEdit                  *lePath2IntermediateResults_;
  QLineEdit                  *lePath2AuxLogs_;
  QCheckBox                  *cbHave2UpdateCatalog_;
  QCheckBox                  *cbHave2MaskSessionCode_;
  QCheckBox                  *cbHave2KeepSpoolFileReports_;
  QCheckBox                  *cbHave2KeepUnusedObsList_;
  QCheckBox                  *cbHave2WarnCloseWindow_;
  QCheckBox                  *cbIsShortScreen_;
  QCheckBox                  *cbHave2AutoloadAllBands_;
  QCheckBox                  *cbMakeAuxLog_;
  QComboBox                  *cbAutosaveMode_;
  
  QCheckBox                  *cbExecExternalCommand_;
  QLineEdit                  *leExternalCommand_;

  QCheckBox                  *cbIsBandPlotPerSrcView_;
  QCheckBox                  *cbUseOwnOnlyDatabase_;
  
  // user identities:
  QLineEdit                  *leUserName_;
  QLineEdit                  *leUserEmail_;
  QLineEdit                  *leUserInitials_;
  QLineEdit                  *leAcFullName_;
  QLineEdit                  *leAcAbbrevName_;
  QLineEdit                  *leAcAbbName_;
  
};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // NS_SETUP_DIALOG_H
