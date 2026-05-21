/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
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

#ifndef NS_SESSION_HANDLER_H
#define NS_SESSION_HANDLER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgVlbiSession.h>
#include <SgTaskConfig.h>
#include <SgParametersDescriptor.h>




class SgDbhImage;
class SgVgosDb;
class SgSolutionReporter;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsSessionHandler
{
  friend class NsScrPrx4SessionHandler;
public:
  enum SaveStatus
  {
    SS_UNDEF          =  0,
    SS_OK             =  1,
    SS_NOT_READY      =  2,
    SS_FILE_EXISTS    =  3,
    SS_SAVING_ERROR   =  4,
    SS_CATALOG_ERROR  =  5,
    SS_INTERNAL_ERROR =  6,
  };

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsSessionHandler(const QString& fileName, const QString& fileNameAux, 
    SgTaskConfig*, SgParametersDescriptor*, SgVlbiSessionInfo::OriginType);

  NsSessionHandler(SgTaskConfig*, SgParametersDescriptor*);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsSessionHandler();


  //
  // Interfaces:
  //
  inline bool isImportOk() const {return isImportOk_;};

  inline const QString& getFileName() const {return fileName_;};
  inline const QString& getFileNameAux() const {return fileNameAux_;};
  inline SgVlbiSessionInfo::OriginType getOtype() const {return oType_;};
  inline const SgTaskConfig* cfg() const {return config_;};
  inline const SgParametersDescriptor* parametersDescriptor() const {return parametersDescriptor_;};
  inline const SgVlbiSession* session() const {return session_;};


  inline void setFileName(const QString& f) {fileName_ = f;};
  inline void setFileNameAux(const QString& f) {fileNameAux_ = f;};
  inline void setOtype(SgVlbiSessionInfo::OriginType ot) {oType_ = ot;};
  inline SgTaskConfig* cfg() {return config_;};
  inline SgParametersDescriptor* parametersDescriptor() {return parametersDescriptor_;};
  inline SgVlbiSession* session() {return session_;};

  //
  // Functions:
  //
  inline QString className() const {return "NsSessionHandler";};

  bool importSession(bool guiExpected);
  bool performPia();
  bool saveResults();
  //
  void process();
  void calcIono();
  void zeroIono();
  void changeActiveBand(int idx);
  //
  void generateReport(bool isExtended=false);
  void generateReport4StcPars();
  void generateReport4Tzds();
  void generateAposterioriFiles();
  void exportDataToNgs();

  bool exportDataIntoVgosDa(QString output=QString(""));
  bool exportDataIntoVgosDb(QString output=QString(""));

  void generateMyReport();
  void addUserComment(const QString& comment);
 
  //
  // Friends:
  //


  //
  // I/O:
  //
  //


protected:
  // I/O generic:
  bool                          isImportOk_;
  SgVlbiSessionInfo::OriginType oType_;
  QString                       fileName_;
  QString                       fileNameAux_;
  // DBH stuff:
  QString                       path2DbhFiles_;
  QMap<QString, SgDbhImage*>    dbhImageByKey_;
  QMap<QString, QString>        dbhNameByKey_;
  QMap<QString, QString>        dbhPathByKey_;
  // vgosDb stuff:
  QString                       path2VdbFiles_;
  QString                       vdbWrapperFileName_;
  SgVgosDb                     *vgosDb_;
  SaveStatus                    saveStatus_;
  // vgosDa stuff:
  QString                       path2VdaFiles_;
  // session:
  SgVlbiSession                *session_;
  SgTaskConfig                 *config_;
  SgParametersDescriptor       *parametersDescriptor_;
  SgSolutionReporter           *reporter_;
  // aux:
  int                           activeBand_;
  
  // functions:
  void                          initData();
  bool                          initSessionFromDbh(bool guiExpected);
  bool                          initSessionFromOdb(bool guiExpected);
  bool                          initSessionFromVda(bool guiExpected);
  void                          setupCatsDbhFileNames(const QString& fName, const QString& fName2);
  void                          setupDogsDbhFileNames(const QString& fileName);
  void                          setupVgosDbFileNames(const QString& fileName);
  void                          setupVgosDaFileNames(const QString& fileName);
  int                           inquireCat(const QString&, int, QString&, QString&);
  int                           notifyCat(const QString&, bool, QString&);
  //
  bool                          exportDataIntoDbh();
  inline virtual bool           canSave() {return true;};
  inline void                   postSave() {};
};
/*=====================================================================================================*/


/*=====================================================================================================*/
#endif // NS_SESSION_HANDLER_H
