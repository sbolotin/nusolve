##
##    This file is a part of test applications for Space Geodetic Library. The
##    library is used by nuSolve, a part of CALC/SOLVE system, and designed to
##    make analysis of geodetic VLBI observations.
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



TESTS = \
Test_3dVector \
Test_3dMatrix \
Test_3dMatrixR \
Test_3dMatrixRdots \
Test_Vector \
Test_Matrix \
Test_MJD\
Test_DBH

noinst_PROGRAMS = \
3dVector_app \
3dMatrix_app \
3dMatrixR_app \
3dMatrixRdots_app \
Vector_app \
Matrix_app \
MJD_app \
DBH_app \
DBH_dump DBH_extract FRN_dump KOMB_dump getSessionList checkMjd

OKs = \
3dVector_app.ok \
3dMatrix_app.ok \
3dMatrixR_app.ok \
3dMatrixRdots_app.ok \
Vector_app.ok \
Matrix_app.ok \
MJD_app.ok\
DBH_app.ok


3dVector_app_SOURCES = 3dVector_app.cpp
3dMatrix_app_SOURCES = 3dMatrix_app.cpp
3dMatrixR_app_SOURCES = 3dMatrixR_app.cpp
3dMatrixRdots_app_SOURCES = 3dMatrixRdots_app.cpp
Vector_app_SOURCES = Vector_app.cpp
Matrix_app_SOURCES = Matrix_app.cpp
MJD_app_SOURCES = MJD_app.cpp
DBH_app_SOURCES = DBH_app.cpp
DBH_dump_SOURCES = DBH_dump.cpp
FRN_dump_SOURCES = FRN_dump.cpp
KOMB_dump_SOURCES = KOMB_dump.cpp
DBH_extract_SOURCES = DBH_extract.cpp
getSessionList_SOURCES = getSessionList.cpp
checkMjd_SOURCES = checkMjd.cpp



MOC         = @QT_MOC@

## Here is a reserved place for tests that have to fail:
XFAIL_TESTS = Test_Expected2Fail
##
TESTS += $(XFAIL_TESTS) Test_Expected2BSkipped
##
EXTRA_DIST = $(TESTS) $(OKs) testSuite 10APR06XU_V004


LDADD = ../src/SgLib/libSG.la

LIBS = @QT_LIBS@ @HOPS_LIB@ @LIBS@

AM_CPPFLAGS = -I$(top_srcdir)/src/SgLib @QT_CXXFLAGS@ @NETCDF_INCLUDE@ @HOPS_INCLUDE@ @HOPS_SHARE@



