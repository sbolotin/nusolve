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
#include "NsStartupWizard.h"

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <QtCore/QDir>



#if QT_VERSION >= 0x050000
#   include <QtWidgets/QBoxLayout>
#   include <QtWidgets/QCheckBox>
#   include <QtWidgets/QFrame>
#   include <QtWidgets/QGroupBox>
#   include <QtWidgets/QGridLayout>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QLineEdit>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QPushButton>
#   include <QtWidgets/QVBoxLayout>
#else
#   include <QtGui/QBoxLayout>
#   include <QtGui/QCheckBox>
#   include <QtGui/QFrame>
#   include <QtGui/QGroupBox>
#   include <QtGui/QGridLayout>
#   include <QtGui/QLabel>
#   include <QtGui/QLineEdit>
#   include <QtGui/QMessageBox>
#   include <QtGui/QPushButton>
#   include <QtGui/QVBoxLayout>
#endif

#include <QtGui/QPixmap>



#include <SgLogger.h>






//
const int NsStartupWizard::serialNumber_ = 20170412; // 2017/04/12
//
/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
int NsStartupWizard::serialNumber()
{
  return NsStartupWizard::serialNumber_;
};



//
NsStartupWizard::NsStartupWizard(bool isFirstRun, bool isForcedRun, bool isSystemWideRun, 
  QWidget *parent) 
  : QWizard(parent)
{
  isFirstRun_ = isFirstRun;
  isForcedRun_ = isForcedRun;
  isSystemWideRun_ = isSystemWideRun;
  
  addPage(createIntroPage());
  addPage(createPageHomeDir());
  if (!isSystemWideRun_)
    addPage(createPageOne());
  addPage(createPageTwo());
  addPage(createConclusionPage());

  setWizardStyle(ModernStyle);
  // setOption(QWizard::ExtendedWatermarkPixmap);

  setPixmap(QWizard::LogoPixmap, QPixmap(":/images/NVI_logo.png"));
  setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_1.png"));

  setWindowTitle(tr("Startup Wizard"));
};



//
NsStartupWizard::~NsStartupWizard()
{
};



//
NsWizardPage* NsStartupWizard::createIntroPage()
{
  NsWizardPage                 *w=new NsWizardPage();
  QString                       str;

  // title:
  //  w->setTitle("<p><font size=+1 face=\"Times\">Wellcome to <b>&nu;Solve</b>!</font></p>");
  w->setTitle("<p><font size=+1 face=\"Times\">Welcome to &nu;Solve</font></p>");

  // subTitle:
  str = "The Startup Wizard will ask you few questions about software configuration.";
  if (isFirstRun_)
    w->setSubTitle("<font face=\"Times\"><p>Thank you for installing &nu;Solve version " +
                  nuSolveVersion.toString() + ".</p><p>" + str + "</p></font>");
  else if (!isForcedRun_)
    w->setSubTitle( "<font face=\"Times\"><p>Congratulation! You just have upgraded SgLib and &nu;Solve "
      "software to the versions " + libraryVersion.toString() + " and " + nuSolveVersion.toString() + 
      ".</p><p>" + str + "</p></font>");
  else
    w->setSubTitle( "<font face=\"Times\"><p>We are appreciate your loyalty to Startup Wizard. "
    "We can run it as many times as you want. Currently, you are using SgLib version " +
    libraryVersion.toString() + " (" + libraryVersion.getCodeName() +
    ") and &nu;Solve version " + nuSolveVersion.toString() + " (" + 
    nuSolveVersion.getCodeName() + ").</p><p>" + str + "</p></font>");

  w->setButtonText(QWizard::NextButton, "Agree");

  QVBoxLayout *layout = new QVBoxLayout;
  w->setLayout(layout);

  // first label:
  str = 
    "<h3>&nu;Solve</h3>"
    "<p>&nu;Solve is software for VLBI data processing. "

    "<p>Copyright (C) 2010-2020 Sergei Bolotin.</p>"

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
NsWizardPage *NsStartupWizard::createPageOne()
{
  NsWizardPage                  *w=new NsWizardPage();
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
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_2.png"));

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
  //  layout->addStretch(1);
  return w;
};



//
NsWizardPage *NsStartupWizard::createPageTwo()
{
  NsWizardPage                 *w=new NsWizardPage();
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
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_6.png"));

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
  //  layout->addStretch(1);
  return w;
};



//
NsWizardPage *NsStartupWizard::createPageHomeDir()
{
  NsWizardPage                 *w=new NsWizardPage();
  QString                       path2Home;
  bool                          have2ForceUser(false);

  QString                       str;
  QLabel                       *label;
  QVBoxLayout                  *layout=new QVBoxLayout;
  QGroupBox                     *gBox;
  QGridLayout                   *gridLayout;
  w->setLayout(layout);

  w->setTitle("<p><font size=+1 face=\"Times\">Essential directories of &nu;Solve</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>The working directory of the software.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_5.png"));

  if (isSystemWideRun_)
    str = 
      "<h3>Default Directories</h3>"
      "<p>Please, provide paths to data on your system.</p>"
      "<p>Users will be able to overwrite default settings.</p>"
      "";
  else
    str = 
      "<h3>&nu;Solve Home Directory</h3>"
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
      path2Home = QDir::homePath() + "/" + nuSolveVersion.getSoftwareName();
    if (path2Home == QDir::homePath()) // cannot be user's home
      have2ForceUser = true;
  
    gBox = new QGroupBox("Directories", w);
    gridLayout = new QGridLayout(gBox);
    //
    label = new QLabel("Path to the software home directory:", gBox);
    label->setMinimumSize(label->sizeHint());
    leHomeDirName_ = new QLineEdit(gBox);
    leHomeDirName_->setText(path2Home);
    leHomeDirName_->setMinimumSize(leHomeDirName_->sizeHint());
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(leHomeDirName_, 0, 1);
    //
    label = new QLabel("Path to logs for each session:", gBox);
    label->setMinimumSize(label->sizeHint());
    leAuxLogsDirName_ = new QLineEdit(gBox);
    leAuxLogsDirName_->setText(setup.getPath2AuxLogs());
    leAuxLogsDirName_->setMinimumSize(leAuxLogsDirName_->sizeHint());
    gridLayout->addWidget(label, 1, 0);
    gridLayout->addWidget(leAuxLogsDirName_, 1, 1);

    cbMakeAuxLog_ = new QCheckBox("Save a log file for each session (command line mode only)", gBox);
    cbMakeAuxLog_->setMinimumSize(cbMakeAuxLog_->sizeHint());
    cbMakeAuxLog_->setChecked(setup.getHave2SavePerSessionLog());
    gridLayout->addWidget(cbMakeAuxLog_, 2, 0, 1, 2);
 
    layout->addWidget(gBox);
    //
    if (have2ForceUser)
      w->registerField("path2Home*",   leHomeDirName_);
  }
  else
  {
    gBox = new QGroupBox("Essential paths", w);
    gridLayout = new QGridLayout(gBox);
    //
    label = new QLabel("Executables of Catalog<->nuSolve Interface:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2CatNu_ = new QLineEdit(gBox);
    lePath2CatNu_->setText(setup.getPath2CatNuInterfaceExec());
    lePath2CatNu_->setMinimumSize(lePath2CatNu_->sizeHint());
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(lePath2CatNu_, 0, 1);

    label = new QLabel("Observations (DBH) files:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2Dbh_ = new QLineEdit(gBox);
    lePath2Dbh_->setText(setup.getPath2DbhFiles());
    lePath2Dbh_->setMinimumSize(lePath2Dbh_->sizeHint());
    gridLayout->addWidget(label, 1, 0);
    gridLayout->addWidget(lePath2Dbh_, 1, 1);
    //
    label = new QLabel("Observations (vgosDb) files:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2VgosDb_ = new QLineEdit(gBox);
    lePath2VgosDb_->setText(setup.getPath2VgosDbFiles());
    lePath2VgosDb_->setMinimumSize(lePath2VgosDb_->sizeHint());
    gridLayout->addWidget(label, 2, 0);
    gridLayout->addWidget(lePath2VgosDb_, 2, 1);
    //
    label = new QLabel("Observations (vgosDa) files:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2VgosDa_ = new QLineEdit(gBox);
    lePath2VgosDa_->setText(setup.getPath2VgosDaFiles());
    lePath2VgosDa_->setMinimumSize(lePath2VgosDa_->sizeHint());
    gridLayout->addWidget(label, 3, 0);
    gridLayout->addWidget(lePath2VgosDa_, 3, 1);
    //
    label = new QLabel("A priori files:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2Apriori_ = new QLineEdit(gBox);
    lePath2Apriori_->setText(setup.getPath2APrioriFiles());
    lePath2Apriori_->setMinimumSize(lePath2Apriori_->sizeHint());
    gridLayout->addWidget(label, 4, 0);
    gridLayout->addWidget(lePath2Apriori_, 4, 1);
    //
    label = new QLabel("Masterfiles:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2Masterfiles_ = new QLineEdit(gBox);
    lePath2Masterfiles_->setText(setup.getPath2MasterFiles());
    lePath2Masterfiles_->setMinimumSize(lePath2Masterfiles_->sizeHint());
    gridLayout->addWidget(label, 5, 0);
    gridLayout->addWidget(lePath2Masterfiles_, 5, 1);

    layout->addWidget(gBox);
    //
    gBox = new QGroupBox("Miscellaneous output", w);
    gridLayout = new QGridLayout(gBox);
    //
    label = new QLabel("Spoolfile output:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2Spoolfiles_ = new QLineEdit(gBox);
    lePath2Spoolfiles_->setText(setup.getPath2SpoolFileOutput());
    lePath2Spoolfiles_->setMinimumSize(lePath2Spoolfiles_->sizeHint());
    gridLayout->addWidget(label, 0, 0);
    gridLayout->addWidget(lePath2Spoolfiles_, 0, 1);
    //
    label = new QLabel("List of not used observations output:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2NotUsedObs_ = new QLineEdit(gBox);
    lePath2NotUsedObs_->setText(setup.getPath2NotUsedObsFileOutput());
    lePath2NotUsedObs_->setMinimumSize(lePath2NotUsedObs_->sizeHint());
    gridLayout->addWidget(label, 1, 0);
    gridLayout->addWidget(lePath2NotUsedObs_, 1, 1);
    //
    label = new QLabel("Reports output:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2Reports_ = new QLineEdit(gBox);
    lePath2Reports_->setText(setup.getPath2ReportOutput());
    lePath2Reports_->setMinimumSize(lePath2Reports_->sizeHint());
    gridLayout->addWidget(label, 2, 0);
    gridLayout->addWidget(lePath2Reports_, 2, 1);
    //
    label = new QLabel("Save observations in NGS format:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2NgsOutput_ = new QLineEdit(gBox);
    lePath2NgsOutput_->setText(setup.getPath2NgsOutput());
    lePath2NgsOutput_->setMinimumSize(lePath2NgsOutput_->sizeHint());
    gridLayout->addWidget(label, 3, 0);
    gridLayout->addWidget(lePath2NgsOutput_, 3, 1);
    //
    label = new QLabel("Plotter output (PS, PDF or ASCII files):", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2PlotterOutput_ = new QLineEdit(gBox);
    lePath2PlotterOutput_->setText(setup.getPath2PlotterOutput());
    lePath2PlotterOutput_->setMinimumSize(lePath2PlotterOutput_->sizeHint());
    gridLayout->addWidget(label, 4, 0);
    gridLayout->addWidget(lePath2PlotterOutput_, 4, 1);
    //
    label = new QLabel("State and intermediate results:", gBox);
    label->setMinimumSize(label->sizeHint());
    lePath2IntermediateResults_ = new QLineEdit(gBox);
    lePath2IntermediateResults_->setText(setup.getPath2IntermediateResults());
    lePath2IntermediateResults_->setMinimumSize(lePath2IntermediateResults_->sizeHint());
    gridLayout->addWidget(label, 5, 0);
    gridLayout->addWidget(lePath2IntermediateResults_, 5, 1);
    //
    label = new QLabel("Path to the auxiliary logs:", gBox);
    label->setMinimumSize(label->sizeHint());
    leAuxLogsDirName_ = new QLineEdit(gBox);
    leAuxLogsDirName_->setText(setup.getPath2AuxLogs());
    leAuxLogsDirName_->setMinimumSize(leAuxLogsDirName_->sizeHint());
    gridLayout->addWidget(label, 6, 0);
    gridLayout->addWidget(leAuxLogsDirName_, 6, 1);

    layout->addWidget(gBox);
  };
  return w;
};



//
NsWizardPage *NsStartupWizard::createConclusionPage()
{
  NsWizardPage                 *w=new NsWizardPage();
  QString                       str;

  w->setTitle("<p><font size=+1 face=\"Times\">Ready to run</font></p>");
  w->setSubTitle("<font face=\"Times\"><p>We are done.</p></font>");
  w->setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/pattern_3.png"));

  QVBoxLayout                  *layout = new QVBoxLayout;
  w->setLayout(layout);

  // first label:
  str = 
    "<h3>&nu;Solve is ready to run</h3>"
    "<p>Have a happy &nu;Solving!</p>"
    "";
  QLabel                       *label = new QLabel("<font face=\"Times\">" + str + "</font>");
  label->setWordWrap(true);
  layout->addWidget(label);

  return w;
};



void NsStartupWizard::accept()
{
  if (!isSystemWideRun_)
  {
    setup.identities().setUserName(leUserName_->text());
    setup.identities().setUserEmailAddress(leUserEmail_->text());
    setup.identities().setUserDefaultInitials(leUserInitials_->text());
    setup.setPath2Home(leHomeDirName_->text());
    setup.setHave2SavePerSessionLog(cbMakeAuxLog_->isChecked());
  };
  setup.identities().setAcFullName(leAcFullName_->text());
  setup.identities().setAcAbbrevName(leAcAbbrevName_->text());
  setup.identities().setAcAbbName(leAcAbbName_->text());

  if (isSystemWideRun_)
  {
    setup.setPath2AuxLogs(leAuxLogsDirName_->text());
    setup.setPath2CatNuInterfaceExec(lePath2CatNu_->text());
    setup.setPath2DbhFiles(lePath2Dbh_->text());
    setup.setPath2VgosDbFiles(lePath2VgosDb_->text());
    setup.setPath2VgosDaFiles(lePath2VgosDa_->text());
    setup.setPath2APrioriFiles(lePath2Apriori_->text());
    setup.setPath2MasterFiles(lePath2Masterfiles_->text());
    setup.setPath2SpoolFileOutput(lePath2Spoolfiles_->text());
    setup.setPath2NotUsedObsFileOutput(lePath2NotUsedObs_->text());
    setup.setPath2ReportOutput(lePath2Reports_->text());
    setup.setPath2NgsOutput(lePath2NgsOutput_->text());
    setup.setPath2PlotterOutput(lePath2PlotterOutput_->text());
    setup.setPath2IntermediateResults(lePath2IntermediateResults_->text());
  };
  //
  // check home dir:
  if (!isSystemWideRun_)
  {
    QDir                          dir(setup.getPath2Home());
    if (!dir.exists())
    {
//    if (!dir.mkpath(setup.getPath2Home()))
      if (!dir.mkpath(dir.absolutePath()))
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
/*=====================================================================================================*/
