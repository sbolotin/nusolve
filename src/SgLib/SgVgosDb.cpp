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
#include <netcdf.h>


#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>



#include <SgIdentities.h>
#include <SgLogger.h>
#include <SgVgosDb.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>
#include <SgVlbiStationInfo.h>




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVdbVariable implementation                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgVdbVariable::className()
{
  return "SgVdbVariable";
};



//
SgVdbVariable::SgVdbVariable(const QString& stdStub, const QString& stdSubDir) :
  stub_(""),
  type_(""),
  band_(""),
  kind_(""),
  institution_(""),
  stdStub_(stdStub),
  currentInstitution_(""),
  subDir_(""),
  fileName_(""),
  stdSubDir_(stdSubDir),
  fileName4Output_("")
{
  version_ = 0;
  have2adjustPermissions_ = true;
};



//
SgVdbVariable::SgVdbVariable(const SgVdbVariable& v) :
  stub_(v.getStub()),
  type_(v.type_),
  band_(v.getBand()),
  kind_(v.getKind()),
  institution_(v.getInstitution()),
  stdStub_(v.stdStub_),
  currentInstitution_(v.getCurrentInstitution()),
  subDir_(v.getSubDir()),
  fileName_(v.getFileName()),
  stdSubDir_(v.stdSubDir_),
  fileName4Output_(v.fileName4Output_)
{
  version_ = v.getVersion();
  have2adjustPermissions_ = v.have2adjustPermissions_;
};



//
QString SgVdbVariable::compositeName() const
{
  QString                       str(stub_);
  if (type_.size())
    str += "_t" + type_;
  if (kind_.size())
    str += "_k" + kind_;
  if (band_.size())
    str += "_b" + band_;
  if (institution_.size())
    str += "_i" + institution_;
  if (version_>0)
    str += "_V" + QString("").sprintf("%03d", version_);
  return str;
};



//
void SgVdbVariable::parseString(const QString& str)
{
  QStringList                   fields=str.split("_");
  if (!fields.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseString(): the list of fields is empty, the string: [" + str + "]");
    return;
  };
  stub_ = fields.at(0);
  for (int i=1; i<fields.size(); i++)
  {
    if (fields.at(i).left(1).contains("t", Qt::CaseInsensitive))
      type_ = fields.at(i).mid(1);
    else if (fields.at(i).left(1).contains("b", Qt::CaseInsensitive))
      band_ = fields.at(i).mid(1);
    else if (fields.at(i).left(1).contains("k", Qt::CaseInsensitive))
      kind_ = fields.at(i).mid(1);
    else if (fields.at(i).left(1).contains("i", Qt::CaseInsensitive))
      institution_ = fields.at(i).mid(1);
    else if (fields.at(i).left(1).contains("v", Qt::CaseInsensitive))
    {
      bool                      isOk;
      version_ = fields.at(i).mid(1).toInt(&isOk);
      if (!isOk)
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::parseString(): cannot convert version to int: [" + fields.at(i).mid(1) + "]");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::parseString(): unparsed part of the name: [" + fields.at(i) + "]");
  };
};



//
bool SgVdbVariable::isEmpty() const
{
  return stub_.isEmpty();
};



//
QString SgVdbVariable::name4export(const QString& rootDir, SgNetCdf::OperationMode om, 
  const QString& aBand)
{
  QString                       str("");
  if (stub_.size())
    str = stub_;
  else
  {
    str = stdStub_;
    stub_ = stdStub_;
  };
  if (type_.size())
    str += "_t" + type_;
  if (kind_.size())
    str += "_k" + kind_;
  if (band_.size())
    str += "_b" + band_;
  else if (aBand.size())
    str += "_b" + aBand;
  if (institution_.size() && currentInstitution_.size())
    str += "_i" + currentInstitution_;
  
  // check existing file:
  QString                       path2file(rootDir), baseStr(str), actualSubDir(subDir_);

  if (!actualSubDir.size())
  {
    actualSubDir = stdSubDir_;
    subDir_ = stdSubDir_;
  };
  if (actualSubDir.size())
  {
    actualSubDir += "/";
    path2file += "/" + actualSubDir;
  }
  else
    path2file += "/";
  //
  // if it is necessary, create a subdir:
  if (om == SgNetCdf::OM_REGULAR) // regular output:
  {
    QDir                        dir(path2file);
    if (!dir.exists())
    {
//    if (!dir.mkpath(path2file))
      if (!dir.mkpath(dir.absolutePath()))
        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
          "::name4export(): cannot create a path to the directory \"" + dir.absolutePath() + "\"");
      else
      {
        logger->write(SgLogger::DBG, SgLogger::IO, className() + 
          "::name4export(): a directory \"" + dir.absolutePath() + "\" has been created");
        if (have2adjustPermissions_)
        {
          QFile::Permissions      perm=QFile::permissions(path2file);
          if (!QFile::setPermissions(path2file, perm | QFile::WriteGroup | QFile::ExeGroup))
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
              "::name4export(): cannot change permissions of the directory \"" + path2file + "\"");
        };
      };
    };
  };
  //
  int                           n=version_;
  while (QFile(path2file + str + ".nc").exists() && n<1000000)
    str.sprintf("%s_V%03d", qPrintable(baseStr), ++n);
  if (n>=1000000)
    logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
          "::name4export(): version overflow: " + compositeName());
  //
  fileName4Output_ = str + ".nc";
  return actualSubDir + fileName4Output_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgVgosDb implementation                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
// static first:
const QString SgVgosDb::className()
{
  return "SgVgosDb";
};



//
SgVgosDb::StationDescriptor::StationDescriptor() :
  stationName_(""),
  stationKey_(""),
  vTimeUTC_("TimeUTC", ""),
  vMet_("Met", ""),
  vAzEl_("AzEl", ""),
  vFeedRotation_("FeedRotation", ""),
  vCal_AxisOffset_("Cal-AxisOffset", ""),
  vCal_Cable_("Cal-Cable", ""),
  vCal_CableCorrections_("Cal-CableCorrections", ""), // tmp
  vCal_CblCorrections_("Cal-CblCorrections", ""),
  vCal_SlantPathTropDry_("Cal-SlantPathTropDry", ""),
  vCal_SlantPathTropWet_("Cal-SlantPathTropWet", ""),
  vCal_OceanLoad_("Cal-StationOceanLoad", ""),
  vPart_AxisOffset_("Part-AxisOffset", ""),
  vPart_ZenithPathTropDry_("Part-ZenithPathTropDry", ""),
  vPart_ZenithPathTropWet_("Part-ZenithPathTropWet", ""),
  vPart_HorizonGrad_("Part-HorizonGrad", ""),
  vDis_OceanLoad_("Dis-OceanLoad", ""),
  vRefClockOffset_("RefClockOffset", ""),
  vTsys_("Tsys", "")
{
  numOfPts_ = 0;
};



//
void SgVgosDb::StationDescriptor::propagateStnKey()
{
  vTimeUTC_.setSubDir(stationKey_); 
  vMet_.setSubDir(stationKey_);
  vAzEl_.setSubDir(stationKey_);
  vFeedRotation_.setSubDir(stationKey_);
  vCal_AxisOffset_.setSubDir(stationKey_);
  vCal_Cable_.setSubDir(stationKey_);
  vCal_CableCorrections_.setSubDir(stationKey_); // tmp
  vCal_CblCorrections_.setSubDir(stationKey_);
  vCal_SlantPathTropDry_.setSubDir(stationKey_);
  vCal_SlantPathTropWet_.setSubDir(stationKey_);
  vCal_OceanLoad_.setSubDir(stationKey_);
  vPart_AxisOffset_.setSubDir(stationKey_);
  vPart_ZenithPathTropDry_.setSubDir(stationKey_); 
  vPart_ZenithPathTropWet_.setSubDir(stationKey_);
  vPart_HorizonGrad_.setSubDir(stationKey_);
  vDis_OceanLoad_.setSubDir(stationKey_);
  vRefClockOffset_.setSubDir(stationKey_);
  vTsys_.setSubDir(stationKey_);
};



//
SgVgosDb::HistoryDescriptor::HistoryDescriptor() :
  processName_(""),
  epochOfCreation_(tZero),
  creator_(""),
  defaultDir_("History"),
  historyFileName_(""),
  inputWrapperFileName_(""),
  version_("")
{
  isMk3Compatible_ = false;
};



//
SgVgosDb::ProgramGenericDescriptor::ProgramGenericDescriptor() :
  programName_(""),
  content_()
{
};



//
SgVgosDb::ProgramSolveDescriptor::ProgramSolveDescriptor() :
  programName_("Solve"),
  // Session:
  vCalcInfo_("CalcInfo", "Solve"),
  vCalibrationSetup_("CalibrationSetup", "Solve"),
  vAtmSetup_("AtmSetup", "Solve"), 
  vClockSetup_("ClockSetup", "Solve"), 
  vErpSetup_("ERPSetup", "Solve"), 
  vIonoSetup_("IonoSetup", "Solve"),   
  vCalcErp_("CalcERP", "Solve"), 
  vBaselineClockSetup_("BaselineClockSetup", "Solve"),
  vSelectionStatus_("SelectionStatus", "Solve"),
  // Scan:
  vScanTimeMJD_("ScanTimeMJD", "Solve"),
  // Observation:
  vIonoBits_("IonoBits", "Solve"), 
  vFractC_("FractC", "Solve"),
  vUnPhaseCalFlag_("UnPhaseCalFlag", "Solve"),
  vUserSup_("UserSup", "Solve"),
  vdbVars_()
{
  vdbVars_  << &vCalcInfo_ << &vCalibrationSetup_ << &vAtmSetup_ << &vClockSetup_ 
            << &vErpSetup_ << &vIonoSetup_ << &vCalcErp_ << &vBaselineClockSetup_ << &vSelectionStatus_
            << &vScanTimeMJD_ << &vIonoBits_ << &vFractC_ << &vUnPhaseCalFlag_ << &vUserSup_;
};



//
bool SgVgosDb::ProgramSolveDescriptor::hasSomething4output()
{
  bool                          has=false;
  for (int i=0; i<vdbVars_.size(); i++)
    if (!vdbVars_.at(i)->isEmpty())
      has = true;
  return has;
};



//
SgVgosDb::BandData::BandData() :
  bandName_(""),
  //
  vChannelInfo_           ("ChannelInfo",             "Observables"),
  vPhaseCalInfo_          ("PhaseCalInfo",            "Observables"),
  vCorrInfo_              ("CorrInfo",                "Observables"),
  vQualityCode_           ("QualityCode",             "Observables"),
  vRefFreq_               ("RefFreq",                 "Observables"),
  vAmbigSize_             ("AmbigSize",               "Observables"),
  vSNR_                   ("SNR",                     "Observables"),
  vCorrelation_           ("Correlation",             "Observables"),
  vPhase_                 ("Phase",                   "Observables"),
  vSBDelay_               ("SBDelay",                 "Observables"),
  vGroupDelay_            ("GroupDelay",              "Observables"),
  vGroupRate_             ("GroupRate",               "Observables"),
  vDelayDataFlag_         ("DelayDataFlag",           "Observables"),
  vDataFlag_              ("DataFlag",                "Observables"),
  //
//  vPhaseDelayFull_        ("PhaseDelayFull",          "ObsDerived"), ???
  vPhaseDelayFull_        ("PhaseDelayFull",          "ObsEdit"),
  vUVFperAsec_            ("UVFperAsec",              "ObsDerived"),
  vEffFreq_EqWt_          ("EffFreq_kEqWt",           "ObsDerived"),
  vEffFreq_               ("EffFreq",                 "ObsDerived"),
  vCal_SlantPathIonoGroup_("Cal-SlantPathIonoGroup",  "ObsDerived"),
  //
  vCal_FeedCorrection_    ("Cal-FeedCorrection",      "ObsCalTheo"),
  vCal_Unphase_           ("Cal-Unphase",             "ObsCalTheo"),
  //
  vNumGroupAmbig_         ("NumGroupAmbig",           "ObsEdit"),
  vNumPhaseAmbig_         ("NumPhaseAmbig",           "ObsEdit"),
  vGroupDelayFull_        ("GroupDelayFull",          "ObsEdit"),
  //
  vMiscFourFit_           ("MiscFourFit",             "Session") 
{
};



//
SgVgosDb::SgVgosDb(SgIdentities *ids, SgVersion *driverVersion) :
  SgIoDriver(ids, driverVersion),
  activeVars_(), path2RootDir_(""), wrapperFileName_(""), 
  corrTypeId_(""), inputFormatEpoch_(tZero), 
//  outputFormatEpoch_(2016, 4, 2),
  outputFormatEpoch_(2017, 10, 2),
  outputFormatId_(""),
//historyDescriptorByName_(), localHistory_(),
  historyDescriptors_(), localHistory_(),
  lastModified_(tZero),
//  creator_(""), software4creation_(""), historyFileName_(""), historyFileName4Output_(""), 
  sessionCode_(""), sessionName_(""),
  // section SESSION:
  vHead_("Head", ""), vGroupBLWeights_("GroupBLWeights", "Session"), 
  vClockBreak_("ClockBreak", "Session"), vLeapSecond_("LeapSecond", "Session"), 
  vMiscFourFit_("MiscFourFit", "Session"), vMiscCable_("MiscCable", "Session"), 
  vStationApriori_("Station", "Apriori"), vSourceApriori_("Source", "Apriori"),
  vClockApriori_("Clock", "Apriori"), 
  vAntennaApriori_("Antenna", "Apriori"), vEccentricity_("Eccentricity", "Apriori"),
  vStationCrossRef_("StationCrossRef", "CrossReference"),
  vSourceCrossRef_("SourceCrossRef", "CrossReference"),
  // section STATION:
  stnDescriptorByKey_(),
  stnDescriptorByName_(),
  // section SCAN:
  vScanTimeUTC_("TimeUTC", "Scan"), vScanName_("ScanName", "Scan"), vErpApriori_("ERPApriori", "Scan"),
  vEphemeris_("Ephemeris", "Scan"), vNutationEqx_kWahr_("NutationEQX_kWahr", "Scan"), 
  vNutationEqx_("NutationEQX", "Scan"), 
  vNutationNro_("NutationNRO", "Scan"), 
  vRot_CF2J2K_("Rot-CF2J2K", "Scan"), 
  vCorrRootFile_("CorrRootFile", "Scan"),
  // section OBSERVATION:
  vObservationTimeUTC_("TimeUTC", "Observables"), vBaseline_("Baseline", "Observables"), 
  vSource_("Source", "Observables"),
  //
  //
  bandDataByName_(),
  //
  //
  vDiffTec_("DiffTec", "Observables"),
  //
  vCal_Bend_("Cal-Bend", "ObsCalTheo"), vCal_BendSun_("Cal-BendSun", "ObsCalTheo"),
  vCal_BendSunHigher_("Cal-BendSunHigher", "ObsCalTheo"), 
  vCal_EarthTide_("Cal-EarthTide", "ObsCalTheo"), 
  vCal_FeedCorrection_("Cal-FeedCorrection", "ObsCalTheo"),
  vCal_HfErp_("Cal-HiFreqERP", "ObsCalTheo"),
  vCal_OceanLoad_("Cal-OceanLoad", "ObsCalTheo"), vCal_OceanLoadOld_("Cal-OceanLoadOld", "ObsCalTheo"),
  vCal_OceanPoleTideLoad_("Cal-OceanPoleTideLoad", "ObsCalTheo"),
  vCal_Parallax_("Cal-Parallax", "ObsCalTheo"),
  vCal_PoleTide_("Cal-PoleTide", "ObsCalTheo"),
  vCal_PoleTideOldRestore_("Cal-PoleTideOldRestore", "ObsCalTheo"),
  vCal_TiltRemover_("Cal-TiltRemover", "ObsCalTheo"),
  vCal_Wobble_("Cal-Wobble", "ObsCalTheo"), 
  vCal_HfLibration_("Cal-HiFreqLibration", "ObsCalTheo"),
  vCal_HiFreqLibration_("Cal-HiFreqLibration", "ObsCalTheo"),
  //
  vEdit_("Edit", "ObsEdit"), vNGSQualityFlag_("NGSQualityFlag", "ObsEdit"), 
  vObsCrossRef_("ObsCrossRef", "CrossReference"), vFeedRotNet_("FeedRotNet", "ObsDerived"), 
  vDelayTheoretical_("DelayTheoretical", "ObsTheoretical"), 
  vRateTheoretical_("RateTheoretical", "ObsTheoretical"), 
  vPart_Bend_("Part-Bend", "ObsPart"), vPart_Gamma_("Part-Gamma", "ObsPart"), 
  vPart_Erp_("Part-ERP", "ObsPart"), vPart_NutationEqx_("Part-NutationEQX", "ObsPart"), 
  vPart_NutationNro_("Part-NutationNRO", "ObsPart"), vPart_Parallax_("Part-Parallax", "ObsPart"), 
  vPart_PoleTide_("Part-Poletide", "ObsPart"), vPart_Precession_("Part-Precession", "ObsPart"), 
  vPart_RaDec_("Part-RaDec", "ObsPart"), vPart_Xyz_("Part-XYZ", "ObsPart"),
  obsUserCorrections_(),
  progSolveDescriptor_(),
  progDescriptorByName_(),

  vdbVariables_()
{
  correlatorType_ = CT_Unknown;
  isNewFile_ = true;
  have2adjustPermissions_ = true;
  numOfObs_ = 0;
  numOfScans_ = 0;
  currentVersion_ = 0;
  numOfChan_ = 0;
  numOfStn_ = 0;
  numOfSrc_ = 0;
  inputFormatVersion_   = 0.0;
  outputFormatVersion_  = 1.002;
  have2redoLeapSecond_ = false;
  vdbVariables_ << &vHead_ << &vGroupBLWeights_ << &vClockBreak_ << &vLeapSecond_ << &vMiscCable_ 
                << &vMiscFourFit_ << &vStationApriori_ << &vSourceApriori_ <<&vClockApriori_
                << &vAntennaApriori_ 
                << &vEccentricity_ << &vStationCrossRef_ << &vSourceCrossRef_ << &vScanTimeUTC_ 
                << &vScanName_ << &vErpApriori_ << &vEphemeris_ << &vNutationEqx_kWahr_ << &vNutationEqx_
                << &vNutationNro_ << &vRot_CF2J2K_ << &vCorrRootFile_ << &vObservationTimeUTC_ 
                << &vBaseline_ << &vSource_ << &vDiffTec_
                << &vCal_Bend_ << &vCal_BendSun_ << &vCal_BendSunHigher_ 
                << &vCal_EarthTide_ << &vCal_FeedCorrection_ << &vCal_HfErp_
                << &vCal_OceanLoad_ << &vCal_OceanLoadOld_ << &vCal_OceanPoleTideLoad_ << &vCal_Parallax_
                << &vCal_PoleTide_ << &vCal_PoleTideOldRestore_ << &vCal_TiltRemover_ << &vCal_Wobble_
                << &vCal_HfLibration_ << &vCal_HiFreqLibration_ << &vEdit_ << &vNGSQualityFlag_
                << &vObsCrossRef_ << &vFeedRotNet_ << &vDelayTheoretical_ << &vRateTheoretical_
                << &vPart_Bend_ << &vPart_Gamma_ << &vPart_Erp_ << &vPart_NutationEqx_
                << &vPart_NutationNro_ << &vPart_Parallax_ << &vPart_PoleTide_ << &vPart_Precession_
                << &vPart_RaDec_ << &vPart_Xyz_;
  vdbVariables_ << progSolveDescriptor_.vdbVars_;
  // form the string:
  outputFormatId_.sprintf("%.3f   ", outputFormatVersion_);
  outputFormatId_ += outputFormatEpoch_.toString(SgMJD::F_YYYYMonDD);
  operationMode_ = SgNetCdf::OM_REGULAR;
  sbOrder_ = SO_UNDEF;
  hasSidebandOrder_ = false;
};



//
SgVgosDb::~SgVgosDb()
{
  activeVars_.clear();
  //
  for (int i=0; i<obsUserCorrections_.size(); i++)
    delete obsUserCorrections_.at(i);
  obsUserCorrections_.clear();
  //
};



//
bool SgVgosDb::init(const QString path, const QString fileName)
{
  bool                          isOk(false);
  QString                       str;
  path2RootDir_ = path;
  wrapperFileName_ = fileName;
  QFile                         f(path2RootDir_ + "/" + wrapperFileName_);
  if (!f.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::init(): the wrapper file [" + f.fileName() + "] does not exist");
    return false;
  };
  //
  int                           numOfParsedStrings;
  bool                          isHistoryBlockParsed, isSessionBlockParsed,
                                isScanBlockParsed, isObservationBlockParsed;
  isHistoryBlockParsed = isSessionBlockParsed = isScanBlockParsed = isObservationBlockParsed = false;
  numOfParsedStrings = 0;
  //
  fcfEdit = &fcfEdit_v1001;
  //
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
      {
        if (numOfParsedStrings==0 && str.contains("VERSION", Qt::CaseInsensitive)) // first string only?
        {
          numOfParsedStrings += parseVersionBlock(s, str);
          // check for versions:
          if (outputFormatVersion_ < inputFormatVersion_ && false)
          {
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::init(): the input wrapper file contains a version (" +
              QString("").setNum(inputFormatVersion_) + " of " + 
              inputFormatEpoch_.toString(SgMJD::F_Date) + ") that is newer than the software knows (" + 
              QString("").setNum(outputFormatVersion_) + " of " + 
              outputFormatEpoch_.toString(SgMJD::F_Date) + ").");
            f.close();
            s.setDevice(NULL);
            return false;
          }
          else if (inputFormatVersion_ == 1.001)
          {
            fcfEdit = &fcfEdit_v1001;
          }
          else
          {
            fcfEdit = &fcfEdit_v1002;
          };
        }
        else if (!isHistoryBlockParsed && str.contains("Begin History", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseHistoryBlock(s);
          isHistoryBlockParsed = true;
        }
        else if (!isSessionBlockParsed && str.contains("Begin Session", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseSessionBlock(s);
          isSessionBlockParsed = true;
        }
        else if (str.contains("Begin Station", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseStationBlock(s, str.mid(14));
        }
        else if (!isScanBlockParsed && str.contains("Begin Scan", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseScanBlock(s);
          isScanBlockParsed = true;
        }
        else if (!isObservationBlockParsed && str.contains("Begin Observation", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseObservationBlock(s);
          isObservationBlockParsed = true;
        }
        else if (str.contains("Begin Program", Qt::CaseInsensitive))
        {
          numOfParsedStrings += parseProgramBlock(s, str.mid(14));
        }
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::init(): got an unexpected string in the wrapper file: [" + str + 
            "]; skipped");
          numOfParsedStrings++;
        };
      };
    };
    f.close();
    s.setDevice(NULL);
    isOk = true;
    QFileInfo                   fi(f);
    QDateTime                   dateTime(fi.created());
    dateOfCreation_ = SgMJD(dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
                            dateTime.time().hour(), dateTime.time().minute(),
                            dateTime.time().second()+dateTime.time().msec()*0.001);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::init(): cannot open the wrapper file [" + f.fileName() + "] for read access");
    return false;
  };
  if (isOk)
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::init(): parsed " + QString("").setNum(numOfParsedStrings) + 
      " strings from the wrapper file [" + f.fileName() + "]");
//dump2stdout();
  // 
  isOk = isValidNcFiles();
  //
  if (currentIdentities_)
    for (int i=0; i<vdbVariables_.size(); i++)
      vdbVariables_.at(i)->setCurrentInstitution(currentIdentities_->getAcAbbName());
    
  corrTypeId_ = "Corr"; //generic name
  isNewFile_ = false;

  // check for [polular] missed data:
  if (bandDataByName_.contains("S"))
  {
    const BandData             &bd=bandDataByName_["S"];
    if (bd.vQualityCode_.isEmpty())
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::init(): no data provided for the Quality Code on the secondary band of " + 
        sessionName_ + ": " + f.fileName());
    if (bd.vAmbigSize_.isEmpty())
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::init(): no data provided for the Ambiguity Size on the secondary band of " + 
        sessionName_ + ": " + f.fileName());
    if (bd.vGroupRate_.isEmpty())
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::init(): no data provided for the Group Delay Rate on the secondary band of " + 
        sessionName_ + ": " + f.fileName());
/*
    if (bd.vEffFreq_.isEmpty())
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::init(): no data provided for the Effective Frequency on the secondary band of " + 
        sessionName_ + ": " + f.fileName());
*/
    if (bd.vSBDelay_.isEmpty())
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::init(): no data provided for the Single Band Delay on the secondary band of " + 
        sessionName_ + ": " + f.fileName());
  };
  //
  //
  collectStationNames();
  //
  makeWorkAround4KnownBugs();
  //
  //
  return isOk;
};



//
bool SgVgosDb::init(SgVlbiSession* session)
{
  // inits data for a new (not represented in VGOS format) session.
  numOfObs_ = session->observations().size();
  // number of scans?
  QMap<QString, SgVlbiObservation*>
                                obsByScan;
  //
  for (int i=0; i<numOfObs_; i++)
  {
    SgVlbiObservation*          obs=session->observations().at(i);
//4KOMB:
//std::cout << " -- - - " << qPrintable(QString("").sprintf("%3d(%3d)", i, obsByScan.size()))
//          << qPrintable(obs->getScanId()) << "  :  "
//          << qPrintable(obs->getKey()) << "  :  " << qPrintable(obs->getScanName())
//          << qPrintable(obs->toString())
//          << " \n";
//  if (!obsByScan.contains(obs->getScanName()))
//      obsByScan.insert(obs->getScanName(), obs);
    if (!obsByScan.contains(obs->getScanId()))
      obsByScan.insert(obs->getScanId(), obs);
  };
  numOfScans_ = obsByScan.size();
  obsByScan.clear();
  numOfChan_ = 0;
  numOfStn_ = session->stationsByName().size();
  numOfSrc_ = session->sourcesByName().size();
//  QList<SgVdbVariable*>         activeVars_;
  if (session->getOriginType()==SgVlbiSession::OT_MK4 ||
      session->getOriginType()==SgVlbiSession::OT_AGV)
    corrTypeId_ = "Mk4";
  else if (session->getOriginType()==SgVlbiSession::OT_KOMB)
    corrTypeId_ = "Komb";
  else
    corrTypeId_ = "Corr"; //generic name
  
  setSessionName(session->getName());
  setSessionCode(session->getSessionCode());
  
  stnDescriptorByKey_.clear();
  stnDescriptorByName_.clear();
  for (StationsByName_it it=session->stationsByName().begin(); it!=session->stationsByName().end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    QString                     key=stn->getKey();
    key = key.simplified();
    key.replace(' ', '_');
    StationDescriptor          &sd=stnDescriptorByKey_[key];
    sd.stationKey_ = key;
    sd.stationName_= stn->getKey();
    // fill the map: 
    stnDescriptorByName_[sd.stationName_] = &stnDescriptorByKey_[key];
    //
    sd.numOfPts_ = stn->auxObservationByScanId()->size();
    //
    procNcString("TimeUTC.nc",  key,  sd.vTimeUTC_);
    vdbVariables_ << &sd.vTimeUTC_;
    if (false &&
          ( session->getOriginType()==SgVlbiSession::OT_MK4 || 
            session->getOriginType()==SgVlbiSession::OT_KOMB  ) )
    {
      procNcString("AzEl_k" + corrTypeId_ + ".nc", key,  sd.vAzEl_);
      vdbVariables_ << &sd.vAzEl_;
    };
    if (session->getOriginType()==SgVlbiSession::OT_KOMB)
    {
      procNcString("RefClockOffset.nc", key, sd.vRefClockOffset_);
      vdbVariables_ << &sd.vRefClockOffset_;
    };
    sd.propagateStnKey();
  };
  //
  for (QMap<QString, SgVlbiBand*>::iterator it=session->bandByKey().begin();
    it!=session->bandByKey().end(); ++it)
  {
    BandData                   &bd=bandDataByName_[it.key()];
    vdbVariables_ << &bd.vChannelInfo_ << &bd.vPhaseCalInfo_ << &bd.vCorrInfo_ << &bd.vQualityCode_
                  << &bd.vRefFreq_ << &bd.vAmbigSize_ << &bd.vSNR_ << &bd.vCorrelation_ << &bd.vPhase_
                  << &bd.vSBDelay_ << &bd.vGroupDelay_ << &bd.vGroupRate_ << &bd.vDelayDataFlag_
                  << &bd.vDataFlag_ << &bd.vPhaseDelayFull_ << &bd.vUVFperAsec_ << &bd.vEffFreq_EqWt_
                  << &bd.vEffFreq_ << &bd.vCal_SlantPathIonoGroup_ << &bd.vCal_FeedCorrection_
                  << &bd.vCal_Unphase_ << &bd.vNumGroupAmbig_ << &bd.vNumPhaseAmbig_ 
                  << &bd.vGroupDelayFull_ << &bd.vMiscFourFit_;
  };
  if (currentIdentities_)
    for (int i=0; i<vdbVariables_.size(); i++)
      vdbVariables_.at(i)->setCurrentInstitution(currentIdentities_->getAcAbbName());

  isNewFile_ = true;
  return true;
};



//
int SgVgosDb::parseVersionBlock(QTextStream&, const QString& str)
{
  QRegExp                       reFmtVer("^VERSION\\s+([+-\\.\\d]+)\\s+(\\d{4}\\w{3}\\d{2})\\s*",
                                  Qt::CaseInsensitive);
  bool                          isOk=false;
  if (reFmtVer.indexIn(str) != -1)
  {
    double                      d=reFmtVer.cap(1).toDouble(&isOk);
    if (isOk)
    {
      SgMJD                     t_tmp(tZero);
      if (t_tmp.fromString(SgMJD::F_YYYYMonDD, reFmtVer.cap(2)))
      {
        inputFormatEpoch_ = t_tmp;
        inputFormatVersion_ = d;
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::parseVersionBlock(): found format version " + QString("").setNum(inputFormatVersion_) + 
          " of " + inputFormatEpoch_.toString());
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseVersionBlock(): cannot get an epoch from VERSION string: [" + str + "]");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::parseVersionBlock(): cannot convert value to double from VERSION string: [" + str + "]");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::parseVersionBlock(): got an unexpected VERSION string: [" + str + "]");
  // currently, it is just one string:
  return 1;
};



//
int SgVgosDb::parseHistoryBlock(QTextStream &s)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End History");
  QString                       bosb("Begin Program");
  QString                       str;
  
  if (1.0 <= inputFormatVersion_)
    bosb = "Begin Process";
    
  while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains(bosb, Qt::CaseInsensitive))
      {
        numOfStrs += parseHistoryProcessSubBlock(s, str.mid(14));
      }
      else if (str.contains(eob, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseHistoryBlock(): got an unknown string in the history block of wrapper file: [" + 
          str + "]; skipped");
        numOfStrs++;
      };
    };
  };
  return numOfStrs;
};



//
int SgVgosDb::parseHistoryProcessSubBlock(QTextStream& s, const QString& progName)
{
  int                           numOfStrs;
  numOfStrs = 0;
  QString                       eosb("End Program");
  QString                       str, subDir(""), key(progName);

  if (1.0 <= inputFormatVersion_)
    eosb = "End Process";
  
  //
  HistoryDescriptor               hd;
  hd.processName_ = progName;
  //
  // lookup the creator and set the sideband order:
  if (sbOrder_ == SO_UNDEF)
  {
    if (progName.contains("db2vgosDB", Qt::CaseInsensitive))
      sbOrder_ = SO_LSB_USB;
    else if (progName.contains("SgLib/vgosDbMake", Qt::CaseInsensitive))
      sbOrder_ = SO_USB_LSB;
  }
  else if ( (sbOrder_ == SO_LSB_USB && progName.contains("SgLib/vgosDbMake", Qt::CaseInsensitive)) ||
            (sbOrder_ == SO_USB_LSB && progName.contains("db2vgosDB", Qt::CaseInsensitive)) )
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::parseHistoryProcessSubBlock(): cannot determine the sideband order");
  //
  //
  
  while (!str.contains(eosb, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains("Default_Dir", Qt::CaseInsensitive))
      {
        //Default_dir History
        subDir = str.mid(12);
        hd.defaultDir_ = subDir;
        numOfStrs++;
      }
      else if (str.contains("RunTimeTag", Qt::CaseInsensitive))
      {
        //RunTimeTag 2014/02/10 08:28:53
        //RunTimeTag 2014/02/10 08:28:53 UTC
        bool                isUtc=false;
        str = str.mid(11).simplified();
        if (str.endsWith("UTC"))
        {
          str.chop(3);
          isUtc = true;
        };
        if (!hd.epochOfCreation_.fromString(SgMJD::F_YYYYMMDDHHMMSSSS, str))
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::parseHistoryProcessSubBlock(): cannot get proper creation epoch: [" + 
            str + "]; skipped");
        else if (!isUtc)
          hd.epochOfCreation_ = hd.epochOfCreation_.toUtc();
        numOfStrs++;
      }
      else if (str.contains("Createdby ", Qt::CaseInsensitive))
      {
        //CreatedBy John M. Gipson
        hd.creator_ = str.mid(10);
        numOfStrs++;
      }
      else if (str.contains("History ", Qt::CaseInsensitive))
      {
        //History  11MAR31XE_V004.hist
        //History    08AUG12XA_kMK3DB_V004.hist
        hd.historyFileName_ = str.mid(8).simplified();
        numOfStrs++;
      }
      else if (str.contains("InputWrapper ", Qt::CaseInsensitive))
      {
        //InputWrapper 15JUL28BB_V001_kall.wrp
        hd.inputWrapperFileName_ = str.mid(13).simplified();
        numOfStrs++;
      }
      else if (str.contains("Version", Qt::CaseInsensitive))
      {
        //Version   Mixed
        //Version   2014Feb08
        hd.version_ = str.mid(8).simplified();
        numOfStrs++;
        if (hd.version_.contains("Mixed", Qt::CaseInsensitive))
          hd.isMk3Compatible_ = true;
      }
      else if (str.contains(eosb, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseHistoryProcessSubBlock(): got an unknown string in "
          "the history block of wrapper file: [" + str + "]; skipped");
        numOfStrs++;
std::cout << "      Fix me: HISTORY need to add a parser for the string [" << qPrintable(str) << "]\n";
      };
    };
  };
  historyDescriptors_ << hd;
  return numOfStrs;
};



//
int SgVgosDb::parseSessionBlock(QTextStream &s)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End Session");
  QString                       str, subDir("");
  QString                       bandKey(""), bKey("");
  
  while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    parseBandKey(str, bandKey);
    bKey = bandKey==""?"X":bandKey;

    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains("Default_Dir", Qt::CaseInsensitive))
      {
        //Default_Dir Session
        subDir = str.mid(12);
        numOfStrs++;
      }
      else if (str.contains("Session ", Qt::CaseInsensitive))
      {
        //Session R4476
        sessionCode_ = str.mid(8);
        numOfStrs++;
      }
      else if (str.contains("AltSessionId ", Qt::CaseInsensitive))
      {
        //AltSessionId 11MAR31XE
        sessionName_ = str.mid(13);
        numOfStrs++;
      }
      else if ( str.contains("Head", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vHead_);
      else if ( str.contains("GroupBLWeights", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vGroupBLWeights_);
      else if ( str.contains("ClockBreak", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vClockBreak_);
      else if ( str.contains("LeapSecond", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vLeapSecond_);
      else if ( str.contains("MiscFourFit", Qt::CaseInsensitive) &&
                str.contains("_b", Qt::CaseInsensitive)          &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vMiscFourFit_);
      else if ( str.contains("MiscFourFit", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vMiscFourFit_);
      else if ( str.contains("MiscCable", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vMiscCable_);
      else if ( str.contains("StationCrossRef", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vStationCrossRef_);
      else if ( str.contains("BaselineClockSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vBaselineClockSetup_);
      else if ( str.contains("ClockSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vClockSetup_);
      else if ( str.contains("Station", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vStationApriori_);
      else if ( str.contains("SourceCrossRef", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vSourceCrossRef_);
      else if ( str.contains("Source", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vSourceApriori_);
      else if ( str.contains("Clock", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vClockApriori_);
      else if ( str.contains("Eccentricity", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vEccentricity_);
      else if ( str.contains("Antenna", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vAntennaApriori_);
      else if ( str.contains("CalcInfo", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalcInfo_);
      else if ( str.contains("CalcErp", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalcErp_);
      else if ( str.contains("AtmSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vAtmSetup_);
      else if ( str.contains("ErpSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vErpSetup_);
      else if ( str.contains("CalibrationSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalibrationSetup_);
      else if ( str.contains("IonoSetup", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vIonoSetup_);
      else if ( str.contains("SelectionStatus", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vSelectionStatus_);
      // just ignore it:
      else if ( str.contains("PhaseBLWeights", Qt::CaseInsensitive))
        numOfStrs++;
      // end of parsing
      else if (str.contains(eob, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseSessionBlock(): got an unexpected string in the session block of wrapper file: [" + 
          str + "]; skipped");
        numOfStrs++;
std::cout << "      Fix me: SESSION need to add a parser for the string [" << qPrintable(str) << "]\n";
      };
    };
  };

  // update vdbVariables_:
  for (QMap<QString, BandData>::iterator it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
  {
    BandData                   &bd=it.value();
    vdbVariables_ << &bd.vMiscFourFit_;
  };
  return numOfStrs;
};



//
int SgVgosDb::parseStationBlock(QTextStream &s, const QString& stnName)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End Station");
  QString                       str, subDir(""), key;
  key = stnName;

//  key.sprintf("%-8s", qPrintable(stnName));
//  key.replace('_', ' ');
  
  //StationDescriptor
  if (stnDescriptorByKey_.contains(key) && 
      !stnDescriptorByKey_.value(key).vTimeUTC_.isEmpty())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::parseStationBlock(): got a duplicate of the station [" + stnName + "]; skipped");
    while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
    {
      str = s.readLine();
      numOfStrs++;
    };
    return numOfStrs;
  };
  StationDescriptor            &sd=stnDescriptorByKey_[key];
  sd.stationKey_ = stnName;
  while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains("Default_Dir", Qt::CaseInsensitive))
      {
        //Default_Dir Session
        subDir = str.mid(12);
        numOfStrs++;
      }
      else if ( str.contains("TimeUTC", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vTimeUTC_);
      else if ( str.contains("Met", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vMet_);
      else if ( str.contains("AzEl", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vAzEl_);
      else if ( str.contains("FeedRotation", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vFeedRotation_);
      else if ( str.contains("Cal-AxisOffset", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_AxisOffset_);
      else if ( str.contains("Cal-CableCorrections", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_CableCorrections_);
      else if ( str.contains("Cal-CblCorrections", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_CblCorrections_);
      else if ( str.contains("Cal-Cable", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_Cable_);
      else if ( str.contains("Cal-SlantPathTropDry", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_SlantPathTropDry_);
      else if ( str.contains("Cal-SlantPathTropWet", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_SlantPathTropWet_);
      else if ( str.contains("Cal-StationOceanLoad", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vCal_OceanLoad_);
      else if ( str.contains("Part-AxisOffset", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vPart_AxisOffset_);
      else if ( str.contains("Part-ZenithPathTropDry", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vPart_ZenithPathTropDry_);
      else if ( str.contains("Part-ZenithPathTropWet", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vPart_ZenithPathTropWet_);
      else if ( str.contains("Part-HorizonGrad", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vPart_HorizonGrad_);
      else if ( str.contains("Dis-OceanLoad", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vDis_OceanLoad_);
      // KOMB:
      else if ( str.contains("RefClockOffset", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vRefClockOffset_);
      // TSYS:
      else if ( str.contains("Tsys", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, sd.vTsys_);
      // end of parsing
      else if (str.contains(eob, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseStationBlock(): got an unknown string in the station block of wrapper file: [" + 
          str + "]; skipped");
        numOfStrs++;
std::cout << "      Fix me: STATION need to add a parser for the string [" << qPrintable(str) << "]\n";
      };
    };
  };
  vdbVariables_ << &sd.vTimeUTC_ << &sd.vMet_ << &sd.vAzEl_ << &sd.vFeedRotation_
                << &sd.vCal_AxisOffset_ << &sd.vCal_Cable_ << &sd.vCal_CableCorrections_ // tmp
                << &sd.vCal_CblCorrections_
                << &sd.vCal_SlantPathTropDry_ << &sd.vCal_SlantPathTropWet_ << &sd.vCal_OceanLoad_ 
                << &sd.vPart_AxisOffset_ << &sd.vPart_ZenithPathTropDry_ << &sd.vPart_ZenithPathTropWet_
                << &sd.vPart_HorizonGrad_ << &sd.vDis_OceanLoad_ << &sd.vRefClockOffset_
                << &sd.vTsys_;
 
  return numOfStrs;
};



//
int SgVgosDb::parseScanBlock(QTextStream &s)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End Scan");
  QString                       str, subDir("");
  
  while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains("Default_Dir", Qt::CaseInsensitive))
      {
        //Default_Dir Session
        subDir = str.mid(12);
        numOfStrs++;
      }
      // begin of parsing:
      else if ( str.contains("TimeUTC", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vScanTimeUTC_);
      else if ( str.contains("ScanName", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vScanName_);
      else if ( str.contains("ERPApriori", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vErpApriori_);
      else if ( str.contains("Ephemeris", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vEphemeris_);
      else if ( str.contains("NutationEQX_kWahr", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vNutationEqx_kWahr_);
      else if ( str.contains("NutationEQX", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vNutationEqx_);
      else if ( str.contains("NutationNRO", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vNutationNro_);
      else if ( str.contains("Rot-CF2J2K", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vRot_CF2J2K_);
      else if ( str.contains("CorrRootFile", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCorrRootFile_);
      // pre 1.000 format version:
      else if ( str.contains("ScanTimeMJD", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vScanTimeMJD_);
      //
      // end of parsing
      else if (str.contains(eob, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseScanBlock(): got an unknown string in the scan block of wrapper file: [" + 
          str + "]; skipped");
std::cout << "      Fix me: SCAN need to add a parser for the string [" << qPrintable(str) << "]\n";
        numOfStrs++;
      };
    };
  };
  
  return numOfStrs;
};



//
void SgVgosDb::parseBandKey(const QString& str, QString& key)
{
  QRegExp                       reBand("_b(\\w).*", Qt::CaseInsensitive);
  if (reBand.indexIn(str) != -1)
    key = reBand.cap(1);
  else
    key = "";
};



//
int SgVgosDb::parseObservationBlock(QTextStream &s)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End Observation");
  QString                       str(""), subDir("");
  QString                       bandKey(""), bKey("");
  
  while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
  {
    str = s.readLine();
    parseBandKey(str, bandKey);
    bKey = bandKey==""?"X":bandKey;

    if (str.at(0) != '!' && str.at(0) != '#' && str.size()>0)
    {
      if (str.contains("Default_Dir", Qt::CaseInsensitive))
      {
        //Default_Dir Session
        subDir = str.mid(12);
        numOfStrs++;
      }
      else if ( str.contains("TimeUTC", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vObservationTimeUTC_);
      else if ( str.contains("Baseline", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vBaseline_);
      else if ( str.contains("Source", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vSource_);
      //
      //
      // per band:
      else if ( str.contains("GroupDelayFull", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vGroupDelayFull_);
      else if ( str.contains("ChannelInfo", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vChannelInfo_);
      else if ( str.contains("PhaseCalInfo", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vPhaseCalInfo_);
      else if ( str.contains("CorrInfo", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
      {
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vCorrInfo_);
        if (str.contains("-Mk3", Qt::CaseInsensitive))
          correlatorType_ = CT_Mk3;
        else if (str.contains("-VLBA", Qt::CaseInsensitive))
          correlatorType_ = CT_VLBA;
        else if (str.contains("-CRL", Qt::CaseInsensitive))
          correlatorType_ = CT_CRL;
        else if (str.contains("-GSI", Qt::CaseInsensitive))
          correlatorType_ = CT_GSI;
        else if (str.contains("-K5", Qt::CaseInsensitive))
          correlatorType_ = CT_GSI;
        else if (str.contains("-S2", Qt::CaseInsensitive))
          correlatorType_ = CT_S2;
        else if (str.contains("-Mk4", Qt::CaseInsensitive))
          correlatorType_ = CT_Mk4;
        else if (str.contains("-Haystack", Qt::CaseInsensitive))
          correlatorType_ = CT_Haystack;
        else if (str.contains("-difx", Qt::CaseInsensitive))
          correlatorType_ = CT_Difx;
        else
          correlatorType_ = CT_Unknown;
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
          "::parseObservationBlock(): the correlator type is set to " + 
          QString("").setNum(correlatorType_));
      }
      else if ( str.contains("QualityCode", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vQualityCode_);
      else if ( str.contains("EffFreq", Qt::CaseInsensitive) &&
                str.contains("_kEqWt", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive) &&
               !str.contains("_kDB", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vEffFreq_EqWt_);
      else if ( str.contains("EffFreq", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive) &&
               !str.contains("_kDB", Qt::CaseInsensitive))
      {
        if (bandDataByName_[bandKey].vEffFreq_.isEmpty() || !str.contains("_kDB", Qt::CaseInsensitive))
        {
          bandDataByName_[bandKey].vEffFreq_.empty();
          numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vEffFreq_);
        };
      }
      else if ( str.contains("RefFreq", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vRefFreq_);
      else if ( str.contains("AmbigSize", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vAmbigSize_);
      else if ( str.contains("SNR", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vSNR_);
      else if ( str.contains("Correlation", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vCorrelation_);
      else if ( str.contains("UVFperAsec", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vUVFperAsec_);
      else if ( str.contains("PhaseDelayFull", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vPhaseDelayFull_);
      else if ( str.contains("SBDelay", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vSBDelay_);
      else if ( str.contains("GroupDelay", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vGroupDelay_);
      else if ( str.contains("GroupRate", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vGroupRate_);
      else if ( str.contains("DelayDataFlag", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vDelayDataFlag_);
      else if ( str.contains("DataFlag", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vDataFlag_);
      // VGOS:
      else if ( str.contains("DiffTec", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vDiffTec_);
      // Theoretical Calibrations:
      else if ( str.contains("Cal-BendSunHigher", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_BendSunHigher_);
      else if ( str.contains("Cal-BendSun", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_BendSun_);
      else if ( str.contains("Cal-Bend", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_Bend_);
      else if ( str.contains("Cal-EarthTide", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_EarthTide_);
      else if ( str.contains("Cal-HiFreqERP", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_HfErp_);
      else if ( str.contains("Cal-OceanLoadOld", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_OceanLoadOld_);
      else if ( str.contains("Cal-OceanLoad", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_OceanLoad_);
      else if ( str.contains("Cal-OceanPoleTideLoad", Qt::CaseInsensitive) && 
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_OceanPoleTideLoad_);
      else if ( str.contains("Cal-Parallax", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_Parallax_);
      else if ( str.contains("Cal-PoleTideOldRestore", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_PoleTideOldRestore_);
      else if ( str.contains("Cal-PoleTide", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_PoleTide_);
      else if ( str.contains("Cal-TiltRemover", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_TiltRemover_);
      else if ( str.contains("Cal-FeedCorrection", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
      {
        numOfStrs += procNcString(str, subDir, bandDataByName_[bKey].vCal_FeedCorrection_);
        if (bandKey == "")
        {
          bandDataByName_[bKey].vCal_FeedCorrection_.setBand(bKey);
          logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
            "::parseObservationBlock(): got unbanded Feed Correction, the band key \"X\" is assumed");
        };
      }
      else if ( str.contains("Cal-Unphase", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
      {
        numOfStrs += procNcString(str, subDir, bandDataByName_[bKey].vCal_Unphase_);
        if (bandKey == "")
        {
          bandDataByName_[bKey].vCal_Unphase_.setBand(bKey);
          logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
            "::parseObservationBlock(): got unbanded Unphase Calibration, the band key \"X\" is assumed");
        };
      }
      else if ( str.contains("Cal-Wobble", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_Wobble_);
      else if ( str.contains("Cal-HiFreqLibration", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive) &&
                str.contains("IERS2006", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_HfLibration_);
      else if ( str.contains("Cal-HiFreqLibration", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive) &&
                str.contains("IERS2010", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vCal_HiFreqLibration_);
      // ObsEdit:
      else if ( str.contains("Edit", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vEdit_);
      else if ( str.contains("NGSQualityFlag", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vNGSQualityFlag_);
      else if ( str.contains("NumGroupAmbig", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vNumGroupAmbig_);
      else if ( str.contains("NumPhaseAmbig", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vNumPhaseAmbig_);
      // CrossRefs:
      else if ( str.contains("ObsCrossRef", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vObsCrossRef_);
      // ObsDerived:
      else if ( str.contains("FeedRotNet", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vFeedRotNet_);
      else if ( str.contains("Cal-SlantPathIonoGroup", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vCal_SlantPathIonoGroup_);
      // ObsTheo:
      else if ( str.contains("DelayTheoretical", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vDelayTheoretical_);
      else if ( str.contains("RateTheoretical", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vRateTheoretical_);
      // ObsPartials:
      else if ( str.contains("Part-Bend", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Bend_);
      else if ( str.contains("Part-Gamma", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Gamma_);
      else if ( str.contains("Part-ERP", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Erp_);
      else if ( str.contains("Part-NutationEQX", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_NutationEqx_);
      else if ( str.contains("Part-NutationNRO", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_NutationNro_);
      else if ( str.contains("Part-Parallax", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Parallax_);
      else if ( str.contains("Part-Poletide", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_PoleTide_);
      else if ( str.contains("Part-Precession", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Precession_);
      else if ( str.contains("Part-RaDec", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_RaDec_);
      else if ( str.contains("Part-XYZ", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, vPart_Xyz_);
      // pre 1.000 format version:
      else if ( str.contains("IonoBits", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vIonoBits_);
      else if ( str.contains("UnPhaseCalFlag", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vUnPhaseCalFlag_);
      else if ( str.contains("FractC", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
        numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vFractC_);
      // special case:
      else if ( str.contains("StructureCorrections", Qt::CaseInsensitive) &&
                str.contains(".nc", Qt::CaseInsensitive))
      {
        SgVdbVariable           *vUser=new SgVdbVariable("StructureCorrections", "ObsUserCorrections");
        numOfStrs += procNcStringSpec(str, subDir, *vUser);
        obsUserCorrections_.append(vUser);
        vdbVariables_ << obsUserCorrections_.last();
      }
      //
      // skip this phaseDelays-dependants:
      // just ignore it:
      else if ( str.contains("Cal-SlantPathIonoPhase", Qt::CaseInsensitive))
        numOfStrs++;
      else if ( str.contains("NumPhaseAmbig", Qt::CaseInsensitive))
        numOfStrs++;
      // "phase" after all "*phase*" strings processed:
      else if ( str.startsWith("Phase", Qt::CaseInsensitive)  &&    // should switch to RegExps
                str.contains(".nc", Qt::CaseInsensitive)      &&
               !str.contains("Ambig", Qt::CaseInsensitive)      )
        numOfStrs += procNcString(str, subDir, bandDataByName_[bandKey].vPhase_);
      // just ignore it:
      else if ( str.contains("Unavailable", Qt::CaseInsensitive) &&
                str.contains("band", Qt::CaseInsensitive))
        numOfStrs++;
      else if ( str.contains("Cal-EccentricityMap", Qt::CaseInsensitive))
        numOfStrs++;
      // end of parsing
      else if (str.contains(eob, Qt::CaseInsensitive))
        numOfStrs++;
      else
      {
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
          "::parseObservationBlock(): got an unknown string in the observation block of wrapper"
          " file: [" + str + "]; skipped");
std::cout << "      Fix me: OBS need to add a parser for the string [" << qPrintable(str) << "]\n";
        numOfStrs++;
      };
    };
  };
  //
  // check a special case:
  //
  QMap<QString, BandData>::iterator
                                itt=bandDataByName_.begin();
/*
  while (itt != bandDataByName_.end())
  {
    BandData                   &bd=itt.value();
    bool                        hasData=true;
    hasData = hasData && !bd.vChannelInfo_.isEmpty()
//                    && !bd.vPhaseCalInfo_.isEmpty() 
                      && !bd.vCorrInfo_.isEmpty()
//                    && !bd.vQualityCode_.isEmpty()
                      && !bd.vRefFreq_.isEmpty()
                      && !bd.vAmbigSize_.isEmpty()
//                    && !bd.vSNR_.isEmpty()
//                    && !bd.vCorrelation_.isEmpty()
                      && !bd.vPhase_.isEmpty()
//                    && !bd.vSBDelay_.isEmpty()
                      && !bd.vGroupDelay_.isEmpty()
                      && !bd.vGroupRate_.isEmpty()
//                    && !bd.vDelayDataFlag_.isEmpty()
//                    && !bd.vDataFlag_.isEmpty()
//                    && !bd.vPhaseDelayFull_.isEmpty()
//                    && !bd.vUVFperAsec_.isEmpty()
//                    && !bd.vEffFreq_EqWt_.isEmpty()
//                    && !bd.vEffFreq_.isEmpty()
//                    && !bd.vCal_SlantPathIonoGroup_.isEmpty()
//                    && !bd.vCal_FeedCorrection_.isEmpty()
//                    && !bd.vCal_Unphase_.isEmpty()
//                    && !bd.vNumGroupAmbig_.isEmpty()
//                    && !bd.vNumPhaseAmbig_.isEmpty()
//                    && !bd.vGroupDelayFull_.isEmpty()
                      ;
    if (!hasData)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::parseObservationBlock(): the database missed essential data for " + itt.key() + "-band");
      itt = bandDataByName_.erase(itt);
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::parseObservationBlock(): the " + itt.key() + "-band is removed from the session");
    }
    else
      ++itt;
  };
*/
  //
  //
  // update vdbVariables_:
  for (itt=bandDataByName_.begin(); itt!=bandDataByName_.end(); ++itt)
  {
    BandData                   &bd=itt.value();
    vdbVariables_ << &bd.vChannelInfo_ << &bd.vPhaseCalInfo_ << &bd.vCorrInfo_ << &bd.vQualityCode_
                  << &bd.vRefFreq_ << &bd.vAmbigSize_ << &bd.vSNR_ << &bd.vCorrelation_ << &bd.vPhase_
                  << &bd.vSBDelay_ << &bd.vGroupDelay_ << &bd.vGroupRate_ << &bd.vDelayDataFlag_
                  << &bd.vDataFlag_ << &bd.vPhaseDelayFull_ << &bd.vUVFperAsec_ << &bd.vEffFreq_EqWt_
                  << &bd.vEffFreq_ << &bd.vCal_SlantPathIonoGroup_ << &bd.vCal_FeedCorrection_
                  << &bd.vCal_Unphase_ << &bd.vNumGroupAmbig_ << &bd.vNumPhaseAmbig_
                  << &bd.vGroupDelayFull_;
  };
  //
  return numOfStrs;
};



//
int SgVgosDb::parseProgramBlock(QTextStream &s, const QString &sProgName)
{
  int                           numOfStrs;
  numOfStrs = 0;
  const QString                 eob("End Program");
  QString                       str(""), subDir(""), key;
  key = sProgName.simplified();
  
  if (key.compare("Solve", Qt::CaseInsensitive) == 0)
  {
    while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
    {
      str = s.readLine();
      numOfStrs++;
      if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
      {
        if (str.contains("Default_Dir", Qt::CaseInsensitive))
          subDir = str.mid(12);
        else if (str.contains("Begin Session", Qt::CaseInsensitive))
        {
          while (!str.contains("End Session", Qt::CaseInsensitive) && !s.atEnd())
          {
            str = s.readLine();
            if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
            {
              if (str.contains("Default_Dir", Qt::CaseInsensitive))
                subDir = str.mid(12);
              else if ( str.contains("AtmSetup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vAtmSetup_);
              else if ( str.contains("BaselineClockSetup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vBaselineClockSetup_);
              else if ( str.contains("ClockSetup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vClockSetup_);
              else if ( str.contains("ErpSetup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vErpSetup_);
              else if ( str.contains("CalibrationSetup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalibrationSetup_);
              else if ( str.contains("IonoSetup", Qt::CaseInsensitive) && 
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vIonoSetup_);
              else if ( str.contains("CalcErp", Qt::CaseInsensitive) && 
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalcErp_);
              else if ( str.contains("SelectionStatus", Qt::CaseInsensitive) && 
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vSelectionStatus_);
              else if ( str.contains("CalcInfo", Qt::CaseInsensitive) && 
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vCalcInfo_);
              else if ( str.contains("End Session", Qt::CaseInsensitive) )
                numOfStrs++;
              else
              {
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::parseProgramBlock(): got an unexpected string in the program block of"
                  "wrapper file: [" + str + "]; skipped");
                numOfStrs++;
std::cout << "      Fix me: PROGRAM need to add a parser for the string [" << qPrintable(str) << "]\n";
              };
            };
          };
        }
        else if (str.contains("Begin Station", Qt::CaseInsensitive))
        {
          while (!str.contains("End Station", Qt::CaseInsensitive) && !s.atEnd())
          {
            str = s.readLine();
            if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
            {
              // .... add here
              numOfStrs++;
            };
          };
        }
        else if (str.contains("Begin Scan", Qt::CaseInsensitive))
        {
          while (!str.contains("End Scan", Qt::CaseInsensitive) && !s.atEnd())
          {
            str = s.readLine();
            if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
            {
              if (str.contains("Default_Dir", Qt::CaseInsensitive))
                subDir = str.mid(12);
              else if ( str.contains("ScanTimeMJD", Qt::CaseInsensitive) && 
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vScanTimeMJD_);
              else if (str.contains("End Scan", Qt::CaseInsensitive))
                numOfStrs++;
              else
              {
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::parseProgramBlock(): got an unexpected string in the program block of"
                  "wrapper file: [" + str + "]; skipped");
                numOfStrs++;
std::cout << "      Fix me: PROGRAM need to add a parser for the string [" << qPrintable(str) << "]\n";
              };
            };
          };
        }
        else if (str.contains("Begin Observation", Qt::CaseInsensitive))
        {
          while (!str.contains("End Observation", Qt::CaseInsensitive) && !s.atEnd())
          {
            str = s.readLine();
            if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
            {
              if (str.contains("Default_Dir", Qt::CaseInsensitive))
                subDir = str.mid(12);
              else if ( str.contains("IonoBits", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vIonoBits_);
              else if ( str.contains("UnPhaseCalFlag", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vUnPhaseCalFlag_);
              else if ( str.contains("FractC", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vFractC_);
              else if ( str.contains("UserSup", Qt::CaseInsensitive) &&
                        str.contains(".nc", Qt::CaseInsensitive))
                numOfStrs += procNcString(str, subDir, progSolveDescriptor_.vUserSup_);
              else if (str.contains("End Observation", Qt::CaseInsensitive))
                numOfStrs++;
              else
              {
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::parseProgramBlock(): got an unexpected string in the program block of"
                  "wrapper file: [" + str + "]; skipped");
                numOfStrs++;
std::cout << "      Fix me: PROGRAM need to add a parser for the string [" << qPrintable(str) << "]\n";
              };
            };
          };
        }
        else if (str.contains(eob, Qt::CaseInsensitive))
          numOfStrs++;
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::parseProgramBlock(): got an unexpected string in the wrapper file: [" + str + 
            "]; skipped");
          numOfStrs++;
        };
      };
    };
  }
  else if (progDescriptorByName_.contains(key))
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::parseProgramBlock(): got a duplicate of the Program section of  [" + 
      key + "]; skipped");
    while (!str.contains(eob, Qt::CaseInsensitive) && !s.atEnd())
    {
      str = s.readLine();
      if (str.size()>0 && str.at(0) != '!' && str.at(0) != '#')
      {
        // add here..
        numOfStrs++;
      };
    };
    return numOfStrs;
  }
  else
  {
    ProgramGenericDescriptor&   pgd=progDescriptorByName_[key];
    pgd.programName_ = key;
    while (!(str=s.readLine()).contains(eob, Qt::CaseInsensitive) && !s.atEnd())
    {
      pgd.content_ << str;
      numOfStrs++;
    };
  };
  return numOfStrs;
};



//
void SgVgosDb::collectStationNames()
{
  for (QMap<QString, StationDescriptor>::iterator it=stnDescriptorByKey_.begin();
    it!=stnDescriptorByKey_.end(); ++it)
  {
    StationDescriptor          &d=it.value();
    d.propagateStnKey();
    if (loadStationName(d.stationKey_))
    {
      if (!stnDescriptorByName_.contains(d.stationName_))
      {
        stnDescriptorByName_[d.stationName_] = &d;
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::collectStationNames(): mapped the station name [" + d.stationName_ + 
          "] to the station key [" + d.stationKey_ + "]");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
          "::collectStationNames(): the station name [" + d.stationName_ + "] is already in the map");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::collectStationNames(): cannot find station name for the key [" + d.stationKey_ + "]");

  };    
};



//
int SgVgosDb::procNcString(const QString& str, const QString& subDir, SgVdbVariable& destination)
{
  if (!destination.isEmpty())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::procNcString(): duplicate: [" + destination.getStub() + "] vs [" + str + "]");
std::cout << "      Fix me: got a duplicate: was [" << qPrintable(destination.getStub()) 
          << "] became [" << qPrintable(str) << "]\n";
  };
  if (str.size() > 0)
  {
    QString                     name(str.simplified());
    if (name.right(3).contains(".nc", Qt::CaseInsensitive))
      name = name.left(name.size() - 3);
    destination.parseString(name);
    destination.setSubDir(subDir);
    destination.setFileName(subDir.size()==0 ? str.simplified() : subDir + "/" + str.simplified());
    destination.setFileName4Output(str.simplified());
    activeVars_ << &destination;
  };
  return 1;
};



//
int SgVgosDb::procNcStringSpec(const QString& str, const QString& subDir, SgVdbVariable& destination)
{
  if (!destination.isEmpty())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::procNcStringSpec(): duplicate: [" + destination.getStub() + "] vs [" + str + "]");
std::cout << "      Fix me: got a duplicate: was [" << qPrintable(destination.getStub()) 
          << "] became [" << qPrintable(str) << "]\n";
  };
  if (str.size() > 0)
  {
    QString                     name(str.simplified());
    if (name.right(3).contains(".nc", Qt::CaseInsensitive))
      name = name.left(name.size() - 3);
    destination.parseString(name);
    destination.setSubDir(subDir);
    destination.setFileName(subDir.size()==0 ? str.simplified() : subDir + "/" + str.simplified());
    destination.setFileName4Output(str.simplified());
    activeVars_ << &destination;
  };
  return 1;
};



//
bool SgVgosDb::isValidNcFile(const QString& fileName)
{
  bool                          isOk(true);
  int                           ncid, rc;

  if ((rc=nc_open(qPrintable(fileName), 0, &ncid)) != NC_NOERR)
  {
    printf("%s\n", nc_strerror(rc));
    isOk = false;
  };

  if (isOk && (rc=nc_close(ncid)) != NC_NOERR)
  {
    printf("%s\n", nc_strerror(rc));
    isOk = false;
  };
  
  if (isOk)
    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
    "::isValidNcFile(): the netCDF file " + fileName + " looks ok");
  else
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
    "::isValidNcFile(): got an error while staring at " + fileName);
  
  return isOk;
};



//
bool SgVgosDb::isValidNcFiles()
{
  bool                          isOk;
  isOk = true;
  for (int i=0; i<activeVars_.size() && isOk; i++)
    isOk = isValidNcFile(path2RootDir_ + "/" + activeVars_.at(i)->getFileName());
  return isOk;
};


//
void SgVgosDb::dump2stdout()
{
  // individual items:
  std::cout
    << "Individual variables:\n"
    << "  path2RootDir_          : [" << qPrintable(path2RootDir_) << "]\n"
    << "  wrapperFileName_       : [" << qPrintable(wrapperFileName_) << "]\n"
    << "  dateOfCreation_        : [" << qPrintable(dateOfCreation_.toString()) << "]\n";

  std::cout << "History part:\n";
//  QMap<QString, HistoryDescriptor>::iterator  ith;
//  for (ith = historyDescriptorByName_.begin(); ith!= historyDescriptorByName_.end(); ++ith)
  for (int i=0; i<historyDescriptors_.size(); i++)
  {
//  const HistoryDescriptor    &hd=ith.value();
    const HistoryDescriptor    &hd=historyDescriptors_.at(i);
//  std::cout << " Process " << qPrintable(ith.key()) << "\n";
    std::cout << " Process " << qPrintable(hd.processName_) << "\n";
    std::cout 
      << "  processName_         : [" << qPrintable(hd.processName_) << "]\n"
      << "  epochOfCreation_     : [" << qPrintable(hd.epochOfCreation_.toString()) << "]\n"
      << "  creator_             : [" << qPrintable(hd.creator_) << "]\n"
      << "  defaultDir_          : [" << qPrintable(hd.defaultDir_) << "]\n"
      << "  historyFileName_     : [" << qPrintable(hd.historyFileName_) << "]\n"
      << "  inputWrapperFileName_: [" << qPrintable(hd.inputWrapperFileName_) << "]\n"
      << "  version_             : [" << qPrintable(hd.version_) << "]\n"
      << "  isMk3Compatible_     : [" << (hd.isMk3Compatible_?"true":"false") << "]\n"
      ;
  };
  std::cout 
    << "  sessionCode_           : [" << qPrintable(sessionCode_) << "]\n"
    << "  sessionName_           : [" << qPrintable(sessionName_) << "]\n"
    << "  vHead_                 : [" << qPrintable(vHead_.compositeName()) << "]\n"
    << "  vGroupBLWeights_       : [" << qPrintable(vGroupBLWeights_.compositeName()) << "]\n"
    << "  vClockBreak_           : [" << qPrintable(vClockBreak_.compositeName()) << "]\n"
    << "  vLeapSecond_           : [" << qPrintable(vLeapSecond_.compositeName()) << "]\n"
    << "  vMiscCable_            : [" << qPrintable(vMiscCable_.compositeName()) << "]\n"
    << "  vMiscFourFit_          : [" << qPrintable(vMiscFourFit_.compositeName()) << "]\n"
    << "  vStationApriori_       : [" << qPrintable(vStationApriori_.compositeName()) << "]\n"
    << "  vSourceApriori_        : [" << qPrintable(vSourceApriori_.compositeName()) << "]\n"
    << "  vClockApriori_         : [" << qPrintable(vClockApriori_.compositeName()) << "]\n"
    << "  vEccentricity_         : [" << qPrintable(vEccentricity_.compositeName()) << "]\n"
    << "  vAntennaApriori_       : [" << qPrintable(vAntennaApriori_.compositeName()) << "]\n"
    << "  vStationCrossRef_      : [" << qPrintable(vStationCrossRef_.compositeName()) << "]\n"
    << "  vSourceCrossRef_       : [" << qPrintable(vSourceCrossRef_.compositeName()) << "]\n"
    << "  vScanTimeUTC_          : [" << qPrintable(vScanTimeUTC_.compositeName()) << "]\n"
    << "  vScanName_             : [" << qPrintable(vScanName_.compositeName()) << "]\n"
    << "  vERPApriori_           : [" << qPrintable(vErpApriori_.compositeName()) << "]\n"
    << "  vEphemeris_            : [" << qPrintable(vEphemeris_.compositeName()) << "]\n"
    << "  vNutationEqx_kWahr_    : [" << qPrintable(vNutationEqx_kWahr_.compositeName()) << "]\n"
    << "  vNutationEqx_          : [" << qPrintable(vNutationEqx_.compositeName()) << "]\n"
    << "  vNutationNro_          : [" << qPrintable(vNutationNro_.compositeName()) << "]\n"
    << "  vRot_CF2J2K_           : [" << qPrintable(vRot_CF2J2K_.compositeName()) << "]\n"
    << "  vCorrRootFile_         : [" << qPrintable(vCorrRootFile_.compositeName()) << "]\n"
    << "  vObservationTimeUTC_   : [" << qPrintable(vObservationTimeUTC_.compositeName()) << "]\n"
    << "  vBaseline_             : [" << qPrintable(vBaseline_.compositeName()) << "]\n"
    << "  vSource_               : [" << qPrintable(vSource_.compositeName()) << "]\n"
    << "  vDiffTec_              : [" << qPrintable(vDiffTec_.compositeName()) << "]\n"
    << "  vCal_Bend_             : [" << qPrintable(vCal_Bend_.compositeName()) << "]\n"
    << "  vCal_BendSun_          : [" << qPrintable(vCal_BendSun_.compositeName()) << "]\n"
    << "  vCal_BendSunHigher_    : [" << qPrintable(vCal_BendSunHigher_.compositeName()) << "]\n"
    << "  vCal_EarthTide_        : [" << qPrintable(vCal_EarthTide_.compositeName()) << "]\n"
    << "  vCal_FeedCorrection_   : [" << qPrintable(vCal_FeedCorrection_.compositeName()) << "]\n"
    << "  vCal_HfErp_            : [" << qPrintable(vCal_HfErp_.compositeName()) << "]\n"
    << "  vCal_OceanLoad_        : [" << qPrintable(vCal_OceanLoad_.compositeName()) << "]\n"
    << "  vCal_OceanLoadOld_     : [" << qPrintable(vCal_OceanLoadOld_.compositeName()) << "]\n"
    << "  vCal_OceanPoleTideLoad_: [" << qPrintable(vCal_OceanPoleTideLoad_.compositeName()) << "]\n"
    << "  vCal_Parallax_         : [" << qPrintable(vCal_Parallax_.compositeName()) << "]\n"
    << "  vCal_PoleTide_         : [" << qPrintable(vCal_PoleTide_.compositeName()) << "]\n"
    << "  vCal_PoleTideOldRestore_:[" << qPrintable(vCal_PoleTideOldRestore_.compositeName()) << "]\n"
    << "  vCal_TiltRemover_      : [" << qPrintable(vCal_TiltRemover_.compositeName()) << "]\n"
    << "  vCal_Wobble_           : [" << qPrintable(vCal_Wobble_.compositeName()) << "]\n"
    << "  vCal_HfLibration_      : [" << qPrintable(vCal_HfLibration_.compositeName()) << "]\n"
    << "  vCal_HiFreqLibration_  : [" << qPrintable(vCal_HiFreqLibration_.compositeName()) << "]\n"
    << "  vEdit_                 : [" << qPrintable(vEdit_.compositeName()) << "]\n"
    << "  vNGSQualityFlag_       : [" << qPrintable(vNGSQualityFlag_.compositeName()) << "]\n"
    << "  vObsCrossRef_          : [" << qPrintable(vObsCrossRef_.compositeName()) << "]\n"
    << "  vFeedRotNet_           : [" << qPrintable(vFeedRotNet_.compositeName()) << "]\n"
    << "  vDelayTheoretical_     : [" << qPrintable(vDelayTheoretical_.compositeName()) << "]\n"
    << "  vRateTheoretical_      : [" << qPrintable(vRateTheoretical_.compositeName()) << "]\n"
    << "  vPart_Bend_            : [" << qPrintable(vPart_Bend_.compositeName()) << "]\n"
    << "  vPart_Gamma_           : [" << qPrintable(vPart_Gamma_.compositeName()) << "]\n"
    << "  vPart_Erp_             : [" << qPrintable(vPart_Erp_.compositeName()) << "]\n"
    << "  vPart_NutationEqx_     : [" << qPrintable(vPart_NutationEqx_.compositeName()) << "]\n"
    << "  vPart_NutationNro_     : [" << qPrintable(vPart_NutationNro_.compositeName()) << "]\n"
    << "  vPart_Parallax_        : [" << qPrintable(vPart_Parallax_.compositeName()) << "]\n"
    << "  vPart_PoleTide_        : [" << qPrintable(vPart_PoleTide_.compositeName()) << "]\n"
    << "  vPart_Precession_      : [" << qPrintable(vPart_Precession_.compositeName()) << "]\n"
    << "  vPart_RaDec_           : [" << qPrintable(vPart_RaDec_.compositeName()) << "]\n"
    << "  vPart_Xyz_             : [" << qPrintable(vPart_Xyz_.compositeName()) << "]\n"
  ;
  std::cout << " Program Solve: \n"
    << "  vAtmSetup_             : [" << qPrintable(progSolveDescriptor_.
                                                          vAtmSetup_.compositeName()) << "]\n"
    << "  vClockSetup_           : [" << qPrintable(progSolveDescriptor_.
                                                          vClockSetup_.compositeName()) << "]\n"
    << "  vErpSetup_             : [" << qPrintable(progSolveDescriptor_.
                                                          vErpSetup_.compositeName()) << "]\n"
    << "  vIonoSetup_            : [" << qPrintable(progSolveDescriptor_.
                                                          vIonoSetup_.compositeName()) << "]\n"
    << "  vCalibrationSetup_     : [" << qPrintable(progSolveDescriptor_.
                                                          vCalibrationSetup_.compositeName()) << "]\n"
    << "  vBaselineClockSetup_   : [" << qPrintable(progSolveDescriptor_.
                                                          vBaselineClockSetup_.compositeName()) << "]\n"
    << "  vCalcErp_              : [" << qPrintable(progSolveDescriptor_.
                                                          vCalcErp_.compositeName()) << "]\n"
    << "  vSelectionStatus_      : [" << qPrintable(progSolveDescriptor_.
                                                          vSelectionStatus_.compositeName()) << "]\n"
    << "  vCalcInfo_             : [" << qPrintable(progSolveDescriptor_.
                                                          vCalcInfo_.compositeName()) << "]\n"
    << "  vScanTimeMJD_          : [" << qPrintable(progSolveDescriptor_.
                                                          vScanTimeMJD_.compositeName()) << "]\n"
    << "  vIonoBits_             : [" << qPrintable(progSolveDescriptor_.
                                                          vIonoBits_.compositeName()) << "]\n"
    << "  vUnPhaseCalFlag_       : [" << qPrintable(progSolveDescriptor_.
                                                          vUnPhaseCalFlag_.compositeName()) << "]\n"
    << "  vFractC_               : [" << qPrintable(progSolveDescriptor_.
                                                          vFractC_.compositeName()) << "]\n"
    << "  vUserSup_              : [" << qPrintable(progSolveDescriptor_.
                                                          vUserSup_.compositeName()) << "]\n"
  ;
  //
  std::cout << "\nOn per band basis:\n";
  QMap<QString, BandData>::iterator itt;
  for (itt=bandDataByName_.begin(); itt!=bandDataByName_.end(); ++itt)
  {
    const BandData&             bd=itt.value();
    std::cout << "Band \"" << qPrintable(itt.key()) << "\":\n";
    std::cout 
    << "  vChannelInfo           : [" << qPrintable(bd.vChannelInfo_.compositeName()) << "]\n"
    << "  vPhaseCalInfo          : [" << qPrintable(bd.vPhaseCalInfo_.compositeName()) << "]\n"
    << "  vCorrInfo              : [" << qPrintable(bd.vCorrInfo_.compositeName()) << "]\n"
    << "  vQualityCode           : [" << qPrintable(bd.vQualityCode_.compositeName()) << "]\n"
    << "  vRefFreq               : [" << qPrintable(bd.vRefFreq_.compositeName()) << "]\n"
    << "  vAmbigSize             : [" << qPrintable(bd.vAmbigSize_.compositeName()) << "]\n"
    << "  vSNR                   : [" << qPrintable(bd.vSNR_.compositeName()) << "]\n"
    << "  vCorrelation           : [" << qPrintable(bd.vCorrelation_.compositeName()) << "]\n"
    << "  vPhase                 : [" << qPrintable(bd.vPhase_.compositeName()) << "]\n"
    << "  vSBDelay               : [" << qPrintable(bd.vSBDelay_.compositeName()) << "]\n"
    << "  vGroupDelay            : [" << qPrintable(bd.vGroupDelay_.compositeName()) << "]\n"
    << "  vGroupRate             : [" << qPrintable(bd.vGroupRate_.compositeName()) << "]\n"
    << "  vDelayDataFlag         : [" << qPrintable(bd.vDelayDataFlag_.compositeName()) << "]\n"
    << "  vDataFlag              : [" << qPrintable(bd.vDataFlag_.compositeName()) << "]\n"
    << "  vPhaseDelayFull        : [" << qPrintable(bd.vPhaseDelayFull_.compositeName()) << "]\n"
    << "  vUVFperAsec            : [" << qPrintable(bd.vUVFperAsec_.compositeName()) << "]\n"
    << "  vEffFreq_EqWt          : [" << qPrintable(bd.vEffFreq_EqWt_.compositeName()) << "]\n"
    << "  vEffFreq               : [" << qPrintable(bd.vEffFreq_.compositeName()) << "]\n"
    << "  vCal_SlantPathIonoGroup: [" << qPrintable(bd.vCal_SlantPathIonoGroup_.compositeName()) << "]\n"
    << "  vCal_FeedCorrection    : [" << qPrintable(bd.vCal_FeedCorrection_.compositeName()) << "]\n"
    << "  vCal_Unphase           : [" << qPrintable(bd.vCal_Unphase_.compositeName()) << "]\n"
    << "  vNumGroupAmbig         : [" << qPrintable(bd.vNumGroupAmbig_.compositeName()) << "]\n"
    << "  vNumPhaseAmbig         : [" << qPrintable(bd.vNumPhaseAmbig_.compositeName()) << "]\n"
    << "  vGroupDelayFull        : [" << qPrintable(bd.vGroupDelayFull_.compositeName()) << "]\n\n"
    << "  vMiscFourFit           : [" << qPrintable(bd.vMiscFourFit_.compositeName()) << "]\n\n"
    ;
  };
  //
  std::cout << "On per station basis:\n";
  QMap<QString, StationDescriptor>::iterator  it;
  for (it = stnDescriptorByKey_.begin(); it!= stnDescriptorByKey_.end(); ++it)
  {
    const StationDescriptor& sd=it.value();
    std::cout << "Station " << qPrintable(it.key()) << "\n";
    std::cout 
      << " stationName            : [" << qPrintable(sd.stationName_) << "]\n"
      << " vTimeUTC               : [" << qPrintable(sd.vTimeUTC_.compositeName()) << "]\n"
      << " vPart-ZenithPathTropDry: [" << qPrintable(sd.vPart_ZenithPathTropDry_.compositeName()) << "]\n"
      << " vPart-ZenithPathTropWet: [" << qPrintable(sd.vPart_ZenithPathTropWet_.compositeName()) << "]\n"
      << " vPart-HorizonGrad      : [" << qPrintable(sd.vPart_HorizonGrad_.compositeName()) << "]\n"
      << " vCal-SlantPathTropDry  : [" << qPrintable(sd.vCal_SlantPathTropDry_.compositeName()) << "]\n"
      << " vCal-SlantPathTropWet  : [" << qPrintable(sd.vCal_SlantPathTropWet_.compositeName()) << "]\n"
      << " vAzEl                  : [" << qPrintable(sd.vAzEl_.compositeName()) << "]\n"
      << " vPart-AxisOffset       : [" << qPrintable(sd.vPart_AxisOffset_.compositeName()) << "]\n"
      << " vCal-AxisOffset        : [" << qPrintable(sd.vCal_AxisOffset_.compositeName()) << "]\n"
      << " vFeedRotation          : [" << qPrintable(sd.vFeedRotation_.compositeName()) << "]\n"
      << " vDis-OceanLoad         : [" << qPrintable(sd.vDis_OceanLoad_.compositeName()) << "]\n"
      << " vRefClockOffset        : [" << qPrintable(sd.vRefClockOffset_.compositeName()) << "]\n"
      << " vTsys                  : [" << qPrintable(sd.vTsys_.compositeName()) << "]\n"
      << " vCal-StationOceanLoad  : [" << qPrintable(sd.vCal_OceanLoad_.compositeName()) << "]\n"
      << " vMet                   : [" << qPrintable(sd.vMet_.compositeName()) << "]\n"
      << " vCal-Cable             : [" << qPrintable(sd.vCal_Cable_.compositeName()) << "]\n\n"
      << " vCal-CableCorrections  : [" << qPrintable(sd.vCal_CableCorrections_.compositeName()) << "]\n\n"
      << " vCal-CblCorrections    : [" << qPrintable(sd.vCal_CblCorrections_.compositeName()) << "]\n\n"
      ;
  };
  //
  std::cout << "Registered NC files:\n";
  for (int i=0; i<activeVars_.size(); i++)
  {
    std::cout 
      << "  [" << qPrintable(activeVars_.at(i)->getFileName()) << "]\n";
  };
};



//
bool SgVgosDb::checkFormat(const QList<FmtChkVar*>& fmt, const SgNetCdf& ncdf, bool ok2fail)
{
  bool                          isOk=true;
  for (int i=0; i<fmt.size(); i++)
  {
    const FmtChkVar            *fmtVar=fmt.at(i);
    SgNcdfVariable             *var=ncdf.lookupVar(fmtVar->name());
    if (!var)
    {
      if (!ok2fail)
        logger->write(fmtVar->isMandatory()?SgLogger::ERR:SgLogger::INF, SgLogger::IO_NCDF, className() + 
          "::checkFormat(): cannot find the variable \"" + fmtVar->name() + "\" in the file " + 
          ncdf.getFileName());
      if (fmtVar->isMandatory())
        isOk = false;
    };
    if (var) // can be NULL
    {
      if (var->getTypeOfData() != fmtVar->typeOfData())
      {
        if (var->getName() != "FRNGERR")
        {
          logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
            "::checkFormat(): enexpected data type of the variable \"" + fmtVar->name() + "\"");
          return false;
        }
        else
          continue; // skip checking
      };
      const QList<int>           &dims=fmtVar->dims();
      if (dims.size() && dims.size() != var->dimensions().size()) // if dims.size()==0 the number of
      {                                                           // dimensions may vary
        for (int j=0; j<dims.size(); j++)
        {
          if (dims.at(j)==SD_Any && dims.size()>var->dimensions().size())
          {
            var->addDimension(dUnity, j);
            logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
              "::checkFormat(): the dimension of \"" + fmtVar->name() + 
              "\" has been expanded by unity at position " + QString("").setNum(j) + 
              " of " + QString("").setNum(var->dimensions().size()));
          };
        };
        if (dims.size() != var->dimensions().size()) // prev operation did not help, complain:
        {
          logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
            "::checkFormat(): number of dimensions, " + QString("").setNum(var->dimensions().size()) + 
            ", missmatch for \"" + fmtVar->name() + "\", expected " + QString("").setNum(dims.size()));
          return false;
        }
      };
      for (int j=0; j<dims.size(); j++)
      {
        // special dimensions are not checked :-|
        if (dims.at(j) > 0) // regular dimension:
        {
          if (dims.at(j) != var->dimensions().at(j)->getN())
          {
            logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
              "::checkFormat(): dimension #" + QString("").setNum(j) + " missmatch for [" + 
              fmtVar->name() + "], expected " + QString("").setNum(dims.at(j)) + " got " + 
              QString("").setNum(var->dimensions().at(j)->getN()));
            return false;
          };
        }
        else                // check selected special dimensions:
        {
          if (dims.at(j)==SD_NumObs &&
              numOfObs_>0           &&
              var->dimensions().at(j)->getN()!=numOfObs_)
          {
            logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
              "::checkFormat(): the special dimension numObs missmatch for [" + 
              fmtVar->name() + "], expected " + QString("").setNum(numOfObs_) + " got " + 
              QString("").setNum(var->dimensions().at(j)->getN()));
            return false;
          }
          else if (dims.at(j)==SD_NumScans  &&
              numOfScans_>0                 &&
              var->dimensions().at(j)->getN()!=numOfScans_)
          {
            logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
              "::checkFormat(): the special dimension numScans missmatch for [" + 
              fmtVar->name() + "], expected " + QString("").setNum(numOfScans_) + " got " + 
              QString("").setNum(var->dimensions().at(j)->getN()));
            return false;
          };
        };
      };
    };
  };
  return isOk;
};



//
const QString SgVgosDb::guessDimName(const QString& varName, int len)
{
  QString                       str("");

  if (len == 1)
    str = dUnity.getName();
  else if (len > 1)
    str.sprintf("DimX%06d", len);
  else
  {
    //  NumBaselines -> PhaseBLWeights.nc
    //
    if (len == SD_NumObs)
      str = "NumObs";
    else if (len == SD_NumScans)
      str = "NumScans";
    else if (len == SD_NumChans)
      str = "NumChannels";
    else if (len == SD_2NumChans)
      str = "NumChannels_x_2";
    else if (len == SD_NumStnPts)
      str = "NumStatScan";
    else if (len == SD_NumSrc)
      str = "NumSource";
    else if (len == SD_NumStn)
      str = "NumStation";
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::guessDimName(): uknown special dimension " + QString("").setNum(len) + " of the var " +
        varName);
      str = "Unknown";
    };
  };
  return str;
};



//
int SgVgosDb::guessDimVal(const QString& varName, int d, const QString& auxKey)
{
  if (d>0)
    return d;
  if (d == SD_NumObs)
    d = numOfObs_;
  else if (d == SD_NumScans)
    d = numOfScans_;
  else if (d == SD_NumChans)
    d = numOfChan_;
  else if (d == SD_2NumChans)
    d = 2*numOfChan_;
  else if (d == SD_NumStnPts)
  {
    if (stnDescriptorByName_.contains(auxKey))
      d = stnDescriptorByName_[auxKey]->numOfPts_;
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
        "::guessDimVal(): cannot find the station [" + auxKey.simplified() + 
        "] in the container numOfObsByStn_");
      d = 0;
    };
  }
  else if (d == SD_NumSrc)
    d = numOfSrc_;
  else if (d == SD_NumStn)
    d = numOfStn_;
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
      "::guessDimVal(): uknown special dimension " + QString("").setNum(d) + " of the var " + varName);
    d = 0;
  };
  return d;
};



//
bool SgVgosDb::setupFormat(const QList<FmtChkVar*>& fmt, SgNetCdf& ncdf,
  const QString& stationKey, const QString& bandKey)
{
  QString                       str("");
  for (int i=0; i<fmt.size(); i++)
  {
    const FmtChkVar            *fmtVar=fmt.at(i);
    SgNcdfVariable             *var=new SgNcdfVariable;
    int                         len;
//--
    var->setName(fmtVar->name());
    var->setTypeOfData(fmtVar->typeOfData());
    var->setId(i);
    // dimensions:
    const QList<int>           &dims=fmtVar->dims();
    for (int j=0; j<dims.size(); j++)
    {
      str = guessDimName(fmtVar->name(), dims.at(j));
      len = guessDimVal(str, dims.at(j), stationKey);
      var->addDimension(str, len, -1);
    };
    // data:
    var->allocateData();
    // attributes:
    SgNcdfAttribute            *attr=NULL;
    if ((len=fmtVar->attLCode().size()) && !fmtVar->have2hideLCode())          // LCODE
    {
      attr = new SgNcdfAttribute;
      attr->setName("LCODE");
      attr->setTypeOfData(NC_CHAR);
      attr->setNumOfElements(len);
      attr->setData(qPrintable(fmtVar->attLCode()), len);
      var->addAttribute(attr);
    };
    if ((len=fmtVar->attDefinition().size()))     // definition
    {
      attr = new SgNcdfAttribute;
      attr->setName("Definition");
      attr->setTypeOfData(NC_CHAR);
      attr->setNumOfElements(len);
      attr->setData(qPrintable(fmtVar->attDefinition()), len);
      var->addAttribute(attr);
    };
    if ((len=fmtVar->attUnits().size()))          // units
    {
      attr = new SgNcdfAttribute;
      attr->setName("Units");
      attr->setTypeOfData(NC_CHAR);
      attr->setNumOfElements(len);
      attr->setData(qPrintable(fmtVar->attUnits()), len);
      var->addAttribute(attr);
    };
    if ((len=fmtVar->attBand().size()) && bandKey.size()) // band
    {
      attr = new SgNcdfAttribute;
      attr->setName("Band");
      attr->setTypeOfData(NC_CHAR);
      attr->setNumOfElements(bandKey.size());
      attr->setData(qPrintable(bandKey), len);
      var->addAttribute(attr);
    };
    str = SgMJD::currentMJD().toUtc().toString(SgMJD::F_Simple) + " UTC";
    len = str.size();
    attr = new SgNcdfAttribute;
    attr->setName("CreateTime");
    attr->setTypeOfData(NC_CHAR);
    attr->setNumOfElements(len);
    attr->setData(qPrintable(str), len);
    var->addAttribute(attr);
    //
    ncdf.registerVariable(var);
  };
  //
  return true;
};



//
void SgVgosDb::makeWorkAround4KnownBugs()
{
  // performs work around bugs that were found in earlier versions:
  //
  // 1. SgLib-0.6.2 and the olders ones did not put "UVFperAsec" in a wrapper file (the coresponding
  // netCDF files were created).
  
  // first, check for history:
  bool                          need2searchForUvf;
  need2searchForUvf = true;
  for (QMap<QString, BandData>::iterator it=bandDataByName_.begin(); it!=bandDataByName_.end(); ++it)
    if (!it.value().vUVFperAsec_.isEmpty())
      need2searchForUvf = false;
  if (need2searchForUvf)
  {
    bool                        calcProcessFound=false;
    SgVersion                   version2correct("temp", 0, 6, 2, QString(""), tZero);
    QRegExp                     reVersions("(\\d+).(\\d+).(\\d+)/(\\d+).(\\d+).(\\d+)");
    bool                        isOk;
    for (int i=historyDescriptors_.size()-1; 0<=i && !calcProcessFound; i--)
    {
      const HistoryDescriptor&  hd=historyDescriptors_.at(i);
      if (hd.processName_ == "SgLib/vgosDbCalc")
      {
        calcProcessFound = true;
        if (reVersions.indexIn(hd.version_) != -1)
        {
          int                   nMaj, nMin, nTyn;
          nMaj = nMin = nTyn = 0;
          nMaj = reVersions.cap(1).toInt(&isOk);
          if (isOk)
          {
            nMin = reVersions.cap(2).toInt(&isOk);
            if (isOk)
            {
              nTyn = reVersions.cap(3).toInt(&isOk);
              if (isOk)
              {
                SgVersion       foundVersion("temp", nMaj, nMin, nTyn, QString(""), tZero);
                if (foundVersion <= version2correct)
                {
                  logger->write(SgLogger::INF, SgLogger::IO_NCDF, className() + 
                    "::makeWorkAround4KnownBugs(): found data that could be corrected");
                  // ok, check each band:
                  for (QMap<QString, BandData>::iterator it=bandDataByName_.begin(); 
                    it!=bandDataByName_.end(); ++it)
                  {
                    BandData&   bd=it.value();
                    const QString&
                                bandKey=it.key();
                    logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
                      "::makeWorkAround4KnownBugs(): checking " + bandKey + "-band");
                    if (QFile::exists(path2RootDir_ + "/ObsDerived/UVFperAsec_b" + bandKey + ".nc"))
                    {
                      procNcString("UVFperAsec_b" + bandKey + ".nc",  "ObsDerived",  bd.vUVFperAsec_);
                      logger->write(SgLogger::DBG, SgLogger::IO_NCDF, className() + 
                        "::makeWorkAround4KnownBugs(): found a missed variable " + 
                        bd.vUVFperAsec_.compositeName());
                    };
                  };
                };
              }
              else
                logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
                  "::makeWorkAround4KnownBugs(): cannot convert \"" + reVersions.cap(3) + "\" to int");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
                "::makeWorkAround4KnownBugs(): cannot convert \"" + reVersions.cap(2) + "\" to int");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_NCDF, className() + 
              "::makeWorkAround4KnownBugs(): cannot convert \"" + reVersions.cap(1) + "\" to int");
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() + 
            "::makeWorkAround4KnownBugs(): the signature \"SgLib/vgosDbCalc\" has been found, but the "
            "version is not in the expected form: \"" + hd.version_ + "\"");
      };
    };
  };
  //
  //
  // other bugs go here:
  //
};



//
const QList<QString> SgVgosDb::listOfInputFiles() const
{
  QString                       path2root("");
  QList<QString>                list;
  
  if (path2RootDir_.size() && path2RootDir_ != "./")
    path2root = path2RootDir_ + "/";
  
  list << path2root + wrapperFileName_;
  for (int i=0; i<activeVars_.size(); i++)
    list << path2root + activeVars_.at(i)->getFileName() ;
 
  return list;
};
/*=====================================================================================================*/









/*=====================================================================================================*/
//
// constants:
//
// Variables:
SgVgosDb::FmtChkVar
  fcSecond              ("Second",              NC_DOUBLE,  true,   QList<int>()  << SD_Any,
    "SEC TAG ", "Seconds part of UTC TAG.........", "second"),
  fcYmdhm               ("YMDHM",               NC_SHORT,   true,   QList<int>()  << SD_Any    << 5,
    "UTC TAG ", "Epoch UTC YMDHM................."),
//  fcUtc4                ("UTC TAG4",            NC_SHORT,   false,  QList<int>()  << SD_Any    << 6,
//    "UTC TAG4", "Epoch UTC YMDHMS (4 digit year)."),

  // 
  fcCalIonoGroup        ("Cal-SlantPathIonoGroup",
                                                NC_DOUBLE,  true,   QList<int>()  << SD_NumObs << 2,
    "ION CORR", "Ion correction. Add to theo. sec", "second", "?"),
  fcCalIonoGroupSigma   ("Cal-SlantPathIonoGroupSigma",
                                                NC_DOUBLE,  true,   QList<int>()  << SD_NumObs << 2,
    "IONRMS  ", "Ion correction to sigma. sec", "second", "?"),
  fcCalIonoGroupDataFlag("Cal-SlantPathIonoGroupDataFlag",
                                                NC_SHORT,   false,  QList<int>()  << SD_NumObs,
    "", "0=OK, -1=Missing,  -2=bad", "", "?"),
  fcIonoBits            ("IonoBits",            NC_SHORT,   true,   QList<int>()  << SD_NumObs,
    "ION_BITS", "ICORR for full ion tracking."),
  fcDelayFlag           ("DelayFlag",           NC_SHORT,   true,   QList<int>()  << SD_NumObs,
    "DELUFLAG", "Delay unweight flag"),
  fcRateFlag            ("RateFlag",            NC_SHORT,   true,   QList<int>()  << SD_NumObs,
    "RATUFLAG", "Delay rate unweight flag........"),
  fcPhaseFlag           ("PhaseFlag",           NC_SHORT,   false,  QList<int>()  << SD_NumObs,
    "PHSUFLAG", ""),
  fcUserSup             ("UserSup",             NC_SHORT,   false,  QList<int>()  << SD_NumObs,
    "UACSUP  ", "User action for suppression"),
  fcNumGroupAmbig       ("NumGroupAmbig",       NC_SHORT,   true,   QList<int>()  << SD_NumObs,
    "# AMBIG ", "Number of group delay ambiguities", "", "?"),
  fcNumGroupSubAmbig    ("NumGroupSubAmbig",    NC_SHORT,   false,  QList<int>()   << SD_NumObs,
    "N/A     ", "Number of group delay subambiguities (test purposes)", "", "?"),
  fcNumPhaseAmbig       ("NumPhaseAmbig",       NC_INT,     true,   QList<int>()  << SD_NumObs,
    "#PAMBIG ", "Number of phase delay ambiguities", "", "?"),
  fcAtmInterval         ("AtmInterval",         NC_DOUBLE,  false,  QList<int>()  << SD_Any,
    "ATM_INTV", "Batchmode atmos interval - hours"),
  fcAtmRateConstraint   ("AtmRateConstraint",   NC_DOUBLE,  true,   QList<int>()  << SD_Any,
    "ATM_CNST", "Atmosphere constraint. ps/hr"),
  fcAtmRateSite         ("AtmRateStationList",  NC_CHAR,    false,  QList<int>()  << SD_Any <<  8,
    "AC_SITES", "Site list for atmos constraints"),
  fcClockInterval       ("ClockInterval",       NC_DOUBLE,  false,  QList<int>()  << SD_Any,
    "CLK_INTV", "Batchmode clock interval - hours"),
  fcClockRateConstraint ("ClockRateConstraint", NC_DOUBLE,  true,   QList<int>()  << SD_Any,
    "CLK_CNST", "Clock constraint-Parts in 1.e14"),
  fcClockRateName       ("ClockRateConstraintStationList", 
                                                NC_CHAR,    false,  QList<int>()  << SD_Any <<  8,
    "CC_SITES", "Site list for clocks constraints"),
  fcPmOffsetConstraint  ("WobOffsetConstraint", NC_DOUBLE,  true,   QList<int>()  << 1,
    "", "Polar Motion Offset Constraint"),
  fcUtOffsetConstraint  ("UT1OffsetConstraint", NC_DOUBLE,  true,   QList<int>()  << 1,
    "", "UT1 Offset Constraint"),
  fcIonoSolveFlag       ("IonoSolveFlag",       NC_SHORT,   true,   QList<int>()  << SD_NumStn,
    "", "Bit flag indicating station has iono correction"),
  fcIonoStationList     ("IonoStationList",     NC_CHAR,    true,   QList<int>()  << SD_NumStn << 8,
    "", "Stations with ionocorrection"),
  fcGroupBLWeights      ("GroupBLWeights",      NC_DOUBLE,  true,   QList<int>()  <<  2 << SD_Any,
    "ERROR K ", "Group delay and rate re-weighting constants."),
  fcGroupBLWeightName   ("GroupBLWeightStationList",   
                                                NC_CHAR,    true,   QList<int>()  << SD_Any << 2 <<  8,
    "ERROR BL", "B.L.names for formal errors"),
  fcSrcSelectionFlag    ("SourceSelectionFlag", NC_SHORT,   true,   QList<int>()  << SD_NumSrc,
    "SOURSTAT", "Source selection status bit-mapped array."),
  fcBlnSelectionFlag    ("BaselineSelectionFlag",
                                                NC_SHORT,   true,   QList<int>()  << SD_NumStn 
                                                                                  << SD_NumStn,
    "BASLSTAT", "Baseline selection bit maped array. 1=some obs, etc."),
  fcSrcSelectionName    ("SourceSelectionName", NC_CHAR,    false,  QList<int>()  << SD_NumSrc << 8,
    "--NEW-- ", "Source names for Source selection array"),
  fcBlnSelectionName    ("BaselineSelectionName",
                                                NC_CHAR,    false,  QList<int>()  << SD_NumStn << 8,
    "--NEW-- ", "Station names for Baseline selection array"),
  fcBaselineClock       ("BaselineClock",       NC_CHAR,    true,   QList<int>()  << SD_Any << 2 << 8,
    "BLDEPCKS", "Bl-dependent clock list"),
  fcEccentricityType    ("EccentricityType",    NC_CHAR,    false,  QList<int>()  << SD_NumStn <<  2,
    "ECCTYPES", "Eccentricity type: XY or NE"),
  fcEccentricityName    ("EccentricityStationList",
                                                NC_CHAR,    true,   QList<int>()  << SD_NumStn <<  8,
    "--NEW-- ", "Station name of the corresponding eccentricity"),
  fcEccentricityMonument("EccentricityMonument",NC_CHAR,    false,  QList<int>()  << SD_NumStn << 10,
    "ECCNAMES", "Eccentricity monument name"),
  fcEccentricityVector  ("EccentricityVector",  NC_DOUBLE,  false,  QList<int>()  << SD_NumStn <<  3,
    "ECCCOORD", "Eccentricity taken from eccentricity file.", "Meter"),
  fcObsCalFlag          ("ObsCalFlag",          NC_SHORT,   false,  QList<int>()  << 1,
    "OBCLFLGS", "Bit set indicate that calibration is recommended."),
//              "Standard contribs configuration"
  fcStatCalFlag         ("StatCalFlag",         NC_SHORT,   true,   QList<int>()  << SD_NumStn,
    "CAL FLGS", "Bit set indicate that calibration is recommended."),
//              "Standard cal configuration"
  fcFlybyFlag           ("FlybyFlag",           NC_SHORT,   false,  QList<int>()  << SD_NumStn <<  7,
    "FCL FLGS", "Standard flcal configuration"),
  fcStatCalName         ("StatCalList",         NC_CHAR,    true,   QList<int>()  << SD_Any << 8,
    "CAL LIST", "Station depedendent calibrations (Cable, Phase,  etc?)"),
  fcFlybyName           ("FlybyList",           NC_CHAR,    false,  QList<int>()  << SD_Any << 8,
    "FCL LIST", "Key to the standard flcal config"),
  fcCalStationName      ("StatCalStationList",  NC_CHAR,    true,   QList<int>()  << SD_NumStn << 8,
    "CALSITES", "List of sites for standard cal"),
  fcObsCalName          ("ObsCalList",          NC_CHAR,    false,  QList<int>()  << SD_Any << 8,
    "OBCLLIST", "Available obs dependent calibrations (poletide, earthdide, ?)"),
  fcClockBreakFlag      ("ClockBreakFlag",      NC_SHORT,   true,   QList<int>()  << SD_Any,
    "BRK_FLAG", "Batchmode clock break flags"),
  fcClockBreakStatus    ("CLKBREAK",            NC_CHAR,    false,  QList<int>()  << 2,
    "CLKBREAK", "Status of clock break existence"),
  fcClockBreakNumber    ("BRK_NUMB",            NC_SHORT,   false,  QList<int>()  << 1,
    "BRK_NUMB", "Number of batchmode clock breaks"),
  fcClockBreakSite      ("ClockBreakStationList",
                                                NC_CHAR,    true,   QList<int>(),
    "BRK_SNAM", "Batchmode clock break stations"),
  fcClockBreakEpoch     ("ClockBreakEpoch",     NC_DOUBLE,  true,   QList<int>() << SD_Any,
    "BRK_EPOC", "Batchmode clock break epochs")
   
    
  ;






// netCDF files:
QList<SgVgosDb::FmtChkVar*>
//  fcfTimeUTC        = QList<SgVgosDb::FmtChkVar*>() << &fcSecond << &fcYmdhm << &fcUtc4,
  fcfTimeUTC        = QList<SgVgosDb::FmtChkVar*>() << &fcSecond << &fcYmdhm,
  fcfCalSlantPathIonoGroup 
                    = QList<SgVgosDb::FmtChkVar*>() << &fcCalIonoGroup << &fcCalIonoGroupSigma
                                                    << &fcCalIonoGroupDataFlag,
  fcfIonoBits       = QList<SgVgosDb::FmtChkVar*>() << &fcIonoBits,
  //
//  fcfEdit           = QList<SgVgosDb::FmtChkVar*>() << &fcDelayFlag << &fcRateFlag << &fcPhaseFlag
//                                                    << &fcUserSup,
 *fcfEdit           = NULL,
  fcfEdit_v1001     = QList<SgVgosDb::FmtChkVar*>() << &fcDelayFlag << &fcRateFlag << &fcPhaseFlag
                                                    << &fcUserSup,
  fcfEdit_v1002     = QList<SgVgosDb::FmtChkVar*>() << &fcDelayFlag << &fcRateFlag << &fcPhaseFlag,
  fcfUserSup        = QList<SgVgosDb::FmtChkVar*>() << &fcUserSup,
  //
  fcfNumGroupAmbig  = QList<SgVgosDb::FmtChkVar*>() << &fcNumGroupAmbig,
  fcfNumGroupAmbigE = QList<SgVgosDb::FmtChkVar*>() << &fcNumGroupAmbig << &fcNumGroupSubAmbig,
  fcfNumPhaseAmbig  = QList<SgVgosDb::FmtChkVar*>() << &fcNumPhaseAmbig,
  fcfAtmSetup       = QList<SgVgosDb::FmtChkVar*>() << &fcAtmInterval << &fcAtmRateConstraint 
                                                    << &fcAtmRateSite,
  fcfClockSetup     = QList<SgVgosDb::FmtChkVar*>() << &fcClockInterval << &fcClockRateConstraint 
                                                    << &fcClockRateName,
  fcfErpSetup       = QList<SgVgosDb::FmtChkVar*>() << &fcPmOffsetConstraint << &fcUtOffsetConstraint,
  fcfIonoSetup      = QList<SgVgosDb::FmtChkVar*>() << &fcIonoSolveFlag << &fcIonoStationList,
  fcfGroupBLWeights = QList<SgVgosDb::FmtChkVar*>() << &fcGroupBLWeights << &fcGroupBLWeightName,
  fcfSelectionStatus= QList<SgVgosDb::FmtChkVar*>() << &fcSrcSelectionFlag << &fcBlnSelectionFlag,
//                                                  << &fcSrcSelectionName << &fcBlnSelectionName,
  fcfBlnClockSetup  = QList<SgVgosDb::FmtChkVar*>() << &fcBaselineClock,
  fcfEccentricity   = QList<SgVgosDb::FmtChkVar*>() << &fcEccentricityType << &fcEccentricityName
                                                    << &fcEccentricityMonument 
                                                    << &fcEccentricityVector,
  fcfCalibrationSetup
                    = QList<SgVgosDb::FmtChkVar*>() << &fcObsCalFlag << &fcStatCalFlag << &fcFlybyFlag 
                                                    << &fcStatCalName << &fcFlybyName 
                                                    << &fcCalStationName << &fcObsCalName,
  fcfClockBreak     = QList<SgVgosDb::FmtChkVar*>() << &fcClockBreakFlag << &fcClockBreakStatus 
                                                    << &fcClockBreakNumber << &fcClockBreakSite
                                                    << &fcClockBreakEpoch

  ;
/*=====================================================================================================*/

