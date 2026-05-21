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

#include <SgEccRec.h>
#include <SgEccSite.h>
#include <SgLogger.h>



bool tillEpochSortingOrderLessThan(SgEccRec *r1, SgEccRec *r2)
{
  return r1->getTTill() > r2->getTTill();
};


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgEccSite::className()
{
  return "SgEccSite";
};



//
SgEccSite::~SgEccSite()
{
  for (int i=0; i<records_.size(); i++)
    delete records_.at(i);
  records_.clear();
};



//
bool SgEccSite::insertRecord(SgEccRec* rec)
{
  // check for clinical cases:
  if (!rec)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the record is NULL");
    return false;
  };
  if (rec->getSiteName() != siteName_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the record's name [" + rec->getSiteName() + 
      "] is not the same as the site's one [" + siteName_ + "]");
    return false;
  };
  if (records_.size()==0) // first record, just insert it:
  {
    records_.append(rec);
    tBegin_ = rec->getTSince();
    tEnd_ = rec->getTTill();
    return true;
  };
  // some additional checks:
  if (rec->getTTill()>tEnd_ && rec->getTSince()>tBegin_ && tEnd_>rec->getTSince())
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the " + siteName_ + "'s records are overlapped: list: [" +  
      tBegin_.toString() + ":" + tEnd_.toString() + "]; the record: [" +  
      rec->getTSince().toString() + ":" + rec->getTTill().toString() + "]");
  if (tBegin_>rec->getTSince() && rec->getTTill()>tBegin_ && tEnd_>rec->getTTill())
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the " + siteName_ + "'s records are overlapped: list: [" +  
      tBegin_.toString() + ":" + tEnd_.toString() + "]; the record: [" +  
      rec->getTSince().toString() + ":" + rec->getTTill().toString() + "]");
  if (rec->getTSince()>tBegin_ && tEnd_>rec->getTTill())
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the " + siteName_ + "'s records are overlapped: list: [" +  
      tBegin_.toString() + ":" + tEnd_.toString() + "]; the record: [" +  
      rec->getTSince().toString() + ":" + rec->getTTill().toString() + "]");
  if (tBegin_>rec->getTSince() && rec->getTTill()>tEnd_)
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": insertRecord(): the " + siteName_ + "'s records are overlapped: list: [" +  
      tBegin_.toString() + ":" + tEnd_.toString() + "]; the record: [" +  
      rec->getTSince().toString() + ":" + rec->getTTill().toString() + "]");

  // append the record:
  records_.append(rec);
  if (tBegin_>rec->getTSince())
    tBegin_ = rec->getTSince();
  if (rec->getTTill()>tEnd_)
    tEnd_ = rec->getTTill();

  return true;
};



//
void SgEccSite::checkRecords()
{
  if (records_.size()<2)
    return;
  qSort(records_.begin(), records_.end(), tillEpochSortingOrderLessThan);

  for (int i=0; i<records_.size()-1; i++)
  {
    if (records_.at(i)->getTTill() < records_.at(i+1)->getTSince())
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": checkRecords(): there are gaps in " + siteName_ + "'s records: from " + 
      records_.at(i)->getTTill().toString() + " till " + 
      records_.at(i+1)->getTSince().toString());
  };
};



//
SgEccRec* SgEccSite::findRecord(const SgMJD& t)
{
  SgEccRec* rec=NULL;
  for (int i=0; i<records_.size(); i++)
    if (records_.at(i)->getTSince()<=t && t<=records_.at(i)->getTTill())
      rec = records_.at(i);
  if (!rec)
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": findRecord(): cannot find ecc record for the epoch " + t.toString());
  return rec;
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
