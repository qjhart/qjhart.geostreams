/*
 * Prints a line in GOES West pixel/line coordinates 
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
	if ( arg + 6 > argc ) {
	   cerr<< "Usage: elug.line lon0 lat0 lon1 lat1 lon_incr lat_incr\n";
	   return 1;
	}

	float lon0 = atof(argv[arg++]);
	float lat0 = atof(argv[arg++]);
	float lon1 = atof(argv[arg++]);
	float lat1 = atof(argv[arg++]);
	float loni = atof(argv[arg++]);
	float lati = atof(argv[arg++]);

	if ( loni < 0 || lati < 0 ) {
	   cerr<< "Non-negative increments please\n";
	   return 1;
	}
	if ( lon1 < lon0 ) {
	   cerr<< "lon1 must be greater than lon0\n";
	   return 1;
	}
	if ( lat1 < lat0 ) {
	   cerr<< "lat1 must be greater than lat0\n";
	   return 1;
	}


	myelug_init();
	LineStringWriter lsw;

	float lon = lon0;
	float lat = lat0;     
	
	for ( ; lon <= lon1 && lat <= lat1; lon += loni, lat += lati ) {
		
		float line, pixel;      
		int ierr = myelug_lldeg2pl(lon, lat, &pixel, &line);
		if ( ierr ) {
			lsw.reset();
			continue;
		}
		
		int iline = (int) round(line);
		int ipixel = (int) round(pixel);
		lsw.addPoint(ipixel, iline, lon, lat);
	}
		
	return 0;
}	


