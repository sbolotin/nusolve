/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2013-2020 Sergei Bolotin.
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

#ifndef SG_PARAMETER_BREAK
#define SG_PARAMETER_BREAK


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>


#include <SgAttribute.h>
#include <SgMJD.h>



class QDataStream;
class SgParameter;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgParameterBreak : public SgMJD, public SgAttribute
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<<0, //!< do not use this break;
    Attr_MODIFIED           = 1<<1, //!< parameters were modified;
    Attr_DYNAMIC            = 1<<2, //!< parameters supposed to be estimated during the common solution;
  };

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgParameterBreak();

  /**A constructor.
   * Creates an object.
   */
  SgParameterBreak(const SgParameterBreak&);

  /**A constructor.
   * Creates an object.
   */
  SgParameterBreak(const SgMJD& t, double a0=0.0, double a1=0.0, double a2=0.0, bool isDynamic=false);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgParameterBreak();


  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgParameterBreak& operator=(const SgParameterBreak& b);
  
  /**Returns a value of the shift.
   */
  inline double getA0() const;

  /**Returns a value of the change in trend.
   */
  inline double getA1() const;

  /**Returns a value of the change in acceleration.
   */
  inline double getA2() const;

  /**Returns a std.var. of the shift.
   */
  inline double getS0() const;

  /**Returns a std.var. of the change in trend.
   */
  inline double getS1() const;

  /**Returns a std.var. of the change in acceleration.
   */
  inline double getS2() const;

  /**Returns a value of the epoch for export.
   */
  inline const SgMJD& getEpoch4Export() const;
  
  /**Sets a value of new shift.
   * \param a -- new shift;
   */
  inline void setA0(double a);

  /**Sets a value of new change in trend.
   * \param a -- new trend;
   */
  inline void setA1(double a);

  /**Sets a value of new change in acceleration.
   * \param a -- new acceleration;
   */
  inline void setA2(double a);

  /**Sets a std.var. of new shift.
   * \param a -- new shift;
   */
  inline void setS0(double a);

  /**Sets a std.var. of new change in trend.
   * \param a -- new trend;
   */
  inline void setS1(double a);

  /**Sets a std.var. of new change in acceleration.
   * \param a -- new acceleration;
   */
  inline void setS2(double a);

  /**Sets a value of new export epoch.
   * \param t -- new epoch;
   */
  inline void setEpoch4Export(const SgMJD&);

  inline SgParameter* pA0() {return pA0_;};
  inline SgParameter* pA1() {return pA1_;};
  inline SgParameter* pA2() {return pA2_;};
  inline const SgParameter* pA0() const {return pA0_;};
  inline const SgParameter* pA1() const {return pA1_;};
  inline const SgParameter* pA2() const {return pA2_;};

  //
  // Functions:
  //
  /**Returns a value of parameter break function.
   * \param  t -- current epoch;
   * \param dT -- interval passed from an arbitrary zero epoch;
   */
  inline double value(const SgMJD& t, double dT) const;
  inline double rate (const SgMJD& t, double dT) const;

  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters(const QString& prefix);

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();

  /**Fills partials for the parameters.
   */
  void propagatePartials(QList<SgParameter*>& parameters, const SgMJD& t, double tau, double sign);
  void propagatePartials4rates(QList<SgParameter*>& parameters, const SgMJD& t, double tau, double sign);


  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //


private:
  double                        a0_;            // shift
  double                        a1_;            // changes in rate
  double                        a2_;            // and acceleration
  double                        s0_;            // sigma of shift
  double                        s1_;            // sigma of changes in rate
  double                        s2_;            // sigma of and acceleration
  SgMJD                         epoch4Export_;  // an export epoch, should be understandable by solve
  SgParameter                  *pA0_;
  SgParameter                  *pA1_;
  SgParameter                  *pA2_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgBreakModel : public QList<SgParameterBreak*>
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgBreakModel();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgBreakModel();


  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgBreakModel& operator=(const SgBreakModel& m);

  /**Returns a value of zero epoch.
   */
  inline const SgMJD& getT0() const;

  /**Sets up the zero epoch.
   * \param t -- new zero epoch;
   */
  inline void setT0(const SgMJD& t);

  //
  // Functions:
  //
  double value(const SgMJD& t) const;
  double rate (const SgMJD& t) const;

  /**Adds a break to the model. Returns true in success (or false, if the epoch already exists).
   * \param t -- epoch of the break;
   * \param a0 -- shift;
   * \param a1 -- trend;
   * \param a2 -- acceleration;
   */
  bool addBreak(const SgMJD& t, double a0=0.0, double a1=0.0, double a2=0.0, bool isDynamic=false);

  /**Adds a break to the model. Returns true in success (or false, if the epoch already exists).
   * WARNING: the model will b eresponsible for the pointer, do not delete it after this call.
   * \param aBreak -- new break;
   */
  bool addBreak(SgParameterBreak* aBreak);

  /**Deletes a break at the specific epoch.
   * \param t -- the epoch;
   */
  bool delBreak(const SgMJD& t);

  /**Deletes a break under the index #n (counting from zero).
   * \param n -- the index;
   */
  bool delBreak(int n);

  inline void resetAllEditings();

  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters(const QString& prefix);

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();

  /**Fills partials for the parameters.
   */
  void propagatePartials(QList<SgParameter*>& parameters, const SgMJD& t, double tau, double sign);
  void propagatePartials4rates(QList<SgParameter*>& parameters, const SgMJD& t, double tau, double sign);

  void sortEvents();

  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //
  

private:
  SgMJD                         t0_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgParameterBreak inline members:                                                                    */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgParameterBreak::SgParameterBreak() : SgMJD(tInf), SgAttribute(), epoch4Export_(tInf)
{
  a0_ = 0.0;
  a1_ = 0.0;
  a2_ = 0.0;
  s0_ = 0.0;
  s1_ = 0.0;
  s2_ = 0.0;
  pA0_ = pA1_ = pA2_ = NULL;
};






//
// INTERFACES:
//
//
//
inline double SgParameterBreak::getA0() const
{
  return a0_;
};



//
inline double SgParameterBreak::getA1() const
{
  return a1_;
};



//
inline double SgParameterBreak::getA2() const
{
  return a2_;
};



//
inline double SgParameterBreak::getS0() const
{
  return s0_;
};



//
inline double SgParameterBreak::getS1() const
{
  return s1_;
};



//
inline double SgParameterBreak::getS2() const
{
  return s2_;
};



//
inline const SgMJD& SgParameterBreak::getEpoch4Export() const
{
  return epoch4Export_;
};



//
inline void SgParameterBreak::setA0(double a)
{
  a0_ = a;
};



//
inline void SgParameterBreak::setA1(double a)
{
  a1_ = a;
};



//
inline void SgParameterBreak::setA2(double a)
{
  a2_ = a;
};



//
inline void SgParameterBreak::setS0(double s)
{
  s0_ = s;
};



//
inline void SgParameterBreak::setS1(double s)
{
  s1_ = s;
};



//
inline void SgParameterBreak::setS2(double s)
{
  s2_ = s;
};



//
inline void SgParameterBreak::setEpoch4Export(const SgMJD& t)
{
  epoch4Export_ = t;
};



//
// FUNCTIONS:
//
//
//
inline double SgParameterBreak::value(const SgMJD& t, double dT) const
{
  return isAttr(SgParameterBreak::Attr_DYNAMIC) || t<*this ? 0.0 : a0_ + a1_*dT + a2_*dT*dT;
};



//
inline double SgParameterBreak::rate(const SgMJD& t, double dT) const
{
  return isAttr(SgParameterBreak::Attr_DYNAMIC) || t<*this ? 0.0 : a1_ + 2.0*a2_*dT;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgBreakModel inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgBreakModel::SgBreakModel()
  : QList<SgParameterBreak*>()
{
  t0_ = tZero;
};



// A destructor:
inline SgBreakModel::~SgBreakModel()
{
  for (int i=0; i<size(); i++)
    delete at(i);
  clear();
};



//
// INTERFACES:
//
//
//
inline const SgMJD& SgBreakModel::getT0() const
{
  return t0_;
};



//
inline void SgBreakModel::setT0(const SgMJD& t)
{
  t0_ = t;
};



//
inline void SgBreakModel::resetAllEditings()
{
  for (int i=0; i<size(); i++)
    delete at(i);
  clear();
};



//
// FUNCTIONS:
//
//
//
//
/*=====================================================================================================*/







/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif //SG_PARAMETER_BREAK
