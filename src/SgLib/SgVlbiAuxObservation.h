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

#ifndef SG_VLBI_AUX_OBSERVATION_H
#define SG_VLBI_AUX_OBSERVATION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <Sg3dVector.h>
#include <SgObservation.h>
#include <SgTaskConfig.h>
#include <SgMeteoData.h>
#include <SgVector.h>




class SgTroposphereModelData
{
public:
  inline SgTroposphereModelData() 
    {dVal0_[0] = dVal0_[1] = dVal1_[0] = dVal1_[1] = dGrdN_[0] = dGrdN_[1] = dGrdE_[0] = dGrdE_[1] = 0.0;
     dMap0_[0] = dMap0_[1] = dMap1_[0] = dMap1_[1] = 1.0;};
  inline ~SgTroposphereModelData() {};
    
  inline double getVal0_delay() const {return dVal0_[0];};
  inline double getVal0_rate()  const {return dVal0_[1];};
  inline double getVal1_delay() const {return dVal1_[0];};
  inline double getVal1_rate()  const {return dVal1_[1];};
  
  inline double getMap0_delay() const {return dMap0_[0];};
  inline double getMap0_rate()  const {return dMap0_[1];};
  inline double getMap1_delay() const {return dMap1_[0];};
  inline double getMap1_rate()  const {return dMap1_[1];};

  inline double getGrdN_delay() const {return dGrdN_[0];};
  inline double getGrdN_rate()  const {return dGrdN_[1];};
  inline double getGrdE_delay() const {return dGrdE_[0];};
  inline double getGrdE_rate()  const {return dGrdE_[1];};


  inline void setVal0_delay(double d) {dVal0_[0] = d;};
  inline void setVal0_rate(double d)  {dVal0_[1] = d;};
  inline void setVal1_delay(double d) {dVal1_[0] = d;};
  inline void setVal1_rate(double d)  {dVal1_[1] = d;};
  
  inline void setMap0_delay(double d) {dMap0_[0] = d;};
  inline void setMap0_rate(double d)  {dMap0_[1] = d;};
  inline void setMap1_delay(double d) {dMap1_[0] = d;};
  inline void setMap1_rate(double d)  {dMap1_[1] = d;};

  inline void setGrdN_delay(double d) {dGrdN_[0] = d;};
  inline void setGrdN_rate(double d)  {dGrdN_[1] = d;};
  inline void setGrdE_delay(double d) {dGrdE_[0] = d;};
  inline void setGrdE_rate(double d)  {dGrdE_[1] = d;};

  inline void setDelayData(double dV0, double dV1, double dM0, double dM1, double dGN, double dGE)
    { dVal0_[0] = dV0; dVal1_[0] = dV1; dMap0_[0] = dM0; dMap1_[0] = dM1; 
      dGrdN_[0] = dGN; dGrdE_[0] = dGE;};
  inline void setRateData(double dV0, double dV1, double dM0, double dM1, double dGN, double dGE)
    { dVal0_[1] = dV0; dVal1_[1] = dV1; dMap0_[1] = dM0; dMap1_[1] = dM1; 
      dGrdN_[1] = dGN; dGrdE_[1] = dGE;};


private:
  double                        dVal0_[2];
  double                        dVal1_[2];
  double                        dMap0_[2];
  double                        dMap1_[2];
  double                        dGrdN_[2];
  double                        dGrdE_[2];
};


extern const SgTroposphereModelData
                                tmdZero;




class SgVlbiObservation;
/***===================================================================================================*/
/**
 * The class is representing observation-like data (meteo parameters, cable calibraiton, other 
 * measurements) for each station.
 *
 */
/**====================================================================================================*/
class SgVlbiAuxObservation : public SgObservation
{
public:
  enum Attributes
  {
//  Attr_NOT_VALID          = 1<<0, //!< omit the observation;
//  Attr_PROCESSED          = 1<<1, //!< the observation has been processed;
//  Attr_FORCE_2_PROCESS    = 1<<2, //!< include the observation in data analysis in any condition;
    Attr_CLOCK_BREAK        = 1<<3, //!< clock break occured at the station
    Attr_CABLE_CAL_BAD      = 1<<4, //!< clock break occured at the station
  };
  //
  enum CableCorrectionType
  {
    CCT_FSLG             = 0,    // field system log
    CCT_CDMS             = 1,    // CDMS
    CCT_PCMT             = 2,    // PCMT
    CCT_DFLT             = 3,    // Default
  };



  // Statics:
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgVlbiAuxObservation();

 
  /**A destructor.
   * Frees allocated memory.
   */
  inline virtual ~SgVlbiAuxObservation();



  //
  // Interfaces:
  //

  // gets:
  inline SgVlbiObservation* getOwner() const;
  
  /**Returns an index of the station.
   */
  inline short int getStationIdx() const;

  /**Returns meteo parameters.
   */
  inline const SgMeteoData& getMeteoData() const;

  /**Returns a reference to meteo parameters.
   */
  inline SgMeteoData& meteoData();

  /**Returns cable calibration correction.
   */
  inline double getCableCalibration() const;

  /**Returns multiplier (usually sign: -1 or +1) of cable calibration correction.
   */
  inline double getCableCalSign() const;

  /**Returns angle of antenna azimuth, rads.
   */
  inline double getAzimuthAngle() const;

  /**Returns rate of angle of antenna azimuth, rads.
   */
  inline double getAzimuthAngleRate() const;

  /**Returns angle of antenna elevation, rads.
   */
  inline double getElevationAngle() const;

  /**Returns rate of angle of antenna elevation, rads.
   */
  inline double getElevationAngleRate() const;

  /**Returns parallactic angle of antenna, rads.
   */
  inline double getParallacticAngle() const;

  /**Returns a priori clock model (if necessary) in sec.
   */
  inline double getAprioriClocks() const;

  /**Returns estimated clocks value.
   */
  inline double getEstClocks() const;

  /**Returns std. deviations of estimated clocks value.
   */
  inline double getEstClocksSigma() const;

  /**Returns estimated zenith delay.
   */
  inline double getEstZenithDelay() const;

  /**Returns std. deviations of estimated zenith delay values.
   */
  inline double getEstZenithDelaySigma() const;

  /**Returns estimated northern component of atmospheric gradient.
   */
  inline double getEstAtmGradN() const;

  /**Returns std. deviations of estimated northern component of atmospheric gradient.
   */
  inline double getEstAtmGradNSigma() const;
  
  /**Returns estimated eastern component of atmospheric gradient.
   */
  inline double getEstAtmGradE() const;

  /**Returns std. deviations of estimated eastern component of atmospheric gradient.
   */
  inline double getEstAtmGradESigma() const;

  /**Returns hydrostatic zenith delay.
   */
  inline double getZenithDelayH() const;

  /**Returns zenith delay caused by water vapour.
   */
  inline double getZenithDelayW() const;

  /**Returns value of the LCode "NDRYCONT".
   */
  inline double getCalcNdryCont4Delay() const;
  inline double getCalcNdryCont4Rate() const;

  /**Returns value of the LCode "NWETCONT".
   */
  inline double getCalcNwetCont4Delay() const;
  inline double getCalcNwetCont4Rate() const;

  /**Returns value of the LCode "UNPHASCL".
   */
  inline double getCalcUnPhaseCal() const;

  /**Returns value of the LCode "AXO CONT".
   */
  inline double getCalcAxisOffset4Delay() const;
  inline double getCalcAxisOffset4Rate() const;

  /**
   */
  inline double getCalcOLoadHorz4Delay() const;
  inline double getCalcOLoadHorz4Rate() const;
  
  /**
   */
  inline double getCalcOLoadVert4Delay() const;
  inline double getCalcOLoadVert4Rate() const;

  /**Returns dTau/dAxisOffset value.
   */
  inline double getDdel_dAxsOfs() const;

  /**Returns dRate/dAxisOffset value.
   */
  inline double getDrat_dAxsOfs() const;

  inline double getDdel_dTzdDry() const {return dDel_dTzdDry_;}
  inline double getDrat_dTzdDry() const {return dRat_dTzdDry_;}
  inline double getDdel_dTzdWet() const {return dDel_dTzdWet_;}
  inline double getDrat_dTzdWet() const {return dRat_dTzdWet_;}
  inline double getDdel_dTzdGrdN() const {return dDel_dTzdGrdN_;}
  inline double getDrat_dTzdGrdN() const {return dRat_dTzdGrdN_;}
  inline double getDdel_dTzdGrdE() const {return dDel_dTzdGrdE_;}
  inline double getDrat_dTzdGrdE() const {return dRat_dTzdGrdE_;}

  inline const Sg3dVector& getOcnLdR() const {return ocnLdR_;};
  inline const Sg3dVector& getOcnLdV() const {return ocnLdV_;};

  inline double getRefClockOffset() const {return refClockOffset_;};

  inline const SgVector* getTsyses() const {return tSyses_;};
  inline const SgVector& cableCorrections() const {return cableCorrections_;};
  inline double getTsys(unsigned int idx) const 
    {return (tSyses_ && idx<tSyses_->n())?tSyses_->getElement(idx):-10.0;};

  inline const QString& getTapeId() const {return tapeId_;};



  // sets:
  //
  inline void setOwner(SgVlbiObservation*);
  
  /**Sets up an index of the station.
   * \param idx -- the index;
   */
  inline void setStationIdx(short int idx);

  /**Sets up meteo data.
   * \param meteo -- meteo data;
   */
  inline void setMeteoData(const SgMeteoData& meteo);

  /**Sets up cable calibration.
   * \param cable -- cable calibration correction;
   */
  inline void setCableCalibration(double cable);

  /**Sets up a multiplier of cable calibration.
   * \param s -- a sign of cable calibration correction;
   */
  inline void setCableCalSign(double s);

  /**Sets up azimuth angle.
   * \param angle -- azimuth angle, rads;
   */
  inline void setAzimuthAngle(double angle);

  /**Sets up azimuth angle rate.
   * \param angle -- azimuth angle, rads;
   */
  inline void setAzimuthAngleRate(double rate);

  /**Sets up elevation angle.
   * \param angle -- elevation angle rate, rad/?;
   */
  inline void setElevationAngle(double angle);

  /**Sets up elevation angle rate.
   * \param angle -- elevation angle rate, rad/?;
   */
  inline void setElevationAngleRate(double rate);

  /**Sets up parallactic angle.
   * \param angle -- parallactic angle, rads;
   */
  inline void setParallacticAngle(double angle);

  /**Sets up apriori clock model.
   * \param clocks -- clock model, sec;
   */
  inline void setAprioriClocks(double clocks);

  /**Sets up estimated clocks value.
   * \param d -- estimations;
   */
  inline void setEstClocks(double d);

  /**Sets up std. deviations of the estimated clocks value.
   * \param e -- sigma;
   */
  inline void setEstClocksSigma(double e);

  /**Sets up estimated zenith delay.
   * \param d -- value;
   */
  inline void setEstZenithDelay(double d);

  /**Sets up std. deviations of the estimated zenith delay.
   * \param e -- sigma;
   */
  inline void setEstZenithDelaySigma(double e);

  /**Sets up estimated northern component of atmospheric gradient.
   * \param g -- gradient;
   */
  inline void setEstAtmGradN(double g);

  /**Sets up std. deviations of the estimated northern component of atmospheric gradient.
   * \param s -- sigma;
   */
  inline void setEstAtmGradNSigma(double s);
  
  /**Sets up estimated eastern component of atmospheric gradient.
   * \param g -- gradient;
   */
  inline void setEstAtmGradE(double g);

  /**Sets up std. deviations of the estimated eastern component of atmospheric gradient.
   * \param s -- sigma;
   */
  inline void setEstAtmGradESigma(double s);

  /**Sets up hydrostatic zenith delay.
   * \param d -- delay;
   */
  inline void setZenithDelayH(double d);

  /**Sets up zenith delay cause by water vapour.
   * \param d -- delay;
   */
  inline void setZenithDelayW(double d);

  /**Sets up the value of the LCode "NDRYCONT".
   * \param v -- the value;
   */
  inline void setCalcNdryCont4Delay(double v);
  inline void setCalcNdryCont4Rate(double v);

  /**Sets up the value of the LCode "NWETCONT".
   * \param v -- the value;
   */
  inline void setCalcNwetCont4Delay(double v);
  inline void setCalcNwetCont4Rate(double v);

  /**Sets up the value of the LCode "UNPHASCL".
   * \param v -- the value;
   */
  inline void setCalcUnPhaseCal(double v);

  /**Sets up the value of the LCode "AXO CONT".
   * \param v -- the value;
   */
  inline void setCalcAxisOffset4Delay(double v);
  inline void setCalcAxisOffset4Rate(double v);

  /**
   */
  inline void setCalcOLoadHorz4Delay(double v);
  inline void setCalcOLoadHorz4Rate(double v);
  
  /**
   */
  inline void setCalcOLoadVert4Delay(double v);
  inline void setCalcOLoadVert4Rate(double v);

  /**Sets the dTau/dAxisOffset value.
   * \param v -- the value;
   */
  inline void setDdel_dAxsOfs(double v);
  inline void setDrat_dAxsOfs(double v);

  inline void setDdel_dTzdDry(double v) {dDel_dTzdDry_ = v;};
  inline void setDrat_dTzdDry(double v) {dRat_dTzdDry_ = v;};
  inline void setDdel_dTzdWet(double v) {dDel_dTzdWet_ = v;};
  inline void setDrat_dTzdWet(double v) {dRat_dTzdWet_ = v;};

  inline void setDdel_dTzdGrdN(double v) {dDel_dTzdGrdN_ = v;};
  inline void setDrat_dTzdGrdN(double v) {dRat_dTzdGrdN_ = v;};
  inline void setDdel_dTzdGrdE(double v) {dDel_dTzdGrdE_ = v;};
  inline void setDrat_dTzdGrdE(double v) {dRat_dTzdGrdE_ = v;};

  inline void setOcnLdR(const Sg3dVector& r) {ocnLdR_ = r;};
  inline void setOcnLdV(const Sg3dVector& v) {ocnLdV_ = v;};
  
  inline void setRefClockOffset(double d) {refClockOffset_ = d;};

  inline void setTsyses(const SgVector& ts);
  inline SgVector* tSyses() {return tSyses_;};
  inline SgVector& cableCorrections() {return cableCorrections_;};
  
  inline void setTapeId(const QString id) {tapeId_=id;};

  
  inline SgTroposphereModelData* tmdFlyBy() {return &tmdFlyBy_;};
  inline SgTroposphereModelData* tmdContribInternal() {return &tmdContribInternal_;};
  inline SgTroposphereModelData* tmdContribExternal() {return &tmdContribExternal_;};

  inline const SgTroposphereModelData& getTmdFlyBy() const {return tmdFlyBy_;};
  inline const SgTroposphereModelData& getTmdContribInternal() const {return tmdContribInternal_;};
  inline const SgTroposphereModelData& getTmdContribExternal() const {return tmdContribExternal_;};

  inline const SgTroposphereModelData*& tmdActual() {return tmdActual_;};
  inline const SgTroposphereModelData* getTmdActual() const {return tmdActual_;};


  
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  virtual inline const QString className() const;

  /**
   */
  virtual bool selfCheck();
  
  /**
   */
  virtual bool isEligible(const SgTaskConfig*);

  /**
   */
  virtual void prepare4Analysis(SgTaskManager*);

  /**
   */
  virtual void evaluateTheoreticalValues(SgTaskManager*);

  /**
   */
  virtual void evaluateResiduals(SgTaskManager*, SgVlbiBand*, bool);

  /**
   */
  virtual void evaluateResiduals(SgTaskManager*);

  /**
   */
  virtual const SgVector& o_c();

  /**
   */
  virtual const SgVector& sigma();

  /**
   */
  inline void resetCable();
  
  /**
   */
  inline void resetMeteo();
  
  /**
   */
  inline void resetTsys();
  
  /**
   */
  using SgObservation::operator==;
  inline virtual bool operator==(const SgVlbiAuxObservation& auxObs) const;

  /**
   */
  using SgObservation::operator!=;
  inline virtual bool operator!=(const SgVlbiAuxObservation& auxObs) const;

  inline void setTsysesSize(int n);
  //
  //
  inline double effectiveCableCalValue(CableCorrectionType idx) const
  {
    return cableCorrections_.getElement(idx);
  };



  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  SgVlbiObservation            *owner_;
  short int                     stationIdx_;
  SgMeteoData                   meteoData_;
  double                        cableCalibration_;
  double                        azimuthAngle_;
  double                        azimuthAngleRate_;
  double                        elevationAngle_;
  double                        elevationAngleRate_;
  double                        parallacticAngle_;
  double                        aprioriClocks_;
  double                        estClocks_;
  double                        estClocksSigma_;
  double                        estZenithDelay_;
  double                        estZenithDelaySigma_;
  double                        estAtmGradN_;
  double                        estAtmGradNSigma_;
  double                        estAtmGradE_;
  double                        estAtmGradESigma_;
  double                        zenithDelayH_;
  double                        zenithDelayW_;
  // CALC's products: station dependent contributions:
  double                        calcUnPhaseCal_;
  double                        calcAxisOffset4Delay_;
  double                        calcAxisOffset4Rate_;
  double                        calcOLoadHorz4Delay_;
  double                        calcOLoadHorz4Rate_;
  double                        calcOLoadVert4Delay_;
  double                        calcOLoadVert4Rate_;
  double                        calcNdryCont4Delay_;
  double                        calcNdryCont4Rate_;
  double                        calcNwetCont4Delay_;
  double                        calcNwetCont4Rate_;
  Sg3dVector                    ocnLdR_;
  Sg3dVector                    ocnLdV_;
  // station dependent partials (mostly, form CALC):
  double                        dDel_dAxsOfs_;
  double                        dRat_dAxsOfs_;
  double                        dDel_dTzdDry_;
  double                        dRat_dTzdDry_;
  double                        dDel_dTzdWet_;
  double                        dRat_dTzdWet_;
  double                        dDel_dTzdGrdN_;
  double                        dRat_dTzdGrdN_;
  double                        dDel_dTzdGrdE_;
  double                        dRat_dTzdGrdE_;
  // KOMB contribution:
  double                        refClockOffset_;
  // 
  //
  // tsys:
  SgVector                     *tSyses_;
  // a set of cable cals: from logs, CDMS and PCMT (extension of cableCalibration_):
  SgVector                      cableCorrections_;
  //
  QString                       tapeId_;
  //
  //
  SgTroposphereModelData        tmdFlyBy_;
  SgTroposphereModelData        tmdContribInternal_;
  SgTroposphereModelData        tmdContribExternal_;
  const SgTroposphereModelData *tmdActual_;
  
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObservation inline members:                                                                       */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//


// A destructor:
inline SgVlbiAuxObservation::~SgVlbiAuxObservation()
{
  // nothing to do
  if (tSyses_)
  {
    delete tSyses_;
    tSyses_ = NULL; 
  };
};



//
// INTERFACES:
//
// Gets:
//
inline SgVlbiObservation* SgVlbiAuxObservation::getOwner() const
{
  return owner_;
};



// returns idx of St.#1
inline short int SgVlbiAuxObservation::getStationIdx() const
{
  return stationIdx_;
};



//
inline SgMeteoData& SgVlbiAuxObservation::meteoData()
{
  return meteoData_;
};



//
inline const SgMeteoData& SgVlbiAuxObservation::getMeteoData() const
{
  return meteoData_;
};



//
inline double SgVlbiAuxObservation::getCableCalibration() const
{
  return cableCalibration_;
};



//
inline double SgVlbiAuxObservation::getAzimuthAngle() const
{
  return azimuthAngle_;
};



//
inline double SgVlbiAuxObservation::getAzimuthAngleRate() const
{
  return azimuthAngleRate_;
};



//
inline double SgVlbiAuxObservation::getElevationAngle() const
{
  return elevationAngle_;
};



//
inline double SgVlbiAuxObservation::getElevationAngleRate() const
{
  return elevationAngleRate_;
};



//
inline double SgVlbiAuxObservation::getParallacticAngle() const
{
  return parallacticAngle_;
};



//
inline double SgVlbiAuxObservation::getAprioriClocks() const
{
  return aprioriClocks_;
};



//
inline double SgVlbiAuxObservation::getEstClocks() const
{
  return estClocks_;
};



//
inline double SgVlbiAuxObservation::getEstClocksSigma() const
{
  return estClocksSigma_;
};



//
inline double SgVlbiAuxObservation::getEstZenithDelay() const
{
  return estZenithDelay_;
};



//
inline double SgVlbiAuxObservation::getEstZenithDelaySigma() const
{
  return estZenithDelaySigma_;
};



//
inline double SgVlbiAuxObservation::getEstAtmGradN() const
{
  return estAtmGradN_;
};



//
inline double SgVlbiAuxObservation::getEstAtmGradNSigma() const
{
  return estAtmGradNSigma_;
};



//
inline double SgVlbiAuxObservation::getEstAtmGradE() const
{
  return estAtmGradE_;
};



//
inline double SgVlbiAuxObservation::getEstAtmGradESigma() const
{
  return estAtmGradESigma_;
};



//
inline double SgVlbiAuxObservation::getZenithDelayH() const
{
  return zenithDelayH_;
};



//
inline double SgVlbiAuxObservation::getZenithDelayW() const
{
  return zenithDelayW_;
};



//
inline double SgVlbiAuxObservation::getCalcNdryCont4Delay() const
{
  return calcNdryCont4Delay_;
};



//
inline double SgVlbiAuxObservation::getCalcNdryCont4Rate() const
{
  return calcNdryCont4Rate_;
};



//
inline double SgVlbiAuxObservation::getCalcNwetCont4Delay() const
{
  return calcNwetCont4Delay_;
};



//
inline double SgVlbiAuxObservation::getCalcNwetCont4Rate() const
{
  return calcNwetCont4Rate_;
};



//
inline double SgVlbiAuxObservation::getCalcUnPhaseCal() const
{
  return calcUnPhaseCal_;
};



//
inline double SgVlbiAuxObservation::getCalcAxisOffset4Delay() const
{
  return calcAxisOffset4Delay_;
};



//
inline double SgVlbiAuxObservation::getCalcAxisOffset4Rate() const
{
  return calcAxisOffset4Rate_;
};



//
inline double SgVlbiAuxObservation::getCalcOLoadHorz4Delay() const
{
  return calcOLoadHorz4Delay_;
};



//
inline double SgVlbiAuxObservation::getCalcOLoadHorz4Rate() const
{
  return calcOLoadHorz4Rate_;
};



//
inline double SgVlbiAuxObservation::getCalcOLoadVert4Delay() const
{
  return calcOLoadVert4Delay_;
};



//
inline double SgVlbiAuxObservation::getCalcOLoadVert4Rate() const
{
  return calcOLoadVert4Rate_;
};



//
inline double SgVlbiAuxObservation::getDdel_dAxsOfs() const
{
  return dDel_dAxsOfs_;
};



//
inline double SgVlbiAuxObservation::getDrat_dAxsOfs() const
{
  return dRat_dAxsOfs_;
};





// Sets:
//
inline void SgVlbiAuxObservation::setOwner(SgVlbiObservation* obs)
{
  owner_ = obs;
};



//
inline void SgVlbiAuxObservation::setStationIdx(short int idx)
{
  stationIdx_ = idx;
};



//
inline void SgVlbiAuxObservation::setMeteoData(const SgMeteoData& meteo)
{
  meteoData_ = meteo; 
};



//
inline void SgVlbiAuxObservation::setCableCalibration(double cable)
{
  cableCalibration_ = cable;
};



//
inline void SgVlbiAuxObservation::setAzimuthAngle(double angle)
{
  azimuthAngle_ = angle;
};



//
inline void SgVlbiAuxObservation::setAzimuthAngleRate(double rate)
{
  azimuthAngleRate_ = rate;
};



//
inline void SgVlbiAuxObservation::setElevationAngle(double angle)
{
  elevationAngle_ = angle;
};



//
inline void SgVlbiAuxObservation::setElevationAngleRate(double rate)
{
  elevationAngleRate_ = rate;
};



//
inline void SgVlbiAuxObservation::setParallacticAngle(double angle)
{
  parallacticAngle_ = angle;
};



//
inline void SgVlbiAuxObservation::setAprioriClocks(double clocks)
{
  aprioriClocks_ = clocks;
};



//
inline void SgVlbiAuxObservation::setEstClocks(double d)
{
  estClocks_ = d;
};



//
inline void SgVlbiAuxObservation::setEstClocksSigma(double e)
{
  estClocksSigma_ = e;
};



//
inline void SgVlbiAuxObservation::setEstZenithDelay(double d)
{
  estZenithDelay_ = d;
};



//
inline void SgVlbiAuxObservation::setEstZenithDelaySigma(double e)
{
  estZenithDelaySigma_ = e;
};



//
inline void SgVlbiAuxObservation::setEstAtmGradN(double g)
{
  estAtmGradN_ = g;
};



//
inline void SgVlbiAuxObservation::setEstAtmGradNSigma(double s)
{
  estAtmGradNSigma_ = s;
};



//  
inline void SgVlbiAuxObservation::setEstAtmGradE(double g)
{
  estAtmGradE_ = g;
};



//
inline void SgVlbiAuxObservation::setEstAtmGradESigma(double s)
{
  estAtmGradESigma_ = s;
};



//  
inline void SgVlbiAuxObservation::setZenithDelayH(double d)
{
  zenithDelayH_ = d;
};



//  
inline void SgVlbiAuxObservation::setZenithDelayW(double d)
{
  zenithDelayW_ = d;
};



//
inline void SgVlbiAuxObservation::setCalcNdryCont4Delay(double v)
{
  calcNdryCont4Delay_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcNdryCont4Rate(double v)
{
  calcNdryCont4Rate_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcNwetCont4Delay(double v)
{
  calcNwetCont4Delay_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcNwetCont4Rate(double v)
{
  calcNwetCont4Rate_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcUnPhaseCal(double v)
{
  calcUnPhaseCal_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcAxisOffset4Delay(double v)
{
  calcAxisOffset4Delay_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcAxisOffset4Rate(double v)
{
  calcAxisOffset4Rate_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcOLoadHorz4Delay(double v)
{
  calcOLoadHorz4Delay_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcOLoadHorz4Rate(double v)
{
  calcOLoadHorz4Rate_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcOLoadVert4Delay(double v)
{
  calcOLoadVert4Delay_ = v;
};



//
inline void SgVlbiAuxObservation::setCalcOLoadVert4Rate(double v)
{
  calcOLoadVert4Rate_ = v;
};



//
inline void SgVlbiAuxObservation::setDdel_dAxsOfs(double v)
{
  dDel_dAxsOfs_ = v;
};



//
inline void SgVlbiAuxObservation::setDrat_dAxsOfs(double v)
{
  dRat_dAxsOfs_ = v;
};



//
inline void SgVlbiAuxObservation::setTsyses(const SgVector& ts)
{
  if (!tSyses_)
    tSyses_ = new SgVector(ts);
  else
  {
    if (tSyses_->n() != ts.n())
      tSyses_->reSize(ts.n());
    *tSyses_ = ts;
  };
};







//
// FUNCTIONS:
//
//
//
inline const QString SgVlbiAuxObservation::className() const 
{
  return "SgVlbiAuxObservation";
};



//
inline void SgVlbiAuxObservation::resetCable()
{
  cableCalibration_ = 0.0; 
};



//
inline void SgVlbiAuxObservation::resetMeteo()
{
  meteoData_.setTemperature(0.0);
  meteoData_.setPressure(0.0);
  meteoData_.setRelativeHumidity(0.0);
};



//
inline void SgVlbiAuxObservation::resetTsys()
{
  if (tSyses_)
    tSyses_->clear();
};



//
inline bool SgVlbiAuxObservation::operator==(const SgVlbiAuxObservation& auxObs) const
{
  return SgObservation::operator==(auxObs) && 
    stationIdx_ == auxObs.getStationIdx() &&
    meteoData_ == auxObs.getMeteoData() &&
    cableCalibration_ == auxObs.getCableCalibration() &&
    azimuthAngle_ == auxObs.getAzimuthAngle() &&
    elevationAngle_ == auxObs.getElevationAngle() &&
    azimuthAngleRate_ == auxObs.getAzimuthAngleRate() &&
    elevationAngleRate_ == auxObs.getElevationAngleRate() &&
    parallacticAngle_ == auxObs.getParallacticAngle() &&
    aprioriClocks_ == auxObs.getAprioriClocks() &&
    tapeId_ == auxObs.getTapeId();
    // should we compare the estimated parameters here?
};



//
inline bool SgVlbiAuxObservation::operator!=(const SgVlbiAuxObservation& auxObs) const
{
  return !(*this==auxObs);
};



//
inline void SgVlbiAuxObservation::setTsysesSize(int n)
{
  if (!tSyses_)
    tSyses_ = new SgVector(n);
  else if ((int)tSyses_->n() != n)
    tSyses_->reSize(n);
};








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
#endif // SG_VLBI_AUX_OBSERVATION_H
