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


#include <QtCore/QDataStream>

#include <SgObjectInfo.h>
#include <SgLogger.h>
#include <SgTaskConfig.h>

/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgObjectInfo::className()
{
  return "SgObjectInfo";
};
//




//
void SgObjectInfo::recalcSigma2add(const SgTaskConfig* cfg)
{
  if (cfg->getUseDelayType() != SgTaskConfig::VD_NONE)
    delayWrms_.recalcSigma2add(cfg->getMinAuxSigma4Delay());
//if (true || cfg->getUseRateType() != SgTaskConfig::VR_NONE)
    rateWrms_ .recalcSigma2add(cfg->getMinAuxSigma4Rate());
};





//
bool SgObjectInfo::saveIntermediateResults(QDataStream& s) const
{
  s << getKey() << getAttributes() << getAka() << getIdx();
  delayWrms_. saveIntermediateResults(s);
  rateWrms_.  saveIntermediateResults(s);
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgObjectInfo::loadIntermediateResults(QDataStream& s)
{
  QString                       key, aka;
  unsigned int                  attributes;
  int                           idx;
  s >> key >> attributes >> aka >> idx;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  delayWrms_. loadIntermediateResults(s);
  rateWrms_.  loadIntermediateResults(s);
  //
  if (getKey() != key)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: wrong order, key mismatch: got [" + key +
      "], expected [" + getKey() + "]");
    return false;
  };
  setAttributes(attributes);
  //
  return s.status()==QDataStream::Ok;
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
