// Apache stuff
//  Its important to use "" instead of <> and to have the -I flags in
//  the right order in the Makefile because there is an Apache alloc.h
//  that is completely different from the system alloc.h.
//
//  $Id: mod_geostream.cpp,v 1.13 2007/09/07 08:44:27 crueda Exp $
///////////////////////////////////////////////////////////////////////

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_main.h"
#include "util_script.h"
//#include "ap_config.h"
#include "http_log.h"
#include "ap_compat.h"
#include "apr_strings.h"
#include "apr_thread_proc.h"

// Apache's compatibility warnings are of no concern to us.
#undef strtoul

// System include files
#include <string>
#include <pthread.h>
#include <sstream>

#include <list>

// Geostream Stuff
#include <event.h>
#include "GvarStream.h"
#include "Fifo.h"
#include "ShmRowFifo.h"
#include "Gvar.h"
#include "ImgStream.h"
#include "GvarConverter.h"

/************************
 * Forward Declarations *
 ************************/
extern "C" module MODULE_VAR_EXPORT geostream_module;

/**************************
 * Configuration Handlers *
 **************************/
  
/* per server configuration structure. */
typedef struct {
  apr_pool_t *p;
  // These should be in Apache Config file. --> They are now :-) (carueda 9/6/07)
  int gvarPort;
  char *gvarName;
  Geostream::GvarStream *gvar ;
  struct {
    int size;
    Geostream::Fifo<Gvar::LineDoc>* fifo; 
    Geostream::FifoWriter<Gvar::LineDoc> *writer;
  } linedoc;

//   struct {
//     int sizes[NUM_OF_CHANNELS] ;
//     Geostream::Fifo<Row>* fifos[NUM_OF_CHANNELS];
//     Geostream::FifoWriter<Row>* writers[NUM_OF_CHANNELS] ;
//   } rows ;

  struct {
    int rowBufSize ;
    Geostream::Fifo<Row>* rowFifo ;
    Geostream::FifoWriter<Row>* rowWriter ;
  } allRows ;
} geostream_server_config ;


// Initialize some server parameters
static void* geostream_create_server_config (apr_pool_t *p, server_rec* server) {
  geostream_server_config *cfg =
    (geostream_server_config*)ap_pcalloc (p, sizeof (geostream_server_config));

  // this initialization is overwritten by the new configuration handling (carueda 9/6/07)
  cfg->gvarPort = 21009;
  cfg->gvarName = "169.237.153.55" ;

  return (void*)cfg ;

}


// a simple strategy is to update a certain file whenever a block0doc block comes
// in the reader thread; then, the response to "ElugParams" request would simply
// write out the current contents of that file ... yeah, this requires locks or
// some other synchronization, but hey, I'm postponing this anyway ...
// not much time to work on this for now; besides, a better approach
// would be using shared memory and related synchronization as already done with
// the rows; perhaps also some timers in case that block0doc comes too fast ... 
// TODO (maybe)
static void elugParams_update(Gvar::Block0Doc* block0doc, const char* out_filename) {

	fprintf (stderr, "elugParams_update\n") ;
	fflush(stderr);

	// hmm, I need the pool -- how?
	 //file* out_file = ap_pfopen(out_filename, "wb");
}


/**
 * Start the gvar reader thread.
 */
void* gvar_reader (apr_thread_t* writer_thread, void* ptr) {
  geostream_server_config *cfg = (geostream_server_config *)ptr ;

  fprintf (stderr, "Init a THREAD, which should be only once.\n") ;
  fflush(stderr);

  Geostream::GvarStream gvar (cfg->gvarName, cfg->gvarPort) ;

  Geostream::RowFifo* rowFifo = Geostream::createRowFifo () ;

  if (rowFifo == NULL) {
    fprintf (stderr, "Can not continue.\n") ; fflush (stderr) ;
    apr_thread_exit (writer_thread, 0) ;
  }

  gvar.shmRowFifo (rowFifo) ;

  Geostream::RowFifoWriter* rowWriter = Geostream::newWriter (rowFifo) ;
  gvar.shmRowsWriter (rowWriter) ;

  while (1) {
    bool succeed = gvar.listen () ;

    while (succeed) {
		succeed = gvar.read();

		// the following, to dispatch ElugParams requests -- 
		// Not being executed --- incomplete implementation TODO
		// NOTE: since I just added the getLastBlock0()  GvarStreami.h,
		// a full compilation would be necessary.
		if ( 0 && succeed ) {
			Gvar::Block0* block0 = gvar.getLastBlock0();
			Gvar::Block0Doc* block0doc = block0->getBlock0Doc();

			// this out fiwill be a onfig parameter when implementation get completed
			const char* out_filename = "/tmp/elugParams";
			elugParams_update(block0doc, out_filename);
		}

    }

    gvar.close () ;

    apr_sleep (10) ;
  }
  apr_thread_exit (writer_thread, 0) ;
}

/**
 * This will run twice when apache starts. Only on the second time,
 * the gvar reader starts.
 */
static int postConfig (apr_pool_t *pconf, apr_pool_t *plog,
			apr_pool_t *ptemp, server_rec *server) {

  geostream_server_config *cfg =
    (geostream_server_config*)
    ap_get_module_config (server->module_config, &geostream_module) ;

  void *data = NULL;
  short first_time = 0;
  const char *userdata_key = "geostream_postconfig";
         
  fprintf (stderr, "calling postconf\n"); fflush (stderr) ;

  apr_pool_userdata_get(&data, userdata_key, server->process->pool) ;
  if (data == NULL) {
    // This is entered only on the first time.
    first_time = 1;
    apr_pool_userdata_set("1", userdata_key,
			  apr_pool_cleanup_null, server->process->pool) ;
  }

  // if this is the second time to call this, then create the thread
  if (!first_time) {
    apr_thread_t* writer_thread ;
    apr_status_t rv = 
      apr_thread_create (&writer_thread, NULL, gvar_reader, (void*)cfg, pconf) ;
    if (rv != APR_SUCCESS) {
      fprintf (stderr, "Failed to create the Gvar Reader thread.\n") ;
      fflush (stderr) ;
    }
  }

  return OK ;
}

/********************
 * Content Handlers *
 ********************/

class ApacheStreamWriter : public StreamWriter {
public:
  ApacheStreamWriter (request_rec *r) : m_r (r) {
  }
  ~ApacheStreamWriter () {
  }

  int writeBuffer(const void* buf, int nbytes) {
    int bytesWritten = ap_rwrite (buf, nbytes, m_r) ;
    ap_rflush (m_r) ;

//     if (bytesWritten != nbytes) {
//       fprintf (stderr, "bytesWritten=%d", bytesWritten) ;
//       fflush (stderr) ;
//     }
    return (bytesWritten == nbytes ? 0 : -1) ;
  }

private:
  request_rec *m_r ;
} ;

/**
 * Return all channel information to the client.
 */
static void writeInfo (geostream_server_config *cfg, request_rec *r) {

  for (int channelNo=0; channelNo<NUM_OF_CHANNELS; channelNo++) {
    ChannelDef channel_def
      (Geostream::channelsDesc[channelNo], channelNo,
       Geostream::xRes[channelNo], Geostream::yRes[channelNo], 16./18., 10);

    ostringstream ostr;
    ostr<< "CH"
	<< " " <<channel_def.getID ()
	<< " " <<channel_def.getPixelSizeX ()
	<< " " <<channel_def.getPixelSizeY ()
	<< " " <<channel_def.getAspect()
	<< " " <<channel_def.getSampleSize()
	<< " " <<channel_def.getName()
	<< " " << " ;-)"
	<< "\n"
      ;
    string str = ostr.str();

    ap_rputs (str.c_str (), r) ;
    ap_rflush (r) ;
  }
}


/**
 * write the Gvar params out to the client
 */
static void writeGvarConfig(geostream_server_config *cfg, request_rec *r) {
	ostringstream ostr;
	ostr<< "GVAR server:" << endl
	    << "  GvarName=" <<cfg->gvarName << endl
	    << "  GvarPort=" <<cfg->gvarPort << endl
	;
	string str = ostr.str();
	ap_rputs(str.c_str (), r) ;
	ap_rflush(r) ;
}


/**
 * write the ELUG params out to the client
 * TODO Not implemented yet
 */
static void writeElugParams(geostream_server_config *cfg, request_rec *r) {
	ostringstream ostr;
	ostr<< "Sorry: ElugParams operation no yet implemented:" << endl;
	string str = ostr.str();
	ap_rputs(str.c_str (), r) ;
	ap_rflush(r) ;
}





/* ------  Not used
static void writeLineDoc (geostream_server_config *cfg, request_rec *r) {
  // Create a new Reader
  Geostream::FifoReader<Gvar::LineDoc> *reader = 
    cfg->linedoc.fifo->newReader();

  int count = 0 ;
  Gvar::LineDoc *lineDoc;
  while ( ( lineDoc = reader->read() ) ) {
    r->content_type = "multipart/mixed;boundary=linedocrandomstr";
    ap_send_http_header(r) ;

    // Generate the message.
    std::stringstream messagetosend ; 

    messagetosend << "\n--linedocrandomstr\n" ;
    messagetosend << "Content-type: text/plain\n\n" ;
    messagetosend << "spcid=" << lineDoc->spcid () 
		  << " spsid=" << lineDoc->spsid()
		  << " lside=" << lineDoc->lside()
		  << " lidet=" << lineDoc->lidet()
		  << " licha=" << lineDoc->licha()
		  << " risct=" << lineDoc->risct()
		  << " l1scan=" << lineDoc->l1scan()
		  << " l2scan=" << lineDoc->l2scan()
		  << " lpixls=" << lineDoc->lpixls()
		  << " lwords=" << lineDoc->lwords()
		  << " lzcor=" << lineDoc->lzcor()
		  << "\n"
      ;

    if (!r->header_only) {
      ap_rputs((messagetosend.str()).c_str(), r);
      ap_rflush (r) ;
    }

    count ++ ;
    if (count == 100) {
      break ;
    }

  }

  // Generate the message.
  std::stringstream messagetosend ; 
  
  messagetosend << "\n--linedocrandomstr--\n" ;

  if (!r->header_only) {
    ap_rputs((messagetosend.str()).c_str(), r);
    ap_rflush (r) ;
  }

}

----- */


/**
 * Parse one parameter in the query string.
 */
bool parseParam (char* param,
		 std::list<char*>& params, 
		 std::list<char*>& values) {
  char* lastTok ;

  // should have at most two tokens in this case
  char* tok1 = apr_strtok (param, "=", &lastTok) ;
  char* tok2 = apr_strtok (NULL, "=", &lastTok) ;

  if (tok2 != NULL) {
    char* nullTok = apr_strtok (NULL, "=", &lastTok) ;

    if (nullTok != NULL) {
      return false ;
    }
  }

  params.push_back (tok1) ;
  values.push_back (tok2) ;

  return true ;
}

/**
 * Parse the query string into two lists. The total number of
 * parameters are also returned.
 */
bool parseQueryString (apr_pool_t *pool,
		       char* uri,
		       std::list<char*>& params, 
		       std::list<char*>& values, 
		       int& numOfParams) {
  numOfParams = 0 ;

  // separate parameters
  char *paramLastTok ;
  char* paramTok = apr_strtok (uri, "&", &paramLastTok) ;

  while (paramTok != NULL) {
    if (!parseParam (paramTok, params, values)) {
      return false ;
    }
    numOfParams ++ ;

    paramTok = apr_strtok (NULL, "&", &paramLastTok) ;
  }

  return true ;
}

static void writeChannelsData 
(geostream_server_config *cfg, Geostream::ClientRequest &clientRequest, 
  request_rec *r) {

  fprintf (stderr, "calling writingChannelsData\n") ; fflush (stderr) ;

  ApacheStreamWriter apWriter (r) ;

  ReferenceSpace rs(5500, 2200, 27000, 14000);

  Geostream::RowFifo* rowFifo = Geostream::retrieveRowFifo () ;

  Geostream::GvarConverter converter
    (rowFifo, clientRequest, &rs, &apWriter) ;

  converter.convert () ;
}

/* The main event */
static int geostream_handler(request_rec *r)
{

  fprintf (stderr, "receive a request: %s\n", r->unparsed_uri) ; fflush (stderr) ;

  if (strncmp (r->uri, "/geostream", strlen ("/geostream")) != 0) {
    return DECLINED ;
  }

  // Get the config for this request.
  geostream_server_config *cfg =
    (geostream_server_config*)
    ap_get_module_config (r->server->module_config, &geostream_module) ;

  // parse the query string into two lists
  std::list<char*> params ;
  std::list<char*> values ;
  int numOfParams ;
  parseQueryString (r->pool, r->args, params, values, numOfParams) ;

  std::list<char*>::iterator paramItr = params.begin () ;
  std::list<char*>::iterator valueItr = values.begin () ;

  Geostream::ClientRequest clientRequest ;

  while (paramItr != params.end () && valueItr != values.end ()) {

    // channel parameter
    if (apr_strnatcmp (*paramItr, "channels") == 0) {
      char* channels = *valueItr ;

      char* lastTok ;
      char* tok = apr_strtok (channels, ",", &lastTok) ;
      fprintf (stderr, "tok=%s\t", tok) ; fflush (stderr) ;
      while (tok != NULL) {
	int channelNo = atoi (tok) ;
	if (channelNo >= NUM_OF_CHANNELS ||
	    channelNo < 0) {
	  return 400 ; // bad request
	}

	clientRequest.isChannelRequested[channelNo] = true ;
	tok = apr_strtok (NULL, ",", &lastTok) ;
	fprintf (stderr, "tok=%s\t", tok) ; fflush (stderr) ;
      }
    }

    // format parameter
    else if (apr_strnatcmp (*paramItr, "format") == 0) {
      if (*valueItr == NULL) {
	return 400 ; // bad request
      }

      clientRequest.format = (Format)(atoi(*valueItr)) ;
      fprintf (stderr, "format=%d\n", clientRequest.format); fflush (stderr) ;
    }

    // getInfo parameter
    else if (apr_strnatcmp (*paramItr, "getInfo") == 0) {
      clientRequest.requestGetInfo = true ;
    }

    // bbox parameter
    else if (apr_strnatcmp (*paramItr, "bbox") == 0) {
      char* bboxStr = *valueItr ;

      char* lastTok ;
      char* tok = apr_strtok (bboxStr, ",", &lastTok) ;

      int* bbox[4] = {&(clientRequest.bbox.x), &(clientRequest.bbox.y),
		      &(clientRequest.bbox.width), &(clientRequest.bbox.height)} ;

      int index = 0 ;
      while (tok != NULL) {
	if (index == 4) {
	  return 400 ;
	}

        *(bbox[index++]) = atoi (tok) ;

        tok = apr_strtok (NULL, ",", &lastTok) ;
      }

      fprintf (stderr, "bbox: [%d, %d, %d, %d]\n", 
	       clientRequest.bbox.x, clientRequest.bbox.y, 
	       clientRequest.bbox.width, clientRequest.bbox.height) ; 
      fflush (stderr) ;
    }

    // nodata parameter
    else if (apr_strnatcmp (*paramItr, "nodata") == 0) {
      if (*valueItr != NULL) {
	return 400 ; // bad request
      }

      clientRequest.nodata = true ;
    }

    // New "service" to show the GVAR server connection parameters (carueda 9/6/07)
    else if (apr_strnatcmp (*paramItr, "GvarConfig") == 0) {
		r->content_type = "text/plain";
		ap_send_http_header(r) ;
		writeGvarConfig(cfg, r) ;
		return OK;
    }

    // New "service" to report the current ELUG parameters (carueda 9/6/07)
    // TODO Not fully implemented yet
    else if (apr_strnatcmp (*paramItr, "ElugParams") == 0) {
		r->content_type = "text/plain";
		ap_send_http_header(r) ;
		writeElugParams(cfg, r) ;
		return OK;
    }

    else {
      fprintf (stderr, "Undefined parameter: %s\n", *paramItr) ; fflush (stderr) ;
      return 400 ;
    }

    paramItr ++ ;
    valueItr ++ ;
  }

  r->content_type = "application/octet-stream";
  ap_send_http_header(r) ;

  if (clientRequest.requestGetInfo) {
    writeInfo (cfg, r) ;
  } else {
    writeChannelsData (cfg, clientRequest, r) ;
  }

  return OK ;
}

static void mod_geostream_register_hooks (apr_pool_t *p) {
  ap_hook_post_config (postConfig, NULL, NULL, APR_HOOK_MIDDLE) ;
  ap_hook_handler(geostream_handler, NULL, NULL, APR_HOOK_MIDDLE) ;
  // ap_hook_child_init (childInit, NULL, NULL, APR_HOOK_FIRST) ;
}

/************************
 * Global Dispatch List *
 ************************/

// Declarations of routines to handle config-file commands

static const char *handle_gvarConf(cmd_parms *cmd, void *cfg, char *arg) {
	geostream_server_config *gcfg = (geostream_server_config*)
		ap_get_module_config(cmd->server->module_config, &geostream_module) ;

	if (apr_strnatcmp(cmd->cmd->name, "GvarName") == 0) {
		gcfg->gvarName = ap_pstrdup(cmd->pool, arg);
		//ap_log_printf(cmd->server, "GvarName set to '%s'", gcfg->gvarName);
	}
	else {
		gcfg->gvarPort = atoi(arg);
		//ap_log_printf(cmd->server, "GvarPort set to %d", gcfg->gvarPort);
	}
	return NULL;
}

// I had to force my handle_gvarConf function via a cast because the command_rec struct expects
// a non-arg function.
static const command_rec geostream_cmds[] = {
	AP_INIT_TAKE1("GvarName", (const char*(*)()) handle_gvarConf, NULL, RSRC_CONF, "GVAR server IP address"),
	AP_INIT_TAKE1("GvarPort", (const char*(*)()) handle_gvarConf, NULL, RSRC_CONF, "GVAR server port"),
	{NULL}
};




// We have to use C style linkage for the API functions that will be
// linked to apache.
extern "C" {
    // Dispatch list for API hooks 
    module MODULE_VAR_EXPORT geostream_module = {
	STANDARD20_MODULE_STUFF, 
	// NULL,                          /* module initializer                  */
	NULL,				  /* create per-dir    config structures */
	NULL,				  /* merge  per-dir    config structures */
	geostream_create_server_config,   /* create per-server config structures */
	NULL,                             /* merge  per-server config structures */
	geostream_cmds,                   /* table of config file commands       */
	//NULL,
	mod_geostream_register_hooks
    };
};
