GRASS Writer using the GRASS GIS libraries
Carlos Rueda
$Id: README.txt,v 1.1 2007/10/11 07:50:11 crueda Exp $

Attempts to compile a GRASS raster writer of the GOES sectors using the 
preliminary sources GrassWriterUsingLib.{h|cpp} provided by Jie Zhang.

- Installed GRASS 6.2 version from sources. This was a really quick
  installation omitting various of the most standard functionalities because
  it was taking too much time to set up everything.

- Fixed several compile errors in preliminary code.

- Added code for missing G_make_mapset. This is in make_mapset.c, which
  I found in http://grass.itc.it/pipermail/grass-commit/2006-October/024856.html
  It seems this function is only available in version later than grass 6.2 ??


- Makefile create to build grasswriter executable --> completing OK.

- Running:
	. setenv.sh
	./grasswriter


Next:
	- The above were quick adjustments and preliminary test mainly to
	  compile and link everything; but now more careful examination is required
