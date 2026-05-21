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


#include <SgLogger.h>
#include <SgTaskConfig.h>
#include <SgVlbiMeasurement.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiMeasurement::className()
{
  return "SgVlbiMeasurement";
};



//
void SgVlbiMeasurement::setupSigma2Apply(const SgTaskConfig* cfg)
{
  // see libs/cutil/get_calib.f (DERR)
  //     libs/cutil/socal.f
  double                        d;
  d = 1.0;
  if ( ionoSigma_ > 0.0 &&                      // ionosphere correction is on
      !cfg->getIsIonosphereFreeUncorrelated())  // take into account the correlion of s/x combination
    d += 2.0*dSign_*q2_;
  sigma2Apply_ = sqrt( fabs(d)*sigma_*sigma_ + ionoSigma_*ionoSigma_ + sigma2add_*sigma2add_ );
};



//
void SgVlbiMeasurement::resetAllEditings()
{
  numOfAmbiguities_ = 0;
  numOfSubAmbigs_ = 0;
  sigma2add_ = 0.0;
  sigma2Apply_ = 0.0;
  residual_ = 0.0;
  residualNorm_ = 0.0;
  q2_         = 0.0;
  ionoValue_  = 0.0;
  ionoSigma_  = 0.0;
  //
  delAttr(Attr_NOT_VALID);
  // delAttr(Attr_PROCESSED);
  delAttr(Attr_IMMUNE2INCL);
  delAttr(Attr_IMMUNE2EXCL);
  delAttr(Attr_HAS_IONO_CORR);
};



//
void SgVlbiMeasurement::adjustAmbiguity(double closestResidual)
{
  if (ambiguitySpacing_==0.0)
    return;
  int                           additionalNumAmbig;
  additionalNumAmbig = round((residual_ - closestResidual)/ambiguitySpacing_);
  residual_ -= additionalNumAmbig*ambiguitySpacing_;
  numOfAmbiguities_ -= additionalNumAmbig;
};



//
bool SgVlbiMeasurement::saveIntermediateResults(QDataStream& s) const
{
  s << getAttributes() << sigma2add_ << sigma2Apply_ << residual_ << residualNorm_ 
    << effFreq_ << effFreqEqWgt_ << q2_ << ionoValue_ << ionoSigma_ << numOfAmbiguities_;
  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiMeasurement::loadIntermediateResults(QDataStream& s)
{
  double                        sigma2add, sigma2Apply, residual, residualNorm;
  double                        effFreq, effFreqEqWgt, q2, ionoValue, ionoSigma;
  int                           numOfAmbiguities;
  unsigned int                  attributes;
  //
  s >> attributes >> sigma2add >> sigma2Apply >> residual >> residualNorm
    >> effFreq >> effFreqEqWgt >> q2 >> ionoValue >> ionoSigma >> numOfAmbiguities;
  if (s.status() == QDataStream::Ok)
  {
    sigma2add_        = sigma2add;
    sigma2Apply_      = sigma2Apply;
    residual_         = residual;
    residualNorm_     = residualNorm;
    effFreq_          = effFreq;
    effFreqEqWgt_     = effFreqEqWgt;
    q2_               = q2;
    ionoValue_        = ionoValue;
    ionoSigma_        = ionoSigma;
    numOfAmbiguities_ = numOfAmbiguities;
    setAttributes(attributes);
    return true;
  };
  logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() +
    ": loadIntermediateResults(): error reading data for " + name_ + ": " +
    (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
  return false;
};
/*=====================================================================================================*/




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


/*=====================================================================================================*/
//
// statics:
//
/*=====================================================================================================*/




/*=====================================================================================================*/
