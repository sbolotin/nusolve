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

#ifndef SG_MATRIX_H
#define SG_MATRIX_H


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


#include <math.h>

#include <stdio.h>
#include <iostream>

#include <SgMathSupport.h>
#include <SgVector.h>



class SgUtMatrix;
class SgSymMatrix;

/***==============================================================================================*/
/**
 * A matrix. Should be useful for various matrix calculations.
 *
 */
/*================================================================================================*/
class SgMatrix
{
  //friend class SgUtMatrix;
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes or later assignments).
   */
  inline SgMatrix();

  /**A constructor.
   * Creates a copy of an object.
   * \param NRow -- number of rows.
   * \param NCol -- number of columns.
   * \param IsNeedClear -- if set to TRUE fills the array with zeros, if set to FALSE skips this
   *        procedure and the state of a matrix depends on compiler realization.
   */
  inline SgMatrix(unsigned int NRow, unsigned int NCol, bool IsNeedClear = true);

  /**A constructor.
   * Creates a copy of an object.
   * \param M -- matrix that will be copied into the new object.
   */
  inline SgMatrix(const SgMatrix& M);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgMatrix();


  //
  // Interfaces:
  //
  /**Returns number of rows in the matrix.
   */
  inline unsigned int nRow() const;

  /**Returns number of columns in the matrix.
   */
  inline unsigned int nCol() const;

  /**Returns a reference at (i,j)-th element of the matrix.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [0,N-1].
   */
  inline double& operator()(unsigned int i, unsigned int j);

  /**Returns a value of (i,j)-th element.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [0,N-1].
   */
  inline double getElement(unsigned int i, unsigned int j) const;

  /**Changes the (i,j)-th element.
   * \param i -- an index; valid ranges are: [0,N-1];
   * \param j -- an index; valid ranges are: [0,N-1];
   * \param d -- a new value for (i,j)-th element.
   */
  inline void setElement(unsigned int i, unsigned int j, double d);



  //
  // Functions:
  //
  /**Fills all elements with zero.
   */
  inline void clear();

  /**Returns transposed matrix (original matrix doesn't changed).
   */
  inline SgMatrix T() const;

  /**Assign the matrix to the another one.
   * \param M -- a matrix to copy.
   */
  SgMatrix& operator=(const SgMatrix& M);

  /**Fills the matrix with a double.
   * \param d -- a value to fill the matrix.
   */
  inline SgMatrix& operator=(double d);

  /**Increments the matrix by another one.
   * \param M -- a matrix to add.
   */
  inline SgMatrix& operator+=(const SgMatrix& M);

  /**Decrements the matrix by another one.
   * \param M -- a matrix to substract.
   */
  inline SgMatrix& operator-=(const SgMatrix& M);

  /**Multiplies the matrix by a scalar.
   * \param d -- a scale.
   */
  inline SgMatrix& operator*=(double d);

  /**Divides the matrix by a scalar.
   * \param d -- a divider.
   */
  inline SgMatrix& operator/=(double d);

  /**Compares the matrix with another one, returns TRUE if the matrices are equal.
   * \param M -- a matrix for comparing.
   */
  inline bool operator==(const SgMatrix& M) const;

  /**Comperes the matrix with another one, returns TRUE if the matrices are different.
   * \param M -- a matrix for comparing.
   */
  inline bool operator!=(const SgMatrix& M) const;



  //
  // Friends:
  //
  /**Returns a matrix with an inverted sign.
   * \param M -- an original matrix.
   */
  friend inline SgMatrix operator-(const SgMatrix& M);

  /**Returns a matrix divided by a scalar.
   * \param M -- a matrix;
   * \param d -- a divider;
   */
  friend inline SgMatrix operator/(const SgMatrix& M, double d);

  /**Returns a matrix multiplied by a scalar.
   * \param M -- a matrix;
   * \param d -- a scale.
   */
  friend inline SgMatrix operator*(const SgMatrix& M, double d);

  /**Returns a natrix multiplied by a scalar.
   * \param d -- a scale.
   * \param M -- a matrix;
   */
  friend inline SgMatrix operator*(double d, const SgMatrix& M);

  /**Returns a sum of two matrices.
   * \param M1 is a first term,
   * \param M2 is a second term in the sum.
   */
  friend inline SgMatrix operator+(const SgMatrix& M1, const SgMatrix& M2);

  /**Returns a difference of two matrices.
   * \param M1 is a first term,
   * \param M2 is a second term in the sum.
   */
  friend inline SgMatrix operator-(const SgMatrix& M1, const SgMatrix& M2);

  /**Makes a product of a matrix and a vector.
   * \param M -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgMatrix& M, const SgVector& V);

  /**Makes a product of two matrices.
   * \param M1 -- a first term in the product;
   * \param M2 -- a second term in the product.
   */
  // friend inline SgMatrix operator*(const SgMatrix& M1, const SgMatrix& M2);
  friend SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, const SgMatrix& M2);
  friend SgMatrix calcProduct_mat_x_mat(const SgUtMatrix& U1, const SgMatrix& M2);
  friend SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, const SgSymMatrix& P2);

   /**Makes a product of a transposed matrix and a matrix, i.e., M1^T*M2.
   * \param M1 -- a matrix that should be transposed;
   * \param M2 -- a matrix, the second term in the product.
   */
  friend SgMatrix calcProduct_matT_x_mat(const SgMatrix& M1, const SgMatrix& M2);

   /**Makes a product of a matrix and a transposed matrix, i.e., M1*M2^T.
   * \param M1 -- a matrix, the first term in the product,
   * \param M2 -- a matrix that should be transposed.
   */
  friend SgMatrix calcProduct_mat_x_matT(const SgMatrix& M1, const SgMatrix& M2);

  /**Makes a product of a transposed matrix and a vector.
   * \param M -- a  that should be transposed;
   * \param V -- a vector.
   */
  friend SgVector calcProduct_matT_x_vec(const SgMatrix& M, const SgVector& V);

  inline double** &base() {return B_;};

  inline const double* const* base_c() const {return B_;};
  
  //
  // I/O:
  //
  // ...



protected:
  unsigned int          NRow_;   //!< An number of rows in a matrix.
  unsigned int          NCol_;   //!< An number of columns in a matrix.
  double                **B_;    //!< A pointer on a pointer of a first element of the matrix.
  double                dTmp_;   //!< Local temporary variable.
};
/*==============================================================================================*/




/*================================================================================================*/
/*                                                                                                */
/* SgMatrix's inline members:                                                                   */
/*                                                                                                */
/*================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgMatrix::SgMatrix()
{
  B_    = NULL;
  NCol_ = NRow_ = 0;
  dTmp_ = 0.0;
};



// A regular constructor:
inline SgMatrix::SgMatrix(unsigned int NRow, unsigned int NCol, bool IsNeedClear)
{
/*
#ifdef DEBUG
  if (NRow == 0)
    std::cerr << "WARNING: SgMatrix::SgMatrix(unsigned int NRow, unsigned int NCol, bool IsNeedClear): "
              << "row index is zero\n";
  if (NCol == 0)
    std::cerr << "WARNING: SgMatrix::SgMatrix(unsigned int NRow, unsigned int NCol, bool IsNeedClear): "
              << "column index is zero\n";
#endif //DEBUG
*/
  dTmp_ = 0.0;
  NRow_ = NRow;
  B_    = new double*[NCol_=NCol];
  double            **w = B_;
  for (unsigned int i=0; i<NCol_; i++, w++)
  {
    *w = new double[NRow_];
    if (IsNeedClear) 
      memset((void*)(*w), 0, sizeof(double)*NRow_);
  };
};



// A copying constructor:
inline SgMatrix::SgMatrix(const SgMatrix& M)
{
  dTmp_ = 0.0;
  NRow_ = M.NRow_;
  B_    = new double*[NCol_=M.NCol_];
  double            **w=B_, **q=M.B_;
  for (unsigned int i=0; i<NCol_; i++,w++,q++)
  {
    *w = new double[NRow_];
    memcpy((void*)(*w), (const void*)(*q), NRow_*sizeof(double));
  };
};



// A destructor:
inline SgMatrix::~SgMatrix()
{
  if (B_)
  {
    double          **w=B_;
    for (unsigned int i=0; i<NCol_; i++,w++)
      delete[] *w;
    delete[] B_;
    B_ = NULL;
    NCol_ = NRow_ = 0;
  };
};




//
// INTERFACES:
//
// returns number of rows:
inline unsigned int SgMatrix::nRow() const
{
  return NRow_;
};



// returns number of columns:
inline unsigned int SgMatrix::nCol() const
{
  return NCol_;
};



//
inline double& SgMatrix::operator()(unsigned int i, unsigned int j)
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: double& SgMatrix::operator()(unsigned int i, unsigned int j): "
              << "row index [" << i << "] out of range [0.." << NRow_-1 << "].\n";
  if (NCol_<=j)
    std::cerr << "WARNING: double& SgMatrix::operator()(unsigned int i, unsigned int j): "
              << "column index [" << j << "] out of range [0.." << NCol_-1 << "].\n";
#endif //DEBUG

  return (i<NRow_ && j<NCol_)?*(*(B_+j)+i):dTmp_;
};



//
inline double SgMatrix::getElement(unsigned int i, unsigned int j) const
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: double SgMatrix::getElement(unsigned int i, unsigned int j) const: "
              << "row index [" << i << "] out of range [0.." << NRow_-1 << "].\n";
  if (NCol_<=j)
    std::cerr << "WARNING: double SgMatrix::getElement(unsigned int i, unsigned int j) const: "
              << "column index [" << j << "] out of range [0.." << NCol_-1 << "].\n";
#endif //DEBUG

  return (i<NRow_ && j<NCol_)?*(*(B_+j)+i):0.0;
};



//
inline void SgMatrix::setElement(unsigned int i, unsigned int j, double d) 
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: void SgMatrix::setElement(unsigned int i, unsigned int j, double d): "
              << "row index [" << i << "] out of range [0.." << NRow_-1 << "].\n";
  if (NCol_<=j)
    std::cerr << "WARNING: void SgMatrix::setElement(unsigned int i, unsigned int j, double d): "
              << "column index [" << j << "] out of range [0.." << NCol_-1 << "].\n";
#endif //DEBUG
  
  if (i<NRow_ && j<NCol_) 
    *(*(B_+j)+i) = d;
};




//
// FUNCTIONS:
//
//
// Fills all elements with zeros:
inline void SgMatrix::clear()
{
  double            **w=B_;
  for (unsigned int i=0; i<NCol_; i++, w++)
  memset((void*)(*w), 0, sizeof(double)*NRow_);
};



// fills all elements with a scalar:
inline SgMatrix& SgMatrix::operator=(double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; i<NRow_; j++,w++)
      *ww++ = d;
  return *this;
};



// increment:
inline SgMatrix& SgMatrix::operator+=(const SgMatrix& M)
{
  double            **w=B_, **q=M.B_, *ww, *qq;
  unsigned int      NCol=std::min(NCol_,M.NCol_), NRow=std::min(NRow_,M.NRow_), i, j;
  for (i=0; i<NCol; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<NRow; j++)
      *ww++ += *qq++;
  return *this;
};



// decrement:
inline SgMatrix& SgMatrix::operator-=(const SgMatrix& M)
{
  double            **w=B_, **q=M.B_, *ww, *qq;
  unsigned int      NCol=std::min(NCol_,M.NCol_), NRow=std::min(NRow_,M.NRow_), i, j;
  for (i=0; i<NCol; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<NRow; j++)
      *ww++-=*qq++;
  return *this;
};



// multiply by a scalar:
inline SgMatrix& SgMatrix::operator*=(double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; j<NRow_; j++)
      *ww++ *= d;
  return *this;
};



// divide by a scalar:
inline SgMatrix& SgMatrix::operator/=(double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; j<NRow_; j++)
      *ww++ /= d;
  return *this;
};



// returns transposed matrix:
inline SgMatrix SgMatrix::T() const
{
  SgMatrix        M(NCol_, NRow_, false);
  double            **a=M.B_, *aa;
  unsigned int      i, j;
  for (j=0; j<M.NCol_; j++,a++)
    for (aa=*a,i=0; i<M.NRow_; i++,aa++)
      *aa = *(*(B_+i)+j);
  return M;
};



// is equal?
inline bool SgMatrix::operator==(const SgMatrix& M) const
{
  bool              Is = true;
  if (NRow_ != M.NRow_ || NCol_ != M.NCol_) return !Is;
  // here the dimensions are the same:
  double            **w=B_, **q=M.B_, *ww, *qq;
  unsigned int      i, j;
  for (i=0; Is && i<NCol_; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; Is && j<NRow_; j++)
      Is = Is && *ww++==*qq++;
  return Is;
};



// is not equal?
inline bool SgMatrix::operator!=(const SgMatrix& M) const
{
  return !(*this==M);
};




//
// FRIENDS:
//
//
// minus sign:
inline SgMatrix operator-(const SgMatrix& M)
{
  return SgMatrix(M)*=-1.0;
};



// matrix / scalar
inline SgMatrix operator/(const SgMatrix& M, double d) 
{
  return SgMatrix(M)/=d;
};



// matrix x scalar
inline SgMatrix operator*(const SgMatrix& M, double d) 
{
  return SgMatrix(M)*=d;
};



// scalar x matrix
inline SgMatrix operator*(double d, const SgMatrix& M) 
{
  return SgMatrix(M)*=d;
};



// matrix + matrix
inline SgMatrix operator+(const SgMatrix& M1, const SgMatrix& M2) 
{
  return SgMatrix(M1)+=M2;
};



// matrix - matrix
inline SgMatrix operator-(const SgMatrix& M1, const SgMatrix& M2) 
{
  return SgMatrix(M1)-=M2;
};
/*================================================================================================*/




/*================================================================================================*/
//
// aux functions:
//
// output to std stream:
/**Makes output to std::ostream, a standard text formatted output.
 * \param s -- a stream;
 * \param M -- a matrix.
 */
std::ostream &operator<<(std::ostream& s, const SgMatrix& M);



/*================================================================================================*/
#endif // SG_MATRIX_H
