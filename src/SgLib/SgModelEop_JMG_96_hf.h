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

#ifndef SG_MODEL_EOP_JMG_96_HF_H
#define SG_MODEL_EOP_JMG_96_HF_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMultiMap>
#include <QtCore/QString>


#include <SgMJD.h>



// this is a preliminary realizaion of accessing to auxiliari 'a priori' files, should be reworked later.


// The high frequency EOP variations in "jmg96.hf" file format:
/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgModelEop_JMG_96_hf
{
public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgModelEop_JMG_96_hf();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgModelEop_JMG_96_hf();

  //
  // Interfaces:
  //
  // gets:
  //
  /**Returns name of the file with the model.
   */
  inline const QString& getFileName() const;
  inline bool isOk() const {return isOk_;};

  // sets:
  /**Sets up a name of the file with the model.
   */
  inline void setFileName(const QString&);
  
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Reads model's values from an external file.
   */
  bool readFile(const QString&);
  
  /*
   */
  void calcCorrections(const SgMJD&, double& dUt1, double& dPx, double& dPy);

private:
  struct HfEopRec
  {
    int                   n_[6];
    double                a_cos_, a_sin_;
    inline HfEopRec();
  };
  QString                       fileName_;
  bool                          isOk_;
  int                           numUt_;
  int                           numPm_;
  HfEopRec                     *baseModel_;
  HfEopRec                     *utModel_;
  HfEopRec                     *pmModel_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgModelEop_JMG_96_hf inline members:                                                                */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
//
inline SgModelEop_JMG_96_hf::HfEopRec::HfEopRec()
{
  n_[0] = n_[1] = n_[2] = n_[3] = n_[4] = n_[5] = 0;
  a_cos_ = a_sin_ = 0.0;
};



// An empty constructor:
inline SgModelEop_JMG_96_hf::SgModelEop_JMG_96_hf() :
  fileName_("none")
{
  numUt_ = 0;
  numPm_ = 0;
  baseModel_ = NULL;
  utModel_ = NULL;
  pmModel_ = NULL;
  isOk_ = false;
};



// A destructor:
inline SgModelEop_JMG_96_hf::~SgModelEop_JMG_96_hf()
{
  numUt_ = numPm_ = 0;
  if (baseModel_)
  {
    delete[] baseModel_;
    baseModel_ = NULL;
  };
  utModel_ = pmModel_ = NULL;
};



//
// INTERFACES:
//
//
inline const QString& SgModelEop_JMG_96_hf::getFileName() const
{
  return fileName_;
};



//
inline void SgModelEop_JMG_96_hf::setFileName(const QString& name)
{
  fileName_ = name;
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
#endif //SG_MODEL_EOP_JMG_96_HF_H
