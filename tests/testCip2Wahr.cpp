#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

//#include <SgMJD.h>


#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


#ifdef OLD_HOPS
char                            progname[80];
int                             msglev=2;
#endif





//
void calcCip2IAU1980_New(const SgMJD& epoch, double dX, double dY, double dPsi_1980, double dEps_1980,
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

std::cout << " ++ New: psi_A = " << psi_A*RAD2MAS << "   chi_A = " << chi_A*RAD2MAS << "  f2= " << f2 << " sineps_A " << sineps_A << "\n";
std::cout << " ++ New: diffPsi(1) = " << diffPsi*RAD2MAS << "   diffEps(1) = " << diffEps*RAD2MAS << "\n";

  // IERS Conventions (2010), IERS TN #36, page 50 eqs. (5.25),
  // Express delta Psi and delta Eps as functions of dX and dY:
  diffPsi+= (dX - f*dY)/(f2 + 1.0)/sineps_A;
  diffEps+= (f*dX + dY)/(f2 + 1.0);
std::cout << " ++ New: diffPsi(2) = " << diffPsi*RAD2MAS << "   diffEps(2) = " << diffEps*RAD2MAS << "\n";

  // Adjust for differences in precession rates IAU 1976 -> IAU 2006:
  // IERS Conventions (2010), IERS TN #36, page 55:
//  diffPsi+= -0.29965/dt1*SEC2RAD;
//  diffEps+= -0.02524/dt1*SEC2RAD;
};




void calcCip2IAU1980_Old(const SgMJD& epoch, double dX, double dY, double dPsi_1980, double dEps_1980,
  double dPsi_2000, double dEps_2000, double& diffPsi, double& diffEps)
{
  double const                  sineps0(0.3977771559319137), coseps0(0.9174820620691818);
  double                        dPsi_model, dEps_model, dt, dt2, dt3, psi_A, chi_A, f;
  dPsi_model = (dPsi_2000 - dPsi_1980)*RAD2MAS;
  dEps_model = (dEps_2000 - dEps_1980)*RAD2MAS;
  dt = (epoch - tEphem)/36525.0;
  dt2 = dt*dt;
  dt3 = dt2*dt;

  // ! Luni-solar precession
  psi_A = (5038.47875*dt - 1.07259*dt2 -0.001147*dt3)*SEC2RAD;
  // ! Planetary precession
  chi_A = (  10.5526 *dt - 2.38064*dt2 -0.001125*dt3)*SEC2RAD;
  f = (psi_A*coseps0 - chi_A)*(psi_A*coseps0 - chi_A);

std::cout << " ++ Old: psi_A = " << psi_A*RAD2MAS << "   chi_A = " << chi_A*RAD2MAS << "  f2= " << f << "  sineps0 " << sineps0 << "\n";
std::cout << " ++ Old: diffPsi(1) = " << dPsi_model << "   diffEps(1) = " << dEps_model << "\n";

  // ! dpsi1980 / deps1980
  diffPsi = (-dX + (psi_A*coseps0 - chi_A)*dY)/(-f*sineps0 - sineps0) + dPsi_model;
  diffEps = (-(psi_A*coseps0 - chi_A)*sineps0*dX - sineps0*dY)/(-f*sineps0 - sineps0) + dEps_model;

std::cout 
<< " ++ Old: diffPsi(.) = " << (-dX + (psi_A*coseps0 - chi_A)*dY)/(-f*sineps0 - sineps0) 
<< "   diffEps(.) = "       << (-(psi_A*coseps0 - chi_A)*sineps0*dX - sineps0*dY)/(-f*sineps0 - sineps0) << "\n";

std::cout << " ++ Old: diffPsi(2) = " << diffPsi << "   diffEps(2) = " << diffEps << "\n";

//  diffPsi+= (-2.9965)*dt*100.0; // values of Herring / UAI 2000
//  diffEps+= (-0.2524)*dt*100.0; // values of Herring / UAI 2000

//  diffPsi+= - 41.7750 + 40.0e-3; // values of Herring / UAI 2000 corrigees
//  diffEps+= -  6.8192 - 40.0e-3; // values of Herring / UAI 2000 corrigees
  diffPsi/= RAD2MAS; // -> rad
  diffEps/= RAD2MAS; // -> rad
};






















//
//
//
//
int main(int argc, char **argv)
{
 SgMJD                                  t;
 double                                 dX, dY, dPsi_1980, dEps_1980, dPsi_2000, dEps_2000;
 double                                 diffPsiO, diffEpsO, diffPsiN, diffEpsN;

// t = SgMJD(2020, 6, 15, 12, 30, 0.0);
 t = SgMJD(2000, 6, 15, 12, 30, 0.0);

 dX = 1.0*SEC2RAD/1000.0;
 dY = -.5*SEC2RAD/1000.0;
 dPsi_1980 = 22.22*SEC2RAD/1000.0;
 dEps_1980 = 11.11*SEC2RAD/1000.0;
 dPsi_2000 = 22.22*SEC2RAD/1000.0;
 dEps_2000 = 11.11*SEC2RAD/1000.0;


 calcCip2IAU1980_New(t, dX, dY, dPsi_1980, dEps_1980, dPsi_2000, dEps_2000, diffPsiN, diffEpsN);
 calcCip2IAU1980_Old(t, dX, dY, dPsi_1980, dEps_1980, dPsi_2000, dEps_2000, diffPsiO, diffEpsO);

 printf("%s ddPsi=%14.8f ddEps=%14.8f\n", qPrintable(t.toString()), (diffPsiN - diffPsiO)*RAD2MAS, (diffEpsN - diffEpsO)*RAD2MAS);



/*
 for (int i=0; i<20; i++)
 {
    t = t + 1.0;
    calcCip2IAU1980_New(t, dX, dY, dPsi_1980, dEps_1980, dPsi_2000, dEps_2000, diffPsiN, diffEpsN);
    calcCip2IAU1980_Old(t, dX, dY, dPsi_1980, dEps_1980, dPsi_2000, dEps_2000, diffPsiO, diffEpsO);
    printf("%s ddPsi=%14.8f ddEps=%14.8f\n", qPrintable(t.toString()), (diffPsiN - diffPsiO)*RAD2MAS, (diffEpsN - diffEpsO)*RAD2MAS);

 };

*/

 return 0;
}


