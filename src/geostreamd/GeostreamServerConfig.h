#ifndef GEOSTREAM_GEOSTREAMSERVERCONFIG_H
#define GEOSTREAM_GEOSTREAMSERVERCONFIG_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uri.h>
#include <time.h>
#include <event.h>
#include <pthread.h>
#include <errno.h>

#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include "Log.h"

namespace Geostream {

  enum MultiType { 
    THREADS,
    EVENTS 
  } ;

  const int TIME_STR_LEN = 30  ;
  const int DEFAULT_LOG_FACILITY = LOG_USER ;
  const int SERVER_BACKLOG = 64 ;

}

// define a global Log instance
extern Geostream::Log *gLog ;

/* Globals */
extern char Debug;
extern char Verbose;
extern char Daemon;
#define debug(args...) if(::Debug) {gLog->add (LOG_DEBUG,args);}

#endif
