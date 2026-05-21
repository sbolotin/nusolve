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

#ifndef SG_WRMSABLE_H
#define SG_WRMSABLE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <iostream>

#include <QtCore/QString>

#include <SgMJD.h>



enum DataType
{
  DT_DELAY                    = 0,
  DT_RATE                     = 1,
};


class QDataStream;


class SgTaskConfig;
class SgVlbiObservable;



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgWrmsable
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgWrmsable(DataType);

  inline SgWrmsable(const SgWrmsable&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgWrmsable();



  //
  // Interfaces:
  //
  /**Returns type of data.
   */
  inline DataType getDataType() const {return dataType_;};

  /**Returns total number of observations.
   */
  inline int getNumTotal() const {return numTotal_;};

  /**Returns number of potentially usable observations.
   */
  inline int getNumUsable() const {return numUsable_;};

  /**Returns number of processed observations.
   */
  inline int getNumProcessed() const {return numProcessed_;};

  /**Returns std.dev adjustment.
   */
  inline double getSigma2add() const {return sigma2add_;};

  /**Returns MJD of the first observation related to the object.
   */
  inline const SgMJD& tFirst() const {return tFirst_;};

  /**Returns MJD of the last observation related to the object.
   */
  inline const SgMJD& tLast() const {return tLast_;};

  /**Returns normalized residuals.
   */
  inline double getNormedResid() const {return normedResid_;};

  /**Returns normalized residuals for spoolfile output -- SOLVE version.
   */
  inline double getSFF_NrmRes() const {return sff_NrmRes_;};

  /**Returns aux normalized residuals for spoolfile output -- SOLVE version.
   */
  inline double getSFF_NrmRes_plus_15() const {return sff_NrmRes_plus_15_;};

  //
  // sets:
  inline void setDataType(DataType dType) {dataType_ = dType;};

  /**Sets number of total observations.
   * \param n -- number of total observations;
   */
  inline void setNumTotal(int n) {numTotal_ = n;};
  
  inline void setNumUsable(int n) {numUsable_ = n;};

  /**Sets number of processed observations.
   * \param n -- number of processed observations;
   */
  inline void setNumProcessed(int n) {numProcessed_ = n;};

//  /**Sets an epoch of the first observations.
//   * \param t -- epoch;
//   */
//  inline void setTFirst(const SgMJD& t) {tFirst_ = t;};

//  /**Sets an epoch of the last observations.
//   * \param t -- epoch;
//   */
//  inline void setTLast(const SgMJD& t) {tLast_ = t;};

  inline void setSigma2add(double s) {sigma2add_ = s;};

  /**Sets value of normalized residuals.
   * \param d -- normalized residulas;
   */
  inline void setNormedResid(double d) {normedResid_ = d;};

  /**Sets value of normalized residuals for spoolfile output -- SOLVE version.
   * \param d -- normalized residulas;
   */
  inline void setSFF_NrmRes(double d) {sff_NrmRes_ = d;};

  /**Sets value of aux normalized residuals for spoolfile output -- SOLVE version.
   * \param d -- normalized residulas;
   */
  inline void setSFF_NrmRes_plus_15(double d) {sff_NrmRes_plus_15_ = d;};


  //
  // Functions:
  //
  /**Returns (non-reduced) Chi-squared for the object.
   */
  inline double chi2() const {return sumWrms2_;};

  /**Returns reduced Chi-squared for the object.
   */
  inline double reducedChi2() const {return chi2()/dof();};

  /**Returns coefficient to scale a weight.
   */
  inline double weightScaleCoefficient() const {return sumW_;};

  /**Increments total number of observations.
   */
  inline void incNumTotal(int inc=1) {numTotal_ += inc;};

  /**Decrements total number of observations.
   */
  inline void decNumTotal(int inc=1) {numTotal_ -= inc;};

  inline void incNumUsable(int inc=1) {numUsable_ += inc;};
  inline void decNumUsable(int inc=1) {numUsable_ -= inc;};
  
  /**Clears accumulated W.R.M.S. statistics.
   */
  inline void clearWrms();
  
  /**Adds a residual to the accumulated W.R.M.S. statistics.
   * \param o_c -- the residual;
   * \param o_c -- its weight (usually, it is sigma^(-2));
   */
  void addWrms(const SgMJD& t, double o_c, double w, double auxSum);

  /**Calculates and returns dispersion of residuals for the object.
   */
  inline double dispersion(bool isSolveCompatible) const;

  /**Calculates and returns W.R.M.S. for the object.
   */
  inline double wrms() const;

  inline double dof() const;

  void recalcSigma2add(double minSigma);

  void copyStatistics(SgWrmsable* wrms);

  inline void resetAllEditings();
  
  void calcNormalizedResiduals(const QList<SgVlbiObservable*> &observables, const SgTaskConfig* cfg);
  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);



private:
  DataType                      dataType_;
  int                           numTotal_;
  int                           numUsable_;
  int                           numProcessed_;
  double                        sumW_;          // sum of weights
  double                        sumWrms1_;      // sum of weighted residuals
  double                        sumWrmsWrms_;   // sum of (weighted residuals)^2
  double                        sumWrms2_;      // sum of weighted residuals*residuals
  double                        sumAuxOne_;
  double                        sumAuxTwo_;
  double                        sigma2add_;
  SgMJD                         tFirst_;
  SgMJD                         tLast_;
  //
  // auxiliaries (mostly, for emulating CALC/SOLVE report outputs):
  double                        normedResid_;
  double                        sff_NrmRes_;
  double                        sff_NrmRes_plus_15_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObjectInfo inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgWrmsable::SgWrmsable(DataType dType) :
  tFirst_(tInf),
  tLast_(tZero)
{
  dataType_ = dType;
  numTotal_ = 0;
  numUsable_ = 0;
  numProcessed_ = 0;
  sumW_ = 0.0;
  sumWrms1_ = 0.0;
  sumWrmsWrms_ = 0.0;
  sumWrms2_ = 0.0;
  sumAuxOne_ = 0.0;
  sumAuxTwo_ = 0.0;
  sigma2add_ = 0.0;
  normedResid_ = 0.0;
  sff_NrmRes_ = 0.0;
  sff_NrmRes_plus_15_ = 0.0;
};



//
inline SgWrmsable::SgWrmsable(const SgWrmsable& w) :
  tFirst_(w.tFirst()),
  tLast_(w.tLast())
{
  dataType_ = w.getDataType();
  numTotal_ = w.getNumTotal();
  numUsable_ = w.getNumUsable();
  numProcessed_ = w.getNumProcessed();
  sumW_ = w.sumW_;
  sumWrms1_ = w.sumWrms1_;
  sumWrmsWrms_ = w.sumWrmsWrms_;
  sumWrms2_ = w.sumWrms2_;
  sumAuxOne_ = w.sumAuxOne_;
  sumAuxTwo_ = w.sumAuxTwo_;
  sigma2add_ = w.sigma2add_;
  normedResid_ = w.normedResid_;
  sff_NrmRes_ = w.sff_NrmRes_;
  sff_NrmRes_plus_15_ = w.sff_NrmRes_plus_15_;
};



// A destructor:
inline SgWrmsable::~SgWrmsable()
{
  // nothing to do
};



//
// INTERFACES:
//
//


//
// FUNCTIONS:
//
//
inline double SgWrmsable::dispersion(bool isSolveCompatible) const
{
  if (numProcessed_ <= 1 || sumW_ == 0.0)
    return 0.0;
  return isSolveCompatible?
// old (pre 0.5.1):
//->sqrt(sumWrmsWrms_/(numProcessed_ - 1))/sumW_ :
    sqrt(sumWrms2_/(numProcessed_ - 1)) :
// dispersion as it should be:
    sqrt((sumWrms2_ - sumWrms1_*sumWrms1_/sumW_)/sumW_);
};



//
inline void SgWrmsable::clearWrms()
{
  numUsable_ = numProcessed_ = 0;
  sumW_ = sumWrms1_ = sumWrmsWrms_ = sumWrms2_ = sumAuxOne_ = sumAuxTwo_ = normedResid_ = 
    sff_NrmRes_ = sff_NrmRes_plus_15_ = 0.0; 
  tFirst_ = tInf;
  tLast_ = tZero;
};



//
inline double SgWrmsable::wrms() const 
{
  return (sumW_>0.0 && numProcessed_>0) ? sqrt(sumWrms2_/sumW_) : 0.0;
};



//
inline double SgWrmsable::dof() const 
{
  return numProcessed_>1 ? numProcessed_-sumAuxOne_ : 1.0;
};



//
inline void SgWrmsable::resetAllEditings()
{
  clearWrms();
  sigma2add_ = 0.0;
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
#endif //SG_WRMSABLE_H
