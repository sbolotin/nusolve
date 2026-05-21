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
#include <stdlib.h>
#include <math.h>


#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>


#include <Sg3dMatrixR.h>
#include <SgConstants.h>
#include <SgEccRec.h>
#include <SgLogger.h>
#include <SgRefraction.h>
#include <SgVlbiStationInfo.h>
#include <SgVlbiObservation.h>



const int SgVlbiStationInfo::maxNumOfPolynomials_=MAX_NUMBER_OF_POLYNOMIALS;


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgVlbiStationInfo::className()
{
  return "SgVlbiStationInfo";
};



//
int SgVlbiStationInfo::mntType2int(MountingType mType)
{
  int                           mnt=0;
  switch (mType)
  {
    case MT_EQUA:
      mnt = 1;
      break;
    case MT_X_YN:
      mnt = 2;
      break;
    case MT_AZEL:
      mnt = 3;
      break;
    case MT_RICHMOND:
      mnt = 4;
      break;
    case MT_X_YE:
      mnt = 5;
      break;
    case MT_UNKN:
    default:
      mnt = 0;
      break;
  };
  return mnt;
};



//
SgVlbiStationInfo::MountingType SgVlbiStationInfo::int2mntType(int iMnt)
{
  MountingType                  mnt=MT_UNKN;
  switch (iMnt)
  {
    case 1:
      mnt = MT_EQUA;
      break;
    case 2:
      mnt = MT_X_YN;
      break;
    case 3:
      mnt = MT_AZEL;
      break;
    case 4:
      mnt = MT_RICHMOND;
      break;
    case 5:
      mnt = MT_X_YE;
      break;
    default:
      logger->write(SgLogger::WRN, SgLogger::IO_NCDF, className() +
        "::int2mntType(): got unknown value of the axis type (" + QString("").setNum(iMnt) + ")");
      break;
    };
  return mnt;
};
//



//
// An empty constructor:
SgVlbiStationInfo::SgVlbiStationInfo(int idx, const QString& key, const QString& aka) :
  SgObjectInfo(idx, key, aka),
  clockBreaks_(),
  auxObservationByScan_(),
  cableCalsOriginTxt_(""),
  meteoDataOriginTxt_(""),
  r_(v3Zero),
  r_ea_(v3Zero),
  v_ea_(v3Zero),
  v3Ecc_(v3Zero),
  tsysIfFreqs_(),
  tsysIfIds_(),
  tsysIfSideBands_(),
  tsysIfPolarizations_(),
  pcClocks_(),
  pcZenith_(),
  numByPolarization_()
{
  clocksModelOrder_ = 2;
  cableCalMultiplier_ = 1.0;
  flybyCableCalSource_ = SgTaskConfig::CCS_DEFAULT;
  refractionModel_     = SgTaskConfig::RM_NONE;
  cableCalMultiplierDBCal_ = 0.0;

  cableCalsOrigin_ = CCO_UNDEF;
  meteoDataOrigin_ = MDO_UNDEF;
  
  axisOffset_ = 0.0;
  axisOffset_ea_ = 0.0;
  //
  gradNorth_ = 0.0;
  gradEast_  = 0.0;
  tilt_[0] = tilt_[1] = 0.0;
  //
  mntType_ = MT_UNKN;
  latitude_= longitude_= height_ = 0.0;
  tectonicPlateName_ = "NONE";
  cdpNumber_ = -1;

  for (int i=0; i<maxNumOfPolynomials_; i++)
  {
    estClockModel_[i] = 0.0;
    estClockModelSigmas_[i] = 0.0;
    pClocks_[i] = NULL;
  };
  aPrioriClockTerm_0_ = 0.0;
  aPrioriClockTerm_1_ = 0.0;
  need2useAPrioriClocks_ = false;
  //
  estWetZenithDelay_ = 0.0;
  estWetZenithDelaySigma_ = 0.0;
  gradientDelay_ = 0.0;

  for (int iWave=0; iWave<11; iWave++)
    for (int iCoord=0; iCoord<3; iCoord++)
    {
      oLoadAmplitudes_[iWave][iCoord] = 0.0;
      oLoadPhases_[iWave][iCoord] = 0.0;
    };
  for (int i=0; i<6; i++)
    optLoadCoeffs_[i] = 0.0;
  cId_ = ' ';
  sId_[0] = ' ';
  sId_[1] = ' ';
  sId_[2] = '\0';
  //
  // parameters:
  pZenithDelay_ = NULL;
  pAtmGradN_ = NULL;
  pAtmGradE_ = NULL;
  pRx_ = NULL;
  pRy_ = NULL;
  pRz_ = NULL;
  pAxisOffset_ = NULL;
  addAttr(Attr_ESTIMATE_COO);
  addAttr(Attr_ESTIMATE_AXO);
  eccRec_ = NULL;
  isEccNonZero_ = false;
  cccIdx_ = SgVlbiAuxObservation::CCT_DFLT;
  netPolarizations_ = NP_UNDEF;
};



//
// A destructor:
SgVlbiStationInfo::~SgVlbiStationInfo()
{
  releaseParameters();

  QMap<QString, SgVlbiAuxObservation*>::iterator it;
  for (it=auxObservationByScan_.begin(); it!=auxObservationByScan_.end(); ++it)
    delete it.value();
  auxObservationByScan_.clear();
  
  if (eccRec_)
  {
    delete eccRec_;
    eccRec_ = NULL;
  };

  tsysIfFreqs_.clear();
  tsysIfIds_.clear();
  tsysIfSideBands_.clear();
  tsysIfPolarizations_.clear();
};



//
void SgVlbiStationInfo::createParameters()
{
  QString prefix = "Stn " + getKey().leftJustified(8, ' ') + ": ";
  releaseParameters();
  for (int i=0; i<maxNumOfPolynomials_; i++)
    pClocks_[i] = new SgParameter(prefix + QString("").sprintf("Clock_%02d", i));
  pZenithDelay_ = new SgParameter(prefix + "Zenith");
  pAtmGradN_ = new SgParameter(prefix + "Grad_N");
  pAtmGradE_ = new SgParameter(prefix + "Grad_E");
  pRx_ = new SgParameter(prefix + "coord-X");
  pRy_ = new SgParameter(prefix + "coord-Y");
  pRz_ = new SgParameter(prefix + "coord-Z");
  pAxisOffset_ = new SgParameter(prefix + "Axis Offset");
  clockBreaks_.createParameters(prefix + "CBr");
};



//
void SgVlbiStationInfo::releaseParameters()
{
  for (int i=0; i<maxNumOfPolynomials_; i++)
    if (pClocks_[i])
    {
      delete pClocks_[i];
      pClocks_[i] = NULL;
    };
  if (pZenithDelay_)
  {
    delete pZenithDelay_;
    pZenithDelay_ = NULL;
  };
  if (pAtmGradN_)
  {
    delete pAtmGradN_;
    pAtmGradN_ = NULL;
  };
  if (pAtmGradE_)
  {
    delete pAtmGradE_;
    pAtmGradE_ = NULL;
  };
  if (pRx_)
  {
    delete pRx_;
    pRx_ = NULL;
  };
  if (pRy_)
  {
    delete pRy_;
    pRy_ = NULL;
  };
  if (pRz_)
  {
    delete pRz_;
    pRz_ = NULL;
  };
  if (pAxisOffset_)
  {
    delete pAxisOffset_;
    pAxisOffset_ = NULL;
  };
  clockBreaks_.releaseParameters();
};



//
void SgVlbiStationInfo::calcCBEpochs4Export(const QMap<QString, SgVlbiAuxObservation*>& auxObsByScan)
{
  if (!auxObsByScan.size())
  {
    logger->write(SgLogger::WRN, SgLogger::STATION, className() +
      "::calcCBEpochs4Export(): cannot determine the epoch, the size of auxObs map is zero");
    return;
  };
  for (int i=0; i<clockBreaks_.size(); i++)
  {
    SgParameterBreak*   pb=clockBreaks_.at(i);
    SgMJD               tLeft(tZero), tRight(tInf);
    QMap<QString, SgVlbiAuxObservation*>::const_iterator it;
    for (it=auxObsByScan.begin(); it!=auxObsByScan.end(); ++it)
    {
      SgVlbiAuxObservation*   obs=it.value();
//    if (false || obs->isAttr(SgVlbiAuxObservation::Attr_PROCESSED))//ok, right now we use only good obs
      if (true || obs->isAttr(SgVlbiAuxObservation::Attr_PROCESSED)) // use all obs
      {
        if (*obs<*pb && tLeft<*obs)
          tLeft = *obs;
        if (*pb<*obs && *obs<tRight)
          tRight = *obs;
      };
    };
    if (tZero<tLeft && tRight<tInf)
    {
      // calculating mean epoch:
      SgMJD     exportEpoch(tLeft);
      exportEpoch += 0.5*(tRight - tLeft);
      // round it to minutes:
      double d=exportEpoch.getTime();
      d = round(d*24.0*60.0)/24.0/60.0;
      exportEpoch.setTime(d);
      // setup epoch for export:
      pb->setEpoch4Export(exportEpoch);
      logger->write(SgLogger::DBG, SgLogger::STATION, className() +
        "::calcCBEpochs4Export(): station " + name() + ": set up clock break epoch for export: " +
        exportEpoch.toString());
    }
    else
    {
      pb->setEpoch4Export(*pb);
      logger->write(SgLogger::WRN, SgLogger::STATION, className() +
        "::calcCBEpochs4Export(): cannot determine clock break epoch for export: tLeft: " +
        tLeft.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + ", tRight: " +
        tRight.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    };
  };
};

/*
Sg3dVector SgVlbiStationInfo::r(const SgMJD& t, SgTaskConfig* cfg)
{
  Sg3dVector    r(r_);
  if (cfg->getUseExtAPrioriSitesPositions())
    r = r_ea_ + v_ea_*(t - t0_);
  return r;
};
*/



//
void SgVlbiStationInfo::adjustEccVector()
{
  isEccNonZero_ = true;
  if (eccRec_->getEccType()==SgEccRec::ET_XYZ)
    v3Ecc_ = eccRec_->getDR();
  else if (eccRec_->getEccType()==SgEccRec::ET_NEU)
  {
    Sg3dMatrixR         R_e(EAST), R_n(NORTH);
    Sg3dMatrix          m3M=R_n(-longitude_)*R_e(latitude_);
    v3Ecc_ = m3M*eccRec_->getDR();
  };
};



//
void SgVlbiStationInfo::checkAuxObs(const QMap<QString, QString>& scanNameById)
{
  if (!auxObservationByScan_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::STATION, className() +
        "::checkAuxObs(): station " + name() + ": the size of aux.obs's container is zero");
    return;
  };
  QMap<SgMJD, QString>          scanByTime;
  QMap<QString, SgVlbiAuxObservation*>::iterator
                                it=auxObservationByScan_.begin();
  SgVlbiAuxObservation         *aux=NULL;
  //
  while (it != auxObservationByScan_.end())
  {
    // check for the "same epoch" case:
    aux = it.value();
    if (!scanByTime.contains(*aux))
      scanByTime.insert(*aux, it.key());
    else
    {
      QString                   scanName=scanByTime.value(*aux);
      logger->write(SgLogger::WRN, SgLogger::STATION, className() +
        "::checkAuxObs(): station " + name() + ": contains different observations at the same epoch: " +
        aux->toString() + "; the scans: " + scanName + " and " + it.key(), true);
    };
    // remove unused data:
    if (!scanNameById.contains(it.key()))
    {
      logger->write(SgLogger::INF, SgLogger::STATION, className() +
        "::checkAuxObs(): the scan " + it.key() + 
        " was removed from the list of known scans of the station " + name());
      it = auxObservationByScan_.erase(it);
    }
    else
      ++it;
  };
  //
  //
  // set up troposphere model values:
  SgTroposphereModelData       *tmd;
  for (it=auxObservationByScan_.begin(); it!=auxObservationByScan_.end(); ++it)
  {
    aux             = it.value();
    tmd             = aux->tmdContribInternal();
   
    tmd->setVal0_delay(aux->getCalcNdryCont4Delay());
    tmd->setVal1_delay(aux->getCalcNwetCont4Delay());
    tmd->setMap0_delay(aux->getDdel_dTzdDry());
    tmd->setMap1_delay(aux->getDdel_dTzdWet());

    tmd->setVal0_rate (aux->getCalcNdryCont4Rate());
    tmd->setVal1_rate (aux->getCalcNwetCont4Rate());
    tmd->setMap0_rate (aux->getDrat_dTzdDry());
    tmd->setMap1_rate (aux->getDrat_dTzdWet());
      
    // gradients:
    tmd->setGrdN_delay(aux->getDdel_dTzdGrdN());
    tmd->setGrdE_delay(aux->getDdel_dTzdGrdE());
    tmd->setGrdN_rate (aux->getDrat_dTzdGrdN());
    tmd->setGrdE_rate (aux->getDrat_dTzdGrdE());
  };  
/*
  for (; it!=auxObservationByScan_.end(); ++it)
  {
    SgVlbiAuxObservation       *auxObs=it.value();
    if (!scanByTime.contains(*auxObs))
      scanByTime.insert(*auxObs, it.key());
    else
    {
      QString                   scanName=scanByTime.value(*auxObs);
      logger->write(SgLogger::WRN, SgLogger::STATION, className() +
        ": WARNING: the station " + getKey() + ": contains different observations at the same epoch: " +
        auxObs->toString() + "; the scans: " + scanName + " and " + it.key(), true);
    };
  };
*/
};



//
bool SgVlbiStationInfo::saveIntermediateResults(QDataStream& s) const
{
  SgObjectInfo::saveIntermediateResults(s);
  
  s << clocksModelOrder_ << estWetZenithDelay_ << estWetZenithDelaySigma_
    << aPrioriClockTerm_0_ << aPrioriClockTerm_1_ << need2useAPrioriClocks_
    << (unsigned int)flybyCableCalSource_;

  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data");
    return false;
  };
  //
  for (int i=0; i<clocksModelOrder_; i++)
    if (s.status() == QDataStream::Ok)
      s << estClockModel_[i] << estClockModelSigmas_[i];
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::saveIntermediateResults(): error writting data idx#" + QString("").setNum(i));
      return false;
    };
  //
  if (s.status() == QDataStream::Ok)
    clockBreaks_.saveIntermediateResults(s);

  if (!pcClocks_.saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data for the local clock parameter setup");
    return false;
  };
  if (!pcZenith_.saveIntermediateResults(s))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::saveIntermediateResults(): error writting data for the local zenith parameter setup");
    return false;
  };
  //
  return s.status() == QDataStream::Ok;
};



//
bool SgVlbiStationInfo::loadIntermediateResults(QDataStream& s)
{
  int                           n;
  double                        v, e, a_0, a_1;
  bool                          is;
  unsigned int                  ccs;

  SgObjectInfo::loadIntermediateResults(s);

  s >> n >> v >> e >> a_0 >> a_1 >> is >> ccs;

  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  clocksModelOrder_ = n;
  estWetZenithDelay_ = v;
  estWetZenithDelaySigma_ = e;

  aPrioriClockTerm_0_ = a_0;
  aPrioriClockTerm_1_ = a_1;
  need2useAPrioriClocks_ = is;
  flybyCableCalSource_ = (SgTaskConfig::CableCalSource)ccs;
  //
  //
  for (int i=0; i<clocksModelOrder_; i++)
  {
    s >> v >> e;
    if (s.status() == QDataStream::Ok)
    {
      estClockModel_[i] = v;
      estClockModelSigmas_[i] = e;
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        "::loadIntermediateResults(): error reading data idx#" + QString("").setNum(i) + ": " + 
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  };
  //
  clockBreaks_.loadIntermediateResults(s);
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading clock breaks data: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  pcClocks_.loadIntermediateResults(s);
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading local clock parameter setup: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  pcZenith_.loadIntermediateResults(s);
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      "::loadIntermediateResults(): error reading local zenith parameter setup: " +
      (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
    return false;
  };
  //
  return s.status()==QDataStream::Ok;
};



//
void SgVlbiStationInfo::resetCable()
{
  for (QMap<QString, SgVlbiAuxObservation*>::iterator it=auxObservationByScan_.begin(); 
    it!=auxObservationByScan_.end(); ++it)
    it.value()->resetCable();
  // clear the attributes too:
  delAttr(Attr_HAS_CABLE_CAL);
};



//
void SgVlbiStationInfo::resetMeteo()
{
  for (QMap<QString, SgVlbiAuxObservation*>::iterator it=auxObservationByScan_.begin(); 
    it!=auxObservationByScan_.end(); ++it)
    it.value()->resetMeteo();
  // clear the attributes too:
  delAttr(Attr_HAS_METEO);
};



//
void SgVlbiStationInfo::resetTsys()
{
  for (QMap<QString, SgVlbiAuxObservation*>::iterator it=auxObservationByScan_.begin(); 
    it!=auxObservationByScan_.end(); ++it)
    it.value()->resetTsys();
  // clear the attributes too:
  delAttr(Attr_HAS_TSYS);
};



//
void SgVlbiStationInfo::checkPresenceOfCableCalibratioCorrections()
{
  delAttr(Attr_HAS_CCC_FSLG);
  delAttr(Attr_HAS_CCC_CDMS);
  delAttr(Attr_HAS_CCC_PCMT);
  bool                          hasFslg, hasCdms, hasPcmt;
  hasFslg = hasCdms = hasPcmt = false;
  //
  for (QMap<QString, SgVlbiAuxObservation*>::iterator it=auxObservationByScan_.begin();
    it!=auxObservationByScan_.end(); ++it)
  {
    SgVlbiAuxObservation*       aux=it.value();
    if (!hasFslg && aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_FSLG) != 0.0)
      hasFslg = true;
    if (!hasCdms && aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_CDMS) != 0.0)
      hasCdms = true;
    if (!hasPcmt && aux->cableCorrections().getElement(SgVlbiAuxObservation::CCT_PCMT) != 0.0)
      hasPcmt = true;
  };
  //
  if (hasFslg)
    addAttr(Attr_HAS_CCC_FSLG);
  if (hasCdms)
    addAttr(Attr_HAS_CCC_CDMS);
  if (hasPcmt)
    addAttr(Attr_HAS_CCC_PCMT);
};



//
void SgVlbiStationInfo::prepare2Run(const SgTaskConfig* cfg, SgRefraction* refraction)
{

  SgTaskConfig::CableCalSource  src=cfg->getFlybyCableCalSource();

  if (flybyCableCalSource_ != SgTaskConfig::CCS_DEFAULT)
    src = flybyCableCalSource_;
  switch (src)
  {
  case SgTaskConfig::CCS_FSLG:
    cccIdx_ = isAttr(Attr_HAS_CCC_FSLG)?SgVlbiAuxObservation::CCT_FSLG:SgVlbiAuxObservation::CCT_DFLT;
    break;
  case SgTaskConfig::CCS_CDMS:
    cccIdx_ = isAttr(Attr_HAS_CCC_CDMS)?SgVlbiAuxObservation::CCT_CDMS:SgVlbiAuxObservation::CCT_DFLT;
    break;
  case SgTaskConfig::CCS_PCMT:
    cccIdx_ = isAttr(Attr_HAS_CCC_PCMT)?SgVlbiAuxObservation::CCT_PCMT:SgVlbiAuxObservation::CCT_DFLT;
    break;
  case SgTaskConfig::CCS_DEFAULT:
  default:
    cccIdx_ = SgVlbiAuxObservation::CCT_DFLT;
  };
  //
  //
  if (cfg->getRefractionModel() != refractionModel_) // the refraction model has changed:
  {
    //
    // store the last model:
    refractionModel_ = cfg->getRefractionModel();
    //
    // iterate through auxObs:
    double                      dDryZd_del, dWetZd_del;
    double                      dDryMf_del, dWetMf_del; //, dGrdMf;
    double                      dDryMf_rat, dWetMf_rat; //, dGrdMf;
    double                      dElevation, dAzimuth;
    double                      dElevationRate, dAzimuthRate;
    double                      cosA, sinA, sinE, tanE;
    SgTroposphereModelData     *tmd=NULL;
    SgVlbiAuxObservation       *aux=NULL;

    for (QMap<QString, SgVlbiAuxObservation*>::iterator it=auxObservationByScan_.begin();
      it!=auxObservationByScan_.end(); ++it)
    {
      aux = it.value();
      if (refractionModel_ == SgTaskConfig::RM_FLYBY_NMF || // need to recalculate:
          refractionModel_ == SgTaskConfig::RM_FLYBY_MTT  )
      {
        dElevation    = aux->getElevationAngle();
        dAzimuth      = aux->getAzimuthAngle();
        dElevationRate= aux->getElevationAngleRate();
        dAzimuthRate  = aux->getAzimuthAngleRate();

        sincos(dAzimuth, &sinA, &cosA);
        tanE = tan(dElevation);
        sinE = sin(dElevation);

        refraction->calcTroposphericDelay(*aux->getOwner(), aux->getMeteoData(),
          dElevation, dAzimuth, this, aux->getOwner()->src());

        dDryZd_del = refraction->dDryZD();
        dWetZd_del = refraction->dWetZD();
        dDryMf_del = refraction->dDryMF();
        dWetMf_del = refraction->dWetMF();
        dDryMf_rat = refraction->dryMappingFunction()->dM_dE()*dElevationRate;
        dWetMf_rat = refraction->wetMappingFunction()->dM_dE()*dElevationRate;

        tmd = aux->tmdFlyBy();

        tmd->setVal0_delay(dDryZd_del*dDryMf_del/vLight);
        tmd->setVal1_delay(dWetZd_del*dWetMf_del/vLight);
        tmd->setMap0_delay(dDryMf_del);
        tmd->setMap1_delay(dWetMf_del);

        tmd->setVal0_rate (dDryZd_del*dDryMf_rat/vLight);
        tmd->setVal1_rate (dWetZd_del*dWetMf_rat/vLight);
        tmd->setMap0_rate (dDryMf_rat);
        tmd->setMap1_rate (dWetMf_rat);

        // gradients:
        tmd->setGrdN_delay(cosA/tanE*dDryMf_del);
        tmd->setGrdE_delay(sinA/tanE*dDryMf_del);
        
        tmd->setGrdN_rate (-sinA/tanE*dDryMf_del*dAzimuthRate -
                            cosA/sinE/sinE*dDryMf_del*dElevationRate + cosA/tanE*dDryMf_rat);
        tmd->setGrdE_rate ( cosA/tanE*dDryMf_del*dAzimuthRate -
                            sinA/sinE/sinE*dDryMf_del*dElevationRate + sinA/tanE*dDryMf_rat);
      };
      //
      // setup the actual:
      switch (refractionModel_)
      {
        case SgTaskConfig::RM_CONTRIB_INTERNAL:
          aux->tmdActual() = aux->tmdContribInternal();
        break;
        //
        case SgTaskConfig::RM_CONTRIB_EXTERNAL:
          aux->tmdActual() = aux->tmdContribExternal();
        break;
        //
        case SgTaskConfig::RM_FLYBY_NMF:
        case SgTaskConfig::RM_FLYBY_MTT:
          aux->tmdActual() = aux->tmdFlyBy();
        break;
        //
        default:
        case SgTaskConfig::RM_NONE:
          aux->tmdActual() = &tmdZero;
        break;
      };

if (aux->tmdActual()->getMap0_delay() == 1.0)
std::cout << " -- Got getMap0_delay() == 1.0:"
<< " stn=[" << qPrintable(getKey()) << "] "
<< " scanId=[" << qPrintable(it.key()) << "] "
<< "\n";

    };
  };


  // 
  logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() +
    "::prepare2Run(): station " + name() + ": ccc index of cable calibration corrections is set to " +
    QString("").setNum(cccIdx_));
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
QString netPolarizations2string(unsigned int p)
{
  QString                       str("");
  
  if (p == SgVlbiStationInfo::NP_UNDEF)
    str = "UNDEF";
  else
  {
    QStringList                 ps;
    if (p & SgVlbiStationInfo::NP_CircPolarizRight)
      ps << "R";
    if (p & SgVlbiStationInfo::NP_CircPolarizLeft)
      ps << "L";
    if (p & SgVlbiStationInfo::NP_LinearPolarizX)
      ps << "X";
    if (p & SgVlbiStationInfo::NP_LinearPolarizY)
      ps << "Y";
    if (p & SgVlbiStationInfo::NP_LinearPolarizH)
      ps << "H";
    if (p & SgVlbiStationInfo::NP_LinearPolarizV)
      ps << "V";
    str = ps.join(",");
  };
  return str;
};
/*=====================================================================================================*/
//
// constants:
//












/*=====================================================================================================*/
