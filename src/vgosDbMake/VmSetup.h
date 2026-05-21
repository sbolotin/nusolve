/*
 *
 *    This file is a part of vgosDbMake. vgosDbMake is a part of CALC/SOLVE
 *    system and is designed to convert correlator output data into VgosDb
 *    format.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
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

#ifndef VM_SETUP_H
#define VM_SETUP_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgIdentities.h>


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class VmSetup
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VmSetup();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VmSetup();



  //
  // Interfaces:
  //
  // gets:
  /**Returns a path to nuSolve's home.
   */
  inline const QString& getPath2Home() const;

  /**Returns a path to vgosDb files (observations).
   */
  inline const QString& getPath2VgosDbFiles() const;

  /**Returns a default path to input files.
   */
  inline const QString& getPath2InputFiles() const;

  /**Returns a path to masterfiles.
   */
  inline const QString& getPath2MasterFiles() const;

  /**Returns a path to auxiliary logs.
   */
  inline const QString& getPath2AuxLogs() const;

  /**Returns true if the software have to save an auxiliary log.
   */
  inline bool getHave2SavePerSessionLog() const;

  /**Returns true if the software should not alter locale set up.
   */
  inline bool getUseLocalLocale() const {return useLocalLocale_;};

  /**Returns identities of user, analysis center, computer, etc.
   */
  inline const SgIdentities& getIdentities() const;

  /**Returns a reference on identities.
   */
  inline SgIdentities& identities();

  inline const QStringList& getMasterfileSuffixes() const {return masterfileSuffixes_;};
  inline bool getUseAltMasterfileSuffixes() {return useAltMasterfileSuffixes_;};

  

  //
  // sets:
  /**Sets up a path to nuSolve's home.
   */
  inline void setPath2Home(const QString&);

  /**Sets up a path to vgosDb files (observations).
   */
  inline void setPath2VgosDbFiles(const QString&);

  /**Sets up a path to input files.
   */
  inline void setPath2InputFiles(const QString&);

  /**Sets up a path to masterfiles.
   */
  inline void setPath2MasterFiles(const QString&);

  /**Sets up a path to auxiliary logs.
   */
  inline void setPath2AuxLogs(const QString&);

  /**Sets up an option to save an auxiliary log.
   */
  inline void setHave2SavePerSessionLog(bool);

  inline void setUseLocalLocale(bool use) {useLocalLocale_ = use;};

  /**Sets up identities of user, analysis center, computer, etc.
   */
  inline void setIdentities(const SgIdentities&);

  inline void setMasterfileSuffixes(const QStringList& lst) {masterfileSuffixes_ = lst;};
  inline void setUseAltMasterfileSuffixes(bool use) {useAltMasterfileSuffixes_ = use;};


  //
  // Functions:
  //
  inline QString className() const {return "VmSetup";};
  
  void setUpBinaryIdentities(const QString&);

  inline QString path2(const QString&) const;


  void print2stdOut();

  //
  // Friends:
  //



  //
  // I/O:
  //
  //


private:
  // pathes:
  QString                       path2Home_;
  QString                       path2VgosDbFiles_;
  QString                       path2InputFiles_;
  QString                       path2MasterFiles_;
  QString                       path2AuxLogs_;
  bool                          have2SavePerSessionLog_;
  bool                          useLocalLocale_;

  // identities:
  SgIdentities                  identities_;
  //
  QStringList                   masterfileSuffixes_;
  bool                          useAltMasterfileSuffixes_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* VmSetup inline members:                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// INTERFACES:
//
//
inline const QString& VmSetup::getPath2Home() const
{
  return path2Home_;
};



//
inline const QString& VmSetup::getPath2VgosDbFiles() const
{
  return path2VgosDbFiles_;
};



//
inline const QString& VmSetup::getPath2InputFiles() const
{
  return path2InputFiles_;
};



//
inline const QString& VmSetup::getPath2MasterFiles() const
{
  return path2MasterFiles_;
};



//
inline const QString& VmSetup::getPath2AuxLogs() const
{
  return path2AuxLogs_;
};



//
inline bool VmSetup::getHave2SavePerSessionLog() const
{
  return have2SavePerSessionLog_;
};



//
inline const SgIdentities& VmSetup::getIdentities() const
{
  return identities_;
};



//
inline SgIdentities& VmSetup::identities()
{
  return identities_;
};



//
// sets:
//
inline void VmSetup::setPath2Home(const QString& str)
{
  path2Home_ = str;
};



//
inline void VmSetup::setPath2VgosDbFiles(const QString& str)
{
  path2VgosDbFiles_ = str;
};



//
inline void VmSetup::setPath2InputFiles(const QString& str)
{
  path2InputFiles_ = str;
};



//
inline void VmSetup::setPath2MasterFiles(const QString& str)
{
  path2MasterFiles_ = str;
};



//
inline void VmSetup::setPath2AuxLogs(const QString& str)
{
  path2AuxLogs_ = str;
};



//
inline void VmSetup::setHave2SavePerSessionLog(bool have2)
{
  have2SavePerSessionLog_ = have2;
}



//
inline void VmSetup::setIdentities(const SgIdentities& id)
{
  identities_ = id;
};



//
inline QString VmSetup::path2(const QString& path) const
{
  return 
    path.size()==0 ? path2Home_ :
      (path.at(0)==QChar('/') ? path : path2Home_ + "/" + path);
};
/*=====================================================================================================*/
#endif // VM_SETUP_H
