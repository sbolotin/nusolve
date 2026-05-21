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
#include <Sg3dMatrix.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(UNUSED int argc, UNUSED char** argv)
{
 Sg3dVector        v(11.7, 0.128, 3.54);
 Sg3dVector        v1;
 Sg3dMatrix        M;                         // first type of constructors
 Sg3dMatrix        M1(m3E);                   // second type of constructors
 Sg3dMatrix        M2(1,2,3,4,5,6,7,8,9);     // third type of constructors
 double              f = 0.0;


 using namespace std;
 // cout.precision(15);
 // cout.setf(ios_base::floatfield);

//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;
 cout << "A matrix M1 is: " << M1 << "and a matrix M2 is: " << M2 << endl;
 cout << "Makeing a copy of diagonal elements from M2 to M1." << endl;
 M1(X_AXIS, X_AXIS) = M2.at(X_AXIS, X_AXIS);
 M1(Y_AXIS, Y_AXIS) = M2.at(Y_AXIS, Y_AXIS);
 M1(Z_AXIS, Z_AXIS) = M2.at(Z_AXIS, Z_AXIS);
 cout << "The matrix M1 is: " << M1 << "and the matrix M2 is: " << M2 << endl;

 cout << "Makeing an assignment of a unit matrix (predefined constant) to the matrix M1." << endl;
 M1 = m3E;
 cout << "The matrix M1 now is: " << M1 << endl;
 cout << "End of the pass one." << endl << endl;



//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;

 cout << "The length of the matrix M1 is: " << M1.module() << endl;
 cout << "The length of the matrix M2 is: " << M2.module() << endl;

 cout << "The matrix M2 is: " << M2 << "and the transposed matrix M2 is: " << M2.T() << endl;

 M = M2;
 cout << "The unified matrix M " << M;
 M.unify();
 cout << "is: " << M << endl;
 M = M2;
 M(Z_AXIS, Z_AXIS) = 1;
 cout << "The unified matrix M " << M;
 M.unify();
 cout << "is: " << M << endl;
 cout << "End of the pass two." << endl << endl;




//==================================================================================
 cout << "Pass three: Checking basic matrix oparations." << endl;
 M = M1;
 cout << "Incremented matrix " << M << " on the matrix " << M2 << " is: ";
 M+= M2;
 cout << M << endl;
 M = M1;

 cout << "Decremented matrix " << M << " on the matrix " << M2 << " is: ";
 M-= M2;
 cout << M << endl;
 M = M1;

 f = 4.0;
 cout << "The Matrix " << M << " multiplied by the scalar " << f << " is ";
 M*= f;
 cout << M << endl;
 M = M1;

 cout << "The Matrix " << M << " divided by the scalar " << f << " is ";
 M/=f;
 cout << M << endl;
 M = M1;

 cout << "The matrix " << M2 << " and the reverted one " << -M2 << endl;

 cout << "The sum of the matrix " << M1 << " and the matrix " << M2 << " is: " << M1 + M2 << endl;
 cout << "The difference of the matrix " << M1 << " and the matrix " << M2 << " is: " << M1 - M2 << endl;

 cout << "The matrix " << M1 << " divided by the scalar " << f << " is: " << M1/f << endl;

 cout << "The product of the scalar " << f << " and the matrix  " << M1 << " is: " << f*M1 << endl;
 cout << "The product of the matrix " << M1 << " and the scalar " << f << " is: " << M1*f << endl;
 cout << "The product of the matrix " << M2 << " and the vector " << v << " is: " << M2*v << endl;

 cout << "The product of the matrix " << M1+M2 << " and the matrix " << M2 << " is: " << (M1+M2)*M2 << endl;
 M = M1+M2;
 M1= ~M;
 cout << "The inverted matrix of the matrix " << M << " is " << M1 << endl;
 cout << "The product of the matrix and the inverted one is " << M*~M << endl;
 cout << "The same but minus unit matrix: " << M*~M - m3E << endl;
 cout << "With the module: " << (M*~M - m3E).module() << endl;

 cout << "End of the pass three." << endl << endl;
 return 0;
};
