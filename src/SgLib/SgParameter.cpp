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


#include <SgParameter.h>
#include <SgLogger.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgParameter::className()
{
  return "SgParameter";
};



//
SgParameter& SgParameter::operator=(const SgParameter& p)
{
  SgPartial::operator=(p);
  value_ = p.getValue();
  solution_ = p.getSolution();
  sigma_ = p.getSigma();
  scale_ = p.getScale();
  pMode_ = p.getPMode();
  sType_ = p.getSType();
  sigmaAPriori_ = p.getSigmaAPriori();
  sigmaAPrioriAux_ = p.getSigmaAPrioriAux();
  tau_ = p.getTau();
  psd_RWN_ = p.getPsd_RWN();
  psd_BN_  = p.getPsd_BN();
  prefix_ = p.getPrefix();
  step_ = p.getStep();
  ttl_ = p.getTTL();
  numOfPolynomials_ = p.getNumOfPolynomials();
  tLeft_ = p.getTLeft();
  tRight_ = p.getTRight();
  idx_ = p.getIdx();
  return *this;
};



//
void SgParameter::tune(const SgParameterCfg& cfg)
{
  scale_ = cfg.getScale();
  switch (cfg.getPMode())
  {
    case SgParameterCfg::PM_STC:
      sigmaAPriori_    = cfg.getStocAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getStocAPriori()/scale_;
    break;
    case SgParameterCfg::PM_PWL:
      step_ = cfg.getPwlStep();
//      sigmaAPriori_    = cfg.getPwlAPriori()*step_/scale_;
      sigmaAPriori_    = cfg.getPwlAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
      numOfPolynomials_= cfg.getPwlNumOfPolynomials();
    break;
    case SgParameterCfg::PM_ARC:
      step_ = cfg.getArcStep();
      ttl_  = step_;
      sigmaAPriori_    = cfg.getConvAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
    break;
    default:
      step_ = cfg.getArcStep();
      sigmaAPriori_    = cfg.getConvAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
    break;
  };
  //
  tau_ = cfg.getTau();
  psd_RWN_ = cfg.getWhiteNoise()/scale_;
  psd_BN_ = cfg.getBreakNoise()/scale_;
  pMode_ = cfg.getPMode();
  sType_ = cfg.getSType();
  //
  if (cfg.isAttr(SgParameterCfg::Attr_IS_PROPAGATED))
    addAttr(Attr_IS_PROPAGATED);
  else 
    delAttr(Attr_IS_PROPAGATED);
};



//
void SgParameter::tune(SgParameterCfg::PMode pMode, const SgParameterCfg& cfg)
{
  scale_ = cfg.getScale();
  switch ((pMode_=pMode))
  {
    case SgParameterCfg::PM_STC:
      sigmaAPriori_    = cfg.getStocAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getStocAPriori()/scale_;
    break;
    case SgParameterCfg::PM_PWL:
      step_ = cfg.getPwlStep();
//      sigmaAPriori_    = cfg.getPwlAPriori()*step_/scale_;
      sigmaAPriori_    = cfg.getPwlAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
      numOfPolynomials_= cfg.getPwlNumOfPolynomials();
    break;
    case SgParameterCfg::PM_ARC:
      step_ = cfg.getArcStep();
      ttl_  = step_;
      sigmaAPriori_    = cfg.getConvAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
    break;
    default:
      step_ = cfg.getArcStep();
      sigmaAPriori_    = cfg.getConvAPriori()/scale_;
      sigmaAPrioriAux_ = cfg.getConvAPriori()/scale_;
    break;
  };

  tau_ = cfg.getTau();
  psd_RWN_ = cfg.getWhiteNoise()/scale_;
  psd_BN_ = cfg.getBreakNoise()/scale_;
  sType_ = cfg.getSType();
  //
  if (cfg.isAttr(SgParameterCfg::Attr_IS_PROPAGATED))
    addAttr(Attr_IS_PROPAGATED);
  else 
    delAttr(Attr_IS_PROPAGATED);
};



//
void SgParameter::tune(const SgParameter& p)
{
  scale_ = p.getScale();
  sigmaAPriori_    = p.getSigmaAPriori();
  sigmaAPrioriAux_ = p.getSigmaAPrioriAux();
  tau_ = p.getTau();
  psd_RWN_ = p.getPsd_RWN();
  psd_BN_ = p.getPsd_BN();
  pMode_ = p.getPMode();
  sType_ = p.getSType();
  step_ = p.getStep();
  prefix_ = p.getPrefix();
  ttl_ = p.getTTL();
  numOfPolynomials_ = p.getNumOfPolynomials();
};



//
void SgParameter::update(SgParameter* p)
{
  if (p)
  {
    value_ += p->getSolution();
    solution_ = p->getSolution();
    sigma_ = p->getSigma();
  }
  else
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() +
        ": update(): cannot update from NULL");
};



//
double SgParameter::calcM(double dT)
{
  double      d=0.0;
  switch (sType_)
  {
    case SgParameterCfg::ST_WHITENOISE:
      d = 0.0;
      break;
    case SgParameterCfg::ST_MARKOVPROCESS:
      d = exp( -24.0*dT/tau_ ); // Tau in hours
      break;
    default:
    case SgParameterCfg::ST_RANDWALK:
      //d = isAttr(Attr_IS_BREAK) ? 0.0 : 1.0;
      d = 1.0;
      break;
  };
  return d;
};



//
double SgParameter::calcRW(double dT)
{
  double      psd = isAttr(Attr_IS_BREAK) ? psd_BN_ : psd_RWN_;
  double      e=0.0, m=0.0;
  switch (sType_)
  {
    case SgParameterCfg::ST_WHITENOISE:
      e = -1.0;
      break;
    case SgParameterCfg::ST_MARKOVPROCESS:
      m = calcM(dT);
      e = tau_/2.0*(1.0 - m*m)*psd*psd; // Tau in hours
      break;
    default:
    case SgParameterCfg::ST_RANDWALK:
      e = 24.0*dT*psd*psd;
      //      e = 24.0*dT*24.0*dT*psd*psd/2.0;  ???IRW???
      break;
  };

  if (isAttr(Attr_IS_BREAK))
  {
    logger->write(SgLogger::INF, SgLogger::ESTIMATOR, className() + 
      ": a Break Flag detected for the parameter [" + getName() + "]; processed");
    delAttr(Attr_IS_BREAK);
  };
  return e>0.0 ? 1.0/sqrt(e) : sigmaAPriori_;
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
bool lessThan4_ParameterByNameSortingOrder(SgParameter* p1, SgParameter* p2)
{
  return p1->getName() < p2->getName();
};



//
bool lessThan4_ParameterByTTLSortingOrder(SgParameter* p1, SgParameter* p2)
{
  return p1->getTTL()==p2->getTTL() ? p1->getName()<p2->getName() : p1->getTTL()<p2->getTTL();
};



//
bool lessThan4_ParameterByTLeftSortingOrder(SgParameter* p1, SgParameter* p2)
{
  return p1->getTLeft()==p2->getTLeft() ?
    (p1->getTRight()==p2->getTRight() ? p1->getName()<p2->getName() : p1->getTRight()<p2->getTRight()) :
    p1->getTLeft()<p2->getTLeft();
};



//
bool lessThan4_ParameterByTRightSortingOrder(SgParameter* p1, SgParameter* p2)
{
  return p1->getTRight()==p2->getTRight() ?
    (p1->getTLeft()==p2->getTLeft() ? p1->getName()<p2->getName() : p1->getTLeft()>p2->getTLeft()) :
    p1->getTRight()<p2->getTRight();
};



//
void updateSolutionAtParameterList(QList<SgParameter*>& list, SgVector* x, SgSymMatrix* P)
{
  if (!x)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR,
      "updateSolutionAtParameterList(): the vector x is NULL");
    return;
  };
  if (!x->n())
  {
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR,
      "updateSolutionAtParameterList(): the dimension of vector x is zero");
    return;
  };
  if (!P)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR,
      "updateSolutionAtParameterList(): the matrix P is NULL");
    return;
  };
  if (!P->n())
  {
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR,
      "updateSolutionAtParameterList(): the dimension of matrix P is zero");
    return;
  };
  if ((int)x->n() != list.size())
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, "updateSolutionAtParameterList(): "
      "the number of parameters and the dimension of vector x are different; solution is ignored");
    return;
  };
  if ((int)P->n() != list.size())
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, "updateSolutionAtParameterList(): "
      "the number of parameters and the dimension of matrix P are different; solution is ignored");
    return;
  };

  for (int i=0; i<list.count(); i++)
  {
    SgParameter* p = list.at(i);
    p->setSolution(x->getElement(i));
    p->setSigma(sqrt(P->getElement(i, i)));
  };
};



//
void reportParameterList(const QList<SgParameter*>& list, bool isShowTLeftRight)
{
  if (!logger->isEligible(SgLogger::DBG, SgLogger::ESTIMATOR))
    return;
    
  QString str;
  for (int i=0; i<list.count(); i++)
  {
    SgParameter* p = list.at(i);
    /*
    str.sprintf(": %20.8f/%20.8f (%.8f) @ %8d pnt ",
                p->getValue()*p->getScale(), p->getSolution()*p->getScale(),
                p->getSigma()*p->getScale(), p->getNumObs());
                */
    str.sprintf(": %15.4f (%.4f) @ %6d pnt ",
                p->getSolution()*p->getScale(), p->getSigma()*p->getScale(), p->getNumObs());
    if (isShowTLeftRight)
      logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, p->getName() + str + "[" +
                  p->getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " : " +
                  p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + "]");
    else
      logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, p->getName() + str + "[" +
                  p->getTStart().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " : " +
                  p->getTFinis().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + "] (" +
                  p->getTMean().toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ")");
  };
  if (list.count())
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, "+++ end of report +++");
};



//
/*
void clearStatisticsForParameters(QList<SgParameter*>& list)
{
  for (int i=0; i<list.count(); i++)
    list.at(i)->clearStatistics();
};
*/


// i/o:


/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
