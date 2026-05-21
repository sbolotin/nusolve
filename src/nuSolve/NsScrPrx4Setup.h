/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2017-2020 Sergei Bolotin.
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

#ifndef NS_SCR_PRX_4_SETUP_H
#define NS_SCR_PRX_4_SETUP_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif



#include <QtCore/QtGlobal>
#include <QtCore/QDir>
#include <QtCore/QObject>

#if 0 < HAVE_SCRIPTS
#    include <QtScript/QScriptEngine>
#    include <QtScript/QScriptValue>
#endif

#include "NsSetup.h"


class QString;

/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4Setup : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString path2Home
    WRITE setPath2Home READ getPath2Home)
  Q_PROPERTY(QString path2CatNuInterfaceExec
    WRITE setPath2CatNuInterfaceExec READ getPath2CatNuInterfaceExec)
  Q_PROPERTY(QString path2DbhFiles
    WRITE setPath2DbhFiles READ getPath2DbhFiles)
  Q_PROPERTY(QString path2VgosDbFiles
    WRITE setPath2VgosDbFiles READ getPath2VgosDbFiles)
  Q_PROPERTY(QString path2VgosDaFiles
    WRITE setPath2VgosDaFiles READ getPath2VgosDaFiles)
  Q_PROPERTY(QString path2APrioriFiles
    WRITE setPath2APrioriFiles READ getPath2APrioriFiles)
  Q_PROPERTY(QString path2TrpFiles
    WRITE setPath2TrpFiles READ getPath2TrpFiles)
  Q_PROPERTY(QString path2MasterFiles
    WRITE setPath2MasterFiles READ getPath2MasterFiles)
  Q_PROPERTY(QString path2SpoolFileOutput
    WRITE setPath2SpoolFileOutput READ getPath2SpoolFileOutput)
  Q_PROPERTY(QString path2NotUsedObsFileOutput
    WRITE setPath2NotUsedObsFileOutput READ getPath2NotUsedObsFileOutput)
  Q_PROPERTY(QString path2ReportOutput
    WRITE setPath2ReportOutput READ getPath2ReportOutput)
  Q_PROPERTY(QString path2NgsOutput
    WRITE setPath2NgsOutput READ getPath2NgsOutput)
  Q_PROPERTY(QString path2PlotterOutput
    WRITE setPath2PlotterOutput READ getPath2PlotterOutput)
  Q_PROPERTY(QString path2AuxLogs
    WRITE setPath2AuxLogs READ getPath2AuxLogs)
  Q_PROPERTY(QString pwd
    READ getPwd)
  Q_PROPERTY(bool have2SavePerSessionLog
    WRITE setHave2SavePerSessionLog READ getHave2SavePerSessionLog)
  Q_PROPERTY(bool have2UpdateCatalog
    WRITE setHave2UpdateCatalog READ getHave2UpdateCatalog)
  Q_PROPERTY(bool have2MaskSessionCode
    WRITE setHave2MaskSessionCode READ getHave2MaskSessionCode)
  Q_PROPERTY(bool have2KeepSpoolFileReports
    WRITE setHave2KeepSpoolFileReports READ getHave2KeepSpoolFileReports)
  Q_PROPERTY(bool have2LoadImmatureSession
    WRITE setHave2LoadImmatureSession READ getHave2LoadImmatureSession)
  Q_PROPERTY(bool have2AutoloadAllBands
    WRITE setHave2AutoloadAllBands READ getHave2AutoloadAllBands)


public:
  inline NsScrPrx4Setup(NsSetup& setup, QObject *parent=0) : QObject(parent) {setup_=&setup;};
  inline ~NsScrPrx4Setup() {setup_=NULL;};


public slots:
  inline QString getPath2Home() const {return setup_->getPath2Home();};
  inline QString getPath2CatNuInterfaceExec() const {return setup_->getPath2CatNuInterfaceExec();};
  inline QString getPath2DbhFiles() const {return setup_->getPath2DbhFiles();};
  inline QString getPath2VgosDbFiles() const {return setup_->getPath2VgosDbFiles();};
  inline QString getPath2VgosDaFiles() const {return setup_->getPath2VgosDaFiles();};
  inline QString getPath2APrioriFiles() const {return setup_->getPath2APrioriFiles();};
  inline QString getPath2TrpFiles() const {return setup_->getPath2TrpFiles();};
  inline QString getPath2MasterFiles() const {return setup_->getPath2MasterFiles();};
  inline QString getPath2SpoolFileOutput() const {return setup_->getPath2SpoolFileOutput();};
  inline QString getPath2NotUsedObsFileOutput() const {return setup_->getPath2NotUsedObsFileOutput();};
  inline QString getPath2ReportOutput() const {return setup_->getPath2ReportOutput();};
  inline QString getPath2NgsOutput() const {return setup_->getPath2NgsOutput();};
  inline QString getPath2PlotterOutput() const {return setup_->getPath2PlotterOutput();};
  inline QString getPath2AuxLogs() const {return setup_->getPath2AuxLogs();};
  inline QString getPwd() const {return QDir::currentPath();};
  //
  inline bool getHave2SavePerSessionLog() const {return setup_->getHave2SavePerSessionLog();};
  inline bool getHave2UpdateCatalog() const {return setup_->getHave2UpdateCatalog();};
  inline bool getHave2MaskSessionCode() const {return setup_->getHave2MaskSessionCode();};
  inline bool getHave2KeepSpoolFileReports() const {return setup_->getHave2KeepSpoolFileReports();};
  inline bool getHave2LoadImmatureSession() const {return setup_->getHave2LoadImmatureSession();};
  inline bool getHave2AutoloadAllBands() const {return setup_->getHave2AutoloadAllBands();};

  //
  inline void setPath2Home(QString path2) {setup_->setPath2Home(path2);};
  inline void setPath2CatNuInterfaceExec(QString path2) {setup_->setPath2CatNuInterfaceExec(path2);};
  inline void setPath2DbhFiles(QString path2) {setup_->setPath2DbhFiles(path2);};
  inline void setPath2VgosDbFiles(QString path2) {setup_->setPath2VgosDbFiles(path2);};
  inline void setPath2VgosDaFiles(QString path2) {setup_->setPath2VgosDaFiles(path2);};
  inline void setPath2APrioriFiles(QString path2) {setup_->setPath2APrioriFiles(path2);};
  inline void setPath2TrpFiles(QString path2) {setup_->setPath2TrpFiles(path2);};
  inline void setPath2MasterFiles(QString path2) {setup_->setPath2MasterFiles(path2);};
  inline void setPath2SpoolFileOutput(QString path2) {setup_->setPath2SpoolFileOutput(path2);};
  inline void setPath2NotUsedObsFileOutput(QString path2) {setup_->setPath2NotUsedObsFileOutput(path2);};
  inline void setPath2ReportOutput(QString path2) {setup_->setPath2ReportOutput(path2);};
  inline void setPath2NgsOutput(QString path2) {setup_->setPath2NgsOutput(path2);};
  inline void setPath2PlotterOutput(QString path2) {setup_->setPath2PlotterOutput(path2);};
  inline void setPath2AuxLogs(QString path2) {setup_->setPath2AuxLogs(path2);};
  //
  inline void setHave2SavePerSessionLog(bool have) {setup_->setHave2SavePerSessionLog(have);};
  inline void setHave2UpdateCatalog(bool have) {setup_->setHave2UpdateCatalog(have);};
  inline void setHave2MaskSessionCode(bool have) {setup_->setHave2MaskSessionCode(have);};
  inline void setHave2KeepSpoolFileReports(bool have) {setup_->setHave2KeepSpoolFileReports(have);};
  inline void setHave2LoadImmatureSession(bool have) {setup_->setHave2LoadImmatureSession(have);};
  inline void setHave2AutoloadAllBands(bool have) {setup_->setHave2AutoloadAllBands(have);};
  
private:
  NsSetup                        *setup_;
};
/*=====================================================================================================*/








/*=====================================================================================================*/
//
// aux functions:
//



/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_SETUP_H
