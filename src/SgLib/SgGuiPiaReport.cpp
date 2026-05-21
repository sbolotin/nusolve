/*
 *
 *    This file is a part of Space Geodetic Library. The library is used by
 *    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
 *    geodetic VLBI observations.
 *    Copyright (C) 2014-2020 Sergei Bolotin.
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




#include <SgGuiPiaReport.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QComboBox>
#   include <QtWidgets/QFrame>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QHeaderView>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPlainTextEdit>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#   include <QtWidgets/QSpinBox>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QCheckBox>
#   include <QtGui/QComboBox>
#   include <QtGui/QFrame>
#   include <QtGui/QGroupBox>
#   include <QtGui/QHeaderView>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPlainTextEdit>
#   include <QtGui/QPushButton>
#   include <QtGui/QRadioButton>
#   include <QtGui/QSpinBox>
#endif




#include <SgVlbiBand.h>
#include <SgVlbiObservation.h>







/*=======================================================================================================
*
*              SgGuiPiaReport's METHODS:
* 
*======================================================================================================*/
//
SgGuiPiaReport::SgGuiPiaReport(SgTaskConfig* config, SgParametersDescriptor* parDescriptor,
  SgVlbiSession* session, QWidget *parent, Qt::WindowFlags f)
  : QDialog(parent, f)
{
  config_ = config;
  parametersDescriptor_ = parDescriptor;
  session_ = session;
  
  bool                          isSuccess=session_->isAttr(SgVlbiSessionInfo::Attr_FF_AUTOPROCESSED);
  QString                       str("finished successfully");
  if (!isSuccess)
    str = "failed";
  
  setWindowTitle("Automatic data processing " + str);
  //  setWindowModality(Qt::WindowModal);

  QBoxLayout                   *layout;
  QLabel                       *label;
  layout = new QVBoxLayout(this);

  if (isSuccess)
  {
    // collect info and display it:
    QList<SgVlbiObservation*>   listOfExcludedObs;
    QStringList                 listOfStrings;
    for (int i=0; i<session_->observations().size(); i++)
    {
      SgVlbiObservation        *obs=session_->observations().at(i);
      if (obs->minQualityFactor() >= config_->getQualityCodeThreshold() && 
          (
//          obs->isAttr(SgVlbiObservation::Attr_NOT_VALID) ||
            obs->primeObs()->activeDelay()->isAttr(SgVlbiMeasurement::Attr_NOT_VALID)))
        listOfExcludedObs << obs;
    };
    QString                     str2copyBlName(""), strQCs("");
    for (int i=0; i<listOfExcludedObs.size(); i++)
    {
      SgVlbiObservation        *obs=listOfExcludedObs.at(i);
      SgVlbiObservable         *o=obs->primeObs();
      // need to specify a baseline:
      str2copyBlName = "";
      if (session_->baselinesByName().size() > 1)
        str2copyBlName = " " + obs->baseline()->getKey() + ",";
      // QC(s):
      strQCs = "";
      for (QMap<QString, SgVlbiObservable*>::iterator it_obs=obs->observableByKey().begin();
        it_obs!=obs->observableByKey().end(); ++it_obs)
        strQCs += it_obs.key() + ":" + QString("").setNum(it_obs.value()->getQualityFactor()) + ",";
      if (strQCs.size()>1)
        strQCs = strQCs.left(strQCs.size() - 1);
      //
      str.sprintf("observation #%d,%s %s, %s, QC=(%s) which fits at  %.2f +/- %.2f ps",
        o->getMediaIdx()+1, qPrintable(str2copyBlName), 
        qPrintable(o->src()->getKey()), qPrintable(o->epoch().toString(SgMJD::F_HHMMSS)),
        qPrintable(strQCs),
        o->activeDelay()->getResidual()*1.0e12,
        o->activeDelay()->getSigma()*1.0e12);
/*
        o->measurement(config_)->getResidual()*1.0e12,
        o->measurement(config_)->getSigma()*1.0e12);
*/
      listOfStrings << str;
    };
    // display info:
    int                         length=0, maxLength=0;

    layout->addWidget(label=new QLabel("The automatic data analysis of the session " +
      session_->getSessionCode() + " (" + session_->getName() + ") has been finished.", 
      this));
    label->setAlignment(Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(label=new QLabel("The summary of operations:",
      this));
    label->setAlignment(Qt::AlignLeft);
    //
    QGridLayout                  *grid=new QGridLayout;
    const QString                 sYes("Yes"), sNo("No");
    layout->addLayout(grid);
    grid->addWidget(new QLabel("Ionoshpere corrections:"),  0,0);
    grid->addWidget(new QLabel("Group delay ambiguities:"), 1,0);
    grid->addWidget(new QLabel("Clock breaks found:"),      2,0);
    grid->addWidget(new QLabel("Outlier processing:"),      3,0);
    grid->addWidget(new QLabel("Weight correction:"),       4,0);
    
    grid->addWidget(new QLabel(session_->isAttr(SgVlbiSessionInfo::Attr_FF_ION_C_CALCULATED)?sYes:sNo),
      0,1);
    grid->addWidget(new QLabel(session_->isAttr(SgVlbiSessionInfo::Attr_FF_AMBIGS_RESOLVED)?sYes:sNo),
      1,1);
    grid->addWidget(new QLabel(sNo),
      2,1);
    grid->addWidget(new QLabel(session_->isAttr(SgVlbiSessionInfo::Attr_FF_OUTLIERS_PROCESSED)?sYes:sNo),
      3,1);
    grid->addWidget(new QLabel(session_->isAttr(SgVlbiSessionInfo::Attr_FF_WEIGHTS_CORRECTED)?sYes:sNo),
      4,1);
    layout->addSpacing(15);
    
    grid=new QGridLayout;
    layout->addLayout(grid);
    grid->addWidget(new QLabel("Number of used observations:"),     0,0);
    grid->addWidget(new QLabel("Weighted root mean squares, ps:"),  1,0);
//    grid->addWidget(new QLabel("Normalized Chi2:"),                 2,0);
    grid->addWidget(new QLabel("Normalized &chi;<sup>2</sup>:"),    2,0);
    //
    SgVlbiBand                 *band=session_->primaryBand();
    grid->addWidget(label=new QLabel(str.sprintf("%d", band->numProcessed(DT_DELAY))),
      0,1);
//    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label=new QLabel(str.sprintf("%.2f", band->wrms(DT_DELAY)*1.0e12)),
      1,1);
//    label->setAlignment(Qt::AlignRight);
    grid->addWidget(label=new QLabel(str.sprintf("%.4f", 
      session->getNumOfDOF()>0.0?band->chi2(DT_DELAY)/session->getNumOfDOF():0.0)),
      2,1);
//    label->setAlignment(Qt::AlignRight);
//    grid->setColumnStretch ( 2, 1);
    layout->addSpacing(15);
    //
    if (listOfStrings.size())
    {
      layout->addWidget(label=new QLabel("The list of potentially usable but deselected observations:", 
        this));
      QPlainTextEdit               *otliersDisplay=new QPlainTextEdit(this);
      layout->addWidget(otliersDisplay);
      otliersDisplay->setReadOnly(true);

      // generate the list of removed observations:
      for (int i=0; i<listOfStrings.size(); i++)
      {
        otliersDisplay->appendPlainText(listOfStrings.at(i));
        length = otliersDisplay->fontMetrics().width(listOfStrings.at(i) + "W");
        if (maxLength < length)
          maxLength = length;
      };
      if (maxLength>0)
        otliersDisplay->setMinimumWidth(maxLength);
    }
    else
      layout->addWidget(new QLabel("No observations were deselected.", this));
  }
  else
  {
    layout->addSpacing(20);
    layout->addWidget(label=new QLabel(
    "Cannot perform automatic data analysis of the session " + 
      session_->getSessionCode() + " (" + session_->getName() + ").", this));
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label=new QLabel("Make the analysis manually.", this));
    label->setAlignment(Qt::AlignCenter);
    layout->addSpacing(20);
  };

  QFrame                       *bar=new QFrame(this);
  bar->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  layout->addWidget(bar);

  QBoxLayout                   *subLayout=new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);

  QPushButton                  *button;
  subLayout->addWidget(button=new QPushButton("Close", this));
  button->setMinimumSize(button->sizeHint());
  connect(button, SIGNAL(clicked()), SLOT(accept()));
};



//
SgGuiPiaReport::~SgGuiPiaReport()
{
  config_ = NULL;
  parametersDescriptor_ = NULL;
  session_ = NULL;
  emit windowClosed();
};



//
void SgGuiPiaReport::accept()
{
  QDialog::accept();
  deleteLater();
};
/*=====================================================================================================*/
