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


#include <SgTaskManager.h>

#include <SgConstants.h>
#include <SgEstimator.h>
#include <SgLogger.h>
#include <SgPwlStorage.h>
#include <SgRefraction.h>
#include <SgVlbiBand.h>




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTaskManager::className()
{
  return "SgTaskManager";
};



// A destructor:
SgTaskManager::~SgTaskManager()
{
  if (isTaskOwner_ && task_)
  {
    delete task_;
    task_ = NULL;
  };
/*
  if (estimator_)
  {
    delete estimator_;
    estimator_ = NULL;
  };
*/
  if (estimator_)
  {
    delete estimator_;
    estimator_ = NULL;
  };
  if (isObsListOwner_ && observations_ && observations_->size())
    observations_->clear();
  if (globalParameters_)
  {
    delete globalParameters_;
    globalParameters_ = NULL;
  };
  if (arcParameters_)
  {
    delete arcParameters_;
    arcParameters_ = NULL;
  };
  if (localParameters_)
  {
    delete localParameters_;
    localParameters_ = NULL;
  };
  if (pwlParameters_)
  {
    delete pwlParameters_;
    pwlParameters_ = NULL;
  };
  if (stochasticParameters_)
  {
    delete stochasticParameters_;
    stochasticParameters_ = NULL;
  };
  if (refraction_)
  {
    delete refraction_;
    refraction_ = NULL;
  };
};



//
bool SgTaskManager::prepare4Run()
{
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    ": preparing to run the task");

  // create CALCable objects:
  refraction_ = new SgRefraction(task_->config());


  //  estimator_ = new SgEstimator(this);
  estimator_ = new SgEstimator(task_->config());

  // create the list of parameters:
  // [...]

  // create a list of VLBI sessions:
  for (SessionInfosByName_it i=task_->sessionsByName().begin(); i!=task_->sessionsByName().end(); ++i)
  {
    SgVlbiSessionInfo *si = i.value();
    if (!si->isAttr(SgVlbiSessionInfo::Attr_NOT_VALID))
      sessions_.append(si);
  };
  // ... and sort them in a chronological order:
  qSort(sessions_.begin(), sessions_.end(), lessThan4_FirstEpochSortingOrder);
  // clear the cache:
  if (observations_ && observations_->size())
    observations_->clear();

  // set up tStart and tFinis:
  tStart_ = **observations_->begin();
  tFinis_ = **observations_->end();
  if (currentSession_)
   tRefer_ = currentSession_->tRefer();
  else 
    tRefer_ = tStart_ + (tFinis_ - tStart_)/2.0;

//  estimator_->prepare4Run();
  return true;
};



//
void SgTaskManager::run(bool)
{
  SgMJD                         startDate(SgMJD::currentMJD());
  int                           obsIdx=0;
  const SgTaskConfig           *cfg=task_->config();
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::run(): data analysis procerure initiated");

  bool                          useDelays=cfg->getUseDelayType()!=SgTaskConfig::VD_NONE;
  bool                          useRates=cfg->getUseRateType()!=SgTaskConfig::VR_NONE;
  int                           nDataTypes(useDelays && useRates?2:1);
  
  estimator_->prepare2Run(nDataTypes*observations_->size(), tStart_, tFinis_, tRefer_);
  for (obsIdx=0; obsIdx<observations_->size(); obsIdx++)
  {
    SgObservation              *obs=observations_->at(obsIdx);
    if (obs->isEligible(cfg))
      obs->prepare4Analysis(this);
    estimator_->processObs(obs->getMJD(), obs->o_c(), obs->sigma());

  };
  estimator_->finisRun();

  // the executions is finished
  SgMJD                         finisDate(SgMJD::currentMJD());
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    ": data analysis procerure finished");
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    ": elapsed time for the task " + " is: " + interval2Str(finisDate - startDate) +
    " (" + QString("").sprintf("%.2f", finisDate - startDate) + " sec)");
};



//
void SgTaskManager::finisRun()
{
//  estimator_->finisRun();

  // destroy calcable objects:
  if (refraction_)
  {
    delete refraction_;
    refraction_ = NULL;
  };

  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    ": the task is finished");
};



//
void SgTaskManager::loadVlbiSession(const QString&)
{

};



//
void SgTaskManager::saveVlbiSession()
{

};



//
void SgTaskManager::updateParamaterLists()
{
  // lists of gloabal parameters:
  if (globalParameters_)
  {
    fillParameterList(globalParameters_, SgParameterCfg::PM_GLB);
    if (globalParameters_->size())
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the list of global parameters (" + QString("_").setNum(globalParameters_->size()) +
        ") has been created");
  };
  // lists of arc parameters:
  if (arcParameters_)
  {
    fillParameterList(arcParameters_, SgParameterCfg::PM_ARC);
    if (arcParameters_->size())
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the list of arc parameters (" + QString("_").setNum(arcParameters_->size()) +
        ") has been created");
  };

  // lists of local parameters:
  if (localParameters_)
  {
    fillParameterList(localParameters_, SgParameterCfg::PM_LOC);
    if (localParameters_->size())
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the list of local parameters (" + QString("_").setNum(localParameters_->size()) +
        ") has been created");
  };

  // lists of PWL parameters:
  if (pwlParameters_)
  {
    fillParameterList(pwlParameters_, SgParameterCfg::PM_PWL);
    if (pwlParameters_->size())
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the list of `picewise linear' parameters (" + QString("_").setNum(pwlParameters_->size()) +
        ") has been created");
  };

  // lists of stochastic parameters:
  if (stochasticParameters_)
  {
    fillParameterList(stochasticParameters_, SgParameterCfg::PM_STC);
    if (stochasticParameters_->size())
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the list of stochastic parameters (" + QString("_").setNum(stochasticParameters_->size()) +
        ") has been created");
  };
};



//
void SgTaskManager::fillParameterList(QList<SgParameter*>*& list, SgParameterCfg::PMode mode)
{
  // remove `in run' flag:
  for (int i=0; i<list->size(); i++)
    list->at(i)->delAttr(SgPartial::Attr_IS_IN_RUN);
  // clear the list:
  list->clear();

/*
  SgTaskConfig                 *cfg=task_->config();
  SgVlbiBand                   *band=currentSession_->bands().at(cfg->getActiveBandIdx());
*/
  // Stations:
  for (StationsByName_it it=currentSession_->stationsByName().begin();
    it!=currentSession_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
/*
    SgVlbiStationInfo          *bsi=NULL;
    if (band && band->stationsByName().contains(si->getKey()))
      bsi = band->stationsByName().value(si->getKey());
*/
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      // clocks:
      if (task_->parameters()->getClock0Mode()==mode &&
          si->getClocksModelOrder()>0 &&
          !(si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) &&
              (mode==SgParameterCfg::PM_ARC/* || mode==SgParameterCfg::PM_PWL*/)) )
      {
        if (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_CLOCKS))
          si->pClock0()->tune(si->getPcClocks());
        else
          si->pClock0()->tune(task_->parameters()->getClock0());
        // adjust a num of polinomials:
        si->pClock0()->setNumOfPolynomials(si->getClocksModelOrder());
        //
        list->append(si->pClock0());
      };
      if (task_->parameters()->getClock1Mode()==mode &&
         !(si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) && mode==SgParameterCfg::PM_ARC) )
      {
        if (si->getClocksModelOrder()>1)
          for (int i=1; i<si->getClocksModelOrder(); i++)
          {
            si->pClocks(i)->tune(task_->parameters()->getClock1());
            list->append(si->pClocks(i));
          };
      };
      // add dynamic clock break parameters:
      if (mode==SgParameterCfg::PM_LOC)
      {
        for (int i=0; i<si->clockBreaks().size(); i++)
        {
          SgParameterBreak     *pb=si->clockBreaks().at(i);
          if (pb->isAttr(SgParameterBreak::Attr_DYNAMIC))
          {
            if (si->getClocksModelOrder()>0)
            {
              pb->pA0()->tune(SgParameterCfg::PM_LOC, task_->parameters()->getClock0());
              list->append(pb->pA0());
            }
            else
              logger->write(SgLogger::WRN, SgLogger::RUN, className() +
                "::fillParameterList: an attempt to register clock break parameter for station " +
                si->getKey() + " with turned off clocks");
            if (si->getClocksModelOrder()>1)
            {
              pb->pA1()->tune(SgParameterCfg::PM_LOC, task_->parameters()->getClock1());
              list->append(pb->pA1());
            };
            if (si->getClocksModelOrder()>2)
            {
              pb->pA2()->tune(SgParameterCfg::PM_LOC, task_->parameters()->getClock1());
              list->append(pb->pA2());
            };
          };
        };
      };
      // tropospheric parameters:
      if (task_->parameters()->getZenithMode() == mode &&
          !si->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS))
      {
        if (si->isAttr(SgVlbiStationInfo::Attr_USE_LOCAL_ZENITH))
          si->pZenithDelay()->tune(si->getPcZenith());
        else
          si->pZenithDelay()->tune(task_->parameters()->getZenith());
        list->append(si->pZenithDelay());
      };
      if (task_->parameters()->getAtmGradMode() == mode &&
          !si->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS))
      {
        si->pAtmGradE()->tune(task_->parameters()->getAtmGrad());
        si->pAtmGradN()->tune(task_->parameters()->getAtmGrad());
        list->append(si->pAtmGradE());
        list->append(si->pAtmGradN());
      };
      // cooridinates:
      if (task_->parameters()->getStnCooMode() == mode &&
          si->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO))
      {
        si->pRx()->tune(task_->parameters()->getStnCoo());
        si->pRy()->tune(task_->parameters()->getStnCoo());
        si->pRz()->tune(task_->parameters()->getStnCoo());
        list->append(si->pRx());
        list->append(si->pRy());
        list->append(si->pRz());
      };
      // axis offset:
      if (task_->parameters()->getAxisOffsetMode() == mode &&
          si->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_AXO))
      {
        si->pAxisOffset()->tune(task_->parameters()->getAxisOffset());
        list->append(si->pAxisOffset());
      };
    };
  };

  // Sources:
  for (SourcesByName_it it=currentSession_->sourcesByName().begin();
    it!=currentSession_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo           *si=it.value();
    if ( !si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO) &&
          task_->parameters()->getSrcCooMode() == mode )
    {
      si->pRA()->tune(task_->parameters()->getSrcCoo());
      si->pDN()->tune(task_->parameters()->getSrcCoo());
      list->append(si->pRA());
      list->append(si->pDN());
    };

    if (  task_->config()->getHave2ApplySourceSsm() &&
         !si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID)  &&
          si->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM)  &&
          task_->parameters()->getSrcSsmMode() == mode )
    {
/*
      if (si->getSmtType() == SgVlbiSourceInfo::SMT_TWO_POINTS)
      {
        if (si->getEstimateRatio())
        {
          si->pK()->tune(task_->parameters()->getSrcSsm());
          list->append(si->pK());
        };
        if (si->getEstimateSpIdx())
        {
          si->pB()->tune(task_->parameters()->getSrcSsm());
          list->append(si->pB());
        };
        if (si->getEstimatePosition())
        {
          si->pX()->tune(task_->parameters()->getSrcCoo());
          si->pY()->tune(task_->parameters()->getSrcCoo());
          list->append(si->pX());
          list->append(si->pY());
        };
        if (si->isExtSS())
        {
          si->pA0()->tune(task_->parameters()->getSrcCoo());
          si->pB0()->tune(task_->parameters()->getSrcCoo());
          si->pTheta0()->tune(task_->parameters()->getSrcSsm());
          si->pA1()->tune(task_->parameters()->getSrcCoo());
          si->pB1()->tune(task_->parameters()->getSrcCoo());
          si->pTheta1()->tune(task_->parameters()->getSrcSsm());

          if (si->getEstimateA0())
            list->append(si->pA0());
          
          if (si->getEstimateB0())
            list->append(si->pB0());
          
          if (si->getEstimateT0())
            list->append(si->pTheta0());
          
          if (si->getEstimateA1())
            list->append(si->pA1());
          
          if (si->getEstimateB1())
            list->append(si->pB1());
          
          if (si->getEstimateT1())
            list->append(si->pTheta1());
        };
      }
      else if (si->getSmtType() == SgVlbiSourceInfo::SMT_MULTIPLE_POINTS)
*/
      {
        si->setIsSsModelEstimating(false);
        for (int i=0; i<si->sModel().size(); i++)
        {
          if (si->sModel()[i].getEstimateRatio())
          {
            si->sModel()[i].pK()->tune(task_->parameters()->getSrcSsm());
            list->append(si->sModel()[i].pK());
            if (!si->getIsSsModelEstimating())
              si->setIsSsModelEstimating(true); 
          };
          if (si->sModel()[i].getEstimateSpIdx())
          {
            si->sModel()[i].pB()->tune(task_->parameters()->getSrcSsm());
            list->append(si->sModel()[i].pB());
            if (!si->getIsSsModelEstimating())
              si->setIsSsModelEstimating(true);
          };
          if (si->sModel()[i].getEstimatePosition())
          {
            si->sModel()[i].pX()->tune(task_->parameters()->getSrcCoo());
            si->sModel()[i].pY()->tune(task_->parameters()->getSrcCoo());
            list->append(si->sModel()[i].pX());
            list->append(si->sModel()[i].pY());
            if (!si->getIsSsModelEstimating())
              si->setIsSsModelEstimating(true);
          };
        };
      };
    };
  };

  // Baselines:
  for (BaselinesByName_it it=currentSession_->baselinesByName().begin();
    it!=currentSession_->baselinesByName().end(); ++it)
  {
    SgVlbiBaselineInfo *bi = it.value();
    // baseline clock offset:
    if ( !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID) &&
          bi->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS) &&
          task_->parameters()->getBlClockMode() == mode )
    {
      bi->pClock()->tune(task_->parameters()->getBlClock());
      list->append(bi->pClock());
    };
    // vector of a baseline:
    if ( !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID) &&
          task_->parameters()->getBlLengthMode() == mode )
    {
      bi->pBx()->tune(task_->parameters()->getBlLength());
      bi->pBy()->tune(task_->parameters()->getBlLength());
      bi->pBz()->tune(task_->parameters()->getBlLength());
      list->append(bi->pBx());
      list->append(bi->pBy());
      list->append(bi->pBz());
    };
  };

  // Session-wide:
  if (task_->parameters()->getPolusXYMode() == mode)
  {
    currentSession_->pPolusX()->tune(task_->parameters()->getPolusXY());
    currentSession_->pPolusY()->tune(task_->parameters()->getPolusXY());
    list->append(currentSession_->pPolusX());
    list->append(currentSession_->pPolusY());
  };
  if (task_->parameters()->getPolusXYRateMode() == mode)
  {
    currentSession_->pPolusXRate()->tune(task_->parameters()->getPolusXYRate());
    currentSession_->pPolusYRate()->tune(task_->parameters()->getPolusXYRate());
    list->append(currentSession_->pPolusXRate());
    list->append(currentSession_->pPolusYRate());
  };
  if (task_->parameters()->getPolusUT1Mode() == mode)
  {
    currentSession_->pUT1()->tune(task_->parameters()->getPolusUT1());
    list->append(currentSession_->pUT1());
  };
  if (task_->parameters()->getPolusUT1RateMode() == mode)
  {
    currentSession_->pUT1Rate()->tune(task_->parameters()->getPolusUT1Rate());
    list->append(currentSession_->pUT1Rate());
  };
  if (task_->parameters()->getPolusNutMode() == mode)
  {
    currentSession_->pNutX()->tune(task_->parameters()->getPolusNut());
    currentSession_->pNutY()->tune(task_->parameters()->getPolusNut());
    list->append(currentSession_->pNutX());
    list->append(currentSession_->pNutY());
  };
  if (task_->parameters()->getPolusNutRateMode() == mode)
  {
    currentSession_->pNutXRate()->tune(task_->parameters()->getPolusNutRate());
    currentSession_->pNutYRate()->tune(task_->parameters()->getPolusNutRate());
    list->append(currentSession_->pNutXRate());
    list->append(currentSession_->pNutYRate());
  };
  
  // rise the `in run' flag:
  for (int i=0; i<list->size(); i++)
    list->at(i)->addAttr(SgPartial::Attr_IS_IN_RUN);
  // ... and sort it:
  qSort(list->begin(), list->end(), lessThan4_ParameterByNameSortingOrder);
};



//
int SgTaskManager::constrainClocks()
{
  int                           numOfConstrainedClocks, numOfConstrains;
  //
  numOfConstrainedClocks = numOfConstrains = 0;
  for (StationsByName_it it=currentSession_->stationsByName().begin();
    it!=currentSession_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    //
    if (si->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) &&
        si->pClock0()->getPMode() != SgParameterCfg::PM_ARC   )
    {
      if (si->pClock0()->isAttr(SgPartial::Attr_IS_IN_RUN))
      {
        si->pClock0()->setD(1.0);
        estimator_->processConstraint(0.0, 1.0e-15);
        numOfConstrains++;
      };
      if (si->pClock0()->getPMode() == SgParameterCfg::PM_LOC)
      {
        for (int i=1; i<si->getClocksModelOrder(); i++)
        {
          if (si->pClocks(i)->isAttr(SgPartial::Attr_IS_IN_RUN))
          {
            si->pClocks(i)->setD(1.0);
            estimator_->processConstraint(0.0, 1.0e-16);
            numOfConstrains++;
          };
        };
      }
      else if (si->pClock0()->getPMode() == SgParameterCfg::PM_PWL)
      {
        SgPwlStorage*           pwl=NULL;
        if (estimator_->pwlStorageByName()->contains(si->pClock0()->getName()))
        {
          pwl = estimator_->pwlStorageByName()->value(si->pClock0()->getName());
          if (pwl)
          {
            for (int i=0; i<pwl->getNumOfPolynomials(); i++)
            {
              pwl->getP_Ai(i)->setD(1.0);
              estimator_->processConstraint(0.0, 1.0e-15);
              numOfConstrains++;
            };
          }
          else
            logger->write(SgLogger::WRN, SgLogger::RUN, className() +
              ": the PWL container is NULL: station " + si->getKey());
        };
      };
      numOfConstrainedClocks++;
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        ": the clocks of the station " + si->getKey() + " have been constrained");
    };
  };
  if (!numOfConstrainedClocks)
  {
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": there is no reference clocks on the session " + currentSession_->getName() +
      ", the solution could fail");
  };
  if (numOfConstrainedClocks>1)
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      ": more than one station has been used as a 'reference clocks' on the session " +
      currentSession_->getName() + ", the solution could fail");
  return numOfConstrains;
};



//
int SgTaskManager::constrainTroposphericParameters()
{
  QString                       key;
  int                           numOfConstrains=0;
  SgVlbiBaselineInfo           *bi=NULL;
  SgVlbiStationInfo            *si_1=NULL, *si_2=NULL;
  for (BaselinesByName_it it=currentSession_->baselinesByName().begin();
                         it!=currentSession_->baselinesByName().end(); ++it)
  {
    bi = it.value();
    if (bi->isAttr(SgVlbiBaselineInfo::Attr_BIND_TROPOSPHERE))
    {
      si_1 = si_2 = NULL;
      key = bi->getKey().mid(0, 8);
      if (currentSession_->stationsByName().contains(key))
        si_1 = currentSession_->stationsByName().value(key);
      key = bi->getKey().mid(9, 8);
      if (currentSession_->stationsByName().contains(key))
        si_2 = currentSession_->stationsByName().value(key);
      if (si_1 && si_2)
      {
        if (si_1->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN) &&
            si_2->pZenithDelay()->isAttr(SgPartial::Attr_IS_IN_RUN)  )
        {
          si_1->pZenithDelay()->setD( 1.0);
          si_2->pZenithDelay()->setD(-1.0);
          estimator_->processConstraint(0.0, 1.0e-6);
          numOfConstrains++;
          logger->write(SgLogger::DBG, SgLogger::RUN, className() +
            ": the trpospheric parameters of the stations " + si_1->getKey() + " and " +
            si_2->getKey() + " have been binded");
        };
      }
      else 
      {
        if (!si_1)
          logger->write(SgLogger::ERR, SgLogger::RUN, className() +
            ": cannot find in the map the first station of the baseline " + bi->getKey());
        if (!si_2)
          logger->write(SgLogger::ERR, SgLogger::RUN, className() +
            ": cannot find in the map the second station of the baseline " + bi->getKey());
      };
    };
  };
  return numOfConstrains;
};



//
int SgTaskManager::constrainStcPositionsNNT()
{
  int           numOfConstrains=0;
  double        sigma = 1.0E-9; // meters
  QList<SgVlbiStationInfo*>   stations4constraints;
  for (StationsByName_it i=currentSession_->stationsByName().begin();
    i!=currentSession_->stationsByName().end(); ++i)
  {
    SgVlbiStationInfo *si = i.value();
    if ( !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO) )
      stations4constraints.append(si);
  };
  // check it:
  if (!stations4constraints.size())
    return 0;
  //
  // ok, the list is not empty, process constraints:
  // No Net Translation of Position:
  //    * x-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRx()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Translation of Positions Constraints: (X-coord) of the station [" + 
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * y-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRy()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Translation of Positions Constraints: (Y-coord) of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * z-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRz()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Translation of Positions Constraints: (Z-coord) of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  return numOfConstrains;
};



//
int SgTaskManager::constrainStcPositionsNNR()
{
  double        r2;
  int           numOfConstrains=0;
  double        sigma = 1.0E-8*SEC2RAD; // rotation is in radians
  QList<SgVlbiStationInfo*>   stations4constraints;
  for (StationsByName_it i=currentSession_->stationsByName().begin();
    i!=currentSession_->stationsByName().end(); ++i)
  {
    SgVlbiStationInfo *si = i.value();
    if ( !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO) )
      stations4constraints.append(si);
  };
  // check it:
  if (!stations4constraints.size())
    return 0;
  //
  // ok, the list is not empty, process constraints:
  //
  // No Net Rotation of Position:
  //    * x-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR();
    si->pRz()->setD( si->getR().at(Y_AXIS)/r2);
    si->pRy()->setD(-si->getR().at(Z_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Rotation of Positions Constraints: (X-coord) of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * y-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR();
    si->pRx()->setD( si->getR().at(Z_AXIS)/r2);
    si->pRz()->setD(-si->getR().at(X_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Rotation of Positions Constraints: (Y-coord) of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * z-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR();
    si->pRy()->setD( si->getR().at(X_AXIS)/r2);
    si->pRx()->setD(-si->getR().at(Y_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No-Net-Rotation of Positions Constraints: (Z-coord) of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;

  return numOfConstrains;
};


/*
int SgTaskManager::constrainStationsPositions()
{
  int           numOfConstrains=0;
  double        sigma = 1.0E-7; // meters
  QList<SgVlbiStationInfo*>   stations4constraints;
  for (StationsByName_it i=currentSession_->stationsByName().begin();
    i!=currentSession_->stationsByName().end(); ++i)
  {
    SgVlbiStationInfo *si = i.value();
    if ( !si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO) )
      stations4constraints.append(si);
  };
  // check it:
  if (!stations4constraints.size())
    return 0;
  //
  // ok, the list is not empty, process constraints:
  // 1) No Net Translation of Position:
  //    * x-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRx()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Translation of Position Constraints: used X-coord of the station [" + 
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * y-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRy()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Translation Position Constraints: used Y-coord of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * z-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    si->pRz()->setD(1.0);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Translation Position Constraints: used Y-coord of the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;

  //
  // 2) No Net Rotation of Position:
  double        r2, r_scale=6378136.6*6378136.6;
  //  r_scale = 1.0;
  //    * x-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR()/r_scale;
    si->pRz()->setD( si->getR().at(Y_AXIS)/r2);
    si->pRy()->setD(-si->getR().at(Z_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Rotation of Position Constraints: (X-coord) used the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * y-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR()/r_scale;
    si->pRx()->setD( si->getR().at(Z_AXIS)/r2);
    si->pRz()->setD(-si->getR().at(X_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Rotation of Position Constraints: (Y-coord) used the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * z-coo:
  for (int i=0; i<stations4constraints.size(); i++)
  {
    SgVlbiStationInfo *si = stations4constraints.at(i);
    r2 = si->getR()*si->getR()/r_scale;
    si->pRy()->setD( si->getR().at(X_AXIS)/r2);
    si->pRx()->setD(-si->getR().at(Y_AXIS)/r2);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": TRF No Net Rotation of Position Constraints: (Z-coord) used the station [" +
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;

  return numOfConstrains;
};
*/


//
int SgTaskManager::constrainSourcesPositions()
{
  int           numOfConstrains=0;
  double        sigma = 1.0E-8*SEC2RAD; // arc sec
  QList<SgVlbiSourceInfo*>   sources4constraints;
  for (SourcesByName_it i=currentSession_->sourcesByName().begin();
    i!=currentSession_->sourcesByName().end(); ++i)
  {
    SgVlbiSourceInfo *si = i.value();
    if ( !si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO) )
      sources4constraints.append(si);
  };
  // check it:
  if (!sources4constraints.size())
    return 0;
  // ok, the list is not empty, process constraints:
  // 1) No Net Translation Position:
  double        s_ra=0.0, c_ra=0.0, s_dn=0.0, c_dn=0.0;
  //    * (1):
  for (int i=0; i<sources4constraints.size(); i++)
  {
    SgVlbiSourceInfo *si = sources4constraints.at(i);
    sincos(si->getRA(), &s_ra, &c_ra);
    sincos(si->getDN(), &s_dn, &c_dn);
    si->pDN()->setD( s_ra          );
    si->pRA()->setD(-s_dn*c_dn*c_ra);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": CRF No Net Source Rotation Position Constraints (1): used the source [" + 
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * (2):
  for (int i=0; i<sources4constraints.size(); i++)
  {
    SgVlbiSourceInfo *si = sources4constraints.at(i);

    sincos(si->getRA(), &s_ra, &c_ra);
    sincos(si->getDN(), &s_dn, &c_dn);
    si->pDN()->setD(c_ra          );
    si->pRA()->setD(s_dn*c_dn*s_ra);
    //      Source->p_RA()->setD(-s_dn*c_dn*s_ra, TZero);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": CRF No Net Source Rotation Position Constraints (2): used the source [" + 
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  //    * (3):
  for (int i=0; i<sources4constraints.size(); i++)
  {
    SgVlbiSourceInfo *si = sources4constraints.at(i);
    c_dn = cos(si->getDN());
    si->pRA()->setD(c_dn*c_dn);
    //      Source->p_DN()->setD(c_dn*c_dn, TZero);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      ": CRF No Net Source Rotation Position Constraints (3): used the source [" + 
      si->getKey() + "]");
  };
  estimator_->processConstraint(0.0, sigma);
  numOfConstrains++;
  return numOfConstrains;
};




int SgTaskManager::constrainSourcesTmp()
{
  int                           numOfConstrains=0;
/*
  double                        sigma = 1.0E-12;
  QList<SgVlbiSourceInfo*>      sources4constraints;
  for (SourcesByName_it i=currentSession_->sourcesByName().begin();
    i!=currentSession_->sourcesByName().end(); ++i)
  {
    SgVlbiSourceInfo           *si=i.value();
    if ( !si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID) &&
          si->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM) )
      sources4constraints.append(si);
  };
  // check it:
  if (!sources4constraints.size())
    return 0;
  //
  for (int i=0; i<sources4constraints.size(); i++)
  {
    SgVlbiSourceInfo           *si=sources4constraints.at(i);

    for (int i=0; i<si->sModel().size(); i++)
    {
      si->sModel()[i].pG()->setD(1.0);
    };
    estimator_->processConstraint(1.0, sigma);
    numOfConstrains++;
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::constrainSourcesTmp(): processed G parameter for \"" +   si->getKey() + "\"");
  };
*/
  return numOfConstrains;
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
