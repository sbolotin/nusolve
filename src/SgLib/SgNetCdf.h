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


#ifndef SG_NET_CDF_H
#define SG_NET_CDF_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <netcdf.h>


#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>



#include <SgAttribute.h>
#include <SgMatrix.h>
#include <SgVector.h>



class SgNcdfDimension;
extern const SgNcdfDimension                 dUnity;



/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgNcdfDimension
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  inline SgNcdfDimension() : name_("") {n_=-1; id_=-1;};

  inline SgNcdfDimension(const QString& name, int n, int id) : name_(name) {n_=n; id_=id;};

  inline SgNcdfDimension(const SgNcdfDimension& d) : name_(d.getName()) 
    {setN(d.getN()); setId(d.getId());};

  /**A destructor.
   * Destroys the object.
   */
  inline ~SgNcdfDimension(){};

  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  inline SgNcdfDimension& operator=(const SgNcdfDimension& d)
    {setName(d.getName()); setN(d.getN()); setId(d.getId()); return *this;};
  
  /**Returns name of the dimension (a key).
   */
  inline const QString& getName() const {return name_;};

  /**Returns value of the dimension.
   */
  inline int getN() const {return n_;};

  /**Returns value of the dimension Id.
   */
  inline int getId() const {return id_;};
  
  inline void setName(const QString& name) {name_ = name;};

  inline void setN(int n) {n_ = n;};

  inline void setId(int id) {id_ = id;};
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  void debug_output();

private:
  QString                       name_;
  int                           n_;
  int                           id_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgNcdfAttribute
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgNcdfAttribute();

  SgNcdfAttribute(const QString name, nc_type typeOfData, size_t numOfElements, void *data);

  SgNcdfAttribute(const SgNcdfAttribute&);

  /**A destructor.
   * Destroys the object.
   */
  ~SgNcdfAttribute();

  //
  // Interfaces:
  //
  inline const QString& getName() const {return name_;};
  inline nc_type getTypeOfData() const {return typeOfData_;};
  inline size_t getNumOfElements() const {return numOfElements_;};
  inline const void* getData() const {return data_;};
  //
  inline void setName(const QString& str) {name_ = str;};
  inline void setTypeOfData(nc_type type) {typeOfData_ = type;};
  inline void setNumOfElements(size_t num) {numOfElements_ = num;};
  void setData(const void*, size_t);
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  //
  // Friends:
  //

  //
  // I/O:
  //
  int nc_get_attr(int ncid, int varid);
  int nc_put_attr(int ncid, int varid);
  void debug_output();

private:
  QString                       name_;
  nc_type                       typeOfData_;
  size_t                        numOfElements_;
  void                         *data_;
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgNcdfVariable
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgNcdfVariable();

  /**A destructor.
   * Destroys the object.
   */
  ~SgNcdfVariable();

  //
  // Interfaces:
  //
  inline const QString& getName() const {return name_;};
  inline int getId() const {return id_;};
  inline nc_type getTypeOfData() const {return typeOfData_;};
  inline const void* getData() const {return data_;};

  const char* data2char() const;
  char* data2char();
  const short* data2short() const;
  short* data2short();
  const int* data2int() const;
  int* data2int();
  const double* data2double() const;
  double* data2double();

  /**Returns a reference on the pointer -- full access.
   */
  inline void*& data() {return data_;};
  
  inline size_t numOfElements() const {return numOfElements_;};

  inline void setName(const QString& name) {name_ = name;};
  inline void setId(int id) {id_ = id;};
  inline void setTypeOfData(nc_type type) {typeOfData_ = type;};
  //  inline void setNumOfElements(size_t num) {numOfElements_ = num;}; 
  inline void setData(void* d) {data_ = d;};

  inline const QMap<QString, SgNcdfDimension*>&dimensionByName() const {return dimensionByName_;};
  inline const QList<SgNcdfDimension*>&        dimensions() const {return dimensions_;};
  inline const QMap<QString, SgNcdfAttribute*>&attributeByName() const {return attributeByName_;};
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**
   */
  inline void addDimension(const SgNcdfDimension& d, int idx=-1) 
    {addDimension(d.getName(), d.getN(), d.getId(), idx);};
  void addDimension(const QString& name, int n, int id, int idx=-1);
  void delDimension(const QString&);

  /**
   */
  void addAttribute(const SgNcdfAttribute&);
  void addAttribute(SgNcdfAttribute*);
  void delAttribute(const QString&);
  /**
   */
 
  //
  // Friends:
  //

  //
  // I/O:
  //
  int nc_get_var(int ncid);
  int nc_put_var(int ncid);
  void debug_output();
  
  int sizeOfData() const;

  void allocateData();
  void multiplyData(int);
  void check4multiplication();

private:
  QString                         name_;
  int                             id_;
  QMap<QString, SgNcdfDimension*> dimensionByName_;
  QList<SgNcdfDimension*>         dimensions_;      // to keep the order of dimensions
  QMap<QString, SgNcdfAttribute*> attributeByName_;
  nc_type                         typeOfData_;
  size_t                          numOfElements_;
  void                           *data_;

  void calcNumOfElements();
};
/*=====================================================================================================*/




class SgIdentities;
/***===================================================================================================*/
/**
 * 
 *
 */
/*=====================================================================================================*/
class SgNetCdf
{
public:
  enum DataType
  {
    DT_INT,
    DT_DBL,
    DT_STR
  };
  enum OperationMode
  {
    OM_REGULAR,
    OM_DRY_RUN,
  };
  
  static const QString          svcStub;
  static const QString          svcCreateTime;
  static const QString          svcCreatedBy;
  static const QString          svcProgram;
  static const QString          svcSubroutine;
  static const QString          svcVgosDbVersion;
  static const QString          svcDataOrigin;
  static const QString          svcTimeTag;
  static const QString          svcTimeTagFile;
  static const QString          svcSession;
  static const QString          svcStation;
  static const QString          svcBand;

  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgNetCdf();

  /**A constructor.
   * Creates a copy of the object.
   */
  SgNetCdf(const QString& fileName, const SgIdentities* ids=NULL,
    const QString& fmtVerId=QString(""), const QString& sessionId=QString(""),
    const QString& stationId=QString(""), const QString& bandId=QString(""));

  /**A destructor.
   * Destroys the object.
   */
  ~SgNetCdf();

  //
  // Interfaces:
  //
  // gets:
  inline const QString& getFileName() const;

  inline const QString& getFmtVerId() const {return fmtVerId_;};
  inline const QString& getSessionId() const {return sessionId_;};
  inline const QString& getStationId() const {return stationId_;};
  inline const QString& getBandId() const {return bandId_;};
  inline OperationMode getOperationMode() const {return operationMode_;};
  
  // sets:
  inline void setFileName(const QString&);
  
  inline void setIdentities(const SgIdentities*);
  inline void setFmtVerId(const QString& str) {fmtVerId_ = str;};
  inline void setSessionId(const QString& str) {sessionId_ = str;};
  inline void setStationId(const QString& str) {stationId_ = str;};
  inline void setBandId(const QString& str) {bandId_ = str;};
  inline void setOperationMode(OperationMode om) {operationMode_ = om;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // can be NULL in return:
  inline SgNcdfVariable* lookupServiceVar(const QString& name) const
    {return serviceVars_.contains(name.toUpper())?serviceVars_.value(name.toUpper()):NULL;};

  // can be NULL in return:
  inline SgNcdfVariable* lookupVar(const QString& name) const
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper()):NULL;};

  /**
   */
  inline const char* lookupData2char(const QString& name) const
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2char():NULL;};
  inline char* lookupData2char(const QString& name)
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2char():NULL;};
  /**
   */
  inline const short* lookupData2short(const QString& name) const
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2short():NULL;};
  inline short* lookupData2short(const QString& name)
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2short():NULL;};
  /**
   */
  inline const int* lookupData2int(const QString& name) const
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2int():NULL;};
  inline int* lookupData2int(const QString& name)
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2int():NULL;};
  /**
   */
  inline const double* lookupData2double(const QString& name) const
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2double():NULL;};
  inline double* lookupData2double(const QString& name)
    {return contentVars_.contains(name.toUpper())?contentVars_.value(name.toUpper())->data2double():NULL;};

  
  /**
   */
  void registerVariable(SgNcdfVariable*, bool is4Service=false);

  /**
   */
  void setServiceVars(const QString& stub, const QString& dataOrigin, 
    const QString& timeTag, const QString& timeTagFile);

  /**
   */
  void reset();

  /**
   */
  bool getData();

  /**
   */
  bool putData();


  //
  // Friends:
  //

  //
  // I/O:
  //

private:
  const SgIdentities             *identities_;
  QString                         fileName_;
  QMap<QString, SgNcdfDimension*> dimensionByName_;
  QMap<QString, SgNcdfVariable*>  serviceVars_;
  QMap<QString, SgNcdfVariable*>  contentVars_;
  
  //
  QString                         fmtVerId_;
  QString                         sessionId_;
  QString                         stationId_;
  QString                         bandId_;
  OperationMode                   operationMode_;
  
  /**
   */
  bool                            setServiceVar(const QString& varName, const QString& varValue);
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgNetCdf inline members:                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:




//
// INTERFACES:
//
//
inline const QString& SgNetCdf::getFileName() const
{
  return fileName_;
};



// sets:
inline void SgNetCdf::setFileName(const QString& fileName)
{
  fileName_ = fileName;
};



//
inline void SgNetCdf::setIdentities(const SgIdentities *ids)
{
  identities_ = ids;
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
#endif //SG_NET_CDF_H
