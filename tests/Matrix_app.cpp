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
#include <SgMatrix.h>


#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;


int main(int UNUSED argc, UNUSED char** argv)
{
 unsigned int           N = 10, M = 20;
 unsigned int           i, j;
 SgVector             vn(N);
 SgVector             vm(M);
 SgMatrix             A1(N, M);


 using namespace std;
 //cout.precision(15);
 //cout.setf(ios_base::floatfield);

//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;

 for (i=0; i<N; i++)
   for (j=0; j<M; j++)
      A1(i, j) = 10*(i+1) + 0.01*(j+1);
 cout << A1;









 cout << "End of the pass one." << endl << endl;



//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;
 cout << "End of the pass two." << endl << endl;



//==================================================================================
 cout << "Pass three: Checking basic vector oparations." << endl;
 cout << "End of the pass three." << endl << endl;

 return 0;
};
