/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2010-2024 Sergei Bolotin.
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


#include <math.h>
#include <iostream>


#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>


#include <SgExternalTrp.h>
#include <SgLogger.h>
#include <SgRegularExpressions.h>













/*=======================================================================================================
*
*                           SgExternalTrp::TropRecord's METHODS:
* 
*======================================================================================================*/
void SgExternalTrp::TrpRecord::parseString_V01_3(const QString& str)
{
  QString                       strSpc("\\s+");
  QString                       strInt("(\\d+)");
  QRegularExpressionMatch       match;

  QRegularExpression            reObs("O" + strSpc + strInt + strSpc + strReSourceName + strSpc + 
    "(\\d{4}\\.\\d{2}\\.\\d{2}-\\d{2}:\\d{2}:[0-9\\.]{2,6})" + 
    strSpc + strReStationName + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble);

//O      1    OJ287        2024.01.02-17:00:10.0  KOKEE     280.62659 31.45939   892.8  12.9    1.3338146D-08   1.9138747D+00   5.7563179D-01  -3.0679867D+00   1.4315261D-12   2.0705669D-04   2.0867853D-04  -7.7620542D-04
  
  isOk_ = false;
  //
  if ((match=reObs.match(str)).hasMatch())
  {
    // epoch:
    t_.fromString(SgMJD::F_SOLVE_SPLFL_V2, match.captured(3));
    // strings:
    srcName_      = match.captured( 2).leftJustified(8, ' ');
    stnName_      = match.captured( 4).leftJustified(8, ' ');
    // ints/doubles:
    scanIdx_      = match.captured( 1).toInt();
    dAzimuth_     = match.captured( 5).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dElevation_   = match.captured( 6).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dPres_        = match.captured( 7).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dTemp_        = match.captured( 8).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dSlant0Delay_ = match.captured( 9).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dSlant1Delay_ = 0.0;
    dMap0Delay_   = match.captured(10).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap1Delay_   = dMap0Delay_;
    dGradNDelay_  = match.captured(11).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradEDelay_  = match.captured(12).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dSlant0Rate_  = match.captured(13).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dSlant1Rate_  = 0.0;
    dMap0Rate_    = match.captured(14).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap1Rate_    = dMap0Rate_;
    dGradNRate_   = match.captured(15).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradERate_   = match.captured(16).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    isOk_ = true;
    hasMate_ = false;
  };
};



void SgExternalTrp::TrpRecord::parseString_V01_4(const QString& str)
{
  QString                       strSpc("\\s+");
  QString                       strInt("(\\d+)");
  QRegularExpressionMatch       match;
  double                        zD_dry, zD_wet;
  double                        zR_dry, zR_wet;

  zD_dry = zD_wet = zR_dry = zR_wet = 0.0;
  
  QRegularExpression            reObs("O" + strSpc + strInt + strSpc + strReSourceName + strSpc + 
    "(\\d{4}\\.\\d{2}\\.\\d{2}-\\d{2}:\\d{2}:[0-9\\.]{2,6})" + 
    strSpc + strReStationName + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + 
    strReDouble + strSpc + strReDouble + strSpc + strReDouble + strSpc + strReDouble);

//#  Scan#    Source       Time_tag               Station    Azimuth Elev      Pres     Temp    Zenith_Dry      Zenith_Wet      Map_Dry         Map_Wet         Grad_N          Grad_E          Zen_Dry_rate    Zen_Wet_Rate    Map_Dry_Rate    Map_Wet_Rate    Grad_N_Rate     Grad_E_Rate
//O      1    OJ287        2024.01.02-17:00:10.0  KOKEE     280.62659 31.45939   892.8  12.9   6.79563558D-09  1.87915450D-10  1.90982867D+00  1.91387474D+00  5.75631789D-01 -3.06798668D+00  0.00000000D+00 -1.73941396D-15  2.05418070D-04  2.07056687D-04  2.08678531D-04 -7.76205418D-04
//O      1    OJ287        2024.01.02-17:00:10.0  SEJONG    149.71923 71.37195   997.1   1.4   7.57885709D-09  2.41014444D-10  1.05513917D+00  1.05520786D+00 -3.07143020D-01  1.79341611D-01  0.00000000D+00  9.21136575D-16 -1.04807339D-05 -1.04945601D-05 -1.66410632D-06 -7.87260555D-05
  
  isOk_ = false;
  //
  if ((match=reObs.match(str)).hasMatch())
  {
    // epoch:
    t_.fromString(SgMJD::F_SOLVE_SPLFL_V2, match.captured(3));
    // strings:
    srcName_      = match.captured( 2).leftJustified(8, ' ');
    stnName_      = match.captured( 4).leftJustified(8, ' ');
    // ints/doubles:
    scanIdx_      = match.captured( 1).toInt();
    dAzimuth_     = match.captured( 5).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dElevation_   = match.captured( 6).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dPres_        = match.captured( 7).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dTemp_        = match.captured( 8).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    
    zD_dry        = match.captured( 9).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    zD_wet        = match.captured(10).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap0Delay_   = match.captured(11).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap1Delay_   = match.captured(12).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradNDelay_  = match.captured(13).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradEDelay_  = match.captured(14).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    
    zR_dry        = match.captured(15).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    zR_wet        = match.captured(16).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap0Rate_    = match.captured(17).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dMap1Rate_    = match.captured(18).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradNRate_   = match.captured(19).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();
    dGradERate_   = match.captured(20).replace(QChar('d'), QChar('e'), Qt::CaseInsensitive).toDouble();

    dSlant0Delay_ = zD_dry*dMap0Delay_;
    dSlant1Delay_ = zD_wet*dMap1Delay_;
    dSlant0Rate_  = zR_dry*dMap0Rate_;
    dSlant1Rate_  = zR_wet*dMap1Rate_;

    isOk_ = true;
    hasMate_ = false;
  };
};
/*=====================================================================================================*/







/*=======================================================================================================
*
*                           SgExternalTrp's METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgExternalTrp::className()
{
  return "SgExternalTrp";
};



//
//
bool SgExternalTrp::readFile(const QString& fileName)
{
  QRegularExpressionMatch       match;
  QRegularExpression            reHdr("TROPO_PATH_DELAY  Exchange format\\s+v\\s+(\\S+)\\s+(\\S+)");

  QFile                         f((inputFileName_=fileName));
  QString                       str;
  //
  isOk_ = false;
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": readFile(): the file \"" + fileName + "\" with external Trp data does not exist");
    return isOk_;
  };

  //
  // clear the stuff:
  for (int i=0; i<trpRecords_.size(); i++)
    delete trpRecords_.at(i);
  trpRecords_.clear();
  //
  recsByStnId_.clear();
  
  //
  //
  TrpRecord                     rec;
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);

    str = s.readLine();
    if ((match=reHdr.match(str)).hasMatch())
    {
      fmtVersion_ = match.captured(1);
      fmtDate_    = match.captured(2);
    };
    if (fmtVersion_ == "1.3")
      ifvFormatVersion_ = IFV_V_13;
    else if (fmtVersion_ == "1.4")
      ifvFormatVersion_ = IFV_V_14;
   
    //
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.at(0) != QChar('#'))
      {
        switch(ifvFormatVersion_)
        {
        case IFV_V_13:
          rec.parseString_V01_3(str);
          break;
        default:
        case IFV_V_14:
          rec.parseString_V01_4(str);
          break;
        };
        if (rec.isOk())
          trpRecords_.append(new TrpRecord(rec));
      };
    };
    f.close();
    s.setDevice(NULL);

  }
  else
    return isOk_;
  


  // split by stations:
  QList<TrpRecord*>            *trpLst=NULL;
  TrpRecord                    *r;
  for (int i=0; i<trpRecords_.size(); i++)
  {
    r = trpRecords_.at(i);
    if (recsByStnId_.contains(r->stnName()))
      trpLst = recsByStnId_[r->stnName()];
    else
    {
      trpLst = new QList<TrpRecord*>;
      recsByStnId_.insert(r->stnName(), trpLst);
    };
    trpLst->append(r);
  };
  
  //scanId = epoch.toString(SgMJD::F_INTERNAL) + "@" + sourceName;
  
  if ((isOk_=(trpRecords_.size()!=0&&recsByStnId_.size()!=0)))
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      "::readFile(): " + QString().setNum(trpRecords_.size()) + " records have been acquired from" + 
      " the external Trp file \"" + inputFileName_ + "\" of the version " + fmtVersion_ + " format");
    for(QMap <QString, QList<TrpRecord*>* >::iterator it=recsByStnId_.begin(); it!=recsByStnId_.end(); 
      ++it)
    {
      logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
        "::readFile(): station \"" + it.key() + "\" has " + QString("").setNum(it.value()->size()) +
        " trop records");
    };
  };
  if (isOk_)
    modelName_ = "VMF3";

  return isOk_;
};



/*=====================================================================================================*/






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








/*=====================================================================================================*/
