/*
  config - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: geostreamd.cpp,v 1.6 2005/06/03 18:59:16 qjhart Exp $

  $Log: geostreamd.cpp,v $
  Revision 1.6  2005/06/03 18:59:16  qjhart
  Removed libgvar from source

  Revision 1.5  2005/04/13 22:29:09  qjhart
  Started Fifo version

  Revision 1.4  2005/04/07 03:18:08  jiezhang
  Check in some fixes on HTTP server C++ code.

  Revision 1.3  2005/04/07 02:17:01  jiezhang
  Fix some minor errors.

  Revision 1.2  2005/04/06 23:43:20  jiezhang
  Remove the old HTTP server C code and check in the HTTP server C++ code.

  Revision 1.1  2005/03/06 10:35:01  qjhart
  Added some CPP versions of some files.  Updated Geostreamd

  Revision 1.1  2004/11/30 19:01:15  qjhart
  New server


*/
#include <iostream>
#include <signal.h>
#include <getopt.h>

#include "geostreamd.h"
#include "GeostreamServerConfig.h"
#include "GvarStream.h"

char Debug = 0;
char Verbose = 0;
char Daemon = 0;

Geostream::Log *gLog = NULL ;

// time_t Time_Cur;
// char Time_Str[TIME_STR_LEN];
int SeqNum ;

static const char rcsid[] =
  "$Id: geostreamd.cpp,v 1.6 2005/06/03 18:59:16 qjhart Exp $";

static int
mysignal (int sig, void (*f) (int))
{
  struct sigaction act;

  act.sa_handler = f;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  return sigaction (sig, &act, 0);
}

void
cleanup (int sig)
{
  // We should do some freeing from here
  delete gLog ;
  exit (0);
}

int
main (int argc, char *argv[])
{
  Geostream::GvarStream *gvar;
  struct gs_config config;

  mysignal (SIGPIPE, SIG_IGN);
  mysignal (SIGINT, cleanup);

  config.conf_file = "geostreamd.conf";
  config.facility = Geostream::DEFAULT_LOG_FACILITY;

  /* Defaults */
  config.def = 
    new Geostream::GeostreamServer 
      (6666, 2, Geostream::EVENTS, Geostream::EVENTS,5) ;
//   config.def->gvar="169.237.153.55:21009";
//   config.def->gvar_name="169.237.153.55";
//   config.def->gvar_port=21009;
 
  static struct option long_options[] = {
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"host",0,0,'h'},
#ifdef USE_CONFIG_FILE
    {"config-file", 1, 0, 'f'},
#else
    {"port", 1, 0, 'p'},
    {"max-clients", 1, 0, 'n'},
    {"local-facility", 1, 0, 'l'},
    {"request-style",1,0,'q'},
    {"write-style",1,0,'w'},
#endif
    {0, 0, 0, 0}
  };

  while (1)
    {
      int option_index = 0;
      int c = getopt_long (argc, argv, "dvf:h:p:n:R:l:q:w:",
                           long_options, &option_index);

      if (c == -1)
        break;
      switch (c)
        {
#ifdef USE_CONFIG_FILE
        case 'f':
          config.conf_file = optarg;
          break;
#else
	case 'q':
	  if (strcasecmp(optarg,"THREADS")==0)
	    config.def->m_reqStyle = Geostream::THREADS;
	  else
	    config.def->m_reqStyle = Geostream::EVENTS;
	  break;
	case 'w':
	  if (strcasecmp(optarg,"THREADS")==0)
	    config.def->m_writeStyle = Geostream::THREADS;
	  else
	    config.def->m_writeStyle = Geostream::EVENTS;
	  break;	      
        case 'l':
          switch (atoi (optarg))
            {
            case (0):
              config.facility = LOG_LOCAL0;
              break;
            case (1):
              config.facility = LOG_LOCAL1;
              break;
            case (2):
              config.facility = LOG_LOCAL2;
              break;
            case (3):
              config.facility = LOG_LOCAL3;
              break;
            case (4):
              config.facility = LOG_LOCAL4;
              break;
            case (5):
              config.facility = LOG_LOCAL5;
              break;
            case (6):
              config.facility = LOG_LOCAL6;
              break;
            case (7):
              config.facility = LOG_LOCAL7;
              break;
            default:
              break;
            }
          break;
        case 'p':
          config.def->m_port = atoi (optarg);
          break;
        case 'n':
          config.def->m_numOfConns = atoi (optarg);
          break;
#endif
        case 'h':
          config.def->m_hostname = optarg;
          break;
        case 'd':
          Debug = 1;
        case 'v':
          Verbose = 1;
          break;
        default:
          break;
        }
    }

  gLog = 
    new Geostream::Log 
    ("geostreamd", ((Verbose) ? LOG_PERROR : 0), config.facility,
     "geostreamd.access.log", "geostreamd.log");

  try {
    gLog->init () ;
  } catch (Geostream::Lib::IllegalStateException e) {
    std::cerr << e.getErrorMessage () << "\n" ;
    exit (1) ;
  }

#ifdef USE_CONFIG_FILE
  if (config_file (&config)) {
    gLog->add (LOG_ERR, "Server Configuration Error");
    exit (1) ;
  }
#endif

  uri_mode_set (URI_MODE_CANNONICAL);

  /* Get second counter */
  // time (&Time_Cur);

  /* If this doesn't match, then assume that it's standalone */
  config_print (&config);

  /* Initialize Events */
  event_init ();

  config.def->init ();

  /* This is for daemons, but hasn't been tested well yet */
  if (0) {
    if (fork())
      _exit(0);
    setsid();
    if (fork())
      _exit(0);
  }

  try {
    config.def->start () ;
  } catch (Geostream::Lib::GeostreamServerException e) {
    std::cerr << e.getErrorMessage () << "\n" ;
    exit (1) ;
  }

  event_dispatch();				/* Now get events */

  return 0;
}
