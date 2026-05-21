all: nuSolveUserGuide.pdf vgosDbMakeUserGuide.pdf vgosDbProcLogsUserGuide.pdf vgosDbCalcUserGuide.pdf log2antUserGuide.pdf

pdf: nuSolveUserGuide.pdf vgosDbMakeUserGuide.pdf vgosDbProcLogsUserGuide.pdf vgosDbCalcUserGuide.pdf log2antUserGuide.pdf

nuSolveUserGuide.pdf: clean nuSolveUserGuide.tex
	pdflatex nuSolveUserGuide
	makeindex nuSolveUserGuide.tex
	pdflatex nuSolveUserGuide


vgosDbMakeUserGuide.pdf: clean vgosDbMakeUserGuide.tex
	pdflatex vgosDbMakeUserGuide
	makeindex vgosDbMakeUserGuide.tex
	pdflatex vgosDbMakeUserGuide


vgosDbProcLogsUserGuide.pdf: clean vgosDbProcLogsUserGuide.tex
	pdflatex vgosDbProcLogsUserGuide
	makeindex vgosDbProcLogsUserGuide.tex
	pdflatex vgosDbProcLogsUserGuide

vgosDbCalcUserGuide.pdf: clean vgosDbCalcUserGuide.tex
	pdflatex vgosDbCalcUserGuide
	makeindex vgosDbCalcUserGuide.tex
	pdflatex vgosDbCalcUserGuide

log2antUserGuide.pdf: clean log2antUserGuide.tex
	pdflatex log2antUserGuide
	makeindex log2antUserGuide.tex
	pdflatex log2antUserGuide




clean:
	rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl


