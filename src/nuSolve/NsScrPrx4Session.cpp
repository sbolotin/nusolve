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
*                           NsScrPrx4Band's METHODS:
* 
*======================================================================================================*/
void NsScrPrx4Band::postLoad(NsScrPrx4Session* sessionProxy)
{
  prxStations_.clear();
  prxBaselines_.clear();
  prxSources_.clear();
  //
  // stations:
  for (StationsByName_it it=bnd()->stationsByName().begin();
    it!=bnd()->stationsByName().end(); ++it)
    prxStations_ << new NsScrPrx4Station(*it.value());
  // baselines:
  for (BaselinesByName_it it=bnd()->baselinesByName().begin();
    it!=bnd()->baselinesByName().end(); ++it)
  {
    SgVlbiBaselineInfo         *bln=it.value();
    NsScrPrx4Baseline           *blnProxy=new NsScrPrx4Baseline(*bln);
    prxBaselines_ << blnProxy;
    // per band baselines:
    for (int i=0; i<bln->observables().size(); i++)
      blnProxy->prxObservations_ << new NsScrPrx4VlbiObservation(*bln->observables().at(i)->owner(), sessionProxy);
    //
  };
  // sources:
  for (SourcesByName_it it=bnd()->sourcesByName().begin();
    it!=bnd()->sourcesByName().end(); ++it)
    prxSources_ << new NsScrPrx4Source(*it.value());
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                          NsScrPrx4Station's METHODS:
* 
*======================================================================================================*/
bool NsScrPrx4Station::setFlybyCableCalSource(NsScrPrx4TaskConfig::CableCalSource s)
{
  switch (s)
  {
    case NsScrPrx4TaskConfig::CCS_FSLG:
      if (stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_FSLG))
        stn()->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);
      else
        return false;
    break;
    case NsScrPrx4TaskConfig::CCS_CDMS:
      if (stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS))
        stn()->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);
      else
        return false;
    break;
    case NsScrPrx4TaskConfig::CCS_PCMT:
      if (stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT))
        stn()->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);
      else
        return false;
    break;
    case NsScrPrx4TaskConfig::CCS_DEFAULT:
    default:
      stn()->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);
    break;
  };
  return true;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                          NsScrPrx4Session's METHODS:
* 
*======================================================================================================*/
void NsScrPrx4Session::postLoad()
{
  prxBands_.clear();
  prxStations_.clear();
  prxBaselines_.clear();
  prxSources_.clear();
  stationsByKey_.clear();
  baselinesByKey_.clear();
  sourcesByKey_.clear();
  // bands:
  for (int i=0; i<session_->bands().size(); i++)
  {
    NsScrPrx4Band               *bnd=new NsScrPrx4Band(*session_->bands().at(i));
    bnd->postLoad(this);
    prxBands_ << bnd;
    if (session_->bands().at(i)->isAttr(SgVlbiBand::Attr_PRIMARY))
      primaryBandIdx_ = i;
  };
  // stations:
  for (StationsByName_it it=session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    NsScrPrx4Station           *proxy=new NsScrPrx4Station(*si);
    prxStations_ << proxy;
    //
    proxy->prxAuxes_.clear();
    for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=si->auxObservationByScanId()->begin(); 
      jt!=si->auxObservationByScanId()->end(); ++jt)
      proxy->prxAuxes_ << new NsScrPrx4VlbiAuxObservation(*jt.value(), proxy);
    //
  };
  // baselines:
  for (BaselinesByName_it it=session_->baselinesByName().begin();
    it!=session_->baselinesByName().end(); ++it)
    prxBaselines_ << new NsScrPrx4Baseline(*it.value());
  // sources:
  for (SourcesByName_it it=session_->sourcesByName().begin();
    it!=session_->sourcesByName().end(); ++it)
    prxSources_ << new NsScrPrx4Source(*it.value());
  // obs:
  for (int i=0; i<session_->observations().size(); i++)
    prxObservations_ << new NsScrPrx4VlbiObservation(*session_->observations().at(i), this);
  //
  // fill the maps:
  for (int i=0; i<prxStations_.size(); i++)
    stationsByKey_.insert(prxStations_.at(i)->getKey(), prxStations_.at(i));
  for (int i=0; i<prxBaselines_.size(); i++)
    baselinesByKey_.insert(prxBaselines_.at(i)->getKey(), prxBaselines_.at(i));
  for (int i=0; i<prxSources_.size(); i++)
    sourcesByKey_.insert(prxSources_.at(i)->getKey(), prxSources_.at(i));
};



//
void NsScrPrx4Session::dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode mode)
{
  for (QMap<QString, SgVlbiStationInfo*>::iterator it = session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->pcClocks()->setPMode(mode);
  };
};



//
void NsScrPrx4Session::dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode mode)
{
  for (QMap<QString, SgVlbiStationInfo*>::iterator it = session_->stationsByName().begin();
    it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->pcZenith()->setPMode(mode);
  };
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
// constants:
//


/*=====================================================================================================*/





/*=====================================================================================================*/

