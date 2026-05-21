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


/*
 *
 * This file is part of General Purpose Geodetic Library
 *
 * Some copyright issues...
 *
 */


#include <Sg3dMatrix.h>

// zero matrix:
const Sg3dMatrix m3Zero
(
  Sg3dVector(0.0, 0.0, 0.0),
  Sg3dVector(0.0, 0.0, 0.0),
  Sg3dVector(0.0, 0.0, 0.0)
);

// unit matrix:
const Sg3dMatrix m3E
(
  Sg3dVector(1.0, 0.0, 0.0),
  Sg3dVector(0.0, 1.0, 0.0),
  Sg3dVector(0.0, 0.0, 1.0)
);



/*=====================================================================================================*/
//
//  Sg3dMatrix implementation
//
/*=====================================================================================================*/
Sg3dMatrix operator* (const Sg3dMatrix& M1, const Sg3dMatrix& M2)
{
  Sg3dMatrix M;
  M.mat[0][0] = M1.mat[0][0]*M2.mat[0][0] + M1.mat[0][1]*M2.mat[1][0] + M1.mat[0][2]*M2.mat[2][0];
  M.mat[0][1] = M1.mat[0][0]*M2.mat[0][1] + M1.mat[0][1]*M2.mat[1][1] + M1.mat[0][2]*M2.mat[2][1];
  M.mat[0][2] = M1.mat[0][0]*M2.mat[0][2] + M1.mat[0][1]*M2.mat[1][2] + M1.mat[0][2]*M2.mat[2][2];

  M.mat[1][0] = M1.mat[1][0]*M2.mat[0][0] + M1.mat[1][1]*M2.mat[1][0] + M1.mat[1][2]*M2.mat[2][0];
  M.mat[1][1] = M1.mat[1][0]*M2.mat[0][1] + M1.mat[1][1]*M2.mat[1][1] + M1.mat[1][2]*M2.mat[2][1];
  M.mat[1][2] = M1.mat[1][0]*M2.mat[0][2] + M1.mat[1][1]*M2.mat[1][2] + M1.mat[1][2]*M2.mat[2][2];

  M.mat[2][0] = M1.mat[2][0]*M2.mat[0][0] + M1.mat[2][1]*M2.mat[1][0] + M1.mat[2][2]*M2.mat[2][0];
  M.mat[2][1] = M1.mat[2][0]*M2.mat[0][1] + M1.mat[2][1]*M2.mat[1][1] + M1.mat[2][2]*M2.mat[2][1];
  M.mat[2][2] = M1.mat[2][0]*M2.mat[0][2] + M1.mat[2][1]*M2.mat[1][2] + M1.mat[2][2]*M2.mat[2][2];
  return M;
};
/*=====================================================================================================*/



/*=====================================================================================================*/
//
// Sg3dMatrix's friends:
//
/*=====================================================================================================*/
Sg3dMatrix operator~ (const Sg3dMatrix& M1)
{
  double                        d(M1.module());
  Sg3dMatrix                    M;

  M.mat[0][0] = (M1.mat[1][1]*M1.mat[2][2] - M1.mat[1][2]*M1.mat[2][1])/d;
  M.mat[0][1] = (M1.mat[2][1]*M1.mat[0][2] - M1.mat[0][1]*M1.mat[2][2])/d;
  M.mat[0][2] = (M1.mat[0][1]*M1.mat[1][2] - M1.mat[1][1]*M1.mat[0][2])/d;

  M.mat[1][0] = (M1.mat[2][0]*M1.mat[1][2] - M1.mat[1][0]*M1.mat[2][2])/d;
  M.mat[1][1] = (M1.mat[0][0]*M1.mat[2][2] - M1.mat[2][0]*M1.mat[0][2])/d;
  M.mat[1][2] = (M1.mat[1][0]*M1.mat[0][2] - M1.mat[0][0]*M1.mat[1][2])/d;

  M.mat[2][0] = (M1.mat[1][0]*M1.mat[2][1] - M1.mat[2][0]*M1.mat[1][1])/d;
  M.mat[2][1] = (M1.mat[2][0]*M1.mat[0][1] - M1.mat[0][0]*M1.mat[2][1])/d;
  M.mat[2][2] = (M1.mat[0][0]*M1.mat[1][1] - M1.mat[1][0]*M1.mat[0][1])/d;
  return M;
};
/*=====================================================================================================*/
