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

#ifndef SG_TASK_H
#define SG_TASK_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>
#include <QtCore/QList>



#include <SgParametersDescriptor.h>
#include <SgTaskConfig.h>
#include <SgVlbiBaselineInfo.h>
#include <SgVlbiSessionInfo.h>
#include <SgVlbiSourceInfo.h>
#include <SgVlbiStationInfo.h>



//class SgAttribute;
//class SgMJD;
//class SgVlbiSession;

class SgVlbiSession;

/***===================================================================================================*/
/**
 * SgTask -- a list of sessions plus set of models.
 *
 */
/**====================================================================================================*/
class SgTask
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgTask(const QString&);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgTask();


  //
  // Interfaces:
  //  
  /**Returns name of the task.
   */
  inline const QString& getName() const;

  /**Returns config of the task.
   */
  inline SgTaskConfig* config(); // direct access:

  /**Returns config of the task.
   */
  inline SgParametersDescriptor* parameters(); // direct access:

  /**Returns a reference on a map of session infos by session name.
   */
  inline SessionInfosByName& sessionsByName();

  /**Returns a reference on a map of station infos by its name.
   */
  inline StationsByName& stationsByName();

  /**Returns a reference on a map of baseline infos by its name.
   */
  inline BaselinesByName& baselinesByName();

  /**Returns a reference on a map of sources infos by its name.
   */
  inline SourcesByName& sourcesByName();

  /**Sets up a name of the task.
   */
  inline void setName(const QString&);
  
  /**Sets up a config of the task.
   */
  inline void setConfig(const SgTaskConfig&);
  
  /**Sets up a parameters descriptor of the task.
   */
  inline void setParameters(const SgParametersDescriptor&);
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Adds a session info to the list of session infos. SgVlbiSession* -- to consult
   * about stations/sources/baselines available at this session.
   */
  void addSession(SgVlbiSessionInfo*, SgVlbiSession*);

  /**Removes a session info from the list of session infos. SgVlbiSession* -- to consult
   * about stations/sources/baselines available at this session (to update he statistics).
   */
  void removeSession(SgVlbiSessionInfo*, SgVlbiSession*);
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  // task name:
  QString                               name_;
  // config:
  SgTaskConfig                          config_;
  // parameters:
  SgParametersDescriptor                parameters_;
  // maps, object-by-name:
  SessionInfosByName                    sessionsByName_;
  StationsByName                        stationsByName_;
  BaselinesByName                       baselinesByName_;
  SourcesByName                         sourcesByName_;
  // hashes:
  // ...
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgTask inline members:                                                                              */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgTask::SgTask(const QString& name) :
  name_(name),
  config_(),
  parameters_(),
  sessionsByName_(),
  stationsByName_(),
  baselinesByName_(),
  sourcesByName_()
{
};



//
// INTERFACES:
//
//
//
inline const QString& SgTask::getName() const
{
  return name_;
};



//
inline SgTaskConfig* SgTask::config()
{
  return &config_;
};



inline SgParametersDescriptor* SgTask::parameters()
{
  return &parameters_;
};



//
inline QMap<QString, SgVlbiSessionInfo*>& SgTask::sessionsByName()
{
  return sessionsByName_;
};



//
inline QMap<QString, SgVlbiStationInfo*>& SgTask::stationsByName()
{
  return stationsByName_;
};
  


//
inline QMap<QString, SgVlbiBaselineInfo*>& SgTask::baselinesByName()
{
  return baselinesByName_;
};
  


//
inline QMap<QString, SgVlbiSourceInfo*>& SgTask::sourcesByName()
{
  return sourcesByName_;
};



//
inline void SgTask::setName(const QString& name)
{
  name_ = name;
};



//
inline void SgTask::setConfig(const SgTaskConfig& cfg)
{
  config_ = cfg;
};



//
inline void SgTask::setParameters(const SgParametersDescriptor& p)
{
  parameters_ = p;
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
#endif //SG_TASK_H
