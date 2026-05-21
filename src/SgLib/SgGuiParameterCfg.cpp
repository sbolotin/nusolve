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

#include <SgGuiParameterCfg.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QGridLayout>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#   include <QtWidgets/QTabWidget>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QGridLayout>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QPushButton>
#   include <QtGui/QRadioButton>
#   include <QtGui/QTabWidget>
#endif


#include <SgParametersDescriptor.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*              SgGuiParameterCfg's METHODS:
* 
*======================================================================================================*/
//
SgGuiParameterCfg::SgGuiParameterCfg(SgParameterCfg *pCfg, int idx, bool isModeAdjustable,
                                     QWidget* parent, Qt::WindowFlags f)
  : QDialog(parent, f),
    str_()
{
  parameterIdx_ = idx;
  parConfigOrig_ = pCfg;
  parConfig_ = new SgParameterCfg(*parConfigOrig_);
  isModified_ = false;
  isModeAdjustable_ = isModeAdjustable;
  setWindowTitle("Parameter Editor: " + parConfig_->getName());

  QBoxLayout                   *layout, *subLayout;
  QSize                         btnSize;
  tabs4types_ = new QTabWidget(this);
  layout = new QVBoxLayout(this);
  tabs4types_->addTab(tab4RegularType(),    "&Regular");
  tabs4types_->addTab(tab4ArcType(),        "&Arc");
  tabs4types_->addTab(tab4PWLType(),        "&PWL");
  tabs4types_->addTab(tab4StochasticType(), "&Stochastic");

  switch (parConfig_->getPMode())
  {
  default:
  case SgParameterCfg::PM_NONE:
    tabs4types_->setCurrentIndex(0);
    break;
  case SgParameterCfg::PM_ARC:
    tabs4types_->setCurrentIndex(1);
    break;
  case SgParameterCfg::PM_PWL:
    tabs4types_->setCurrentIndex(2);
    break;
  case SgParameterCfg::PM_STC:
    tabs4types_->setCurrentIndex(3);
    break;
  };
  
  layout->addWidget(tabs4types_, 10);
  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  
  QPushButton *btnApply   = new QPushButton("Apply", this);
  QPushButton *btnOk      = new QPushButton("OK", this);
  QPushButton *btnCancel  = new QPushButton("Cancel", this);
  QPushButton *btnDefault = new QPushButton("Default", this);
  btnOk->setDefault(true);
  btnDefault->setMinimumSize((btnSize=btnDefault->sizeHint()));
  btnCancel ->setMinimumSize( btnSize );
  btnOk     ->setMinimumSize( btnSize );
  btnApply  ->setMinimumSize( btnSize );

  subLayout->addWidget(btnApply);
  subLayout->addWidget(btnDefault);
  subLayout->addStretch(1);
  subLayout->addWidget(btnOk);
  subLayout->addWidget(btnCancel);
  setSizeGripEnabled(true);

  connect(btnApply,  SIGNAL(clicked()), SLOT(apply()));
  connect(btnDefault,SIGNAL(clicked()), SLOT(setDefault()));
  connect(btnOk,     SIGNAL(clicked()), SLOT(accept()));
  connect(btnCancel, SIGNAL(clicked()), SLOT(reject()));
};



//
void SgGuiParameterCfg::apply()
{
  acquireData();
  if (isModified_)
    *parConfigOrig_ = *parConfig_;
  emit valueModified(isModified_);
};



//
void SgGuiParameterCfg::accept()
{
  acquireData();
  if (isModified_)
    *parConfigOrig_ = *parConfig_;
  emit valueModified(isModified_);
  QDialog::accept();
  deleteLater();
};



//
void SgGuiParameterCfg::setDefault()
{
  SgParameterCfg::PMode pm=parConfig_->getPMode();
  *parConfig_ = SgParametersDescriptor().getParameter(parameterIdx_);
  parConfig_->setPMode(pm);
  isModified_ = true;
  browseData();
};



//
void SgGuiParameterCfg::browseData()
{
  leConvApriori_->setText(str_.sprintf("%.4f", parConfig_->getConvAPriori()));
  leArcLength_->setText(str_.sprintf("%.4f", 24.0*parConfig_->getArcStep()));
  lePwlApriori_->setText(str_.sprintf("%.4f", parConfig_->getPwlAPriori()/24.0));
  lePwlLength_->setText(str_.sprintf("%.4f", 24.0*parConfig_->getPwlStep()));
  leStcApriori_->setText(str_.sprintf("%.4f", parConfig_->getStocAPriori()));
  leRWNPower_->setText(str_.sprintf("%.4f", parConfig_->getWhiteNoise()));
  leTau_->setText(str_.sprintf("%.4f", parConfig_->getTau()));
  rbSType_[(int)parConfig_->getSType()]->setChecked(true);
};



//
void SgGuiParameterCfg::acquireData()
{
  bool                          isOK;
  double                        d;
  SgParameterCfg::SType         type=SgParameterCfg::ST_RANDWALK;
  // conventional parameter:
  str_ = leConvApriori_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getConvAPriori())
  {
    isModified_ = true;
    parConfig_->setConvAPriori(d);
  };
  // arc parameter:
  str_ = leArcLength_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getArcStep()*24.0)
  {
    isModified_ = true;
    parConfig_->setArcStep(d/24.0);
  };
  // PWL parameter:
  str_ = lePwlApriori_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getPwlAPriori()/24.0)
  {
    isModified_ = true;
    parConfig_->setPwlAPriori(d*24.0);
  };
  str_ = lePwlLength_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getPwlStep()*24.0)
  {
    isModified_ = true;
    parConfig_->setPwlStep(d/24.0);
  };
  // stochastic parameter:
  str_ = leStcApriori_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getStocAPriori())
  {
    isModified_ = true;
    parConfig_->setStocAPriori(d);
  };
  str_ = leRWNPower_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getWhiteNoise())
  {
    isModified_ = true;
    parConfig_->setWhiteNoise(d);
  };
  str_ = leTau_->text();
  d = str_.toDouble(&isOK);
  if (isOK && d!=parConfig_->getTau())
  {
    isModified_ = true;
    parConfig_->setTau(d);
  };
  if (rbSType_[0]->isChecked())
    type = SgParameterCfg::ST_WHITENOISE;
  else if (rbSType_[1]->isChecked())
    type = SgParameterCfg::ST_MARKOVPROCESS;
  else if (rbSType_[2]->isChecked())
    type = SgParameterCfg::ST_RANDWALK;
  if (type != parConfig_->getSType())
  {
    isModified_ = true;
    parConfig_->setSType(type);
  };
  //
  SgParameterCfg::PMode         oldPMode=parConfig_->getPMode();
  if (isModeAdjustable_)
    switch (tabs4types_->currentIndex())
    {
    default:
    case 0:
      parConfig_->setPMode(SgParameterCfg::PM_NONE);
      break;
    case 1:
      parConfig_->setPMode(SgParameterCfg::PM_ARC);
      break;
    case 2:
      parConfig_->setPMode(SgParameterCfg::PM_PWL);
      break;
    case 3:
      parConfig_->setPMode(SgParameterCfg::PM_STC);
      break;
    };
  if (parConfig_->getPMode() != oldPMode)
    isModified_ = true;
};



//
QWidget* SgGuiParameterCfg::tab4RegularType()
{
  QWidget      *w=new QWidget(this);
  QBoxLayout   *layout=new QVBoxLayout(w);
  QGroupBox    *gbox=new QGroupBox("Conventional Parameter", w);
  layout->addWidget(gbox);
  QGridLayout  *grid=new QGridLayout(gbox);
  QLabel       *label=new QLabel("A priori (initial) sigma (" + parConfig_->getScaleName() + "):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);
  
  leConvApriori_ = new QLineEdit(gbox);
  leConvApriori_->setText(str_.sprintf("%.4f", parConfig_->getConvAPriori()));
  leConvApriori_->setMinimumSize(leConvApriori_->sizeHint());
  grid->addWidget(leConvApriori_, 0, 1);

  return w;
};



//
QWidget* SgGuiParameterCfg::tab4ArcType()
{
  QWidget      *w=new QWidget(this);
  QBoxLayout   *layout=new QVBoxLayout(w);
  QGroupBox    *gbox=new QGroupBox("Arc Parameter", w);
  layout->addWidget(gbox);
  QGridLayout  *grid=new QGridLayout(gbox);
  QLabel       *label=new QLabel("Arc length (hrs):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);
  
  leArcLength_ = new QLineEdit(gbox);
  leArcLength_->setText(str_.sprintf("%.4f", 24.0*parConfig_->getArcStep()));
  leArcLength_->setMinimumSize(leArcLength_->sizeHint());
  grid->addWidget(leArcLength_, 0, 1);

  return w;
};



//
QWidget* SgGuiParameterCfg::tab4PWLType()
{
  QWidget      *w=new QWidget(this);
  QBoxLayout   *layout=new QVBoxLayout(w);
  QGroupBox    *gbox=new QGroupBox("PieceWise Linear Parameter", w);
  layout->addWidget(gbox);
  QGridLayout  *grid=new QGridLayout(gbox);
//"<p>&sigma;</p>"
  QLabel       *label=new QLabel("A priori sigmas for rate terms (" +
                                    parConfig_->getScaleName() + "/hr):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->setRowStretch(0, 1);
  grid->addWidget(label, 1, 0);
  //
  lePwlApriori_ = new QLineEdit(gbox);
  lePwlApriori_->setText(str_.sprintf("%.4f", parConfig_->getPwlAPriori()/24.0));
  lePwlApriori_->setMinimumSize(lePwlApriori_->sizeHint());
  grid->addWidget(lePwlApriori_, 1, 1);

  label = new QLabel("Interval length (hrs):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2, 0);
  //
  lePwlLength_ = new QLineEdit(gbox);
  lePwlLength_->setText(str_.sprintf("%.4f", 24.0*parConfig_->getPwlStep()));
  lePwlLength_->setMinimumSize(lePwlLength_->sizeHint());
  grid->addWidget(lePwlLength_, 2, 1);
  grid->setRowStretch(3, 1);
  
  return w;
};



//
QWidget* SgGuiParameterCfg::tab4StochasticType()
{
 static const QString typeNames[] = 
  {
    QString("White noise"),
    QString("2-nd order Markov process"),
    QString("Random Walk"),
  };
  QWidget      *w=new QWidget(this);
  QBoxLayout   *layout=new QVBoxLayout(w);
  QGroupBox    *gbox=new QGroupBox("Stochastic Parameter", w);
  layout->addWidget(gbox);
  QGridLayout  *grid=new QGridLayout(gbox);

  QLabel       *label=new QLabel("A priori (initial) sigma (" + parConfig_->getScaleName() + "):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);
  //
  leStcApriori_ = new QLineEdit(gbox);
  leStcApriori_->setText(str_.sprintf("%.4f", parConfig_->getStocAPriori()));
  leStcApriori_->setMinimumSize(leStcApriori_->sizeHint());
  grid->addWidget(leStcApriori_, 0, 1);

  label = new QLabel("Ruled White Noise Power (" + parConfig_->getScaleName() + "/sqrt(hr)):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 1, 0);
  //
  leRWNPower_ = new QLineEdit(gbox);
  leRWNPower_->setText(str_.sprintf("%.4f", parConfig_->getWhiteNoise()));
  leRWNPower_->setMinimumSize(leRWNPower_->sizeHint());
  grid->addWidget(leRWNPower_, 1, 1);

  label = new QLabel("Time of relaxation (hr):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2, 0);
  //
  leTau_ = new QLineEdit(gbox);
  leTau_->setText(str_.sprintf("%.4f", parConfig_->getTau()));
  leTau_->setMinimumSize(leTau_->sizeHint());
  grid->addWidget(leTau_, 2, 1);

  gbox = new QGroupBox("Type of stochastic modelling", gbox);
  layout = new QVBoxLayout(gbox);
  QButtonGroup  *gr=new QButtonGroup(gbox);
  for (int i=0; i<3; i++)
  {
    rbSType_[i] = new QRadioButton(typeNames[i], gbox);
    rbSType_[i]->setMinimumSize(rbSType_[i]->sizeHint());
    layout->addWidget(rbSType_[i]);
    gr->addButton(rbSType_[i], i);
  };
  grid->addWidget(gbox, 3,0, 2,1);
  connect (gr, SIGNAL(buttonClicked(int)), SLOT(stochasticTypeChanged(int)));
//  rbSType_[(int)config_.getSType()]->setChecked(true);
  rbSType_[(int)parConfig_->getSType()]->click();
  return w;
};



//
void SgGuiParameterCfg::stochasticTypeChanged(int id)
{
  switch (id)
  {
    case 0:
      leRWNPower_->setEnabled(false);
      leTau_->setEnabled(false);
    break;
    case 1:
      leRWNPower_->setEnabled(true);
      leTau_->setEnabled(true);
    break;
    case 2:
    default:
      leRWNPower_->setEnabled(true);
      leTau_->setEnabled(false);
    break;
  };
};
/*=====================================================================================================*/




/*=====================================================================================================*/
