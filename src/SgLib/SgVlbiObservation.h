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

#ifndef SG_VLBI_OBSERVATION_H
#define SG_VLBI_OBSERVATION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QVector>

class QDataStream;


#include <Sg3dVector.h>
#include <Sg3dMatrix.h>
#include <SgObservation.h>
#include <SgVlbiObservable.h>



class SgTaskConfig;
class SgParameter;
class SgVlbiAuxObservation;
class SgVlbiBand;
class SgVlbiBaselineInfo;
class SgVlbiSession;
class SgVlbiSourceInfo;
class SgVlbiStationInfo;


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgVlbiObservation : public SgObservation
{
public:
  enum Attributes
  {
//  Attr_NOT_VALID          = 1<<0, //!< omit the observation;
//  Attr_PROCESSED          = 1<<1, //!< the observation has been processed;
//  Attr_FORCE_2_PROCESS    = 1<<2, //!< include the observation in data analysis in any condition;
    Attr_CLOCK_BREAK_AT_1   = 1<<3, //!< clock break occured at station #1
    Attr_CLOCK_BREAK_AT_2   = 1<<4, //!< clock break occured at station #2
    Attr_NOT_MATCHED        = 1<<5, //!< there is no match at another band(s)
    Attr_PASSED_CL_FN1      = 1<<6, //!< the observation participated in the procedure "calc clc fn#1"
    Attr_SBD_NOT_VALID      = 1<<7, //!< omit the observation if single band delay is analysed;
    Attr_CONTAINS_ALL_DATA  = 1<<8, //!< an observaiton probably is from a primary band (for DBH IO)
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgVlbiObservation(SgVlbiSession*);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~SgVlbiObservation();


  //
  // Interfaces:
  //
  /**An assignment operator.
   */
  // inline SgVlbiObservation& operator=(const SgVlbiObservation& obs);

  inline const QString& getKey();
  
  /**Returns a pointer on the session.
   */
  inline SgVlbiSession* session();
  
  /**Returns a pointer on the observable.
   */
  inline SgVlbiObservable* observable(const QString&);
  inline QMap<QString, SgVlbiObservable*>& observableByKey() {return observableByKey_;};

  // gets:
  /**Returns an index of the first station.
   */
  inline short int getStation1Idx() const;

  /**Returns an index of the second station.
   */
  inline short int getStation2Idx() const;

  /**Returns an index of the source.
   */
  inline short int getSourceIdx() const;
  
  /**Returns an index of the baseline -- just to unify interface.
   */
  inline short int getBaselineIdx() const;
  
  /**Returns an observable from the active band.
   */
  inline SgVlbiObservable* activeObs();

  /**Returns an observable from the primary band.
   */
  inline SgVlbiObservable* primeObs();

  inline QList<SgVlbiObservable*>& passiveObses() {return passiveObses_;};
  
  /**Returns a name of scan.
   */
  inline const QString& getScanName() const;
  
  inline const QString& getCorrRootFileName() const {return corrRootFileName_;};

  /**Returns an ID of scan.
   */
  inline const QString& getScanId() const;

  inline const QString& getScanFullName() const;

  inline double getDTec() const;

  inline double getDTecStdDev() const;

  /**Returns theoretical value of the delay (consensus model).
   */
  inline double getCalcConsensusDelay() const;
  inline double getCalcConsensusRate() const {return calcConsensusRate_;};

  /**Returns theoretical value of the bending delay.
   */
  inline double getCalcConsBendingDelay() const {return calcConsBendingDelay_;};
  inline double getCalcConsBendingRate () const {return calcConsBendingRate_;};
  inline double getCalcConsBendingSunDelay() const {return calcConsBendingSunDelay_;};
  inline double getCalcConsBendingSunRate () const {return calcConsBendingSunRate_;};
  inline double getCalcConsBendingSunHigherDelay() const {return calcConsBendingSunHigherDelay_;};
  inline double getCalcConsBendingSunHigherRate () const {return calcConsBendingSunHigherRate_;};
  inline double getCalcGpsIonoCorrectionDelay() const {return calcGpsIonoCorrectionDelay_;};
  inline double getCalcGpsIonoCorrectionRate () const {return calcGpsIonoCorrectionRate_;};

  /**Returns theoretical value of the delay due to ocean loading.
   */
  inline double getCalcOceanTideDelay() const {return calcOceanTideDelay_;};
  inline double getCalcOceanTideRate () const {return calcOceanTideRate_;};

  /**Returns value of the LCode "UT1ORTHO".
   */
  inline double getCalcHiFyUt1Delay() const {return calcHiFyUt1Delay_;};
  inline double getCalcHiFyUt1Rate() const {return calcHiFyUt1Rate_;};

  /**Returns an alternative to the LCode "UT1ORTHO" value.
   */
  inline double getExtDelayHiFyUt1() const {return extDelayHiFyUt1_;};
  inline double getExtRateHiFyUt1()  const {return extRateHiFyUt1_;};

  /**Returns value of the LCode "WOBORTHO".
   */
  inline double getCalcHiFyPxyDelay() const {return calcHiFyPxyDelay_;};
  inline double getCalcHiFyPxyRate() const {return calcHiFyPxyRate_;};
    
  /**Returns an alternative to the LCode "WOBORTHO" value.
   */
  inline double getExtDelayHiFyPxy() const {return extDelayHiFyPxy_;};
  inline double getExtRateHiFyPxy()  const {return extRateHiFyPxy_;};

  /**Returns precalculated correction for applied ERP from an external file.
   */
  inline double getExtDelayErp() const {return extDelayErp_;};
  inline double getExtRateErp()  const {return extRateErp_;};

  inline double getAprioriPxHfContrib() const {return aPrioriPxHfContrib_;};
  inline double getAprioriPxLfContrib() const {return aPrioriPxLfContrib_;};
  inline double getAprioriPyHfContrib() const {return aPrioriPyHfContrib_;};
  inline double getAprioriPyLfContrib() const {return aPrioriPyLfContrib_;};
  inline double getAprioriUt1HfContrib() const {return aPrioriUt1HfContrib_;};
  inline double getAprioriUt1LfContrib() const {return aPrioriUt1LfContrib_;};
  inline double getAprioriCxLfContrib() const {return aPrioriCxLfContrib_;};
  inline double getAprioriCyLfContrib() const {return aPrioriCyLfContrib_;};

  inline double aPrioriPx() const {return aPrioriPxHfContrib_ + aPrioriPxLfContrib_;};
  inline double aPrioriPy() const {return aPrioriPyHfContrib_ + aPrioriPyLfContrib_;};
  inline double aPrioriUt1() const {return aPrioriUt1HfContrib_ + aPrioriUt1LfContrib_;};
  inline double aPrioriCipX() const {return aPrioriCxLfContrib_;};
  inline double aPrioriCipY() const {return aPrioriCyLfContrib_;};

  /**Returns value of the LCode "PTD CONT".
   */
  inline double getCalcPoleTideDelay() const {return calcPoleTideDelay_;};
  inline double getCalcPoleTideRate () const {return calcPoleTideRate_;};

  /**Returns value of the LCode "ETD CONT".
   */
  inline double getCalcEarthTideDelay() const {return calcEarthTideDelay_;};
  inline double getCalcEarthTideRate () const {return calcEarthTideRate_;};

  /**Returns value of the LCode "WOBXCONT".
   */
  inline double getCalcPxDelay() const {return calcPxDelay_;};
  inline double getCalcPxRate() const {return calcPxRate_;};

  /**Returns value of the LCode "WOBYCONT".
   */
  inline double getCalcPyDelay() const {return calcPyDelay_;};
  inline double getCalcPyRate() const {return calcPyRate_;};

  /**Returns value of the LCode "WOBNUTAT".
   */
  inline double getCalcWobNutatContrib() const;

  /**Returns value of the LCode "FEED.COR".
   */
  inline double getCalcFeedCorrDelay() const {return calcFeedCorrDelay_;};
  inline double getCalcFeedCorrRate () const {return calcFeedCorrRate_;};

  /**Returns value of the LCode "TILTRMVR".
   */
  inline double getCalcTiltRemvrDelay() const {return calcTiltRemvrDelay_;};
  inline double getCalcTiltRemvrRate() const {return calcTiltRemvrRate_;};
   
  inline double getCalcOceanPoleTideLdDelay() const {return calcOceanPoleTideLdDelay_;};
  inline double getCalcOceanPoleTideLdRate() const {return calcOceanPoleTideLdRate_;};

  inline double getCalcHiFyUt1LibrationDelay() const {return calcHiFyUt1LibrationDelay_;};
  inline double getCalcHiFyUt1LibrationRate() const {return calcHiFyUt1LibrationRate_;};
  inline double getCalcHiFyPxyLibrationDelay() const {return calcHiFyPxyLibrationDelay_;};
  inline double getCalcHiFyPxyLibrationRate() const {return calcHiFyPxyLibrationRate_;};

  inline double getCalcOceanTideOldDelay() const {return calcOceanTideOldDelay_;};
  inline double getCalcOceanTideOldRate () const {return calcOceanTideOldRate_;};
  inline double getCalcPoleTideOldDelay() const {return calcPoleTideOldDelay_;};
  inline double getCalcPoleTideOldRate () const {return calcPoleTideOldRate_;};

  inline const Sg3dVector& getRsun() const {return rSun_;};
  inline const Sg3dVector& getVsun() const {return vSun_;};
  inline const Sg3dVector& getRmoon() const {return rMoon_;};
  inline const Sg3dVector& getVmoon() const {return vMoon_;};
  inline const Sg3dVector& getRearth() const {return rEarth_;};
  inline const Sg3dVector& getVearth() const {return vEarth_;};
  inline const Sg3dVector& getAearth() const {return aEarth_;};

  inline const Sg3dMatrix& getTrf2crfVal() const {return trf2crfVal_;};
  inline const Sg3dMatrix& getTrf2crfRat() const {return trf2crfRat_;};
  inline const Sg3dMatrix& getTrf2crfAcc() const {return trf2crfAcc_;};

  inline double getCalcUt1_Tai() const {return calcUt1_Tai_;};
  inline double getCalcPmX() const {return calcPmX_;};
  inline double getCalcPmY() const {return calcPmY_;};

  inline double getCalcCipXv() const;
  inline double getCalcCipYv() const;
  inline double getCalcCipSv() const;
  inline double getCalcCipXr() const;
  inline double getCalcCipYr() const;
  inline double getCalcCipSr() const;
  inline double getCalcNutWahr_dPsiV() const;
  inline double getCalcNutWahr_dEpsV() const;
  inline double getCalcNutWahr_dPsiR() const;
  inline double getCalcNutWahr_dEpsR() const;
  inline double getCalcNut2006_dPsiV() const;
  inline double getCalcNut2006_dEpsV() const;
  inline double getCalcNut2006_dPsiR() const;
  inline double getCalcNut2006_dEpsR() const;
   
  /**Returns site#1 partial derivatives. 
   */
  inline const Sg3dVector& getDdel_dR_1() const {return dDel_dR_1_;};
  inline const Sg3dVector& getDrat_dR_1() const {return dRat_dR_1_;};

  /**Returns site#2 partial derivatives.
   */
  inline const Sg3dVector& getDdel_dR_2() const {return dDel_dR_2_;};
  inline const Sg3dVector& getDrat_dR_2() const {return dRat_dR_2_;};

  /**Returns source partial derivative (right ascension).
   */
  inline double getDdel_dRA() const {return  dDel_dRA_;};
  inline double getDrat_dRA() const {return  dRat_dRA_;};
  /**Returns source partial derivative (declination).
   */
  inline double getDdel_dDN() const {return  dDel_dDN_;};
  inline double getDrat_dDN() const {return  dRat_dDN_;};

  inline double getDdel_dBend() const {return dDel_dBend_;};
  inline double getDrat_dBend() const {return dRat_dBend_;};
  /**Returns UT1 partial derivatives.
   */
  inline double getDdel_dUT1() const {return dDel_dUT1_;};
  inline double getDrat_dUT1() const {return dRat_dUT1_;};
  /**Returns second order UT1 partial derivatives.
   */
  inline double getD2del_dUT12() const {return d2Del_dUT12_;};
  inline double getD2rat_dUT12() const {return d2Rat_dUT12_;};
  /**Returns polar motion (x-coo) partial derivatives.
   */
  inline double getDdel_dPx() const {return dDel_dPx_;};
  inline double getDrat_dPx() const {return dRat_dPx_;};
  /**Returns polar motion (y-coo) partial derivatives.
   */
  inline double getDdel_dPy() const {return dDel_dPy_;};
  inline double getDrat_dPy() const {return dRat_dPy_;};
  /**Returns CIP (x-coo) partial derivatives.
   */
  inline double getDdel_dCipX() const {return dDel_dCipX_;};
  inline double getDrat_dCipX() const {return dRat_dCipX_;};
  /**Returns CIP (y-coo) partial derivatives.
   */
  inline double getDdel_dCipY() const {return dDel_dCipY_;};
  inline double getDrat_dCipY() const {return dRat_dCipY_;};
  // aux:
  inline double getDdel_dGamma() const {return dDel_dGamma_;};
  inline double getDrat_dGamma() const {return dRat_dGamma_;};
  inline double getDdel_dParallax() const {return dDel_dParallax_;};
  inline double getDrat_dParallax() const {return dRat_dParallax_;};
  inline double getDdel_dParallaxRev() const {return dDel_dParallaxRev_;};
  inline double getDrat_dParallaxRev() const {return dRat_dParallaxRev_;};
  inline double getDdel_dPolTideX() const {return dDel_dPolTideX_;};
  inline double getDrat_dPolTideX() const {return dRat_dPolTideX_;};
  inline double getDdel_dPolTideY() const {return dDel_dPolTideY_;};
  inline double getDrat_dPolTideY() const {return dRat_dPolTideY_;};

  inline double getFractC() const {return fractC_;};

  /**Returns function #1 of baseline's clocks (tmp).
   */
  inline double getBaselineClock_F1() const;
  inline double getBaselineClock_F2() const;
  inline double getBaselineClock_F1l() const;
  inline double getBaselineClock_F1r() const;
  //
  //
  inline double getApLength() const {return apLength_;};
  inline double sumAX_4delay() const {return sumAX_4delay_;};
  inline double sumAX_4rate() const {return sumAX_4rate_;};


//  /**Returns a pointer on band's stationInfo structure for the first station.
//   */
//  SgVlbiStationInfo* bandStn_1() {return bandStn_1_ ;};

//  /**Returns a pointer on band's stationInfo structure for the second station.
//   */
//  SgVlbiStationInfo* bandStn_2() {return bandStn_2_;};

//  /**Returns a pointer on band's sourceInfo structure for the source.
//   */
//  SgVlbiSourceInfo* bandSrc() {return bandSrc_;};

//  /**Returns a pointer on band's baselineInfo structure for the baseline.
//   */
//  SgVlbiBaselineInfo* bandBaseline() {return bandBaseline_;};

  /**Returns a pointer on session's stationInfo structure for the first station.
   */
  inline SgVlbiStationInfo* stn_1() {return stn_1_;};

  /**Returns a pointer on session's stationInfo structure for the second station.
   */
  inline SgVlbiStationInfo* stn_2() {return stn_2_;};

  /**Returns a pointer on session's sourceInfo structure for the source.
   */
  inline SgVlbiSourceInfo* src() {return src_;};

  /**Returns a pointer on session's baselineInfo structure for the baseline.
   */
  inline SgVlbiBaselineInfo* baseline() {return baseline_;};

  /**Returns a pointer on auxObservation structure for the first station.
   */
  inline SgVlbiAuxObservation* auxObs_1() {return auxObs_1_;};

  /**Returns a pointer on auxObservation structure for the second station.
   */
  inline SgVlbiAuxObservation* auxObs_2() {return auxObs_2_;};

  inline double theoDelay() const {return theoDelay_;};
  inline double theoRate() const {return theoRate_;};


  inline double reweightAuxSum4delay() const {return reweightAuxSum4delay_;};
  inline double reweightAuxSum4rate () const {return reweightAuxSum4rate_;};

  inline const QVector<double>& getUserCorrections() const {return userCorrections_;};

  inline int getPimaAutoSup() const {return pimaAutoSup_;};
  inline int getPimaUserSup() const {return pimaUserSup_;};
  inline int getPimaUserRec() const {return pimaUserRec_;};
  

  //-------------------------------------------------------------------------------------
  // sets:
  //
  inline void setKey(const QString&);

  /**Sets up an index of the first station.
   * \param idx -- the index;
   */
  inline void setStation1Idx(short int idx);

  /**Sets up an index of the second station.
   * \param idx -- the index;
   */
  inline void setStation2Idx(short int idx);

  /**Sets up an index of the source.
   * \param idx -- the index;
   */
  inline void setSourceIdx(short int idx);

  /**Sets up an index of the baseline.
   * \param idx -- the index;
   */
  inline void setBaselineIdx(short int idx);

  /**Sets up the name of the scan.
   * \param sName -- the new name;
   */
  inline void setScanName(const QString& sName);
  
  inline void setCorrRootFileName(const QString& sName) {corrRootFileName_=sName;};
  
  /**Sets up the ID of the scan.
   * \param sName -- the new name;
   */
  inline void setScanId(const QString& sId);

  inline void setScanFullName(const QString& sId);
  
  inline void setDTec(double d);

  inline void setDTecStdDev(double e);
  
  /**Sets up the theoretical value for delay ("consensus" model).
   * \param tau -- the delay;
   */
  inline void setCalcConsensusDelay(double tau);
  inline void setCalcConsensusRate(double r) {calcConsensusRate_ = r;};

  /**Sets up the value for bending delay.
   * \param tau -- the delay;
   */
  inline void setCalcConsBendingDelay(double d) {calcConsBendingDelay_ = d;};
  inline void setCalcConsBendingRate (double d) {calcConsBendingRate_ = d;};
  inline void setCalcConsBendingSunDelay(double d) {calcConsBendingSunDelay_ = d;};
  inline void setCalcConsBendingSunRate (double d) {calcConsBendingSunRate_ = d;};
  inline void setCalcConsBendingSunHigherDelay(double d) {calcConsBendingSunHigherDelay_ = d;};
  inline void setCalcConsBendingSunHigherRate (double d) {calcConsBendingSunHigherRate_ = d;};
  inline void setCalcGpsIonoCorrectionDelay(double d) {calcGpsIonoCorrectionDelay_ = d;};
  inline void setCalcGpsIonoCorrectionRate (double d) {calcGpsIonoCorrectionRate_ = d;};
  
  /**Sets up the value for a delay part caused by ocean loading.
   * \param tau -- the delay;
   */
  inline void setCalcOceanTideDelay(double v) {calcOceanTideDelay_ = v;};
  inline void setCalcOceanTideRate (double v) {calcOceanTideRate_ = v;};

  /**Sets up the value for the LCode "UT1ORTHO".
   * \param v -- the value;
   */
  inline void setCalcHiFyUt1Delay(double v) {calcHiFyUt1Delay_ = v;};
  inline void setCalcHiFyUt1Rate(double v) {calcHiFyUt1Rate_ = v;};

  /**Sets up the value for the external high frequency UT1 variations.
   * \param v -- the value;
   */
  inline void setExtDelayHiFyUt1(double v) {extDelayHiFyUt1_ = v;};
  inline void setExtRateHiFyUt1 (double v) {extRateHiFyUt1_  = v;};

  /**Sets up the value for the LCode "WOBORTHO".
   * \param v -- the value;
   */
  inline void setCalcHiFyPxyDelay(double v) {calcHiFyPxyDelay_ = v;};
  inline void setCalcHiFyPxyRate(double v) {calcHiFyPxyRate_ = v;};
  
  /**Sets up the value for the external high frequency PM variations.
   * \param v -- the value;
   */
  inline void setExtDelayHiFyPxy(double v) {extDelayHiFyPxy_ = v;};
  inline void setExtRateHiFyPxy (double v) {extRateHiFyPxy_  = v;};
  
  /**Sets up precalculated correction for applied ERP from an external file.
   * \param v -- the value;
   */
  inline void setExtDelayErp(double v) {extDelayErp_ = v;};
  inline void setExtRateErp (double v) {extRateErp_  = v;};

  inline void setAprioriPxHfContrib(double v) {aPrioriPxHfContrib_ = v;};
  inline void setAprioriPxLfContrib(double v) {aPrioriPxLfContrib_ = v;};
  inline void setAprioriPyHfContrib(double v) {aPrioriPyHfContrib_ = v;};
  inline void setAprioriPyLfContrib(double v) {aPrioriPyLfContrib_ = v;};
  inline void setAprioriUt1HfContrib(double v) {aPrioriUt1HfContrib_ = v;};
  inline void setAprioriUt1LfContrib(double v) {aPrioriUt1LfContrib_ = v;};
  inline void setAprioriCxLfContrib(double v) {aPrioriCxLfContrib_ = v;};
  inline void setAprioriCyLfContrib(double v) {aPrioriCyLfContrib_ = v;};

  /**Sets up the value of the LCode "PTD CONT".
   * \param v -- the value;
   */
  inline void setCalcPoleTideDelay(double v) {calcPoleTideDelay_ = v;};
  inline void setCalcPoleTideRate (double v) {calcPoleTideRate_ = v;};

  /**Sets up the value of the LCode "ETD CONT".
   * \param v -- the value;
   */
  inline void setCalcEarthTideDelay(double v) {calcEarthTideDelay_ = v;};
  inline void setCalcEarthTideRate (double v) {calcEarthTideRate_ = v;};

  /**Sets up the value of the LCode "WOBXCONT".
   * \param v -- the value;
   */
  inline void setCalcPxDelay(double v) {calcPxDelay_ = v;};
  inline void setCalcPxRate(double v) {calcPxRate_ = v;};

  /**Sets up the value of the LCode "WOBYCONT".
   * \param v -- the value;
   */
  inline void setCalcPyDelay(double v) {calcPyDelay_ = v;};
  inline void setCalcPyRate(double v) {calcPyRate_ = v;};

  /**Sets up the value of the LCode "WOBNUTAT".
   * \param v -- the value;
   */
  inline void setCalcWobNutatContrib(double v);

  /**Sets up the value of the LCode "FEED.COR".
   * \param v -- the value;
   */
  inline void setCalcFeedCorrDelay(double v) {calcFeedCorrDelay_ = v;};
  inline void setCalcFeedCorrRate (double v) {calcFeedCorrRate_ = v;};

  /**Sets up the value of the LCode "TILTRMVR".
   * \param v -- the value;
   */
  inline void setCalcTiltRemvrDelay(double v) {calcTiltRemvrDelay_ = v;};
  inline void setCalcTiltRemvrRate (double v) {calcTiltRemvrRate_ = v;};
    
  inline void setCalcOceanPoleTideLdDelay(double v) {calcOceanPoleTideLdDelay_ = v;};
  inline void setCalcOceanPoleTideLdRate (double v) {calcOceanPoleTideLdRate_ = v;};
  inline void setCalcHiFyUt1LibrationDelay(double v) {calcHiFyUt1LibrationDelay_ = v;};
  inline void setCalcHiFyUt1LibrationRate(double v) {calcHiFyUt1LibrationRate_ = v;};
  inline void setCalcHiFyPxyLibrationDelay(double v) {calcHiFyPxyLibrationDelay_ = v;};
  inline void setCalcHiFyPxyLibrationRate(double v) {calcHiFyPxyLibrationRate_ = v;};
  
 
  inline void setCalcOceanTideOldDelay(double v) {calcOceanTideOldDelay_ = v;};
  inline void setCalcOceanTideOldRate (double v) {calcOceanTideOldRate_ = v;};
  inline void setCalcPoleTideOldDelay(double v) {calcPoleTideOldDelay_ = v;};
  inline void setCalcPoleTideOldRate (double v) {calcPoleTideOldRate_ = v;};

  inline void setRsun(const Sg3dVector& r) {rSun_ = r;};
  inline void setVsun(const Sg3dVector& v) {vSun_ = v;};
  inline void setRmoon(const Sg3dVector& r) {rMoon_ = r;};
  inline void setVmoon(const Sg3dVector& v) {vMoon_ = v;};
  inline void setRearth(const Sg3dVector& r) {rEarth_ = r;};
  inline void setVearth(const Sg3dVector& v) {vEarth_ = v;};
  inline void setAearth(const Sg3dVector& a) {aEarth_ = a;};

  inline void setTrf2crfVal(const Sg3dMatrix& m) {trf2crfVal_ = m;};
  inline void setTrf2crfRat(const Sg3dMatrix& m) {trf2crfRat_ = m;};
  inline void setTrf2crfAcc(const Sg3dMatrix& m) {trf2crfAcc_ = m;};

  inline Sg3dMatrix& trf2crfVal() {return trf2crfVal_;};
  inline Sg3dMatrix& trf2crfRat() {return trf2crfRat_;};
  inline Sg3dMatrix& trf2crfAcc() {return trf2crfAcc_;};


  inline void setCalcUt1_Tai(double d) {calcUt1_Tai_ = d;};
  inline void setCalcPmX(double d) {calcPmX_ = d;};
  inline void setCalcPmY(double d) {calcPmY_ = d;};
  
  inline void setCalcCipXv(double v);
  inline void setCalcCipYv(double v);
  inline void setCalcCipSv(double v);
  inline void setCalcCipXr(double v);
  inline void setCalcCipYr(double v);
  inline void setCalcCipSr(double v);
  inline void setCalcNutWahr_dPsiV(double v);
  inline void setCalcNutWahr_dEpsV(double v);
  inline void setCalcNutWahr_dPsiR(double v);
  inline void setCalcNutWahr_dEpsR(double v);
  inline void setCalcNut2006_dPsiV(double v);
  inline void setCalcNut2006_dEpsV(double v);
  inline void setCalcNut2006_dPsiR(double v);
  inline void setCalcNut2006_dEpsR(double v);

  /**Sets up the site#1 partial derivatives.
   * \param r -- the derivatives;
   */
  inline void setDdel_dR_1(const Sg3dVector& r) {dDel_dR_1_ = r;};
  inline void setDrat_dR_1(const Sg3dVector& r) {dRat_dR_1_ = r;};

  /**Sets up the site#2 partial derivatives.
   * \param r -- the derivatives;
   */
  inline void setDdel_dR_2(const Sg3dVector& r) {dDel_dR_2_ = r;};
  inline void setDrat_dR_2(const Sg3dVector& r) {dRat_dR_2_ = r;};

  /**Sets up the source partial derivative (right ascension).
   * \param d -- the derivatives;
   */
  inline void setDdel_dRA(double d) {dDel_dRA_ = d;};
  inline void setDrat_dRA(double d) {dRat_dRA_ = d;};
  /**Sets up the source partial derivative (declination).
   * \param d -- the derivatives;
   */
  inline void setDdel_dDN(double d) {dDel_dDN_ = d;};
  inline void setDrat_dDN(double d) {dRat_dDN_ = d;};

  inline void setDdel_dBend(double d) {dDel_dBend_ = d;};
  inline void setDrat_dBend(double d) {dRat_dBend_ = d;};
  /**Sets up UT1 partial derivative.
   * \param d -- the derivatives;
   */
  inline void setDdel_dUT1(double d) {dDel_dUT1_ = d;};
  inline void setDrat_dUT1(double d) {dRat_dUT1_ = d;};
  /**Sets up 2nd degree UT1 partial derivative.
   * \param d -- the derivatives;
   */
  inline void setD2del_dUT12(double d) {d2Del_dUT12_ = d;};
  inline void setD2rat_dUT12(double d) {d2Rat_dUT12_ = d;};
  /**Sets up polar motion (x-coo) partial derivative.
   * \param d -- the derivatives;
   */
  inline void setDdel_dPx(double d) {dDel_dPx_ = d;};
  inline void setDrat_dPx(double d) {dRat_dPx_ = d;};
  /**Sets up polar motion (y-coo) partial derivative.
   * \param d -- the derivatives;
   */
  inline void setDdel_dPy(double d) {dDel_dPy_ = d;};
  inline void setDrat_dPy(double d) {dRat_dPy_ = d;};
  /**Sets up CIP (x-coo) partial derivative.
   * \param d -- the derivatives;
   */
  inline void setDdel_dCipX(double d) {dDel_dCipX_ = d;};
  inline void setDrat_dCipX(double d) {dRat_dCipX_ = d;};
  /**Sets up CIP (y-coo) partial derivative.
   * \param d -- the derivatives;
   */
  inline void setDdel_dCipY(double d) {dDel_dCipY_ = d;};
  inline void setDrat_dCipY(double d) {dRat_dCipY_ = d;};

  inline void setDdel_dGamma   (double d) {dDel_dGamma_     = d;};
  inline void setDrat_dGamma   (double d) {dRat_dGamma_     = d;};
  inline void setDdel_dParallax(double d) {dDel_dParallax_  = d;};
  inline void setDrat_dParallax(double d) {dRat_dParallax_  = d;};
  inline void setDdel_dParallaxRev(double d) {dDel_dParallaxRev_ = d;};
  inline void setDrat_dParallaxRev(double d) {dRat_dParallaxRev_ = d;};
  inline void setDdel_dPolTideX(double d) {dDel_dPolTideX_  = d;};
  inline void setDrat_dPolTideX(double d) {dRat_dPolTideX_  = d;};
  inline void setDdel_dPolTideY(double d) {dDel_dPolTideY_  = d;};
  inline void setDrat_dPolTideY(double d) {dRat_dPolTideY_  = d;};

  inline void setFractC(double d) {fractC_ = d;};


  /**Sets up the function #1 of baseline's clocks (tmp).
   * \param f -- value of the function;
   */
  inline void setBaselineClock_F1(double f);
  inline void setBaselineClock_F2(double f);
  inline void setBaselineClock_F1l(double f);
  inline void setBaselineClock_F1r(double f);
  //
  //
  inline void setApLength(double d) {apLength_ = d;};

  inline void setAuxObs_1(SgVlbiAuxObservation* aux) {auxObs_1_ = aux;};
  inline void setAuxObs_2(SgVlbiAuxObservation* aux) {auxObs_2_ = aux;};

  //
  inline QVector<double>& userCorrections() {return userCorrections_;};


  inline void setPimaAutoSup(int n) {pimaAutoSup_ = n;};
  inline void setPimaUserSup(int n) {pimaUserSup_ = n;};
  inline void setPimaUserRec(int n) {pimaUserRec_ = n;};
  


//  inline void setReweightAuxSum(double d) {reweightAuxSum_ = d;};

  //-----------------------------------------------------------------------------------------
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  virtual inline const QString className() const;

  /**
   */
  bool addObservable(const QString&, const SgVlbiObservable&);

  /**
   */
  bool addObservable(const QString&, SgVlbiObservable*);

  /**
   */
  virtual bool selfCheck();

  /**
   */
  void setupIdentities();
  
  /**
   */
  void setupActiveObservable(const QString&);
  
  /**
   */
  void setupActiveMeasurement(const SgTaskConfig*);
  
  /**
   */
  virtual bool isEligible(const SgTaskConfig*);

  /**
   */
  virtual void evaluateTheoreticalValues(SgTaskManager*);

  /**
   */
  virtual void prepare4Analysis(SgTaskManager*);

  /**
   */
  virtual void evaluateResiduals(SgTaskManager*);

  /**
   *
  bool modifyNotValidAttribute(bool =false);
  */

  /**
   */
  virtual const SgVector& o_c();

  /**
   */
  virtual const SgVector& sigma();
  
  /**
   */
  using SgObservation::operator<;
  inline virtual bool operator<(const SgVlbiObservation& obs) const;
  
  
  int calculateIonoBits();
  /**
   */
  void zerofyIono();
  void calcIonoCorrections(const SgTaskConfig*);
  inline double getNormalizedDelayResidual();
  inline double getNormalizedRateResidual();
  
  void resetAllEditings();
  
  int minQualityFactor() const;
  
  void propagateAttr(uint attr);
  void eradicateAttr(uint attr);
  void eradicateAttr(uint attr, SgTaskConfig::VlbiDelayType);

  void digestPimaAutoSupFlags();
  void digestPimaUserSupFlags();
  void digestPimaUserRecFlags();


  QString strId() const;
  
  
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //



private:
  QString                       key_;
  // an owner:
  SgVlbiSession                *session_;
  // subjects:
  short int                     station1Idx_;
  short int                     station2Idx_;
  short int                     sourceIdx_;
  short int                     baselineIdx_;
  // our observables (from databases, vgosDb trees, etc):
  QMap<QString, SgVlbiObservable*>
                                observableByKey_;
  SgVlbiObservable             *activeObs_;
  SgVlbiObservable             *primeObs_;
  QList<SgVlbiObservable*>      passiveObses_;
  QString                       scanName_;
  QString                       corrRootFileName_;
  QString                       scanId_;
  QString                       scanFullName_;
  // VGOS contribution:
  double                        dTec_;
  double                        dTecStdDev_;
  // CALC's products: theoretic values and contributions:
  double                        calcConsensusDelay_;
  double                        calcConsensusRate_;
  double                        calcConsBendingDelay_;
  double                        calcConsBendingRate_;
  double                        calcConsBendingSunDelay_;
  double                        calcConsBendingSunRate_;
  double                        calcConsBendingSunHigherDelay_;
  double                        calcConsBendingSunHigherRate_;
  double                        calcGpsIonoCorrectionDelay_;
  double                        calcGpsIonoCorrectionRate_;
  double                        calcOceanTideDelay_;
  double                        calcOceanTideRate_;
  double                        calcHiFyUt1Delay_;
  double                        calcHiFyUt1Rate_;
  double                        calcHiFyPxyDelay_;
  double                        calcHiFyPxyRate_;
  double                        calcPoleTideDelay_;
  double                        calcPoleTideRate_;
  double                        calcEarthTideDelay_;
  double                        calcEarthTideRate_;
  double                        calcPxDelay_;
  double                        calcPxRate_;
  double                        calcPyDelay_;
  double                        calcPyRate_;
  double                        calcWobNutatContrib_;
  double                        calcFeedCorrDelay_;
  double                        calcFeedCorrRate_;
  double                        calcTiltRemvrDelay_;
  double                        calcTiltRemvrRate_;
  //
  double                        calcOceanPoleTideLdDelay_;
  double                        calcOceanPoleTideLdRate_;
  double                        calcHiFyUt1LibrationDelay_;
  double                        calcHiFyUt1LibrationRate_;
  double                        calcHiFyPxyLibrationDelay_;
  double                        calcHiFyPxyLibrationRate_;
  //
  double                        calcOceanTideOldDelay_;
  double                        calcOceanTideOldRate_;
  double                        calcPoleTideOldDelay_;
  double                        calcPoleTideOldRate_;
  //
  Sg3dVector                    rSun_, vSun_;
  Sg3dVector                    rMoon_, vMoon_;
  Sg3dVector                    rEarth_, vEarth_, aEarth_;
  Sg3dMatrix                    trf2crfVal_, trf2crfRat_, trf2crfAcc_;
  double                        calcUt1_Tai_;
  double                        calcPmX_;
  double                        calcPmY_;
  double                        calcCipXv_;
  double                        calcCipXr_;
  double                        calcCipYv_;
  double                        calcCipYr_;
  double                        calcCipSv_;
  double                        calcCipSr_;
  double                        calcNutWahr_dPsiV_;
  double                        calcNutWahr_dEpsV_;
  double                        calcNutWahr_dPsiR_;
  double                        calcNutWahr_dEpsR_;
  double                        calcNut2006_dPsiV_;
  double                        calcNut2006_dEpsV_;
  double                        calcNut2006_dPsiR_;
  double                        calcNut2006_dEpsR_;

  // corrections for the external series of ERP:
  double                        extDelayErp_;
  double                        extRateErp_;
  // external high frequency ERP model:
  double                        extDelayHiFyUt1_;
  double                        extRateHiFyUt1_;
  double                        extDelayHiFyPxy_;
  double                        extRateHiFyPxy_;
  
  
  // for spoolfile output:
  double                        aPrioriPxHfContrib_;
  double                        aPrioriPyHfContrib_;
  double                        aPrioriUt1HfContrib_;
  double                        aPrioriPxLfContrib_;
  double                        aPrioriPyLfContrib_;
  double                        aPrioriUt1LfContrib_;
  double                        aPrioriCxLfContrib_;
  double                        aPrioriCyLfContrib_;
  
  // partials:
  // stations:
  Sg3dVector                    dDel_dR_1_;
  Sg3dVector                    dRat_dR_1_;
  Sg3dVector                    dDel_dR_2_;
  Sg3dVector                    dRat_dR_2_;
  // source:
  double                        dDel_dRA_;
  double                        dRat_dRA_;
  double                        dDel_dDN_;
  double                        dRat_dDN_;
  // UT1:
  double                        dDel_dUT1_;
  double                        dRat_dUT1_;
  double                        d2Del_dUT12_;
  double                        d2Rat_dUT12_;
  // Polar motion:
  double                        dDel_dPx_;
  double                        dRat_dPx_;
  double                        dDel_dPy_;
  double                        dRat_dPy_;
  // Angles of nutation:
  double                        dDel_dCipX_;
  double                        dRat_dCipX_;
  double                        dDel_dCipY_;
  double                        dRat_dCipY_;
  // Bending:
  double                        dDel_dBend_;
  double                        dRat_dBend_;
  // Gamma:
  double                        dDel_dGamma_;
  double                        dRat_dGamma_;
  // Parallax:
  double                        dDel_dParallax_;
  double                        dRat_dParallax_;
  double                        dDel_dParallaxRev_;
  double                        dRat_dParallaxRev_;
  // Polar Tides:
  double                        dDel_dPolTideX_;
  double                        dRat_dPolTideX_;
  double                        dDel_dPolTideY_;
  double                        dRat_dPolTideY_;
  //
  double                        fractC_;
  //
  // end of DB variables
  // from VEX:
  double                        apLength_;
  
  // for PIMA feeding:
  double                        sumAX_4delay_;
  double                        sumAX_4rate_;
  //
  // user corrections (bandless):
  QVector<double>               userCorrections_;
  
  // for data exchange:
  static SgVector               storage4O_C;
  static SgVector               storage4Sigmas;

  // pointers on external data structures, need during run-time for various procedures:
  SgVlbiStationInfo            *stn_1_;
  SgVlbiStationInfo            *stn_2_;
  SgVlbiSourceInfo             *src_;
  SgVlbiBaselineInfo           *baseline_;
  SgVlbiAuxObservation         *auxObs_1_;
  SgVlbiAuxObservation         *auxObs_2_;

  // theoretical values:
  double                        theoDelay_;
  double                        theoRate_;
  //
  double                        extAprioriCorrections4delay_;
  double                        extAprioriCorrections4rate_;

  // supplementary:
  double                        baselineClock_F1_;
  double                        baselineClock_F2_;
  double                        baselineClock_F1l_;
  double                        baselineClock_F1r_;

  double                        reweightAuxSum4delay_;
  double                        reweightAuxSum4rate_;
  
  //
  QList<SgParameter*>           parameters_;
  
  // PIMA's suppresion codes:
  int                           pimaAutoSup_;
  int                           pimaUserSup_;
  int                           pimaUserRec_;
  
  /**
   */
  void                          fillPartials(SgTaskManager*);
  void                          fillPartials4delay(SgTaskManager*);
  void                          fillPartials4rate(SgTaskManager*);
  
  void                          evalExtApriori_StnPos(const SgTaskConfig*, double& dD, double& dR);
  void                          evalExtApriori_SrcPos(double& dD, double& dR);
  void                          evalExtApriori_AxsOfs(double& dD, double& dR);
  void                          evalExtApriori_MeanGrads(double& dD, double& dR);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiObservation inline members:                                                                   */
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
inline const QString& SgVlbiObservation::getKey()
{
  return key_;
};



//
inline SgVlbiSession* SgVlbiObservation::session()
{
  return session_;
};



inline SgVlbiObservable* SgVlbiObservation::observable(const QString& bandKey)
{
  return observableByKey_.contains(bandKey) ? observableByKey_.value(bandKey) : NULL;
};



//
inline SgVlbiObservable* SgVlbiObservation::activeObs()
{
  return activeObs_;
};



//
inline SgVlbiObservable* SgVlbiObservation::primeObs()
{
  return primeObs_;
};



// returns idx of St.#1
inline short int SgVlbiObservation::getStation1Idx() const
{
  return station1Idx_;
};



// returns idx of St.#2
inline short int SgVlbiObservation::getStation2Idx() const
{
  return station2Idx_;
};



// returns idx of Src
inline short int SgVlbiObservation::getSourceIdx() const
{
  return sourceIdx_;
};



// returns idx of baseline
inline short int SgVlbiObservation::getBaselineIdx() const
{
  return baselineIdx_;
};



//
inline const QString& SgVlbiObservation::getScanName() const
{
  return scanName_;
};



//
inline const QString& SgVlbiObservation::getScanId() const
{
  return scanId_;
};



//
inline const QString& SgVlbiObservation::getScanFullName() const
{
  return scanFullName_;
};



//
inline double SgVlbiObservation::getDTec() const 
{
  return dTec_;
};



//
inline double SgVlbiObservation::getDTecStdDev() const 
{
  return dTecStdDev_;
};



//
inline double SgVlbiObservation::getCalcConsensusDelay() const
{
  return calcConsensusDelay_;
};



//
inline double SgVlbiObservation::getCalcWobNutatContrib() const
{
  return calcWobNutatContrib_;
};



//
inline double SgVlbiObservation::getCalcCipXv() const
{
  return calcCipXv_;
};



//
inline double SgVlbiObservation::getCalcCipYv() const
{
  return calcCipYv_;
};



//
inline double SgVlbiObservation::getCalcCipSv() const
{
  return calcCipSv_;
};



//
inline double SgVlbiObservation::getCalcCipXr() const
{
  return calcCipXr_;
};



//
inline double SgVlbiObservation::getCalcCipYr() const
{
  return calcCipYr_;
};



//
inline double SgVlbiObservation::getCalcCipSr() const
{
  return calcCipSr_;
};



//
inline double SgVlbiObservation::getCalcNutWahr_dPsiV() const
{
  return calcNutWahr_dPsiV_;
};



//
inline double SgVlbiObservation::getCalcNutWahr_dEpsV() const
{
  return calcNutWahr_dEpsV_;
};



//
inline double SgVlbiObservation::getCalcNutWahr_dPsiR() const
{
  return calcNutWahr_dPsiR_;
};



//
inline double SgVlbiObservation::getCalcNutWahr_dEpsR() const
{
  return calcNutWahr_dEpsR_;
};



//
inline double SgVlbiObservation::getCalcNut2006_dPsiV() const
{
  return calcNut2006_dPsiV_;
};



//
inline double SgVlbiObservation::getCalcNut2006_dEpsV() const
{
  return calcNut2006_dEpsV_;
};



//
inline double SgVlbiObservation::getCalcNut2006_dPsiR() const
{
  return calcNut2006_dPsiR_;
};



//
inline double SgVlbiObservation::getCalcNut2006_dEpsR() const
{
  return calcNut2006_dEpsR_;
};






//  sets:
//
//
inline void SgVlbiObservation::setKey(const QString& key)
{
  key_ = key;
};



//
inline void SgVlbiObservation::setStation1Idx(short int idx)
{
  station1Idx_ = idx;
};



//
inline void SgVlbiObservation::setStation2Idx(short int idx)
{
  station2Idx_ = idx;
};



//
inline void SgVlbiObservation::setSourceIdx(short int idx)
{
  sourceIdx_ = idx;
};



//
inline void SgVlbiObservation::setBaselineIdx(short int idx)
{
  baselineIdx_ = idx;
};



//
inline void SgVlbiObservation::setScanName(const QString& sName)
{
  scanName_ = sName;
};



//
inline void SgVlbiObservation::setScanId(const QString& sId)
{
  scanId_ = sId;
};



//
inline void SgVlbiObservation::setScanFullName(const QString& sId)
{
  scanFullName_ = sId;
};



//
inline void SgVlbiObservation::setDTec(double d)
{
  dTec_ = d;
};



//
inline void SgVlbiObservation::setDTecStdDev(double e)
{
  dTecStdDev_ = e;
};



//
inline void SgVlbiObservation::setCalcConsensusDelay(double d)
{
  calcConsensusDelay_ = d;
};



//
inline void SgVlbiObservation::setCalcWobNutatContrib(double v)
{
  calcWobNutatContrib_ = v;
};



//
inline void SgVlbiObservation::setCalcCipXv(double v)
{
  calcCipXv_ = v;
};



//
inline void SgVlbiObservation::setCalcCipYv(double v)
{
  calcCipYv_ = v;
};



//
inline void SgVlbiObservation::setCalcCipSv(double v)
{
  calcCipSv_ = v;
};



//
inline void SgVlbiObservation::setCalcCipXr(double v)
{
  calcCipXr_ = v;
};



//
inline void SgVlbiObservation::setCalcCipYr(double v)
{
  calcCipYr_ = v;
};



//
inline void SgVlbiObservation::setCalcCipSr(double v)
{
  calcCipSr_ = v;
};



//
inline void SgVlbiObservation::setCalcNutWahr_dPsiV(double v)
{
  calcNutWahr_dPsiV_ = v;
};



//
inline void SgVlbiObservation::setCalcNutWahr_dEpsV(double v)
{
  calcNutWahr_dEpsV_ = v;
};



//
inline void SgVlbiObservation::setCalcNutWahr_dPsiR(double v)
{
  calcNutWahr_dPsiR_ = v;
};



//
inline void SgVlbiObservation::setCalcNutWahr_dEpsR(double v)
{
  calcNutWahr_dEpsR_ = v;
};



//
inline void SgVlbiObservation::setCalcNut2006_dPsiV(double v)
{
  calcNut2006_dPsiV_ = v;
};



//
inline void SgVlbiObservation::setCalcNut2006_dEpsV(double v)
{
  calcNut2006_dEpsV_ = v;
};



//
inline void SgVlbiObservation::setCalcNut2006_dPsiR(double v)
{
  calcNut2006_dPsiR_ = v;
};



//
inline void SgVlbiObservation::setCalcNut2006_dEpsR(double v)
{
  calcNut2006_dEpsR_ = v;
};




//
inline double SgVlbiObservation::getBaselineClock_F1() const
{
  return baselineClock_F1_;
};

//
inline double SgVlbiObservation::getBaselineClock_F2() const
{
  return baselineClock_F2_;
};

inline double SgVlbiObservation::getBaselineClock_F1r() const
{
  return baselineClock_F1r_;
};

inline double SgVlbiObservation::getBaselineClock_F1l() const
{
  return baselineClock_F1l_;
};

inline void SgVlbiObservation::setBaselineClock_F1(double f)
{
  baselineClock_F1_ = f;
};

inline void SgVlbiObservation::setBaselineClock_F2(double f)
{
  baselineClock_F2_ = f;
};

inline void SgVlbiObservation::setBaselineClock_F1l(double f)
{
  baselineClock_F1l_ = f;
};

inline void SgVlbiObservation::setBaselineClock_F1r(double f)
{
  baselineClock_F1r_ = f;
};


//
// FUNCTIONS:
//
//
inline const QString SgVlbiObservation::className() const 
{
  return "SgVlbiObservation";
};



//
inline bool SgVlbiObservation::operator<(const SgVlbiObservation& obs) const
{
  return
    *this < (const SgMJD&)obs ? true : 
      (SgMJD::operator==(obs) ? (mediaIdx_ < obs.getMediaIdx()) : false);
};



//
inline double SgVlbiObservation::getNormalizedDelayResidual()
{
  return activeObs_?(activeObs_->activeDelay()?activeObs_->activeDelay()->getResidualNorm():0.0):0.0;
};



//
inline double SgVlbiObservation::getNormalizedRateResidual()
{
  return activeObs_?activeObs_->phDRate().getResidualNorm():0.0;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
#endif // SG_VLBI_OBSERVATION_H
