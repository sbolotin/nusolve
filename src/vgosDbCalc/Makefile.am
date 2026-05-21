##
##
##    This file is a part of vgosDbCalc. vgosDbCalc is a part of CALC/SOLVE
##    system and is designed to calculate theoretical values and store them
##    in the vgosDb format.
##    Copyright (C) 2016-2021 Sergei Bolotin.
##
##    This program is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
##
##
##

LC_ALL              = C
LANG                = C

EXTRA_DIST          = images/dummy.png
CLEANFILES          = *.bak *~ *% $(Vc_Ifc_m)
SUFFIXES            = .moc.cpp


MOC                 = @QT_MOC@

AM_CPPFLAGS         = -I$(top_srcdir)/src/SgLib @QT_CXXFLAGS@ @NETCDF_INCLUDE@
AM_CXXFLAGS         = -DSWCONFIG=\"$(sysconfdir)\" -DDEBUG -ggdb -g -O3 -fopenmp

bin_PROGRAMS        = vgosDbCalc
vgosDbCalc_LDADD    = ../SgLib/libSG.la @CALC_LIB@ @QT_LIBS@ @HOPS_LIB@ @LIBS@


########### headers:
Vc_Gen_h           = vgosDbCalc.h VcSetup.h VcCalc2SessionIfc.h
Vc_Gen_cuh         = VcVersion.cpp vgosDbCalcSupport.cpp
Vc_Ifc_hum         =
Vc_Ifc_hm          = VcStartupWizard.h
Vc_res             = vgosDbCalc_resources.cpp

########### sources, mocs (implicitly):
Vc_Gen_c           = $(Vc_Gen_h:.h=.cpp) $(Vc_Gen_cuh)
Vc_Ifc_h           = $(Vc_Ifc_hm) $(Vc_Ifc_hum)
Vc_Ifc_m           = $(Vc_Ifc_hm:.h=.moc.cpp)
Vc_Ifc_mo          = $(Vc_Ifc_m:.cpp=.o)
Vc_Ifc_c           = $(Vc_Ifc_hm:.h=.cpp)


vgosDbCalc_SOURCES        = $(Vc_Gen_c) $(Vc_Geo_c) $(Vc_Ifc_c) $(noinst_HEADERS) $(Vc_res)
nodist_vgosDbCalc_SOURCES = $(Vc_Ifc_m)
noinst_HEADERS            = $(Vc_Gen_h) $(Vc_Ifc_hum) $(Vc_Ifc_hm)
vgosDbCalc_DEPENDENCIES   = $(Vc_Ifc_m) $(Vc_Ifc_mo) $(Vc_res)

## aux targets:
%.moc.cpp:%.h; $(MOC) $< -o $@
calc:;@find . -not -name '*.o' -and -not -name '*.moc.cpp' -and -not -name '*.P' \
-and -not -path './.deps/*' -and -not -path './.libs/*' -and -not -path './images/*' -and -not -name '*.lo' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "CompileInfo" -not -name "libGPGeo.la" -and -not -name "Makefile" \
-and -not -name "Makefile.in"  -and -not -name "vgosDbCalc_resources.cpp" -and -not -type d -print | sort -r | xargs wc -lc

vgosDbCalc_resources.cpp: images/*.png; rcc -o vgosDbCalc_resources.cpp vgosDbCalc.qrc

