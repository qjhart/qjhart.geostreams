/*
 * common point to call conversion routines 
 */

#include "selFloat.h"
#include "ELUG.h"

#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cassert>

using namespace std;

int main(int argc, char** argv) {

	FILE* file = fopen("rec.dat", "rb");
	float rec[336];
	int nm = fread(rec, sizeof(rec), 1, file);
	fclose(file);
	if ( nm != 1 ) {
		fprintf(stderr, "couldn't read rec.dat\n");
	}

	SelFloat ref_lon(rec[4]);	
	ref_lon.ByteSwap();
	
	SelFloat ref_lat(rec[6]);	
	ref_lat.ByteSwap();
	
	cout<< " ref_lon = " <<rad2deg(ref_lon)<< " degrees" <<endl;
	cout<< " ref_lat = " <<rad2deg(ref_lat)<< " degrees" <<endl;
	
	return 0;
}


