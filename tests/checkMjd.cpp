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


#include <SgMJD.h>


#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;



int main(UNUSED int argc, UNUSED char** argv)
{

 SgMJD                  t(tZero);
 QString                str("");

// SgMJD::currentMJD().toUtc()

 int                    yr, mn, dy, hr, mi;
 double                 sc;
 double                 day, time;

 while (true)
 {
    yr = mn = dy = hr = mi = 0;
    sc = 0.0;
    t = SgMJD::currentMJD().toUtc();

    day = t.getDate();
    time = t.getTime();

    SgMJD::MJD_reverse(t.getDate(), t.getTime(),  yr, mn, dy, hr, mi, sc);

    if (hr == 0 && mi == 0 && sc < 0.5)
    {
      str.sprintf("day=%.8f time=%.8f", day, time);
      std::cout << "Got it: " << qPrintable(t.toString(SgMJD::F_Simple)) << "   " << qPrintable(str) << "\n";

    }
 };


 return 0;
};
