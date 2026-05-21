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

#ifndef SG_VLBI_MEASUREMENT_H
#define SG_VLBI_MEASUREMENT_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <QtCore/QString>

#include <math.h>


#include <SgAttribute.h>
#include <SgTaskConfig.h>


// #define SAS 310.0e-12

const double dSas = 310.0e-12;


class QDataStream;

class SgTaskConfig;

/***===================================================================================================*/
/**
 * A class that represents VLBI quasi-measurements. 
 *
 */
/**====================================================================================================*/
class SgVlbiMeasurement  : public SgAttribute
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<<0, //!< omit the observation;
    Attr_PROCESSED          = 1<<1, //!< the observation has been processed;
    Attr_IMMUNE2INCL        = 1<<2, //!< if deselected, the observation will not be automaticaly restored
    Attr_IMMUNE2EXCL        = 1<<3, //!< if is ok, the observation will not be automaticaly deselected
    Attr_REQ_IONO_CORR      = 1<<4, //!< the ionosphere correction were requested to calculate
    Attr_HAS_IONO_CORR      = 1<<5, //!< the ionosphere correction is available
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgVlbiMeasurement(const QString& name, SgTaskConfig::VlbiDelayType delayType, 
    SgTaskConfig::VlbiRateType rateType);

  /**A constructor.
   * Creates a copy of the object.
   */
  inline SgVlbiMeasurement(const SgVlbiMeasurement&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiMeasurement();



  //
  // Interfaces:
  //
  // gets:
  /**
   */
  inline const QString& getName() const;

  inline SgTaskConfig::VlbiDelayType getDelayType() const {return delayType_;};
  inline SgTaskConfig::VlbiRateType  getRateType() const {return rateType_;};

  /**
   */
  inline double getValue() const;
  inline double getAlternateValue() const;

  /**
   */
  inline double getGeocenterValue() const;

  /**
   */
  inline double getAmbiguitySpacing() const;
  inline double getSubAmbigSpacing() const {return subAmbigSpacing_;};

  /**
   */
  inline int getNumOfAmbiguities() const;
  inline int getNumOfSubAmbigs() const {return numOfSubAmbigs_;};

  /**
   */
  inline double getSigma() const;

  /**
   */
  inline double getSigma2add() const;

  /**
   */
  inline double sigma2Apply() const;

  /**
   */
  inline double getResidual() const;

  inline double getResidualFringeFitting() const {return residualFringeFitting_;};
  /**
   */
  inline double getResidualNorm() const;

  /**
   */
  inline short int getUnweightFlag() const;

  /**
   */
  inline double getEffFreq() const;
  inline double getEffFreqEqWgt() const;

  /**
   */
  inline double getQ2() const;

  /**
   */
  inline double getIonoValue() const;

  /**
   */
  inline double getIonoSigma() const;


  // sets:
  /**
   */
  inline void setValue(double);
  inline void setAlternateValue(double);

  /**
   */
  inline void setGeocenterValue(double);

  /**
   */
  inline void setAmbiguitySpacing(double);
  inline void setSubAmbigSpacing(double s) {subAmbigSpacing_ = s;};

  /**
   */
  inline void setNumOfAmbiguities(int);
  inline void setNumOfSubAmbigs(int n) {numOfSubAmbigs_ = n;};

  /**
   */
  inline void setSigma(double);

  /**
   */
  inline void setSigma2add(double);

  /**
   */
  inline void setResidual(double);

  inline void setResidualFringeFitting(double r) {residualFringeFitting_ = r;};

  /**
   */
  inline void setResidualNorm(double);

  /**
   */
  inline void setUnweightFlag(short int);

  /**
   */
  inline void setEffFreq(double);
  inline void setEffFreqEqWgt(double);

  /**
   */
  inline void setQ2(double);

  /**
   */
  inline void setIonoValue(double);

  /**
   */
  inline void setIonoSigma(double);


  //
  // Functions:
  //
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  inline void zerofy();
  
  //
  inline void zerofyIono();
  
  /**
   */
  inline double ambiguity() const;
  inline double subAmbig() const;
  
  void adjustAmbiguity(double closestResidual=0.0);

  
  /**
   */
  void setupSigma2Apply(const SgTaskConfig*);

  void resetAllEditings();


  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);

  
private:
  // name:
  const QString                 name_;
  SgTaskConfig::VlbiDelayType   delayType_;
  SgTaskConfig::VlbiRateType    rateType_;
  double                        dSign_;
  // measurement:
  double                        value_;
  double                        alternateValue_;
  double                        sigma_;
  double                        geocenterValue_;
  double                        ambiguitySpacing_;
  int                           numOfAmbiguities_;
  double                        subAmbigSpacing_;
  int                           numOfSubAmbigs_;
  // aux sigmas:
  double                        sigma2add_;
  double                        sigma2Apply_;
  // residuals:
  double                        residual_;
  double                        residualNorm_;
  double                        residualFringeFitting_;
  //
  short int                     unweightFlag_;
  // ionospheric stuff:
  double                        effFreq_;
  double                        effFreqEqWgt_;
  double                        q2_;
  double                        ionoValue_;
  double                        ionoSigma_;
  //
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiMbandObservation inline members:                                                              */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgVlbiMeasurement::SgVlbiMeasurement(const QString& name, SgTaskConfig::VlbiDelayType delayType,
  SgTaskConfig::VlbiRateType rateType) :
  SgAttribute(),
  name_(name)
{
  delayType_ = delayType;
  rateType_ = rateType;
  value_ = 0.0;
  alternateValue_ = 0.0;
  geocenterValue_ = 0.0;
  ambiguitySpacing_ = 0.0;
  numOfAmbiguities_ = 0;
//  subAmbigSpacing_ = 310.0e-12; //0.0;
//  subAmbigSpacing_ = 160.0e-12; //0.0;
  subAmbigSpacing_ = dSas; //0.0;
  numOfSubAmbigs_ = 0;
    
  sigma_ = 0.0;
  sigma2add_ = 0.0;
  sigma2Apply_ = 0.0;
  residual_ = 0.0;
  residualFringeFitting_ = 0.0;
  residualNorm_ = 0.0;
  unweightFlag_ = 0;
  effFreq_ = effFreqEqWgt_ = 0.0;
  q2_ = 0.0;
  ionoValue_ = 0.0;
  ionoSigma_ = 0.0;
  dSign_ = (delayType == SgTaskConfig::VD_PHS_DELAY)? -1.0 : 1.0;
};



//
inline SgVlbiMeasurement::SgVlbiMeasurement(const SgVlbiMeasurement& m) :
  SgAttribute(m),
  name_(m.getName()),
  delayType_(m.delayType_),
  rateType_(m.rateType_)
{
  setValue(m.getValue());
  setAlternateValue(m.getAlternateValue());
  setGeocenterValue(m.getGeocenterValue());
  setAmbiguitySpacing(m.getAmbiguitySpacing());
  setNumOfAmbiguities(m.getNumOfAmbiguities());
 
  setSubAmbigSpacing(m.getSubAmbigSpacing());
  setNumOfSubAmbigs(m.getNumOfSubAmbigs());
  
  setSigma(m.getSigma());
  setSigma2add(m.getSigma2add());
  sigma2Apply_ = m.sigma2Apply_;
  setResidual(m.getResidual());
  setResidualFringeFitting(m.getResidualFringeFitting());
  setResidualNorm(m.getResidualNorm());
  setUnweightFlag(m.getUnweightFlag());
  setEffFreq(m.getEffFreq());
  setEffFreqEqWgt(m.getEffFreqEqWgt());
  setQ2(m.getQ2());
  setIonoValue(m.getIonoValue());
  setIonoSigma(m.getIonoSigma());
  dSign_ = m.dSign_;
};



// A destructor:
inline SgVlbiMeasurement::~SgVlbiMeasurement()
{
};



//
// INTERFACES:
//
//
// Gets:
//
inline const QString& SgVlbiMeasurement::getName() const
{
  return name_;
};



//
inline double SgVlbiMeasurement::getValue() const
{
  return value_;
};



//
inline double SgVlbiMeasurement::getAlternateValue() const
{
  return alternateValue_;
};



//
inline double SgVlbiMeasurement::getGeocenterValue() const
{
  return geocenterValue_;
};



//
inline double SgVlbiMeasurement::getAmbiguitySpacing() const
{
  return ambiguitySpacing_;
};



//
inline int SgVlbiMeasurement::getNumOfAmbiguities() const
{
  return numOfAmbiguities_;
};



//
inline double SgVlbiMeasurement::getSigma() const
{
  return sigma_;
};



//
inline double SgVlbiMeasurement::getSigma2add() const
{
  return sigma2add_;
};



//
inline double SgVlbiMeasurement::sigma2Apply() const
{
  return sigma2Apply_;
};



//
inline double SgVlbiMeasurement::getResidual() const
{
  return residual_;
};



//
inline double SgVlbiMeasurement::getResidualNorm() const
{
  return residualNorm_;
};



//
inline short int SgVlbiMeasurement::getUnweightFlag() const
{
  return unweightFlag_;
};



//
inline double SgVlbiMeasurement::getEffFreq() const
{
  return effFreq_;
};



//
inline double SgVlbiMeasurement::getEffFreqEqWgt() const
{
  return effFreqEqWgt_;
};



//
inline double SgVlbiMeasurement::getQ2() const
{
  return q2_;
};



//
inline double SgVlbiMeasurement::getIonoValue() const
{
  return ionoValue_;
};



//
inline double SgVlbiMeasurement::getIonoSigma() const
{
  return ionoSigma_;
};




// Sets:
//
inline void SgVlbiMeasurement::setValue(double v)
{
  value_ = v;
};



//
inline void SgVlbiMeasurement::setAlternateValue(double v)
{
  alternateValue_ = v;
};



//
inline void SgVlbiMeasurement::setGeocenterValue(double v)
{
  geocenterValue_ = v;
};



//
inline void SgVlbiMeasurement::setAmbiguitySpacing(double a)
{
  ambiguitySpacing_ = a;
};



//
inline void SgVlbiMeasurement::setNumOfAmbiguities(int n)
{
  numOfAmbiguities_ = n;
};



//
inline void SgVlbiMeasurement::setSigma(double s)
{
  sigma_ = s;
};



//
inline void SgVlbiMeasurement::setSigma2add(double s)
{
  sigma2add_ = s;
};



//
inline void SgVlbiMeasurement::setResidual(double r)
{
  residual_ = r;
};



//
inline void SgVlbiMeasurement::setResidualNorm(double r)
{
  residualNorm_ = r;
};



//
inline void SgVlbiMeasurement::setUnweightFlag(short int f)
{
  unweightFlag_ = f;
};



//
inline void SgVlbiMeasurement::setEffFreq(double f)
{
  effFreq_ = f;
};



//
inline void SgVlbiMeasurement::setEffFreqEqWgt(double f)
{
  effFreqEqWgt_ = f;
};



//
inline void SgVlbiMeasurement::setQ2(double q)
{
  q2_ = q;
};



//
inline void SgVlbiMeasurement::setIonoValue(double v)
{
  ionoValue_ = v;
};



//
inline void SgVlbiMeasurement::setIonoSigma(double s)
{
  ionoSigma_ = s;
  if (0.0 < ionoSigma_)
    addAttr(Attr_HAS_IONO_CORR);
};




//
// FUNCTIONS:
//
//
inline void SgVlbiMeasurement::zerofy()
{
  value_ = 0.0;
  alternateValue_ = 0.0;
  geocenterValue_ = 0.0;
  ambiguitySpacing_ = 0.0;
  numOfAmbiguities_ = 0;
//  subAmbigSpacing_ = 310.0e-12;
//  subAmbigSpacing_ = 160.0e-12;
  subAmbigSpacing_ = dSas;
  numOfSubAmbigs_ = 0;
  sigma_ = 0.0;
  sigma2add_ = 0.0;
  sigma2Apply_ = 0.0;
  residual_ = 0.0;
  residualNorm_ = 0.0;
  unweightFlag_ = 0;
  effFreq_ = effFreqEqWgt_ = 0.0;
  q2_ = 0.0;
  ionoValue_ = 0.0;
  ionoSigma_ = 0.0;
  delAttr(Attr_HAS_IONO_CORR);
};



//
inline void SgVlbiMeasurement::zerofyIono()
{
  q2_         = 0.0;
  ionoValue_  = 0.0;
  ionoSigma_  = 0.0;
  delAttr(Attr_HAS_IONO_CORR);
};



//
inline double SgVlbiMeasurement::ambiguity() const
{
  return ambiguitySpacing_*numOfAmbiguities_;
};



//
inline double SgVlbiMeasurement::subAmbig() const
{
  return subAmbigSpacing_*numOfSubAmbigs_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_VLBI_MEASUREMENT_H
