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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>


#include <Sg3dMatrix.h>
#include <SgLogger.h>
#include <SgVlbiAuxObservation.h>
#include <SgVlbiObservation.h>


#include "VcCalc2SessionIfc.h"
#include "vgosDbCalc.h"



#define UNUSED __attribute__((__unused__))



//static const QString          obsoleteLcodesA[] =
static const char*              obsoleteLcodesA[] =
{
  "EUT1TEXT", "EWOBTEXT", "PEP TAPE", "PWOBTEXT", "PUT1TEXT", "REL MESS", "RGD MESS", "THODELAY",
};

//static const QString          obsoleteLcodesR[] =
static const char*              obsoleteLcodesR[] =
{
  "ATM CONT", "ATM PART", "AXIS OLD", "AXO2CONT", "C82ETCON", "CF2J2000", "CFA PART", "CFA22DRY",
  "CFA22WET", "CON PART", "CONSCONT", "DEPS    ", "DPSI    ", "ELASTCON", "ETD PART", "ETD2CONT",
  "ETD3CONT", "ETDKCONT", "ETJMGPAR", "EUT1 INF", "EUT1 PTS", "EWOB INF", "EWOBX&YT", "EQE CONT",
  "EQE DIFF", "FUNDARGS", "GDNUTCON", "HELL EMS", "HELL DEL", "HELL RAT", "LANYIDRY", "LANYINFO",
  "LANYIWET", "NEWEQCON", "NUT 1996", "NUT PART", "NUT2000A", "NUT2000P", "NUT2KXYP", "NUT2KXYS",
  "OLDEQCON", "PERMDEF ", "PMOT2CON", "PMOTNCON", "PRE CFLG", "PRE MESS", "PRE PART", "PUT1 INF",
  "PUT1 PTS", "PWOB INF", "PWOBX&YT", "REL CONT", "REL PART", "RGD CONT", "SHAP DEL", "SHAP RAT",
  "SHAP T62", "SHAPCONT", "THODELAY", "THODRATE", "WAHRCONT", "WET PART", "WOB CONT", "WOBNUTAT",
};



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString VcCalc2SessionIfc::className()
{
  return "VcCalc2SessionIfc";
};


VcCalc2SessionIfc::VcCalc2SessionIfc() :
  inputDbName_(""),
  path2data_(""),
  wrapperFileName_(""),
  bandList_(),
  currentBandKey_(""),
  delAbyLCode_(),
  delIbyLCode_(),
  delJbyLCode_(),
  del4byLCode_(),
  delRbyLCode_(),
  getAbyLCode_(),
  getIbyLCode_(),
  get4byLCode_(),
  putAbyLCode_(),
  putIbyLCode_(),
  putRbyLCode_(),
  put4byLCode_(),
  bandDepPutsByLCode_()
{
  openMode_ = OM_None;
  session_ = NULL;
  vgosDb_ = NULL;
  obs_ = NULL;
  o_ = NULL;
  aux1_ = aux2_ = NULL;
  obsIdx_ = -1;
  numOfObservations_ = 0;
  numOfStations_ = 0;
  numOfSources_ = 0;
  calcCallingMode_ = CCM_None;
  isPrimeBand_ = false;
  
  //
  // fill the maps:
  //

  // dels:
  int                            num;
  delAbyLCode_.clear();
//  num = sizeof(obsoleteLcodesA)/sizeof(QString);
  num = sizeof(obsoleteLcodesA)/sizeof(const char*);
  for (int i=0; i<num; i++)
    delAbyLCode_.insert(QString(obsoleteLcodesA[i]), &VcCalc2SessionIfc::delA_obsolete);
  delIbyLCode_.clear();
  delJbyLCode_.clear();
  del4byLCode_.clear();
  delRbyLCode_.clear();
//  num = sizeof(obsoleteLcodesR)/sizeof(QString);
  num = sizeof(obsoleteLcodesR)/sizeof(const char*);
  for (int i=0; i<num; i++)
    delRbyLCode_.insert(QString(obsoleteLcodesR[i]), &VcCalc2SessionIfc::delR_obsolete);

  // gets:
  // char:
  getAbyLCode_.clear();
  getAbyLCode_.insert("SITNAMES", &VcCalc2SessionIfc::getA_sitNames);
  getAbyLCode_.insert("STRNAMES", &VcCalc2SessionIfc::getA_strNames);
  getAbyLCode_.insert("BASELINE", &VcCalc2SessionIfc::getA_baseline);
  getAbyLCode_.insert("STAR ID ", &VcCalc2SessionIfc::getA_starId);
  
  // short:
  getIbyLCode_.clear();
  getIbyLCode_.insert("# SITES ", &VcCalc2SessionIfc::getI_numSites);
  getIbyLCode_.insert("# STARS ", &VcCalc2SessionIfc::getI_numStars);
  getIbyLCode_.insert("INTRVAL4", &VcCalc2SessionIfc::getI_intrval4);
  getIbyLCode_.insert("UTC TAG ", &VcCalc2SessionIfc::getI_utc_tag2);
  getIbyLCode_.insert("UTC TAG4", &VcCalc2SessionIfc::getI_utc_tag4);
  
  // float/double?
  get4byLCode_.clear();
  get4byLCode_.insert("ROTEPOCH", &VcCalc2SessionIfc::get4_rotEpoch);
  get4byLCode_.insert("SEC TAG ", &VcCalc2SessionIfc::get4_sec_Tag);
  get4byLCode_.insert("REF FREQ", &VcCalc2SessionIfc::get4_ref_freq);

  // ----------------------------------------------------------------------------------
  // puts:
  putAbyLCode_.clear();
  // header:
  putAbyLCode_.insert("ATI CFLG", &VcCalc2SessionIfc::putA_ati_cFlg);
  putAbyLCode_.insert("ATI MESS", &VcCalc2SessionIfc::putA_ati_mess);
  putAbyLCode_.insert("ATM CFLG", &VcCalc2SessionIfc::putA_atm_cFlg);
  putAbyLCode_.insert("ATM MESS", &VcCalc2SessionIfc::putA_atm_mess);
  putAbyLCode_.insert("AXO CFLG", &VcCalc2SessionIfc::putA_axo_cFlg);
  putAbyLCode_.insert("AXO MESS", &VcCalc2SessionIfc::putA_axo_mess);
  putAbyLCode_.insert("CALCFLGN", &VcCalc2SessionIfc::putA_calcFlgN);
  putAbyLCode_.insert("CTI CFLG", &VcCalc2SessionIfc::putA_cti_cFlg);
  putAbyLCode_.insert("CTI MESS", &VcCalc2SessionIfc::putA_cti_mess);
  putAbyLCode_.insert("EOPSCALE", &VcCalc2SessionIfc::putA_obsolete);
  putAbyLCode_.insert("ETD CFLG", &VcCalc2SessionIfc::putA_etd_cFlg);
  putAbyLCode_.insert("ETD MESS", &VcCalc2SessionIfc::putA_etd_mess);
  putAbyLCode_.insert("FUT1TEXT", &VcCalc2SessionIfc::putA_fUt1Text);
  putAbyLCode_.insert("FWOBTEXT", &VcCalc2SessionIfc::putA_fWobText);
  putAbyLCode_.insert("NUT CFLG", &VcCalc2SessionIfc::putA_nut_cFlg);
  putAbyLCode_.insert("NUT MESS", &VcCalc2SessionIfc::putA_nut_mess);
  putAbyLCode_.insert("OCE CFLG", &VcCalc2SessionIfc::putA_oce_cFlg);
  putAbyLCode_.insert("OCE MESS", &VcCalc2SessionIfc::putA_oce_mess);
  putAbyLCode_.insert("OCE STAT", &VcCalc2SessionIfc::putA_oce_stat);
  putAbyLCode_.insert("PAN MESS", &VcCalc2SessionIfc::putA_pan_mess);
  putAbyLCode_.insert("PEP MESS", &VcCalc2SessionIfc::putA_pep_mess);
  putAbyLCode_.insert("PLX CFLG", &VcCalc2SessionIfc::putA_plx_cFlg);
  putAbyLCode_.insert("PLX MESS", &VcCalc2SessionIfc::putA_plx_mess);
  putAbyLCode_.insert("PTD CFLG", &VcCalc2SessionIfc::putA_ptd_cFlg);
  putAbyLCode_.insert("PTD MESS", &VcCalc2SessionIfc::putA_ptd_mess);
  putAbyLCode_.insert("REL CFLG", &VcCalc2SessionIfc::putA_rel_cFlg); // rel cflg
  putAbyLCode_.insert("SIT MESS", &VcCalc2SessionIfc::putA_sit_mess);
  putAbyLCode_.insert("STAR REF", &VcCalc2SessionIfc::putA_star_ref);
  putAbyLCode_.insert("STR MESS", &VcCalc2SessionIfc::putA_str_mess);
  putAbyLCode_.insert("STR CFLG", &VcCalc2SessionIfc::putA_str_cFlg);
  putAbyLCode_.insert("TECTPLNM", &VcCalc2SessionIfc::putA_tectPlNm);
  putAbyLCode_.insert("THE MESS", &VcCalc2SessionIfc::putA_the_mess); // rel mess
  putAbyLCode_.insert("UT1 CFLG", &VcCalc2SessionIfc::putA_ut1_cFlg);
  putAbyLCode_.insert("UT1 MESS", &VcCalc2SessionIfc::putA_ut1_mess);
  putAbyLCode_.insert("UT1INTRP", &VcCalc2SessionIfc::putA_ut1Intrp);
  putAbyLCode_.insert("WOB MESS", &VcCalc2SessionIfc::putA_wob_mess);
  putAbyLCode_.insert("WOB CFLG", &VcCalc2SessionIfc::putA_wob_cFlg);
  putAbyLCode_.insert("WOBINTRP", &VcCalc2SessionIfc::putA_wobIntrp);
  //
  //
  putIbyLCode_.clear();
  putIbyLCode_.insert("AXISTYPS", &VcCalc2SessionIfc::putI_axisTyps);
  putIbyLCode_.insert("CALCFLGV", &VcCalc2SessionIfc::putI_calcFlgV);
  putIbyLCode_.insert("TIDALUT1", &VcCalc2SessionIfc::putI_tidalUt1);
  //
  //
  putRbyLCode_.clear();
  putRbyLCode_.insert("AXISOFFS", &VcCalc2SessionIfc::putR_axisOffs);
  putRbyLCode_.insert("AXISTILT", &VcCalc2SessionIfc::putR_axisTilt);
  putRbyLCode_.insert("OPTLCOEF", &VcCalc2SessionIfc::putR_optlCoef);
  putRbyLCode_.insert("SITERECS", &VcCalc2SessionIfc::putR_siteRecs);
  putRbyLCode_.insert("SITEZENS", &VcCalc2SessionIfc::putR_siteZens);
  putRbyLCode_.insert("SITHOCPH", &VcCalc2SessionIfc::putR_sitHocPh);
  putRbyLCode_.insert("SITHOCAM", &VcCalc2SessionIfc::putR_sitHocAm);
  putRbyLCode_.insert("SITOCAMP", &VcCalc2SessionIfc::putR_sitOcAmp);
  putRbyLCode_.insert("SITOCPHS", &VcCalc2SessionIfc::putR_sitOcPhs);
  putRbyLCode_.insert("STAR2000", &VcCalc2SessionIfc::putR_star2000);
  //
  //
  put4byLCode_.clear();
  put4byLCode_.insert("A1 - TAI", &VcCalc2SessionIfc::put4_obsolete);
  put4byLCode_.insert("A1 - UTC", &VcCalc2SessionIfc::put4_obsolete);
  put4byLCode_.insert("AXO CONT", &VcCalc2SessionIfc::put4_axo_cont);
  put4byLCode_.insert("AXO PART", &VcCalc2SessionIfc::put4_axo_part);
  put4byLCode_.insert("AZ-THEO ", &VcCalc2SessionIfc::put4_az_theo );
  put4byLCode_.insert("BENDPART", &VcCalc2SessionIfc::put4_bendPart);
  put4byLCode_.insert("CALC VER", &VcCalc2SessionIfc::put4_calc_ver);
  put4byLCode_.insert("CF2J2K  ", &VcCalc2SessionIfc::put4_cf2j2k);
  put4byLCode_.insert("CON CONT", &VcCalc2SessionIfc::put4_con_cont);
  put4byLCode_.insert("CONSNDEL", &VcCalc2SessionIfc::put4_consnDel);
  put4byLCode_.insert("CONSNRAT", &VcCalc2SessionIfc::put4_consnRat);
  put4byLCode_.insert("CONSPART", &VcCalc2SessionIfc::put4_consPart);
  put4byLCode_.insert("CT SITE1", &VcCalc2SessionIfc::put4_ct_site1);
  put4byLCode_.insert("EARTH CE", &VcCalc2SessionIfc::put4_earth_ce);
  put4byLCode_.insert("EL-THEO ", &VcCalc2SessionIfc::put4_el_theo );
  put4byLCode_.insert("ETD CONT", &VcCalc2SessionIfc::put4_etd_cont);
  put4byLCode_.insert("FEED.COR", &VcCalc2SessionIfc::put4_feed_cor);
  put4byLCode_.insert("FUT1 INF", &VcCalc2SessionIfc::put4_fUt1_inf);
  put4byLCode_.insert("FUT1 PTS", &VcCalc2SessionIfc::put4_fUt1_pts);
  put4byLCode_.insert("FWOB INF", &VcCalc2SessionIfc::put4_fWob_inf);
  put4byLCode_.insert("FWOBX&YT", &VcCalc2SessionIfc::put4_fWobXnYt);
  put4byLCode_.insert("MOONDATA", &VcCalc2SessionIfc::put4_moonData);
  put4byLCode_.insert("NDRYCONT", &VcCalc2SessionIfc::put4_nDryCont);
  put4byLCode_.insert("NDRYPART", &VcCalc2SessionIfc::put4_nDryPart);
  put4byLCode_.insert("NGRADPAR", &VcCalc2SessionIfc::put4_nGradPar);
  put4byLCode_.insert("NUT WAHR", &VcCalc2SessionIfc::put4_nut_wahr);
  put4byLCode_.insert("NUT06XYP", &VcCalc2SessionIfc::put4_nut06XyP);
  put4byLCode_.insert("NUT06XYS", &VcCalc2SessionIfc::put4_nut06Xys);
  put4byLCode_.insert("NUT2006A", &VcCalc2SessionIfc::put4_nut2006A);
  put4byLCode_.insert("NWETCONT", &VcCalc2SessionIfc::put4_nWetCont);
  put4byLCode_.insert("NWETPART", &VcCalc2SessionIfc::put4_nWetPart);
  put4byLCode_.insert("OCE CONT", &VcCalc2SessionIfc::put4_oce_cont);
  put4byLCode_.insert("OCE DELD", &VcCalc2SessionIfc::put4_oce_deld);
  put4byLCode_.insert("OCE HORZ", &VcCalc2SessionIfc::put4_oce_horz);
  put4byLCode_.insert("OCE VERT", &VcCalc2SessionIfc::put4_oce_vert);
  put4byLCode_.insert("OCE_OLD ", &VcCalc2SessionIfc::put4_oce_old );
  put4byLCode_.insert("OPTLCONT", &VcCalc2SessionIfc::put4_optlCont);
  put4byLCode_.insert("PARANGLE", &VcCalc2SessionIfc::put4_parAngle);
  put4byLCode_.insert("PLX PART", &VcCalc2SessionIfc::put4_plx_part);
  put4byLCode_.insert("PLX1PSEC", &VcCalc2SessionIfc::put4_plx1psec);
  put4byLCode_.insert("POLAR XY", &VcCalc2SessionIfc::put4_polar_xy);
  put4byLCode_.insert("PTD CONT", &VcCalc2SessionIfc::put4_ptd_cont);
  put4byLCode_.insert("PTDXYPAR", &VcCalc2SessionIfc::put4_pTdXyPar);
  put4byLCode_.insert("PTOLDCON", &VcCalc2SessionIfc::put4_ptOldCon);
  put4byLCode_.insert("ROTEPOCH", &VcCalc2SessionIfc::put4_obsolete);
  put4byLCode_.insert("SIT PART", &VcCalc2SessionIfc::put4_sit_part);
  put4byLCode_.insert("STR PART", &VcCalc2SessionIfc::put4_str_part);
  put4byLCode_.insert("SUN CONT", &VcCalc2SessionIfc::put4_sun_cont);
  put4byLCode_.insert("SUN DATA", &VcCalc2SessionIfc::put4_sun_data);
  put4byLCode_.insert("SUN2CONT", &VcCalc2SessionIfc::put4_sun2cont);
  put4byLCode_.insert("TAI- UTC", &VcCalc2SessionIfc::put4_tai__utc);
  put4byLCode_.insert("TILTRMVR", &VcCalc2SessionIfc::put4_tiltRmvr);
  put4byLCode_.insert("UT1 -TAI", &VcCalc2SessionIfc::put4_ut1_tai);
  put4byLCode_.insert("UT1 PART", &VcCalc2SessionIfc::put4_ut1_part);
  put4byLCode_.insert("UT1EPOCH", &VcCalc2SessionIfc::put4_ut1Epoch);
  put4byLCode_.insert("UT1LIBRA", &VcCalc2SessionIfc::put4_ut1Libra);
  put4byLCode_.insert("UT1ORTHO", &VcCalc2SessionIfc::put4_ut1Ortho);
  put4byLCode_.insert("UVF/ASEC", &VcCalc2SessionIfc::put4_uvf_asec);
  put4byLCode_.insert("WOB PART", &VcCalc2SessionIfc::put4_wob_part);
  put4byLCode_.insert("WOBEPOCH", &VcCalc2SessionIfc::put4_wobEpoch);
  put4byLCode_.insert("WOBLIBRA", &VcCalc2SessionIfc::put4_wobLibra);
  put4byLCode_.insert("WOBORTHO", &VcCalc2SessionIfc::put4_wobOrtho);
  put4byLCode_.insert("WOBXCONT", &VcCalc2SessionIfc::put4_wobXcont);
  put4byLCode_.insert("WOBYCONT", &VcCalc2SessionIfc::put4_wobYcont);

  bandDepPutsByLCode_.clear();
  bandDepPutsByLCode_.insert("UVF/ASEC", 0);
  bandDepPutsByLCode_.insert("FEED.COR", 0);
};



//
VcCalc2SessionIfc::~VcCalc2SessionIfc()
{
  if (calcCallingMode_ == CCM_Legacy)
  {
    if (session_)
    {
      delete session_;
      session_ = NULL;
    };
    if (vgosDb_)
    {
      delete vgosDb_;
      vgosDb_ = NULL;
    };
  };
  getAbyLCode_.clear();
  getIbyLCode_.clear();
  get4byLCode_.clear();
  putAbyLCode_.clear();
  putIbyLCode_.clear();
  putRbyLCode_.clear();
  put4byLCode_.clear();
  bandDepPutsByLCode_.clear();
  obs_ = NULL;
  o_ = NULL;
  aux1_ = aux2_ = NULL;
};



//
void VcCalc2SessionIfc::setBandList(const QList<QString>& bands)
{
  bandList_.clear();
  for (int i=0; i<bands.size(); i++)
    bandList_.append(bands.at(i));
};



//
bool VcCalc2SessionIfc::readSession()
{
  bool                          isOk(true);
  bool                          isGuiEnabled;
  QStringList		                lst;
  //
  if (setup.getUseAltMasterfileSuffixes())
    lst = setup.getMasterfileSuffixes();
 
  //
  isGuiEnabled = getenv("DISPLAY")!=NULL && strlen(getenv("DISPLAY"))>0;  
  if (calcCallingMode_ == CCM_Modern)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::readSession(): unexpected calling mode");
    return false;
  };
  if (wrapperFileName_.size() == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::readSession(): the wrapper file name is empty");
    return false;
  };
  if (path2data_.size())
  {
    QDir                        dir(path2data_);
    if (!dir.exists())
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
        "::readSession(): the provided directory, " + path2data_ + ", does not exists");
      return false;
    };
  };
  QFileInfo                      fi(path2data_ + "/" + wrapperFileName_);
  if (!fi.exists())
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::readSession(): the file, " + path2data_ + "/" + wrapperFileName_ + ", does not exists");
    return false;
  };
  session_ = new SgVlbiSession;
  session_->setPath2Masterfile(setup.path2(setup.getPath2MasterFiles()));
  
  vgosDb_ = new SgVgosDb(&setup.identities(), &vgosDbCalcVersion);
  logger->write(SgLogger::INF, SgLogger::SESSION, className() +
    "::readSession(): the vgosDb object has been created");
  
  isOk = vgosDb_->init(path2data_, wrapperFileName_);
  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() +
    "::readSession(): parsing of wrapper file " + path2data_ + "/" + wrapperFileName_ + " has been" +
    (isOk?" complete":" failed"));

  isOk = session_->getDataFromVgosDb(vgosDb_, true, isGuiEnabled, lst);
  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() +
    "::readSession(): import data from " + path2data_ + "/" + wrapperFileName_ + " has been" +
    (isOk?" complete":" failed"));

  if (isOk)
  {
    numOfObservations_ = session_->observations().size();
    numOfStations_ = session_->stationsByName().size();
    numOfSources_ = session_->sourcesByName().size();
  };
  return true;
};



//
bool VcCalc2SessionIfc::writeSession()
{
  if (calcCallingMode_ == CCM_Modern)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      "::writeSession(): unexpected calling mode");
    return false;
  };
  //
  bool                           isOk;
  session_->addAttr(SgVlbiSessionInfo::Attr_FF_CALC_DATA_MODIFIED);
  isOk = session_->putDataIntoVgosDb(vgosDb_);

  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() +
    "::writeSession(): import data from " + path2data_ + "/" + wrapperFileName_ + " has been" +
    (isOk?" complete":" failed"));

  return isOk;
};



//
void VcCalc2SessionIfc::mimic_Kai(short openMode, const QString& inputDbNm, short inputDbVersion, 
    const QString& outputDbName, short &outputDbVersion, short &errorFlag)
{
  QString                        inputDbName(inputDbNm.simplified());
  
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::mimic_Kai(): got a call for \"" + inputDbName + "\" of version " +
    QString("").setNum(inputDbVersion) + ", open mode=" + QString("").setNum(openMode));
  // check the mode:
  if (openMode!=1 && openMode!=2)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_Kai(): wrong open mode: " + QString("").setNum(openMode));
    errorFlag = 4;
    return;
  };
  if (outputDbName.left(4) != "SAME") // unable to save with an alternate name (at least, now)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_Kai(): unexpected name of the output database: \"" + outputDbName + "\"");
    errorFlag = 9;
    return;
  };
  if (inputDbVersion < 1) // fix it later too
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_Kai(): unexpected database version number: " + QString("").setNum(inputDbVersion));
    errorFlag = 7;
    return;
  };
  if (inputDbName == inputDbName_ && session_ && vgosDb_) // already read and should be ok
  {
    if (calcCallingMode_ == CCM_Legacy)
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::mimic_Kai(): the session " + inputDbName + " already read");
    errorFlag = 0;
    openMode_ = openMode==1?OM_ReadOnly:OM_ReadWrite;
    obsIdx_ = -1;
    if (calcCallingMode_ == CCM_Modern)
    {
      numOfObservations_ = session_->observations().size();
      numOfStations_ = session_->stationsByName().size();
      numOfSources_ = session_->sourcesByName().size();
    };
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::mimic_Kai(): the session " + inputDbName + " is ready for " + 
      (openMode_==OM_ReadOnly?"reading":"writting") + ", " +
      QString("").sprintf("Num(Obs/Stn/Src)=%d/%d/%d", 
        numOfObservations_, numOfStations_, numOfSources_));
    return;
  };
  //
  //
  QRegularExpression            reOldDbName(".*(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName(".*(\\d{8})-([\\S]{2,12}).*");
  QRegularExpressionMatch       match;

  int                           yr;
  bool                          isOk;
  if ((match=reOldDbName.match(inputDbName)).hasMatch())         // dbName: 12JUL16XA
  {
    yr = match.captured(1).left(2).toInt(&isOk);
  }
  else if ((match=reNewDbName.match(inputDbName)).hasMatch())    // dbName: 20120716-r1542
  {
    yr = match.captured(1).left(4).toInt(&isOk);
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::SESSION, className() +
      "::mimic_Kai(): cannot guess a database name from the string \"" + inputDbName + "\"");
    return;
  };
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_Kai(): cannot infer the year of the database: \"" + inputDbName + "\"");
    errorFlag = 8;
    inputDbName_ = "";
    openMode_ = OM_None;
    return;
  };
  if (100 < yr)
    yr += (78<yr)?1900:2000;
  
  path2data_ = setup.getPath2VgosDbFiles() + QString("").sprintf("/%04d/", yr) + inputDbName;
  wrapperFileName_.sprintf("%s_V%03d_kall.wrp",  qPrintable(inputDbName), inputDbVersion);
  
  if (readSession())
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::mimic_Kai(): the session " + inputDbName + " has been successfully read from " + path2data_);
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_Kai(): read of session failed");
    errorFlag = 9;
    inputDbName_ = "";
    openMode_ = OM_None;
    return;
  };
  //
  openMode_ = openMode==1?OM_ReadOnly:OM_ReadWrite;
  inputDbName_ = inputDbName;
  outputDbVersion = inputDbVersion + 1;
  errorFlag = 0;
  obsIdx_ = -1;
};



//
short VcCalc2SessionIfc::mimic_MvRec(short ntoc, short kmode, short knum)
{
  short                          rc=0;
  if (!session_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_MvRec(): the session is NULL");
    return 5;
  };
  if (!vgosDb_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_MvRec(): the vgosDb data are not loaded yet");
    return 5;
  };
  if (ntoc<0 || 2<ntoc)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_MvRec(): unexpected nToC number: " + QString("").setNum(ntoc));
    return 4;
  };
  if (kmode<0 || 1<kmode)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_MvRec(): unexpected kMode number: " + QString("").setNum(kmode));
    return 4;
  };
  if (knum<0 || 1<knum)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::mimic_MvRec(): unexpected kNum number: " + QString("").setNum(knum));
    return 4;
  };
  //
  //
  if (ntoc == 1)
  {
    obsIdx_ = -1;
    rc = 0;
  }
  else
  {
    if (kmode == 1)
      obsIdx_++;
    else
      obsIdx_ = knum;
    if (numOfObservations_ <= obsIdx_)
//    if (2 <= obsIdx_) // shortcut:
      rc = 1;
  };
  obs_ = NULL;
  o_ = NULL;
  aux1_ = aux2_ = NULL;
  if (rc==0 && 0<=obsIdx_ && obsIdx_<numOfObservations_)
  {
    obs_  = session_->observations().at(obsIdx_);
    o_    = obs_->observable(currentBandKey_);
    aux1_ = obs_->auxObs_1();
    aux2_ = obs_->auxObs_2();
  };
  //
  /*
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::mimic_MvRec(): set the observation index to " + QString("").setNum(obsIdx_));
  */
  return rc;
};



//
void VcCalc2SessionIfc::mimic_Finis()
{
  if (openMode_ == OM_ReadOnly)
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::mimic_Finis(): the openMode is ReadOnly: nothing to do");
  }
  else if (openMode_ == OM_ReadWrite)
  {
    if (calcCallingMode_ == CCM_Legacy)
    {
      if (writeSession())
        logger->write(SgLogger::INF, SgLogger::IO, className() +
          "::mimic_Finis(): the session has been updated");
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::mimic_Finis(): the procedure has failed");
    };
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::mimic_Finis(): the openMode is ReadWrite: set it to None");
    openMode_ = OM_None;
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimic_Finis(): inappropriate call of the function: openMode is not R/O nor R/W");
};



//
void VcCalc2SessionIfc::rewind()
{
  if (calcCallingMode_ == CCM_Modern)
  {
    if (openMode_ == OM_None)
    {
      openMode_ = OM_ReadWrite;
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::rewind(): the mode has been set to R/W");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::mimic_Finis(): inappropriate call of the function: openMode is not None");
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::rewind(): unexpected call of the function");
};



//
void VcCalc2SessionIfc::mimic_pHist(const QString& histRecord)
{
  session_->contemporaryHistory().addHistoryRecord(histRecord, SgMJD::currentMJD().toUtc());
};




//
//
// dispatchers: adds: 
//
void VcCalc2SessionIfc::mimicAddA(short nToc, const QString& lCode, const QString& descr,
  short dim1, short dim2, short dim3)
{
  if (!putAbyLCode_.contains(lCode))
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicAddA(): unknown LCode: \"" + lCode + "\" [" + descr + "] (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicAddI(short nToc, const QString& lCode, const QString& descr,
  short dim1, short dim2, short dim3)
{
  if (!putIbyLCode_.contains(lCode))
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicAddI(): unknown LCode: \"" + lCode + "\" [" + descr + "] (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicAddJ(short nToc, const QString& lCode, const QString& descr,
  short dim1, short dim2, short dim3)
{
  //  if (!putJbyLCode_.contains(lCode))
  if (true)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicAddJ(): unknown LCode: \"" + lCode + "\" [" + descr + "] (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicAdd4(short nToc, const QString& lCode, const QString& descr,
  short dim1, short dim2, short dim3)
{
  if (!put4byLCode_.contains(lCode) && !putRbyLCode_.contains(lCode))
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicAdd4(): unknown LCode: \"" + lCode + "\" [" + descr + "] (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicAddR(short nToc, const QString& lCode, const QString& descr,
  short dim1, short dim2, short dim3)
{
  if (!putRbyLCode_.contains(lCode) && !put4byLCode_.contains(lCode))
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicAddR(): unknown LCode: \"" + lCode + "\" [" + descr + "] (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") of the nToc #" + QString("").setNum(nToc));
  };
};



//
//
// dispatchers: dels: 
//
void VcCalc2SessionIfc::mimicDelA(short nToc, const QString& lCode)
{
  if (delAbyLCode_.contains(lCode))
    return (this->*delAbyLCode_.value(lCode))(lCode, nToc);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicDelA(): unknown LCode: \"" + lCode + "\" of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicDelI(short nToc, const QString& lCode)
{
  if (delIbyLCode_.contains(lCode))
    return (this->*delIbyLCode_.value(lCode))(lCode, nToc);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicDelI(): unknown LCode: \"" + lCode + "\" of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicDelJ(short nToc, const QString& lCode)
{
  if (delJbyLCode_.contains(lCode))
    return (this->*delJbyLCode_.value(lCode))(lCode, nToc);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicDelJ(): unknown LCode: \"" + lCode + "\" of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicDel4(short nToc, const QString& lCode)
{
  if (del4byLCode_.contains(lCode))
    return (this->*del4byLCode_.value(lCode))(lCode, nToc);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicDel4(): unknown LCode: \"" + lCode + "\" of the nToc #" + QString("").setNum(nToc));
  };
};



//
void VcCalc2SessionIfc::mimicDelR(short nToc, const QString& lCode)
{
  if (delRbyLCode_.contains(lCode))
    return (this->*delRbyLCode_.value(lCode))(lCode, nToc);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicDelR(): unknown LCode: \"" + lCode + "\" of the nToc #" + QString("").setNum(nToc));
  };
};



//
// dispatchers: gets: 
//
short VcCalc2SessionIfc::mimicGetA(const QString& lCode, char values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  for (int i=0; i<dim1; i++)
    for (int j=0; j<dim2; j++)
      for (int k=0; k<dim3; k++)
        *(values + i + dim1*j + dim1*dim2*k) = ' ';
  if (getAbyLCode_.contains(lCode))
    return (this->*getAbyLCode_.value(lCode))(values, dim1, dim2, dim3, actualDimension);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicGetA(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    actualDimension[0] = dim1;
    actualDimension[1] = dim2;
    actualDimension[2] = dim3;
    return 1; // data not found
  };
};



//
short VcCalc2SessionIfc::mimicGetI(const QString& lCode, short values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  for (int i=0; i<dim1; i++)
    for (int j=0; j<dim2; j++)
      for (int k=0; k<dim3; k++)
        *(values + i + dim1*j + dim1*dim2*k) = 0;
  if (getIbyLCode_.contains(lCode))
    return (this->*getIbyLCode_.value(lCode))(values, dim1, dim2, dim3, actualDimension);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicGetI(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    actualDimension[0] = dim1;
    actualDimension[1] = dim2;
    actualDimension[2] = dim3;
    return 1; // data not found
  };
};



//
short VcCalc2SessionIfc::mimicGet4(const QString& lCode, double values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  for (int i=0; i<dim1; i++)
    for (int j=0; j<dim2; j++)
      for (int k=0; k<dim3; k++)
        *(values + i + dim1*j + dim1*dim2*k) = 0.0;
  if (get4byLCode_.contains(lCode))
    return (this->*get4byLCode_.value(lCode))(values, dim1, dim2, dim3, actualDimension);
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicGet4(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    actualDimension[0] = dim1;
    actualDimension[1] = dim2;
    actualDimension[2] = dim3;
    return 1; // data not found
  };
};



//
void VcCalc2SessionIfc::mimicPutA(const QString& lCode, const char values[], 
  short dim1, short dim2, short dim3)
{
//  std::cout << " =======> puta(): lcode=[" << qPrintable(lCode) 
//    << "], dims=(" << dim1 << "," << dim2  << "," << dim3 << ")" << "\n";
  if (putAbyLCode_.contains(lCode))
  {
    if (isPrimeBand_ || bandDepPutsByLCode_.contains(lCode))
      (this->*putAbyLCode_.value(lCode))(values, dim1, dim2, dim3);
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicPutA(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    return; // data not found
  };
};



//
void VcCalc2SessionIfc::mimicPutI(const QString& lCode, const short values[], 
  short dim1, short dim2, short dim3)
{
//  std::cout << " =======> puti(): lcode=[" << qPrintable(lCode) 
//    << "], dims=(" << dim1 << "," << dim2  << "," << dim3 << ")" << "\n";
  if (putIbyLCode_.contains(lCode))
  {
    if (isPrimeBand_ || bandDepPutsByLCode_.contains(lCode))
      (this->*putIbyLCode_.value(lCode))(values, dim1, dim2, dim3);
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicPutI(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    return; // data not found
  };
};



//
void VcCalc2SessionIfc::mimicPutR(const QString& lCode, const double values[], 
  short dim1, short dim2, short dim3)
{
  if (putRbyLCode_.contains(lCode))
  {
    if (isPrimeBand_ || bandDepPutsByLCode_.contains(lCode))
      (this->*putRbyLCode_.value(lCode))(values, dim1, dim2, dim3);
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicPutR(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    return; // data not found
  };
};



//
void VcCalc2SessionIfc::mimicPut4(const QString& lCode, const double values[],
  short dim1, short dim2, short dim3)
{
  if (put4byLCode_.contains(lCode))
  {
    if (isPrimeBand_ || bandDepPutsByLCode_.contains(lCode))
      (this->*put4byLCode_.value(lCode))(values, dim1, dim2, dim3);
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::mimicPut4(): unknown LCode: \"" + lCode + "\" (" + QString("").setNum(dim1) +
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      "), the operation has been skipped");
    return; // data not found
  };
};





//                     Implementation:
//

//
// known dels:
//
// DelAs:
//
void VcCalc2SessionIfc::delA_obsolete(UNUSED const QString& lCode, UNUSED short nToc)
{
/*
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::delA_obsolete(): the known obsolete LCode \"" + lCode + "\" of the nToc# " + 
    QString("").setNum(nToc) + " has been removed");
*/
};



//
void VcCalc2SessionIfc::delR_obsolete(UNUSED const QString& lCode, UNUSED short nToc)
{
/*
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::delR_obsolete(): the known obsolete LCode \"" + lCode + "\" of the nToc# " + 
    QString("").setNum(nToc) + " has been removed");
*/
};
//







// Gets:
// known getAs:
//
short VcCalc2SessionIfc::getA_sitNames(char values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  char                           b;
  int                           strLen;
  int                           valIdx=0;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    const QString&              key=it.value()->getKey();
    strLen = key.size();
    for (int i=0; i<8; i++)
    {
      b = i<strLen?key.at(i).toLatin1():0;
      *(values + valIdx++) = b==0?' ':b;
    };
  };
  actualDimension[0] = 4;
  actualDimension[1] = session_->stationsByName().size();
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getA_strNames(char values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  char                           b;
  int                           strLen;
  int                           valIdx=0;
  for (SourcesByName_it it=session_->sourcesByName().begin(); it!=session_->sourcesByName().end(); 
    ++it)
  {
    const QString&              key=it.value()->getKey();
    strLen = key.size();
    for (int i=0; i<8; i++)
    {
      b = i<strLen?key.at(i).toLatin1():0;
      *(values + valIdx++) = b==0?' ':b;
    };
  };
  actualDimension[0] = 4;
  actualDimension[1] = session_->sourcesByName().size();
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getA_baseline(char values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getA_baseline(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  char                           b;
  int                           strLen;
  int                           valIdx=0;
  const QString&                stn1=session_->observations().at(obsIdx_)->stn_1()->getKey();
  strLen = stn1.size();
  for (int i=0; i<8; i++)
  {
    b = i<strLen?stn1.at(i).toLatin1():0;
    *(values + valIdx++) = b==0?' ':b;
  };
  const QString&                stn2=session_->observations().at(obsIdx_)->stn_2()->getKey();
  strLen = stn2.size();
  for (int i=0; i<8; i++)
  {
    b = i<strLen?stn2.at(i).toLatin1():0;
    *(values + valIdx++) = b==0?' ':b;
  };

  actualDimension[0] = 4;
  actualDimension[1] = 2;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getA_starId(char values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getA_starId(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  char                           b;
  int                           strLen;
  int                           valIdx=0;
  const QString&                src=session_->observations().at(obsIdx_)->src()->getKey();
  strLen = src.size();
  for (int i=0; i<8; i++)
  {
    b = i<strLen?src.at(i).toLatin1():0;
    *(values + valIdx++) = b==0?' ':b;
  };
  actualDimension[0] = 4;
  actualDimension[1] = 1;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};






//                                         known getIs:
//
short VcCalc2SessionIfc::getI_numSites(short values[], short dim1, short dim2, short dim3, 
  short actualDimension[])
{
  values[0] = session_->stationsByName().size();
  actualDimension[0] = actualDimension[1] = actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getI_numStars(short values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  values[0] = session_->sourcesByName().size();
  actualDimension[0] = actualDimension[1] = actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getI_intrval4(short values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  SgMJD                          tFirst(*session_->observations().first());
  SgMJD                          tLast (*session_->observations().last ());
  int                           nE[5];
  double                         d;
  tFirst.toYMDHMS_tr(nE[0], nE[1], nE[2], nE[3], nE[4], d);
  for (int i=0; i<5; i++)
    *(values + i) = nE[i];
  tLast.toYMDHMS_tr(nE[0], nE[1], nE[2], nE[3], nE[4], d);
  for (int i=0; i<5; i++)
    *(values + 5 + i) = nE[i];
  actualDimension[0] = 5;
  actualDimension[1] = 2;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getI_utc_tag2(short values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getI_utc_tag2(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  SgMJD                          t(*session_->observations().at(obsIdx_));
  int                           nE[5];
  double                         d;
  t.toYMDHMS_tr(nE[0], nE[1], nE[2], nE[3], nE[4], d);
  nE[0] %= 100;
  for (int i=0; i<5; i++)
    *(values + i) = nE[i];
  if (dim1==6)
  {
    *(values + 5) = round(d);
    actualDimension[0] = 6;
  }
  else
    actualDimension[0] = 5;
  actualDimension[1] = 1;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::getI_utc_tag4(short values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::getI_utc_tag4(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  SgMJD                          t(*session_->observations().at(obsIdx_));
  int                           nE[5];
  double                         d;
  t.toYMDHMS_tr(nE[0], nE[1], nE[2], nE[3], nE[4], d);
  for (int i=0; i<5; i++)
    *(values + i) = nE[i];
  if (dim1==6)
  {
    *(values + 5) = round(d);
    actualDimension[0] = 6;
  }
  else
    actualDimension[0] = 5;
  actualDimension[1] = 1;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//                                         known get4s:
//
short VcCalc2SessionIfc::get4_rotEpoch(double values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  values[0    ] = 0.0;
  values[0 + 1] = 0.0;
  values[0 + 2] = 0.0;
  values[0 + 3] = 0.0;
  actualDimension[0] = 2;
  actualDimension[1] = 2; 
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
//    return 2;
    return 0;
};



//
short VcCalc2SessionIfc::get4_sec_Tag(double values[],
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::get4_sec_Tag(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  SgMJD                          t(*session_->observations().at(obsIdx_));
  int                           nE[5];
  double                         d;
  t.toYMDHMS_tr(nE[0], nE[1], nE[2], nE[3], nE[4], d);
  *values = d;
  actualDimension[0] = 1;
  actualDimension[1] = 1;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};



//
short VcCalc2SessionIfc::get4_ref_freq(double values[], 
  short dim1, short dim2, short dim3, short actualDimension[])
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::get4_ref_freq(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return 4;
  };
  values[0] = o_?o_->getReferenceFrequency():1.0;
  actualDimension[0] = 1;
  actualDimension[1] = 1;
  actualDimension[2] = 1;
  if (dim1==actualDimension[0] && dim2==actualDimension[1] && dim3==actualDimension[2])
    return 0;
  else
    return 2;
};




//
//                                         known putAs:
//
void VcCalc2SessionIfc::putA_calcFlgN(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_calcFlgN(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  QList<QString>&                flagNames=session_->calcInfo().controlFlagNames();
  flagNames.clear();
  for (int i=0; i<dim2; i++)
    flagNames.append(fortArray2QString(values + 4*i, 4));
};



//
void VcCalc2SessionIfc::putA_atm_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_atm_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().troposphere().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_atm_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_atm_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().troposphere().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_axo_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_axo_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().axisOffset().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_axo_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_axo_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().axisOffset().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_etd_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_etd_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().earthTide().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_etd_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_etd_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().earthTide().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ptd_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ptd_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().poleTide().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ptd_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ptd_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().poleTide().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_nut_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_nut_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().nutation().setDefinition(fortArray2QString(values, dim1*2).simplified());
};




void VcCalc2SessionIfc::putA_nut_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_nut_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().nutation().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_oce_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_oce_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().oceanLoading().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_oce_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_oce_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().oceanLoading().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ati_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ati_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().atomicTime().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ati_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ati_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().atomicTime().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_cti_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_cti_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().coordinateTime().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_cti_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_cti_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().coordinateTime().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_plx_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_plx_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().parallax().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_plx_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_plx_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().parallax().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_str_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_str_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().star().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_str_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_str_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().star().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};
  


//
void VcCalc2SessionIfc::putA_the_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_the_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().relativity().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_rel_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_rel_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().relativity().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_pan_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_pan_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().feedHorn().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_sit_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_sit_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().site().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_pep_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_pep_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().ephemeris().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_wob_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_wob_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().polarMotion().setDefinition(fortArray2QString(values, dim1*2).simplified());
  session_->calcInfo().polarMotionInterpolation().
                                     setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_wob_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_wob_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().polarMotion().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ut1_mess(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ut1_mess(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().ut1().setDefinition(fortArray2QString(values, dim1*2).simplified());
  session_->calcInfo().ut1Interpolation().setDefinition(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_ut1_cFlg(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ut1_cFlg(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().ut1().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_oce_stat(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==numOfStations_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_oce_stat(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  QList<QString>&                statuses=session_->calcInfo().oLoadStationStatus();
  statuses.clear();
  for (int i=0; i<numOfStations_; i++)
    statuses.append(fortArray2QString(values + dim1*2*i, dim1*2));
};



//
void VcCalc2SessionIfc::putA_ut1Intrp(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_ut1Intrp(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().ut1Interpolation().setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_fUt1Text(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_fUt1Text(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().ut1Interpolation().setOrigin(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_wobIntrp(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_wobIntrp(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().polarMotionInterpolation().
                                      setControlFlag(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_fWobText(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_fWobText(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().polarMotionInterpolation().
                                      setOrigin(fortArray2QString(values, dim1*2).simplified());
};



//
void VcCalc2SessionIfc::putA_star_ref(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==numOfSources_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_star_ref(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (SourcesByName_it it=session_->sourcesByName().begin(); it!=session_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo            *src=it.value();
    src->setAprioriReference(fortArray2QString(values + dim1*2*idx, dim1*2).simplified());
    idx++;
  };
};



//
void VcCalc2SessionIfc::putA_tectPlNm(const char values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==numOfStations_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putA_tectPlNm(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    SgVlbiStationInfo         *stn=it.value();
    stn->setTectonicPlateName(fortArray2QString(values + dim1*2*idx, dim1*2).simplified());
    idx++;
  };
};



//
void VcCalc2SessionIfc::putA_obsolete(UNUSED const char values[], 
  UNUSED short dim1, UNUSED short UNUSED dim2, UNUSED short dim3)
{
};









//
//                                         known putIs:
//


void VcCalc2SessionIfc::putI_calcFlgV(const short values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putI_calcFlgV(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  QList<int>&                    flagVals=session_->calcInfo().controlFlagValues();
  flagVals.clear();
  for (int i=0; i<dim1; i++)
    flagVals.append(values[i]);
};



//
void VcCalc2SessionIfc::putI_tidalUt1(const short values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putI_tidalUt1(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  //
  session_->calcInfo().setFlagTidalUt1(values[0]);
};



//
void VcCalc2SessionIfc::putI_axisTyps(const short values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==numOfStations_ && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putI_axisTyps(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  SgVlbiStationInfo::MountingType
                                mnt=SgVlbiStationInfo::MT_UNKN;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    switch (values[idx])
    {
    case 1:
      mnt = SgVlbiStationInfo::MT_EQUA;
      break;
    case 2:
      mnt = SgVlbiStationInfo::MT_X_YN;
      break;
    case 3:
      mnt = SgVlbiStationInfo::MT_AZEL;
      break;
    case 4:
      mnt = SgVlbiStationInfo::MT_RICHMOND;
      break;
    case 5:
      mnt = SgVlbiStationInfo::MT_X_YE;
      break;
    default:
      mnt = SgVlbiStationInfo::MT_UNKN;
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::putI_axisTyps(): got unknown value for the axis type (" + 
        QString("").setNum(values[idx]) + ") for the station #" + QString("").setNum(idx));
      break;
    };
    stn->setMntType(mnt);
    idx++;
  };
};





//
//                                         known putRs:
//


//
void VcCalc2SessionIfc::putR_siteZens(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==numOfStations_ && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_siteZens(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  session_->calcInfo().siteZenDelays().clear();
  for (int i=0; i<numOfStations_; i++)
    session_->calcInfo().siteZenDelays().append(values[i]);
};



//
void VcCalc2SessionIfc::putR_optlCoef(const double values[], short dim1, short dim2, short dim3)
{
  int                           n=session_->calcInfo().stations().size();
  if (!(dim1==6 && dim2==n && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_optlCoef(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  for (int i=0; i<n; i++)
  {
    SgVlbiStationInfo           *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      for (int j=0; j<dim1; j++)
        si->setOptLoadCoeff(j, values[6*i + j]);
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::putR_optlCoef(): the station # " + QString("").setNum(i) + " is NULL");
  };
};



//
void VcCalc2SessionIfc::putR_sitOcPhs(const double values[], short dim1, short dim2, short dim3)
{
  int                           n=session_->calcInfo().stations().size();
  if (!(dim1==11 && dim2==n && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_sitOcPhs(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  for (int i=0; i<n; i++)
  {
    SgVlbiStationInfo           *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      for (int j=0; j<dim1; j++)
        si->setOLoadPhase(j, 0,  values[dim1*i + j]);
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::putR_sitOcPhs(): the station # " + QString("").setNum(i) + " is NULL");
  };
};



//
void VcCalc2SessionIfc::putR_sitOcAmp(const double values[], short dim1, short dim2, short dim3)
{
  int                           n=session_->calcInfo().stations().size();
  if (!(dim1==11 && dim2==n && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_sitOcAmp(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  for (int i=0; i<n; i++)
  {
    SgVlbiStationInfo           *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      for (int j=0; j<dim1; j++)
        si->setOLoadAmplitude(j, 0,  values[dim1*i + j]);
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::putR_sitOcAmp(): the station # " + QString("").setNum(i) + " is NULL");
  };
};



//
void VcCalc2SessionIfc::putR_sitHocPh(const double values[], short dim1, short dim2, short dim3)
{
  int                           n=session_->calcInfo().stations().size();
  if (!(dim1==11 && dim2==2 && dim3==n))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_sitHocPh(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  for (int i=0; i<n; i++)
  {
    SgVlbiStationInfo           *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      for (int j=0; j<dim1; j++)
      {
        si->setOLoadPhase(j, 1,  values[2*dim1*i        + j]);
        si->setOLoadPhase(j, 2,  values[2*dim1*i + dim1 + j]);
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::putR_sitHocPh(): the station # " + QString("").setNum(i) + " is NULL");
  };
};



//
void VcCalc2SessionIfc::putR_sitHocAm(const double values[], short dim1, short dim2, short dim3)
{
  int                           n=session_->calcInfo().stations().size();
  if (!(dim1==11 && dim2==2 && dim3==n))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_sitHocAm(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  for (int i=0; i<n; i++)
  {
    SgVlbiStationInfo           *si=session_->calcInfo().stations().at(i);
    if (si)
    {
      for (int j=0; j<dim1; j++)
      {
        si->setOLoadAmplitude(j, 1,  values[2*dim1*i        + j]);
        si->setOLoadAmplitude(j, 2,  values[2*dim1*i + dim1 + j]);
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::putR_sitHocAm(): the station # " + QString("").setNum(i) + " is NULL");
  };
};



//
void VcCalc2SessionIfc::putR_star2000(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim2==numOfSources_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_star2000(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (SourcesByName_it it=session_->sourcesByName().begin(); it!=session_->sourcesByName().end(); ++it)
  {
    SgVlbiSourceInfo            *src=it.value();
    src->setRA(values[2*idx    ]);
    src->setDN(values[2*idx + 1]);
    idx++;
  };
};



//
void VcCalc2SessionIfc::putR_siteRecs(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==3 && dim2==numOfStations_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_siteRecs(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    stn->setR(Sg3dVector(values[3*idx], values[3*idx + 1], values[3*idx + 2]));
    idx++;
  };
};



//
void VcCalc2SessionIfc::putR_axisOffs(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==numOfStations_ && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_axisOffs(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    stn->setAxisOffset(values[idx]);
    idx++;
  };
};


//
void VcCalc2SessionIfc::putR_axisTilt(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim2==numOfStations_ && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::putR_axisTilt(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                           idx=0;
  for (StationsByName_it it=session_->stationsByName().begin(); it!=session_->stationsByName().end(); 
    ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    stn->setTilt(0, values[2*idx    ]);
    stn->setTilt(1, values[2*idx + 1]);
    idx++;
  };
};





//
//                                         known put4s:
//


//
void VcCalc2SessionIfc::put4_calc_ver(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==1 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_calc_ver(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  session_->calcInfo().setDversion(values[0]);
  session_->calcInfo().setProgramName("CALC");
  session_->calcInfo().setVersion(QString("").setNum(values[0]));
};



//
void VcCalc2SessionIfc::put4_ut1Epoch(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1Epoch(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  int                                     nn;
  if (!session_->calcInfo().ut1InterpData())
  {
    session_->calcInfo().ut1InterpData() = new SgMatrix(dim2, 2);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::put4_ut1Epoch(): the matrix has been alocated");
  }
  else if (dim2 != (nn=session_->calcInfo().ut1InterpData()->nRow()))
  {
    delete session_->calcInfo().ut1InterpData();
    session_->calcInfo().ut1InterpData() = new SgMatrix(dim2, 2);
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::put4_ut1Epoch(): adjusted the dimension of the matrix: " + QString("").setNum(nn) + 
      " => " + QString("").setNum(dim2));
  };
  for (int i=0; i<dim2; i++)
    for (int j=0; j<dim1; j++)
      session_->calcInfo().ut1InterpData()->setElement(i, j,  values[dim1*i + j]);
};



//
void VcCalc2SessionIfc::put4_wobEpoch(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobEpoch(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (dim2 == 0)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobEpoch(): the dimensions #2 is zero, adjusted to 1");
    dim2 = 1;
  };
  int                                     nn;
  if (!session_->calcInfo().wobInterpData())
  {
    session_->calcInfo().wobInterpData() = new SgMatrix(dim2, 2);
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::put4_wobEpoch(): the matrix has been alocated");
  }
  else if (dim2 != (nn=session_->calcInfo().wobInterpData()->nRow()))
  {
    delete session_->calcInfo().wobInterpData();
    session_->calcInfo().wobInterpData() = new SgMatrix(dim2, 2);
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::put4_wobEpoch(): adjusted the dimension of the matrix: " + QString("").setNum(nn) + 
      " => " + QString("").setNum(dim2));
  };
  for (int i=0; i<dim2; i++)
    for (int j=0; j<dim1; j++)
      session_->calcInfo().wobInterpData()->setElement(i, j,  values[dim1*i + j]);
};




//--
//
void VcCalc2SessionIfc::put4_fUt1_inf(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==4 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_fUt1_inf(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  double                        t0=values[0];
  double                        dt=values[1];
  unsigned int                  numOfPts=round(values[2]);
  if (t0 > 2390000.0)
    t0 -= 2400000.5;
  session_->allocUt1InterpolEpochs(t0, dt, numOfPts);
};



//
void VcCalc2SessionIfc::put4_fWob_inf(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==3 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_fWob_inf(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  double                        t0=values[0];
  double                        dt=values[1];
  unsigned int                  numOfPts=round(values[2]);
  if (t0 > 2390000.0)
    t0 -= 2400000.5;
  session_->allocPxyInterpolEpochs(t0, dt, numOfPts);
};



//
void VcCalc2SessionIfc::put4_fUt1_pts(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_fUt1_pts(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  SgVector                      vals(dim1);
  for (int i=0; i<dim1; i++)
    vals.setElement(i, values[i]);
  session_->allocUt1InterpolValues(&vals);
};



//
void VcCalc2SessionIfc::put4_fWobXnYt(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_fWobXnYt(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  SgMatrix                      vals(dim2, 2);
  for (int i=0; i<dim2; i++)
  {
    vals.setElement(i, 0,  values[2*i      ]);
    vals.setElement(i, 1,  values[2*i + 1 ]);
  };
  session_->allocPxyInterpolValues(&vals);
};



//
void VcCalc2SessionIfc::put4_tai__utc(const double values[], short dim1, short dim2, short dim3)
{
  if (!(dim1==3 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_tai__utc(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  double                         ls=values[1];
  if (0.5<ls && ls<1000)
  {
    session_->setLeapSeconds(ls);
    logger->write(SgLogger::DBG, SgLogger::IO_DBH, className() +
      "::put4_tai__utc(): " + QString().setNum(ls) + " leap seconds were assigned");
  };
};




//--



//
//
void VcCalc2SessionIfc::put4_axo_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_axo_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_axo_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setDdel_dAxsOfs  (-values[0]);
    aux1_->setDrat_dAxsOfs  (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setDdel_dAxsOfs  ( values[1]);
    aux2_->setDrat_dAxsOfs  ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_az_theo(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_az_theo(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_az_theo(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setAzimuthAngle      (values[0]);
    aux1_->setAzimuthAngleRate  (values[2]);
  };
  if (aux2_)
  {
    aux2_->setAzimuthAngle      (values[1]);
    aux2_->setAzimuthAngleRate  (values[3]);
  };
};


void VcCalc2SessionIfc::put4_el_theo(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_el_theo(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_el_theo(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setElevationAngle      (values[0]);
    aux1_->setElevationAngleRate  (values[2]);
  };
  if (aux2_)
  {
    aux2_->setElevationAngle      (values[1]);
    aux2_->setElevationAngleRate  (values[3]);
  };
};


//
void VcCalc2SessionIfc::put4_parAngle(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_parAngle(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_parAngle(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
    aux1_->setParallacticAngle(values[0]*DEG2RAD);
  if (aux2_)
    aux2_->setParallacticAngle(values[1]*DEG2RAD);
};



//
void VcCalc2SessionIfc::put4_axo_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_axo_cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_axo_cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setCalcAxisOffset4Delay(-values[0]);        // ?? sign???
    aux1_->setCalcAxisOffset4Rate (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setCalcAxisOffset4Delay( values[1]);
    aux2_->setCalcAxisOffset4Rate ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nDryCont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nDryCont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nDryCont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setCalcNdryCont4Delay(-values[0]);
    aux1_->setCalcNdryCont4Rate (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setCalcNdryCont4Delay( values[1]);
    aux2_->setCalcNdryCont4Rate ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nWetCont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nWetCont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nWetCont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setCalcNwetCont4Delay(-values[0]);
    aux1_->setCalcNwetCont4Rate (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setCalcNwetCont4Delay( values[1]);
    aux2_->setCalcNwetCont4Rate ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_oce_horz(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_horz(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_horz(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setCalcOLoadHorz4Delay(-values[0]);
    aux1_->setCalcOLoadHorz4Rate (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setCalcOLoadHorz4Delay( values[1]);
    aux2_->setCalcOLoadHorz4Rate ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_oce_vert(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_vert(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_vert(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setCalcOLoadVert4Delay(-values[0]);
    aux1_->setCalcOLoadVert4Rate (-values[2]);
  };
  if (aux2_)
  {
    aux2_->setCalcOLoadVert4Delay( values[1]);
    aux2_->setCalcOLoadVert4Rate ( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_oce_deld(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_deld(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==2 && dim3==2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_deld(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setOcnLdR( Sg3dVector(values[ 0], values[ 1], values[ 2]) );
    aux1_->setOcnLdV( Sg3dVector(values[ 3], values[ 4], values[ 5]) );
  };
  if (aux2_)
  {
    aux2_->setOcnLdR( Sg3dVector(values[ 6], values[ 7], values[ 8]) );
    aux2_->setOcnLdV( Sg3dVector(values[ 9], values[10], values[11]) );
  };
};



//
void VcCalc2SessionIfc::put4_nDryPart(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nDryPart(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nDryPart(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setDdel_dTzdDry(-values[0]);
    aux1_->setDrat_dTzdDry(-values[2]);
  };
  if (aux2_)
  {
    aux2_->setDdel_dTzdDry( values[1]);
    aux2_->setDrat_dTzdDry( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nWetPart(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nWetPart(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nWetPart(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setDdel_dTzdWet(-values[0]);
    aux1_->setDrat_dTzdWet(-values[2]);
  };
  if (aux2_)
  {
    aux2_->setDdel_dTzdWet( values[1]);
    aux2_->setDrat_dTzdWet( values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nGradPar(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nGradPar(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nGradPar(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (aux1_)
  {
    aux1_->setDdel_dTzdGrdN( values[0]);
    aux1_->setDdel_dTzdGrdE( values[2]);
    aux1_->setDrat_dTzdGrdN( values[4]);
    aux1_->setDrat_dTzdGrdE( values[6]);
  };
  if (aux2_)
  {
    aux2_->setDdel_dTzdGrdN( values[1]);
    aux2_->setDdel_dTzdGrdE( values[3]);
    aux2_->setDrat_dTzdGrdN( values[5]);
    aux2_->setDrat_dTzdGrdE( values[7]);
  };
};



//
void VcCalc2SessionIfc::put4_bendPart(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_bendPart(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_bendPart(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dBend(values[0]);
    obs_->setDrat_dBend(values[1]);
  };
};




void VcCalc2SessionIfc::put4_ut1_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    // revert the sign: d*/d(TAI-UT1)
//  obs_->setDdel_dUT1  (-values[0]);
//  obs_->setDrat_dUT1  (-values[1]);
    obs_->setDdel_dUT1  (-values[0]*86400.0);
    obs_->setDrat_dUT1  (-values[1]*86400.0);
    obs_->setD2del_dUT12(-values[2]);
    obs_->setD2rat_dUT12(-values[3]);
  };
};




void VcCalc2SessionIfc::put4_wob_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wob_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wob_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dPx(values[0]);
    obs_->setDdel_dPy(values[1]);
    obs_->setDrat_dPx(values[2]);
    obs_->setDrat_dPy(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nut06XyP(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06XyP(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06XyP(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dCipX(values[0]);
    obs_->setDdel_dCipY(values[1]);
    obs_->setDrat_dCipX(values[2]);
    obs_->setDrat_dCipY(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_str_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_str_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_str_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dRA(values[0]);
    obs_->setDdel_dDN(values[1]);
    obs_->setDrat_dRA(values[2]);
    obs_->setDrat_dDN(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_sit_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sit_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==2 && dim3==2))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sit_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dR_1(Sg3dVector(values[ 0], values[ 1], values[ 2]));
    obs_->setDdel_dR_2(Sg3dVector(values[ 3], values[ 4], values[ 5]));
    obs_->setDrat_dR_1(Sg3dVector(values[ 6], values[ 7], values[ 8]));
    obs_->setDrat_dR_2(Sg3dVector(values[ 9], values[10], values[11]));
  };
};



//
void VcCalc2SessionIfc::put4_consPart(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consPart(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consPart(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dGamma(values[0]);
    obs_->setDrat_dGamma(values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_plx_part(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_plx_part(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_plx_part(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dParallax(values[0]);
    obs_->setDrat_dParallax(values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_pTdXyPar(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_pTdXyPar(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_pTdXyPar(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dPolTideX(values[0]);
    obs_->setDdel_dPolTideY(values[1]);
    obs_->setDrat_dPolTideX(values[2]);
    obs_->setDrat_dPolTideY(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_consnDel(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consnDel(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consnDel(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
    obs_->setCalcConsensusDelay((values[0] + values[1])*1.0e-6);
};



//
void VcCalc2SessionIfc::put4_consnRat(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consnRat(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==1 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_consnRat(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
    obs_->setCalcConsensusRate(values[0]);
};



//
void VcCalc2SessionIfc::put4_con_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_con_cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_con_cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcConsBendingDelay(values[0]);
    obs_->setCalcConsBendingRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_sun_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun_cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun_cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcConsBendingSunDelay(values[0]);
    obs_->setCalcConsBendingSunRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_sun2cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun2cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun2cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcConsBendingSunHigherDelay(values[0]);
    obs_->setCalcConsBendingSunHigherRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_etd_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_etd_cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_etd_cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcEarthTideDelay(values[0]);
    obs_->setCalcEarthTideRate (values[1]);
  };
};



void VcCalc2SessionIfc::put4_feed_cor(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_feed_cor(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_feed_cor(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcFeedCorrDelay(values[0]);
    obs_->setCalcFeedCorrRate (values[1]);
    if (o_)
    {
      o_->setCalcFeedCorrDelay(values[0]);
      o_->setCalcFeedCorrRate (values[1]);
    };
  };
};




void VcCalc2SessionIfc::put4_oce_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_cont: the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_cont: the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcOceanTideDelay(values[0]);
    obs_->setCalcOceanTideRate (values[1]);
  };
};




void VcCalc2SessionIfc::put4_oce_old (const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_old(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_oce_old(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcOceanTideOldDelay(values[0]);
    obs_->setCalcOceanTideOldRate (values[1]);
  };
};




void VcCalc2SessionIfc::put4_optlCont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_optlCont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_optlCont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcOceanPoleTideLdDelay(values[0]);
    obs_->setCalcOceanPoleTideLdRate (values[1]);
  };
};




void VcCalc2SessionIfc::put4_ptd_cont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ptd_cont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ptd_cont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcPoleTideDelay(values[0]);
    obs_->setCalcPoleTideRate (values[1]);
  };
};





void VcCalc2SessionIfc::put4_ptOldCon(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ptOldCon(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ptOldCon(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcPoleTideOldDelay(values[0]);
    obs_->setCalcPoleTideOldRate (values[1]);
  };
};





void VcCalc2SessionIfc::put4_tiltRmvr(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_tiltRmvr(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_tiltRmvr(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcTiltRemvrDelay(values[0]);
    obs_->setCalcTiltRemvrRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_ut1Ortho(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1Ortho(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1Ortho(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcHiFyUt1Delay(values[0]);
    obs_->setCalcHiFyUt1Rate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_uvf_asec(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_uvf_asec(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_uvf_asec(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_ && o_)
  {
    o_->setUvFrPerAsec(0, values[0]);
    o_->setUvFrPerAsec(1, values[1]);
  };
};




//
void VcCalc2SessionIfc::put4_wobOrtho(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobOrtho(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobOrtho(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcHiFyPxyDelay(values[0]);
    obs_->setCalcHiFyPxyRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_ut1Libra(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1Libra(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1Libra(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcHiFyUt1LibrationDelay(values[0]);
    obs_->setCalcHiFyUt1LibrationRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_wobLibra(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobLibra(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobLibra(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcHiFyPxyLibrationDelay(values[0]);
    obs_->setCalcHiFyPxyLibrationRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_wobXcont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobXcont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobXcont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcPxDelay(values[0]);
    obs_->setCalcPxRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_wobYcont(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobYcont(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_wobYcont(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcPyDelay(values[0]);
    obs_->setCalcPyRate (values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_plx1psec(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_plx1psec(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_plx1psec(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setDdel_dParallaxRev(values[0]);
    obs_->setDrat_dParallaxRev(values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_ct_site1(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ct_site1(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==1 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ct_site1(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setFractC(values[0]);
  };
};



//
void VcCalc2SessionIfc::put4_sun_data(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun_data(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_sun_data(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setRsun(Sg3dVector(values[0], values[1], values[2]));
    obs_->setVsun(Sg3dVector(values[3], values[4], values[5]));
  };
};



//
void VcCalc2SessionIfc::put4_moonData(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_moonData(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_moonData(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setRmoon(Sg3dVector(values[0], values[1], values[2]));
    obs_->setVmoon(Sg3dVector(values[3], values[4], values[5]));
  };
};



//
void VcCalc2SessionIfc::put4_earth_ce(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_earth_ce(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==3 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_earth_ce(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setRearth(Sg3dVector(values[0], values[1], values[2]));
    obs_->setVearth(Sg3dVector(values[3], values[4], values[5]));
    obs_->setAearth(Sg3dVector(values[6], values[7], values[8]));
  };
};



//
void VcCalc2SessionIfc::put4_nut06Xys(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcCipXv(values[0]);
    obs_->setCalcCipYv(values[1]);
    obs_->setCalcCipSv(values[2]);
    obs_->setCalcCipXr(values[3]);
    obs_->setCalcCipYr(values[4]);
    obs_->setCalcCipSr(values[5]);
  };
};



//
void VcCalc2SessionIfc::put4_nut2006A(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcNut2006_dPsiV(values[0]);
    obs_->setCalcNut2006_dEpsV(values[1]);
    obs_->setCalcNut2006_dPsiR(values[2]);
    obs_->setCalcNut2006_dEpsR(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_nut_wahr(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==2 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_nut06Xys(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcNutWahr_dPsiV(values[0]);
    obs_->setCalcNutWahr_dEpsV(values[1]);
    obs_->setCalcNutWahr_dPsiR(values[2]);
    obs_->setCalcNutWahr_dEpsR(values[3]);
  };
};



//
void VcCalc2SessionIfc::put4_cf2j2k  (const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_cf2j2k(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==3 && dim2==3 && dim3==3))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_cf2j2k(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    Sg3dMatrix                  val(values[ 0], values[ 1], values[ 2], 
                                    values[ 3], values[ 4], values[ 5], 
                                    values[ 6], values[ 7], values[ 8] );
    Sg3dMatrix                  rat(values[ 9], values[10], values[11], 
                                    values[12], values[13], values[14], 
                                    values[15], values[16], values[17] );
    Sg3dMatrix                  acc(values[18], values[19], values[20], 
                                    values[21], values[22], values[23], 
                                    values[24], values[25], values[26] );
    obs_->setTrf2crfVal(val);
    obs_->setTrf2crfRat(rat);
    obs_->setTrf2crfAcc(acc);
  };
};




void VcCalc2SessionIfc::put4_ut1_tai (const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1_tai(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==1 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_ut1_tai(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcUt1_Tai(values[0]);
  };
};



//
void VcCalc2SessionIfc::put4_polar_xy(const double values[], short dim1, short dim2, short dim3)
{
  if (numOfObservations_==0 || numOfObservations_<obsIdx_)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_polar_xy(): the obs.index (" + QString("").setNum(obsIdx_) + 
      ") is out of range [0..." + QString("").setNum(numOfObservations_) + "]");
    return;
  };
  if (!(dim1==2 && dim2==1 && dim3==1))
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::put4_polar_xy(): the dimensions (" + QString("").setNum(dim1) + 
      "," + QString("").setNum(dim2) + "," + QString("").setNum(dim3) +
      ") are unexpected");
    return;
  };
  if (obs_)
  {
    obs_->setCalcPmX(values[0]);
    obs_->setCalcPmY(values[1]);
  };
};



//
void VcCalc2SessionIfc::put4_obsolete(UNUSED const double values[], 
  UNUSED short dim1, UNUSED short dim2, UNUSED short dim3)
{
};





/*=====================================================================================================*/














/*=====================================================================================================*/
