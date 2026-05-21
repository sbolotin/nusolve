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

#include "nuSolve.h"
#include "NsTestFour1Dialog.h"

#include <iostream>
#include <stdlib.h>


#include <SgCubicSpline.h>
#include <SgGuiPlotter.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgMathSupport.h>
#include <SgVector.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QFrame>
#else
#   include <QtGui/QLabel>
#   include <QtGui/QGroupBox>
#   include <QtGui/QBoxLayout>
#   include <QtGui/QPushButton>
#   include <QtGui/QFrame>
#endif



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsTestFour1Dialog::NsTestFour1Dialog(QWidget *parent, Qt::WindowFlags flags) 
  : QDialog(parent, flags)
{
  QBoxLayout           *Layout, *SubLayout;
  QSize                  BtnSize;

  Layout = new QVBoxLayout(this);
  Layout->addWidget(testPlot(), 10);
  SubLayout = new QHBoxLayout();
  Layout->addLayout(SubLayout);
  SubLayout->addStretch(1);
  
  QPushButton          *Ok=new QPushButton("OK", this);
  QPushButton          *Cancel=new QPushButton("Cancel", this);
  Ok->setDefault(true);

  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok    ->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Cancel);

  connect(Ok,     SIGNAL(clicked()), SLOT(accept()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  setWindowTitle("Test Dialog");
};



NsTestFour1Dialog::~NsTestFour1Dialog()
{
  if (carrier_)
  {
    delete carrier_;
    carrier_ = NULL;
  }
};



void NsTestFour1Dialog::accept()
{
  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": accept selected");
  deleteLater();
};



//
void NsTestFour1Dialog::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": reject selected");
  deleteLater();
};



//
QWidget* NsTestFour1Dialog::testPlot()
{
  SgPlotBranch                 *branch;
  int                           numOfData;
  SgVector                     *vecR, *vecI;
  std::complex<double>         *x_a;
  std::complex<double>         *x_a_rev;
  std::complex<double>         *x_A;
  double                        t;

  numOfData = 256;
  vecR = new SgVector(numOfData);
  vecI = new SgVector(numOfData);


  for (int i=0; i<numOfData; i++)
  {
    t = double(i)/double(numOfData);
    vecR->setElement(i,
      1.0*sin(2.0*M_PI*t/0.50) + 
      0.8*sin(2.0*M_PI*t/0.04) +
      1.8*sin(2.0*M_PI*t/0.12)
    );
    vecI->setElement(i,
      1.0*sin(2.0*M_PI*t/0.32) + 
      1.5*sin(2.0*M_PI*t/0.025) +
      0.8*sin(2.0*M_PI*t/0.8)
    );
  };

/*
  vecR->setElement(0, 1.0);
  vecR->setElement(1, 1.0);
  vecR->setElement(2, 1.0);
  vecR->setElement(3, 1.0);
  vecR->setElement(4, 1.0);
  vecR->setElement(5, 1.0);
  vecR->setElement(6, 1.0);
  vecR->setElement(7, 1.0);
*/

/*
  vecR->setElement( 0, 0.056533*cos((-174.916)*DEG2RAD));
  vecI->setElement( 0, 0.056533*sin((-174.916)*DEG2RAD));
  vecR->setElement( 1, 0.059263*cos((-129.290)*DEG2RAD));
  vecI->setElement( 1, 0.059263*sin((-129.290)*DEG2RAD));
  vecR->setElement( 4, 0.025482*cos(( -13.035)*DEG2RAD));
  vecI->setElement( 4, 0.025482*sin(( -13.035)*DEG2RAD));
  vecR->setElement(10, 0.062434*cos(( -75.062)*DEG2RAD));
  vecI->setElement(10, 0.062434*sin(( -75.062)*DEG2RAD));
  vecR->setElement(21, 0.051808*cos(( 120.126)*DEG2RAD));
  vecI->setElement(21, 0.051808*sin(( 120.126)*DEG2RAD));
  vecR->setElement(29, 0.058286*cos((  87.796)*DEG2RAD));
  vecI->setElement(29, 0.058286*sin((  87.796)*DEG2RAD));
  vecR->setElement(34, 0.060018*cos(( 129.865)*DEG2RAD));
  vecI->setElement(34, 0.060018*sin(( 129.865)*DEG2RAD));
*/
/*
t207 channel[ 0]: Ref: Amp=(l:0.056533 ) Phase=(l:-174.916 ) Offs=(l:   3.900 )
t207 channel[ 1]: Ref: Amp=(l:0.059263 ) Phase=(l:-129.290 ) Offs=(l:   3.100 )
t207 channel[ 2]: Ref: Amp=(l:0.025482 ) Phase=(l: -13.035 ) Offs=(l:  -0.500 )
t207 channel[ 3]: Ref: Amp=(l:0.062434 ) Phase=(l: -75.062 ) Offs=(l:  -5.100 )
t207 channel[ 4]: Ref: Amp=(l:0.051808 ) Phase=(l: 120.126 ) Offs=(l:  -6.200 )
t207 channel[ 5]: Ref: Amp=(l:0.058286 ) Phase=(l:  87.796 ) Offs=(l:   5.800 )
t207 channel[ 6]: Ref: Amp=(l:0.060018 ) Phase=(l: 129.865 ) Offs=(l:  -1.200 )

t207 channel[ 0]: Rem: Amp=(l:0.032446 ) Phase=(l:-157.032 ) Offs=(l:   0.900 )
t207 channel[ 1]: Rem: Amp=(l:0.032063 ) Phase=(l:-129.068 ) Offs=(l:   1.800 )
t207 channel[ 2]: Rem: Amp=(l:0.031816 ) Phase=(l: -26.614 ) Offs=(l:  -2.900 )
t207 channel[ 3]: Rem: Amp=(l:0.031788 ) Phase=(l:-129.755 ) Offs=(l:  -2.900 )
t207 channel[ 4]: Rem: Amp=(l:0.030973 ) Phase=(l:-179.681 ) Offs=(l:  -4.400 )
t207 channel[ 5]: Rem: Amp=(l:0.030735 ) Phase=(l: -22.263 ) Offs=(l:   8.400 )
t207 channel[ 6]: Rem: Amp=(l:0.030000 ) Phase=(l: 177.788 ) Offs=(l:  -2.200 )
*/

  
  x_a = new std::complex<double>[numOfData];
  x_a_rev = new std::complex<double>[numOfData];
  x_A = new std::complex<double>[numOfData];
  
  for (int i=0; i<numOfData; i++)
  {
    x_a[i] = std::complex<double>(vecR->getElement(i), vecI->getElement(i));
    x_A[i] = x_a_rev[i] = 0.0;
  };
//  fft(x, y, numOfData, 1);
  fft(x_a, x_A, numOfData, FFT_Forward);
  fft(x_A, x_a_rev, numOfData, FFT_Inverse);

  
  int numOfBranches = 7;
  const char* branchNames[7] = {"OrigR", "OrigI", "Real", "Imag", "Abs", "RevR", "RevI"};
  
  carrier_ = new SgPlotCarrier(2, 0, "Four1 testing");

  for (int i=0; i<numOfBranches; i++)
    carrier_->createBranch(numOfData, branchNames[i]);

  carrier_->setNameOfColumn(0, "Freq");
  carrier_->setNameOfColumn(1, "Four1");

  // input:
  branch = carrier_->listOfBranches()->at(0);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, vecR->getElement(i));
  };
  branch = carrier_->listOfBranches()->at(1);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, vecI->getElement(i));
  };
  //
  // output:
  branch = carrier_->listOfBranches()->at(2);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, x_A[i].real());
  };
  branch = carrier_->listOfBranches()->at(3);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, x_A[i].imag());
  };
  branch = carrier_->listOfBranches()->at(4);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, abs(x_A[i]));
  };
  //
  branch = carrier_->listOfBranches()->at(5);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, x_a_rev[i].real());
  };
  branch = carrier_->listOfBranches()->at(6);
  for (int i=0; i<numOfData; i++)
  {
    branch->data()->setElement(i, 0, i);
    branch->data()->setElement(i, 1, x_a_rev[i].imag());
  };
  
  delete[] x_a;
  delete[] x_a_rev;
  delete[] x_A;

  //
  //
  plot_ = new SgPlot(carrier_, setup.getPath2PlotterOutput(), this);
  plot_->setOutputFormat(setup.getPlotterOutputFormat());

  return plot_;
};
/*=====================================================================================================*/
