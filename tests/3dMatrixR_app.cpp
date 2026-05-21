#include <iostream>
#include <complex>
#include <math.h>
#include <map>
#include <config.h>


#include <Sg3dVector.h>
#include <Sg3dMatrixR.h>

// HOPS's whims:
#ifdef OLD_HOPS
char                           *progname;
int                             msglev=2;
#endif

#include <SgConstants.h>
#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(UNUSED int argc, UNUSED char** argv)
{
 Sg3dVector        v(4.55, 5.128, -7.77);
 Sg3dMatrixR       R1(X_AXIS);
 Sg3dMatrixR       R2(Y_AXIS);
 Sg3dMatrixR       R3(Z_AXIS);


 using namespace std;
 // cout.precision(14);
 // cout.setf(ios_base::floatfield);

 const char*         AxisNames[3] = {"X", "Y", "Z"};

//==================================================================================
 cout << "Pass one: Checking the interface, basic functions and matrix operations of the object." << endl;
 R1(M_PI/6.0);
 R2(M_PI/3.0);
 R3(M_PI/2.0);
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R1.axis()] << " on the angle "
      << R1.angle()*RAD2DEG << " is: \n"
      << R1 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R2.axis()] << " on the angle "
      << R2.angle()*RAD2DEG << " is: \n"
      << R2 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R3.axis()] << " on the angle "
      << R3.angle()*RAD2DEG << " is: \n"
      << R3 << endl;

 Sg3dMatrixR       R(R3);
 cout << "A copy of the last rotational matrix around axis "
      << AxisNames[(int)R.axis()] << " on the angle "
      << R.angle()*RAD2DEG << " is: \n" 
      << R << endl;

 Sg3dMatrix       M;
 M = R1*R2*R3;
 cout << "A product of the vector " << v << " and rotational matrix \n"
      << M << "\n is " << M*v << endl << endl;

 cout << "Now three matrices are: \n" << R1(M_PI_2) << "\n" << R2(-M_PI_2) 
      << "\n" << R3(M_PI_2) << endl;
 Sg3dVector v0(1.0, 0, 0);
 v = R2*(R1*(R3*v0));
 cout << "The vector " << v0 << " becames " << v << endl;

 cout << "End of the pass one." << endl << endl;
 return 0;
};
