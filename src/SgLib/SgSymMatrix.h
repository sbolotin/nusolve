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

#ifndef SG_SYM_MATRIX_H
#define SG_SYM_MATRIX_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgUtMatrix.h>




/***===================================================================================================*/
/**
 * A symmetrical matrix. SRIF uses it.
 *
 */
/*=====================================================================================================*/
class SgSymMatrix: public SgUtMatrix
{
 public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes or later assignments).
   */
  inline SgSymMatrix();
    
  /**A constructor.
   * Creates a copy of an object.
   * \param N -- number of elements in a row or column.
   * \param IsNeedClear -- if set to TRUE fills the array with zeros, if set to "false" 
   *        skips this procedure and the state of a matrix depends on compiler realization.
   */
  inline SgSymMatrix(unsigned int N, bool IsNeedClear = true);
    
  /**A constructor.
   * Creates a copy of an object.
   * \param P -- upper-triangular matrix that will be copied into the new object.
   */
  inline SgSymMatrix(const SgSymMatrix& P);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgSymMatrix();



  //
  // Interfaces:
  //
  /**Returns a reference at (i,j)-th element of the matrix, so
   * P(i,j)=x is working.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [0,N-1].
   */
  inline double& operator()(unsigned int i, unsigned int j);

  /**Returns a value of (i,j)-th element. The matrix does not change.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [0,N-1].
   */
  inline double getElement(unsigned int i, unsigned int j) const;

  /**Changes the (i,j)-th element.
   * \param i -- an index; valid ranges are: [0,N-1];
   * \param j -- an index; valid ranges are: [0,N-1];
   * \param d -- a new value for i-th element.
   */
  inline void setElement(unsigned int i, unsigned int j, double d);

  inline double** &base() {return B_;};

  inline const double* const* base_c() const {return B_;};


  //
  // Functions:
  //
  /**Assign a matrix to another one.
   * \param P -- a matrix to copy.
   */
  inline SgSymMatrix& operator=(const SgSymMatrix& P);

  /**Increments the matrix by the another one.
   * \param P -- a matrix to add.
   */
  inline SgSymMatrix& operator+=(const SgSymMatrix& P);

  /**Decrements the matrix by the another one.
   * \param P -- a matrix to substract.
   */
  inline SgSymMatrix& operator-=(const SgSymMatrix& P);

  /**Fills the matrix with a double.
   * \param d -- a value to fill the matrix.
   */
  inline SgSymMatrix& operator=(double d);

  /**Multiplies the matrix by a scalar.
   * \param d -- a scale.
   */
  inline SgSymMatrix& operator*=(double d);

  /**Divides the matrix by a scalar.
   * \param d -- a divider.
   */
  inline SgSymMatrix& operator/=(double d);

  /**Returns transposed matrix (the original matrix doesn't change).
   */
  inline SgSymMatrix T() const;



  //
  // Friends:
  //
  /**Returns a matrix with an inverted sign.
   * \param P -- an original matrix.
   */
  friend inline SgSymMatrix operator-(const SgSymMatrix& P);

  /**Returns a matrix divided by a scalar.
   * \param P -- a matrix;
   * \param d -- a divider;
   */
  friend inline SgSymMatrix operator/(const SgSymMatrix& P, double d);

  /**Returns a matrix multiplied by a scalar.
   * \param P -- a matrix;
   * \param d -- a scale.
   */
  friend inline SgSymMatrix operator*(const SgSymMatrix& P, double d);

  /**Returns a natrix multiplied by a scalar.
   * \param d -- a scale.
   * \param P -- a matrix;
   */
  friend inline SgSymMatrix operator*(double d, const SgSymMatrix& P);

  /**Returns a sum of two matrices.
   * \param P1 is a first term,
   * \param P2 is a second term in the sum.
   */
  friend inline SgSymMatrix operator+(const SgSymMatrix& P1, const SgSymMatrix& P2);

  /**Returns a difference of two matrices.
   * \param P1 is a first term,
   * \param P2 is a second term in the sum.
   */
  friend inline SgSymMatrix operator-(const SgSymMatrix& P1, const SgSymMatrix& P2);

  /**Makes a product of a matrix and a vector.
   * \param R -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgSymMatrix& P, const SgVector& V);

  friend SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, const SgSymMatrix& P2);


  //
  // I/O:
  //
  // ...



private:
  /**Returns inversed matrix.
   * This is time consumed operation, shouldn't use in ordinary operations.
   * R*~R == ~R*R == E (original matrix doesn't change)..
   */
  friend SgSymMatrix operator~(const SgSymMatrix& P);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgSymMatrix's inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgSymMatrix::SgSymMatrix()
  : SgUtMatrix() 
{
  // nothing to do
};



// A regular constructor:
inline SgSymMatrix::SgSymMatrix(unsigned int N, bool IsNeedClear)
  : SgUtMatrix(N, IsNeedClear) 
{
  // nothing to do
};



// A copying constructor:
inline SgSymMatrix::SgSymMatrix(const SgSymMatrix& P)
  : SgUtMatrix(P) 
{
  // nothing to do
};



// A destructor:
inline SgSymMatrix::~SgSymMatrix()
{
  // nothing to do
};




//
// INTERFACES:
//
// returns a reference on (i,j)-th element:
inline double& SgSymMatrix::operator()(unsigned int i, unsigned int j)
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: double& SgSymMatrix::operator()(unsigned int i, unsigned int j):"
              << " row's index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: double& SgSymMatrix::operator()(unsigned int i, unsigned int j):"
              << " column's index [" << j + "] out of range [0.." << NCol_-1 << "]\n";
#endif //DEBUG

  return (i<NRow_ && j<NCol_)?*(*(B_+std::max(j,i))+std::min(i,j)):dTmp_;
};



// returns a value of (i,j)-th element:
inline double SgSymMatrix::getElement(unsigned int i, unsigned int j) const
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: double SgSymMatrix::getElement(unsigned int, unsigned int):"
              << " row's index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: double SgSymMatrix::getElement(unsigned int, unsigned int):"
              << " column's index [" << j << "] out of range [0.." << NCol_-1 << "]\n";
#endif //DEBUG

  return (i<NRow_ && j<NCol_)?*(*(B_+std::max(j,i))+std::min(i,j)):0.0;
};



// sets a value of (i,j)-th element:
inline void SgSymMatrix::setElement(unsigned int i, unsigned int j, double d)
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: void SgSymMatrix::setElement(unsigned int i, unsigned int j,"
              << " double d): row's index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: void SgSymMatrix::setElement(unsigned int i, unsigned int j,"
              << " double d): column's index [" << j + "] out of range [0.." << NCol_-1 << "]\n";
#endif //DEBUG
  
  if (i<NRow_ && j<NCol_)
    *(*(B_+std::max(j,i))+std::min(i,j)) = d;
};




//
// FUNCTIONS:
//
//
inline SgSymMatrix& SgSymMatrix::operator=(const SgSymMatrix& P)
{
  return (SgSymMatrix&)(SgUtMatrix::operator=(P));
};



//
inline SgSymMatrix& SgSymMatrix::operator+=(const SgSymMatrix& P)
{
  return (SgSymMatrix&)(SgUtMatrix::operator+=(P));
};



//
inline SgSymMatrix& SgSymMatrix::operator-=(const SgSymMatrix& P)
{
  return (SgSymMatrix&)(SgUtMatrix::operator-=(P));
};



//
inline SgSymMatrix& SgSymMatrix::operator=(double d)
{
  return (SgSymMatrix&)(SgUtMatrix::operator=(d));
};



//
inline SgSymMatrix& SgSymMatrix::operator*=(double d)
{
  return (SgSymMatrix&)(SgUtMatrix::operator*=(d));
};



//
inline SgSymMatrix& SgSymMatrix::operator/=(double d)
{
  return (SgSymMatrix&)(SgUtMatrix::operator/=(d));
};



// in this case, just returns a copy of the matrix:
inline SgSymMatrix SgSymMatrix::T() const
{
  return SgSymMatrix(*this);
};




//
// FRIENDS:
//
//
inline SgSymMatrix operator*(const SgSymMatrix& P, double d) 
{
  return SgSymMatrix(P)*=d;
};



//
inline SgSymMatrix operator*(double d, const SgSymMatrix& P) 
{
  return SgSymMatrix(P)*=d;
};



//
inline SgSymMatrix operator/(const SgSymMatrix& P, double d) 
{
  return SgSymMatrix(P)/=d;
};



//
inline SgSymMatrix operator-(const SgSymMatrix& P)
{
  return SgSymMatrix(P)*=-1.0;
};



//
inline SgSymMatrix operator+(const SgSymMatrix& P1, const SgSymMatrix& P2) 
{
  return SgSymMatrix(P1)+=P2;
};



//
inline SgSymMatrix operator-(const SgSymMatrix& P1, const SgSymMatrix& P2) 
{
  return SgSymMatrix(P1)-=P2;
};



// disabled:
inline SgSymMatrix operator~(const SgSymMatrix& P)
{
  return SgSymMatrix(P);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
//
// aux functions:
//
// output to std stream:
/**Makes output to std::ostream, standard text formatted output.
  * \param s -- a stream;
  * \param P -- a matrix.
  */
std::ostream &operator<<(std::ostream& s, const SgSymMatrix& P);



#endif // SG_SYM_MATRIX_H
