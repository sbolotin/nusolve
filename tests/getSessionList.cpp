#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

//#include <SgMJD.h>


#include <SgVersion.h>
SgVersion                       drVersion("test", 0, 0, 1, "Banzai", SgMJD(2020, 9, 14, 14, 32));
SgVersion                      *driverVersion=&drVersion;
const QString                   dMonths[] =
                                    {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};


#ifdef OLD_HOPS
char                            progname[80];
int                             msglev=2;
#endif

//
//
//
//
int main(int argc, char **argv)
{
//## Master file format version 2.0                             2022.11.01 CAD&CCT
//## Master file format version 1.0           2001.08.21 CCT&NRV
//## Master file format version 1.0           2001.08.21 CCT&NRV

 QRegularExpression     reFmt("## Master file format version\\s+([0-9\\.]{2,6})\\s+(\\d{2,4}\\.\\d{2}\\.\\d{2}).*",
                          QRegularExpression::CaseInsensitiveOption);
 QRegularExpression     reYy("\\s+(\\d{4})\\s+MULTI-AGENCY\\s+(?:INTENSIVES\\s+|VGOS\\s+|)SCHEDULE\\s*",
                          QRegularExpression::CaseInsensitiveOption);
 QRegularExpressionMatch
                        match;
 QString                fileName("");
 QString                str(""), str1(""), str2(""), sMonth("");
 QFile                  f;
 QString                yy, name, code, date, stations, dbcCode, scheduledBy, correlatedBy, submittedBy;
 double                 dVer;
 bool                   isOk, printMap(false), printCorr(false), printSubm(false);
 int                    n;

 dVer = 0.0;
 isOk = false;

 if (argc>1)
 {
    fileName = argv[1];
    f.setFileName(fileName);
    if (!f.exists())
    {
      std::cout << "file \"" << qPrintable(fileName) << "\" does not exist\n";
      return 1;
    };
 }
 else
 {
    std::cout << "say a file name [, optionally m (map), or c (correlator) or submitter (s)]\n";
    return 1;
 };

 if (argc>2)
 {
    str = argv[2];
    if (str == "m")
        printMap = true;
 };

 if (argc>2)
 {
    str = argv[2];
    if (str == "c")
        printCorr = true;
 };

 if (argc>2)
 {
    str = argv[2];
    if (str == "s")
        printSubm = true;
 };




// std::cout << "Trying to read [" << qPrintable(fileName) << "] file\n";

 if (f.open(QFile::ReadOnly))
 {
   QTextStream                 s(&f);
   while (!s.atEnd())
   {
     str = s.readLine();

     if ((match=reFmt.match(str)).hasMatch())
     {
        dVer = match.captured(1).toDouble(&isOk);
        if (!isOk)
        {
            std::cout << "Error converting \"" << qPrintable(match.captured(1)) << "\" to double\n";
            return 1;
        };
        //std::cout << " ++ get format: " << dVer << "\n";
     };
     if ((match=reYy.match(str)).hasMatch())
     {
       // std::cout << " ++ get match: " << qPrintable(match.captured(1)) << "\n";
       yy = match.captured(1).right(2);
     };

     if (1.0 <= dVer && dVer < 2.0)
     {
        if (str.size()>16)
        {
            if (str.at(0) == QChar('|') &&
                str.at(str.size()-1) == QChar('|') &&
                str.count(QChar('|')) == 16)
            {
                QStringList fields=str.split("|");
                name         = fields.at(1).simplified();
                code         = fields.at(2).simplified();
                date         = fields.at(3).simplified();
                stations     = fields.at(7).simplified();
                dbcCode      = fields.at(12).simplified();
                scheduledBy  = fields.at(8).simplified();
                correlatedBy = fields.at(9).simplified();
                submittedBy  = fields.at(13).simplified();
                if (name.size() && code.size() && dbcCode.size() && date.size())
                {
                    str = yy + date + dbcCode;
                    if (printCorr)
                        str += "  " + correlatedBy;
                    std::cout << qPrintable(str) << "\n";
                };
            };
        };
     }
     else if (2.0 <= dVer)
     {
//     1            2       3           4   5     6                         7                                  8    9      10    11   12    13
//|IVS-R1      |20230103|r11084      |  3|17:00|24:00|AgHbHtIsKeKkKvMaNsOnWzYg -Ny                          |NASA|BONN|20230124| XA |NASA|  20|
//|IVS-INT-2   |20220102|q22002      |  2|07:30| 1:00|IsMkWz                                                |DACH|GSI |        | XK |GSI |   0|
        if (80 < str.size())
        {
            if (str.at(0) == QChar('|') &&
                14 <= str.count(QChar('|')))
            {
                QStringList fields=str.split("|");
                name         = fields.at(1).simplified();
                date         = fields.at(2).simplified();
                code         = fields.at(3).simplified();
                stations     = fields.at(7).simplified();
                dbcCode      = fields.at(11).simplified();
                scheduledBy  = fields.at(8).simplified();
                correlatedBy = fields.at(9).simplified();
                submittedBy  = fields.at(12).simplified();
                if (1) //name.size() && code.size() && dbcCode.size() && date.size())
                {
                    str1 = date + "-" + code;
                    if (printMap)
                    {
                        n = date.mid(4,2).toInt(&isOk);
                        if (!isOk || n<1 || 12<n)
                        {
                            std::cout << "Error: wrong month number [" << qPrintable(date.mid(4,2)) << "] from the string ["
                                      << qPrintable(str) << "]\n";
                            sMonth = "NON";
                        }
                        else
                            sMonth = dMonths[n-1];

                        str2 = date.mid(2,2) + sMonth + date.mid(6,2) + dbcCode;
                        std::cout << qPrintable(str2 + "  " + str1) << "\n";
                    }
                    else if (printCorr)
                    {
                        std::cout << qPrintable(str1 + "  " + correlatedBy) << "\n";
                    }
                    else if (printSubm)
                    {
                        std::cout << qPrintable(str1 + "  " + submittedBy) << "\n";
                    }
                    else
                        std::cout << qPrintable(str1) << "\n";
                };
            };
        };


     }
     else
     {
        std::cout << "Unknown version " << dVer << "\n";

     };
   };
   f.close();
   s.setDevice(NULL);
 };


 return 0;
}


