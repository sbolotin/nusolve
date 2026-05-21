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

#ifndef SG_VLBI_SOURCE_INFO_H
#define SG_VLBI_SOURCE_INFO_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QMap>

#include <SgObjectInfo.h>
#include <SgParameter.h>


class QDataStream;

class SgParametersDescriptor;


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
// typedefs:
class SgVlbiSourceInfo;
typedef QMap<QString, SgVlbiSourceInfo*> SourcesByName;
typedef QMap<QString, SgVlbiSourceInfo*>::iterator SourcesByName_it;
//
//
class SgVlbiSourceInfo : public SgObjectInfo
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<<0, //!< omit the source;
    Attr_ESTIMATE_COO       = 1<<1, //!< estimate source position;
    Attr_ESTIMATE_VEL       = 1<<2, //!< estimate source velocity;
    Attr_CONSTRAIN_COO      = 1<<3, //!< constrain source position in estimation;
    Attr_CONSTRAIN_VEL      = 1<<4, //!< constrain source velocity in estimation;
    Attr_APPLY_SSM          = 1<<5, //!< apply the source structure model;
    // ...
    Attr_HAS_A_PRIORI_POS   = 1<<10,//!< indicates that the source has a priori position;
    Attr_TEST               = 1<<31,//!< for testing purposes.
  };
  //
  //
  class StructModel
  {
  public:
    inline StructModel(double k, double b, double x, double y) {k_=k; b_=b; x_=x; y_=y; 
      kSig_=xSig_=ySig_=bSig_=0.0;};
    inline ~StructModel() {};
    inline double getK() const {return k_;};
    inline double getKsig() const {return kSig_;};
    inline double getB() const {return b_;};
    inline double getBsig() const {return bSig_;};
    inline double getX() const {return x_;};
    inline double getXsig() const {return xSig_;};
    inline double getY() const {return y_;};
    inline double getYsig() const {return ySig_;};
    inline void setK(double d) {k_ = d;};
    inline void setKsig(double d) {kSig_ = d;};
    inline void setB(double d) {b_ = d;};
    inline void setBsig(double d) {bSig_ = d;};
    inline void setX(double d) {x_ = d;};
    inline void setXsig(double d) {xSig_ = d;};
    inline void setY(double d) {y_ = d;};
    inline void setYsig(double d) {ySig_ = d;};
    double tauS(double f, double u, double v) const;
  private:
    double                      k_;
    double                      kSig_;
    double                      b_;
    double                      bSig_;
    double                      x_;
    double                      xSig_;
    double                      y_;
    double                      ySig_;
  };
  class StructModelMp : public StructModel 
  {
  public:
    inline StructModelMp(double k, double b, double x, double y, 
      bool estK=true, bool estB=true, bool estCoo=true) : StructModel(k, b, x, y)
      {pK_=pB_=pX_=pY_=NULL; estimateRatio_=estK; estimateSpIdx_=estB; estimatePosition_=estCoo;};
    inline ~StructModelMp() {if (pK_) {delete pK_; pK_=NULL;}; if (pB_) {delete pB_; pB_=NULL;};
      if (pX_) {delete pX_; pX_=NULL;}; if (pY_) {delete pY_; pY_=NULL;};};
    inline SgParameter* &pK() {return pK_;};
    inline SgParameter* &pB() {return pB_;};
    inline SgParameter* &pX() {return pX_;};
    inline SgParameter* &pY() {return pY_;};
    inline double getR() const {return dR_;};
    inline double getS() const {return dS_;};
    inline double getC() const {return dC_;};
    inline bool getEstimateRatio() const {return estimateRatio_;};
    inline bool getEstimateSpIdx() const {return estimateSpIdx_;};
    inline bool getEstimatePosition() const {return estimatePosition_;};
    inline void setEstimateRatio(bool b) {estimateRatio_ = b;};
    inline void setEstimateSpIdx(bool b) {estimateSpIdx_ = b;};
    inline void setEstimatePosition(bool b) {estimatePosition_ = b;};
    void prepareModel(double u, double v);
  private:
    SgParameter                *pK_;
    SgParameter                *pB_;
    SgParameter                *pX_;
    SgParameter                *pY_;
    double                      dR_; // R_l
    double                      dS_; // S_l
    double                      dC_; // C_l
    bool                        estimateRatio_;
    bool                        estimateSpIdx_;
    bool                        estimatePosition_;
  };
  
  //
  //
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  static double arcLength(double ra_1, double dn_1, double ra_2, double dn_2);
  
  static QString ra2String(double ra);
  
  static QString dn2String(double dn, bool mandatorySign=false);

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  SgVlbiSourceInfo(int idx=-1, const QString& key="Unknown", const QString& aka="Unknown too");

  /**A constructor.
   * Creates a copy of an object.
   */
  SgVlbiSourceInfo(const SgVlbiSourceInfo&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgVlbiSourceInfo();



  //
  // Interfaces:
  //
  inline const QString& getAprioriReference() const {return aprioriReference_;};
  inline const QString& getAprioriComments() const {return aprioriComments_;};
  inline int getTotalScanNum() const {return totalScanNum_;};
  inline int getProcdScanNum() const {return procdScanNum_;};

  /**Returns a pointer on the parameter: src coordinates: Right Ascension.
   */
  inline SgParameter* pRA();

  /**Returns a pointer on the parameter: src coordinates: Declination North.
   */
  inline SgParameter* pDN();




  /**Returns a value of source's right ascension.
   */
  inline double getRA() const;

  /**Returns a value of source's declination.
   */
  inline double getDN() const;

  /**Returns a value of source's right ascension (ext. a priori file value).
   */
  inline double getRA_ea() const;

  /**Returns a value of source's declination (ext. a priori file value).
   */
  inline double getDN_ea() const;

  inline double getAl2ExtA() const {return al2ExtA_;};
  inline double getAl2Estd() const {return al2Estd_;};






  // sets:
  inline void setAprioriReference(const QString& ref) {aprioriReference_ = ref;};
  inline void setAprioriComments(const QString& str) {aprioriComments_ = str;};
  
  /**Sets right ascension of the source.
   * \param  -- ra new value of the right ascension;
   */
  inline void setRA(double ra);

  /**Sets declination of the source.
   * \param  -- dn new value of the right ascension;
   */
  inline void setDN(double dn);

  /**Sets right ascension of the source (ext. a priori file value).
   * \param  -- ra new value of the right ascension;
   */
  inline void setRA_ea(double ra);

  /**Sets declination of the source (ext. a priori file value).
   * \param  -- dn new value of the right ascension;
   */
  inline void setDN_ea(double dn);

  inline void setAl2ExtA(double l) {al2ExtA_ = l;};
  inline void setAl2Estd(double l) {al2Estd_ = l;};

  //
  inline void setTotalScanNum(int n) {totalScanNum_ = n;};
  inline void setProcdScanNum(int n) {procdScanNum_ = n;};

  //

  //
  // Functions:
  //
  /**Allocates necessary memory for estimated parameters.
   */
  void createParameters();

  /**Frees previously allocated memory for estimated parameters.
   */
  void releaseParameters();
  
//  double calcR(double u, double v);
  double tauS(double f, double u, double v);

//  double dTauS_dK(double f, double u, double v);
//  double dTauS_dB(double f, double u, double v);
//  double dTauS_dR(double f, double u, double v);
//  double dTauS_dX(double f, double u, double v);
//  double dTauS_dY(double f, double u, double v);

//  double dTauS_dA0(double f, double u, double v);
//  double dTauS_dB0(double f, double u, double v);
//  double dTauS_dTheta0(double f, double u, double v);
//  double dTauS_dA1(double f, double u, double v);
//  double dTauS_dB1(double f, double u, double v);
//  double dTauS_dTheta1(double f, double u, double v);

  //double dTauS_dKj(int j, double f, double u, double v);
  void processPartials4SrcStructModel(QList<SgParameter*>& parameters, double f, double u, double v);
  
  //
  inline void incTotalScanNum(int n=1) {totalScanNum_ += n;};
  inline void incProcdScanNum(int n=1) {procdScanNum_ += n;};
  
  inline void resetAllEditings();
  
  inline bool getIsSsModelEstimating() const {return isSsModelEstimating_;};
  inline void setIsSsModelEstimating(bool is) {isSsModelEstimating_ = is;};
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);

  //
  short calculateStatusBits();
  void applyStatusBits(short bitArray);

private:
  //
  // :
  QString                       aprioriReference_;
  QString                       aprioriComments_;
  int                           totalScanNum_;  //! total number of scans for the source;
  int                           procdScanNum_;  //! number of processed scans for the source;
  
  // temporary place for the coordinates:
  double                        RA_;
  double                        DN_;
  // temporary place for the coordinates (from external a priori file):
  double                        RA_ea_;
  double                        DN_ea_;
  // arc length:
  double                        al2ExtA_; // length of arc from database a priori to external a priori
  double                        al2Estd_; // length of arc from a priori to estimated position

  // temporary place for the parameters:
  SgParameter                  *pRA_;
  SgParameter                  *pDN_;

  // multiple point:
  QList<StructModelMp>          sModel_;
  bool                          isSsModelEstimating_;
public:
  inline QList<StructModelMp>& sModel() {return sModel_;};
  inline const QList<StructModelMp>& sModel() const {return sModel_;};
  inline void addSrcStructPoint(double k, double b, double x, double y,
    bool estK=false, bool estB=false, bool estR=false)
    {sModel_.append(SgVlbiSourceInfo::StructModelMp(k, b, x, y, estK, estB, estR));};
  inline void clearSrcStructPoints() {sModel_.clear();};
  int calcNumOfEstimatedSsmParameters() const;
  
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgObjectInfo inline members:                                                                        */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// A destructor:
inline SgVlbiSourceInfo::~SgVlbiSourceInfo()
{
  releaseParameters();
  sModel_.clear();
};



//
// INTERFACES:
//
// 
inline SgParameter* SgVlbiSourceInfo::pRA()
{
  return pRA_;
};



//
inline SgParameter* SgVlbiSourceInfo::pDN()
{
  return pDN_;
};



//
inline double SgVlbiSourceInfo::getRA() const
{
  return RA_;
};



//
inline double SgVlbiSourceInfo::getDN() const
{
  return DN_;
};



//
inline double SgVlbiSourceInfo::getRA_ea() const
{
  return RA_ea_;
};



//
inline double SgVlbiSourceInfo::getDN_ea() const
{
  return DN_ea_;
};



//
inline void SgVlbiSourceInfo::setRA(double ra)
{
  RA_ = ra;
};



//
inline void SgVlbiSourceInfo::setDN(double dn)
{
  DN_ = dn;
};


//
inline void SgVlbiSourceInfo::setRA_ea(double ra)
{
  RA_ea_ = ra;
};



//
inline void SgVlbiSourceInfo::setDN_ea(double dn)
{
  DN_ea_ = dn;
};


//
// FUNCTIONS:
//
//
//
//
inline void SgVlbiSourceInfo::resetAllEditings()
{
  SgObjectInfo::resetAllEditings();
  procdScanNum_ = 0;
  delAttr(Attr_NOT_VALID);
  delAttr(Attr_ESTIMATE_COO);
  delAttr(Attr_ESTIMATE_VEL);
  delAttr(Attr_CONSTRAIN_COO);
  delAttr(Attr_CONSTRAIN_VEL);
  delAttr(Attr_APPLY_SSM);
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
#endif //SG_VLBI_SOURCE_INFO_H
