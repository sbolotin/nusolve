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


#include <math.h>


#include <SgConstants.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgMappingFunction.h>
#include <SgVlbiStationInfo.h>


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgMappingFunction::className()
{
  return "SgMappingFunction";
};



//
double SgMappingFunction::calc(const SgMeteoData&, double e,
  const SgVlbiStationInfo* stnInfo, const SgMJD& epoch)
{
  double                        height(stnInfo->getHeight());
  m_ = 1.0;
  dM_dE_ = 1.0;
  
  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString                     str;
    str.sprintf("Mapping Function       (%s): lat.=%12.6f(d), elev.=%16.12f(d), h=%12.6f(m); epoch: %s",
      qPrintable(stnInfo->getKey()), stnInfo->getLatitude()*RAD2DEG, e*RAD2DEG, height,
      qPrintable(epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS)));
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Mapping Function result(%s): M=%20.16f",
      qPrintable(stnInfo->getKey()), m_);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };

  return m_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgDryMF_NMF:
* 
*======================================================================================================*/
//
// static first:
const QString SgDryMF_NMF::className()
{
  return "SgDryMF_NMF";
};



//
double SgDryMF_NMF::linterpolate(double x1, double x2, double y1, double y2, double x)
{
  double a = (y2-y1)/(x2-x1);
  double b = y1 - a*x1;
  return a*x + b;
};



//
double SgDryMF_NMF::calc(const SgMeteoData&, double e,
  const SgVlbiStationInfo* stnInfo, const SgMJD& epoch)
{
  double        a_dry_avg, a_dry_amp, b_dry_avg, b_dry_amp, c_dry_avg, c_dry_amp;
  double        phi(fabs(stnInfo->getLatitude()));

  /**A.E. Niell, "Global mapping functions for the atmosphere delay at radio wavelengths",
   * J.G.R. 101, b2, 3227-3246, 1996 (Equations 4 and 5 corrected)
   *
   * available on <http://web.haystack.mit.edu/Geodesy/GeodesyIndex.html>, thanks to Arthur
   *
   */
  double                        height(stnInfo->getHeight());
  double                        beta, gamma;
  double                        q;
  double                        deltaM, d_deltaM_dx;
  double                        sinE=sin(e);
  double                        cosE=cos(e);
  double                        a=2.53e-5;
  double                        b=5.49e-3;
  double                        c=1.14e-3;

  beta   = b/(sinE + c );
  gamma  = a/(sinE + beta);
  q = 1.0 + a/(1.0 + b/(1.0 + c));

  deltaM = (1.0/sinE - q/(sinE + gamma))*height/1000.0;
  
  d_deltaM_dx = (-cosE/sinE/sinE - 
      -q*cosE/(sinE + gamma)/(sinE + gamma)*
      (1.0 - a/(sinE + beta)/(sinE + beta)*(1.0 - b/(sinE + c)/(sinE + c))) )*height/1000.0;
  
  if (phi <= modelArg_[0])
  {
    a_dry_avg = modelA_avg_[0];
    a_dry_amp = modelA_amp_[0];
    b_dry_avg = modelB_avg_[0];
    b_dry_amp = modelB_amp_[0];
    c_dry_avg = modelC_avg_[0];
    c_dry_amp = modelC_amp_[0];
  }
  else if (phi >= modelArg_[4])
  {
    a_dry_avg = modelA_avg_[4];
    a_dry_amp = modelA_amp_[4];
    b_dry_avg = modelB_avg_[4];
    b_dry_amp = modelB_amp_[4];
    c_dry_avg = modelC_avg_[4];
    c_dry_amp = modelC_amp_[4];
  }
  else
  {
    int i = 0;
    while (++i<4 && phi>modelArg_[i]) ;

    a_dry_avg = linterpolate(modelArg_[i-1], modelArg_[i], modelA_avg_[i-1], modelA_avg_[i], phi);
    a_dry_amp = linterpolate(modelArg_[i-1], modelArg_[i], modelA_amp_[i-1], modelA_amp_[i], phi);
    b_dry_avg = linterpolate(modelArg_[i-1], modelArg_[i], modelB_avg_[i-1], modelB_avg_[i], phi);
    b_dry_amp = linterpolate(modelArg_[i-1], modelArg_[i], modelB_amp_[i-1], modelB_amp_[i], phi);
    c_dry_avg = linterpolate(modelArg_[i-1], modelArg_[i], modelC_avg_[i-1], modelC_avg_[i], phi);
    c_dry_amp = linterpolate(modelArg_[i-1], modelArg_[i], modelC_amp_[i-1], modelC_amp_[i], phi);
  };
  
  // Arthur's notation:
  double                        cost;
  double                        t_doy = epoch.calcDayOfYear() + epoch.getTime();
  //double                      t_doy(epoch - SgMJD(1980, 1, 0)); // <- CALC's

  if (stnInfo->getLatitude() < 0.0) // !
    t_doy += 365.25/2.0;
  t_doy -= 28.0;
  // cost = cos(t_doy*2.0*3.14159265/365.25);
  cost = cos(t_doy*2.0*M_PI/365.25);

  a = a_dry_avg - a_dry_amp*cost;
  b = b_dry_avg - b_dry_amp*cost;
  c = c_dry_avg - c_dry_amp*cost;

  beta   = b/(sinE + c);
  gamma  = a/(sinE + beta);
  q = 1.0 + a/(1.0 + b/(1.0 + c));
  
  m_ = q/(sinE + gamma) + deltaM;
 
  dM_dE_ = -q/(sinE + gamma)/(sinE + gamma) *
    (cosE - a/(sinE + beta)/(sinE + beta)*cosE*(1.0 - b/(sinE + c)/(sinE + c))) + d_deltaM_dx;


  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Mapping Function       (%s): lat.=%12.6f(d), elev.=%16.12f(d), h=%12.6f(m); epoch: %s",
      qPrintable(stnInfo->getKey()), stnInfo->getLatitude()*RAD2DEG, e*RAD2DEG, height,
      qPrintable(epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS)));
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Mapping Function       (%s): doy = %16.12f, cost = %16.12f",
      qPrintable(stnInfo->getKey()), t_doy, cost);
    str.sprintf("Mapping Function       (%s): a = %16.12f, b = %16.12f, c = %16.12f",
      qPrintable(stnInfo->getKey()), a, b, c);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Mapping Function result(%s): M=%20.16f",
      qPrintable(stnInfo->getKey()), m_);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return m_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgWetMF_NMF:
* 
*======================================================================================================*/
//
// static first:
const QString SgWetMF_NMF::className()
{
  return "SgWetMF_NMF";
};



//
double SgWetMF_NMF::linterpolate(double x1, double x2, double y1, double y2, double x)
{
  double a = (y2-y1)/(x2-x1);
  double b = y1 - a*x1;
  return a*x + b;
};



//
double SgWetMF_NMF::calc(const SgMeteoData&, double e,
  const SgVlbiStationInfo* stnInfo, const SgMJD&)
{
  double                        a, b, c, q;
  double                        phi=fabs(stnInfo->getLatitude());

 /**A.E. Niell, "Global mapping functions for the atmosphere delay at radio wavelengths",
  * J.G.R. 101, b2, 3227-3246, 1996 (Equations 4 and 5 corrected)
  *
  * available on <http://web.haystack.mit.edu/Geodesy/GeodesyIndex.html>, thanks to Arthur
  *
  */
  if (phi <= modelArg_[0])
  {
    a = modelA_avg_[0];
    b = modelB_avg_[0];
    c = modelC_avg_[0];
  }
  else if (phi >= modelArg_[4])
  {
    a = modelA_avg_[4];
    b = modelB_avg_[4];
    c = modelC_avg_[4];
  }
  else
  {
    int                         i=0;
    while (++i<4 && phi>modelArg_[i]);

    a = linterpolate(modelArg_[i-1], modelArg_[i], modelA_avg_[i-1], modelA_avg_[i], phi);
    b = linterpolate(modelArg_[i-1], modelArg_[i], modelB_avg_[i-1], modelB_avg_[i], phi);
    c = linterpolate(modelArg_[i-1], modelArg_[i], modelC_avg_[i-1], modelC_avg_[i], phi);
  };
  
  double                        beta, gamma;
  double                        sinE=sin(e);
  double                        cosE=cos(e);
  
  beta  = b/(sinE + c);
  gamma = a/(sinE + beta);
  q     = 1.0 + a/(1.0 + b/(1.0 + c));

  m_ = q/(sinE + gamma);

  dM_dE_ = -q/(sinE + gamma)/(sinE + gamma)*
     (cosE - a/(sinE + beta)/(sinE + beta)*cosE*(1.0 - b/(sinE + c)/(sinE + c)));
  
  if (logger->isEligible(SgLogger::DBG, SgLogger::REFRACTION))
  {
    QString str;
    str.sprintf("Mapping Function       (%s): lat.=%12.6f(d), elev.=%16.12f(d)",
      qPrintable(stnInfo->getKey()), stnInfo->getLatitude()*RAD2DEG, e*RAD2DEG);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
    str.sprintf("Mapping Function result(%s): M=%20.16f",
      qPrintable(stnInfo->getKey()), m_);
    logger->write(SgLogger::DBG, SgLogger::REFRACTION, className() + ": " + str);
  };
  return m_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgGradMF_CH:
* 
*======================================================================================================*/
//
// static first:
const QString SgGradMF_CH::className()
{
  return "SgGradMF_CH";
};



//
double SgGradMF_CH::calc(const SgMeteoData&, double e, const SgVlbiStationInfo*, const SgMJD&)
{
  /**
   * IERS Technical Note 2?,
   * IERS Conventions (2003), Chapter 9, page 2:
   *
   * "Chen and Herring (1997) propose using $m_g(e) = 1/(\sin e\tan e + 0.0032)$. The various forms 
   * agree to within 10% for elevation angles higher then $10^\circ$, but the the differences reach 
   * 50% for $5^\circ$ elevation due to the singularity of the $\cot(e)$ function."
   */
  
  m_ = 1.0/(sin(e)*tan(e) + 0.0032);
  return m_;
};
/*=====================================================================================================*/







/*=======================================================================================================
*
*  class SgDryMF_MMT implementation
*
=======================================================================================================*/
//
// static first:
const QString SgDryMF_MTT::className()
{
  return "SgDryMF_MTT";
};



//
double SgDryMF_MTT::calc(const SgMeteoData& meteo, double e, const SgVlbiStationInfo* stnInfo, 
    const SgMJD& epoch)
{
  double                        cosPhi, sinE, height, a, b, c, tempC;
  tempC  = meteo.getTemperature();
  cosPhi = cos(stnInfo->getLatitude());
  height = stnInfo->getHeight()/1000.0; // height in km
  
 /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
  * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
  * October 1994. Page 27, Eq. (48):
  *
  */
  a = ( 1.2320 + 0.0139*cosPhi - 0.0209*height + 0.00215*(tempC - 10.0))*1.0e-3;
  b = ( 3.1612 - 0.1600*cosPhi - 0.0331*height + 0.00206*(tempC - 10.0))*1.0e-3;
  c = (71.244  - 4.293 *cosPhi - 0.149 *height - 0.0021 *(tempC - 10.0))*1.0e-3;

  sinE = sin(e);
  m_ = (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));

  return m_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*  class SgDryMF_MTT implementation
*
=======================================================================================================*/
//
// static first:
const QString SgWetMF_MTT::className()
{
  return "SgWetMF_MTT";
};



//
double SgWetMF_MTT::calc(const SgMeteoData& meteo, double e, const SgVlbiStationInfo* stnInfo, 
    const SgMJD& epoch)
{
  double                        cosPhi, sinE, height, a, b, c, tempC;
  tempC  = meteo.getTemperature();
  cosPhi = cos(stnInfo->getLatitude());
  height = stnInfo->getHeight()/1000.0; // height in km
  
 /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
  * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
  * October 1994. Page 27, Eq. (48):
  *
  */
  a = ( 0.583  - 0.011 *cosPhi - 0.052 *height + 0.0014*(tempC - 10.0))*1.0e-3;
  b = ( 1.402  - 0.102 *cosPhi - 0.101 *height + 0.0020*(tempC - 10.0))*1.0e-3;
  c = (45.85   - 1.91  *cosPhi - 1.29  *height + 0.015 *(tempC - 10.0))*1.0e-3;

  sinE = sin(e);
  m_ = (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
  return m_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/







/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/
// SgDryMF_NMF's:
const double SgDryMF_NMF::modelArg_[5]= 
{
  15.0*DEG2RAD, 30.0*DEG2RAD, 45.0*DEG2RAD, 60.0*DEG2RAD, 75.0*DEG2RAD
};

const double SgDryMF_NMF::modelA_avg_[5]=
{
  1.2769934e-3, 1.2683230e-3, 1.2465397e-3, 1.2196049e-3, 1.2045996e-3
};

const double SgDryMF_NMF::modelB_avg_[5]=
{
  2.9153695e-3, 2.9152299e-3, 2.9288445e-3, 2.9022565e-3, 2.9024912e-3
};

const double SgDryMF_NMF::modelC_avg_[5]=
{
  62.610505e-3, 62.837393e-3, 63.721774e-3, 63.824265e-3, 64.258455e-3
};

const double SgDryMF_NMF::modelA_amp_[5]=
{
  0.0, 1.2709626e-5, 2.6523662e-5, 3.4000452e-5, 4.1202191e-5
};

const double SgDryMF_NMF::modelB_amp_[5]=
{
  0.0, 2.1414979e-5, 3.0160779e-5, 7.2562722e-5, 11.723375e-5
};

const double SgDryMF_NMF::modelC_amp_[5]=
{
  0.0, 9.0128400e-5, 4.3497037e-5, 84.795348e-5, 170.37206e-5
};



// SgWetMF_NMF's:
const double SgWetMF_NMF::modelArg_[5]=
{
  15.0*DEG2RAD, 30.0*DEG2RAD, 45.0*DEG2RAD, 60.0*DEG2RAD, 75.0*DEG2RAD
};

const double SgWetMF_NMF::modelA_avg_[5]=
{
  5.8021897e-4, 5.6794847e-4, 5.8118019e-4, 5.9727542e-4, 6.1641693e-4
};

const double SgWetMF_NMF::modelB_avg_[5]=
{
  1.4275268e-3, 1.5138625e-3, 1.4572752e-3, 1.5007428e-3, 1.7599082e-3
};

const double SgWetMF_NMF::modelC_avg_[5]=
{
  4.3472961e-2, 4.6729510e-2, 4.3908931e-2, 4.4626982e-2, 5.4736038e-2
};
/*=====================================================================================================*/
