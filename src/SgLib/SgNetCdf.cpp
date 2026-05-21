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

#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgNetCdf.h>
#include <SgVersion.h>


//
//
static const QString            dimensionUnityName("DimUnity");
static const int                dimensionUnityValue = 1;
const SgNcdfDimension           dUnity(dimensionUnityName, dimensionUnityValue, -1);

/*=======================================================================================================
*
*                           Helpers:
* 
*======================================================================================================*/
//
size_t ncdfaux_type_2_size(nc_type type)
{
  size_t                        size_of_type=1;
  switch (type)
  {
    case NC_BYTE:
    case NC_CHAR:
      size_of_type = sizeof(char);
      break;
    case NC_SHORT:
      size_of_type = sizeof(short);
      break;
    case NC_INT:
      size_of_type = sizeof(int);
      break;
    case NC_FLOAT:
      size_of_type = sizeof(float);
      break;
    case NC_DOUBLE:
      size_of_type = sizeof(double);
      break;
    case NC_NAT:
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, "ncdfaux_type_2_size: unexpected data type");
      break;
  };
  return size_of_type;
};



//
void ncdfaux_data_allocate(void*& dest, nc_type type, size_t num)
{
  if (dest != NULL)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, 
      "ncdfaux_data_allocate: the destination is not clean");
  };
  size_t                        size_of_type=ncdfaux_type_2_size(type);
  switch (type)
  {
    case NC_BYTE:
    case NC_CHAR:
      dest = new char[num];
      break;
    case NC_SHORT:
      dest = new short[num];
      break;
    case NC_INT:
      dest = new int[num];
      break;
    case NC_FLOAT:
      dest = new float[num];
      break;
    case NC_DOUBLE:
      dest = new double[num];
      break;
    case NC_NAT:
      dest = NULL;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, "ncdfaux_data_allocate: unexpected data type");
      break;
  };
  if (dest)
    memset(dest, 0, size_of_type*num);
};



//
void ncdfaux_data_allocate_n_copy(void*& dest, const void* src, nc_type type, size_t num)
{
  if (dest != NULL)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, 
      "ncdfaux_data_allocate_n_copy: the destination is not clean");
  };
  size_t                        size_of_type=ncdfaux_type_2_size(type);
  switch (type)
  {
    case NC_BYTE:
    case NC_CHAR:
      dest = new char[num];
      break;
    case NC_SHORT:
      dest = new short[num];
      break;
    case NC_INT:
      dest = new int[num];
      break;
    case NC_FLOAT:
      dest = new float[num];
      break;
    case NC_DOUBLE:
      dest = new double[num];
      break;
    case NC_NAT:
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, 
        "ncdfaux_data_allocate_n_copy: unexpected data type");
      break;
  };
  memcpy(dest, src, size_of_type*num);
};



//
void ncdfaux_data_free(void*& dest, nc_type type)
{
  if (!dest)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, "ncdfaux_data_free: the destination is NULL");
    return;
  };
  switch (type)
  {
    case NC_BYTE:
    case NC_CHAR:
      delete[] (char*)dest;
      break;
    case NC_SHORT:
      delete[] (short*)dest;
      break;
    case NC_INT:
      delete[] (int*)dest;
      break;
    case NC_FLOAT:
      delete[] (float*)dest;
      break;
    case NC_DOUBLE:
      delete[] (double*)dest;
      break;
    case NC_NAT:
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, "ncdfaux_data_free: unexpected data type");
      break;
  };
  dest = NULL;
};



//
QString ncdfaux_type_2_string(nc_type xtype)
{
  QString str("");
  switch (xtype)
  {
    case NC_BYTE:
      str = "NC_BYTE";
      break;
    case NC_CHAR:
      str = "NC_CHAR";
      break;
    case NC_SHORT:
      str = "NC_SHORT";
      break;
    case NC_INT:
      str = "NC_INT";
      break;
    case NC_FLOAT:
      str = "NC_FLOAT";
      break;
    case NC_DOUBLE:
      str = "NC_DOUBLE";
      break;
    case NC_NAT:
      str = "NC_NAT";
      break;
  };
  return str;
};







/*=====================================================================================================*/
/*                                                                                                     */
/* SgNcdfDimension implementation                                                                      */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgNcdfDimension::className()
{
  return "SgNcdfDimension";
};



//
void SgNcdfDimension::debug_output()
{
  std::cout << "                    dim #" << id_ << ", \"" << qPrintable(name_) << "\" = " 
            << n_ << "\n";
};
/*=====================================================================================================*/


const QString                   SgNetCdf::svcStub("Stub");
const QString                   SgNetCdf::svcCreateTime("CreateTime");
const QString                   SgNetCdf::svcCreatedBy("CreatedBy");
const QString                   SgNetCdf::svcProgram("Program");
const QString                   SgNetCdf::svcSubroutine("Subroutine");
const QString                   SgNetCdf::svcVgosDbVersion("vgosDB_Version");
const QString                   SgNetCdf::svcDataOrigin("DataOrigin");
const QString                   SgNetCdf::svcTimeTag("TimeTag");
const QString                   SgNetCdf::svcTimeTagFile("TimeTagFile");
const QString                   SgNetCdf::svcSession("Session");
const QString                   SgNetCdf::svcStation("Station");
const QString                   SgNetCdf::svcBand("Band");



/*=====================================================================================================*/
/*                                                                                                     */
/* SgNcdfAttribute implementation                                                                      */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgNcdfAttribute::className()
{
  return "SgNcdfAttribute";
};



//
SgNcdfAttribute::SgNcdfAttribute()
  : name_("")
{
  data_ = NULL;
  numOfElements_ = 0;
  typeOfData_ = NC_NAT; // == "not a type"
};



//
SgNcdfAttribute::SgNcdfAttribute(const QString name, nc_type typeOfData, 
  size_t numOfElements, void *data)
  : name_(name)
{
  typeOfData_ = typeOfData;
  numOfElements_ = numOfElements;
  data_ = NULL;
  ncdfaux_data_allocate_n_copy(data_, data, typeOfData_, numOfElements_);
};



//
SgNcdfAttribute::SgNcdfAttribute(const SgNcdfAttribute& attr)
  : name_(attr.getName())
{
  setTypeOfData(attr.getTypeOfData());
  setNumOfElements(attr.getNumOfElements());
  data_ = NULL;
  ncdfaux_data_allocate_n_copy(data_, attr.getData(), typeOfData_, numOfElements_);
};



//
SgNcdfAttribute::~SgNcdfAttribute()
{
  if (data_)
    ncdfaux_data_free(data_, typeOfData_);
};



//
void SgNcdfAttribute::setData(const void* src, size_t num)
{
  if (data_)
    ncdfaux_data_free(data_, typeOfData_);
  ncdfaux_data_allocate_n_copy(data_, src, typeOfData_, num);
};



//
int SgNcdfAttribute::nc_get_attr(int ncid, int varid)
{
  int                           rc(0);
  if (!data_)
    ncdfaux_data_allocate(data_, typeOfData_, numOfElements_);
  switch (typeOfData_)
  {
    case NC_BYTE:
      rc = nc_get_att_schar(ncid, varid, qPrintable(name_), (signed char*)data_);
      break;
    case NC_CHAR:
      rc = nc_get_att_text(ncid, varid, qPrintable(name_), (char*)data_);
      break;
    case NC_SHORT:
      rc = nc_get_att_short(ncid, varid, qPrintable(name_), (short*)data_);
      break;
    case NC_INT:
      rc = nc_get_att_int(ncid, varid, qPrintable(name_), (int*)data_);
      break;
    case NC_FLOAT:
      rc = nc_get_att_float(ncid, varid, qPrintable(name_), (float*)data_);
      break;
    case NC_DOUBLE:
      rc = nc_get_att_double(ncid, varid, qPrintable(name_), (double*)data_);
      break;
    case NC_NAT:
      rc = NC_EBADTYPE;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::readAttr(): unexpected data type");
      break;
  };
  return rc;
};



//
int SgNcdfAttribute::nc_put_attr(int ncid, int varid)
{
  int                           rc(0);
  if (!data_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::nc_put_attr(): the data is NULL");
    return NC_ENOMEM;
  };
  switch (typeOfData_)
  {
    case NC_CHAR:
      rc = nc_put_att_text(ncid, varid, qPrintable(name_), numOfElements_, 
        (const char*)data_);
      break;
    case NC_BYTE:
    case NC_SHORT:
    case NC_INT:
    case NC_FLOAT:
    case NC_DOUBLE:
      rc = nc_put_att(ncid, varid, qPrintable(name_), typeOfData_, numOfElements_, data_);
      break;
    case NC_NAT:
      rc = NC_EBADTYPE;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::nc_put_attr(): unexpected data type");
      break;
  };
  return rc;
};



//
void SgNcdfAttribute::debug_output()
{
  
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgNcdfVariable implementation                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgNcdfVariable::className()
{
  return "SgNcdfVariable";
};



//
SgNcdfVariable::SgNcdfVariable() : 
  name_(), 
  dimensionByName_(),
  dimensions_(),
  attributeByName_()
{
  id_ = -1;
  data_ = NULL;
  typeOfData_ = NC_NAT; // == "not a type"
  numOfElements_ = 0;
};



//
SgNcdfVariable::~SgNcdfVariable()
{
  if (data_)
    ncdfaux_data_free(data_, typeOfData_);
  for (int i=0; i<dimensions_.size(); i++)
    delete dimensions_.at(i);
  dimensions_.clear();
  dimensionByName_.clear();
  QMap<QString, SgNcdfAttribute*>::iterator it=attributeByName_.begin();
  for (; it!=attributeByName_.end(); ++it)
    delete it.value();
  attributeByName_.clear();
};



//
const char* SgNcdfVariable::data2char() const
{
  if (typeOfData_ != NC_CHAR)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2char(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (const char*)data_;
};



//
char* SgNcdfVariable::data2char()
{
  if (typeOfData_ != NC_CHAR)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2char(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (char*)data_;
};



//
const short* SgNcdfVariable::data2short() const
{
  if (typeOfData_ != NC_SHORT)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2short(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (const short*)data_;
};



//
short* SgNcdfVariable::data2short()
{
  if (typeOfData_ != NC_SHORT)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2short(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (short*)data_;
};



//
const int* SgNcdfVariable::data2int() const 
{
  if (typeOfData_ != NC_INT)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2int(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (const int*)data_;
};



//
int* SgNcdfVariable::data2int()
{
  if (typeOfData_ != NC_INT)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2int(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (int*)data_;
};



//
const double* SgNcdfVariable::data2double() const 
{
  if (typeOfData_ != NC_DOUBLE)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2double(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (const double*)data_;
};



//
double* SgNcdfVariable::data2double()
{
  if (typeOfData_ != NC_DOUBLE)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::data2double(): wrong conversion, the data is of type " + ncdfaux_type_2_string(typeOfData_));
  return (double*)data_;
};



//
void SgNcdfVariable::addDimension(const QString& name, int n, int id, int idx)
{
  SgNcdfDimension                *d=new SgNcdfDimension(name, n, id);

  if (idx < 0)
    dimensions_.append(d);
  else
    dimensions_.insert(idx, d);

  if (!dimensionByName_.contains(name))
    dimensionByName_.insert(d->getName(), d);

  if (dimensions_.size() == 1)
    numOfElements_ = n;
  else
    numOfElements_ *= n;
};



//
void SgNcdfVariable::delDimension(const QString& name)
{
  int                           n, idx;
  if (!dimensionByName_.contains(name))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::delDimension(): the dimension " + name + " is not in the map");
    return;
  };
  //
  n = dimensionByName_.value(name)->getN();
  //
  idx = -1;
  for (int i=0; i<dimensions_.size(); i++)
    if (dimensions_.at(i)->getName() == name)
      idx = i;

  if (idx == -1)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::delDimension(): cannot find [" + name + "] in the list");
  else
  {
    delete dimensions_.at(idx);
    dimensions_.removeAt(idx);
  };
  //
  //n = dimensionByName_.value(name)->getN();
  // remove from the map:
  //dimensionByName_.remove(name);
  if (n != 0)
  {
    if (dimensionByName_.size() == 0)
      numOfElements_ = 0;
    else
      numOfElements_ /= n;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::delDimension(): the dimension " + name + " was set to zero");
};



//
void SgNcdfVariable::addAttribute(const SgNcdfAttribute& attr)
{
  if (attributeByName_.contains(attr.getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::addAttribute(): the attribute " + attr.getName() + " is already in the list");
    delAttribute(attr.getName());
  };
  SgNcdfAttribute              *a=new SgNcdfAttribute(attr);
  attributeByName_.insert(a->getName(), a);
};



//
void SgNcdfVariable::addAttribute(SgNcdfAttribute* a)
{
  if (attributeByName_.contains(a->getName()))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() +
      "::addAttribute(): the attribute " + a->getName() + " is already in the list");
    delAttribute(a->getName());
  };
  attributeByName_.insert(a->getName(), a);
};



//
void SgNcdfVariable::delAttribute(const QString& name)
{
  if (!attributeByName_.contains(name))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::delAttribute(): the attribute " + name + " is not in the list");
    return;
  };
  delete attributeByName_.value(name);
  // remove from the map:
  attributeByName_.remove(name);
};



//
void SgNcdfVariable::calcNumOfElements()
{
  if (dimensions_.size()==0)
  {
    numOfElements_ = 0;
    return;
  };
  numOfElements_ = 1;
  for (int i=0; i<dimensions_.size(); i++)
    numOfElements_ *= dimensions_.at(i)->getN();
};



//
int SgNcdfVariable::nc_get_var(int ncid)
{
  int                           rc(0);
  if (!data_)
    ncdfaux_data_allocate(data_, typeOfData_, numOfElements_);
  switch (typeOfData_)
  {
    case NC_BYTE:
      rc = nc_get_var_schar(ncid, id_, (signed char*)data_);
      break;
    case NC_CHAR:
      rc = nc_get_var_text(ncid, id_, (char*)data_);
      break;
    case NC_SHORT:
      rc = nc_get_var_short(ncid, id_, (short*)data_);
      break;
    case NC_INT:
      rc = nc_get_var_int(ncid, id_, (int*)data_);
      break;
    case NC_FLOAT:
      rc = nc_get_var_float(ncid, id_, (float*)data_);
      break;
    case NC_DOUBLE:
      rc = nc_get_var_double(ncid, id_, (double*)data_);
      break;
    case NC_NAT:
      rc = NC_EBADTYPE;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::nc_get_var(): unexpected data type");
      break;
  };
  return rc;
};



//
int SgNcdfVariable::nc_put_var(int ncid)
{
  int                           rc(0);
  if (!data_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::nc_put_var(): the data is NULL");
    return NC_ENOMEM;
  };
  switch (typeOfData_)
  {
    case NC_BYTE:
      rc = nc_put_var_schar(ncid, id_, (const signed char*)data_);
      break;
    case NC_CHAR:
      rc = nc_put_var_text(ncid, id_, (const char*)data_);
      break;
    case NC_SHORT:
      rc = nc_put_var_short(ncid, id_, (const short*)data_);
      break;
    case NC_INT:
      rc = nc_put_var_int(ncid, id_, (const int*)data_);
      break;
    case NC_FLOAT:
      rc = nc_put_var_float(ncid, id_, (const float*)data_);
      break;
    case NC_DOUBLE:
      rc = nc_put_var_double(ncid, id_, (const double*)data_);
      break;
    case NC_NAT:
      rc = NC_EBADTYPE;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
        "::nc_put_var(): unexpected data type");
      break;
  };
  return rc;
};



//
void SgNcdfVariable::multiplyData(int mult)
{
  size_t                        size_of_type=ncdfaux_type_2_size(typeOfData_);
  void                         *oldData=data_;
  data_ = NULL;
  ncdfaux_data_allocate(data_, typeOfData_, mult*numOfElements_);
  for (int i=0; i<mult; i++)
    memcpy((unsigned char*)data_ + size_of_type*numOfElements_*i, oldData, size_of_type*numOfElements_);
  ncdfaux_data_free(oldData, typeOfData_);
  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
    "::multiplyData(): the data of \"" + name_ + "\" were multiplied " + 
    QString("").setNum(mult) + " times");
  //
  // adjust the dimensions:
  if (dimensions_.size()==1 && dimensions_.at(0)->getN()==1)
    dimensions_.first()->setN(mult);
  else
  {
    SgNcdfDimension *d=new SgNcdfDimension("REPEAT-multiplier", mult, -1);
    dimensions_.prepend(d);
    dimensionByName_.insert(d->getName(), d);
  };
  numOfElements_ *= mult;
};



//
void SgNcdfVariable::check4multiplication()
{
  int                           nSize;
  SgNcdfAttribute              *attr=new SgNcdfAttribute("Size", NC_INT, 1, &(nSize=sizeOfData()));
  addAttribute(attr);

  if (attributeByName_.contains("REPEAT"))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::check4multiplication(): the variable \"" + name_ + "\" already has an attribute \"REPEAT\"");
    return;
  };
  int                           cDim;
  if (dimensions_.size() && (cDim=dimensions_.at(0)->getN())>1) // ok, there is something
  {
    QString                     cDimName=dimensions_.at(0)->getName();
    bool                        canBeConvolved=true;
    size_t                      size_of_type=ncdfaux_type_2_size(typeOfData_);
    size_t                      size_of_newData;
    void                       *newData=NULL;
    int                         newNumOfElements=1;
    for (int i=1; i<dimensions_.size(); i++)
      newNumOfElements *= dimensions_.at(i)->getN();

    ncdfaux_data_allocate(newData, typeOfData_, newNumOfElements);
    size_of_newData = size_of_type*newNumOfElements;
    memcpy((unsigned char*)newData, data_, size_of_newData);
    for (int i=0; i<cDim && canBeConvolved; i++)
      if (memcmp((unsigned char*)data_ + size_of_newData*i, newData, size_of_newData) != 0)
        canBeConvolved = false;
    if (canBeConvolved) // ok, do it:
    {
      // adjust the dimensions:
      delDimension(cDimName);
      if (!dimensions_.size())
      {
        addDimension(dimensionUnityName, dimensionUnityValue, -1);
        numOfElements_ = newNumOfElements;
      };
      // set up the attribute:
      attr = new SgNcdfAttribute("REPEAT", NC_INT, 1, &cDim);
      addAttribute(attr);
      attr = new SgNcdfAttribute("StoredSize", NC_INT, 1, &(nSize=sizeOfData()));
      addAttribute(attr);
      // delete old data:
      ncdfaux_data_free(data_, typeOfData_);
      data_ = newData;
      // make report:
      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() +
        "::check4multiplication(): the variable \"" + name_ + 
        "\" has been convolved: dimension \"" + cDimName + "\", multiplier=" + 
        QString("").setNum(cDim));
    };
  };
};



//
void SgNcdfVariable::allocateData()
{
  if (data_)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::allocateData(): the data of \"" + name_ + "\" were already allocated");
    ncdfaux_data_free(data_, typeOfData_);
  };
  if (numOfElements_>0)
    ncdfaux_data_allocate(data_, typeOfData_, numOfElements_);
  else
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
      "::allocateData(): impossible to allocate the data of \"" + name_ + "\": the size is zero");
};



//
int SgNcdfVariable::sizeOfData() const
{
  return ncdfaux_type_2_size(typeOfData_)*numOfElements_;
};



//
void SgNcdfVariable::debug_output()
{
  QString                       str("");
  std::cout << "Variable # " << id_ << " \"" << qPrintable(name_) << "\" of type ";
  std::cout << qPrintable(ncdfaux_type_2_string(typeOfData_));
  std::cout << " and length " << numOfElements_ << "\n";
  std::cout << "         Dimensions:\n";
  for (int i=0; i<dimensions_.size(); i++)
    dimensions_.at(i)->debug_output();
  // attributes:
  if (attributeByName_.size())
  {
    std::cout << "         Variable's attributes:\n";
    const QList<SgNcdfAttribute*>         &atts = attributeByName_.values();
    for (int i=0; i<atts.size(); i++)
    {
      SgNcdfAttribute    *a=atts.at(i);
      
      std::cout << "                    Attr \"" << qPrintable(a->getName()) << "\" of type ";
      std::cout << qPrintable(ncdfaux_type_2_string(a->getTypeOfData()));
      std::cout << " and length " << a->getNumOfElements();
      str = "";
      switch (a->getTypeOfData())
      {
      case NC_BYTE:
        for (unsigned int k=0; k<a->getNumOfElements(); k++)
          str += QString("").sprintf("%d,", ((signed char*)a->getData())[k]);
        break;
      case NC_CHAR:
        str = QString::fromLatin1((const char*)a->getData(), a->getNumOfElements());
        break;
      case NC_SHORT:
        for (unsigned int k=0; k<a->getNumOfElements(); k++)
          str += QString("").sprintf("%d,", ((short*)a->getData())[k]);
        break;
      case NC_INT:
        for (unsigned int k=0; k<a->getNumOfElements(); k++)
          str += QString("").sprintf("%d,", ((int*)a->getData())[k]);
        break;
      case NC_FLOAT:
        for (unsigned int k=0; k<a->getNumOfElements(); k++)
          str += QString("").sprintf("%g,", ((float*)a->getData())[k]);
        break;
      case NC_DOUBLE:
        for (unsigned int k=0; k<a->getNumOfElements(); k++)
          str += QString("").sprintf("%g,", ((double*)a->getData())[k]);
        break;
      case NC_NAT:
        str = "***";
        break;
      };
      std::cout << "; value : [" << qPrintable(str) << "]\n";
    };
  };
  // values:
  std::cout << "         Value: ";
  switch (typeOfData_)
  {
    case NC_BYTE:
      for (unsigned int k=0; k<numOfElements_; k++)
        std::cout << qPrintable(QString("").sprintf("%d,", ((signed char*)data_)[k]));
      std::cout << "\n";
      break;
    case NC_CHAR:
      std::cout << qPrintable(QString::fromLatin1((const char*)data_, numOfElements_));
      std::cout << "\n";
      break;
    case NC_SHORT:
      for (unsigned int k=0; k<numOfElements_; k++)
        std::cout << qPrintable(QString("").sprintf("%d,", ((short*)data_)[k]));
      std::cout << "\n";
      break;
    case NC_INT:
      for (unsigned int k=0; k<numOfElements_; k++)
        std::cout << qPrintable(QString("").sprintf("%d,", ((int*)data_)[k]));
      std::cout << "\n";
      break;
    case NC_FLOAT:
      for (unsigned int k=0; k<numOfElements_; k++)
        std::cout << qPrintable(QString("").sprintf("%g,", ((float*)data_)[k]));
      std::cout << "\n";
      break;
    case NC_DOUBLE:
      for (unsigned int k=0; k<numOfElements_; k++)
        std::cout << qPrintable(QString("").sprintf("%g,", ((double*)data_)[k]));
      std::cout << "\n";
      break;
    case NC_NAT:
      std::cout << "NC_NAT";
      std::cout << "\n";
      break;
  };
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgNetCdf implementation                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgNetCdf::className()
{
  return "SgNetCdf";
};



//
SgNetCdf::SgNetCdf() :
  fileName_(""),
  dimensionByName_(),
  serviceVars_(),
  contentVars_(),
//  dUnity_(dimensionUnityName, dimensionUnityValue, -1),
  fmtVerId_(""),
  sessionId_(""),
  stationId_(""),
  bandId_("")
{
  identities_ = NULL;
  dimensionByName_.insert(dUnity.getName(), new SgNcdfDimension(dUnity));
  operationMode_ = OM_REGULAR;
};



//
SgNetCdf::SgNetCdf(const QString& fileName, const SgIdentities *ids, const QString& fmtVerId,
  const QString& sessionId, const QString& stationId, const QString& bandId) :
  fileName_(fileName),
  dimensionByName_(),
  serviceVars_(),
  contentVars_(),
//  dUnity_(dimensionUnityName, dimensionUnityValue, -1),
  fmtVerId_(fmtVerId),
  sessionId_(sessionId),
  stationId_(stationId),
  bandId_(bandId)
{
  identities_ = ids;
  dimensionByName_.insert(dUnity.getName(), new SgNcdfDimension(dUnity));
  operationMode_ = OM_REGULAR;
};



//
SgNetCdf::~SgNetCdf()
{
  identities_ = NULL;
  reset();
  // clear the map, it still contains "Unity"
  QMap<QString, SgNcdfDimension*>::iterator it_d=dimensionByName_.begin();
  for (; it_d!=dimensionByName_.end(); ++it_d)
    delete it_d.value();
  dimensionByName_.clear();
};



//
void SgNetCdf::reset()
{
  QMap<QString, SgNcdfDimension*>::iterator it_d=dimensionByName_.begin();
  for (; it_d!=dimensionByName_.end(); ++it_d)
    delete it_d.value();
  dimensionByName_.clear();
  dimensionByName_.insert(dUnity.getName(), new SgNcdfDimension(dUnity));

  //
  QMap<QString, SgNcdfVariable*>::iterator  it;
  for (it=serviceVars_.begin(); it!=serviceVars_.end(); ++it)
    delete it.value();
  for (it=contentVars_.begin(); it!=contentVars_.end(); ++it)
    delete it.value();
  serviceVars_.clear();
  contentVars_.clear();
  operationMode_ = OM_REGULAR;
};



//
bool SgNetCdf::getData()
{
  bool                          isOk(false);
  int                           ncid, rc;
  // checking:
  if (!fileName_.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getData(): the file name is not specified");
    return isOk;
  };
  if ((rc=nc_open(qPrintable(fileName_), 0, &ncid)) != NC_NOERR)
  {
    printf("%s: %s\n", qPrintable("Error opening " + fileName_), nc_strerror(rc));
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getData(): cannot open the file " + fileName_);
    return isOk;
  };
  //
  reset();
  //
  // general variables:
  size_t                                    len;
  nc_type                                   xtype;
  //
  // get dimensions:
  QMap<int, SgNcdfDimension*>               dimById;
  int                                       numOfDims;
  char                                      buff[NC_MAX_NAME + 1];
  if ((rc=nc_inq_ndims(ncid, &numOfDims)) != NC_NOERR)
  {
    printf("%s: %s\n", 
      qPrintable("Error inquiring number of dimensions from " + fileName_), nc_strerror(rc));
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getData(): inquire of total number of dimensions has failed");
    nc_close(ncid);
    return isOk;
  };
  for (int i=0; i<numOfDims; i++)
  {
    if ((rc=nc_inq_dim(ncid, i, buff, &len)) != NC_NOERR)
    {
      printf("%s: %s\n", 
        qPrintable("Error inquiring dimension name and length from " + fileName_), nc_strerror(rc));
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::getData(): inquire of dimension name and length for dim#" + QString("").setNum(i) + 
        " has failed");
      nc_close(ncid);
      return isOk;
    };
    dimById.insert(i, new SgNcdfDimension(QString(buff), len, i));
  };
  //
  //
  // get variables:
  QMap<int, SgNcdfVariable*>                varById;
  int                                       numOfVars;
  if ((rc=nc_inq_nvars(ncid, &numOfVars)) != NC_NOERR)
  {
    printf("%s: %s\n", 
      qPrintable("Error inquiring number of dimensions from " + fileName_), nc_strerror(rc));
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::getData(): inquire of total number of variables has failed");
    nc_close(ncid);
    return isOk;
  };
  for (int i=0; i<numOfVars; i++)
  {
    int                         dimids[NC_MAX_VAR_DIMS];
    int                         ndims, natts;
    SgNcdfVariable             *v=NULL;
    if ((rc=nc_inq_var(ncid, i, buff, &xtype, &ndims, dimids, &natts)) != NC_NOERR)
    {
      printf("%s: %s\n", 
        qPrintable("Error inquiring variable from " + fileName_), nc_strerror(rc));
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::getData(): inquire of variable for var#" + QString("").setNum(i) + 
        " has failed");
      nc_close(ncid);
      return isOk;
    };
    v = new SgNcdfVariable;
    v->setName(QString(buff));
    v->setTypeOfData(xtype);
    v->setId(i);
    // dimensions:
    for (int j=0; j<ndims; j++)
    {
      SgNcdfDimension        *d=dimById.value(dimids[j]);
      v->addDimension(*d);
    };
    // attributes:
    for (int j=0; j<natts; j++)
    {
      SgNcdfAttribute        *attr=new SgNcdfAttribute;
      if ((rc=nc_inq_attname(ncid, i, j, buff)) != NC_NOERR)
      {
        printf("%s: %s\n", 
          qPrintable("Error inquiring attribute name from " + fileName_), nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::getData(): inquire of name of attribute# "+ QString("").setNum(j) + " for variable #" + 
          QString("").setNum(i) + " has failed");
        nc_close(ncid);
        return isOk;
      };
      attr->setName(QString(buff));
      if ((nc_inq_att(ncid, i, buff, &xtype, &len)) != NC_NOERR)
      {
        printf("%s: %s\n", 
          qPrintable("Error inquiring attribute from " + fileName_), nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::getData(): inquire of attribute# "+ QString("").setNum(j) + " for variable #" + 
          QString("").setNum(i) + " has failed");
        nc_close(ncid);
        return isOk;
      };
      attr->setTypeOfData(xtype);
      attr->setNumOfElements(len);
      if ((rc=attr->nc_get_attr(ncid, i)) != NC_NOERR)
      {
        printf("%s: %s\n", 
          qPrintable("Error inquiring attribute name from " + fileName_), nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::getData(): inquire of name of attribute# "+ QString("").setNum(j) + " for variable #" + 
          QString("").setNum(i) + " has failed");
        nc_close(ncid);
        return isOk;
      };
      v->addAttribute(attr);
    };
    // read the data:
    if ((rc=v->nc_get_var(ncid)) != NC_NOERR)
    {
      printf("%s: %s\n", 
        qPrintable("Error reading variable from " + fileName_), nc_strerror(rc));
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::getData(): reading of data of variable# "+ QString("").setNum(i) + " has failed");
      nc_close(ncid);
      return isOk;
    };
    // check for "REPEAT" attribute:
    if (v->attributeByName().contains("REPEAT"))
    {
      SgNcdfAttribute          *att=v->attributeByName().value("REPEAT");
      if (att && att->getTypeOfData()==NC_INT && att->getNumOfElements()==1)
      {
        int                     mult=*(int*)att->getData();
        v->multiplyData(mult);
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::getData(): reading of data of variable# "+ QString("").setNum(i) + 
          ": has wrong attribute REPEAT");
    };
    varById.insert(i, v);
  };
  //
  //
  // dimensions:
  QMap<int, SgNcdfDimension*>::iterator it_d=dimById.begin();
  for(; it_d!=dimById.end(); ++it_d)
    dimensionByName_.insert(it_d.value()->getName(), it_d.value());
  dimById.clear();
  //
  // variables:
  const QString srvVarNames("Stub:CreateTime:CreatedBy:Program:Subroutine:DataOrigin:TimeTag:"
    "TimeTagFile:Session:Station:REPEAT:vgosDB_Version");
  QMap<int, SgNcdfVariable*>::iterator it_v=varById.begin();
  for(; it_v!=varById.end(); ++it_v)
  {
    SgNcdfVariable             *v=it_v.value();
    if (srvVarNames.contains(v->getName()))
      serviceVars_.insert(v->getName().toUpper(), v);
    else
      contentVars_.insert(v->getName().toUpper(), v);
  };
  varById.clear();

  logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::getData(): read " + QString("").setNum(dimensionByName_.size()) + " dimensions, " +
    QString("").setNum(serviceVars_.size()) + " service and " +
    QString("").setNum(contentVars_.size()) + " content variables");

  nc_close(ncid);
  isOk = true;
  return isOk;
};



//
bool SgNetCdf::setServiceVar(const QString& varName, const QString& varValue)
{
  QString                     str("");
  SgNcdfVariable             *var=new SgNcdfVariable;
  int                         len=varValue.size();
  var->setName(varName);
  var->setTypeOfData(NC_CHAR);
  var->setId(serviceVars_.size());
  str.sprintf("Char_x_%d", len);
  var->addDimension(str, len, -1);
  var->allocateData();
  strncpy(var->data2char(), qPrintable(varValue), len);
  registerVariable(var, true);
  return true;
};



//
void SgNetCdf::setServiceVars(const QString& stub, const QString& dataOrigin, const QString& timeTag,
    const QString& timeTagFile)
{
  QString                       str("");
  if (stub.size())
    setServiceVar(svcStub, stub);
  else
    std::cout << "  no stub!  \n";
  //
  setServiceVar(svcCreateTime, SgMJD::currentMJD().toUtc().toString(SgMJD::F_Simple) + " UTC");
  //
  if (identities_)
  {
    if (identities_->getUserName().size())
      str = identities_->getUserName();
    if (identities_->getAcFullName().size())
      str += ", " + identities_->getAcFullName();
    setServiceVar(svcCreatedBy, str);
    if (identities_->getDriverVersion().getSoftwareName().size())
      str = identities_->getDriverVersion().name() + " executed from ";
    else
      str = "";
    setServiceVar(svcProgram, str + identities_->getExecDir() + "/" + identities_->getExecBinaryName() +
      " on " + identities_->getMachineNodeName() + "[" + identities_->getMachineMachineName() + "] " +
      identities_->getMachineSysName() + " " + identities_->getMachineRelease());
    //
    setServiceVar(svcSubroutine, "Class " + className() + " of " + libraryVersion.name());
  };
  //
  if (fmtVerId_.size())
    setServiceVar(svcVgosDbVersion, fmtVerId_);
  //
  if (dataOrigin.size())
    setServiceVar(svcDataOrigin, dataOrigin);
  //
  if (timeTag.size())
    setServiceVar(svcTimeTag, timeTag);
  //
  if (timeTagFile.size())
    setServiceVar(svcTimeTagFile, timeTagFile);
  //
  if (sessionId_.size())
    setServiceVar(svcSession, sessionId_);
  //
  if (stationId_.size())
    setServiceVar(svcStation, stationId_);
  //
  if (bandId_.size())
    setServiceVar(svcBand, bandId_);
};



//
void SgNetCdf::registerVariable(SgNcdfVariable* var, bool is4Service)
{
  // check the input:
  if (!var)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::registerVariable(): the variable is NULL");
    return;
  };
  if (!var->dimensions().size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::registerVariable(): the variable \"" + var->getName() + "\" has no dimension");
    return;
  };
  QMap<QString, SgNcdfVariable*>  &vars=is4Service?serviceVars_:contentVars_;
  if (vars.contains(var->getName().toUpper()))
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::registerVariable(): the variable \"" + var->getName() + "\" already in the " +
      (is4Service?"service":"content") + " map");
  else
    vars.insert(var->getName().toUpper(), var);
//  contentVars_.insert(var->getName().toUpper(), var);
  for (int i=0; i<var->dimensions().size(); i++)
  {
    SgNcdfDimension            *d=var->dimensions().at(i);
    if (!dimensionByName_.contains(d->getName()))
      dimensionByName_.insert(d->getName(), new SgNcdfDimension(*d));
    else // check the dimension:
      if (dimensionByName_.value(d->getName())->getN() != d->getN())
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::registerVariable(): the dimension \"" + d->getName() + "\" has different meanings, " +
          QString("").setNum(dimensionByName_.value(d->getName())->getN()) + " and " +
          QString("").setNum(d->getN()));
  };
};



//
bool SgNetCdf::putData()
{
  if (operationMode_ == OM_REGULAR) // normal work:
  {
    int                           ncid, rc;
    bool                          isOk=false;
    // checking:
    if (!fileName_.size())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::putData(): the file name is not specified");
      return isOk;
    };
    if ((rc=nc_create(qPrintable(fileName_), 0, &ncid)) != NC_NOERR)
    {
      printf("NetCDF: %s\n", nc_strerror(rc));
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::putData(): cannot create the file " + fileName_ + ": " + QString(nc_strerror(rc)));
      return isOk;
    };
    //
    // check for REPEATable variables (it can change dimensions):
    for (QMap<QString, SgNcdfVariable*>::iterator it=contentVars_.begin(); it!=contentVars_.end(); ++it)
      it.value()->check4multiplication();
    //
    // tell netCDF about our dimensions (describe the dimensions):
    for (QMap<QString, SgNcdfDimension*>::iterator it=dimensionByName_.begin(); it!=dimensionByName_.end();
      ++it)
    {
      SgNcdfDimension            *dim=it.value();
      int                         dimId;
      if ((rc=nc_def_dim(ncid, qPrintable(dim->getName()), dim->getN(), &dimId)) != NC_NOERR)
      {
        printf("%s\n", nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::putData(): defining of a new dimension " + dim->getName() + " has been failed");
        nc_close(ncid);
        return isOk;
      };
      dim->setId(dimId);
    };
    //
    // describe the variables:
    QList<SgNcdfVariable*>        variables;
    QMap<int, SgNcdfVariable*>    varByOrder;
    //
    // sort the service variables by their ID:
    for (QMap<QString, SgNcdfVariable*>::iterator it=serviceVars_.begin(); it!=serviceVars_.end(); ++it)
      varByOrder.insert(it.value()->getId(), it.value());
    variables = varByOrder.values();
    varByOrder.clear();
    // sort the content variables by their sizes:
    for (QMap<QString, SgNcdfVariable*>::iterator it=contentVars_.begin(); it!=contentVars_.end(); ++it)
      varByOrder.insertMulti(it.value()->sizeOfData(), it.value());
  //    varByOrder.insert(it.value()->getId(), it.value());
    //
    variables << varByOrder.values();
    varByOrder.clear();
    for (int i=0; i<variables.size(); i++)
    {
      SgNcdfVariable             *var=variables.at(i);
      int                         nDims=var->dimensions().size();
      int                        *dimids=new int[nDims];
      int                         varId;
      for (int j=0; j<nDims; j++)
        dimids[j] = dimensionByName_[var->dimensions().at(j)->getName()]->getId();
      if ((rc=nc_def_var(ncid, qPrintable(var->getName()), var->getTypeOfData(), nDims, dimids, &varId))
          != NC_NOERR)
      {
        printf("%s\n", nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::putData(): defining of a new variable " + var->getName() + " has been failed");
        nc_close(ncid);
        delete[] dimids;
        return isOk;
      };
      delete[] dimids;
      var->setId(varId);
      for (QMap<QString, SgNcdfAttribute*>::const_iterator it=var->attributeByName().begin(); 
        it!=var->attributeByName().end(); ++it)
      {
        SgNcdfAttribute          *att=it.value();
        if ((rc=att->nc_put_attr(ncid, var->getId())) != NC_NOERR)
        {
          printf("%s: %s\n", 
            qPrintable("Error specifying an attribute " + att->getName() + " for " + var->getName()), 
            nc_strerror(rc));
          logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
            "::putData(): defining the attribute \"" + att->getName() + "\" for variable " + 
            var->getName() + " has been failed");
          nc_close(ncid);
          return isOk;
        };
      };
    };
    nc_enddef(ncid);
    //
    // end of definitions,
    // data output:
    //
    for (int i=0; i<variables.size(); i++)
    {
      SgNcdfVariable             *var=variables.at(i);
      if ((rc=var->nc_put_var(ncid)) != NC_NOERR)
      {
        printf("%s: %s\n", qPrintable("Error writting variable to " + fileName_), nc_strerror(rc));
        logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
          "::putData(): writting the  variable " + qPrintable(var->getName()) + " has failed");
        nc_close(ncid);
        return isOk;
      };
    };
    //
    nc_close(ncid);
    //
    // adjust permissions:
    QFile                       f(fileName_);
    QFile::Permissions          perm=f.permissions();
    if (!f.setPermissions(perm | QFile::WriteGroup))
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::putData(): cannot adjust the permissions of the file \"" + f.fileName() + "\"");
    //
    isOk = true;
    return isOk;
  }
  else  // dry run:
  {
    if (!fileName_.size())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::putData(): the file name is not specified");
      return false;
    };

    std::cout << "DRY RUN: The netCDF file will be created: \""
              << qPrintable(fileName_) << "\"\n";
    return true;
  };
};
/*=====================================================================================================*/


