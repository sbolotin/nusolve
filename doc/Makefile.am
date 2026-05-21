##
##
##


DOX = libGPGeo.dox


html_DOCS     = html/*
latex_DOCS    = latex/*
man_DOCS      = man/man3/*

alldocs    = $(html_DOCS) $(latex_DOCS) $(man_DOCS)

EXTRA_DIST = $(DOX) $(alldocs)

clean-local:;rm -fR html man latex

latex/* html/* man/man3/*:;VER="$(VERSION)" doxygen $(DOX)

