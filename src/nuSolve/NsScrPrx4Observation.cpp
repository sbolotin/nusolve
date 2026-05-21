/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2017-2020 Sergei Bolotin.
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
#include "nuSolve.h"
#include "NsScrPrx4Observation.h"
#include "NsScrPrx4Session.h"


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
double NsScrPrx4VlbiObservation::delayValue(const QString& bandKey, 
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         v(0.0);
  if (!obs_->observableByKey().contains(bandKey))
    return v;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      v = 0.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      v = obs_->observableByKey().value(bandKey)->sbDelay().getValue();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      v = obs_->observableByKey().value(bandKey)->grDelay().getValue();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      v = obs_->observableByKey().value(bandKey)->phDelay().getValue();
      break;
  };
  return v;
};



//
double NsScrPrx4VlbiObservation::delayValueGeoc(const QString& bandKey, 
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         v(0.0);
  if (!obs_->observableByKey().contains(bandKey))
    return v;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      v = 0.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      v = obs_->observableByKey().value(bandKey)->sbDelay().getGeocenterValue();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      v = obs_->observableByKey().value(bandKey)->grDelay().getGeocenterValue();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      v = obs_->observableByKey().value(bandKey)->phDelay().getGeocenterValue();
      break;
  };
  return v;
};




//
double NsScrPrx4VlbiObservation::delayStdDev(const QString& bandKey,
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         v(-1.0);
  if (!obs_->observableByKey().contains(bandKey))
    return v;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      v = 0.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      v = obs_->observableByKey().value(bandKey)->sbDelay().getSigma();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      v = obs_->observableByKey().value(bandKey)->grDelay().getSigma();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      v = obs_->observableByKey().value(bandKey)->phDelay().getSigma();
      break;
  };
  return v;
};



//
double NsScrPrx4VlbiObservation::delayResidual(const QString& bandKey,
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         r(0.0);
  if (!obs_->observableByKey().contains(bandKey))
    return r;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      r = 0.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      r = obs_->observableByKey().value(bandKey)->sbDelay().getResidual();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      r = obs_->observableByKey().value(bandKey)->grDelay().getResidual();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      r = obs_->observableByKey().value(bandKey)->phDelay().getResidual();
      break;
  };
  return r;
};



//
double NsScrPrx4VlbiObservation::delayResidualNorm(const QString& bandKey, 
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         r(0.0);
  if (!obs_->observableByKey().contains(bandKey))
    return r;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      r = 0.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      r = obs_->observableByKey().value(bandKey)->sbDelay().getResidualNorm();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      r = obs_->observableByKey().value(bandKey)->grDelay().getResidualNorm();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      r = obs_->observableByKey().value(bandKey)->phDelay().getResidualNorm();
      break;
  };
  return r;
};



//
double NsScrPrx4VlbiObservation::delayAmbiguitySpacing(const QString& bandKey, 
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  double                         r(-1.0);
  if (!obs_->observableByKey().contains(bandKey))
    return r;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      r = -1.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      r = obs_->observableByKey().value(bandKey)->sbDelay().getAmbiguitySpacing();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      r = obs_->observableByKey().value(bandKey)->grDelay().getAmbiguitySpacing();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      r = obs_->observableByKey().value(bandKey)->phDelay().getAmbiguitySpacing();
      break;
  };
  return r;
};



//
int NsScrPrx4VlbiObservation::delayNumOfAmbiguities(const QString& bandKey, 
  NsScrPrx4TaskConfig::VlbiDelayType t) const
{
  int                           n(0);
  if (!obs_->observableByKey().contains(bandKey))
    return n;
  switch (t)
  {
    default:
    case NsScrPrx4TaskConfig::VD_NONE:
      n = -1.0;
      break;
    case NsScrPrx4TaskConfig::VD_SB_DELAY:
      n = obs_->observableByKey().value(bandKey)->sbDelay().getNumOfAmbiguities();
      break;
    case NsScrPrx4TaskConfig::VD_GRP_DELAY:
      n = obs_->observableByKey().value(bandKey)->grDelay().getNumOfAmbiguities();
      break;
    case NsScrPrx4TaskConfig::VD_PHS_DELAY:
      n = obs_->observableByKey().value(bandKey)->phDelay().getNumOfAmbiguities();
      break;
  };
  return n;
};



//
NsScrPrx4Station* NsScrPrx4VlbiObservation::getStation_1()
{
  return session_->lookUpStation(obs_->stn_1()->getKey());
};



//
NsScrPrx4Station* NsScrPrx4VlbiObservation::getStation_2()
{
  return session_->lookUpStation(obs_->stn_2()->getKey());
};



//
NsScrPrx4Baseline* NsScrPrx4VlbiObservation::getBaseline()
{
  return session_->lookUpBaseline(obs_->baseline()->getKey());
};



//
NsScrPrx4Source* NsScrPrx4VlbiObservation::getSource()
{
  return session_->lookUpSource(obs_->src()->getKey());
};



/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//


















/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/





/*=====================================================================================================*/
