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


#include <SgIoAgvDriver.h>


/*=====================================================================================================*/
//
// constants:
//


const SgAgvDatumDescriptor
  adNobsSta  ("NOBS_STA", "Number of observations per site",       
    ADS_SESSION, ADT_I4,  SD_NumStn, 1, ACS_ANY),
  adNumbObs  ("NUMB_OBS", "Number of observations in the session",
    ADS_SESSION, ADT_I4,          1, 1, ACS_ANY),
  adNumbSca  ("NUMB_SCA", "Number of scans in the session",
    ADS_SESSION, ADT_I4,          1, 1, ACS_ANY),
  adNumbSta  ("NUMB_STA", "Number of sites",
    ADS_SESSION, ADT_I4,          1, 1, ACS_ANY),
  adObsTab   ("OBS_TAB ", "Observation tables: scan index, indices of the first and the second station",
    ADS_SESSION, ADT_I4,  3, SD_NumObs, ACS_ANY),
  adSiteNames("SITNAMES", "IVS site names",
    ADS_SESSION, ADT_CHAR,8, SD_NumStn, ACS_ANY)
  ;


const SgAgvDatumDescriptor
// strings:  
  adBandNames("BAND_NAM", "Band names",
    ADS_SESSION,   ADT_CHAR,  1, SD_NumBands, ACS_NATIVE),
  adCorrPlace("CORPLACE", "Correlator place name",
    ADS_SESSION,   ADT_CHAR, 32, 1, ACS_NATIVE),
  adCorrType ("COR_TYPE", "Correlator type: MK3 MK4 K4 S2 VLBA MITAKA-1",
    ADS_SESSION,   ADT_CHAR,  8, 1, ACS_NATIVE),
  adExpCode  ("EXP_CODE", "Experiment code",
    ADS_SESSION,   ADT_CHAR, 32, 1, ACS_NATIVE),
  adExpDesc  ("EXP_DESC", "Experiment description",
    ADS_SESSION,   ADT_CHAR, 80, 1, ACS_NATIVE),
  adExpName  ("EXP_NAME", "Experiment program name",
    ADS_SESSION,   ADT_CHAR, 80, 1, ACS_NATIVE),
  adMk3DbName("MK3_DBNM", "Mark-3 DBH database name",
    ADS_SESSION,  ADT_CHAR, 10, 1, ACS_NATIVE),
  adPiName   ("PI_NAME ", "Name of the principal investigator",
    ADS_SESSION,   ADT_CHAR, 80, 1, ACS_NATIVE),
  adRecMode  ("REC_MODE", "Recording mode",
    ADS_SESSION,   ADT_CHAR, 80, 1, ACS_NATIVE),

  adSrcName  ("SRCNAMES", "Source names",
    ADS_SESSION,   ADT_CHAR,  8,SD_NumSrc, ACS_NATIVE),
  adRefClocks("CLK_SITS", "List of clock reference stations",
    ADS_SESSION,   ADT_CHAR,  1, 1, ACS_NATIVE),
  adScanNames("SCANNAME", "Scan name",
    ADS_SCAN,     ADT_CHAR, 16, 1, ACS_NATIVE),
  adCrootFnam("CROOTFIL", "Correlator root file name",
    ADS_SCAN,     ADT_CHAR, 16, 1, ACS_NATIVE),
  adFScanName("FSCANAME", "Full scan name",
    ADS_SCAN,     ADT_CHAR, 16, 1, ACS_NATIVE),

  adQualCodes("QUALCODE", "Quality code as char value: 5-9 is good, 0 -- non-detection, letter -- failure",
    ADS_BASELINE, ADT_CHAR,  1, SD_NumBands, ACS_NATIVE),

  adFourfCmd ("FOURF_CS", "Command string used for fourfit",
    ADS_BASELINE, ADT_CHAR, 16, SD_NumBands, ACS_NATIVE),
  adFourfCtrl("FOURF_CF", "Control file name for fourfit",
    ADS_BASELINE, ADT_CHAR, 16, SD_NumBands, ACS_NATIVE),

  adErrorK   ("ERROR_K ", "Group delay and rate re-weighting constants",
    ADS_SESSION,   ADT_R8,    2, SD_NumBln, ACS_NATIVE),
  adErrorBl  ("ERROR_BL", "B.L.names for formal errors",
    ADS_SESSION,   ADT_CHAR,  1, SD_NumBln, ACS_NATIVE),

  adSrcStatus("SOURSTAT", "Source selection status bit-mapped array",
    ADS_SESSION,  ADT_I2,    SD_NumSrc, 1, ACS_NATIVE),
  adBlnStatus("BASLSTAT", "Baseline selection bit maped array. 1=some obs, etc.",
    ADS_SESSION,  ADT_I2,    SD_NumStn,  SD_NumStn, ACS_NATIVE),
  adBlnClocks("BLDEPCKS", "Bl-dependent clock list",
    ADS_SESSION,  ADT_CHAR,  1, 1, ACS_NATIVE),
  
  
  adObsCalFlg("OBCLFLGS", "Bit set indicate that calibration is recommended for observations",
    ADS_SESSION,  ADT_I2,    1, 1, ACS_NATIVE),
  adStnCalFlg("CAL_FLGS", "Bit set indicate that calibration is recommended for stations",
    ADS_SESSION,  ADT_I2,    SD_NumStn, 1, ACS_NATIVE),
  adFlybyFlg ("FCL_FLGS", "Standard flcal configuration for stations",
    ADS_SESSION,  ADT_I2,    SD_NumStn, 7, ACS_NATIVE),
  adObsCalNam("OBCLLIST", "Available obs dependent calibrations (poletide, earthdide, ?)",
    ADS_SESSION,  ADT_CHAR,  1, 1, ACS_NATIVE),
  adStnCalNam("CAL_LIST", "Station depedendent calibrations (Cable, Phase,  etc?)",
    ADS_SESSION,  ADT_CHAR,  1, 1, ACS_NATIVE),
  adFlyCalNam("FCL_LIST", "Key to the standard flcal config",
    ADS_SESSION,  ADT_CHAR,  1, 1, ACS_NATIVE),
  
  
  // correlator info stuff:
  adFourfErr ("FRNGERR ", "Fourfit error flag blank=OK.",
    ADS_BASELINE, ADT_CHAR,  1, SD_NumBands, ACS_NATIVE),
  adFourfFile("FOURFFIL", "Fourfit output filename.",
    ADS_BASELINE, ADT_CHAR,  1, SD_NumBands, ACS_NATIVE),


// ints:                                                                  
  adExpSerNo ("EXPSERNO", "Experiment serial number at correlator",
    ADS_SESSION,   ADT_I2,  1, 1, ACS_NATIVE),
  adNumSrc   ("NUMB_SOU", "Number of observed sources",
    ADS_SESSION,  ADT_I4,  1, 1, ACS_NATIVE),
  adNumBands ("NUM_BAND", "Number of frequency bands observed in the experiment",
    ADS_SESSION,  ADT_I4,  1, 1, ACS_NATIVE),
  adNumChnTot("NUM_CHAN", "Number of frequency channels at all bands",
    ADS_SESSION,  ADT_I4,  1, 1, ACS_NATIVE),
  adNumChnBnd("NUM_CHBN", "Number of frequency channels per band",
    ADS_SESSION,  ADT_I4,  1, SD_NumBands, ACS_NATIVE),
  adNumAvaBnd("N_AVBAND", "Number of frequency bands for which information is availble",
    ADS_SESSION,  ADT_I4,  1, 1, ACS_GVH),
  adMjdObs   ("MJD_OBS ", "MJD of fringe reference time at pseudo-UTC timecale for the scan (days)",
    ADS_SCAN,     ADT_I4,  1, 1, ACS_NATIVE),
  adSrcIdx   ("SOU_IND ", "Source name index",
    ADS_SCAN,     ADT_I4,  1, 1, ACS_NATIVE),
  adStnIdx   ("STA_IND ", "Station names indexes",
    ADS_BASELINE, ADT_I4,  2, 1, ACS_NATIVE),

  // added:
  adInterval4("INTRVAL4", "First and last UTC time tag in input file.",
    ADS_SESSION,  ADT_I2,  2, 5, ACS_NATIVE),
  adNumLags  ("NLAGS   ", "Num of lags used for correlation",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE),
  adUvf_Asec ("UVF/ASEC", "U V in FR per arcsec from CALC per band",
    ADS_BASELINE, ADT_R8,  SD_NumBands, 2, ACS_NATIVE),
  adBitsSampl("BITSAMPL", "Number of bits per sample",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE, true),
  adNusedChnl("NUSEDCHN", "Number of channels used in bandwidth synthesis per band",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE),

  // clock breaks:
  adCbrNumber("NUM_CLBR", "Number of clock breaks in the experiment",
    ADS_SESSION,  ADT_I4,   1, 1, ACS_NATIVE),
  adCbrSite  ("STA_CLBR", "Names of stations with clock breaks",
    ADS_SESSION,  ADT_CHAR, 1, 1, ACS_NATIVE),
  adCbrImjd  ("MJD_CLBR", "Integer part of MJD of clock break epochs",
    ADS_SESSION,  ADT_I4,   1, 1, ACS_NATIVE),
  adCbrTime  ("UTC_CLBR", "UTC time tag of clock break, seconds since 0:0:0",
    ADS_SESSION,  ADT_R8,   1, 1, ACS_NATIVE),
  // eocb

  // correlator info stuff:
  adStrtOffst("STARTOFF", "Offset nominal start time (sec).",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE),
  adStopOffst("STOP_OFF", "Offset nominal stop time (sec).",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE),
  adHopsVer  ("HOPS_VER", "HOPS software revision number.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 1, ACS_NATIVE),
  adFourFtVer("FOURFVER", "Fourfit version number.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 2, ACS_NATIVE),
  adScanUtc  ("SCAN_UTC", "Nominal scan time YMDHMS.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 6, ACS_NATIVE),
  adUtcMtag  ("UTCM_TAG", "UTC at central epoch YMDHMS.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 6, ACS_NATIVE),
  adCorrUtc  ("CORR_UTC", "UTC time tag of correlation.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 6, ACS_NATIVE),
  adFourUtc  ("FOURFUTC", "Fourfit processing time YMDHMS.",
    ADS_BASELINE, ADT_I2,  SD_NumBands, 6, ACS_NATIVE),
  adTapQcode ("TAPQCODE", "Tape quality code",
    ADS_BASELINE, ADT_CHAR,  1, SD_NumBands, ACS_NATIVE),


  adStnCoord ("SIT_COOR", "Site coordinates in a crust-fixed terrestrial reference system: X, Y, Z (meters)",
    ADS_SESSION,  ADT_R8,  3, SD_NumStn, ACS_NATIVE),
  adSrcCoord ("SOU_COOR", "Source coordinates in a baricenteric reference system: right asc. decl. (rad)",
    ADS_SESSION,  ADT_R8,  2, SD_NumSrc, ACS_NATIVE),
  adUtcMtai  ("UTC_MTAI", "Difference UTC minus TAI at first time tag of the database (sec)",
    ADS_SESSION,  ADT_R8,  1, 1, ACS_NATIVE),
  adAtmInterv("ATM_INTV", "Batchmode atmos interval - hours",
    ADS_SESSION,  ADT_R8, SD_NumStn, 1, ACS_NATIVE),
  adAtmConstr("ATM_CNST", "Atmosphere constraint. ps/hr",
    ADS_SESSION,  ADT_R8, SD_NumStn, 1, ACS_NATIVE),
  adClkInterv("CLK_INTV", "Batchmode clock interval - hours",
    ADS_SESSION,  ADT_R8, SD_NumStn, 1, ACS_NATIVE),
  adClkConstr("CLK_CNST", "Clock constraint-Parts in 1.e14",
    ADS_SESSION,  ADT_R8, SD_NumStn, 1, ACS_NATIVE),

  adUtcObs   ("UTC_OBS ", "Pseudo-UTC time tag of fringe reference time for the scan (sec)",
    ADS_SCAN,     ADT_R8,  1, 1, ACS_NATIVE),
  adDelRate  ("DEL_RATE", "Phase delay rate delays per band (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands, 1, ACS_NATIVE),
  adGrDelAmbg("GDAMBSP ", "Group delay ambiguity spacings per band (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands, 1, ACS_NATIVE),
  adGrDelErr ("GRDELERR", "Group delay errors per band (sec)",
    ADS_BASELINE,  ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adGrDelVal ("GR_DELAY", "Group delays per band (sec)",
    ADS_BASELINE,  ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adGrDelIonV("ION_GDEL", "Ionospheric contribution to group delay at the first band (sec)",
    ADS_BASELINE,  ADT_R8,  1, 1, ACS_GVH),
  adGrDelIonE("ION_GERR", "Uncertainty of ionospheric contribution to group delay at the first band (sec)",
    ADS_BASELINE, ADT_R8,  1, 1, ACS_GVH),
  adPhRatIonV("ION_PRAT", "Ionospheric contribution to phase delay rate at the first band (sec)",
    ADS_BASELINE, ADT_R8,  1, 1, ACS_GVH),
  adPhRatIonE("ION_RERR", "Uncertainty of ionospheric contribution to phase delay rate at the 1st band (d/l)",
    ADS_BASELINE, ADT_R8,  1, 1, ACS_GVH),
  adPhRatErr ("PHRATERR", "Phase delay rate delay errors per band (d/l)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adRefFreq  ("REF_FREQ", "Reference frequency for phase delay per band (Hz)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adSbDelErr ("SBDELERR", "Single-band delay errors per band (sec)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adSbDelVal ("SB_DELAY", "Single-band delays per band (sec)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adSnr      ("SNRATIO ", "Fringe amplitude signal to noise ratio (d/l)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adTotPhase ("TOTPHASE", "Total fringe phases at time of arrival singal at station 1 per band (rad)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),

  adCorrelatn("COHERCOR", "Corr coeff (0 --> 1)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE),
  adApLength ("APLENGTH", "Length of accumul. period in sec",
    ADS_BASELINE, ADT_R8, 1,           1, ACS_NATIVE, true),
  adUnPhaseCl("UNPHASCL", "UnPhaseCal effect, group delay for first and second station",
    ADS_BASELINE, ADT_R8, SD_NumBands, 2, ACS_NATIVE),
  adSamplRate("SAMPLRAT", "Sample rate (Hz)",
    ADS_BASELINE, ADT_R8, SD_NumBands, 1, ACS_NATIVE, true),

  adIonCorr  ("ION_CORR", "Ion correction for delay (sec) and rate (unitless) per band",
    ADS_BASELINE, ADT_R8,   2, SD_NumBands, ACS_NATIVE),
  adIonRms   ("IONRMS  ", "Ion correction sigma for delay (sec) and rate (unitless) per band",
    ADS_BASELINE, ADT_R8,   2, SD_NumBands, ACS_NATIVE),
  adIonDtFlg ("IONDTFLG", "Ion correction data flag per band. 0=OK, -1=Missing, -2=bad",
    ADS_BASELINE, ADT_I2,   SD_NumBands, 1, ACS_NATIVE),

  adEffFreq  ("EFF_FREQ", "Effective ionospheric frequencies for gr.del, ph.del, ph.rate per band (Hz)",
    ADS_BASELINE, ADT_R8,   3, SD_NumBands, ACS_NATIVE),
  adEffFrew  ("EFF_FREW", "Effective equal weighted ionospheric frequencies for gr.del, ph.del, "
                          "ph.rate per band (Hz)",
    ADS_BASELINE, ADT_R8,   3, SD_NumBands, ACS_NATIVE),
  // correlator info stuff:
  adQbFactor ("QBFACTOR", "Measure of uniformity of data.",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adDiscard  ("DISCARD ", "Percent data discarded by FRNGE",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adDeltaEpo ("DELTAEPO", "Offset from center of scan (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adStartSec ("STARTSEC", "Start time in sec past hour",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adStopSec  ("STOP_SEC", "Stop  time in sec past hour",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adSbResid  ("SBRESID ", "Single band delay residual",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adRtResid  ("RATRESID", "Rate resid (sec per sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adEffDura  ("EFF.DURA", "Effective run duration sec",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGcSbDel  ("GC_SBD  ", "Tot geocenter sbd delay (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGcMbDel  ("GC_MBD  ", "Tot geocenter group delay (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGcRate   ("GC_RATE ", "Tot geocenter delay rate (s/s)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGcPhase  ("GC_PHASE", "Tot phase ref to cen of Earth",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGcPhsRes ("GCRESPHS", "Resid phs corrected to cen of E.",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adFalseDet ("FALSEDET", "Prob of false det from FRNGE",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adIncohAmp ("INCOHAMP", "Fr. amp from incoh int of chan.",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adIncohAmp2("INCOH2  ", "Incoh amp from FRNGE plot segs.",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adGrResid  ("DELRESID", "Delay residual (sec).",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),

  adAprioriDl("ABASDEL ", "Corel bas/apr delay (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adAprioriRt("ABASRATE", "Corel bas/apr delay rate (s/s)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adAprioriAc("ABASACCE", "Corel bas/apr accel (1/sec**2)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),

  adUrVr      ("URVR    ", "Rate derivatives mHz per asec",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_NATIVE),
  adIdelay    ("IDELAY  ", "Corel instrumental delay (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_NATIVE),
  adSrchPar  ("SRCHPAR ", "FRNGE/Fourfit search parameters",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 6, ACS_NATIVE),
  adCorrClock("CORCLOCK", "Clock offset ref, rem [sec], rate ref, rem [sec/sec]",                                                            
    ADS_BASELINE, ADT_R8,   SD_NumBands, 4, ACS_NATIVE),
  adCorBasCd ("CORBASCD", "Correlator baseline code (2 ch).",
    ADS_BASELINE, ADT_CHAR,  2, SD_NumBands, ACS_NATIVE),

  adZdelay   ("ZDELAY  ", "Corel zenith atmos. delay (sec).",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_NATIVE),
  adStarElev ("STARELEV", "Elev angles calc by COREL",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_NATIVE),
  // EoCIS

  // phase cal data:
  adPhcRate  ("PHC_RATE", "PC rate by sta ( us per s)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_NATIVE),
  // end of phase cal info


  adIonoBits ("ION_BITS", "ICORR for full ion tracking.",
    ADS_BASELINE, ADT_I2, 1, 1, ACS_NATIVE),
  //
  // Editings:
  adDelayFlag("DELUFLAG", "Delay unweight flag",
    ADS_BASELINE, ADT_I2, 1, 1, ACS_NATIVE),
  adRateFlag ("RATUFLAG", "Delay rate unweight flag",
    ADS_BASELINE, ADT_I2, 1, 1, ACS_NATIVE),
  adPhaseFlag("PHSUFLAG", "Phase delay unweight flag",
    ADS_BASELINE, ADT_I2, 1, 1, ACS_NATIVE),
  adUAcSup  ("UACSUP  ", "User action for suppression",
    ADS_BASELINE, ADT_I2, 1, 1, ACS_NATIVE),
  adNumGrAmbg("N_GRAMB ", "Number of group delay ambiguities to be added to measured "
    "group delays per band",
    ADS_BASELINE,  ADT_I4, SD_NumBands, 1, ACS_NATIVE),
  adNumPhAmbg("N_PHAMB ", "Number of phase delay ambiguities to be added to measured "
    "phase delays per band",
    ADS_BASELINE,  ADT_I4, SD_NumBands,  1, ACS_NATIVE),


  // VGOS-specific:
  adDtec     ("DTEC    ", "Difference of the total electron contents, TEC units",
    ADS_BASELINE, ADT_R8, 1, 1, ACS_NATIVE),
  adDtecAdj  ("DTEC_ADJ", "Adjustement of the differential total electron contents, TEC units",
    ADS_BASELINE, ADT_R8, 1, 1, ACS_NATIVE),
  adDtecSig  ("DTEC_SIG", "Standard deviation of dTec estimation, TEC units",
    ADS_BASELINE,  ADT_R8, 1, 1, ACS_NATIVE),



  adAirTemp  ("AIR_TEMP", "Air temperature at the station (K)",
    ADS_STATION,  ADT_R8,  1, 1, ACS_NATIVE),
  adAirPress ("ATM_PRES", "Atmospheric pressure at the station (Pa)",
    ADS_STATION,  ADT_R8,  1, 1, ACS_NATIVE),
  adRelHumd  ("REL_HUMD", "Relative humidity at the station (0-1)",
    ADS_STATION,  ADT_R8,  1, 1, ACS_NATIVE),
  adCableDel ("CABL_DEL", "Cable delay (sec)",
    ADS_STATION,  ADT_R8,  1, 1, ACS_NATIVE),
  adCableSgn ("CABL_SGN", "Cable sign: +1, 0 or -1",
    ADS_SESSION,  ADT_I2,  SD_NumStn, 1, ACS_NATIVE),

  adCblsSet  ("CBLS_SET", "A set of cable correctons from various sources: FS log, CDMS, PCMT; (sec)",
    ADS_STATION,  ADT_R8,  3, 1, ACS_NATIVE),

  // new:
  adCablOnTp ("CABLONTP", "Cable calibration origin type: FS log file(1), CDMS(2), PCMT(3), ...",
    ADS_SESSION,  ADT_I2,  SD_NumStn, 1, ACS_NATIVE),
  adCablOnTx ("CABLONTX", "Cable calibration origin text",
    ADS_SESSION,  ADT_CHAR,1, SD_NumStn, ACS_NATIVE),
  adMeteOnTp ("METEONTP", "Meteo data origin type: undef(0), FS log file(1), external(2)",
    ADS_SESSION,  ADT_I2,  SD_NumStn, 1, ACS_NATIVE),
  adMeteOnTx ("METEONTX", "Meteo data origin text",
    ADS_SESSION,  ADT_CHAR,1, SD_NumStn, ACS_NATIVE),

  adSrcApRef ("STAR_REF", "Source a priori coordinates reference",
    ADS_SESSION,  ADT_CHAR,8,  SD_NumSrc, ACS_NATIVE),
  adTectPlNm ("TECTPLNM", "4-char tectonic plate names",
    ADS_SESSION,  ADT_CHAR,8,  SD_NumStn, ACS_NATIVE),
  adUtc_Tai  ("UTC_MTAI", "Difference UTC minus TAI at first time tag of the database (sec)",
    ADS_SESSION,  ADT_R8, 1,  1, ACS_NATIVE),


  //4CALC:
  adConsnDel("CONSNDEL", "Consensus theoretical delay, sec",
    ADS_BASELINE,  ADT_R8, 1,  1, ACS_NATIVE),
  adConsnRat("CONSNRAT", "Consensus theoretical rate, sec/sec",
    ADS_BASELINE,  ADT_R8, 1,  1, ACS_NATIVE),
  adCtSite1 ("CT_SITE1", "Coordinate time at site 1",
    ADS_BASELINE,  ADT_R8, 1,  1, ACS_NATIVE),

  adEtdCont ("ETD_CONT", "Earth tide contributions def.",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adOceCont ("OCE_CONT", "Obs dependent ocean loading",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adOceOld  ("OCE_OLD ", "Add to Cal-OceanLoad to get Cal10 OceanLoading",           
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adOptlCont("OPTLCONT", "Ocean Pole Tide Load Contribution",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adPtdCont ("PTD_CONT", "Pole tide contributions def.",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adPtoLdCon("PTOLDCON", "Old Pole Tide Restorer Contrib.",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adTiltRmvr("TILTRMVR", "Axis Tilt Contribution Remover",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adUt1Ortho("UT1ORTHO", "ORTHO_EOP Tidal UT1 contribution",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adWobOrtho("WOBORTHO", "ORTHO_EOP tidal wobble contribtn",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adUt1Libra("UT1LIBRA", "Hi Freq UT1 Libration Contribution",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adWobLibra("WOBLIBRA", "Hi Freq Wobble Libration Contribution",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adWobXcont("WOBXCONT", "X Wobble contribution definition",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adWobYcont("WOBYCONT", "Y Wobble contribution definition",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adConCont ("CON_CONT", "Consensus bending contrib. (sec)",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adSunCont ("SUN_CONT", "Consensus bending contrib. (sec)",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adSun2cont("SUN2CONT", "High order bending contrib.(sec)",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adPlx1pSec("PLX1PSEC", "Parallax partial/contr  1 parsec",
    ADS_BASELINE, ADT_R8, 2,  1, ACS_NATIVE),

  adFeedCor ("FEED.COR", "Feedhorn corr. in CORFIL scheme [per band]",
    ADS_BASELINE, ADT_R8, 2,  2, ACS_NATIVE),
  
  adBendPart("BENDPART", "Grav. bend. partial w.r.t. Gamma",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adWobPart ("WOB_PART", "Wobble partial derivatives def.",
    ADS_BASELINE,  ADT_R8, 2,  2, ACS_NATIVE),
  adUt1Part ("UT1_PART", "UT1 partial derivatives def.",
    ADS_BASELINE,  ADT_R8, 2,  2, ACS_NATIVE),
  adConsPart("CONSPART", "Consensus partial w.r.t. Gamma",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adNut06Xyp("NUT06XYP", "2000/2006 Nut/Prec X,Y Partials",
    ADS_BASELINE,  ADT_R8, 2,  2, ACS_NATIVE),
  adPlxPart ("PLX_PART", "Parallax partial deriv. def.",
    ADS_BASELINE,  ADT_R8, 2,  1, ACS_NATIVE),
  adPtdXyPar("PTDXYPAR", "Pole Tide Partials w.r.t. X & Y",
    ADS_BASELINE,  ADT_R8, 2,  2, ACS_NATIVE),
  adStrPart ("STR_PART", "Star partial derivatives def.",
    ADS_BASELINE,  ADT_R8, 2,  2, ACS_NATIVE),
  adSitPart ("SIT_PART", "Site partials: dtau/dr_1=-dtau/dr_2",
    ADS_BASELINE,  ADT_R8, 2,  3, ACS_NATIVE),
  adSunData ("SUN_DATA", "Solar geocentric coordinates and velocities",
    ADS_SCAN,      ADT_R8, 2,  3, ACS_NATIVE),
  adMunData ("MOONDATA", "Lunar geocentric coordinates and velocities",
    ADS_SCAN,      ADT_R8, 2,  3, ACS_NATIVE),
  adEarthCe ("EARTH_CE", "Earth barycentric coordinates, vels and accs",
    ADS_SCAN,      ADT_R8, 3,  3, ACS_NATIVE),
  adNutWahr ("NUT_WAHR", "Wahr nut vals  - Dpsi Deps&rates",
    ADS_SCAN,      ADT_R8, 2,  2, ACS_NATIVE),
  adNut2006a("NUT2006A", "IAU2006A Nut. - Dpsi  Deps  Rates",
    ADS_SCAN,      ADT_R8, 2,  2, ACS_NATIVE),
  adNut06xys("NUT06XYS", "2000/2006 Nut/Prec X,Y, S & Rates",
    ADS_SCAN,      ADT_R8, 3,  2, ACS_NATIVE),
  adCf2J2k_0("CF2J2K_0", "Crust-fixed to J2000 Rot. Matrix",
    ADS_SCAN,      ADT_R8, 3,  3, ACS_NATIVE),
  adCf2J2k_1("CF2J2K_1", "Crust-fixed to J2000 Rot. Matrix rate",
    ADS_SCAN,      ADT_R8, 3,  3, ACS_NATIVE),
  adCf2J2k_2("CF2J2K_2", "Crust-fixed to J2000 Rot. Matrix accel",
    ADS_SCAN,      ADT_R8, 3,  3, ACS_NATIVE),
  adUt1_tai ("UT1_-TAI", "UT1 time of day for this obsvr.",
    ADS_SCAN,      ADT_R8, 1,  1, ACS_NATIVE),
  adPolarXy ("POLAR_XY", "Polar motion X & Y for obs (rad)",
    ADS_SCAN,      ADT_R8, 2,  1, ACS_NATIVE),

  adElTheo  ("EL-THEO ", "Elevation array definition",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adAzTheo  ("AZ-THEO ", "Azimuth array definition",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adAxoCont ("AXO_CONT", "New Axis Offset Contributions",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adNdryCont("NDRYCONT", "Nhmf (dry) atm. contribution",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adNwetCont("NWETCONT", "Whmf (wet) atm. contribution",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adOceVert ("OCE_VERT", "Site-dep ocean cont - vertical",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adOceHorz ("OCE_HORZ", "Site-dep ocean cont - horizontal",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adOceDeld ("OCE_DELD", "Ocean load site dependent displace",
    ADS_STATION,  ADT_R8, 2,  3, ACS_NATIVE),
  adParangle("PARANGLE", "Feedhorn rot. angle",
    ADS_STATION,  ADT_R8, 1,  1, ACS_NATIVE),
  adAxoPart ("AXO_PART", "Axis Offset partial deriv. def.",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adNgradPar("NGRADPAR", "Niell dry atm. gradient partials",
    ADS_STATION,  ADT_R8, 2,  2, ACS_NATIVE),
  adNdryPart("NDRYPART", "Nhmf2 dry partial deriv. def.",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),
  adNwetPart("NWETPART", "Whmf2 wet partial deriv. def.",
    ADS_STATION,  ADT_R8, 2,  1, ACS_NATIVE),

  adCalcFlgV("CALCFLGV", "CALC flow control flags valu def",
    ADS_SESSION,  ADT_I2,    SD_Any,  1, ACS_NATIVE),
  adCalcFlgN("CALCFLGN", "CALC flow control flags name def",
    ADS_SESSION,  ADT_CHAR, 1,  SD_Any, ACS_NATIVE),
  adAtmMess ("ATM_MESS", "Atmosphere message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adAtmCflg ("ATM_CFLG", "Atmosphere control flag mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adTidalUt1("TIDALUT1", "Flag for tidal terms in UT1 sers",
    ADS_SESSION,  ADT_I2,   1,  1, ACS_NATIVE),
  adCalcVer ("CALC_VER", "CALC version number",
    ADS_SESSION,  ADT_R8,   1,  1, ACS_NATIVE),
  adRelData ("REL_DATA", "Relativity mod data (gamma).",
    ADS_SESSION,  ADT_R8,   1,  1, ACS_NATIVE),
  adPreData ("PRE_DATA", "Precession constant (asec/cent).",
    ADS_SESSION,  ADT_R8,   1,  1, ACS_NATIVE),
  adEtdData ("ETD_DATA", "Earth tide module data (la. h l)",
    ADS_SESSION,  ADT_R8,   3,  1, ACS_NATIVE),
  adAxoMess ("AXO_MESS", "Axis Offset Message Definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adAxoCflg ("AXO_CFLG", "Axis Offset Control flag mes def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adEtdMess ("ETD_MESS", "Earth Tide message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adEtdCflg ("ETD_CFLG", "Earth Tide flow control mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPtdMess ("PTD_MESS", "Pole tide message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPtdCflg ("PTD_CFLG", "Pole tide flow control mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adNutMess ("NUT_MESS", "Nutation flow control mess def.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adNutCflg ("NUT_CFLG", "Nutation message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adOceMess ("OCE_MESS", "Ocean loading message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adOceCflg ("OCE_CFLG", "Ocean load flow control mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adAtiMess ("ATI_MESS", "ATIME Message Definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adAtiCflg ("ATI_CFLG", "ATIME Flow Control Message Def.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adCtiMess ("CTI_MESS", "CTIMG Message Definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adCtiCflg ("CTI_CFLG", "CTIMG Flow Control Message Def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPlxMess ("PLX_MESS", "Parallax message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPlxCflg ("PLX_CFLG", "Parallax flow control mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adStrMess ("STR_MESS", "Star module message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adStrCflg ("STR_CFLG", "Parallax flow control mess def",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adTheMess ("THE_MESS", "Theory module identification",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adRelCflg ("REL_CFLG", "Relativisitc bending use status",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adSitMess ("SIT_MESS", "Site Module Message Definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPanMess ("PAN_MESS", "Feedhorn rot. angle mod. ident.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adPepMess ("PEP_MESS", "PEP Utility Message Definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adWobCflg ("WOB_CFLG", "Wobble flow control mess def.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adUt1Cflg ("UT1_CFLG", "UT1 control flag message def.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adOceStat ("OCE_STAT", "Ocean loading station status.",
    ADS_SESSION,  ADT_CHAR, 1,  SD_NumStn, ACS_NATIVE),
  adUt1Epoch("UT1EPOCH", "TAI - UT1 epoch value definition",
    ADS_SESSION,  ADT_R8,   2,  2, ACS_NATIVE),
  adWobEpoch("WOBEPOCH", "Interpolated wobble array def",
    ADS_SESSION,  ADT_R8,   2,  2, ACS_NATIVE),
  adSiteZens("SITEZENS", "Site zenith path delays (nsec).",
    ADS_SESSION,  ADT_R8,   SD_NumStn,  1, ACS_NATIVE),
  adOPTLCoef("OPTLCOEF", "Ocean Pole Tide loading Coefficients",
    ADS_SESSION,  ADT_R8,   SD_NumStn,  6, ACS_NATIVE),
  adSitOcPhs("SITOCPHS", "Vert ocean loading phases (rad).",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),
  adSitOcAmp("SITOCAMP", "Vert ocean loading ampltudes (m)",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),
  adSitHWOPh("SITHWOPH", "Horz west ocean loading phases (rad).",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),
  adSitHWOAm("SITHWOAM", "Horz west ocean loading ampltudes (m)",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),
  adSitHSOPh("SITHSOPH", "Horz south ocean loading phases (rad).",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),
  adSitHSOAm("SITHSOAM", "Horz south ocean loading ampltudes (m)",
    ADS_SESSION,  ADT_R8,   SD_NumStn, 11, ACS_NATIVE),

  adAxisTyps("AXISTYPS", "Axis type (1-eq 2-xy 3-azel 4 5)",
    ADS_SESSION,  ADT_I2,   SD_NumStn,  1, ACS_NATIVE),
  adAxisOffs("AXISOFFS", "Axis offsets (m).",
    ADS_SESSION,  ADT_R8,   SD_NumStn,  1, ACS_NATIVE),
  adAxisTilt("AXISTILT", "Fixed axis tilt",
    ADS_SESSION,  ADT_R8,   SD_NumStn,  2, ACS_NATIVE),

  adEccTypes("ECCTYPES", "Eccentricity type: XY or NE",
    ADS_SESSION,  ADT_CHAR, 2,  SD_NumStn, ACS_NATIVE),
  adEccCoord("ECCCOORD", "Eccentricity taken from eccentricity file.",
    ADS_SESSION,  ADT_R8,   SD_NumStn,  3, ACS_NATIVE),
  adEccNames("ECCNAMES", "Eccentricity monument name",
    ADS_SESSION,  ADT_CHAR, 10, SD_NumStn, ACS_NATIVE),

  adFwobInf ("FWOB_INF", "Array: (FJD of start, spacing in days, number points)",
    ADS_SESSION,  ADT_R8, 3,  1, ACS_NATIVE),
  adFut1Inf ("FUT1_INF", "Array: (FJD of start, spacing in days, number points, Scaling (should be 1))",
    ADS_SESSION,  ADT_R8, 4,  1, ACS_NATIVE),
  adUt1Intrp("UT1INTRP", "Message for UT1 interp. scheme",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adUt1Mess ("UT1_MESS", "UT1 Module message definition",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adWobMess ("WOB_MESS", "Wobble message definition.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adWobIntrp("WOBINTRP", "Interp. scheme for polar motion.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adFut1Text("FUT1TEXT", "Final Value TAI-UT1 origin text.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adFwobText("FWOBTEXT", "Final Value wobble origin text.",
    ADS_SESSION,  ADT_CHAR, 1,  1, ACS_NATIVE),
  adFut1Pts ("FUT1_PTS", "Final Value TAI-UT1 data points.",
    ADS_SESSION,  ADT_R8,   15,  1, ACS_NATIVE),
  adFwobXnYt("FWOBX&YT", "Final wobble X Y component value",
    ADS_SESSION,  ADT_R8,   15,  2, ACS_NATIVE),




  adPimaCnt  ("PIMA_CNT", "Full path name of pima control file",
    ADS_SESSION,  ADT_CHAR, 1, 1, ACS_GVH_PIMA),
  adPimaVer  ("PIMA_VER", "Version of PIMA software",
    ADS_SESSION,  ADT_CHAR, 1, 1, ACS_GVH_PIMA),
  adSkyFrqChn("SKYFRQCH", "Sky frequency of channels in Hz",
    ADS_SESSION,ADT_R8,    14, 1, ACS_GVH_PIMA),
  adFrnAmpl  ("FRN_AMPL", "Normalized fringe amplitude in range [0, 1]",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_NATIVE),
  adFrTypFit ("FRTYPFIT", "Fringe type. 1-99 reserved for HOPS, 101-199 reserved for PIMA",
    ADS_SESSION,  ADT_I4,   1, 1, ACS_NATIVE),


  // lcodes for the fuzzy descriptors, just list them as known:
  adNumAp1   ("NUM_AP1 ", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adNumAp2   ("NUM_AP2 ", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adCiNum1   ("CI_NUM1 ", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adCiNum2   ("CI_NUM2 ", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adBbcIdx1  ("BBC_IDX1", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adBbcIdx2  ("BBC_IDX2", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adErrRate1 ("ERRATE_1", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adErrRate2 ("ERRATE_2", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_NATIVE),
  adChanId1  ("CHANID1 ", "", ADS_BASELINE,  ADT_CHAR, SD_Any, SD_Any, ACS_NATIVE),
  adChanId2  ("CHANID2 ", "", ADS_BASELINE,  ADT_CHAR, SD_Any, SD_Any, ACS_NATIVE),
  adPolariz1 ("POLARZ1 ", "", ADS_BASELINE,  ADT_CHAR, SD_Any, SD_Any, ACS_NATIVE),
  adPolariz2 ("POLARZ2 ", "", ADS_BASELINE,  ADT_CHAR, SD_Any, SD_Any, ACS_NATIVE),
  adRfFreq1  ("RFREQ1  ", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adRfFreq2  ("RFREQ2  ", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adLoFreq1  ("LO_FREQ1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adLoFreq2  ("LO_FREQ2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adNSampls1 ("NSAMPLS1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adNSampls2 ("NSAMPLS2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adApByFrq1 ("APBYFRQ1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adApByFrq2 ("APBYFRQ2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCFrq1  ("PHCFRQ_1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCFrq2  ("PHCFRQ_2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCAmp1  ("PHCAMP_1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCAmp2  ("PHCAMP_2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCPhs1  ("PHCPHS_1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCPhs2  ("PHCPHS_2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCOff1  ("PHCOFF_1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adPhCOff2  ("PHCOFF_2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_NATIVE),
  adIndChn1  ("IND_CHN1", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_ANY),
  adIndChn2  ("IND_CHN2", "", ADS_BASELINE,  ADT_I2,   SD_Any, SD_Any, ACS_ANY),
  adNumSam1  ("NUM_SAM1", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_ANY),
  adNumSam2  ("NUM_SAM2", "", ADS_BASELINE,  ADT_R8,   SD_Any, SD_Any, ACS_ANY),
  adUvChn1   ("UV_CHN1 ", "", ADS_BASELINE,  ADT_R4,   SD_Any, SD_Any, ACS_ANY),
  adUvChn2   ("UV_CHN2 ", "", ADS_BASELINE,  ADT_R4,   SD_Any, SD_Any, ACS_ANY),
  adPCalFr1  ("PCAL_FR1", "", ADS_STATION,   ADT_R8,   SD_Any, SD_Any, ACS_GVH),
  adPCalFr2  ("PCAL_FR2", "", ADS_STATION,   ADT_R8,   SD_Any, SD_Any, ACS_GVH),
  adPCalCm1  ("PCAL_CM1", "", ADS_STATION,   ADT_R4,   SD_Any, SD_Any, ACS_GVH),
  adPCalCm2  ("PCAL_CM2", "", ADS_STATION,   ADT_R4,   SD_Any, SD_Any, ACS_GVH),





/*
     &     'APBYFRQ1',  'UV_CHN1 ', '        ', GVH__FR2, & !   1
     &     'APBYFRQ2',  'UV_CHN2 ', '        ', GVH__FR2, & !   2
     &     'ATM_CNST',  'ATM_CNS ', '        ', GVH__SL1, & !   3
     &     'ATM_INTV',  'ATM_INTR', '        ', GVH__SL1, & !   4
     &     'BASLSTAT',  'BAS_USE ', 'STA_USE ', GVH__SL1, & !   5
     &     'BLDEPCKS',  'BSCL_EST', '        ', GVH__SL1, & !   6
     &     'CAL_FLGS',  'CAL_STS ', '        ', GVH__SL1, & !   7
     &     'CAL_LIST',  'CAL_NAME', 'CAL_INFO', GVH__CL1, & !   8
     &     'CLK_CNST',  'CLO_CNS ', '        ', GVH__SL1, & !   9
     &     'CLK_INTV',  'CLO_INTR', '        ', GVH__SL1, & !  10
     &     'CLK_SITS',  'STA_CLRF', '        ', GVH__SL1, & !  11
     &     'COHERCOR',  'FRN_AMPL', '        ', GVH__FR2, & !  12
     &     'DELUFLAG',  'USER_REC', '        ', GVH__SL1, & !  13
     &     'EFF.DURA',  'SCAN_DUR', '        ', GVH__FR2, & !  14
     &     'ERROR_BL',  'RWBASNAM', '        ', GVH__SL1, & !  15
     &     'ERROR_K ',  'RWDELVAL', 'RWRATVAL', GVH__SL1, & !  16
     &     'EXP_CODE',  'EXP_CODE', '        ', GVH__FR1, & !  14
     &     'SCANNAME',  'SCANNAME', '        ', GVH__FR1, & !  17
     &     'IONRMS  ',  'ION_GERR', 'ION_RERR', GVH__FR1, & !  18
     &     'ION_CORR',  'ION_GDEL', 'ION_PRAT', GVH__FR1, & !  19
     &     'NSAMPLS1',  'NUM_SAM1', '        ', GVH__FR2, & !  20
     &     'NSAMPLS2',  'NUM_SAM2', '        ', GVH__FR2, & !  21
     &     'NUM_BAND',  'N_AVBAND', '        ', GVH__FR1, & !  22
     &     'PHSUFLAG',  '        ', '        ', GVH__PRC, & !  23
     &     'RFREQ1  ',  'IND_CHN1', 'NUSEDCHN', GVH__FR2, & !  24
     &     'RFREQ2  ',  'IND_CHN2', 'NUSEDCHN', GVH__FR2, & !  25
     &     'QUALCODE',  'QUALCODE', '        ', GVH__FR1, & !  26
     &     'SOURSTAT',  'SOU_USE ', 'SOCO_EST', GVH__SL1, & !  27
     &     'UACSUP  ',  'USER_SUP', '        ', GVH__SL1, & !  28
     &     'UNPHASCL',  'UNPHASCL', '        ', GVH__CL1  & !  29
*/




  adAprCloOf  ("APRCLOOF", "Apriori clock offset used for correlation",
    ADS_STATION,  ADT_R8,   1, 1, ACS_GVH),
  adAprCloRt  ("APRCLORT", "Apriori clock rate used for correlation",
    ADS_STATION,  ADT_R8,   1, 1, ACS_GVH),
  adAprDel    ("APR_DEL ", "Apriori delay (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_GVH),
  adAprRate   ("APR_RATE", "Apriori delay rate (d/l)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_GVH),

  adAzimuth   ("AZIMUTH ", "Apparent source azimuth at both stations of the baseline (rad)",
    ADS_STATION,  ADT_R8,   1, 1, ACS_GVH),
  adElev      ("ELEV    ", "Apparent source elevation at both stations of the baseline (rad)",
    ADS_STATION,  ADT_R8,   1, 1, ACS_GVH),
  adPolariz   ("POLARIZ ", "Polarization label: RR, RL, LR, or LL",
    ADS_BASELINE, ADT_CHAR, SD_NumBands,  1, ACS_GVH),
  adScanDur   ("SCAN_DUR", "Scan duration per band (sec)",
    ADS_BASELINE, ADT_R8,   SD_NumBands,  1, ACS_GVH),
  adStaClRf   ("STA_CLRF", " Names of clock reference stations",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  SD_Any, ACS_GVH),

  
  // tmp:
  adAutoSup   ("AUTO_SUP", "Bit field of automatic suppression status for combination of observables",
    ADS_BASELINE, ADT_I4,   1,   1, ACS_GVH),
  adUserRec   ("USER_REC", "Bit field of analyst defined recovery status for combination "
    "of observables",
    ADS_BASELINE, ADT_I4,   1,   1, ACS_GVH),
  adUserSup   ("USER_SUP", "Bit field of analyst defined suppression status for combination "
    "of observables",
    ADS_BASELINE, ADT_I4,   1,   1, ACS_GVH),
  adSupMet    ("SUPMET  ", "Code of the suppression method used in the solution",
    ADS_SESSION,  ADT_I2,  SD_Any,  SD_Any, ACS_GVH),

  // not used yet:
  adAntGain   ("ANT_GAIN", "Antenna gain per band K/Jy",
    ADS_STATION, ADT_R4,   SD_NumBands, 1, ACS_GVH),
  adAprPhas   ("APR_PHAS", "Apriori fringe phase (rad)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 1, ACS_GVH),
  adAprPhGc   ("APR_PHGC", "Apriori fringe phase per station at the geocenter (rad)",
    ADS_BASELINE, ADT_R8,   SD_NumBands, 2, ACS_GVH),
  adAtmCns    ("ATM_CNS ", "Reciprocal weights of constraints on atm. path delay rate per station, "
    "soltype",
    ADS_SESSION,  ADT_R8,  10,  32, ACS_GVH),
  adAtmIntr   ("ATM_INTR", "Length of time span between spline nodes for atm. path delay per soltyp "
    "(sec)",
    ADS_SESSION,  ADT_R8,  32,   1, ACS_GVH),
  adAprEop    ("APR_EOP ", "Aprori EOP array as Euler angles and its derivatives (rad)",
    ADS_BASELINE, ADT_R8,   3,   2, ACS_GVH),
  adBand2nd   ("BAND_2ND", "Bit field with status of information about the second band observations",
    ADS_BASELINE, ADT_I2,   1,   1, ACS_GVH),
  adBasUse    ("BAS_USE ", "Bit field of baseline selection status",
    ADS_SESSION,  ADT_I4,  SD_NumBln,  1, ACS_GVH),
  adBsclCns   ("BSCL_CNS", "Reciprocal weights of constraints on basdep. clock, per baseline, "
    "per soltype",
    ADS_SESSION,  ADT_R8,  SD_NumBln,  SD_Any, ACS_GVH),
  adBsclEst   ("BSCL_EST", "Estimation status for baseline dependent clock, per baseline",
    ADS_SESSION,  ADT_I4,  SD_NumBln,  1, ACS_GVH),
  adCalSts    ("CAL_STS ", "Bit field of using available calibrations per station, per calibration",
    ADS_SESSION,  ADT_I4,  SD_NumStn,  1, ACS_GVH),
  adCloCns    ("CLO_CNS ", "Reciprocal weights of constraints on clock rate per station, "
    "per solution type",
    ADS_SESSION,  ADT_R8,  SD_NumStn,  SD_Any, ACS_GVH),
  adCloIntr   ("CLO_INTR", "Length of time span between spline nodes for clock function "
    "per soltyp (sec)",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adDaTyp     ("DATYP   ", "Type of the observable or a combination of observables used "
    "in the solution",
    ADS_SESSION,  ADT_I2,  SD_Any,  SD_Any, ACS_GVH),
  adDerDel    ("DER_DEL ", "Array of partial derivatives of theoretical path delay wrt parameters of "
    "the mod",
    ADS_BASELINE, ADT_R8,   SD_Any,   SD_Any, ACS_GVH),
  adDerRat    ("DER_RAT ", "Array of partial derivatives of theoretical delay rate wrt parameters of "
    "the mod",
    ADS_BASELINE, ADT_R8,   SD_Any,   SD_Any, ACS_GVH),
  adDgClEst   ("DGCL_EST", "Degree of global clock function polynomial per station, per solution type",
    ADS_SESSION,  ADT_I2,  SD_Any,  SD_Any, ACS_GVH),
  adEditSts   ("EDIT_STS", "Bit field of database editing status for different types of solutions",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adEopCns    ("EOP_CNS ", "Reciprocal weights of constraints on EOP related parameters",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adEopEst    ("EOP_EST ", "Estimation status for EOP-related parameters",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adRwBasNam  ("RWBASNAM", "Baseline names for additive baseline-dependent reweighting parameters",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  SD_Any, ACS_GVH),
  adRwDelVal  ("RWDELVAL", "Additive baseline-dependent reweighting parameters for delays (sec)",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adRwRatVal  ("RWRATVAL", "Additive baseline-dependent reweighting parameters for delay rates (d/l)",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adSoCoCns   ("SOCO_CNS", "Reciprocal weights of constraints on source coordinates per object, "
    "per soltype",
    ADS_SESSION,  ADT_R8,  SD_NumSrc,  SD_Any, ACS_GVH),
  adSoCoEst   ("SOCO_EST", "Estimation status for source coordinates per component, per object",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adSouUse    ("SOU_USE ", "Bit field of source selection status",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adStaUse    ("STA_USE ", "Bit field of station selection status",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adStPsCns   ("STPS_CNS", "Reciprocal weights of constraints on site positions per site, "
    "per solution type",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adStPsEst   ("STPS_EST", "Estimation status for station positions per component, per station",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adTecSts    ("TEC_STS ", "Flag of availability/usage of the external ionosphere calibration",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adTilIntr   ("TIL_INTR", "Length of time span between spline nodes for atmosphere tilt "
    "per soltyp (sec)",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adTlOfCns   ("TLOF_CNS", "Reciprocal weights of constraints on atm. tilt offset per station, "
    "per soltype",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adTlRtCns   ("TLRT_CNS", "Reciprocal weights of constraints on atm. tilt rate per station, "
    "per soltype",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),


  adCalInfo   ("CAL_INFO", "Information about class and type of available calibrations",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adCalName   ("CAL_NAME", "Name of available calibrations",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  SD_Any, ACS_GVH),
  adMeanCabl  ("MEANCABL", "Mean cable delay (sec)",
    ADS_SESSION,  ADT_R8,  SD_NumStn,  1, ACS_GVH),
  adNcalib    ("N_CALIB ", "Number of available calibrations",
    ADS_SESSION,  ADT_I2,  SD_Any,  SD_Any, ACS_GVH),
  adEopTab    ("EOP_TAB ", "Table of aprori EOP as Euler angles with frequencies > 2 cpd filtered out",
    ADS_SESSION,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adMjdEop    ("MJD_EOP ", "Modified Julian date of the first epoch for the table of apriori EOP",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adNutDer    ("NUT_DER ", "Partial derivatives wth nutation deaily offset parameters (sec)",
    ADS_BASELINE,  ADT_R8,  SD_Any,  SD_Any, ACS_GVH),
  adNaprEop   ("N_APREOP", "Number of nodes with apriori EOP",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adStepEop   ("STEP_EOP", "Step of the EOP table of apriori EOP (sec)",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adTaiEop    ("TAI_EOP ", "TAI time tag of first epoch of the table of apriori EOP (sec)",
    ADS_SESSION,  ADT_I4,  SD_Any,  SD_Any, ACS_GVH),
  adThGrDel   ("THGR_DEL", "Theoretical group delay (sec)",
    ADS_BASELINE,  ADT_R8,  1,  1, ACS_GVH),
  adThPhDel   ("THPH_DEL", "Theoretical phase delay (sec)",
    ADS_BASELINE,  ADT_R8,  1,  1, ACS_GVH),
  adThPhRat   ("THPH_RAT", "Theoretical phase delay date (d/l)",
    ADS_BASELINE,  ADT_R8,  1,  1, ACS_GVH),
  adThProg    ("TH_PROG ", "Name and version of the program which computed theoretical path delays",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  SD_Any, ACS_GVH),
  adThRuDat   ("TH_RUDAT", "Date and time of theoretical delay compuation",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  SD_Any, ACS_GVH),
  adUvCoord   ("UV_COOR ", "UV coordinates of the baseline vector projection calibration",
    ADS_BASELINE,  ADT_R8,  2,  1, ACS_GVH),

  adRateCen   ("RATE_CEN", "Rate window center used for fringe search (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adRateWdt   ("RATE_WDT", "Rate window width used for fringe search (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResGrRat  ("RESGRRAT", "Residual group delay rate (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResMbDel  ("RESMBDEL", "Residual multiband group delay (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResPhas   ("RESPHAS ", "Residual fringe phase",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResPhRat  ("RESPHRAT", "Residual phase delay rate (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResSbDel  ("RESSBDEL", "Residual singleband group delay (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResGrDl   ("RES_GRDL", "Residual group delay reported by the post-correlator software (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResPhGc   ("RES_PHGC", "Residual fringe phase per station at the geocenter (rad)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adResRate   ("RES_RATE", "Apriori phase delay rate reported by the post-correlator software (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adSpChWdt   ("SPCH_WDT", "Spectral channel width in Hz per channel",
    ADS_SESSION,  ADT_R8,  SD_Any,  1, ACS_GVH),
  adSrtOffs   ("SRT_OFFS", "Scan reference time offset relative to the scan start (sec)",
    ADS_SCAN,     ADT_R8,  1,  1, ACS_GVH),
  adTsys1     ("TSYS1   ", "System temperature per channel at the 1st band (K)",
    ADS_STATION,  ADT_R4,  1,  SD_Any, ACS_GVH),
  adTsys2     ("TSYS2   ", "System temperature per channel at the 2nd band (K)",
    ADS_STATION,  ADT_R4,  1,  SD_Any, ACS_GVH),
  adUvStaOrd  ("UVSTAORD", "Original station order in the baseline: 1 (ascending) or -1 (descending)",
    ADS_BASELINE, ADT_I2,  1,  1, ACS_GVH),


// can be useful:
  adChanSdb   ("CHAN_SDB", "Index of lower (-1) or upper (1) sideband per channel",
    ADS_SESSION,  ADT_I2,  SD_Any,  1, ACS_GVH),
  adChanWdt   ("CHAN_WDT", "Frequency channel width in Hz per channel",
    ADS_SESSION,  ADT_R8,  SD_Any,  1, ACS_GVH),
  adDelWcen   ("DELW_CEN", "Delay window center used for fringe search (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adDelWwdt   ("DELW_WDT", "Delay window width used for fringe search (sec)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adNoiseRms  ("NOISERMS", "RMS of the fringe amplitude noise (d/l)",
    ADS_BASELINE,  ADT_R4,  SD_NumBands,  1, ACS_GVH),
  adPindObs   ("PIND_OBS", "Internal index of observation used by PIMA",
    ADS_BASELINE,  ADT_I4,  1,  1, ACS_GVH),

  adCorVers   ("COR_VERS", "Correlator software and/or hardware version",
    ADS_SESSION,  ADT_CHAR,  SD_Any,  1, ACS_GVH),
  adFrtOffs   ("FRT_OFFS", "Fringe reference time offset relative to the scan start (sec)",
    ADS_SCAN,     ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adGrRatErr  ("GRRATERR", "Group delay rate errors per band (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adGrRate    ("GR_RATE ", "Group delays rate per band (d/l)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adNumClRf   ("NUM_CLRF", "Number of clock reference stations",
    ADS_SESSION,  ADT_I4,  1,  1, ACS_GVH),
  adPhDelErr  ("PHDELERR", "Phase delay error (rad)",
    ADS_BASELINE,  ADT_R8,  SD_NumBands,  1, ACS_GVH),
  adScanPima  ("SCANPIMA", "Pima internal scan name",
    ADS_SCAN,     ADT_CHAR,  SD_Any,  1, ACS_GVH),




// end of everything:
  adNone     ("NONE", "N/A",
    ADS_NONE,    ADT_NONE, 0, 0, ACS_NONE);


const QString SgAgvChunk::magicPrefix_("VGOSDA Format of ");
const QString SgAgvChunk::currentFmtVersion_("2019.09.09");







/*=====================================================================================================*/

/*=====================================================================================================*/
