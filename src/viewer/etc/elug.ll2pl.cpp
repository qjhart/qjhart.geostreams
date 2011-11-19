/*
 * latlon to pixel-line conversion
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
	   cerr<< "Usage: elug.ll2pl lon lat\n";
	   return 1;
	}

	float lon = atof(argv[arg++]);
	float lat = atof(argv[arg++]);

	myelug_init();

	float line, pixel;      
	int ierr = myelug_lldeg2pl(lon, lat, &pixel, &line);
	if ( ierr ) {
		cerr<< " Error: " << ierr <<endl;
		return ierr;
	}
		
	int iline = (int) round(line);
	int ipixel = (int) round(pixel);

	cout<< ipixel << ", " << iline <<endl;		
	return 0;
}	


