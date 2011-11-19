/**
 * common point to call conversion routines 
 *
 * (the code is not very clean but works ;-))
 *
 * @author Carlos A. Rueda-Velasquez
 * @version $Id: myelug.cpp,v 1.8 2006/12/08 02:29:53 crueda Exp $
 *	Now reading file assuming the following format: See retrieve_elug_params.cpp
	FORMAT:
		float[336]            -- as before
		"imc=%d\n"
		"flipflag=%d\n"
		"iofnc=nscyc1=%d"
		"iofni=nsinc1=%d\n"
		"iofec=ewcyc1=%d\n"
		"iofei=ewinc1=%d\n"
		"%d,%d,%d,%d,%d\n"    -- epoch time: year, day, hrs, min, sec   
		"%d,%d,%d,%d,%d\n"    -- curr  time: year, day, hrs, min, sec   
 */

#include "myelug.h"
#include "selFloat.h"

#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cassert>

#include "ELUG.h"

using namespace std;

static const char* in_filename = "rec.dat";


static Elug elug;


void myelug_init() {
	elug.setInstr(1);	// 1=imager   2=sounder

	cerr<< " Opening " << in_filename << endl;
	FILE* file = fopen(in_filename, "rb");
	float rec[336];
	int nm = fread(rec, sizeof(rec), 1, file);
	if ( nm != 1 ) {
		fprintf(stderr, "couldn't read rec.dat\n");
		return;
	}

	// imc & flipflag
	int imc, flipflag;
        fscanf(file, "imc=%d\n", &imc);
        fscanf(file, "flipflag=%d\n", &flipflag);
        fprintf(stderr, "imc=%d\n", imc);
        fprintf(stderr, "flipflag=%d\n", flipflag);

        // only for imager (sounder ignored)
	int iofnc, iofni, iofec, iofei;
        fscanf(file, "iofnc=nscyc1=%d\n", &iofnc);
        fscanf(file, "iofni=nsinc1=%d\n", &iofni);
        fscanf(file, "iofec=ewcyc1=%d\n", &iofec);
        fscanf(file, "iofei=ewinc1=%d\n", &iofei);
        fprintf(stderr, "iofnc=nscyc1=%d\n", iofnc);
        fprintf(stderr, "iofni=nsinc1=%d\n", iofni);
        fprintf(stderr, "iofec=ewcyc1=%d\n", iofec);
        fprintf(stderr, "iofei=ewinc1=%d\n", iofei);

	// epoch time:
	int epoch_year, epoch_day, epoch_hrs, epoch_min, epoch_sec;
        fscanf(file, "%d,%d,%d,%d,%d\n",
                &epoch_year,
                &epoch_day,
                &epoch_hrs,
                &epoch_min,
                &epoch_sec
        );
        fprintf(stderr, "Epoch time: %d,%d,%d,%d,%d\n",
                epoch_year,
                epoch_day,
                epoch_hrs,
                epoch_min,
                epoch_sec
        );

	// current time:
	int curr_year, curr_day, curr_hrs, curr_min, curr_sec;
        fscanf(file, "%d,%d,%d,%d,%d\n",
                &curr_year,
                &curr_day,
                &curr_hrs,
                &curr_min,
                &curr_sec
        );
        fprintf(stderr, "Current time: %d,%d,%d,%d,%d\n",
                curr_year,
                curr_day,
                curr_hrs,
                curr_min,
                curr_sec
        );

	fclose(file);
	
	cerr<< " ref lon,lat = " <<rad2deg(rec[4])
	    << ", " <<rad2deg(rec[6])<< " degrees" <<endl;
	
	elug.setRec(rec);
	elug.setcons(iofnc,iofni,iofec,iofei,  4,1402,2,1402);

	elug.setFlipflag(flipflag);
	elug.setImc(imc);
	
	double epoch, time;
	epoch = elug.timex(epoch_year, epoch_day, epoch_hrs, epoch_min, epoch_sec);
	time = elug.timex(curr_year, curr_day, curr_hrs, curr_min, curr_sec);
	
	elug.lmodel(time, epoch, imc);


/////// Old manual values used before   /////////////////////////
//	elug.setcons(4,3068,2,3068,4,1402,2,1402);
//	elug.setcons(4,3299,2,3025,4,1402,2,1402);
//	elug.setcons(4,2970,2,3035,4,1402,2,1402); // Dec7 2006
//	epoch = elug.timex(2005, 203, 8, 0, 0 ) ;
//	time =  elug.timex(2005, 203, 18, 17, 29 ) ;
//	elug.lmodel(time, epoch, imc);
/////////////////////////////////////////////////////////////////
}

int myelug_lldeg2pl(float lon, float lat, float *pixel, float* line) {
//	lon += (114.71 - 80.3221);
//	lat -= (31.855 - 29.0452);
	
	float rlon = lon * .017453292519943295; 
	float rlat = lat * .017453292519943295;  
	*pixel = 99999;
	*line = 99999;
	int ierr = elug.ll2pl(rlon, rlat, pixel, line);
	if ( ierr ) {
		*pixel = 99999;
		*line = 99999;
	}
	return ierr;
}	

int myelug_pl2lldeg(float pixel, float line, float *rlon, float *rlat) {
	int ierr = elug.pl2ll(pixel, line, rlon, rlat);
	if ( ierr == 0 ) {
		*rlon = rad2deg(*rlon);
		*rlat = rad2deg(*rlat);
	}
	else {
		*rlon = 99999;
		*rlat = 99999;
	}
	return ierr;
}	



void LineStringWriter::reset() {
	have_previous = false;
	points_included = false;
}

void LineStringWriter::addPoint(int pixel, int line, float lon, float lat) {
	if ( have_previous ) {
		// print previous:
		if ( !points_included ) {
			points_included = true;
			cout<< "LINESTRING" <<endl;
		}
		cout<< previous.pixel<< "," <<previous.line<< "," <<previous.lon<< "," <<previous.lat <<endl;
	}
		
	// and save new previous:
	previous.pixel = pixel;
	previous.line = line;
	previous.lon = lon;
	previous.lat = lat;
	
	have_previous = true;
}

