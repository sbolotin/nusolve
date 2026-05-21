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

#include <SgEstimator.h>

#include <SgVector.h>
#include <SgSymMatrix.h>
#include <SgUtMatrix.h>

#include <SgLogger.h>

#include <SgParameter.h>
#include <SgArcStorage.h>
#include <SgPwlStorage.h>
#include <SgPwlStorageBSplineL.h>
#include <SgPwlStorageBSplineQ.h>
#include <SgPwlStorageIncRates.h>
#include <SgTaskConfig.h>

//#include <SgTaskManager.h>


//#define LOCDEB

#include <pthread.h>
#include <signal.h>
#include <unistd.h>


const char* nsNumOfThreads = "NUSOLVE_NUM_THREADS";

struct arg4Prc_01
{
  int                           column_;
  int                           thread_;
  int                           num_;
  int                           nZ_;
  double                      **ppRx_;
  double                      **ppAx_;
  double                       *pZx_;
  double                       *pZ_;
  double                       *pU_;
  pthread_mutex_t              *pthMutexes_;
  pthread_mutex_t              *pthMutL_;
  int                          *lastPrcColumn_;
};

struct arg4Prc_02
{
  int                           column_;
  int                            thread_;
  int                           nP_;
  int                           nX_;
  int                           nZ_;
  double                      **ppRp_;
  double                      **ppRpx_;
  double                      **ppAp_;
  double                      **ppAx_;
  double                       *pZp_;
  double                       *pZ_;
  double                       *pU_;
  pthread_mutex_t              *pthMutexes_;
  pthread_mutex_t              *pthMutL_;
  int                          *lastPrcColumn_;
};

void* thPrc_01(void *);
void* thPrc_02(void *);

/**/
//pthread_mutex_t                *pthMutexes;
//pthread_mutex_t                 pthMutL;
//int                             lastPrcColumn;
/**/


void * thPrc_01(void *pArg)
{
  struct arg4Prc_01            *a=(struct arg4Prc_01*)pArg;
  int                           l=a->column_;
//int                           m=a->thread_;
  int                           num=a->num_;
  int                           nZ=a->nZ_;
  double                      **ppRx=a->ppRx_;
  double                      **ppAx=a->ppAx_;
  double                       *pZx=a->pZx_;
  double                       *pZ=a->pZ_;
  double                       *pU=a->pU_;
  //
  pthread_mutex_t              *pthMutexes = a->pthMutexes_;
  pthread_mutex_t              *pthMutL = a->pthMutL_;
  int                          *lastPrcColumn = a->lastPrcColumn_;
  //
  double                        ul=0.0, s=0.0, g=0.0;
  double                        *p, *q;
  int                           i,j;
  //
//printf("  ++ begin: m=%d, l=%d\n", m, l);
  //
  pthread_mutex_lock(pthMutL);
  l = *lastPrcColumn + 1;
  /* check for previous threads:*/
/*
  for (i=l-numOfThreads; i<l; i++)
  {
    if (-1 < i)
    {
      pthread_mutex_lock  (pthMutexes + i);
      pthread_mutex_unlock(pthMutexes + i);
    };
  };
*/
  // processing column l:
  pthread_mutex_lock(pthMutexes + l);
  //
  //
  s = *(*(ppRx+l)+l)**(*(ppRx+l)+l);
  p = *(ppAx+l);
  for (i=0; i<nZ; i++, p++)
    s += *p**p;
  //
  if (s>0.0)
  {
    s = -signum(*(*(ppRx+l)+l))*sqrt(s);
    //
    ul = *(*(ppRx+l)+l) - s;                            // U_l
    memcpy((void*)(pU), (void*)(*(ppAx+l)), sizeof(double)*(nZ));

    *(*(ppRx+l)+l) = s;                                  // Rx_(l,l)
    s = 1.0/(s*ul);                                     // betta
    //                                                  // |
    // lock next column:                                // |
    pthread_mutex_lock  (pthMutexes + l + 1);            // |
    // update and unlock lastPrcColumn:
    (*lastPrcColumn)++;
    pthread_mutex_unlock(pthMutL);

    pthread_mutex_unlock(pthMutexes + l);                // |
    // processing l+1 ... n columns:                    // |
    for (j=l+1; j<num; j++)                             // |<-- processing "Rx/Ax"-block
    {                                                   // |
      g = ul**(*(ppRx+j)+l);                            // |
      p = *(ppAx+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        g += *q**p;                                     // |
                                                        // |
      *(*(ppRx+j)+l) += s*g*ul;                         // |
      p = *(ppAx+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        *p += s*g**q;                                   // |
      pthread_mutex_lock  (pthMutexes + j + 1);          // |
      pthread_mutex_unlock(pthMutexes + j);              // |
    };                                                  // |_________________________
    //                                                  // |
    g = ul**(pZx+l);                                    // |--
    p = pZ;                                              // |
    q = pU;                                              // |
    for (i=0; i<nZ; i++, p++, q++)                      // |
      g += *q**p;                                       // | dealing with the vectors Zx
    //                                                  // |
    *(pZx+l) += s*g*ul;                                 // |            and Z
    p = pZ;                                              // |
    q = pU;                                              // |
    for (i=0; i<nZ; i++, p++, q++)                      // |
      *p += s*g**q;                                     // |--

    pthread_mutex_unlock(pthMutexes + num);
  }
  else // unlock resources:
  {
    (*lastPrcColumn)++;
    pthread_mutex_unlock(pthMutL);
    pthread_mutex_unlock(pthMutexes + l);                // |
  };
//printf("  ++ end: m=%d, l=%d\n", m, l);
  return NULL;
};





void * thPrc_02(void *pArg)
{
  struct arg4Prc_02            *a=(struct arg4Prc_02*)pArg;
  int                           l=a->column_;
//int                           m=a->thread_;
  int                           nP  	= a->nP_;
  int                           nX  	= a->nX_;
  int                           nZ    = a->nZ_;
  double                      **ppRp  = a->ppRp_;
  double                      **ppRpx = a->ppRpx_;
  double                      **ppAp  = a->ppAp_;
  double                      **ppAx  = a->ppAx_;
  double                       *pZp   = a->pZp_;
  double                       *pZ    = a->pZ_;
  double                       *pU    = a->pU_;
  //
  pthread_mutex_t              *pthMutexes    = a->pthMutexes_;
  pthread_mutex_t              *pthMutL       = a->pthMutL_;
  int                          *lastPrcColumn = a->lastPrcColumn_;
  //
  double                        ul=0.0, s=0.0, g=0.0;
  double                        *p, *q;
  int                           i,j;
  //
  //
//printf("      ++ begin: m=%d, l=%d\n", m, l);
  pthread_mutex_lock(pthMutL);
  l = *lastPrcColumn + 1;
//printf("      ++        m=%d  l=%d (Get Access to lastPrcColumn)\n", m, l);

  // processing column l:
  pthread_mutex_lock(pthMutexes + l);
//printf("      ++      : m=%d, l=%d   locked(0): %d\n", m, l, l);
  //
  //
  s = *(*(ppRp+l)+l)**(*(ppRp+l)+l);
  p = *(ppAp+l);
  for (i=0; i<nZ; i++, p++)
    s += *p**p;
  //
  if (s>0.0)
  {
    s = -signum(*(*(ppRp+l)+l))*sqrt(s);
    //
    ul = *(*(ppRp+l)+l) - s;                            // U_l
    memcpy((void*)(pU), (void*)(*(ppAp+l)), sizeof(double)*(nZ));

    *(*(ppRp+l)+l) = s;                                  // Rx_(l,l)
    s = 1.0/(s*ul);                                     // betta
    //                                                  // |
    // lock next column:                                // |
    pthread_mutex_lock  (pthMutexes + l + 1);            // |
//printf("      ++      : m=%d, l=%d   locked(1): %d\n", m, l, l + 1);
    // update and unlock lastPrcColumn:
    (*lastPrcColumn)++;
    pthread_mutex_unlock(pthMutL);
//printf("      ++        m=%d  l=%d (Released Access to lastPrcColumn)\n", m, l);

    pthread_mutex_unlock(pthMutexes + l);                // |
//printf("      ++      : m=%d, l=%d unlocked(1): %d\n", m, l, l);
    // processing l+1 ... nP columns:                    // |
    for (j=l+1; j<nP; j++)                              // |<-- processing "Rp/Ap"-block
    {                                                   // |
      g = ul**(*(ppRp+j)+l);                            // |
      p = *(ppAp+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        g += *q**p;                                     // |
                                                        // |
      *(*(ppRp+j)+l) += s*g*ul;                         // |
      p = *(ppAp+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        *p += s*g**q;                                   // |
      pthread_mutex_lock  (pthMutexes + j + 1);          // |
//printf("      ++      : m=%d, l=%d   locked(2): %d\n", m, l, j + 1);
      pthread_mutex_unlock(pthMutexes + j);              // |
//printf("      ++      : m=%d, l=%d unlocked(2): %d\n", m, l, j);
    };                                                  // |_________________________
    // processing nP+1 ... nX columns:                  // |
    for (j=0; j<nX; j++)                                // |<-- processing "Rpx/Ax"-block
    {                                                   // |
      g = ul**(*(ppRpx+j)+l);                           // |
      p = *(ppAx+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        g += *q**p;                                     // |
                                                        // |
      *(*(ppRpx+j)+l) += s*g*ul;                        // |
      p = *(ppAx+j);                                    // |
      q = pU;                                            // |
      for (i=0; i<nZ; i++, p++, q++)                    // |
        *p += s*g**q;                                   // |
      pthread_mutex_lock  (pthMutexes + nP + j + 1);     // |
//printf("      ++      : m=%d, l=%d   locked(3): %d\n", m, l, nP + j + 1);
      pthread_mutex_unlock(pthMutexes + nP + j);        // |
//printf("      ++      : m=%d, l=%d unlocked(3): %d\n", m, l, nP + j);
    };                                                  // |_________________________
    //                                                  // |
    g = ul**(pZp+l);                                    // |--
    p = pZ;                                              // |
    q = pU;                                              // |
    for (i=0; i<nZ; i++, p++, q++)                      // |
      g += *q**p;                                       // | dealing with the vectors Zp
    //                                                  // |
    *(pZp+l) += s*g*ul;                                 // |            and Z
    p = pZ;                                              // |
    q = pU;                                              // |
    for (i=0; i<nZ; i++, p++, q++)                      // |
      *p += s*g**q;                                     // |--

    pthread_mutex_unlock(pthMutexes + nP + nX);
//printf("      ++      : m=%d, l=%d unlocked(4): %d\n", m, l, nP + nX);
  }
  else // unlock resources:
  {
    (*lastPrcColumn)++;
    pthread_mutex_unlock(pthMutL);
//printf("      ++        m=%d  l=%d (Released Access to lastPrcColumn, s==0)\n", m, l);

    pthread_mutex_unlock(pthMutexes + l);                // |
//printf("      ++      : m=%d, l=%d unlocked(0): %d, s==0\n", m, l, l);
  };

//printf("      -- end:   m=%d, l=%d\n", m, l);
  return NULL;
};



 





/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgEstimator::className()
{
  return "SgEstimator";
};



//
SgEstimator::SgEstimator(SgTaskConfig *config) :
  tLastProcessed_(tZero),
  tNextLeft_(tZero),
  tNextRight_(tInf)
{
  config_ = config;
  // regular parameters:
  listXAll_ = new QList<SgParameter*>;
  xAllByName_ = new QMap<QString, SgParameter*>;
  //
  listX_ = new QList<SgParameter*>;
  xByName_ = new QMap<QString, SgParameter*>;
  nX_ = 0;
  x_  = NULL;
  zx_ = NULL;
  Rx_ = NULL;
  Px_ = NULL;

  // stochastic parameters:
  listP_ = new QList<SgParameter*>;
  pByName_ = new QMap<QString, SgParameter*>;
  nP_ = 0;
  p_  = NULL;
  zp_ = NULL;
  Rp_ = NULL;
  Rpx_ = NULL;
  Pp_  = NULL;
  SR_  = NULL;
  //
  // stochastic smoothing:
  Rp_s_  = NULL;
  Rpp_s_ = NULL;
  Rpx_s_ = NULL;
  zp_s_  = NULL;

  // arc parameters' specials:
  listA_ = new QList<SgParameter*>;
  aByName_ = new QMap<QString, SgParameter*>;
  arcStorage_ = new QList<SgArcStorage*>;

  // PWL parameters' specials:
  listPwl_ = new QList<SgParameter*>;
  pwlParByName_ = new QMap<QString, SgParameter*>;
  pwlStorageByName_ = new QMap<QString, SgPwlStorage*>;
  pwlStorage_ = new QList<SgPwlStorage*>;

  // data:
  nZ_ = 0;
  z_  = NULL;
  Ap_ = NULL;
  Ax_ = NULL;
  u_  = NULL;
  //
  u4pth_ = NULL;
  //
  numOfProcessedObs_ = 0;
  maxConditionNumber_ = 0.0;
  
  numOfThreads_ = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_attr_init(&pthreadAttr_);
  pthread_attr_setscope(&pthreadAttr_, PTHREAD_SCOPE_SYSTEM);
  //
//  pthread_attr_setschedpolicy(&pthreadAttr_, SCHED_FIFO);
//  pthread_attr_setschedpolicy(&pthreadAttr_, SCHED_RR);
//  pthread_attr_setschedpolicy(&pthreadAttr_, SCHED_OTHER);

  QString                       str(getenv(nsNumOfThreads));
  if (str.size())
  {
    int                         n;
    bool                        is;
    n = str.toInt(&is);
    if (is)
    {
      if (0 <= n)
      {
        numOfThreads_ = n;
        logger->write(SgLogger::INF, SgLogger::ESTIMATOR, className() + 
          "::SgEstimator(): the number of threads is set to " + QString("").setNum(numOfThreads_) + 
          " form the environmental variable \"" + nsNumOfThreads + "\"");
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
          "::SgEstimator(): the the environmental variable \"" + nsNumOfThreads + 
          "\" is set incorrectly: cannot convert \"" + str + "\" to number of threads");
        logger->write(SgLogger::INF, SgLogger::ESTIMATOR, className() + 
          "::SgEstimator(): the number of threads is " + QString("").setNum(numOfThreads_));
      };
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
        "::SgEstimator(): the the environmental variable \"" + nsNumOfThreads + 
        "\" is set incorrectly: cannot convert \"" + str + "\" to int");
      logger->write(SgLogger::INF, SgLogger::ESTIMATOR, className() + 
        "::SgEstimator(): the number of threads is " + QString("").setNum(numOfThreads_));
    };
  }
  else
    logger->write(SgLogger::INF, SgLogger::ESTIMATOR, className() + 
      "::SgEstimator(): using the default number of threads " + QString("").setNum(numOfThreads_));
};



// A destructor:
SgEstimator::~SgEstimator()
{
  // threads:
  // free attrs:
  pthread_attr_destroy(&pthreadAttr_);
  logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() + 
    "::~SgEstimator():  " + QString("").setNum(numOfThreads_) + " thread attributes were released");


  // regular:
  if (xAllByName_)
  {
    xAllByName_->clear();
    delete xAllByName_;
    xAllByName_ = NULL;
  };
  if (listXAll_)
  {
    listXAll_->clear();
    delete listXAll_;
    listXAll_ = NULL;
  };
  if (xByName_)
  {
    xByName_->clear();
    delete xByName_;
    xByName_ = NULL;
  };
  if (listX_)
  {
    listX_->clear();
    delete listX_;
    listX_ = NULL;
  };
  if (x_)
  {
    delete x_;
    x_ = NULL;
  };
  if (zx_)
  {
    delete zx_;
    zx_ = NULL;
  };
  if (Rx_)
  {
    delete Rx_;
    Rx_ = NULL;
  };
  if (PxAll_)
  {
    delete PxAll_;
    PxAll_ = NULL;
  };

  // stochastics:
  if (pByName_)
  {
    pByName_->clear();
    delete pByName_;
    pByName_ = NULL;
  };
  if (listP_)
  {
/*
    for (int i=0; i<listP_->size(); i++)
      delete listP_->at(i);
*/
    listP_->clear();
    delete listP_;
    listP_ = NULL;
  };
  if (p_)
  {
    delete p_;
    p_ = NULL;
  };
  if (zp_)
  {
    delete zp_;
    zp_ = NULL;
  };
  if (Rp_)
  {
    delete Rp_;
    Rp_ = NULL;
  };
  if (Rpx_)
  {
    delete Rpx_;
    Rpx_ = NULL;
  };
  if (SR_)
  {
    delete SR_;
    SR_ = NULL;
  };
  if (Rp_s_)
  {
    delete Rp_s_;
    Rp_s_ = NULL;
  };
  if (Rpp_s_)
  {
    delete Rpp_s_;
    Rpp_s_ = NULL;
  };
  if (Rpx_s_)
  {
    delete Rpx_s_;
    Rpx_s_ = NULL;
  };
  if (zp_s_)
  {
    delete zp_s_;
    zp_s_ = NULL;
  };
  //
  for (QMap<QString, StochasticSolutionCarrier>::iterator it=stcSolutions_.begin(); 
    it!=stcSolutions_.end(); ++it)
  {
    StochasticSolutionCarrier    &stcSol=it.value();
    for (int i=0; i<stcSol.list_->size(); i++)
      delete stcSol.list_->at(i);
    delete stcSol.list_;
    delete stcSol.x_;
    delete stcSol.P_;
    stcSol.list_ = NULL;
    stcSol.x_ = NULL;
    stcSol.P_ = NULL;
  };
  stcSolutions_.clear();
  //

  // arc parameters:
  if (listA_)
  {
    listA_->clear();
    delete listA_;
    listA_ = NULL;
  };
  if (aByName_)
  {
    aByName_->clear();
    delete aByName_;
    aByName_ = NULL;
  };
  if (arcStorage_)
  {
    for (int i=0; i<arcStorage_->size(); i++)
      delete arcStorage_->at(i);  // here we are an owner of these parameters
    delete arcStorage_;
    arcStorage_ = NULL;
  };

  // PWL parameters:
  if (listPwl_)
  {
    listPwl_->clear();
    delete listPwl_;
    listPwl_ = NULL;
  };
  if (pwlParByName_)
  {
    pwlParByName_->clear();
    delete pwlParByName_;
    pwlParByName_ = NULL;
  };
  if (pwlStorageByName_)
  {
    pwlStorageByName_->clear();
    delete pwlStorageByName_;
    pwlStorageByName_ = NULL;
  };
  if (pwlStorage_)
  {
    for (int i=0; i<pwlStorage_->size(); i++)
      delete pwlStorage_->at(i);  // here we are an owner of these parameters
    delete pwlStorage_;
    pwlStorage_ = NULL;
  };

  // data:
  if (z_)
  {
    delete z_;
    z_ = NULL;
  };
  if (Ap_)
  {
    delete Ap_;
    Ap_ = NULL;
  };
  if (Ax_)
  {
    delete Ax_;
    Ax_ = NULL;
  };
  if (u_)
  {
    delete u_;
    u_ = NULL;
  };
  if (u4pth_)
  {
    delete u4pth_;
    u4pth_ = NULL;
  };
  
};



//
void SgEstimator::addParameter(SgParameter* p, bool isSpecial)
{
  if (!p)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): the parameter is NULL");
    return;
  };
  if (xAllByName_->contains(p->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): the parameter [" + p->getName() +
      "] is already in the regular parameters map");
    return;
  };
  if (pByName_->contains(p->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): the parameter [" + p->getName() +
      "] is already in the stochastic parameters map");
    return;
  };
  if (aByName_->contains(p->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): the parameter [" + p->getName() +
      "] is already in the arc parameters map");
    return;
  };
  if (pwlParByName_->contains(p->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): the parameter [" + p->getName() +
      "] is already in the PWL parameters map");
    return;
  };
  if (nZ_) // we will fix it latter
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParameter(): nZ_!=0; the parameter [" + p->getName() +
      "]");
    return;
  };
  QList<SgParameter*>           *l=listXAll_;
  QMap<QString, SgParameter*>   *m=xAllByName_;
  if (p->getPMode() == SgParameterCfg::PM_STC)
  {
    l = listP_;
    m = pByName_;
  }
  else if (p->getPMode() == SgParameterCfg::PM_ARC)
  {
    l = listA_;
    m = aByName_;
  }
  else if (p->getPMode() == SgParameterCfg::PM_PWL)
  {
    l = listPwl_;
    m = pwlParByName_;
  }
  if (isSpecial)
    p->addAttr(SgPartial::Attr_IS_SPECIAL);
  l->append(p);
  m->insert(p->getName(), p);
};



//
void SgEstimator::addParametersList(QList<SgParameter*>* l, bool isSpecial)
{
  if (!l)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::addParametersList(): the parameters list is NULL");
    return;
  };
  for (int i=0; i<l->size(); i++)
  {
    SgParameter* p=l->at(i);
    addParameter(p, isSpecial);
  };
};



//
void SgEstimator::removeParameter(SgParameter *p)
{
  if (!p)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter is NULL");
    return;
  };
  if (!xAllByName_->contains(p->getName())  && p->getPMode() == SgParameterCfg::PM_LOC)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter [" + p->getName() +
      "] is not in the regular parameters map");
    return;
  };
  if (!pByName_->contains(p->getName()) && p->getPMode() == SgParameterCfg::PM_STC)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter [" + p->getName() +
      "] is not in the stochastic parameters map");
    return;
  };
  if (!aByName_->contains(p->getName()) && p->getPMode() == SgParameterCfg::PM_ARC)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter [" + p->getName() +
      "] is not in the arc parameters map");
    return;
  };
  if (pwlParByName_->contains(p->getName()) && p->getPMode() == SgParameterCfg::PM_PWL)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter [" + p->getName() +
      "] is not in the PWL parameters map");
    return;
  };
  if (nZ_) // we will fix it latter too
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): nZ_!=0; the parameter [" + p->getName() +
      "]");
    return;
  };
  QList<SgParameter*>           *l=listXAll_;
  QMap<QString, SgParameter*>   *m=xAllByName_;
  if (p->getPMode() == SgParameterCfg::PM_STC)
  {
    l = listP_;
    m = pByName_;
  }
  else if (p->getPMode() == SgParameterCfg::PM_ARC)
  {
    l = listA_;
    m = aByName_;
  }
  else if (p->getPMode() == SgParameterCfg::PM_PWL)
  {
    l = listPwl_;
    m = pwlParByName_;
  };
  int       idx=0;
  while (p->getName() != l->at(idx)->getName())
    idx++;
  if (idx<l->size())
  {
    l->removeAt(idx);
    m->remove(p->getName());
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParameter(): the parameter [" + p->getName() +
      "] is not in the parameters list");
    return;
  };
};



//
void SgEstimator::removeParametersList(QList<SgParameter*> *l)
{
  if (!l)
  {
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::removeParametersList(): the parameters list is NULL");
    return;
  };
  for (int i=0; i<l->size(); i++)
  {
    SgParameter* p=l->at(i);
    removeParameter(p);
  };
};



//
void SgEstimator::prepare2Run(int numOfExpectedObs, 
  const SgMJD& tFirst, const SgMJD& tLast, const SgMJD& tRefer)
{
  //
  // check consistency:
  if (listXAll_->size() != xAllByName_->size())
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::prepare2Run(): size of the regular parameters list and its dictionary mismatch");
  if (listP_->size() != pByName_->size())
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::prepare2Run(): size of the stochastic parameters list and its dictionary mismatch");
  if (listA_->size() != aByName_->size())
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::prepare2Run(): size of the arc parameters list and its dictionary mismatch");
  if (listPwl_->size() != pwlParByName_->size())
    logger->write(SgLogger::ERR, SgLogger::ESTIMATOR, className() + 
      "::prepare2Run(): size of the PWL parameters list and its dictionary mismatch");
  //
  deployArcParameters(tFirst, tLast, tRefer);
  //
  deployPwlParameters(tFirst, tLast, tRefer);
  logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() + 
      "::prepare2Run(): the size of the XAll parameters list is " + QString().setNum(listXAll_->size()));
  //
  // at this point we assume that parmaters lists are defined and fixed.
  qSort(listXAll_->begin(), listXAll_->end(), lessThan4_ParameterByTLeftSortingOrder);
  for (int i=0; i<listXAll_->size(); i++)
  {
    SgParameter *p=listXAll_->at(i);
    p->setIdx(i);
    p->delAttr(SgParameter::Attr_IS_SOLVED);
    if (p->getTLeft() <= tZero)
      listX_->append(p);
    if (p->getTRight()<tNextRight_)
      tNextRight_ = p->getTRight();
    if (p->getTLeft()!=tZero && (tNextLeft_==tZero || p->getTLeft()<tNextLeft_))
      tNextLeft_ = p->getTLeft();
  };
  
  // sort the lists:
  qSort(listX_->begin(), listX_->end(), lessThan4_ParameterByTRightSortingOrder);
  qSort(listP_->begin(), listP_->end(), lessThan4_ParameterByNameSortingOrder);
  qSort(listA_->begin(), listA_->end(), lessThan4_ParameterByNameSortingOrder);
  qSort(listPwl_->begin(), listPwl_->end(), lessThan4_ParameterByNameSortingOrder);
  //
  //
  // regular parameters:
  nX_ = listX_->size();
  Rx_ = new SgUtMatrix(nX_);
  zx_ = new SgVector(nX_);
  for (unsigned int i=0; i<nX_; i++)
  {
    Rx_->setElement(i,i, 1.0/listX_->at(i)->getSigmaAPriori());
/*
    std::cout << qPrintable(listX_->at(i)->getName()) << " (" 
              << qPrintable(SgParameterCfg::mode2String(listX_->at(i)->getPMode())) << "): "
              << listX_->at(i)->getSigmaAPriori()*listX_->at(i)->getScale() << "\n";
*/
  };
  rpStack_.clear();
  PxAll_ = new SgSymMatrix(listXAll_->size());
  //
  //
  // stochastic parameters:
  nP_ = listP_->size();
  Rp_ = new SgUtMatrix(nP_);
  Rpx_= new SgMatrix(nP_, nX_);
  zp_ = new SgVector(nP_);
  for (unsigned int i=0; i<nP_; i++)
    Rp_->setElement(i,i, 1.0/listP_->at(i)->getSigmaAPriori());
  tLastProcessed_ = tZero;
  //
  // 4smoothing:
  Rp_s_ = new SgUtMatrix(nP_);
  Rpp_s_= new SgMatrix(nP_, nP_);
  Rpx_s_= new SgMatrix(nP_, nX_);
  SR_   = new SgMatrix(2*nP_, 2*nP_ + nX_ + 1);
  zp_s_ = new SgVector(nP_);
  scStack_.clear();
  for (QMap<QString, StochasticSolutionCarrier>::iterator it=stcSolutions_.begin(); 
    it!=stcSolutions_.end(); ++it)
  {
    StochasticSolutionCarrier    &stcSol=it.value();
    for (int i=0; i<stcSol.list_->size(); i++)
      delete stcSol.list_->at(i);
    delete stcSol.list_;
    delete stcSol.x_;
    delete stcSol.P_;
    stcSol.x_ = NULL;
    stcSol.P_ = NULL;
  };
  stcSolutions_.clear();
//  regSolutions_.clear();
  //
  //
  // data:
  nZ_   = numOfExpectedObs + 16; // ok, "16" here are reserved for constrains
  z_    = new SgVector(nZ_);
  Ap_   = new SgMatrix(nZ_, nP_);
  Ax_   = new SgMatrix(nZ_, nX_);
  u_    = new SgVector(nZ_ + 1);
  //
  // threads: Proc01:
  u4pth_= new SgMatrix(nZ_ + 1, numOfThreads_);
  //
  /* init threads: */
  // proc #1:
  th4Prc_01_   = (pthread_t*) calloc(numOfThreads_, sizeof(pthread_t));
  args4Prc_01_ = (struct arg4Prc_01*) calloc(numOfThreads_, sizeof(struct arg4Prc_01));
  for (int thIdx=0; thIdx<numOfThreads_; thIdx++)
  {
    (args4Prc_01_+thIdx)->thread_        = thIdx;
    (args4Prc_01_+thIdx)->pZ_            = z_->base();
    (args4Prc_01_+thIdx)->pU_            = *(u4pth_->base() + thIdx);
    (args4Prc_01_+thIdx)->lastPrcColumn_ = &lastPrcColumn4Prc_01_;  
  };
  // proc #2:
  th4Prc_02_   = (pthread_t*) calloc(numOfThreads_, sizeof(pthread_t));
  args4Prc_02_ = (struct arg4Prc_02*) calloc(numOfThreads_, sizeof(struct arg4Prc_02));
  for (int thIdx=0; thIdx<numOfThreads_; thIdx++)
  {
    (args4Prc_02_+thIdx)->thread_        = thIdx;
    (args4Prc_02_+thIdx)->pZ_            = z_->base();
    (args4Prc_02_+thIdx)->pU_            = *(u4pth_->base() + thIdx);
    (args4Prc_02_+thIdx)->lastPrcColumn_ = &lastPrcColumn4Prc_02_;  
  };
  //
  // tmp:
  time4DataUpdate_ = 0.0;
  time4PropagateStc_ = 0.0;
  time4SaveStc_ = 0.0;
  time4ProcObs_ = 0.0;
  

  //
  //
  nZ_ = 0; // from here nZ_ is a count of processed observations
  numOfProcessedObs_ = 0;
  maxConditionNumber_ = 0.0;
};



//
void SgEstimator::processObs(const SgMJD& t, const SgVector& o_c, const SgVector& sigma)
{
  QString                       str;
  unsigned int                  i;
  SgPartial                    *d=NULL;

tWall4ProcObs0_ = SgMJD::currentMJD();

  // is it a new epoch?
  if (nP_ && tZero<tLastProcessed_ && tLastProcessed_<t)
  {
tWall0_ = SgMJD::currentMJD();
    if (0 < numOfThreads_)
      dataUpdate_th();
    else
      dataUpdate();
tWall_ = SgMJD::currentMJD();
time4DataUpdate_ += (tWall_ - tWall0_)*86400.0;

tWall0_ = tWall_;
    if (0 < numOfThreads_)
      propagateStochasticParameters_th(t - tLastProcessed_);
    else
      propagateStochasticParameters(t - tLastProcessed_);
tWall_ = SgMJD::currentMJD();
time4PropagateStc_ += (tWall_ - tWall0_)*86400.0;

tWall0_ = tWall_;
    save4Smoothing(tLastProcessed_);
tWall_ = SgMJD::currentMJD();
time4SaveStc_ += (tWall_ - tWall0_)*86400.0;
tWall0_ = tWall_;

    for (i=0; i<nP_; i++)
      listP_->at(i)->resetStatistics();

  };
  // check for finished parameters:
  if (tNextRight_ <= t)
  {
/*----
    for (i=0; i<nX_; i++)
    {
      SgParameter              *p=listX_->at(i);
      if (p->isAttr(SgPartial::Attr_IS_SPECIAL) && p->getNumObs())
      {
        Ax_->setElement(nZ_,i,  1.0/p->getSigmaAPrioriAux());
        z_->setElement (nZ_,    0.0);
        nZ_++;
//    std::cout << "   Constrained: " << qPrintable(p->getName()) << " (" 
//              << qPrintable(SgParameterCfg::mode2String(p->getPMode())) << "): "
//              << p->getSigmaAPriori()*p->getScale() << ", aux= "
//              << p->getSigmaAPrioriAux()*p->getScale() << ", num= "
//              << p->getNumObs() 
//              << "\n";
      };
    };
----*/
    if (nZ_)
    {
tWall0_ = SgMJD::currentMJD();
      if (0 < numOfThreads_)
        dataUpdate_th();
      else
        dataUpdate();
tWall_ = SgMJD::currentMJD();
time4DataUpdate_ += (tWall_ - tWall0_)*86400.0;
    };
    saveProcessedParameters(t);
  };
  // check for new parameters:
  if (tZero<tNextLeft_ && tNextLeft_<=t)
  {
    if (nZ_)
    {

tWall0_ = SgMJD::currentMJD();
      if (0 < numOfThreads_)
        dataUpdate_th();
      else
        dataUpdate();
tWall_ = SgMJD::currentMJD();
time4DataUpdate_ += (tWall_ - tWall0_)*86400.0;

    };
    insertNewParameters(t);
  };
  //
  // ---- fill the nZ_'th row of the information array:
  //
  // first, update partials of arc and PWL parameters:
  propagateArcPartials(t);
  propagatePwlPartials(t);
  //
  // fill local parameters:
  for (i=0; i<nX_; i++)
  {
    d = listX_->at(i);
    Ax_->setElement(nZ_, i,  d->getD()/sigma.getElement(0));
    d->updateStatistics(t, 1.0/sigma.getElement(0)/sigma.getElement(0));
    d->zerofy();
  };
  //
  //
  for (int ii=0; ii<listA_->size(); ii++)
    listA_->at(ii)->zerofy();
  for (int ii=0; ii<listPwl_->size(); ii++)
    listPwl_->at(ii)->zerofy();
  //
  //
  // fill stochastic parameters:
  for (i=0; i<nP_; i++)
  {
    d = listP_->at(i);
    Ap_->setElement(nZ_, i,  d->getD()/sigma.getElement(0));
    d->updateStatistics(t, 1.0/sigma.getElement(0)/sigma.getElement(0));
    d->zerofy();
  };
  //
  // vector Z:
  z_->setElement(nZ_,  o_c.getElement(0)/sigma.getElement(0));
  nZ_++;
  numOfProcessedObs_++;
  // ---- end of filling
  tLastProcessed_ = t;


tWall4ProcObs_ = SgMJD::currentMJD();
time4ProcObs_ += (tWall4ProcObs_ - tWall4ProcObs0_)*86400.0;
};



//
void SgEstimator::processConstraint(double o_c, double sigma)
{
  unsigned int      i;
  SgPartial        *d=NULL;
  //
  // local parameters:
  propagatePwlPartials(tZero);
  for (i=0; i<nX_; i++)
  {
    d = listX_->at(i);
    Ax_->setElement(nZ_,i,  d->getD()/sigma);
    d->zerofy();
  };
  for (int ii=0; ii<listA_->size(); ii++)
    listA_->at(ii)->zerofy();
  for (int ii=0; ii<listPwl_->size(); ii++)
    listPwl_->at(ii)->zerofy();
  //
  // stochastic parameters:
  for (i=0; i<nP_; i++)
  {
    d = listP_->at(i);
    Ap_->setElement(nZ_,i,  d->getD()/sigma);
    d->zerofy();
  };
  // vector Z:
  z_->setElement(nZ_, o_c/sigma);
  nZ_++;

tWall0_ = SgMJD::currentMJD();
  if (0 < numOfThreads_)
    dataUpdate_th();
  else
    dataUpdate();
tWall_ = SgMJD::currentMJD();
time4DataUpdate_ += (tWall_ - tWall0_)*86400.0;
};



//
void SgEstimator::saveProcessedParameters(const SgMJD& t)
{
#ifdef LOCDEB
  std::cout << "\n\n++++++    Saving processed parameters at epoch: "
            << qPrintable(t.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << ", Obs Idx " << numOfProcessedObs_ << "\n     tNextLeft: "
            << qPrintable(tNextLeft_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << "; tNextRight: " << qPrintable(tNextRight_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << "\n";
#endif
  RPCarrier                     carrier;
  SgMJD                         tL(tInf), tR(tZero);
  int                           mRow, mCol;
  SgParameter                  *p;
  SgVector                     *z, *zx_New;
  SgUtMatrix                   *R, *Rx_New;
  SgMatrix                     *Rxx;
  // fill the list of parameters that will be stored:
  QList<SgParameter*>          *l=new QList<SgParameter*>;
  QList<SgParameter*>          *r=new QList<SgParameter*>;
  QList<SgParameter*>          *aux=new QList<SgParameter*>;
  tNextRight_ = tInf;
  //
  for (int i=0; i<listX_->size(); i++)
  {
    p=listX_->at(i);
    if (p->getTRight() <= t)
    {
      l->append(new SgParameter(*p));
      if (p->getTLeft() < tL)
        tL = p->getTLeft();
      if (tR < p->getTRight())
        tR = p->getTRight();
    }
    else
    {
      aux->append(new SgParameter(*p));
      r->append(p);
      if (p->getTRight() < tNextRight_)
        tNextRight_ = p->getTRight();
    };
  };
  //
  // copy the vector and the matrices:
  mRow = l->size();
  mCol = listX_->size() - mRow;
#ifdef LOCDEB
  std::cout << "new tNextRight: " << qPrintable(tNextRight_.toString())  << "\n";
  std::cout << "saving processed parameters : splitted: " << mRow << " by " << mCol  << "\n";
#endif
  z = new SgVector(mRow);
  R = new SgUtMatrix(mRow);
  Rxx = new SgMatrix(mRow, mCol);
  // that what is rest:
  zx_New = new SgVector(mCol);
  Rx_New = new SgUtMatrix(mCol);
  // copy the data to save:
  for (int i=0; i<mRow; i++)
  {
    z->setElement(i, zx_->getElement(i));
    for (int j=i; j<mRow; j++)
      R->setElement(i,j, Rx_->getElement(i,j));
    for (int j=0; j<mCol; j++)
      Rxx->setElement(i,j, Rx_->getElement(i, mRow+j));
  };
  // copy the data to continue with:
  for (int i=0; i<mCol; i++)
  {
    zx_New->setElement(i, zx_->getElement(mRow+i));
    for (int j=i; j<mCol; j++)
      Rx_New->setElement(i,j, Rx_->getElement(mRow+i, mRow+j));
  };
  // fill carrier's fields:
  carrier.tLeft_ = tL;
  carrier.tRight_ = tR;
  carrier.list_ = l;
  carrier.listOthers_ = aux;
  carrier.zx_ = z;
  carrier.Rx_ = R;
  carrier.Rxx_ = Rxx;
#ifdef LOCDEB
  QString str;
  std::cout << "\n";
  for (int i=0; i<listX_->size(); i++)
  {
    SgParameter *p=listX_->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "         X(before saving):    " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "matrix before:\n" << *Rx_ << "\n";
  for (int i=0; i<l->size(); i++)
  {
    SgParameter *p=l->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "         X(l):    " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "   __saved: Rx_:\n" << *carrier.Rx_ << "\n";
  std::cout << "   __saved: Rxx_:\n" << *carrier.Rxx_ << "\n";
  std::cout << "   __saved: zx_:\n" << *carrier.zx_ << "\n";
#endif
  // and push it into the stack:
  rpStack_.push(carrier);
#ifdef LOCDEB
  std::cout << " pushed epoch: " << qPrintable(carrier.tRight_.toString()) << "\n";
#endif
  // reset the main list and its info.array:
  listX_->clear();
  delete listX_;
  delete zx_;
  delete Rx_;
  zx_ = zx_New;
  Rx_ = Rx_New;
  listX_ = r;
  nX_ = listX_->size();
  delete Ax_;
  Ax_ = new SgMatrix(z_->n(), nX_);
#ifdef LOCDEB
  std::cout << "\n";
  for (int i=0; i<listX_->size(); i++)
  {
    SgParameter *p=listX_->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "         X(after saving):    " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "matrix after:\n" << *Rx_ << "\n";
#endif
};



//
void SgEstimator::insertNewParameters(const SgMJD& t)
{
#ifdef LOCDEB
  QString str;
  std::cout << "\n\n++++++ Inserting new parameters at epoch: "
            << qPrintable(t.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << ", Obs Idx " << numOfProcessedObs_ << "\n     tNextLeft: "
            << qPrintable(tNextLeft_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << "; tNextRight: " << qPrintable(tNextRight_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << "\n";
#endif
  SgUtMatrix                   *Rx_New;
  SgVector                     *zx_New;
  SgParameter                  *p;
  int                           idx, n;
  QList<SgParameter*>          *l=new QList<SgParameter*>;
  //spec. case:
  if (listX_->size()==0)
    tNextRight_ = tInf;
  // form new X-list:
  for (idx=0; idx<listXAll_->size(); idx++)
  {
    p = listXAll_->at(idx);
    if (p->getTLeft()<=t && t<p->getTRight())
    {
      l->append(p);
      if (p->getTRight() < tNextRight_)
        tNextRight_ = p->getTRight();
      if (idx+1 < listXAll_->size())
        tNextLeft_ = listXAll_->at(idx+1)->getTLeft();
      else if (idx+1 == listXAll_->size())
        tNextLeft_ = tZero;
    };
  };
  // sort it in tRight-order:
  qSort(l->begin(), l->end(), lessThan4_ParameterByTRightSortingOrder);
  n = l->size();
#ifdef LOCDEB
  std::cout << "\n";
  for (int i=0; i<listX_->size(); i++)
  {
    SgParameter *p=listX_->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "    list X_:  " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "\n";
  for (int i=0; i<n; i++)
  {
    SgParameter *p=l->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "    list l:  " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "    l size: " << n
            << ", new nextTLeft: "  << qPrintable(tNextLeft_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << ", new nextTRight: "  << qPrintable(tNextRight_.toString(SgMJD::F_YYYYMMDDHHMMSSSS))
            << "\n";
#endif
  Rx_New = new SgUtMatrix(n);
  zx_New = new SgVector(n);
  // create reference indexes:
  int                        *idxs=new int[nX_];
  QMap<QString, int>          idxByName;
  for (int i=0; i<n; i++)
  {
    idxByName.insert(l->at(i)->getName(), i);
    Rx_New->setElement(i,i, 1.0/l->at(i)->getSigmaAPriori());
  };
  for (unsigned int i=0; i<nX_; i++)
  {
    if (idxByName.contains(listX_->at(i)->getName()))
    {
      idx = idxByName.find(listX_->at(i)->getName()).value();
      *(idxs+i) = idx;
    }
    else std::cout << "cannot find " << qPrintable(listX_->at(i)->getName()) << "\n";
  };
  // fill the matrices and vectors:
  for (unsigned int i=0; i<nX_; i++)
  {
    zx_New->setElement(*(idxs+i), zx_->getElement(i));
    for (unsigned int j=i; j<nX_; j++)
      Rx_New->setElement(*(idxs+i),*(idxs+j),  Rx_->getElement(i,j));
  };
#ifdef LOCDEB
  std::cout << "\n";
  for (int i=0; i<listX_->size(); i++)
  {
    SgParameter *p=listX_->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "         X(before inserting):    " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "matrix before:\n" << *Rx_ << "\n";
#endif
  // reset the main list and its info.array:
  listX_->clear();
  delete[] idxs;
  delete listX_;
  delete zx_;
  delete Rx_;
  zx_ = zx_New;
  Rx_ = Rx_New;
  listX_ = l;
  nX_ = listX_->size();
  delete Ax_;
  Ax_ = new SgMatrix(z_->n(), nX_);
#ifdef LOCDEB
  std::cout << "\n";
  for (int i=0; i<listX_->size(); i++)
  {
    SgParameter *p=listX_->at(i);
    str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
    std::cout << "         X(after inserting):    " << qPrintable(str)
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "matrix after:\n" << *Rx_ << "\n";
#endif
};



//
void SgEstimator::calcConditionNumber(const SgUtMatrix& R, const SgUtMatrix& RInv)
{
  double                        fNormOfR, fNormOfInv, k_F;
  int                           n;
  fNormOfR = fNormOfInv = 0.0;
  n = R.n();
  // calculate Frobenius norm for the upper triangular matrix and its inverse matrix:
  for (int i=0; i<n; i++)
    for (int j=i; j<n; j++)
    {
      fNormOfR  += R.   getElement(i, j)*R.   getElement(i, j);
      fNormOfInv+= RInv.getElement(i, j)*RInv.getElement(i, j);
    };
  fNormOfR  = sqrt(fNormOfR);
  fNormOfInv= sqrt(fNormOfInv);
  k_F = fNormOfR*fNormOfInv;
  logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() + 
    "::calcConditionNumber(): condition number: " + QString("").sprintf("k_F=%.6e with n=%d", k_F, n));
  if (k_F > maxConditionNumber_)
    maxConditionNumber_ = k_F;
  
};



//
void SgEstimator::finisRun()
{
SgMJD                           tWall4finisRun0=SgMJD::currentMJD();
SgMJD                           tWall4finisRun;


tWall0_ = SgMJD::currentMJD();
  if (0 < numOfThreads_)
    dataUpdate_th();
  else
    dataUpdate();
tWall_ = SgMJD::currentMJD();
time4DataUpdate_ += (tWall_ - tWall0_)*86400.0;



  // backward run:
  // solve:
  RPCarrier                     carrier;
  SgUtMatrix                   *RxInv=new SgUtMatrix(nX_);
  x_ = new SgVector(nX_);
  Px_ = new SgSymMatrix(nX_);
#ifdef LOCDEB
  std::cout << "nX_= " << nX_ << ", rpStack_.size()=" << rpStack_.size() << "\n";
#endif
  if (nX_ || rpStack_.size())
  {
#ifdef LOCDEB
    std::cout << "   === main sequence: Rx_:\n" << *Rx_ << "\n";
    std::cout << "   === main sequence: zx_:\n" << *zx_ << "\n\n";
#endif
    // solve the parameters:
    // Rx * x = zx
    solveEquation(*Rx_, *x_,  *zx_);
    *RxInv = ~*Rx_;
    calcConditionNumber(*Rx_, *RxInv);
    calculateRRT(*Px_, *RxInv);
    calcPwlTraces(listX_, *Px_);
    updateSolutionAtParameterList(*listX_, x_, Px_);
    submittSolution2GlobalStore(Px_, listX_);
    reportParameterList(*listX_);
    // get proper tLeft:
    SgMJD tL(tInf);
    for (int i=0; i<listX_->size(); i++)
      if (listX_->at(i)->getTLeft() < tL)
        tL = listX_->at(i)->getTLeft();
    if (rpStack_.size())
    {
#ifdef LOCDEB
      std::cout << "solving x, arc's stack size is: " << rpStack_.size() << "\n";
#endif
      while (!rpStack_.isEmpty())
      {
        carrier = rpStack_.pop();
#ifdef LOCDEB
        std::cout << " popped epoch: " << qPrintable(carrier.tRight_.toString()) << "\n";
#endif
        SgVector               *zxi = carrier.zx_;
        SgUtMatrix             *Rxi = carrier.Rx_;
        SgMatrix               *Rxxi= carrier.Rxx_;
        QList<SgParameter*>    *l   = carrier.list_;
        QList<SgParameter*>    *r   = carrier.listOthers_;
        SgMJD                   tL  = carrier.tLeft_;
        SgMJD                   tR  = carrier.tRight_;
        // "dependent" parameters (solved before):
#ifdef LOCDEB
        std::cout << " r->size(): " << r->size() << "\n";
#endif
        SgVector               *xd  = new SgVector(r->size());
        SgSymMatrix            *Pd  = new SgSymMatrix(r->size());
#ifdef LOCDEB
        std::cout << " l->size(): " << l->size() << "\n";
#endif
        if (l->size())
        {
#ifdef LOCDEB
          QString str;
          std::cout << "Processing batch for tR: " << qPrintable(tR.toString()) << "\n";
          for (int i=0; i<l->size(); i++)
          {
            std::cout << "i=" << i << "\n";
            SgParameter *p=l->at(i);
            str.sprintf(": [%2d] %s ", i, qPrintable(p->getName()));
            std::cout << "         l-list: " << qPrintable(str)
                      << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
                      << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
                      << "\n";
          };
          
          std::cout << "   __restored: Rx_:\n" << *carrier.Rx_ << "\n";
          std::cout << "   __restored: Rxx_:\n" << *carrier.Rxx_ << "\n";
          std::cout << "   __restored: zx_:\n" << *carrier.zx_ << "\n\n";
#endif
          if (r->size())
            collectDependentInfArray(carrier, xd, Pd);
          nX_ = l->size();
          // free previously allocated memory:
          if (RxInv)
            delete RxInv;
          if (x_)
            delete x_;
          if (Px_)
            delete Px_;
          RxInv = new SgUtMatrix(nX_);
          x_ = new SgVector(nX_);
          Px_ = new SgSymMatrix(nX_);
          solveEquation(*Rxi, *x_,  *zxi - *Rxxi**xd);
          *RxInv = ~*Rxi;
          calcConditionNumber(*Rxi, *RxInv);
          calculateRRT(*Px_, *RxInv);
          *Px_ += calcQForm(calcQForm(*Pd, *Rxxi), *RxInv);
          calcPwlTraces(l, *Px_);
          updateSolutionAtParameterList(*l, x_, Px_);
          SgMatrix *Pxx = new SgMatrix(l->size(), r->size());
          *Pxx = -calcProduct_mat_x_mat(calcProduct_mat_x_mat(*RxInv, *Rxxi), *Pd);
          submittSolution2GlobalStore(Px_, l, Pxx, r);
#ifdef LOCDEB
          std::cout << "\n";
          std::cout << "   The list of l-parameters:\n";
          for (int i=0; i<l->size(); i++)
          {
            SgParameter *p=l->at(i);
            std::cout << "    l-list: " 
                      << qPrintable(QString().sprintf(": [%2d] %s ", i, qPrintable(p->getName())))
                      << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
                      << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
                      << "\n";
          };
          std::cout << "   The list of r-parameters:\n";
          for (int i=0; i<r->size(); i++)
          {
            SgParameter *p=r->at(i);
            std::cout << "    r-list: " 
                      << qPrintable(QString().sprintf(": [%2d] %s ", i, qPrintable(p->getName())))
                      << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
                      << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
                      << "\n";
          };
          std::cout << "\n";
          std::cout << "      RxInv:\n" << *RxInv << "\n";
          std::cout << "\n";
          std::cout << "      Rxxi:\n" << *Rxxi << "\n";
          std::cout << "\n";
          std::cout << "      RxInv*Rxxi:\n" << calcProduct_mat_x_mat(*RxInv, *Rxxi) << "\n";
          std::cout << "\n";
          std::cout << "      Pd:\n" << *Pd << "\n";
          std::cout << "\n";
          std::cout << "      Pxx:\n" << *Pxx << "\n";
          std::cout << "\n";
#endif
          delete Pxx;
          reportParameterList(*l);
        };
        delete xd;
        delete Pd;
        //
        while (!carrier.list_->isEmpty())
          delete carrier.list_->takeFirst();
        delete carrier.list_;
        carrier.list_ = NULL;
        //
        while (!carrier.listOthers_->isEmpty())
          delete carrier.listOthers_->takeFirst();
        delete carrier.listOthers_;
        carrier.listOthers_ = NULL;
        //
        delete carrier.zx_;
        carrier.zx_ = NULL;
        delete carrier.Rx_;
        carrier.Rx_ = NULL;
        delete carrier.Rxx_;
        carrier.Rxx_ = NULL;
      };
    }
    else // solve stc parameters:
    {
tWall0_ = SgMJD::currentMJD();
      solveStochasticParameters();
//printf("  ++ Time for solveStochasticParameters(1) : %8.4f\n", (SgMJD::currentMJD() - tWall0_)*86400.0);
    }
  }
  else // solve stc parameters:
  {
tWall0_ = SgMJD::currentMJD();
    solveStochasticParameters();
//printf("  ++ Time for solveStochasticParameters(2) : %8.4f\n", (SgMJD::currentMJD() - tWall0_)*86400.0);
  };
#ifdef LOCDEB
  std::cout << "\n";
  std::cout << "   The list of all parameters:\n";
  for (int i=0; i<listXAll_->size(); i++)
  {
    SgParameter *p=listXAll_->at(i);
    std::cout << "    XAll-list: " 
              << qPrintable(QString().sprintf(": [%2d] %s ", i, qPrintable(p->getName())))
              << qPrintable(p-> getTLeft().toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << " : "
              << qPrintable(p->getTRight().toString(SgMJD::F_YYYYMMDDHHMMSSSS))
              << "\n";
  };
  std::cout << "\n";
  std::cout << "   Covariance matrix:\n" << *PxAll_ << "\n";
  std::cout << "\n";
#endif

  // for threads:
  /* free threads: */
  free(th4Prc_01_);
  free(args4Prc_01_);
  free(th4Prc_02_);
  free(args4Prc_02_);

  if (u4pth_)
  {
    delete u4pth_;
    u4pth_ = NULL;
  };

  //
  // tmp:
/*
printf("  ++ Time for dataUpdate: %8.4f\n", time4DataUpdate_);
printf("  ++ Time for propagate : %8.4f\n", time4PropagateStc_);
printf("  ++ Time for savingStc : %8.4f\n", time4SaveStc_);
printf("  ++ Time for processObs: %8.4f\n", time4ProcObs_);
printf("  ++ Time for finishRun : %8.4f\n", (SgMJD::currentMJD() - tWall4finisRun0)*86400.0);
*/


  // freeing allocated memory:
  // local:
  if (x_)
  {
    delete x_;
    x_ = NULL;
  };
  if (zx_)
  {
    delete zx_;
    zx_ = NULL;
  };
  if (Rx_)
  {
    delete Rx_;
    Rx_ = NULL;
  };
  if (Px_)
  {
    delete Px_;
    Px_ = NULL;
  };
  if (RxInv)
  {
    delete RxInv;
    RxInv = NULL;
  };
};



//
void SgEstimator::solveStochasticParameters()
{
  StochasticSolutionCarrier   stcSol;
  SmoothCarrier               carrier;
  p_  = new SgVector(nP_);
  Pp_ = new SgSymMatrix(nP_);
  SgUtMatrix *RpInv = new SgUtMatrix(nP_);
  if (nP_)
  {
    QList<SgParameter*> *listP_saved = listP_;
//    
    listP_ = new QList<SgParameter*>;
    for (int i=0; i<listP_saved->size(); i++)
      listP_->append(new SgParameter(*listP_saved->at(i)));
//    
    // solve:
    // the last solution, ~ == *, smoothed equal to filtered,
    // so, calc filtered estimation (the last in the batch):
    // solve the parameters:
    //             Rp * p   = zp  -  Rpq*q    -  Rpx*x

tWall0_ = SgMJD::currentMJD();

    solveEquation(*Rp_,*p_,  *zp_ - *Rpx_**x_);
    *RpInv = ~*Rp_;
    calculateRRT(*Pp_, *RpInv);
    //              Pp  +    Rp^(-1) * (Rpx * Px * Rpx^T) * Rp^(-T)
    if (nX_)
      *Pp_ += calcQForm(calcQForm(*Px_, *Rpx_), *RpInv);
    // there are two small additional terms, should add them too.

    updateSolutionAtParameterList(*listP_, p_, Pp_);
    // put the latest solution in the container:
    stcSol.epoch_ = tLastProcessed_;
    stcSol.list_ = listP_;
    if (logger->isEligible(SgLogger::DBG, SgLogger::STOCH))
      reportParameterList(*stcSol.list_);
    stcSol.x_ = new SgVector(*p_);
    stcSol.P_ = new SgSymMatrix(*Pp_);
    stcSolutions_.insert(stcSol.epoch_.toString(SgMJD::F_INTERNAL), stcSol);
    //
    // we move listP_ to the container stcSolutions_, no needs to delete it
    delete zp_s_;
    delete Rp_s_;
    delete Rpp_s_;
    delete Rpx_s_;
    // make a backward pass:
    SgVector    *pPrev = new SgVector(*p_);
    SgSymMatrix *PpPrev = new SgSymMatrix(*Pp_);
    int stcIdx=0;

//("  ++ Time for solveStochasticParameters phase 1 : %8.4f\n", (SgMJD::currentMJD() - tWall0_)*86400.0);

SgMJD                       tWallLocal0 = SgMJD::currentMJD();
double t4solveEquation=0.0;
double t4inv=0.0;
double tcalculateRRT=0.0;
double tcalcQForm1=0.0;
double tcalcQForm2=0.0;
double trest=0.0;

tWall0_ = SgMJD::currentMJD();
    while (!scStack_.isEmpty())
    {
      // pop saved matrices from the stack:
      carrier = scStack_.pop();
      listP_ = carrier.p_s_;
      zp_s_ = carrier.zp_s_;
      Rp_s_ = carrier.Rp_s_;
      Rpp_s_ = carrier.Rpp_s_;
      Rpx_s_ = carrier.Rpx_s_;

      // obtain solution:
tWallLocal0 = SgMJD::currentMJD();
      solveEquation(*Rp_s_,*p_,  *zp_s_ - *Rpp_s_**pPrev - *Rpx_s_**x_);
t4solveEquation += (SgMJD::currentMJD() - tWallLocal0)*86400.0;
      // evaluate covariance matrix:

tWallLocal0 = SgMJD::currentMJD();
      *RpInv = ~*Rp_s_;
t4inv += (SgMJD::currentMJD() - tWallLocal0)*86400.0;

tWallLocal0 = SgMJD::currentMJD();
      // here to add a calculations for covariance matrix:
      //  Pp = (Rp*)^(-1) * (Rp*)^(-T)
      calculateRRT(*Pp_, *RpInv);
      //      Pp +   (Rp*)^(-1) * ((Rpp*) * Pp_prev * (Rpp*)^T) * (Rp*)^(-T)
tcalculateRRT +=(SgMJD::currentMJD() - tWallLocal0)*86400.0;

tWallLocal0 = SgMJD::currentMJD();
      *Pp_ += calcQForm(calcQForm(*PpPrev, *Rpp_s_), *RpInv);
      //        Pp +   (Rp*)^(-1) * ((Rpx*) * Px * (Rpx*)^T) * (Rp*)^(-T)
tcalcQForm1 += (SgMJD::currentMJD() - tWallLocal0)*86400.0;

tWallLocal0 = SgMJD::currentMJD();
      if (nX_)
        *Pp_ += calcQForm(calcQForm(*Px_, *Rpx_s_), *RpInv);
tcalcQForm2 += (SgMJD::currentMJD() - tWallLocal0)*86400.0;
tWallLocal0 = SgMJD::currentMJD();
      // update list of parameters:
      updateSolutionAtParameterList(*listP_, p_, Pp_);
      *pPrev = *p_;
      *PpPrev = *Pp_;
      // and save them in the container:
      stcSol.epoch_ = carrier.epoch_;
      stcSol.list_ = listP_;
      if (logger->isEligible(SgLogger::DBG, SgLogger::STOCH))
        reportParameterList(*stcSol.list_);
      stcSol.x_ = new SgVector(*p_);
      stcSol.P_ = new SgSymMatrix(*Pp_);
      stcSolutions_.insert(stcSol.epoch_.toString(SgMJD::F_INTERNAL), stcSol);
    
      // delete objects:
      delete zp_s_;
      delete Rp_s_;
      delete Rpp_s_;
      delete Rpx_s_;
      zp_s_ = NULL;
      Rp_s_ = NULL;
      Rpp_s_ = NULL;
      Rpx_s_ = NULL;
      stcIdx++;
trest +=(SgMJD::currentMJD() - tWallLocal0)*86400.0;
    };

/*
printf("  ++ Time for solveStochasticParameters t4solveEquation : %8.4f\n", t4solveEquation);
printf("  ++ Time for solveStochasticParameters t4inv           : %8.4f\n", t4inv);
printf("  ++ Time for solveStochasticParameters tcalculateRRT   : %8.4f\n", tcalculateRRT);
printf("  ++ Time for solveStochasticParameters tcalcQForm1     : %8.4f\n", tcalcQForm1);
printf("  ++ Time for solveStochasticParameters tcalcQForm2     : %8.4f\n", tcalcQForm2);
printf("  ++ Time for solveStochasticParameters trest           : %8.4f\n", trest);
//printf("  ++ Time for solveStochasticParameters t4solveEquation : %8.4f\n", t4solveEquation);

printf("  ++ Time for solveStochasticParameters phase 2 : %8.4f\n", (SgMJD::currentMJD() - tWall0_)*86400.0);
*/

/////--------------------------------------------------------------------------------------------------------
    // restore the pointer:
    listP_ = listP_saved;
//  -> listP_ = taskMgr_->stochasticParameters();
    if (PpPrev)
    {
      delete PpPrev;
      PpPrev = NULL;
    };
    if (pPrev)
    {
      delete pPrev;
      pPrev = NULL;
    };
  };
  // freeing allocated memory:
  // stochastics:
  if (p_)
  {
    delete p_;
    p_ = NULL;
  };
  if (zp_)
  {
    delete zp_;
    zp_ = NULL;
  };
  if (Rp_)
  {
    delete Rp_;
    Rp_ = NULL;
  };
  if (Rpx_)
  {
    delete Rpx_;
    Rpx_ = NULL;
  };
  if (SR_)
  {
    delete SR_;
    SR_ = NULL;
  };
  if (Rp_s_)
  {
    delete Rp_s_;
    Rp_s_ = NULL;
  };
  if (Rpp_s_)
  {
    delete Rpp_s_;
    Rpp_s_ = NULL;
  };
  if (Rpx_s_)
  {
    delete Rpx_s_;
    Rpx_s_ = NULL;
  };
  if (zp_s_)
  {
    delete zp_s_;
    zp_s_ = NULL;
  };
  if (Pp_)
  {
    delete Pp_;
    Pp_ = NULL;
  };
  if (RpInv)
  {
    delete RpInv;
    RpInv = NULL;
  };
};



//
void SgEstimator::dataUpdate()
{
  // make a Householder transformation
  unsigned int  i,j,l;
  double        s,ul,g;
   
  // Part one: dealing with the "p" block:
  u_->clear();                                            // U-vector, + ul -- the first elementh of U
  for (l=0; l<nP_; l++)
  {
    s = Rp_->getElement(l,l)*Rp_->getElement(l,l);
    for (i=0; i<nZ_; i++)
      s += Ap_->getElement(i,l)*Ap_->getElement(i,l);
    if (s>0.0)
    {
      s = -signum(Rp_->getElement(l,l))*sqrt(s);
      ul = Rp_->getElement(l,l) - s;                      // U_l
      for (i=0; i<nZ_; i++)
        u_->setElement(i, Ap_->getElement(i,l));          // U_i (i=l+1, N)
      Rp_->setElement(l,l, s);
      s = 1.0/(s*ul);                                     // betta
      for (j=l+1; j<nP_; j++)                             // |<-- processing "Rp/Ap"-block
      {                                                   // |
        g = ul*Rp_->getElement(l,j);                      // |
        for (i=0; i<nZ_; i++)                             // |
          g += u_->getElement(i)*Ap_->getElement(i,j);    // |
        (*Rp_)(l,j) += s*g*ul;                            // |
        for (i=0; i<nZ_; i++)                             // |
          (*Ap_)(i,j) += s*g*u_->getElement(i);           // |
      };                                                  // |_________________________
      for (j=0; j<nX_; j++)                               // |<-- processing "Rpx/Ax"-block
      {                                                   // |
        g = ul*Rpx_->getElement(l,j);                     // |
        for (i=0; i<nZ_; i++)                             // |
          g += u_->getElement(i)*Ax_->getElement(i,j);    // |
        (*Rpx_)(l,j) += s*g*ul;                           // |
        for (i=0; i<nZ_; i++)                             // |
          (*Ax_)(i,j) += s*g*u_->getElement(i);           // |
      };                                                  // |_________________________
      g = ul*zp_->getElement(l);                          // |--
      for (i=0; i<nZ_; i++)                               // |
        g += u_->getElement(i)*z_->getElement(i);         // | dealing with the vectors Zp
      (*zp_)(l) += s*g*ul;                                // |            and Z
      for (i=0; i<nZ_; i++)                               // |
        (*z_)(i) += s*g*u_->getElement(i);                // |--
    };
  };
  //
  // Part two: dealing with the "x" block:
  u_->clear();                                            // U-vector, + ul -- the first elementh of U
  for (l=0; l<nX_; l++)
  {
    s = Rx_->getElement(l,l)*Rx_->getElement(l,l);
    for (i=0; i<nZ_; i++)
      s += Ax_->getElement(i,l)*Ax_->getElement(i,l);
    if (s>0.0)
    {
      s = -signum(Rx_->getElement(l,l))*sqrt(s);
      ul = Rx_->getElement(l,l) - s;                      // U_l
      for (i=0; i<nZ_; i++)                               //
        u_->setElement(i, Ax_->getElement(i,l));          // U_i (i=l+1, N)
      Rx_->setElement(l,l, s);                            //
      s = 1.0/(s*ul);                                     // betta
      for (j=l+1; j<nX_; j++)                             // |<-- processing "Rx/Ax"-block
      {                                                   // |
        g = ul*Rx_->getElement(l,j);                      // |
        for (i=0; i<nZ_; i++)                             // |
          g += u_->getElement(i)*Ax_->getElement(i,j);    // |
        (*Rx_)(l,j) += s*g*ul;                            // |
        for (i=0; i<nZ_; i++)                             // |
          (*Ax_)(i,j) += s*g*u_->getElement(i);           // |
      };                                                  // |_________________________
      g = ul*zx_->getElement(l);                          // |--
      for (i=0; i<nZ_; i++)                               // |
        g += u_->getElement(i)*z_->getElement(i);         // | dealing with the vectors Zx
      (*zx_)(l) += s*g*ul;                                // |            and Z
      for (i=0; i<nZ_; i++)                               // |
        (*z_)(i) += s*g*u_->getElement(i);                // |--
    };
  };
  nZ_ = 0;
};





//
void SgEstimator::dataUpdate_th()
{
  unsigned int                  l;
  int                           thIdx;
  int                           rc;
  pthread_mutex_t               *m;


  if (nP_)
  {
    /* init mutexes: */
    pthMutexes4Prc_02_ = (pthread_mutex_t*) calloc(nP_+nX_+1, sizeof(pthread_mutex_t)); // +1 for vector zp/z
    for(l=0, m=pthMutexes4Prc_02_; l<nP_+nX_+1; l++, m++)
      pthread_mutex_init(m, NULL);
    pthread_mutex_init(&pthMutL4Prc_02_, NULL);

    for (thIdx=0; thIdx<numOfThreads_; thIdx++)
    {
      (args4Prc_02_+thIdx)->nP_          = nP_;
      (args4Prc_02_+thIdx)->nX_          = nX_;
      (args4Prc_02_+thIdx)->nZ_         = nZ_;
      (args4Prc_02_+thIdx)->ppRp_       = Rp_->base();
      (args4Prc_02_+thIdx)->ppRpx_       = Rpx_->base();
      (args4Prc_02_+thIdx)->ppAp_       = Ap_->base();
      (args4Prc_02_+thIdx)->ppAx_       = Ax_->base();
      (args4Prc_02_+thIdx)->pZp_        = zp_->base();
      (args4Prc_02_+thIdx)->pthMutexes_  = pthMutexes4Prc_02_;
      (args4Prc_02_+thIdx)->pthMutL_    = &pthMutL4Prc_02_;
    };
    lastPrcColumn4Prc_02_ = -1;
    //
    // make a Householder transformation
    // Part one: dealing with the "p" block:
    l = 0;
    thIdx = 0;
    while (thIdx < numOfThreads_ && l < nP_)
    {
      rc = pthread_create(th4Prc_02_+thIdx, &pthreadAttr_, thPrc_02, (void *) (args4Prc_02_+thIdx));
      if (rc != 0)
        printf("create failed %d\n", rc);
      thIdx++;
      l++;
    };
    if (l == nP_)
    {  
      for (int k=0; k<thIdx; k++)
      {
        rc = pthread_join(*(th4Prc_02_+k), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
      };
    }
    else
    {
      thIdx = 0;
      while (l < nP_)
      {
        rc = pthread_join(*(th4Prc_02_+thIdx), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
        rc = pthread_create(th4Prc_02_+thIdx, &pthreadAttr_, thPrc_02, (void *) (args4Prc_02_+thIdx));
        if (rc != 0)
          printf("create failed %d\n", rc);
        thIdx++;
        l++;
        thIdx %= numOfThreads_;
      };
      for (int k=0; k<numOfThreads_; k++)
      {
        rc = pthread_join(*(th4Prc_02_+thIdx), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
        thIdx++;
        thIdx %= numOfThreads_;
      };
    };
/*
    for (l=0; l<nP_; l+=numOfThreads_)
    {
      for (thIdx=0; thIdx<numOfThreads_; thIdx++)
        if (l+thIdx < nP_)
        {
          rc = pthread_create(th4Prc_02_+thIdx, &pthreadAttr_, thPrc_02, (void *) (args4Prc_02_+thIdx));
          if (rc != 0)
            printf("create failed %d\n", rc);
        };
      for (thIdx=0; thIdx<numOfThreads_; thIdx++)
        if (l+thIdx < nP_)
        {
          rc = pthread_join(*(th4Prc_02_+thIdx), NULL);
          if (rc != 0)
            printf("join failed %d\n", rc);
        };
    };
*/
    /* free mutexes: */
    pthread_mutex_destroy(&pthMutL4Prc_02_);
    for(l=0, m=pthMutexes4Prc_02_; l<nP_+nX_+1; l++, m++)
      pthread_mutex_destroy(m);
    free(pthMutexes4Prc_02_);
  };
  //
  // Part two: dealing with the "x" block:
  if (nX_)
  {
    //
    /* init mutexes: */
    pthMutexes4Prc_01_ = (pthread_mutex_t*) calloc(nX_ + 1, sizeof(pthread_mutex_t)); // +1 for vector zx/z
    for(l=0, m=pthMutexes4Prc_01_; l<nX_+1; l++, m++)
      pthread_mutex_init(m, NULL);
    pthread_mutex_init(&pthMutL4Prc_01_, NULL);

    for (thIdx=0; thIdx<numOfThreads_; thIdx++)
    {
      (args4Prc_01_+thIdx)->num_        = nX_;
      (args4Prc_01_+thIdx)->nZ_         = nZ_;
      (args4Prc_01_+thIdx)->ppRx_       = Rx_->base();
      (args4Prc_01_+thIdx)->ppAx_       = Ax_->base();
      (args4Prc_01_+thIdx)->pZx_        = zx_->base();
      (args4Prc_01_+thIdx)->pthMutexes_  = pthMutexes4Prc_01_;
      (args4Prc_01_+thIdx)->pthMutL_    = &pthMutL4Prc_01_;
    };
    lastPrcColumn4Prc_01_ = -1;
/*
    for (l=0; l<nX_; l+=numOfThreads_)
    {
      for (thIdx=0; thIdx<numOfThreads_; thIdx++)
        if (l+thIdx < nX_)
        {
          rc = pthread_create(th4Prc_01_+thIdx, &pthreadAttr_, thPrc_01, (void *) (args4Prc_01_+thIdx));
          if (rc != 0)
            printf("create failed %d\n", rc);
        };
      for (thIdx=0; thIdx<numOfThreads_; thIdx++)
        if (l+thIdx < nX_)
        {
          rc = pthread_join(*(th4Prc_01_+thIdx), NULL);
          if (rc != 0)
            printf("join failed %d\n", rc);
        };
    };
*/
    // this one looks faster:
    l = 0;
    thIdx = 0;
    while (thIdx < numOfThreads_ && l < nX_)
    {
      rc = pthread_create(th4Prc_01_+thIdx, &pthreadAttr_, thPrc_01, (void *) (args4Prc_01_+thIdx));
      if (rc != 0)
        printf("create failed %d\n", rc);
      thIdx++;
      l++;
    };
    if (l == nX_)
    {  
      for (int k=0; k<thIdx; k++)
      {
        rc = pthread_join(*(th4Prc_01_+k), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
      };
    }
    else
    {
      thIdx = 0;
      while (l < nX_)
      {
        rc = pthread_join(*(th4Prc_01_+thIdx), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
        rc = pthread_create(th4Prc_01_+thIdx, &pthreadAttr_, thPrc_01, (void *) (args4Prc_01_+thIdx));
        if (rc != 0)
          printf("create failed %d\n", rc);
        thIdx++;
        l++;
        thIdx %= numOfThreads_;
      };
      for (int k=0; k<numOfThreads_; k++)
      {
        rc = pthread_join(*(th4Prc_01_+thIdx), NULL);
        if (rc != 0)
          printf("join failed %d\n", rc);
        thIdx++;
        thIdx %= numOfThreads_;
      };
    };
    /* free mutexes: */
    pthread_mutex_destroy(&pthMutL4Prc_01_);
    for(l=0, m=pthMutexes4Prc_01_; l<nX_+1; l++, m++)
      pthread_mutex_destroy(m);
    free(pthMutexes4Prc_01_);
    // pthreads done
  };

  nZ_ = 0;
};




//
void SgEstimator::propagateStochasticParameters(double dT)
{
  // propagate the stochastic parameters from an epoch "T" to a new one "T+dT":
  unsigned int        i,j,l;
  unsigned int        n=2*nP_, m=2*nP_+nX_;
  double              s,g;
  SR_->clear();
  u_ ->clear();
  //
  // fill the matrix SR:
  for (i=0; i<nP_; i++)
  {
    SgParameter      *p  = listP_->at(i);
    double            mm = p->calcM(dT);
    double            rwn= p->calcRW(dT);

    SR_->setElement(i,     i, -mm*rwn);
    SR_->setElement(i, nP_+i,  rwn);
    SR_->setElement(nP_+i, m,  zp_->getElement(i));

    for (j=i; j<nP_; j++)
      SR_->setElement(nP_+i,   j,  Rp_ ->getElement(i, j));
    for (j=0; j<nX_; j++)
      SR_->setElement(nP_+i, n+j,  Rpx_->getElement(i, j));
  };
  //
  // make a Householder transformation
  // this should be optimized...
  for (l=0; l<n; l++)
  {
    for (i=l, s=0.0; i<n; i++)
      s += SR_->getElement(i,l)*SR_->getElement(i,l);
    if (s>0.0)
    {
      s = -signum(SR_->getElement(l,l))*sqrt(s);
      u_->setElement(l,  SR_->getElement(l,l) - s);              // U_l
      for (i=l+1; i<n; i++)                                      //
        u_->setElement(i,  SR_->getElement(i,l));                // U_i (i=l+1, N)
      SR_->setElement(l,l, s);                                   //
      s = 1.0/(s*u_->getElement(l));                             // betta
      for (j=l+1; j<m+1; j++)                                    // |-------------------------
      {                                                          // |
        for (i=l,g=0.0; i<n; i++)                                // |
          g += u_->getElement(i)*SR_->getElement(i,j);           // |
        for (i=l; i<n; i++)                                      // |
          (*SR_)(i,j) += s*g*u_->getElement(i);                  // |
      };                                                         // |_________________________
    };
  };
  //
  // fill the matrices for smoothing and filtering:
  for (i=0; i<nP_; i++)
  {
    for (j=i; j<nP_; j++)
    {
      Rp_s_->setElement(i,j,  SR_->getElement(    i,     j));
      Rp_  ->setElement(i,j,  SR_->getElement(nP_+i, nP_+j));
    };
    for (j=0; j<nP_; j++)
      Rpp_s_->setElement(i,j, SR_->getElement(i, nP_+j));
    for (j=0; j<nX_; j++)
    {
      Rpx_s_->setElement(i,j, SR_->getElement(    i, n+j));
      Rpx_  ->setElement(i,j, SR_->getElement(nP_+i, n+j));
    };
    zp_  ->setElement(i,  SR_->getElement(nP_+i, m));
    zp_s_->setElement(i,  SR_->getElement(    i, m));
  };
};



/**/
//
void SgEstimator::propagateStochasticParameters_th(double dT)
{
  // propagate the stochastic parameters from an epoch "T" to a new one "T+dT":
  unsigned int                  i,j,l;
  unsigned int                  n=2*nP_, m=2*nP_+nX_;
  double                        s,g;
  double                        mm;
  double                        rwn;

  SR_->clear();
  u_ ->clear();
  //
  double                       **ppSR     = SR_->base();
  double                       **ppRp     = Rp_->base();
  double                       **ppRpx    = Rpx_->base();
  double                        *pZp      = zp_->base();
  double                        *pU       = u_->base();
  // 4 smoothing:
  double                       **ppRp_s  = Rp_s_->base();
  double                       **ppRpp_s  = Rpp_s_->base();
  double                       **ppRpx_s  = Rpx_s_->base();
  double                        *pZp_s    = zp_s_->base();

  double                        *p, *q;
  //
  // fill the matrix SR:
  for (i=0; i<nP_; i++)
  {
    SgParameter                 *p=listP_->at(i);
    mm  = p->calcM(dT);
    rwn = p->calcRW(dT);
    SR_->setElement(i,     i, -mm*rwn);
    SR_->setElement(i, nP_+i,  rwn);
  };
  // vector zp:
  memcpy((void*)(*(ppSR+m)+nP_), (void*)(pZp), sizeof(double)*(nP_));
  // matrix Rp:
  for (j=0; j<nP_; j++)
    memcpy((void*)(*(ppSR+j)+nP_), (void*)(*(ppRp+j)), sizeof(double)*(j+1));
  // matrix Rpx:
  for (j=0; j<nX_; j++)
    memcpy((void*)(*(ppSR+n+j)+nP_), (void*)(*(ppRpx+j)), sizeof(double)*(nP_));
  //
  // make a Householder transformation
  // this should be optimized...
  for (l=0; l<n; l++)
  {
    s = 0.0;
    p = *(ppSR+l) + l;
    for (i=l; i<n; i++, p++)
      s += *p**p;
    if (s>0.0)
    {
      s = -signum(*(*(ppSR+l)+l))*sqrt(s);
      *(pU+l) = *(*(ppSR+l)+l) - s;                                              // U_l
      memcpy((void*)(pU+l+1), (void*)(*(ppSR+l)+l+1), sizeof(double)*(n-l-1));   // U_i (i=l+1, N)
      *(*(ppSR+l)+l) = s;                                                       //
      s = 1.0/(s**(pU+l));                                                       // betta
      for (j=l+1; j<m+1; j++)                                                    // |---------------------
      {                                                                          // |
        g = 0.0;                                                                // |
        p = pU + l;                                                              // |
        q = *(ppSR+j) + l;                                                      // |
        for (i=l; i<n; i++, p++, q++)                                            // |
          g += *p**q;                                                           // |
        p = pU + l;                                                              // |
        q = *(ppSR+j) + l;                                                      // |
        for (i=l; i<n; i++, p++, q++)                                            // |
          *q += s*g**p;                                                          // |
      };                                                                         // |_____________________
    };
  };
  //
  // fill the vectors and matrices for smoothing and filtering:
  // vector zp:
  memcpy((void*)(pZp),   (void*)(*(ppSR+m)+nP_), sizeof(double)*(nP_));
  // vector zps:
  memcpy((void*)(pZp_s), (void*)(*(ppSR+m)    ), sizeof(double)*(nP_));
  for (j=0; j<nP_; j++)
  {
    // matrix Rp:
    memcpy((void*)(*(ppRp   +j)),  (void*)(*(ppSR+nP_+j)+nP_), sizeof(double)*(j+1));
    // matrix Rp_s:
    memcpy((void*)(*(ppRp_s +j)),  (void*)(*(ppSR+j)),         sizeof(double)*(j+1));
    // matrix Rpp_s:
    memcpy((void*)(*(ppRpp_s+j)),  (void*)(*(ppSR+nP_+j)),     sizeof(double)*(nP_));
  };
  //
  for (j=0; j<nX_; j++)
  {
    // matrix Rpx:
    memcpy((void*)(*(ppRpx+j)),   (void*)(*(ppSR+n+j)+nP_),  sizeof(double)*(nP_));
    // matrix Rpx_s:
    memcpy((void*)(*(ppRpx_s+j)), (void*)(*(ppSR+n+j)),  sizeof(double)*(nP_));
  };
};
/**/



//
void SgEstimator::save4Smoothing(const SgMJD& t)
{
  SmoothCarrier        carrier;
  // make copies:
  carrier.epoch_ = t;
  carrier.p_s_   = new QList<SgParameter*>;
  for (int i=0; i<listP_->size(); i++)
    carrier.p_s_->append(new SgParameter(*listP_->at(i)));
  carrier.zp_s_  = new SgVector(*zp_s_);
  carrier.Rp_s_  = new SgUtMatrix(*Rp_s_);
  carrier.Rpp_s_ = new SgMatrix(*Rpp_s_);
  carrier.Rpx_s_ = new SgMatrix(*Rpx_s_);
  // and store them:
  scStack_.push(carrier);
};



// some estimator-specific calculations:
//
SgVector& SgEstimator::solveEquation(const SgUtMatrix& R, SgVector& x, const SgVector& z)
{
#ifdef DEBUG
  if (x.n()!=z.n())
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::solveEquation(): incompatible ranges of vectors x and z");
  if (R.nRow()!=x.n())
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::solveEquation(): incompatible ranges of vector x and matrix R");
#endif //DEBUG

  unsigned int          i, j, n=x.n()-1;
  double                s;

  if (!x.n())
    return x;

  if (R.getElement(n,n) != 0.0)
    x(n) = z.getElement(n)/R.getElement(n,n);
  else
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::solveEquation(): division by zero caused by R[N,N]=0");
  for (i=n; i>0; i--) // i ==> (i+1)
  {
    for (j=i, s=0.0; j<x.n(); j++)
      s += R.getElement(i-1,j)*x(j);
    if (R.getElement(i-1,i-1) != 0.0)
      x(i-1) = (z.getElement(i-1) - s)/R.getElement(i-1,i-1);
    else
      logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
        "::solveEquation(): division by zero caused by R[j,j]=0");
  };
  return x;
};



//
SgSymMatrix& SgEstimator::calculateRRT(SgSymMatrix& M, const SgUtMatrix& R)
{
#ifdef DEBUG
  if (M.nRow() != R.nRow()) 
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::calculateRRT(): incompatible ranges of input matrices");
#endif //DEBUG
  //  double            **m=M.B, **r=R.B;
  double                v;
  unsigned int          i, j, l;

  for (i=0; i<M.nRow(); i++)
    for (j=i; j<M.nCol(); j++)
    {
      for (v=0.0, l=j; l<R.nCol(); l++)
        v += R.getElement(i,l)*R.getElement(j,l);
      M(i,j) = v;
    };
  return M;
};



/*
//
SgSymMatrix SgEstimator::calcQForm(const SgSymMatrix& P, const SgMatrix& A)
{
#ifdef DEBUG
  if (A.nCol() != P.nRow()) 
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::calcQForm: incompatible ranges of the matrices");
#endif //DEBUG

  SgSymMatrix         S(A.nRow(), false);
  SgMatrix            T(A.nRow(), A.nCol());
  unsigned int          i, j, l;
  double                s;
  
  for (i=0; i<T.nRow(); i++)
    for (j=0; j<T.nCol(); j++)
    {
      for (l=0, s=0.0; l<A.nCol(); l++) 
        s += A.getElement(i,l)*P.getElement(l,j);
      T(i,j) = s;
    };
  for (i=0; i<S.nRow(); i++)
    for (j=i; j<S.nCol(); j++)
    {
      for (l=0, s=0.0; l<T.nCol(); l++) 
        s += T.getElement(i,l)*A.getElement(j,l);
      S(i,j) = s;
    };
  return S;
};



//
SgSymMatrix SgEstimator::calcQForm(const SgSymMatrix& P, const SgUtMatrix& R)
{
#ifdef DEBUG
  if (R.nCol()!=P.nRow()) 
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::calcQForm: incompatible ranges of the matrices");
#endif //DEBUG

  SgSymMatrix S(R.nRow(), false);
  SgMatrix    T(R.nRow(), R.nCol());
  unsigned int          i, j, l;
  double                s;
  
  for (i=0; i<T.nRow(); i++)
    for (j=0; j<T.nCol(); j++)
    {
      for (l=i, s=0.0; l<R.nCol(); l++)
        s += R.getElement(i,l)*P.getElement(l,j);
      T(i,j) = s;
    };

  for (i=0; i<S.nRow(); i++)
    for (j=i; j<S.nCol(); j++)
    {
      for (l=j, s=0.0; l<T.nCol(); l++) 
        s += T.getElement(i,l)*R.getElement(j,l);
      S(i,j) = s;
    };
  return S;
};
*/



//
SgSymMatrix SgEstimator::calcQForm(const SgSymMatrix& P, const SgMatrix& A)
{
#ifdef DEBUG
  if (A.nCol() != P.nRow()) 
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::calcQForm(): incompatible ranges of the matrices");
#endif //DEBUG
  int                            i, j, k;
  int                           aNrow=A.nRow();
  int                           aNcol=A.nCol();
  SgSymMatrix                   S(aNrow, false);
  SgMatrix                      T(aNrow, aNcol);
  double                        s;
  //
  if (0 < numOfThreads_)
  {
    const double *const *        ppA=A.base_c();
    const double *const *        ppP=P.base_c();
    double                     **ppT=T.base();
    double                     **ppS=S.base();
    const double                *p;
    double                       *q;
    //
    for (j=0; j<aNcol; j++)
    {
      q = *(ppT+j);
      for (i=0; i<aNrow; i++, q++)
      {
        s = 0.0;
        p = *(ppP+j);
        for (k=0; k<j; k++, p++) 
          s += *(*(ppA+k)+i)**p;
        for (k=j; k<aNcol; k++) 
          s += *(*(ppA+k)+i)* *(*(ppP+k)+j);
        *q = s;
      };
    };
    for (j=0; j<aNrow; j++)
    {
      q = *(ppS+j);
      for (i=0; i<=j; i++, q++)
      {
        s = 0.0;
        for (k=0; k<aNcol; k++) 
          s += *(*(ppT+k)+i)**(*(ppA+k)+j);
        *q = s;
      };
    };
  }
  else
  {
    for (i=0; i<aNrow; i++)
      for (j=0; j<aNcol; j++)
      {
        for (k=0, s=0.0; k<aNcol; k++) 
          s += A.getElement(i,k)*P.getElement(k,j);
        T(i,j) = s;
      };
    for (i=0; i<aNrow; i++)
      for (j=i; j<aNrow; j++)
      {
        for (k=0, s=0.0; k<aNcol; k++) 
          s += T.getElement(i,k)*A.getElement(j,k);
        S(i,j) = s;
      };
  };
  //
  return S;
};



/**/
//
SgSymMatrix SgEstimator::calcQForm(const SgSymMatrix& P, const SgUtMatrix& R)
{
#ifdef DEBUG
  if (R.nCol()!=P.nRow()) 
    logger->write(SgLogger::ERR, SgLogger::MATRIX, className() + 
      "::calcQForm(): incompatible ranges of the matrices");
#endif //DEBUG
  int                            i, j, k;
  int                           n=R.nCol();
  SgSymMatrix                   S(n, false);
  SgMatrix                      T(n, n);
  double                        s;
  //
  if (0 < numOfThreads_)
  {
    const double *const *        ppR=R.base_c();
    const double *const *        ppP=P.base_c();
    double                     **ppT=T.base();
    double                     **ppS=S.base();
    const double                *p;
    double                      *q;
    //
    for (j=0; j<n; j++)
    {
      q = *(ppT+j);
      for (i=0; i<n; i++, q++)
      {
        s = 0.0;
        p = *(ppP+j)+i;
        for (k=i; k<j; k++, p++)
          s += *(*(ppR+k)+i)**p;
        for (k=j; k<n; k++, p++)
          s += *(*(ppR+k)+i)**(*(ppP+k)+j);
        *q = s;
      };
    };
    //
    for (j=0; j<n; j++)
    {
      q = *(ppS+j);
      for (i=0; i<=j; i++, q++)
      {
        s = 0.0;
        for (k=j; k<n; k++) 
          s += *(*(ppT+k)+i)**(*(ppR+k)+j);
        *q = s;
      };
    };
  }
  else
  {
    for (i=0; i<n; i++)
      for (j=0; j<n; j++)
      {
        for (k=i, s=0.0; k<n; k++)
          s += R.getElement(i,k)*P.getElement(k,j);
        T(i,j) = s;
      };
    for (i=0; i<n; i++)
      for (j=i; j<n; j++)
      {
        for (k=j, s=0.0; k<n; k++) 
          s += T.getElement(i,k)*R.getElement(j,k);
        S(i,j) = s;
      };
  };
  //
  return S;
};



//
void SgEstimator::prepareStochasticSolution4Epoch(const SgMJD& t, QList<SgParameter*> *stcPars)
{
  QString       str;
  if (!stcSolutions_.size() || !stcPars) // nothing to do
    return;

  for (int i=0; i<stcPars->size(); i++)
    stcPars->at(i)->resetStatistics();

  StochasticSolutionCarrier   stcSol;
  QString tag = t.toString(SgMJD::F_INTERNAL);
  if (stcSolutions_.contains(tag))
  {
    stcSol = stcSolutions_.value(tag);
    for (int i=0; i<stcSol.list_->size(); i++)
      if (stcSol.list_->at(i)->getNumObs())
      {
        SgParameter *p=stcPars->at(i);
        p->setSolution(stcSol.list_->at(i)->getSolution());
        p->setSigma(stcSol.list_->at(i)->getSigma());
        p->setNumObs(stcSol.list_->at(i)->getNumObs());
      };
  };
};



//
void SgEstimator::clearPartials()
{
  unsigned int                  i;
  for (int i=0; i<listXAll_->size(); i++)
    listXAll_->at(i)->zerofy();
  for (i=0; i<nP_; i++)
    listP_->at(i)->zerofy();
  for (int i=0; i<listA_->size(); i++)
    listA_->at(i)->zerofy();
  for (int i=0; i<listPwl_->size(); i++)
    listPwl_->at(i)->zerofy();
/*
  for (int i=0; i<arcStorage_->size(); i++)
  {
    SgArcStorage               *arcP=arcStorage_->at(i);
    for (int j=0; j<arcP->getNum(); j++)
      arcP->getPi(j)->zerofy();
  };
  for (int i=0; i<pwlStorage_->size(); i++)
    pwlStorage_->at(i)->zerofy();
*/
};



//
double SgEstimator::calcAX(const SgMJD& t, bool isWorkingBand)
{
  unsigned int      i;
  SgParameter      *p;
  double            sum = 0.0;
  //
  // regular parameters:
  for (int i=0; i<listX_->size(); i++)
  {
    p = listX_->at(i);
    if (!p->isAttr(SgPartial::Attr_IS_SPECIAL))
      sum += p->getD()*p->getSolution();
  };
  //
  // arc parameters:
  for (int i=0; i<arcStorage_->size(); i++)
    sum += arcStorage_->at(i)->calcAX(t, isWorkingBand);
  //
  // PWL parameters:
  for (int i=0; i<pwlStorage_->size(); i++)
    sum += pwlStorage_->at(i)->calcAX(t, isWorkingBand);
  //
  // stochastic parameters:
  for (i=0; i<nP_; i++)
    sum += listP_->at(i)->getD()*listP_->at(i)->getSolution();

  return sum;
};



//
void SgEstimator::deployArcParameters(const SgMJD& tFirst, const SgMJD& tLast, const SgMJD& tRefer)
{
  SgArcStorage    *arcP=NULL;
  int              num=0;
  for (int i=0; i<listA_->size(); i++)
  {
    SgParameter *p=listA_->at(i);
    arcP = new SgArcStorage;
    arcP->deployParameters(p, tFirst, tLast, tRefer);
    arcStorage_->append(arcP);
  };
  // 
  for (int i=0; i<arcStorage_->size(); i++)
  {
    arcP = arcStorage_->at(i);
    for (int j=0; j<arcP->getNum(); j++)
      addParameter(arcP->getPi(j), true);
    num += arcP->getNum();
  };
  logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() +
    "::deployArcParameters(): deployed " + QString("").sprintf("%d", num) + " single parameters");
};



//
void SgEstimator::deployPwlParameters(const SgMJD& tFirst, const SgMJD& tLast, const SgMJD& tRefer)
{
  SgPwlStorage    *pwl=NULL;
  int              num=0;
  for (int i=0; i<listPwl_->size(); i++)
  {
    SgParameter *p=listPwl_->at(i);
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
    pwl->deployParameters(p, tFirst, tLast, tRefer, config_);
    pwlStorage_->append(pwl);
    pwlStorageByName_->insert(p->getName(), pwl);
  };
  //
  for (int i=0; i<pwlStorage_->size(); i++)
  {
    pwl = pwlStorage_->at(i);
    for (int j=0; j<pwl->getNumOfPolynomials(); j++)
      addParameter(pwl->getP_Ai(j), true);
    num += pwl->getNumOfPolynomials();
    for (int j=0; j<pwl->getNumOfNodes(); j++)
      addParameter(pwl->getP_Bi(j), true);
    num += pwl->getNumOfNodes();
  };
  if (num)
    logger->write(SgLogger::DBG, SgLogger::ESTIMATOR, className() +
      "::deployPwlParameters(): deployed " + QString("").sprintf("%d", num) + " single parameters");
};



//
void SgEstimator::propagateArcPartials(const SgMJD& t)
{
  for (int i=0; i<arcStorage_->size(); i++)
    arcStorage_->at(i)->propagatePartials(t);
};



//
void SgEstimator::propagatePwlPartials(const SgMJD& t)
{
  for (int i=0; i<pwlStorage_->size(); i++)
    pwlStorage_->at(i)->propagatePartials(t);
};



//
void SgEstimator::calcPwlTraces(const QList<SgParameter*> *listX, const SgSymMatrix& mPx)
{
  for (int i=0; i<pwlStorage_->size(); i++)
    pwlStorage_->at(i)->collectTraces4Sfo(listX, mPx);
};



//
void SgEstimator::collectDependentInfArray(const RPCarrier& car, SgVector* xd, SgSymMatrix* Pd)
{
  QList<SgParameter*>        *r=car.listOthers_;
  int                         num=r->size();
  int                        *idxs=new int[num];

  for (int i=0; i<num; i++)
    idxs[i] = r->at(i)->getIdx();

  for (int i=0; i<num; i++)
  {
    SgParameter *p=r->at(i);
    if (listXAll_->at(idxs[i])->isAttr(SgParameter::Attr_IS_SOLVED))
      xd->setElement(i, listXAll_->at(idxs[i])->getSolution());
    else
      logger->write(SgLogger::WRN, SgLogger::ESTIMATOR, className() +
        "::collectDependentInfArray(): trying to get a parameter (" + p->getName() +
        ") that was not yet solved.");
    for (int j=i; j<num; j++)
      Pd->setElement(i,j,  PxAll_->getElement(idxs[i], idxs[j]));
  };
  delete[] idxs;
};



//
void SgEstimator::submittSolution2GlobalStore( SgSymMatrix* Px, QList<SgParameter*>* xList,
                                                  SgMatrix* Pxx, QList<SgParameter*>* others)
{
  int       *idxs_x, n=xList->size();
  idxs_x = new int[n];
  for (int i=0; i<n; i++)
    idxs_x[i] = xList->at(i)->getIdx();

  // the vector of parameters:
  for (int i=0; i<n; i++)
  {
    SgParameter *p=listXAll_->at(idxs_x[i]);
    if (p->isAttr(SgParameter::Attr_IS_SOLVED))
      logger->write(SgLogger::WRN, SgLogger::ESTIMATOR, className() +
        "::submittSolution2GlobalStore(): trying to set up a parameter (" + p->getName() +
        ") that was already solved.");
    else
    {
      SgParameter *p2copy=xList->at(i);
      p->setSolution(p2copy->getSolution());
      p->setSigma(p2copy->getSigma());
      p->setNumObs(p2copy->getNumObs());
      p->addAttr(SgParameter::Attr_IS_SOLVED);
    };
  };
  // the covariance matrix:
  for (int i=0; i<n; i++)
    for (int j=0; j<=i; j++)
      PxAll_->setElement(idxs_x[i], idxs_x[j], Px->getElement(i, j));
  if (!(Pxx && others))
  {
    delete[] idxs_x;
    return;
  };
  // 
  int       *idxs_o, m=others->size();
  idxs_o = new int[m];
  for (int i=0; i<m; i++)
    idxs_o[i] = others->at(i)->getIdx();
  for (int i=0; i<n; i++)
    for (int j=0; j<m; j++)
      PxAll_->setElement(idxs_x[i], idxs_o[j], Pxx->getElement(i, j));
  delete[] idxs_x;
  delete[] idxs_o;
};



//
double SgEstimator::calc_aT_P_a(const SgMJD& t)
{
  double                        aT_P_a;
  propagateArcPartials(t);
  propagatePwlPartials(t);
  int                           n=PxAll_->nCol();
  SgVector                     *a=new SgVector(n);
  for (int i=0; i<n; i++)
    a->setElement(i, listXAll_->at(i)->getD());
  aT_P_a = *a*(*PxAll_**a);
  // std::cout << "  aTPa=" << aT_P_a << "\n";
  delete a;
  return aT_P_a;
};



//
double SgEstimator::calc_aT_P_a_opt(const SgMJD& t)
{
  double                        aT_P_a, sum;
  int                           numAll=listXAll_->size(), num=listX_->size();
  SgVector                     *vP_a = new SgVector(numAll);
  bool                         *usedRows = new bool[numAll];

  // init the array:
  for (int i=0; i<numAll; i++)
    usedRows[i] = false;
  // mark actually used rows:
  for (int i=0; i<num; i++)
    usedRows[listX_->at(i)->getIdx()] = true;
  for (int i=0; i<pwlStorage_->size(); i++)
  {
    SgPwlStorage               *pwl=pwlStorage_->at(i);
    int                         idx=pwl->calcCurrentIdx(t);
    for (int j=0; j<idx+1; j++)
      usedRows[pwl->getP_Bi(j)->getIdx()] = true;
    for (int j=0; j<pwl->getNumOfPolynomials(); j++)
      usedRows[pwl->getP_Ai(j)->getIdx()] = true;
  };
  // calc the product only for actually used rows:
  for (int i=0; i<numAll; i++)
  if (usedRows[i])
    {
      sum = 0.0;
      // regular parameters:
      for (int j=0; j<num; j++)
      {
        SgParameter            *p=listX_->at(j);
        if (!p->isAttr(SgPartial::Attr_IS_SPECIAL))
          sum += PxAll_->getElement(i, p->getIdx())*p->getD();
      };
      // arc parameters:
      for (int j=0; j<arcStorage_->size(); j++)
        sum += arcStorage_->at(j)->calc_P_a(*PxAll_, i, t);
      //
      // PWL parameters:
      for (int j=0; j<pwlStorage_->size(); j++)
        sum += pwlStorage_->at(j)->calc_P_a(*PxAll_, i, t);
      vP_a->setElement(i, sum);
    };
  
  aT_P_a = 0.0;
  //
  // regular parameters:
  for (int i=0; i<num; i++)
  {
    SgParameter     *p=listX_->at(i);
    if (!p->isAttr(SgPartial::Attr_IS_SPECIAL))
      aT_P_a += p->getD()*vP_a->getElement(p->getIdx());
  };
  // arc parameters:
  for (int j=0; j<arcStorage_->size(); j++)
    aT_P_a += arcStorage_->at(j)->calc_aT_P_a(*vP_a, t);
  //
  // PWL parameters:
  for (int i=0; i<pwlStorage_->size(); i++)
    aT_P_a += pwlStorage_->at(i)->calc_aT_P_a(*vP_a, t);
  //
  delete[] usedRows;
  delete vP_a;
  return aT_P_a;
};



//
double SgEstimator::calc_aT_P_a_opt(const SgMJD& t, const QList<SgParameter*>& pars)
{
  if (listP_->size()) // not implemented
    return 0.0;
  
  propagateArcPartials(t);
  propagatePwlPartials(t);
  SgPwlStorage                 *pwl=NULL;
  //
  // have to add arcs here too...
  //  SgArcStorage                 *arc=NULL;
  int                           num(0);
  // first, calculate the size of the vector a:
  for (int i=0; i<pars.size(); i++)
  {
    SgParameter                *p=pars.at(i);
    // PWL:
    if (p->getPMode() == SgParameterCfg::PM_PWL)
    {
      pwl = pwlStorageByName_->value(p->getName());
      num += pwl->getNumOfActiveParameters(t);
    }
    // ARC:
    else if (p->getPMode() == SgParameterCfg::PM_ARC)
    {
      //aByName_
      // arc = arcStorageByName_->value(p->getName());
      // num += arc->getNum();
    }
    // regular:
    else
      num++;
  };
  //
  // then, fill the vector:
  SgVector                     *va=new SgVector(num);
  int                          *loc2glb, idx;
  loc2glb = new int[num];
  idx = 0;
  QList<SgParameter*>           pwlActivePars;
  for (int i=0; i<pars.size(); i++)
  {
    SgParameter                *p=pars.at(i);
    // PWL:
    if (p->getPMode() == SgParameterCfg::PM_PWL)
    {
      pwl = pwlStorageByName_->value(p->getName());
      pwl->getListOfActiveParameters(t, pwlActivePars);
      SgParameter              *pp;
      for (int j=0; j<pwlActivePars.size(); j++)
      {
        pp = pwlActivePars.at(j);
        va->setElement(idx, pp->getD());
        loc2glb[idx] = pp->getIdx();
        pp->zerofy();
        idx++;
      };
    }
    // ARC:
    else if (p->getPMode() == SgParameterCfg::PM_ARC)
    {
      //aByName_
      // arc = arcStorageByName_->value(p->getName());
      // num += arc->getNum();
    }
    // regular:
    else// if (p->getPMode() != SgParameterCfg::PM_STC)
    {
      va->setElement(idx, p->getD());
      loc2glb[idx] = p->getIdx();
      p->zerofy();
      idx++;
    };
  };
  //
  double                        aT_P_a(0.0), sum(0.0);
  SgVector                     *vP_a=new SgVector(num);
  // first, calculate P*a:
  for (int i=0; i<num; i++)
  {
    //std::cout << "  i=" << i << " va->getElement(i)=" << va->getElement(i) << "\n";
    sum = 0.0;
    for (int j=0; j<num; j++)
    {
//      if (pars.at(i)->getPMode() != SgParameterCfg::PM_STC && 
//          pars.at(j)->getPMode() != SgParameterCfg::PM_STC)
        sum += PxAll_->getElement(loc2glb[i], loc2glb[j])*va->getElement(j);
//std::cout << "  i=" << i << "  j=" << j
//          << " PxAll_->getElement(loc2glb[i], loc2glb[j])=" << PxAll_->getElement(loc2glb[i], loc2glb[j]) << "\n";
    };
    vP_a->setElement(i, sum);
  };
  //std::cout << "\n";
  //
  // and then, evaluate aT*(P*a):
  aT_P_a = 0.0;
  for (int i=0; i<num; i++)
    aT_P_a += va->getElement(i)*vP_a->getElement(i);
  //
  //
  delete[] loc2glb;
  delete va;
  delete vP_a;
  return aT_P_a;
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
