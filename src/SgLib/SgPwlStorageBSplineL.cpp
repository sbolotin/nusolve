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

#include <SgPwlStorageBSplineL.h>

#include <SgTaskConfig.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPwlStorageBSplineL::className()
{
  return "SgPwlStorageBSplineL";
};



// An empty constructor:
SgPwlStorageBSplineL::SgPwlStorageBSplineL() : 
  SgPwlStorage()
{
};



// A destructor:
SgPwlStorageBSplineL::~SgPwlStorageBSplineL()
{
};



//
int SgPwlStorageBSplineL::getNumOfActiveParameters(const SgMJD&)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  int                           nAs, nBs(2);
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      nAs = getNumOfPolynomials();
      break;
    case SgPartial::DD_RATE:
      nAs = getNumOfPolynomials()>0?getNumOfPolynomials()-1:0;
      break;
  };
  return nAs + nBs;
};



//
int SgPwlStorageBSplineL::getNumOfSegments()
{
  return numOfNodes_ - 1;
};



//
void SgPwlStorageBSplineL::deployParameters(SgParameter* p,
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
  //tRefer_.setTime(0.0); // 0hr
  tRefer_ = tRefer;
  step_ = pOrig_->getStep();
  // emulate SOLVE:
//  SgMJD       t0h(tN);
//  t0h.setTime(0); // 0hr
//  if (cfg->getIsSolveCompatible())

/*
  if (?)
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
//
//    if (step_ < tFinis_-tStart_)
//    {
//      tStart_ = t0h - ceil((t0h - t0)/step_)*step_;
//      tFinis_ = t0h + ceil((tN - t0h)/step_)*step_;
//    }
//    else
//    {
//      tStart_ = t0h - ceil((t0h - t0)*24.0)/24.0;
//      tFinis_ = t0h + ceil((tN - t0h)*24.0)/24.0;
//      step_ = tFinis_ - tStart_;
//    };
//
  }
  else 
*/ 
  
  // if the step equal to an interval of data set, no polynomial terms are expected:
  if (step_ > tFinis_-tStart_)
  {
    step_ = tFinis_ - tStart_ + 1.0E-4/DAY2SEC;
    numOfPolynomials_ = 0;
  }
  else
    numOfPolynomials_ = pOrig_->getNumOfPolynomials() /*- 1*/;

//  if (numOfPolynomials_ == 1) // no polynomials
//    numOfPolynomials_ = 0;

  if (numOfPolynomials_ > 0)
  {
    pAi_ = new SgParameter[numOfPolynomials_];
    for (int i=0; i<numOfPolynomials_; i++)
    {
      pAi_[i].setName(pOrig_->getName() + ": A_" + QString("").sprintf("%d", i));
      pAi_[i].tune(*pOrig_);
      pAi_[i].setSigmaAPriori(pOrig_->getSigmaAPrioriAux());
      pAi_[i].setPMode(SgParameterCfg::PM_LOC);
      pAi_[i].setTLeft(tZero);
      pAi_[i].setTRight(tInf);
    };
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() +
      "::deployParameters(): " + QString("").setNum(numOfPolynomials_) + 
      " polynomial parameters were allocated for " + pOrig_->getName() + " parameter");
  };

  // B-Spline nodes:
  // modify pOrig's a priori std.dev:
//  pOrig_->setSigmaAPriori(pOrig_->getSigmaAPriori()*step_);
  numOfNodes_ = floor((tFinis_ - tStart_)/step_) + 2;
  pBi_ = new SgParameter[numOfNodes_];
  for (int i=0; i<numOfNodes_; i++)
  {
    pBi_[i].setName(pOrig_->getName() + ": B_" + QString("").sprintf("%03d", i));
    pBi_[i].tune(*pOrig_);
    pBi_[i].setSigmaAPriori(p->getSigmaAPriori()*step_);
    pBi_[i].setPMode(SgParameterCfg::PM_LOC);
    if (true)
    {
      pBi_[i].setTLeft (tStart_ + (i - 1)*step_);
      pBi_[i].setTRight(tStart_ + (i + 1)*step_);
    }
    else // test:
    {
      pBi_[i].setTLeft(tZero);
      pBi_[i].setTRight(tInf);
    };
  };
  pBi_[0            ].setTLeft (tStart_);
  pBi_[numOfNodes_-1].setTRight(tFinis_);
  
  sumP2_ = 0.0;
  sumX1P2_ = 0.0;
  sumX2P2_ = 0.0;
  sumT1P2_ = 0.0;
};



//
void SgPwlStorageBSplineL::propagatePartials(const SgMJD& tt)
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
  //
  // B-Splines:
  int                           j=calcCurrentIdx(t);
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      pBi_[j  ].setD(d*((tStart_ - t) + (j+1)*step_)/step_);
      pBi_[j+1].setD(d*((t - tStart_) -     j*step_)/step_);
      break;
    case SgPartial::DD_RATE:
      pBi_[j  ].setD(-d/step_);
      pBi_[j+1].setD( d/step_);
      break;
  };
  //
  pOrig_->zerofy();
};



//
double SgPwlStorageBSplineL::calcAX(const SgMJD& t, bool isWorkingBand)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  double                        f(0.0), s2(0.0), d;
  int                           j, n;
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
  // B-Splines:
  double                        fs(0.0), s2s(s2), b;
  if (numOfPolynomials_==1)
    fs = f;
  j = calcCurrentIdx(t);

  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      //
      d = ((tStart_ - t) + (j+1)*step_)/step_;
      b = pBi_[j].getSolution()*d;
      f  += b;
      fs += b;
      b = pBi_[j].getSigma()*pBi_[j].getSigma()*d*d;
      s2 += b;
      s2s+= b;
      //
      d = ((t - tStart_) -     j*step_)/step_;
      b = pBi_[j+1].getSolution()*d;
      f  += b;
      fs += b;
      b = pBi_[j+1].getSigma()*pBi_[j+1].getSigma()*d*d;
      s2 += b;
      s2s+= b;
      //
      n = std::min(pBi_[j].getNumObs(), pBi_[j+1].getNumObs());
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
      d  = -pBi_[j  ].getSolution()/step_;
      f  += d;
      d  =  pBi_[j+1].getSolution()/step_;
      f  += d;
      break;
  };
  // return Ax part:
  return pOrig_->getD()*f;
};



//
double SgPwlStorageBSplineL::calcRateSolution(const SgMJD& t)
{
  double                        f(0.0), d, dt(t - tStart_);
  int                           j = calcCurrentIdx(t);
  //
  d = (-dt + (j+1)*step_)/step_;
  f += pBi_[j  ].getSolution()*d;
  //
  d = ( dt -     j*step_)/step_;
  f += pBi_[j+1].getSolution()*d;
  //
  // sets up the original parameter solution:
  return f;
};



//
double SgPwlStorageBSplineL::calcRateSigma(const SgMJD& t)
{
  // B-splines only:
  double                        s2(0.0), d, dt(t - tStart_);
  int                           j = calcCurrentIdx(t);
  //
  d = (-dt + (j+1)*step_)/step_;
  s2 += pBi_[j  ].getSigma()*pBi_[j  ].getSigma()*d*d;
  //
  d = ( dt -     j*step_)/step_;
  s2 += pBi_[j+1].getSigma()*pBi_[j+1].getSigma()*d*d;
  //
  return sqrt(s2);
};



//
double SgPwlStorageBSplineL::calc_P_a(const SgSymMatrix& P, int idx, const SgMJD& t)
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
      v += 
        P.getElement(idx, pBi_[j  ].getIdx()) * ((tStart_ - t) + (j+1)*step_)/step_ +
        P.getElement(idx, pBi_[j+1].getIdx()) * ((t - tStart_) -     j*step_)/step_ ;
      break;
    case SgPartial::DD_RATE:
      v += ( -P.getElement(idx, pBi_[j  ].getIdx()) + P.getElement(idx, pBi_[j+1].getIdx()) )/step_;
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
double SgPwlStorageBSplineL::calc_aT_P_a(const SgVector& vP_a, const SgMJD& t)
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
      v +=
        vP_a.getElement(pBi_[j  ].getIdx())*((tStart_ - t) + (j+1)*step_)/step_ +
        vP_a.getElement(pBi_[j+1].getIdx())*((t - tStart_) -     j*step_)/step_ ;
      break;
    case SgPartial::DD_RATE:
      v += ( -vP_a.getElement(pBi_[j].getIdx()) + vP_a.getElement(pBi_[j+1].getIdx()) )/step_;
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
void SgPwlStorageBSplineL::getListOfActiveParameters(const SgMJD& t, QList<SgParameter*>& pars)
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
      pars.append(&pBi_[idx  ]);
      pars.append(&pBi_[idx+1]);
      break;
    case SgPartial::DD_RATE:
      for (int i=1; i<getNumOfPolynomials(); i++)
        pars.append(&pAi_[i]);
      pars.append(&pBi_[idx  ]);
      pars.append(&pBi_[idx+1]);
      break;
  };
};



//
double SgPwlStorageBSplineL::calcRateRms4Sfo()
{
  double                        vi, vim1, sD2;
  sD2 = 0.0;
  for (int i=1; i<numOfNodes_; i++)
  {
    vi   = pBi_[i  ].getSolution();
    vim1 = pBi_[i-1].getSolution();
    sD2 += (vi - vim1)*(vi - vim1);
  };
  return sD2/step_/step_;
};
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
