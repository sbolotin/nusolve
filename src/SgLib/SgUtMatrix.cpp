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

#include <iostream>

#include <QtCore/QString>

#include <SgUtMatrix.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// make a copy:
SgUtMatrix& SgUtMatrix::operator=(const SgUtMatrix& R)
{
#ifdef DEBUG
  if (NRow_!=R.NRow_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator= (const SgUtMatrix&):"
              << " ranges of matrices " << "are different (rows): " << NRow_ << " and " 
              << R.NRow_ << "\n";
  if (NCol_!=R.NCol_)
    std::cerr << "WARNING: SgUtMatrix& SgUtMatrix::operator= (const SgUtMatrix&):"
              << " ranges of matrices " << "are different (columns): " << NCol_ << " and " 
              << R.NCol_ << "\n";
#endif //DEBUG

  double            **w=B_, **q=R.B_;
  unsigned int      N=std::min(NCol_,R.NCol_);
  for (unsigned int i=0; i<N; i++,w++,q++)
    memcpy((void*)(*w), (const void*)(*q), (i+1)*sizeof(double));
  return *this;
};




/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//
// matrix x vector
SgVector operator*(const SgUtMatrix& R, const SgVector& V)
{
#ifdef DEBUG
  if (R.nCol()!=V.n()) 
    std::cerr << "WARNING: SgVector operator*(const SgUtMatrix&, const SgVector&): "
              << "incompatible ranges of matrix (" << R.nCol() 
              << ") and vector (" << V.n() << ")\n";
#endif //DEBUG

  SgVector            X(R.nRow(), false);
  double                *x=X.B_, **r=R.B_, *v=V.B_;
  unsigned int          i, j;
  for (j=0; j<X.N_; j++,r++)
    for (*x=0.0,i=j; i<R.NCol_; i++)
      *x += *(*(r+j)+i)**(v+i);
  return X;
};



// inverse matrix:
SgUtMatrix operator~(const SgUtMatrix& R)
{
  SgUtMatrix      U(R.n(), false);
  double            s;

  if (!R.n()) 
    return U;
  
  if (R.getElement(0,0) != 0.0)
    U(0,0) = 1.0/R.getElement(0,0);
  else 
    std::cerr << "WARNING: SgUtMatrix operator~(const SgUtMatrix&): R[0,0] is zero.\n";

  for (unsigned int j=1; j<R.n(); j++)
    if (R.getElement(j,j) != 0.0)
    {
      U(j,j) = 1.0/R.getElement(j,j);
      for (unsigned int k=0; k<j; k++)
      {
        s = 0.0;
        for (unsigned int i=k; i<j; i++) 
          s += U.getElement(k,i)*R.getElement(i,j);
        U(k,j) = -s*U.getElement(j,j);
      };
    }
  else
    std::cerr << "WARNING: SgUtMatrix operator~(const SgUtMatrix&): R[j,j] is zero, j=" 
              << j << ".\n";
  return U;
};



// matrix x matrix
SgMatrix calcProduct_mat_x_mat(const SgUtMatrix& R1, const SgMatrix& M2)
{
#ifdef DEBUG
  if (R1.nCol()!=M2.nRow()) // complain:
    std::cerr << "WARNING: SgMatrix calcProduct_mat_x_mat(const SgUtMatrix& R1, "
              << "const SgMatrix& M2): matrix size mismatch.\n";
#endif //DEBUG

  unsigned int      N=std::min(R1.nCol(), M2.nRow()), i, j, l;
  unsigned int      NRow=R1.nRow(), NCol=M2.nCol();
  SgMatrix        M(NRow, NCol, false);
/*
  // optimized version:
  double            **m=M.B_, **m1=M1.B_, **m2=M2.B_, *mm, *mm2;

  for (j=0; j<M.NCol_; j++,m++,m2++)
    for (mm=*m,i=0; i<M.NRow_; i++,mm++)
      for (mm2=*m2,*mm=0.0,l=0; l<N; l++,mm2++)
        *mm += *(*(m1+l)+i)**mm2;
*/
  // that should be good for all types of matrices:
  for (j=0; j<NCol; j++)
    for (i=0; i<NRow; i++)
    {
//      long double        d=0.0;
      double        d=0.0;
      for (l=i; l<N; l++)
        d += R1.getElement(i,l)*M2.getElement(l,j);
//      M.setElement(i,j, ((double)d));
      M.setElement(i,j, d);
    };
  return M;
};



/*=====================================================================================================*/
//
// aux functions:
//
// R*x = z; 
// x-?
SgVector& solveEquation(const SgUtMatrix& R, SgVector& x, const SgVector& z)
{
#ifdef DEBUG
  if (x.n()!=z.n())
    std::cerr << "WARNING: SgVector& solveEquation(const SgUtMatrix&, SgVector&,"
      "const SgVector&): incompatible ranges of vectors.\n";
  if (R.nRow()!=x.n())
    std::cerr << "WARNING: SgVector& solveEquation(const SgUtMatrix&, SgVector&,"
      "const SgVector&): incompatible ranges of vector and matrix.\n";
#endif //DEBUG
  unsigned int      i, j, n=x.n()-1;
  double            s;

  if (!x.n()) 
    return x;

  if (R.getElement(n,n)!=0.0)
    x(n)=z.getElement(n)/R.getElement(n,n);
  else
    std::cerr << "WARNING: SgVector& solveEquation(const SgUtMatrix& R, SgVector& x,"
              << " const SgVector& z): division by zero caused by R[0,0]==0.\n";

  for (i=n; i>0; i--) // i ==> (i+1)
    {
      for (j=i, s=0.0; j<x.n(); j++)
        s += R.getElement(i-1,j)*x.getElement(j);
      if (R.getElement(i-1,i-1)!=0.0)
        x(i-1) = (z.getElement(i-1) - s)/R.getElement(i-1,i-1);
      else
        std::cerr << "WARNING: SgVector& solveEquation(const SgUtMatrix& R, SgVector& x,"
                  << " const SgVector& z): division by zero caused by R[j,j]==0, j=" 
                  << j << ".\n";
    };
  return x;
};



// i/o:
std::ostream &operator<<(std::ostream& s, const SgUtMatrix& R)
{
  QString             str;
  unsigned int        i, j;
  if (R.nCol()<30) // trying to write in conventional form:
  {
    for (i=0; i<R.nRow(); i++)
    {
      s << "|";
      for (j=0; j<i; j++)
      {
        str.sprintf(" %.6g", 0.0);
        s << qPrintable(str);
      };
      for (j=i; j<R.nCol(); j++)
      {
        str.sprintf(" %.6g", R.getElement(i,j));
        s << qPrintable(str);
      };
      s << " |\n";
    };
  }
  else // will use a similar to SINEX format presentation:
  {
    for (j=0; j<R.nCol(); j++)
      for (i=0; i<=j; i++)
        s << i << "  " << j << "  " << R.getElement(i,j) << "\n";
  };
  return s;
};
/*=====================================================================================================*/
