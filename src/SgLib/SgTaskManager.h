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

#ifndef SG_TASK_MANAGER_H
#define SG_TASK_MANAGER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>


#include <SgMJD.h>
#include <SgVlbiSession.h>
#include <SgTask.h>


class SgEstimator;
class SgParameter;
class SgRefraction;
class SgObservation;
class SgVlbiSession;
class SgVlbiSessionInfo;



/***===================================================================================================*/
/**
 * SgTaskManager -- an object that run all the things.
 *
 */
/**====================================================================================================*/
class SgTaskManager
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgTaskManager();

  /**A constructor.
   * Creates an object and set up its task.
   */
  inline SgTaskManager(SgTask*);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgTaskManager();



  //
  // Interfaces:
  //
  /** Retruns a reference on the pointer of the current session info:
   */
  SgVlbiSessionInfo*& currentSessionInfo() {return currentSessionInfo_;};

  /** Retruns a reference on the pointer of the current session:
   */
  SgVlbiSession*& currentSession() {return currentSession_;};

  // gets:
  /**Returns a pointer on the current task.
   */
  inline SgTask* getTask() const;

  /**Returns a pointer on the estimator.
   */
//  inline SgEstimator* estimator();

  /**Returns a pointer on the estimator.
   */
  inline SgEstimator* estimator();

  /**Returns a status of task owner.
   */
  inline bool getIsTaskOwner();

  /**Returns a pointer on a Refraction object.
   */
  inline SgRefraction* refraction();

  /**Returns an epoch of the begin of processing:
   */
  inline const SgMJD& getStartRunEpoch() const;

  /**Returns an epoch of the end of processing:
   */
  inline const SgMJD& getFinisRunEpoch() const;

  /**Returns an epoch of the first observations (approx):
   */
  inline const SgMJD& getTStart() const;
  inline const SgMJD& getTRefer() const {return tRefer_;};

  /**Returns an epoch of the last observations (approx):
   */
  inline const SgMJD& getTFinis() const;

  /**Returns number of parameters:
   */
  inline int getNumOfParameters() const;

  /**Returns number of parameters:
   */
  inline int getNumOfConstraints() const;

  // sets:
  /**Sets up a pointer on new task.
   */
  inline void setTask(SgTask*);

  /**Sets up a status of task owner.
   */
  inline void setIsTaskOwner(bool);

  /**Sets up a list of observations from an external container.
   */
  inline void setObservations(QList<SgObservation*>*);

  /**Returns a pointer on a global parameters list.
   */
  QList<SgParameter*>* globalParameters() {return globalParameters_;};

  /**Returns a pointer on an arc parameters list.
   */
  QList<SgParameter*>* arcParameters() {return arcParameters_;};

  /**Returns a pointer on a local parameters list.
   */
  QList<SgParameter*>* localParameters() {return localParameters_;};

  /**Returns a pointer on a PWL parameters list.
   */
  QList<SgParameter*>* pwlParameters() {return pwlParameters_;};

  /**Returns a pointer on a stochastic parameters list.
   */
  QList<SgParameter*>* stochasticParameters() {return stochasticParameters_;};


  inline void setHave2InteractWithGui(bool have2) {have2InteractWithGui_ = have2;};

  inline void setLongOperationStart(void (*operation)(int, int, const QString&))
    {longOperationStart_ = operation;};

  inline void setLongOperationProgress(void (*operation)(int)) {longOperationProgress_ = operation;};

  inline void setLongOperationStop(void (*operation)()) {longOperationStop_ = operation;};

  inline void setLongOperationMessage(void (*operation)(const QString&))
    {longOperationMessage_ = operation;};

  inline void setLongOperationShowStats(void (*operation)
    (const QString& label, int numTot, int numPrc, int numUsb, double wrms, double chi2))
    {longOperationShowStats_ = operation;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  virtual const QString className();
  
  virtual void run(bool =true);
  virtual bool prepare4Run();
  virtual void finisRun();
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  SgTask                       *task_;
  bool                          isTaskOwner_;
  SgEstimator                  *estimator_;
  SgMJD                         startRunEpoch_;
  SgMJD                         finisRunEpoch_;
  int                           numOfParameters_;
  int                           numOfConstraints_;
  

  // a cache of observations:
  QList<SgObservation*>  *observations_;
  bool                    isObsListOwner_;
  // working list of sessions infos:
  SessionInfos            sessions_;
  // current session info:
  SgVlbiSessionInfo      *currentSessionInfo_;
  // current session:
  SgVlbiSession          *currentSession_;

  // lists of parameters:
  QList<SgParameter*>    *globalParameters_;
  QList<SgParameter*>    *arcParameters_;
  QList<SgParameter*>    *localParameters_;
  QList<SgParameter*>    *pwlParameters_;
  QList<SgParameter*>    *stochasticParameters_;
  
  // for calculations:
  SgRefraction           *refraction_;
  SgMJD                   tStart_;
  SgMJD                   tFinis_;
  SgMJD                   tRefer_;
  
  // private functions:
  // session manipulations:
  void loadVlbiSession(const QString&);
  void saveVlbiSession();
  // lists of parameters:
  void updateParamaterLists();
  void fillParameterList(QList<SgParameter*>*&, SgParameterCfg::PMode);
  // constraints:
  int constrainClocks();
  int constrainStcPositionsNNT();
  int constrainStcPositionsNNR();
  int constrainSourcesPositions();
  int constrainTroposphericParameters();
  int constrainSourcesTmp();
  
  // interactions with GUI:
  bool  have2InteractWithGui_;
  void (*longOperationStart_) (int minStep, int maxStep, const QString& message);
  void (*longOperationProgress_) (int step);
  void (*longOperationStop_) ();
  void (*longOperationMessage_) (const QString& message);
  void (*longOperationShowStats_) (const QString& label, int numTot, int numPrc, int numUsb, 
    double wrms, double chi2);
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgTaskManager inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgTaskManager::SgTaskManager() :
  startRunEpoch_(tZero),
  finisRunEpoch_(tZero),
  tStart_(tZero),
  tFinis_(tZero),
  tRefer_(tZero)
{
  task_ = NULL;
  isTaskOwner_ = false;
//  estimator_ = NULL;
  estimator_ = NULL;
  observations_ = NULL;
  isObsListOwner_ = true;
  globalParameters_ = NULL;
  arcParameters_ = NULL;
  localParameters_ = NULL;
  pwlParameters_ = NULL;
  stochasticParameters_ = NULL;
  currentSessionInfo_ = NULL;
  currentSession_ = NULL;
  refraction_ = NULL;
  numOfParameters_ = 0;
  numOfConstraints_ = 0;
  have2InteractWithGui_ = false;
  longOperationStart_ = NULL;
  longOperationProgress_ = NULL;
  longOperationStop_ = NULL;
  longOperationMessage_  = NULL;
  longOperationShowStats_ = NULL;
};



// another constructor:
inline SgTaskManager::SgTaskManager(SgTask* task) :
  startRunEpoch_(tZero),
  finisRunEpoch_(tZero),
  tStart_(tZero),
  tFinis_(tZero),
  tRefer_(tZero)
{
  task_ = task;
  isTaskOwner_ = false;
//  estimator_ = NULL;
  estimator_ = NULL;
  observations_ = NULL;
  isObsListOwner_ = true;
  globalParameters_ = NULL;
  arcParameters_ = NULL;
  localParameters_ = NULL;
  pwlParameters_ = NULL;
  stochasticParameters_ = NULL;
  currentSessionInfo_ = NULL;
  currentSession_ = NULL;
  refraction_ = NULL;
  numOfParameters_ = 0;
  numOfConstraints_ = 0;
};



//
// INTERFACES:
//
//
inline SgTask* SgTaskManager::getTask() const
{
  return task_;
};



//
inline bool SgTaskManager::getIsTaskOwner()
{
  return isTaskOwner_;
};



//
inline SgEstimator* SgTaskManager::estimator()
{
  return estimator_;
};



//
inline SgRefraction* SgTaskManager::refraction()
{
  return refraction_;
};



//
inline const SgMJD& SgTaskManager::getStartRunEpoch() const
{
  return startRunEpoch_;
};



//
inline const SgMJD& SgTaskManager::getFinisRunEpoch() const
{
  return finisRunEpoch_;
};



//
inline const SgMJD& SgTaskManager::getTStart() const
{
  return tStart_;
};



//
inline const SgMJD& SgTaskManager::getTFinis() const
{
  return tFinis_;
};



//
inline int SgTaskManager::getNumOfParameters() const
{
  return numOfParameters_;
};



//
inline int SgTaskManager::getNumOfConstraints() const
{
  return numOfConstraints_;
};



//
inline void SgTaskManager::setTask(SgTask* task)
{
  if (isTaskOwner_ && task_)
    delete task_;
  task_ = task;
};



//
inline void SgTaskManager::setIsTaskOwner(bool is)
{
  isTaskOwner_ = is;
};



//
inline void SgTaskManager::setObservations(QList<SgObservation*> *observations)
{
  observations_ = observations;
  isObsListOwner_ = false;
};




//
// FRUNCTIONS:
//
//
//



//
// FRIENDS:
//
//
//
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif //SG_TASK_MANAGER_H
