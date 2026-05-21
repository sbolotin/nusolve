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

#ifndef SG_IO_AGV
#define SG_IO_AGV


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include <iostream>
#include <stdlib.h>

#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QTextStream>
#include <QtCore/QVector>


#include <SgConstants.h>
#include <SgIoDriver.h>
#include <SgLogger.h>
#include <SgModelsInfo.h>
#include <SgVlbiHistory.h>

//class SgAgvChapter;
class SgAgvChunk;
class SgAgvDriver;
class SgAgvSection;
class SgVlbiSession;

//
enum AgvDataScope
{
  ADS_NONE                = 0, //!< undefinite
  ADS_SESSION             = 1, //!< session scope
  ADS_SCAN                = 2, //!< scan scope
  ADS_STATION             = 3, //!< station scope
  ADS_BASELINE            = 4, //!< baseline scope
};

//
enum AgvDataType
{
  ADT_NONE                = 0, //!< undefinite
  ADT_CHAR                = 1, //!< session scope
  ADT_I2                  = 2, //!< scan scope
  ADT_I4                  = 3, //!< station scope
  ADT_I8                  = 4, //!< baseline scope
  ADT_R4                  = 5, //!< baseline scope
  ADT_R8                  = 6, //!< baseline scope
};

//
enum AgvContentStyle
{
  ACS_NONE                =     0, //!< undefinited
  ACS_NATIVE              = 1<< 0, //!< 
  ACS_GVH_DBH             = 1<< 1, //!< 
  ACS_GVH_VDB             = 1<< 2, //!< 
  ACS_GVH_PIMA            = 1<< 3, //!< 
  ACS_GVH                 = ACS_GVH_DBH | ACS_GVH_VDB | ACS_GVH_PIMA,
  ACS_ANY                 = ACS_NATIVE | ACS_GVH,
};


//
/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvDatumDescriptor
{
public:
  inline SgAgvDatumDescriptor() : lCode_(""), description_("")
    {dataScope_=ADS_NONE; dataType_=ADT_NONE; dim1_=dim2_=dim3_=dim4_=0; driver_=NULL; 
      hasData_=false; isUsable_=true; hasMutableScope_=false; expectance_=ACS_ANY;};

  SgAgvDatumDescriptor(const QString& lCode, const QString descr, AgvDataScope scp, AgvDataType typ,
    int d1, int d2, unsigned int exp, bool hasMutableScope=false);

  SgAgvDatumDescriptor(const SgAgvDatumDescriptor& add);

  inline virtual ~SgAgvDatumDescriptor() {};
  inline QString className() const {return "SgAgvDatumDescriptor";};
  
  inline bool  getHasData() const {return hasData_;};
  inline bool  getIsUsable() const {return isUsable_;};
  inline bool  getHasMutableScope() const {return hasMutableScope_;};

  inline const QString& getLCode() const {return lCode_;};
  inline const QString& getDescription() const {return description_;};

  inline AgvDataScope getDataScope() const {return dataScope_;};
  inline AgvDataType  getDataType() const {return dataType_;};
  
  inline int getDim1() const {return dim1_;};
  inline int getDim2() const {return dim2_;};
  inline int getDim3() const {return dim3_;};
  inline int getDim4() const {return dim4_;};

  inline const SgAgvDriver* getDriver() const {return driver_;};
  inline unsigned int getExpectance() const {return expectance_;};


  inline void setLCode(const QString& lCode) {lCode_=lCode;};
  inline void setDescription(const QString& description) {description_=description;};

  inline void setDataScope(AgvDataScope scope) {dataScope_=scope;};
  inline void setDataType(AgvDataType typ) {dataType_=typ;};

  inline void setDim1(int d) {dim1_=d;};
  inline void setDim2(int d) {dim2_=d;};
  inline void setDim3(int d) {dim3_=d;};
  inline void setDim4(int d) {dim4_=d;};
  
  inline void setHasData(bool h) {hasData_=h;};
  inline void setIsUsable(bool is) {isUsable_=is;};
  inline void setHasMutableScope(bool has) {hasMutableScope_=has;};

  inline void setDriver(SgAgvDriver* drv) {driver_=drv;};
  inline void setExpectance(unsigned int expect) {expectance_ = expect;};
  
  //
  inline int d1() const {return effectiveSize(dim1_);};
  inline int d2() const {return effectiveSize(dim2_);};
  inline int d3() const {return effectiveSize(dim3_);};
  inline int d4() const {return effectiveSize(dim4_);};
  inline int totalMaxSize() const {return (dataType_==ADT_CHAR?1:d1())*d2()*d3()*d4();};
  int totalSize() const;

  static QString dataType2str(AgvDataType);
  static QString dataScope2str(AgvDataScope);
  
  inline bool isExpected(AgvContentStyle contentStyle) const {return expectance_ & contentStyle;};
  
protected:
  QString                       lCode_;
  QString                       description_;
  AgvDataScope                  dataScope_;
  AgvDataType                    dataType_;
  int                           dim1_;
  int                           dim2_;
  int                           dim3_;
  int                           dim4_;
  bool                          hasMutableScope_;
  SgAgvDriver                   *driver_;
  bool                          hasData_;
  bool                          isUsable_; // for reading
  unsigned int                  expectance_;
  //
  int effectiveSize(int d) const;
};
/*=====================================================================================================*/





//
/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
template<class C> class SgAgvDatum
{
public:
  inline SgAgvDatum() {descriptor_=NULL; data_=NULL;};

  inline SgAgvDatum(SgAgvDatumDescriptor* descr) {descriptor_=descr; data_=NULL;};

  inline ~SgAgvDatum() {freeSpace(); descriptor_=NULL;};
  inline QString className() const {return "SgAgvDatum";};
  inline bool isAllocated() const {return data_ != NULL;};
  
  C getValue(int idx1, int idx2, int idx3, int idx4) const;

  C& value(int idx1, int idx2, int idx3, int idx4);

  void allocateSpace();
  
  void freeSpace();
  
  bool isEmpty();
  
protected:
  SgAgvDatumDescriptor         *descriptor_;
  C                            *data_;
};
/*=====================================================================================================*/




//
/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvDatumString
{
public:
  inline SgAgvDatumString() {descriptor_=NULL; data_=NULL;};

  SgAgvDatumString(SgAgvDatumDescriptor* descr) {descriptor_=descr; data_=NULL;};

  inline ~SgAgvDatumString() {freeSpace();};
  inline QString className() const {return "SgAgvDatumString";};
  inline bool isAllocated() const {return data_ != NULL;};
  
  const QString& getValue(int idx2, int idx3, int idx4) const;
  
  inline SgAgvDatumDescriptor* descriptor() {return descriptor_;};

  QString& value(int idx2, int idx3, int idx4);

  void allocateSpace();
  
  void freeSpace();

  bool isEmpty();
  
protected:
  SgAgvDatumDescriptor         *descriptor_;
  QString                      *data_;
};
/*=====================================================================================================*/





//
/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvRecord
{
public:
  inline SgAgvRecord() : content_("") {owner_=NULL;};
  inline SgAgvRecord(SgAgvSection *owner, const QString& content, int si=0)
    : content_(content), lCode_(content.left(8)) {owner_=owner; subIdx_=si;};
  inline ~SgAgvRecord() {};
  inline QString className() const {return "SgAgvRecord";};

  inline const QString& content() const {return content_;};
  inline const QString& lCode() const {return lCode_;};
  inline int getSubIdx() const {return subIdx_;};

  int exportData(QTextStream& ts, const QString& prefix);
  
protected:
  QString                        content_;
  QString                        lCode_;
  SgAgvSection                 *owner_;
  int                            subIdx_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvSection : public QList<SgAgvRecord*>
{
public:
  inline SgAgvSection() : startEpoch_(tZero), prefix_("") {idx_=0;};
  inline SgAgvSection(const QString prefix, int idx) : prefix_(prefix) {idx_=idx;};
  inline ~SgAgvSection() {for (int i=0; i<size(); i++) delete at(i); clear();};
  inline QString className() const {return "SgAgvSection";};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* /*drv*/)
    {ts << prefix_ << "." << idx_ + 1 << " @section_length: 0 records\n";  return 1;};
  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:
  SgMJD                         startEpoch_;
  QString                       prefix_;
  int                            idx_;
  bool parseSectionLengthString(const QString& str, int& num2read);
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvFileSection : public SgAgvSection
{
public:
  inline SgAgvFileSection() {prefix_="FILE";};
  inline SgAgvFileSection(int idx) : SgAgvSection("FILE", idx) {};
  inline ~SgAgvFileSection() {};
  inline QString className() const {return "SgAgvFileSection";};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* drv);
//  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:

};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvPreaSection : public SgAgvSection
{
public:
  inline SgAgvPreaSection() {prefix_="PREA";};
  inline SgAgvPreaSection(int idx) : SgAgvSection("PREA", idx) {};
  inline ~SgAgvPreaSection() {};
  inline QString className() const {return "SgAgvPreaSection";};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* drv);
  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvTextSection : public SgAgvSection
{
public:
  inline SgAgvTextSection() : headerByIdx_(), numByIdx_(), maxLenByIdx_(), history_(), histEpoch_(tZero)
    {prefix_="TEXT"; histVer_=-1;};
  inline SgAgvTextSection(int idx) : SgAgvSection("TEXT", idx), headerByIdx_(),
    numByIdx_(), maxLenByIdx_() {histVer_=idx+1;};
  inline ~SgAgvTextSection() {headerByIdx_.clear(); numByIdx_.clear(); maxLenByIdx_.clear();};
  inline QString className() const {return "SgAgvTextSection";};

  inline SgVlbiHistory& history() {return history_;};
  inline const SgVlbiHistory& history() const {return history_;};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* drv);
  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:
  QMap<int, QString>            headerByIdx_;
  QMap<int, int>                numByIdx_;
  QMap<int, int>                maxLenByIdx_;
  SgVlbiHistory                 history_;
  SgMJD                         histEpoch_;
  int                           histVer_;
  
  bool parseChapterInitString(const QString& str, int& num2read);
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvTocsSection : public SgAgvSection
{
public:
  inline SgAgvTocsSection() {prefix_="TOCS";};
  inline SgAgvTocsSection(int idx) : SgAgvSection("TOCS", idx) {};
  inline ~SgAgvTocsSection() {;};
  inline QString className() const {return "SgAgvTocsSection";};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* drv);
  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:
  SgAgvRecord* datumDescriptor2agvRecord(SgAgvDatumDescriptor* dd);
  SgAgvDatumDescriptor* agvRecord2datumDescriptor(SgAgvRecord* rec);
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvDataSection : public SgAgvSection
{
public:
  inline SgAgvDataSection() {prefix_="DATA";};
  inline SgAgvDataSection(int idx) : SgAgvSection("DATA", idx) {};
  inline ~SgAgvDataSection() {;};
  inline QString className() const {return "SgAgvDataSection";};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  int exportData(QTextStream& ts, SgAgvDriver* drv);
  int importData(QTextStream& ts, SgAgvDriver* drv);
  
protected:
  int writeDatum(QTextStream& ts, SgAgvDatumDescriptor* dd, const QString& prfx, SgAgvDriver* drv);
  int writeDatumOpt(QTextStream& ts, SgAgvDatumDescriptor* dd, const QString& prfx, SgAgvDriver* drv);
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgAgvChunk
{
public:
  inline SgAgvChunk() : sMagicString_(magicPrefix_ + currentFmtVersion_), inputFmtVersion_(""),
    inputFmtDate_(tZero) {idx_=0;};
  inline SgAgvChunk(int idx) : sFile_(idx), sPrea_(idx), sText_(idx), sTocs_(idx), sData_(idx), 
    sHeap_("HEAP", idx), sMagicString_(magicPrefix_ + currentFmtVersion_), inputFmtVersion_(""),
    inputFmtDate_(tZero) {idx_=idx;};
  inline ~SgAgvChunk() {};
  inline QString className() const {return "SgAgvChunk";};
  inline int getIdx() const {return idx_;};

  void fillDataStructures(const SgVlbiSession *session, SgAgvDriver* drv);

  void exportData(QTextStream& ts, SgAgvDriver* drv);
  void importData(QTextStream& ts, SgAgvDriver* drv);

  inline SgVlbiHistory& history() {return sText_.history();};
  inline const SgVlbiHistory& history() const {return sText_.history();};

  
protected:
  int                           idx_;
  SgAgvFileSection              sFile_;
  SgAgvPreaSection              sPrea_;
  SgAgvTextSection              sText_;
  SgAgvTocsSection              sTocs_;
  SgAgvDataSection              sData_;
  SgAgvSection                  sHeap_;
  QString                       sMagicString_;
  static const QString          magicPrefix_;
  static const QString          currentFmtVersion_;  //the verions is a string in a form of an
                                                     //date, like "2005.01.14"
  QString                       inputFmtVersion_;
  SgMJD                         inputFmtDate_;
  bool parseMagicString(const QString& str);
  bool parseEndString(const QString& str);
};
/*=====================================================================================================*/


/*=====================================================================================================*/

/*=====================================================================================================*/
#endif     //SG_IO_AGV

