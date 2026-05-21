#include <iostream>
#include <complex>
#include <math.h>
#include <map>
#include <config.h>


// HOPS's whims:
#ifdef OLD_HOPS
char                           *progname;
int                             msglev=2;
#endif


#include <Sg3dVector.h>
#include <Sg3dMatrixRdots.h>
#include <SgConstants.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(UNUSED int argc, UNUSED char** argv)
{
 Sg3dVector             v(4.55, 5.128, -7.77);

 const char*            AxisNames[3] = {"X", "Y", "Z"};

 using namespace std;
 // cout.precision(15);
 // cout.setf(ios_base::floatfield);


 { //one:
//==================================================================================
 cout << "Pass one: Checking the derivatives of rotation matrices." << endl;
 Sg3dMatrixRdot       R1(X_AXIS);
 Sg3dMatrixRdot       R2(Y_AXIS);
 Sg3dMatrixRdot       R3(Z_AXIS);
 R1(M_PI/6.0);
 R2(M_PI/3.0);
 R3(M_PI/2.0);
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R1.axis()] << " on the angle "
      << R1.angle()*RAD2DEG << " is: "
      << R1 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R2.axis()] << " on the angle "
      << R2.angle()*RAD2DEG << " is: "
      << R2 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R3.axis()] << " on the angle "
      << R3.angle()*RAD2DEG << " is: "
      << R3 << endl;

 Sg3dMatrixRdot       R(R3);
 cout << "A copy of the last rotational matrix around axis "
      << AxisNames[(int)R.axis()] << " on the angle "
      << R.angle()*RAD2DEG << " is: " 
      << R << endl;

 Sg3dMatrix       M;
 M = R1*R2*R3;
 cout << "A product of the vector " << v << " and rotational matrix " 
      << M << " is " << M*v << endl << endl;

 cout << "Now three matrices are: " << R1(M_PI_2) << R2(-M_PI_2) << R3(M_PI_2) << endl;
 v = Sg3dVector(1,0,0);
 cout << "The vector " << v << " becames " << R2*(R1*(R3*v)) << endl;

 cout << "End of the pass one." << endl << endl;
//==================================================================================
 };


 { //two:
//==================================================================================
 cout << "Pass two: Checking the second derivatives of rotation matrices." << endl;
 Sg3dMatrixR2dot       R1(X_AXIS);
 Sg3dMatrixR2dot       R2(Y_AXIS);
 Sg3dMatrixR2dot       R3(Z_AXIS);

 R1(M_PI/6.0);
 R2(M_PI/3.0);
 R3(M_PI/2.0);
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R1.axis()] << " on the angle "
      << R1.angle()*RAD2DEG << " is: "
      << R1 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R2.axis()] << " on the angle "
      << R2.angle()*RAD2DEG << " is: "
      << R2 << endl;
 cout << "A rotational matrix around axis "
      << AxisNames[(int)R3.axis()] << " on the angle "
      << R3.angle()*RAD2DEG << " is: "
      << R3 << endl;

 Sg3dMatrixR2dot       R(R3);
 cout << "A copy of the last rotational matrix around axis "
      << AxisNames[(int)R.axis()] << " on the angle "
      << R.angle()*RAD2DEG << " is: "
      << R << endl;

 Sg3dMatrix       M;
 M = R1*R2*R3;
 cout << "A product of the vector " << v << " and rotational matrix "
           << M << " is " << M*v << endl << endl;

 cout << "Now three matrices are: " << R1(M_PI_2) << R2(-M_PI_2) << R3(M_PI_2) << endl;
 v = Sg3dVector(1,0,0);
 cout << "The vector " << v << " becames " << R2*(R1*(R3*v)) << endl;

 cout << "End of the pass one." << endl << endl;
//==================================================================================
 };

 return 0;
};

