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

#include <QtCore/QDataStream>


#include <SgParametersDescriptor.h>

#include <SgConstants.h>
#include <SgLogger.h>
#include <SgMathSupport.h>
#include <SgMJD.h>




const int SgParametersDescriptor::num_(NUMBER_OF_KNOWN_PARAMETER_SPECIES);
/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgParametersDescriptor::className()
{
  return "SgParamatersDescriptor";
};



//
int SgParametersDescriptor::num()
{
  return num_;
};



//
SgParametersDescriptor::SgParametersDescriptor()
{
  //SgParameterCfg::SgParameterCfg(const QString& name,
  //                              PMode pMode, SType sType,
  // double convAPriori, double arcStep, double stocAPriori, double breakNoise, double tau,
  // double whiteNoise, double pwlStep, double pwlAPriori, int pwlNumOfPolinomials,
  // double scale, const QString& scaleName);
  // Clock0()
  parameters_[Idx_CLOCK_0] =    SgParameterCfg("Clock offset",
                                SgParameterCfg::PM_LOC, SgParameterCfg::ST_RANDWALK,
                                1.0e24, 1.0/8, 1.0e8, 1.0e8, 1.0,
                                16.0, 1.0/24.0, 72.0*24.0, 3,
                                1.0e12, "ps");
                                //               5.0ps/hr as in SOLVE
 
  // Clock1()
  parameters_[Idx_CLOCK_1] =    SgParameterCfg("Clock trend",
                                SgParameterCfg::PM_LOC, SgParameterCfg::ST_RANDWALK,
                                1.0e24, 1.0, 1.0e8, 1.0e8, 1.0,
                                1.0, 1.0/24.0, 5.0*24.0, 1,
                                1.0e12/DAY2SEC, "ps/day");

  // Clock2()
  parameters_[Idx_CLOCK_2] =    SgParameterCfg("Clock 2-nd order",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e24, 1.0, 1.0e8, 1.0e8, 1.0,
                                1.0, 1.0/24.0, 5.0*24.0, 1,
                                1.0e12/DAY2SEC/DAY2SEC, "ps/(day)^2");

  // Clock3()
  parameters_[Idx_CLOCK_3] =    SgParameterCfg("Clock 3-nd order",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e24, 1.0, 1.0e8, 1.0e8, 1.0,
                                1.0, 1.0/24.0, 5.0*24.0, 1,
                                1.0e12, "ps/(day^3)");

  // Zenith
  parameters_[Idx_ZENITH] =     SgParameterCfg("Zenith delay",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e6, 1.0/6, 1.0e4, 1.0e4, 1.0,
                                1.5, 1.0/24.0, 40.0e-12*24.0*vLight*100.0, 1,
                                1.0e2, "cm");
                                //             40ps/hr

  // AtmGrad
  parameters_[Idx_ATMGRAD] =    SgParameterCfg("Atmospheric gradients",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e6, 1.0/4, 1.0e4, 1.0e4, 1.0,
                                0.06, 1.2, 1.0e-3*24.0*vLight*100, 1,
                                1.0e2, "cm");
//                              0.06, 1.2, 0.5e-12*24.0*vLight*100, 1.0e2, "cm");
                                //          .5ps/hr

  // Cable
  parameters_[Idx_CABLE] =      SgParameterCfg("Cable Calibration multiplier",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e6, 1.0, 1.0e6, 1.0e6, 1.0,
                                1.0, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0, "units");

  // AxisOffset
  parameters_[Idx_AXISOFFSET] = SgParameterCfg("Axis offset",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e6, 1.0, 1.0e6, 1.0e6, 1.0,
                                0.1, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e2, "cm");

  // StnCoo
  parameters_[Idx_STNCOO] =     SgParameterCfg("Stations coordinates",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e6, 1.0e6, 1.0,
                                0.1, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e2, "cm");

  // StnVel
  parameters_[Idx_STNVEL] =     SgParameterCfg("Stations velocities",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e7, 1.0, 1.0e7, 1.0e7, 1.0,
                                0.1, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e3, "mm/year");

  // SrcCoo
  parameters_[Idx_SRCCOO] =     SgParameterCfg("Sources coordinates",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e8, 1.0e8, 1.0,
                                0.16, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e3*RAD2SEC, "mas");
  // SrcSsm
  parameters_[Idx_SRCSSM] =     SgParameterCfg("Sources SSM",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e8, 1.0e8, 1.0,
                                1.0, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0, "unitless");

  // PolusXY
  parameters_[Idx_POLUSXY] =    SgParameterCfg("Polar motion, px and py",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0/48.0, 1.0e4, 1.0e4, 1.0,
                                0.18, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e3*RAD2SEC, "mas");
  // PolusXYRate
  parameters_[Idx_POLUSXYR] =   SgParameterCfg("Polar motion rates, pxr and pyr",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e4, 1.0e4, 1.0,
                                0.18, 1.0/4.0, 1.0e6*24.0, 1,
                                1.0e3*RAD2SEC, "mas/day");

  // PolusUT1
  parameters_[Idx_POLUSUT1] =   SgParameterCfg("Earth rotation, d(UT1-UTC)",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e4, 1.0e4, 1.0,
                                0.012, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e3*DAY2SEC, "ms");

  // PolusUT1Rate
  parameters_[Idx_POLUSUT1R] =  SgParameterCfg("Rate of Earth rotation, d(UT1-UTC) rate",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e4, 1.0e4, 1.0,
                                0.012, 1.0/4.0, 1.0e6*24.0, 1,
                                1.0e3*DAY2SEC, "ms/d");

  // PolusNut
  parameters_[Idx_POLUSNUT] =   SgParameterCfg("Angles of nutation",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e4, 1.0e4, 1.0,
                                0.18, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e3*RAD2SEC, "mas");

  // PolusNutRate
  parameters_[Idx_POLUSNUTR] =  SgParameterCfg("Rates of angles of nutation",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e4, 1.0e4, 1.0,
                                0.18, 1.0/4.0, 1.0e6*24.0, 1,
                                1.0e3*RAD2SEC, "mas/day");

  parameters_[Idx_BL_CLK]  =    SgParameterCfg("Baseline clock offset",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e8, 1.0e8, 1.0,
                                16.0, 1.0/24.0, 72.0*24.0, 1,
                                1.0e12, "ps");

  parameters_[Idx_BL_LENGTH] =  SgParameterCfg("Baseline vector",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e8, 1.0, 1.0e6, 1.0e6, 1.0,
                                0.1, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0e2, "cm");

  // Test
  parameters_[Idx_TEST] =       SgParameterCfg("Test parameter",
                                SgParameterCfg::PM_NONE, SgParameterCfg::ST_RANDWALK,
                                1.0e10, 1.0, 1.0e10, 1.0e10, 1.0,
                                1.0, 1.0/24.0, 1.0e6*24.0, 1,
                                1.0, "units");

//SgParameterCfg::SgParameterCfg(const QString& name, PMode pMode, SType sType, 
//    double convAPriori, double arcStep, double stocAPriori, double breakNoise, double tau,
//    double whiteNoise, double pwlStep, double pwlAPriori, int pwlNumOfPolynomials,
//    double scale, const QString& scaleName)
};



//
void SgParametersDescriptor::unsetAllParameters()
{
  for (int i=0; i<num(); i++)
    parameters_[i].setPMode(SgParameterCfg::PM_NONE);
};



//
void SgParametersDescriptor::setMode4Parameter (SgParameterCfg::PMode mode, ParIdx idx)
{
  parameters_[idx].setPMode(mode);
};



//
void SgParametersDescriptor::unsetParameter(ParIdx idx)
{
  parameters_[idx].setPMode(SgParameterCfg::PM_NONE);
};



//
void SgParametersDescriptor::setMode4Parameters(SgParameterCfg::PMode mode, const QList<ParIdx>& idxs)
{
  for (int i=0; i<idxs.size(); i++)
    parameters_[idxs.at(i)].setPMode(mode);
};



//
void SgParametersDescriptor::unsetParameters(const QList<ParIdx>& idxs)
{
  for (int i=0; i<idxs.size(); i++)
    parameters_[idxs.at(i)].setPMode(SgParameterCfg::PM_NONE);
};



//
bool SgParametersDescriptor::saveIntermediateResults(QDataStream& s) const
{
  for (int i=0; i<num(); i++)
  {
    parameters_[i].saveIntermediateResults(s);
    if (s.status() != QDataStream::Ok)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": saveIntermediateResults(): error writting data #" + QString("").setNum(i));
      return false;
    };
  };
  if (s.status() != QDataStream::Ok)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
      ": saveIntermediateResults(): error writting data");
    return false;
  };
  return s.status() == QDataStream::Ok;
};



//
bool SgParametersDescriptor::loadIntermediateResults(QDataStream& s)
{
  for (int i=0; i<num(); i++)
  {
    parameters_[i].loadIntermediateResults(s);
    if (s.status() != QDataStream::Ok)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_BIN, className() +
        ": loadIntermediateResults(): error writting data #" + QString("").setNum(i) + ": " +
        (s.status()==QDataStream::ReadPastEnd?"read past end of the file":"read corrupt data"));
      return false;
    };
  };
  //
  return s.status()==QDataStream::Ok;
};
/*=======================================================================================================
*
*                           FRIENDS:
* 
*======================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//


// i/o:


/*=====================================================================================================*/
//
// constants:
//


/*=====================================================================================================*/
