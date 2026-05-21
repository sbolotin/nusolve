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
#include "nuSolve.h"
#include "NsScrPrx4ParametersDescriptor.h"



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
void NsScrPrx4ParametersDescriptor::setMode4Parameter (ParIdx idx, ParMode mode)
{
  parametersDescriptor_->
      setMode4Parameter((SgParameterCfg::PMode)mode, (SgParametersDescriptor::ParIdx)idx);
  if (idx==Clocks)
  {
    emit clocksParModeChanged((SgParameterCfg::PMode)mode);

    if (mode==EstimateArc || mode==EstimatePwl || mode==EstimateStochastic)
      mode = EstimateNo;

    parametersDescriptor_->
      setMode4Parameter((SgParameterCfg::PMode)mode, SgParametersDescriptor::Idx_CLOCK_1);
    parametersDescriptor_->
      setMode4Parameter((SgParameterCfg::PMode)mode, SgParametersDescriptor::Idx_CLOCK_2);
    parametersDescriptor_->
      setMode4Parameter((SgParameterCfg::PMode)mode, SgParametersDescriptor::Idx_CLOCK_3);
  }
  else if (idx==Zenith)
    emit zenithParModeChanged((SgParameterCfg::PMode)mode);

};


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
const QString                    str_EstimateNo("Parameters.EstimateNo");
const QString                    str_EstimateArc("Parameters.EstimateArc");
const QString                    str_EstimateLocal("Parameters.EstimateLocal");
const QString                    str_EstimatePwl("Parameters.EstimatePwl");
const QString                    str_EstimateStochastic("Parameters.EstimateStochastic");

const QString                    str_Clocks("Parameters.Clocks");
const QString                    str_Zenith("Parameters.Zenith");
const QString                    str_AtmGrad("Parameters.AtmGrad");
const QString                    str_Cable("Parameters.Cable");
const QString                    str_AxisOffset("Parameters.AxisOffset");
const QString                    str_StnCoo("Parameters.StnCoo");
const QString                    str_StnVel("Parameters.StnVel");
const QString                    str_SrcCoo("Parameters.SrcCoo");
const QString                    str_SrcSsm("Parameters.SrcSsm");
const QString                    str_PolusXy("Parameters.PolusXy");
const QString                    str_PolusXyR("Parameters.PolusXyR");
const QString                    str_PolusUt1("Parameters.PolusUt1");
const QString                    str_PolusUt1R("Parameters.PolusUt1R");
const QString                    str_PolusNut("Parameters.PolusNut");
const QString                    str_PolusNutR("Parameters.PolusNutR");
const QString                    str_Bl_Clk("Parameters.Bl_Clk");
const QString                    str_Bl_Length("Parameters.Bl_Length");
const QString                    str_Test("Parameters.Test");


/*=====================================================================================================*/





/*=====================================================================================================*/
