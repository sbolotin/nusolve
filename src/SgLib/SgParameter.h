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

#ifndef SG_EST_PARAMETER_H
#define SG_EST_PARAMETER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgPartial.h>
#include <SgParameterCfg.h>
#include <SgVector.h>
#include <SgSymMatrix.h>



/***===================================================================================================*/
/**
 * SgParamater -- a parameter to estimate.
 *
 */
/**====================================================================================================*/
class SgParameter : public SgPartial
{
public:
  enum Attributes
  {
    // Attr_IS_BREAK       = 1<<0,       //!< there is a break in the parameter's behavior
    // Attr_IS_PROPAGATED  = 1<<1,       //!< the parameter's info have to be propagated in future
    // Attr_IS_IN_RUN      = 1<<2,       //!< 
    // Attr_IS_PARTIAL_SET = 1<<3,       //!< rise this bit on when the partial is assigned
    // Attr_IS_SPECIAL     = 1<<4,       //!< for estimator use
    Attr_IS_SOLVED         = 1<<5,      //!< for estimator use
    Attr_AUX_FLAG          = 1<<6,      //!< for external use
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgParameter();

  /**A constructor.
  * Creates a copy of an object.
  * \param a -- a attribute to be copied.
  */
  inline SgParameter(const SgParameter& p);

  /**A constructor.
  * Creates a default object with a name.
  * \param name -- a name.
  */
  inline SgParameter(const QString& name);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgParameter();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgParameter& operator=(const SgParameter& a);

  /**Returns a value of the parameter.
   */
  inline double getValue() const;

  /**Returns a latest solution for the parameter.
   */
  inline double getSolution() const;
  
  /**Returns a std.derivation for the parameter.
   */
  inline double getSigma() const;
  
  /**Returns a scale for the parameter.
   */
  inline double getScale() const;

  /**Returns a mode for parameter estimation.
   */
  inline SgParameterCfg::PMode getPMode() const;

  /**Returns a type of stochastic parameter (means only for stochastic parameters).
   */
  inline SgParameterCfg::SType getSType() const;

  /**Returns an a priori sigma for the parameter.
   */
  inline double getSigmaAPriori() const;

  /**Returns an a priori sigma for the parameter.
   */
  inline double getSigmaAPrioriAux() const;

  /**Returns a time constant for the parameter (means only for stochastic parameters).
   */
  inline double getTau() const;

  /**Returns a PSD for ruled white noise for the parameter (means only for stochastic parameters).
   */
  inline double getPsd_RWN() const;

  /**Returns a PSD for break event for the parameter (means only for stochastic parameters).
   */
  inline double getPsd_BN() const;

  /**Returns a prefix for the parameter.
   */
  inline const QString& getPrefix() const;

  /**Returns a value of the step for modelling by picewise linear function.
   */
  inline double getStep() const;

  /**Returns a value of the time-to-live (days).
   */
  inline double getTTL() const;

  /**Returns a value of a number of polinomials.
   */
  inline int getNumOfPolynomials() const;

  /**
   */
  inline SgMJD getTLeft() const;

  /**
   */
  inline SgMJD getTRight() const;

  /**
   */
  inline int getIdx() const;

  //
  /**Sets up a value of the parameter.
   * \param v -- a value;
   */
  inline void setValue(double v);
  
  /**Sets up a latest solution of the parameter.
   * \param v -- a solution;
   */
  inline void setSolution(double v);
  
  /**Sets up a std.derivation for the parameter.
   * \param v -- a sigma;
   */
  inline void setSigma(double v);
  
  /**Sets up a scale for the parameter.
   * \param v -- a scale;
   */
  inline void setScale(double v);

  /**Sets up a mode for parameter estimation.
   * \param mode -- a new mode;
   */
  inline void setPMode(SgParameterCfg::PMode mode);

  /**Sets up a type of stochastic parameter (means only for stochastic parameters).
   * \param type -- a new type;
   */
  inline void setSType(SgParameterCfg::SType type);

  /**Sets up an a priori sigma for the parameter.
   * \param v -- an a priori;
   */
  inline void setSigmaAPriori(double v);

  /**Sets up an a priori sigma for the parameter.
   * \param v -- an a priori;
   */
  inline void setSigmaAPrioriAux(double v);

  /**Sets up a time constant for the parameter (means only for stochastic parameters).
   * \param v -- a new tau;
   */
  inline void setTau(double v);

  /**Sets up a PSD for ruled white noise for the parameter (means only for stochastic parameters).
   * \param v -- a new PSD;
   */
  inline void setPsd_RWN(double v);

  /**Sets up a PSD for break event for the parameter (means only for stochastic parameters).
   * \param v -- a new PSD;
   */
  inline void setPsd_BN(double v);

  /**Sets up a prefix for the parameter.
   * \param prefix -- a prefix;
   */
  inline void setPrefix(const QString& prefix);

  /**Sets up a value of the step for modelling by picewise linear function.
   * \param s -- new step;
   */
  inline void setStep(double step);

  /**Sets up a value of the time-to-live (days).
   */
  inline void setTTL(double ttl);

  /**Set up a value of a number of polinomials.
   */
  inline void setNumOfPolynomials(int);

  /**
   */
  inline void setTLeft(const SgMJD& t);

  /**
   */
  inline void setTRight(const SgMJD& t);

  /**
   */
  inline void setIdx(int idx);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Sets up parameter's configuration.
   */
  void tune(const SgParameterCfg&);

  /**Sets up parameter's configuration.
   */
  void tune(SgParameterCfg::PMode, const SgParameterCfg&);

  /**Sets up parameter's configuration.
   */
  void tune(const SgParameter&);
  
  /**Clears values.
   */
  inline void resetStatistics();

  inline void decreaseTTL(double);

  inline void resetTTL();

  /**Updates values from the solution.
   */
  void update(SgParameter* p);

  double calcM(double);

  double calcRW(double);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  double                value_;                 //!< the value of adjustment process (cumulative);
  double                solution_;              //!< a solution from latest estimation;
  double                sigma_;                 //!< a sigma of the latest estimation;
  double                scale_;                 //!< a scale factor;

  SgParameterCfg::PMode pMode_;
  SgParameterCfg::SType sType_;
  double                sigmaAPriori_;          //!< a priori std. deviation (to init the Inform. matrix)
  double                sigmaAPrioriAux_;       //!< a priori std. deviation (to init the Inform. matrix)
  double                tau_;                   //!< time of relaxation (for stochastic parameters)
  double                psd_RWN_;               //!< PSD of Ruled White Noise
  double                psd_BN_;                //!< PSD of White Noise used in break
  QString               prefix_;
  double                step_;
  double                ttl_;
  int                   numOfPolynomials_;      //!< number of polinomials for a model
  //-- experimental:
  SgMJD                 tLeft_;
  SgMJD                 tRight_;
  int                   idx_;                   //!< an index in common list of parameters
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgParamater inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgParameter::SgParameter() : 
  SgPartial(),
  tLeft_(tZero),
  tRight_(tInf)
{
  value_ = 0.0;
  solution_ = 0.0;
  sigma_ = 1.0;
  scale_ = 1.0;
  pMode_ = SgParameterCfg::PM_NONE;
  sType_ = SgParameterCfg::ST_RANDWALK;
  sigmaAPriori_ = 1.0;
  sigmaAPrioriAux_ = 1.0;
  tau_ = 0.0;
  psd_RWN_ = 1.0;
  psd_BN_  = 1.0;
  prefix_ = " ";
  step_ = 1.0;
  ttl_ = 36525.0;
  numOfPolynomials_ = 0;
  idx_ = -1;
};



// An empty constructor, just set up a name:
inline SgParameter::SgParameter(const QString& name) : 
  SgPartial(name),
  tLeft_(tZero),
  tRight_(tInf)
{
  value_ = 0.0;
  solution_ = 0.0;
  sigma_ = 1.0;
  scale_ = 1.0;
  pMode_ = SgParameterCfg::PM_NONE;
  sType_ = SgParameterCfg::ST_RANDWALK;
  sigmaAPriori_ = 1.0;
  sigmaAPrioriAux_ = 1.0;
  tau_ = 0.0;
  psd_RWN_ = 1.0;
  psd_BN_  = 1.0;
  prefix_ = " ";
  step_ = 1.0;
  ttl_ = 36525.0;
  numOfPolynomials_ = 0;
  idx_ = -1;
};



// A regular constructor:
inline SgParameter::SgParameter(const SgParameter& p) : 
  SgPartial(p),
  tLeft_(p.getTLeft()),
  tRight_(p.getTRight())
{
  value_ = p.getValue();
  solution_ = p.getSolution();
  sigma_ = p.getSigma();
  scale_ = p.getScale();
  pMode_ = p.getPMode();
  sType_ = p.getSType();
  sigmaAPriori_ = p.getSigmaAPriori();
  sigmaAPrioriAux_ = p.getSigmaAPrioriAux();
  tau_ = p.getTau();
  psd_RWN_ = p.getPsd_RWN();
  psd_BN_  = p.getPsd_BN();
  prefix_ = p.getPrefix();
  step_ = p.getStep();
  ttl_ = p.getTTL();
  numOfPolynomials_ = p.getNumOfPolynomials();
  idx_ = p.getIdx();
};



// A destructor:
inline SgParameter::~SgParameter()
{
  // nothing to do
};



//
// INTERFACES:
//
//
//
inline double SgParameter::getValue() const
{
  return value_;
};



//
inline double SgParameter::getSolution() const
{
  return solution_;
};



//  
inline double SgParameter::getSigma() const
{
  return sigma_;
};



//  
inline double SgParameter::getScale() const
{
  return scale_;
};



//  
inline SgParameterCfg::PMode SgParameter::getPMode() const
{
  return pMode_;
};



//  
inline SgParameterCfg::SType SgParameter::getSType() const
{
  return sType_;
};



//  
inline double SgParameter::getSigmaAPriori() const
{
  return sigmaAPriori_;
};



//  
inline double SgParameter::getSigmaAPrioriAux() const
{
  return sigmaAPrioriAux_;
};



//  
inline double SgParameter::getTau() const
{
  return tau_;
};



//  
inline double SgParameter::getPsd_RWN() const
{
  return psd_RWN_;
};



//  
inline double SgParameter::getPsd_BN() const
{
  return psd_BN_;
};



//  
inline const QString& SgParameter::getPrefix() const
{
  return prefix_;
};



//
inline double SgParameter::getStep() const
{
  return step_;
};



//
inline double SgParameter::getTTL() const
{
  return ttl_;
};



//
inline int SgParameter::getNumOfPolynomials() const
{
  return numOfPolynomials_;
};



//
inline SgMJD SgParameter::getTLeft() const
{
  return tLeft_;
};



//
inline SgMJD SgParameter::getTRight() const
{
  return tRight_;
};



//
inline int SgParameter::getIdx() const
{
  return idx_;
};



//
inline void SgParameter::setValue(double v)
{
  value_ = v;
};



//
inline void SgParameter::setSolution(double v)
{
  solution_ = v;
};



//
inline void SgParameter::setSigma(double v)
{
  sigma_ = v;
};



//
inline void SgParameter::setScale(double v)
{
  scale_ = v;
};



//
inline void SgParameter::setPMode(SgParameterCfg::PMode mode)
{
  pMode_ = mode;
};



//
inline void SgParameter::setSType(SgParameterCfg::SType type)
{
  sType_ = type;
};



//
inline void SgParameter::setSigmaAPriori(double v)
{
  sigmaAPriori_ = v;
};



//
inline void SgParameter::setSigmaAPrioriAux(double v)
{
  sigmaAPrioriAux_ = v;
};



//
inline void SgParameter::setTau(double v)
{
  tau_ = v;
};



//
inline void SgParameter::setPsd_RWN(double v)
{
  psd_RWN_ = v;
};



//
inline void SgParameter::setPsd_BN(double v)
{
  psd_BN_ = v;
};



//
inline void SgParameter::setPrefix(const QString& prefix)
{
  prefix_ = prefix;
};



//
inline void SgParameter::setStep(double s)
{
  step_ = s;
};



//
inline void SgParameter::setTTL(double ttl)
{
  ttl_ = ttl;
};



//
inline void SgParameter::setNumOfPolynomials(int n)
{
  numOfPolynomials_ = n;
};



//
inline void SgParameter::setTLeft(const SgMJD& t)
{
  tLeft_ = t;
};



//
inline void SgParameter::setTRight(const SgMJD& t)
{
  tRight_ = t;
};



//
inline void SgParameter::setIdx(int idx)
{
  idx_ = idx;
};



//
// FUNCTIONS:
//
//
//
inline void SgParameter::resetStatistics()
{
  SgPartial::resetStatistics();
  value_ = solution_ = 0.0;
  sigma_ = 1.0;
};



//
inline void SgParameter::decreaseTTL(double dt)
{
  ttl_ -= dt;
};



//
inline void SgParameter::resetTTL()
{
  while (ttl_<=0.0)
    ttl_ += step_;
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
//
bool lessThan4_ParameterByNameSortingOrder(SgParameter*, SgParameter*);

//
bool lessThan4_ParameterByTTLSortingOrder(SgParameter*, SgParameter*);

//
bool lessThan4_ParameterByTLeftSortingOrder(SgParameter*, SgParameter*);

//
bool lessThan4_ParameterByTRightSortingOrder(SgParameter*, SgParameter*);

//
void updateSolutionAtParameterList(QList<SgParameter*>&, SgVector*, SgSymMatrix*);

//
void reportParameterList(const QList<SgParameter*>&, bool=false);

//
//void clearStatisticsForParameters(QList<SgParameter*>&);
/*=====================================================================================================*/
#endif //SG_EST_PARAMETER_H
