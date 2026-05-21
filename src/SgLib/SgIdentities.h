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

#ifndef SG_IDENTITIES
#define SG_IDENTITIES


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#include <SgVersion.h>



/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgIdentities
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**A constructor.
   * Creates an object.
   */
  SgIdentities();

  /**A constructor.
   * Creates a copy of the object.
   */
  SgIdentities(const SgIdentities&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgIdentities();


  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  SgIdentities& operator=(const SgIdentities&);
  
  // gets:
  /**Returns a name of an user.
   */
  inline const QString& getUserName() const;
  
  /**Returns a user's e-mail address.
   */
  inline const QString& getUserEmailAddress() const;
  
  /**Returns user's initials.
   */
  inline const QString& getUserDefaultInitials() const;
  
  /**Returns a name of an Analysis Center.
   */
  inline const QString& getAcFullName() const;
  
  /**Returns a standard-abbreviated name of an Analysis Center.
   */
  inline const QString& getAcAbbrevName() const;
  
  /**Returns a 3-chars abbreviation of an Analysis Center's name.
   */
  inline const QString& getAcAbbName() const;
  
  inline const QString& getExecDir() const;
  
  inline const QString& getCurrentDir() const;

  inline const QString& getExecBinaryName() const;
  
  inline const SgVersion& getDriverVersion() const;
  
  inline const QString& getUserCommand() const;

  inline const QString& getMachineNodeName() const;
  
  inline const QString& getMachineMachineName() const;
  
  inline const QString& getMachineSysName() const;
  
  inline const QString& getMachineRelease() const;

//  inline const QString& getAltConfigName() const;



  
  // sets:
  /**Sets up a user's e-mail address.
   */
  inline void setUserName(const QString&);
  
  /**Sets up user's initials.
   */
  inline void setUserEmailAddress(const QString&);
  
  /**Sets up a name of an Analysis Center.
   */
  inline void setUserDefaultInitials(const QString&);
  
  /**Sets up a name of an Analysis Center.
   */
  inline void setAcFullName(const QString&);

  /**Sets up a standard-abbreviated name of an Analysis Center.
   */
  inline void setAcAbbrevName(const QString&);
  
  /**Sets up a 3-chars abbreviation of an Analysis Center's name.
   */
  inline void setAcAbbName(const QString&);
  
  inline void setExecDir(const QString&);
  
  inline void setCurrentDir(const QString&);
  
  inline void setExecBinaryName(const QString&);
  
  void setDriverVersion(const SgVersion&);
  
  inline void setUserCommand(const QString&);

  inline void setMachineNodeName(const QString&);
  
  inline void setMachineMachineName(const QString&);
  
  inline void setMachineSysName(const QString&);
  
  inline void setMachineRelease(const QString&);
  
//  inline void setAltConfigName(const QString&);
  
  
  // functions:
  void print2stdOut();


private:
  // user identities:
  QString                 userName_;
  QString                 userEmailAddress_;
  QString                 userDefaultInitials_;
  // analysis center identities:
  QString                 acFullName_;
  QString                 acAbbrevName_;            // standard abbreviation
  QString                 acAbbName_;               // 3-chars abbreviation
  // executables identities:
  QString                 execDir_;
  QString                 currentDir_;
  QString                 execBinaryName_;
  SgVersion               driverVersion_;
  QString                 userCommand_;
  // computer's identities:
  QString                 machineNodeName_;
  QString                 machineMachineName_;
  QString                 machineSysName_;
  QString                 machineRelease_;
  // aux:
//  QString                 altConfigName_;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgIdentities inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
// A destructor:
inline SgIdentities::~SgIdentities()
{
};



//
// INTERFACE:
//
//
//
inline const QString& SgIdentities::getUserName() const
{
  return userName_;
};



//
inline const QString& SgIdentities::getUserEmailAddress() const
{
  return userEmailAddress_;
};



//
inline const QString& SgIdentities::getUserDefaultInitials() const
{
  return userDefaultInitials_;
};



//
inline const QString& SgIdentities::getAcFullName() const
{
  return acFullName_;
};



//
inline const QString& SgIdentities::getAcAbbrevName() const
{
  return acAbbrevName_;
};



//
inline const QString& SgIdentities::getAcAbbName() const
{
  return acAbbName_;
};



//
inline const QString& SgIdentities::getExecDir() const
{
  return execDir_;
};



//
inline const QString& SgIdentities::getCurrentDir() const
{
  return currentDir_;
};



//
inline const QString& SgIdentities::getExecBinaryName() const
{
  return execBinaryName_;
};



//
inline const SgVersion& SgIdentities::getDriverVersion() const
{
  return driverVersion_;
};



//
inline const QString& SgIdentities::getUserCommand() const
{
  return userCommand_;
};



//
inline const QString& SgIdentities::getMachineNodeName() const
{
  return machineNodeName_;
};



//
inline const QString& SgIdentities::getMachineMachineName() const
{
  return machineMachineName_;
};



//
inline const QString& SgIdentities::getMachineSysName() const
{
  return machineSysName_;
};



//
inline const QString& SgIdentities::getMachineRelease() const
{
  return machineRelease_;
};



//
inline void SgIdentities::setUserName(const QString& str)
{
  userName_ = str;
};



//
inline void SgIdentities::setUserEmailAddress(const QString& str)
{
  userEmailAddress_ = str;
};



//
inline void SgIdentities::setUserDefaultInitials(const QString& str)
{
  userDefaultInitials_ = str;
};



//
inline void SgIdentities::setAcFullName(const QString& str)
{
  acFullName_ = str;
};



//
inline void SgIdentities::setAcAbbrevName(const QString& str)
{
  acAbbrevName_ = str;
};



//
inline void SgIdentities::setAcAbbName(const QString& str)
{
  acAbbName_ = str;
};



//
inline void SgIdentities::setExecDir(const QString& str)
{
  execDir_ = str;
};



//
inline void SgIdentities::setCurrentDir(const QString& str)
{
  currentDir_ = str;
};



//
inline void SgIdentities::setExecBinaryName(const QString& str)
{
  execBinaryName_ = str;
};



//
inline void SgIdentities::setUserCommand(const QString& str)
{
  userCommand_ = str;
};



//
inline void SgIdentities::setMachineNodeName(const QString& str)
{
  machineNodeName_ = str;
};



//
inline void SgIdentities::setMachineMachineName(const QString& str)
{
  machineMachineName_ = str;
};



//
inline void SgIdentities::setMachineSysName(const QString& str)
{
  machineSysName_ = str;
};



//
inline void SgIdentities::setMachineRelease(const QString& str)
{
  machineRelease_ = str;
};




//
//inline void SgIdentities::setAltConfigName(const QString& str)
//{
//  altConfigName_ = str;
//};




//
// FRUNCTIONS:
//
//
//



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
#endif //SG_IDENTITIES
