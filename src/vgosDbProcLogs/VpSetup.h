/*
 *    This file is a part of vgosDbProcLogs. vgosDbProcLogs is a part of
 *    CALC/SOLVE system and is designed to extract data, meteo parameters
 *    and cable calibrations, from stations log files and store them in
 *    the vgosDb format.
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

#ifndef VP_SETUP_H
#define VP_SETUP_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>


#include <SgIdentities.h>


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class VpSetup
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VpSetup();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VpSetup();



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

  /**Returns a path to session files (logs).
   */
  inline const QString& getPath2SessionFiles() const;

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

  inline const QMap<QString, int>& getDefaultCableSignByStn() const {return defaultCableSignByStn_;};

  inline const QMap<QString, QString>& getRinexFileNameByStn() const {return rinexFileNameByStn_;};

  inline const QMap<QString, double>& getRinexPressureOffsetByStn() const 
    {return rinexPressureOffsetByStn_;};

  inline const QStringList& getMasterfileSuffixes() const {return masterfileSuffixes_;};
  inline bool getUseAltMasterfileSuffixes() {return useAltMasterfileSuffixes_;};
  inline bool getUseOwnOnlyDatabase() {return useOwnOnlyDatabase_;};


  //
  // sets:
  /**Sets up a path to nuSolve's home.
   */
  inline void setPath2Home(const QString&);

  /**Sets up a path to vgosDb files (observations).
   */
  inline void setPath2VgosDbFiles(const QString&);

  /**Sets up a path to session files.
   */
  inline void setPath2SessionFiles(const QString&);

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

  inline QMap<QString, int>& defaultCableSignByStn() {return defaultCableSignByStn_;};

  inline QMap<QString, QString>& rinexFileNameByStn() {return rinexFileNameByStn_;};

  inline QMap<QString, double>& rinexPressureOffsetByStn() {return rinexPressureOffsetByStn_;};

  inline void setMasterfileSuffixes(const QStringList& lst) {masterfileSuffixes_ = lst;};
  inline void setUseAltMasterfileSuffixes(bool use) {useAltMasterfileSuffixes_ = use;};
  inline void setUseOwnOnlyDatabase(bool use) {useOwnOnlyDatabase_ = use;};


  //
  // Functions:
  //
  inline QString className() const {return "VpSetup";};
  
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
  QString                       path2MasterFiles_;
  QString                       path2SessionFiles_;
  QString                       path2AuxLogs_;
  bool                          have2SavePerSessionLog_;
  bool                          useLocalLocale_;

  // identities:
  SgIdentities                  identities_;
  
  // default cable signs:
  QMap<QString, int>            defaultCableSignByStn_;
  // RINEX files: GPS station names:
  QMap<QString, QString>        rinexFileNameByStn_;
  // RINEX files: pressure corrections:
  QMap<QString, double>         rinexPressureOffsetByStn_;
  //
  QStringList                   masterfileSuffixes_;
  bool                          useAltMasterfileSuffixes_;
  bool                          useOwnOnlyDatabase_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* VpSetup inline members:                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// INTERFACES:
//
//
inline const QString& VpSetup::getPath2Home() const
{
  return path2Home_;
};



//
inline const QString& VpSetup::getPath2VgosDbFiles() const
{
  return path2VgosDbFiles_;
};



//
inline const QString& VpSetup::getPath2SessionFiles() const
{
  return path2SessionFiles_;
};



//
inline const QString& VpSetup::getPath2MasterFiles() const
{
  return path2MasterFiles_;
};



//
inline const QString& VpSetup::getPath2AuxLogs() const
{
  return path2AuxLogs_;
};



//
inline bool VpSetup::getHave2SavePerSessionLog() const
{
  return have2SavePerSessionLog_;
};



//
inline const SgIdentities& VpSetup::getIdentities() const
{
  return identities_;
};



//
inline SgIdentities& VpSetup::identities()
{
  return identities_;
};



//
// sets:
//
inline void VpSetup::setPath2Home(const QString& str)
{
  path2Home_ = str;
};



//
inline void VpSetup::setPath2VgosDbFiles(const QString& str)
{
  path2VgosDbFiles_ = str;
};



//
inline void VpSetup::setPath2SessionFiles(const QString& str)
{
  path2SessionFiles_ = str;
};



//
inline void VpSetup::setPath2MasterFiles(const QString& str)
{
  path2MasterFiles_ = str;
};



//
inline void VpSetup::setPath2AuxLogs(const QString& str)
{
  path2AuxLogs_ = str;
};



//
inline void VpSetup::setHave2SavePerSessionLog(bool have2)
{
  have2SavePerSessionLog_ = have2;
};



//
inline void VpSetup::setIdentities(const SgIdentities& id)
{
  identities_ = id;
};



//
inline QString VpSetup::path2(const QString& path) const
{
  return 
    path.size()==0 ? path2Home_ :
      (path.at(0)==QChar('/') ? path : path2Home_ + "/" + path);
};
/*=====================================================================================================*/
#endif // VP_SETUP_H
