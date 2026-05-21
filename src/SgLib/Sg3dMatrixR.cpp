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

#include <math.h>
#include <Sg3dMatrixR.h>



/*=====================================================================================================*/
/*                                                                                                     */
/* Sg3dMatrixR implementation                                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
Sg3dMatrixR::Sg3dMatrixR(DIRECTION axis, double angle) : Sg3dMatrix(m3E)
{
  axis_ = axis;
  (*this)(angle);
};



//
Sg3dMatrixR::Sg3dMatrixR(const Sg3dMatrixR& rm) : Sg3dMatrix(rm)
{
  axis_ = rm.axis_;
  sincos((angle_=rm.angle_), &sinA_, &cosA_);
};



//
const Sg3dMatrix& Sg3dMatrixR::operator()(double angle)
{
  sincos(angle_=angle, &sinA_, &cosA_);
  switch (axis_)
    {
    case X_AXIS:
/*    mat[0][0]=  1.0;    mat[0][1]=  0.0;    mat[0][2]=  0.0; */
/*    mat[1][0]=  0.0; */ mat[1][1]= cosA_;    mat[1][2]= sinA_;
/*    mat[2][0]=  0.0; */ mat[2][1]=-sinA_;    mat[2][2]= cosA_;
      break;
    case Y_AXIS:
      mat[0][0]= cosA_; /* mat[0][1]=  0.0; */ mat[0][2]=-sinA_;
/*    mat[1][0]=  0.0;     mat[1][1]=  1.0;    mat[1][2]=  0.0; */
      mat[2][0]= sinA_; /* mat[2][1]=  0.0; */ mat[2][2]= cosA_;
      break;
    case Z_AXIS:
      mat[0][0]= cosA_;    mat[0][1]= sinA_; /* mat[0][2]=  0.0; */
      mat[1][0]=-sinA_;    mat[1][1]= cosA_; /* mat[1][2]=  0.0; */
/*    mat[2][0]=  0.0;     mat[2][1]=  0.0;     mat[2][2]=  1.0; */
      break;
    };
  return *this;
};
/*=====================================================================================================*/

