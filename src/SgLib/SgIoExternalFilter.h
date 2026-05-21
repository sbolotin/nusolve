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

#ifndef SG_IO_EXTERNAL_FILTER
#define SG_IO_EXTERNAL_FILTER


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <QtCore/QFile>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QTextStream>


//#include <SgConstants.h>
#include <SgLogger.h>
#include <SgMJD.h>




/*=====================================================================================================*/




/*=====================================================================================================*/

enum FilterDirection
{
  FLTD_Udefined   =   0,         //
  FLTD_Input      =   1,         //
  FLTD_Output     =   2,         // 
};




/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
//
class SgIoExternalFilter
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  inline SgIoExternalFilter() : name_("none"), defaultExtension_(""), 
    command2read_(""), command2write_("") 
    {isOk_ = true;};

  inline SgIoExternalFilter(const QString& name, const QString& defaultExtension, 
    const QString& command2read, const QString& command2write) : 
    name_(name), defaultExtension_(defaultExtension), 
    command2read_(command2read), command2write_(command2write)
    {isOk_ = selfCheck();};

  inline SgIoExternalFilter(const SgIoExternalFilter& ef) :
    name_(ef.name_), defaultExtension_(ef.defaultExtension_), 
    command2read_(ef.command2read_), command2write_(ef.command2write_)
    {isOk_ = ef.isOk_;};

  /**A destructor.
   * Destroys the object.
   */
  inline ~SgIoExternalFilter() {};


  //
  // Interfaces:
  //
  // gets:
  /**
   */
  inline const QString& getName() const {return name_;};
  inline const QString& getDefaultExtension() const {return defaultExtension_;};
  inline const QString& getCommand2read() const {return command2read_;};
  inline const QString& getCommand2write() const {return command2write_;};
  inline bool isOk() const {return isOk_;};

  // sets:
  inline void setName(const QString& name) {name_ = name;};
  inline void setDefaultExtension(const QString& defaultExtension) {defaultExtension_ = defaultExtension;};
  inline void setCommand2read(const QString& command2read) {command2read_ = command2read;};
  inline void setCommand2write(const QString& command2write) {command2write_ = command2write;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool selfCheck();
  FILE* openFlt(const QString& fileName, QFile& file, QTextStream& ts, FilterDirection) const;
  static void closeFlt(FILE*& p, QFile& file, QTextStream& ts);

  //
  // Friends:
  //
  
protected:
  QString                       name_;
  QString                       defaultExtension_;
  QString                       command2read_;
  QString                       command2write_;
  bool                          isOk_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgIoExtFilterHandler
{
public:
  //
  // Constructors/destructors:
  //
  /**A constructor.
   * Creates a default copy of the object.
   */
  SgIoExtFilterHandler();


  inline SgIoExtFilterHandler(const SgIoExtFilterHandler& handler) : filterByExt_(handler.filterByExt_)
    {};

  /**A destructor.
   * Destroys the object.
   */
  ~SgIoExtFilterHandler();



  inline SgIoExtFilterHandler& operator=(const SgIoExtFilterHandler& handler)
    {filterByExt_ = handler.filterByExt_; return *this;};


  //
  // Interfaces:
  //
  // gets:
  /**
   */
  inline const QMap<QString, const SgIoExternalFilter*>& getFilterByExt() const {return filterByExt_;};

  // sets:
  inline void setFilterByExt(const QMap<QString, const SgIoExternalFilter*>& filterByExt)
    {filterByExt_ = filterByExt;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool addFilter(const SgIoExternalFilter* filter);

  inline bool addFilter(const QString& name, const QString& defaultExtension, 
    const QString& command2read, const QString& command2write)
    {return addFilter(new SgIoExternalFilter(name, defaultExtension, command2read, command2write));};

  bool removeFilter(const SgIoExternalFilter* filter);

  bool removeFilter(const QString& extension);

  const SgIoExternalFilter* lookupFilterByFileName(const QString& fileName);

  FILE* openFlt(const QString& fileName, QFile& file, QTextStream& ts, FilterDirection);

  inline static void closeFlt(FILE*& p, QFile& file, QTextStream& ts) 
    {SgIoExternalFilter::closeFlt(p, file, ts);};

  //
  // Friends:
  //
  
protected:
  QMap<QString, const SgIoExternalFilter*>
                                  filterByExt_;
//  SgIoExternalFilter           *defaultFilter_;
};













/*=====================================================================================================*/








/*=====================================================================================================*/
//
// constants:
//

extern SgIoExtFilterHandler compressors;

/*=====================================================================================================*/


/*=====================================================================================================*/
#endif     //SG_IO_EXTERNAL_FILTER

