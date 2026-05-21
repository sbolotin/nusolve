/*
 *
 *    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
 *    and is designed to perform data analyis of a geodetic VLBI session.
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

#include "nuSolve.h"
#include "NsSessionNameDialog.h"


#include <SgLogger.h>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QPushButton>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QCheckBox>
#   include <QtGui/QGroupBox>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QPushButton>
#endif






/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsSessionNameDialog::NsSessionNameDialog(QWidget *parent, QString *fileName, QString *auxFileName,
  Qt::WindowFlags flags) : QDialog(parent, flags)
{
  QGroupBox                    *gbox;
  QGridLayout                  *grid;
  QLabel                       *label;
  bool                          isVdb = auxFileName==NULL; // a vgosDb session
  
  fileName_ = fileName;
  auxFileName_ = auxFileName;

  if (isVdb)
  {
    setWindowTitle("Open a session");
    gbox = new QGroupBox("Session name", this);
    grid = new QGridLayout(gbox);

    label= new QLabel("<h5>Loading default wrapper file for the specified session.</h5>"
      "<p>"
      "Enter a name of the session to open."
      "</p>"
      "<p>"
   //"The format of the name is one of: <b>YYMMMDDBC</b>[_V<b>xxx</b>]<br>"
      "The format of the name is one of:"
      "<ul>"
      "<li></t></t> <b>YYMMMDDBC</b>[_V<b>xxx</b>]</li>"
      "<li></t></t> <b>YYYYMMDD-SessionCode</b>[_V<b>xxx</b>]</li>"
      "</ul>"
      "Where"
      "<ul>"
      "<li><b>YY</b>   </t> - two digits of a year of observations</li>"
      "<li><b>YYYY</b> </t> - four digits of a year of observations</li>"
      "<li><b>MMM</b> - three chars of a month</li>"
      "<li><b>MM</b> - number of a month</li>"
      "<li><b>DD</b> - day of observation</li>"
      "<li><b>B</b> - band, usually, it is <b>X</b></li>"
      "<li><b>C</b> - network code</li>"
      "<li><b>SessionCode</b> - session code according to masterfile</li>"
      "<li><b>xxx</b> - version number of the session (optional)</li>"
      "</ul>"
      "</p>"
      "<p></p>", 
      gbox);
  }
  else
  {
    setWindowTitle("Open a database");
    gbox = new QGroupBox("Database file name", this);
    grid = new QGridLayout(gbox);

    label= new QLabel("<h5>Working through CALC/SOLVE catalog sysem.</h5>"
      "<p>"
      "Enter a name of the database to open. You can specify one file, "
      "X- or S-band, &nu;Solve will pick the second one automatically (if it exists). "
      "Or, you can provide both file names, but be sure that they belongs to one VLBI session."
      "</p>"
      "<p>"
      "The format of the name is: [$]<b>YYMMMDDBC</b>[_V<b>xxx</b>]<br>"
      "Where"
      "<ul>"
      "<li><b>YY</b> </t> - two digits of year of observation</li>"
      "<li><b>MMM</b> - three chars of month</li>"
      "<li><b>DD</b> - day of observation</li>"
      "<li><b>B</b> - band, usually, it is <b>X</b> or <b>S</b></li>"
      "<li><b>C</b> - network code</li>"
      "<li><b>xxx</b> - version number of the database (optional)</li>"
      "</ul>"
      "</p>"
      "<p></p>", 
      gbox);
  };

  label->setWordWrap(true);
//label->setMinimumSize(label->sizeHint());
  label->setMinimumWidth(1.5*label->sizeHint().width());
  label->setMinimumHeight(label->sizeHint().height());
  grid ->addWidget(label, 0, 0, 1, 2);
  grid ->setRowStretch(1, 2);

  label= new QLabel("Session name to open: ", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);

  leDbName_ = new QLineEdit("YYMMMDDBC_Vxxx", gbox);
  leDbName_->setFixedWidth(leDbName_->fontMetrics().width("YYMMMDDBC_VxxxQQQQ") + 10);
  leDbName_->selectAll();
  grid ->addWidget(leDbName_, 2, 1);

  //
  if (!isVdb)
  {
    cbAutoLoad2band_ = new QCheckBox("Load 2nd file (empty==AUTO):", gbox);
    cbAutoLoad2band_->setMinimumSize(cbAutoLoad2band_->sizeHint());
    grid->addWidget(cbAutoLoad2band_, 3, 0, Qt::AlignLeft | Qt::AlignVCenter);
    cbAutoLoad2band_->setCheckState(setup.getHave2AutoloadAllBands()?Qt::Checked:Qt::Unchecked);
    connect(cbAutoLoad2band_, SIGNAL(stateChanged(int)), SLOT(changeAutoload(int)));
    //
    leDbName2_ = new QLineEdit("", gbox);
    leDbName2_->setFixedWidth(leDbName_->fontMetrics().width("YYMMMDDBC_VxxxQQQQ") + 10);
    grid->addWidget(leDbName2_, 3, 1);

    if (!setup.getHave2AutoloadAllBands())
      leDbName2_->setEnabled(false);
  };
  //---
  QVBoxLayout                  *layout;
  QHBoxLayout                  *subLayout;
  layout = new QVBoxLayout(this);
  layout -> addWidget(gbox);
  subLayout = new QHBoxLayout();
  layout->addLayout(subLayout);
  subLayout->addStretch(1);

  QPushButton                  *bOk=new QPushButton("OK", this);
  QPushButton                  *bCancel=new QPushButton("Cancel", this);
  bOk->setDefault(true);
  QSize btnSize;

  bCancel->setMinimumSize((btnSize=bCancel->sizeHint()));
  bOk->setMinimumSize(btnSize);
  subLayout->addWidget(bOk);
  subLayout->addWidget(bCancel);
  connect(bOk, SIGNAL(clicked()), SLOT(accept()));
  connect(bCancel, SIGNAL(clicked()), SLOT(reject()));
};



//
NsSessionNameDialog::~NsSessionNameDialog()
{
};



//
void NsSessionNameDialog::changeAutoload(int chkState)
{
  leDbName2_->setEnabled(chkState==Qt::Checked);
};



//
void NsSessionNameDialog::accept()
{
  *fileName_ = leDbName_->text();

  if (auxFileName_)
  {
    if (leDbName2_->isEnabled())
      *auxFileName_ = leDbName2_->text();
    else
      *auxFileName_ = "/dev/null";
  };

  QDialog::accept();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": accept selected");
  deleteLater();
};



//
void NsSessionNameDialog::reject() 
{
  QDialog::reject();
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": reject selected");
  deleteLater();
};
/*=====================================================================================================*/
