/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#ifndef VC_CALC2SESSION_IFC_H
#define VC_CALC2SESSION_IFC_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>

#include <SgVgosDb.h>
#include <SgVlbiAuxObservation.h>
#include <SgVlbiObservable.h>
#include <SgVlbiSession.h>


class VcCalc2SessionIfc;
extern VcCalc2SessionIfc c2s;



// FAQ Pointers to Member Functions:
// https://isocpp.org/wiki/faq/pointers-to-members
//

/*
typedef void (VcCalc2SessionIfc::*pmfAddV)(const QString& txt, short nToc, short d1, short d2, short d3);
*/

typedef void (VcCalc2SessionIfc::*pmfDelV)(const QString& lCode, short nToc);


typedef short (VcCalc2SessionIfc::*pmfGetA)(char   v[], short d1, short d2, short d3, short actDims[]);
typedef short (VcCalc2SessionIfc::*pmfGetI)(short  v[], short d1, short d2, short d3, short actDims[]);
typedef short (VcCalc2SessionIfc::*pmfGet4)(double v[], short d1, short d2, short d3, short actDims[]);

typedef void (VcCalc2SessionIfc::*pmfPutA)(const char   values[], short d1, short d2, short d3);
typedef void (VcCalc2SessionIfc::*pmfPutI)(const short  values[], short d1, short d2, short d3);
typedef void (VcCalc2SessionIfc::*pmfPutR)(const double values[], short d1, short d2, short d3);
typedef void (VcCalc2SessionIfc::*pmfPut4)(const double values[], short d1, short d2, short d3);


/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class VcCalc2SessionIfc
{
public:
  enum CalcCallingMode
  {
    CCM_None,
    CCM_Legacy,
    CCM_Modern,
  };
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  VcCalc2SessionIfc();

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~VcCalc2SessionIfc();



  //
  // Interfaces:
  //
  // gets:
  /**Returns database name.
   */
  inline const QString& getInputDbName() const;

  /**Returns a path to vgosDb files.
   */
  inline const QString& getPath2Data() const;

  /**Returns a name of a wrapper file.
   */
  inline const QString& getWrapperFileName() const;
  
  inline CalcCallingMode getCalcCallingMode() const {return calcCallingMode_;};


  //
  // sets:
  /**Sets up a path to vgosDb files.
   */
  inline void setInputDbName(const QString&);

  /**Sets up a path to vgosDb files.
   */
  inline void setPath2Data(const QString&);

  /**Sets up a name of a wrapper file.
   */
  inline void setWrapperFileName(const QString&);

  inline void setCalcCallingMode(CalcCallingMode mode) {calcCallingMode_ = mode;};


  inline SgVlbiSession*& session() {return session_;};
  inline SgVgosDb*& vgosDb() {return vgosDb_;};

  void setBandList(const QList<QString>& bands);
  inline void setCurrentBand(const QString& key, bool is) {currentBandKey_=key; isPrimeBand_=is;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
  */
  static const QString className();
    
  
  void mimic_Kai(short openMode, const QString& inputDbName, short inputDbVersion, 
    const QString& outputDbName, short &outputDbVersion, short &errorFlag);
  
  short mimic_MvRec(short ntoc, short kmode, short knum);
  void  mimic_Finis();
  void  mimic_pHist(const QString& histRecord);
  void  rewind();
  
  //
  // adds:
  void mimicAddA(short nToc, const QString& lCode, const QString& descr,
    short dim1, short dim2, short dim3);
  void mimicAddI(short nToc, const QString& lCode, const QString& descr,
    short dim1, short dim2, short dim3);
  void mimicAddJ(short nToc, const QString& lCode, const QString& descr,
    short dim1, short dim2, short dim3);
  void mimicAdd4(short nToc, const QString& lCode, const QString& descr,
    short dim1, short dim2, short dim3);
  void mimicAddR(short nToc, const QString& lCode, const QString& descr,
    short dim1, short dim2, short dim3);  

  // dels:
  void mimicDelA(short nToc, const QString& lCode);
  void mimicDelI(short nToc, const QString& lCode);
  void mimicDelJ(short nToc, const QString& lCode);
  void mimicDel4(short nToc, const QString& lCode);
  void mimicDelR(short nToc, const QString& lCode);
  
  
  // gets:
  short mimicGetA(const QString& lCode, char   values[], short dim1, short dim2, short dim3,
    short actualDimension[]);
  short mimicGetI(const QString& lCode, short  values[], short dim1, short dim2, short dim3,
    short actualDimension[]);
  short mimicGet4(const QString& lCode, double values[], short dim1, short dim2, short dim3,
    short actualDimension[]);

  // puts:
  void mimicPutA(const QString& lCode, const char   values[], short dim1, short dim2, short dim3);
  void mimicPutI(const QString& lCode, const short  values[], short dim1, short dim2, short dim3);
  void mimicPutR(const QString& lCode, const double values[], short dim1, short dim2, short dim3);
  void mimicPut4(const QString& lCode, const double values[], short dim1, short dim2, short dim3);



  //
  // Friends:
  //



  //
  // I/O:
  //
  //


private:
  enum OpenMode
  {
    OM_None,
    OM_ReadOnly,
    OM_ReadWrite,
  };
  OpenMode                      openMode_;
  CalcCallingMode               calcCallingMode_;
  QString                       inputDbName_;
  QString                       path2data_;
  QString                       wrapperFileName_;
  SgVlbiSession                *session_;
  SgVgosDb                     *vgosDb_;
  QList<QString>                bandList_;
  QString                       currentBandKey_;
  bool                          isPrimeBand_;
  int                           obsIdx_;
  int                           numOfObservations_;
  int                           numOfStations_;
  int                           numOfSources_;
  SgVlbiObservation            *obs_;
  SgVlbiObservable             *o_;
  SgVlbiAuxObservation         *aux1_;
  SgVlbiAuxObservation         *aux2_;
  
  // dels:
  QMap<QString, pmfDelV>        delAbyLCode_;
  QMap<QString, pmfDelV>        delIbyLCode_;
  QMap<QString, pmfDelV>        delJbyLCode_;
  QMap<QString, pmfDelV>        del4byLCode_;
  QMap<QString, pmfDelV>        delRbyLCode_;

  // gets:
  QMap<QString, pmfGetA>        getAbyLCode_;
  QMap<QString, pmfGetI>        getIbyLCode_;
  QMap<QString, pmfGet4>        get4byLCode_;

  // puts:
  QMap<QString, pmfPutA>        putAbyLCode_;
  QMap<QString, pmfPutI>        putIbyLCode_;
  QMap<QString, pmfPutR>        putRbyLCode_;
  QMap<QString, pmfPut4>        put4byLCode_;

  QMap<QString, int>            bandDepPutsByLCode_;


  //
  bool readSession();
  //
  bool writeSession();


  // known delAs:
  void delA_obsolete(const QString& lCode, short nToc);
  void delR_obsolete(const QString& lCode, short nToc);



  // known getAs:
  short getA_sitNames(char   values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getA_strNames(char   values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getA_baseline(char   values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getA_starId  (char   values[], short dim1, short dim2, short dim3, short actualDimension[]);
  
  // known getIs:
  short getI_numSites(short  values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getI_numStars(short  values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getI_intrval4(short  values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getI_utc_tag4(short  values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short getI_utc_tag2(short  values[], short dim1, short dim2, short dim3, short actualDimension[]);
  
  // known get4s:
  short get4_rotEpoch(double values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short get4_sec_Tag (double values[], short dim1, short dim2, short dim3, short actualDimension[]);
  short get4_ref_freq(double values[], short dim1, short dim2, short dim3, short actualDimension[]);



  //
  //
  void putA_calcFlgN(const char values[], short dim1, short dim2, short dim3);
  void putA_atm_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_atm_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_axo_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_axo_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_etd_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_etd_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_ptd_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_ptd_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_nut_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_nut_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_oce_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_oce_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_ati_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_ati_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_cti_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_cti_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_plx_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_plx_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_str_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_str_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_the_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_rel_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_pan_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_sit_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_pep_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_wob_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_wob_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_ut1_mess(const char values[], short dim1, short dim2, short dim3);
  void putA_ut1_cFlg(const char values[], short dim1, short dim2, short dim3);
  void putA_oce_stat(const char values[], short dim1, short dim2, short dim3);
  void putA_ut1Intrp(const char values[], short dim1, short dim2, short dim3);
  void putA_fUt1Text(const char values[], short dim1, short dim2, short dim3);
  void putA_wobIntrp(const char values[], short dim1, short dim2, short dim3);
  void putA_fWobText(const char values[], short dim1, short dim2, short dim3);
  void putA_star_ref(const char values[], short dim1, short dim2, short dim3);
  void putA_tectPlNm(const char values[], short dim1, short dim2, short dim3);
  void putA_obsolete(const char values[], short dim1, short dim2, short dim3);
  
  
  // known putIs:
  void putI_calcFlgV(const short values[], short dim1, short dim2, short dim3);
  void putI_tidalUt1(const short values[], short dim1, short dim2, short dim3);
  void putI_axisTyps(const short values[], short dim1, short dim2, short dim3);

  
  // known putRs:
  void putR_siteZens(const double values[], short dim1, short dim2, short dim3);
  void putR_optlCoef(const double values[], short dim1, short dim2, short dim3);
  void putR_sitOcPhs(const double values[], short dim1, short dim2, short dim3);
  void putR_sitOcAmp(const double values[], short dim1, short dim2, short dim3);
  void putR_sitHocPh(const double values[], short dim1, short dim2, short dim3);
  void putR_sitHocAm(const double values[], short dim1, short dim2, short dim3);
  void putR_star2000(const double values[], short dim1, short dim2, short dim3);
  void putR_siteRecs(const double values[], short dim1, short dim2, short dim3);
  void putR_axisOffs(const double values[], short dim1, short dim2, short dim3);
  void putR_axisTilt(const double values[], short dim1, short dim2, short dim3);

  

  // known put4s:
  void put4_calc_ver(const double values[], short dim1, short dim2, short dim3);
  void put4_ut1Epoch(const double values[], short dim1, short dim2, short dim3);
  void put4_wobEpoch(const double values[], short dim1, short dim2, short dim3);
  void put4_fUt1_inf(const double values[], short dim1, short dim2, short dim3);
  void put4_fWob_inf(const double values[], short dim1, short dim2, short dim3);
  void put4_fUt1_pts(const double values[], short dim1, short dim2, short dim3);
  void put4_fWobXnYt(const double values[], short dim1, short dim2, short dim3);
  void put4_tai__utc(const double values[], short dim1, short dim2, short dim3);
  //
  void put4_axo_part(const double values[], short dim1, short dim2, short dim3);
  void put4_az_theo  (const double values[], short dim1, short dim2, short dim3);
  void put4_el_theo  (const double values[], short dim1, short dim2, short dim3);

  void put4_parAngle(const double values[], short dim1, short dim2, short dim3);
  void put4_axo_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_nDryCont(const double values[], short dim1, short dim2, short dim3);
  void put4_nWetCont(const double values[], short dim1, short dim2, short dim3);
  void put4_oce_horz(const double values[], short dim1, short dim2, short dim3);
  void put4_oce_vert(const double values[], short dim1, short dim2, short dim3);
  void put4_oce_deld(const double values[], short dim1, short dim2, short dim3);
  void put4_nDryPart(const double values[], short dim1, short dim2, short dim3);
  void put4_nWetPart(const double values[], short dim1, short dim2, short dim3);
  void put4_nGradPar(const double values[], short dim1, short dim2, short dim3);
  //
  void put4_bendPart(const double values[], short dim1, short dim2, short dim3);
  void put4_ut1_part(const double values[], short dim1, short dim2, short dim3);
  void put4_wob_part(const double values[], short dim1, short dim2, short dim3);
  void put4_nut06XyP(const double values[], short dim1, short dim2, short dim3);
  void put4_str_part(const double values[], short dim1, short dim2, short dim3);
  void put4_sit_part(const double values[], short dim1, short dim2, short dim3);
  void put4_consPart(const double values[], short dim1, short dim2, short dim3);
  void put4_plx_part(const double values[], short dim1, short dim2, short dim3);
  void put4_pTdXyPar(const double values[], short dim1, short dim2, short dim3);

  void put4_consnDel(const double values[], short dim1, short dim2, short dim3);
  void put4_consnRat(const double values[], short dim1, short dim2, short dim3);
  void put4_con_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_sun_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_sun2cont(const double values[], short dim1, short dim2, short dim3);
  void put4_etd_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_feed_cor(const double values[], short dim1, short dim2, short dim3);
  void put4_oce_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_oce_old (const double values[], short dim1, short dim2, short dim3);
  void put4_optlCont(const double values[], short dim1, short dim2, short dim3);
  void put4_ptd_cont(const double values[], short dim1, short dim2, short dim3);
  void put4_ptOldCon(const double values[], short dim1, short dim2, short dim3);
  void put4_tiltRmvr(const double values[], short dim1, short dim2, short dim3);
  void put4_ut1Ortho(const double values[], short dim1, short dim2, short dim3);
  void put4_uvf_asec(const double values[], short dim1, short dim2, short dim3);
  void put4_wobOrtho(const double values[], short dim1, short dim2, short dim3);
  void put4_ut1Libra(const double values[], short dim1, short dim2, short dim3);
  void put4_wobLibra(const double values[], short dim1, short dim2, short dim3);
  void put4_wobXcont(const double values[], short dim1, short dim2, short dim3);
  void put4_wobYcont(const double values[], short dim1, short dim2, short dim3);

  void put4_plx1psec(const double values[], short dim1, short dim2, short dim3);
  void put4_ct_site1(const double values[], short dim1, short dim2, short dim3);
  void put4_sun_data(const double values[], short dim1, short dim2, short dim3);
  void put4_moonData(const double values[], short dim1, short dim2, short dim3);
  void put4_earth_ce(const double values[], short dim1, short dim2, short dim3);

  void put4_nut06Xys(const double values[], short dim1, short dim2, short dim3);
  void put4_nut2006A(const double values[], short dim1, short dim2, short dim3);
  void put4_nut_wahr(const double values[], short dim1, short dim2, short dim3);
  void put4_cf2j2k  (const double values[], short dim1, short dim2, short dim3);
  void put4_ut1_tai (const double values[], short dim1, short dim2, short dim3);
  void put4_polar_xy(const double values[], short dim1, short dim2, short dim3);

  void put4_obsolete(const double values[], short dim1, short dim2, short dim3);

};
/*=====================================================================================================*/





/*=====================================================================================================*/
/*                                                                                                     */
/* VpSetup inline members:                                                                             */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
//
// INTERFACES:
//
//
// gets:
//
inline const QString& VcCalc2SessionIfc::getInputDbName() const
{
  return inputDbName_;
};



inline const QString& VcCalc2SessionIfc::getPath2Data() const
{
  return path2data_;
};



//
inline const QString& VcCalc2SessionIfc::getWrapperFileName() const
{
  return wrapperFileName_;
};



//
// sets:
//
inline void VcCalc2SessionIfc::setInputDbName(const QString& str)
{
  inputDbName_ = str;
};



inline void VcCalc2SessionIfc::setPath2Data(const QString& str)
{
  path2data_ = str;
};



//
inline void VcCalc2SessionIfc::setWrapperFileName(const QString& str)
{
  wrapperFileName_ = str;
};



//
/*=====================================================================================================*/
//
#endif // VC_CALC2SESSION_IFC_H
