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

#include <math.h>


#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <SgEccDat.h>
#include <SgEccRec.h>
#include <SgEccSite.h>
#include <SgLogger.h>

/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
//
// static first:
const QString SgEccDat::className()
{
  return "SgEccDat";
};



// A destructor:
SgEccDat::~SgEccDat()
{
  QMap<QString, SgEccSite*>::iterator it=siteByName_.begin();
  for (; it!=siteByName_.end(); ++it)
    delete it.value();
  siteByName_.clear();
};



//
void SgEccDat::importEccFile()
{
  QString                       str("");
  QDir                          dir(path2File_);
  if (path2File_.size() && !dir.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": importEccFile(): the directory [" + path2File_ + 
      "] does not exist; import of eccentricities is not available");
    return;
  };
  if (path2File_.size())
    str = path2File_ + "/";
  str += fileName_;
  QFile                         f(str);
  if (!f.exists())
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": importEccFile(): the file [" + fileName_ + 
      "] does not exist; import of eccentricities is not available");
    return;
  };
  SgEccSite          *site;
  SgEccRec            rec;
  if (f.open(QFile::ReadOnly))
  {
    QTextStream       s(&f);
    while (!s.atEnd())
    {
      str = s.readLine();
      if (str.size()>89 && 
          str.mid(0,1) == " " && // all others are comments
          rec.parseString(str))
      {
        if (siteByName_.contains(rec.getSiteName()))
          site = siteByName_.find(rec.getSiteName()).value();
        else
        {
          site = new SgEccSite(rec.getSiteName());
          siteByName_.insert(rec.getSiteName(), site);
        };
        site->insertRecord(new SgEccRec(rec));
      };
    };
    f.close();
    s.setDevice(NULL);
  };
  QMap<QString, SgEccSite*>::iterator it=siteByName_.begin();
  for (; it!=siteByName_.end(); ++it)
    it.value()->checkRecords();
  logger->write(SgLogger::INF, SgLogger::IO_TXT, className() + 
      ": importEccFile(): imported " + QString().setNum(siteByName_.size()) + 
      " ecc sites from the file " + f.fileName());
};



//
SgEccRec* SgEccDat::lookupRecord(const QString& siteName, const SgMJD& t)
{
  SgEccSite      *site=NULL;
  
  if (siteByName_.contains(siteName))
    site = siteByName_.find(siteName).value();
  else 
  {
    logger->write(SgLogger::WRN, SgLogger::IO_TXT, className() + 
      ": lookupRecord(): cannot find site " + siteName + " in the ecc site list");
    return NULL;
  };
  return site->findRecord(t);
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
