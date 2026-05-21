/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2012-2020 Sergei Bolotin.
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


#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <SgAPrioriData.h>
#include <SgConstants.h>
#include <SgLogger.h>


/*=======================================================================================================
*
*                          SgAPrioriRec METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgAPrioriRec::className()
{
  return "SgAPrioriRec";
};
/*=====================================================================================================*/





/*=======================================================================================================
*
*                          SgAPriories METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgAPriories::className()
{
  return "SgAPriories";
};



//
void SgAPriories::clearStorage()
{
  QList<SgAPrioriRec*> recs=values();
  for (int i=0; i<recs.size(); i++)
    delete recs.at(i);
  clear();
  fileName_ = "";
};



//
bool SgAPriories::parseString4StnPos(const QString& str, SgAPrioriRec& rec)
{
  // station name:
  int         idx=0;
  while (str.at(idx) == ' ')
    idx++;
  rec.setKey(str.mid(idx, 8));
  
  QStringList l = str.mid(idx+8).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 6)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString4StnGrd(): cannot parse the string [" + str.mid(idx+8).simplified() + 
      "], not enough data: l=" + QString("").setNum(l.size()));
    return false;
  };

  int                           nYr, nMn, nDy;
  SgVector                      r(3);
  bool                          isOk;
  //          1         2         3         4         5         6         7
  //01234567890123456789012345678901234567890123456789012345678901234567890123456789
  //    AIRA       -3530219.613     4118797.487    3344015.689   00 00 00
  //    AIRA       -3530219.605     4118797.488    3344015.688   08 06 14
  //    KOKEE      -5543837.656    -2054567.673     2387852.042   00 00 00
  
  // station positions:
  r(0) = l.at(0).toDouble(&isOk);
  if (!isOk)
    return false;
  r(1) = l.at(1).toDouble(&isOk);
  if (!isOk)
    return false;
  r(2) = l.at(2).toDouble(&isOk);
  if (!isOk)
    return false;
  
  rec.append(SgAPrioriRecComponent());
  rec[0].setDvalue("0", r(0));
  rec[0].setDvalue("1", r(1));
  rec[0].setDvalue("2", r(2));

  // valid epoch:
  nYr = l.at(3).toInt(&isOk);
  if (!isOk)
    return false;
  nMn = l.at(4).toInt(&isOk);
  if (!isOk)
    return false;
  nDy = l.at(5).toInt(&isOk);
  if (!isOk)
    return false;
  if (nYr==0 && nMn==0 && nDy==0)
    rec.setTsince(tZero);
  else
    rec.setTsince(SgMJD(nYr, nMn, nDy));
  //
  //std::cout << "Got a priori coords for station " << qPrintable(rec.getKey()) << ": "
  //          << "X: " << rec.getVi(0) << " Y: " << rec.getVi(1) << " Z: " << rec.getVi(2)
  //          << " for the epoch " << qPrintable(rec.getTsince().toString())
  //          << "\n";
  return true;
};



//
bool SgAPriories::parseString4StnVel(const QString& str, SgAPrioriRec& rec)
{
  // station name:
  int         idx=0;
  while (str.at(idx) == ' ')
    idx++;
  rec.setKey(str.mid(idx, 8));
  
  QStringList l = str.mid(idx+8).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 3)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString4StnGrd(): cannot parse the string [" + str.mid(idx+8).simplified() + 
      "], not enough data: l=" + QString("").setNum(l.size()));
    return false;
  };

  SgVector                  v(3);
  bool                        isOk;
  //          1         2         3         4         5         6
  //012345678901234567890123456789012345678901234567890123456789012345
  //    AIRA              -25.8            -7.7          -15.4       
  //    ALGOPARK          -16.2            -3.6            3.9       
  //    WETTZELL          -15.80           16.90           10.40
  
  // station velocities:
  v(0) = l.at(0).toDouble(&isOk);
  if (!isOk)
    return false;
  v(1) = l.at(1).toDouble(&isOk);
  if (!isOk)
    return false;
  v(2) = l.at(2).toDouble(&isOk);
  if (!isOk)
    return false;
  //  rec.setV(v*1.0e-3/365.25); // mm/yr => m/d
  // here we have tropical year (is it true?)
  v *= 1.0e-3/365.242198781250; // mm/yr => m/d
  rec.append(SgAPrioriRecComponent());
  rec[0].setDvalue("0", v(0));
  rec[0].setDvalue("1", v(1));
  rec[0].setDvalue("2", v(2));
  rec.setTsince(tZero);
  //
  //  std::cout << "Got a priori vels for station " << qPrintable(rec.getKey()) << ": "
  //            << "X: " << rec.getVi(0) << " Y: " << rec.getVi(1) << " Z: " << rec.getVi(2)
  //            << " for the epoch " << qPrintable(rec.getTsince().toString())
  //            << "\n";
  return true;
};



//
bool SgAPriories::parseString4SrcPos(const QString& str, SgAPrioriRec& rec)
{
  // source name:
  int                           idx=0;
  while (str.at(idx) == ' ')
    idx++;
  rec.setKey(str.mid(idx, 8));
  
  QStringList                   l=str.mid(idx+8).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 6)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString4StnGrd(): cannot parse the string [" + str.mid(idx+8).simplified() + 
      "], not enough data: l=" + QString("").setNum(l.size()));
    return false;
  };

  double                        dg, hr, mn, sc;
  SgVector                      k(2);
  bool                          isOk;
  //          1         2         3         4         5         6         7
  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
  //    2358+189  00 01 08.621568     +19 14 33.80173             gsf2011b Globl    
  //    2359-221  00 02 11.981470     -21 53 09.86663             gsf2011b Globl    
  //    IIIZW2    00 10 31.005902     +10 58 29.50438             ICRF2 Defining    
  
  // source coordinates:
  hr = l.at(0).toDouble(&isOk);
  if (!isOk)
    return false;
  mn = l.at(1).toDouble(&isOk);
  if (!isOk)
    return false;
  sc = l.at(2).toDouble(&isOk);
  if (!isOk)
    return false;
  k(0) = (hr + (mn + sc/60.0)/60.0)/RAD2HR;

  dg = l.at(3).toDouble(&isOk);
  if (!isOk)
    return false;
  mn = l.at(4).toDouble(&isOk);
  if (!isOk)
    return false;
  sc = l.at(5).toDouble(&isOk);
  if (!isOk)
    return false;
  if (l.at(3).at(0) != '-')
    k(1) = (dg + (mn + sc/60.0)/60.0)/RAD2DEG;
  else
    k(1) = (dg - (mn + sc/60.0)/60.0)/RAD2DEG;

  rec.append(SgAPrioriRecComponent());
  rec[0].setDvalue("0", k(0));
  rec[0].setDvalue("1", k(1));
  rec.setTsince(tZero);
  
  if (l.size() > 6)
  {
    QString                     str("");
    for (int i=6; i<l.size(); i++)
      str += l.at(i) + " ";
    str.chop(1);
    rec.setComments(str);
  };
  //
  //  std::cout << "Got a priori coords for source " << qPrintable(rec.getKey()) << ": "
  //            << "RA: " << rec.getVi(0)*RAD2HR << " DE: " << rec.getVi(1)*RAD2DEG 
  //            << " for the epoch " << qPrintable(rec.getTsince().toString())
  //            << "\n";
  return true;
};



//
bool SgAPriories::parseString4AxsOfs(const QString& str, SgAPrioriRec& rec)
{
  // station name:
  int                           idx=0;
  while (str.at(idx) == ' ')
    idx++;
  rec.setKey(str.mid(idx, 8));
  
  QStringList                   l=str.mid(idx+8).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 2)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString4StnGrd(): cannot parse the string [" + str.mid(idx+8).simplified() + 
      "], not enough data: l=" + QString("").setNum(l.size()));
    return false;
  };

  SgVector                      o(2);
  bool                          isOk;
  //          1         2         3         4         5         6
  //012345678901234567890123456789012345678901234567890123456789012345
  //AIRA       0.0000    -0.0089 -+ 0.0014 adjustment
  //ALASKANO   7.2852     7.2852           blokq.dat
  
  // station velocities:
  o(0) = l.at(0).toDouble(&isOk);
  if (!isOk)
    return false;
  o(1) = l.at(1).toDouble(&isOk);
  if (!isOk)
    return false;
  rec.append(SgAPrioriRecComponent());
  rec[0].setDvalue("0", o(0)); // a priori from antenna design
  rec[0].setDvalue("1", o(1)); // estimated
  
  rec.setTsince(tZero);
//    std::cout << "Got a priori axis offsets for station " << qPrintable(rec.getKey()) << ": "
//              << "AxOf_0: " << rec[0].getDvalue("0") << " AxOf: " << rec[0].getDvalue("1")
//              << " for the epoch " << qPrintable(rec.getTsince().toString())
//              << "\n";
  return true;
};



//
bool SgAPriories::parseString4StnGrd(const QString& str, SgAPrioriRec& rec)
{
  // station name:
  int                           idx=0;
  while (str.at(idx) == ' ')
    idx++;
  rec.setKey(str.mid(idx, 8));
  
  QStringList                   l=str.mid(idx+8).simplified().split(' ', QString::SkipEmptyParts);
  if (l.size() < 4)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": parseString4StnGrd(): cannot parse the string [" + str.mid(idx+8).simplified() + 
      "], not enough data: l=" + QString("").setNum(l.size()));
    return false;
  };

  SgVector                      o(2);
  bool                          isOk;
  //          1         2         3         4         5         6
  //012345678901234567890123456789012345678901234567890123456789012345
  //*Site       North Gradient (mm)     East Gradient (mm)
  //*
  //*               mean         rms       mean          rms
  //*
  //AIRA        -.50E+00     .51E+00     .32E-01     .33E+00
  //MCD 7850    -.24E+00     .27E+00    -.18E-01     .21E+00
  
  // station gradients:
  o(0) = l.at(0).toDouble(&isOk)/1000.0; // mm => m
  if (!isOk)
    return false;
  o(1) = l.at(2).toDouble(&isOk)/1000.0; // mm => m
  if (!isOk)
    return false;
  rec.append(SgAPrioriRecComponent());
  rec[0].setDvalue("0", o(0));
  rec[0].setDvalue("1", o(1));
  rec.setTsince(tZero);
  //  std::cout << "Got a priori axis offsets for station " << qPrintable(rec.getKey()) << ": "
  //            << "GrdN: " << rec.getVi(0) << ",  GrdE: " << rec.getVi(1)
  //            << "\n";
  return true;
};



/*
bool SgAPriories::parseString4SrcSsm(const QString& str, QTextStream& s, SgAPrioriRec& rec)
{
  bool                          isOk=false;
  int                           idx=0, n;
  QString                       ss("");
  double                        d;
  QRegExp                       reSrc("^Src:([\\.A-Z0-9+-]+{2,8})\\.*", Qt::CaseInsensitive);
  QRegExp                       reMpx("\\.*M_PX:([\\.\\d+-]+)\\.*", Qt::CaseInsensitive);
  QRegExp                       reMpy("\\.*M_PY:([\\.\\d+-]+)\\.*", Qt::CaseInsensitive);
  rec.setKey("");
  
  
  if (reSrc.indexIn(str) != -1)
  {
    ss = reSrc.cap(1);
    std::cout << "    -- Got source [" << qPrintable(ss) << "]\n";
    rec.setKey(ss);
  }
  else
    return isOk;

  double                        px, py, k, b;
  px = py = k = b = 0.0;

  if (reMpx.indexIn(str) != -1)
  {
    px = reMpx.cap(1).toDouble(&isOk);
    if (!isOk)
      return isOk;
  };
  if (reMpy.indexIn(str) != -1)
  {
    py = reMpy.cap(1).toDouble(&isOk);
    if (!isOk)
      return isOk;
  };
  std::cout << "    --           p_x= " << px << ", p_y= " << py << "\n";


  //rec.append(SgAPrioriRecComponent());

  return true;
};
*/


//
bool SgAPriories::readFile(const QString& fileName, SgAPriories::DataType dataType)
{
  clearStorage();
  SgAPrioriRec       *apdRec;
  QString             str;
  QFile               f((fileName_=fileName));
  bool                isOk=false;
  if (!f.exists())
  {
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
      ": the file [" + fileName + "] with a priori data does not exist");
    return isOk;
  };
  //
  // readFile's call can override the default:
  if (dataType==DT_UNDEF)
    dataType = dataType_;
  if (dataType==DT_UNDEF)
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() + 
      ": the data type of the file [" + fileName + "] is not specified; skipping it");
    return isOk;
  };
  //
  //
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    if (dataType == DT_SRC_SSM)
      isOk = parseFileSrcSsm(s);
    else
      while (!s.atEnd())
      {
        str = s.readLine();
        //000101
        if (str.size()==6) // check for the "zero" epoch:
        {
          bool          isOk;
          int           yr, mn, dy;
          yr = str.mid( 0, 2).toInt(&isOk);
          if (isOk)
          {
            mn = str.mid( 2, 2).toInt(&isOk);
            if (isOk)
            {
              dy = str.mid( 4, 2).toInt(&isOk);
              if (isOk)
              {
                if (0<mn && mn<13  &&  0<dy && dy<32)
                {
                  t0_.setUpEpoch(yr, mn, dy, 0, 0, 0.0);
                  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() + 
                    "::readFile(): got an initial epoch: " + t0_.toString() + " for the a priori set");
                }
                else
                  logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                    "::readFile(): got a suspicious epoch: " + str + ", skipping");
              };
            };
          };
        }
        else
        if (str.size()>8 && str.at(0)!='$' && str.at(0)!='#' && str.at(0)!='*')
        {
          apdRec = new SgAPrioriRec;
          isOk = false;
          switch (dataType)
          {
            case DT_STN_POS:
              isOk = parseString4StnPos(str, *apdRec);
              break;
            case DT_STN_VEL:
              isOk = parseString4StnVel(str, *apdRec);
              break;
            case DT_SRC_POS:
              isOk = parseString4SrcPos(str, *apdRec);
              break;
            case DT_AXS_OFS:
              isOk = parseString4AxsOfs(str, *apdRec);
              break;
            case DT_STN_GRD:
              isOk = parseString4StnGrd(str, *apdRec);
              break;
            case DT_SRC_SSM:
            default:
              break;
          };
          if (isOk)
            insert(apdRec->getKey(), apdRec);
          else
            delete apdRec;
        };
      };
    f.close();
    s.setDevice(NULL);
  };
  return (0<size());
};



//
bool SgAPriories::parseFileSrcSsm(QTextStream& s)
{
  SgAPrioriRec                 *rec;
  bool                          isOk=false;
  bool                          estR, estK, estB;
  int                           nY, nM, nD;
  double                        px, py, k, b;
  SgMJD                         tFrom;
  QString                       str(""), ss(""), srcName(""), sModelType("");
  QRegExp                       reSrc(".*Src:([\\.A-Z0-9+-]{2,8}).*", Qt::CaseInsensitive);
  QRegExp                       reTsince(".*T:([\\d]{4})/([\\d]{2})/([\\d]{2}).*", Qt::CaseInsensitive);
  QRegExp                       reModelType(".*SSM_T:([\\w]+)\\s+.*", Qt::CaseInsensitive);
  QRegExp                       reMptX(".*X:([\\s\\.\\d+-]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptY(".*Y:([\\s\\.\\d+-]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptK(".*K:([\\s\\.\\d+-]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptB(".*B:([\\s\\.\\d+-]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptEr(".*ER:([\\w]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptEk(".*EK:([\\w]+).*", Qt::CaseInsensitive);
  QRegExp                       reMptEb(".*EB:([\\w]+).*", Qt::CaseInsensitive);
  enum modelType
  {
    MT_NONE             = 0,
    MT_MP               = 1, // multi point
  };
  modelType                     mType;


  rec = NULL;
  mType = MT_NONE;
  while (!s.atEnd())
  {
    //rec = NULL;
    isOk = false;
    estR = estK = estB = false;
    px = py = k = b = 0.0;
    nY = nM = nD = 0;
    tFrom = tZero;

    str = s.readLine();
    if (str.size()>8 && str.at(0)!='$' && str.at(0)!='#' && str.at(0)!='*' && str.at(0)!='/')
    {
      if (reSrc.indexIn(str) != -1)
      {
        rec = new SgAPrioriRec;
        srcName = reSrc.cap(1);
        rec->setKey(srcName.leftJustified(8, ' '));
        rec->setTsince(tZero);
        mType = MT_NONE;
        sModelType = "";
        insert(rec->getKey(), rec);
      };
      if (reModelType.indexIn(str) != -1)
      {
        sModelType = reModelType.cap(1);
        if (sModelType.toUpper() == "MP")
          mType = MT_MP;
        else
          logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
            "::parseFileSrcSsm(): MP: got an unknown type of model: \"" + sModelType + "\"");
      };
      // known models:
      if (mType == MT_MP)
      {
        if (reMptX.indexIn(str) != -1)
        {
          px = (ss = reMptX.cap(1)).toDouble(&isOk);
          if (!isOk)
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::parseFileSrcSsm(): MPT_X: cannot convert \"" + ss + "\" to double");
        };
        if (reMptY.indexIn(str) != -1)
        {
          py = (ss = reMptY.cap(1)).toDouble(&isOk);
          if (!isOk)
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::parseFileSrcSsm(): MPT_Y: cannot convert \"" + ss + "\" to double");
        };
        if (reMptK.indexIn(str) != -1)
        {
          k = (ss = reMptK.cap(1)).toDouble(&isOk);
          if (!isOk)
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::parseFileSrcSsm(): MPT_K: cannot convert \"" + ss + "\" to double");
        };
        if (reMptB.indexIn(str) != -1)
        {
          b = (ss = reMptB.cap(1)).toDouble(&isOk);
          if (!isOk)
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::parseFileSrcSsm(): MPT_B: cannot convert \"" + ss + "\" to double");
        };
        if (reTsince.indexIn(str) != -1)
        {
          nY = (ss = reTsince.cap(1)).toDouble(&isOk);
          if (isOk)
          {
            nM = (ss = reTsince.cap(2)).toDouble(&isOk);
            if (isOk)
            {
              nD = (ss = reTsince.cap(3)).toDouble(&isOk);
              if (isOk)
              {
                tFrom = SgMJD(nY, nM, nD);
                if (rec)
                  rec->setTsince(tFrom);
              }
              else
                logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                  "::parseFileSrcSsm(): MPT_T: cannot convert \"" + ss + "\" to days");
            }
            else
              logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
                "::parseFileSrcSsm(): MPT_T: cannot convert \"" + ss + "\" to month");
          }
          else
            logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
              "::parseFileSrcSsm(): MPT_T: cannot convert \"" + ss + "\" to years");
        };
        if (reMptEr.indexIn(str) != -1)
        {
          estR = (reMptEr.cap(1).toUpper().at(0) == 'Y');
        };
        if (reMptEk.indexIn(str) != -1)
        {
          estK = (reMptEk.cap(1).toUpper().at(0) == 'Y');
        };
        if (reMptEb.indexIn(str) != -1)
        {
          estB = (reMptEb.cap(1).toUpper().at(0) == 'Y');
        };
        if (rec )
        {
          SgAPrioriRecComponent     point;
          point.setDvalue("MP_X", px);
          point.setDvalue("MP_Y", py);
          point.setDvalue("MP_K", k);
          point.setDvalue("MP_B", b);
          point.setBvalue("MP_ER", estR);
          point.setBvalue("MP_EK", estK);
          point.setBvalue("MP_EB", estB);
          rec->append(point);
        };
      }; // end of MT_MP model
    };
  };
  isOk = true;
  return isOk; 
};



//
SgAPrioriRec* SgAPriories::lookupApRecord(const QString& key, const SgMJD& t)
{
  if (!contains(key))
    return NULL;
  // std::cout << qPrintable(key) << "\n";
  QList<SgAPrioriRec*>          recs=values(key);
  if (recs.size()==1)
    return recs.at(0);
  
  // more than one record:
  QMap<double, SgAPrioriRec*>   recByEpoch;
  for (int i=0; i<recs.size(); i++)
    recByEpoch.insert(recs.at(i)->getTsince().toDouble(), recs.at(i));
  recs.clear();
  recs = recByEpoch.values();
  //
  if (t==tZero)
    return recs.at(0);
  //
  if (recs.last()->getTsince() <= t)
    return recs.last();
  //
  int                           idx=recs.size() - 1;
  while (0<idx && t<=recs.at(idx)->getTsince())
    idx--;
  return recs.at(idx);
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


/*===== ================================================================================================*/
//
// constants:
//
/*=====================================================================================================*/








/*=====================================================================================================*/
