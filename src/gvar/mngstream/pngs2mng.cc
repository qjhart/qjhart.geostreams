/*
	GeoStreams Project
	pngs2mng - test program to create a MNG stream from PNG files
	$Id: pngs2mng.cc,v 1.1 2004/05/08 19:05:21 crueda Exp $
*/

/*
	This program is a simple test that creates a MNG file comprising
	one or more PNG files.
	
	Compilation:
		make pngs2mng
		
	Usage:
		./pngs2mng <mng> <width> <height> <png1> png2> ...
	  creates <mng> comprising the given PNG files with given dimensions
*/

#include "MngStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// test program to create a MNG stream from PNG files
int main (int argc, char *argv[]) {
	if ( argc+1 < 4 ) {
		fprintf(stderr, "pngs2mng <mng> <width> <height> <png1> png2> ...\n");
		return 1;
	}
	char* mng_filename = argv[1];
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);

	fprintf(stderr, "Creating %s with dimensions %d x %d\n", mng_filename, width, height);

	MngStream mngStream(mng_filename, width, height);
	for ( int i = 4; i < argc; i++ ) {
		char* png_filename = argv[i];
		fprintf(stderr, "\tadding %s\n", png_filename);
		mngStream.addPng(0, 0, png_filename);
	}
	mngStream.close();
	fprintf(stderr, "Done\n");
	return 0;
}

