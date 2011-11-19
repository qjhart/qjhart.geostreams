GeoStreams Project
mngstream - Module for MNG stream management
Carlos A. Rueda
$Id: README.txt,v 1.12 2004/08/10 21:15:52 crueda Exp $

- Reading a MNG stream    
    Java code. Use the Apache Ant build tool. See build.xml.
	See geostreams/**.java
	As an applet, open in your browser:
		http://casil.ucdavis.edu/~crueda/goes/applet
	For a non-gui mng reader, see MngStreamClient.java, which
	is used to keep the most recent image (see the run shell script).

- Writing a MNG stream
    C++ code. See Makefile.
	MngStream is an "easy" API to create a MNG stream of images.
		MngStream.h   interface
		MngStream.cc  implementation
		pgms2mng      test program
		pngs2mng      test program
		goes.php      script that calls gvnetapi
					  Use MngViewer or your preferred browser:
						http://casil.ucdavis.edu/~crueda/goes/goes?maxrows=1000
		
	synopsis of usage:
	
		#include "MngStream.h"
		
		// create MNG stream
		MngStream* mngStream = new MngStream(mngfilename, fullwidth, fullheight);
		
		// add a PNG file
		mngStream.addPng(x, y, png_filename);
		
		// add an image (array of rows)
		uint16** image = ...
		mngStream->addImage(x, y, image, iwidth, iheight);
	
		// close stream
		delete mngStream;
		
	See MngStream.h for more details.


