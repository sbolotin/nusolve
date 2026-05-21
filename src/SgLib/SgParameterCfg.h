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


#ifndef SG_EST_PARAMETER_CFG_H
#define SG_EST_PARAMETER_CFG_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

class QDataStream;

#include <SgAttribute.h>




/***===================================================================================================*/
/**
 * SgParamaterCfg -- a configuration for estimated parameter.
 *
 */
/**====================================================================================================*/
class SgParameterCfg : public SgAttribute
{
public:
  enum PMode    {PM_NONE=0, PM_GLB=1, PM_ARC=2, PM_LOC=3, PM_PWL=4, PM_STC=5};
  enum SType    {ST_WHITENOISE=0, ST_MARKOVPROCESS=1, ST_RANDWALK=2};
  enum Attributes
  {
    Attr_IS_PROPAGATED  = 1<<0,       //!< propagate the parameter in time;
    Attr_IS_CONSTRAINED = 1<<1,       //!< the parameter needs in constrains;
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgParameterCfg();

  /**A constructor.
   * Creates a object.
   */
  SgParameterCfg(const QString& name, PMode pMode, SType sType, double convAPriori, double arcStep,
                  double stocAPriori, double breakNoise, double tau, double whiteNoise,
                  double pwlStep, double pwlAPriori, int pwlNumOfPolynomials,
                  double scale = 1.0, const QString& scaleName = "unscaled");

  /**A copy constructor.
  * Creates a copy of an object.
  * \param a -- a attribute to be copied.
  */
  inline SgParameterCfg(const SgParameterCfg& p);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgParameterCfg();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgParameterCfg& operator=(const SgParameterCfg& a);

  /**Returns a name of the parameter configurator.
   */
  inline const QString& getName() const;

  /**Returns a mode of the parameter.
   */
  inline PMode getPMode() const;

  /**Returns a type of the parameter.
   */
  inline SType getSType() const;

  /**Returns a priori sigma for this type of parameters.
   */
  inline double getConvAPriori() const;

  /**Returns a priori sigma for this type of parameters in stochastic case.
   */
  inline double getStocAPriori() const;

  /**Returns a priori sigma for B-coeffs of PWL function.
   */
  inline double getPwlAPriori() const;

  /**Returns a value of a ruled white noise for a break for this type of parameters.
   */
  inline double getBreakNoise() const;

  /**Returns a tau for this type of parameters.
   */
  inline double getTau() const;

  /**Returns a density of ruled white noise for this type of parameters.
   */
  inline double getWhiteNoise() const;

  /**Returns a step for modelling by picewise linear function.
   */
  inline double getPwlStep() const;
  
  /**Returns a number of polinomials in the model of picewise linear function.
   */
  inline int getPwlNumOfPolynomials() const;
  
  /**Returns a step for arc parameter.
   */
  inline double getArcStep() const;

  /**Returns a scale for this type of parameters.
   */
  inline double getScale() const;

  /**Returns a name of scale (i.e., units) for this type of parameters.
   */
  inline const QString& getScaleName() const;

  /**Sets up a name for this type of parameters.
   * \param name -- new name;
   */
  inline void setName(const QString&);

  /**Sets up a mode for this type of parameters.
   * \param mode -- a mode;
   */
  inline void setPMode(PMode mode);

  /**Sets up a type for this type of parameters.
   * \param type -- a type;
   */
  inline void setSType(SType type);

  /**Sets up an a priori noise for this type of parameters (in conventional case).
   * \param v -- a value;
   */
  inline void setConvAPriori(double);

  /**Sets up an a priori noise for this type of parameters (in stochastic case).
   * \param v -- a value;
   */
  inline void setStocAPriori(double);

  /**Sets up an a priori noise for this B-coeffs of PWL functions.
   * \param v -- a value;
   */
  inline void setPwlAPriori(double);

  /**Sets up a value of noise that will be injected in the case of break for this type of parameters.
   * \param v -- a value;
   */
  inline void setBreakNoise(double v);

  /**Sets up a relaxation time for this type of parameters.
   * \param tau -- a new value;
   */
  inline void setTau(double tau);

  /**Sets up a value of the density of ruled white noise for this type of parameters.
   * \param w -- a new value;
   */
  inline void setWhiteNoise(double w);

  /**Sets up a value of the step for modelling by picewise linear function.
   * \param s -- a new value;
   */
  inline void setPwlStep(double s);

  /**Sets up a number of polinomials in the model of picewise linear function.
   * \param n -- a new value;
   */
  inline void setPwlNumOfPolynomials(int n);

  /**Sets up a value of the step for arc parameter.
   * \param s -- a new value;
   */
  inline void setArcStep(double s);

  /**Sets up a scale factor for this type of parameters.
   * \param s -- a scale;
   */
  inline void setScale(double s);

  /**Sets up a scale name for this type of parameters.
   * \param units -- new scale name;
   */
  inline void setScaleName(const QString& units);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  static const QString& type2String(SType);
  static const QString& mode2String(PMode);
  
  

  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //

private:
  QString       name_;
  PMode         pMode_;
  SType         sType_;
  double        scale_;               //!< Value of the scale factor
  QString       scaleName_;           //!< Name of the scale factor.
  // conventional parameter properties:
  double        convAPriori_;         //!< a priori std. deviation (conventional pars)
  double        arcStep_;             //!< a step for arc parameter
  // piecewise-linear parameter properties:
  double        pwlAPriori_;          //!< a priori std. deviation (for pisewice linear B-coeffs)
  double        pwlStep_;             //!< a step for modelling by picewise linear function
  int           pwlNumOfPolynomials_; //!< number of polynomials in the PWL model
  // stochastic parameter properties:
  double        stocAPriori_;         //!< a priori std. deviation (stochastic pars)
  double        breakNoise_;          //!< psd of noise process used for "breaks"
  double        tau_;                 //!< time of relaxation
  double        whiteNoise_;          //!< psd of ruled white noise
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgParameterCfg inline members:                                                                      */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgParameterCfg::SgParameterCfg() : 
  SgAttribute()
{
  name_ = "a parameter";
  pMode_= PM_NONE;
  sType_= ST_RANDWALK;
  scale_ = 1.0;
  scaleName_ = "units";
  //
  convAPriori_ = 1.0;
  arcStep_ = 7.0; // one week [days]
  //
  pwlAPriori_ = 1.0;
  pwlStep_ = 1.0/24.0; // 1hr [days]
  pwlNumOfPolynomials_ = 1;
  //
  stocAPriori_ = 1.0;
  breakNoise_ = 1.0;
  tau_ = 1.0;
  whiteNoise_ = 1.0;
};



// A regular constructor:
inline SgParameterCfg::SgParameterCfg(const SgParameterCfg& pc) : 
  SgAttribute(pc)
{
  name_ = pc.getName();
  pMode_= pc.getPMode();
  sType_= pc.getSType();
  scale_ = pc.getScale();
  scaleName_ = pc.getScaleName();
  //
  convAPriori_ = pc.getConvAPriori();
  arcStep_ = pc.getArcStep();
  //
  pwlAPriori_ = pc.getPwlAPriori();
  pwlStep_ = pc.getPwlStep();
  pwlNumOfPolynomials_ = pc.getPwlNumOfPolynomials();
  //
  stocAPriori_ = pc.getStocAPriori();
  breakNoise_ = pc.getBreakNoise();
  tau_ = pc.getTau();
  whiteNoise_ = pc.getWhiteNoise();
};



// A destructor:
inline SgParameterCfg::~SgParameterCfg()
{
  // nothing to do
};



//
// INTERFACES:
//
//
inline SgParameterCfg& SgParameterCfg::operator=(const SgParameterCfg& pc)
{
  SgAttribute::operator=(pc);
  name_ = pc.getName();
  pMode_= pc.getPMode();
  sType_= pc.getSType();
  scale_ = pc.getScale();
  scaleName_ = pc.getScaleName();
  //
  convAPriori_ = pc.getConvAPriori();
  arcStep_ = pc.getArcStep();
  //
  pwlAPriori_ = pc.getPwlAPriori();
  pwlStep_ = pc.getPwlStep();
  pwlNumOfPolynomials_ = pc.getPwlNumOfPolynomials();
  //
  stocAPriori_ = pc.getStocAPriori();
  breakNoise_ = pc.getBreakNoise();
  tau_ = pc.getTau();
  whiteNoise_ = pc.getWhiteNoise();
  return *this;
};



//
inline const QString& SgParameterCfg::getName() const
{
  return name_;
};



//
inline SgParameterCfg::PMode SgParameterCfg::getPMode() const
{
  return pMode_;
};



//
inline SgParameterCfg::SType SgParameterCfg::getSType() const
{
  return sType_;
};



//
inline double SgParameterCfg::getConvAPriori() const
{
  return convAPriori_;
};



//
inline double SgParameterCfg::getStocAPriori() const
{
  return stocAPriori_;
};



//
inline double SgParameterCfg::getPwlAPriori() const
{
  return pwlAPriori_;
};



//
inline double SgParameterCfg::getBreakNoise() const
{
  return breakNoise_;
};



//
inline double SgParameterCfg::getTau() const
{
  return tau_;
};



//
inline double SgParameterCfg::getWhiteNoise() const
{
  return whiteNoise_;
};



//
inline double SgParameterCfg::getPwlStep() const
{
  return pwlStep_;
};



//
inline int SgParameterCfg::getPwlNumOfPolynomials() const
{
  return pwlNumOfPolynomials_;
};



//
inline double SgParameterCfg::getArcStep() const
{
  return arcStep_;
};



//
inline double SgParameterCfg::getScale() const
{
  return scale_;
};



//
inline const QString& SgParameterCfg::getScaleName() const
{
  return scaleName_;
};



//
inline void SgParameterCfg::setName(const QString& name)
{
  name_ = name;
};



//
inline void SgParameterCfg::setPMode(PMode mode)
{
  pMode_ = mode;
};



//
inline void SgParameterCfg::setSType(SType type)
{
  sType_ = type;
};



//
inline void SgParameterCfg::setConvAPriori(double v)
{
  convAPriori_ = v;
};



//
inline void SgParameterCfg::setStocAPriori(double v)
{
  stocAPriori_ = v;
};



//
inline void SgParameterCfg::setPwlAPriori(double v)
{
  pwlAPriori_ = v;
};



//
inline void SgParameterCfg::setBreakNoise(double v)
{
  breakNoise_ = v;
};



//
inline void SgParameterCfg::setTau(double tau)
{
  tau_ = tau;
};



//
inline void SgParameterCfg::setWhiteNoise(double w)
{
  whiteNoise_ = w;
};



//
inline void SgParameterCfg::setPwlStep(double s)
{
  pwlStep_ = s;
};



//
inline void SgParameterCfg::setPwlNumOfPolynomials(int n)
{
  pwlNumOfPolynomials_ = n;
};



//
inline void SgParameterCfg::setArcStep(double s)
{
  arcStep_ = s;
};



//
inline void SgParameterCfg::setScale(double s)
{
  scale_ = s;
};



//
inline void SgParameterCfg::setScaleName(const QString& units)
{
  scaleName_ = units;
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
#endif //SG_EST_PARAMETER_CFG_H
