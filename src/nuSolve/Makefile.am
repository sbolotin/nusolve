##
##
##    This file is a part of nuSolve. nuSolve is a part of CALC/SOLVE system
##    and is designed to perform data analyis of a geodetic VLBI session.
##    Copyright (C) 2010-2021 Sergei Bolotin.
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
CLEANFILES          = *.bak *~ *% $(Ns_Ifc_m)
SUFFIXES            = .moc.cpp


MOC                 = @QT_MOC@
AM_CPPFLAGS         = -I$(top_srcdir)/src/SgLib @QT_CXXFLAGS@ @NETCDF_INCLUDE@
AM_CXXFLAGS         = -DSWCONFIG=\"$(sysconfdir)\" -DSWDATA=\"$(pkgdatadir)\" -DDEBUG -ggdb -g -O3 -ffast-math -fopenmp

bin_PROGRAMS        = nuSolve
nuSolve_LDADD       = ../SgLib/libSG.la @QT_LIBS@ @HOPS_LIB@ @LIBS@


########### headers:
Ns_Gen_h            = nuSolve.h NsSetup.h NsSessionHandler.h NsScrSupport.h
Ns_Gen_cuh          = NsVersion.cpp
Ns_Ifc_hum          =
Ns_Ifc_hm           = NsMainWindow.h NsTestDialog.h NsSessionEditDialog.h NsSetupDialog.h NsStartupWizard.h\
                      NsSessionNameDialog.h NsTestFour1Dialog.h NsBrowseNotUsedObsDialog.h\
                      NsScrPrx4SessionHandler.h NsScrPrx4TaskConfig.h NsScrPrx4ParametersDescriptor.h\
                      NsScrPrx4Session.h NsScrPrx4Observation.h NsScrPrx4Logger.h NsScrPrx4Setup.h \
                      NsBrowseObservation.h
Ns_res              = nuSolve_resources.cpp

########### sources, mocs (implicitly):
Ns_Gen_c            = $(Ns_Gen_h:.h=.cpp) $(Ns_Gen_cuh)
Ns_Ifc_h            = $(Ns_Ifc_hm) $(Ns_Ifc_hum)
Ns_Ifc_m            = $(Ns_Ifc_hm:.h=.moc.cpp)
Ns_Ifc_mo           = $(Ns_Ifc_m:.cpp=.o)
Ns_Ifc_c            = $(Ns_Ifc_hm:.h=.cpp)


nuSolve_SOURCES        = $(Ns_Gen_c) $(Ns_Geo_c) $(Ns_Ifc_c) $(noinst_HEADERS) $(Ns_res)
nodist_nuSolve_SOURCES = $(Ns_Ifc_m)
#include_HEADERS       = $(Ns_Gen_h) $(Ns_Ifc_hum) $(Ns_Ifc_hm)
noinst_HEADERS         = $(Ns_Gen_h) $(Ns_Ifc_hum) $(Ns_Ifc_hm)
nuSolve_DEPENDENCIES   = $(Ns_Ifc_m) $(Ns_Ifc_mo) $(Ns_res)

## aux targets:
#all-local:;size nuSolve
%.moc.cpp:%.h; $(MOC) $< -o $@
calc:;@find . -not -name '*.o' -and -not -name '*.moc.cpp' -and -not -name '*.P' \
-and -not -path './.deps/*' -and -not -path './.libs/*' -and -not -path './images/*' -and -not -name '*.lo' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "CompileInfo" -not -name "libGPGeo.la" -and -not -name "Makefile" \
-and -not -name "Makefile.in" -and -not -name "nuSolve_resources.cpp" -and -not -type d -print | sort -r | xargs wc -lc

nuSolve_resources.cpp: images/*.png; rcc -o nuSolve_resources.cpp nuSolve.qrc

