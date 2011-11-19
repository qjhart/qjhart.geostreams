#include "Connection.h"
#include "HttpRequest.h"
#include "GeostreamServer.h"

using namespace Geostream ;

static const char rcsid[] =
  "$Id: Connection.cpp,v 1.4 2005/06/03 18:59:16 qjhart Exp $" ;

// Maybe these should be in one big array that I can use everywhere.
const char* Connection::RES_Info[] = {
  "Continue",                   /*100 */
  "Switching Protocols"         /*101 */
};

const char* Connection::RES_Success[] = {
  "OK",                         /* 200 */
  "Created",
  "Accepted",
  "Non Authoritative Information",      /* 203 */
  "No Content",                 /* 204 */
  "Reset Content",              /* 205 */
  "Partial Content"             /* 206 */
};

const char* Connection::RES_Redirection[] = {
  "Multiple Choices",           /* 300 */
  "Moved Permanently",          /* 301 */
  "Found",                      /* 302 */
  "See Other",                  /* 303 */
  "Not Modified",               /* 304 */
  "Use Proxy",                  /* 305 */
  "Temporary Redirect"          /* 307 */
};

const char* Connection::RES_Client_Error[] = {
  "Bad Request",                /* 400 */
  "Unauthorized",               /* 401 */
  "Payment Required",           /* 402 */
  "Forbidden",                  /* 403 */
  "Not Found",                  /* 404 */
  "Method Not Allowed",         /* 405 */
  "Not Acceptable",             /* 406 */
  "Proxy Authentication Required",      /* 407 */
  "Request Timeout",            /* 408 */
  "Conflict",                   /* 409 */
  "Gone",                       /* 410 */
  "Length Required",            /* 411 */
  "Precondition Failed",        /* 412 */
  "Request Entity Too Large",   /* 413 */
  "Request URI Too Large",      /* 414 */
  "Unsupported Media Type",     /* 415 */
  "Requested range not satisfiable",    /* 416 */
  "Expectation Failed"          /* 417 */
};

const char* Connection::RES_Server_Error[] = {
  "Internal Server Error",      /* 500 */
  "Not Implemented",            /* 501 */
  "Bad Gateway",                /* 502 */
  "Service Unavailable",        /* 503 */
  "Gateway Timeout",            /* 504 */
  "HTTP Version not supported"  /* 505 */
};

Connection::Connection (GeostreamServer* server, MultiType writeStyle) :
  m_server (server),
  m_writeStyle (writeStyle) {
  
  m_buffer = new HttpRequestBuffer (CLIENT_BUFFER_SIZE,writeStyle) ;
  if (m_buffer == NULL) {
    std::string errMsg = "Can not allocate a buffer." ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  m_request = new HttpRequest () ;
  try {
    m_request->newRequest (this) ;
  } catch (Geostream::Lib::IllegalStateException e) {
    throw Geostream::Lib::GeostreamServerException (e.getErrorMessage ()) ;
  }

  m_requestEvent = (struct event*)malloc (sizeof (struct event)) ;
  if (m_requestEvent == NULL) {
    std::string errMsg = "Can not allocate an event structure." ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  m_requestThread = (pthread_t *)malloc (sizeof (pthread_t)) ;
  if (m_requestThread == NULL) {
    std::string errMsg = "Can not allocate a thread structure." ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  m_keepAlive = 0 ;
  m_fn = 0 ;
  m_fnZ = 0 ;
}

Connection::~Connection () {
  delete m_request ;
  delete m_buffer ;
  delete m_requestEvent ;
  delete m_requestThread ;

  if (m_fn != NULL) {
    delete[] m_fn ;
  }
}

void Connection::reset (int keepAlive) {
  m_buffer->reset () ;
  m_request->close () ;

  if (keepAlive) {
    if (m_writeStyle == EVENTS) {
      event_set (m_requestEvent, m_sock, EV_READ,
		 Connection::handleRequestEvent, this) ;
      event_add (m_requestEvent, NULL) ;
    }
  }

  else {
    if (m_sock != 0) {
      close (m_sock) ;
    }
    m_server->freeConnection (this) ;
  }
}

int Connection::accept (MultiType reqStyle)
  throw (Geostream::Lib::GeostreamServerException) {
  m_reqStyle = reqStyle ;
  m_sock = 0 ;

  m_sock = m_server->accept () ;
  if (m_sock == -1) {
    std::string errMsg = "Can not accept a connection." ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  fcntl (m_sock, F_SETFD, FD_CLOEXEC) ;

  if (m_reqStyle == EVENTS) {
    fcntl (m_sock, F_SETFL, O_NONBLOCK) ;
    event_set (m_requestEvent, m_sock, EV_READ, 
	       Connection::handleRequestEvent, this) ;
    event_add (m_requestEvent, NULL) ;
  } else {
    pthread_create (m_requestThread, NULL, 
		    (void *(*)(void *))Connection::handleRequestThread,
		    (void *)this) ;
	pthread_detach (*(conn->m_requestThread)) ;
  }

  return m_sock ;
}

int Connection::cork (int on)
  throw (Geostream::Lib::GeostreamServerException) {
  int rc = setsockopt (m_sock, IPPROTO_TCP, TCP_CORK, &on, sizeof (on)) ;
  if (rc == -1) {
    std::string errMsg = "setsockopt error." ;
    debug ("ERROR: %s\n", errMsg.c_str ()) ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  return rc ;
}

/*
  prepareHeader - Fills in Connection m_buffer with the Header
  information.  Needs the response code from the URL request to answer
  correctly.  On return, the m_buffer has headers in it.

*/
void Connection::prepareHeader () {

  m_buffer->reset () ;

  const char *reason ;
  int rc = m_responseCode ;

  // status line
  if (rc >= 100 && rc <= 101)
    reason = RES_Info[rc - 100];
  else if (rc >= 200 && rc <= 206)
    reason = RES_Success[rc - 200];
  else if (rc >= 300 && rc <= 307)
    reason = RES_Redirection[rc - 300];
  else if (rc >= 400 && rc <= 417)
    reason = RES_Client_Error[rc - 400];
  else if (rc >= 500 && rc <= 505)
    reason = RES_Server_Error[rc - 500];
  else
    reason = "UNKNOWN";

  m_buffer->printf ("HTTP/1.1 %d %s\r\n", rc, reason) ;

  time_t newTime ;
  struct tm tm ;
  gmtime_r (&newTime, &tm) ;
  strftime (m_timeStr, TIME_STR_LEN, "%a, %d %b %Y %T GMT", &tm) ;

  // Default header
  m_buffer->printf 
    ("Server: geostreamd liburi/2.13\r\nDate: %s\r\nLocation: %s\r\nContent-Type: %s\r\nConnection: %s\r\n\r\n",
     m_timeStr,
     uri_uri (m_request->getURI ()),
     "multipart/x-mixed-replace;boundary=---ThisRandomString---", 
	 (m_keepAlive) ? "Keep-Alive" : "closed");
}

/* 
   Read the connection request, into the m_buffer, and parse it.  You
   have to parse as you go, since you never know when the request is
   going to stop.

   Returns the HttpRequest Status CODE.  In this code we can find
   RES_BadRequest, and RES_KeepWorking,
   
*/
int Connection::getResponseCode () {
  ssize_t count = m_buffer->readFrom (m_sock) ;

  switch (count) {
    case -1:
      // debug("buffer: %s", strerror(errno));
      return -1;
    case 0:
      return HttpRequest::RES_CLOSED;
    case CLIENT_BUFFER_SIZE:
      return 414;
  }

  /* Scan the m_buffer, and let us know if a valid request is there.
   */
  RequestState rs = m_request->scan (m_buffer) ;
  if (rs == ERR)
      return HttpRequest::RES_BadRequest;
  if (rs != REQ)		/* Keep Reading */
      return HttpRequest::RES_WORKING;
  if (m_buffer->rewind () == -1)
       return 414;
  int rc = m_request->parse (m_buffer->readp ()) ;
  if (rc != HttpRequest::RES_GOODREQUEST)
    return rc;
#if 1
  if (m_request->m_keepAlive)
    m_keepAlive = 1;
#endif


  // Right now only GET Requests are handled
  // Now, fill in the uri for the request
  RequestMethod rm = m_request->getRequestMethod () ;
  if (rm == HEAD || rm == GET) {
    uri_t *uri = m_request->getURI () ;
    char* furi = uri_path (uri) ;
    int fnZ = strlen (furi) + 128 ;
    if (m_fnZ < fnZ) {
      m_fn = (char*)realloc (m_fn, fnZ) ;
      if (m_fn == NULL) {
	return -1 ;
      }
      m_fnZ = fnZ ;
    }

    sprintf (m_fn, "%s", furi) ;
    return HttpRequest::RES_OK ;
  }

  else if (rm == POST) {
    return 501 ;
  }

  return 501 ;
}


void Connection::handleRequestEvent (int sock, short event, void *arg) {
  Connection *conn = (Connection*)arg ;

  conn->m_responseCode = conn->getResponseCode () ;
  switch (conn->m_responseCode) {
  case (HttpRequest::RES_CLOSED):
    conn->reset (0);
    break;
  case (-1):
  case (HttpRequest::RES_WORKING):
    event_add (conn->m_requestEvent, NULL); /* Redo */
    break;
  case (414):
  case (HttpRequest::RES_BadRequest):
  default:
    conn->prepareHeader () ;
	callback->callback=Connection::ResponseSendEvent;
	conn->writeToCallBack(conn->sock,NULL,callback);
    break;
  }
}

void Connection::ResponseSendEvent() {
  if (conn->m_request->getRequestMethod () == GET &&
	  conn->m_responseCode == 200) {
	if (conn->m_writeStyle == EVENTS) {
	  conn->cork (1) ;
	  event_set (conn->m_requestEvent, conn->m_sock, EV_WRITE, 
				 Connection::eventSend, conn) ;
	  event_add (conn->m_requestEvent, NULL) ;
	} else {
	  pthread_create 
		(conn->m_requestThread, NULL,
		 (void *(*)(void *))Connection::threadSend, (void*)conn) ;
	  pthread_detach (*(conn->m_requestThread)) ;
	}
  } else {
	conn->reset (conn->m_keepAlive) ;
  }
  conn->LineDocSender();
}

 
void Connection::threadSend (Connection* conn) {
  conn->cork (1) ;
  int count = conn->send () ;
  conn->cork (0) ;

  if (count == -1) {
    conn->m_keepAlive = 0 ;
  }
  conn->reset (conn->m_keepAlive) ;
}

void Connection::eventSend (int sock, short event, void *arg) {
  Connection* conn = (Connection*)arg ;

  int count = conn->send () ;
  if (count == -1) {
    if (errno == EAGAIN) {
      event_add (conn->m_requestEvent, NULL) ; // redo send
    }

    else {
      conn->cork (0) ;
      conn->reset (0) ;
    }
  }

  // hack
  else if (count > 5) {
    conn->cork (0) ;
    conn->reset (conn->m_keepAlive) ;
  }

  else {
    event_add (conn->m_requestEvent, NULL) ; // redo send
  }
}

/*
  This threaded request handler does an entire request in one
  function.  It parses repsonse, prepares the header, sends the
  m_buffer, and then sends the file.  (In the old version).  The new
  version should be prepare header, send_buffer, while read (fifo) ->
  prepare_buffer -> send_buffer.  Where does the FIFO get created?
*/
void Connection::handleRequestThread (Connection* conn) {
  while (1) {
    conn->m_responseCode = conn->getResponseCode () ;
    switch (conn->m_responseCode) {
    case (HttpRequest::RES_CLOSED):
      break;
    case (-1):
      break;
    default:
      conn->prepareHeader () ;
      int count ;
      count = conn->m_buffer->writeTo(conn->m_sock);
	  
      if (conn->m_request->getRequestMethod () == GET &&
		  conn->m_responseCode == 200) {
		conn->cork (1) ;
		conn->LineDocSenderThread;
		conn->cork (0) ;
      }
	  
      conn->reset (1) ;
      if (conn->m_keepAlive) {
		continue ;
      }
    }	
    break ;
  }
  conn->reset (0) ;
}

/* 
   eventSend and threadSend all call this method to actually send the
   data.

   This currently sends the information.  It came from the last
   program, where this was a bufferless transform, but will have to be
   changed now that we have a bufferful transform method.  I will
   change this to be a write buffer, send buffer pair.
*/

void Connection::LineDocSenderThread () {
  Gvar::LineDoc *lineDoc;

 
  m_LineDocReader = m_server->newLineDocReader();
  
  while (lineDoc = m_LineDocReader->read()) {
	m_buffer->reset();
	m_buffer->printf("--BUFFER--HERE\n");
	m_buffer->printf("spcid=%d spsid=%d lside=%d lidet=%d licha=%d risct=%d l1scan=%d	l2scan=%d lpixls=%d lwords=%d lzcor=%d",
					 lineDoc->spcid(),lineDoc->spsid(),lineDoc->lside(),
					 lineDoc->lidet(),lineDoc->licha(),lineDoc->risct(),
					 lineDoc->l1scan(),lineDoc->l2scan(),lineDoc->lpixls(),
					 lineDoc->lwords(),lineDoc->lzcor());
	m_buffer->writeTo(m_sock);
  }
  return;
}

