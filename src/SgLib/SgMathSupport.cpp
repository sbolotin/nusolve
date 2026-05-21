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

#include <iostream>
#include <stdlib.h>



//#include <SgLogger.h>
#include <SgConstants.h>
#include <SgMathSupport.h>
#include <SgMJD.h>
#include <Sg3dVector.h>





#define SWAP(a, b) {typeof(a) t; t = a; a = b; b = t;}




/*=====================================================================================================*/
//
// just aux functions:
//
//
/*=====================================================================================================*/
//
unsigned int reverseBitOrder(unsigned int n, unsigned int k) // n = 0...2^k
{
  unsigned int                  x=0;
  const unsigned int            mask=1;
  while (k > 0)
  {
    x = x << 1;
    if ( mask & n )
      x = x | mask;
    n = n >> 1;
    k--;
  }
  return x;
};



// FFT:
// Radix-2 Cooley-Tukey algorythm is available at:
// https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm
void fft(std::complex<double> x_a[], std::complex<double> x_A[], unsigned int n, FFT_Direction dir)
{
  unsigned int                  log2n; // log2(n)
  unsigned int                  m, hm;
  std::complex<double>          omega, omega_m;
  std::complex<double>          t, u;
  std::complex<double>          a;
  a = dir==FFT_Forward?-2.0*M_PI*zI:2.0*M_PI*zI;

  log2n = 0;
  while (1u<<log2n<n && log2n<32u)
    log2n++;

  if (1u<<log2n != n)
  {
    std::cerr << "iterative_fft: number of points, " << n << ", is not a power of 2.\n";
    return;
  };

  for (unsigned int i=0; i<n; i++)
    x_A[reverseBitOrder(i, log2n)] = x_a[i];

  for (unsigned int s=1; s<=log2n; s++)
  {
    m = 1 << s;
    hm = m >> 1;
    omega_m = std::exp(a/((double)m));
    for (unsigned int k=0; k<n; k+=m)
    {
      omega = std::complex<double>(1.0, 0.0);
      for (unsigned int j=0; j<hm; j++)
      {
        t = omega*x_A[k + j + hm];
        u = x_A[k + j];
        x_A[k + j] = u + t;
        x_A[k + j + hm] = u - t;
        omega *= omega_m;
      };
    };
  };
  if (dir == FFT_Inverse)
    for (unsigned int i=0; i<n; i++)
      x_A[i] /= n;
};



// The algorithm is from
// T. Fukushima, "Transformation from Cartesian to geodetic coordinates accelerated by Halley’s method",
// February 2006 Journal of Geodesy 79(12):689-693
// 
void geocentric2geodetic(const Sg3dVector& r, double& latitude, double& longitude, double& height,
  bool useOldEllipsoid)
{
//useOldEllipsoid = false;
//useOldEllipsoid = true;

//Set up the geoid parameters:
  double                        a, f;
  a = 6378137.0;
  f = 0.00335281068118;
  
  if (useOldEllipsoid)
  {
    a = 6378136.3;
    f = 0.0033528918690;
  };
  double                        p, dP, ec, b, z, dZ, dE;
  double                        dC0, dC1, dS0, dS1;
  double                        dA0, dA1, dB0, dD0, dF0;

  dE = (2.0 - f)*f;
  ec = sqrt(1.0 - dE);
  b  = a*ec;
  z  = fabs(r.at(Z_AXIS));
  p  = hypot(r.at(X_AXIS), r.at(Y_AXIS));
  if (1.0e-3 < p)
  {
    dZ = z*ec/a;
    dP = p/a;
//  Initial values for C0 and S0, eq. (17):
    dC0 = ec*dP;
    dS0 = dZ;
//  The first iteration:
    dA0 = hypot(dC0, dS0);                                            // eq. (14)
    dB0 = 1.5*dE*dS0*dC0*dC0*( (dP*dS0 - dZ*dC0)*dA0 - dE*dS0*dC0);   // eq. (15)
    dD0 = dZ*dA0*dA0*dA0 + dE*dS0*dS0*dS0;                            // eq. (12)
    dF0 = dP*dA0*dA0*dA0 - dE*dC0*dC0*dC0;                            // eq. (13)
//  Next iteration:
    dS1 = dD0*dF0 - dB0*dS0;                                          // eq. (10)
    dC1 = dF0*dF0 - dB0*dC0;                                          // eq. (11)
    dA1 = hypot(dC1, dS1);                                            // eq. (14)
//  Calculate geodetic coordinates:
//  longitude:
    longitude = atan2(r.at(Y_AXIS), r.at(X_AXIS));
    if (longitude < 0.0)
      longitude += M_PI*2.0;
//  latitude:
    latitude = signum(r.at(Z_AXIS))*atan(dS1/dC1/ec);                 // eq. (19)
//  height above geoid:
    height = (p*dC1*ec + z*dS1 - b*dA1)/hypot(ec*dC1, dS1);           // eq. (20)
  }
  else // is it pole?
  {
    longitude = 0.0;
    latitude = M_PI_2*signum(r.at(Z_AXIS));
    height   = z - a*ec;
  };
};



//
void calcCip2IAU1980(const SgMJD& epoch, double dX, double dY, double dPsi_1980, double dEps_1980,
  double dPsi_2000, double dEps_2000, double& diffPsi, double& diffEps)
{
  double                        psi_A, chi_A, eps_A, eps_0;
  double                        dt1, dt2, dt3, dt4, dt5;
  double                        coseps_0;
  double                        sineps_A;
  double                        f, f2;
  dt1 = (epoch - tEphem)/36525.0;
  dt2 = dt1*dt1;
  dt3 = dt2*dt1;
  dt4 = dt2*dt2;
  dt5 = dt3*dt2;
  // Obliquity of the ecliptic at J2000.0:
  eps_0 = 84381.406;
  // IERS Conventions (2010), IERS TN #36 eqs. (5.39):
  psi_A = 5038.481507*dt1 - 1.0790069*dt2 - 0.00114045*dt3 + 0.000132851*dt4 - 0.0000000951*dt5;
  // IERS Conventions (2010), IERS TN #36 eqs. (5.40):
  chi_A = 10.556403*dt1 - 2.3814292*dt2 - 0.00121197*dt3 + 0.000170663*dt4 - 0.000000056*dt5;
  eps_A = eps_0 - 46.836769*dt1 - 0.0001831*dt2 + 0.0020034*dt3 - 0.000000576*dt4 - 0.0000000434*dt5;
  // convert to radians:
  eps_0 *= SEC2RAD;
  psi_A *= SEC2RAD;
  chi_A *= SEC2RAD;
  eps_A *= SEC2RAD;
  coseps_0 = cos(eps_0);
  sineps_A = sin(eps_A);

  f = f2 = psi_A*coseps_0 - chi_A;
  f2 *= f;

  diffPsi = dPsi_2000 - dPsi_1980;
  diffEps = dEps_2000 - dEps_1980;

  // IERS Conventions (2010), IERS TN #36, page 50 eqs. (5.25),
  // Express delta Psi and delta Eps as functions of dX and dY:
  diffPsi+= (dX - f*dY)/(f2 + 1.0)/sineps_A;
  diffEps+= (f*dX + dY)/(f2 + 1.0);

  // Adjust for differences in precession rates IAU 1976 -> IAU 2006:
  // IERS Conventions (2010), IERS TN #36, page 55:
  diffPsi+= -0.29965*dt1*SEC2RAD;
  diffEps+= -0.02524*dt1*SEC2RAD;

  // Adjust offset of CIP with respect to IAU 1976 Precession/Nutation origin:
  // N. Capitaine et al., "Expressions for IAU 2000 precession quantities",
  // A&A 412, 567–586 (2003), DOI: 10.1051/0004-6361:20031539
  // Eq (11):
  diffPsi+= (-0.0417750)*SEC2RAD;
  diffEps+= (-0.0068192)*SEC2RAD;
};



//
// Calculates the five nutation fundamental arguments according to the IERS Conventions 2003 / IERS TN32:
// (The same coeffs are in IERS Conventions 2010 / IERS TN36):
void calcNutationFundArgs_IersConv2003(const SgMJD& tEpoch, double args[5])
{
  // Page 48, eq. (40):
  const double cL  [5] = {134.96340251*DEG2SEC, 1717915923.2178, 31.8792, 0.051635, -0.00024470};
  const double cL0 [5] = {357.52910918*DEG2SEC,  129596581.0481, -0.5532, 0.000136, -0.00001149};
  const double cF  [5] = { 93.27209062*DEG2SEC, 1739527262.8478,-12.7512,-0.001037,  0.00000417};
  const double cD  [5] = {297.85019547*DEG2SEC, 1602961601.2090, -6.3706, 0.006593, -0.00003169};
  const double cOm [5] = {125.04455501*DEG2SEC,   -6962890.5431,  7.4722, 0.007702, -0.00005939};
  double                        t((tEpoch - tEphem)/36525.0); // centuries since Tephem
  double                        t2, t3, t4;
  double                        dL, dL0, dF, dD, dOm;
  t2  = t*t;
  t3  = t2*t;
  t4  = t2*t2;
  dL  = cL [0] + cL [1]*t  + cL [2]*t2  + cL [3]*t3  + cL [4]*t4;
  dL0 = cL0[0] + cL0[1]*t  + cL0[2]*t2  + cL0[3]*t3  + cL0[4]*t4;
  dF  = cF [0] + cF [1]*t  + cF [2]*t2  + cF [3]*t3  + cF [4]*t4;
  dD  = cD [0] + cD [1]*t  + cD [2]*t2  + cD [3]*t3  + cD [4]*t4;
  dOm = cOm[0] + cOm[1]*t  + cOm[2]*t2  + cOm[3]*t3  + cOm[4]*t4;

  args[0] = fmod(dL,  360.0*DEG2SEC)*SEC2RAD;
  args[1] = fmod(dL0, 360.0*DEG2SEC)*SEC2RAD;
  args[2] = fmod(dF,  360.0*DEG2SEC)*SEC2RAD;
  args[3] = fmod(dD,  360.0*DEG2SEC)*SEC2RAD;
  args[4] = fmod(dOm, 360.0*DEG2SEC)*SEC2RAD;
};



//
//
// Calculates the five nutation fundamental arguments according to the IERS Conventions 1996 / IERS TN21:
void calcNutationFundArgs_IersConv1996(const SgMJD& tEpoch, double args[5])
{
// Page 23:
  const double cL  [5] = {134.96340251*DEG2SEC, 1717915923.2178, 31.8792, 0.051635, -0.00024470};
  const double cL0 [5] = {357.52910918*DEG2SEC,  129596581.0481, -0.5532,-0.000136, -0.00001149};
  const double cF  [5] = { 93.27209062*DEG2SEC, 1739527262.8478,-12.7512,-0.001037,  0.00000417};
  const double cD  [5] = {297.85019547*DEG2SEC, 1602961601.2090, -6.3706, 0.006593, -0.00003169};
  const double cOm [5] = {125.04455501*DEG2SEC,   -6962890.2665,  7.4722, 0.007702, -0.00005939};

  double                        t((tEpoch - tEphem)/36525.0); // centuries since Tephem
  double                        t2, t3, t4;
  double                        dL, dL0, dF, dD, dOm;
  t2  = t*t;
  t3  = t2*t;
  t4  = t2*t2;
  dL  = cL [0] + cL [1]*t  + cL [2]*t2  + cL [3]*t3  + cL [4]*t4;
  dL0 = cL0[0] + cL0[1]*t  + cL0[2]*t2  + cL0[3]*t3  + cL0[4]*t4;
  dF  = cF [0] + cF [1]*t  + cF [2]*t2  + cF [3]*t3  + cF [4]*t4;
  dD  = cD [0] + cD [1]*t  + cD [2]*t2  + cD [3]*t3  + cD [4]*t4;
  dOm = cOm[0] + cOm[1]*t  + cOm[2]*t2  + cOm[3]*t3  + cOm[4]*t4;

  args[0] = fmod(dL,  360.0*DEG2SEC)*SEC2RAD;
  args[1] = fmod(dL0, 360.0*DEG2SEC)*SEC2RAD;
  args[2] = fmod(dF,  360.0*DEG2SEC)*SEC2RAD;
  args[3] = fmod(dD,  360.0*DEG2SEC)*SEC2RAD;
  args[4] = fmod(dOm, 360.0*DEG2SEC)*SEC2RAD;
};



//
//
// Calculates the five nutation fundamental arguments according to the IERS Standards 1992 / IERS TN13:
void calcNutationFundArgs_IersStds1992(const SgMJD& tEpoch, double args[5])
{
// Page 32:
  double                        t((tEpoch - tEphem)/36525.0); // centuries since Tephem
  double                        t2, t3, r;
  double                        dL, dL0, dF, dD, dOm;

  r   = 1296000.0; // sec in 2*M_PI
  t2  = t*t;
  t3  = t2*t;

  // Mean Anomaly of the Moon:
  dL  = (134.0*3600.0 + 57.0*60.0 + 46.733) +
    fmod((1325.0*r + 198.0*3600.0 + 52.0*60.0 + 2.633)*t, r) + 31.310*t2 + 0.064*t3;

  // Mean Anomaly of the Sun:
  dL0 = (357.0*3600.0 + 31.0*60.0 + 39.804) + 
    fmod((  99.0*r + 359.0*3600.0 +  3.0*60.0 + 1.224)*t, r) -  0.577*t2 - 0.012*t3;

  // L - Omega, L is Mean Longitude of the Moon:
  dF  = ( 93.0*3600.0 + 16.0*60.0 + 18.877) + 
    fmod((1342.0*r +  82.0*3600.0 +  1.0*60.0 + 3.137)*t, r) - 13.257*t2 + 0.011*t3;

  //Mean Elongation of the Moon from the Sun:
  dD  = (297.0*3600.0 + 51.0*60.0 +  1.307) + 
    fmod((1236.0*r + 307.0*3600.0 +  6.0*60.0 + 41.328)*t, r) -  6.891*t2 + 0.019*t3;

  // Mean Longitude of the Ascending Node of the Moon:
  dOm = (125.0*3600.0 +  2.0*60.0 + 40.280) - 
    fmod((   5.0*r + 134.0*3600.0 +  8.0*60.0 + 10.539)*t, r)  + 7.455*t2 + 0.008*t3;
                   
  args[0] = fmod(dL,  360.0*DEG2SEC)*SEC2RAD;
  args[1] = fmod(dL0, 360.0*DEG2SEC)*SEC2RAD;
  args[2] = fmod(dF,  360.0*DEG2SEC)*SEC2RAD;
  args[3] = fmod(dD,  360.0*DEG2SEC)*SEC2RAD;
  args[4] = fmod(dOm, 360.0*DEG2SEC)*SEC2RAD;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
//
// statics:
//
/*=====================================================================================================*/




/*=====================================================================================================*/
const std::complex<double>      zI(0.0, 1.0);
















