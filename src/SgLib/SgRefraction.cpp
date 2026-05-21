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

#include <SgConstants.h>
#include <SgLogger.h>
#include <SgRefraction.h>
#include <SgTaskConfig.h>
#include <SgVlbiStationInfo.h>
#include <SgVlbiSourceInfo.h>
#include <SgZenithDelay.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgRefraction::className()
{
  return "SgRefraction";
};



//
SgRefraction::SgRefraction(const SgTaskConfig* cfg)
{
  cfg_ = cfg;

//dryZenithDelay_ = new SgZenithDelay;
  dryZenithDelay_ = new SgDryZD_Saastamoinen;
//wetZenithDelay_ = new SgZenithDelay;
  wetZenithDelay_ = new SgWetZD_Saastamoinen;

  //
  if (cfg->getRefractionModel() == SgTaskConfig::RM_FLYBY_NMF)
  {
    dryMappingFunction_ = new SgDryMF_NMF;
    wetMappingFunction_ = new SgWetMF_NMF;
  }
  else if (cfg->getRefractionModel() == SgTaskConfig::RM_FLYBY_MTT)
  {
    dryMappingFunction_ = new SgDryMF_MTT;
    wetMappingFunction_ = new SgWetMF_MTT;
  }
  else
  {
    dryMappingFunction_ = new SgMappingFunction;
    wetMappingFunction_ = new SgMappingFunction;
  };
  
  gradientMappingFunction_ = new SgGradMF_CH;
  dDryZD_ = 0.0;
  dWetZD_ = 0.0;
  dDryMF_ = 0.0;
  dWetMF_ = 0.0;
  dGrdMF_ = 0.0;
  dGradientDelay_ = 0.0;
  dTotalZD_ = 0.0;
  dTotalDelay_ = 0.0;
};



// A destructor:
SgRefraction::~SgRefraction()
{
  if (dryZenithDelay_)
  {
    delete dryZenithDelay_;
    dryZenithDelay_ = NULL;
  };
  if (wetZenithDelay_)
  {
    delete wetZenithDelay_;
    wetZenithDelay_ = NULL;
  };
  if (dryMappingFunction_)
  {
    delete dryMappingFunction_;
    dryMappingFunction_ = NULL;
  };
  if (wetMappingFunction_)
  {
    delete wetMappingFunction_;
    wetMappingFunction_ = NULL;
  };
  if (gradientMappingFunction_)
  {
    delete gradientMappingFunction_;
    gradientMappingFunction_ = NULL;
  };
};



//
double SgRefraction::calcTroposphericDelay(const SgMJD& epoch, const SgMeteoData& meteo,
          double e, double a,
          SgVlbiStationInfo* stnInfo, const SgVlbiSourceInfo* /*srcInfo*/)
{
  //  double gradN = stnInfo->pAtmGradN()->getSolution();
  //  double gradE = stnInfo->pAtmGradE() ? stnInfo->pAtmGradE()->getSolution() : 0.0;
  dGradientDelay_ = 0.0;
  
  dDryZD_ = dryZenithDelay_->calc(meteo, stnInfo);
  dWetZD_ = wetZenithDelay_->calc(meteo, stnInfo) /*+ stnInfo->getPZenithDelay()->getSolution()*/;
  //dWetZD_ = 0.0;

  dDryMF_ = dryMappingFunction_->calc(meteo, e, stnInfo, epoch);
  dWetMF_ = wetMappingFunction_->calc(meteo, e, stnInfo, epoch);

  dGrdMF_ = gradientMappingFunction_->calc(meteo, e, stnInfo, epoch);
  //
  // evaluate a contribution to the delay caused by a priori mean gradients, 
  // that could be used if there is no proper LCode:
  if (cfg_->getUseExtAPrioriMeanGradients())
    dGradientDelay_ = (stnInfo->getGradNorth()*cos(a) + stnInfo->getGradEast()*sin(a))*dGrdMF_;
  
  stnInfo->setGradientDelay(dGradientDelay_);
  
  dTotalZD_  = dDryZD_ + dWetZD_;
  
  dTotalDelay_ = dDryZD_*dDryMF_ + dWetZD_*dWetMF_;
  
  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("LOS delay (%s): %20.14f(m) or %24.14f(ps)",
      qPrintable(stnInfo->getKey()), dTotalDelay_, dTotalDelay_/vLight*1.0e12);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return dTotalDelay_;
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








/*=====================================================================================================*/
