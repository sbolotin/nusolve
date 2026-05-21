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


#ifndef SG_CONSTANTS_H
#define SG_CONSTANTS_H

#include <math.h>


#include <QtCore/QMap>



//!< radians to degrees:
#ifndef RAD2DEG
#define RAD2DEG         (180.0/M_PI)
#endif  
//!< radians to hours:
#ifndef RAD2HR
#define RAD2HR          (12.0 /M_PI)
#endif
//!< degrees to radians:
#ifndef DEG2RAD
#define DEG2RAD         (M_PI/180.0)
#endif
//!< degrees to seconds:
#ifndef DEG2SEC
#define DEG2SEC         (3600.0)
#endif
//!< hours to radians:
#ifndef HR2RAD
#define HR2RAD          (M_PI/ 12.0)
#endif
//!< arc seconds to radians:
#ifndef SEC2RAD
#define SEC2RAD         (DEG2RAD/3600.0)
#endif
//!< radians to arc seconds:
#ifndef RAD2SEC
#define RAD2SEC         (RAD2DEG*3600.0)
#endif
//!< radians to mas:
#ifndef RAD2MAS
#define RAD2MAS         (RAD2SEC*1000.0)
#endif
//!< radians to ms:
#ifndef RAD2MS
#define RAD2MS          (RAD2HR*3600.0*1000.0)
#endif



// some physical constants:

extern const double vLight;



enum PimaAutoSuppress
{
/*
  PAS_BQCX__SPS = 1<<  0,  //  Bad quality code for X-band
  PAS_BQCS__SPS = 1<<  1,  //  Bad quality code for S-band
  PAS_NOFX__SPS = 1<<  2,  //  No fringes for X-band
  PAS_NOFS__SPS = 1<<  3,  //  No fringes for S-band
  PAS_CUEL__SPS = 1<<  4,  //  Observation made below elevation cut off limit
  PAS_DSBS__SPS = 1<<  5,  //  Observation at deselected baseline
  PAS_DSSO__SPS = 1<<  6,  //  Observation of deselected source
  PAS_BWVR__SPS = 1<<  7,  //  Bad WVR mask
  PAS_BPRN__SPS = 1<<  8,  //  No parangle correction available
  PAS_GION__SPS = 1<<  9,  //  GION calibration is not available *compatibility*
  PAS_GIO1__SPS = 1<< 10,  //  GION calibration is bad           *compatibility*
  PAS_GIO2__SPS = 1<< 11,  //  GION calibration is bad           *compatibility*
  PAS_GIO3__SPS = 1<< 12,  //  GION calibration is bad           *compatibility*
  PAS_GIO4__SPS = 1<< 13,  //  GION calibration is bad           *compatibility*
  PAS_PION__SPS = 1<< 14,  //  PION calibration is not available *compatibility*
  PAS_PIO1__SPS = 1<< 15,  //  PION calibration is bad           *compatibility*
  PAS_PIO2__SPS = 1<< 16,  //  PION calibration is bad           *compatibility*
// !!
  PAS_PIO3__SPS = 1<< 17,  //  PION calibration is bad           *compatibility*
  PAS_EXTS__SPS = 1<< 17,  //  Suppress by the flagging in the external file
// !!
  PAS_LSNR__SPS = 1<< 18,  //  Low SNR
  PAS_FURE__SPS = 1<< 19,  //  Recoverable for FUSED data type
  PAS_XAMB__SPS = 1<< 20,  //  X-band phase ambiguity not resolved
  PAS_SAMB__SPS = 1<< 21,  //  S-band phase ambiguity not resolved
  PAS_IUNW__SPS = 1<< 22,  //  IUNW code is not zero
  PAS_WPAS__SPS = 1<< 23,  //  Wrong phase delay ambiguity spacings
  PAS_IOUS__SPS = 1<< 24,  //  Ionospheric calibration is used if available
  PAS_DECM__SPS = 1<< 25,  //  Decimation is set on
  PAS_INIT__SPS = 1<< 26,  //  The bit field has been initialized
  PAS_SET1__SPS = 1<< 27,  //  Circumstnaces bits are set up
  PAS_SET2__SPS = 1<< 28,  //  Usage status bits are set up
  PAS_GOOD__SPS = 1<< 29,  //  Observation is marked as good
  PAS_CBAD__SPS = 1<< 30,  //  Observation is marked as conditionally bad, but may become recoverable
  PAS_UNRC__SPS = 1<< 31,  //  Observation is marked as bad and may never become recoverable
  oops, the counting actually starts from zero:
*/
  PAS_BQCX__SPS = 1 <<  1,  //  Bad quality code for X-band
  PAS_BQCS__SPS = 1 <<  2,  //  Bad quality code for S-band
  PAS_NOFX__SPS = 1 <<  3,  //  No fringes for X-band
  PAS_NOFS__SPS = 1 <<  4,  //  No fringes for S-band
  PAS_CUEL__SPS = 1 <<  5,  //  Observation made below elevation cut off limit
  PAS_DSBS__SPS = 1 <<  6,  //  Observation at deselected baseline
  PAS_DSSO__SPS = 1 <<  7,  //  Observation of deselected source
  PAS_BWVR__SPS = 1 <<  8,  //  Bad WVR mask
  PAS_BPRN__SPS = 1 <<  9,  //  No parangle correction available
  PAS_GION__SPS = 1 << 10,  //  GION calibration is not available *compatibility*
  PAS_GIO1__SPS = 1 << 11,  //  GION calibration is bad           *compatibility*
  PAS_GIO2__SPS = 1 << 12,  //  GION calibration is bad           *compatibility*
  PAS_GIO3__SPS = 1 << 13,  //  GION calibration is bad           *compatibility*
  PAS_GIO4__SPS = 1 << 14,  //  GION calibration is bad           *compatibility*
  PAS_PION__SPS = 1 << 15,  //  PION calibration is not available *compatibility*
  PAS_PIO1__SPS = 1 << 16,  //  PION calibration is bad           *compatibility*
  PAS_PIO2__SPS = 1 << 17,  //  PION calibration is bad           *compatibility*
// !!
  PAS_PIO3__SPS = 1 << 18,  //  PION calibration is bad           *compatibility*
  PAS_EXTS__SPS = 1 << 18,  //  Suppress by the flagging in the external file
// !!
  PAS_LSNR__SPS = 1 << 19,  //  Low SNR
  PAS_FURE__SPS = 1 << 20,  //  Recoverable for FUSED data type
  PAS_XAMB__SPS = 1 << 21,  //  X-band phase ambiguity not resolved
  PAS_SAMB__SPS = 1 << 22,  //  S-band phase ambiguity not resolved
  PAS_IUNW__SPS = 1 << 23,  //  IUNW code is not zero
  PAS_WPAS__SPS = 1 << 24,  //  Wrong phase delay ambiguity spacings
  PAS_IOUS__SPS = 1 << 25,  //  Ionospheric calibration is used if available
  PAS_DECM__SPS = 1 << 26,  //  Decimation is set on
  PAS_INIT__SPS = 1 << 27,  //  The bit field has been initialized
  PAS_SET1__SPS = 1 << 28,  //  Circumstnaces bits are set up
  PAS_SET2__SPS = 1 << 29,  //  Usage status bits are set up
  PAS_GOOD__SPS = 1 << 30,  //  Observation is marked as good
  PAS_CBAD__SPS = 1 << 31,  //  Observation is marked as conditionally bad, but may become recoverable
  //PAS_UNRC__SPS = 1<< 32,  //  Observation is marked as bad and may never become recoverable

};
/*
enum PimaSolutionType
{
  PAS_UNRC__SPS   = 1 <<  0, // Observation is marked as bad and may never become recoverable
  PST_GRPRAT__DTP = 1 <<  1, // GRPRAT__DTC='Group delay & rate  ' )
   PST_PHSRAT__DTP = 1 <<  2, // PHSRAT__DTC='Phase delay & rate  ' )
   PST_SNBRAT__DTP = 1 <<  3, // SNBRAT__DTC='N.Band delay & rate ' )
  PST_GRPONL__DTP = 1 <<  4, // GRPONL__DTC='Group delay only    ' )
   PST_PHSONL__DTP = 1 <<  5, // PHSONL__DTC='Phase delay only    ' )
   PST_SNBONL__DTP = 1 <<  6, // SNBONL__DTC='N.Band delay only   ' )
   PST_RATONL__DTP = 1 <<  7, // RATONL__DTC='Rate only           ' )
  PST_G_GXS__DTP  = 1 <<  8, // G_GXS__DTC='G-Gxs combination   ' )
  PST_PX_GXS__DTP = 1 <<  9, // PX_GXS__DTC='Px-Gxs combination  ' )
  PST_PS_GXS__DTP = 1 << 10, // PS_GXS__DTC='Ps-Gxs combination  ' )
  PST_PX_GX__DTP  = 1 << 11, // PX_GX__DTC='Px-Gx combination   ' )
  PST_PX_GS__DTP  = 1 << 12, // PX_GS__DTC='Px-Gs combination   ' )
  PST_PS_GX__DTP  = 1 << 13, // PS_GX__DTC='Ps-Gx combination   ' )
  PST_PS_GS__DTP  = 1 << 14, // PS_GS__DTC='Ps-Gs combination   ' )
   PST_P_PXS__DTP  = 1 << 15, // P_PXS__DTC='P-Pxs combination   ' )
  PST_GX__DTP     = 1 << 16, // GX__DTC='Group delay X-band  ' )
  PST_GS__DTP     = 1 << 17, // GS__DTC='Group delay S-band  ' )
  PST_PX__DTP     = 1 << 18, // PX__DTC='Phase delay X-band  ' )
   PST_PS__DTP     = 1 << 19, // PS__DTC='Phase delay S-band  ' )
   PST_SNG_X__DTP  = 1 << 20, // SNG_X__DTC='Single band X-band  ' )
  PST_SNG_S__DTP  = 1 << 21, // SNG_S__DTC='Single band S-band  ' )
  PST_FUSED__DTP  = 1 << 22, // FUSED__DTC='Fused group delays' )
};
*/

enum PimaSolutionType
{
  PAS_UNRC__SPS   = 1 << 23, // Observation is marked as bad and may never become recoverable
  PST_GRPRAT__DTP = 1 <<  0, // GRPRAT__DTC='Group delay & rate  ' )
  PST_PHSRAT__DTP = 1 <<  1, // PHSRAT__DTC='Phase delay & rate  ' )
  PST_SNBRAT__DTP = 1 <<  2, // SNBRAT__DTC='N.Band delay & rate ' )
  PST_GRPONL__DTP = 1 <<  3, // GRPONL__DTC='Group delay only    ' )
  PST_PHSONL__DTP = 1 <<  4, // PHSONL__DTC='Phase delay only    ' )
  PST_SNBONL__DTP = 1 <<  5, // SNBONL__DTC='N.Band delay only   ' )
  PST_RATONL__DTP = 1 <<  6, // RATONL__DTC='Rate only           ' )
  PST_G_GXS__DTP  = 1 <<  7, // G_GXS__DTC='G-Gxs combination   ' )
  PST_PX_GXS__DTP = 1 <<  8, // PX_GXS__DTC='Px-Gxs combination  ' )
  PST_PS_GXS__DTP = 1 <<  9, // PS_GXS__DTC='Ps-Gxs combination  ' )
  PST_PX_GX__DTP  = 1 << 10, // PX_GX__DTC='Px-Gx combination   ' )
  PST_PX_GS__DTP  = 1 << 11, // PX_GS__DTC='Px-Gs combination   ' )
  PST_PS_GX__DTP  = 1 << 12, // PS_GX__DTC='Ps-Gx combination   ' )
  PST_PS_GS__DTP  = 1 << 13, // PS_GS__DTC='Ps-Gs combination   ' )
  PST_P_PXS__DTP  = 1 << 14, // P_PXS__DTC='P-Pxs combination   ' )
  PST_GX__DTP     = 1 << 15, // GX__DTC='Group delay X-band  ' )
  PST_GS__DTP     = 1 << 16, // GS__DTC='Group delay S-band  ' )
  PST_PX__DTP     = 1 << 17, // PX__DTC='Phase delay X-band  ' )
  PST_PS__DTP     = 1 << 18, // PS__DTC='Phase delay S-band  ' )
  PST_SNG_X__DTP  = 1 << 19, // SNG_X__DTC='Single band X-band  ' )
  PST_SNG_S__DTP  = 1 << 20, // SNG_S__DTC='Single band S-band  ' )
  PST_FUSED__DTP  = 1 << 21, // FUSED__DTC='Fused group delays' )
};


//   3         2         1
// 21098765432109876543210987654321
// 01001000000000000000000000000000
// 01001000000000010000000000000000

//PHSRAT__DTP  SNBRAT__DTP  PHSONL__DTP  SNBONL__DTP  RATONL__DTP  P_PXS__DTP  PS__DTP  SNG_X__DTP


enum SgSidebandOrder
{
  SO_UNDEF   = 0,
  SO_USB_LSB = 1,
  SO_LSB_USB = 2,
};



extern const QMap<PimaAutoSuppress, QString> pasAbbrByType;
extern const QMap<PimaSolutionType, QString> pstAbbrByType;

extern const QMap<SgSidebandOrder, QString> sidebandOrderByType;



#endif //SG_CONSTANTS_H
