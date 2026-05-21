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

#ifndef SG_EST_PARAMETERS_DESCRIPTOR_H
#define SG_EST_PARAMETERS_DESCRIPTOR_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

class QDataStream;

#include <SgParameterCfg.h>


#define NUMBER_OF_KNOWN_PARAMETER_SPECIES  21


/***===================================================================================================*/
/**
 * SgParamatersDescriptor -- a descriptor of parameters that will be estimated.
 *
 */
/**====================================================================================================*/
class SgParametersDescriptor
{
public:
  enum ParIdx
  {
    Idx_CLOCK_0          = 0,   //!< index for clocks (order #0)
    Idx_CLOCK_1          = 1,   //!< index for clocks (order #1)
    Idx_CLOCK_2          = 2,   //!< index for clocks (order #2)
    Idx_CLOCK_3          = 3,   //!< index for clocks (order #3)
    Idx_ZENITH           = 4,   //!< index for wet zenith delay
    Idx_ATMGRAD          = 5,   //!< index for atmospheric gradients
    Idx_CABLE            = 6,   //!< index for cable corrections
    Idx_AXISOFFSET       = 7,   //!< index for axis offset
    Idx_STNCOO           = 8,   //!< index for station coordinates
    Idx_STNVEL           = 9,   //!< index for station velocities
    Idx_SRCCOO           =10,   //!< index for source coordinates
    Idx_SRCSSM           =11,   //!< index for source SSM
    Idx_POLUSXY          =12,   //!< index for EOP: polar motion
    Idx_POLUSXYR         =13,   //!< index for EOP: polar motion
    Idx_POLUSUT1         =14,   //!< index for EOP: d(UT1-UTC)
    Idx_POLUSUT1R        =15,   //!< index for EOP: d(UT1-UTC) Rate
    Idx_POLUSNUT         =16,   //!< index for EOP: nutation angles
    Idx_POLUSNUTR        =17,   //!< index for EOP: nutation angles
    Idx_BL_CLK           =18,   //!< index for baseline clocks
    Idx_BL_LENGTH        =19,   //!< index for baseline length
    Idx_TEST             =20,   //!< index for test purposes
  };


  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgParametersDescriptor();

  /**A constructor.
   * Creates a copy of the object.
   */
  inline SgParametersDescriptor(const SgParametersDescriptor&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgParametersDescriptor();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgParametersDescriptor& operator=(const SgParametersDescriptor&);
  
  // gets:
  //
  // Clocks:
  //
  /**Returns a parameter configuration for clocks model at a station (zero order).
   */
  inline const SgParameterCfg& getClock0() const;

  /**Returns a parameter configuration for clocks model at a station (first order).
   */
  inline const SgParameterCfg& getClock1() const;

  /**Returns a parameter configuration for clocks model at a station (second order).
   */
  inline const SgParameterCfg& getClock2() const;

  /**Returns a parameter configuration for clocks model at a station (third order).
   */
  inline const SgParameterCfg& getClock3() const;

  // troposphere:
  /**Returns a parameter configuration for zenith delay.
   */
  inline const SgParameterCfg& getZenith() const;

  /**Returns a parameter configuration for atmospheric gradients.
   */
  inline const SgParameterCfg& getAtmGrad() const;

  /**Returns a parameter configuration for cable correction.
   */
  inline const SgParameterCfg& getCable() const;

  /**Returns a parameter configuration for axis offset.
   */
  inline const SgParameterCfg& getAxisOffset() const;

  // coordinates:
  /**Returns a parameter configuration for coordinates of a stations.
   */
  inline const SgParameterCfg& getStnCoo() const;

  /**Returns a parameter configuration for velocities of a station.
   */
  inline const SgParameterCfg& getStnVel() const;

  /**Returns a parameter configuration for coordinates of a radio source.
   */
  inline const SgParameterCfg& getSrcCoo() const;

  /**Returns a parameter configuration for parameters of a source structure model
   */
  inline const SgParameterCfg& getSrcSsm() const;

  // EOP:
  /**Returns a parameter configuration for Polus coordinates.
   */
  inline const SgParameterCfg& getPolusXY() const;

  /**Returns a parameter configuration for rates of Polus coordinates.
   */
  inline const SgParameterCfg& getPolusXYRate() const;

  /**Returns a parameter configuration for d(UT1-UTC).
   */
  inline const SgParameterCfg& getPolusUT1() const;

  /**Returns a parameter configuration for d(UT1-UTC) rate.
   */
  inline const SgParameterCfg& getPolusUT1Rate() const;

  /**Returns a parameter configuration for nutation angles.
   */
  inline const SgParameterCfg& getPolusNut() const;

  /**Returns a parameter configuration for rates of nutation angles.
   */
  inline const SgParameterCfg& getPolusNutRate() const;

  /**Returns a parameter configuration for baseline clock offsets.
   */
  inline const SgParameterCfg& getBlClock() const;

  /**Returns a parameter configuration for vector of baseline.
   */
  inline const SgParameterCfg& getBlLength() const;

  // test:
  /**Returns a parameter configuration for test parameter.
   */
  inline const SgParameterCfg& getTest() const;



  // sets:
  /**Sets up a parameter configuration for clocks at a station (zero order).
   * \param p -- new configuration of the parameter.
   */
  inline void setClock0(const SgParameterCfg& p);

  /**Sets up a parameter configuration for clocks at a station (first order).
   * \param p -- new configuration of the parameter.
   */
  inline void setClock1(const SgParameterCfg& p);

  /**Sets up a parameter configuration for clocks at a station (second order).
   * \param p -- new configuration of the parameter.
   */
  inline void setClock2(const SgParameterCfg& p);

  /**Sets up a parameter configuration for clocks at a station (third order).
   * \param p -- new configuration of the parameter.
   */
  inline void setClock3(const SgParameterCfg& p);

  /**Sets up a parameter configuration for zenith delay.
   * \param p -- new configuration of the parameter.
   */
  inline void setZenith(const SgParameterCfg& p);

  /**Sets up a parameter configuration for atmospheric gradients.
   * \param p -- new configuration of the parameter.
   */
  inline void setAtmGrad(const SgParameterCfg& p);

  /**Sets up a parameter configuration for cable correction.
   * \param p -- new configuration of the parameter.
   */
  inline void setCable(const SgParameterCfg& p);

  /**Sets up a parameter configuration for axis offset.
   * \param p -- new configuration of the parameter.
   */
  inline void setAxisOffset(const SgParameterCfg& p);

  /**Sets up a parameter configuration for coordinates of a station.
   * \param p -- new configuration of the parameter.
   */
  inline void setStnCoo(const SgParameterCfg& p);

  /**Sets up a parameter configuration for velocities of a station.
   * \param p -- new configuration of the parameter.
   */
  inline void setStnVel(const SgParameterCfg& p);

  /**Sets up a parameter configuration for coordinates of a radio source.
   * \param p -- new configuration of the parameter.
   */
  inline void setSrcCoo(const SgParameterCfg& p);

  inline void setSrcSsm(const SgParameterCfg& p);

  // EOP:
  /**Sets up a parameter configuration for polus coordinates.
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusXY(const SgParameterCfg& p);

  /**Sets up a parameter configuration for polus coordinates rates.
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusXYRate(const SgParameterCfg& p);

  /**Sets up a parameter configuration for d(UT1-UTC).
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusUT1(const SgParameterCfg& p);

  /**Sets up a parameter configuration for d(UT1-UTC) rate.
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusUT1Rate(const SgParameterCfg& p);

  /**Sets up a parameter configuration for nutation angles.
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusNut(const SgParameterCfg& p);

  /**Sets up a parameter configuration for rates of nutation angles.
   * \param p -- new configuration of the parameter.
   */
  inline void setPolusNutRate(const SgParameterCfg& p);

  /**Sets up a parameter configuration for baseline clock offsets.
   * \param p -- new configuration of the parameter.
   */
  inline void setBlClock(const SgParameterCfg& p);

  /**Sets up a parameter configuration for vector of baseline.
   * \param p -- new configuration of the parameter.
   */
  inline void setBlLength(const SgParameterCfg& p);

  /**Sets up a parameter configuration for test parameter.
   * \param p -- new configuration of the parameter.
   */
  inline void setTest(const SgParameterCfg& p);


  // modes:
  /**Returns a parameter mode for clocks model at a station (zero order).
   */
  inline SgParameterCfg::PMode getClock0Mode() const;

  /**Returns a parameter mode for clocks model at a station (first order).
   */
  inline SgParameterCfg::PMode getClock1Mode() const;

  /**Returns a parameter mode for clocks model at a station (second order).
   */
  inline SgParameterCfg::PMode getClock2Mode() const;

  /**Returns a parameter mode for clocks model at a station (third order).
   */
  inline SgParameterCfg::PMode getClock3Mode() const;

  // troposphere:
  /**Returns a parameter mode for zenith delay.
   */
  inline SgParameterCfg::PMode getZenithMode() const;

  /**Returns a parameter mode for atmospheric gradients.
   */
  inline SgParameterCfg::PMode getAtmGradMode() const;

  /**Returns a parameter mode for cable correction.
   */
  inline SgParameterCfg::PMode getCableMode() const;

  /**Returns a parameter mode for axis offset.
   */
  inline SgParameterCfg::PMode getAxisOffsetMode() const;

  // coordinates:
  /**Returns a parameter mode for coordinates of a stations.
   */
  inline SgParameterCfg::PMode getStnCooMode() const;

  /**Returns a parameter mode for velocities of a station.
   */
  inline SgParameterCfg::PMode getStnVelMode() const;

  /**Returns a parameter mode for coordinates of a radio source.
   */
  inline SgParameterCfg::PMode getSrcCooMode() const;
  inline SgParameterCfg::PMode getSrcSsmMode() const;

  // EOP:
  /**Returns a parameter mode for Polus coordinates.
   */
  inline SgParameterCfg::PMode getPolusXYMode() const;

  /**Returns a parameter mode for rats of Polus coordinates.
   */
  inline SgParameterCfg::PMode getPolusXYRateMode() const;

  /**Returns a parameter mode for d(UT1-UTC).
   */
  inline SgParameterCfg::PMode getPolusUT1Mode() const;

  /**Returns a parameter mode for d(UT1-UTC) rate.
   */
  inline SgParameterCfg::PMode getPolusUT1RateMode() const;

  /**Returns a parameter mode for nutation angles.
   */
  inline SgParameterCfg::PMode getPolusNutMode() const;

  /**Returns a parameter mode for nutation angles rates.
   */
  inline SgParameterCfg::PMode getPolusNutRateMode() const;

  /**Returns a parameter mode for baseline clock offsets.
   */
  inline SgParameterCfg::PMode getBlClockMode() const;

  /**Returns a parameter mode for vector of baseline.
   */
  inline SgParameterCfg::PMode getBlLengthMode() const;

  // test:
  /**Returns a parameter mode for test parameter.
   */
  inline SgParameterCfg::PMode getTestMode() const;
  


  /**Changes a mode for the parameter configuration for clocks at a station (zero order).
   * \param mode -- new mode of the parameter.
   */
  inline void setClock0Mode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for clocks at a station (first order).
   * \param mode -- new mode of the parameter.
   */
  inline void setClock1Mode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for clocks at a station (second order).
   * \param mode -- new mode of the parameter.
   */
  inline void setClock2Mode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for clocks at a station (third order).
   * \param mode -- new mode of the parameter.
   */
  inline void setClock3Mode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for zenith delay.
   * \param mode -- new mode of the parameter.
   */
  inline void setZenithMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for atmospheric gradients.
   * \param mode -- new mode of the parameter.
   */
  inline void setAtmGradMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for cable correction.
   * \param mode -- new mode of the parameter.
   */
  inline void setCableMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for axis offset.
   * \param mode -- new mode of the parameter.
   */
  inline void setAxisOffsetMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for coordinates of a station.
   * \param mode -- new mode of the parameter.
   */
  inline void setStnCooMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for velocities of a station.
   * \param mode -- new mode of the parameter.
   */
  inline void setStnVelMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for coordinates of a sources.
   * \param mode -- new mode of the parameter.
   */
  inline void setSrcCooMode(SgParameterCfg::PMode mode);
  inline void setSrcSsmMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for polus coordinates.
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusXYMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for polus coordinates rates.
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusXYRateMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for d(UT1-UTC).
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusUT1Mode(SgParameterCfg::PMode mode);

  /**Changes a mode for the parameter configuration for d(UT1-UTC) rate.
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusUT1RateMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for nutation angles.
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusNutMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for nutation angles rates.
   * \param mode -- new mode of the parameter.
   */
  inline void setPolusNutRateMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for baseline clock offsets.
   * \param mode -- new mode of the parameter.
   */
  inline void setBlClockMode(SgParameterCfg::PMode mode);

  /**Changes a mode for the parameter configuration for vector of baseline.
   * \param mode -- new mode of the parameter.
   */
  inline void setBlLengthMode(SgParameterCfg::PMode mode);
  
  /**Changes a mode for the parameter configuration for test parameter.
   * \param mode -- new mode of the parameter.
   */
  inline void setTestMode(SgParameterCfg::PMode mode);



  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  void setMode(int i, SgParameterCfg::PMode mode);
  
  SgParameterCfg::PMode getMode(int i) const;

  /**Returns i-th parameter.
   */
  inline const SgParameterCfg& getParameter(int i) const;

  /**Sets i-th parameter.
   */
  inline void setParameter(int i, const SgParameterCfg& p);

  /**Returns a reference on i-th parameter.
   */
  inline SgParameterCfg& parameter(ParIdx i) {return parameters_[0<=i&&i<num_ ? i : 0];};

  /**Returns total number of the available parameters.
   */
  static int num();
  
  void unsetAllParameters();
  void setMode4Parameter (SgParameterCfg::PMode, ParIdx);
  void unsetParameter    (ParIdx);
  void setMode4Parameters(SgParameterCfg::PMode, const QList<ParIdx>&);
  void unsetParameters   (const QList<ParIdx>&);
  
  
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
  SgParameterCfg        parameters_[NUMBER_OF_KNOWN_PARAMETER_SPECIES];
  static const int      num_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgParamatersDescriptor inline members:                                                              */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// A copying constructor:
inline SgParametersDescriptor::SgParametersDescriptor(const SgParametersDescriptor& pd)
{
  *this = pd;
};



// A destructor:
inline SgParametersDescriptor::~SgParametersDescriptor()
{
  // nothing to do
};



//
// INTERFACES:
//
//
//
inline SgParametersDescriptor& SgParametersDescriptor::operator=(const SgParametersDescriptor& pd)
{
  for (int i=0; i<pd.num(); i++)
    setParameter(i, pd.getParameter(i));
  return *this;
};



// gets:
inline const SgParameterCfg& SgParametersDescriptor::getClock0() const
{
  return parameters_[Idx_CLOCK_0];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getClock1() const
{
  return parameters_[Idx_CLOCK_1];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getClock2() const
{
  return parameters_[Idx_CLOCK_2];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getClock3() const
{
  return parameters_[Idx_CLOCK_3];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getZenith() const
{
  return parameters_[Idx_ZENITH];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getAtmGrad() const
{
  return parameters_[Idx_ATMGRAD];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getCable() const
{
  return parameters_[Idx_CABLE];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getAxisOffset() const
{
  return parameters_[Idx_AXISOFFSET];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getStnCoo() const
{
  return parameters_[Idx_STNCOO];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getStnVel() const
{
  return parameters_[Idx_STNVEL];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getSrcCoo() const
{
  return parameters_[Idx_SRCCOO];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getSrcSsm() const
{
  return parameters_[Idx_SRCSSM];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusXY() const
{
  return parameters_[Idx_POLUSXY];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusXYRate() const
{
  return parameters_[Idx_POLUSXYR];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusUT1() const
{
  return parameters_[Idx_POLUSUT1];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusUT1Rate() const
{
  return parameters_[Idx_POLUSUT1R];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusNut() const
{
  return parameters_[Idx_POLUSNUT];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getPolusNutRate() const
{
  return parameters_[Idx_POLUSNUTR];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getBlClock() const
{
  return parameters_[Idx_BL_CLK];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getBlLength() const
{
  return parameters_[Idx_BL_LENGTH];
};



//
inline const SgParameterCfg& SgParametersDescriptor::getTest() const
{
  return parameters_[Idx_TEST];
};



// sets:
inline void SgParametersDescriptor::setClock0(const SgParameterCfg& p)
{
  parameters_[Idx_CLOCK_0] = p;
};



//
inline void SgParametersDescriptor::setClock1(const SgParameterCfg& p)
{
  parameters_[Idx_CLOCK_1] = p;
};



//
inline void SgParametersDescriptor::setClock2(const SgParameterCfg& p)
{
  parameters_[Idx_CLOCK_2] = p;
};



//
inline void SgParametersDescriptor::setClock3(const SgParameterCfg& p)
{
  parameters_[Idx_CLOCK_3] = p;
};



//
inline void SgParametersDescriptor::setZenith(const SgParameterCfg& p)
{
  parameters_[Idx_ZENITH] = p;
};



//
inline void SgParametersDescriptor::setAtmGrad(const SgParameterCfg& p)
{
  parameters_[Idx_ATMGRAD] = p;
};



//
inline void SgParametersDescriptor::setCable(const SgParameterCfg& p)
{
  parameters_[Idx_CABLE] = p;
};



//
inline void SgParametersDescriptor::setAxisOffset(const SgParameterCfg& p)
{
  parameters_[Idx_AXISOFFSET] = p;
};



//
inline void SgParametersDescriptor::setStnCoo(const SgParameterCfg& p)
{
  parameters_[Idx_STNCOO] = p;
};



//
inline void SgParametersDescriptor::setStnVel(const SgParameterCfg& p)
{
  parameters_[Idx_STNVEL] = p;
};



//
inline void SgParametersDescriptor::setSrcCoo(const SgParameterCfg& p)
{
  parameters_[Idx_SRCCOO] = p;
};



//
inline void SgParametersDescriptor::setSrcSsm(const SgParameterCfg& p)
{
  parameters_[Idx_SRCSSM] = p;
};



//
inline void SgParametersDescriptor::setPolusXY(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSXY] = p;
};



//
inline void SgParametersDescriptor::setPolusXYRate(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSXYR] = p;
};



//
inline void SgParametersDescriptor::setPolusUT1(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSUT1] = p;
};



//
inline void SgParametersDescriptor::setPolusUT1Rate(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSUT1R] = p;
};



//
inline void SgParametersDescriptor::setPolusNut(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSNUT] = p;
};



//
inline void SgParametersDescriptor::setPolusNutRate(const SgParameterCfg& p)
{
  parameters_[Idx_POLUSNUTR] = p;
};



//
inline void SgParametersDescriptor::setBlClock(const SgParameterCfg& p)
{
  parameters_[Idx_BL_CLK] = p;
};



//
inline void SgParametersDescriptor::setBlLength(const SgParameterCfg& p)
{
  parameters_[Idx_BL_LENGTH] = p;
};



//
inline void SgParametersDescriptor::setTest(const SgParameterCfg& p)
{
  parameters_[Idx_TEST] = p;
};





//
// FUNCTIONS:
//
//
//
inline const SgParameterCfg& SgParametersDescriptor::getParameter(int i) const
{
  return parameters_[0<=i&&i<num_ ? i : 0];
};



//
inline void SgParametersDescriptor::setParameter(int i, const SgParameterCfg& p)
{
  parameters_[0<=i&&i<num_ ? i : 0] = p;
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getClock0Mode() const
{
  return parameters_[Idx_CLOCK_0].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getClock1Mode() const
{
  return parameters_[Idx_CLOCK_1].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getClock2Mode() const
{
  return parameters_[Idx_CLOCK_2].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getClock3Mode() const
{
  return parameters_[Idx_CLOCK_3].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getZenithMode() const
{
  return parameters_[Idx_ZENITH].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getAtmGradMode() const
{
  return parameters_[Idx_ATMGRAD].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getCableMode() const
{
  return parameters_[Idx_CABLE].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getAxisOffsetMode() const
{
  return parameters_[Idx_AXISOFFSET].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getStnCooMode() const
{
  return parameters_[Idx_STNCOO].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getStnVelMode() const
{
  return parameters_[Idx_STNVEL].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getSrcCooMode() const
{
  return parameters_[Idx_SRCCOO].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getSrcSsmMode() const
{
  return parameters_[Idx_SRCSSM].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusXYMode() const
{
  return parameters_[Idx_POLUSXY].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusXYRateMode() const
{
  return parameters_[Idx_POLUSXYR].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusUT1Mode() const
{
  return parameters_[Idx_POLUSUT1].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusUT1RateMode() const
{
  return parameters_[Idx_POLUSUT1R].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusNutMode() const
{
  return parameters_[Idx_POLUSNUT].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getPolusNutRateMode() const
{
  return parameters_[Idx_POLUSNUTR].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getBlClockMode() const
{
  return parameters_[Idx_BL_CLK].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getBlLengthMode() const
{
  return parameters_[Idx_BL_LENGTH].getPMode();
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getTestMode() const
{
  return parameters_[Idx_TEST].getPMode();
};



//
inline void SgParametersDescriptor::setMode(int i, SgParameterCfg::PMode mode)
{
  parameters_[0<=i&&i<num_ ? i : 0].setPMode(mode);
};



//
inline SgParameterCfg::PMode SgParametersDescriptor::getMode(int i) const
{
  return parameters_[0<=i&&i<num_ ? i : 0].getPMode();
};



//
inline void SgParametersDescriptor::setClock0Mode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_CLOCK_0].setPMode(mode);
};



//
inline void SgParametersDescriptor::setClock1Mode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_CLOCK_1].setPMode(mode);
};



//
inline void SgParametersDescriptor::setClock2Mode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_CLOCK_2].setPMode(mode);
};



//
inline void SgParametersDescriptor::setClock3Mode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_CLOCK_3].setPMode(mode);
};



//
inline void SgParametersDescriptor::setZenithMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_ZENITH].setPMode(mode);
};



//
inline void SgParametersDescriptor::setAtmGradMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_ATMGRAD].setPMode(mode);
};



//
inline void SgParametersDescriptor::setCableMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_CABLE].setPMode(mode);
};



//
inline void SgParametersDescriptor::setAxisOffsetMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_AXISOFFSET].setPMode(mode);
};



//
inline void SgParametersDescriptor::setStnCooMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_STNCOO].setPMode(mode);
};



//
inline void SgParametersDescriptor::setStnVelMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_STNVEL].setPMode(mode);
};



//
inline void SgParametersDescriptor::setSrcCooMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_SRCCOO].setPMode(mode);
};



//
inline void SgParametersDescriptor::setSrcSsmMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_SRCSSM].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusXYMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSXY].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusXYRateMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSXYR].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusUT1Mode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSUT1].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusUT1RateMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSUT1R].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusNutMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSNUT].setPMode(mode);
};



//
inline void SgParametersDescriptor::setPolusNutRateMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_POLUSNUTR].setPMode(mode);
};



//
inline void SgParametersDescriptor::setBlClockMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_BL_CLK].setPMode(mode);
};



//
inline void SgParametersDescriptor::setBlLengthMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_BL_LENGTH].setPMode(mode);
};



//
inline void SgParametersDescriptor::setTestMode(SgParameterCfg::PMode mode)
{
  parameters_[Idx_TEST].setPMode(mode);
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
#endif //SG_EST_PARAMETERS_DESCRIPTOR_H
