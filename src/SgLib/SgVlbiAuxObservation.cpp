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


#include <SgVlbiAuxObservation.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
// An empty constructor:
SgVlbiAuxObservation::SgVlbiAuxObservation() :
    SgObservation(), meteoData_(), 
    ocnLdR_(v3Zero),
    ocnLdV_(v3Zero),
    cableCorrections_(4),
    tapeId_(""),
    tmdFlyBy_(),
    tmdContribInternal_(),
    tmdContribExternal_()
{
  owner_ = NULL;
  stationIdx_ = -1;
  cableCalibration_ = 0.0;
  azimuthAngle_ = -10.0; // unrealistic data
  elevationAngle_ = -10.0; // unrealistic data
  azimuthAngleRate_ = -10.0; // unrealistic data
  elevationAngleRate_ = -10.0; // unrealistic data
  parallacticAngle_ = 0.0;
  aprioriClocks_ = 0.0;
  estClocks_ = 0.0;
  estClocksSigma_ = 0.0;
  estZenithDelay_ = 0.0;
  estZenithDelaySigma_ = 0.0;
  estAtmGradN_ = 0.0;
  estAtmGradNSigma_ = 0.0;
  estAtmGradE_ = 0.0;
  estAtmGradESigma_ = 0.0;
  zenithDelayH_ = 0.0;
  zenithDelayW_ = 0.0;
  calcNdryCont4Delay_ = 0.0;
  calcNdryCont4Rate_ = 0.0;
  calcNwetCont4Delay_ = 0.0;
  calcNwetCont4Rate_ = 0.0;
  calcUnPhaseCal_ = 0.0;
  calcAxisOffset4Delay_ = 0.0;
  calcAxisOffset4Rate_ = 0.0;
  calcOLoadHorz4Delay_ = 0.0;
  calcOLoadHorz4Rate_ = 0.0;
  calcOLoadVert4Delay_ = 0.0;
  calcOLoadVert4Rate_ = 0.0;
  dDel_dAxsOfs_   = 0.0;
  dRat_dAxsOfs_   = 0.0;
  dDel_dTzdDry_   = 0.0;
  dRat_dTzdDry_   = 0.0;
  dDel_dTzdWet_   = 0.0;
  dRat_dTzdWet_   = 0.0;
  dDel_dTzdGrdN_  = 0.0;
  dRat_dTzdGrdN_  = 0.0;
  dDel_dTzdGrdE_  = 0.0;
  dRat_dTzdGrdE_  = 0.0;
  refClockOffset_ = 0.0;

  tSyses_ = NULL;
  //tmdActual_ = &tmdContribInternal_;
  tmdActual_ = &tmdFlyBy_;
};





//
// static first:



//
bool SgVlbiAuxObservation::selfCheck()
{
  bool isOk = true;
  
  // make selfcheck here:
  
  return isOk;
};



//  
bool SgVlbiAuxObservation::isEligible(const SgTaskConfig* cfg)
{
  bool isOk = SgObservation::isEligible(cfg);
  
  // make selfcheck here:
  
  return isOk;
};



//
void SgVlbiAuxObservation::prepare4Analysis(SgTaskManager*)
{
};



//
void SgVlbiAuxObservation::evaluateResiduals(SgTaskManager*, SgVlbiBand*, bool)
{
};



//
void SgVlbiAuxObservation::evaluateResiduals(SgTaskManager*)
{
};



//
void SgVlbiAuxObservation::evaluateTheoreticalValues(SgTaskManager*)
{
};



//
const SgVector& SgVlbiAuxObservation::o_c()
{
  logger->write(SgLogger::WRN, SgLogger::OBS | SgLogger::ESTIMATOR, className() + 
    ": call to (O-C)");  
  return vZero;
};



//
const SgVector& SgVlbiAuxObservation::sigma()
{
  logger->write(SgLogger::WRN, SgLogger::OBS | SgLogger::ESTIMATOR, className() + 
    ": call to sigma");  
  return vZero;
};





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

const SgTroposphereModelData    tmdZero;




/*=====================================================================================================*/
