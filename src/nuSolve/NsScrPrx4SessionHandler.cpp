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


#include <iostream>
#include <stdlib.h>

#include <QtCore/QRegExp>
#include <QtCore/QRegularExpression>

#include "NsScrPrx4SessionHandler.h"
#include "NsScrPrx4Session.h"


const QString                    str_OT_DBH("DBH");
const QString                    str_OT_VDB("VDB");
const QString                    str_OT_VDA("VDA");
const QString                    str_OT_UND("---");


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsScrPrx4SessionHandler::NsScrPrx4SessionHandler(NsSessionHandler& handler)
 : guessedSessionName_("")
{
  handler_ = &handler;
  sessionProxy_ = NULL;
};



//
NsScrPrx4SessionHandler::~NsScrPrx4SessionHandler()
{
  //
  handler_ = NULL;
  sessionProxy_ = NULL;
};



//
const QString& NsScrPrx4SessionHandler::getInputType() const
{
  if (handler_->getOtype() == SgVlbiSessionInfo::OT_DBH)
    return str_OT_DBH;
  else if (handler_->getOtype() == SgVlbiSessionInfo::OT_VDB)
    return str_OT_VDB;
  else if (handler_->getOtype() == SgVlbiSessionInfo::OT_AGV)
    return str_OT_VDA;
  else
    return str_OT_UND;
};



//
void NsScrPrx4SessionHandler::setInputType(const QString& inputType)
{
  if (inputType == str_OT_DBH)
    handler_->setOtype(SgVlbiSessionInfo::OT_DBH);
  else if (inputType == str_OT_VDB)
    handler_->setOtype(SgVlbiSessionInfo::OT_VDB);
  else if (inputType == str_OT_VDA)
    handler_->setOtype(SgVlbiSessionInfo::OT_AGV);
  else
    handler_->setOtype(SgVlbiSessionInfo::OT_UNKNOWN);
};



//
bool NsScrPrx4SessionHandler::importSession()
{
  bool                           isOk;
  if ((isOk=handler_->importSession(false)))
    sessionProxy_->postLoad();
  sessionProxy_->setIsOk(isOk);
  return isOk;
};



//
bool NsScrPrx4SessionHandler::saveResults()
{
  bool                           isOk(false);
  if (handler_->getOtype() == SgVlbiSessionInfo::OT_DBH)
    isOk = handler_->exportDataIntoDbh();
  else if (handler_->getOtype() == SgVlbiSessionInfo::OT_VDB)
    isOk = handler_->exportDataIntoVgosDb();
  else if (handler_->getOtype() == SgVlbiSessionInfo::OT_AGV)
    isOk = handler_->exportDataIntoVgosDa();
  return isOk;
};



//
void NsScrPrx4SessionHandler::guessSessionName()
{
  guessedSessionName_ = "undefined";
  const QString                 fName(handler_->getFileName());
/*
  int                           idx;
  QRegExp                       reSnV(".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2})_V([0-9]{3}).*");
  QRegExp                       reSn (".*([0-9]{2}[A-Z]{3}[0-9]{2}[A-Z0-9]{1,2}).*");
*/
  QRegularExpression            reOldDbName("^(\\d{2}[A-Z]{3}\\d{2}[A-Z0-9]{1,2}).*");
  QRegularExpression            reNewDbName("^(\\d{8})-([^\\s_]{2,12}).*");
	QRegularExpressionMatch       match;

  if ((match=reOldDbName.match(fName)).hasMatch())         // dbName: 12JUL16XA
  {
    guessedSessionName_ = match.captured(1);
  }
  else if ((match=reNewDbName.match(fName)).hasMatch())
  {
    guessedSessionName_ = match.captured(1) + "-" + match.captured(2);
  };
/*
  if (-1 < reSnV.indexIn(fName))
    guessedSessionName_ = reSnV.cap(1);// + "_V" + reSnV.cap(2);
  else if (-1 < reSn.indexIn(fName))
    guessedSessionName_ = reSn.cap(1);
  else if ((idx=fName.lastIndexOf("/")) != -1)
    guessedSessionName_ = fName.mid(idx + 1);
*/
};
/*=====================================================================================================*/






/*=====================================================================================================*/
