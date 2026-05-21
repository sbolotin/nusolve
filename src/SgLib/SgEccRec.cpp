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
#include <SgLogger.h>


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgEccRec::className()
{
  return "SgEccRec";
};



//
bool SgEccRec::parseString(const QString& str)
{
  bool            isOk;
  double          r1, r2, r3;
  isOk_ = false;
  //          1         2         3         4         5         6         7         8         9
  //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
  //  AIRA     7348  1970.01.01-00:00  2050.01.01-00:00      0.0        0.0        0.0     XYZ 
  //  PT.REYES 7251  1983.08.27-00:00  2050.01.01-00:00      0.3298     2.4913     4.4005  NEU
                  
  siteName_ = str.mid(2,8);
  cdpNumber_ = str.mid(11, 4);
  nCDP_ = cdpNumber_.toInt(&isOk);
  if (!isOk || nCDP_<999) // it can happend
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      ": parseString(): not a CDP number: " + qPrintable(cdpNumber_)  );
    nCDP_ = 0;
  };
                  
  if (!tSince_.fromString(SgMJD::F_ECCDAT, str.mid(17, 16))) 
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      ": parseString(): cannot get tSince field from [" + qPrintable(str.mid(17, 16)) + "] string");
    return isOk_;
  };
  if (!tTill_.fromString(SgMJD::F_ECCDAT, str.mid(35, 16))) 
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      ": parseString(): cannot get tTill field from [" + qPrintable(str.mid(36, 16)) + "] string");
    return isOk_;
  };
  if (tTill_<tSince_)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString(): wrong order of epochs: [" + qPrintable(str.mid(17, 34)) + "]");
    return isOk_;
  };
  
  // value of the eccentricity
  r1 = str.mid(53,10).simplified().toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString(): cannot get r1 field from [" + qPrintable(str.mid(53, 10)) + "] string");
    return isOk_;
  };
  r2 = str.mid(64,10).simplified().toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString(): cannot get r2 field from [" + qPrintable(str.mid(64, 10)) + "] string");
    return isOk_;
  };
  r3 = str.mid(75,10).simplified().toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString(): cannot get r3 field from [" + qPrintable(str.mid(75, 10)) + "] string");
    return isOk_;
  };
  // type of the eccentricity:
  if (str.mid(87,3) == "NEU")
  {
    dR_(VERTICAL) = r3;
    dR_(EAST)     = r2;
    dR_(NORTH)    = r1;
    eccType_ = ET_NEU;
  }
  else if (str.mid(87,3) == "XYZ")
  {
    dR_(X_AXIS) = r1;
    dR_(Y_AXIS) = r2;
    dR_(Z_AXIS) = r3;
    eccType_ = ET_XYZ;
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString(): unknown type [" + qPrintable(str.mid(87, 3)) + "]");
    return isOk_;
  };
  return (isOk_=true);
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
