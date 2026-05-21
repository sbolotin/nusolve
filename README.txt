This is a README.txt file of nuSolve package.

nuSolve is a software for data analysis of geodetic VLBI observations.
The package consists of GUI software nuSolve and three command line utilities,
vgosDbMake, vgosDbCalc and vgosDbProcLogs. If a user wants only to convert
fringe files into vgosDb format (as most of correlators want), it is possible
to compile and install only vgosDbMake utility. On the other hand, if a user
does not have installed HOPS libraries (as most of AC do), vgosDbMake will
not be compiled.
   To install it, read the following files that are provided by the
distribution:

INSTALL:
   general instruction how to use GNU Build tools (configure, make, etc.).
   If you are not familiar with the tools, it is worth to read it at least
   once.
INSTALL.local:
   comments on nuSolve installation (it is assumed that a user comprehends
   the content of INSTALL file).
docs/UserGuides/nuSolveUserGuide.pdf:
   A user guide for nuSolve users. The Chapter 2 of the guide discuss
   configuration and installation processes.
docs/UserGuides/vgosDbCalcUserGuide.pdf:
   A user guide for vgosDbCalc utility. The Chapter 2 of the guide discuss
   additional peculiarities of the compilation of CALC.
docs/UserGuides/vgosDbMakeUserGuide.pdf:
   A user guide for vgosDbMake utility. The Chapter 2 of the guide discuss
   how to configure the package to use HOPS libraries as well as how to
   compile only one utility -- vgosDbMake.
docs/UserGuides/vgosDbProcLogs.pdf:
   A user guide for vgosDbProcLogs utility. Nothing special with respect to
   installation of the software.


