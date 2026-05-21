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

#ifndef SG_EXTERNAL_TRP
#define SG_EXTERNAL_TRP


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>


#include <SgMJD.h>




/***===================================================================================================*/
/**
 *
 */
/**====================================================================================================*/
class SgExternalTrp
{
public:
  enum InputFormatVersion
  {
    IFV_UNKN        = 0,
    IFV_V_13        = 1,
    IFV_V_14        = 2,
  };
  

  class TrpRecord
  {
  private:
    int                         scanIdx_;
    SgMJD                       t_;
    QString                     srcName_;
    QString                     stnName_;
    double                      dAzimuth_;
    double                      dElevation_;
    double                      dPres_;
    double                      dTemp_;

    double                      dSlant0Delay_;
    double                      dSlant1Delay_;
    double                      dMap0Delay_;
    double                      dMap1Delay_;
    double                      dSlant0Rate_;
    double                      dSlant1Rate_;
    double                      dMap0Rate_;
    double                      dMap1Rate_;

    double                      dGradNDelay_;
    double                      dGradEDelay_;
    double                      dGradNRate_;
    double                      dGradERate_;
    bool                        isOk_;
    bool                        hasMate_;

    
  public:
    inline TrpRecord() : t_(tZero), srcName_(""), stnName_("") 
      {scanIdx_=0; dAzimuth_=dElevation_=dPres_=dTemp_=0.0;
        dSlant0Delay_=dSlant1Delay_=dSlant0Rate_=dSlant1Rate_=0.0;
        dMap0Delay_=dMap1Delay_=dMap0Rate_=dMap1Rate_=1.0;
        dGradNDelay_=dGradEDelay_=dGradNRate_=dGradERate_=1.0; isOk_=hasMate_=false;};

    inline TrpRecord(const TrpRecord& rec) : t_(rec.t()), srcName_(rec.srcName()), stnName_(rec.stnName())
      { scanIdx_ = rec.scanIdx(); dAzimuth_ = rec.dAzimuth(); dElevation_ = rec.dElevation(); 
        dPres_ = rec.dPres(); dTemp_=rec.dTemp(); 
        dSlant0Delay_ = rec.dSlant0Delay(); dSlant1Delay_ = rec.dSlant1Delay(); 
        dSlant0Rate_ = rec.dSlant0Rate(); dSlant1Rate_ = rec.dSlant1Rate(); 
        dMap0Delay_ = rec.dMap0Delay(); dMap1Delay_ = rec.dMap1Delay(); 
        dMap0Rate_ = rec.dMap0Rate(); dMap1Rate_ = rec.dMap1Rate();
        dGradNDelay_ = rec.dGradNDelay(); dGradEDelay_ = rec.dGradEDelay(); 
        dGradNRate_ = rec.dGradNRate(); dGradERate_ = rec.dGradERate(); isOk_ = rec.isOk(); 
        hasMate_ = rec.hasMate();};

    inline ~TrpRecord() {};
    
    inline int            scanIdx() const {return scanIdx_;};
    inline const SgMJD&   t() const {return t_;};
    inline const QString& srcName() const {return srcName_;};
    inline const QString& stnName() const {return stnName_;};
    inline double         dAzimuth() const {return dAzimuth_;};
    inline double         dElevation() const {return dElevation_;};
    inline double         dPres() const {return dPres_;};
    inline double         dTemp() const {return dTemp_;};
    inline double         dSlant0Delay() const {return dSlant0Delay_;};
    inline double         dSlant1Delay() const {return dSlant1Delay_;};
    inline double         dMap0Delay() const {return dMap0Delay_;};
    inline double         dMap1Delay() const {return dMap1Delay_;};
    inline double         dSlant0Rate() const {return dSlant0Rate_;};
    inline double         dSlant1Rate() const {return dSlant1Rate_;};
    inline double         dMap0Rate() const {return dMap0Rate_;};
    inline double         dMap1Rate() const {return dMap1Rate_;};
    inline double         dGradNDelay() const {return dGradNDelay_;};
    inline double         dGradEDelay() const {return dGradEDelay_;};
    inline double         dGradNRate() const {return dGradNRate_;};
    inline double         dGradERate() const {return dGradERate_;};
    inline bool           isOk() const {return isOk_;};
    inline bool           hasMate() const {return hasMate_;};
    

    inline void setHasMate(bool has) {hasMate_ = has;};
    inline void setIsOk(bool is) {isOk_ = is;};
    void parseString_V01_3(const QString&);
    void parseString_V01_4(const QString&);
  };

public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  inline SgExternalTrp();

  /**A destructor.
   * Frees allocated memory.
   */
  inline ~SgExternalTrp();


  const QString className();
  
  
  //
  // Interfaces:
  //
  // gets:
  //
  inline bool isOk() const {return isOk_;};


  inline const QString& getInputFileName() const {return inputFileName_;};
  inline const QString& modelName() const {return modelName_;};

  inline const QString& fmtVersion() const {return fmtVersion_;};
  inline const QString& fmtDate() const {return fmtDate_;};
  
  const QList<TrpRecord*>& getTrpRecords() const {return trpRecords_;};
  const QMap <QString, QList<TrpRecord*>* >& getTrpRecordsByStnId() const {return recsByStnId_;};
  const QList<TrpRecord*>* getTrpRecords(const QString& stnId) const 
    {return recsByStnId_.contains(stnId)?recsByStnId_.value(stnId):NULL;};

  // sets:
  
  //
  // Functions:
  //
  bool readFile(const QString&);



private:
  bool                          isOk_;
  QList<TrpRecord*>             trpRecords_;
  QMap< QString, QList<TrpRecord*>* >
                                recsByStnId_;
  QString                       fmtVersion_;
  QString                       fmtDate_;
/*
  QMap< QString, QMap<QString, SgVlbiAuxObservation*>* >
                                recsByScanByStn_;
*/
  QString                       inputFileName_;
  QString                       modelName_;
  InputFormatVersion            ifvFormatVersion_;
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
inline SgExternalTrp::SgExternalTrp() :
  trpRecords_(),
  recsByStnId_(),
//  recsByScanByStn_(),
  fmtVersion_(""),
  fmtDate_(""),
  inputFileName_(""),
  modelName_("")
{
  isOk_ = false;
  ifvFormatVersion_ = IFV_UNKN;
};



// A destructor:
inline SgExternalTrp::~SgExternalTrp()
{
  for (int i=0; i<trpRecords_.size(); i++)
    delete trpRecords_.at(i);
  trpRecords_.clear();
  for(QMap< QString, QList<TrpRecord*>* >::iterator it=recsByStnId_.begin(); it!=recsByStnId_.end(); 
    ++it)
    delete it.value();
  recsByStnId_.clear();
  
/*  
  for(QMap< QString, QMap<QString, SgVlbiAuxObservation*>* >::iterator it=recsByScanByStn_.begin();
    it!=recsByScanByStn_.end(); ++it)
  {
    delete it.value();
  };
  recsByScanByStn_.clear();
*/

};



//
// INTERFACES:
//
//


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
