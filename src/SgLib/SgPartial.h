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

#ifndef SG_EST_PARTIAL_H
#define SG_EST_PARTIAL_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <QtCore/QString>

#include <SgAttribute.h>
#include <SgMJD.h>




/***===================================================================================================*/
/**
 * SgPartial -- a top part of SgParameter that estimator needs to know.
 *
 */
/**====================================================================================================*/
class SgPartial : public SgAttribute
{
public:
  enum Attributes
  {
    Attr_IS_BREAK       = 1<<0,       //!< there is a break in the parameter's behavior
    Attr_IS_PROPAGATED  = 1<<1,       //!< the parameter's info have to be propagated in future
    Attr_IS_IN_RUN      = 1<<2,       //!< 
    Attr_IS_PARTIAL_SET = 1<<3,       //!< rise this bit on when the partial is assigned
    Attr_IS_SPECIAL     = 1<<4,       //!< for estimator use
  };
  enum DataDomain
  {
    DD_OFFSET                   = 0,
    DD_RATE                     = 1,
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgPartial();

  /**A constructor.
  * Creates a copy of an object.
  * \param a -- a attribute to be copied.
  */
  inline SgPartial(const SgPartial& p);

  /**A constructor.
  * Creates a defult object with a name.
  * \param mname -- a name.
  */
  inline SgPartial(const QString& name);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgPartial();

  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgPartial& operator=(const SgPartial& a);

  /**Returns a name of the derivative.
   */
  inline const QString& getName() const;

  inline DataDomain getDataDomain() const {return dataDomain_;};

  /**Returns a value of derivative.
   */
  inline double getD() const;

  /**Returns a number of observations.
   */
  inline int getNumObs() const;

  /**Returns a number of sessions.
   */
  inline int getNumSes() const;

  /**Returns an epoch of the first observation.
   */
  inline const SgMJD& getTStart() const;

  /**Returns an epoch of the last observation.
   */
  inline const SgMJD& getTFinis() const;

  /**Returns an epoch of the current observation.
   */
  inline const SgMJD& getTCurrent() const;
  
  /**Sets up a name of the derivative.
   * \param name -- a name;
   */
  inline void setName(const QString& name);

  /**Sets a value of partial derivative.
   * \param d -- a new value;
   */
  inline void setD(double d);

  inline void setDataDomain(DataDomain domain) {dataDomain_=domain;};

  /**Sets a number of observations.
   * \param n -- a number of observations;
   */
  inline void setNumObs(int n);

  /**Sets a number of sessions.
   * \param n -- a number of sessions;
   */
  inline void setNumSes(int n);

  /**Sets up an epoch of the first observation.
   * \param t -- a new epoch;
   */
  inline void setTStart(const SgMJD& t);

  /**Sets up an epoch of the last observation.
   * \param t -- a new epoch;
   */
  inline void setTFinis(const SgMJD& t);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Resets current statistics.
   */
  inline void resetStatistics();
  
  /**Updates current statistics.
   * \param t -- an epoch of the observations;
   * \param weight -- a weight of the observations;
   */
  void updateStatistics(const SgMJD& t, double weight);
  
  /**Sets the value of the derivation to zero.
   */
  inline void  zerofy();

  /**Returns weighted mean epoch of the processed partial.
   */
  inline SgMJD getTMean(const SgMJD& =tZero) const;

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  QString       name_;                  //!< name of a derivation;
  DataDomain    dataDomain_;
  double        d_;                     //!< value of a derivation;
  int           numObs_;                //!< number of points per derivation;
  int           numSes_;                //!< number of sessions per derivation;
  SgMJD         tStart_;                //!< MJD of the first measurement;
  SgMJD         tFinis_;                //!< MJD of the last measurement;
  double        twSum_;                 //!< the weighted sum of epochs of observation;
  double        wSum_;                  //!< the sum of weights of observations;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgPartial inline members:                                                                           */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgPartial::SgPartial() : 
  SgAttribute(),
  name_("unnamed"),
  tStart_(tZero),
  tFinis_(tZero)
{
  dataDomain_ = DD_OFFSET;
  d_ = 0.0;
  numObs_ = 0;
  numSes_ = 0;
  twSum_ = 0.0;
  wSum_ = 0.0;
};



// An empty constructor that set up a name:
inline SgPartial::SgPartial(const QString& name) : 
  SgAttribute(),
  name_(name),
  tStart_(tZero),
  tFinis_(tZero)
{
  dataDomain_ = DD_OFFSET;
  d_ = 0.0;
  numObs_ = 0;
  numSes_ = 0;
  twSum_ = 0.0;
  wSum_ = 0.0;
};



// A regular constructor:
inline SgPartial::SgPartial(const SgPartial& p) : 
  SgAttribute(p),
  name_(p.getName()),
  tStart_(p.getTStart()),
  tFinis_(p.getTFinis())
{
  dataDomain_ = p.getDataDomain();
  d_ = p.getD();
  numObs_ = p.getNumObs();
  numSes_ = p.getNumSes();
  twSum_ = p.twSum_;
  wSum_ = p.wSum_;
};




// A destructor:
inline SgPartial::~SgPartial()
{
  // nothing to do
};



//
// INTERFACES:
//
//
//
inline const QString& SgPartial::getName() const
{
  return name_;
};



//
inline double SgPartial::getD() const
{
  return d_;
};



//
inline int SgPartial::getNumObs() const
{
  return numObs_;
};



//
inline int SgPartial::getNumSes() const
{
  return numSes_;
};



//
inline const SgMJD& SgPartial::getTStart() const
{
  return tStart_;
};



//
inline const SgMJD& SgPartial::getTFinis() const
{
  return tFinis_;
};



//
inline const SgMJD& SgPartial::getTCurrent() const
{
  return tFinis_;
};



//
inline void SgPartial::setName(const QString& name)
{
  name_ = name;
};



//
inline void SgPartial::setD(double d)
{
  d_ = d;
  addAttr(Attr_IS_PARTIAL_SET);
};



//
inline void SgPartial::setNumObs(int n)
{
  numObs_ = n;
};



//
inline void SgPartial::setNumSes(int n)
{
  numSes_ = n;
};



//
inline void SgPartial::setTStart(const SgMJD& t)
{
  tStart_ = t;
};



//
inline void SgPartial::setTFinis(const SgMJD& t)
{
  tFinis_ = t;
};






//
// FRUNCTIONS:
//
//
//
inline void SgPartial::resetStatistics()
{
  numObs_ = 0;
  numSes_ = 0;
  tStart_ = tZero;
  tFinis_ = tZero;
  twSum_ = 0.0;
  wSum_ = 0.0;
};



//
inline void SgPartial::zerofy()
{
  d_ = 0.0;
  delAttr(Attr_IS_PARTIAL_SET);
};



inline SgMJD SgPartial::getTMean(const SgMJD&) const
{
  return (numObs_ && wSum_>0.0) ? SgMJD(twSum_/wSum_) : tZero;
};



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
#endif //SG_EST_PARTIAL_H
