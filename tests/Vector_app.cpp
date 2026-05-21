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


#include <SgVector.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(UNUSED int argc, UNUSED char** argv)
{
 unsigned int         N=10;
 SgVector             v1(N);
 SgVector             v2(N);

 using namespace std;
 //cout.precision(15);
 //cout.setf(ios_base::floatfield);

//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;
 v1 = 17.0;
 for (unsigned int i = 0; i<N; i++)
    v2(i) = 10.0*i;

 cout << "Vector v1 is: " << v1 << " and vector v2 is: " << v2 << endl;
 cout << "Copying 2nd, 3rd, 4th and 5th elements from v2 to v1." << endl;
 v1(1) = v2.getElement(1);
 v1(2) = v2.getElement(2);
 v1(3) = v2.getElement(3);
 v1(4) = v2.getElement(4);
 cout << "Now, the vector v1 is: " << v1 << " and the vector v2 is: " << v2 << endl;

 cout << "Constructing the vector from the another one:" << endl;
 SgVector             v3(v2);
 cout << "Now, the vector v2 is: " << v2 << " and the vector v3 is: " << v3 << endl;
 cout << "Makeing an assignment of one vector to another one:" << endl;
 v3 = v1;
 cout << "Vector v3 is: " << v3 << endl;

 SgVector             v4(2*N);
 for (unsigned int i = 0; i<2*N; i++)  
   v4(i) = 0.1 + 0.5*i;

 cout << "Vector v4 is: " << v4 << " with dimension: " << v4.n() << endl;
 cout << "Makeing an assignment of one vector to another one:" << endl;
 v4 = v1;
 cout << "Vector v4 is: " << v4 << " with dimension: " << v4.n() << endl;
 v4.setElement(5, 17.0);
 v4.setElement(6, 5.555);
 v4.setElement(7, M_PI);
 v4.setElement(8, M_PI_2);
 cout << "Vector v4 is: " << v4 << " with dimension: " << v4.n() << endl;

 cout << "End of the pass one." << endl << endl;

 for (unsigned int i = 0; i<N; i++) 
   v4(i) = 1.0 + 0.5*i;



//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;
 cout << "The length of the vector v1 is: " << v1.module() << endl;
 cout << "The length of the vector v2 is: " << v2.module() << endl;
 cout << "The length of the vector v3 is: " << v3.module() << endl;
 cout << "The length of the vector v4 is: " << v4.module() << endl;
 v4.clear();
 cout << "The length of the cleared vector v4 is: " << v4.module() << endl;

 cout << "End of the pass two." << endl << endl;



//==================================================================================
 cout << "Pass three: Checking basic vector oparations." << endl;

 for (unsigned int i = 0; i<N; i++)
 {
   v1(i) = -2.0 + 1.5*i;
   v2(i) = -1.0 + 0.5*i;
   v3(i) =  1.0 - 0.5*i;
   v4(i) =  2.0 - 1.5*i;
 };

 SgVector v(v3);

 cout << "Incremented vector " << v << " by the vector " << v2 << " is: ";
 v+=v2;
 cout << v << endl;
 v = v3;

 cout << "Decremented vector " << v << " by the vector " << v2 << " is: ";
 v-=v2;
 cout << v << endl;
 v = v3;

 double f = 4.0;
 cout << "The vector " << v << " multiplied by the scalar " << f << " is ";
 v*=f;
 cout << v << endl;
 v = v3;

 cout << "The vector " << v << " divided by the scalar " << f << " is ";
 v/=f;
 cout << v << endl;
 v = v3;

 cout << "The vector " << v1 << " and the reverted one " << -v1 << endl;

 cout << "The sum of the vector " << v1 << " and the vector " << v2 << " is: " << v1 + v2 << endl;
 cout << "The sum of the vector " << v1 << " and the vector " << v4 << " is: " << v1 + v4 << endl;
 cout << "The difference of the vector " << v1 << " and the vector " << v2 << " is: " << v1 - v2 << endl;
 cout << "The difference of the vector " << v1 << " and the vector " << v4 << " is: " << v1 - v4 << endl;
 cout << "The vector " << v2 << " divided by the scalar " << f << " is: " << v2/f << endl;
 cout << "The product of the scalar " << f << " and the vector  " << v1 << " is: " << f*v1 << endl;
 cout << "The product of the vector " << v1 << " and the scalar " << f << " is: " << v1*f << endl;
 cout << "The scalar product of the vector " << v1 << " and the vector " << v2 << " is: " << v1*v2 << endl;

 v = v3;
 if (v == v3)
   cout << "Vectors " << v  << " and " << v3 << " are equal" << endl;
 else 
   cout << "Vectors " << v  << " and " << v3 << " are not equal" << endl;
 v = v2;
 if (v == v3)
   cout << "Vectors " << v  << " and " << v3 << " are equal" << endl;
 else 
   cout << "Vectors " << v  << " and " << v3 << " are not equal" << endl;

 if (v1 != v2)
   cout << "Vectors " << v1 << " and " << v2 << " are not equal" << endl;
 else 
   cout << "Vectors " << v1 << " and " << v2 << " are equal" << endl;
 if (v != v2)
   cout << "Vectors " << v  << " and " << v3 << " are not equal" << endl;
 else 
   cout << "Vectors " << v  << " and " << v3 << " are equal" << endl;

 cout << "End of the pass three." << endl << endl;

 return 0;
};
