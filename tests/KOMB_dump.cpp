#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <QtCore/QString>


#include <SgKombFormat.h>
#include <SgMJD.h>


#ifdef OLD_HOPS
char progname[] = "KOMB_dump";
int msglev = 2;
#endif


#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;

//
//
//
//
int main(int argc, char **argv)
{
 QString                fileName("");
 QString                str("");

 if (argc>1)
 {
    fileName = argv[1];
 }
 else
 {
    std::cout << "say a file name\n";
    return 1;
 };

 std::cout << "Trying to read [" << qPrintable(fileName) << "] file\n";

 SgKombReader           kombReader;
 kombReader.setIsVerbose(true);
 kombReader.readFile(fileName);
 if (!kombReader.isOk())
 {
    std::cout << "the KOMB file " << qPrintable(fileName) << " is not Ok; skipped\n";
    return 1;
 };

 std::cout << "\nThe file has been successfully read, the session:" << kombReader.experimentCode() << "\n";
 return 0;
}


