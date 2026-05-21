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

#ifndef NS_SCR_PRX_4_PARAMETERS_DESCRIPTOR_H
#define NS_SCR_PRX_4_PARAMETERS_DESCRIPTOR_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QtGlobal>
#include <QtCore/QObject>

#if 0 < HAVE_SCRIPTS
#    include <QtScript/QScriptEngine>
#    include <QtScript/QScriptValue>
#endif

#include <SgParameterCfg.h>
#include <SgParametersDescriptor.h>




class QString;

extern const QString            str_WRONG;

extern const QString            str_EstimateNo;
extern const QString            str_EstimateArc;
extern const QString            str_EstimateLocal;
extern const QString            str_EstimatePwl;
extern const QString            str_EstimateStochastic;

extern const QString            str_Clocks;
extern const QString            str_Zenith;
extern const QString            str_AtmGrad;
extern const QString            str_Cable;
extern const QString            str_AxisOffset;
extern const QString            str_StnCoo;
extern const QString            str_StnVel;
extern const QString            str_SrcCoo;
extern const QString            str_SrcSsm;
extern const QString            str_PolusXy;
extern const QString            str_PolusXyR;
extern const QString            str_PolusUt1;
extern const QString            str_PolusUt1R;
extern const QString            str_PolusNut;
extern const QString            str_PolusNutR;
extern const QString            str_Bl_Clk;
extern const QString            str_Bl_Length;
extern const QString            str_Test;



/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/
class NsScrPrx4ParametersDescriptor : public QObject
{
  Q_OBJECT
  Q_ENUMS (ParMode ParIdx)

public:
  enum ParMode
  {
    EstimateNo                = SgParameterCfg::PM_NONE,
//  EstimateGlobal            = SgParameterCfg::PM_GLB,
    EstimateArc               = SgParameterCfg::PM_ARC,
    EstimateLocal             = SgParameterCfg::PM_LOC,
    EstimatePwl               = SgParameterCfg::PM_PWL,
    EstimateStochastic        = SgParameterCfg::PM_STC,
  };
  enum ParIdx
  {
    Clocks                    = SgParametersDescriptor::Idx_CLOCK_0,
    Zenith                    = SgParametersDescriptor::Idx_ZENITH,
    AtmGrad                   = SgParametersDescriptor::Idx_ATMGRAD,
    Cable                     = SgParametersDescriptor::Idx_CABLE,
    AxisOffset                = SgParametersDescriptor::Idx_AXISOFFSET,
    StnCoo                    = SgParametersDescriptor::Idx_STNCOO,
    StnVel                    = SgParametersDescriptor::Idx_STNVEL,
    SrcCoo                    = SgParametersDescriptor::Idx_SRCCOO,
    SrcSsm                    = SgParametersDescriptor::Idx_SRCSSM,
    PolusXy                   = SgParametersDescriptor::Idx_POLUSXY,
    PolusXyR                  = SgParametersDescriptor::Idx_POLUSXYR,
    PolusUt1                  = SgParametersDescriptor::Idx_POLUSUT1,
    PolusUt1R                 = SgParametersDescriptor::Idx_POLUSUT1R,
    PolusNut                  = SgParametersDescriptor::Idx_POLUSNUT,
    PolusNutR                 = SgParametersDescriptor::Idx_POLUSNUTR,
    Bl_Clk                    = SgParametersDescriptor::Idx_BL_CLK,
    Bl_Length                 = SgParametersDescriptor::Idx_BL_LENGTH,
    Test                      = SgParametersDescriptor::Idx_TEST,
  };
  //enum StochasticType    {ST_WHITENOISE=0, ST_MARKOVPROCESS=1, ST_RANDWALK=2};
  

  inline NsScrPrx4ParametersDescriptor(SgParametersDescriptor& descr, QObject *parent=0)
    :  QObject(parent) {parametersDescriptor_=&descr;};

  inline ~NsScrPrx4ParametersDescriptor() {parametersDescriptor_=NULL;};



signals:
  void clocksParModeChanged(SgParameterCfg::PMode);
  void zenithParModeChanged(SgParameterCfg::PMode);


public slots:
  inline void unsetAllParameters()
    {parametersDescriptor_->unsetAllParameters();};
  inline void unsetParameter    (ParIdx idx)
    {parametersDescriptor_->unsetParameter((SgParametersDescriptor::ParIdx)idx);};

  void setMode4Parameter (ParIdx idx, ParMode mode);

  inline double getWhiteNoise(ParIdx idx)
    {return parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).getWhiteNoise();};


  inline void setConvAPriori(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setConvAPriori(d);};
  inline void setPwlAPriori(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setPwlAPriori(d);};
  inline void setStocAPriori(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setStocAPriori(d);};
  inline void setWhiteNoise(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setWhiteNoise(d);};
  inline void setArcStep(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setArcStep(d);};
  inline void setPwlStep(ParIdx idx, double d)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setPwlStep(d);};
  inline void setPwlNumOfPolynomials(ParIdx idx, int n)
    {parametersDescriptor_->
      parameter((SgParametersDescriptor::ParIdx)idx).setPwlNumOfPolynomials(n);};


  inline void setMode4Parameters(ParMode mode, const QList<ParIdx>& list)
    {parametersDescriptor_->setMode4Parameters((SgParameterCfg::PMode)mode,
      (const QList<SgParametersDescriptor::ParIdx>&)list);};
  inline void unsetParameters (const QList<ParIdx>& list)
    {parametersDescriptor_->unsetParameters((const QList<SgParametersDescriptor::ParIdx>&)list);};





private:
  SgParametersDescriptor       *parametersDescriptor_;
};
/*=====================================================================================================*/




#if 0 < HAVE_SCRIPTS

Q_DECLARE_METATYPE(NsScrPrx4ParametersDescriptor::ParIdx);
Q_DECLARE_METATYPE(QList<NsScrPrx4ParametersDescriptor::ParIdx>);
Q_DECLARE_METATYPE(NsScrPrx4ParametersDescriptor::ParMode);

#endif
















/*=====================================================================================================*/
//
// aux functions:
//
#if 0 < HAVE_SCRIPTS

#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ParIdx(QScriptEngine */*eng*/,
  const NsScrPrx4ParametersDescriptor::ParIdx &i)
{
  return
    i==NsScrPrx4ParametersDescriptor::Clocks?str_Clocks:
      (i==NsScrPrx4ParametersDescriptor::Zenith?str_Zenith:
        (i==NsScrPrx4ParametersDescriptor::AtmGrad?str_AtmGrad:
          (i==NsScrPrx4ParametersDescriptor::Cable?str_Cable:
            (i==NsScrPrx4ParametersDescriptor::AxisOffset?str_AxisOffset:
              (i==NsScrPrx4ParametersDescriptor::StnCoo?str_StnCoo:
                (i==NsScrPrx4ParametersDescriptor::StnVel?str_StnVel:
                  (i==NsScrPrx4ParametersDescriptor::SrcCoo?str_SrcCoo:
                    (i==NsScrPrx4ParametersDescriptor::SrcSsm?str_SrcSsm:
                      (i==NsScrPrx4ParametersDescriptor::PolusXy?str_PolusXy:
                        (i==NsScrPrx4ParametersDescriptor::PolusXyR?str_PolusXyR:
                          (i==NsScrPrx4ParametersDescriptor::PolusUt1?str_PolusUt1:
                            (i==NsScrPrx4ParametersDescriptor::PolusUt1R?str_PolusUt1R:
                              (i==NsScrPrx4ParametersDescriptor::PolusNut?str_PolusNut:
    (i==NsScrPrx4ParametersDescriptor::PolusNutR?str_PolusNutR:
      (i==NsScrPrx4ParametersDescriptor::Bl_Clk?str_Bl_Clk:
        (i==NsScrPrx4ParametersDescriptor::Bl_Length?str_Bl_Length:
          (i==NsScrPrx4ParametersDescriptor::Test?str_Test:str_WRONG)))))))))))))))));
};
#else
inline QScriptValue toScriptValue4ParIdx(QScriptEngine *eng,
  const NsScrPrx4ParametersDescriptor::ParIdx &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ParIdx(const QScriptValue &obj, NsScrPrx4ParametersDescriptor::ParIdx &i)
{
  i = (NsScrPrx4ParametersDescriptor::ParIdx) obj.toInt32();
};




#if QT_VERSION >= 0x040800
inline QScriptValue toScriptValue4ParMode(QScriptEngine */*eng*/, 
  const NsScrPrx4ParametersDescriptor::ParMode &m)
{
  return
    m==NsScrPrx4ParametersDescriptor::EstimateNo?str_EstimateNo:
      (m==NsScrPrx4ParametersDescriptor::EstimateArc?str_EstimateArc:
        (m==NsScrPrx4ParametersDescriptor::EstimateLocal?str_EstimateLocal:
          (m==NsScrPrx4ParametersDescriptor::EstimatePwl?str_EstimatePwl:
            (m==NsScrPrx4ParametersDescriptor::EstimateStochastic?str_EstimateStochastic:str_WRONG))));
};
#else
inline QScriptValue toScriptValue4ParMode(QScriptEngine *eng, 
  const NsScrPrx4ParametersDescriptor::ParMode &)
{
  return QScriptValue(eng, str_WRONG);
};
#endif


inline void fromScriptValue4ParMode(const QScriptValue &obj, 
  NsScrPrx4ParametersDescriptor::ParMode &m)
{
  m = (NsScrPrx4ParametersDescriptor::ParMode) obj.toInt32();
};


#endif   // SCRIPT_SUPPORT


/*=====================================================================================================*/
#endif // NS_SCR_PRX_4_PARAMETERS_DESCRIPTOR_H
