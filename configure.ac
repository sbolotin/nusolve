# Process this file with autoconf to produce a configure script.
#


AC_INIT([nuSolve],[0.8.3.2],[sergei.bolotin@nasa.gov])
AC_COPYRIGHT([This file is a part of Space Geodetic Library. The library
is used by nuSolve, a part of CALC/SOLVE system, and designed to make
analysis of geodetic VLBI observations.

Copyright (C) 2010-2021 Sergei Bolotin.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.])

##AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AM_INIT_AUTOMAKE([-Wall])
AM_PROG_AR
AC_PROG_CC
AC_PROG_CXX
AC_PROG_F77([gfortran f77])
AC_PROG_INSTALL
LT_INIT
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_MACRO_DIRS([m4])


#
#
########################################################################
# Checks for standard header files.
#AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS([errno.h stdlib.h string.h unistd.h fcntl.h argp.h])

########################################################################
# standard stuff:
AC_CHECK_LIB([c],  [printf])
AC_CHECK_LIB([m],  [sin])

########################################################################
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_UID_T
AC_TYPE_SIZE_T

########################################################################
# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([dup2 floor gethostname memset memcpy mkdir sqrt strdup \
strerror strstr uname sincos modf rmdir])


#
# FORTRAN stuff:
#
if test "x$F77" = xgfortran; then
    AC_MSG_RESULT([FORTRAN compiler: gfortran])
elif test "x$F77" = xf77; then
    AC_MSG_RESULT([FORTRAN compiler: f77])
else
    AC_MSG_ERROR([Cannot find usable FORTRAN compiler])
fi


#
#
################################################################################
#
#
#GCC_WARN="-Wall -W -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls"
# "-Wconversion" is comment out temporary -- a lot of warnings from Qt sources...
#GCC_WARN="-W -Wall -Wextra -Wpointer-arith -Wcast-align -Wredundant-decls"
#GCC_WARN="-Wall -Wextra"
GCC_WARN="-Wall -Wno-deprecated-declarations"


debug="no"
optimization="no"
netcdf_lib=""
netcdf_include=""
calc_lib=""
hops_lib=""
hops_include=""
hops_share=""
vgosDbMake_only="no"
NETCDF_INCLUDE=""
NETCDF_LIB=""
CALC_LIB=""
HOPS_INCLUDE=""
HOPS_LIB=""
HOPS_SHARE=""

qt_lib=""
qt_include=""
qt_bin=""

AC_ARG_WITH(netcdf-dir,
    AS_HELP_STRING([--with-netcdf-dir=DIR],[where the library netCDF is installed]),
    [netcdf_include="$withval"/include; netcdf_lib="$withval"/lib])
AC_ARG_WITH(netcdf-include,
    AS_HELP_STRING([--with-netcdf-include=DIR],[where the netCDF includes are]),
    [netcdf_include="$withval"])
AC_ARG_WITH(netcdf-lib,
    AS_HELP_STRING([--with-netcdf-lib=DIR],[where the netCDF libraries are]),
    [netcdf_lib="$withval"])
AC_ARG_WITH(calc-lib,
    AS_HELP_STRING([--with-calc-lib=DIR],[a prefix of installed CALC library]),
    [calc_lib="$withval"])
AC_ARG_WITH(hops-dir,
    AS_HELP_STRING([--with-hops-dir=DIR],[where HOPS is installed (if you're unsure, you do not need it)]),
    [hops_include="$withval"/include; hops_lib="$withval"/lib; hops_share="$withval"/share])
AC_ARG_WITH(hops-include,
    AS_HELP_STRING([--with-hops-include=DIR],[where HOPS includes are]),
    [hops_include="$withval"])
AC_ARG_WITH(hops-lib,
    AS_HELP_STRING([--with-hops-lib=DIR],[where the hops libraries are]),
    [hops_lib="$withval"])
AC_ARG_WITH(hops-share,
    AS_HELP_STRING([--with-hops-share=DIR],[where the hops share files are]),
    [hops_share="$withval"])
#AC_ARG_WITH(calc-datadir,
#    AS_HELP_STRING([--with-calc-datadir=DIR],[where CALC files with a priori are]),
#    [CALC_DATA="$withval"])
AC_ARG_WITH(qt-dir,
    AS_HELP_STRING([--with-qt-dir=DIR],[where the root of Qt library is installed]),
    [qt_include="$withval"/include; qt_lib="$withval"/lib; qt_bin="$withval"/bin])
AC_ARG_WITH(qt-include,
    AS_HELP_STRING([--with-qt-include=DIR],[where the Qt includes are]),
    [qt_include="$withval"])
AC_ARG_WITH(qt-lib,
    AS_HELP_STRING([--with-qt-lib=DIR],[where the Qt libraries are]),
    [qt_lib="$withval"])
AC_ARG_WITH(qt-bin,
    AS_HELP_STRING([--with-qt-bin=DIR],[where the Qt binaries (e.g., moc) are]),
    [qt_bin="$withval"])

AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--enable-debug],[enable debug/developing mode (default)]),
    [if test "x$enableval" == "xyes"; then debug="yes"; fi],
    [debug="yes"])
AC_ARG_ENABLE(debug,
    AS_HELP_STRING([--disable-debug],[disable debug/developing mode (default=enable)]),
    [if test "x$enableval" == "xno"; then debug="no"; fi],
    [debug="yes"])
AC_ARG_ENABLE(scripts,
    AS_HELP_STRING([--enable-scripts],[enable script mode (default)]),
    [if test "x$enableval" == "xyes"; then scripts="yes"; fi],
    [scripts="yes"])
AC_ARG_ENABLE(vgosDbMake_only,
    AS_HELP_STRING([--enable-vgosDbMake-only],[enable building vgosDbMake only, do not make other utilities]),
    [if test "x$enableval" == "xyes"; then vgosDbMake_only="yes"; fi],
    [vgosDbMake_only="no"])

#
#
#
#if test -n "$CALC_DATA"; then
#    AC_MSG_RESULT(Path to CALC a priori files: $CALC_DATA)
#else
#    AC_MSG_ERROR([Path to CALC a priori files is not specified. Use, at least, "--with-calc-datadir=./"])
#fi
#AC_SUBST(CALC_DATA)

PATH_2_BIN=${bindir}
AC_SUBST(PATH_2_BIN)




#
################################################################################
#
#
# Start with Qt:
if test -n "$qt_lib" && test -n "$qt_include" && test -n "$qt_bin"
then
    QT_PATH_TO_LIBS="$qt_lib"
    QT_PATH_TO_INCS="$qt_include"
    QT_PATH_TO_BINS="$qt_bin"

    if test -n "$QT_PATH_TO_BINS"
    then
        PATH="$QT_PATH_TO_BINS:$PATH"
        QT_MOC="$QT_PATH_TO_BINS/moc"
    fi

else
    AC_CHECK_PROGS(QMAKE, [qmake-qt5 qmake])
    if test -z "$QMAKE"
    then
        AC_MSG_ERROR([Cannot find qmake.])
    fi
    QT_PATH_TO_LIBS=`$QMAKE -query QT_INSTALL_LIBS`
    QT_PATH_TO_INCS=`$QMAKE -query QT_INSTALL_HEADERS`
    QT_PATH_TO_BINS=`$QMAKE -query QT_HOST_BINS`

    AC_CHECK_PROGS(QT_MOC, [moc-qt5 moc])
    if test -z "$QT_MOC"
    then
        AC_MSG_ERROR([Cannot find moc.])
    fi
fi

QT_CXXFLAGS="-I$QT_PATH_TO_INCS -fPIC"
QT_LIBS="-L$QT_PATH_TO_LIBS"

AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_LIBS)
AC_SUBST(QT_MOC)
#

###
#AC_MSG_RESULT([QT_LIBS: $QT_LIBS, QT_CXXFLAGS: $QT_CXXFLAGS, LIBS: $LIBS, QT_MOC: $QT_MOC])
###


####
#
#
# First, check the _presence_ of the required Qt5 libraries:
#
AC_CHECK_LIB([Qt5Core], [sync], [QT_LIBS="-lQt5Core $QT_LIBS"],
    [AC_MSG_ERROR([Cannot find libQt5Core. Ask your sysadmin to install the library.])],
    ["$QT_LIBS"])
AC_MSG_RESULT([Checking Qt5Core library: found])

AC_CHECK_LIB([Qt5Gui], [sync], [QT_LIBS="-lQt5Gui $QT_LIBS"],
    [AC_MSG_ERROR([Cannot find libQt5Gui. Ask your sysadmin to install the library.])],
    ["$QT_LIBS"])
AC_MSG_RESULT([Checking Qt5Gui library: found])

AC_CHECK_LIB([Qt5Widgets], [sync], [QT_LIBS="-lQt5Widgets $QT_LIBS"],
    [AC_MSG_ERROR([Cannot find libQt5Widgets. Ask your sysadmin to install the library.])],
    ["$QT_LIBS"])
AC_MSG_RESULT([Checking Qt5Widgets library: found])

#AC_CHECK_LIB([Qt5PrintSupport], [sync], [QT_LIBS="-lQt5PrintSupport $QT_LIBS"],
#    [AC_MSG_ERROR([Cannot find libQt5PrintSupport. Ask your sysadmin to install the library.])])
#AC_MSG_RESULT([Checking Qt5PrintSupport library: found])

if test "$scripts" == "yes"
then
    AC_CHECK_LIB([Qt5Script], [sync], [QT_LIBS="-lQt5Script $QT_LIBS"],
        [AC_MSG_ERROR([Cannot find libQt5Script. Ask your sysadmin to install the library.])],
    ["$QT_LIBS"])
    AC_MSG_RESULT([Checking Qt5Script library: found])
fi


#
#
# Then, verify _presence_ of the header files (usually, they are shipped in separate packages:)
#
AC_LANG(C++)
CPPFLAGS_SAVED="$CPPFLAGS"
LIBS_SAVED="$LIBS"

CPPFLAGS="-I$QT_PATH_TO_INCS -fPIC"
LIBS="$QT_LIBS"

AC_CHECK_HEADER([QtCore/QDataStream],
    [AC_MSG_RESULT([Headers for Qt5Core have been found.])],
    [AC_MSG_ERROR([Cannot find headers for QtCore. Ask your sysadmin to install the proper package(s).])])

AC_CHECK_HEADER([QtGui/QPen],
    [AC_MSG_RESULT([Headers for Qt5Gui have been found.])],
    [AC_MSG_ERROR([Cannot find headers for Qt5Gui. Ask your sysadmin to install the proper package(s).])])

AC_CHECK_HEADER([QtWidgets/QLabel],
    [AC_MSG_RESULT([Headers for Qt5Widgets have been found.])],
    [AC_MSG_ERROR([Cannot find headers for Qt5Widgets. Ask your sysadmin to install the proper package(s).])])

#AC_CHECK_HEADER([QtPrintSupport/QPrinter],
#    [AC_MSG_RESULT([Headers for Qt5PrintSupport have been found.])],
#    [AC_MSG_ERROR([Cannot find headers for Qt5PrintSupport. Ask your sysadmin to install the proper package(s).])])

if test "$scripts" == "yes"
then
    AC_CHECK_HEADER([QtScript/QScriptEngine],
        [AC_MSG_RESULT([Headers for Qt5Script have been found.])],
        [AC_MSG_ERROR([Cannot find headers for Qt5Script. Ask your sysadmin to install the proper package(s).])])
fi

#
# if the libs and headers exist, try to use them:
#
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <QtCore/QString>],
    [QString str])],
    [AC_MSG_RESULT([Checking Qt5Core: ok])],
    [AC_MSG_ERROR([using QString object from QtCore has failed.])])

AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <QtGui/QPen>],
    [QPen pen])],
    [AC_MSG_RESULT([Checking Qt5Gui: ok])],
    [AC_MSG_ERROR([using QPen object from QtGui has failed.])])

AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <QtWidgets/QLabel>],
    [QLabel label])],
    [AC_MSG_RESULT([Checking QtWidgets: ok])],
    [AC_MSG_ERROR([using QLabel object from QtWidgets has failed.])])

#AC_LINK_IFELSE(
#    [AC_LANG_PROGRAM([#include <QtPrintSupport/QPrinter>],
#    [QPrinter printer])],
#    [AC_MSG_RESULT([Checking QtPrintSupport: ok])],
#    [AC_MSG_ERROR([using QPrinter object from QtPrintSupport has failed.])])

if test "$scripts" == "yes"
then
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([#include <QtScript/QScriptEngine>],
        [QScriptEngine engine])],
        [AC_MSG_RESULT([Checking QtScript: ok])],
        [AC_MSG_ERROR([using QScriptEngine object from QtScript has failed.])])
fi
#
# restore:
#
CPPFLAGS="$CPPFLAGS_SAVED"
LIBS="$LIBS_SAVED"
AC_LANG(C)



########################################################################
# Check for netCDF's path:
AC_MSG_CHECKING(for path to netCDF)
if test -n "$netcdf_include"; then
    NETCDF_INCLUDE="-I$netcdf_include"
    CPPFLAGS="$CPPFLAGS $NETCDF_INCLUDE"
    AC_SUBST(NETCDF_INCLUDE)
fi
if test -n "$netcdf_lib"; then
    NETCDF_LIB="-L$netcdf_lib"
    LIBS="$LIBS $NETCDF_LIB"
    AC_SUBST(NETCDF_LIB)
fi

AC_CHECK_HEADER([netcdf.h],
    [AC_MSG_RESULT([Headers for NetCDF have been found.])],
    [AC_MSG_ERROR([Cannot find headers for netCDF. Ask your sysadmin to install the proper package(s).])])

AC_CHECK_LIB([netcdf], [nc_inq_libvers], [],
    [AC_MSG_ERROR([Cannot find or link the netcdf library. Ask your sysadmin to install the proper package(s).])])

AC_MSG_RESULT(Checking NetCDF is done)



########################################################################
# CALC is distributing in a separate package:
AC_MSG_CHECKING(for presense of CALC library)
if test -n "$calc_lib"; then
    CALC_LIB="-L$calc_lib/lib -lCalc"
fi
AC_MSG_RESULT([done])
AC_SUBST(CALC_LIB)


AC_MSG_RESULT(Checking for CALC library is done)



########################################################################
# Check for HOPS's path:
AC_MSG_CHECKING(for path to HOPS)
#
LIBS_SAVED="$LIBS"
LIBS="$QT_LIBS"
#
if test -n "$hops_include"; then
    HOPS_INCLUDE="-I$hops_include"
fi
if test -n "$hops_lib"; then
    HOPS_LIB="-L$hops_lib/hops -lvex -ldfio -lmk4util"
    LIBS="$HOPS_LIB"
    AC_CHECK_LIB([msg], [msg],
        [AC_MSG_NOTICE([New variety of HOPS found.]); HOPS_LIB="$HOPS_LIB -lmsg"; CXXFLAGS="$CXXFLAGS -DNEW_HOPS"],
        [AC_MSG_NOTICE([No new HOPS found.]); CXXFLAGS="$CXXFLAGS -DOLD_HOPS"]
        )
    AC_MSG_RESULT([Checking new HOPS: done])
fi
#
LIBS="$LIBS_SAVED"
#
AC_MSG_RESULT([Configuring HOPS: done])
AC_SUBST(HOPS_INCLUDE)
AC_SUBST(HOPS_LIB)




# cannot check the libraries using the configure because of undetermined
# "char progname[]" and "int msglev" external variables in HOPS package.
#LDFLAGS="$LDFLAGS $HOPS_LIB"
#AC_CHECK_LIB([dfio], [read_mk4fringe], [], [AC_MSG_RESULT([HOPS not found])], [-lmk4util])
#AC_CHECK_LIB([vex],  [get_vex],        [], [AC_MSG_RESULT([HOPS not found])], [-lmk4util])
# restore flags:
#LDFLAGS="$LDFLAGS_SAVED"

AC_MSG_CHECKING(HOPS stuff)
if test -n "$hops_include" -a -n "$hops_lib"; then
#    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $HOPS_INCLUDE/hops"
    AC_CHECK_HEADERS([mk4_vex.h mk4_data.h mk4_dfio.h],
        [AC_MSG_RESULT([HOPS headers have been found])],
        [AC_MSG_ERROR([Cannot find header files for HOPS, ask your sysadmin to install the software.])])
#    CPPFLAGS="$CPPFLAGS_SAVED"

    AC_MSG_CHECKING(path to HOPS share files)
    if test -z "$hops_share"; then
        AC_MSG_ERROR(not specified)
    else
        HOPS_SHARE="-DPATH_2_HOPS_SHARE=\\\"$hops_share/hops\\\""
        AC_MSG_RESULT(found)
    fi
else
    AC_MSG_RESULT(HOPS are not required)
fi
AC_SUBST(HOPS_SHARE)
AC_MSG_RESULT(Checking HOPS is done)




########################################################################
#
AC_MSG_CHECKING(for debug flags)
# do not strip executable if the debug `-g' is given:
LDFLAGS=`echo $LDFLAGS | sed 's/-s//g'`
# remove duplicates (-Wall is added anyway; `-g' is added if debug is enabled,
# otherwise, it is removed because of the strip `-s' flag for linking):
CFLAGS=`echo $CFLAGS | sed 's/-g//g' | sed 's/-Wall//g' | sed 's/-O2//g'`
CXXFLAGS=`echo $CXXFLAGS | sed 's/-g//g' | sed 's/-Wall//g' | sed 's/-O2//g'`
if test "$debug" == "yes"; then
    CFLAGS="$CFLAGS -g $GCC_WARN -DDEBUG"
    CXXFLAGS="$CXXFLAGS -g $GCC_WARN -DDEBUG"
    LDFLAGS="$LDFLAGS"
else
    CFLAGS="$CFLAGS $GCC_WARN"
    CXXFLAGS="$CXXFLAGS $GCC_WARN"
    LDFLAGS="$LDFLAGS -s"
fi
AC_MSG_RESULT($debug)


AC_MSG_CHECKING(for script mode)
if test "$scripts" == "yes"; then
    AC_DEFINE([HAVE_SCRIPTS], [1], [Define to 1 if you have QtScript module installed.])
else
    AC_DEFINE([HAVE_SCRIPTS], [0], [Define to 1 if you have QtScript module installed.])
fi
AC_MSG_RESULT($scripts)


AC_DEFINE([UNUSED], [__attribute__((__unused__))], [People complain about the warning.])

AC_CONFIG_FILES([
        Makefile
        src/SgLib/Makefile
        doc/Makefile
        tests/Makefile
])


if test "$vgosDbMake_only" == "no"; then
AC_CONFIG_FILES([
        src/nuSolve/Makefile
        src/vgosDbProcLogs/Makefile
        src/log2ant/Makefile
        scripts/vgosDxConvertor
])
fi


##      src/vgosDbCalc/calc11/Makefile
##      src/vgosDbCalc/calc11/param11.i
##      src/vgosDbCalc/Makefile


AM_CONDITIONAL([WHOLEDIST], [test "$vgosDbMake_only" == "no"])

AM_CONDITIONAL([HASHOPS], [test -n "$hops_include" -a -n "$hops_lib"])

AM_CONDITIONAL([HASCALC], [test -n "$calc_lib"])

if test -n "$hops_include" -a -n "$hops_lib"; then
    AC_CONFIG_FILES([src/vgosDbMake/Makefile])
fi

if test -n "$calc_lib"; then
    AC_CONFIG_FILES([src/vgosDbCalc/Makefile])
fi

AC_OUTPUT


