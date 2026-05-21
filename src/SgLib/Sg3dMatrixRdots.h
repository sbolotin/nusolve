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

#ifndef SG_3D_MATRIX_R_DOTS_H
#define SG_3D_MATRIX_R_DOTS_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <Sg3dMatrixR.h>




/***===================================================================================================*/
/**
 * A derivative matrix from elementary rotation matrix.
 */
/*=====================================================================================================*/
class Sg3dMatrixRdot : public Sg3dMatrixR
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object, direction Axis_ is a mandatory argument.
   */
  Sg3dMatrixRdot(DIRECTION axis, double angle=0.0) : Sg3dMatrixR(axis, angle)
  {
   switch (axis_)
    {
    case X_AXIS:
      mat[0][0]=  0.0;
      break;
    case Y_AXIS:
      mat[1][1]=  0.0;
      break;
    case Z_AXIS:
      mat[2][2]=  0.0;
      break;
    };
  };

  //
  // Functions:
  //
  /**Returns a matrix, which is describing a rotation on the angle Angle around the axis Axis.
   */
  const Sg3dMatrix& operator()(double);
};
/*=====================================================================================================*/


/***===================================================================================================*/
/**
 * A second derivative matrix from elementary rotation matrix.
 */
/*=====================================================================================================*/
class Sg3dMatrixR2dot : public Sg3dMatrixR
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object, direction Axis_ is a mandatory argument.
   */
  Sg3dMatrixR2dot(DIRECTION axis, double angle=0.0) : Sg3dMatrixR(axis, angle)
  {
   switch (axis_)
    {
    case X_AXIS:
      mat[0][0]=  0.0;
      break;
    case Y_AXIS:
      mat[1][1]=  0.0;
      break;
    case Z_AXIS:
      mat[2][2]=  0.0;
      break;
    };
  };

  //
  // Functions:
  //
  /**Returns a matrix, which is describing a rotation on the angle Angle around the axis Axis.
   */
  const Sg3dMatrix& operator()(double);
};
/*=====================================================================================================*/



#endif //SG_3D_MATRIX_R_DOTS_H
