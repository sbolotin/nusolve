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

#ifndef NS_SCR_PRX_4_SESSION_HANDLER_H
#define NS_SCR_PRX_4_SESSION_HANDLER_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QtGlobal>



#include "NsSessionHandler.h"
#include "nuSolve.h"






class NsScrPrx4Session;
/***===================================================================================================*/
/**
 * 
 *
 */
/**====================================================================================================*/
class NsScrPrx4SessionHandler : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString fileName WRITE setFileName READ getFileName)
  Q_PROPERTY(QString fileNameAux WRITE setFileNameAux READ getFileNameAux)
  Q_PROPERTY(QString inputType WRITE setInputType READ getInputType)
  Q_PROPERTY(QString guessedSessionName READ getGuessedSessionName)


public:
  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsScrPrx4SessionHandler(NsSessionHandler&);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsScrPrx4SessionHandler();


  //
  // Interfaces:
  //
public slots:
  inline const QString& getFileName() const {return handler_->getFileName();}
  inline const QString& getFileNameAux() const {return handler_->getFileNameAux();};
  const QString& getInputType() const;
  inline const QString& getGuessedSessionName() const {return guessedSessionName_;};

  inline void setFileName(const QString& fname) {handler_->setFileName(fname); guessSessionName();};
  inline void setFileNameAux(const QString& fnameAux) {handler_->setFileNameAux(fnameAux);};
  void setInputType(const QString&);

  bool importSession();
  inline bool performPia() {return handler_->performPia();};
  bool saveResults();
  inline void exportDataToNgs() {handler_->exportDataToNgs();};
  inline bool saveDataAsVgosDa(const QString& output) {return handler_->exportDataIntoVgosDa(output);};
  inline bool saveDataAsVgosDb(const QString& output) {return handler_->exportDataIntoVgosDb(output);};
  inline void generateReport(bool isExtended=false) {handler_->generateReport(isExtended);};
  inline void generateReport4StcPars() {handler_->generateReport4StcPars();};
  inline void generateReport4Tzds() {handler_->generateReport4Tzds();};
  inline void generateAposterioriFiles() {handler_->generateAposterioriFiles();};
  inline void addUserComment2Report(const QString& comment) {handler_->addUserComment(comment);};
  //

public:
  //
  // Functions:
  //
  inline QString className() const {return "NsScrPrx4SessionHandler";};

  inline void setSessionProxy(NsScrPrx4Session *sProxy) {sessionProxy_=sProxy;};

  //
  // Friends:
  //


  //
  // I/O:
  //
  //

protected:
  NsSessionHandler             *handler_;
  NsScrPrx4Session             *sessionProxy_;
  QString                       guessedSessionName_;
  
  void guessSessionName();
};
/*=====================================================================================================*/



/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_SESSION_HANDLER_H
