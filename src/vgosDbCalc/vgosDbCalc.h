/*
 *    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
 *    system and is designed to calculate theoretical values and store them
 *    in the vgosDb format.
 *    Copyright (C) 2016-2020 Sergei Bolotin.
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

#ifndef VGOS_DB_CALC_H
#define VGOS_DB_CALC_H

#include <stddef.h>


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif


#include <SgVersion.h>



#include "VcSetup.h"


#define UNUSED __attribute__((__unused__))
#define f_true (1)
#define f_false (0)




/*=====================================================================================================*/
//
//
extern "C" 
{
/*
!  cuser11.i  - Calc user type
*/
  extern struct
  {
    char                        c_mode[6];
  } mode_;

/*
! Inputs.i
*/
  extern struct
  {
    char                        External_inputs[80], Ex_sites[80], Ex_stars[80], Ex_ocean[80],
                                Ex_EOP[80], Ex_tilts[80], Ex_OPTL[80];
    int                         External_aprioris, Input_sites, Input_stars, Input_ocean, Input_EOP,
                                Input_tilts, Input_OPTL, le4;
  } extrnl_;


  //
  //                                               replacements:
  //
  // 
  void kai_   (short &kruc, short &in, short &iout, short &istp, char *lkynm_chr,
              short &iveri, char *lkout_chr, char *lfto_chr, short &ivero, 
              char *lfio_chr, short &kerr, 
              size_t len1, size_t len2, size_t len3, size_t len4);
  void wridr_();
//  void ask_  (char (&lcode)[14], short *ntoc, short &dim1, short &dim2, short &dim3, 
              //short &nver, char (&ltext_chr)[160], short &ktype, short &kerr, 
              //size_t len1, size_t len2);

  void ask_    (const char *lcode, const short &ntoc, const short &dim1, const short &dim2,
               const short &dim3, const short &nver, const char* ltext_chr, const short &ktype,
               short &kerr, size_t len1, size_t len2);


  void mvrec_(short &ntoc, short &kmode, short &knum, short &kerr);
  void phist_(short &nhist, const char *lhist_chr, size_t len);
  void finis_(short par);
  void datsv_();

  // adds:
  void adda_ (const short& ntoc, const char *lcode, const char *descrText,
              const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2);
  void addi_ (const short& ntoc, const char *lcode, const char *descrText,
              const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2);
  void addj_ (const short& ntoc, const char *lcode, const char *descrText,
              const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2);
  void add4_ (const short& ntoc, const char *lcode, const char *descrText,
              const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2);
  void addr_ (const short& ntoc, const char *lcode, const char *descrText,
              const short& dim1, const short& dim2, const short& dim3, size_t len1, size_t len2);

  // dels:
  void dela_(const short& ntoc, const char *lcode, size_t len);
  void deli_(const short& ntoc, const char *lcode, size_t len);
  void delj_(const short& ntoc, const char *lcode, size_t len);
  void del4_(const short& ntoc, const char *lcode, size_t len);
  void delr_(const short& ntoc, const char *lcode, size_t len);

  // gets:
  void getr_(  char (&lcode)[14], double values[], short *dim1, short *dim2, short *dim3, short numDims[],
              short &kerr, size_t len);
  void get4_(  const char *lcode, double values[], 
              const short &dim1, const short &dim2, const short &dim3, short numDims[], 
              short &kerr, size_t len);
  void geti_(  const char *lcode, short values[], 
              const short &dim1, const short &dim2, const short &dim3, short numDims[],
              short &kerr, size_t len);
  void getj_(  char (&lcode)[14], int values[],
              short *dim1, short *dim2, short *dim3, short numDims[], short &kerr, size_t len);
  void geta_(  const char *lcode, char values[], 
              const short &dim1, const short &dim2, const short &dim3, short numDims[], 
              short &kerr, size_t len);

  // puts:
  void puta_(  const char *lcode, const char vals[],
              const short &dim1, const short &dim2, const short &dim3,
              size_t len);
  void puti_(  const char *lcode, const short vals[],
              const short &dim1, const short &dim2, const short &dim3,
              size_t len);
  void putr_(  const char *lcode, const double vals[],
              const short &dim1, const short &dim2, const short &dim3,
              size_t len);
  void put4_(  const char *lcode, const double vals[],
              const short &dim1, const short &dim2, const short &dim3,
              size_t len);
  void putj_(char (&lcode)[14], int     vals[], short *dim1, short *dim2, short *dim3, size_t len);

// Aux:
  void setmodeflag_(short *modeflag);
  short getunit_();
  short gethostname_(char *hostName, size_t len);
  short trimlen_(char *chrs, size_t len);

  void quit_calc_(short iPar[]);
};



extern "C" 
{
//                                                CALC's functions:
//
//  SUBROUTINE START (ILU, CALCON_NAME, Iout)
//      Integer*2       ILU, iout
//      Character*128   CALCON_NAME
  void start_(short& iLu, const char* calcOnName, short& iOut, size_t len);
  
//  SUBROUTINE TOCUP()
  void tocup_();

//  SUBROUTINE INITL ( KOUNT )
//      Integer*4  KOUNT
  void initl_(int& kount);
  
//  SUBROUTINE OBSNT ( KOUNT, KEND)
//      Integer*4 KOUNT, KEND
  void obsnt_(int& kount, int& kend);

//  SUBROUTINE DRIVR()
  void drivr_();


 
};
/*=====================================================================================================*/




/***===================================================================================================*/
/**
 *
 *
 */
/**====================================================================================================*/

extern VcSetup                  setup;

extern SgVersion                vgosDbCalcVersion;

class VcCalc2SessionIfc;
extern VcCalc2SessionIfc        c2s;
extern QString                  histIntroRec;


// helpers:
QString  fortArray2QString(const char *chrs, size_t len);

void qString2fortArray(const QString& str, char *chrs, size_t len);

void mimicCalcMain(int arg1, int arg2, const char* arg3, const char* arg4);
void mimicCalcMain2(int arg1, int arg2, const char* arg3, const char* arg4, const QList<QString>& bands);


/*=====================================================================================================*/
#endif // VGOS_DB_CALC_H
