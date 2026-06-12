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

#ifndef SG_GUI_PARAMETER_CFG_H
#define SG_GUI_PARAMETER_CFG_H

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <SgParameterCfg.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QDialog>
#else
#   include <QtGui/QDialog>
#endif




class QButtonGroup;
class QLineEdit;
class QRadioButton;
class QTabWidget;
//class SgParametersDescriptor;

class SgVlbiSession;


/*=====================================================================================================*/
class SgGuiParameterCfg : public QDialog
{
  Q_OBJECT

public:
  //
  SgGuiParameterCfg(SgParameterCfg *parConfig, int parIdx, 
                    SgVlbiSession* session, bool isModeAdjustable=false,
                    QWidget* parent=0, Qt::WindowFlags f=Qt::Widget);
  // 
  virtual inline ~SgGuiParameterCfg() {parConfig_=NULL; emit windowClosed();};
  //
  inline QString className() const {return "SgGuiParameterCfg";};


signals:
  void windowClosed();
  void valueModified(bool);
  
private slots:
  void accept();
  void apply();
  void reject() {QDialog::reject(); emit valueModified(false); deleteLater();};
  void setDefault();
  void stochasticTypeChanged(int);
  void arcBoundaryChanged(int idx);
  void changeArcBoundary();
  void pwlBoundaryChanged(int idx);
  void changePwlBoundary();


protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};


private:
  // data part:
  int                         parameterIdx_;
  SgParameterCfg             *parConfig_;
  SgParameterCfg             *parConfigOrig_;
  bool                        isModified_;
  bool                        isModeAdjustable_;
  // GUI part:
  QString                     str_;
  //
  QLineEdit                  *leConvApriori_;
  // Arc:
  QLineEdit                  *leArcLength_;
  QButtonGroup               *bgArcBoundary_;
  QLineEdit                  *leArcBoundary_;
  // PWL:    
  QLineEdit                  *lePwlApriori_;
  QLineEdit                  *lePwlLength_;
  QButtonGroup               *bgPwlBoundary_;
  QLineEdit                  *lePwlBoundary_;
  // Stc:  
  QLineEdit                  *leStcApriori_;
  QLineEdit                  *leRWNPower_;
  QLineEdit                  *leTau_;
  QTabWidget                 *tabs4types_;
  QRadioButton               *rbSType_[3];
  //
  SgVlbiSession              *session_;

  // functions:
  virtual void                acquireData();
  virtual void                browseData();

  QWidget*                    tab4RegularType();
  QWidget*                    tab4ArcType();
  QWidget*                    tab4PWLType();
  QWidget*                    tab4StochasticType();
};
/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_GUI_PARAMETER_CFG_H
