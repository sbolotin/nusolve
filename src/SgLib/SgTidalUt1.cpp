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

#include <math.h>


#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <SgTidalUt1.h>
#include <SgLogger.h>
#include <SgMathSupport.h>







void fundArgs_stub   (const SgMJD&, double[5]);


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTidalUt1::className()
{
  return "SgTidalUt1";
};



// An empty constructor:
SgTidalUt1::SgTidalUt1(UT1TideContentType tc, double v) :
  numOfUt1TableRecs4ShortPeriods_(41),
  numZero_(0)
{
  tideContent_ = tc;
  calcVersionValue_ = v;
  
  if (tideContent_ == CT_SHORT_TERMS_REMOVED)           // UT1R
  {
    fundArgs_  = &calcNutationFundArgs_IersConv2003;
    ut1Ttable_ = ut1Ttable_IersConv2003;
    // periods < 35days:
    numOfUt1TableRecs_ = &numOfUt1TableRecs4ShortPeriods_;
  }
  else if (tideContent_ == CT_ALL_TERMS_REMOVED)        // UT1S
  {
    if (calcVersionValue_ <= 8.200001)
    {
      fundArgs_  = &calcNutationFundArgs_IersStds1992;
      ut1Ttable_ = ut1Ttable_IersStds1992;
      numOfUt1TableRecs_ = &numOfUt1TableRecs_IersStds1992;
    }
    else if (9.0 <= calcVersionValue_ && calcVersionValue_ < 9.9)
    {
      fundArgs_  = &calcNutationFundArgs_IersConv1996;
      ut1Ttable_ = ut1Ttable_IersConv1996;
      numOfUt1TableRecs_ = &numOfUt1TableRecs_IersConv1996;
    }
    else if (9.99 < calcVersionValue_ && calcVersionValue_ < 99.99)
    {
      fundArgs_  = &calcNutationFundArgs_IersConv2003;
      ut1Ttable_ = ut1Ttable_IersConv2003;
      numOfUt1TableRecs_ = &numOfUt1TableRecs_IersConv2003;
    } 
    else
    {
      fundArgs_           = &calcNutationFundArgs_IersConv2003;
      ut1Ttable_          = ut1Ttable_IersConv2010;
      numOfUt1TableRecs_  = &numOfUt1TableRecs_IersConv2010;
      logger->write(SgLogger::ERR, SgLogger::TIME, className() +
        ": the CALC version number, " + QString("").setNum(calcVersionValue_) +
        ", is undocumented. Removing of zonal tides from UT1 is impossible");
    };
  }
  else
  {
    fundArgs_         = &fundArgs_stub;
    ut1Ttable_        = ut1Ttable_IersConv2003;
    numOfUt1TableRecs_= &numZero_;
  };
};



// A destructor:
SgTidalUt1::~SgTidalUt1()
{
};



//
double SgTidalUt1::calc(const SgMJD& t)
{
  double                        dUt(0.0);
  double                        fArgs[5];
  
  if (tideContent_ != CT_FULL)
  {
    // evaluate the fundamental arguments:
    (*fundArgs_)(t, fArgs);
    // calculate the zonal tidal correction:
    calcTidalUt1(fArgs, dUt);
  };
  return dUt;
};



//
void SgTidalUt1::calcTidalUt1(const double args[5], double& dUt1)
{
  double                        arg, sinArg, cosArg;
  dUt1 = 0.0;
  //
  for (int i=0; i<*numOfUt1TableRecs_; i++)
  {
    arg = 0.0;
    for (int j=0; j<5; j++)
      arg += args[j]*ut1Ttable_[i].n_[j];
    arg = fmod(arg, M_PI*2.0);
    sincos(arg,   &sinArg, &cosArg);
    dUt1 += ut1Ttable_[i].ut1_sin_*sinArg + ut1Ttable_[i].ut1_cos_*cosArg;
  };
  dUt1 *= 1.0E-4; // convert to seconds
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
// 
//
void fundArgs_stub(const SgMJD&, double args[5])
{
  args[0] = args[1] = args[2] = args[3] = args[4] = 0.0;
};



// i/o:


/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/








/*=====================================================================================================*/
