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

#ifndef SG_EST_PWL_STORAGE_BSPLINE_L_H
#define SG_EST_PWL_STORAGE_BSPLINE_L_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <SgPwlStorage.h>


/***===================================================================================================*/
/**
 * SgPwlStorageBSplineL -- a specialized parameter to estimate.
 *
 */
/**====================================================================================================*/
class SgPwlStorageBSplineL : public SgPwlStorage
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgPwlStorageBSplineL();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgPwlStorageBSplineL();


  //
  // Interfaces:
  //
 
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
    const SgTaskConfig*);
  
  /**Propagates the partials.
   */
  virtual void propagatePartials(const SgMJD& t);

  virtual int getNumOfActiveParameters(const SgMJD& t);

  virtual int getNumOfSegments();

  virtual void getListOfActiveParameters(const SgMJD& t, QList<SgParameter*>& pars);

  virtual double calcAX(const SgMJD&, bool);

  virtual double calc_P_a(const SgSymMatrix&, int, const SgMJD&);

  virtual double calc_aT_P_a(const SgVector&, const SgMJD&);
  
  virtual double calcRateSolution(const SgMJD&);

  virtual double calcRateSigma(const SgMJD&);

//virtual double calcMeansResids4Sfo(double& sumSqr, double& rateSigma);
  virtual double calcRateRms4Sfo();
  
  //
  // Friends:
  //
	using SgPwlStorage::operator=;

  //
  // I/O:
  //
  // ...

private:
  // original/external parameter:
};
/*=====================================================================================================*/







/*=====================================================================================================*/
/*                                                                                                     */
/* SgPwlStorageSC inline members:                                                                      */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
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
#endif //SG_EST_PWL_STORAGE_BSPLINE_L_H
