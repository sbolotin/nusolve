##
##
##    This file is a part of log2ant utility. The utility is a part of
##    nuSolve distribution and is designed to extract meteo parameters,
##    cable calibrations, tsys data from stations log files and store them
##    in ANTAB format.
##    Copyright (C) 2020-2021 Sergei Bolotin.
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

#EXTRA_DIST          = images/dummy.png
CLEANFILES          = *.bak *~ *% $(l2a_Ifc_m)
SUFFIXES            = .moc.cpp


MOC                 = @QT_MOC@
AM_CPPFLAGS         = -I$(top_srcdir)/src/SgLib @QT_CXXFLAGS@
AM_CXXFLAGS         = -DSWCONFIG=\"$(sysconfdir)\" -DDEBUG -ggdb -g -O3 -fopenmp

bin_PROGRAMS        = log2ant
log2ant_LDADD       = ../SgLib/libSG.la @QT_LIBS@ @HOPS_LIB@ @LIBS@


########### headers:
l2a_Gen_h           = log2ant.h
l2a_Gen_cuh         = l2aVersion.cpp
l2a_Ifc_hum         =
l2a_Ifc_hm          =
l2a_res             =

########### sources, mocs (implicitly):
l2a_Gen_c           = $(l2a_Gen_h:.h=.cpp) $(l2a_Gen_cuh)
l2a_Ifc_h           = $(l2a_Ifc_hm) $(l2a_Ifc_hum)
l2a_Ifc_m           = $(l2a_Ifc_hm:.h=.moc.cpp)
l2a_Ifc_mo          = $(l2a_Ifc_m:.cpp=.o)
l2a_Ifc_c           = $(l2a_Ifc_hm:.h=.cpp)


log2ant_SOURCES        = $(l2a_Gen_c) $(l2a_Geo_c) $(l2a_Ifc_c) $(noinst_HEADERS) $(l2a_res)
nodist_log2ant_SOURCES = $(l2a_Ifc_m)
noinst_HEADERS         = $(l2a_Gen_h) $(l2a_Ifc_hum) $(l2a_Ifc_hm)
log2ant_DEPENDENCIES   = $(l2a_Ifc_m) $(l2a_Ifc_mo) $(l2a_res)

## aux targets:
calc:;@find . -not -name '*.o' -and -not -name '*.moc.cpp' -and -not -name '*.P' \
-and -not -path './.deps/*' -and -not -path './.libs/*' -and -not -path './images/*' -and -not -name '*.lo' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "CompileInfo" -not -name "libGPGeo.la" -and -not -name "Makefile" \
-and -not -name "Makefile.in" -and -not -type d -print | sort -r | xargs wc -lc


