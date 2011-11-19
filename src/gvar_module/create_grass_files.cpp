/* $Id: create_grass_files.cpp,v 1.4 2007/10/11 07:51:33 crueda Exp $ */
#include "GrassWriter.h"
#include "GvarStream.h"
#include "Block.h"
#include <unistd.h>      //  gcc complained about undef sleep()

using namespace Geostream ;

// OLD IP: "169.237.153.55" ;
char* ipaddr = "169.237.230.75" ;

int port = 21009 ;

int main(int argc, char* argv[]) {
  if(argc != 4) {
    std::cout << "Usage: " << argv[0] 
	      << " <path> <channelNo> <# of rows>\n" 
	      << "  <# of rows>: define the maximum number of rows to scan "
	      << " in channel <channelNo>. 0 means unlimited.\n" ;

    exit(1);
  }
  
  char* path = argv[1];
  int channelNo = atoi(argv[2]);
  int num_of_rows = atoi(argv[3]);
  Block0* block0 = NULL;

  GrassWriter* grasswriter = new GrassWriter(path);
  GvarStream* gvar = new GvarStream(ipaddr, port);

  while (true) { 
    bool succeed = gvar->listen();

    while(succeed && 
	  (num_of_rows == 0 || 
	   (grasswriter->getNumOfRowsPerChannel(channelNo - 1) < num_of_rows))
	  ) {
      Block* block = gvar->readBlock ();

      if (block == NULL) {
	succeed = false ;
      } else {
    	grasswriter->write(block);
      }
    }

    grasswriter->writeAllHeaderFiles () ;

    if (num_of_rows != 0 &&
	(grasswriter->getNumOfRowsPerChannel(channelNo - 1) >= num_of_rows)) {
      break ;
    }

    gvar->close () ;

    sleep (10) ;
  }

  gvar->close () ;

  delete gvar ;
  delete grasswriter ;
} 
