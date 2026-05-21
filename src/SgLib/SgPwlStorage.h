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

#ifndef SG_EST_PWL_STORAGE_H
#define SG_EST_PWL_STORAGE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgParameter.h>



class SgTaskConfig;
class SgUtMatrix;
/***===================================================================================================*/
/**
 * SgPwlStorage -- virtual class, a skeleton for a specialized parameter to estimate.
 *
 */
/**====================================================================================================*/
class SgPwlStorage
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgPwlStorage();

  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgPwlStorage();


  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  virtual SgPwlStorage& operator=(const SgPwlStorage&);
 
  /**Returns a number of nodes in the PWL model.
   */
  inline int getNumOfNodes() const;

  /**Returns a number of polynomials in the PWL model.
   */
  inline int getNumOfPolynomials() const;

  inline double step() const;

  inline const SgMJD& tStart() const;

  inline const SgMJD& tFinis() const;
  
  inline const SgMJD& tRefer() const;

  inline SgMJD tMean() const;

  /**Returns a pointer on the Ai parameter.
   */
  inline SgParameter* getP_Ai(int);

  /**Returns a pointer on the Bi parameter.
   */
  inline SgParameter* getP_Bi(int);

  inline SgParameter* getPOrig();
  
  inline void setNumOfPolynomials(int);

  inline double trace() const {return trace_;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Deploys parameters.
   */
  virtual void deployParameters(SgParameter*, 
    const SgMJD& tStart, const SgMJD& tFinis, const SgMJD& tRefer, 
    const SgTaskConfig*) = 0;
  
  /**Propagates the partials.
   */
  virtual void propagatePartials(const SgMJD& t) = 0;

  inline int calcCurrentIdx(const SgMJD& t);
  
  virtual int getNumOfActiveParameters(const SgMJD& t) = 0;

  virtual int getNumOfSegments() = 0;

  virtual void getListOfActiveParameters(const SgMJD& t, QList<SgParameter*>& pars) = 0;


  /**Clear info.
   */
  void zerofy();

  virtual double calcAX(const SgMJD&, bool) = 0;

  virtual double calc_P_a(const SgSymMatrix&, int, const SgMJD&) = 0;

  virtual double calc_aT_P_a(const SgVector&, const SgMJD&) = 0;
  
  virtual double calcRateRms4Sfo();

  void collectTraces4Sfo(const QList<SgParameter*> *listX, const SgSymMatrix& mPx);

  double calcPolySolution(const SgMJD&);
  
  virtual double calcRateSolution(const SgMJD&) = 0;

  virtual double calcSolution(const SgMJD&);

  virtual double calcRateSigma(const SgMJD&);

  inline double calcMean();

  inline double calcWRMS();

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  // original/external parameter:
  SgParameter        *pOrig_;

  SgParameter        *pAi_;
  SgParameter        *pBi_;
  int                 numOfPolynomials_;
  int                 numOfNodes_;
  SgMJD               tStart_;
  SgMJD               tFinis_;
  SgMJD               tRefer_;
  double              step_;
  // for statistics/spoolfile outputs:
  double              sumP2_;
  double              sumX1P2_;
  double              sumX2P2_;
  double              sumT1P2_;
  bool                isPOrigOwner_;
  // exercise:
  double              trace_;
  
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
inline SgPwlStorage::SgPwlStorage() :
  tStart_(tZero),
  tFinis_(tZero),
  tRefer_(tZero)
{
  pOrig_ = NULL;
  pAi_ = NULL;
  pBi_ = NULL;
  numOfNodes_ = 0;
  numOfPolynomials_ = 0;
  sumP2_ = 0.0;
  sumX1P2_ = 0.0;
  sumX2P2_ = 0.0;
  sumT1P2_ = 0.0;
  isPOrigOwner_ = false;
  trace_ = 0.0;
};



// A destructor:
inline SgPwlStorage::~SgPwlStorage()
{
  if (isPOrigOwner_)
    delete pOrig_;
  pOrig_ = NULL; // not an owner
  if (pAi_)
  {
    delete[] pAi_;
    pAi_ = NULL;
  };
  if (pBi_)
  {
    delete[] pBi_;
    pBi_ = NULL;
  };
  numOfNodes_ = 0;
  numOfPolynomials_ = 0;
};



//
// INTERFACES:
//
//
//
inline int SgPwlStorage::getNumOfNodes() const
{
  return numOfNodes_;
};



//
inline int SgPwlStorage::getNumOfPolynomials() const
{
  return numOfPolynomials_;
};



//
inline double SgPwlStorage::step() const
{
  return step_;
};



//
inline const SgMJD& SgPwlStorage::tStart() const
{
  return tStart_;
};



//
inline const SgMJD& SgPwlStorage::tFinis() const
{
  return tFinis_; 
};



//
inline const SgMJD& SgPwlStorage::tRefer() const
{
  return tRefer_;
};



//
inline SgMJD SgPwlStorage::tMean() const
{
  return sumP2_>0.0 ? SgMJD(sumT1P2_/sumP2_) : tZero;
};



//
inline SgParameter* SgPwlStorage::getP_Bi(int idx)
{
  int i = (-1<idx && idx<numOfNodes_) ? idx : 0;
  return &pBi_[i];
};



//
inline SgParameter* SgPwlStorage::getP_Ai(int idx)
{
  int i = (-1<idx && idx<numOfPolynomials_) ? idx : 0;
  return &pAi_[i];
};



//
inline SgParameter* SgPwlStorage::getPOrig()
{
  return pOrig_;
};


// sets:
//
//
inline void SgPwlStorage::setNumOfPolynomials(int n)
{
  numOfPolynomials_ = n;
};


//
// FUNCTIONS:
//
//
//
inline int SgPwlStorage::calcCurrentIdx(const SgMJD& t)
{
  int                           idx;
  idx = floor((t - tStart_)/step_);
  if (idx < 0)
    idx = 0;
  if (numOfNodes_ <= idx)
    idx = numOfNodes_ - 1;
  return idx;
};



//
inline double SgPwlStorage::calcMean()
{
  return sumP2_>0.0 ? sumX1P2_/sumP2_ : 0.0;
};



//
inline double SgPwlStorage::calcWRMS()
{
  return sumP2_>0.0 ? sqrt((sumX2P2_ - sumX1P2_*sumX1P2_/sumP2_)/sumP2_) : 0.0;
};



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
#endif //SG_EST_PWL_STORAGE_H
