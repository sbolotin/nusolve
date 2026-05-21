/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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
#include <math.h>


#include <QtCore/QDataStream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>


#include <SgConstants.h>
#include <SgStnLogCollector.h>
#include <SgVersion.h>
#include <SgIoExternalFilter.h>

#ifndef SEPARATED_LOG2ANT
#   include <SgVlbiAuxObservation.h>
#   include <SgVlbiObservation.h>
#   include <SgVlbiStationInfo.h>
#   include <SgVlbiSourceInfo.h>
#endif



QString asciifyString(const QString& str);




/*=======================================================================================================
*
*                           SgCableCalReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgCableCalReading::className()
{
  return "SgCableCalReading";
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgMeteoReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgMeteoReading::className()
{
  return "SgMeteoReading";
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgPointingReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTraklReading::className()
{
  return "SgTraklReading";
};
/*=====================================================================================================*/







/*=======================================================================================================
*
*                           SgTsysReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgTsysReading::className()
{
  return "SgTsysReading";
};



//
void SgTsysReading::addTsyses(const QMap<QString, float>& tsys)
{
  for (QMap<QString, float>::const_iterator it=tsys.begin(); it!=tsys.end(); ++it)
    tsys_.insert(it.key(), it.value());
};



//
void SgTsysReading::addTpconts(const QMap<QString, QVector<int> >& tpcont)
{
  for (QMap<QString, QVector<int> >::const_iterator it=tpcont.begin(); it!=tpcont.end(); ++it)
    tpcont_.insert(it.key(), it.value());
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgPcalReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgPcalReading::className()
{
  return "SgPcalReading";
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgSefdReading's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgSefdReading::className()
{
  return "SgSefdReading";
};



//
bool SgSefdReading::addValByChanKey(const SgMJD& t, const QString& srcName, double az, double el,
  const QString& chanKey, const QVector<double>& val)
{
  bool                        isOk=true;
  if (t_!=tZero && 0.3/DAY2SEC < fabs(t-t_))
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      "::addValByChanKey(): unexpected epoch: old: \"" + t_.toString(SgMJD::F_YYYYMMDDHHMMSSSS) + 
      "\", new: \"" + t.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
  };
  if (srcName_.size() && srcName != srcName_)
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      "::addValByChanKey(): source name has changed: old: \"" + srcName_ + "\", new: \"" + 
      srcName + "\"; record of " + t.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
  };
  if (-1000<az_ && az != az_)
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      "::addValByChanKey(): azimuth has changed: old: \"" + QString("").setNum(az_) + ", new: " + 
      QString("").setNum(az) + "; record of " + t.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
  };
  if (-1000<el_ && el != el_)
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      "::addValByChanKey(): elevation has changed: old: \"" + QString("").setNum(el_) + ", new: " + 
      QString("").setNum(el) + "; record of " + t.toString(SgMJD::F_YYYYMMDDHHMMSSSS));
  };
  QVector<double>            *v =new QVector<double>(val);
  if (valByChanKey_.contains(chanKey))
  {
    isOk = false;
    valByChanKey_.remove(chanKey);
    logger->write(SgLogger::WRN, SgLogger::RUN, className() +
      "::addValByChanKey(): the channel key \"" + chanKey + "\" is already in the map");
  };
  t_ = t;
  srcName_ = srcName;
  az_ = az;
  el_ = el;
  valByChanKey_.insert(chanKey, v);
  return isOk;
};


/*=====================================================================================================*/






/*=======================================================================================================
*
*                           SgOnSourceRecord's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgOnSourceRecord::className()
{
  return "SgOnSourceRecord";
};
/*=====================================================================================================*/







/*=======================================================================================================
*
*                           SgChannelSetup's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgChannelSetup::className()
{
  return "SgChannelSetup";
};



//
double SgChannelSetup::calcSkyFreq(double loFreq, double bbcFreq, double width, 
  SgChannelSideBand loSideBand, SgChannelSideBand ifSideBand)
{
  double                        skyFrq;
/*
  if a sideband of the IF is LSB and a sideband of LO is USB, then
     frq_if = frq_lo + frq_bbc - bandwidth

  if a sideband of the IF is LSB and a sideband of LO is LSB, then
     frq_if = frq_lo - frq_bbc - bandwidth

  if a sideband of the IF is USB and a sideband of LO is USB, then
     frq_if = frq_lo + frq_bbc

  and if a sideband of the IF is USB and a sideband of LO is LSB, then
     frq_if = frq_lo - frq_bbc  

*/ 
  //
  if (ifSideBand==CSB_LSB && loSideBand==CSB_USB)
    skyFrq = loFreq + bbcFreq - width;
  else if (ifSideBand==CSB_LSB && loSideBand==CSB_LSB)
    skyFrq = loFreq - bbcFreq - width;
  else if (ifSideBand==CSB_USB && loSideBand==CSB_USB)
    skyFrq = loFreq + bbcFreq;
  else if (ifSideBand==CSB_USB && loSideBand==CSB_LSB)
    skyFrq = loFreq - bbcFreq;
  else
    return -99.9;

  return skyFrq + width/2.0;
};






//
bool SgChannelSetup::selfCheck(const QString& stnKey)
{
  isOk_ = true;

  if (hwType_      == HT_VGOS   &&
      backEndType_ != BET_DBBC3 )    // dbbc3 setup can be figured out from a log file
  {
/*
    const double                frqsStd[64] = {
      3480.40, 3448.40, 3384.40, 3320.40, 3224.40, 3096.40, 3064.40, 3032.40,
      3480.40, 3448.40, 3384.40, 3320.40, 3224.40, 3096.40, 3064.40, 3032.40,
      
      5720.40, 5688.40, 5624.40, 5560.40, 5464.40, 5336.40, 5304.40, 5272.40, 
      5720.40, 5688.40, 5624.40, 5560.40, 5464.40, 5336.40, 5304.40, 5272.40, 

      6840.40, 6808.40, 6744.40, 6680.40, 6584.40, 6456.40, 6424.40, 6392.40, 
      6840.40, 6808.40, 6744.40, 6680.40, 6584.40, 6456.40, 6424.40, 6392.40, 

      10680.40,10648.40,10584.40,10520.40,10424.40,10296.40,10264.40,10232.40, 
      10680.40,10648.40,10584.40,10520.40,10424.40,10296.40,10264.40,10232.40};

    const double                frqsIsh[64] = {
      3032.40, 3064.40, 3096.40, 3224.40, 3320.40, 3384.40, 3448.40, 3480.40,
      3032.40, 3064.40, 3096.40, 3224.40, 3320.40, 3384.40, 3448.40, 3480.40,

      5272.40, 5304.40, 5336.40, 5464.40, 5560.40, 5624.40, 5688.40, 5720.40,
      5272.40, 5304.40, 5336.40, 5464.40, 5560.40, 5624.40, 5688.40, 5720.40,

      6392.40, 6424.40, 6456.40, 6584.40, 6680.40, 6744.40, 6808.40, 6840.40,
      6392.40, 6424.40, 6456.40, 6584.40, 6680.40, 6744.40, 6808.40, 6840.40,
 
      10232.40,10264.40,10296.40,10424.40,10520.40,10584.40,10648.40,10680.40,
      10232.40,10264.40,10296.40,10424.40,10520.40,10584.40,10648.40,10680.40};
*/
/*
    QList<QString>              chanIds;
    for (int i=0; i<16; i++)
      for (int j=0; j<2; j++)
        chanIds << QString("").sprintf("  %02da%01d", i, j) << QString("").sprintf("  %02db%01d", i, j)
                << QString("").sprintf("  %02dc%01d", i, j) << QString("").sprintf("  %02dd%01d", i, j);
*/ 

    // std. setup:
    if (!loFreqById_.size())
    {
      loFreqById_.insert("a0", 2472.4);
      loFreqById_.insert("a1", 2472.4);

      loFreqById_.insert("b0", 4712.4);
      loFreqById_.insert("b1", 4712.4);

      loFreqById_.insert("c0", 5832.4);
      loFreqById_.insert("c1", 5832.4);

      loFreqById_.insert("d0", 9672.4);
      loFreqById_.insert("d1", 9672.4);
    };

    for (QMap<QString, double>::iterator it=loFreqByTpSensorKey_.begin(); it!=loFreqByTpSensorKey_.end(); 
      ++it)
    {
      QString                   sensorKey=it.key();
      QString                   sensorId("");
      //
      if (ifIdByTpSensorKey_.contains(sensorKey))
        sensorId = ifIdByTpSensorKey_.value(sensorKey);
      else
        logger->write(SgLogger::ERR, SgLogger::IO, className() +
          "::selfCheck(): VGOS setup: cannot find sensor ID for key \"" +
          sensorKey + "\" for station \"" + stnKey + "\"");
      //
    };
  }
  else
  {
    if (bbcFreqByIdx_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map bbcFreqByIdx is empty");
    };
    if (bbcBandwidthByIdx_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map bbcBandwidthByIdx is empty");    
    };
    if (loIdByIdx_.size()==0 && loIdByCid_.size()==0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the maps loIdByIdx/loIdByCid are empty");    
    };
    if (loFreqById_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map loFreqById is empty");    
    };
    if (loSideBandById_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map loSideBandById is empty");    
    };
    if (loPolarizationById_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map loPolarizationById is empty");    
    };
    if (ifIdxById_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map ifIdxById is empty");    
    };
    if (ifSideBandById_.size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
        "the map ifSideBandById is empty");    
    };
    for (QMap<QString, int>::const_iterator it=ifIdxById_.begin(); it!=ifIdxById_.end(); ++it)
    {
      int                       cIdx=it.value();
      QString                   ifId(it.key());
      QString                   loId("");
 
      if (loIdByIdx_.contains(cIdx))
        loId = loIdByIdx_.value(cIdx);
      else if (loIdByCid_.contains(ifId))
        loId = loIdByCid_.value(ifId);
    
      if (loId.size())
      {
        // LO side band:
        if (!loSideBandById_.contains(loId))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find side band for LO \"" + loId + "\"");    
        };
        // channel side band:
        if (!ifSideBandById_.contains(ifId))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find side band for channel #" + QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");
        };

        // polarization:
        if (!loPolarizationById_.contains(loId))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find polarization for LO \"" + loId + "\"");
        };

        // bandwidth:
        if (!bbcBandwidthByIdx_.contains(cIdx))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find bandwidth for channel #" + QString("").setNum(cIdx) + ", ID=" + ifId);
        };

        if (!bbcFreqByIdx_.contains(cIdx))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find BBC_FRQ for channel #" + QString("").setNum(cIdx));
        };
        if (!loFreqById_.contains(loId))
        {
          isOk_ = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
            "cannot find LO frq for channel #" + QString("").setNum(cIdx) + ", loID=" + loId);
        };
      }
      else
      {
        isOk_ = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::selfCheck(): the channel setup for station \"" + stnKey + "\" is not usable: "
          "cannot find LO ID for channel #" + QString("").setNum(cIdx) + " with id \"" + ifId + "\"");
      };
    };
  };

  return isOk_;
};
/*=====================================================================================================*/









/*=======================================================================================================
*
*                           SgChannelSkeded's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgChannelSkeded::className()
{
  return "SgChannelSkeded";
};



//
bool SgChannelSkeded::parseSkdFile(const QString& fileName)
{
  QString                       str("");
  QFile                         f(fileName);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::parseSkdFile(): going to parse \"" + f.fileName() + "\" sked file");
  
  if (!f.exists()) // it is ok
  {
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      "::parseSkdFile(): the file " + f.fileName() + " does not exists");
    return (isOk_=false);
  };

  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    bool                        haveDone=false;
    int                         numOfReadStrs=0;
    QRegExp                     reCodes("^\\$CODES", Qt::CaseInsensitive);
    QRegExp                     reFrec("^F\\s+(\\S+)\\s+(\\S+)\\s+(.+)$", Qt::CaseInsensitive);
    //C SX X  8445.75  10000.0    2 Mk341:1    8.00 1(1,,2)  4
    QRegExp                     reCrec("^C\\s+(\\w+)\\s+(\\w+)\\s+(.+)$", Qt::CaseInsensitive);
    QRegExp                     reSb("\\w*\\s*\\(([-+0-9,]+)\\).*", Qt::CaseInsensitive);
    bool                        getCodesRec=false;
    QList<QString>              lst;
    QList<QString>              stations;
    double                      frq, width;
    int                         cIdx;
    bool                        isOk;
    bool                        hasUsb, hasLsb, hasBothBands;
    QString                     bandKey("");

    while (!s.atEnd() && !haveDone)
    {
      str = s.readLine();
      numOfReadStrs++;
      
      if (str.size() && str.at(0)== "*")
      {
      }
      else if (reCodes.indexIn(str) != -1)
        getCodesRec = true;
      else if (getCodesRec)
      {
        if (reFrec.indexIn(str) != -1)
        {
          stations.clear();
          lst = reFrec.cap(3).split(" ", QString::SkipEmptyParts);
          for (int i=0; i<lst.size(); i++)
            stations << lst.at(i).leftJustified(8, ' ');
          if (!stations.size())
            logger->write(SgLogger::ERR, SgLogger::IO, className() + 
              "::parseSkdFile(): got \"F\" record without stations: \"" + str + "\"");
          else
          {
            for (int i=0; i<stations.size(); i++)
            {
              const QString&      key=stations.at(i);
              stnCfgByKey_[key] = StnCfg(key);
            };
            logger->write(SgLogger::DBG, SgLogger::IO, className() + 
              "::parseSkdFile(): got " + QString("").setNum(stations.size()) + " stations from F record");
          };
        }
        else if (reCrec.indexIn(str) != -1)
        {
          // parse C rec:
          lst = reCrec.cap(0).split(" ", QString::SkipEmptyParts);

//        for (int i=0; i<lst.size(); i++)

          bandKey = lst.at(2);
          // Sky frequency:
          frq = lst.at(3).toDouble(&isOk);
          if (!isOk)
          {
            frq = -1.0;
            logger->write(SgLogger::ERR, SgLogger::IO, className() + 
              "::parseSkdFile(): cannot get sky frequency from C record \"" + str + "\"");
          };
          // channel index:
          cIdx = lst.at(5).toDouble(&isOk);
          if (!isOk)
          {
            cIdx = -1;
            logger->write(SgLogger::ERR, SgLogger::IO, className() + 
              "::parseSkdFile(): cannot get channel index from C record \"" + str + "\"");
          };
          // channel width:
          width = lst.at(7).toDouble(&isOk);
          if (!isOk)
          {
            width = -1.0;
            logger->write(SgLogger::ERR, SgLogger::IO, className() + 
              "::parseSkdFile(): cannot get channel width from C record \"" + str + "\"");
          };
          // upper/lower side band:
          hasUsb = hasLsb = hasBothBands = false;
          QList<SgChannelSideBand>sideBands;
          if (reSb.indexIn(lst.at(8)) != -1)
          {
            lst = reSb.cap(1).split(",", QString::KeepEmptyParts);
            if (!lst.size())
              logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                "::parseSkdFile(): nothing to parse side band set up: \"" + str + "\"");
            else
            {
              if (lst.at(0).size())
              {
                sideBands << CSB_USB;
                hasUsb = true;
              };
              if (1 < lst.size() && lst.at(1).size())
              {
                sideBands << CSB_LSB;
                hasLsb = true;
              };
              if (2 < lst.size() && lst.at(2).size())
              {
                sideBands << CSB_USB;
                hasUsb = true;
              };
              if (3 < lst.size() && lst.at(3).size())
              {
                sideBands << CSB_LSB;
                hasLsb = true;
              };
              hasBothBands = hasUsb && hasLsb;
            };
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO, className() + 
              "::parseSkdFile(): cannot get parse side band set up: \"" + str + "\"");
          //
          // crutches:
          if (hasBothBands)
          {
            ChanCfg               cc(bandKey, cIdx, frq - width, width, sideBands);
            for (int i=0; i<stations.size(); i++)
              stnCfgByKey_[stations.at(i)].channelCfgs().append(ChanCfg(cc));
          };
          ChanCfg                 cc(bandKey, cIdx, frq, width, sideBands);
          for (int i=0; i<stations.size(); i++)
            stnCfgByKey_[stations.at(i)].channelCfgs().append(ChanCfg(cc));
        }
        else
          haveDone = true;
      };
    };
  };
  f.close();
  //
  // check:
  if (!stnCfgByKey_.size())
  {
    isOk_ = false;
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::parseSkdFile(): nothing has found in the sked file \"" + f.fileName() + "\"");
    return isOk_;
  };
  //
  //
  isOk_ = true;
  for (QMap<QString, StnCfg>::iterator it=stnCfgByKey_.begin(); it!=stnCfgByKey_.end(); ++it)
  {
    const StnCfg&               cfg=it.value();
    if (isOk_ && cfg.getStnKey().size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::parseSkdFile(): the sked file \"" + f.fileName() + 
        "\" is not usable: station name is missing");
    }
    if (isOk_ && cfg.getChannelCfgs().size() == 0)
    {
      isOk_ = false;
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
        ": no channels setup");
    }
    else
    {
      for (int i=0; i<cfg.getChannelCfgs().size(); i++)
      {
        const ChanCfg&          cc=cfg.getChannelCfgs().at(i);
        if (isOk_ && cc.getChanIdx() < 0)
        {
          isOk_ = false;
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
            ": bad channel index for chan#" + QString("").setNum(i));
        }
        if (isOk_ && cc.getBandKey().size() < 0)
        {
          isOk_ = false;
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
            ": bad band key for chan#" + QString("").setNum(i));
        }
        if (isOk_ && cc.getSkyFrq() < 0.0)
        {
          isOk_ = false;
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
            ": bad sky frequency for chan#" + QString("").setNum(i));
        }
        if (isOk_ && cc.getBandWidth() < 0.0)
        {
          isOk_ = false;
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
            ": bad bandwidth for chan#" + QString("").setNum(i));
        }
        if (isOk_ && cc.getSideBands().size() < 0)
        {
          isOk_ = false;
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::parseSkdFile(): the sked file \"" + f.fileName() + "\" is not usable for " + it.key() + 
            ": no sideband setup for chan#" + QString("").setNum(i));
        };
      };
    };
  };
  return isOk_;
};
/*=====================================================================================================*/










/*=======================================================================================================
*
*                           SgStnLogReadings's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgStnLogReadings::className()
{
  return "SgStnLogReadings";
};



//
SgStnLogReadings::SgStnLogReadings() :
  stationName_(""),
  experimentName_(""),
  fsVersionLabel_(""),
  backEndInfo_(""),
  recorderInfo_(""),
  onSourceRecords_(),
  cableCals_(),
  cableLongs_(),
  meteoPars_(),
  tSyses_(),
  dbbc3Tps_(),
  trakls_(),
  pcals_(),
  dot2xpses_(),
  recordByScan_(),
  channelSetup_()
{
  itType_= IT_UNDEF;
  areMeteosPresent_ = areCableCalsPresent_ = areTsysesPresent_ = false;
  tSensorsPrinted2AntCal_ = false;
  cableCalAmbig_ = 100.0e-9;
  cableSign_ = 0;
  contentType_ = CT_UNDEF;
};



//
SgStnLogReadings::SgStnLogReadings(const QString& stationName) :
  stationName_(stationName),
  experimentName_(""),
  fsVersionLabel_(""),
  backEndInfo_(""),
  recorderInfo_(""),
  onSourceRecords_(),
  cableCals_(),
  cableLongs_(),
  meteoPars_(),
  tSyses_(),
  dbbc3Tps_(),
  trakls_(),
  pcals_(),
  dot2xpses_(),
  recordByScan_()
{
  areMeteosPresent_ = areCableCalsPresent_ = areTsysesPresent_ = false;
  tSensorsPrinted2AntCal_ = false;
  cableCalAmbig_ = 100.0e-9;
  cableSign_ = 0;
  contentType_ = CT_UNDEF;
};



//
SgStnLogReadings::~SgStnLogReadings()
{
  // free the used memory:
  onSourceRecords_.clear();
  //
  for (int i=0; i<cableCals_.size(); i++)
    delete cableCals_[i];
  for (int i=0; i<cableLongs_.size(); i++)
    delete cableLongs_[i];
  for (int i=0; i<meteoPars_.size(); i++)
    delete meteoPars_[i];
  for (int i=0; i<tSyses_.size(); i++)
    delete tSyses_[i];
  for (int i=0; i<dbbc3Tps_.size(); i++)
    delete dbbc3Tps_[i];
  for (int i=0; i<trakls_.size(); i++)
    delete trakls_[i];
  for (int i=0; i<pcals_.size(); i++)
    delete pcals_[i];
//  for (int i=0; i<sefds_.size(); i++)
//    delete sefds_[i];
  for (int i=0; i<dot2xpses_.size(); i++)
    delete dot2xpses_[i];

  cableCals_.clear();
  cableLongs_.clear();
  meteoPars_.clear();
  tSyses_.clear();
  dbbc3Tps_.clear();
  trakls_.clear();
  pcals_.clear();
  sefds_.clear();
  dot2xpses_.clear();
  recordByScan_.clear();
};



//
void SgStnLogReadings::removeCableCalReadings()
{
  for (int i=0; i<onSourceRecords_.size(); i++)
  {
    if (onSourceRecords_.at(i)->cableCals().size())
      onSourceRecords_.at(i)->cableCals().clear();
  };
  for (int i=0; i<cableCals_.size(); i++)
    delete cableCals_[i];
  cableCals_.clear();
  areCableCalsPresent_ = false;
};



//
void SgStnLogReadings::removeMeteoReadings()
{
  for (int i=0; i<onSourceRecords_.size(); i++)
    if (onSourceRecords_.at(i)->meteoPars().size())
      onSourceRecords_.at(i)->meteoPars().clear();
  for (int i=0; i<meteoPars_.size(); i++)
    delete meteoPars_[i];
  meteoPars_.clear();
  areMeteosPresent_ = false;
};



//
void SgStnLogReadings::removeTsysReadings()
{
  for (int i=0; i<tSyses_.size(); i++)
    delete tSyses_[i];
  tSyses_.clear();
  areTsysesPresent_ = false;
};



//
#ifndef SEPARATED_LOG2ANT
void SgStnLogReadings::verifyReadings(SgVlbiStationInfo* stn, 
  const QMap<QString, int> *defaultCableSignByStn)
{
  bool                          hasCableSign;
  SgMJD                         tFirst, tLast;
  SgMJD                         tLeftMarging, tRightMarging;

  if (!stationName_.size())
    stationName_ = stn->name(); // simplified name, no spaces

  //
  if (!stn->auxObservationByScanId()->size()) // nothing to worry about
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::verifyReadings(): the station " + stationName_ + " has no any observations; skipped");
    return;
  };
  //
  tFirst = *stn->auxObservationByScanId()->begin().value();
  tLast  = *(--stn->auxObservationByScanId()->end()).value();

  tLeftMarging  = tFirst - 1.0/24.0; // one hr earlier
  tRightMarging = tLast  + 1.0/24.0; // one hr later

  hasCableSign = cableSign_ != 0;

  if (stationName_ == "KOKEE")
    cableCalAmbig_ = 1.0e-9;

  for (int i=0; i<onSourceRecords_.size(); i++)
  {
    SgOnSourceRecord           *osRec=onSourceRecords_.at(i);
    // if the scan name is missed, try to pick it up:
    if (!osRec->getScanName().size())
      lookupScanName(osRec, *stn->auxObservationByScanId());
    // if the scan name is ok (it is from the log file or picked up):
    if (osRec->getScanName().size() && 
        tLeftMarging <= osRec->getTstart() && osRec->getTfinis() <= tRightMarging)
    {
      SgOnSourceRecord         *r=NULL;
      if (!recordByScan_.contains(osRec->getScanName()))
        recordByScan_.insert(osRec->getScanName(), osRec);
      else
      {
        r = recordByScan_.value(osRec->getScanName());
        /*
        if (r->cableCals().size()==0 && r->meteoPars().size()==0 &&        // an empty record, replace it
            (osRec->cableCals().size()!=0 || osRec->meteoPars().size()!=0) )
        {
          recordByScan_.remove(r->getScanName());
          recordByScan_.insert(osRec->getScanName(), osRec);
          logger->write(SgLogger::INF, SgLogger::IO, className() +
            "::verifyReadings(): station " + stationName_ + ": the empty record for the scan " + 
            r->getScanName() + " [" + r->getTstart().toString() + " -- " + r->getTfinis().toString() + 
            "] @ " + r->getSourceName() + " has been replaced by the new one");
          logger->write(SgLogger::INF, SgLogger::IO, className() +
            "::verifyReadings(): station " + stationName_ + ": the new record is: [" + 
            osRec->getTstart().toString() + " -- " + osRec->getTfinis().toString() + "] @ " + 
            osRec->getSourceName() + " numRecs(c/m)=" + 
            QString("").setNum(osRec->cableCals().size()) + "/" + 
            QString("").setNum(osRec->meteoPars().size()));
          //delete r;
        }
        else */
        if (*r == *osRec)
        {
          logger->write(SgLogger::INF, SgLogger::IO, className() +
            "::verifyReadings(): station " + stationName_ + ": got the duplicate scan \"" + 
            osRec->getScanName() + "\" [" + osRec->getTstart().toString() + " -- " +
            osRec->getTfinis().toString() + "] @ " + osRec->getSourceName());
        }
        else
        {
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::verifyReadings(): station " + stationName_ + 
            ": cannot insert osRec [" + osRec->getTstart().toString(SgMJD::F_Simple) + 
            " -- " + osRec->getTfinis().toString(SgMJD::F_Simple) + "] @ " + osRec->getSourceName() + 
            " scan " + osRec->getScanName() + ", it's already in the map, nr(c/m)=" +
            QString("").setNum(osRec->cableCals().size()) + "/" + 
            QString("").setNum(osRec->meteoPars().size()));

          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::verifyReadings(): station " + stationName_ + 
            ": content of the map  [" + r->getTstart().toString(SgMJD::F_Simple) + 
            " -- " + r->getTfinis().toString(SgMJD::F_Simple) + "] @ " + r->getSourceName() +
            ", nr(c/m)=" +
            QString("").setNum(osRec->cableCals().size()) + "/" + 
            QString("").setNum(osRec->meteoPars().size()));
        };
      };
    };
  };
  //
  int                           inRangePts;
  inRangePts = 0;
  for (int i=0; i<meteoPars_.size(); i++)
    if (tFirst<=meteoPars_.at(i)->getT() && meteoPars_.at(i)->getT()<=tLast)
    {
      areMeteosPresent_ = true;
      inRangePts++;
    };
  // check for number acquired of data:
  if (inRangePts<4)
  {
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": insufficient number (" + 
      QString("").setNum(inRangePts) + ") of meteo parameters; skipped");
    removeMeteoReadings();
    areMeteosPresent_ = false;
  };
  //
  // cable cals:
  inRangePts = 0;
  for (int i=0; i<cableCals_.size(); i++)
    if (tFirst<=cableCals_.at(i)->getT() && cableCals_.at(i)->getT()<=tLast)
    {
      areCableCalsPresent_ = true;
      inRangePts++;
    };
  // check for number acquired of data:
  if (inRangePts<4)
  {
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": insufficient number (" + 
      QString("").setNum(inRangePts) + ") of cable calibrations; skipped");
    removeCableCalReadings();
    areCableCalsPresent_ = false;
  };
  //
  // tsyses:
  inRangePts = 0;
  for (int i=0; i<tSyses_.size(); i++)
    if (tFirst<=tSyses_.at(i)->getT() && tSyses_.at(i)->getT()<=tLast)
    {
      areTsysesPresent_ = true;
      inRangePts++;
    };
  // check for number acquired of data:
  if (inRangePts<4)
  {
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": insufficient number (" + 
      QString("").setNum(inRangePts) + ") of tsys readings; skipped");
    removeTsysReadings();
    areTsysesPresent_ = false;
  };
  //
  if (areCableCalsPresent_)
  {
    if (cableLongs_.size())   // try to figure out the cable sign:
    {
      int                         signCableLong;
      if ((signCableLong=determineCableSignFromLong()) == 0)
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::verifyReadings(): station " + stationName_ + 
          ": cannot determine the cable sign from cablelong measurements");
      else if (!hasCableSign)
      {
        cableSign_ = signCableLong;
        logger->write(SgLogger::DBG, SgLogger::IO, className() +
          "::verifyReadings(): station " + stationName_ + ": the cable sign has been set to \"" + 
          (cableSign_>0?"+":"-") + "\" according to cablelong measurements");
        hasCableSign = true;
      }
      else if (signCableLong != cableSign_)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::verifyReadings(): station " + stationName_ + 
          ": the found cable sign is different from the cablelong measurements: \"" +
          (cableSign_>0?"+":"-") + "\" vs \"" + (signCableLong>0?"+":"-") +
          "\"; the sign has been reverted");
        cableSign_ = signCableLong;
      }
      else
        logger->write(SgLogger::DBG, SgLogger::IO, className() +
          "::verifyReadings(): station " + stationName_ + 
          ": the cable sign has been confirmed by cablelong measurements");
    }
    else
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::verifyReadings(): station " + stationName_ +
        ": cannot find any usable cablelong measurements");
  };
  //
  // complain:
  if (!areMeteosPresent_ && (itType_==IT_FSLG || itType_==IT_METE))
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": cannot find any usable meteo readings");
  if (!areCableCalsPresent_ && (itType_==IT_FSLG || itType_!=IT_METE))
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": cannot find any usable cable calibrations");

  if (areCableCalsPresent_ && !hasCableSign)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::verifyReadings(): station " + stationName_ + ": cannot determine the cable calibration sign");
    //
    // predefined station names:
    //
    if (defaultCableSignByStn)
    {
      if (defaultCableSignByStn->contains(stationName_))
      {
        cableSign_ = defaultCableSignByStn->value(stationName_);
        logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::verifyReadings(): station " + stationName_ + 
        ": the cable sign was set to \"" + QString(cableSign_==1?"+":"-") +
        "\" according to the list of default cable signs");
      };

    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::verifyReadings(): no default cable cal list were provided");
  };
  //
  // check ambigs, remove outliers and, then, the average:
  if (areCableCalsPresent_)
    cleanCableCalibrations();
  if (areMeteosPresent_)
    cleanMeteoParameters();
};



//
void SgStnLogReadings::lookupScanName(SgOnSourceRecord* osRec, 
  const QMap<QString, SgVlbiAuxObservation*> &auxObservationByScan)
{
  if (!osRec->getSourceName().size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::lookupScanName(): station " + stationName_ + 
      ": unable to lookup the scan name: no source name provided,");
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::lookupScanName(): station " + stationName_ + ": the on_data record is: " + 
      osRec->getScanName() + " [" + osRec->getTstart().toString(SgMJD::F_Simple) + " -- " + 
      osRec->getTfinis().toString(SgMJD::F_Simple) + "] @ \"" + osRec->getSourceName() + "\"");
    return;
  };
  
  double                        tLimit=15.0/60.0/24.0; // 15min window where the scan can be
  double                        dT;
  const SgMJD                  &t=osRec->getTstart() + (osRec->getTfinis() - osRec->getTstart())/2.0;
  SgVlbiAuxObservation         *auxClosest=auxObservationByScan.begin().value();
  SgMJD                         tClosest=*auxClosest;
  //
  dT = fabs(tClosest - t);
  for (QMap<QString, SgVlbiAuxObservation*>::const_iterator it=auxObservationByScan.begin();
    it!=auxObservationByScan.end(); ++it)
  {
    SgMJD                      &ti=*it.value();
    if ( (fabs(ti-t)<dT) && it.value()->getOwner()->src()->getKey().contains(osRec->getSourceName()))
    {
      tClosest = ti;
      dT = fabs(ti - t);
      auxClosest = it.value();
    };
  };
  
  if (auxClosest && dT<=tLimit) // found something:
  {
    osRec->setScanName(auxClosest->getOwner()->getScanName().simplified().toLower());
/*
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::lookupScanName(): the scan name " + osRec->getScanName() + 
      " has been assigned to the on_data record; source: " +  osRec->getSourceName() +
      ", interval: [" + osRec->getTstart().toString(SgMJD::F_Simple) + 
      " -- " + osRec->getTfinis().toString(SgMJD::F_Simple) + "]");
*/
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::lookupScanName(): station " + stationName_ + 
      ": no scan name was assigned to the on_data record: [" + 
      osRec->getTstart().toString(SgMJD::F_Simple) + " -- " + 
      osRec->getTfinis().toString(SgMJD::F_Simple) + "] @ " + 
      osRec->getSourceName());
};
#endif



//
int SgStnLogReadings::determineCableSignFromLong()
{
  int                           cableSign=0;
  double                        window=10.0/60.0/24.0; // 10 mins, is it ok?
  QList<int>                    signByCablelong;
  QList<SgCableCalReading*>     selectedReadings;
  for (int i=0; i<cableLongs_.size(); i++)
  {
    const SgMJD                &tL=cableLongs_.at(i)->getT();
    double                      clV=cableLongs_.at(i)->getV();
    for (int j=0; j<cableCals_.size(); j++)
      if (fabs(cableCals_.at(j)->getT() - tL) < window)
        selectedReadings << cableCals_.at(j);
    //
    if (selectedReadings.size())
    {
      double                    vAverage, ratio;
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::determineCableSignFromLong(): collected " + QString("").setNum(selectedReadings.size()) + 
        " close cable readings for cablelong measurement on " + tL.toString());
      // find average cable measurement:
      vAverage = 0.0;
      for (int j=0; j<selectedReadings.size(); j++)
        vAverage += selectedReadings.at(j)->getV();
      vAverage /= selectedReadings.size();
      //
      ratio = (clV - vAverage)/vAverage;
      if (1.0e-3 < ratio)
        cableSign = 1;
      else if (ratio < -1.0e-3 )
        cableSign = -1;
      else
        cableSign = 0;
      signByCablelong << cableSign;
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::determineCableSignFromLong(): average cable cal: " + QString("").setNum(vAverage*1.0e9) +
        "ns, cablelong: " + QString("").setNum(clV*1.0e9) +
        "ns; the sign is \"" + QString(cableSign==0?"?":(cableSign<0?"-":"+")) + "\"");
      selectedReadings.clear();
    }
    else
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::determineCableSignFromLong(): no close cable readings found for cablelong measurement on " +
        tL.toString());
  };

  if (signByCablelong.size() == 0)
    cableSign = 0;
  else if (signByCablelong.size() == 1)
    cableSign = signByCablelong.at(0);
  else
  {
    QMap<int, int>              signByNum;
    for (int i=0; i<signByCablelong.size(); i++)
    {
      if (signByNum.contains(signByCablelong.at(i)))
        signByNum[signByCablelong.at(i)]++;
      else
        signByNum[signByCablelong.at(i)] = 1;
    };
    if (signByNum.size() == 1)
    {
      cableSign = signByNum.begin().key();
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::determineCableSignFromLong(): according to all " +
        QString("").setNum(signByCablelong.size()) + " cablelong measurements, the sign is \"" + 
        QString(cableSign==0?"?":(cableSign<0?"-":"+")) + "\"");
    }
    else
    {
      cableSign = 0;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::determineCableSignFromLong(): cannot determine the proper sign, the "
        "numbers of measuremnts are:");
      for (QMap<int, int>::iterator it=signByNum.begin(); it!=signByNum.end(); ++it)
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::determineCableSignFromLong():   " + 
          QString("").sprintf("sign \"%s\": %2d count(s)", it.key()==0?"?":(it.key()<0?"-":"+"), 
            it.value()));
    };
  };
  signByCablelong.clear();
  return cableSign;
};



//
void SgStnLogReadings::cleanCableCalibrations()
{
  double                        vAverage;
  double                        vDispersion;
  double                        vTmp;
  SgCableCalReading            *rec=NULL;
  int                           numOfResolvedAmbigs=0;
  int                           numOfOutliers=0;
  bool                          allReadingsRsame;
  //
  // first, resolve ambiguities:
  vAverage = 0.0;
  for (int i=0; i<cableCals_.size(); i++)
  {
    rec = cableCals_.at(i);
    if (fabs((rec->getV()-vAverage)/cableCalAmbig_) > 0.75)
    {
      rec->setV(rec->getV() - round((rec->getV()-vAverage)/cableCalAmbig_)*cableCalAmbig_);
      numOfResolvedAmbigs++;
    };
    vAverage = vAverage*i/(i + 1) + rec->getV()/(i+1);
  };
  if (numOfResolvedAmbigs)
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::cleanCableCalibrations() station " + stationName_ + ": " + 
      QString("").setNum(numOfResolvedAmbigs) + " ambiguit" + (numOfResolvedAmbigs==1?"y":"ies") + 
      " resolved in the cable cal readings");

  //
  // check for outliers:
  vAverage = 0.0;
  vDispersion = 0.0;
  vTmp = 0.0;
  if (cableCals_.size() > 0)
    vTmp = cableCals_.at(0)->getV();
  allReadingsRsame = true;
  for (int i=0; i<cableCals_.size(); i++)
  {
    vAverage += cableCals_.at(i)->getV();
    vDispersion += cableCals_.at(i)->getV()*cableCals_.at(i)->getV();
    if (allReadingsRsame && cableCals_.at(i)->getV()!=vTmp)
      allReadingsRsame = false;
  };
  if (!allReadingsRsame)
  {
    vAverage /= cableCals_.size();
    vDispersion /= cableCals_.size();
    vDispersion -= vAverage*vAverage;
    if (vDispersion < 0.0)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::cleanCableCalibrations(): station " + stationName_ + ": dispersion^2<0.0 (" + 
        QString("").sprintf("%.16g", vDispersion) + ")");
      vDispersion = -vDispersion;
    };
    if (vDispersion > 0.0)
    {
      vDispersion = sqrt(vDispersion);
      for (int i=0; i<cableCals_.size(); i++)
        if (fabs(cableCals_.at(i)->getV()-vAverage) > 5.0*vDispersion)
        {
          cableCals_[i]->setIsOk(false);
          numOfOutliers++;
        };
    };
    // and adjust the data structures:
    if (numOfOutliers)
    {
      int                         j;
      for (int i=0; i<onSourceRecords_.size(); i++)
      {
        SgOnSourceRecord       *rec=onSourceRecords_[i];
        QList<SgCableCalReading*>&cl=rec->cableCals();
        j = 0;
        while (j < cl.size())
          if (!cl.at(j)->getIsOk())
            cl.removeAt(j);
          else
            j++;
      };
      j = 0;
      while (j < cableCals_.size())
        if (!cableCals_.at(j)->getIsOk())
        {
          delete cableCals_[j];
          cableCals_[j] = NULL;
          cableCals_.removeAt(j);
        }
        else
          j++;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::cleanCableCalibrations(): station " + stationName_ + ": " + 
        QString("").setNum(numOfOutliers) + " outlier" + 
        QString(numOfOutliers==1?" has ":"s have ") + "been removed", true);
    };
  };
  //
  // remove average:
  vAverage = 0.0;
  for (int i=0; i<cableCals_.size(); i++)
    vAverage += cableCals_.at(i)->getV();
  vAverage /= cableCals_.size();
  for (int i=0; i<cableCals_.size(); i++)
    cableCals_.at(i)->setV(cableCals_.at(i)->getV() - vAverage);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::cleanCableCalibrations(): station " + stationName_ + ": subtracted the average value of " + 
    QString("").setNum(vAverage*1.0e9) + "ns from the cable cal readings");
};



//
#ifndef SEPARATED_LOG2ANT
void SgStnLogReadings::cleanMeteoParameters()
{
  // first, make sure the parameters are in time order:
  QMap<QString, SgMeteoReading*>
                                mpByEpoch;
  QString                       str("");
  double                        d, dt, extrRmin=0.0, extrRmax=1.0;
  int                           nExtr4Rmin=0, nExtr4Rmax=0, numOfDuples=0, numOfSameEpochs=0;
  SgMJD                         t;

  // first, check meteo parameters:
  for (int i=0; i<meteoPars_.size(); i++)
  {
    t = meteoPars_.at(i)->getT();
    str = t.toString(SgMJD::F_INTERNAL);
    //
    // check for duplicates:
    if (mpByEpoch.contains(str))
    {
      if (*mpByEpoch[str] == *meteoPars_.at(i))
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::cleanMeteoParameters(): station " + stationName_ + ": got duplicate weather record at " +
          t.toString(SgMJD::F_Simple));
        numOfDuples++;
      }
      else
      {
        logger->write(SgLogger::ERR, SgLogger::IO, className() + "::cleanMeteoParameters(): station " +
          stationName_ + ": got two different records at the same epoch " + 
          t.toString(SgMJD::F_Simple));
        numOfSameEpochs++;
      };
      delete meteoPars_.at(i);
    }
    else
      mpByEpoch.insert(str, meteoPars_.at(i));
  };
  //
  if (numOfDuples)
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + ": " + QString("").setNum(numOfDuples) +
      " duplicate meteo records found");
  if (numOfSameEpochs)
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + ": " + QString("").setNum(numOfSameEpochs) +
      " records with the same epoch and different values found");
  //
  //
//  for (int i=0; i<meteoPars_.size(); i++)
//    delete meteoPars_.at(i);
  meteoPars_.clear();
  meteoPars_ = mpByEpoch.values();
  mpByEpoch.clear();
  //
  if (meteoPars_.size() <= 2)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + ": too small number of meteo records (" + 
      QString("").setNum(meteoPars_.size()) + "), nothing to do");
    return;
  };
  //
  //
  int                           nPars(meteoPars_.size());
  SgVector                      dP(nPars), dT(nPars), dR(nPars);
  t = meteoPars_.at(0)->getT();
  for (int i=1; i<nPars; i++)
  {
    t = meteoPars_.at(i)->getT();
    //
    // calculate reates:
    dt = t - meteoPars_.at(i-1)->getT();
    d = meteoPars_.at(i)->getM().getPressure() - meteoPars_.at(i-1)->getM().getPressure();
    if (0.15 < fabs(d)) // current format has one digit after dot:
      dP.setElement(i, d/dt);
    
    d = meteoPars_.at(i)->getM().getTemperature() - meteoPars_.at(i-1)->getM().getTemperature();
    if (0.15 < fabs(d)) // current format has one digit after dot:
      dT.setElement(i, d/dt);
    
    d = meteoPars_.at(i)->getM().getRelativeHumidity() - meteoPars_.at(i-1)->getM().getRelativeHumidity();
    if (0.15 < fabs(d)) // current format has one digit after dot:
      dR.setElement(i, d/dt);
    //
    // check for extremums:
    if (fabs(meteoPars_.at(i)->getM().getRelativeHumidity() - extrRmin) < 1.0e-6)
      nExtr4Rmin++;
    if (fabs(meteoPars_.at(i)->getM().getRelativeHumidity() - extrRmax) < 1.0e-6)
      nExtr4Rmax++;
    //
  };

  //
  double                        mrP, mrT, mrR;
  int                           n4P=0, n4T=0, n4R=0;
  bool                          isOkP=true, isOkT=true, isOkR=true, hasSuspiciousExtrems4R=false;
  // max allowable rates:
  mrP =100.0*24.0;    // 100 mbar per hour
  mrT =200.0*24.0;    // 200 degree per hour
  mrR = 10.0*24.0;    // 1000% per hour
  for (int i=1; i<nPars; i++)
  {
    if (mrP < fabs(dP.getElement(i)))
    {
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::cleanMeteoParameters(): station " + stationName_ + 
        " has too big pressure rate, " + QString("").setNum(dP.getElement(i)/24.0) + 
        "(mbar/hr) at the idx " + QString("").setNum(i));
      n4P++;
    };
    if (mrT < fabs(dT.getElement(i)))
    {
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::cleanMeteoParameters(): station " + stationName_ + 
        " has too big atm.temperature rate, " + QString("").setNum(dT.getElement(i)/24.0) + 
        "(C/hr) at the idx " + QString("").setNum(i));
      n4T++;
    };
    if (mrR < fabs(dR.getElement(i)))
    {
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::cleanMeteoParameters(): station " + stationName_ + 
        " has too big rel.humidity rate, " + QString("").setNum(100.0*dR.getElement(i)/24.0) + 
        "(%/hr) at the idx " + QString("").setNum(i));
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::cleanMeteoParameters(): station " + stationName_ + ": i-1: " + 
          meteoPars_.at(i-1)->getT().toString(SgMJD::F_Simple) + " " + 
          QString("").sprintf("%.4f", meteoPars_.at(i-1)->getM().getRelativeHumidity()) +
          ";  i: " + meteoPars_.at(i)->getT().toString(SgMJD::F_Simple) + " " + 
          QString("").sprintf("%.4f", meteoPars_.at(i)->getM().getRelativeHumidity()));

      n4R++;
    };
  };
  if (n4P>10)
  {
    isOkP = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + " has more than 10 (" + 
      QString("").setNum(n4P) + ") suspicious big pressure rates, use standard pressure instead");
  };
  if (n4T>10)
  {
    isOkT = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + " has more than 10 (" + 
      QString("").setNum(n4T) + 
      ") suspicious big atm.temperature rates, use default temperature instead");
  };
  if (n4R>10)
  {
    isOkR = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + " has more than 10 (" + 
      QString("").setNum(n4R) + 
      ") suspicious big rel.humidity rates, use default humidity instead");
  };
  if (8 < nExtr4Rmin && 8 < nExtr4Rmax)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::cleanMeteoParameters(): station " + stationName_ + " has too much extremal values (" + 
      QString("").setNum(nExtr4Rmin) + "/" + QString("").setNum(nExtr4Rmax) +
      ") for rel.humidity, use default humidity instead");
    hasSuspiciousExtrems4R = true;
  };
  //
  //
  // if one of readings is wrong, set it to constant:
  if (!isOkP || !isOkT || !isOkR || hasSuspiciousExtrems4R)
  {
    SgMeteoData                 m;
    for (int i=0; i<meteoPars_.size(); i++)
    {
      m = meteoPars_.at(i)->getM();
      if (!isOkP)
        m.setPressure(1013.25);
      if (!isOkT)
        m.setTemperature(10.0);
      if (!isOkR || hasSuspiciousExtrems4R)
        m.setRelativeHumidity(0.5);
      meteoPars_.at(i)->setM(m);
    };
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::cleanMeteoParameters(): station " + stationName_ + ": the meteoparameters have been ordered");
};



//
void SgStnLogReadings::interpolateMeteoParameters(SgVlbiAuxObservation* aux)
{
  SgMJD                        &t=*aux;
  int                           idx_r=0;
  int                           idx_l=meteoPars_.size() - 1;
  //
  // limit the extrapolation:
  if (600.0/DAY2SEC < meteoPars_.at(idx_r)->getT() - t)       // 10 mins
  {
    aux->setMeteoData(meteoPars_.at(idx_r)->getM());
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::interpolateMeteoParameters(): station " + stationName_ + ": epoch " + t.toString() + 
      " is too far from the first available meteo reading (" + meteoPars_.at(idx_r)->getT().toString() +
      "), no extrapolation");
    return;
  };
  if (600.0/DAY2SEC < t - meteoPars_.at(idx_l)->getT())       // 10 mins
  {
    aux->setMeteoData(meteoPars_.at(idx_l)->getM());
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::interpolateMeteoParameters(): station " + stationName_ + ": epoch " + t.toString() + 
      " is too far from the last available meteo reading (" + meteoPars_.at(idx_l)->getT().toString() +
      "), no extrapolation");
    return;
  };
  //
  // assume that first and last measurements are ok
  if (t < meteoPars_.at(idx_r)->getT())
    idx_l = idx_r++;
  else if (meteoPars_.at(idx_l)->getT() < t)
    idx_r = idx_l--;
  else
  {
    while (meteoPars_.at(idx_r)->getT()<t && idx_r<meteoPars_.size()-1)
      idx_r++;
    while (t<meteoPars_.at(idx_l)->getT() && 0<idx_l)
      idx_l--;
  };
  //
  //
  if (idx_l == idx_r)
    aux->setMeteoData(meteoPars_.at(idx_r)->getM());
  else
  {
    double                      dt, dPr, dTp, dRh, x;
    dt  = meteoPars_.at(idx_r)->getT() - meteoPars_.at(idx_l)->getT();
    x   = t - meteoPars_.at(idx_l)->getT();
    dPr = meteoPars_.at(idx_r)->getM().getPressure() - 
      meteoPars_.at(idx_l)->getM().getPressure();
    dTp = meteoPars_.at(idx_r)->getM().getTemperature() - 
      meteoPars_.at(idx_l)->getM().getTemperature();
    dRh = meteoPars_.at(idx_r)->getM().getRelativeHumidity() - 
      meteoPars_.at(idx_l)->getM().getRelativeHumidity();
    aux->meteoData().setPressure(meteoPars_.at(idx_l)->getM().getPressure() + dPr/dt*x);
    aux->meteoData().setTemperature(meteoPars_.at(idx_l)->getM().getTemperature() + dTp/dt*x);
    aux->meteoData().setRelativeHumidity(meteoPars_.at(idx_l)->getM().getRelativeHumidity() + dRh/dt*x);
  };
};



//
bool SgStnLogReadings::propagateData(SgVlbiStationInfo* stn)
{
  QRegExp                       reBrokenScanName("([a-zA-Z0-9+-]+)_.+", Qt::CaseInsensitive);
  bool                          isScanNameSuspicious;
  //
  int                           numOfExactMatches, numOfMultExactMatches, numOfClosestMatches, cSign;
  int                           numOfChannels, chanIdx, ccIdx;
  bool                          hasMeteoOk, hasCableOk, hasTsysOk;
  SgMJD                         t(tZero);
  SgVector                     *tS;
  numOfExactMatches = numOfMultExactMatches = numOfClosestMatches = 0;
  hasMeteoOk = hasCableOk = hasTsysOk = false;
  static const QString          sideBand2str[] = {"UNDEF", "LSB", "USB", "DUAL"};
  static const QString          polarization2str[] = {"UNDEF", "RCP", "LCP", "HLP", "VLP"};
  
  cSign = cableSign_;
  if (cSign==0)
    cSign = 1; // no cable sign found

  ccIdx = SgVlbiAuxObservation::CCT_FSLG;   // idx => FS log
  if (itType_ == IT_CDMS)
    ccIdx = SgVlbiAuxObservation::CCT_CDMS; // idx => CDMS
  else if (itType_ == IT_PCMT)
    ccIdx = SgVlbiAuxObservation::CCT_PCMT; // idx => PCMT
  // adjust the index if CDMS data are from FS log file
  if (ccIdx == SgVlbiAuxObservation::CCT_FSLG && stn->isAttr(SgVlbiStationInfo::Attr_CABLE_CAL_IS_CDMS))
    ccIdx = SgVlbiAuxObservation::CCT_CDMS; // idx => CDMS too

  if (stn->isAttr(SgVlbiStationInfo::Attr_CABLE_CAL_IS_CDMS) && cSign < 0)
  {
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::propagateData(): station " + stationName_ + 
      ": the negative cable sign extracted from the log file is not applicable for the CDMS data, "
      "the sign was corrected");
    cSign = 1;
  };

  //
  numOfChannels = channelSetup_.ifIdByTpSensorKey().size();
  tS = new SgVector(numOfChannels);
  //
  if (areMeteosPresent_ || areCableCalsPresent_ || areTsysesPresent_)
    for (QMap<QString, SgVlbiAuxObservation*>::iterator it=stn->auxObservationByScanId()->begin();
      it!=stn->auxObservationByScanId()->end(); ++it)
    {
      SgVlbiAuxObservation       *aux=it.value();
      QString                     scanName=aux->getOwner()->getScanName().simplified().toLower();
      QString                     scanNameAux("");
      QString                     sourceName=aux->getOwner()->src()->getKey();
      QString                     surrogateScanName("");
      int                         idx4Cable=-1;
      QList<SgCableCalReading*>  *cables = &cableCals_;
      double                      dt, f;

      //
      // set up scan start time:
      if (itType_==IT_CDMS || itType_==IT_PCMT)
      {
        if (aux->getOwner()->observableByKey().size())
          t = aux->getOwner()->observableByKey().begin().value()->getTstart();
        else
          t = tZero;
      }
      else
        t = *aux;
      //
      for (int i=0; i<numOfChannels; i++)
        tS->setElement(i, -4.0);
      //
      // workaround a database bug:
      isScanNameSuspicious = reBrokenScanName.indexIn(scanName) != -1;
      if (isScanNameSuspicious)
        scanNameAux = reBrokenScanName.cap(1).toLower();
      if (scanName.size()==9 && scanName.at(8).isLetter())
        surrogateScanName = scanName.left(8);
      //
      // here is a missed scan, e.g. the FS has been restarted:
      if (!recordByScan_.contains(scanName) && !isScanNameSuspicious)
      {
        if (itType_==IT_FSLG)
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::propagateData(): station " + stationName_ + ": cannot find the scan \"" + scanName +
            "\" in the log file");
      }
      else if (isScanNameSuspicious && !recordByScan_.contains(scanNameAux))
      {
        if (itType_==IT_FSLG)
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::propagateData(): station " + stationName_ + ": cannot find the suspicious scan \"" + 
            scanNameAux + "\"' in the log file");
      }
      else if (surrogateScanName.size() && 
              !recordByScan_.contains(surrogateScanName) &&
              !recordByScan_.contains(scanName))
      {
        if (itType_==IT_CDMS)
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::propagateData(): station " + stationName_ + ": cannot find the surrogate scan \"" + 
            scanNameAux + "\"' in the CDMS file");
      }
      else 
      {
        SgOnSourceRecord       *osRec=recordByScan_.value(scanName);
        if (!osRec && isScanNameSuspicious)
          osRec = recordByScan_.value(scanNameAux);
        else if (!osRec && surrogateScanName.size())
          osRec = recordByScan_.value(surrogateScanName);
        //
        if (osRec) // just in case
        {
          //
          // cable calibration:
          if (areCableCalsPresent_)
          {
            if (osRec->cableCals().size())
            {
              cables = &osRec->cableCals();
              numOfExactMatches++;
            }
            else
            {
              logger->write(SgLogger::INF, SgLogger::IO, className() +
                "::propagateData(): station " + stationName_ + 
                ": no cable cal record found for the scan \"" + scanName + "\" in the log file");
              numOfClosestMatches++;
            };
            //
            if (osRec->cableCals().size() == 1)      // only one record with cable cals:
              idx4Cable = 0;
            else if (osRec->cableCals().size() > 1)
            {
              /*
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::propagateData(): station " + stationName_ + ": found more than one (" +
                QString("").setNum(osRec->cableCals().size()) + ") cable cal records for the scan [" + 
                scanName + "] in the log file");
              */
              numOfMultExactMatches++;
            };
            // check source name:
            if (osRec->getSourceName().leftJustified(8, ' ') != sourceName)
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::propagateData(): source name mismatch: from the log: \"" + osRec->getSourceName() + 
                "\",  from the obs: \"" + sourceName + "\"");
          };
          //
          if (areTsysesPresent_ && osRec->tSyses().size())
          {
            SgTsysReading      *tsr=NULL;
            if (osRec->tSyses().size() == 1)
              tsr = osRec->tSyses().last();
            else
            {
              int               idx=0;
              dt = fabs(osRec->tSyses().at(0)->getT() - t);
              for (int i=1; i<osRec->tSyses().size(); i++)
                if ((f=fabs(osRec->tSyses().at(i)->getT() - t)) < dt)
                {
                  idx = i;
                  dt = f;
                };
              tsr = osRec->tSyses().at(idx);
            };
            //
            chanIdx = 0;
            if (tsr)
              for (QMap<QString, QString>::const_iterator jt=channelSetup_.ifIdByTpSensorKey().begin();
                jt!=channelSetup_.ifIdByTpSensorKey().end(); ++jt, chanIdx++)
                if (tsr->getTsys().contains(jt.value()))
                  tS->setElement(chanIdx, tsr->getTsys().value(jt.value()));
            //
            if (channelSetup_.getHwType()!=SgChannelSetup::HT_VGOS &&
                1 < osRec->tSyses().size())
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::propagateData(): station " + stationName_ + 
                ": the OnSource record has more than one (" + 
                QString("").setNum(osRec->tSyses().size()) + ") TSYS record for the scan \"" +
                scanName + "\"");
          };
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::propagateData(): the OnSource record is NULL: the station= " + stationName_ + 
            ",  the scan= \"" + scanName + "\"");
      };
      // set up:
      // meteo:
      if (areMeteosPresent_ && !stn->isAttr(SgVlbiStationInfo::Attr_HAS_METEO))
      {
        interpolateMeteoParameters(aux);
        hasMeteoOk = true;
      };
      // cable calibration:
      if (areCableCalsPresent_ && !stn->isAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL))
      {
        if (idx4Cable == -1)
        {
          dt = fabs(cables->at((idx4Cable=0))->getT() - t);
          for (int i=1; i<cables->size(); i++)
            if ((f=fabs(cables->at(i)->getT() - t)) < dt)
            {
              idx4Cable = i;
              dt = f;
            };
        };
        aux->setCableCalibration(cables->at(idx4Cable)->getV()*cSign);
        aux->cableCorrections().setElement(ccIdx, cables->at(idx4Cable)->getV()*cSign);
        hasCableOk = true;
      };
      // tsys:
      if (areTsysesPresent_ && !stn->isAttr(SgVlbiStationInfo::Attr_HAS_TSYS))
      {
        aux->setTsyses(*tS);
        hasTsysOk = true;
      };
    };
  //
  //
  if (hasCableOk)
  {
    stn->addAttr(SgVlbiStationInfo::Attr_HAS_CABLE_CAL);
    stn->addAttr(SgVlbiStationInfo::Attr_CABLE_CAL_MODIFIED);
    stn->setCableCalMultiplierDBCal(cSign);
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::propagateData(): station " + stationName_ + ": " + QString("").setNum(numOfExactMatches) +
      " exact " + (numOfMultExactMatches?("(" + QString("").setNum(numOfMultExactMatches) + 
        (numOfMultExactMatches==1?" is ":" are ") + "multiple) "):"") +
      (numOfClosestMatches?"and " + QString("").setNum(numOfClosestMatches) + " closest ":"") +
      "matches of cable calibrations were found");
  };
  if (hasMeteoOk)
  {
    stn->addAttr(SgVlbiStationInfo::Attr_HAS_METEO);
    stn->addAttr(SgVlbiStationInfo::Attr_METEO_MODIFIED);
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::propagateData(): station " + stationName_ + ": meteo parameters were found");
  };
  if (hasTsysOk)
  {
    stn->addAttr(SgVlbiStationInfo::Attr_HAS_TSYS);
    stn->addAttr(SgVlbiStationInfo::Attr_TSYS_MODIFIED);
    //set up channel info:
    stn->tsysIfFreqs().resize(numOfChannels);
    stn->tsysIfIds().resize(numOfChannels);
    stn->tsysIfSideBands().resize(numOfChannels);
    stn->tsysIfPolarizations().resize(numOfChannels);
    chanIdx = 0;
    for (QMap<QString, QString>::const_iterator jt=channelSetup_.ifIdByTpSensorKey().begin();
      jt!=channelSetup_.ifIdByTpSensorKey().end(); ++jt, chanIdx++)
    {
      const QString            &id=jt.value();
      const QString            &sensorKey=jt.key();
      //
      stn->tsysIfIds()[chanIdx] = id;
      stn->tsysIfFreqs()[chanIdx] = channelSetup_.ifFreqByTpSensorKey()[sensorKey];
      //
      SgChannelSideBand         sb=channelSetup_.ifSideBandById()[id];
      stn->tsysIfSideBands()[chanIdx] = sideBand2str[sb];
      //
      SgChannelPolarization     pz=channelSetup_.ifPolarizationById()[id];
      stn->tsysIfPolarizations()[chanIdx] = polarization2str[pz];
    };

    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::propagateData(): station " + stationName_ + ": tsys data were found");
  };

  //
  delete tS;
  //
  return hasMeteoOk || hasCableOk || hasTsysOk;
};
#endif



//
void SgStnLogReadings::allocateCableReadings()
{
  int                           allocatedNum=0;
  for (int i=0; i<cableCals_.size(); i++)
  {
    SgCableCalReading          *cbl=cableCals_.at(i);
    for (int j=0; j<onSourceRecords_.size(); j++)
    {
      SgOnSourceRecord         *osRec=onSourceRecords_.at(j);
      if (osRec->getTstart() <= cbl->getT() && cbl->getT() <= osRec->getTfinis())
      {
        osRec->cableCals() << cbl;
        allocatedNum++;
      };
    };
  };
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::allocateCableReadings(): station " + stationName_ + ": " + QString("").setNum(allocatedNum) +
    " cable cal readings were linked to on data records");
};



//
void SgStnLogReadings::export2antabFrqs_nativeSetup(QTextStream& s)
{
  QString                       str("");
  int                           idx;

//s << "#\nNUMB_FRQ: " << channelSetup_.ifIdxById().size() << "\n#\n";
  s << "#\nNUMB_FRQ: " << channelSetup_.ifIdByTpSensorKey().size() << "\n#\n";
  s << "#       Sensor#  IF_Freq     LO_Freq    Sky_freq   Pol  SensorId Channel# SideBand\n#\n";

  idx = 1;
//for (QMap<QString, int>::const_iterator it=channelSetup_.ifIdxById().begin();
//  it!=channelSetup_.ifIdxById().end(); ++it)
  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     ifKey(it.key());
    QString                     ifId(it.value());

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_nativeSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
//  int                         cIdx=it.value();
//  QString                     ifId(it.key());
    QString                     loId("");
    SgChannelSideBand           loSb=CSB_UNDEF;
    SgChannelSideBand           ifSb=CSB_UNDEF;
    SgChannelPolarization       polarization=CP_UNDEF;
    double                      ifFrq, loFrq, skyFrq, bw;
    ifFrq = loFrq = skyFrq = bw = 0.0;

    if (channelSetup_.loIdByIdx().contains(cIdx))
      loId = channelSetup_.loIdByIdx().value(cIdx);
    else if (channelSetup_.loIdByCid().contains(ifId))
      loId = channelSetup_.loIdByCid().value(ifId);
    if (loId.size())
    {
      // LO side band:
      if (channelSetup_.loSideBandById().contains(loId))
        loSb = channelSetup_.loSideBandById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find side band for LO \"" + 
          loId + "\"");

      // channel side band:
      if (channelSetup_.ifSideBandById().contains(ifId))
        ifSb = channelSetup_.ifSideBandById().value(ifId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find side band for channel #" +
          QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

      // polarization:
      if (channelSetup_.loPolarizationById().contains(loId))
        polarization = channelSetup_.loPolarizationById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find polarization for LO \"" +
          loId + "\"");

      // bandwidth:
      if (channelSetup_.bbcBandwidthByIdx().contains(cIdx))
        bw = channelSetup_.bbcBandwidthByIdx().value(cIdx);
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find bandwidth for channel #" +
          QString("").setNum(cIdx) + ", ID=" + ifId);

      if (channelSetup_.bbcFreqByIdx().contains(cIdx))
      {
        ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        //
        if (loSb == CSB_USB && ifSb == CSB_USB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        else if (loSb == CSB_USB && ifSb == CSB_LSB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
        else if (loSb == CSB_LSB && ifSb == CSB_LSB)
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
//        ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx);
        else if (loSb == CSB_LSB && ifSb == CSB_USB)
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx);
//        ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
      }
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find BBC_FRQ for channel #" +
          QString("").setNum(cIdx));

      if (channelSetup_.loFreqById().contains(loId))
        loFrq = channelSetup_.loFreqById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find LO frq for channel #" +
          QString("").setNum(cIdx) + ", loID=" + loId);
    
      skyFrq = loFrq + ifFrq;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_nativeSetup(): station " + stationName_ + ": cannot find LO ID for channel #" +
        QString("").setNum(cIdx));

    channelSetup_.ifFreqByIdx()[cIdx] = skyFrq;
    channelSetup_.ifPolarizationById()[ifId] = polarization;
   
    str.sprintf("FRQ: %7d %11.2f %11.2f %11.2f",
      idx, ifFrq, loFrq, skyFrq);
    s << str << "   "; 
    if (polarization == CP_RightCP)
      s << "  R";
    else if (polarization == CP_LeftCP)
      s << "  L";
    else 
      s << "  ?";

    str.sprintf("  %6s   ", qPrintable(ifId));
    s << str;
    str.sprintf("%6d", cIdx);
    s << str << "   ";
 
    if (ifSb == CSB_USB)
      s << "USB";
    else if (ifSb == CSB_LSB)
      s << "LSB";
    else if (ifSb == CSB_DUAL)
      s << "USB/LSB";
    else 
      s << "UNK";
    s << "\n";
    idx++;
  };
  s << "#\n";
};



//
void SgStnLogReadings::export2antabFrqs_vgosSetup(QTextStream& s)
{
  QString                       str("");
  int                           idx;

  s << "#\nNUMB_FRQ: " << channelSetup_.ifIdByTpSensorKey().size() << "\n#\n";
  s << "#       Sensor#  IF_Freq     LO_Freq    Sky_freq   Pol  SensorId Channel# SideBand\n#\n";

  idx = 1;
  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     ifKey(it.key());
    QString                     ifId(it.value());
    QString                     loId("");

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
    SgChannelSideBand           ifSb=CSB_UNDEF;
    SgChannelPolarization       polarization=CP_UNDEF;
    double                      skyFrq, loFrq=-1.0;
    skyFrq = -1.0;

    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_vgosSetup(): station " + stationName_ +
        ": cannot find side band for channel #" + QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

    // bandwidth:
    skyFrq = channelSetup_.ifFreqByIdx()[cIdx];

    if (channelSetup_.loIdByIdx().contains(cIdx))
      loId = channelSetup_.loIdByIdx().value(cIdx);
    else if (channelSetup_.loIdByCid().contains(ifId))
      loId = channelSetup_.loIdByCid().value(ifId);


    if (loId.size() && channelSetup_.loFreqById().contains(loId))
      loFrq = channelSetup_.loFreqById().value(loId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find LO frq for channel #" + QString("").setNum(cIdx) + ", loID=\"" + loId + "\"");


    // polarization:
    if (channelSetup_.loPolarizationById().contains(ifId))
      polarization = channelSetup_.loPolarizationById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find polarization for LO \"" + ifId + "\"");

    channelSetup_.ifPolarizationById()[ifId] = polarization;
  
    str.sprintf("FRQ: %7d %11.2f %11.2f %11.2f   ",
      idx, -1.0, loFrq, skyFrq);
    s << str;

    if (polarization == CP_RightCP)
      s << "  R";
    else if (polarization == CP_LeftCP)
      s << "  L";
    else if (polarization == CP_HorizontalLP)
      s << "  H";
    else if (polarization == CP_VerticalLP)
      s << "  V";
    else 
      s << "  ?";

    str.sprintf("    %6s   %6d   ", qPrintable(ifId), cIdx);
    s << str;
 
    if (ifSb == CSB_USB)
      s << "USB";
    else if (ifSb == CSB_LSB)
      s << "LSB";
    else 
      s << "UNK";
    s << "\n";
    idx++;
  };
  s << "#\n";
};



//
void SgStnLogReadings::export2antabFrqs_skededSetup(QTextStream& s, const SgChannelSkeded::StnCfg& sCfg)
{
  QString                       str("");
  int                           idx;

  if (channelSetup_.ifIdxById().size() != sCfg.getChannelCfgs().size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::export2antabFrqs_skededSetup(): station " + stationName_ + 
      ": number of channels are different");
    return;
  };
//std::cout << "      -- 1: sCfg.getChannelCfgs().size()=" << sCfg.getChannelCfgs().size() << "\n";
  s << "#\nNUMB_FRQ: " << channelSetup_.ifIdxById().size() << "\n#\n";
  s << "#       Sensor#  IF_Freq     LO_Freq    Sky_freq   Pol  SensorId Channel# SideBand\n#\n";

  idx = 1;

  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     ifKey(it.key());
    QString                     ifId(it.value());

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_nativeSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
    SgChannelSideBand           ifSb=CSB_UNDEF;
    double                      skyFrq, bw;
    skyFrq = bw = 0.0;
    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_skededSetup(): station " + stationName_ + ": cannot find side band for channel #" +
        QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");
    // bandwidth:
    bw = sCfg.getChannelCfgs().at(idx - 1).getBandWidth();
    skyFrq = sCfg.getChannelCfgs().at(idx - 1).getSkyFrq();
    channelSetup_.ifFreqByIdx()[cIdx] = skyFrq;
    channelSetup_.ifPolarizationById()[ifId] = CP_UNDEF;

    str.sprintf("FRQ: %7d %11.2f %11.2f %11.2f",
      idx, -1.0, -1.0, skyFrq);
    s << str << "   ";
    s << "  ?";

    str.sprintf("  %6s   ", qPrintable(ifId));
    s << str;
    str.sprintf("%6d", cIdx);
    s << str << "   ";
 
    if (ifSb == CSB_USB)
      s << "USB";
    else if (ifSb == CSB_LSB)
      s << "LSB";
    else 
      s << "UNK";
    s << "\n";
    idx++;
  };
  s << "#\n";
//std::cout << "      -- 10\n";
};



//
void SgStnLogReadings::export2antabFrqs_missedSetup(QTextStream& s)
{
  QString                       str("");
  int                           idx;

  s << "#\nNUMB_FRQ: " << channelSetup_.ifIdxById().size() << "\n#\n";
  s << "#       Sensor#  IF_Freq     LO_Freq    Sky_freq   Pol  SensorId Channel# SideBand\n#\n";

  idx = 1;

  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
  {
    QString                     ifKey(it.key());
    QString                     ifId(it.value());

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_missedSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
    SgChannelSideBand           ifSb=CSB_UNDEF;

    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antabFrqs_missedSetup(): station " + stationName_ + 
        ": cannot find side band for channel #" + QString("").setNum(cIdx) + 
        ", ID: \"" + ifId + "\"");

    channelSetup_.ifFreqByIdx()[cIdx] = -1.0;
    channelSetup_.ifPolarizationById()[ifId] = CP_UNDEF;
    channelSetup_.ifSideBandById()[ifId] = ifSb;
    
    str.sprintf("FRQ: %7d %11.2f %11.2f %11.2f",
      idx, -1.0, -1.0, -1.0);
    s << str << "     ?";

    str.sprintf("  %6s   ", qPrintable(ifId));
    s << str;
    str.sprintf("%6d", cIdx);
    s << str << "   ";

    if (ifSb == CSB_USB)
      s << "USB";
    else if (ifSb == CSB_LSB)
      s << "LSB";
    else 
      s << "UNK";
    s << "\n";
  };

  s << "#\n";
};



//
bool SgStnLogReadings::compareChannelSetups(const SgChannelSkeded::StnCfg& sCfg)
{
  bool                          isOk;
  double                        skyFrq, bw;
  int                           idx;
  QMap<int, double>             frqByIdx_Fsl, frqByIdx_Skd;
  QMap<int, double>             bwByIdx_Fsl, bwByIdx_Skd;
  int                           numOfChansAtSked, numOfChansAtLog;

  numOfChansAtSked = numOfChansAtLog = 0;
  isOk = true;
  //
  // sked's:
  for (int i=0; i<sCfg.getChannelCfgs().size(); i++)
  {
    bw = sCfg.getChannelCfgs().at(i).getBandWidth();
    skyFrq = sCfg.getChannelCfgs().at(i).getSkyFrq();
    frqByIdx_Skd[i] = skyFrq;
    bwByIdx_Skd[i] = bw;
    numOfChansAtSked++;
  };
  
  // FS log:
  idx = 0;
  for (QMap<QString, int>::const_iterator it=channelSetup_.ifIdxById().begin();
    it!=channelSetup_.ifIdxById().end(); ++it, idx++)
  {
    int                         cIdx=it.value();
    QString                     ifId(it.key());
    QString                     loId("");
    SgChannelSideBand           loSb=CSB_UNDEF;
    SgChannelSideBand           ifSb=CSB_UNDEF;
    double                      ifFrq, loFrq, skyFrq, bw;
    ifFrq = loFrq = skyFrq = bw = 0.0;

    if (channelSetup_.loIdByIdx().contains(cIdx))
      loId = channelSetup_.loIdByIdx().value(cIdx);
    else if (channelSetup_.loIdByCid().contains(ifId))
      loId = channelSetup_.loIdByCid().value(ifId);
    
    if (loId.size())
    {
//     loId = channelSetup_.loIdByIdx().value(cIdx);

      // LO side band:
      if (channelSetup_.loSideBandById().contains(loId))
        loSb = channelSetup_.loSideBandById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": cannot find side band for LO \"" + 
          loId + "\"");

      // channel side band:
      if (channelSetup_.ifSideBandById().contains(ifId))
        ifSb = channelSetup_.ifSideBandById().value(ifId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": cannot find side band for channel #" +
          QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

      // bandwidth:
      if (channelSetup_.bbcBandwidthByIdx().contains(cIdx))
        bw = channelSetup_.bbcBandwidthByIdx().value(cIdx);
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": cannot find bandwidth for channel #" +
          QString("").setNum(cIdx) + ", ID=" + ifId);

      if (channelSetup_.bbcFreqByIdx().contains(cIdx))
      {
        ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        //
        if (loSb == CSB_USB && ifSb == CSB_USB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        else if (loSb == CSB_USB && ifSb == CSB_LSB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
// here is freq calculation is perverted -- at the time of reading sked file we do not know about
// LO's sidebanad:
        else if (loSb == CSB_LSB && ifSb == CSB_LSB)
//        ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx);
        else if (loSb == CSB_LSB && ifSb == CSB_USB)
//        ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx);
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
      }
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": cannot find BBC_FRQ for channel #" +
          QString("").setNum(cIdx));

      if (channelSetup_.loFreqById().contains(loId))
        loFrq = channelSetup_.loFreqById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": cannot find LO frq for channel #" +
          QString("").setNum(cIdx) + ", loID=" + loId);
    
      skyFrq = loFrq + ifFrq;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::compareChannelSetups(): station " + stationName_ + ": cannot find LO ID for channel #" +
        QString("").setNum(cIdx));

    // special case:
    if (ifSb == CSB_DUAL)
    {
      frqByIdx_Fsl[idx] = skyFrq - bw;
      bwByIdx_Fsl[idx] = bw;
      idx++;
      numOfChansAtLog++;
    };
    frqByIdx_Fsl[idx] = skyFrq;
    bwByIdx_Fsl[idx] = bw;
    numOfChansAtLog++;
  };

  // check:
  if (frqByIdx_Fsl.size() != frqByIdx_Skd.size())
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::compareChannelSetups(): station " + stationName_ + ": the number of channels are different, " +
      QString("").sprintf("%d (log) vs %d (sked)", frqByIdx_Fsl.size(), frqByIdx_Skd.size()));
  }
  else
  {
    // Grrr!
    // freqs:
    QMap<double, int>           idxByFrq_Fsl, idxByFrq_Skd;
    for (QMap<int, double>::iterator it=frqByIdx_Fsl.begin(); it!=frqByIdx_Fsl.end(); ++it, idx++)
      idxByFrq_Fsl[it.value()] = it.key();
    for (QMap<int, double>::iterator it=frqByIdx_Skd.begin(); it!=frqByIdx_Skd.end(); ++it, idx++)
      idxByFrq_Skd[it.value()] = it.key();
    //
    QList<double>               d_Fsl=idxByFrq_Fsl.keys();
    QList<double>               d_Skd=idxByFrq_Skd.keys();

    for (int i=0; i<d_Fsl.size(); i++)
    {
      double                    frqFsl=d_Fsl.at(i), frqSkd=d_Skd.at(i);
      if (1.0e-4 < fabs(frqSkd - frqFsl))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + 
          ": the sky frequency for the channel #" +
          QString("").setNum(i) + " is different: " + 
          QString("").sprintf("%.4f (log) vs %.4f (sked)", frqFsl, frqSkd));
      };
    };
    // bandwidth:
    d_Fsl = bwByIdx_Fsl.values();
    d_Skd = bwByIdx_Skd.values();
    for (int i=0; i<d_Fsl.size(); i++)
    {
      double                    bwFsl=d_Fsl.at(i), bwSkd=d_Skd.at(i);
      if (1.0e-2 < fabs(bwSkd - bwFsl))
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + 
          ": the width for the channel #" +
          QString("").setNum(i) + " is different: " + 
          QString("").sprintf("%.2f (log) vs %.2f (sked)", bwFsl, bwSkd));
      };
    };
    /*
    for (QMap<int, double>::iterator it=frqByIdx_Fsl.begin(); it!=frqByIdx_Fsl.end(); ++it, idx++)
    {
      int                       cIdx=it.key();
      double                    frqFsl=it.value(), frqSkd;
      if (frqByIdx_Skd.contains(cIdx))
      {
        frqSkd = frqByIdx_Skd.value(cIdx);
        if (1.0e-4 < fabs(frqSkd - frqFsl))
        {
          isOk = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::compareChannelSetups(): station " + stationName_ + 
            ": the sky frequency for the channel #" +
            QString("").setNum(cIdx) + " is different: " + 
            QString("").sprintf("%.4f (log) vs %.4f (sked)", frqFsl, frqSkd));
        }
      }
      else
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": canot find channel index #" +
          QString("").setNum(cIdx) + " in the list of sked's channels (frq)");
      };
      double                    bwFsl, bwSkd;
      bwFsl = bwByIdx_Fsl.value(cIdx);
      if (bwByIdx_Skd.contains(cIdx))
      {
        bwSkd = bwByIdx_Skd.value(cIdx);
        if (1.0e-2 < fabs(bwSkd - bwFsl))
        {
          isOk = false;
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::compareChannelSetups(): station " + stationName_ + 
            ": the width for the channel #" +
            QString("").setNum(cIdx) + " is different: " + 
            QString("").sprintf("%.2f (log) vs %.2f (sked)", bwFsl, bwSkd));
        };
      }
      else
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::compareChannelSetups(): station " + stationName_ + ": canot find channel index #" +
          QString("").setNum(cIdx) + " in the list of sked's channels (bw)");
      };
    };
    */
  };
  return isOk;
};



//
bool SgStnLogReadings::compareChannelNumbers(const SgChannelSkeded::StnCfg& sCfg)
{
  bool                          isOk;
  int                           numOfChansAtSked, numOfChansAtLog;

  numOfChansAtSked = numOfChansAtLog = 0;
  isOk = true;
  //
  // sked's:
  numOfChansAtSked = sCfg.getChannelCfgs().size();

  // FS log:
  for (QMap<QString, int>::const_iterator it=channelSetup_.ifIdxById().begin();
    it!=channelSetup_.ifIdxById().end(); ++it)
  {
    QString                     ifId(it.key());
    SgChannelSideBand           ifSb=CSB_UNDEF;
    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::compareChannelNumbers(): station " + stationName_ + ": cannot find side band for channel #" +
        QString("").setNum(it.value()) + ", ID: \"" + ifId + "\"");

    // special case:
    if (ifSb == CSB_DUAL)
      numOfChansAtLog++;
    numOfChansAtLog++;
  };

  // check:
  if (numOfChansAtLog != numOfChansAtSked)
  {
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::compareChannelNumbers(): station " + stationName_ + ": the number of channels are different, " +
      QString("").sprintf("%d (log) vs %d (sked)", numOfChansAtLog, numOfChansAtSked));
  };
  return isOk;
};



//
bool SgStnLogReadings::createAntabFile(const QString& inputLogFileName, const QString& outputFileName,
  const QString& stnKey, const SgChannelSkeded *channelSkeded, bool useSkededChannelSetup, 
  bool have2overwrite, bool reportAllTsysData)
{
  enum                          ChannelSetup2Use {CSU_NONE, CSU_NATIVE, CSU_SKED, CSU_VEX, CSU_VGOS};
  bool                          haveSkeded;
  bool                          haveTsys;
  ChannelSetup2Use              setup2use;
  setup2use = CSU_NONE;
  haveSkeded = false;
  haveTsys = 0 < tSyses_.size();
  
  QString                       str("");
  
  if (!outputFileName.size())
  {
    // set up outut file name:
    if (5 < inputLogFileName.size() && inputLogFileName.right(4) == QString(".log"))
    {
      str = inputLogFileName.left(inputLogFileName.size() - 4) + ".ant";
    }
    else
      str = inputLogFileName + ".ant";
  }
  else
    str = outputFileName;
  QFile                         f(str);
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::createAntabFile(): station " + stnKey + ": " + 
    " going to create " + f.fileName() + " LOG-ANTAB file");

  if (haveTsys)
  {
    // check channel set up:
    if (!channelSkeded)
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
        ": the sked's channel setup is missing");
    else if (!channelSkeded->isOk())
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
      ": the sked's channel setup is not good, skipping it");
    else if (!channelSkeded->stnCfgByKey_.contains(stnKey))
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
        ": the sked's channel setup does not contain this station, skipping it");
    else if (channelSetup_.getHwType() != SgChannelSetup::HT_VGOS &&
            !compareChannelNumbers(channelSkeded->stnCfgByKey_.value(stnKey)))
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
        ": the number of collected channels and number of channels in the sked are different, skipping it");
    else
    {
      haveSkeded = true;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
        ": looks like we have channel setup from sked");
    };
    //
    //
    if (channelSetup_.selfCheck(stnKey))
    {
      setup2use = CSU_NATIVE;
      if (channelSetup_.getHwType() == SgChannelSetup::HT_VGOS)
        setup2use = CSU_VGOS;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + 
        ": channel setup from the log file looks ok");
      if (haveSkeded && channelSetup_.getHwType() != SgChannelSetup::HT_VGOS)
      {
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::createAntabFile(): station " + stnKey +
          ": comparing sked's and FS log channel setups");
        compareChannelSetups(channelSkeded->stnCfgByKey_.value(stnKey));
      };
    }
    else if (haveSkeded)
    {
      setup2use = CSU_SKED;
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + ": cannot figure out channel setup from the log "
        "file, falling back to the set up from sked file");
    }
    else
    {
      setup2use = CSU_NONE;
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + ": cannot figure out the channel setup");
    };
  
    if (setup2use == CSU_NATIVE && useSkededChannelSetup)
    {
      setup2use = CSU_SKED;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): station " + stnKey + ": forced to use the set up from sked file");
    };
  }
  else
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::createAntabFile(): station " + stnKey + ": no TSYS data have found in the log file");

  //
  //
  if (f.exists())
  {
    if (have2overwrite)
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): the file " + f.fileName() + " already exists; it will be overwritten");
    else
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntabFile(): the file " + f.fileName() + " already exists, skipping");
      return false;
    };
  };
  // find associated OnSource records for tsyses:
  // ...
  // ...


  if (f.open(QFile::WriteOnly))
  {
    QTextStream                 s(&f);

    s << "# LOG-ANTAB Format  Version of 2009.08.07\n";
    s << "#\n";
    s << "# Generator: " << qPrintable(libraryVersion.name()) << "\n";
    s << "#\n";
    s << "# Generated from log file " << qPrintable(inputLogFileName) << " on "
      << qPrintable(SgMJD().currentMJD().toString(SgMJD::F_SOLVE_SPLFL))<<"\n";
    s << "#\n";
    s << "STATION: " << qPrintable(stnKey) << "\n";
    s << "#\n";

    // Data On/Off:
    s << "NUMB_DATA_ON: " << onSourceRecords_.size() << "\n";
    s << "#\n";
    s << "#         Data_on start           Data_on end             Source    Scan\n";
    s << "#\n";
    for (int i=0; i<onSourceRecords_.size(); i++)
    {
      SgOnSourceRecord         *rec=onSourceRecords_.at(i);
      s << "DATA_ON:  "
        << qPrintable(rec->getTstart().toString(SgMJD::F_SOLVE_SPLFL_V3)) << "  "
        << qPrintable(rec->getTfinis().toString(SgMJD::F_SOLVE_SPLFL_V3)) << "  "
        << qPrintable(rec->getSourceName().leftJustified(8, ' ')) << "  "
        << qPrintable(rec->getScanName().leftJustified(8, ' ')) << "\n";
    };

    // Cable cals:
    s << "#\n";
    s << "NUMB_CAB: " << cableCals_.size() << "\n";

    if (cableCals_.size())
    {
      s << "#\n";
      s << "SIGN_CAB:   " << (cableSign_==0?"UNDEF":(cableSign_<0?"-1":"+1")) << "\n";
      s << "#\n";
      s << "#       Date                     Cable delay (sec)\n";
      s << "#\n";
      for (int i=0; i<cableCals_.size(); i++)
      {
        SgCableCalReading      *cc=cableCals_.at(i);
        s << "CABLE:  "
          << qPrintable(cc->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)) << "  "
          << qPrintable(QString("").sprintf("%12.5E", cc->getV())) << "\n";
      };
    };
  
    // meteo:
    s << "#\n";
    s << "NUMB_METEO: " << meteoPars_.size() << "\n";

    if (meteoPars_.size())
    {
      s << "#\n";
      s << "#       Date                    Temp    Pres    Humid\n";
      s << "#\n";
      for (int i=0; i<meteoPars_.size(); i++)
      {
        SgMeteoReading         *mr=meteoPars_.at(i);
        s << "METEO:  "
          << qPrintable(mr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)) << "  "
          << qPrintable(QString("").sprintf("%6.2f  %7.1f  %4.1f", 
              mr->getM().getTemperature() + 273.15, 
              mr->getM().getPressure()*100.0, 
              mr->getM().getRelativeHumidity()*100.0)) 
          << "\n";
      };
    };
    s << "#\n";

    if (tSyses_.size())
    {
      if (setup2use == CSU_NATIVE)
        export2antabFrqs_nativeSetup(s);
      else if (setup2use == CSU_VGOS)
        export2antabFrqs_vgosSetup(s);
      else if (setup2use == CSU_SKED && channelSkeded->stnCfgByKey_.contains(stnKey))
        export2antabFrqs_skededSetup(s, channelSkeded->stnCfgByKey_.value(stnKey));
      else if (setup2use == CSU_NONE)
        export2antabFrqs_missedSetup(s);
      else
      {
        s << "#\n";
        s << "#No FRQ data available\n";
        s << "#\n";
      };
      //
      //
      int                       numOfTsyses=0;
      if (!reportAllTsysData)
      {
        s << "#       Scan  Scan_name   Src_name    UTC_Time_tag           ";
        int                     idx;
        idx = 1;
        for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
          it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
          s << "  Ts#" << QString("").sprintf("%03d", idx);
        s << "\n#\n";

        // tsyses only for data_valid=on:
        for (int i=0; i<onSourceRecords_.size(); i++)
          numOfTsyses += onSourceRecords_.at(i)->tSyses().size();
        s << "NUMB_TSYS: " << numOfTsyses << "\n";
        s << "#\n";
        int                       counter=1;
        for (int i=0; i<onSourceRecords_.size(); i++)
        {
          SgOnSourceRecord         *sOn=onSourceRecords_.at(i);
          SgTsysReading            *tsr=NULL;
          for (int j=0; j<sOn->tSyses().size(); j++)
          {
            tsr = sOn->tSyses().at(j);
            s << str.sprintf("TSYS: %6d  ", counter);
            str = sOn->getScanName().leftJustified(9, ' ') + "   " +
                  sOn->getSourceName().leftJustified(8, ' ') + "    ";
            s << str;

            if (tsr && tsr->osRec())
              s << tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3) << " ";
            else
              s << sOn->getTstart().toString(SgMJD::F_SOLVE_SPLFL_V3) << " ";

            for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
              it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
            {
              double                  v;
              v = -2.0;
              if (tsr && tsr->getTsys().contains(it.value()))
                v = tsr->getTsys().value(it.value());
              s << str.sprintf(" %7.1f", v);
            };
            s << "\n";
            counter++;
          };
        };
      }
      else // all tsys collected from a log file:
      {
        s << "#        Idx     Azimuth  Elevation    UTC_Time_tag           ";
        int                     idx;
        idx = 1;
        for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
          it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
          s << "  Ts#" << QString("").sprintf("%03d", idx);
        s << "\n#                 (deg)     (deg)\n";

        for (int i=0; i<tSyses_.size(); i++)
        {
          SgTsysReading        *tsr=tSyses_.at(i);
          s << str.sprintf("TSYS: %6d  ", i+1);
          str = "SLWEING    SLEWING ";
          //
          SgTraklReading       *trakl=NULL;
          double                dt=1.0e3, d=0.0;
          for (int j=0; j<tsr->trakls().size(); j++)
          {
            if (tsr->trakls().at(j))
            {
              if ((d=fabs(tsr->getT() - tsr->trakls().at(j)->getT())) < dt)
              {
                dt = d;
                trakl = tsr->trakls().at(j);
              };
            };
          };
          if (trakl)
            str.sprintf("%10.5f %10.5f", trakl->getAz(), trakl->getEl());
//          str.sprintf("%10.5f %10.5f %2d %10.5f", 
//            trakl->getAz(), trakl->getEl(), tsr->trakls().size(), dt*DAY2SEC);

          s << str << "    " << tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3) << " ";
          for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
            it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
          {
            double              v;
            v = -2.0;
            if (tsr && tsr->getTsys().contains(it.value()))
              v = tsr->getTsys().value(it.value());
            s << str.sprintf(" %7.1f", v);
          };
          s << "\n";
        };
      };
    }
    else
    {
      s << "#\nNUMB_FRQ: 0\n#\n";
      s << "NUMB_TSYS: 0\n";
      s << "#\n#\n# No TSYS records found in the log file\n#\n";
    };


    s << "#\n";
    f.close();
    s.setDevice(NULL);
  };

  return true;
};



//
//
void SgStnLogReadings::export2antCalFrqs_nativeSetup(QTextStream& s)
{
  QString                       str("");
  QString                       sPol("");
  QString                       sSbd("");
  int                           idx;

  if (tSensorsPrinted2AntCal_)
    return;
  else
    tSensorsPrinted2AntCal_ = true;

//s << "#\nNUM_TP_SENSOR: " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\nDIMENSION: TP_SENSOR " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\n";
  s << "#           Sensor  IF_Freq   LO_Freq    Sky_frq_Cen   Bdw  Pol SensorId IF# SideBand\n";
  s << "#             tag     MHz       MHz         MHz        MHz\n";
  s << "#\n";
  idx = 1;

  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     sensorKey(it.key());
    QString                     ifId(it.value());

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_nativeSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);

    QString                     loId("");
    SgChannelSideBand           loSb=CSB_UNDEF;
    SgChannelSideBand           ifSb=CSB_UNDEF;
    SgChannelPolarization       polarization=CP_UNDEF;
    double                      ifFrq, loFrq, skyFrq, bw;
    ifFrq = loFrq = skyFrq = bw = dFiller_;

    if (channelSetup_.loIdByIdx().contains(cIdx))
      loId = channelSetup_.loIdByIdx().value(cIdx);
    else if (channelSetup_.loIdByCid().contains(ifId))
      loId = channelSetup_.loIdByCid().value(ifId);
    if (loId.size())
    {
      // LO side band:
      if (channelSetup_.loSideBandById().contains(loId))
        loSb = channelSetup_.loSideBandById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find side band for LO \"" + 
          loId + "\"");

      // channel side band:
      if (channelSetup_.ifSideBandById().contains(ifId))
        ifSb = channelSetup_.ifSideBandById().value(ifId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find side band for channel #" +
          QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

      // polarization:
      if (channelSetup_.loPolarizationById().contains(loId))
        polarization = channelSetup_.loPolarizationById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find polarization for LO \"" +
          loId + "\"");

      // bandwidth:
      if (channelSetup_.bbcBandwidthByIdx().contains(cIdx))
        bw = channelSetup_.bbcBandwidthByIdx().value(cIdx);
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find bandwidth for channel #" +
          QString("").setNum(cIdx) + ", ID=" + ifId);

      if (channelSetup_.bbcFreqByIdx().contains(cIdx))
      {
        ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        //
        if (loSb == CSB_USB && ifSb == CSB_USB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx);
        else if (loSb == CSB_USB && ifSb == CSB_LSB)
          ifFrq = channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
        else if (loSb == CSB_LSB && ifSb == CSB_LSB)
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx) - bw;
        else if (loSb == CSB_LSB && ifSb == CSB_USB)
          ifFrq = - channelSetup_.bbcFreqByIdx().value(cIdx);
        // report central frq:
        ifFrq += bw/2.0;
      }
      else 
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find BBC_FRQ for channel #" +
          QString("").setNum(cIdx));

      if (channelSetup_.loFreqById().contains(loId))
        loFrq = channelSetup_.loFreqById().value(loId);
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find LO frq for channel #" +
          QString("").setNum(cIdx) + ", loID=" + loId);
    
      skyFrq = loFrq + ifFrq;
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_nativeSetup(): station " + stationName_ + ": cannot find LO ID for channel #" +
        QString("").setNum(cIdx));

    channelSetup_.ifFreqByIdx()[cIdx] = skyFrq;
    channelSetup_.ifPolarizationById()[ifId] = polarization;

    sPol = sFiller_;
    if (polarization == CP_RightCP)
      sPol = "R";
    else if (polarization == CP_LeftCP)
      sPol = "L";
    else if (polarization == CP_HorizontalLP)
      sPol = "H";
    else if (polarization == CP_VerticalLP)
      sPol = "V";

    sSbd = sFiller_;
    if (ifSb == CSB_USB)
      sSbd = "USB";
    else if (ifSb == CSB_LSB)
      sSbd = "LSB";
    else if (ifSb == CSB_DUAL)
      sSbd = "USB/LSB";
 
//  str.sprintf("TP_SENSOR:  TS_%03d %8.2f %10.2f  %10.2f  %6.2f  %3s %8s %3d  %7s",
    str.sprintf("TP_SENSOR:  TP_%03d %8.2f %10.2f  %10.2f  %6.2f  %3s %8s %3d  %7s",
      idx, ifFrq, loFrq, skyFrq, bw,
      qPrintable(sPol),
      qPrintable(ifId),
      cIdx,
      qPrintable(sSbd)
      );
    s << str << "\n";
    
    idx++;
  };
  s << "#\n";
};



//
void SgStnLogReadings::export2antCalFrqs_vgosSetup(QTextStream& s)
{
  QString                       str("");
  QString                       sPol("");
  QString                       sSensorSbd("");
  QString                       sNetSbd("");
  int                           idx;
  QString                       sortingKey("");
  QMap<QString, QString>        sensorSetupByKey;
  

  if (tSensorsPrinted2AntCal_)
    return;
  else
    tSensorsPrinted2AntCal_ = true;

//s << "#\nNUM_TP_SENSOR: " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\nDIMENSION: TP_SENSOR " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\n";
  //
  // let know the user about duality of sensors:
  if (channelSetup_.origSensorIdById().size())
  {
    s << "# Note, the following sensor ID(s) were splitted by two sidebands: \n";
    for (QMap<QString, QList<QString> >::iterator it=channelSetup_.origSensorIdById().begin(); 
      it!=channelSetup_.origSensorIdById().end(); ++it)
    {
      const QString&            key=it.key();
      const QList<QString>&     lst=it.value();
      str = "#    sensor " + key + " => (";
      for (int i=0; i<lst.size(); i++)
        str += lst.at(i) + ", ";
      str.chop(2);
      s << str << ")\n";
    };
    s << "#\n";
  };
//  s << "#           Sensor  IF_Freq   LO_Freq    Sky_frq_Cen BndWd  Pol SensorId IF#   Sensor     Net   Comments\n";
//  s << "#             tag     MHz       MHz         MHz        MHz                   SideBand SideBand\n";
  s << "#           Sensor  IF_Freq   LO_Freq    Sky_frq_Cen BndWd  Pol SensorId IF#   Sensor     Net\n";
  s << "#             tag     MHz       MHz         MHz        MHz                   SideBand SideBand\n";
  s << "#\n";
  idx = 1;

  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     sensorKey(it.key());
    QString                     ifId(it.value());
    QString                     loId("");

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find IF index for IF with ID  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
    SgChannelSideBand           ifSb=CSB_UNDEF;
    SgChannelSideBand           loSb=CSB_UNDEF;
    SgChannelPolarization       polarization=CP_UNDEF;
    double                      ifFrq, loFrq, skyFrq, bw;
    ifFrq = loFrq = skyFrq = bw = dFiller_;
    //
 /*
    if (channelSetup_.loIdByIdx().contains(cIdx))
      loId = channelSetup_.loIdByIdx().value(cIdx);
    else if (channelSetup_.loIdByCid().contains(ifId))
      loId = channelSetup_.loIdByCid().value(ifId);
*/
    if (channelSetup_.loIdByTpSensorKey().contains(sensorKey))
      loId = channelSetup_.loIdByTpSensorKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find LO ID for sensorKey \"" + sensorKey + "\"");
    //
    // lo sideband:
    if (channelSetup_.loSideBandByTpSensorKey().contains(sensorKey))
      loSb = channelSetup_.loSideBandByTpSensorKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find LO sideband for sensorKey \"" + sensorKey + "\"");

    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ +
        ": cannot find side band for channel #" + QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

    // LO freq:
    if (channelSetup_.loFreqByTpSensorKey().contains(sensorKey))
      loFrq = channelSetup_.loFreqByTpSensorKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find LO frq for sensorKey \"" + sensorKey + "\"");

    // IF freq:
    if (channelSetup_.ifFreqByTpSensorKey().contains(sensorKey))
      ifFrq = channelSetup_.ifFreqByTpSensorKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find BBC_FRQ for channel #" + QString("").setNum(cIdx));

    // bandwidth:
    if (channelSetup_.ifBandwidthByTpSensorKey().contains(sensorKey))
      bw = channelSetup_.ifBandwidthByTpSensorKey().value(sensorKey);
    else 
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ +
        ": cannot find bandwidth for sensor key \"" + sensorKey + "\", ID=" + ifId);

    skyFrq = SgChannelSetup::calcSkyFreq(loFrq, ifFrq, bw, loSb, ifSb);

    // polarization:
    if (channelSetup_.loPolarizationByTpSensorKey().contains(sensorKey))
      polarization = channelSetup_.loPolarizationByTpSensorKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_vgosSetup(): station " + stationName_ + 
        ": cannot find LO polarization for sensorKey \"" + sensorKey + "\"");

    channelSetup_.ifPolarizationById()[ifId] = polarization;

    sPol = sFiller_;
    if (polarization == CP_RightCP)
      sPol = "R";
    else if (polarization == CP_LeftCP)
      sPol = "L";
    else if (polarization == CP_HorizontalLP)
      sPol = "H";
    else if (polarization == CP_VerticalLP)
      sPol = "V";

    sSensorSbd = sNetSbd = sFiller_;
    if (ifSb == CSB_USB)
      sSensorSbd = "USB";
    else if (ifSb == CSB_LSB)
      sSensorSbd = "LSB";
    else if (ifSb == CSB_DUAL)
      sSensorSbd = "USB/LSB";

    if (loSb == CSB_USB)
      sNetSbd = "USB";
    else if (loSb == CSB_LSB)
      sNetSbd = "LSB";
    else if (loSb == CSB_DUAL)
      sNetSbd = "USB/LSB";


//  str.sprintf("TP_SENSOR:  TS_%03d %8.2f %10.2f  %10.2f  %6.2f  %3s %8s %3d  %7s %7s   %s",
//    str.sprintf("TP_SENSOR:  TP_%03d %8.2f %10.2f  %10.2f  %6.2f  %3s %8s %3d  %7s %7s   %s",
    str.sprintf("TP_SENSOR:  TP_%03d %8.2f %10.2f  %10.2f  %6.2f  %3s %8s %3d  %7s %7s",
      idx, ifFrq, loFrq, skyFrq, bw,
      qPrintable(sPol),
      qPrintable(ifId),
      cIdx,
      qPrintable(sSensorSbd),
      qPrintable(sNetSbd)
//      qPrintable(QString(sensorKey).replace(' ', '_'))
      );
    s << str << "\n";
    idx++;
/*
//    sortingKey.sprintf("%10.2f %s %s",
//      skyFrq, qPrintable(sPol), qPrintable(sensorKey));

    sortingKey.sprintf("%s %10.2f %s",
      qPrintable(sPol), 
      skyFrq,
      qPrintable(sensorKey));

    if (!sensorSetupByKey.contains(sortingKey))
      sensorSetupByKey[sortingKey] = str;
*/
  };
/*  
  // make sorted output:
  for (QMap<QString, QString>::iterator it=sensorSetupByKey.begin(); it!=sensorSetupByKey.end(); ++it)
    s << it.value() << "\n";
*/

  s << "#\n";  
};



//
void SgStnLogReadings::export2antCalFrqs_skededSetup(QTextStream& s, const SgChannelSkeded::StnCfg& sCfg)
{
  QString                       str("");
  QString                       sPol("");
  QString                       sSbd("");
  int                           idx;

  if (tSensorsPrinted2AntCal_)
    return;
  else
    tSensorsPrinted2AntCal_ = true;

//s << "#\nNUM_TP_SENSOR: " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\nDIMENSION: TP_SENSOR " << channelSetup_.ifIdByTpSensorKey().size() << "\n";
  s << "#\n";
  s << "#           Sensor  IF_Freq   LO_Freq    Sky_frq_Cen   Bdw  Pol SensorId IF# SideBand\n";
  s << "#             tag     MHz       MHz         MHz        MHz\n";
  s << "#\n";
  idx = 1;

  if (channelSetup_.ifIdxById().size() != sCfg.getChannelCfgs().size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::export2antCalFrqs_skededSetup(): station " + stationName_ + 
      ": number of channels are different");
    return;
  };

  for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
    it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
  {
    QString                     sensorKey(it.key());
    QString                     ifId(it.value());

    int                         cIdx=-1;
    if (!channelSetup_.ifIdxById().contains(ifId))
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_skededSetup(): station " + stationName_ + 
        ": cannot find IF index for if with the id  \"" + ifId + "\"");
      continue;
    }
    else
      cIdx = channelSetup_.ifIdxById().value(ifId);
    
    SgChannelSideBand           ifSb=CSB_UNDEF;
    double                      ifFrq, loFrq, skyFrq, bw;
    ifFrq = loFrq = skyFrq = bw = dFiller_;

    // channel side band:
    if (channelSetup_.ifSideBandById().contains(ifId))
      ifSb = channelSetup_.ifSideBandById().value(ifId);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalFrqs_skededSetup(): station " + stationName_ + ": cannot find side band for channel #" +
        QString("").setNum(cIdx) + ", ID: \"" + ifId + "\"");

    // bandwidth:
    bw = sCfg.getChannelCfgs().at(idx - 1).getBandWidth();
    skyFrq = sCfg.getChannelCfgs().at(idx - 1).getSkyFrq();
    channelSetup_.ifFreqByIdx()[cIdx] = skyFrq;
    channelSetup_.ifPolarizationById()[ifId] = CP_UNDEF;

    skyFrq += bw/2.0;

    sPol = sFiller_;
    sSbd = sFiller_;
    if (ifSb == CSB_USB)
      sSbd = "USB";
    else if (ifSb == CSB_LSB)
      sSbd = "LSB";
    else if (ifSb == CSB_DUAL)
      sSbd = "USB/LSB";
 
//  str.sprintf("TP_SENSOR:  TS_%03d %8.2f %10.2f  %10.2f  %6.2f   %1s  %8s %3d  %7s",
    str.sprintf("TP_SENSOR:  TP_%03d %8.2f %10.2f  %10.2f  %6.2f   %1s  %8s %3d  %7s",
      idx, ifFrq, loFrq, skyFrq, bw,
      qPrintable(sPol),
      qPrintable(ifId),
      cIdx,
      qPrintable(sSbd)
      );
    s << str << "\n";
    idx++;
  };
  s << "#\n";
};



//
void SgStnLogReadings::export2antCalPcal_vgosSetup(QTextStream& s)
{
  QString                       str("");
  QString                       sPol("");
  QString                       sNetSbd("");
  QString                       sSensorSbd("");
  int                           idx;

//s << "#\nNUM_PC_SENSOR: " << channelSetup_.pcalIdByKey().size() << "\n";
  s << "#\nDIMENSION: PC_SENSOR " << channelSetup_.pcalIdByKey().size() << "\n";
  s << "#           Sensor    Sky_frq_Cen   Pol  SensorId    IF#  Sensor      Net\n";
  s << "#             tag         MHz                            SideBand SideBand\n#\n";

  idx = 1;

  for (QMap<QString, QString>::const_iterator it=channelSetup_.pcalIdByKey().begin();
    it!=channelSetup_.pcalIdByKey().end(); ++it)
  {
    QString                     sensorKey(it.key());
    QString                     sensorId(it.value());
    QString                     loId("");

    SgChannelSideBand           sideBand=CSB_UNDEF; // LO sideband
    SgChannelPolarization       polarization=CP_UNDEF;
    double                      skyFrq;
    double                      ifFreq;
    double                      freqOffset;
    double                      width;
    bool                        isOk;
    //
    skyFrq = dFiller_;
    freqOffset = width = 0.0;

    //
    // LO frequency:
    if (channelSetup_.pcalLoFreqByKey().contains(sensorKey))
      skyFrq = channelSetup_.pcalLoFreqByKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot find LO frequency for PCAL sensor key \"" + sensorKey + "\"");
    //
    if (channelSetup_.pcalSideBandByKey().contains(sensorKey))
      sideBand = channelSetup_.pcalSideBandByKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot find sideband for PCAL sensor key \"" + sensorKey + "\"");
    // 
    // polarization:
    if (channelSetup_.pcalPolarizationByKey().contains(sensorKey))
      polarization = channelSetup_.pcalPolarizationByKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot find polarization for PCAL sensor key \"" + sensorKey + "\"");
    
    // freq offset:
    if (channelSetup_.pcalOffsetByKey().contains(sensorKey))
      freqOffset = channelSetup_.pcalOffsetByKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot find frequency offset for PCAL sensor key \"" + sensorKey + "\"");
    
    // width:
    if (channelSetup_.pcalWidthByKey().contains(sensorKey))
      width = channelSetup_.pcalWidthByKey().value(sensorKey);
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot find width for PCAL sensor key \"" + sensorKey + "\"");
    
    //
    // 
    // calculate sky frequency:
    ifFreq = 0.0;
    ifFreq = sensorId.right(4).toDouble(&isOk);
    if (isOk)
    {
      if (sideBand == CSB_USB)
        skyFrq += 1024.0 - freqOffset - ifFreq;
      else if (sideBand == CSB_LSB)
        skyFrq -= 1024.0 - freqOffset - ifFreq;
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
          ": unsupported sideband \"" + sideBand2Str(sideBand) +
          "\" for PCAL sensor key \"" + sensorKey + "\", cannot calculate the sky frequency");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::export2antCalPcal_vgosSetup(): station " + stationName_ + 
        ": cannot convert \"" + sensorId.right(4) +
        "\" to double for PCAL sensor key \"" + sensorKey + "\"");
    //
    //
    sPol = sFiller_;
    if (polarization == CP_RightCP)
      sPol = "R";
    else if (polarization == CP_LeftCP)
      sPol = "L";
    else if (polarization == CP_HorizontalLP)
      sPol = "H";
    else if (polarization == CP_VerticalLP)
      sPol = "V";


  sNetSbd = sSensorSbd = sFiller_;
    if (sideBand == CSB_USB)
      sNetSbd = "USB";
    else if (sideBand == CSB_LSB)
      sNetSbd = "LSB";
    else if (sideBand == CSB_DUAL)
      sNetSbd = "USB/LSB";


//  str.sprintf("PC_SENSOR:  PC_%03d %14.2f  %3s  %8s    %d %7s   %s",
    str.sprintf("PC_SENSOR:  PS_%03d %14.2f  %3s  %8s    %d %7s  %7s",
      idx, skyFrq,
      qPrintable(sPol),
      qPrintable(sensorId),
      nFiller_,
      qPrintable(sSensorSbd),
      qPrintable(sNetSbd)//,
//      qPrintable(QString(key).replace(' ', '_'))
      );
    s << str << "\n";
    idx++;
  };
  s << "#\n";
};



/*
QString SgStnLogReadings::srcName4AntCal(const QString& srcName)
{
  return (srcName.size()?srcName:QString("DUMMY")).leftJustified(8, ' ');
};
*/


//
QString SgStnLogReadings::str4AntCal(const QString& str, int width)
{
  return (str.size()?str:sFiller_).leftJustified(width, ' ');
};



//
bool SgStnLogReadings::createAntCalFile(const QString& inputLogFileName, 
  const QString& outputFileName, const QString& stnKey, const SgChannelSkeded *channelSkeded, 
  bool useSkededChannelSetup, bool have2overwrite, bool reportAllReadings, bool supressNonUsedSensors,
  int wishedOutputData, const QString& ext4compress, const QString& userCommand)
{
  enum                          ChannelSetup2Use {CSU_NONE, CSU_NATIVE, CSU_SKED, CSU_VEX, CSU_VGOS};
  const QString                 dataFlag[] = {"DATA:OFF", "DATA:ON_SOURCE", "DATA:VALID_ON"};
  bool                          haveSkeded;
  ChannelSetup2Use              setup2use;
  int                           scanIdx, epochIdx;
  QString                       srcName(sFiller_);
  QString                       scnName(sFiller_);
  QString                       outputContent("");
  QString                       str(""), str2("");
  float                         el, az;
  int                           outputData=wishedOutputData;
  int                           numOfRecs, numOfEpochs, numOfSensors;
  int                           dataFlagIdx;
  bool                          hasSensor;
  bool                          isOk;
  QMap<QString, int>            numByEpoch;

  numOfEpochs = numOfSensors = 0;
  tSensorsPrinted2AntCal_ = false;
  setup2use = CSU_NONE;
  haveSkeded = false;
  isOk = true;
  hasSensor = false;
  
  // check what user want and what we have:
  if (outputData & AOD_DATA_ON && onSourceRecords_.size()==0)
    outputData &= ~AOD_DATA_ON;
  if (outputData & AOD_CABLE_SIGN && cableSign_==0)
    outputData &= ~AOD_CABLE_SIGN;
  if (outputData & AOD_CABLE_CAL && cableCals_.size()==0)
    outputData &= ~AOD_CABLE_CAL;
  if (outputData & AOD_METEO && meteoPars_.size()==0)
    outputData &= ~AOD_METEO;
  if (outputData & AOD_TSYS && tSyses_.size()==0)
    outputData &= ~AOD_TSYS;
  if (outputData & AOD_TSYS && tSyses_.size()!=0 && tSyses_.at(0)->getTsys().size()==0)
    outputData &= ~AOD_TSYS;
  if (outputData & AOD_TPI && tSyses_.size()==0)
    outputData &= ~AOD_TPI;
  if (outputData & AOD_TPI && tSyses_.size()!=0 && tSyses_.at(0)->getTpcont().size()==0)
    outputData &= ~AOD_TPI;
  if (outputData & AOD_PCAL && pcals_.size()==0)
    outputData &= ~AOD_PCAL;
  if (outputData & AOD_FMTGPS && dot2xpses_.size()==0)
    outputData &= ~AOD_FMTGPS;
  if (outputData & AOD_DBBC3TP && dbbc3Tps_.size()==0)
    outputData &= ~AOD_DBBC3TP;
  if (outputData & AOD_SEFD && sefds_.size()==0)
    outputData &= ~AOD_SEFD;
  //
  // form string with output info:
  if (outputData & AOD_DATA_ON)
    outputContent += "DATA_ON ";
  if (outputData & AOD_CABLE_SIGN)
    outputContent += "CABLE_SIGN ";
  if (outputData & AOD_CABLE_CAL)
    outputContent += "CABLE ";
  if (outputData & AOD_METEO)
    outputContent += "METEO ";
  if (outputData & AOD_TSYS)
  {
    outputContent += "TP_SENSOR ";
    outputContent += "TSYS ";
    hasSensor = true;
  };
  if (outputData & AOD_TPI)
  {
    if (!hasSensor)
    {
      outputContent += "TP_SENSOR ";
      hasSensor = true;
    };
    outputContent += "TPI ";
  };
  if (outputData & AOD_DBBC3TP)
  {
    if (!hasSensor)
    {
      outputContent += "TP_SENSOR ";
      hasSensor = true;
    };
    outputContent += "TPC ";
  };
  if (outputData & AOD_SEFD)
  {
    if (!hasSensor)
    {
      outputContent += "TP_SENSOR ";
      hasSensor = true;
    };
    outputContent += "SEFD ";
  };
  if (outputData & AOD_PCAL)
    outputContent += "PC_SENSOR PCAL ";
  if (outputData & AOD_FMTGPS)
    outputContent += "FMT2GPS_TIMER FMTGPS";

  if (!outputContent.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::createAntCalFile(): station " + stnKey + ": nothing to output");
    isOk = false;
    return isOk;
  }
  else
  {
    outputContent.chop(1);
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::createAntCalFile(): station " + stnKey + ": collected content for output: \"" + 
      outputContent + "\"");
  };
  //
  if (!outputFileName.size())
  {
    str = inputLogFileName;
    const SgIoExternalFilter   *filter=compressors.lookupFilterByFileName(str);
    if (filter)
      str.chop(filter->getDefaultExtension().size() + 1);

    // set up outut file name:
    if (5 < str.size() && str.right(4) == QString(".log"))
      str.chop(4);
    str += "_orig.anc";
    if (ext4compress.size())
      str += "." + ext4compress;
  }
  else
    str = outputFileName;

  if (ext4compress.size() && str.right(ext4compress.size()) != ext4compress)
    str += "." + ext4compress;

  FILE                         *pipe=NULL;
  QTextStream                   s;
  QFile                         f(str);
  //
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::createAntCalFile(): station " + stnKey + ": " + 
    " going to create ANTCAL file \"" + f.fileName() + "\"");
  //
  if (f.exists())
  {
    if (have2overwrite)
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): the file " + f.fileName() + " already exists; it will be overwritten");
    else
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): the file " + f.fileName() + " already exists, skipping");
      isOk = false;
      return isOk;
    };
  };
  //
  pipe = compressors.openFlt(str, f, s, FLTD_Output);
  //

  //  if (haveTsys)
  if (outputData & (AOD_TSYS | AOD_DBBC3TP | AOD_TPI | AOD_SEFD))
  {
    // check channel set up:
    if (!channelSkeded)
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
        ": the sked's channel setup is missing");
    else if (!channelSkeded->isOk())
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
      ": the sked's channel setup is not good, skipping it");
    else if (!channelSkeded->stnCfgByKey_.contains(stnKey))
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
        ": the sked's channel setup does not contain this station, skipping it");
    else if (channelSetup_.getHwType() != SgChannelSetup::HT_VGOS &&
            !compareChannelNumbers(channelSkeded->stnCfgByKey_.value(stnKey)))
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
        ": the number of collected channels and number of channels in the sked are different, skipping it");
    else
    {
      haveSkeded = true;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
        ": looks like we have channel setup from sked");
    };
    //
    //
    if (channelSetup_.selfCheck(stnKey))
    {
      setup2use = CSU_NATIVE;
      if (channelSetup_.getHwType() == SgChannelSetup::HT_VGOS ||
          channelSetup_.getBackEndType() == SgChannelSetup::BET_DBBC3)
        setup2use = CSU_VGOS;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + 
        ": channel setup from the log file looks ok");
      if (haveSkeded && channelSetup_.getHwType() != SgChannelSetup::HT_VGOS)
      {
        logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
          "::createAntCalFile(): station " + stnKey +
          ": comparing sked's and FS log channel setups");
        compareChannelSetups(channelSkeded->stnCfgByKey_.value(stnKey));
      };
    }
    else if (haveSkeded)
    {
      setup2use = CSU_SKED;
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + ": cannot figure out channel setup from the log "
        "file, falling back to the set up from sked file");
    }
    else
    {
      setup2use = CSU_NONE;
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + ": cannot figure out the channel setup");
    };
  
    if (setup2use == CSU_NATIVE && useSkededChannelSetup)
    {
      setup2use = CSU_SKED;
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::createAntCalFile(): station " + stnKey + ": forced to use the set up from sked file");
    };
  };

  // find associated onSource records for tsyses:
  // ...
  // ...

  if (s.device())
  {
    QFile                       fInpt(inputLogFileName); 
    QFileInfo                   fi(fInpt);

    s << "# ANTCAL Format  Version  1.0 of 2025.08.22\n#\n";
    s << "#\n# Fillers for uninitialized double, integer, string and date-time values\n";
    s << "FILLERS:   " << dFiller_ << "   " << nFiller_ << "    " 
      <<  qPrintable(sFiller_) << "    "  << qPrintable(tZero.toString(SgMJD::F_SOLVE_SPLFL)) 
      << "\n#\n";
    s << "STATION:  " << qPrintable(stnKey) << "\n#\n";

    if (backEndInfo_.size())
      s << "# BACKEND INFO:  " << qPrintable(backEndInfo_) << "\n";
    if (recorderInfo_.size())
      s << "# RECORDER INFO:  " << qPrintable(recorderInfo_) << "\n#\n";

    s << "EXP_CODE: " << qPrintable(experimentName_.size()?experimentName_.toLower():sFiller_) << "\n#\n";
    s << "#\n#         sec\n#\nUTC_MTAI: " << dFiller_ << "\n#\n#\n";
    s << "NUM_PROVENANCE: 1\n";
    s << "#\n";


//  s << "PROVENANCE: 1 GENERATOR:      "  << qPrintable(libraryVersion.name()) << " / "
//    << qPrintable(driverVersion->name()) << "\n";
//  s << "PROVENANCE: 1 GENERATOR:      "  << qPrintable(libraryVersion.name(SgVersion::NF_Petrov)) 
    s << "PROVENANCE: 1 GENERATOR:      "  << qPrintable(driverVersion->name(SgVersion::NF_Petrov)) 
      << "\n";
    s << "PROVENANCE: 1 COMMAND_LINE:   "  << qPrintable(userCommand) << "\n";
    s << "PROVENANCE: 1 CREATED_VARS:   "  << qPrintable(outputContent) << "\n";
    s << "PROVENANCE: 1 CREATION_DATE:  "
      << qPrintable(SgMJD().currentMJD().toString(SgMJD::F_SOLVE_SPLFL)) << "\n";
    s << "PROVENANCE: 1 NUM_FILES:      1\n";

    s << "PROVENANCE: 1 DATA_TYPE:      1 FS_LOG Version " << fsVersionLabel_ << "\n";
    s << "PROVENANCE: 1 DATA_FILE:      " << qPrintable(fi.canonicalFilePath()) << "\n";
//  s << "PROVENANCE: 1 DATA_FILE:      " << qPrintable(inputLogFileName) << "\n";

    s << "PROVENANCE: 1 DATA_CREATED:   1 "
      << qPrintable(SgMJD(fi.lastModified()).toString(SgMJD::F_SOLVE_SPLFL)) << "\n";

    switch (contentType_)
    {
      case CT_IVS: 
        s << "PROVENANCE: 1 NUM_COMMENTS:   1\n";
        s << "PROVENANCE: 1 COMMENT:        Original field system log\n#\n";
        break;
      case CT_SDE: 
        s << "PROVENANCE: 1 NUM_COMMENTS:   2\n";
        s << "PROVENANCE: 1 COMMENT:        Original field system log\n";
        s << "PROVENANCE: 1 COMMENT:        Original schedule Generated by sds_to_snap\n#\n";
        break;
      case CT_UNDEF: 
      default:
        s << "PROVENANCE: 1 NUM_COMMENTS:   1\n";
        s << "PROVENANCE: 1 COMMENT:        Undefined content type\n#\n";
        break;
    };
    //
    //
    //
    // Data On/Off:
    if (outputData & AOD_DATA_ON)
    {
//    s << "#\nNUM_DATA_ON: " << onSourceRecords_.size() << "\n#\n";
      s << "#\nDIMENSION: DATA_ON " << onSourceRecords_.size() << "\n#\n";
      s << "#       ScanIdx Data_on UTC start       Data_on UTC end         Source    ScanName\n";
      s << "#               YYYY.DD.MM-hh:mm:ss.ff  YYYY.DD.MM-hh:mm:ss.ff\n";
      s <<"#\n";
      for (int i=0; i<onSourceRecords_.size(); i++)
      {
        SgOnSourceRecord         *sOn=onSourceRecords_.at(i);
        s << "DATA_ON: "
          << qPrintable(str.sprintf("%6d ", i+1) + 
//            sOn->getTstart().toString(SgMJD::F_SOLVE_SPLFL_V3) + "  " +   // <- epoch of when antenna is on source
              sOn->getTdataOn().toString(SgMJD::F_SOLVE_SPLFL_V3) + "  " +  // <- epoch of data_valid=on 
              sOn->getTfinis().toString(SgMJD::F_SOLVE_SPLFL_V3) + "  " +   // <- epoch of data_valid=off
//            srcName4AntCal(sOn->getSourceName()) + "  " +
              str4AntCal(sOn->getSourceName(), 8) + "  " +
              str4AntCal(sOn->getScanName(), 8) )
//              sOn->getScanName().leftJustified(8, ' ') )
          << "\n";
      };
      s << "#\n";
    };
    //
    //
    // Cable sign:
    if (outputData & AOD_CABLE_SIGN)
    {
      s << "#\nDIMENSION: CABLE_SIGN 1\n";
      s << "#\nCABLE_SIGN:  1 " << qPrintable(tZero.toString(SgMJD::F_SOLVE_SPLFL)) 
        << " " << (cableSign_<0?"-1":"+1") << "\n";
    };
    //
    // Cable cals:
    if (outputData & AOD_CABLE_CAL)
    {
      // count the numbers:
      numOfEpochs = numOfRecs = 0;
      numByEpoch.clear();
      for (int i=0; i<cableCals_.size(); i++)
      {
        SgCableCalReading      *cc=cableCals_.at(i);
        if (reportAllReadings || cc->getOsRec())
        {
          str2 = cc->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
          if (!numByEpoch.contains(str2))               // a new epoch 
            numByEpoch[str2] = numByEpoch.size();
          numOfRecs++;
        };
      };
      numOfEpochs = numByEpoch.size();
      //
      s << "#\nDIMENSION: CABLE " << numOfRecs << "\n";
      s << "#\n";
      s << "#      EpocIdx UTC Date                 Cable delay   Source    ScanName\n";
      s << "#              YYYY.DD.MM-hh:mm:ss.ff   s\n";
      s << "#\n";
      for (int i=0; i<cableCals_.size(); i++)
      {
        SgCableCalReading      *cc=cableCals_.at(i);
        const SgOnSourceRecord *sOn=cc->getOsRec();
        scanIdx = epochIdx = nFiller_;
        if (reportAllReadings || sOn)
        {
          str2 = cc->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
          if (numByEpoch.contains(str2))
            epochIdx = numByEpoch.value(str2);
          s << "CABLE: " << qPrintable(str.sprintf("%6d  ", epochIdx + 1));
          s << qPrintable(
                str2 +
                str.sprintf("  %12.5E   ", cc->getV()) +
                str4AntCal(sOn?sOn->getSourceName():"", 8) + "  " +
                str4AntCal(sOn?sOn->getScanName():"", 8) )
                << "\n";
        };
      };
      s << "#\n";
    };
    //
    //
    // meteo:
    if (outputData & AOD_METEO)
    {
      // count the numbers:
      numOfEpochs = numOfRecs = 0;
      numByEpoch.clear();
      for (int i=0; i<meteoPars_.size(); i++)
      {
        SgMeteoReading         *mr=meteoPars_.at(i);
        if (reportAllReadings || mr->getOsRec())
        {
          str2 = mr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
          if (!numByEpoch.contains(str2))               // a new epoch 
            numByEpoch[str2] = numByEpoch.size();
          numOfRecs++;
        };
      };
      numOfEpochs = numByEpoch.size();
      //
      s << "#\nDIMENSION: METEO " << numOfRecs << "\n";
      s << "#\n";
      s << "#     EpocIdx   UTC Date                 Temp      Pres   Humid  ScanIdx  Source    Scan\n";
      s << "#               YYYY.DD.MM-hh:mm:ss.ff     K        Pa     %\n";
      s << "#\n";
      for (int i=0; i<meteoPars_.size(); i++)
      {
        SgMeteoReading         *mr=meteoPars_.at(i);
        const SgOnSourceRecord *sOn=mr->getOsRec();
        scanIdx = epochIdx = nFiller_;
        if (reportAllReadings || sOn)
        {
          str2 = mr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
          if (numByEpoch.contains(str2))
            epochIdx = numByEpoch.value(str2);
          s << "METEO: "
            << qPrintable(str.sprintf("%7d  ", epochIdx + 1))
            << qPrintable(str2) << "  "
            << qPrintable(QString("").sprintf("%6.2f  %7.1f  %5.1f  ", 
                mr->getM().getTemperature() + 273.15, 
                mr->getM().getPressure()*100.0, 
                mr->getM().getRelativeHumidity()*100.0)) 
            << qPrintable(str.sprintf("%6d  ", scanIdx))
            << qPrintable(
                str4AntCal(sOn?sOn->getSourceName():"", 8) + "  " +
                str4AntCal(sOn?sOn->getScanName():"", 8) )
            << "\n";
        };
      };
      s << "#\n";
    };
    //
    //
    // TP_SENSORS:
    if (outputData & (AOD_TSYS | AOD_TPI | AOD_SEFD | AOD_DBBC3TP))
    {
      // sensor info:
      s << "#\n#\n";
      if (setup2use == CSU_NATIVE)
      {
//        if (outputData & AOD_DBBC3TP)
//          export2antCalFrqs_dbbc3Setup(s);
//        else
          //export2antCalFrqs_nativeSetup(s);
          export2antCalFrqs_vgosSetup(s);
      }
      else if (setup2use == CSU_VGOS)
      {
//        if (outputData & AOD_DBBC3TP)
//          export2antCalFrqs_dbbc3Setup(s);
//        else
//          export2antCalFrqs_dbbc3Setup(s);
        export2antCalFrqs_vgosSetup(s);
      }
      else if (setup2use == CSU_SKED && channelSkeded->stnCfgByKey_.contains(stnKey))
        export2antCalFrqs_skededSetup(s, channelSkeded->stnCfgByKey_.value(stnKey));
      else if (setup2use == CSU_NONE)
        export2antabFrqs_missedSetup(s);
      else
      {
        s << "#\n";
        s << "# No TP_SENSOR data available\n";
        s << "#\n";
      };
    };
    //
    //
    // TSYS:
    if (outputData & AOD_TSYS)
    {
      // calc the numbers:
      numOfRecs = numOfEpochs = numOfSensors = 0;
      numOfSensors = channelSetup_.ifIdByTpSensorKey().size();
      numByEpoch.clear();
      for (int i=0; i<tSyses_.size(); i++)
      {
        SgTsysReading          *tsr=tSyses_.at(i);
        if (tsr->getTsys().size())
        {
          const SgOnSourceRecord 
                               *osRec=tsr->getOsRec();
          if (reportAllReadings || osRec)
            for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
              it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
              if (!supressNonUsedSensors || tsr->getTsys().contains(it.key()))
              {
                numByEpoch[tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
                numOfRecs++;
              };
        };
      };
      numOfEpochs = numByEpoch.size();
      if (numOfEpochs) // can be zero
      {
        //     
        //
//      s << "#\nNUM_TSYS: " << numOfRecs << "\n";
//        s << "#\nDIMENSION: TSYS " << numOfRecs << "\n";
        s << "#\nDIMENSION:   TSYS " << numOfEpochs << " " << numOfSensors << "\n";
        s << "NUM_RECORDS: TSYS " << numOfRecs << "\n";
        s << "#\n";
        s << "#     EpocIdx UTC_Time_tag            Sensor    Tsys    Azimuth   Elevat  Source    Scan       DataScopeFlag\n";
        s << "#             YYYY.DD.MM-hh:mm:ss.ff    tag      K          deg      deg\n";
        s << "#\n";
        //
        //
        for (int i=0; i<tSyses_.size(); i++)
        {
          SgTsysReading        *tsr=tSyses_.at(i);
          if (tsr->getTsys().size())
          {
            el = az = dFiller_;
            //
            // check for az/el info:
            SgTraklReading       *trakl=NULL;
            double                dt=1.0e3, d=0.0;
            //
            for (int j=0; j<tsr->trakls().size(); j++)
              if (tsr->trakls().at(j) &&
                (d=fabs(tsr->getT() - tsr->trakls().at(j)->getT())) < dt)
              {
                dt = d;
                trakl = tsr->trakls().at(j);
              };
            //
            if (trakl && (trakl->isAttr(SgTraklReading::Attr_FROM_SOURCE) || dt<4.0/DAY2SEC))
            {
              az = trakl->getAz();
              el = trakl->getEl();
            };
            //
            // src & scan names:
            srcName = scnName = sFiller_;
            scanIdx = epochIdx = nFiller_;
            const SgOnSourceRecord *osRec=tsr->getOsRec();
            if (osRec)
            {
              srcName = osRec->getSourceName();
              scnName = osRec->getScanName();
              scanIdx = osRec->getIdx() + 1;
            };
            //
            if (reportAllReadings || osRec)
            {
              int                   idx=1;
              for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
                it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
              {
                QString             sensorKey(it.key());
                double              v;
                hasSensor = false;
                v = dFiller_;
                if (tsr->getTsys().contains(sensorKey))
                {
                  v = tsr->getTsys().value(sensorKey);
                  hasSensor = true;
                };
                if (!supressNonUsedSensors || hasSensor)
                {
                  str2 = tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
                  if (numByEpoch.contains(str2))
                    epochIdx = numByEpoch.value(str2);
                  dataFlagIdx = osRec?(osRec->getTdataOn()<tsr->getT()?2:1):0;
//                  str.sprintf("TSYS:  %6d %s  TS_%03d %7.1f  %9.4f %8.4f  %s  %s  %s", 
                  str.sprintf("TSYS:  %6d %s  TP_%03d %7.1f  %9.4f %8.4f  %s  %s  %s", 
//                    scanIdx, qPrintable(tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)),
                    epochIdx, qPrintable(str2),
                    idx, v, az, el,
                    qPrintable(str4AntCal(srcName, 8)),
                    qPrintable(str4AntCal(scnName, 9)),
                    qPrintable(dataFlag[dataFlagIdx])
                  );
                  s << str << "\n";
                };
              };
            };
          };
        };
        s << "#\n";
        numByEpoch.clear();
      };
    };
    //
    //
    // TPI data:
    if (outputData & AOD_TPI)
    {
      // calc the numbers:
      numOfRecs = numOfEpochs = numOfSensors = 0;
      numOfSensors = channelSetup_.ifIdByTpSensorKey().size();
      numByEpoch.clear();
      for (int i=0; i<tSyses_.size(); i++)
      {
        SgTsysReading          *tsr=tSyses_.at(i);
        if (tsr->getTpcont().size())
        {
          if (reportAllReadings || tsr->getOsRec())
            for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
              it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
              if (!supressNonUsedSensors || tsr->getTpcont().contains(it.key()))
              {
                numByEpoch[tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
                numOfRecs++;
              };
        };
      };      
      numOfEpochs = numByEpoch.size();
      if (numOfEpochs) // can be zero
      {
        //
        //
//       s << "#\nNUM_TPI: " << numOfRecs << "\n";
        s << "#\nDIMENSION: TPI " << numOfEpochs << " " << numOfSensors << "\n";
        s << "NUM_RECORDS: TPI " << numOfRecs << "\n";
        s << "#\n";
        s << "#    EpocIdx UTC_Time_tag            Sensor     TpiOn   TpiOff  TpiZero    Azimuth  Elevat   Source    Scan       DataScopeFlag\n";
        s << "#            YYYY.DD.MM-hh:mm:ss.ff    tag        K        K       K       deg      deg\n";
        s << "#\n";
        //
        for (int i=0; i<tSyses_.size(); i++)
        {
          SgTsysReading          *tsr=tSyses_.at(i);
          if (tsr->getTpcont().size())
          {
            el = az = dFiller_;
            //
            // check for az/el info:
            SgTraklReading       *trakl=NULL;
            double                dt=1.0e3, d=0.0;
            //
            for (int j=0; j<tsr->trakls().size(); j++)
              if (tsr->trakls().at(j) &&
                (d=fabs(tsr->getT() - tsr->trakls().at(j)->getT())) < dt)
              {
                dt = d;
                trakl = tsr->trakls().at(j);
              };
            //
            if (trakl && (trakl->isAttr(SgTraklReading::Attr_FROM_SOURCE) || dt<4.0/DAY2SEC))
            {
              az = trakl->getAz();
              el = trakl->getEl();
            };
            //
            // src & scan names:
            srcName = scnName = sFiller_;
            scanIdx = epochIdx = nFiller_;
            const SgOnSourceRecord *osRec=tsr->getOsRec();
            if (osRec)
            {
              srcName = osRec->getSourceName();
              scnName = osRec->getScanName();
              scanIdx = osRec->getIdx() + 1;
            };
            //
            if (reportAllReadings || osRec)
            {
              int                 idx=1;
              for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
                it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
              {
                QString           sensorKey(it.key());
                int               vOn, vOf, v00;
                vOn = vOf = v00 = nFiller_;
                hasSensor = false;
                if (tsr->getTpcont().contains(sensorKey))
                {
                  vOn = tsr->getTpcont().value(sensorKey)[0];
                  vOf = tsr->getTpcont().value(sensorKey)[1];
                  hasSensor = true;
                };
                if (!supressNonUsedSensors || hasSensor)
                {
                  str2 = tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
                  if (numByEpoch.contains(str2))
                    epochIdx = numByEpoch.value(str2);
                  dataFlagIdx = osRec?(osRec->getTdataOn()<tsr->getT()?2:1):0;
//                  str.sprintf("TPI:  %6d %s  TS_%03d  %8d %8d  %9.4f %8.4f  %s  %s  %s", 
                  str.sprintf("TPI:  %6d %s  TP_%03d  %8d %8d %8d  %9.4f %8.4f  %s  %s  %s", 
//                    scanIdx, qPrintable(tsr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)),
                    epochIdx, qPrintable(str2),
                    idx, vOn, vOf, v00, az, el,
                    qPrintable(str4AntCal(srcName, 8)),
                    qPrintable(str4AntCal(scnName, 9)),
                    qPrintable(dataFlag[dataFlagIdx]));
                  s << str << "\n"; 
                };
              };
            };
          };
        };
        s << "#\n";
        numByEpoch.clear();
      };
      //
      //
    };
    //
    //
    if (outputData & AOD_PCAL)
    {
      // sensor setups:
      export2antCalPcal_vgosSetup(s);
      //
      //
      numOfRecs = numOfEpochs = numOfSensors = 0;
      // calc the numbers:
      
      numOfSensors = channelSetup_.pcalIdByKey().size();
      numByEpoch.clear();
      
      for (int i=0; i<pcals_.size(); i++)
      {
        SgPcalReading          *pcr=pcals_.at(i);
        const SgOnSourceRecord *osRec=pcr->getOsRec();
        //
        if (reportAllReadings || osRec)
          for (QMap<QString, QString>::const_iterator it=channelSetup_.pcalIdByKey().begin();
            it!=channelSetup_.pcalIdByKey().end(); ++it)
            if (!supressNonUsedSensors || pcr->getPcal().contains(it.value()))
            {
              numByEpoch[pcr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
              numOfRecs++;
            };
      };
      numOfEpochs = numByEpoch.size();
     

//    s << "#\nNUM_PCAL: " << numOfRecs << "\n";
//      s << "#\nDIMENSION: PCAL " << numOfRecs << "\n";
      s << "#\nDIMENSION: PCAL " << numOfEpochs << " " << numOfSensors << "\n";
      s << "NUM_RECORDS: PCAL " << numOfRecs << "\n";
      s << "#\n";
      //
      // pcal data:
      s << "#\n";
      s << "#     EpocIdx UTC_Time_tag            Sensor     Ampl      Phase      Source    Scan       DataScopeFlag\n";
      s << "#             YYYY.DD.MM-hh:mm:ss.ff    tag            rad [0,2pi)\n";
      s << "#\n";
 
      for (int i=0; i<pcals_.size(); i++)
      {
        SgPcalReading          *pcr=pcals_.at(i);
        //
        // src & scan names:
        srcName = scnName = sFiller_;
        scanIdx = epochIdx = nFiller_;
        const SgOnSourceRecord *osRec=pcr->getOsRec();
        if (osRec)
        {
          srcName = osRec->getSourceName();
          scnName = osRec->getScanName();
          scanIdx = osRec->getIdx() + 1;
        };
        //
        if (reportAllReadings || osRec)
        {
          int                   idx=1;
          for (QMap<QString, QString>::const_iterator it=channelSetup_.pcalIdByKey().begin();
            it!=channelSetup_.pcalIdByKey().end(); ++it, idx++)
          {
            float               vAmp, vPhs;
            vAmp = vPhs = dFiller_;
            hasSensor = false;
            if (pcr->getPcal().contains(it.value()))
            {
              vAmp = *pcr->getPcal().value(it.value());
              vPhs = *(pcr->getPcal().value(it.value()) + 1);
              vPhs = fmodf(vPhs + 360.0, 360.0)/RAD2DEG; // deg -> rad
              hasSensor = true;
            };
            if (!supressNonUsedSensors || hasSensor)
            {
              str2 = pcr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
              if (numByEpoch.contains(str2))
                epochIdx = numByEpoch.value(str2);

              dataFlagIdx = osRec?(osRec->getTdataOn()<pcr->getT()?2:1):0;
//              str.sprintf("PCAL:  %6d %s  PC_%03d  %8.3f  %10.5f    %s  %s  %s", 
              str.sprintf("PCAL:  %6d %s  PS_%03d  %8.3f  %10.5f    %s  %s  %s", 
//                scanIdx, qPrintable(pcr->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)),
                epochIdx, qPrintable(str2),
                idx, vAmp, vPhs,
//              qPrintable(srcName.leftJustified(8, ' ')),
                qPrintable(str4AntCal(srcName, 8)),
                qPrintable(str4AntCal(scnName, 9)),
//              qPrintable(srcName4AntCal(srcName)),
//              qPrintable(scnName.leftJustified(9, ' ')),
                qPrintable(dataFlag[dataFlagIdx]));
              s << str << "\n";
            };
          };
        };
      };
      s << "#\n";
      numByEpoch.clear();
    };
    //
    //
    if (outputData & AOD_FMTGPS)
    {
      // header:
//    s << "#\nNUM_FMT2GPS_TIMER: " << channelSetup_.xpsIdByKey().size() << "\n";
      s << "#\nDIMENSION: FMT2GPS_TIMER " << channelSetup_.xpsIdByKey().size() << "\n";
      s << "#                 Timer Board\n";
      s << "#                   tag\n";
      s << "#\n";
      int                       idx=1;
      for (QMap<QString, QString>::const_iterator it=channelSetup_.xpsIdByKey().begin();
        it!=channelSetup_.xpsIdByKey().end(); ++it)
      {
        QString                 id(it.value());
        str.sprintf("FMT2GPS_TIMER:  TMR_%03d   %s",
          idx, qPrintable(id) );
        s << str << "\n";
        idx++;
      };

      numOfRecs = numOfEpochs = numOfSensors = 0;
      // calc the numbers:
      numOfSensors = channelSetup_.xpsIdByKey().size();
      numByEpoch.clear();
      for (int i=0; i<dot2xpses_.size(); i++)
      {
        SgDot2xpsReading       *d2r=dot2xpses_.at(i);
        if (reportAllReadings || d2r->getOsRec())
        {
          for (QMap<QString, QString>::const_iterator it=channelSetup_.xpsIdByKey().begin();
            it!=channelSetup_.xpsIdByKey().end(); ++it)
            if (!supressNonUsedSensors || d2r->getDot2gpsByBrd().contains(it.value()))
            {
              numByEpoch[d2r->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
              numOfRecs++;
            };
        };
      };
      numOfEpochs = numByEpoch.size();

      s << "#\nDIMENSION: FMTGPS " << numOfEpochs << " " << numOfSensors << "\n";
      s << "NUM_RECORDS: FMTGPS " << numOfRecs << "\n";
      s << "#\n";
      s << "#          Epoch     UTC_Time_tag         Timer   Formatter minus     Formatter minus    Source    Scan\n";
      s << "#          Idx                              tag       GPS time            PPS time\n";
      s << "#               YYYY.DD.MM-hh:mm:ss.ff                 s                   s\n";
      s << "#\n";
      for (int i=0; i<dot2xpses_.size(); i++)
      {
        SgDot2xpsReading       *d2r=dot2xpses_.at(i);
        //
        // src & scan names:
        srcName = scnName = sFiller_;
        scanIdx = nFiller_;
        const SgOnSourceRecord *osRec=d2r->getOsRec();
        if (osRec)
        {
          srcName = osRec->getSourceName();
          scnName = osRec->getScanName();
          scanIdx = osRec->getIdx() + 1;
        };
        //
        if (reportAllReadings || osRec)
        {
          int                   idx=1;
          for (QMap<QString, QString>::const_iterator it=channelSetup_.xpsIdByKey().begin();
            it!=channelSetup_.xpsIdByKey().end(); ++it, idx++)
          {
            float               v1, v2;
            v1 = v2 = dFiller_;
            hasSensor = false;
            if (d2r->getDot2gpsByBrd().contains(it.value()))
            {
              v1 = d2r->getDot2gpsByBrd().value(it.value());
              v2 = d2r->getDot2ppsByBrd().value(it.value());
              hasSensor = true;
            };
            if (!supressNonUsedSensors || hasSensor)
            {
              str.sprintf("FMTGPS:  %6d %s  TMR_%03d  %16.9e    %16.9e    %s  %s", 
                scanIdx, qPrintable(d2r->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)),
                idx, v1, v2,
//              qPrintable(srcName.leftJustified(8, ' ')),
                qPrintable(str4AntCal(srcName, 8)),
                qPrintable(str4AntCal(scnName, 9)));
//              qPrintable(srcName4AntCal(srcName)),
//              qPrintable(scnName.leftJustified(9, ' ')) );
              s << str << "\n";
            };
          };
        };
      };
      s << "#\n";
    };
    //
    //
    if (outputData & AOD_DBBC3TP)
    {
      // header:
      s << "#\n#\n";
//      export2antCalFrqs_dbbc3Setup(s);
      export2antCalFrqs_vgosSetup(s);

      // calc the numbers:
      numOfRecs = numOfEpochs = numOfSensors = 0;
      numOfSensors = channelSetup_.ifIdByTpSensorKey().size();
      numByEpoch.clear();
      for (int i=0; i<dbbc3Tps_.size(); i++)
      {
        SgDbbc3TpReading       *rec=dbbc3Tps_.at(i);
        if (reportAllReadings || rec->getOsRec())
        {
          for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
            it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
          {
            QString             bbcId(it.value());
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
            QString             sbId(it.value().at(it.value().size() - 1));
#else
            QString             sbId(it.value().back());
#endif
            bbcId.chop(1);
            if (!supressNonUsedSensors || rec->getTpBySensor().contains(bbcId))
            {
              numByEpoch[rec->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
              numOfRecs++;
            };
          };
        };
      };
      numOfEpochs = numByEpoch.size();
      //
      //
//    s << "#\nNUM_TPC: " << numOfRecs << "\n";
//      s << "#\nDIMENSION: TPC " << numOfRecs << "\n";
      s << "#\nDIMENSION: TPC " << numOfEpochs << " " << numOfSensors << "\n";
      s << "NUM_RECORDS: TPC " << numOfRecs << "\n";
      s << "#\n";
      s << "#    EpocIdx UTC_Time_tag            Sensor      Gain    TpCalOn   TpCalOff      SEFD      Azimuth   Elevat   Source    Scan       DataScopeFlag\n";
      s << "#            YYYY.DD.MM-hh:mm:ss.ff    tag                                        Jy         deg      deg\n";
      s << "#\n";
      //
      for (int i=0; i<dbbc3Tps_.size(); i++)
      {
        SgDbbc3TpReading       *rec=dbbc3Tps_.at(i);
        el = az = dFiller_;
        //
        // check for az/el info:
        // ...
        //
        // src & scan names:
        srcName = scnName = sFiller_;
        scanIdx = epochIdx = nFiller_;
        const SgOnSourceRecord *osRec=rec->getOsRec();
        if (osRec)
        {
          srcName = osRec->getSourceName();
          scnName = osRec->getScanName();
          scanIdx = osRec->getIdx() + 1;
        };
        //
        if (reportAllReadings || osRec)
        {
          int                   idx=1;
          for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
            it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
          {
            double              vGain, vOn, vOf, vSefd;
            vGain = vOn = vOf = vSefd = dFiller_;
            QString             bbcId(it.value());
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
            QString             sbId(it.value().at(it.value().size() - 1));
#else
            QString             sbId(it.value().back());
#endif
            bbcId.chop(1);
            hasSensor = false;
            if (rec->getTpBySensor().contains(bbcId))
            {
              if (sbId == "l")
              {
                vGain = rec->getTpBySensor().value(bbcId).gainL_;
                vOn   = rec->getTpBySensor().value(bbcId).tpOnL_;
                vOf   = rec->getTpBySensor().value(bbcId).tpOffL_;
                vSefd = rec->getTpBySensor().value(bbcId).sefdL_;
              }
              else if (sbId == "u")
              {
                vGain = rec->getTpBySensor().value(bbcId).gainU_;
                vOn   = rec->getTpBySensor().value(bbcId).tpOnU_;
                vOf   = rec->getTpBySensor().value(bbcId).tpOffU_;
                vSefd = rec->getTpBySensor().value(bbcId).sefdU_;
              }
              else
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::createAntCalFile(): station " + stnKey + 
                  ": got an unknown sideband id: \"" + sbId + "\", IF id is \"" +
                  it.value() + "\"");
              hasSensor = true;
            };
            if (!supressNonUsedSensors || hasSensor)
            {
              str2 = rec->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
              if (numByEpoch.contains(str2))
                epochIdx = numByEpoch.value(str2);

              dataFlagIdx = osRec?(osRec->getTdataOn()<rec->getT()?2:1):0;
//              str.sprintf("TPC:  %6d %s  TS_%03d  %8.1f %10.1f %10.1f %10.1f   %9.4f %8.4f  %s  %s  %s", 
              str.sprintf("TPC:  %6d %s  TP_%03d  %8.1f %10.1f %10.1f %10.1f   %9.4f %8.4f  %s  %s  %s", 
//                scanIdx, qPrintable(rec->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)),
                epochIdx, qPrintable(str2),
                idx, vGain, vOn, vOf, vSefd, az, el,
//              qPrintable(srcName.leftJustified(8, ' ')),
                qPrintable(str4AntCal(srcName, 8)),
                qPrintable(str4AntCal(scnName, 9)),
//              qPrintable(srcName4AntCal(srcName)),
//              qPrintable(scnName.leftJustified(9, ' ')),
                qPrintable(dataFlag[dataFlagIdx])
                );
              s << str << "\n";
            };
          };
        };
      };
      s << "#\n";
      numByEpoch.clear();
    };
    //
    //
    if (outputData & AOD_SEFD)
    {
      // header:
      s << "#\n#\n";
//      export2antCalFrqs_dbbc3Setup(s);
      export2antCalFrqs_vgosSetup(s);
      
      // calc the numbers:
      numOfRecs = numOfEpochs = numOfSensors = 0;
      numOfSensors = channelSetup_.ifIdByTpSensorKey().size();
      numByEpoch.clear();
      for (int i=0; i<sefds_.size(); i++)
      {
        SgSefdReading          *sefd=sefds_.at(i);
        for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
          it!=channelSetup_.ifIdByTpSensorKey().end(); ++it)
          if (!supressNonUsedSensors || sefd->getValByChanKey().contains(it.key()))
          {
            numByEpoch[sefd->getT().toString(SgMJD::F_SOLVE_SPLFL_V3)] = numByEpoch.size();
            numOfRecs++;
          };
      };
      numOfEpochs = numByEpoch.size();
      //
      //
//    s << "#\nNUM_SEFD: " << sefds_.size() << "\n#\n";
//      s << "#\nDIMENSION: SEFD " << sefds_.size() << "\n#\n";
      s << "#\nDIMENSION: SEFD " << numOfEpochs << " " << numOfSensors << "\n";
      s << "NUM_RECORDS: SEFD " << numOfRecs << "\n#\n";
      s << "# some documentation about OnOff measurements can be found in FS sources, fs-master/onoff/onoff.txt\n";
      s << "# and fs-master/onoff/gain.txt\n";
      s << "#\n";
      s << "#     EpocIdx       UTC_Time_tag       Sensor      SEFD        Tsys       Tcal       Trat       Gain      Az        El    Source\n";
      s << "#              YYYY.DD.MM-hh:mm:ss.ff     tag       Jy          K         Jy                 Compress     deg       deg\n";
      s << "#\n";
      //
      for (int i=0; i<sefds_.size(); i++)
      {
        SgSefdReading          *sefd=sefds_.at(i);
        az = sefd->getAz();
        el = sefd->getEl();
        srcName = sefd->getSrcName();
        //
        int                   idx=1;
        for (QMap<QString, QString>::const_iterator it=channelSetup_.ifIdByTpSensorKey().begin();
          it!=channelSetup_.ifIdByTpSensorKey().end(); ++it, idx++)
        {
          QString             sensorKey(it.key());
          double              v2, v3, v4, v5, v6;
          v2 = v3 = v4 = v5 = v6 = dFiller_;
          epochIdx = nFiller_;
          hasSensor = false;
          if (sefd->getValByChanKey().contains(sensorKey))
          {
            const QVector<double>
                               *vec=sefd->getValByChanKey().value(sensorKey);
            v2 = vec->at(1);
            v3 = vec->at(2);
            v4 = vec->at(3);
            v5 = vec->at(4);
            v6 = vec->at(5);
            hasSensor = true;
          }
          else
            logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
              "::createAntCalFile(): station " + stnKey + 
              ": the sensor key \"" + sensorKey + "\" is not found in the SEFD map");
          if (!supressNonUsedSensors || hasSensor)
          {
            str2 = sefd->getT().toString(SgMJD::F_SOLVE_SPLFL_V3);
            if (numByEpoch.contains(str2))
              epochIdx = numByEpoch.value(str2);
            str.sprintf("SEFD:  %6d  %s  TP_%03d  %10.3f %10.3f %10.3f %10.3f %10.3f  %9.4f %9.4f  %s", 
              epochIdx, qPrintable(str2), idx,
              v4, v3, v5, v6, v2, az, el, qPrintable(srcName));
            s << str << "\n";
          };
        };
      };
      s << "#\n";
      numByEpoch.clear();
    };
    //
    // end of output
    //
    s << "#\n";
    s << "#\n";
    s << "# End of file\n";
    compressors.closeFlt(pipe, f, s);
  }
  else
    isOk = false;

  return isOk;
};
/*=====================================================================================================*/








/*=======================================================================================================
*
*                          SgStnLogCollector's METHODS:
* 
*======================================================================================================*/
const QRegularExpression SgStnLogCollector::reFsOld_("^\\d{9}\\D+");
const QRegularExpression SgStnLogCollector::reFsNew_("^\\d{13}\\D+");
//const QRegularExpression SgStnLogCollector::reFsNewest_("^(\\d{4})\\.(\\d{3})\\.(\\d{2}):(\\d{2})[:\\s]"
//"(\\d{2}\\.\\d{1,3})\\D+");
const QRegularExpression SgStnLogCollector::reFsNewest_("^(\\d{4})\\.([0-9\\s]{3})\\.(\\d{2}):(\\d{2})[:\\s]"
  "(\\d{2}\\.?\\d{0,3})\\D+");
const QRegularExpression SgStnLogCollector::rePcmt_("^\\s*\\d{4}\\s+\\d{1,2}\\s+\\d{1,2}\\s+\\d{1,2}\\s+"
  "\\d{1,2}\\s+\\d{1,2}\\s+[\\d\\.eEdD+-]{1,}\\s+[\\dA-Z+-]{2,}\\s+\\d{3}-\\d{4}[a-z]?\\s*$");
const QRegularExpression SgStnLogCollector::reMet_("^\\s*\\d{2,4}\\s+\\d{1,2}\\s+\\d{1,2}\\s+\\"
  "d{1,2}\\s+\\d{1,2}\\s+\\d{1,2}\\s+[+-]?[\\d\\.]{1,}\\s+[+-]?[\\d\\.]{1,}\\s+[+-]?[\\d\\.]{1,}\\s*$");


//
// static first:
//
const QString SgStnLogCollector::className()
{
  return "SgStnLogCollector";
};



// An empty constructor:
SgStnLogCollector::SgStnLogCollector() :
  stationName_(""),
  logReadings_(),
  inputLogFileName_(""),
  userCommand_("")
{
  antcalOutputData_ = AOD_ALL;
  fsFmt_ = FSEF_UNDEF;
  inYear_ = 0;
  defaultCableSignByStn_ = NULL;
  rinexFileNameByStn_ = NULL;
  rinexPressureOffsetByStn_ = NULL;
  hasCableRecord_ = hasCdmsRecord_ = hasTsysRecord_= false;
  useSkededChannelSetup_ = false;
  channelSkeded_ = NULL;
};



//
// A destructor:
SgStnLogCollector::~SgStnLogCollector()
{
  for (QMap<QString, Procedure*>::iterator it=procedureByName_.begin(); 
    it!=procedureByName_.end(); ++it)
    delete it.value();
  procedureByName_.clear();
};



//
bool SgStnLogCollector::readLogFile(const QString& fileName, const QString& stnName,
  const SgMJD& tFirst, const SgMJD& tLast, const QString& orderOfMeteo)
{
  stationName_ = stnName; // simplified name, no spaces
  if (!stationName_.size())
    stationName_ = "n/a";

  hasCableRecord_ = hasCdmsRecord_ = hasTsysRecord_ = false;
  inputLogFileName_ = fileName;

  QString                       str("");
  QString                       strTmp("");
  QString                       sScanName("");
  QString                       sSourceName("");
  QString                       versionStr("");
  QFile                         f(fileName);
  FILE                         *pipe=NULL;
  SgMJD                         t, lastReadTsys(tZero), lastReadPcal(tZero);
  SgMJD                         lastReadDot2xps(tZero), lastReadSefd(tZero);
  SgMJD                         tRightMargin(tLast + 5.0/24.0/60.0); //  5 min
  SgMJD                         tAtTheEnd(tLast - 75.0/24.0/60.0);   // 75 min
  SgMJD                         tAtTheBegin(tFirst - 7.0/24.0/60.0); //  7 min
  SgOnSourceRecord             *osRec;
  bool                          isDataOn, isOnSource, reachedEndOfSession, hasStnName;
  int                           oom[3];
  hasStnName = (0 < stnName.size());

  // define meteo parameters order:
  // natural order:
  oom[0] = 0;
  oom[1] = 1;
  oom[2] = 2;
  //
  //
  if (orderOfMeteo.size())
  {
    str = "";
    for (int i=0; i<orderOfMeteo.size(); i++)
    {
      if (orderOfMeteo.at(i) == 'T')
      {
        oom[0] = i;
        str += "T";
      }
      else if (orderOfMeteo.at(i) == 'P')
      {
        oom[1] = i;
        str += "P";
      }
      else if (orderOfMeteo.at(i) == 'H')
      {
        oom[2] = i;
        str += "H";
      }
    };
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      "::readLogFile(): the order of meteoparamters is set to \"" + str + "\"");
  };


// /500/sessions/2001/cb901/cb90145.log:2001.127.20:06:01.66:"data_valid=on

//QRegularExpression            reOnSource(":preob|#trakl#\\s*Source acquired|#flagr#flagr/antenna,acquired|/onsource/TRACKING", 
// "/onsource/TRACKING" occured between data_valid=on and data_valid=off too (e.g., r1980ht.log)

//  QRegularExpression            reOnSource(":preob|#trakl#\\s*Source acquired|"
//                                  "#flagr#flagr/antenna,acquired|&preob/onsource|&preob/track|/onsource",
//                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reOnSource(":preob|#trakl#\\s*Source acquired|"
                                  "#flagr#flagr/antenna,acquired|&preob[a-z0-9+_-]*/onsource|&preob/track",
                                  QRegularExpression::CaseInsensitiveOption);
//QRegularExpression            reDataOn(":data_valid=on|data start\\W+|:midob",
  QRegularExpression            reDataOn("(:|/)data_valid=on|data start\\W+|:midob",
                                  QRegularExpression::CaseInsensitiveOption);
//QRegularExpression            reDataOff(":data_valid=off|data stop\\W+|;terminate|;halt|:postob",
  QRegularExpression            reDataOff("(:|/)data_valid=off|data stop\\W+|;terminate|;halt|:postob",
                                  QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reTapeOn("/tape\\d?/.*moving,.*(?:nostop|locked)",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reTapeOff("/tape\\d?/.*(?:stopped,.*unlocked|moving,.*stop|"
                                  "stopped,.*stop)", QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reScanName(":scan_name=([_a-zA-Z0-9+-]{1,}),([a-zA-Z0-9_+-]+),?.*",
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reSourceName(":source=([\\.a-zA-Z0-9_+-]{1,10}),.*",
                                  QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reAzElFromSourceName(":source=azel,(.*)",
                                  QRegularExpression::CaseInsensitiveOption);
//:source=azel,-89.50000d,30.00000d,,ccw

  QRegularExpression            reMeteo("(#wx#/WX/|/?wx/|DSNWX/|wx/weather:)([^,]+),([^,]+),([^,]+).*",
                                  QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reCableCal("(/cable/|cableget/)(.+)", 
                                  QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reCableLong("(/cablelong/)(.+)", 
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reVersion("(mark\\s+.+\\s+field system\\s+v\\w*\\s*)(.+)", 
                                  QRegularExpression::CaseInsensitiveOption);

//2010.140.18:22:06.70/cablediff/683.3e-6,+
  QRegularExpression            reCableDiff("((?:/|;\"*\\s*)cable\\s*diff(?:/|;*\\s*))([^,]+),([+-])",
                                  QRegularExpression::CaseInsensitiveOption);
//2012.068.18:11:48.08;"cable sign is positive, OK
  QRegularExpression            reCableSign("(c\\w?a\\w?b\\w?l\\w?e\\w?|ca\\w{2,2}e)\\s+sign\\s+is\\s+"
                                  "(positive|negative)", QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reCableDiffIs("cable\\s+dif{1,2}e\\w{2,3}ce\\s+(?:is|was)", 
                                  QRegularExpression::CaseInsensitiveOption);
  QRegularExpression            reOpComments("\\d+;\"(.+)", QRegularExpression::CaseInsensitiveOption);
  //
  //2017    9    11    18    3    46    7.92e-12    254-1803
  QRegularExpression            rePcmt("(\\d{4})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                       "(\\d{1,2})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                       "(.+)\\s+([\\dA-Z+-]{2,8})\\s+([0-9a-z-]{0,}).*",
                                  QRegularExpression::CaseInsensitiveOption);
//  2017    09    11    18    05    8    -2.8e-12    1300+580    254-1805
//  2017    09    11    18    06    16    -1.765e-11    NRAO512    254-1806
  QRegularExpression            reMet("(\\d{2,4})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                      "(\\d{1,2})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                      "([\\d\\.+-]+)\\s+([\\d\\.+-]+)\\s+([\\d\\.+-]+).*",
                                  QRegularExpression::CaseInsensitiveOption);

  QRegularExpression            reCdms("([/#]CDMS/)(.+)", QRegularExpression::CaseInsensitiveOption);

  // tsys:
  //2019.007.19:06:32.52/tsys/1l,62.2,1u,65.6,2u,56.4,3u,68.0,4u,61.6,ia,60.4
  //2019.007.19:06:32.52/tsys/5u,66.5,6u,58.4,7u,61.2,8l,59.1,8u,58.6,ib,55.0
  //2019.007.19:06:32.52/tsys/9u,89.3,au,85.7,bu,51.5,cu,92.3,ic,82.8
  //2019.007.19:06:32.52/tsys/du,118.7,eu,168.7,id,46.5
  QRegularExpression            reTsys("([/#]tsys/)([^,]+),([^,]+).*",
                                  QRegularExpression::CaseInsensitiveOption);

  //2020.021.18:00:00.15#rdtca#tsys/ 00a0,217.6, 01a0,126.9, 02a0,123.6, 03a0,106.4, 04a0, 95.2, 05a0, 92.7, 06a0, 98.3, 07a0,119.9
  //2020.021.18:00:00.15#rdtca#tsys/ 08a0, 50.6, 09a0, 84.7, 10a0, 87.4, 11a0,118.1, 12a0,$$$$$, 13a0, 86.5, 14a0, 80.5, 15a0, 79.4
  //2020.021.18:00:00.15#rdtca#tsys/ AVa0,101.5, SMa0,233.4
  //2020.021.18:00:00.15#rdtca#tsys/ 00a1, 80.2, 01a1, 51.4, 02a1, 47.8, 03a1, 45.7, 04a1, 46.4, 05a1, 46.5, 06a1, 44.2, 07a1, 52.0
  //2020.021.18:00:00.15#rdtca#tsys/ 08a1, 35.5, 09a1, 46.0, 10a1, 43.8, 11a1, 43.8, 12a1, 60.6, 13a1, 44.5, 14a1, 48.7, 15a1, 49.4
  //2020.021.18:00:00.15#rdtca#tsys/ AVa1, 47.8, SMa1, 47.5
  /*
  */
  QRegularExpression            reTsysVgos("(#rdtc\\w#tsys/ )([^,]+),([^,]+).*");
  QRegularExpression            reTsysCdas("(#tsys/)([^,]+),([^,]+).*");


  //2020.021.18:00:00.17#rdtcd#tpcont/ 00d0,  12501,  12325, 01d0,  16054,  15765, 02d0,  20774,  20410, 03d0,  24388,  23985, 04d0,  27599,  27153
  //2020.021.18:00:00.17#rdtcd#tpcont/ 05d0,  28085,  27647, 06d0,  29475,  29027, 07d0,  30963,  30479, 08d0,  29392,  28953, 09d0,  30078,  29643
  //2020.021.18:00:00.17#rdtcd#tpcont/ 10d0,  37494,  36960, 11d0,  42335,  41705, 12d0,  46499,  45802, 13d0,  51354,  50535, 14d0,  49839,  48925
  //2020.021.18:00:00.17#rdtcd#tpcont/ 15d0,  43153,  42272
  QRegularExpression            reTpcontVgos("(#rdtc\\w#tpcont/ )([^,]+),([^,]+).*");

  
  //2022.347.15:11:13.04&setmode_m08/rdbea=pcal=1.400e6;
  //2022.347.15:11:13.04&setmode_m08/rdbeb=pcal=1.400e6;
  //2022.347.15:11:13.04&setmode_m08/rdbec=pcal=4.400e6;
  //2022.347.15:11:13.04&setmode_m08/rdbed=pcal=2.400e6;
  //
  //2022.322.23:30:09.85&rdbebb/rdbe=pcal=1.4e6;
  
  QRegularExpression            rePcalOffset("&.+/rdbe([a-z0-9]*)=pcal=([0-9\\.eEfFdD+-]+).*");

  //2020.195.11:30:01.17#rdtca#pcal/ 1a0000,   8.686,  -98.7, 1a0005,  78.913,  116.7, 1a0010,   8.729,  147.9, 1a0015,  81.273,    4.1
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0020,   9.421,   37.5, 1a0025,  88.580, -105.5, 1a0030,  10.177,  -72.2, 1a0035,  97.212,  144.1
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0040,  11.646,  176.3, 1a0045,  89.248,   33.1, 1a0050,  10.577,   65.5, 1a0055,  93.330,  -79.2
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0060,  10.255,  -47.4, 1a0065,  93.203,  169.1, 1a0070,  10.437, -155.9, 1a0075,  89.414,   59.5
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0080,  10.014,   92.8, 1a0085,  84.492,  -47.7, 1a0090,   9.561,  -14.2, 1a0095,  82.770, -155.3
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0100,   8.782, -120.1, 1a0105,  79.763,  101.6, 1a0110,   8.836,  137.8, 1a0115,  77.939,   -3.8
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0120,   8.740,   32.5, 1a0125,  73.421, -105.2, 1a0130,   8.134,  -68.7, 1a0135,  74.094,  150.8
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0140,   7.557, -168.7, 1a0145,  69.966,   50.6, 1a0150,   7.977,   88.8, 1a0155,  72.224,  -54.9
  QRegularExpression            rePcalVgos("(#rdtc\\w#pcal/ )([^,]+),([^,]+).*");
 
  //2020.034.17:11:35.51#onoff#    source       Az   El  De   I P   Center   Comp   Tsys  SEFD  Tcal(j) Tcal(r)
  //2020.034.17:11:35.51#onoff#VAL casa       295.4 63.4 001l 1 l   3464.40 0.9025 62.33 2139.2  58.787  0.93
  //2020.034.17:11:35.51#onoff#VAL casa       295.4 63.4 002l 1 l   3432.40 0.8950 63.19 2407.4  65.123  1.03
  //2020.034.17:11:35.51#onoff#VAL casa       295.4 63.4 003l 1 l   3368.40 0.9119 72.97 2638.0  73.812  0.98
  // for adults:
  QRegularExpression            reSefd("#onoff#VAL\\s+([\\.\\d\\w+-]+)\\s+([\\.\\d+-]+)\\s+"
    "([\\.\\d+-]+)\\s+([\\w\\d]+)\\s+([\\w\\d]+)\\s+([\\w\\d]+)\\s+([\\.\\d+-]+)\\s+([\\.\\d+-]+)\\s+"
    "([\\.\\d+-]+)\\s+([\\.\\d+-]+)\\s+([\\.\\d+-]+)\\s+([\\.\\d+-]+).*");
 
 
  //2020.021.18:00:00.15#rdtca#dot2pps/-1.953124995e-08
  //2020.021.18:00:00.15#rdtca#dot2gps/-1.073085937e-04
  //QRegularExpression          reDot2xpsVgos("(#rdtc\\w#dot2(?:g|p)ps/)([^,]+),([^,]+).*");
  //QRegularExpression          reDot2xpsVgos("#rdtc(\\w)#dot2([gp])ps/\\s*([0-9eEdD\\.+-]+).*");
//->last QRegularExpression     reDot2gpsVgos("#rdtc(\\w)#dot2gps/\\s*([0-9eEdD\\.+-]+).*");
  QRegularExpression            reDot2xpsVgos("#rdtc(\\w)#dot2(g|p)ps/\\s*([0-9eEdD\\.+-]+).*");

 
  //2019.003.18:05:50.86&vcsx8/vc01=132.99,8.000,ul
  //2019.003.18:05:50.86&vcsx8/vc02=172.99,8.000,u
  QRegularExpression            reVc("(/vc)([0-9]{2})=([0-9\\.]+),([^,]+).*");
  
  //2019.003.18:05:52.80&ifdsx/patch=lo1,1l,2l,3h,4h
  //2019.003.18:05:52.80&ifdsx/patch=lo2,9l,10h,11h,12h,13h,14h
  //2018.094.17:20:22.65&ifdsx/patch=lo1,a1,a2,a3,a4
  //2018.094.17:20:22.65&ifdsx/patch=lo2,b1,b2,b3,b4,b5,b6
  //2018.094.17:20:22.65&ifdsx/patch=lo3,a5,a6,a7,a8
  QRegularExpression            rePatch("([&].+/\"?patch=lo)([^,]+),([^,]+).*");
  //2018.003.17:14:29.55&ifdsx/lo=loa,8080.00,usb,rcp,1
  QRegularExpression            reLo("([&].+/\"?lo=lo)([^,]+),([0-9\\.]+),(\\w)+,(\\w)+.*");

  //2021.190.19:42:45.43/lo/rxg,loa0,2152.4,lcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.190.19:42:45.43/lo/rxg,loa1,2152.4,rcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.315.17:00:25.44/lo/rxg,loa,0,lcp,ottn.rxg,range,0,14000,2021,3,3,lcp,2.50000e-02,rcp,2.50000e-02
  QRegularExpression            reLoRxg("[\\d](/lo/rxg,\\s*lo)([^,]+),\\s*([a-zA-Z0-9\\.+-]+),\\s*(\\w)+,\\s*(\\w)+.*");

  //2025.336.12:38:02.16&ifdbb/udc=a,2472.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=b,4712.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=c,5832.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=d,9672.4,20,20,1
  QRegularExpression            reLoCdasUdc("([&].+/\"?udc=)([^,]+),([0-9\\.]+),(\\w)+,(\\w)+.*");


  //2016.236.17:21:21.92&setupsx/"channel  sky freq  lo freq  video
  //2016.236.17:21:21.92&setupsx/"    01   8210.99   8080.00   130.99
  //2016.236.17:21:21.92&setupsx/"    02   8220.99   8080.00   140.99
  //2016.236.17:21:21.92&setupsx/"    03   8250.99   8080.00   170.99
  //2016.236.17:21:21.92&setupsx/"    04   8310.99   8080.00   230.99
  //2016.236.17:21:21.92&setupsx/"    05   8420.99   8080.00   340.99
  //2016.236.17:21:21.92&setupsx/"    06   8500.99   8080.00   420.99
  //2016.236.17:21:21.92&setupsx/"    07   8550.99   8080.00   470.99
  //2016.236.17:21:21.92&setupsx/"    08   8570.99   8080.00   490.99
  //2016.236.17:21:21.92&setupsx/"    09   2212.99   2020.00   192.99
  //2016.236.17:21:21.92&setupsx/"    10   2227.99   2020.00   207.99
  //2016.236.17:21:21.92&setupsx/"    11   2237.99   2020.00   217.99
  //2016.236.17:21:21.92&setupsx/"    12   2267.99   2020.00   247.99
  //2016.236.17:21:21.92&setupsx/"    13   2287.99   2020.00   267.99
  //2016.236.17:21:21.92&setupsx/"    14   2292.99   2020.00   272.99
  QRegularExpression            reChanSkyFreq("&setup.*/\"channel\\s+sky\\s+freq.*");
  QRegularExpression            reChanSkyFreqValues("&setup.*/\"\\s+([0-9]+)\\s+([0-9\\.]+)\\s+"
                                  "([0-9\\.]+)\\s+([0-9\\.]+).*");
 
  //2016.236.17:21:21.92&setupsx/vci01=130.99,4r
  //2016.236.17:21:21.92&setupsx/vci02=140.99,4r
  //2016.236.17:21:21.92&setupsx/vci14=272.99,4r
  //2016.236.17:21:21.92&setupsx/vci15=272.99,4r
  //2016.236.17:21:21.92&setupsx/vci16=272.99,4r
  QRegularExpression            reVci("(/\"?vci)([0-9]{2})=([0-9\\.]+),([^,]+).*");


  //2019.003.19:31:02.40&bbcsx8/bbc01=612.99,a,8.000,8.000
  //2019.070.21:51:37.16&dbbcsx8/bbc01=612.99,a,8.00
  //2019.344.18:07:19.78&dbbc_bb/dbbc3=dbbc01=3480.4,a,32,1
  //QRegularExpression            reBbc("([&]d?bbc.+(?:/|=d)bbc)([0-9]+)=([0-9\\.]+),([^,]+).*");
  //2021.315.17:00:17.91&dbbcbb32/bbc041=859.6,f,32.0
  //2021.271.20:19:39.04&freq_001/bbc01=612.99,a,8.000,8.000
  QRegularExpression            reBbc("([&].+(?:/|=d)bbc)([0-9]+)=([0-9\\.]+),([^,]+).*");

  //2020.202.18:01:44.03/dbbc3/dbbc001/ 3480.400000,a,32,1,agc,79,84,15299,15290,14977,14972;
  QRegularExpression            reDbbc3Tp("/dbbc3/dbbc(\\d{3})/\\s*([0-9\\.+-]+),([^,]+),(\\d+),(\\d+),"
                                          "(\\w+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+);.*");

  // /dbbc3/version/
  // /dbbc3/pps_delay/

  //2020.203.18:21:45.11#trakl#[az   10.0149 el  50.0437 azv   0.00000000 elv   0.00000000 mjd 59051 s 66104 ms 980 ]
  QRegularExpression            reTrakl("#trakl#\\[az\\s+([\\.0-9+-]+)\\s+el\\s+([\\.0-9+-]+)\\s+azv"
                                        "\\s+([\\.0-9+-]+)\\s+elv\\s+([\\.0-9+-]+)\\s+mjd\\s+"
                                        "([\\.0-9+-]+)\\s+s\\s+([\\.0-9+-]+)\\s+ms\\s+"
                                        "([\\.0-9+-]+)\\s*\\]");
  // catch a procedure:
  QRegularExpression            reProcedureDecl("[\\.0-9+-]{2}:\"%\\s+define\\s+(\\S+).*");

  QRegularExpression            reEndOfSchd(":(sched_end|\\*end of schedule).*");

  //2015.182.16:40:02.73;proc=t2104ke
  QRegularExpression            reProc(";proc=(\\S+)");

  // FS 10:
  //2021.315.16:59:18.48;equip1,dbbc3_ddc_v,flexbuff,none,none,500.10,3,a/d,101,60,20,none,40,1,in,8bit,cdp,3,return
  //2021.315.16:59:18.48;equip2,v107,v15_1,1,1,1,1,15000,15000,15000,15000,128,vsi1-2
  //2021.315.16:59:18.48;dbbc3,8,8,v125,v124,57,1
  QRegularExpression            reEquip("\\d{2}.equip1{0,1},([^,]+),([^,]+),(.+)");
//QRegularExpression            reEquip("\\d{2}.equip,([^,]+),([^,]+),(.+)");
  QRegularExpression            reEquipCdas("cdas2bb/chsel"); // they do not use "equip" yet
  
  //2025.067.18:48:00.03:" Created with  /opt64/bin/sds_to_snap.py gsx116.sds gsx116.snp gsx116.prc
  QRegularExpression            reSdePattern("\"\\s+Created with\\s+\\S+/sds_to_snap.py\\s+\\S+.sds\\s+\\S+.snp\\s+\\S+.prc");
  
  bool                          hasProc;
  QString                       procName("");
  bool                          hasEquip;
  QString                       backEndName("");
  QString                       recorderName("");
  
  bool                          hasProcedures;
  bool                          isProcedureContentMode;
  QMap<QString, QString>        procedureNameByName;
  QList<QString>                procedureContent;
  QRegularExpression            reProcedures("");
  QRegularExpression            reProcedureContent("");
  QString                       currentProcedureName("");
  QString                       currentProcedureTimeStamp("");
  
  bool                          wantTsys(antcalOutputData_ & AOD_TSYS);
  bool                          wantTpi (antcalOutputData_ & AOD_TPI);
  bool                          wantPcal(antcalOutputData_ & AOD_PCAL);
  bool                          wantFmt (antcalOutputData_ & AOD_FMTGPS);

  // what to skip:
  if (antcalOutputData_ & AOD_NOVEL)
  {
    str = "#trakl#(?!\\[az)|#rdtc.#(?!";
    if (wantTsys)
      str += "tsys|";
    if (wantTpi)
      str += "tpcont|";
    if (wantPcal)
      str += "pcal|";
    if (wantFmt)
//    str += "dot2(:?g|p)ps";
      str += "dot2(?:g|p)ps";
    str.chop(1);
    str += ")|#antcn#(?!/wx)|\\d[:/]mk6a[=/]/";
  }
  else
    str = "#trakl#(?!\\[az)|#rdtc.#|#antcn#(?!/wx)|\\d[:/]mk6a[=/]/";


  QRegularExpression            re2skip(str);


//2020.174.13:46:04.25;location,MGO12M  ,104.02,30.68,1890.6
  QRegularExpression            reLocation(";location,(\\S{1,8})\\s*,\\s*([-+\\.\\d]+)\\s*,"
    "\\s*([-+\\.\\d]+)\\s*,\\s*([-+\\.\\d]+).*", QRegularExpression::CaseInsensitiveOption);
  bool                          hasLocation=false;

//2010.349.17:30:36.87:" APSG27    2010 URUMQI   U Ur
//2020.201.05:08:53.14:" VO0202    2020 ONSA13NE 1 Oe
  QRegularExpression            reStnId("\\d:\"\\s+([A-Z0-9]{1,6})\\s+([0-9]{4})\\s+"
    "([_A-Z0-9-]{2,8})\\s+([A-Z0-9])\\s+([A-Z0-9][a-z0-9+-])");



  SgTraklReading                trakl;
  bool                          have2checkT;
  bool                          hasVersionStr, hasCableSign, needCableSign;
  int                           val, numOfSkippedStrs, numOfReadStrs, numOfProcedureExpanded;
  int                           numOfTries;
  qint64                        flogSize;
  double                        d;
//bool                          isData, isTape;
  QList<QString>                operatorComments;
  // /500/sessions/2003/r1083/r1083tc.log  ^--??

  QRegularExpressionMatch       match;
  SgStnLogReadings::ContentType contentType=SgStnLogReadings::CT_UNDEF;

  // adjust:
  if ((tLast - tFirst) < 3.0/24.0)
    tAtTheEnd = tLast - 5.0/24.0/60.0; // for short sessions

  
  numOfSkippedStrs = numOfReadStrs = numOfProcedureExpanded = 0;
  flogSize = 0;
  have2checkT = false;
  hasVersionStr = hasCableSign = needCableSign = false;
  hasProc = hasEquip = false;

  if (antcalOutputData_ & (AOD_CABLE_CAL | AOD_CABLE_SIGN))
    needCableSign = true;


//isData = isTape = false;
  numOfTries = 0;
  //
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::readLogFile(): the log file " + fileName + " does not exist");
    return false;
  };
  //
  //
  //
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readLogFile(): station \"" + stationName_ + "\": reading log file " + fileName);
  //
  if (tZero < tFirst)
    inYear_ = tFirst.calcYear();
  //
  if (fileName.right(9).toLower() == ".cdms.dat")
    logReadings_.setItType(SgStnLogReadings::IT_CDMS);
  else if (fileName.right(4).toLower() == ".dat" && fileName.toLower().contains(".pcmt."))
    logReadings_.setItType(SgStnLogReadings::IT_PCMT);
  else if (fileName.right(8).toLower() == ".met.dat")
    logReadings_.setItType(SgStnLogReadings::IT_METE);
  else
    logReadings_.setItType(SgStnLogReadings::IT_FSLG);

  //
  isDataOn = false;
  isOnSource = false;
  reachedEndOfSession = false;
  osRec = new SgOnSourceRecord;
  osRec->setScanName("");
  osRec->setSourceName("");

  hasProcedures = false;
  isProcedureContentMode = false;
  

  flogSize = QFileInfo(fileName).size();
  QTextStream                   s;
  pipe = compressors.openFlt(fileName, f, s, FLTD_Input);
  if (s.device())
  {
    bool                        have2reportNumOfStrings;
    have2reportNumOfStrings = (10000000 < flogSize) && logger->isEligible(SgLogger::INF, SgLogger::IO);
    while (!s.atEnd() && !reachedEndOfSession)
    {
      if (procedureContent.size())
      {
        str = currentProcedureTimeStamp.size()?
          currentProcedureTimeStamp + "&" + currentProcedureName + "/" + procedureContent.takeFirst():
          procedureContent.takeFirst();
        if (procedureContent.size() == 0)
        {
          currentProcedureName = "";
          currentProcedureTimeStamp = "";
        };
      }
      else
      {
        str = s.readLine();
        numOfReadStrs++;
      };

if (have2reportNumOfStrings && numOfReadStrs%100000 == 0)
std::cout << "  -- read " << numOfReadStrs/1000 << "K strings file size: " << flogSize << "\n";
      //
      //
      if (str.right(4) == "\\par")
        str.chop(4);
      // 
      // guess format of the file:
      if ( fsFmt_==FSEF_UNDEF && 
          (fsFmt_=guessFsFormat(str))==FSEF_UNDEF &&
          numOfTries++>10 && str.size())
      {
        logger->write(SgLogger::ERR, SgLogger::IO, className() +
          "::readLogFile(): station " + stationName_ +
          ": cannot determine the time mark format after " + QString("").setNum(numOfTries) + 
          " tries, the log file: " + fileName);
        logger->write(SgLogger::ERR, SgLogger::IO, className() +
          "::readLogFile(): station " + stationName_ +
          ": input: \"" + asciifyString(str) + "\"");
        f.close();
        s.setDevice(NULL);
        return false;
      };
      // 
      // lookup "version" string:
      if (!hasVersionStr && (match=reVersion.match(str)).hasMatch())
      {
        versionStr = match.captured(0);
        logReadings_.setFsVersionLabel(versionStr);
        hasVersionStr = true;
      }; 
      //
      //
      // check for lines to skip:
      if(str.size()==0 || re2skip.match(str).hasMatch())
      {
        numOfSkippedStrs++;
      }
      else if (fsFmt_ == FSEF_PCMT) // simple readings:
      {
        if ((match=rePcmt.match(str)).hasMatch() &&
            extractDataFromPcmtRecord(str, match, t, d, sSourceName, sScanName))
        {
          SgCableCalReading    *cableRec=new SgCableCalReading(t, d);  // input is in seconds
          logReadings_.cableCals() << cableRec;
          
          osRec->tStart() = t;
          osRec->setScanName(sScanName.toLower());
          osRec->setSourceName(sSourceName);
          osRec->cableCals() << cableRec;
         
          if (logReadings_.onSourceRecords().size())
            osRec->tFinis() = logReadings_.onSourceRecords().last()->tStart();

          osRec->setIdx(logReadings_.onSourceRecords().size());
          logReadings_.onSourceRecords() << osRec;
          osRec = new SgOnSourceRecord;
   
          if (tRightMargin <= t && (!needCableSign || hasCableSign) )
          {
            reachedEndOfSession = true;
            logger->write(SgLogger::DBG, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": got to the end of the session: " + 
              tRightMargin.toString(SgMJD::F_Simple) + " <= " + 
              t.toString(SgMJD::F_Simple));
          };
          //
          if (!hasVersionStr)
          {
            versionStr = "PCMT or CDMS file";
            hasVersionStr = true;
          };
        };
      }
      else if (fsFmt_ == FSEF_MET) // simple readings too:
      {
        SgMeteoData             m;
        if ((match=reMet.match(str)).hasMatch() &&
            extractDataFromMetRecord(str, match, t, m))
        {
          SgMeteoReading       *meteoRec=new SgMeteoReading(t, m);
          logReadings_.meteoPars() << meteoRec;
          if (tRightMargin <= t && (!needCableSign || hasCableSign))
          {
            reachedEndOfSession = true;
            logger->write(SgLogger::DBG, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": got to the end of the session: " + 
              tRightMargin.toString(SgMJD::F_Simple) + " <= " +
              t.toString(SgMJD::F_Simple));
          };
          //
          if (!hasVersionStr)
          {
            versionStr = "External file with meteo data";
            hasVersionStr = true;
          };
        };
      }
      else // FS log file:
      {
        //
        // first, deal with macroses:
        if ((match=reProcedureDecl.match(str)).hasMatch())
        {
          hasProcedures = true;
          procedureNameByName.insert(match.captured(1), match.captured(1));
          str = "[\\.0-9]{2}:(";
          for (QMap<QString, QString>::iterator it=procedureNameByName.begin(); 
            it!=procedureNameByName.end(); ++it)
            str += it.key() + "|";
          str.chop(1);
          reProcedures.setPattern(str + ")");
          logger->write(SgLogger::DBG, SgLogger::IO, className() + 
            "::readLogFile(): station " + stationName_ + 
            ": found declaration of procedure \"" + match.captured(1) + "\"");
          continue;
        }
        else if (hasProcedures && (match=reProcedures.match(str)).hasMatch())
        {
          // empty procedure + another one:
          if (isProcedureContentMode && currentProcedureName.size() &&
              procedureByName_.contains(currentProcedureName)     &&
              procedureNameByName.contains(currentProcedureName)   )
          {
            procedureByName_.remove(currentProcedureName);
            procedureNameByName.remove(currentProcedureName);
            // redefine the regExp:
            strTmp = "[\\.0-9]{2}:(";
            for (QMap<QString, QString>::iterator it=procedureNameByName.begin(); 
              it!=procedureNameByName.end(); ++it)
              strTmp += it.key() + "|";
            strTmp.chop(1);
            reProcedures.setPattern(strTmp + ")");
            logger->write(SgLogger::DBG, SgLogger::IO, className() + 
              "::readLogFile(): station " + stationName_ + 
              ": procedure \"" + currentProcedureName + 
              "\" is empty, removed from the substitution pool");
          };
          currentProcedureName = match.captured(1);
          currentProcedureTimeStamp = str.left(fsContentStartsAt_[fsFmt_]);
          if (procedureByName_.contains(currentProcedureName)) // procedure use mode:
          {
            Procedure          *prc=procedureByName_.value(currentProcedureName);
            for (int i=0; i<prc->content().size(); i++)
              procedureContent.append(prc->content().at(i));
            prc->incNumOfExpanded();
            numOfProcedureExpanded++;
            continue;
          }
          else // procedure content mode:
          {
            procedureByName_.insert(currentProcedureName, new Procedure(currentProcedureName));
            isProcedureContentMode = true;
            reProcedureContent.setPattern("[\\.0-9]{2}&" + currentProcedureName + "/(.*)");
            logger->write(SgLogger::DBG, SgLogger::IO, className() + 
              "::readLogFile(): station " + stationName_ + 
              ": found content of procedure \"" + currentProcedureName + "\"");
          };
        }
        else if (isProcedureContentMode)
        {
          Procedure            *prc=NULL;
          if (procedureByName_.contains(currentProcedureName))
          {
            prc = procedureByName_.value(currentProcedureName);
            if ((match=reProcedureContent.match(str)).hasMatch())
              prc->content().append(match.captured(1));
            else
            {
              logger->write(SgLogger::DBG, SgLogger::IO, className() + 
                "::readLogFile(): station " + stationName_ + 
                ": collected content of procedure \"" + currentProcedureName + "\", " +
                QString("").setNum(prc->content().size()) + " record(s) total");
              if (!prc->content().size())
              {
                procedureByName_.remove(currentProcedureName);
                procedureNameByName.remove(currentProcedureName);
                // redefine the regExp:
                strTmp = "[\\.0-9]{2}:(";
                for (QMap<QString, QString>::iterator it=procedureNameByName.begin(); 
                  it!=procedureNameByName.end(); ++it)
                  strTmp += it.key() + "|";
                strTmp.chop(1);
                reProcedures.setPattern(strTmp + ")");
                logger->write(SgLogger::DBG, SgLogger::IO, className() + 
                  "::readLogFile(): station " + stationName_ + 
                  ": procedure \"" + currentProcedureName + 
                  "\" is empty, removed from the substitution pool");
              };
              //
              // push the strings back:
              for (int i=0; i<prc->content().size(); i++)
                procedureContent.append(
                  currentProcedureTimeStamp + "&" + currentProcedureName + "/" + prc->content().at(i));
              //
              //
              isProcedureContentMode = false;
              currentProcedureName = "";
              currentProcedureTimeStamp = "";
            };
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ + 
              ": cannot find procedure \"" + currentProcedureName + "\" in the map; input: \"" + 
              asciifyString(str) + "\"");
          continue;
        }
        //
        // check proc:
        else if ((match=reProc.match(str)).hasMatch() && extractEpoch(str, t))
        {
          hasProc = true;
          if (t < tFirst) // session is not started yet:
          {
            procName = match.captured(1);
          }
          else if (procName != match.captured(1)) // another session?:
          {
            if (procName.size())
              logger->write(SgLogger::INF, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": proc has changed: \"" + procName + "\" => \"" + match.captured(1) +
                "\"; input: \"" + asciifyString(str) + "\"");
            procName = match.captured(1);
            if (tRightMargin <= t && (!needCableSign || hasCableSign))
            {
              reachedEndOfSession = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": got to the end of the session: " + 
                tRightMargin.toString(SgMJD::F_Simple) + " <= " +
                t.toString(SgMJD::F_Simple));
            };
          };
        };
        if ((match=reEquip.match(str)).hasMatch() && extractEpoch(str, t))
        {
          if (backEndName.size() && backEndName != match.captured(1))
          {
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": equip (backEnd) has changed: \"" + backEndName + "\" => \"" + match.captured(1) +
              "\"; input: \"" + asciifyString(str) + "\"");
          };
          backEndName = match.captured(1);
          //
          if (recorderName.size() && recorderName != match.captured(2))
          {
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
               ": equip (recorder) has changed: \"" + recorderName + "\" => \"" + match.captured(2) +
              "\"; input: \"" + asciifyString(str) + "\"");
          };
          recorderName = match.captured(2);
          hasEquip = true;
          //
          if (backEndName == "dbbc3")
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_DBBC3);
          if (backEndName.contains("dbbc3_")) // FS 10 whims
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_DBBC3);
          else if (backEndName == "dbbc")
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_DBBC);
          else if (backEndName == "rdbe")
          {
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_RDBE);
            logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
          }
          else if (backEndName == "cdas")
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_CDAS);
          else if (backEndName == "none")
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_NONE);
          else
            logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_OTHER);
          logReadings_.setBackEndInfo(backEndName);
          logReadings_.setRecorderInfo(recorderName);

          logger->write(SgLogger::DBG, SgLogger::IO, className() +
            "::readLogFile(): station " + stationName_ +
            ": found: backend=\"" + logReadings_.getBackEndInfo() + 
            "\", recorder=\"" + logReadings_.getRecorderInfo() + "\"");
        };
        //
        // CDAS:
        if ((match=reEquipCdas.match(str)).hasMatch() && extractEpoch(str, t))
        {
          if (backEndName.size() && backEndName != "cdas")
          {
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": equip (backEnd) has changed: \"" + backEndName + "\" => \"cdas\"; input: \"" + 
              asciifyString(str) + "\"");
          };
          logReadings_.setBackEndInfo("cdas");
          logReadings_.setRecorderInfo("mk6");
          hasEquip = true;
          logReadings_.channelSetup().setBackEndType(SgChannelSetup::BET_CDAS);
          logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
          logger->write(SgLogger::DBG, SgLogger::IO, className() +
            "::readLogFile(): station \"" + stationName_ +
            "\": found: backend=\"" + logReadings_.getBackEndInfo() + 
            "\", recorder=\"" + logReadings_.getRecorderInfo() + "\"");
        };
  
        if (contentType==SgStnLogReadings::CT_UNDEF && 
            (match=reSdePattern.match(str)).hasMatch() && extractEpoch(str, t))
          contentType=SgStnLogReadings::CT_SDE;
        //
        //
        if (!hasStnName)
        {
          if ((match=reStnId.match(str)).hasMatch())
          {
            stationName_ = match.captured(3).leftJustified(8, ' ');
            hasStnName = true;
            hasLocation = true; // do not need to check location
            logger->write(SgLogger::INF, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": found a name of the station (from \"stnId\"): \"" + stationName_ + "\"");
          };
        };
        if (!hasLocation)
        {
          if ((match=reLocation.match(str)).hasMatch())
          {
            if (!hasStnName)
            {
              stationName_ = match.captured(1).leftJustified(8, ' ');
              hasLocation = true;
              logger->write(SgLogger::INF, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": found a name of the station (from \"location\"): \"" + stationName_ + "\"");
            }
            else if (stationName_ != match.captured(1))
            {
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": the station name from \"location\" (\"" + match.captured(1) + 
                "\") is different");
            };
          };
        };
        //
        if (have2checkT && extractEpoch(str, t) && tRightMargin <= t && (!needCableSign || hasCableSign))
        {
          reachedEndOfSession = true;
          logger->write(SgLogger::DBG, SgLogger::IO, className() +
            "::readLogFile(): station " + stationName_ +
            ": got to the end of the session: " + 
            tRightMargin.toString(SgMJD::F_Simple) + " <= " +
            t.toString(SgMJD::F_Simple));
        };        
        //
        if (reEndOfSchd.match(str).hasMatch() && extractEpoch(str, t) && tFirst < t && 
            (!needCableSign || hasCableSign))
        {
          reachedEndOfSession = true;
          logger->write(SgLogger::DBG, SgLogger::IO, className() +
            "::readLogFile(): station " + stationName_ +
            ": got to the end of the session: end of schedule at " + 
            t.toString(SgMJD::F_Simple));
        };
        //
        if (reachedEndOfSession)
          continue;
        //
        //
        //
        // search for on source:
        if (reOnSource.match(str).hasMatch() ||
            reTapeOn.match(str).hasMatch() )
        {
          if (extractEpoch(str, t))
          {
            if (!isOnSource)
            {
              osRec->tStart() = t;
              isOnSource = true;
              if (tRightMargin <= t && (!needCableSign || hasCableSign))
              {
                reachedEndOfSession = true;
                logger->write(SgLogger::DBG, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ +
                  ": got to the end of a session: " + 
                  tRightMargin.toString(SgMJD::F_Simple) + " <= " +
                  t.toString(SgMJD::F_Simple) + " (tRightMargin <= osRec->tStart())");
              };
              if (tAtTheEnd <= t)
                have2checkT = true;
            }
            else if (420.0/DAY2SEC < (t-osRec->tStart())) 
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": got unexpected `on source' record; string: \"" + asciifyString(str) + "\"");
//              "\", prev t: " + osRec->tStart().toString());
          };
        }
        else if ((match=reScanName.match(str)).hasMatch())
        {
          if (3<osRec->getScanName().size() && osRec->getScanName()!=match.captured(1))
          {
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ + ": the scan name \"" + 
              osRec->getScanName() + "\" will be replaced with \"" + match.captured(1) + 
              "\", looks like discontinuity in the log file");
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ + ": string: \"" + asciifyString(str) + "\"");
            // check if we already got accumulated data:
            if (osRec->cableCals().size())
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": the scan is not empty: size of cable cal list is " + 
                QString("").setNum(osRec->cableCals().size()));
            if (osRec->meteoPars().size())
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": the scan is not empty: size of meteo parameters list is " +
                QString("").setNum(osRec->meteoPars().size()));
          };
          osRec->setScanName(match.captured(1).toLower());
          //
          // check experiment name:
          if (match.captured(2).size())
          {
            if (logReadings_.getExperimentName() != match.captured(2))
            {
              if (logReadings_.getExperimentName().size())
                logger->write(SgLogger::WRN, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ +
                  ": the experiment name has been changed: \"" + 
                  logReadings_.getExperimentName() + "\" => \"" +
                  match.captured(2) + "\"");
              else
                logger->write(SgLogger::DBG, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ +
                  ": found experiment name: \"" + match.captured(2) + "\"");
              logReadings_.setExperimentName(match.captured(2));
            };
          };
          //
        }
        else if ((match=reSourceName.match(str)).hasMatch())
        {
          if (3<osRec->getSourceName().size() && 
                osRec->getSourceName()!= QString("idle") &&
                osRec->getSourceName()!= QString("azel") &&
                osRec->getSourceName()!=match.captured(1).toUpper())
            logger->write(SgLogger::WRN, SgLogger::IO, className() +
              "::readLogFile(): station " + stationName_ +
              ": the source name \"" + osRec->getSourceName() +
              "\" will be replaced with \"" + match.captured(1).toUpper() +
              "\", looks like discontinuity in the log file");
          osRec->setSourceName(match.captured(1).toUpper());
          //
          // get Az/El info:
          if ((match=reAzElFromSourceName.match(str)).hasMatch())
          {
            if (extractEpoch(str, t) && tFirst < t)
            {
              QStringList       strL=match.captured(1).split(',');
              if (2 < strL.size())
              {
                double          dAz, dEl, scale=60.0*60.0;
                bool            isTmp;
                QString         ss("");

                ss = strL.at(0);
                if (ss.at(ss.size() - 1) == "d")
                {
                  ss.chop(1);
                  scale = 1.0;
                };
                dAz = ss.toDouble(&isTmp)*scale;
                scale = 60.0*60.0;
                if (isTmp)
                {
                  ss = strL.at(1);
                  if (ss.at(ss.size() - 1) == "d")
                  {
                    ss.chop(1);
                    scale = 1.0;
                  };
                  dEl = ss.toDouble(&isTmp)*scale;
                  if (isTmp)
                  {
                    trakl.setT(t);
                    trakl.setAz(dAz);
                    trakl.setEl(dEl);
                    trakl.addAttr(SgTraklReading::Attr_FROM_SOURCE);

                    SgTraklReading   *pTrakl=new SgTraklReading(trakl);
                    logReadings_.trakls() << pTrakl;
                  }
                  else
                  logger->write(SgLogger::WRN, SgLogger::IO, className() +
                    "::readLogFile(): station " + stationName_ + ": cannot get El data from the 'source=azel' record;"
                    " the string: \"" + str + "\"");
                }
                else
                  logger->write(SgLogger::WRN, SgLogger::IO, className() +
                    "::readLogFile(): station " + stationName_ + ": cannot get Az data from the 'source=azel' record;"
                    " the string: \"" + str + "\"");
              }
              else
                logger->write(SgLogger::WRN, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ + ": cannot extract az/el data from the 'source=azel' record;"
                  " the string: \"" + str + "\"");


            };
          };
          // end of Az/El
        }
        // search for data on:
        else if (reDataOn.match(str).hasMatch() && extractEpoch(str, osRec->tDataOn()))
        {
          isDataOn = true;
        }
        // data_valid=off:
        else if (reDataOff.match(str).hasMatch() ||
                 reTapeOff.match(str).hasMatch() )
        {
          if (extractEpoch(str, t))
          {
            if (isOnSource)
            {
              osRec->tFinis() = t;
              if (tRightMargin <= t  && (!needCableSign || hasCableSign))
              {
                reachedEndOfSession = true;
                logger->write(SgLogger::DBG, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ +
                  ": got to the end of a session: " + 
                  tRightMargin.toString(SgMJD::F_Simple) + " <= " +
                  t.toString(SgMJD::F_Simple) + " (tRightMargin <= osRec->tFinis())");
              };
              //
              if (tFirst < t)
              {
                osRec->setIdx(logReadings_.onSourceRecords().size());
                logReadings_.onSourceRecords() << osRec;
              };
              // check:
              if (osRec->getScanName().size() < 3)
              {
                logger->write(SgLogger::WRN, SgLogger::IO, className() +
                  "::readLogFile(): station " + stationName_ + ": the scan name \"" + 
                  osRec->getScanName() + 
                  "\" too short and cannot be used; the `data_valid=off' string: \"" + 
                  asciifyString(str) + "\"");
              };
              osRec = new SgOnSourceRecord;
              osRec->setScanName("");
              osRec->setSourceName("");
              isOnSource = false; 
              isDataOn = false; 
            }
            else if (logReadings_.onSourceRecords().size() && 
                420.0/DAY2SEC < (t - logReadings_.onSourceRecords().at(logReadings_.onSourceRecords().size()-1)->getTfinis()))
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": got unexpected data_valid=off record; string: \"" + asciifyString(str) + "\"");
          };
        };
        //
        //
        //
        //
        // end of data on/off switches
        //
        // check for operator comments:
        QRegularExpression      reNotUserComments(";\"(?:/| Command from |FiLa10G:|Flexbuff:|"
          "start transferring scan|Error opening connection to FILA10G|"
          "Error opening connection to flexbuff|.+\\s+1pps diff:).*",
            QRegularExpression::CaseInsensitiveOption);
        QRegularExpression      reFilter1(";\"\\s*Comment from\\s+'\\w+\\(?\\d?\\)?':\\s+\""
          "\\s+CHECKLIST\\(?\\w*\\)?:\\s+(\\b.+)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpression      reFilter2(";\"\\s*CHECKLIST:\\s+(.+)", 
          QRegularExpression::CaseInsensitiveOption);
        QRegularExpression      reFilter3(";\"\\s*Comment from\\s+'\\w+\\(?\\w*\\)?':\\s+\"\\s+(\\b.+)", 
          QRegularExpression::CaseInsensitiveOption);
        QRegularExpression      reFilterLast(";\"\\s*(\\b.+)", 
          QRegularExpression::CaseInsensitiveOption);
        if (reOpComments.match(str).hasMatch())
        {
          if (extractEpoch(str, t))
          {
            extractContent(str, strTmp);
            if (!reNotUserComments.match(strTmp).hasMatch())
            {
              if ((match=reFilter1.match(strTmp)).hasMatch())
                strTmp = match.captured(1);
              else if ((match=reFilter2.match(strTmp)).hasMatch())
                strTmp = match.captured(1);
              else if ((match=reFilter3.match(strTmp)).hasMatch())
                strTmp = match.captured(1);
              else if ((match=reFilterLast.match(strTmp)).hasMatch())
                strTmp = match.captured(1);
              //
              if (strTmp.size() < 100)
                operatorComments << t.toString(SgMJD::F_Simple) + ": " + strTmp;
              else // split too long strings:
              {
                QStringList     sLst=strTmp.split(QRegExp("\\s"));
                QString         sAux("");
                for (int i=0; i<sLst.size(); i++)
                {
                  sAux += " " + sLst.at(i);
                  if (sAux.size() > 81)
                  {
                    operatorComments << t.toString(SgMJD::F_Simple) + ":" + sAux;
                    sAux = "";
                  };
                };
                if (sAux.size())
                  operatorComments << t.toString(SgMJD::F_Simple) + ":" + sAux;
              };
            };
          };
        };
        //
        //
        if ((match=reTrakl.match(str)).hasMatch()) // VGOS has these records every sec
        {
          if (extractTraklReading(str, match, t, trakl, tFirst))
          {
            SgTraklReading     *pTrakl=new SgTraklReading(trakl);
            logReadings_.trakls() << pTrakl;
            //
            // add to tsys recs:
            int                 idx=logReadings_.tSyses().size() - 1;
            SgTsysReading      *pTs=NULL;
            while (0 < idx && 
              (pTs=logReadings_.tSyses().at(idx)) && 
              (t - pTs->getT()) < 60.0/DAY2SEC)
            {
              if (pTs->trakls().size() < 10)
                pTs->trakls() << pTrakl;
              idx--;
            };
          };
        }
        //
        //
        // vgos tsys:
        else if (wantTsys && 
                (reTsysVgos.match(str).hasMatch() ||
                  (logReadings_.channelSetup().getBackEndType()==SgChannelSetup::BET_CDAS && 
                   reTsysCdas.match(str).hasMatch())))
        {
          QMap<QString, float>  tsys;
          QRegularExpression   *reTmp=&reTsysVgos;
          if (logReadings_.channelSetup().getBackEndType() == SgChannelSetup::BET_CDAS)
            reTmp=&reTsysCdas;
//        if (extractTsysVgosReading(str, reTsysVgos, t, tsys, tAtTheBegin) && tsys.size())
          if (extractTsysVgosReading(str, *reTmp, t, tsys, tAtTheBegin) && tsys.size())
          {
            SgTsysReading      *tsr=NULL;
            if (0.3/DAY2SEC < fabs(lastReadTsys - t))
            {
              tsr = new SgTsysReading;
              tsr->setT(t);
              tsr->setOsRec(isOnSource?osRec:NULL);
              tsr->setIsOnSource(isOnSource);
              tsr->setIsDataOn(isDataOn);
              logReadings_.tSyses() << tsr;
              osRec->tSyses() << tsr;
              //
              // add trakls:
              int               idx=logReadings_.trakls().size() - 1;
              SgTraklReading   *pTrakl=NULL;
              while (0 < idx && 
                (pTrakl=logReadings_.trakls().at(idx)) && 
                (pTrakl->isAttr(SgTraklReading::Attr_FROM_SOURCE) || (t - pTrakl->getT()) < 60.0/DAY2SEC)
                )
              {
                if (tsr->trakls().size() < 5)
                  tsr->trakls() << pTrakl;
                idx--;
              };
            }
            else
              tsr = logReadings_.tSyses().last();
            tsr->addTsyses(tsys);
            lastReadTsys = t;
          };
          tsys.clear();
        }
        //
        // vgos tpcont:
        else if (reTpcontVgos.match(str).hasMatch())
        {
          QMap<QString, QVector<int> >
                                tpcont;
          if (extractTpcontVgosReading(str, reTpcontVgos, t, tpcont, tAtTheBegin) && tpcont.size())
          {
            SgTsysReading      *tsr=NULL;
            if (0.2/DAY2SEC < fabs(lastReadTsys - t))
            {
              tsr = new SgTsysReading;
              tsr->setT(t);
              tsr->setOsRec(isOnSource?osRec:NULL);
              tsr->setIsOnSource(isOnSource);
              tsr->setIsDataOn(isDataOn);
              logReadings_.tSyses() << tsr;
              osRec->tSyses() << tsr;
              //
              // add trakls:
              int               idx=logReadings_.trakls().size() - 1;
              SgTraklReading   *pTrakl=NULL;
              while (0 < idx && 
                (pTrakl=logReadings_.trakls().at(idx)) && 
                (pTrakl->isAttr(SgTraklReading::Attr_FROM_SOURCE) || (t - pTrakl->getT()) < 60.0/DAY2SEC)
                )
              {
                if (tsr->trakls().size() < 5)
                  tsr->trakls() << pTrakl;
                idx--;
              };
            }
            else
              tsr = logReadings_.tSyses().last();
            tsr->addTpconts(tpcont);
            lastReadTsys = t;
          };
          tpcont.clear();
        }
        //
        //
        // vgos pcal:
        else if ((match=rePcalOffset.match(str)).hasMatch())
        {
          extractPcalOffsetReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=rePcalVgos.match(str)).hasMatch())
        {
          QMap<QString, float*> pcals;
          if (extractPcalVgosReading(str, match, t, pcals, logReadings_.channelSetup(), tAtTheBegin) && 
              pcals.size())
          {
            SgPcalReading      *pcr=NULL;
/*          
            strTmp = "";
            if (logReadings_.pcals().size() && logReadings_.pcals().last()->pcal().size())
              strTmp = logReadings_.pcals().last()->pcal().begin().key();
std::cout << "++ Prev: [" << qPrintable(strTmp) << "] current: [" << qPrintable(pcals.begin().key()) << "]\n";
*/
            if (0.2/DAY2SEC < fabs(lastReadPcal - t))
            {
              pcr = new SgPcalReading;
              pcr->setT(t);
              pcr->setOsRec(isOnSource?osRec:NULL);
              logReadings_.pcals() << pcr;
            }
            else
              pcr = logReadings_.pcals().last();

            if (!pcr->addPcals(pcals))
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": got duplicate pcals; input: \"" + asciifyString(str) + "\"");
            lastReadPcal = t;
          };
          for (QMap<QString, float*>::iterator it=pcals.begin(); it!=pcals.end(); ++it)
            delete[] it.value();
          pcals.clear();
        }
        //
        // vgos dot2pps/gps:
        else if ((match=reDot2xpsVgos.match(str)).hasMatch())
        {
          float                 dot2xps=1.0e20;
          bool                  isGps;
          QString               sKey("");
          if (extractDot2xpsVgosReading(str, match, t, sKey, dot2xps, tAtTheBegin, isGps) && 
              dot2xps < 1.0e12)
          {
            if (!logReadings_.channelSetup().xpsIdByKey().contains(sKey))
              logReadings_.channelSetup().xpsIdByKey().insert(sKey, sKey);
            SgDot2xpsReading   *d2r=NULL;
            if (0.2/DAY2SEC < fabs(lastReadDot2xps - t))
            {
              d2r = new SgDot2xpsReading;
              d2r->setT(t);
              d2r->setOsRec(isOnSource?osRec:NULL);
              logReadings_.dot2xpses() << d2r;
            }
            else
              d2r = logReadings_.dot2xpses().last();
            if (!(isGps?d2r->addDot2gpsByBrd(sKey, dot2xps):d2r->addDot2ppsByBrd(sKey, dot2xps)))
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": got duplicate " + (isGps?"dot2gps":"dot2pps") + 
                "; input: \"" + asciifyString(str) + "\"");
            lastReadDot2xps = t;
          };
        }
        // dbbc3 tp reading:
        else if ((match=reDbbc3Tp.match(str)).hasMatch())
        {
          extractEpoch(str, t);

          QString               sensorId(match.captured( 1));
          QString               boardId(match.captured( 3));
          QString               agc(match.captured( 6));          
          double                bw=0.0;
          double                gainU=0.0;
          double                gainL=0.0;
          double                tpOffU=0.0;
          double                tpOffL=0.0;
          double                tpOnU=0.0;
          double                tpOnL=0.0;
          double                sefdU=0.0;
          double                sefdL=0.0;
          bw      = match.captured( 4).toDouble();
          gainU   = match.captured( 7).toDouble();
          gainL   = match.captured( 8).toDouble();
          tpOffU  = match.captured( 9).toDouble();
          tpOffL  = match.captured(10).toDouble();
          tpOnU   = match.captured(11).toDouble();
          tpOnL   = match.captured(12).toDouble();
          sensorId= sensorId.simplified().rightJustified(5, ' ');
          QString               chanKey=setupTpSensor(sensorId + 'l', "readLogFile()", str);
          if (chanKey.size())
          {
            chanKey = setupTpSensor(sensorId + 'u', "readLogFile()", str);
            if (chanKey.size())
            {
              SgDbbc3TpReading     *tp=NULL;
              if (1.0/DAY2SEC < fabs(lastReadTsys - t))
              {
                tp = new SgDbbc3TpReading;
                tp->setT(t);
                tp->setOsRec(isOnSource?osRec:NULL);
                tp->setIsOnSource(isOnSource);
                tp->setIsDataOn(isDataOn);
                logReadings_.dbbc3Tps() << tp;
                osRec->dbbc3Tps() << tp;
              }
              else
                tp = logReadings_.dbbc3Tps().last();
              tp->addRecord(sensorId, boardId, bw, agc, gainU, gainL, tpOffU, tpOffL, tpOnU, tpOnL, 
                sefdU, sefdL);
              lastReadTsys = t;
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                "::readLogFile(): station " + stationName_ + ": parsing chanKey has failed for \"" + 
                sensorId + 'u' + "\"; input: \"" + asciifyString(str) + "\"");
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO, className() + 
              "::readLogFile(): station " + stationName_ + ": parsing chanKey has failed for \"" + 
              sensorId + 'l' + "\"; input: \"" + asciifyString(str) + "\"");
        }
        //
        // SEFD:
        else if ((match=reSefd.match(str)).hasMatch())
        {
          QString               sensId("");
          QString               srcNam("");
          double                az, de;
          QVector<double>       vec;
          if (extractSefdReading(str, match, t, sensId, srcNam, az, de, vec, tAtTheBegin))
          {
            SgSefdReading      *sefd=NULL;
            if (0.3/DAY2SEC < fabs(lastReadSefd - t))
              logReadings_.sefds() << (sefd=new SgSefdReading);
            else
              sefd = logReadings_.sefds().last();
            if (!sefd->addValByChanKey(t, srcNam, az, de, sensId, vec))
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": got duplicate SEFD; input: \"" + asciifyString(str) + "\"");
            lastReadSefd = t;
          };
        }
        //
        //
        // SX tsys reading:
        else if (wantTsys && 
                logReadings_.channelSetup().getBackEndType()!=SgChannelSetup::BET_CDAS &&
                reTsys.match(str).hasMatch())
        {
          QMap<QString, float>  tsys;
          if (extractTsysReading(str, reTsys, t, tsys, tAtTheBegin) && tsys.size())
          {
            if (fabs(lastReadTsys - t) < 0.1/DAY2SEC)
              logReadings_.tSyses().last()->addTsyses(tsys);
            else
            {
              SgTsysReading    *tsr=new SgTsysReading;
              tsr->setT(t);
              tsr->addTsyses(tsys);
              tsr->setOsRec(isOnSource?osRec:NULL);
              logReadings_.tSyses() << tsr;
              osRec->tSyses() << tsr;
              //
              // add trakls:
              int               idx=logReadings_.trakls().size() - 1;
              SgTraklReading   *pTrakl=NULL;
              while (0 < idx && 
                (pTrakl=logReadings_.trakls().at(idx)) && 
                (pTrakl->isAttr(SgTraklReading::Attr_FROM_SOURCE) || (t - pTrakl->getT()) < 60.0/DAY2SEC)
                )
              {
                if (tsr->trakls().size() < 5)
                  tsr->trakls() << pTrakl;
                idx--;
              };
            };
            lastReadTsys = t;
          };
          tsys.clear();
        }
        //
        // meteo parameters:
        else if (reMeteo.match(str).hasMatch())
        {
          SgMeteoData             m;
          if (extractMeteoReading(str, reMeteo, t, m, oom) && tFirst < t)
          {
            SgMeteoReading       *meteoRec=new SgMeteoReading(t, m);
            logReadings_.meteoPars() << meteoRec;
            if (isOnSource)
            {
              osRec->meteoPars() << meteoRec;
              meteoRec->setOsRec(osRec);
            };
          };
        }
        //
        // bbc reading:
        else if ((match=reBbc.match(str)).hasMatch())
        {
          extractBbcReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        // vc reading:
        else if ((match=reVc.match(str)).hasMatch())
        {
          extractVcReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        // patch reading:
        else if ((match=rePatch.match(str)).hasMatch())
        {
          extractPatchReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=reLo.match(str)).hasMatch())
        {
          extractLoReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=reLoRxg.match(str)).hasMatch())
        {
          extractLoRxgReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=reLoCdasUdc.match(str)).hasMatch())
        {
          extractLoCdasUdc(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=reChanSkyFreqValues.match(str)).hasMatch())
        {
          extractChanSkyFrqReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        else if ((match=reVci.match(str)).hasMatch())
        {
          extractVciReading(str, match, t, tAtTheEnd, logReadings_.channelSetup(), tFirst);
        }
        //
        //
        // cable calibration:
        else if ((match=reCableCal.match(str)).hasMatch())
        {
          hasCableRecord_ = true;
          if (extractCableCalibration(str, match, t, d) && tFirst < t)
          {
            double                scale = 4.0e5;
            SgCableCalReading    *cableRec=new SgCableCalReading(t, d/scale);
            logReadings_.cableCals() << cableRec;
            if (isOnSource)
            {
              osRec->cableCals() << cableRec;
              cableRec->setOsRec(osRec);
            };
//          if (tRightMargin <= t)
//            reachedEndOfSession = true;
          };
        }
        else if ((match=reCdms.match(str)).hasMatch())
        {
          hasCdmsRecord_ = true;
          if (extractCableCalibration(str, match, t, d) && tFirst < t)
          {
            double                scale = 2e12;
            SgCableCalReading    *cableRec=new SgCableCalReading(t, d/scale);
            logReadings_.cableCals() << cableRec;
            if (isOnSource)
            {
              osRec->cableCals() << cableRec;
              cableRec->setOsRec(osRec);
            };
//          if (tRightMargin <= t)
//            reachedEndOfSession = true;
          };
        }
        // cablelong measurements:
        else if ((match=reCableLong.match(str)).hasMatch())
        {
          double                scale = 4.0e5;
          if (extractCableCalibration(str, match, t, d))
            logReadings_.cableLongs() << new SgCableCalReading(t, d/scale);
        }
        // cable calibration sign:
        else if (!hasCableSign)
        {
          // 1) 2014.252.16:54:02.27/cablediff/647.7e-6,-
          if ((match=reCableDiff.match(str)).hasMatch())
          {
            if (extractCableCalibrationSignByDiff(match, val))
            {
              logReadings_.setCableSign(val);
              hasCableSign = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": the cable calibration sign was set to \"" + 
                QString(logReadings_.getCableSign()==1?"+":"-") + "\". Source: cable_diff");
            };
          }
          // 2) 2012.150.17:40:46.48;"cabvle sign is positive, OK
          else if (reCableSign.match(str).hasMatch())
          {
            if (str.contains("positive", Qt::CaseInsensitive))
            {
              logReadings_.setCableSign(1);
              hasCableSign = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": the cable calibration sign was set to \"+\". Source: cable_sign_is");
            }
            else if (str.contains("negative", Qt::CaseInsensitive))
            {
              logReadings_.setCableSign(-1);
              hasCableSign = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + 
                ": the cable calibration sign was set to \"-\". Source: cable_sign_is");
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ + ": cannot determine proper cable sign "
                "from cable_sign_is pattern; input: \"" + asciifyString(str) + "\"");
          }
          else if (reCableDiffIs.match(str).hasMatch())
          {
            if (str.contains("positive", Qt::CaseInsensitive) || 
                str.contains("larger", Qt::CaseInsensitive)     )
            {
              logReadings_.setCableSign(1);
              hasCableSign = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": the cable calibration sign was set to \"+\". Source: cable_diff_is");
            }
            else if ( str.contains("negative", Qt::CaseInsensitive) || 
                      str.contains("smaller", Qt::CaseInsensitive)    )
            {
              logReadings_.setCableSign(-1);
              hasCableSign = true;
              logger->write(SgLogger::DBG, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": the cable calibration sign was set to \"-\". Source: cable_diff_is");
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO, className() +
                "::readLogFile(): station " + stationName_ +
                ": cannot determine proper cable sign from cable_diff_is pattern; input: \"" + 
                asciifyString(str) + "\"");
          };
        };
      };
    };
    compressors.closeFlt(pipe, f, s);

    if (contentType == SgStnLogReadings::CT_UNDEF)
      contentType = SgStnLogReadings::CT_IVS;
    logReadings_.setContentType(contentType);


    // pick up the last data on if the log has been terminated:
    if (osRec && osRec->getScanName().size() /*&& osRec->getSourceName().size()*/ &&
      (osRec->cableCals().size() || osRec->meteoPars().size() || 
       osRec->tSyses().size() || osRec->dbbc3Tps().size() ))
    {
      osRec->tFinis() = t;
      osRec->setIdx(logReadings_.onSourceRecords().size());
      logReadings_.onSourceRecords() << osRec;
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::readLogFile(): station " + stationName_ + ": picked up not finished onSource record");
    };
  };
  //
  if (!hasProc)
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": no proc record has been found in the log file");
  //
  if (operatorComments.size())
  {
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": collected operator's comments: ");
    for (int i=0; i<operatorComments.size(); i++)
      logger->write(SgLogger::INF, SgLogger::IO, className() +
        "::readLogFile(): station " + stationName_ + ": " + operatorComments.at(i));
  };
  //
  // ok, end of parsing. 
  //
  if (flogSize < 1024)
  {
    str.setNum(flogSize);
    str += "b";
  }
  else if (flogSize < 1048576)
  {
    str.setNum(flogSize/1024);
    str += "K";
  }
  else if (flogSize < 1073741824)
  {
    str.setNum(flogSize/1048576);
    str += "M";
  }
  else 
  {
    str.setNum(flogSize/1073741824);
    str += "G";
  };
  logger->write(SgLogger::INF, SgLogger::IO, className() +
    "::readLogFile(): station " + stationName_ + ": " + QString("").setNum(numOfReadStrs) + 
    " strings of the log file (" + str + ") were read" +
    (numOfSkippedStrs>1?" (" + QString("").setNum(numOfSkippedStrs) + " strings were skipped)":""));
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readLogFile(): station " + stationName_ + ": the log file version: " + versionStr);
  if (numOfProcedureExpanded)
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": " + QString("").setNum(numOfProcedureExpanded) +
      " time(s) procedure(s) were expanded:");
    for (QMap<QString, Procedure*>::iterator it=procedureByName_.begin(); it!=procedureByName_.end(); 
      ++it)
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::readLogFile(): station " + stationName_ + ":     " + it.value()->getName() + ": " +
        QString("").setNum(it.value()->getNumOfExpanded()));
  };
  logger->write(SgLogger::INF, SgLogger::IO, className() +
    "::readLogFile(): station " + stationName_ + ": collected " + 
    QString("").setNum(logReadings_.onSourceRecords().size()) + " on_source, " + 
    QString("").setNum(logReadings_.cableCals().size()) + " cable_cal and " + 
    QString("").setNum(logReadings_.meteoPars().size()) + " meteo records");
  //
  //
  if (wantTsys)
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": collected " + 
      QString("").setNum(logReadings_.tSyses().size()) + " tsys records");
  if (antcalOutputData_ & AOD_DBBC3TP)
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": collected " + 
      QString("").setNum(logReadings_.dbbc3Tps().size()) + " dbbc3 TP records");
  if (antcalOutputData_ & AOD_SEFD)
    logger->write(SgLogger::INF, SgLogger::IO, className() +
      "::readLogFile(): station " + stationName_ + ": collected " + 
      QString("").setNum(logReadings_.sefds().size()) + " SEFD evaluation records");
  //
  //
  if (5 < logReadings_.tSyses().size())
    hasTsysRecord_ = true;
  
  if (rinexFileNameByStn_ &&
    rinexFileNameByStn_->contains(stationName_) && logReadings_.meteoPars().size()==0)
  {
    if (checkRinexFile(fileName, tFirst, tLast, rinexFileNameByStn_->value(stationName_)))
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::readLogFile(): station " + stationName_ + ": collected " + 
        QString("").setNum(logReadings_.meteoPars().size()) +
        " meteo records from RINEX files");
    else
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::readLogFile(): station " + stationName_ + ": nothing to get from RINEX files");
  };
  //
  return true;
};



//
SgStnLogCollector::FieldSystemEpochFormat SgStnLogCollector::guessFsFormat(const QString& str)
{
  FieldSystemEpochFormat        retFsFmt;
  if (reFsNewest_.match(str).hasMatch())
    retFsFmt = FSEF_NEWEST;
  else if (reFsNew_.match(str).hasMatch())
    retFsFmt = FSEF_NEW;
  else if (reFsOld_.match(str).hasMatch())
    retFsFmt = FSEF_OLD;
  else if (rePcmt_.match(str).hasMatch())
    retFsFmt = FSEF_PCMT;
  else if (reMet_.match(str).hasMatch())
    retFsFmt = FSEF_MET;
  else
    retFsFmt = FSEF_UNDEF;

  return retFsFmt;
};



//
bool SgStnLogCollector::extractEpoch(const QString& str, SgMJD& t)
{
  int                           nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0;
  double                        dSecond=0.0, f=0.0;
  bool                          isOk=true;
  QRegularExpressionMatch       match;
  //
  switch (fsFmt_)
  {
  //340184033;MARK III PC FIELD SYSTEM  V 3.65   EFLSBERG 1995 00000000
  case FSEF_OLD:        // DDDHHMMSS
/*  if (reFsOld_.indexIn(str) != -1) */
    if (reFsOld_.match(str).hasMatch())
    {
      nDay = str.mid(0,3).toInt(&isOk);
      if (isOk)
      {
        nHour = str.mid(3,2).toInt(&isOk);
        if (isOk)
        {
          nMin = str.mid(5,2).toInt(&isOk);
          if (isOk)
          {
            dSecond = str.mid(7,2).toInt(&isOk);
            nYear = inYear_;
          };
        };
      };
    }
    else
    {
      isOk = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractEpoch(): looks like not OLD FS format, string: \"" + asciifyString(str) + "\"");
//        "::extractEpoch(): looks like not OLD FS format, string: \"" + str + "\"");
    };
    break;
  //9903209333995;Log Opened: Mark IV Field System Version 9.3.17
  case FSEF_NEW:        // YYDDDHHMMSSss
/*  if (reFsNew_.indexIn(str) != -1)*/
    if (reFsNew_.match(str).hasMatch())
    {
      nYear = str.mid(0,2).toInt(&isOk);
      if (isOk)
      {
        nDay = str.mid(2,3).toInt(&isOk);
        if (isOk)
        {
          nHour = str.mid(5,2).toInt(&isOk);
          if (isOk)
          {
            nMin = str.mid(7,2).toInt(&isOk);
            if (isOk)
            {
              dSecond = str.mid(9,2).toInt(&isOk);
              if (isOk)
              {
                f = str.mid(11,2).toInt(&isOk);
                if (isOk)
                {
                  dSecond += 0.01*f;
                  nYear += (nYear>68)?1900:2000;
                };
              };
            };
          };
        };
      };
    }
    else
    {
      isOk = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractEpoch(): looks like not NEW FS format, string: \"" + asciifyString(str) + "\"");
//        "::extractEpoch(): looks like not NEW FS format, string: \"" + str + "\"");
    };
    break;
  //012345678901234567890
  //2006.214.20:21:46.26
  //2016.019.14:17:00.08;Log Opened: Mark IV Field System Version 9.12.2
  //2002.247.18:20:00.0        /wx/ 9.1,1017.0,90.0                            -- surprise?
  //2020.356.17:00:05          /wx/14.7,84.0,1023.1,4.0,-2                          --"--
  case FSEF_NEWEST:     // YYYY.DDD.HH.MM.SS.ss
/*  if (reFsNewest_.indexIn(str) != -1) */
    if ((match=reFsNewest_.match(str)).hasMatch())
    {
      nYear = match.captured(1).toInt(&isOk);
      if (isOk)
      {
        nDay = match.captured(2).toInt(&isOk);
        if (isOk)
        {
          nHour = match.captured(3).toInt(&isOk);
          if (isOk)
          {
            nMin = match.captured(4).toInt(&isOk);
            if (isOk)
            {
              dSecond = match.captured(5).toDouble(&isOk);
              fsContentStartsAt_[FSEF_NEWEST] = 15 + match.captured(5).size();
            };
          };
        };
      };
      /*
      nYear = str.mid(0,4).toInt(&isOk);
      if (isOk)
      {
        nDay = str.mid(5,3).toInt(&isOk);
        if (isOk)
        {
          nHour = str.mid(9,2).toInt(&isOk);
          if (isOk)
          {
            nMin = str.mid(12,2).toInt(&isOk);
            if (isOk)
            {
              dSecond = str.mid(15,5).toDouble(&isOk);
              if (!isOk)  // try as exception:
              {
                dSecond = str.mid(15,4).toDouble(&isOk);
                fsContentStartsAt_[FSEF_NEWEST] = 19;
              }
              else if (fsContentStartsAt_[FSEF_NEWEST] == 19)
                fsContentStartsAt_[FSEF_NEWEST] = 20;
            };
          };
        };
      };
      */
    }
    else
    {
      isOk = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractEpoch(): looks like not NEWEST FS format, string: \"" + asciifyString(str) + "\"");
//        "::extractEpoch(): looks like not NEWEST FS format, string: \"" + str + "\"");
    };
    break;
  case FSEF_PCMT:     //
  case FSEF_MET:      //
    //2017    9    11    18    50    29    1.196e-11    254-1850
/*  if (rePcmt_.indexIn(str) != -1) */
    if (rePcmt_.match(str).hasMatch())
    {
      QStringList               l=str.split(QRegExp("\\s+"));
      if (l.size() >= 6)
      {
        nYear = l.at(0).toInt(&isOk);
        if (isOk)
        {
          nMonth = l.at(1).toInt(&isOk);
          if (isOk)
          {
            nDay = l.at(2).toInt(&isOk);
            if (isOk)
            {
              nHour = l.at(3).toInt(&isOk);
              if (isOk)
              {
                nMin = l.at(4).toInt(&isOk);
                if (isOk)
                  dSecond = l.at(5).toDouble(&isOk);
              };
            };
          };
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::extractEpoch(): not enough data for PCMT format, the string: \"" + 
          asciifyString(str) + "\"");
//          "::extractEpoch(): not enough data for PCMT format, the string: \"" + str + "\"");
    }
    else
    {
      isOk = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractEpoch(): looks like not PCMT format, the string: \"" + asciifyString(str) + "\"");
//        "::extractEpoch(): looks like not PCMT format, the string: \"" + str + "\"");
    };
    break;
  default:
  case FSEF_UNDEF: 
    isOk = false;
    break;
  };
  //
  if (isOk)
  {
   if (0 < inYear_ && nYear != inYear_)
   {
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractEpoch(): year has changed: " + QString("").setNum(inYear_) + " => " +
        QString("").setNum(nYear) + "; input: \"" + asciifyString(str) + "\"");
      inYear_ = nYear;
    }
    else if (inYear_ == 0)
      inYear_ = nYear;
  };
  //
  if (!isOk)
  {
    if (fsFmt_==FSEF_NEWEST)
    {
      fsFmt_ = FSEF_NEW;
      isOk = extractEpoch(str, t);
      if (isOk)
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::extractEpoch(): looks like FS format has changed NEWEST=>NEW: \"" + 
          asciifyString(str) + "\"");
//          "::extractEpoch(): looks like FS format has changed NEWEST=>NEW: \"" + str + "\"");
      else
        fsFmt_ = FSEF_NEWEST;
    }
    else if (fsFmt_==FSEF_NEW)
    {
      // e.g.: /500/sessions/1998/na253/na253ft.log
      // olf version of FS has been launched while station observed
      fsFmt_ = FSEF_OLD;
      isOk = extractEpoch(str, t);
      if (isOk)
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::extractEpoch(): looks like FS format has changed NEW=>OLD: \"" + asciifyString(str) + "\"");
//          "::extractEpoch(): looks like FS format has changed NEW=>OLD: \"" + str + "\"");
      else
        fsFmt_ = FSEF_NEW;
    };
    if (!isOk)
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::extractEpoch(): failed to parse, string: \"" + asciifyString(str) + "\"");
//      "::extractEpoch(): failed to parse, string: \"" + str + "\"");
  }
  else
    t.setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSecond);
  return isOk;
};



//
bool SgStnLogCollector::extractContent(const QString& str, QString& content)
{
  content = str.mid(fsContentStartsAt_[fsFmt_]);
  return content.size() > 0;
};



//
bool SgStnLogCollector::extractMeteoReading(const QString& str, const QRegularExpression& reMeteo,
  SgMJD& t, SgMeteoData& m, const int* oom)
{
  double                        d;
  bool                          isOk=false;

  if (!extractEpoch(str, t))
    return false;

  //
  // here a little bit different approach: there can be meteo parameters: 
  // either (T,P,RH) or (T,P,RH,WindVel,WindAzimuth)
  // PS: we do not use wind velocity and direction.
  //
//#if QT_VERSION >= 0x040800
//  QString                       metStr(str.mid(reMeteo.indexIn(str) + reMeteo.cap(1).size()));
//#else
//  QRegExp                       reMeteo2(reMeteo);
//  QString                       metStr(str.mid(reMeteo2.indexIn(str) + reMeteo2.cap(1).size()));
//#endif

  QString                       metStr(str.mid(reMeteo.match(str).capturedEnd(1)));

  if (metStr.contains("C"))
    metStr = metStr.remove("C");
  if (metStr.contains("mB"))
    metStr = metStr.remove("mB");
  if (metStr.contains("%"))
    metStr = metStr.remove("%");
  if (metStr.contains("weather:"))
    metStr = metStr.remove("weather:");

  if (!metStr.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractMeteoReading(): cannot extract meteo data from \"" + asciifyString(str) + "\"");
//      "::extractMeteoReading(): cannot extract meteo data from \"" + str + "\"");
    return isOk;
  };
    
  QStringList                   lst=metStr.split(",");
   
  if (lst.size() > 2)
  {
    const QString              &strT =lst.at(oom[0]);
    const QString              &strP =lst.at(oom[1]);
    const QString              &strRh=lst.at(oom[2]);

    d = strT.toDouble(&isOk);
    if (isOk)
    {
      if (-50.0<=d && d<=80.0) // ?? is it ok, can stations operate in worse conditions?
      {
        m.setTemperature(d);
        d = strP.toDouble(&isOk);
        if (isOk)
        {
          if (d == 1100.0)
          {
            isOk = false;
            logger->write(SgLogger::WRN, SgLogger::IO, className() + 
              "::extractMeteoReading(): the extracted atmospheric pressure looks suspicious: " + 
              QString("").setNum(d) + "hPa (i.e.,==std.atm.); skipped");
          }
          else if (d >= 500.0) // 500kPa is at ~5.5km height of the standard atmosphere
          {
            m.setPressure(d);
            d = strRh.toDouble(&isOk);
            if (isOk)
            {
              if (-9.9<=d && d<=120.0)
              {
                if (d < 0.0)
                {
                  logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                    "::extractMeteoReading(): the extracted relative humidity looks suspicious: " + 
                    QString("").setNum(d) + "%; adjusted it to zero");
                  m.setRelativeHumidity(0.0);
                }
                else if (100.0 < d)
                {
                  logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                    "::extractMeteoReading(): the extracted relative humidity looks suspicious: " + 
                    QString("").setNum(d) + "%; adjusted it to 100%");
                  m.setRelativeHumidity(1.0);
                }
                else
                  m.setRelativeHumidity(d*0.01);
              }
              else
              {
                isOk = false;
                logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                  "::extractMeteoReading(): the extracted relative humidity looks unrealstic: " + 
                  QString("").setNum(d) + "%; skipped");
              };
            }
            else if (strRh.contains("EE.0", Qt::CaseInsensitive))
            {
              m.setRelativeHumidity(1.0);
              logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                "::extractMeteoReading(): forcefully set relative humidity to 1.00 from the string \"" +
                strRh + "\"");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                "::extractMeteoReading(): failed to extract relative humidity from \"" + strRh + "\"");
          }
          else
          {
            isOk = false; 
            logger->write(SgLogger::WRN, SgLogger::IO, className() + 
              "::extractMeteoReading(): the extracted atmospheric pressure is too low: " + 
              QString("").setNum(d) + "hPa; skipped");
          };
        }
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::extractMeteoReading(): failed to extract pressure from \"" + strP + "\"");
      }
      else
      {
        isOk = false;
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractMeteoReading(): the extracted atmospheric temperature looks unreal: " + 
          QString("").setNum(d) + "C; skipped");
      };
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractMeteoReading(): failed to extract temperature: from \"" + strT + "\"");
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractMeteoReading(): the number of data less than 3: \"" + metStr + "\"");
  return isOk;
};



//
bool SgStnLogCollector::extractPcalOffsetReading(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, const SgMJD& tAtTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  QString                       sLo("");
  double                        pcalOffset;
  bool                          isOk;

  if (!extractEpoch(str, t))
    return false;

  if (tAtTheEnd <= t)
    return false;
  
  sLo = match.captured(1);
  pcalOffset = match.captured(2).toDouble(&isOk)*1.0e-6;
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractPcalOffsetReading(): station " + stationName_ + 
      ": cannot convert \"" + match.capturedEnd(2) + "\" to double; input: \"" + 
      asciifyString(str) + "\"");
    return false;
  };

  QList<QString>                los;
  if (sLo.size() == 0)
    los << "*";
  else if (sLo.size() == 1)
    los << sLo << sLo + "0" << sLo + "1";
  else
    los << sLo;
  
  for (int i=0; i<los.size(); i++)
  {
    sLo = los.at(i);
    if (cs.pcalOffsetByLoId().contains(sLo) && cs.pcalOffsetByLoId().value(sLo) != pcalOffset)
      logger->write(SgLogger::INF, SgLogger::IO, className() + 
        "::extractPcalOffsetReading(): station " + stationName_ + 
        ": pcal frequency offset for LO=" + sLo + " has changed from " + 
        QString("").setNum(cs.pcalOffsetByLoId().value(sLo)) + " to " + 
        QString("").setNum(pcalOffset)  + "; input: \"" + 
        asciifyString(str) + "\"");
    cs.pcalOffsetByLoId()[sLo] = pcalOffset;
  };
  return true;
};





//
//bool SgStnLogCollector::extractBbcReading(const QString& str, const QRegExp& re, SgMJD& t, 
bool SgStnLogCollector::extractBbcReading(const QString& str, const QRegularExpressionMatch& match, 
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  QString                       bbcStr(str.mid(match.capturedEnd(2) + 1));
  QString                       bbcId(match.captured(2));

  if (!extractEpoch(str, t))
    return false;

  if (nearTheEnd <= t)
    return false;
  
  bool                          isOk;
  int                           cIdx;
  cIdx = bbcId.toInt(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractBbcReading(): station " + stationName_ + 
      ": cannot convert \"" + bbcId + "\" to int; input: \"" + asciifyString(str) + "\"");
    return false;
  };

  QStringList                   lst=bbcStr.split(",");
/*
Syntax: bbcnn=freq,ifsource,bwu,bwl,avper,gainmode,gainu,gainl

Response: bbcnn/freq,ifsource,bwu,bwl,avper,gainmode,gainu,gainl,lock,USBpwr,LSBpwr,serno,err

nn is the BBC index number corresponding to its position in the rack, 01 to 14. Not all racks
      have all BBCs.
 
Settable Parameters:

freq -- L.O. frequency in MHz, between 450.00 and 1050.00, inclusive. No default. This frequency
        range is greater than the normal range over which the BBC is specified to lock (500 to 1000 MHz)
        to allow for testing.

ifsource -- I.F. input source, one of A, B, C, D. No default.

bwu  -- Bandwidth for USB in MHz. One of 0.0625, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16. Default 2.

bwl  -- Bandwidth for LSB in MHz. One of 0.0625, 0.125, 0.25, 0.5, 1, 2, 4, 8, 16. Default bwu.

avper -- Averaging period in seconds for TPI. May be 0, 1, 2, 4, 10, 20, 40, or 60 seconds. A value
         of 0 results in 1/80 sec averaging time. Default 1. The averaging period is common to both
         upper and lower sideband. The averaging period is synchronous with the 1 pps.
*/
  //
  //2018.003.17:14:29.54&dbbcsx8/bbc03=285.75,a,8.00
  //2018.003.17:14:29.54&dbbcsx8/bbc04=365.75,a,8.00
  //2018.003.17:14:29.54&dbbcsx8/bbc05=725.75,b,8.00
  //
  if (3 <= lst.size())
  {
    double                      d;
    // BBC freq:
    d = lst.at(0).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ + 
        ": cannot convert \"" + lst.at(0) + "\" to double; input: \"" +
        asciifyString(str) + "\"");
      return false;
    };
    // IFD ID:
    if (cs.bbcFreqByIdx().contains(cIdx) && 
        cs.bbcFreqByIdx()[cIdx] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ +
        ": IF frequency has changed for BBC #" + bbcId + ": " + 
        QString("").sprintf("%.2f => %.2f", cs.bbcFreqByIdx()[cIdx], d) +
        "; input: \"" + asciifyString(str) + "\"");
    cs.bbcFreqByIdx()[cIdx] = d;
    cs.loIdByIdx()[cIdx] = lst.at(1);

    // bandwidth:
    d = lst.at(2).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ + 
        ": cannot convert \"" + lst.at(2) + "\" to double; input: \"" +
        asciifyString(str) + "\"");
      return false;
    };
    if (cs.bbcBandwidthByIdx().contains(cIdx) &&
        cs.bbcBandwidthByIdx()[cIdx] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ +
        ": IF bandwidth has changed for BBC #" + bbcId + ": " + 
        QString("").sprintf("%.2f => %.2f", cs.bbcBandwidthByIdx()[cIdx], d) +
        "; input: \"" + asciifyString(str) + "\"");
    cs.bbcBandwidthByIdx()[cIdx] = d;
    //
    if (4 < lst.size())
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ + 
        ": got extra data; input: \"" + asciifyString(str) + "\"");
    if (cs.getHwType() == SgChannelSetup::HT_UNDEF)
      cs.setHwType(SgChannelSetup::HT_BBC);
    else if (cs.getHwType() != SgChannelSetup::HT_BBC)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractBbcReading(): station " + stationName_ + 
        ": got an unexpected \"bbc\" record; input: \"" + asciifyString(str) + "\"");
      cs.setHwType(SgChannelSetup::HT_BBC);
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractBbcReading(): station " + stationName_ + 
      ": the number of data less than 3: \"" + bbcStr + "\"");
    return false;
  };
  return true;
};



//
bool SgStnLogCollector::extractVcReading(const QString& str, const QRegularExpressionMatch& match, 
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
//  QString                       vcStr(str.mid(re.indexIn(str) + re.cap(1).size() + 
//                                  re.cap(2).size() + 1));
  QString                       vcStr(str.mid(match.capturedEnd(2) + 1));

  if (!extractEpoch(str, t))
    return false;
    
  if (nearTheEnd <= t)
    return false;
  
  QString                       bbcId(match.captured(2));
  bool                          isOk;
  int                           cIdx;
  cIdx = bbcId.toInt(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractVcReading(): station " + stationName_ + 
      ": cannot convert \"" + bbcId + "\" to int; input: \"" + asciifyString(str) + "\"");
    return false;
  };

  QStringList                   lst=vcStr.split(",");

/*
Syntax:     vcnn=freq,bandwidth,TPIsel,attenU,attenL

Response:   vcnn/freq,bandwidth,TPIsel,attenU,attenl,rem/lcl,LOlock,TP

Settable parameters:

freq      - Synthesizer frequency in MHz, value must be less than 500.00 MHz. No default.
bandwidth - Final filter bandwidth in MHz, default 2. Choices for Mark III are: 0.125, 0.25, 0.5,
            1.0, 2.0, 4.0, 0 (external filter). Choices for Mark IV are: 0.125, 0.5, 2.0, 4.0, 8.0,
            16.0, 0 (external filter). Value may be integer for bandwidths of 1, 2, 4, 8, or 16 MHz.
TPIsel    - Total power integrator selection. Choices are ul, l, u, if, lo, gr (ground). Default u.
attenU    - USB attenuator, 0 or 10 db (default) only.
attenL    - LSB attenuator, 0 or 10 db (default) only.* 
*/
  //
  //2019.003.18:05:50.86&vcsx8/vc01=132.99,8.000,ul
  //2019.003.18:05:50.86&vcsx8/vc02=172.99,8.000,u
  //2019.003.18:05:50.86&vcsx8/vc03=272.99,8.000,u

  if (lst.size())
  {
    double                      d;
    // VC freq:
    d = lst.at(0).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractVcReading(): station " + stationName_ + 
        ": cannot convert \"" + lst.at(0) + "\" to double; input: \"" +
        asciifyString(str) + "\"");
      return false;
    };
    // IFD ID:
    if (cs.bbcFreqByIdx().contains(cIdx) &&
        cs.bbcFreqByIdx()[cIdx] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractVcReading(): station " + stationName_ +
        ": video frequency has changed for IF #" + bbcId + ": " + 
        QString("").sprintf("%.2f => %.2f", cs.bbcFreqByIdx()[cIdx], d) +
        "; input: \"" + asciifyString(str) + "\"");
    cs.bbcFreqByIdx()[cIdx] = d;
    
    // bandwidth:
    d = 2.0;
    if (1 < lst.size())
    {
      d = lst.at(1).toDouble(&isOk);
      if (!isOk)
      {
        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
          "::extractVcReading(): station " + stationName_ + 
          ": cannot convert \"" + lst.at(2) + "\" to double; input: \"" +
          asciifyString(str) + "\"");
        return false;
      };
    };
    if (cs.bbcBandwidthByIdx().contains(cIdx) &&
        cs.bbcBandwidthByIdx()[cIdx] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractVcReading(): station " + stationName_ + 
        ": bandwidth has changed for IF #" + bbcId + ": " + 
        QString("").sprintf("%.2f => %.2f", cs.bbcBandwidthByIdx()[cIdx], d) +
        "; input: \"" + asciifyString(str) + "\"");
    cs.bbcBandwidthByIdx()[cIdx] = d;
    //
    
    if (cs.getHwType() == SgChannelSetup::HT_UNDEF)
      cs.setHwType(SgChannelSetup::HT_VC);
    else if (cs.getHwType() != SgChannelSetup::HT_VC)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractVcReading(): station " + stationName_ + 
        ": got unexpected \"vc\" record; input: \"" + asciifyString(str) + "\"");
      cs.setHwType(SgChannelSetup::HT_VC);
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractVcReading(): station " + stationName_ + 
      ": the number of data less than 2: \"" + vcStr + "\"");
    return false;
  };
  return true;
};



//
bool SgStnLogCollector::extractLoReading(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  QString                       loStr(str.mid(match.capturedEnd(1)));

  if (!extractEpoch(str, t))
    return false;

  if (nearTheEnd <= t)
    return false;

  bool                          isOk;
  QString                       loId(match.captured(2));
  QStringList                   lst=loStr.split(",");

  //2018.003.17:14:29.55&ifdsx/lo=loa,8080.00,usb,rcp,1
  //2018.003.17:14:29.55&ifdsx/lo=lob,8080.00,usb,rcp,1
  //2018.003.17:14:29.55&ifdsx/lo=loc,2020.00,usb,rcp,1
  //2018.003.17:14:29.55&ifdsx/lo=lod,2020.00,usb,rcp,1
  //
  if (4 <= lst.size())
  {
    double                      d;
    d = lst.at(1).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractLoReading(): station " + stationName_ + 
        ": cannot convert \"" + lst.at(1) + "\" to double; input: \"" + asciifyString(str) + "\"");
      return false;
    };

    if (cs.loFreqById().contains(loId) &&
        cs.loFreqById()[loId] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractLoReading(): station " + stationName_ + ": LO frequency changed for LO \"" + loId + 
        "\": " + 
        QString("").sprintf("%.2f", cs.loFreqById()[loId]) + " => " +
        QString("").sprintf("%.2f", d) + "; input: \"" + asciifyString(str) + "\"");

    cs.loFreqById()[loId] = d;
    const QString              &sb=lst.at(2);
    const QString              &pz=lst.at(3);
    
    if (sb == "usb")
      cs.loSideBandById()[loId] = CSB_USB;
    else if (sb == "lsb")
      cs.loSideBandById()[loId] = CSB_LSB;
    else
    {
      cs.loSideBandById()[loId] = CSB_UNDEF;
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractLoReading(): station " + stationName_ + 
        ": cannot figure out side band form the string \"" + asciifyString(str) + "\"");
    };
    // polarization:
    if (pz == "rcp")
      cs.loPolarizationById()[loId] = CP_RightCP;
    else if (pz == "lcp")
      cs.loPolarizationById()[loId] = CP_LeftCP;
    else
    {
      cs.loPolarizationById()[loId] = CP_UNDEF;
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractLoReading(): station " + stationName_ + 
        ": cannot figure out polarization form the string \"" + asciifyString(str) + "\"");
    };
    if (5 <= lst.size())
    {
      d = lst.at(4).toDouble(&isOk);
      if (!isOk)
      {
        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
          "::extractLoReading(): station " + stationName_ + 
          ": cannot convert \"" + lst.at(4) + "\" to double; input: \"" + asciifyString(str) + "\"");
        return false;
      };
      cs.loPcWidthById()[loId] = d;
      // aux:
      if (5 < lst.size())
        logger->write(SgLogger::INF, SgLogger::IO, className() + 
          "::extractLoReading(): station " + stationName_ + 
          ": got extra data; input: \"" + asciifyString(str) + "\"");
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractLoReading(): station " + stationName_ + 
      ": the number of data less than 4: \"" + loStr + "\"");
    return false;
  };

  return true;
};



//
//
bool SgStnLogCollector::extractLoRxgReading(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  QString                       loStr(str.mid(match.capturedEnd(1)));

  if (!extractEpoch(str, t))
    return false;
  if (nearTheEnd <= t)
    return false;

  bool                          isOk;
  QString                       loId(match.captured(2));
  QStringList                   lst=loStr.split(",");

  //2021.190.19:40:27.52/lo/rxg,loa0,3592.4,lcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.190.19:40:27.52/lo/rxg,loa1,3592.4,rcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.190.19:40:27.52/lo/rxg,lob0,3990.4,lcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.190.19:40:27.53/lo/rxg,lob1,3990.4,rcp,bb.rxg,range,1000,10000,2015,3,22,lcp,3.00000e-02,rcp,3.00000e-02
  //2021.190.19:40:27.53/lo/rxg,loc0,10354.4,lcp,undefined
  //2021.190.19:40:27.53/lo/rxg,loc1,10354.4,rcp,undefined
  //2021.190.19:40:27.53/lo/rxg,lod0,10498.4,lcp,undefined
  //2021.190.19:40:27.53/lo/rxg,lod1,10498.4,rcp,undefined
  if (3 <= lst.size())
  {
    double                      d;
    d = lst.at(1).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractLoRxgReading(): station " + stationName_ + 
        ": cannot convert \"" + lst.at(1) + "\" to double; input: \"" + asciifyString(str) + "\"");
      return false;
    };
    if (cs.loFreqById().contains(loId) &&
        cs.loFreqById()[loId] != d &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractLoRxgReading(): station " + stationName_ + ": frequency changed for LO \"" + loId + 
        "\": " + 
        QString("").sprintf("%.2f", cs.loFreqById()[loId]) + " => " +
        QString("").sprintf("%.2f", d) + "; input: \"" + asciifyString(str) + "\"");

    cs.loFreqById()[loId] = d;

    const QString              &pz=lst.at(2);
    SgChannelPolarization       polariz=CP_UNDEF;
    
    if (pz == "rcp")
      polariz = CP_RightCP;
    else if (pz == "lcp")
      polariz = CP_LeftCP;
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractLoRxgReading(): station " + stationName_ + 
        ": cannot figure out polarization form the string \"" + asciifyString(str) + "\"");
    if (cs.loPolarizationById().contains(loId) &&
        cs.loPolarizationById()[loId] != polariz &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractLoRxgReading(): station " + stationName_ + ": polarization changed for LO \"" + 
        loId + "\": " + 
        polarization2Str(cs.loPolarizationById()[loId]) + " => " +
        polarization2Str(polariz) + "; input: \"" + asciifyString(str) + "\"");
    cs.loPolarizationById()[loId] = polariz;
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractLoRxgReading(): station " + stationName_ + 
      ": the number of data less than 3: \"" + loStr + "\"");
    return false;
  };
  return true;
};



//
//
//
bool SgStnLogCollector::extractLoCdasUdc(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  QString                       udcStr(str.mid(match.capturedEnd(1)));

  if (!extractEpoch(str, t))
    return false;
  if (nearTheEnd <= t)
    return false;

  double                        f;
  bool                          isOk;
  QString                       loId(match.captured(2));
  QStringList                   lst=udcStr.split(",");

  if (lst.size() < 2)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractLoCdasUdc(): station " + stationName_ + 
      ": the number of data less than 2: \"" + udcStr + "\"");
    return false;
  };

  //QRegularExpression            reLoCdasUdc("([&].+/\"?udc=)([^,]+),([0-9\\.]+),(\\w)+,(\\w)+.*");
  //2025.336.12:38:02.16&ifdbb/udc=a,2472.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=b,4712.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=c,5832.4,20,20,1
  //2025.336.12:38:02.16&ifdbb/udc=d,9672.4,20,20,1
  
  f = lst.at(1).toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractLoCdasUdc(): station " + stationName_ + 
      ": cannot convert \"" + lst.at(1) + "\" to double; input: \"" + asciifyString(str) + "\"");
    return false;
  };
  //
  if (loId.size() == 1) // current version:
  {
    QString                     loId0(loId + "0");
    QString                     loId1(loId + "1");
    //
    cs.loFreqById()[loId0] = f;
    cs.loFreqById()[loId1] = f;
    //
    cs.loPolarizationById()[loId0] = CP_UNDEF;
    cs.loPolarizationById()[loId1] = CP_UNDEF;
    //
    cs.loSideBandById()[loId0] = CSB_USB;
    cs.loSideBandById()[loId1] = CSB_LSB;
  }
  else if (3 <= lst.size())
  {
    if (cs.loFreqById().contains(loId) &&
        cs.loFreqById()[loId] != f &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractLoCdasUdc(): station " + stationName_ + ": frequency changed for LO \"" + loId + 
        "\": " + 
        QString("").sprintf("%.2f", cs.loFreqById()[loId]) + " => " +
        QString("").sprintf("%.2f", f) + "; input: \"" + asciifyString(str) + "\"");

    cs.loFreqById()[loId] = f;

    const QString              &pz=lst.at(2);
    SgChannelPolarization       polariz=CP_UNDEF;
    
    if (pz == "rcp")
      polariz = CP_RightCP;
    else if (pz == "lcp")
      polariz = CP_LeftCP;
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractLoCdasUdc(): station " + stationName_ + 
        ": cannot figure out polarization form the string \"" + asciifyString(str) + "\"");
    if (cs.loPolarizationById().contains(loId) &&
        cs.loPolarizationById()[loId] != polariz &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractLoCdasUdc(): station " + stationName_ + ": polarization changed for LO \"" + 
        loId + "\": " + 
        polarization2Str(cs.loPolarizationById()[loId]) + " => " +
        polarization2Str(polariz) + "; input: \"" + asciifyString(str) + "\"");
    cs.loPolarizationById()[loId] = polariz;
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractLoCdasUdc(): station " + stationName_ + 
      ": the number of data less than 3: \"" + udcStr + "\"");
    return false;
  };
  return true;
};



//
bool SgStnLogCollector::extractPatchReading(const QString& str, const QRegularExpressionMatch& match, 
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, UNUSED const SgMJD& tFirst)
{
//QString                       patchStr(str.mid(re.indexIn(str) + re.cap(1).size()));
  QString                       patchStr(str.mid(match.capturedEnd(1)));

  if (!extractEpoch(str, t))
    return false;
  if (nearTheEnd <= t)
    return false;

  bool                          isOk;
  QStringList                   lst=patchStr.split(",");

  //2019.003.18:05:52.80&ifdsx/patch=lo1,1l,2l,3h,4h
  //2019.003.18:05:52.80&ifdsx/patch=lo2,9l,10h,11h,12h,13h,14h
  //2019.003.18:05:52.80&ifdsx/patch=lo3,5h,6h,7h,8h
  //
  //2018.094.17:20:22.65&ifdsx/patch=lo1,a1,a2,a3,a4
  //2018.094.17:20:22.65&ifdsx/patch=lo2,b1,b2,b3,b4,b5,b6
  //2018.094.17:20:22.65&ifdsx/patch=lo3,a5,a6,a7,a8

  if (1 < lst.size())
  {
    QString                     loId(lst.at(0));

    for (int i=1; i<lst.size(); i++)
    {
      QString                   cid(lst.at(i));
      QChar                     sb(cid.at(cid.size() - 1));
      if (sb == 'l' || sb == 'h')
      {
        cid.chop(1);
        int                     cIdx=-1;
        cIdx = cid.toInt(&isOk);
        if (!isOk)
        {
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::extractPatchReading(): station " + stationName_ + 
            ": cannot convert \"" + cid + "\" to int; input: \"" + asciifyString(str) + "\"");
        }
        else
          cs.loIdByIdx()[cIdx] = loId;
      }
      else
        cs.loIdByCid()[cid] = loId;
//      else
//        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
//          "::extractPatchReading(): unexpected patch record #" + QString("").setNum(i) + 
//          " \"" + cid + "\"; the string \"" + str + "\"");
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractPatchReading(): station " + stationName_ + 
      ": nothing found: \"" + asciifyString(str) + "\"");
//      ": nothing found: \"" + str + "\"");
    return false;
  };

  return true;
};




//
bool SgStnLogCollector::extractChanSkyFrqReading(const QString& str, 
  const QRegularExpressionMatch& match, 
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
  if (!extractEpoch(str, t))
    return false;
  if (nearTheEnd <= t)
    return false;

  bool                          isOk;
  int                           cIdx=-1;
  double                        skyFrq=-1.0, loFrq=-1.0, videoFrq=-1.0;
  QString                       bbcId(match.captured(1));
  
  cIdx = bbcId.toInt(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractChanSkyFrqReading(): station " + stationName_ + 
      ": cannot get channel index; input: \"" + asciifyString(str) + "\"");
    return isOk;
  };
  skyFrq = match.captured(2).toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractChanSkyFrqReading(): station " + stationName_ + 
      ": cannot get sky frequency; input: \"" + asciifyString(str) + "\"");
    return isOk;
  };
  loFrq = match.captured(3).toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractChanSkyFrqReading(): station " + stationName_ + 
      ": cannot get LO frequency; input: \"" + asciifyString(str) + "\"");
    return isOk;
  };
  videoFrq = match.captured(4).toDouble(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractChanSkyFrqReading(): station " + stationName_ + 
      ": cannot get video frequency; input: \"" + asciifyString(str) + "\"");
    return isOk;
  };
  
  QString                       loId(""), cId("");
  char                          sbIds[2] = {'l', 'u'};
  
  if (loFrq < 4000.0)
    loId = "b";
  else
    loId = "a";

  //2016.236.17:21:21.92&setupsx/"channel  sky freq  lo freq  video
  //2016.236.17:21:21.92&setupsx/"    01   8210.99   8080.00   130.99
  //2016.236.17:21:21.92&setupsx/"    02   8220.99   8080.00   140.99
  //...
  //2016.236.17:21:21.92&setupsx/"    14   2292.99   2020.00   272.99

  for (int i=0; i<2; i++)
  {
    cId = QString("").sprintf("%02d%c", cIdx, sbIds[i]);

    if (cs.bbcFreqByIdx().contains(cIdx) && 
        cs.bbcFreqByIdx()[cIdx] != videoFrq &&
        tFirst < t)
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::extractChanSkyFrqReading(): station " + stationName_ +
        ": video frequency has changed for IF #" + bbcId + ": " +
        QString("").sprintf("%.2f => %.2f", cs.bbcFreqByIdx()[cIdx], videoFrq) +
        "; input: \"" + asciifyString(str) + "\"");

    cs.bbcFreqByIdx()[cIdx] = videoFrq;
    cs.loIdByIdx()[cIdx] = loId;
    cs.loIdByCid()[cId] = loId;
    cs.loFreqById()[loId] = loFrq;
    cs.ifSideBandById()[cId] = (i==0?CSB_LSB:CSB_USB);
    
    if (1.0e-2 < fabs(loFrq + videoFrq - skyFrq)) // check set up:
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractChanSkyFrqReading(): station " + stationName_ + 
        ": got unexpected frequency setup; input: \"" + asciifyString(str) + "\"");
  };
  //
  // just assumption:
  cs.loSideBandById()[loId] = CSB_USB;

  if (cs.getHwType() == SgChannelSetup::HT_UNDEF)
    cs.setHwType(SgChannelSetup::HT_NN);
  else if (cs.getHwType() != SgChannelSetup::HT_NN)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractChanSkyFrqReading(): station " + stationName_ + 
      ": got unexpected \"setup\" record (expected type #" + QString("").setNum(cs.getHwType()) +
      "); input: \"" + asciifyString(str) + "\"");
    cs.setHwType(SgChannelSetup::HT_NN);
  };

  return true;
};



//
bool SgStnLogCollector::extractVciReading(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, const SgMJD& nearTheEnd, SgChannelSetup& cs, const SgMJD& tFirst)
{
//QString                       targetStr(str.mid(re.indexIn(str) + re.cap(1).size() + 
//                                re.cap(2).size() + 1));
  QString                       targetStr(str.mid(match.capturedEnd(2) + 1));

  if (!extractEpoch(str, t))
    return false;

  if (nearTheEnd <= t)
    return false;

  bool                          isOk;
  int                           cIdx;
  QString                       bbcId(match.captured(2));
  cIdx = bbcId.toInt(&isOk);
  if (!isOk)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractVciReading(): station " + stationName_ +
      ": cannot convert \"" + bbcId + "\" to int; input: \"" + asciifyString(str) + "\"");
    return false;
  };
  //2016.236.17:21:21.92&setupsx/vci01=130.99,4r
  //2016.236.17:21:21.92&setupsx/vci02=140.99,4r
  QStringList                   lst=targetStr.split(",");
  if (2 <= lst.size())
  {
    double                      d;
    // video freq:
    d = lst.at(0).toDouble(&isOk);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::extractVciReading(): station " + stationName_ + ": cannot convert \"" + lst.at(0) + 
        "\" to double; input: \"" + asciifyString(str) + "\"");
      return false;
    };
    // IFD ID:
    if (!cs.bbcFreqByIdx().contains(cIdx))
      cs.bbcFreqByIdx()[cIdx] = d;
    else if (cs.bbcFreqByIdx()[cIdx] != d &&
             tFirst < t)
    {
      logger->write(SgLogger::DBG, SgLogger::IO, className() + 
        "::extractVciReading(): station " + stationName_ + 
        ": video frequency has changed for IF #" + bbcId + ": " +
        QString("").sprintf("%.2f => %.2f", cs.bbcFreqByIdx()[cIdx], d) +
        "; input: \"" + asciifyString(str) + "\"");
      cs.bbcFreqByIdx()[cIdx] = d;
    };
    // bandwidth:
    QString                     ss(lst.at(1));
    if (1 < ss.size())
    {
      QString                   pc(ss.at(ss.size() - 1));
      if (pc == "r")
      {
        cs.loPolarizationById()["a"] = CP_RightCP;
        cs.loPolarizationById()["b"] = CP_RightCP;
      }
      else if (pc == "l")
      {
        cs.loPolarizationById()["a"] = CP_LeftCP;
        cs.loPolarizationById()["b"] = CP_LeftCP;
      }
      else
      {
        cs.loPolarizationById()["a"] = CP_UNDEF;
        cs.loPolarizationById()["b"] = CP_UNDEF;
      };

      ss.chop(1);
      d = ss.toDouble(&isOk);
      if (!isOk)
      {
        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
          "::extractVciReading(): station " + stationName_ + ": cannot convert \"" + lst.at(2) + 
          "\" to double; input: \"" + asciifyString(str) + "\"");
        return false;
      };
      cs.bbcBandwidthByIdx()[cIdx] = d;
    };

    if (!cs.loIdByIdx().contains(cIdx))
    {
      cs.loIdByIdx()[cIdx] = "b";
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractVciReading(): station " + stationName_ + ": found unused channel #" + 
        QString("").sprintf("%02d", cIdx) +
        ", attached it to the LO \"b\"; input: \"" + asciifyString(str) + "\"");
    };

    //
    if (cs.getHwType() == SgChannelSetup::HT_UNDEF)
      cs.setHwType(SgChannelSetup::HT_NN);
    else if (cs.getHwType() != SgChannelSetup::HT_NN)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractVciReading(): station " + stationName_ + 
        ": got unexpected \"vci\" record; input: \"" + asciifyString(str) + "\"");
      cs.setHwType(SgChannelSetup::HT_NN);
    };
  }
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractVciReading(): station " + stationName_ + 
      ": the number of data less than 2; input: \"" + asciifyString(str) + "\"");
    return false;
  };
  return true;
};



//
bool SgStnLogCollector::extractTsysReading(const QString& str, const QRegularExpression& reTsys, 
  SgMJD& t, QMap<QString, float>& tSys, const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  float                         f;
  QRegularExpression            reNoData("\\$+");
  QRegularExpression            reNotASensor("i([0-9A-Za-z])+");
  QRegularExpression            reDualSideBand("([0-9A-Za-z])+d");


  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;
  //
  QRegularExpressionMatch       match;
  QString                       tsysStr(str.mid(reTsys.match(str).capturedEnd(1)));

  if (!tsysStr.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractTsysReading(): station " + stationName_ + 
      ": cannot extract tsys data from \"" + asciifyString(str) + "\"");
//      ": cannot extract tsys data from \"" + str + "\"");
    return isOk;
  };
    
  QStringList                   lst=tsysStr.split(",");
  if (lst.size()%2)
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractTsysReading(): station " + stationName_ + 
      ": got odd number of tsys readings: \"" + asciifyString(str) + "\"");
//      ": got odd number of tsys readings: \"" + str + "\"");

  int                           numOfRecs=lst.size()/2;
  for (int i=0; i<numOfRecs; i++)
  {
    const QString              &strIdx=lst.at(2*i);
    const QString              &strVal=lst.at(2*i + 1);
    QString                     chanId(strIdx);
    if (strIdx.size())
    {
      if (strVal.size())
      {
        // skip the averages:
        if (!reNotASensor.match(strIdx).hasMatch())
        {
          if (reNoData.match(strVal).hasMatch())
          {
            f = SgStnLogReadings::dFiller();
            isOk = true;
          }
          else
            f = strVal.toFloat(&isOk);
          if (isOk)
          {
            QString             sensorKey("");
            // special case for sensors with "dual" sideband:
            if ((match=reDualSideBand.match(strIdx)).hasMatch())
            {
              QString           sss(match.captured(1));
              sensorKey = setupTpSensor(sss + "u", "extractTsysReading()", str);
              if (sensorKey.size())
              {
                tSys.insert(sensorKey, f);
                if (!logReadings_.channelSetup().origSensorIdById().contains(strIdx) ||
                    (logReadings_.channelSetup().origSensorIdById().contains(strIdx) &&
                     logReadings_.channelSetup().origSensorIdById().value(strIdx).size()<2))
                  logReadings_.channelSetup().origSensorIdById()[strIdx].append(sss + "u");
              };
              sensorKey = setupTpSensor(sss + "l", "extractTsysReading()", str);
              if (sensorKey.size())
              {
                tSys.insert(sensorKey, f);
                if (!logReadings_.channelSetup().origSensorIdById().contains(strIdx) ||
                    (logReadings_.channelSetup().origSensorIdById().contains(strIdx) &&
                     logReadings_.channelSetup().origSensorIdById().value(strIdx).size()<2))
                  logReadings_.channelSetup().origSensorIdById()[strIdx].append(sss + "l");
              };
            }
            else // regular sensors:
            {
              sensorKey = setupTpSensor(strIdx, "extractTsysReading()", str);
              if (sensorKey.size())
                tSys.insert(sensorKey, f);
            };
          };
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractTsysReading(): station " + stationName_ + ": tsysVal #" + QString("").setNum(i) +
          " is empty of tsys readings: \"" + asciifyString(str) + "\"");
//          " is empty of tsys readings: \"" + str + "\"");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTsysReading(): station " + stationName_ + ": chanKey #" + QString("").setNum(i) +
        " is empty of tsys readings: \"" + asciifyString(str) + "\"");
//        " is empty of tsys readings: \"" + str + "\"");
  };
  return isOk;
};



//
bool SgStnLogCollector::extractTsysVgosReading(const QString& str, const QRegularExpression& reTsys,
  SgMJD& t, QMap<QString, float>& tSys, const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  float                         f;
  QRegularExpression            reNoData("\\$+");
  QRegularExpression            reNoChan("(AV|SM)(\\w)(\\d)");

  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;

  QString                       tsysStr(str.mid(reTsys.match(str).capturedEnd(1)));
  if (!tsysStr.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractTsysVgosReading(): station " + stationName_ + 
      ": cannot extract tsys data from \"" + asciifyString(str) + "\"");
//      ": cannot extract tsys data from \"" + str + "\"");
    return isOk;
  };
    
  QStringList                   lst=tsysStr.split(",");
  if (lst.size()%2)
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractTsysVgosReading(): station " + stationName_ + 
      ": got odd number of tsys readings: \"" + asciifyString(str) + "\"");
//      ": got odd number of tsys readings: \"" + str + "\"");

//2020.021.18:15:39.17#rdtcb#tsys/ 00b0, 53.5, 01b0, 43.6, 02b0, 40.5, 03b0, 41.6, 04b0, 42.7, 05b0, 44.6, 06b0, 44.7, 07b0, 45.6
//2020.021.18:15:39.17#rdtcb#tsys/ 08b0, 46.8, 09b0, 47.7, 10b0, 47.4, 11b0, 50.9, 12b0, 47.7, 13b0, 43.1, 14b0, 46.6, 15b0, 50.2
//2020.021.18:15:39.17#rdtcb#tsys/ AVb0, 45.8, SMb0, 45.7
//2020.021.18:15:39.17#rdtcb#tsys/ 00b1, 53.2, 01b1, 49.8, 02b1, 49.1, 03b1, 49.4, 04b1, 49.5, 05b1, 52.1, 06b1, 53.8, 07b1, 54.7
//2020.021.18:15:39.17#rdtcb#tsys/ 08b1, 57.8, 09b1, 50.6, 10b1, 52.5, 11b1, 49.5, 12b1, 52.3, 13b1, 48.5, 14b1, 48.3, 15b1, 49.5
//2020.021.18:15:39.17#rdtcb#tsys/ AVb1, 51.2, SMb1, 51.2
  
  int                           numOfRecs=lst.size()/2;
  for (int i=0; i<numOfRecs; i++)
  {
    const QString              &strIdx=lst.at(2*i);
    const QString              &strVal=lst.at(2*i + 1);
    if (strIdx.size())
    {
      if (strVal.size())
      {
        // skip the averages:
        if (!reNoChan.match(strIdx).hasMatch() && !reNoData.match(strIdx).hasMatch())
        {
          if (reNoData.match(strVal).hasMatch())
          {
            f = SgStnLogReadings::dFiller();
            isOk = true;
          }
          else
            f = strVal.toFloat(&isOk);
          //
          if (isOk)
          {
            QString               sensorKey=setupTpSensor(strIdx, "extractTsysVgosReading()", str);
            if (sensorKey.size())
              tSys.insert(sensorKey, f);
          };
        };
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractTsysVgosReading(): station " + stationName_ + ": tsysVal #" + QString("").setNum(i) +
          " is empty of tsys readings: \"" + asciifyString(str) + "\"");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTsysVgosReading(): station " + stationName_ + ": chanKey #" + QString("").setNum(i) +
        " is empty of tsys readings: \"" + asciifyString(str) + "\"");
  };

  if (logReadings_.channelSetup().getHwType() == SgChannelSetup::HT_UNDEF)
    logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
  else if (logReadings_.channelSetup().getHwType() != SgChannelSetup::HT_VGOS)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractTsysVgosReading(): station " + stationName_ + 
      ": got an unexpected \"bbc\" record; input: \"" + asciifyString(str) + "\"");
    logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
  };

  return isOk;
};



//
QString SgStnLogCollector::setupTpSensor(const QString& sensorTag, const QString& callerName, 
  const QString& inputString)
{
  const double                  rdbeChannelBandwidth=32.0;
// DBBC3:
//2020.201.05:14:29.04#onoff#VAL casa       290.1 70.9 001l 1 l   3464.40 1.1078 65.39 2466.4  64.613  1.02
// detector ID: "001l -- 064l, 001u -- 064u:
//
// RDBE:
//2021.028.15:55:34.15#rdtcc#tpcont/ 00c0,  23583,  22926, 01c0,  36283,  35218, 02c0,  41679,  40468, 03c0,  41843,  40647, 04c0,  44458,  43162
// detector ID: "00c0 -- 15c0, 00c1 -- 15c1, ..."
//
//2021.004.16:11:41.88#onoff#    source       Az   El  De   I P   Center   Comp   Tsys  SEFD  Tcal(j) Tcal(r)
//2021.004.16:11:41.88#onoff#VAL virgoa     177.3 80.1 1l   1 r   8208.99 0.9961 109.6 2523.7 126.611  0.03

  SgChannelSetup               &cs=logReadings_.channelSetup();
  double                        loFreq=-9999.9, ifFreq=-9999.9, ifBw=-9.9;
  SgChannelSideBand             loSBnd=CSB_UNDEF;
  SgChannelSideBand             ifSBnd=CSB_UNDEF;
  SgChannelPolarization         loPlrz=CP_UNDEF;
  QString                       chanId(sensorTag.simplified().rightJustified(6, ' '));
  QString                       loId;
  QChar                         cSideBand('-');
  QString                       sFreqSfx(""), sPlrzSfx("x"), sLoSBndSfx("x"), sIfSBndSfx("x");
  bool                          isOk;
  int                           sensorIdx;
  
  if (cs.getBackEndType() == SgChannelSetup::BET_RDBE ||
      cs.getBackEndType() == SgChannelSetup::BET_CDAS  )
  {
    QString                     cPlrz=sensorTag.at(sensorTag.size() - 1);
    QString                     cBand=sensorTag.at(sensorTag.size() - 2);
    QString                     sIdx =sensorTag.left(sensorTag.size() - 2);
    loId = QString(cBand) + cPlrz;
    //
    int                         offset;
    //
    if (cBand == 'a')
      offset = 0;
    else if (cBand == 'b')
      offset = 16;
    else if (cBand == 'c')
      offset = 32;
    else if (cBand == 'd')
      offset = 48;
    else if (cBand == 'e')
      offset = 64;
    else if (cBand == 'f')
      offset = 80;
    else if (cBand == 'g')
      offset = 96;
    else if (cBand == 'h')
      offset =112;
    else
    {
      offset = 0;
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": get unexpected LO key \"" + cBand + 
        "\"; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      return "";
    };
    sensorIdx = 0;
    sensorIdx = sIdx.toInt(&isOk);
    if (isOk)
      sensorIdx += offset;
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot convert \"" + sIdx + "\" to int; the tag is \"" + 
        asciifyString(sensorTag) + "\", VGOS; input: \"" + inputString + "\"");
      return "";
    };
    //
    if (cPlrz == '0')
      loPlrz = CP_HorizontalLP;
    else if (cPlrz == '1')
      loPlrz = CP_VerticalLP;
    else
    {
      loPlrz = CP_UNDEF;
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot figure out RDBE polarization from \"" + cPlrz + "\"; the tag is: \"" + 
        asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
    };
    // there is no channel setup:
    if (stationName_ == "ISHIOKA ")
      ifFreq = 512.0 + rdbeChannelBandwidth/2.0 + rdbeChannelBandwidth*(sensorIdx%16);
    else
      ifFreq = 512.0 + rdbeChannelBandwidth/2.0 + rdbeChannelBandwidth*(15 - sensorIdx%16);
    //
    ifSBnd = CSB_USB;
    //
    ifBw = rdbeChannelBandwidth;
  }
  else // 
  {
    QString                     sIdx=sensorTag.left(sensorTag.size() - 1);
    cSideBand = sensorTag.at(sensorTag.size() - 1);
    //
    if (cs.getBackEndType() == SgChannelSetup::BET_DBBC3 ||
        cs.getHwType() == SgChannelSetup::HT_NN           )
    {
      sensorIdx = sIdx.toInt(&isOk);
      if (!isOk)
        sensorIdx = sIdx.toInt(&isOk, 16); //?
    }
    else
      sensorIdx = sIdx.toInt(&isOk, 16);
    if (!isOk)
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot convert \"" + sIdx + "\" to int; the tag is \"" +
        sensorTag + "\", other; input: \"" + inputString + "\"");
      return "";
    };
    // LO id:
    if (cs.loIdByIdx().contains(sensorIdx))
      loId = cs.loIdByIdx()[sensorIdx];
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot find LO id for the sensorIdx " + QString("").setNum(sensorIdx) + 
        "; the tag is \"" + sensorTag + "\"; input: \"" + inputString + "\"");
      return "";
    };
    //
    if (cs.loPolarizationById().contains(loId))
      loPlrz = cs.loPolarizationById().value(loId);
    else
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": LO polarization for LO \"" + loId + "\" is not defined; the tag is: \"" + 
        asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      return "";
    };
    //
    // if frequency:
    if (cs.bbcFreqByIdx().contains(sensorIdx))
      ifFreq = cs.bbcFreqByIdx()[sensorIdx];
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot find BBC frequency for the sensorIdx " + QString("").setNum(sensorIdx) + 
        "; the tag is \"" + sensorTag + "\"; input: \"" + inputString + "\"");
      return "";
    };
    // if bandwidth:
    if (cs.bbcBandwidthByIdx().contains(sensorIdx))
      ifBw = cs.bbcBandwidthByIdx()[sensorIdx];
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot find BBC bandwidth for the sensorIdx " + QString("").setNum(sensorIdx) + 
        "; the tag is \"" + sensorTag + "\"; input: \"" + inputString + "\"");
      return "";
    };
    // if sideband:
    if (cSideBand.toLower() == 'u')
      ifSBnd = CSB_USB;
    else if (cSideBand.toLower() == 'l')
      ifSBnd = CSB_LSB;
    else if (cSideBand.toLower() == 'd')
      ifSBnd = CSB_DUAL;
    else
    {
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": cannot guess sideband; the tag is \"" + sensorTag + "\"; input: \"" + inputString + "\"");
      return "";
    };
  };
  //
  //
  //
  // collect the current LO setup:
  if (cs.loFreqById().contains(loId))
    loFreq = cs.loFreqById().value(loId);
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::setupTpSensor(): station " + stationName_ + ", caller " + callerName + 
      ": LO frequency for LO \"" + loId + "\" is not defined; the tag is: \"" + 
      asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");

  if (cs.loSideBandById().contains(loId))
    loSBnd = cs.loSideBandById().value(loId);
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
      ": LO sideband for LO \"" + loId + "\" is not defined; the tag is: \"" + 
      asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
  //
  //
  //
  // 
  // generate the strings:
  double                        skyFrq;
  skyFrq = SgChannelSetup::calcSkyFreq(loFreq, ifFreq, ifBw, loSBnd, ifSBnd);
  sFreqSfx.sprintf("%08.1f-%08.1f:%05.1f", loFreq, ifFreq, ifBw);
  sPlrzSfx = polarization2Str(loPlrz);
  sLoSBndSfx = sideBand2Str(loSBnd);
  sIfSBndSfx = sideBand2Str(ifSBnd);
  //
  // ok.

  // set up channel key (for sorting):
  QString                       sensorKey("");
  sensorKey.sprintf("%08.1f-", skyFrq);
//sensorKey = sFreqSfx + '-' + sPlrzSfx + '-' + sLoSBndSfx + '-' + sIfSBndSfx + '-' + loId + '-' + chanId;
  sensorKey += sPlrzSfx + '-' + loId + '-' + sLoSBndSfx + '-' + sIfSBndSfx + '-' + chanId + '-' + sFreqSfx;
  
  //
  // set up time-varying info:
  //
  // LO id:
  if (loId.size())
  {
    if (cs.loIdByTpSensorKey().contains(sensorKey))
    {
      // check:
      if (cs.loIdByTpSensorKey()[sensorKey] != loId)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
          ": got new LO ID \"" + loId + "\" for sensor key \"" + sensorKey + 
          "\", the previous one is \"" + cs.loIdByTpSensorKey()[sensorKey] + 
          "\"; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
        cs.loIdByTpSensorKey()[sensorKey] = loId;
      };
    }
    else
      cs.loIdByTpSensorKey()[sensorKey] = loId;
  };
  // LO freq:
  if (-1.0 < loFreq)
  {
    if (cs.loFreqByTpSensorKey().contains(sensorKey))
    {
      // check:
      if (cs.loFreqByTpSensorKey()[sensorKey] != loFreq)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
          ": got new LO frequency " + 
          QString("").sprintf("%8.1f", loFreq) + " for sensor \"" +
          chanId + "\", with the sensor key \"" + sensorKey + 
          "\", the previous one is " + 
          QString("").sprintf("%8.1f", cs.loFreqByTpSensorKey()[sensorKey]) +
          "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
        cs.loFreqByTpSensorKey()[sensorKey] = loFreq;
      };
    }
    else
      cs.loFreqByTpSensorKey()[sensorKey] = loFreq;
  };
  //
  // LO sideband:
  if (cs.loSideBandByTpSensorKey().contains(sensorKey))
  {
    // check:
    if (cs.loSideBandByTpSensorKey()[sensorKey] != loSBnd)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": got new LO sideband " + 
        sideBand2Str(loSBnd) + " for sensor \"" +
        chanId + "\", with the sensor key \"" + sensorKey + 
        "\", the previous one is " + 
        sideBand2Str(cs.loSideBandByTpSensorKey()[sensorKey]) +
        "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      cs.loSideBandByTpSensorKey()[sensorKey] = loSBnd;
    };
  }
  else
    cs.loSideBandByTpSensorKey()[sensorKey] = loSBnd;
  //
  // LO polarization:
  if (cs.loPolarizationByTpSensorKey().contains(sensorKey))
  {
    // check:
    if (cs.loPolarizationByTpSensorKey()[sensorKey] != loPlrz)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": got new LO polarization " + 
        polarization2Str(loPlrz) + " for sensor \"" +
        chanId + "\", with the sensor key \"" + sensorKey + 
        "\", the previous one is " + 
        polarization2Str(cs.loPolarizationByTpSensorKey()[sensorKey]) +
        "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      cs.loPolarizationByTpSensorKey()[sensorKey] = loPlrz;
    };
  }
  else
    cs.loPolarizationByTpSensorKey()[sensorKey] = loPlrz;
  //
  // IF frequency:
  if (-1.0 < ifFreq)
  {
    if (cs.ifFreqByTpSensorKey().contains(sensorKey))
    {
      // check:
      if (cs.ifFreqByTpSensorKey()[sensorKey] != ifFreq)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
          ": got new IF frequency " + 
          QString("").sprintf("%8.1f", ifFreq) + " for sensor \"" +
          chanId + "\", with the sensor key \"" + sensorKey + 
          "\", the previous one is " + 
          QString("").sprintf("%8.1f", cs.ifFreqByTpSensorKey()[sensorKey]) +
          "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
        cs.ifFreqByTpSensorKey()[sensorKey] = ifFreq;
      };
    }
    else
      cs.ifFreqByTpSensorKey()[sensorKey] = ifFreq;
  };
  // IF bandwidth:
  if (-1.0 < ifBw)
  {
    if (cs.ifBandwidthByTpSensorKey().contains(sensorKey))
    {
      // check:
      if (cs.ifBandwidthByTpSensorKey()[sensorKey] != ifBw)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
          ": got new IF bandwidth " + 
          QString("").sprintf("%8.1f", ifBw) + " for sensor \"" +
          chanId + "\", with the sensor key \"" + sensorKey + 
          "\", the previous one is " + 
          QString("").sprintf("%8.1f", cs.ifBandwidthByTpSensorKey()[sensorKey]) +
          "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
        cs.ifBandwidthByTpSensorKey()[sensorKey] = ifBw;
      };
    }
    else
      cs.ifBandwidthByTpSensorKey()[sensorKey] = ifBw;
  };
  //
  // IF id:
  if (cs.ifIdByTpSensorKey().contains(sensorKey))
  {
    // check:
    if (cs.ifIdByTpSensorKey()[sensorKey] != chanId)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": got new ID, " + 
        chanId + ", for the channel key \"" + sensorKey + 
        "\", the previous one is " + cs.ifIdByTpSensorKey()[sensorKey] +
        "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      cs.ifIdByTpSensorKey()[sensorKey] = chanId;
    };
  }
  else
    cs.ifIdByTpSensorKey()[sensorKey] = chanId;
  //
  //
  // done with time varying setup
  //
  //
  // IF index:
  if (cs.ifIdxById().contains(chanId))
  {
    // check:
    if (cs.ifIdxById()[chanId] != sensorIdx)
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
        ": got new value, " + 
        QString("").setNum(sensorIdx) + ", for the channel id \"" + chanId + 
        "\", the previous one is " + 
        QString("").setNum(cs.ifIdxById()[chanId]) +
        "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
      cs.ifIdxById()[chanId] = sensorIdx;
    };
  }
  else
    cs.ifIdxById()[chanId] = sensorIdx;
  //
  //
  // LO id:
//  if (!cs.loIdByIdx().contains(sensorIdx))
//    cs.loIdByIdx()[sensorIdx] = loId;
  //
  //
  if (ifSBnd != CSB_UNDEF)
  {
    if (cs.ifSideBandById().contains(chanId))
    {
      // check:
      if (cs.ifSideBandById()[chanId] != ifSBnd)
      {
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::setupTpSensor(): station " + stationName_ +  ", caller " + callerName + 
          ": got new sideband, " + 
          sideBand2Str(ifSBnd) + ", for the channel id \"" + chanId + 
          "\", the previous one is " + 
          sideBand2Str(cs.ifSideBandById()[chanId]) +
          "; the tag is \"" + asciifyString(sensorTag) + "\"; input: \"" + inputString + "\"");
        cs.ifSideBandById()[chanId] = ifSBnd;
      };
    }
    else
      cs.ifSideBandById()[chanId] = ifSBnd;
  };
  //

  return sensorKey;
};



//
bool SgStnLogCollector::extractTpcontVgosReading(const QString& str, const QRegularExpression& reTpcont,
  SgMJD& t, QMap<QString, QVector<int> >& tPcont, const SgMJD& tAtTheBegin)
//  SgMJD& t, QMap<char*, int*>& tPcont, const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  int                           nOn, nOf;
  QRegularExpression            reNoData("\\$+");
  QVector<int>                  v(2);
  
  //
  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;

  QString                       tpcontStr(str.mid(reTpcont.match(str).capturedEnd(1)));

  if (!tpcontStr.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractTpcontVgosReading(): station " + stationName_ + 
      ": cannot extract tpcont data from \"" + asciifyString(str) + "\"");
//      ": cannot extract tpcont data from \"" + str + "\"");
    return isOk;
  };
    
  QStringList                   lst=tpcontStr.split(",");
  if (lst.size()%3)
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractTpcontVgosReading(): station " + stationName_ + 
      ": got odd number of tpcont readings: \"" + asciifyString(str) + "\"");
//      ": got odd number of tpcont readings: \"" + str + "\"");

  //2020.021.18:00:00.17#rdtcd#tpcont/ 00d0,  12501,  12325, 01d0,  16054,  15765, 02d0,  20774,  20410, 03d0,  24388,  23985, 04d0,  27599,  27153
  //2020.021.18:00:00.17#rdtcd#tpcont/ 05d0,  28085,  27647, 06d0,  29475,  29027, 07d0,  30963,  30479, 08d0,  29392,  28953, 09d0,  30078,  29643
  //2020.021.18:00:00.17#rdtcd#tpcont/ 10d0,  37494,  36960, 11d0,  42335,  41705, 12d0,  46499,  45802, 13d0,  51354,  50535, 14d0,  49839,  48925
  //2020.021.18:00:00.17#rdtcd#tpcont/ 15d0,  43153,  42272
  //2020.021.18:00:00.17#rdtcd#tpcont/ 00d1,  11542,  11379, 01d1,  13078,  12877, 02d1,  16776,  16491, 03d1,  22232,  21871, 04d1,  27568,  27119
  //2020.021.18:00:00.17#rdtcd#tpcont/ 05d1,  26698,  26274, 06d1,  26024,  25611, 07d1,  27530,  27137, 08d1,  33180,  32491, 09d1,  32567,  32122
  //2020.021.18:00:00.17#rdtcd#tpcont/ 10d1,  42659,  42099, 11d1,  46937,  46283, 12d1,  50407,  49687, 13d1,  56087,  55247, 14d1,  56042,  55150
  //2020.021.18:00:00.17#rdtcd#tpcont/ 15d1,  47024,  46233
 
  int                           numOfRecs=lst.size()/3;
  for (int i=0; i<numOfRecs; i++)
  {
    const QString              &strIdx=lst.at(3*i);
    const QString              &strValOn=lst.at(3*i + 1);
    const QString              &strValOf=lst.at(3*i + 2);
    //
    if (strIdx.size() && strValOn.size() && strValOf.size())
    {
      if (reNoData.match(strValOn).hasMatch())
      {
        nOn = SgStnLogReadings::nFiller();
        isOk = true;
      }
      else
        nOn = strValOn.toInt(&isOk);

      if (isOk && reNoData.match(strValOf).hasMatch())
      {
        nOf = SgStnLogReadings::nFiller();
        isOk = true;
      }
      else
        nOf = strValOf.toInt(&isOk);
      //
      if (isOk)
      {
        QString                 sensorKey=setupTpSensor(strIdx, "extractTpcontVgosReading()", 
          asciifyString(str));
        //
        v[0] = nOn;
        v[1] = nOf;
        if (sensorKey.size())
          tPcont.insert(sensorKey, QVector<int>(v));
      };
    }
    else
    {
      if (strIdx.size() == 0)
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractTpcontVgosReading(): station " + stationName_ + ": chanKey #" + 
            QString("").setNum(i) + " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
      if (strValOn.size())
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractTpcontVgosReading(): station " + stationName_ + ": tpcalon #" + 
          QString("").setNum(i) + " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
      if (strValOf.size())
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractTpcontVgosReading(): station " + stationName_ + ": tpcaloff #" + 
          QString("").setNum(i) + " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
    };
  };

  if (logReadings_.channelSetup().getHwType() == SgChannelSetup::HT_UNDEF)
    logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
  else if (logReadings_.channelSetup().getHwType() != SgChannelSetup::HT_VGOS)
  {
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractTpcontVgosReading(): station " + stationName_ + 
      ": got an unexpected \"bbc\" record; input: \"" + asciifyString(str) + "\"");
    logReadings_.channelSetup().setHwType(SgChannelSetup::HT_VGOS);
  };

  return isOk;
};



//
bool SgStnLogCollector::extractPcalVgosReading(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, QMap<QString, float*>& pcal, SgChannelSetup& cs, const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  float                         dAmp, dPhs;
  QRegularExpression            reNoData("\\$+");

  //
  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;

  QString                       pcalStr(str.mid(match.capturedEnd(1)));

  if (!pcalStr.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::extractPcalVgosReading(): station " + stationName_ + 
      ": cannot extract tpcont data from \"" + asciifyString(str) + "\"");
//    ": cannot extract tpcont data from \"" + str + "\"");
    return isOk;
  };
    
  QStringList                   lst=pcalStr.split(",");
  if (lst.size()%3)
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractPcalVgosReading(): station " + stationName_ + 
      ": got odd number of tpcont readings: \"" + asciifyString(str) + "\"");
//      ": got odd number of tpcont readings: \"" + str + "\"");

  //2020.195.11:30:01.17#rdtca#pcal/ 1a0000,   8.686,  -98.7, 1a0005,  78.913,  116.7, 1a0010,   8.729,  147.9, 1a0015,  81.273,    4.1
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0020,   9.421,   37.5, 1a0025,  88.580, -105.5, 1a0030,  10.177,  -72.2, 1a0035,  97.212,  144.1
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0040,  11.646,  176.3, 1a0045,  89.248,   33.1, 1a0050,  10.577,   65.5, 1a0055,  93.330,  -79.2
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0060,  10.255,  -47.4, 1a0065,  93.203,  169.1, 1a0070,  10.437, -155.9, 1a0075,  89.414,   59.5
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0080,  10.014,   92.8, 1a0085,  84.492,  -47.7, 1a0090,   9.561,  -14.2, 1a0095,  82.770, -155.3
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0100,   8.782, -120.1, 1a0105,  79.763,  101.6, 1a0110,   8.836,  137.8, 1a0115,  77.939,   -3.8
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0120,   8.740,   32.5, 1a0125,  73.421, -105.2, 1a0130,   8.134,  -68.7, 1a0135,  74.094,  150.8
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0140,   7.557, -168.7, 1a0145,  69.966,   50.6, 1a0150,   7.977,   88.8, 1a0155,  72.224,  -54.9
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0160,   8.275,  -19.4, 1a0165,  74.537, -158.0, 1a0170,   8.204, -123.8, 1a0175,  72.798,   88.5
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0180,   8.256,  131.2, 1a0185,  73.794,   -6.5, 1a0190,   7.957,   28.8, 1a0195,  74.957, -109.7
  //2020.195.11:30:01.17#rdtca#pcal/ 1a0200,   8.315,  -75.6, 1a0205,  73.965,  144.7, 1a0210,   8.324,  179.7, 1a0215,  74.379,   38.9

  int                           numOfRecs=lst.size()/3;
  for (int i=0; i<numOfRecs; i++)
  {
    const QString              &strIdx=lst.at(3*i).simplified();
    const QString              &strValAm=lst.at(3*i + 1);
    const QString              &strValPh=lst.at(3*i + 2);
    QString                     chanId(strIdx.simplified().rightJustified(8, ' '));

    //
    if (strIdx.size() && strValAm.size() && strValPh.size())
    {
      if (reNoData.match(strValAm).hasMatch())
      {
        dAmp = SgStnLogReadings::dFiller();
        isOk = true;
      }
      else
        dAmp = strValAm.toFloat(&isOk);

      if (isOk && reNoData.match(strValPh).hasMatch())
      {
        dPhs = SgStnLogReadings::dFiller();
        isOk = true;
      }
      else
        dPhs = strValPh.toFloat(&isOk);
      //
      if (isOk)
      {
        QChar                   cPlrz(strIdx.at(0));
        QChar                   cBand(strIdx.at(1));
        QString                 loId(cBand);
        QString                 sensorKey("");
        QString                 sFreqOffset("");
        QString                 sSBndSfx("");
        SgChannelSideBand       sbd=CSB_UNDEF;
        double                  loFreq;
        double                  freqOffset;
        double                  pcalWidth;
        bool                    hasPcalOffset;
        loFreq = freqOffset = pcalWidth = SgStnLogReadings::dFiller();
        hasPcalOffset = false;
        loId += cPlrz;

        //
        // get LO's freq:
        if (cs.loFreqById().contains(loId))
          loFreq = cs.loFreqById().value(loId);
        else
          logger->write(SgLogger::WRN, SgLogger::IO, className() + 
            "::extractPcalVgosReading(): station " + stationName_ + ": " + 
            "frequency for LO \"" + loId + "\" is not defined; the tag is: \"" + loId + 
            "\"; input: \"" + asciifyString(str) + "\"");
        //
        // get LO's sideband:
        if (cs.loSideBandById().contains(loId))
          sbd = cs.loSideBandById().value(loId);
        else
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::extractPcalVgosReading(): station " + stationName_ +
            ": LO sideband for LO \"" + loId + "\" is not defined; the tag is: \"" +
            chanId + "\"; input: \"" + asciifyString(str) + "\"");
        //
        // get pcal width from LO setup:
        if (cs.loPcWidthById().contains(loId))
          pcalWidth = cs.loPcWidthById().value(loId);
        else
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::extractPcalVgosReading(): station " + stationName_ +
            ": pcal width for LO \"" + loId + "\" is not defined; the tag is: \"" +
            chanId + "\"; input: \"" + asciifyString(str) + "\"");
        //
        // get pcal offset:
        if (cs.pcalOffsetByLoId().contains(loId))
        {
          freqOffset = cs.pcalOffsetByLoId().value(loId);
          hasPcalOffset = true;
        }
        else if (cs.pcalOffsetByLoId().contains(QString(cBand)))
        {
          freqOffset = cs.pcalOffsetByLoId().value(QString(cBand));
          hasPcalOffset = true;
        }
        else if (cs.pcalOffsetByLoId().contains("*"))
        {
          freqOffset = cs.pcalOffsetByLoId().value("*");
          hasPcalOffset = true;
        }
        else
          logger->write(SgLogger::WRN, SgLogger::IO, className() +
            "::extractPcalVgosReading(): station " + stationName_ +
            ": pcal offset for LO \"" + loId + "\" is not defined; the tag is: \"" +
            chanId + "\"; input: \"" + asciifyString(str) + "\"");
        //
        //
        //   setup a sensor key:
        sFreqOffset.sprintf("%04.1f:%04.1f", freqOffset, pcalWidth);
        sensorKey.sprintf("%08.1f-", loFreq);
        sSBndSfx = sideBand2Str(sbd);
        sensorKey += cPlrz + '-' + loId + '-' + sSBndSfx + '-' + chanId + '-' + sFreqOffset;
        //
        //
        //
        //
        if (logReadings_.channelSetup().pcalIdByKey().contains(sensorKey))
        {
          // check:
          if (logReadings_.channelSetup().pcalIdByKey()[sensorKey] != chanId)
          {
            logger->write(SgLogger::WRN, SgLogger::IO, className() + 
              "::extractPcalVgosReading(): station " + stationName_ + ": get new id, " + 
              chanId + ", for the channel key \"" + sensorKey + 
              "\", the previous one is " + 
              logReadings_.channelSetup().pcalIdByKey()[sensorKey] +
              "; input: \"" + asciifyString(str) + "\"");
            logReadings_.channelSetup().pcalIdByKey()[sensorKey] = chanId;
          };
        }
        else
          logReadings_.channelSetup().pcalIdByKey()[sensorKey] = chanId;
        // 
        //
        //
        // LO freq:
        if (logReadings_.channelSetup().pcalLoFreqByKey().contains(sensorKey))
        {
          // check:
          if (logReadings_.channelSetup().pcalLoFreqByKey()[sensorKey] != loFreq)
          {
            logger->write(SgLogger::DBG, SgLogger::IO, className() + 
              "::extractPcalVgosReading(): station " + stationName_ + ": LO frequency changed from " + 
              QString("").sprintf("%.1f to %.1f", 
                logReadings_.channelSetup().pcalLoFreqByKey()[sensorKey], loFreq) + 
              " for the pcal key \"" + sensorKey + "; input: \"" + asciifyString(str) + "\"");
            logReadings_.channelSetup().pcalLoFreqByKey()[sensorKey] = loFreq;
          };
        }
        else if (0.0 < loFreq) // if it is defined:
          logReadings_.channelSetup().pcalLoFreqByKey()[sensorKey] = loFreq;
        //
        // LO sideband:
        if (logReadings_.channelSetup().pcalSideBandByKey().contains(sensorKey))
        {
          // check:
          if (logReadings_.channelSetup().pcalSideBandByKey()[sensorKey] != sbd)
          {
            logger->write(SgLogger::DBG, SgLogger::IO, className() + 
              "::extractPcalVgosReading(): station " + stationName_ + ": LO sideband changed from " + 
              sideBand2Str(logReadings_.channelSetup().pcalSideBandByKey()[sensorKey]) + " to " +
              sideBand2Str(sbd) +
              " for the pcal key \"" + sensorKey + "; input: \"" + asciifyString(str) + "\"");
            logReadings_.channelSetup().pcalSideBandByKey()[sensorKey] = sbd;
          };
        }
        else if (sbd != CSB_UNDEF)
          logReadings_.channelSetup().pcalSideBandByKey()[sensorKey] = sbd;
        //
        // polarization:         
        if (cPlrz == '0')
          logReadings_.channelSetup().pcalPolarizationByKey()[sensorKey] = CP_HorizontalLP;
        else if (cPlrz == '1')
          logReadings_.channelSetup().pcalPolarizationByKey()[sensorKey] = CP_VerticalLP;
        else
        {
          logReadings_.channelSetup().pcalPolarizationByKey()[sensorKey] = CP_UNDEF;
          logger->write(SgLogger::WRN, SgLogger::IO, className() + 
            "::extractPcalVgosReading(): station " + stationName_ + 
            ": cannot figure out polarization from \"" + strIdx +
            "\"; input: \"" + asciifyString(str) + "\"");
        };
        //
        // frequency offset:
        if (logReadings_.channelSetup().pcalOffsetByKey().contains(sensorKey))
        {
          // check:
          if (logReadings_.channelSetup().pcalOffsetByKey()[sensorKey] != freqOffset)
          {
            logger->write(SgLogger::DBG, SgLogger::IO, className() +
              "::extractPcalVgosReading(): station " + stationName_ + ": frequency offset changed from " +
              QString("").sprintf("%.1f to %.1f",
                logReadings_.channelSetup().pcalOffsetByKey()[sensorKey], freqOffset) +
              " for the pcal key \"" + sensorKey + "; input: \"" + asciifyString(str) + "\"");
            logReadings_.channelSetup().pcalOffsetByKey()[sensorKey] = freqOffset;
          };
        }
        else if (hasPcalOffset) // if it is defined:
          logReadings_.channelSetup().pcalOffsetByKey()[sensorKey] = freqOffset;
        //
        // pcal width:
        if (logReadings_.channelSetup().pcalWidthByKey().contains(sensorKey))
        {
          // check:
          if (logReadings_.channelSetup().pcalWidthByKey()[sensorKey] != pcalWidth)
          {
            logger->write(SgLogger::DBG, SgLogger::IO, className() +
              "::extractPcalVgosReading(): station " + stationName_ + ": width changed from " +
              QString("").sprintf("%.1f to %.1f",
                logReadings_.channelSetup().pcalWidthByKey()[sensorKey], pcalWidth) +
              " for the pcal key \"" + sensorKey + "; input: \"" + asciifyString(str) + "\"");
            logReadings_.channelSetup().pcalWidthByKey()[sensorKey] = pcalWidth;
          };
        }
        else if (0.0 < pcalWidth) // if it is defined:
          logReadings_.channelSetup().pcalWidthByKey()[sensorKey] = pcalWidth;
        //
        //
        //
        float                  *f=new float[2];
        *f      = dAmp;
        *(f+1)  = dPhs;
        pcal.insert(logReadings_.channelSetup().pcalIdByKey()[sensorKey], f);
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::extractPcalVgosReading(): station " + stationName_ + ": cannot convert str \"" +
          strIdx.mid(2) + "\" to int; input: \"" + asciifyString(str) + "\"");
    }
    else
    {
      if (strIdx.size() == 0)
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractPcalVgosReading(): station " + stationName_ + ": sensorKey #" + QString("").setNum(i) +
          " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
      if (strValAm.size())
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractPcalVgosReading(): station " + stationName_ + ": amplitude #" + QString("").setNum(i) +
          " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
      if (strValPh.size())
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractPcalVgosReading(): station " + stationName_ + ": phase #" + QString("").setNum(i) +
          " is empty of tpcont readings: \"" + asciifyString(str) + "\"");
//          " is empty of tpcont readings: \"" + str + "\"");
    };
  };
  return isOk;
};



//
bool SgStnLogCollector::extractDot2xpsVgosReading(const QString& str, 
  const QRegularExpressionMatch& match, SgMJD& t, QString& sKey, float& dot2xps, 
  const SgMJD& tAtTheBegin, bool& isGps)
{
  bool                          isOk=false;
  float                         f;
  //
  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;

  sKey = match.captured(1);
  QString                       cap2(match.captured(2));
  QString                       cap3(match.captured(3));
  
  isGps = cap2 == "g";
  f = cap3.toFloat(&isOk);
  if (isOk)
    dot2xps = f;
  else
    logger->write(SgLogger::WRN, SgLogger::IO, className() + 
      "::extractDot2xpsVgosReading(): station " + stationName_ + ": cannot convert \"" + cap3 +
      "\" to double; input: \"" + asciifyString(str) + "\"");

  return isOk;
};





//
bool SgStnLogCollector::extractTraklReading(const QString& str, const QRegularExpressionMatch& match, 
  SgMJD& t, SgTraklReading& tr, const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  double                        d;
  QString                       s("");

  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;
//                                   1           2            3                4              5       6        7
//2020.203.18:21:45.11#trakl#[az   10.0149 el  50.0437 azv   0.00000000 elv   0.00000000 mjd 59051 s 66104 ms 980 ]

  tr.setT(t);
  //
  if ((s=match.captured(1)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTraklReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else 
      tr.setAz(d);
  };
  if ((s=match.captured(2)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTraklReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else 
      tr.setEl(d);
  };
  if ((s=match.captured(3)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTraklReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else 
      tr.setAzv(d);
  };
  if ((s=match.captured(4)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractTraklReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else 
      tr.setElv(d);
  };
  return isOk;
};



//
bool SgStnLogCollector::extractSefdReading(const QString& str, const QRegularExpressionMatch& match, 
  SgMJD& t, QString& sensorId, QString& srcName, double& az, double& de, QVector<double>& vec, 
  const SgMJD& tAtTheBegin)
{
  bool                          isOk=false;
  double                        d;
  QString                       sensorTag(match.captured(4));
  QString                       s("");
//QRegularExpression            notATag("ia|ib|ic|id|ie|if|ig|ih");
  QRegularExpression            notATag("i[A-Za-z0-9]");

  if (!extractEpoch(str, t))
    return false;
  if (t < tAtTheBegin)
    return false;
  if (notATag.match(sensorTag).hasMatch())
    return false;
  
//                                1           2     3    4  5 6       7     8       9     10     11     12
//2020.034.17:11:35.51#onoff#VAL casa       295.4 63.4 001l 1 l   3464.40 0.9025 62.33 2139.2  58.787  0.93
//2020.034.17:11:35.51#onoff#VAL casa       295.4 63.4 002l 1 l   3432.40 0.8950 63.19 2407.4  65.123  1.03

  srcName = match.captured(1);
  QString                       sensorKey=setupTpSensor(sensorTag, "extractSefdReading()", str);
  if (!sensorKey.size())
    return false;

//sensorId = logReadings_.channelSetup().ifIdBySensorKey()[chanKey];
  sensorId = sensorKey;

  //
  if ((s=match.captured(2)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractSefdReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else
      az = d;
  };
  //
  if ((s=match.captured(3)).size())
  {
    d = s.toDouble(&isOk);
    if (!isOk)
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::extractSefdReading(): station " + stationName_ + 
        ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
    else
      de = d;
  };
  vec.resize(6);
  for (int i=0; i<6; i++)
  {
    if ((s=match.captured(7 + i)).size())
    {
      d = s.toDouble(&isOk);
      if (!isOk)
        logger->write(SgLogger::WRN, SgLogger::IO, className() + 
          "::extractSefdReading(): station " + stationName_ + 
          ": cannot convert to double: \"" + s + "\"; input: \"" + asciifyString(str) + "\"");
      else
        vec[i] = d;
    };
  };
  return isOk;
};



//
bool SgStnLogCollector::extractCableCalibration(const QString& str, const QRegularExpressionMatch& match,
  SgMJD& t, double& v)
{
  if (!extractEpoch(str, t))
    return false;

  bool                          isOk=true;
  QString                       strCableVal(match.captured(2));


  if (strCableVal.endsWith('-'))
    strCableVal.chop(1);
  v = strCableVal.toDouble(&isOk);
  if (!isOk)
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::extractCableCalibration(): failed to extract cable calibration value from \"" + 
      strCableVal + "\"");
  else if (fabs(v) > 1.0e6) // is it enough?
  {
    v = 0.0;
    isOk = false;
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::extractCableCalibration(): the exctracted value looks suspicious: \"" + 
      strCableVal + "\", skipped");
  };
//  else
//    v /= 4.0e5;
  return isOk;
};



//
bool SgStnLogCollector::extractDataFromPcmtRecord(const QString& str, 
  const QRegularExpressionMatch& match, SgMJD& t, double& v, QString& source, QString& scan)
{
  int                           nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0;
  double                        dSecond=0.0, f=0.0;
  bool                          isOk=true;

  
  nYear = match.captured(1).toInt(&isOk);
  if (isOk)
  {
    nMonth = match.captured(2).toInt(&isOk);
    if (isOk)
    {
      nDay = match.captured(3).toInt(&isOk);
      if (isOk)
      {
        nHour = match.captured(4).toInt(&isOk);
        if (isOk)
        {
          nMin = match.captured(5).toInt(&isOk);
          if (isOk)
          {
            dSecond = match.captured(6).toDouble(&isOk);
            if (isOk)
              t.setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSecond);
          };
        };
      };
    };
  };
  if (!isOk)
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::extractDataFromPcmtRecord(): failed to extract epoch from the string \"" + asciifyString(str) + "\"");
//      "::extractDataFromPcmtRecord(): failed to extract epoch from the string \"" + str + "\"");
  else
  {
    f = match.captured(7).toDouble(&isOk);
    if (isOk)
    {
      v = f;
      source = match.captured(8);
      if (match.lastCapturedIndex() >= 9)
        scan = match.captured(9);
      else
        std::cout << "   --- no scan\n";
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::extractDataFromPcmtRecord(): failed to extract cable cal value from the string \"" + 
        asciifyString(str) + "\"");
//        str + "\"");
  };
  return isOk;
};



//
bool SgStnLogCollector::extractDataFromMetRecord(const QString& str, 
  const QRegularExpressionMatch& match, SgMJD& t, SgMeteoData& m)
{
  int                           nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0;
  double                        dSecond=0.0, f=0.0;
  bool                          isOk=true;

  nYear = match.captured(1).toInt(&isOk);
  if (isOk)
  {
    nMonth = match.captured(2).toInt(&isOk);
    if (isOk)
    {
      nDay = match.captured(3).toInt(&isOk);
      if (isOk)
      {
        nHour = match.captured(4).toInt(&isOk);
        if (isOk)
        {
          nMin = match.captured(5).toInt(&isOk);
          if (isOk)
          {
            dSecond = match.captured(6).toDouble(&isOk);
            if (isOk)
              t.setUpEpoch(nYear, nMonth, nDay, nHour, nMin, dSecond);
          };
        };
      };
    };
  };
  if (!isOk)
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::extractDataFromMetRecord(): failed to extract epoch from the string \"" + 
      asciifyString(str) + "\"");
//      "::extractDataFromMetRecord(): failed to extract epoch from the string \"" + str + "\"");
  else
  {
    f = match.captured(7).toDouble(&isOk);
    if (isOk)
    {
      m.setPressure(f);
      f = match.captured(8).toDouble(&isOk);
      if (isOk)
      {
        m.setTemperature(f);
        f = match.captured(9).toDouble(&isOk);
        if (isOk)
          m.setRelativeHumidity(f*0.01);
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() +
            "::extractDataFromMetRecord(): failed to extract relative humidity from the string \"" + 
            asciifyString(str) + "\"");
//            str + "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO, className() +
          "::extractDataFromMetRecord(): failed to extract pressure from the string \"" + 
          asciifyString(str) + "\"");
//          str + "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() +
        "::extractDataFromMetRecord(): failed to extract temperature from the string \"" + 
        asciifyString(str) + "\"");
//        str + "\"");
  };
  return isOk;
};



//
bool SgStnLogCollector::extractCableCalibrationSignByDiff(const QRegularExpressionMatch& match, int& v)
{
  double                        d;
  bool                          isOk;

  QString                       strCableDiff(match.captured(2));
  QString                       strCableSign(match.captured(3));

  d = strCableDiff.toDouble(&isOk);
  if (isOk)
  {
    if (fabs(d) < 1.0e6) // is it enough?
    {
      if (strCableSign.contains("-"))
        v = -1;
      else if (strCableSign.contains("+"))
        v = 1;
      else
        logger->write(SgLogger::WRN, SgLogger::IO, className() +
          "::extractCableCalibrationSignByDiff(): get unusable cable sign char sequence: \"" + 
          strCableSign + "\"");
    }
    else
    {
      isOk = false;
      logger->write(SgLogger::WRN, SgLogger::IO, className() +
        "::extractCableCalibrationSignByDiff(): the cable diff is too big: \"" + strCableDiff + "\"");
    };
  }
  else 
    logger->write(SgLogger::WRN, SgLogger::IO, className() +
      "::extractCableCalibrationSignByDiff(): cannot convert diff to double: \"" + strCableDiff + "\"");
  return isOk;
};



//
bool SgStnLogCollector::checkRinexFile(const QString& fileName, const SgMJD& tStart,
  const SgMJD& tFinis, const QString& rinexStnName)
{
  const QString                 eoh("END OF HEADER");
  QString                       path2Rinex("");
  QString                       str("");
  int                           idx=fileName.lastIndexOf("/");
  if (idx>-1)
    path2Rinex = fileName.left(idx);

  QDir                          dir(path2Rinex);
  QStringList                   nameFilters;
//  nameFilters << "sa01*0.??m";
  nameFilters << rinexStnName + "*.??m";
  QStringList                   nameList=dir.entryList(nameFilters, 
                              QDir::Files | QDir::Readable, QDir::Name | QDir::IgnoreCase);
  if (!nameList.size())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::checkRinexFile(): no RINEX file found");
    return false;
  }
  else
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::checkRinexFile(): processing RINEX files for the station " + stationName_);
//          1         2         3         4
//0123456789012345678901234567890123456789012345678901234567890123456789
// 09  9 24  0  1  1 1000.0   22.4   76.7
// 13  6  3  0  1  1  993.3   23.5   76.1
// 18  2 15  0  0  0 1010.5   22.9  100.6    0.0    0.0    0.0    0.0
// 18  5 14  0  0  0 1009.5   27.0   96.3-9999.9-9999.9-9999.9-9999.9
// 18  5 14  0  1  0 1009.5   27.0   96.3-9999.9-9999.9-9999.9-9999.9
  QRegExp                       reData("\\s*(\\d{1,2})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                  "(\\d{1,2})\\s+(\\d{1,2})\\s+(\\d{1,2})\\s+"
                                  "(\\d{1,4}\\.\\d+)\\s+([+-\\.\\d]+)\\s+([+-]?[\\.\\d]+)(?:\\s+|$|-99)",
                                  Qt::CaseInsensitive);
  int                           nYr, nMn, nDy, nHr, nMi, nSc;
  double                        fPr, fTp, fRh;
  SgMeteoData                   m;
  bool                          isOk;
  logReadings_.meteoPars().clear();
  QFile                         f;
  double                        dP=0.0;
  //
  if (rinexPressureOffsetByStn_ && rinexPressureOffsetByStn_->contains(stationName_))
  {
    dP = rinexPressureOffsetByStn_->value(stationName_);
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::checkRinexFile(): using offset of " + QString("").setNum(dP) + " mbar for atmospheric "
      "pressure of \"" + stationName_ + "\" station");
  };
  //
  for (int i=0; i<nameList.size(); i++)
  {
    const QString              &fn=nameList.at(i);

    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::checkRinexFile(): processing " + fn + " file");
    str = "";
    idx = 0;
    f.setFileName(path2Rinex + "/" + fn);
    if (f.open(QFile::ReadOnly))
    {
      QTextStream               s(&f);
      int                       count=0;
      while (!s.atEnd() && !str.contains(eoh))
      {
        str = s.readLine();
        idx++;
      };
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
        "::checkRinexFile(): skipped " + QString("").setNum(idx) + " strings");
      while (!s.atEnd())
      {
        nYr = nMn = nDy = nHr = nMi = nSc = 0;
        fPr = fTp = fRh = 0.0;
        str = s.readLine();
        if (reData.indexIn(str) != -1)
        { 
          nYr = reData.cap(1).toInt(&isOk);
          if (isOk)
          {
            nMn = reData.cap(2).toInt(&isOk);
            if (isOk)
            {
              nDy = reData.cap(3).toInt(&isOk);
              if (isOk)
              {
                nHr = reData.cap(4).toInt(&isOk);
                if (isOk)
                {
                  nMi = reData.cap(5).toInt(&isOk);
                  if (isOk)
                  {
                    nSc = reData.cap(6).toInt(&isOk);
                    if (isOk)
                    {
                      fPr = reData.cap(7).toDouble(&isOk);
                      if (isOk)
                      {
                        fTp = reData.cap(8).toDouble(&isOk);
                        if (isOk)
                        {
                          fRh = reData.cap(9).toDouble(&isOk);
                          if (isOk)
                          {
                            SgMJD t(nYr, nMn, nDy, nHr, nMi, (double)nSc);
                            if (tStart<=t && t<= tFinis)
                            {
                              m.setTemperature(fTp);
                              m.setPressure(fPr + dP);
                              if (fRh < 0.0)
                              {
                                logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                                  "::checkRinexFile(): the extracted relative humidity looks "
                                  "suspicious: " + QString("").setNum(fRh) + "%; adjusted it to zero");
                                m.setRelativeHumidity(0.0);
                              }
                              else if (100.0 < fRh)
                              {
                                logger->write(SgLogger::WRN, SgLogger::IO, className() + 
                                  "::checkRinexFile(): the extracted relative humidity looks "
                                  "suspicious: " + QString("").setNum(fRh) + "%; adjusted it to 100%");
                                m.setRelativeHumidity(1.0);
                              }
                              else
                                m.setRelativeHumidity(fRh*0.01);
                              //
                              SgMeteoReading *meteoRec=new SgMeteoReading(t, m);
                              logReadings_.meteoPars() << meteoRec;
                              count++;
                            };
                            idx++;
                          }
                          else
                            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                              "::checkRinexFile(): cannot convert relative humidity to double, "
                              "string=\"" + reData.cap(9) + "\"");
                        }
                        else
                          logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                            "::checkRinexFile(): cannot convert temperature to double, string=\"" +
                            reData.cap(8) + "\"");
                      }
                      else
                        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                          "::checkRinexFile(): cannot convert pressure to double, string=\"" +
                          reData.cap(7) + "\"");
                    }
                    else
                      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                        "::checkRinexFile(): cannot convert second to int, string=\"" +
                        reData.cap(6) + "\"");
                  }
                  else
                    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                      "::checkRinexFile(): cannot convert minute to int, string=\"" +
                      reData.cap(5) + "\"");
                }
                else
                  logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                    "::checkRinexFile(): cannot convert hour to int, string=\"" + reData.cap(4) + "\"");
              }
              else
                logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                  "::checkRinexFile(): cannot convert day to int, string=\"" + reData.cap(3) + "\"");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
                "::checkRinexFile(): cannot convert month to int, string=\"" + reData.cap(2) + "\"");
          }
          else
            logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
              "::checkRinexFile(): cannot convert year to int, string=\"" + reData.cap(1) + "\"");
        };
      };
      f.close();
      s.setDevice(NULL);
      logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
        "::checkRinexFile(): extracted " + QString("").setNum(count) + " meteo records from " +
        QString("").setNum(idx) + " strings of " + f.fileName() + " RINEX file");
    }
    else
    {
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
        "::checkRinexFile(): unable to read the file \"" + f.fileName() + "\"");
      return false;
    };
  };
  logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
    "::checkRinexFile(): collected " + QString("").setNum(logReadings_.meteoPars().size()) + 
    " meteo records");
  return logReadings_.meteoPars().size()>0;
};



//
#ifndef SEPARATED_LOG2ANT
bool SgStnLogCollector::propagateData(SgVlbiStationInfo* stn, 
  bool createAntabFile, bool overwriteAntabFile, bool reportAllReadings)
{
  // output of the raw data:
  logReadings_.setStationName(stn->name());
  
  // ANTAB:
  if (createAntabFile)
    logReadings_.createAntabFile(inputLogFileName_, "", stn->getKey(), channelSkeded_, 
      useSkededChannelSetup_, overwriteAntabFile, reportAllReadings);

  // verify collected data:
  logReadings_.verifyReadings(stn, defaultCableSignByStn_);
  // put data into station data structures:
  if (!hasCableRecord_ && hasCdmsRecord_)
  {
    stn->addAttr(SgVlbiStationInfo::Attr_CABLE_CAL_IS_CDMS);
    logger->write(SgLogger::INF, SgLogger::IO_TXT, className() +
      "::propagateData(): the cable calibration data for " + stn->getKey() + " are CDMS");
  }
  else if (hasCableRecord_ && hasCdmsRecord_)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::propagateData(): both /cable/ and /CDMS/ commands were found for " + stn->getKey() +
      " in the log file");
  };
  //
  return logReadings_.propagateData(stn);
};
#endif



//
bool SgStnLogCollector::readDbbc3DumpFile(const QString& fileName)
{
  QString                       str("");
  QFile                         f;
  QTextStream                   s;
  FILE                         *pipe;
  qint64                        fileSize;
  int                           numOfReadStrs=0;
  bool                          reachedEndOfSession=false;
  QRegularExpressionMatch       match;
  SgMJD                         t, lastReadTsys(tZero);


  QString                       sEpoch("(\\d{4})-(\\d{2})-(\\d{2})T(\\d{2}):(\\d{2}):([0-9\\.]+)\\s*UTC\\s*");
  QString                       sSep("\\s*,\\s*");
  QString                       sDbl("([0-9\\.+-]+)");
  

  //2020-07-20T17:00:00.379712 UTC: BBC002:                         3448,32,agc,66,63,15202,15130,15298,15222,SEFDU: 10174.0 Jy,SEFDL: 9848.0 Jy
  QRegularExpression            reBbc(sEpoch + ":\\s*BBC(\\d+):\\s*" + sDbl + sSep +
    "(\\d+)" + sSep + "(\\w+)" + sSep + "(\\d+)" + sSep + "(\\d+)" + sSep + "(\\d+)" + sSep + 
    "(\\d+)" + sSep + "(\\d+)" + sSep + "(\\d+)" + sSep + "SEFDU:\\s*" + sDbl + "\\s*Jy" + sSep + 
    "SEFDL:\\s*" + sDbl + "\\s*Jy.*");

  //2020.202.18:01:44.03/dbbc3/dbbc001/ 3480.400000,a,32,1,agc,79,84,15299,15290,14977,14972;
  //QRegularExpression            reDbbc3Tp("/dbbc3/dbbc(\\d{3})/\\s*([0-9\\.+-]+),([^,]+),(\\d+),(\\d+),"
  //                                        "(\\w+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+);.*");

  QString                       sensorId("");
  int                           nYr, nMn, nDy, nHr, nMi;
  double                        ifFrq, dSc;
  double                        bw;
  QString                       agc("");
  double                        bbcGainU, bbcGainL;
  double                        bbcTPUOn, bbcTPLOn, bbcTPUOff, bbcTPLOff;
  double                        bbcSEFDU, bbcSEFDL;
  
  bool                          isDataOn=false;
  bool                          isOnSource=false;
  SgOnSourceRecord             *osRec=NULL;
  int                           onSourceIdx=0;
  int                           numOfOnSource;

  fileSize = QFileInfo(fileName).size();

  pipe = compressors.openFlt(fileName, f, s, FLTD_Input);
  if (s.device())
  {
    // clear previous TPC readings:
    for (int i=0; i<logReadings_.onSourceRecords().size(); i++)
      logReadings_.onSourceRecords()[i]->dbbc3Tps().clear();
    for (int i=0; i<logReadings_.dbbc3Tps().size(); i++)
      delete logReadings_.dbbc3Tps()[i];
    logReadings_.dbbc3Tps().clear();
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::readDbbc3DumpFile(): station " + stationName_ + 
      ": previously accumulated dbbc3 TP records were reset");

    if ((numOfOnSource=logReadings_.onSourceRecords().size()))
      osRec = logReadings_.onSourceRecords().first();

    bool                        have2reportNumOfStrings=10000000 < fileSize;
    while (!s.atEnd() && !reachedEndOfSession)
    {
      str = s.readLine();
      numOfReadStrs++;

if (have2reportNumOfStrings && numOfReadStrs%200000 == 0)
std::cout << "  -- read " << numOfReadStrs/1000 << "K strings; file size: " << fileSize << "\n";

      agc = "";
//    bbcIdx = 0;
      nYr = nMn = nDy = nHr = nMi = 0;
      dSc = ifFrq = bw = bbcGainU = bbcGainL = bbcTPUOn = 
        bbcTPLOn = bbcTPUOff = bbcTPLOff = bbcSEFDU = bbcSEFDL = 0.0;
    
      if ((match=reBbc.match(str)).hasMatch())
      {
        agc = match.captured(10);
        // they pass regexp, it is ok do not check for conversion success:
        nYr = match.captured( 1).toInt();
        nMn = match.captured( 2).toInt();
        nDy = match.captured( 3).toInt();
        nHr = match.captured( 4).toInt();
        nMi = match.captured( 5).toInt();
        dSc = match.captured( 6).toDouble();
        t.setUpEpoch(nYr, nMn, nDy, nHr, nMi, dSc);
        sensorId = match.captured(7).simplified().rightJustified(5, ' ');
//      bbcIdx    = match.captured( 7).toInt();
        ifFrq     = match.captured( 8).toDouble();
        bw        = match.captured( 9).toDouble();
        bbcGainU  = match.captured(11).toDouble();
        bbcGainL  = match.captured(12).toDouble();
        bbcTPUOn  = match.captured(13).toDouble();
        bbcTPLOn  = match.captured(14).toDouble();
        bbcTPUOff = match.captured(15).toDouble();
        bbcTPLOff = match.captured(16).toDouble();
        bbcSEFDU  = match.captured(17).toDouble();
        bbcSEFDL  = match.captured(18).toDouble();
        //
        // find onSource record:
        while (onSourceIdx<numOfOnSource && (osRec=logReadings_.onSourceRecords()[onSourceIdx]) &&
          osRec->tFinis()<t)
          onSourceIdx++;
        if (osRec && osRec->tStart()<t)
          isOnSource = true;
        else
          isOnSource = false;
        //
        if (osRec && osRec->tDataOn()<t)
          isDataOn = true;
        else
          isDataOn = false;
        //
        SgDbbc3TpReading       *tp=NULL;
        if (0.1/DAY2SEC < fabs(lastReadTsys - t))
        {
          tp = new SgDbbc3TpReading;
          tp->setT(t);
          tp->setOsRec(isOnSource?osRec:NULL);
          tp->setIsOnSource(isOnSource);
          tp->setIsDataOn(isDataOn);
          logReadings_.dbbc3Tps() << tp;
          if (osRec)
            osRec->dbbc3Tps() << tp;
        }
        else
          tp = logReadings_.dbbc3Tps().last();
        //
        tp->addRecord(sensorId, "", bw, agc, bbcGainU, bbcGainL, bbcTPUOff, bbcTPLOff, 
          bbcTPUOn, bbcTPLOn, bbcSEFDU, bbcSEFDL);
        lastReadTsys = t;
      };
    };
  
  };
  compressors.closeFlt(pipe, f, s);


  logger->write(SgLogger::INF, SgLogger::IO, className() +
    "::readDbbc3DumpFile(): station " + stationName_ + ": " + QString("").setNum(numOfReadStrs) + 
    " strings of the DBBC3 dump file \"" + fileName + "\" were read");
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readDbbc3DumpFile(): station " + stationName_ + ": collected " + 
    QString("").setNum(logReadings_.dbbc3Tps().size()) + " dbbc3 TP records");

  return true;
};

/*=====================================================================================================*/







/*=======================================================================================================
*
*                          SgVlbaLogCollector's METHODS:
* 
*======================================================================================================*/
//
// static first:
//
const QString SgVlbaLogCollector::className()
{
  return "SgVlbaLogCollector";
};



// An empty constructor:
SgVlbaLogCollector::SgVlbaLogCollector() :
  logFileName_(""),
  readingsByKey_(),
  ivs2vlba_()
{
  for (int i=0; i<NUM_OF_VLBA_ENTRIES; i++)
    ivs2vlba_.insert(ivsStationNames_[i], vlbaStationCodes_[i]);
};



//
// A destructor:
SgVlbaLogCollector::~SgVlbaLogCollector()
{
  for (QMap<QString, SgStnLogReadings*>::iterator it=readingsByKey_.begin(); 
    it!=readingsByKey_.end(); ++it)
    delete it.value();
  readingsByKey_.clear();
};



//
bool SgVlbaLogCollector::readLogFile(const QString& fileName, int nYear)
{
  QString                       str("");
  QString                       key("");
  QFile                         f(fileName);
  QString                       stnPattern("([\\w]{2})");
  QString                       srcPattern("([\\d\\w+-]{2,8})");
  QString                       epcPattern("(\\d{3})(?:-|\\s+)(\\d{2}):(\\d{2}):(\\d{2})");
  QString                       dblPattern("([+-\\.\\d]+)");
  
  QRegExp                       rePulseCalInfo("PulseCal information for\\s+" + stnPattern + "\\s+",
                                                Qt::CaseInsensitive);
  QRegExp                       reScanCalInfo("Scan information for\\s+" + stnPattern + "\\s+",
                                                Qt::CaseInsensitive);
  QRegExp                       reWeatherInfo("Weather information for\\s+" + stnPattern + "\\s+",
                                                Qt::CaseInsensitive);
  QRegExp                       rePulseCalData
    (epcPattern + "\\s+'CC'\\s+" + dblPattern + "\\s+" + dblPattern,
    Qt::CaseInsensitive);
  QRegExp                       reScanCalData
    (srcPattern + "\\s+" + epcPattern + "\\s+" + epcPattern + "\\s+" + 
      dblPattern + "\\s+" + dblPattern + "\\s+" + dblPattern,
      Qt::CaseInsensitive);
  QRegExp                       rePulseScanData
    ("!\\s+" + stnPattern + "\\s+([\\w\\d+-]+)\\s+" + srcPattern + "/(\\d+)\\s+" + 
      epcPattern + "/" + epcPattern, 
      Qt::CaseInsensitive);
  QRegExp                       reWeatherData
    (epcPattern + "\\s+" + dblPattern + "\\s+" + dblPattern + "\\s+" + dblPattern + "\\s+" + 
      dblPattern + "\\s+" + dblPattern + "\\s+" + dblPattern + "\\s+" + dblPattern, 
      Qt::CaseInsensitive);


  /*
   *  The files are usually at http://www.vlba.nrao.edu/astro/VOBS/IVS
   * 
   * 
   * 
   */


  logFileName_ = fileName;
  SgStnLogReadings             *reading=NULL;
  SgOnSourceRecord             *osRec=NULL;
  SgMJD                         t, t_aux;
  bool                          have2read, isOk;
  double                        d;
  int                           m;
  //
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      "::readLogFile(): the log file " + fileName + " does not exist");
    return false;
  };
  //
  //
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readLogFile(): reading log file " + fileName);
  //
  have2read = true;
  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    while (!s.atEnd())
    {
      if (have2read)
        str = s.readLine();
      else
        have2read = true;
      //
      // pulse cal info:
      if (rePulseCalInfo.indexIn(str) != -1)
      {
        key = rePulseCalInfo.cap(1);
        //
        if (readingsByKey_.contains(key))
          reading = readingsByKey_[key];
        else
          readingsByKey_.insert(key, (reading=new SgStnLogReadings(key)));
        //
//      while (!s.atEnd() && !(str.contains(" information ") && !str.contains(key)))
        str = "";
        while (!s.atEnd() && !str.contains(" information "))
        {
          str = s.readLine();
          //
          //
          if (rePulseCalData.indexIn(str) != -1)
          {
            if (strs2mjd(nYear, rePulseCalData.cap(1), rePulseCalData.cap(2), 
                          rePulseCalData.cap(3), rePulseCalData.cap(4), t))
            {
              d = rePulseCalData.cap(5).toDouble(&isOk);
              if (isOk)
                reading->cableCals() << new SgCableCalReading(t, d*1.0e-12);
              else
                logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                  "::readLogFile(): failed to extract cable cal value from PulseCalData: \"" + 
                  str + "\"");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                "::readLogFile(): failed to extract epoch from PulseCalData: \"" + str + "\"");
          };
          if (rePulseScanData.indexIn(str) != -1)
          { 
            m = rePulseScanData.cap(4).toInt(&isOk);
            if (isOk)
            {
              if (m != 999) // are others ok?
              {
                if (strs2mjd(nYear, rePulseScanData.cap(5), rePulseScanData.cap(6),
                          rePulseScanData.cap(7), rePulseScanData.cap(8), t))
                {
                  if (strs2mjd(nYear, rePulseScanData.cap(9), rePulseScanData.cap(10),
                            rePulseScanData.cap(11), rePulseScanData.cap(12), t_aux))
                  {
                    osRec = new SgOnSourceRecord;
                    osRec->setSourceName(rePulseScanData.cap(3).toUpper());
                    osRec->setTstart(t);
                    osRec->setTfinis(t_aux);
                    // check for duplicates:
                    if (reading->onSourceRecords().size()==0)
                      reading->onSourceRecords() << osRec;
//                  else if (*reading->onSourceRecords().last() != *osRec)
                    else if (!reading->onSourceRecords().last()->isSameArgs(*osRec))
                      reading->onSourceRecords() << osRec;
                    else
                    {
                      logger->write(SgLogger::INF, SgLogger::IO, className() + 
                        "::readLogFile(): got a duplicate string of PulseScanData: \"" + str + "\"");
                      delete osRec;
                    };
                  }
                  else
                    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                      "::readLogFile(): failed to extract epoch_2 from PulseScanData: \"" + str + "\"");
                }
                else
                  logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                    "::readLogFile(): failed to extract epoch_1 from PulseScanData: \"" + str + "\"");
              };
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                "::readLogFile(): failed to extract qualifier value from PulseCalData: \"" + str + "\"");
          };
        };
        have2read = false;
      };
      //
      // scan info:
      /*
      if (reScanCalInfo.indexIn(str) != -1)
      {
        key = reScanCalInfo.cap(1);
        if (readingsByKey_.contains(key))
          reading = readingsByKey_[key];
        else
        {
          reading = new SgStnLogReadings;
          reading->setStationName(key);
          readingsByKey_.insert(key, reading);
        };

        while (!s.atEnd() && !(str.contains(" information ") && !str.contains(key)))
        {
          str = s.readLine();
          if (reScanCalData.indexIn(str) != -1)
          {
            ..*..
          };
        };
        have2read = false;
      };
      */
      //
      //
      // weather info:
      if (reWeatherInfo.indexIn(str) != -1)
      {
        key = reWeatherInfo.cap(1);
        //
        if (readingsByKey_.contains(key))
          reading = readingsByKey_[key];
        else
          readingsByKey_.insert(key, (reading=new SgStnLogReadings(key)));
        //
//      while (!s.atEnd() && !(str.contains(" information ") && !str.contains(key)))
        str = "";
        while (!s.atEnd() && !str.contains(" information "))
        {
          str = s.readLine();
          if (reWeatherData.indexIn(str) != -1)
          {
            if (strs2mjd(nYear, reWeatherData.cap(1), reWeatherData.cap(2),
                  reWeatherData.cap(3), reWeatherData.cap(4), t))
            {
              double            temp, press, dewpt;
              temp = reWeatherData.cap(5).toDouble(&isOk);
              if (isOk)
              {
                press = reWeatherData.cap(6).toDouble(&isOk);
                if (isOk)
                {
                  dewpt = reWeatherData.cap(7).toDouble(&isOk);
                  if (isOk)
                    reading->meteoPars() << new 
                      SgMeteoReading(t, SgMeteoData(temp, press, SgMeteoData::dewPt2Rho(temp, dewpt)));
                  else
                    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                      "::readLogFile(): failed to extract dew point from WeatherData: \"" + str + "\"");
                }
                else
                  logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                    "::readLogFile(): failed to extract pressure from WeatherData: \"" + str + "\"");
              }
              else
                logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                  "::readLogFile(): failed to extract temperature from WeatherData: \"" + str + "\"");
            }
            else
              logger->write(SgLogger::ERR, SgLogger::IO, className() + 
                "::readLogFile(): failed to extract epoch from WeatherData: \"" + str + "\"");
          };
        };
        have2read = false;
      };
    };
    f.close();
    s.setDevice(NULL);
  };

  for (QMap<QString, SgStnLogReadings*>::iterator it=readingsByKey_.begin();
    it!=readingsByKey_.end(); ++it)
  {
    SgStnLogReadings           *rd=it.value();
    rd->setCableSign(-1);
    rd->setCableCalAmbig(1.0e-9);
    rd->allocateCableReadings();
  };
  //
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readLogFile(): collected logs of " + QString("").setNum(readingsByKey_.size())+ 
    " stations from the log file");
  logger->write(SgLogger::DBG, SgLogger::IO, className() +
    "::readLogFile(): parsing of the log file has been completed");
  //
  return true;
};



//
bool SgVlbaLogCollector::strs2mjd(int nYear, const QString& sDay, const QString& sHr, 
  const QString& sMin, const QString& sSec, SgMJD& t)
{
  int                           dy, hr, mi, se;
  bool                          isOk;
  isOk = true;

  dy = sDay.toInt(&isOk);
  if (isOk)
  {
    hr = sHr.toInt(&isOk);
    if (isOk)
    {
      mi = sMin.toInt(&isOk);
      if (isOk)
      {
        se = sSec.toInt(&isOk);
        if (isOk)
          t.setUpEpoch(nYear, 0, dy, hr, mi, se);
        else
          logger->write(SgLogger::ERR, SgLogger::IO, className() + 
            "::strs2mjd(): failed to extract seconds from \"" + sSec +  + "\"");
      }
      else
        logger->write(SgLogger::ERR, SgLogger::IO, className() + 
          "::strs2mjd(): failed to extract minutes from \"" + sMin +  + "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO, className() + 
        "::strs2mjd(): failed to extract hours from \"" + sHr +  + "\"");
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::strs2mjd(): failed to extract days from \"" + sDay + "\"");
  return isOk;
};



//
#ifndef SEPARATED_LOG2ANT
bool SgVlbaLogCollector::propagateData(SgVlbiStationInfo* stn)
{
  if (isVlba(stn->getKey()))
  {
    QString                     vlbaKey=ivs2vlba_.value(stn->getKey());
    if (readingsByKey_.contains(vlbaKey))
    {
      SgStnLogReadings         *logReadings=readingsByKey_.value(vlbaKey);
      
      // verify collected data:
      logReadings->verifyReadings(stn, NULL);

      // put data into station data structures:
      return logReadings->propagateData(stn);
    }
    else // VLBA station not found in the log file
    {
      logger->write(SgLogger::WRN, SgLogger::IO, className() + 
        "::propagateData(): cannot find VLBA station " + stn->getKey() + " (aka " + vlbaKey + 
        ") in the log file");
      return false;
    };
  }
  else // not a VLBA station
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::propagateData(): cannot find a station " + stn->getKey() + " in the map of VLBA stations");
    return false;
  };
};
#endif
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
//




/*=====================================================================================================*/
//
// constants:
//
//const QString        SgStnLogReadings::sFiller_("n/a");
const QString          SgStnLogReadings::sFiller_("DUMMY");
const int              SgStnLogReadings::nFiller_ = -999;
const double           SgStnLogReadings::dFiller_ = -99.9;


int SgStnLogCollector::fsContentStartsAt_[] = {0, 10, 14, 20, 0, 0};


const QString SgVlbaLogCollector::vlbaStationCodes_[NUM_OF_VLBA_ENTRIES] =
  { "BR", "CA", "EB", "FD", "GA", "GT", "HN", "JV", "KP", "LA", "MK", "NL", "OV", "PT", "SC" };
const QString SgVlbaLogCollector::ivsStationNames_[NUM_OF_VLBA_ENTRIES] =
  { "BR-VLBA ", "CAMBRIDG", "EFLSBERG", "FD-VLBA ", "VLBA85_3", "GBT-VLBA", "HN-VLBA ", 
    "JODRELL2", "KP-VLBA ", "LA-VLBA ", "MK-VLBA ", "NL-VLBA ", "OV-VLBA ", "PIETOWN ", 
    "SC-VLBA " };






/*=====================================================================================================*/




QString asciifyString(const QString& str)
{
  QString                       s("");
//QRegularExpression            rx("[\\x{00}-\\x{20}]");
  ushort                        n;
  for (int i=0; i<str.size(); i++)
  {
//  if (rx.match(str.at(i)).hasMatch())
    n = str.at(i).unicode();
    if (n<32 || 126<n)
      s += QString("").sprintf("\\0x%04X", n);
    else
      s += str.at(i);
  };
  return s;
};
















/*=====================================================================================================*/





