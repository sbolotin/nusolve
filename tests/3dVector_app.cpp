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
#include <SgConstants.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(UNUSED int argc, UNUSED char** argv)
{
 Sg3dVector        v1(1, 7, 8.5);
 Sg3dVector        v2(41, -3.3, 17);
 Sg3dVector        v3(v3Unit);

 using namespace std;
 //cout.precision(15);
 //cout.setf(ios_base::floatfield);
 
//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;

 cout << "Vector v3 is: " << v3 << " and vector v2 is: " << v2 << endl;
 cout << "Makeing a copy of Y-coordinate from v2 to v3." << endl;
 v3(Y_AXIS) = v2.at(Y_AXIS);
 cout << "Vector v3 is: " << v3 << " and vector v2 is: " << v2 << endl;

 cout << "Makeing an assignment of a unit vector (predefined constant) to the vector v3" << endl;
 v3 = v3Unit;
 cout << "Vector v3 is: " << v3 << endl;

 cout << "End of the pass one." << endl << endl;



//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;
 cout << "The length of the vector v3 is: " << v3.module() << endl;
 cout << "The latitude and the longitude of the vector v3, " << v3 << ", are: "
      << v3.phi()*RAD2DEG << " and " << v3.lambda()*RAD2DEG << " degrees" << endl;

 cout << "The unit vector corresponding to v3 is: " << v3.unit() << endl;

 v1 = v3Unit.unit();
 double f = (v3.unify() - v1).module();
 v1 =  Sg3dVector(1, 7, 8.5);

 if (f>1.0e-15)
    cout << "The difference between .unify() and unit() it greater than 1.0e-15" << endl;

 cout << "The length of (unify() - unit()) is: " << f << endl;
 cout << "End of the pass two." << endl << endl;
 v3 = v3Unit;



//==================================================================================
 cout << "Pass three: Checking basic vector oparations." << endl;

 cout << "Incremented vector " << v3 << " by the vector " << v2 << " is: ";
 v3+=v2;
 cout << v3 << endl;
 v3 = v3Unit;

 cout << "Decremented vector " << v3 << " by the vector " << v2 << " is: ";
 v3-=v2;
 cout << v3 << endl;
 v3 = v3Unit;

 f = 4.0;
 cout << "The vector " << v3 << " multiplied by the scalar " << f << " is ";
 v3*=f;
 cout << v3 << endl;
 v3 = v3Unit;

 cout << "The vector " << v3 << " divided by the scalar " << f << " is ";
 v3/=f;
 cout << v3 << endl;
 v3 = v3Unit;

 cout << "The vector " << v1 << " and the reverted one " << -v1 << endl;

 cout << "The sum of the vector " << v1 << " and the vector " << v2 << " is: " << v1 + v2 << endl;
 cout << "The difference of the vector " << v1 << " and the vector " << v2 << " is: " << v1 - v2 << endl;

 cout << "The vector " << v2 << " divided by the scalar " << f << " is: " << v2/f << endl;

 cout << "The product of the scalar " << f << " and the vector  " << v1 << " is: " << f*v1 << endl;
 cout << "The product of the vector " << v1 << " and the scalar " << f << " is: " << v1*f << endl;

 cout << "The scalar product of the vector " << v1 << " and the vector " << v2 << " is: " << v1*v2 << endl;

 cout << "The vector product of the vector " << v1 << " and the vector " << v2 << " is: " << v1 % v2 << endl;

 v3 = v1;
 if (v3 == v3Unit)
   cout << "Vectors " << v3 << " and " << v3Unit << " are equal" << endl;
 else 
   cout << "Vectors " << v3 << " and " << v3Unit << " are not equal" << endl;
 v3 = v3Unit;
 if (v3 == v3Unit)
   cout << "Vectors " << v3 << " and " << v3Unit << " are equal" << endl;
 else 
   cout << "Vectors " << v3 << " and " << v3Unit << " are not equal" << endl;

 if (v1 != v2)
   cout << "Vectors " << v1 << " and " << v2 << " are not equal" << endl;
 else 
   cout << "Vectors " << v1 << " and " << v2 << " are equal" << endl;
 if (v3 != v3Unit)
   cout << "Vectors " << v3 << " and " << v3Unit << " are not equal" << endl;
 else 
   cout << "Vectors " << v3 << " and " << v3Unit << " are equal" << endl;

 cout << "End of the pass three." << endl << endl;

 return 0;
};
