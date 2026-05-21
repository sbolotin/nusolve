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

 SgMJD                  t1(55300.017);        // first type of constructor
 SgMJD                  t2(55291, 43200.0/DAY2SEC);    // second type of constructor
 SgMJD                  t3(2009, 4, 12, 1, 3, 25.2);    // third type of constructor
 SgMJD                  t4(t3);          // fourth type of constructor

 SgMJD                  E = SgMJD::currentMJD();
 QString                Str("Qua");


 using namespace std;
 //cout.precision(15);
 //cout.setf(ios_base::floatfield);

//==================================================================================
 cout << "Pass one: Checking the interface of the object." << endl;

 cout << "Different output formats: " << endl;
 //
 cout << "Format: F_Verbose: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_Verbose)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_Verbose)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_Verbose)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_Verbose)) << endl;
 //
 cout << "Format: F_VerboseLong: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_VerboseLong)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_VerboseLong)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_VerboseLong)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_VerboseLong)) << endl;
 //
 cout << "Format: F_YYYYMMDDHHMMSSSS: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_YYYYMMDDHHMMSSSS)) << endl;
 //
 cout << "Format: F_YYYYMMDDSSSSSS: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_YYYYMMDDSSSSSS)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_YYYYMMDDSSSSSS)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_YYYYMMDDSSSSSS)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_YYYYMMDDSSSSSS)) << endl;
 //
 cout << "Format: F_INTERNAL: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_INTERNAL)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_INTERNAL)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_INTERNAL)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_INTERNAL)) << endl;
 //
 cout << "Format: F_ECCDAT: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_ECCDAT)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_ECCDAT)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_ECCDAT)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_ECCDAT)) << endl;
 //
 cout << "Format: F_MJD: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_MJD)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_MJD)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_MJD)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_MJD)) << endl;
 //
 cout << "Format: F_UNIX: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_UNIX)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_UNIX)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_UNIX)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_UNIX)) << endl;
 //
 cout << "Format: F_SINEX: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_SINEX)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_SINEX)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_SINEX)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_SINEX)) << endl;
 //
 cout << "Format: F_Date: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_Date)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_Date)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_Date)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_Date)) << endl;
 //
 cout << "Format: F_DDMonYYYY: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_DDMonYYYY)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_DDMonYYYY)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_DDMonYYYY)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_DDMonYYYY)) << endl;
 //
 cout << "Format: F_YYYYMMDD: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_YYYYMMDD)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_YYYYMMDD)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_YYYYMMDD)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_YYYYMMDD)) << endl;
 //
 cout << "Format: F_YYMonDD: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_YYMonDD)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_YYMonDD)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_YYMonDD)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_YYMonDD)) << endl;
 //
 cout << "Format: F_Year: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_Year)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_Year)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_Year)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_Year)) << endl;
 //
 cout << "Format: F_SINEX_S: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_SINEX_S)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_SINEX_S)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_SINEX_S)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_SINEX_S)) << endl;
//
 cout << "Format: F_Time: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_Time)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_Time)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_Time)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_Time)) << endl;
 //
 cout << "Format: F_HHMMSS: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_HHMMSS)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_HHMMSS)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_HHMMSS)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_HHMMSS)) << endl;
 //
 cout << "Format: F_TimeShort: " << endl;
 cout << "      t1: " << qPrintable(t1.toString(SgMJD::F_TimeShort)) << endl
      << "      t2: " << qPrintable(t2.toString(SgMJD::F_TimeShort)) << endl
      << "      t3: " << qPrintable(t3.toString(SgMJD::F_TimeShort)) << endl
      << "      t4: " << qPrintable(t4.toString(SgMJD::F_TimeShort)) << endl;



 t4 = t3 + 10.0;
 cout << "t4 now: " << qPrintable(t4.toString(SgMJD::F_VerboseLong)) << endl;


 cout << "t1 - t2: " << qPrintable(interval2Str(t1-t2)) << endl;
 cout << "t1 - tZero: " << qPrintable(interval2Str(t1-tZero)) << ", or " << t1-tZero << " days" << endl;
 cout << "t2 - tZero: " << qPrintable(interval2Str(t2-tZero)) << ", or " << t2-tZero << " days" << endl;
 cout << "t1 - tUnix0: " << qPrintable(interval2Str(t1-tUnix0)) << ", or " << t1-tUnix0 << " days" << endl;
 cout << "t2 - tUnix0: " << qPrintable(interval2Str(t2-tUnix0)) << ", or " << t2-tUnix0 << " days" << endl;

 cout << "t2 - (t2+1yr): " << qPrintable(interval2Str(t2-(t2+366.25))) << ", or " << t2-(t2+366.25) << " days" << endl;
 cout << "t2 - (t2+1month): " << qPrintable(interval2Str(t2-(t2+31.0))) << ", or " << t2-(t2+31.0) << " days" << endl;
 cout << "t2 - (t2+1day): " << qPrintable(interval2Str(t2-(t2+1.5))) << ", or " << t2-(t2+1.5) << " days" << endl;

 cout << "t1: "        << qPrintable(QString().sprintf("%.8f", t1.toDouble()))
      << ",  tUnix0: " << qPrintable(QString().sprintf("%.8f", tUnix0.toDouble()))
      << ", t1-tUnix0: " << qPrintable(QString().sprintf("%.8f", t1-tUnix0)) << " days" << endl;
 cout << "0.017days: " << qPrintable(interval2Str(0.017)) << ", and (1-0.017)days: "
      << qPrintable(interval2Str(1.0-0.017)) << endl;
 cout << "End of the pass one." << endl << endl;



 cout << "t1       : " << qPrintable(t1.toString())     << ", " << qPrintable(QString().sprintf("%.8f", t1.toDouble())) << endl;
 cout << "tUnix0   : " << qPrintable(tUnix0.toString()) << ", " << qPrintable(QString().sprintf("%.8f", tUnix0.toDouble())) << endl;
 cout << "t1-tUnix0: " << qPrintable(QString().sprintf("%.8f", t1-tUnix0)) << " days" << endl;
 cout << "t1-tUnix0: " << qPrintable(interval2Str(t1-tUnix0)) << endl;

 cout << endl << endl;



//==================================================================================
 cout << "Pass two: Checking basic functions of the object." << endl;

 cout << "2007: " << qPrintable(SgMJD(2007, 1, 98, 16, 15, 14).toString()) << endl
      << "2008: " << qPrintable(SgMJD(2008, 1, 98, 16, 15, 14).toString()) << endl
      << "2009: " << qPrintable(SgMJD(2009, 1, 98, 16, 15, 14).toString()) << endl;



 t1.fromString(SgMJD::F_SINEX, "80:272:00000");
 cout << "t1: " << qPrintable(t1.toString()) << endl;

 t1.fromString(SgMJD::F_SINEX, "08:365:00000");
 cout << "t1: " << qPrintable(t1.toString()) << endl;

 t1.fromString(SgMJD::F_SINEX, "05:250:00000");
 cout << "t1: " << qPrintable(t1.toString()) << endl;

 t1.fromString(SgMJD::F_SINEX, "92:179:00000");
 cout << "t1: " << qPrintable(t1.toString()) << endl;

 t1.fromString(SgMJD::F_SINEX, "92:176:56394");
 cout << "t1: " << qPrintable(t1.toString()) << endl;
 t1.fromString(SgMJD::F_SINEX, "92:181:29904");
 cout << "t1: " << qPrintable(t1.toString()) << endl;



 cout << "End of the pass two." << endl << endl;






//==================================================================================
 cout << "Pass three: Checking basic vector oparations." << endl;

 cout << "End of the pass three." << endl << endl;

 return 0;
};
