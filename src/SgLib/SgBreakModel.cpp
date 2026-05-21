/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2013-2020 Sergei Bolotin.
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


#include <SgBreakModel.h>

#include <SgLogger.h>
#include <SgParameter.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgParameterBreak::className()
{
  return "SgParameterBreak";
};



//
SgParameterBreak::SgParameterBreak(const SgParameterBreak& b) : 
  SgMJD(b), 
  SgAttribute(b), 
  epoch4Export_(b.epoch4Export_)
{
  setA0(b.getA0());
  setA1(b.getA1());
  setA2(b.getA2());
  setS0(b.getS0());
  setS1(b.getS1());
  setS2(b.getS2());
  pA0_ = pA1_ = pA2_ = NULL;
  if (b.pA0())
    pA0_ = new SgParameter(*b.pA0());
  if (b.pA1())
    pA1_ = new SgParameter(*b.pA1());
  if (b.pA2())
    pA2_ = new SgParameter(*b.pA2());
};



//
SgParameterBreak::SgParameterBreak(const SgMJD& t, double a0, double a1, double a2, bool isDynamic) : 
  SgMJD(t), 
  SgAttribute(), 
  epoch4Export_(t)
{
  setA0(a0);
  setA1(a1);
  setA2(a2);
  setS0(0.0);
  setS1(0.0);
  setS2(0.0);
  if (isDynamic)
    addAttr(Attr_DYNAMIC);
  pA0_ = pA1_ = pA2_ = NULL;
};



// A destructor:
SgParameterBreak::~SgParameterBreak()
{
  if (pA0_)
  {
    delete pA0_;
    pA0_ = NULL;
  };
  if (pA1_)
  {
    delete pA1_;
    pA1_ = NULL;
  };
  if (pA2_)
  {
    delete pA2_;
    pA2_ = NULL;
  };
};



//
SgParameterBreak& SgParameterBreak::operator=(const SgParameterBreak& b)
{
  *(SgMJD*)this = b;
  *(SgAttribute*)this = b;
  setA0(b.getA0());
  setA1(b.getA1());
  setA2(b.getA2());
  setS0(b.getS0());
  setS1(b.getS1());
  setS2(b.getS2());
  setEpoch4Export(b.getEpoch4Export());
  if (pA0_)
  {
    delete pA0_;
    pA0_ = NULL;
  };
  if (pA1_)
  {
    delete pA1_;
    pA1_ = NULL;
  };
  if (pA2_)
  {
    delete pA2_;
    pA2_ = NULL;
  };
  if (b.pA0())
    pA0_ = new SgParameter(*b.pA0());
  if (b.pA1())
    pA1_ = new SgParameter(*b.pA1());
  if (b.pA2())
    pA2_ = new SgParameter(*b.pA2());
  return *this;
};



//
void SgParameterBreak::createParameters(const QString& prefix)
{
  pA0_ = new SgParameter(prefix + "_0");
  pA1_ = new SgParameter(prefix + "_1");
  pA2_ = new SgParameter(prefix + "_2");
};



//
void SgParameterBreak::releaseParameters()
{
  if (pA0_)
  {
    delete pA0_;
    pA0_ = NULL;
  };
  if (pA1_)
  {
    delete pA1_;
    pA1_ = NULL;
  };
  if (pA2_)
  {
    delete pA2_;
    pA2_ = NULL;
  };
};



//
void SgParameterBreak::propagatePartials(QList<SgParameter*>& parameters, 
  const SgMJD& t, double dT, double sign)
{
  if (pA0_)
  {
    if (t <= *this)
      pA0_->setD(0.0);
    else
      pA0_->setD(sign);
    if (pA0_->isAttr(SgPartial::Attr_IS_IN_RUN))
      parameters.append(pA0_);
  };
  if (pA1_)
  {
    if (t <= *this)
      pA1_->setD(0.0);
    else
      pA1_->setD(sign*dT);
    if (pA1_->isAttr(SgPartial::Attr_IS_IN_RUN))
      parameters.append(pA1_);
  };
  if (pA2_)
  {
    if (t <= *this)
      pA2_->setD(0.0);
    else
      pA2_->setD(sign*dT*dT);
    if (pA2_->isAttr(SgPartial::Attr_IS_IN_RUN))
      parameters.append(pA2_);
  };
};



//
void SgParameterBreak::propagatePartials4rates(QList<SgParameter*>& parameters,
  const SgMJD& t, double dT, double sign)
{
  if (pA1_)
  {
    if (t <= *this)
      pA1_->setD(0.0);
    else
      pA1_->setD(sign);
    if (pA1_->isAttr(SgPartial::Attr_IS_IN_RUN))
      parameters.append(pA1_);
  };
  if (pA2_)
  {
    if (t <= *this)
      pA2_->setD(0.0);
    else
      pA2_->setD(2.0*sign*dT);
    if (pA2_->isAttr(SgPartial::Attr_IS_IN_RUN))
      parameters.append(pA2_);
  };
};



//
bool SgParameterBreak::saveIntermediateResults(QDataStream& s) const
{
  s << getAttributes() << a0_ << a1_ << a2_ << s0_ << s1_ << s2_;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  if (!SgMJD::saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data for the epoch");
    return false;
  };
  if (!epoch4Export_.saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data for the export epoch");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgParameterBreak::loadIntermediateResults(QDataStream& s)
{
  double                        a0, a1, a2;
  double                        s0, s1, s2;
  unsigned int                  attributes;
  s >> attributes >> a0 >> a1 >> a2 >> s0 >> s1 >> s2;
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (!SgMJD::loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data for the epoch: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  if (!epoch4Export_.loadIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error writting data for the export epoch");
    return false;
  };
  setAttributes(attributes);
  a0_ = a0;
  a1_ = a1;
  a2_ = a2;
  s0_ = s0;
  s1_ = s1;
  s2_ = s2;
  return true;
};
/*=====================================================================================================*/





bool mjdOrderLessThan(const SgParameterBreak*, const SgParameterBreak*);
/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgBreakModel::className()
{
  return "SgBreakModel";
};



//
SgBreakModel& SgBreakModel::operator=(const SgBreakModel& m)
{
  // first, remove all existed breaks:
  for (int i=0; i<size(); i++)
    delete at(i);
  clear();

  // then, make a copy from one list to *this:
  for (int i=0; i<m.size(); i++)
    append(new SgParameterBreak(*m.at(i)));

  // at last, adjust value of t0:
  setT0(m.getT0());
  return *this;
};



//
double SgBreakModel::value(const SgMJD& t) const
{
  double                        v=0.0;
  QList<SgParameterBreak*>::const_iterator 
                                it=constBegin();
  for (int i=0; i<size(); i++)
    v += it[i]->value(t, t-t0_);
  return v;
};



//
double SgBreakModel::rate(const SgMJD& t) const
{
  double                        r=0.0;
  QList<SgParameterBreak*>::const_iterator 
                                it=constBegin();
  for (int i=0; i<size(); i++)
    r += it[i]->rate(t, t-t0_);
  return r;
};



//
bool SgBreakModel::addBreak(const SgMJD& t, double a0, double a1, double a2, bool isDynamic)
{
  QList<SgParameterBreak*>::iterator it=begin();
  for (int i=0; i<size(); i++)
    if (*it[i]==t)
      return false;

  append(new SgParameterBreak(t, a0, a1, a2, isDynamic));
  return true;
};



//
bool SgBreakModel::addBreak(SgParameterBreak* aBreak)
{
  QList<SgParameterBreak*>::iterator it=begin();
  for (int i=0; i<size(); i++)
    if (*it[i]==*((SgMJD*)aBreak))
      return false;
  
  append(aBreak);
  return true;
};



//
bool SgBreakModel::delBreak(const SgMJD& t)
{
  SgParameterBreak             *aBreak=NULL;
  int                           idx2Del=-1;
  bool                          isOK=false;
  QList<SgParameterBreak*>::iterator 
                                it=begin();
  for (int i=0; i<size(); i++)
    if (fabs(*it[i]-t) < 0.8/DAY2SEC)
      idx2Del = i;
  if (idx2Del > -1)
  {
    aBreak = takeAt(idx2Del);
    if (aBreak)
    {
      isOK = true;
      delete aBreak;
    };
  }
  else
    return false;
  return isOK;
};



//
bool SgBreakModel::delBreak(int n)
{
  SgParameterBreak   *aBreak=NULL;
  bool                isOK=false;
  if (n>-1 && n<size())
  {
    aBreak = takeAt(n);
    if (aBreak)
    {
      isOK = true;
      delete aBreak;
    };
  }
  else
    return false;
  return isOK;
};



//
void SgBreakModel::createParameters(const QString& prefix)
{
  for (int i=0; i<size(); i++)
    at(i)->createParameters(prefix + QString("").sprintf("#%02d", i));
};



//
void SgBreakModel::releaseParameters()
{
  for (int i=0; i<size(); i++)
    at(i)->releaseParameters();
};



//
void SgBreakModel::propagatePartials(QList<SgParameter*>& parameters, 
  const SgMJD& t, double tau, double sign)
{
  for (int i=0; i<size(); i++)
    at(i)->propagatePartials(parameters, t, tau, sign);
};



//
void SgBreakModel::propagatePartials4rates(QList<SgParameter*>& parameters, 
  const SgMJD& t, double tau, double sign)
{
  for (int i=0; i<size(); i++)
    at(i)->propagatePartials4rates(parameters, t, tau, sign);
};



//
bool SgBreakModel::saveIntermediateResults(QDataStream& s) const
{
  int                           n(size());
  bool                          b(false);
  s << n << b;
  for (int i=0; i<n; i++)
  {
    at(i)->saveIntermediateResults(s);
    if (s.status() != QDataStream::Ok)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error writting data: idx#" + QString("").setNum(i));
      return false;
    };
  };
  return s.status()==QDataStream::Ok;
};



//
bool SgBreakModel::loadIntermediateResults(QDataStream& s)
{
  int                           n;
  bool                          b;
  s >> n >> b;
  //
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": loadIntermediateResults(): error reading data (num): " + 
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  clear();
  for (int i=0; i<n; i++)
  {
    if (s.status() == QDataStream::Ok)
    {
      SgParameterBreak         *b=new SgParameterBreak;
      b->loadIntermediateResults(s);
      addBreak(b);
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error reading data (idx#" + QString("").setNum(i) + "): " + 
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  };
  if (s.status() == QDataStream::Ok)
  {
    return true;
  };
  //
  logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
    ": loadIntermediateResults(): error reading data (@end): " +
    (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
  return false;
};



//
void SgBreakModel::sortEvents()
{
  qSort(begin(), end(), mjdOrderLessThan);
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
bool mjdOrderLessThan(const SgParameterBreak *pb1, const SgParameterBreak *pb2)
{
  return *pb1 < *pb2;
};



/*=====================================================================================================*/
//
// constants:
//

/*=====================================================================================================*/
