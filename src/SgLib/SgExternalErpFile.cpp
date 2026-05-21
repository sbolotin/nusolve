/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2020 Sergei Bolotin.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <math.h>


#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <SgExternalErpFile.h>
#include <SgLogger.h>
#include <SgMatrix.h>
#include <SgVector.h>




/*=======================================================================================================
*
*                          SgExternalEopFile's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgExternalEopFile::className()
{
  return "SgExternalEopFile";
};



// A destructor:
SgExternalEopFile::~SgExternalEopFile()
{
  if (argument_)
  {
    delete argument_;
    argument_ = NULL;
  };
  if (eopTable_)
  {
    delete eopTable_;
    eopTable_ = NULL;
  };
};



//
bool SgExternalEopFile::readFile(const QString& fileName, const SgMJD& tMean, int numOfPoints)
{
  int                           num;
  num = 5;
  isOk_ = false;  

  //
  if (fileName.right(4) == ".erp")
  {
    inputEopType_ = IET_ERP;
  }
  else if (fileName.right(5) == ".data")
  {
    inputEopType_ = IET_FINALS;
  }
  else if (fileName.right(4) == ".txt")
  {
    inputEopType_ = IET_C04;
  }
  else
    inputEopType_ = IET_UNKN;
  
  if (inputEopType_==IET_UNKN)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readFile(): uknown file type of the file \"" + fileName + "\"; import is not possible");
    return isOk_;
  };

  if (argument_)
    delete argument_;
  argument_ = new SgVector(numOfPoints);
  if (eopTable_)
    delete eopTable_;
  eopTable_ = new SgMatrix(numOfPoints, num);
  
  if (inputEopType_ == IET_ERP)
    isOk_ = readErpFile(fileName, tMean, numOfPoints);
  else if (inputEopType_ == IET_FINALS)
    isOk_ = readFinalsFile(fileName, tMean, numOfPoints);
  else if (inputEopType_ == IET_C04)
    isOk_ = readC04File(fileName, tMean, numOfPoints);


  if (!isOk_)
  {
    delete argument_;
    argument_ = NULL;
    delete eopTable_;
    eopTable_ = NULL;
  };
  return isOk_;
};



//
bool SgExternalEopFile::readErpFile(const QString& fileName, const SgMJD& tMean, int numOfPoints)
{
  bool                          isOk(false);
  QFile                         f(fileName);
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readErpFile(): the ERP file [" + fileName + 
      "] does not exist; import of external ERP is not possible");
    return false;
  };
  QString                       str;

  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    str = s.readLine();
    // parse first record:
    //          1         2         3         4         5         6         7         8
    //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
    //EOP-MOD Ver 2.0  2444074.5   1.0  12122  UT1-TAI   UNDEF
    if (str.mid(0, 14) != "EOP-MOD Ver 2." || str.length()<57)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): the ERP file [" + fileName + 
        "] is not real ERP file or its version is unknown; import is not possible");
      return false;
    };
    SgMJD                       t0(tZero);
    double                      d, step(0.0);
    int                         numOfRecords(0);
    d = str.mid(17, 9).toDouble(&isOk);
    if (!isOk)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: cannot determine declared first epoch (orig='" + str.mid(17, 9) + 
        "'); import is not possible");
      return false;
    };
    d -= 2400000.5;
    t0 = d;
    step = str.mid(28, 4).toDouble(&isOk);
    if (!isOk)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: cannot determine declared spacing (orig='" + str.mid(28, 4) + 
        "'); import is not possible");
      return false;
    };
    numOfRecords = str.mid(34, 5).toInt(&isOk);
    if (!isOk)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: cannot determine declared number of records (orig='" + str.mid(34, 6) + 
        "'); import is not possible");
      return false;
    };
    if (str.mid(41, 4) == "UT1 " || str.mid(41, 4) == "UT1-")
      ut1Type_ = SgTidalUt1::CT_FULL;
    else if (str.mid(41, 4) == "UT1S")
      ut1Type_ = SgTidalUt1::CT_ALL_TERMS_REMOVED;
    else if (str.mid(41, 4) == "UT1R")
      ut1Type_ = SgTidalUt1::CT_SHORT_TERMS_REMOVED;
    else
      ut1Type_ = SgTidalUt1::CT_FULL;
//    ut1Type_ = SgTidalUt1::CT_SHORT_TERMS_REMOVED;
    
    //    std::cout << qPrintable(fileName) << ": t0=" << qPrintable(t0.toString()) << ", dt = " << step  
    //              << ",  numOfRecords = " << numOfRecords << ", type: " << ut1Type_ 
    //              << "\n";
    
    // skip comments:
    str = "##";
    while (!s.atEnd() && str.at(0)=='#')
      str = s.readLine();

    if (s.atEnd())
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: the file consists from comments; import is not possible");
      return false;
    };

    // determine how many strings have to skip:
    int                         num2skip;
    num2skip = (tMean - t0 + 0.1)/step - (numOfPoints + 1)/2 - 1;
    if (num2skip < 0)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: the file does not contain necessary points; nothing to import");
      return false;
    };
    if (numOfRecords < num2skip + numOfPoints)
    {
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        ": readErpFile(): problems with the ERP file [" + fileName + 
        "]: the file is too old; nothing to import");
      return false;
    };
    //
    // skip the lines
    for (int i=0; i<num2skip; i++)
      str = s.readLine();
    // read the data:
    for (int i=0; i<numOfPoints; i++)
    {
      //          1         2         3         4         5         6         7
      //0123456789012345678901234567890123456789012345678901234567890123456789012345
      //2444074.5 -0.4840  3.8424 -17986194 -.0681 -.0403    -26. 0.000 0.000 0.000
      //2444075.5 -0.4840  3.8424 -17985965 -.0681 -.0403    -26. 0.000 0.000 0.000
      //...
      //2456195.5  1.7226  3.4290 -34616396 -.0752 -.0751  -1974. 0.001 0.000 0.000
      //2456196.5  1.7190  3.4183 -34617166 -.0829 -.0828  -2178. 0.001 0.000 0.000
      //
      // there is no specified format here, just "read is list-directed"
      str = s.readLine();
      double                    pmX, pmY, pmUT1;
      QStringList               strList=str.simplified().split(' ', QString::SkipEmptyParts);
      if (strList.size() < 4)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "readErpFile(): does not look like a proper string: [" + str + 
          "]; import has been interrupted");
        f.close();
        s.setDevice(NULL);
        return false;
      };
      d = strList.at(0).toDouble(&isOk);
      if (!isOk)
      {
        f.close();
        s.setDevice(NULL);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "readErpFile(): cannot convert epoch string to double: [" + strList.at(0) + 
          "]; import has been interrupted");
        return false;
      };
      if (d > 2390000.0)
        d -= 2400000.5;
      pmX = strList.at(1).toDouble(&isOk);
      if (!isOk)
      {
        f.close();
        s.setDevice(NULL);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "readErpFile(): cannot convert PM_X string to double: [" + strList.at(1) + 
          "]; import has been interrupted");
        return false;
      };
      pmX *= 100.0;
      pmY = strList.at(2).toDouble(&isOk);
      if (!isOk)
      {
        f.close();
        s.setDevice(NULL);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "readErpFile(): cannot convert PM_Y string to double: [" + strList.at(2) + 
          "]; import has been interrupted");
        return false;
      };
      pmY *= 100.0;
      pmUT1 = strList.at(3).toDouble(&isOk);
      if (!isOk)
      {
        f.close();
        s.setDevice(NULL);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "readErpFile(): cannot convert PM_UT1 string to double: [" + strList.at(3) + 
          "]; import has been interrupted");
        return false;
      };
      pmUT1 /= 1.0E6;

      argument_->setElement(i, d);
      eopTable_->setElement(i, PMX_IDX, pmX);
      eopTable_->setElement(i, PMY_IDX, pmY);
      eopTable_->setElement(i, UT1_IDX, pmUT1);
      eopTable_->setElement(i, CIX_IDX, 0.0);
      eopTable_->setElement(i, CIY_IDX, 0.0);
    };
    f.close();
    s.setDevice(NULL);
    fileName_ = fileName;
  }
  else
  {
    f.close();
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readErpFile(): unable to open the ERP file [" + fileName + 
      "]; import is not possible");
    return false;
  };
  //
  return true;
};



//
bool SgExternalEopFile::readFinalsFile(const QString& fileName, const SgMJD& tMean, int numOfPoints)
{
  bool                          isOk;
  QFile                         f(fileName);
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readFinalsFile(): the finals EOP file \"" + fileName + 
      "\" does not exist; import of external EOP is not possible");
    return false;
  };
  QString                       str(""), ss("");

  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    double                      d, dFirst;
    double                      pmX, pmY, pmUT1, cipX, cipY;
    int                         numOfReadRecs, l;

    dFirst = tMean.toDouble() - (numOfPoints + 1)/2;

//-------  ------  -------------------------------------------------------------
//19-27    F9.6    Bull. A PM-x (sec. of arc)
//28-36    F9.6    error in PM-x (sec. of arc)
//37       X       [blank]
//38-46    F9.6    Bull. A PM-y (sec. of arc)
//47-55    F9.6    error in PM-y (sec. of arc)
//56-57    2X      [blanks]
//58       A1      IERS (I) or Prediction (P) flag for Bull. A UT1-UTC values
//59-68    F10.7   Bull. A UT1-UTC (sec. of time)
//69-78    F10.7   error in UT1-UTC (sec. of time)
//79       X       [blank]
//80-86    F7.4    Bull. A LOD (msec. of time) -- NOT ALWAYS FILLED
//87-93    F7.4    error in LOD (msec. of time) -- NOT ALWAYS FILLED
//94-95    2X      [blanks]
//96       A1      IERS (I) or Prediction (P) flag for Bull. A nutation values
//97       X       [blank]
//98-106   F9.3    Bull. A dX wrt IAU2000A Nutation (msec. of arc), Free Core Nutation NOT Removed
//107-115  F9.3    error in dX (msec. of arc)
//116      X       [blank]
//117-125  F9.3    Bull. A dY wrt IAU2000A Nutation (msec. of arc), Free Core Nutation NOT Removed
//126-134  F9.3    error in dY (msec. of arc)
//135-144  F10.6   Bull. B PM-x (sec. of arc)
//145-154  F10.6   Bull. B PM-y (sec. of arc)
//155-165  F11.7   Bull. B UT1-UTC (sec. of time)
//166-175  F10.3   Bull. B dX wrt IAU2000A Nutation (msec. of arc)
//176-185  F10.3   Bull. B dY wrt IAU2000A Nutation (msec. of arc)
//


//          1         2         3         4         5         6         7         8         9        10        11        12        13        14        15        16
//01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
//21 325 59298.00 I  0.072757 0.000016  0.407851 0.000024  I-0.1705248 0.0000033 -0.0594 0.0024  I     0.246    0.246     0.051    0.066  0.072812  0.407873 -0.1705313     0.224     0.065         PM-x   sigma      PM-y    sigma        dUT1       sigma     LOD    sig          dX       sig      dY        sig     Bull. B values
//


    numOfReadRecs = 0;
    isOk = true;

    while (!s.atEnd() && numOfReadRecs<numOfPoints)
    {
      pmX = pmY = pmUT1 = cipX = cipY = 0.0;
      str = s.readLine();
      l = str.size();
      if (77 < l)
      {
        d = str.mid(7, 8).toDouble(&isOk);
        if (isOk && dFirst <= d)
        {
          pmX = str.mid( 18,  9).toDouble(&isOk);
          if (isOk)
          {
            pmY = str.mid( 37,  9).toDouble(&isOk);
            if (isOk)
            {
              pmUT1 = str.mid( 58, 10).toDouble(&isOk);
              if (isOk)
              {
                cipX = str.mid( 97,  9).toDouble(&isOk);
                if (isOk)
                {
                  cipY = str.mid(116,  9).toDouble(&isOk);
                  if (isOk)
                  {
                    argument_->setElement(numOfReadRecs, d);
                    eopTable_->setElement(numOfReadRecs, PMX_IDX, pmX*1000.0); // arcsec -> mas
                    eopTable_->setElement(numOfReadRecs, PMY_IDX, pmY*1000.0); // arcsec -> mas
                    eopTable_->setElement(numOfReadRecs, UT1_IDX, pmUT1);      // sec
                    eopTable_->setElement(numOfReadRecs, CIX_IDX, cipX);       // mas
                    eopTable_->setElement(numOfReadRecs, CIY_IDX, cipY);       // mas
                    numOfReadRecs++;
                  };
                };
              };
            };
          };
        };
      };
    };
    f.close();
    s.setDevice(NULL);
    fileName_ = fileName;
    ut1Type_ = SgTidalUt1::CT_FULL;
  }
  else
  {
    f.close();
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readFinalsFile(): unable to open the EOP file \"" + fileName + 
      "\"; import is not possible");
    return false;
  };
  //
  return true;
};



//
bool SgExternalEopFile::readC04File(const QString& fileName, const SgMJD& tMean, int numOfPoints)
{
  bool                          isOk;
  QFile                         f(fileName);
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readC04File(): the finals EOP file \"" + fileName + 
      "\" does not exist; import of external EOP is not possible");
    return false;
  };
  QString                       str(""), ss("");

  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    double                      d, dFirst;
    double                      pmX, pmY, pmUT1, cipX, cipY;
    int                         numOfReadRecs, l;

    dFirst = tMean.toDouble() - (numOfPoints + 1)/2;

//            FORMAT(3(I4),I7,2(F11.6),2(F12.7),2(F11.6),2(F11.6),2(F11.7),2(F12.6))
//##################################################################################
//
//      Date      MJD      x          y        UT1-UTC       LOD         dX        dY        x Err     y Err   UT1-UTC Err  LOD Err     dX Err       dY Err
//                         "          "           s           s          "         "           "          "          s         s            "           "
//     (0h UTC)
//
//          1         2         3         4         5         6         7         8         9        10        11        12        13        14        15
//0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
//1962   1   1  37665  -0.012700   0.213000   0.0326338   0.0017230   0.000000   0.000000   0.030000   0.030000  0.0020000  0.0014000    0.004774    0.002000

// new:
//# YR  MM  DD  HH       MJD        x(")        y(")  UT1-UTC(s)       dX(")      dY(")       xrt(")      yrt(")      LOD(s)        x Er        y Er  UT1-UTC Er      dX Er       dY Er
//          1         2         3         4         5         6         7         8         9        10        11        12        13        14        15
//0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
//1979   1   1   0  43874.00    0.138800    0.085000   0.5984263    0.000000    0.000000    0.000000    0.000000   0.0031970    0.015000    0.015000   0.0019000    0.001989    0.002000


    numOfReadRecs = 0;
    isOk = true;

    while (!s.atEnd() && numOfReadRecs<numOfPoints)
    {
      pmX = pmY = pmUT1 = cipX = cipY = 0.0;
      str = s.readLine();
      l = str.size();
      if (90 < l)
      {
        d = str.mid(16, 10).toDouble(&isOk);
        if (isOk && dFirst <= d)
        {
          pmX = str.mid(27, 11).toDouble(&isOk);
          if (isOk)
          {
            pmY = str.mid(39, 11).toDouble(&isOk);
            if (isOk)
            {
              pmUT1 = str.mid(51, 11).toDouble(&isOk);
              if (isOk)
              {
                cipX = str.mid(63, 11).toDouble(&isOk);
                if (isOk)
                {
                  cipY = str.mid(75, 11).toDouble(&isOk);
                  if (isOk)
                  {
                    argument_->setElement(numOfReadRecs, d);
                    eopTable_->setElement(numOfReadRecs, PMX_IDX, pmX*1000.0); // arcsec -> mas
                    eopTable_->setElement(numOfReadRecs, PMY_IDX, pmY*1000.0); // arcsec -> mas
                    eopTable_->setElement(numOfReadRecs, UT1_IDX, pmUT1);      // sec
                    eopTable_->setElement(numOfReadRecs, CIX_IDX, cipX*1000.0);// arcsec -> mas
                    eopTable_->setElement(numOfReadRecs, CIY_IDX, cipY*1000.0);// arcsec -> mas
                    numOfReadRecs++;
                  };
                };
              };
            };
          };
        };
      };
    };
    f.close();
    s.setDevice(NULL);
    fileName_ = fileName;
    ut1Type_ = SgTidalUt1::CT_FULL;
  }
  else
  {
    f.close();
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readC04File(): unable to open the EOP file \"" + fileName + 
      "\"; import is not possible");
    return false;
  };
  //
  return true;
};


/*=====================================================================================================*/






/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/




/*=====================================================================================================*/
