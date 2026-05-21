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


#include <SgGuiLoggerConfig.h>




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
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QSpinBox>
#   include <QtWidgets/QRadioButton>
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
#   include <QtGui/QPushButton>
#   include <QtGui/QSpinBox>
#   include <QtGui/QRadioButton>
#endif



extern const QString facilityNames[];
extern const int numOfFacilities;
extern const QString levelNames[4];
extern const int logFacilities[32];
/*=======================================================================================================
*
*              SgGuiTaskConfig's METHODS:
* 
*======================================================================================================*/
//
SgGuiLoggerConfig::SgGuiLoggerConfig(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
{
  QGroupBox                    *gBox;
  QBoxLayout                   *mainLayout;
  QLabel                       *label;
  QGridLayout                  *gridLayout;

  mainLayout = new QVBoxLayout(this);
  mainLayout -> addStretch(1);

  gBox = new QGroupBox("Logger's Options", this);
  gridLayout = new QGridLayout(gBox);

  eLogFileName_ = new QLineEdit(gBox);
  eLogFileName_->setText(logger->getFileName());
  label = new QLabel("Log file name:", gBox);
  label->setMinimumSize(label->sizeHint());
  eLogFileName_->setMinimumSize(eLogFileName_->sizeHint());

  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(eLogFileName_, 0, 1);

  sLogCapacity_ = new QSpinBox(gBox);
  sLogCapacity_ -> setSuffix(" lines");
  label = new QLabel("Log capacity:", gBox);
  label->setMinimumSize(label->sizeHint());
  sLogCapacity_->setMinimumSize(sLogCapacity_->sizeHint());
  sLogCapacity_->setMaximum(100000);
  sLogCapacity_->setMinimum(100);
  sLogCapacity_->setSingleStep(100);
  sLogCapacity_->setValue(logger->getCapacity());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(sLogCapacity_, 1, 1);

  gridLayout->setColumnStretch( 2, 1);

  cbLogStoreInFile_ = new QCheckBox("Save log to the file", gBox);
  cbLogTimeLabel_ = new QCheckBox("Put time stamps", gBox);
  cbFullDate_ = new QCheckBox("Use full date format", gBox);
  QCheckBox                    *cb=new QCheckBox("Reserved", gBox);
  cb->setEnabled(false);
  //
  cbLogStoreInFile_->setMinimumSize(cbLogStoreInFile_->sizeHint());
  cbLogTimeLabel_->setMinimumSize(cbLogTimeLabel_->sizeHint());
  cbFullDate_->setMinimumSize(cbFullDate_->sizeHint());
  cbLogStoreInFile_->setChecked(logger->getIsStoreInFile());
  cbLogTimeLabel_->setChecked(logger->getIsNeedTimeMark());
  cbFullDate_->setChecked(logger->getUseFullDateFormat());
  gridLayout->addWidget(cbLogStoreInFile_, 0, 3);
  gridLayout->addWidget(cbLogTimeLabel_  , 1, 3);
  gridLayout->addWidget(cb, 0, 4);
  gridLayout->addWidget(cbFullDate_      , 1, 4);

  gridLayout->activate();

  mainLayout->addWidget(gBox);
  mainLayout->addStretch(1);
  //---

  
  gBox = new QGroupBox("Log Levels", this);
  gridLayout = new QGridLayout(gBox);

  label = new QLabel("Er", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 2);
  label = new QLabel("Wn", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 3);
  label = new QLabel("In", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 4);
  label = new QLabel("Db", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 5);
  label = new QLabel("Er", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 8);
  label = new QLabel("Wn", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0, 9);
  label = new QLabel("In", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0,10);
  label = new QLabel("Db", gBox);
  label -> setMinimumSize(label->sizeHint());
  gridLayout -> addWidget(label, 0,11);

  gridLayout -> setRowStretch( 0, 2);
  gridLayout -> setColumnStretch( 0, 2);
  gridLayout -> setColumnStretch( 1, 1);

  for (int i=0; i<16; i++)
  {      
    label= new QLabel(facilityNames[i], gBox);
    label-> setMinimumSize(label->sizeHint());
    gridLayout-> addWidget(label, 1+i, 1);
    for (int j=0; j<4; j++)
    {
      cbLLevel_[j][i] = new QCheckBox(gBox);
      cbLLevel_[j][i]->setMinimumSize(cbLLevel_[j][i]->sizeHint());
//    QToolTip::add(cbLLevel_[j][i], levelNames[j]);
      gridLayout -> addWidget(cbLLevel_[j][i], 1+i, 2+j);
    };
  };
  gridLayout->setColumnStretch( 6, 7);
  gridLayout->setColumnStretch( 7, 1);
  gridLayout->setColumnMinimumWidth( 6, label->fontMetrics().width("WW"));
  for (int i=0; i<16; i++)
  {      
    label = new QLabel(facilityNames[16+i], gBox);
    label->setMinimumSize(label->sizeHint());
    gridLayout->addWidget(label, 1+i, 7);
    for (int j=0; j<4; j++)
    {
      cbLLevel_[j][16+i] = new QCheckBox(gBox);
      cbLLevel_[j][16+i]->setMinimumSize(cbLLevel_[j][16+i]->sizeHint());
//    QToolTip::add(cbLLevel[j][16+i], levelNames[j]);
      gridLayout->addWidget(cbLLevel_[j][16+i], 1+i, 8+j);
    };
  };
  gridLayout->setColumnStretch( 12, 2);
  gridLayout->activate();
  
  mainLayout->addWidget(gBox);
  mainLayout->addStretch(2);

  // set up check boxes:
  unsigned int                  mask[4];
  mask[0] = logger->getLogFacility(SgLogger::ERR);
  mask[1] = logger->getLogFacility(SgLogger::WRN);
  mask[2] = logger->getLogFacility(SgLogger::INF);
  mask[3] = logger->getLogFacility(SgLogger::DBG);
  for (int i=0; i<4; i++)
    for (int j=0; j<32; j++)
      cbLLevel_[i][j]->setChecked(mask[i] & logFacilities[j]);
};



//
void SgGuiLoggerConfig::acquireData()
{
  logger->setFileName(eLogFileName_->text());
  logger->setCapacity(sLogCapacity_->value());
  logger->setIsStoreInFile(cbLogStoreInFile_->isChecked());
  logger->setIsNeedTimeMark(cbLogTimeLabel_->isChecked());
  logger->setUseFullDateFormat(cbFullDate_->isChecked());

  unsigned int                  mask[4]={0,0,0,0};
  for (int i=0; i<4; i++)
    for (int j=0; j<32; j++)
      if (cbLLevel_[i][j]->isChecked())
        mask[i] |= logFacilities[j];
  
  logger->setLogFacility(SgLogger::ERR, mask[0]);
  logger->setLogFacility(SgLogger::WRN, mask[1]);
  logger->setLogFacility(SgLogger::INF, mask[2]);
  logger->setLogFacility(SgLogger::DBG, mask[3]);
};
/*=====================================================================================================*/







/*=====================================================================================================*/
const QString                   facilityNames[32]=
{
  QString("IO: generic binary operations"),             // SgLogger::IO_BIN,
  QString("IO: generic text operations"),               // SgLogger::IO_TXT,
  QString("IO: NetCDF"),                                // SgLogger::IO_NCDF,
  QString("IO: Database handler"),                      // SgLogger::IO_DBH,
  QString("General matrices"),                          // SgLogger::MATRIX,
  QString("3D Matrices"),                               // SgLogger::MATRIX3D,
  QString("Data interpolation"),                        // SgLogger::INTERP,
  QString("Tools: Reserved"),                           // SgLogger::MATH_RES_1,
  QString("Data structures: observation"),              // SgLogger::OBS,
  QString("Data structures: station"),                  // SgLogger::STATION,
  QString("Data structures: source"),                   // SgLogger::SOURCE,
  QString("Data structures: session"),                  // SgLogger::SESSION,
  QString("Reference frames"),                          // SgLogger::REF_FRAME,
  QString("Time conversion, scales"),                   // SgLogger::TIME,
  QString("Ionospheric correction"),                    // SgLogger::IONO,
  QString("Tropospheric refraction"),                   // SgLogger::REFRACTION,
  QString("Calculation of delay"),                      // SgLogger::DELAY,
  QString("Calculation of rate"),                       // SgLogger::RATE,
  QString("Applying flyby info"),                       // SgLogger::FLY_BY,
  QString("Site displacements"),                        // SgLogger::DISLPACEMENT,
  QString("Geo: Reserved"),                             // SgLogger::GEO_RES_1,
  QString("Estimator"),                                 // SgLogger::ESTIMATOR,
  QString("Piecewise linear parameters"),               // SgLogger::PWL,
  QString("Stochastic parameters"),                     // SgLogger::STOCH,
  QString("Config/Setup"),                              // SgLogger::CONFIG,
  QString("Graphical User Interface"),                  // SgLogger::GUI,
  QString("Report generator"),                          // SgLogger::REPORT,
  QString("System resources"),                          // SgLogger::RESOURCE,
  QString("Task manager"),                              // SgLogger::RUN,
  QString("Preliminary processing"),                    // SgLogger::PREPROC,
  QString("Reserved #1"),                               // SgLogger::RESERVED_1,
  QString("Reserved #2")                                // SgLogger::RESERVED_2
};

const int                       numOfFacilities=sizeof(facilityNames)/sizeof(const QString);

const QString                   levelNames[4]=
{
  QString("ERROR"), 
  QString("WARNING"), 
  QString("INFO"), 
  QString("DEBUG")
};

const int                       logFacilities[32]=
{
  SgLogger::IO_BIN,
  SgLogger::IO_TXT,
  SgLogger::IO_NCDF,
  SgLogger::IO_DBH,
  SgLogger::MATRIX,
  SgLogger::MATRIX3D,
  SgLogger::INTERP,
  SgLogger::MATH_RES_1,
  SgLogger::OBS,
  SgLogger::STATION,
  SgLogger::SOURCE,
  SgLogger::SESSION,
  SgLogger::REF_FRAME,
  SgLogger::TIME,
  SgLogger::IONO,
  SgLogger::REFRACTION,
  SgLogger::DELAY,
  SgLogger::RATE,
  SgLogger::FLY_BY,
  SgLogger::DISPLACEMENT,
  SgLogger::GEO_RES_1,
  SgLogger::ESTIMATOR,
  SgLogger::PWL,
  SgLogger::STOCH,
  SgLogger::CONFIG,
  SgLogger::GUI,
  SgLogger::REPORT,
  SgLogger::RESOURCE,
  SgLogger::RUN,
  SgLogger::PREPROC,
  SgLogger::RESERVED_1,
  SgLogger::RESERVED_2
};


/*=====================================================================================================*/
