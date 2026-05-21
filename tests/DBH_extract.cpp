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
// QString                Str("Qua");

 SgDbhStream            s;
 SgDbhImage             image;
 QTextStream            out(stdout);

// using namespace std;
 std::cout.precision(24);
 //std::cout.setf(std::ios_base::floatfield);
// logger->setIsNeedTimeMark(false);



//==================================================================================

 QString                 dbhFileName("10APR06XU_V004");
 QString                 lCode;
 QString                 str;
 bool                    have2ModPrecision(false);
 int                     nPrec(6);


 if (argc>2)
 {
    lCode       = argv[1];
    dbhFileName = argv[2];
    if (argc>3)
    {
       str = argv[3];
       nPrec = str.toInt(&have2ModPrecision);
    };

 }
 else
 {
    std::cout << "say: LCODE database_file_name\n";
    return 1;
 };


 QFile                 dbhFile(dbhFileName);

 dbhFile.open(QIODevice::ReadOnly);
 s.setDevice(&dbhFile);
 s >> image;
 s.setDevice(NULL);
 dbhFile.close();

 int                   num = image.numberOfObservations();
 int                   iObs;
 if (!num)
 {
    std::cout << "The number of observations is zero, exiting.\n";
    return -1;
 };
 SgDbhDatumDescriptor* d = image.lookupDescriptor(qPrintable(lCode));
 SgDbhDatumDescriptor* dUtc = image.lookupDescriptor("UTC TAG ");
 SgDbhDatumDescriptor* dSec = image.lookupDescriptor("SEC TAG ");
 SgDbhDatumDescriptor* dStn = image.lookupDescriptor("BASELINE");
 SgDbhDatumDescriptor* dSrc = image.lookupDescriptor("STAR ID ");

 if (!d)
 {
    std::cout << "The descriptor is NULL, exiting.\n";
    return -1;
 };
 //
 if (!dUtc)
 {
    std::cout << "Cannot find \"UTC TAG \" LCODE in the image, exiting.\n";
    return -1;
 };
 if (!dSec)
 {
    std::cout << "Cannot find \"SEC TAG \" LCODE in the image, exiting.\n";
    return -1;
 };
 if (!dStn)
 {
    std::cout << "Cannot find \"BASELINE\" LCODE in the image, exiting.\n";
    return -1;
 };
 if (!dSrc)
 {
    std::cout << "Cannot find \"STAR ID \" LCODE in the image, exiting.\n";
    return -1;
 };
 //
 if (d->nTc()==0)
 {
    std::cout << "The descriptor is from TOC#0, exiting.\n";
    return -1;
 };

 SgMJD        t(tZero);
 QString      st1(""), st2(""), src("");
 int          year, month, day, hour, minute;
 double       second;

 std::cout << "Dump of [" << qPrintable(d->getLCode()) << "] LCODE" << std::endl;

 if (have2ModPrecision)
 {
    std::cout.precision(nPrec);
    std::cout << "Precision has been shifted to " << nPrec << "\n";
 };


 for (iObs=0; iObs<num; iObs++)
 {
   year   = image.getI2(dUtc, 0, 0, 0, iObs);
   month  = image.getI2(dUtc, 1, 0, 0, iObs);
   day    = image.getI2(dUtc, 2, 0, 0, iObs);
   hour   = image.getI2(dUtc, 3, 0, 0, iObs);
   minute = image.getI2(dUtc, 4, 0, 0, iObs);
   second = image.getR8(dSec, 0, 0, 0, iObs);

   t.setUpEpoch(year, month, day, hour, minute, second);

   st1 = image.getStr(dStn, 0, 0, iObs);
   st2 = image.getStr(dStn, 1, 0, iObs);
   src = image.getStr(dSrc, 0, 0, iObs);

   str.sprintf("%6d %12.8f  %-8s:%-8s %-8s",
    iObs, t.toDouble(), qPrintable(st1), qPrintable(st2), qPrintable(src));
   std::cout << qPrintable(str) << " ";


   for (int li=0; li<d->dim3(); li++)
   {
     std::cout << "   ";
     for (int lj=0; lj<d->dim2(); lj++)
     {
       if (d->type()==SgDbhDatumDescriptor::T_A2)
         std::cout  << "\""
                    << qPrintable(image.getStr(d, lj, li, iObs))
                    << "\"";
       else
       {
         for (int lk=0; lk<d->dim1(); lk++)
         {
           switch(d->type())
           {
             case SgDbhDatumDescriptor::T_R8:
               str.sprintf("%32.28f", image.getR8(d, lk, lj, li, iObs));
               std::cout << qPrintable(str);
//               std::cout << image.getR8(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_I2:
               std::cout << image.getI2(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_D8:
               str.sprintf("%32.28f", image.getD8(d, lk, lj, li, iObs));
               std::cout << qPrintable(str);
//               std::cout << image.getD8(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_J4:
               std::cout << image.getJ4(d, lk, lj, li, iObs);
               break;
             default: std::cout << "*";
           };
           std::cout << (lk<d->dim1()-1?" ":"");
         };
       };
       std::cout << (lj<d->dim2()-1?" ":"");
     };
//     std::cout << (li<d->dim3()-1 ? "," : "");
   };
   std::cout << std::endl;


   /*
   std::cout << "[" << qPrintable(d->getLCode()) << "]:" << std::endl << "(" << std::endl;
   for (int li=0; li<d->dim3(); li++)
   {
     std::cout << "   (";
     for (int lj=0; lj<d->dim2(); lj++)
     {
       if (d->type()==SgDbhDatumDescriptor::T_A2)
         std::cout  << "\""
                    << qPrintable(image.getStr(d, lj, li, iObs))
                    << "\"";
       else
       {
         std::cout << "(";
         for (int lk=0; lk<d->dim1(); lk++)
         {
           switch(d->type())
           {
             case SgDbhDatumDescriptor::T_R8: 
               std::cout << image.getR8(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_I2:
               std::cout << image.getI2(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_D8:
               std::cout << image.getD8(d, lk, lj, li, iObs);
               break;
             case SgDbhDatumDescriptor::T_J4:
               std::cout << image.getJ4(d, lk, lj, li, iObs);
               break;
             default: std::cout << "????";
           };
           std::cout << (lk<d->dim1()-1?", ":"");
         };
         std::cout << ")";
       };
       std::cout << (lj<d->dim2()-1?", ":"");
     };
     std::cout << ")" << (li<d->dim3()-1 ? "," : "") << std::endl;
   };
   std::cout << ");" << std::endl;
   */
 };

 return 0;
};
