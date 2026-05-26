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



# Define the final PDF as a data file so it gets installed
#docdir = $(datadir)/doc/$(PACKAGE)
doc_DATA = nuSolveUserGuide.pdf vgosDbMakeUserGuide.pdf vgosDbProcLogsUserGuide.pdf vgosDbCalcUserGuide.pdf log2antUserGuide.pdf
SUFFIXES = .tex .pdf

.tex.pdf:
	pdflatex -interaction=batchmode $<
	makeindex $<
	pdflatex -interaction=batchmode $<

#
#nuSolveUserGuide.pdf: clean nuSolveUserGuide.tex
#        pdflatex -interaction=batchmode nuSolveUserGuide
#        makeindex nuSolveUserGuide.tex
#        pdflatex -interaction=batchmode nuSolveUserGuide
#
#
#vgosDbMakeUserGuide.pdf: clean vgosDbMakeUserGuide.tex
#        pdflatex vgosDbMakeUserGuide
#        makeindex vgosDbMakeUserGuide.tex
#        pdflatex vgosDbMakeUserGuide
#
#
#vgosDbProcLogsUserGuide.pdf: clean vgosDbProcLogsUserGuide.tex
#        pdflatex vgosDbProcLogsUserGuide
#        makeindex vgosDbProcLogsUserGuide.tex
#        pdflatex vgosDbProcLogsUserGuide
#
#vgosDbCalcUserGuide.pdf: clean vgosDbCalcUserGuide.tex
#        pdflatex vgosDbCalcUserGuide
#        makeindex vgosDbCalcUserGuide.tex
#        pdflatex vgosDbCalcUserGuide
#
#log2antUserGuide.pdf: clean log2antUserGuide.tex
#        pdflatex log2antUserGuide
#        makeindex log2antUserGuide.tex
#        pdflatex log2antUserGuide

CLEANFILES = $(doc_DATA) *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl

EXTRA_DIST = nuSolveUserGuide.tex vgosDbMakeUserGuide.tex vgosDbProcLogsUserGuide.tex vgosDbCalcUserGuide.tex log2antUserGuide.tex


