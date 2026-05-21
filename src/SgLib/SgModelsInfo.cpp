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
#include <stdlib.h>


#include <SgModelsInfo.h>


#include <SgLogger.h>
#include <SgMatrix.h>
#include <SgVector.h>
#include <SgVlbiStationInfo.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgModelsInfo::className()
{
  return "SgModelsInfo";
};


// CONSTRUCTORS:
//
// A constructor:
SgModelsInfo::SgModelsInfo() :
  programName_("n/a"),
  version_("n/a"),
  site_("Site"),
  star_("Star"),
  ephemeris_("Ephemeris"),
  coordinateTime_("CoordTime"),
  atomicTime_("AtomicTime"),
  relativity_("Relativity"),
  polarMotion_("PolarMotion"),
  polarMotionInterpolation_("PolarMotionInterpolation"),
  ut1_("UT1"),
  ut1Interpolation_("UT1Interpolation"),
  nutation_("Nutation"),
  earthTide_("EarthTide"),
  poleTide_("PoleTide"),
  oceanLoading_("OceanLoading"),
  troposphere_("Troposphere"),
  axisOffset_("AxisOffset"),
  parallax_("Parallax"),
  feedHorn_("FeedHorn"),
  controlFlagNames_(),
  controlFlagValues_(),
  oLoadStationStatus_(),
  siteZenDelays_(),
  stations_()
{
  dVersion_ = 0.0;
  // from dbedit/mathbd.f:
  relativityData_ = 1.0;
  precessionData_ = 5029.09660;
  earthTideData_[0] = 0.0;
  earthTideData_[1] = 0.6090;
  earthTideData_[2] = 0.0852;

//  ut1InterpData_ = new SgMatrix(1, 2);
//  wobInterpData_ = new SgMatrix(1, 2);
  ut1InterpData_ = NULL;
  wobInterpData_ = NULL;
};



// A destructor:
SgModelsInfo::~SgModelsInfo()
{
  if (ut1InterpData_)
  {
    delete ut1InterpData_;
    ut1InterpData_ = NULL;
  };
  if (wobInterpData_)
  {
    delete wobInterpData_;
    wobInterpData_ = NULL;
  };
  stations_.clear();
};



//
void SgModelsInfo::prepareUt1InterpData(int nRow)
{
  int                           l;
  if (!ut1InterpData_)
  {
    ut1InterpData_ = new SgMatrix(nRow, 2);
    logger->write(SgLogger::DBG, SgLogger::INTERP, className() +
      "::prepareUt1InterpData(): the UT1 interp.data matrix(" + 
      QString("").setNum(ut1InterpData_->nCol()) + ", " + 
      QString("").setNum(ut1InterpData_->nRow()) + ") has been allocated");
  }
  else if (nRow != (l=ut1InterpData_->nRow()))
  {
    delete ut1InterpData_;
    ut1InterpData_ = new SgMatrix(nRow, 2);
    logger->write(SgLogger::INF, SgLogger::INTERP, className() +
      "::prepareUt1InterpData(): adjusted the dimension of the UT1 interp.data matrix rows: " + 
      QString("").setNum(l) +  " => " + QString("").setNum(nRow));
  };
};


void SgModelsInfo::prepareWobInterpData(int nRow)
{
  int                           l;
  if (!wobInterpData_)
  {
    wobInterpData_ = new SgMatrix(nRow, 2);
    logger->write(SgLogger::DBG, SgLogger::INTERP, className() +
      "::prepareWobInterpData(): the Pxy nterp.data matrix(" + 
      QString("").setNum(wobInterpData_->nCol()) + ", " + 
      QString("").setNum(wobInterpData_->nRow()) + ") has been allocated");
  }
  else if (nRow != (l=wobInterpData_->nRow()))
  {
    delete wobInterpData_;
    wobInterpData_ = new SgMatrix(nRow, 2);
    logger->write(SgLogger::INF, SgLogger::INTERP, className() +
      "::prepareWobInterpData(): adjusted the dimension of the Pxy interp.data matrix rows: " + 
      QString("").setNum(l) +  " => " + QString("").setNum(nRow));
  };
};


// FUNCTIONS:
//
//

/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/
