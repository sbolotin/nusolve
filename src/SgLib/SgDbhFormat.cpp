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


#include <SgDbhFormat.h>
#include <SgLogger.h>







/***===================================================================================================*/
/**
 *   Physical record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgDbhPhysicalRecord::className()
{
  return "SgDbhPhysicalRecord";
}; 



//
SgDbhPhysicalRecord::SgDbhPhysicalRecord(const SgDbhPhysicalRecord& record)
{
  length_ = -1;
  logicalRecord_ = NULL;
  *this = record;
};



//
SgDbhPhysicalRecord::~SgDbhPhysicalRecord()
{
  if (logicalRecord_) 
    delete[] logicalRecord_; 
  logicalRecord_ = NULL;
};



//
SgDbhPhysicalRecord& SgDbhPhysicalRecord::operator=(const SgDbhPhysicalRecord& record)
{
  length_ = record.length();
  isOK_ = record.isOk();
  if (record.logicalRecord_)
  {
    if (logicalRecord_)
      delete[] logicalRecord_;
    logicalRecord_ = new char[length_+1];
    memcpy((void*)logicalRecord_, (void*)record.logicalRecord_, length_+1);
  }
  else 
    logicalRecord_ = NULL;
  return *this;
};



//
void SgDbhPhysicalRecord::reSize(int length)
{
  if (length%2)
    length++;
  length_ = length;
  if (logicalRecord_) // it was non-NULL, rearrange the area and clear it:
    delete []logicalRecord_;
  logicalRecord_ = new char[length_+1];
  memset((void*)logicalRecord_, 0, length_+1);
};



//
int SgDbhPhysicalRecord::readLR(SgDbhStream& s)
{
  if (logicalRecord_)
    delete []logicalRecord_;
  logicalRecord_ = new char[length_+1];
  int readBytes = s.readRawData(logicalRecord_, length_);
  *(logicalRecord_+length_) = 0;
  return readBytes; // could be =length_, <length_ or -1
};



//
int SgDbhPhysicalRecord::writeLR(SgDbhStream& s) const
{
  return logicalRecord_ ? s.writeRawData(logicalRecord_, length_) : 0;
};



//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhPhysicalRecord& record)
{
  int           hdd1, edd1, hd2, ed2;
  short         dd1;
  char          c;
  record.isOK_ = true;
  s >> (qint32 &)hdd1;
  if (hdd1!=2)
  {
    // complain:
    logger->write(SgLogger::WRN, SgLogger::IO_DBH,
      QString().sprintf("DBH input error: format violation: HDD1!=2 (HDD1=%d)", hdd1));
    record.isOK_ = false;
    return s;
  };
  s >> (qint16 &)dd1 >> (qint32 &)edd1;
  if (edd1!=hdd1)
  {
    // complain:
    logger->write(SgLogger::WRN, SgLogger::IO_DBH,
      QString().sprintf("DBH input error: format violation: EDD1!=HDD1 (EDD1=%d, HDD1=%d)", 
      edd1, hdd1));
    record.isOK_ = false;
    return s;
  };
  s >> (qint32 &)hd2;
  if (hd2!=dd1+dd1)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_DBH,
      QString().sprintf("DBH input error: format violation: HD2!=DD1+DD1 (HD2=%d, DD1=%d)", 
      hd2, dd1));
    record.isOK_ = false;
    return s;
  };
  record.length_ = hd2;
  int parsedBytes = record.readLR(s);
  if (parsedBytes>record.length_)
  {
    // complain:
    logger->write(SgLogger::ERR, SgLogger::IO_DBH,
      QString().sprintf("DBH input error: number of read bytes (%d) exceed record length (%d)", 
      parsedBytes, record.length_));
    record.isOK_ = false;
    return s;
  }
  else
    for (int j=parsedBytes; j<record.length_; j++) 
      s >> (qint8 &)c;
  
  s >> (qint32 &)ed2;
  if (ed2!=hd2)
  {
    // complain:
    logger->write(SgLogger::ERR, SgLogger::IO_DBH,
      QString().sprintf("DBH input error: format violation: ED2!=HD2 (ED2=%d, HD2=%d)", 
      ed2, hd2));
    record.isOK_ = false;
  };
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhPhysicalRecord& record)
{
  int           hdd1, edd1, hd2, ed2;
  short         dd1;
  char          c=0;

  hdd1 = edd1 = 2;
  hd2 = ed2 = record.length();
  dd1 = (short) record.length()/2;
  s << (const qint32 &)hdd1 << (const qint16 &)dd1 << (const qint32 &)edd1 << (const qint32 &)hd2;
  int writtenBytes = record.writeLR(s);
  if (writtenBytes<record.length())
    for (int j=writtenBytes; j<record.length(); j++) 
      s << (qint8)c;
  s << (qint32)ed2;
  return s;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Data record: a string.
 *
 */
/**====================================================================================================*/
//
QString const SgDbhDataRecordString::className()
{
  return "SgDbhDataRecordString";
};



//
SgDbhDataRecordString::~SgDbhDataRecordString()
{
};



//
QString SgDbhDataRecordString::getValue(SgDbhDatumDescriptor* descriptor, int dim2, int dim3)
{
  int               l = 2*descriptor->dim1() + 1; // space to copy array of chars + 0-terminator
  char              *c = new char[l];
  char              *p = logicalRecord_ + 2*(descriptor->offset() 
                        + descriptor->dim1()*dim2 + descriptor->dim1()*descriptor->dim2()*dim3);
  for (int i=0; i<l-1; i++) // copy actual data
    *(c+i) = *(p+i);
  *(c+l-1) = 0; // put a string terminator
  QString Ret = (const char*)c; // convert to QString
  delete []c;
  return Ret;
};



//
void SgDbhDataRecordString::setValue(SgDbhDatumDescriptor* descriptor, 
                                      int dim2, int dim3, const QString& str)
{
  int               l = 2*descriptor->dim1();
  char              *p= logicalRecord_ + 2*(descriptor->offset() 
                        + descriptor->dim1()*dim2 + descriptor->dim1()*descriptor->dim2()*dim3);
  int               i, n2copy = std::min(l,str.length());
  for (i=0; i<n2copy; i++)
    *(p+i) = str.at(i).toLatin1();
  for (; i<l; i++)
    *(p+i) = ' '; // fill the rest of the fields with "space" chars.
};



//
void SgDbhDataRecordString::setText(const QString& text) 
{
  // save in the cashe,
  text_ = text;
  if (length_<0) // this is just created record, arrange space to store data:
    reSize(text_.length());
  // and copy it to the logical record:
  char              *p = logicalRecord_;
  int               i, n2copy = std::min(length(), text_.length());
  for (i=0; i<n2copy; i++)
    *(p+i) = text_.at(i).toLatin1();
  for (; i<length(); i++)
    *(p+i) = ' '; // fill the rest of the fields with "space" chars.
};



//
int SgDbhDataRecordString::readLR(SgDbhStream& s)
{
  int i=SgDbhPhysicalRecord::readLR(s); 
  if(isOK_) 
    text_= (const char*)logicalRecord_; 
  return i;
};



//
SgDbhDataRecordString& SgDbhDataRecordString::operator=(const SgDbhDataRecordString& rec)
{
  SgDbhPhysicalRecord::operator=(rec);
  text_ = rec.getText();
  return *this;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Data record: a number.
 *
 */
/**====================================================================================================*/
/*
template<class C> SgDbhDataRecord<C>::SgDbhDataRecord(SgDbhDataRecord<C> const & rec)
  : SgDbhPhysicalRecord(rec)
{
  num_ = rec.num_;
  if (base_) 
    delete []base_;
  base_ = new C[num_];
  memcpy((void*)base_, (void*)rec.base_, sizeof(C)*num_);
};
*/
template<class C> SgDbhDataRecord<C>::~SgDbhDataRecord<C>() 
{
  if (base_) 
    delete []base_;
  base_=NULL;
};



//
template<class C> C& SgDbhDataRecord<C>::operator[](int i) 
{
  return *(base_+i);
};



//
template<class C> C SgDbhDataRecord<C>::at(int i) const 
{
  return base_&&-1<i&&i<num_ ? *(base_+i) : (C)0;
};



//
template<class C> C SgDbhDataRecord<C>::value(SgDbhDatumDescriptor* d, int dim1, int dim2, int dim3)
{
  // make additional checks for each index:
  return (dim1<d->dim1() && dim2<d->dim2() && dim3<d->dim3() && dim1>=0 && dim2>=0 && dim3>=0) ?
    operator[](d->offset() + dim1 + d->dim1()*dim2 + d->dim1()*d->dim2()*dim3) : (C)0;
};



//
template<class C> char* SgDbhDataRecord<C>::base() 
{
  return (char*)base_;
};



//
template<class C> SgDbhDataRecord<C>& SgDbhDataRecord<C>::operator=(const SgDbhDataRecord<C>& rec)
{
  SgDbhPhysicalRecord::operator=(rec); 
  if (base_) 
    delete []base_; 
  num_ = rec.num_; 
  base_ = new C[num_]; 
  memcpy((void*)base_, (void*)rec.base_, sizeof(C)*num_); 
  return *this;
};



//
template<class C> C& SgDbhDataRecord<C>::access(SgDbhDatumDescriptor* d, int i, int j, int k)
{
#ifdef DEBUG
  // make additional checks for each index:
  if (i<0 || i>=d->dim1())
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() 
      + QString().sprintf(": access: the first index <%d> out of range [0:%d]", i, d->dim1()));
  if (j<0 || j>=d->dim2())
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() 
      + QString().sprintf(": access: the second index <%d> out of range [0:%d]", j, d->dim2()));
  if (k<0 || k>=d->dim3())
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() 
      + QString().sprintf(": access: the third index <%d> out of range [0:%d]", k, d->dim3()));
#endif
  return operator[](d->offset() + i + d->dim1()*j + d->dim1()*d->dim2()*k);
};



//
template<class C> int SgDbhDataRecord<C>::readLR(SgDbhStream& s)
{
  if (base_) 
    delete []base_;
  base_ = new C[(num_=length_/sizeof(C))];
  C *p = base_;
  for(int i=0; i<num_; i++,p++) 
    s >> *p;
  return length_;
};



//
template<class C> int SgDbhDataRecord<C>::writeLR(SgDbhStream& s) const
{
  C *p = base_;
  for(int i=0; i<num_; i++,p++) 
    s << *p;
  return length_;
};



//
template<class C> void SgDbhDataRecord<C>::reSize(int length)
{
  length_ = length;
  if (base_)
    delete []base_;
  base_ = new C[(num_=length_/sizeof(C))];
  memset((void*)base_, 0, num_*sizeof(C));
};


template<class C> QString SgDbhDataRecord<C>::className() const 
{
  return "SgDbhDataRecord<C>";
};

/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * A record. The class is used as an abstract class for other service records.
 *
 */
/**====================================================================================================*/
//
bool SgDbhServiceRecord::isPrefixParsed(SgDbhStream& s)
{
  s >> (qint8 &)prefix_[0] >> (qint8 &)prefix_[1];
  if (!isCorrectPrefix() && !isAltered())
    return (isOK_=false);
  else
    return true;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "HS1".
 *
 */
/**====================================================================================================*/
//
void SgDbhServiceRecordHS1::setHistoryEpoch(const SgMJD & t)
{
  int               nYear, nMonth, nDay, nHour, nMin;
  double            dSec;
  SgMJD::MJD_reverse(t.getDate(), t.getTime(),  nYear, nMonth, nDay, nHour, nMin, dSec);

  epoch_[0] = (short)nYear;
  epoch_[1] = (short)t.calcDayOfYear();
  epoch_[2] = (short)nHour;
  epoch_[3] = (short)nMin;
  epoch_[4] = (short)round(dSec);
};



//
int SgDbhServiceRecordHS1::readLR(SgDbhStream& s)
{
  short             p8, p9, p10, p11;
  lengthOfHistoryString_ = 0;
  epoch_[0] = epoch_[1] = epoch_[2] = epoch_[3] = epoch_[4] = 0;
  versionNumber_ = 0;

  if (isPrefixParsed(s) && !isAltered())
  {
    s >> (qint16 &)lengthOfHistoryString_
      >> (qint16 &)epoch_[0] >> (qint16 &)epoch_[1] >> (qint16 &)epoch_[2] 
      >> (qint16 &)epoch_[3] >> (qint16 &)epoch_[4] 
      >> (qint16 &)versionNumber_ 
      >> (qint16 &)p8 >> (qint16 &)p9 >> (qint16 &)p10 >> (qint16 &)p11; // unused fields 
    return 24;
  };
  return 2;
};



//
int SgDbhServiceRecordHS1::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
  if (!isAltered())
  {
    s << (qint16)lengthOfHistoryString_
      << (qint16)epoch_[0] << (qint16)epoch_[1] << (qint16)epoch_[2] 
      << (qint16)epoch_[3] << (qint16)epoch_[4] 
      << (qint16)versionNumber_ 
      << (qint16)0 << (qint16)0 << (qint16)0 << (qint16)0; // unused fields 
    writtenBytes+=22;
  };
  return writtenBytes;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "HS2".
 *
 */
/**====================================================================================================*/
//
SgDbhServiceRecordHS2::SgDbhServiceRecordHS2() : SgDbhServiceRecord("HS")
{
  // mimic current DBH files:
  machineTypeNumber_ = 1;
  machineNumber_ = 1;
  installationNumber_ = 1;
  dbhVersionNumber_ = 1996;
  semiName_[0] = 'U';
  semiName_[1] = 'N';
  semiName_[2] = 'D';
  semiName_[3] = 'E';
  semiName_[4] = 'F';
  semiName_[5] = '.';
  semiName_[6] =   0;
  length_     =  24;
};



//
void SgDbhServiceRecordHS2::setSemiName(const QString& semiName)
{
  for (int i=0; i<6; i++)
    semiName_[i] = semiName.at(i).toLatin1();
};



//
int SgDbhServiceRecordHS2::readLR(SgDbhStream& s)
{
  short             p5, p6, p7, p8;
  machineTypeNumber_ = machineNumber_ = installationNumber_ = dbhVersionNumber_ = 0;
  semiName_[0] = 'U';
  semiName_[1] = 'N';
  semiName_[2] = 'D';
  semiName_[3] = 'E';
  semiName_[4] = 'F';
  semiName_[5] = '.';
  semiName_[6] =   0;
  if (!isPrefixParsed(s)) 
    return 2; 
  else 
    s >> (qint16 &)machineTypeNumber_ >> (qint16 &)machineNumber_ >> (qint16 &)installationNumber_
      >> (qint16 &)dbhVersionNumber_ 
      >> (qint8 &)semiName_[0] >> (qint8 &)semiName_[1] >> (qint8 &)semiName_[2] 
      >> (qint8 &)semiName_[3] >> (qint8 &)semiName_[4] >> (qint8 &)semiName_[5]
      >> (qint16 &)p5 >> (qint16 &)p6 >> (qint16 &)p7 >> (qint16 &)p8; // unused fields 
  return 24;
};



//
int SgDbhServiceRecordHS2::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
  s << (qint16)machineTypeNumber_ << (qint16)machineNumber_ << (qint16)installationNumber_
    << (qint16)dbhVersionNumber_ 
    << (qint8)semiName_[0] << (qint8)semiName_[1] << (qint8)semiName_[2] 
    << (qint8)semiName_[3] << (qint8)semiName_[4] << (qint8)semiName_[5]
    << (qint16)0 << (qint16)0 << (qint16)0 << (qint16)0; // unused fields 
  writtenBytes += 22;
  return writtenBytes;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "TC".
 *
 */
/**====================================================================================================*/
//
void SgDbhServiceRecordTc::dump(QTextStream& s) const
{
  s << "TC record: TOC Type: " << tocType_
    << "; number of TE blocks: " << numTeBlocks_ 
    << ", rest part: [" << pRest[0] << ":" << pRest[1] << ":" << pRest[2] 
    << ":" << pRest[3] << ":" << pRest[4] << ":" << pRest[5] << ":" << pRest[6] 
    << ":" << pRest[7] << ":" << pRest[8] << "]" 
    << endl;
};



//
int SgDbhServiceRecordTc::readLR(SgDbhStream& s)
{
  tocType_ = numTeBlocks_ = 0;
  isZ2_ = false;
  if (isPrefixParsed(s) && !isAltered())
  {
    s >> (qint16 &)tocType_ >> (qint16 &)numTeBlocks_
      >> (qint16 &)pRest[0] >> (qint16 &)pRest[1] >> (qint16 &)pRest[2] >> (qint16 &)pRest[3]
      >> (qint16 &)pRest[4] >> (qint16 &)pRest[5] >> (qint16 &)pRest[6] >> (qint16 &)pRest[7]
      >> (qint16 &)pRest[8]
    ;
//    return 6;
    return 24;
  }
  else if (isAltered())
  {
    unsigned char c[2];
    s >> (qint8 &)c[0] >> (qint8 &)c[1];
    if (c[0]==(unsigned char)7 && c[1]==(unsigned char)208)
    {
      isOK_ = true;
      isZ2_ = true;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() + 
        QString().sprintf(": format violation: c[0]!=7 || c[1]!=208 (%d:%d)", c[0], c[1]));
    return 4;
  };
  return 2;
};



//
int SgDbhServiceRecordTc::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
  if (!isAltered())
  {
    s << (const qint16 &)tocType_ << (const qint16 &)numTeBlocks_
      << (const qint16 &)pRest[0] << (const qint16 &)pRest[1] << (const qint16 &)pRest[2] 
      << (const qint16 &)pRest[3] << (const qint16 &)pRest[4] << (const qint16 &)pRest[5] 
      << (const qint16 &)pRest[6] << (const qint16 &)pRest[7] << (const qint16 &)pRest[8];
    writtenBytes += 22;
  }
  else
  {
    s << (const qint8 &)7 << (const qint8 &)208;
    writtenBytes += 2;
  };
  return writtenBytes;
};
/*=====================================================================================================*/



/***===================================================================================================*/
/**
 * Service record "TE".
 *
 */
/**====================================================================================================*/
//
SgDbhServiceRecordTe::SgDbhServiceRecordTe() 
  : SgDbhServiceRecord("TE")
{
  teBlockNum_ = numberOfDescriptors_ = 0;
  offsetI_ = offsetA_ = offsetD_ = offsetJ_ = 0;
  r8Num_ = i2Num_ = a2Num_ = d8Num_ = j4Num_ = 0;
};



//
void SgDbhServiceRecordTe::dump(QTextStream& s) const
{
  s << "TE record: TE block number: " << teBlockNum_ << "; number of descriptors: " 
    << numberOfDescriptors_ << endl
    << "Offsets: I2:" << offsetI_ << " A2:" << offsetA_ << " D8:" << offsetD_ << " J4:" << offsetJ_
    << " (bytes)" << endl 
    << "Numbers: I2:" << i2Num_ << " A2:" << a2Num_ << " D8:" << d8Num_ << " J4:" << j4Num_
    << " R8:" << r8Num_ << endl;
};



//
int SgDbhServiceRecordTe::readLR(SgDbhStream& s)
{
  teBlockNum_ = numberOfDescriptors_ = 0;
  offsetI_ = offsetA_ = offsetD_ = offsetJ_ = 0;
  r8Num_ = i2Num_ = a2Num_ = d8Num_ = j4Num_ = 0;
  if (!isPrefixParsed(s)) 
    return 2; 
  else
    s >> (qint16 &)teBlockNum_ >> (qint16 &)offsetI_ >> (qint16 &)offsetA_ >> (qint16 &)offsetD_ 
      >> (qint16 &)r8Num_ >> (qint16 &)i2Num_ >> (qint16 &)a2Num_ >> (qint16 &)offsetJ_ 
      >> (qint16 &)numberOfDescriptors_ >> (qint16 &)d8Num_ >> (qint16 &)j4Num_;
  return 24;
};



//
int SgDbhServiceRecordTe::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
  s << (const qint16 &)teBlockNum_ << (const qint16 &)offsetI_ << (const qint16 &)offsetA_ 
    << (const qint16 &)offsetD_ << (const qint16 &)r8Num_ << (const qint16 &)i2Num_ 
    << (const qint16 &)a2Num_ << (const qint16 &)offsetJ_ << (const qint16 &)numberOfDescriptors_ 
    << (const qint16 &)d8Num_ << (const qint16 &)j4Num_;
  writtenBytes += 22;
  return writtenBytes;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "DR".
 *
 */
/**====================================================================================================*/
//
int SgDbhServiceRecordDr::readLR(SgDbhStream& s)
{
  tcNo_ = numOfTeBlocks_ = 0;
  if (!isPrefixParsed(s)) // reads two bytes
    return 2; 
  else 
  {
    s >> (qint16 &)tcNo_ >> (qint16 &)numOfTeBlocks_
      >> (qint16 &)pRest_[0] >> (qint16 &)pRest_[1] >> (qint16 &)pRest_[2] >> (qint16 &)pRest_[3]
      >> (qint16 &)pRest_[4] >> (qint16 &)pRest_[5] >> (qint16 &)pRest_[6] >> (qint16 &)pRest_[7]
      >> (qint16 &)pRest_[8]
      ;
  };
//  return 6;
  return 24; //+ prefix
};



//
int SgDbhServiceRecordDr::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
    s << (const qint16 &)tcNo_ << (const qint16 &)numOfTeBlocks_
      << (const qint16 &)pRest_[0] << (const qint16 &)pRest_[1] << (const qint16 &)pRest_[2] 
      << (const qint16 &)pRest_[3] << (const qint16 &)pRest_[4] << (const qint16 &)pRest_[5] 
      << (const qint16 &)pRest_[6] << (const qint16 &)pRest_[7] << (const qint16 &)pRest_[8];
  writtenBytes += 22;
  return writtenBytes;
};



//
void SgDbhServiceRecordDr::dump(QTextStream& s) const
{
  s << "DR record (length=" << length() << "): TC#: " << tcNo_ << "; NumOf TE Blocks: " 
    << numOfTeBlocks_ << "; Prefix: ["  << prefix_[0] << prefix_[1] << "]"
    << ", rest part: [" << pRest_[0] << ":" << pRest_[1] << ":" 
    << pRest_[2] << ":" << pRest_[3] << ":" << pRest_[4] << ":" << pRest_[5] << ":" 
    << pRest_[6] << ":" << pRest_[7] << ":" << pRest_[8] << "]" 
    << endl;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "DE".
 *
 */
/**====================================================================================================*/
//
int SgDbhServiceRecordDe::readLR(SgDbhStream& s)
{
  short p5=0, p6=0, p7=0; 
  teNo_ = numOfR8_ = numOfI2_ = numOfA2_ = numOfD8_ = numOfJ4_ = numOfPhRecs_ = 0;
  isZ3_ = false;
  if (isPrefixParsed(s) && !isAltered())
  {
    s >> (qint16 &)teNo_ >> (qint16 &)numOfR8_ >> (qint16 &)numOfI2_ >> (qint16 &)numOfA2_
      >> (qint16 &)p5 >> (qint16 &)p6 >> (qint16 &)p7
      >> (qint16 &)numOfD8_ >> (qint16 &)numOfJ4_ >> (qint16 &)pRest_[0] >> (qint16 &)pRest_[1];
    if (p5!=0 || p6!=0 || p7!=0)
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
        QString().sprintf(": surprise, non-null unused data (%d:%d:%d)", p5, p6, p7));
    return 24;
//    return 16;
  }
  else if (isAltered())
  {
    short z3p1;
    s >> (qint16 &)z3p1;
    isOK_ = true;
    isZ3_ = true;
    numOfPhRecs_ = z3p1;
    pRest_[0] = pRest_[1] = 0;
    return 4;
  };
  return 2;
};



//
int SgDbhServiceRecordDe::writeLR(SgDbhStream& s) const
{
  int writtenBytes = SgDbhServiceRecord::writeLR(s); // write prefix
  if (!isAltered())
  {
    s << (const qint16 &)teNo_ << (const qint16 &)numOfR8_ 
      << (const qint16 &)numOfI2_ << (const qint16 &)numOfA2_ 
      << (const qint16 &)0 << (const qint16 &)0 << (const qint16 &)0
      << (const qint16 &)numOfD8_ << (const qint16 &)numOfJ4_ 
      << (const qint16 &)pRest_[0] << (const qint16 &)pRest_[1];
    writtenBytes += 22;
  }
  else
  {
    s << (const qint16 &)numOfPhRecs_;
    writtenBytes += 2;
  };
  return writtenBytes;
};



//
void SgDbhServiceRecordDe::dump(QTextStream& s) const 
{
  s << "DE record (length=" << length() << ")";
  if (!isZ3())
    s << ": TE#: " << teNo_
      << "; NumOf R8:" << numOfR8_ << "; NumOf I2:" << numOfI2_ << "; NumOf A2:" << numOfA2_ 
      << "; NumOf D8:" << numOfD8_ << "; NumOf J4:" << numOfJ4_;
  s << "; Prefix: [" << prefix_[0] << prefix_[1] << "]";
  if (isZ3())
    s << "; this is a Z3 record, number of phys.records above: " << numOfPhRecs_ << endl;
  else
    s <<  ", rest part: [" << pRest_[0] << ":" << pRest_[1] << "]" << endl;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * First Block.
 *
 */
/**====================================================================================================*/
//
SgDbhStartBlock::SgDbhStartBlock() :
  dbNameRec_(),
  expDescriptRec_(),
  sessIDRec_(),
  prevDbRec_(),
  prevRec_(),
  epochRec_(),
  versionRec_()
{
  isOK_=false; 
  epoch_[0]=epoch_[1]=epoch_[2]=epoch_[3]=epoch_[4]=0;
};



//
void SgDbhStartBlock::dump(QTextStream& s) const
{
  s << "== Start Block dump: ==" << endl 
    << "Database name         : " << dbNameRec_.getText() << endl
    << "Date of creation      : " << epoch().toString(SgMJD::F_Verbose) 
        << " (" << epoch_[0] << ":" << epoch_[1] << ":" << epoch_[2] << ":" 
        << epoch_[3] << ":" << epoch_[4] << ")" << endl
    << "Experiment description: " << expDescript().simplified() << endl
    << "Session ID            : " << sessionID() << endl
    << "Version               : " << version() << endl
    << "Previous Database name: " << prevDb() << endl
    << "Previous description  : " << prevDescript() << endl
    << "== End of dump ==" << endl;
};



//
void SgDbhStartBlock::rotateVersion(int newVersion, const QString& newFileName)
{
  versionRec_[0] = (short)newVersion;
  
  QString str = dbNameRec_.getText();
  prevDbRec_.setText(str);
  dbNameRec_.setText(newFileName);
  prevRec_.reSize(36);
  prevRec_.setText("$" + str.left(9) + str.right(2) + "DBH880930 GSFC GNU/Linux");
};



//
void SgDbhStartBlock::alternateCode(const QString& code)
{
  if (!code.size())
    return;

  QString                     str;
  QChar                       c=code.at(0);
  // name:
  str = dbNameRec_.getText();
  str.replace(8, 1, c);
  dbNameRec_.setText(str);
  // ID:
  str = sessIDRec_.getText();
  str.replace(9, 1, c);
  sessIDRec_.setText(str);
  // Prev name:
  str = prevDbRec_.getText();
  str.replace(8, 1, c);
  prevDbRec_.setText(str);
  // Prev descr:
  str = prevRec_.getText();
  str.replace(9, 1, c);
  prevRec_.setText(str);
};



//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhStartBlock& block)
{
  s >> block.dbNameRec_ >> block.epochRec_ >> block.expDescriptRec_ 
    >> block.sessIDRec_ >> block.versionRec_ >> block.prevDbRec_ >> block.prevRec_;
  if ((block.isOK_ = block.dbNameRec_.isOk() && block.epochRec_.isOk() 
      && block.expDescriptRec_.isOk() && block.sessIDRec_.isOk() && block.versionRec_.isOk() 
      && block.prevDbRec_.isOk() && block.prevRec_.isOk()))
    {
      block.epoch_[0] = block.epochRec_[0];
      block.epoch_[1] = block.epochRec_[1];
      block.epoch_[2] = block.epochRec_[2];
      block.epoch_[3] = block.epochRec_[3];
      block.epoch_[4] = block.epochRec_[4];
    };
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhStartBlock& block)
{
  SgDbhDataRecord<short>  currentEpochRecord=block.epochRec_;
  SgMJD                   t=SgMJD::currentMJD();
  int                     nYear, nMonth, nDay, nHour, nMin;
  double                  dSec;
  SgMJD::MJD_reverse(t.getDate(), t.getTime(),  nYear, nMonth, nDay, nHour, nMin, dSec);
  
  currentEpochRecord[0] = (short)nYear;
  currentEpochRecord[1] = (short)t.calcDayOfYear();
  currentEpochRecord[2] = (short)nHour;
  currentEpochRecord[3] = (short)nMin;
  currentEpochRecord[4] = (short)round(dSec);
  
  s << block.dbNameRec_ << currentEpochRecord << block.expDescriptRec_ 
    << block.sessIDRec_ << block.versionRec_ << block.prevDbRec_ << block.prevRec_;
  return s;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * One entry in the History.
 *
 */
/**====================================================================================================*/
//
void SgDbhHistoryEntry::dump(QTextStream& s) const
{
  s << record1_.getHistoryEpoch().toString() << " (" << record1_.getVersionNumber() << "): \""
    << record3_.getText() << "\" [" 
    << record2_.machineTypeNumber_  << ":" << record2_.machineNumber_  << ":"
    << record2_.installationNumber_  << ":"
    << record2_.dbhVersionNumber_  << "] SN=(" << record2_.semiName_ << ")"
    << endl;
};



//
void SgDbhHistoryEntry::setText(const QString& text)
{
  record3_.setText(text);
  record1_.setLengthOfHistoryString(record3_.length()/2);
};



//
void SgDbhHistoryEntry::setEvent(const QString& text, const QString& sessionID, 
  int version, const SgMJD& t)
{
  setVersion(version);
  setEpoch(t);
  setText(text);
  record1_.setPrefix("HS");
  record2_.setPrefix("HS");
  record2_.setSemiName(sessionID);
};



//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhHistoryEntry& entry)
{
  s >> entry.record1_;
  if (entry.record1_.isCorrectPrefix())
  {
    s >> entry.record2_ >> entry.record3_;
    if (2*entry.record1_.getLengthOfHistoryString()!=entry.record3_.length())  // complain:
      logger->write(SgLogger::ERR, SgLogger::IO_DBH,
        QString().sprintf("SgDbhStream: corrupted DBH file: wrong history string length for "
        "[%s]: %d, expected: %d", qPrintable(entry.record3_.getText()), 
        2*entry.record1_.getLengthOfHistoryString(), entry.record3_.length()));
  };
  entry.isOK_ = entry.record1_.isOk() && entry.record2_.isOk() && entry.record3_.isOk();
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhHistoryEntry& entry)
{
  if (entry.isOk())
    s << entry.record1_ << entry.record2_ << entry.record3_;
  else
  {
    SgDbhServiceRecordHS1       rec1;
    rec1.setAltered();
    s << rec1;
  };
  return s;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* List of descriptors, one per a Te block:
* 
*======================================================================================================*/
//
void SgDbhListOfDescriptors::dump(QTextStream& s) const
{
  const char    *sTypes[] = {"R8", "I2", "A2", "D8", "J4", "UN"};
  QString       str;
  s << "List of descriptors (=format) dump (Offsets are in sizeof(Par)):" << endl
    << "---- --------  --- --- ---  --- ---- -- --------------------------------" << endl
    << " Num Abbrev.    D1  D2  D3  Ver Offs Tp Description" << endl
    << "---- --------  --- --- ---  --- ---- -- --------------------------------" << endl;
  for (int i=0; i<listOfDescriptors_.size(); i++)
    {
      SgDbhDatumDescriptor* descriptor = listOfDescriptors_.at(i);
      str.sprintf("%3d. %8s [%3d,%3d,%3d] %3d %4d %2s %32s", 
                  i, qPrintable(descriptor->getLCode()),        
                  descriptor->dim1(), descriptor->dim2(), descriptor->dim3(),
                  descriptor->getModifiedAtVersion(), descriptor->offset(), 
                  sTypes[descriptor->type()], qPrintable(descriptor->getDescription()));
      s << str << endl;
    };
  s << "------------------------------------------------------------------------" << endl;
};



//
SgDbhListOfDescriptors& SgDbhListOfDescriptors::operator=(const SgDbhListOfDescriptors& dList)
{
  // clear the list:
  while (!listOfDescriptors_.isEmpty())
    delete listOfDescriptors_.takeFirst();

  // refill the list of descriptors (deep copy):
  for (int i=0; i<dList.listOfDescriptors_.size(); i++)
  {
    SgDbhDatumDescriptor *descriptor = new SgDbhDatumDescriptor;
    *descriptor = *dList.listOfDescriptors_.at(i);
    listOfDescriptors_.append(descriptor);
  };
  return *this;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* A collection of data records, one per TE block:
* 
*======================================================================================================*/
//
SgDbhDataBlock::~SgDbhDataBlock()
{
  recordByType_.clear();
  for (int i=0; i<listOfRecords_.size(); i++)
    delete listOfRecords_.at(i);
  listOfRecords_.clear();
};



//
SgDbhDataBlock& SgDbhDataBlock::operator=(const SgDbhDataBlock& block)
{
  // clear the list and the hash:
  recordByType_.clear();
  while (!listOfRecords_.isEmpty())
    delete listOfRecords_.takeFirst();

  // refill the list of records (deep copy) and the hash:
  QHash<int, SgDbhPhysicalRecord*>::const_iterator j;
  for (j=block.recordByType_.constBegin(); j!=block.recordByType_.constEnd(); ++j)
  {
    SgDbhPhysicalRecord *rec_in = j.value(), *record;
    switch (j.key())
    {
    case SgDbhDatumDescriptor::T_R8:
      record = new SgDbhDataRecord<double>;
      *(SgDbhDataRecord<double>*)record = *(SgDbhDataRecord<double>*)rec_in;
      listOfRecords_.append(record);
      recordByType_.insert(SgDbhDatumDescriptor::T_R8, record);
      break;
    case SgDbhDatumDescriptor::T_I2:
      record = new SgDbhDataRecord<short>;
      *(SgDbhDataRecord<short>*)record = *(SgDbhDataRecord<short>*)rec_in;
      listOfRecords_.append(record);
      recordByType_.insert(SgDbhDatumDescriptor::T_I2, record);
      break;
    case SgDbhDatumDescriptor::T_A2:
      record = new SgDbhDataRecordString;
      *(SgDbhDataRecordString*)record = *(SgDbhDataRecordString*)rec_in;
      listOfRecords_.append(record);
      recordByType_.insert(SgDbhDatumDescriptor::T_A2, record);
      break;
    case SgDbhDatumDescriptor::T_D8:
      record = new SgDbhDataRecord<double>;
      *(SgDbhDataRecord<double>*)record = *(SgDbhDataRecord<double>*)rec_in;
      listOfRecords_.append(record);
      recordByType_.insert(SgDbhDatumDescriptor::T_D8, record);
      break;
    case SgDbhDatumDescriptor::T_J4:
      record = new SgDbhDataRecord<int>;
      *(SgDbhDataRecord<int>*)record = *(SgDbhDataRecord<int>*)rec_in;
      listOfRecords_.append(record);
      recordByType_.insert(SgDbhDatumDescriptor::T_J4, record);
      break;
    case SgDbhDatumDescriptor::T_UNKN:
    default:
      // complain:
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +        
        ": SgDbhStream: operator=(): Wrong type of data record");
      break;
    };
  };
  return *this;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* TE block of MK-III database format:
* 
*======================================================================================================*/
//
SgDbhTeBlock& SgDbhTeBlock::operator=(const SgDbhTeBlock& teBlock)
{
  // copy the members:
  recTe_ = teBlock.recTe_;
  recP3_ = teBlock.recP3_;
  recP4_ = teBlock.recP4_;
  isOK_ = teBlock.isOK_;
  *(SgDbhListOfDescriptors*)this = teBlock;
  *(SgDbhDataBlock*)this = teBlock;
  return *this;
};



//
int SgDbhTeBlock::readRecordP3(SgDbhStream& s)
{
  char tmps[9];
  for (int i=0; i<recTe_.numberOfDescriptors_; i++)
  {
    SgDbhDatumDescriptor *descriptor = new SgDbhDatumDescriptor;
    s.readRawData(tmps, 8);
    *(tmps + 8) = 0;
    descriptor->lCode_ = (const char*)tmps;
    s >> (qint16 &)descriptor->modifiedAtVersion_ 
      >> (qint16 &)descriptor->dim1_ 
      >> (qint16 &)descriptor->dim2_ 
      >> (qint16 &)descriptor->dim3_;
    listOfDescriptors_.append(descriptor);
  };
  return 16*recTe_.numberOfDescriptors_;
};



//
int SgDbhTeBlock::readRecordP4(SgDbhStream& s)
{
  char tmps[33];
  for (int i=0; i<recTe_.numberOfDescriptors_; i++)
  {
    s.readRawData(tmps, 32); 
    *(tmps + 32) = 0;
    listOfDescriptors_.at(i)->description_ = (const char*)tmps;
  };
  return 32*recTe_.numberOfDescriptors_;
};



//
int SgDbhTeBlock::writeRecordP3(SgDbhStream& s) const
{
  char tmps[8];
  int num = recTe_.numberOfDescriptors_;
  for (int dIdx=0; dIdx<num; dIdx++)
  {
    SgDbhDatumDescriptor *descriptor = listOfDescriptors_.at(dIdx);
    for (int i=0; i<8; i++)
      tmps[i] = descriptor->lCode_.at(i).toLatin1();
    s.writeRawData(tmps, 8);
    s << (const qint16 &)descriptor->modifiedAtVersion_ 
      << (const qint16 &)descriptor->dim1_ 
      << (const qint16 &)descriptor->dim2_ 
      << (const qint16 &)descriptor->dim3_;
  };
  return 16*num;
};



//
int SgDbhTeBlock::writeRecordP4(SgDbhStream& s) const
{
  char tmps[32];
  int num = recTe_.numberOfDescriptors_;
  for (int dIdx=0; dIdx<num; dIdx++)
  {
    SgDbhDatumDescriptor *descriptor = listOfDescriptors_.at(dIdx);
    int n2write = std::min(descriptor->description_.size(), 32), i;
    for (i=0; i<n2write; i++)
      tmps[i] = descriptor->description_.at(i).toLatin1();
    for (; i<32; i++)
      tmps[i] = 0;
    s.writeRawData(tmps, 32);
  };
  return 32*num;
};



//
void SgDbhTeBlock::dump(QTextStream& s) const
{
  s << "== TE[" << recTe_.teBlockNum_ << "] Block dump: ==" << endl;
  s << "Block length: " << calcDataSize() << " (bytes)" << endl;
  recTe_.dump(s);
  SgDbhListOfDescriptors::dump(s);
  s  << "== End of TE Block dump" << endl;
};



//
int SgDbhTeBlock::calculateNumOfData(SgDbhDatumDescriptor::Type type) const
{
  int numOfData=0;
  QList<SgDbhDatumDescriptor*>::const_iterator i;
  for (i=listOfDescriptors_.constBegin(); i!=listOfDescriptors_.constEnd(); ++i)
  {
    SgDbhDatumDescriptor *descriptor=*i;
    if (descriptor->type()==type)
      numOfData += descriptor->dim1()*descriptor->dim2()*descriptor->dim3();
  };
  return numOfData;
};



//
void SgDbhTeBlock::adjustServiceRecords()
{
  int num = listOfDescriptors_.size();
  recP3_.reSize(16*num);
  recP4_.reSize(32*num);
  recTe_.numberOfDescriptors_ = (short)num;
  QList<SgDbhDatumDescriptor*>::const_iterator it;
  int i=0;
  for (it=listOfDescriptors_.constBegin(); it!=listOfDescriptors_.constEnd(); ++it, i++)
  {
    SgDbhDatumDescriptor *descriptor = *it;
    if (descriptor->getLCode()=="I-FILLER")
      recTe_.offsetI_ = (short)i;
    else if (descriptor->getLCode()=="A-FILLER")
      recTe_.offsetA_ = (short)i;
    else if (descriptor->getLCode()=="D-FILLER")
      recTe_.offsetD_ = (short)i;
    else if (descriptor->getLCode()=="J-FILLER")
      recTe_.offsetJ_ = (short)i;
  };
  recTe_.r8Num_ = calculateNumOfData(SgDbhDatumDescriptor::T_R8);
  recTe_.i2Num_ = calculateNumOfData(SgDbhDatumDescriptor::T_I2);
  recTe_.a2Num_ = calculateNumOfData(SgDbhDatumDescriptor::T_A2);
  recTe_.d8Num_ = calculateNumOfData(SgDbhDatumDescriptor::T_D8);
  recTe_.j4Num_ = calculateNumOfData(SgDbhDatumDescriptor::T_J4);
};



//
int SgDbhTeBlock::calcDataSize() const
{
  int                           dataSize=0;
  int                           sizeOfType=0;
  for (int i=0; i<listOfDescriptors_.size(); i++)
  {
    SgDbhDatumDescriptor *descriptor=listOfDescriptors_.at(i);
    switch (descriptor->type())
    {
      case SgDbhDatumDescriptor::T_R8:
        sizeOfType = sizeof(double);
      break;
      case SgDbhDatumDescriptor::T_I2:
        sizeOfType = sizeof(short);
      break;
      case SgDbhDatumDescriptor::T_A2:
        sizeOfType = 2*sizeof(char);
      break;
      case SgDbhDatumDescriptor::T_D8:
        sizeOfType = sizeof(double);
      break;
      case SgDbhDatumDescriptor::T_J4:
        sizeOfType = sizeof(int);
      break;
      default:
      case SgDbhDatumDescriptor::T_UNKN:
        sizeOfType = 0;
      break;
    };
    dataSize += descriptor->dim1()*descriptor->dim2()*descriptor->dim3()*sizeOfType;
  };
  return dataSize;
};



//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhTeBlock& B)
{
  s >> B.recTe_ >> B.recP3_ >> B.recP4_;
  B.isOK_ = B.isOK_ && B.recTe_.isOk() && B.recP3_.isOk() && B.recP4_.isOk();
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhTeBlock& B)
{
  s << B.recTe_ << B.recP3_ << B.recP4_;
  return s;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* TC block of MK-III database format:
* 
*======================================================================================================*/
//
SgDbhTcBlock::~SgDbhTcBlock()
{
  while (!listOfTeBlocks_.isEmpty()) 
    delete listOfTeBlocks_.takeFirst();
  descriptorByLCode_.clear();
};



//
void SgDbhTcBlock::dump(QTextStream& s) const
{
  s << "== TC[" << tocType() << "] Block dump: ==" << endl;
  recTc_.dump(s);
  for (int i=0; i<listOfTeBlocks_.size(); i++) 
    listOfTeBlocks_.at(i)->dump(s);
  s << "==" << endl;
};



//
SgDbhTcBlock& SgDbhTcBlock::operator=(const SgDbhTcBlock& tcBlock)
{
  // copy the members:
  isOK_ = tcBlock.isOK_;
  recTc_= tcBlock.recTc_;
  
  // clear the lists and the hash:
  while (!listOfTeBlocks_.isEmpty()) 
    delete listOfTeBlocks_.takeFirst();
  descriptorByLCode_.clear();
  
  for (int teCount=0; teCount<tcBlock.listOfTeBlocks_.size(); teCount++)
  {
    SgDbhTeBlock *teBlock = new SgDbhTeBlock; 
    *teBlock = *tcBlock.listOfTeBlocks_.at(teCount);
    listOfTeBlocks_.append(teBlock);

    for (int i=0; i<teBlock->listOfDescriptors()->size(); i++)
    {
      SgDbhDatumDescriptor* descriptor=teBlock->listOfDescriptors()->at(i);
      if (descriptor->getLCode()!="R-FILLER" && 
          descriptor->getLCode()!="I-FILLER" &&
          descriptor->getLCode()!="A-FILLER" &&
          descriptor->getLCode()!="D-FILLER" &&
          descriptor->getLCode()!="J-FILLER")
        descriptorByLCode_.insert(descriptor->getLCode(), descriptor);
    };
  };
  return *this;
};



//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhTcBlock& B)
{
  s >> B.recTc_;
  while (!B.listOfTeBlocks_.isEmpty()) 
    delete B.listOfTeBlocks_.takeFirst();
  if (!B.isLast() && (B.isOK_=B.recTc_.isOk()))
    for (int i=0; i<B.recTc_.numTeBlocks(); i++)
    {
      SgDbhTeBlock *teBlock = new SgDbhTeBlock; 
      s >> *teBlock;
      if ((B.isOK_ = B.isOK_ && teBlock->isOk())) 
        B.listOfTeBlocks_.append(teBlock);
      else 
        delete teBlock;
    };
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhTcBlock& B)
{
  s << B.recTc_;
  for (int i=0; i<B.listOfTeBlocks_.size(); i++)
    s << *B.listOfTeBlocks_.at(i);
  return s;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* A list of data blocks that represent one observation (several TOCs):
* 
*======================================================================================================*/
//
SgDbhObservationEntry::~SgDbhObservationEntry()
{  
  // clear data from all TOCs for current observation entry:

  while (!listOfTcsData_.isEmpty())
  {
    QList<SgDbhDataBlock*>     *listOfDataBlocks=listOfTcsData_.first();
    while (!listOfDataBlocks->isEmpty())
      delete listOfDataBlocks->takeFirst();
    listOfTcsData_.removeFirst();
    delete listOfDataBlocks;
  };
};



//
void SgDbhObservationEntry::saveDataBlocksFromTcBlock(int tocNumber, SgDbhTcBlock& tcBlock)
{
  tocNumber--; // adjust for TOC General
  if (listOfTcsData_.size()!=tocNumber)  // should not happen, complain:
    logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
      ": listOfTcsData_.size()>tocNumber (need to replace QList with QHash)");

  QList<SgDbhDataBlock*> *listOfDataBlocks = new QList<SgDbhDataBlock*>;
  listOfTcsData_.append(listOfDataBlocks);

  for (int teCount=0; teCount<tcBlock.listOfTeBlocks()->size(); teCount++)
  {
    SgDbhDataBlock *dataBlock = new SgDbhDataBlock; 
    *dataBlock = *tcBlock.listOfTeBlocks()->at(teCount);
    listOfDataBlocks->append(dataBlock);
  };
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
SgDbhPhysicalRecord* SgDbhFormat::properRecord(SgDbhDatumDescriptor* descriptor)
{
  // check:
  if (!isOk())
  {
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      ": properRecord(): called while the format is not ready");
    return NULL;
  }
  if (!descriptor)
  {
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      ": properRecord(): argument is NULL");
    return NULL;
  };
  if (descriptor->nTc()!=currentTcNumber())
  {
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      QString().sprintf(": properRecord(): the descriptor [%s] aka \"%s\", is from other TOC, %d;"
      " current TOC is %d", qPrintable(descriptor->getLCode()), 
      qPrintable(descriptor->getDescription()), descriptor->nTc(), currentTcNumber()));
    return NULL;
  };
  SgDbhTcBlock      *tcBlock = currentTcBlock();
  if (!tcBlock)
  {
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      QString().sprintf(": properRecord(): current TC block is NULL, TC# %d", currentTcNumber()));
    return NULL;
  };
  SgDbhTeBlock *teBlock = tcBlock->listOfTeBlocks()->at(descriptor->nTe());
  return teBlock ?  (
                      teBlock->recordByType()->contains(descriptor->type()) ? 
                      teBlock->recordByType()->value(descriptor->type()) : NULL
                    ) : NULL;
};



//
void SgDbhFormat::postRead()
{
  int                         offset;
  short                       tcCount, teCount;
  SgDbhDatumDescriptor::Type  type = SgDbhDatumDescriptor::T_UNKN;

  for (tcCount=0; tcCount<listOfTcBlocks_.size(); tcCount++)
  {
    SgDbhTcBlock *tcBlock = listOfTcBlocks_.at(tcCount);
    tcBlock->descriptorByLCode_.clear();

    for (teCount=0; teCount<tcBlock->listOfTeBlocks()->size(); teCount++)
    {
      SgDbhTeBlock* teBlock = tcBlock->listOfTeBlocks()->at(teCount);
      SgDbhPhysicalRecord *record;
      offset = 0;
      teBlock->recordByType_.clear();
      while (!teBlock->listOfRecords()->isEmpty()) 
        delete teBlock->listOfRecords()->takeFirst();
      for (int i=0; i<teBlock->listOfDescriptors()->size(); i++)
      {
        SgDbhDatumDescriptor* descriptor=teBlock->listOfDescriptors()->at(i);
        if (descriptor->getLCode()=="R-FILLER")
        {
          offset=0;
          teBlock->listOfRecords()->append((record = new SgDbhDataRecord<double>));
          teBlock->recordByType()->insert((type=SgDbhDatumDescriptor::T_R8), record);
        }
        else if (descriptor->getLCode()=="I-FILLER")
        {
          offset=0;
          teBlock->listOfRecords()->append((record = new SgDbhDataRecord<short>));
          teBlock->recordByType()->insert((type=SgDbhDatumDescriptor::T_I2), record);
        }
        else if (descriptor->getLCode()=="A-FILLER")
        {
          offset=0;
          teBlock->listOfRecords()->append((record = new SgDbhDataRecordString));
          teBlock->recordByType()->insert((type=SgDbhDatumDescriptor::T_A2), record);
        }
        else if (descriptor->getLCode()=="D-FILLER")
        {
          offset=0;
          teBlock->listOfRecords()->append((record = new SgDbhDataRecord<double>));
          teBlock->recordByType()->insert((type=SgDbhDatumDescriptor::T_D8), record);
        }
        else if (descriptor->getLCode()=="J-FILLER")
        {
          offset=0;
          teBlock->listOfRecords()->append((record = new SgDbhDataRecord<int>));
          teBlock->recordByType()->insert((type=SgDbhDatumDescriptor::T_J4), record);
        }
        else if (!tcBlock->descriptorByLCode()->contains(descriptor->getLCode()))
          tcBlock->descriptorByLCode()->insert(descriptor->getLCode(), descriptor);
        else 
          logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
            QString().sprintf(": postRead(): descriptor [%s] already in the hash", 
            qPrintable(descriptor->getLCode())));
        descriptor->setOffset(offset);
        descriptor->setType(type);
        descriptor->setNTc(tcCount);
        descriptor->setNTe(teCount);
        offset += descriptor->dim1()*descriptor->dim2()*descriptor->dim3();
      };
      switch (teBlock->listOfRecords()->count())
      {
      case 0:
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
          ": postRead(): the order of records is not parsed correctly");
        break;
      case 1 ... 4:
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
          ": postRead(): some data records are missed");
        break;
      case 5:
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
          ": postRead(): the order of records is parsed correctly");
        break;
      default:
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
          ": postRead(): too many records");
      };
    };
  };
};



//
SgDbhDatumDescriptor *SgDbhFormat::lookupDescriptor(const char* tag)
{
  return currentTcBlock()->descriptorByLCode()->contains(tag) ? 
    currentTcBlock()->descriptorByLCode()->value(tag) : NULL;
};



//
void SgDbhFormat::getBlock(SgDbhStream& s)
{
  SgDbhServiceRecordDr    DR;
  SgDbhServiceRecordDe    DE;
  SgDbhPhysicalRecord     *record = NULL;
  SgDbhTcBlock            *tcBlock = NULL;
  SgDbhTeBlock            *teBlock = NULL;

  s >> DR;
  if (dumpStream_)
  {
    *dumpStream_ << "== Data Block dump: ==" << endl;
    DR.dump(*dumpStream_);
  };

  if (!(isOK_ = isOK_ && DR.isOk())) 
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      ": getBlock: error reading DR record");
  else
    tcBlock = listOfTcBlocks_.at((currentTcNumber_=(short)(DR.getTcNo()))); // is it right????????????
  
  while (!DE.isZ3() && isOK_)
  {
    s >> DE;
    if (!(isOK_ = isOK_ && DE.isOk()))
      logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
        ": getBlock: error reading DE record");
    if (dumpStream_ && isOK_) 
      DE.dump(*dumpStream_);
    if (DE.isCorrectPrefix() && isOK_)
    {
      teBlock = tcBlock->listOfTeBlocks()->at(DE.getTeNo());
      for (int i=0; i<teBlock->listOfRecords()->size() && isOK_; i++)
      {
        record = teBlock->listOfRecords()->at(i);
        s >> *record;
        isOK_ = isOK_ && record->isOk();
      };
    };
  };
  
  if (!isOK_)
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
      ": getBlock: error reading data record");

  // make dump:
  if (dumpStream_ && isOK_)
  {
    *dumpStream_ << "== TC[" << currentTcNumber_ << "] Data Block dump: ==" << endl;
    for (int i=0; i<tcBlock->listOfTeBlocks()->size(); i++)
    {
      teBlock = tcBlock->listOfTeBlocks()->at(i);
      *dumpStream_ << "== TE[" << i << "] Data Block dump: ==" << endl;
      *dumpStream_ << "The sizes of data records are:" << endl;
      for (int j=0; j<teBlock->listOfRecords()->size(); j++)
        *dumpStream_ << "  " << j << ": " << teBlock->listOfRecords()->at(j)->length() << endl;
      *dumpStream_ << "--" << endl;
      teBlock->recTe_.dump(*dumpStream_);
      for (int j=0; j<teBlock->listOfDescriptors()->size(); j++)
      {
        SgDbhDatumDescriptor* descriptor = teBlock->listOfDescriptors()->at(j);
        if ((record = teBlock->recordByType()->value(descriptor->type())))
        {
          *dumpStream_<< "\"" << descriptor->getLCode() << "\" (aka \"" 
                      << descriptor->getDescription().simplified() << "\") [" 
                      << descriptor->dim1() << "," << descriptor->dim2() << "," 
                      << descriptor->dim3() << "] =" << endl << "(" << endl;
          for (int li=0; li<descriptor->dim3(); li++)
          {
            *dumpStream_ << "   (";
            for (int lj=0; lj<descriptor->dim2(); lj++)
            {
              if (descriptor->type()==SgDbhDatumDescriptor::T_A2)
                *dumpStream_  << "\"" 
                              << ((SgDbhDataRecordString*)record)->getValue(descriptor, lj, li)
                              << "\"";
              else
              {
                *dumpStream_ << "(";
                for (int lk=0; lk<descriptor->dim1(); lk++)
                {
                  switch(descriptor->type())
                  {
                  case SgDbhDatumDescriptor::T_R8:
                    *dumpStream_ << ((SgDbhDataRecord<double>*)record)->value(descriptor, lk, lj, li);
                    break;
                  case SgDbhDatumDescriptor::T_I2:
                    *dumpStream_ << ((SgDbhDataRecord<short>*)record)->value(descriptor, lk, lj, li);
                    break;
                  case SgDbhDatumDescriptor::T_D8:
                    *dumpStream_ << ((SgDbhDataRecord<double>*)record)->value(descriptor, lk, lj, li);
                    break;
                  case SgDbhDatumDescriptor::T_J4:
                    *dumpStream_ << ((SgDbhDataRecord<int>*)record)->value(descriptor, lk, lj, li);
                    break;
                  default: *dumpStream_ << "????";
                  };
                  *dumpStream_ << (lk<descriptor->dim1()-1?", ":"");
                };
                *dumpStream_ << ")";
              };
              *dumpStream_ << (lj<descriptor->dim2()-1?", ":"");
            };
            *dumpStream_ << ")" << (li<descriptor->dim3()-1 ? "," : "") << endl;
          };
          *dumpStream_ << ");" << endl;
        }
        else
          logger->write(SgLogger::INF, SgLogger::IO_DBH, className() + 
            QString().sprintf(": getBlock: can't find record for [%s] descriptor", 
            qPrintable(descriptor->getLCode())));
      };
    };
    *dumpStream_ << endl;
  };
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//
SgDbhStream &operator>>(SgDbhStream& s, SgDbhFormat& F)
{
  SgDbhTcBlock  *tcBlock;
  do
  {
    tcBlock = new SgDbhTcBlock;
    s >> *tcBlock;
    if (!tcBlock->isLast() && (F.isOK_ = F.isOK_ && tcBlock->isOk())) 
      F.listOfTcBlocks_.append(tcBlock);
    else 
      delete tcBlock;
  } while (tcBlock && !tcBlock->isLast() && F.isOK_);
  F.postRead();
  return s;
};



//
SgDbhStream &operator<<(SgDbhStream& s, const SgDbhFormat& F)
{
  for (int i=0; i<F.listOfTcBlocks_.size(); i++)
    s << *F.listOfTcBlocks_.at(i);
  SgDbhServiceRecordTc tcRec = F.listOfTcBlocks_.at(0)->recTc();
  tcRec.setAltered();
  return s << tcRec;
};
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
