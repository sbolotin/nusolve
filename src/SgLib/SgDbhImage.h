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

#ifndef SG_DBHIMAGE_H
#define SG_DBHIMAGE_H

#include <math.h>


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>
#include <QtCore/QDataStream>
#include <QtCore/QTextStream>
#include <QtCore/QList>
#include <QtCore/QHash>



#include <SgMJD.h>


/*  To Do:
 * 
 *  0) There could be duplicates of LCODEs in different TOCs (e.g., "UTC TAG "). Should we 
 *     delete data from some TOC or it is OK?
 * 
 *  1) Some old DB files have TOCs that are not presented in data section (e.g., 79NOV25X__V048), 
 *     software should be aware of it.
 * 
 *  2) Epochs of history records -- what about time zones? Convert them to UTC?
 * 
 */


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgDbhStream : public QDataStream // just reserved type for this kind of operations
{};
/*=====================================================================================================*/



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgDbhDatumDescriptor
{
  friend class SgDbhTeBlock;
public:
  enum Type {T_R8=0, T_I2=1, T_A2=2, T_D8=3, T_J4=4, T_UNKN=5};
  //
  // constructors/destructors:
  //
  SgDbhDatumDescriptor();
  SgDbhDatumDescriptor(const QString&, const QString&);
  inline ~SgDbhDatumDescriptor() {};
  //
  // Interfaces:
  //
  inline const QString& getLCode() const {return lCode_;};
  inline const QString& getDescription() const {return description_;};
  inline short dim1() const {return dim1_;};
  inline short dim2() const {return dim2_;};
  inline short dim3() const {return dim3_;};
  inline short getModifiedAtVersion() const {return modifiedAtVersion_;};
  inline short nTc() const {return nTc_;};
  inline short nTe() const {return nTe_;};
  inline int offset() const {return offset_;};
  inline Type type() const {return type_;};

  void setLCode(const QString& lCode);
  void setDescription(const QString& description);
  inline void setType(Type type) {type_=type;};
  inline void setDimensions(int d1, int d2, int d3) 
    {dim1_=(short)d1; dim2_=(short)d2; dim3_=(short)d3;};
  inline void setModifiedAtVersion(int V) {modifiedAtVersion_=(short)V;};
  inline void setNTc(int nTc) {nTc_=(short)nTc;};
  inline void setNTe(int nTe) {nTe_=(short)nTe;};
  inline void setOffset(int offset) {offset_=offset;};
  //
  // Functions:
  //
  SgDbhDatumDescriptor& operator=(const SgDbhDatumDescriptor& descriptor);
  inline QString className() const {return "SgDbhDatumDescriptor";};
  inline const QString& typeName() const {return typeNames_[type_];};
  static const QString& typeName(Type type) {return typeNames_[type];};
  //
protected:
  static const QString
                    typeNames_[6];
  QString           lCode_;
  QString           description_;
  Type              type_;
  short             dim1_, dim2_, dim3_;
  short             modifiedAtVersion_;
  short             nTc_, nTe_;
  int               offset_;
};
/*=====================================================================================================*/





// Try do not use <SgDbhFormat.h>, keep DBH reading/writting stuff not visible to end user,
// so user should use SgDbhImage class and its methods to manipulate with data from DBH files.
class SgDbhStream;
class SgDbhStartBlock;
class SgDbhHistoryEntry;
class SgDbhTcBlock;
class SgDbhDataBlock;
class SgDbhFormat;
class SgDbhPhysicalRecord;
class SgDbhObservationEntry;
/***===================================================================================================*/
/**
 * The object that keeps Mk-III DB file into memory and provides access to their fields.
 *
 */
/**====================================================================================================*/
class SgDbhImage
{
public:
  enum ContentState { CS_Bare,            //< image contains nothing useful, just has been inited
                      CS_DataFromFile,    //< image keeps data form DBH file
                      CS_DataAltered,     //< image keeps data form DBH file, some data are altered
                    };
  enum FormatState  { FS_Bare,            //< image does not contain format, just has been inited
                      FS_FormatFromFile,  //< image keeps format according to read DBH file
                      FS_FormatAltering,  //< the format of the image is in changing state
                      FS_FormatAltered,   //< the format of the image has been changed
                    };
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  SgDbhImage();

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgDbhImage();

  //
  // Interfaces:
  //
  inline void setDumpStream(QTextStream*);
  inline ContentState contentState() const {return contentState_;};
  inline FormatState formatState() const {return formatState_;};

  inline QTextStream* getDumpStream();
  SgMJD fileCreationEpoch() const;
  int   fileVersion() const;
  inline int currentVersion() const;
  const QString& fileName() const;
  const QString& sessionDescription() const;
  const QString& sessionID() const;
  const QString& previousFileName() const;
  const QString& previousFileDescription() const;

  const QList<SgDbhHistoryEntry*>& historyList() {return history_;};

  //
  // Functions:
  //
  inline const QString className() const {return "SgDbhImage";};

  void addHistoryEntry(const QString& text, const SgMJD& t = SgMJD::currentMJD().toUtc());

  void startFormatModifying();

  void finisFormatModifying();

  bool addDescriptor(SgDbhDatumDescriptor*);

  bool delDescriptor(SgDbhDatumDescriptor*);

  // get a descriptor for the tag (=LCODE)
  inline SgDbhDatumDescriptor *lookupDescriptor(const char* tag);

  // returns number of observations in the image
  int numberOfObservations() const {return listOfObservations_.size();};

  // reading data:
  QString getStr(SgDbhDatumDescriptor *d, int i, int j, int obsNumber=-1);

  template<class C> C getData(const QString&, SgDbhDatumDescriptor*, int, int, int, int =-1);

  double getR8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber=-1);

  double getD8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber=-1);

  int getJ4(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber=-1);

  short getI2(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber=-1);

  // altering data:
  void setStr(SgDbhDatumDescriptor *d, int i, int j, int obsNumber, const QString& str);

  template<class C> void setData(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, C r8);

  void setR8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, double r8);

  void setD8(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, double d8);

  void setJ4(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, int j4);

  void setI2(SgDbhDatumDescriptor *d, int i, int j, int k, int obsNumber, short i2);
   
  //
  void dumpFormat();

  // setups proper values for the header of a file:
  void prepare2save();

  inline void alterCode(const QString& c) {isSessionCodeAltered_=true; alterCode_=c;};

  //
  // Friends:
  //
  friend SgDbhStream &operator>>(SgDbhStream&, SgDbhImage&);

  friend SgDbhStream &operator<<(SgDbhStream&, const SgDbhImage&);

  //
  // I/O:
  //
  // ...

private:
  void clearHistoryList();
  void clearListOfObservations();
  void copyToc0Content();
  SgDbhPhysicalRecord *properRecord(SgDbhDatumDescriptor*, int);
  void updateDataRecords(QList<SgDbhDataBlock*>*, int);
  void updateDescriptorsParameters();
  void writeDataRecords(SgDbhStream&, const QList<SgDbhDataBlock*>*, int) const;

protected:
  //
  ContentState                            contentState_;
  FormatState                             formatState_;
  int                                     currentVersion_;
  QString                                 canonicalName_; // that sould looks like: `10MAY06XE'
  // dump stream:
  QTextStream                            *dumpStream_;
  // initial block:
  SgDbhStartBlock                        *startBlock_;
  // history block (just a list of SgDbhHistoryEntry, nothing special):
  QList<SgDbhHistoryEntry*>               history_;
  // fromat description:
  SgDbhFormat                            *format_;
  // an array of parameter descriptors that is indexed by thier LCodes (common for whole image):
  QHash<QString, SgDbhDatumDescriptor*>   descriptorByLCode_;
  // Common data (TOC#0):
  QList<SgDbhDataBlock*>                  listOfDataBlocksToc0_;
  // Observations:
  QList<SgDbhObservationEntry*>           listOfObservations_;
  // for format changing:
  QList<SgDbhDatumDescriptor*>            listOfNewDescriptors_;
  QList<SgDbhDatumDescriptor*>            listOfDeletedDescriptors_;

  // for test purposes:
  bool                                    isSessionCodeAltered_;
  QString                                 alterCode_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgDbhImage inline members:                                                                          */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//

//
// INTERFACES:
//
inline void SgDbhImage::setDumpStream(QTextStream* s)
{
  dumpStream_ = s;
};



inline QTextStream* SgDbhImage::getDumpStream()
{
  return dumpStream_;
};



//
inline int SgDbhImage::currentVersion() const
{
  return currentVersion_;
};



//
// FUNCTIONS:
//
SgDbhDatumDescriptor *SgDbhImage::lookupDescriptor(const char* tag)
{
  return descriptorByLCode_.contains(tag) ? descriptorByLCode_.value(tag) : NULL;
};
/*=====================================================================================================*/



/*=====================================================================================================*/
#endif //SG_DBHIMAGE_H
