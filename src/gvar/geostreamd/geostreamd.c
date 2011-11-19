/*
  config - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: geostreamd.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: geostreamd.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/


#include "geostreamd.h"
#include <signal.h>
#include <getopt.h>
#include <time.h>
//#include "../gvarreader.h"


static const char rcsid[] =
  "$Id: geostreamd.c,v 1.1 2004/08/12 17:41:53 singhals Exp $";

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
  log_close ();
  exit (0);
}

int
main (int argc, char *argv[])
{
  struct server *server;
  struct gs_config config;
  char *hostname;

  mysignal (SIGPIPE, SIG_IGN);
  mysignal (SIGINT, cleanup);

  Debug = 0;
  Verbose = 0;
  Daemon = 0;

  config.conf_file = "geostreamd.conf";
  config.facility = DEFAULT_LOG_FACILITY;

  /* Defaults */
  config.def=server_new(NULL); 
  config.def->next=config.def;
  config.def->prev=config.def;
  config.def->files="";
  config.def->host="localhost";
  config.def->port = 6666;
  config.def->num_clients = 2;
  config.def->document_root = "/var/www";
  config.def->document_root_z = strlen (config.def->document_root);
  config.def->req_style = EVENTS;
  config.def->write_style = EVENTS;
  
config.servers=NULL;

  static struct option long_options[] = {
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"host",0,0,'h'},
#ifdef USE_CONFIG_FILE
    {"config-file", 1, 0, 'f'},
#else
    {"port", 1, 0, 'p'},
    {"max-clients", 1, 0, 'n'},
    {"document-root", 1, 0, 'R'},
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
	    config.def->req_style=THREADS;
	  else
	    config.def->req_style=EVENTS;
	  break;
	case 'w':
	  if (strcasecmp(optarg,"THREADS")==0)
	    config.def->write_style=THREADS;
	  else
	    config.def->write_style=EVENTS;
	  break;	      
        case 'l':
          switch (atoi (optarg))
            {
            case (0):
              facility = LOG_LOCAL0;
              break;
            case (1):
              facility = LOG_LOCAL1;
              break;
            case (2):
              facility = LOG_LOCAL2;
              break;
            case (3):
              facility = LOG_LOCAL3;
              break;
            case (4):
              facility = LOG_LOCAL4;
              break;
            case (5):
              facility = LOG_LOCAL5;
              break;
            case (6):
              facility = LOG_LOCAL6;
              break;
            case (7):
              facility = LOG_LOCAL7;
              break;
            default:
              break;
            }
          break;
        case 'p':
          config.def->port = atoi (optarg);
          break;
        case 'n':
          config.def->num_clients = atoi (optarg);
          break;
        case 'R':
          config.def->document_root = optarg;
          break;
#endif
        case 'h':
          hostname = optarg;
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

  log_open ("geostreamd", ((Verbose) ? LOG_PERROR : 0), config.facility);

#ifdef USE_CONFIG_FILE
  if (config_file (&config))
    die ("Server Configuration Error");
#endif

  uri_mode_set (URI_MODE_CANNONICAL);

  /* Get second counter */
  time (&Time_Cur);

  /* If this doesn't match, then assume that it's standalone */
  config_print(&config);
  {
    server=config.def;		/* Single Host */
    struct server *host_p;
    host_p = config.servers;
    do
      {
	if (strcmp(host_p->host,hostname)==0)
	  {
	    server=host_p;
	    break;
	  }
	host_p=host_p->next;
      } while (host_p != config.servers);
  }
  
  
  /* Setup a cache */
  Cache=cache_new(server->cache_size,server->num_clients);

  log_add(LOG_INFO,"Starting port: %d,connections: %d, req:%s,write:%s", 
	  server->port, server->num_clients,
	  (server->req_style==EVENTS)?"EVENTS":"THREADS",
	  (server->write_style==EVENTS)?"EVENTS":"THREADS"
	  );

  log_add(LOG_INFO,"Cache: %d (%d files initialized)", 
	  server->cache_size,server->num_clients   
	  );

  server_init(server);
  server_connect (server);
  /* This is for daemons, but hasn't been tested well yet */
  if (0) {
    if (fork())
      _exit(0);
    setsid();
    if (fork())
      _exit(0);
  }
  if (server_listen (server) != 0)
    {
      perror("server_listen");
      return 1;
    }
  return 0;
}
