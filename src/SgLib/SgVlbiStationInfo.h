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

#ifndef SG_VLBI_STATION_INFO_H
#define SG_VLBI_STATION_INFO_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>


#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QVector>


#include <Sg3dVector.h>
#include <SgBreakModel.h>
#include <SgParameter.h>
#include <SgObjectInfo.h>
#include <SgVlbiAuxObservation.h>


class QDataStream;
class SgEccRec;
class SgRefraction;
class SgVlbiStationInfo;



// "...
// Tracking station 43, Canberra, come in Canberra! Tracking station 63, can
// you hear me, Madrid? Can anybody hear me? Come in, come in..."
//



#define MAX_NUMBER_OF_POLYNOMIALS  10

/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
typedef QMap<QString, SgVlbiStationInfo*> StationsByName;
typedef QMap<QString, SgVlbiStationInfo*>::iterator StationsByName_it;
//
//
class SgVlbiStationInfo : public SgObjectInfo
{
public:
  enum Attributes
  {
    Attr_NOT_VALID              = 1<< 0, //!< omit the station;
    Attr_ESTIMATE_COO           = 1<< 1, //!< estimate station position;
    Attr_ESTIMATE_VEL           = 1<< 2, //!< estimate station velocity;
    Attr_CONSTRAIN_COO          = 1<< 3, //!< constrain station position in estimation;
    Attr_CONSTRAIN_VEL          = 1<< 4, //!< constrain station velocity in estimation;
    Attr_REFERENCE_CLOCKS       = 1<< 5, //!< the clocks are the reference ones;
    Attr_IGNORE_CABLE_CAL       = 1<< 6, //!< do not apply cable calibration;
    Attr_INVERSE_CABLE          = 1<< 7, //!< cable calibration correction has wrong sign;
    Attr_BAD_METEO              = 1<< 8, //!< indicates that the station has bad meteo parameters;
    Attr_ART_METEO              = 1<< 9, //!< indicates that the station has artificial meteo parameters;
    Attr_HAS_A_PRIORI_POS       = 1<<10, //!< indicates that the station has a priori r and v;
    Attr_HAS_A_PRIORI_AXO       = 1<<11, //!< indicates that the station has a priori axis offset;
    Attr_DONT_ESTIMATE_TRPS     = 1<<12, //!< do not estimate parameters of troposphere delay for station;
    Attr_USE_LOCAL_CLOCKS       = 1<<13, //!< use its own setup for the clock parameter;
    Attr_USE_LOCAL_ZENITH       = 1<<14, //!< use its own setup for the zenith delay parameter;
    Attr_ESTIMATE_AXO           = 1<<15, //!< estimate axis offset;
    Attr_HAS_CABLE_CAL          = 1<<16, //!< a station has cable calibration readings;
    Attr_HAS_METEO              = 1<<17, //!< a station has meteo parameters;
    Attr_CABLE_CAL_MODIFIED     = 1<<18, //!< a station has cable calibration readings;
    Attr_METEO_MODIFIED         = 1<<19, //!< a station has meteo parameters;
    Attr_CABLE_CAL_IS_CDMS      = 1<<20, //!< FS log file contains /CDMS/ readings instead of /cable/;
    Attr_HAS_TSYS               = 1<<21, //!<
    Attr_TSYS_MODIFIED          = 1<<22, //!<
    Attr_APRIORI_CLOCK_MODIFIED = 1<<23, //!<
    Attr_HAS_CCC_FSLG           = 1<<24, //!< a station has cable calibrations from FS log file
    Attr_HAS_CCC_CDMS           = 1<<25, //!< a station has cable calibrations from CDMS
    Attr_HAS_CCC_PCMT           = 1<<26, //!< a station has cable calibrations from PCMT
    Attr_HAS_MANUAL_PCAL        = 1<<27, //!< a station has cable calibrations from PCMT
  };

  // temporary place:
  enum MountingType
  {
    MT_AZEL         = 0,
    MT_EQUA         = 1,
    MT_X_YN         = 2,
    MT_X_YE         = 3,
    MT_RICHMOND     = 4,
    MT_UNKN         = 5
  };
  enum CableCalsOrigin
  {
    CCO_UNDEF       = 0,
    CCO_FS_LOG      = 1,
    CCO_CDMS        = 2,
    CCO_PCMT        = 3,
    CCO_CDMS_FS_LOG = 4,
  };
  enum MeteoDataOrigin
  {
    MDO_UNDEF       = 0,
    MDO_FS_LOG      = 1,
    MDO_EXTERNAL    = 2,
  };
  // 
  enum NetPolarization // we need this to create fourfit command for refringing
  {
    NP_UNDEF                 = 0,
    NP_CircPolarizRight      = 1<<0,
    NP_CircPolarizLeft       = 1<<1,
    NP_LinearPolarizX        = 1<<2,
    NP_LinearPolarizY        = 1<<3,
    NP_LinearPolarizH        = 1<<4, // should not happened
    NP_LinearPolarizV        = 1<<5, //     --"--
  };




  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  static int mntType2int(MountingType);
  static MountingType int2mntType(int);
  
  static const int maxNumOfPolynomials_;
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  SgVlbiStationInfo(int idx=-1, const QString& key="Unknown", const QString& aka="Unknown too");

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgVlbiStationInfo();



  //
  // Interfaces:
  //
  /**Returns a reference on the clock break model object.
   */
  inline SgBreakModel& clockBreaks();
  inline const SgBreakModel& clockBreaks() const;

  /**Returns a pointer on the container (a map) with the auxiliary observations.
   */
  inline QMap<QString, SgVlbiAuxObservation*>* auxObservationByScanId();
  inline const QMap<QString, SgVlbiAuxObservation*>* auxObservationByScanId() const;

  /**Returns a const reference on the station coordinates vector.
   */
  inline const Sg3dVector& getR();
  
  /**Returns a const reference on the station coordinates vector from external a priori file.
   */
  inline const Sg3dVector& getR_ea();
  
  /**Returns a const reference on the station velocity vector from external a priori file.
   */
  inline const Sg3dVector& getV_ea();
  
  /**Returns an order of clocks behavior polynomial model.
   */
  inline int getClocksModelOrder() const;

  /**Returns a multiplier for cable calibration correction (usually, +1 or -1).
   */
  inline double getCableCalMultiplier() const;
 
  inline SgTaskConfig::CableCalSource getFlybyCableCalSource() const {return flybyCableCalSource_;};

  /**Returns the original (from dbcal) multiplier for cable calibration correction (+1 or -1).
   */
  inline double getCableCalMultiplierDBCal() const;

  /**Returns the description of the cable calibration correction origin (a file name).
   */
  inline const QString& getCableCalsOriginTxt() const {return cableCalsOriginTxt_;};

  /**Returns the description of the meteo parameters origin (a file name).
   */
  inline const QString& getMeteoDataOriginTxt() const {return meteoDataOriginTxt_;};

  /**Returns the cable calibration correction origin.
   */
  inline CableCalsOrigin getCableCalsOrigin() const {return cableCalsOrigin_;};

  /**Returns the meteo parameters origin.
   */
  inline MeteoDataOrigin getMeteoDataOrigin() const {return meteoDataOrigin_;};

  /**Returns a value of axis offset, meters.
   */
  inline double getAxisOffset() const;

  /**Returns a value of axis offset from external a priori file, meters.
   */
  inline double getAxisOffset_ea() const;

  inline double getTilt(int i) const {return tilt_[0<=i&&i<2?i:0];};

  /**Returns a value of a priori mean north gradient.
   */
  inline double getGradNorth() const;

  /**Returns a value of a priori mean east gradient.
   */
  inline double getGradEast() const;

  /**Returns a type of antenna mounting.
   */
  inline MountingType getMntType() const;

  /**Returns a latitude of the station.
   */
  inline double getLatitude() const;

  /**Returns a longitude of the station.
   */
  inline double getLongitude() const;

  /**Returns height above geoid of the station.
   */
  inline double getHeight() const;

  /**Returns name of corresponding tectonic plate.
   */
  inline const QString& getTectonicPlateName() const;

  /**Returns CDP number.
   */
  inline int getCdpNumber() const;

  /**Returns the value for idx-th elementh in clock model.
   */
  inline double getEstClockModel(int idx) const;

  /**Returns the sigma for idx-th elementh in clock model.
   */
  inline double getEstClockModelSigma(int idx) const;

  /**Returns the value for wet zenith delay.
   */
  inline double getEstWetZenithDelay() const;

  /**Returns the sigma for wet zenith delay.
   */
  inline double getEstWetZenithDelaySigma() const;

  inline double getAPrioriClockTerm_0() const;

  inline double getAPrioriClockTerm_1() const;

  inline bool getNeed2useAPrioriClocks() const;

  /**Returns the value for an amplitude [in m] of ocean loading component.
   * \param iWave -- the index of a wave;
   * \param iCoord -- the index of a coordinate; here 0 is "Up", 1 is "West" and 2 is "South";
   */
  inline double getOLoadAmplitude(int iWave, int iCoord) const;
  inline double getOptLoadCoeff(int idx) const;

  /**Returns the value for a phase [in rad] of ocean loading component.
   * \param iWave -- the index of a wave;
   * \param iCoord -- the index of a coordinate; here 0 is "Up", 1 is "West" and 2 is "South";
   */
  inline double getOLoadPhase(int iWave, int iCoord) const;

  inline bool getIsEccNonZero() const {return isEccNonZero_;};
  
  inline const Sg3dVector& getV3Ecc() const {return v3Ecc_;};
  
  inline double getGradientDelay() const {return gradientDelay_;};

  inline const QVector<double>& getTsysIfFreqs() const {return tsysIfFreqs_;};
  inline const QVector<QString>& getTsysIfIds() const {return tsysIfIds_;};
  inline const QVector<QString>& getTsysIfSideBands() const {return tsysIfSideBands_;};
  inline const QVector<QString>& getTsysIfPolarizations() const {return tsysIfPolarizations_;};
  
  inline const SgParameterCfg& getPcClocks() const {return pcClocks_;};
  inline const SgParameterCfg& getPcZenith() const {return pcZenith_;};
  inline int maxNumOfPolynomials() const {return maxNumOfPolynomials_;};

  inline char getCid() const {return cId_;};
  inline const char* getSid() const {return sId_;};

  inline const QMap<QString, int>& numByPolarization() const {return numByPolarization_;};
  inline unsigned int getNetPolarizations() const {return netPolarizations_;};


  // sets:
  //
  /**Sets an order for polynomial model of clocks behavior.
   * \param n -- an order;
   */
  inline void setClocksModelOrder(int n);

  /**Sets a multiplier for cable calibration correction (usually, +1 or -1).
   * \param c -- a multiplier;
   */
  inline void setCableCalMultiplier(double c);

  inline void setFlybyCableCalSource(SgTaskConfig::CableCalSource s) {flybyCableCalSource_ = s;};

  /**Sets a original (from dbcal) multiplier for cable calibration correction (+1 or -1).
   * \param c -- a multiplier;
   */
  inline void setCableCalMultiplierDBCal(double c);

  /**Sets the description of the cable calibration correction origin.
   */
  inline void setCableCalsOriginTxt(const QString& origin) {cableCalsOriginTxt_ = origin;};

  /**Sets the description of the meteo parameters origin.
   */
  inline void setMeteoDataOriginTxt(const QString& origin) {meteoDataOriginTxt_ = origin;};

  /**Sets the cable calibration correction origin.
   */
  inline void setCableCalsOrigin(CableCalsOrigin origin) {cableCalsOrigin_ = origin;};

  /**Sets the meteo parameters origin.
   */
  inline void setMeteoDataOrigin(MeteoDataOrigin origin) {meteoDataOrigin_ = origin;};

  /**Sets a reference on the station coordinates vector.
   * \param r -- a vector;
   */
  inline void setR(const Sg3dVector& r);

  /**Sets a reference on the station coordinates vector from external a priori file.
   * \param r -- a vector;
   */
  inline void setR_ea(const Sg3dVector& r);

  /**Sets a reference on the station velocity vector from external a priori file.
   * \param v -- a vector;
   */
  inline void setV_ea(const Sg3dVector& v);

  /**Sets a value of axis offset.
   * \param v -- new value;
   */
  inline void setAxisOffset(double v);

  /**Sets a value of axis offset from ext. a priori file.
   * \param v -- new value;
   */
  inline void setAxisOffset_ea(double v);
  
  inline void setTilt(int i, double v) {tilt_[0<=i&&i<2?i:0] = v;};


  /**Sets a value of a priori north gradient.
   * \param v -- new value;
   */
  inline void setGradNorth(double v);

  /**Sets a value of a priori east gradient.
   * \param v -- new value;
   */
  inline void setGradEast(double v);

  /**Sets a type of antenna mounting.
   * \param m -- mounting type;
   */
  inline void setMntType(MountingType m);

  /**Sets up a name of corresponding tectonic plate.
   * \param name -- the name of the plate;
   */
  inline void setTectonicPlateName(const QString& name);

  /**Sets up a CDP number of the station.
   * \param cdp -- the CDP number;
   */
  inline void setCdpNumber(int cdp);

  /**Sets an element of the saved clock model behavior.
   * \param idx -- the index of the elementh;
   * \param  d -- the value;
   */
  inline void setEstClockModel(int idx, double d, double s);

  /**Sets the value for wet zenith delay.
   * \param  d -- the value;
   */
  inline void setEstWetZenithDelay(double d, double s);

  inline void setAPrioriClockTerm_0(double);

  inline void setAPrioriClockTerm_1(double);

  inline void setNeed2useAPrioriClocks(bool);

  /**Sets the value for an amplitude of ocean loading component.
   * \param iWave -- the index of a wave;
   * \param iCoord -- the index of a coordinate; here 0 is "Up", 1 is "West" and 2 is "South";
   * \param amplitude -- the amplitude [m].
   */
  inline void setOLoadAmplitude(int iWave, int iCoord, double amplitude);
  inline void setOptLoadCoeff(int idx, double c);

  /**Sets the value for a phase of ocean loading component.
   * \param iWave -- the index of a wave;
   * \param iCoord -- the index of a coordinate; here 0 is "Up", 1 is "West" and 2 is "South";
   * \param phase -- the phase [rad].
   */
  inline void setOLoadPhase(int iWave, int iCoord, double phase);
  
  inline void setGradientDelay(double d) {gradientDelay_ = d;};

  inline QVector<double>& tsysIfFreqs() {return tsysIfFreqs_;};
  inline QVector<QString>& tsysIfIds() {return tsysIfIds_;};
  inline QVector<QString>& tsysIfSideBands() {return tsysIfSideBands_;};
  inline QVector<QString>& tsysIfPolarizations() {return tsysIfPolarizations_;};

  inline void setPcClocks(const SgParameterCfg& pc) {pcClocks_ = pc;};
  inline void setPcZenith(const SgParameterCfg& pc) {pcZenith_ = pc;};

  inline void setCid(char c) {cId_=c;};
  inline void setSid(char c1, char c2) {sId_[0]=c1; sId_[1]=c2; sId_[2]=0;};


  // acess to pointers:
  // 
  /**Returns a pointer on the parameter: clock zero order.
   */
  inline SgParameter* pClock0();

  /**Returns a pointer on the parameter: clock first order.
   */
  inline SgParameter* pClock1();

  /**Returns a pointer on the parameter: clock second order.
   */
  inline SgParameter* pClock2();

  /**Returns a pointer on the parameter: clock third order.
   */
  inline SgParameter* pClock3();

  /**Returns a pointer on the parameter: clock i-th order.
   * \param i -- an order of the term;
   */
  inline SgParameter* pClocks(int i);

  /**Returns a pointer on the parameter: zenith delay.
   */
  inline SgParameter* pZenithDelay();

  /**Returns a const pointer on the parameter: zenith delay.
   */
  inline const SgParameter* getPZenithDelay() const;

  /**Returns a pointer on the parameter: atmospheric gradient (north).
   */
  inline SgParameter* pAtmGradN();

  /**Returns a pointer on the parameter: atmospheric gradient (east).
   */
  inline SgParameter* pAtmGradE();

  /**Returns a pointer on the parameter: x-coordinates.
   */
  inline SgParameter* pRx();

  /**Returns a pointer on the parameter: y-coordinates.
   */
  inline SgParameter* pRy();

  /**Returns a pointer on the parameter: z-coordinates.
   */
  inline SgParameter* pRz();

  /**Returns a pointer on the parameter: axis offset.
   */
  inline SgParameter* pAxisOffset();
  
  inline SgEccRec*& eccRec();

  inline SgParameterCfg* pcClocks() {return &pcClocks_;};
  inline SgParameterCfg* pcZenith() {return &pcZenith_;};

  inline QMap<QString, int>& numByPolarization() {return numByPolarization_;};
  inline void setNetPolarization(unsigned int np) {netPolarizations_ = np;};
  inline void addNetPolarization(unsigned int np) {netPolarizations_ |= np;};
  
  //
  // Functions:
  //
  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters();

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();

  /**Clears (sets to zero) estimated clock model coefficients.
   */
  inline void clearEstClockModel();

  /**Clears (sets to zero) estimated wet zenith delay correction.
   */
  inline void clearEstWetZenithDelay();

  /**Returns an effective multiplier for cable calibration correction (usually, +1 or -1).
   * It is accounting for "Bad cable calibration" flag.
   */
  inline double effectiveCableCalMultiplier() const;

  /**Calculates export clock breaks epochs.
   */
  void calcCBEpochs4Export(const QMap<QString, SgVlbiAuxObservation*>&);
  
  //  Sg3dVector  r(const SgMJD&, SgTaskConfig*);

  void adjustEccVector();
  
  void recalcRLF(bool isSolveCompatible);
  
  void checkAuxObs(const QMap<QString, QString>& scanNameById);

  inline void resetAllEditings();
  
  inline QString name() const {return getKey().simplified();};
  
  void resetCable();
  void resetMeteo();
  void resetTsys();
  inline void resetRefractionModel() {refractionModel_ = SgTaskConfig::RM_NONE;};
  
  void checkPresenceOfCableCalibratioCorrections();
  
  inline SgVlbiAuxObservation::CableCorrectionType cccIdx() const {return cccIdx_;};
  
  void prepare2Run(const SgTaskConfig* cfg, SgRefraction*);

  
  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //


private:
  // clocks model:
  int                                   clocksModelOrder_;
  // "working" cable sign:
  double                                cableCalMultiplier_;
  // source of cable cals:
  SgTaskConfig::CableCalSource          flybyCableCalSource_;
  SgTaskConfig::RefractionModel         refractionModel_;
  // original cable sign from dbcal:
  double                                cableCalMultiplierDBCal_;
  SgBreakModel                          clockBreaks_;
  QMap<QString, SgVlbiAuxObservation*>  auxObservationByScan_;
  QString                               cableCalsOriginTxt_;
  QString                               meteoDataOriginTxt_;
  CableCalsOrigin                       cableCalsOrigin_;
  MeteoDataOrigin                       meteoDataOrigin_;
  SgVlbiAuxObservation::CableCorrectionType
                                        cccIdx_;

  // temporary place:
  Sg3dVector                            r_;
  // external a priori position:
  Sg3dVector                            r_ea_;
  // external a priori velocity:
  Sg3dVector                            v_ea_;
  double                                axisOffset_;
  // external a priori axis offset:
  double                                axisOffset_ea_;
  // a priori mean gradients (all of them a "external", so can skip "_ea" suffix):
  double                                tilt_[2];
  double                                gradNorth_;
  double                                gradEast_;
  MountingType                          mntType_;
  double                                latitude_, longitude_, height_;
  QString                               tectonicPlateName_;
  int                                   cdpNumber_;
  SgEccRec                             *eccRec_;
  bool                                  isEccNonZero_;
  Sg3dVector                            v3Ecc_;
  // AKAs:
  char                                  cId_;     // one-char site ID (mostly for compatibility of IO)
  char                                  sId_[3];  // two-chars site ID plus null-terminator
  
  // temporary place for ocean loading coeff:
  double                                oLoadAmplitudes_[11][3];
  double                                oLoadPhases_[11][3];
  // temporary place for ocean pole tide loading coeff:
  double                                optLoadCoeffs_[6];
  // saved estimations:
  double                                estClockModel_[MAX_NUMBER_OF_POLYNOMIALS];
  double                                estClockModelSigmas_[MAX_NUMBER_OF_POLYNOMIALS];
  double                                estWetZenithDelay_;
  double                                estWetZenithDelaySigma_;
  double                                aPrioriClockTerm_0_;
  double                                aPrioriClockTerm_1_;
  bool                                  need2useAPrioriClocks_;
  // saved intermediate values:
  double                                gradientDelay_;
  //
  QVector<double>                       tsysIfFreqs_;
  QVector<QString>                      tsysIfIds_;
  QVector<QString>                      tsysIfSideBands_;
  QVector<QString>                      tsysIfPolarizations_;

  // temporary place for the parameters:
  SgParameter                          *pClocks_[MAX_NUMBER_OF_POLYNOMIALS];
  SgParameter                          *pZenithDelay_;
  SgParameter                          *pAtmGradN_;
  SgParameter                          *pAtmGradE_;
  SgParameter                          *pRx_;
  SgParameter                          *pRy_;
  SgParameter                          *pRz_;
  SgParameter                          *pAxisOffset_;
  
  // peculiar parameter setup:
  SgParameterCfg                        pcClocks_;
  SgParameterCfg                        pcZenith_;
  //
  QMap<QString, int>                    numByPolarization_;
  unsigned int                          netPolarizations_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiStationInfo inline members:                                                                   */
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
inline SgBreakModel& SgVlbiStationInfo::clockBreaks()
{
  return clockBreaks_;
};



//
inline const SgBreakModel& SgVlbiStationInfo::clockBreaks() const
{
  return clockBreaks_;
};



//
inline QMap<QString, SgVlbiAuxObservation*>* SgVlbiStationInfo::auxObservationByScanId()
{
  return &auxObservationByScan_;
};



//
inline const QMap<QString, SgVlbiAuxObservation*>* SgVlbiStationInfo::auxObservationByScanId() const
{
  return &auxObservationByScan_;
};



//
inline const Sg3dVector& SgVlbiStationInfo::getR()
{
  return r_;
};



//
inline const Sg3dVector& SgVlbiStationInfo::getR_ea()
{
  return r_ea_;
};



//
inline const Sg3dVector& SgVlbiStationInfo::getV_ea()
{
  return v_ea_;
};



// returns number of MJDays
inline int SgVlbiStationInfo::getClocksModelOrder() const 
{
  return clocksModelOrder_;
};



//
inline double SgVlbiStationInfo::getCableCalMultiplier() const
{
  return cableCalMultiplier_;
};



//
inline double SgVlbiStationInfo::getCableCalMultiplierDBCal() const
{
  return cableCalMultiplierDBCal_;
};



//
inline double SgVlbiStationInfo::getAxisOffset() const
{
  return axisOffset_;
};



//
inline double SgVlbiStationInfo::getAxisOffset_ea() const
{
  return axisOffset_ea_;
};



//
inline double SgVlbiStationInfo::getGradNorth() const
{
  return gradNorth_;
};



//
inline double SgVlbiStationInfo::getGradEast() const
{
  return gradEast_;
};



//
inline SgVlbiStationInfo::MountingType SgVlbiStationInfo::getMntType() const
{
  return mntType_;
};



//
inline double SgVlbiStationInfo::getLatitude() const
{
  return latitude_;
};



//
inline double SgVlbiStationInfo::getLongitude() const
{
  return longitude_;
};



//
inline double SgVlbiStationInfo::getHeight() const
{
  return height_;
};



//
inline const QString& SgVlbiStationInfo::getTectonicPlateName() const
{
  return tectonicPlateName_;
};



//
inline int SgVlbiStationInfo::getCdpNumber() const
{
  return cdpNumber_;
};



//
inline double SgVlbiStationInfo::getEstClockModel(int idx) const
{
  return estClockModel_[idx<maxNumOfPolynomials_ ? idx : 0];
};



//
inline double SgVlbiStationInfo::getEstClockModelSigma(int idx) const
{
  return estClockModelSigmas_[idx<maxNumOfPolynomials_ ? idx : 0];
};



//
inline double SgVlbiStationInfo::getEstWetZenithDelay() const
{
  return estWetZenithDelay_;
};



//
inline double SgVlbiStationInfo::getEstWetZenithDelaySigma() const
{
  return estWetZenithDelaySigma_;
};



//
inline double SgVlbiStationInfo::getAPrioriClockTerm_0() const
{
  return aPrioriClockTerm_0_;
};



//
inline double SgVlbiStationInfo::getAPrioriClockTerm_1() const
{
  return aPrioriClockTerm_1_;
};



//
inline bool SgVlbiStationInfo::getNeed2useAPrioriClocks() const
{
  return need2useAPrioriClocks_;
};



//
inline double SgVlbiStationInfo::getOLoadAmplitude(int iWave, int iCoord) const
{
  return oLoadAmplitudes_[iWave][iCoord];
};



//
inline double SgVlbiStationInfo::getOptLoadCoeff(int idx) const
{
  return optLoadCoeffs_[(-1<idx && idx<6)?idx:0];
};



//
inline double SgVlbiStationInfo::getOLoadPhase(int iWave, int iCoord) const
{
  return oLoadPhases_[iWave][iCoord];
};



//
inline void SgVlbiStationInfo::setClocksModelOrder(int n)
{
  clocksModelOrder_ = n;
};



//
inline void SgVlbiStationInfo::setCableCalMultiplier(double c)
{
  cableCalMultiplier_ = c;
};



//
inline void SgVlbiStationInfo::setCableCalMultiplierDBCal(double c)
{
  cableCalMultiplierDBCal_ = c;
};



//
inline void SgVlbiStationInfo::setR(const Sg3dVector& r)
{
  r_ = r;
  if (r_.module()>6000.0) // meters
    geocentric2geodetic(r_, latitude_, longitude_, height_, true);
  if (height_<-1000.0)
    std::cout << "Warning: r set for " << qPrintable(getKey()) << "; h: " << height_
              << "; r: " << r <<  "\n";
};



//
inline void SgVlbiStationInfo::setR_ea(const Sg3dVector& r)
{
  r_ea_ = r;
  /*
  if (r_ea_.module()>6000.0) // meters
    calcRLF(latitude_, longitude_, height_, r_ea_, false);
  */
};



inline void SgVlbiStationInfo::recalcRLF(bool isSolveCompatible)
{
  geocentric2geodetic(r_, latitude_, longitude_, height_, isSolveCompatible);
};



//
inline void SgVlbiStationInfo::setV_ea(const Sg3dVector& v)
{
  v_ea_ = v;
};



//
inline void SgVlbiStationInfo::setAxisOffset(double v)
{
  axisOffset_ = v;
};



//
inline void SgVlbiStationInfo::setAxisOffset_ea(double v)
{
  axisOffset_ea_ = v;
};



//
inline void SgVlbiStationInfo::setGradNorth(double v)
{
  gradNorth_ = v;
};



//
inline void SgVlbiStationInfo::setGradEast(double v)
{
  gradEast_ = v;
};



//
inline void SgVlbiStationInfo::setMntType(SgVlbiStationInfo::MountingType m)
{
  mntType_ = m;
};



//
inline void SgVlbiStationInfo::setTectonicPlateName(const QString& name)
{
  tectonicPlateName_ = name;
};



//
inline void SgVlbiStationInfo::setCdpNumber(int cdp)
{
  cdpNumber_ = cdp;
};



//
inline void SgVlbiStationInfo::setEstClockModel(int idx, double d, double s)
{
  estClockModel_[idx<maxNumOfPolynomials_ ? idx : 0] = d;
  estClockModelSigmas_[idx<maxNumOfPolynomials_ ? idx : 0] = s;
};



//
inline void SgVlbiStationInfo::setEstWetZenithDelay(double d, double s)
{
  estWetZenithDelay_ = d;
  estWetZenithDelaySigma_ = s;
};



//
inline void SgVlbiStationInfo::setAPrioriClockTerm_0(double v)
{
  aPrioriClockTerm_0_ = v;
};



//
inline void SgVlbiStationInfo::setAPrioriClockTerm_1(double v)
{
  aPrioriClockTerm_1_ = v;
};



//
inline void SgVlbiStationInfo::setNeed2useAPrioriClocks(bool need2)
{
  need2useAPrioriClocks_ = need2;
};



//
inline void SgVlbiStationInfo::setOLoadAmplitude(int iWave, int iCoord, double amplitude)
{
  oLoadAmplitudes_[iWave][iCoord] = amplitude;
};



//
inline void SgVlbiStationInfo::setOptLoadCoeff(int idx, double c)
{
  if (-1<idx && idx<6)
    optLoadCoeffs_[idx] = c;
};



//
inline void SgVlbiStationInfo::setOLoadPhase(int iWave, int iCoord, double phase)
{
  oLoadPhases_[iWave][iCoord] = phase;
};



//
inline SgParameter* SgVlbiStationInfo::pClock0()
{
  return pClocks_[0];
};



//
inline SgParameter* SgVlbiStationInfo::pClock1()
{
  return pClocks_[1];
};



//
inline SgParameter* SgVlbiStationInfo::pClock2()
{
  return pClocks_[2];
};



//
inline SgParameter* SgVlbiStationInfo::pClock3()
{
  return pClocks_[3];
};



//
inline SgParameter* SgVlbiStationInfo::pClocks(int i)
{
  return pClocks_[i<maxNumOfPolynomials_ ? i : 0];
};



//
inline SgParameter* SgVlbiStationInfo::pZenithDelay()
{
  return pZenithDelay_;
};



//
inline const SgParameter* SgVlbiStationInfo::getPZenithDelay() const
{
  return pZenithDelay_;
};



//
inline SgParameter* SgVlbiStationInfo::pAtmGradN()
{
  return pAtmGradN_;
};



//
inline SgParameter* SgVlbiStationInfo::pAtmGradE()
{
  return pAtmGradE_;
};



//
inline SgParameter* SgVlbiStationInfo::pRx()
{
  return pRx_;
};



//
inline SgParameter* SgVlbiStationInfo::pRy()
{
  return pRy_;
};



//
inline SgParameter* SgVlbiStationInfo::pRz()
{
  return pRz_;
};



//
inline SgParameter* SgVlbiStationInfo::pAxisOffset()
{
  return pAxisOffset_;
};



//
inline SgEccRec*& SgVlbiStationInfo::eccRec() 
{
  return eccRec_;
};



//
// FUNCTIONS:
//
//
//
inline void SgVlbiStationInfo::clearEstClockModel()
{
  for (int i=0; i<maxNumOfPolynomials_; i++)
  {
    estClockModel_[i] = 0.0;
    estClockModelSigmas_[i] = 0.0;
  };
};



//
inline void SgVlbiStationInfo::clearEstWetZenithDelay()
{
  estWetZenithDelay_ = 0.0;
  estWetZenithDelaySigma_ = 0.0;
};



//
inline double SgVlbiStationInfo::effectiveCableCalMultiplier() const
{
  return isAttr(Attr_IGNORE_CABLE_CAL)?0.0:
    (isAttr(Attr_INVERSE_CABLE)?-cableCalMultiplier_:cableCalMultiplier_);
};



//
inline void SgVlbiStationInfo::resetAllEditings()
{
  SgObjectInfo::resetAllEditings();
  clockBreaks_.resetAllEditings();

  clocksModelOrder_ = 2;
  clearEstClockModel();
  clearEstWetZenithDelay();
  aPrioriClockTerm_0_ = 0.0;
  aPrioriClockTerm_1_ = 0.0;
  need2useAPrioriClocks_ = false;
  gradientDelay_ = 0.0;
  flybyCableCalSource_  = SgTaskConfig::CCS_DEFAULT;
  refractionModel_      = SgTaskConfig::RM_NONE;

  delAttr(Attr_NOT_VALID);
  delAttr(Attr_ESTIMATE_VEL);
  delAttr(Attr_CONSTRAIN_COO);
  delAttr(Attr_CONSTRAIN_VEL);
  delAttr(Attr_REFERENCE_CLOCKS);
//delAttr(Attr_IGNORE_CABLE_CAL);
  delAttr(Attr_INVERSE_CABLE);
  delAttr(Attr_DONT_ESTIMATE_TRPS);
  delAttr(Attr_USE_LOCAL_CLOCKS);
  delAttr(Attr_USE_LOCAL_ZENITH);
  addAttr(Attr_ESTIMATE_COO);

  if (isAttr(Attr_HAS_CABLE_CAL))
    delAttr(Attr_IGNORE_CABLE_CAL);
  else
    addAttr(Attr_IGNORE_CABLE_CAL);
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
QString netPolarizations2string(unsigned int);

/*=====================================================================================================*/
#endif //SG_VLBI_STATION_INFO_H
