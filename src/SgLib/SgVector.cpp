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


#include <SgVector.h>


const SgVector vZero(1);


/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
std::ostream &operator<<(std::ostream& s, const SgVector& v)
{
// there is no need to be a friend to vector
  unsigned int                  n=v.n();
  s << "(";
  if (n>0)
  {
    for(unsigned int i=0; i<n-1; i++)
      s << v.getElement(i) << ", ";
    s << v.getElement(n-1);
  };
  return s << ")";
};
/*=====================================================================================================*/
