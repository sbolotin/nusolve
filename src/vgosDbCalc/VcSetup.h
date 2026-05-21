/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#ifndef VC_SETUP_H
#define VC_SETUP_H


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
class VcSetup
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VcSetup();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VcSetup();



  //
  // Interfaces:
  //
  // gets:
  /**Returns a path to vgosDbCalc's home.
   */
  inline const QString& getPath2Home() const;

  /**Returns a path to vgosDb files (observations).
   */
  inline const QString& getPath2VgosDbFiles() const;

  /**Returns a path to session files (logs).
   */
  inline const QString& getPath2CalcAprioriFiles() const;

  /**Returns a path to masterfiles.
   */
  inline const QString& getPath2MasterFiles() const;

  inline const QString& getAprioriFile4Sites() const {return aPrioriFile4Sites_;};
  inline const QString& getAprioriFile4Sources() const {return aPrioriFile4Sources_;};
  inline const QString& getAprioriFile4OceanLd() const {return aPrioriFile4OceanLd_;};
  inline const QString& getAprioriFile4Eops() const {return aPrioriFile4Eops_;};
  inline const QString& getAprioriFile4Tilts() const {return aPrioriFile4Tilts_;};
  inline const QString& getAprioriFile4OceanPoleTideLd() const {return aPrioriFile4OceanPoleTideLd_;};

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
  inline bool getUseOwnOnlyDatabase() {return useOwnOnlyDatabase_;};

  

  //
  // sets:
  /**Sets up a path to vgosDbCalc's home.
   */
  inline void setPath2Home(const QString&);

  /**Sets up a path to vgosDb files (observations).
   */
  inline void setPath2VgosDbFiles(const QString&);

  /**Sets up a path to session files.
   */
  inline void setPath2CalcAprioriFiles(const QString&);

  /**Sets up a path to masterfiles.
   */
  inline void setPath2MasterFiles(const QString&);

  inline void setAprioriFile4Sites(const QString& file) {aPrioriFile4Sites_ = file;};
  inline void setAprioriFile4Sources(const QString& file) {aPrioriFile4Sources_ = file;};
  inline void setAprioriFile4OceanLd(const QString& file) {aPrioriFile4OceanLd_ = file;};
  inline void setAprioriFile4Eops(const QString& file) {aPrioriFile4Eops_ = file;};
  inline void setAprioriFile4Tilts(const QString& file) {aPrioriFile4Tilts_ = file;};
  inline void setAprioriFile4OceanPoleTideLd(const QString& file) {aPrioriFile4OceanPoleTideLd_ = file;};

  /**Sets up a path to auxiliary logs.
   */
  inline void setPath2AuxLogs(const QString&);

  inline void setUseLocalLocale(bool use) {useLocalLocale_ = use;};

  /**Sets up an option to save an auxiliary log.
   */
  inline void setHave2SavePerSessionLog(bool);

  /**Sets up identities of user, analysis center, computer, etc.
   */
  inline void setIdentities(const SgIdentities&);

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
  QString                       path2CalcAprioriFiles_;
  QString                       aPrioriFile4Sites_;
  QString                       aPrioriFile4Sources_;
  QString                       aPrioriFile4OceanLd_;
  QString                       aPrioriFile4Eops_;
  QString                       aPrioriFile4Tilts_;
  QString                       aPrioriFile4OceanPoleTideLd_;
  QString                       path2AuxLogs_;
  bool                          have2SavePerSessionLog_;
  bool                          useLocalLocale_;
  // identities:
  SgIdentities                  identities_;

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
inline const QString& VcSetup::getPath2Home() const
{
  return path2Home_;
};



//
inline const QString& VcSetup::getPath2VgosDbFiles() const
{
  return path2VgosDbFiles_;
};



//
inline const QString& VcSetup::getPath2CalcAprioriFiles() const
{
  return path2CalcAprioriFiles_;
};



//
inline const QString& VcSetup::getPath2MasterFiles() const
{
  return path2MasterFiles_;
};



//
inline const QString& VcSetup::getPath2AuxLogs() const
{
  return path2AuxLogs_;
};



//
inline bool VcSetup::getHave2SavePerSessionLog() const
{
  return have2SavePerSessionLog_;
};



//
inline const SgIdentities& VcSetup::getIdentities() const
{
  return identities_;
};



//
inline SgIdentities& VcSetup::identities()
{
  return identities_;
};



//
// sets:
//
inline void VcSetup::setPath2Home(const QString& str)
{
  path2Home_ = str;
};



//
inline void VcSetup::setPath2VgosDbFiles(const QString& str)
{
  path2VgosDbFiles_ = str;
};



//
inline void VcSetup::setPath2CalcAprioriFiles(const QString& str)
{
  path2CalcAprioriFiles_ = str;
};



//
inline void VcSetup::setPath2MasterFiles(const QString& str)
{
  path2MasterFiles_ = str;
};



//
inline void VcSetup::setPath2AuxLogs(const QString& str)
{
  path2AuxLogs_ = str;
};



//
inline void VcSetup::setHave2SavePerSessionLog(bool have2)
{
  have2SavePerSessionLog_ = have2;
};



//
inline void VcSetup::setIdentities(const SgIdentities& id)
{
  identities_ = id;
};



//
inline QString VcSetup::path2(const QString& path) const
{
  return 
    path.size()==0 ? path2Home_ :
      (path.at(0)==QChar('/') ? path : path2Home_ + "/" + path);
};
/*=====================================================================================================*/
#endif // VC_SETUP_H
