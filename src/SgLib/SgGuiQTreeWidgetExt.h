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


#ifndef SG_GUI_QTREE_WIDGET_EXT_H
#define SG_GUI_QTREE_WIDGET_EXT_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QString>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QTreeWidget>
#   include <QtWidgets/QTreeWidgetItem>
#else
#   include <QtGui/QTreeWidget>
#   include <QtGui/QTreeWidgetItem>
#endif




// a little bit extended version of QTreeWidget


/*================================================================================================*/
class SgGuiQTreeWidgetExt : public QTreeWidget 
{
  Q_OBJECT

public:
  SgGuiQTreeWidgetExt(QWidget* =0);
  ~SgGuiQTreeWidgetExt() {};
  //
  inline Qt::MouseButton
                        getMouseButtonState() {return mouseButtonState_;};

  signals:
  void                  moveUponItem(QTreeWidgetItem*, int, Qt::MouseButton, Qt::KeyboardModifiers);
  void                  itemClickedWithButton(QTreeWidgetItem*, int, Qt::MouseButton);

  private
  slots:
  void                  movingStarted(QTreeWidgetItem*, int);

private:
  bool                  isMoving_;
  int                   mCol_;
  QTreeWidgetItem      *mItem_;
  Qt::MouseButton       mouseButtonState_;

protected:
  void                  mouseMoveEvent(QMouseEvent*);
  //void                  contentsMouseMoveEvent(QMouseEvent*);
  void                  mousePressEvent(QMouseEvent*);
//  void                  contentsMousePressEvent(QMouseEvent*);
  void                  mouseReleaseEvent(QMouseEvent*);
//  void                  contentsMouseReleaseEvent(QMouseEvent*);
};
/*================================================================================================*/


/*================================================================================================*/
#endif // SG_GUI_QTREE_WIDGET_EXT_H
