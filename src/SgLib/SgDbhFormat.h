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

#ifndef SG_DBHFORMAT_H
#define SG_DBHFORMAT_H


#include <math.h>
#include <string.h>


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QList>
#include <QtCore/QHash>


#include <SgMathSupport.h>
#include <SgMJD.h>
#include <SgDbhImage.h>





/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgDbhPhysicalRecord
{
public:
  //
  // constructors/destructors:
  //
  inline SgDbhPhysicalRecord() {length_=-1; logicalRecord_=NULL; isOK_=true;};
  SgDbhPhysicalRecord(const SgDbhPhysicalRecord&);
  virtual ~SgDbhPhysicalRecord();
  //
  // Interfaces:
  //
  inline bool  isOk() const {return isOK_;};
  inline int   length() const {return length_;};
  //
  // Functions:
  //
  virtual int readLR(SgDbhStream& s);
  virtual int writeLR(SgDbhStream& s) const;
  virtual SgDbhPhysicalRecord& operator=(const SgDbhPhysicalRecord&);
  virtual void reSize(int length);
  virtual char* base() {return logicalRecord_;};
  static const QString className();
  //
  // Friends:
  //
  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhPhysicalRecord&);
  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhPhysicalRecord&);
protected:
  int               length_;
  char*             logicalRecord_;
  bool              isOK_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Data record: a string.
 * In fact, contains an interface for two types or records. The first one is from the start block,
 * there is just one string and there are no descriptors with dimensions. Here should be used 
 * getText()/setText() API. Other type of strings are multidimensional (they are stored in TE 
 * blocks), in this case getValue()/setValue() should be called.
 */
/**====================================================================================================*/
class SgDbhDataRecordString : public SgDbhPhysicalRecord
{
protected:
  QString       text_;
public:
  inline SgDbhDataRecordString() : SgDbhPhysicalRecord(), text_("Is there anybody out there?") {};
  inline SgDbhDataRecordString(const SgDbhDataRecordString& rec) 
    : SgDbhPhysicalRecord(rec), text_(rec.text_) {};
  virtual ~SgDbhDataRecordString();
  inline const QString& getText() const {return text_;};
  void setText(const QString& text);
  virtual QString getValue(SgDbhDatumDescriptor*, int dim2, int dim3);
  void setValue(SgDbhDatumDescriptor*, int dim2, int dim3, const QString&);
  virtual int readLR(SgDbhStream& s);
  using SgDbhPhysicalRecord::operator=;
  virtual SgDbhDataRecordString& operator=(const SgDbhDataRecordString& rec);
  static const QString className();
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Data record: a number.
 *
 */
/**====================================================================================================*/
template<class C> class SgDbhDataRecord : public SgDbhPhysicalRecord
{
protected:
  C*                base_;
  int               num_;
public:
  inline SgDbhDataRecord<C>() : SgDbhPhysicalRecord() {base_=NULL; num_=0;};
  inline SgDbhDataRecord<C>(SgDbhDataRecord<C> const & rec) : SgDbhPhysicalRecord(rec) 
    {base_=NULL; *this=rec;};
  virtual ~SgDbhDataRecord<C>();
  virtual C& operator[](int i);
  virtual C at(int i) const;
  virtual C value(SgDbhDatumDescriptor* descriptor, int d1, int d2, int d3); // zero based indexes
  virtual C& access(SgDbhDatumDescriptor* descriptor, int d1, int d2, int d3); // zero based indexes
  inline int num() const {return num_;};
  virtual char* base();
  virtual int readLR(SgDbhStream& s);
  virtual int writeLR(SgDbhStream& s) const;
  using SgDbhPhysicalRecord::operator=;
  virtual SgDbhDataRecord<C>& operator=(const SgDbhDataRecord<C>& rec);
  virtual void reSize(int length);
  virtual QString className() const;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * A record. The class is used as an abstract class for other service records.
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecord : public SgDbhPhysicalRecord
{
public:
  inline SgDbhServiceRecord(const char expectedPrefix[2]) : SgDbhPhysicalRecord()
    {prefix_[0]=prefix_[1]='Q'; expectedPrefix_[0]=expectedPrefix[0];
     expectedPrefix_[1]=expectedPrefix[1]; expectedPrefix_[2]=0;};
  inline SgDbhServiceRecord(const SgDbhServiceRecord& sr) : SgDbhPhysicalRecord() {*this=sr;};
  inline virtual ~SgDbhServiceRecord() {};
  inline bool isCorrectPrefix() const 
    {return prefix_[0]==expectedPrefix_[0] && prefix_[1]==expectedPrefix_[1];};
  inline virtual bool isAltered() const {return false;};
  bool isPrefixParsed(SgDbhStream&);
  inline void setPrefix(const char prefix[2]) {prefix_[0]=prefix[0]; prefix_[1]=prefix[1];};
  inline void setAltered() {prefix_[0]='Z'; prefix_[1]='Z';};
  using SgDbhPhysicalRecord::operator=;
  inline SgDbhServiceRecord& operator=(const SgDbhServiceRecord& sr)
    {SgDbhPhysicalRecord::operator=(sr); prefix_[0]=sr.prefix_[0]; prefix_[1]=sr.prefix_[1];
    expectedPrefix_[0]=sr.expectedPrefix_[0]; expectedPrefix_[1]=sr.expectedPrefix_[1];
    expectedPrefix_[2]=sr.expectedPrefix_[2]; return *this;};
  inline virtual int readLR(SgDbhStream& s) 
    {if (!isPrefixParsed(s)) return 2; else return 2;};
  inline virtual int writeLR(SgDbhStream& s) const 
    {s << (qint8)prefix_[0] << (qint8)prefix_[1]; return 2;};
  inline virtual QString className() const {return "SgDbhServiceRecord";};
protected:
  char          prefix_[2];
  char          expectedPrefix_[3];
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "HS1".
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecordHS1 : public SgDbhServiceRecord
{
public:
  SgDbhServiceRecordHS1() : SgDbhServiceRecord("HS")
    {lengthOfHistoryString_=epoch_[0]=epoch_[1]=epoch_[2]=epoch_[3]=epoch_[4]=versionNumber_=0;
    length_=24;};
  inline virtual ~SgDbhServiceRecordHS1() {};
//interface:
  int getLengthOfHistoryString() const {return lengthOfHistoryString_;};
  int getVersionNumber() const {return versionNumber_;};
  SgMJD getHistoryEpoch() const {return SgMJD(epoch_[0],1,epoch_[1],epoch_[2],epoch_[3],epoch_[4]);};
//
  void setLengthOfHistoryString(int length) {lengthOfHistoryString_=(short)length;};
  void setVersionNumber(int number) {versionNumber_=(short)number;};
  void setHistoryEpoch(const SgMJD &);
//
  bool isZ1() const {return prefix_[0]=='Z' && prefix_[1]=='Z';};
//  void setAltered() {prefix_[0]='Z'; prefix_[1]='Z';};
  virtual bool isAltered() const {return isZ1();};
  virtual int readLR(SgDbhStream& s);
  virtual int writeLR(SgDbhStream& s) const;
  inline virtual QString className() const {return "SgDbhServiceRecordHS1";};
  using SgDbhServiceRecord::operator=;

protected:
  short         lengthOfHistoryString_;
  short         epoch_[5];
  short         versionNumber_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "HS2".
 *
 */
/**====================================================================================================*/
class SgDbhHistoryEntry;
class SgDbhServiceRecordHS2 : public SgDbhServiceRecord
{
  friend class SgDbhHistoryEntry;
public:
  SgDbhServiceRecordHS2();
  inline virtual ~SgDbhServiceRecordHS2() {};
  virtual int readLR(SgDbhStream& s);
  virtual int writeLR(SgDbhStream& s) const;
  void setSemiName(const QString&);
  inline virtual QString className() const {return "SgDbhServiceRecordHS2";};
  using SgDbhServiceRecord::operator=;
protected:
  short         machineTypeNumber_;
  short         machineNumber_;
  short         installationNumber_;
  short         dbhVersionNumber_;
  char          semiName_[7];
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "TC".
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecordTc : public SgDbhServiceRecord
{
protected:
  short         tocType_, numTeBlocks_;
  bool          isZ2_;
  short         pRest[9];
public:
  SgDbhServiceRecordTc() : SgDbhServiceRecord("TC") {tocType_=numTeBlocks_=0; isZ2_=false;};
  inline virtual ~SgDbhServiceRecordTc() {};
  bool isZ2() const {return isZ2_;};
  short numTeBlocks() const {return numTeBlocks_;};
  short tocType() const {return tocType_;};
  virtual bool isAltered() const {return prefix_[0]=='Z' && prefix_[1]=='Z';};
  void dump(QTextStream& s) const;
  virtual int readLR(SgDbhStream&);
  virtual int writeLR(SgDbhStream& s) const;
  using SgDbhServiceRecord::operator=;
  SgDbhServiceRecordTc& operator=(const SgDbhServiceRecordTc& tc)
    {SgDbhServiceRecord::operator=(tc); tocType_=tc.tocType_; numTeBlocks_=tc.numTeBlocks_;
    isZ2_=tc.isZ2_; pRest[0]=tc.pRest[0]; pRest[1]=tc.pRest[1]; pRest[2]=tc.pRest[2];
    pRest[3]=tc.pRest[3]; pRest[4]=tc.pRest[4]; pRest[5]=tc.pRest[5]; pRest[6]=tc.pRest[6];
    pRest[7]=tc.pRest[7]; pRest[8]=tc.pRest[8]; return *this;};
  inline virtual QString className() const {return "SgDbhServiceRecordTc";};
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "TE".
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecordTe : public SgDbhServiceRecord
{
  friend class SgDbhTeBlock;
protected:
  short         teBlockNum_, numberOfDescriptors_;
  short         offsetI_, offsetA_, offsetD_, offsetJ_;
  short         r8Num_, i2Num_, a2Num_, d8Num_, j4Num_;
public:
  SgDbhServiceRecordTe();
  inline virtual ~SgDbhServiceRecordTe() {};
  int r8Num() const {return r8Num_;};
  int i2Num() const {return i2Num_;};
  int a2Num() const {return a2Num_;};
  int d8Num() const {return d8Num_;};
  int j4Num() const {return j4Num_;};
  void dump(QTextStream& s) const;
  virtual int readLR(SgDbhStream&);
  virtual int writeLR(SgDbhStream& s) const;
  using SgDbhServiceRecord::operator=;
  SgDbhServiceRecordTe& operator=(const SgDbhServiceRecordTe& te)
    {SgDbhServiceRecord::operator=(te); teBlockNum_=te.teBlockNum_; 
    numberOfDescriptors_=te.numberOfDescriptors_; offsetI_=te.offsetI_; offsetA_=te.offsetA_;
    offsetD_=te.offsetD_; offsetJ_=te.offsetJ_; r8Num_=te.r8Num_; i2Num_=te.i2Num_; 
    a2Num_=te.a2Num_; d8Num_=te.d8Num_; j4Num_=te.j4Num_; return *this;};
  inline virtual QString className() const {return "SgDbhServiceRecordTe";};
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "DR".
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecordDr : public SgDbhServiceRecord
{
protected:
  short         tcNo_;
  short         numOfTeBlocks_;
  short         pRest_[9];
public:
  SgDbhServiceRecordDr() : SgDbhServiceRecord("DR")
    {tcNo_=numOfTeBlocks_=0;
      pRest_[0]=pRest_[1]=pRest_[2]=pRest_[3]=pRest_[4]=pRest_[5]=pRest_[6]=pRest_[7]=pRest_[8]=0;};
  inline virtual ~SgDbhServiceRecordDr() {};
  int getTcNo() const {return tcNo_-1;};
  int getNumOfTeBlocks() const {return numOfTeBlocks_;};
  void setTcNo(int no) {tcNo_ = (short)(no+1);};
  void setNumOfTeBlocks(int num) {numOfTeBlocks_ = (short)num;};
  virtual int readLR(SgDbhStream& s);
  virtual int writeLR(SgDbhStream& s) const;
  void dump(QTextStream& s) const;
  inline virtual QString className() const {return "SgDbhServiceRecordDr";};
  using SgDbhServiceRecord::operator=;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * Service record "DE".
 *
 */
/**====================================================================================================*/
class SgDbhServiceRecordDe : public SgDbhServiceRecord
{
protected:
  short         teNo_;
  short         numOfR8_;
  short         numOfI2_;
  short         numOfA2_;
  short         numOfD8_;
  short         numOfJ4_;
  short         numOfPhRecs_;
  bool          isZ3_;
  short         pRest_[2];
public:
  SgDbhServiceRecordDe() : SgDbhServiceRecord("DE") 
    {teNo_=numOfR8_=numOfI2_=numOfA2_=numOfD8_=numOfJ4_=numOfPhRecs_=0; 
    pRest_[0]=pRest_[1]=0; isZ3_=false;};
  inline virtual ~SgDbhServiceRecordDe() {};
  int getTeNo() const {return teNo_-1;};
  int getNumOfR8() const {return numOfR8_;};
  int getNumOfI2() const {return numOfI2_;};
  int getNumOfA2() const {return numOfA2_;};
  int getNumOfD8() const {return numOfD8_;};
  int getNumOfJ4() const {return numOfJ4_;};
  int getNumOfPhRecs () const {return numOfPhRecs_;};
  void setTeNo(int no) {teNo_=(short)(no+1);};
  void setNumOfR8(int num) {numOfR8_=(short)num;};
  void setNumOfI2(int num) {numOfI2_=(short)num;};
  void setNumOfA2(int num) {numOfA2_=(short)num;};
  void setNumOfD8(int num) {numOfD8_=(short)num;};
  void setNumOfJ4(int num) {numOfJ4_=(short)num;};
  void setNumOfPhRecs(int num) {numOfPhRecs_=(short)num;};
  virtual bool isAltered() const {return prefix_[0]=='Z' && prefix_[1]=='Z';};
  bool isZ3() const {return isZ3_;};
  virtual int readLR(SgDbhStream&);
  virtual int writeLR(SgDbhStream& s) const;
  void dump(QTextStream& s) const;
  using SgDbhServiceRecord::operator=;
  virtual SgDbhServiceRecordDe& operator=(const SgDbhServiceRecordDe& de)
    {SgDbhServiceRecord::operator=(de); teNo_=de.teNo_; numOfR8_=de.numOfR8_; numOfI2_=de.numOfI2_;
    numOfA2_=de.numOfA2_; numOfD8_=de.numOfD8_; numOfJ4_=de.numOfJ4_; isZ3_=de.isZ3_;
    numOfPhRecs_=de.numOfPhRecs_; pRest_[0]=de.pRest_[0]; pRest_[1]=de.pRest_[1]; return *this;};
  inline virtual QString className() const {return "SgDbhServiceRecordDe";};
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * First Block.
 *
 */
/**====================================================================================================*/
class SgDbhStartBlock
{
protected:
  SgDbhDataRecordString       dbNameRec_;
  SgDbhDataRecordString       expDescriptRec_;
  SgDbhDataRecordString       sessIDRec_;
  SgDbhDataRecordString       prevDbRec_;
  SgDbhDataRecordString       prevRec_;
  SgDbhDataRecord<short>      epochRec_;
  SgDbhDataRecord<short>      versionRec_;
  bool                        isOK_;
  short                       epoch_[5];
public:
  SgDbhStartBlock();
  inline ~SgDbhStartBlock() {};
  bool isOk() const {return isOK_;};
  SgMJD epoch() const {return SgMJD(epoch_[0], 1, epoch_[1], epoch_[2], epoch_[3], epoch_[4]);};
  int version() const {return versionRec_.at(0);};
  const QString& dbName() const {return dbNameRec_.getText();};
  const QString& expDescript() const {return expDescriptRec_.getText();};
  const QString& sessionID() const {return sessIDRec_.getText();};
  const QString& prevDb() const {return prevDbRec_.getText();};
  const QString& prevDescript() const {return prevRec_.getText();};
  void rotateVersion(int newVersion, const QString& newFileName);
  void dump(QTextStream&) const;
  inline QString className() const {return "SgDbhStartBlock";};
  void alternateCode(const QString&);
  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhStartBlock&);
  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhStartBlock&);
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * One entry in the History.
 *
 */
/**====================================================================================================*/
class SgDbhHistoryEntry
{
protected:
  SgDbhServiceRecordHS1       record1_;
  SgDbhServiceRecordHS2       record2_;
  SgDbhDataRecordString       record3_;
  bool                        isOK_;
public:
  inline SgDbhHistoryEntry() : record1_(), record2_(), record3_() {isOK_=true;};
  inline ~SgDbhHistoryEntry() {};
  bool isOk() const {return isOK_;};
  bool isHistoryLine() const {return record1_.isCorrectPrefix();};
  bool isLast() const {return record1_.isZ1();};
  void dump(QTextStream& s) const;

  const QString& getText() {return record3_.getText();};
  int getVersion() {return record1_.getVersionNumber();};
  SgMJD getEpoch() const {return record1_.getHistoryEpoch();};

  void setText(const QString& text);
  void setVersion(int version) {record1_.setVersionNumber(version);};
  void setEpoch(const SgMJD &t) {record1_.setHistoryEpoch(t);};
  void unsetOkFlag() {isOK_=false;};
  void setEvent(const QString&, const QString&, int, const SgMJD&);
  inline QString className() const {return "SgDbhHistoryEntry";};

  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhHistoryEntry&);
  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhHistoryEntry&);
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* List of descriptors, one per a TE block:
* 
*======================================================================================================*/
class SgDbhListOfDescriptors
{
public:
  //
  // constructors/destructors:
  //
  inline SgDbhListOfDescriptors() : listOfDescriptors_() {};
  inline ~SgDbhListOfDescriptors()
    {while (!listOfDescriptors_.isEmpty()) delete listOfDescriptors_.takeFirst();};
  //
  // Interfaces:
  //
  QList<SgDbhDatumDescriptor*> *listOfDescriptors() {return &listOfDescriptors_;};
  //
  // Functions:
  //
  void dump(QTextStream&) const;
  SgDbhListOfDescriptors& operator=(const SgDbhListOfDescriptors&);
  inline QString className() const {return "SgDbhListOfDescriptors";};
  //
protected:
  QList<SgDbhDatumDescriptor*>      listOfDescriptors_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* A collection of data records, one per TE block:
* 
*======================================================================================================*/
class SgDbhDataBlock
{
public:
  //
  // constructors/destructors:
  //
  inline SgDbhDataBlock() : listOfRecords_(), recordByType_() {};
  ~SgDbhDataBlock();
  //
  // Interfaces:
  //
  inline QList<SgDbhPhysicalRecord*> *listOfRecords() {return &listOfRecords_;};
  inline QHash<int, SgDbhPhysicalRecord*> *recordByType() {return &recordByType_;};
  //
  // Functions:
  //
  SgDbhDataBlock& operator=(const SgDbhDataBlock&);
  inline QString className() const {return "SgDbhDataBlock";};
  //
protected:
  QList<SgDbhPhysicalRecord*>       listOfRecords_;
  QHash<int, SgDbhPhysicalRecord*>  recordByType_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* TE block of MK-III database format:
* 
*======================================================================================================*/
class SgDbhTeBlock : public SgDbhListOfDescriptors, public SgDbhDataBlock
{
  friend class SgDbhFormat;
public:
  //
  // constructors/destructors:
  //
  inline SgDbhTeBlock() : 
    SgDbhListOfDescriptors(), SgDbhDataBlock(), 
    recTe_(), recP3_(this), recP4_(this) 
    {isOK_=true; isFormatModified_=false;};
  inline ~SgDbhTeBlock() {};
  //
  // Interfaces:
  //
  bool isOk() const {return isOK_;};
  bool isFormatModified() const {return isFormatModified_;};
  void setIsFormatModified(bool Is) {isFormatModified_=Is;};
  const SgDbhServiceRecordTe& recTe() {return recTe_;};
  //
  // Functions:
  //
  int readRecordP3(SgDbhStream&);
  int readRecordP4(SgDbhStream&);
  int writeRecordP3(SgDbhStream&) const;
  int writeRecordP4(SgDbhStream&) const;
  void dump(QTextStream&) const;
  SgDbhTeBlock& operator=(const SgDbhTeBlock&);
  int calculateNumOfData(SgDbhDatumDescriptor::Type type) const;
  void adjustServiceRecords();
  inline QString className() const {return "SgDbhTeBlock";};
  int calcDataSize() const;
  //
  // Friends:
  //
  friend SgDbhStream &operator>>(SgDbhStream& s, SgDbhTeBlock& B);
  friend SgDbhStream &operator<<(SgDbhStream& s, const SgDbhTeBlock& B);
  //
protected:
  class SgDbhServiceRecordP3 : public SgDbhPhysicalRecord
  {
  public:
    SgDbhTeBlock        *owner_;
    inline SgDbhServiceRecordP3(SgDbhTeBlock* owner){owner_=owner;};
    inline virtual ~SgDbhServiceRecordP3() {owner_=NULL;};
    inline virtual int readLR(SgDbhStream& s){return owner_->readRecordP3(s);};
    inline virtual int writeLR(SgDbhStream& s) const {return owner_->writeRecordP3(s);};
    using SgDbhPhysicalRecord::operator=;
  };
  class SgDbhServiceRecordP4: public SgDbhPhysicalRecord
  {
  public:
    SgDbhTeBlock        *owner_;
    inline SgDbhServiceRecordP4(SgDbhTeBlock* owner){owner_=owner;};
    inline virtual ~SgDbhServiceRecordP4() {owner_=NULL;};
    inline virtual int readLR(SgDbhStream& s){return owner_->readRecordP4(s);};
    inline virtual int writeLR(SgDbhStream& s) const {return owner_->writeRecordP4(s);};
    using SgDbhPhysicalRecord::operator=;
  };
  SgDbhServiceRecordTe              recTe_;
  SgDbhServiceRecordP3              recP3_;
  SgDbhServiceRecordP4              recP4_;
  bool                              isOK_;
  bool                              isFormatModified_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* TC block of MK-III database format:
* 
*======================================================================================================*/
class SgDbhTcBlock
{
  friend class SgDbhFormat;
public:
  //
  // constructors/destructors:
  //
  inline SgDbhTcBlock() : recTc_(), listOfTeBlocks_(), descriptorByLCode_() {isOK_=true;};
  ~SgDbhTcBlock();
  //
  // Interfaces:
  //
  inline QList<SgDbhTeBlock*> *listOfTeBlocks() {return &listOfTeBlocks_;};
  inline QHash<QString, SgDbhDatumDescriptor*> *descriptorByLCode() {return &descriptorByLCode_;};
  inline const SgDbhServiceRecordTc& recTc() const {return recTc_;};
  inline bool isOk() const {return isOK_;};
  //
  // Functions:
  //
  inline bool isLast() const {return recTc_.isZ2();};
  inline short numTeBlocks() const {return recTc_.numTeBlocks();};
  inline short tocType() const {return recTc_.tocType();};
  void dump(QTextStream& s) const;
  SgDbhTcBlock& operator=(const SgDbhTcBlock&);
  inline QString className() const {return "SgDbhTcBlock";};
  //
  // Friends:
  //
  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhTcBlock&);
  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhTcBlock&);
  //
protected:
  bool                                  isOK_;
  SgDbhServiceRecordTc                  recTc_;
  QList<SgDbhTeBlock*>                  listOfTeBlocks_;
  QHash<QString, SgDbhDatumDescriptor*> descriptorByLCode_;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
* A list of data blocks that represent one observation (several TOCs):
* 
*======================================================================================================*/
class SgDbhObservationEntry
{
public:
  //
  // constructors/destructors:
  //
  inline SgDbhObservationEntry() : listOfTcsData_() {};
  ~SgDbhObservationEntry();
  //
  // Interfaces:
  //
  QList< QList<SgDbhDataBlock*>*>& listOfTcsData() {return listOfTcsData_;};
  QList<SgDbhDataBlock*>* dataBlocksFromTocI(int tocNumber) const 
    {return listOfTcsData_.at(tocNumber-1);};
  //
  // Functions:
  //
  void saveDataBlocksFromTcBlock(int TocNumber, SgDbhTcBlock& tcBlock);
  inline QString className() const {return "SgDbhObservationEntry";};
  //
protected:
  QList<QList<SgDbhDataBlock*>*>       listOfTcsData_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgDbhFormat
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  inline SgDbhFormat();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgDbhFormat();


  //
  // Interfaces:
  //
  inline QList<SgDbhTcBlock*> *listOfTcBlocks() {return &listOfTcBlocks_;};

  inline bool isOk() const {return isOK_;};

  inline short currentTcNumber() {return currentTcNumber_;};

  inline void setDumpStream(QTextStream* s) {dumpStream_ = s;};

  inline QTextStream* getDumpStream() {return dumpStream_;};


  //
  // Functions:
  //
  void postRead();
  
  void dump(QTextStream& s);

  void getBlock(SgDbhStream&);
  
  inline SgDbhTcBlock *currentTcBlock() 
    {return currentTcNumber_>-1?listOfTcBlocks_.at(currentTcNumber_):NULL;};

  SgDbhPhysicalRecord *properRecord(SgDbhDatumDescriptor*);

  SgDbhDatumDescriptor *lookupDescriptor(const char*);

  inline short i2(SgDbhDatumDescriptor *d, int i, int j, int k)
    {return d ? ((SgDbhDataRecord<short>*)properRecord(d))->value(d, i,j,k) : (short)0;};

  inline double r8(SgDbhDatumDescriptor *d, int i, int j, int k)
    {return d ? ((SgDbhDataRecord<double>*)properRecord(d))->value(d, i,j,k) : 0.0;};

  inline double d8(SgDbhDatumDescriptor *d, int i, int j, int k)
    {return d ? ((SgDbhDataRecord<double>*)properRecord(d))->value(d, i,j,k) : 0.0;};

  inline int j4(SgDbhDatumDescriptor *d, int i, int j, int k)
    {return d ? ((SgDbhDataRecord<int>*)properRecord(d))->value(d, i,j,k) : 0;};

  inline QString str(SgDbhDatumDescriptor *d, int i, int j)
    {return d ? ((SgDbhDataRecordString*)properRecord(d))->getValue(d, i,j) : QString("UNPARSED");};

  inline QString className() const {return "SgDbhFormat";};


  //
  // Friends:
  //
  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhFormat&);
  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhFormat&);


  //
  // I/O:
  //
  // ...

private:

protected:
  QList<SgDbhTcBlock*>            listOfTcBlocks_;
  QTextStream                     *dumpStream_;
  bool                            isOK_;
  short                           currentTcNumber_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgDbhFormat inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
inline SgDbhFormat::SgDbhFormat() :
  listOfTcBlocks_()
{
  currentTcNumber_ = -1;
  dumpStream_ = NULL;
  isOK_ = true;
};



//
inline SgDbhFormat::~SgDbhFormat()
{
  while (!listOfTcBlocks_.isEmpty())
    delete listOfTcBlocks_.takeFirst();
  dumpStream_ = NULL;
  isOK_ = true;
  currentTcNumber_ = -1; 
};



//
// FUNCTIONS:
//
//
inline void SgDbhFormat::dump(QTextStream& s)
{
  s << "== Format Block dump: ==\n";
  for(int i=0; i<listOfTcBlocks_.size(); i++)
    listOfTcBlocks_.at(i)->dump(s);
  s << "== End of dump ==\n";
};




//
// FRIENDS:
//
//
//

/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/
#endif //SG_DBHFORMAT_H
