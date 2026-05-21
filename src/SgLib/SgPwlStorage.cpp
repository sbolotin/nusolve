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

#include <SgPwlStorage.h>

#include <SgTaskConfig.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPwlStorage::className()
{
  return "SgPwlStorage";
};



//
SgPwlStorage& SgPwlStorage::operator=(const SgPwlStorage& s)
{
  tStart_ = s.tStart();
  tFinis_ = s.tFinis();
  tRefer_ = s.tRefer();
  if (isPOrigOwner_)
    delete pOrig_;
  pOrig_  = new SgParameter(*s.pOrig_);
  isPOrigOwner_ = true; // that is for reporter
  step_   = s.step();
  numOfNodes_ = s.getNumOfNodes();
  numOfPolynomials_ = s.getNumOfPolynomials();
  if (pAi_)
    delete pAi_;
  pAi_    = new SgParameter[numOfPolynomials_];
  for (int i=0; i<numOfPolynomials_; i++)
    pAi_[i] = s.pAi_[i];
  if (pBi_)
    delete pBi_;
  pBi_    = new SgParameter[numOfNodes_];
  for (int i=0; i<numOfNodes_; i++)
    pBi_[i] = s.pBi_[i];

  sumP2_   = s.sumP2_;
  sumX1P2_ = s.sumX1P2_;
  sumX2P2_ = s.sumX2P2_;
  sumT1P2_ = s.sumT1P2_;

  trace_ = s.trace_;

  return *this;
};



//
void SgPwlStorage::zerofy()
{
  for (int i=0; i<numOfPolynomials_; i++)
    pAi_[i].zerofy();
  for (int i=0; i<numOfNodes_; i++)
    pBi_[i].zerofy();
};



//
double SgPwlStorage::calcSolution(const SgMJD& t)
{
  return calcPolySolution(t) + calcRateSolution(t);
};



//
double SgPwlStorage::calcPolySolution(const SgMJD& t)
{
  double                        f(0.0);
  // Polynomials only:
  if (numOfPolynomials_)
  {
    double                      dt = (t - tRefer_);
    double                      dd = 1.0;
    for (int i=0; i<numOfPolynomials_; i++)
    {
      f += pAi_[i].getSolution()*dd;
      dd *= dt;
    };
  };
  return f;
};



//
double SgPwlStorage::calcRateSigma(const SgMJD& t)
{
  // Rates only:
  int                           j(calcCurrentIdx(t));
  if (j < 0)
    j = 0;
  else if (j > numOfNodes_-1)
    j = numOfNodes_ - 1;
  return pBi_[j].getSigma()*step_;
};



/*
double SgPwlStorage::calcMeansResids4Sfo(double& sumSqr, double& rateSigma)
{
  double                        v, s;
  double                        sWX2, sWX1, sW, sX, sX2;

  sWX2 = sWX1 = sW = sX = sX2 = 0.0;
  for (int i=0; i<numOfNodes_; i++)
  {
    v = pBi_[i].getSolution();
    s = pBi_[i].getSigma();
    sX   += v;
    sW   += 1.0/s/s;
    sWX1 += v/s/s;
    sWX2 += v*v/s/s;
    sX2  += v*v;
  };
//wrmsRate = sqrt(fabs(sWX2 - sWX1*sWX1/sW)/sW);
  sumSqr = sX2;
  rateSigma = 1.0;
  return sX/numOfNodes_;
};
*/



//
double SgPwlStorage::calcRateRms4Sfo()
{
  double                        v, sX2;
  sX2 = 0.0;
  for (int i=0; i<numOfNodes_; i++)
  {
    v = pBi_[i].getSolution();
    sX2  += v*v;
  };
  return sX2;
};



//
void SgPwlStorage::collectTraces4Sfo(const QList<SgParameter*> *listX, const SgSymMatrix& mPx)
{
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
  {
    double                      aConstr=l.at(i)->getSigmaAPriori();
    d += mPx.getElement(idx2r[i], idx2r[i])/aConstr/aConstr;
  };
  trace_ += d;
  l.clear();
  delete[] idx2r;
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
