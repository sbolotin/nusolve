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



#include <QtCore/QString>


#include <SgMatrix.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// copy one matrix into another one:
SgMatrix& SgMatrix::operator=(const SgMatrix& M)
{
  unsigned int      i;
  double            **w, **q;
  if (NCol_ != M.NCol_ || NRow_ != M.NRow_)  // adjust dimensions:
  {
    // delete previously allocated array:
    if (B_)
    {
      for (i=0,w=B_; i<NCol_; i++,w++) 
        delete[] *w;
      delete[] B_;
    };
    // create a new one:
    NRow_ = M.NRow_;
    B_    = new double*[NCol_=M.NCol_];
    for (i=0,w=B_; i<NCol_; i++,w++)
      *w=new double[NRow_];
  };
  //make a copy:
  for (w=B_,q=M.B_,i=0; i<NCol_; i++,w++,q++)
    memcpy((void*)(*w), (const void*)(*q), NRow_*sizeof(double));
  return *this;
};



/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//
// matrix x vector
SgVector operator*(const SgMatrix& M, const SgVector& V)
{
  SgVector                      R(M.NRow_, false);
  double                       *v=V.B_, *r=R.B_;
  unsigned int                  N=std::min(M.NCol_, V.N_), i, l;

#ifdef DEBUG
  if (M.NCol_!=V.N_)
    std::cout << "WARNING: SgVector operator*(const SgMatrix&, const SgVector&):"
              << " incompatible ranges of the matrix and the vector\n";
#endif //DEBUG

  for (i=0; i<R.N_; i++,r++)
    for (*r=0.0,v=V.B_,l=0; l<N; l++,v++) 
      *r += *(*(M.B_+l)+i)**v;
  return R;
};



// matrix x matrix
SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, const SgMatrix& M2)
{
#ifdef DEBUG
  if (M1.nCol()!=M2.nRow()) // complain:
    std::cerr << "WARNING: SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, "
              << "const SgMatrix& M2): matrix size mismatch.\n";
#endif //DEBUG

  unsigned int      N=std::min(M1.nCol(), M2.nRow()), i, j, l;
  unsigned int      NRow=M1.nRow(), NCol=M2.nCol();
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
      for (l=0; l<N; l++)
        d += M1.getElement(i,l)*M2.getElement(l,j);
//      M.setElement(i,j, ((double)d));
      M.setElement(i,j, d);
    };
  return M;
};



// (matrix)^T x matrix
SgMatrix calcProduct_matT_x_mat(const SgMatrix& M1, const SgMatrix& M2)
{
#ifdef DEBUG
  if (M1.nRow()!=M2.nRow()) 
    std::cerr << "WARNING: SgMatrix matT_x_mat(const SgMatrix&, const SgMatrix&):"
              << " matrix size mismatch\n";
#endif //DEBUG

  unsigned int      N=std::min(M1.nRow(), M2.nRow()), i, j, l;
  unsigned int      NRow=M1.nCol(), NCol=M2.nCol();
  SgMatrix        M(NRow, NCol, false);
/*
  // optimized version:
  double            **m=M.B_, **m1=M1.B_, **m2=M2.B_, *mm, *mm1, *mm2;
  for (j=0; j<M.NCol_; j++,m++,m2++)
    for (mm=*m,m1=M1.B_,i=0; i<M.NRow_; i++,mm++,m1++)
      for (mm2=*m2,mm1=*m1,*mm=0.0,l=0; l<N; l++,mm1++,mm2++)
        *mm += *mm1**mm2;
*/
  // that should be good for all types of matrices:
  for (j=0; j<NCol; j++)
    for (i=0; i<NRow; i++)
    {
      double        d=0.0;
      for (l=0; l<N; l++)
        d += M1.getElement(l,i)*M2.getElement(l,j);
      M.setElement(i,j, d);
    };
  return M;
};



// matrix x (matrix)^T
SgMatrix calcProduct_mat_x_matT(const SgMatrix& M1, const SgMatrix& M2)
{
#ifdef DEBUG
  if (M1.nCol()!=M2.nCol()) 
    std::cerr << "WARNING: SgMatrix mat_x_matT(const SgMatrix&, const SgMatrix&):"
              << " matrix size mismatch\n";
#endif //DEBUG

  unsigned int      N=std::min(M1.nCol(), M2.nCol()), i, j, l;
  unsigned int      NRow=M1.nRow(), NCol=M2.nRow();
  SgMatrix        M(NRow, NCol, false);

  // that should be good for all types of matrices:
  for (j=0; j<NCol; j++)
    for (i=0; i<NRow; i++)
    {
      double        d=0.0;
      for (l=0; l<N; l++)
        d += M1.getElement(i,l)*M2.getElement(j,l);
      M.setElement(i,j, d);
    };
  return M;
};



// (matrix)^T x vector
SgVector calcProduct_matT_x_vec(const SgMatrix& M, const SgVector& V)
{
#ifdef DEBUG
  if (M.nRow()!=V.n())
    std::cout << "WARNING: SgVector matT_x_vec(const SgMatrix&, const SgVector&):"
              << " incompatible ranges of the matrix and the vector\n";
#endif //DEBUG

  unsigned int      N=std::min(M.nRow(), V.n()), i, l;
  unsigned int      NRow=M.nCol();
  SgVector        X(NRow, false);
/*
  // optimized version:
  double            **m=M.B_, *r=R.B_, *v, *mm;
  for (i=0; i<R.N_; i++,r++,m++)
    for (*r=0.0,v=V.B_,mm=*m,l=0; l<N; l++,v++,mm++)
      *r += *mm**v;
*/
  // that should be good for all types of matrices:
  for (i=0; i<NRow; i++)
  {
    double          d=0.0;
    for (l=0; l<N; l++)
      d += M.getElement(l,i)*V.getElement(l);
    X.setElement(i, d);
  }
  return X;
};



/*=====================================================================================================*/
//
// aux functions:
//
// i/o:
std::ostream &operator<<(std::ostream& s, const SgMatrix& M)
{
  unsigned int                  i, j, NRow=M.nRow(), NCol=M.nCol();
  QString                       str("");
  if (NCol<55) // trying to write in conventional form:
  {
    for (i=0; i<NRow; i++)
    {
      s << "|";
      for (j=0; j<NCol; j++)
        s << "  " << qPrintable(str.sprintf("%12.5e", M.getElement(i,j))) << "  ";
      s << "|\n";
    };
  }
  else // will use a similar to SINEX format presentation:
  {
    for (j=0; j<NCol; j++)
      for (i=0; i<NRow; i++)
        s << i << "  " << j << "   " << M.getElement(i,j) << "\n";
  };
  return s;
};
/*=====================================================================================================*/
