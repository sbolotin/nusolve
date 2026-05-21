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

#ifndef SG_ECC_REC_H
#define SG_ECC_REC_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>


#include <Sg3dVector.h>

#include <SgMJD.h>

/***===================================================================================================*/
/**
 * SgEccRec -- arecord from CALC/SOLVE's ECCDAT.ecc file.
 *
 */
/**====================================================================================================*/
class SgEccRec
{
public:
  enum EccType
  {
    ET_N_A          = 0,
    ET_NEU          = 1,
    ET_XYZ          = 2,
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgEccRec();
  inline SgEccRec(const SgEccRec&);

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgEccRec();

  //
  // Interfaces:
  //
  //
  inline SgEccRec& operator= (const SgEccRec&);

  // gets:
  inline bool getIsOk() const;
  inline const QString& getSiteName() const;
  inline const QString& getCdpNumber() const;
  inline int getNCdp() const;
  inline const SgMJD& getTSince() const;
  inline const SgMJD& getTTill() const;
  inline const Sg3dVector& getDR() const;
  inline EccType getEccType() const;

  // sets:
  inline void setSiteName(const QString&);
  inline void setCdpNumber(const QString&);
  inline void setNCdp(int);
  inline void setTSince(const SgMJD& );
  inline void setTTill(const SgMJD& );
  inline void setDR(const Sg3dVector&);
  inline void setEccType(EccType);

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();
  
  bool parseString(const QString&);
  
  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  bool              isOk_;
  QString           siteName_;
  QString           cdpNumber_;
  int               nCDP_;
  SgMJD             tSince_;
  SgMJD             tTill_;
  Sg3dVector        dR_;
  EccType           eccType_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* SgEccDat inline members:                                                                            */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//
// An empty constructor:
inline SgEccRec::SgEccRec() :
  siteName_("ZABRSKPT"),
  cdpNumber_("0007"),
  tSince_(tZero),
  tTill_(tZero),
  dR_(v3Zero)
{
  isOk_ = false;
  nCDP_ = 0;
  eccType_ = ET_N_A;
};



inline SgEccRec::SgEccRec(const SgEccRec& r) :
  siteName_(r.getSiteName()),
  cdpNumber_(r.getCdpNumber()),
  tSince_(r.getTSince()),
  tTill_(r.getTTill()),
  dR_(r.getDR())
{
  isOk_ = r.getIsOk();
  nCDP_ = r.getNCdp();
  eccType_ = r.getEccType();
};




// A destructor:
inline SgEccRec::~SgEccRec()
{
  // nothing to do
};



//
// INTERFACES:
//
//
inline SgEccRec& SgEccRec::operator=(const SgEccRec& r)
{
  setSiteName(r.getSiteName());
  setCdpNumber(r.getCdpNumber());
  setTSince(r.getTSince());
  setTTill(r.getTTill());
  setDR(r.getDR());
  isOk_ = r.getIsOk();
  nCDP_ = r.getNCdp();
  eccType_ = r.getEccType();
  return *this;
};



//
inline bool SgEccRec::getIsOk() const
{
  return isOk_;
};



//
inline const QString& SgEccRec::getSiteName() const
{
  return siteName_;
};



//
inline const QString& SgEccRec::getCdpNumber() const
{
  return cdpNumber_;
};



//
inline int SgEccRec::getNCdp() const
{
  return nCDP_;
};



//
inline const SgMJD& SgEccRec::getTSince() const
{
  return tSince_;
};



//
inline const SgMJD& SgEccRec::getTTill() const
{
  return tTill_;
};



//
inline const Sg3dVector& SgEccRec::getDR() const
{
  return dR_;
};



//
inline SgEccRec::EccType SgEccRec::getEccType() const
{
  return eccType_;
};



//
inline void SgEccRec::setSiteName(const QString& s)
{
  siteName_ = s;
};



//
inline void SgEccRec::setCdpNumber(const QString& s)
{
  cdpNumber_ = s;
};



//
inline void SgEccRec::setNCdp(int n)
{
  nCDP_ = n;
};



//
inline void SgEccRec::setTSince(const SgMJD& t)
{
  tSince_ = t;
};



//
inline void SgEccRec::setTTill(const SgMJD& t)
{
  tTill_ = t;
};



//
inline void SgEccRec::setDR(const Sg3dVector& r)
{
  dR_ = r;
};



//
inline void SgEccRec::setEccType(EccType et)
{
  eccType_ = et;
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
#endif //SG_ECC_REC_H
