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
#include "NsMainWindow.h"

#include <iostream>
#include <stdlib.h>


#include <QtCore/QSettings>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QAction>
#   include <QtWidgets/QApplication>
#   include <QtWidgets/QFileDialog>
#   include <QtWidgets/QFontDialog>
#   include <QtWidgets/QLabel>
#   include <QtWidgets/QMenu>
#   include <QtWidgets/QMenuBar>
#   include <QtWidgets/QMessageBox>
#   include <QtWidgets/QStyle>
#   include <QtWidgets/QStyleFactory>
#else
#   include <QtGui/QAction>
#   include <QtGui/QApplication>
#   include <QtGui/QFileDialog>
#   include <QtGui/QFontDialog>
#   include <QtGui/QLabel>
#   include <QtGui/QMenu>
#   include <QtGui/QMenuBar>
#   include <QtGui/QMessageBox>
#   include <QtGui/QStyle>
#   include <QtGui/QStyleFactory>
#endif

#   include <QtGui/QFont>


#include <SgGuiLogger.h>
#include <SgGuiTaskConfig.h>
#include <SgMJD.h>
#include <SgVersion.h>
#include <SgVlbiStationInfo.h>


#include "NsSetupDialog.h"
#include "NsTestDialog.h"
#include "NsTestFour1Dialog.h"
#include "NsSessionNameDialog.h"
#include "NsSessionEditDialog.h"




NsMainWindow *mainWindow;


/*=======================================================================================================
*
*                           METHODS:
* 
*======================================================================================================*/
NsMainWindow::NsMainWindow(QWidget *parent, Qt::WindowFlags flags) 
  : QMainWindow(parent, flags) 
{
  QString                       windowTitle("");
  styleKeys_ = QStyleFactory::keys();

  createActions();
  createMenus();
  createStatusBar();

  
  logger = new SgGuiLogger(this, 800, true, "nuSolve.log");
  // load logger's config:
  QSettings settings;
  
  if (settings.contains("Version/LibraryName"))
  {
    logger->setLogFacility(SgLogger::ERR, 
      settings.value("Logger/FacilityERR",
      logger->getLogFacility(SgLogger::ERR)).toUInt());
    logger->setLogFacility(SgLogger::WRN, 
      settings.value("Logger/FacilityWRN",
      logger->getLogFacility(SgLogger::WRN)).toUInt());
    logger->setLogFacility(SgLogger::INF, 
      settings.value("Logger/FacilityINF",
      logger->getLogFacility(SgLogger::INF)).toUInt());
    logger->setLogFacility(SgLogger::DBG, 
      settings.value("Logger/FacilityDBG",
      logger->getLogFacility(SgLogger::DBG)).toUInt());
  };
  logger->setFileName(
    settings.value("Logger/FileName", 
    logger->getFileName()).toString());
  logger->setDirName(setup.getPath2Home());
  logger->setCapacity(
    settings.value("Logger/Capacity", 
    logger->getCapacity()).toInt());
  logger->setIsStoreInFile(
    settings.value("Logger/IsStoreInFile", 
    logger->getIsStoreInFile()).toBool());
  logger->setIsNeedTimeMark(
    settings.value("Logger/IsNeedTimeMark", 
    logger->getIsNeedTimeMark()).toBool());
  logger->setUseFullDateFormat(
    settings.value("Logger/UseFullDate", 
    logger->getUseFullDateFormat()).toBool());
  
 
  //
  setCentralWidget((SgGuiLogger*)logger);
  windowTitle = setup.identities().getUserName() + " (" + setup.identities().getAcFullName() + 
    ") running " + nuSolveVersion.getSoftwareName();
  if (config.getName().size() > 0)
    windowTitle += " with alt config=\"" + config.getName() + "\"";
  setWindowTitle(windowTitle);
  
  setMinimumSize(200, 160);
  move(setup.getMainWinPosX(), setup.getMainWinPosY());
  resize( setup.getMainWinWidth()>0?setup.getMainWinWidth():1000,       
          setup.getMainWinHeight()>0?setup.getMainWinHeight():700);
  
  logger->write(SgLogger::INF, SgLogger::GUI | SgLogger::RUN, className() +
    ": nuSolve's rising; today is " + SgMJD::currentMJD().toString(SgMJD::F_VerboseLong));

  logger->write(SgLogger::INF, SgLogger::GUI | SgLogger::RUN, className() +
    ": software: " + libraryVersion.name() + " released on " + 
    libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
  logger->write(SgLogger::INF, SgLogger::GUI | SgLogger::RUN, className() +
    ": software: " + nuSolveVersion.name() + " released on " + 
    nuSolveVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY));
  logger->write(SgLogger::INF, SgLogger::GUI | SgLogger::RUN, className() +
    ": Testing logger's facilities:");
  logger->write(SgLogger::ERR, SgLogger::GUI | SgLogger::RUN, className() +
    ": This is a demo of error string");
  logger->write(SgLogger::WRN, SgLogger::GUI | SgLogger::RUN, className() +
    ": Warnings should look like this");
  logger->write(SgLogger::INF, SgLogger::GUI | SgLogger::RUN, className() +
    ": Normal output");
  logger->write(SgLogger::DBG, SgLogger::GUI | SgLogger::RUN, className() +
    ": ...and some debug output. End of demo.");
  
  statusBar_->showMessage("So say we all", 1200);
};



//
NsMainWindow::~NsMainWindow()
{
  // save logger's config:
  QSettings settings;
  settings.setValue("Logger/FacilityERR",       logger->getLogFacility(SgLogger::ERR));
  settings.setValue("Logger/FacilityWRN",       logger->getLogFacility(SgLogger::WRN));
  settings.setValue("Logger/FacilityINF",       logger->getLogFacility(SgLogger::INF));
  settings.setValue("Logger/FacilityDBG",       logger->getLogFacility(SgLogger::DBG));
  settings.setValue("Logger/FileName",          logger->getFileName());
  settings.setValue("Logger/Capacity",          logger->getCapacity());
  settings.setValue("Logger/IsStoreInFile",     logger->getIsStoreInFile());
  settings.setValue("Logger/IsNeedTimeMark",    logger->getIsNeedTimeMark());
  settings.setValue("Logger/UseFullDate",       logger->getUseFullDateFormat());
 
  setup.setMainWinWidth(width());
  setup.setMainWinHeight(height());
  setup.setMainWinPosX(x());
  setup.setMainWinPosY(y());
  //
  logger->clearSpool();
  logger->setIsStoreInFile(false);
  // 
  if (changeStyleMenu_)
    delete changeStyleMenu_;
  //
  if (logger)
    delete logger;
};



//
void NsMainWindow::createActions()
{
  // File menu:
  newFileAct_ = new QAction(tr("&New"), this);
  newFileAct_->setShortcuts(QKeySequence::New);
  newFileAct_->setStatusTip(tr("Create a new file"));
  connect(newFileAct_, SIGNAL(triggered()), this, SLOT(newFile()));
  newFileAct_->setEnabled(false);

  openFileAct_ = new QAction(tr("&Open"), this);
  openFileAct_->setShortcuts(QKeySequence::Open);
  openFileAct_->setStatusTip(tr("Open a file"));
  connect(openFileAct_, SIGNAL(triggered()), this, SLOT(openFile()));
  openFileAct_->setEnabled(false);

  saveFileAct_ = new QAction(tr("&Save"), this);
  saveFileAct_->setShortcuts(QKeySequence::Save);
  saveFileAct_->setStatusTip(tr("Save a file"));
  connect(saveFileAct_, SIGNAL(triggered()), this, SLOT(saveFile()));
  saveFileAct_->setEnabled(false);
  
  printFileAct_ = new QAction(tr("&Print"), this);
  printFileAct_->setShortcuts(QKeySequence::Print);
  printFileAct_->setStatusTip(tr("Print a file"));
  connect(printFileAct_, SIGNAL(triggered()), this, SLOT(printFile()));
  printFileAct_->setEnabled(false);
  
  closeFileAct_ = new QAction(tr("&Close"), this);
  closeFileAct_->setShortcuts(QKeySequence::Close);
  closeFileAct_->setStatusTip(tr("Close a file"));
  connect(closeFileAct_, SIGNAL(triggered()), this, SLOT(closeFile()));
  closeFileAct_->setEnabled(false);
  
  closeAllAct_ = new QAction(tr("Close &All"), this);
  closeAllAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_A));
  closeAllAct_->setStatusTip(tr("Close all files"));
  connect(closeAllAct_, SIGNAL(triggered()), this, SLOT(closeAll()));
  closeAllAct_->setEnabled(false);
  
  flushLogAct_ = new QAction(tr("Flush Log"), this);
  flushLogAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_L));
  flushLogAct_->setStatusTip(tr("Save all log records into a file"));
  connect(flushLogAct_, SIGNAL(triggered()), this, SLOT(flushLog()));
  
  quitAct_ = new QAction(tr("&Quit"), this);
  //  quitAct_->setShortcuts(QKeySequence::Quit);
  quitAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_Q));
  quitAct_->setStatusTip(tr("Quit the application"));
  connect(quitAct_, SIGNAL(triggered()), this, SLOT(quit()));

  // Edit menu:
  editGenConfigAct_ = new QAction(tr("Edit &General Config"), this);
  editGenConfigAct_->setStatusTip(tr("Start edit general config dialog"));
  editGenConfigAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_G));
  connect(editGenConfigAct_, SIGNAL(triggered()), this, SLOT(editGenConfig()));
  
  editSessionAct_ = new QAction(tr("&Edit Session (DBH)"), this);
  editSessionAct_->setStatusTip(tr("Start edit session dialog (DBH)"));
  editSessionAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_E));
  connect(editSessionAct_, SIGNAL(triggered()), this, SLOT(editSession()));

  openWrapperFileAct_ = new QAction(tr("&Open Wrapper file (vgosDB)"), this);
  openWrapperFileAct_->setStatusTip(tr("Start edit session dialog (vgosDB)"));
  openWrapperFileAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_O));
  connect(openWrapperFileAct_, SIGNAL(triggered()), this, SLOT(openWrapperFile()));
  
  openSessionAct_ = new QAction(tr("&Open Session (vgosDB)"), this);
  openSessionAct_->setStatusTip(tr("Start edit session dialog (vgosDB)"));
  openSessionAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));
  connect(openSessionAct_, SIGNAL(triggered()), this, SLOT(openSession()));

  openAgvAct_ = new QAction(tr("Open vgosDa file"), this);
  openAgvAct_->setStatusTip(tr("Open vgosDa file"));
  openAgvAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_A));
  connect(openAgvAct_, SIGNAL(triggered()), this, SLOT(openAgvFile()));


  loadLnfSessionAct_ = new QAction(tr("Load last &non-finished session"), this);
  loadLnfSessionAct_->setStatusTip(tr("Continue previous work"));
  loadLnfSessionAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_N));
  connect(loadLnfSessionAct_, SIGNAL(triggered()), this, SLOT(loadLnfSession()));
  loadLnfSessionAct_->setEnabled( 
    setup.getLnfsOriginType()!=SgVlbiSessionInfo::OT_UNKNOWN && 
    setup.getLnfsFileName().size()>0 &&
    setup.getLnfsIsThroughCatalog() == setup.getHave2UpdateCatalog());
  
  undoAct_ = new QAction(tr("&Undo"), this);
  undoAct_->setShortcuts(QKeySequence::Undo);
  undoAct_->setStatusTip(tr("Undo"));
  connect(undoAct_, SIGNAL(triggered()), this, SLOT(undo()));
  undoAct_->setEnabled(false);
  
  redoAct_ = new QAction(tr("&Redo"), this);
  redoAct_->setShortcuts(QKeySequence::Redo);
  redoAct_->setStatusTip(tr("Redo"));
  connect(redoAct_, SIGNAL(triggered()), this, SLOT(redo()));
  redoAct_->setEnabled(false);
  
  cutAct_ = new QAction(tr("&Cut"), this);
  cutAct_->setShortcuts(QKeySequence::Cut);
  cutAct_->setStatusTip(tr("Cut"));
  connect(cutAct_, SIGNAL(triggered()), this, SLOT(cut()));
  cutAct_->setEnabled(false);
  
  copyAct_ = new QAction(tr("&Copy"), this);
  copyAct_->setShortcuts(QKeySequence::Copy);
  copyAct_->setStatusTip(tr("Copy"));
  connect(copyAct_, SIGNAL(triggered()), this, SLOT(copy()));
  copyAct_->setEnabled(false);
  
  pasteAct_ = new QAction(tr("&Paste"), this);
  pasteAct_->setShortcuts(QKeySequence::Paste);
  pasteAct_->setStatusTip(tr("Paste"));
  connect(pasteAct_, SIGNAL(triggered()), this, SLOT(paste()));
  pasteAct_->setEnabled(false);
  
  preferencesAct_ = new QAction(tr("Preferences"), this);
  //  preferencesAct_->setShortcuts(QKeySequence::Preferences);
  preferencesAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_R));
  preferencesAct_->setStatusTip(tr("Preferences"));
  connect(preferencesAct_, SIGNAL(triggered()), this, SLOT(preferences()));
   
  selectFontAct_ = new QAction(tr("Select &Font"), this);
  selectFontAct_->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_F));
  selectFontAct_->setStatusTip(tr("Fontifying"));
  connect(selectFontAct_, SIGNAL(triggered()), this, SLOT(selectFont()));
  
  // Tools menu:
  makeSomething1Act_ = new QAction(tr("&Make something useful #1"), this);
  //  makeSomething1Act_->setShortcuts(QKeySequence::);
  makeSomething1Act_->setStatusTip(tr("Make something useful, theme #1"));
  connect(makeSomething1Act_, SIGNAL(triggered()), this, SLOT(makeSomething1()));
  makeSomething1Act_->setEnabled(false);
  
  testSomething1Act_ = new QAction(tr("&Test something testful #1"), this);
  //  testSomething1Act_->setShortcuts(QKeySequence::);
  testSomething1Act_->setStatusTip(tr("Test something testful, theme #1"));
  connect(testSomething1Act_, SIGNAL(triggered()), this, SLOT(testSomething1()));
  testSomething1Act_->setEnabled(false);
  

  // Test menu:
  testPlotterAct_ = new QAction(tr("Test Plotter"), this);
  testPlotterAct_->setStatusTip(tr("Run test for plotter dialog (temporary)"));
  connect(testPlotterAct_, SIGNAL(triggered()), this, SLOT(testPlotter()));
  
  testFour1Act_ = new QAction(tr("Test Four1"), this);
  testFour1Act_->setStatusTip(tr("Run test for four1 function dialog (temporary)"));
  connect(testFour1Act_, SIGNAL(triggered()), this, SLOT(testFour1()));
  
  test1Act_ = new QAction(tr("Test #1"), this);
  test1Act_->setStatusTip(tr("Run test #1"));
  connect(test1Act_, SIGNAL(triggered()), this, SLOT(test1()));
  test1Act_->setEnabled(false);
  
  

  // Help menu:
  aboutAct_ = new QAction(tr("&About"), this);
  //  aboutAct->setShortcuts(QKeySequence::);
  aboutAct_->setStatusTip(tr("About this application"));
  connect(aboutAct_, SIGNAL(triggered()), this, SLOT(about()));
  
  aboutQtAct_ = new QAction(tr("&About Qt"), this);
  //  aboutQtAct_->setShortcuts(QKeySequence::);
  aboutQtAct_->setStatusTip(tr("About Qt library"));
  connect(aboutQtAct_, SIGNAL(triggered()), this, SLOT(aboutQt()));
};



//
void NsMainWindow::createMenus()
{
  fileMenu_ = menuBar()->addMenu(tr("&File"));
  fileMenu_->addAction(newFileAct_);
  fileMenu_->addAction(openFileAct_);
  fileMenu_->addAction(saveFileAct_);
  fileMenu_->addAction(printFileAct_);
  fileMenu_->addAction(closeFileAct_);
  fileMenu_->addAction(closeAllAct_);
  fileMenu_->addAction(flushLogAct_);
  fileMenu_->addSeparator();
  fileMenu_->addAction(quitAct_);

  editMenu_ = menuBar()->addMenu(tr("&Edit"));
  editMenu_->addAction(editGenConfigAct_);
  editMenu_->addAction(editSessionAct_);
  editMenu_->addAction(openWrapperFileAct_);
  editMenu_->addAction(openSessionAct_);
  editMenu_->addAction(openAgvAct_);
  editMenu_->addAction(loadLnfSessionAct_);
  editMenu_->addAction(undoAct_);
  editMenu_->addAction(redoAct_);
  editMenu_->addAction(cutAct_);
  editMenu_->addAction(copyAct_);
  editMenu_->addAction(pasteAct_);
  editMenu_->addAction(preferencesAct_);
  editMenu_->addAction(selectFontAct_);

  changeStyleMenu_ = new QMenu(tr("Styles"));
  for (int i=0; i<styleKeys_.count(); i++)
    changeStyleMenu_->addAction(styleKeys_.at(i));
  connect(changeStyleMenu_, SIGNAL(triggered(QAction*)), this, SLOT(switchToStyle(QAction*)));

  editMenu_->addMenu(changeStyleMenu_);
  
  toolsMenu_ = menuBar()->addMenu(tr("&Tools"));
  toolsMenu_->addAction(makeSomething1Act_);
//  toolsMenu_->addAction(testSomething1Act_);

  testMenu_ = menuBar()->addMenu(tr("Te&st"));
  testMenu_->addAction(testPlotterAct_);
  testMenu_->addAction(testFour1Act_);
  testMenu_->addAction(test1Act_);
//  testMenu_->addAction(test2Act_);
  
  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  helpMenu_->addAction(aboutAct_);
  helpMenu_->addAction(aboutQtAct_);
};



//
void NsMainWindow::createStatusBar()
{
  statusBar_ = statusBar();
  lStats_ = new QLabel("X/S: WWWW/WWWW/WWWW 88888.88(ps) 888.8", statusBar_);
  lStats_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  lStats_->setMinimumWidth(lStats_->sizeHint().width() + 30);
  lStats_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  lStats_->setLineWidth(2);
  lStats_->setToolTip("Total and processed number of observation, delays WRMS, Chi2");
  lStats_->setStatusTip("Total and processed number of observation, delays WRMS, Chi2");
  
    
//QLabel *label = new QLabel("<p><font size=+1 face=\"Times\"><b>&nu;Solve-" + 
  QString                       labelStr("<p><font face=\"Times\"><b>&nu;Solve-" + 
                                          nuSolveVersion.toString() + "</b>");
  if (nuSolveVersion.getCodeName().length())
    labelStr += " (" + nuSolveVersion.getCodeName() + ") ";
  labelStr += "</font></p>";
  
  QLabel                       *label=new QLabel(labelStr, statusBar_);
  label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  label->setMinimumWidth(label->sizeHint().width() + 30);
  label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  label->setLineWidth(2);

  progressBar_ = new QProgressBar(statusBar_);
  progressBar_->reset();

  statusBarMessager_ = new QLabel("", statusBar_);
  statusBar_->addWidget(statusBarMessager_, 3);
  statusBar_->addWidget(progressBar_, 1);
  statusBar_->addPermanentWidget(lStats_);
  statusBar_->addPermanentWidget(label);
  showStats("", 0, 0, 0, 0.0, 0.0);
};



//
//                   === Functionality ===
//
// File menu:
void NsMainWindow::newFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": New file menu selected");
};



//
void NsMainWindow::openFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Open file menu selected");
};



//
void NsMainWindow::saveFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Save file menu selected");
};



//
void NsMainWindow::printFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Print file menu selected");
};



//
void NsMainWindow::closeFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Close file menu selected");
};



//
void NsMainWindow::closeAll()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Close all files menu selected");
};



//
void NsMainWindow::flushLog()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Flush log menu selected");
  logger->clearSpool();
};



//
void NsMainWindow::quit()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Quit menu selected");
  close();
};



// Edit menu:
void NsMainWindow::editGenConfig()
{
  (new SgGuiTaskConfigDialog(&config, &parametersDescriptor, NULL, this))->show();
};



//
void NsMainWindow::editSession()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Edit Session menu selected");

  QString                       fileName("");
  QString                       fileName2("");
  if (setup.getHave2UpdateCatalog())
  {
    NsSessionNameDialog     *nd;
    nd = new NsSessionNameDialog(this, &fileName, &fileName2);
    if (nd->exec() != QDialog::Accepted)
    {
      fileName  = "";
      fileName2 = "";
    };
  }
  else
  {
    QString     path2Databases(QDir::home().absolutePath() + "/DBH/");
    if (!QDir(path2Databases).exists())
      path2Databases = setup.getPath2DbhFiles();
    fileName = QFileDialog::getOpenFileName(this, tr("Open a database"), path2Databases, "*_V*");
  };
  //
  if (fileName.isEmpty())
    logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
      ": Session selection is canceled");
  else
    (new NsSessionEditDialog(fileName, fileName2, &config, &parametersDescriptor, 
      SgVlbiSessionInfo::OT_DBH, false, this, Qt::Window))->show();
};



//
void NsMainWindow::openWrapperFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Open Wrapper file menu selected");

  QString                       fileName("");
  QString                       path2Data(QDir::home().absolutePath() + "/vgosDB/");
  if (!QDir(path2Data).exists())
    path2Data = setup.getPath2VgosDbFiles();
  fileName = QFileDialog::getOpenFileName(this, tr("Open a wrap file"), path2Data, "*.wrp");

  if (fileName.isEmpty())
    logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
      ": Session selection is canceled");
  else
    (new NsSessionEditDialog(fileName, "", &config, &parametersDescriptor,
      SgVlbiSessionInfo::OT_VDB, false, this, Qt::Window))->show();
};



//
void NsMainWindow::openSession()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Open Session menu selected");

  QString                       fileName("");
  NsSessionNameDialog          *nd;
  nd = new NsSessionNameDialog(this, &fileName);
  if (nd->exec() != QDialog::Accepted)
    fileName  = "";
  //
  if (fileName.isEmpty())
    logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
      ": Session selection is canceled");
  else
    (new NsSessionEditDialog(fileName, "", &config, &parametersDescriptor,
      SgVlbiSessionInfo::OT_VDB, false, this, Qt::Window))->show();
};



//
void NsMainWindow::openAgvFile()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    "::openAgvFile(): Open vgosDa file menu selected");

  QString                       fileName("");
  QString                       path2Data(setup.path2(setup.getPath2VgosDaFiles()));
  fileName = QFileDialog::getOpenFileName(this, tr("Open VDA file"), path2Data, 
                "vgosDa file (*.vda)");
//              "vgosDa file (*.vda *.vda.bz2)");

  if (fileName.isEmpty())
    logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
      "::openAgvFile(): File selection was canceled");
  else
    (new NsSessionEditDialog(fileName, "", &config, &parametersDescriptor,
      SgVlbiSessionInfo::OT_AGV, false, this, Qt::Window))->show();
};



//
void NsMainWindow::loadLnfSession()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Load the Last Non-finished Session menu selected", true);

  (new NsSessionEditDialog(setup.getLnfsFileName(), "", &config, &parametersDescriptor, 
    setup.getLnfsOriginType(), true, this, Qt::Window))->show();
};



//
void NsMainWindow::undo()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Undo menu selected");
};



//
void NsMainWindow::redo()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Redo menu selected");
};



//
void NsMainWindow::cut()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Cut menu selected");
};



//
void NsMainWindow::copy()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Copy menu selected");
};



//
void NsMainWindow::paste()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Paste menu selected");
};



//
void NsMainWindow::preferences()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Preferences menu selected");
  (new NsSetupDialog(this))->show();
};



//
void NsMainWindow::selectFont()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Select font menu selected");

  bool        isOk(false);
  QFont       f=QFontDialog::getFont(&isOk, QApplication::font());
  if (isOk)
  {
    QApplication::setFont(f);
    QSettings settings;
    settings.setValue("Setup/GuiFont", f);
  };
};



//
void NsMainWindow::switchToStyle(QAction* action)
{
  QString key = action->text();
  QApplication::setStyle(key);
  QSettings settings;
  settings.setValue("Setup/GuiStyle", key);
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Switched to style <" + key + ">");
};



// Tools menu:
void NsMainWindow::makeSomething1()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Make something useful #1 menu selected");
};



//
void NsMainWindow::testSomething1()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Test something useful #1 menu selected");
};



// Test menu:
void NsMainWindow::testPlotter()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Test Plotter menu selected");
  (new NsTestDialog(this))->show();
};



void NsMainWindow::testFour1()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Test Four1 menu selected");
  (new NsTestFour1Dialog(this))->show();
};



// Test menu:
void NsMainWindow::test1()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Test #1 menu selected");
};



//
void NsMainWindow::test2()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": Test #2 menu selected");
};



// Help menu:
void NsMainWindow::about()
{
  QString                       aboutText;
  
  //nuSolveVersion.getSoftwareName()
  
  //nuSolveVersion.name()
  
  aboutText = 
    "<h3>This is &nu;Solve.</h3>"
    "<p>"
    "The software &nu;Solve is designed to perform analysis of geodetic VLBI observations."
    "</p>"

    "<p>"
    "You are running:" 
    "<ul>"
    "<li>The driver: <b>" + nuSolveVersion.name() + "</b> released on " + 
      nuSolveVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) + ";</li>" +
    "<li>The library: <b>" + libraryVersion.name() + "</b> released on " + 
      libraryVersion.getReleaseEpoch().toString(SgMJD::F_DDMonYYYY) + ".</li>"
    "</ul>"
    "</p>"
    
    "<p>"
    "If you have suggestions or comments, please send e-mail to &lt;sergei.bolotin@nasa.gov&gt;."
    "</p>"

//  "<p style=\"color:red;text-align:right;\">"
    "<p>"
    "Goddard VLBI group"
    "</p>"

    "";
  
  
  
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": About menu selected");
  QMessageBox::about(this, nuSolveVersion.name(), aboutText);
};



//
void NsMainWindow::aboutQt()
{
  logger->write(SgLogger::DBG, SgLogger::GUI, className() + 
    ": About Qt menu selected");
  QMessageBox::aboutQt(this, nuSolveVersion.name());
};



//
void NsMainWindow::showStats(const QString& label, int numTot, int numUsb, int numPrc, 
  double wrms, double chi2)
{
  lStats_->setText(label + QString("").sprintf(" %d/%d/%d %.2f(ps) %.5g", 
    numTot, numUsb, numPrc, wrms, chi2));
};
/*=====================================================================================================*/






/*=====================================================================================================*/
//
// aux functions:
//
void longOperationStartDisplay(int minStep, int maxStep, const QString& message)
{
  if (maxStep <= 0)
    maxStep = 1; // do not need "busy indicator"
  mainWindow->progressBar()->setRange(minStep, maxStep);
  mainWindow->statusBarMessager()->setText(message);
  qApp->processEvents();
};



//
void longOperationProgressDisplay(int step)
{
  mainWindow->progressBar()->setValue(step);
  qApp->processEvents();
};



//
void longOperationStopDisplay()
{
  mainWindow->progressBar()->reset();
  mainWindow->statusBarMessager()->setText("");
};



//
void longOperationMessageDisplay(const QString& message)
{
  mainWindow->statusBarMessager()->setText(message);
  qApp->processEvents();
};



//
void longOperationShowStats(const QString& label, int numTot, int numUsb, int numPrc, 
  double wrms, double chi2)
{
  mainWindow->showStats(label, numTot, numUsb, numPrc, wrms, chi2);
  qApp->processEvents();
}
/*=====================================================================================================*/
