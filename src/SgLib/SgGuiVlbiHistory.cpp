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

#include <QtCore/QRegExp>


#include <SgGuiVlbiHistory.h>
#include <SgLogger.h>
#include <SgVlbiHistory.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QAction>
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QMenu>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QShortcut>
#else
#   include <QtGui/QAction>
#   include <QtGui/QBoxLayout>
#   include <QtGui/QLabel>
#   include <QtGui/QMenu>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#   include <QtGui/QShortcut>
#endif




/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
SgGuiVlbiHistory::SgGuiVlbiHistory(SgVlbiHistory *history, QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  history_ = history;
  QString str;
  QBoxLayout *layout = new QVBoxLayout(this);
  
  twHistory_ = new QTreeWidget(this);
  twHistory_->setColumnCount(3);

//QBrush                        userRecordBrushF(QBrush(QColor(0, 140, 0)));
  QBrush                        userRecordBrushF(QBrush(QColor(30, 90, 255)));
  QBrush                        operatorRecordBrushF(QBrush(QColor(214, 61, 210)));

  QRegExp                       reUserComment("^\\w{2}>(.+)", Qt::CaseInsensitive);
  QRegExp                       reOperatorComment("readLogFile: station ([\\w\\s-+_]{1,8}): "
                                  "\\d{4}/\\d{2}/\\d{2}\\s+(.+)", Qt::CaseInsensitive);
  
  
  QStringList headerLabels;
  headerLabels 
    << "Date" 
    << "Ver" 
    << "Event"
    ;
  twHistory_->setHeaderLabels(headerLabels);
  for (int i=0; i<history_->size(); i++)
  {
    SgVlbiHistoryRecord* rec = history_->at(i);

    QTreeWidgetItem *item = new QTreeWidgetItem(twHistory_);

    item->setText(0, rec->getEpoch().toLocal().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    item->setData(0, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(0, Qt::UserRole, i); // == idx
    item->setText(1, str.sprintf("%3d", rec->getVersion()));
    item->setData(1, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setText(2, rec->getText());
    item->setData(2, Qt::TextAlignmentRole, Qt::AlignLeft);
    if (reUserComment.indexIn(rec->getText()) != -1)
    {
      QFont                     fnt=item->font(2);
      fnt.setBold(true);
      item->setFont(2, fnt);
      item->setForeground(2, userRecordBrushF);
    }
    else if (reOperatorComment.indexIn(rec->getText()) != -1)
    {
      QFont                     fnt=item->font(2);
      fnt.setBold(true);
      item->setFont(2, fnt);
      item->setForeground(2, operatorRecordBrushF);
    };
  };
  
  twHistory_->header()->resizeSections(QHeaderView::ResizeToContents);
#if QT_VERSION >= 0x050000
  twHistory_->header()->setSectionResizeMode(QHeaderView::Interactive);
#else
  twHistory_->header()->setResizeMode(QHeaderView::Interactive);
#endif
  twHistory_->header()->setStretchLastSection(true);
  
  //twHistory_->setSortingEnabled(true);
  //twHistory_->setUniformRowHeights(true);
  twHistory_->setFocus();
  twHistory_->setItemsExpandable(false);
  twHistory_->setAllColumnsShowFocus(true);

  layout->addWidget(twHistory_);
  
  QBoxLayout *hLayout = new QHBoxLayout();
  layout->addLayout(hLayout);

  historyLine_ = new QLineEdit(this);
  QLabel *label = new QLabel("&Add a record:", this);
  label->setBuddy(historyLine_);
  label->setMinimumSize(label->sizeHint());
  historyLine_->setMinimumHeight(historyLine_->sizeHint().height());
  historyLine_->setMinimumWidth(10*historyLine_->fontMetrics().width("WWW") + 10);

  hLayout->addWidget(label);
  hLayout->addWidget(historyLine_, 10);
  
  QPushButton *button = new QPushButton("Submit", this);
  button->setMinimumSize(button->sizeHint());
  button->setDefault(true);
  hLayout->addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(addHistoryLine()));
  connect (historyLine_, SIGNAL(returnPressed()), SLOT(addHistoryLine()));
  
  QShortcut                    *shCut=new QShortcut(QKeySequence(QKeySequence::Delete), this);
  connect(shCut, SIGNAL(activated()), this, SLOT(delHistoryLine()));
};



//
void SgGuiVlbiHistory::addHistoryLine()
{
  QBrush                        newRecordBrush(Qt::blue);
  QString str;
  QString text = historyLine_->text();
  if (text.simplified() != "")
  {
    int                         version=history_->last()->getVersion();
    int                         idx=history_->size();
    SgVlbiHistoryRecord        *rec=new SgVlbiHistoryRecord(SgMJD::currentMJD(), version, text, true);
    QTreeWidgetItem            *item=new QTreeWidgetItem(twHistory_);
    history_->append(rec);
    //
    if (0<idx && !history_->at(idx-1)->getIsEditable())
      history_->setFirstRecordFromUser(text);
//  item->setText(0, rec->getEpoch().toLocal().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    item->setText(0, rec->getEpoch().toString(SgMJD::F_YYYYMMDDHHMMSSSS));
    item->setData(0, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(0, Qt::ForegroundRole, newRecordBrush);
    item->setData(0, Qt::UserRole, idx);
    
    item->setText(1, str.sprintf("%3d", rec->getVersion()));
    item->setData(1, Qt::TextAlignmentRole, Qt::AlignRight);
    item->setData(1, Qt::ForegroundRole, newRecordBrush);
    item->setText(2, rec->getText());
    item->setData(2, Qt::TextAlignmentRole, Qt::AlignLeft);
    item->setData(2, Qt::ForegroundRole, newRecordBrush);
    
    historyLine_->setText("");
    emit modified(true);
  };
};



//
void SgGuiVlbiHistory::delHistoryLine()
{
  QTreeWidgetItem              *item=twHistory_->currentItem();
  if (item)
  { 
    bool                        isOk;
    int                         idx=item->data(0, Qt::UserRole).toInt(&isOk);
    if (isOk)
    {
      SgVlbiHistoryRecord      *rec=history_->at(idx);
      if (rec->getIsEditable())
      {
        if (QMessageBox::warning(this, "Warning",
          "Are you sure to delete the history record?\nThe record was issued on " + 
          rec->getEpoch().toString() + ". It says:\n" + rec->getText(),
          QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
          logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::SESSION, className() +
            ": delHistoryLine(): the history record \"" + rec->getText() + "\" sunk into oblivion");
          //
          // update first user's record (if necessary)
          if (idx==0)                                     // lonely record
            history_->setFirstRecordFromUser("");
          else if (!history_->at(idx-1)->getIsEditable()) // this one was the first user's record
          {
            if (idx<history_->size()-1)
              history_->setFirstRecordFromUser(history_->at(idx+1)->getText());
            else
              history_->setFirstRecordFromUser("");
          };
          //
          // remove from the list:
          history_->removeAt(idx);
          //
          // adjust indices:
          QTreeWidgetItem      *it=twHistory_->itemBelow(item);
          while (it)
          {
            it->setData(0, Qt::UserRole, idx++);
            it = twHistory_->itemBelow(it);
          };
          delete rec;
          delete item;
          emit modified(true);
        };
      };
    };
  };
};
/*=====================================================================================================*/
