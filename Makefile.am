##
##
##    This file is a part of nusolve software distribution. The distribution
##    provides such programs as vgosDbCalc, vgosDbProcLogs, nuSolve, and others.
##    They allow a user to process and analyze geodetic VLBI observations.
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



SUBDIRS = src/SgLib doc tests


if WHOLEDIST
SUBDIRS += src/nuSolve src/vgosDbProcLogs src/log2ant
endif

##SUBDIRS += src/nuSolve src/vgosDbProcLogs src/vgosDbCalc/calc11 src/vgosDbCalc src/log2ant


if HASHOPS
SUBDIRS += src/vgosDbMake
endif

if HASCALC
SUBDIRS += src/vgosDbCalc
endif



EXTRA_DIST = BUGS ChangeLog INSTALL INSTALL.local NEWS README\
        docs/UserGuides/nuSolveUserGuide.pdf docs/UserGuides/vgosDbMakeUserGuide.pdf\
        docs/UserGuides/vgosDbCalcUserGuide.pdf docs/UserGuides/vgosDbProcLogsUserGuide.pdf\
        docs/UserGuides/log2antUserGuide.pdf\
        \
        docs/antcal_specs-v2.txt docs/elim_02_hlp.ps docs/upwei_02_hlp.ps\
        docs/Ionosphere.pdf\
        \
        data/ECCDAT.ecc data/glo.axis data/glo.sit data/glo.src data/glo.ssm data/glo.vel data/glo_baseline.wgt\
        data/gsfc_dao_9095.mgr data/jmg96.hf data/last.erp\
        \
        scripts/pia4INT.js scripts/export2ngs.js scripts/vgosDx.js scripts/vgosDxConvertor scripts/autoINT.js\
        log2ant_sep.tgz




adatadir = $(pkgdatadir)/data
docdir = $(pkgdatadir)/doc
scriptsdir = $(pkgdatadir)/scripts

#
#dist_adata_DATA = data/blokq.c11.dat data/DE421_little_Endian data/tilt.dat data/usno_finals.erp data/ut1ls.dat\
#                  data/ECCDAT.ecc data/glo.axis data/glo.sit data/glo.src data/glo.ssm data/glo.vel\
#                  data/glo_baseline.wgt data/gsfc_dao_9095.mgr data/jmg96.hf data/last.erp
#

dist_doc_DATA = docs/UserGuides/nuSolveUserGuide.pdf docs/UserGuides/vgosDbMakeUserGuide.pdf \
                docs/UserGuides/vgosDbCalcUserGuide.pdf docs/UserGuides/vgosDbProcLogsUserGuide.pdf \
                docs/UserGuides/log2antUserGuide.pdf docs/antcal_specs-v2.txt docs/elim_02_hlp.ps \
                docs/upwei_02_hlp.ps docs/Ionosphere.pdf

dist_scripts_DATA = scripts/pia4INT.js scripts/export2ngs.js scripts/vgosDx.js scripts/autoINT.js
dist_bin_SCRIPTS = scripts/vgosDxConvertor


ACLOCAL_AMFLAGS = -I m4

calc:;@find src -not -name '*.o' -and -not -name '*.moc.cpp' -and -not -name '*.P'\
-and -not -path '*/.deps/*' -and -not -path '*/.libs/*' -and -not -path '*/images/*' -and -not -name '*.lo' -and -not -name '*.Plo'\
-and -not -name '*.Po' -and -not -name '*~' -and -not -name '#*#' -and -not -name '*%' -and -not -name 'nuSolve.stc' -and -not -name 'nuSolve_stc' \
-and -not -name 'CompileInfo' -and -not -name 'nuSolve' -not -name 'libSG.la'  -not -name 'vgosDbMake' \
-and -not -path 'src/tests/*' -and -not -name 'Makefile' -and -not -name "nuSolve_resources.cpp" \
-and -not -name 'Makefile.in' -and -not -name '*.log' -and -not -path 'src/.libs/*' -and -not -path 'src/.deps/*' \
-and -not -type d -print | sort -r | xargs wc -lc


