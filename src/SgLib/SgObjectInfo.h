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

#ifndef SG_OBJECT_INFO_H
#define SG_OBJECT_INFO_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QMap>
#include <QtCore/QString>

#include <SgAttribute.h>
#include <SgMJD.h>
#include <SgWrmsable.h>




class QDataStream;

enum SgObjectBrowseMode {OBM_BAND, OBM_SESSION, OBM_PROJECT};


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgObjectInfo : public SgAttribute
{
  friend class SgVlbiBaselineInfo;
public:
  // Statics:
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an object.
   */
  inline SgObjectInfo(int idx=-1, const QString& key="Unknown", const QString& aka="");

  inline SgObjectInfo(const SgObjectInfo&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgObjectInfo();



  //
  // Interfaces:
  //
  /**Returns a key.
   */
  inline const QString& getKey() const;

  /**Returns an AKA.
   */
  inline const QString& getAka() const;

  /**Returns an index of the object.
   */
  inline int getIdx() const;

  /**Returns total number of observations.
   */
  inline int numTotal(DataType dType) const;

  /**Returns number of processed observations.
   */
  inline int numProcessed(DataType dType) const;

  /**Returns number of potentially good (recoverable) observations.
   */
  inline int numUsable(DataType) const;

  inline double getSigma2add(DataType) const;

  /**Returns MJD of the first observation related to the object.
   */
  inline const SgMJD& tFirst(DataType) const;

  /**Returns MJD of the last observation related to the object.
   */
  inline const SgMJD& tLast(DataType) const;

  /**Returns normalized residuals.
   */
  inline double normedResid(DataType) const;

  /**Returns normalized residuals for spoolfile output -- SOLVE version.
   */
  inline double getSFF_NrmRes(DataType) const;

  /**Returns aux normalized residuals for spoolfile output -- SOLVE version.
   */
  inline double getSFF_NrmRes_plus_15(DataType) const;

  /**Returns (non-reduced) Chi-squared for the object.
   */
  inline double chi2(DataType) const;

  /**Returns reduced Chi-squared for the object.
   */
  inline double reducedChi2(DataType) const;

  /**Returns coefficient to scale a weight.
   */
  inline double weightScaleCoefficient(DataType) const;

  /**Returns dispersion of residuals for the object.
   */
  inline double dispersion(DataType, bool=false) const;

  /**Sets a key.
   * \param srcKey -- new key;
   */
  inline void setKey(const QString& key);

  /**Sets up an "Also known as" field.
   * \param aka -- new AKA;
   */
  inline void setAka(const QString& aka);

  /**Sets up an index.
   * \param idx -- new index;
   */
  inline void setIdx(int idx);

  /**Sets number of potentially good (recoverable) observations.
   * \param n -- number of recoverable observations;
   */
  inline void setNumUsable(DataType dType, int n);

  /**Sets value of normalized residuals.
   * \param d -- normalized residulas;
   */
  inline void setNormedResid(DataType dType, double d);

  inline void setSigma2add(DataType dType, double d);

  /**Sets value of normalized residuals for spoolfile output -- SOLVE version.
   * \param d -- normalized residulas;
   */
  inline void setSFF_NrmRes(DataType dType, double d);

  /**Sets value of aux normalized residuals for spoolfile output -- SOLVE version.
   * \param d -- normalized residulas;
   */
  inline void setSFF_NrmRes_plus_15(DataType dType, double d);



  //
  // Functions:
  //
  /**Increments total number of observations.
   */
  inline void incNumTotal(DataType, int=1);

  /**Decrements total number of observations.
   */
  inline void decNumTotal(DataType, int=1);

  inline void incNumUsable(DataType, int=1);
  inline void decNumUsable(DataType, int=1);
  
  
  /**Adds a residual to the accumulated W.R.M.S. statistics.
   * \param o_c -- the residual;
   * \param w -- its weight (usually, it is sigma^(-2));
   * \param auxSum -- the additional sigma);
   */
  inline void addWrms(DataType, const SgMJD& t, double o_c, double w, double auxSum);

  /**Calculates and returns W.R.M.S. for the object.
   */
  inline double wrms(DataType) const;

  inline double dof(DataType) const;

  /**Clears accumulated W.R.M.S. statistics.
   */
  inline void clearWrms();

  /**Gets statistics from other object.
   */
  inline void copyStatistics(SgObjectInfo*);

  inline void copySigmas2add(SgObjectInfo*);

  inline void resetAllEditings();

  void recalcSigma2add(const SgTaskConfig* cfg);

  inline void calcNormalizedResiduals(const QList<SgVlbiObservable*> &observables, 
    const SgTaskConfig* cfg);

  //
  // Friends:
  //

  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);



private:
  // identifications:
  QString                       key_;     //! a key -- a string of chars that identifies an object;
  QString                       aka_;     //! "also known as", a second name or an alias;
  int                           idx_;     //! local (session) index;
  SgWrmsable                    delayWrms_;
  SgWrmsable                    rateWrms_;
  //
  SgWrmsable                   *wrmss[2];
  QMap<DataType, int>           dType2Idx_;
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
// An empty constructor:
inline SgObjectInfo::SgObjectInfo(int idx, const QString& key, const QString& aka) :
  SgAttribute(),
  key_(key),
  aka_(aka),
  delayWrms_(DT_DELAY),
  rateWrms_(DT_RATE),
  dType2Idx_()
{
  idx_ = idx;
  dType2Idx_[DT_DELAY] = 0;
  dType2Idx_[DT_RATE ] = 1;
  wrmss[dType2Idx_[DT_DELAY]] = &delayWrms_;
  wrmss[dType2Idx_[DT_RATE ]] = &rateWrms_;
};



// A copy constructor:
inline SgObjectInfo::SgObjectInfo(const SgObjectInfo& obj) :
  SgAttribute(obj),
  key_(obj.getKey()),
  aka_(obj.getAka()),
  delayWrms_(DT_DELAY),
  rateWrms_(DT_RATE),
  dType2Idx_()
{
  idx_ = obj.getIdx();
  dType2Idx_[DT_DELAY] = 0;
  dType2Idx_[DT_RATE ] = 1;
  wrmss[dType2Idx_[DT_DELAY]] = &delayWrms_;
  wrmss[dType2Idx_[DT_RATE ]] = &rateWrms_;
};





// A destructor:
inline SgObjectInfo::~SgObjectInfo()
{
  // nothing to do
};



//
// INTERFACES:
//
//
inline const QString& SgObjectInfo::getKey() const 
{
  return key_;
};



//
inline const QString& SgObjectInfo::getAka() const
{
  return aka_;
};



//
inline int SgObjectInfo::getIdx() const
{
  return idx_;
};



//
inline int SgObjectInfo::numTotal(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getNumTotal();
};



//
inline int SgObjectInfo::numProcessed(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getNumProcessed();
};



//
inline int SgObjectInfo::numUsable(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getNumUsable();
};



//
inline double SgObjectInfo::getSigma2add(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getSigma2add();
};



//
inline const SgMJD& SgObjectInfo::tFirst(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->tFirst();
};



//
inline const SgMJD& SgObjectInfo::tLast(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->tLast();
};



//
inline double SgObjectInfo::normedResid(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getNormedResid();
};



//
inline double SgObjectInfo::getSFF_NrmRes(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getSFF_NrmRes();
};



//
inline double SgObjectInfo::getSFF_NrmRes_plus_15(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->getSFF_NrmRes_plus_15();
};



//
inline double SgObjectInfo::chi2(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->chi2();
};



//
inline double SgObjectInfo::reducedChi2(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->reducedChi2();
};



//
inline double SgObjectInfo::weightScaleCoefficient(DataType dType) const
{
  return wrmss[dType2Idx_[dType]]->weightScaleCoefficient();
};



//
inline double SgObjectInfo::dispersion(DataType dType, bool isSolveCompatible) const
{
  return wrmss[dType2Idx_[dType]]->dispersion(isSolveCompatible);
};



//
inline void SgObjectInfo::setKey(const QString& key)
{
  key_ = key;
};



//
inline void SgObjectInfo::setAka(const QString& aka)
{
  aka_ = aka;
};



//
inline void SgObjectInfo::setIdx(int idx)
{
  idx_ = idx;
};



//
inline void SgObjectInfo::setSigma2add(DataType dType, double s)
{
  return wrmss[dType2Idx_[dType]]->setSigma2add(s);
};



//
inline void SgObjectInfo::setNumUsable(DataType dType, int n)
{
  wrmss[dType2Idx_[dType]]->setNumUsable(n);
};



//
inline void SgObjectInfo::setNormedResid(DataType dType, double d)
{
  wrmss[dType2Idx_[dType]]->setNormedResid(d);
};



//
inline void SgObjectInfo::setSFF_NrmRes(DataType dType, double d)
{
  wrmss[dType2Idx_[dType]]->setSFF_NrmRes(d);
};



//
inline void SgObjectInfo::setSFF_NrmRes_plus_15(DataType dType, double d)
{
  wrmss[dType2Idx_[dType]]->setSFF_NrmRes_plus_15(d);
};







//
// FUNCTIONS:
//
//
//
inline void SgObjectInfo::incNumTotal(DataType dType, int n)
{
  wrmss[dType2Idx_[dType]]->incNumTotal(n);
};



//
inline void SgObjectInfo::decNumTotal(DataType dType, int n)
{
  wrmss[dType2Idx_[dType]]->decNumTotal(n);
};


//
inline void SgObjectInfo::incNumUsable(DataType dType, int n)
{
  wrmss[dType2Idx_[dType]]->incNumUsable(n);
};



//
inline void SgObjectInfo::decNumUsable(DataType dType, int n)
{
  wrmss[dType2Idx_[dType]]->decNumUsable(n);
};



//
inline void SgObjectInfo::clearWrms()
{
  delayWrms_.clearWrms();
  rateWrms_ .clearWrms();
};
  


//
inline void SgObjectInfo::addWrms(DataType dType, const SgMJD &t, double o_c, double w, double auxSum)
{
  wrmss[dType2Idx_[dType]]->addWrms(t, o_c, w, auxSum);
};



//
inline double SgObjectInfo::wrms(DataType dType) const 
{
  return wrmss[dType2Idx_[dType]]->wrms();
};



//
inline double SgObjectInfo::dof(DataType dType) const 
{
  return wrmss[dType2Idx_[dType]]->dof();
};



//
inline void SgObjectInfo::copyStatistics(SgObjectInfo* info)
{
  if (info)
  {
    delayWrms_.copyStatistics(&info->delayWrms_);
    rateWrms_ .copyStatistics(&info->rateWrms_ );
  };
};



//
inline void SgObjectInfo::resetAllEditings()
{
  delayWrms_.resetAllEditings();
  rateWrms_ .resetAllEditings();
};



//
inline void SgObjectInfo::copySigmas2add(SgObjectInfo *info)
{
  if (info)
  {
    delayWrms_.setSigma2add(info->delayWrms_.getSigma2add());
    rateWrms_ .setSigma2add(info->rateWrms_ .getSigma2add());
  };
};



//
#include <SgTaskConfig.h>

inline void SgObjectInfo::calcNormalizedResiduals(const QList<SgVlbiObservable*> &observables,
  const SgTaskConfig* cfg)
{
  delayWrms_.calcNormalizedResiduals(observables, cfg);
  rateWrms_ .calcNormalizedResiduals(observables, cfg);
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
#endif //SG_OBJECT_INFO_H
