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

noinst_PROGRAMS     = CompileInfo
CompileInfo_SOURCES = CompileInfo.c
AM_CXXFLAGS        = -DDEBUG -g -ggdb -O3 -ffast-math -fno-finite-math-only -fopenmp -march=native
#AM_CXXFLAGS         = -DDEBUG -g -ggdb -O3 -fopenmp

lib_LTLIBRARIES     = libSG.la


#EXTRA_DIST         = xpm/*.xpm
SUFFIXES            = moc.cpp
CLEANFILES          = *.bak *~ *% $(SG_GUI_moc_cpp)
LDADD               = @QT_LIBS@

MOC                 = @QT_MOC@
AM_CPPFLAGS         = @QT_CXXFLAGS@ @NETCDF_INCLUDE@ @HOPS_INCLUDE@ @HOPS_SHARE@



## #################################################################################################
##
##                                           SOURCES
##
## #################################################################################################
##
## Utility Module:
##
## source files that do not have corresponding headers:
#SG_Util_cpp_wohpp   = SgCompileInfo.cpp
SG_Util_cpp_wohpp   = 
## regular source files that have corresponding headers:
SG_Util_cpp_w_hpp   = Sg3dVector.cpp Sg3dMatrix.cpp Sg3dMatrixR.cpp Sg3dMatrixRdots.cpp\
                      SgVector.cpp SgMatrix.cpp SgUtMatrix.cpp SgSymMatrix.cpp SgAttribute.cpp\
                      SgLogger.cpp SgCubicSpline.cpp SgMathSupport.cpp SgRegularExpressions.cpp
## header files that do not have corresponding source files:
# SG_Util_hpp_wocpp = SgMathDefinitions.h
SG_Util_hpp_wocpp   =



##
## Geodesy Module:
##
## source files that do not have corresponding headers:
SG_Geod_cpp_wohpp   = SgVlbiSessionUtilities.cpp SgVlbiSessionUtils4Automation.cpp\
                      SgVlbiSessionUtils4LogProc.cpp SgVlbiSessionIoSupport.cpp\
                      SgVlbiSessionIoDbh.cpp SgVlbiSessionIoNgs.cpp SgVlbiSessionIoVdbInput.cpp\
                      SgVlbiSessionIoVdbOutput.cpp SgVlbiSessionIoFringes.cpp SgVlbiSessionIoKomb.cpp\
                      SgVlbiSessionIoAgv.cpp SgTidalUt1_Tables.cpp
## regular source files:
SG_Geod_cpp_w_hpp   = SgMeteoData.cpp SgMJD.cpp SgSourceKey.cpp SgWrmsable.cpp SgObservation.cpp\
                      SgVlbiObservation.cpp SgVlbiAuxObservation.cpp SgObjectInfo.cpp\
                      SgVlbiStationInfo.cpp SgVlbiBaselineInfo.cpp SgVlbiSourceInfo.cpp\
                      SgVlbiSession.cpp SgVlbiSessionInfo.cpp SgVlbiBand.cpp SgVlbiHistory.cpp\
                      SgZenithDelay.cpp SgMappingFunction.cpp SgRefraction.cpp SgConstants.cpp\
                      SgBreakModel.cpp SgOceanLoad.cpp SgMasterRecord.cpp SgNetworkStnRecord.cpp\
                      SgEccDat.cpp SgEccRec.cpp SgEccSite.cpp\
                      SgAPrioriData.cpp SgModelEop_JMG_96_hf.cpp\
                      SgExternalErpFile.cpp SgExternalWeights.cpp SgExternalTrp.cpp SgVersion.cpp\
                      SgTidalUt1.cpp SgVlbiMeasurement.cpp SgVlbiObservable.cpp SgVlbiNetworkId.cpp\
                      SgStnLogCollector.cpp SgModelsInfo.cpp
## header files that do not have corresponding source files:
SG_Geod_hpp_wocpp   =



##
## Report Generator Module:
##
## source files that do not have corresponding headers:
SG_Rprt_cpp_wohpp   = 
## regular source files:
SG_Rprt_cpp_w_hpp   = SgIdentities.cpp SgSolutionReporter.cpp
## header files that do not have corresponding source files:
SG_Rprt_hpp_wocpp   =



##
## Input/Output Module:
##
## source files that do not have corresponding headers:
SG_IO_cpp_wohpp   = SgVgosDbLoadScan.cpp SgVgosDbLoadObs.cpp\
                    SgVgosDbLoadStation.cpp SgVgosDbLoadSession.cpp SgVgosDbStoreObs.cpp\
                    SgVgosDbStoreSession.cpp SgVgosDbStoreScan.cpp SgVgosDbStoreStation.cpp\
                    SgIoAgvDriverDds.cpp
## regular source files:
SG_IO_cpp_w_hpp   = SgDbhFormat.cpp SgDbhImage.cpp SgVgosDb.cpp SgNetCdf.cpp SgKombFormat.cpp\
                    SgIoDriver.cpp SgIoAgvDriver.cpp SgIoAgv.cpp SgVex.cpp SgIoExternalFilter.cpp
## header files that do not have corresponding source files:
SG_IO_hpp_wocpp   =



##
## GUI Module:
##
## source files that do not have corresponding headers:
SG_GUI_cpp_wohpp   =
## regular source files that need creation of .moc.cpp files:
SG_GUI_cpp_w_hppmoc= SgGuiLogger.cpp SgGuiPlotter.cpp SgGuiVlbiStationList.cpp\
                     SgGuiVlbiBaselineList.cpp SgGuiVlbiSourceList.cpp SgGuiVlbiHistory.cpp\
                     SgGuiQTreeWidgetExt.cpp SgGuiTaskConfig.cpp SgGuiParameterCfg.cpp\
                     SgGuiLoggerConfig.cpp SgGuiPiaReport.cpp
## regular source files (do not need MOC):
SG_GUI_cpp_w_hpp___=
## header files that do not have corresponding source files:
SG_GUI_hpp_wocpp   =





##
## Estimator Module:
##
## source files that do not have corresponding headers:
SG_Est_cpp_wohpp   =
## regular source files that have corresponding headers:
SG_Est_cpp_w_hpp   = SgParameterCfg.cpp SgPartial.cpp SgParameter.cpp\
                     SgParametersDescriptor.cpp SgArcStorage.cpp SgPwlStorage.cpp\
                     SgPwlStorageIncRates.cpp SgPwlStorageBSplineL.cpp SgPwlStorageBSplineQ.cpp SgEstimator.cpp
## header files that do not have corresponding source files:
SG_Est_hpp_wocpp   =





##
## Task Manager Module:
##
## source files that do not have corresponding headers:
SG_Mgr_cpp_wohpp   =
## regular source files that have corresponding headers:
SG_Mgr_cpp_w_hpp   = SgTaskConfig.cpp SgTask.cpp SgTaskManager.cpp SgSingleSessionTaskManager.cpp
## header files that do not have corresponding source files:
SG_Mgr_hpp_wocpp   =





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

SG_headers          = $(SG_Util_hpp) $(SG_Geod_hpp) $(SG_Rprt_hpp) $(SG_IO_hpp) $(SG_GUI_hpp)\
                      $(SG_Est_hpp) $(SG_Mgr_hpp)

libSG_la_SOURCES        = $(SG_Util_cpp) $(SG_Geod_cpp) $(SG_Rprt_cpp) $(SG_IO_cpp) $(SG_GUI_cpp)\
                          $(SG_Est_cpp) $(SG_Mgr_cpp) $(SG_headers)
nodist_libSG_la_SOURCES = $(SG_GUI_moc_cpp)
#include_HEADERS         = $(SG_Util_hpp) $(SG_Geod_hpp) $(SG_Rprt_hpp) $(SG_IO_hpp) $(SG_GUI_hpp)\
#noinst_HEADERS          = $(SG_Util_hpp) $(SG_Geod_hpp) $(SG_Rprt_hpp) $(SG_IO_hpp) $(SG_GUI_hpp)\
#                          $(SG_Est_hpp) $(SG_Mgr_hpp)



## aux targets:
%.moc.cpp:%.h; $(MOC) $< -o $@
#
#SgCompileInfo.cpp:SgCompileInfo.h
#all-local:;@rm -f SgCompileInfo.h SgCompileInfo.o SgCompileInfo.lo
## SgCompileInfo.h:CompileInfo; @./CompileInfo @SG_VERSION_MAJOR@ @SG_VERSION_MINOR@ @SG_VERSION_PATCHLEVEL@ >$@
#SgCompileInfo.h:CompileInfo; @./CompileInfo 0 0 1 >$@
#

showfiles:;@echo $(libSG_la_SOURCES) $(SG_headers)
showgeod:;@echo $(SG_Geod_hpp) $(SG_Geod_cpp)
showutil:;@echo $(SG_Util_hpp) $(SG_Util_cpp)
showio:;@echo $(SG_IO_hpp) $(SG_IO_cpp)
calc:;@find . -not -name '*.o' -and -not -name '*.moc.C' -and -not -name '*.P' \
-and -not -path './.deps/*' -and -not -path './.libs/*' -and -not -name '*.lo' \
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' \
-and -not -name "CompileInfo" -not -name "libSG.la" -and -not -name "Makefile" \
-and -not -name "Makefile.in" -and -not -type d -print | sort -r | xargs wc -lc

showmoc:;@echo $(SG_GUI_moc_cpp)

