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

#include <iostream>
#include <stdlib.h>


#include <QtCore/QDataStream>


#include <SgVlbiBaselineInfo.h>
#include <SgVlbiObservation.h>
#include <SgVlbiObservable.h>

#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiBaselineInfo::className()
{
  return "SgVlbiBaselineInfo";
};



//
void SgVlbiBaselineInfo::createParameters()
{
  QString prefix = "Bln: " + getKey() + ": ";
  releaseParameters();
  pClock_ = new SgParameter(prefix + "Clock_0");
  pBx_ = new SgParameter(prefix + "coord-X");
  pBy_ = new SgParameter(prefix + "coord-Y");
  pBz_ = new SgParameter(prefix + "coord-Z");
  dClock_ = 0.0;
  dClockSigma_ = 0.0;
};



//
void SgVlbiBaselineInfo::releaseParameters()
{
  if (pClock_)
  {
    if (pClock_->isAttr(SgParameter::Attr_IS_IN_RUN))
    {
      dClock_ = pClock_->getSolution();
      dClockSigma_ = pClock_->getSigma();
    };
    delete pClock_;
    pClock_ = NULL;
  };
  if (pBx_)
  {
    delete pBx_;
    pBx_ = NULL;
  };
  if (pBy_)
  {
    delete pBy_;
    pBy_ = NULL;
  };
  if (pBz_)
  {
    delete pBz_;
    pBz_ = NULL;
  };
};



//
bool SgVlbiBaselineInfo::selfCheck()
{
  bool                          isOk=true;
  int                           num=0, count=0;
  numOfChanByCount_.clear();
  grdAmbigsBySpacing_.clear();
  phdAmbigsBySpacing_.clear();
  //
  // set up "typical" group delay ambiguity spacing for the band:
  for (int i=0; i<observables_.size(); i++)
  {
    grdAmbigsBySpacing_[observables_.at(i)->grDelay().getAmbiguitySpacing()]++;
    phdAmbigsBySpacing_[observables_.at(i)->phDelay().getAmbiguitySpacing()]++;
    numOfChanByCount_[observables_.at(i)->getNumOfChannels()]++;
  };
  // group delay:
  if (grdAmbigsBySpacing_.size() == 1)
  {
    typicalGrdAmbigSpacing_ = grdAmbigsBySpacing_.begin().key();
    strGrdAmbigsStat_.sprintf("%.1f", typicalGrdAmbigSpacing_*1.0e9);
  }
  else
  {
    strGrdAmbigsStat_ = "";
    num = observables_.size();
    count = 0;
    for (QMap<double, int>::iterator it=grdAmbigsBySpacing_.begin(); it!=grdAmbigsBySpacing_.end(); ++it)
    {
      strGrdAmbigsStat_ += QString("").sprintf("%.1f (%.1f%%), ", it.key()*1.0e9, it.value()*100.0/num);
      if (count < it.value())
      {
        count = it.value();
        typicalGrdAmbigSpacing_ = it.key();
      };
    };
    strGrdAmbigsStat_ = strGrdAmbigsStat_.left(strGrdAmbigsStat_.size() - 2);
  };
  // phase dleay:
  if (phdAmbigsBySpacing_.size() == 1)
  {
    typicalPhdAmbigSpacing_ = phdAmbigsBySpacing_.begin().key();
    strPhdAmbigsStat_.sprintf("%.3f", typicalPhdAmbigSpacing_*1.0e9);
  }
  else
  {
    strPhdAmbigsStat_ = "";
    num = observables_.size();
    count = 0;
    for (QMap<double, int>::iterator it=phdAmbigsBySpacing_.begin(); it!=phdAmbigsBySpacing_.end(); ++it)
    {
      strPhdAmbigsStat_ += QString("").sprintf("%.3f (%.1f%%), ", it.key()*1.0e9, it.value()*100.0/num);
      if (count < it.value())
      {
        count = it.value();
        typicalPhdAmbigSpacing_ = it.key();
      };
    };
    strPhdAmbigsStat_ = strPhdAmbigsStat_.left(strPhdAmbigsStat_.size() - 2);
  };
  //
  // num of channels:
  if (numOfChanByCount_.size() == 1)
    typicalNumOfCannels_ = numOfChanByCount_.begin().key();
  else
  {
    count = 0;
    for (QMap<int, int>::iterator it=numOfChanByCount_.begin(); it!=numOfChanByCount_.end(); ++it)
      if (count < it.value())
      {
        count = it.value();
        typicalNumOfCannels_ = it.key();
      };
  };
  //
  return isOk;
};






//
void SgVlbiBaselineInfo::calculateClockF1(SgTaskConfig*)
{
  const int                     M0=3;
  const double                  dS=50.0E-12; // 50ps
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;
  double                        x0, s;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    o->owner()->delAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
        (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)) )
      oList.append(o);
  };

  if (oList.size()<2*M0)
  {
    oList.clear();
    return;
  };
  
  x0 = oList.at(0)->epoch().toDouble();

  double                      x, y, p;
  //
  double                      A_l, B_l, C_l, D_l, E_l, F_l;
  double                      a_l, b_l, wrms_l;
  //
  double                      A_r, B_r, C_r, D_r, E_r, F_r;
  double                      a_r, b_r, wrms_r;
  //
  
  A_l = B_l = C_l = D_l = E_l = F_l = 0.0;
  a_l = b_l = wrms_l = 0.0;
  
  A_r = B_r = C_r = D_r = E_r = F_r = 0.0;
  a_r = b_r = wrms_r = 0.0;

  //
  // calc whole wrms residuals:
  for (int i=0; i<oList.size(); i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    p = 1.0/s/s;
    //
    A_l += p*x*x;
    B_l += p*x;
    C_l += p*x*y;
    D_l += p;
    E_l += p*y;
    F_l += p*y*y;
  };
  b_l = (A_l*E_l - B_l*C_l)/(A_l*D_l - B_l*B_l);
  a_l = (C_l - B_l*b_l)/A_l;
  s = (A_l*a_l*a_l + D_l*b_l*b_l + F_l + 2.0*(B_l*a_l*b_l - C_l*a_l - E_l*b_l))/D_l;
  cbd_total_wrms_ = 0.0<s?sqrt(s):-1.0;
  //
  //
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    o->owner()->setBaselineClock_F1(cbd_total_wrms_);
    //o->setTest(o->owner()->getBaselineClock_F1());
  };
  //std::cout << " >> cbd_total_wrms_= " << cbd_total_wrms_*1.0e9 << " ns\n";
  //
  //
  A_l = B_l = C_l = D_l = E_l = F_l = 0.0;
  a_l = b_l = 0.0;
  // initial evaluating:
  for (int i=0; i<M0; i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    p = 1.0/s/s;
    //
    A_l += p*x*x;
    B_l += p*x;
    C_l += p*x*y;
    D_l += p;
    E_l += p*y;
    F_l += p*y*y;
  };
  b_l = (A_l*E_l - B_l*C_l)/(A_l*D_l - B_l*B_l);
  a_l = (C_l - B_l*b_l)/A_l;
  wrms_l = (A_l*a_l*a_l + D_l*b_l*b_l + F_l + 2.0*(B_l*a_l*b_l - C_l*a_l - E_l*b_l))/D_l;
  //
  for (int i=M0; i<oList.size(); i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    p = 1.0/s/s;
    //
    A_r += p*x*x;
    B_r += p*x;
    C_r += p*x*y;
    D_r += p;
    E_r += p*y;
    F_r += p*y*y;
  };
  b_r = (A_r*E_r - B_r*C_r)/(A_r*D_r - B_r*B_r);
  a_r = (C_r - B_r*b_r)/A_r;
  wrms_r = (A_r*a_r*a_r + D_r*b_r*b_r + F_r + 2.0*(B_r*a_r*b_r - C_r*a_r - E_r*b_r))/D_r;
  //
  //
  oList.at(M0-1)->owner()->setBaselineClock_F1 (sqrt(wrms_l + wrms_r));
  oList.at(M0-1)->owner()->setBaselineClock_F1l(sqrt(fabs(wrms_l)));
  oList.at(M0-1)->owner()->setBaselineClock_F1r(sqrt(fabs(wrms_r)));
  oList.at(M0-1)->owner()->addAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
  //  
  for (int i=M0; i<oList.size()-M0; i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    p = 1.0/s/s;
    //
    A_l += p*x*x;
    B_l += p*x;
    C_l += p*x*y;
    D_l += p;
    E_l += p*y;
    F_l += p*y*y;

    A_r -= p*x*x;
    B_r -= p*x;
    C_r -= p*x*y;
    D_r -= p;
    E_r -= p*y;
    F_r -= p*y*y;

    b_l = (A_l*E_l - B_l*C_l)/(A_l*D_l - B_l*B_l);
    a_l = (C_l - B_l*b_l)/A_l;
    wrms_l = (A_l*a_l*a_l + D_l*b_l*b_l + F_l + 2.0*(B_l*a_l*b_l - C_l*a_l - E_l*b_l))/D_l;

    b_r = (A_r*E_r - B_r*C_r)/(A_r*D_r - B_r*B_r);
    a_r = (C_r - B_r*b_r)/A_r;
    wrms_r = (A_r*a_r*a_r + D_r*b_r*b_r + F_r + 2.0*(B_r*a_r*b_r - C_r*a_r - E_r*b_r))/D_r;

    o->owner()->setBaselineClock_F1 (sqrt(wrms_l + wrms_r));
    o->owner()->setBaselineClock_F1l(sqrt(fabs(wrms_l)));
    o->owner()->setBaselineClock_F1r(sqrt(fabs(wrms_r)));
    o->owner()->addAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
    //o->setTest(o->owner()->getBaselineClock_F1());
  };

  oList.clear();
};









//
void SgVlbiBaselineInfo::calculateClockF1_save(SgTaskConfig*)
{
  const int                   M0=3;
  const double                dS=50.0E-12; // 50ps
  QList<SgVlbiObservable*>    oList;
  SgVlbiObservable           *o=NULL;
  double                      x0, s;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    o->owner()->delAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
  };

  if (oList.size()<2*M0)
    return;
  
  x0 = observables_.at(0)->epoch().toDouble();

  double      x, y, w;
  double      A_l, B_l, C_l, P_l, Q_l, R_l;
  double      a_l, b_l, wrms_l;
  double      A_r, B_r, C_r, P_r, Q_r, R_r;
  double      a_r, b_r, wrms_r;
  x = y = w = 0.0;
  A_l = B_l = C_l = P_l = Q_l = R_l = 0.0;
  a_l = b_l = wrms_l = 0.0;
  A_r = B_r = C_r = P_r = Q_r = R_r = 0.0;
  a_r = b_r = wrms_r = 0.0;

  // calc whole wrms residuals:
  for (int i=0; i<oList.size(); i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    w = 1.0/s/s;
    A_l += w;
    B_l += w*x;
    C_l += w*x*x;
    P_l += w*y;
    Q_l += w*x*y;
    R_l += w*y*y;
  };
  b_l = (P_l*B_l - A_l*Q_l)/(B_l*B_l - A_l*C_l);
  a_l = (Q_l - C_l*b_l)/B_l;
  cbd_total_wrms_ = sqrt(fabs((R_l + a_l*a_l*A_l + b_l*b_l*C_l +
                                2.0*(a_l*b_l*B_l - a_l*P_l - b_l*Q_l))/A_l));
  //
  //
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    o->owner()->setBaselineClock_F1(cbd_total_wrms_);
    //o->setTest(o->owner()->getBaselineClock_F1());
  };
  //std::cout << " >> cbd_total_wrms_= " << cbd_total_wrms_*1.0e9 << " ns\n";
  //
  //
  A_l = B_l = C_l = P_l = Q_l = R_l = 0.0;
  a_l = b_l = 0.0;
  // initial evaluating:
  for (int i=0; i<M0; i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    w = 1.0/s/s;
    
    A_l += w;
    B_l += w*x;
    C_l += w*x*x;
    P_l += w*y;
    Q_l += w*x*y;
    R_l += w*y*y;
  };
  for (int i=M0; i<oList.size(); i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    w = 1.0/s/s;

    A_r += w;
    B_r += w*x;
    C_r += w*x*x;
    P_r += w*y;
    Q_r += w*x*y;
    R_r += w*y*y;
  };

  b_l = (P_l*B_l - A_l*Q_l)/(B_l*B_l - A_l*C_l);
  a_l = (Q_l - C_l*b_l)/B_l;
  wrms_l = (R_l + a_l*a_l*A_l + b_l*b_l*C_l + 2.0*(a_l*b_l*B_l - a_l*P_l - b_l*Q_l))/A_l;
  b_r = (P_r*B_r - A_r*Q_r)/(B_r*B_r - A_r*C_r);
  a_r = (Q_r - C_r*b_r)/B_r;
  wrms_r = (R_r + a_r*a_r*A_r + b_r*b_r*C_r + 2.0*(a_r*b_r*B_r - a_r*P_r - b_r*Q_r))/A_r;
  //
  oList.at(M0-1)->owner()->setBaselineClock_F1 (sqrt(fabs(wrms_l) + fabs(wrms_r)));
  oList.at(M0-1)->owner()->setBaselineClock_F1l(sqrt(fabs(wrms_l)));
  oList.at(M0-1)->owner()->setBaselineClock_F1r(sqrt(fabs(wrms_r)));

  oList.at(M0-1)->owner()->addAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
  //  
  for (int i=M0; i<oList.size()-M0; i++)
  {
    o = oList.at(i);
    x = o->epoch().toDouble() - x0;
    y = o->activeDelay()->getResidual();
    s = fabs(o->activeDelay()->getSigma()) + dS;
    w = 1.0/s/s;

    A_l += w;
    B_l += w*x;
    C_l += w*x*x;
    P_l += w*y;
    Q_l += w*x*y;
    R_l += w*y*y;

    A_r -= w;
    B_r -= w*x;
    C_r -= w*x*x;
    P_r -= w*y;
    Q_r -= w*x*y;
    R_r -= w*y*y;

    b_l = (P_l*B_l - A_l*Q_l)/(B_l*B_l - A_l*C_l);
    a_l = (Q_l - C_l*b_l)/B_l;
    wrms_l = (R_l + a_l*a_l*A_l + b_l*b_l*C_l + 2.0*(a_l*b_l*B_l - a_l*P_l - b_l*Q_l))/A_l;
    b_r = (P_r*B_r - A_r*Q_r)/(B_r*B_r - A_r*C_r);
    a_r = (Q_r - C_r*b_r)/B_r;
    wrms_r = (R_r + a_r*a_r*A_r + b_r*b_r*C_r + 2.0*(a_r*b_r*B_r - a_r*P_r - b_r*Q_r))/A_r;
    o->owner()->setBaselineClock_F1 (sqrt(fabs(wrms_l) + fabs(wrms_r)));
    o->owner()->setBaselineClock_F1l(sqrt(fabs(wrms_l)));
    o->owner()->setBaselineClock_F1r(sqrt(fabs(wrms_r)));
    o->owner()->addAttr(SgVlbiObservation::Attr_PASSED_CL_FN1);
    //o->setTest(o->owner()->getBaselineClock_F1());
  };

  oList.clear();
};







//
void SgVlbiBaselineInfo::evaluateCBIndicator()
{
  QList<SgVlbiObservable*>      obsList;
  SgVlbiObservable             *o=NULL;
  int                           iM0=3;

  if (!cbIndicator_)
    cbIndicator_ = new SgClockBreakIndicator;
  cbIndicator_->totWrms_ = cbd_total_wrms_;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
//        &&
//        o->owner()->isAttr(SgVlbiObservation::Attr_PASSED_CL_FN1)
       )
      obsList.append(o);
  };
  if (obsList.size()<4)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    return;
  };
  //
  double                        minWrms, maxWrms;
  int                           minIdx=-1;
//  minWrms = maxWrms = obsList.at(0)->owner()->getBaselineClock_F1();
  minWrms = maxWrms = obsList.at(iM0)->owner()->getBaselineClock_F1();

/*
std::cout 
<< " >> minWrms= " << minWrms << "  "
<< " >> maxWrms= " << maxWrms << "  "
<< " >> obsList.size()= " << obsList.size() << "  "
<< "\n";
*/
//  for (int i=0; i<obsList.size(); i++)
  for (int i=iM0; i<obsList.size()-iM0; i++)
  {
    o = obsList.at(i);
/*    
std::cout 
<< " >> i= " << i << "  "
<< " epoch: " << qPrintable(obs->epoch().toString()) << "  "
<< " minWrms= " << minWrms << "  "
<< " obs->owner()->getBaselineClock_F1()= " << obs->owner()->getBaselineClock_F1() << "  "

<< "\n";
*/
    if (minWrms > o->owner()->getBaselineClock_F1())
    {
      minWrms = o->owner()->getBaselineClock_F1();
      minIdx = i;
    };
    if (maxWrms < o->owner()->getBaselineClock_F1())
      maxWrms = o->owner()->getBaselineClock_F1();
  };
  if (maxWrms == minWrms)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    return;
  };
  if (minIdx == -1)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    return;
  };
  if (minIdx < iM0)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    return;
  };
  if (obsList.size()-iM0 < minIdx)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    return;
  };
  //
  // ok:
  cbIndicator_->epoch_ = obsList.at(minIdx)->epoch();
  cbIndicator_->minWrms_ = minWrms;

/*
std::cout << " >> minIdx= " << minIdx << "  "
<< " epoch: " << qPrintable(obsList.at(minIdx)->epoch().toString())

<< "\n";
*/

  int                         i, n;
  // right wing:
  n = 0;
  i = minIdx + 1;
  while (i<obsList.size()-iM0 && 
    obsList.at(i-1)->owner()->getBaselineClock_F1() <= obsList.at(i)->owner()->getBaselineClock_F1())
  {
    n++;
    i++;
  };
  cbIndicator_->numOnRight_ = n;
  cbIndicator_->tOnRight_ = obsList.at(minIdx+n)->epoch();
  // left wing:
  n = 0;
  i = minIdx;
  while (i>0 && 
    obsList.at(i)->owner()->getBaselineClock_F1() <= obsList.at(i-1)->owner()->getBaselineClock_F1())
  {
    n++;
    i--;
  };
  cbIndicator_->numOnLeft_ = n;
  cbIndicator_->tOnLeft_ = obsList.at(minIdx-n)->epoch();
  obsList.clear();
};



//
void SgVlbiBaselineInfo::calculateClockF2(SgTaskConfig*)
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)) 
    )
      oList.append(o);
  };

  int                           n=oList.size();

  SgVector                      x (n);
  SgVector                      t (n);
  SgVector                      x0(n);
//SgVector                      w (n);
  double                        s, tp;
  tp = 2.0/24.0;

  for (int i=0; i<n; i++)
  {
    o = oList.at(i);
    t .setElement(i, o->owner()->toDouble());
    x0.setElement(i, o->activeDelay()->getResidual());
    x .setElement(i, o->activeDelay()->getResidual());

//  w .setElement(i, 1.0/o->activeDelay()->sigma2Apply()/o->activeDelay()->sigma2Apply());
//  w .setElement(i, 1.0);
  };
  //
  /*
  QList<double>                 lst;
  for (int l=1; l<n-1; l++)
  {
    s = 0.0;
    lst.clear();
    lst << x0.getElement(l-1);
    lst << x0.getElement(l  );
    lst << x0.getElement(l+1);
    //
    std::sort(lst.begin(), lst.end());
    //
    s = lst.at(1);
//    x.setElement(l, s);
//    x.setElement(l, x0.getElement(l));
  };
  */
  //
  // 22SEP28XA
  //
  //
  for (int l=0; l<n; l++)
  {
    o = oList.at(l);
    s = 0.0;
    for (int i=0; i<l; i++)
    {
      if (fabs(t.getElement(i) - t.getElement(l)) < tp)
        s -= x.getElement(i);
    };
    for (int i=l; i<n; i++)
    {
      if (fabs(t.getElement(i) - t.getElement(l)) < tp)
        s += x.getElement(i);
    };
    o->owner()->setBaselineClock_F2(s);
    //o->setTest2(s);
  };
  oList.clear();
};



//
void SgVlbiBaselineInfo::evaluateCBIndicator2()
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  if (!cbIndicator_)
    cbIndicator_ = new SgClockBreakIndicator;
  
  
  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
  };
  if (oList.size() < 4)
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    oList.clear();
    return;
  };

  //
  //
  double                        minV, maxV;
  double                        d;
  int                           minIdx=0, maxIdx=0, extremumIdx=0;
  minV = maxV = oList.at(extremumIdx)->owner()->getBaselineClock_F2();
  
  for (int i=0; i<oList.size(); i++)
  {
    o = oList.at(i);
    d = o->owner()->getBaselineClock_F2();

    if (d < minV)
    {
      minV = d;
      minIdx = i;
    };
    if (maxV < d)
    {
      maxV = d;
      maxIdx = i;
    };
  };
  //
  if (maxIdx == minIdx) // the same
  {
    delete cbIndicator_;
    cbIndicator_ = NULL;
    oList.clear();
    return;
  };
  //
  //
  // ok:
  if (fabs(minV) < fabs(maxV))
  {
    extremumIdx = maxIdx;
    cbIndicator_->extremum_ = maxV;
    cbIndicator_->options_ = SgClockBreakIndicator::O_MAX;
  }
  else
  {
    extremumIdx = minIdx;
    cbIndicator_->extremum_ = fabs(minV);
    cbIndicator_->options_ = SgClockBreakIndicator::O_MIN;
  };
 
  cbIndicator_->epoch_ = oList.at(extremumIdx)->epoch();
  
  int                           i, n;
  //
  // right wing:
  n = 0;
  i = extremumIdx + 1;
  while (i<oList.size()-1 && 
    (fabs(oList.at(i)->owner()->getBaselineClock_F2()) <= 
      fabs(oList.at(i-1)->owner()->getBaselineClock_F2()) ||
    fabs(oList.at(i)->owner()->getBaselineClock_F2()) <= 
      fabs(oList.at(i-2)->owner()->getBaselineClock_F2())
    )
        )
  {
    n++;
    i++;
  };
  cbIndicator_->numOnRight_ = n;
  cbIndicator_->tOnRight_ = oList.at(extremumIdx+n)->epoch();
  //
  // left wing:
  n = 0;
  i = extremumIdx;
  while (i>1 && 
    (fabs(oList.at(i-1)->owner()->getBaselineClock_F2()) <= 
      fabs(oList.at(i)->owner()->getBaselineClock_F2()) ||
    fabs(oList.at(i-2)->owner()->getBaselineClock_F2()) <= 
      fabs(oList.at(i)->owner()->getBaselineClock_F2())
      )
    )
  {
    n++;
    i--;
  };
  cbIndicator_->numOnLeft_ = n;
  cbIndicator_->tOnLeft_ = oList.at(extremumIdx-n)->epoch();

  if (1<extremumIdx && extremumIdx<oList.size()-1 &&
    fabs(oList.at(extremumIdx)->activeDelay()->getResidual() - 
      oList.at(extremumIdx+1)->activeDelay()->getResidual())    <
    fabs(oList.at(extremumIdx-1)->activeDelay()->getResidual() - 
      oList.at(extremumIdx)->activeDelay()->getResidual()) )
    cbIndicator_->epoch_ = oList.at(extremumIdx - 1)->epoch();

  oList.clear();
};



//
SgClockBreakIndicator* SgVlbiBaselineInfo::lookupClockF2(SgTaskConfig*, 
  const SgClockBreakIndicator* target)
{
  SgClockBreakIndicator        *cbi;
  cbi = NULL;
  
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  cbi = new SgClockBreakIndicator;
  
  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if ((o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
        (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
        &&
        target->tOnLeft_ <= *o->owner() && *o->owner() <= target->tOnRight_)
      oList.append(o);
  };
  if (oList.size() < 4)
  {
    oList.clear();
    delete cbi;
    cbi = NULL;
    return cbi;
  };
  //
  //
  double                        minV, maxV;
  double                        d, s;
  double                        epochAtTarget, dt;
  int                           minIdx=0, maxIdx=0, extremumIdx=0, idxAtTarget=0;
  minV = maxV = oList.at(extremumIdx)->owner()->getBaselineClock_F2();
  epochAtTarget = target->epoch_.toDouble();
  dt = fabs(oList.at(idxAtTarget)->epoch().toDouble() - epochAtTarget);
  
  for (int i=0; i<oList.size(); i++)
  {
    o = oList.at(i);
    d = o->owner()->getBaselineClock_F2();
    if (d < minV)
    {
      minV = d;
      minIdx = i;
    };
    if (maxV < d)
    {
      maxV = d;
      maxIdx = i;
    };
    s = fabs(oList.at(i)->epoch().toDouble() - epochAtTarget);
    if (s < dt)
    {
      idxAtTarget = i;
      dt = s;
    };
  };
  //
  if (maxIdx == minIdx) // the same
  {
    delete cbi;
    cbi = NULL;
    oList.clear();
    return cbi;
  };
  //
  //
  // ok:
  if (fabs(minV) < fabs(maxV))
  {
    extremumIdx = maxIdx;
    cbi->extremum_ = maxV;
    cbi->options_ = SgClockBreakIndicator::O_MAX;
  }
  else
  {
    extremumIdx = minIdx;
    cbi->extremum_ = fabs(minV);
    cbi->options_ = SgClockBreakIndicator::O_MIN;
  };
  cbi->epoch_ = oList.at(extremumIdx)->epoch();
  //
  if (4 < abs(idxAtTarget - extremumIdx)) 
  {
    delete cbi;
    cbi = NULL;
    oList.clear();
    return cbi;
  };
  //
  if (1<extremumIdx && extremumIdx<oList.size()-1 &&
    fabs(oList.at(extremumIdx)->activeDelay()->getResidual() - 
      oList.at(extremumIdx+1)->activeDelay()->getResidual())    <
    fabs(oList.at(extremumIdx-1)->activeDelay()->getResidual() - 
      oList.at(extremumIdx)->activeDelay()->getResidual()) )
    cbi->epoch_ = oList.at(extremumIdx - 1)->epoch();

  oList.clear();
  return cbi;
};



//
void SgVlbiBaselineInfo::scan4Ambiguities()
{
  scan4Ambiguities_m1();
//  scan4Ambiguities_m2();
};



//
void SgVlbiBaselineInfo::scan4Ambiguities_m1()
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
  };
  if (!oList.size())
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      ": scan4Ambiguities(): the number of processed observations at the baseline [" +
      getKey() + "] is zero; skipping");
    oList.clear();
    return;
  };
  //
  int                           num=oList.size(), numUp=0, numDn=0;
  for (int i=0; i<num; i++)
  {
    o = oList.at(i);
    o->activeDelay()->adjustAmbiguity();
    if (o->activeDelay()->getResidual() > 0.0)
      numUp++;
    if (o->activeDelay()->getResidual() < 0.0)
      numDn++;
  };
  bool                          isUp=numUp>numDn;
  int                           n=std::max(numUp, numDn);
  if (n<4)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": scan4Ambiguities(): the number of observations in the good stratum at the baseline [" +
      getKey() + "] less than 4; applying simple correction");
    for (int i=0; i<oList.size()-1; i++)
    {
      double                    closestResidual=oList.at(i)->activeDelay()->getResidual();
      o = oList.at(i+1);
      o->activeDelay()->adjustAmbiguity(closestResidual);
    };
    oList.clear();
    return;
  };
  SgVector                      x(n), y(n), e(n);
  SgMJD                         t0 = (*oList.at(oList.size()-1)->owner() - *oList.at(0)->owner())/2.0;
  int                           j=0;
  for (int i=0; i<num; i++)
  {
    o = oList.at(i);
    if ((isUp && o->activeDelay()->getResidual()>0.0) ||
        (!isUp && o->activeDelay()->getResidual()<0.0)  )
    {
      x(j) = *o->owner() - t0;
      y(j) = o->activeDelay()->getResidual();
      e(j) = o->activeDelay()->getSigma() + 10.0E-12;
      j++;
    };
  };
  // determine mean shift:
  double                        sA=0.0, sB=0.0, sC=0.0, closestResidual, d;
  for (int i=0; i<n; i++)
  {
    sA += 1.0/e.getElement(i)/e.getElement(i);
    sB += (d=y.getElement(i)/e.getElement(i))/e.getElement(i);
    sC += d*d;
  };
  closestResidual = sB/sA;

  oList.clear();
  // reprocess residuals:
  //
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    o->activeDelay()->adjustAmbiguity(closestResidual);
  };
};



//
void SgVlbiBaselineInfo::scan4Ambiguities_m2()
{
  QList<SgVlbiObservable*>      obsList;
  SgVlbiObservable             *o=NULL;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      obsList.append(o);
  };
  if (!obsList.size())
  {
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": scan4Ambiguities(): the number of processed observations at the baseline [" +
      getKey() + "] is zero; skipping");
    obsList.clear();
    return;
  };
  //
  int                           nAvg=5;
  double                        closestResidual;
  if (obsList.size() <= nAvg)
  {
    obsList.at(0)->activeDelay()->adjustAmbiguity();
    closestResidual = obsList.at(0)->activeDelay()->getResidual();
    for (int i=0; i<obsList.size()-1; i++)
    {
      obsList.at(i+1)->activeDelay()->adjustAmbiguity(closestResidual);
      closestResidual = obsList.at(i+1)->activeDelay()->getResidual();
    };
  }
  else
  {
    closestResidual = 0.0;
    for (int i=0; i<nAvg; i++)
      closestResidual += obsList.at(i)->activeDelay()->getResidual();
    closestResidual /= nAvg;
    for (int i=0; i<nAvg; i++)
      obsList.at(i)->activeDelay()->adjustAmbiguity(closestResidual);
    for (int i=nAvg; i<obsList.size(); i++)
    {
      closestResidual = 0.0;
      for (int j=0; j<nAvg; j++)
        closestResidual += obsList.at(i + j - nAvg)->activeDelay()->getResidual();
      closestResidual /= nAvg;

      obsList.at(i)->activeDelay()->adjustAmbiguity(closestResidual);
    };
  };
  return;



  int                           crIdx=0;
  closestResidual = fabs(obsList.at(crIdx)->activeDelay()->getResidual());
  for (int i=0; i<obsList.size(); i++)
  {
    o = obsList.at(i);
    if (fabs(o->activeDelay()->getResidual()) < closestResidual)
    {
      closestResidual = fabs(o->activeDelay()->getResidual());
      crIdx = i;
    };
  };
  closestResidual = obsList.at(crIdx)->activeDelay()->getResidual();
  // adjust the closest residual:
  obsList.at(crIdx)->activeDelay()->adjustAmbiguity(closestResidual);
  closestResidual = obsList.at(crIdx)->activeDelay()->getResidual();
  // process other residuals:
  for (int i=crIdx; i>0; i--)
  {
    closestResidual = obsList.at(i)->activeDelay()->getResidual();
    obsList.at(i-1)->activeDelay()->adjustAmbiguity(closestResidual);
  };
  for (int i=crIdx; i<obsList.size()-1; i++)
  {
    closestResidual = obsList.at(i)->activeDelay()->getResidual();
    o = obsList.at(i + 1);
    obsList.at(i+1)->activeDelay()->adjustAmbiguity(closestResidual);
  };
  obsList.clear();
};



//
void SgVlbiBaselineInfo::evaluateMeanGrDelResiduals()
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;
  //
  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
  };
  if (oList.size() == 0)
  {
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": evaluateMeanGrDelResiduals(): the number of processed observations at the baseline [" +
      getKey() + "] is zero; skipping");
    oList.clear();
    return;
  };
  //
  double                        sA=0.0, sB=0.0, sC=0.0;
  double                        y, w;
  for (int i=0; i<oList.size(); i++)
  {
    o = oList.at(i);
    y = o->activeDelay()->getResidual();
    w = o->activeDelay()->getSigma() + 5.0E-12;
    w = 1.0/w/w;
    sA += w;
    sB += w*y;
    sC += w*y*y;
  };
  meanGrDelResiduals_     = sB/sA;
  meanGrDelResidualsSigma_= (sC - sB)/sA;
  //
  oList.clear();
};



//
void SgVlbiBaselineInfo::shiftAmbiguities(int deltaN)
{
  if (deltaN==0 || observables_.size()==0)
    return; // nothing to do
  
  SgVlbiObservable             *o=observables_.at(0);
  double                        ambigSpacing=typicalGrdAmbigSpacing_, shift;
  if (o->activeDelay()->getDelayType() == SgTaskConfig::VD_PHS_DELAY)
    ambigSpacing = typicalPhdAmbigSpacing_;
  shift = deltaN*ambigSpacing;
  int                           n;
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    n = rint(shift/o->activeDelay()->getAmbiguitySpacing());
    o->activeDelay()->setResidual(o->activeDelay()->getResidual() + 
      n*o->activeDelay()->getAmbiguitySpacing());
    o->activeDelay()->setNumOfAmbiguities(o->activeDelay()->getNumOfAmbiguities() + n);
  };
};



//
SgVlbiStationInfo* SgVlbiBaselineInfo::stn_1(QMap<QString, SgVlbiStationInfo*> stations) const
{
  SgVlbiStationInfo            *stn=NULL;
  QString                       name(getKey().left(8));
  if (stations.contains(name))
    stn = stations.value(name);
  return stn;
};



//
SgVlbiStationInfo* SgVlbiBaselineInfo::stn_2(QMap<QString, SgVlbiStationInfo*> stations) const
{
  SgVlbiStationInfo            *stn=NULL;
  QString                       name(getKey().right(8));
  if (stations.contains(name))
    stn = stations.value(name);
  return stn;
};



//
void SgVlbiBaselineInfo::setGrdAmbiguities2min()
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
  };
  if (!oList.size())
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      ": setAmbiguities2min(): the number of processed observations at the baseline [" +
      getKey() + "] is zero; skipping");
    oList.clear();
    return;
  };
  //
  int                         num=oList.size(), meanAmbig;
  double                      sum=0.0;
  for (int i=0; i<num; i++)
    sum += oList.at(i)->activeDelay()->getNumOfAmbiguities();
  meanAmbig = round(sum/num);
  if (meanAmbig != 0)
    for (int i=0; i<observables_.size(); i++)
    {
      o = observables_.at(i);
      o->activeDelay()->setNumOfAmbiguities(o->activeDelay()->getNumOfAmbiguities() 
        - meanAmbig);
    };
};



//
bool SgVlbiBaselineInfo::saveIntermediateResults(QDataStream& s) const
{
  SgObjectInfo::saveIntermediateResults(s);
  //
  s << dClock_ << dClockSigma_;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiBaselineInfo::loadIntermediateResults(QDataStream& s)
{
  double                        dClock, dClockSigma;
  //
  SgObjectInfo::loadIntermediateResults(s);
  //
  s >> dClock >> dClockSigma;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  dClock_ = dClock;
  dClockSigma_ = dClockSigma;
  //
  return s.status()==QDataStream::Ok;
};



//
void SgVlbiBaselineInfo::calculateTest2(SgTaskConfig*)
{
  QList<SgVlbiObservable*>      oList;
  SgVlbiObservable             *o=NULL;

  // first, clear the flag, then, pick up processed observations:
  for (int i=0; i<observables_.size(); i++)
  {
    o = observables_.at(i);
    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
       (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
      oList.append(o);
    //o->setTest(0.0);
  };

  int                           n;

  n = oList.size();
  SgVector                      x0(n);
  SgVector                      x (n);
  SgVector                      s (n);
  SgVector                      t (n);
  double                        sp, w, dFWHM, a, d;
//double                        sm;
  double                        sY, sW;
  double                        dL, dR;
  
  w = 0.0;
  for (int i=0; i<n; i++)
  {
    o = oList.at(i);
    x0.setElement(i, o->activeDelay()->getResidual());
    x .setElement(i, o->activeDelay()->getResidual());
    s .setElement(i, o->activeDelay()->sigma2Apply());
    t .setElement(i, o->owner()->toDouble());
    w += x.getElement(i);
  };    

  //

  dFWHM = 2.0/24.0;

  a = dFWHM/2.35482;
  for (int l=0; l<n; l++)
  {

    sp = sY = sW = 0.0;
    for (int i=0; i<n; i++)
    {
      d = (t.getElement(l) - t.getElement(i))/a;
      d*= d;
      d = exp(-d/2.0)/s.getElement(i)/s.getElement(i);
      sY += d*x.getElement(i);
      sW += d;
    };
    sp = sY/sW;
    //oList.at(l)->setTest (sp); // <- just Gaussian smoothing
    //
    // ---- 
    dL = dR = 0.0;

   
    sY = sW = 0.0;
    for (int i=0; i<l; i++)
    {
      d = (t.getElement(l) - t.getElement(i))/a;
      d*= d;
      d = exp(-d/2.0)/s.getElement(i)/s.getElement(i);
      sY += d*x.getElement(i);
      sW += d;
    };
    if (0.0 < sW)
      dL = sY/sW;

    sY = sW = 0.0;
    for (int i=l+1; i<n; i++)
    {
      d = (t.getElement(l) - t.getElement(i))/a;
      d*= d;
      d = exp(-d/2.0)/s.getElement(i)/s.getElement(i);
      sY += d*x.getElement(i);
      sW += d;
    };
    if (0.0 < sW)
      dR = sY/sW;

    //oList.at(l)->setTest2(dL - dR);
/*
 * 
    sp = 0.0;
    sm = 0.0;

    for (int i=l; i<n; i++, m++)
    {
      d = (t.getElement(l) - t.getElement(i))/a;
      sp += x.getElement(i)*c1*exp(-d*d/2.0);
    };
    
    for (int i=0; i<l; i++, m++)
    {
      d = (t.getElement(l) - t.getElement(i))/a;
      sm += x.getElement(i)*c1*exp(-d*d/2.0);
    };

    oList.at(l)->setTest (sp);
    oList.at(l)->setTest2(sm);
 *
 */


/*  Good 2nd case:
    m = 0;    
    for (int i=l; i<n; i++, m++)
    {
      sp += x.getElement(i);       // <- convolution
    };
    oList.at(l)->setTest (sp/m);
    
    m = 0;
    for (int i=0; i<l; i++, m++)
    {
      sm += x.getElement(i);       // <- convolution
    };
    if (m<=0)
      m = 1;
    oList.at(l)->setTest2(sm/m - oList.at(l)->getTest());
*/
  };

  oList.clear();
};



//
void SgVlbiBaselineInfo::calculateTest(SgTaskConfig*)
{
  if (false)
  {
    QList<SgVlbiObservable*>    oList;
    SgVlbiObservable           *o=NULL;

    // first, clear the flag, then, pick up processed observations:
    for (int i=0; i<observables_.size(); i++)
    {
      o = observables_.at(i);
      if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) ||
         (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED)))
        oList.append(o);
    };

    int                           n;

    n = oList.size();
    SgVector                      x0(n);
    SgVector                      x (n);
    SgVector                      t (n);
    SgVector                      w (n);
    double                        sp, sm;
  
    for (int i=0; i<n; i++)
    {
      o = oList.at(i);
      t .setElement(i, o->owner()->toDouble());
      x0.setElement(i, o->activeDelay()->getResidual());
      w .setElement(i, 1.0/o->activeDelay()->sigma2Apply()/o->activeDelay()->sigma2Apply());
//    w .setElement(i, 1.0);
    };    
    //
    // 22SEP28XA
    //
    QList<double>                 lst;
    for (int l=1; l<n-1; l++)
    {
      sm = 0.0;

      lst.clear();
//    lst << x0.getElement(l-4);
//    lst << x0.getElement(l-3);
//    lst << x0.getElement(l-2);
      lst << x0.getElement(l-1);
      lst << x0.getElement(l  );
      lst << x0.getElement(l+1);
//    lst << x0.getElement(l+2);
//    lst << x0.getElement(l+3);
//    lst << x0.getElement(l+4);

      std::sort(lst.begin(), lst.end());
    
      sm = lst.at(1);
//      x.setElement(l, sm);
      x.setElement(l, x0.getElement(l));
    
      //oList.at(l)->setTest2(sm);
    };


//double                        dt;
  double                        tp;
  tp = 4.0/24.0;

  //
  /*
  for (int l=0; l<n; l++)
  {
    sp = 0.0;
    for (int i=0; i<n; i++)
    {
      dt = t.getElement(i) - t.getElement(l);
   
      if (-tp <= dt && dt <= 0.0)
        sp += x.getElement(i)*(1.0 + dt/tp);
      else if (0.0 < dt && dt <= -tp)
        sp += x.getElement(i)*(-1.0 + dt/tp);
    };
    oList.at(l)->setTest (sp);
  };
  */

  /*
    for (int l=0; l<n; l++)
    {
      sp = 0.0;
      sm = 0.0;
      for (int i=0; i<l; i++)
      {
        dt = fabs(t.getElement(i) - t.getElement(l));
        if (dt <= tp)
        {
          sp += x.getElement(i)*(-1.0 + dt/tp)*w.getElement(i);
          sm += w.getElement(i);
        }
      };
      for (int i=l; i<n; i++)
      {
        dt = fabs(t.getElement(i) - t.getElement(l));
        if (dt <= tp)
        {
          sp += x.getElement(i)*(1.0 - dt/tp)*w.getElement(i);
          sm += w.getElement(i);
        };
      };
      oList.at(l)->setTest (sp/sm);
    };
  */

    for (int l=0; l<n; l++)
    {
      sp = 0.0;
      sm = 0.0;
      for (int i=0; i<l; i++)
      {
        if (fabs(t.getElement(i) - t.getElement(l)) < tp)
        {
          sp -= x.getElement(i);// *w.getElement(i);
          sm += w.getElement(i);
        };
      };
      for (int i=l; i<n; i++)
      {
        if (fabs(t.getElement(i) - t.getElement(l)) < tp)
        {
          sp += x.getElement(i); //*w.getElement(i);
          sm += w.getElement(i);
        };
      };
      //oList.at(l)->setTest2(sp);
    };

    //
    //
    /*  
    m = 10;
    double                        dN, dM, dI;
    dM = m;
    dN = n;
    for (int l=0; l<n; l++)
    {
      sm = 0.0;

      if (!true)
      {
        // convolution:
        for (int i=l-n+1; i<=l; i++)
        {
          dI = i;
          a = 0.0;

          if (-n/2/m<i && i<=0)
            a = 1.0 + 2.0*dM/dN*dI;
          else if (0<i && i<n/2/m)
            a = 1.0 - 2.0*dM/dN*dI;

          sm += oList.at(l-i)->getTest()*a; // <- convolution
        };
      }
      else
      {
        // cross-correlation:
        for (int i=-l; i<=n-1-l; i++)
        {
          dI = i;
          a = 0.0;

          if (-n/2/m<i && i<=0)
            a = 1.0 + 2.0*dM/dN*dI;
          else if (0<i && i<n/2/m)
            a = 1.0 - 2.0*dM/dN*dI;

          sm += oList.at(l+i)->getTest()*a; // <- cross-correlation
        };
      };

      oList.at(l)->setTest2(sm);
    };
    */ 

    oList.clear();
  };
};





/*=====================================================================================================*/




/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
//

/*=====================================================================================================*/
