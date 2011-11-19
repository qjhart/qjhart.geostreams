/*
 * lonlat to pixel-line conversion on a list of lonlat specs.
 */

#include "myelug.h"
#include "Csv.h"
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

static void usage() {
   cerr<< endl
       << "Usage: elug.cimis.ll2pl [--separator <sep>]\n"
       << "Read from stdin, assuming the format:\n"
       << "   #id , name,   elev, latitude, longitude    [, ..]\n"
       << "    2  , foo,  eleval,  latval,   lonval [, ...]\n"
       << "   ...\n"
       << "Writes to stdout:\n"
       << "   #id , pixel,  line,    lon ,    lat  \n"
       << "    2  , pixval, linval, lonval , latval\n"
       << "   ...\n"
       << " where(pixval,linval) is the conversion of (lonval,latval).\n"
       <<endl;
}

int main(int argc, char** argv) {
	string separator = ",";
	int arg = 1;
	for ( ; arg < argc ; arg++ ) {
		if ( strcmp("--separator", argv[arg]) == 0 ) {
			separator = argv[++arg];
		}
		else if ( strcmp("--help", argv[arg]) == 0 ) {
			usage();
			return 0;
		}
		else {
			cerr<< "invalid argument: " <<argv[arg]<< "   Try --help" <<endl;
			return 1;
		}
	}

	Csv csv(cin, separator);

	myelug_init();
	
	string line;	
	while ( csv.getline(line) ) {
		//cout << "{{{{{{ line = `" << line <<"'\n";
			
		string id = csv.getfield(0);
		
		if ( strncmp("#id", id.c_str(), 3) == 0 ) {
			cout<< "#id , pixel, line, longitude, latitude" <<endl;
		}
		else {
			cout<< id;

			string lat_s = csv.getfield(3);
			string lon_s = csv.getfield(4);
	
			float lon = atof(lon_s.c_str());
			float lat = atof(lat_s.c_str());
	
			//cout<< " converting (lon,lat) = (" <<lon<< "," <<lat<< ")" <<endl;
			float lin, pix;
			int ierr = myelug_lldeg2pl(lon, lat, &pix, &lin);
			if ( ierr ) {
				cerr<< " Error: " << ierr <<endl;
				return ierr;
			}
				
			int iline = (int) round(lin);
			int ipixel = (int) round(pix);
		
			cout<< ", " <<ipixel<< ", " <<iline<< ", " <<lon_s<< ", " <<lat_s<< endl;
		}		
	}
	
	return 0;
}	


