/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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


#include <signal.h>
#include <string.h>
#include <unistd.h>


#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>

#if QT_VERSION >= 0x050000
#    include <QtWidgets/QApplication>
#else
#    include <QtGui/QApplication>
#endif



#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiSession.h>



#include "vgosDbCalc.h"
#include "VcCalc2SessionIfc.h"



VcCalc2SessionIfc               c2s;
QString                          histIntroRec;




// stubs:
//
void kai_(short &kruc, UNUSED short &in, UNUSED short &iout, UNUSED short &istp,
  char *lkynm_chr, short &iveri, char *lkout_chr, 
  UNUSED char *lfto_chr, short &ivero, UNUSED char *lfio_chr, short &kerr,
  size_t len1, size_t len2, UNUSED size_t len3, UNUSED size_t len4)
{
  QString                       inputDbName(QString::fromLatin1(lkynm_chr, len1));
  QString                       outputDbName(QString::fromLatin1(lkout_chr, len2));
//QString                       inputDbDescriptor("TEMPLATE_LFIO_chr_TEMPLATE_LFIO_chr_TEMPLATE");
//QString                       outputDbDescriptor("TEMPLATE_LFTO_chr_TEMPLATE_LFTO_chr_TEMPLATE");

  if (inputDbName.at(0) == '$')
    inputDbName = inputDbName.mid(1);
  
  c2s.mimic_Kai(kruc, inputDbName, iveri, outputDbName, ivero, kerr);

/*
  if (len3 < 256) // is it FORTRAN's bug?
    qString2fortArray(outputDbDescriptor, lfto_chr,   len3);
  if (len4 < 256)
    qString2fortArray(inputDbDescriptor,  lfio_chr,   len4);
  if (len2 < 256)
    qString2fortArray(outputDbName,       lkout_chr,   len2);
*/
};



//
void wridr_()
{
//  std::cout << ".. wridr().\n";
};



//
void ask_(const char *lcode, const short &ntoc, const short &dim1, const short &dim2,
  const short &dim3, const short &nver, UNUSED const char* ltext_chr, UNUSED const short &ktype, 
  short &kerr, UNUSED size_t len1, UNUSED size_t len2)
{
  if (fortArray2QString(lcode, 8) != "ROTEPOCH")
    logger->write(SgLogger::WRN, SgLogger::IO,
      "ask_(): got a query on LCODE \"" + fortArray2QString(lcode, 8) + "\" (" + 
      QString("").setNum(dim1) + "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) + 
      ") of TOC=" + QString("").setNum(ntoc) + " and ver# " + QString("").setNum(nver));
  
  kerr = 1; // anyway: not found
};



//
void mvrec_(short &ntoc, short &kmode, short &knum, short &kerr)
{
  kerr = c2s.mimic_MvRec(ntoc, kmode, knum);
};



//
void phist_(short &nhist, const char *lhist_chr, size_t len)
{
  QString                        histString("");
  if ((size_t)nhist < len)
    histString = fortArray2QString(lhist_chr, len).left(nhist);
  else
  {
    // try to workaround the CALC's bug:
    histString = fortArray2QString(lhist_chr, len).trimmed();
    if (histString.contains(histIntroRec))
      histString = histIntroRec;
    else
    {
      while (!histString.at(histString.size() - 1).isPrint())
        histString.chop(1);
      histString = histString.trimmed();
    };
  };
  //
  c2s.mimic_pHist(histString);
};



//
void finis_(UNUSED short par)
{
  c2s.mimic_Finis();
};



//
void datsv_() {}; // just a stub



//
//
// adds:
void adda_(const short& ntoc, const char *lcode, const char *descrText,
  const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2)
{
  if (len1 < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "adda_(): improper use of the function: len1 (" + QString("").setNum(len1) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  if (len2 < 32)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "adda_(): improper use of the function: len2 (" + QString("").setNum(len2) + 
      ") is less than 32 for description of the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  QString                       descr(fortArray2QString(descrText, 32));
  c2s.mimicAddA(ntoc, lCode, descr, dim1, dim2, dim3);
};



//
void addi_(const short& ntoc, const char *lcode, const char *descrText,
  const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2)
{
  if (len1 < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addi_(): improper use of the function: len1 (" + QString("").setNum(len1) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  if (len2 < 32)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addi_(): improper use of the function: len2 (" + QString("").setNum(len2) + 
      ") is less than 32 for description of the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  QString                       descr(fortArray2QString(descrText, 32));
  c2s.mimicAddI(ntoc, lCode, descr, dim1, dim2, dim3);
};



//
void addj_(const short& ntoc, const char *lcode, const char *descrText,
  const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2)
{
  if (len1 < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addj_(): improper use of the function: len1 (" + QString("").setNum(len1) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  if (len2 < 32)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addj_(): improper use of the function: len2 (" + QString("").setNum(len2) + 
      ") is less than 32 for description of the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  QString                       descr(fortArray2QString(descrText, 32));
  c2s.mimicAddJ(ntoc, lCode, descr, dim1, dim2, dim3);
};



//
void add4_(const short& ntoc, const char *lcode, const char *descrText,
  const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2)
{
  if (len1 < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "add4_(): improper use of the function: len1 (" + QString("").setNum(len1) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  if (len2 < 32)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "add4_(): improper use of the function: len2 (" + QString("").setNum(len2) + 
      ") is less than 32 for description of the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  QString                       descr(fortArray2QString(descrText, 32));
  c2s.mimicAdd4(ntoc, lCode, descr, dim1, dim2, dim3);
};



//
void addr_(const short& ntoc, const char *lcode, const char *descrText,
  const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2)
{
  if (len1 < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addr_(): improper use of the function: len1 (" + QString("").setNum(len1) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  if (len2 < 32)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "addr_(): improper use of the function: len2 (" + QString("").setNum(len2) + 
      ") is less than 32 for description of the LCODE \"" + fortArray2QString(lcode, len1) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  QString                       descr(fortArray2QString(descrText, 32));
  c2s.mimicAddR(ntoc, lCode, descr, dim1, dim2, dim3);
};




//
//
//                                                 dels:
//
void dela_(const short& ntoc, const char *lcode, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "dela_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicDelA(ntoc, lCode);
};



//
void delr_(const short& ntoc, const char *lcode, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "delr_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicDelR(ntoc, lCode);
};




//
//
//                                                 gets:
void geta_(const char *lcode, char values[], const short &dim1, const short &dim2, const short &dim3,
  short numDims[], short &kerr, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "geta_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  kerr = c2s.mimicGetA(lCode, values, dim1, dim2, dim3, numDims);
//  std::cout << " -------> getA returned:\n";
//  QString                       str("");
//  for (int j=0; j<numDims[1]; j++)
//    for (int k=0; k<numDims[2]; k++)
//    {
//      str = fortArray2QString(values + 2*numDims[0]*j + 2*numDims[0]*numDims[0]*k, numDims[0]*2);
//      std::cout << "     " << j << ":" << k << " = \"" 
//                << qPrintable(str) << "\"\n";
//    };
};


//
void geti_(const char *lcode, short values[], const short &dim1, const short &dim2, const short &dim3,
  short numDims[], short &kerr, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "geti_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  kerr = c2s.mimicGetI(lCode, values, dim1, dim2, dim3, numDims);
//  std::cout << " -------> getI returned:\n";
//  for (int i=0; i<numDims[0]; i++)
//    for (int j=0; j<numDims[1]; j++)
//      for (int k=0; k<numDims[2]; k++)
//        std::cout << "     " << i << ":" << j << ":" << k << " = " 
//                  << values[i + numDims[0]*j + numDims[0]*numDims[1]*k] << "\n";
/*
  std::cout << "  ----\n  ";
  for (int i=0; i<dim1*dim2*dim3; i++)
    std::cout << " (" << i << ")=" << value[i] << ", ";
  std::cout << "\n";
*/
};



//extern "C" void getj_(char (&lcode)[14], int valu[], short *dim1, short *dim2, short *dim3,
//    short numDims[], short &kerr, size_t len);

void get4_(const char *lcode, double values[], const short &dim1, const short &dim2, const short &dim3,
  short numDims[], short &kerr, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "get4_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  kerr = c2s.mimicGet4(lCode, values, dim1, dim2, dim3, numDims);
//  std::cout << " -------> get4 returned:\n";
//  for (int i=0; i<numDims[0]; i++)
//    for (int j=0; j<numDims[1]; j++)
//      for (int k=0; k<numDims[2]; k++)
//        std::cout << "     " << i << ":" << j << ":" << k << " = " 
//                  << values[i + numDims[0]*j + numDims[0]*numDims[1]*k] << "\n";
};





//
//
//                                                 puts:
//
void puta_(const char *lcode, const char vals[], const short &dim1, const short &dim2, const short &dim3,
  size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "puta_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicPutA(lCode, vals, dim1, dim2, dim3);
};



//
void puti_(const char *lcode, const short vals[], 
  const short &dim1, const short &dim2, const short &dim3, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "puti_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicPutI(lCode, vals, dim1, dim2, dim3);
};



//
void putr_(const char *lcode, const double vals[], 
  const short &dim1, const short &dim2, const short &dim3, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "putr_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicPutR(lCode, vals, dim1, dim2, dim3);
};



//
void put4_(const char *lcode, const double vals[],
  const short &dim1, const short &dim2, const short &dim3, size_t len)
{
  if (len < 8)
    logger->write(SgLogger::WRN, SgLogger::IO,
      "put4_(): improper use of the function: len (" + QString("").setNum(len) + 
      ") is less than 8 for the LCODE \"" + fortArray2QString(lcode, len) + "\"");
  QString                       lCode(fortArray2QString(lcode, 8));
  c2s.mimicPut4(lCode, vals, dim1, dim2, dim3);
};




//
void puti_(UNUSED char (&lcode)[14], UNUSED short valu[], UNUSED short *dim1, UNUSED short *dim2, 
  UNUSED short *dim3, UNUSED size_t len)
{
  std::cout << ".. puti().\n";
};





//extern "C" void putj_(char (&lcode)[14], int vals[], 
//                      short *dim1, short *dim2, short *dim3, size_t len);

/*
void put4_(UNUSED char (&lcode)[14], UNUSED double vals[], UNUSED short *dim1, UNUSED short *dim2,
  UNUSED short *dim3, UNUSED size_t len)
{
  std::cout << ".. put4().\n";
};

void putr_(UNUSED char (&lcode)[14], UNUSED double vals[], 
  UNUSED short *dim1, UNUSED short *dim2, UNUSED short *dim3, UNUSED size_t len)
{
  std::cout << ".. putr().\n";
};
*/



//
//                                                 Aux:
//
short gethostname_(char *hostName, size_t len)
{
  return gethostname(hostName, len);
};



//
short trimlen_(char *chrs, size_t len)
{
  short                         idx=len;
  while (idx>0 && (chrs[idx-1] == ' ' || chrs[idx-1] == '\0'))
    idx--;
  return idx;
};



//
void quit_calc_(UNUSED short iPar[])
{
  if (c2s.getCalcCallingMode() == VcCalc2SessionIfc::CCM_Legacy)
  {
    logger->write(SgLogger::DBG, SgLogger::IO,
      "quit_calc_(): Quitting: legacy mode");
    exit(0);
  }
  else
  {
    logger->write(SgLogger::DBG, SgLogger::IO,
      "quit_calc_(): Quitting: modern mode");
  };
};
/*=====================================================================================================*/





/*=====================================================================================================*/
// helpers:
QString  fortArray2QString(const char *chrs, size_t len)
{
  unsigned int                   l=strlen(chrs);
  return QString::fromLatin1(chrs, l<len?l:len);
  std::min(len, len);
};



//
void qString2fortArray(const QString& str, char *chrs, size_t len)
{
  // clear everythig:
  memset(chrs, 0, len);
//  strncpy(chrs, qPrintable(str), len-1);
  strncpy(chrs, qPrintable(str), len);
};
/*=====================================================================================================*/





/*=====================================================================================================*/
//
//
//                                                CALC's main:
//
void mimicCalcMain(UNUSED int iArg1, int iArg2, const char* sArg3, const char* sArg4)
{
  char                          sCalcOnName[128];
  int                           kount, kend;
  short                         iLuOut, iLu;

  memset(sCalcOnName, 0, sizeof(sCalcOnName));
  memset(extrnl_.External_inputs, 0, sizeof(extrnl_.External_inputs));

  iLu = 6;
  iLuOut = iArg2;

  if (sArg3 && strlen(sArg3)>0)
  {
    strcpy(sCalcOnName, sArg3);
    std::cout << "CalcOn file name " << sCalcOnName << "\n";
  };
  if (sArg4 && strlen(sArg4)>0)
  {
    strcpy(extrnl_.External_inputs, sArg4);
    extrnl_.External_aprioris = f_true;
    std::cout << "External_inputs  " << extrnl_.External_inputs << "\n";
  };
  //
  // ok:
  datsv_();
  while (true)
  {
    start_(iLu, sCalcOnName, iLuOut, sizeof(sCalcOnName));
    tocup_();
    initl_(kount);
    
    while (true)
    {
      obsnt_(kount, kend);
      if (kend == 1)
        break;
      drivr_();
      wridr_();
    };
    
    if (iLuOut != -1)
      std::cout << " All data for this experiment has been processed.\n";
    //
    // for modern mode, return to calling function:
    if (c2s.getCalcCallingMode()==VcCalc2SessionIfc::CCM_Modern)
    {
      // just print some funny text:
      start_(iLu, sCalcOnName, iLuOut, sizeof(sCalcOnName));
      return;
    };
    //
  };
};



//
void mimicCalcMain2(UNUSED int iArg1, int iArg2, const char* sArg3, const char* sArg4,
  const QList<QString>& bands)
{
  char                          sCalcOnName[128];
  int                           kount, kend;
  short                         iLuOut, iLu;
  kount = kend = 0;

  memset(sCalcOnName, 0, sizeof(sCalcOnName));
  memset(extrnl_.External_inputs, 0, sizeof(extrnl_.External_inputs));

  iLu = 6;
  iLuOut = iArg2;

  if (sArg3 && strlen(sArg3)>0)
  {
    strcpy(sCalcOnName, sArg3);
    std::cout << "CalcOn file name " << sCalcOnName << "\n";
  };
  if (sArg4 && strlen(sArg4)>0)
  {
    strcpy(extrnl_.External_inputs, sArg4);
    extrnl_.External_aprioris = f_true;
    std::cout << "External_inputs  " << extrnl_.External_inputs << "\n";
  };
  //
  // ok:
  c2s.setBandList(bands);
  datsv_();
  logger->write(SgLogger::DBG, SgLogger::SESSION,
    "mimicCalcMain2(): calling CALC's START");
  start_(iLu, sCalcOnName, iLuOut, sizeof(sCalcOnName));
  logger->write(SgLogger::DBG, SgLogger::SESSION,
    "mimicCalcMain2(): calling CALC's TOCUP");
  tocup_();
  // process for each band:
  for (int i=0; i<bands.size(); i++)
  {
    const QString&               bandKey=bands.at(i);
    // the first band in the list is the primary band:
    c2s.setCurrentBand(bandKey, i==0);

    logger->write(SgLogger::DBG, SgLogger::SESSION,
      "mimicCalcMain2(): processing " + bandKey + "-band has been started");

    initl_(kount);
    while (true)
    {
      obsnt_(kount, kend);
      if (kend == 1)
        break;
      drivr_();
      wridr_();
    };
    logger->write(SgLogger::DBG, SgLogger::SESSION,
      "mimicCalcMain2(): processing " + bandKey + "-band has been finished");
    c2s.rewind();
  };
  if (iLuOut != -1)
    std::cout << " All data for this experiment has been processed.\n";
  start_(iLu, sCalcOnName, iLuOut, sizeof(sCalcOnName));
};
/*=====================================================================================================*/







/*

Solve/CalcInfo.nc:
        double RelativityMessage(Dim000001) ;
                RelativityMessage:LCODE = "REL DATA" ;
                RelativityMessage:CreateTime = "2015/09/14 11:30:01 " ;
                RelativityMessage:Definition = "Relativity mod data (gamma)." ;
 
        char TheoryMessage(Char00079) ;
                TheoryMessage:LCODE = "THE MESS" ;
                TheoryMessage:CreateTime = "2015/09/14 15:14:16 " ;
                TheoryMessage:Definition = "Theory module identification" ;

        char RelativityControl(Char00059) ;
                RelativityControl:LCODE = "REL CFLG" ;
                RelativityControl:CreateTime = "2015/09/14 15:14:16 " ;
                RelativityControl:Definition = "Relativisitc bending use status" ;



Solve/CalcERP.nc:                                                                                ^
        char CalcWobModule(Char00057) ;                                                          |
                CalcWobModule:LCODE = "WOB MESS" ;                                            ?????
                CalcWobModule:CreateTime = "2015/09/14 15:14:16 " ;
                CalcWobModule:Definition = "Wobble message definition." ;
        char CalcUt1Module(Char00054) ;
                CalcUt1Module:LCODE = "UT1 MESS" ;                                            ?????
                CalcUt1Module:CreateTime = "2015/09/14 15:14:16 " ;
                CalcUt1Module:Definition = "UT1 Module message definition" ;
 



Solve/CalcInfo.nc: the lcodes are form dbedit(:-\), not calc:
        double RelativityMessage(DimX000001) ;
                RelativityMessage:CreateTime = "2016/12/23 14:57:53 UTC" ;
                RelativityMessage:Definition = "Relativity mod data (gamma)." ;
                RelativityMessage:LCODE = "REL DATA" ;
                RelativityMessage:Size = 8 ;
        double PrecessionData(DimX000001) ;
                PrecessionData:CreateTime = "2016/12/23 14:57:53 UTC" ;
                PrecessionData:Definition = "Precession constant (asec/cent)." ;
                PrecessionData:LCODE = "PRE DATA" ;
                PrecessionData:Size = 8 ;
        double EarthTideData(DimX000003) ;
                EarthTideData:CreateTime = "2016/12/23 14:57:53 UTC" ;
                EarthTideData:Definition = "Earth tide module data (la. h l)" ;
                EarthTideData:LCODE = "ETD DATA" ;
                EarthTideData:Size = 24 ;


Solve/CalcInfo.nc: it is ot "info", move to CalcOceanLoading or something similar:
        char OceanStationsFlag(Dim000005, Char00004) ;
                OceanStationsFlag:LCODE = "OCE STAT" ;
                OceanStationsFlag:CreateTime = "2015/09/14 15:14:16 " ;
                OceanStationsFlag:Definition = "Ocean loading station status." ;
        double OceanPoleTideCoef(Dim000005, Dim000006) ;
                OceanPoleTideCoef:LCODE = "OPTLCOEF" ;
                OceanPoleTideCoef:CreateTime = "2015/09/14 15:14:16 " ;
                OceanPoleTideCoef:Definition = "Ocean Pole Tide loading Coefficients" ;
                OceanPoleTideCoef:Units = "Radian" ;
                                           ~~~~~~~~                                         <------ ?????
        double OceanUpPhase(Dim000005, Dim000011) ;
                OceanUpPhase:LCODE = "SITOCPHS" ;
                OceanUpPhase:CreateTime = "2015/09/14 15:14:16 " ;
                OceanUpPhase:Definition = "Vert ocean loading phases (rad)." ;
                OceanUpPhase:Units = "Radian" ;
        double OceanUpAmp(Dim000005, Dim000011) ;
                OceanUpAmp:LCODE = "SITOCAMP" ;
                OceanUpAmp:CreateTime = "2015/09/14 15:14:16 " ;
                OceanUpAmp:Definition = "Vert ocean loading ampltudes (m)" ;
                OceanUpAmp:Units = "M" ;
        double OceanHorizontalAmp(Dim000005, Dim000002, Dim000011) ;
                OceanHorizontalAmp:LCODE = "SITHOCAM" ;
                OceanHorizontalAmp:CreateTime = "2015/09/14 15:14:16 " ;
                OceanHorizontalAmp:Definition = "Horz ocean loading ampltudes (m)" ;
                OceanHorizontalAmp:Units = "M" ;
        double OceanHorizontalPhase(Dim000005, Dim000002, Dim000011) ;
                OceanHorizontalPhase:LCODE = "SITHOCPH" ;
                OceanHorizontalPhase:CreateTime = "2015/09/14 15:14:16 " ;
                OceanHorizontalPhase:Definition = "Horz ocean loading phases (rad)." ;
                OceanHorizontalPhase:Units = "Radian" ;

                  ^
                  |
      +order of stations?              OceanLoadingStationList?




  units: all units to the same convention: "M" => "meter"


  per band:
     
      FeedCorrection
  
  
*/









/*=====================================================================================================*/
