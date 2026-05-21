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

#include <iostream>
#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>



#include <Sg3dVector.h>
#include <SgConstants.h>
#include <SgEccRec.h>
#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVgosDb.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>
#include <SgVlbiStationInfo.h>




#include <SgIoAgv.h>
#include <SgIoAgvDriver.h>


//#define LOCAL_DEBUG



extern QString                  strNa;

/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvDatumDescriptor implementation                                                                 */
/*                                                                                                     */
/*=====================================================================================================*/
//
SgAgvDatumDescriptor::SgAgvDatumDescriptor(const QString& lCode, const QString descr, 
  AgvDataScope scp, AgvDataType typ, int d1, int d2, unsigned int exp, bool hasMutableScope) : 
  lCode_(lCode), 
  description_(descr)
{
  dataScope_ = scp; 
  dataType_ = typ; 
  dim1_ = d1; 
  dim2_ = d2; 
  hasData_ = false;
  expectance_ = exp;
  switch (dataScope_)
  {
    case ADS_SESSION:
      dim3_ = 1;
      dim4_ = 1;
    break;
    case ADS_SCAN:
      dim3_ = SD_NumScans;
      dim4_ = 1;
    break;
    case ADS_STATION:
      dim3_ = SD_NumStnPts;
      dim4_ = SD_NumStn;
    break;
    case ADS_BASELINE:
      dim3_ = SD_NumObs;
      dim4_ = 1;
    break;
    case ADS_NONE:
    default:
      dim3_ = 0;
      dim4_ = 0;
    break;
  };
  driver_ = NULL;
  hasMutableScope_ = hasMutableScope;
};



//
SgAgvDatumDescriptor::SgAgvDatumDescriptor(const SgAgvDatumDescriptor& add)
{
  lCode_ = add.getLCode();
  description_ = add.getDescription();
  dataScope_ = add.getDataScope();
  dataType_ = add.getDataType();
  dim1_ = add.getDim1();
  dim2_ = add.getDim2();
  dim3_ = add.getDim3();
  dim4_ = add.getDim4();
  driver_ = add.driver_;
  hasData_ = add.getHasData();
  expectance_ =  add.getExpectance();
  hasMutableScope_ = add.hasMutableScope_;
};



//
int SgAgvDatumDescriptor::effectiveSize(int d) const
{
  if (0 < d)
    return d;
  if (!driver_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::effectiveSize(): the driver is NULL for the lCode \"" + lCode_ + "\"");
    return 0;
  };

  if (d == SD_NumObs)
    d = driver_->getNumOfObs();
  else if (d == SD_NumScans)
    d = driver_->getNumOfScans();
  else if (d == SD_NumChans)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::effectiveSize(): the type SD_NumChans is not used in this format; the lCode is \"" + 
      lCode_ + "\"");
    d = 0;
  }
  else if (d == SD_2NumChans)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::effectiveSize(): the type SD_2NumChans is not used in this format; the lCode is \"" + 
      lCode_ + "\"");
    d = 0;
  }
  else if (d == SD_NumStnPts)
    d = driver_->getMaxNumPerStn();
  else if (d == SD_NumSrc)
    d = driver_->getNumOfSrc();
  else if (d == SD_NumStn)
    d = driver_->getNumOfStn();
  else if (d == SD_NumBands)
    d = driver_->getNumOfBands();
  else if (d == SD_NumBln)
    d = driver_->getNumOfBln();
  else if (d < 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::effectiveSize(): uknown special dimension \"" + QString("").setNum(d) + "\" for the lCode \"" +
      lCode_ + "\"");
    d = 0;
  };
  return d;
//  inline const QMap<QString, int>& getNumOfChansByBand() const {return numOfChansByBand_;};
//  inline int getNumOfStnPts() const {return numOfStnPts_;};
//  inline int getNumOfChans2() const {return numOfChans2_;};
//  inline const QMap<QString, int>& getNumOfObsByStn() const {return numOfObsByStn_;};
};



//
int SgAgvDatumDescriptor::totalSize() const
{
  int                           n;
  if (dataScope_ != ADS_STATION)
    n = totalMaxSize();
  else // special case for stations, data are NOT arrays there:
  {
    n = 0;
    for (QMap<QString, int>::const_iterator it=driver_->getNumOfObsByStn().begin();
      it != driver_->getNumOfObsByStn().end(); ++it)
      n += it.value();
    n *= (dataType_==ADT_CHAR?1:d1())*d2();
  };
  return n;
};



//
QString SgAgvDatumDescriptor::dataType2str(AgvDataType tp)
{
  const QString                 dtype2char[] = {"NO", "C1", "I2", "I4", "I8", "R4", "R8"};
  return dtype2char[tp];
};



//
QString SgAgvDatumDescriptor::dataScope2str(AgvDataScope sc)
{
  const QString                 scope2char[] = {"NON", "SES", "SCA", "STA", "BAS"};
  return scope2char[sc];
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvDatum implementation                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
template<class C> void SgAgvDatum<C>::allocateSpace()
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::allocateSpace(): the descriptor is NULL");
    data_ = NULL;
    return;
  };
//  if (data_)
//    return;     // already allocated

  int                            numOfElements=descriptor_->totalMaxSize();
  if (numOfElements<=0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::allocateSpace(): cannot allocate space for the descriptor \"" + descriptor_->getLCode() + 
      "\": the size, " + QString("").setNum(numOfElements) + ", is unusable");
    data_ = NULL;
    return;
  };
  freeSpace();
  data_ = new C[numOfElements];
  memset((void*)data_, 0, numOfElements*sizeof(C));
};



//
template<class C> void SgAgvDatum<C>::freeSpace()
{
  if (data_)
    delete []data_;
  data_ = NULL;
};



//
template<class C> bool SgAgvDatum<C>::isEmpty()
{
  if (!data_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::isEmpty(): the data are not allocated yet");
    return true;
  };
  //
  for (int i=0; i<descriptor_->totalMaxSize(); i++)
    if (*(data_ + i) != C(0))
      return false;
  return true;
};



//
template<class C> C SgAgvDatum<C>::getValue(int idx1, int idx2, int idx3, int idx4) const
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::getValue(): the descriptor is NULL");
    return (C)0;
  };
  return
    ( idx1<descriptor_->d1() && 
      idx2<descriptor_->d2() && 
      idx3<descriptor_->d3() && 
      idx4<descriptor_->d4()  )?
      *(data_ + idx1 + 
        descriptor_->d1()*idx2 + 
        descriptor_->d1()*descriptor_->d2()*idx3 + 
        descriptor_->d1()*descriptor_->d2()*descriptor_->d3()*idx4) : (C)0;
};



//
template<class C> C& SgAgvDatum<C>::value(int idx1, int idx2, int idx3, int idx4)
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::value(): the descriptor is NULL");
    return *data_;
  };
  if (idx1<0 || descriptor_->d1()<=idx1)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the first index, %d, is out of range [0:%d] ", 
      idx1, descriptor_->d1()) + descriptor_->getLCode());
  if (idx2<0 || descriptor_->d2()<=idx2)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the second index, %d, is out of range [0:%d] ", 
      idx2, descriptor_->d2()) + descriptor_->getLCode());
  if (idx3<0 || descriptor_->d3()<=idx3)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the third index, %d, is out of range [0:%d] ", 
      idx3, descriptor_->d3()) + descriptor_->getLCode());
  if (idx4<0 || descriptor_->d4()<=idx4)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the fourth index, %d, is out of range [0:%d] ", 
      idx4, descriptor_->d4()) + descriptor_->getLCode());
  
  return *(data_ + idx1 + 
    descriptor_->d1()*idx2 + 
    descriptor_->d1()*descriptor_->d2()*idx3 + 
    descriptor_->d1()*descriptor_->d2()*descriptor_->d3()*idx4);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvDatumString implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvDatumString::allocateSpace()
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::allocateSpace(): the descriptor is NULL");
    return;
  };

  //if (data_)
  //  return;     // already allocated

  int                            numOfElements=descriptor_->d2()*descriptor_->d3()*descriptor_->d4();
  if (numOfElements<=0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::allocateSpace(): cannot allocate space for the descriptor \"" + descriptor_->getLCode() + 
      "\": the size, " + QString("").setNum(numOfElements) + ", is unusable");
    return;
  };
  freeSpace();
  data_ = new QString[numOfElements];
  for (int i=0; i<numOfElements; i++)
    *(data_ + i) = descriptor_->d1()>0 ? QString(descriptor_->d1(), QChar(' ')) : QString("");
};



//
void SgAgvDatumString::freeSpace()
{
  if (data_)
    delete []data_;
  data_ = NULL;
};



//
bool SgAgvDatumString::isEmpty()
{
  if (!data_)
    return true;
  //
  int                            d1=descriptor_->d1();
  if (d1 <= 0)
    return true;
  QString                       emptyStr(d1, QChar(' '));
  int                            numOfElements=descriptor_->d2()*descriptor_->d3()*descriptor_->d4();
  //
  //numOfElements = descriptor_->totalSize()/d1
  
  for (int i=0; i<numOfElements; i++)
    if (*(data_ + i) != emptyStr)
      return false;
  return true;
};



//
const QString& SgAgvDatumString::getValue(int idx2, int idx3, int idx4) const
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::getValue(): the descriptor is NULL");
    return strNa;
  };
  return
    ( idx2<descriptor_->d2() && 
      idx3<descriptor_->d3() && 
      idx4<descriptor_->d4()  )?
      *(data_ + idx2 + 
        descriptor_->d2()*idx3 + 
        descriptor_->d2()*descriptor_->d3()*idx4) : strNa;
};



//
QString& SgAgvDatumString::value(int idx2, int idx3, int idx4)
{
  if (!descriptor_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::value(): the descriptor is NULL");
    return *data_;
  };
  if (idx2<0 || descriptor_->d2()<=idx2)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the second index, %d, is out of range [0:%d] ",
        idx2, descriptor_->d2() - 1) + 
      "for " + descriptor_->getLCode());
  if (idx3<0 || descriptor_->d3()<=idx3)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the third index, %d, is out of range [0:%d] ", 
        idx3, descriptor_->d3() - 1) + 
      "for " + descriptor_->getLCode());
  if (idx4<0 || descriptor_->d4()<=idx4)
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      QString().sprintf("::value(): the fourth index, %d, is out of range [0:%d] ", 
        idx4, descriptor_->d4() - 1) + 
      "for " + descriptor_->getLCode());
  
  return *(data_ + idx2 + 
    descriptor_->d2()*idx3 + 
    descriptor_->d2()*descriptor_->d3()*idx4);
};

/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvSection implementation                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
int SgAgvRecord::exportData(QTextStream& ts, const QString& prefix)
{
  ts << prefix << " " << content_ << "\n";
  return 1;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvSection implementation                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvSection::fillDataStructures(const SgVlbiSession */*session*/, SgAgvDriver* /*drv*/)
{
  for (int i=0; i<size(); i++)
  {
    //at(i)->fillDataStructures(session);
  };
  
};



//
int SgAgvSection::importData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
  int                            numOfReadRecords, num2read; //, idx;
  QRegExp                       re(prefix_ + "\\.(\\d+)\\s+(.*)", Qt::CaseInsensitive);
  QString                        str("");
  // bool                          isOk;
  startEpoch_  = SgMJD::currentMJD();
  numOfReadRecords = 0;
  str = ts.readLine();
  if (parseSectionLengthString(str, num2read))
  {
    numOfReadRecords = 1;
    for (int i=0; i<num2read; i++)
    {
      str = ts.readLine();
//    if (re.indexIn(str) != -1)
      if (true || re.indexIn(str) != -1)
      {
//        idx = re.cap(1).toInt(&isOk);
//        if (isOk)
        {
//          if (idx != idx_ + 1)
//            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
//              "::importData(): the current index, " + QString("").setNum(idx_ + 1) +
//              ", does not match the index from input, " + QString("").setNum(idx));
//          append(new SgAgvRecord(this, re.cap(2)));
          append(new SgAgvRecord(this, str.mid(7)));
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::importData(): cannot parse a string \"" + str + "\" as a record of " + prefix_ + 
          " section");
    };
    numOfReadRecords += num2read;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::importData(): read " + QString("").setNum(numOfReadRecords) + " records of the section " + 
      prefix_);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::importData(): error reading the section_length string: \"" + str + "\"");
  };

#ifdef LOCAL_DEBUG
  std::cout << " SgAgvSection::importData finished, prefix=[" << qPrintable(prefix_) << "]"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfReadRecords;
};



//
bool SgAgvSection::parseSectionLengthString(const QString& str, int& num2read)
{
  bool                           isOk;
  QRegExp                       re(prefix_ + "\\.(\\d+)\\s+@section_length:\\s+(\\d+)\\s+.*",
                                  Qt::CaseInsensitive);
  num2read = 0;
  isOk = false;
  if (re.indexIn(str) != -1)
  {
    int                         idx;
    idx = re.cap(1).toInt(&isOk);
    if (isOk)
    {
      num2read = re.cap(2).toInt(&isOk);
      if (isOk)
      {
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::parseSectionLengthString(): section parsing the length string is ok: idx=" + 
          QString("").setNum(idx) + ", num2read=" + QString("").setNum(num2read));
        if (idx != idx_ + 1)
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::parseSectionLengthString(): the current index, " + QString("").setNum(idx_ + 1) +
            ", does not match the index from input, " + QString("").setNum(idx));
      };
    };
  }
  else
  {
    isOk = false;
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseSectionLengthString(): cannot parse the length string \"" + str + "\"");
  };
  return isOk;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvFileSection implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvFileSection::fillDataStructures(const SgVlbiSession *session, SgAgvDriver* /*drv*/)
{
  if (size())
    for (int i=0; i<size(); i++)
      delete at(i);
  clear();

  const SgIoDriver             *inptDrv=session->getInputDriver();
  
  if (inptDrv) // can be NULL, it is ok
  {
    const QList<QString>        inputFileNames=inptDrv->listOfInputFiles();
    for (int i=0; i<inputFileNames.size(); i++)
    {
      append(new SgAgvRecord(this, inputFileNames.at(i)));
    };
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::fillDataStructures(): collected " + QString("").setNum(size()) + " records");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::fillDataStructures(): the input driver is NULL, nothing to do");
};



//
int SgAgvFileSection::exportData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
  int                           numOfWritenRecords=0;
  QString                       prfx(prefix_ + "." + QString("").setNum(idx_ + 1));
  
  ts << prfx << " @section_length: " << size() << " files\n";

  for (int i=0; i<size(); i++)
    numOfWritenRecords += at(i)->exportData(ts, prfx);
  
  return numOfWritenRecords + 1;
};
/*=====================================================================================================*/





/*===========================================================prea==========================================*/
/*                                                                                                     */
/* SgAgvPreaSection implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvPreaSection::fillDataStructures(const SgVlbiSession *session, SgAgvDriver* /*drv*/)
{
  if (size())
    for (int i=0; i<size(); i++)
      delete at(i);
  clear();
  const SgIoDriver             *inptDrv=session->getInputDriver();

  // these records are beyound our control:
  append(new SgAgvRecord(this, "DEF_TYPE: 1 CHARACTER ASCII"));
  append(new SgAgvRecord(this, "DEF_TYPE: 2 INTEGER*2 IEEE-231"));
  append(new SgAgvRecord(this, "DEF_TYPE: 3 INTEGER*4 IEEE-231"));
  append(new SgAgvRecord(this, "DEF_TYPE: 4 REAL*4 IEEE 754-1985"));
  append(new SgAgvRecord(this, "DEF_TYPE: 5 REAL*8 IEEE 754-1985"));
  append(new SgAgvRecord(this, "DEF_TYPE: 6 INTEGER*8 IEEE-231"));
  append(new SgAgvRecord(this, "DEF_CLASS: 81 Session"));
  append(new SgAgvRecord(this, "DEF_CLASS: 82 Scan"));
  append(new SgAgvRecord(this, "DEF_CLASS: 83 Station"));
  append(new SgAgvRecord(this, "DEF_CLASS: 84 Baseline"));

  if (inptDrv) // can be NULL, it is ok
  {
    const SgVersion             *drvVersion=inptDrv->getCurrentDriverVersion();
    const SgIdentities         *ids=inptDrv->getCurrentIdentities();
    if (drvVersion)
    {
      append(new SgAgvRecord(this, "GVH_VERSION: " + 
        libraryVersion.name() + " released on " + 
        libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY)));
      append(new SgAgvRecord(this, "GENERATOR: " + 
        drvVersion->name() + " released on " + 
        drvVersion->getReleaseEpoch().toString(SgMJD::F_DDMonYYYY)));
    };
    append(new SgAgvRecord(this, "CREATED_AT: " + 
      SgMJD::currentMJD().toUtc().toString(SgMJD::F_SOLVE_SPLFL) + " UTC"));
    if (ids)
    {
      append(new SgAgvRecord(this, "CREATED_BY: " + ids->getUserName() + 
        " ( " + ids->getUserEmailAddress() + " ) of " + ids->getAcAbbrevName() ));
    };
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::fillDataStructures(): the input driver is NULL, some information is missed");

  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    "::fillDataStructures(): collected " + QString("").setNum(size()) + " records");
};



//
int SgAgvPreaSection::exportData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
  int                           numOfWritenRecords=0;
  QString                       prfx(prefix_ + "." + QString("").setNum(idx_ + 1));

  ts << prfx << " @section_length: " << size() << " keywords\n";

  for (int i=0; i<size(); i++)
    numOfWritenRecords += at(i)->exportData(ts, prfx);
  
  return numOfWritenRecords + 1;
};



//
int SgAgvPreaSection::importData(QTextStream& ts, SgAgvDriver* drv)
{
  SgMJD                         t(tZero);
  QString                       res;
//PREA.1 GVH_VERSION: SgLib-0.6.4 (Tir-na Nog'th) released on 20 Sep, 2019
//PREA.1 GVH_VERSION: GVH release of 2011.01.22
  QRegExp                       reVer("^GVH_VERSION:\\s+(.+)(?: released on | release of )\\s*(.*)",
                                  Qt::CaseInsensitive);
//PREA.1 GENERATOR: nuSolve-0.6.4 (Corilaine) released on 20 Sep, 2019
//PREA.1 GENERATOR: gvf_transform v 3.0  release of 2019.09.15 AFTER nuSolve-0.6.4
//PREA.1 GENERATOR: PIMA v 2.22   2016.02.29
//PREA.1 GENERATOR: PIMA v 2.30g  2019.02.24
//PREA.1 GENERATOR: mark3_to_gvf   Version of 2007.10.15
  QRegExp                       reGen("^GENERATOR:\\s+(.+)(?: released on | v | Version of )\\s*(.*)",
                                  Qt::CaseInsensitive);
//PREA.1 CREATED_AT: 2019.10.04-18:42:44 UTC
//PREA.1 CREATED_AT: 2016.04.23-22:14:04
  QRegExp                       reCat("^CREATED_AT:\\s+(\\d{4}\\.\\d{2}\\.\\d{2}-\\d{2}:\\d{2}:\\d{2})"
                                      "\\s*([A-Z0-9:+-]*)", 
                                  Qt::CaseInsensitive);
//PREA.1 CREATED_BY: Sergei Bolotin ( sergei.bolotin@nasa.gov ) of NASA GSFC
//PREA.1 CREATED_BY: Leonid Petrov ( lpetrov@astrogeo.home )
  QRegExp                       reCby("^CREATED_BY:\\s+(.+)\\s+\\(\\s*(.+)\\s*\\)\\s*(.*)",
                                  Qt::CaseInsensitive);
  QRegExp                       reOth("^(DEF_TYPE|DEF_CLASS):.*",
                                  Qt::CaseInsensitive);
  int                            numOfReadRecords;
  numOfReadRecords = SgAgvSection::importData(ts, drv);
  SgIdentities                 *ids=drv->getInputIdentities();
//SgVersion                    *ver=drv->getInputDriverVersion();
  SgVersion                      inptVer;
  
  // digest what have get:
  for (int i=0; i<size(); i++)
  {
    const QString&              str=at(i)->content();

    if (reOth.indexIn(str) != -1) // just a stub
    {
    }
    else if (reVer.indexIn(str) != -1)
    {
      res = reVer.cap(1);
      if (inptVer.parseString(res))
      {
        t.fromString(SgMJD::F_DDMonYYYY, reVer.cap(2));
        inptVer.setReleaseEpoch(t);
        if (inptVer.getSoftwareName() == libraryVersion.getSoftwareName())
        {
          drv->setExpectedStyle(ACS_NATIVE);
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::importData(): content style has been set to ACS_NATIVE");
        };
      }
      else // foreign:
      {
        inptVer.setSoftwareName(reVer.cap(1));
        t.fromString(SgMJD::F_YYYYMMDD, reVer.cap(2));
        inptVer.setReleaseEpoch(t);
        if (inptVer.getSoftwareName() == "GVH")
          drv->setExpectedStyle(ACS_GVH);
      };
    }
    else if (reGen.indexIn(str) != -1)
    {
      res = reGen.cap(1);
      if (inptVer.parseString(res))
      {
        t.fromString(SgMJD::F_DDMonYYYY, reGen.cap(2));
        inptVer.setReleaseEpoch(t);
      }
      else // foreign:
      {
//PREA.1 GENERATOR: nuSolve-0.6.4 (Corilaine) released on 20 Sep, 2019
//PREA.1 GENERATOR: gvf_transform v 3.0  release of 2019.09.15 AFTER nuSolve-0.6.4
//PREA.1 GENERATOR: PIMA v 2.22   2016.02.29
//PREA.1 GENERATOR: PIMA v 2.30g  2019.02.24
//PREA.1 GENERATOR: mark3_to_gvf   Version of 2007.10.15
                                //            1         2    3        4
        QRegExp                 reAft("\\s*(\\d+)\\.?(\\d*)(.*)\\s*(\\d{4}\\.\\d{2}\\.\\d{2})\\s*AFTER\\s*nuSolve",
                                  Qt::CaseInsensitive);
        QRegExp                 reDte("\\s*(.*)\\s*(\\d{4}\\.\\d{2}\\.\\d{2})\\s*",
                                  Qt::CaseInsensitive);

        QRegExp                 reVr("\\s*(\\d+)\\.?(\\d*)(.*)\\s*",
                                  Qt::CaseInsensitive);

        int                     n;
        bool                     isOk;
        inptVer.setSoftwareName(res);
        res = reGen.cap(2);
   
        if (reAft.indexIn(res) != -1)
        {
          res = "";
          if (reAft.captureCount() == 4)
          {
            t.fromString(SgMJD::F_YYYYMMDD, reAft.cap(4));
            res = reAft.cap(2);
          }
          else if (reAft.captureCount() == 3)
            t.fromString(SgMJD::F_YYYYMMDD, reAft.cap(3));
          else if (reAft.captureCount() == 2)
            t.fromString(SgMJD::F_YYYYMMDD, reAft.cap(2));
          else
            logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
              "::importData(): do not know what to do with \"" + reAft.cap(0) + "\"");
        
          inptVer.setReleaseEpoch(t);

          n = reAft.cap(1).toInt(&isOk);
          if (isOk)
            inptVer.setMajorNumber(n);
          if (res.size())
          {
            n = res.toInt(&isOk);
            if (isOk)
              inptVer.setMinorNumber(n);
          };
        }
        else if (reDte.indexIn(res) != -1)
        {
          res = reDte.cap(2);
          t.fromString(SgMJD::F_YYYYMMDD, res);
          inptVer.setReleaseEpoch(t);
          res = reDte.cap(1);
          if (reVr.indexIn(res) != -1)
          {
            if (reVr.cap(1).size())
            {
              n = reVr.cap(1).toInt(&isOk);
              if (isOk)
                inptVer.setMajorNumber(n);
            };
            if (reVr.cap(2).size())
            {
              n = reVr.cap(2).toInt(&isOk);
              if (isOk)
                inptVer.setMinorNumber(n);
            };
            if (reVr.cap(3).size())
              inptVer.setCodeName(reVr.cap(3));
          };
        };
        
        if (inptVer.getSoftwareName() == "gvf_transform")
        {
          drv->setExpectedStyle(ACS_GVH_VDB);
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::importData(): content style has been set to ACS_GVH_VDB");
        }
        else if (inptVer.getSoftwareName() == "mark3_to_gvf")
        {
          drv->setExpectedStyle(ACS_GVH_DBH);
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::importData(): content style has been set to ACS_GVH_DBH");
        }
        else if (inptVer.getSoftwareName() == "PIMA")
        {
          drv->setExpectedStyle(ACS_GVH_PIMA);
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::importData(): content style has been set to ACS_GVH_PIMA");
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::importData(): cannot determine content style from a string \"" + str + "\"");
      };
    }
    else if (reCat.indexIn(str) != -1)
    {
      t.fromString(SgMJD::F_SOLVE_SPLFL, reCat.cap(1));
      drv->setDateOfCreation(t);
      res = reCat.cap(2);
      if (res.size() && res != "UTC") // have to add support for time zones
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::importData(): got a non standard time zone: \"" + res + "\", need to fix this");
    }
    else if (reCby.indexIn(str) != -1 && ids)
    {
      ids->setUserName(reCby.cap(1));
      if (reCby.cap(2).size())
        ids->setUserEmailAddress(reCby.cap(2).simplified());
      res = reCby.cap(3).simplified();
      if (res.startsWith("of "))
        res = res.mid(3);
      if (res.size())
        ids->setAcAbbrevName(res);
    }
    else // something new
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::importData(): got unknown keyword: \"" + str + "\"");
    };
  };

  
  
  if (inptVer.getSoftwareName().size())
    ids->setDriverVersion(inptVer);

#ifdef LOCAL_DEBUG
  std::cout << " SgAgvPreaSection::importData finished"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfReadRecords;
};
/*=====================================================================================================*/




//
/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvTextSection implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvTextSection::fillDataStructures(const SgVlbiSession *session, SgAgvDriver* /*drv*/)
{
  if (size())
    for (int i=0; i<size(); i++)
      delete at(i);
  clear();
  headerByIdx_.clear();
  numByIdx_.clear();
  maxLenByIdx_.clear();
  
  SgMJD                          t(tZero);
  int                            v(-1);
  int                           chapterIdx(-1);
  const SgVlbiBand             *pBand=session->primaryBand();
  if (pBand)
  {
    const SgVlbiHistory         *history=&pBand->history();
    if (history)
    {
      for (int i=0; i<history->size() - 1 /* we do not need here the last record*/; i++)
//      for (int i=0; i<history->size(); i++)
      {
        const SgVlbiHistoryRecord
                               *hRec=history->at(i);
        if (t < hRec->getEpoch() || v != hRec->getVersion())
        {
          chapterIdx++;
          t = hRec->getEpoch();
          v = hRec->getVersion();
          headerByIdx_[chapterIdx] = "Comments of the vgosDb database " + session->getName() +
            " Version " + QString("").setNum(hRec->getVersion()) + " " + t.toString(SgMJD::F_SOLVE_SPLFL);
          numByIdx_[chapterIdx] = 0;
          maxLenByIdx_[chapterIdx] = hRec->getText().size();
        };
        append(new SgAgvRecord(this, hRec->getText(), chapterIdx));
        numByIdx_[chapterIdx]++;
        if (maxLenByIdx_[chapterIdx] < hRec->getText().size())
          maxLenByIdx_[chapterIdx] = hRec->getText().size();
      };
    }
  };
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    "::fillDataStructures(): collected " + QString("").setNum(size()) + " records");
};



//
int SgAgvTextSection::exportData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
startEpoch_ = SgMJD::currentMJD();

  int                            numOfWritenRecords=0;
  int                           chapterIdx;
  QString                       prfx(prefix_ + "." + QString("").setNum(idx_ + 1));

  ts << prfx << " @section_length: " << headerByIdx_.size() << " chapters\n";

  chapterIdx = -1;
  for (int i=0; i<size(); i++)
  {
    const SgAgvRecord           *rec=at(i);
    if (chapterIdx < rec->getSubIdx())
    {
      chapterIdx++;
      ts   << prfx << " @@chapter: " << chapterIdx + 1
          << " " << numByIdx_.value(chapterIdx)
          << "  records, max_len: " << maxLenByIdx_.value(chapterIdx)
          << " charaters " << headerByIdx_.value(chapterIdx)
          << "\n";
      numOfWritenRecords++;
    }
    numOfWritenRecords += at(i)->exportData(ts, prfx);
  };

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvTextSection::exportData: done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfWritenRecords + 1;
};



//
int SgAgvTextSection::importData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
  startEpoch_ = SgMJD::currentMJD();
  
  int                            numOfReadRecords, numChapter2read, numRecord2read, idx;
  QRegExp                       re(prefix_ + "\\.(\\d+)\\s+(.*)", Qt::CaseInsensitive);
  QString                        str("");
  bool                           isOk;
  
  history_.clear();
  numOfReadRecords = 0;
  str = ts.readLine();
  if (parseSectionLengthString(str, numChapter2read))
  {
    numOfReadRecords = 1;
    for (int i=0; i<numChapter2read; i++)
    {
      str = ts.readLine();
      numOfReadRecords++;
      if (parseChapterInitString(str, numRecord2read))
      {
        for (int j=0; j<numRecord2read; j++)
        {
          str = ts.readLine();
          if (re.indexIn(str) != -1)
          {
            idx = re.cap(1).toInt(&isOk);
            if (isOk)
            {
              if (idx != idx_ + 1)
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::importData(): the current index, " + QString("").setNum(idx_ + 1) +
                  ", does not match the index from input, " + QString("").setNum(idx));
              append(new SgAgvRecord(this, re.cap(2)));
              history_.append(new SgVlbiHistoryRecord(histEpoch_, histVer_, re.cap(2)));
            };
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::importData(): cannot parse a string \"" + str + "\" as a record of " + prefix_ + 
              " section");
        };
        numOfReadRecords += numRecord2read;
      }
      else
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::importData(): parsing chapter string has failed: \"" + str + "\"");
    };
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::importData(): read " + QString("").setNum(numOfReadRecords) + " records of the section " + 
      prefix_);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::importData(): error reading the section_length string: \"" + str + "\"");
  };

#ifdef LOCAL_DEBUG
  std::cout << " SgAgvTextSection::importData finished"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfReadRecords;
};



//
bool SgAgvTextSection::parseChapterInitString(const QString& str, int& num2read)
{
  bool                           isOk;
  //TEXT.1   @@chapter: 1    225  records, max_len:      72 charaters Comments of the Mark-3 DBH database $18JAN02XA Version 1  2018.01.17-22:40:17.
  //TEXT.1 @@chapter: 1 191  records, max_len: 72 charaters Comments of the Mark-3 DBH database 18JUN29XU Version 1 2018.06.30-01:32:39
  QRegExp                       re(prefix_ + "\\.(\\d+)\\s+@*@chapter:\\s+(\\d+)\\s+(\\d+)\\s+records,\\s+.*",
                                  Qt::CaseInsensitive);
  QRegExp                       reVer(".*\\s+Version\\s+(\\d+)\\s+(\\d{4}\\.\\d{2}\\.\\d{2}-\\d{2}:\\d{2}:\\d{2}).*",
                                  Qt::CaseInsensitive);

  num2read = 0;
  isOk = false;
  if (re.indexIn(str) != -1)
  {
    int                         idx, chapterIdx;
    idx = re.cap(1).toInt(&isOk);
    if (isOk)
    {
      chapterIdx = re.cap(2).toInt(&isOk);
      if (isOk)
      {
        num2read = re.cap(3).toInt(&isOk);
        if (isOk)
        {
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::parseChapterInitString(): parsing the chapter string is ok: idx=" + 
            QString("").setNum(idx) + ", chapterIdx=" + QString("").setNum(chapterIdx) + 
            ", num2read=" + QString("").setNum(num2read));
          if (reVer.indexIn(str) != -1)
          {
            histEpoch_.fromString(SgMJD::F_SOLVE_SPLFL, reVer.cap(2));
            histVer_ = reVer.cap(1).toInt(&isOk);
          }
          else
            logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
              "::parseChapterInitString(): cannot figure out version and epoch from the string \"" +
              str + "\"");

        };
      };
    };
  }
  else
  {
    isOk = false;
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseChapterInitString(): cannot parse the chapter string \"" + str + "\"");
  };
  return isOk;
};

/*=====================================================================================================*/




//
/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvTocsSection implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvTocsSection::fillDataStructures(const SgVlbiSession */*session*/, SgAgvDriver* drv)
{
  SgAgvDatumDescriptor       *dd;
  if (size())
    for (int i=0; i<size(); i++)
      delete at(i);
  clear();

  // first, process the mandatory entries:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->mandatoryDatumByKey().begin(); 
    it!=drv->mandatoryDatumByKey().end(); ++it)
  {
    dd = it.value();
    if (dd->getHasData())
      append(datumDescriptor2agvRecord(dd));
  };
  // then, everything else:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->datumByKey().begin(); 
    it!=drv->datumByKey().end(); ++it)
  {
    dd = it.value();
    if (dd->getHasData())
      append(datumDescriptor2agvRecord(dd));
  };

  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    "::fillDataStructures(): collected " + QString("").setNum(size()) + " records");
};



//
SgAgvRecord* SgAgvTocsSection::datumDescriptor2agvRecord(SgAgvDatumDescriptor* dd)
{
  const char*                   scope2char[] = {"NON", "SES", "SCA", "STA", "BAS"};
  const char*                   dtype2char[] = {"NO", "C1", "I2", "I4", "I8", "R4", "R8"};
  return new SgAgvRecord(this,
    QString("").sprintf("%-8s   %3s  %2s %3d %3d  ",
      qPrintable(dd->getLCode()), 
      scope2char[dd->getDataScope()], 
      dtype2char[dd->getDataType()], 
      dd->d1(), 
      dd->d2()) 
    + dd->getDescription());
};



//
SgAgvDatumDescriptor* SgAgvTocsSection::agvRecord2datumDescriptor(SgAgvRecord* rec)
{
  //POLARZ1    BAS  C1  32   1  Space separated polarization per sta/chan in band 1
  QRegExp                       re("(\\S+)\\s+([A-Z]{3})\\s+([CIR][1248])\\s+(\\d+)\\s+(\\d+)\\s+(.*)",
                                  Qt::CaseInsensitive);
  bool                          isOk;
  QString                       lCode(""), descr(""), str("");
  AgvDataScope                  dScope;
  AgvDataType                    dType;
  int                            d1, d2;
  SgAgvDatumDescriptor         *dd;
  
  dd = NULL;
  if (re.indexIn(rec->content()) != -1)
  {
    lCode = re.cap(1).leftJustified(8, ' ');
    descr = re.cap(6).simplified();
    //
    str = re.cap(2);
    if (str == "SES")
      dScope = ADS_SESSION;
    else if (str == "SCA")
      dScope = ADS_SCAN;
    else if (str == "STA")
      dScope = ADS_STATION;
    else if (str == "BAS")
      dScope = ADS_BASELINE;
    else
      dScope = ADS_NONE;
    //
    str = re.cap(3);
    if (str == "C1")
      dType = ADT_CHAR;
    else if (str == "I2")
      dType = ADT_I2;
    else if (str == "I4")
      dType = ADT_I4;
    else if (str == "I8")
      dType = ADT_I8;
    else if (str == "R4")
      dType = ADT_R4;
    else if (str == "R8")
      dType = ADT_R8;
    else
      dType = ADT_NONE;
    //
    d1 = re.cap(4).toInt(&isOk);
    if (!isOk)
      d1 = -1;
    d2 = re.cap(5).toInt(&isOk);
    if (!isOk)
      d2 = -1;
    //
    if (dScope!=ADS_NONE && dType!=ADT_NONE && d1!=-1 && d2!=-1)
      dd = new SgAgvDatumDescriptor(lCode, descr, dScope, dType, d1, d2, ACS_ANY);
    else
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::agvRecord2datumDescriptor(): cannot create datumDescriptor from for a TOC record \"" + 
        rec->content() + "\"");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::agvRecord2datumDescriptor(): cannot parse a TOC record \"" + rec->content() + "\"");
  
  return dd;
};



//
int SgAgvTocsSection::exportData(QTextStream& ts, SgAgvDriver* /*drv*/)
{
startEpoch_ = SgMJD::currentMJD();
  int                            numOfWritenRecords=0;
  QString                       prfx(prefix_ + "." + QString("").setNum(idx_ + 1));

  ts << prfx << " @section_length: " << size() << " lcodes\n";

  for (int i=0; i<size(); i++)
    numOfWritenRecords += at(i)->exportData(ts, prfx);

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvTocsSection::exportData: done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfWritenRecords + 1;
};



//
int SgAgvTocsSection::importData(QTextStream& ts, SgAgvDriver* drv)
{
  int                            numOfReadRecords;
  SgAgvDatumDescriptor         *dd;
  numOfReadRecords = SgAgvSection::importData(ts, drv);
  
#ifdef LOCAL_DEBUG
  std::cout << " SgAgvTocsSection::importData: (" << idx_ << ") begin\n";
#endif
  
  for (int i=0; i<size(); i++)
  {
    dd = agvRecord2datumDescriptor(at(i));
    if (dd)
      drv->addDatumDescriptor(dd);
  };
#ifdef LOCAL_DEBUG
  std::cout << " SgAgvTocsSection::importData: (" << idx_ << ") end"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif
  
  return numOfReadRecords;
};
/*=====================================================================================================*/





//
/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvTocsSection implementation                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvDataSection::fillDataStructures(const SgVlbiSession */*session*/, SgAgvDriver* drv)
{
  if (size())
    for (int i=0; i<size(); i++)
      delete at(i);
  clear();

  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
    "::fillDataStructures(): there are " + 
    QString("").setNum(drv->mandatoryDatumByKey().size() + drv->datumByKey().size()) + 
    " registered entries for output");
};



//
int SgAgvDataSection::exportData(QTextStream& ts, SgAgvDriver* drv)
{
startEpoch_ = SgMJD::currentMJD();

  int                            numOfWritenRecords=0, num2write=0;

  // evaluate number of records to write:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->mandatoryDatumByKey().begin();
    it!=drv->mandatoryDatumByKey().end(); ++it)
    if (it.value()->getHasData())
      num2write += it.value()->totalSize();

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvDataSection::exportData: calced num of mandatory records"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->datumByKey().begin();
    it!=drv->datumByKey().end(); ++it)
    if (it.value()->getHasData())
      num2write += it.value()->totalSize();

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvDataSection::exportData: calced num of other records"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  
  QString                       prfx(prefix_ + "." + QString("").setNum(idx_ + 1));
  ts << prfx << " @section_length: " << num2write << " records\n";

  // make output:
  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->mandatoryDatumByKey().begin();
    it!=drv->mandatoryDatumByKey().end(); ++it)
    numOfWritenRecords += writeDatumOpt(ts, it.value(), prfx, drv);
//    numOfWritenRecords += writeDatum(ts, it.value(), prfx, drv);

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvDataSection::exportData: wrote mandatory records"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  for (QMap<QString, SgAgvDatumDescriptor*>::iterator it=drv->datumByKey().begin();
    it!=drv->datumByKey().end(); ++it)
    numOfWritenRecords += writeDatumOpt(ts, it.value(), prfx, drv);
//    numOfWritenRecords += writeDatum(ts, it.value(), prfx, drv);

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvDataSection::exportData: wrote other records"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
  std::cout << "SgAgvDataSection::exportData: done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfWritenRecords + 1;
};



//
int SgAgvDataSection::writeDatum(QTextStream& ts, SgAgvDatumDescriptor* dd, const QString& prfx, 
  SgAgvDriver* drv)
{
  int                           numOfWritenRecords=0;
  int                           d1, d2, d3, d4;
  if (!dd->getHasData())
    return numOfWritenRecords;
  //
  QString                       content("");
  d1 = dd->d1();
  d2 = dd->d2();
  d3 = dd->d3();
  d4 = dd->d4();
  if (dd->getDataType() == ADT_CHAR)
  {
    for (int l=0; l<d4; l++)
    {
      if (dd->getDataScope()==ADS_STATION)
        d3 = drv->getNumOfStnPts(l);
      for (int k=0; k<d3; k++)
        for (int j=0; j<d2; j++)
        {
/*
          content.sprintf(" %-8s %d %d %2d %2d ", qPrintable(dd->getLCode()), k + 1, l + 1, 1, j + 1);
          ts << prfx << content << drv->data2str(dd, 0, j, k, l) << "\n";
          ts << prfx << " " << "\n";
*/
          ts 
            << prfx 
            << " " 
            << dd->getLCode().leftJustified(8, ' ') << " "
            << QString("").setNum(k + 1) << " "
            << QString("").setNum(l + 1) << " 1 "
            << QString("").setNum(j + 1) << " "
            << drv->data2str(dd, 0, j, k, l)
            << "\n";

          numOfWritenRecords++;
        };
    };
  }
  else
  {
    for (int l=0; l<d4; l++)
    { 
      if (dd->getDataScope()==ADS_STATION)
        d3 = drv->getNumOfStnPts(l);
      for (int k=0; k<d3; k++)
        for (int j=0; j<d2; j++)
          for (int i=0; i<d1; i++)
          {
            /*
            content.sprintf("%-8s %d %d %2d %2d ",
              qPrintable(dd->getLCode()), k + 1, l + 1,  i + 1, j + 1);
            ts << prfx << " " << content << drv->data2str(dd, i, j, k, l) << "\n";
            */
            ts 
              << prfx 
              << " " 
              << dd->getLCode().leftJustified(8, ' ') << " "
              << content
              << QString("").setNum(k + 1) << " "
              << QString("").setNum(l + 1) << " "
              << QString("").setNum(i + 1) << " "
              << QString("").setNum(j + 1) << " "
              << drv->data2str(dd, i, j, k, l) 
              << "\n";

            numOfWritenRecords++;
          };
    };
  };
  return numOfWritenRecords;
};



//
int SgAgvDataSection::writeDatumOpt(QTextStream& ts, SgAgvDatumDescriptor* dd, const QString& prfx,
  SgAgvDriver* drv)
{
  int                           numOfWritenRecords=0;
  int                           d1, d2, d3, d4;
  if (!dd->getHasData())
    return numOfWritenRecords;
  //
  QString                       content("N/A");
  SgAgvDatumString             *dstr=NULL;
  SgAgvDatum<short int>        *dsin=NULL;
  SgAgvDatum<int>              *dint=NULL;
  SgAgvDatum<long int>         *dlin=NULL;
  SgAgvDatum<float>            *dflt=NULL;
  SgAgvDatum<double>           *ddbl=NULL;

  d1 = dd->d1();
  d2 = dd->d2();
  d3 = dd->d3();
  d4 = dd->d4();
  
  switch (dd->getDataType())
  {
    case ADT_CHAR:
      dstr = drv->c1Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      {
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
          {
            content = dstr->getValue(j, k, l).trimmed();
            content.replace(' ', '_');
            ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
              << QString("").setNum(k + 1) << " "  << QString("").setNum(l + 1) << " 1 "
              << QString("").setNum(j + 1) << " "  << content << "\n";
            numOfWritenRecords++;
          };
      };
    break;
    case ADT_I2:
      dsin = drv->i2Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      { 
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
            for (int i=0; i<d1; i++)
            {
              content.setNum(dsin->getValue(i, j, k, l));
              ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
                << QString("").setNum(k + 1) << " " << QString("").setNum(l + 1) << " "
                << QString("").setNum(i + 1) << " " << QString("").setNum(j + 1) << " "  
                << content << "\n";
            };
            numOfWritenRecords++;
      };
    break;
    case ADT_I4:
      dint = drv->i4Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      { 
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
            for (int i=0; i<d1; i++)
            {
              content.setNum(dint->getValue(i, j, k, l));
              ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
                << QString("").setNum(k + 1) << " "  << QString("").setNum(l + 1) << " "
                << QString("").setNum(i + 1) << " "  << QString("").setNum(j + 1) << " "
                << content << "\n";
              numOfWritenRecords++;
            };
      };
    break;
    case ADT_I8:
      dlin = drv->i8Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      { 
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
            for (int i=0; i<d1; i++)
            {
              content.setNum(dlin->getValue(i, j, k, l));
              ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
                << QString("").setNum(k + 1) << " "  << QString("").setNum(l + 1) << " "
                << QString("").setNum(i + 1) << " "  << QString("").setNum(j + 1) << " "
                << content << "\n";
              numOfWritenRecords++;
            };
      };
    break;
    case ADT_R4:
      dflt = drv->r4Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      { 
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
            for (int i=0; i<d1; i++)
            {
              content.setNum(dflt->getValue(i, j, k, l), 'E', 7);
              ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
                << QString("").setNum(k + 1) << " "  << QString("").setNum(l + 1) << " "
                << QString("").setNum(i + 1) << " "  << QString("").setNum(j + 1) << " "
                << content << "\n";
              numOfWritenRecords++;
            };
      };
    break;
    case ADT_R8:
      ddbl = drv->r8Data().value(dd->getLCode());
      for (int l=0; l<d4; l++)
      { 
        if (dd->getDataScope()==ADS_STATION)
          d3 = drv->getNumOfStnPts(l);
        for (int k=0; k<d3; k++)
          for (int j=0; j<d2; j++)
            for (int i=0; i<d1; i++)
            {
              content.setNum(ddbl->getValue(i, j, k, l), 'E', 15);
              ts << prfx << " " << dd->getLCode().leftJustified(8, ' ') << " "
                << QString("").setNum(k + 1) << " "  << QString("").setNum(l + 1) << " "
                << QString("").setNum(i + 1) << " "  << QString("").setNum(j + 1) << " "
                << content << "\n";
              numOfWritenRecords++;
            };
      };
    break;
    case ADT_NONE:
    default:
    break;
  };

  return numOfWritenRecords;
};



//
int SgAgvDataSection::importData(QTextStream& ts, SgAgvDriver* drv)
{
  int                            numOfReadRecords;
#ifdef LOCAL_DEBUG
  std::cout << " SgAgvDataSection::importData: (" << idx_ << ") start\n";
#endif
  numOfReadRecords = SgAgvSection::importData(ts, drv);

  // it is a first chunk and the special dimensions are not known yet:
  if (drv->getNumOfObs()==0 || drv->getNumOfStn()==0 || drv->getNumOfScans()==0)
    drv->figureOutImplicitDimensions(this); // grrr..

  if (drv->getNumOfObs()==0 || drv->getNumOfStn()==0 || drv->getNumOfScans()==0) // failed
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::importData(): parsing the mandatory records have failed");
    return numOfReadRecords;
  };

  drv->allocateData();
#ifdef LOCAL_DEBUG
  std::cout << " SgAgvDataSection::importData: allocating data: (" << idx_ << ") done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

//  drv->digestData(this);
  drv->digestDataNoRegEx(this);
#ifdef LOCAL_DEBUG
  std::cout << " SgAgvDataSection::importData: digesting data: (" << idx_ << ") done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
  std::cout << " SgAgvDataSection::importData finished"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif

  return numOfReadRecords;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgAgvChunk implementation                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
void SgAgvChunk::fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv)
{
  sFile_.fillDataStructures(session, drv);
  sPrea_.fillDataStructures(session, drv);
  sText_.fillDataStructures(session, drv);
  sTocs_.fillDataStructures(session, drv);
  sData_.fillDataStructures(session, drv);
  sHeap_.fillDataStructures(session, drv);
};
  


//
void SgAgvChunk::exportData(QTextStream& ts, SgAgvDriver* drv)
{
SgMJD startEpoch_  = SgMJD::currentMJD();

  int                            numOfWritenRecords;
  ts << sMagicString_ << "\n";

  numOfWritenRecords = 0;

  numOfWritenRecords += sFile_.exportData(ts, drv);
  numOfWritenRecords += sPrea_.exportData(ts, drv);
  numOfWritenRecords += sText_.exportData(ts, drv);
  numOfWritenRecords += sTocs_.exportData(ts, drv);
  numOfWritenRecords += sData_.exportData(ts, drv);
  numOfWritenRecords += sHeap_.exportData(ts, drv);

  ts << "CHUN." << idx_ + 1 << " @chunk_size: " << numOfWritenRecords << " records\n";

#ifdef LOCAL_DEBUG
  std::cout << "SgAgvChunk::exportData: done"
            << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch_)*86400000.0))
            << "\n";
#endif
};



//
void SgAgvChunk::importData(QTextStream& ts, SgAgvDriver* drv)
{
  int                            numOfReadRecords;
  QString                        str("");

  SgMJD                         startEpoch(SgMJD::currentMJD());

  str = ts.readLine();
  if (parseMagicString(str))
  {
#ifdef LOCAL_DEBUG
    std::cout << " SgAgvChunk::importData: (" << idx_ << ") start\n";
#endif
    numOfReadRecords = 1;

    numOfReadRecords += sFile_.importData(ts, drv);
    numOfReadRecords += sPrea_.importData(ts, drv);
    numOfReadRecords += sText_.importData(ts, drv);
    numOfReadRecords += sTocs_.importData(ts, drv);
    numOfReadRecords += sData_.importData(ts, drv);
    numOfReadRecords += sHeap_.importData(ts, drv);
    
    str = ts.readLine();
    numOfReadRecords++;
    
    if (parseEndString(str))
    {
      numOfReadRecords++;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::importData(): read " + QString("").setNum(numOfReadRecords) + " records of the chunk #" + 
        QString("").setNum(idx_));
      //
#ifdef LOCAL_DEBUG
      std::cout << " SgAgvChunk::importData: (" << idx_ << ") done"
                << qPrintable(QString("").sprintf(", dt= %.2f ms", (SgMJD::currentMJD() - startEpoch)*86400000.0))
                << "\n";
#endif
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::importData(): error parsing the ending string: \"" + str + "\"");
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::importData(): error parsing the magic string: \"" + str + "\"");
    return;
  };
};



//
bool SgAgvChunk::parseMagicString(const QString& str)
{
  bool                           isOk;
                                // "AGV format of 2005.01.14"
  QRegExp                       reMagicDate(magicPrefix_ + "\\s*(\\d+)\\.(\\d+)\\.(\\d+).*",
                                  Qt::CaseInsensitive);
  QRegExp                       reMagicStr(magicPrefix_ + "\\s*(.+)",  Qt::CaseInsensitive);
  if (reMagicDate.indexIn(str) != -1)
  {
    int                         vYr, vMn, vDy;
    vYr = reMagicDate.cap(1).toInt(&isOk);
    if (isOk)
    {
      vMn = reMagicDate.cap(2).toInt(&isOk);
      if (isOk)
      {
        vDy = reMagicDate.cap(3).toInt(&isOk);
        if (isOk)
        {
          inputFmtDate_ = SgMJD(vYr, vMn, vDy);
          logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
            "::parseMagicString(): found format version of " + inputFmtDate_.toString(SgMJD::F_Date));
        }
      };
    };
  }
  else
  {
    isOk = false;
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseMagicString(): cannot extract a date from format string \"" + str + "\"");
  };
  //
  if (reMagicStr.indexIn(str) != -1)
  {
    inputFmtVersion_ = reMagicStr.cap(1);
  }
  else
  {
    isOk = false;
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseMagicString(): cannot extract a version from format string \"" + str + "\"");
  };
  return isOk;
};



//
bool SgAgvChunk::parseEndString(const QString& str)
{
  bool                           isOk;
//CHUN.5 @chunk_size: 484843 records
//CHUN.1 @chunk_size: 11699 records
  QRegExp                       reClosingStr("CHUN\\.(\\d+)\\s+@chunk_size:\\s+(\\d+)\\s+records.*",  
                                  Qt::CaseInsensitive);
  //
  if (reClosingStr.indexIn(str) != -1)
  {
    isOk = true;
  }
  else
  {
    isOk = false;
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseEndString(): cannot recognize the ending string \"" + str + "\"");
  };
  return isOk;
};
/*=====================================================================================================*/


QString                   strNa("N/A");

template class SgAgvDatum<float>;
template class SgAgvDatum<double>;
template class SgAgvDatum<int>;
template class SgAgvDatum<short int>;
template class SgAgvDatum<long int>;

/*=====================================================================================================*/

/*=====================================================================================================*/
