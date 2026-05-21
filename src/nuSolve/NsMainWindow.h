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

#ifndef NS_MAIN_WINDOW_H
#define NS_MAIN_WINDOW_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgParametersDescriptor.h>
#include <SgTaskConfig.h>


#if QT_VERSION >= 0x050000
#   include <QtWidgets/QMainWindow>
#   include <QtWidgets/QStatusBar>
#   include <QtWidgets/QProgressBar>
#else
#   include <QtGui/QMainWindow>
#   include <QtGui/QStatusBar>
#   include <QtGui/QProgressBar>
#endif





class QLabel;
class NsMainWindow;
extern NsMainWindow *mainWindow;

void longOperationStartDisplay(int minStep, int maxStep, const QString& message);
void longOperationProgressDisplay(int step);
void longOperationStopDisplay();
void longOperationMessageDisplay(const QString& message);
void longOperationShowStats(const QString& label, int numTot, int numUsb, int numPrc, 
                            double wrms, double chi2);


/*=====================================================================================================*/
class NsMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  // Statics:


  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes or later assignments).
   */
  NsMainWindow(QWidget *parent=0, Qt::WindowFlags flags=0);

  /**A destructor.
   * Frees allocated memory.
   */
  virtual ~NsMainWindow();



  //
  // Interfaces:
  //
  QProgressBar*   progressBar() {return progressBar_;};
  
  QLabel*         statusBarMessager() {return statusBarMessager_;};


  //
  // Functions:
  //
  inline QString className() const {return "NsMainWindow";};
  void showStats(const QString& label, int numTot, int numUsb, int numPrc, double wrms, double chi2);

 
  //
  // Friends:
  //


  //
  // I/O:
  //
  //

private slots:
  // File menu:
  void newFile();
  void openFile();
  void saveFile();
  void printFile();
  void closeFile();
  void closeAll();
  void flushLog();
  void quit();
  // Edit menu:
  void editGenConfig();
  void editSession();
  void openWrapperFile();
  void openSession();
  void openAgvFile();
  void loadLnfSession();
  void undo();
  void redo();
  void cut();
  void copy();
  void paste();
  void preferences();
  void selectFont();
  // sub menu:
  void switchToStyle(QAction*);
  // Tools menu:
  void makeSomething1();
  void testSomething1();
  // Test menu:
  void testPlotter();
  void testFour1();
  void test1();
  void test2();
  // Help menu:
  void about();
  void aboutQt();


private:
  QMenu          *fileMenu_;
  QMenu          *editMenu_;
  QMenu          *changeStyleMenu_;
  QMenu          *toolsMenu_;
  QMenu          *helpMenu_;
  QMenu          *testMenu_;
  //
  QStatusBar     *statusBar_;
  QProgressBar   *progressBar_;
  QLabel         *lStats_;
  //  QActionGroup *alignmentGroup_;
  // File menu:
  QAction        *newFileAct_;
  QAction        *openFileAct_;
  QAction        *saveFileAct_;
  QAction        *printFileAct_;
  QAction        *closeFileAct_;
  QAction        *closeAllAct_;
  QAction        *flushLogAct_;
  QAction        *quitAct_;
  // Edit menu:
  QAction        *editGenConfigAct_;
  QAction        *editSessionAct_;
  QAction        *openWrapperFileAct_;
  QAction        *openSessionAct_;
  QAction        *openAgvAct_;
  QAction        *loadLnfSessionAct_;
  QAction        *undoAct_;
  QAction        *redoAct_;
  QAction        *cutAct_;
  QAction        *copyAct_;
  QAction        *pasteAct_;
  QAction        *preferencesAct_;
  QAction        *selectFontAct_;
  // Tools menu:
  QAction        *makeSomething1Act_;
  QAction        *testSomething1Act_;
  // Test menu:
  QAction        *testPlotterAct_;
  QAction        *testFour1Act_;
  QAction        *test1Act_;
  // Help menu:
  QAction        *aboutAct_;
  QAction        *aboutQtAct_;

  void            createActions();
  void            createMenus();
  void            createStatusBar();
  QLabel         *statusBarMessager_;

protected:
  QToolBar                   *toolBar_;
  QStringList                 styleKeys_;
};
/*=====================================================================================================*/









/*=====================================================================================================*/
#endif // NS_MAIN_WINDOW_H
