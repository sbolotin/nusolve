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


#include <SgWrmsable.h>

#include <SgLogger.h>
#include <SgTaskConfig.h>
#include <SgVlbiObservable.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgWrmsable::className()
{
  return "SgWrmsable";
};
//



//
void SgWrmsable::addWrms(const SgMJD& t, double o_c, double w, double auxSum)
{
  sumW_     	+= w;
  sumWrms1_ 	+= w*o_c;
  sumWrmsWrms_+= w*o_c*w*o_c;
  sumWrms2_ 	+= w*o_c*o_c;
  sumAuxOne_	+= w*auxSum;
  sumAuxTwo_	+= w*w*auxSum;
  if (t < tFirst_)
    tFirst_ = t;
  if (tLast_ < t)
    tLast_ = t;
  numProcessed_++;
};



//
void SgWrmsable::copyStatistics(SgWrmsable* wrms)
{
  if (wrms)
  {
    setNumTotal(wrms->getNumTotal());
    setNumUsable(wrms->getNumUsable());
    setNumProcessed(wrms->getNumProcessed());
    sumW_         = wrms->sumW_;
    sumWrms1_     = wrms->sumWrms1_;
    sumWrmsWrms_  = wrms->sumWrmsWrms_;
    sumWrms2_     = wrms->sumWrms2_;
    sumAuxOne_    = wrms->sumAuxOne_;
    sumAuxTwo_    = wrms->sumAuxTwo_;
    normedResid_  = wrms->normedResid_;
    sff_NrmRes_   = wrms->sff_NrmRes_;
    sff_NrmRes_plus_15_ = wrms->sff_NrmRes_plus_15_;
    tFirst_       = wrms->tFirst();
    tLast_        = wrms->tLast();
  };
};



//
void SgWrmsable::recalcSigma2add(double minSigma)
{
  if (numProcessed_ <= 1)
    return; // no good observations
  //
  double                        q, newSigma;
  double                        d=1.0; // Chi^2 == d
  //
  q = (d*sumWrms2_ - (numProcessed_ - sumAuxOne_))/(sumW_ - sumAuxTwo_);
  if (q < 0.0)
    q = -sqrtl(-q);
  else
    q = sqrtl(q);
  //
  if (q > 0.0)
    newSigma = sqrt(sigma2add_*sigma2add_ + q*q);
  else if (sigma2add_*sigma2add_ <= q*q + minSigma*minSigma)
    newSigma = minSigma;
  //newSigma = 10.0e-12; // 10ps,  a la SOLVE
  else
    newSigma = sqrt(sigma2add_*sigma2add_ - q*q);
  //
  sigma2add_ = newSigma;

/*
if (dataType_ == DT_RATE)
{
  std::cout << "recalcSigma2add: "
    << " numTotal= " << numTotal_
    << " numUsable= " << numUsable_
    << " numProcessed= " << numProcessed_
    << " sumWrms2_ = " << sumWrms2_
    << " sumAuxOne_ = " << sumAuxOne_
    << " sumW_ = " << sumW_
    << " sumAuxTwo_ = " << sumAuxTwo_
    << " sigma2add_= " << sigma2add_
    << "\n";

};
*/

};



//
void SgWrmsable::calcNormalizedResiduals(const QList<SgVlbiObservable*> &observables, 
  const SgTaskConfig* cfg)
{
  bool                           isOpSolveCompatible=cfg->getOpIsSolveCompatible();
  double                         disp=dispersion(isOpSolveCompatible), s;
    
  for (int i=0; i<observables.size(); i++)
  {
    SgVlbiObservable           *o=observables.at(i);
    SgVlbiMeasurement          *m=(dataType_==DT_DELAY)?o->activeDelay():&o->phDRate();
    //
    if (m)
    {
      if (cfg->getOpHave2NormalizeResiduals())
      {
        s = m->sigma2Apply();
        if (isOpSolveCompatible)
// old (pre 0.5.1):
//->      m->setResidualNorm(m->getResidual()/s/s/sumW_/disp);
          m->setResidualNorm(m->getResidual()/s/disp);
        else
          m->setResidualNorm(numProcessed_*m->getResidual()/s/s/sumW_/disp);
      }
      else // scale by WRMS, so 3 == 3*sigma
      {
        s = std::max(wrms(), 1.0e-15); // 1 fs is min
        m->setResidualNorm(m->getResidual()/s);
      };
    };
  };
};



//
bool SgWrmsable::saveIntermediateResults(QDataStream& s) const
{
  s << numTotal_ << numUsable_ << numProcessed_ << sumW_ << sumWrms1_ << sumWrmsWrms_
    << sumWrms2_ << sumAuxOne_ << sumAuxTwo_ << sigma2add_ 
    << tFirst_.getDate() << tFirst_.getTime() << tLast_.getDate() << tLast_.getTime()
    << normedResid_ << sff_NrmRes_ << sff_NrmRes_plus_15_;

  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgWrmsable::loadIntermediateResults(QDataStream& s)
{
  int                           numTotal, numUsable, numProcessed;
  double                        sumW, sumWrms1, sumWrmsWrms, sumWrms2, sumAuxOne, sumAuxTwo, sigma2add;
  int                           tFirst_date, tLast_date;
  double                        tFirst_time, tLast_time;
  double                        normedResid, sff_NrmRes, sff_NrmRes_plus_15;

  s >> numTotal >> numUsable >> numProcessed >> sumW >> sumWrms1 >> sumWrmsWrms
    >> sumWrms2 >> sumAuxOne >> sumAuxTwo >> sigma2add 
    >> tFirst_date >> tFirst_time >> tLast_date >> tLast_time
    >> normedResid >> sff_NrmRes >> sff_NrmRes_plus_15;

  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  numTotal_     = numTotal;
  numUsable_    = numUsable;
  numProcessed_ = numProcessed;
  sumW_         = sumW;
  sumWrms1_     = sumWrms1;
  sumWrmsWrms_  = sumWrmsWrms;
  sumWrms2_     = sumWrms2;
  sumAuxOne_    = sumAuxOne;
  sumAuxTwo_    = sumAuxTwo;
  sigma2add_    = sigma2add;
  tFirst_.setDate(tFirst_date);
  tFirst_.setTime(tFirst_time);
  tLast_ .setDate(tLast_date);
  tLast_ .setTime(tLast_time);
  normedResid_  = normedResid;
  sff_NrmRes_   = sff_NrmRes;
  sff_NrmRes_plus_15_ = sff_NrmRes_plus_15;
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
