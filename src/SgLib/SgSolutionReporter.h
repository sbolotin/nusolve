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

#ifndef SG_SOLUTION_REPORTER
#define SG_SOLUTION_REPORTER


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QTextStream>


#include <SgEstimator.h>
#include <SgIdentities.h>
#include <SgMJD.h>



class SgSymMatrix;

class SgArcStorage;
class SgParameter;
class SgParameterCfg;
class SgParametersDescriptor;
class SgPwlStorage;
class SgTaskConfig;
class SgTaskManager;
class SgVlbiBand;
class SgVlbiSession;
class SgVlbiSourceInfo;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class SgSolutionReporter
{
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
  SgSolutionReporter(SgVlbiSession*, const SgIdentities&);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgSolutionReporter();


  //
  // Interfaces:
  //
  inline SgVlbiSession* getSession() const {return session_;};
  //
  inline const SgMJD& getRunEpoch() const;
  //
  inline const QString& getPath2APrioriFiles() const;
  //
  inline int getNumOfUnknowns() const;
  //
  inline int getNumOfObservations() const;
  //
  void setSession(SgVlbiSession*);
  //
  inline void setRunEpoch(const SgMJD& epoch);
  //
  inline void setPath2APrioriFiles(const QString& path);
  
  //
  inline SgIdentities* identities();
  
  //
  inline QMap<QString, SgParameter*> *parByName();

  inline QMap<QString, QMap<QString, SgParameter*> >* stcParByName() {return &stcParByName_;};

  inline const QMap<QString, SgPwlStorage*>&  pwlByName() {return pwlByName_;};

  inline const QMap<QString, SgArcStorage*>&  arcByName() {return arcByName_;};

  inline double dUt1Value() const {return dUt1Value_;};
  inline double dUt1Correction() const {return dUt1Correction_;};
  inline double dUt1StdDev() const {return dUt1StdDev_;};



  //
  // Functions:
  //
  void report2spoolFile(const QString& path, const QString& path2obsStatus, const QString& fileName,
    const QString& rptrDirName, bool=false, bool=false);
  void report2aposterioriFiles(const QString& path, const QString& fileNameBase);
  bool reportTotalZenithDelays(const QString& path);
  void reportStochasticEstimations(const QString& path);
  void report2MyFile(const QString& path, const QString& fileName);

  void absorbInfo(SgTaskManager*);
  // can be called separately:
  bool reportNotUsedObs(const QString& path, const QString& fileName);
  
  //
  void addUserComment(const QString& coment) {userComents_.append(coment);};
  

private:
  // sesson:
  SgVlbiSession                *session_;
  SgVlbiBand                   *activeBand_;
  QString                       activeBandKey_;
  // identities:
  SgIdentities                  identities_;
  SgMJD                         runEpoch_;
  SgMJD                         creationEpoch_;
  SgTaskConfig                 *config_;
  SgParametersDescriptor       *parametersDescriptor_;
  QString                       reportID_;
  // general stats:
  QString                       solutionTypeName_;
  int                           numOfUnknowns_;
  int                           numOfObservations_;
  int                           numOfConstraints_;
  // pathes and filenames:
  QString                       path2APrioriFiles_;
  // estimated parameters:
  QList<SgParameter*>           parList_;
  QList<SgParameter*>           allParList_;
  QList<SgPwlStorage*>          pwlList_;
  QList<SgArcStorage*>          arcList_;

  QMap<QString, SgEstimator::StochasticSolutionCarrier>
                                stcSolutions_;
  SgSymMatrix                  *PxAll_;
  double                        condNumber_;

  QMap<QString, SgParameter*>   parByName_;
  QMap<QString, QMap<QString, SgParameter*> >
                                stcParByName_;
  QMap<QString, SgPwlStorage*>  pwlByName_;
  QMap<QString, SgArcStorage*>  arcByName_;
  QList<SgVlbiSourceInfo*>      usedSources_;
  QList<SgVlbiSourceInfo*>      skippedSources_;
  // user comments:
  QList<QString>                userComents_;
  
  // a priori:
  SgMJD                         erpTref_;
  double                        erp_ut1_0_, erp_ut1_1_, erp_ut1_2_, erp_ut1_3_;
  double                        erp_pmx_0_, erp_pmx_1_, erp_pmx_2_, erp_pmx_3_;
  double                        erp_pmy_0_, erp_pmy_1_, erp_pmy_2_, erp_pmy_3_;
  double                        eop_cix_0_, eop_cix_1_, eop_cix_2_, eop_cix_3_;
  double                        eop_ciy_0_, eop_ciy_1_, eop_ciy_2_, eop_ciy_3_;


  double                        dUt1Value_;
  double                        dUt1Correction_;
  double                        dUt1StdDev_;

  // private functions:
  void freeResources();
  void synchronizeInfo();
  void evaluateUsedErpApriori();
  void evaluateUsedErpApriori2();
  void lookupParameter(SgParameter*);
  void calculateConditionNumber();

  // spoolfile output:
  void reportIdentitiesBlock_Output4Spoolfile(QTextStream&);
  void reportConfiguresBlock_Output4Spoolfile(QTextStream&);
  void reportMetStatsBlock_Output4Spoolfile(QTextStream&);
  void reportGeneralStatsBlock_Output4Spoolfile(QTextStream&);
  void reportBaselineStatsBlock_Output4Spoolfile(QTextStream&);
  void reportSourceStatsBlock_Output4Spoolfile(QTextStream&);
  void reportStationStatsBlock_Output4Spoolfile(QTextStream&);
  void reportAPrioriBlock_Output4Spoolfile(QTextStream&);
  void reportEstimationBlock_Output4Spoolfile(QTextStream&);
  void reportResidualsBlock_Output4Spoolfile_old(QTextStream&);
  void reportResidualsBlock_Output4Spoolfile(QTextStream&);
  void reportBaselineVariationsBlock_Output4Spoolfile(QTextStream&);
  void reportDeselectedObsBlock_Output4Spoolfile(QTextStream&);
  void reportDeselectedObsBlock_Output4Spoolfile_v2(QTextStream&);
  void reportDeselectedObsBlock_Output4Spoolfile_v3(QTextStream&);

  bool reportAtmo(const QString& path, const QString& fileName); // ATMO<U.I.>
  bool reportCloc(const QString& path, const QString& fileName); // CLOC<U.I.>
  bool reportPall(const QString& path, const QString& fileName); // PALL<U.I.>

  bool reportSources_Output4AposterioriFiles(const QString& path, const QString& fileName);
  bool reportStations_Output4AposterioriFiles(const QString& path, const QString& fileName);
  
  bool reportStoch4Stn(const QString& path);

  bool reportStochParameter(const QString& key, const SgParameterCfg& parCfg, 
    const QString& path, const QString& fileName);

};
/*=====================================================================================================*/






/*=====================================================================================================*/
/*                                                                                                     */
/* SgSolutionReporter inline members:                                                                  */
/*                                                                                                     */
/*=====================================================================================================*/
//
//
// CONSTRUCTORS:
//

//
// INTERFACE:
//
//
//
inline const SgMJD& SgSolutionReporter::getRunEpoch() const
{
  return runEpoch_;
};



//
inline const QString& SgSolutionReporter::getPath2APrioriFiles() const
{
  return path2APrioriFiles_;
};



//
inline int SgSolutionReporter::getNumOfUnknowns() const
{
  return numOfUnknowns_;
};



//
inline int SgSolutionReporter::getNumOfObservations() const
{
  return numOfObservations_;
};




//
inline void SgSolutionReporter::setRunEpoch(const SgMJD& epoch)
{
  runEpoch_ = epoch;
};



//
inline void SgSolutionReporter::setPath2APrioriFiles(const QString& path)
{
  path2APrioriFiles_ = path;
};



//
inline SgIdentities* SgSolutionReporter::identities()
{
  return &identities_;
};



//
inline QMap<QString, SgParameter*>* SgSolutionReporter::parByName()
{
  return &parByName_;
};





//
// FUNCTIONS:
//
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
#endif //SG_SOLUTION_REPORTER
