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
#include <math.h>

#include <QtCore/QDataStream>

#include <SgVlbiSourceInfo.h>

#include <SgConstants.h>
#include <SgLogger.h>
//#include <SgParametersDescriptor.h>



/*=======================================================================================================
*
*                           METHODS (aux):
* 
*======================================================================================================*/
//
//
double SgVlbiSourceInfo::StructModel::tauS(double f, double u, double v) const
{
  double                        d=0.0;
  double                        r, c, s, a;
  
  r = u*x_ + v*y_;
 
  sincos(2.0*M_PI*r, &s, &c);
  a = k_*k_ + 2.0*k_*c + 1.0;
  
  d = k_*(1.0 - k_)*r*(1.0 - c)/f/(1.0 + k_)/a;
   
  return d;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
//
void SgVlbiSourceInfo::StructModelMp::prepareModel(double u, double v)
{
  dR_ = u*getX() + v*getY();
  sincos(2.0*M_PI*dR_, &dS_, &dC_);
  return;
};
/*=====================================================================================================*/





/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiSourceInfo::className()
{
  return "SgVlbiSourceInfo";
};



// A constructor:
SgVlbiSourceInfo::SgVlbiSourceInfo(int idx, const QString& key, const QString& aka) :
  SgObjectInfo(idx, key, aka),
  aprioriReference_("Unrefered"),
  aprioriComments_(""),
//  aPrioriStructModels_(),
  sModel_()
{
  RA_ = RA_ea_ = 0.0;
  DN_ = DN_ea_ = 0.0;
  al2ExtA_ = al2Estd_ = 0.0;
  pRA_ = NULL;
  pDN_ = NULL;

//  addAttr(Attr_ESTIMATE_COO);
  delAttr(Attr_ESTIMATE_COO);
  totalScanNum_ = 0;
  procdScanNum_ = 0;
  isSsModelEstimating_ = false;
};



//
SgVlbiSourceInfo::SgVlbiSourceInfo(const SgVlbiSourceInfo& src) :
  SgObjectInfo(src),
  aprioriReference_(src.getAprioriReference()),
  aprioriComments_(src.getAprioriComments()),
  sModel_(src.sModel_)
{
  totalScanNum_ = src.totalScanNum_;
  procdScanNum_ = src.procdScanNum_;
  RA_ = RA_;
  DN_ = DN_;
  RA_ea_ = RA_ea_;
  DN_ea_ = DN_ea_;
  al2ExtA_ = al2ExtA_;
  al2Estd_ = al2Estd_;

  if (src.pRA_)
    pRA_ = new SgParameter(*src.pRA_);
  else
    pRA_ = NULL;
  if (src.pDN_)
    pDN_ = new SgParameter(*src.pDN_);
  else
    pDN_ = NULL;

  isSsModelEstimating_ = src.isSsModelEstimating_;
};




//
double SgVlbiSourceInfo::arcLength(double ra_1, double dn_1, double ra_2, double dn_2)
{
  long double                   sdn1=0.0, cdn1=0.0, sdn2=0.0, cdn2=0.0;
  long double                   cdra=0.0;
  
  if (abs(ra_1-ra_2)<20.0/RAD2MAS && abs(dn_1-dn_2)<20.0/RAD2MAS) // almost linear 
    return sqrt((ra_1-ra_2)*(ra_1-ra_2)*cos(dn_1)*cos(dn_1) + (dn_1-dn_2)*(dn_1-dn_2));

  // calculate arc-length:
  sincosl(dn_1, &sdn1, &cdn1);
  sincosl(dn_2, &sdn2, &cdn2);
  cdra = cosl(ra_1 - ra_2);
  return acosl(sdn1*sdn2 + cdn1*cdn2*cdra);
};



//
QString SgVlbiSourceInfo::ra2String(double ra)
{
  QString     str;
  int         nHour, nMin;
  double      dSec, dTmp;

  dTmp = ra*RAD2MS;
  nHour = (int)(dTmp/3600.0/1000.0);
  nMin  = (int)((dTmp - 3600.0*1000.0*nHour)/60.0/1000.0);
  dSec  = (dTmp - 3600.0*1000.0*nHour - 60.0*1000.0*nMin)/1000.0;
  
  str.sprintf("%02d %02d %011.8f", nHour, nMin, dSec);

  return str;
};



//
QString SgVlbiSourceInfo::dn2String(double dn, bool mandatorySign)
{
  QString     str;
  int         nDeg, nMin;
  double      dSec, dTmp;
  char        cSign=mandatorySign?'+':' ';
  if (dn<0.0)
  {
    dn = -dn;
    cSign = '-';
  };

  dTmp = dn*RAD2MAS;
  nDeg = (int)(dTmp/3600/1000.0);
  nMin = (int)((dTmp - 3600.0*1000.0*nDeg)/60.0/1000.0);
  dSec = (dTmp - 3600.0*1000.0*nDeg - 60.0*1000.0*nMin)/1000.0;
  
  str.sprintf("%c%02d %02d %011.8f", cSign, nDeg, nMin, dSec);

  return str;
};



//
void SgVlbiSourceInfo::createParameters()
{
  QString prefix = "Src " + getKey().leftJustified(8, ' ') + ": ";
  releaseParameters();
  pRA_ = new SgParameter(prefix + "RA");
  pDN_ = new SgParameter(prefix + "DN");

  for (int i=0; i<sModel_.size(); i++)
  {
    sModel_[i].pK() = new SgParameter(prefix + "k_" + QString("").sprintf("%03d", i));
    sModel_[i].pB() = new SgParameter(prefix + "b_" + QString("").sprintf("%03d", i));
    sModel_[i].pX() = new SgParameter(prefix + "x_" + QString("").sprintf("%03d", i));
    sModel_[i].pY() = new SgParameter(prefix + "y_" + QString("").sprintf("%03d", i));
  };
};



//
void SgVlbiSourceInfo::releaseParameters()
{
  if (pRA_)
  {
    delete pRA_;
    pRA_ = NULL;
  };
  if (pDN_)
  {
    delete pDN_;
    pDN_ = NULL;
  };
  for (int i=0; i<sModel_.size(); i++)
  {
    if (sModel_[i].pK())
    {
      delete sModel_[i].pK();
      sModel_[i].pK() = NULL;
    };
    if (sModel_[i].pB())
    {
      delete sModel_[i].pB();
      sModel_[i].pB() = NULL;
    };
    if (sModel_[i].pX())
    {
      delete sModel_[i].pX();
      sModel_[i].pX() = NULL;
    };
    if (sModel_[i].pY())
    {
      delete sModel_[i].pY();
      sModel_[i].pY() = NULL;
    };
  };
};



//
bool SgVlbiSourceInfo::saveIntermediateResults(QDataStream& s) const
{
  SgObjectInfo::saveIntermediateResults(s);
  if (s.status() == QDataStream::Ok)
  {
    s << sModel_.size();
    for (int i=0; i<sModel_.size(); i++)
    {
      const StructModelMp      &m=sModel_.at(i);
      s << m.getK() << m.getB() << m.getX() << m.getY()
        << m.getEstimateRatio() << m.getEstimateSpIdx() << m.getEstimatePosition();
    };
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiSourceInfo::loadIntermediateResults(QDataStream& s)
{
  sModel_.clear();
  SgObjectInfo::loadIntermediateResults(s);
  if (s.status() == QDataStream::Ok)
  {
    int                         n;
    double                      x, y, k, b;
    bool                        eR, eK, eB;
    s >> n;
    for (int i=0; i<n; i++)
    {
      s >> k >> b >> x >> y
        >> eK >> eB >> eR;
      sModel_.append(StructModelMp(k, b, x, y, eK, eB, eR));
    };
  };
  return s.status()==QDataStream::Ok;
};



//
double SgVlbiSourceInfo::tauS(double f, double u, double v)
{
  double                        d=0.0;
  if (!sModel_.size())
    return d;
    
  double              dT1, dT2, dQ;
  double              s1, s2, s3, s4, s5;
  double              ss1, ss2, ss3;
  int                 n=sModel_.size();
  s1 = s2 = s3 = s4 = s5 = ss1 = ss2 = ss3 = 0.0;
      
  for (int i=0; i<n; i++)
  {
    sModel_[i].prepareModel(u, v);
    s1 += sModel_.at(i).getB()*sModel_.at(i).getK()*sModel_.at(i).getS();
    s2 += sModel_.at(i).getK()*sModel_.at(i).getR()*sModel_.at(i).getC();
    s3 += sModel_.at(i).getK()*sModel_.at(i).getK()*sModel_.at(i).getR();
    s4 += sModel_.at(i).getK()*sModel_.at(i).getK();
    s5 += sModel_.at(i).getK()*sModel_.at(i).getC();
  };
      
  for (int i=0; i<n-1; i++)
    for (int j=i+1; j<n; j++)
    {
      double                sPhi_ij, cPhi_ij;
  
      sincos(2.0*M_PI*(sModel_.at(i).getR() - sModel_.at(j).getR()),    &sPhi_ij, &cPhi_ij);

      ss1 += sModel_.at(i).getK()*sModel_.at(j).getK()*
            (sModel_.at(i).getB() - sModel_.at(j).getB())*sPhi_ij;
            
      ss2 += sModel_.at(i).getK()*sModel_.at(j).getK()*
            (sModel_.at(i).getR() + sModel_.at(j).getR())*cPhi_ij;
          
      ss3 += sModel_.at(i).getK()*sModel_.at(j).getK()*cPhi_ij;
    };

  dT1 = -1.0/(2.0*M_PI*f)*(s1 + ss1);
  dT2 = -1.0/f*(s2 + s3 + ss2);
  dQ  = 1.0 + s4 + 2.0*(s5 + ss3);
    
  d = (dT1 + dT2)/dQ;

  return d;
};



//
void SgVlbiSourceInfo::processPartials4SrcStructModel(QList<SgParameter*>& parameters,
  double f, double u, double v)
{
  if (!sModel_.size())
    return;

  double              dT1, dT2, dQ;
  double              s1, s2, s3, s4, s5;
  double              ss1, ss2, ss3;
  int                 n=sModel_.size();
  s1 = s2 = s3 = s4 = s5 = ss1 = ss2 = ss3 = 0.0;
  for (int i=0; i<n; i++)
  {
    sModel_[i].prepareModel(u, v);
    s1 += sModel_.at(i).getB()*sModel_.at(i).getK()*sModel_.at(i).getS();
    s2 += sModel_.at(i).getK()*sModel_.at(i).getR()*sModel_.at(i).getC();
    s3 += sModel_.at(i).getK()*sModel_.at(i).getK()*sModel_.at(i).getR();
    s4 += sModel_.at(i).getK()*sModel_.at(i).getK();
    s5 += sModel_.at(i).getK()*sModel_.at(i).getC();
  };
     
  for (int i=0; i<n-1; i++)
    for (int j=i+1; j<n; j++)
    {
      double                sPhi_ij, cPhi_ij;
      sincos(2.0*M_PI*(sModel_.at(i).getR() - sModel_.at(j).getR()),    &sPhi_ij, &cPhi_ij);
      ss1 += sModel_.at(i).getK()*sModel_.at(j).getK()*
            (sModel_.at(i).getB() - sModel_.at(j).getB())*sPhi_ij;
      ss2 += sModel_.at(i).getK()*sModel_.at(j).getK()*
            (sModel_.at(i).getR() + sModel_.at(j).getR())*cPhi_ij;
      ss3 += sModel_.at(i).getK()*sModel_.at(j).getK()*cPhi_ij;
      };

  dT1 = -1.0/(2.0*M_PI*f)*(s1 + ss1);
  dT2 = -1.0/f*(s2 + s3 + ss2);
  dQ  = 1.0 + s4 + 2.0*(s5 + ss3);
    
  for (int l=0; l<n; l++)
  {
    double                    dCl=sModel_.at(l).getC();
    double                    dSl=sModel_.at(l).getS();
    double                    dRl=sModel_.at(l).getR();
    double                    dKl=sModel_.at(l).getK();
    double                    dBl=sModel_.at(l).getB();
    double                    t1L, t1R, t2L, t2R, t3L, t3R, t4L, t4R, t5L, t5R, t6L, t6R;
    t1L = t1R = t2L = t2R = t3L = t3R = t4L = t4R = t5L = t5R = t6L = t6R = 0.0;

    double                    sPhi_il, cPhi_il;
    double                    sPhi_li, cPhi_li;
      
    // left part:
    for (int i=0; i<l; i++)
    {
      sincos(2.0*M_PI*(sModel_.at(i).getR() - sModel_.at(l).getR()),    &sPhi_il, &cPhi_il);
      sPhi_li =-sPhi_il;
      cPhi_li = cPhi_il;

      t1L = sModel_.at(i).getK()*sPhi_il;
      t2L = sModel_.at(i).getK()*(sModel_.at(i).getB() - sModel_.at(l).getB())*sPhi_il;
      t3L = sModel_.at(i).getK()*(sModel_.at(i).getR() + sModel_.at(l).getR())*cPhi_il;
      t4L = sModel_.at(i).getK()*cPhi_il;
      t5L = sModel_.at(i).getK()*(sModel_.at(i).getB() - sModel_.at(l).getB())*cPhi_il;
      t6L = sModel_.at(i).getK()*(sModel_.at(i).getR() + sModel_.at(l).getR())*sPhi_il;
    };
    // right part:
    for (int i=l+1; i<n; i++)
    {
      sincos(2.0*M_PI*(sModel_.at(i).getR() - sModel_.at(l).getR()),    &sPhi_il, &cPhi_il);
      sPhi_li =-sPhi_il;
      cPhi_li = cPhi_il;
        
      t1R = sModel_.at(i).getK()*sPhi_li;
      t2R = sModel_.at(i).getK()*(sModel_.at(l).getB() - sModel_.at(i).getB())*sPhi_li;
      t3R = sModel_.at(i).getK()*(sModel_.at(l).getR() + sModel_.at(i).getR())*cPhi_li;
      t4R = sModel_.at(i).getK()*cPhi_li;
      t5R = sModel_.at(i).getK()*(sModel_.at(l).getB() - sModel_.at(i).getB())*cPhi_li;
      t6R = sModel_.at(i).getK()*(sModel_.at(l).getR() + sModel_.at(i).getR())*sPhi_li;
    };
    double                    dTau1_dBl, dTau1_dKl, dTau2_dKl, dQ_dKl, dTau1_dRl, dTau2_dRl, dQ_dRl;

    dTau1_dBl = -dKl/(2.0*M_PI*f)*(dSl - t1L + t1R);
      
    dTau1_dKl = -1.0/(2.0*M_PI*f)*(dBl*dSl + t2L + t2R);
    dTau2_dKl = -1.0/f*(dRl*(dCl + 2.0*dKl) + t3L + t3R);
    dQ_dKl    = 2.0*(dKl + dCl + t4L + t4R);
      
    dTau1_dRl = -dKl/f*(dBl*dCl - t5L + t5R);
    dTau2_dRl = -dKl/f*(dCl + dKl - 2.0*M_PI*dRl*dSl + t4L + t4R + 2.0*M_PI*(t6L - t6R));
    dQ_dRl    = -4.0*M_PI*dKl*(dSl - t1L + t1R);

    double                    d;
    if (sModel_[l].pK()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      d = (dTau1_dKl + dTau2_dKl)/dQ - (dT1 + dT2)*dQ_dKl/dQ/dQ;
      sModel_[l].pK()->setD(d);
      parameters.append(sModel_[l].pK());
    };
    if (sModel_[l].pB()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      d = dTau1_dBl/dQ;
      sModel_[l].pB()->setD(d);
      parameters.append(sModel_[l].pB());
    };

    if (sModel_[l].pX()->isAttr(SgPartial::Attr_IS_IN_RUN))
    {
      d = (dTau1_dRl + dTau2_dRl)/dQ - (dT1 + dT2)*dQ_dRl/dQ/dQ;
      sModel_[l].pX()->setD(d*u);
      sModel_[l].pY()->setD(d*v);
      parameters.append(sModel_[l].pX());
      parameters.append(sModel_[l].pY());
    };
  };
 
};



//
short SgVlbiSourceInfo::calculateStatusBits()
{
  short                         bitArray;
  bitArray = 0;
  //
  //  ! *                                                                      *
  //  ! *   Format of SOURSTAT_I2 array:                                       *
  //  ! *          Array SOURSTAT_I2  contains NUMSTR elements, where NUMSTR   *
  //  ! *               is the number of sources in database.                  *
  //  ! *          Each element of the array is 16-bits bit field. I-th element*
  //  ! *               of SOURSTAT_I2 corresponds to the I-th element of the  *
  //  ! *               source name array kept in database under lcode         *
  //  ! *               STRNAMES. In general source order in this array        *
  //  ! *               MAY NOT coincide with order of sources in the list     *
  //  ! *               ISTRN_CHR kept in prfil.i  !!                          *
  //  ! *          Bits are counted from 1.                                    *
  //  ! *          1-st bit is set when the source was selected in solution.   *
  //  ! *          2-nd bit reserved for future use. It is always 1.           *
  //  ! *          3-rd bit is set if right ascension of the I-th source       *
  //  ! *                   was estimated in solution.                         *
  //  ! *          4-th bit is set if declination of the I-th source           *
  //  ! *                   was estimated in solution.                         *
  //
  // first bit:
  if (!isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
    bitArray |= (1<<0);
  // second bit:
  bitArray |= (1<<1);
  // third and fourth bits:
  if (isAttr(Attr_ESTIMATE_COO) && !isAttr(Attr_NOT_VALID))
  {
    bitArray |= (1<<2);
    bitArray |= (1<<3);
  };
  
  return bitArray;
};



//
//
void SgVlbiSourceInfo::applyStatusBits(short bitArray)
{
  // check for deselect flag:
  if (!(bitArray & (1<<0)))
    addAttr(Attr_NOT_VALID);

  // third and fourth bits:
  if (bitArray & (1<<2|1<<3)) // if one of bits is on
    addAttr(Attr_ESTIMATE_COO);
};



//
int SgVlbiSourceInfo::calcNumOfEstimatedSsmParameters() const
{
  int                           num=0, n=sModel_.size();
  for (int i=0; i<n; i++)
  {
    if (sModel_.at(i).getEstimateRatio())
      num++;
    if (sModel_.at(i).getEstimateSpIdx())
      num++;
    if (sModel_.at(i).getEstimatePosition())
      num += 2;
  };
  return num;
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
//
// constants:
//

/*=====================================================================================================*/
