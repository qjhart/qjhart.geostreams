Vector datasets readme
$Id: README.txt,v 1.4 2007/06/21 02:39:47 crueda Exp $

This is the ELUG-based module to generate vector data in GOES
coordinates so it can be included in the visualizer.

* Shorelines

The shorelines are obtained from:
GSHHS: A Global Self-consistant Hierarchical High-resolution 
Shorelines Database
http://www.soest.hawaii.edu/wessel/gshhs/gshhs.html

Subdir GSHHS/ contains some of the dataset in various resolutions and
part of the GSHHS software for reference. My main source file is
elug.gshhs.cpp.  Note that rec.dat (containing the required 336 coeffs
from the GOES instruement) is assumed to exist in the current directory.

* elug.gshhs

This program accepts options --level and --min_area.
For example, to generate the lakes with min area 2000 and 5000 Km2:
	  ./elug.gshhs --level 2 --min_area 5000 GSHHS/gshhs_l.b > lakes5000.txt
	  ./elug.gshhs --level 2 --min_area 2000 GSHHS/gshhs_l.b > lakes2000.txt
	  

To build the programs:
	make
This requires the ELUG module. See Makefile.
	
To create the vector datasets:
	make vectors

Note: Program retrieve_elug_params (in gvar_module) is required to
generate the current ELUG parameters file. See DEVNOTES.txt