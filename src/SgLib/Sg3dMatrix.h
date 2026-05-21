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

#ifndef SG_3D_MATRIX
#define SG_3D_MATRIX


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <ostream>
#include <SgMathSupport.h>
#include <Sg3dVector.h>


/***===================================================================================================*/
/**
 * A three-dimentional matrix. Should be useful for various geometrical calculations.
 *
 *
 */
/*=====================================================================================================*/
class Sg3dMatrix
{
protected:
  double        mat[3][3];              //!< The 3x3 array that is storing elements of a matrix.

public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object, fills it with zeros.
   */
  inline Sg3dMatrix();

  /**A constructor.
   * Creates a copy of the object from given reference.
   */
  inline Sg3dMatrix(const Sg3dMatrix&);

  /**A constructor.
   * Creates a matrix with columns equal to col0, col1 and col2.
   */
  inline Sg3dMatrix(const Sg3dVector& col0, const Sg3dVector& col1, const Sg3dVector& col2);

  /**A constructor.
   * Creates a matrix with given elements (first row, second row, third row) of doubles.
   */
  inline Sg3dMatrix(double a00, double a01, double a02,
                      double a10, double a11, double a12,
                      double a20, double a21, double a22);

  /**A destructor.
   * Destroys the object.
   */
  inline ~Sg3dMatrix(){};


  //
  // Interfaces:
  //
  /**Returns a reference on (i,j)-th element. Here i is index of a row, j -- column;
   * the valid ranges are [0,1,2]. The return value can be "left hand side", i.e.,
   * one could write A(i,j) = b.
   */
  inline double& operator()(DIRECTION i, DIRECTION j) {return mat[i][j];};

  /**Returns a value of (i,j)-th element. Here i is index of a row, j -- column;
   * the valid ranges are [0,1,2]. The function does not change the object.
   */
  inline double  at (DIRECTION  i, DIRECTION   j) const {return mat[i][j];};

  //
  // Functions:
  //
  /**Returns a determinant of a matrix.
   */
  inline double module() const;  /* determinant */

  /**Unifies a matrix, i.e., |M|==1.
   */
  inline void   unify() {operator/=(module());};

  /**Returns a transposed matrix (the original matrix does not change).
  */
  inline Sg3dMatrix T() const; /*  transpose: */

  /**Makes an assignment.
  */
  inline Sg3dMatrix& operator = (const Sg3dMatrix&);

  /**Increase a matrix with a value of a given matrix, i.e., user can write M+=A.
  */
  inline Sg3dMatrix& operator+= (const Sg3dMatrix&);

  /**Decrease a matrix with a value of a given matrix, i.e., user can write M-=A.
  */
  inline Sg3dMatrix& operator-= (const Sg3dMatrix&);

  /**Multiplies a matrix with a double.
  */
  inline Sg3dMatrix& operator*= (double);

  /**Divides a matrix on a double.
  */
  inline Sg3dMatrix& operator/= (double);

  /**Returns a sum of two matrices.
  */
  friend inline Sg3dMatrix  operator+  (const Sg3dMatrix&, const Sg3dMatrix&);

  /**Returns a difference of two matrices.
  */
  friend inline Sg3dMatrix  operator-  (const Sg3dMatrix&, const Sg3dMatrix&);

  /**Returns a matrix divided by scalar.
  */
  friend inline Sg3dMatrix  operator/  (const Sg3dMatrix&, double);

  /**Returns a product of a matrix and a scalar.
  */
  friend inline Sg3dMatrix  operator*  (const Sg3dMatrix&, double);

  /**Returns a product of a scalar and a matrix.
  */
  friend inline Sg3dMatrix  operator*  (double, const Sg3dMatrix&);

  /**Returns a matrix with inverted sign.
  */
  friend inline Sg3dMatrix  operator-  (const Sg3dMatrix&);

  /**Returns a product of a matrix and a vector.
  */
  friend inline Sg3dVector  operator*  (const Sg3dMatrix&, const Sg3dVector&);

  /**Returns a product of two matrices.
  */
  friend        Sg3dMatrix  operator*  (const Sg3dMatrix&, const Sg3dMatrix&);

  /**Returns inverted matrix: A*~A=~A*A=1 (the original matrix does not change).
  */
  friend        Sg3dMatrix  operator~  (const Sg3dMatrix&);


  //
  // I/O:
  //
  /**Makes an output to std::ostream, standard text formatted output.
   */
  friend std::ostream &operator<<(std::ostream& s, const Sg3dMatrix& M);
};
/*=====================================================================================================*/



/*=====================================================================================================*/
/*                                                                                                     */
/* Sg3dMatrix's inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
inline Sg3dMatrix::Sg3dMatrix()
{
  mat[0][0]=1.0; mat[0][1]=0.0; mat[0][2]=0.0;
  mat[1][0]=0.0; mat[1][1]=1.0; mat[1][2]=0.0;
  mat[2][0]=0.0; mat[2][1]=0.0; mat[2][2]=1.0;
};

inline Sg3dMatrix::Sg3dMatrix(const Sg3dVector& col0, const Sg3dVector& col1, const Sg3dVector& col2)
{
  mat[0][0]=col0.at(X_AXIS); mat[0][1]=col1.at(X_AXIS); mat[0][2]=col2.at(X_AXIS);
  mat[1][0]=col0.at(Y_AXIS); mat[1][1]=col1.at(Y_AXIS); mat[1][2]=col2.at(Y_AXIS);
  mat[2][0]=col0.at(Z_AXIS); mat[2][1]=col1.at(Z_AXIS); mat[2][2]=col2.at(Z_AXIS);
};

inline Sg3dMatrix::Sg3dMatrix(double a00, double a01, double a02,
                                  double a10, double a11, double a12,
                                  double a20, double a21, double a22)
{
  mat[0][0]=a00; mat[0][1]=a01; mat[0][2]=a02;
  mat[1][0]=a10; mat[1][1]=a11; mat[1][2]=a12;
  mat[2][0]=a20; mat[2][1]=a21; mat[2][2]=a22;
};

inline Sg3dMatrix::Sg3dMatrix(const Sg3dMatrix& M)
{
  mat[0][0]=M.mat[0][0]; mat[0][1]=M.mat[0][1]; mat[0][2]=M.mat[0][2];
  mat[1][0]=M.mat[1][0]; mat[1][1]=M.mat[1][1]; mat[1][2]=M.mat[1][2];
  mat[2][0]=M.mat[2][0]; mat[2][1]=M.mat[2][1]; mat[2][2]=M.mat[2][2];
};

inline Sg3dMatrix& Sg3dMatrix::operator=(const Sg3dMatrix& M)
{
  mat[0][0]=M.mat[0][0]; mat[0][1]=M.mat[0][1]; mat[0][2]=M.mat[0][2];
  mat[1][0]=M.mat[1][0]; mat[1][1]=M.mat[1][1]; mat[1][2]=M.mat[1][2];
  mat[2][0]=M.mat[2][0]; mat[2][1]=M.mat[2][1]; mat[2][2]=M.mat[2][2];
  return *this;
};

inline Sg3dMatrix& Sg3dMatrix::operator+= (const Sg3dMatrix& M)
{
  mat[0][0]+=M.mat[0][0]; mat[0][1]+=M.mat[0][1]; mat[0][2]+=M.mat[0][2];
  mat[1][0]+=M.mat[1][0]; mat[1][1]+=M.mat[1][1]; mat[1][2]+=M.mat[1][2];
  mat[2][0]+=M.mat[2][0]; mat[2][1]+=M.mat[2][1]; mat[2][2]+=M.mat[2][2];
  return *this;
};

inline Sg3dMatrix& Sg3dMatrix::operator-= (const Sg3dMatrix& M)
{
  mat[0][0]-=M.mat[0][0]; mat[0][1]-=M.mat[0][1]; mat[0][2]-=M.mat[0][2];
  mat[1][0]-=M.mat[1][0]; mat[1][1]-=M.mat[1][1]; mat[1][2]-=M.mat[1][2];
  mat[2][0]-=M.mat[2][0]; mat[2][1]-=M.mat[2][1]; mat[2][2]-=M.mat[2][2];
  return *this;
};

inline Sg3dMatrix& Sg3dMatrix::operator*= (double v)
{
  mat[0][0]*=v; mat[0][1]*=v; mat[0][2]*=v;
  mat[1][0]*=v; mat[1][1]*=v; mat[1][2]*=v;
  mat[2][0]*=v; mat[2][1]*=v; mat[2][2]*=v;
  return *this;
};

inline Sg3dMatrix& Sg3dMatrix::operator/= (double v)
{
  mat[0][0]/=v; mat[0][1]/=v; mat[0][2]/=v;
  mat[1][0]/=v; mat[1][1]/=v; mat[1][2]/=v;
  mat[2][0]/=v; mat[2][1]/=v; mat[2][2]/=v;
  return *this;
};

inline double Sg3dMatrix::module() const
{
  return
    mat[0][0]*(mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1]) -
    mat[0][1]*(mat[1][0]*mat[2][2] - mat[1][2]*mat[2][0]) +
    mat[0][2]*(mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0]) ;
};

inline Sg3dMatrix Sg3dMatrix::T() const
{
  return Sg3dMatrix(mat[0][0], mat[1][0], mat[2][0],
                      mat[0][1], mat[1][1], mat[2][1],
                      mat[0][2], mat[1][2], mat[2][2]);
};
/*=====================================================================================================*/



/*=====================================================================================================*/
/*                                                                                                     */
/* Sg3dMatrix's inline friends:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
inline Sg3dMatrix operator- (const Sg3dMatrix& M1)
{
  Sg3dMatrix M;
  M.mat[0][0] = -M1.mat[0][0];
  M.mat[0][1] = -M1.mat[0][1];
  M.mat[0][2] = -M1.mat[0][2];

  M.mat[1][0] = -M1.mat[1][0];
  M.mat[1][1] = -M1.mat[1][1];
  M.mat[1][2] = -M1.mat[1][2];

  M.mat[2][0] = -M1.mat[2][0];
  M.mat[2][1] = -M1.mat[2][1];
  M.mat[2][2] = -M1.mat[2][2];
  return M;
};
inline Sg3dMatrix operator+ (const Sg3dMatrix& M1, const Sg3dMatrix& M2)
{
  Sg3dMatrix M(M1);
  M.mat[0][0]+= M2.mat[0][0];
  M.mat[0][1]+= M2.mat[0][1];
  M.mat[0][2]+= M2.mat[0][2];

  M.mat[1][0]+= M2.mat[1][0];
  M.mat[1][1]+= M2.mat[1][1];
  M.mat[1][2]+= M2.mat[1][2];

  M.mat[2][0]+= M2.mat[2][0];
  M.mat[2][1]+= M2.mat[2][1];
  M.mat[2][2]+= M2.mat[2][2];
  return M;
};

inline Sg3dMatrix operator- (const Sg3dMatrix& M1, const Sg3dMatrix& M2)
{
  Sg3dMatrix M(M1);
  M.mat[0][0]-= M2.mat[0][0];
  M.mat[0][1]-= M2.mat[0][1];
  M.mat[0][2]-= M2.mat[0][2];

  M.mat[1][0]-= M2.mat[1][0];
  M.mat[1][1]-= M2.mat[1][1];
  M.mat[1][2]-= M2.mat[1][2];

  M.mat[2][0]-= M2.mat[2][0];
  M.mat[2][1]-= M2.mat[2][1];
  M.mat[2][2]-= M2.mat[2][2];
  return M;
};

inline Sg3dMatrix operator/ (const Sg3dMatrix& M1, double v2)
{
  Sg3dMatrix M(M1);
  M.mat[0][0]/=v2;  M.mat[0][1]/=v2;  M.mat[0][2]/=v2;
  M.mat[1][0]/=v2;  M.mat[1][1]/=v2;  M.mat[1][2]/=v2;
  M.mat[2][0]/=v2;  M.mat[2][1]/=v2;  M.mat[2][2]/=v2;
  return M;
};

inline Sg3dMatrix operator* (double v1, const Sg3dMatrix& M2)
{
  Sg3dMatrix M(M2);
  M.mat[0][0]*=v1;  M.mat[0][1]*=v1;  M.mat[0][2]*=v1;
  M.mat[1][0]*=v1;  M.mat[1][1]*=v1;  M.mat[1][2]*=v1;
  M.mat[2][0]*=v1;  M.mat[2][1]*=v1;  M.mat[2][2]*=v1;
  return M;
};

inline Sg3dMatrix operator* (const Sg3dMatrix& M1, double v2)
{
  Sg3dMatrix M(v2*M1);
  return M;
};

inline Sg3dVector  operator*(const Sg3dMatrix& M, const Sg3dVector& V)
{
  return
      Sg3dVector(M.mat[0][0]*V.vec[0] + M.mat[0][1]*V.vec[1] + M.mat[0][2]*V.vec[2],
                   M.mat[1][0]*V.vec[0] + M.mat[1][1]*V.vec[1] + M.mat[1][2]*V.vec[2],
                   M.mat[2][0]*V.vec[0] + M.mat[2][1]*V.vec[1] + M.mat[2][2]*V.vec[2]);
};

inline std::ostream &operator<<(std::ostream& s, const Sg3dMatrix& M)
{
  s << "|" << M.mat[0][0] << ", " << M.mat[0][1] << ", " << M.mat[0][2] << "|" << std::endl;
  s << "|" << M.mat[1][0] << ", " << M.mat[1][1] << ", " << M.mat[1][2] << "|" << std::endl;
  s << "|" << M.mat[2][0] << ", " << M.mat[2][1] << ", " << M.mat[2][2] << "|" << std::endl;
  return s;
};
//*====================================================================================================*/

#endif //SG_3D_MATRIX
