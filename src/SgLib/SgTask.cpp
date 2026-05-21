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

#include <SgTask.h>


#include <SgLogger.h>
#include <SgVlbiSession.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTask::className()
{
  return "SgTask";
};


// A destructor:
SgTask::~SgTask()
{
  // SgTask is an owner of them, so delete the pointers:
  // sessionInfos:
  for (SessionInfosByName_it i=sessionsByName_.begin(); i!=sessionsByName_.end(); ++i)
    delete i.value();
  sessionsByName_.clear();
  // stationInfos:
  for (StationsByName_it i=stationsByName_.begin(); i!=stationsByName_.end(); ++i)
    delete i.value();
  stationsByName_.clear();
  // baselineInfos:
  for (BaselinesByName_it i=baselinesByName_.begin(); i!=baselinesByName_.end(); ++i)
    delete i.value();
  baselinesByName_.clear();
  // sourceInfos:
  for (SourcesByName_it i=sourcesByName_.begin(); i!=sourcesByName_.end(); ++i)
    delete i.value();
  sourcesByName_.clear();
};



//
void SgTask::addSession(SgVlbiSessionInfo* sInfo, SgVlbiSession* session)
{
  // check for NULLs:
  if (!sInfo)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": addSession(SgVlbiSessionInfo*, SgVlbiSession*): sInfo is NULL");
    return;
  };
  if (!sInfo)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": addSession(SgVlbiSessionInfo*, SgVlbiSession*): session is NULL");
    return;
  };
  
  // check for duplicates:
  if (sessionsByName_.contains(sInfo->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      ": addSession(SgVlbiSessionInfo*, SgVlbiSession*): session " + sInfo->getName() + 
      " already in the task " + name_);
    return;
  };
  
  // add a session to the container:
  sessionsByName_.insert(sInfo->getName(), new SgVlbiSessionInfo(*sInfo));
  
  // update statistics per object:
  // stations:
  for (QMap<QString, SgVlbiStationInfo*>::iterator i=session->stationsByName().begin(); 
    i!=session->stationsByName().end(); ++i)
  {
    SgVlbiStationInfo* stationInfo = NULL;
    if (stationsByName_.contains(i.key()))
    {
      stationInfo = stationsByName_.value(i.key());
      stationInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
    }
    else
    {
      stationInfo = new SgVlbiStationInfo(stationsByName_.size(), 
        i.value()->getKey(), i.value()->getAka());
      stationInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      stationsByName_.insert(stationInfo->getKey(), stationInfo);
    };
  };
  // sources:
  for (QMap<QString, SgVlbiSourceInfo*>::iterator i=session->sourcesByName().begin(); 
    i!=session->sourcesByName().end(); ++i)
  {
    SgVlbiSourceInfo* sourceInfo = NULL;
    if (sourcesByName_.contains(i.key()))
    {
      sourceInfo = sourcesByName_.value(i.key());
      sourceInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
    }
    else
    {
      sourceInfo = new SgVlbiSourceInfo(sourcesByName_.size(),
        i.value()->getKey(), i.value()->getAka());
      sourceInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      sourcesByName_.insert(sourceInfo->getKey(), sourceInfo);
    };
  };
  // baselines:
  for (QMap<QString, SgVlbiBaselineInfo*>::iterator i=session->baselinesByName().begin(); 
    i!=session->baselinesByName().end(); ++i)
  {
    SgVlbiBaselineInfo* baselineInfo = NULL;
    if (baselinesByName_.contains(i.key()))
    {
      baselineInfo = baselinesByName_.value(i.key());
      baselineInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
    }
    else
    {
      baselineInfo = new SgVlbiBaselineInfo(baselinesByName_.size(),
        i.value()->getKey(), i.value()->getAka());
      baselineInfo->incNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      baselinesByName_.insert(baselineInfo->getKey(), baselineInfo);
    };
  };
  //
};



//
void SgTask::removeSession(SgVlbiSessionInfo* sInfo, SgVlbiSession* session)
{
  // check for NULLs:
  if (!sInfo)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): sInfo is NULL");
    return;
  };
  if (!sInfo)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): session is NULL");
    return;
  };
  
  // check for duplicates:
  if (!sessionsByName_.contains(sInfo->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): session " + sInfo->getName() + 
      " is not a part of the task " + name_);
    return;
  };

  // update statistics per object:
  // stations:
  for (QMap<QString, SgVlbiStationInfo*>::iterator i=session->stationsByName().begin(); 
    i!=session->stationsByName().end(); ++i)
  {
    SgVlbiStationInfo* stationInfo = NULL;
    if (stationsByName_.contains(i.key()))
    {
      stationInfo = stationsByName_.value(i.key());
      stationInfo->decNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      if (stationInfo->numTotal(DT_DELAY)<0) // sometimes it could happen,correct it:
      {
        logger->write(SgLogger::INF, SgLogger::RUN, className() +
          ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): the station " + 
          stationInfo->getKey() + " has got num of total obs. less than zero, fixed it");
//        stationInfo->setNumTotal(DT_DELAY, 0);
      };
      if (stationInfo->numTotal(DT_DELAY)==0) // remove the object from the task:
      {
        stationsByName_.remove(stationInfo->getKey());
        delete stationInfo;
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::RUN, className() +
        ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): cannot remove the station " + 
        i.key() + " from the container; the task is " + name_);
  };
  // sources:
  for (QMap<QString, SgVlbiSourceInfo*>::iterator i=session->sourcesByName().begin(); 
    i!=session->sourcesByName().end(); ++i)
  {
    SgVlbiSourceInfo* sourceInfo = NULL;
    if (sourcesByName_.contains(i.key()))
    {
      sourceInfo = sourcesByName_.value(i.key());
      sourceInfo->decNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      if (sourceInfo->numTotal(DT_DELAY) < 0) // sometimes it could happen,correct it:
      {
        logger->write(SgLogger::INF, SgLogger::RUN, className() +
          ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): the source " + 
          sourceInfo->getKey() + " has got num of total obs. less than zero, fixed it");
//        sourceInfo->setTotalNum(0);
      };
      if (sourceInfo->numTotal(DT_DELAY) == 0) // remove the object from the task:
      {
        sourcesByName_.remove(sourceInfo->getKey());
        delete sourceInfo;
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::RUN, className() +
        ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): cannot remove the source " + 
        i.key() + " from the container; the task is " + name_);
  };
  // baselines:
  for (QMap<QString, SgVlbiBaselineInfo*>::iterator i=session->baselinesByName().begin(); 
    i!=session->baselinesByName().end(); ++i)
  {
    SgVlbiBaselineInfo* baselineInfo = NULL;
    if (baselinesByName_.contains(i.key()))
    {
      baselineInfo = baselinesByName_.value(i.key());
      baselineInfo->decNumTotal(DT_DELAY, i.value()->numTotal(DT_DELAY));
      if (baselineInfo->numTotal(DT_DELAY) < 0) // sometimes it could happen,correct it:
      {
        logger->write(SgLogger::INF, SgLogger::RUN, className() +
          ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): the baseline " + 
          baselineInfo->getKey() + " has got num of total obs. less than zero, fixed it");
//        baselineInfo->setTotalNum(0);
      };
      if (baselineInfo->numTotal(DT_DELAY) == 0) // remove the object from the task:
      {
        baselinesByName_.remove(baselineInfo->getKey());
        delete baselineInfo;
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::RUN, className() +
        ": removeSession(SgVlbiSessionInfo*, SgVlbiSession*): cannot remove the baseline " + 
        i.key() + " from the container; the task is " + name_);
  };

  // remove a sessionInfo from the container and delete it:
  delete sessionsByName_.take(sInfo->getName());
  //
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
