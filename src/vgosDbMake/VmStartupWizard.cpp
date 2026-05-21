/*
 *
 *    This file is a part of vgosDbMake. vgosDbMake is a part of CALC/SOLVE
 *    system and is designed to convert correlator output data into VgosDb format.
 *    Copyright (C) 2015-2020 Sergei Bolotin.
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

#include "vgosDbMake.h"
#include "VmStartupWizard.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <QtCore/QDir>
#include <QtCore/QSettings>

#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QButtonGroup>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QFrame>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QGridLayout>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QRadioButton>
#   include <QtWidgets/QSpinBox>
#   include <QtWidgets/QVBoxLayout>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QButtonGroup>
#   include <QtGui/QCheckBox>
#   include <QtGui/QFrame>
#   include <QtGui/QGroupBox>
#   include <QtGui/QGridLayout>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#   include <QtGui/QRadioButton>
#   include <QtGui/QSpinBox>
#   include <QtGui/QVBoxLayout>
#endif

#include <QtGui/QPixmap>




#include <SgLogger.h>




//
const int VmStartupWizard::serialNumber_ = 20150527; // 2015/05/27


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
int VmStartupWizard::serialNumber()
{
  return VmStartupWizard::serialNumber_;
};



//
VmStartupWizard::VmStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun,
  QSettings *settings, QWidget *parent)
  : QWizard(parent)
{
  isFirstRun_ = isFirstRun;
  isForcedRun_ = isForcedRun;
  isSystemWideRun_ = isSystemWideRun;
  settings_ = settings;
  addPage(createIntroPage());
  addPage(createPageHomeDir());
  if (!isSystemWideRun_)
    addPage(createPageOne());
  addPage(createPageTwo());
  addPage(createPageThree());
  addPage(createConclusionPage());

  setWizardStyle(ModernStyle);

  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/NVI_logo.png"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_1.png"));


  setWindowTitle(vgosDbMakeVersion.getSoftwareName() + " Startup Wizard" + 
    QString(isSystemWideRun?" (system-wide settings)":""));
};



//
VmStartupWizard::~VmStartupWizard()
{
};



//
VmWizardPage* VmStartupWizard::createIntroPage()
{
  VmWizardPage                 *w=new VmWizardPage();
  QString                       str;

  // title:
  //  w->setTitle("<p><font size=+1 face=\"Times\">Wellcome to <b>&nu;Solve</b>!</font></p>");
  w->setTitle("<p><font size=+1 face=\"Times\">Welcome to vgosDbMake</font></p>");

  // subTitle:
  str = "The Startup Wizard will ask you few questions about software configuration.";
  if (isFirstRun_)
    w->setSubTitle("<font face=\"Times\"><p>Thank you for installing vgosDbMake version " +
                  vgosDbMakeVersion.toString() + ".</p><p>" + str + "</p></font>");
  else if (!isForcedRun_)
    w->setSubTitle( "<font face=\"Times\"><p>Congratulation! You just have upgraded SgLib and "
    "vgosDbMake software to the versions " + libraryVersion.toString() + " and " + 
    vgosDbMakeVersion.toString() + ".</p><p>" + str + "</p></font>");
  else
    w->setSubTitle( "<font face=\"Times\"><p>Startup Wizard welcomes you. "
    "You are using SgLib version " +
    libraryVersion.toString() + " (" + libraryVersion.getCodeName() +
    ") and vgosDbMake version " + vgosDbMakeVersion.toString() + " (" + 
    vgosDbMakeVersion.getCodeName() + ").</p><p>" + str + "</p></font>");

  w->setButtonText(QWizard::NextButton, "Agree");

  QVBoxLayout *layout = new QVBoxLayout;
  w->setLayout(layout);

  // first label:
  str = 
    "<h3>vgosDbMake</h3>"
    "<p>vgosDbMake is software that converts VLBI data in vgosDb format."

    "<p>Copyright (C) 2015-2020 Sergei Bolotin.</p>"

    "<p>This program is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version.</p>"

    "<p>This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.</p>"

    "<p>You should have received a copy of the GNU General Public License "
    "along with this program. If not, see http://www.gnu.org/licenses.</p>"
  
    "<p>Please, read carefully the license agreement and if you agree, press \"Agree\" button.</p>"
    ""
    "";
  QLabel                       *label=new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addStretch(1);

  QCheckBox                    *cb=new QCheckBox("I have read and accept the conditions.", w);
  w->registerField("warrantyAccepted*", cb);
  layout->addWidget(cb);
  layout->addSpacing(40);
  
  return w;
};



//
VmWizardPage *VmStartupWizard::createPageHomeDir()
{
  VmWizardPage                 *w=new VmWizardPage();
  QString                       path2Home;
  bool                          have2ForceUser(false);

  QString                       str;
  QLabel                       *label;
  QVBoxLayout                  *layout=new QVBoxLayout;
  QGroupBox                     *gBox;
  QGridLayout                   *gridLayout;
  w->setLayout(layout);


  w->setTitle("<p><font size=+1 face=\"Times\">Essential directories of vgosDbMake</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>The working directory of the software.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_2.png"));

  if (isSystemWideRun_)
    str = 
      "<h3>Default Directories</h3>"
      "<p>Please, provide paths to data on your system.</p>"
      "<p>Users will be able to overwrite default settings.</p>"
      "";
  else
    str = 
      "<h3>vgosDbMake Home Directory</h3>"
      "<p>Please, specify the software's home directory.</p>"
      "<p>All paths that are not absolute will be counted form this directory. The software home "
      "directory have to be writable for a user and should be different from user's home directory.</p>"
      "";
  label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addSpacing(20);

  if (!isSystemWideRun_)
  {
    path2Home = setup.getPath2Home();
    if (isFirstRun_ || setup.getPath2Home()=="")
      path2Home = QDir::homePath() + "/" + vgosDbMakeVersion.getSoftwareName();
    if (path2Home == QDir::homePath()) // cannot be user's home
      have2ForceUser = true;

    gBox = new QGroupBox("vgosDbMake home directory", w);
    gridLayout = new QGridLayout(gBox);
    label = new QLabel("Path to the software home directory:", gBox);
    label->setMinimumSize(label->sizeHint());
    leHomeDirName_ = new QLineEdit(gBox);
    leHomeDirName_->setText(path2Home);
    leHomeDirName_->setMinimumSize(leHomeDirName_->sizeHint());
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(leHomeDirName_, 0, 1);

    if (have2ForceUser)
      w->registerField("path2Home*",   leHomeDirName_);
    layout->addWidget(gBox);
    layout->addSpacing(20);
  };

  gBox = new QGroupBox("Essential directories", w);
  gridLayout = new QGridLayout(gBox);

  label = new QLabel("Path to mark{3,4} files (input):", gBox);
  label->setMinimumSize(label->sizeHint());
  leInputDirName_ = new QLineEdit(gBox);
  leInputDirName_->setText(setup.getPath2InputFiles());
  leInputDirName_->setMinimumSize(leInputDirName_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leInputDirName_, 0, 1);

  label = new QLabel("Path to vgosDb files (output):", gBox);
  label->setMinimumSize(label->sizeHint());
  leVgosDbDirName_ = new QLineEdit(gBox);
  leVgosDbDirName_->setText(setup.getPath2VgosDbFiles());
  leVgosDbDirName_->setMinimumSize(leVgosDbDirName_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(leVgosDbDirName_, 1, 1);

  label = new QLabel("Path to Master files:", gBox);
  label->setMinimumSize(label->sizeHint());
  leMasterFilesDirName_ = new QLineEdit(gBox);
  leMasterFilesDirName_->setText(setup.getPath2MasterFiles());
  leMasterFilesDirName_->setMinimumSize(leMasterFilesDirName_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(leMasterFilesDirName_, 2, 1);

  //
  // compose a string:
  cbUseAltMasterfileSuffixes_ = new QCheckBox("Use alternative masterfile extensions:", gBox);
  cbUseAltMasterfileSuffixes_->setMinimumSize(cbUseAltMasterfileSuffixes_->sizeHint());
  cbUseAltMasterfileSuffixes_->setCheckState(setup.getUseAltMasterfileSuffixes()?Qt::Checked:Qt::Unchecked);
  //
  leAltMasterfileSuffixes_ = new QLineEdit(gBox);
  leAltMasterfileSuffixes_->setText(setup.getMasterfileSuffixes().join(","));
  leAltMasterfileSuffixes_->setMinimumSize(leAltMasterfileSuffixes_->sizeHint());
  gridLayout->addWidget(cbUseAltMasterfileSuffixes_,  3, 0);
  gridLayout->addWidget(leAltMasterfileSuffixes_,     3, 1);
  //
  leAltMasterfileSuffixes_->setEnabled(setup.getUseAltMasterfileSuffixes());
  connect(cbUseAltMasterfileSuffixes_, SIGNAL(stateChanged(int)), SLOT(changeAltMasterfileSuffixes(int)));


  layout->addWidget(gBox);


  // general options:
  gBox = new QGroupBox("General options", w);
  QVBoxLayout                  *aLayout=new QVBoxLayout(gBox);
  cbUseLocalLocale_ = new QCheckBox("Do not alternate locale set up", gBox);
  cbUseLocalLocale_->setMinimumSize(cbUseLocalLocale_->sizeHint());
  cbUseLocalLocale_->setChecked(setup.getUseLocalLocale());
  aLayout -> addWidget(cbUseLocalLocale_);


  layout->addWidget(gBox);

  return w;
};



//
VmWizardPage *VmStartupWizard::createPageOne()
{
  VmWizardPage                  *w=new VmWizardPage();
  QString                       userName(""), userLoginName(""), userEMail(""), userInitials("");
  QString                       hostName(""), domainName("");
  bool                          isDUN(false);
  SgIdentities                  ids;
  if (!isFirstRun_ && 
        setup.identities().getUserName()==ids.getUserName() &&
        setup.identities().getUserEmailAddress()==ids.getUserEmailAddress())
    isDUN = true;

  if (isFirstRun_ || isDUN)
  {
    // guess user login name:
    uid_t                       uid=geteuid();
    struct passwd              *pw=getpwuid(uid);
    if (!pw)
      perror("getpwuid: ");
    else
    {
      char                      buff[256];
      userLoginName = pw->pw_name;
      userName = pw->pw_gecos;
      if (userName.contains(","))
        userName = userName.left(userName.indexOf(","));
      
      // guess host name:
      if (gethostname(buff, sizeof(buff))==-1)
      {
        perror("gethostname: ");
        userEMail = userName + "@localhost";
      }
      else
      {
        hostName = buff;
        // If hostname contains at least one dot, assume this is F.Q.D.N. host name
        // and do not ask about the domain name:
        //
        if (!hostName.contains("."))
        {
          // guess domain name:
          if (getdomainname(buff, sizeof(buff))==-1)
            perror("getdomainname: ");
          else
          {
            domainName = buff;
            if (!domainName.contains("(none)") && domainName.simplified().size()>0)
              hostName += "." + domainName;
          };
        };
        userEMail = userLoginName + "@" + hostName;
      };
      if (userName.contains(" "))
      {
        userInitials = userName.mid(0, 1) + userName.mid(userName.indexOf(" ") + 1, 1);
      }
      else if (userName.size()>=2)
        userInitials = userName.left(2);
      else
        userInitials = "??";
    };
  }
  else
  {
    userName = setup.identities().getUserName();
    userEMail = setup.identities().getUserEmailAddress();
    userInitials = setup.identities().getUserDefaultInitials();
  };

  w->setTitle("<p><font size=+1 face=\"Times\">User identities</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>Please, specify your name and e-mail address.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_3.png"));

  QString                       str;
  QLabel                       *label;
  QVBoxLayout                  *layout=new QVBoxLayout;
  w->setLayout(layout);
  //  layout->addStretch(1);

  str = 
    "<h3>User Identities</h3>"
    "<p>Describe yourself, your name, e-mail, etc.</p>"
    "<p>[Plus something else, more polite...]</p>"
    "";
  label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addSpacing(20);

  QGroupBox                    *gBox=new QGroupBox("User Identities", w);
  QGridLayout                  *gridLayout=new QGridLayout(gBox);

  label = new QLabel("User Name:", gBox);
  label->setMinimumSize(label->sizeHint());
  leUserName_ = new QLineEdit(gBox);
  leUserName_->setText(userName);
  leUserName_->setMinimumSize(leUserName_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leUserName_, 0, 1);
  //
  label = new QLabel("E-mail address:", gBox);
  label->setMinimumSize(label->sizeHint());
  leUserEmail_ = new QLineEdit(gBox);
  leUserEmail_->setText(userEMail);
  leUserEmail_->setMinimumSize(leUserEmail_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(leUserEmail_, 1, 1);
  //
  label = new QLabel("User's default initials:", gBox);
  label->setMinimumSize(label->sizeHint());
  leUserInitials_ = new QLineEdit(gBox);
  leUserInitials_->setText(userInitials);
  leUserInitials_->setMinimumSize(leUserInitials_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(leUserInitials_, 2, 1);
  
  layout->addWidget(gBox);
  return w;
};



//
VmWizardPage *VmStartupWizard::createPageTwo()
{
  VmWizardPage                 *w=new VmWizardPage();
  QString                       acFullName, acAbbrevName, acAbbName;
  SgIdentities                  ids;
  bool                          have2ForceUser(false);

  acFullName = setup.identities().getAcFullName();
  acAbbrevName = setup.identities().getAcAbbrevName();
  acAbbName = setup.identities().getAcAbbName();
  
  if (isFirstRun_ || setup.identities().getAcFullName()==ids.getAcFullName())
  {
    acFullName = "";
    acAbbrevName = "";
    acAbbName = "";
    have2ForceUser = true;
  };
  
  w->setTitle("<p><font size=+1 face=\"Times\">Analysis Center identities</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>Please, describe organization where you work.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_4.png"));

  QString                       str;
  QLabel                       *label;
  QVBoxLayout                  *layout=new QVBoxLayout;
  w->setLayout(layout);
  //  layout->addStretch(1);

  str = 
    "<h3>Analysis Center</h3>"
    "<p>Please, specify the analysis center.</p>"
    "<p>[Plus something else, more polite...]</p>"
    "";
  label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addSpacing(20);

  QGroupBox                    *gBox=new QGroupBox("Analysis Center", w);
  QGridLayout                  *gridLayout=new QGridLayout(gBox);
  //  int                           l;
  label = new QLabel("Full Name:", gBox);
  label->setMinimumSize(label->sizeHint());
  leAcFullName_ = new QLineEdit(gBox);
  leAcFullName_->setText(acFullName);
  leAcFullName_->setMinimumSize(leAcFullName_->sizeHint());
  // AC names could be long:
  //  leAcFullName_->setMinimumWidth(leAcFullName_->fontMetrics().width(acFullName) + 20);
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leAcFullName_, 0, 1);
  //
  label = new QLabel("Abbreviation:", gBox);
  label->setMinimumSize(label->sizeHint());
  leAcAbbrevName_ = new QLineEdit(gBox);
  leAcAbbrevName_->setText(acAbbrevName);
  leAcAbbrevName_->setMinimumSize(leAcAbbrevName_->sizeHint());
  gridLayout->addWidget(label, 1, 0);
  gridLayout->addWidget(leAcAbbrevName_, 1, 1);
  //
  label = new QLabel("Abbrev.:", gBox);
  label->setMinimumSize(label->sizeHint());
  leAcAbbName_ = new QLineEdit(gBox);
  leAcAbbName_->setText(acAbbName);
  leAcAbbName_->setMinimumSize(leAcAbbName_->sizeHint());
  gridLayout->addWidget(label, 2, 0);
  gridLayout->addWidget(leAcAbbName_, 2, 1);

  if (have2ForceUser)
  {
    w->registerField("acFullName*",   leAcFullName_);
    w->registerField("acAbbrevName*", leAcAbbrevName_);
//  w->registerField("acAbbName*",    leAcAbbName_);
  };

  layout->addWidget(gBox);
  return w;
};



//
VmWizardPage *VmStartupWizard::createPageThree()
{
  VmWizardPage                 *w=new VmWizardPage();
  const QString                 logLevelNames[4] = {"Error", "Warning", "Info", "Debug"};
  int                           logLevel(settings_->value("Logger/LogLevel", 2).toInt());
  
  w->setTitle("<p><font size=+1 face=\"Times\">Logger </font></p>");
  w->setSubTitle("<font face=\"Times\"><p>Set up configuration of the logging subsystem.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_5.png"));

  QString                       str;
  QLabel                       *label;
  QVBoxLayout                  *layout=new QVBoxLayout;
  w->setLayout(layout);

  str = 
    "<h3>Logger</h3>"
    "<p>Change parameters of the logging subsystem.</p>"
    "";
  label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addSpacing(20);


  QGroupBox                    *gBox;
  QGridLayout                  *gridLayout;

  gBox = new QGroupBox("Main log options", w);
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
  cbLogStoreInFile_->setMinimumSize(cbLogStoreInFile_->sizeHint());
  cbLogTimeLabel_->setMinimumSize(cbLogTimeLabel_->sizeHint());
  cbLogStoreInFile_->setChecked(logger->getIsStoreInFile());
  cbLogTimeLabel_->setChecked(logger->getIsNeedTimeMark());
  gridLayout->addWidget(cbLogStoreInFile_, 0, 3);
  gridLayout->addWidget(cbLogTimeLabel_  , 1, 3);

  gridLayout->activate();
  layout->addWidget(gBox);


  gBox = new QGroupBox("Main log level", w);
  bgLogLevels_ = new QButtonGroup(gBox);
  QVBoxLayout                  *aLayout=new QVBoxLayout(gBox);
  QRadioButton                 *rbLogLevel[4];
  for (int i=0; i<4; i++)
  {
    rbLogLevel[i] = new QRadioButton(logLevelNames[i], gBox);
    rbLogLevel[i]-> setMinimumSize(rbLogLevel[i]->sizeHint());
    bgLogLevels_->addButton(rbLogLevel[i], i);
    aLayout->addWidget(rbLogLevel[i]);
  };
  rbLogLevel[logLevel]->setChecked(true);
  layout->addWidget(gBox);

  // ----
  //
  gBox = new QGroupBox("Aux log options", w);
  gridLayout = new QGridLayout(gBox);

  leAuxLogsDirName_ = new QLineEdit(gBox);
  leAuxLogsDirName_->setText(setup.getPath2AuxLogs());
  label = new QLabel("Path to logs for each session:", gBox);
  label->setMinimumSize(label->sizeHint());
  leAuxLogsDirName_->setMinimumSize(leAuxLogsDirName_->sizeHint());
  gridLayout->addWidget(label, 0, 0);
  gridLayout->addWidget(leAuxLogsDirName_, 0, 1);

  cbMakeAuxLog_ = new QCheckBox("Save log file for each session", gBox);
  cbMakeAuxLog_->setMinimumSize(cbMakeAuxLog_->sizeHint());
  cbMakeAuxLog_->setChecked(setup.getHave2SavePerSessionLog());
  gridLayout->addWidget(cbMakeAuxLog_, 1, 0, 1, 2);

  gridLayout->activate();
  layout->addWidget(gBox);

  return w;
};



//
VmWizardPage *VmStartupWizard::createConclusionPage()
{
  VmWizardPage                 *w=new VmWizardPage();
  QString                       str;

  w->setTitle("<p><font size=+1 face=\"Times\">Ready to run</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>We are done.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_6.png"));

  QVBoxLayout                  *layout = new QVBoxLayout;
  w->setLayout(layout);

  // first label:
  str = 
    "<h3>vgosDbMake is ready to run</h3>"
    "<p>Good luck.</p>"
    "";
  QLabel                       *label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);

  return w;
};



//
void VmStartupWizard::accept()
{
  QString                       str;
  QStringList                   lst;
  //
  if (!isSystemWideRun_)
  {
    setup.identities().setUserName(leUserName_->text());
    setup.identities().setUserEmailAddress(leUserEmail_->text());
    setup.identities().setUserDefaultInitials(leUserInitials_->text());
    setup.setPath2Home(leHomeDirName_->text());
  };
  setup.identities().setAcFullName(leAcFullName_->text());
  setup.identities().setAcAbbrevName(leAcAbbrevName_->text());
  setup.identities().setAcAbbName(leAcAbbName_->text());
  setup.setPath2VgosDbFiles(leVgosDbDirName_->text());
  setup.setPath2InputFiles(leInputDirName_->text());
  setup.setPath2MasterFiles(leMasterFilesDirName_->text());
  
  //
  setup.setUseAltMasterfileSuffixes(cbUseAltMasterfileSuffixes_->isChecked());
  //
  str = leAltMasterfileSuffixes_->text();
  if (str.size())
  {
    lst = str.split(QRegularExpression("[,;:]"), QString::SkipEmptyParts);
    if (lst.size())
      setup.setMasterfileSuffixes(lst);
    else
      logger->write(SgLogger::DBG, SgLogger::IO, className() +
        "::accept(): empty list for masterfile extensions, the string: \"" + str + "\"");
  };
  //

  setup.setPath2AuxLogs(leAuxLogsDirName_->text());
  setup.setHave2SavePerSessionLog(cbMakeAuxLog_->isChecked());
  setup.setUseLocalLocale(cbUseLocalLocale_->isChecked());

  //
  logger->setFileName(eLogFileName_->text());
  logger->setCapacity(sLogCapacity_->value());
  logger->setIsStoreInFile(cbLogStoreInFile_->isChecked());
  logger->setIsNeedTimeMark(cbLogTimeLabel_->isChecked());

  int                           logLevel=bgLogLevels_->checkedId();
  if (logLevel != -1)
  {
    settings_->setValue("Logger/LogLevel", logLevel);
    logger->setLogFacility(SgLogger::ERR, logLevel>=0?0xFFFFFFFF:0);
    logger->setLogFacility(SgLogger::WRN, logLevel>=1?0xFFFFFFFF:0);
    logger->setLogFacility(SgLogger::INF, logLevel>=2?0xFFFFFFFF:0);
    logger->setLogFacility(SgLogger::DBG, logLevel==3?0xFFFFFFFF:0);
  };
  //
  // check home dir:
  if (!isSystemWideRun_)
  {
    QDir                          dir;
    if (!dir.exists(setup.getPath2Home()))
    {
      if (!dir.mkpath(setup.getPath2Home()))
        QMessageBox::warning(this, "Warning", "Cannot create Home directory " + 
          setup.getPath2Home());
      else
        logger->write(SgLogger::DBG, SgLogger::IO, className() + 
          ": the directory " + setup.getPath2Home() + " has been created");
    };
  };
  //
  QWizard::accept();
};



//
void VmStartupWizard::changeAltMasterfileSuffixes(int chkState)
{
  bool                          have2=chkState==Qt::Checked;
  setup.setUseAltMasterfileSuffixes(have2);
  leAltMasterfileSuffixes_->setEnabled(have2);
};
/*=====================================================================================================*/
