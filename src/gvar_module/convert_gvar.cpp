#include <getopt.h>

#include <iostream>
#include <string>
#include <pthread.h>

#include "GvarConverter.h"
#include "GvarStream.h"
#include "ImgStream.h"


char* ipaddr = "169.237.153.55" ;
int port = 21009 ;

class FileStreamWriter : public StreamWriter {
  string filename;
  FILE* file;
                                                                                                        
public:
                                                                                                        
  /** @param filename The output filename. If equals to "-" then
   * it will write to stdout
   */
  FileStreamWriter(string filename) : filename(filename) {
    if ( filename == "-" )
      file = fdopen(fileno(stdout), "wb");
    else
      file = fopen(filename.c_str(), "wb");
  }
                                                                                                        
  ~FileStreamWriter() {
    if ( file && filename != "-" )
      fclose(file);
    file = 0;
  };
                                                                                                        
  /** calls fwrite(buf, nbytes, 1, file) and returns 0
   * if that call was successful. */
  int writeBuffer(const void* buf, int nbytes) {
    if ( 1 == fwrite(buf, nbytes, 1, file) )
      return 0;   // OK
    else
      return -1;  //   error ocurred
  }
} ;

void printUsage (char* name) {
  std::cout << "Usage: " << name 
	    << " -dvh -o <format> -p <path> -f <filename> -m <num>\n"
	    << " -d/--debug: Debug\n"
	    << " -v/--verbose: Verbose\n"
	    << " -h/--help: print out this page\n"
	    << " -o/--format: define the output format\n"
	    << "    0 -- unknown format\n"
	    << "    1 -- bin_format\n"
	    << "    2 -- ascii_format\n"
	    << " -p/--path <path>: define the path for files to be generated\n"
	    << " -f/--filename <filename>: define the filename\n"
	    << " -m/--max-number-of-rows <#-of-rows>: define the maximum "
	    << "number of rows to be processed in a visible channel.\n"
	    << "\n" ;
}

void *gvar_reader (void* ptr) {
  Geostream::GvarStream* gvar = (Geostream::GvarStream*)ptr ;

  gvar->listen () ;

  while (1) {
    gvar->read () ;
  }

  pthread_exit (0) ;
}

int main (int argc, char* argv[]) {

  struct option long_options[] = {
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"help", 0, 0, 'h'},
    {"format", 1, 0, 'o'},
    {"path",1,0,'p'},
    {"filename", 1, 0, 'f'},
    {"max-number-of-rows", 1, 0, 'm'},
    {0, 0, 0, 0}
  };

  std::string path ;
  std::string filename ;
  long long maxNumOfRows ;
  Format format = UNKNOWN_FORMAT ;

  while (1) {
    int option_index = 0;
    int c = getopt_long (argc, argv, "dvho:p:f:m:",
			 long_options, &option_index);
    
    if (c == -1)
      break;

    if (c == 63) {
      printUsage (argv[0]) ;
      exit (0) ;
    }
 
    switch (c) {
    case 'h':
      printUsage (argv[0]) ;
      exit (0) ;
    case 'o':
      format = (Format)(atoi (optarg)) ;
      break ;
    case 'p':
      std::cout << "optarg=" << optarg << "\n" ;
      path = optarg ;
      break ;
    case 'f':
      filename = optarg ;
      break ;
    case 'm':
      maxNumOfRows = atoi (optarg) ;
      break ;
    default:
      break ;
    }
  }

  if (format == UNKNOWN_FORMAT ||
      (path.length () == 0 && filename.length () == 0)) {
    printUsage (argv[0]) ;
    exit (0) ;
  }


  fprintf (stderr, "format=%d\n", format) ;

  Geostream::GvarStream* gvar = 
    new Geostream::GvarStream (ipaddr, port) ;
  Geostream::Fifo<Row>* rowFifo = new Geostream::Fifo<Row> (100*8+3*100*4+100) ;
  Geostream::FifoWriter<Row>* rowWriter = rowFifo->newWriter () ;
  gvar->allRowsWriter (rowWriter) ;

  // start gvar reading
  pthread_t writer_thread ;
  pthread_create (&writer_thread, NULL, gvar_reader, (void*)gvar) ;

  ReferenceSpace rs(5500, 2200, 27000, 14000);

  FileStreamWriter* writer = new FileStreamWriter (filename) ;

  Geostream::ClientRequest request ;

  request.format = format ;
  for (int i=0; i<NUM_OF_CHANNELS; i++) {
    if (i != 2) {
      request.isChannelRequested[i] = false ;
    } else {
      request.isChannelRequested[i] = true ;
    }
  }

  Geostream::GvarConverter* converter = 
    new Geostream::GvarConverter (gvar, rowFifo, request,
				  &rs, writer) ;

  converter->convert () ;

  delete converter ;
  delete writer ;
  delete rowFifo ;
  delete gvar ;
}
