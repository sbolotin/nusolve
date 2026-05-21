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

#ifndef SG_IO_DRIVER
#define SG_IO_DRIVER


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif





#include <QtCore/QList>


#include <SgConstants.h>
#include <SgLogger.h>
#include <SgMJD.h>




/*=====================================================================================================*/




/*=====================================================================================================*/
enum SpecDims
{
  SD_NumObs     =   -1,         //! 
  SD_NumScans   =   -2,         //! 
  SD_NumChans   =   -3,         //! 
  SD_NumStnPts  =   -4,         //! 
  SD_NumSrc     =   -5,         //! 
  SD_NumStn     =   -6,         //! 
  SD_2NumChans  =   -7,         //!
  SD_NumBands   =   -8,         //!
  SD_NumBln     =   -9,         //!
  SD_Any        = -200,         //! 
};




/***===================================================================================================*/
/**
 * Abstract IO driver 
 *
 */
/**====================================================================================================*/
class SgIdentities;
class SgVersion;
//
class SgIoDriver
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgIoDriver(const SgIdentities* ids, const SgVersion* dv);

  /**A destructor.
   * Destroys the object.
   */
  virtual ~SgIoDriver();


  //
  // Interfaces:
  //
  // gets:
  /**
   */
  inline const SgMJD& getDateOfCreation() const {return dateOfCreation_;};

  inline const SgIdentities* getCurrentIdentities() const {return currentIdentities_;};
  inline SgIdentities* getInputIdentities() const {return inputIdentities_;};
  inline const SgVersion* getCurrentDriverVersion() const {return currentDriverVersion_;};
  //inline SgVersion* getInputDriverVersion() const {return inputDriverVersion_;};



  // sets:
  inline void setDateOfCreation(const SgMJD& t) {dateOfCreation_ = t;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Returns list of files that contain data
   */
  virtual const QList<QString> listOfInputFiles() const = 0;



  //
  // Friends:
  //
  
protected:
  SgMJD                         dateOfCreation_;          //  "RunTimeTag 2014/02/10 08:28:53"
  // identities:
  const SgIdentities           *currentIdentities_;
  const SgVersion              *currentDriverVersion_;
  SgIdentities                 *inputIdentities_;
//  SgVersion                    *inputDriverVersion_;


};



/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/


/*=====================================================================================================*/
#endif     //SG_IO_DRIVER

