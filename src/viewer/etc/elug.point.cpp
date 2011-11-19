/*
 * conversion from (pixel,line) to (lon,lat) 
 */

#include "myelug.h"
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

int main(int argc, char** argv) {
	int arg = 1;
	for ( ; arg < argc && strncmp("--", argv[arg], 2) == 0; arg++ ) {
		if ( false ) {
		}
		else {
			cerr<< "invalid option: " <<argv[arg]<< endl;
			return 1;
		}
	}
	if ( arg + 2 > argc ) {
	   cerr<< "Usage: elug.point [--noround] pixel line\n";
	   return 1;
	}

	float pixel = atof(argv[arg++]);
	float line = atof(argv[arg++]);


	myelug_init();

	float lat, lon;      
	int ierr = myelug_pl2lldeg(pixel, line, &lon, &lat);
	if ( ierr ) {
		cerr<< pixel<< " " <<line<< " : couldn't be converted err = "<< ierr << endl;
	}
	else {
		cout<< pixel<< "," <<line<< " --> " <<lon<< "," <<lat <<endl;
	}
		
	return 0;
}	


