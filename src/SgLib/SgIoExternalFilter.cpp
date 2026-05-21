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

#include <unistd.h>

#include <SgIoExternalFilter.h>



/*=======================================================================================================
*
*                          SgIoExternalFilter's METHODS:
* 
*======================================================================================================*/
// static first:
const QString SgIoExternalFilter::className()
{
  return "SgExternalFilter";
};



//
FILE* SgIoExternalFilter::openFlt(const QString& fileName, QFile& file, QTextStream& ts, 
  FilterDirection d) const
{
  FILE                         *p=NULL;
  if (d == FLTD_Udefined)
    return NULL;

  const QString                &cmd=d==FLTD_Input?command2read_:command2write_;
  const char*                   popenFlag(d==FLTD_Input?"r":"w");

  if ((p=popen(qPrintable(cmd + " \"" + fileName + "\""), popenFlag)))
  {
    file.open(p, d==FLTD_Input?QIODevice::ReadOnly:(QIODevice::WriteOnly|QIODevice::Truncate));
    ts.setDevice(&file);
  }
  else
    logger->write(SgLogger::ERR, SgLogger::IO, className() +
      "::openFlt(): got an error: " + QString(strerror(errno)));

  return p;
};



//
void SgIoExternalFilter::closeFlt(FILE*& p, QFile& file, QTextStream& ts)
{
  file.close();
  if (p)
  {
    pclose(p);
    p = NULL;
  };
  ts.setDevice(NULL);
};



//
bool SgIoExternalFilter::selfCheck()
{
  if (command2read_ == "NONE" || command2write_ == "NONE")
    return true;  // cannot to check

  QString                       strWrite("QWERTY1234567890qwerty"), strRead, tmpNam;
  QFile                         f;
  QTextStream                   ts;
  FILE                         *pipe=NULL;
  const char                   *sFileTemplate="/tmp/SgLib_XXXXXX";
  char                         *tmpl=strdup(sFileTemplate);

  if ( !(pipe=openFlt((tmpNam=mktemp(tmpl)), f, ts, FLTD_Output)) )
  {
    free(tmpl);
    return false;
  };

  ts << strWrite;
  closeFlt(pipe, f, ts);
  free(tmpl);
  /*
  logger->write(SgLogger::DBG, SgLogger::IO, className() + 
    "::selfCheck(): file \"" + tmpNam + "\" has been created");
  */
  if ( !(pipe=openFlt(tmpNam, f, ts, FLTD_Input)) )
    return false;

  ts >> strRead;
  closeFlt(pipe, f, ts);

  if (unlink(qPrintable(tmpNam)) == -1) 
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::selfCheck(): file \"" + tmpNam + "\" cannot be deleted: " + QString(strerror(errno)));
  /*
  else
    logger->write(SgLogger::DBG, SgLogger::IO, className() +
      "::selfCheck(): file \"" + tmpNam + "\" deleted");
  */
  return strWrite==strRead;
};
/*=====================================================================================================*/






/*=======================================================================================================
*
*                          SgIoExtFilterHandler's METHODS:
* 
*======================================================================================================*/
// static first:
const QString SgIoExtFilterHandler::className()
{
  return "SgIoExtFilterHandler";
};



//
SgIoExtFilterHandler::SgIoExtFilterHandler() : filterByExt_()
{
  addFilter("GZIP", "gz", "gzip -dc", "gzip -c >");
  addFilter("BZIP2", "bz2", "bzip2 -dc", "bzip2 -c >");
//  addFilter("Compress", "Z", "compress -dc", "compress -c >");
};



//
SgIoExtFilterHandler::~SgIoExtFilterHandler()
{
  for (QMap<QString, const SgIoExternalFilter*>::iterator it=filterByExt_.begin(); 
    it!=filterByExt_.end(); it++)
    delete it.value();
  filterByExt_.clear();
};



//
bool SgIoExtFilterHandler::addFilter(const SgIoExternalFilter* filter)
{
  if (!filter)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::addFilter(): the filter is NULL");
    return false;
  }
  else if (!filter->getDefaultExtension().size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::addFilter(): the default file extension of the filter is empty");
    return false;
  }
  else if (filterByExt_.contains(filter->getDefaultExtension()))
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::addFilter(): the default file extension \"" + filter->getDefaultExtension() +
      "\" already registered");
    return false;
  };

  filterByExt_.insert(filter->getDefaultExtension(), filter);
  return true;
};



//
bool SgIoExtFilterHandler::removeFilter(const SgIoExternalFilter* filter)
{
  if (!filter)
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::removeFilter(): the filter is NULL");
    return false;
  }
  else if (!filter->getDefaultExtension().size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::removeFilter(): the default file extension of the filter is empty");
    return false;
  }
  else if (!filterByExt_.contains(filter->getDefaultExtension()))
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::removeFilter(): the default file extension \"" + filter->getDefaultExtension() +
      "\" is not registered");
    return false;
  };

  filterByExt_.remove(filter->getDefaultExtension());
  return true;
};



//
bool SgIoExtFilterHandler::removeFilter(const QString& extension)
{
  const SgIoExternalFilter* filter=NULL;
  if (!extension.size())
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::removeFilter(): the default file extension is empty");
    return false;
  }
  else if (!filterByExt_.contains(extension))
  {
    logger->write(SgLogger::DBG, SgLogger::IO, className() + 
      "::removeFilter(): the default file extension \"" + filter->getDefaultExtension() +
      "\" is not registered");
    return false;
  }
  else
    filter = filterByExt_.value(extension);

  filterByExt_.remove(extension);
  if (filter)
    delete filter;

  return true;
};



//
const SgIoExternalFilter* SgIoExtFilterHandler::lookupFilterByFileName(const QString& fileName)
{
  int                           idx=fileName.lastIndexOf('.');
  QString                       ext(-1<idx?fileName.mid(idx + 1):"");

  if (filterByExt_.contains(ext))
    return filterByExt_.value(ext);
  return NULL;
};



//
FILE* SgIoExtFilterHandler::openFlt(const QString& fileName, QFile& file, QTextStream& ts, 
  FilterDirection d)
{
  const SgIoExternalFilter     *filter=lookupFilterByFileName(fileName);
  if (filter)
    return filter->openFlt(fileName, file, ts, d);

  // not found: unknown format/uncompressed data:
  file.setFileName(fileName);
  if (file.open(d==FLTD_Input?QIODevice::ReadOnly:(QIODevice::WriteOnly|QIODevice::Truncate)))
    ts.setDevice(&file);
  else
  {
    logger->write(SgLogger::ERR, SgLogger::IO, className() + 
      "::openFlt(): cannot open file \"" + file.fileName() + "\"");    
    ts.setDevice(NULL);
  };
  return NULL;
};











/*=====================================================================================================*/
//
// constants:
//


SgIoExtFilterHandler compressors;


/*=====================================================================================================*/

/*=====================================================================================================*/
