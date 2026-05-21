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

#ifndef SG_CUBIC_SPLINE_H
#define SG_CUBIC_SPLINE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgMatrix.h>
#include <SgVector.h>



/***===================================================================================================*/
/**
 * SgCubicSpline.
 *
 */
/**====================================================================================================*/
class SgCubicSpline
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgCubicSpline(int numOfRecords, int dimension);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgCubicSpline();



  //
  // Interfaces:
  //

  /**Returns a number of records in the table.
   */
  inline int numOfRecords() const;

  /**Returns dimension of the splined table.
   */
  inline int dimension() const;

  inline bool isOk() const;

  inline SgVector& argument();

  inline SgMatrix& table();

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  void prepare4Spline();
  
  double spline(double arg, int nColumn, double &r);


  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  int                           dimension_;
  int                           numOfRecords_;
  SgVector                      argument_;
  SgMatrix                      table_;
  SgMatrix                      coeffs_;
  bool                          isOk_;
  
  void solveSpline(int);
  
  inline double h(int i) const {return argument_.getElement(i) - argument_.getElement(i-1);};
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgCubicSpline inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
// CONSTRUCTORS:
//
//
// An empty constructor:
inline SgCubicSpline::SgCubicSpline(int numOfRecords, int dimension) :
  argument_(numOfRecords),
  table_(numOfRecords, dimension),
  coeffs_(numOfRecords, dimension)
{
  numOfRecords_ = numOfRecords;
  dimension_ = dimension;
  isOk_ = false;
};



// A destructor:
inline SgCubicSpline::~SgCubicSpline()
{
};



//
// INTERFACES:
//
//
//
inline int SgCubicSpline::dimension() const
{
  return dimension_;
};



//
inline int SgCubicSpline::numOfRecords() const
{
  return numOfRecords_;
};



//
inline bool SgCubicSpline::isOk() const
{
  return isOk_;
};



// sets:
//
inline SgVector& SgCubicSpline::argument()
{
  return argument_;
};



//
inline SgMatrix& SgCubicSpline::table()
{
  return table_;
};



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
#endif //SG_CUBIC_SPLINE_H
