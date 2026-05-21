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

#include <iostream>
#include <stdlib.h>

#include <SgVlbiSession.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QMessageBox>
#else
#   include <QtGui/QMessageBox>
#endif


#include <QtCore/QTextStream>
#include <QtCore/QFile>


#include <SgConstants.h>
#include <SgCubicSpline.h>
#include <SgEstimator.h>
#include <SgLogger.h>
#include <SgSingleSessionTaskManager.h>
#include <SgSolutionReporter.h>
#include <SgTaskManager.h>
#include <SgVector.h>
#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>


#define LOCAL_DEBUG




// sorts obs by residuals:
bool normResidSortingOrderLessThan(SgVlbiObservable*, SgVlbiObservable*);



/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
void SgVlbiSession::collectAPriories()
{
  bool                    stnPosRead, stnVelRead, srcPosRead, srcSmRead, axsOfsRead, hiFyEopRead, 
                          stnGrdRead, extErpRead;
  QString                 path2efaStnPos(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriSitesPositionsFileName()));
  QString                 path2efaStnVel(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriSitesVelocitiesFileName()));
  QString                 path2efaSrcPos(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriSourcesPositionsFileName()));
  QString                 path2efaSrcSsm(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriSourceSsmFileName()));
  QString                 path2efaAxsOfs(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriAxisOffsetsFileName()));
  QString                 path2efaHiFyEop(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriHiFyErpFileName()));
  QString                 path2efaStnGrd(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriMeanGradientsFileName()));
  QString                 path2extErp(config_->evaluatePath2(path2APrioriFiles_, 
                                                      config_->getExtAPrioriErpFileName()));

  stnPosRead = stnVelRead = srcPosRead = srcSmRead = axsOfsRead = hiFyEopRead = stnGrdRead = 
    extErpRead = false;
    
  hiFyEopRead = (0<apHiFyEop_.getFileName().size() && apHiFyEop_.isOk());
  // check for others too:
  
  
  
  
  //
  // read external sites position file:
  if (path2efaStnPos != apStationPositions_.getFileName())
  {
    apStationPositions_.readFile(path2efaStnPos);
    stnPosRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
      "::collectAPriories(): load from the file " + apStationPositions_.getFileName() + 
      QString("").sprintf(" %d", apStationPositions_.size()) + " records of a priori sites positions");
  };
  // read external sites velocities file:
  if (path2efaStnVel != apStationVelocities_.getFileName())
  {
    apStationVelocities_.readFile(path2efaStnVel);
    stnVelRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): load from the file " + apStationVelocities_.getFileName() +
      QString("").sprintf(" %d", apStationVelocities_.size()) + " records of a priori sites velocities");
  };
  // read external source positions file:
  if (path2efaSrcPos != apSourcePositions_.getFileName())
  {
    apSourcePositions_.readFile(path2efaSrcPos);
    srcPosRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): load from the file " + apSourcePositions_.getFileName() +
      QString("").sprintf(" %d", apSourcePositions_.size()) + " records of a priori sources positions");
  };
  // read external source ssm file:
  if (path2efaSrcSsm != apSourceStrModel_.getFileName())
  {
    apSourceStrModel_.readFile(path2efaSrcSsm);
    srcSmRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): load from the file " + apSourceStrModel_.getFileName() +
      QString("").sprintf(" %d", apSourceStrModel_.size()) + " records of source structure model");
  };
  // read external axis offsets file:
  if (path2efaAxsOfs != apAxisOffsets_.getFileName())
  {
    apAxisOffsets_.readFile(path2efaAxsOfs);
    axsOfsRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): load from the file " + apAxisOffsets_.getFileName() +
      QString("").sprintf(" %d", apAxisOffsets_.size()) + " records of a priori axis offsets");
  };
  // read external high frequency ERP file:
  if (path2efaHiFyEop != apHiFyEop_.getFileName())
  {
    apHiFyEop_.readFile(path2efaHiFyEop);
    hiFyEopRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): load from the file " + apHiFyEop_.getFileName() + " model of hi frequency EOP");
  };
  // read external mean sites gradients file:
  if (path2efaStnGrd != apStationGradients_.getFileName())
  {
    apStationGradients_.readFile(path2efaStnGrd);
    stnGrdRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
      "::collectAPriories(): load from the file " + apStationGradients_.getFileName() + 
      QString("").sprintf(" %d", apStationGradients_.size()) + 
      " records of a priori mean sites gradients");
  };
  // read external ERP file:
  if ((hasUt1Interpolation_ || hasPxyInterpolation_) &&
      path2extErp != externalErpFile_.getFileName())
  {
    externalErpFile_.readFile(path2extErp, tMean_, numOfPts4ErpInterpolation_);
    extErpRead = true;
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
      "::collectAPriories(): read " + QString("").setNum(numOfPts4ErpInterpolation_) + 
      " records from the external a priori ERP file " + externalErpFile_.getFileName() + 
      "; mean epoch: " + tMean_.toString());
  };
  //
  // Trp data:
  externalTrpFile_.readFile(path2TrpFiles_ + "/" + QString("").setNum(tStart_.calcYear()) + "/" +                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
                              name_ + ".trp");
  if (externalTrpFile_.isOk())
  {
    SgExternalTrp::TrpRecord   *rec=NULL;
    //
    for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
    {
      SgVlbiStationInfo        *si=it.value();
      QMap<QString, SgExternalTrp::TrpRecord*>
                                trpByScan;
      const QList<SgExternalTrp::TrpRecord*>        
                               *trps=NULL;
      int                       num;
      //
      num = 0;
      trps = externalTrpFile_.getTrpRecords(si->getKey());
      if (trps && trps->size())
      {
        trpByScan.clear();
        for (int i=0; i<trps->size(); i++)
        {
          rec = trps->at(i);
          trpByScan.insert(rec->t().toString(SgMJD::F_INTERNAL) + "@" + rec->srcName(), rec);
        };
        //
        for (QMap<QString, SgVlbiAuxObservation*>::iterator jt=si->auxObservationByScanId()->begin(); 
          jt!=si->auxObservationByScanId()->end(); ++jt)
        {
          QString               scanId=jt.key();
          SgVlbiAuxObservation *aux=jt.value();
          SgTroposphereModelData 
                               *tmd;
          //
          if (trpByScan.contains(scanId))
          {
            rec = trpByScan.value(scanId);
            if (aux && rec)
            {
              tmd = aux->tmdContribExternal();
              //
              tmd->setVal0_delay(rec->dSlant0Delay());
              tmd->setVal1_delay(rec->dSlant1Delay());
              tmd->setMap0_delay(rec->dMap0Delay());
              tmd->setMap1_delay(rec->dMap1Delay());
              tmd->setVal0_rate (rec->dSlant0Rate());
              tmd->setVal1_rate (rec->dSlant1Rate());
              tmd->setMap0_rate (rec->dMap0Rate());
              tmd->setMap1_rate (rec->dMap1Rate());
              tmd->setGrdN_delay(rec->dGradNDelay());
              tmd->setGrdE_delay(rec->dGradEDelay());
              tmd->setGrdN_rate (rec->dGradNRate());
              tmd->setGrdE_rate (rec->dGradERate());
              rec->setHasMate(true);
              num++;
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
                "::collectAPriories(): station \"" + si->getKey() + "\": scan \"" + scanId + 
                "\" aux or rec is NULL");
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
              "::collectAPriories(): station \"" + si->getKey() + "\": cannot find the scan \"" + scanId + 
              "\" in the trp records of the TRP file \"" + externalTrpFile_.getInputFileName() + 
              "\"");
        };
        //
        trpByScan.clear();
        // check for unmatched records:
        for (int i=0; i<trps->size(); i++)
        {
          rec = trps->at(i);
          if (!rec->hasMate())
            logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
              "::collectAPriories(): station \"" + si->getKey() + "\": the trp record for the epoch " + 
              rec->t().toString(SgMJD::F_SOLVE_SPLFL_V2) + " and source \"" + rec->srcName() +
              "\" of the TRP file \"" + externalTrpFile_.getInputFileName() + 
              "\" has no mate in the database");
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
          "::collectAPriories(): cannot find station \"" + si->getKey() + 
          "\" in the TRP file \"" + externalTrpFile_.getInputFileName() + "\"");
      logger->write(SgLogger::INF, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
        "::collectAPriories(): station \"" + si->getKey() + "\": absorbed " + QString("").setNum(num) +
        " trp records of the TRP file \"" + externalTrpFile_.getInputFileName() + "\"");
    };
  }
  else if (config_->getRefractionModel() == SgTaskConfig::RM_CONTRIB_EXTERNAL)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
      "::collectAPriories(): error reading the TRP file \"" + externalTrpFile_.getInputFileName() + 
      "\", refraction model is rolling back to fly by NMF");
    config_->setRefractionModel(SgTaskConfig::RM_FLYBY_NMF);
  };



  //  
  // end of importing Trp data
  
  // End of reading external files.
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::collectAPriories(): files with external a priori were read", true);
  
  // Update info:
  //
  // first, check stations:
  SgAPrioriRec         *apRec=NULL;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo    *si=it.value();
    // process coordinates and velocities:
    if (apStationPositions_.size() && apStationVelocities_.size() && (stnPosRead || stnVelRead))
    {
      apRec = apStationPositions_.lookupApRecord(si->getKey(), tStart_);
      if (apRec)
      {
        si->addAttr(SgVlbiStationInfo::Attr_HAS_A_PRIORI_POS);
        si->setR_ea(Sg3dVector(apRec->at(0).getDvalue("0"), apRec->at(0).getDvalue("1"), 
                                apRec->at(0).getDvalue("2")));
        logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): set up external a priori coords for station " + 
          si->getKey() + "; the diff is " + 
          QString("").sprintf("%.1f mm", (si->getR() - si->getR_ea()).module()*1.0e3) +
          "; tSince: " + apRec->getTsince().toString());
      }
      else
      {
        si->delAttr(SgVlbiStationInfo::Attr_HAS_A_PRIORI_POS);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): cannot find station " + si->getKey() + 
          " in the list of a priori coordinates");
        si->setR_ea(si->getR());
      };
      apRec = apStationVelocities_.lookupApRecord(si->getKey());
      if (apRec)
        si->setV_ea(Sg3dVector(apRec->at(0).getDvalue("0"), apRec->at(0).getDvalue("1"), 
          apRec->at(0).getDvalue("2")));
      else
      {
        si->delAttr(SgVlbiStationInfo::Attr_HAS_A_PRIORI_POS);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): cannot find station " + si->getKey() + 
          " in the list of a priori velocities");
        si->setV_ea(v3Zero);
      };
    };
    // process external axis offsets:
    if (apAxisOffsets_.size() && axsOfsRead)
    {
      if ((apRec=apAxisOffsets_.lookupApRecord(si->getKey())))
      {
        si->addAttr(SgVlbiStationInfo::Attr_HAS_A_PRIORI_AXO);
        // set up a posteriori value:
        si->setAxisOffset_ea(apRec->at(0).getDvalue("1")); // from estimation
        logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): set an external a priori axis offset for station " + 
          si->getKey() + "; the diff is " + 
          QString("").sprintf("%.1f mm", (si->getAxisOffset() - si->getAxisOffset_ea())*1.0e3));
        // check the value from the database and the "a priori" column (==0):
        if (fabs(si->getAxisOffset() - apRec->at(0).getDvalue("0")) > 1.0e-4)
          logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() + 
            "::collectAPriories(): a priori axis offset for station " + si->getKey() + 
            QString("").sprintf(" has different value in the database (%.1f mm) and "
            "the external file (%.1f mm)", si->getAxisOffset()*1.0e3, apRec->at(0).getDvalue("0")*1.0e3));
      }
      else
      {
        si->delAttr(SgVlbiStationInfo::Attr_HAS_A_PRIORI_AXO);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): cannot find station " + si->getKey() +
          " in the list of a priori axis offsets");
        si->setAxisOffset_ea(si->getAxisOffset());
      };
    };
    // process external mean sites gradients:
    if (apStationGradients_.size() && stnGrdRead)
    {
      if ((apRec=apStationGradients_.lookupApRecord(si->getKey())))
      {
        // set up a priori values:
        si->setGradNorth(apRec->at(0).getDvalue("0"));
        si->setGradEast (apRec->at(0).getDvalue("1"));
        logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): a priori gradients " + QString("").sprintf("(%6.2f and %6.1f mm)", 
            si->getGradNorth()*1000.0, si->getGradEast()*1000.0) +
          " have been set up for station " + si->getKey());
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): cannot find station " + si->getKey() + 
          " in the list of a priori mean site gradients");
    };
    // end of station loop
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::collectAPriories(): the stations were done", true);
  //
  // then sources:
  if ( (apSourcePositions_.size() && srcPosRead) ||
//     (apSourceStrModel_.size() && srcSmRead)    
       srcSmRead )
  {
    for (SourcesByName_it it=sourcesByName_.begin(); it!=sourcesByName_.end(); ++it)
    {
      SgVlbiSourceInfo    *si=it.value();
      // positions:
      if ((apRec=apSourcePositions_.lookupApRecord(si->getKey())))
      {
        si->addAttr(SgVlbiSourceInfo::Attr_HAS_A_PRIORI_POS);
        si->setRA_ea(apRec->at(0).getDvalue("0"));
        si->setDN_ea(apRec->at(0).getDvalue("1"));
        si->setAprioriComments(apRec->getComments());
        si->setAl2ExtA(SgVlbiSourceInfo::arcLength(si->getRA(), si->getDN(), 
          si->getRA_ea(), si->getDN_ea()));
        logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): set up external a priori coords for source " + si->getKey() + 
          "; the diff is " + QString("").sprintf("%.3f mas", 
            si->getAl2ExtA()*RAD2MAS));
      }
      else
      {
        si->delAttr(SgVlbiSourceInfo::Attr_HAS_A_PRIORI_POS);
        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): cannot find source " + si->getKey() + 
          " in the list of a priori coordinates");
        si->delAttr(SgVlbiSourceInfo::Attr_HAS_A_PRIORI_POS);
        si->setRA_ea(si->getRA());
        si->setDN_ea(si->getDN());
        si->setAprioriComments("");
        si->setAl2ExtA(0.0);
      };
      // SSM:
      // clear the existing model:
      if (si->sModel().size())
        si->clearSrcStructPoints();
      if ((apRec=apSourceStrModel_.lookupApRecord(si->getKey(), tStart_)))
      {
        for (int i=0; i<apRec->size(); i++)
        {
          double                x, y, k, b;
          bool                  er, ek, eb;
          x = y = k = b = 0.0;
          er = ek = eb = false;
          x = apRec->at(i).getDvalue("MP_X");
          y = apRec->at(i).getDvalue("MP_Y");
          k = apRec->at(i).getDvalue("MP_K");
          b = apRec->at(i).getDvalue("MP_B");
          er= apRec->at(i).getBvalue("MP_ER");
          ek= apRec->at(i).getBvalue("MP_EK");
          eb= apRec->at(i).getBvalue("MP_EB");
/*
          if (fabs(x) < 1.0e-6 && fabs(y) < 1.0e-6)
            logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
              "::collectAPriories(): cannot add SSM for the source \"" + si->getKey() + 
              "\": x and y are zeros");
          else if (!(0.0 < k && k < 1.0))
            logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
              "::collectAPriories(): cannot add SSM for the source \"" + si->getKey() + 
              "\": k is out of range");
          else
*/
          // check for usable values; a user can turn off a model if k==0:
          if ((1.0e-6 < fabs(x) || 1.0e-6 < fabs(y)) && 0.0 < k && k < 1.0)
            si->addSrcStructPoint(k, b, x/RAD2MAS, y/RAD2MAS, ek, eb, er);
         };
        logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
          "::collectAPriories(): found " + QString("").setNum(apRec->size()) + 
          "-component multipoint SS model for the source \"" + si->getKey() + "\"");
      }
      else
      {
//        logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
//          "::collectAPriories(): cannot find source " + si->getKey() + 
//          " in the list of SSM");
      };
    };
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::collectAPriories(): the sources were done", true);


  // ERP:
  //  if (extErpRead && externalErpFile_.isOk())
  if (args4Ut1Interpolation_ && args4PxyInterpolation_ && 
      tabs4Ut1Interpolation_ && tabs4PxyInterpolation_)
  {
    prepare4ErpInterpolation();
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::collectAPriories(): ERP were prepared for interpolation", true);
  }
  else
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::collectAPriories(): cannot make ERP interpolation: data are not provided", true);

  // and individual observations:
  QString                       str;
  double                        dT, t;
  dT = (getLeapSeconds() + 32.184)/DAY2SEC;
  for (int iObs=0; iObs<observations_.size(); iObs++)
  {
    SgVlbiObservation          *obs=observations_.at(iObs);
    double                      dUt, dPx, dPy, dCx, dCy;
    double                      ut0i, px0i, py0i;
    double                      ut0e, px0e, py0e;
    double                      cx0e, cy0e;
    double                      r;
    dUt = dPx = dPy = dCx = dCy = 0.0;
    t  = obs->toDouble() + dT;
    //
    // external HiFrequency ERP model:
    if (hiFyEopRead)
    {
      apHiFyEop_.calcCorrections(*obs+dT, dUt, dPx, dPy);
      dUt /= 1.0e6*DAY2SEC;
      dPx /= 1.0e3*RAD2MAS;
      dPy /= 1.0e3*RAD2MAS;
      /*
      obs->setAprioriUt1HfContrib(dUt);
      obs->setAprioriPxHfContrib(dPx);
      obs->setAprioriPyHfContrib(dPy);
      */
      obs->setExtDelayHiFyPxy((obs->getDdel_dPx()*dPx + obs->getDdel_dPy()*dPy));
      obs->setExtDelayHiFyUt1( obs->getDdel_dUT1()*dUt );
      obs->setExtRateHiFyPxy ((obs->getDrat_dPx()*dPx + obs->getDrat_dPy()*dPy));
      obs->setExtRateHiFyUt1 ( obs->getDrat_dUT1()*dUt );
      // make it SOLVE-compatible:
      obs->setAprioriUt1HfContrib(0.0);
      obs->setAprioriPxHfContrib(0.0);
      obs->setAprioriPyHfContrib(0.0);
    }
    else
    {
std::cout << "    -- no HF calculated\n";
      /*
      // need to add a priori HF from database too:
      // ... later ...
      obs->setAprioriUt1HfContrib(dUt);
      obs->setAprioriPxHfContrib(dPx);
      obs->setAprioriPyHfContrib(dPy);
      */
      obs->setExtDelayHiFyPxy(0.0);
      obs->setExtDelayHiFyUt1(0.0);
      obs->setExtRateHiFyPxy (0.0);
      obs->setExtRateHiFyUt1 (0.0);
    };
    //
    obs->setAprioriUt1HfContrib(dUt);
    obs->setAprioriPxHfContrib(dPx);
    obs->setAprioriPyHfContrib(dPy);
    //
    // external ERP a priori:
    if (isAble2InterpolateErp_)
    {
      ut0e = externalErpInterpolator_->spline(t, SgExternalEopFile::UT1_IDX, r)/DAY2SEC;
      px0e = externalErpInterpolator_->spline(t, SgExternalEopFile::PMX_IDX, r)/RAD2MAS;
      py0e = externalErpInterpolator_->spline(t, SgExternalEopFile::PMY_IDX, r)/RAD2MAS;
      cx0e = externalErpInterpolator_->spline(t, SgExternalEopFile::CIX_IDX, r)/RAD2MAS;
      cy0e = externalErpInterpolator_->spline(t, SgExternalEopFile::CIY_IDX, r)/RAD2MAS;
      
      ut0i = innerUt1Interpolator_->spline(t, 0, r)/DAY2SEC;
      px0i = innerPxyInterpolator_->spline(t, 0, r)/RAD2MAS;
      py0i = innerPxyInterpolator_->spline(t, 1, r)/RAD2MAS;
      //
      dUt = ut0e - ut0i;
      dPx = px0e - px0i;
      dPy = py0e - py0i;
      dCx = cx0e;
      dCy = cy0e;
      //      
      if (config_->getUseExtAPrioriErp())
      {
        obs->setExtDelayErp(obs->getDdel_dUT1()*dUt +
          obs->getDdel_dPx()  *dPx + obs->getDdel_dPy()  *dPy + 
          obs->getDdel_dCipX()*dCx + obs->getDdel_dCipY()*dCy);
        obs->setExtRateErp (obs->getDrat_dUT1()*dUt +
          obs->getDrat_dPx()  *dPx + obs->getDrat_dPy()  *dPy +
          obs->getDrat_dCipX()*dCx + obs->getDrat_dCipY()*dCy);
        obs->setAprioriUt1LfContrib(ut0e);
        obs->setAprioriPxLfContrib(px0e);
        obs->setAprioriPyLfContrib(py0e);
        obs->setAprioriCxLfContrib(cx0e);
        obs->setAprioriCyLfContrib(cy0e);
      }
      else
      {
        obs->setExtDelayErp(0.0);
        obs->setExtRateErp (0.0);
        obs->setAprioriUt1LfContrib(ut0i);
        obs->setAprioriPxLfContrib(px0i);
        obs->setAprioriPyLfContrib(py0i);
      };
    }
    else
    {
      obs->setExtDelayErp(0.0);
      obs->setExtRateErp (0.0);
      if (innerUt1Interpolator_ && innerUt1Interpolator_->isOk())
      {
        ut0i = innerUt1Interpolator_->spline(t, 0, r)/DAY2SEC;
        obs->setAprioriUt1LfContrib(ut0i);
      };
      if (innerPxyInterpolator_ && innerPxyInterpolator_->isOk())
      {
        px0i = innerPxyInterpolator_->spline(t, 0, r)/RAD2MAS;
        py0i = innerPxyInterpolator_->spline(t, 1, r)/RAD2MAS;
        obs->setAprioriPxLfContrib(px0i);
        obs->setAprioriPyLfContrib(py0i);
      };
    };
  };
  if (hiFyEopRead)
    logger->write(SgLogger::DBG, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
      "::collectAPriories(): the observations have been updated with external model "
      "of high frequency EOP variations");
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::collectAPriories(): the observations were done", true);
};



//
void SgVlbiSession::getAprioriErp(const SgMJD& epoch, 
  double& vUt1, double& rUt1,
  double& vPx, double& rPx, double& vPy, double& rPy,
  double& vCx, double& rCx, double& vCy, double& rCy)
{
  vUt1 = rUt1 = vPx = rPx = vPy = rPy = vCx = rCx = vCy = rCy = 0.0;
  double                        dT, t;
  dT = (getLeapSeconds() + 32.184)/DAY2SEC;
  t  = epoch.toDouble() + dT;

  if (config_->getUseExtAPrioriErp() && isAble2InterpolateErp_)
  {
    vUt1= externalErpInterpolator_->spline(t, SgExternalEopFile::UT1_IDX, rUt1);
    vPx = externalErpInterpolator_->spline(t, SgExternalEopFile::PMX_IDX, rPx );
    vPy = externalErpInterpolator_->spline(t, SgExternalEopFile::PMY_IDX, rPy );
    vCx = externalErpInterpolator_->spline(t, SgExternalEopFile::CIX_IDX, rCx );
    vCy = externalErpInterpolator_->spline(t, SgExternalEopFile::CIY_IDX, rCy );
  }
  else
  {
    vUt1= innerUt1Interpolator_->spline(t, 0, rUt1);
    vPx = innerPxyInterpolator_->spline(t, 0, rPx );
    vPy = innerPxyInterpolator_->spline(t, 1, rPy );
  };
  vUt1/= DAY2SEC;
  rUt1/= DAY2SEC;
  vPx /= RAD2MAS;
  rPx /= RAD2MAS;
  vPy /= RAD2MAS;
  rPy /= RAD2MAS;
  vCx /= RAD2MAS;
  rCx /= RAD2MAS;
  vCy /= RAD2MAS;
  rCy /= RAD2MAS;
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): the ERP apriori for the epoch " + (epoch + dT).toString() + " are:");
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): ut1: " + 
    QString("").sprintf("%12.6fms %12.6fms/d", vUt1*1.0e3*DAY2SEC, rUt1*1.0e3*DAY2SEC)); 
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): p_x: " + 
    QString("").sprintf("%12.6fmas %12.6fmas/d", vPx*RAD2MAS, rPx*RAD2MAS));
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): p_y: " + 
    QString("").sprintf("%12.6fmas %12.6fmas/d", vPy*RAD2MAS, rPy*RAD2MAS));
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): CIP_x: " + 
    QString("").sprintf("%12.6fmas %12.6fmas/d", vCx*RAD2MAS, rCx*RAD2MAS));
  logger->write(SgLogger::DBG, SgLogger::FLY_BY | SgLogger::REPORT, className() +
    "::getAprioriErp(): CIP_y: " + 
    QString("").sprintf("%12.6fmas %12.6fmas/d", vCy*RAD2MAS, rCy*RAD2MAS));
};



//
void SgVlbiSession::lookupExternalWeights()
{
  QString   path2file(config_->evaluatePath2(path2APrioriFiles_, config_->getExtWeightsFileName()));
  if (path2file != externalWeights_.getFileName())
    externalWeights_.readFile(path2file);
};



//
void SgVlbiSession::prepare4ErpInterpolation()
{

  if (externalErpInterpolator_)
    delete externalErpInterpolator_;
  if (innerUt1Interpolator_)
    delete innerUt1Interpolator_;
  if (innerPxyInterpolator_)
    delete innerPxyInterpolator_;
  externalErpInterpolator_ = NULL;
  innerUt1Interpolator_ = NULL;
  innerPxyInterpolator_ = NULL;


  if (externalErpFile_.isOk())
    externalErpInterpolator_ = new SgCubicSpline(externalErpFile_.argument()->n(), 5);
  innerUt1Interpolator_ = new SgCubicSpline(args4Ut1Interpolation_->n(), 1);
  innerPxyInterpolator_ = new SgCubicSpline(args4PxyInterpolation_->n(), 2);

  // feed the interpolators:
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::prepare4ErpInterpolation(): the spliners were created", true);
  int                           n;
  //
  // UT1:
  n = args4Ut1Interpolation_->n();
  for (int i=0; i<n; i++)
  {
    innerUt1Interpolator_->argument().setElement(i, args4Ut1Interpolation_->getElement(i));
    innerUt1Interpolator_->table().setElement   (i, 0, tabs4Ut1Interpolation_->getElement(i, 0));
  };
  // Polar motion:
  n = args4PxyInterpolation_->n();
  for (int i=0; i<n; i++)
  {
    innerPxyInterpolator_->argument().setElement(i, args4PxyInterpolation_->getElement(i));
    innerPxyInterpolator_->table().setElement   (i, 0, tabs4PxyInterpolation_->getElement(i, 0));
    innerPxyInterpolator_->table().setElement   (i, 1, tabs4PxyInterpolation_->getElement(i, 1));
  };
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::prepare4ErpInterpolation(): the inner interpolator has been fed", true);

  if (externalErpFile_.isOk())
  {
    double                      utOffset = 0.0;
    if (externalErpFile_.inputEopType() == SgExternalEopFile::IET_FINALS ||
        externalErpFile_.inputEopType() == SgExternalEopFile::IET_C04)
    {
      utOffset = getLeapSeconds();
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::prepare4ErpInterpolation(): UT1-UTC will be adjusted by " + QString("").setNum(utOffset) + 
        " secs to get TAI", true);
    };
    // External table, UT1 and PM are in one file:
    n = externalErpFile_.argument()->n();
    for (int i=0; i<n; i++)
    {
      externalErpInterpolator_->argument().setElement(i,
        externalErpFile_.argument()->getElement(i));
      externalErpInterpolator_->table().setElement(i,
        SgExternalEopFile::UT1_IDX,
        externalErpFile_.eopTable()->getElement(i, SgExternalEopFile::UT1_IDX) - utOffset);
      externalErpInterpolator_->table().setElement(i,
        SgExternalEopFile::PMX_IDX,
        externalErpFile_.eopTable()->getElement(i, SgExternalEopFile::PMX_IDX));
      externalErpInterpolator_->table().setElement(i,
        SgExternalEopFile::PMY_IDX,
        externalErpFile_.eopTable()->getElement(i, SgExternalEopFile::PMY_IDX));
      if (externalErpFile_.inputEopType() == SgExternalEopFile::IET_FINALS ||
          externalErpFile_.inputEopType() == SgExternalEopFile::IET_C04)
      {
        externalErpInterpolator_->table().setElement(i,
          SgExternalEopFile::CIX_IDX,
          externalErpFile_.eopTable()->getElement(i, SgExternalEopFile::CIX_IDX));
        externalErpInterpolator_->table().setElement(i,
          SgExternalEopFile::CIY_IDX,
          externalErpFile_.eopTable()->getElement(i, SgExternalEopFile::CIY_IDX));
      };
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::prepare4ErpInterpolation(): the outer interpolator has been fed", true);
    //
    if (externalErpFile_.ut1Type() != tabsUt1Type_)
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT | SgLogger::FLY_BY, className() +
        "::prepare4ErpInterpolation(): database and external file UT1 types mismatch!"
        " Trying to adjust, however, it was not tested.");
      
      // mimic SOLVE's flybymap.f behavior:
      SgTidalUt1      innerUt1(tabsUt1Type_, calcInfo_.getDversion());
      SgTidalUt1      outerUt1(externalErpFile_.ut1Type(), calcInfo_.getDversion()); //??
    
      for (unsigned int i=0; i<innerUt1Interpolator_->argument().n(); i++)
        innerUt1Interpolator_->table()(i, 0) -= 
          innerUt1.calc(SgMJD(innerUt1Interpolator_->argument().getElement(i)));
      for (unsigned int i=0; i<externalErpInterpolator_->argument().n(); i++)
        externalErpInterpolator_->table()(i, SgExternalEopFile::UT1_IDX) -= 
          outerUt1.calc(SgMJD(externalErpInterpolator_->argument().getElement(i)));
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::prepare4ErpInterpolation(): the UT1type has been adjusted", true);
    };
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::prepare4ErpInterpolation(): the UT1type was checked", true);
  };
  //
  innerUt1Interpolator_->prepare4Spline();
  innerPxyInterpolator_->prepare4Spline();
  if (externalErpFile_.isOk())
    externalErpInterpolator_->prepare4Spline();
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::prepare4ErpInterpolation(): the spliners were prepared", true);
  
  isAble2InterpolateErp_ = externalErpFile_.isOk() && innerUt1Interpolator_->isOk() && 
    innerPxyInterpolator_->isOk() && externalErpInterpolator_->isOk();

  /*
  for (int i=0; i<innerUt1Interpolator_->numOfRecords(); i++)
  {
    QString str;
    str.sprintf("%2d   %14.6f   %14.6f", 
      i,  innerUt1Interpolator_->argument().getElement(i), 
          innerUt1Interpolator_->table().getElement(i, 0));
    std::cout << qPrintable(str) << "\n";
  };
  std::cout << "PM table of interpolation from the database:\n";
  for (int i=0; i<innerPxyInterpolator_->numOfRecords(); i++)
  {
    QString str;
    str.sprintf("%2d   %14.6f   %14.6f %14.6f", 
      i,  innerPxyInterpolator_->argument().getElement(i), 
          innerPxyInterpolator_->table().getElement(i, 0),
          innerPxyInterpolator_->table().getElement(i, 1));
    std::cout << qPrintable(str) << "\n";
  };
  std::cout << "ERP table of interpolation from the external file:\n";
  for (int i=0; i<externalErpInterpolator_->numOfRecords(); i++)
  {
    QString str;
    str.sprintf("%2d   %14.6f   %14.6f %14.6f %14.6f", 
      i,  externalErpInterpolator_->argument().getElement(i), 
          externalErpInterpolator_->table().getElement(i, SgExternalErpFile::UT1_IDX),
          externalErpInterpolator_->table().getElement(i, SgExternalErpFile::PMX_IDX),
          externalErpInterpolator_->table().getElement(i, SgExternalErpFile::PMY_IDX));
    std::cout << qPrintable(str) << "\n";
  };
  */
};



//
int SgVlbiSession::suppressNotSoGoodObs()
{
  int                         numOfCorrected=0;
  // mimic SOLVE:
  if (!(config_->getUseGoodQualityCodeAtStartup() /* && primaryBand_->getInputFileVersion()<4 */))
    return numOfCorrected;

  QString                     primeBandKey=primaryBand_->getKey();
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation        *obs=observations_.at(i);
    for (QMap<QString, SgVlbiObservable*>::iterator it=obs->observableByKey().begin();
      it!=obs->observableByKey().end(); ++it)
    {
      SgVlbiObservable           *o=it.value();
      //
      // turn off any obs with QC<config_->getGoodQualityCodeAtStartup()
      if (o->getQualityFactor() < config_->getGoodQualityCodeAtStartup())
      {
        o->propagateAttr(SgVlbiMeasurement::Attr_NOT_VALID);
        numOfCorrected++;
      };
    };
  };
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::suppressNotSoGoodObs(): " + QString("").setNum(numOfCorrected) + 
    " observation" + (numOfCorrected==1?" was":"s were") + 
    " deselected due to not sufficiently high Quality Code, " + 
    QString("").setNum(config_->getGoodQualityCodeAtStartup()), true);
  //
  return numOfCorrected;
};



//
void SgVlbiSession::doPostReadActions_old()
{
  if (!(config_ && parametersDescriptor_))
    return;

  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::doPostReadActions_old(): post read actions initiated for the session " + getName(), true);

  // save pointers:
  SgTaskConfig               *config_saved = config_;
  SgParametersDescriptor     *parametersDescriptor_saved = parametersDescriptor_;

  config_ = new SgTaskConfig;
  *config_ = *config_saved;
  parametersDescriptor_ = new SgParametersDescriptor;
  *parametersDescriptor_ = *parametersDescriptor_saved;

  suppressNotSoGoodObs();
  //
  //
  config_->setUseDelayType(SgTaskConfig::VD_SB_DELAY);
  //
  if (config_->getDoIonoCorrection4SBD() && bands().size()>1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions_old(): evaluating ionospheric corrections for single band delays", true);
    calculateIonoCorrections(NULL);
  };
  //
  if (config_->getDoClockBreakDetectionMode1())
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions_old(): running clock break detections, early stage", true);
    detectAndProcessClockBreaks();
  };
  //
  if (config_->getDoOutliersEliminationMode1())
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::doPostReadActions_old(): elimination of outliers, early stage", true);
    eliminateOutliersMode1();
  };
  //
  // restore pointers:
  delete config_;
  delete parametersDescriptor_;
  config_ = config_saved;
  parametersDescriptor_ = parametersDescriptor_saved;
};



//
void SgVlbiSession::process(bool haveProcessAllBands, bool interactWithGui)
{
  if (!(config_ && parametersDescriptor_))
  {
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::process(): called in nonappropriate form: config of parametersDescriptor is NULL, ignoring");
    return;
  };
  
  bool                          hasArc, hasStc;
  hasArc = hasStc = false;
  for (int i=0; i<parametersDescriptor_->num(); i++)
  {
    if (parametersDescriptor_->getMode(i) == SgParameterCfg::PM_ARC ||
        parametersDescriptor_->getMode(i) == SgParameterCfg::PM_PWL  )
      hasArc = true;
    if (parametersDescriptor_->getMode(i) == SgParameterCfg::PM_STC)
      hasStc = true;
  };
  if (hasArc && hasStc)
  {
    QMessageBox::warning(NULL, "Cannot estimate", "The software is unable to estimate a combination of "
      "arc or piece-wise parameters and stochastic parameters. Latter releases will do it. Sorry for "
      "unconvenience");
    logger->write(SgLogger::ERR, SgLogger::RUN, className() +
      "::process(): cannot estimate arc or pwl AND stochastic parameters in one solution");
    return;
  };
  
  
  SgTask                    *task = new SgTask("Internal run manager for the session " + getName());
  QList<SgObservation*>     *obsList = NULL;

  task->setConfig(*config_);
  task->addSession((SgVlbiSessionInfo*)this, this);
  task->setParameters(*parametersDescriptor_);

  //
  QString                 path2efa;
  bool                    have2reloadEfas(false);
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriSitesPositionsFileName());
  if (path2efa != apStationPositions_.getFileName() && config_->getUseExtAPrioriSitesPositions())
    have2reloadEfas = true;
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriSitesVelocitiesFileName());
  if (path2efa != apStationVelocities_.getFileName() && config_->getUseExtAPrioriSitesVelocities())
    have2reloadEfas = true;

  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriSourcesPositionsFileName());
  if (path2efa != apSourcePositions_.getFileName() && config_->getUseExtAPrioriSourcesPositions())
    have2reloadEfas = true;
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriSourceSsmFileName());
  if (path2efa != apSourceStrModel_.getFileName() && config_->getUseExtAPrioriSourceSsm())
    have2reloadEfas = true;   
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriAxisOffsetsFileName());
  if (path2efa != apAxisOffsets_.getFileName() && config_->getUseExtAPrioriAxisOffsets())
    have2reloadEfas = true;
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriHiFyErpFileName());
  if (path2efa != apHiFyEop_.getFileName() && config_->getUseExtAPrioriHiFyErp())
    have2reloadEfas = true;
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriMeanGradientsFileName());
  if (path2efa != apStationGradients_.getFileName() && config_->getUseExtAPrioriMeanGradients())
    have2reloadEfas = true;
  path2efa = config_->evaluatePath2(path2APrioriFiles_, config_->getExtAPrioriErpFileName());
  if (path2efa != externalErpFile_.getFileName() && config_->getUseExtAPrioriErp())
    have2reloadEfas = true;
  //
//  if (have2reloadEfas || lastProcessedConfig_.getIsSolveCompatible() != config_->getIsSolveCompatible())
//  if (lastProcessedConfig_ != *config_)
  if (have2reloadEfas || 
      lastProcessedConfig_.getIsSolveCompatible() != config_->getIsSolveCompatible() ||
      lastProcessedConfig_.getUseExtAPrioriHiFyErp() != config_->getUseExtAPrioriHiFyErp() ||
      lastProcessedConfig_.getHave2ApplyUt1OceanTideHFContrib() != 
        config_->getHave2ApplyUt1OceanTideHFContrib() ||
      lastProcessedConfig_.getHave2ApplyPxyOceanTideHFContrib() != 
        config_->getHave2ApplyPxyOceanTideHFContrib() ||
      lastProcessedConfig_.getUseExtAPrioriErp() != config_->getUseExtAPrioriErp() || 
      lastProcessedConfig_.getExtAPrioriErpFileName() != config_->getExtAPrioriErpFileName()
     )
  {
    collectAPriories();
    if (lastProcessedConfig_.getIsSolveCompatible() != config_->getIsSolveCompatible())
    {
      // recalc geodetic coordinates:
      for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
        it.value()->recalcRLF(config_->getIsSolveCompatible());
      // do something else:
      
    };
  };
  //
  // check for a file with external weights, if necessary -- read it and update weights.
  if (config_->getUseExternalWeights())
  {
    lookupExternalWeights();
    if (externalWeights_.isOk())
    {
      zerofySigma2add();
      externalWeights_.setupExternalWeights(baselinesByName_);
    }
    else
    {
      // zerofySigma2add();
      logger->write(SgLogger::WRN, SgLogger::RUN | SgLogger::IO_TXT, className() +
        "::process(): reading an external weights file failed, no weights were applied");
    };
  };
  
  SgTaskManager                *mgr=new SgSingleSessionTaskManager(task);

  obsList = (QList<SgObservation*>*) &observations_;
  QString                       bandKey=bands_.at(config_->getActiveBandIdx())->getKey();
  logger->write(SgLogger::DBG, SgLogger::RUN, className() +
    "::process(): starting analysis of the " + bandKey + "-band");

  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    obs->setupActiveObservable(bandKey);
    obs->setupActiveMeasurement(config_);
  };

  // set up the manager:
  mgr->setObservations(obsList);
  mgr->currentSessionInfo() = (SgVlbiSessionInfo*)this;
  mgr->currentSession() = this;

  // GUI:
  mgr->setLongOperationStart(longOperationStart_);
  mgr->setLongOperationProgress(longOperationProgress_);
  mgr->setLongOperationStop(longOperationStop_);
  mgr->setLongOperationMessage(longOperationMessage_);
  mgr->setLongOperationShowStats(longOperationShowStats_);
  mgr->setHave2InteractWithGui(interactWithGui);
  // mgr->setHave2InteractWithGui(false);

  // run it:
  mgr->prepare4Run();
  mgr->run(haveProcessAllBands);
  if (reporter_)
    reporter_->absorbInfo(mgr);
  mgr->finisRun();

  delete task;
  task = NULL;
  delete mgr;
  mgr = NULL;
  // save the config:
  lastProcessedConfig_ = *config_;
};



//
void SgVlbiSession::checkExcludedStations()
{
  for (StationsByName_it it_s=stationsByName_.begin(); it_s!=stationsByName_.end(); ++it_s)
  {
    SgVlbiStationInfo          *si=it_s.value();
    //
    bool                        isOk=false;
    for (BaselinesByName_it it_b=baselinesByName_.begin(); it_b!=baselinesByName_.end(); ++it_b)
    {
      QString                   st1Name, st2Name;
      SgVlbiBaselineInfo       *bi=it_b.value();
      st1Name = bi->getKey().left(8);
      st2Name = bi->getKey().right(8);
      if ((si->getKey() == st1Name || si->getKey() == st2Name) && 
          !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
        isOk = true;
    };
    if (!isOk)
    {
      si->addAttr(SgVlbiStationInfo::Attr_NOT_VALID);
      logger->write(SgLogger::DBG, SgLogger::RUN, className() +
        "::checkExcludedStations(): all baselines of the station " + si->getKey() + 
        " are deselected, the station is turned off too");
    };
  };
};



//
void SgVlbiSession::restoreIonCorrections()
{


  
};


//
//
//



//
//
//
//
// ------------------------ clock break related procedures:
//
//
void SgVlbiSession::calculateClockBreakParameter(const SgMJD& tBreak, SgVlbiStationInfo* stn,
  SgVlbiBand* band, double& cbShift, double& cbSigma)
{
  // collect the observations for the station:
  QList<SgVlbiBaselineInfo*>    baselines;
  BaselinesByName_it            baselineItr=band->baselinesByName().begin();
  for (int iBaseline=0; baselineItr!=band->baselinesByName().end(); ++baselineItr, iBaseline++)
  {
    SgVlbiBaselineInfo* baselineInfo=baselineItr.value();
    if ( baselineInfo &&
        !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID) &&
         baselineInfo->getKey().contains(stn->getKey()) )
      {
        // determine how many points on both sides of the break:
        int                       numOnLeft=0, numOnRight=0;
        QList<SgVlbiObservable*> *observables=&baselineInfo->observables();
        for (int idx=0; idx<observables->size(); idx++)
        {
          if (observables->at(idx)->epoch()<=tBreak)
            numOnLeft++;
          else
            numOnRight++;
        };
        if (numOnLeft>2 && numOnRight>2)
          baselines.append(baselineInfo);
      };
  };
  if (!baselines.size()) // complain and return
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
      "::calculateClockBreakParameter(): cannot evaluate clock break parameters: num of baselines==0");
    return;
  }
  else 
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::calculateClockBreakParameter(): collected " + QString("").setNum(baselines.size()) +
      " baselines for the station " + stn->getKey());
  
  int                           nZ=0;
  for (int iBaseline=0; iBaseline<baselines.size(); iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    QList<SgVlbiObservable*>   *observables=&baselineInfo->observables();
    for (int idx=0; idx<observables->size(); idx++)
      nZ++;
  };

  SgEstimator                  *estimator=new SgEstimator(config_);
  QList<SgParameter*>          *localParameters=new QList<SgParameter*>;
  // create parameters list:
  SgParameter                  *pC, *pAi, *pBi;
  pC = new SgParameter("Shift");
  pC->setSigmaAPriori(1.0e+8);
  localParameters->append(pC);
  for (int iBaseline=0; iBaseline<baselines.size(); iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    pAi = new SgParameter("A_" + QString("").sprintf("%03d_(", iBaseline) +
      baselineInfo->getKey() + ")");
    pBi = new SgParameter("B_" + QString("").sprintf("%03d_(", iBaseline) +
      baselineInfo->getKey() + ")");
    pAi->setSigmaAPriori(1.0e+8);
    pBi->setSigmaAPriori(1.0e+8);
    localParameters->append(pAi);
    localParameters->append(pBi);
  };
  estimator->addParametersList(localParameters);
//  estimator->setListP(stochasticParameters);
  estimator->prepare2Run(nZ + 10, tStart_, tFinis_, tRefer_);
  // feed the estimator:
  double                        res, sig, sign;
  SgVector                      vO_C(1), vSigma(1);
  double                        dsSB=0.1E-9; // 0.1ns
  double                        dsGR=5.E-12; // 5ps
  for (int iBaseline=0; iBaseline<baselines.size(); iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    sign = -1.0;
    if (baselineInfo->getKey().indexOf(stn->getKey())>0) // the second station
      sign = 1.0;
    pAi = localParameters->at(2*iBaseline + 1);
    pBi = localParameters->at(2*iBaseline + 2);
    QList<SgVlbiObservable*>   *observables=&baselineInfo->observables();
    for (int idx=0; idx<observables->size(); idx++)
    {
      SgVlbiObservable         *o=observables->at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
//    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) && m)
      if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        res = m->getResidual();
        sig = m->sigma2Apply() + dsGR;
        if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
          sig += dsSB;
        if (o->epoch()<=tBreak)
          pC->setD(0.0);
        else
          pC->setD(1.0E-9*sign);
        pAi->setD(1.0E-9);
        pBi->setD(1.0E-9*(o->epoch() - tBreak));
        vO_C.setElement  (0, res);
        vSigma.setElement(0, sig);
        estimator->processObs(o->epoch(), vO_C, vSigma);
      };
    };
  };
  estimator->finisRun();
  cbShift = pC->getSolution()*1.0E-9;
  cbSigma = pC->getSigma()*1.0E-9;
  //
  for (int i=0; i<localParameters->size(); i++)
    delete localParameters->at(i);
  delete localParameters;
  localParameters = NULL;
  delete estimator;
};



//
void SgVlbiSession::calculateClockBreaksParameters(QList<SgClockBreakAgent*>& agents, SgVlbiBand* band)
{
  if (!band || !agents.size())
    return;

  // first, pick up baselines with clock breaks:
  QList<SgVlbiBaselineInfo*>    baselines;
  QMap<QString, SgVlbiBaselineInfo*>    
                                baselinesByName;
  BaselinesByName_it            it=band->baselinesByName().begin();
  int                           nZ=0;
  for (int iBaseline=0; it!=band->baselinesByName().end(); ++it, iBaseline++)
  {
    SgVlbiBaselineInfo         *blInfo=it.value();
    if (blInfo && !blInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
    {
      for (int idxAgent=0; idxAgent<agents.size(); idxAgent++)
      {
        SgClockBreakAgent      *agent=agents.at(idxAgent);
        if (agent->stn_ && blInfo->getKey().contains(agent->stn_->getKey()))
        {
          logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
            "::calculateClockBreaksParameters(): gathering info for CB on " + agent->epoch_.toString() +
            " at " + agent->stn_->getKey() + "; baseline " + blInfo->getKey());
          // determine how many points on both sides of the break:
          int                   numOnLeft=0, numOnRight=0;
          QList<SgVlbiObservable*> *observables=&blInfo->observables();
          for (int idx=0; idx<observables->size(); idx++)
          {
            SgVlbiObservable   *o=observables->at(idx);
            if (o && o->activeDelay())
            {
              if (o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED) && 
                  o->epoch() < agent->epoch_)
                numOnLeft++;
              else if (o->activeDelay()->isAttr(SgVlbiMeasurement::Attr_PROCESSED) &&
                  agent->epoch_ < o->epoch())
                numOnRight++;
            };
          };
          if (numOnLeft>2 && numOnRight>2)
          {
            if (!baselinesByName.contains(blInfo->getKey()))
            {
              baselines.append(blInfo);
              baselinesByName.insert(blInfo->getKey(), blInfo);
            };
            nZ += numOnLeft + numOnRight;
          };
        };
      };
    };
  };
  baselinesByName.clear();
  if (!baselines.size()) // complain and return
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::calculateClockBreaksParameters(): cannot evaluate clock break parameters: num of baselines==0");
    return;
  }
  else 
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::calculateClockBreaksParameters(): collect " + QString("").setNum(baselines.size()) +
      " baselines for clock breaks evaluation");
  
  // prepare the estimator:
  SgEstimator                  *estimator=new SgEstimator(config_);
  QList<SgParameter*>          *localParameters=new QList<SgParameter*>;
  // create parameters list:
  for (int idxAgent=0; idxAgent<agents.size(); idxAgent++)
  {
    SgClockBreakAgent          *agent=agents.at(idxAgent);
    agent->pC_ = new SgParameter("Shift at " + agent->stn_->getKey() + 
      QString("").sprintf("_%03d", idxAgent));
    agent->pC_->setSigmaAPriori(1.0e+8);
    localParameters->append(agent->pC_);
  };
  SgParameter                  *pAi;
  QMap< QString, QList<SgParameter*> >
                                parametersByBaseline;
  for (int iBaseline=0; iBaseline<baselines.size(); iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    int                         nOrder=2;
    SgVlbiStationInfo          *si;
    QList<SgParameter*>         parList;
    si = baselineInfo->stn_1(stationsByName_);
    if (si && si->getClocksModelOrder()>nOrder)
      nOrder = si->getClocksModelOrder();
    si = baselineInfo->stn_2(stationsByName_);
    if (si && si->getClocksModelOrder()>nOrder)
      nOrder = si->getClocksModelOrder();
    for (int idxP=0; idxP<nOrder; idxP++)
    {
      pAi = new SgParameter("A_" + QString("").sprintf("%02d_%03d_(", idxP, iBaseline) +
        baselineInfo->getKey() + ")");
      pAi->setSigmaAPriori(1.0e+8);
      localParameters->append(pAi);
      parList.append(pAi);
    };
    parametersByBaseline.insert(baselineInfo->getKey(), parList);
  };
  // set up the estimator:
  estimator->addParametersList(localParameters);
  estimator->prepare2Run(nZ + 10, tStart_, tFinis_, tRefer_);
  // feed the estimator:
  double                res, sig, sign;
  SgVector              vO_C(1), vSigma(1);
  for (int iBaseline=0; iBaseline<baselines.size(); iBaseline++)
  {
    SgVlbiBaselineInfo         *blInfo=baselines.at(iBaseline);
    const QList<SgParameter*>  &parList=parametersByBaseline.value(blInfo->getKey());
    QList<SgVlbiObservable*>   *observables=&blInfo->observables();
    for (int idx=0; idx<observables->size(); idx++)
    {
      SgVlbiObservable         *o=observables->at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
//    if (o->owner()->isAttr(SgVlbiObservation::Attr_PROCESSED) && m)
      if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED) )
      {
        res = m->getResidual();
        sig = m->sigma2Apply();
        // clock breaks parameters:
        for (int idxAgent=0; idxAgent<agents.size(); idxAgent++)
        {
          SgClockBreakAgent    *agent=agents.at(idxAgent);
          SgParameter          *pC=agent->pC_;
          sign = -1.0;
          if (blInfo->getKey().indexOf(agent->stn_->getKey()) > 0) // the second station
            sign = 1.0;
          if (o->epoch() <= agent->epoch_)
            pC->setD(0.0);
          else
            pC->setD(1.0E-9*sign);
        };
        // baselines:
        double                  d(1.0), dt(o->epoch() - tRefer_);
        for (int idxP=0; idxP<parList.size(); idxP++)
        {
          pAi = parList.at(idxP);
          pAi->setD(1.0E-9*d);
          d *= dt;
        };
        vO_C.setElement  (0, res);
        vSigma.setElement(0, sig);
        estimator->processObs(o->epoch(), vO_C, vSigma);
      };
    };
  };
  // make solution:
  estimator->finisRun();
  // pick up the solved values:
  for (int idxAgent=0; idxAgent<agents.size(); idxAgent++)
  {
    SgClockBreakAgent      *agent=agents.at(idxAgent);
    SgParameter            *pC=agent->pC_;
    agent->shift_ = pC->getSolution()*1.0E-9;
    agent->sigma_ = pC->getSigma()*1.0E-9;
  };
  //
  for (int i=0; i<localParameters->size(); i++)
    delete localParameters->at(i);
  delete localParameters;
  localParameters = NULL;
  delete estimator;
};



//
bool SgVlbiSession::detectClockBreaks_mode1(SgVlbiBand* band, QString& stnName, SgMJD& tBreak, 
  QMultiMap<QString, SgMJD>& candidatesByBln)
{

/*
std::cout << "  **-1 "
<< " stnName=[" << qPrintable(stnName) << "] "
<< " tBreak: " << qPrintable(tBreak.toString()) 
<< "\n";
*/


  BaselinesByName               candidates;
  tBreak = tZero;
  // now, we are working with SBD only:
  BaselinesByName_it            itBaselineInfo=band->baselinesByName().begin();
  for (int iBaseline=0; itBaselineInfo!=band->baselinesByName().end(); ++itBaselineInfo, iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=itBaselineInfo.value();
    if (baselineInfo && !baselineInfo->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
    {
      baselineInfo->calculateClockF1(config_);
      baselineInfo->evaluateCBIndicator();
      SgClockBreakIndicator    *cbi=baselineInfo->cbIndicator();
      if (cbi &&
          ( (0.25 < (cbi->totWrms_ - cbi->minWrms_)/cbi->totWrms_ && 
              0 < cbi->numOnLeft_ && 
              0 < cbi->numOnRight_)
            ||
            (cbi->minWrms_ < cbi->totWrms_ && 
              10 < cbi->numOnLeft_ && 
              10 < cbi->numOnRight_)
          )
        )
      {
        //
        if (candidatesByBln.contains(baselineInfo->getKey(), cbi->epoch_))
        {
        }
        else
        {
          candidates.insert(baselineInfo->getKey(), baselineInfo);
          /*
          std::cout << qPrintable(baselineInfo->getKey()) << ": " << qPrintable(band->getKey()) << "-band ++ value "
                    << 100.0*(cbi->totWrms_ - cbi->minWrms_)/cbi->totWrms_ << "% at "
                     << qPrintable(cbi->epoch_.toString())
                    << "; num on the left wing " << cbi->numOnLeft_ 
                    << "; num on the right wing " << cbi->numOnRight_ 
                    << " cbi->totWrms=" << cbi->totWrms_
                    << " cbi->minWrms=" << cbi->minWrms_
                    << "\n";
          */
        };
      }
      else if (cbi &&
          cbi->numOnLeft_  > 0 &&
          cbi->numOnRight_ > 0)
      {
        /*
        std::cout << qPrintable(baselineInfo->getKey()) << qPrintable(band->getKey()) << "-band -- value "
                  << 100.0*(cbi->totWrms_ - cbi->minWrms_)/cbi->totWrms_ << "% at "
                  << qPrintable(cbi->epoch_.toString())
                  << "; num on the left wing " << cbi->numOnLeft_ 
                  << "; num on the right wing " << cbi->numOnRight_ 
                  << " cbi->totWrms=" << cbi->totWrms_
                  << " cbi->minWrms=" << cbi->minWrms_
                  << "\n";
       */
      };

    };
  };
  if (!candidates.size())
    return false;
  // get maximum (absolute):
  double                        maxConsumed=0.0;
  SgVlbiBaselineInfo           *blBreak=NULL;
  QString                       blName, stnName_1, stnName_2;
  BaselinesByName_it            iBli=candidates.begin();
  for (; iBli!=candidates.end(); ++iBli)
    if (maxConsumed < iBli.value()->cbIndicator()->totWrms_ - iBli.value()->cbIndicator()->minWrms_)
    {
      maxConsumed = iBli.value()->cbIndicator()->totWrms_ - iBli.value()->cbIndicator()->minWrms_;
      blName = iBli.key();
      tBreak = iBli.value()->cbIndicator()->epoch_;
    };
  if (band->baselinesByName().contains(blName))
    blBreak = band->baselinesByName().value(blName);
  else
  {
    // complain:
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::detectClockBreaks_mode1(): baseline name " + blName +
      " is not in band's baselinesByName_ container");
    return false;
  };
  //
  candidatesByBln.insert(blName, tBreak);
  //  
  SgVlbiObservable *obs=blBreak->observables().at(0);
  stnName_1 = obs->stn_1()->getKey();
  stnName_2 = obs->stn_2()->getKey();
  /*
  std::cout << "Determined: " << qPrintable(blBreak->getKey()) << " with components "
            << qPrintable(stnName_1) << " and " << qPrintable(stnName_2)
            << " from " << candidates.size() << " candidates\n";
  */
  //
  // ok, now what we've got: blBreak   == baseline info with a clock break
  //                         blName    == it's name
  //                         stnName_1 == name of the first station and 
  //                         stnName_2 == name of the second station at the baseline.
  //
  // evaluate frequencies:
  QMap<QString, int>            numByName;
  iBli = candidates.begin();
  for (; iBli!=candidates.end(); ++iBli)
    if (iBli.key().contains(stnName_1) || iBli.key().contains(stnName_2))
    {
      SgVlbiObservable         *obs=iBli.value()->observables().at(0);
      SgVlbiStationInfo        *s=NULL;
      
      if ( (s=lookupStationByIdx(obs->owner()->getStation1Idx())) &&
           !s->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) )
        numByName[obs->stn_1()->getKey()]++;
      if ( (s=lookupStationByIdx(obs->owner()->getStation2Idx())) &&
           !s->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) )
        numByName[obs->stn_2()->getKey()]++;
    };
  // get maximum:
  QMap<QString, int>::iterator  iAux=numByName.begin();
  int                           minN=iAux.value(), maxN=iAux.value();
  stnName = iAux.key();
  for (; iAux!=numByName.end(); ++iAux)
  {
    int                         n=iAux.value();
    if (minN>n)
      minN = n;
    if (maxN<n)
    {
      maxN = n;
      stnName = iAux.key();
    };
    // std::cout << "    ... " << qPrintable(iAux.key()) << ": " << iAux.value() << " ... \n";
  };
  // determine proper station and epoch:
  if (maxN!=minN || (baselinesByName_.size()==1 && maxN==1 && minN==1))
  {
    if (stationsByName().contains(stnName))
    {
      // stnBreak = stationsByName().value(stnName);
      // std::cout << "The break station is: " << qPrintable(stnName) << "\n";
      // adjust break epoch:
      iBli = candidates.begin();
      for (; iBli!=candidates.end(); ++iBli)
        if (iBli.key().contains(stnName) &&
            iBli.value()->cbIndicator()->tOnLeft_ <= tBreak && // ensure that it is the same break
            tBreak <= iBli.value()->cbIndicator()->tOnRight_)
        {
          if (tBreak < iBli.value()->cbIndicator()->epoch_)
          {
            tBreak = iBli.value()->cbIndicator()->epoch_;
            // std::cout << " processing " << qPrintable(iBli.key()) << ", epoch: "
            //          << qPrintable(tBreak.toString()) << "\n";
          };
        };
        // std::cout << "The break epoch is: " << qPrintable(tBreak.toString()) << "\n";
      return true;
    }
    else
      logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
        "::detectClockBreaks_mode1(): stnName " + stnName + " is not in stationsByName() container");
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::detectClockBreaks_mode1(): impossible to guess the station name: maxN==minN==" +
      QString("").setNum(maxN));
  return false;
};



//
int SgVlbiSession::checkBandForClockBreaks_part1(int bandIdx, bool have2AdjustResiduals)
{
  const int                     clockBreaksLimit  = 10;
  int                           numOfClockBreaks  =  0;
  const double                  shiftThreshold    =  0.4;  // in ns
  const double                  sigmaThreshold    = 10;
  
  // check:
  if (bandIdx < 0 || bands_.size()-1 < bandIdx)
    return numOfClockBreaks;
  SgVlbiBand                   *band=bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::checkBandForClockBreaks_part1(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return numOfClockBreaks;
  };
  // proceed:
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::checkBandForClockBreaks_part1(): processing observations from " + band->getKey() + "-band", true);
  QString                       stnName("");
  QMultiMap<QString, SgMJD>     candidatesByBln;
  SgMJD                         tBreak(tZero);
  int                           idx=0, bandIdxSaved;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  while (detectClockBreaks_mode1(band, stnName, tBreak, candidatesByBln) && idx<clockBreaksLimit)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::checkBandForClockBreaks_part1(): detected clock break #" + QString("").setNum(idx) + " at " +
      stnName + " on " + tBreak.toString(), true);
    idx++;
    // process the clock break:
    // determine parameters of the break:
    SgVlbiStationInfo          *stnBreak=stationsByName().value(stnName);
    double                      shift, sigma;
    calculateClockBreakParameter(tBreak, stnBreak, band, shift, sigma);
    if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
      shift = round(1.0E9*shift);            // round to ns
    else if ( config_->getUseDelayType()==SgTaskConfig::VD_GRP_DELAY ||
              config_->getUseDelayType()==SgTaskConfig::VD_PHS_DELAY   )
      shift = round(1.0E12*shift)*1.0E-3;    // round to ps
    sigma *= 1.0E9;
    //
    if (fabs(shift) > sigmaThreshold*sigma &&   // 10*sigma threshold (otherwise, do it by hands),
        shiftThreshold < fabs(shift)            // 0.4ns or bigger (otherwise, do it by hands):
      )
    {
      //
      // ok, let user know
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::checkBandForClockBreaks_part1(): detected clock break at " + stnBreak->getKey() +
        " station on " + tBreak.toString() + "; shift on " + 
        QString("").sprintf("%.4f ns", shift), true);
      // adjust station information concerning the clock break:
      correctClockBreak(band, stnBreak, tBreak, shift, sigma, have2AdjustResiduals);
      numOfClockBreaks++;
    };
    // recalculate residuals:
    process(true, false);
  };
  config_->setActiveBandIdx(bandIdxSaved);
  candidatesByBln.clear();
  return numOfClockBreaks;
};



//
void SgVlbiSession::detectAndProcessClockBreaks()
{
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::detectAndProcessClockBreaks(): clock break detection procedure initiated", true);
  // evaluate residuals:
  process(true, false);
  // run through all bands:
  for (int iBand=0; iBand<numberOfBands(); iBand++)
    checkBandForClockBreaks_part1(iBand, false);
};



//
int SgVlbiSession::checkBandForClockBreaks_part2(int bandIdx, bool have2AdjustResiduals)
{
  const int                     clockBreaksLimit  = 10;
  int                           numOfClockBreaks  =  0;
  const double                  shiftThreshold    =  0.4;  // in ns
  const double                  sigmaThreshold    = 10;
  
  // check:
  if (bandIdx < 0 || bands_.size()-1 < bandIdx)
    return numOfClockBreaks;
  SgVlbiBand                   *band=bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::checkBandForClockBreaks_part2(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return numOfClockBreaks;
  };
  //
  // proceed:
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::checkBandForClockBreaks_part2(): processing observations from " + band->getKey() + "-band", true);
  QString                       stnName("");
  QString                       baselineName("");
  QMultiMap<QString, SgMJD>     candidatesByBln;
  SgMJD                         tBreak(tZero);
  int                           idx=0, bandIdxSaved;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  while (detectClockBreaks_mode2(band, stnName, tBreak, candidatesByBln) && idx<clockBreaksLimit)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::checkBandForClockBreaks_part2(): detected clock break #" + QString("").setNum(idx) + " at " +
      stnName + " on " + tBreak.toString(), true);
    idx++;
    // process the clock break:
    // determine parameters of the break:
    SgVlbiStationInfo          *stnBreak=stationsByName().value(stnName);
    double                      shift, sigma;
    calculateClockBreakParameter(tBreak, stnBreak, band, shift, sigma);
    if (config_->getUseDelayType() == SgTaskConfig::VD_SB_DELAY)
      shift = round(1.0E9*shift);               // round to ns
    else if ( config_->getUseDelayType()==SgTaskConfig::VD_GRP_DELAY ||
              config_->getUseDelayType()==SgTaskConfig::VD_PHS_DELAY   )
      shift = round(1.0E12*shift)*1.0E-3;       // round to ps
    sigma *= 1.0E9;
    if (fabs(shift) > sigmaThreshold*sigma &&   // 10*sigma threshold (otherwise, do it by hands),
        shiftThreshold < fabs(shift)            // 0.4ns or bigger (otherwise, do it by hands):
      )
    {
      //
      // ok, let user know
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::checkBandForClockBreaks_part2(): detected clock break at " + stnBreak->getKey() +
        " station on " + tBreak.toString() + "; shift on " + 
        QString("").sprintf("%.4f ns", shift), true);
      // adjust station information concerning the clock break:
      correctClockBreak(band, stnBreak, tBreak, shift, sigma, have2AdjustResiduals);
      numOfClockBreaks++;
    };
    // recalculate residuals:
    process(true, false);
  };
  config_->setActiveBandIdx(bandIdxSaved);
  candidatesByBln.clear();
  return numOfClockBreaks;
};



//
bool SgVlbiSession::detectClockBreaks_mode2(SgVlbiBand* band, QString& stnName, SgMJD& tBreak, 
  QMultiMap<QString, SgMJD>& candidatesByBln)
{
  BaselinesByName               candidates;
  QMap<QString, SgClockBreakIndicator*>
                                cbiByKey;
  SgClockBreakIndicator        *cbiTarget;
  cbiTarget = NULL;
  //
  BaselinesByName_it            itBaselineInfo=band->baselinesByName().begin();
  for (int iBaseline=0; itBaselineInfo!=band->baselinesByName().end(); ++itBaselineInfo, iBaseline++)
  {
    SgVlbiBaselineInfo         *bi=itBaselineInfo.value();
    if (bi && !bi->isAttr(SgVlbiBaselineInfo::Attr_NOT_VALID))
    {
      bi->calculateClockF2(config_);
      bi->evaluateCBIndicator2();
      SgClockBreakIndicator    *cbi=bi->cbIndicator();
      //
      if (cbi && 1 < cbi->numOnLeft_ && 1 < cbi->numOnRight_)
      {
        //
        if (candidatesByBln.contains(bi->getKey(), cbi->epoch_))
        {
        }
        else
        {
          candidates.insert(bi->getKey(), bi);
          cbiByKey.insert(bi->getKey(), cbi);
        };
      };
      //
    };
  };
  if (!candidates.size())
    return false;

  // get the maximum (absolute):
  double                        max=0.0;
  SgVlbiBaselineInfo           *blBreak=NULL;
  QString                       blName, stnName_1, stnName_2;
  int                           options=0;
  //
  itBaselineInfo = candidates.begin();
  for (; itBaselineInfo!=candidates.end(); ++itBaselineInfo)
    if (max < itBaselineInfo.value()->cbIndicator()->extremum_)
    {
      max = itBaselineInfo.value()->cbIndicator()->extremum_;
      blName = itBaselineInfo.key();
    };
  if (band->baselinesByName().contains(blName))
  {
    blBreak = band->baselinesByName().value(blName);
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::detectClockBreaks_mode2(): get baseline \"" + blName +
      "\" as a candidate for clock break detection");
  }
  else
  {
    // complain:
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::detectClockBreaks_mode2(): baseline name " + blName +
      " is not in band's baselinesByName_ container");
    return false;
  };
  // done.
  //
  cbiTarget = blBreak->cbIndicator();
  tBreak    = cbiTarget->epoch_;
  options   = cbiTarget->options_;
  candidatesByBln.insert(blName, tBreak);
  
  
//  baselineName = blName;
  //

/*
std::cout << "   Candidate : " << qPrintable(blName) 
<< "  tBreak: " << qPrintable(tBreak.toString())
<< "  nOnLeft= " << cbiTarget->numOnLeft_
<< "  nOnRight= " << cbiTarget->numOnRight_
<< " Interval: [" << qPrintable(cbiTarget->tOnLeft_.toString()) 
<< " -- " << qPrintable(cbiTarget->tOnRight_.toString()) << "]"
<< "\n";
*/

  //
  SgVlbiObservable             *o=blBreak->observables().at(0);
  stnName_1 = o->stn_1()->getKey();
  stnName_2 = o->stn_2()->getKey();
  /*
  std::cout << "Determined: " << qPrintable(blBreak->getKey()) << " with components "
            << qPrintable(stnName_1) << " and " << qPrintable(stnName_2)
            << " from " << candidates.size() << " candidates\n";
  */
  //
  // ok, now what we've got: blBreak   == baseline info with a clock break
  //                         blName    == it's name
  //                         stnName_1 == name of the first station and 
  //                         stnName_2 == name of the second station at the baseline.
  //
  // evaluate frequencies:
  QMap<QString, int>            numByName;
  itBaselineInfo = candidates.begin();
  for (; itBaselineInfo!=candidates.end(); ++itBaselineInfo)
    if (itBaselineInfo.key().contains(stnName_1) || itBaselineInfo.key().contains(stnName_2))
    {
      SgVlbiBaselineInfo       *bi=itBaselineInfo.value();
      SgClockBreakIndicator    *cbi;
      o = bi->observables().at(0);
      //
//std::cout << "   -- Bln : " << qPrintable(bi->getKey()) << "\n";
        SgVlbiStationInfo      *s=NULL;
        if ((s=lookupStationByIdx(o->owner()->getStation1Idx()))        &&
            !s->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS)        &&
            // ensure that it is the same break
            (cbi=bi->lookupClockF2(config_, cbiTarget))                 &&
            (
              (s->getKey() == stnName_1 && options == cbi->options_) ||
              (s->getKey() == stnName_2 && options != cbi->options_)
            )                                                           &&
            0.1*cbiTarget->extremum_ < cbi->extremum_                    )
        {
          numByName[o->stn_1()->getKey()]++;
/*
std::cout << "   -- Stn1: " << qPrintable(o->stn_1()->getKey()) 
<< ": " << qPrintable(interval2Str(cbiTarget->epoch_ - cbi->epoch_))
<< "  target/test: " << cbiTarget->extremum_*1.0e12 << "/" << cbi->extremum_*1.0e12
<< "\n";
*/
          if (tBreak < cbi->epoch_)
          {
/*
std::cout << "   -- Stn2: " << qPrintable(o->stn_2()->getKey()) 
<< " adjustment: " << qPrintable(tBreak.toString())
<< "  => " 
<< qPrintable(cbi->epoch_.toString())
<< "\n";
*/
            tBreak = cbi->epoch_;
          };
        };
        if ((s=lookupStationByIdx(o->owner()->getStation2Idx()))        &&
            !s->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS)        &&
            // ensure that it is the same break
            (cbi=bi->lookupClockF2(config_, cbiTarget))                 &&
            (
              (s->getKey() == stnName_2 && options == cbi->options_) ||
              (s->getKey() == stnName_1 && options != cbi->options_)
            )                                                           &&
            0.4*cbiTarget->extremum_ < cbi->extremum_                    )
        {
          numByName[o->stn_2()->getKey()]++;
/*
std::cout << "   -- Stn2: " << qPrintable(o->stn_2()->getKey()) 
<< ": " << qPrintable(interval2Str(cbiTarget->epoch_ - cbi->epoch_))
<< "  target/test: " << cbiTarget->extremum_*1.0e12 << "/" << cbi->extremum_*1.0e12
<< "\n";
*/
          if (tBreak < cbi->epoch_)
          {
/*
std::cout << "   -- Stn2: " << qPrintable(o->stn_2()->getKey()) 
<< " adjustment: " << qPrintable(tBreak.toString())
<< "  => " 
<< qPrintable(cbi->epoch_.toString())
<< "\n";
*/
            tBreak = cbi->epoch_;
          };
        };
    };
  //
  // get maximum:
  QMap<QString, int>::iterator  iAux=numByName.begin();
  int                           minN, maxN;
  //
  minN = maxN = iAux.value();
  stnName = iAux.key();
  for (; iAux!=numByName.end(); ++iAux)
  {
    int                         n=iAux.value();
    if (n < minN)
      minN = n;
    if (maxN < n)
    {
      maxN = n;
      stnName = iAux.key();
    };
  };
  // determine proper station and epoch:
  if (maxN!=minN || (baselinesByName_.size()==1 && maxN==1 && minN==1))
  {
    if (stationsByName().contains(stnName))
    {
      // adjust the epoch:
/*
      int                       idx=0;
      for (int i=0; i<blBreak->observables().size(); i++)
        if (blBreak->observables().at(i)->epoch() == tBreak)
          idx = i;
      if (0 < idx)
      {
        tBreak = blBreak->observables().at(idx - 1)->epoch();
      };
*/

/*
std::cout << "   ++ Break: " << qPrintable(stnName) 
<< " at " << qPrintable(tBreak.toString())
<< "\n";
*/

      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::detectClockBreaks_mode2(): get a candidate for clock break event: station \"" +
        stnName + "\" epoch " + tBreak.toString());
      return true;
    }
    else
      logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
        "::detectClockBreaks_mode2(): stnName " + stnName + " is not in stationsByName() container");
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::detectClockBreaks_mode2(): impossible to guess the station name: maxN==minN==" +
      QString("").setNum(maxN));
  return false;
};





//
void SgVlbiSession::correctClockBreak(SgVlbiBand* band, SgVlbiStationInfo* stnInfo,
    const SgMJD& tBreak, double shift, double sigma, bool have2AdjustResiduals, bool have2Complain)
{
  //
  // check for clock reference station, SOLVE will not tolerate it:
  if (have2Complain &&
    stationsByName().value(stnInfo->getKey())->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS) &&
    QMessageBox::warning(NULL, "A clock break is at a reference clock station",
      "A clock break has occured at clocks of a reference clock station.\n"
      "It is suggested to assign a reference clock station attribute to another station "
      "and reprocess data.\n"
      "Continue anyway?",
      QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)
  {
    return;
  };
  //
  //
  SgVlbiStationInfo               *stn2Correct=NULL;
  if (config_->getUseDynamicClockBreaks() || config_->getUseDelayType()==SgTaskConfig::VD_SB_DELAY)
    stn2Correct = stnInfo;
  else
    stn2Correct = band->stationsByName().value(stnInfo->getKey());
 
  //check for existing clock breaks:
  SgParameterBreak                *clockBreak=NULL;
  if (stn2Correct->clockBreaks().size())
    for (int i=0; i<stn2Correct->clockBreaks().size(); i++)
      if (fabs(*(stn2Correct->clockBreaks().at(i)) - tBreak) < 5.5E-6) // 0.4752 sec
        clockBreak = stn2Correct->clockBreaks().at(i);
  if (clockBreak) // update existing clock break:
  {
    clockBreak->setA0(config_->getUseDynamicClockBreaks()? 0.0 : (clockBreak->getA0()+shift));
    clockBreak->setS0(sigma);
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::processClockBreakMarks(): the clock break at " + stn2Correct->getKey() +
      " station on " + tBreak.toString() + "; has been updated to " +
      QString("").sprintf("%.4f ns", clockBreak->getA0()));
  }
  else  // create a new clock break:
  {
    clockBreak = new SgParameterBreak(tBreak + 5.0E-6, config_->getUseDynamicClockBreaks()?0.0:shift);
    clockBreak->setS0(sigma);
    if (stn2Correct->clockBreaks().addBreak(clockBreak))
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
        "::processClockBreakMarks(): the new break has been appended to the model");
    else
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::processClockBreakMarks(): adding new clock break at " + stn2Correct->getKey() +
        " station on " + tBreak.toString() + " failed");
  };
  //
  //
  if (config_->getUseDynamicClockBreaks())
    clockBreak->addAttr(SgParameterBreak::Attr_DYNAMIC);
  //
  //
  // at last, update residuals for all bands and redraw plots:
  if (have2AdjustResiduals)
    for (int iBand=0; iBand<numberOfBands(); iBand++)
    {
      SgVlbiBand*     band=bands_.at(iBand);
      for (int iObs=0; iObs<band->observables().size(); iObs++)
      {
        SgVlbiObservable       *o=band->observables().at(iObs);
        if (o->epoch()>tBreak+5.0E-6 && o->owner()->baseline()->getKey().contains(stn2Correct->getKey()))
        {
          if (o->owner()->baseline()->getKey().indexOf(stn2Correct->getKey()) == 0)
          {
            o->sbDelay().setResidual(o->sbDelay().getResidual() + shift*1.0E-9);
            o->grDelay().setResidual(o->grDelay().getResidual() + shift*1.0E-9);
          }
          else
          {
            o->sbDelay().setResidual(o->sbDelay().getResidual() - shift*1.0E-9);
            o->grDelay().setResidual(o->grDelay().getResidual() - shift*1.0E-9);
          }
        };
      };
    };
};
// -------------------------- end of "clock break" procedures.
//
//
//



//
//
//
// -------------------------- dealing with outliers:
//
//
void SgVlbiSession::eliminateOutliersMode1()
{
  for (int iBand=0; iBand<numberOfBands(); iBand++)
    eliminateOutliersSimpleMode(iBand, 200, 5.0);
};



//
void SgVlbiSession::eliminateOutliersSimpleMode(int bandIdx, int maxNumOfPasses, double threshold, 
  double upperLimit)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return;
  SgVlbiBand       *band = bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::eliminateOutliersSimpleMode(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return;
  };
  int                           numOfEliminated, numOfPasses;
  int                           bandIdxSaved;
  bool                          hasBeenModified;
  bool                          need2check4absValue=upperLimit>0.0;
  double                        sigma;
  QList<SgVlbiObservable*>      elluminated;
  numOfEliminated = 0;
  numOfPasses = 0;
  hasBeenModified = true;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  while (hasBeenModified && (numOfPasses<maxNumOfPasses))
  {
    if (numOfPasses > 0)
      process(true, false);
    sigma = band->wrms(DT_DELAY);
    hasBeenModified = false;
    for (int idx=0; idx<band->observables().size(); idx++)
    {
      SgVlbiObservable         *o=band->observables().at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
      if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        if (fabs(m->getResidual()) > threshold*sigma                   ||
          (need2check4absValue && fabs(m->getResidual()) > upperLimit)  )
        {
          //
          m->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
          //          
          hasBeenModified = true;
          numOfEliminated++;
          elluminated << o;
        };
      };
    };
    numOfPasses++;
  };
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::eliminateOutliersSimpleMode(): eliminated " + QString("").setNum(numOfEliminated) +
    (config_->getUseDelayType()==SgTaskConfig::VD_SB_DELAY?" SbDelay":" GrDelay") +
    " observables from the " + band->getKey() + "-band in " + QString("").setNum(numOfPasses) +
    " passes, the threshold is " + QString("").setNum(threshold));
  if (elluminated.size())
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::eliminateOutliersSimpleMode(): the eliminated observations are:");
    for (int i=0; i<elluminated.size(); i++)
    {
      SgVlbiObservable         *o=elluminated.at(i);
      SgVlbiMeasurement        *m=o->activeDelay();
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +  "::eliminateOutliersSimpleMode():     # " + 
        QString("").setNum(o->getMediaIdx()) + " " + 
        o->epoch().toString(SgMJD::F_Simple) + " " + 
        o->getBaseline()->getKey() + " @ " + o->getSrc()->getKey() + "  residual " +
        QString("").sprintf("%.3f +/- %.3f (ns) | %.3f with QC %d",
          m->getResidual()*1.0e9, 
          m->getSigma()*1.0e9,
          m->getResidualNorm(), o->getQualityFactor()));
    };
    elluminated.clear();
  };
  config_->setActiveBandIdx(bandIdxSaved);
};



//
void SgVlbiSession::eliminateLargeOutliers(int bandIdx, int maxNumOfPasses, double wrmsRatio)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return;
  SgVlbiBand       *band = bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::eliminateLargeOutliers(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return;
  };
  int                           numOfPasses;
  int                           bandIdxSaved;
  bool                          hasBeenModified;
  double                        sigma, prevSigma, residMax;
  QList<SgVlbiObservable*>      elliminated;
  numOfPasses = 0;
  hasBeenModified = true;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  //
  prevSigma = band->wrms(DT_DELAY);
  //
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::eliminateLargeOutliers(): start of the procedure");
  while (hasBeenModified && 
        (numOfPasses < maxNumOfPasses) && 
        (8 < band->numProcessed(DT_DELAY)))
 //     (numOfParameters_ < band->numProcessed(DT_DELAY) + 2))
  {
    SgVlbiObservable           *testedObs=NULL;
    residMax = 0.0;
    hasBeenModified = false;
    // first, find a largest residual:
    for (int idx=0; idx<band->observables().size(); idx++)
    {
      SgVlbiObservable         *o=band->observables().at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
      if (m && m->isAttr(SgVlbiMeasurement::Attr_PROCESSED))
      {
        if (residMax < fabs(m->getResidualNorm()))
        {
          residMax = fabs(m->getResidualNorm());
          testedObs= o;
        };
      };
    };
    if (testedObs)
    {
      logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
        "::eliminateLargeOutliers(): found a candidate: " + testedObs->strId() + " with norm.resid= " +
        QString("").sprintf("%10.4f", residMax));
      // deselect it:
      //
      testedObs->activeDelay()->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      //
      // and make a new solution:
      process(true, false);
      sigma = band->wrms(DT_DELAY);
      // check how it was improved:
      //std::cout << "  ++ wrmsRatio=" << wrmsRatio << "  prevSigma=" << prevSigma
      //<< "  sigma=" << sigma << "   prevSigma/sigma=" << prevSigma/sigma << "\n";
      if (wrmsRatio < prevSigma/sigma)
      {
        hasBeenModified = true;
        elliminated << testedObs;
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::eliminateLargeOutliers(): the obs is removed: the ratio = " + 
          QString("").asprintf("%.2f with the threshold of %.2f", prevSigma/sigma, wrmsRatio));
        prevSigma = sigma;
      }
      else // restore the obs:
      {
        testedObs->owner()->eradicateAttr(SgVlbiMeasurement::Attr_NOT_VALID, config_->getUseDelayType());
        process(true, false);
        logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
          "::eliminateLargeOutliers(): the obs is ok: the ratio = " + 
          QString("").asprintf("%.2f with the threshold of %.2f", prevSigma/sigma, wrmsRatio));
      };
    }
    else
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::eliminateLargeOutliers(): the observable is NULL");
    numOfPasses++;
  };
  //
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::eliminateLargeOutliers(): eliminated " + QString("").setNum(elliminated.size()) +
    (config_->getUseDelayType()==SgTaskConfig::VD_SB_DELAY?" SbDelay":" GrDelay") +
    " observables from the " + band->getKey() + "-band in " + QString("").setNum(numOfPasses) +
    " passes");
  if (elliminated.size())
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
      "::eliminateLargeOutliers(): the eliminated observations are:");
    for (int i=0; i<elliminated.size(); i++)
    {
      SgVlbiObservable         *o=elliminated.at(i);
      SgVlbiMeasurement        *m=o->activeDelay();
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +  
        "::eliminateLargeOutliers():     # " + 
        QString("").setNum(o->getMediaIdx()) + " " + 
        o->epoch().toString(SgMJD::F_Simple) + " " + 
        o->getBaseline()->getKey() + " @ " + o->getSrc()->getKey() + "  residual " +
        QString("").sprintf("%.3f +/- %.3f (ns) | %.3f with QC %d",
          m->getResidual()*1.0e9, 
          m->getSigma()*1.0e9,
          m->getResidualNorm(), o->getQualityFactor()));
    };
    elliminated.clear();
  };
  config_->setActiveBandIdx(bandIdxSaved);
};




//
int SgVlbiSession::eliminateOutliers(int bandIdx)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return 0;
  SgVlbiBand       *band = bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::eliminateOutliers(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return 0;
  };
  double                        threshold;
  int                           numOfEliminated, numOfPasses, maxNumOfPasses;
  int                           bandIdxSaved;
  bool                          hasBeenModified;
  bool                          wasMeddledIntoWc=false;
  QList<SgVlbiObservable*>      elluminated;
  //
  if (config_->getDoWeightCorrection() && config_->getOpHave2SuppressWeightCorrection())
  {
    config_->setDoWeightCorrection(false);
    wasMeddledIntoWc = true;
  };
  //
  maxNumOfPasses = config_->getOpIterationsLimit();
  threshold = config_->getOpThreshold();
  numOfEliminated = 0;
  numOfPasses = 0;
  hasBeenModified = true;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  
  // GUI: 
  if (have2InteractWithGui_ && longOperationStart_)
    (*longOperationStart_)(numOfPasses, maxNumOfPasses, "");
  //
  //
  while (hasBeenModified && (numOfPasses<maxNumOfPasses))
  {
    if (numOfPasses>0)
      process(false, false);
    QList<SgVlbiObservable*>    outlierCandidates;
    for (int idx=0; idx<band->observables().size(); idx++)
    {
      SgVlbiObservable         *o=band->observables().at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
      if (m && 
          m->isAttr(SgVlbiMeasurement::Attr_PROCESSED) && 
         !m->isAttr(SgVlbiMeasurement::Attr_IMMUNE2EXCL) && 
          fabs(m->getResidualNorm()) > threshold)
        outlierCandidates << o;
    };
    // ok, there are outliers
    if (outlierCandidates.size())
    {
      qSort(outlierCandidates.begin(), outlierCandidates.end(), normResidSortingOrderLessThan);
      // determine how much outliers should be removed:
      double                    max=fabs(outlierCandidates.first()->activeDelay()->getResidualNorm());
      if (outlierCandidates.size()>2 && max>2.0*threshold)
      {
#ifdef LOCAL_DEBUG
        std::cout << " MODE: cluster\n";
#endif
        while (fabs(outlierCandidates.last()->activeDelay()->getResidualNorm())<max-0.5*threshold &&
               outlierCandidates.size()>1)
          outlierCandidates.removeLast();
      }
      else // otherwise, eliminate only the largest outlier:
      {
#ifdef LOCAL_DEBUG
        std::cout << " MODE: the largest one\n";
#endif
        SgVlbiObservable       *o=outlierCandidates.at(0);
        outlierCandidates.clear();
        outlierCandidates << o;
      };

#ifdef LOCAL_DEBUG
      std::cout << "Pass # " << numOfPasses << " (" << maxNumOfPasses << ") "
                << "observations to be excluded:\n";
#endif
      for (int idx=0; idx<outlierCandidates.size(); idx++)
      {
        SgVlbiObservable       *o=outlierCandidates.at(idx);
        SgVlbiMeasurement      *m=o->activeDelay();
        //
        m->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
        //
        numOfEliminated++;
        elluminated << o;
#ifdef LOCAL_DEBUG
        std::cout << "obs# " << o->owner()->getMediaIdx() << ":  (" << qPrintable(o->epoch().toString()) 
                  << ")  " << qPrintable(o->baseline()->getKey() + " @" + o->src()->getKey())
                  << "   " << m->getResidualNorm() << "  "
                  << m->getResidual()*1.0e9 << " +/- "  << m->getSigma()*1.0e9 << "\n";
#endif
      };
      numOfPasses++;
      hasBeenModified = true;
      // GUI part:
      if (have2InteractWithGui_)
      {
        if (longOperationMessage_)
          (*longOperationMessage_)("Processing outliers: pass " + 
            QString("").setNum(numOfPasses) + " from " + QString("").setNum(maxNumOfPasses));
        if (longOperationProgress_)
          (*longOperationProgress_)(numOfPasses);
      };
      //
    }
    else
    {
      hasBeenModified = false;
#ifdef LOCAL_DEBUG
      std::cout << "the size is 0\n";
#endif
    };
    outlierCandidates.clear();
  };

#ifdef LOCAL_DEBUG
  if (numOfPasses==maxNumOfPasses)
    std::cout << "reached max passing number (" << numOfPasses << ")\n";
#endif

  //
  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::eliminateOutliers(): eliminated " + QString("").setNum(numOfEliminated) +
    " observables from " + band->getKey() + "-band in " + QString("").setNum(numOfPasses) +
    " passes with the threshold " + QString("").setNum(threshold));
  if (elluminated.size())
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
          "::eliminateOutliers(): the eliminated observations are:");
    for (int i=0; i<elluminated.size(); i++)
    {
      SgVlbiObservable         *o=elluminated.at(i);
      SgVlbiMeasurement        *m=o->activeDelay();
      logger->write(SgLogger::INF, SgLogger::PREPROC, className() +  "::eliminateOutliers():     # " + 
        QString("").setNum(o->getMediaIdx()) + " " + 
        o->epoch().toString(SgMJD::F_Simple) + " " +
        o->getBaseline()->getKey() + " @ " + o->getSrc()->getKey() + "  residual " +
        QString("").sprintf("%.3f +/- %.3f (ns) | %.3f with QC %d",
          m->getResidual()*1.0e9, 
          m->getSigma()*1.0e9,
          m->getResidualNorm(), o->getQualityFactor()));
    };
    elluminated.clear();
  };
  //
  // restore config:
  config_->setActiveBandIdx(bandIdxSaved);
  if (wasMeddledIntoWc)
    config_->setDoWeightCorrection(true);
  hasOutlierEliminationRun_ = true;
  if (have2InteractWithGui_ && longOperationStop_)
    (*longOperationStop_)();
  if (hasOutlierRestoratioRun_)
    addAttr(Attr_FF_OUTLIERS_PROCESSED);
  return numOfEliminated;
};



//
int SgVlbiSession::restoreOutliers(int bandIdx)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return 0;
  SgVlbiBand                   *band=bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::restoreOutliers(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return 0;
  };
  double                        threshold;
  int                           numOfRestored, numOfPasses, maxNumOfPasses;
  int                           bandIdxSaved;
  bool                          hasBeenModified;
  bool                          wasMeddledIntoWc=false;
  if (config_->getDoWeightCorrection() && config_->getOpHave2SuppressWeightCorrection())
  {
    config_->setDoWeightCorrection(false);
    wasMeddledIntoWc = true;
  };
  //
  maxNumOfPasses = config_->getOpIterationsLimit();
  threshold = config_->getOpThreshold();
  numOfRestored = 0;
  numOfPasses = 0;
  hasBeenModified = true;
  bandIdxSaved = config_->getActiveBandIdx();
  config_->setActiveBandIdx(bandIdx);
  while (hasBeenModified && (numOfPasses<maxNumOfPasses))
  {
    if (numOfPasses > 0)
      process(false, false);
    QList<SgVlbiObservable*>    outlierCandidates;
    for (int idx=0; idx<band->observables().size(); idx++)
    {
      SgVlbiObservable         *o=band->observables().at(idx);
      SgVlbiMeasurement        *m=o->activeDelay();
      bool                      isDeselected=m->isAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      if (!isDeselected && m->isAttr(SgVlbiMeasurement::Attr_HAS_IONO_CORR)) // check the opposite bands:
      {
        for (int jj=0; jj<o->owner()->passiveObses().size(); jj++)
          isDeselected = isDeselected || 
            o->owner()->passiveObses().at(jj)->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID);
      };
      //
      if (m &&
         !m->isAttr(SgVlbiMeasurement::Attr_PROCESSED)   &&
         !m->isAttr(SgVlbiMeasurement::Attr_IMMUNE2INCL) &&
         isDeselected &&
          fabs(m->getResidualNorm()) < threshold)
        outlierCandidates << o;
    };
    // ok, there are outliers
    if (outlierCandidates.size())
    {
      qSort(outlierCandidates.begin(), outlierCandidates.end(), normResidSortingOrderLessThan);
      // determine how much outliers should be removed:
      double                    max, min;
      max = fabs(outlierCandidates.first()->activeDelay()->getResidualNorm());
      min = fabs(outlierCandidates.last ()->activeDelay()->getResidualNorm());
      if (outlierCandidates.size()>2 && min<1.0)
      {
#ifdef LOCAL_DEBUG
        std::cout << " MODE: cluster-bottom\n";
#endif
        while (fabs(outlierCandidates.first()->activeDelay()->getResidualNorm())>1.0 &&
               outlierCandidates.size()>1)
          outlierCandidates.removeFirst();
      }
      else if (outlierCandidates.size()>2 && max>1.0) // here the outliers are in the range
                                                      // from sigma to sigma*threshold, so we will
                                                      // include them all -- sigma will not get lower,
                                                      // there is no sense to recalculate the solution
      {
#ifdef LOCAL_DEBUG
        std::cout << " MODE: cluster-top\n";
#endif
      }
      else // otherwise, eliminate only the largest outlier:
      {
#ifdef LOCAL_DEBUG
        std::cout << " MODE: the smallest one\n";
#endif
        SgVlbiObservable       *o=outlierCandidates.last();
        outlierCandidates.clear();
        outlierCandidates << o;
      };
#ifdef LOCAL_DEBUG
      std::cout << "Pass # " << numOfPasses << " (" << maxNumOfPasses << ") "
                << "observation to be restored:\n";
#endif
      for (int idx=0; idx<outlierCandidates.size(); idx++)
      {
        SgVlbiObservable       *o=outlierCandidates.at(idx);
        o->owner()->eradicateAttr(SgVlbiMeasurement::Attr_NOT_VALID, config_->getUseDelayType());
//      m->addAttr(SgVlbiMeasurement::Attr_NOT_VALID);
        numOfRestored++;
#ifdef LOCAL_DEBUG
        std::cout << "obs# " << o->owner()->getMediaIdx() << ":  (" << qPrintable(o->epoch().toString()) 
                  << ")  " << qPrintable(o->baseline()->getKey() + " @" + o->src()->getKey())
                  << "   " << o->activeDelay()->getResidualNorm() << "  "
                  << o->activeDelay()->getResidual()*1.0e9 << " +/- " 
                  << o->activeDelay()->getSigma()*1.0e9
                  << "\n";
#endif
      };
      numOfPasses++;
      hasBeenModified = true;
    }
    else
    {
      hasBeenModified = false;
#ifdef LOCAL_DEBUG
      std::cout << "the size is 0\n";
#endif
    };
    outlierCandidates.clear();
  };

#ifdef LOCAL_DEBUG
  if (numOfPasses==maxNumOfPasses)
    std::cout << "reached max passing number (" << numOfPasses << ")\n";
#endif

  logger->write(SgLogger::INF, SgLogger::PREPROC, className() +
    "::restoreOutliers(): restored " + QString("").setNum(numOfRestored) +
    " observables from " + band->getKey() + "-band in " + QString("").setNum(numOfPasses) +
    " passes with the threshold " + QString("").setNum(threshold));
  // restore config:
  config_->setActiveBandIdx(bandIdxSaved);
  if (wasMeddledIntoWc)
    config_->setDoWeightCorrection(true);
  hasOutlierRestoratioRun_ = true;
  if (hasOutlierEliminationRun_)
    addAttr(Attr_FF_OUTLIERS_PROCESSED);
  return numOfRestored;
};




//
//
//
//
// -------------------------- ambiguity resolving part:
//
void SgVlbiSession::scanBaselines4GrDelayAmbiguities(int bandIdx)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return;
  SgVlbiBand                   *band=bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::scanBaselines4GrDelayAmbiguities(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return;
  };
  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    //obs->setupActiveObservable(bandKey);
    obs->setupActiveMeasurement(config_);
  };

  bool                          noTriange=band->stationsByName().size()<3;
  //
  BaselinesByName_it            itBaselineInfo=band->baselinesByName().begin();
  for (; itBaselineInfo!=band->baselinesByName().end(); ++itBaselineInfo)
  {
    SgVlbiBaselineInfo         *baselineInfo=itBaselineInfo.value();
    if (isInUse(baselineInfo))
    {
      baselineInfo->scan4Ambiguities();
      baselineInfo->evaluateMeanGrDelResiduals();
      if (noTriange)
        baselineInfo->setGrdAmbiguities2min();
    };
  };

  // no any closed triangle:
  if (noTriange)
  {
    addAttr(Attr_FF_AMBIGS_RESOLVED);
    return;
  };
  
  resolveGrAmbigTriangles(bandIdx);

  //
  addAttr(Attr_FF_AMBIGS_RESOLVED);
};



//
void SgVlbiSession::resolveGrAmbigTriangles(int bandIdx)
{
  SgVlbiBand                   *band=bands_.at(bandIdx);
  SgVlbiStationInfo            *refStation=NULL, *station=NULL;
  int                           m;
  //
  // session's map:
  m = 0;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo         *stn=it.value();
    if (isInUse(stn) && stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      refStation = stn;
    if (m < stn->numProcessed(DT_DELAY))
    {
      m = stn->numProcessed(DT_DELAY);
      station = stn;
    };
  }
  // if the atribute not set yet, choose the reference station that has max observations:
  if (!refStation)
  {
    if (station)
    {
      refStation = station;
      logger->write(SgLogger::WRN, SgLogger::PREPROC, className() +
        "::resolveGrAmbigTriangles(): cannot find the reference clock station, picked " +
        refStation->getKey() + " as the reference one");
    }
    else
    {
      logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
        "::resolveGrAmbigTriangles(): unable to determine the reference clock station");
      return;
    };
  }
  else
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::resolveGrAmbigTriangles(): found " + refStation->getKey() + " as the reference clock station");

  //
  // create a list of stations and a list of baselines; the elements of the lists are
  // pointers, so no time consuming copies required
  QList<SgVlbiStationInfo*>     stations;
  QList<SgVlbiBaselineInfo*>    baselines;
  //
  //
  //StationsByName_it             itStationInfo=band->stationsByName().begin();
  for (StationsByName_it it=band->stationsByName().begin(); it!=band->stationsByName().end(); ++it)
    if (isInUse(it.value()))
      stations.append(it.value());
  //
  for (BaselinesByName_it it=band->baselinesByName().begin(); it!=band->baselinesByName().end(); ++it)
    if (isInUse(it.value()))
      baselines.append(it.value());
  //
  int                           numOfStations=stations.size();
  int                           numOfBaselines=baselines.size();

  SgEstimator                  *estimator=new SgEstimator(config_);
  QList<SgParameter*>          *localParameters=new QList<SgParameter*>;
  // create parameters list:
  SgParameter                  *pN;
  for (int iBaseline=0; iBaseline<numOfBaselines; iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    pN = new SgParameter("N_" + QString("").sprintf("%03d_(", iBaseline) +
      baselineInfo->getKey() + ")");
    pN->setSigmaAPriori(1.0e+8);
    baselineInfo->pAux() = pN;
    localParameters->append(pN);
  };
  estimator->addParametersList(localParameters);
  estimator->prepare2Run(numOfStations*(numOfStations-1)*(numOfStations-2)/2/3 + 10, 
    tStart_, tFinis_, tRefer_);
  // make constraints:
  // first, figure out the reference station:
  // lookup session's map for a reference station:
  // apply constraints:
  for (int i=0; i<numOfBaselines; i++)
  {
    SgVlbiBaselineInfo        *bl=baselines.at(i);
    if (bl->getKey().contains(refStation->getKey() + ":") || 
        bl->getKey().contains(":" + refStation->getKey())  )
    {
      bl->pAux()->setD(1.0);
      estimator->processConstraint(0.0, 1.0e-13);
    };
  };
  // feed the estimator:
  double                        b, sig;
  SgVector                      vO_C(1), vSigma(1);
  SgVlbiBaselineInfo           *biIJ, *biIK, *biJK;
  SgVlbiStationInfo            *stI, *stJ, *stK;
  QString                       blName;
  SgMJD                         t=getTMean();

  for (int i=0; i<numOfStations; i++)
  {
    stI = stations.at(i);
    for (int j=i+1; j<numOfStations; j++)
    {
      stJ = stations.at(j);
      biIJ = NULL;
      // set up ij-th baseline:
      blName = stI->getKey() + ":" + stJ->getKey();
      if (band->baselinesByName().contains(blName))
      {
        biIJ = band->baselinesByName().value(blName);
        biIJ->setAuxSign( 1.0);
      }
      else
      {
        blName = stJ->getKey() + ":" + stI->getKey();
        if (band->baselinesByName().contains(blName))
        {
          biIJ = band->baselinesByName().value(blName);
          biIJ->setAuxSign(-1.0);
        };
      };

      for (int k=j+1; k<numOfStations; k++)
      {
        stK = stations.at(k);
        biIK = NULL;
        biJK = NULL;
        // set up ik-th baseline:
        blName = stI->getKey() + ":" + stK->getKey();
        if (band->baselinesByName().contains(blName))
        {
          biIK = band->baselinesByName().value(blName);
          biIK->setAuxSign( 1.0);
        }
        else
        {
          blName = stK->getKey() + ":" + stI->getKey();
          if (band->baselinesByName().contains(blName))
          {
            biIK = band->baselinesByName().value(blName);
            biIK->setAuxSign(-1.0);
          };
        };
        // set up jk-th baseline:
        blName = stJ->getKey() + ":" + stK->getKey();
        if (band->baselinesByName().contains(blName))
        {
          biJK = band->baselinesByName().value(blName);
          biJK->setAuxSign( 1.0);
        }
        else
        {
          blName = stK->getKey() + ":" + stJ->getKey();
          if (band->baselinesByName().contains(blName))
          {
            biJK = band->baselinesByName().value(blName);
            biJK->setAuxSign(-1.0);
          };
        };
        // process the triangle:
        if (isInUse(stI)  &&  isInUse(stJ)  &&  isInUse(stK)  && 
            isInUse(biIJ) &&  isInUse(biIK) &&  isInUse(biJK)    )
        {
          b = biIK->getMeanGrDelResiduals()*biIK->getAuxSign() -
              biIJ->getMeanGrDelResiduals()*biIJ->getAuxSign() -
              biJK->getMeanGrDelResiduals()*biJK->getAuxSign();
          sig = sqrt(
            biIK->getMeanGrDelResidualsSigma()*biIK->getMeanGrDelResidualsSigma() +
            biIJ->getMeanGrDelResidualsSigma()*biIJ->getMeanGrDelResidualsSigma() +
            biJK->getMeanGrDelResidualsSigma()*biJK->getMeanGrDelResidualsSigma() );
//          biIJ->pAux()->setD( biIJ->getMaxGrdAmbigSpacing()*biIJ->getAuxSign());
//          biIK->pAux()->setD(-biIK->getMaxGrdAmbigSpacing()*biIK->getAuxSign());
//          biJK->pAux()->setD( biJK->getMaxGrdAmbigSpacing()*biJK->getAuxSign());
          biIJ->pAux()->setD(
            biIJ->getTypicalAmbigSpacing(config_->getUseDelayType())*biIJ->getAuxSign());
          biIK->pAux()->setD(
           -biIK->getTypicalAmbigSpacing(config_->getUseDelayType())*biIK->getAuxSign());
          biJK->pAux()->setD(
            biJK->getTypicalAmbigSpacing(config_->getUseDelayType())*biJK->getAuxSign());

          vO_C.setElement  (0, b);
          vSigma.setElement(0, sig);
          estimator->processObs(t, vO_C, vSigma);
        };
      };
    };
  };

  estimator->finisRun();
  // adjust ambiguities at the baselines:
  for (int iBaseline=0; iBaseline<numOfBaselines; iBaseline++)
  {
    SgVlbiBaselineInfo         *baselineInfo=baselines.at(iBaseline);
    int                         deltaN=rint(baselineInfo->pAux()->getSolution());
    baselineInfo->shiftAmbiguities(deltaN);
    // std::cout << qPrintable(baselineInfo->getKey()) << ": "
    //           << baselineInfo->pAux()->getSolution() << " +/ " << baselineInfo->pAux()->getSigma()
    //           << "\n";
  };
  //
  stations.clear();
  baselines.clear();
  //
  for (int i=0; i<localParameters->size(); i++)
    delete localParameters->at(i);
  delete localParameters;
  localParameters = NULL;
  delete estimator;
};



//
void SgVlbiSession::resetDelayAmbiguities(int bandIdx, SgTaskConfig::VlbiDelayType dType)
{
  if (bandIdx<0 || bandIdx>bands_.size()-1)
    return;
  //
  SgVlbiBand                   *band=bands_.at(bandIdx);
  if (!band)
  {
    logger->write(SgLogger::ERR, SgLogger::PREPROC, className() +
      "::resetDelayAmbiguities(): got wrong band (=NULL) for the band index " +
      QString("").setNum(bandIdx));
    return;
  };
  //
  for (int i=0; i<band->observables().size(); i++)
  {
    SgVlbiObservable           *o=band->observables().at(i);
    switch (dType)
    {
      case SgTaskConfig::VD_GRP_DELAY:
        o->grDelay().setResidual(o->grDelay().getResidual() - o->grDelay().ambiguity());
        o->grDelay().setNumOfAmbiguities(0);
        break;
      case SgTaskConfig::VD_PHS_DELAY:
        o->phDelay().setResidual(o->phDelay().getResidual() - o->phDelay().ambiguity());
        o->phDelay().setNumOfAmbiguities(0);
        break;
      default:
        break;
    };
  };
};



//
bool SgVlbiSession::need2runAutomaticDataProcessing()
{
  // if the session was not processed yet or was analyzed at another analysis center,
  // we are going to process it. Otherwise (i.e., it was already analyzed by us) just
  // to read data and display it.
  //
  if (!(isAttr(Attr_HAS_IONO_CORR) && isAttr(Attr_HAS_WEIGHTS)))
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): this session is not processed yet");
    return true;
  };
  
  const SgVlbiHistory&          history=primaryBand_->history();
  int                           ver, verOfExactAcFullName, verOfAcFullName, verOfAcAbbrevName;
  int                           verOfDomainName;

  if (history.size()<3) // nothing to check
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): the history too short");
    return true;
  };
  //
  if (!reporter_ || !reporter_->identities())
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): the reporter is not exists yet");
    return true; // nowhere to get clues
  };
  //
  SgIdentities                 *ids=reporter_->identities();
  QString                       str("");
  QStringList                   ambigNames, fullNameElements, abbrevNameElements;
  ambigNames  << "Authority" << "Center" << "Centre" << "Centro" << "Institute" << "National"
              << "Observatory" << "Observatoire" << "Technology" << "University";

  str = "(?:";
  for (int i=0; i<ambigNames.size()-1; i++)
    str += ambigNames.at(i) + "|";
  str += ambigNames.at(ambigNames.size()-1) + ")";

  QRegExp                       rxF(str), rxA, rxM;
  rxF.setCaseSensitivity(Qt::CaseInsensitive);
  rxA.setCaseSensitivity(Qt::CaseInsensitive);
  rxM.setCaseSensitivity(Qt::CaseInsensitive);
  
  // to serch for a full ac name or part of it:
  fullNameElements = ids->getAcFullName().split(QRegExp("\\W+"), QString::SkipEmptyParts);
  str = "(?:";
  for (int i=0; i<fullNameElements.size(); i++)
    if (!fullNameElements.at(i).contains(rxF))
      str += fullNameElements.at(i) + "|";
  if (str[str.size()-1] == QChar('|'))
     str[str.size()-1] = QChar(')');
  rxF.setPattern(str);

  // to serch for an abbreviated ac name or part of it:
  abbrevNameElements = ids->getAcAbbrevName().split(QRegExp("\\W+"), QString::SkipEmptyParts);
  str = "(?:";
  for (int i=0; i<abbrevNameElements.size(); i++)
    str += abbrevNameElements.at(i) + "|";
  if (str[str.size()-1] == QChar('|'))
     str[str.size()-1] = QChar(')');
  rxA.setPattern(str);

  // to serch for our domain name:
  str = ids->getUserEmailAddress();
  QString                       domainName("");

  rxM.setPattern("\\s*([A-Za-z0-9_]+)@([A-Za-z0-9_]+\\.[A-Za-z0-9_]+)");

  if (ids->getUserEmailAddress().indexOf(rxM) != -1)
    domainName = rxM.cap(2);

  ver = history.last()->getVersion() - 1;
  verOfExactAcFullName = verOfAcFullName = verOfAcAbbrevName = verOfDomainName = -1;
  for (int i=0; i<history.size(); i++)
  {
    const QString&              st=history.at(i)->getText();
    int                         v=history.at(i)->getVersion();
    if (st.contains(ids->getAcFullName(), Qt::CaseInsensitive))
      verOfExactAcFullName = v;
    if (st.contains(rxF))
      verOfAcFullName = v;
    if (st.contains(rxA))
      verOfAcAbbrevName = v;
    if (domainName.size()>0 && st.contains(domainName, Qt::CaseInsensitive))
      verOfDomainName = v;
  };
  //
  if (verOfAcFullName == -1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): cannot find AC full name (" +
      ids->getAcFullName() + ") in the history");
    return true; 
  };
  if (verOfAcAbbrevName == -1)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): cannot find AC abbreviated name, " + ids->getAcAbbrevName() +
      ", in the history");
    return true;
  };
  //
  //
  // definitely, it is ours:
  if (verOfExactAcFullName == ver)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): we know this session, found exact full AC name");
    return false;
  };
  if (domainName.size()>0 && verOfDomainName==ver)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): we know this session, found contact info from our domain");
    return false;
  };
  //
  //
  if (verOfAcFullName < ver || verOfAcAbbrevName < ver)
  {
    logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
      "::need2runAutomaticDataProcessing(): found fingerprints (" + 
      QString("").sprintf("%d,%d", verOfAcFullName, verOfAcAbbrevName) +
      ") but not for the last version (" + QString("").sprintf("%d", ver) + ")");
    return true; 
  };
  //
  // perhaps:
  logger->write(SgLogger::DBG, SgLogger::PREPROC, className() +
    "::need2runAutomaticDataProcessing(): the session looks familiar");
  return false;
};



//
bool SgVlbiSession::hasReferenceClocksStation()
{
  bool                          has=false;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    if ( !stn->isAttr(SgVlbiStationInfo::Attr_NOT_VALID)        && 
          stn->isAttr(SgVlbiStationInfo::Attr_REFERENCE_CLOCKS))
      has = true;
  };
  return has;
};



//
bool SgVlbiSession::hasReferenceCoordinatesStation()
{
  bool                          has=false;
  for (StationsByName_it it=stationsByName_.begin(); it!=stationsByName_.end(); ++it)
  {
    SgVlbiStationInfo          *stn=it.value();
    if (isInUse(stn) && 
      (!stn->isAttr(SgVlbiStationInfo::Attr_ESTIMATE_COO) || 
        stn->isAttr(SgVlbiStationInfo::Attr_CONSTRAIN_COO) ) )
      has = true;
  };
  return has;
};



//
bool SgVlbiSession::writeUserData2File(const QString& fileName)
{
  bool                          isOk=false;
  QString                       str("");
  //
  QFile                         f(fileName);
  if (!f.open(QIODevice::WriteOnly))
  {
    logger->write(SgLogger::ERR, SgLogger::REPORT, className() +
      "::writeUserData2File(): error opening output file \"" + f.fileName() + "\"");
    return isOk;
  };
  //
  SgMJD                         prevT(tZero);
  QMap<QString, QString>        hasReported;
  // make output:
  QTextStream                   ts(&f);

  ts  << "#\n"
      << "#YYY MM DD HH MM SS  Station_ _ScanID_   Elevation    SlntHDel SlntWDel   ___partial___      "
      << "SlntHRte SlntWRte   ___partial___     _GradN_delay_    _GradE_delay_    _GradN_rate__    _GradE_rate__\n"
      << "#\n";

  for (int i=0; i<observations_.size(); i++)
  {
    SgVlbiObservation          *obs=observations_.at(i);
    SgVlbiAuxObservation       *auxObs_1=obs->auxObs_1(), *auxObs_2=obs->auxObs_2();

// YY MM DD HH MM SS.S station scan_name 
// slant_delay partial_slant_delay slant_delay_rate partial_slant_delay_rate
    if (prevT < *obs)
    {
      hasReported.clear();
      prevT = *obs;
    };
    if (auxObs_1 && auxObs_2)
    {
      if (!hasReported.contains(obs->stn_1()->getKey()))
      {
        str.sprintf("%s  %8s %10s %9.6f  %10.2f %8.2f %15.6e    %10.2f %8.2f %15.6e"
                    "   %15.6e  %15.6e  %15.6e  %15.6e",
          qPrintable(obs->toString(SgMJD::F_Simple).replace(QRegExp("[:/]"), " ")),
          qPrintable(obs->stn_1()->getKey()),
          qPrintable(obs->getScanName()),
          auxObs_1->getElevationAngle()*RAD2DEG,
          auxObs_1->tmdActual()->getVal0_delay()*1.0e12,
          auxObs_1->tmdActual()->getVal1_delay()*1.0e12, 
          auxObs_1->tmdActual()->getMap1_delay(), 
          auxObs_1->tmdActual()->getVal0_rate ()*1.0e15,
          auxObs_1->tmdActual()->getVal1_rate ()*1.0e15, 
          auxObs_1->tmdActual()->getMap1_rate (),
          auxObs_1->tmdActual()->getGrdN_delay(),
          auxObs_1->tmdActual()->getGrdE_delay(),
          auxObs_1->tmdActual()->getGrdN_rate (),
          auxObs_1->tmdActual()->getGrdE_rate ());
        ts << str << "\n";
        hasReported.insert(obs->stn_1()->getKey(), "1");
      };
      if (!hasReported.contains(obs->stn_2()->getKey()))
      {
        str.sprintf("%s  %8s %10s %9.6f  %10.2f %8.2f %15.6e    %10.2f %8.2f %15.6e"
                    "   %15.6e  %15.6e  %15.6e  %15.6e",
          qPrintable(obs->toString(SgMJD::F_Simple).replace(QRegExp("[:/]"), " ")),
          qPrintable(obs->stn_2()->getKey()),
          qPrintable(obs->getScanName()),
          auxObs_2->getElevationAngle()*RAD2DEG,
          auxObs_2->tmdActual()->getVal0_delay()*1.0e12,
          auxObs_2->tmdActual()->getVal1_delay()*1.0e12, 
          auxObs_2->tmdActual()->getMap1_delay(), 
          auxObs_2->tmdActual()->getVal0_rate ()*1.0e15,
          auxObs_2->tmdActual()->getVal1_rate ()*1.0e15, 
          auxObs_2->tmdActual()->getMap1_rate (),
          auxObs_2->tmdActual()->getGrdN_delay(),
          auxObs_2->tmdActual()->getGrdE_delay(),
          auxObs_2->tmdActual()->getGrdN_rate (),
          auxObs_2->tmdActual()->getGrdE_rate ());
        ts << str << "\n";
        hasReported.insert(obs->stn_2()->getKey(), "2");
      };
    };
  };
  ts.setDevice(NULL);
  f.close();
  return true;
};
/*=====================================================================================================*/






/*=====================================================================================================*/
//
//                           FRIENDS:
// 
/*=====================================================================================================*/
//



/*=====================================================================================================*/
//
// aux functions:
//
bool normResidSortingOrderLessThan(SgVlbiObservable *o1, SgVlbiObservable *o2)
{
  return 
    fabs(o1->activeDelay()->getResidualNorm()) > fabs(o2->activeDelay()->getResidualNorm());
};



//
void evaluatePCal4GrpDelay(SgVector& pCalByChan, const SgVector& rFreqByChan, int numOfChannels,
                          double& pCal4GrpDelay, double& pCalSigma4GrpDelay,
                          int obsIdx, const SgMJD& epoch, 
                          const QString& sourceName, const QString& baselineName)
{
  int numOfActualChannels = rFreqByChan.n();
  while (rFreqByChan.getElement(numOfActualChannels-1)==0.0)
    numOfActualChannels--;
    
  if (numOfChannels != numOfActualChannels)
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      QString("").sprintf("evaluatePCal4GrpDelay(): number of actual channels (%d) is not equal"
      "to the declared number (%d), for the Obs#%d, %s @%s, on ", 
      numOfActualChannels, numOfChannels, obsIdx, qPrintable(sourceName), qPrintable(baselineName))
      + epoch.toString());

  if (!numOfActualChannels)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      QString("").sprintf("evaluatePCal4GrpDelay(): number of actual channels is zero, "
      "nothing to do, for the Obs#%d, %s @%s, on ",
      obsIdx, qPrintable(sourceName), qPrintable(baselineName)) + epoch.toString());
    return;
  };
  if (numOfActualChannels==1)
  {
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      QString("").sprintf("evaluatePCal4GrpDelay(): only one actual channel, "
      "nothing to do, for the Obs#%d, %s @%s, on ",
      obsIdx, qPrintable(sourceName), qPrintable(baselineName)) + epoch.toString());
    return;
  };
  
  for (int i=0; i<numOfActualChannels; i++)
    pCalByChan.setElement(i, fmod(pCalByChan.getElement(i), 360.0));

  for (int i=0; i<numOfActualChannels; i++)
    pCalByChan.setElement(i, pCalByChan.getElement(i) - pCalByChan.getElement(0));

  for (int i=0; i<numOfActualChannels; i++)
  {
    while (pCalByChan.getElement(i) > 180.0)
      pCalByChan.setElement(i, pCalByChan.getElement(i) - 360.0);
    while (pCalByChan.getElement(i) < -180.0)
      pCalByChan.setElement(i, pCalByChan.getElement(i) + 360.0);
  };
  // now the phases are in cycles:
  for (int i=0; i<numOfActualChannels; i++)
    pCalByChan.setElement(i, pCalByChan.getElement(i)/360.0);

  for (int j=0; j<2; j++)
    for (int i=0; i<numOfActualChannels-1; i++)
    {
      if (pCalByChan.getElement(i+1) - pCalByChan.getElement(i) > 0.5)
        pCalByChan.setElement(i+1, pCalByChan.getElement(i+1) - 1.0);
      if (pCalByChan.getElement(i+1) - pCalByChan.getElement(i) < -0.5)
        pCalByChan.setElement(i+1, pCalByChan.getElement(i+1) + 1.0);
    };
  
  // estimate the slope:
  double      weight=1.0;
  double      sum=0.0, sumX=0.0, sumY=0.0;
  double      sumXY=0.0, sumX2=0.0, sumY2=0.0;
  double      a=0.0, b=0.0, /*sigmaA=0.0,*/ sigmaB=0.0;
  for (int i=0; i<numOfActualChannels; i++)
  {
    sum  += weight;
    sumX += weight*rFreqByChan.getElement(i);
    sumY += weight* pCalByChan.getElement(i);
    sumXY+= weight*rFreqByChan.getElement(i)* pCalByChan.getElement(i);
    sumX2+= weight*rFreqByChan.getElement(i)*rFreqByChan.getElement(i);
    sumY2+= weight* pCalByChan.getElement(i)* pCalByChan.getElement(i);
  };
  double delta = sum*sumX2 - sumX*sumX;
  a = (sumX2*sumY - sumX*sumXY)/delta;
  b = (sumXY*sum  - sumX*sumY )/delta;
  double varnc;
  if (numOfActualChannels>2)
  {
    varnc = (sumY2 + a*a*sum + b*b*sumX2 - 2.0*(a*sumY + b*sumXY - a*b*sumX))/(numOfActualChannels - 2);
    // sigmaA = sqrt(varnc*sumX2/delta);
    sigmaB = sqrt(varnc*sum  /delta);
  }
  else
  {
    // sigmaA = 0.0;
    sigmaB = 0.0;
  };

  pCal4GrpDelay = b*1.0e6;
  pCalSigma4GrpDelay = sigmaB*1.0e6;
  return;
};







//
// Evaluating the ionospherie effective frequencies for group delay, phase delay and
// group rate observables. The code is from "mk5/progs/solve/sdbh/ionfr.f" file.
// utils/db2vgosDB/do_freq.f
// utils/db2vgosDB/do_freq_merge.f
//
void evaluateEffectiveFreqs_old_version(
  // input:
  const SgVector& numOfAccPeriodsByChan_USB, const SgVector& numOfAccPeriodsByChan_LSB,
  const SgVector& refFreqByChan, const SgVector& fringeAmplitudeByChan,
  const SgVector& numOfSamplesByChan_USB, const SgVector& numOfSamplesByChan_LSB,
  double sampleRate, double refFreq, int numOfChannels,
  // output:
  double& effFreq4GR, double& effFreq4PH, double& effFreq4RT,
  // info about the observation (for Log's purposes) and equalWeightsFlag:
  const QString& oId, bool useEqualWeights)
{
  // set up nominal values:
  effFreq4GR = effFreq4PH = effFreq4RT = refFreq;
  
  // first, check the declared number of channels:
  int                           numOfActualChannels=refFreqByChan.n();
  while (numOfActualChannels && refFreqByChan.getElement(numOfActualChannels-1)==0.0)
    numOfActualChannels--;
    
  if (numOfChannels != numOfActualChannels)
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      QString("").sprintf("evaluateEffectiveFreqs_old(): number of actual channels (%d) is not equal"
      "to the declared number (%d) for obs ", numOfActualChannels, numOfChannels) + oId);
  if (numOfActualChannels == 0)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs_old(): number of actual channels is zero, nothing to do for obs " + oId);
    return;
  };
  if (numOfActualChannels == 1)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs_old(): only one actual channel, nothing to do for obs " + oId);
    return;
  };

  int                           numOfGoodChannels=0;
  int                           numOfGoodAccPeriods=0;
  bool                          hasBadAmplitude=false;
  bool                          hasSequenceBug=true;
  bool                          areSampleNumsUsable=false;
  bool                          areAllFreqsSame=false;
  double                        minFrq, maxFrq, d;
  minFrq = maxFrq = refFreqByChan.getElement(0);
  // check the channels set up:
  for (int i=0; i<numOfActualChannels; i++)
  {
    // fringe amplitudes:
    if (fringeAmplitudeByChan.getElement(i)<0.0 || fringeAmplitudeByChan.getElement(i)>1.0)
      hasBadAmplitude = true; // at least one value are bad (ionfr.f: BAD_CHAN)
    //
    if ((numOfAccPeriodsByChan_USB.getElement(i) + numOfAccPeriodsByChan_LSB.getElement(i))*
      fringeAmplitudeByChan.getElement(i) > 1.0E-8)
      numOfGoodChannels++;
    //
    if (numOfAccPeriodsByChan_USB.getElement(i) + numOfAccPeriodsByChan_LSB.getElement(i) > 0.0)
      numOfGoodAccPeriods++;
    //
    if (i>0 && numOfAccPeriodsByChan_LSB.getElement(i)-1 != numOfAccPeriodsByChan_LSB.getElement(i-1))
      hasSequenceBug = false;
    //
    if (numOfSamplesByChan_USB.getElement(i)!=0.0 || numOfSamplesByChan_LSB.getElement(i)!=0.0)
      areSampleNumsUsable = true;
    //
    if (refFreqByChan.getElement(i) > maxFrq)
      maxFrq = refFreqByChan.getElement(i);
    //
    if (refFreqByChan.getElement(i) < minFrq)
      minFrq = refFreqByChan.getElement(i);
  };
  if (maxFrq-minFrq < 0.001) // MHz
    areAllFreqsSame = true;

  if (areAllFreqsSame) // complain, return
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs_old(): all frequencies are the same, nothing to do for obs " + oId);
    return;
  };

  if (numOfActualChannels!=numOfGoodChannels || numOfActualChannels!=numOfGoodAccPeriods)// just complain
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      QString("").sprintf("evaluateEffectiveFreqs_old(): the numbers of actual channels (%d), "
      "good channels (%d) and good accumulated periods (%d) are not the same "
      "for obs ", numOfActualChannels, numOfGoodChannels, numOfGoodAccPeriods) + oId);

  SgVector                      weightByChan(numOfActualChannels);
  // from John's ionfr.f:
  SgVector                      weightByChan_USB(numOfActualChannels);
  SgVector                      weightByChan_LSB(numOfActualChannels);
  //
  if (numOfGoodChannels<2 || numOfGoodAccPeriods<2 || hasSequenceBug || hasBadAmplitude)
    for (int i=0; i<numOfActualChannels; i++)
    {
      weightByChan.setElement    (i, 1.0);
      weightByChan_USB.setElement(i, 0.0);
      weightByChan_LSB.setElement(i, 1.0);
    }
  else if (!areSampleNumsUsable)
    for (int i=0; i<numOfActualChannels; i++)
    {
      d = 1.0;
      if (!useEqualWeights)
        d = (numOfAccPeriodsByChan_USB.getElement(i) + numOfAccPeriodsByChan_LSB.getElement(i))
          *fringeAmplitudeByChan.getElement(i);
      weightByChan.setElement(i, d);
      weightByChan_USB.setElement(i, numOfAccPeriodsByChan_USB.getElement(i));
      weightByChan_LSB.setElement(i, numOfAccPeriodsByChan_LSB.getElement(i));
    }
  else
    for (int i=0; i<numOfActualChannels; i++)
    {
      d = 1.0;
      if (!useEqualWeights)
        d = (numOfSamplesByChan_USB.getElement(i) + numOfSamplesByChan_LSB.getElement(i))
          *fringeAmplitudeByChan.getElement(i)/sampleRate;
      weightByChan.setElement(i, d);
      weightByChan_USB.setElement(i, numOfSamplesByChan_USB.getElement(i));
      weightByChan_LSB.setElement(i, numOfSamplesByChan_LSB.getElement(i));
    };

  // make sums:
  double                        sumWei=0.0;
  double                        sumFr1=0.0;
  double                        sumFr2=0.0;
  double                        sumFq2=0.0;
  double                        sumFri=0.0;
  double                        sumDfi=0.0;
  //
  double                        halfBw=sampleRate/4.0/1.0e6;
  double                        df, frq_i;
  //
  for (int i=0; i<numOfActualChannels; i++)
  {
    frq_i = refFreqByChan.getElement(i);
    // new way:
    if (weightByChan_USB.getElement(i)>0.0 && weightByChan_LSB.getElement(i)<1.0E-3)
      frq_i = refFreqByChan.getElement(i) - halfBw;
    else if (weightByChan_USB.getElement(i)<1.0E-3 && weightByChan_LSB.getElement(i)>0.0)
      frq_i = refFreqByChan.getElement(i) + halfBw;
    //
    df = frq_i - refFreq;
    sumWei += weightByChan.getElement(i);
    sumFr1 += weightByChan.getElement(i)*df;
    sumFr2 += weightByChan.getElement(i)*df*df;
    sumFq2 += weightByChan.getElement(i)*frq_i*frq_i;
    sumFri += weightByChan.getElement(i)/frq_i;
    sumDfi += weightByChan.getElement(i)*df/frq_i;
  };
  double                        da=sumWei*sumFr2 - sumFr1*sumFr1;
  double                        db=sumFri*sumFr1 - sumDfi*sumWei;
  double                        dc=sumFri*sumFr2 - sumDfi*sumFr1;

  effFreq4GR = (fabs(db)>1.0E-10)?sqrt(da/db):refFreq;
  effFreq4PH = (fabs(dc)>1.0E-10)?sqrt(refFreq*da/dc):refFreq;
  effFreq4RT = sqrt(sumFq2/sumWei);

  return;
};




//
// Evaluating the ionospherie effective frequencies for group delay, phase delay and
// group rate observables. The code is from "mk5/progs/solve/sdbh/ionfr.f" file.
// utils/db2vgosDB/do_freq.f
// utils/db2vgosDB/do_freq_merge.f
//
void evaluateEffectiveFreqs(
  // input:
  const SgVector& numOfAccPeriodsByChan_USB, const SgVector& numOfAccPeriodsByChan_LSB,
  const SgVector& refFreqByChan, const SgVector& fringeAmplitudeByChan,
  const SgVector& numOfSamplesByChan_USB, const SgVector& numOfSamplesByChan_LSB,
  const SgVector& channelBandwidth, double refFreq, int numOfChannels,
  // output:
  double& effFreq4GR, double& effFreq4PH, double& effFreq4RT,
  // type of a correlator:
  const QString& correlatorType,
  // info about the observation (for Log's purposes) and equalWeightsFlag:
  //  const QString& oId, 
  SgVlbiObservable* o,
  bool useEqualWeights, bool mimicSignError)
{
  QString                       str("");
  QString                       oId(o->strId());
/*
std::cout << "ObsId: " << qPrintable(oId) << "\n";
std::cout << "sampleRate= " << sampleRate << " refFreq=" << refFreq << " numOfChannels=" << numOfChannels << "\n";
std::cout << "numOfAccPeriodsByChan_USB= " << numOfAccPeriodsByChan_USB << " numOfAccPeriodsByChan_LSB=" << numOfAccPeriodsByChan_LSB << "\n";
std::cout << "refFreqByChan= " << refFreqByChan << " fringeAmplitudeByChan=" << fringeAmplitudeByChan << "\n";
std::cout << "numOfSamplesByChan_USB= " << numOfSamplesByChan_USB << " numOfSamplesByChan_LSB=" << numOfSamplesByChan_LSB << "\n\n";
*/
  // set up nominal values:
  effFreq4GR = effFreq4PH = effFreq4RT = refFreq;

  //
  // first, check the declared number of channels:
  int                           numOfActualChannels=refFreqByChan.n();
  while (numOfActualChannels && fabs(refFreqByChan.getElement(numOfActualChannels-1))<1.0e-6)
    numOfActualChannels--;


  if (numOfActualChannels == 0)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): number of actual channels is zero, nothing to do for the obs " + oId);
    return;
  };
  if (numOfActualChannels == 1)
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): only one actual channel, nothing to do for the obs " + oId);
    return;
  };
  //
  if (!(correlatorType.contains("GSI", Qt::CaseInsensitive) || 
        correlatorType.contains("K5", Qt::CaseInsensitive)) &&
      numOfChannels != numOfActualChannels)
    logger->write(SgLogger::INF, SgLogger::PREPROC,
      str.sprintf("evaluateEffectiveFreqs(): number of actual channels (%d) is not equal"
      "to the declared number (%d) for the obs ", numOfActualChannels, numOfChannels) + oId);
  //
  //
  int                           numOfGoodChannels=0;
  int                           numOfGoodAccPeriods=0;
  bool                          hasBadAmplitude=false;
  bool                          hasSequenceBug=true;
  bool                          areSampleNumsUsable=false;
  bool                          areAllFreqsSame=false;
  double                        minFrq, maxFrq;
  minFrq = maxFrq = refFreqByChan.getElement(0);
  // check the channels set up:
  for (int i=0; i<numOfActualChannels; i++)
  {
    // fringe amplitudes:
    if (fringeAmplitudeByChan.getElement(i)<0.0 || fringeAmplitudeByChan.getElement(i)>1.0)
      hasBadAmplitude = true; // at least one value are bad (ionfr.f: BAD_CHAN)
    //
    if ((numOfAccPeriodsByChan_USB.getElement(i) + numOfAccPeriodsByChan_LSB.getElement(i))*
      fringeAmplitudeByChan.getElement(i) > 1.0E-8)
      numOfGoodChannels++;
    //
    if (numOfAccPeriodsByChan_USB.getElement(i) + numOfAccPeriodsByChan_LSB.getElement(i) > 0.0)
      numOfGoodAccPeriods++;
    //
    if (i>0 && numOfAccPeriodsByChan_LSB.getElement(i)-1 != numOfAccPeriodsByChan_LSB.getElement(i-1))
      hasSequenceBug = false;
    //
    if (numOfSamplesByChan_USB.getElement(i)>=1.0 || numOfSamplesByChan_LSB.getElement(i)>=1.0)
      areSampleNumsUsable = true;
    //
    if (refFreqByChan.getElement(i) > maxFrq)
      maxFrq = refFreqByChan.getElement(i);
    //
    if (refFreqByChan.getElement(i) < minFrq)
      minFrq = refFreqByChan.getElement(i);
    //
  };

  if (maxFrq-minFrq < 0.001) // MHz
    areAllFreqsSame = true;

  if (areAllFreqsSame) // complain, return
  {
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): all frequencies are the same, nothing to do for obs " + oId);
    return;
  };

  if (!(correlatorType.contains("GSI", Qt::CaseInsensitive) || 
        correlatorType.contains("K5", Qt::CaseInsensitive)  ||
        o->owner()->session()->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA) &&
      (numOfActualChannels!=numOfGoodChannels || numOfActualChannels!=numOfGoodAccPeriods)) // complain
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      str.sprintf("evaluateEffectiveFreqs(): the numbers of actual channels (%d), "
      "good channels (%d) and good accumulated periods (%d) are not the same "
      "for obs ", numOfActualChannels, numOfGoodChannels, numOfGoodAccPeriods) + oId);
  if (hasBadAmplitude) // just complain
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): bad amplitudes are detected for the obs " + oId);
  if (hasSequenceBug)  // just complain
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): the sequence bug is detected for the obs " + oId);

  //
  // PIMA does not have data if no fringe case:
  if (o->owner()->session()->getCppsSoft() == SgVlbiSessionInfo::CPPS_PIMA && o->getQualityFactor()==0)
    return; // nothing to do


  SgVector                      noAP_USB(numOfActualChannels);
  SgVector                      noAP_LSB(numOfActualChannels);
  for (int i=0; i<numOfActualChannels; i++)
  {
    if (useEqualWeights)
    {
      if (numOfSamplesByChan_USB.getElement(i) > 0.0)
        noAP_USB.setElement(i, 1.0);
      else 
        noAP_USB.setElement(i, 0.0);
      if (numOfSamplesByChan_LSB.getElement(i) > 0.0)
        noAP_LSB.setElement(i, 1.0);
      else 
        noAP_LSB.setElement(i, 0.0);
    }
    else if (areSampleNumsUsable)
    {
      noAP_USB.setElement(i, numOfSamplesByChan_USB.getElement(i));
      noAP_LSB.setElement(i, numOfSamplesByChan_LSB.getElement(i));
    }
    else
    {
      noAP_USB.setElement(i, numOfAccPeriodsByChan_USB.getElement(i));
      noAP_LSB.setElement(i, numOfAccPeriodsByChan_LSB.getElement(i));
    };
  };

  // make sums:
  double                        sumWei=0.0;
  double                        sumFr1=0.0;
  double                        sumFr2=0.0;
  double                        sumFq2=0.0;
  double                        sumFri=0.0;
  double                        sumDfi=0.0;
  //
  double                        df, frq_i, wei2, weiUSB, weiLSB;

  // old way:
  if (false)
  {
    for (int i=0; i<numOfActualChannels; i++)
    {
      frq_i = refFreqByChan.getElement(i);
      df = frq_i - refFreq;
      if (!useEqualWeights)
      {
        if (!areSampleNumsUsable)
          wei2 = (noAP_USB.getElement(i) + noAP_LSB.getElement(i))*fringeAmplitudeByChan.getElement(i);
        else
          wei2 = (numOfSamplesByChan_USB.getElement(i) + numOfSamplesByChan_LSB.getElement(i))
                  *fringeAmplitudeByChan.getElement(i);
      }
      else
        wei2 = 1.0;
      //
      sumWei += wei2;
      sumFr1 += wei2*df;
      sumFr2 += wei2*df*df;
      sumFq2 += wei2*frq_i*frq_i;
      sumFri += wei2/frq_i;
      sumDfi += wei2*df/frq_i;
    };
  }
  // new way:
  else
  {
    for (int i=0; i<numOfActualChannels; i++)
    {
      frq_i = refFreqByChan.getElement(i);
      //
      if (!useEqualWeights)
      {
        if (!areSampleNumsUsable)
        {
          weiUSB = noAP_USB.getElement(i);
          weiLSB = noAP_LSB.getElement(i);
        }
        else
        {
          weiUSB = numOfSamplesByChan_USB.getElement(i);
          weiLSB = numOfSamplesByChan_LSB.getElement(i);
        };
        wei2 = (weiUSB + weiLSB)*fringeAmplitudeByChan.getElement(i);
        if (weiUSB>0.0 && weiLSB>0.0) // nothing to do
        {
        }
        else if (weiUSB > 0.0)
        {
          if (mimicSignError)
            frq_i -= 0.5*channelBandwidth.getElement(i);
          else
            frq_i += 0.5*channelBandwidth.getElement(i);
        }
        else if (weiLSB > 0.0)
        {
          if (mimicSignError)
            frq_i += 0.5*channelBandwidth.getElement(i);
          else
            frq_i -= 0.5*channelBandwidth.getElement(i);
        };
      }
      else
        wei2 = 1.0;

      df = frq_i - refFreq;

      sumWei += wei2;
      sumFr1 += wei2*df;
      sumFr2 += wei2*df*df;
      sumFq2 += wei2*frq_i*frq_i;
      sumFri += wei2/frq_i;
      sumDfi += wei2*df/frq_i;
    };
  };
  //
  double                        da=sumWei*sumFr2 - sumFr1*sumFr1;
  double                        db=sumFri*sumFr1 - sumWei*sumDfi;
  double                        dc=sumFri*sumFr2 - sumFr1*sumDfi;

  effFreq4GR = (fabs(db)<1.0E-12)?refFreq:sqrt(da/db);
  effFreq4PH = (fabs(dc)<1.0E-12)?refFreq:sqrt(refFreq*da/dc);
  effFreq4RT = (sumWei<1.0E-16)?refFreq:sqrt(sumFq2/sumWei);

  // complain:
  if (fabs(db)<1.0E-12)
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): suspicious channel setup (fabs(db)<1.0E-12) for the obs " + oId);
  if (fabs(dc)<1.0E-12)
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): suspicious channel setup (fabs(dc)<1.0E-12) for the obs " + oId);
  if (sumWei<1.0E-16)
    logger->write(SgLogger::WRN, SgLogger::PREPROC,
      "evaluateEffectiveFreqs(): suspicious channel setup (sumWei<1.0E-16) for the obs " + oId);
  //
  //
  /*
  if (logger->isEligible(SgLogger::DBG, SgLogger::IONO))
  {
    str.sprintf("EffFreqs (MHz): grDelay= %14.8f phDelay= %14.8f phDRate= %14.8f",
      effFreq4GR, effFreq4PH, effFreq4RT);
    logger->write(SgLogger::DBG, SgLogger::IONO,
      "evaluateEffectiveFreqs(): " + str);
  };
  */
  //
  return;
};
/*=====================================================================================================*/



/*=====================================================================================================*/
