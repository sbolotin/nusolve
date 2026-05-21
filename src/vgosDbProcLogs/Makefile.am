##
##
##    This file is a part of vgosDbProcLogs. vgosDbProcLogs is a part of
##    CALC/SOLVE system and is designed to extract data, meteo parameters
##    and cable calibrations, from stations log files and store them in
##    the vgosDb format.
##    Copyright (C) 2015-2021 Sergei Bolotin.
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
CLEANFILES          = *.bak *~ *% $(Vdb_Ifc_m)
SUFFIXES            = .moc.cpp


MOC                 = @QT_MOC@
AM_CPPFLAGS         = -I$(top_srcdir)/src/SgLib @QT_CXXFLAGS@ @NETCDF_INCLUDE@
AM_CXXFLAGS         = -DSWCONFIG=\"$(sysconfdir)\" -DDEBUG -ggdb -g -O3 -fopenmp

bin_PROGRAMS        = vgosDbProcLogs
vgosDbProcLogs_LDADD = ../SgLib/libSG.la @QT_LIBS@ @HOPS_LIB@ @LIBS@


########### headers:
Vdb_Gen_h           = vgosDbProcLogs.h VpSetup.h
Vdb_Gen_cuh         = VpVersion.cpp
Vdb_Ifc_hum         =
Vdb_Ifc_hm          = VpStartupWizard.h
Vdb_res             = vgosDbProcLogs_resources.cpp

########### sources, mocs (implicitly):
Vdb_Gen_c           = $(Vdb_Gen_h:.h=.cpp) $(Vdb_Gen_cuh)
Vdb_Ifc_h           = $(Vdb_Ifc_hm) $(Vdb_Ifc_hum)
Vdb_Ifc_m           = $(Vdb_Ifc_hm:.h=.moc.cpp)
Vdb_Ifc_mo          = $(Vdb_Ifc_m:.cpp=.o)
Vdb_Ifc_c           = $(Vdb_Ifc_hm:.h=.cpp)


vgosDbProcLogs_SOURCES        = $(Vdb_Gen_c) $(Vdb_Geo_c) $(Vdb_Ifc_c) $(noinst_HEADERS) $(Vdb_res)
nodist_vgosDbProcLogs_SOURCES = $(Vdb_Ifc_m)
#include_HEADERS              = $(Vdb_Gen_h) $(Vdb_Ifc_hum) $(Vdb_Ifc_hm)
noinst_HEADERS                = $(Vdb_Gen_h) $(Vdb_Ifc_hum) $(Vdb_Ifc_hm)
vgosDbProcLogs_DEPENDENCIES   = $(Vdb_Ifc_m) $(Vdb_Ifc_mo) $(Vdb_res)

## aux targets:
%.moc.cpp:%.h; $(MOC) $< -o $@
calc:;@find . -not -name '*.o' -and -not -name '*.moc.cpp' -and -not -name '*.P' \
-and -not -path './.deps/*' -and -not -path './.libs/*' -and -not -path './images/*' -and -not -name '*.lo' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "CompileInfo" -not -name "libGPGeo.la" -and -not -name "Makefile" \
-and -not -name "Makefile.in"  -and -not -name "vgosDbProcLogs_resources.cpp" -and -not -type d -print | sort -r | xargs wc -lc

vgosDbProcLogs_resources.cpp: images/*.png; rcc -o vgosDbProcLogs_resources.cpp vgosDbProcLogs.qrc

