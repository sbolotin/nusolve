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

#ifndef SG_ESTIMATOR_H
#define SG_ESTIMATOR_H


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <QtCore/QMap>
#include <QtCore/QStack>
#include <QtCore/QString>



#include <SgMJD.h>



class SgVector;
class SgMatrix;
class SgUtMatrix;
class SgSymMatrix;

class SgParameter;
class SgPwlStorage;
class SgArcStorage;
class SgTaskConfig;


struct arg4Prc_01;
struct arg4Prc_02;



/***===================================================================================================*/
/**
 * SgEstimator -- an estimator.
 *
 */
/**====================================================================================================*/
class SgEstimator
{
public:
  struct StochasticSolutionCarrier
  {
    SgMJD                       epoch_;
    QList<SgParameter*>        *list_;
    SgVector                   *x_;
    SgSymMatrix                *P_;
  };
  //
  //
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a default object.
   */
  SgEstimator(SgTaskConfig*);

  /**A destructor.
   * Frees allocated memory.
   */
  ~SgEstimator();


  //
  // Interfaces:
  //
  inline QList<SgParameter*>*   regularParametersList();

  inline QList<SgParameter*>*   allRegularParametersList();

  inline QList<SgParameter*>*   stochasticParametersList();

  inline QList<SgParameter*>*   listPwl();
  
  inline QList<SgPwlStorage*>  *pwlStorage();

  inline QMap<QString, SgPwlStorage*> *pwlStorageByName();

  inline QList<SgArcStorage*>*  arcStorage();
  
  inline SgSymMatrix*           mPall();

  inline int numOfProcessedObs() const {return numOfProcessedObs_;};

  inline const QMap<QString, StochasticSolutionCarrier>& stcSolutions() {return stcSolutions_;};


  //
  // Functions:
  //
  /**Returns name of the class (for debug output).
   */
  static const QString className();

  /**Adds a parameter into the estimation process.
   */
  void addParameter(SgParameter*, bool=false);

  /**Adds a list of parameters into the estimation process.
   */
  void addParametersList(QList<SgParameter*>*, bool=false);

  /**Removes a parameter from the estimation process.
   */
  void removeParameter(SgParameter*);

  /**Removes a list of parameters from the estimation process.
   */
  void removeParametersList(QList<SgParameter*>*);

  /**Prepares the local structures to run process of estimation.
   * \param numOfExpectedObs -- a size of the batch (usually, == number of expected observations).
   */
  void prepare2Run(int numOfExpectedObs, const SgMJD&, const SgMJD&, const SgMJD&);

  /**Process an observation.
   * \param t -- epoch of the observation;
   * \param o_c -- a vector with (O-C);
   * \param sigma -- a vector with sigmas.
   */
  void processObs(const SgMJD& t, const SgVector& o_c, const SgVector& sigma);

  /**Process a constraint equation.
   * \param o_c -- (O-C);
   * \param sigma -- a sigma of the constraint equation.
   */
  void processConstraint(double o_c, double sigma);

  /**Finishes the esimation process.
   */
  void finisRun();

//
//  void prepareRegSolution4Epoch(const SgMJD&, QList<SgParameter*>*);
//
  void prepareStochasticSolution4Epoch(const SgMJD&, QList<SgParameter*>*);
//
  void clearPartials();
//
  double calcAX(const SgMJD&, bool);
//
  double calc_aT_P_a(const SgMJD&);
  double calc_aT_P_a_opt(const SgMJD&);
  double calc_aT_P_a_opt(const SgMJD&, const QList<SgParameter*>&);
//
  inline double maxConditionNumber() const {return maxConditionNumber_;};

  //
  // Friends:
  //

  //
  // I/O:
  //
  // ...

private:
  struct SmoothCarrier
  {
    SgMJD                       epoch_;
    QList<SgParameter*>        *p_s_;
    SgVector                   *zp_s_;
    SgUtMatrix                 *Rp_s_;
    SgMatrix                   *Rpp_s_;
    SgMatrix                   *Rpx_s_;
  };
  struct RPCarrier
  {
    SgMJD                       tLeft_;
    SgMJD                       tRight_;
    QList<SgParameter*>        *list_;
    QList<SgParameter*>        *listOthers_;
    SgVector                   *zx_;
    SgUtMatrix                 *Rx_;
    SgMatrix                   *Rxx_;
  };
  struct RegularSolutionCarrier
  {
    SgMJD                       tLeft_;
    SgMJD                       tRight_;
    QList<SgParameter*>        *list_;
    QList<SgParameter*>        *listOthers_;
    SgVector                   *x_;
    SgSymMatrix                *P_;
    SgMatrix                   *Pxx_;
    RegularSolutionCarrier()
      {tLeft_=tRight_=tZero; list_=listOthers_=NULL; x_=NULL; P_=NULL; Pxx_=NULL;};
  };
  
  //
  SgTaskConfig                 *config_;

  //
  SgMJD                         tLastProcessed_;
  SgMJD                         tNextLeft_;
  SgMJD                         tNextRight_;

  // regular parameters:
  QList<SgParameter*>          *listXAll_;
  SgSymMatrix                  *PxAll_;
  QMap<QString, SgParameter*>  *xAllByName_;
  //
  unsigned int                  nX_;
  QList<SgParameter*>          *listX_;
  QMap<QString, SgParameter*>  *xByName_;
  SgVector                     *x_;
  SgVector                     *zx_;
  SgUtMatrix                   *Rx_;
  SgSymMatrix                  *Px_;
  QStack<RPCarrier>             rpStack_;


  // stochastic parameters:
  unsigned int                  nP_;
  QList<SgParameter*>          *listP_;
  QMap<QString, SgParameter*>  *pByName_;
  SgVector                     *p_;
  SgVector                     *zp_;
  SgUtMatrix                   *Rp_;
  SgMatrix                     *Rpx_;
  SgSymMatrix                  *Pp_;
  SgMatrix                     *SR_;
  // stochastic smoothing:
  SgUtMatrix                   *Rp_s_;
  SgMatrix                     *Rpp_s_;
  SgMatrix                     *Rpx_s_;
  SgVector                     *zp_s_;
  QStack<SmoothCarrier>         scStack_;
//------------------------------------------------------------
//  // maps of solutions of regular and stochastic parameters:
//  QMap<QString, RegularSolutionCarrier*>    regSolutions_;
//  QMap<QString, RegularSolutionCarrier*>    regSolByName_;
//  QMap<QString, SgParameter*>               regParByName_;
  QMap<QString, StochasticSolutionCarrier>  stcSolutions_;

  //
  // special types of parameters:
  // arc parameters:
  QList<SgParameter*>          *listA_;
  QMap<QString, SgParameter*>  *aByName_;
  QList<SgArcStorage*>         *arcStorage_;
  // PWL parameters:
  QList<SgParameter*>          *listPwl_;
  QMap<QString, SgParameter*>  *pwlParByName_;
  QMap<QString, SgPwlStorage*> *pwlStorageByName_;
  QList<SgPwlStorage*>         *pwlStorage_;


  //
  // data:
  unsigned int                  nZ_;
  SgVector                     *z_;
  SgMatrix                     *Ap_;
  SgMatrix                     *Ax_;
  SgVector                     *u_;
  int                           numOfProcessedObs_;
  double                        maxConditionNumber_;

  // threadisation:
  int                           numOfThreads_;
  pthread_attr_t                pthreadAttr_;
  SgMatrix                     *u4pth_;
  // proc01:
  struct arg4Prc_01             *args4Prc_01_;
  pthread_t                     *th4Prc_01_;
  pthread_mutex_t              *pthMutexes4Prc_01_;
  pthread_mutex_t               pthMutL4Prc_01_;
  int                           lastPrcColumn4Prc_01_;
  // proc02:
  struct arg4Prc_02             *args4Prc_02_;
  pthread_t                     *th4Prc_02_;
  pthread_mutex_t              *pthMutexes4Prc_02_;
  pthread_mutex_t               pthMutL4Prc_02_;
  int                           lastPrcColumn4Prc_02_;

  // tmp:

  SgMJD                         tWall0_;
  SgMJD                          tWall_;
  SgMJD                         tWall4ProcObs0_;
  SgMJD                          tWall4ProcObs_;

  double                         time4DataUpdate_;
  double                         time4PropagateStc_;
  double                         time4SaveStc_;
  double                         time4ProcObs_;



  // private functions:
  void      dataUpdate();
  void      dataUpdate_th();
  void      propagateStochasticParameters(double);
  void      propagateStochasticParameters_th(double);

  void      solveStochasticParameters();
  void      save4Smoothing(const SgMJD&);

  // tst:
  void      saveProcessedParameters(const SgMJD&);
  void      insertNewParameters(const SgMJD&);
  //
  void      deployArcParameters(const SgMJD&, const SgMJD&, const SgMJD&);
  void      propagateArcPartials(const SgMJD&);
  //
  void      deployPwlParameters(const SgMJD&, const SgMJD&, const SgMJD&);
  void      propagatePwlPartials(const SgMJD&);
  void      calcPwlTraces(const QList<SgParameter*> *listX, const SgSymMatrix& mPx);
  // tst:
  void      collectDependentInfArray(const RPCarrier&, SgVector*, SgSymMatrix*);
  void      prepareRegSolution4Epoch(const SgMJD&, QList<SgParameter*>*);
  void      submittSolution2GlobalStore(SgSymMatrix*, QList<SgParameter*>*,
                          SgMatrix* =NULL, QList<SgParameter*>* =NULL);

  // some private math:
  // solves equation   "R*x = z", x -- unknown.
  SgVector& solveEquation(const SgUtMatrix& R, SgVector& x, const SgVector& z);
  // evaluate R*R^T product:
  SgSymMatrix& calculateRRT(SgSymMatrix&, const SgUtMatrix&);

  SgSymMatrix calcQForm(const SgSymMatrix&, const SgMatrix&);

  SgSymMatrix calcQForm(const SgSymMatrix&, const SgUtMatrix&);
  
  void calcConditionNumber(const SgUtMatrix& R, const SgUtMatrix& RInv);
};
/*=====================================================================================================*/








/*=====================================================================================================*/
/*                                                                                                     */
/* SgEstimator inline members:                                                                         */
/*                                                                                                     */
/*=====================================================================================================*/



//
// INTERFACES:
//
//
inline QList<SgParameter*>* SgEstimator::regularParametersList()
{
  return listX_;
};



inline QList<SgParameter*>* SgEstimator::allRegularParametersList() 
{
  return listXAll_;
};



//
inline QList<SgParameter*>* SgEstimator::stochasticParametersList()
{
  return listP_;
};



//
inline QList<SgParameter*>* SgEstimator::listPwl()
{
  return listPwl_;
};



//  
inline QList<SgPwlStorage*>* SgEstimator::pwlStorage()
{
  return pwlStorage_;
};



//
inline QMap<QString, SgPwlStorage*>* SgEstimator::pwlStorageByName()
{
  return pwlStorageByName_;
};



//
inline QList<SgArcStorage*>* SgEstimator::arcStorage()
{
  return arcStorage_;
};



//
inline SgSymMatrix* SgEstimator::mPall()
{
  return PxAll_;
};


//
// FRUNCTIONS:
//
//
//



//
// FRIENDS:
//
//
//
/*=====================================================================================================*/





/*=====================================================================================================*/
//
// aux functions:
//


/*=====================================================================================================*/


/*=====================================================================================================*/
#endif //SG_ESTIMATOR_H
