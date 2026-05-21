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

#ifndef SG_GUI_VLBI_HISTORY_H
#define SG_GUI_VLBI_HISTORY_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgVlbiSession.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QTreeWidget>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QHeaderView>
#   include <QtGui/QLineEdit>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QTreeWidget>
#   include <QtGui/QTreeWidgetItem>
#endif




class SgVlbiHistory;

/*=====================================================================================================*/
class SgGuiVlbiHistory : public QWidget
{
  Q_OBJECT
public:
  SgGuiVlbiHistory(SgVlbiHistory*, QWidget* =0, Qt::WindowFlags=0);
  virtual ~SgGuiVlbiHistory() {twHistory_=NULL;};

  signals:
  void modified(bool);

  private
  slots:
  void addHistoryLine();
  void delHistoryLine();

protected:
  SgVlbiHistory                *history_;
  QTreeWidget                  *twHistory_;
  QLineEdit                    *historyLine_;
  virtual inline QString className() const {return "SgGuiVlbiHistory";};
};
/*=====================================================================================================*/



/*=====================================================================================================*/
#endif // SG_GUI_VLBI_HISTORY_H
