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

#ifndef NS_SCR_PRX_4_OBSERVATION_H
#define NS_SCR_PRX_4_OBSERVATION_H


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
#include <SgVlbiAuxObservation.h>
#include <SgVlbiBand.h>
#include <SgVlbiBaselineInfo.h>
#include <SgVlbiObservation.h>
#include <SgVlbiObservation.h>
#include <SgVlbiSession.h>
#include <SgVlbiSourceInfo.h>
#include <SgVlbiStationInfo.h>



#include "NsScrPrx4TaskConfig.h"


class NsScrPrx4Baseline;
class NsScrPrx4Session;
class NsScrPrx4Source;
class NsScrPrx4Station;
/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4VlbiObservation : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString key
    READ getKey)
  Q_PROPERTY(QString scanName
    READ getScanName)
  Q_PROPERTY(QDateTime epoch
    READ getEpoch)
  Q_PROPERTY(QString epochStr
    READ getEpochStr)
  Q_PROPERTY(double gmst
    READ getGmst)
  Q_PROPERTY(bool isValid
    READ isValid WRITE setIsValid)
  Q_PROPERTY(bool isProcessed
    READ isProcessed /*WRITE setIsProcessed*/)
  Q_PROPERTY(int numOfBands
    READ getNumOfBands)

  Q_PROPERTY(NsScrPrx4Station* station_1
    READ getStation_1)
  Q_PROPERTY(NsScrPrx4Station* station_2
    READ getStation_2)
  Q_PROPERTY(NsScrPrx4Baseline* baseline
    READ getBaseline)
  Q_PROPERTY(NsScrPrx4Source* source
    READ getSource)


public:
  inline NsScrPrx4VlbiObservation(SgVlbiObservation& obs, NsScrPrx4Session* session, QObject *parent=0)
    : QObject(parent) {obs_=&obs; session_=session;};
  inline ~NsScrPrx4VlbiObservation() {obs_=NULL; session_=NULL;};

public slots:
  inline const QString& getKey() const {return obs_->getKey();};
  inline QString getScanName() const {return obs_->getScanName();};
  inline QDateTime getEpoch() const {return obs_->toQDateTime();};
  inline QString getEpochStr() const {return obs_->toString(SgMJD::F_YYYYMMDDHHMMSSSS);};
  inline double getGmst() const {return obs_->gmst();};
  inline bool isValid() const {return !obs_->isAttr(SgVlbiObservation::Attr_NOT_VALID);};
  inline bool isProcessed() const {return obs_->isAttr(SgVlbiObservation::Attr_PROCESSED);};
  inline int getNumOfBands() const {return obs_->observableByKey().size();};

  NsScrPrx4Station* getStation_1();
  NsScrPrx4Station* getStation_2();
  NsScrPrx4Baseline* getBaseline();
  NsScrPrx4Source* getSource();


  inline void setIsValid(bool is) {obs_->assignAttr(SgVlbiObservation::Attr_NOT_VALID, !is);};
//inline void setIsProcessed(bool is) {obs_->assignAttr(SgVlbiObservation::Attr_PROCESSED, is);};

  inline int qualityFactor(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->getQualityFactor():-1;};
  inline double correlationCoeff(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->getCorrCoeff():-1.0;};
  inline double snr(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->getSnr():-1.0;};
  inline int numOfChannels(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->getNumOfChannels():-1;};
  inline bool isUsable(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->isUsable():false;};

  double delayValue(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delayValueGeoc(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delayStdDev(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delayResidual(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delayResidualNorm(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delayAmbiguitySpacing(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  int delayNumOfAmbiguities(const QString& bandKey, NsScrPrx4TaskConfig::VlbiDelayType t) const;
  double delaySourceStructure(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->getTauS():0.0;};

  double rateValue(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->phDRate().getValue():0.0;};
  double rateStdDev(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->phDRate().getSigma():-1.0;};
  double rateResidual(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->phDRate().getResidual():0.0;};
  double rateResidualNorm(const QString& bandKey) const
    {return obs_->observableByKey().contains(bandKey)?
      obs_->observableByKey().value(bandKey)->phDRate().getResidualNorm():0.0;};





protected:
  SgVlbiObservation            *obs_;
  NsScrPrx4Session             *session_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4VlbiAuxObservation : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QDateTime epoch
    READ getEpoch)
  Q_PROPERTY(bool isValid
    READ isValid)
  Q_PROPERTY(double cableCalibration
    READ getCableCalibration)
  Q_PROPERTY(double atmPressure
    READ getAtmPressure)
  Q_PROPERTY(double atmTemperature
    READ getAtmTemperature)
  Q_PROPERTY(double atmHumidity
    READ getAtmHumidity)

  Q_PROPERTY(NsScrPrx4Station* station
    READ getStation)


public:
  inline NsScrPrx4VlbiAuxObservation(SgVlbiAuxObservation& aux, NsScrPrx4Station* stn, QObject *parent=0)
    : QObject(parent) {aux_=&aux; station_=stn;};
  inline ~NsScrPrx4VlbiAuxObservation() {aux_=NULL; station_=NULL;};

public slots:
  inline QDateTime getEpoch() const {return aux_->toQDateTime();};
  inline bool isValid() const {return !aux_->isAttr(SgVlbiObservation::Attr_NOT_VALID);};
  inline bool isProcessed() const {return aux_->isAttr(SgVlbiObservation::Attr_PROCESSED);};
  inline double getCableCalibration() const  {return aux_->getCableCalibration();};
  inline double getAtmPressure() const  {return aux_->getMeteoData().getPressure();};
  inline double getAtmTemperature() const  {return aux_->getMeteoData().getTemperature();};
  inline double getAtmHumidity() const  {return aux_->getMeteoData().getRelativeHumidity();};
  inline NsScrPrx4Station* getStation() {return station_;};

protected:
  SgVlbiAuxObservation          *aux_;
  NsScrPrx4Station             *station_;
};
/*=====================================================================================================*/








#if 0 < HAVE_SCRIPTS

Q_DECLARE_METATYPE(NsScrPrx4VlbiObservation*);
Q_DECLARE_METATYPE(QList<NsScrPrx4VlbiObservation*>);

Q_DECLARE_METATYPE(NsScrPrx4VlbiAuxObservation*);
Q_DECLARE_METATYPE(QList<NsScrPrx4VlbiAuxObservation*>);

#endif











/*=====================================================================================================*/
//
// aux functions:
//
#if 0 < HAVE_SCRIPTS



// observation:
inline QScriptValue toScriptValue4VlbiObs(QScriptEngine *eng, NsScrPrx4VlbiObservation* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4VlbiObs(const QScriptValue &obj, NsScrPrx4VlbiObservation* &out)
{
  out = qobject_cast<NsScrPrx4VlbiObservation*>(obj.toQObject());
};



// auxiliary data:
inline QScriptValue toScriptValue4VlbiAuxObs(QScriptEngine *eng, NsScrPrx4VlbiAuxObservation* const &in)
{
  return eng->newQObject(in);
};
//
inline void fromScriptValue4VlbiAuxObs(const QScriptValue &obj, NsScrPrx4VlbiAuxObservation* &out)
{
  out = qobject_cast<NsScrPrx4VlbiAuxObservation*>(obj.toQObject());
};


#endif  // SCRIPT_SUPPORT


/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_OBSERVATION_H

