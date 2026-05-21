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


#include <SgSingleSessionTaskManager.h>


#include <Sg3dVector.h>
#include <SgConstants.h>
#include <SgEstimator.h>
#include <SgLogger.h>
#include <SgRefraction.h>
#include <SgVlbiBand.h>



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
const QString SgSingleSessionTaskManager::className()
{
  return "SgSingleSessionTaskManager";
};



//
bool SgSingleSessionTaskManager::prepare4Run()
{
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::prepare4Run(): preparing to run the task");

  // create CALCable objects:
  refraction_ = new SgRefraction(task_->config());


  estimator_ = new SgEstimator(task_->config());

  // create the list of parameters:
  globalParameters_ = new QList<SgParameter*>;
  arcParameters_ = new QList<SgParameter*>;
  localParameters_ = new QList<SgParameter*>;
  pwlParameters_ = new QList<SgParameter*>;
  stochasticParameters_ = new QList<SgParameter*>;

  // a list of VLBI sessions, just check that there is only one session, warn user if it is not true:
  if (task_->sessionsByName().size()==0)
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::prepare4Run(): there is no VLBI session to process");
    return false;
  };
  if (task_->sessionsByName().size()>1)
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::prepare4Run(): too many sessions in the task");

  if (!currentSessionInfo_)
  {
    currentSessionInfo_ = task_->sessionsByName().begin().value();
    // ok, we expect that that should be already seted up, so notify a user:
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::prepare4Run(): the current sessionInfo object has been adjusted");
  };
  if (!currentSession_)
  {
    // ok, we expect that that should be already seted up, so notify a user:
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::prepare4Run(): the current session object is not defined");
    return false;
  };

  // check the cache, it should be prepared before prepare4Run() call:
  if (!observations_)
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::prepare4Run(): the pointer on observations is NULL");
    return false;
  };
  if (!observations_->size())
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::prepare4Run(): there is no any observations in the container of observations");
    return false;
  };

  tStart_ = *observations_->first();
  tFinis_ = *observations_->last();
  tRefer_ = currentSession_->tRefer();


  // prepare the session:
  currentSession_->prepare4Analysis();
  currentSession_->createParameters();

  // should we clear them?
  // clear estimated parameters:
  for (StationsByName_it it=currentSession_->stationsByName().begin(); 
    it!=currentSession_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *si=it.value();
    si->prepare2Run(task_->config(), refraction_);
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      if (task_->parameters()->getClock0Mode() == SgParameterCfg::PM_NONE ||
          task_->parameters()->getClock0Mode() == SgParameterCfg::PM_LOC // ||
//        task_->parameters()->getClock0Mode() == SgParameterCfg::PM_PWL
         )
      {
        si->clearEstClockModel();
        logger->write(SgLogger::DBG, SgLogger::RUN, className() +
          "::prepare4Run(): clocks have been reset for " + si->getKey());
      };
      //
      // if the num of polynomials was decreased, clear higher terms:
      for (int i=si->getClocksModelOrder(); i<si->maxNumOfPolynomials(); i++)
        si->setEstClockModel(i, 0.0, 0.0);
        //
      if (task_->parameters()->getZenithMode() == SgParameterCfg::PM_NONE  ||
          task_->parameters()->getZenithMode() == SgParameterCfg::PM_LOC //  ||
//        task_->parameters()->getZenithMode() == SgParameterCfg::PM_PWL   ||
//        task_->parameters()->getZenithMode() == SgParameterCfg::PM_STC  
         )
        si->clearEstWetZenithDelay();
    };
  };
  //
  //
  // eow?
  return true;
};



//
void SgSingleSessionTaskManager::run(bool haveProcessAllBands)
{
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::run(): data analysis procedure initiated", true);
  startRunEpoch_ = SgMJD::currentMJD();
  if (have2InteractWithGui_ && 
    !(longOperationStart_ && longOperationProgress_ && longOperationStop_ && longOperationMessage_ &&
      longOperationShowStats_))
    have2InteractWithGui_ = false;

  SgVlbiBand                   *band=NULL; 
  SgTaskConfig                 *cfg=task_->config();
  int                           obsIdx=0;
  bool                          isNeedReweighting=cfg->getDoWeightCorrection();
  bool                          isRoughAnalysis=cfg->getUseDelayType()==SgTaskConfig::VD_SB_DELAY;
  bool                          useDelays=cfg->getUseDelayType()!=SgTaskConfig::VD_NONE;
  bool                          useRates=cfg->getUseRateType()!=SgTaskConfig::VR_NONE;
  
  // set up parameters lists:
  updateParamaterLists();
  int                           nDataTypes(useDelays && useRates?2:1);
  estimator_->prepare2Run(nDataTypes*observations_->size(), tStart_, tFinis_, tRefer_);
  //
  //
  // set up constraints:
  if (task_->parameters()->getClock0Mode() != SgParameterCfg::PM_NONE)
    numOfConstraints_ += constrainClocks();
  if (task_->parameters()->getZenithMode() != SgParameterCfg::PM_NONE)
    numOfConstraints_ += constrainTroposphericParameters();
  if (task_->parameters()->getStnCooMode() != SgParameterCfg::PM_NONE)
  {
    numOfConstraints_ += constrainStcPositionsNNT();
    if (task_->parameters()->getPolusUT1Mode() != SgParameterCfg::PM_NONE ||
        task_->parameters()->getPolusXYMode() != SgParameterCfg::PM_NONE   )
      numOfConstraints_ += constrainStcPositionsNNR();
  };
  if (task_->parameters()->getSrcCooMode() != SgParameterCfg::PM_NONE)
    numOfConstraints_ += constrainSourcesPositions();
  //
  //
  // collect eligible observations:
  QList<SgObservation*>         eligibleObservations;
  for (obsIdx=0; obsIdx<observations_->size(); obsIdx++)
  {
    SgObservation              *obs=observations_->at(obsIdx);
    obs->delAttr(SgObservation::Attr_PROCESSED);
    obs->evaluateTheoreticalValues(this);
    if (obs->isEligible(cfg))
    {
      eligibleObservations << obs;
    }
  };
  // and process them:
  // GUI: 
  //
  //------------------- begin of data processing -------------------
  //
  if (have2InteractWithGui_)
    (*longOperationStart_)(0, eligibleObservations.size(), "Processing observations");
  for (obsIdx=0; obsIdx<eligibleObservations.size(); obsIdx++)
  {
    SgObservation              *obs=eligibleObservations.at(obsIdx);
    // delay:
    if (useDelays)
    {
      cfg->setDataType(SgTaskConfig::VDT_DELAY);
      obs->prepare4Analysis(this);
      estimator_->processObs(*obs, obs->o_c(), obs->sigma());
    };
    // rate:
    if (useRates)
    {
      cfg->setDataType(SgTaskConfig::VDT_RATE);
      obs->prepare4Analysis(this);
      estimator_->processObs(*obs, obs->o_c(), obs->sigma());
    };
    //
    // tell GUI about progress:
    if (have2InteractWithGui_ && (obsIdx%400==0))
      (*longOperationProgress_)(obsIdx);
  };

  // GUI: 
  if (have2InteractWithGui_)
    (*longOperationStop_)();
  eligibleObservations.clear();
  estimator_->finisRun();
  //
  //------------------- observations are processed -------------------
  //
  //
  //
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::run(): the solution is obtained, starting evaluation of residuals and statistics", true);
  //
  numOfParameters_ =  estimator_->allRegularParametersList()->size() +
                      estimator_->stochasticParametersList()->size();
  //-------------------------------------------------------------------
  //
  // evaluate residuals:
  //
  for (obsIdx=0; obsIdx<observations_->size(); obsIdx++)
  {
    SgObservation              *obs=observations_->at(obsIdx);
    estimator_->prepareStochasticSolution4Epoch(*obs, stochasticParameters_);
    obs->evaluateResiduals(this); // <--- need to optimize a little bit..
  };
  //
  // update statistics:
  for (int iBand=0; iBand<currentSession_->numberOfBands(); iBand++)
  {
    band = currentSession_->bands().at(iBand);
    //
    if (isNeedReweighting && cfg->getWcMode()==SgTaskConfig::WCM_BAND)
      band->recalcSigma2add(cfg);
    //
    if (haveProcessAllBands || iBand==cfg->getActiveBandIdx())
    {
      switch (cfg->getOpMode())
      {
      default:
      case SgTaskConfig::OPM_BAND:
        band->calcNormalizedResiduals(band->observables(), cfg);
        break;
      case SgTaskConfig::OPM_BASELINE:
        for (BaselinesByName_it it=band->baselinesByName().begin(); it!=band->baselinesByName().end();
          ++it)
        {
          SgVlbiBaselineInfo     *baselineInfo=it.value();
          if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
            baselineInfo->calcNormalizedResiduals(baselineInfo->observables(), cfg);
        };
        break; 
      };
    };
  };
  //
  //
  //
  // propagate statistics to session-wide structures:
  QString                       sLabel("");
  band = currentSession_->bands().at(cfg->getActiveBandIdx());
  //
  if (band)
  {
    sLabel = band->getKey();
//  if (currentSession_->isAttr(SgVlbiSession::Attr_FF_ION_C_CALCULATED))
    if (currentSession_->isAttr(SgVlbiSession::Attr_HAS_IONO_CORR))
    {
      for (int i=0; i<currentSession_->bands().size(); i++)
        if (currentSession_->bands().at(i)->getKey() != band->getKey())
          sLabel += "/" + currentSession_->bands().at(i)->getKey();
    };
    sLabel += ":";
    if (longOperationShowStats_)
      (*longOperationShowStats_)(sLabel, band->numTotal(DT_DELAY), band->numUsable(DT_DELAY), 
        band->numProcessed(DT_DELAY), band->wrms(DT_DELAY)*1.0e12, band->reducedChi2(DT_DELAY));
    
    // stations infos:
    for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value(), *sni=NULL;
      if (si && (sni=currentSession_->stationsByName().value(si->getKey())))
      {
        sni->copyStatistics(si);
        if (!isRoughAnalysis && sni->numProcessed(DT_DELAY)<4 &&
          !sni->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
        {
          sni->addAttr(SgVlbiStationInfo::Attr_NOT_VALID);
          logger->write(SgLogger::WRN, SgLogger::RUN, className() +
            "::run(): the station " + sni->getKey() + 
            " was deselected from the solution due to a small number of observations");
        };
      };
    };
    // sources infos:
    for (SourcesByName_it it=band->sourcesByName().begin(); it!=band->sourcesByName().end(); ++it)
    {
      SgVlbiSourceInfo *si=it.value(), *sni=NULL;
      if (si && (sni=currentSession_->sourcesByName().value(si->getKey())))
      {
        sni->copyStatistics(si);
        double                  ra(sni->getRA()), dn(sni->getDN());
        if (cfg->getUseExtAPrioriSourcesPositions())
        {
          ra = sni->getRA_ea();
          dn = sni->getDN_ea();
        };
        sni->setAl2Estd(SgVlbiSourceInfo::arcLength(ra, dn,
          ra+sni->pRA()->getSolution(), dn+sni->pDN()->getSolution()));
      };
    };
    // baselines infos:
    for (BaselinesByName_it it=band->baselinesByName().begin(); it!=band->baselinesByName().end(); ++it)
    {
      SgVlbiBaselineInfo       *bi=it.value(), *bni=NULL;
//    bi->calculateTest(cfg);
      bi->calculateClockF1(cfg);
      bi->calculateClockF2(cfg);
      if (bi && (bni=currentSession_->baselinesByName().value(bi->getKey())))
      {
        if (isNeedReweighting && 
            cfg->getWcMode()==SgTaskConfig::WCM_BASELINE &&
            !bni->isAttr(SgVlbiBaselineInfo::Attr_SKIP_WC) )
        {
          bi->recalcSigma2add(cfg);
          bni->copySigmas2add(bi);
        };
        //
        // special case for rates (set default sig0 if rates were not in the solution):
        if (!useRates && bni->getSigma2add(DT_RATE)==0.0)
          bni->setSigma2add(DT_RATE, 100.0e-15);
        //
        bni->copyStatistics(bi);
        //
        if (!isRoughAnalysis && bni->numProcessed(DT_DELAY)<4 && 
          !bni->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        {
          bni->addAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);
          logger->write(SgLogger::WRN, SgLogger::RUN, className() +
            "::run(): the baseline " + bni->getKey() + 
            " was deselected from the solution due to a small number of observations");
        };
      };
    };
  };
  //-------------------------- ??????
  //
  for (SourcesByName_it it=currentSession_->sourcesByName().begin(); 
    it!=currentSession_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo           *si=it.value();
    if (!si->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID) &&
         si->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM)
       )
    {
/*
      if (si->getSmtType() == SgVlbiSourceInfo::SMT_TWO_POINTS)
      {
        if (si->pX() && si->pY() && si->pK() && si->pB())
        {
          si->setK(si->getK() + si->pK()->getSolution());
          si->setB(si->getB() + si->pB()->getSolution());
          si->setX(si->getX() + si->pX()->getSolution());
          si->setY(si->getY() + si->pY()->getSolution());
          si->setKsig(si->pK()->getSigma());
          si->setBsig(si->pB()->getSigma());
          si->setXsig(si->pX()->getSigma());
          si->setYsig(si->pY()->getSigma());

          if (si->isExtSS())
          {
            si->setA0(si->getA0() + si->pA0()->getSolution());
            si->setB0(si->getB0() + si->pB0()->getSolution());
            si->setTheta0(si->getTheta0() + si->pTheta0()->getSolution());
            si->setA1(si->getA1() + si->pA1()->getSolution());
            si->setB1(si->getB1() + si->pB1()->getSolution());
            si->setTheta1(si->getTheta1() + si->pTheta1()->getSolution());
            
            si->setA0Sig(si->pA0()->getSigma());
            si->setB0Sig(si->pB0()->getSigma());
            si->setTheta0Sig(si->pTheta0()->getSigma());
            si->setA1Sig(si->pA1()->getSigma());
            si->setB1Sig(si->pB1()->getSigma());
            si->setTheta1Sig(si->pTheta1()->getSigma());
          };

          if (si->isExtSS())
            logger->write(SgLogger::DBG, SgLogger::RUN, className() +
              "::run(): aux parameters for " + si->getKey() + 
              QString("").sprintf(": K=%.4f  B=%.4f    X=%.4f(mas) Y=%.4f(mas)   "
                "A0=%.4f(mas) B0=%.4f(mas) Th0=%.4f(deg)   A1=%.4f(mas) B1=%.4f(mas) "
                "Th1=%.4f(deg) %.3f(ps)",
                si->getK(), si->getB(), si->getX()*RAD2MAS, si->getY()*RAD2MAS,
                si->getA0()*RAD2MAS, si->getB0()*RAD2MAS, si->getTheta0()*RAD2DEG,
                si->getA1()*RAD2MAS, si->getB1()*RAD2MAS, si->getTheta1()*RAD2DEG,
                si->wrms(DT_DELAY)*1.0e12));
          else
            logger->write(SgLogger::DBG, SgLogger::RUN, className() +
              "::run(): aux parameters for " + si->getKey() + 
              QString("").sprintf(": K=%.4f  B=%.4f    X=%.4f(mas) Y=%.4f(mas)",
                si->getK(), si->getB(), si->getX()*RAD2MAS, si->getY()*RAD2MAS));
        }
        else
          logger->write(SgLogger::ERR, SgLogger::RUN, className() +
            "::run(): aux parameters are NULL for " + si->getKey());
      }
      else if (si->getSmtType() == SgVlbiSourceInfo::SMT_MULTIPLE_POINTS)
*/
      {
        for (int i=0; i<si->sModel().size(); i++)
        {
          if (si->sModel()[i].pK())
          {
            si->sModel()[i].setK(si->sModel()[i].getK() + si->sModel()[i].pK()->getSolution());
            si->sModel()[i].setB(si->sModel()[i].getB() + si->sModel()[i].pB()->getSolution());
            si->sModel()[i].setX(si->sModel()[i].getX() + si->sModel()[i].pX()->getSolution());
            si->sModel()[i].setY(si->sModel()[i].getY() + si->sModel()[i].pY()->getSolution());

            si->sModel()[i].setKsig(si->sModel()[i].pK()->getSigma());
            si->sModel()[i].setBsig(si->sModel()[i].pB()->getSigma());
            si->sModel()[i].setXsig(si->sModel()[i].pX()->getSigma());
            si->sModel()[i].setYsig(si->sModel()[i].pY()->getSigma());
 
            logger->write(SgLogger::DBG, SgLogger::RUN, className() +
              "::run(): aux parameters for " + si->getKey() + 
              QString("").sprintf(": X=%.4f (%.4f)  Y=%.4f (%.4f)  K=%.4f (%.4f)  B=%.4f (%.4f)  %.3f(ps)",
                si->sModel()[i].getX()*RAD2MAS, 
                si->sModel()[i].getXsig()*RAD2MAS, 
                si->sModel()[i].getY()*RAD2MAS,
                si->sModel()[i].getYsig()*RAD2MAS,
                si->sModel()[i].getK(), 
                si->sModel()[i].getKsig(), 
                si->sModel()[i].getB(),
                si->sModel()[i].getBsig(),
                si->wrms(DT_DELAY)*1.0e12));
          };
        };
      };
    };
  };
  //-------------------------- ??????
  //
  //
  // the execution is finished
  finisRunEpoch_ = SgMJD::currentMJD();
  if (isNeedReweighting)
    currentSession_->addAttr(SgVlbiSessionInfo::Attr_FF_WEIGHTS_CORRECTED);
  //
  // set some general stat info:
  currentSession_->setNumOfParameters(numOfParameters_);
  currentSession_->setNumOfConstraints(numOfConstraints_);
  currentSession_->setNumOfDOF(round(band->dof(DT_DELAY)));

  //
  // make a report:
  const char*                   obsTypeNames[]=
                                        {"None", "Single Band Delays", "Group Delays", "Phase Delays"};
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::run(): data analysis procedure finished", true);
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::run(): " + QString("").sprintf("%d observables were reduced to %d parameters", 
      estimator_->numOfProcessedObs(), numOfParameters_), true);
  if (useDelays)
  {
    logger->write(SgLogger::INF, SgLogger::RUN, className() +
      "::run(): " + 
      QString("").sprintf("(%d/%d/%d) observations of %s on the %s-Band were processed",
        band->numTotal(DT_DELAY),
        band->numUsable(DT_DELAY),
        band->numProcessed(DT_DELAY),
        obsTypeNames[cfg->getUseDelayType()],
        qPrintable(band->getKey())), true);
    logger->write(SgLogger::INF, SgLogger::RUN, className() +
      "::run(): " + 
      QString("").sprintf("WRMS= %.2fps with reduced chi2=%.4f, Tmean = ",
        band->wrms(DT_DELAY)*1.0E12,
        band->reducedChi2(DT_DELAY)) +
      qPrintable(currentSession_->getTMean().toString(SgMJD::F_Simple)), true);
    if (cfg->getUseDelayType() == SgTaskConfig::VD_PHS_DELAY)
      currentSession_->addAttr(SgVlbiSessionInfo::Attr_FF_PHASE_DEL_USED);
  };
  if (useRates)
  {
    logger->write(SgLogger::INF, SgLogger::RUN, className() +
      "::run(): " + 
      QString("").sprintf("(%d/%d/%d) observations of Delay Rates on the %s-Band were processed",
        band->numTotal(DT_RATE),
        band->numUsable(DT_RATE),
        band->numProcessed(DT_RATE),
        qPrintable(band->getKey())), true);
    logger->write(SgLogger::INF, SgLogger::RUN, className() +
      "::run(): " + 
      QString("").sprintf("WRMS= %.2ffs/s with reduced chi2=%.4f, Tmean = ",
        band->wrms(DT_RATE )*1.0E15,
        band->reducedChi2(DT_RATE)) +
      qPrintable(currentSession_->getTMean().toString(SgMJD::F_Simple)), true);
  };
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::run(): " + QString("").sprintf("the largest condition number of the working matrix is %.6E",
      estimator_->maxConditionNumber()), true);
  logger->write(SgLogger::INF, SgLogger::RUN, className() +
    "::run(): elapsed time for the task is: " + 
    interval2Str(finisRunEpoch_ - startRunEpoch_) + " (" + 
    QString("").sprintf("%.2f", (finisRunEpoch_ - startRunEpoch_)*86400.0) + " sec)", true);
};



//
void SgSingleSessionTaskManager::finisRun()
{
  // save estimated parameters and made aux reports:
  StationsByName_it it=currentSession_->stationsByName().begin();
  for (; it!=currentSession_->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo *si = it.value();
    if (!si->isAttr(SgVlbiStationInfo::Attr_NOT_VALID))
    {
      if (task_->parameters()->getClock0Mode() == SgParameterCfg::PM_LOC)
        for (int i=0; i<si->getClocksModelOrder(); i++)
        {
          double c = si->getEstClockModel(i) + si->pClocks(i)->getSolution();
          si->setEstClockModel(i, c, si->pClocks(i)->getSigma());
        };
      if (task_->parameters()->getZenithMode() == SgParameterCfg::PM_LOC)
      {
        double c = si->getEstWetZenithDelay() + si->pZenithDelay()->getSolution();
        si->setEstWetZenithDelay(c, si->pZenithDelay()->getSigma());
      };
    };
  };
  //
  //
  if (task_->parameters()->getBlLengthMode() == SgParameterCfg::PM_LOC)
  {
    QString                     str;
    logger->write(SgLogger::INF, SgLogger::RUN | SgLogger::ESTIMATOR, className() +
      "::finisRun(): ==== baseline length aux. report ====");
    //
    //
    BaselinesByName_it          it_bl;
    for (it_bl=currentSession_->baselinesByName().begin();
         it_bl!=currentSession_->baselinesByName().end(); ++it_bl)
    {
      SgVlbiBaselineInfo       *bi=it_bl.value();
      QString                   blName;
      blName = bi->getKey();
      // baseline vector:
      if (bi->pBx()->isAttr(SgParameter::Attr_IS_SOLVED))
      {
        SgVlbiStationInfo    *s1, *s2;
        if (currentSession_->stationsByName().contains(blName.mid(0,8)))
          s1 = currentSession_->stationsByName().value(blName.mid(0,8));
        else
          s1 = NULL;
        if (currentSession_->stationsByName().contains(blName.mid(9,8)))
          s2 = currentSession_->stationsByName().value(blName.mid(9,8));
        else
          s2 = NULL;
        if (s1 && s2)
        {
          Sg3dVector            b, b_apriori;
          b_apriori = (s2->getR() - s1->getR());
          if (task_->config()->getUseExtAPrioriSitesPositions())
          {
            SgMJD                 t(bi->pBx()->getTMean());
            b_apriori = s2->getR_ea() - s1->getR_ea() + 
                       (s2->getV_ea() - s1->getV_ea())*
                                              (t - currentSession_->getApStationVelocities().getT0());
          };
          b = b_apriori + 
              Sg3dVector(bi->pBx()->getSolution(), bi->pBy()->getSolution(), bi->pBz()->getSolution());
          double                  l(b.module()), l_apriori(b_apriori.module());
          str.sprintf("length a priori: %14.2f (mm)  a posteriori: %14.2f (mm)"
                      "  change in length: %10.2f (mm)",
                      l_apriori*1000.0, l*1000.0, (l-l_apriori)*1000.0);
          logger->write(SgLogger::INF, SgLogger::RUN | SgLogger::ESTIMATOR, className() +
            "::finisRun(): Baseline " + blName + ": " + str +
            " on " + bi->pBx()->getTMean().toString(SgMJD::F_Simple));
        }
        else
          logger->write(SgLogger::ERR, SgLogger::DATA, className() +
            "::finisRun(): a station pointer is NULL");
      };
    };
    //
    //
    logger->write(SgLogger::INF, SgLogger::RUN | SgLogger::ESTIMATOR, className() +
      "::finisRun(): ==== end of aux report ====");
  };

  currentSession_->releaseParameters();

  // destroy calcable objects:
  if (refraction_)
  {
    delete refraction_;
    refraction_ = NULL;
  };

  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::finisRun(): the task is finished");
};



//
void SgSingleSessionTaskManager::updateParamaterLists()
{
  // lists of gloabal parameters:
  if (globalParameters_)
  {
    fillParameterList(globalParameters_, SgParameterCfg::PM_GLB);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::updateParamaterLists(): the list of global parameters (" +
      QString("_").setNum(globalParameters_->size()) + ") has been created");
  };
  // lists of arc parameters:
  if (arcParameters_)
  {
    fillParameterList(arcParameters_, SgParameterCfg::PM_ARC);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::updateParamaterLists(): the list of arc parameters (" +
      QString("_").setNum(arcParameters_->size()) + ") has been created");
  };

  // lists of local parameters:
  if (localParameters_)
  {
    fillParameterList(localParameters_, SgParameterCfg::PM_LOC);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::updateParamaterLists(): the list of local parameters (" + 
      QString("_").setNum(localParameters_->size()) + ") has been created");
  };

  // lists of PWL parameters:
  if (pwlParameters_)
  {
    fillParameterList(pwlParameters_, SgParameterCfg::PM_PWL);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::updateParamaterLists(): the list of `picewise linear' parameters (" +
      QString("_").setNum(pwlParameters_->size()) + ") has been created");
  };

  // lists of stochastic parameters:
  if (stochasticParameters_)
  {
    fillParameterList(stochasticParameters_, SgParameterCfg::PM_STC);
    logger->write(SgLogger::DBG, SgLogger::RUN, className() +
      "::updateParamaterLists(): the list of stochastic parameters (" + 
      QString("_").setNum(stochasticParameters_->size()) + ") has been created");
  };

  estimator_->addParametersList(localParameters_);
  estimator_->addParametersList(arcParameters_);
  estimator_->addParametersList(pwlParameters_);
  estimator_->addParametersList(stochasticParameters_); 
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
