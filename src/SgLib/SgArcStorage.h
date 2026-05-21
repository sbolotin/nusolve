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

#ifndef SG_EST_ARC_STORAGE_H
#define SG_EST_ARC_STORAGE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgParameter.h>




class SgVector;
class SgSymMatrix;




/***===================================================================================================*/
/**
 * SgArcParamater -- a storage for specialized parameters to estimate.
 *
 */
/**====================================================================================================*/
class SgArcStorage
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgArcStorage();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgArcStorage();



  //
  // Interfaces:
  //
  /**An assignment operator.
  */
  SgArcStorage& operator=(const SgArcStorage&);

  /**Returns a number of pieces in the PWL model.
   */
  inline int getNum() const;

  /**Returns a pointer on the Pi-th parameter.
   */
  inline SgParameter*   getPi(int);

  inline SgParameter*   getPOrig();

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  inline int calcCurrentIdx(const SgMJD& t);

  /**Deploys parameters.
   */
  void deployParameters(SgParameter*, const SgMJD& tStart, const SgMJD& tFinis, const SgMJD& tRefer);
  
  /**Propagates the partials.
   */
  void propagatePartials(const SgMJD& t);

  double calcAX(const SgMJD&, bool);

  double calc_P_a(const SgSymMatrix&, int, const SgMJD&);

  double calc_aT_P_a(const SgVector&, const SgMJD&);


  double calcSolution(const SgMJD&);
  
  double calcSigma(const SgMJD&);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  // original/external parameter:
  SgParameter       *pOrig_;

  // arcs:
  SgParameter       *pPi_;
  int                num_;
  SgMJD              tStart_;
  SgMJD              tFinis_;
  double             step_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgPwlStorage inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgArcStorage::SgArcStorage() : 
  tStart_(tZero),
  tFinis_(tZero)
{
  pOrig_ = NULL;
  pPi_ = NULL;
  num_ = 0;
};



// A destructor:
inline SgArcStorage::~SgArcStorage()
{
  pOrig_ = NULL; // not an owner
  if (pPi_)
  {
    delete[] pPi_;
    pPi_ = NULL;
  };
  num_ = 0;
};



//
// INTERFACES:
//
//
//
inline int SgArcStorage::getNum() const
{
  return num_;
};



//
inline SgParameter* SgArcStorage::getPi(int idx)
{
  int i=(-1<idx && idx<num_) ? idx : 0;
  return &pPi_[i];
};



//
inline SgParameter* SgArcStorage::getPOrig()
{
  return pOrig_;
};



//
inline int SgArcStorage::calcCurrentIdx(const SgMJD& t)
{
  int                           idx;
  idx = floor((t - tStart_)/step_);
  if (idx < 0)
    idx = 0;
  if (num_ <= idx)
    idx = num_ - 1;
  return idx;
};



// sets:
//


//
// FUNCTIONS:
//
//
//


//
// FRIENDS:
//
//
//


/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//
/*=====================================================================================================*/
#endif //SG_EST_ARC_STORAGE_H
