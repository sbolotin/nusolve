/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2024 Sergei Bolotin.
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

#ifndef SG_EXTERNAL_EOP_FILE_H
#define SG_EXTERNAL_EOP_FILE_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <SgMJD.h>
#include <SgTidalUt1.h>



class SgVector;
class SgMatrix;
/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgExternalEopFile
{
public:
  enum EopIdx
  {
    UT1_IDX                   = 0,
    PMX_IDX                   = 1,
    PMY_IDX                   = 2,
    CIX_IDX                   = 3,
    CIY_IDX                   = 4,
  };
  enum InputEopType
  {
    IET_UNKN                  = 0,
    IET_ERP                   = 1,
    IET_FINALS                = 2,
    IET_C04                   = 3,
  };
 
 
 
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgExternalEopFile();

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgExternalEopFile();

  //
  // Interfaces:
  //
  // gets:
  //
  inline SgTidalUt1::UT1TideContentType ut1Type() const {return ut1Type_;};
  inline InputEopType inputEopType() const {return inputEopType_;}
  
  /**Returns name of the file with the model.
   */
  inline const QString& getFileName() const;

  // sets:
  /**Sets up a name of the file with the model.
   */
  inline void setFileName(const QString&);
  
  inline bool isOk() const;

  inline SgVector* argument();

  inline SgMatrix* eopTable();
 
  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Reads model's values from an external file.
   */
  bool readFile(const QString&, const SgMJD&, int numOfPoints);
  
  /*
   */
private:
  SgMJD                         firstEpoch_;
  SgMJD                         lastEpoch_;
  QString                       fileName_;
  bool                          isOk_;
  SgVector                     *argument_;
  SgMatrix                     *eopTable_;
  SgTidalUt1::UT1TideContentType
                                ut1Type_;
  InputEopType                  inputEopType_;

  bool readErpFile(const QString& fileName, const SgMJD& tMean, int numOfPoints);
  bool readFinalsFile(const QString& fileName, const SgMJD& tMean, int numOfPoints);
  bool readC04File(const QString& fileName, const SgMJD& tMean, int numOfPoints);
};
/*=====================================================================================================*/




/*=====================================================================================================*/
/*                                                                                                     */
/* SgExternalEopFile inline members:                                                                   */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
//
// An empty constructor:
inline SgExternalEopFile::SgExternalEopFile() :
  firstEpoch_(tZero),
  lastEpoch_(tZero),
  fileName_("")
{
  isOk_ = false;
  argument_ = NULL;
  eopTable_ = NULL;
  ut1Type_ = SgTidalUt1::CT_FULL;
  inputEopType_ = IET_UNKN;
};



//
// INTERFACES:
//
//
inline const QString& SgExternalEopFile::getFileName() const
{
  return fileName_;
};



//
inline void SgExternalEopFile::setFileName(const QString& name)
{
  fileName_ = name;
};


//
inline bool SgExternalEopFile::isOk() const
{
  return isOk_;
};



//
inline SgVector* SgExternalEopFile::argument()
{
  return argument_;
};



//
inline SgMatrix* SgExternalEopFile::eopTable()
{
  return eopTable_;
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
#endif //SG_EXTERNAL_EOP_FILE_H
