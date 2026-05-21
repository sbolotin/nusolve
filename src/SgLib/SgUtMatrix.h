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

#ifndef SG_UT_MATRIX_H
#define SG_UT_MATRIX_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgMathSupport.h>
#include <SgVector.h>
#include <SgMatrix.h>


/***===================================================================================================*/
/**
 * An upper triangular matrix. SRIF uses it.
 *
 */
/**====================================================================================================*/
class SgUtMatrix: public SgMatrix
{
 public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes or later assignments).
   */
  inline SgUtMatrix();
    
 /**A constructor.
  * Creates a copy of an object.
  * \param N -- number of elements in a row or column.
  * \param IsNeedClear -- if set to TRUE fills the array with zeros, if set to "false" skips this 
  *        procedure and the state of a matrix depends on compiler realization.
  */
  inline SgUtMatrix(unsigned int N, bool IsNeedClear=true);
    
  /**A constructor.
   * Creates a copy of an object.
   * \param M -- upper-triangular matrix that will be copied into the new object.
   */
  inline SgUtMatrix(const SgUtMatrix& R);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgUtMatrix();



  //
  // Interfaces:
  //
  /**Returns number of rows/columns in the matrix.
   */
  inline unsigned int n() const;

  /**Returns a reference at (i,j)-th element of the matrix, so
   * R(i,j)=x is working.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [i,N-1].
   */
  inline double& operator()(unsigned int i, unsigned int j);

  /**Returns a value of (i,j)-th element. The matrix does not change.
   * \param i -- an index; valid ranges are: [0,N-1].
   * \param j -- an index; valid ranges are: [0,N-1].
   */
  inline double getElement(unsigned int i, unsigned int j) const;

  /**Changes the (i,j)-th element.
   * \param i -- an index; valid ranges are: [0,N-1];
   * \param j -- an index; valid ranges are: [i,N-1];
   * \param d -- a new value for (i,j)-th element. 
   */
  inline void setElement(unsigned int i, unsigned int j, double d);



  //
  // Functions:
  //
  /**Fills all elements with zero.
  */
  inline void clear();

  /**Returns transposed matrix (the original matrix doesn't change).
   */
  inline SgMatrix T() const;

  /**Assign a matrix to the another one.
   * \param R -- a matrix to copy.
   */
  SgUtMatrix& operator=(const SgUtMatrix& R);

  /**Fills the UT matrix with a double.
   * \param d -- a value to fill the matrix.
   */
  inline SgUtMatrix& operator=(double d);

  /**Increments the UT matrix by the another one.
   * \param R -- a matrix to add.
   */
  inline SgUtMatrix& operator+=(const SgUtMatrix& R);

  /**Decrements the UT matrix by the another one.
   * \param R -- a matrix to substract.
   */
  inline SgUtMatrix& operator-=(const SgUtMatrix& R);

  /**Multiplies the UT matrix by a scalar.
   * \param d -- a scale.
   */
  inline SgUtMatrix& operator*=(double d);

  /**Divides the UT matrix by a scalar.
   * \param d -- a divider.
   */
  inline SgUtMatrix& operator/=(double d);



  //
  // Friends:
  //
  /**Returns a matrix with an inverted sign.
   * \param R -- an original matrix.
   */
  friend inline SgUtMatrix operator-(const SgUtMatrix& R);

  /**Returns a matrix divided by a scalar.
   * \param R -- a matrix;
   * \param d -- a divider;
   */
  friend inline SgUtMatrix operator/(const SgUtMatrix& R, double d);
  
  /**Returns a matrix multiplied by a scalar.
   * \param R -- a matrix;
   * \param d -- a scale.
   */
  friend inline SgUtMatrix operator*(const SgUtMatrix& R, double d);

  /**Returns a natrix multiplied by a scalar.
   * \param d -- a scale.
   * \param R -- a matrix;
   */
  friend inline SgUtMatrix operator*(double d, const SgUtMatrix& R);

  /**Returns a sum of two UT matrices.
   * \param R1 is a first term,
   * \param R2 is a second term in the sum.
   */
  friend inline SgUtMatrix operator+(const SgUtMatrix& R1, const SgUtMatrix& R2);

  /**Returns a difference of two matrices.
   * \param R1 is a first term,
   * \param R2 is a second term in the sum.
   */
  friend inline SgUtMatrix operator-(const SgUtMatrix& R1, const SgUtMatrix& R2); 
  
  /**Makes a product of a matrix and a vector.
   * \param R -- a matrix;
   * \param V -- a vector.
   */
  friend SgVector operator*(const SgUtMatrix& R, const SgVector& V);

  friend SgMatrix calcProduct_mat_x_mat(const SgUtMatrix& R1, const SgMatrix& M2);
  

  /**Returns inversed matrix.
   * This is time consumed operation, shouldn't use in ordinary operations.
   * R*~R == ~R*R == E (original matrix doesn't change)..
   */
  friend SgUtMatrix operator~(const SgUtMatrix& R);


  inline double** &base() {return B_;};


  inline const double* const* base_c() const {return B_;};
  //
  // I/O:
  //
  // ...
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgUtMatrix's inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgUtMatrix::SgUtMatrix()
  : SgMatrix()
{
  // nothing to do
};



// A regular constructor:
inline SgUtMatrix::SgUtMatrix(unsigned int N, bool IsNeedClear)
  : SgMatrix()
{
  NRow_ = NCol_ = N;
  dTmp_ = 0.0;
  B_    = new double*[NCol_];
  double **w = B_;
  for (unsigned int i=0; i<NCol_; i++,w++)
    {
      *w = new double[i+1];
      if (IsNeedClear)
        memset((void*)(*w), 0, sizeof(double)*(i+1));
    };
};



// A copying constructor:
inline SgUtMatrix::SgUtMatrix(const SgUtMatrix& R)
  : SgMatrix()
{
  NRow_ = R.NRow_;
  dTmp_ = 0.0;
  B_    = new double*[NCol_=R.NCol_];
  double **w=B_, **q=R.B_;
  for (unsigned int i=0; i<NCol_; i++,w++,q++)
    {
      *w=new double[i+1];
      memcpy((void*)(*w), (const void*)(*q), (i+1)*sizeof(double));
    };
};



// A destructor:
inline SgUtMatrix::~SgUtMatrix()
{
  if (B_)
    {
      double **w=B_;
      for (unsigned int i=0; i<NCol_; i++,w++) 
        delete[] *w;
      delete[] B_;
      B_ = NULL;
    };
  NRow_ = NCol_ = 0;
  dTmp_ = 0.0;
};




//
// INTERFACES:
//
// number of elements in a row or column:
inline unsigned int SgUtMatrix::n() const 
{
  return NRow_;
};



// returns a reference on (i,j)-th element:
inline double& SgUtMatrix::operator()(unsigned int i, unsigned int j)
{
#ifdef DEBUG
  if (i>j)
    std::cerr << "WARNING: double& SgUtMatrix::operator()(unsigned int, unsigned int):"
              << " row's index [" << i << "] greater than the column index [" << j << "]\n";
  if (NRow_<=i)
    std::cerr << "WARNING: double& SgUtMatrix::operator()(unsigned int, unsigned int):"
              << " row's index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: double& SgUtMatrix::operator()(unsigned int, unsigned int):"
              << " column's index [" << j << "] out of range [0.." << NCol_-1 << "]\n";
#endif //DEBUG

  return (i<=j && j<NCol_)?*(*(B_+j)+i):dTmp_;
};



//
inline double SgUtMatrix::getElement(unsigned int i, unsigned int j) const
{
#ifdef DEBUG
  if (NRow_<=i)
    std::cerr << "WARNING: SgUtMatrix::getElement(i,j):"
              << " row index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: SgUtMatrix::getElement(i,j):"
              << " column index [" << j << "] out of range [0.." << NCol_-1 << "]\n";
  if (i>j)
    std::cerr << "WARNING: SgUtMatrix::getElement(i,j):"
              << " row's index [" << i << "] greater than the column index [" << j << "]\n";
#endif //DEBUG
  
  return (i<=j && j<NCol_)?*(*(B_+j)+i):0.0;
};



//
inline void SgUtMatrix::setElement(unsigned int i, unsigned int j, double d)
{
#ifdef DEBUG
  if (i>j)
    std::cerr << "WARNING: void SgUtMatrix::set(unsigned int, unsigned int, double):"
              << " row's index [" << i << "] greater than the column index [" << j << "]\n";
  if (NRow_<=i)
    std::cerr << "WARNING: void SgUtMatrix::set(unsigned int, unsigned int, double):"
              << " row's index [" << i << "] out of range [0.." << NRow_-1 << "]\n";
  if (NCol_<=j)
    std::cerr << "WARNING: void SgUtMatrix::set(unsigned int, unsigned int, double):"
              << " column's index [" << j << "] out of range [0.." << NCol_-1 << "]\n";
#endif //DEBUG
  
  if (i<=j && j<NCol_)
    *(*(B_+j)+i) = d;
};




//
// FUNCTIONS:
//
//
// Fills all elements with zeros:
inline void SgUtMatrix::clear()
{
  double            **w=B_;
  for (unsigned int i=0; i<NCol_; i++, w++)
    memset((void*)(*w), 0, sizeof(double)*(i+1));
};



// returns transposed matrix:
inline SgMatrix SgUtMatrix::T() const
{
  SgMatrix        M(NCol_,NRow_, false);
  unsigned int      i, j;
  for (j=0; j<M.nCol(); j++)
    for (i=0; i<M.nRow(); i++)
      M.setElement(j,i, i<=j?*(*(B_+i)+j):0.0);
  return M;
};



// fill matrix with a constant value:
inline SgUtMatrix& SgUtMatrix::operator=(double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; j<=i; j++,w++)
      *ww++ = d;
  return *this;
};



// increment:
inline SgUtMatrix& SgUtMatrix::operator+=(const SgUtMatrix& R)
{
#ifdef DEBUG
  if (NRow_!=R.NRow_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator+= (const SgUtMatrix&):"
              << " ranges of matrices are different (rows): " << NRow_ << " and " 
              << R.NRow_ << "\n";
  if (NCol_!=R.NCol_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator+= (const SgUtMatrix&):"
              << " ranges of matrices are different (columns): " << NCol_ << " and " 
              << R.NCol_ << "\n";
#endif //DEBUG

  double            **w=B_, **q=R.B_, *ww, *qq;
  unsigned int      N=std::min(NCol_,R.NCol_), i, j;
  for (i=0; i<N; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<=i; j++)
      *ww++ += *qq++;
  return *this;
};



// decrement:
inline SgUtMatrix& SgUtMatrix::operator-= (const SgUtMatrix& R)
{
#ifdef DEBUG
  if(NRow_!=R.NRow_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator-= (const SgUtMatrix&):"
              << " ranges of matrices are different (rows): " << NRow_ << " and " << R.NRow_ 
              << "\n";
  if(NCol_!=R.NCol_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator-= (const SgUtMatrix&):"
              << " ranges of matrices are different (columns): " << NCol_ << " and " << R.NCol_
              << "\n";
#endif //DEBUG

  double            **w=B_, **q=R.B_, *ww, *qq;
  unsigned int      N=std::min(NCol_,R.NCol_), i, j;
  for(i=0; i<N; i++,w++,q++)
    for(ww=*w,qq=*q,j=0; j<=i; j++)
      *ww++ -= *qq++;
  return *this;
};



// multiply by a scalar:
inline SgUtMatrix& SgUtMatrix::operator*= (double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; j<=i; j++)
      *ww++ *= d;
  return *this;
};



// divide by a scalar:
inline SgUtMatrix& SgUtMatrix::operator/= (double d)
{
  double            **w=B_, *ww;
  unsigned int      i, j;
  for (i=0; i<NCol_; i++,w++)
    for (ww=*w,j=0; j<=i; j++)
      *ww++ /= d;
  return *this;
};




//
// FRIENDS:
//
//
//
inline SgUtMatrix operator-(const SgUtMatrix& R)
{
  return SgUtMatrix(R)*=-1.0;
};



//
inline SgUtMatrix operator/(const SgUtMatrix& R, double d)
{
  return SgUtMatrix(R)/=d;
};



//  
inline SgUtMatrix operator*(const SgUtMatrix& R, double d) 
{
  return SgUtMatrix(R)*=d;
};



//
inline SgUtMatrix operator*(double d, const SgUtMatrix& R) 
{
  return SgUtMatrix(R)*=d;
};



//
inline SgUtMatrix operator+(const SgUtMatrix& R1, const SgUtMatrix& R2) 
{
  return SgUtMatrix(R1)+=R2;
};



//
inline SgUtMatrix operator-(const SgUtMatrix& R1, const SgUtMatrix& R2)
{
  return SgUtMatrix(R1)-=R2;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//
// output to std stream:
/**Makes output to std::ostream, a standard text formatted output.
 * \param s -- a stream;
 * \param R -- a matrix.
 */
std::ostream &operator<<(std::ostream& s, const SgUtMatrix& R);



/**Returns a vector x that is satisfies an equation system Rx=z.
 * \param R -- a matrix, input;
 * \param x -- a vector, output.
 * \param z -- a vector, input;
 */
SgVector& solveEquation(const SgUtMatrix& R, SgVector& x, const SgVector& z);



/*=====================================================================================================*/
#endif // SG_UT_MATRIX_H
