/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2012-2020 Sergei Bolotin.
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

#include <SgArcStorage.h>
#include <SgLogger.h>
#include <SgSymMatrix.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgArcStorage::className()
{
  return "SgArcStorage";
};



//
SgArcStorage& SgArcStorage::operator=(const SgArcStorage& s)
{
  pOrig_  = new SgParameter(*s.pOrig_);
  num_    = s.num_;
  tStart_ = s.tStart_;
  tFinis_ = s.tFinis_;
  step_   = s.step_;
  pPi_    = new SgParameter[num_];
  for (int i=0; i<num_; i++)
    pPi_[i] = s.pPi_[i];
  return *this;
};



//
void SgArcStorage::deployParameters(SgParameter* p, 
  const SgMJD& t0, const SgMJD& tN, const SgMJD& /*tFinis*/)
{
  pOrig_ = p;
  tStart_ = t0;
  tFinis_ = tN;
  step_ = pOrig_->getStep();

  num_ = ceil((tFinis_ - tStart_)/step_);
  // arcs:
  pPi_ = new SgParameter[num_];
  for (int i=0; i<num_; i++)
  {
    pPi_[i].setName(pOrig_->getName() + ": P_" + QString("").sprintf("%03d", i));
    pPi_[i].tune(*p);
    pPi_[i].setPMode(SgParameterCfg::PM_LOC);
    pPi_[i].setTLeft(tStart_ + i*step_);
    pPi_[i].setTRight(tStart_ + (i + 1)*step_);
  };
};



//
void SgArcStorage::propagatePartials(const SgMJD& t)
{
  if (!pOrig_->isAttr(SgPartial::Attr_IS_PARTIAL_SET))
    return;
  if (t<tStart_ || tFinis_<t)
    return;
//int                           j=floor((t - tStart_)/step_);
  int                           j=calcCurrentIdx(t);
  pPi_[j].setD(pOrig_->getD());
  pOrig_->zerofy();
};



//
double SgArcStorage::calcAX(const SgMJD& t, bool)
{
//int             j=floor((t - tStart_)/step_);
  int             j=calcCurrentIdx(t);
  pOrig_->setSolution(pPi_[j].getSolution());
  pOrig_->setSigma(pPi_[j].getSigma());
  pOrig_->setNumObs(pPi_[j].getNumObs());
  return pOrig_->getD()*pPi_[j].getSolution();
};



//
double SgArcStorage::calc_P_a(const SgSymMatrix& P, int idx, const SgMJD& t)
{
//int             j=floor((t - tStart_)/step_);
  int             j=calcCurrentIdx(t);
  return P.getElement(idx, pPi_[j].getIdx())*pOrig_->getD();
};



//
double SgArcStorage::calc_aT_P_a(const SgVector& vP_a, const SgMJD& t)
{
//int             j=floor((t - tStart_)/step_);
  int             j=calcCurrentIdx(t);
  return pOrig_->getD()*vP_a.getElement(pPi_[j].getIdx());
};



//
double SgArcStorage::calcSolution(const SgMJD& t)
{
  int             j=calcCurrentIdx(t);
  return pPi_[j].getSolution();
};



//
double SgArcStorage::calcSigma(const SgMJD& t)
{
  int             j=calcCurrentIdx(t);
  return pPi_[j].getSigma();
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
