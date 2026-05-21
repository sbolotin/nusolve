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


#include <SgDbhImage.h>
#include <SgMJD.h>
#include <QtCore/QFile>
#include <SgLogger.h>

#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;



int main(int argc, char** argv)
{
 QString                Str("Qua");

 SgDbhStream            s;
 SgDbhImage             image;
 QTextStream            dump;

// QTextStream            out(stdout);
// using namespace std;
// cout.precision(15);
// cout.setf(ios_base::floatfield);

//==================================================================================
 QString                 dbhFileName("10APR06XU_V004");
 QString                 str;
 int                     nPrec(6);
//bool                   isMakeDump=true, have2ModPrecision(false);
 bool                    have2ModPrecision(false);


 if (argc>1)
 {
    dbhFileName = argv[1];
    if (argc>2)
    {
       str = argv[2];
       nPrec = str.toInt(&have2ModPrecision);
    };
 }
 else
 {
    std::cout << "say file name [precision]\n";
    return 1;
 };

 QString               dumpFileName = dbhFileName + ".dump";
 QFile                 dbhFile(dbhFileName);
 QFile                 dumpFile(dumpFileName);

 dbhFile.open(QIODevice::ReadOnly);
 s.setDevice(&dbhFile);

 dumpFile.open(QIODevice::WriteOnly);
 dump.setDevice(&dumpFile);
 image.setDumpStream(&dump);
 if (have2ModPrecision)
 {
    dump.setRealNumberPrecision(nPrec);
    std::cout << "Precision has been shifted to " << nPrec << "\n";
 };

 s >> image;
 s.setDevice(NULL);
 dbhFile.close();

 dump.setDevice(NULL);
 dumpFile.close();

 return 0;
};
