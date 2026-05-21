/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
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

#include <iostream>
#include <stdlib.h>



#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>
#include <QtCore/QVector>


#include <SgVlbiSession.h>

#include <SgConstants.h>
#include <SgLogger.h>
#include <SgVersion.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>
#include <SgRegularExpressions.h>






#ifdef HAVE_MK4_DATA_H
extern "C"
{
// workaround "complex" stuff:
  #define T230_VERSION 0
  #include <hops/mk4_data.h>
}
#endif
//
//
#ifdef HAVE_MK4_DFIO_H
extern "C"
{
  #include <hops/mk4_dfio.h>
}
#endif
//
//
#ifdef HAVE_MK4_VEX_H
// workaround warnings about redefinitions of TRUE and FALSE (Qt vs HOPS):
# ifdef FALSE
#   define FALSE_SAVED FALSE
#   undef FALSE
# endif
# ifdef TRUE
#   define TRUE_SAVED TRUE
#   undef TRUE
# endif
extern "C"
{
    #include <hops/mk4_vex.h>
}
# ifdef FALSE_SAVED
#   undef FALSE
#   define FALSE FALSE_SAVED
#   undef FALSE_SAVED
# endif
# ifdef TRUE_SAVED
#   undef TRUE
#   define TRUE TRUE_SAVED
#   undef TRUE_SAVED
# endif
#endif





/*=======================================================================================================
*
*                     Auxiliary data structures
* 
*======================================================================================================*/




/*=======================================================================================================
*
*                           I/O utilities:
* 
*======================================================================================================*/
//
#if !defined HAVE_MK4_DATA_H || !defined HAVE_MK4_DFIO_H || !defined HAVE_MK4_VEX_H
bool SgVlbiSession::getDataFromFringeFiles(const QString&, const QString&, const QString&,
  const QString&, const QString&, const QList<QString>&, const QStringList&, bool)
{
  std::cout << "HOPS were not found.\n"; 
  logger->write(SgLogger::ERR, SgLogger::IO, className() +
    "::getDataFromFringeFiles(): cannot read fringe files, the software was compiled without HOPS "
    "support");
  return false;
};



//
void SgVlbiSession::parseVexFile(const QString&, QString&, QString&)
{
};



//
void SgVlbiSession::processVexFile(const QString&, QMap<QString, StationInfo>&, VexInfo&,
  const QMap<QString, QString>&, QString&)
{
};



//
void SgVlbiSession::processFringeFile(const QString&, const QString&, const QMap<QString, StationInfo>&, 
  const VexInfo&, const QString&, const QMap<QString, QString>&, const QMap<QString, QString>&,
  const QMap<QString, QString>&, const QMap<QString, QString>&, const QMap<QString, QString>&,
  const QMap<QString, int>&, int&)
{
};



//
bool SgVlbiSession::getCorrelatorHistory(const QString&)
{
  return false;
};
//
//
//
//
#else
//
//
//
//
// auxiliary data structures (we do not need them outside of this file):
struct ChannelInfo
{
  QString                       name_;                /* External channel name */
  QString                       polarization_;        /* R or L */
  double                        sky_frequency_;       /* Hz */
  QString                       net_sideband_;        /* U or L */
  double                        bandwidth_;           /* Hz */
  QString                       band_id_;             /* Linkword (internal use) */
  QString                       chan_id_;             /* Linkword (internal use) */
  QString                       bbc_id_;              /* Linkword (internal use) */
  QString                       pcal_id_;             /* Linkword (internal use) */
  QString                       if_id_;               /* Linkword (internal use) */
  int                           bbc_no_;              /* Physical BBC# */
  QString                       if_name_;             /* Physical IF name */
  double                        if_total_lo_;         /* Hz */
  QString                       if_sideband_;         /* U or L */
  ChannelInfo();
  ChannelInfo(const ChannelInfo& ci);
  ChannelInfo& operator=(const ChannelInfo& ci);
};
//
ChannelInfo::ChannelInfo() :
  name_(""),
  polarization_(""),
  net_sideband_(""),
  band_id_(""),
  chan_id_(""),
  bbc_id_(""),
  pcal_id_(""),
  if_id_(""),
  if_name_(""),
  if_sideband_("")
{
  sky_frequency_ = -1.0;
  bandwidth_ = -1.0;
  bbc_no_ = -1;
  if_total_lo_ = -1.0;
};
ChannelInfo::ChannelInfo(const ChannelInfo& ci)
{
  name_ = ci.name_;
  polarization_ = ci.polarization_;
  sky_frequency_ = ci.sky_frequency_;
  net_sideband_ = ci.net_sideband_;
  bandwidth_ = ci.bandwidth_;
  band_id_ = ci.band_id_;
  chan_id_ = ci.chan_id_;
  bbc_id_ = ci.bbc_id_;
  pcal_id_ = ci.pcal_id_;
  if_id_ = ci.if_id_;
  bbc_no_ = ci.bbc_no_;
  if_name_ = ci.if_name_;
  if_total_lo_ = ci.if_total_lo_;
  if_sideband_ = ci.if_sideband_;
};
ChannelInfo& ChannelInfo::operator=(const ChannelInfo& ci)
{
  name_ = ci.name_;
  polarization_ = ci.polarization_;
  sky_frequency_ = ci.sky_frequency_;
  net_sideband_ = ci.net_sideband_;
  bandwidth_ = ci.bandwidth_;
  band_id_ = ci.band_id_;
  chan_id_ = ci.chan_id_;
  bbc_id_ = ci.bbc_id_;
  pcal_id_ = ci.pcal_id_;
  if_id_ = ci.if_id_;
  bbc_no_ = ci.bbc_no_;
  if_name_ = ci.if_name_;
  if_total_lo_ = ci.if_total_lo_;
  if_sideband_ = ci.if_sideband_;
  return *this;
};
//
//
//
struct StationInfo
{
  QString                       name_;
  QString                       id_2char_;
  QString                       id_1char_;
  int                           recorderType_;
  int                           trackFormat_;
  int                           bits_sample_;
  double                        sampleRate_;
  QMap<QString, ChannelInfo>    channelByName_;
  StationInfo();
  StationInfo(const StationInfo& si);
  StationInfo& operator=(const StationInfo& si);
};
//
StationInfo::StationInfo() :
  name_(""),
  id_2char_(""),
  id_1char_(""),
  channelByName_()
{
  recorderType_ = -1;
  trackFormat_ = -1;
  bits_sample_ = -1;
  sampleRate_ = -1.0;
};
//
StationInfo::StationInfo(const StationInfo& si) :
  name_(si.name_),
  id_2char_(si.id_2char_),
  id_1char_(si.id_1char_),
  channelByName_()
{
  recorderType_ = si.recorderType_;
  trackFormat_ = si.trackFormat_;
  bits_sample_ = si.bits_sample_;
  sampleRate_ = si.sampleRate_;
  channelByName_.clear();
  for (QMap<QString, ChannelInfo>::const_iterator it=si.channelByName_.begin(); 
    it!=si.channelByName_.end(); ++it)
    channelByName_.insert(it.key(), it.value());
};
StationInfo& StationInfo::operator=(const StationInfo& si)
{
  name_ = si.name_;
  id_2char_ = si.id_2char_;
  id_1char_ = si.id_1char_;
  recorderType_ = si.recorderType_;
  trackFormat_ = si.trackFormat_;
  bits_sample_ = si.bits_sample_;
  sampleRate_ = si.sampleRate_;
  channelByName_.clear();
  for (QMap<QString, ChannelInfo>::const_iterator it=si.channelByName_.begin(); 
    it!=si.channelByName_.end(); ++it)
    channelByName_.insert(it.key(), it.value());
  return *this;
};
//



struct VexInfo
{
  double                        sampleRate_;
  double                        apLength_;
  QString                       expName_;
  VexInfo();
  VexInfo(const VexInfo& vi);
  VexInfo& operator=(const VexInfo& vi);
};
//
VexInfo::VexInfo() : expName_("")
{
  sampleRate_ = -1.0;
  apLength_ = -1.0;
};
VexInfo::VexInfo(const VexInfo& vi)
{
  *this = vi;
};
VexInfo& VexInfo::operator=(const VexInfo& vi)
{
  sampleRate_ = vi.sampleRate_;
  apLength_ = vi.apLength_;
  expName_ = vi.expName_;
  return *this;
};
//



//
struct FringeFileName
{
  QString                       baselineId_;
  QString                       bandId_;
  QString                       sequenceId_;
  QString                       rootCodeId_;
  bool                          isOk_;
  int                           sequenceNumber_;
  FringeFileName();
  inline FringeFileName(const FringeFileName& fn) {*this = fn;};
  FringeFileName(const QString fileName);
  QString fileName() const 
    {return isOk_?(baselineId_ + "." + bandId_ + "." + sequenceId_ + "." + rootCodeId_):"";};
  QString id() const 
    {return isOk_?(baselineId_ + "." + bandId_):"";};
  FringeFileName& operator=(const FringeFileName& fn);
};
//
FringeFileName::FringeFileName() : baselineId_(""), bandId_(""), sequenceId_(""), rootCodeId_("")
{
  isOk_ = false;
  sequenceNumber_ = 0;
};
//
FringeFileName::FringeFileName(const QString fileName) : 
  baselineId_(""), bandId_(""), sequenceId_(""), rootCodeId_("")
{
  isOk_ = false;
  sequenceNumber_ = 0;
  QStringList                   itemList=fileName.split('.');
  if (itemList.size() == 4)
  {
    baselineId_ = itemList.at(0);
    bandId_ = itemList.at(1);
    sequenceId_ = itemList.at(2);
    rootCodeId_ = itemList.at(3);
    if (baselineId_.size()==2 && baselineId_.at(0)!=baselineId_.at(1) && 
        bandId_.size()==1 && sequenceId_.size()>0 && rootCodeId_.size()>0)
      sequenceNumber_ = sequenceId_.toInt(&isOk_);
  };
};
//
FringeFileName& FringeFileName::operator=(const FringeFileName& fn)
{
  baselineId_ = fn.baselineId_;
  bandId_ = fn.bandId_;
  sequenceId_ = fn.sequenceId_;
  rootCodeId_ = fn.rootCodeId_;
  isOk_ = fn.isOk_;
  sequenceNumber_ = fn.sequenceNumber_;
  return *this;
};





//
class FringeFileMap : public QMap<QString, FringeFileName>
{
public:
  inline FringeFileMap() : QMap<QString, FringeFileName>() {};
  inline ~FringeFileMap() {clear();};
  void registerFileName(const QString fileName);
};
//
void FringeFileMap::registerFileName(const QString fileName)
{
  FringeFileName                ffName(fileName);
  if (ffName.isOk_)
  {
    if (contains(ffName.id()))
    {
      if (value(ffName.id()).sequenceNumber_ < ffName.sequenceNumber_)
      {
        logger->write(SgLogger::DBG, SgLogger::IO,
          "FringeFileMap::registerFileName(): the fringe file name " + 
          (*this)[ffName.id()].fileName() + " is being replaced by " + ffName.fileName());
        (*this)[ffName.id()] = ffName;
      };
    }
    else
    {
      insert(ffName.id(), ffName);
    }
  };
};



//
struct CorelRootFileName
{
  QString                       sourceId_;
  QString                       rootCodeId_;
  bool                          isOk_;
  CorelRootFileName();
  CorelRootFileName(const QString fileName);
  inline CorelRootFileName(const CorelRootFileName& cr) {*this = cr;};

  QString fileName() const 
    {return isOk_?(sourceId_ + "." + rootCodeId_):"";};
  CorelRootFileName& operator=(const CorelRootFileName& cr);
};
//
CorelRootFileName::CorelRootFileName() : sourceId_(""), rootCodeId_("")
{
  isOk_ = false;
};
//
CorelRootFileName::CorelRootFileName(const QString fileName) : 
  sourceId_(""), rootCodeId_("")
{
  isOk_ = false;
  QStringList                   itemList=fileName.split('.');
  if (itemList.size() == 2)
  {
    sourceId_ = itemList.at(0);
    rootCodeId_ = itemList.at(1);
    isOk_ = true;
  };
};
//
CorelRootFileName& CorelRootFileName::operator=(const CorelRootFileName& cr)
{
  sourceId_ = cr.sourceId_;
  rootCodeId_ = cr.rootCodeId_;
  isOk_ = cr.isOk_;
  return *this;
};











// end of aux data structures


//
//
//
bool SgVlbiSession::getDataFromFringeFiles(const QString& path2data,
  const QString& altDatabaseName, const QString& altCorrelatorName, 
  const QString& historyFileName, const QString& mapFileName,
  const QList<QString>& fringeErrorCodes2Skip, const QStringList& masterfileSuffixes,
  bool acceptAllRootFiles)
{
  bool                          isOk=false, isScanDirOk;
  QDir                          dir(path2data);
  QList<QString>                dirsOfScans;
  QStringList                   dirList;
//QRegExp                       reScanDirName("[0-9]{3}-[0-9]{4}[a-zA-Z]{0,1}");
//QRegExp                       reVexFileName("^([0-9a-zA-Z+-_]{1,8})\\.([\\w]{6,7})$");
  int                           idx;
  // per scan entries:
//  QString                       vexFileName("");
  FringeFileMap                 scanFileNames;
  QMap<QString, CorelRootFileName>
                                crootById;
  QString                       sPiName, sExperDescr;
  QMap<QString, int>            piNamesByCount, expDescrsByCount, expNameByCount;
  QMap<int, int>                expSerialNumByCount;
  QString                       correlatorComments(historyFileName);
  QString                       correlatorName("");
  QMap<QString, QString>        stnNameById, stnNameByI; // maps of station name by 2- and 1-char Ids
  //
  // for name maps:
  QMap<QString, QString>        stn2stn;
  QMap<QString, QString>        src2src;
  QMap<QString, QString>        bnd2bnd;
  QMap<QString, int>            fringeErrorCodeByInt;
  QMap<QString, int>            corrNameByCount;
  // an alternative database name:
  QRegularExpression            reOldDbName("(\\d{2})([A-Z]{3})(\\d{2})([A-Z0-9]{1,2})");
  QRegularExpression            reNewDbName("(\\d{8})-([\\S]{2,12})");
  QRegularExpression            reSrc(strReSourceName);
//QRegularExpressionMatch       match;
  
  dirList = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
  if (dirList.size() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): nothing to read");
    return isOk;
  };
  //
  //
  importMapFile(mapFileName, stn2stn, src2src, bnd2bnd);
  if (stn2stn.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): loaded " + QString("").setNum(stn2stn.size()) + 
      " entries for station name maps");
  if (src2src.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): loaded " + QString("").setNum(src2src.size()) + 
      " entries for source name maps");
  if (bnd2bnd.size())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): loaded " + QString("").setNum(bnd2bnd.size()) + 
      " entries for bnd name maps");
  if (fringeErrorCodes2Skip.size())
  {
    for (int i=0; i<fringeErrorCodes2Skip.size(); i++)
      fringeErrorCodeByInt.insert(fringeErrorCodes2Skip.at(i), 1);
  };
  //
  //
  for (int i=0; i<dirList.size(); i++)
    dirsOfScans << dirList.at(i);
      
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::getDataFromFringeFiles(): filtered " + QString("").setNum(dirsOfScans.size()) + 
    " scan entries from " + QString("").setNum(dirList.size()) + " total number of directories");
  
  stnNameById.clear();
  stnNameByI.clear();
  for (int i=0; i<dirsOfScans.size(); i++)
  {
    const QString              &inDirName=dirsOfScans.at(i);
    CorelRootFileName           croot;

    dir.setPath(path2data + "/" + inDirName);
    dirList.clear();
    dirList = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name);
    
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): processing directory '" + dir.path() + "'");

    isScanDirOk = true;
    scanFileNames.clear();
    crootById.clear();
    //
    // analyze what we get:
    QStringList                 listOfVexFiles;
    for (int j=0; j<dirList.size(); j++)
    {
      const QString            &fileName=dirList.at(j);
      CorelRootFileName         croot(fileName);
      if (croot.isOk_)
      {
        if (acceptAllRootFiles || reSrc.match(fileName).hasMatch())
        {
          if (!crootById.contains(croot.rootCodeId_))
            crootById.insert(croot.rootCodeId_, croot);
           else
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::getDataFromFringeFiles(): the croot file " + fileName + " with crootId \"" +
              croot.rootCodeId_ + "\" already registered as \"" + 
              crootById.value(croot.rootCodeId_).fileName() + "\"");
        }
        else
          logger->write(SgLogger::DBG, SgLogger::IO, className() +
            "::getDataFromFringeFiles(): not a Mk4 root file: \"" + fileName + "\"");
      }
      else
        scanFileNames.registerFileName(fileName);
    };
    //
    // check our containers:
    if (crootById.size() < 1)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): cannot find any VEX file in the directory \"" + dir.path() + 
        "\", skipped");
      isScanDirOk = false;
    };
    if (scanFileNames.size() < 1)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): cannot find any usable fringe file in the directory \"" + 
        dir.path() + "\", skipped");
      isScanDirOk = false;
    };
    //
    if (isScanDirOk)
    {
      QMap<QString, QMap<QString, StationInfo> >
                                stnInfoByNameById;
      QMap<QString, VexInfo>    vexInfoById;
      //
      // ok, first, read VEXes:
      for (QMap<QString, CorelRootFileName>::iterator it=crootById.begin(); it!=crootById.end(); ++it)
      {
        VexInfo                 vi;
        QMap<QString, StationInfo>
                                stnInfoByName;
        const QString          &vexFileName=it.value().fileName();
        //
        // collect available data from a VEX file (data that we should expect in the scan dir):

        processVexFile(dir.path() + "/" + vexFileName, stnInfoByName, vi, stn2stn, correlatorName);
        if (correlatorName == "difx" || correlatorName == "sfxc")
          correlatorName = "";
        vexInfoById.insert(it.key(), vi);
        stnInfoByNameById.insert(it.key(), stnInfoByName);

        // check station names and ids:
        for (QMap<QString, StationInfo>::iterator it=stnInfoByName.begin();
          it!=stnInfoByName.end(); ++it)
        {
          const QString&        stnName=it.value().name_;
          const QString&        stn2chrId=it.value().id_2char_;
          const QString&        stn1chrId=it.value().id_1char_;
          // check 2-chars id:
          if (stnNameById.contains(stn2chrId))
          {
            if (stnNameById.value(stn2chrId) != stnName)
            {
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): -------------------------------------------");
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): the croot file " + inDirName + "/" + vexFileName + 
                " contains station identities that missmatch previous data:");
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): " + stnNameById.value(stn2chrId) + "[" + 
                stn2chrId + "] != " + stnName);
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): -------------------------------------------");
            }; 
          }
          else
            stnNameById.insert(stn2chrId, stnName);
          // check 1-chars id:
          if (stnNameByI.contains(stn1chrId))
          {
            if (stnNameByI.value(stn1chrId) != stnName)
            {
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): -------------------------------------------");
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): the croot file " + inDirName + "/" + vexFileName + 
                " contains station identities that missmatch previous data:");
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): " + stnNameByI.value(stn1chrId) + "[" + 
                stn1chrId + "] != " + stnName);
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::getDataFromFringeFiles(): -------------------------------------------");
            }; 
          }
          else
            stnNameByI.insert(stn1chrId, stnName);
        };
        //
        parseVexFile(dir.path() + "/" + vexFileName, sPiName, sExperDescr);
        // collect all available P.I. names and experiment descriptions to check consistency (later)
        piNamesByCount[sPiName]++;
        expDescrsByCount[sExperDescr]++;
        expNameByCount[vi.expName_]++;
        corrNameByCount[correlatorName]++;
        logger->write(SgLogger::DBG, SgLogger::IO, className() +
          "::getDataFromFringeFiles(): the corel.root file " + vexFileName + " has been processed");
      };
      //
      for (QMap<QString, FringeFileName>::iterator jt=scanFileNames.begin(); jt!=scanFileNames.end();
        ++jt)
      {
        FringeFileName         &ffn=jt.value();
        const QString          &crootId=ffn.rootCodeId_;
        int                     sn;
        if (!(crootById.contains(crootId) && vexInfoById.contains(crootId) &&
              stnInfoByNameById.contains(crootId)))
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::getDataFromFringeFiles(): cannot find a corel.root file for the fringe file '" + 
            ffn.fileName() + "' with rootId=[" + crootId + "], skipped");
        else
        {
          sn = 0;
          processFringeFile(dir.path(), ffn.fileName(), stnInfoByNameById.value(crootId),
            vexInfoById.value(crootId), crootById.value(crootId).fileName(), stnNameById, stnNameByI,
            stn2stn, src2src, bnd2bnd, fringeErrorCodeByInt, sn);
            expSerialNumByCount[sn]++;
        };
      };
      //
      // end of data processing.
    };
  };
  //
  //
  // check: did we read something at all:
  if (!bands_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): nothing useful found");
    return false;
  };
  //
  //
  setOriginType(OT_MK4);    // flag it
  setCorrelatorType("MK4     ");
  addAttr(Attr_FF_CREATED);
  cppsSoft_ = CPPS_HOPS;
  //
  //
  // set global stuff here:
  //
  if (piNamesByCount.count() == 1)
  {
    if (piNamesByCount.begin().key().size())
      setPiAgencyName(piNamesByCount.begin().key());
    else
      setPiAgencyName("Undefined");
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): session P.I agency name was set to " + getPiAgencyName());
  }
  else
  {
    // need add more diagnostics here:
    setPiAgencyName("Mixed");
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): more than one PI agency names collected:");
    for (QMap<QString, int>::iterator itt=piNamesByCount.begin(); itt!=piNamesByCount.end(); ++itt)
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles():    \"" + itt.key() + "\" => " + QString("").setNum(itt.value()));
  };
  if (expNameByCount.count() == 1)
  {
    if (expNameByCount.begin().key().size())
      setSessionCode(expNameByCount.begin().key().toUpper()); // take into account Mike Titus effect
    else
      setSessionCode("Undefined");
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): session code was set to " + getSessionCode());
  }
  else
  {
    // need add more diagnostics here:
    setSessionCode("Mixed");
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): more than one experiment name found:");
    for (QMap<QString, int>::iterator itt=expNameByCount.begin(); itt!=expNameByCount.end(); ++itt)
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles():    \"" + itt.key() + "\" => " + QString("").setNum(itt.value()));
  };
  if (expDescrsByCount.count() == 1)
  {
    if (expDescrsByCount.begin().key().size())
      setDescription(expDescrsByCount.begin().key());
    else 
      setDescription("Undefined");
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): session description was set to " + getDescription());
  }
  else
  {
    // need add more diagnostics here:
    setDescription("Mixed");
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): more than one experiment description found:");
    for (QMap<QString, int>::iterator itt=expDescrsByCount.begin(); itt!=expDescrsByCount.end(); ++itt)
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles():    \"" + itt.key() + "\" => " + QString("").setNum(itt.value()));
  };
  //
  if (getExperimentSerialNumber() == 0) // is not set through command line arg
  {
    if (expSerialNumByCount.count() == 1)
    {
      int                       n;
      n = expSerialNumByCount.begin().key();
      if (0<n && n!=16383)
      {
        setExperimentSerialNumber(expSerialNumByCount.begin().key());
        logger->write(SgLogger::DBG, SgLogger::IO, className() +
          "::getDataFromFringeFiles(): session serial number was set to " + 
          QString("").sprintf("%d", getExperimentSerialNumber()));
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::getDataFromFringeFiles(): the observations do not have correct experiment serial number " +
          QString("").sprintf("(==%d)", n));
    }
    else
    {
      // need add more diagnostics here:
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): more than one experiment serial number found:");
      for (QMap<int, int>::iterator it=expSerialNumByCount.begin(); it!=expSerialNumByCount.end(); ++it)
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::getDataFromFringeFiles(): " + QString("").sprintf("%d -> %d times", it.key(), it.value()));
    };
  };
  if (corrNameByCount.count() == 1)
  {
    if (corrNameByCount.begin().key().size())
    {
      setCorrelatorName(corrNameByCount.begin().key());
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): correlator name was set to \"" + getCorrelatorName() + "\"");
    }
    else
    {
      setCorrelatorName("");
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): cannot find correlator name in the vex files");
    };
  }
  else
  {
    // need add more diagnostics here:
    setCorrelatorName("Mixed");
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): more than one correlator name collected:");
    for (QMap<QString, int>::iterator itt=corrNameByCount.begin(); itt!=corrNameByCount.end(); ++itt)
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles():    \"" + itt.key() + "\" => " + QString("").setNum(itt.value()));
  };
  //
  //
  // if a user has provided an alternative database name, check it:
  if (2 < altDatabaseName.size())
  {
    if (reOldDbName.match(altDatabaseName).hasMatch())
    {
      setName(altDatabaseName);
      setNetworkSuffix(altDatabaseName.at(8));
      setOfficialName("UNKN");
      setSubmitterName("UNKN");
      setSchedulerName("UNKN");
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): database name was explicitly set to " + altDatabaseName + 
        ", a version 1 database name pattern");
    }
    else if (reNewDbName.match(altDatabaseName).hasMatch())
    {
      setName(altDatabaseName);
      setNetworkSuffix("-");
      setOfficialName("UNKN");
      setSubmitterName("UNKN");
      setSchedulerName("UNKN");
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): database name was explicitly set to " + altDatabaseName + 
        ", a version 2 database name pattern");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): the provided database name, " + altDatabaseName + 
        ", does not fit any known pattern and was ignored");
  };
  //
  //
  QMap<double, SgVlbiBand*>     bandByFreq;
  QString                       str("");
  for (int i=0; i<bands_.size(); i++)
  {
    SgVlbiBand                 *band=bands_.at(i);
    QString                     bandKey=band->getKey();
    band->setTCreation(SgMJD::currentMJD().toUtc());
    band->setInputFileName(path2data + "/*");
    band->setInputFileVersion(0);
    //
    // quick'n'dirty:
    idx = 0;
    SgVlbiObservable           *o=observations_.at(idx)->observable(bandKey);
    while (!o && idx<observations_.size())
      o = observations_.at(idx++)->observable(bandKey);
    if (o)
      band->setFrequency(o->getReferenceFrequency());
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): cannot set up a frequency for " + bandKey + "-band");
    if (bandByFreq.contains(band->getFrequency()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): the frequency " + QString("").setNum(band->getFrequency()) + 
        " is already registered");
    }
    else
      bandByFreq.insert(band->getFrequency(), band);
  };
  //
  // set the order:
  setSidebandOrder(SO_USB_LSB);
  //
  //
  // load correlator's comments for history records:
  //
  if (correlatorComments.size()==0)
  {
    dir.setPath(path2data + "/../control/");
    str = "";
    if (!dir.exists())
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::getDataFromFringeFiles(): cannot get correlator comments, the directory does not exist: " +
        dir.path());
    else
    {
      dirList.clear();
      dirList = dir.entryList(QStringList() << "*.corr", 
        QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name);
      if (dirList.size() == 0)
      { 
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromFringeFiles(): cannot get correlator comments, no any *.corr file found: " + 
          dir.path());
      }
      else if (dirList.size() > 1)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromFringeFiles(): found more than one file with correlator comments:");
        for (int j=0; j<dirList.size(); j++)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromFringeFiles(): " + dirList.at(j));
        str = dirList.at(0);
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::getDataFromFringeFiles(): picked up this one: " + str);
      }
      else
        str = dirList.at(0);
    };
    if (str.size())
      correlatorComments = path2data + "/../control/" + str;
  };
  if (correlatorComments.size())
    getCorrelatorHistory(correlatorComments);
  else
  {
    for (int i=0; i<bands_.size(); i++)
    {
      bands_.at(i)->history().addHistoryRecord("== The correlator report was not provided. ==", 
        SgMJD::currentMJD().toUtc());
      bands_.at(i)->history().addHistoryRecord("== ", SgMJD::currentMJD().toUtc());
    };
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromFringeFiles(): the correlator report was not provided.");
  };
  //
  //
  // set up a primary band:
  // standard IVS case:
  QString                       primeBandKey;
  if (bandByKey_.size()==2 && bandByKey_.contains("X") && bandByKey_.contains("S"))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromFringeFiles(): importing typical IVS session");
    primaryBand_ = bandByKey_.value("X");
  }
  else
  {
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::getDataFromFringeFiles(): importing a foreign session setup");
    if (bandByFreq.size() > 1)
      primaryBand_ = bandByFreq.begin().value();
    else
      primaryBand_ = bands_.at(0);
  };
  primaryBand_->addAttr(SgVlbiBand::Attr_PRIMARY);
  primeBandKey = primaryBand_->getKey();
  //
  // remove observations that are not in the primary band:
  int                           num;
  idx = num = 0;
  while (idx<observations_.size())
  {
    SgVlbiObservation          *obs=observations_.at(idx);
    if (!obs->observable(primeBandKey))
    {
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::getDataFromFringeFiles(): the observation " + obs->getKey() + 
        " of the scan " + obs->getScanName() + " is being removed from the data set");
      observationByKey_.remove(obs->getKey());
      observations_.removeAt(idx);
      idx--;
      num++;
      delete obs;
    };
    idx++;
  };
  if (num)
  {
    logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
      "::getDataFromFringeFiles(): " + QString("").setNum(num) + 
      " second-band-only observation" + (num==1?" was":"s were") + " removed");
    //
    // check source list:
    // ...
  };
  //
  //
  if (selfCheck(false, masterfileSuffixes))
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::getDataFromFringeFiles(): session selfcheck complete");
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::getDataFromFringeFiles(): the selfcheck for the session failed");
    return false;
  };
  //
  // overwrite correlator name with user-provided info:
  if (altCorrelatorName.size())
  {
    setCorrelatorName(altCorrelatorName);
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::getDataFromFringeFiles(): correlator name was explicitly set to \"" + altCorrelatorName + "\"");
  };
  //
  //
  // calculate aux data:
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin(); 
      it!=obs->observableByKey().end(); ++it)
    {
      SgVlbiObservable         *o=it.value();
      o->calcPhaseCalDelay();
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO, className() + 
    "::getDataFromFringeFiles(): phase cal delays were calculated");
  //
  //
  return true;
};



//
void SgVlbiSession::parseVexFile(const QString& vexFileName, QString& sPiName, QString& sExpDescr)
{
  QFile                         f(vexFileName);
  QString                       str;
  bool                          foundPiName, foundExpDescr;
//  QRegExp                       reExpDescr("^\\s*exper_description\\s*=\\s*(\\S+\\s*)+;$");
  QRegExp                       reExpDescr("^\\s*exper_description\\s*=\\s*\"?(\\S+.*\\S+)\"?;$");
  QRegExp                       rePiName("^\\s*PI_name\\s*=\\s*(\\S+);");
  
  
  foundPiName = foundExpDescr = false;
  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    //
    while (!s.atEnd() && !(foundPiName && foundExpDescr))
    {
      str = s.readLine();
      if (str.contains(reExpDescr))
      {
        sExpDescr = reExpDescr.cap(1);
        if (sExpDescr.at(0) == '\"')
          sExpDescr.remove(0,1);
        if (sExpDescr.at(sExpDescr.size()-1) == '\"')
          sExpDescr.chop(1);
        foundExpDescr = true;
      }
      else if (str.contains(rePiName))
      {
        sPiName = rePiName.cap(1);
        foundPiName = true;
      };
    };
    if (false && !foundPiName) // modern PIs do not bother to provide PI name
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::parseVexFile(): PI Name was not found; the file is " + vexFileName);
    if (!foundExpDescr)
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::parseVexFile(): Experiment Description was not found; the file is " + vexFileName);
    
    f.close();
    s.setDevice(NULL);
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::parseVexFile(): cannot open a file " + vexFileName + " for reading");
};



//
void SgVlbiSession::processVexFile(const QString& vexFileName, QMap<QString, StationInfo> &stationByName,
  VexInfo& vi, const QMap<QString, QString>& stn2stn, QString& correlatorName)
{
  bool                          have2clearEnv;
  QString                       path2textFiles(PATH_2_HOPS_SHARE);
  
  have2clearEnv = false;
  if (!getenv("TEXT"))
  {
    path2textFiles += QString("/text");
    setenv("TEXT", qPrintable(path2textFiles), 0);
    have2clearEnv = true;
    /*
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::processVexFile(): the env.variable TEXT was set to " + path2textFiles);
    */
  };
  //
  //
  int                           rc=0;
  char                          vexkey[2]={0,0};
  char                         *filename;
  vex                           vexRoot;
  scan_struct                  *scan=NULL;
  evex_struct                  *evex=NULL;
  *vexRoot.filename = 0;

  filename = new char[vexFileName.size() + 1];
  strcpy(filename, qPrintable(vexFileName));

  if ((rc=get_vex(filename, OVEX | EVEX | IVEX | LVEX, vexkey, &vexRoot)) != 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::processVexFile(): cannot get_vex the file " + vexFileName + "; RC=" + QString("").setNum(rc));
  }
  else if (!(scan=vexRoot.ovex))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::processVexFile(): the ovex struct is NULL");
  }
  else if (!(evex=vexRoot.evex))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::processVexFile(): the evex struct is NULL");
  }
  else
  {
    QString                     sScanName("");
    QString                     sSrcName("");
    int                         numStns;
    
    vi.expName_       = scan->exper_name;
    vi.apLength_      = evex->ap_length;
    
    stationByName.clear();
    // SAMPLRAT
    // APLENGTH
    // BBC IND
    // BIT SAMPL
    // LO FREQ
    source_struct              &src=scan->src;
    sScanName = scan->scan_name;
    correlatorName = scan->correlator;
    sSrcName = src.source_name;
    numStns = scan->nst;
    for (int i=0; i<numStns; i++)
    {
      station_struct           &stn=scan->st[i];
      int                       maxNumChannels=sizeof(stn.channels)/sizeof(chan_struct);
      StationInfo               station;
      station.name_.sprintf("%-8s", stn.site_name);
      // map it:
      if (stn2stn.size() && stn2stn.contains(station.name_) && stn2stn.value(station.name_).size()>1)
      {
        station.name_ = stn2stn.value(station.name_);
        logger->write(SgLogger::DBG, SgLogger::IO, className() +
          "::processVexFile(): the input station name \"" + stn.site_name + "\" has been mapped to \"" +
          station.name_ + "\"");
      };
      station.id_2char_ = stn.site_id;
      station.id_1char_ = stn.mk4_site_id;
      if (stn.recorder_type != I_UNDEFINED)
        station.recorderType_ = stn.recorder_type;
      else if (stn.rack_type != I_UNDEFINED)
        station.recorderType_ = stn.rack_type;
      if (stn.track_format != I_UNDEFINED)
        station.trackFormat_ = stn.track_format;
      if (stn.bits_sample != I_UNDEFINED)
        station.bits_sample_ = stn.bits_sample;
      if (stn.samplerate != F_UNDEFINED)
        station.sampleRate_ = stn.samplerate;
      for (int j=0; j<maxNumChannels; j++)
      {
        chan_struct            &chan=stn.channels[j];
        if (chan.sky_frequency > 1.0e3)
        {
          ChannelInfo           channel;
          channel.name_ = chan.chan_name;
          channel.polarization_ = chan.polarization;
          channel.sky_frequency_ = chan.sky_frequency;
          channel.net_sideband_ = chan.net_sideband;
          channel.bandwidth_ = chan.bandwidth;
          channel.band_id_ = chan.band_id;
          channel.chan_id_ = chan.chan_id;
          channel.bbc_id_ = chan.bbc_id;
          channel.pcal_id_ = chan.pcal_id;
          channel.if_id_ = chan.if_id;
          channel.bbc_no_ = chan.bbc_no;
          channel.if_name_ = chan.if_name;
          channel.if_total_lo_ = chan.if_total_lo;
          channel.if_sideband_ = chan.if_sideband;
          station.channelByName_[channel.name_] = channel;
        };
      };
      if (stationByName.contains(station.name_))
        logger->write(SgLogger::ERR, SgLogger::IO, className() +
          "::processVexFile(): got a duplicate record for the station \"" + station.name_ + "\"");
      stationByName[station.name_] = station;
    };
  };
  //
  // clear stuff:
  delete[] filename;
  if (have2clearEnv)
    unsetenv("TEXT");
};



//
void SgVlbiSession::processFringeFile(const QString& path2file, const QString& fringeFileName,
  const QMap<QString, StationInfo>& stnsInfo, const VexInfo& vexInfo, const QString& vexFileName,
  const QMap<QString, QString>& stnNameById, const QMap<QString, QString>& stnNameByI,
  const QMap<QString, QString>& stn2stn, const QMap<QString, QString>& src2src,
  const QMap<QString, QString>& bnd2bnd, const QMap<QString, int>& fringeErrorCodeByInt, 
  int& expSerialNumber)
{
  int                           rc=0;
  char                         *filename;
  mk4_fringe                    fringe;
  QString                       str("");
  QString                       bandKey("");
  QRegExp                       reBandName("^(\\S*)([0-9a-zA-Z]{2})\\.([a-zA-Z]{1})\\."
                                  "\\d+\\.([0-9a-zA-Z]{5,7})$");
  //
  fringe.nalloc = 0;
  str = path2file + "/" + fringeFileName;
  filename = new char[str.size() + 1];
  strcpy(filename, qPrintable(str));
  //
  if (fringeFileName.contains(reBandName))
  {
    bandKey = reBandName.cap(3);
  };
  // 
  // check map of bands:
  if (bnd2bnd.size())
  {
    if (!check4NameMap(bnd2bnd, bandKey))
    {
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::processFringeFile(): skipping the observation " + fringeFileName + 
        ": the band \"" + bandKey + "\" have to be skipped");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
  };
  //
  if ((rc=read_mk4fringe(filename, &fringe)) != 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::processFringeFile(): cannot read_mk4fringe the file " + str + 
      "; RC=" + QString("").setNum(rc));
  }
  else if (!bandKey.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::processFringeFile(): cannot figure out a name of band from the file name " + str);
  }
  else
  {
    type_200                   *t200=fringe.t200;
    type_201                   *t201=fringe.t201;
    type_202                   *t202=fringe.t202;
    type_203                   *t203=fringe.t203;
    type_204                   *t204=fringe.t204;
    type_205                   *t205=fringe.t205;
    type_206                   *t206=fringe.t206;
    type_207                   *t207=fringe.t207;
    type_208                   *t208=fringe.t208;
    type_210                   *t210=fringe.t210;
    //type_212                 *t212=fringe.t212[0];

    if (fringeErrorCodeByInt.size() && t208->errcode != ' ')
    {
      if (fringeErrorCodeByInt.contains("*") ||
          fringeErrorCodeByInt.contains(QString(t208->errcode)))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processFringeFile(): skipping the observation " + fringeFileName + 
          ": the fringe error code is \"" + QString(t208->errcode) + "\"");
        clear_mk4fringe(&fringe);
        delete[] filename;
        return;
      };
    };

/*
//    fringeErrorCodeByInt
    if (t208->errcode != ' ')
    {
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::processFringeFile(): skipping the observation " + fringeFileName + 
        ": the fringe error code is \"" + QString(t208->errcode) + "\"");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
*/

    // check is it already imported:
    SgVlbiBand                 *band=NULL;
    //
    QString                     station1Name, station2Name, sourceName, baselineName;
    QString                     scanName, scanId, obsKey;
    int                         obsIdx;
    int                         nTmp;
    double                      f;
    SgMJD                       epoch;
    SgVlbiStationInfo          *station1Info, *station2Info;
    SgVlbiStationInfo          *bandStation1Info, *bandStation2Info;
    SgVlbiSourceInfo           *sourceInfo, *bandSourceInfo;
    SgVlbiBaselineInfo         *baselineInfo, *bandBaselineInfo;
    SgVlbiObservation          *obs=NULL;
    SgVlbiObservable           *o=NULL;
    SgVlbiAuxObservation       *auxObs_1=NULL, *auxObs_2=NULL;
    
    double                      refFreq=0.0;
    bool                        isSbdSigmaNan, isSbdSigmaInf;
    bool                        isGrdSigmaNan, isGrdSigmaInf;
    bool                        isPhrSigmaNan, isPhrSigmaInf;
    bool                        isTmp;
    char                        buff1[32], buff2[32];
    int                         numOfChannels;
    double                      effFreq4GR=0.0, effFreq4PH=0.0, effFreq4RT=0.0;


    if (bandByKey_.contains(bandKey))
      band = bandByKey_.value(bandKey);
    else
    {
      band = new SgVlbiBand;
      band->setKey(bandKey);
      bands_.append(band);
      bandByKey_.insert(bandKey, band);
    };
    //
    epoch.setUpEpoch(t200->frt.year, 0, t200->frt.day,                      // Fringe Reference Time
                     t200->frt.hour, t200->frt.minute, t200->frt.second);
    
    // stations and source names:
    // the t202's char fields are not null-terminated strings, just arrays
    memset(buff1, 0, 32);
    memset(buff2, 0, 32);
    strncpy(buff1, t202->ref_name, 8);
    strncpy(buff2, t202->rem_name, 8);
    station1Name.sprintf("%-8s", buff1);
    station2Name.sprintf("%-8s", buff2);
    sourceName  .sprintf("%-8s", t201->source);
    //
    // map the names:
    if (stn2stn.size())
    {
      if (!check4NameMap(stn2stn, station1Name))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processFringeFile(): skipping the observation " + fringeFileName + 
          ": the station \"" + station1Name + "\" have to be skipped");
        clear_mk4fringe(&fringe);
        delete[] filename;
        return;
      };
      if (!check4NameMap(stn2stn, station2Name))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processFringeFile(): skipping the observation " + fringeFileName + 
          ": the station \"" + station2Name + "\" have to be skipped");
        clear_mk4fringe(&fringe);
        delete[] filename;
        return;
      };
    };
    if (src2src.size())
    {
      if (!check4NameMap(src2src, sourceName))
      {
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::processFringeFile(): skipping the observation " + fringeFileName + 
          ": the source \"" + sourceName + "\" have to be skipped");
        clear_mk4fringe(&fringe);
        delete[] filename;
        return;
      };
    };
    //
    //
    // verify stations names and ids:
    // one-char id:
    str = QString(t202->baseline[0]);
    if (!stnNameByI.contains(str))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::processFringeFile(): cannot find a station for ref one-char id '" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    }
    else if (stnNameByI.value(str) != station1Name)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::processFringeFile(): the file '" + filename + "' contains wrong station identities: " +
        station1Name + " instead of " + stnNameByI.value(str) + " with CId='" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
    str = QString(t202->baseline[1]);
    if (!stnNameByI.contains(str))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::processFringeFile(): cannot find a station for rem one-char id '" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    }
    else if (stnNameByI.value(str) != station2Name)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::processFringeFile(): the file '" + filename + "' contains wrong station identities: " +
        station2Name + " instead of " + stnNameByI.value(str) + " with CId='" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
    // two-chars id:
    str  = QString(t202->ref_intl_id[0]);
    str += QString(t202->ref_intl_id[1]);
    if (!stnNameById.contains(str))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::processFringeFile(): cannot find a station for two-char id '" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    }
    else if (stnNameById.value(str) != station1Name)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::processFringeFile(): the file '" + filename + "' contains wrong station identities: " +
        station1Name + " instead of " + stnNameById.value(str) + " with SId='" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
    str  = QString(t202->rem_intl_id[0]);
    str += QString(t202->rem_intl_id[1]);
    if (!stnNameById.contains(str))
    {
      logger->write(SgLogger::ERR, SgLogger::IO_DBH, className() +
        "::processFringeFile(): cannot find a station for two-char id '" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    }
    else if (stnNameById.value(str) != station2Name)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::processFringeFile(): the file '" + filename + "' contains wrong station identities: " +
        station2Name + " instead of " + stnNameById.value(str) + " with SId='" + str + "'");
      clear_mk4fringe(&fringe);
      delete[] filename;
      return;
    };
    // ok here
    //
    baselineName = station1Name + ":" + station2Name;
    // scanId and scanName:
    scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
//  if (t200->scan_name)
    if (strnlen(t200->scan_name, 32))
      scanName.sprintf("%-10s", t200->scan_name);
    else
      scanName.sprintf("%03d-%02d:%02d:%04.1f@%s",
        epoch.calcDayOfYear(), epoch.calcHour(), epoch.calcMin(), epoch.calcSec(), 
        qPrintable(sourceName));

    // pick up or create an observation:
    obsKey.sprintf("%s", 
      qPrintable(epoch.toString(SgMJD::F_INTERNAL) + "-" + baselineName + "@" + sourceName));
    if (observationByKey_.contains(obsKey))
      obs = observationByKey_.value(obsKey);
    else
    {
      obs = new SgVlbiObservation(this);
      obs->setMJD(epoch);
      obs->setScanName(scanName);
      obs->setScanId(scanId);
      obs->setCorrRootFileName(vexFileName);
      obs->setKey(obsKey);
      obs->setMediaIdx(observations_.size());
      observations_.append(obs);
      observationByKey_.insert(obsKey, obs);
    };
    o = new SgVlbiObservable(obs, band);
    obsIdx = obs->getMediaIdx();
    o->setMediaIdx(obsIdx);
    if (!obs->addObservable(bandKey, o))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): failed to add the observable at the baseline <" + 
        station1Name + ":" + station2Name + "> observing " + sourceName + " source at " + 
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + " on the " + bandKey + "-band");
    };
    obs->setupActiveObservable(bandKey);
    // determine number of channels:
    numOfChannels = 0;
    nTmp = 16; // ver "00" has dimension of 16
    if (t205->version_no[0] == '0' && t205->version_no[1] == '0')
      {}
    else if (t205->version_no[0] == '0' && t205->version_no[1] == '1')
      nTmp = 64;
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): unknown version of the type_205 struct: " + 
        path2file + "/" + fringeFileName);
    for (int i=0; i<nTmp; i++)
      if (t205->ffit_chan[i].channels[0]>-1 ||
          t205->ffit_chan[i].channels[1]>-1  )
        numOfChannels++;
    o->allocateChannelsSetupStorages(numOfChannels);
    expSerialNumber = t200->expt_no;
    //
    //
    // station #1:
    if (stationsByName_.contains(station1Name))
      station1Info = stationsByName_.value(station1Name);
    else // new station, add it to the container and register its index:
    {
      station1Info = new SgVlbiStationInfo(stationsByName_.size(), station1Name);
      stationsByName_.insert(station1Info->getKey(), station1Info);
      stationsByIdx_.insert(station1Info->getIdx(), station1Info);
      station1Info->setCid(t202->baseline[0]);
      station1Info->setSid(t202->ref_intl_id[0], t202->ref_intl_id[1]);
    };
    // station #2:
    if (stationsByName_.contains(station2Name))
      station2Info = stationsByName_.value(station2Name);
    else // new station, add it to the container and register its index:
    {
      station2Info = new SgVlbiStationInfo(stationsByName_.size(), station2Name);
      stationsByName_.insert(station2Info->getKey(), station2Info);
      stationsByIdx_.insert(station2Info->getIdx(), station2Info);
      station2Info->setCid(t202->baseline[1]);
      station2Info->setSid(t202->rem_intl_id[0], t202->rem_intl_id[1]);
    };
    // source:
    if (sourcesByName_.contains(sourceName))
      sourceInfo = sourcesByName_.value(sourceName);
    else // new source, add it to the container and register its index:
    {
      sourceInfo = new SgVlbiSourceInfo(sourcesByName_.size(), sourceName);
      sourcesByName_.insert(sourceInfo->getKey(), sourceInfo);
      sourcesByIdx_.insert(sourceInfo->getIdx(), sourceInfo);
    };
    // baseline:
    if (baselinesByName_.contains(baselineName))
      baselineInfo = baselinesByName_.value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      baselineInfo = new SgVlbiBaselineInfo(baselinesByName_.size(), baselineName);
      baselinesByName_.insert(baselineInfo->getKey(), baselineInfo);
      baselinesByIdx_.insert(baselineInfo->getIdx(), baselineInfo);
    };
    // 4band:
    // band's station #1:
    if (band->stationsByName().contains(station1Name))
      bandStation1Info = band->stationsByName().value(station1Name);
    else // new station, add it to the container:
    {
      bandStation1Info = new SgVlbiStationInfo(station1Info->getIdx(), station1Name);
      band->stationsByName().insert(bandStation1Info->getKey(), bandStation1Info);
      band->stationsByIdx().insert(bandStation1Info->getIdx(), bandStation1Info);
    };
    // band's station #2:
    if (band->stationsByName().contains(station2Name))
      bandStation2Info = band->stationsByName().value(station2Name);
    else // new station, add it to the container:
    {
      bandStation2Info = new SgVlbiStationInfo(station2Info->getIdx(), station2Name);
      band->stationsByName().insert(bandStation2Info->getKey(), bandStation2Info);
      band->stationsByIdx().insert(bandStation2Info->getIdx(), bandStation2Info);
    };
    // band's source:
    if (band->sourcesByName().contains(sourceName))
      bandSourceInfo = band->sourcesByName().value(sourceName);
    else // new source, add it to the container:
    {
      bandSourceInfo = new SgVlbiSourceInfo(sourceInfo->getIdx(), sourceName);
      band->sourcesByName().insert(bandSourceInfo->getKey(), bandSourceInfo);
      band->sourcesByIdx().insert(bandSourceInfo->getIdx(), bandSourceInfo);
    };
    // band's baselines:
    if (band->baselinesByName().contains(baselineName))
      bandBaselineInfo = band->baselinesByName().value(baselineName);
    else // new baseline, add it to the container and register its index:
    {
      bandBaselineInfo = new SgVlbiBaselineInfo(baselineInfo->getIdx(), baselineName);
      band->baselinesByName().insert(bandBaselineInfo->getKey(), bandBaselineInfo);
      band->baselinesByIdx().insert(bandBaselineInfo->getIdx(), bandBaselineInfo);
    };
    //
    obs->setStation1Idx(station1Info->getIdx());
    obs->setStation2Idx(station2Info->getIdx());
    obs->setSourceIdx(sourceInfo->getIdx());
    obs->setBaselineIdx(baselineInfo->getIdx());
    obs->setApLength(vexInfo.apLength_);
    //
    // plus additional info:
    o->setFourfitOutputFName(fringeFileName);
    //
    // collect observables:
    // single band delay:

    o->sbDelay().setValue(1.0e-6*t208->tot_sbd_ref);
    // group delay:
    o->grDelay().setValue(1.0e-6*t208->tot_mbd_ref);
    // delay rate:
    o->phDRate().setValue(1.0e-6*t208->tot_rate_ref);
    //
    // workaround the DBH "feature":
    isSbdSigmaNan = isSbdSigmaInf = isGrdSigmaNan = isGrdSigmaInf = isPhrSigmaNan = isPhrSigmaInf 
      = false;
    // single band delay sigma (in microsec):
    f = t208->sbd_error;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the single band delay", true);
        isSbdSigmaInf = true;
      f = 1.0;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the single band delay", true);
      isSbdSigmaNan = true;
      f = 1.0;
    };
    o->sbDelay().setSigma(1.0e-6*f);
    //
    // group delay sigma (in seconds):
    f = t208->mbd_error;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isGrdSigmaInf = true;
      f = 1.0e-6;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isGrdSigmaNan = true;
      f = 1.0e-6;
    };
    o->grDelay().setSigma(1.0e-6*f);
    //
    // delay rate sigma:
    f = t208->rate_error;
    if (isinf(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (inf) for the group delay", true);
      isPhrSigmaInf = true;
      f = 1.0e-6;
    };
    if (isnan(f) != 0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
        " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
        epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
        " contains wrong sigma (nan) for the group delay", true);
      isPhrSigmaNan = true;
      f = 1.0e-6;
    };
    o->phDRate().setSigma(1.0e-6*f);
    // complain:
    if (isSbdSigmaNan || isSbdSigmaInf ||
        isGrdSigmaNan || isGrdSigmaInf ||
        isPhrSigmaNan || isPhrSigmaInf   )
    {
      QString                   s1(""), s2(""), s3("");
      s1 = (isSbdSigmaNan || isGrdSigmaNan || isPhrSigmaNan)?"(nan)":"(inf)";
      if (isSbdSigmaNan || isSbdSigmaInf)
        s2 = "single band delay, ";
      if (isGrdSigmaNan || isGrdSigmaInf)
        s2+= "group delay, ";
      if (isPhrSigmaNan || isPhrSigmaInf)
        s2+= "delay rate, ";
      s2 = s2.left(s2.size() - 2);
      if (o->getFourfitOutputFName().size())
        s3 = obs->getScanName().trimmed() + "/" + o->getFourfitOutputFName();

      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): The observation #" + QString("").setNum(obsIdx) +
        ", " + s3 + " contains wrong sigma " + s1 + " for " + s2 + ".");
    };
    //
    // single band:
    o->sbDelay().setResidualFringeFitting(1.0e-6*t208->resid_sbd); // sec
    // group delay:
    o->grDelay().setResidualFringeFitting(1.0e-6*t208->resid_mbd); // sec
    // delay rate:
    o->phDRate().setResidualFringeFitting(1.0e-6*t208->resid_rate); // sec per sec
    //
    //
    o->grDelay().setAmbiguitySpacing(1.0e-6*t208->ambiguity); // usec->sec
    str = t208->quality;
    nTmp = str.toInt(&isTmp);
    o->setQualityFactor(isTmp ? nTmp : 0);
    o->setErrorCode(QString(t208->errcode));
    //
    memset(buff1, 0, 32);
    strncpy(buff1, t208->tape_qcode, sizeof(t208->tape_qcode)<32?sizeof(t208->tape_qcode):31);
    o->setTapeQualityCode(buff1);
    //
    o->setReferenceFrequency((refFreq=t205->ref_freq));
    o->setCorrCoeff(t208->amplitude);
    o->setTotalPhase(t208->totphase_ref*DEG2RAD);         // in radians
    o->setIncohChanAddAmp(t208->inc_chan_ampl);
    o->setIncohSegmAddAmp(t208->inc_seg_ampl);
    o->setSnr(t208->snr);
    o->setProbabOfFalseDetection(t208->prob_false);
    o->setGeocenterTotalPhase(t208->totphase*DEG2RAD);    // in radians
    o->setGeocenterResidPhase(t208->resphase*DEG2RAD);    // in radians
    o->setAprioriDra(0, 1.0e-6*t208->adelay);
    o->setAprioriDra(1, 1.0e-6*t208->arate);
    o->setAprioriDra(2, 1.0e-6*t208->aaccel);
    // single band:
    o->sbDelay().setGeocenterValue(1.0e-6*t208->tot_sbd); // sec
    // group delay:
    o->grDelay().setGeocenterValue(1.0e-6*t208->tot_mbd); // sec
    // delay rate:
    o->phDRate().setGeocenterValue(1.0e-6*t208->tot_rate);// sec per sec
    //
    // VGOS contribution:
    str  = QString(t208->version_no[0]);
    str += QString(t208->version_no[1]);
    nTmp = str.toInt(&isTmp);
    if (isTmp)
    {
      if (nTmp > 0)
      {
        obs->setDTec(t201->dispersion);
        f = t208->tec_error;
//      if (!isnormal(f))
        if (!isfinite(f))
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processFringeFile(): the observation #" + QString("").setNum(obsIdx) +
            " at the baseline <" + baselineName + "> observing " + sourceName + " source at " +
            epoch.toString(SgMJD::F_YYYYMMDDHHMMSSSS) +
            " contains unnormal value (" + QString("").setNum(t208->tec_error) + ") of diffTEC sigma" +
            "; corrected to 1e3", true);
          f = 1.0e3;
        };
        obs->setDTecStdDev(f);
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): cannot convert version number [" + str + 
        "] to integer type: " + o->strId());
    //
    //
    // alloc auxiliary data:
    if (!station1Info->auxObservationByScanId()->contains(scanId)) // new scan, insert data:
    {
      auxObs_1 = new SgVlbiAuxObservation;
      auxObs_1->setMJD(epoch);
      auxObs_1->setAzimuthAngle   (t202->ref_az   *DEG2RAD);
      auxObs_1->setElevationAngle (t202->ref_elev *DEG2RAD);
      auxObs_1->setEstZenithDelay (t202->ref_zdelay*1.0e-6); // musec -> sec
      auxObs_1->setTapeId(QString("").sprintf("%.8s", t202->ref_tape));
      station1Info->auxObservationByScanId()->insert(scanId, auxObs_1);
    }
    else
      auxObs_1 = station1Info->auxObservationByScanId()->value(scanId);
    if (!station2Info->auxObservationByScanId()->contains(scanId)) // new scan, insert data:
    {
      auxObs_2 = new SgVlbiAuxObservation;
      auxObs_2->setMJD(epoch);
      auxObs_2->setAzimuthAngle   (t202->rem_az   *DEG2RAD);
      auxObs_2->setElevationAngle (t202->rem_elev *DEG2RAD);
      auxObs_2->setEstZenithDelay (t202->rem_zdelay*1.0e-6); // musec -> sec
      auxObs_2->setTapeId(QString("").sprintf("%.8s", t202->rem_tape));
      station2Info->auxObservationByScanId()->insert(scanId, auxObs_2);
    }
    else
      auxObs_2 = station2Info->auxObservationByScanId()->value(scanId);
    //
    // aux data:
    o->setStartOffset(t200->start_offset);
    o->setStopOffset (t200->stop_offset);
    o->setCentrOffset(t205->offset);
    o->setEffIntegrationTime(t206->intg_time);
    o->setAcceptedRatio(t206->accept_ratio);
    o->setDiscardRatio(t206->discard);

    o->setCorrelStarElev_1(t202->ref_elev *DEG2RAD);
    o->setCorrelStarElev_2(t202->rem_elev *DEG2RAD);
    o->setCorrelZdelay_1  (t202->ref_zdelay*1.0e-6); // musec -> sec
    o->setCorrelZdelay_2  (t202->rem_zdelay*1.0e-6); // musec -> sec

    // set up epochs of events:
    // correlation:
    epoch.setUpEpoch( t200->corr_date.year, 0,
                      t200->corr_date.day, 
                      t200->corr_date.hour, 
                      t200->corr_date.minute, 
                      t200->corr_date.second);
    o->setEpochOfCorrelation(epoch);
    // fringing:
    epoch.setUpEpoch( t200->fourfit_date.year, 0,
                      t200->fourfit_date.day, 
                      t200->fourfit_date.hour, 
                      t200->fourfit_date.minute, 
                      t200->fourfit_date.second);
    o->setEpochOfFourfitting(epoch);
    // nominal scan time:
    epoch.setUpEpoch( t200->scantime.year, 0,
                      t200->scantime.day, 
                      t200->scantime.hour, 
                      t200->scantime.minute, 
                      t200->scantime.second);
    o->setEpochOfScan(epoch);
    //
    // "central" scan epoch:
    epoch.setUpEpoch( t205->utc_central.year, 0,
                      t205->utc_central.day, 
                      t205->utc_central.hour, 
                      t205->utc_central.minute, 
                      t205->utc_central.second);
    o->setEpochCentral(epoch);
    // and another couple of epochs:
    // "start":
    epoch.setUpEpoch( t205->start.year, 0,
                      t205->start.day, 
                      t205->start.hour, 
                      t205->start.minute, 
                      t205->start.second);
    o->setTstart(epoch);
    // "stop":
    epoch.setUpEpoch( t205->stop.year, 0,
                      t205->stop.day, 
                      t205->stop.hour, 
                      t205->stop.minute, 
                      t205->stop.second);
    o->setTstop(epoch);
    //
    // sample rate (mimic read_mk4.f):
    bool                        hasStn_1Info=false, hasStn_2Info=false;
    StationInfo                 si_1, si_2;
    if (stnsInfo.contains(station1Name))
    {
      si_1 = stnsInfo[station1Name];
      hasStn_1Info = true;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the station#1, " + station1Name + ", was not found in the VEX file");
    if (stnsInfo.contains(station2Name))
    {
      si_2 = stnsInfo[station2Name];
      hasStn_2Info = true;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): the station#2, " + station2Name + ", was not found in the VEX file");
    if (hasStn_1Info && hasStn_2Info && si_1.sampleRate_ != si_2.sampleRate_)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): stations sample rates mismatch: " +
        QString("").sprintf("%g vs %g.", si_1.sampleRate_, si_2.sampleRate_) + ": " + o->strId());
    };
    f = t203->channels[0].sample_rate*1.0e3;
    if (hasStn_1Info && fabs(si_1.sampleRate_ - f)>0.0 && si_1.sampleRate_>0.0)
    {
      if (t204->ff_version[0] <= 3)
        f = si_1.sampleRate_;
      else
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): OVEX and Fringe sample rates mismatch: " + 
          QString("").sprintf("%g vs %g.", si_1.sampleRate_, f) + ": " + o->strId());
    };
    o->setSampleRate(f);
    //
    if (hasStn_1Info && hasStn_2Info && si_1.bits_sample_ != si_2.bits_sample_)
      logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
        "::processFringeFile(): stations bits per sample mismatch: " +
        QString("").sprintf("%d vs %d", si_1.bits_sample_, si_2.bits_sample_) + ": " + o->strId());
    if (hasStn_1Info)
      o->setBitsPerSample(si_1.bits_sample_);
    //
    // collect data necessary to evaluate the effective frequencies (for ionospheric correction):
    isTmp = ( t206->version_no[0]=='0' && 
              t206->version_no[1]=='0'    ); // this version does not contain weights
    for (int i=0; i<numOfChannels; i++)
    {
      int                       idx=t205->ffit_chan[i].channels[0];
      if (idx==-1)
      {
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): first channel index is -1, switch to the second one");
        idx=t205->ffit_chan[i].channels[1];
      }; 
      o->numOfAccPeriodsByChan_USB()->setElement(i, t206->accepted[i].usb);
      o->numOfAccPeriodsByChan_LSB()->setElement(i, t206->accepted[i].lsb);
      o->numOfSamplesByChan_USB()->setElement   (i, isTmp?0.0:t206->weights[i].usb);
      o->numOfSamplesByChan_LSB()->setElement   (i, isTmp?0.0:t206->weights[i].lsb);
      o->refFreqByChan()->setElement            (i, t203->channels[idx].ref_freq*1.0e-6); // ->MHz
      (*o->polarization_1ByChan())[i] = t203->channels[idx].refpol;
      (*o->polarization_2ByChan())[i] = t203->channels[idx].rempol;

/*
std::cout 
<< "scanId= " << qPrintable(scanId) << " "
<< "i= " << i << " "
<< "Stn #1: " << qPrintable(station1Name) << " "
<< "(*o->polarization_1ByChan())[i]= " << (*o->polarization_1ByChan())[i] << " "
<< "Stn #2: " << qPrintable(station2Name) << " "
<< "(*o->polarization_2ByChan())[i]= " << (*o->polarization_2ByChan())[i] << " "
<< "\n";
*/

      o->fringeAmplitudeByChan()->setElement    (i, t210->amp_phas[i].ampl);
      o->fringePhaseByChan()->setElement        (i, t210->amp_phas[i].phase*DEG2RAD);
      // phase calibration data:
      o->phaseCalData_1ByChan()->setElement (0,i, t207->ref_pcamp   [i].usb*1.0e4);
      o->phaseCalData_2ByChan()->setElement (0,i, t207->rem_pcamp   [i].usb*1.0e4);
      o->phaseCalData_1ByChan()->setElement (1,i, t207->ref_pcphase [i].usb*DEG2RAD);
      o->phaseCalData_2ByChan()->setElement (1,i, t207->rem_pcphase [i].usb*DEG2RAD);
      o->phaseCalData_1ByChan()->setElement (2,i, t207->ref_pcfreq  [i].usb*1.0e-3);
      o->phaseCalData_2ByChan()->setElement (2,i, t207->rem_pcfreq  [i].usb*1.0e-3);
      o->phaseCalData_1ByChan()->setElement (3,i, t207->ref_pcoffset[i].usb*DEG2RAD);
      o->phaseCalData_2ByChan()->setElement (3,i, t207->rem_pcoffset[i].usb*DEG2RAD);
      o->phaseCalData_1ByChan()->setElement (4,i, t207->ref_errate  [i]);
      o->phaseCalData_2ByChan()->setElement (4,i, t207->rem_errate  [i]);
      // check and rescale phase cal data
      // compare usb and lsb pcal's:
      // amps:
      if (fabs(t207->ref_pcamp[i].usb - t207->ref_pcamp[i].lsb) > 1.0e-8)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): reference station, sidebands have different phase cal amplitudes: " +
          QString("").sprintf("%g vs %g, diff=%g", t207->ref_pcamp[i].usb, t207->ref_pcamp[i].lsb, 
            fabs(t207->ref_pcamp[i].usb - t207->ref_pcamp[i].lsb)));
      if (fabs(t207->rem_pcamp[i].usb - t207->rem_pcamp[i].lsb) > 1.0e-8)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): remote station, sidebands have different phase cal amplitudes: " +
          QString("").sprintf("%g vs %g, diff=%g", t207->rem_pcamp[i].usb, t207->rem_pcamp[i].lsb, 
            fabs(t207->rem_pcamp[i].usb - t207->rem_pcamp[i].lsb)));
      // phases:
      if (fabs(t207->ref_pcphase[i].usb - t207->ref_pcphase[i].lsb) > 1.0e-8)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): reference station, sidebands have different phase cal phases: " +
          QString("").sprintf("%g vs %g, diff=%g", t207->ref_pcphase[i].usb, t207->ref_pcphase[i].lsb, 
            fabs(t207->ref_pcphase[i].usb - t207->ref_pcphase[i].lsb)));
      if (fabs(t207->rem_pcphase[i].usb - t207->rem_pcphase[i].lsb) > 1.0e-8)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): remote station, sidebands have different phase cal phases: " +
          QString("").sprintf("%g vs %g, diff=%g", t207->rem_pcphase[i].usb, t207->rem_pcphase[i].lsb, 
            fabs(t207->rem_pcphase[i].usb - t207->rem_pcphase[i].lsb)));
      if (t207->ref_errate[i] != 0.0)
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
          "::processFringeFile(): reference station has a non-zero pcal error rate: " +
          QString("").sprintf("%g", t207->ref_errate[i]));
      if (t207->rem_errate[i] != 0.0)
        logger->write(SgLogger::INF, SgLogger::IO_DBH, className() +
          "::processFringeFile(): remote station has a non-zero pcal error rate: " +
          QString("").sprintf("%g", t207->ref_errate[i]));
      //
      // ok, continue:
      (*o->chanIdByChan())    [i] = t205->ffit_chan[i].ffit_chan_id;
      (*o->corelIndexNumUSB())[i] = t205->ffit_chan[i].channels[0]>-1 ?
                                              t203->channels[t205->ffit_chan[i].channels[0]].index:-1;
      (*o->corelIndexNumLSB())[i] = t205->ffit_chan[i].channels[1]>-1 ?
                                              t203->channels[t205->ffit_chan[i].channels[1]].index:-1;
      //
      // station #1:
      str = t203->channels[idx].ref_chan_id;
      if (hasStn_1Info)
      {
        if (si_1.channelByName_.contains(str))
        {
          const ChannelInfo      &ci=si_1.channelByName_.value(str);
          o->loFreqByChan_1()->setElement(i, ci.if_total_lo_);
          o->channelBandwidth()->setElement(i, ci.bandwidth_*1.0e-6); // Hz -> MHz
          (*o->bbcIdxByChan_1())[i] = ci.bbc_no_;
        }
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processFringeFile(): cannot find channel ID [" + str + "] in the corel root file " +
            vexFileName + " for the station " + station1Info->getKey());
        };
      };
      // station #2:
      str = t203->channels[idx].rem_chan_id;
      if (hasStn_2Info)
      {
        if (si_2.channelByName_.contains(str))
        {
          const ChannelInfo      &ci=si_2.channelByName_.value(str);
          o->loFreqByChan_2()->setElement(i, ci.if_total_lo_);
          if (0.0 < o->channelBandwidth()->getElement(i) && 
                    o->channelBandwidth()->getElement(i) != ci.bandwidth_*1.0e-6)
            logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
              "::processFringeFile(): channel bandwidth for " + str + " is different for " + 
              station1Info->getKey() + " and " + station2Info->getKey() + " in the corel root file " + 
              vexFileName);
          o->channelBandwidth()->setElement(i, ci.bandwidth_*1.0e-6);
          (*o->bbcIdxByChan_2())[i] = ci.bbc_no_;
        }
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processFringeFile(): cannot find channel ID [" + str + "] in the corel root file " +
            vexFileName + " for the station " + station2Info->getKey());
        };
      };
      //
      //
      //
    };
    // check and rescale phase cal data
    for (int i=0; i<numOfChannels; i++)
    {
      for (int j=0; j<5; j++)
      {
        if (isnan(o->phaseCalData_1ByChan()->getElement (j,i)))
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processFringeFile(): st1 one of phase cal values is NAN " + 
            QString("").sprintf("(chan=%d:dat=%d)", i, j) + " from the scan " + obs->getScanName() + 
            " fringefile " + fringeFileName);
          o->phaseCalData_1ByChan()->setElement (j,i, -100.0);
        };
        if (isnan(o->phaseCalData_2ByChan()->getElement (j,i)))
        {
          logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
            "::processFringeFile(): st2 one of phase cal values is NAN " + 
            QString("").sprintf("(chan=%d:dat=%d)", i, j) + " from the scan " + obs->getScanName() + 
            " fringefile " + fringeFileName);
          o->phaseCalData_2ByChan()->setElement (j,i, -100.0);
        };
      };
    };
    //
    evaluateEffectiveFreqs(*o->numOfAccPeriodsByChan_USB(), *o->numOfAccPeriodsByChan_LSB(), 
      *o->refFreqByChan(), *o->fringeAmplitudeByChan(), 
      *o->numOfSamplesByChan_USB(), *o->numOfSamplesByChan_LSB(),
      *o->channelBandwidth(), refFreq, numOfChannels,
      effFreq4GR, effFreq4PH, effFreq4RT, getCorrelatorType(), o);
    o->grDelay().setEffFreq(effFreq4GR);
    o->phDelay().setEffFreq(effFreq4PH);
    o->phDRate().setEffFreq(effFreq4RT);
    o->setPhaseCalModes(t207->pcal_mode);
    //
    //
    o->setFourfitVersion(0, t204->ff_version[0]);
    o->setFourfitVersion(1, t204->ff_version[1]);
    o->setFourfitControlFile(t204->control_file);
    o->setFourfitCommandOverride(t204->override);
    //
    o->setPhaseCalRates(0, t207->ref_pcrate*1.0e-6);
    o->setPhaseCalRates(1, t207->rem_pcrate*1.0e-6);
    o->setUvFrPerAsec(0, t202->u);
    o->setUvFrPerAsec(1, t202->v);
    o->setUrVr(0, t202->uf);
    o->setUrVr(1, t202->vf);
    o->setCorrClocks(0, 0, t202->ref_clock*1.0e-6); // musec -> sec
    o->setCorrClocks(0, 1, t202->rem_clock*1.0e-6); // musec -> sec
    o->setCorrClocks(1, 0, t202->ref_clockrate);
    o->setCorrClocks(1, 1, t202->rem_clockrate);
    o->setInstrDelay(0, t202->ref_idelay*1.0e-6); // musec -> sec
    o->setInstrDelay(1, t202->rem_idelay*1.0e-6); // musec -> sec
    o->setNlags(t202->nlags);
    for (int i=0; i<6; i++)
      o->setFourfitSearchParameters(i, t205->search[i]);
    o->setHopsRevisionNumber(t200->software_rev[0]);
    // check the rest of array, if the fields are non-zero, complain:
    for (unsigned int i=1; i<sizeof(type_200::software_rev)/sizeof(short); i++)
      if (t200->software_rev[i] != 0)
        logger->write(SgLogger::WRN, SgLogger::IO_DBH, className() +
          "::processFringeFile(): type200's software revision has more than one meaningful field at " + 
          "the scan " + obs->getScanName() + " fringefile " + fringeFileName);

    //
    // per band statistics:
    bandStation1Info->incNumTotal(DT_DELAY);
    bandStation2Info->incNumTotal(DT_DELAY);
    bandSourceInfo->incNumTotal(DT_DELAY);
    bandBaselineInfo->incNumTotal(DT_DELAY);

    bandStation1Info->incNumTotal(DT_RATE);
    bandStation2Info->incNumTotal(DT_RATE);
    bandSourceInfo->incNumTotal(DT_RATE);
    bandBaselineInfo->incNumTotal(DT_RATE);
    //
    clear_mk4fringe(&fringe);
  };
  //
  // clear stuff:
  delete[] filename;
};



//
bool SgVlbiSession::getCorrelatorHistory(const QString& fileName)
{
  QString                       str, strAux;
  QFile                         f(fileName);
  if (!bands_.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::getCorrelatorHistory(): the session has no any band");
    return false;
  };
  SgVlbiHistory                &history=bands_.at(0)->history();
  if (!f.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::getCorrelatorHistory(): the history file [" + f.fileName() + "] does not exist");
    return false;
  }
  else
  {
    if (f.open(QFile::ReadOnly))
    {
      QTextStream               s(&f);
      int                       numOfStrs(0);
      QFileInfo                 fi(f.fileName());
      QDateTime                 d(fi.lastModified());

//std::cout << " fi.lastModified(): " << qPrintable(fi.lastModified().toString()) << "\n";
//std::cout << " fi.created     (): " << qPrintable(fi.created().toString()) << "\n";

      // dealing with Qt's bug: in the current version of Qt QFileInfo::created() returns last modified
      // epoch, while QFileInfo::lastModified() reports time of creation (hope, they will fix it later):
      if (d < fi.created())
        d = fi.created();
      d = d.toUTC(); // keep history records in UTC
      // check the last modified epoch, sometimes when clocks at comuters are not synchronized,
      // it can show time that is ahead of the current time:
      QDateTime                 ct(QDateTime::currentDateTimeUtc());
      SgMJD                     tCreated( d.date().year(), d.date().month(), d.date().day(),
                                          d.time().hour(), d.time().minute(),
                                          d.time().second()+d.time().msec()*0.001);
      SgMJD                     tCurrent( ct.date().year(), ct.date().month(), ct.date().day(),
                                          ct.time().hour(), ct.time().minute(),
                                          ct.time().second()+ct.time().msec()*0.001);
      if (tCurrent < tCreated) // the file was created in future:
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::getCorrelatorHistory(): the correlator report file has the `last modified time' that "
          "is from future, " + tCreated.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ", the current time is: " +
          tCurrent.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ". The difference is " + 
          interval2Str(tCreated - tCurrent));
        tCreated = tCurrent - 60.0/DAY2SEC;
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::getCorrelatorHistory(): the `last modified time' of the correlator report file "
          "has been adjuted to " + tCreated.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
      };
      while (!s.atEnd())
      {
        str = s.readLine();
        history.append(new SgVlbiHistoryRecord(tCreated, 1, str));
        numOfStrs++;
      };
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::getCorrelatorHistory(): read " + QString("").setNum(numOfStrs) + 
        " strings from the history file [" + f.fileName() + "]");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::getCorrelatorHistory(): cannot open the history file [" + f.fileName() + "] for read access");
      return false;
    };
  };
  // propagate to other bands:
  if (bands_.size() > 1)
  {
    for (int bIdx=1; bIdx<bands_.size(); bIdx++)
    {
      SgVlbiHistory            &historyDest=bands_.at(bIdx)->history();
      for (int i=0; i<history.size(); i++)
        historyDest.append(new SgVlbiHistoryRecord(history.at(i)->getEpoch(), 
          history.at(i)->getVersion(), history.at(i)->getText()));
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::getCorrelatorHistory(): the history records were propagated to the " + 
        bands_.at(bIdx)->getKey() + "-band");
    };
  }
  return true;
};





#endif // !defined HAVE_MK4_DATA_H || !defined HAVE_MK4_DFIO_H || !defined HAVE_MK4_VEX_H
/*=====================================================================================================*/




/*=====================================================================================================*/
