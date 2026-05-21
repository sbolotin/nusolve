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


#include <SgCubicSpline.h>
#include <SgLogger.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgCubicSpline::className()
{
  return "SgCubicSpline";
};



//
void SgCubicSpline::prepare4Spline()
{
  isOk_ = false;
  if (dimension_>0 && numOfRecords_>3)
  {
    // check the order of arg:
    double                      f=argument_.getElement(0);
    for (int i=1; i<numOfRecords_; i++)
    {
      if (f >= argument_.getElement(i))
      {
        logger->write(SgLogger::ERR, SgLogger::INTERP, className() + 
          ": the argument [" + QString().setNum(i) + "] has a wrong order, the table is skipped");
        for (int k=0; k<numOfRecords_; k++)
          logger->write(SgLogger::ERR, SgLogger::INTERP, className() + 
            QString().sprintf(": [%04d] = %.6f", k, argument_.getElement(k)));
        return;
      };
      f = argument_.getElement(i);
    };
    for (int i=0; i<dimension_; i++)
      solveSpline(i);
    isOk_ = true;
  };
};



//
void SgCubicSpline::solveSpline(int nCol)
{
  SgVector      aleph(numOfRecords_), beth(numOfRecords_);
  double        f;

  f = h(1) + h(2);
  aleph(2) = -0.5*h(2)/f;
  beth (2) = 3.0*(table_.getElement(0, nCol)/h(1) - 
                  table_.getElement(1, nCol)*(1.0/h(1) + 1.0/h(2)) + 
                  table_.getElement(2, nCol)/h(2)
                  )/f;
  
  for (int i=2; i<numOfRecords_-2; i++)
  {
    f = aleph(i)*h(i) + 2.0*(h(i) + h(i+1));
    aleph(i+1) = -h(i+1)/f;
    beth (i+1) = (6.0*( table_.getElement(i-1, nCol)/h(i) - 
                        table_.getElement(i,   nCol)*(1.0/h(i) + 1.0/h(i+1)) + 
                        table_.getElement(i+1, nCol)/h(i+1)
                      ) - beth(i)*h(i))/f;
  };

  coeffs_(numOfRecords_-1, nCol) = 0.0;
  coeffs_(numOfRecords_-2, nCol) = 
    (6.0*(table_.getElement(numOfRecords_-3, nCol)/h(numOfRecords_-2) - 
          table_.getElement(numOfRecords_-2, nCol)*(1.0/h(numOfRecords_-2) + 1.0/h(numOfRecords_-1)) + 
          table_.getElement(numOfRecords_-1, nCol)/h(numOfRecords_-1)
         ) - h(numOfRecords_-2)*beth(numOfRecords_-2)
    )/(h(numOfRecords_-2)*aleph(numOfRecords_-2) + 2.0*(h(numOfRecords_-2) + h(numOfRecords_-1)));

  for (int i=numOfRecords_-3; i>0; i--)
    coeffs_(i, nCol) = aleph.getElement(i+1)*coeffs_.getElement(i+1, nCol) + beth.getElement(i+1);

  coeffs_(0, nCol) = 0.0;
};



//
double SgCubicSpline::spline(double arg, int nColumn, double &r)
{
  double                        d=0.0;
  r = 0.0;
  if (!isOk_)
    return d;
    
  // get the idx:
  int                           nIdx=0;
  if (arg <= argument_.getElement(1))
    nIdx = 1;
  else if (argument_.getElement(numOfRecords_-1) <= arg)
    nIdx = numOfRecords_ - 1;
  else
  {
    int                         i=2;
    while (i<numOfRecords_ && argument_.getElement(i-1) <= arg) 
      i++;
    nIdx = i - 1;
  };
  
  double                        f1, f2, h2;
  
  f1 = argument_.getElement(nIdx) - arg; 
  f2 = arg - argument_.getElement(nIdx-1);
  h2 = h(nIdx)*h(nIdx);
  
  d = ( coeffs_.getElement(nIdx-1, nColumn)*f1*f1*f1/6.0 + 
        coeffs_.getElement(nIdx,   nColumn)*f2*f2*f2/6.0 +
        (table_.getElement(nIdx-1, nColumn) - coeffs_.getElement(nIdx-1, nColumn)*h2/6.0)*f1 +
        (table_.getElement(nIdx,   nColumn) - coeffs_.getElement(nIdx,   nColumn)*h2/6.0)*f2)/h(nIdx);
        
  r = (-coeffs_.getElement(nIdx-1, nColumn)*f1*f1/2.0 + 
        coeffs_.getElement(nIdx,   nColumn)*f2*f2/2.0 +
        table_.getElement(nIdx,   nColumn) - table_.getElement(nIdx-1, nColumn) +
        (coeffs_.getElement(nIdx-1, nColumn) - coeffs_.getElement(nIdx, nColumn))*h2/6.0 )/h(nIdx);
  
  return d;
};
/*=====================================================================================================*/




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
