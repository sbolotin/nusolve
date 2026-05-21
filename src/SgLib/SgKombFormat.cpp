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

#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <SgConstants.h>
#include <SgKombFormat.h>
#include <SgLogger.h>
#include <SgVlbiSourceInfo.h>







/***===================================================================================================*/
/**
 *   SgKombHeaderRecord:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombHeaderRecord::className()
{
  return "SgKombHeaderRecord";
}; 



//
void SgKombHeaderRecord::debugReport()
{
  std::cout << "   HDentry #" << idx_ 
            << ": " << prefix_;
  if (bandId_[0] != ' ' || bandId_[1] != ' ' )
    std::cout << " @ " << bandId_ << "-band\n";
  else
    std::cout << "\n";
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombHeaderRecord& record)
{
  s >> (qint16 &)record.idx_
    >> (qint8 &)record.prefix_[0] >> (qint8 &)record.prefix_[1] 
    >> (qint8 &)record.prefix_[2] >> (qint8 &)record.prefix_[3]
    >> (qint8 &)record.bandId_[0] >> (qint8 &)record.bandId_[1];
  record.prefix_[4] = record.bandId_[2] = 0;
  return s;
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 *   SgKombHeader:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombHeader::className()
{
  return "SgKombHeader";
}; 



//
void SgKombHeader::debugReport()
{
  std::cout << "HD #" << idx_ 
            << ": [" << prefix_ 
            << "] Exp: " << experimentCode_ 
            << " Bl: " << baselineId_ 
            << ", KOMB=[" << kombFileName_ << "]\n";
  std::cout << "   #" << obsIdx_ 
            << "  " << totalNumberOfRecs_ 
            << "/" << totalNumberOfHeaderRecs_ << "\n";
  for (int i=0; i<entries_.size(); i++)
    entries_.at(i)->debugReport();
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombHeader& header)
{
  char                           c1, c2, c3;
  s >> (qint8 &)header.prefix_[0] >> (qint8 &)header.prefix_[1];
  s >> (qint8 &)header.prefixIdx_[0] >> (qint8 &)header.prefixIdx_[1];
  s >> (qint8 &)c1 >> (qint8 &)c2 >> (qint8 &)c3; // == KSP
  //std::cout << " Got: " << c1 << c2 << c3 << "\n";
  s >> (qint8 &)c1; // unused
    
  s >> (qint8 &)header.experimentCode_[0] >> (qint8 &)header.experimentCode_[1] 
    >> (qint8 &)header.experimentCode_[2] >> (qint8 &)header.experimentCode_[3]
    >> (qint8 &)header.experimentCode_[4] >> (qint8 &)header.experimentCode_[5]
    >> (qint8 &)header.experimentCode_[6] >> (qint8 &)header.experimentCode_[7]
    >> (qint8 &)header.experimentCode_[8] >> (qint8 &)header.experimentCode_[9];
     
  s >> (qint16 &)header.obsIdx_;
    
  s >> (qint8 &)header.baselineId_[0] >> (qint8 &)header.baselineId_[1];
  s >> (qint16 &)header.totalNumberOfRecs_;
  s >> (qint16 &)header.totalNumberOfHeaderRecs_;
  s >> (qint8 &)header.kombFileName_[0] >> (qint8 &)header.kombFileName_[1]
    >> (qint8 &)header.kombFileName_[2] >> (qint8 &)header.kombFileName_[3]
    >> (qint8 &)header.kombFileName_[4] >> (qint8 &)header.kombFileName_[5];
  
  header.prefix_[2] = header.prefixIdx_[2] = header.experimentCode_[10] = header.baselineId_[2] =
    header.kombFileName_[6] = 0;
  
  bool                          isOk;
  header.idx_ = QString(header.prefixIdx_).toInt(&isOk);
  if (!isOk)
    header.idx_ = -1;
  
//  for (int i=0; i<24; i++)
//    s >> (qint8 &)c1;
  s.skipRawData(24);

  SgKombHeaderRecord           *rec;
  for (int i=0; i<25; i++)
  {
    rec = new SgKombHeaderRecord;
    s >> *rec;
    if (rec->prefix_[0] != 0)
      header.entries_ << rec;
    else
      delete rec;
  };
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombOb01Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombOb01Record::className()
{
  return "SgKombOb01Record";
}; 



//
void SgKombOb01Record::debugReport()
{
  std::cout << " OB01: prefix= " << prefix_ << " Experiment: " << experimentCode_ 
            << " obsIdx_=" << obsIdx_ << " baseline: [" << baselineId_ << "]\n";
  std::cout << "       TStart: " << qPrintable(tStart_.toString())
            << " TStop : " << qPrintable(tFinis_.toString())
            << " TRefer: " << qPrintable(tRefer_.toString()) << "\n";
  std::cout << "       CorrFName=[" << correlatorFileName_ << "] "
            << " KombFName=[" << kombFileName_ << "] "
            << " TCorrPrcs: " << qPrintable(tProcByCorr_.toString()) << "\n";
  std::cout << "       PPperiod: " << periodPp_ << " " << " numOfPP: " << numOfPp_
            << " samplingPeriod: " << samplingPeriod_ << " videoBandWidth: " << videoBandWidth_
            << " CorrMode= [" << correlatorMode_ << "]\n";
  std::cout << "       Baseline=[" << station1Name_ << ":" << station2Name_ << "]"
            << " @ " << sourceName_ << "\n";
  std::cout << "       aPrioriObses: " << aPrioriObses_[0] 
            << ", " << aPrioriObses_[1]
            << ", " << aPrioriObses_[2]
            << ", " << aPrioriObses_[3] << "\n";
  std::cout << "       aPrioriClockError: " << aPrioriClockError_
            << " aPrioriClockOffset_: " << aPrioriClockOffset_
            << " clockRateDiff_: " << clockRateDiff_
            << " instrumentalDelay: " << instrumentalDelay_ << "\n";
  std::cout << "       SrcCoords: RA " << qPrintable(SgVlbiSourceInfo::ra2String(srcRa_/RAD2DEG))
            << "  DE " << qPrintable(SgVlbiSourceInfo::dn2String(srcDe_/RAD2DEG)) << "\n";
  std::cout << "       Stn1Coords: R=" << 
    qPrintable(QString("").sprintf("(%12.3f, %12.3f, %12.3f)", 
    r1_.at(X_AXIS), r1_.at(Y_AXIS), r1_.at(Z_AXIS))) << "\n";
  std::cout << "       Stn2Coords: R=" << 
    qPrintable(QString("").sprintf("(%12.3f, %12.3f, %12.3f)", 
    r2_.at(X_AXIS), r2_.at(Y_AXIS), r2_.at(Z_AXIS))) << "\n";
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombOb01Record& rec)
{
  s >> (qint8 &)rec.prefix_[0] >> (qint8 &)rec.prefix_[1]
    >> (qint8 &)rec.prefix_[2] >> (qint8 &)rec.prefix_[3];
  s.skipRawData(4);
  s >> (qint8 &)rec.experimentCode_[0] >> (qint8 &)rec.experimentCode_[1] 
    >> (qint8 &)rec.experimentCode_[2] >> (qint8 &)rec.experimentCode_[3]
    >> (qint8 &)rec.experimentCode_[4] >> (qint8 &)rec.experimentCode_[5]
    >> (qint8 &)rec.experimentCode_[6] >> (qint8 &)rec.experimentCode_[7]
    >> (qint8 &)rec.experimentCode_[8] >> (qint8 &)rec.experimentCode_[9];
  s >> (qint16 &)rec.obsIdx_;
  s >> (qint8 &)rec.baselineId_[0] >> (qint8 &)rec.baselineId_[1];
  s >> (qint16 &)rec.epochObsStart_[0] >> (qint16 &)rec.epochObsStart_[1] 
    >> (qint16 &)rec.epochObsStart_[2] >> (qint16 &)rec.epochObsStart_[3]
    >> (qint16 &)rec.epochObsStart_[4];
  s >> (qint16 &)rec.epochObsStop_[0] >> (qint16 &)rec.epochObsStop_[1] 
    >> (qint16 &)rec.epochObsStop_[2] >> (qint16 &)rec.epochObsStop_[3]
    >> (qint16 &)rec.epochObsStop_[4];
  s >> (qint16 &)rec.epochObsRef_[0] >> (qint16 &)rec.epochObsRef_[1] 
    >> (qint16 &)rec.epochObsRef_[2] >> (qint16 &)rec.epochObsRef_[3]
    >> (qint16 &)rec.epochObsRef_[4];
  s >> (qint8 &)rec.correlatorFileName_[0] >> (qint8 &)rec.correlatorFileName_[1]
    >> (qint8 &)rec.correlatorFileName_[2] >> (qint8 &)rec.correlatorFileName_[3]
    >> (qint8 &)rec.correlatorFileName_[4] >> (qint8 &)rec.correlatorFileName_[5];
  s.skipRawData(2);
  s >> (qint8 &)rec.kombFileName_[0] >> (qint8 &)rec.kombFileName_[1]
    >> (qint8 &)rec.kombFileName_[2] >> (qint8 &)rec.kombFileName_[3]
    >> (qint8 &)rec.kombFileName_[4] >> (qint8 &)rec.kombFileName_[5];
  s.skipRawData(2);
  s >> (qint16 &)rec.epochProcCorr_[0] >> (qint16 &)rec.epochProcCorr_[1] 
    >> (qint16 &)rec.epochProcCorr_[2] >> (qint16 &)rec.epochProcCorr_[3];
  s.skipRawData(4);
  s >> (qint16 &)rec.periodPp_ >> (qint16 &)rec.numOfPp_;
  s >> (float&)rec.samplingPeriod_ >> (float&)rec.videoBandWidth_;
  
  s >> (qint8 &)rec.correlatorMode_[0] >> (qint8 &)rec.correlatorMode_[1];
  s >> (qint8 &)rec.sourceName_[0] >> (qint8 &)rec.sourceName_[1]
    >> (qint8 &)rec.sourceName_[2] >> (qint8 &)rec.sourceName_[3]
    >> (qint8 &)rec.sourceName_[4] >> (qint8 &)rec.sourceName_[5]
    >> (qint8 &)rec.sourceName_[6] >> (qint8 &)rec.sourceName_[7];
  s >> (float&)rec.srcDe_;
  s.skipRawData(4); // Greenwhich hour angle of the source at PRT
//  s.skipRawData(8); // declination of radio source J2000 + Greenwhich hour angle of the source at PRT
  s >> (qint8 &)rec.station1Name_[0] >> (qint8 &)rec.station1Name_[1]
    >> (qint8 &)rec.station1Name_[2] >> (qint8 &)rec.station1Name_[3]
    >> (qint8 &)rec.station1Name_[4] >> (qint8 &)rec.station1Name_[5]
    >> (qint8 &)rec.station1Name_[6] >> (qint8 &)rec.station1Name_[7]
    >> (qint8 &)rec.station2Name_[0] >> (qint8 &)rec.station2Name_[1]
    >> (qint8 &)rec.station2Name_[2] >> (qint8 &)rec.station2Name_[3]
    >> (qint8 &)rec.station2Name_[4] >> (qint8 &)rec.station2Name_[5]
    >> (qint8 &)rec.station2Name_[6] >> (qint8 &)rec.station2Name_[7];
  s >> (double&)rec.r1_(X_AXIS) >> (double&)rec.r1_(Y_AXIS) >> (double&)rec.r1_(Z_AXIS);
  s >> (double&)rec.r2_(X_AXIS) >> (double&)rec.r2_(Y_AXIS) >> (double&)rec.r2_(Z_AXIS);
  s >> (double&)rec.aPrioriObses_[0] >> (double&)rec.aPrioriObses_[1] 
    >> (double&)rec.aPrioriObses_[2] >> (double&)rec.aPrioriObses_[3]
    >> (double&)rec.aPrioriClockError_ >> (double&)rec.clockRateDiff_
    >> (double&)rec.instrumentalDelay_
    >> (double&)rec.aPrioriClockOffset_;
  s >> (float&)rec.srcRa_;

  rec.prefix_[4] = rec.experimentCode_[10] = rec.baselineId_[2] = rec.correlatorFileName_[6] = 
    rec.kombFileName_[6] = rec.correlatorMode_[2] = rec.sourceName_[8] = rec.station1Name_[8] = 
    rec.station2Name_[8] = 0;
  s.skipRawData(14);

  
  rec.tStart_.setUpEpoch(rec.epochObsStart_[0], 0, rec.epochObsStart_[1], 
    rec.epochObsStart_[2], rec.epochObsStart_[3], rec.epochObsStart_[4]);
  rec.tFinis_.setUpEpoch(rec.epochObsStop_[0], 0, rec.epochObsStop_[1], 
    rec.epochObsStop_[2], rec.epochObsStop_[3], rec.epochObsStop_[4]);
  rec.tRefer_.setUpEpoch(rec.epochObsRef_[0], 0, rec.epochObsRef_[1], 
    rec.epochObsRef_[2], rec.epochObsRef_[3], rec.epochObsRef_[4]);
  rec.tProcByCorr_.setUpEpoch(rec.epochProcCorr_[0], 0, rec.epochProcCorr_[1], 
    rec.epochProcCorr_[2], rec.epochProcCorr_[3], 0);

  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombOb02Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombOb02Record::className()
{
  return "SgKombOb02Record";
}; 



//
void SgKombOb02Record::debugReport()
{
  std::cout << " OB02: prefix= " << prefix_ 
            << " numOfChannels= " << numOfChannels_
            << ", index table:\n";

  for (int i=0; i<16; i++)
  {
    for (int j=0; j<2; j++)
      std::cout << idxNumTable_[i][j] << "  ";
    std::cout << "\n";
  };

};



//
SgKombStream &operator>>(SgKombStream& s, SgKombOb02Record& rec)
{
  s >> (qint8 &)rec.prefix_[0] >> (qint8 &)rec.prefix_[1]
    >> (qint8 &)rec.prefix_[2] >> (qint8 &)rec.prefix_[3];
  rec.prefix_[4] = 0;
  s.skipRawData(4 + 8 + 8 + 32);
  s >> (qint16 &)rec.numOfChannels_;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (qint16 &)rec.idxNumTable_[i][j];
  s.skipRawData(134);
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombOb03Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombOb03Record::className()
{
  return "SgKombOb03Record";
}; 



//
void SgKombOb03Record::debugReport()
{
  std::cout << " OB03: prefix= " << prefix_ << "\n";
  for (int i=0; i<16; i++)
  {
    std::cout << "      #" << i 
              << ": rfFreqs=" << rfFreqs_[i]
              << ", phCalFreqs=" << phCalFreqs_[i] << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombOb03Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  rec.prefix_[4] = 0;
  s.skipRawData(4);
  for (int i=0; i<16; i++)
    s >> (double&)rec.rfFreqs_[i];
  for (int i=0; i<16; i++)
    s >> (float&)rec.phCalFreqs_[i];
  s.skipRawData(56);
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombBd01Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombBd01Record::className()
{
  return "SgKombBd01Record";
}; 



//
void SgKombBd01Record::debugReport()
{
  std::cout << " BD01: prefix= " << prefix_ 
            << " bwsMode= [" << bwsMode_ << "]"
            << " " << bandId_ << "-band"
            << " KOMB Procsd Number= " << kombProcsdNumber_
            << "\n";
  std::cout << " total Num Of Proc Channels: " << totalNumOfProcChannels_
            << " tapeQualityCode= [" << tapeQualityCode_ << "]"
            << " procRefFreq= " << procRefFreq_ << "Hz"
            << "\n";
  std::cout 
            << " TCorrStart : " << qPrintable(tCorrStart_.toString())   << "\n"
            << " TCorrFinis : " << qPrintable(tCorrFinis_.toString())   << "\n"
            << " TProcByKomb: " << qPrintable(tProcByKomb_.toString())  << "\n";

  for (int i=0; i<16; i++)
  {
    std::cout << "      #" << i 
              << ": rfFreqs=" << rfFreqs_[i]
              << "  idxNum: " << idxNumTable_[i][0] << ":" << idxNumTable_[i][1] 
              << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombBd01Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  s >> (qint8&)rec.bwsMode_[0] >> (qint8&)rec.bwsMode_[1]
    >> (qint8&)rec.bwsMode_[2] >> (qint8&)rec.bwsMode_[3];
  s >> (qint8&)rec.bandId_[0] >> (qint8&)rec.bandId_[1];
  s >> (qint16&)rec.epochProcKomb_[0] >> (qint16&)rec.epochProcKomb_[1] 
    >> (qint16&)rec.epochProcKomb_[2] >> (qint16&)rec.epochProcKomb_[3];
  s >> (qint16&)rec.kombProcsdNumber_;
  s >> (qint16&)rec.epochCorrStart_[0] >> (qint16&)rec.epochCorrStart_[1] 
    >> (qint16&)rec.epochCorrStart_[2] >> (qint16&)rec.epochCorrStart_[3]
    >> (qint16&)rec.epochCorrStart_[4] >> (qint16&)rec.epochCorrStart_[5];
  s >> (qint16&)rec.epochCorrFinis_[0] >> (qint16&)rec.epochCorrFinis_[1] 
    >> (qint16&)rec.epochCorrFinis_[2] >> (qint16&)rec.epochCorrFinis_[3]
    >> (qint16&)rec.epochCorrFinis_[4] >> (qint16&)rec.epochCorrFinis_[5];
  s >> (qint16&)rec.totalNumOfProcChannels_;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (qint16 &)rec.idxNumTable_[i][j];
  s >> (qint8&)rec.tapeQualityCode_[0] >> (qint8&)rec.tapeQualityCode_[1]
    >> (qint8&)rec.tapeQualityCode_[2] >> (qint8&)rec.tapeQualityCode_[3]
    >> (qint8&)rec.tapeQualityCode_[4] >> (qint8&)rec.tapeQualityCode_[5];
  s >> (double&)rec.procRefFreq_;
  for (int i=0; i<16; i++)
    s >> (double&)rec.rfFreqs_[i];
  s.skipRawData(4);

  rec.tProcByKomb_.setUpEpoch(rec.epochProcKomb_[0], 0, rec.epochProcKomb_[1], 
    rec.epochProcKomb_[2], rec.epochProcKomb_[3], 0.0);

  rec.tCorrStart_.setUpEpoch(rec.epochCorrStart_[0], 0, rec.epochCorrStart_[1], 
    rec.epochCorrStart_[2], rec.epochCorrStart_[3], 
    rec.epochCorrStart_[4] + rec.epochCorrStart_[5]/1000.0);

  rec.tCorrFinis_.setUpEpoch(rec.epochCorrFinis_[0], 0, rec.epochCorrFinis_[1], 
    rec.epochCorrFinis_[2], rec.epochCorrFinis_[3], 
    rec.epochCorrFinis_[4] + rec.epochCorrFinis_[5]/1000.0);

  rec.prefix_[4] = rec.bwsMode_[4] = rec.bandId_[2] = rec.tapeQualityCode_[6] = 0;
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombBd02Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombBd02Record::className()
{
  return "SgKombBd02Record";
}; 



//
void SgKombBd02Record::debugReport()
{
  std::cout << " BD02: prefix= " << prefix_ 
            << " bwsMode= [" << bwsMode_ << "]"
            << " " << bandId_ << "-band"
            << "\n";
  std::cout << " KOMB QCode: [" << kombQualityCode_ << "]"
            << " rmsPpNumByChan= " << rmsPpNumByChan_
            << " effectiveIntegrationPeriod= " << effectiveIntegrationPeriod_
            << " rejectionRate= " << rejectionRate_
            << "\n";
  for (int i=0; i<20; i++)
  {
    std::cout << "      #" << i 
              << "  errorStatusMaskCode: [" << errorStatusMaskCode(i) << "]"
              << "\n";
  };
  //char                          errorStatusMaskCode_[20][5];
  std::cout << " obsDelayAtCentrlEpoch= " << obsDelayAtCentrlEpoch_
            << " obsDelayRateAtCentrlEpoch= " << obsDelayRateAtCentrlEpoch_
            << " obsTotalPhaseAtCentrlEpoch= " << obsTotalPhaseAtCentrlEpoch_
            << "\n";
  std::cout << " obsTotalPhaseAtReferEpoch= " << obsTotalPhaseAtReferEpoch_
            << " obsTotalPhaseAtEarthCenterEpoch= " << obsTotalPhaseAtEarthCenterEpoch_
            << " obsResidPhaseAtEarthCenterEpoch= " << obsResidPhaseAtEarthCenterEpoch_
            << "\n";
  std::cout << " searchWndCoarseDelay= (" 
            << searchWndCoarseDelay_[0] << ", " << searchWndCoarseDelay_[1] << ")"
            << " searchWndFineDelay= (" 
            << searchWndFineDelay_[0] << ", " << searchWndFineDelay_[1] << ")"
            << " searchWndFineDelayRate= (" 
            << searchWndFineDelayRate_[0] << ", " << searchWndFineDelayRate_[1] << ")"
            << "\n";
  std::cout << " deltaEpochs= " << deltaEpochs_
            << "\n";

  std::cout << " TObsCentral: " << qPrintable(tObsCentral_.toString())  << "\n";
  for (int i=0; i<16; i++)
  {
    std::cout << "      #" << i 
              << "  procPpNumTable: " << procPpNumTable_[i][0] << ":" << procPpNumTable_[i][1] 
              << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombBd02Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  s >> (qint8&)rec.bwsMode_[0] >> (qint8&)rec.bwsMode_[1]
    >> (qint8&)rec.bwsMode_[2] >> (qint8&)rec.bwsMode_[3];
  s >> (qint8&)rec.bandId_[0] >> (qint8&)rec.bandId_[1];
  s >> (qint8&)rec.kombQualityCode_[0] >> (qint8&)rec.kombQualityCode_[1];
  for (int i=0; i<20; i++)
  {
    s >> (qint8&)rec.errorStatusMaskCode_[i][0] >> (qint8&)rec.errorStatusMaskCode_[i][1]
      >> (qint8&)rec.errorStatusMaskCode_[i][2] >> (qint8&)rec.errorStatusMaskCode_[i][3];
    rec.errorStatusMaskCode_[i][4] = 0;
  };
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (qint16 &)rec.procPpNumTable_[i][j];
  s >> (float&)rec.rmsPpNumByChan_ >> (float&)rec.effectiveIntegrationPeriod_ 
    >> (float&)rec.rejectionRate_;
  s >> (qint16&)rec.epochObsCentral_[0] >> (qint16&)rec.epochObsCentral_[1] 
    >> (qint16&)rec.epochObsCentral_[2] >> (qint16&)rec.epochObsCentral_[3]
    >> (qint16&)rec.epochObsCentral_[4] >> (qint16&)rec.epochObsCentral_[5];
  s >> (double&)rec.obsDelayAtCentrlEpoch_ >> (double&)rec.obsDelayRateAtCentrlEpoch_
    >> (float&)rec.obsTotalPhaseAtCentrlEpoch_;
  s >> (float&)rec.searchWndCoarseDelay_[0] >> (float&)rec.searchWndCoarseDelay_[1]
    >> (float&)rec.searchWndFineDelay_[0] >> (float&)rec.searchWndFineDelay_[1]
    >> (float&)rec.searchWndFineDelayRate_[0] >> (float&)rec.searchWndFineDelayRate_[1];

  s >> (double&)rec.deltaEpochs_ >> (float&)rec.obsTotalPhaseAtReferEpoch_
    >> (float&)rec.obsTotalPhaseAtEarthCenterEpoch_ >> (float&)rec.obsResidPhaseAtEarthCenterEpoch_;
  s.skipRawData(12);

  rec.tObsCentral_.setUpEpoch(rec.epochObsCentral_[0], 0, rec.epochObsCentral_[1], 
    rec.epochObsCentral_[2], rec.epochObsCentral_[3], 
    rec.epochObsCentral_[4] + rec.epochObsCentral_[5]/1000.0);
  rec.prefix_[4] = rec.bwsMode_[4] = rec.bandId_[2] = rec.kombQualityCode_[2] = 0;
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombBd03Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombBd03Record::className()
{
  return "SgKombBd03Record";
}; 



//
void SgKombBd03Record::debugReport()
{
  std::cout << " BD03: prefix= " << prefix_ 
            << " bwsMode= [" << bwsMode_ << "]"
            << " " << bandId_ << "-band"
            << "\n";
  std::cout 
//            << " phaseCalRate@1= " << phaseCalRate1_
//            << " phaseCalRate@2= " << phaseCalRate2_
              << qPrintable(QString("").sprintf(" phaseCalRate@1=%.28g phaseCalRate@2= %.28g",
                    phaseCalRate1_, phaseCalRate2_))

            << "\n";
  for (int i=0; i<16; i++)
  {
    std::cout << "   #" << i 
              << " phaseCal: Amp&Phase @1: " 
              << phaseCalAmpPhase1_[i][0] << " : " << phaseCalAmpPhase1_[i][1] 
              << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombBd03Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  s >> (qint8&)rec.bwsMode_[0] >> (qint8&)rec.bwsMode_[1]
    >> (qint8&)rec.bwsMode_[2] >> (qint8&)rec.bwsMode_[3];
  s >> (qint8&)rec.bandId_[0] >> (qint8&)rec.bandId_[1];
  s >> (double&)rec.phaseCalRate1_ >> (double&)rec.phaseCalRate2_;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (float&)rec.phaseCalAmpPhase1_[i][j];
  s.skipRawData(102);
  rec.prefix_[4] = rec.bwsMode_[4] = rec.bandId_[2] = 0;
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombBd04Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombBd04Record::className()
{
  return "SgKombBd04Record";
}; 



//
void SgKombBd04Record::debugReport()
{
  std::cout << " BD04: prefix= " << prefix_ 
            << " bwsMode= [" << bwsMode_ << "]"
            << " " << bandId_ << "-band"
            << "\n";
  for (int i=0; i<16; i++)
  {
    std::cout << "   #" << i 
              << " phaseCal: Amp&Phase @2: " 
              << phaseCalAmpPhase2_[i][0] << " : " << phaseCalAmpPhase2_[i][1] 
              << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombBd04Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  s >> (qint8&)rec.bwsMode_[0] >> (qint8&)rec.bwsMode_[1]
    >> (qint8&)rec.bwsMode_[2] >> (qint8&)rec.bwsMode_[3];
  s >> (qint8&)rec.bandId_[0] >> (qint8&)rec.bandId_[1];
  s.skipRawData(16);
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (float&)rec.phaseCalAmpPhase2_[i][j];
  s.skipRawData(102);
  rec.prefix_[4] = rec.bwsMode_[4] = rec.bandId_[2] = 0;
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombBd05Record:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombBd05Record::className()
{
  return "SgKombBd05Record";
}; 



//
void SgKombBd05Record::debugReport()
{
  std::cout << " BD05: prefix= " << prefix_ 
            << " bwsMode= [" << bwsMode_ << "]"
            << " " << bandId_ << "-band"
            << "\n";
  std::cout << " fineSearchFringeAmplitude= " << fineSearchFringeAmplitude_
            << " coarseSearchFringeAmplitude= " << coarseSearchFringeAmplitude_
            << " snr= " << snr_
            << " incohFringeAmplitude= " << incohFringeAmplitude_
            << " falseDetectionProbability= " << falseDetectionProbability_
            << "\n";
  std::cout << " obsDelayAtReferEpoch= " << obsDelayAtReferEpoch_
            << " obsDelayResid= " << obsDelayResid_
            << " obsDelaySigma= " << obsDelaySigma_
            << " obsDelayAmbiguity= " << obsDelayAmbiguity_
            << "\n";
  std::cout << " obsDelayRateResid= " << obsDelayRateResid_
            << " obsDelaySigma= " << obsDelaySigma_
            << " obsDelayRateSigma= " << obsDelayRateSigma_
            << "\n";
  std::cout << " obsCoarseDelayAtReferEpoch= " << obsCoarseDelayAtReferEpoch_
            << " obsCoarseDelayResid= " << obsCoarseDelayResid_
            << " obsCoarseDelaySigma= " << obsCoarseDelaySigma_
            << " obsCoarseDelayRateResid= " << obsCoarseDelayRateResid_
            << "\n";
  std::cout << " phaseDelayAtReferEpoch= " << phaseDelayAtReferEpoch_
            << " phaseDelayAtReferEpochPlus_1sec= " << phaseDelayAtReferEpochPlus_1sec_
            << " phaseDelayAtReferEpochMinus_1sec= " << phaseDelayAtReferEpochMinus_1sec_
            << "\n";
  for (int i=0; i<16; i++)
  {
    std::cout << "   #" << i 
              << " fringe Amp&Phase: " 
              << fringeAmpPhase_[i][0] << " : " << fringeAmpPhase_[i][1] 
              << "\n";
  };
};



//
SgKombStream &operator>>(SgKombStream& s, SgKombBd05Record& rec)
{
  s >> (qint8&)rec.prefix_[0] >> (qint8&)rec.prefix_[1]
    >> (qint8&)rec.prefix_[2] >> (qint8&)rec.prefix_[3];
  s >> (qint8&)rec.bwsMode_[0] >> (qint8&)rec.bwsMode_[1]
    >> (qint8&)rec.bwsMode_[2] >> (qint8&)rec.bwsMode_[3];
  s >> (qint8&)rec.bandId_[0] >> (qint8&)rec.bandId_[1];
  s >> (float&)rec.fineSearchFringeAmplitude_ >> (float&)rec.coarseSearchFringeAmplitude_
    >> (float&)rec.snr_ >> (float&)rec.incohFringeAmplitude_ >> (float&)rec.falseDetectionProbability_;
  s >> (double&)rec.obsDelayAtReferEpoch_ >> (double&)rec.obsDelayResid_ >> (float&)rec.obsDelaySigma_
    >> (float&)rec.obsDelayAmbiguity_;
  s >> (double&)rec.obsDelayRateAtReferEpoch_ >> (double&)rec.obsDelayRateResid_
    >> (float&)rec.obsDelayRateSigma_ >> (double&)rec.obsCoarseDelayAtReferEpoch_
    >> (double&)rec.obsCoarseDelayResid_ >> (float&)rec.obsCoarseDelaySigma_ 
    >> (double&)rec.obsCoarseDelayRateResid_;
  s >> (double&)rec.phaseDelayAtReferEpoch_ >> (double&)rec.phaseDelayAtReferEpochPlus_1sec_
    >> (double&)rec.phaseDelayAtReferEpochMinus_1sec_;
  for (int i=0; i<16; i++)
    for (int j=0; j<2; j++)
      s >> (float&)rec.fringeAmpPhase_[i][j];
  s.skipRawData(2);
  rec.prefix_[4] = rec.bwsMode_[4] = rec.bandId_[2] = 0;
  return s;
};
/*=====================================================================================================*/










/***===================================================================================================*/
/**
 *   SgKombReader:
 * 
 */
/**====================================================================================================*/
// static first:
const QString SgKombReader::className()
{
  return "SgKombReader";
};



//
void SgKombReader::debugReport()
{
};



//
void SgKombReader::clearData()
{
  for (int i=0; i<headers_.size(); i++)
    delete headers_.at(i);
  headers_.clear();
  headRecs_.clear();
  for (QMap<QString, SgKombBd01Record*>::iterator it=bd01ByBand_.begin(); it!=bd01ByBand_.end(); ++it)
    delete it.value();
  for (QMap<QString, SgKombBd02Record*>::iterator it=bd02ByBand_.begin(); it!=bd02ByBand_.end(); ++it)
    delete it.value();
  for (QMap<QString, SgKombBd03Record*>::iterator it=bd03ByBand_.begin(); it!=bd03ByBand_.end(); ++it)
    delete it.value();
  for (QMap<QString, SgKombBd04Record*>::iterator it=bd04ByBand_.begin(); it!=bd04ByBand_.end(); ++it)
    delete it.value();
  for (QMap<QString, SgKombBd05Record*>::iterator it=bd05ByBand_.begin(); it!=bd05ByBand_.end(); ++it)
    delete it.value();
  bd01ByBand_.clear();
  bd02ByBand_.clear();
  bd03ByBand_.clear();
  bd04ByBand_.clear();
  bd05ByBand_.clear();
  mapOfBands_.clear();
  isOk_ = false;
};



//
void SgKombReader::readFile(const QString& fileName)
{
  SgKombStream                  s;
  QFile                         f;
  f.setFileName((fileName_=fileName));
  f.open(QIODevice::ReadOnly);
  s.setDevice(&f);
  s.setByteOrder(QDataStream::BigEndian);
#if QT_VERSION >= 0x040500
  s.setVersion (QDataStream::Qt_4_5); // starting from Qt_4_6 Qt spoils floats and doubles :(
#endif
//  s.setVersion (QDataStream::Qt_4_5); // starting from Qt_4_6 Qt spoils floats and doubles :(
  // s.setByteOrder(QDataStream::LittleEndian);

  clearData();
  // read the header:
  int                           numOfHdRecs;
  SgKombHeader*                 hd;
  QString                       bandKey("");
  hd = new SgKombHeader;
  s >> *hd;
  numOfHdRecs = hd->totalNumberOfHeaderRecs();
  if (isVerbose_)
    hd->debugReport();
  headers_ << hd;
  for (int j=0; j<hd->entries().size(); j++)
    headRecs_ << hd->entries().at(j);
  for (int i=1; i<numOfHdRecs; i++)
  {
    hd = new SgKombHeader;
    s >> *hd;
    headers_ << hd;
    if (isVerbose_)
      hd->debugReport();
    for (int j=0; j<hd->entries().size(); j++)
      headRecs_ << hd->entries().at(j);
  };

  for (int i=numOfHdRecs; i<headRecs_.size() && !s.atEnd(); i++)
  {
    SgKombHeaderRecord         *entry=headRecs_.at(i);
    if (strcmp(entry->prefix(), "OB01")==0)
    {
      s >> ob01_;
      if (isVerbose_)
        ob01_.debugReport();
    }
    else if (strcmp(entry->prefix(), "OB02")==0)
    {
      s >> ob02_;
      if (isVerbose_)
        ob02_.debugReport();
    }
    else if (strcmp(entry->prefix(), "OB03")==0)
    {
      s >> ob03_;
      if (isVerbose_)
        ob03_.debugReport();
    }
    else if (strcmp(entry->prefix(), "BD01")==0)
    {
      SgKombBd01Record         *bd01=new SgKombBd01Record;
      s >> *bd01;
      bandKey = bd01->bandId();
      if (bd01ByBand_.contains(bandKey))
      {
        delete bd01ByBand_[bandKey];
        bd01ByBand_.remove(bandKey);
        logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() + 
          "::readFile(): the BD01 record has been replaced with the new one");
      };
      bd01ByBand_.insert(bandKey, bd01);
      if (!mapOfBands_.contains(bandKey))
        mapOfBands_.insert(bandKey, bandKey);
      if (isVerbose_)
        bd01->debugReport();
    }
    else if (strcmp(entry->prefix(), "BD02")==0)
    {
      SgKombBd02Record         *bd02=new SgKombBd02Record;
      s >> *bd02;
      bandKey = bd02->bandId();
      if (bd02ByBand_.contains(bandKey))
      {
        delete bd02ByBand_[bandKey];
        bd02ByBand_.remove(bandKey);
        logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() + 
          "::readFile(): the BD02 record has been replaced with the new one");
      };
      bd02ByBand_.insert(bandKey, bd02);
      if (!mapOfBands_.contains(bandKey))
        mapOfBands_.insert(bandKey, bandKey);
      if (isVerbose_)
        bd02->debugReport();
    }
    else if (strcmp(entry->prefix(), "BD03")==0)
    {
      SgKombBd03Record         *bd03=new SgKombBd03Record;
      s >> *bd03;
      bandKey = bd03->bandId();
      if (bd03ByBand_.contains(bandKey))
      {
        delete bd03ByBand_[bandKey];
        bd03ByBand_.remove(bandKey);
        logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() + 
          "::readFile(): the BD03 record has been replaced with the new one");
      };
      bd03ByBand_.insert(bandKey, bd03);
      if (!mapOfBands_.contains(bandKey))
        mapOfBands_.insert(bandKey, bandKey);
      if (isVerbose_)
        bd03->debugReport();
    }
    else if (strcmp(entry->prefix(), "BD04")==0)
    {
      SgKombBd04Record         *bd04=new SgKombBd04Record;
      s >> *bd04;
      bandKey = bd04->bandId();
      if (bd04ByBand_.contains(bandKey))
      {
        delete bd04ByBand_[bandKey];
        bd04ByBand_.remove(bandKey);
        logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() + 
          "::readFile(): the BD04 record has been replaced with the new one");
      };
      bd04ByBand_.insert(bandKey, bd04);
      if (!mapOfBands_.contains(bandKey))
        mapOfBands_.insert(bandKey, bandKey);
      if (isVerbose_)
        bd04->debugReport();
    }
    else if (strcmp(entry->prefix(), "BD05")==0)
    {
      SgKombBd05Record         *bd05=new SgKombBd05Record;
      s >> *bd05;
      bandKey = bd05->bandId();
      if (bd05ByBand_.contains(bandKey))
      {
        delete bd05ByBand_[bandKey];
        bd05ByBand_.remove(bandKey);
        logger->write(SgLogger::DBG, SgLogger::IO_BIN, className() + 
          "::readFile(): the BD05 record has been replaced with the new one");
      };
      bd05ByBand_.insert(bandKey, bd05);
      if (!mapOfBands_.contains(bandKey))
        mapOfBands_.insert(bandKey, bandKey);
      if (isVerbose_)
        bd05->debugReport();
    }
    else
    {
      s.skipRawData(256);
      //      std::cout << "   ...skipped\n";
    };
  };
  //
  // check the records:
  if (strlen(ob01_.sourceName())==0 || *ob01_.sourceName()=='?')
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the OB01 record has wrong source name");
    return;
  };
  if (strlen(ob01_.station1Name())==0 || *ob01_.station1Name()=='?')
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the OB01 record has wrong station1 name");
    return;
  };
  if (strlen(ob01_.station2Name())==0 || *ob01_.station2Name()=='?')
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the OB01 record has wrong station1 name");
    return;
  };
  if (ob02_.numOfChannels()==0)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the OB02 record has zero number of channels");
    return;
  };
  // numbers of bands:
  if (mapOfBands_.size() != bd01ByBand_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the number of registered bands, " + QString("").setNum(mapOfBands_.size()) +
      ", is not equal to the number of bands of BD01 records, " + 
      QString("").setNum(bd01ByBand_.size()));
    return;
  };
  if (mapOfBands_.size() != bd02ByBand_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the number of registered bands, " + QString("").setNum(mapOfBands_.size()) +
      ", is not equal to the number of bands of BD02 records, " + 
      QString("").setNum(bd02ByBand_.size()));
    return;
  };
  if (mapOfBands_.size() != bd03ByBand_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the number of registered bands, " + QString("").setNum(mapOfBands_.size()) +
      ", is not equal to the number of bands of BD03 records, " + 
      QString("").setNum(bd03ByBand_.size()));
    return;
  };
  if (mapOfBands_.size() != bd04ByBand_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the number of registered bands, " + QString("").setNum(mapOfBands_.size()) +
      ", is not equal to the number of bands of BD04 records, " + 
      QString("").setNum(bd04ByBand_.size()));
    return;
  };
  if (mapOfBands_.size() != bd05ByBand_.size())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
      "::readFile(): the number of registered bands, " + QString("").setNum(mapOfBands_.size()) +
      ", is not equal to the number of bands of BD05 records, " + 
      QString("").setNum(bd05ByBand_.size()));
    return;
  };
  for (QMap<QString, QString>::iterator it=mapOfBands_.begin(); it!=mapOfBands_.end(); ++it)
  {
    if (!bd01ByBand_.contains(it.key()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
        "::readFile(): the " + it.key() + "-band is missed in the set of bands of BD01 records");
      return;
    };
    if (!bd02ByBand_.contains(it.key()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
        "::readFile(): the " + it.key() + "-band is missed in the set of bands of BD02 records");
      return;
    };
    if (!bd03ByBand_.contains(it.key()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
        "::readFile(): the " + it.key() + "-band is missed in the set of bands of BD03 records");
      return;
    };
    if (!bd04ByBand_.contains(it.key()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
        "::readFile(): the " + it.key() + "-band is missed in the set of bands of BD04 records");
      return;
    };
    if (!bd05ByBand_.contains(it.key()))
    {
      logger->write(SgLogger::WRN, SgLogger::IO_BIN, className() + 
        "::readFile(): the " + it.key() + "-band is missed in the set of bands of BD05 records");
      return;
    };
  };
  isOk_ = true;
};
/*=====================================================================================================*/










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
