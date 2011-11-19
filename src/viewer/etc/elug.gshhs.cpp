/*
 * converts a gsh file into my ad hoc ascii format for lines
 * in pix/line GOES west coordinates 
 * @author Carlos A. Rueda-Velasquez
 * @version $Id: elug.gshhs.cpp,v 1.7 2006/12/08 02:29:53 crueda Exp $
 */

#include "myelug.h"
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>

#include "GSHHS/gshhs.h"

using namespace std;

// I'm using data from the GSHHS site, which is big-endian
#define FLIP


int main(int argc, char** argv) {
	// level = 1 land, 2 lake, 3 island_in_lake, 4 pond_in_island_in_lake
	// see gshhs.h
	int level = 1;
	double min_area = 1000;

	int polyID = -1;
	int arg = 1;
	for ( ; arg < argc && strncmp("--", argv[arg], 2) == 0; arg++ ) {
		if ( strcmp(argv[arg], "--level") == 0 ) {
			if ( ++arg < argc ) {
				level = atoi(argv[arg]);
			}
			else {
				cerr<< "--level: expecting a level value" << endl;
				return 1;
			}
		}
		else if ( strcmp(argv[arg], "--min_area") == 0 ) {
			if ( ++arg < argc ) {
				min_area = atof(argv[arg]);
			}
			else {
				cerr<< "--min_area: expecting a real value" << endl;
				return 1;
			}
		}
		else if ( strcmp(argv[arg], "--polyid") == 0 ) {
			if ( ++arg < argc ) {
				polyID = atoi(argv[arg]);
			}
			else {
				cerr<< "--polyid: expecting ID" << endl;
				return 1;
			}
		}
		else {
			cerr<< "invalid option: " <<argv[arg]<< endl;
			return 1;
		}
	}
	if ( arg + 1 > argc ) {
	   cerr<< "Usage: elug.line [--level #] [--min_area val] [--polyid ID] file.b\n";
	   return 1;
	}

	char* filename = argv[arg++];

	myelug_init();

	// From here adapted from GSHHS/gshhs.c
	
	double w, e, s, n, area, lon, lat;
	char source;
	FILE	*fp;
	int	k, max_east = 270000000;
	struct	POINT p;
	struct GSHHS h;
        
	if ((fp = fopen (filename, "rb")) == NULL ) {
		fprintf (stderr, "gshhs:  Could not find file %s.\n", filename);
		exit (EXIT_FAILURE);
	}
	
	LineStringWriter lsw;
		
	while (fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp) == 1) {

#ifdef FLIP
		h.id = swabi4 ((unsigned int)h.id);
		h.n = swabi4 ((unsigned int)h.n);
		h.level = swabi4 ((unsigned int)h.level);
		h.west = swabi4 ((unsigned int)h.west);
		h.east = swabi4 ((unsigned int)h.east);
		h.south = swabi4 ((unsigned int)h.south);
		h.north = swabi4 ((unsigned int)h.north);
		h.area = swabi4 ((unsigned int)h.area);
		h.greenwich = swabi2 ((unsigned int)h.greenwich);
		h.source = swabi2 ((unsigned int)h.source);
#endif
		w = h.west  * 1.0e-6;
		e = h.east  * 1.0e-6;
		s = h.south * 1.0e-6;
		n = h.north * 1.0e-6;
		source = (h.source == 1) ? 'W' : 'C';
		area = 0.1 * h.area;

		//printf ("P %6d%8d%2d%2c%13.3lf%10.5lf%10.5lf%10.5lf%10.5lf\n", h.id, h.n, h.level, source, area, w, e, s, n);
		
		bool elug_process = h.level == level && h.area >= min_area * 10;
	//	   (h.level == 1 && h.area > 1000*10)     // main LANDS 
	//	|| (h.level == 2 && h.area > 2000*10)     // LAKES
	//	;
		
		lsw.reset();

		for (k = 0; k < h.n; k++) {

			if (fread ((void *)&p, (size_t)sizeof(struct POINT), (size_t)1, fp) != 1) {
				fprintf (stderr, "gshhs:  Error reading file %s for polygon %d, point %d.\n", filename, h.id, k);
				exit (EXIT_FAILURE);
			}
#ifdef FLIP
			p.x = swabi4 ((unsigned int)p.x);
			p.y = swabi4 ((unsigned int)p.y);
#endif
			lon = (h.greenwich && p.x > max_east) ? p.x * 1.0e-6 - 360.0 : p.x * 1.0e-6;
			lat = p.y * 1.0e-6;
			
			if ( elug_process ) {
				float line, pixel;      
				int ierr = myelug_lldeg2pl(lon, lat, &pixel, &line);
				if ( ierr ) {
					lsw.reset();
					continue;
				}
				
				int iline  = (int) round(line);
				int ipixel = (int) round(pixel);
				
				lsw.addPoint(ipixel, iline, lon, lat);
			}
		}
		max_east = 180000000;	/* Only Eurasiafrica needs 270 */
	}
		
	fclose (fp);
	
	return 0;
}	

