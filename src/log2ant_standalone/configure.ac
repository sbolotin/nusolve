# Process this file with autoconf to produce a configure script.
#


AC_INIT([log2ant],[0.8.3-rc4],[sergei.bolotin@nasa.gov])
AC_COPYRIGHT([This file is a part of Space Geodetic Library. The library
is used by nuSolve, a part of CALC/SOLVE system, and designed to make
analysis of geodetic VLBI observations.

Copyright (C) 2010-2024 Sergei Bolotin.

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
AC_PROG_INSTALL
LT_INIT
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_MACRO_DIRS([m4])
### AC_CHECK_HEADER_STDBOOL

#
#
########################################################################
# Checks for standard header files.
#### AC_CHECK_INCLUDES_DEFAULT
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
#
################################################################################
#
#
#GCC_WARN="-Wall -W -Wpointer-arith -Wcast-align -Wconversion -Wredundant-decls"
# "-Wconversion" is comment out temporary -- a lot of warnings from Qt sources...
#GCC_WARN="-W -Wall -Wextra -Wpointer-arith -Wcast-align -Wredundant-decls"
#GCC_WARN="-Wall -Wextra"
GCC_WARN="-Wall -Wno-deprecated-declarations"
#GCC_WARN="-Wall"


debug="no"
optimization="no"

qt_lib=""
qt_include=""
qt_bin=""


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

#
# if the libs and headers exist, try to use them:
#
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <QtCore/QString>],
    [QString str])],
    [AC_MSG_RESULT([Checking Qt5Core: ok])],
    [AC_MSG_ERROR([using QString object from QtCore has failed.])])

#
# restore:
#
CPPFLAGS="$CPPFLAGS_SAVED"
LIBS="$LIBS_SAVED"
AC_LANG(C)







AC_DEFINE([UNUSED], [__attribute__((__unused__))], [People complain about the warning.])

AC_CONFIG_FILES([
        Makefile
        src/SgLib/Makefile
        src/log2ant/Makefile])



AC_OUTPUT


