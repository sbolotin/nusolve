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

#include <iostream>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>


#include <SgIdentities.h>


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgIdentities::className()
{
  return "SgIdentities";
};



//
//
// CONSTRUCTORS:
//
// An empty constructor:
SgIdentities::SgIdentities() :
  // identities (defaults should be obviously fictional):
  userName_           ("Luavrik L. Luavrik"),
  userEmailAddress_   ("luavrik@localhost"),
  userDefaultInitials_("LL"),
  acFullName_         ("Space Research Center, National Academy of Science of Irukan"),
  acAbbrevName_       ("SRC NASI"),
  acAbbName_          ("SRI"),
  execDir_            ("./"),
  currentDir_         ("./"),
  execBinaryName_     ("a.out"),
  driverVersion_      (),
  userCommand_        (""),
  machineNodeName_    (""),
  machineMachineName_ (""),
  machineSysName_     (""),
  machineRelease_     ("")
//  altConfigName_      ("")
{
  char                          buff[256];
  QString                       hostName("localhost");
  struct utsname                uts;
  struct passwd                *pw=NULL;
  //
  // guess OS name:
  if (uname(&uts) == -1)
    perror("uname: ");
  else
  {
    machineNodeName_ = QString(uts.nodename);
    machineMachineName_ = QString(uts.machine);
    machineSysName_ = QString(uts.sysname);
    machineRelease_ = QString(uts.release);
  };
  //
  // guess host name:
  if (gethostname(buff, sizeof(buff)) == -1)
    perror("gethostname: ");
  else
    hostName = QString(buff);
  // If hostname contain at least one dot, assume this is F.Q.D.N. host name
  if (!hostName.contains("."))
  {
    if (getdomainname(buff, sizeof(buff))==-1)
      perror("getdomainname: ");
    else if (!strstr(buff, "(none)"))
      hostName += "." + QString(buff);
  };
  //
  //
  // guess login name:
  pw = getpwuid(geteuid());
  if (!pw)
    perror("getpwuid: ");
  else
  {
    userEmailAddress_ = QString(pw->pw_name) + "@" + hostName;
    // guess user name:
    if (!strtok(pw->pw_gecos, ","))
      userName_ = QString(pw->pw_gecos);
    else
      userName_ = QString(strtok(pw->pw_gecos, ","));
  };
};



// A copying constructor:
SgIdentities::SgIdentities(const SgIdentities& id) :
  userName_(id.getUserName()),
  userEmailAddress_(id.getUserEmailAddress()),
  userDefaultInitials_(id.getUserDefaultInitials()),
  acFullName_(id.getAcFullName()),
  acAbbrevName_(id.getAcAbbrevName()),
  acAbbName_(id.getAcAbbName()),
  execDir_(id.getExecDir()),
  currentDir_(id.getCurrentDir()),
  execBinaryName_(id.getExecBinaryName()),
  driverVersion_(id.getDriverVersion()),
  userCommand_(id.getUserCommand()),
  machineNodeName_(id.getMachineNodeName()),
  machineMachineName_(id.getMachineMachineName()),
  machineSysName_(id.getMachineSysName()),
  machineRelease_(id.getMachineRelease())
{
};



//
SgIdentities& SgIdentities::operator=(const SgIdentities& id)
{
  setUserName(id.getUserName());
  setUserEmailAddress(id.getUserEmailAddress());
  setUserDefaultInitials(id.getUserDefaultInitials());
  setAcFullName(id.getAcFullName());
  setAcAbbrevName(id.getAcAbbrevName());
  setAcAbbName(id.getAcAbbName());
  setExecDir(id.getExecDir());
  setCurrentDir(id.getCurrentDir());
  setExecBinaryName(id.getExecBinaryName());
  setDriverVersion(id.getDriverVersion());
  setUserCommand(id.getUserCommand());
  setMachineNodeName(id.getMachineNodeName());
  setMachineMachineName(id.getMachineMachineName());
  setMachineSysName(id.getMachineSysName());
  setMachineRelease(id.getMachineRelease());
  return *this;
};



//
void SgIdentities::setDriverVersion(const SgVersion& v)
{
  driverVersion_.setSoftwareName(v.getSoftwareName());
  driverVersion_.setMajorNumber (v.getMajorNumber());
  driverVersion_.setMinorNumber (v.getMinorNumber());
  driverVersion_.setTeenyNumber (v.getTeenyNumber());
  driverVersion_.setCodeName    (v.getCodeName());
  driverVersion_.setReleaseEpoch(v.getReleaseEpoch());
};



//
void SgIdentities::print2stdOut()
{
  std::cout << "User/Analysis Center Identities:\n";

  std::cout 
    << "User:\n"
    << "   Name:\t\t\t\t\"" << qPrintable(userName_) << "\"\n"
    << "   E-mail adderess:\t\t\t\"" << qPrintable(userEmailAddress_) << "\"\n"
    << "   Initials:\t\t\t\t\"" << qPrintable(userDefaultInitials_) << "\"\n"

    << "Analysis Center:\n"
    << "   Name:\t\t\t\t\"" << qPrintable(acFullName_) << "\"\n"
    << "   Abbreviation:\t\t\t\"" << qPrintable(acAbbrevName_) << "\"\n"
    << "   Abbreviation, short version:\t\t\"" << qPrintable(acAbbName_) << "\"\n"
    ;
};

/*=====================================================================================================*/






/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//

/*=====================================================================================================*/
//
// aux functions:
//

/*=====================================================================================================*/
//
// constants:
//

/*=====================================================================================================*/
