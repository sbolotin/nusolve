/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2020 Sergei Bolotin.
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

#include <SgVex.h>



#include <QtCore/QFile>
#include <QtCore/QRegularExpression>








/*======================================================================================================
 *
 * SgVexParameter implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexParameter::className()
{
  return "SgVexParameter";
};



//
bool SgVexParameter::parseVexStatement(const QString& str)
{
  QRegularExpression            re("(\\S+)\\s*=\\s*(.*)");
  QRegularExpressionMatch       match=re.match(str);
  if (match.hasMatch())
  {
    values_.clear();
    key_ = match.captured(1).trimmed();
    if (match.captured(2).size())
    {
      QList<QString>            lst=match.captured(2).split(':');
      for (int i=0; i<lst.size(); i++)
        values_ << lst.at(i).trimmed();
    };
  }
  else
    return false;
  return true;
};
/*=====================================================================================================*/






/*======================================================================================================
 *
 * SgVexRefSatement implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexRefSatement::className()
{
  return "SgVexRefSatement";
};



//
bool SgVexRefSatement::parseVexStatement(const QString& str)
{
  QRegularExpression            re("ref\\s+(\\S+)\\s+=\\s+([^:]+):?(.*)");
  QRegularExpressionMatch       match=re.match(str);
  if (match.hasMatch())
  {
    qualifiers_.clear();
    qualifierByQ_.clear();
    key_ = match.captured(1).trimmed();
    value_ = match.captured(2).trimmed();
    if (match.captured(3).size())
    {
      QList<QString>            lst=match.captured(3).split(':');
      for (int i=0; i<lst.size(); i++)
      {
        qualifiers_ << lst.at(i);
        qualifierByQ_[lst.at(i)] = lst.at(i);
      };
    };
  }
  else
    return false;
  return true;
};
/*=====================================================================================================*/






/*======================================================================================================
 *
 * SgVexLiteralBlock implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexLiteralBlock::className()
{
  return "SgVexLiteralBlock";
};



//
bool SgVexLiteralBlock::parseVex(const QList<QString>& statements, int& idx)
{
  QRegularExpression            reEnd("end_literal\\((.*)\\)$");
  QRegularExpression            re("^start_literal\\((.*)\\)");
  QString                       str=statements.at(idx);
  QRegularExpressionMatch       match=re.match(str);
  if (match.hasMatch())
  {
    literalStrings_.clear();
    key_ = match.captured(1).trimmed();
    while(++idx < statements.size() && !reEnd.match((str=statements.at(idx))).hasMatch() )
      literalStrings_ << str;
  }
  else
    return false;
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::parseVex(): literalBlock \"" + key_ + "\": collected " + 
    QString("").setNum(literalStrings_.size()) + " string(s)");

  return true;
};
/*=====================================================================================================*/






/*======================================================================================================
 *
 * SgVexRefSatement implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexDefBlock::className()
{
  return "SgVexDefBlock";
};



//
SgVexDefBlock::SgVexDefBlock(const SgVexDefBlock& df) : key_(df.key_)
{
  for (int i=0; i<df.refStatements_.size(); i++)
  {
    SgVexRefSatement           *rs=new SgVexRefSatement(*df.refStatements_.at(i));
    refStatements_ << rs;
    refsByKey_.insertMulti(rs->getKey(), rs);
  };
  for (int i=0; i<df.parameters_.size(); i++)
  {
    SgVexParameter             *par=new SgVexParameter(*df.parameters_.at(i));
    parameters_ << par;
    parsByKey_.insertMulti(par->getKey(), par);
  };
  for (int i=0; i<df.literalas_.size(); i++)
    literalas_ << new SgVexLiteralBlock(*df.literalas_.at(i));
};




SgVexDefBlock::~SgVexDefBlock()
{
  refsByKey_.clear();
  parsByKey_.clear();
  for (int i=0; i<refStatements_.size(); i++)
    delete refStatements_.at(i);
  for (int i=0; i<parameters_.size(); i++)
    delete parameters_.at(i);
  for (int i=0; i<literalas_.size(); i++)
    delete literalas_.at(i);
  refStatements_.clear(); 
  parameters_.clear(); 
  literalas_.clear(); 
};



//
bool SgVexDefBlock::parseVex(const QList<QString>& statements, int& idx)
{
  const QString                 strEndDef("enddef");
  bool                          isOk=false;
  QString                       str=statements.at(idx);
  SgVexRefSatement             *rs=new SgVexRefSatement;
  SgVexParameter               *par=new SgVexParameter;
  SgVexLiteralBlock            *lit=new SgVexLiteralBlock;
  QRegularExpression            reDef("^def\\s+(.+)");
  QRegularExpression            reRef("^ref\\s+(.+)");
  QRegularExpression            reLit("start_literal\\(");
//QRegularExpression            reEndDef("enddef");
  QRegularExpressionMatch       match=reDef.match(str);
  if (match.hasMatch())
  {
    refStatements_.clear();
    parameters_.clear();
    literalas_.clear();
    key_ = match.captured(1).trimmed();

    while(++idx < statements.size() && (str=statements.at(idx)) != strEndDef)
    {
      if (reRef.match(str).hasMatch() && rs->parseVexStatement(str))
      {
        refStatements_ << rs;
        refsByKey_.insertMulti(rs->getKey(), rs);
        rs = new SgVexRefSatement;
      }
      else if (reLit.match(str).hasMatch() && lit->parseVex(statements, idx))
      {
        literalas_ << lit;
        lit = new SgVexLiteralBlock;
      }
      else if (par->parseVexStatement(str))
      {
        parameters_ << par;
        parsByKey_.insertMulti(par->getKey(), par);
        par = new SgVexParameter;
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
          "::parseVex(): encountered an unknown statement \"" + str + "\"");
    };
    isOk = true;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::parseVex(): defBlock \"" + key_ + "\": collected " + QString("").setNum(refStatements_.size()) +
    " ref(s) and " + QString("").setNum(parameters_.size()) + " par(s)");

  return isOk;
};
/*=====================================================================================================*/







/*======================================================================================================
 *
 * SgVexScanBlock implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexScanBlock::className()
{
  return "SgVexScanBlock";
};



//
bool SgVexScanBlock::parseVex(const QList<QString>& statements, int& idx)
{
  const QString                 strEndScan("endscan");
  bool                          isOk=false;
  QString                       str=statements.at(idx);
  SgVexParameter               *par=new SgVexParameter;
  QRegularExpression            reScan("^scan\\s+(.+)");
  QRegularExpressionMatch       match=reScan.match(str);
  if (match.hasMatch())
  {
    parameters_.clear();
    key_ = match.captured(1).trimmed();

    while(++idx < statements.size() && (str=statements.at(idx)) != strEndScan)
    {
      if (par->parseVexStatement(str))
      {
        parameters_ << par;
        par = new SgVexParameter;
      }
      else
        logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
          "::parseVex(): encountered an unknown statement \"" + str + "\"");
    };
    isOk = true;
  };
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::parseVex(): scanBlock \"" + key_ + "\": collected " + QString("").setNum(parameters_.size()) + 
    " par(s)");

  return isOk;
};
/*=====================================================================================================*/







/*======================================================================================================
 *
 * SgVexSection implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexSection::className()
{
  return "SgVexSection";
};



SgVexSection::~SgVexSection()
{
  refByKey_.clear();
  defByKey_.clear();
  scanByKey_.clear();

  for (int i=0; i<refStatements_.size(); i++)
    delete refStatements_.at(i);
  for (int i=0; i<defBlocks_.size(); i++)
    delete defBlocks_.at(i);
  for (int i=0; i<scanBlocks_.size(); i++)
    delete scanBlocks_.at(i);
  statements_.clear(); 
  refStatements_.clear(); 
  defBlocks_.clear(); 
  scanBlocks_.clear();
};



//
bool SgVexSection::parseVex(const QList<QString>& statements, int& idx)
{
  while(idx < statements.size() && statements.at(idx).at(0)!='$')
    statements_ << statements.at(idx++);

  refStatements_.clear();
  defBlocks_.clear();
  scanBlocks_.clear();

  QRegularExpression            reRef("^ref\\s+(.+)");
  QRegularExpression            reDef("^def\\s+(.+)");
  QRegularExpression            reScn("^scan\\s+(.+)");

  SgVexRefSatement             *rs=new SgVexRefSatement;
  SgVexDefBlock                *df=new SgVexDefBlock;
  SgVexScanBlock               *sb=new SgVexScanBlock;
  for (int i=0; i<statements_.size(); i++)
  {
    if (reDef.match(statements_.at(i)).hasMatch() && df->parseVex(statements_, i))
    {
      defBlocks_ << df; 
      defByKey_[df->getKey()] = df;
      df = new SgVexDefBlock;
    }
    else if (reRef.match(statements_.at(i)).hasMatch() && rs->parseVexStatement(statements_.at(i)))
    { 
      refStatements_ << rs;
      refByKey_.insertMulti(rs->getKey(), rs);
      rs = new SgVexRefSatement;
    }
    else if (reScn.match(statements_.at(i)).hasMatch() && sb->parseVex(statements_, i))
    { 
      scanBlocks_ << sb;
      scanByKey_[sb->getKey()] = sb;
      sb = new SgVexScanBlock;
    }
    else
      std::cout << "  ++ unparsed [" << qPrintable(statements_.at(i)) << "]\n";
  };
  logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
    "::parseVex(): section \"" + key_ + "\": collected " + QString("").setNum(defBlocks_.size()) +
    " def(s), " + QString("").setNum(refStatements_.size()) + " ref(s) and " +
    QString("").setNum(scanBlocks_.size()) + " scan(s)");

  return true;
};
/*=====================================================================================================*/











/*======================================================================================================
 *
 * SgVexFile implementation
 *
 *=====================================================================================================*/
//
// static first:
const QString SgVexFile::className()
{
  return "SgVexFile";
};



//
SgVexFile::SgVexFile() :
  inputFileName_(""),
  exper_(""),
  experName_(""),
  experDescription_(""),
  experNominalStart_(tZero),
  experNominalStop_(tZero),
  piName_(""),
  piEmail_(""),
  contactName_(""),
  contactEmail_(""),
  schedulerName_(""),
  schedulerEmail_(""),
  targetCorrelator_(""),
  sections_(),
  sectionByName_(),
  stnSetupById_()
{

};



//
SgVexFile::SgVexFile(const QString& inputFileName) :
  inputFileName_(inputFileName),
  exper_(""),
  experName_(""),
  experDescription_(""),
  experNominalStart_(tZero),
  experNominalStop_(tZero),
  piName_(""),
  piEmail_(""),
  contactName_(""),
  contactEmail_(""),
  schedulerName_(""),
  schedulerEmail_(""),
  targetCorrelator_(""),
  sections_(),
  stnSetupById_()
{

};




//
SgVexFile::~SgVexFile()
{
  for (int i=0; i<sections_.size(); i++)
    delete sections_.at(i);
  sections_.clear();
  stnSetupById_.clear();
};



//
bool SgVexFile::parseVexFile(const QString& fileName)
{
  bool                          isOk=false;
  if (fileName.size())
    inputFileName_ = fileName;


  QFile                         f(inputFileName_);
  QString                       str("");
//QRegularExpression            reComments("([^\\*.]*)\\*(.*)");
  QRegularExpression            reComments("([^\\*]*)\\*(.*)");

  if (f.open(QFile::ReadOnly))
  {
    QTextStream                 s(&f);
    int                         strCount=0;
    QString                     statement("");
    statements_.clear();
    
    
    while (!s.atEnd())
    {
      str = s.readLine().trimmed();
      strCount++;

      QRegularExpressionMatch   matchComments=reComments.match(str);
      if (matchComments.hasMatch())
        str = matchComments.captured(1).trimmed();

      // skip empty strings:
      if (str.size())
      {
        if (str.contains(';'))
        {
          QList<QString>        lst=str.split(';');
          for (int i=0; i<lst.size(); i++)
          {
            str = lst.at(i).trimmed();
            statement += str;
            if (statement.size())
              statements_.append(statement);
            statement = "";
          };
        }
        else
          statement += str + " ";
      };
    };
    logger->write(SgLogger::DBG, SgLogger::IO_TXT, className() +
      "::parseVexFile(): read " + QString("").setNum(strCount) + " strings, " + 
      QString("").setNum(statements_.size()) + " statements from " + f.fileName());

std::cout << "\n\n";
for (int i=0; i<statements_.size(); i++)
std::cout << "   " << i << ": [" << qPrintable(statements_.at(i)) << "]\n";

    s.setDevice(NULL);
  };
  f.close();
  
  //
  if (statements_.size() < 2)
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::parseVexFile(): not enough statements to proceed");
    return false;
  };

  str = statements_.at(0);
  if (str.left(9) != "VEX_rev =")
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::parseVexFile(): got unknown file type");
    return false;
  };
  
  // process sections:
  int                           idx;
  idx = 1;
  while (idx < statements_.size() && parseSection(idx))
    ;

  
  for (int i=0; i<sections_.size(); i++)
  {
std::cout << "  ++ " << i << "  -> " << qPrintable(sections_.at(i)->getKey()) 
<< "  n=" << sections_.at(i)->getDefBlocks().size()
<< "\n";  

    sectionByName_[sections_.at(i)->getKey()] = sections_.at(i);
  };

  analyzeVex();
std::cout << "  analyzed\n";  

  return isOk;
};



//
bool SgVexFile::parseSection(int& idx)
{
  bool                          isOk;
  isOk = true;

  if (statements_.at(idx).at(0) == '$')
  {
    SgVexSection               *sect=new SgVexSection(statements_.at(idx++));
    sections_ << sect;
    sect->parseVex(statements_, idx);
  }
  else
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
      "::parseSection(): unexpected begin of a section: \"" + statements_.at(idx) + "\", idx=" +
      QString("").setNum(idx));
    return false;
  };
  return isOk;
};



//
bool SgVexFile::analyzeVex()
{
  QString                       str;
  const QString                 snStation("$STATION");
  const QString                 snBbc("$BBC");
  const QString                 snFreq("$FREQ");
  const QString                 snIf("$IF");
  const QString                 snMode("$MODE");
  const QString                 snSite("$SITE");
  //
  QList<QString>                stations;
  QMap<QString, QString>        stationById;
  QList<SgVexRefSatement*>      refs;

std::cout << "  ++ 0\n";  
  if (!sectionByName_.contains(snStation))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snStation + "\", exitting");
    return false;
  };
  if (!sectionByName_.contains(snBbc))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snBbc + "\", exitting");
    return false;
  };
  if (!sectionByName_.contains(snFreq))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snFreq + "\", exitting");
    return false;
  };
  if (!sectionByName_.contains(snIf))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snIf + "\", exitting");
    return false;
  };
  if (!sectionByName_.contains(snMode))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snMode + "\", exitting");
    return false;
  };
  if (!sectionByName_.contains(snSite))
  {
    logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
      "::analyzeVex(): canot find section \"" + snSite + "\", exitting");
    return false;
  };


  const SgVexSection           *sectStations=sectionByName_[snStation];
  const SgVexSection           *sectMode=sectionByName_[snMode];

  if (sectMode->getDefBlocks().size() != 1)
  {
    
  };
  
  // one def in $mode secton:
  const SgVexDefBlock          *pModeDef=sectMode->getDefBlocks().at(0);



  for (int i=0; i<sectStations->getDefBlocks().size(); i++)
  {
    const SgVexDefBlock        *pStnDef=sectStations->getDefBlocks().at(i);
    const SgVexRefSatement     *ref=lookupRef(pStnDef, snSite);
    StationSetup*               stnS;
    if (!ref)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + snSite + "\" in \"" + pStnDef->getKey() + "\", exitting");
      return false;
    };
    const SgVexDefBlock        *pSiteDef=lookupDef(ref->getKey(), ref->getValue());
    if (!pSiteDef)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + ref->getValue() + "\" in " + ref->getKey()
        + ", exitting");
      return false;
    };
    SgVexParameter             *par;
    
    QString                     siteId(""), siteName("");

    par = pSiteDef->parsByKey().value("site_ID");
    if (!par)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"site_ID\" in " + pSiteDef->getKey()
        + ", exitting");
      return false;
    };
    siteId = par->getValues().at(0);
    
    par = pSiteDef->parsByKey().value("site_name");
    if (!par)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"site_ID\" in " + pSiteDef->getKey()
        + ", exitting");
      return false;
    };
    siteName = par->getValues().at(0);

    stationById[siteId] = siteName;
    
    const SgVexRefSatement     *refFreq=lookupRef(pModeDef, snFreq, siteId);
    const SgVexRefSatement     *refIf=lookupRef(pModeDef, snIf, siteId);
    const SgVexRefSatement     *refBbc=lookupRef(pModeDef, snBbc, siteId);

    if (!refFreq)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + snFreq + "\" in \"" + snMode + "\" for " + 
        siteName + ", exitting");
      return false;
    };
    if (!refIf)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + snIf + "\" in \"" + snMode + "\" for " + 
        siteName + ", exitting");
    };
    if (!refBbc)
    {
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + snBbc + "\" in \"" + snMode + "\" for " + 
        siteName + ", exitting");
    };
    stnS = new StationSetup;
    stnSetupById_.insert(siteId, stnS);
    

    // check IF:
    const SgVexDefBlock        *df=lookupDef(refIf->getKey(), refIf->getValue());
    QList<SgVexParameter*>      pars=df->parsByKey().values("if_def");
    for (int i=0; i<pars.size(); i++)
    {
      IfSetup                  *ifS=ifSetup(pars.at(i));
      stnS->ifs() << ifS;
    };
    //
    // compose BBC:
    df = lookupDef(refBbc->getKey(), refBbc->getValue());
    pars = df->parsByKey().values("BBC_assign");
    for (int i=0; i<pars.size(); i++)
    {
      BbcSetup                  *bbcS=bbcSetup(pars.at(i));
      stnS->bbcs() << bbcS;
    };
    //
    // and channels:
    df = lookupDef(refFreq->getKey(), refFreq->getValue());
    pars = df->parsByKey().values("chan_def");
    for (int i=0; i<pars.size(); i++)
    {
      FreqSetup                  *freqS=freqSetup(pars.at(i));
      stnS->freqs() << freqS;
    };
  
  
  
  };




  for (QMap<QString, QString>::iterator it=stationById.begin(); it!=stationById.end(); ++it)
  {
std::cout << "   :: got: [" << qPrintable(it.key()) << "] -> [" << qPrintable(it.value()) << "]\n"; 

    QString                     siteId = it.key();
    if (stnSetupById_.contains(siteId))
    {
      StationSetup             *stnS=stnSetupById_.value(siteId);
      if (stnS)
      {
        std::cout << "   .. " << qPrintable(it.value()) << " got " << stnS->getIfs().size() << " IF setup records:\n";
        for (int i=0; i<stnS->getIfs().size(); i++)
        {
          IfSetup              *ifS=stnS->getIfs().at(i);
          std::cout << "  " << i << ": " << qPrintable(ifS->getIfId()) << " : \""
          << qPrintable(ifS->getIfName()) << "\"  LO= " << ifS->getTotalLo()
          << " SideBand: " << qPrintable(sideBand2Str(ifS->getNetSideBand()))
          << " Polarizations: " << qPrintable(polarization2Str(ifS->getPolarization()))
          << "\n";
        };
        std::cout << "   .. " << " got " << stnS->getBbcs().size() << " BBC setup records:\n";
        for (int i=0; i<stnS->getBbcs().size(); i++)
        {
          BbcSetup             *bbcS=stnS->getBbcs().at(i);
          std::cout << "  " << i << ": " << qPrintable(bbcS->getBbcId()) << "  IF = "
          << qPrintable(bbcS->getIfId()) << "  phys.num= " << bbcS->getPhysNumber()
          << "\n";
        };
        std::cout << "   .. " << " got " << stnS->getFreqs().size() << " FREQ setup records:\n";
        for (int i=0; i<stnS->getFreqs().size(); i++)
        {
          FreqSetup            *freqS=stnS->getFreqs().at(i);
          std::cout << "  " << i 
          << ": Band=[" << qPrintable(freqS->getBandId()) << "]"
          << " skyFreq = " << freqS->getSkyFreq()
          << " SideBand: [" << qPrintable(sideBand2Str(freqS->getNetSideBand())) << "]"
          << " bandwidth = " << freqS->getBandwidth()
          << " Chan=[" << qPrintable(freqS->getChanId()) << "]"
          << " BBCId=[" << qPrintable(freqS->getBbcId()) << "]"
          << " PhaseCalId=[" << qPrintable(freqS->getPhaseCalId()) << "]"
          << "\n";
        };
     
     
     
      }
      else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::analyzeVex(): found \"" + siteId + "\" station setup is NULL");
    }
    else
      logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() +
        "::analyzeVex(): canot find \"" + siteId + "\" in the station setups");
    
  
  
  };



  return true;
};



SgVexFile::IfSetup* SgVexFile::ifSetup(const SgVexParameter* par)
{
  const QList<QString>&     lst=par->getValues();
  int                       l=lst.size();
  QString                   str("");
  QString                   ifId("");
  QString                   ifName("");
  double                    totalLo(0.0);
  SgChannelPolarization     polarization(CP_UNDEF);
  SgChannelSideBand         netSideBand(CSB_UNDEF);
 
  ifId = lst.at(0);
  if (1 < l)
    ifName = lst.at(1);
  if (2 < l)
  {
    str = lst.at(2);
    if (str == "R")
      polarization = CP_RightCP;
    else if (str == "L")
      polarization = CP_LeftCP;
    else if (str == "H")
      polarization = CP_HorizontalLP;
    else if (str == "V")
      polarization = CP_VerticalLP;
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::ifSetup(): got an unlnown polarization code: \"" + str + "\"");
  };
  if (3 < l)
    totalLo = str2frq(lst.at(3));
  if (4 < l)
  {
    str = lst.at(4);
    if (str == "L")
      netSideBand = CSB_LSB;
    else if (str == "U")
      netSideBand = CSB_USB;
  };
  
  return new IfSetup(ifId, ifName, totalLo, polarization, netSideBand);
};



//
SgVexFile::BbcSetup* SgVexFile::bbcSetup(const SgVexParameter* par)
{
  const QList<QString>&     lst=par->getValues();
  int                       l=lst.size();

  bool                      isOk;
  QString                   bbcId("");
  QString                   ifId("");
  int                       physNumber(0);
 
  bbcId = lst.at(0);
  if (1 < l)
  {
    physNumber = lst.at(1).toInt(&isOk);
    if (!isOk)
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::bbcSetup(): cannot convert \"" + lst.at(1) + "\" to int");
  }
  if (2 < l)
    ifId = lst.at(2);
  //
  return new BbcSetup(bbcId, ifId, physNumber);
};



//
SgVexFile::FreqSetup* SgVexFile::freqSetup(const SgVexParameter* par)
{
  const QList<QString>&     lst=par->getValues();
  int                       l=lst.size();
  QString                   str("");
  QString                   bandId("");
  double                    skyFreq(0.0);
  SgChannelSideBand         netSideBand(CSB_UNDEF);
  double                    bandwidth(0.0);
  QString                   chanId("");
  QString                   bbcId("");
  QString                   phaseCalId("");
  //
  bandId = lst.at(0);
  if (1 < l)
    skyFreq = str2frq(lst.at(1));
  if (2 < l)
  {
    str = lst.at(2);
    if (str == "L")
      netSideBand = CSB_LSB;
    else if (str == "U")
      netSideBand = CSB_USB;
  };
  if (3 < l)
    bandwidth = str2frq(lst.at(3));
  if (4 < l)
    chanId = lst.at(4);
  if (5 < l)
    bbcId = lst.at(5);
  if (6 < l)
    phaseCalId = lst.at(6);
  //
  return new FreqSetup(bandId, skyFreq, netSideBand, bandwidth, chanId, bbcId, phaseCalId);
};





//
const SgVexDefBlock* SgVexFile::lookupDef(const QString& sectName, const QString& refName)
{
  const SgVexSection           *sect=sectionByName_[sectName];
  SgVexDefBlock                *def=NULL;
  if (sect)
  {
    if (sect->getDefByKey().contains(refName))
    {
      def = sect->getDefByKey().value(refName);
    };
  };
std::cout << "--lookupDef: [" << (def?qPrintable(def->getKey()):"NULL") << "]\n";
  return def;
}


//
const SgVexRefSatement* SgVexFile::lookupRef(const QString& sectName, const QString& refCode)
{
  const SgVexSection           *sect=sectionByName_[sectName];
  const SgVexRefSatement       *ref=NULL;
  if (sect && sect->getRefByKey().contains(refCode))
    ref = sect->getRefByKey().value(refCode);

std::cout << "--lookupRef: [" << (ref?qPrintable(ref->getValue()):"NULL") << "]\n";

  return ref;
};



//
const SgVexRefSatement* SgVexFile::lookupRef(const SgVexDefBlock* defBlock, const QString& refCode)
{
  const SgVexRefSatement       *ref=NULL;

  if (defBlock && defBlock->refsByKey().contains(refCode))
    ref = defBlock->refsByKey().value(refCode);

std::cout << "--lookupRef: [" << (ref?qPrintable(ref->getValue()):"NULL") << "]\n";

  return ref;
};




const SgVexRefSatement* SgVexFile::lookupRef(const SgVexDefBlock* defBlock, const QString& refCode, 
  const QString& qualifier)
{
  const SgVexRefSatement       *ref=NULL;

std::cout << "--lookupRef: [" << qPrintable(refCode) << "][" << qPrintable(qualifier) << "] ";


  if (defBlock && defBlock->refsByKey().contains(refCode))
  {
    if (defBlock->refsByKey().values(refCode).size() == 1)
    {
std::cout << "(single)";
      ref = defBlock->refsByKey().value(refCode);
    }
    else
    {
std::cout << "(multi)";
      for (int i=0; i<defBlock->refsByKey().values(refCode).size(); i++)
        if (defBlock->refsByKey().values(refCode).at(i)->getQualifierByQ().contains(qualifier))
          ref = defBlock->refsByKey().values(refCode).at(i);      
    };
  };
  
std::cout << " => [" << (ref?qPrintable(ref->getValue()):"NULL") << "]\n";

  return ref;
};



//
double SgVexFile::str2frq(const QString& str) const
{
  double                        f;
  bool                          isOk;
  double                        scale;
  f = 0.0;
  scale = 1.0;

  f = str.toDouble(&isOk);
  if (!isOk)
  {
    if (str.right(4) == " MHz")
    {
      scale = 1.0;
      f = str.left(str.size() - 4).toDouble(&isOk);
      if (!isOk)
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::str2frq(): cannot convert to double \"" + str.left(str.size() - 4) + "\"");
    }
    else if (str.right(4) == " GHz")
    {
      scale = 1.0e3;
      f = str.left(str.size() - 4).toDouble(&isOk);
      if (!isOk)
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::str2frq(): cannot convert to double \"" + str.left(str.size() - 4) + "\"");
    }
    else if (str.right(4) == " kHz")
    {
      scale = 1.0e-3;
      f = str.left(str.size() - 4).toDouble(&isOk);
      if (!isOk)
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::str2frq(): cannot convert to double \"" + str.left(str.size() - 4) + "\"");
    }
    else if (str.right(3) == " Hz")
    {
      scale = 1.0e-6;
      f = str.left(str.size() - 3).toDouble(&isOk);
      if (!isOk)
        logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
          "::str2frq(): cannot convert to double \"" + str.left(str.size() - 3) + "\"");
    }
    else
      logger->write(SgLogger::ERR, SgLogger::IO_TXT, className() +
        "::str2frq(): cannot convert to double \"" + str + "\"");
  };
  
  return f*scale;
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
QString sideBand2Str(SgChannelSideBand sb)
{
  if (sb == CSB_LSB)
    return "LSB";
  else if (sb == CSB_USB)
    return "USB";
  else if (sb == CSB_DUAL)
    return "DUO";
  else
    return "N/A";
};



QString polarization2Str(SgChannelPolarization p)
{
  if (p == CP_LeftCP)
    return "LCP";
  else if (p == CP_RightCP)
    return "RCP";
  else if (p == CP_HorizontalLP)
    return "HLP";
  else if (p == CP_VerticalLP)
    return "VLP";
  else
    return "N/A";
};



// i/o:




/*=====================================================================================================*/
//
// constants:
//



/*=====================================================================================================*/
