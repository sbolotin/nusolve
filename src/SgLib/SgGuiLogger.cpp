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

#include <SgGuiLogger.h>
#if QT_VERSION >= 0x050000
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
SgGuiLogger::SgGuiLogger(QWidget* parent, int capacity, bool isStoreInFile, const QString& fileName)
  : QPlainTextEdit(parent), SgLogger(capacity, isStoreInFile, fileName)

{
  setReadOnly(true);
  setMaximumBlockCount (3*capacity_);

  errFormat = currentCharFormat();
  errFormat.setFontWeight(QFont::Bold);
  errFormat.setForeground(QColor(222, 0, 0));

  wrnFormat = currentCharFormat();
  wrnFormat.setFontWeight(QFont::Bold);
  //  wrnFormat.setForeground(QColor(30, 50, 180));
  //  wrnFormat.setForeground(QColor(204, 120, 0));
  wrnFormat.setForeground(QColor(30, 98, 255));

  infFormat = currentCharFormat();
  infFormat.setFontWeight(QFont::Bold);
  //  infFormat.setForeground(QColor(0, 125, 90));
  //  infFormat.setForeground(QColor(0, 125, 124));
  infFormat.setForeground(QColor(0, 127, 140));
 
  dbgFormat = currentCharFormat();
  setCenterOnScroll(true);
  //setTextInteractionFlags(Qt::NoTextInteraction);
};



void SgGuiLogger::makeOutput(LogLevel level, const QString &s, bool isAsync)
{
  QTextCharFormat              *format=NULL;
  switch (level)
  {
  case ERR: 
    format = &errFormat;
    break;
  case WRN: 
    format = &wrnFormat;
    break;
  case INF: 
    format = &infFormat;
    break;
  case DBG: 
  default:
    format = &dbgFormat;
    break;
  };

  setCurrentCharFormat(*format);
  appendPlainText(s);
//  appendHtml(s);
  if (isAsync)
    qApp->processEvents();
};



void SgGuiLogger::clearSpool()
{
  SgLogger::clearSpool();
  logger->write(DBG, IO_TXT, ClassName() + ": the spool has been cleared");
};
/*=====================================================================================================*/
