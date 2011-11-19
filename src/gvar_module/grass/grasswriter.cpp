// Main grasswriter program
// Carlos Rueda
// $Id: grasswriter.cpp,v 1.1 2007/10/11 07:50:11 crueda Exp $

// TODO: Do the actual work!

#include "GrassWriterUsingLib.h"

#include "Block.h"
#include "GvarStream.h"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;
using namespace Geostream ;


extern "C" {
#include <grass/gis.h>
}


int main(int argc, char** argv) {

	char *gisbase_name = "gisbase";  // Not used, actually
	char *location_name = "MyLocation";  // Not used, actually



	G_gisinit(argv[0]);

	// TODO: args no used -- remove from interface
	GrassWriterUsingLib grassWriter(gisbase_name, location_name);


	char* ipaddr = "169.237.230.75" ;
	int port = 21009 ;

	bool succeed = true;
	cout<< "ipaddr=" <<ipaddr<< " port=" <<port<< endl;
	GvarStream* gvar = new GvarStream(ipaddr, port);
	succeed = gvar->listen();

	Gvar::Block0Doc* block0doc = NULL;
	    

	while ( succeed )  {
		cout<< "."; cout.flush();
		
		Gvar::Block* block = gvar->readBlock ();


		grassWriter.write(block);

		delete block;
	}


	cout<< "Done." << endl;

	return 0;
}



