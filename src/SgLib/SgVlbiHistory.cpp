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


#include <SgDbhImage.h>
#include <SgDbhFormat.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVlbiHistory.h>






/*=======================================================================================================
*
*               SgVlbiHistory's METHODS:
* 
*======================================================================================================*/
void SgVlbiHistory::importDbhHistoryBlock(SgDbhImage* image)
{
  if (size())
  {
    for (int i=0; i<size(); i++)
      delete at(i);
    clear();
    logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
      ": importDbhHistoryBlock(): the list is cleared");
  };
  for (int i=0; i<image->historyList().size(); i++)
  {
    SgDbhHistoryEntry *e = image->historyList().at(i);
    SgVlbiHistoryRecord *rec = new SgVlbiHistoryRecord(e->getEpoch(), e->getVersion(), e->getText());
    append(rec);
  };
  // put fingerprint: (should it be here?)
  append(new SgVlbiHistoryRecord(SgMJD::currentMJD().toUtc(), image->currentVersion(),
    libraryVersion.name() + ": data have been read from Mk3 database"));
  // R1619: GSFC Analysis  -DG-
};



//
void SgVlbiHistory::export2DbhHistoryBlock(SgDbhImage* image)
{
  for (int i=0; i<size(); i++)
  {
    SgVlbiHistoryRecord *rec=at(i);
    image->addHistoryEntry(rec->getText(), rec->getEpoch());
  };
};
  


//
void SgVlbiHistory::addHistoryRecord(const QString& record, const SgMJD& t)
{
  append(new SgVlbiHistoryRecord(t, -1, record, false));
};
/*=====================================================================================================*/

/*=====================================================================================================*/
