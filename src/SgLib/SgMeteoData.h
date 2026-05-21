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

#ifndef SG_METEO_DATA_H
#define SG_METEO_DATA_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <SgAttribute.h>



#ifndef SEPARATED_LOG2ANT
class SgVlbiStationInfo;
#endif





/***===================================================================================================*/
/**
 * A storage for meteo parameters.
 *
 */
/**====================================================================================================*/
class SgMeteoData : public SgAttribute
{
public:
  enum Attributes
  {
    Attr_BAD_DATA           = 1<<0, //!< data readings are wrong;
    Attr_ARTIFICIAL_DATA    = 1<<1, //!< data are artificial (mean values or some model);
  };


  // Statics:
  static double dewPt2Rho(double temperature, double dewPtTemperature);
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgMeteoData();

  /**Another constructor.
   * Creates an object with data.
   */
  inline SgMeteoData(double t, double p, double rho);

  /**Another constructor.
   * Creates an object with data.
   */
  inline SgMeteoData(const SgMeteoData&);
 
  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgMeteoData();



  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgMeteoData& operator=(const SgMeteoData&);

  // gets:
  /**Returns temperature readings, C.
   */
  inline double getTemperature() const;

  /**Returns pressure readings, mbar.
   */
  inline double getPressure() const;

  /**Returns relative humidity readings.
   */
  inline double getRelativeHumidity() const;




  // sets:
  /**Sets up temperature value.
   * \param t -- the new temperature, C;
   */
  inline void setTemperature(double t);

  /**Sets up pressure value.
   * \param p -- the new pressure, mbar;
   */
  inline void setPressure(double p);

  /**Sets up relative humidity value.
   * \param rho -- the new relative humidity, %;
   */
  inline void setRelativeHumidity(double rho);


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  inline const QString className() const;

  inline bool operator==(const SgMeteoData& meteo) const;

  inline bool operator!=(const SgMeteoData& meteo) const;


#ifndef SEPARATED_LOG2ANT
  /**Returns usable temperature.
   */
  double temperature(double height, const SgVlbiStationInfo* stnInfo) const;
  
  /**Returns usable pressure.
   */
  double pressure(double height, const SgVlbiStationInfo* stnInfo) const;
  
  /**Returns usable relative humidity.
   */
  double relativeHumidity(double height, const SgVlbiStationInfo* stnInfo) const;
#endif

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  double              temperature_;
  double              pressure_;
  double              relativeHumidity_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgMeteoData inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgMeteoData::SgMeteoData() : 
    SgAttribute()
{
  temperature_ = 0.0;
  pressure_ = 0.0;
  relativeHumidity_ = 0.0;
  // these data are unreal, when user setups correct values, he/she will turn off this flag:
  addAttr(Attr_ARTIFICIAL_DATA);
};



//
inline SgMeteoData::SgMeteoData(const SgMeteoData& m) : 
    SgAttribute()
{
  temperature_ = m.getTemperature();
  pressure_ = m.getPressure();
  relativeHumidity_ = m.getRelativeHumidity();
};



//
inline SgMeteoData::SgMeteoData(double t, double p, double rho) : 
    SgAttribute()
{
  temperature_ = t;
  pressure_ = p;
  relativeHumidity_ = rho;
};



// A destructor:
inline SgMeteoData::~SgMeteoData()
{
  // nothing to do
};



//
inline SgMeteoData& SgMeteoData::operator=(const SgMeteoData& m)
{
  setAttributes(m.getAttributes());
  temperature_ = m.temperature_;
  pressure_ = m.pressure_;
  relativeHumidity_ = m.relativeHumidity_;
  return *this;
};



//
// INTERFACES:
//
// returns temperature
inline double SgMeteoData::getTemperature() const
{
  return temperature_;
};



//
inline double SgMeteoData::getPressure() const
{
  return pressure_;
};



//
inline double SgMeteoData::getRelativeHumidity() const
{
  return relativeHumidity_;
};



//
inline void SgMeteoData::setTemperature(double t)
{
  temperature_ = t;
};



//
inline void SgMeteoData::setPressure(double p)
{
  pressure_ = p;
};



//
inline void SgMeteoData::setRelativeHumidity(double rho)
{
  relativeHumidity_ = rho;
};



//
// FUNCTIONS:
//
//
//
inline const QString SgMeteoData::className() const 
{
  return "SgMeteoData";
};



//
inline bool SgMeteoData::operator==(const SgMeteoData& meteo) const
{
  return temperature_ == meteo.getTemperature() &&
    pressure_ == meteo.getPressure() &&
    relativeHumidity_ == meteo.getRelativeHumidity();
};



//
inline bool SgMeteoData::operator!=(const SgMeteoData& meteo) const
{
  return !(*this==meteo);
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
#endif // SG_METEO_DATA_H
