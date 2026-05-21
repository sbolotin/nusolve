/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2014-2020 Sergei Bolotin.
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

#ifndef SG_GUI_PIA_REPORT_H
#define SG_GUI_PIA_REPORT_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include <SgParameterCfg.h>
#include <SgParametersDescriptor.h>
#include <SgTaskConfig.h>
#include <SgVlbiSession.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#else
#   include <QtGui/QDialog>
#endif





/*=====================================================================================================*/
class SgGuiPiaReport : public QDialog
{
  Q_OBJECT

public:
  SgGuiPiaReport(SgTaskConfig*, SgParametersDescriptor*, SgVlbiSession*, 
    QWidget* =NULL, Qt::WindowFlags =0);
  virtual ~SgGuiPiaReport();
  inline QString className() const {return "SgGuiPiaReport";};

signals:
  void windowClosed();
  
private slots:
  void accept();

protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};

private:
  SgTaskConfig               *config_;
  SgParametersDescriptor     *parametersDescriptor_;
  SgVlbiSession              *session_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
#endif // SG_GUI_PIA_REPORT_H
