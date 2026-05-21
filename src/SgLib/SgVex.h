/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2020 Sergei Bolotin.
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

#ifndef SG_VEX_H
#define SG_VEX_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMultiMap>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QVector>



#include <SgLogger.h>
#include <SgMJD.h>


enum SgChannelSideBand
{
  CSB_UNDEF                = 0,
  CSB_LSB                  = 1,
  CSB_USB                  = 2,
  CSB_DUAL                 = 3,
};



enum SgChannelPolarization
{
  CP_UNDEF                 = 0,
  CP_RightCP               = 1,
  CP_LeftCP                = 2,
  CP_HorizontalLP          = 3,
  CP_VerticalLP            = 4,
};

QString sideBand2Str(SgChannelSideBand);
QString polarization2Str(SgChannelPolarization);



/*=====================================================================================================*/
class SgVexParameter
{
public:
  SgVexParameter() : key_(""), values_() {};
  SgVexParameter(const QString& key, const QList<QString>& values) : key_(key), values_(values) {};
  SgVexParameter(const SgVexParameter& par) : key_(par.key_), values_(par.values_) {};
 ~SgVexParameter() {values_.clear();};


  inline const QString& getKey() const {return key_;};
  inline const QList<QString>& getValues() const {return values_;};

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVexStatement(const QString& str);

protected:
  QString                       key_;
  QList<QString>                values_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgVexRefSatement
{
public:
  SgVexRefSatement() : key_(""), value_(""), qualifiers_(), qualifierByQ_() {};
  SgVexRefSatement(const QString& key, const QString& value, const QList<QString>& qualifiers)
    : key_(key), value_(value), qualifiers_(qualifiers) {};
  SgVexRefSatement(const SgVexRefSatement& ref)
    : key_(ref.key_), value_(ref.value_), qualifiers_(ref.qualifiers_), 
    qualifierByQ_(ref.qualifierByQ_) {};
 ~SgVexRefSatement() {qualifiers_.clear(); qualifierByQ_.clear();};

  inline const QString& getKey() const {return key_;};
  inline const QString& getValue() const {return value_;};
  inline const QList<QString>& getQualifiers() const {return qualifiers_;};
  inline const QMap<QString, QString>& getQualifierByQ() const {return qualifierByQ_;};

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVexStatement(const QString& str);

protected:
  QString                       key_;
  QString                       value_;
  QList<QString>                qualifiers_;
  QMap<QString, QString>        qualifierByQ_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgVexLiteralBlock
{
public:
  SgVexLiteralBlock() : key_(""), literalStrings_() {};
  SgVexLiteralBlock(const QString& key, const QList<QString>& literalStrings)
    : key_(key), literalStrings_(literalStrings) {};
  SgVexLiteralBlock(const SgVexLiteralBlock& lit)
    : key_(lit.key_), literalStrings_(lit.literalStrings_) {};
 ~SgVexLiteralBlock() {literalStrings_.clear();};


  inline const QString& getKey() const {return key_;};
  inline const QList<QString>& getLiteralStrings() const {return literalStrings_;};

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVex(const QList<QString>& statements, int& idx);

protected:
  QString                       key_;
  QList<QString>                literalStrings_;
};
/*=====================================================================================================*/




/*=====================================================================================================*/
class SgVexDefBlock
{
public:
  SgVexDefBlock() : key_("") {};

  SgVexDefBlock(const SgVexDefBlock& df);

 ~SgVexDefBlock();

  inline const QString& getKey() const {return key_;};
  inline const QList<SgVexRefSatement*>& getRefStatements() const {return refStatements_;};
  inline const QList<SgVexParameter*>& getParameters() const {return parameters_;};
  inline const QList<SgVexLiteralBlock*>& getLiteralas() const {return literalas_;};
  inline const QMap<QString, SgVexRefSatement*>& refsByKey() const {return refsByKey_;};
  inline const QMap<QString, SgVexParameter*>& parsByKey() const {return parsByKey_;};
  

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVex(const QList<QString>& statements, int& idx);

protected:
  QString                       key_;
  QList<SgVexRefSatement*>      refStatements_;
  QList<SgVexParameter*>        parameters_;
  QList<SgVexLiteralBlock*>     literalas_;
  QMap<QString, SgVexRefSatement*>
                                refsByKey_;
  QMap<QString, SgVexParameter*>parsByKey_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
class SgVexScanBlock
{
public:
  SgVexScanBlock() : key_("") {};
  SgVexScanBlock(const SgVexScanBlock& sb) : key_(sb.key_), parameters_(sb.parameters_) {};
 ~SgVexScanBlock() 
  {for (int i=0; i<parameters_.size(); i++) delete parameters_.at(i); parameters_.clear();};

  inline const QString& getKey() const {return key_;};
  inline const QList<SgVexParameter*>& getParameters() const {return parameters_;};

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVex(const QList<QString>& statements, int& idx);

protected:
  QString                       key_;
  QList<SgVexParameter*>        parameters_;
};
/*=====================================================================================================*/





/*=====================================================================================================*/
class SgVexSection
{
public:
  SgVexSection() : key_(""), statements_(), refStatements_(), defBlocks_(), scanBlocks_() {};
  SgVexSection(const QString& key) : key_(key), statements_(), refStatements_(), 
    defBlocks_(), scanBlocks_() {};

  SgVexSection(const SgVexSection& sect) : key_(sect.key_), statements_(sect.statements_), 
    refStatements_(sect.refStatements_), defBlocks_(sect.defBlocks_), scanBlocks_(sect.scanBlocks_)
    {};

 ~SgVexSection();


  inline const QString& getKey() const {return key_;};
  inline const QList<QString>& getStatements() const {return statements_;};
  inline const QList<SgVexRefSatement*>& getRefStatements() const {return refStatements_;};
  inline const QList<SgVexDefBlock*>& getDefBlocks() const {return defBlocks_;};
  inline const QList<SgVexScanBlock*>& getScanBlocks() const {return scanBlocks_;};


  inline const QMap<QString, SgVexRefSatement*>& getRefByKey() const {return refByKey_;};
  inline const QMap<QString, SgVexDefBlock*>& getDefByKey() const {return defByKey_;};
  inline const QMap<QString, SgVexScanBlock*>& getScanByKey() const {return scanByKey_;};

  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVex(const QList<QString>& statements, int& idx);

protected:
  QString                       key_;
  QList<QString>                statements_;
  QList<SgVexRefSatement*>      refStatements_;
  QList<SgVexDefBlock*>         defBlocks_;
  QList<SgVexScanBlock*>        scanBlocks_;

  QMap<QString, SgVexRefSatement*>
                                refByKey_;
  QMap<QString, SgVexDefBlock*> defByKey_;
  QMap<QString, SgVexScanBlock*>scanByKey_;

};
/*=====================================================================================================*/











/*=====================================================================================================*/
class SgVexFile
{
public:

  class IfSetup
  {
  public:
    inline IfSetup() : ifId_(""), ifName_(""), totalLo_(0.0), polarization_(CP_UNDEF), 
      netSideBand_(CSB_UNDEF) {};
    inline IfSetup(const QString& ifId, const QString& ifName, double totalLo, 
      SgChannelPolarization polarization, SgChannelSideBand netSideBand) : 
      ifId_(ifId), ifName_(ifName), totalLo_(totalLo), polarization_(polarization), 
      netSideBand_(netSideBand) {};
    inline IfSetup(const IfSetup& is) : ifId_(is.ifId_), ifName_(is.ifName_), totalLo_(is.totalLo_), 
      polarization_(is.polarization_), netSideBand_(is.netSideBand_) {};
    inline ~IfSetup() {};

    inline const QString& getIfId() const {return ifId_;};
    inline const QString& getIfName() const {return ifName_;};
    inline double getTotalLo() const {return totalLo_;};
    inline SgChannelPolarization getPolarization() const {return polarization_;};
    inline SgChannelSideBand getNetSideBand() const {return netSideBand_;};
  
    inline void setIfId(const QString& s) {ifId_ = s;};
    inline void setIfName(const QString& s) {ifName_ = s;};
    inline void setTotalLo(double d) {totalLo_ = d;};
    inline void setPolarization(SgChannelPolarization p) {polarization_ = p;};
    inline void setNetSideBand(SgChannelSideBand sb) {netSideBand_ = sb;};
  protected:
    QString                     ifId_;
    QString                     ifName_;
    double                      totalLo_;
    SgChannelPolarization       polarization_;
    SgChannelSideBand           netSideBand_;
  };
  //
  class BbcSetup
  {
  public:
    inline BbcSetup() : bbcId_(""), ifId_(""), physNumber_(0)
      {};
    inline BbcSetup(const QString& bbcId, const QString& ifId, int num) : 
      bbcId_(bbcId), ifId_(ifId), physNumber_(num) {};
    inline BbcSetup(const BbcSetup& bbcs) : bbcId_(bbcs.bbcId_), ifId_(bbcs.ifId_), 
      physNumber_(bbcs.physNumber_) {}; 
    inline ~BbcSetup() {};
    //
    inline const QString& getBbcId() const {return bbcId_;};
    inline const QString& getIfId() const {return ifId_;};
    inline int getPhysNumber() const {return physNumber_;};
    //
    inline void setBbcId(const QString& s) {bbcId_ = s;};
    inline void setIfId(const QString& s) {ifId_ = s;};
    inline void setPhysNumber(int n) {physNumber_ = n;};
  protected:
    QString                     bbcId_;
    QString                     ifId_;
    int                         physNumber_;
  };
  //
  class FreqSetup
  {
  public:
    inline FreqSetup() : 
      bandId_(""), skyFreq_(0.0), netSideBand_(CSB_UNDEF), bandwidth_(0), chanId_(""), bbcId_(""), 
      phaseCalId_("") {};
    inline FreqSetup(QString bandId, double skyFreq, SgChannelSideBand netSideBand, double bandwidth,
      QString chanId, QString bbcId, QString phaseCalId) : 
      bandId_(bandId), skyFreq_(skyFreq), netSideBand_(netSideBand), bandwidth_(bandwidth), 
      chanId_(chanId), bbcId_(bbcId), phaseCalId_(phaseCalId) {};
    inline FreqSetup(const FreqSetup& freqs) :
      bandId_(freqs.bandId_), skyFreq_(freqs.skyFreq_), netSideBand_(freqs.netSideBand_),
      bandwidth_(freqs.bandwidth_), chanId_(freqs.chanId_), bbcId_(freqs.bbcId_), 
      phaseCalId_(freqs.phaseCalId_) {};
    inline ~FreqSetup() {};
    //
    inline const QString& getBandId() const {return bandId_;};
    inline double getSkyFreq() const {return skyFreq_;};
    inline SgChannelSideBand getNetSideBand() const {return netSideBand_;};
    inline double getBandwidth() const {return bandwidth_;};
    inline const QString& getChanId() const {return chanId_;};
    inline const QString& getBbcId() const {return bbcId_;};
    inline const QString& getPhaseCalId() const {return phaseCalId_;};
    //
    inline void setBandId(const QString& s) {bandId_ = s;};
    inline void setSkyFreq(double f) {skyFreq_ = f;};
    inline void setNetSideBand(SgChannelSideBand sb) {netSideBand_ = sb;};
    inline void setBandwidth(double w) {bandwidth_ = w;};
    inline void setChanId(const QString& s) {chanId_ = s;};
    inline void setBbcId(const QString& s) {bbcId_ = s;};
    inline void setPhaseCalId(const QString& s) {phaseCalId_ = s;};
  protected:
    QString                     bandId_;
    double                      skyFreq_;
    SgChannelSideBand           netSideBand_;
    double                      bandwidth_;
    QString                     chanId_;
    QString                     bbcId_;
    QString                     phaseCalId_;
  };


  class StationSetup
  {
  public:
    inline StationSetup() : ifs_(), bbcs_(), freqs_()
      {};
    inline StationSetup(const StationSetup& ss) : ifs_(ss.ifs_), bbcs_(ss.bbcs_), freqs_(ss.freqs_)
      {};
    inline ~StationSetup() {ifs_.clear(); bbcs_.clear(); freqs_.clear();};
    //
    inline const QList<IfSetup*>& getIfs() const {return ifs_;};
    inline const QList<BbcSetup*>& getBbcs() const {return bbcs_;};
    inline const QList<FreqSetup*>& getFreqs() const {return freqs_;};
    //
    inline QList<IfSetup*>& ifs() {return ifs_;};
    inline QList<BbcSetup*>& bbcs() {return bbcs_;};
    inline QList<FreqSetup*>& freqs() {return freqs_;};
  protected:
    QList<IfSetup*>        ifs_;
    QList<BbcSetup*>       bbcs_;
    QList<FreqSetup*>      freqs_;
  };



  SgVexFile();
  SgVexFile(const QString& inputFileName);
 ~SgVexFile();


  //
  // Interfaces:
  //
  // gets:
  inline const QString& getInputFileName() const {return inputFileName_;};
  inline const QString& getExperName() const {return experName_;};
  inline const QString& getExperDescription() const {return experDescription_;};

  /**Returns nominal start epoch.
   */
  inline const SgMJD& getExperNominalStart() const {return experNominalStart_;};

  /**Returns nominal stop epoch.
   */
  inline const SgMJD& getExperNominalStop() const {return experNominalStop_;};

  inline const QString& getPiName() const {return piName_;};
  inline const QString& getPiEmail() const {return piEmail_;};
  inline const QString& getContactName() const {return contactName_;};
  inline const QString& getcontactEmail() const {return contactEmail_;};
  inline const QString& getSchedulerName() const {return schedulerName_;};
  inline const QString& getSchedulerEmail() const {return schedulerEmail_;};
  inline const QString& getTargetCorrelator() const {return targetCorrelator_;};






  // sets:

  inline void setInputFileName(const QString& name) {inputFileName_ = name;};
  inline void setExperName(const QString& name) {experName_ = name;};
  inline void setExperDescription(const QString& desc) {experDescription_ = desc;};

  /**Sets up nominal start epoch.
  */
  inline void setExperNominalStart(const SgMJD& t) {experNominalStart_ = t;};

  /**Sets up nominal stop epoch.
  */
  inline void setExperNominalStop(const SgMJD& t) {experNominalStop_ = t;};

  inline void setPiName(const QString& s) {piName_ = s;};
  inline void setPiEmail(const QString& s) {piEmail_ = s;};
  inline void setContactName(const QString& s) {contactName_ = s;};
  inline void setContactEmail(const QString& s) {contactEmail_ = s;};
  inline void setSchedulerName(const QString& s) {schedulerName_ = s;};
  inline void setSchedulerEmail(const QString& s) {schedulerEmail_ = s;};
  inline void setTargetCorrelator(const QString& s) {targetCorrelator_ = s;};


  inline const QList<SgVexSection*>& getSections() const {return sections_;};

  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  bool parseVexFile(const QString& fileName);


protected:
  QString                       inputFileName_;

  // global block:
  QString                       exper_;

  // exper block:
  QString                       experName_;
  QString                       experDescription_;
  SgMJD                         experNominalStart_;
  SgMJD                         experNominalStop_;
  QString                       piName_;
  QString                       piEmail_;
  QString                       contactName_;
  QString                       contactEmail_;
  QString                       schedulerName_;
  QString                       schedulerEmail_;
  QString                       targetCorrelator_; 
  
  
  QList<QString>                statements_;

  QList<SgVexSection*>          sections_;
  QMap<QString, SgVexSection*>  sectionByName_;

  QMap<QString, StationSetup*>  stnSetupById_;


  bool parseSection(int& idx);
  bool analyzeVex();

  
  const SgVexDefBlock* lookupDef(const QString& sectName, const QString& refName);
  const SgVexRefSatement* lookupRef(const QString& sectName, const QString& refName);
  const SgVexRefSatement* lookupRef(const SgVexDefBlock* defBlock, const QString& refCode);
  const SgVexRefSatement* lookupRef(const SgVexDefBlock* defBlock, const QString& refCode, 
    const QString& qualifier);


  IfSetup* ifSetup(const SgVexParameter* par);
  BbcSetup* bbcSetup(const SgVexParameter* par);
  FreqSetup* freqSetup(const SgVexParameter* par);

  double str2frq(const QString&) const;

};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // SG_VEX_H
