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

#ifndef SG_MODELS_INFO_H
#define SG_MODELS_INFO_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>


#include <QtCore/QMap>

#include <SgAttribute.h>
#include <SgMJD.h>


class SgMatrix;
class SgVector;
class SgVlbiStationInfo;

/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
//
//
class SgModelsInfo
{
public:
  // "Sie ist ein Model und sie sieht gut aus
  // ich nehm sie heut gerne mit zu mir nach Haus..."
  class DasModel
  {
  public:
    inline DasModel(const QString& key) :
      key_(key), controlFlag_("Undefined"), definition_("Undefined"), origin_("Undefined") {};
    inline ~DasModel() {};
    //
    inline const QString& getKey() const {return key_;};
    inline const QString& getControlFlag() const {return controlFlag_;};
    inline const QString& getDefinition() const {return definition_;};
    inline const QString& getOrigin() const {return origin_;};
    //
    inline void setKey(const QString& key) {key_ = key;};
    inline void setControlFlag(const QString& flag) {controlFlag_ = flag;};
    inline void setDefinition(const QString& def) {definition_ = def;};
    inline void setOrigin(const QString& orig) {origin_ = orig;};
    //
  protected:
    QString                     key_;            // type of name
    QString                     controlFlag_;
    QString                     definition_;
    QString                     origin_;
  };


  // Statics:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgModelsInfo();

  /**A constructor.
   * Creates a copy of the object.
   */

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgModelsInfo();


  //
  // Interfaces:
  //
  
  // gets:
  inline const QString& getProgramName() const {return programName_;};
  inline double getDversion() const {return dVersion_;};
  inline const QString& getVersion() const {return version_;};
  inline short getFlagTidalUt1() const {return flagTidalUt1_;};
  inline double getRelativityData() const {return relativityData_;};
  inline double getPrecessionData() const {return precessionData_;};
  inline double getEarthTideData(int i) const {return 0<=i&&i<=2?earthTideData_[i]:0.0;};

  // sets:
  inline void setProgramName(const QString& name) {programName_ = name;};
  inline void setDversion(double d) {dVersion_ = d;};
  inline void setVersion(const QString& ver) {version_ = ver;};
  inline void setFlagTidalUt1(short f) {flagTidalUt1_ = f;};
  inline void setRelativityData(double d) {relativityData_ = d;};
  inline void setPrecessionData(double d) {precessionData_ = d;};
  inline void setEarthTideData(int i, double d) {if (0<=i && i<=2) earthTideData_[i] = d;};

  //
  inline DasModel& site() {return  site_;};
  inline DasModel& star() {return  star_;};
  inline DasModel& ephemeris() {return  ephemeris_;};
  inline DasModel& coordinateTime() {return  coordinateTime_;};
  inline DasModel& atomicTime() {return  atomicTime_;};
  inline DasModel& relativity() {return  relativity_;};
  inline DasModel& polarMotion() {return  polarMotion_;};
  inline DasModel& polarMotionInterpolation() {return  polarMotionInterpolation_;};
  inline DasModel& ut1() {return  ut1_;};
  inline DasModel& ut1Interpolation() {return  ut1Interpolation_;};
  inline DasModel& nutation() {return  nutation_;};
  inline DasModel& earthTide() {return  earthTide_;};
  inline DasModel& poleTide() {return  poleTide_;};
  inline DasModel& oceanLoading() {return  oceanLoading_;};
  inline DasModel& troposphere() {return troposphere_;};
  inline DasModel& axisOffset() {return  axisOffset_;};
  inline DasModel& parallax() {return  parallax_;};
  inline DasModel& feedHorn() {return  feedHorn_;};
  inline QList<QString>& controlFlagNames() {return  controlFlagNames_;};
  inline QList<int>& controlFlagValues() {return controlFlagValues_;};
  inline QList<QString>& oLoadStationStatus() {return oLoadStationStatus_;};
  inline QList<double>& siteZenDelays() {return siteZenDelays_;};
  inline SgMatrix*& ut1InterpData() {return ut1InterpData_;};
  inline SgMatrix*& wobInterpData() {return wobInterpData_;};
  inline QList<SgVlbiStationInfo*>& stations() {return stations_;};



  inline const DasModel& site() const {return  site_;};
  inline const DasModel& star() const {return  star_;};
  inline const DasModel& ephemeris() const {return  ephemeris_;};
  inline const DasModel& coordinateTime() const {return  coordinateTime_;};
  inline const DasModel& atomicTime() const {return  atomicTime_;};
  inline const DasModel& relativity() const {return  relativity_;};
  inline const DasModel& polarMotion() const {return  polarMotion_;};
  inline const DasModel& polarMotionInterpolation() const {return  polarMotionInterpolation_;};
  inline const DasModel& ut1() const {return  ut1_;};
  inline const DasModel& ut1Interpolation() const {return  ut1Interpolation_;};
  inline const DasModel& nutation() const {return  nutation_;};
  inline const DasModel& earthTide() const {return  earthTide_;};
  inline const DasModel& poleTide() const {return  poleTide_;};
  inline const DasModel& oceanLoading() const {return  oceanLoading_;};
  inline const DasModel& troposphere() const {return troposphere_;};
  inline const DasModel& axisOffset() const {return  axisOffset_;};
  inline const DasModel& parallax() const {return  parallax_;};
  inline const DasModel& feedHorn() const {return  feedHorn_;};
  inline const QList<QString>& controlFlagNames() const {return  controlFlagNames_;};
  inline const QList<int>& controlFlagValues() const {return controlFlagValues_;};
  inline const QList<QString>& oLoadStationStatus() const {return oLoadStationStatus_;};
  inline const QList<double>& siteZenDelays() const {return siteZenDelays_;};
  inline const SgMatrix* ut1InterpData() const {return ut1InterpData_;};
  inline const SgMatrix* wobInterpData() const {return wobInterpData_;};
  inline const QList<SgVlbiStationInfo*> stations() const {return stations_;};
  
  void prepareUt1InterpData(int nRow);
  void prepareWobInterpData(int nRow);


  //
  // Functions:


  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

protected:
  QString             					programName_;     //!< name of the software that provided models
  double           							dVersion_;
  QString             					version_; 
  DasModel            					site_;
  DasModel            					star_;
  DasModel            					ephemeris_;
  DasModel            					coordinateTime_;
  DasModel            					atomicTime_;
  DasModel            					relativity_;
  DasModel            					polarMotion_;
  DasModel            					polarMotionInterpolation_;
  DasModel            					ut1_;
  DasModel            					ut1Interpolation_;
  DasModel            					nutation_;
  DasModel            					earthTide_;
  DasModel            					poleTide_;
  DasModel            					oceanLoading_;
  DasModel            					troposphere_;
  DasModel            					axisOffset_;
  DasModel            					parallax_;
  DasModel            					feedHorn_;
  QList<QString>      					controlFlagNames_;
  QList<int>          					controlFlagValues_;
  QList<QString>      					oLoadStationStatus_;
  QList<double>       					siteZenDelays_;
  
  SgMatrix           					 *ut1InterpData_;
  SgMatrix           					 *wobInterpData_;
  short                					flagTidalUt1_;
  double               					relativityData_;
  double               					precessionData_;
  double               					earthTideData_[3];

  QList<SgVlbiStationInfo*>			stations_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiSessionInfo inline members:                                                                   */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// INTERFACES:
//
//


//
// FUNCTIONS:
//
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
#endif // SG_MODELS_INFO_H
