/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2026 Sergei Bolotin.
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

#ifndef SG_VLBI_SCAN_H
#define SG_VLBI_SCAN_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>


#include <QtCore/QList>
#include <QtCore/QString>


//#include <SgObservation.h>
//#include <SgTaskConfig.h>
//#include <SgMeteoData.h>
//#include <SgVector.h>




class SgVlbiSession;
class SgVlbiObservation;
/***===================================================================================================*/
/**
 * The class is representing observation-like data (meteo parameters, cable calibraiton, other 
 * measurements) for each station.
 *
 */
/**====================================================================================================*/
class SgVlbiScan
{
public:

  // Statics:
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgVlbiScan();

  inline SgVlbiScan(SgVlbiSession* session, const QString scheduleName);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiScan();



  //
  // Interfaces:
  //


  // gets:
  inline const SgVlbiSession* getSession() const {return session_;};
  inline const QList<SgVlbiObservation*>& getObservations() const {return observations_;};
  inline QList<SgVlbiObservation*>& observations() {return observations_;};
  inline const QString& getKey() const {return key_;};
  inline const QString& getScheduleName() const {return scheduleName_;};

  inline double getExtrPmX_Hf() const {return extrPmX_Hf_;};
  inline double getExtrPmY_Hf() const {return extrPmY_Hf_;};
  inline double getExtrPmX_Lf() const {return extrPmX_Lf_;};
  inline double getExtrPmY_Lf() const {return extrPmY_Lf_;};
  inline double getExtrUt1_Hf() const {return extrUt1_Hf_;};
  inline double getExtrUt1_Lf() const {return extrUt1_Lf_;};
  inline double getExtrCpX_Lf() const {return extrCpX_Lf_;};
  inline double getExtrCpY_Lf() const {return extrCpY_Lf_;};

  inline double getIntrPmX_Hf() const {return intrPmX_Hf_;};
  inline double getIntrPmY_Hf() const {return intrPmY_Hf_;};
  inline double getIntrPmX_Lf() const {return intrPmX_Lf_;};
  inline double getIntrPmY_Lf() const {return intrPmY_Lf_;};
  inline double getIntrUt1_Hf() const {return intrUt1_Hf_;};
  inline double getIntrUt1_Lf() const {return intrUt1_Lf_;};
  inline double getIntrCpX_Lf() const {return intrCpX_Lf_;};
  inline double getIntrCpY_Lf() const {return intrCpY_Lf_;};
    
  inline double getActlPmX_Hf() const {return actlPmX_Hf_;};
  inline double getActlPmY_Hf() const {return actlPmY_Hf_;};
  inline double getActlPmX_Lf() const {return actlPmX_Lf_;};
  inline double getActlPmY_Lf() const {return actlPmY_Lf_;};
  inline double getActlUt1_Hf() const {return actlUt1_Hf_;};
  inline double getActlUt1_Lf() const {return actlUt1_Lf_;};
  inline double getActlCpX_Lf() const {return actlCpX_Lf_;};
  inline double getActlCpY_Lf() const {return actlCpY_Lf_;};


  // sets:
  //
  inline void setSession(SgVlbiSession* session) {session_ = session;};
  inline void setKey(const QString& s) {key_ = s;};
  inline void setScheduleName(const QString& s) {scheduleName_ = s;};
 
  inline void setExtrPmX_Hf(double d) {extrPmX_Hf_ = d;};
  inline void setExtrPmY_Hf(double d) {extrPmY_Hf_ = d;};
  inline void setExtrPmX_Lf(double d) {extrPmX_Lf_ = d;};
  inline void setExtrPmY_Lf(double d) {extrPmY_Lf_ = d;};
  inline void setExtrUt1_Hf(double d) {extrUt1_Hf_ = d;};
  inline void setExtrUt1_Lf(double d) {extrUt1_Lf_ = d;};
  inline void setExtrCpX_Lf(double d) {extrCpX_Lf_ = d;};
  inline void setExtrCpY_Lf(double d) {extrCpY_Lf_ = d;};

  inline void setIntrPmX_Hf(double d) {intrPmX_Hf_ = d;};
  inline void setIntrPmY_Hf(double d) {intrPmY_Hf_ = d;};
  inline void setIntrPmX_Lf(double d) {intrPmX_Lf_ = d;};
  inline void setIntrPmY_Lf(double d) {intrPmY_Lf_ = d;};
  inline void setIntrUt1_Hf(double d) {intrUt1_Hf_ = d;};
  inline void setIntrUt1_Lf(double d) {intrUt1_Lf_ = d;};
  inline void setIntrCpX_Lf(double d) {intrCpX_Lf_ = d;};
  inline void setIntrCpY_Lf(double d) {intrCpY_Lf_ = d;};
    
  inline void setActlPmX_Hf(double d) {actlPmX_Hf_ = d;};
  inline void setActlPmY_Hf(double d) {actlPmY_Hf_ = d;};
  inline void setActlPmX_Lf(double d) {actlPmX_Lf_ = d;};
  inline void setActlPmY_Lf(double d) {actlPmY_Lf_ = d;};
  inline void setActlUt1_Hf(double d) {actlUt1_Hf_ = d;};
  inline void setActlUt1_Lf(double d) {actlUt1_Lf_ = d;};
  inline void setActlCpX_Lf(double d) {actlCpX_Lf_ = d;};
  inline void setActlCpY_Lf(double d) {actlCpY_Lf_ = d;};



  //
  // Functions:
  //
  static const QString className();

  inline double getAprioriPmX() const {return actlPmX_Hf_ + actlPmX_Lf_;};
  inline double getAprioriPmY() const {return actlPmY_Hf_ + actlPmY_Lf_;};
  inline double getAprioriUt1() const {return actlUt1_Hf_ + actlUt1_Lf_;};
  inline double getAprioriCpX() const {return actlCpX_Lf_;};
  inline double getAprioriCpY() const {return actlCpY_Lf_;};

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  SgVlbiSession                *session_;
  QList<SgVlbiObservation*>     observations_;
  QString                       key_;
  QString                       scheduleName_;
  
  // for spoolfile output:
  // external values of EOP:
  double                        extrPmX_Hf_;
  double                        extrPmY_Hf_;
  double                        extrPmX_Lf_;
  double                        extrPmY_Lf_;
  double                        extrUt1_Hf_;
  double                        extrUt1_Lf_;
  double                        extrCpX_Lf_;
  double                        extrCpY_Lf_;

  // internal (database) values of EOP:
  double                        intrPmX_Hf_;
  double                        intrPmY_Hf_;
  double                        intrPmX_Lf_;
  double                        intrPmY_Lf_;
  double                        intrUt1_Hf_;
  double                        intrUt1_Lf_;
  double                        intrCpX_Lf_;
  double                        intrCpY_Lf_;
    
  // applied a priori values of EOP:
  double                        actlPmX_Hf_;
  double                        actlPmY_Hf_;
  double                        actlPmX_Lf_;
  double                        actlPmY_Lf_;
  double                        actlUt1_Hf_;
  double                        actlUt1_Lf_;
  double                        actlCpX_Lf_;
  double                        actlCpY_Lf_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObservation inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//


// A destructor:
inline SgVlbiScan::~SgVlbiScan()
{
};



inline SgVlbiScan::SgVlbiScan(SgVlbiSession* session, const QString scheduleName) :
  scheduleName_(scheduleName)
{
  session_ = session;  

  extrPmX_Hf_ = extrPmY_Hf_ = extrPmX_Lf_ = extrPmY_Lf_ = extrUt1_Hf_ = extrUt1_Lf_ = extrCpX_Lf_ =
  extrCpY_Lf_ =
  intrPmX_Hf_ = intrPmY_Hf_ = intrPmX_Lf_ = intrPmY_Lf_ = intrUt1_Hf_ = intrUt1_Lf_ = intrCpX_Lf_ =
  intrCpY_Lf_ =
  actlPmX_Hf_ = actlPmY_Hf_ = actlPmX_Lf_ = actlPmY_Lf_ = actlUt1_Hf_ = actlUt1_Lf_ = actlCpX_Lf_ =
  actlCpY_Lf_ = 0.0;
};



//
// FUNCTIONS:
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
#endif // SG_VLBI_SCAN_H
