
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

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QMap>



#include <SgSolutionReporter.h>


#include <Sg3dMatrixR.h>
#include <SgArcStorage.h>
#include <SgConstants.h>
#include <SgEstimator.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgMathSupport.h>
#include <SgPwlStorage.h>
#include <SgPwlStorageBSplineL.h>
#include <SgPwlStorageBSplineQ.h>
#include <SgPwlStorageIncRates.h>
#include <SgTaskManager.h>
#include <SgVersion.h>
#include <SgVlbiBand.h>
#include <SgVlbiNetworkId.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>



bool rightAscensionSortingOrderLessThan(SgVlbiSourceInfo*, SgVlbiSourceInfo*);

void calcLhv(const Sg3dVector& r1, const Sg3dVector& dr1, const Sg3dVector& r2, const Sg3dVector& dr2,
  const SgMatrix& r, Sg3dVector& lhv, Sg3dVector& lhvSigma, double& length, double& lengthSigma);
void calcLhv(const Sg3dVector& r1, const Sg3dVector& r2, const Sg3dVector& db,
  const SgMatrix& r, Sg3dVector& lhv, Sg3dVector& lhvSigma, double& length, double& lengthSigma);



static const QString            srcChars[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
 "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
static const int                numOfSrcChars = sizeof(srcChars)/sizeof(QString);



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgSolutionReporter::className()
{
  return "SgSolutionReporter";
};



//
SgSolutionReporter::SgSolutionReporter(SgVlbiSession* session, const SgIdentities& id) :
  activeBandKey_(""),
  identities_(id),
  runEpoch_(tZero),
  creationEpoch_(tZero),
  reportID_("?"),
  solutionTypeName_("N/A"),
  path2APrioriFiles_("NONE"),
  stcSolutions_(),
  parByName_(),
  stcParByName_(),
  pwlByName_(),
  arcByName_(),
  usedSources_(),
  skippedSources_(),
  userComents_(),
  erpTref_(tZero)
{
  setSession(session);
  activeBand_ = NULL;
  PxAll_ = NULL;
  condNumber_ = 0.0;
  erp_ut1_0_ = erp_ut1_1_ = erp_ut1_2_ = erp_ut1_3_ = 0.0;
  erp_pmx_0_ = erp_pmx_1_ = erp_pmx_2_ = erp_pmx_3_ = 0.0;
  erp_pmy_0_ = erp_pmy_1_ = erp_pmy_2_ = erp_pmy_3_ = 0.0;
  eop_cix_0_ = eop_cix_1_ = eop_cix_2_ = eop_cix_3_ = 0.0;
  eop_ciy_0_ = eop_ciy_1_ = eop_ciy_2_ = eop_ciy_3_ = 0.0;

  dUt1Value_ = dUt1Correction_ = dUt1StdDev_ = 0.0;
};



// A destructor:
SgSolutionReporter::~SgSolutionReporter()
{
  session_ = NULL;
  config_ = NULL;
  parametersDescriptor_ = NULL;
  activeBand_ = NULL;
  if (PxAll_)
  {
    delete PxAll_;
    PxAll_ = NULL;
  };
  numOfUnknowns_ = 0;
  numOfObservations_ = 0;
  numOfConstraints_ = 0;
  freeResources();
  usedSources_.clear();
  skippedSources_.clear();
  userComents_.clear();
};



//
void SgSolutionReporter::setSession(SgVlbiSession* session)
{
  if ((session_=session))
  {
    config_ = session_->getConfig();
    parametersDescriptor_ = session_->getParametersDescriptor();
    session_->setReporter(this);
  }
  else
  {
    config_ = NULL;
    parametersDescriptor_ = NULL;
  };
};



//
void SgSolutionReporter::freeResources()
{
  if (PxAll_)
  {
    delete PxAll_;
    PxAll_ = NULL;
  };
  
  parByName_.clear();
  pwlByName_.clear();
  arcByName_.clear();
  stcParByName_.clear();

  for (int i=0; i<parList_.size(); i++)
    delete parList_.at(i);
  parList_.clear();

  for (int i=0; i<allParList_.size(); i++)
    delete allParList_.at(i);
  allParList_.clear();

  for (int i=0; i<pwlList_.size(); i++)
    delete pwlList_.at(i);
  pwlList_.clear();

  for (int i=0; i<arcList_.size(); i++)
    delete arcList_.at(i);
  arcList_.clear();
  
  for (QMap<QString, SgEstimator::StochasticSolutionCarrier>::iterator it=stcSolutions_.begin();
    it!=stcSolutions_.end(); ++it)
  {
    SgEstimator::StochasticSolutionCarrier 
                               &stcSC=it.value();
    for (int i=0; i<stcSC.list_->size(); i++)
      delete stcSC.list_->at(i);
    stcSC.list_->clear();
    delete stcSC.list_;
    delete stcSC.P_;
    delete stcSC.x_;
    stcSC.list_ = NULL;
    stcSC.x_ = NULL;
    stcSC.P_ = NULL;
  };
  stcSolutions_.clear();
};



//
void SgSolutionReporter::absorbInfo(SgTaskManager* mgr)
{
  bool                          useDelays=config_->getUseDelayType()!=SgTaskConfig::VD_NONE;
  bool                          useRates=config_->getUseRateType()!=SgTaskConfig::VR_NONE;
  QString                       str("");
  // later, add deleting of objects here
  freeResources();

  numOfUnknowns_ = 0;
  numOfObservations_ = 0;
  numOfConstraints_ = 0;
  condNumber_ = 0.0;

  activeBand_ = session_->bands().at(config_->getActiveBandIdx());
  activeBandKey_ = activeBand_->getKey();

  //  "Single band X-band";
  //  "Group delay X-band";
  switch (config_->getUseDelayType())
  {
  case SgTaskConfig::VD_NONE:
    solutionTypeName_ =  "No data ";
    break;
  case SgTaskConfig::VD_SB_DELAY:
    solutionTypeName_ =  "Single band ";
    break;
  case SgTaskConfig::VD_GRP_DELAY:
    solutionTypeName_ =  "Group delay ";
    break;
  case SgTaskConfig::VD_PHS_DELAY:
    solutionTypeName_ =  "Phase delay ";
    break;
  };
  if (useDelays && useRates)
    solutionTypeName_ += "and delay rates ";
  else if (useRates)
    solutionTypeName_ += "Delay rates ";
  //
  if (session_->isAttr(SgVlbiSession::Attr_HAS_IONO_CORR))
  {
    str = "";
    for (int i=0; i<session_->bands().size(); i++)
      str += session_->bands().at(i)->getKey() + "/";
    str.chop(1);
    solutionTypeName_ += str + " combination";
  }
  else
    solutionTypeName_ += activeBandKey_ + "-band";

  // evaluate normalized residuals per object:
  // baselines:
  QMap<QString, SgVlbiBaselineInfo*>::iterator itb;
  for (itb=activeBand_->baselinesByName().begin(); itb!=activeBand_->baselinesByName().end(); ++itb)
  {
    double                      sum1(0.0), sum2(0.0), sumNRD(0.0), sumNRD_plus_15(0.0);
    double                      ro, sig4NRD2, sig4NRD2_plus_15;
    int                         num(0);
    SgVlbiBaselineInfo         *bi=itb.value();
    for (int i=0; i<bi->observables().size(); i++)
    {
      SgVlbiObservable         *o=bi->observables().at(i);
      SgVlbiMeasurement        *m=o->activeDelay();
      if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        ro = m->getResidual();
        sig4NRD2 = m->sigma2Apply()*m->sigma2Apply();
        sig4NRD2_plus_15 = sig4NRD2 - 
          m->getSigma2add()*m->getSigma2add() +
          15.0*15.0*1.0e-24;
        sumNRD += ro*ro/sig4NRD2;
        sumNRD_plus_15 += ro*ro/sig4NRD2_plus_15;
        sum1 += m->getResidualNorm();
        sum2 += m->getResidualNorm()*m->getResidualNorm();
        num++;
      };
    };
    if (num > 1)
    {
      bi->setNormedResid(DT_DELAY, sqrt((sum2 - sum1*sum1/num)/num));
      bi->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD/num));
      bi->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15/num));
    }
    else if (num == 1)
    {
      bi->setNormedResid(DT_DELAY, 1.0);
      bi->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD));
      bi->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15));
    };
  };
  // sources and stations (they have not their own lists of observations [yet]):
  QMap< QString, QList<SgVlbiObservation*> >    obsBySrcNames;
  QMap< QString, QList<SgVlbiObservation*> >    obsByStnNames;
  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    SgVlbiObservable           *o=obs->activeObs();
    if (o && o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      obsBySrcNames[o->src()->getKey()]   << obs;
      obsByStnNames[o->stn_1()->getKey()] << obs;
      obsByStnNames[o->stn_2()->getKey()] << obs;
    };
  };
  QMap< QString, QList<SgVlbiObservation*> >::iterator  its;
  for (its=obsBySrcNames.begin(); its!=obsBySrcNames.end(); ++its)
  {
    SgVlbiSourceInfo           *si=activeBand_->sourcesByName().find(its.key()).value();
    QList<SgVlbiObservation*>  &obsList=its.value();
    double      sum1(0.0), sum2(0.0), sumNRD(0.0), sumNRD_plus_15(0.0), ro, sig4NRD2, sig4NRD2_plus_15;
    int         num(0);
    for (int i=0; i<obsList.size(); i++)
    {
      SgVlbiObservation        *obs=obsList.at(i);
      if (obs->activeObs() && obs->activeObs()->activeDelay())
      {
        SgVlbiMeasurement      *m=obs->activeObs()->activeDelay();
        ro = m->getResidual();
        sig4NRD2 = m->sigma2Apply()*m->sigma2Apply();
        sig4NRD2_plus_15 = sig4NRD2 - m->getSigma2add()*m->getSigma2add() + 15.0*15.0*1.0e-24;
        sumNRD += ro*ro/sig4NRD2;
        sumNRD_plus_15 += ro*ro/sig4NRD2_plus_15;
        sum1 += obs->getNormalizedDelayResidual();
        sum2 += obs->getNormalizedDelayResidual()*obs->getNormalizedDelayResidual();
        num++;
      };
    };
    if (num > 1)
    {
      si->setNormedResid(DT_DELAY, sqrt((sum2 - sum1*sum1/num)/num));
      si->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD/num));
      si->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15/num));
    }
    else if (num == 1)
    {
      si->setNormedResid(DT_DELAY, 1.0);
      si->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD));
      si->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15));
    }
    else
    {
      si->setNormedResid(DT_DELAY, 0.0);
      si->setSFF_NrmRes(DT_DELAY, 0.0);
      si->setSFF_NrmRes_plus_15(DT_DELAY, 0.0);
    };
  };
  for (its=obsByStnNames.begin(); its!=obsByStnNames.end(); ++its)
  {
    SgVlbiStationInfo          *si=activeBand_->stationsByName().find(its.key()).value();
    QList<SgVlbiObservation*>  &obsList=its.value();
    double      sum1(0.0), sum2(0.0), sumNRD(0.0), sumNRD_plus_15(0.0), ro, sig4NRD2, sig4NRD2_plus_15;
    int         num(0);
    for (int i=0; i<obsList.size(); i++)
    {
      SgVlbiObservation        *obs=obsList.at(i);
      if (obs->activeObs() && obs->activeObs()->activeDelay())
      {
        SgVlbiMeasurement      *m=obs->activeObs()->activeDelay();
        ro = m->getResidual();
        sig4NRD2 = m->sigma2Apply()*m->sigma2Apply();
        sig4NRD2_plus_15 = sig4NRD2 - m->getSigma2add()*m->getSigma2add() + 15.0*15.0*1.0e-24;
        sumNRD += ro*ro/sig4NRD2;
        sumNRD_plus_15 += ro*ro/sig4NRD2_plus_15;

        sum1 += obs->getNormalizedDelayResidual();
        sum2 += obs->getNormalizedDelayResidual()*obs->getNormalizedDelayResidual();
        num++;
      };
    };
    if (num > 1)
    {
      si->setNormedResid(DT_DELAY, sqrt((sum2 - sum1*sum1/num)/num));
      si->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD/num));
      si->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15/num));
    }
    else if (num == 1)
    {
      si->setNormedResid(DT_DELAY, 1.0);
      si->setSFF_NrmRes(DT_DELAY, sqrt(sumNRD));
      si->setSFF_NrmRes_plus_15(DT_DELAY, sqrt(sumNRD_plus_15));
    }
    else
    {
      si->setNormedResid(DT_DELAY, 0.0);
      si->setSFF_NrmRes(DT_DELAY, 0.0);
      si->setSFF_NrmRes_plus_15(DT_DELAY, 0.0);
    };
  };
  // make a copy of saved parameters (will use them later):
  SgEstimator                  *estimator=NULL;
  if (mgr && (estimator=mgr->estimator()))
  {
    QList<SgParameter*>        *xAll=estimator->allRegularParametersList();
    for (int i=0; i<xAll->size(); i++)
    {
      SgParameter              *p=new SgParameter(*xAll->at(i));
      allParList_.append(p);
      if (!xAll->at(i)->isAttr(SgParameter::Attr_IS_SPECIAL))
      {
        p = new SgParameter(*xAll->at(i));
        parList_.append(p);
        parByName_.insert(p->getName(), p);
      };
    };
    QList<SgPwlStorage*>       *pwlList=estimator->pwlStorage();
    for (int i=0; i<pwlList->size(); i++)
    {
      SgPwlStorage             *pwl;
      switch (config_->getPwlMode())
      {
        default:
        case SgTaskConfig::EPM_BSPLINE_LINEA:
          pwl = new SgPwlStorageBSplineL;
        break;
        case SgTaskConfig::EPM_BSPLINE_QUADR:
          pwl = new SgPwlStorageBSplineQ;
        break;
        case SgTaskConfig::EPM_INCRATE:
          pwl = new SgPwlStorageIncRates;
        break;
      };
      *pwl = *pwlList->at(i);
      pwlList_.append(pwl); 
      pwlByName_.insert(pwl->getPOrig()->getName(), pwl);
    };
    QList<SgArcStorage*>       *arcList=estimator->arcStorage();
    for (int i=0; i<arcList->size(); i++)
    {
      SgArcStorage             *arc=new SgArcStorage;
      *arc = *arcList->at(i);
      arcList_.append(arc); 
      arcByName_.insert(arc->getPOrig()->getName(), arc);
    };
    PxAll_ = new SgSymMatrix(*estimator->mPall());
    condNumber_ = estimator->maxConditionNumber();
  };
  //
  //
  // stochastic parameters:
  const QMap<QString, SgEstimator::StochasticSolutionCarrier> stcs=estimator->stcSolutions();
  for (QMap<QString, SgEstimator::StochasticSolutionCarrier>::const_iterator it=stcs.begin();
    it!=stcs.end(); ++it)
  {
    const SgEstimator::StochasticSolutionCarrier
                               &stcSC=it.value();
    SgEstimator::StochasticSolutionCarrier
                                carrier;
    QString                     timeTag(stcSC.epoch_.toString(SgMJD::F_INTERNAL));
    // put the solution in the container:
    carrier.epoch_ = stcSC.epoch_;
    carrier.list_ = new QList<SgParameter*>();
    for (int i=0; i<stcSC.list_->size(); i++)
    {
      SgParameter              *par=NULL, *p=stcSC.list_->at(i);
      if (p->getNumObs())
      {
        carrier.list_->append((par=new SgParameter(*p)));
        (stcParByName_[par->getName()])[timeTag] = par;
      };
    };
    carrier.x_ = new SgVector(*stcSC.x_);
    carrier.P_ = new SgSymMatrix(*stcSC.P_);
    stcSolutions_.insert(timeTag, carrier);
  };
  //
  //
  numOfObservations_ = 
    activeBand_->numProcessed(config_->getUseDelayType()!=SgTaskConfig::VD_NONE?DT_DELAY:DT_RATE);
  numOfConstraints_ = mgr->getNumOfConstraints();
  numOfUnknowns_ = mgr->getNumOfParameters();

  //
  //
  SgParameter                  *p=session_->pUT1();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dUt1Correction_ = p->getSolution();
    dUt1StdDev_ = p->getSigma();
  };
};



//
void SgSolutionReporter::lookupParameter(SgParameter* p)
{
  if (parByName_.contains(p->getName()))
  {
    *p = *parByName_[p->getName()];
  }
  else if (pwlByName_.contains(p->getName()))
  {
    *p = *pwlByName_[p->getName()]->getPOrig();
  }
  else if (arcByName_.contains(p->getName()))
  {
    *p = *arcByName_[p->getName()]->getPOrig();
  }
  else if (stcParByName_.contains(p->getName()))
  {
    *p = **stcParByName_[p->getName()].begin();
  }
  else
  {
//    std::cout << "Parameter " << qPrintable(p->getName()) << " was not estimated\n";
  };
};



//
void SgSolutionReporter::synchronizeInfo()
{
  //
  // stations:
  StationsByName_it             it_st;
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo *si = it_st.value();
    for (int i=0; i<si->getClocksModelOrder(); i++)
      lookupParameter(si->pClocks(i));
    for (int i=0; i<si->clockBreaks().size(); i++)
    {
      lookupParameter(si->clockBreaks().at(i)->pA0());
      lookupParameter(si->clockBreaks().at(i)->pA1());
      lookupParameter(si->clockBreaks().at(i)->pA2());
    };
    lookupParameter(si->pZenithDelay());
    lookupParameter(si->pAtmGradN());
    lookupParameter(si->pAtmGradE());
    lookupParameter(si->pRx());
    lookupParameter(si->pRy());
    lookupParameter(si->pRz());
    lookupParameter(si->pAxisOffset());
  };
  //
  // sources:
  SourcesByName_it              it_so;
  for (it_so=session_->sourcesByName().begin(); it_so!=session_->sourcesByName().end(); ++it_so)
  {
    SgVlbiSourceInfo *si = it_so.value();
    lookupParameter(si->pRA());
    lookupParameter(si->pDN());
  };
  //
  // baselines:
  BaselinesByName_it            it_bl;
  for (it_bl=session_->baselinesByName().begin(); it_bl!=session_->baselinesByName().end(); ++it_bl)
  {
    SgVlbiBaselineInfo *bi = it_bl.value();
    lookupParameter(bi->pClock());
    lookupParameter(bi->pBx());
    lookupParameter(bi->pBy());
    lookupParameter(bi->pBz());
  };
  //
  // EOP:
  lookupParameter(session_->pPolusX());
  lookupParameter(session_->pPolusXRate());
  lookupParameter(session_->pPolusY());
  lookupParameter(session_->pPolusYRate());
  lookupParameter(session_->pUT1());
  lookupParameter(session_->pUT1Rate());
  lookupParameter(session_->pNutX());
  lookupParameter(session_->pNutY());
};



//
void SgSolutionReporter::evaluateUsedErpApriori2()
{
  erp_ut1_0_ = erp_ut1_1_ = erp_ut1_2_ = erp_ut1_3_ = 0.0;
  erp_pmx_0_ = erp_pmx_1_ = erp_pmx_2_ = erp_pmx_3_ = 0.0;
  erp_pmy_0_ = erp_pmy_1_ = erp_pmy_2_ = erp_pmy_3_ = 0.0;
  eop_cix_0_ = eop_cix_1_ = eop_cix_2_ = eop_cix_3_ = 0.0;
  eop_ciy_0_ = eop_ciy_1_ = eop_ciy_2_ = eop_ciy_3_ = 0.0;

  if (erpTref_ == tZero)
  {
    erpTref_ = session_->tRefer();
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::evaluateUsedErpApriori2(): the ERP reference time has been adjusted to " + erpTref_.toString());
  };
  session_->getAprioriErp(erpTref_, 
    erp_ut1_0_, erp_ut1_1_, 
    erp_pmx_0_, erp_pmx_1_, erp_pmy_0_, erp_pmy_1_,
    eop_cix_0_, eop_cix_1_, eop_ciy_0_, eop_ciy_1_);
};




//
void SgSolutionReporter::evaluateUsedErpApriori()
{
  if (erpTref_ == tZero)
  {
    erpTref_ = session_->tRefer();
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::evaluateUsedErpApriori(): the ERP reference time has been adjusted to " + erpTref_.toString());
  };
  bool                          isNormalSession;
  SgVlbiObservation            *obs;
  int                           num;
  erp_ut1_0_ = erp_ut1_1_ = erp_ut1_2_ = erp_ut1_3_ = 0.0;
  erp_pmx_0_ = erp_pmx_1_ = erp_pmx_2_ = erp_pmx_3_ = 0.0;
  erp_pmy_0_ = erp_pmy_1_ = erp_pmy_2_ = erp_pmy_3_ = 0.0;
  eop_cix_0_ = eop_cix_1_ = eop_cix_2_ = eop_cix_3_ = 0.0;
  eop_ciy_0_ = eop_ciy_1_ = eop_ciy_2_ = eop_ciy_3_ = 0.0;

  SgEstimator                  *est_ut=new SgEstimator(config_);
  SgEstimator                  *est_px=new SgEstimator(config_);
  SgEstimator                  *est_py=new SgEstimator(config_);
  SgEstimator                  *est_cx=new SgEstimator(config_);
  SgEstimator                  *est_cy=new SgEstimator(config_);
  QList<SgParameter*>          *prs_ut=new QList<SgParameter*>;
  QList<SgParameter*>          *prs_px=new QList<SgParameter*>;
  QList<SgParameter*>          *prs_py=new QList<SgParameter*>;
  QList<SgParameter*>          *prs_cx=new QList<SgParameter*>;
  QList<SgParameter*>          *prs_cy=new QList<SgParameter*>;
  SgParameter                  *pA_ut, *pB_ut, *pC_ut, *pD_ut;
  SgParameter                  *pA_px, *pB_px, *pC_px, *pD_px;
  SgParameter                  *pA_py, *pB_py, *pC_py, *pD_py;
  SgParameter                  *pA_cx, *pB_cx, *pC_cx, *pD_cx;
  SgParameter                  *pA_cy, *pB_cy, *pC_cy, *pD_cy;
  isNormalSession = 16.0/24.0 <= (session_->getTFinis() - session_->getTStart());
  // UT1:
  prs_ut->append((pA_ut=new SgParameter("UT1, 0-term (ms    )")));
  prs_ut->append((pB_ut=new SgParameter("UT1, 1-term (ms/d  )")));
  // Px:
  prs_px->append((pA_px=new SgParameter("PMx, 0-term (mas    )")));
  prs_px->append((pB_px=new SgParameter("PMx, 1-term (mas/d  )")));
  // Py:
  prs_py->append((pA_py=new SgParameter("PMy, 0-term (mas    )")));
  prs_py->append((pB_py=new SgParameter("PMy, 1-term (mas/d  )")));
  // CIPx:
  prs_cx->append((pA_cx=new SgParameter("CIPx, 0-term (mas    )")));
  prs_cx->append((pB_cx=new SgParameter("CIPx, 1-term (mas/d  )")));
  // CIPy:
  prs_cy->append((pA_cy=new SgParameter("CIPy, 0-term (mas    )")));
  prs_cy->append((pB_cy=new SgParameter("CIPy, 1-term (mas/d  )")));
  //
  if (isNormalSession)
  {
    prs_ut->append((pC_ut=new SgParameter("UT1, 2-term (ms/d^2)")));
    prs_ut->append((pD_ut=new SgParameter("UT1, 3-term (ms/d^3)")));
    prs_px->append((pC_px=new SgParameter("PMx, 2-term (mas/d^2)")));
    prs_px->append((pD_px=new SgParameter("PMx, 3-term (mas/d^3)")));
    prs_py->append((pC_py=new SgParameter("PMy, 2-term (mas/d^2)")));
    prs_py->append((pD_py=new SgParameter("PMy, 3-term (mas/d^3)")));
    prs_cx->append((pC_cx=new SgParameter("CIPx, 2-term (mas/d^2)")));
    prs_cx->append((pD_cx=new SgParameter("CIPx, 3-term (mas/d^3)")));
    prs_cy->append((pC_cy=new SgParameter("CIPy, 2-term (mas/d^2)")));
    prs_cy->append((pD_cy=new SgParameter("CIPy, 3-term (mas/d^3)")));
  }
  for (int i=0; i<prs_ut->size(); i++)
  {
    SgParameter                *p=prs_ut->at(i);
    p->setSigmaAPriori(1.0e+8);
    p->setScale(1.0e3*DAY2SEC);
    p->setPMode(SgParameterCfg::PM_LOC);
  };
  for (int i=0; i<prs_px->size(); i++)
  {
    SgParameter                *p=prs_px->at(i);
    p->setSigmaAPriori(1.0e+8);
    p->setScale(1.0e3*RAD2SEC);
    p->setPMode(SgParameterCfg::PM_LOC);
  };
  for (int i=0; i<prs_py->size(); i++)
  {
    SgParameter                *p=prs_py->at(i);
    p->setSigmaAPriori(1.0e+8);
    p->setScale(1.0e3*RAD2SEC);
    p->setPMode(SgParameterCfg::PM_LOC);
  };
  for (int i=0; i<prs_cx->size(); i++)
  {
    SgParameter                *p=prs_cx->at(i);
    p->setSigmaAPriori(1.0e+8);
    p->setScale(1.0e3*RAD2SEC);
    p->setPMode(SgParameterCfg::PM_LOC);
  };
  for (int i=0; i<prs_cy->size(); i++)
  {
    SgParameter                *p=prs_cy->at(i);
    p->setSigmaAPriori(1.0e+8);
    p->setScale(1.0e3*RAD2SEC);
    p->setPMode(SgParameterCfg::PM_LOC);
  };
  //
  est_ut->addParametersList(prs_ut);
  est_px->addParametersList(prs_px);
  est_py->addParametersList(prs_py);
  est_cx->addParametersList(prs_cx);
  est_cy->addParametersList(prs_cy);
  est_ut->prepare2Run(session_->observations().size()+10, 
    session_->getTStart(), session_->getTFinis(), session_->tRefer());
  est_px->prepare2Run(session_->observations().size()+10, 
    session_->getTStart(), session_->getTFinis(), session_->tRefer());
  est_py->prepare2Run(session_->observations().size()+10, 
    session_->getTStart(), session_->getTFinis(), session_->tRefer());
  est_cx->prepare2Run(session_->observations().size()+10,
    session_->getTStart(), session_->getTFinis(), session_->tRefer());
  est_cy->prepare2Run(session_->observations().size()+10,
    session_->getTStart(), session_->getTFinis(), session_->tRefer());
  //
  // feed the estimators:
  double                        sig, dT, dt, dt2, dt3;
  double                        dsSB=0.1E-9; // 0.1ns
  double                        dsGR=2.E-12; // 2ps
  SgVector                      vO_C(1), vSigma(1);
  SgMJD                         t0(erpTref_);
  num = 0;
  dT = (session_->getLeapSeconds() + 32.184)/DAY2SEC;
  for (int i=0; i<session_->observations().size(); i++)
  {
    obs = session_->observations().at(i);
    SgVlbiMeasurement          *m=obs->activeObs()?obs->activeObs()->activeDelay():NULL;
    if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      sig = m->sigma2Apply() + dsGR;
      if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
        sig += dsSB;
      vSigma.setElement(0, sig);
      
      dt  = (*obs - t0) + dT;
      dt2 = dt*dt;
      dt3 = dt2*dt;
      //
      pA_ut->setD(1.0);
      pB_ut->setD(dt);

      pA_px->setD(1.0);
      pB_px->setD(dt);

      pA_py->setD(1.0);
      pB_py->setD(dt);

      pA_cx->setD(1.0);
      pB_cx->setD(dt);

      pA_cy->setD(1.0);
      pB_cy->setD(dt);

      if (isNormalSession)
      {
        pC_ut->setD(dt2);
        pD_ut->setD(dt3);
        pC_px->setD(dt2);
        pD_px->setD(dt3);
        pC_py->setD(dt2);
        pD_py->setD(dt3);
        pC_cx->setD(dt2);
        pD_cx->setD(dt3);
        pC_cy->setD(dt2);
        pD_cy->setD(dt3);
      };
      //
      vO_C.setElement  (0, obs->aPrioriUt1());
      est_ut->processObs(*obs, vO_C, vSigma);
      //
      vO_C.setElement  (0, obs->aPrioriPx());
      est_px->processObs(*obs, vO_C, vSigma);
      //
      vO_C.setElement  (0, obs->aPrioriPy());
      est_py->processObs(*obs, vO_C, vSigma);
      //
      vO_C.setElement  (0, obs->aPrioriCipX());
      est_cx->processObs(*obs, vO_C, vSigma);
      //
      vO_C.setElement  (0, obs->aPrioriCipY());
      est_cy->processObs(*obs, vO_C, vSigma);
      num++;
    };
  };
  est_ut->finisRun();
  est_px->finisRun();
  est_py->finisRun();
  est_cx->finisRun();
  est_cy->finisRun();
  //
  erp_ut1_0_ = pA_ut->getSolution();
  erp_ut1_1_ = pB_ut->getSolution();
  //
  erp_pmx_0_ = pA_px->getSolution();
  erp_pmx_1_ = pB_px->getSolution();
  //
  erp_pmy_0_ = pA_py->getSolution();
  erp_pmy_1_ = pB_py->getSolution();
  //
  eop_cix_0_ = pA_cx->getSolution();
  eop_cix_1_ = pB_cx->getSolution();
  //
  eop_ciy_0_ = pA_cy->getSolution();
  eop_ciy_1_ = pB_cy->getSolution();
  //
  if (isNormalSession)
  {
    erp_ut1_2_ = pC_ut->getSolution();
    erp_ut1_3_ = pD_ut->getSolution();
    erp_pmx_2_ = pC_px->getSolution();
    erp_pmx_3_ = pD_px->getSolution();
    erp_pmy_2_ = pC_py->getSolution();
    erp_pmy_3_ = pD_py->getSolution();
    eop_cix_2_ = pC_cx->getSolution();
    eop_cix_3_ = pD_cx->getSolution();
    eop_ciy_2_ = pC_cy->getSolution();
    eop_ciy_3_ = pD_cy->getSolution();
  };
  //
  for (int i=0; i<prs_ut->size(); i++)
    delete prs_ut->at(i);
  for (int i=0; i<prs_px->size(); i++)
    delete prs_px->at(i);
  for (int i=0; i<prs_py->size(); i++)
    delete prs_py->at(i);
  for (int i=0; i<prs_cx->size(); i++)
    delete prs_cx->at(i);
  for (int i=0; i<prs_cy->size(); i++)
    delete prs_cy->at(i);
  delete prs_ut;
  delete prs_px;
  delete prs_py;
  delete prs_cx;
  delete prs_cy;
  //
  delete est_ut;
  delete est_px;
  delete est_py;
  delete est_cx;
  delete est_cy;
};



//
void SgSolutionReporter::calculateConditionNumber()
{
  if (!PxAll_)
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::calculateConditionNumber(): PxAll is NULL");
    return;
  };
  if (PxAll_->n()<1)
  {
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::calculateConditionNumber(): the dimenstion of PxAll is less than unity, nothing to calculate");
    condNumber_ = -1.0;
    return;
  };
  if (PxAll_->n()==1)
  {
    logger->write(SgLogger::INF, SgLogger::REPORT, className() +
      "::calculateConditionNumber(): the dimenstion of PxAll is unity, nothing to calculate");
    condNumber_ = 1.0;
    return;
  };
  //
  SgMatrix                     *mP;
  SgVector                     *u;
  int                           n;
  double                        s, betta, gamma;
  //
  n = PxAll_->n();
  mP = new SgMatrix(n, n);
  u = new SgVector(n);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++)
      mP->setElement(i, j,  PxAll_->getElement(i, j));
  
  for (int l=0; l<n; l++)
  {
    s = 0.0;
    for (int i=l; i<n; i++)
      s += mP->getElement(i, l)*mP->getElement(i, l);
    s = - signum(mP->getElement(l, l))*sqrt(s);
    u->setElement(l, mP->getElement(l, l) - s);
    for (int i=l+1; i<n; i++)
      u->setElement(i, mP->getElement(i, l));
    mP->setElement(l, l,  s);
    betta = 1.0/(s*u->getElement(l));
    for (int j=l+1; j<n; j++)
    {
      s = 0.0;
      for (int i=l; i<n; i++)
        s += u->getElement(i)*mP->getElement(i, j);
      gamma = betta*s;
      for (int i=l+1; i<n; i++)
        mP->setElement(i, j,    mP->getElement(i, j) + gamma*u->getElement(i));
    };
  };
  // end of householdering


  for (int i=0; i<n; i++)
    u->setElement(i,  sqrt(PxAll_->getElement(i, i)));
  // diagonals of P are eigenvalues.
  double                        minEv, maxEv;
  minEv = maxEv = u->getElement(0);
  for (int i=0; i<n; i++)
  {
std::cout << i << ": " << u->getElement(i) << "\n";
    if (u->getElement(i) < minEv)
      minEv = u->getElement(i);
    if (u->getElement(i) > maxEv)
      maxEv = u->getElement(i);
  };
std::cout << "max eigenvalue= " << maxEv << ", min eigenvalue= " << minEv 
          << "condition number= " << (condNumber_=(minEv!=0.0?maxEv/minEv:-1.0))
          << "\n";
  
  delete mP;
  delete u;
};



//
void SgSolutionReporter::report2spoolFile(const QString& path, const QString& path2obsStatus,
  const QString& fileName, const QString& rptrDirName,
  bool needResidualsOutput, bool storeUnusedObsList)
{
  if (PxAll_ == NULL)
  {
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::report2spoolFile(): an attempt to create a report without a solution");
    return;
  };

  creationEpoch_ = SgMJD::currentMJD();
  if (runEpoch_==tZero)
    runEpoch_ = creationEpoch_;

  QFile                         f(path + "/" + fileName);
  QString                       auxFname("");
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::report2spoolFile(): error opening output file: " + path + "/" + fileName);
    return;
  };
  const SgTaskConfig::AutomaticProcessing&
                                ap = config_->apByNetId().contains(session_->getNetworkID()) ?
    config_->apByNetId()[session_->getNetworkID()] : config_->apByNetId()[defaultNetIdName];

  session_->createParameters();
  synchronizeInfo();
//  calculateConditionNumber();
  
  bool                          hasEop(false);
  if (session_->pPolusX() && session_->pPolusX()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusX()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusXRate() && session_->pPolusXRate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusXRate()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusY() && session_->pPolusY()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusY()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusYRate() && session_->pPolusYRate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusYRate()->getTMean();
    hasEop = true;
  };
  if (session_->pUT1() && session_->pUT1()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pUT1()->getTMean();
    hasEop = true;
  };
  if (session_->pUT1Rate() && session_->pUT1Rate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pUT1Rate()->getTMean();
    hasEop = true;
  };
  if (hasEop)  
    evaluateUsedErpApriori();

  //
  //
  QTextStream ts(&f);
  // make output:
  // the header:
  reportIdentitiesBlock_Output4Spoolfile(ts);
  // the fly-byes:
  reportConfiguresBlock_Output4Spoolfile(ts);
  if (needResidualsOutput)
    reportResidualsBlock_Output4Spoolfile_old(ts);
  // meteo statistics:
  reportMetStatsBlock_Output4Spoolfile(ts);
  // general statistics:
  reportGeneralStatsBlock_Output4Spoolfile(ts);
  // per baseline statistics:
  reportBaselineStatsBlock_Output4Spoolfile(ts);
  // per source statistics:
  reportSourceStatsBlock_Output4Spoolfile(ts);
  // per station statistics:
  reportStationStatsBlock_Output4Spoolfile(ts);
  // a priori and "flybies":
  reportAPrioriBlock_Output4Spoolfile(ts);
  // results of estimation process:
  reportEstimationBlock_Output4Spoolfile(ts);
  // baseline variations:
  reportBaselineVariationsBlock_Output4Spoolfile(ts);
  // close the file:
  ts.setDevice(NULL);
  f.close();
  //
  if (config_->getIsSolveCompatible())
  {
    if (parametersDescriptor_->getZenithMode() == SgParameterCfg::PM_PWL)
    {
      if (reportAtmo(path, auxFname = "ATMO" + identities_.getUserDefaultInitials()))
        logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
          "::report2spoolFile(): the estimated zenith delays PWL-parameters have been saved "
          "in the file: " + path + "/" + auxFname);
      else
        logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
          "::report2spoolFile(): saving of the estimated zenith delays PWL-parameters failed");
    };
    //
    if (parametersDescriptor_->getClock0Mode() == SgParameterCfg::PM_PWL)
    {
      if (reportCloc(path, auxFname = "CLOC" + identities_.getUserDefaultInitials()))
        logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
          "::report2spoolFile(): the estimated clock PWL-parameters have been saved "
          "in the file: " + path + "/" + auxFname);
      else
        logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
          "::report2spoolFile(): saving of the estimated clock PWL-parameters failed");
    };
    //
    if (ap.doReportNotUsedData_)
    {
      if (reportNotUsedObs(path2obsStatus,
        auxFname = "nuSolve_unused_observations_" + identities_.getUserDefaultInitials()))
      {
        logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
          "::report2spoolFile(): the not used observations have been saved "
          "in the file: " + path2obsStatus + "/" + auxFname);
        // 
        if (storeUnusedObsList)
        {
          QDir                  d(rptrDirName);
          bool                  isOk;
          if (!d.exists())
          {
//          isOk = d.mkpath("./");
            isOk = d.mkpath(d.absolutePath());
            if (!isOk)
            {
              logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
                "::report2spoolFile(): cannot create directory \"" + rptrDirName + 
                "\"; saving report failed");
            };
          };
          QString                 str(session_->getName() + ".NUO");
          if (QFile::exists(rptrDirName + "/" + str) && !QFile(rptrDirName + "/" + str).remove())
          {
            logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
              "::report2spoolFile(): cannot remove the file " + rptrDirName + "/" + str + 
              "; saving the report has failed");
            return;
          };
          if (QFile::copy(path2obsStatus + "/" + auxFname, rptrDirName + "/" + str))
            logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
              "::report2spoolFile(): the list of not used observations has been copied as \"" + 
                rptrDirName + "/" + str + "\" file");
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::REPORT, className() + 
              "::report2spoolFile(): copying of \"" + path2obsStatus + "/" + auxFname + "\" as \"" + 
              rptrDirName + "/" + str + "\" has filed");
        };
      }
      else
        logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
          "::report2spoolFile(): saving of the not used observations failed");
    };
  };
  //
  //
  if (config_->getHave2outputCovarMatrix() && PxAll_ && allParList_.size())
  {
    if (reportPall(path, auxFname = "PALL" + identities_.getUserDefaultInitials()))
      logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
        "::report2spoolFile(): the covariance matrix has been saved "
        "in the file: " + path + "/" + auxFname);
    else
      logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
        "::report2spoolFile(): saving of the covariance matrix failed");
  };
  //
  //
  session_->releaseParameters();
};



//
void SgSolutionReporter::report2aposterioriFiles(const QString& path, const QString& fileNameBase)
{
  if (PxAll_ == NULL)
  {
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::report2aposterioriFiles(): an attempt to create a report without a solution");
    return;
  };
  //
  creationEpoch_ = SgMJD::currentMJD();
  if (runEpoch_==tZero)
    runEpoch_ = creationEpoch_;
  //
  session_->createParameters();
  synchronizeInfo();
  //
  bool                          hasEop(false);
  if (session_->pPolusX() && session_->pPolusX()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusX()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusXRate() && session_->pPolusXRate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusXRate()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusY() && session_->pPolusY()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusY()->getTMean();
    hasEop = true;
  };
  if (session_->pPolusYRate() && session_->pPolusYRate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pPolusYRate()->getTMean();
    hasEop = true;
  };
  if (session_->pUT1() && session_->pUT1()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pUT1()->getTMean();
    hasEop = true;
  };
  if (session_->pUT1Rate() && session_->pUT1Rate()->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    erpTref_ = session_->pUT1Rate()->getTMean();
    hasEop = true;
  };
  if (hasEop)  
    evaluateUsedErpApriori();
  //
  //
  if (reportSources_Output4AposterioriFiles(path, fileNameBase))
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::report2aposterioriFiles(): a file with a posteriori positions "
      "of radio sources has been created");
  else
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::report2aposterioriFiles(): creating a file with a posteriori positions "
      "of radio sources failed");
  
  if (reportStations_Output4AposterioriFiles(path, fileNameBase))
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::report2aposterioriFiles(): files with a posteriori positions "
      "of stations have been created");
  else
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::report2aposterioriFiles(): creating files with a posteriori positions "
      "of stations failed");
  //
  session_->releaseParameters();
};



//
bool SgSolutionReporter::reportSources_Output4AposterioriFiles(const QString& path,
  const QString& fileNameBase)
{
  if (parametersDescriptor_->getSrcCooMode() != SgParameterCfg::PM_LOC)
  {
    logger->write(SgLogger::INF, SgLogger::REPORT, className() +
      "::reportSources_Output4AposterioriFiles(): nothing to export, "
      "source positions were not estimated");
    return false;
  };
  //
  // collect statistics:
  if (!usedSources_.size())
  {
    for (QMap<QString, SgVlbiSourceInfo*>::iterator it=session_->sourcesByName().begin(); 
      it!=session_->sourcesByName().end(); ++it)
    {
      SgVlbiSourceInfo         *si=it.value();
      if (!si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
        usedSources_.append(si);
    };
    // sort in RA order:
    qSort(usedSources_.begin(),    usedSources_.end(),    rightAscensionSortingOrderLessThan);
  };
  //
  //
  QFile                         f(path + "/" + fileNameBase + ".src");
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportSources_Output4AposterioriFiles(): error opening output file: " + path + 
      "/" + f.fileName() + " for writing source a posteriori positions");
    return false;
  };
  QString                       str("");
  QTextStream                   ts(&f);
  // make output:
  ts << "$$ Source flyby mod file from local solution of " << session_->getName() << " session\n";
  ts << "$$  " << usedSources_.size() << " total sources\n$$\n";

  for (int i=0; i<usedSources_.size(); i++)
  {
    SgVlbiSourceInfo           *si=usedSources_.at(i);
    if (si->pRA()->isAttr(SgParameter::Attr_IS_SOLVED) &&
        si->pDN()->isAttr(SgParameter::Attr_IS_SOLVED)  )
    {
      double                    vRA, vDN;
      vRA = config_->getUseExtAPrioriSourcesPositions()?si->getRA_ea():si->getRA();
      vDN = config_->getUseExtAPrioriSourcesPositions()?si->getDN_ea():si->getDN();
      vRA+= si->pRA()->getSolution();
      vDN+= si->pDN()->getSolution();
      //
      //
      str.sprintf("#-> %-8s  %s  %s   %.6f %.6f %5d   %.6f  %-20s",
        qPrintable(si->getKey()),
        qPrintable(si->ra2String(vRA)),
        qPrintable(si->dn2String(vDN, true)),
        si->pRA()->getSigma()*RAD2MS,
        si->pDN()->getSigma()*RAD2MAS,
        si->pRA()->getNumObs(),
        si->pRA()->getTMean().toDouble(),
        qPrintable(session_->getName())
        );
      ts << str << "\n";
      str.sprintf("    %-8s  %s   %s          %s",
        qPrintable(si->getKey()),
        qPrintable(si->ra2String(vRA)),
        qPrintable(si->dn2String(vDN, true)),
        qPrintable(si->getAprioriComments())
        );
      ts << str << "\n";
    };
  };
  // close the file:
  ts.setDevice(NULL);
  f.close();
  logger->write(SgLogger::INF, SgLogger::REPORT, className() +
    "::reportSources_Output4AposterioriFiles(): sources a posteriori positions were stored "
    "in file \"" + path + "/" + f.fileName() + "\"");
  return true;
};



//
bool SgSolutionReporter::reportStations_Output4AposterioriFiles(const QString& path,
  const QString& fileNameBase)
{
  // coords:
  QFile                         f(path + "/" + fileNameBase + ".sit");
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportStations_Output4AposterioriFiles(): error opening output file: " + path +
      "/" + f.fileName() + " for writing station a posteriori positions");
    return false;
  };
  QString                       sTsince("-- -- --");
  QString                       sT0("------");
  if (session_->observations().size() > 0)
  {
    int                         nYear, nMonth, nDay, nHour, nMin;
    double                      dSec;
    SgMJD::MJD_reverse(session_->observations().at(0)->getDate(), 
      session_->observations().at(0)->getTime(),    nYear, nMonth, nDay, nHour, nMin, dSec);
    sTsince.sprintf("%02d %02d %02d", nYear%100, nMonth, nDay);

    SgMJD::MJD_reverse(session_->getApStationVelocities().getT0().getDate(),
      session_->getApStationVelocities().getT0().getTime(),    nYear, nMonth, nDay, nHour, nMin, dSec);
    sT0.sprintf("%02d%02d%02d", nYear%100, nMonth, nDay);
  };
  //
  QString                       str("");
  QTextStream                   ts(&f);
  // make output:
  ts
    << "$$ This file was produced by " << libraryVersion.name() << "\n"
    << "$$ From analysis of " << session_->getName() << " database.\n"
    << "$$\n"
    << "$$\n"
    << "$$ VLBI Site positions\n"
    << "$$  positions:     x (m)          y (m)          z (m)\n";

  for (StationsByName_it it_st=session_->stationsByName().begin(); 
    it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      // coordinates:
      if (si->pRx()->isAttr(SgParameter::Attr_IS_SOLVED) &&
          si->pRy()->isAttr(SgParameter::Attr_IS_SOLVED) &&
          si->pRz()->isAttr(SgParameter::Attr_IS_SOLVED)  )
      {
        Sg3dVector                dr(si->pRx()->getSolution(), si->pRy()->getSolution(), 
                                    si->pRz()->getSolution());
        Sg3dVector                r(si->getR());
        if (config_->getUseExtAPrioriSitesPositions())
          r = si->getR_ea();
        r += dr;
        str.sprintf("    %-8s   %14.6f    %14.6f   %14.6f   ",
          qPrintable(si->getKey()),   r.at(X_AXIS), r.at(Y_AXIS), r.at(Z_AXIS));
        ts << str << sTsince << "\n";
      };
    };
  };
  // close the file:
  ts.setDevice(NULL);
  f.close();
  logger->write(SgLogger::INF, SgLogger::REPORT, className() +
    "::reportStations_Output4AposterioriFiles(): stations a posteriori coordinates were stored "
    "in file \"" + path + "/" + f.fileName() + "\"");
  //
  // vels:
  if (!config_->getUseExtAPrioriSitesPositions())
  {
    logger->write(SgLogger::INF, SgLogger::REPORT, className() +
      "::reportStations_Output4AposterioriFiles(): external velocities were not used");
    return true;
  };
  //
  f.setFileName(path + "/" + fileNameBase + ".vel");
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportStations_Output4AposterioriFiles(): error opening output file: " + path +
      "/" + f.fileName() + " for writing station a posteriori velocities");
    return false;
  };
  ts.setDevice(&f);
  // make output:
  ts
    << sT0 << "\n"
    << "$$ This file was produced by " << libraryVersion.name() << "\n"
    << "$$ From analysis of " << session_->getName() << " database.\n"
    << "$$\n"
    << "$$\n"
    << "$$  velocities:    x (mm/yr)       y (mm/yr)      z (mm/yr)\n"
    << "$$\n";

  for (StationsByName_it it_st=session_->stationsByName().begin(); 
    it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      // coordinates:
      if (si->pRx()->isAttr(SgParameter::Attr_IS_SOLVED) &&
          si->pRy()->isAttr(SgParameter::Attr_IS_SOLVED) &&
          si->pRz()->isAttr(SgParameter::Attr_IS_SOLVED)  )
      {
        Sg3dVector                v(si->getV_ea()*1.0e3*365.242198781250);
        str.sprintf("    %-8s       %8.1f        %8.1f       %8.1f",
          qPrintable(si->getKey()),   v.at(X_AXIS), v.at(Y_AXIS), v.at(Z_AXIS));

        ts << str << "\n";
      };
    };
  };
  // close the file:
  ts.setDevice(NULL);
  f.close();
  logger->write(SgLogger::INF, SgLogger::REPORT, className() +
    "::reportStations_Output4AposterioriFiles(): stations velocities were stored "
    "in file \"" + path + "/" + f.fileName() + "\"");
  return true;
};



//
void SgSolutionReporter::reportIdentitiesBlock_Output4Spoolfile(QTextStream& ts)
{
  int                           nYear, nMonth, nDay, nHour, nMin;
  double                        dSec;
  QString                       str(""), dbName("");

  SgMJD::MJD_reverse(runEpoch_.getDate(), runEpoch_.getTime(),
                    nYear, nMonth, nDay, nHour, nMin, dSec);
  
  QDateTime                     dt(QDate(nYear, nMonth, nDay), QTime(nHour, nMin, (int)(dSec)));
  QDateTime                     dtUTC(dt.toUTC());

  reportID_.sprintf("%2d%03d-%02d%02d",
    dtUTC.date().year() - (dtUTC.date().year()/100)*100,
    dtUTC.date().dayOfYear(),
    dtUTC.time().hour(),
    dtUTC.time().minute());
  ts << "1Run " << reportID_ << "\n";

  SgMJD::MJD_reverse(creationEpoch_.getDate(), creationEpoch_.getTime(),
                    nYear, nMonth, nDay, nHour, nMin, dSec);

  str.sprintf(" Analysis center: %3s -- %s",
    qPrintable(identities_.getAcAbbName()), qPrintable(identities_.getAcFullName()));
  ts << str << "\n";

  str.sprintf(" Analyst:         %s ( %s )",
    qPrintable(identities_.getUserName()), qPrintable(identities_.getUserEmailAddress()));
  ts << str << "\n";

  str.sprintf(" Machine:         %s  %s  %s %s",
    qPrintable(identities_.getMachineNodeName()), qPrintable(identities_.getMachineMachineName()),
    qPrintable(identities_.getMachineSysName()), qPrintable(identities_.getMachineRelease()));
  ts << str << "\n";

  str.sprintf(" Executables:     %s",
    qPrintable(identities_.getExecDir()));
  ts << str << "\n";

  str.sprintf(" Solve initials:  %2s",
    qPrintable(identities_.getUserDefaultInitials()));
  ts << str << "\n";
  ts << " Spool format:    " << libraryVersion.name() << "\n";

  str.sprintf(" Local time:      %04d.%02d.%02d-%02d:%02d:%02d",
    nYear, nMonth, nDay, nHour, nMin, (int)dSec);
  ts << str << "\n";

  str.sprintf(" Correlator type: %s",
    qPrintable(activeBand_->getCorrelatorType()));
  ts << str << "\n";
  ts << " Mark-3 db_name:" << "\n\n" ;

  dbName = session_->getName();
  if (dbName.at(0) == '$')
    dbName.remove(0, 1);

  // keep $-sign for old names still:
  if (dbName.size() <= 9)
    dbName = "$" + dbName;

  if (false)
  {
    if (session_->bands().size() == 2)
      dbName = dbName.mid(0, 7) + activeBand_->getKey() + session_->getNetworkSuffix();
  };
//str.sprintf(" Data base $%9s Ver%3d",
  str.sprintf(" Data base %s Ver%3d",
//    qPrintable(session_->getName()), session_->primaryBand()->getInputFileVersion());
//    qPrintable(session_->getName()), activeBand_->getInputFileVersion());
    qPrintable(dbName), activeBand_->getInputFileVersion());
  ts << str << "\n\n";

  str.sprintf(" Matrix Condition Number =%24.15E",
    condNumber_);
  ts << str << "\n\n";

  // should be replaced with actual info later:
  ts << " Listing_Options:  CRES_EMULATION NO  BASELINES NO  MINIMUM NO" << "\n"
     << " Listing_Options:  MAPPED_EOP_OUTPUT NO  SEG_OUTPUT NO  APRIORI_ZENDEL NO" << "\n"
     << " Listing_Options:  NRD_TABLE YES  CHI_TABLE NO  SRC_STAT PRE2004  SEG_STYLE PRE2005" << "\n";
  ts << " SgLib release  :  " << 
        qPrintable(libraryVersion.getReleaseEpoch().toString(SgMJD::F_Date)) << "\n\n";

  if (userComents_.size())
  {
    ts << " User comments:" << "\n";
    for (int i=0; i<userComents_.size(); i++)
      ts << " " << identities_.getUserDefaultInitials() << ": " << userComents_.at(i) << "\n";
    ts << "\n";
  };
};



//
void SgSolutionReporter::reportConfiguresBlock_Output4Spoolfile(QTextStream& ts)
{
  ts  << "  Flyby Station Cals:    DB Station Cals:              | DB Non-station Cals: "
      << "| Atmosphere Partial:\n"
      << " ------------------------------------------------------------------------------"
      << "--------------------\n";

  QString               str;
  QList<QString>        listOfContributions;
  int                   numOfStrings, idxStr, idxContr;
  StationsByName_it     it;

  if (config_->getHave2ApplyPoleTideContrib())
    listOfContributions << "Pol Tide";
  if (config_->getHave2ApplyPxContrib())
    listOfContributions << "WobXCont";
  if (config_->getHave2ApplyPyContrib())
    listOfContributions << "WobYCont";
  if (config_->getHave2ApplyEarthTideContrib())
    listOfContributions << "EarthTid";
  if (config_->getHave2ApplyOceanTideContrib())
    listOfContributions << "Ocean";
  if (config_->getHave2ApplyUt1OceanTideHFContrib())
    listOfContributions << "UT1Ortho";
  if (config_->getHave2ApplyPxyOceanTideHFContrib())
    listOfContributions << "XpYpOrth";
  //
  if (session_->calcInfo().getDversion()<11.0)
  {
    if (config_->getHave2ApplyNutationHFContrib())
      listOfContributions << "PxyNutat";
  }
  else
  {
    if (config_->getHave2ApplyPxyLibrationContrib())
      listOfContributions << "XpYpLib";
    if (config_->getHave2ApplyUt1LibrationContrib())
      listOfContributions << "UT1Libra";
  };
  //
  if (config_->getHave2ApplyFeedCorrContrib())
    listOfContributions << "FeedCorr";
  if (config_->getHave2ApplyTiltRemvrContrib())
    listOfContributions << "TiltRmvr";
  if (config_->getHave2ApplyOceanPoleTideContrib())
    listOfContributions << "OPTLCont";
  if (config_->getHave2ApplyOldOceanTideContrib())
    listOfContributions << "OldOcean";
  if (config_->getHave2ApplyOldPoleTideContrib())
    listOfContributions << "OldPTide";

  
  // determine how the ionosphere corrections were applied:
  QString                       sIono("SION");
  if (config_->getUseDelayType() == SgTaskConfig::VD_GRP_DELAY)
    sIono = "GION";
  else if (config_->getUseDelayType() == SgTaskConfig::VD_PHS_DELAY)
    sIono = "PION";
  QMap<QString, int>            gionByName;
  QString                       sIonos[3]={"", sIono, ""};
  sIonos[2] = sIono + "/no" + sIono;
  //
  for (StationsByName_it it=session_->stationsByName().begin(); 
    it!=session_->stationsByName().end(); ++it)
  {
    QString                     stName=it.value()->getKey();
    for (BaselinesByName_it jt=session_->baselinesByName().begin(); 
      jt!=session_->baselinesByName().end(); ++jt)
    {
      SgVlbiBaselineInfo       *bln=jt.value();
      if (!bln->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      {
        QString                   blName=bln->getKey();
        if (blName.contains(stName + ":") || blName.contains(":" + stName))
        {
          int                     nVal(0);
          if (  (bln->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4GRD) && 
                (config_->getUseDelayType() == SgTaskConfig::VD_GRP_DELAY)) ||
                (bln->isAttr(SgVlbiBaselineInfo::Attr_USE_IONO4PHD) && 
                (config_->getUseDelayType() == SgTaskConfig::VD_PHS_DELAY))  )
            nVal = 1;
          if (!gionByName.contains(stName))
            gionByName.insert(stName, nVal);
          else if (gionByName.value(stName)!=2 && gionByName.value(stName)!=nVal)
            gionByName[stName] = 2;
        };
      };
    };
  };
  // fill default value for not used objects:
  for (StationsByName_it it=session_->stationsByName().begin(); 
    it!=session_->stationsByName().end(); ++it)
    if (!gionByName.contains(it.value()->getKey()))
      gionByName.insert(it.value()->getKey(), 0);
  //
  numOfStrings = std::max(session_->stationsByName().size(), listOfContributions.size());
  idxStr = idxContr = 0;
  it = session_->stationsByName().begin();
  while (idxStr<numOfStrings)
  {
    if (it!=session_->stationsByName().end())
    {
      SgVlbiStationInfo *si=it.value();
      str.sprintf("    %-8s:NMFDRFLY    %-8s %-11s          ",
        qPrintable(si->getKey()),
        si->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL)?"     ":"cable",
        qPrintable(sIonos[gionByName[si->getKey()]])
//        session_->isAttr(SgVlbiSessionInfo::Attr_FF_ION_C_CALCULATED)?"GION":"    "
        );
       ++it;
    }
    else
      str = "                                                       ";
    ts << str;
    if (idxContr<listOfContributions.size())
    {
      str.sprintf("| %-8s             |",
        qPrintable(listOfContributions.at(idxContr)));
      idxContr++;
    }
    else
      str = "|                      |";
    ts << str;
    if (idxStr==0)
      ts << " NMFWTFLY              \n";
    else
      ts << "                       \n";
    idxStr++;
  };

  ts  << " ----------------------------------------------------------------------------"
      << "----------------------\n";
  str.sprintf(" CALC Version:  %.2f  ",    //SOLVE release: ????.??.??  SOLVE revision: ????.??.??  ",
    session_->calcInfo().getDversion());
  ts  << str << qPrintable(libraryVersion.name()) << "\n\n";
  //      << " Fast_mode: ???  Fast_cov: ???                                                \n\n";

//QString             primaryBandKey = session_->primaryBand()->getKey(), auxBandKey, s2;
/*
  QString             auxBandKey, s2;
  for (int i=0; i<session_->bands().size(); i++)
  {
    auxBandKey = session_->bands().at(i)->getKey();
    if (auxBandKey != activeBandKey_)
    {
      s2 = session_->getName();
      str.sprintf("          %10s %2d  NOT IN SOLUTION",
        qPrintable(s2.replace(s2.size()-2, 1, auxBandKey)),
        session_->bands().at(i)->getInputFileVersion());
      ts << str << "\n";
    };
  };
  ts << "\n";
*/
};



//
void SgSolutionReporter::reportResidualsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString                       str(""), useChar1(""), useChar2(""), sQcOb("");
  QString                       sVal("");
  QString                       oppBandKey("");
  double                        dSnrOb, dDelay;
  bool                          isMultipleBand;
  isMultipleBand = session_->bands().size() > 1;
  for (int i=0; i<session_->bands().size(); i++)
    if (session_->bands().at(i)->getKey() != activeBandKey_)
      oppBandKey = session_->bands().at(i)->getKey();
  //
 
  ts  << "1Residuals from Run " << reportID_ << "\n";
  ts  << "         Baseline          Source   Date       Time             Obs del Del res   "
      << "Del er.     Obs rate   Rate res R.err  elev    azim      frq2-rat QC XS"
      << "    SNR /  SNS_S   Tau_obs_x       #AmbX   Tau_obs_s        #AmbS   Tau_apriori_clo  "
      << "Tau_theoretical   Tau_est_contrb     Rate_obs_x           Rate_obs_s"
      << "           Rate_apriori_clock   Rate_theoretical     Rate_estim_contrib     Eff. Dur.    "
      << "Res_gr_del_X      Res_gr_del_S     Gr_Spc_X   Gr_Spc_S  USR Ampltude    Phase    Obsind  DS  "
      << "Delay_residual\n";
  ts  << "                                                                     ps      ps       ps"
      << "          fs/s    fs/s     fs/s  deg deg deg deg                                    seconds"
      << "                 seconds                  seconds          seconds           seconds"
      << "            d/l                  d/l                  d/l                  d/l                  d/l"
      << "                     sec         sec               sec                 ns         ns     "
      << "d/l d/l         rad                     seconds\n";

  for (int idx=0; idx<session_->observations().size(); idx++)
  {
    SgVlbiObservation          *obs=session_->observations().at(idx);
    SgVlbiObservable           *o=obs?obs->activeObs():NULL;
    SgVlbiObservable           *o_ob=NULL;
    SgVlbiAuxObservation       *aux_1=NULL, *aux_2=NULL;
    aux_1 = obs?obs->auxObs_1():NULL;
    aux_2 = obs?obs->auxObs_2():NULL;
    
    double                      delay_clock=0.0, rate_clock=0.0;
    
    if (o && aux_1 && aux_2)
    {
      if (obs->observableByKey().contains(oppBandKey))
        o_ob = obs->observableByKey().value(oppBandKey);
      else
        o_ob = NULL;
      if (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        useChar1 = " ";
        useChar2 = " ";
      }
      else
      {
        useChar1 = ">";
        // no or bad opposite-band observable:
        if (isMultipleBand && 
              (o_ob==NULL || (o_ob->getQualityFactor() < config_->getQualityCodeThreshold())))
          useChar2 = "I";
        // bad quality of the active band observable:
        else if (o->getQualityFactor() < config_->getQualityCodeThreshold())
          useChar2 = "F";
        // everything else:
        else 
          useChar2 = "H";
      };
      if (o_ob)
      {
        sQcOb.sprintf("%2d", o_ob->getQualityFactor());
        dSnrOb = o_ob->getSnr();
      }
      else
      {
        sQcOb = "  ";
        dSnrOb = -1.0;
      };
//      dGrDelay = o->grDelay().getValue() + o->grDelay().ambiguity();
      dDelay = o->activeDelay()->getValue() + o->activeDelay()->ambiguity();
      // mimic FORTRAN's deficiency of data representation:
      if (dDelay<=-1.0 || dDelay>=10.0)
        sVal = "**************";
      else
        sVal.sprintf("%#14.0f", dDelay*1.0e12);
      
      str.sprintf("%7d%1s %8s/%8s %8s %s %s%#8.0f %1s %#6.0f%#14.0f%#8.0f %1s %#6.0f"
                  "%4d%4d%4d%4d  %9.6f %2d/%2s %6.1f / %6.1f"
                  " %17.13f %5d %17.13f  %5d"
                  "  %16.13f %16.13f %17.13f"
                  "  %20.12e %20.12e %20.12e %20.12e %20.12e @@ "
                  " %9.4f %17.8e %17.8e  %9.4f  %9.4f %3d %10.7f %9.5f  %6d %24.12e"
                  //
                  ,
        o->getMediaIdx() + 1, 
        qPrintable(useChar1), 
        qPrintable(obs->stn_1()->getKey()), 
        qPrintable(obs->stn_2()->getKey()),
        qPrintable(obs->src()->getKey()), 
        qPrintable(obs->toString(SgMJD::F_SOLVE_SPLFL_V2)),
        // delay:
        qPrintable(sVal),
        o->activeDelay()->getResidual()*1.0e12,
        qPrintable(useChar2),
        o->activeDelay()->sigma2Apply()*1.0e12,
        // delay rate:
        o->phDRate().getValue()*1.0e15,
        o->phDRate().getResidual()*1.0e15,
        qPrintable(useChar2),
        o->phDRate().getSigma()*1.0e15,
        //
        (int)round(aux_1->getElevationAngle()*RAD2DEG),
        (int)round(aux_2->getElevationAngle()*RAD2DEG),
        (int)round(aux_1->getAzimuthAngle()*RAD2DEG),
        (int)round(aux_2->getAzimuthAngle()*RAD2DEG),
        o->activeDelay()->getQ2() + 1.0,
        o->getQualityFactor(), 
        qPrintable(sQcOb),
        o->getSnr(), 
        dSnrOb,
        
        o->grDelay().getValue(),
        o->grDelay().getNumOfAmbiguities(),
        o_ob?o_ob->grDelay().getValue():0.0,
        o_ob?o_ob->grDelay().getNumOfAmbiguities():0,

        delay_clock,
        obs->theoDelay(),
        obs->sumAX_4delay(),
        // e:
        o->phDRate().getValue(),
        o_ob?o_ob->phDRate().getValue():0.0,
        rate_clock,
        obs->theoRate(),
        obs->sumAX_4rate(),
        // @@
        o->getEffIntegrationTime(),
        o->grDelay().getResidual(),
        o_ob?o->grDelay().getResidual():0.0,
        o->grDelay().getAmbiguitySpacing()*1.0e9,
        o_ob?o->grDelay().getAmbiguitySpacing()*1.0e9:-1,
        1,
        o->getCorrCoeff(),
        o->getTotalPhase(),
        obs->getMediaIdx() + 1,
        o->activeDelay()->getResidual()
        );


// Tau_obs_x       #AmbX   Tau_obs_s        #AmbS   Tau_apriori_clo  Tau_theoretical   Tau_est_contrb     Rate_obs_x
//           Rate_obs_s           Rate_apriori_clock   Rate_theoretical     Rate_estim_contrib     Eff. Dur.    Res_gr_del_X      Res_gr_del_S     Gr_Spc_X   Gr_Spc_S
//  USR Ampltude    Phase    Obsind  DS  Delay_residual




//      1> KOKEE   /WETTZELL 3C371    2014.11.18-18:30:31.0   -9421890080.   -295. F   125.    834105517.   -119. F   245.  19  50  21 329   1.076813  0/ 9    5.5 /   14.8
//      2  KOKEE   /WETTZELL 0529+483 2014.11.18-18:33:47.0   -5983218674.     66.      72.  -1626992487.      8.      37.  15  32 317  51   1.076494  9/ 9    9.6 /   20.2
//
//        mk5_merge_v03/progs/solve/cres/secnd.f:
//                      WRITE ( 23, 120 ) IOBS, IDLT, ISITN_CHR(ISITE(1)), &
//     &                  ISITN_CHR(ISITE(2)), ISTRN_CHR(ISTAR), STR_DATE(1:21), &
//     &                  FOBS*1.D3, DOC*1.D3, IPUNC, DOERR*1.D3, &
//     &                  ROBS*1.D3, ROC*1.D3, IPUNC,  RERR*1.D3, &
//     &                  IPELV, IPAZ, COEF_IONO, LQUAL_CHR, LQUALXS_CHR, &
//     &                  SNR, SNR_S
//
// 120                 FORMAT ( I7, A1, 1X, A8, "/", A8, 1X, A, 1X, A, 1X, &
//     &                        2(F14.0, F8.0, 1X, A1, 1X, F6.0), 4I4, 2X, F9.6, &
//     &                        1X, A2, '/', A2, 1X, F6.1, ' / ', F6.1 )
// 120                 FORMAT (
//  I7,             IOBS
//  A1, 1X,         IDLT
//  A8, "/",        ISITN_CHR(ISITE(1)),
//  A8, 1X,         ISITN_CHR(ISITE(2))
//  A, 1X,          ISTRN_CHR(ISTAR)
//  A, 1X,          STR_DATE(1:21),
//  2(F14.0, F8.0, 1X, A1, 1X, F6.0), 
//==FOBS*1.D3, DOC*1.D3, IPUNC, DOERR*1.D3,
//==ROBS*1.D3, ROC*1.D3, IPUNC,  RERR*1.D3,
//  4I4, 2X,        IPELV IPAZ arrays?
//  F9.6, 1X,       COEF_IONO, 
//  A2, '/',        LQUAL_CHR
//  A2, 1X,         LQUALXS_CHR
//  F6.1, ' / ',    SNR
//  F6.1            SNR_S
      ts << qPrintable(str) << "\n";
    };
  };
  
  ts << "\n";
};



//
void SgSolutionReporter::reportResidualsBlock_Output4Spoolfile_old(QTextStream& ts)
{
  QString                       str(""), useChar1(""), useChar2(""), sQcOb("");
  QString                       sVal("");
  QString                       oppBandKey("");
  double                        dSnrOb, dDelay;
  bool                          isMultipleBand;
  isMultipleBand = session_->bands().size() > 1;
  for (int i=0; i<session_->bands().size(); i++)
    if (session_->bands().at(i)->getKey() != activeBandKey_)
      oppBandKey = session_->bands().at(i)->getKey();
  //
  //
  ts  << "1Residuals from Run " << reportID_ << "\n";
  ts  << "          Baseline        Source      Date       Time            Obs del  Del res  Del err"
      << "    Obs rate    Rate res   Rate err   elev     frq2-rat QC XS SNR  / SNS_S\n"
      << "                                                                      ps        ps      ps"
      << "          fs/s     fs/s   fs/s  deg\n";
  for (int idx=0; idx<session_->observations().size(); idx++)
  {
    SgVlbiObservation          *obs=session_->observations().at(idx);
    SgVlbiObservable           *o=obs?obs->activeObs():NULL;
    SgVlbiObservable           *o_ob=NULL;
    SgVlbiAuxObservation       *aux_1=NULL, *aux_2=NULL;
    aux_1 = obs?obs->auxObs_1():NULL;
    aux_2 = obs?obs->auxObs_2():NULL;
    if (o && aux_1 && aux_2)
    {
      if (obs->observableByKey().contains(oppBandKey))
        o_ob = obs->observableByKey().value(oppBandKey);
      else
        o_ob = NULL;
      if (o->activeDelay() && o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        useChar1 = " ";
        useChar2 = " ";
      }
      else
      {
        useChar1 = ">";
        // no or bad opposite-band observable:
        if (isMultipleBand && 
              (o_ob==NULL || (o_ob->getQualityFactor() < config_->getQualityCodeThreshold())))
          useChar2 = "I";
        // bad quality of the active band observable:
        else if (o->getQualityFactor() < config_->getQualityCodeThreshold())
          useChar2 = "F";
        // everything else:
        else 
          useChar2 = "H";
      };
      if (o_ob)
      {
        sQcOb.sprintf("%2d", o_ob->getQualityFactor());
        dSnrOb = o_ob->getSnr();
      }
      else
      {
        sQcOb = "  ";
        dSnrOb = -1.0;
      };
//      dGrDelay = o->grDelay().getValue() + o->grDelay().ambiguity();
      dDelay = o->activeDelay()->getValue() + o->activeDelay()->ambiguity();
      // mimic FORTRAN's deficiency of data representation:
      if (dDelay<=-1.0 || dDelay>=10.0)
        sVal = "**************";
      else
        sVal.sprintf("%#14.0f", dDelay*1.0e12);

      str.sprintf("%7d%1s %8s/%8s %8s %s %s%#8.0f %1s %#6.0f%#14.0f%#8.0f %1s %#6.0f"
                  "%4d%4d%4d%4d  %9.6f %2d/%2s %6.1f / %6.1f",
        o->getMediaIdx() + 1, qPrintable(useChar1),
        qPrintable(obs->stn_1()->getKey()),
        qPrintable(obs->stn_2()->getKey()),
        qPrintable(obs->src()->getKey()),
        qPrintable(obs->toString(SgMJD::F_SOLVE_SPLFL_V2)),
        // delay:
        qPrintable(sVal),
        o->activeDelay()->getResidual()*1.0e12,
        qPrintable(useChar2),
        o->activeDelay()->sigma2Apply()*1.0e12,
        // delay rate:
        o->phDRate().getValue()*1.0e15,
        o->phDRate().getResidual()*1.0e15,
        qPrintable(useChar2),
        o->phDRate().getSigma()*1.0e15,
        //
        (int)round(aux_1->getElevationAngle()*RAD2DEG),
        (int)round(aux_2->getElevationAngle()*RAD2DEG),
        (int)round(aux_1->getAzimuthAngle()*RAD2DEG),
        (int)round(aux_2->getAzimuthAngle()*RAD2DEG),
        o->activeDelay()->getQ2() + 1.0,
        o->getQualityFactor(), qPrintable(sQcOb),
        o->getSnr(), dSnrOb
        );
//      1> KOKEE   /WETTZELL 3C371    2014.11.18-18:30:31.0   -9421890080.   -295. F   125.    834105517.   -119. F   245.  19  50  21 329   1.076813  0/ 9    5.5 /   14.8
//      2  KOKEE   /WETTZELL 0529+483 2014.11.18-18:33:47.0   -5983218674.     66.      72.  -1626992487.      8.      37.  15  32 317  51   1.076494  9/ 9    9.6 /   20.2
//
//        mk5_merge_v03/progs/solve/cres/secnd.f:
//                      WRITE ( 23, 120 ) IOBS, IDLT, ISITN_CHR(ISITE(1)), &
//     &                  ISITN_CHR(ISITE(2)), ISTRN_CHR(ISTAR), STR_DATE(1:21), &
//     &                  FOBS*1.D3, DOC*1.D3, IPUNC, DOERR*1.D3, &
//     &                  ROBS*1.D3, ROC*1.D3, IPUNC,  RERR*1.D3, &
//     &                  IPELV, IPAZ, COEF_IONO, LQUAL_CHR, LQUALXS_CHR, &
//     &                  SNR, SNR_S
//
// 120                 FORMAT ( I7, A1, 1X, A8, "/", A8, 1X, A, 1X, A, 1X, &
//     &                        2(F14.0, F8.0, 1X, A1, 1X, F6.0), 4I4, 2X, F9.6, &
//     &                        1X, A2, '/', A2, 1X, F6.1, ' / ', F6.1 )
// 120                 FORMAT (
//  I7,             IOBS
//  A1, 1X,         IDLT
//  A8, "/",        ISITN_CHR(ISITE(1)),
//  A8, 1X,         ISITN_CHR(ISITE(2))
//  A, 1X,          ISTRN_CHR(ISTAR)
//  A, 1X,          STR_DATE(1:21),
//  2(F14.0, F8.0, 1X, A1, 1X, F6.0), 
//==FOBS*1.D3, DOC*1.D3, IPUNC, DOERR*1.D3,
//==ROBS*1.D3, ROC*1.D3, IPUNC,  RERR*1.D3,
//  4I4, 2X,        IPELV IPAZ arrays?
//  F9.6, 1X,       COEF_IONO, 
//  A2, '/',        LQUAL_CHR
//  A2, 1X,         LQUALXS_CHR
//  F6.1, ' / ',    SNR
//  F6.1            SNR_S
      ts << qPrintable(str) << "\n";
    };
  };
  
  ts << "\n";
};



//
void SgSolutionReporter::reportMetStatsBlock_Output4Spoolfile(QTextStream& ts)
{
  ts  << "  Met Statistics:\n"
      << "                    Temperature      Pressure        Humidity\n"
      <<"   Station         average   rms   average   rms   average   rms\n";

  QString                       str;
  StationsByName_it             it;
  it=session_->stationsByName().begin();

  for (it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    str.sprintf("   %-8s  MET",
      qPrintable(si->getKey()));
    ts << str;

    int                         num;
    double                      sumT_1, sumT_2, sumP_1, sumP_2, sumH_1, sumH_2;
    double                      rmsT, rmsP, rmsH, d;
    sumT_1 = sumT_2 = sumP_1 = sumP_2 = sumH_1 = sumH_2 = 0.0;
    rmsT = rmsP = rmsH = 0.0;
    num = 0;
//    QMap<SgMJD, SgVlbiAuxObservation*>::iterator jt;
//    for (jt=si->auxObservationByEpoch()->begin(); jt!=si->auxObservationByEpoch()->end(); ++jt)
    QMap<QString, SgVlbiAuxObservation*>::iterator jt;
    for (jt=si->auxObservationByScanId()->begin(); jt!=si->auxObservationByScanId()->end(); ++jt)
    {
      SgVlbiAuxObservation        *auxObs=jt.value();
      if (!auxObs->getMeteoData().isAttr(SgMeteoData::Attr_ARTIFICIAL_DATA))
      {
        double      t, p, h;
        t = auxObs->getMeteoData().getTemperature();
        p = auxObs->getMeteoData().getPressure();
        h = auxObs->getMeteoData().getRelativeHumidity()*100.0;
        sumT_1 += t;
        sumT_2 += t*t;
        sumP_1 += p;
        sumP_2 += p*p;
        sumH_1 += h;
        sumH_2 += h*h;
        num++;
      };
    };
    if (num>0)
    {
      d = sumT_2 - sumT_1*sumT_1/num;
      if (d<=0.0 && fabs(d)/sumT_2<1.0e-12)
        rmsT = 0.0;
      else
        rmsT = sqrt(d/num);
      d = sumP_2 - sumP_1*sumP_1/num;
      if (d<=0.0 && fabs(d)/sumP_2<1.0e-12)
        rmsP = 0.0;
      else
        rmsP = sqrt(d/num);
      d = sumH_2 - sumH_1*sumH_1/num;
      if (d<=0.0 && fabs(d)/sumH_2<1.0e-12)
        rmsH = 0.0;
      else
        rmsH = sqrt(d/num);
      str.sprintf("   %6.1f  %6.1f  %6.1f  %6.1f%8.1f %7.1f",
        sumT_1/num, rmsT, sumP_1/num, rmsP, sumH_1/num, rmsH);
    }
    else
      str = "   -999.0     0.0  -999.0     0.0-99900.0     0.0";
    ts << str << "\n";
  };
  ts << "\n";
};



//
void SgSolutionReporter::reportGeneralStatsBlock_Output4Spoolfile(QTextStream& ts)
{
  const int                     qCodeLimit(config_->getQualityCodeThreshold());
  QString                       str;
  int                           numRecObs, numRec59Obs, numProcdObs, num;
  int                           nYear, nMonth, nDay, nHour, nMin;
  int                           numOfBands;
  double                        dSec, fInterval;
  int                           numProcRateObs;
  double                        rmsDelay(0.0), nrmsDelay(0.0), chi2Delay(0.0);
  double                        rmsRate(0.0),  nrmsRate(0.0),  chi2Rate(0.0);
  double                        nrmsCombined(0.0),  chi2Combined(0.0);
  double                        sum1d, sum2d;
  double                        sum1r, sum2r;
  SgMJD                         t(session_->getTStart());
  DataType                      dType=config_->getUseDelayType()!=SgTaskConfig::VD_NONE?DT_DELAY:DT_RATE;
  numOfBands = session_->bands().size();
  
//pBand = session_->primaryBand();
  SgMJD::MJD_reverse(t.getDate(), t.getTime(),  nYear, nMonth, nDay, nHour, nMin, dSec);
  fInterval = session_->getTFinis() - t;

  numProcdObs = activeBand_->numProcessed(dType);
  numRecObs = numProcRateObs = numRec59Obs = 0;
  numRecObs = session_->primaryBand()->numUsable(DT_DELAY);
  // clear "num of recoverable obs" per object values:
  /*
  for (StationsByName_it it=activeBand_->stationsByName().begin(); 
    it!=activeBand_->stationsByName().end(); ++it)
    it.value()->setNumUsable(DT_DELAY, 0);
  for (BaselinesByName_it it=activeBand_->baselinesByName().begin(); 
    it!=activeBand_->baselinesByName().end(); ++it)
    it.value()->setNumUsable(DT_DELAY, 0);
  for (SourcesByName_it it=activeBand_->sourcesByName().begin(); 
    it!=activeBand_->sourcesByName().end(); ++it)
    it.value()->setNumUsable(DT_DELAY, 0);
  */
  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    bool                        isOk=obs->observableByKey().size()==numOfBands;
    bool                        isOk59=obs->observableByKey().size()==numOfBands;
   
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin();
      it!=obs->observableByKey().end(); ++it)
    {
      int                       qCode=it.value()->getQualityFactor();
      isOk = isOk && qCode>=qCodeLimit;
      isOk59 = isOk59 && qCode>=5;
    };
/*      
    if (isOk)
    {
      numRecObs++;
      int                       idx;
      idx = obs->getStation1Idx();
      if (activeBand_->stationsByIdx().contains(idx))
        activeBand_->stationsByIdx().find(idx).value()->incNumUsable(DT_DELAY);
        
      idx = obs->getStation2Idx();
      if (activeBand_->stationsByIdx().contains(idx))
        activeBand_->stationsByIdx().find(idx).value()->incNumUsable(DT_DELAY);
        
      idx = obs->getSourceIdx();
      if (activeBand_->sourcesByIdx().contains(idx))
        activeBand_->sourcesByIdx().find(idx).value()->incNumUsable(DT_DELAY);
        
      idx = obs->getBaselineIdx();
      if (activeBand_->baselinesByIdx().contains(idx))
        activeBand_->baselinesByIdx().find(idx).value()->incNumUsable(DT_DELAY);
    };
*/
    if (isOk59)
      numRec59Obs++;
  };
  sum1r = sum2r = sum1d = sum2d = 0.0;
  num = 0;
  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    if (obs->primeObs() && obs->primeObs()->activeDelay() && 
        obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      sum1d += obs->getNormalizedDelayResidual();
      sum2d += obs->getNormalizedDelayResidual()*obs->getNormalizedDelayResidual();
      sum1r += obs->getNormalizedRateResidual();
      sum2r += obs->getNormalizedRateResidual()*obs->getNormalizedRateResidual();
      num++;
    };
  };
  if (num != numProcdObs)
    std::cout << "num != numProcdObs: " << num << " vs " << numProcdObs << "\n";
  rmsDelay = activeBand_->wrms(dType)*1.0E12;
  nrmsDelay = sqrt((sum2d - sum1d*sum1d/num)/num);
//chi2Delay = activeBand_->chi2(dType)/session_->getNumOfDOF();
  chi2Delay = activeBand_->reducedChi2(dType);
  
  if (config_->getUseRateType()!=SgTaskConfig::VR_NONE)
  {
    numProcRateObs  = activeBand_->numProcessed(DT_RATE);
    rmsRate         = activeBand_->wrms(DT_RATE)*1.0e15;
    nrmsRate        = sqrt((sum2r - sum1r*sum1r/num)/num);
//  chi2Rate        = activeBand_->chi2(DT_RATE)/dof;
    chi2Rate        = activeBand_->reducedChi2(DT_RATE);
  };
  
  str.sprintf(" Run %10s      %7d Observation Pairs Available ",
    qPrintable(reportID_), session_->observations().size());
  ts << str << "\n";

  str.sprintf(" Session started on:    %14.6f   %04d.%02d.%02d %02d:%02d:%06.3f  UTC",
    t.toDouble() + 2400000.5,
    nYear, nMonth, nDay, nHour, nMin, dSec);
  ts << str << "\n";

  nDay = (int)trunc(fInterval);
  fInterval = fInterval - nDay;

  fInterval *= DAY2SEC;
  nHour = (int)(fInterval/3600.0);
  nMin  = (int)((fInterval - 3600.0*nHour)/60.0);
  dSec  = fInterval - 3600.0*nHour - 60.0*nMin;
  str.sprintf(" Actual duration:       %9.3f  sec           %02d %02d:%02d:%06.3f  sec",
    fInterval + nDay*DAY2SEC, nDay, nHour, nMin, dSec);
  ts << str << "\n";

  str.sprintf(" Solution type: %-17s    ",
    qPrintable(solutionTypeName_));
  ts << str << "\n\n";
  
  ts <<
    " Data Type     Number of   Weighted RMS    Normalized RMS   Chi Square"   << "\n"
    "             Observations    Residual         Residual      (precis)"     << "\n"
    "                 Used"                                                    << "\n"
    " "                                                                        << "\n";

  str.sprintf("   Delay %8d %18.3f ps %17.2f %12.4f",
    numProcdObs, rmsDelay, nrmsDelay, chi2Delay);
  ts << str << "\n";

  str.sprintf("   Rate  %8d %18g fs/s %15.2f %12.4f",
    numProcRateObs, rmsRate, nrmsRate, chi2Rate);
  ts << str << "\n";

  str.sprintf("Combined %8d                         %15.2f %12.4f",
    0, nrmsCombined, chi2Combined);
  ts << str << "\n";
  
  ts  << "----------------------------------------------------------------------- \n";
  //    << " CRES mode:                                    ??????                   \n"
  //    << " Suppression method in use:                    ?????????????            \n";

  str.sprintf(" Used quality_code_limit:                                %3d            ",
    qCodeLimit);
  ts << str << "\n";

  str.sprintf(" Number of potentially recoverable observations:    %8d            ",
    numRecObs);
  ts << str << "\n";

  str.sprintf(" Number of potentially good observations with QC 5-9:%7d           ",
    numRec59Obs);
  ts << str << "\n";

  str.sprintf(" Number of used observations:                       %8d  (%6.2f%%) ",
    numProcdObs, numProcdObs*100.0/numRecObs);
  ts << str << "\n";

  str.sprintf(" Number of suppressed observations:                 %8d  (%6.2f%%) ",
    (numRecObs - numProcdObs), (numRecObs - numProcdObs)*100.0/numRecObs);
  ts << str << "\n";

  ts << "----------------------------------------------------------------------- \n\n\n\n";
};



//
void SgSolutionReporter::reportBaselineStatsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString             str;
  // print a header:
  ts  
      << " Baseline Statistics\n"
      << "      Baseline        # W.Obs   W.RMS Del   N.R.D.   N.R.D. W.RMS Rate   N.R.R.  "
      << "D.RW   R.RW \n"
      << "                    used/recov     ps     standard ( 15ps+i)  fs/s                "
      << "ps    fs/s  \n\n";

  // colect a list of "supressed" stations:
  QMap<QString, SgVlbiStationInfo*>            skippedStationsByName;
  QMap<QString, SgVlbiStationInfo*>::iterator  itSi;
  for (itSi=session_->stationsByName().begin(); itSi!=session_->stationsByName().end(); ++itSi)
  {
    SgVlbiStationInfo       *si=itSi.value();
    if (si->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
      skippedStationsByName.insert(si->getKey(), si);
  };
  // report baselines' statistics:
  QList<SgVlbiBaselineInfo*>                    skippedBaselines;
  QMap<QString, SgVlbiBaselineInfo*>::iterator  it;
  for (it=activeBand_->baselinesByName().begin(); it!=activeBand_->baselinesByName().end(); ++it)
  {
    SgVlbiBaselineInfo       *bi=it.value();
    QString                   biName=bi->getKey();
    QString                   st1Name=biName.left(8);
    QString                   st2Name=biName.right(8);
    if (!session_->baselinesByName()[bi->getKey()]->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID) &&
        !skippedStationsByName.contains(st1Name)        &&
        !skippedStationsByName.contains(st2Name)         )
    {
//          1         2         3         4         5         6         7         8         9        10
//01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
// HOBART12-KATH12M   967/ 1004      37       0.97      1.38     202        5.40     29      0
// HOBART12-YARRA12M  947/  982      42       0.97      1.52     227        5.19     36      0
// KATH12M -YARRA12M 1009/ 1029      32       0.95      1.24     216        4.74     26      0

      str.sprintf(" %-17s%5d/%5d%8.1f  %9.2f %9.2f %7.1f  %10.2f%7.1f%7.1f",
        qPrintable(biName.replace(8, 1, "-")),
        bi->numProcessed(DT_DELAY), bi->numUsable(DT_DELAY),
        bi->wrms(DT_DELAY)*1.0E12,

//        bi->getNrmRes(),
        bi->getSFF_NrmRes(DT_DELAY),
        bi->getSFF_NrmRes_plus_15(DT_DELAY),
//        0.0,    // nrd (15ps+i) ?
        bi->wrms(DT_RATE)*1.0E15,           // wrms rate
        bi->getSFF_NrmRes(DT_RATE),         // nrr
        bi->getSigma2add(DT_DELAY)*1.0E12,
        bi->getSigma2add(DT_RATE )*1.0E15   // rate's sigma2add
        );
      ts << str << "\n";
    }
    else
      skippedBaselines.append(bi);
  };
  ts << "\n\n";
  
  if (skippedBaselines.size())
  {
    ts << " Not included:                                                                          \n\n";
    for (int i=0; i<skippedBaselines.size(); i++)
    {
      SgVlbiBaselineInfo       *bi=skippedBaselines.at(i);
      QString                   biName=bi->getKey();
      str.sprintf(" %-17s%5d/%5d    deselected",
        qPrintable(biName.replace(8, 1, "-")),
        bi->numProcessed(DT_DELAY), bi->numUsable(DT_DELAY));
      ts << str << "\n";
    };
  ts << "\n\n";
  };
  // clear aux stuff:
  skippedStationsByName.clear();
  skippedBaselines.clear();
};



//
void SgSolutionReporter::reportSourceStatsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString             str;
  // print a header:
  ts
      << " Source Statistics \n"
      << "     Source           # W.Obs   W.RMS Del   N.R.D.   N.R.D. W.RMS Rate   N.R.R. \n"
      << "                                   ps     standard  ( 15ps)   fs/s  \n \n";

  usedSources_.clear();
  skippedSources_.clear();
  QMap<QString, SgVlbiSourceInfo*>::iterator  it;
  for (it=activeBand_->sourcesByName().begin(); it!=activeBand_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo         *si=it.value();
    if (!session_->sourcesByName()[si->getKey()]->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
      usedSources_.append(si);
    else
      skippedSources_.append(si);
  };
  // sort in RA order:
  qSort(usedSources_.begin(),    usedSources_.end(),    rightAscensionSortingOrderLessThan);
  qSort(skippedSources_.begin(), skippedSources_.end(), rightAscensionSortingOrderLessThan);
  // report sources' statistics:
  for (int i=0; i<usedSources_.size(); i++)
  {
    //     0059+581 A     58/  60     655        0.33    29.76     195        0.77
    SgVlbiSourceInfo         *si=usedSources_.at(i);
    str.sprintf(" %12s %1s   %5d/%5d%8.1f   %9.2f %8.2f %7.1f  %10.2f",
      qPrintable(si->getKey()), qPrintable(srcChars[i % numOfSrcChars]),
      si->numProcessed(DT_DELAY), si->numUsable(DT_DELAY),// numbers of observations, processed and total
      si->wrms(DT_DELAY)*1.0E12,                          // WRMS delay
      si->getSFF_NrmRes(DT_DELAY),                        // nomalized residuals
      si->getSFF_NrmRes_plus_15(DT_DELAY),                // nomalized residuals with fixed aux. weight
      si->wrms(DT_RATE)*1.0E15,                           // wrms rate
      si->getSFF_NrmRes(DT_RATE)                          // nrr
      );
    ts << str << "\n";
  };
  ts << "\n\n";
  
  if (skippedSources_.size())
  {
    ts << " Not included:                                                                          \n\n";
    for (int i=0; i<skippedSources_.size(); i++)
    {
      SgVlbiSourceInfo         *si=skippedSources_.at(i);
      str.sprintf(" %-10s%5d/%5d    deselected",
        qPrintable(si->getKey()),
        si->numProcessed(DT_DELAY), si->numUsable(DT_DELAY));
      ts << str << "\n";
    };
  ts << "\n\n";
  };
  //
  // at the end, collect stats from session level:
  usedSources_.clear();
  skippedSources_.clear();
  for (it=session_->sourcesByName().begin(); it!=session_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo         *si=it.value();
    if (!si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID))
      usedSources_.append(si);
    else
      skippedSources_.append(si);
  };
  // sort in RA order:
  qSort(usedSources_.begin(),    usedSources_.end(),    rightAscensionSortingOrderLessThan);
  qSort(skippedSources_.begin(), skippedSources_.end(), rightAscensionSortingOrderLessThan);
};



//
void SgSolutionReporter::reportStationStatsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString             str;
  // print a header:
  ts
      << " Station Statistics \n"
      << "     Station          # W.Obs   W.RMS Del   N.R.D.   N.R.D. W.RMS Rate   N.R.R. \n"
      << "                                   ps     standard  ( 15ps)   fs/s   \n \n";

//pBand = session_->primaryBand();
  // report stations' statistics:
  QList<SgVlbiStationInfo*>                   skippedStations;
  QMap<QString, SgVlbiStationInfo*>::iterator it;
  for (it=activeBand_->stationsByName().begin(); it!=activeBand_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo                        *si=it.value();
    if (!session_->stationsByName()[si->getKey()]->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      str.sprintf(" %12s     %5d/%5d%8.1f   %9.2f %8.2f %7.1f  %10.2f",
        qPrintable(si->getKey()),
        si->numProcessed(DT_DELAY), si->numUsable(DT_DELAY),
        si->wrms(DT_DELAY)*1.0E12,
//        si->getNrmRes(),
        si->getSFF_NrmRes(DT_DELAY),
        si->getSFF_NrmRes_plus_15(DT_DELAY),
//        0.0,                                // nrd (15ps+i) ?
        si->wrms(DT_RATE)*1.0E15,             // wrms rate
        si->getSFF_NrmRes(DT_RATE)            // nrr
        );
      ts << str << "\n";
    }
    else
      skippedStations.append(si);
  };
  ts << "\n\n";
  
  if (skippedStations.size())
  {
    ts << " Not included:                                                                          \n\n";
    for (int i=0; i<skippedStations.size(); i++)
    {
      SgVlbiStationInfo        *si=skippedStations.at(i);
      str.sprintf(" %-10s%5d/%5d    deselected",
        qPrintable(si->getKey()),
        si->numProcessed(DT_DELAY), si->numUsable(DT_DELAY));
      ts << str << "\n";
    };
  ts << "\n\n";
  };
  // clear aux stuff:
  skippedStations.clear();
};



//
void SgSolutionReporter::reportAPrioriBlock_Output4Spoolfile(QTextStream& ts)
{
  QString                       path2AF;
  if (path2APrioriFiles_.at(0)=='/')
    path2AF = path2APrioriFiles_;
  else
    path2AF = identities_.getCurrentDir() + "/" + path2APrioriFiles_;
  
  ts
      << "                       *** Flyby Status ***\n\n"
      << "                    directory: " << qPrintable(path2AF) << "\n\n"
      << "Station   Source    Nutation  Nutation  Earth     Earth     Station   "
      << "Pressure  EOP Intp. EOP Intp. High Freq Axis      \n"
      << "Positions Positions Model     Time      Rotation  Rotation  Velocity  "
      << "Loading   Smoothing Smoothing EOP       Offset    \n"
      << "                              Series    Series    Interpol. Model     "
      << "          CALC      Mod File  Model     Mod File  \n"
      << "--------- --------- --------- --------- --------- --------- --------- "
      << "--------- --------- --------- --------- ---------\n";
  // Station Positions
  ts  << qPrintable((config_->getUseExtAPrioriSitesPositions() ? 
    config_->getExtAPrioriSitesPositionsFileName() : "NONE").leftJustified(9, ' ', true)) << " ";
  // Source Positions
  ts  << qPrintable((config_->getUseExtAPrioriSourcesPositions() ? 
    config_->getExtAPrioriSourcesPositionsFileName() : "NONE").leftJustified(9, ' ', true)) << " ";
  // Nutation Model
  ts  << qPrintable(QString("NONE").leftJustified(9, ' ', true)) << " ";
  // Nutation Time Series
  ts  << qPrintable(QString("NONE").leftJustified(9, ' ', true)) << " ";
  // Earth Rotation Series
  ts  << qPrintable((config_->getUseExtAPrioriErp() ? 
    config_->getExtAPrioriErpFileName() : "NONE").leftJustified(9, ' ', true)) << " ";
  // Earth Rotation Interpol.
  ts  << qPrintable((config_->getUseExtAPrioriErp() ?
    QString("C. Spline") : "NONE").leftJustified(9, ' ', true)) << " ";
  // Station Velocity Model
  ts  << qPrintable((config_->getUseExtAPrioriSitesVelocities() ? 
    config_->getExtAPrioriSitesVelocitiesFileName() : "NONE").leftJustified(9, ' ', true)) << " ";
  // Pressure Loading
  ts  << qPrintable(QString("NONE").leftJustified(9, ' ', true)) << " ";
  // EOP Intp. Smoothing CALC
  ts  << qPrintable(QString("NO_ZONAL").leftJustified(9, ' ', true)) << " ";
  // EOP Intp. Smoothing Mod File
  ts  << qPrintable(QString("N/A").leftJustified(9, ' ', true)) << " ";
  // High Freq EOP Model
  ts  << qPrintable((config_->getUseExtAPrioriHiFyErp() ? 
    config_->getExtAPrioriHiFyErpFileName() : "NONE").leftJustified(9, ' ', true)) << " ";
  // Axis Offset Mod File
  ts  << qPrintable((config_->getUseExtAPrioriAxisOffsets() ? 
    config_->getExtAPrioriAxisOffsetsFileName() : "NONE").leftJustified(9, ' ', true)) << " \n\n";
  //
  ts  << "Site Plate Map: " << qPrintable(path2AF) << "/sitpl.dat           \n\n";  
};



//
void SgSolutionReporter::reportEstimationBlock_Output4Spoolfile(QTextStream& ts)
{
  static const double     clcScaleVals[10] = 
    {1.0e09,         1.0e14/86400.0, 1.0e14/86400.0, 1.0e14/86400.0, 1.0e14/86400.0,
     1.0e14/86400.0, 1.0e14/86400.0, 1.0e14/86400.0, 1.0e14/86400.0, 1.0e14/86400.0};
  static const QString    clcScaleNames[10] = 
    {"ns        ", "D-14      ", "D-14/day  ", "D-14/day^2", "D-14/day^3",
     "D-14/day^4", "D-14/day^5", "D-14/day^6", "D-14/day^7", "D-14/day^8"};
 
  QString                       str;
  int                           idx;
  SgPwlStorage                 *pwl=NULL;
  double                        chi;
  DataType                      dType=config_->getUseDelayType()!=SgTaskConfig::VD_NONE?DT_DELAY:DT_RATE;

//  if (session_->primaryBand()->getChi2()>0.0)
//    chi = sqrt(session_->primaryBand()->getChi2()/session_->primaryBand()->getNumOfDOF());
  if (activeBand_->chi2(dType) > 0.0)
//  chi = sqrt(activeBand_->chi2(dType)/session_->getNumOfDOF());
    chi = sqrt(activeBand_->reducedChi2(dType));
  else
    chi = 1.0; // do not scale at all
  //  SgArcStorage           *arc=NULL;
 
  ts 
      << " Parameter adjustments for run " << qPrintable(reportID_) 
      << " User=" << qPrintable(identities_.getUserDefaultInitials()) << "\n"
      << " Reference epoch for polynomial models: " 
      << qPrintable(session_->tRefer().toString(SgMJD::F_SOLVE_SPLFL_SHORT)) << "\n"
      << "                                        Parameter"
      << "               Adjustment              a-sigma              m-sigma\n";

  StationsByName_it     it_st;
  // reference clocks:
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
      (si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) || si->getClocksModelOrder()==0))
    {
      SgMJD                     t=*si->auxObservationByScanId()->begin().value();
      //      WESTFORD CLCK 12/06/21 18:29 Reference
      str.sprintf("      %8s CLCK %s Reference",
        qPrintable(si->getKey()),
        qPrintable(t.toString(SgMJD::F_SOLVE_SPLFL_SHORT)));
      ts << str << "\n";
    };
  };
  
  idx = 1;
  // stations:
  Sg3dMatrixR                   W(EAST), V(NORTH);
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      // coordinates:
      if (si->pRx()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        double                    cXY, cXZ, cYZ;
        double                    latitude, longitude, height;
        Sg3dVector                r, rs;
        Sg3dVector                r_apriori(si->getR());
        Sg3dMatrix                m3M;
        SgMJD                     t(si->pRx()->getTMean());
        if (config_->getUseExtAPrioriSitesPositions())
          r_apriori = si->getR_ea() + si->getV_ea()*(t - session_->getApStationVelocities().getT0());

        geocentric2geodetic(r_apriori, latitude, longitude, height, config_->getIsSolveCompatible());
        m3M = W(-latitude)*V(longitude);

        // covariances (i.e., rho_{1,2}*sigma_1*sigma_2):
        cXY = PxAll_->getElement(si->pRx()->getIdx(), si->pRy()->getIdx());
        cXZ = PxAll_->getElement(si->pRx()->getIdx(), si->pRz()->getIdx());
        cYZ = PxAll_->getElement(si->pRy()->getIdx(), si->pRz()->getIdx());
      
        // evaluate displacement in VEN:
        r  = m3M*Sg3dVector(si->pRx()->getSolution(), si->pRy()->getSolution(), si->pRz()->getSolution());
        // evaluate std. deviations:
        rs(VERTICAL) = sqrt( 
          m3M.at(VERTICAL, X_AXIS)*m3M.at(VERTICAL, X_AXIS)*si->pRx()->getSigma()*si->pRx()->getSigma() +
          m3M.at(VERTICAL, Y_AXIS)*m3M.at(VERTICAL, Y_AXIS)*si->pRy()->getSigma()*si->pRy()->getSigma() +
          m3M.at(VERTICAL, Z_AXIS)*m3M.at(VERTICAL, Z_AXIS)*si->pRz()->getSigma()*si->pRz()->getSigma() +
          2.0*(m3M.at(VERTICAL, X_AXIS)*m3M.at(VERTICAL, Y_AXIS)*cXY +
              m3M.at(VERTICAL, X_AXIS)*m3M.at(VERTICAL, Z_AXIS)*cXZ +
              m3M.at(VERTICAL, Y_AXIS)*m3M.at(VERTICAL, Z_AXIS)*cYZ
              )
          );
        rs(EAST) = sqrt( 
          m3M.at(EAST, X_AXIS)*m3M.at(EAST, X_AXIS)*si->pRx()->getSigma()*si->pRx()->getSigma() +
          m3M.at(EAST, Y_AXIS)*m3M.at(EAST, Y_AXIS)*si->pRy()->getSigma()*si->pRy()->getSigma() +
          m3M.at(EAST, Z_AXIS)*m3M.at(EAST, Z_AXIS)*si->pRz()->getSigma()*si->pRz()->getSigma() +
          2.0*(m3M.at(EAST, X_AXIS)*m3M.at(EAST, Y_AXIS)*cXY +
               m3M.at(EAST, X_AXIS)*m3M.at(EAST, Z_AXIS)*cXZ +
               m3M.at(EAST, Y_AXIS)*m3M.at(EAST, Z_AXIS)*cYZ
              )
          );
        rs(NORTH) = sqrt( 
          m3M.at(NORTH, X_AXIS)*m3M.at(NORTH, X_AXIS)*si->pRx()->getSigma()*si->pRx()->getSigma() +
          m3M.at(NORTH, Y_AXIS)*m3M.at(NORTH, Y_AXIS)*si->pRy()->getSigma()*si->pRy()->getSigma() +
          m3M.at(NORTH, Z_AXIS)*m3M.at(NORTH, Z_AXIS)*si->pRz()->getSigma()*si->pRz()->getSigma() +
          2.0*(m3M.at(NORTH, X_AXIS)*m3M.at(NORTH, Y_AXIS)*cXY +
               m3M.at(NORTH, X_AXIS)*m3M.at(NORTH, Z_AXIS)*cXZ +
               m3M.at(NORTH, Y_AXIS)*m3M.at(NORTH, Z_AXIS)*cYZ
              )
          );
      
        ts  << "Station positions are for epoch: "
            << qPrintable(si->pRx()->getTMean().toString(SgMJD::F_SOLVE_SPLFL)) << "  "
            << "Stn=[" << qPrintable(si->getKey()) << "]"
            << "\n";
        str.sprintf("%5d. %-8s %04d %4s  X Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          idx, qPrintable(si->getKey()), si->getCdpNumber(), qPrintable(si->getTectonicPlateName()),
          (r_apriori.at(X_AXIS) + si->pRx()->getSolution())*1000.0,
          si->pRx()->getSolution()*1000.0,
          si->pRx()->getSigma()*1000.0, 
          si->pRx()->getSigma()*1000.0*chi );
        ts << str << "\n";
        idx++;

        str.sprintf("%5d. %-8s %04d %4s  Y Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          idx, qPrintable(si->getKey()), si->getCdpNumber(), qPrintable(si->getTectonicPlateName()),
          (r_apriori.at(Y_AXIS) + si->pRy()->getSolution())*1000.0,
          si->pRy()->getSolution()*1000.0, 
          si->pRy()->getSigma()*1000.0, 
          si->pRy()->getSigma()*1000.0*chi );
        ts << str << "\n";
        idx++;

        str.sprintf("%5d. %-8s %04d %4s  Z Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          idx, qPrintable(si->getKey()), si->getCdpNumber(), qPrintable(si->getTectonicPlateName()),
          (r_apriori.at(Z_AXIS) + si->pRz()->getSolution())*1000.0,
          si->pRz()->getSolution()*1000.0,
          si->pRz()->getSigma()*1000.0, 
          si->pRz()->getSigma()*1000.0*chi );
        ts << str << "\n";
        idx++;

        // the same but in VEN:
        str.sprintf("       %-8s %04d       U Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          qPrintable(si->getKey()), si->getCdpNumber(), 
          r.at(VERTICAL)*1000.0,
          r.at(VERTICAL)*1000.0,
          rs.at(VERTICAL)*1000.0,
          rs.at(VERTICAL)*1000.0*chi);
        ts << str << "\n";
        str.sprintf("       %-8s %04d       E Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          qPrintable(si->getKey()), si->getCdpNumber(), 
          r.at(EAST)*1000.0,
          r.at(EAST)*1000.0,
          rs.at(EAST)*1000.0,
          rs.at(EAST)*1000.0*chi);
        ts << str << "\n";
        str.sprintf("       %-8s %04d       N Comp      %14.2f mm %14.3f mm     %14.3f mm     %14.3f mm",
          qPrintable(si->getKey()), si->getCdpNumber(), 
          r.at(NORTH)*1000.0,
          r.at(NORTH)*1000.0,
          rs.at(NORTH)*1000.0,
          rs.at(NORTH)*1000.0*chi);
        ts << str << "\n";
      };
      // axis offset:
      if (si->pAxisOffset()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        double                    vAxisOffset, sAxisOffset;
        vAxisOffset = config_->getUseExtAPrioriAxisOffsets()?si->getAxisOffset_ea():si->getAxisOffset();
        vAxisOffset+= si->pAxisOffset()->getSolution();
        sAxisOffset = si->pAxisOffset()->getSigma();
  
        str.sprintf("%5d. %-8s Axis Offset  %24.2f mm %14.3f mm %18.3f mm %18.3f mm",
          idx, 
          qPrintable(si->getKey()),
          vAxisOffset*1.0e3,
          si->pAxisOffset()->getSolution()*1.0e3,
          sAxisOffset*1.0e3,
          sAxisOffset*1.0e3*chi );
        ts << str << "\n";
        idx++;
      };
      //
      // clocks:
      SgMJD                     t=*si->auxObservationByScanId()->begin().value();
      if (si->pClock0()->getPMode() != SgParameterCfg::PM_NONE) 
      {
        int                     nPwl, nLoc, nMax;
        double                 *dVal, *dSig;
        pwl = pwlByName_.contains(si->pClock0()->getName())?
          pwlByName_.find(si->pClock0()->getName()).value():NULL;
        nPwl = pwl?pwl->getNumOfPolynomials():0;
        nLoc = si->getClocksModelOrder();
        nMax = std::max(nPwl, nLoc);
        dVal = new double[nMax];
        dSig = new double[nMax];
        for (int i=0; i<nMax; i++)
          dVal[i] = dSig[i] = 0.0;
        for (int i=0; i<nLoc; i++)
        {
          dVal[i] = si->getEstClockModel(i);
          dSig[i] = si->getEstClockModelSigma(i);
        };
        for (int i=0; i<nPwl; i++)
        {
          dVal[i]+= pwl->getP_Ai(i)->getSolution();
          dSig[i] = pwl->getP_Ai(i)->getSigma();
        };
        for (int i=0; i<nMax; i++)
        {
          str.sprintf("%5d. %-8s CL %1d %s                %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()), i,
            qPrintable(t.toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            dVal[i]*clcScaleVals[i],     qPrintable(clcScaleNames[i]),
            dSig[i]*clcScaleVals[i],     qPrintable(clcScaleNames[i]),
            dSig[i]*clcScaleVals[i]*chi, qPrintable(clcScaleNames[i]));
          ts << str << "\n";
          idx++;
        };
        delete[] dVal;
        delete[] dSig;
        // adjust index for pwl/arc parameters (clocks):
        if (pwlByName_.contains(si->pClock0()->getName()))
          idx += pwlByName_.find(si->pClock0()->getName()).value()->getNumOfNodes();
        else if (arcByName_.contains(si->pClock0()->getName()))
          idx += arcByName_.find(si->pClock0()->getName()).value()->getNum();
      };
      if (si->clockBreaks().size())
      {
        si->clockBreaks().sortEvents();
        for (int j=0; j<si->clockBreaks().size(); j++)
        {
          SgParameterBreak     *pb=si->clockBreaks().at(j);
          if (pb->isAttr(SgParameterBreak::Attr_DYNAMIC))
          {
            if (pb->pA0())
            {
//  219. SVETLOE  BR 0 15/07/31 07:19                               3.006 ns              0.022 ns              0.022 ns
              str.sprintf("%5d. %-8s BR %1d %s                %20.3f %-s %10.3f %-s %10.3f %-s",
                idx, qPrintable(si->getKey()), 0,
                qPrintable(pb->toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
                pb->pA0()->getSolution()*clcScaleVals[0],  qPrintable(clcScaleNames[0]),
                pb->pA0()->getSigma()*clcScaleVals[0],     qPrintable(clcScaleNames[0]),
                pb->pA0()->getSigma()*clcScaleVals[0]*chi, qPrintable(clcScaleNames[0]));
              ts << str << "\n";
              idx++;
            };
            if (pb->pA1() && si->getClocksModelOrder()>1)
            {
//  220. SVETLOE  BR 1 15/07/31 07:19                              -2.092 D-14            1.794 D-14            1.816 D-14
              str.sprintf("%5d. %-8s BR %1d %s                %20.3f %-s %10.3f %-s %10.3f %-s",
                idx, qPrintable(si->getKey()), 1,
                qPrintable(pb->toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
                pb->pA1()->getSolution()*clcScaleVals[1],  qPrintable(clcScaleNames[1]),
                pb->pA1()->getSigma()*clcScaleVals[1],     qPrintable(clcScaleNames[1]),
                pb->pA1()->getSigma()*clcScaleVals[1]*chi, qPrintable(clcScaleNames[1]));
              ts << str << "\n";
              idx++;
            };
            if (pb->pA2() && si->getClocksModelOrder()>2)
            {
//  221. SVETLOE  BR 2 15/07/31 07:19                              -2.186 D-14/day        4.439 D-14/day        4.494 D-14/day
              str.sprintf("%5d. %-8s BR %1d %s                %20.3f %-s %10.3f %-s %10.3f %-s",
                idx, qPrintable(si->getKey()), 2,
                qPrintable(pb->toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
                pb->pA2()->getSolution()*clcScaleVals[2],  qPrintable(clcScaleNames[2]),
                pb->pA2()->getSigma()*clcScaleVals[2],     qPrintable(clcScaleNames[2]),
                pb->pA2()->getSigma()*clcScaleVals[2]*chi, qPrintable(clcScaleNames[2]));
              ts << str << "\n";
              idx++;
            };
          }
          else
          {
//
// 390. YARRA12M BR 0 14/11/13 23:44                               0.472 ns              0.075 ns              0.075 ns
//
            str.sprintf("       %-8s BR 0 %s #%3d band=*         %15.3f ns    %15.3f ns %18.3f ns",
              qPrintable(si->getKey()),
              qPrintable(pb->toString(SgMJD::F_SOLVE_SPLFL_SHORT)), j,
              pb->getA0(),
              pb->getS0(),
              pb->getS0()*chi);
            ts << str << "\n";
          };
        };
      };
      // band-dependent breaks:
      for (int i=0; i<session_->bands().size(); i++)
      {
        SgVlbiBand             *band=session_->bands().at(i);
        if (band->stationsByName().contains(si->getKey()))
        {
          SgVlbiStationInfo    *sib=band->stationsByName().value(si->getKey());
          if (sib->clockBreaks().size())
          {
            sib->clockBreaks().sortEvents();
            for (int j=0; j<sib->clockBreaks().size(); j++)
            {
              SgParameterBreak     *pb=sib->clockBreaks().at(j);
              str.sprintf("       %-8s BR 0 %s #%3d band=%s         %15.3f ns    %15.3f ns %18.3f ns",
                qPrintable(sib->getKey()),
                qPrintable(pb->toString(SgMJD::F_SOLVE_SPLFL_SHORT)), j, qPrintable(band->getKey()),
                pb->getA0(),
                pb->getS0(),
                pb->getS0()*chi);
              ts << str << "\n";
            };
          };
        }
        else
          logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
            "::reportEstimationBlock_Output4Spoolfile(): cannot find station " + si->getKey() +
            " in the " + band->getKey() + "-band");
      };
      // zenith delay:
      // adjust index for pwl/arc parameters (zenith delay):
      if (pwlByName_.contains(si->pZenithDelay()->getName()))
      {
        pwl = pwlByName_.find(si->pZenithDelay()->getName()).value();
        for (int i=0; i<pwl->getNumOfPolynomials(); i++)
        {
          str.sprintf("%5d. %-8s AT %d %s                %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()), i,
            qPrintable(pwl->tStart().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            (i==0 ? pwl->getP_Ai(i)->getSolution() + si->getEstWetZenithDelay() :
                    pwl->getP_Ai(i)->getSolution()                         )/vLight*1.0e12, "ps        ",
            pwl->getP_Ai(i)->getSigma()/vLight*1.0e12, "ps        ",
            pwl->getP_Ai(i)->getSigma()/vLight*1.0e12*chi, "ps");
          ts << str << "\n";
          idx++;
        };
        str.sprintf("       %-8s Atm  %s  Avg: %9.3f ps   Rms: %11.3f ps   Tot_Rms: %11.3f ps",
          qPrintable(si->getKey()),
          qPrintable(
          pwl->tMean()
          // si->pZenithDelay()->getTMean()
          .toString(SgMJD::F_SOLVE_SPLFL_LONG)),
          pwl->calcMean()/vLight*1.0e12,
          pwl->calcWRMS()/vLight*1.0e12,
          pwl->calcWRMS()/vLight*1.0e12*chi);
        ts << str << "\n";
        idx += pwl->getNumOfNodes();
      }
      else if (arcByName_.contains(si->pZenithDelay()->getName()))
        idx += arcByName_.find(si->pZenithDelay()->getName()).value()->getNum();
      else if (si->pZenithDelay()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        str.sprintf("%5d. %-8s AT 0 %s                %20.3f %-s %10.3f %-s %10.3f %-s",
          idx, qPrintable(si->getKey()),
          qPrintable(t.toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
          si->pZenithDelay()->getSolution()/vLight*1.0e12, "ps        ",
          si->pZenithDelay()->getSigma()/vLight*1.0e12, "ps        ",
          si->pZenithDelay()->getSigma()/vLight*1.0e12*chi, "ps");
        ts << str << "\n";
        idx++;
      };
      //
      // adjust index for pwl/arc parameters (gradients):
      if (pwlByName_.contains(si->pAtmGradN()->getName()))
      {
        pwl = pwlByName_.find(si->pAtmGradN()->getName()).value();
        if (pwl->getNumOfSegments()==1)
        {
          SgPwlStorage         *pwlGrdN, *pwlGrdE;
          pwlGrdN = pwl;
          pwlGrdE = pwlByName_.find(si->pAtmGradE()->getName()).value();
          // gradient north, left:
          str.sprintf("%5d. %-8s NG %s                  %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()),
            qPrintable(pwlGrdN->tStart().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            pwlGrdN->getP_Bi(0)->getSolution()*1.0e3, "mm        ",
            pwlGrdN->getP_Bi(0)->getSigma()*1.0e3,    "mm        ",
            pwlGrdN->getP_Bi(0)->getSigma()*1.0e3*chi,"mm        ");
          ts << str << "\n";
          idx++;
          // gradient east, left:
          str.sprintf("%5d. %-8s EG %s                  %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()),
            qPrintable(pwlGrdE->tStart().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            pwlGrdE->getP_Bi(0)->getSolution()*1.0e3, "mm        ",
            pwlGrdE->getP_Bi(0)->getSigma()*1.0e3,    "mm        ",
            pwlGrdE->getP_Bi(0)->getSigma()*1.0e3*chi,"mm        ");
          ts << str << "\n";
          idx++;
          // gradient north, right:
          str.sprintf("%5d. %-8s NG %s                  %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()),
            qPrintable(pwlGrdN->tFinis().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            pwlGrdN->getP_Bi(1)->getSolution()*1.0e3, "mm        ",
            pwlGrdN->getP_Bi(1)->getSigma()*1.0e3,    "mm        ",
            pwlGrdN->getP_Bi(1)->getSigma()*1.0e3*chi,"mm        ");
          ts << str << "\n";
          idx++;
          // gradient east, right:
          str.sprintf("%5d. %-8s EG %s                  %20.3f %-s %10.3f %-s %10.3f %-s",
            idx, qPrintable(si->getKey()),
            qPrintable(pwlGrdE->tFinis().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
            pwlGrdE->getP_Bi(1)->getSolution()*1.0e3, "mm        ",
            pwlGrdE->getP_Bi(1)->getSigma()*1.0e3,    "mm        ",
            pwlGrdE->getP_Bi(1)->getSigma()*1.0e3*chi,"mm        ");
          ts << str << "\n";
          idx++;
        }
        else  // report in the same way as zenith delays:
        {
          str.sprintf("       %-8s NGr  %s  Avg: %9.3f ps   Rms: %11.3f ps   Tot_Rms: %11.3f ps",
            qPrintable(si->getKey()),
//          qPrintable(si->pAtmGradN()->getTMean().toString(SgMJD::F_SOLVE_SPLFL_LONG)),
            qPrintable(pwl->tMean().toString(SgMJD::F_SOLVE_SPLFL_LONG)),
            pwl->calcMean()/vLight*1.0e12,
            pwl->calcWRMS()/vLight*1.0e12,
            pwl->calcWRMS()/vLight*1.0e12*chi);
          ts << str << "\n";
          idx += pwl->getNumOfNodes() + pwl->getNumOfPolynomials();
          pwl = pwlByName_.find(si->pAtmGradE()->getName()).value();
          str.sprintf("       %-8s EGr  %s  Avg: %9.3f ps   Rms: %11.3f ps   Tot_Rms: %11.3f ps",
            qPrintable(si->getKey()),
            qPrintable(pwl->tMean().toString(SgMJD::F_SOLVE_SPLFL_LONG)),
//          qPrintable(si->pAtmGradN()->getTMean().toString(SgMJD::F_SOLVE_SPLFL_LONG)),
            pwl->calcMean()/vLight*1.0e12,
            pwl->calcWRMS()/vLight*1.0e12,
            pwl->calcWRMS()/vLight*1.0e12*chi);
          ts << str << "\n";
          idx += pwl->getNumOfNodes() + pwl->getNumOfPolynomials();
        };
      }
      else 
      {
        if (arcByName_.contains(si->pAtmGradN()->getName()))
          idx += arcByName_.find(si->pAtmGradN()->getName()).value()->getNum();
        if (arcByName_.contains(si->pAtmGradE()->getName()))
          idx += arcByName_.find(si->pAtmGradE()->getName()).value()->getNum();
      };
      ts << "\n";
    };
  };
  //
  //
  if (parametersDescriptor_->getSrcCooMode() == SgParameterCfg::PM_LOC)
  {
    for (int i=0; i<usedSources_.size(); i++)
    {
      SgVlbiSourceInfo         *si=usedSources_.at(i);
      if (si->pRA()->isAttr(SgParameter::Attr_IS_SOLVED) &&
          si->pDN()->isAttr(SgParameter::Attr_IS_SOLVED)  )
      {
        double                  vRA, vDN;
        double                  d;
        vRA = config_->getUseExtAPrioriSourcesPositions()?si->getRA_ea():si->getRA();
        vDN = config_->getUseExtAPrioriSourcesPositions()?si->getDN_ea():si->getDN();
        vRA+= si->pRA()->getSolution();
        vDN+= si->pDN()->getSolution();
        //
        // RA:
        str.sprintf("%5d.%s %-8s RT. ASC.          %s %11.4f      m-asec %10.4f      m-asec %10.4f"
                    "      m-asec",
          idx,
          qPrintable(srcChars[i % numOfSrcChars]),
          qPrintable(si->getKey()),
          qPrintable(si->ra2String(vRA)),
          si->pRA()->getSolution()*RAD2MAS,
          si->pRA()->getSigma()*RAD2MAS,
          si->pRA()->getSigma()*RAD2MAS*chi
          );
        ts << str << "\n";
        str.sprintf("                  CORRECTION  %20.7f\n                SCALED SIGMA  %20.7f",
          si->pRA()->getSolution()*RAD2SEC/15.0,
          si->pRA()->getSigma()*RAD2SEC/15.0*chi
          );
        ts << str << "\n";
        ts << "\n";
        idx++;
        
        // DN:
        str.sprintf("%5d.%s %-8s DEC.             %s %16.4f m-asec %15.4f m-asec %15.4f m-asec",
          idx,
          qPrintable(srcChars[i % numOfSrcChars]),
          qPrintable(si->getKey()),
          qPrintable(si->dn2String(vDN)),
          si->pDN()->getSolution()*RAD2MAS,
          si->pDN()->getSigma()*RAD2MAS,
          si->pDN()->getSigma()*RAD2MAS*chi
          );
        ts << str << "\n";
        str.sprintf("                  CORRECTION  %20.7f\n                SCALED SIGMA  %20.7f",
          si->pDN()->getSolution()*RAD2SEC,
          si->pDN()->getSigma()*RAD2SEC*chi
          );
        ts << str << "\n";
        ts << "\n";
        idx++;
        //
        // correlation:
        // covariances (i.e., rho_{1,2}*sigma_1*sigma_2):
        d = PxAll_->getElement(si->pRA()->getIdx(), si->pDN()->getIdx());
        if (si->pRA()->getSigma()>0.0 && si->pDN()->getSigma()>0.0)
          str.sprintf("        %-8s CORRELATION %10.4f  Reference date: 2000.01.01-12:00:00",
            qPrintable(si->getKey()),  
            d/(si->pRA()->getSigma()*si->pDN()->getSigma())
            );
        else
          str.sprintf("        %-8s CORRELATION    -------  Reference date: 2000.01.01-12:00:00",
            qPrintable(si->getKey()));
        ts << str << "\n";
      };
    };
  };
  //
  // stations again:
  //
  // zenith delays:
  int         idxAux, sum;
  if (parametersDescriptor_->getZenithMode() == SgParameterCfg::PM_PWL)
  {
    idxAux = 1;
    sum = 0;
    ts << " Atmosphere Constraint Statistics\n";
    double                      d, ovrl_rms, ovrl_rel, ovrl_trace;
    double                      scale=1.0e12/vLight/24.0;
    ovrl_rms = ovrl_trace = ovrl_rel = 0.0;
    for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    {
      SgVlbiStationInfo        *si = it_st.value();
      if (pwlByName_.contains(si->pZenithDelay()->getName()))
      {
        pwl = pwlByName_.find(si->pZenithDelay()->getName()).value();
//      pwl->calcMeansResids4Sfo(d, v_sig);
        d = pwl->calcRateRms4Sfo();
//      double                  sigmaAPriori=pwl->getP_Bi(0)->getSigmaAPriori(); // actual
        double                  sigmaAPriori=pwl->getPOrig()->getSigmaAPriori(); // nominal
        ovrl_rms += d;
        ovrl_rel += d/sigmaAPriori/sigmaAPriori;
        d = sqrt(d/pwl->getNumOfNodes());
        ovrl_trace += pwl->trace();
        str.sprintf("%3d. %-8s Input %6.2f ps/h RMS %6.2f ps/h NRMS %5.2f share %4.2f count %3d",
          idxAux, qPrintable(si->getKey()),
          pwl->getPOrig()->getSigmaAPriori()*scale,
          d*scale,
          d/sigmaAPriori,
          pwl->getNumOfNodes()>0?pwl->trace()/pwl->getNumOfNodes():0.0,
          pwl->getNumOfNodes());
        ts << str << "\n";
        idxAux++;
        sum += pwl->getNumOfNodes();
      };
    };
    ovrl_rms = sqrt(ovrl_rms/sum);
    ovrl_rel = sqrt(ovrl_rel/sum);
    str.sprintf("     Overall                    RMS %6.2f ps/h NRMS %5.2f share %4.2f count %3d",
      ovrl_rms*scale,
      ovrl_rel,
      ovrl_trace/sum,
      sum);
    ts << str << "\n\n";
  };
  // clocks:
  if (parametersDescriptor_->getClock0Mode() == SgParameterCfg::PM_PWL)
  {
    double                      d, ovrl_rms, ovrl_rel, ovrl_trace;
    double                      scale=1.0e14/86400.0;
    ovrl_rms = ovrl_trace = ovrl_rel = 0.0;
    idxAux = 1;
    sum = 0;
    ts << " Clock Constraint Statistics\n";
    for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    {
      SgVlbiStationInfo *si = it_st.value();
      if (pwlByName_.contains(si->pClock0()->getName()))
      {
        pwl = pwlByName_.find(si->pClock0()->getName()).value();
//      pwl->calcMeansResids4Sfo(d, v_sig);
        d = pwl->calcRateRms4Sfo();
//      double                  sigmaAPriori=pwl->getP_Bi(0)->getSigmaAPriori(); // actual
        double                  sigmaAPriori=pwl->getPOrig()->getSigmaAPriori(); // nominal
        ovrl_rms += d;
        ovrl_rel += d/sigmaAPriori/sigmaAPriori;
        d = sqrt(d/pwl->getNumOfNodes());
        ovrl_trace += pwl->trace();
        str.sprintf("%3d. %-8s Input %6.2f D-14 RMS %6.2f D-14 NRMS %5.2f share %4.2f count %3d",
          idxAux, qPrintable(si->getKey()),
          pwl->getPOrig()->getSigmaAPriori()*scale,
          d*scale,
          d/sigmaAPriori,
          pwl->getNumOfNodes()>0?pwl->trace()/pwl->getNumOfNodes():0.0,
          pwl->getNumOfNodes());
        ts << str << "\n";
        idxAux++;
        sum += pwl->getNumOfNodes();
      };
    };
    ovrl_rms = sqrt(ovrl_rms/sum);
    ovrl_rel = sqrt(ovrl_rel/sum);
    str.sprintf("     Overall                    RMS %6.2f D-14 NRMS %5.2f share %4.2f count %3d",
      ovrl_rms*scale,
      ovrl_rel,
      ovrl_trace/sum,
      sum);
    ts << str << "\n\n";
  };
  //
  // EOPs:
  SgParameter          *p, *p4erp=NULL;
  bool                  isErpAdjusted(false);
  double                dUt1=0.0, dPmx=0.0, dPmy=0.0;
  double                rUt1=0.0, rPmx=0.0, rPmy=0.0;
  double                sigUt1=0.0, sigPmx=0.0, sigPmy=0.0;
  double                aprUt1=0.0, aprPmx=0.0, aprPmy=0.0, dt, dd;
  double                aprCipX=0.0, aprCipY=0.0;
  //
  // corrections to polar motion, x:
  p = session_->pPolusX();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    aprPmx = erp_pmx_0_ + erp_pmx_1_*dt + erp_pmx_2_*dt*dt + erp_pmx_3_*dt*dt*dt;
    dd = 0.0;
    if (session_->pPolusXRate() && session_->pPolusXRate()->isAttr(SgParameter::Attr_IS_SOLVED))
      dd = session_->pPolusXRate()->getSolution()*dt;
    str.sprintf("%5d. X Wobble  0,  %s  %12.4f masec %10.2f microasec %10.2f microasec %10.2f microasec",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (aprPmx + p->getSolution() + dd)*RAD2SEC*1.0e3,
      p->getSolution()*RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6*chi
      );
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    p4erp = p;
    dPmx  = p->getSolution();
    sigPmx= p->getSigma();
  };
  // corrections to polar-x motion rate:
  p = session_->pPolusXRate();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    str.sprintf("%5d. X Wobble  1,  %s  %12.4f mas/d %10.2f microas/d %10.2f microas/d %10.2f microas/d",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (erp_pmx_1_ + 2.0*erp_pmx_2_*dt + 3.0*erp_pmx_3_*dt*dt + p->getSolution())*RAD2SEC*1.0e3,
      p->getSolution()*RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6*chi
      );
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    rPmx  = p->getSolution();
    p4erp = p;
  };
  //
  // corrections to polar-y motion:
  p = session_->pPolusY();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    aprPmy = erp_pmy_0_ + erp_pmy_1_*dt + erp_pmy_2_*dt*dt + erp_pmy_3_*dt*dt*dt;
    dd = 0.0;
    if (session_->pPolusYRate() && session_->pPolusYRate()->isAttr(SgParameter::Attr_IS_SOLVED))
      dd = session_->pPolusYRate()->getSolution()*dt;
    str.sprintf("%5d. Y Wobble  0,  %s  %12.4f masec %10.2f microasec %10.2f microasec %10.2f microasec",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (aprPmy + p->getSolution() + dd)*RAD2SEC*1.0e3,
      p->getSolution()*RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6*chi
      );
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    p4erp = p;
    dPmy  = p->getSolution();
    sigPmy= p->getSigma();
  };
  // corrections to polar-y motion rate:
  p = session_->pPolusYRate();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    str.sprintf("%5d. Y Wobble  1,  %s  %12.4f mas/d %10.2f microas/d %10.2f microas/d %10.2f microas/d",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (erp_pmy_1_ + 2.0*erp_pmy_2_*dt + 3.0*erp_pmy_3_*dt*dt + p->getSolution())*RAD2SEC*1.0e3,
      p->getSolution()*RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6,
      p->getSigma()*   RAD2SEC*1.0e6*chi
      );
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    rPmy  = p->getSolution();
    p4erp = p;
  };
  //
  // corrections to UT1:
  p = session_->pUT1();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    aprUt1 = erp_ut1_0_ + erp_ut1_1_*dt + erp_ut1_2_*dt*dt + erp_ut1_3_*dt*dt*dt;
    dd = 0.0;
    if (session_->pUT1Rate() && session_->pUT1Rate()->isAttr(SgParameter::Attr_IS_SOLVED))
      dd = session_->pUT1Rate()->getSolution()*dt;
    str.sprintf("%5d. UT1-TAI   0,  %s  %12.4f  msec %10.2f microsec %11.2f microsec %11.2f microsec",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (aprUt1 + p->getSolution() + dd)*DAY2SEC*1.0e3,
      p->getSolution()*DAY2SEC*1.0e6,
      p->getSigma()*   DAY2SEC*1.0e6,
      p->getSigma()*   DAY2SEC*1.0e6*chi);
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    p4erp = p;
    dUt1  = p->getSolution();
    sigUt1= p->getSigma();
    //
    dUt1Value_ = aprUt1 + p->getSolution() + dd;
    dUt1Correction_ = p->getSolution();
    dUt1StdDev_ = p->getSigma();
    //
  };
  // corrections to UT1 rate:
  p = session_->pUT1Rate();
  if (p && p->isAttr(SgParameter::Attr_IS_SOLVED))
  {
    dt = p->getTMean() - erpTref_;
    // progs/solve/adjst/a2jst_segeop.f ?
    str.sprintf("%5d. UT1-TAI   1,  %s  %12.4f  ms/d %10.2f micros/d %11.2f micros/d %11.2f micros/d",
      idx,
      qPrintable(p->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (erp_ut1_1_ + 2.0*erp_ut1_2_*dt + 3.0*erp_ut1_3_*dt*dt + p->getSolution())*DAY2SEC*1.0e3,
      p->getSolution()*DAY2SEC*1.0e6,
      p->getSigma()*   DAY2SEC*1.0e6,
      p->getSigma()*   DAY2SEC*1.0e6*chi
      );
    ts << str << "\n";
    idx++;
    isErpAdjusted = true;
    rUt1  = p->getSolution();
    p4erp = p;
  };
  if (isErpAdjusted && p4erp)
  {
    dt = p4erp->getTMean() - erpTref_;
    ts 
//    << " EOP with included hi-freq variations  (a-sigmas)  \n"
      << " EOP without included hi-freq variations  (a-sigmas)  \n"
      << "                      XWOB          YWOB          UT1-TAI"
      << "          XSIG         YSIG         USIG\n"
      << "                       mas           mas            ms   "
      << "        microasec    microasec    microsec\n";
//  str.sprintf("%s        0.0000         0.0000         0.0000          0.00         0.00         0.00",
    str.sprintf("%s  %12.4f   %12.4f   %12.4f   %11.2f  %11.2f  %11.2f",
      qPrintable(p4erp->getTMean().toString(SgMJD::F_SOLVE_SPLFL_SHORT)),
      (aprPmx + rPmx*dt + dPmx)*RAD2SEC*1.0e3,
      (aprPmy + rPmy*dt + dPmy)*RAD2SEC*1.0e3,
      (aprUt1 + rUt1*dt + dUt1)*DAY2SEC*1.0e3,
      sigPmx*RAD2SEC*1.0e6,
      sigPmy*RAD2SEC*1.0e6,
      sigUt1*DAY2SEC*1.0e6);
    ts << str << "\n" << "\n";
  };
  //
//------------------------------------------------------------------------------------------------------
  // Nutation:
  if (session_->pNutX() && session_->pNutX()->isAttr(SgParameter::Attr_IS_SOLVED) &&
      session_->pNutY() && session_->pNutY()->isAttr(SgParameter::Attr_IS_SOLVED)  )
  {  
    p = session_->pNutX();
    SgMJD                         epoch(p->getTMean());
    // conver UTC to TT, cheap'n'angry:
    epoch += (session_->getLeapSeconds() + 32.184)/DAY2SEC;
    //
    dt = p->getTMean() - erpTref_;
    aprCipX     = eop_cix_0_ + eop_cix_1_*dt + eop_cix_2_*dt*dt + eop_cix_3_*dt*dt*dt;
    aprCipY     = eop_ciy_0_ + eop_ciy_1_*dt + eop_ciy_2_*dt*dt + eop_ciy_3_*dt*dt*dt;
    //
    double                        dX, dY, dPsi, dEps;
    double                        dX_sigma, dY_sigma, dPsi_sigma, dEps_sigma;
    if (session_->hasCipPartials())
    {
      dX          = session_->pNutX()->getSolution() + aprCipX;
      dX_sigma    = session_->pNutX()->getSigma();
      dY          = session_->pNutY()->getSolution() + aprCipY;
      dY_sigma    = session_->pNutY()->getSigma();

      dPsi        = dX/sin(84381.4059*SEC2RAD);
      dPsi_sigma  = dX_sigma/sin(84381.4059*SEC2RAD);
      dEps        = dY;
      dEps_sigma  = dY_sigma;
    }
    else
    {
      dPsi        = session_->pNutX()->getSolution();
      dPsi_sigma  = session_->pNutX()->getSigma();
      dEps        = session_->pNutY()->getSolution();
      dEps_sigma  = session_->pNutY()->getSigma();

      dX          = dPsi*sin(84381.4059*SEC2RAD) + aprCipX;
      dX_sigma    = dPsi_sigma*sin(84381.4059*SEC2RAD);
      dY          = dEps + aprCipY;
      dY_sigma    = dEps_sigma;
    };
    double                        psiEst2Wahr=0.0, epsEst2Wahr=0.0;
    SgVlbiObservation            *obs=session_->observations().at(0);
    if (obs)
    {
      calcCip2IAU1980(epoch, dX, dY, 
        obs->getCalcNutWahr_dPsiV(), obs->getCalcNutWahr_dEpsV(),
        obs->getCalcNut2006_dPsiV(), obs->getCalcNut2006_dEpsV(), 
        psiEst2Wahr, epsEst2Wahr);
    };
  
    // corrections to Celestial intermediate pole, x:
    p = session_->pNutX();
    str.sprintf(" EOP epoch (TT)   MJD: %12.6f        NUT epoch (TT)   MJD: %12.6f",
                epoch.toDouble(), epoch.toDouble());
    ts << str << "\n";
    str.sprintf("%5d. Nutation offset in longitude (Psi)       %14.3f mas %10.1f microasec"
                " %10.1f microasec       ",
                idx,
                dPsi      *RAD2SEC*1.0e3,
                dPsi_sigma*RAD2SEC*1.0e6,
                dPsi_sigma*RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
    idx++;
    str.sprintf("       Nutation offset around X-axis       (dX) %14.3f mas %10.1f "
                "microasec %10.1f microasec       ",
                dX      *RAD2SEC*1.0e3,
                dX_sigma*RAD2SEC*1.0e6,
                dX_sigma*RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
    str.sprintf("       Nutation offset wrt IAU 1980 model (Psi) %14.3f mas %10.1f "
                "microasec %10.1f microasec       ",
//              (dPsi + psiEst2Wahr)*RAD2SEC*1.0e3,
                psiEst2Wahr *RAD2SEC*1.0e3,
                dPsi_sigma  *RAD2SEC*1.0e6,
                dPsi_sigma  *RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
    // corrections to Celestial intermediate pole, y:
    p = session_->pNutY();
    str.sprintf("%5d. Nutation offset in obliquity (Eps)       %14.3f mas %10.1f "
                "microasec %10.1f microasec       ",
                idx,
                dEps      *RAD2SEC*1.0e3,
                dEps_sigma*RAD2SEC*1.0e6,
                dEps_sigma*RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
    idx++;
    str.sprintf("       Nutation offset around Y-axis       (dY) %14.3f mas "
                "%10.1f microasec %10.1f microasec       ",
                dY      *RAD2SEC*1.0e3,
                dY_sigma*RAD2SEC*1.0e6,
                dY_sigma*RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
    str.sprintf("       Nutation offset wrt IAU 1980 model (Eps) %14.3f mas "
                "%10.1f microasec %10.1f microasec       ",
//              (dEps + epsEst2Wahr)*RAD2SEC*1.0e3,
                epsEst2Wahr *RAD2SEC*1.0e3,
                dEps_sigma  *RAD2SEC*1.0e6,
                dEps_sigma  *RAD2SEC*1.0e6*chi
                );
    ts << str << "\n";
  };//---------------------------------------------------------------------------------------------------
  
  // Baselines, clocks:
  if (  parametersDescriptor_->getBlClockMode() != SgParameterCfg::PM_NONE)
  {
    ts << "\n";
    BaselinesByName_it            it_bl;
    for (it_bl=session_->baselinesByName().begin(); it_bl!=session_->baselinesByName().end(); ++it_bl)
    {
      SgVlbiBaselineInfo       *bi=it_bl.value();
      QString                   blName;
      blName = bi->getKey();
      blName.replace(8, 1,  "-");
      if (bi->pClock()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        str.sprintf("%5d. %-17s Clock offset                      "
                    "%12.3f ps          %9.3f ps          %9.3f ps",
          idx, qPrintable(blName), 
          bi->pClock()->getSolution()*1.0E12,
          bi->pClock()->getSigma()   *1.0E12, 
          bi->pClock()->getSigma()   *1.0E12*chi );
        ts << str << "\n";
        idx++;
      };
    };
  };
  // Baselines, baseline vector:
  if (parametersDescriptor_->getBlLengthMode() != SgParameterCfg::PM_NONE)
  {
    ts << "\n";
    BaselinesByName_it            it_bl;
    for (it_bl=session_->baselinesByName().begin(); it_bl!=session_->baselinesByName().end(); ++it_bl)
    {
      SgVlbiBaselineInfo       *bi=it_bl.value();
      QString                   blName;
      blName = bi->getKey();
      blName.replace(8, 1,  "-");
      // baseline vector:
      if (bi->pBx()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        SgVlbiStationInfo    *s1, *s2;
        if (session_->stationsByName().contains(bi->getKey().mid(0,8)))
          s1 = session_->stationsByName().value(bi->getKey().mid(0,8));
        else
          s1 = NULL;
        if (session_->stationsByName().contains(bi->getKey().mid(9,8)))
          s2 = session_->stationsByName().value(bi->getKey().mid(9,8));
        else
          s2 = NULL;
        if (s1 && s2)
        {
          // covariances (i.e., rho_{1,2}*sigma_1*sigma_2):
          double                cXY, cXZ, cYZ;
          cXY = PxAll_->getElement(bi->pBx()->getIdx(), bi->pBy()->getIdx());
          cXZ = PxAll_->getElement(bi->pBx()->getIdx(), bi->pBz()->getIdx());
          cYZ = PxAll_->getElement(bi->pBy()->getIdx(), bi->pBz()->getIdx());
          Sg3dVector            b, b_apriori;
          b_apriori = (s2->getR() - s1->getR());
          if (config_->getUseExtAPrioriSitesPositions())
          {
            SgMJD                 t(bi->pBx()->getTMean());
            b_apriori = s2->getR_ea() - s1->getR_ea() + 
                       (s2->getV_ea() - s1->getV_ea())*(t - session_->getApStationVelocities().getT0());
          };
          b = b_apriori +
            Sg3dVector(bi->pBx()->getSolution(), bi->pBy()->getSolution(), bi->pBz()->getSolution());
          double                  l, lSigma;
          double                  x2, y2, z2, sigX2, sigY2, sigZ2;
          x2 = b.at(X_AXIS)*b.at(X_AXIS);
          y2 = b.at(Y_AXIS)*b.at(Y_AXIS);
          z2 = b.at(Z_AXIS)*b.at(Z_AXIS);
          sigX2 = bi->pBx()->getSigma()*bi->pBx()->getSigma();
          sigY2 = bi->pBy()->getSigma()*bi->pBy()->getSigma();
          sigZ2 = bi->pBz()->getSigma()*bi->pBz()->getSigma();
          l = b.module();
          lSigma = x2*sigX2 + y2*sigY2 + z2*sigZ2 +
            2.0*( b.at(X_AXIS)*b.at(Y_AXIS)*cXY +
                  b.at(X_AXIS)*b.at(Z_AXIS)*cXZ +
                  b.at(Y_AXIS)*b.at(Z_AXIS)*cYZ );
          lSigma = sqrt(lSigma)/l;
          // sigma of latitude:
          double                phiSigma, a2, l4;
          a2 = x2 + y2;
          l4 = l*l*l*l;
          phiSigma = (x2*z2*sigX2 + y2*z2*sigY2 + a2*a2*sigZ2 + 
            2.0*(b.at(X_AXIS)*b.at(Y_AXIS)*z2*cXY - 
                            a2*(b.at(X_AXIS)*b.at(Z_AXIS)*cXZ + b.at(Y_AXIS)*b.at(Z_AXIS)*cYZ))  )/l4/a2;
          phiSigma = sqrt(phiSigma);
          // sigma of longitude:
          double                lambdaSigma;
          lambdaSigma = (y2*sigX2 + x2*sigY2 - 2.0*b.at(X_AXIS)*b.at(Y_AXIS)*cXY)/a2/a2;
          lambdaSigma = sqrt(lambdaSigma);

          str.sprintf("%5d. %-17s   X Comp     %15.2f mm %14.3f mm     "
                      "%14.3f mm     %14.3f mm",
            idx, qPrintable(blName),
            b.at(X_AXIS)            *1000.0,
            bi->pBx()->getSolution()*1000.0,
            bi->pBx()->getSigma()   *1000.0,
            bi->pBx()->getSigma()   *1000.0*chi );
          ts << str << "\n";
          idx++;
          str.sprintf("%5d. %-17s   Y Comp     %15.2f mm %14.3f mm     "
                      "%14.3f mm     %14.3f mm",
            idx, qPrintable(blName),
            b.at(Y_AXIS)            *1000.0,
            bi->pBy()->getSolution()*1000.0,
            bi->pBy()->getSigma()   *1000.0,
            bi->pBy()->getSigma()   *1000.0*chi );
          ts << str << "\n";
          idx++;
          str.sprintf("%5d. %-17s   Z Comp     %15.2f mm %14.3f mm     "
                      "%14.3f mm     %14.3f mm",
            idx, qPrintable(blName),
            b.at(Z_AXIS)            *1000.0,
            bi->pBz()->getSolution()*1000.0,
            bi->pBz()->getSigma()   *1000.0,
            bi->pBz()->getSigma()   *1000.0*chi );
          ts << str << "\n";
          idx++;
          // length
          str.sprintf("       %-17s   length     %15.2f mm %14.3f mm     "
                      "%14.3f mm     %14.3f mm",
            qPrintable(blName),
            l                       *1000.0,
            (l - b_apriori.module())*1000.0,
            lSigma                  *1000.0,
            lSigma                  *1000.0*chi );
          ts << str << "\n";
          // latitude
          str.sprintf("       %-17s   latitude   %15.2f deg %13.3f mas    "
                      "%14.3f mas    %14.3f mas",
            qPrintable(blName),
            b.phi()                    *RAD2DEG,
            (b.phi() - b_apriori.phi())*RAD2MAS,
            phiSigma                   *RAD2MAS,
            phiSigma                   *RAD2MAS*chi );
          ts << str << "\n";
          // longitude
          str.sprintf("       %-17s   longitude  %15.2f deg %13.3f mas    "
                      "%14.3f mas    %14.3f mas",
            qPrintable(blName),
            b.lambda()                        *RAD2DEG,
            (b.lambda() - b_apriori.lambda()) *RAD2MAS,
            lambdaSigma                       *RAD2MAS,
            lambdaSigma                       *RAD2MAS*chi );
          ts << str << "\n";
        }
        else
          logger->write(SgLogger::ERR, SgLogger::DATA, className() +
            "::reportEstimationBlock_Output4Spoolfile(): a station pointer is NULL");
      };
    };
  };
  //
  //
  if (numOfConstraints_ || pwlList_.size())
  {
    ts
      << " \n" 
      << " General constraints usage information:        "
      << "                                                                         \n"
      << " \n";
    str.sprintf(" 1) CLO_RATE \"Clock rate between segments     \" sigma %10.3E 10^-14 sec/sec",
      parametersDescriptor_->getClock0().getPwlAPriori()
        /parametersDescriptor_->getClock0().getScale()
        /86400.0*1.0e14);
    ts << str << "\n";
    str.sprintf(" 2) ATM_RATE \"Atmosphere rate between segments\" sigma %10.3E psec/hr",
      parametersDescriptor_->getZenith().getPwlAPriori()
        /parametersDescriptor_->getZenith().getScale()
        /vLight/24.0*1.0e12);
    ts << str << "\n \n";
  }
  else
    ts << "    No constraints have been imposed\n \n";
  
  str.sprintf(" Corrected Reduced Chi-Square %8.4f",
//    session_->primaryBand()->getChi2()/session_->primaryBand()->getNumOfDOF());
//  activeBand_->chi2(dType)/session_->getNumOfDOF());
    activeBand_->reducedChi2(dType));
  ts << str << "\n\n";
};



//
bool SgSolutionReporter::reportAtmo(const QString& path, const QString& fileName)
{
  // emulates SOLVE output:
  // ATMO<U.I.>
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportAtmo(): error opening output file: " + path + "/" + fileName);
    return false;
  };
  //
  // check data type (it should be equidistant):
  int                           num;
  bool                          isOk;
  SgMJD                         t_i;
  double                        step;
  num = -1;
  step = -1.0;
  isOk = true;
  StationsByName_it             it_st;
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    if (isOk)
    {
      SgVlbiStationInfo        *si = it_st.value();
      if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) && 
            pwlByName_.contains(si->pZenithDelay()->getName()))
      {
        SgPwlStorage           *pwl = pwlByName_.find(si->pZenithDelay()->getName()).value();
        if (num==-1 && pwl->getNumOfSegments()>0)
        {
          num = pwl->getNumOfSegments();
          t_i = pwl->tStart();
          step = pwl->step();
        }
        else if (num==-1 && pwl->getNumOfSegments()<=0)
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportAtmo(): cannot report PWL zenith delays, the number of parameteres less than 1");
          isOk = false;
        }
        else if (num != pwl->getNumOfSegments())
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportAtmo(): cannot report PWL zenith delays, non-equal number of parameters");
          isOk = false;
        }
        else if (step != pwl->step())
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportAtmo(): cannot report PWL zenith delays, non-equidistant steps");
          isOk = false;
        };
      }
      else if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
      {
        logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
          "::reportAtmo(): cannot report PWL zenith delays, missed station" + si->getKey());
        isOk = false;
      };
    };
  if (!isOk)
    return false;
  //
  // make output:
  QTextStream                   ts(&f);
  QString                       str;
  ts << " yr mn dy hr min Julian Date           ";
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    ts << str.sprintf("%-8s             ", qPrintable(it_st.value()->getKey()));
  ts << "\n                                       ";
  for (int i=0; i<session_->stationsByName().size(); i++)
    ts << "offset sigma         ";
  ts << "\n                                       ";
  for (int i=0; i<session_->stationsByName().size(); i++)
    ts << "    ps    ps         ";
  ts << "\n";
  for (int idx=0; idx<num+1; idx++)
  {
    int                         yr, mn, dy, hr, mi;
    double                      sc;
    SgMJD::MJD_reverse(t_i.getDate(), t_i.getTime(), yr, mn, dy, hr, mi, sc);
    mi = (60*mi + sc)/60;
    yr -= (yr/100)*100;
    str.sprintf(" %2d %2d %2d %2d %2d  %.5f",
      yr, mn, dy, hr, mi, t_i.toDouble() + 2400000.5);
    ts << str;
    for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    {
      SgVlbiStationInfo        *si = it_st.value();
      if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
      {
        SgPwlStorage           *pwl = pwlByName_.find(si->pZenithDelay()->getName()).value();
//        SgParameter             *pB = pwl->getP_Bi(idx);
        ts << str.sprintf(" %14.0f %5.0f",
//          (si->getEstWetZenithDelay() + pwl->calcPolySolution(t_i) + pB->getSolution())/vLight*1.0e12,
          ( si->getEstWetZenithDelay() + 
            pwl->calcPolySolution(t_i) + 
            pwl->calcRateSolution(t_i)  )/vLight*1.0e12,
//            pB->getSigma()/vLight*1.0e12);
            pwl->calcRateSigma(t_i)/vLight*1.0e12);
      }
      else
        ts << "              0     0";
    };
    ts << "\n";
    t_i += step;
  };
  // close the file:
  ts.setDevice(NULL);
  f.close();
  return true;
}; 



//
bool SgSolutionReporter::reportCloc(const QString& path, const QString& fileName)
{
  // emulates SOLVE output:
  // CLOC<U.I.>
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportCloc(): error opening output file: " + path + "/" + fileName);
    return false;
  };
  //
  // check data type (it should be equidistant):
  int                           num;
  bool                          isOk;
  SgMJD                         t_i;
  double                        step;
  num = -1;
  step = -1.0;
  isOk = true;
  StationsByName_it             it_st;
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    if (isOk)
    {
      SgVlbiStationInfo        *si = it_st.value();
      if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
            pwlByName_.contains(si->pClock0()->getName()))
      {
        SgPwlStorage           *pwl = pwlByName_.find(si->pClock0()->getName()).value();
        if (num==-1 && pwl->getNumOfSegments()>0)
        {
          num = pwl->getNumOfSegments();
          t_i = pwl->tStart();
          step = pwl->step();
        }
        else if (num==-1 && pwl->getNumOfSegments()<=0)
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportCloc(): cannot report PWL clocks, the number of parameteres less than 1");
          isOk = false;
        }
        else if (num != pwl->getNumOfSegments())
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportCloc(): cannot report PWL clocks, non-equal number of parameters");
          isOk = false;
        }
        else if (step != pwl->step())
        {
          logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
            "::reportCloc(): cannot report PWL clocks, non-equidistant steps");
          isOk = false;
        };
      }
      else if (!si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) && 
               !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)           )
      {
        logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
          "::reportCloc(): cannot report PWL clocks, missed station " + si->getKey());
        isOk = false;
      };
    };
  if (!isOk)
    return false;
  //
  // make output:
  QTextStream                   ts(&f);
  QString                       str;
  ts << " yr mn dy hr min Julian Date           ";
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    ts << str.sprintf("%-8s             ", qPrintable(it_st.value()->getKey()));
  ts << "\n                                       ";
  for (int i=0; i<session_->stationsByName().size(); i++)
    ts << "offset sigma         ";
  ts << "\n                                       ";
  for (int i=0; i<session_->stationsByName().size(); i++)
    ts << "    ps    ps         ";
  ts << "\n";
  for (int idx=0; idx<num+1; idx++)
  {
    int                         yr, mn, dy, hr, mi;
    double                      sc;
    SgMJD::MJD_reverse(t_i.getDate(), t_i.getTime(), yr, mn, dy, hr, mi, sc);
    mi = (60*mi + sc)/60;
    yr -= (yr/100)*100;
    str.sprintf(" %2d %2d %2d %2d %2d  %.5f",
      yr, mn, dy, hr, mi, t_i.toDouble() + 2400000.5);
    ts << str;
    for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
    {
      SgVlbiStationInfo        *si = it_st.value();
      if (!si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) &&
          !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)          )
      {
        SgPwlStorage           *pwl = pwlByName_.find(si->pClock0()->getName()).value();
        ts << str.sprintf(" %14.0f %5.0f",  
          pwl->calcRateSolution(t_i)*1.0e12, 
          pwl->calcRateSigma(t_i)*1.0e12);
      }
      else
        ts << "              0     0";
    };
    ts << "\n";
    t_i += step;
  };
  // close the file:
  ts.setDevice(NULL);
  f.close();
  return true;
};



//
bool SgSolutionReporter::reportPall(const QString& path, const QString& fileName)
{
  // emulates SOLVE output:
  // CLOC<U.I.>
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportPall(): error opening the output file: " + path + "/" + fileName);
    return false;
  };

  //
  // make output:
  QTextStream                   ts(&f);
  QString                       str;

  ts << allParList_.size() << " parameters in the list\n";
  for (int i=0; i<allParList_.size(); i++)
    ts << str.sprintf("%4d ", i) << allParList_.at(i)->getName() << "\n";
  
  ts << "\n" << PxAll_->nRow() << " elements in a row of the covariance matrix\n";
  for (unsigned int i=0; i<PxAll_->nRow(); i++)
    for (unsigned int j=i; j<PxAll_->nCol(); j++)
      ts << str.sprintf("%4d %4d %22.15E", i, j, PxAll_->getElement(i, j)) << "\n";

  // close the file:
  ts.setDevice(NULL);
  f.close();
  return true;
};



//
bool SgSolutionReporter::reportNotUsedObs(const QString& path, const QString& fileName)
{
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportNotUsedObs(): error opening output file: " + path + "/" + fileName);
    return false;
  };
  //
  QTextStream                   ts(&f);

//reportDeselectedObsBlock_Output4Spoolfile(ts);
//reportDeselectedObsBlock_Output4Spoolfile_v2(ts);
  reportDeselectedObsBlock_Output4Spoolfile_v3(ts);

  // close the file:
  ts.setDevice(NULL);
  f.close();
  return true;
};



//
void SgSolutionReporter::reportBaselineVariationsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString                       str;
  int                           n;
  SgMJD                         t(session_->tRefer());

  if (parametersDescriptor_->getStnCooMode()   == SgParameterCfg::PM_NONE &&
      parametersDescriptor_->getBlLengthMode() == SgParameterCfg::PM_NONE  )
    return;

  ts
      << "1     Baseline information for run " << qPrintable(reportID_) << "\n"
      << "      Monument to monument values at epoch " 
      << qPrintable(t.toString(SgMJD::F_YYYYMonDD)) << "\n"
      << "      Baseline vector components: Length, Vertical and Transverse components\n"
      << "                                                Vector mag c-sigma        Length c-sigma "
      << "Horizontal c-sigma Vertical c-sigma\n"
      << "                                                      (mm)   (mm)           (mm)   (mm)"
      << "      (mm)   (mm)       (mm)   (mm)\n";
  //
  if (parametersDescriptor_->getStnCooMode() == SgParameterCfg::PM_LOC)
  {
    // stations:
    QList<SgVlbiStationInfo*>   stations;
    for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
          ++it)
      if (!it.value()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
        stations << it.value();
    //
    n = stations.size();
    for (int i=0; i<n; i++)
      for (int j=i+1; j<n; j++)
      {
        SgVlbiStationInfo      *si=stations.at(i);
        SgVlbiStationInfo      *sj=stations.at(j);
        bool                    isSi, isSj;
        isSi = si->pRx()->isAttr(SgParameter::Attr_IS_SOLVED);
        isSj = sj->pRx()->isAttr(SgParameter::Attr_IS_SOLVED);
        if (isSi || isSj)
        {
          Sg3dVector            dr_i, dr_j;
          Sg3dVector            ri_apriori(si->getR());
          Sg3dVector            rj_apriori(sj->getR());
          Sg3dVector            lhv, lhvSigma;
          double                length, lengthSigma;
          if (isSi)
            dr_i = Sg3dVector(si->pRx()->getSolution(), si->pRy()->getSolution(),
                              si->pRz()->getSolution());
          if (isSj)
            dr_j = Sg3dVector(sj->pRx()->getSolution(), sj->pRy()->getSolution(),
                              sj->pRz()->getSolution());
          if (config_->getUseExtAPrioriSitesPositions())
          {
            ri_apriori = si->getR_ea() + si->getV_ea()*(t - session_->getApStationVelocities().getT0());
            rj_apriori = sj->getR_ea() + sj->getV_ea()*(t - session_->getApStationVelocities().getT0());
          };
          SgMatrix              mA(6, 6);
          int                   idxs[6];
          if (isSi)
          {
            idxs[0] = si->pRx()->getIdx();
            idxs[1] = si->pRy()->getIdx();
            idxs[2] = si->pRz()->getIdx();
          }
          else
            idxs[0] = idxs[1] = idxs[2] = -1;
          if (isSj)
          {
            idxs[3] = sj->pRx()->getIdx();
            idxs[4] = sj->pRy()->getIdx();
            idxs[5] = sj->pRz()->getIdx();
          }
          else
            idxs[3] = idxs[4] = idxs[5] = -1;
          //
          for (int l=0; l<6; l++)
            for (int m=0; m<6; m++)
              if (idxs[l]>=0 && idxs[m]>=0)
                mA.setElement(l,m,  PxAll_->getElement(idxs[l], idxs[m]));
          //
          calcLhv(ri_apriori, dr_i, rj_apriori, dr_j, mA, lhv, lhvSigma, length, lengthSigma);
          str.sprintf(" %-8s %04d       to %-8s %04d       %14.2f %6.2f %14.2f %6.2f %9.2f %6.2f"
                      "  %9.2f %6.2f",
            qPrintable(si->getKey()), si->getCdpNumber(),
            qPrintable(sj->getKey()), sj->getCdpNumber(),
            length*1000.0, lengthSigma*1000.0,
            lhv.at(X_AXIS)*1000.0, lhvSigma.at(X_AXIS)*1000.0,
            lhv.at(Y_AXIS)*1000.0, lhvSigma.at(Y_AXIS)*1000.0,
            lhv.at(Z_AXIS)*1000.0, lhvSigma.at(Z_AXIS)*1000.0);
//        ts << str << "\n";
          ts << str << "  " << qPrintable(session_->getTMean().toString(SgMJD::F_Simple)) 
                    << "\n";
        };
      };
  }
  else if (parametersDescriptor_->getBlLengthMode() == SgParameterCfg::PM_LOC)
  {
    // baselines:
    QList<SgVlbiBaselineInfo*>  baselines;
    for (BaselinesByName_it it=session_->baselinesByName().begin(); 
        it!=session_->baselinesByName().end(); ++it)
      if (!it.value()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
        baselines << it.value();
    //
    n = baselines.size();
    for (int i=0; i<n; i++)
    {
      SgVlbiBaselineInfo       *bl=baselines.at(i);
      SgVlbiStationInfo        *si=bl->stn_1(session_->stationsByName());
      SgVlbiStationInfo        *sj=bl->stn_2(session_->stationsByName());

      Sg3dVector                db;
      Sg3dVector                ri_apriori(si->getR());
      Sg3dVector                rj_apriori(sj->getR());
      Sg3dVector                lhv, lhvSigma;
      double                    length, lengthSigma;

      db = Sg3dVector(bl->pBx()->getSolution(), bl->pBy()->getSolution(), bl->pBz()->getSolution());
      if (config_->getUseExtAPrioriSitesPositions())
      {
        ri_apriori = si->getR_ea() + si->getV_ea()*(t - session_->getApStationVelocities().getT0());
        rj_apriori = sj->getR_ea() + sj->getV_ea()*(t - session_->getApStationVelocities().getT0());
      };
      SgMatrix                  mA(3, 3);
      int                       idxs[3];
      idxs[0] = bl->pBx()->getIdx();
      idxs[1] = bl->pBy()->getIdx();
      idxs[2] = bl->pBz()->getIdx();
      //
      for (int l=0; l<3; l++)
        for (int m=0; m<3; m++)
          mA.setElement(l,m,  PxAll_->getElement(idxs[l], idxs[m]));
          //
      calcLhv(ri_apriori, rj_apriori, db, mA, lhv, lhvSigma, length, lengthSigma);
      str.sprintf(" %-8s %04d       to %-8s %04d       %14.2f %6.2f %14.2f %6.2f %9.2f %6.2f"
                  "  %9.2f %6.2f",
        qPrintable(si->getKey()), si->getCdpNumber(),
        qPrintable(sj->getKey()), sj->getCdpNumber(),
        length*1000.0, lengthSigma*1000.0,
        lhv.at(X_AXIS)*1000.0, lhvSigma.at(X_AXIS)*1000.0,
        lhv.at(Y_AXIS)*1000.0, lhvSigma.at(Y_AXIS)*1000.0,
        lhv.at(Z_AXIS)*1000.0, lhvSigma.at(Z_AXIS)*1000.0);
      ts << str << "  " << qPrintable(bl->pBx()->getTMean().toString(SgMJD::F_Simple)) 
                << "  " << QString("").sprintf("%5d", bl->numProcessed(DT_DELAY))
                << "\n";
    };
  };
  //
  ts 
    << "--\n"
    << "Note: a posteriori baseline vectors are expressed in a baseline-centric reference frames. "
    << "The first basis vector of the\nframe, l, is in direction of a priori baseline, (r_2 - r_1). "
    << "Direction of the second vector, h, is defined by a cross\nproduct of a priori baseline vector "
    << "and a priori geocentric vector of the 2nd station. The last basis vector, v, is\nperpendicular "
    << "to the vectors l and h and is radially inward at the center of the baseline. For the short "
    << "baselines, the\nvectors l and h are close to the horizontal plane and v is almost vertical "
    << "(with opposite sign). In the table above the\ncolumns are:\n * Vector mag:  length of the a "
    << "posteriori baseline;\n * Length:      l-component of the baseline;\n * Horizontal:  "
    << "h-component of the baseline;\n * Vertical:    v-component of the baseline;\n * c-sigma:     "
    << "calculated (using standard deviations of adjusted station coordinates of baselines) standard "
    << "deviations\n                of the corresponding values.\n\n";
};



//
void SgSolutionReporter::reportDeselectedObsBlock_Output4Spoolfile_v3(QTextStream& ts)
{
  QList<SgVlbiObservation*>     excludedObs, unusableObs, includedObs, allObs;
  int                           outputVersion;
  
  outputVersion = activeBand_?activeBand_->getInputFileVersion()+1:1;

  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    if (obs->primeObs() && !obs->primeObs()->isUsable())
      unusableObs << obs;
    else if (obs->primeObs() && obs->primeObs()->isUsable() &&
             obs->primeObs()->activeDelay() && 
             obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID) )
      excludedObs << obs;
    else
      includedObs << obs;
  };
  //
  if (excludedObs.size() + unusableObs.size() + includedObs.size() == 0)
  {
    ts  << "No observations are in the solution.\n";
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::reportDeselectedObsBlock_Output4Spoolfile(): no obs to report");
    return;
  };
  //
  // reorder the observations:
  for (int idx=0; idx<unusableObs.size(); idx++)
    allObs << unusableObs.at(idx);
  for (int idx=0; idx<excludedObs.size(); idx++)
    allObs << excludedObs.at(idx);
  for (int idx=0; idx<includedObs.size(); idx++)
    allObs << includedObs.at(idx);
  //
  //
  ts  << "# Status of observations of the solution of the Run " << reportID_ << "\n";
  ts  << "# Session " << session_->getOfficialName() << "/" << session_->getSessionCode()
      << " database " << session_->getName() << " version " << outputVersion << "\n";
  ts  << "# First column: a status flag:\n";
  ts  << "#    u - the observation is unusable: either missed data on one of the bands,\n";
  ts  << "#        low quality code, deselected baseline, station or sources.\n";
  ts  << "#    e - excluded observation, explicitly excluded observation either by user\n";
  ts  << "#        or by the software (due to a high residual or not so good quality factor).\n";
  ts  << "#    i - the observation was included in the solution.\n";
  ts  << "# Second column: index in a database or other media.\n";
  ts  << "# Third column: time of observation.\n";
  ts  << "# Forth and fifth columns: quality code (QC) for S- and X-bands. The char `-' means\n";
  ts  << "# no data on the band.\n";
  ts  << "# Sixth and seventh columns: fourfit error code (EC) for S- and X-bands. The char `-' means\n";
  ts  << "# no data on the band, the char '.' means no fourfit error code for the observation.\n";
  ts  << "# Eighth and ninth columns: SNR on the S- and X-bands.\n";
  ts  << "# Tenth and eleventh columns: number of used channels on the S- and X-bands.\n";
  ts  << "# Twelveth and thirteenth columns: baseline and source names.\n";
  ts  << "# The last three columns: residual and applied standard deviation (ps) and\n";
  ts  << "# normalized residual (unitless).\n";
  ts  << "#\n";
  ts  << "#                 QC  EC     SNR     SNR NumChan\n";
  ts  << "# __N__ __Time__ S X S X      S       X   S  X ____Baseline_____ _Source_ Resid.(ps) ";
  ts  << "_Std.Dev_ Normalized\n";

  QString                       strQC_S(""), strQC_X("");
  QString                       strEC_S(""), strEC_X("");
  QString                       strSnr_S(""), strSnr_X("");
  QString                       strNoC_S(""), strNoC_X("");
  QString                       str(""), sSts("");
  double                        scale4Delay(1.0e12);

  for (int idx=0; idx<allObs.size(); idx++)
  {
    SgVlbiObservation          *obs=allObs.at(idx);
    SgVlbiObservable           *o=obs->primeObs();
    strQC_S = "-";
    strQC_X = "-";
    strEC_S = "-";
    strEC_X = "-";
    strSnr_S = "       ";
    strSnr_X = "       ";
    strNoC_S = "  ";
    strNoC_X = "  ";
    sSts = "u ";
    if (obs->observableByKey().contains("S"))
    {
      strQC_S.setNum(obs->observable("S")->getQualityFactor());
      strEC_S.sprintf("%s", qPrintable(
        obs->observable("S")->getErrorCode().size()==1?obs->observable("S")->getErrorCode():"."));
      strSnr_S.sprintf("%7.1f", obs->observable("S")->getSnr());
      strNoC_S.sprintf("%2d", obs->observable("S")->getNumOfChannels());
    };
    if (obs->observableByKey().contains("X"))
    {
      strQC_X.setNum(obs->observable("X")->getQualityFactor());
      strEC_X.sprintf("%s", qPrintable(
        obs->observable("X")->getErrorCode().size()==1?obs->observable("X")->getErrorCode():"."));
      strSnr_X.sprintf("%7.1f", obs->observable("X")->getSnr());
      strNoC_X.sprintf("%2d", obs->observable("X")->getNumOfChannels());
    };
    str = QString("").sprintf("%5d ", o->getMediaIdx()+1) + obs->toString(SgMJD::F_HHMMSS) + " " + 
      strQC_S  + " " + strQC_X  + " " + 
      strEC_S  + " " + strEC_X  + " " + 
      strSnr_S + " " + strSnr_X + " " +
      strNoC_S + " " + strNoC_X + " " +
      obs->baseline()->getKey() + " " + obs->src()->getKey();
  
    if (o->isUsable())
    {
      if (o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
        sSts = "e ";
      else
        sSts = "i ";
      str +=  QString("").sprintf(" %10.1f %9.1f %8.1f",
          o->activeDelay()->getResidual()*scale4Delay,
          o->activeDelay()->sigma2Apply()*scale4Delay,
          o->activeDelay()->getResidualNorm());
    };
    ts << qPrintable(sSts + str) << "\n";
  };
  ts << "\n";
  //
  excludedObs.clear();
  unusableObs.clear();
  includedObs.clear();
  allObs.clear();
};



//
void SgSolutionReporter::reportDeselectedObsBlock_Output4Spoolfile_v2(QTextStream& ts)
{
  QList<SgVlbiObservation*>     excludedObs, unusableObs;

  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    if (obs->primeObs() && obs->primeObs()->activeDelay() &&
       !obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
    {
      if (obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID))
        excludedObs << obs;
      else
        unusableObs << obs;
    };
  };
  //
  if (excludedObs.size() + unusableObs.size() == 0)
  {
    ts  << "All observations were used in the solution.\n";
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::reportDeselectedObsBlock_Output4Spoolfile(): no deselected obs to report");
    return;
  };
  //
  //
  ts  << " Observations that are not in the solution of the Run " << reportID_ << "\n";
  ts  << "#  N __Time__ SQC XQC S_SNR_ X_SNR_ ____Baseline_____ _Source_ Resid.(ps) _Std.Dev_ NormResid\n";

  QString                       strQC_S(""), strQC_X("");
  QString                       strSnr_S(""), strSnr_X("");
  QString                       str("");
  double                        scale4Delay(1.0e12);
  for (int idx=0; idx<unusableObs.size(); idx++)
  {
    SgVlbiObservation          *obs=unusableObs.at(idx);
    SgVlbiObservable           *o=obs->primeObs();
    strQC_S = "-";
    strQC_X = "-";
    strSnr_S = "     ";
    strSnr_X = "     ";
    if (obs->observableByKey().contains("S"))
    {
      strQC_S.setNum(obs->observable("S")->getQualityFactor());
      strSnr_S.sprintf("%4.1f", obs->observable("S")->getSnr());
    };
    if (obs->observableByKey().contains("X"))
    {
      strQC_X.setNum(obs->observable("X")->getQualityFactor());
      strSnr_X.sprintf("%4.1f", obs->observable("X")->getSnr());
    };
    str = QString("").sprintf("%2d ", o->getMediaIdx()+1) + obs->toString(SgMJD::F_HHMMSS) +
          " S:" + strQC_S + " X:" + strQC_X + " S:" + strSnr_S + " X:" + strSnr_X + " " +
          obs->baseline()->getKey() + " " + obs->src()->getKey();
    ts << "u " << qPrintable(str) << "\n";
  };
  //
  for (int idx=0; idx<excludedObs.size(); idx++)
  {
    SgVlbiObservation          *obs=excludedObs.at(idx);
    SgVlbiObservable           *o=obs->primeObs();
    strQC_S = "-";
    strQC_X = "-";
    strSnr_S = "     ";
    strSnr_X = "     ";
    if (obs->observableByKey().contains("S"))
    {
      strQC_S.setNum(obs->observable("S")->getQualityFactor());
      strSnr_S.sprintf("%4.1f", obs->observable("S")->getSnr());
    };
    if (obs->observableByKey().contains("X"))
    {
      strQC_X.setNum(obs->observable("X")->getQualityFactor());
      strSnr_X.sprintf("%4.1f", obs->observable("X")->getSnr());
    };
    str = QString("").sprintf("%2d ", o->getMediaIdx()+1) + obs->toString(SgMJD::F_HHMMSS) +
      " S:" + strQC_S + " X:" + strQC_X + " S:" + strSnr_S + " X:" + strSnr_X + " " +
      obs->baseline()->getKey() + " " + obs->src()->getKey() +
      QString("").sprintf(" %10.1f %9.1f %8.1f",
        o->activeDelay()->getResidual()*scale4Delay,
        o->activeDelay()->sigma2Apply()*scale4Delay,
        o->activeDelay()->getResidualNorm());
/*
        o->measurement(config_)->getResidual()*scale4Delay,
        o->measurement(config_)->sigma2Apply()*scale4Delay,
        o->measurement(config_)->getResidualNorm());
*/
    ts << "e " << qPrintable(str) << "\n";
  };
  ts << "\n";
};



//
void SgSolutionReporter::reportDeselectedObsBlock_Output4Spoolfile(QTextStream& ts)
{
  QString                       str(""), str2copy(""), strQC_S(""), strQC_X("");
  double                        scale4Delay(1.0e12);
  QList<SgVlbiObservation*>     unusedObs;

  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    if (obs->primeObs() && obs->primeObs()->activeDelay() &&
       !obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      unusedObs << obs;
  };
  //
  if (/*excludedObs.size() + */ unusedObs.size() == 0)
  {
    ts  << "All observations were used in the solution.\n";
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::reportDeselectedObsBlock_Output4Spoolfile(): no deselected obs to report");
    return;
  };
  //
  //
  for (int idx=0; idx<unusedObs.size(); idx++)
  {
    SgVlbiObservation          *obs=unusedObs.at(idx);
    SgVlbiObservable           *o=obs->primeObs();
    strQC_S = "-";
    strQC_X = "-";
    if (obs->observableByKey().contains("S"))
      strQC_S.setNum(obs->observable("S")->getQualityFactor());
    if (obs->observableByKey().contains("X"))
      strQC_X.setNum(obs->observable("X")->getQualityFactor());

    str2copy.sprintf("observation %2d, %s, %s, %s",
      o->getMediaIdx() + 1,
      qPrintable(o->baseline()->getKey().simplified()),
      qPrintable(o->src()->getKey().simplified()),
      qPrintable(o->epoch().toString(SgMJD::F_HHMMSS))
      );
    
    str2copy.sprintf("observation %2d, %s, %s, %s, which fits at %.2f +/- %.2f ps, norm: %.2f, ",
      o->getMediaIdx() + 1,
      qPrintable(o->baseline()->getKey().simplified()),
      qPrintable(o->src()->getKey().simplified()),
      qPrintable(o->epoch().toString(SgMJD::F_HHMMSS)),
      o->activeDelay()->getResidual()*scale4Delay,
      o->activeDelay()->sigma2Apply()*scale4Delay,
      o->activeDelay()->getResidualNorm()
      );
    str.sprintf("S:%s X:%s",
      qPrintable(strQC_S), qPrintable(strQC_X));

    ts << qPrintable(str2copy + str) << "\n";
  };
  ts << "\n";
};



//
void SgSolutionReporter::reportStochasticEstimations(const QString& path)
{
  session_->createParameters();
  synchronizeInfo();
  //
  QDir                          dir(path);
  if (!dir.exists())
  {
//  if (dir.mkpath(path))
    if (dir.mkpath(dir.absolutePath()))
      logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
        "::reportStochasticEstimations(): a directory \"" + path + "\" has been created");
    else
      logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
        "::reportStochasticEstimations(): creating a directory \""  + path + "\" has been failed");
  };
  //
  if (reportStoch4Stn(path))
    logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
      "::reportStochasticEstimations(): files with station dependent stochastic parameters "
      " have been created");
  else
    logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
      "::reportStochasticEstimations(): creating files with station dependent stochastic parameters "
      " have failed");
  //
  session_->releaseParameters();
};



//
bool SgSolutionReporter::reportStoch4Stn(const QString& path)
{
  QString                       prefix("Stn_"), suffix(".dat");
  StationsByName_it             it_st;
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    reportStochParameter(si->pClock0()->getName(), parametersDescriptor_->getClock0(),
      path, prefix + si->getKey().simplified() + "_Clocks" + suffix);
    reportStochParameter(si->pZenithDelay()->getName(), parametersDescriptor_->getZenith(),
      path, prefix + si->getKey().simplified() + "_Zenith" + suffix);
    reportStochParameter(si->pAtmGradN()->getName(), parametersDescriptor_->getAtmGrad(),
      path, prefix + si->getKey().simplified() + "_AtmGrdN" + suffix);
    reportStochParameter(si->pAtmGradE()->getName(), parametersDescriptor_->getAtmGrad(),
      path, prefix + si->getKey().simplified() + "_AtmGrdE" + suffix);
    reportStochParameter(si->pRx()->getName(), parametersDescriptor_->getZenith(),
      path, prefix + si->getKey().simplified() + "_Rx" + suffix);
    reportStochParameter(si->pRy()->getName(), parametersDescriptor_->getZenith(),
      path, prefix + si->getKey().simplified() + "_Ry" + suffix);
    reportStochParameter(si->pRz()->getName(), parametersDescriptor_->getZenith(),
      path, prefix + si->getKey().simplified() + "_Rz" + suffix);
    reportStochParameter(si->pAxisOffset()->getName(), parametersDescriptor_->getZenith(),
      path, prefix + si->getKey().simplified() + "_AxsOff" + suffix);
  };
  prefix = "EOP_";
  reportStochParameter(session_->pPolusX()->getName(), parametersDescriptor_->getPolusXY(),
    path, prefix + "Px" + suffix);
  reportStochParameter(session_->pPolusY()->getName(), parametersDescriptor_->getPolusXY(),
    path, prefix + "Py" + suffix);
  reportStochParameter(session_->pUT1()->getName(), parametersDescriptor_->getPolusUT1(),
    path, prefix + "Ut1" + suffix);
  // do we need these at all:?
  reportStochParameter(session_->pNutX()->getName(), parametersDescriptor_->getPolusNut(),
    path, prefix + "CipX" + suffix);
  reportStochParameter(session_->pNutY()->getName(), parametersDescriptor_->getPolusNut(),
    path, prefix + "CipY" + suffix);
  //
  // what else I should add here?
  // ...
  return true;
};



//
bool SgSolutionReporter::reportStochParameter(const QString& key, const SgParameterCfg& parCfg,
  const QString& path, const QString& fileName)
{
  if (!stcParByName_.contains(key))
    return true; // it is ok
  
  QString                       str("");
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::reportStochParameter(): error opening output file: " + path + "/" + fileName);
    return false;
  };
  //
  // make output:
  double                        scale(parCfg.getScale());
  QString                       scaleName(parCfg.getScaleName());
  QTextStream                   ts(&f);
  const QMap<QString, SgParameter*>
                               &parByEpoch=stcParByName_.value(key);
  ts << "# Output of " << key << ", (" << scaleName << ")\n";
  for (QMap<QString, SgParameter*>::const_iterator it=parByEpoch.begin(); it!=parByEpoch.end(); ++it)
  {
    SgParameter          *par=it.value();
    if (par)
    {
      str.sprintf("%s  %.4f %.4f  %d",
        qPrintable(par->getTMean().toString(SgMJD::F_Simple)),
        par->getSolution()*scale, par->getSigma()*scale, par->getNumObs()); 
      ts << str << "\n";
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
        "::reportStochParameter(): the parameter " + key + " is NULL");
      ts << it.key() << ": the parameter \"" << key << "\" is NULL\n";
    };
  };
  //
  // close the file:
  ts.setDevice(NULL);
  f.close();
  return true;
};



//
bool SgSolutionReporter::reportTotalZenithDelays(const QString& path)
{
  QDir                          dir(path);
  if (!dir.exists())
  {
//  if (dir.mkpath(path))
    if (dir.mkpath(dir.absolutePath()))
      logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
        "::reportTotalZenithDelays(): a directory \"" + path + "\" has been created");
    else
    {
      logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
        "::reportTotalZenithDelays(): creating a directory \""  + path + "\" has been failed");
      return false;
    };
  };
  //
  QString                       str("");
  QString                       prefix("Stn_"), suffix("_Tzd.dat");
  StationsByName_it             it_st;
  for (it_st=session_->stationsByName().begin(); it_st!=session_->stationsByName().end(); ++it_st)
  {
    SgVlbiStationInfo          *si=it_st.value();
    QMap<QString, SgVlbiAuxObservation*> 
                               *auxObservationsByScan=si->auxObservationByScanId();
    //
    if (auxObservationsByScan->size())
    {
      QFile                     f(path + "/" + prefix + si->getKey().simplified() + suffix);
      if (!f.open(QIODevice::WriteOnly))
      {
        logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
          "::reportTotalZenithDelays(): error opening output file: \"" + f.fileName() + "\"");
        return false;
      };
      //
      // make output:
      QTextStream                   ts(&f);
      ts << "# Output of Total zenith delays for " << si->getKey().simplified() << ", (cm)\n";

      QMap<QString, SgVlbiAuxObservation*>::const_iterator jt=auxObservationsByScan->constBegin();
      for (int idx=0; jt!=auxObservationsByScan->constEnd(); ++jt, idx++)
      {
        SgVlbiAuxObservation   *auxObs=jt.value();
        double                  tzd, tzdSigma;
        tzd = (auxObs->getZenithDelayH() + auxObs->getZenithDelayW())*100.0 + auxObs->getEstZenithDelay();
        tzdSigma = auxObs->getEstZenithDelaySigma();
        if (tzdSigma > 0.0)
        {
          str.sprintf("%s  %.4f %.4f",
            qPrintable(auxObs->toString(SgMJD::F_Simple)), tzd, tzdSigma);
          ts << str << "\n";
        };
      };
      ts.setDevice(NULL);
      f.close();
    };
  };
  return true;
};



//
void SgSolutionReporter::report2MyFile(const QString& path, const QString& fileName)
{
  QDir                          dir(path);
  if (!dir.exists())
  {
//  if (dir.mkpath(path))
    if (dir.mkpath(dir.absolutePath()))
      logger->write(SgLogger::DBG, SgLogger::REPORT, className() +
        "::report2MyFile(): a directory \"" + path + "\" has been created");
    else
    {
      logger->write(SgLogger::WRN, SgLogger::REPORT, className() +
        "::report2MyFile(): creating a directory \""  + path + "\" has been failed");
      return;
    };
  };
  //
  QFile                         f(path + "/" + fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::report2MyFile(): error opening output file: \"" + f.fileName() + "\"");
    return;
  };
  //
  // make output:
  QTextStream                   ts(&f);
  ts << "# test outputs...\n";

  for (int i=0; i<session_->observations().size(); i++)
  {
    SgVlbiObservation          *obs=session_->observations().at(i);
    SgVlbiObservable           *o=obs->primeObs();
    SgVlbiAuxObservation       *auxObs_1=obs->auxObs_1(), *auxObs_2=obs->auxObs_2();
    QString                     str("");
//  if (obs->isAttr(SgVlbiObservation::Attr_PROCESSED) && o && auxObs_1 && auxObs_2)
    if (o && o->activeDelay() && !o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID) && 
        auxObs_1 && auxObs_2)
    {
      str.sprintf("%s  %16.4f %8.4f  %14.2f %14.2f  %14.2f %14.2f  %-8s:%-8s %-8s PP:%s",
        qPrintable(obs->toString(SgMJD::F_Simple)), 
        o->grDelay().getResidual()*1.0e12, o->grDelay().sigma2Apply()*1.0e12,
        o->getUvFrPerAsec(0)*360.0*3600.0/2.0/M_PI, o->getUvFrPerAsec(1)*360.0*3600.0/2.0/M_PI,
        auxObs_1->getParallacticAngle()*RAD2DEG, auxObs_2->getParallacticAngle()*RAD2DEG,
        qPrintable(obs->stn_1()->getKey()),
        qPrintable(obs->stn_2()->getKey()),
        qPrintable(obs->src()->getKey()),
        o->activeDelay()->isAttr(SgVlbiObservation::Attr_PROCESSED)?"Y":"N"
        );

      ts << str << "\n";
    };
  };
  ts.setDevice(NULL);
  f.close();
}
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
bool rightAscensionSortingOrderLessThan(SgVlbiSourceInfo *src1, SgVlbiSourceInfo *src2)
{
  return src1->getRA() < src2->getRA();
};



// progs/solve/basfe/*.f:
void calcLhv(const Sg3dVector& r1, const Sg3dVector& dr1, const Sg3dVector& r2, const Sg3dVector& dr2,
  const SgMatrix& r, Sg3dVector& lhv, Sg3dVector& lhvSigma, double& length, double& lengthSigma)
{
  Sg3dVector                    l,h,v;
  Sg3dVector                    b;
  SgMatrix                      a(4, 6); // matrix of the derivatives
  double                        d;

  // calculate lhv unit vectors:
  l = (b = r2-r1);
  l.unify();
  
  h = r2%r1;
  h.unify();
  
  v = l%h;
  v.unify();

  b += dr2 - dr1; // a posteriori
  length = b.module();
  
  lhv(X_AXIS) = b*l;
  lhv(Y_AXIS) = b*h;
  lhv(Z_AXIS) = b*v;
  
  a.setElement(0, 0, -l.at(X_AXIS)); // d(lhv_1)/d(x_1)
  a.setElement(0, 1, -l.at(Y_AXIS)); // d(lhv_1)/d(y_1)
  a.setElement(0, 2, -l.at(Z_AXIS)); // d(lhv_1)/d(z_1)

  a.setElement(1, 0, -h.at(X_AXIS)); // d(lhv_2)/d(x_1)
  a.setElement(1, 1, -h.at(Y_AXIS)); // d(lhv_2)/d(y_1)
  a.setElement(1, 2, -h.at(Z_AXIS)); // d(lhv_2)/d(z_1)

  a.setElement(2, 0, -v.at(X_AXIS)); // d(lhv_3)/d(x_1)
  a.setElement(2, 1, -v.at(Y_AXIS)); // d(lhv_3)/d(y_1)
  a.setElement(2, 2, -v.at(Z_AXIS)); // d(lhv_3)/d(z_1)

//a.setElement(3, 0, -1.0); // d(length)/d(x_1)
//a.setElement(3, 1, -1.0); // d(length)/d(y_1)
//a.setElement(3, 2, -1.0); // d(length)/d(z_1)
  a.setElement(3, 0,  b.at(X_AXIS)/length); // d(length)/d(b_x)
  a.setElement(3, 1,  b.at(Y_AXIS)/length); // d(length)/d(b_y)
  a.setElement(3, 2,  b.at(Z_AXIS)/length); // d(length)/d(b_z)

  // for the second station the partials are the same just with reverted sign:
  for (int i=0; i<4; i++)
    for (int j=0; j<3; j++)
      a.setElement(i, j+3, -a.getElement(i,j));
  
  // r is a matrix of covariations for (x1,y1,z1,x2,y2,z2)
  for (int k=0; k<4; k++)
  {
    d = 0.0;
    for (int i=0; i<6; i++)
    {
      d += a.getElement(k, i)*a.getElement(k, i)*r.getElement(i, i);
      for (int j=i+1; j<6; j++)
        d += 2.0*a.getElement(k, i)*a.getElement(k, j)*r.getElement(i, j);
    };
    if (d > 0.0)
    {
      if (k < 3)
        lhvSigma((DIRECTION)k) = sqrt(d);
      else
        lengthSigma = sqrt(d);
    }
    else
    {
      logger->write(SgLogger::INF, SgLogger::REPORT, "calcLhv(): "
        "the sigma2 is less than zero: " + QString("").sprintf("%g", d));
      lhvSigma((DIRECTION)k) = 1.0;
    };
  };
};



// another version: the baselines have been estimated:
void calcLhv(const Sg3dVector& r1, const Sg3dVector& r2, const Sg3dVector& db,
  const SgMatrix& r, Sg3dVector& lhv, Sg3dVector& lhvSigma, double& length, double& lengthSigma)
{
  Sg3dVector                    l,h,v;
  Sg3dVector                    b;
  SgMatrix                      a(4, 3); // matrix of the derivatives
  double                        d;

  // calculate lhv unit vectors:
  l = (b = r2-r1);
  l.unify();
  
  h = r2%r1;
  h.unify();
  
  v = l%h;
  v.unify();

  b += db; // a posteriori
  length = b.module();
  
  lhv(X_AXIS) = b*l;
  lhv(Y_AXIS) = b*h;
  lhv(Z_AXIS) = b*v;
  
  a.setElement(0, 0,  l.at(X_AXIS)); // d(lhv_1)/d(b_x)
  a.setElement(0, 1,  l.at(Y_AXIS)); // d(lhv_1)/d(b_y)
  a.setElement(0, 2,  l.at(Z_AXIS)); // d(lhv_1)/d(b_z)

  a.setElement(1, 0,  h.at(X_AXIS)); // d(lhv_2)/d(b_x)
  a.setElement(1, 1,  h.at(Y_AXIS)); // d(lhv_2)/d(b_y)
  a.setElement(1, 2,  h.at(Z_AXIS)); // d(lhv_2)/d(b_z)

  a.setElement(2, 0,  v.at(X_AXIS)); // d(lhv_3)/d(b_x)
  a.setElement(2, 1,  v.at(Y_AXIS)); // d(lhv_3)/d(b_y)
  a.setElement(2, 2,  v.at(Z_AXIS)); // d(lhv_3)/d(b_z)

//a.setElement(3, 0,  1.0); // d(length)/d(b_x)
//a.setElement(3, 1,  1.0); // d(length)/d(b_y)
//a.setElement(3, 2,  1.0); // d(length)/d(b_z)
  a.setElement(3, 0,  b.at(X_AXIS)/length); // d(length)/d(b_x)
  a.setElement(3, 1,  b.at(Y_AXIS)/length); // d(length)/d(b_y)
  a.setElement(3, 2,  b.at(Z_AXIS)/length); // d(length)/d(b_z)
  
  // r is a matrix of covariations for (b_x,b_y,b_z)
  for (int k=0; k<4; k++)
  {
    d = 0.0;
    for (int i=0; i<3; i++)
    {
      d += a.getElement(k, i)*a.getElement(k, i)*r.getElement(i, i);
      for (int j=i+1; j<3; j++)
        d += 2.0*a.getElement(k, i)*a.getElement(k, j)*r.getElement(i, j);
    };
    if (d > 0.0)
    {
      if (k < 3)
        lhvSigma((DIRECTION)k) = sqrt(d);
      else
        lengthSigma = sqrt(d);
    }
    else
    {
      logger->write(SgLogger::INF, SgLogger::REPORT, "calcLhv(): "
        "the sigma2 is less than zero: " + QString("").sprintf("%g", d));
      lhvSigma((DIRECTION)k) = 1.0;
    };
  };
};



/*=====================================================================================================*/
//
// constants:
//

/*=====================================================================================================*/
