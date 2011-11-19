/*
 * GRASS Ascii raster lonlat to pixel-line conversion 
 */

#include "myelug.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

static void usage() {
   cerr<< endl
       << "Usage: elug.cimis.ll2pl.raster [--long raster] [--lat raster] [--pixlin raster2]\n"
       << "Inputs:\n"
       << "  --long long.raster \n"
       << "  --lat lat.raster \n"
       << "Output:\n"
       << "  --pixlin  pixlin.raster2 \n"
       <<endl;
}


struct Extension {
	float north;
	float south;
	float east;
	float west;
	int rows;
	int cols;

	Extension(FILE* file) {
		fscanf(file,
			"north: %f\n"
			"south: %f\n"
			"east: %f\n"
			"west: %f\n"
			"rows: %d\n"
			"cols: %d\n",
			&north, &south, &east, &west, &rows, &cols
		);
	}

	void print(FILE* file) {
		fprintf(file,
			"north: %f\n"
			"south: %f\n"
			"east: %f\n"
			"west: %f\n"
			"rows: %d\n"
			"cols: %d\n",
			north, south, east, west, rows, cols
		);
	}
};



int main(int argc, char** argv) {
	int arg = 1;
	char* lon_filename = "long.raster";
	char* lat_filename = "lat.raster";
	char* pixlin_filename = "pixlin.raster2";

	for ( ; arg < argc ; arg++ ) {
		if ( strcmp("--help", argv[arg]) == 0 ) {
			usage();
			return 0;
		}
		else if ( strcmp("--long", argv[arg]) == 0 ) {
			if ( ++arg >= argc ) {
				usage();
				return 1;
			}
			lon_filename = argv[arg];
		}
		else if ( strcmp("--lat", argv[arg]) == 0 ) {
			if ( ++arg >= argc ) {
				usage();
				return 2;
			}
			lat_filename = argv[arg];
		}
		else if ( strcmp("--pixlin", argv[arg]) == 0 ) {
			if ( ++arg >= argc ) {
				usage();
				return 3;
			}
			pixlin_filename = argv[arg];
		}
		else {
			cerr<< "invalid argument: " <<argv[arg]<< "   Try --help" <<endl;
			return 4;
		}
	}


	FILE* lon_file = fopen(lon_filename, "r");
	if ( lon_file == 0 ) {
		cerr<< "Error opening file " << lon_filename << endl;
		return 5;
	}
	FILE* lat_file = fopen(lat_filename, "r");
	if ( lat_file == 0 ) {
		cerr<< "Error opening file " << lat_filename << endl;
		return 6;
	}
	FILE* pixlin_file = fopen(pixlin_filename, "w");
	if ( pixlin_file == 0 ) {
		cerr<< "Error opening file " << pixlin_filename << endl;
		return 7;
	}

	Extension lat_ext(lon_file);
	Extension lon_ext(lat_file);

	if ( lat_ext.rows != lon_ext.rows ) {
		cerr<< "Diff  of rows:  lat= " << lat_ext.rows << "  lon= " << lon_ext.rows << endl;
		return 8;
	}

	if ( lat_ext.cols != lon_ext.cols ) {
		cerr<< "Diff  of cols:  lat= " << lat_ext.cols << "  lon= " << lon_ext.cols << endl;
		return 9;
	}

	myelug_init();

	int total = lat_ext.cols * lat_ext.rows ;
	//cout<< " cols * rows = " << lat_ext.cols << " * " << lat_ext.rows << " = " << total << endl;


	lat_ext.print(pixlin_file);

	for ( int i = 0; i < total; i++ ) {
		char lon_str[1024];
		char lat_str[1024];

		fscanf(lon_file, "%s", lon_str);
		fscanf(lat_file, "%s", lat_str);

		//cout<< i+1 << " lon_str=[" << lon_str << "]  lat_str=[" << lat_str << "]" << endl;
		//cout.flush();

		if ( strcmp(lon_str, "N") == 0 || strcmp(lat_str, "N") == 0 ) {
			fprintf(pixlin_file, "N ");
		}
		else {
			float lon = atof(lon_str);
			float lat = atof(lat_str);
	
		//	cout<< " converting (lon,lat) = (" <<lon<< "," <<lat<< ")" <<endl;
			float lin, pix;
			int ierr = myelug_lldeg2pl(lon, lat, &pix, &lin);
			if ( ierr ) {
				cerr<< " Error: " << ierr <<endl;
				return ierr;
			}
				
			int iline = (int) round(lin);
			int ipixel = (int) round(pix);

			fprintf(pixlin_file, "%d,%d ", ipixel, iline);		
			fflush(pixlin_file);
		}
		if ( i > 0 && i % lat_ext.cols == 0 ) {
			fprintf(pixlin_file, "\n");
		}
	}
	fclose(pixlin_file);
	fclose(lat_file);
	fclose(lon_file);
	
	return 0;
}	


