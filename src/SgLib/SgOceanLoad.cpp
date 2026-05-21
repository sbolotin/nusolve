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
#include <SgOceanLoad.h>
#include <SgTaskConfig.h>
#include <SgVlbiStationInfo.h>


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgOceanLoad::className()
{
  return "SgOceanLoad";
};



//
SgOceanLoad::SgOceanLoad(const SgTaskConfig* cfg)
{
  cfg_ = cfg;
  for (int i=0; i<11; i++)
    angle[i] = 0.0;
};



// A destructor:
SgOceanLoad::~SgOceanLoad()
{
};



//
void SgOceanLoad::calcArgs(const SgMJD& time)
{
  // 
  // Ernst W. Schwiderski, U.S. Naval Surface Weapons Center
  // "Atlas of Ocean Tidal Charts and Maps, Part I: The Semidiurnal Principal Lunar Tide M2"
  // Marine Geodesy, Volume 6, Number 3-4
  // page 226:
  int                           nD;   // D = day number of year Y(D = I for January 1)
  double                        t;    // t = universal standard time of day D (in sec)
  double                        h0;   // mean longitude of the sun relative to Greenwich 
                                      // midnight of day D (in deg)
  double                        s0;   // mean longitude of the moon relative to Greenwich
                                      // midnight of day D (in deg)
  double                        p0;   // lunar perigee, from page 254
  double                        dT, dT2, dT3;
  
  nD = time.calcDayOfYear();
  dT = (27392.500528 + 1.0000000356*nD)/36525.0;
  dT2 = dT*dT;
  dT3 = dT2*dT;
  t = time.getTime()*86400.0;
  
  h0 =  279.69668 + 36000.768930485*dT2 + 3.03e-4*dT3;  // the second term is corrected according to 
                                                        // the recent IERS Conventions
//h0 =  279.69668 + 36000.768925485*dT2 + 3.03e-4*dT3;
  //
  s0 = 270.434358 + 481267.88314137*dT - 0.001133*dT2 + 1.9e-6*dT3;
  //
  p0 = 334.329653 + 4069.0340329577*dT - 0.010325*dT2 - 1.2e-5*dT3;   // the second term is corrected
                                                                      // according to the recent IERS 
                                                                      // Conventions
//p0 = 334.329653 + 4069.0340329575*dT - 0.010325*dT2  - 1.2e-5*dT3;
  //
  // convert to radians:
  h0 *= DEG2RAD;
  s0 *= DEG2RAD;
  p0 *= DEG2RAD;
  //
  for (int k=0; k<11; k++)
  {
    angle[k] = freq[k]*t + 
      argNumbers[0][k]*h0 + argNumbers[1][k]*s0 + argNumbers[2][k]*p0 + argNumbers[3][k]*2.0*M_PI;
    angle[k] = fmod(angle[k], 2.0*M_PI);
    if (angle[k]<0.0)
      angle[k] = angle[k] + 2.0*M_PI;
  };
};



//
const Sg3dVector& SgOceanLoad::calcDisplacement(const SgVlbiStationInfo* stnInfo, const SgMJD& t)
{
  clear();
  if (!stnInfo)
  {
    logger->write(SgLogger::ERR, SgLogger::DISPLACEMENT, className() +
      ": calcDisplacement(): stnInfo is NULL");
    return *this;
  };
  
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 53 says:
    "   A shorter form of (2) is obtained if the summation considers only the tidal 
    species of Table 7.1 and corrections for the modulating effect of the lunar node.
    Then, 
    
    \Delta c = \Sum ( f_j*A_cj*cos(omega_j*t + hi_j + u_j - Phi_cj) ),  (4)
    
    where f_j and u_j depend on longitude of lunar node according to Table 26 of 
    Doodson (1928)."
  */
  double        fS1N, fC1N, fS2N, fC2N, fS3N, fC3N;
  double        f[11], u[11], fTmp=0.0;
  double        dt  = (t - tEphem)/36525.0;
  double        dt2 = dt*dt;
  double        dt3 = dt2*dt;
  double        dt4 = dt2*dt2;
  fTmp = 125.04455501*3600.0 - 6962890.5431*dt + 7.4722*dt2 + 0.007702*dt3 - 0.00005939*dt4;
  fTmp = fmod(fTmp*SEC2RAD, 2.0*M_PI);

  for (int i=0; i<11; i++) 
  {
    f[i] = 1.0;
    u[i] = 0.0;
  };
  
  sincos (    fTmp,  &fS1N, &fC1N);
  sincos (2.0*fTmp,  &fS2N, &fC2N);
  sincos (3.0*fTmp,  &fS3N, &fC3N);
  
  f[ 0] = 1.0004 - 0.0373*fC1N + 0.0002*fC2N              ;
  f[ 3] = 1.0241 + 0.2863*fC1N + 0.0083*fC2N - 0.0015*fC3N;
  f[ 4] = 1.0060 + 0.1150*fC1N - 0.0088*fC2N + 0.0006*fC3N;
  f[ 5] = 1.0089 + 0.1871*fC1N - 0.0147*fC2N + 0.0014*fC3N;
  f[ 8] = 1.0429 + 0.4135*fC1N - 0.0040*fC2N              ;
  f[ 9] = 1.0000 - 0.1300*fC1N + 0.0013*fC2N              ;
  
  u[ 0] = (      - 2.14  *fS1N                            )*DEG2RAD;
  u[ 3] = (      -17.74  *fS1N + 0.68  *fS2N - 0.04  *fS3N)*DEG2RAD;
  u[ 4] = (      - 8.86  *fS1N + 0.68  *fS2N - 0.07  *fS3N)*DEG2RAD;
  u[ 5] = (       10.80  *fS1N - 1.34  *fS2N + 0.19  *fS3N)*DEG2RAD;
  u[ 8] = (      -23.74  *fS1N + 2.68  *fS2N - 0.38  *fS3N)*DEG2RAD;
  
  /*
    "The astronomical arguments needed in (4) can be computed with subroutune ARG below."
  */
  calcArgs(t);

  // summation:
  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      vec[i] += f[j]*stnInfo->getOLoadAmplitude(j,i)*cos(angle[j] + u[j] - stnInfo->getOLoadPhase(j,i));
  
  //make it in VEN:
  vec[1] *= -1.0;
  vec[2] *= -1.0;
  return *this;
};



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
const double SgOceanLoad::freq[11]= // frequencies of the tides, rad per sec:
{
  1.40519e-4, 1.45444e-4, 1.37880e-4, 1.45842e-4, 0.72921e-4,
  0.67598e-4, 0.72523e-4, 0.64959e-4, 0.53234e-5, 0.26392e-5, 0.03982e-5
};


const double SgOceanLoad::argNumbers[4][11]=
{
  {  2.0, 0.0, 2.0, 2.0, 1.0 , 1.0 ,-1.0 , 1.0 , 0.0, 0.0, 2.0},
  { -2.0, 0.0,-3.0, 0.0, 0.0 ,-2.0 , 0.0 ,-3.0 , 2.0, 1.0, 0.0},
  {  0.0, 0.0, 1.0, 0.0, 0.0 , 0.0 , 0.0 , 1.0 , 0.0,-1.0, 0.0},
  {  0.0, 0.0, 0.0, 0.0, 0.25,-0.25,-0.25,-0.25, 0.0, 0.0, 0.0}
};

const SgMJD SgOceanLoad::_1975(1975, 1, 0); 





/*=====================================================================================================*/
