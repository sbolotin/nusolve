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

#ifndef SG_3D_MATRIX_R_H
#define SG_3D_MATRIX_R_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <Sg3dMatrix.h>


/***===================================================================================================*/
/**
 * A 3d matrix which is describing an elementary rotation around one of axises.
 *
 */
/*=====================================================================================================*/
class Sg3dMatrixR : public Sg3dMatrix
{
protected:
  double                 angle_;         //!< An angle of rotation.
  double                 sinA_, cosA_;
  DIRECTION              axis_;          //!< An axis of rotation.

public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object, direction Axis_ is a mandatory argument.
   */
  Sg3dMatrixR(DIRECTION axis_, double=0.0);

  /**A constructor.
   * Creates a copy of the object from given reference.
   */
  Sg3dMatrixR(const Sg3dMatrixR&);

  /**A destructor.
   * Destroys the object.
   */
  inline ~Sg3dMatrixR(){};

  //
  // Interfaces:
  //
  /**Returns a value of the angle of rotation, rad.
   */
  inline double angle() const {return angle_;};

  /**Returns an axis of rotation.
   */
  inline DIRECTION axis() const {return axis_;};

  //
  // Functions:
  //
  /**Returns a matrix, which is describing a rotation on the angle Angle around the axis Axis.
   */
  const Sg3dMatrix& operator()(double);
};
/*=====================================================================================================*/


#endif //SG_3D_MATRIX_R_H
