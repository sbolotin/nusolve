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

#ifndef SG_VECTOR_H
#define SG_VECTOR_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
//  check for mem* functions:
#   ifndef HAVE_MEMSET
#   error: "need right C-Library: Your C-Library doesn't have memset() function"
#   endif //HAVE_MEMSET
#   ifndef HAVE_MEMCPY
#   error: "need right C-Library: Your C-Library doesn't have memcpy() function"
#   endif //HAVE_MEMCPY
#endif


#include <stdlib.h>
#include <iostream>
#include <string.h>


#include <SgMathSupport.h>




class SgVector;
class SgMatrix;
class SgUtMatrix;
class SgSymMatrix;


extern const SgVector vZero;
/***===================================================================================================*/
/**
 * A vector. Should be useful for various matrix calculations.
 *
 */
/*=====================================================================================================*/
class SgVector
{
  friend class SgMatrix;
  friend class SgUtMatrix;
  
 public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes).
   */
  inline SgVector();
  
  /**A constructor.
   * Creates a copy of an object.
   * \param N -- number of elements.
   * \param IsNeedClear -- if set to true fills the array with zeros, if set to false
   *        skips this procedure and the state of *B depend on compiler realization.
   */
  inline SgVector(unsigned int N, bool IsNeedClear);
  
  /**A constructor.
   * Creates a copy of an object.
   * \param V -- vector that will be copied into the new object.
   */
  inline SgVector(const SgVector& V);
  
  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVector();

  inline void reSize(unsigned int n);
  
  //
  // Interfaces:
  //
  /**Returns number of elements in the vector.
   */
  inline unsigned int n() const;
  
  /**Returns reference at i-th element of the vector.
   * \param i -- an index; valid ranges are: [0,N-1].
   */
  inline double& operator()(unsigned int i);
  
  /**Changes the i-th element.
   * \param i -- an index; valid ranges are: [0,N-1];
   * \param d -- a new value for i-th element.
   */
  inline void setElement(unsigned int i, double d);
  
  /**Returns a value of i-th element.
   * \param i -- an index; valid ranges are: [0,N-1].
   */
  inline double getElement(unsigned int i) const;
  
  
  //
  // Functions:
  //
  /**Returns a length of the vector.
   */
  inline double module() const;
  
  /**Fills elements with zero.
   */
  inline void clear();
  
  /**Assign the vector to the another one.
   * \param V -- a vector to copy.
   */
  inline SgVector& operator=(const SgVector& V);
  
  /**Fills the vector with a double.
   * \param d -- a value to fill the vector.
   */
  inline SgVector& operator=(double d);
  
  /**Increments vector by another one.
   * \param V -- a vector to add.
   */
  inline SgVector& operator+=(const SgVector& V);
  
  /**Decrements vector by another one.
   * \param V -- a vector to substract.
   */
  inline SgVector& operator-=(const SgVector& V);
  
  /**Multiplies vector by scalar.
   * \param d -- a scale.
   */
  inline SgVector& operator*=(double d);
  
  /**Divides vector by scalar.
   * \param d -- a divider.
   */
  inline SgVector& operator/=(double d);
  
  /**Comperes the vector with another one, returns TRUE if the vectors are equal.
   * \param V -- a vector for comparing.
   */
  inline bool operator==(const SgVector& V) const;
  
  /**Comperes the vector with another one, returns TRUE if the vectors are different.
   * \param V -- a vector for comparing.
   */
  inline bool operator!=(const SgVector& V) const;
  
  
  //
  // Friends:
  //
  /**Returns a vector with an inverted sign (unary operator).
   * \param V -- an original vector.
   */
  friend inline SgVector operator-(const SgVector& V);
  
  /**Returns a vector divided by scalar.
   * \param V -- a vector;
   * \param d -- a divider;
   */
  friend inline SgVector operator/(const SgVector& V, double d);
  
  /**Returns a vector multiplied by scalar.
   * \param V -- a vector;
   * \param d -- a scale.
   */
  friend inline SgVector operator*(const SgVector& V, double d);
  
  /**Returns a vector multiplied by scalar.
   * \param d -- a scale.
   * \param V -- a vector;
   */
  friend inline SgVector operator*(double d, const SgVector& V);
  
  /**Returns a sum of two vectors.
   * \param V1 is a first term,
   * \param V2 is a second term in the sum.
   */
  friend inline SgVector operator+(const SgVector& V1, const SgVector& V2);
  
  /**Returns a difference of two vectors.
   * \param V1 is a first term,
   * \param V2 is a second term in the sum.
   */
  friend inline SgVector operator-(const SgVector& V1, const SgVector& V2);
  
  /**Makes a scalar product of two vectors.
   * \param V1 -- a first term in the product;
   * \param V2 -- a second term in the product.
   */
  friend inline double operator*(const SgVector& V1, const SgVector& V2);
  
  /**Makes a product of a matrix and a vector.
   * \param M -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgMatrix& M, const SgVector& V);

  /**Makes a product of a matrix and a vector.
   * \param R -- an upper triangular matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgUtMatrix& R, const SgVector& V);
  
  /**Makes a product of a matrix and a vector.
   * \param P -- a symmetric matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgSymMatrix& P, const SgVector& V);

  /**Makes a product of a transposed matrix and a vector.
   * \param M -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector calcProduct_matT_x_vec(const SgMatrix&, const SgVector&);

  /**Makes a product of a transposed upper triangular matrix and a vector.
   * \param M -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector calcProduct_matT_x_vec(const SgUtMatrix&, const SgVector&);
  
  inline double* &base() {return B_;};
  inline const double* base_c() const {return B_;};
  
  //
  // I/O:
  //
  // ...


 protected:
  unsigned int          N_;     //!< An number of elements in a vector (dimension).
  double               *B_;     //!< A pointer on a first element of the vector.
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVector's inline members:                                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVector::SgVector() 
{
  B_ = NULL;
  N_ = 0;
};



// A regular constructor:
inline SgVector::SgVector(unsigned int N, bool IsNeedClear = true) 
{
  B_ = new double[N_=N];
  if (IsNeedClear) 
    clear();
};



// A copying constructor:
inline SgVector::SgVector(const SgVector& V)
{
  B_ = new double[N_=V.N_];
  memcpy((void*)B_, (const void*)V.B_, N_*sizeof(double));
};



// A destructor:
inline SgVector::~SgVector()
{
  if (B_)
    delete[] B_;
  B_ = NULL;
  N_ = 0;
};



//
inline void SgVector::reSize(unsigned int n)
{
  if (B_)
    delete[] B_;
  B_ = new double[N_=n];
  clear();
};
 
 
 
//
// INTERFACES:
//
// dimension of a vector:
inline unsigned int SgVector::n() const
{
  return N_;
};



// a reference to i-th element (e.g., it is possible to write `v(i) = a'):
inline double& SgVector::operator()(unsigned int i) 
{
#ifdef DEBUG
  if (N_<=i)
    std::cerr << "WARNING: double& SgVector::operator()(unsigned int i) :"
              << " incompatible index of the vector (" << i << "), greater than " << N_ << "\n";
#endif //DEBUG
  return (i<N_)?*(B_+i):*B_;
};



// one part of operator(): set a value of i-th element:
inline void SgVector::setElement(unsigned int i, double d)
{
#ifdef DEBUG
  if (N_<=i)
    std::cerr << "WARNING: double& SgVector::setElement(unsigned int i, double d) :"
              << " incompatible index of the vector (" << i << "), greater than " << N_ << "\n";
#endif //DEBUG
  if(i<N_) 
    *(B_+i) = d;
};



// second part of operator(): get a value of i-th element:
inline double SgVector::getElement(unsigned int i) const
{
#ifdef DEBUG
  if (N_<=i)
    std::cerr << "WARNING: double& SgVector::getElement(unsigned int i) const :"
              << " incompatible index of the vector (" << i << "), greater than " << N_ << "\n";
#endif //DEBUG
  return (i<N_)?*(B_+i):0.0;
};



//
// FUNCTIONS:
//
// evaluates a length of a vector:
inline double SgVector::module() const
{
  double            d=0.0, *b=B_;
  for(unsigned int i=0; i<N_; i++,b++)
    d += *b**b;
  return sqrt(d);
};



// fills all elements with zeros:
inline void SgVector::clear() 
{
  memset((void*)B_, 0, sizeof(double)*N_);
};



// copy one vector into another one:
inline SgVector& SgVector::operator=(const SgVector& V)
{
  if(N_!=V.N_)  // adjust dimensions:
  {
    //   delete previously allocated array:
    if (B_) 
      delete[] B_;
    //   create a new one:
    B_ = new double[N_=V.N_];
  };

  //make a copy:
  memcpy((void*)B_, (const void*)V.B_, N_*sizeof(double));
  return *this;
};



// fills all elements with a scalar:
inline SgVector& SgVector::operator=(double d)
{
  double            *b=B_;
  for(unsigned int i=0; i<N_; i++)
    *b++ = d;
  return *this;
};



// increment:
inline SgVector& SgVector::operator+=(const SgVector& V)
{
#ifdef DEBUG
  if (N_!=V.N_)
    std::cerr << "WARNING: double& SgVector::operator+=(const SgVector& V) :"
              << " the dimensions of vectors ( " << N_ << " and " << V.N_ << " ) are different.\n";
#endif //DEBUG
  double            *b=B_, *v=V.B_;
  unsigned int      N=std::min(N_, V.N_);
  for(unsigned int i=0; i<N; i++)
    *b++ += *v++;
  return *this;
};



// decrement:
inline SgVector& SgVector::operator-=(const SgVector& V)
{
#ifdef DEBUG
  if (N_!=V.N_)
    std::cerr << "WARNING: double& SgVector::operator-=(const SgVector& V) :"
              << " the dimensions of vectors ( " << N_ << " and " << V.N_ << " ) are different.\n";
#endif //DEBUG
  double            *b=B_, *v=V.B_;
  unsigned int      N=std::min(N_, V.N_);
  for(unsigned int i=0; i<N; i++)
    *b++ -= *v++;
  return *this;
};



// mult by a scalar:
inline SgVector& SgVector::operator*=(double d)
{
  double            *b=B_;
  for(unsigned int i=0; i<N_; i++)
    *b++ *= d;
  return *this;
};



// divide by a scalar:
inline SgVector& SgVector::operator/=(double d)
{
  double            *b=B_;
  for(unsigned int i=0; i<N_; i++)
    *b++ /= d;
  return *this;
};



// is equal?
inline bool SgVector::operator==(const SgVector& V) const
{
  bool              IsEqual=true;
  if (N_!=V.N_)     // nothing to complain, we can compare vectors that have different dimension,
    return !IsEqual;// they just not equal.

  double            *b=B_, *v=V.B_;
  unsigned int      i=0;
  while (i++<N_ && (IsEqual = IsEqual && *b++==*v++)) ;
  return IsEqual;
};



// is not equal?
inline bool SgVector::operator!=(const SgVector& V) const
{
  return !(*this==V);
};



// minus sign:
inline SgVector operator-(const SgVector& V)
{
  return SgVector(V)*=-1.0;
};



// vector x scalar:
inline SgVector operator*(const SgVector& V, double d) 
{
  return SgVector(V)*=d;
};



// scalar x vector:
inline SgVector operator*(double d, const SgVector& V) 
{
  return SgVector(V)*=d;
};
  


// vector / scalar:
inline SgVector operator/(const SgVector& V, double d) 
{
  return SgVector(V)/=d;
};



// vector + vector:
inline SgVector operator+(const SgVector& V1, const SgVector& V2) 
{
  return SgVector(V1)+=V2;
};



// vector - vector:
inline SgVector operator-(const SgVector& V1, const SgVector& V2)
{
  return SgVector(V1)-=V2;
};



// vector x vector:
inline double operator*(const SgVector& V1, const SgVector& V2)
{
#ifdef DEBUG
  if (V1.N_!=V2.N_)
    std::cerr << "WARNING: double operator*(const SgVector& V1, const SgVector& V2) :"
              << " the dimensions of vectors ( " << V1.N_ << " and " << V2.N_ 
              << " ) are different.\n";
#endif //DEBUG
  double            *v1=V1.B_, *v2=V2.B_, d=0.0;
  unsigned int      N=std::min(V1.N_, V2.N_);
  for(unsigned int i=0; i<N; i++) 
    d += *v1++**v2++;
  return d;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//
// output to std stream:
/**Makes output to std::ostream, a standard text formatted output.
 * \param s -- a stream;
 * \param V -- a vector.
 */
std::ostream &operator<<(std::ostream& s, const SgVector& V);



/*=====================================================================================================*/
#endif // SG_VECTOR_H
