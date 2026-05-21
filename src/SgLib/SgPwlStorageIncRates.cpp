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

#include <SgLogger.h>

#include <SgPwlStorageIncRates.h>

#include <SgTaskConfig.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPwlStorageIncRates::className()
{
  return "SgPwlStorageIncRates";
};



SgPwlStorageIncRates::SgPwlStorageIncRates() :
  SgPwlStorage()
{
};



// A destructor:
SgPwlStorageIncRates::~SgPwlStorageIncRates()
{
};



//
int SgPwlStorageIncRates::getNumOfActiveParameters(const SgMJD& t)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  int                           nAs, nBs;
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      nAs = getNumOfPolynomials();
      nBs = calcCurrentIdx(t) + 1;
      break;
    case SgPartial::DD_RATE:
      nAs = getNumOfPolynomials()>0?getNumOfPolynomials()-1:0;
      nBs = 1;
      break;
  };
  return nAs + nBs;
//  return getNumOfPolynomials() + calcCurrentIdx(t) + 1;
};



//
int SgPwlStorageIncRates::getNumOfSegments()
{
  return numOfNodes_;
};



//
void SgPwlStorageIncRates::deployParameters(SgParameter* p, 
  const SgMJD& t0, const SgMJD& tN, const SgMJD& tRefer,
  const SgTaskConfig* /*cfg*/)
{
  if (isPOrigOwner_)
    delete pOrig_;
  pOrig_ = p;
  isPOrigOwner_ = false;
  tStart_ = t0;
  tFinis_ = tN;
  //tRefer_ = tN;
  //tRefer_.setTime(0); // 0hr
  tRefer_ = tRefer;
  step_ = pOrig_->getStep();
  //
  if (false)
  {
    SgMJD                       t0h(tRefer_);
    if (step_ > tFinis_-tStart_)
    {
      tStart_ = t0h - ceil((t0h - t0)*24.0)/24.0;
      tFinis_ = t0h + ceil((tN - t0h)*24.0)/24.0;
      step_ = tFinis_ - tStart_;
    }
    else
    {
      tStart_ = t0h - ceil((t0h - t0)*24.0)/24.0;
      tFinis_ = tStart_ + ceil((tN - tStart_)/step_)*step_;
    };
  };
  //
  //
  // if the step equal to an interval of data set, only a zero order (shift) should be set:
  if (step_ > tFinis_-tStart_)
  {
    step_ = tFinis_ - tStart_ + 1.0E-4/DAY2SEC;
    numOfPolynomials_ = 1;
  }
  else
    numOfPolynomials_ = p->getNumOfPolynomials();
  if (numOfPolynomials_ > 0)
  {
    pAi_ = new SgParameter[numOfPolynomials_];
    for (int i=0; i<numOfPolynomials_; i++)
    {
      pAi_[i].setName(pOrig_->getName() + ": A_" + QString("").sprintf("%d", i));
      pAi_[i].tune(*p);
      pAi_[i].setSigmaAPriori(p->getSigmaAPrioriAux());
      pAi_[i].setPMode(SgParameterCfg::PM_LOC);
      pAi_[i].setTLeft(tZero);
      pAi_[i].setTRight(tInf);
    };
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() +
      "::deployParameters(): " + QString("").setNum(numOfPolynomials_) + 
      " polynomial parameters were allocated for " + pOrig_->getName() + " parameter");
  };

  // segmented rates:
  numOfNodes_ = ceil((tFinis_ - tStart_)/step_);
  pBi_ = new SgParameter[numOfNodes_];
  for (int i=0; i<numOfNodes_; i++)
  {
    pBi_[i].setName(pOrig_->getName() + ": B_" + QString("").sprintf("%03d", i));
    pBi_[i].tune(*p);
    pBi_[i].setPMode(SgParameterCfg::PM_LOC);
    pBi_[i].setTLeft(tStart_ + i*step_);
    pBi_[i].setTRight(tInf);
  };
  
  sumP2_ = 0.0;
  sumX1P2_ = 0.0;
  sumX2P2_ = 0.0;
  sumT1P2_ = 0.0;
};



//
void SgPwlStorageIncRates::propagatePartials(const SgMJD& tt)
{
  if (!pOrig_->isAttr(SgPartial::Attr_IS_PARTIAL_SET))
    return;
  //
  // adjust time from constraints equations:
  SgMJD                         t(tt==tZero?tStart_:tt);
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  //
  if (t < tStart_)
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      ": propagatePartials(): t(" + t.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ") < tStart(" + 
      tStart_.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ")");
  if (tFinis_ < t)
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      ": propagatePartials(): tFinis(" + tFinis_.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ") < t(" + 
      t.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ")");
  //
  double                        d(pOrig_->getD());
  //
  // Polynomials:
  if (numOfPolynomials_ > 0)
  {
    double                      dt = (t - tRefer_);
    double                      dd = 1.0;
    switch (dDomain)
    {
      default:
      case SgPartial::DD_OFFSET:
        for (int i=0; i<numOfPolynomials_; i++)
        {
          pAi_[i].setD(d*dd);
          dd *= dt;
        };
        break;
      case SgPartial::DD_RATE:
        for (int i=1; i<numOfPolynomials_; i++)
        {
          pAi_[i].setD(i*d*dd);
          dd *= dt/*/DAY2SEC*/;
        };
        break;
    };
  };
  //
  // rates:
  int                           j=calcCurrentIdx(t);
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      for (int i=0; i<j; i++)
        pBi_[i].setD(d*step_);
      pBi_[j].setD(d*((t - tStart_) - j*step_));
      break;
    case SgPartial::DD_RATE:
      pBi_[j].setD(d);
      break;
  };
  //
  pOrig_->zerofy();
};



//
double SgPwlStorageIncRates::calcAX(const SgMJD& t, bool isWorkingBand)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  double                        f(0.0), s2(0.0);
  int                           j, n(0);
  //
  // Polynomials:
  if (numOfPolynomials_ > 0)
  {
    double                      dt = (t - tRefer_);
    double                      dd = 1.0;
    switch (dDomain)
    {
      default:
      case SgPartial::DD_OFFSET:
        for (int i=0; i<numOfPolynomials_; i++)
        {
          f += pAi_[i].getSolution()*dd;
          s2+= pAi_[i].getSigma()*pAi_[i].getSigma()*dd*dd;
          dd *= dt;
        };
        break;
      case SgPartial::DD_RATE:
        for (int i=1; i<numOfPolynomials_; i++)
        {
          f += i*pAi_[i].getSolution()*dd;
          s2+= i*i*pAi_[i].getSigma()*pAi_[i].getSigma()*dd*dd;
          dd *= dt/*/DAY2SEC*/;
        };
        break;
    };
  };
  //
  // Rates:
  double                        fs(0.0), s2s(s2), tt, d;
  if (numOfPolynomials_==1)
    fs = f;
  j = calcCurrentIdx(t);
  //
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      for (int i=0; i<j; i++)
      {
        d = step_*pBi_[i].getSolution();
        f  += d;
        fs += d;
        d = step_*step_*pBi_[i].getSigma()*pBi_[i].getSigma();
        s2 += d;
        s2s+= d;
      };
      tt = ((t - tStart_) - j*step_);
      d  = tt*pBi_[j].getSolution();
      f  += d;
      fs += d;
      d = tt*tt*pBi_[j].getSigma()*pBi_[j].getSigma();
      s2 += d;
      s2s+= d;
      n = pBi_[j].getNumObs();
      //
      // sets up the original parameter solution:
      pOrig_->setSolution(fs);
      pOrig_->setSigma(sqrt(s2s));
      pOrig_->setNumObs(n);
      // and statistics:
      if (isWorkingBand)
      {
        sumP2_   += 1.0/s2;
        sumX1P2_ += f  /s2;
        sumX2P2_ += f*f/s2;
        sumT1P2_ += t.toDouble()/s2;
      };
      break;
    case SgPartial::DD_RATE:
      d  = pBi_[j].getSolution();
      f  += d;
      break;
  };
  // return Ax part:
  return pOrig_->getD()*f;
};



//
double SgPwlStorageIncRates::calcRateSolution(const SgMJD& t)
{
  double                        f(0.0);
  // Rates only:
  int                           j(calcCurrentIdx(t));
  if (j < 0)
    j = 0;
  else if (j > numOfNodes_-1)
    j = numOfNodes_-1;
  for (int i=0; i<j; i++)
    f += step_*pBi_[i].getSolution();
  f += ((t - tStart_) - j*step_)*pBi_[j].getSolution();
  return f;
};



//
double SgPwlStorageIncRates::calcRateSigma(const SgMJD& t)
{
  // Rates only:
  int                           j(calcCurrentIdx(t));
  if (j < 0)
    j = 0;
  else if (j > numOfNodes_-1)
    j = numOfNodes_ - 1;
  return pBi_[j].getSigma()*step_;
};



//
double SgPwlStorageIncRates::calc_P_a(const SgSymMatrix& P, int idx, const SgMJD& t)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  double                        v(0.0);
  if (numOfPolynomials_ > 0)
  {
    double                      dt = (t - tRefer_);
    double                      dd = 1.0;
    switch (dDomain)
    {
      default:
      case SgPartial::DD_OFFSET:
        for (int i=0; i<numOfPolynomials_; i++)
        {
          v += P.getElement(idx, pAi_[i].getIdx())*dd;
          dd *= dt;
        };
        break;
      case SgPartial::DD_RATE:
        for (int i=1; i<numOfPolynomials_; i++)
        {
          v += i*P.getElement(idx, pAi_[i].getIdx())*dd;
          dd *= dt/*/DAY2SEC*/;
        };
        break;
    };
  };
  //
  int                           j=calcCurrentIdx(t);
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      for (int i=0; i<j; i++)
        v += P.getElement(idx, pBi_[i].getIdx()) * step_;
      v += P.getElement(idx, pBi_[j].getIdx()) * ((t - tStart_) - j*step_);
      break;
    case SgPartial::DD_RATE:
      v += P.getElement(idx, pBi_[j].getIdx());
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
double SgPwlStorageIncRates::calc_aT_P_a(const SgVector& vP_a, const SgMJD& t)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  double                        v(0.0);
  if (numOfPolynomials_ > 0)
  {
    double                      dt = (t - tRefer_);
    double                      dd = 1.0;
    switch (dDomain)
    {
      default:
      case SgPartial::DD_OFFSET:
        for (int i=0; i<numOfPolynomials_; i++)
        {
          v += vP_a.getElement(pAi_[i].getIdx())*dd;
          dd *= dt;
        };
        break;
      case SgPartial::DD_RATE:
        for (int i=1; i<numOfPolynomials_; i++)
        {
          v += i*vP_a.getElement(pAi_[i].getIdx())*dd;
          dd *= dt/*/DAY2SEC*/;
        };
        break;
    };
  };
  //
  int                           j=calcCurrentIdx(t);
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      for (int i=0; i<j; i++)
        v += vP_a.getElement(pBi_[i].getIdx()) * step_;
      v += vP_a.getElement(pBi_[j].getIdx()) * ((t - tStart_) - j*step_);
      break;
    case SgPartial::DD_RATE:
      v += vP_a.getElement(pBi_[j].getIdx());
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
void SgPwlStorageIncRates::getListOfActiveParameters(const SgMJD& t, QList<SgParameter*>& pars)
{
  pars.clear();
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  int                           idx(calcCurrentIdx(t));
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      for (int i=0; i<getNumOfPolynomials(); i++)
        pars.append(&pAi_[i]);
      for (int i=0; i<idx+1; i++)
        pars.append(&pBi_[i]);
      break;
    case SgPartial::DD_RATE:
      for (int i=1; i<getNumOfPolynomials(); i++)
        pars.append(&pAi_[i]);
      pars.append(&pBi_[idx]);
      break;
  };
};



//
/*
void SgPwlStorageIncRates::collectTraces4Sfo(const QList<SgParameter*> *listX, const SgSymMatrix& mPx)
{
  double                        aConstr=pOrig_->getSigmaAPriori();
  //
  QMap<QString, SgParameter*>   name2par;
  QMap<QString, int>            name2idx;
  for (int i=0; i<listX->size(); i++)
  {
    name2par[listX->at(i)->getName()] = listX->at(i);
    name2idx[listX->at(i)->getName()] = i;
  };
  //
  QList<SgParameter*>           l;
  // collect B_i parameters that are in this partial solution:
  for (int i=0; i<numOfNodes_; i++)
    if (name2par.contains(pBi_[i].getName()))
      l.append(&pBi_[i]);
  //
  // collect indices:
  int                          *idx2r;
  idx2r = new int[l.size()];
  for (int i=0; i<l.size(); i++)
    idx2r[i] = name2idx[l.at(i)->getName()];
  //
  // calc trace:
  double                        d;
  d = 0.0;
  for (int i=0; i<l.size(); i++)
    d += mPx.getElement(idx2r[i], idx2r[i])/aConstr/aConstr;

  trace_ += d;

  l.clear();
  delete[] idx2r;
};
*/
/*=====================================================================================================*/




/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
