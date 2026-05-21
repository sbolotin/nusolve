       SUBROUTINE GET_CALIB ( JCAPPL, JSITI, ITT,ISITE, DT, RT, &
     &                  CALIBS, ICORR, GION, GIONSG, PHION, PHIONS,&
     &                  DERR, RERR, DPHER,DERR_S, RERR_S,DPHER_S,  & 
     &                  DERR_RAW, RERR_RAW, DPHER_RAW, &  
     &                  EFFREQ, REFFREQ, PHEFFREQ,  EFFREQ_XS,REFFREQ_XS,PHEFFREQ_XS,  &      
     &                  ITTB, ET, SE, SS, CALIBB, CALIBM, OBCAPL, MCAPL, &
     &                  ISITN, ISTAR, VSTARC, AZ, ELEV, ATMPR, RELHU, TEMPC, &
     &                  LATS, HEIGHTS, AX_OFFS, AX_TYPES, BARO_CALS, BARO_HEIGHTS, &
     &                  APX, JCAFFL, NFCAL, FCAL_NAMES, NAMSTA,IDB, &             
     &                  AXDIF, ISTRN_CHR, SOURCE_WEIGHT_FILE, &
     &                  SOURCE_WEIGHTS, AVG_ATM, KELDEP_NOISE, ATM_ZENDEL, &
     &                  RWT_EL_USE, RWT_SRC_USE, MAP_FUN, fjday, &
     &                  IONO_USE, TRP_USE, STS_VTD,cals,kdebug)
      IMPLICIT   NONE ! Updated by Jim Ryan for I*4 compliance, Sept 2002

!
! 1.  GET_CALIB PROGRAM SPECIFICATION
!
! 1.1 Apply the weather, cable and ionosphere calibrations to the
!     theoretical delays and rates.
!
! 1.2 REFERENCES:
!
! 2.  GET_CALIB INTERFACE
!     Updated to specificaly type integers which
!-------------------------------------------------
!
! 2.1 Parameter File
      INCLUDE 'solve.i'
      INCLUDE 'astro_constants.i'

! This has to go before cals 
      include 'cals.i'   
!
! 2.2 INPUT Variables:
!
      INTEGER*2  OBCAPL                    ! OBCAPL - Bit flag of contributions to be applied
      Integer*2  MCAPL                     ! MCAPL  - Bit flag of mode calibrations to be applied 

      integer*2  JCAPPL(MAX_ARC_STA)       ! JCAPPL - Bit flag of station cable & atmosphere calibrations to be applied
      integer*2  JSITI(MAX_ARC_STA)        ! JSITI - Station ion calibration flags 
      INTEGER*2  ITTB(MAX_ARC_BSL)         ! ITTB - NAMFIL station/baseline correspondence table, packed 
      INTEGER*2  ITT(MAX_ARC_STA)          ! ITT - NAMFIL/PARFIL station correspondence table
      integer*2  ISITE(2)                  ! ISITE - Site number of the two stations in this observation
      INTEGER*2  ISITN(4,MAX_STA)          ! ISITN - Array of site names
      INTEGER*2  NAMSTA                    ! namsta - # stations in namfil
      INTEGER*2  ISTAR                     ! ISTAR - Source number for this observation
      Integer*2  AX_TYPES(MAX_ARC_STA)     ! AX_TYPES - ANtenna axis type for each station
      REAL*8     VSTARC(2,MAX_SRC)         ! VSTARC - Array of source coordinates (RA and DEC, in radians)
      real*8     AX_OFFS(MAX_ARC_STA)      ! AX_OFFS - Antenna axis offset  

      REAL*8     CALIBS(2,2,MAX_CAL)
      REAL*8     CALIBB(2,MAX_CONT)
      real*8     CALIBM(6,M_CLM)
      real*8     GION(2), GIONSG(2), PHIONS

      REAL*8     LATS(MAX_ARC_STA)          ! LATS - Latitude of each station
      real*8     HEIGHTS(MAX_ARC_STA)       ! HEIGHTS - Heights of each station
      REAL*8     BARO_CALS(MAX_ARC_STA)     ! BARO_CALS - Barometer calibration for each station
      real*8     BARO_HEIGHTS(MAX_ARC_STA)  ! BARO_HEIGHTS - Height of barometer for each station

      real*8     ET(2,MAX_ARC_BSL)         ! ET - Baseline formal errors (from NAMFIL)

      REAL*8     AZ(2), ELEV(2)            ! AZ - Azimuth,  ELEV - Elevation
      real*8     ATMPR(2)                  ! ATMPR - Atmospheric pressure, millibars
      real*8     RELHU(2)                  ! RELHU - Relative humidity
      real*8     TEMPC(2)                  ! TEMPC - Temerature (Celsius)

   
      REAL*8     DT, RT                    !theoretical delay, rate
      real*8     derr,rerr,dpher           !sigmas of X-band   (At the end of the routine these are modified)   
      real*8     derr_s, rerr_s, dpher_s   !sigmas of S-band 
      REAL*8     EFFREQ,     REFFREQ,      PHEFFREQ    !Effective frequencie for ionosphere
      real*8     EFFREQ_XS,  REFFREQ_XS,  PHEFFREQ_XS  !frequencies other bands. 
      real*8     derr_raw, rerr_raw, dpher_raw   !error after ionosphere correction
      real*8     derr_ion_free
      

      INTEGER*2  JCAFFL(7,MAX_ARC_STA)     ! JCAFFL - Bit array of flyby calibrations to be applied
      integer*2 NFCAL,IDB
      CHARACTER  FCAL_NAMES(*)*8
      real*8        AXDIF(*)
      REAL*8        AVG_ATM(4,*)
      LOGICAL*2     KELDEP_NOISE
      CHARACTER     ISTRN_CHR*(*), SOURCE_WEIGHT_FILE*(*), SOURCE_WEIGHTS*(*)
      INTEGER*4     RWT_EL_USE, RWT_SRC_USE
      real*8 fjday
      INTEGER*4     IONO_USE, TRP_USE, STS_VTD
      TYPE ( CALS_STRU ) ::  CALS                  !calibration structure
      logical  kdebug                             !if true, write debug info 
!
! 2.3 OUTPUT Variables:
!
!
! 2.4 COMMON BLOCKS USED
      INCLUDE 'socom.i'
      INCLUDE 'glbc3.i'
      INCLUDE 'glbcm.i'
      INCLUDE 'bindisp.i'
      INCLUDE 'flyby.i'
      include 'trp.i'
      include 'precm.i'     !used to get letters. 
   
!     include 'prfil.i' 

!
! 2.5 SUBROUTINE INTERFACE
!
!       CALLING SUBROUTINES:
!       CALLED SUBROUTINES: calcalc
!
! Functions

      CHARACTER, EXTERNAL :: MJDSEC_TO_DATE*30
      CHARACTER*21 JD_TO_DATE_1
      LOGICAL*2, EXTERNAL :: KBIT
      LOGICAL*4, EXTERNAL :: DATYP_INQ
      INTEGER*4, EXTERNAL :: ILEN, I_LEN, LTM_DIF

! 3.  LOCAL VARIABLES
!      

   
      real*8     SE(MAX_ARC_STA), SS(MAX_ARC_SRC), MAP_FUN(2,2) 
      REAL*8     PHION, SRC_WT(4)
   
      real*8     fix_group, fix_rate, fix_phase         !used to fix sigmas when applying ionosphere
         
      real*8     TROP_FRACT(4)
   
      LOGICAL*2  MTT_SEAS_DRY(2), MTT_SEAS_WET(2)
      LOGICAL*2  IFA_SEAS_DRY(2), IFA_SEAS_WET(2)

      INTEGER*2  ID_CFAKBDRY, ID_CFAJJDRY, ID_CFAKBWET, ID_LANYI
      INTEGER*2  ID_MTTDRYSS, ID_MTTDYFLY, ID_IFADRYSS, ID_IFADYFLY
      INTEGER*2  ID_NMFDYFLY, ID_NMFWTFLY
      INTEGER*2  DO_LANWET, DO_PRCOR
   
      real*8     ATM_ZENDEL(2)
      REAL*8     ELN(4,2)

      REAL*8     CFAKBDRY(2,2), CFAKBWET(2,2)
  
      REAL*8     APP(2,2), APX(2,2)
      REAL*8     FREQ_GR_X, FREQ_GR_S, FREQ_PH_X, FREQ_PH_S, &
     &           FREQ_RATE_X, FREQ_RATE_S

      Integer*2   IDB_SAVE, I, ICORR, ISTA1, ISTA2, ISTAT, J, JJ,   N
     
      integer*4 istat4                 !integer*4 version of ISTAT 
      INTEGER*2  ID_JJ(2), ID_WET(3)
      LOGICAL*2  SET_THIS_ID, TWICE_IN_NAMF, IONO_APPLIED
      CHARACTER  ERRSTR*128, STR*128
   
      real*8     TDB_OBS, TAI_OBS, TIM_TLR,    EL_TLR, EL_MIN
      PARAMETER  ( TIM_TLR =  1.0D0 )
      PARAMETER  ( EL_TLR  =  1.0D0*DEG__TO__RAD )
      PARAMETER  ( EL_MIN  =  1.0D0*DEG__TO__RAD )
      INTEGER*4  MJD_TAI_OBS, IND_TRP(2), IND_SCA(2)
      LOGICAL*2  GET_SOURCE_WEIGHT
      INTEGER*2  ICTS, ICTT
      INTEGER*4  J1, J2, J3, IND_STA
      REAL*8     CAL_ADD, CAL_FRACT(2)
      REAL*8     CALIBB_DEL, CALIBB_RATE
      REAL*8     ATMP_TRP, HZD, HZDDOT
   
       
! Troposphere related variables. 
      logical kdebug_trp                             !print out trp debuging info.
      real*8  az_trp,el_trp, p_trp, tempC_trp         !other trp info. 
      real*8  slant_trp, map_Trp, grad_n_trp, grad_e_trp
      real*8  slant_rate, map_rate, grad_n_rate, grad_e_rate 
      integer ierr_trp                               !error 
       
! Quick way to get station names.
      integer*2  isite_name(4,2) 
      character*8 lsite_name(2)
      equivalence (isite_name,lsite_name)   

      LOGICAL*2  MET_CHANGE
      real*8  dt_del, rt_del    !change in dt, rt, 
      character*25 lkind        !kind of calibration
      character*22 ltime_tag
      integer*4    itemp          

! CALIBS -
! ICORR -

! APX -
! ID_CFAKBDRY, ID_CFAKBWET, ID_CFAJJDRY

!
      SAVE ID_CFAKBDRY, ID_CFAKBWET, ID_CFAJJDRY, ID_LANYI, &
     &     DO_LANWET,IDB_SAVE, ID_MTTDRYSS, ID_MTTDYFLY, &
     &     MTT_SEAS_DRY, MTT_SEAS_WET, &
     &     ID_IFADRYSS, ID_IFADYFLY, &
     &     IFA_SEAS_DRY, IFA_SEAS_WET, &
     &     ID_NMFDYFLY, ID_NMFWTFLY
!
      DATA IDB_SAVE /0/, TWICE_IN_NAMF /.FALSE./
!
! 4.  HISTORY
!   WHO   WHEN   WHAT
!   2026-02-10  John Gipson. Removed checking of ionosphere which is now done in supstat_set.
!               Previously had flag NOGOOD which was set in this routine, but never used outside of it. 
!   AM   8411??  Created
!   IS   860113  New calibrations/contributions handling scheme:
!                contributions are in the array CALIBS, and are applied
!                or not according to the bit flag in JCAPPL, where if
!                the Jth bit in JCAPPL(I) is set, the contribution stored
!                in CALIBS(*,*,J) is applied to the delay and rate value
!                for station I. The actual source (LCODE) of each element in
!                CALIBS is given in the CORFIL.
!   KDB  860303  The scheme listed in the 13 Jan
!                1986 modification applies to station dependent calibrations/
!                contributions.  Now GET_CALIB is being modified to handle
!                observation dependent contributions.  These contributions are
!                in array CALIBB, and are applied or not applied according to
!                bit flag OBCAPL, where if bit J in OBCAPL is set, the
!                contribution stored in CALIBB(*,J) is applied to the delay and
!                rate values for the data base.  The actual source (LCODE) of
!                each element in CALIBB is given in the CORFIL.
!   JRR 861216:  Fixed a mistake of my own making dealing with the
!                application of ionospheric corrections to phase data.
!
!   KDB 870817:  Code to handle special calibrations (CFAWET and CFADRY,
!                which can be applied, even if the data base does not
!                contain their lcodes and they're therefore technically
!                unavailable.
!   JWR 870606   Modified to return the raw (not reweighted) delay and rate
!                errors.
!   KDB 910715   Use the latest namfil/corfil scheme, where flyby and regular
!                calibrations separate in namfil, namfil carries calib/contrib
!                names and only SDBH accesses corfil.
!   AEE 911120   Added MTT and IFADIS dry and wet mapping functions.
!   AEE 920212   Added CHAO DRY stuff.
!   AEE 920627   Removed partial stuff and placed them in ATMPART.F
!
!   JMG 930428   Corrected for Bad ionosphere calculation. Previous version
!                did not take into correlation between iono sigma and gruop/
!                phase sigma.  This error was re-discovered in April, 93.
!                Approximate fix uses constant ratio between X and S band freq.
!   kdb 950831   Add atmospheric turbulence rate constant and mapping function
!                parameter error delay and rate constants.
!   kdb 950905   Add alternate atmospheric turbulence constants scheme:
!                a negative delay or rate constant indicates the application of
!                a specified fraction of the delay or rate part of the
!                flyby atmosphere calibration used.
!   kdb 970401   Reverse the sign of the EQE CONT contribution (correction to
!                the equation of the equinox to fix a CALC error.) (But only in
!                calc versions < 8.3, the version in which this will be fixed.)
!   kdb 970418   Add a small fudge factor to the reversal of the sign of the
!                EQE CONT contribution to account for the ratio of universal
!                to sidereal time.
!   pet 980203   Substituted hard-coded test of solution type by DATYP_INQ
!   pet 980212   Rewrote comments
!   pet 1999.11.17  Addded three new formal arguemnts: CALIBM, REFFREQ_XS, MCAPL
!                   Added support of mode calibrations.
!   pet 2000.03.27  Improved an error message about ITT error
!   pet 2001.04.30  Fixed the bug: the previous verion worked incorrectly if the
!                   the sky frequency was less than 5 MHz and the opposite
!                   band was not available
!   pet 2001.07.17  Fixed the bug: the previous verion inprocessing S-band data
!                   tried to get square root from a negative number.
!   pet 2002.03.05  Added saving of the atmosphere elay in zenith direction
!                   in variable ATM_ZENDEL
!   kdb 2003.10.21  Rearranged expression from B*-A to ( -B*A )
!   JWR 2004.05.20  Sleep bug in a series of if tests fixed.
!   pet 2008.02.21  Added support of external troposphere path delay
!   pet 2008.04.29  Fixed several bugs in longic for handling 
!                   the external troposphere path delay
!   dsm 2010.01.08  Added computation of hydrostatic zenith delay using
!                   pressure read from external troposphere delay file when the
!                   MAPPING.EXTERNAL_TRP_DELAY is used.
!                   The a priori hydrostatic delay at epochs of the estimated
!                   linear spline can be printed to the spoolfile with the
!                   option APRIORI_ZENDEL
!   JMG 2012.08.21  Calculation of fjday now done outside of routine. 
!                   
!
! 5.  GET_CALIB PROGRAM STRUCTURE
         

100   format("get_calib: ",a," del tot", 1x,4f17.2)     
110   format("get_calib: ",a,"        ", 1x,4f17.4)        

   
      if(kdebug) then 
        open(lu_debug,file=trim(pre_scr_dir)//'DEBUG'//PRE_LETRS,access='Append')      
      endif 
      lkind="On Entry" 
      if(kdebug) write(lu_debug,100) lkind, 0.0d0, 0.0d0,  dt*1.d6, rt*1.d15

!
      ISTA1 = ITT(ISITE(1))
      ISTA2 = ITT(ISITE(2))
!
! --- Apply the observation dependent contributions where requested.
!   
      DO J = 1, cals%l_cont
         IF ( KBIT (OBCAPL, J) ) THEN
              DT_DEL  = CALIBB(1,J)
              RT_del = CALIBB(2,J)
              IF ( J.EQ. IREVCONT ) THEN
!
! ---------------- Kluge: reverse the sign of up to one observation dependent
! ---------------- contribution selected by hard coding in ncort.
! ---------------- Plus use a small fudge factor, the ratio of universal to
! ---------------- sidereal time.)
!
                   dt_del  = -1.0027379 * dt_del
                   rt_del = -1.0027379 * rt_del
              ENDIF
!
              DT = DT + dt_del * 1.0D6
              RT = RT + rt_del
              lkind=Cals%Cont(j)  
              if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d12,rt_del*1.d15,  dt*1.d6, rt*1.d15            
         END IF
      END DO
   
!
! --- Apply the selected non-flyby calibrations:
! --- Loop over stations and across the bits in JCAPPL, and apply the
! --- calibrations where requested.  Signs have been selected in SDBH
! --- such that all calibrations must be ADDED here.
!
! --- First loop over the stations (1 and 2)
! 
! Check to make sure the correspondence table is good. 
      do i=1,2
         ISTAT = ITT(ISITE(I))         
         IF ( ISTAT .LE. 0  .OR. ISTAT .GT. 16384 ) THEN
              WRITE ( 6, * ) ' ISITE = ',ISITE(I),' ITT(ISITE) = ',ITT(ISITE(I))                 
              WRITE ( 6, * ) ' $$$  Station correspondence table ITT is spoiled '// &
     &              'or has not been loaded $$$'
              WRITE ( 6, * ) ' $$$  Such a case may occur when PARFIL has not '// &
     &               'been read  $$$'
              WRITE ( 6, * ) ' $$$  or it corresponds to CGM-type PARFIL  $$$'
              STOP '(GET_CALIB) Abnormal termination'
         END IF
      end do 

! Correspondence table is OK. Get the site names. 
      isite_name(1:4,1)=isitn(1:4,isite(1))
      isite_name(1:4,2)=isitn(1:4,isite(2))       

      DO I = 1, 2
         ISTAT = ITT(ISITE(I))                 
         DO J = 1, MAX_CAL  !across the calibrations
            IF ( KBIT (JCAPPL(ISTAT), J) ) THEN  ! Apply this one                
               dt_del=calibs(i,1,j)
               rt_del=calibs(i,2,j)

               DT = DT + dt_del * 1.0D6
               RT = RT + rt_del
               write(lkind,'(a,1x,a)') lsite_name(i), cals%scal(j)               
               if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d12,rt_del*1.d15,  dt*1.d6,    rt*1.d15
           
            END IF
         END DO
      END DO

    
! --- Apply the selected flyby calibrations:
!
! --- First, if this is a new data base,
! --- locate specific calibrations within the namfil info
! --- This was once done in ncort, but will now be done here, so
! --- that as new flyby calibs are added, users will not have to
! --- keep modifying GET_CALIB's argument list and callers, to keep
! --- passing new id_ pointers.
!
      IF ( .NOT. FL_CAL_SAVE   .OR.  IDB .NE. IDB_SAVE ) THEN
           ID_CFAJJDRY = 0
           ID_CFAKBDRY = 0
           ID_CFAKBWET = 0
           ID_LANYI    = 0
           DO_LANWET   = 0
           ID_MTTDRYSS = 0
           ID_MTTDYFLY = 0
           ID_IFADRYSS = 0
           ID_IFADYFLY = 0
           ID_NMFDYFLY = 0
           ID_NMFWTFLY = 0
!
           DO J = 1, NFCAL
              I = 1
              SET_THIS_ID = .TRUE.
              DO WHILE ( I .LE. NAMSTA  .AND. SET_THIS_ID )
                 IF ( KBIT (JCAFFL(1,I),J)) THEN
                    SET_THIS_ID = .FALSE.
                    IF ( FCAL_NAMES(J) .EQ. 'CFAJJDRY' ) THEN
                         IF ( ID_CFAJJDRY.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_CFAJJDRY = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'CFAKBDRY' ) THEN
                         IF ( ID_CFAKBDRY.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_CFAKBDRY = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'CFAKBWET' ) THEN
                         IF ( ID_CFAKBWET.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_CFAKBWET = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'MTTDRYSS' ) THEN
                         IF ( ID_MTTDRYSS.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_MTTDRYSS = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'MTTDRFLY' ) THEN
                         IF ( ID_MTTDYFLY.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_MTTDYFLY = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'IFADRYSS' ) THEN
                         IF ( ID_IFADRYSS.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_IFADRYSS = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'IFADRFLY' ) THEN
                         IF ( ID_IFADYFLY.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_IFADYFLY = J
                      ELSE IF ( FCAL_NAMES(J) .EQ. 'NMFDRFLY' ) THEN
                         IF ( ID_NMFDYFLY.NE.0 ) TWICE_IN_NAMF = .TRUE.
                         ID_NMFDYFLY = J
!
                      ELSE IF ( FCAL_NAMES(J) .NE. 'MTTWTFLY' ) THEN
                         WRITE ( 6, * ) ' I=',I,' J=',J
                         WRITE ( 6, * ) ' FCAL_NAMES(J) >>',FCAL_NAMES(J),'<<'
                         WRITE ( ERRSTR, '("Invalid flyby cal ",a8,'// &
     &                                   '" in GET_CALIB --  cannot apply")') &
     &                           FCAL_NAMES(J)
                         CALL FERR  ( INT2(201), ERRSTR, INT2(0), INT2(0) )
                         CALL FATAL ( ERRSTR )
                    END IF
                    IF ( TWICE_IN_NAMF ) THEN
                         WRITE ( ERRSTR, '("In apply_calib - a flyby cal ",'// &
     &                           '"appears in namfil twice for one db")' )
                         CALL FERR  ( INT2(1201), ERRSTR, INT2(0), INT2(0) )
                         CALL FATAL (       ERRSTR       )
                    END IF
                 END IF
                 I = I + 1
              END DO
           END DO
!
           IDB_SAVE = IDB
      END IF
      DO_PRCOR = 0

!
! --- Next generate the necessary values for this observation
!
      IF ( ID_CFAKBDRY.NE.0 .OR. ID_CFAJJDRY.NE.0 .OR. &
     &     ID_CFAKBWET.NE.0 .OR. ID_MTTDRYSS.NE.0 .OR. &
     &     ID_MTTDYFLY.NE.0 .OR. ID_NMFDYFLY.NE.0 .OR. &
     &     ID_IFADRYSS.NE.0 .OR. &
     &     ID_IFADYFLY.NE.0                             ) THEN
!
! -------- The subroutine which calculates the cfadry correction needs
! -------- to know whether it is calculating the kbdry or jjdry value
! -------- for the stations in this observation
!
            DO I = 1,2
               ISTAT = ITT(ISITE(I))
               ID_JJ(I) = 0
               MTT_SEAS_WET(I) = .FALSE.  ! default for mttwet is non-seasonal
               MTT_SEAS_DRY(I) = .FALSE.  ! default for mttdry is non-seasonal
               IFA_SEAS_WET(I) = .FALSE.  ! default for ifawet is non-seasonal
               IFA_SEAS_DRY(I) = .FALSE.  ! default for ifadry is non-seasonal

               IF ( ID_CFAJJDRY .NE.0 ) THEN
                 IF( KBIT(JCAFFL(1,ISTAT),ID_CFAJJDRY) ) ID_JJ(I) = 1
               ENDIF

               IF ( ID_MTTDRYSS .NE.0 ) THEN
                 IF(KBIT(JCAFFL(1,ISTAT),ID_MTTDRYSS) ) THEN
!
                    ID_JJ(I) = 2  ! use mttdry & seasonal mapping function.
                    MTT_SEAS_DRY(I) = .TRUE.
                 ENDIF
               END IF

              IF ( ID_MTTDYFLY .NE.0 ) THEN
                IF( KBIT(JCAFFL(1,ISTAT),ID_MTTDYFLY) ) THEN
!
                   ID_JJ(I) = 2  ! use mttdry & unseasonal mapping function.
                   MTT_SEAS_DRY(I) = .FALSE.
                END IF
              ENDIF
              IF ( ID_IFADRYSS .NE.0 ) THEN
                IF( KBIT(JCAFFL(1,ISTAT),ID_IFADRYSS) ) THEN
!
                  ID_JJ(I) = 3  ! use ifadry & seasonal mapping function.
                  IFA_SEAS_DRY(I) = .TRUE.
                END IF
              ENDIF

              IF ( ID_IFADYFLY .NE.0 ) THEN
                IF( KBIT(JCAFFL(1,ISTAT),ID_IFADYFLY) ) THEN
!
                  ID_JJ(I) = 3  ! use ifadry & unseasonal mapping function.
                  IFA_SEAS_DRY(I) = .FALSE.
                 END IF
              ENDIF

              IF ( ID_NMFDYFLY .NE.0 )THEN
                IF( KBIT(JCAFFL(1,ISTAT),ID_NMFDYFLY) ) THEN
!
                  ID_JJ(I) = 4  ! use nmfdry & unseasonal mapping function.
                  IFA_SEAS_DRY(I) = .FALSE.
                END IF
              ENDIF
!
              ID_WET(I)= -1 ! so that we don't use partials in cfacalc.
              IF(TRP_USE == REQ__TRP .or. TRP_USE == USE__TRP) then
                ID_JJ(I) = 4
                CFAKBWET = .FALSE.
              endif 
            END DO

!
            CALL CALCALC ( ISITE, ISITN, ISTAR, VSTARC, AZ, ELEV, PI_VAR, &
     &                     ATMPR, RELHU, TEMPC, VLIGHT, CFAKBDRY, CFAKBWET, &
     &                     ID_JJ, FLYBY_WARNING, LATS, HEIGHTS, AX_OFFS, &
     &                     AX_TYPES, BARO_CALS, BARO_HEIGHTS, APP, &
     &                     MTT_SEAS_DRY, MTT_SEAS_WET, IFA_SEAS_DRY, &
     &                     IFA_SEAS_WET, AXDIF )
      END IF

!Initialize ATM_ZENDEL 

      ATM_ZENDEL(1) = 0.D0
      ATM_ZENDEL(2) = 0.D0
!
!     write(*,*) "STS_VTD ", sts_VTD
!     write(*,*) "TRP_USE ", TRP_USE, REQ__TRP, USE__TRP 
!     pause 
   
      itemp=0
      IF ( STS_VTD == 0                                    .AND. & 
     &     ELEV(1) > EL_MIN  .and. elEV(2) > EL_MIN        .AND. &
     &   ( TRP_USE == REQ__TRP  .OR.  TRP_USE == USE__TRP )     ) THEN
  
!        kdebug_trp=.true. 
        kdebug_trp=.false.
        do i=1,2      
          ltime_tag=jd_to_date_1(fjday,itemp)              
          call get_trp_info(fjday, lsite_name(i),elev(i)/deg2rad,kdebug_trp, az_trp,el_trp, &
    &             P_trp,TempC_trp,slant_trp, map_trp, grad_n_trp, grad_e_trp, &
    &      slant_rate,map_rate,grad_n_rate,grad_e_rate,   ierr_trp)
      
!          write(*,*) ltime_tag, " ", lsite_name(i), slant_trp 
          if(ierr_trp .ne. 0) then
             write(*,*) "get_trp_info error ", ierr_trp
             stop
          endif 
          if(abs(elev(i)-el_trp*deg2rad) .gt. el_tlr .or.  &
    &        abs(az(i)  -az_trp*deg2rad) .gt. el_tlr) then                     
            write(*,'("get_calib: Bad trp az or el for station ", a, " at epoch ", a)') lsite_name(i),  jd_to_date_1(fjday,itemp)
! "get_trp: el or az out of tolerance for station "//lsite_name(i)//" at "//ltime_tag  
            write(*,'("El want, got, dif: ", 3f10.2)') elev(i)/deg2rad,     &
    &                    el_trp, elev(i)/deg2rad-el_trp           
            write(*,*) elev(i)/deg2rad,     &
    &                    el_trp, elev(i)/deg2rad-el_trp    

            write(*,'("AZ want, got, dif: ", 3f10.2)') az(i)/deg2rad,     &
    &                    az_trp, az(i)/deg2rad-az_trp              
          endif 
       
          atm_zendel(i)=slant_trp/map_trp
          if(i .eq. 1) then
             Map_fun(1,1)=-map_trp
             map_fun(1,2)=-map_rate
             dt_del=-slant_trp
             rt_del=-slant_rate
!              write(*,'(a,1x,a,1x,2f16.4)') ltime_tag,lsite_name(1), slant_trp*1.D12, Map_fun(1)         
          else
             map_fun(2,1)=map_trp
             map_fun(2,2)=map_rate
!             write(*,'(a,1x,a,1x,2f16.4)') ltime_tag,lsite_name(2), +slant_trp*1.D12, Map_fun(2)    
             dt_del= slant_trp
             rt_del= slant_rate
          endif 
              DT = DT + dt_del * 1.0D6
              RT = RT + rt_del
              lkind="TRP "
              if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d12,rt_del*1.d15,  dt*1.d6, rt*1.d15            
        end do    
      END IF

!
! --- Loop over stations and across the calibration bits in JCAFFL,
! --- and apply the calibrations where requested.  Signs have been selectedt
! --- in SDBH such that all calibrations must be ADDED here.
!
      DO I = 1, 2   ! Stations 1 and 2
         ISTAT = ITT(ISITE(I))
!        ATM_ZENDEL(I) = 0.0D0
         DO J = 1, NFCAL    !loop over calibrations
            IF ( KBIT (JCAFFL(1,ISTAT), J) ) THEN
              if(j .eq. ID_CFAKBDRY .or. j .eq. ID_NMFDYFLY .or. &
     &           j .eq. ID_MTTDRYSS .or. j .eq. ID_MTTDYFLY .or. &
     &           j .eq. ID_IFADRYSS  .or. j .eq. ID_IFADYFLY) then      
                 ATM_ZENDEL(I) = ATM_ZENDEL(I) + CFAKBDRY(I,1)/APP(I,1)
                 DT_del = CFAKBDRY(I,1)
                 RT_del = CFAKBDRY(I,2)
              else if(j .eq. id_cfakbwet) then               
                ATM_ZENDEL(I) = ATM_ZENDEL(I) + CFAKBWET(I,1)/APP(I,1)
                DT_del = CFAKBWET(I,1) 
                rt_del = CFAKBWET(I,2)
              else 
                WRITE ( 6, * ) ' I=',I,' J=',J,' NFCAL=',NFCAL
                WRITE ( 6, * ) ' ID_NMFDYFLY = ', ID_NMFDYFLY ! %%%
                CALL FERR ( INT2(202), &
     &                   ' - in GET_CALIB - fly cal app problem', INT2(0), INT2(0) )
                CALL FATAL ( ' in GET_CALIB, fly cal app problem' )
              endif 
              write(lkind,'(a,1x," atm")') lsite_name(i) 
              DT = DT + dt_del * 1.0D6
              RT = RT + rt_del                
              if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d12,rt_del*1.d15,  dt*1.d6, rt*1.d15           

            END IF ! This one applied
         END DO ! Calibrations loop
      END DO ! Stations loop
! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
!        call lib$movc3 ( 8, isitn(1,isite(1)), str1 )
!        call lib$movc3 ( 8, isitn(1,isite(2)), str2 )
!        write ( 6, 123 ) str1(1:8), str2(1:8), & ! %%%%%%%%%%%%%%%%%%%%%%%%%%%
!     &                   atm_zendel(1)*1.0d12,  atm_zendel(2)*1.0d12, & ! %%%
!!@     &                   app(1,1), app(2,1), HEIGHTS(ISITE(1)), HEIGHTS(ISITE(2))  ! %%%%%
!     &                   app(1,1), app(2,1), LATS(ISITE(1))/DEG__TO__RAD, LATS(ISITE(2))/DEG__TO__RAD  ! %%%%%
! 123    format ( 'Stations: ',A8,'/',A8,' Atm_zen: ',F10.2, 1x, F10.2/ &
!     &           ' App: ', F10.7, 2X, F10.7, ' hei: ', f14.10, 1x, f14.10 ) ! %%
!        call pause ( 'get_calib' ) ! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
!
! --- Add troposphere noise based on average atmosphere delay
! --- (roughly elevation dependent)
!   
      IF ( KELDEP_NOISE ) THEN
!
! -------- TROP_FRACT converts the atmospheric turbulence and mapping function
! -------- parameter error constants to the formal error units.
! -------- The delay and rate constants are stored in the input elevation
! -------- dependent constants file as picosecs and femtosecs/sec respectively.
!
           TROP_FRACT(1) = 1.D-12  ! Atmospheric turbulence delay
           TROP_FRACT(2) = 1.D-15  ! Atmospheric turbulence rate
           TROP_FRACT(3) = 1.D-12  ! Map funct parm error delay
           TROP_FRACT(4) = 1.D-15  ! Map funct parm error rate
!
! -------- cal_fract converts the flyby atmosphere calibration to the formal
! -------- error units.
!        
           CAL_FRACT(1) = 1.0D6 ! delay
           CAL_FRACT(2) = 1.0D0 ! rate
           DO ICTS = 1,2 ! Sites
!
! ----------- Terms 1-4 are:
! -----------       atmospheric turbulence for delays
! -----------       atmospheric turbulence for rates
! -----------       mapping function parameter error for delays
! -----------       mapping function parameter error for rates
!
              DO ICTT = 1,4
                  IF ( ICTT .LE. 2 .AND. &
     &                 AVG_ATM ( ICTT,ISITE(ICTS) ) .LT. 0.0D0 ) THEN
!
! -------------------- A negative atmospheric turbulence delay/rate constant
! -------------------- indicates that the user wants to apply a fraction
! -------------------- of the delay/rate part of the flyby atmosphere
! -------------------- calibration used in the solution.
! -------------------- (The assumption is that one and only one flyby
! -------------------- calibration is being applied.
! -------------------- If this is false, then if none is applied,
! -------------------- a calibration of 0 is added, and if multiple ones
! -------------------- are applied, the last calibration in the namfil
! -------------------- list is used.)
! -------------------- The fraction to be applied is
! -------------------- the atmospheric turbulence delay/rate "constant" itself.
!
                       ISTAT   = ITT ( ISITE(ICTS) )
                       CAL_ADD = 0.0D0
                       DO J = 1, NFCAL  ! Loop over calibrations
                          IF ( KBIT (JCAFFL(1,ISTAT), J) ) THEN
                               IF ( J .EQ. ID_CFAJJDRY ) THEN
                                    CAL_ADD = CFAKBDRY(ICTS,ICTT)
                                 ELSE IF ( J .EQ. ID_CFAKBDRY ) THEN
                                    CAL_ADD =  CFAKBDRY(ICTS,ICTT)
                                 ELSE IF ( J .EQ. ID_MTTDRYSS .OR. &
     &                                     J .EQ. ID_MTTDYFLY      ) THEN
                                    CAL_ADD =  CFAKBDRY(ICTS,ICTT)
                                 ELSE IF ( J .EQ. ID_IFADRYSS .OR. &
     &                                     J .EQ. ID_IFADYFLY      ) THEN
                                    CAL_ADD =  CFAKBDRY(ICTS,ICTT)
                                 ELSE IF ( J .EQ. ID_NMFDYFLY ) THEN
                                    CAL_ADD =  CFAKBDRY(ICTS,ICTT)
                                 ELSE IF ( J .EQ. ID_CFAKBWET ) THEN
                                    CAL_ADD =  CFAKBWET(ICTS,ICTT)
                               END IF
                          END IF ! This one applied
                       END DO ! Calibrations loop
!
                       ELN(ICTT,ICTS) = CAL_FRACT(ICTT)* &
     &                                  AVG_ATM(ICTT,ISITE(ICTS))*CAL_ADD
                    ELSE
                       ELN(ICTT,ICTS) = TROP_FRACT(ICTT)* &
     &                                  AVG_ATM(ICTT, &
     &                                  ISITE(ICTS))/((DSIN(ELEV(ICTS)))**ICTT)
                  ENDIF
               ENDDO
           ENDDO  
           write(*,*) "get_calib: Elevation dependent noise!!"       
!
           DERR = DSQRT ( DERR**2 + ELN(1,1)**2 + ELN(1,2)**2 + &
     &                              ELN(3,1)**2 + ELN(3,2)**2 )
           RERR = DSQRT ( RERR**2 + ELN(2,1)**2 + ELN(2,2)**2 + &
     &                              ELN(4,1)**2 + ELN(4,2)**2 )
      ENDIF     
     
!
! --- Add ionosphere calibration and modify errors
! This section of the coded substantially re-written. 
! The ionopshere free delay is:
!    Tau_ion_free = Tau_x * freq_x^2/(freq_x^2-freq_s^2) - Tau_s *freq_s^2/(freq_x^2-freq_s^2)
! Which can be written as:
!    Tau_ion_free = Tau_x +  (Tau_x - Tau_S) * freq_s^2/(freq_x^2-freq_s^2)
!                 = Tau_x  + Tau_iono_corr
! The second term is the ionospheric correction = gion(1) with a similar term for the rate.
! The error in this term (gionsig(1) 
!     sig_iono_corr^2= (Sig_X^2+Sig_Y^2)* (freq_s^2/(freq_x^2-freq_s^2))^2
! Previously when computing the error in the Tau_iono_free assumed that
!     sig_tau_iono_free_bad^2 = sig_x^2 + sig_iono_corr^2
! This assumed that Tau_X and Tau_iono_corr were independent. But since Tau_iono_corr depends on Tau_x this is false.
! 
! The correct form for the error in the ionopshere delay is:
!     sig_tau_iono_free = sig_x^2 (freq_x^2/(freq_x^2-freq_s^2))^2 + sig_s^2 * (freq_s^2/(freq_x^2-freq_s^2))^2
!  Note that this depends on having 
!   1.) The x-band and the s_band error.
!   2.) The x_band and the s_band frequencies
!
! In some of the older sessions the s-band terms are not available.
! In this case we can use the following identity:
!     sig_tau_iono_free = sig_x^2 *(1+2.*freq_s^2/(freq_x^2-freq_s^2)+ sig_iono_corr^2
!                       = sig_x^2*(1 + 2.0d/(1-freq_s^2/freq_x^2)    + sig_iono_corr^2
!                       = sig_x^2 + sig_x^2*fix_factor      + sig_iono_corr^2 
! Note that this just depends on:
! 1. sig_x
! 2. sig_iono_corr 
! 3. freq_s^2/freq_x^2
! The first two terms are ALWAYS available. The s-band frequency may not be availalbe. 
! In this case we can make a reasonable assumption. 
!
! Note that if we set fix_factor=0 we recover the incorrect form for the sigma which assumes that there is 
!   the ionopshere correction is UNCORRELATED with the ionosphere delay. 
! 

! Prior to 2012JUL there were several bugs in how the sigmas were calculated.
      IF(IONO_USE == IONO__PRE2012JUL) then
        fix_group=0.d0  
        IF ( EFFREQ_XS .GT. MIN__FRQ  ) THEN          !only calculate if we have S-band frequency 
           fix_group =2.d0* (EFFREQ_XS*EFFREQ_XS)/(EFFREQ*EFFREQ) 
        ENDIF     
        fix_phase = 0.d0
        fix_rate = fix_group        

      ELSE IF(IONO_USE == IONO__NOCORRELATION) then 
! New option.  Assume no correlation
        fix_group = 0.d0 
        fix_rate  = 0.d0 
        fix_phase = 0.d0
      ELSE 
! 2012July27. This calculates things more accurated.
        IF(EFFREQ_XS .GT. MIN__FRQ)  then      !if we have frequency data opposite band, can calculate correction.
          fix_group = 2.d0*(EFFREQ_XS**2)/(EFFREQ**2-EFFREQ_XS**2)
          fix_phase = -2.d0* PHEFFREQ_XS**2 / ( PHEFFREQ**2 - PHEFFREQ_XS**2 )
          fix_rate  = 2.d0* REFFREQ_XS**2  / ( REFFREQ**2  - REFFREQ_XS**2  )  
        else
! We are missing frequency in the other band. Assume default values         
          fix_group = 2.d0*0.079 
          fix_rate  = fix_group
          fix_phase = -fix_group 
        ENDIF    
      ENDIF       
!   
      IONO_APPLIED = .FALSE.
      IF (       KBIT ( JSITI(ISTA1), INT2(4) ) .AND. &
     &     .NOT. KBIT ( JSITI(ISTA1), INT2(5) ) .AND. &
     &           KBIT ( JSITI(ISTA2), INT2(4) ) .AND. &
     &     .NOT. KBIT ( JSITI(ISTA2), INT2(5) )       ) THEN
!
            lkind="Eff freqs X & S group_fix "
            if(kdebug) write(lu_debug,110) "    "//lkind,  effreq, effreq_xs, fix_group 
!
! -------- Group delay data
!
           IF ( DATYP_INQ ( IDATYP, GRPRAT__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, GRPONL__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, SNBRAT__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, SNBONL__DTP )        ) THEN
!
! ------------- Some cases of group delay or narrow-band delay
!
                lkind="giono_sig " 
                if(kdebug) write(lu_debug,100) "    "//lkind,  gionsg(1)*1.d12,gionsg(2)*1.d15 

                lkind="obs_sig before iono" 
                if(kdebug) write(lu_debug,100) "    "//lkind,  derr*1.d12,rerr*1.d15

                dt_del = gion(1)
                rt_del =gion(2) 
                lkind="Iono "                
                DT = DT + dt_del 
                RT = RT + rt_del      

                if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d6,rt_del*1.d15,  dt*1.d6,    rt*1.d15    
              
                IF ( DERR .LT. TAU_ERR__BAD ) THEN
! Calculate the ionosphere free delay based on the exact formula.  Only do if debugging                
                   if(kdebug)then 
                     derr_ion_free =dsqrt( derr**2*(EFFREQ**2/(EFFREQ**2-EFFREQ_XS**2))**2 +  &
           &                             derr_s**2*(EFFREQ_XS**2/(EFFREQ**2-EFFREQ_XS**2))**2)    
                    endif 
! Calculate the ionosphere free delay based on fixing naive formula. should give same result as above.                     
                    DERR = DSQRT ( DABS( DERR**2*(1.d0+fix_group) + GIONSG(1)**2 ) )
                    RERR = DSQRT ( DABS( RERR**2*(1.d0+fix_rate)  + GIONSG(2)**2 ) )         
                   
                    if(kdebug) then
                      write(lu_debug,'(a,3f12.3)') "get_calib:     sig_ion_free sig_fix and dif ", &
           &                        derr_ion_free*1.d12, derr*1.d12, (derr_ion_free-derr)*1.d12
                    endif                       
                     
                END IF
                IONO_APPLIED = .TRUE.     
                lkind="obs_sig after iono" 
                if(kdebug) write(lu_debug,100) "    "//lkind,  derr*1.d12,rerr*1.d15


              ELSE IF ( DATYP_INQ ( IDATYP, PHSRAT__DTP )  .OR. &
     &                  DATYP_INQ ( IDATYP, PHSONL__DTP )        ) THEN
!
! ------------- Some cases of phase delay data
!
                lkind="giono_sig " 
                if(kdebug) write(lu_debug,100) "    "//lkind,  gionsg(1)*1.d12,gionsg(2)*1.d15

                lkind="phase_sig before iono" 
                if(kdebug) write(lu_debug,100) "    "//lkind,  dpher*1.d12,rerr*1.d15

                dt_del = - GION(1)*(EFFREQ**2)/(PHEFFREQ**2)
                rt_del =   GION(2)*(EFFREQ**2)/(REFFREQ**2)
                lkind="giono+Phase delay"
               
                DT = DT + dt_del 
                RT = RT + rt_del    
                if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d6,rt_del*1.d15,  dt*1.d6,    rt*1.d15           
             
                IF ( DPHER .LT. TAU_ERR__BAD ) THEN
                     DPHER = DSQRT ( DPHER**2 + GIONSG(1)**2 )
                     RERR  = DSQRT ( DABS(RERR**2*(1.d0+fix_rate) + GIONSG(2)**2 ) )
                END IF
                lkind="phase_sig after iono" 
                if(kdebug) write(lu_debug,100) "    "//lkind,  dpher*1.d12,rerr*1.d15

                IONO_APPLIED = .TRUE.
              ELSE IF ( DATYP_INQ ( IDATYP, RATONL__DTP ) ) THEN
!
! ------------ Rates only
!
                rt_del =gion(2) 
                lkind="Iono Rate only "                             
                RT = RT + rt_del      
                if(kdebug) write(lu_debug,100) lkind,  0.d0, rt_del*1.d15,  dt*1.d6, rt*1.d15                  
                RERR = DSQRT ( DABS(RERR**2*(1.d0+fix_rate) + GIONSG(2)**2 ) )
                lkind="After iono Rate sig" 
                if(kdebug) write(lu_debug,100) "    "//lkind,  0.d0, rerr*1.d15
                IONO_APPLIED = .TRUE.
           END IF
      END IF
!
! --- Want to apply PHION
!
      IF ( .NOT. KBIT( JSITI(ISTA1), INT2(4) ) .AND. &
     &           KBIT( JSITI(ISTA1), INT2(5) ) .AND. &
     &     .NOT. KBIT( JSITI(ISTA2), INT2(4) ) .AND. &
     &           KBIT( JSITI(ISTA2), INT2(5) )       ) THEN
!
          IF ( DATYP_INQ ( IDATYP, GRPRAT__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, GRPONL__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, SNBRAT__DTP )  .OR. &
     &          DATYP_INQ ( IDATYP, SNBONL__DTP )        ) THEN
!
! ------------- Apply to group delay data. NOTE: Group delay error UNCORRELATED with PHION.Hence no fix_factor
!
                dt_del = PHION*(PHEFFREQ**2)/(EFFREQ**2)
                rt_del = GION(2)
                lkind="Group + Phion"
               
                DT = DT + dt_del 
                RT = RT + rt_del    
                if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d6,rt_del*1.d15,  dt*1.d6,    rt*1.d15              

                DERR  = DSQRT (DERR**2 + PHIONS**2 )                
                RERR  = DSQRT ( DABS(RERR**2 + GIONSG(2)**2 ) )
                IONO_APPLIED = .TRUE.
              ELSE IF ( DATYP_INQ ( IDATYP, PHSRAT__DTP )  .OR. &
     &                  DATYP_INQ ( IDATYP, PHSONL__DTP )        ) THEN
!
! ------------ Apply to phase delay data. NOTE: Phase delay error is CORRELATED with PHION. Hence TEMP_PHASE
!
                dt_del = -PHION
                rt_del = GION(2)*(EFFREQ**2)/(REFFREQ**2)
                lkind="Phase + Phion"
               
                DT = DT + dt_del 
                RT = RT + rt_del    
                if(kdebug) write(lu_debug,100) lkind,  dt_del*1.d6,rt_del*1.d15,  dt*1.d6,    rt*1.d15                    
                RERR  = DSQRT ( DABS(RERR**2*(1.d0+fix_phase) + GIONSG(2)**2 ) )
                IONO_APPLIED = .TRUE.
              ELSE IF ( DATYP_INQ ( IDATYP, RATONL__DTP ) ) THEN
!
! ------------- Rates only
!
                RT    = RT + GION(2)
                RERR  = DSQRT ( DABS(RERR**2*(1.d0+fix_phase) + GIONSG(2)**2 ) )
                IONO_APPLIED = .TRUE.
           END IF
      END IF
 

!
      IF ( MCAPL .NE. 0 ) THEN
           FREQ_GR_X    = EFFREQ*1.D6
           FREQ_GR_S    = EFFREQ_XS*1.D6
           FREQ_PH_X    = PHEFFREQ*1.D6
           FREQ_PH_S    = PHEFFREQ_XS*1.D6
           FREQ_RATE_X  = REFFREQ*1.D6
           FREQ_RATE_S  = REFFREQ_XS*1.D6
!
! -------- We need apply some mode calibrations
!
           DO I=1,M_CLM
              IF ( KBIT ( MCAPL, I ) ) THEN
!
! ---------------- Apply mode calibration to delay in according with solution type
!
                   IF ( DATYP_INQ ( IDATYP, GRPRAT__DTP ) .OR. &
     &                  DATYP_INQ ( IDATYP, SNBRAT__DTP ) .OR. &
     &                  DATYP_INQ ( IDATYP, GRPONL__DTP ) .OR. &
     &                  DATYP_INQ ( IDATYP, SNBONL__DTP )      ) THEN
                        IF ( IONO_APPLIED ) THEN
                             DT = DT + ( CALIBM(MCL__GRX,I)*FREQ_GR_X**2 - &
     &                                   CALIBM(MCL__GRS,I)*FREQ_GR_S**2   )/ &
     &                                 ( FREQ_GR_X**2 - FREQ_GR_S**2 )*1.D6
                          ELSE
                             DT = DT + CALIBM(MCL__GRX,I) * 1.D6
                        END IF
                      ELSE IF ( DATYP_INQ ( IDATYP, PHSRAT__DTP ) .OR. &
     &                          DATYP_INQ ( IDATYP, PHSONL__DTP )      ) THEN
                        IF ( IONO_APPLIED ) THEN
                             DT = DT + ( CALIBM(MCL__PHX,I)*FREQ_PH_X**2 - &
     &                                   CALIBM(MCL__PHS,I)*FREQ_PH_S**2   )/ &
     &                                 ( FREQ_PH_X**2 - FREQ_PH_S**2 )*1.D6
                           ELSE
                             DT = DT + CALIBM(MCL__PHX,I) * 1.D6
                        END IF
                      ELSE IF ( DATYP_INQ ( IDATYP, G_GXS__DTP ) ) THEN
                        DT = DT + ( CALIBM(MCL__GRX,I)*FREQ_GR_X**2 - &
     &                              CALIBM(MCL__GRS,I)*FREQ_GR_S**2   )/ &
     &                            ( FREQ_GR_X**2 - FREQ_GR_S**2 ) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PX_GXS__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHX,I) - &
     &                             ( CALIBM(MCL__GRX,I)*FREQ_GR_X**2 - &
     &                               CALIBM(MCL__GRS,I)*FREQ_GR_S**2   )/ &
     &                             ( FREQ_GR_X**2 - FREQ_GR_S**2 )     )*1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PS_GXS__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHS,I) - &
     &                             ( CALIBM(MCL__GRX,I)*FREQ_GR_X**2 - &
     &                               CALIBM(MCL__GRS,I)*FREQ_GR_S**2   )/ &
     &                             ( FREQ_GR_X**2 - FREQ_GR_S**2 )     )*1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PX_GX__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHX,I)*FREQ_PH_X**2 + &
     &                               CALIBM(MCL__GRX,I)*FREQ_GR_X**2   )/ &
     &                             ( FREQ_PH_X**2 + FREQ_GR_X**2       )*1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PX_GS__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHX,I)*FREQ_PH_X**2 + &
     &                            CALIBM(MCL__GRS,I)*FREQ_GR_S**2)   &
     &                          /( FREQ_PH_X**2 + FREQ_GR_S**2          ) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PS_GX__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHS,I)*FREQ_PH_S**2 + &
     &                               CALIBM(MCL__GRX,I)*FREQ_GR_X**2   )/ &
     &                             ( FREQ_PH_S**2 + FREQ_GR_X**2       )*1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PS_GS__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHS,I)*FREQ_PH_S**2 + &
     &                               CALIBM(MCL__GRS,I)*FREQ_GR_S**2   )/&
     &                             ( FREQ_PH_S**2 + FREQ_GR_S**2       ) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, P_PXS__DTP ) ) THEN
                         DT = DT + ( CALIBM(MCL__PHX,I)*FREQ_PH_X**2 - &
     &                               CALIBM(MCL__PHS,I)*FREQ_PH_S**2   )/ &
     &                             ( FREQ_PH_X**2 - FREQ_PH_S**2       )*1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, GX__DTP ) ) THEN
                         DT = DT + CALIBM(MCL__GRX,I) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, GS__DTP ) ) THEN
                         DT = DT + CALIBM(MCL__GRS,I) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PX__DTP ) ) THEN
                         DT = DT + CALIBM(MCL__PHX,I) * 1.D6
                      ELSE IF ( DATYP_INQ ( IDATYP, PS__DTP ) ) THEN
                         DT = DT + CALIBM(MCL__PHS,I) * 1.D6
                   END IF
!
! ---------------- Apply mode calibration to delay rate in according with
! ---------------- solution type
!
                   IF ( DATYP_INQ ( IDATYP, RATE__DTP   ) .OR. &
     &                  DATYP_INQ ( IDATYP, PHSRAT__DTP ) .OR. &
     &                  DATYP_INQ ( IDATYP, SNBRAT__DTP ) .OR. &
     &                  DATYP_INQ ( IDATYP, RATONL__DTP )      ) THEN
                        IF ( IONO_APPLIED ) THEN
                             RT = RT + ( CALIBM(MCL__RTX,I)*FREQ_GR_X**2 - &
     &                                   CALIBM(MCL__RTS,I)*FREQ_GR_S**2   )/ &
     &                                 ( FREQ_RATE_X**2 - FREQ_RATE_S**2 )
                           ELSE
                             RT = RT + CALIBM(MCL__RTX,I)
                        END IF
                   END IF
              END IF
           END DO
      END IF
 
!
! --- Get the raw observation weights with ionosphere sigmas.
!
      DERR_RAW  = DERR
      RERR_RAW  = RERR
      DPHER_RAW = DPHER
!
! --- Add formal errors
!
! *** switched order 3/6/95  mwh
!
      IF ( ISTA1 .GT. ISTA2 ) THEN     ! swap stations
           I = ISTA1
           ISTA1 = ISTA2
           ISTA2 = I
      END IF
!
! --- Calculate index in packed table which points to error for this
! --- baseline. N is (max # stations per database - 2) = 32 - 2 = 30
!
      N = MAX_ARC_STA - 2
      J = ((ISTA1-1) * N) - (((ISTA1-1) * (ISTA1-2))/2) + ISTA2-1
      JJ = ITTB(J)
!
      IF ( DATYP_INQ ( IDATYP, PHASE__DTP ) ) THEN
           DPHER = SQRT ( DPHER**2 + ET(1,JJ)**2 )
        ELSE
           DERR  = SQRT ( DERR**2  + ET(1,JJ)**2 )
      END IF
!
      RERR = SQRT ( RERR**2 + ET(2, JJ)**2 )
      
      lkind="baseline wts" 
      if(kdebug) write(lu_debug,100) "    "//lkind,  et(1,jj)*1.d12,et(2,jj)*1.d15
      lkind="derr rerr after wts" 
      if(kdebug) write(lu_debug,100) "    "//lkind,  derr*1.d12,rerr*1.d15

     
!
! --- Get source-specific weights and RSS them with current error
!
      IF ( SOURCE_WEIGHT_FILE .NE. ' ' ) THEN
         IF (GET_SOURCE_WEIGHT (ISTRN_CHR, SOURCE_WEIGHT_FILE, SRC_WT)) then  
!
              SRC_WT(1) = SRC_WT(1)*1.D-12
              SRC_WT(2) = SRC_WT(2)*1.D-15
              SRC_WT(3) = SRC_WT(3)*1.D-12
              SRC_WT(4) = SRC_WT(4)*1.D-15
!
              IF ( DATYP_INQ ( IDATYP, PHASE__DTP ) ) THEN
                   DPHER = DSQRT ( DPHER**2 + SRC_WT(3)*SRC_WT(3) )
                   RERR  = DSQRT ( RERR**2  + SRC_WT(4)*SRC_WT(4) )
                ELSE
                   DERR = DSQRT  ( DERR**2  + SRC_WT(1)*SRC_WT(1) )
                   RERR = DSQRT  ( RERR**2  + SRC_WT(2)*SRC_WT(2) )
              ENDIF
           ELSE
              IF ( SOURCE_WEIGHTS .EQ. 're' ) THEN
                   CALL FERR ( INT2(123), 'Source-dependent weight missing', &
     &                         INT2(0), INT2(0) )
              ENDIF
         ENDIF
      ENDIF
!
      IF ( RWT_EL_USE == SOLVE__YES ) THEN 
           IF ( DATYP_INQ ( IDATYP, PHASE__DTP ) ) THEN
                DPHER = DSQRT ( DPHER**2 + (SE(ISTA1)*MAP_FUN(1,1))**2 + &
     &                                     (SE(ISTA2)*MAP_FUN(2,1))**2   )
              ELSE
                DERR = DSQRT  ( DERR**2  + (SE(ISTA1)*MAP_FUN(1,1))**2 + &
     &                                     (SE(ISTA2)*MAP_FUN(2,1))**2   )
           ENDIF
      END IF
      IF ( RWT_SRC_USE == SOLVE__YES ) THEN
           IF ( DATYP_INQ ( IDATYP, PHASE__DTP ) ) THEN
                DPHER = DSQRT ( DPHER**2 + SS(ISTAR)**2 )
              ELSE
                DERR  = DSQRT ( DERR**2  + SS(ISTAR)**2 )
           ENDIF
      END IF
      lkind="On Exit" 
      if(kdebug) write(lu_debug,100) lkind, 0.d0,0.d0, dt*1.d6, rt*1.d15
      if(kdebug) close(lu_debug)

!
! --- Finished
!
      RETURN
      END  !#!  GET_CALIB  #!#
