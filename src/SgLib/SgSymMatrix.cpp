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

#include <SgSymMatrix.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/




/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//
// matrix x vector
SgVector operator*(const SgSymMatrix& P, const SgVector& V)
{
#ifdef DEBUG
  if (P.nCol()!=V.n()) 
    std::cerr << "WARNING: SgVector operator*(const SgSymMatrix&, const SgVector&): "
              << "incompatible ranges of matrix (" << P.nCol() << ") and vector (" 
              << V.n() << ")\n";
#endif //DEBUG

  unsigned int          N=std::min(P.nRow(), V.n()), i, l;
  SgVector            X(N, false);
  double                d;
  for (i=0; i<N; i++)
  {
    d = 0.0;
    for (l=0; l<N; l++)
      d += P.getElement(i,l)*V.getElement(l);
    X.setElement(i, d);
  };
  return X;
};



// matrix x matrix
SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, const SgSymMatrix& P2)
{
#ifdef DEBUG
  if (M1.nCol()!=P2.nRow()) // complain:
    std::cerr << "WARNING: SgMatrix calcProduct_mat_x_mat(const SgMatrix& M1, "
              << "const SgSymMatrix& P2): matrix size mismatch.\n";
#endif //DEBUG

  unsigned int      N=std::min(M1.nCol(), P2.nRow()), i, j, l;
  unsigned int      NRow=M1.nRow(), NCol=P2.nCol();
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
        d += M1.getElement(i,l)*P2.getElement(l,j);
//      M.setElement(i,j, ((double)d));
      M.setElement(i,j, d);
    };
  return M;
};





/*=====================================================================================================*/
//
// aux functions:
//
// i/o:
std::ostream &operator<<(std::ostream& s, const SgSymMatrix& P)
{
 unsigned int                   i, j;
 QString                        str("");
 if (P.nCol()<55) // trying to write in conventional form:
 {
   for (i=0; i<P.nRow(); i++)
   {
     s << "|";
     for (j=0; j<P.nCol(); j++)
        s << "  " << qPrintable(str.sprintf("%12.5e", P.getElement(i,j))) << "  ";
     s << "|\n";
   };
 }
 else // will use a similar to SINEX format presentation:
 {
   for (j=0; j<P.nCol(); j++)
     for (i=0; i<=j; i++)
       s << i << "  " << j << "   " << P.getElement(i,j) << "\n";
 };
 return s;
};
/*=====================================================================================================*/
