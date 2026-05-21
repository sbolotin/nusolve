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

#include <SgPwlStorageBSplineQ.h>

#include <SgTaskConfig.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPwlStorageBSplineQ::className()
{
  return "SgPwlStorageBSplineQ";
};



// An empty constructor:
SgPwlStorageBSplineQ::SgPwlStorageBSplineQ()
  : SgPwlStorage()
{
};



// A destructor:
SgPwlStorageBSplineQ::~SgPwlStorageBSplineQ()
{
};



//
int SgPwlStorageBSplineQ::getNumOfActiveParameters(const SgMJD&)
{
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
  int                           nAs, nBs(3);
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
int SgPwlStorageBSplineQ::getNumOfSegments()
{
  return numOfNodes_ - 2;
};



//
void SgPwlStorageBSplineQ::deployParameters(SgParameter* p,
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

  if (numOfPolynomials_ == 1) // no polynomials
    numOfPolynomials_ = 0;

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
  numOfNodes_ = floor((tFinis_ - tStart_)/step_) + 3;
  pBi_ = new SgParameter[numOfNodes_];
  for (int i=0; i<numOfNodes_; i++)
  {
    pBi_[i].setName(pOrig_->getName() + ": B_" + QString("").sprintf("%03d", i));
    pBi_[i].tune(*pOrig_);
    pBi_[i].setSigmaAPriori(pOrig_->getSigmaAPriori()*step_);
    pBi_[i].setPMode(SgParameterCfg::PM_LOC);
    if (true)
    {
      pBi_[i].setTLeft (tStart_ + (i - 2)*step_);
      pBi_[i].setTRight(tStart_ + (i + 1)*step_);
    }
    else
    {
      pBi_[i].setTLeft(tZero);
      pBi_[i].setTRight(tInf);
    };
  };
  pBi_[0            ].setTLeft (tStart_);
  pBi_[1            ].setTLeft (tStart_);
  pBi_[numOfNodes_-2].setTRight(tFinis_ + step_);
  pBi_[numOfNodes_-1].setTRight(tFinis_ + step_);


  sumP2_ = 0.0;
  sumX1P2_ = 0.0;
  sumX2P2_ = 0.0;
  sumT1P2_ = 0.0;
};



//
void SgPwlStorageBSplineQ::propagatePartials(const SgMJD& tt)
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
  double                        dt(t - tStart_), g(2.0*step_*step_), a;
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      a = -dt + (j+1)*step_;
      pBi_[j  ].setD(d* a*a/g);
      //
      a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
      pBi_[j+1].setD(d*   a/g);
      //
      a = dt - j*step_;
      pBi_[j+2].setD(d* a*a/g);
      break;
    case SgPartial::DD_RATE:
      a = -dt + (j+1)*step_;
      pBi_[j  ].setD(-2.0*d*a/g);
      //
      a = (-2.0*dt + (2*j+1)*step_);
      pBi_[j+1].setD( 2.0*d*a/g);
      //
      a = dt - j*step_;
      pBi_[j+2].setD( 2.0*d*a/g);
      break;
  };
  //
  pOrig_->zerofy();
};



//
double SgPwlStorageBSplineQ::calcAX(const SgMJD& t, bool isWorkingBand)
{
  double                        f(0.0), s2(0.0), d;
  SgPartial::DataDomain         dDomain=pOrig_->getDataDomain();
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
  //
  int                           j = calcCurrentIdx(t);
  double                         dt(t - tStart_), g(2.0*step_*step_), a;
  //
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      // j-th element:
      a = -dt + (j+1)*step_;
      d = a*a/g;
      b = pBi_[j].getSolution()*d;
      f  += b;
      fs += b;
      b = pBi_[j].getSigma()*pBi_[j].getSigma()*d*d;
      s2 += b;
      s2s+= b;
      // (j+1)-th element:
      a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
      d = a/g;
      b = pBi_[j+1].getSolution()*d;
      f  += b;
      fs += b;
      b = pBi_[j+1].getSigma()*pBi_[j+1].getSigma()*d*d;
      s2 += b;
      s2s+= b;
      // (j+2)-th element:
      a = dt - j*step_;
      d = a*a/g;
      b = pBi_[j+2].getSolution()*d;
      f  += b;
      fs += b;
      b = pBi_[j+2].getSigma()*pBi_[j+2].getSigma()*d*d;
      s2 += b;
      s2s+= b;
      //
      // sets up the original parameter solution:
      pOrig_->setSolution(fs);
      pOrig_->setSigma(sqrt(s2s));
      pOrig_->setNumObs(pBi_[j].getNumObs() + pBi_[j+1].getNumObs() + pBi_[j+2].getNumObs());
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
      a = -dt + (j+1)*step_;
      d = -2.0*a/g*pBi_[j  ].getSolution();
      f  += d;
      //
      a = (-2.0*dt + (2*j+1)*step_);
      d =  2.0*a/g*pBi_[j+1].getSolution();
      f  += d;
      //
      a = dt - j*step_;
      d =  2.0*a/g*pBi_[j+2].getSolution();
      f  += d;
      break;
  };
  // return Ax part:
  return pOrig_->getD()*f;
};



//
double SgPwlStorageBSplineQ::calcRateSolution(const SgMJD& t)
{
  int                           j=calcCurrentIdx(t);
  double                         f(0.0), dt(t - tStart_), g(2.0*step_*step_), d, a;
  //
  a = -dt + (j+1)*step_;
  d = a*a/g;
  f += pBi_[j  ].getSolution()*d;
  //
  a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
  d = a/g;
  f += pBi_[j+1].getSolution()*d;
  //
  a = dt - j*step_;
  d = a*a/g;
  f += pBi_[j+2].getSolution()*d;
  //
  return f;
};



//
double SgPwlStorageBSplineQ::calcRateSigma(const SgMJD& t)
{
  // B-splines only:
  int                           j=calcCurrentIdx(t);
  double                         s2(0.0), dt(t - tStart_), g(2.0*step_*step_), d, a;
  //
  a = -dt + (j+1)*step_;
  d = a*a/g;
  s2 += pBi_[j  ].getSigma()*pBi_[j  ].getSigma()*d*d;
  //
  a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
  d = a/g;
  s2 += pBi_[j+1].getSigma()*pBi_[j+1].getSigma()*d*d;
  //
  a = dt - j*step_;
  d = a*a/g;
  s2 += pBi_[j+2].getSigma()*pBi_[j+2].getSigma()*d*d;
  //
  return sqrt(s2);
};



//
double SgPwlStorageBSplineQ::calc_P_a(const SgSymMatrix& P, int idx, const SgMJD& t)
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
  double                         dt(t - tStart_), g(2.0*step_*step_), d_0, d_1, d_2, a;
  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      //
      a = -dt + (j+1)*step_;
      d_0 = a*a/g;
      //
      a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
      d_1 = a/g;
      //
      a = dt - j*step_;
      d_2 = a*a/g;
      //
      v +=
        P.getElement(idx, pBi_[j  ].getIdx())*d_0 +
        P.getElement(idx, pBi_[j+1].getIdx())*d_1 + 
        P.getElement(idx, pBi_[j+2].getIdx())*d_2 ;
      break;
    case SgPartial::DD_RATE:
      //
      a = -dt + (j+1)*step_;
      d_0 = -2.0*a/g;
      //
      a = (-2.0*dt + (2*j+1)*step_);
      d_1 = 2.0*a/g;
      //
      a = dt - j*step_;
      d_2 = 2.0*a/g;
      //
      v +=
        P.getElement(idx, pBi_[j  ].getIdx())*d_0 +
        P.getElement(idx, pBi_[j+1].getIdx())*d_1 + 
        P.getElement(idx, pBi_[j+2].getIdx())*d_2 ;
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
double SgPwlStorageBSplineQ::calc_aT_P_a(const SgVector& vP_a, const SgMJD& t)
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
  double                         dt(t - tStart_), g(2.0*step_*step_), d_0, d_1, d_2, a;

  switch (dDomain)
  {
    default:
    case SgPartial::DD_OFFSET:
      //
      a = -dt + (j+1)*step_;
      d_0 = a*a/g;
      //
      a = (dt - (j-1)*step_)*((j+1)*step_ - dt) + (dt - j*step_)*((j+2)*step_ - dt);
      d_1 = a/g;
      //
      a = dt - j*step_;
      d_2 = a*a/g;
      //
      v +=
        vP_a.getElement(pBi_[j  ].getIdx())*d_0 +
        vP_a.getElement(pBi_[j+1].getIdx())*d_1 +
        vP_a.getElement(pBi_[j+2].getIdx())*d_2 ;
      break;
    case SgPartial::DD_RATE:
      //
      a = -dt + (j+1)*step_;
      d_0 = -2.0*a/g;
      //
      a = (-2.0*dt + (2*j+1)*step_);
      d_1 = 2.0*a/g;
      //
      a = dt - j*step_;
      d_2 = 2.0*a/g;
      //
      v +=
        vP_a.getElement(pBi_[j  ].getIdx())*d_0 +
        vP_a.getElement(pBi_[j+1].getIdx())*d_1 +
        vP_a.getElement(pBi_[j+2].getIdx())*d_2 ;
      break;
  };
  //
  return pOrig_->getD()*v;
};



//
void SgPwlStorageBSplineQ::getListOfActiveParameters(const SgMJD& t, QList<SgParameter*>& pars)
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
      pars.append(&pBi_[idx+2]);
      break;
    case SgPartial::DD_RATE:
      for (int i=1; i<getNumOfPolynomials(); i++)
        pars.append(&pAi_[i]);
      pars.append(&pBi_[idx  ]);
      pars.append(&pBi_[idx+1]);
      pars.append(&pBi_[idx+2]);
      break;
  };
};



//
double SgPwlStorageBSplineQ::calcRateRms4Sfo()
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
