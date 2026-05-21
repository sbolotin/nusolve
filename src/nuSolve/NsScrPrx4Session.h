/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2017-2020 Sergei Bolotin.
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

#ifndef NS_SCR_PRX_4_SESSION_H
#define NS_SCR_PRX_4_SESSION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QtGlobal>


#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>


#if 0 < HAVE_SCRIPTS
#    include <QtScript/QScriptEngine>
#    include <QtScript/QScriptValue>
#endif

#include <SgObjectInfo.h>
#include <SgVlbiBand.h>
#include <SgVlbiBaselineInfo.h>
#include <SgVlbiSession.h>
#include <SgVlbiSourceInfo.h>
#include <SgVlbiStationInfo.h>


#include "NsScrPrx4TaskConfig.h"


class NsScrPrx4VlbiObservation;
class NsScrPrx4VlbiAuxObservation;
class NsScrPrx4Session;
class NsScrPrx4Station;
class NsScrPrx4Baseline;
class NsScrPrx4Source;


/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Object : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString key
    READ getKey)
  Q_PROPERTY(QString name
    READ getName)
  Q_PROPERTY(int numTotal
    READ getNumTotal)
  Q_PROPERTY(int numProcessed
    READ getNumProcessed)
  Q_PROPERTY(int numUsable
    READ getNumUsable)
  Q_PROPERTY(QDateTime tFirst
    READ getTfirst)
  Q_PROPERTY(QDateTime tLast
    READ getTlast)
  Q_PROPERTY(double sigma2add
    READ getSigma2add WRITE setSigma2add)
//  Q_PROPERTY(double normalizedResid
//    READ getNormedResid)
//  Q_PROPERTY(double dispersion
//    READ getDispersion)
  Q_PROPERTY(double wrms
    READ getWrms)
  Q_PROPERTY(double chi2
    READ getChi2)
  Q_PROPERTY(double reducedChi2
    READ getReducedChi2)
  Q_PROPERTY(double dof
    READ getDof)


public:
  inline NsScrPrx4Object(SgObjectInfo& obj, QObject *parent=0)
    : QObject(parent) {obj_=&obj; setUpName();};
  inline ~NsScrPrx4Object() {obj_=NULL;};

public slots:
  inline const QString& getKey() const {return obj_->getKey();};
  inline QString getName() const {return name_;};
  inline int getNumTotal() const {return obj_->numTotal(DT_DELAY);};
  inline int getNumProcessed() const {return obj_->numProcessed(DT_DELAY);};
  inline int getNumUsable() const {return obj_->numUsable(DT_DELAY);};
  inline QDateTime getTfirst() const {return obj_->tFirst(DT_DELAY).toQDateTime();};
  inline QDateTime getTlast() const {return obj_->tLast(DT_DELAY).toQDateTime();};
   inline double getSigma2add() const {return obj_->getSigma2add(DT_DELAY);};
  //inline double getNormedResid() const {return obj_->normedResid(DT_DELAY);};
  //inline double getDispersion() const {return obj_->dispersion(DT_DELAY, false);};
  inline double getWrms() const {return obj_->wrms(DT_DELAY);};
  inline double getChi2() const {return obj_->chi2(DT_DELAY);};
  inline double getReducedChi2() const {return obj_->reducedChi2(DT_DELAY);};
  inline double getDof() const {return obj_->dof(DT_DELAY);};

  inline void setSigma2add(double d) {obj_->setSigma2add(DT_DELAY, d);};


protected:
  SgObjectInfo                 *obj_;
  QString                       name_;
  inline void setUpName() {name_=obj_->getKey().simplified();};
};
/*=====================================================================================================*/





/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Band : public NsScrPrx4Object
{
  Q_OBJECT
  Q_PROPERTY(double refFreq
    READ getRefFreq)
  Q_PROPERTY(int numOfChannels
    READ getNumOfChannels)
  Q_PROPERTY(QDateTime tCreation
    READ getTCreation)
  Q_PROPERTY(int inputFileVersion
    READ getInputFileVersion)
  Q_PROPERTY(QString correlatorType
    READ getCorrelatorType)
  Q_PROPERTY(double groupDelaysAmbigSpacing
    READ getGroupDelaysAmbigSpacing)
  Q_PROPERTY(const QList<NsScrPrx4Station*>& stations
    READ getStations)
  Q_PROPERTY(const QList<NsScrPrx4Baseline*>& baselines
    READ getBaselines)
  Q_PROPERTY(const QList<NsScrPrx4Source*>& sources
    READ getSources)
  Q_PROPERTY(QDateTime lastHistoryEntry
    READ getLastHistoryEntry)
  Q_PROPERTY(QDateTime calcHistoryEntry
    READ getCalcHistoryEntry)
  Q_PROPERTY(double meanEffectiveIntegrationTime
    READ getMeanEffectiveIntegrationTime)

public:
  inline NsScrPrx4Band(SgVlbiBand& band, QObject *parent=0) : NsScrPrx4Object(band, parent),
    prxStations_(), prxBaselines_(), prxSources_() {};
  inline ~NsScrPrx4Band() {};
  void postLoad(NsScrPrx4Session*);

public slots:
  inline double getRefFreq() const {return bnd()->getFrequency();};
  inline int getNumOfChannels() const {return bnd()->getMaxNumOfChannels();};
  inline QDateTime getTCreation() const {return bnd()->getTCreation().toQDateTime();};
  inline int getInputFileVersion() const {return bnd()->getInputFileVersion();};
  inline const QString& getCorrelatorType() const {return bnd()->getCorrelatorType();};
  inline double getGroupDelaysAmbigSpacing() const {return bnd()->typicalGrdAmbigSpacing();};
  inline const QList<NsScrPrx4Station*>& getStations() {return prxStations_;};
  inline const QList<NsScrPrx4Baseline*>& getBaselines() {return prxBaselines_;};
  inline const QList<NsScrPrx4Source*>& getSources() {return prxSources_;};
  inline QDateTime getLastHistoryEntry() {return bnd()->lastHistoryEntry().toQDateTime();};
  inline QDateTime getCalcHistoryEntry() {return bnd()->calcHistoryEntry().toQDateTime();};
  inline double getMeanEffectiveIntegrationTime() {return bnd()->meanEffectiveIntegrationTime();};

protected:
  inline SgVlbiBand* bnd() {return (SgVlbiBand*)obj_;};
  inline const SgVlbiBand* bnd() const {return (SgVlbiBand*)obj_;};

private:
  QList<NsScrPrx4Station*>      prxStations_;
  QList<NsScrPrx4Baseline*>     prxBaselines_;
  QList<NsScrPrx4Source*>       prxSources_;
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Station : public NsScrPrx4Object
{
  Q_OBJECT

  Q_PROPERTY(int numOfClockPolynoms
    READ getClocksModelOrder WRITE setClocksModelOrder)
  Q_PROPERTY(double cableCalSign
    READ getCableCalSign)
  Q_PROPERTY(double latitude
    READ getLatitude)
  Q_PROPERTY(double longitude
    READ getLongitude)
  Q_PROPERTY(QString sId
    READ getSid)

  // Attributes:
  Q_PROPERTY(bool isValid
    READ isValid WRITE setIsValid)
  Q_PROPERTY(bool estimateCoords
    READ getEstimateCoords WRITE setEstimateCoords)
  Q_PROPERTY(bool constrainCoords
    READ getConstrainCoords WRITE setConstrainCoords)
  Q_PROPERTY(bool referenceClocks
    READ getRreferenceClocks WRITE setRreferenceClocks)
  Q_PROPERTY(bool useCableCal
    READ getUseCableCal WRITE setUseCableCal)
  Q_PROPERTY(bool hasManualPcal
    READ getHasManualPcal)
  Q_PROPERTY(int numOfClockBreaks
    READ getNumOfClockBreaks)

  //
  Q_PROPERTY(bool badMeteo
    READ getBadMeteo WRITE setBadMeteo)
  //
  Q_PROPERTY(bool hasCccFslg
    READ getHasCccFslg)
  Q_PROPERTY(bool hasCccCdms
    READ getHasCccCdms)
  Q_PROPERTY(bool hasCccPcmt
    READ getHasCccPcmt)
  //
  Q_PROPERTY(bool estimateTroposphere
    READ getEstimateTroposphere WRITE setEstimateTroposphere)
  Q_PROPERTY(int numOfScans
    READ getNumOfScans)
  Q_PROPERTY(const QList<NsScrPrx4VlbiAuxObservation*>& auxObs
    READ getAuxObs)
  Q_PROPERTY(NsScrPrx4TaskConfig::CableCalSource flybyCableCalSource
    READ getFlybyCableCalSource)
//  WRITE setFlybyCableCalSource READ getFlybyCableCalSource)


public:
  inline NsScrPrx4Station(SgVlbiStationInfo& stn, QObject *parent=0) : 
    NsScrPrx4Object(stn, parent), prxAuxes_() {};
  inline ~NsScrPrx4Station() {prxAuxes_.clear();};

public slots:
  inline int getClocksModelOrder() const {return stn()->getClocksModelOrder();};
  inline double getCableCalSign() const {return stn()->getCableCalMultiplierDBCal();};
  inline bool isValid() const {return !stn()->isAttr(SgVlbiStationInfo::Attr_NOT_VALID);};
  inline bool getEstimateCoords() const {return stn()->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO);};
  inline bool getConstrainCoords() const {return stn()->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO);};
  inline bool getRreferenceClocks() const 
    {return stn()->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS);};
  inline bool getUseCableCal() const {return !stn()->isAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL);};
  inline bool getHasManualPcal() const {return stn()->isAttr(SgVlbiStationInfo::Attr_HAS_MANUAL_PCAL);};
  
  inline int getNumOfClockBreaks() const {return stn()->clockBreaks().size();};
  
  
  inline bool getBadMeteo() const {return stn()->isAttr(SgVlbiStationInfo::Attr_BAD_METEO);};
  inline bool getHasCccFslg() const {return stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_FSLG);};
  inline bool getHasCccCdms() const {return stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_CDMS);};
  inline bool getHasCccPcmt() const {return stn()->isAttr(SgVlbiStationInfo::Attr_HAS_CCC_PCMT);};
  inline bool getEstimateTroposphere() const 
    {return !stn()->isAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS);};
  inline NsScrPrx4TaskConfig::CableCalSource getFlybyCableCalSource() const 
    {return (NsScrPrx4TaskConfig::CableCalSource)stn()->getFlybyCableCalSource();};
 
  inline int getNumOfScans() const {return stn()->auxObservationByScanId()->size();};
  inline const QList<NsScrPrx4VlbiAuxObservation*>& getAuxObs() const {return prxAuxes_;};
  inline double getLatitude() const {return stn()->getLatitude();};
  inline double getLongitude() const {return stn()->getLongitude();};
  inline QString getSid() const {return stn()->getSid();};

  inline void setClocksModelOrder(int m) {stn()->setClocksModelOrder(m);};
  //  inline void setCableCalMultiplier(double d)  {stn()->setCableCalMultiplier(d);};
  inline void setIsValid(bool is) {stn()->assignAttr(SgVlbiStationInfo::Attr_NOT_VALID, !is);};

  inline void setEstimateCoords(bool is) {stn()->assignAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO, is);};
  inline void setConstrainCoords(bool is)
    {stn()->assignAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO, is);};
  inline void setRreferenceClocks(bool is)
    {stn()->assignAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS, is);};
  inline void setUseCableCal(bool is)
    {stn()->assignAttr(SgVlbiStationInfo::Attr_IGNORE_CABLE_CAL, !is);};
  inline void setBadMeteo(bool is)
    {stn()->assignAttr(SgVlbiStationInfo::Attr_BAD_METEO, is);};
  inline void setEstimateTroposphere(bool is)
    {stn()->assignAttr(SgVlbiStationInfo::Attr_DONT_ESTIMATE_TRPS, !is);};
//inline void setFlybyCableCalSource(NsScrPrx4TaskConfig::CableCalSource s) 
//  {stn()->setFlybyCableCalSource((SgTaskConfig::CableCalSource) s);};
  bool setFlybyCableCalSource(NsScrPrx4TaskConfig::CableCalSource s);


private:
  QList<NsScrPrx4VlbiAuxObservation*>
                                prxAuxes_;

  inline SgVlbiStationInfo* stn() {return (SgVlbiStationInfo*)obj_;};
  inline const SgVlbiStationInfo* stn() const {return (SgVlbiStationInfo*)obj_;};
  friend class NsScrPrx4Session;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Baseline : public NsScrPrx4Object
{
//friend class NsScrPrx4Session;
  friend class NsScrPrx4Band;
  Q_OBJECT
  Q_PROPERTY(double length
    READ getLength)
  // Attributes:
  Q_PROPERTY(bool isValid
    READ isValid WRITE setIsValid)
  Q_PROPERTY(bool estimateClocks
    READ getEstimateClocks WRITE setEstimateClocks)
  Q_PROPERTY(const QList<NsScrPrx4VlbiObservation*>& observations
    READ getObservations)
  // Clock offset estimation:
  Q_PROPERTY(double clockOffset
    READ getClockOffset)
  Q_PROPERTY(double clockOffsetStdDev
    READ getClockOffsetStdDev)

public:
  inline NsScrPrx4Baseline(SgVlbiBaselineInfo& bln, QObject *parent=0) :
    NsScrPrx4Object(bln, parent), 
    prxObservations_()
    {setUpName();};
  inline ~NsScrPrx4Baseline() {prxObservations_.clear();};

public slots:
  inline double getLength() const {return bln()->getLength();};
  inline bool isValid() const {return !bln()->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID);};
  inline bool getEstimateClocks() const
    {return bln()->isAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS);};
  inline const QList<NsScrPrx4VlbiObservation*>& getObservations() {return prxObservations_;};
  inline double getClockOffset() const {return bln()->dClock();};
  inline double getClockOffsetStdDev() const {return bln()->dClockSigma();};
  inline void setIsValid(bool is) {bln()->assignAttr(SgVlbiBaselineInfo::Attr_NOT_VALID, !is);};
  inline void setEstimateClocks(bool is)
    {bln()->assignAttr(SgVlbiBaselineInfo::Attr_ESTIMATE_CLOCKS, is);};

private:
  QList<NsScrPrx4VlbiObservation*>
                                prxObservations_;
  //
  inline SgVlbiBaselineInfo* bln() {return (SgVlbiBaselineInfo*)obj_;};
  inline const SgVlbiBaselineInfo* bln() const {return (SgVlbiBaselineInfo*)obj_;};
  inline void setUpName()
    {name_=obj_->getKey().left(8).simplified() + ":" + obj_->getKey().right(8).simplified();};
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Source : public NsScrPrx4Object
{
  Q_OBJECT

  Q_PROPERTY(double rightAscension
    READ getRa WRITE setRa)
  Q_PROPERTY(double declination
    READ getDn WRITE setDn)
  //
  Q_PROPERTY(QString aprioriComments
    READ getAprioriComments)
  //
  // Attributes:
  Q_PROPERTY(bool isValid
    READ isValid WRITE setIsValid)
  Q_PROPERTY(bool estimateCoords
    READ getEstimateCoords WRITE setEstimateCoords)
  Q_PROPERTY(bool constrainCoords
    READ getConstrainCoords WRITE setConstrainCoords)
//
  Q_PROPERTY(bool applySsm
    READ getApplySsm WRITE setApplySsm)
  Q_PROPERTY(bool testAttr
    READ getTestAttr WRITE setTestAttr)
    
public:
  inline NsScrPrx4Source(SgVlbiSourceInfo& src, QObject *parent=0) : NsScrPrx4Object(src, parent) {};
  inline ~NsScrPrx4Source() {};

public slots:
  inline double getRa() const {return src()->getRA();};
  inline double getDn() const {return src()->getDN();};
  //
  inline bool isValid() const {return !src()->isAttr(SgVlbiSourceInfo::Attr_NOT_VALID);};
  inline bool getEstimateCoords() const {return src()->isAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO);};
  inline bool getConstrainCoords() const {return src()->isAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO);};
  inline bool getApplySsm() const {return src()->isAttr(SgVlbiSourceInfo::Attr_APPLY_SSM);};
  inline bool getTestAttr() const {return src()->isAttr(SgVlbiSourceInfo::Attr_TEST);};

  inline const QString& getAprioriComments() const {return src()->getAprioriComments();};

  inline void setRa(double d)  {src()->setRA(d);};
  inline void setDn(double d)  {src()->setDN(d);};
  //
  //
  inline void setIsValid(bool is) {src()->assignAttr(SgVlbiSourceInfo::Attr_NOT_VALID, !is);};
  inline void setEstimateCoords(bool is) {src()->assignAttr(SgVlbiSourceInfo::Attr_ESTIMATE_COO, is);};
  inline void setConstrainCoords(bool is) {src()->assignAttr(SgVlbiSourceInfo::Attr_CONSTRAIN_COO, is);};
  inline void setTestAttr(bool is) {src()->assignAttr(SgVlbiSourceInfo::Attr_TEST, is);};
  inline void setApplySsm(bool is) {src()->assignAttr(SgVlbiSourceInfo::Attr_APPLY_SSM, is);};

  //
  inline int numOfSrcStructPoints() {return src()->sModel().size();};
  //
  inline void addSrcStructPoint(double k, double b, double x, double y, 
    bool estK=false, bool estB=false, bool estR=false)
    {src()->addSrcStructPoint(k, b, x, y, estK, estB, estR);};
  //
  inline void clearSrcStructPoints() {src()->clearSrcStructPoints();};
  //
  inline void setK_i(int i, double d)  {if (0<=i && i<src()->sModel().size()) src()->sModel()[i].setK(d);};
  inline void setB_i(int i, double d)  {if (0<=i && i<src()->sModel().size()) src()->sModel()[i].setB(d);};
  inline void setX_i(int i, double d)  {if (0<=i && i<src()->sModel().size()) src()->sModel()[i].setX(d);};
  inline void setY_i(int i, double d)  {if (0<=i && i<src()->sModel().size()) src()->sModel()[i].setY(d);};

  inline double getK_i(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getK():0.0;};
  inline double getB_i(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getB():0.0;};
  inline double getX_i(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getX():0.0;};
  inline double getY_i(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getY():0.0;};
  inline double getK_iSig(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getKsig():0.0;};
  inline double getB_iSig(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getBsig():0.0;};
  inline double getX_iSig(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getXsig():0.0;};
  inline double getY_iSig(int i) const 
    {return 0<=i && i<src()->sModel().size()?src()->sModel().at(i).getYsig():0.0;};


private:
  inline SgVlbiSourceInfo* src() {return (SgVlbiSourceInfo*)obj_;};
  inline const SgVlbiSourceInfo* src() const {return (SgVlbiSourceInfo*)obj_;};
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Session : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString name 
    READ getName)
  Q_PROPERTY(bool isOk
    READ getIsOk)
  Q_PROPERTY(QString networkSuffix
    READ getNetworkSuffix)
  Q_PROPERTY(QString sessionCode
    READ getSessionCode)
  Q_PROPERTY(QString networkID
    READ getNetworkID)
  Q_PROPERTY(QString description
    READ getDescription)
  Q_PROPERTY(QString userFlag
    READ getUserFlag)
  Q_PROPERTY(QString officialName
    READ getOfficialName)
  Q_PROPERTY(QString correlatorName
    READ getCorrelatorName)
  Q_PROPERTY(QString correlatorType
    READ getCorrelatorType)
  Q_PROPERTY(QString submitterName
    READ getSubmitterName)
  Q_PROPERTY(QString schedulerName
    READ getSchedulerName)
  Q_PROPERTY(QString piAgencyName
    READ getPiAgencyName)
  Q_PROPERTY(int numOfBands
    READ getNumOfBands)
  Q_PROPERTY(int numOfStations
    READ getNumOfStations)
  Q_PROPERTY(int numOfBaselines
    READ getNumOfBaselines)
  Q_PROPERTY(int numOfSources
    READ getNumOfSources)
  Q_PROPERTY(int numOfObservations
    READ getNumOfObservations)
  Q_PROPERTY(QDateTime tCreation
    READ getTCreation)
  Q_PROPERTY(QDateTime tStart
    READ getTStart)
  Q_PROPERTY(QDateTime tFinis
    READ getTFinis)
  Q_PROPERTY(QDateTime tMean
    READ getTMean)
  Q_PROPERTY(QDateTime tLastProcessed
    READ getLastProcessed)

  Q_PROPERTY(int primaryBandIdx
    READ getPrimaryBandIdx)
  Q_PROPERTY(const QList<NsScrPrx4Band*>& bands
    READ getBands)
  Q_PROPERTY(const QList<NsScrPrx4Station*>& stations
    READ getStations)
  Q_PROPERTY(const QList<NsScrPrx4Baseline*>& baselines
    READ getBaselines)
  Q_PROPERTY(const QList<NsScrPrx4Source*>& sources
    READ getSources)
  Q_PROPERTY(const QList<NsScrPrx4VlbiObservation*>& observations
    READ getObservations)

  Q_PROPERTY(bool hasReferenceClocksStation
    READ hasReferenceClocksStation)
  Q_PROPERTY(bool hasReferenceCoordinatesStation
    READ hasReferenceCoordinatesStation)

  Q_PROPERTY(double dUt1Value
    READ dUt1Value)
  Q_PROPERTY(double dUt1Correction
    READ dUt1Correction)
  Q_PROPERTY(double dUt1StdDev
    READ dUt1StdDev)

  Q_PROPERTY(QString recordMode
    READ getRecordingMode)

  Q_PROPERTY(QString sessionType
    READ getSessionType)




public:
  inline NsScrPrx4Session(SgVlbiSession& session, QObject *parent=0)
    : QObject(parent), prxBands_(), prxStations_(), prxBaselines_(), prxSources_(),
      prxObservations_(), stationsByKey_(), baselinesByKey_(), sourcesByKey_()
      {session_=&session; primaryBandIdx_=-1;};

  inline ~NsScrPrx4Session() 
    {session_=NULL;
      prxBands_.clear(); prxStations_.clear(); prxBaselines_.clear(), prxSources_.clear();
      prxObservations_.clear(); stationsByKey_.clear(); baselinesByKey_.clear(); sourcesByKey_.clear();};

  inline void setIsOk(bool is) {isOk_=is;};
  void postLoad();
  


public slots:
  inline const QString& getName() const {return session_->getName();};
  inline const QString& getNetworkSuffix() const {return session_->getNetworkSuffix();};
  inline const QString& getSessionCode() const {return session_->getSessionCode();};
  inline const QString& getNetworkID() const {return session_->getNetworkID();};
  inline const QString& getDescription() const {return session_->getDescription();};
  inline const QString& getUserFlag() const {return session_->getUserFlag();};
  inline const QString& getOfficialName() const {return session_->getOfficialName();};
  inline const QString& getCorrelatorName() const {return session_->getCorrelatorName();};
  inline const QString& getCorrelatorType() const {return session_->getCorrelatorType();};
  inline const QString& getSubmitterName() const {return session_->getSubmitterName();};
  inline const QString& getSchedulerName() const {return session_->getSchedulerName();};
  inline const QString& getPiAgencyName() const {return session_->getPiAgencyName();};
  inline int getNumOfBands() const {return session_->bandByKey().size();};
  inline int getNumOfStations() const {return session_->stationsByName().size();};
  inline int getNumOfBaselines() const {return session_->baselinesByName().size();};
  inline int getNumOfSources() const {return session_->sourcesByName().size();};
  inline int getNumOfObservations() const {return session_->observations().size();};
  inline QDateTime getTCreation() const {return session_->getTCreation().toQDateTime();};
  inline QDateTime getTStart() const {return session_->getTStart().toQDateTime();};
  inline QDateTime getTFinis() const {return session_->getTFinis().toQDateTime();};
  inline QDateTime getTMean() const {return session_->getTMean().toQDateTime();};
  inline QDateTime getLastProcessed() const {return session_->getLastProcessed().toQDateTime();};

  inline const QString& getRecordingMode() const {return session_->getRecordingMode();};
  inline const QString& getSessionType() const {return session_->getSessionType();};

  inline bool getIsOk() const {return isOk_;};
  //
  inline int getPrimaryBandIdx() const {return primaryBandIdx_;};
  inline const QList<NsScrPrx4Band*>& getBands() {return prxBands_;};
  inline const QList<NsScrPrx4Station*>& getStations() {return prxStations_;};
  inline const QList<NsScrPrx4Baseline*>& getBaselines() {return prxBaselines_;};
  inline const QList<NsScrPrx4Source*>& getSources() {return prxSources_;};
  inline const QList<NsScrPrx4VlbiObservation*>& getObservations() {return prxObservations_;};
  

  inline double dUt1Value() const {return session_->dUt1Value();};
  inline double dUt1Correction() const {return session_->dUt1Correction();};
  inline double dUt1StdDev() const {return session_->dUt1StdDev();};

  inline QString recordModeByBand(const QString& bandKey) const 
    {
      if (session_->bandByKey().contains(bandKey)) 
        return session_->bandByKey().value(bandKey)->recordingMode();
      else 
        return QString("");
    };

  inline NsScrPrx4Station* lookUpStation(const QString& key)
    {return stationsByKey_.contains(key)?stationsByKey_.value(key):NULL;};
  inline NsScrPrx4Baseline* lookUpBaseline(const QString& key)
    {return baselinesByKey_.contains(key)?baselinesByKey_.value(key):NULL;};
  inline NsScrPrx4Source* lookUpSource(const QString& key)
    {return sourcesByKey_.contains(key)?sourcesByKey_.value(key):NULL;};


  void dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode);
  void dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode);

  //
  // functionality:
  //
  //
  inline bool hasReferenceClocksStation() {return isOk_ && session_->hasReferenceClocksStation();};
  //
  inline bool hasReferenceCoordinatesStation()
    {return isOk_ && session_->hasReferenceCoordinatesStation();};
  //
  //
  inline void resetAllEditings() {if (isOk_) session_->resetAllEditings(false);};
  //
  inline void clearAuxSigmas() {if (isOk_) session_->zerofySigma2add();};
  //
  inline void suppressNotSoGoodObs() {if (isOk_) session_->suppressNotSoGoodObs();};
  //
  //
  inline void pickupReferenceClocksStation() {if (isOk_) session_->pickupReferenceClocksStation();};
  //
  inline void pickupReferenceCoordinatesStation()
    {if (isOk_) session_->pickupReferenceCoordinatesStation();};
  //
  inline void setNumOfClockPolynoms4Stations(int n) {if (isOk_) session_->setClockModelOrder4Stations(n);};
  //
  inline void setReferenceClocksStation(const QString& stnKey)
    {if (isOk_) session_->setReferenceClocksStation(stnKey);};
  //
  inline void checkUseOfManualPhaseCals() {if (isOk_) session_->checkUseOfManualPhaseCals();};


  //
  inline void process() {if (isOk_) session_->process(true, false);};
  //
  inline void calcIono(bool sbdOnly=false) 
    {if (isOk_) session_->calculateIonoCorrections(sbdOnly?NULL:session_->getConfig());};
  //
  inline void zeroIono() {if (isOk_) session_->zerofyIonoCorrections(session_->getConfig());};

  //
  inline int checkClockBreaks(int bandIdx)
    {if (isOk_) return session_->checkBandForClockBreaks_part1(bandIdx, true); else return 0;};
  //
  inline int eliminateOutliers(int bandIdx) {return isOk_?session_->eliminateOutliers(bandIdx):0;};
  //
  inline int restoreOutliers(int bandIdx) {return isOk_?session_->restoreOutliers(bandIdx):0;};
  //
  inline int doReWeighting() {return isOk_?session_->doReWeighting():0;};

  //
  inline void scanAmbiguityMultipliers(int bandIdx)
    {if (isOk_) session_->scanBaselines4GrDelayAmbiguities(bandIdx);};

  inline void eliminateOutliersSimpleMode(int bandIdx, int maxNumOfPasses, 
    double threshold, double upperLimit)
    {if (isOk_) session_->eliminateOutliersSimpleMode(bandIdx, maxNumOfPasses, threshold, upperLimit);};
  
  inline void eliminateLargeOutliers(int bandIdx, int maxNumOfPasses, double wrmsRatio)
    {if (isOk_) session_->eliminateLargeOutliers(bandIdx, maxNumOfPasses, wrmsRatio);};

  inline bool writeUserData2File(const QString& fileName)
    {if (isOk_) return session_->writeUserData2File(fileName); else return false;};
  
  // just a shortcut:
  inline void doStdSetup()
    {suppressNotSoGoodObs(); pickupReferenceClocksStation();
      pickupReferenceCoordinatesStation(); checkUseOfManualPhaseCals();};
      
  inline void propagateChannelBandwidth(double bw)
    {session_->propagateChannelBandwidth(bw);};

  inline void addHistoryRecord(const QString&);


private:
  bool                          isOk_;
  SgVlbiSession                *session_;
  int                           primaryBandIdx_;
  QList<NsScrPrx4Band*>         prxBands_;
  QList<NsScrPrx4Station*>      prxStations_;
  QList<NsScrPrx4Baseline*>     prxBaselines_;
  QList<NsScrPrx4Source*>       prxSources_;
  QList<NsScrPrx4VlbiObservation*>
                                prxObservations_;
  QMap<QString, NsScrPrx4Station*>
                                stationsByKey_;
  QMap<QString, NsScrPrx4Baseline*>
                                baselinesByKey_;
  QMap<QString, NsScrPrx4Source*>
                                sourcesByKey_;
  
};
/*=====================================================================================================*/





#if 0 < HAVE_SCRIPTS

Q_DECLARE_METATYPE(NsScrPrx4Band*);
Q_DECLARE_METATYPE(QList<NsScrPrx4Band*>);

Q_DECLARE_METATYPE(NsScrPrx4Station*);
Q_DECLARE_METATYPE(QList<NsScrPrx4Station*>);

Q_DECLARE_METATYPE(NsScrPrx4Baseline*);
Q_DECLARE_METATYPE(QList<NsScrPrx4Baseline*>);

Q_DECLARE_METATYPE(NsScrPrx4Source*);
Q_DECLARE_METATYPE(QList<NsScrPrx4Source*>);

#endif















/*=====================================================================================================*/
//
// aux functions:
//

#if 0 < HAVE_SCRIPTS


// band:
inline QScriptValue toScriptValue4Band(QScriptEngine *eng, NsScrPrx4Band* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4Band(const QScriptValue &obj, NsScrPrx4Band* &out)
{
  out = qobject_cast<NsScrPrx4Band*>(obj.toQObject());
};



// station:
inline QScriptValue toScriptValue4Stn(QScriptEngine *eng, NsScrPrx4Station* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4Stn(const QScriptValue &obj, NsScrPrx4Station* &out)
{
  out = qobject_cast<NsScrPrx4Station*>(obj.toQObject());
};



// baseline:
inline QScriptValue toScriptValue4Bln(QScriptEngine *eng, NsScrPrx4Baseline* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4Bln(const QScriptValue &obj, NsScrPrx4Baseline* &out)
{
  out = qobject_cast<NsScrPrx4Baseline*>(obj.toQObject());
};




// source:
inline QScriptValue toScriptValue4Src(QScriptEngine *eng, NsScrPrx4Source* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4Src(const QScriptValue &obj, NsScrPrx4Source* &out)
{
  out = qobject_cast<NsScrPrx4Source*>(obj.toQObject());
};
#endif  // SCRIPT_SUPPORT


//
inline void NsScrPrx4Session::addHistoryRecord(const QString& rec)
{
  session_->contemporaryHistory().addHistoryRecord(rec, SgMJD::currentMJD().toUtc());
};




/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_SESSION_H
