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


#include <SgPartial.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPartial::className()
{
  return "SgPartial";
};



//
SgPartial& SgPartial::operator=(const SgPartial& p)
{
  SgAttribute::operator=(p);
  setName(p.getName());
  setDataDomain(p.getDataDomain());
  setD(p.getD());
  setNumObs (p.getNumObs());
  setNumSes (p.getNumSes());
  setTStart (p.getTStart());
  setTFinis (p.getTFinis());
  twSum_ = p.twSum_;
  wSum_ = p.wSum_;
  return *this;
};



// sets partial and update statistcis:
void SgPartial::updateStatistics(const SgMJD& t, double weight)
{
  if (isAttr(Attr_IS_PARTIAL_SET) && tZero<t && t<tInf)
  {
    if (numObs_ == 0)
      tStart_ = t;
    numObs_++;
    tFinis_ = t;
    twSum_ += t.toDouble()*weight;
    wSum_ += weight;
  };
};
/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//



// i/o:



/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
