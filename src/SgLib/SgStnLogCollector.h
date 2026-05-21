/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
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

#ifndef SG_STN_LOG_COLLECTOR_H
#define SG_STN_LOG_COLLECTOR_H

#include <map> 


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif




#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>
#include <QtCore/QVector>


#include <SgAttribute.h>
#include <SgLogger.h>
#include <SgMJD.h>
#include <SgMeteoData.h>
#include <SgVex.h>


#ifndef SEPARATED_LOG2ANT
#     include <SgVector.h>
#endif


#ifndef SEPARATED_LOG2ANT
class SgVlbiAuxObservation;
class SgVlbiStationInfo;
#endif

class SgOnSourceRecord;

enum AntcalOutputData
{
  AOD_NONE            = 0,
  AOD_DATA_ON         = 1 << 0,
  AOD_CABLE_SIGN      = 1 << 1,
  AOD_CABLE_CAL       = 1 << 2,
  AOD_METEO           = 1 << 3,
  AOD_TSYS            = 1 << 4,
  AOD_TPI             = 1 << 5,
  AOD_PCAL            = 1 << 6,
  AOD_FMTGPS          = 1 << 7,
  AOD_DBBC3TP         = 1 << 8,
  AOD_SEFD            = 1 << 9,
  AOD_TRADITIONAL     = AOD_DATA_ON | AOD_CABLE_SIGN | AOD_CABLE_CAL | AOD_METEO,
  AOD_NOVEL           = AOD_TSYS | AOD_TPI | AOD_PCAL | AOD_FMTGPS | AOD_DBBC3TP | AOD_SEFD,
  AOD_ALL             = AOD_TRADITIONAL | AOD_NOVEL
};




/***===================================================================================================*/
/**
 *  SgCableCalReading:
 *
 */
/**====================================================================================================*/
class SgCableCalReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgCableCalReading() : t_(tZero) {v_=0.0; isOk_=false; osRec_=NULL;};
  inline SgCableCalReading(const SgMJD& t, double v) : t_(t) {v_=v; isOk_=true; osRec_=NULL;};
  inline SgCableCalReading(const SgCableCalReading& cr) : t_(cr.getT()) 
    {setV(cr.getV()); setIsOk(cr.getIsOk()); osRec_ = cr.osRec_;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgCableCalReading() {osRec_=NULL;};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline double getV() const {return v_;};
  inline bool getIsOk() const {return isOk_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setV(double v) {v_=v;};
  inline void setIsOk(bool isOk) {isOk_=isOk;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  
  //
  // Functions:
  //
  inline bool operator==(const SgCableCalReading& cab) const
  {
    return 
      cab.t_ == t_ &&
      cab.v_ == v_ &&
      cab.isOk_ == isOk_ &&
      cab.osRec_ == osRec_; // point to the same osRec
  };

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  double                        v_;
  bool                          isOk_;
  SgOnSourceRecord             *osRec_;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 *  SgMeteoReading:
 *
 */
/**====================================================================================================*/
class SgMeteoReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgMeteoReading() : t_(tZero), m_() {isOk_=false; osRec_=NULL;};
  inline SgMeteoReading(const SgMJD& t, const SgMeteoData& m) : t_(t), m_(m) {isOk_=true; osRec_=NULL;};
  inline SgMeteoReading(const SgMeteoReading& mr)
    : t_(mr.getT()), m_(mr.getM()) {setIsOk(mr.getIsOk()); osRec_=mr.osRec_;};
  inline SgMeteoReading(const SgMJD& t, double temp, double pres, double rho) 
    : t_(t), m_(temp, pres, rho) {isOk_=true; osRec_=NULL;};


  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgMeteoReading() {osRec_=NULL;};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline const SgMeteoData& getM() const {return m_;};
  inline bool getIsOk() const {return isOk_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setM(const SgMeteoData& m) {m_=m;};
  inline void setIsOk(bool isOk) {isOk_=isOk;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  
  //
  // Functions:
  //
  inline bool operator==(const SgMeteoReading& met) const
  {
    return 
      met.t_ == t_ &&
      met.m_ == m_ &&
      met.isOk_ == isOk_;
  };

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  SgMeteoData                   m_;
  bool                          isOk_;
  SgOnSourceRecord             *osRec_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgPointingReading:
 *
 */
/**====================================================================================================*/
class SgTraklReading : public SgAttribute
{
public:

  enum Attributes
  {
    Attr_FROM_SOURCE          = 1<<0, //!< 
  };

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgTraklReading() : t_(tZero) {az_=azv_=el_=elv_=0.0;};
  inline SgTraklReading(const SgMJD& t, float az, float azv, float el, float elv)
    : t_(t) {az_=az; azv_=azv; el_=el; elv_=elv;};
  inline SgTraklReading(const SgTraklReading& r) : SgAttribute(r), t_(r.getT()) 
      {setAz(r.getAz()); setAzv(r.getAzv()); setEl(r.getEl()); setElv(r.getElv());};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgTraklReading() {};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline float getAz() const {return az_;};
  inline float getAzv() const {return azv_;};
  inline float getEl() const {return el_;};
  inline float getElv() const {return elv_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setAz(float az) {az_=az;};
  inline void setAzv(float azv) {azv_=azv;};
  inline void setEl(float el) {el_=el;};
  inline void setElv(float elv) {elv_=elv;};
  
  //
  // Functions:
  //
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  float                         az_;
  float                         azv_;
  float                         el_;
  float                         elv_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgTsysReading:
 *
 */
/**====================================================================================================*/
class SgTsysReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgTsysReading() : t_(tZero), tsys_(), tpcont_(), trakls_() 
    {isOk_=isOnSource_=isDataOn_=false; osRec_=NULL;};
//  inline SgTsysReading(const SgMJD& t, const QMap<QString, float>& tsys, 
//    QMap<QString, QVector<int> > tpcont, bool isDataOn)
//    : t_(t), tsys_(tsys), tpcont_(tpcont), trakls_() {isOk_=true; isDataOn_=isDataOn; osRec_=NULL;};
  inline SgTsysReading(const SgTsysReading& tsr) 
    : t_(tsr.getT()), tsys_(), tpcont_(), trakls_()
    {addTsyses(tsr.getTsys()); addTpconts(tsr.getTpcont());
      for (int i=0; i<tsr.getTrakls().size(); i++)
        trakls_.append(tsr.getTrakls().at(i));
      setIsOk(tsr.getIsOk()); setIsDataOn(tsr.getIsDataOn()); setIsOnSource(tsr.getIsOnSource()); osRec_=tsr.osRec_;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgTsysReading() {osRec_=NULL; trakls_.clear(); tsys_.clear(); tpcont_.clear();};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
/*
  inline const QMap<const QString*, float>& getTsys() const {return tsys_;};
  inline const QMap<const QString*, QVector<int> >& getTpcont() const {return tpcont_;};
  inline QMap<const QString*, float>& tsys() {return tsys_;};
  inline QMap<const QString*, QVector<int> >& tpcont() {return tpcont_;};
*/
  inline const QMap<QString, float>& getTsys() const {return tsys_;};
  inline const QMap<QString, QVector<int> >& getTpcont() const {return tpcont_;};
  inline QMap<QString, float>& tsys() {return tsys_;};
  inline QMap<QString, QVector<int> >& tpcont() {return tpcont_;};
  inline bool getIsOk() const {return isOk_;};
  inline bool getIsDataOn() const {return isDataOn_;};
  inline bool getIsOnSource() const {return isOnSource_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};
  inline const QList<SgTraklReading*>& getTrakls() const {return trakls_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setIsOk(bool isOk) {isOk_=isOk;};
  inline void setIsDataOn(bool isDataOn) {isDataOn_=isDataOn;};
  inline void setIsOnSource(bool isOnSource) {isOnSource_=isOnSource;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  inline SgOnSourceRecord* &osRec() {return osRec_;};
  inline QList<SgTraklReading*>& trakls() {return trakls_;};
  
  //
  // Functions:
  //
/*
  void addTsyses(const QMap<const QString*, float>& tsys);
  void addTpconts(const QMap<const QString*, QVector<int> >& tpcont);
*/
  void addTsyses(const QMap<QString, float>& tsys);
  void addTpconts(const QMap<QString, QVector<int> >& tpcont);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
/*
  QMap<const QString*, float>   tsys_;
  QMap<const QString*, QVector<int> > 
                                tpcont_;
*/
  QMap<QString, float>          tsys_;
  QMap<QString, QVector<int> >  tpcont_;

//std::map<std::string, int*>   tpcont2_;
  bool                          isOk_;
  bool                          isDataOn_;
  bool                          isOnSource_;
  SgOnSourceRecord             *osRec_;
  QList<SgTraklReading*>        trakls_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgDbbc3TpReading:
 *
 */
/**====================================================================================================*/
class SgDbbc3TpReading
{
public:
  
  class TpRecord
  {
  public:
    inline TpRecord() : sensorId_(""), boardId_(""), agc_("") 
      {bw_=gainU_=gainL_=tpOffU_=tpOffL_=tpOnU_=tpOnL_=sefdU_=sefdL_=0.0;};
    inline TpRecord(const TpRecord& tpr) : 
      sensorId_(tpr.sensorId_), boardId_(tpr.boardId_), agc_(tpr.agc_) 
      {bw_=tpr.bw_; gainU_=tpr.gainU_; gainL_=tpr.gainL_; tpOffU_=tpr.tpOffU_;
        tpOffL_=tpr.tpOffL_; tpOnU_=tpr.tpOnU_; tpOnL_=tpr.tpOnL_;
        sefdU_=tpr.sefdU_; sefdL_=tpr.sefdL_;};
    inline TpRecord& operator=(const TpRecord& tpr)
      {sensorId_=tpr.sensorId_; boardId_=tpr.boardId_; agc_=tpr.agc_; bw_=tpr.bw_; 
        gainU_=tpr.gainU_; gainL_=tpr.gainL_; tpOffU_=tpr.tpOffU_; tpOffL_=tpr.tpOffL_; 
        tpOnU_=tpr.tpOnU_; tpOnL_=tpr.tpOnL_; sefdU_=tpr.sefdU_; sefdL_=tpr.sefdL_; 
        return *this;};

    QString                     sensorId_;
    QString                     boardId_;
    double                      bw_;
    QString                     agc_;
    double                      gainU_;
    double                      gainL_;
    double                      tpOffU_;
    double                      tpOffL_;
    double                      tpOnU_;
    double                      tpOnL_;
    double                      sefdU_;
    double                      sefdL_;
  };

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgDbbc3TpReading() : t_(tZero), tpBySensor_()
    {isDataOn_=false; isOnSource_=false; osRec_=NULL;};
  inline SgDbbc3TpReading(const SgDbbc3TpReading& tps) 
    : t_(tps.getT()), tpBySensor_(tps.tpBySensor_)
    {setIsDataOn(tps.getIsDataOn()); setIsOnSource(tps.getIsOnSource()); osRec_=tps.osRec_;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgDbbc3TpReading() {osRec_=NULL; tpBySensor_.clear();};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline const QMap<QString, TpRecord>& getTpBySensor() const {return tpBySensor_;};
  inline QMap<QString, TpRecord>& tpBySensor() {return tpBySensor_;};
  inline bool getIsOnSource() const {return isOnSource_;};
  inline bool getIsDataOn() const {return isDataOn_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setIsOnSource(bool isOnSource) {isOnSource_=isOnSource;};
  inline void setIsDataOn(bool isDataOn) {isDataOn_=isDataOn;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  inline SgOnSourceRecord* &osRec() {return osRec_;};
  
  //
  // Functions:
  //
  inline void addRecord(const QString& sensorId, const QString& boardId, double bw,
    const QString& agc, double gainU, double gainL, double tpOffU, double tpOffL, 
    double tpOnU, double tpOnL, double sefdU, double sefdL)
  {
    TpRecord                    rec;
    rec.sensorId_ = sensorId;
    rec.boardId_  = boardId; 
    rec.bw_   = bw;
    rec.agc_  = agc;
    rec.gainU_ = gainU;
    rec.gainL_ = gainL;
    rec.tpOffU_ = tpOffU;
    rec.tpOffL_ = tpOffL;
    rec.tpOnU_ = tpOnU;
    rec.tpOnL_ = tpOnL;
    rec.sefdU_ = sefdU;
    rec.sefdL_ = sefdL;
    tpBySensor_.insert(sensorId, rec);
  };
 

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  QMap<QString, TpRecord>       tpBySensor_;
  bool                          isOnSource_;
  bool                          isDataOn_;
  SgOnSourceRecord             *osRec_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgPcalReading:
 *
 */
/**====================================================================================================*/
class SgPcalReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgPcalReading() : t_(tZero), pcal_() {isOk_=false; osRec_=NULL;};
  inline SgPcalReading(const SgPcalReading& pcr) : t_(pcr.getT()), pcal_()
    {addPcals(pcr.getPcal()); setIsOk(pcr.getIsOk()); osRec_=pcr.osRec_;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgPcalReading() 
  {
    for (QMap<QString, float*>::iterator it=pcal_.begin(); it!=pcal_.end(); ++it)
      delete[] it.value();
    pcal_.clear(); osRec_=NULL;
  };

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline const QMap<QString, float*>& getPcal() const {return pcal_;};
  inline QMap<QString, float*>& pcal() {return pcal_;};
  inline bool getIsOk() const {return isOk_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setIsOk(bool isOk) {isOk_=isOk;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  inline SgOnSourceRecord* &osRec() {return osRec_;};
  
  //
  // Functions:
  //
  inline bool addPcals(const QMap<QString, float*>& pcals)
  {
    bool                        isOk=true;
    for (QMap<QString, float*>::const_iterator it=pcals.begin(); it!=pcals.end(); ++it)
    {
      if (pcal_.contains(it.key()))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::RUN, className() +
          "::addPcals(): the key \"" + it.key() + "\" is already in the map");
      };
      float                    *f = new float[2];
      *f = *it.value();
      *(f + 1) = *(it.value() + 1);
      pcal_.insert(it.key(), f);
    };
    return isOk;
  };

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  QMap<QString, float*>         pcal_;
  bool                          isOk_;
  SgOnSourceRecord             *osRec_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgSefdReading:
 *
 */
/**====================================================================================================*/
class SgSefdReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgSefdReading() : t_(tZero), srcName_(""), valByChanKey_() {isOk_=false; az_=el_=-10000.0;};

  inline SgSefdReading(const SgSefdReading& sr) : t_(sr.getT()), srcName_(sr.getSrcName()),
    az_(sr.getAz()), el_(sr.getEl())
    {
      for (QMap<QString, QVector<double>*>::const_iterator it=sr.getValByChanKey().begin(); 
        it!=sr.getValByChanKey().end(); ++it)
        valByChanKey_.insert(it.key(), new QVector<double>(*it.value()));
      setIsOk(sr.getIsOk());
    };

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgSefdReading() 
  {
    for (QMap<QString, QVector<double>*>::iterator it=valByChanKey_.begin(); it!=valByChanKey_.end(); 
      ++it)
      delete[] it.value();
    valByChanKey_.clear();
  };

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline const QString& getSrcName() const {return srcName_;};
  inline double getAz() const {return az_;};
  inline double getEl() const {return el_;};

  inline const QMap<QString, QVector<double>*>& getValByChanKey() const {return valByChanKey_;};
  inline QMap<QString, QVector<double>*>& valByChanKey() {return valByChanKey_;};
  inline bool getIsOk() const {return isOk_;};

  inline void setT(const SgMJD& t) {t_=t;};
  inline void setIsOk(bool isOk) {isOk_=isOk;};
  inline void setSrcName(const QString& name) {srcName_=name;};
  inline void setAz(double d) {az_=d;};
  inline void setEl(double d) {el_=d;};
  
  //
  // Functions:
  //
  bool addValByChanKey(const SgMJD& t, const QString& srcName, double az, double el,
    const QString& chanKey, const QVector<double>& val);

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  QString                       srcName_;
  double                        az_;
  double                        el_;
  QMap<QString, QVector<double>*>
                                valByChanKey_;
  bool                          isOk_;
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 *  SgDot2xpsReading:
 *
 */
/**====================================================================================================*/
class SgDot2xpsReading
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgDot2xpsReading() : t_(tZero), dot2gpsByBrd_(), dot2ppsByBrd_() {osRec_=NULL;};
  inline SgDot2xpsReading(const SgDot2xpsReading& d2r) : t_(d2r.getT()), 
    dot2gpsByBrd_(d2r.dot2gpsByBrd_), dot2ppsByBrd_(d2r.dot2ppsByBrd_) {osRec_=d2r.osRec_;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgDot2xpsReading() {dot2gpsByBrd_.clear(); dot2ppsByBrd_.clear(); osRec_=NULL;};

  //
  // Interfaces:
  //
  inline const SgMJD& getT() const {return t_;};
  inline const SgOnSourceRecord* getOsRec() const {return osRec_;};
  inline const QMap<QString, float>& getDot2gpsByBrd() const {return dot2gpsByBrd_;};
  inline const QMap<QString, float>& getDot2ppsByBrd() const {return dot2ppsByBrd_;};

  inline void setT(const SgMJD& t) {t_ = t;};
  inline void setOsRec(SgOnSourceRecord* os) {osRec_ = os;};
  inline SgOnSourceRecord* &osRec() {return osRec_;};

  inline bool addDot2gpsByBrd(const QString& key, float v)
    {bool isOk=!dot2gpsByBrd_.contains(key); dot2gpsByBrd_.insert(key, v); return isOk;};
  inline bool addDot2ppsByBrd(const QString& key, float v)
    {bool isOk=!dot2ppsByBrd_.contains(key); dot2ppsByBrd_.insert(key, v); return isOk;};

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  SgMJD                         t_;
  QMap<QString, float>          dot2gpsByBrd_;
  QMap<QString, float>          dot2ppsByBrd_;
  SgOnSourceRecord             *osRec_;
};
/*=====================================================================================================*/







/***===================================================================================================*/
/**
 *  SgOnSourceRecord:
 *
 */
/**====================================================================================================*/
class SgOnSourceRecord
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgOnSourceRecord()
    : tStart_(tZero), tFinis_(tZero), tDataOn_(tZero), scanName_(""), sourceName_(""), cableCals_(), 
      meteoPars_(), tSyses_(), dbbc3Tps_() {idx_=-1;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgOnSourceRecord() {cableCals_.clear(); meteoPars_.clear(); tSyses_.clear(); dbbc3Tps_.clear();};

  //
  // Interfaces:
  //
  inline const SgMJD& getTstart() const {return tStart_;};
  inline const SgMJD& getTfinis() const {return tFinis_;};
  inline const SgMJD& getTdataOn() const {return tDataOn_;};
  inline const QString& getScanName() const {return scanName_;};
  inline const QString& getSourceName() const {return sourceName_;};
  inline int getIdx() const {return idx_;};
  inline void setTstart(const SgMJD& t) {tStart_=t;};
  inline void setTfinis(const SgMJD& t) {tFinis_=t;};
  inline void setTdataOn(const SgMJD& t) {tDataOn_=t;};
  inline void setSourceName(const QString& str) {sourceName_=str;};
  inline void setScanName(const QString& str) {scanName_=str;};
  inline void setIdx(int i) {idx_=i;};

  inline QList<SgCableCalReading*>& cableCals() {return cableCals_;};
  inline QList<SgMeteoReading*>&  meteoPars() {return meteoPars_;};
  inline QList<SgTsysReading*>&  tSyses() {return tSyses_;};
  inline QList<SgDbbc3TpReading*>& dbbc3Tps() {return dbbc3Tps_;};

  inline SgMJD& tStart() {return tStart_;};
  inline SgMJD& tFinis() {return tFinis_;};
  inline SgMJD& tDataOn() {return tDataOn_;};
  
  //
  // Functions:
  //
  /** Compares the object with another one.
  */
  inline bool isSameArgs(const SgOnSourceRecord& rec) const
  {
    return 
      rec.tStart_ == tStart_ && 
      rec.tFinis_ == tFinis_ && 
      rec.tDataOn_ == tDataOn_ && 
      rec.scanName_ == scanName_ &&
      rec.sourceName_ == sourceName_;
  };

  inline bool operator==(const SgOnSourceRecord& rec) const
  {
    if (!isSameArgs(rec)) 
      return false;
    if (rec.cableCals_.size() != cableCals_.size())
      return false;
    if (rec.meteoPars_.size() != meteoPars_.size())
      return false;
    if (rec.tSyses_.size() != tSyses_.size())
      return false;
    if (rec.dbbc3Tps_.size() != dbbc3Tps_.size())
      return false;
    for (int i=0; i<cableCals_.size(); i++)
      if (rec.cableCals_.at(i) != cableCals_.at(i))
        return false;
    for (int i=0; i<meteoPars_.size(); i++)
      if (rec.meteoPars_.at(i) != meteoPars_.at(i))
        return false;
    for (int i=0; i<tSyses_.size(); i++)
      if (rec.tSyses_.at(i) != tSyses_.at(i))
        return false;
    for (int i=0; i<dbbc3Tps_.size(); i++)
      if (rec.dbbc3Tps_.at(i) != dbbc3Tps_.at(i))
        return false;
    return true;
  };

  /** Compares the object with another one.
   */
//  inline bool operator!=(const SgOnSourceRecord& rec) const {return !(*this==rec);};

  //
  // Friends:
  //
  
  //
  // I/O:
  //
  // ...
private:
  SgMJD                         tStart_;
  SgMJD                         tFinis_;
  SgMJD                         tDataOn_;
  QString                       scanName_;
  QString                       sourceName_;
  QList<SgCableCalReading*>     cableCals_;
  QList<SgMeteoReading*>        meteoPars_;
  QList<SgTsysReading*>         tSyses_;
  QList<SgDbbc3TpReading*>      dbbc3Tps_;
  int                           idx_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgChannelSetup:
 *
 */
/**====================================================================================================*/
class SgChannelSetup
{
public:
  enum HwType
  {
    HT_UNDEF                = 0,
    HT_VC                   = 1,
    HT_BBC                  = 2,
    HT_NN                   = 3,
    HT_VGOS                 = 4,
  };
  enum BackEndType
  {
    BET_UNDEF               = 0,
    BET_NONE                = 1,
    BET_DBBC                = 2,
    BET_DBBC3               = 3,
    BET_RDBE                = 4,
    BET_CDAS                = 5,
    BET_OTHER               = 6,
  };

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  static double calcSkyFreq(double loFreq, double bbcFreq, double width, 
    SgChannelSideBand loSideBand, SgChannelSideBand ifSideBand);

  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgChannelSetup() : bbcFreqByIdx_(), bbcBandwidthByIdx_(), 
    loIdByIdx_(), loIdByCid_(), loFreqById_(), loPcWidthById_(), loSideBandById_(), loPolarizationById_(),
    loIdByTpSensorKey_(),
    loFreqByTpSensorKey_(), loSideBandByTpSensorKey_(), loPolarizationByTpSensorKey_(),
    ifFreqByTpSensorKey_(), ifBandwidthByTpSensorKey_(), ifIdByTpSensorKey_(),
    ifPolarizationById_(), 
    ifIdxById_(), ifSideBandById_(), ifFreqByIdx_(), origSensorIdById_(),
    pcalIdByKey_(), pcalPolarizationByKey_(), pcalLoFreqByKey_(), pcalSideBandByKey_(), 
    pcalOffsetByKey_(), pcalWidthByKey_(),
    pcalOffsetByLoId_(), xpsIdByKey_()
    {isOk_=false; hwType_=HT_UNDEF; backEndType_=BET_UNDEF;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgChannelSetup() {};

  //
  // Interfaces:
  //
  inline HwType getHwType() const {return hwType_;};
  inline BackEndType getBackEndType() const {return backEndType_;};
  inline bool isOk() const {return isOk_;};
  
  inline QMap<int, double>& bbcFreqByIdx() {return bbcFreqByIdx_;};
  inline QMap<int, double>& bbcBandwidthByIdx() {return bbcBandwidthByIdx_;};
  inline QMap<int, QString>& loIdByIdx() {return loIdByIdx_;};
  inline QMap<QString, QString>& loIdByCid() {return loIdByCid_;};
  inline QMap<QString, double>& loFreqById() {return loFreqById_;};
  inline QMap<QString, double>& loPcWidthById() {return loPcWidthById_;};
  inline QMap<QString, SgChannelSideBand>& loSideBandById() {return loSideBandById_;};
  inline QMap<QString, SgChannelPolarization>& loPolarizationById() {return loPolarizationById_;};
  inline QMap<QString, QString>& loIdByTpSensorKey() {return loIdByTpSensorKey_;};
  inline QMap<QString, double>& loFreqByTpSensorKey() {return loFreqByTpSensorKey_;};
  inline QMap<QString, SgChannelSideBand>& loSideBandByTpSensorKey() {return loSideBandByTpSensorKey_;};
  inline QMap<QString, SgChannelPolarization>& loPolarizationByTpSensorKey() {return loPolarizationByTpSensorKey_;};
  inline QMap<QString, double>& ifFreqByTpSensorKey() {return ifFreqByTpSensorKey_;};
  inline QMap<QString, double>& ifBandwidthByTpSensorKey() {return ifBandwidthByTpSensorKey_;};
  inline QMap<QString, SgChannelPolarization>& ifPolarizationById() {return ifPolarizationById_;};
  inline QMap<QString, int>& ifIdxById() {return ifIdxById_;};
  inline QMap<QString, SgChannelSideBand>& ifSideBandById() {return ifSideBandById_;};
  inline QMap<int, double>& ifFreqByIdx() {return ifFreqByIdx_;};
  inline QMap<QString, QList<QString> >& origSensorIdById() {return origSensorIdById_;};
  inline QMap<QString, QString>& ifIdByTpSensorKey() {return ifIdByTpSensorKey_;};

  inline QMap<QString, QString>& pcalIdByKey() {return pcalIdByKey_;};
  inline QMap<QString, SgChannelPolarization>& pcalPolarizationByKey() {return pcalPolarizationByKey_;};
  inline QMap<QString, double>& pcalLoFreqByKey() {return pcalLoFreqByKey_;};
  inline QMap<QString, SgChannelSideBand>& pcalSideBandByKey() {return pcalSideBandByKey_;};
  inline QMap<QString, double>& pcalOffsetByKey() {return pcalOffsetByKey_;}
  inline QMap<QString, double>& pcalWidthByKey() {return pcalWidthByKey_;}
  inline QMap<QString, double>& pcalOffsetByLoId() {return pcalOffsetByLoId_;}
  inline QMap<QString, QString>& xpsIdByKey() {return xpsIdByKey_;};

  inline void setHwType(HwType tp) {hwType_ = tp;};
  inline void setBackEndType(BackEndType tp) {backEndType_ = tp;};

  //
  // Functions:
  //
  bool selfCheck(const QString& stnKey);
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  QMap<int, double>             bbcFreqByIdx_;
  QMap<int, double>             bbcBandwidthByIdx_;
  // LOs:
  QMap<int, QString>            loIdByIdx_;
  QMap<QString, QString>        loIdByCid_;
  QMap<QString, double>         loFreqById_;
  QMap<QString, double>         loPcWidthById_;
  QMap<QString, SgChannelSideBand>
                                loSideBandById_;
  QMap<QString, SgChannelPolarization>
                                loPolarizationById_;
  // 4VGOS, variable setup:
  QMap<QString, QString>        loIdByTpSensorKey_;
  QMap<QString, double>         loFreqByTpSensorKey_;
  QMap<QString, SgChannelSideBand>
                                loSideBandByTpSensorKey_;
  QMap<QString, SgChannelPolarization>
                                loPolarizationByTpSensorKey_;
  QMap<QString, double>         ifFreqByTpSensorKey_;
  QMap<QString, double>         ifBandwidthByTpSensorKey_;
  QMap<QString, QString>        ifIdByTpSensorKey_;

  //
  // IFs:
  QMap<QString, SgChannelPolarization>
                                ifPolarizationById_;
  QMap<QString, int>            ifIdxById_;  // 1u -> 1
  QMap<QString, SgChannelSideBand>
                                ifSideBandById_;  // 1u -> USB
  
  QMap<int, double>             ifFreqByIdx_; // 1 -> frq
  // splitted by SideBand sensors:
  QMap<QString, QList<QString> >origSensorIdById_;
  //
  // 4 pcal:
  QMap<QString, QString>        pcalIdByKey_;
  QMap<QString, SgChannelPolarization>
                                pcalPolarizationByKey_;
  QMap<QString, double>         pcalLoFreqByKey_;
  QMap<QString, SgChannelSideBand>
                                pcalSideBandByKey_;
  QMap<QString, double>         pcalOffsetByKey_;
  QMap<QString, double>         pcalWidthByKey_;

  QMap<QString, double>         pcalOffsetByLoId_;

  QMap<QString, QString>        xpsIdByKey_;

  bool                          isOk_;
  HwType                        hwType_;
  BackEndType                   backEndType_;
};
/*=====================================================================================================*/






/***===================================================================================================*/
/**
 *  SgChannelSkeded:
 *
 */
/**====================================================================================================*/
class SgChannelSkeded
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgChannelSkeded() : stnCfgByKey_()
    {isOk_=false;};

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgChannelSkeded() {stnCfgByKey_.clear();};

  //
  // Interfaces:
  //
  inline bool isOk() const {return isOk_;};

  //
  // Functions:
  //
  bool parseSkdFile(const QString& fileName);
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  class ChanCfg
  {
  public:
    ChanCfg() : bandKey_(""), sideBands_() {skyFrq_=0.0; bandWidth_=0.0; chanIdx_=0;};
    ChanCfg(const ChanCfg& cc) : bandKey_(cc.bandKey_), sideBands_(cc.sideBands_)
      {skyFrq_=cc.skyFrq_; bandWidth_=cc.bandWidth_; chanIdx_=cc.chanIdx_;};
    ChanCfg(const QString& bandKey, int cIdx, double skyFrq, double bandWidth,
      const QList<SgChannelSideBand>& sideBands) :
      bandKey_(bandKey), sideBands_(sideBands) {skyFrq_=skyFrq; bandWidth_=bandWidth; chanIdx_=cIdx;};
   ~ChanCfg() {sideBands_.clear();};

    inline int getChanIdx() const {return chanIdx_;};
    inline const QString& getBandKey() const {return bandKey_;};
    inline double getSkyFrq() const {return skyFrq_;};
    inline double getBandWidth() const {return bandWidth_;};
    inline const QList<SgChannelSideBand>& getSideBands() const {return sideBands_;};

  private:
    int                         chanIdx_;
    QString                     bandKey_;
    double                      skyFrq_;
    double                      bandWidth_;
    QList<SgChannelSideBand>    sideBands_;
  };
  class StnCfg
  {
  public:
    StnCfg() : stnKey_(""), channelCfgs_() {};
    StnCfg(const QString& key) : stnKey_(key), channelCfgs_() {};
   ~StnCfg() {channelCfgs_.clear();};
    inline const QString& getStnKey() const {return stnKey_;};
    inline const QList<ChanCfg>& getChannelCfgs() const {return channelCfgs_;};
    inline QList<ChanCfg>& channelCfgs() {return channelCfgs_;};
    inline void setStnKey(const QString& key) {stnKey_ = key;};
  private:
    QString                     stnKey_;
    QList<ChanCfg>              channelCfgs_;
  };
  
  QMap<QString, StnCfg>         stnCfgByKey_;
  bool                          isOk_;
  friend class SgStnLogReadings;
};
/*=====================================================================================================*/









/***===================================================================================================*/
/**
 *  SgStnLogReadings:
 *
 */
/**====================================================================================================*/
class SgStnLogReadings
{
public:
  enum InputType
  {
    IT_UNDEF            = 0,
    IT_FSLG             = 1,    // field system log
    IT_CDMS             = 2,    // CDMS
    IT_PCMT             = 3,    // PCMT
    IT_METE             = 4,    // auxiliary meteo data
  };
  enum ContentType
  {
    CT_UNDEF            = 0,
    CT_IVS              = 1,    // a regular IVS session
    CT_SDE              = 2,    // a single dish experiment
  };

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  SgStnLogReadings();

  /**A constructor.
   * Creates an object.
   */
  SgStnLogReadings(const QString& stationName);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgStnLogReadings();

  //
  // Interfaces:
  inline InputType getItType() const {return itType_;};
  inline double getCableCalAmbig() const {return cableCalAmbig_;};
  inline int getCableSign() const {return cableSign_;};
  inline QList<SgOnSourceRecord*>& onSourceRecords() {return onSourceRecords_;}
  inline QList<SgCableCalReading*>& cableCals() {return cableCals_;}
  inline QList<SgCableCalReading*>& cableLongs() {return cableLongs_;}
  inline QList<SgMeteoReading*>& meteoPars() {return meteoPars_;}
  inline QList<SgTsysReading*>& tSyses() {return tSyses_;}
  inline QList<SgDbbc3TpReading*>& dbbc3Tps() {return dbbc3Tps_;}
  inline QList<SgTraklReading*>& trakls() {return trakls_;}
  inline QList<SgPcalReading*>& pcals() {return pcals_;}
  inline QList<SgSefdReading*>& sefds() {return sefds_;}
  inline QList<SgDot2xpsReading*>& dot2xpses() {return dot2xpses_;}
  
  inline bool areMeteosPresent() const {return areMeteosPresent_;};
  inline bool areCableCalsPresent() const {return areCableCalsPresent_;};
  inline const QString& getExperimentName() const {return experimentName_;};
  inline const QString& getFsVersionLabel() const {return fsVersionLabel_;};
  inline const QString& getBackEndInfo() const {return backEndInfo_;};
  inline const QString& getRecorderInfo() const {return recorderInfo_;};
  inline const SgChannelSetup& getChannelSetup() const {return channelSetup_;};
  inline ContentType getContentType() const {return contentType_;};



  inline void setItType(InputType t) {itType_ = t;};
  inline void setCableCalAmbig(double a) {cableCalAmbig_ = a;};
  inline void setCableSign(int n) {cableSign_ = n;};
  inline void setStationName(const QString& name) {stationName_ = name;};
  inline void setExperimentName(const QString& name) {experimentName_ = name;};
  inline void setFsVersionLabel(const QString& ver) {fsVersionLabel_ = ver;};
  inline void setBackEndInfo(const QString& info) {backEndInfo_ = info;};
  inline void setRecorderInfo(const QString& info) {recorderInfo_ = info;};
  inline SgChannelSetup& channelSetup() {return channelSetup_;};
  inline void setContentType(ContentType ct) {contentType_ = ct;};
   
  //
  // Functions:
  //
#ifndef SEPARATED_LOG2ANT
  void verifyReadings(SgVlbiStationInfo* stn, const QMap<QString, int> *defaultCableSignByStn);
  bool propagateData(SgVlbiStationInfo* stn);
#endif
  void allocateCableReadings();

  bool createAntabFile(const QString& inputLogFileName, const QString& outputFileName, 
    const QString& stnKey, const SgChannelSkeded *channelSkeded, bool useSkededChannelSetup, 
    bool have2overwrite, bool reportAllTsysData);
  bool createAntCalFile(const QString& inputLogFileName, const QString& outputFileName, 
    const QString& stnKey, const SgChannelSkeded *channelSkeded, bool useSkededChannelSetup, 
    bool have2overwrite, bool reportAllReadings, bool supressNonUsedSensors, int antcalOutputData, 
    const QString& ext4compress, const QString& userCommand);

  inline static const QString& sFiller() {return sFiller_;};
  inline static int            nFiller() {return nFiller_;};
  inline static double         dFiller() {return dFiller_;};

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...
private:
  InputType                     itType_;
  bool                          areMeteosPresent_;
  bool                          areCableCalsPresent_;
  bool                          areTsysesPresent_;
  bool                          tSensorsPrinted2AntCal_;
  double                        cableCalAmbig_;
  int                           cableSign_;
  QString                       stationName_;
  QString                       experimentName_;
  QString                       fsVersionLabel_;
  QString                       backEndInfo_;
  QString                       recorderInfo_;
  QList<SgOnSourceRecord*>      onSourceRecords_;
  QList<SgCableCalReading*>     cableCals_;
  QList<SgCableCalReading*>     cableLongs_;
  QList<SgMeteoReading*>        meteoPars_;
  QList<SgTsysReading*>         tSyses_;
  QList<SgDbbc3TpReading*>      dbbc3Tps_;
  QList<SgTraklReading*>        trakls_;
  QList<SgPcalReading*>         pcals_;
  QList<SgSefdReading*>         sefds_;
  QList<SgDot2xpsReading*>      dot2xpses_;
  static const QString          sFiller_;
  static const int              nFiller_;
  static const double           dFiller_;
  ContentType                   contentType_;

  
  QMap<QString, SgOnSourceRecord*>
                                recordByScan_;
  SgChannelSetup                channelSetup_;
  
  // functions:
  void removeCableCalReadings();
  void removeMeteoReadings();
  void removeTsysReadings();

  int determineCableSignFromLong();

#ifndef SEPARATED_LOG2ANT
  void lookupScanName(SgOnSourceRecord* osRec, const QMap<QString, SgVlbiAuxObservation*> &auxObsByScan);
#endif

  void cleanCableCalibrations();
  void cleanMeteoParameters();

#ifndef SEPARATED_LOG2ANT
  void interpolateMeteoParameters(SgVlbiAuxObservation* aux);
#endif  

  void export2antabFrqs_nativeSetup(QTextStream& s);
  void export2antabFrqs_vgosSetup(QTextStream& s);
  void export2antabFrqs_skededSetup(QTextStream& s, const SgChannelSkeded::StnCfg& sCfg);
  void export2antabFrqs_missedSetup(QTextStream& s);

  void export2antCalFrqs_nativeSetup(QTextStream& s);
  void export2antCalFrqs_vgosSetup(QTextStream& s);
//  void export2antCalFrqs_dbbc3Setup(QTextStream& s);
  void export2antCalFrqs_skededSetup(QTextStream& s, const SgChannelSkeded::StnCfg& sCfg);
  void export2antCalPcal_vgosSetup(QTextStream& s);

  bool compareChannelSetups(const SgChannelSkeded::StnCfg& sCfg);
  bool compareChannelNumbers(const SgChannelSkeded::StnCfg& sCfg);
//QString srcName4AntCal(const QString& srcName);
  QString str4AntCal(const QString& str, int width);
  
};
/*=====================================================================================================*/











/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
//
//
class SgStnLogCollector
{
public:
  enum FieldSystemEpochFormat
  {
    FSEF_UNDEF          = 0,
    FSEF_OLD            = 1,    // DDDHHMMSS
    FSEF_NEW            = 2,    // YYDDDHHMMSSss
    FSEF_NEWEST         = 3,    // YYYY.DDD.HH.MM.SS.ss
    FSEF_PCMT           = 4,    // YYYY M D H M S
    FSEF_MET            = 5,    // YYYY M D H M S T P Rh
  };


  class Procedure
  {
  public:
    inline Procedure() : name_(""), content_() {numOfExpanded_=0;};
    inline Procedure(const QString& name) : name_(name), content_() {numOfExpanded_=0;};
    inline Procedure(const Procedure& p) : name_(p.name_), content_(p.content_) 
      {numOfExpanded_=p.numOfExpanded_;};
    inline ~Procedure() {content_.clear();};
    // get/set:
    inline const QString& getName() const {return name_;};
    inline const QList<QString>& getContent() const {return content_;};
    inline int getNumOfExpanded() const {return numOfExpanded_;};
    inline void setName(const QString& name) {name_ = name;};
    inline QList<QString>& content() {return content_;};
    //
    inline void incNumOfExpanded() {numOfExpanded_++;};
    inline bool hasContent() const {return 0<content_.size();};
  private:
    QString                     name_;
    QList<QString>              content_;
    int                         numOfExpanded_;
  };

  // indices of contents:
  static int                    fsContentStartsAt_[];

  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  SgStnLogCollector();

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgStnLogCollector();


  //
  // Interfaces:
  //
  inline const QString& getStationName() const {return stationName_;};
  inline FieldSystemEpochFormat fsFmt() const {return fsFmt_;};
  inline int getAntcalOutputData() const {return antcalOutputData_;};

  inline const QMap<QString, int>* getDefaultCableSignByStn() const {return defaultCableSignByStn_;};

  inline bool hasCableRecord() const {return hasCableRecord_;};
  inline bool hasTsysRecord() const {return hasTsysRecord_;};
  inline bool hasCdmsRecord() const {return hasCdmsRecord_;};
  inline bool getUseSkededChannelSetup() const {return useSkededChannelSetup_;};
  inline const SgChannelSkeded* getChannelSkeded() const {return channelSkeded_;};
  inline const QString& getUserCommand() {return userCommand_;};

  //
  inline void setStationName(const QString& name) {stationName_ = name;};
  inline void setAntcalOutputData(int outputData) {antcalOutputData_ = outputData;};
  inline void setDefaultCableSignByStn(const QMap<QString, int>* map) {defaultCableSignByStn_ = map;};

  inline void setRinexFileNameByStn(const QMap<QString, QString>* map) {rinexFileNameByStn_ = map;};
  inline void setRinexPressureOffsetByStn(const QMap<QString, double>* map)
    {rinexPressureOffsetByStn_ = map;};

  inline void setUseSkededChannelSetup(bool use) {useSkededChannelSetup_ = use;};
  inline void setChannelSkeded(SgChannelSkeded* cs) {channelSkeded_ = cs;};
  inline SgChannelSkeded* channelSkeded() {return channelSkeded_;};
  //
  inline SgStnLogReadings& logReadings() {return logReadings_;};
  inline void setUserCommand(const QString& str) {userCommand_ = str;};
  inline QMap<QString, Procedure*>& procedureByName() {return procedureByName_;}

  //
  // Functions:
  //
  bool readLogFile(const QString& fileName, const QString& stnName, 
    const SgMJD& tFirst, const SgMJD& tLast, 
    const QString& orderOfMeteo);
  bool readDbbc3DumpFile(const QString& fileName);
  // just a shortcut:

#ifndef SEPARATED_LOG2ANT
  bool propagateData(SgVlbiStationInfo* stn, bool createAntabFile, bool overwriteAntabFile,
    bool reportAllTsysData);
#endif

  inline bool createAntabFile(const QString& stnKey, const QString& outputFileName, bool have2owrt, 
    bool reportAllTsysData)
    {
      return 
        logReadings_.createAntabFile(inputLogFileName_, outputFileName, stnKey, channelSkeded_,
          useSkededChannelSetup_, have2owrt, reportAllTsysData);
    };
  inline bool createAntCalFile(const QString& stnKey, const QString& outputFileName, bool have2owrt, 
    bool reportAllReadings, bool supressNonUsedSensors, const QString& ext4compress)
    {
      return 
        logReadings_.createAntCalFile(inputLogFileName_, outputFileName, stnKey, channelSkeded_,
          useSkededChannelSetup_, have2owrt, reportAllReadings, supressNonUsedSensors,
          antcalOutputData_, ext4compress, userCommand_);
    };
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...


private:
  QString                       stationName_;
  FieldSystemEpochFormat        fsFmt_;
  SgStnLogReadings              logReadings_;
  int                           inYear_;
  int                           antcalOutputData_;
  static const QRegularExpression
                                reFsOld_;
  static const QRegularExpression
                                reFsNew_;
  static const QRegularExpression
                                reFsNewest_;
  static const QRegularExpression
                                rePcmt_;
  static const QRegularExpression
                                reMet_;
  const QMap<QString, int>     *defaultCableSignByStn_;
  const QMap<QString, QString> *rinexFileNameByStn_;
  const QMap<QString, double>  *rinexPressureOffsetByStn_;
  bool                          hasCableRecord_;
  bool                          hasCdmsRecord_;
  bool                          hasTsysRecord_;
  bool                          useSkededChannelSetup_;
  QString                       inputLogFileName_;
  SgChannelSkeded              *channelSkeded_;
  QString                       userCommand_;
  QMap<QString, Procedure*>     procedureByName_;

  
  FieldSystemEpochFormat guessFsFormat(const QString& str);
  bool extractEpoch(const QString& str, SgMJD& t);
  bool extractContent(const QString& str, QString& content);
  bool extractMeteoReading(const QString& str, const QRegularExpression& reMeteo, SgMJD& t,
    SgMeteoData& m, const int* oom);

  bool extractTsysReading(const QString& str, const QRegularExpression& reTsys, SgMJD& t, 
    QMap<QString, float>& tSys, const SgMJD& tAtTheBegin);
/*
  bool extractTsysVgosReading(const QString& str, const QRegExp& reTsys, SgMJD& t, 
    QMap<QString, double>& tSys, const SgMJD& tAtTheBegin);
*/
  bool extractTsysVgosReading(const QString& str, const QRegularExpression& reTsys, SgMJD& t, 
    QMap<QString, float>& tSys, const SgMJD& tAtTheBegin);
/*
  bool extractTraklReading(const QString& str, const QRegExp& reTrakl, SgMJD& t, 
    SgTraklReading&, const SgMJD& tAtTheBegin);
*/

  bool extractTpcontVgosReading(const QString& str, const QRegularExpression& reTpcont, SgMJD& t,
    QMap<QString, QVector<int> >& tPcont, const SgMJD& tAtTheBegin);

  bool extractPcalVgosReading(const QString& str, const QRegularExpressionMatch& match, SgMJD& t,
    QMap<QString, float*>& pcal, SgChannelSetup& cs, const SgMJD& tAtTheBegin);

  bool extractPcalOffsetReading(const QString& str, const QRegularExpressionMatch& match, SgMJD& t,
    const SgMJD& tAtTheEnd, SgChannelSetup& cs, const SgMJD& tFirst);

  bool extractDot2xpsVgosReading(const QString& str, const QRegularExpressionMatch& match, SgMJD& t,
    QString& sKey, float& dot2xps, const SgMJD& tAtTheBegin, bool& isGps);

  bool extractTraklReading(const QString& str, const QRegularExpressionMatch& match, SgMJD& t,
    SgTraklReading&, const SgMJD& tAtTheBegin);

  bool extractSefdReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, QString& sensorId, QString& srcName, double& az, double& de, QVector<double>& vec, 
    const SgMJD& tAtTheBegin);

  bool extractCableCalibration(const QString& str, const QRegularExpressionMatch& match, SgMJD& t, 
    double& v);
  bool extractCableCalibrationSignByDiff(const QRegularExpressionMatch& match, int& v);
  bool checkRinexFile(const QString& fileName, const SgMJD& tStart, const SgMJD& tFinis, 
    const QString& rinexStnName);
  bool extractDataFromPcmtRecord(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, double& v, QString& source, QString& scan);
  bool extractDataFromMetRecord(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, SgMeteoData& m);

//bool extractBbcReading(const QString& str, const QRegExp& re, SgMJD& t, const SgMJD& atEnd, 
  bool extractBbcReading(const QString& str, const QRegularExpressionMatch& match, SgMJD& t, 
    const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractVcReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractLoReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractLoRxgReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractLoCdasUdc(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractPatchReading(const QString& str, const QRegularExpressionMatch& match,
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractChanSkyFrqReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  bool extractVciReading(const QString& str, const QRegularExpressionMatch& match, 
    SgMJD& t, const SgMJD& atEnd, SgChannelSetup& cs, const SgMJD& tFirst);
  //
  // support:
  QString setupTpSensor(const QString& sensorTag, const QString& callerName, const QString& inputString);
};
/*=====================================================================================================*/






#define NUM_OF_VLBA_ENTRIES (15)
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgVlbaLogCollector
{
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  // Constructors/Destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  SgVlbaLogCollector();

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgVlbaLogCollector();


  //
  // Interfaces:
  //
  const QString& getLogFileName() const {return logFileName_;};
  //
  // Functions:
  //
  bool readLogFile(const QString& fileName, int year);


#ifndef SEPARATED_LOG2ANT
  bool propagateData(SgVlbiStationInfo* stn);
#endif
  
  inline bool isVlba(const QString& key) const {return ivs2vlba_.contains(key);};

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...


private:
  QString                       logFileName_;
  QMap<QString, SgStnLogReadings*>
                                readingsByKey_;
  static const QString          vlbaStationCodes_[NUM_OF_VLBA_ENTRIES];
  static const QString          ivsStationNames_[NUM_OF_VLBA_ENTRIES];
  QMap<QString, QString>        ivs2vlba_;

  bool strs2mjd(int nYear, const QString& sDay, const QString& sHr, const QString& sMin, 
    const QString& sSec, SgMJD& t);
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* inline members:                                                                                     */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//



//
// INTERFACES:
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
#endif //SG_STN_LOG_COLLECTOR_H
