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

#ifndef SG_GUI_LOGGER_CONFIG_H
#define SG_GUI_LOGGER_CONFIG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <SgLogger.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QWidget>
#else
#   include <QtGui/QWidget>
#endif


class QCheckBox;
class QDialog;
class QLineEdit;
class QSpinBox;


/*=====================================================================================================*/
class SgGuiLoggerConfig : public QWidget
{
  Q_OBJECT

public:
   SgGuiLoggerConfig(QWidget*, Qt::WindowFlags=Qt::Widget);
  virtual inline ~SgGuiLoggerConfig(){};
  inline QString className() const {return "SgGuiLoggerConfig";};

//signals:
//  void configIsModified();

  void acquireData();

public slots:

private slots:

private:
  QCheckBox                    *cbLogStoreInFile_;
  QCheckBox                    *cbLogTimeLabel_;
  QCheckBox                    *cbFullDate_;
  QLineEdit                    *eLogFileName_;
  QSpinBox                     *sLogCapacity_;
  QCheckBox                    *cbLLevel_[4][32];
};
/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_GUI_LOGGER_CONFIG_H
