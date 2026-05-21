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

#ifndef SG_EXTERNAL_WEIGHTS
#define SG_EXTERNAL_WEIGHTS


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>

#include <SgVlbiBaselineInfo.h>



/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgBaselineExternalWeight
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgBaselineExternalWeight();

  /**A copy constructor.
   * Creates a copy of the object.
   */
  inline SgBaselineExternalWeight(const SgBaselineExternalWeight&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgBaselineExternalWeight();

  //
  // Interfaces:
  //
  // gets:
  //
  /**Returns a name of the baseline.
   */
  inline const QString& getBaselineName() const;

  /**Returns external weight for the delay.
   */
  inline double getDelayWeight() const;

  /**Returns external weight for the delay rate.
   */
  inline double getRateWeight() const;
  
  /**Returns version of the DBH file for the weights.
   */
  inline int getDbhVersionNumber() const;

  // sets:
  /**Sets up a name of the baseline.
   */
  inline void setBaselineName(const QString&);

  /**Sets up external weight for the delay.
   */
  inline void setDelayWeight(double);

  /**Sets up external weight for the delay rate.
   */
  inline void setRateWeight(double);

  /**Sets up a version of the DBH file for the weights.
   */
  inline void setDbhVersionNumber(int);
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Reads model's values from an external file.
   */
  bool parseString(const QString&);
  
private:
  QString                       baselineName_;
  double                        delayWeight_;
  double                        rateWeight_;
  int                           dbhVersionNumber_;
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgExternalWeights
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgExternalWeights();

  /**A constructor.
   * The same but with a sessionName
   */
  inline SgExternalWeights(const QString& sessionName);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgExternalWeights();

  //
  // Interfaces:
  //
  // gets:
  //
  /**Returns a name of the file with external weights.
   */
  inline const QString& getFileName() const;

  /**Returns a name of the baseline.
   */
  inline const QString& getSessionName() const;

  /**Returns an OKness status.
   */
  inline bool isOk() const;

  // sets:
  /**Sets up a name of the baseline.
   */
  inline void setSessionName(const QString&);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Reads model's values from an external file.
   */
  bool readFile(const QString&);
  
  void setupExternalWeights(BaselinesByName&);
  
private:
  QString                                   fileName_;
  QString                                   sessionName_;
  QMap<QString, SgBaselineExternalWeight*>  weights_;
  bool                                      isOk_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgBaselineExternalWeight inline members:                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
//
// An empty constructor:
inline SgBaselineExternalWeight::SgBaselineExternalWeight() :
  baselineName_("A:B")
{
  delayWeight_ = rateWeight_ = 0.0;
  dbhVersionNumber_ = 0;
};



// A copy constructor:
inline SgBaselineExternalWeight::SgBaselineExternalWeight(const SgBaselineExternalWeight& w) :
  baselineName_(w.baselineName_)
{
  setDelayWeight(w.getDelayWeight());
  setRateWeight(w.getRateWeight());
  setDbhVersionNumber(w.getDbhVersionNumber());
};



// A destructor:
inline SgBaselineExternalWeight::~SgBaselineExternalWeight()
{
  // empty, nothing to do
};



//
// INTERFACES:
//
//
inline const QString& SgBaselineExternalWeight::getBaselineName() const
{
  return baselineName_;
};



//
inline double SgBaselineExternalWeight::getDelayWeight() const
{
  return delayWeight_;
};



//
inline double SgBaselineExternalWeight::getRateWeight() const
{
  return rateWeight_;
};



//
inline int SgBaselineExternalWeight::getDbhVersionNumber() const
{
  return dbhVersionNumber_;
};



//
inline void SgBaselineExternalWeight::setBaselineName(const QString& name)
{
  baselineName_ = name;
};



//
inline void SgBaselineExternalWeight::setDelayWeight(double w)
{
  delayWeight_ = w;
};



//
inline void SgBaselineExternalWeight::setRateWeight(double w)
{
  rateWeight_ = w;
};



//
inline void SgBaselineExternalWeight::setDbhVersionNumber(int v)
{
  dbhVersionNumber_ = v;
};



//
// FUNCTIONS:
//
//
//

// FRIENDS:
//
//
//
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgExternalWeights inline members:                                                                   */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
//
// An empty constructor:
inline SgExternalWeights::SgExternalWeights() :
  fileName_("?"),
  sessionName_("?"),
  weights_()
{
  isOk_ = false;
};



// An empty constructor:
inline SgExternalWeights::SgExternalWeights(const QString& sessionName) :
  fileName_("?"),
  sessionName_(sessionName),
  weights_()
{
  isOk_ = false;
};



// A destructor:
inline SgExternalWeights::~SgExternalWeights()
{
  for (QMap<QString, SgBaselineExternalWeight*>::iterator it=weights_.begin(); it!=weights_.end(); ++it)
    delete it.value();
  weights_.clear();
};



//
// INTERFACES:
//
//
// gets:
inline const QString& SgExternalWeights::getFileName() const
{
  return fileName_;
};



//
inline const QString& SgExternalWeights::getSessionName() const
{
  return sessionName_;
};



//
inline bool SgExternalWeights::isOk() const
{
  return isOk_;
}



// sets:
//
inline void SgExternalWeights::setSessionName(const QString& name)
{
  sessionName_ = name;
};




//
// FUNCTIONS:
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
#endif //SG_EXTERNAL_WEIGHTS
