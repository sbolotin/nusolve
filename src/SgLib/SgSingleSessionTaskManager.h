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

#ifndef SG_SINGLE_SESSION_TASK_MANAGER_H
#define SG_SINGLE_SESSION_TASK_MANAGER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>


#include <SgTaskManager.h>




/***===================================================================================================*/
/**
 * SgSingleSessionTaskManager -- an object that run all the things (a single-VLBI-session version).
 *
 */
/**====================================================================================================*/
class SgSingleSessionTaskManager : public SgTaskManager
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgSingleSessionTaskManager();

  /**A constructor.
   * Creates an object and set up its task.
   */
  inline SgSingleSessionTaskManager(SgTask*);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual inline ~SgSingleSessionTaskManager();



  //
  // Interfaces:
  //
  // gets:

  // sets:

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  virtual const QString className();
  
  virtual bool prepare4Run();

  virtual void run(bool =true);

  virtual void finisRun();
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  // private functions:
  void updateParamaterLists();
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgSingleSessionTaskManager inline members:                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgSingleSessionTaskManager::SgSingleSessionTaskManager() :
  SgTaskManager()
{
};



// another constructor:
inline SgSingleSessionTaskManager::SgSingleSessionTaskManager(SgTask* task) :
  SgTaskManager(task)
{
};



// A destructor:
inline SgSingleSessionTaskManager::~SgSingleSessionTaskManager()
{
};



//
// INTERFACES:
//
//




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
#endif //SG_SINGLE_SESSION_TASK_MANAGER_H
