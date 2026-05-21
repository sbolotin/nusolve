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

#include <iostream>



#include <SgConstants.h>



/*=====================================================================================================*/
//
// constants:
//

const double                    vLight = 299792458.0; // m/s


const QMap<PimaAutoSuppress, QString> pasAbbrByType
{
  {PAS_BQCX__SPS, "BQCX__SPS"},
  {PAS_BQCS__SPS, "BQCS__SPS"},
  {PAS_NOFX__SPS, "NOFX__SPS"},
  {PAS_NOFS__SPS, "NOFS__SPS"},
  {PAS_CUEL__SPS, "CUEL__SPS"},
  {PAS_DSBS__SPS, "DSBS__SPS"},
  {PAS_DSSO__SPS, "DSSO__SPS"},
  {PAS_BWVR__SPS, "BWVR__SPS"},
  {PAS_BPRN__SPS, "BPRN__SPS"},
  {PAS_GION__SPS, "GION__SPS"},
  {PAS_GIO1__SPS, "GIO1__SPS"},
  {PAS_GIO2__SPS, "GIO2__SPS"},
  {PAS_GIO3__SPS, "GIO3__SPS"},
  {PAS_GIO4__SPS, "GIO4__SPS"},
  {PAS_PION__SPS, "PION__SPS"},
  {PAS_PIO1__SPS, "PIO1__SPS"},
  {PAS_PIO2__SPS, "PIO2__SPS"},
// !!
//PAS_PIO3__SPS PAS_PIO3__SPS 
  {PAS_EXTS__SPS, "EXTS__SPS"},
// !!
  {PAS_LSNR__SPS, "LSNR__SPS"},
  {PAS_FURE__SPS, "FURE__SPS"},
  {PAS_XAMB__SPS, "XAMB__SPS"},
  {PAS_SAMB__SPS, "SAMB__SPS"},
  {PAS_IUNW__SPS, "IUNW__SPS"},
  {PAS_WPAS__SPS, "WPAS__SPS"},
  {PAS_IOUS__SPS, "IOUS__SPS"},
  {PAS_DECM__SPS, "DECM__SPS"},
  {PAS_INIT__SPS, "INIT__SPS"},
  {PAS_SET1__SPS, "SET1__SPS"},
  {PAS_SET2__SPS, "SET2__SPS"},
  {PAS_GOOD__SPS, "GOOD__SPS"},
  {PAS_CBAD__SPS, "CBAD__SPS"}

};



const QMap<PimaSolutionType, QString> pstAbbrByType
{
  {PAS_UNRC__SPS,   "UNRC__SPS"},
  {PST_GRPRAT__DTP, "GRPRAT__DTP"},
  {PST_PHSRAT__DTP, "PHSRAT__DTP"},
  {PST_SNBRAT__DTP, "SNBRAT__DTP"},
  {PST_GRPONL__DTP, "GRPONL__DTP"}, 
  {PST_PHSONL__DTP, "PHSONL__DTP"}, 
  {PST_SNBONL__DTP, "SNBONL__DTP"}, 
  {PST_RATONL__DTP, "RATONL__DTP"}, 
  {PST_G_GXS__DTP,  "G_GXS__DTP"}, 
  {PST_PX_GXS__DTP, "PX_GXS__DTP"}, 
  {PST_PS_GXS__DTP, "PS_GXS__DTP"}, 
  {PST_PX_GX__DTP,  "PX_GX__DTP"}, 
  {PST_PX_GS__DTP,  "PX_GS__DTP"}, 
  {PST_PS_GX__DTP,  "PS_GX__DTP"}, 
  {PST_PS_GS__DTP,  "PS_GS__DTP"}, 
  {PST_P_PXS__DTP,  "P_PXS__DTP"}, 
  {PST_GX__DTP,     "GX__DTP"}, 
  {PST_GS__DTP,     "GS__DTP"}, 
  {PST_PX__DTP,     "PX__DTP"}, 
  {PST_PS__DTP,     "PS__DTP"}, 
  {PST_SNG_X__DTP,  "SNG_X__DTP"}, 
  {PST_SNG_S__DTP,  "SNG_S__DTP"}, 
  {PST_FUSED__DTP,  "FUSED__DTP"}
};



const QMap<SgSidebandOrder, QString> sidebandOrderByType
{
  {SO_UNDEF,   "UNDEF"},
  {SO_USB_LSB, "USB, LSB"},
  {SO_LSB_USB, "LSB, USB"},
};



















/*=====================================================================================================*/

