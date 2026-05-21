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
#include "nuSolve.h"
#include "NsScrSupport.h"


#include <iostream>



#include <QtCore/QFile>
#include <QtCore/QFileInfo>


#if QT_VERSION >= 0x050000
#    include <QtWidgets/QApplication>
#    include <QtWidgets/QMessageBox>
#    include <QtWidgets/QPushButton>
#else
#    include <QtGui/QApplication>
#    include <QtGui/QMessageBox>
#    include <QtGui/QPushButton>
#endif

#include <SgVlbiSession.h>

#if 0 < HAVE_SCRIPTS

#include "NsScrPrx4Logger.h"
#include "NsScrPrx4Observation.h"
#include "NsScrPrx4ParametersDescriptor.h"
#include "NsScrPrx4Session.h"
#include "NsScrPrx4SessionHandler.h"
#include "NsScrPrx4Setup.h"
#include "NsScrPrx4TaskConfig.h"

#endif


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/





/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//














/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
QScriptValue myPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
  QString result;
  for (int i = 0; i < context->argumentCount(); ++i)
  {
    if (i > 0)
      result.append(" ");
    result.append(context->argument(i).toString());
  };
  std::cout << qPrintable(result) << std::endl;
  return engine->undefinedValue();
};






//
int executeScript(const QString& scriptFileName, const QList<QString> args, bool hasDisplay)
{
  QString                       str("");
  int                           rc;
  rc = 0;

#if 0 < HAVE_SCRIPTS

  QFile                          file(scriptFileName);
 
  if (!file.exists())
  {
#ifdef SWDATA
    str = SWDATA;
#endif
    if (str.size())
    {
      file.setFileName(str + "/scripts/" + scriptFileName);
      if (!file.exists())
      {
        std::cout << "Error: cannot not find the file " << qPrintable(scriptFileName) 
                  << " neither in the current directory nor in the system one, " 
                  << qPrintable(str + "/scripts/") << "\n";
        return 1;
      };
    }
    else
    {
      std::cout << "Error: cannot not find the file " << qPrintable(scriptFileName) << "\n";
      return 1;
    };
  };
 
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    std::cout << "Error: cannot not open the file " << qPrintable(scriptFileName) << "\n";
    return 2;
  };

	
  if (logger->isEligible(SgLogger::DBG, SgLogger::IO_TXT))
  {
		QFileInfo										fi(file);
		str = "Executing " + fi.absoluteFilePath() + " script"; // for debug purposes
    //std::cout << qPrintable(str) << "\n";
		logger->write(SgLogger::DBG, SgLogger::IO_TXT, nuSolveVersion.name() + ": executeScript(): " + str);
	};

  QString                       strProgram(file.readAll());
  file.close();

  QScriptEngine                  interpreter;
  

  // do static check so far of code:
  if (QScriptSyntaxCheckResult::Valid != interpreter.checkSyntax(strProgram).state())
  {
    std::cout << "Error: bad syntax of the file " << qPrintable(scriptFileName) << "\n";
    return 3;
  };
 
  // actually do the eval:
//std::cout << "program: \n" << qPrintable(strProgram) << "\n";

//-  QPushButton                   button;
//-  QScriptValue                   scriptButton = interpreter.newQObject(&button);
//-  interpreter.globalObject().setProperty("button", scriptButton);

  /*
  NsVlbiSessionProxy            session;
  QScriptValue                   scriptSession = scriptEngine.newQObject(&session);
  scriptEngine.globalObject().setProperty("session", scriptSession);
  */
  

  qScriptRegisterMetaType(&interpreter, toScriptValue4ParIdx, fromScriptValue4ParIdx);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ParMode, fromScriptValue4ParMode);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigDtype, fromScriptValue4ConfigDtype);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigRtype, fromScriptValue4ConfigRtype);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigWrMode, fromScriptValue4ConfigWrMode);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigOpMode, fromScriptValue4ConfigOpMode);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigOpAction, fromScriptValue4ConfigOpAction);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigEpwlMode, fromScriptValue4ConfigEpwlMode);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigRefractionModel, 
    fromScriptValue4ConfigRefractionModel);
  qScriptRegisterMetaType(&interpreter, toScriptValue4ConfigCableCalSource, 
    fromScriptValue4ConfigCableCalSource);

  qScriptRegisterMetaType(&interpreter, toScriptValue4Band, fromScriptValue4Band);
  qScriptRegisterMetaType(&interpreter, toScriptValue4Stn, fromScriptValue4Stn);
  qScriptRegisterMetaType(&interpreter, toScriptValue4Bln, fromScriptValue4Bln);
  qScriptRegisterMetaType(&interpreter, toScriptValue4Src, fromScriptValue4Src);
  qScriptRegisterMetaType(&interpreter, toScriptValue4VlbiObs, fromScriptValue4VlbiObs);
  qScriptRegisterMetaType(&interpreter, toScriptValue4VlbiAuxObs, fromScriptValue4VlbiAuxObs);
  qScriptRegisterMetaType(&interpreter, toScriptValue4LogLevel, fromScriptValue4LogLevel);
  qScriptRegisterMetaType(&interpreter, toScriptValue4LogFacility, fromScriptValue4LogFacility);

  qScriptRegisterSequenceMetaType< QList<NsScrPrx4ParametersDescriptor::ParIdx> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4Band*> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4Station*> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4Baseline*> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4Source*> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4VlbiObservation*> >(&interpreter);
  qScriptRegisterSequenceMetaType< QList<NsScrPrx4VlbiAuxObservation*> >(&interpreter);


  // args:
  QScriptValue                   args4Script=qScriptValueFromSequence(&interpreter, args);
  interpreter.globalObject().setProperty("args", args4Script);
  
  
  // handler:
  NsSessionHandler             *handler;
  handler = new NsSessionHandler(&config, &parametersDescriptor);
  NsScrPrx4SessionHandler       handlerProxy(*handler);
  QScriptValue                  handlerScript=interpreter.newQObject(&handlerProxy);
  interpreter.globalObject().setProperty("handler", handlerScript);

  // config:
  NsScrPrx4TaskConfig           config4Scr(*handler->cfg());
  QScriptValue                  scriptConfig=interpreter.newQObject(&config4Scr);
  interpreter.globalObject().setProperty("config", scriptConfig);

  QScriptValue                  metaScrConfig=interpreter.newQMetaObject(config4Scr.metaObject());
  interpreter.globalObject().setProperty("CFG", metaScrConfig);
  
  // setup:
  NsScrPrx4Setup                setup4Scr(setup);
  QScriptValue                  scriptSetup=interpreter.newQObject(&setup4Scr);
  interpreter.globalObject().setProperty("setup", scriptSetup);
  
  // parameters descriptor:
  NsScrPrx4ParametersDescriptor parsDescript4Scr(*handler->parametersDescriptor());
  QScriptValue                  scriptParsDescript=interpreter.newQObject(&parsDescript4Scr);
  interpreter.globalObject().setProperty("parsDescript", scriptParsDescript);

  QScriptValue                  metaScrParsDescript=
                                  interpreter.newQMetaObject(parsDescript4Scr.metaObject());
  interpreter.globalObject().setProperty("Parameters", metaScrParsDescript);

  // session:
  NsScrPrx4Session              session4Scr(*handler->session());
  QScriptValue                  scriptSession=interpreter.newQObject(&session4Scr);
  interpreter.globalObject().setProperty("session", scriptSession);

  QScriptValue                  metaScrSession=
                                  interpreter.newQMetaObject(session4Scr.metaObject());
  interpreter.globalObject().setProperty("Session", metaScrSession);
  handlerProxy.setSessionProxy(&session4Scr);

  // logger:
  NsScrPrx4Logger               logger4Scr(logger);
  QScriptValue                  scriptLogger=interpreter.newQObject(&logger4Scr);
  interpreter.globalObject().setProperty("logger", scriptLogger);

  QScriptValue                  metaScrLogger=interpreter.newQMetaObject(logger4Scr.metaObject());
  interpreter.globalObject().setProperty("Log", metaScrLogger);


  QObject::connect(&parsDescript4Scr, SIGNAL(clocksParModeChanged(SgParameterCfg::PMode)),
    &session4Scr, SLOT(dispatchChangeOfClocksParameterModel(SgParameterCfg::PMode)));
  QObject::connect(&parsDescript4Scr,  SIGNAL(zenithParModeChanged(SgParameterCfg::PMode)),
    &session4Scr, SLOT(dispatchChangeOfZenithParameterModel(SgParameterCfg::PMode)));


  interpreter.globalObject().setProperty("nsPrint", interpreter.newFunction(myPrintFunction));
 
  
  //
  //
  //
  QScriptValue                  result=interpreter.evaluate(strProgram, scriptFileName);
  rc = logger4Scr.getReturnCode(); // a script can provide its return code through logger's proxy
  
  if (result.isError())
  {
    QString                     strError(QString::fromLatin1("%0:%1: %2")
                                  .arg(scriptFileName)
                                  .arg(result.property("lineNumber").toInt32())
                                  .arg(result.toString()));
    if (hasDisplay)
    {
      QMessageBox::critical(0, "Error running script", strError);
    }
    else
    {
      std::cerr << "ERROR: " << qPrintable(strError) << "\n";
    };
    return -1;
  };
  //
  delete handler;
  //
#else
  rc = -1;
  std::cout << "script mode is not supported\n";
#endif
  //
  return rc;
};
/*=====================================================================================================*/













/*=====================================================================================================*/

