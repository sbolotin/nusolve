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


#include <SgVlbiSessionInfo.h>


#include <SgVlbiNetworkId.h>
#include <SgLogger.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
QString SgVlbiSessionInfo::className()
{
  return "SgVlbiSessionInfo";
};



// CONSTRUCTORS:
//
// A constructor:
SgVlbiSessionInfo::SgVlbiSessionInfo() : 
  SgAttribute(),
  tCreation_(tZero),
  tStart_(tZero),
  tFinis_(tZero),
  tMean_(tZero),
  name_("unnamed"),
  networkSuffix_("Z"),
  sessionCode_("UNKN"),
  sessionType_(""),
  networkID_("UNKN"),
  description_("an empty entry"),
  userFlag_("H"),
  officialName_("noname"),
  correlatorName_(""),
  correlatorType_("UNKN"),
  submitterName_("UNKN"),
  schedulerName_("UNKN"),
  piAgencyName_("UNKN"),
  recordingMode_("N/A")
{
  originType_ = OT_UNKNOWN;

  experimentSerialNumber_ = 0;
  numOfStations_ = 0;
  numOfBaselines_ = 0;
  numOfSources_ = 0;
  numOfObservations_ = 0;
  cppsSoft_ = CPPS_UNKNOWN;

  delaySumRMS2_ = 0.0;
  rateSumRMS2_ = 0.0;
  delaySumW_ = 0.0;
  rateSumW_ = 0.0;
};



//
SgVlbiSessionInfo::SgVlbiSessionInfo(const SgVlbiSessionInfo& si) : 
  SgAttribute(si)
{
  *this = si;
};



// A destructor:
SgVlbiSessionInfo::~SgVlbiSessionInfo()
{
  // nothing to do more yet...
};



//
SgVlbiSessionInfo& SgVlbiSessionInfo::operator=(const SgVlbiSessionInfo& si)
{
  SgAttribute::operator=(si);
  
  originType_ = si.originType_;
  tCreation_ = si.tCreation_;
  tStart_ = si.tStart_;
  tFinis_ = si.tFinis_;
  tMean_ = si.tMean_;

  name_ = si.name_;
  networkSuffix_ = si.networkSuffix_;
  sessionCode_ = si.sessionCode_;
  sessionType_ = si.sessionType_;
  networkID_ = si.networkID_;
  description_ = si.description_;
  userFlag_ = si.userFlag_;
  officialName_ = si.officialName_;
  correlatorName_ = si.correlatorName_;
  correlatorType_ = si.correlatorType_;
  submitterName_ = si.submitterName_;
  schedulerName_ = si.schedulerName_;
  piAgencyName_ = si.piAgencyName_;
  experimentSerialNumber_ = si.experimentSerialNumber_; 
  recordingMode_ = si.recordingMode_;

  numOfStations_ = si.numOfStations_;
  numOfBaselines_ = si.numOfBaselines_;
  numOfSources_ = si.numOfSources_;
  numOfObservations_ = si.numOfObservations_;

  delaySumRMS2_ = si.delaySumRMS2_;
  rateSumRMS2_ = si.rateSumRMS2_;
  delaySumW_ = si.delaySumW_;
  rateSumW_ = si.rateSumW_;
  
  return *this;
};



// FUNCTIONS:
//
//
void SgVlbiSessionInfo::guessNetworkId()
{
  QMap<int, const SgVlbiNetworkId*> idByCount;
  int                               count;
  
  for (int i=0; i<networks.count(); i++)
  {
    if ((count=networks.at(i).countMatches(getOfficialName())) > 0)
    {
      if (idByCount.contains(count))
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::guessNetworkId(): got multiple matches for the key [" + getOfficialName() +
          "] (count=" + QString("").setNum(count) +
          ") for " + idByCount.value(count)->getName() + " and " + networks.at(i).getName() + 
          " networks; the last one will be used");
      idByCount[count] = &(networks.at(i));
    };
  };
  if (idByCount.size())
  {
    const SgVlbiNetworkId      *nid;
    nid = idByCount.values().last();
    if (nid)
    {
      setNetworkID(nid->getName());
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::guessNetworkId(): the network ID was set to: " + getNetworkID());
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::guessNetworkId(): nid is NULL");
  }
  else
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::guessNetworkId(): unable to guess the network ID, nothing to match");
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
// sorts in chronological order:
bool lessThan4_FirstEpochSortingOrder(SgVlbiSessionInfo *si1, SgVlbiSessionInfo *si2)
{
  return si1->getTStart() > si2->getTStart();
};

/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/

