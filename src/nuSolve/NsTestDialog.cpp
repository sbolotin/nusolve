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
#include "NsTestDialog.h"

#include <iostream>
#include <stdlib.h>


#include <SgCubicSpline.h>
#include <SgGuiPlotter.h>
#include <SgLogger.h>
#include <SgMJD.h>

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
NsTestDialog::NsTestDialog(QWidget *parent, Qt::WindowFlags flags) 
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



NsTestDialog::~NsTestDialog()
{
  if (carrier_)
  {
    delete carrier_;
    carrier_ = NULL;
  }
};



void NsTestDialog::accept()
{
  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": accept selected");
  deleteLater();
};



void NsTestDialog::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": reject selected");
  deleteLater();
};




QWidget* NsTestDialog::testPlot()
{
  int numOfVals = 4;
  int numOfSigs = 2;
  int numOfBranches = 3;
  int numsOfRows[3] = {20, 45, 57};
  int numOfSpl = 500;
  const char* branchNames[3] = {"Branch #1", "Branch #2", "Branch #3"};
  
  carrier_ = new SgPlotCarrier(numOfVals, numOfSigs, "Plot testing");

  for (int i=0; i<numOfBranches; i++)
    carrier_->createBranch(numsOfRows[i], branchNames[i]);

  carrier_->createBranch(numOfSpl, "Cubic Spline #2");
 
  carrier_->setNameOfColumn(0, "Time, MJD");
  carrier_->setAxisType(0, SgPlotCarrier::AxisType_MJD);
  carrier_->setNameOfColumn(1, "Value #1");
  carrier_->setNameOfColumn(2, "Value #2");
  carrier_->setNameOfColumn(3, "Value #3");
  carrier_->setNameOfColumn(4, "Sigma #1");
  carrier_->setNameOfColumn(5, "Sigma #2");

  carrier_->setStdVarIdx( 1,  4);
  carrier_->setStdVarIdx( 2,  5);
      

  double t0=tEphem.getDate()-10.2, dt=0.1;
  double tShifts[3] = {2.33, -1.8, 1.16};
  double dPeriods[3] = {-0.8, 0.3, 1.1};
  double dPhases[3] = {-3.8, 2.3, 4.7};
  double dAmplitudes[3] = {-1.3, 2.8, 0.7};
  for (int i =0; i<numOfBranches; i++)
  {
    SgPlotBranch* branch = carrier_->listOfBranches()->at(i);
    double t = t0 + tShifts[i];
    double a, b, c;
    double ae, be;
    for (int j=0; j<numsOfRows[i]; j++)
    {
      t += dt;
      
      a = 8.0 + dPhases[i] + (3.0 + dAmplitudes[i])*
              cos(2.0*M_PI/(7.0 + dPeriods[i])*(t - tEphem + 4.5 + dPhases[i])) + 
              2.5*dAmplitudes[i]*(((double)random())/RAND_MAX - 0.5);
      
  
      b = 2.0 + tShifts[i] + (0.8 + dAmplitudes[i])*
              cos(2.0*M_PI/(3.5 + dPeriods[i])*(t - tEphem - 2.3 + dPhases[i])) + 
              1.2*dAmplitudes[i]*(((double)random())/RAND_MAX - 0.5);
      
      c = 18.0 - 6.3*(t - tEphem + dPhases[i]) + 
              4.2*dAmplitudes[i]*(t - tEphem + dPhases[i])/(2.25 + 
                    1.0*sin(2.0*M_PI/(2.3 + dPeriods[i])*(t - tEphem + dPhases[i])));
      
      ae = 4.0 + 3.3*sin(2.0*M_PI/3.0*(t-tEphem + 3.3));
      be = 1.5 + 1.0*sin(2.0*M_PI/7.0*(t-tEphem));
      branch->data()->setElement(j, 0, t);
      branch->data()->setElement(j, 1, a);
      branch->data()->setElement(j, 2, b);
      branch->data()->setElement(j, 3, c);
      branch->data()->setElement(j, 4, ae);
      branch->data()->setElement(j, 5, be);
    };
  };
  // an example how to use splines:
  // prepare splines:
  SgPlotBranch                 *workBranch   = carrier_->listOfBranches()->at(1);
  SgPlotBranch                 *splineValBranch = carrier_->listOfBranches()->at(numOfBranches);
  int                           nRows=workBranch->data()->nRow();
  // create the spline object:
  SgCubicSpline                 spliner(nRows, 5);
  // feed it:
  for (int i=0; i<nRows; i++)
  {
    spliner.argument().setElement(i, workBranch->data()->getElement(i, 0));
    spliner.table().setElement(i, 0, workBranch->data()->getElement(i, 1));
    spliner.table().setElement(i, 1, workBranch->data()->getElement(i, 2));
    spliner.table().setElement(i, 2, workBranch->data()->getElement(i, 3));
    spliner.table().setElement(i, 3, workBranch->data()->getElement(i, 4));
    spliner.table().setElement(i, 4, workBranch->data()->getElement(i, 5));
  };
  // evaluate spline coefficients:
  spliner.prepare4Spline();
  // add splines to the plot:
  double                        t, r;
  t = spliner.argument().getElement(0);
  dt = (spliner.argument().getElement(nRows-1) - spliner.argument().getElement(0))/(numOfSpl - 1);
  // use splines:
  for (int i=0; i<numOfSpl; i++)
  {
    splineValBranch->data()->setElement(i, 0, t);
    splineValBranch->data()->setElement(i, 1, spliner.spline(t, 0, r));
    splineValBranch->data()->setElement(i, 2, spliner.spline(t, 1, r));
    splineValBranch->data()->setElement(i, 3, spliner.spline(t, 2, r));
    splineValBranch->data()->setElement(i, 4, spliner.spline(t, 3, r));
    splineValBranch->data()->setElement(i, 5, spliner.spline(t, 4, r));
    t += dt;
  };
  plot_ = new SgPlot(carrier_, setup.getPath2PlotterOutput(), this);
  plot_->setOutputFormat(setup.getPlotterOutputFormat());
  return plot_;
};
/*=====================================================================================================*/
