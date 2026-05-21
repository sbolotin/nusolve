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

#ifndef SG_VLBI_BAND_H
#define SG_VLBI_BAND_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <math.h>

#include <QtCore/QList>
#include <QtCore/QMap>

class QDataStream;

#include <SgVlbiBaselineInfo.h>
#include <SgVlbiHistory.h>
#include <SgVlbiStationInfo.h>
#include <SgVlbiSourceInfo.h>



class SgVlbiObservable;

/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgVlbiBand : public SgObjectInfo
{
public:
  enum Attributes
  {
    Attr_NOT_VALID          = 1<<0, //!< omit these data;
    Attr_PRIMARY            = 1<<1, //!< the band is a primary band;
    Attr_HAS_IONO_SOLUTION  = 1<<2, //!< the band contains ionospheric corrections;
    Attr_HAS_AMBIGS         = 1<<3, //!< the band contains group delays and ambiguities;
    Attr_MISSING_CHANNEL_INFO
                            = 1<<4, //!< the band lacks info on channel setup;
    Attr_CHANNEL_INFO_MODIFIED
                            = 1<<5, //!< channel setup was updated;
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgVlbiBand();

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgVlbiBand();



  //
  // Interfaces:
  //
  // direct access:
  /**Returns a pointer on a observation list.
   */
  inline QList<SgVlbiObservable*>& observables();
  
  /**Returns a pointer on a map: stationInfo by idx.
   */
  inline QMap<int, SgVlbiStationInfo*>& stationsByIdx();

  /**Returns a pointer on a map: basleineInfo by idx.
   */
  inline QMap<int, SgVlbiBaselineInfo*>& baselinesByIdx();

  /**Returns a pointer on a map: sourceInfo by idx.
   */
  inline QMap<int, SgVlbiSourceInfo*>& sourcesByIdx();

  inline QMap<QString, SgVector*>& phCalOffset_1ByBln() {return phCalOffset_1ByBln_;};
  inline QMap<QString, SgVector*>& phCalOffset_2ByBln() {return phCalOffset_2ByBln_;};


  // gets:
  /**Returns frequency of the band.
   */
  inline double getFrequency() const;
 
  /**Returns epoch of the creation of the input file.
   */
  inline const SgMJD& getTCreation() const;

  /**Returns name of the input file.
   */
  inline const QString& getInputFileName() const;

  /**Returns version of the input file.
   */
  inline int getInputFileVersion() const;
  
  /**Returns type of the correlator that processed the session.
   */
  inline const QString& getCorrelatorType() const;

  /**Returns a reference on history list (read/write access).
   */
  inline SgVlbiHistory& history();
  inline const SgVlbiHistory& history() const {return history_;};

  /**Returns a const reference on set (actually, a map: name => station info) of stations info.
   */
  inline QMap<QString, SgVlbiStationInfo*>& stationsByName();

  /**Returns a const reference on set (actually, a map: name => baseline info) of baselines info.
   */
  inline QMap<QString, SgVlbiBaselineInfo*>& baselinesByName();

  /**Returns a const reference on set (actually, a map: name => source info) of sources info.
   */
  inline QMap<QString, SgVlbiSourceInfo*>& sourcesByName();

  /**Returns number of channels.
   */
  inline int getMaxNumOfChannels() const;
  inline double sampleRate() const {return sampleRate_;};
  inline int bitsPerSample() const {return bitsPerSample_;};
  inline double meanEffectiveIntegrationTime() const {return meanEffectiveIntegrationTime_;};
  inline const QMap<double, int>& sampleRateByCount() const {return sampleRateByCount_;};
  inline const QMap<double, int>& channelBandwidthByCount() const {return channelBandwidthByCount_;};
  inline const QMap<int, int>& bitsPerSampleByCount() const {return bitsPerSampleByCount_;};
  inline const QString& recordingMode() const {return recordingMode_;};


  /**Returns number of applied equations of constraints at the last run.
   */
  inline int getNumOfConstraints() const;

  inline const QMap<double, int>& grdAmbigsBySpacing() const {return grdAmbigsBySpacing_;};
  inline double typicalGrdAmbigSpacing() const {return typicalGrdAmbigSpacing_;};
  inline const QString& strGrdAmbigsStat() const {return strGrdAmbigsStat_;};

  inline const QMap<double, int>& phdAmbigsBySpacing() const {return phdAmbigsBySpacing_;};
  inline double typicalPhdAmbigSpacing() const {return typicalPhdAmbigSpacing_;};
  inline const QString& strPhdAmbigsStat() const {return strPhdAmbigsStat_;};


  // sets:
  /**Sets up frequency of the band.
   * \param idx -- the index;
   */
  inline void setFrequency(double f);

  /**Sets up epoch of the creation of input file.
   */
  inline void setTCreation(const SgMJD& t);

  /**Sets up a file name from which the data were imported.
   * \param fileName -- a file name;
   */
  inline void setInputFileName(const QString& fileName);

  /**Sets up a input database version.
   * \param version -- a number of version;
   */
  inline void setInputFileVersion(int version);

  /**Sets up correlator type for the session.
   */
  inline void setCorrelatorType(const QString& name);

  /**Sets up number of channels.
   * \param num -- a number of channels;
   */
  inline void setMaxNumOfChannels(int num);


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  inline const QString className() const;

  inline bool operator<(const SgVlbiBand& band) const;
  
  /** Returns stationInfo object for the index (if exists, otherwice NULL).
   */
  inline SgVlbiStationInfo* lookupStationByIdx(int);

  /** Returns sourceInfo object for the index (if exists).
   */
  inline SgVlbiSourceInfo* lookupSourceByIdx(int);

  /** Returns baselineInfo object for the index (if exists).
   */
  inline SgVlbiBaselineInfo* lookupBaselineByIdx(int);
  
  void resetAllEditings();
  
  bool selfCheck();
  
  bool checkHistory();
  
  SgMJD lastHistoryEntry(); 
  SgMJD calcHistoryEntry(); 
  
  void propagateChannelBandwidth(double bw);


  //
  // I/O:
  //
  bool saveIntermediateResults(QDataStream&) const;

  bool loadIntermediateResults(QDataStream&);
  //

private:
  //
  double                                frequency_;         // a reference frequency of the band
  // links to observations (not an owner):
  QList<SgVlbiObservable*>              observables_;
  // statistics & info:
  SgMJD                                 tCreation_;         // each band could has its own creation time
  QString                               inputFileName_;     // the same for file name
  int                                   inputFileVersion_;  // version of the DBH file
  QString                               correlatorType_;    // Correlator type
  SgVlbiHistory                         history_;
  QMap<QString, SgVlbiStationInfo*>     stationsByName_;
  QMap<QString, SgVlbiBaselineInfo*>    baselinesByName_;
  QMap<QString, SgVlbiSourceInfo*>      sourcesByName_;
  // just for quick look-ups, not the owners:
  QMap<int, SgVlbiStationInfo*>         stationsByIdx_;
  QMap<int, SgVlbiBaselineInfo*>        baselinesByIdx_;
  QMap<int, SgVlbiSourceInfo*>          sourcesByIdx_;
  //
  int                                   maxNumOfChannels_;
  double                                sampleRate_;
  int                                   bitsPerSample_;
  double                                meanEffectiveIntegrationTime_;
  QMap<double, int>                     sampleRateByCount_;
  QMap<double, int>                     channelBandwidthByCount_;
  QMap<int, int>                        bitsPerSampleByCount_;
  QString                               recordingMode_;

  //
  // 4group delay:
  QMap<double, int>                     grdAmbigsBySpacing_;
  double                                typicalGrdAmbigSpacing_;
  QString                               strGrdAmbigsStat_;
  // 4phase delay:
  QMap<double, int>                     phdAmbigsBySpacing_;
  double                                typicalPhdAmbigSpacing_;
  QString                               strPhdAmbigsStat_;
  // support stuff:
  QMap<QString, SgVector*>              phCalOffset_1ByBln_;
  QMap<QString, SgVector*>              phCalOffset_2ByBln_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgVlbiBand inline members:                                                                          */
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
inline QList<SgVlbiObservable*>& SgVlbiBand::observables()
{
  return observables_;
};



//
inline QMap<int, SgVlbiStationInfo*>& SgVlbiBand::stationsByIdx()
{
  return stationsByIdx_;
};



//
inline QMap<int, SgVlbiBaselineInfo*>& SgVlbiBand::baselinesByIdx()
{
  return baselinesByIdx_;
};



//
inline QMap<int, SgVlbiSourceInfo*>& SgVlbiBand::sourcesByIdx()
{
  return sourcesByIdx_;
};



//
inline double SgVlbiBand::getFrequency() const
{
  return frequency_;
};



//
inline const SgMJD& SgVlbiBand::getTCreation() const
{
  return tCreation_;
};



//
inline const QString& SgVlbiBand::getInputFileName() const
{
  return inputFileName_;
};



//
inline int SgVlbiBand::getInputFileVersion() const
{
  return inputFileVersion_;
};



//
inline const QString& SgVlbiBand::getCorrelatorType() const
{
  return correlatorType_;
};



//
inline SgVlbiHistory& SgVlbiBand::history()
{
  return history_;
};



//
inline QMap<QString, SgVlbiStationInfo*>& SgVlbiBand::stationsByName()
{
  return stationsByName_;
};



//
inline QMap<QString, SgVlbiBaselineInfo*>& SgVlbiBand::baselinesByName()
{
  return baselinesByName_;
};



//
inline QMap<QString, SgVlbiSourceInfo*>& SgVlbiBand::sourcesByName()
{
  return sourcesByName_;
};



//
inline int SgVlbiBand::getMaxNumOfChannels() const
{
  return maxNumOfChannels_;
};



// sets:

//
inline void SgVlbiBand::setFrequency(double f)
{
  frequency_ = f;
};



//
inline void SgVlbiBand::setTCreation(const SgMJD& t)
{
  tCreation_ = t;
};



//
inline void SgVlbiBand::setInputFileName(const QString& fileName)
{
  inputFileName_ = fileName;
};



//
inline void SgVlbiBand::setInputFileVersion(int version)
{
  inputFileVersion_ = version;
};



//
inline void SgVlbiBand::setCorrelatorType(const QString& name)
{
  correlatorType_ = name;
};



//
inline void SgVlbiBand::setMaxNumOfChannels(int num)
{
  maxNumOfChannels_ = num;
};



//
// FUNCTIONS:
//
//
inline const QString SgVlbiBand::className() const
{
  return "SgVlbiBand";
};



//
inline bool SgVlbiBand::operator<(const SgVlbiBand& band) const
{
  return frequency_ > band.getFrequency();
};



//
inline SgVlbiStationInfo* SgVlbiBand::lookupStationByIdx(int idx)
{
  if (stationsByIdx_.contains(idx))
    return stationsByIdx_.value(idx);
  else
    return NULL;
};



//
inline SgVlbiSourceInfo* SgVlbiBand::lookupSourceByIdx(int idx)
{
  if (sourcesByIdx_.contains(idx))
    return sourcesByIdx_.value(idx);
  else
    return NULL;
};



//
inline SgVlbiBaselineInfo* SgVlbiBand::lookupBaselineByIdx(int idx)
{
  if (baselinesByIdx_.contains(idx))
    return baselinesByIdx_.value(idx);
  else
    return NULL;
};
/*=====================================================================================================*/
#endif // SG_VLBI_BAND_H
