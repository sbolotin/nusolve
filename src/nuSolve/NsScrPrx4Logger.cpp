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


#include "NsScrPrx4Logger.h"


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsScrPrx4Logger::NsScrPrx4Logger(SgLogger* logger)
{
  logger_ = logger;
  returnCode_ = 0;
};



//
NsScrPrx4Logger::~NsScrPrx4Logger()
{
  //
  logger_ = NULL;
};
/*=====================================================================================================*/







/*=====================================================================================================*/
//
// constants:
//

const QString                    str_LogErr("Log.Err");
const QString                    str_LogWrn("Log.Wrn");
const QString                    str_LogInf("Log.Inf");
const QString                    str_LogDbg("Log.Dbg");


const QString                    str_LogIoBin("Log.IoBin");
const QString                    str_LogIoTxt("Log.IoTxt");
const QString                    str_LogIoNcdf("Log.IoNcdf");
const QString                    str_LogIoDbh("Log.IoDbh");
const QString                    str_LogIo("Log.Io");

const QString                    str_LogMatrix("Log.Matrix");
const QString                    str_LogMatrix3D("Log.Matrix3d");
const QString                    str_LogInterp("Log.Interp");
const QString                    str_LogMath("Log.Math");

const QString                    str_LogObs("Log.Obs");
const QString                    str_LogStation("Log.Station");
const QString                    str_LogSource("Log.Source");
const QString                    str_LogSession("Log.Session");
const QString                    str_LogData("Log.Data");

const QString                    str_LogRefFrame("Log.RefFrame");
const QString                    str_LogTime("Log.Time");
const QString                    str_LogIono("Log.Iono");
const QString                    str_LogRefraction("Log.Refraction");
const QString                    str_LogDelay("Log.Delay");
const QString                    str_LogRate("Log.Rate");
const QString                    str_LogFlyBy("Log.FlyBy");
const QString                    str_LogDisplacement("Log.Displacement");
const QString                    str_LogGeo("Log.Geo");

const QString                    str_LogEstimator("Log.Estimator");
const QString                    str_LogPwl("Log.Pwl");
const QString                    str_LogStoch("Log.Stoch");
const QString                    str_LogConfig("Log.Config");
const QString                    str_LogGui("Log.Gui");
const QString                    str_LogReport("Log.Report");
const QString                    str_LogRun("Log.Run");
const QString                    str_LogPreproc("Log.Preproc");
const QString                    str_LogAll("Log.All");
/*=====================================================================================================*/
