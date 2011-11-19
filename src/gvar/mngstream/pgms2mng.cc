/*
	GeoStreams Project
	pgms2mng - test program to create a MNG stream from PGM files
	$Id: pgms2mng.cc,v 1.2 2004/05/11 00:24:47 crueda Exp $
*/

/*
	This program is a simple test that creates a MNG file comprising
	one or more PGM files.
	
	Compilation:
		make pgms2mng
		
	Usage:
		./pgms2mng <mng> <pgm1> pgm2> ...
	  creates a <mng> comprising the given PGM files.
	  Use ``-'' to write to stdout
*/

#include "MngStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // sleep

#include <png.h>

static uint16** loadPgm(char* pgm_filename, int *iwidth, int *iheight) {
	FILE* pgmfile = fopen(pgm_filename, "r");
	if ( !pgmfile ) {
		fprintf(stderr, "Cannot open %s\n", pgm_filename);
		return 0;
	}
	int maxval;
	if ( 3 != fscanf(pgmfile, "P5\n%d%d\n%d\n", iwidth, iheight, &maxval) ) {
		fprintf(stderr, "Are you sure %s is a (P5) PGM file?\n", pgm_filename);
		return 0;
	}
	uint16** pgm = new uint16*[*iheight];
	for ( int i = 0; i < *iheight; i++ ) {
		pgm[i] = new uint16[*iwidth];
		for ( int j = 0; j < *iwidth; j++ ) {
			unsigned char b;
			if ( 1 != fread(&b, 1, 1, pgmfile) ) {
				fclose(pgmfile);
				fprintf(stderr, "Cannot read %d columns in row %d\n", *iwidth, i);
				return 0;
			}
			pgm[i][j] = b;
		}
	}
	fclose(pgmfile);
	
	return pgm;
}


// test program to create a MNG stream from PGM files
int main (int argc, char *argv[]) {
	if ( argc+1 < 4 ) {
		fprintf(stderr, 
			"pgms2mng <mng> <iters> <delay> <pgm1> pgm2> ...\n"
			"  <mng>     output file.   - means stdout\n"
			"  <iters>   iterations\n"
			"  <delay>   to add each image (not a real delay into the mng!)\n"
			"  <pgm*>    images in PGM format\n"
		);
		return 1;
	}
	int arg = 1;
	char* mng_filename = argv[arg++];
	int iters = atoi(argv[arg++]);
	int delay = atoi(argv[arg++]);

	MngStream* mngStream = 0;
	int x = 0;
	int y = 0;
	for ( int k = 0; k < iters; k++ ) {
		for ( int i = arg; i < argc; i++ ) {
			char* pgm_filename = argv[i];
			int iwidth, iheight;
			uint16** pgm = loadPgm(pgm_filename, &iwidth, &iheight);
			if ( !pgm )
				continue;
			
			if ( !mngStream ) {
				fprintf(stderr, "Creating %s with dimensions %d x %d\n", 
					mng_filename, iwidth, iheight);
				mngStream = new MngStream(mng_filename, iwidth, iheight);
			}
			fprintf(stderr, "\tadding %s\n", pgm_filename);
			mngStream->addImage(x, y, pgm, iwidth, iheight);
			if ( delay ) 
				sleep(delay);
			
			x += 50;
			y -= 30;
		}
	}
	mngStream->close();
	delete mngStream;
	fprintf(stderr, "Done\n");
	return 0;
}



