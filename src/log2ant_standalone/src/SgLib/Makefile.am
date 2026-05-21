##
##
##    This file is a part of Space Geodetic Library. The library is used by
##    nuSolve, a part of CALC/SOLVE system, and designed to make analysis of
##    geodetic VLBI observations.
##    Copyright (C) 2009-2021 Sergei Bolotin.
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
##
LC_ALL              = C
LANG                = C
AM_CXXFLAGS         = -DDEBUG -g -ggdb -O3 -ffast-math -fno-finite-math-only -fopenmp -march=native -DSEPARATED_LOG2ANT -Wall -Wno-deprecated-declarations
lib_LTLIBRARIES     = libSGs.la

SUFFIXES            = moc.cpp
CLEANFILES          = -r *.bak *~ *% $(SG_GUI_moc_cpp) $(libSGs_la_SOURCES) .deps
LDADD               = @QT_LIBS@
MOC                 = @QT_MOC@
AM_CPPFLAGS         = @QT_CXXFLAGS@


## #################################################################################################
##
##                                           SOURCES
##
## #################################################################################################
##
## Utility Module:
##
## source files that do not have corresponding headers:
SG_Util_cpp_wohpp   =
## regular source files that have corresponding headers:
SG_Util_cpp_w_hpp   = SgLogger.cpp SgAttribute.cpp
##
## header files that do not have corresponding source files:
SG_Util_hpp_wocpp   =
##
## Geodesy Module:
##
## regular source files:
SG_Geod_cpp_w_hpp   = SgStnLogCollector.cpp SgConstants.cpp SgVersion.cpp SgMJD.cpp SgMeteoData.cpp
##
## header files that do not have corresponding source files:
SG_Geod_hpp_wocpp   =
##
## Report Generator Module:
##
## source files that do not have corresponding headers:
SG_Rprt_cpp_wohpp   =
## regular source files:
SG_Rprt_cpp_w_hpp   = SgIdentities.cpp
## header files that do not have corresponding source files:
SG_Rprt_hpp_wocpp   =
##
##
## Input/Output Module:
##
## regular source files:
SG_IO_cpp_w_hpp   = SgIoExternalFilter.cpp SgVex.cpp
SG_IO_hpp_wocpp   =










## #################################################################################################
## #################################################################################################
## implicit rules:
SG_Util_hpp_w_cpp   = $(SG_Util_cpp_w_hpp:.cpp=.h)
SG_Geod_hpp_w_cpp   = $(SG_Geod_cpp_w_hpp:.cpp=.h)
SG_Rprt_hpp_w_cpp   = $(SG_Rprt_cpp_w_hpp:.cpp=.h)
SG_Est_hpp_w_cpp    = $(SG_Est_cpp_w_hpp:.cpp=.h)
SG_Mgr_hpp_w_cpp    = $(SG_Mgr_cpp_w_hpp:.cpp=.h)
SG_IO_hpp_w_cpp     = $(SG_IO_cpp_w_hpp:.cpp=.h)
SG_GUI_hpp_w_cpp___ = $(SG_GUI_cpp_w_hpp___:.cpp=.h)
SG_GUI_hpp_w_cppmoc = $(SG_GUI_cpp_w_hppmoc:.cpp=.h)
SG_GUI_moc_cpp      = $(SG_GUI_hpp_w_cppmoc:.h=.moc.cpp)
SG_GUI_hpp_w_cpp    = $(SG_GUI_hpp_w_cppmoc) $(SG_GUI_hpp_w_cpp___)

SG_Util_cpp         = $(SG_Util_cpp_w_hpp) $(SG_Util_cpp_wohpp)
SG_Util_hpp         = $(SG_Util_hpp_w_cpp) $(SG_Util_hpp_wocpp)
SG_Geod_cpp         = $(SG_Geod_cpp_w_hpp) $(SG_Geod_cpp_wohpp)
SG_Geod_hpp         = $(SG_Geod_hpp_w_cpp) $(SG_Geod_hpp_wocpp)
SG_Rprt_cpp         = $(SG_Rprt_cpp_w_hpp) $(SG_Rprt_cpp_wohpp)
SG_Rprt_hpp         = $(SG_Rprt_hpp_w_cpp) $(SG_Rprt_hpp_wocpp)
SG_IO_cpp           = $(SG_IO_cpp_w_hpp)   $(SG_IO_cpp_wohpp)
SG_IO_hpp           = $(SG_IO_hpp_w_cpp)   $(SG_IO_hpp_wocpp)
SG_GUI_cpp          = $(SG_GUI_cpp_w_hpp)  $(SG_GUI_cpp_wohpp) $(SG_GUI_cpp_w_hppmoc)
SG_GUI_hpp          = $(SG_GUI_hpp_w_cpp)  $(SG_GUI_hpp_wocpp)
SG_Est_cpp          = $(SG_Est_cpp_w_hpp)  $(SG_Est_cpp_wohpp)
SG_Est_hpp          = $(SG_Est_hpp_w_cpp)  $(SG_Est_hpp_wocpp)
SG_Mgr_cpp          = $(SG_Mgr_cpp_w_hpp)  $(SG_Mgr_cpp_wohpp)
SG_Mgr_hpp          = $(SG_Mgr_hpp_w_cpp)  $(SG_Mgr_hpp_wocpp)

SGs_headers         = $(SG_Util_hpp) $(SG_Geod_hpp) $(SG_Rprt_hpp) $(SG_IO_hpp) $(SG_GUI_hpp)\
                      $(SG_Est_hpp) $(SG_Mgr_hpp)

libSGs_la_SOURCES       = $(SG_Util_cpp) $(SG_Geod_cpp) $(SG_Rprt_cpp) $(SG_IO_cpp) $(SG_GUI_cpp)\
                          $(SG_Est_cpp) $(SG_Mgr_cpp) $(SGs_headers)
nodist_libSGs_la_SOURCES= $(SG_GUI_moc_cpp)



## aux targets:
%.moc.cpp:%.h; $(MOC) $< -o $@
#


