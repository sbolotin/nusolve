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

#ifndef SG_MAPPING_FUNCTION_H
#define SG_MAPPING_FUNCTION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgMJD.h>
#include <SgMeteoData.h>




class SgVlbiStationInfo;
/***===================================================================================================*/
/**
 * SgMappingFunction -- an abstract class for mapping functions.
 *
 */
/**====================================================================================================*/
class SgMappingFunction
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgMappingFunction();

  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgMappingFunction();

  //
  // Interfaces:
  //
  inline double m() const {return m_;};
  inline double dM_dE() const {return dM_dE_;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return 1.0 (dumb class).
   */
  virtual double calc(const SgMeteoData& meteo, double e,
                        const SgVlbiStationInfo* stnInfo, const SgMJD& epoch);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  double                        m_;               // mapping function
  double                        dM_dE_;           // mapping function partial with resp. to elevation ang
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/**NMF Hydrostatic mapping Function by Niell, 1996.
 *
 */
/*=====================================================================================================*/
class SgDryMF_NMF : public SgMappingFunction
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return value of mapping function.
   */
  virtual double calc(const SgMeteoData& meteo, double e,
                        const SgVlbiStationInfo* stnInfo, const SgMJD& epoch);
  
  SgDryMF_NMF() : SgMappingFunction() {};
  ~SgDryMF_NMF() {};

private:
  static const double   modelArg_[5];
  static const double   modelA_avg_[5];
  static const double   modelB_avg_[5];
  static const double   modelC_avg_[5];
  static const double   modelA_amp_[5];
  static const double   modelB_amp_[5];
  static const double   modelC_amp_[5];
  double                linterpolate(double, double, double, double, double);
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/**NMF Wet mapping Function by Niell, 1996.
 *
 */
/*=====================================================================================================*/
class SgWetMF_NMF : public SgMappingFunction
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return value of mapping function.
   */
  virtual double calc(const SgMeteoData& meteo, double e,
                        const SgVlbiStationInfo* stnInfo, const SgMJD& epoch);

  SgWetMF_NMF() : SgMappingFunction() {};
  ~SgWetMF_NMF() {};

private:
  static const double   modelArg_[5];
  static const double   modelA_avg_[5];
  static const double   modelB_avg_[5];
  static const double   modelC_avg_[5];
  double                linterpolate(double, double, double, double, double);
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/**Gradient Mapping Function, Chen and Herring, 1997.
 *
 */
/*=====================================================================================================*/
class SgGradMF_CH : public SgMappingFunction
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return value of mapping function.
   */
  virtual double calc(const SgMeteoData& meteo, double e,
                        const SgVlbiStationInfo* stnInfo, const SgMJD& epoch);
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/**MTT Hydrostatic mapping Function by Herring, 1992.
 *
 */
/*=====================================================================================================*/
class SgDryMF_MTT : public SgMappingFunction
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  SgDryMF_MTT() : SgMappingFunction() {};
  ~SgDryMF_MTT() {};


  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return value of mapping function.
   */
  virtual double calc(const SgMeteoData& meteo, double e, const SgVlbiStationInfo* stnInfo, 
    const SgMJD& epoch);
};
/*=====================================================================================================*/



/*=====================================================================================================*/
/**MTT Wet mapping Function by Herring, 1992.
 *
 */
/*=====================================================================================================*/
class SgWetMF_MTT : public SgMappingFunction
{
public:
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  SgWetMF_MTT() : SgMappingFunction() {};
  ~SgWetMF_MTT() {};

  /**Calculates mapping function.
   * \param meteo is a meteodata;
   * \param e is a source elevation angle, rad;
   * \param stnInfo is a pointer to the SgStationInfo;
   * \param epoch is an epoch of observations.
   * \return value of mapping function.
   */
  virtual double calc(const SgMeteoData& meteo, double e, const SgVlbiStationInfo* stnInfo, 
    const SgMJD& epoch);
};
/*=====================================================================================================*/










/*=====================================================================================================*/
/*                                                                                                     */
/* SgMappingFunction inline members:                                                                   */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgMappingFunction::SgMappingFunction() 
{
  m_ = dM_dE_ = 0.0;
};



// A destructor:
inline SgMappingFunction::~SgMappingFunction()
{
  // nothing to do
};



//
// INTERFACES:
//

//
// FUNCTIONS:
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
#endif //SG_MAPPING_FUNCTION_H
