/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
 *    Copyright (C) 2025 Sergei Bolotin.
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

#ifndef NS_BROWSE_OBSERVATION_H
#define NS_BROWSE_OBSERVATION_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QList>
#include <QtCore/QString>



#if QT_VERSION >= 0x050000
#    include <QtWidgets/QDialog>
#    include <QtWidgets/QTreeWidgetItem>
#else
#    include <QtGui/QDialog>
#    include <QtGui/QTreeWidgetItem>
#endif


class QTreeWidget;




#include <SgGuiPlotter.h>
#include <SgGuiQTreeWidgetExt.h>
#include <SgVlbiObservation.h>




////
/////*=====================================================================================================*/
////class NsQTreeWidgetItem : public QTreeWidgetItem
////{
////public:
////   NsQTreeWidgetItem(QTreeWidget *parent, int type = Type) : QTreeWidgetItem(parent, type) {};
////  virtual ~NsQTreeWidgetItem() {};
////
////  virtual bool operator<(const QTreeWidgetItem &other) const;
////};
/////*=====================================================================================================*/






/*=====================================================================================================*/
class NsBrowseObservation : public QDialog
{
  Q_OBJECT
public:

  //
  // constructors/destructors:
  //
  /**A constructor.
   */
  NsBrowseObservation(SgVlbiObservation *obs,
    QWidget *parent=0, Qt::WindowFlags flags=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsBrowseObservation();



  //
  // Interfaces:
  //



  //
  // Functions:
  //
  inline QString className() const {return "NsBrowseObservation";};
  

  //
  // Friends:
  //



  //
  // I/O:
  //
  //

private slots:
  void accept();
  void reject();

private:
  SgVlbiObservation            *obs_;

  QList<SgPlotCarrier*>         plotCarriers4ChannelData_;
  QList<SgPlot*>                plots4ChannelData_;
  QList<SgPlotCarrier*>         plotCarriers4PcalData_;
  QList<SgPlot*>                plots4PcalData_;

  QWidget*                      tab4GeneralInfo();
  QWidget*                      tab4Bands();
  QWidget*                      tab4Band(SgVlbiObservable*);
  QWidget*                      tab4EditingInfo(SgVlbiObservable*);
  QWidget*                      tab4ChannelList(SgVlbiObservable*);
  QWidget*                      tab4ChannelPlots(SgVlbiObservable*);
  QWidget*                      tab4CPcalPlots(SgVlbiObservable*);



protected:
  virtual void closeEvent(QCloseEvent *) {deleteLater();};
};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // NS_BROWSE_OBSERVATION_H
