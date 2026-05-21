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

#ifndef NS_SESSION_NAME_DIALOG_H
#define NS_SESSION_NAME_DIALOG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <QtCore/QtGlobal>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#else
#   include <QtGui/QDialog>
#endif


class QCheckBox;
class QLineEdit;

/*=====================================================================================================*/
class NsSessionNameDialog : public QDialog
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsSessionNameDialog(QWidget *parent, QString *fileName, QString *auxFileName=NULL, Qt::WindowFlags flags=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsSessionNameDialog();



  //
  // Interfaces:
  //



  //
  // Functions:
  //
  inline QString className() const {return "NsSessionNameDialog";};
  

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
  void changeAutoload(int);



private:
  QString                      *fileName_;
  QString                      *auxFileName_;
  QLineEdit                    *leDbName_;
  QLineEdit                    *leDbName2_;
  QCheckBox                    *cbAutoLoad2band_;

protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};
};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // NS_SESSION_NAME_DIALOG_H
