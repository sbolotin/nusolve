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



int main(UNUSED int argc, UNUSED char** argv)
{
 QString                Str("Qua");

 SgDbhStream            s;
 SgDbhImage             image;
 QTextStream            dump;
 QTextStream            out(stdout);

 using namespace std;
 //cout.precision(15);
 //cout.setf(ios_base::floatfield);
 logger->setIsNeedTimeMark(false);



//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;

// QFile                 dbhFile("10APR06XU_V001");
// QFile                 dumpFile("10APR06XU_V001.dump");
// QFile                 dbhFile ("03JUL09XA_V004");
// QFile                 dumpFile("03JUL09XA_V004.dump");

 QString                 dbhFileName("10APR06XU_V004");
 bool                    isMakeDump=true;


 if (argc>1)
    dbhFileName = argv[1];
 if (argc>2)
    isMakeDump=false;
 

 QString               dumpFileName = dbhFileName + ".dump";
 QFile                 dbhFile(dbhFileName);
 QFile                 dumpFile(dumpFileName);
 int                   inqObsNum = 17;


 dbhFile.open(QIODevice::ReadOnly);
 s.setDevice(&dbhFile);

 if (isMakeDump)
 {
    dumpFile.open(QIODevice::WriteOnly);
    dump.setDevice(&dumpFile);
    image.setDumpStream(&dump);
 };

 s >> image;
 s.setDevice(NULL);
 dbhFile.close();

 if (isMakeDump)
 {
    dump.setDevice(NULL);
    dumpFile.close();
 };

 return 0;


if (true)
{
 if (image.numberOfObservations()<=inqObsNum)
  inqObsNum = image.numberOfObservations()/2;

 cout << "Checking general TOC." << endl;
 SgDbhDatumDescriptor *d;

 int numOfLCodesR8FromTocGeneral = 7;
 const char* lCodesR8FromTocGeneral[7] = {"12345678", "HALF PI ", "VLIGHT  ", "AXISTILT", "TAI- UTC", "ERROR K ", "ECCCOORD",};

 int numOfLCodesI2FromTocGeneral = 7;
 const char* lCodesI2FromTocGeneral[7] = {"12345678", "EXPSERNO", "INTERVAL", "NUMB OBS", "CALCFLGV", "BASLSTAT", "SOURSTAT",};

 int numOfLCodesA2FromTocGeneral = 7;
 const char* lCodesA2FromTocGeneral[7] = {"12345678", "CORPLACE", "EXPDESC ", "SITNAMES", "STRNAMES", "CALCFLGN", "MCALNAMS",};

 int numOfLCodesJ4FromTocGeneral = 2;
 const char* lCodesJ4FromTocGeneral[2] = {"12345678", "NUM4 OBS",};


 d = image.lookupDescriptor("HALF PI ");
 if (d)
 {
   image.setR8(d, 0,0,0,-1, 2.01212);
 };

 for (int i=0; i<numOfLCodesR8FromTocGeneral; i++)
 {
    d = image.lookupDescriptor(lCodesR8FromTocGeneral[i]);
    if (d && d->nTc()==0)
    {
        cout << "    descriptor: [" << qPrintable(d->getLCode()) << "] or {" << qPrintable(d->getDescription()) << "}; max indexes: ("
             << d->dim1() << ":" << d->dim2() << ":" << d->dim3() << "), offset: " << d->offset() << endl << "(" << endl;
        for (int li=0; li<d->dim3(); li++)
        {
          cout << "   (";
          for (int lj=0; lj<d->dim2(); lj++)
          {
            cout << "(";
            for (int lk=0; lk<d->dim1(); lk++)
              cout << image.getR8(d, lk, lj, li) << (lk<d->dim1()-1?", ":"");
            cout << ")" << (lj<d->dim2()-1?", ":"");
          };
          cout << ")" << (li<d->dim3()-1 ? "," : "") << endl << ");" << endl;
        };
    }
    else
        cout << "LCode [" << lCodesR8FromTocGeneral[i] << "] is not in TOC General" << endl;
 };


 for (int i=0; i<numOfLCodesI2FromTocGeneral; i++)
 {
    d = image.lookupDescriptor(lCodesI2FromTocGeneral[i]);
    if (d && d->nTc()==0)
    {
        cout << "    descriptor: [" << qPrintable(d->getLCode()) << "] or {" << qPrintable(d->getDescription()) << "}; max indexes: ("
             << d->dim1() << ":" << d->dim2() << ":" << d->dim3() << "), offset: " << d->offset() << endl << "(" << endl;
        for (int li=0; li<d->dim3(); li++)
        {
          cout << "   (";
          for (int lj=0; lj<d->dim2(); lj++)
          {
            cout << "(";
            for (int lk=0; lk<d->dim1(); lk++)
              cout << image.getI2(d, lk, lj, li) << (lk<d->dim1()-1?", ":"");
            cout << ")" << (lj<d->dim2()-1?", ":"");
          };
          cout << ")" << (li<d->dim3()-1 ? "," : "") << endl << ");" << endl;
        };
    }
    else
        cout << "LCode [" << lCodesI2FromTocGeneral[i] << "] is not in TOC General" << endl;
 };


 for (int i=0; i<numOfLCodesA2FromTocGeneral; i++)
 {
    d = image.lookupDescriptor(lCodesA2FromTocGeneral[i]);
    if (d && d->nTc()==0)
    {
        cout << "    descriptor: [" << qPrintable(d->getLCode()) << "] or {" << qPrintable(d->getDescription()) << "}; max indexes: ("
             << d->dim1() << ":" << d->dim2() << ":" << d->dim3() << "), offset: " << d->offset() << endl << "(" << endl;
        for (int li=0; li<d->dim3(); li++)
        {
          cout << "   (";
          for (int lj=0; lj<d->dim2(); lj++)
          {
            cout << "\"" << qPrintable(image.getStr(d, lj, li))  << "\"";
            cout << (lj<d->dim2()-1?", ":"");
          }
          cout << ")" << (li<d->dim3()-1 ? "," : "") << endl << ");" << endl;
        };
    }
    else
        cout << "LCode [" << lCodesA2FromTocGeneral[i] << "] is not in TOC General" << endl;
 };

 for (int i=0; i<numOfLCodesJ4FromTocGeneral; i++)
 {
    d = image.lookupDescriptor(lCodesJ4FromTocGeneral[i]);
    if (d && d->nTc()==0)
    {
        cout << "    descriptor: [" << qPrintable(d->getLCode()) << "] or {" << qPrintable(d->getDescription()) << "}; max indexes: ("
             << d->dim1() << ":" << d->dim2() << ":" << d->dim3() << "), offset: " << d->offset() << endl << "(" << endl;
        for (int li=0; li<d->dim3(); li++)
        {
          cout << "   (";
          for (int lj=0; lj<d->dim2(); lj++)
          {
            cout << "(";
            for (int lk=0; lk<d->dim1(); lk++)
              cout << image.getJ4(d, lk, lj, li) << (lk<d->dim1()-1?", ":"");
            cout << ")" << (lj<d->dim2()-1?", ":"");
          };
          cout << ")" << (li<d->dim3()-1 ? "," : "") << endl << ");" << endl;
        };
    }
    else
        cout << "LCode [" << lCodesJ4FromTocGeneral[i] << "] is not in TOC General" << endl;
 };



 d = image.lookupDescriptor("CORPLACE");
 if (d && d->nTc()==0)
 {
   cout << qPrintable(d->getLCode()) << " was [" << qPrintable(image.getStr(d, 0,0))  << "]" << endl;
   image.setStr(d, 0,0,-1, "New-Vasjuki");
   cout << qPrintable(d->getLCode()) << " now [" << qPrintable(image.getStr(d, 0,0))  << "]" << endl;
 }
 else
    cout << "LCode [CORPLACE] is not in TOC General" << endl;



 d = image.lookupDescriptor("UTC TAG ");
 if (d)
 {
    int yr, mn, dy, hr, mi;
    yr = image.getI2(d, 0, 0, 0, inqObsNum);
    mn = image.getI2(d, 1, 0, 0, inqObsNum);
    dy = image.getI2(d, 2, 0, 0, inqObsNum);
    hr = image.getI2(d, 3, 0, 0, inqObsNum);
    mi = image.getI2(d, 4, 0, 0, inqObsNum);
    cout << "yr: " << yr << ", mn: " << mn << ", dy: " << dy << ", hr: " << hr << ", mi: " << mi<< endl;
    cout << "Epoch of Obs#" << inqObsNum+1 << " is: " << qPrintable(SgMJD(yr,mn,dy,hr,mi).toString()) << endl;
 }
 else
    cout << "LCode [RFREQ   ] is not in TOC General" << endl;




 d = image.lookupDescriptor("RFREQ   ");
 if (d)
 {
    cout << "    descriptor: [" << qPrintable(d->getLCode()) << "] or {" << qPrintable(d->getDescription()) << "}; max indexes: ("
         << d->dim1() << ":" << d->dim2() << ":" << d->dim3() << "), offset: " << d->offset() << endl << "(" << endl;
    for (int li=0; li<d->dim3(); li++)
    {
        cout << "   (";
        for (int lj=0; lj<d->dim2(); lj++)
        {
            cout << "(";
            for (int lk=0; lk<d->dim1(); lk++)
                cout << image.getR8(d, lk, lj, li, inqObsNum) << (lk<d->dim1()-1?", ":"");
            cout << ")" << (lj<d->dim2()-1?", ":"");
        };
        cout << ")" << (li<d->dim3()-1 ? "," : "") << endl << ");" << endl;
    };
 }
 else
    cout << "LCode [RFREQ   ] is not in TOC General" << endl;


 // check Pi:
 d = image.lookupDescriptor("PI      ");
 if (d)
 {
    double dPi = image.getR8(d, 0,0,0) - M_PI;
    printf ("deltaPi=%24.20f\n", dPi);
 }
 else
    cout << "LCode [PI      ] is not in TOC General" << endl;

 // check speed of light:
 d = image.lookupDescriptor("VLIGHT  ");
 if (d)
 {
    double dC = image.getR8(d, 0,0,0) - 299792458.0;
    printf ("deltaC=%24.20f\n", dC);
 }
 else
    cout << "LCode [VLIGHT  ] is not in TOC General" << endl;

 SgDbhDatumDescriptor* d1 = new SgDbhDatumDescriptor("PI      ", "pi?");
 SgDbhDatumDescriptor* d2 = new SgDbhDatumDescriptor("VLIGHT  ", "c?");
 SgDbhDatumDescriptor* d3 = new SgDbhDatumDescriptor("UTC TAG ", "t?");

 image.startFormatModifying();
 image.delDescriptor(d1);
 image.delDescriptor(d2);
 image.delDescriptor(d3);

 delete d1;
 d1 = new SgDbhDatumDescriptor("ACCELGRV", "what?");
 image.delDescriptor(d1);

 delete d1;
 delete d2;
 delete d3;

 d1 = new SgDbhDatumDescriptor("QQQ", "A Test string");
 d2 = new SgDbhDatumDescriptor("Some LCODE", "Test parameter to TOC#0");
 d3 = new SgDbhDatumDescriptor("TEMP TST", "Another test parameter to TOC#0");

 d1->setType(SgDbhDatumDescriptor::T_R8);
 d2->setType(SgDbhDatumDescriptor::T_A2);
 d3->setType(SgDbhDatumDescriptor::T_D8);

 d1->setDimensions( 2,1,1);
 d2->setDimensions( 3,3,2);
 d3->setDimensions(16,4,4);

 d1->setNTc(0);
 d2->setNTc(0);
 d3->setNTc(1);

 image.addDescriptor(d1);
 image.addDescriptor(d2);
 image.addDescriptor(d3);

 image.finisFormatModifying();


 // check Pi:
 d = image.lookupDescriptor("PI      ");
 if (d)
 {
    double dPi = image.getR8(d, 0,0,0) - M_PI;
    printf ("deltaPi=%24.20f\n", dPi);
 }
 else
    cout << "LCode [PI      ] is not in TOC General" << endl;

 // check speed of light:
 d = image.lookupDescriptor("VLIGHT  ");
 if (d)
 {
    double dC = image.getR8(d, 0,0,0) - 299792458.0;
    printf ("deltaC=%24.20f\n", dC);
 }
 else
    cout << "LCode [VLIGHT  ] is not in TOC General" << endl;


 image.startFormatModifying();

 SgDbhDatumDescriptor* d4 = new SgDbhDatumDescriptor("VASIA   ", "Test record #4");
 SgDbhDatumDescriptor* d5 = new SgDbhDatumDescriptor("PETIA V.", "Another record to test parameter insertions (this is a long line, at least more than 32 chars)");

 d4->setType(SgDbhDatumDescriptor::T_R8);
 d5->setType(SgDbhDatumDescriptor::T_A2);

 d4->setDimensions(1,1,1);
 d5->setDimensions(1,1,1);

 image.addDescriptor(d4);
 image.addDescriptor(d5);
 image.finisFormatModifying();


 d = image.lookupDescriptor("TEMP TST");
 if (d)
 {
    for (int i=0; i<10; i++)
        image.setD8(d, 0,0,0, i, 2.01212+i);

    double t = image.getD8(d, 0,0,0, 2);
    printf ("t=%24.20f\n", t);
    t = image.getD8(d, 0,0,0, 3);
    printf ("t=%24.20f\n", t);
    t = image.getD8(d, 0,0,0, 4);
    printf ("t=%24.20f\n", t);
 }
 else
    cout << "LCode [TEMP TST] is not in TOC General" << endl;

// image.setDumpStream(&out);
// image.dumpFormat();




//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;

 image.prepare2save();
 dbhFileName = image.fileName();
 dbhFile.setFileName(dbhFileName);
 dbhFile.open(QIODevice::WriteOnly);
 s.setDevice(&dbhFile);
 s << image;
 s.setDevice(NULL);
 dbhFile.close();
 cout << "End of the pass two." << endl;
}
else
{

 image.startFormatModifying();

 SgDbhDatumDescriptor* d1 = new SgDbhDatumDescriptor("TST R8 V", "Test record #4");
 SgDbhDatumDescriptor* d2 = new SgDbhDatumDescriptor("TST I2 V", "Another record to test parameter insertions (this is a long line, at least more than 32 chars)");

 d1->setType(SgDbhDatumDescriptor::T_R8);
 d2->setType(SgDbhDatumDescriptor::T_I2);

 d1->setDimensions(2,1,1);
 d1->setNTc(2);

 d2->setDimensions(1,2,1);
 d2->setNTc(2);

 image.addDescriptor(d1);
 image.addDescriptor(d2);
 image.finisFormatModifying();

 d1 = image.lookupDescriptor("TST R8 V");
 d2 = image.lookupDescriptor("TST I2 V");
 SgDbhDatumDescriptor* dUTC  = image.lookupDescriptor("UTC TAG ");
// SgDbhDatumDescriptor* dSec  = image.lookupDescriptor("SEC TAG ");
 SgDbhDatumDescriptor* dBase = image.lookupDescriptor("BASELINE");
 SgDbhDatumDescriptor* dDelay= image.lookupDescriptor("DEL OBSV");

 SgMJD tRef = SgMJD(2010, 5, 21, 16, 51, 59.0);
 SgMJD t;
 for (int i=0; i<image.numberOfObservations(); i++)
 {
    if (d1)
    {
        image.setR8(d1, 0,0,0, i, 2.01212 + 1.8*sin(2.0*M_PI*i/1000.0));
        image.setR8(d1, 1,0,0, i, 2.01212 + 1.8*cos(2.0*M_PI*i/1000.0));
    };
    if (d2)
    {
        image.setI2(d2, 0,0,0, i, (short)(-20 + 2*i));
        image.setI2(d2, 0,1,0, i, (short)( 80 - 6*i));
    };
    if (!dDelay)
        cerr << "Cannot find OBS_DELAY's tag\n";
    if (!dUTC)
        cerr << "Cannot find UTC's tag\n";
    if (!dBase)
        cerr << "Cannot find BASELINE's tag\n";

    if (dDelay && dUTC && dBase)
    {
        int yr = image.getI2(dUTC, 0,0,0, i);
        int mn = image.getI2(dUTC, 1,0,0, i);
        int dy = image.getI2(dUTC, 2,0,0, i);
        int hr = image.getI2(dUTC, 3,0,0, i);
        int mi = image.getI2(dUTC, 4,0,0, i);
        t.setUpEpoch(yr+2000, mn, dy, hr, mi, 0.0);
        QString stat_1, stat_2;
        stat_1 = image.getStr(dBase, 0,0, i);
        stat_2 = image.getStr(dBase, 1,0, i);

//        cout << "Processing: " << qPrintable(stat_1) << ":" << qPrintable(stat_2) << " at " 
//             << yr << "/" << mn << "/" << dy << " " << hr << ":" << mi << endl;
        if (stat_1 == "BADARY  ")
        {
//            cout << "This is BADAY\n";
            if (t>tRef)
            {
                double del1 = image.getR8(dDelay, 0,0,0, i);
                double del2 = image.getR8(dDelay, 1,0,0, i);
                double sh = 17000 - 40;
                image.setR8(dDelay, 0,0,0, i, del1 - sh);
                image.setR8(dDelay, 1,0,0, i, del2 - trunc(del2));
                cout << "Got: " << mn << "/" << dy << " " << hr << ":" << mi 
                     << ", was: " << del1 << "+" << del2
                     << ", became: " << del1 - sh << "+" << del2 - trunc(del2) << "\n";
            };
        };
    };
 };

 image.prepare2save();
 dbhFileName = image.fileName();
 dbhFile.setFileName(dbhFileName);
 dbhFile.open(QIODevice::WriteOnly);
 s.setDevice(&dbhFile);
 s << image;
 s.setDevice(NULL);
 dbhFile.close();
}

 logger->clearSpool();

//==================================================================================
 cout << "Pass three: Checking basic vector oparations." << endl;

 cout << "End of the pass three." << endl;

 return 0;
};
