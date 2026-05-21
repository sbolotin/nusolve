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


#include <QtCore/QDataStream>

#include <SgLogger.h>
#include <SgParameterCfg.h>

const QString typeNames[] = {"WhiteNoise", "MarkovProcess", "RandomWalk"};

const QString modeNames[] = {"None", "Global", "Arc", "Local", "PWL", "Stochastic"};

/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgParameterCfg::className()
{
  return "SgParameterCfg";
};



// a constuctor:
SgParameterCfg::SgParameterCfg(const QString& name, PMode pMode, SType sType, double convAPriori,
                        double arcStep, double stocAPriori, double breakNoise, double tau,
                        double whiteNoise, double pwlStep, double pwlAPriori, int pwlNumOfPolynomials,
                        double scale, const QString& scaleName)
: SgAttribute()
{
  name_ = name;
  pMode_= pMode;
  sType_= sType;
  scale_ = scale;
  scaleName_ = scaleName;
  //
  convAPriori_ = convAPriori;
  arcStep_ = arcStep;
  //
  pwlAPriori_ = pwlAPriori;
  pwlStep_ = pwlStep;
  pwlNumOfPolynomials_ = pwlNumOfPolynomials;
  //
  stocAPriori_ = stocAPriori;
  breakNoise_ = breakNoise;
  tau_ = tau;
  whiteNoise_ = whiteNoise;
};



//
const QString& SgParameterCfg::type2String(SType type)
{
  return typeNames[type];
};



//
const QString& SgParameterCfg::mode2String(PMode mode)
{
  return modeNames[mode];
};



//
bool SgParameterCfg::saveIntermediateResults(QDataStream& s) const
{
  s << name_ << getAttributes() << (unsigned int)pMode_ << (unsigned int)sType_ << scale_ << scaleName_ 
    << convAPriori_ << arcStep_ << pwlAPriori_ << pwlStep_ << pwlNumOfPolynomials_ << stocAPriori_ 
    << breakNoise_ << tau_ << whiteNoise_;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgParameterCfg::loadIntermediateResults(QDataStream& s)
{
  QString                       name, scaleName;
  unsigned int                  attributes, pMode, sType;
  double                        scale, convAPriori, arcStep, pwlAPriori, pwlStep, stocAPriori;
  double                        breakNoise, tau, whiteNoise;
  int                           pwlNumOfPolynomials;
  //
  s >> name >> attributes >> pMode >> sType >> scale >> scaleName 
    >> convAPriori >> arcStep >> pwlAPriori >> pwlStep >> pwlNumOfPolynomials >> stocAPriori 
    >> breakNoise >> tau >> whiteNoise;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (name_ != name)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: wrong order, names mismatch: got [" + name +
      "], expected [" + name_ + "]");
    return false;
  };
  setAttributes(attributes);
  pMode_ = (PMode) pMode;
  sType_ = (SType) sType;
  scale_ = scale;
  scaleName_ = scaleName;
  convAPriori_ = convAPriori;
  arcStep_ = arcStep;
  pwlAPriori_ = pwlAPriori;
  pwlStep_ = pwlStep;
  pwlNumOfPolynomials_ = pwlNumOfPolynomials;
  stocAPriori_ = stocAPriori;
  breakNoise_ = breakNoise;
  tau_ = tau;
  whiteNoise_ = whiteNoise;
  //
  return s.status()==QDataStream::Ok;
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
