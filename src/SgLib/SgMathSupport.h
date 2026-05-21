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

#ifndef SG_MATH_DEFINITIONS_H
#define SG_MATH_DEFINITIONS_H


#include <math.h>
#include <complex>

#include <QtCore/QString>


class SgMJD;
class Sg3dVector;
class Sg3dMatrix;




enum DIRECTION {X_AXIS=0, VERTICAL=0, Y_AXIS=1, EAST=1, Z_AXIS=2, NORTH=2};
enum FFT_Direction {FFT_Forward = 0, FFT_Inverse = 1};


//
// functions:
//

inline double signum(const double x) {return x<0.0 ? -1.0 : 1.0;};
inline double cpsign(const double a1, const double a2) {return fabs(a1)*signum(a2);};// copysign(a1, a2)?
inline void swap(double &a1, double &a2) {double tmp=a1; a1=a2; a2=tmp;};

// FFT support:
unsigned int reverseBitOrder(unsigned int n, unsigned int k);
void fft(std::complex<double> x_a[], std::complex<double> x_A[], unsigned int n, FFT_Direction dir);

// some geodetic stuff (replacements of IERS procedures)
//
void geocentric2geodetic(const Sg3dVector& r, double& latitude, double& longitude, double& height,
  bool useOldEllipsoid);
//
//
void calcCip2IAU1980(const SgMJD& epoch, double dX, double dY, double dPsi_1980, double dEps_1980,
  double dPsi_2000, double dEps_2000, double& diffPsi, double& diffEps);
//
void calcNutationFundArgs_IersConv2003(const SgMJD& tEpoch, double args[5]);
//
void calcNutationFundArgs_IersConv1996(const SgMJD& tEpoch, double args[5]);
//
void calcNutationFundArgs_IersStds1992(const SgMJD& tEpoch, double args[5]);









//
// constants:
//
// these constants are defined in GPGeoMath3dVector.cpp:
extern const Sg3dVector         v3Zero;
extern const Sg3dVector         v3Unit;

// these constants are defined in GPGeoMath3dMatrix.cpp:
extern const Sg3dMatrix         m3E;
extern const Sg3dMatrix         m3Zero;

// Imaginary unit:
extern const std::complex<double>
                                zI;







#endif //SG_MATH_DEFINITIONS_H
