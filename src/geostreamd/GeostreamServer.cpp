#include "GeostreamServer.h"
#include "Connection.h"

using namespace Geostream ;

static const char rcsid[] =
  "$Id: GeostreamServer.cpp,v 1.4 2005/06/03 18:59:16 qjhart Exp $" ;

GeostreamServer::GeostreamServer 
(int port, unsigned int numOfConns, 
 MultiType reqStyle, MultiType writeStyle, int lineDocSize) : 
  m_port (port),
  m_numOfConns (numOfConns),
  m_reqStyle (reqStyle),
  m_writeStyle (writeStyle),
  m_fifoLineDocSize (lineDocSize){
}

GeostreamServer::~GeostreamServer () {
}

void GeostreamServer::init ()
throw (Geostream::Lib::GeostreamServerException) {
  m_connEvent = (struct event*)malloc (sizeof (struct event)) ;
  m_timerEvent = (struct event*)malloc (sizeof (struct event)) ;

  if (m_connEvent == NULL || m_timerEvent == NULL) {
    std::string errMsg = "Can not allocate an event." ;
    throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  // create the free connections
  for (int connNo=0; connNo < m_numOfConns; connNo++) {
    Connection * conn = new Connection (this, m_writeStyle) ;
    m_freeConns.push_back (conn) ;
  }
}

void GeostreamServer::start ()
throw (Geostream::Lib::GeostreamServerException) {
  m_gvarStream = new Geostream::GvarStream(m_gvarName,m_gvarPort);

  m_gvarStream->listen();			/* GVAR events */

  gLog->add(LOG_INFO,"Starting port: %d,connections: %d, req:%s,write:%s", 
	    m_port, m_numOfConns,
	    (m_reqStyle==Geostream::EVENTS)?"EVENTS":"THREADS",
	    (m_writeStyle==Geostream::EVENTS)?"EVENTS":"THREADS"
	    );

  try {
    Socket::create () ;

    if (m_reqStyle == EVENTS) {
      fcntl (m_sock, F_SETFL, O_NONBLOCK) ;
    }

    Socket::bind (m_port) ;
    Socket::listen () ;
  } catch (Geostream::Lib::SocketException e) {
    throw Geostream::Lib::GeostreamServerException (e.getErrorMessage ()) ;
  }

  evtimer_set (m_timerEvent, GeostreamServer::timeKeeper, this) ;
  event_set (m_connEvent, m_sock, EV_READ, 
	     GeostreamServer::newConnection, this) ;
  event_add (m_connEvent, NULL) ;
}

int GeostreamServer::accept ()
  throw (Geostream::Lib::GeostreamServerException) {
  try {
    return Socket::accept () ;
  } catch (Geostream::Lib::SocketException e) {
    throw Geostream::Lib::GeostreamServerException (e.getErrorMessage ()) ;
  }
}

void GeostreamServer::newConnection (int sock, short event, void* arg) {
  GeostreamServer* gServer = (GeostreamServer*)arg ;

  if (gServer->m_freeConns.size () == 0) {
    gLog->add (LOG_ERR, "All connections are busy.") ;
    return ;
//     std::string errMsg = "All connections are busy." ;
//     throw Geostream::Lib::GeostreamServerException (errMsg) ;
  }

  // Connection* conn = gServer->m_freeConns.pop_front () ;
  Connection* conn = gServer->m_freeConns.front () ;
  gServer->m_freeConns.pop_front () ;

  event_add (gServer->m_connEvent, NULL) ;

  try {
    if (conn->accept (gServer->m_reqStyle) == -1) {
      gServer->m_freeConns.push_back (conn) ;
    } 

    else {
      gServer->m_busyConns.push_back (conn) ;
    }
  } catch (Geostream::Lib::GeostreamServerException e) {
    gLog->add (LOG_ERR, (e.getErrorMessage ()).c_str ()) ;
  }
}

void GeostreamServer::timeKeeper (int sock, short event, void *arg) {
  GeostreamServer* gServer = (GeostreamServer*)arg ;

  struct tm tm ;
  time_t new_time ;
  struct timeval tv ;
  timerclear (&tv) ;
  tv.tv_sec = 1 ;

  time (&new_time) ;
  gmtime_r (&new_time, &tm) ;
  strftime (gServer->m_timeStr, TIME_STR_LEN, 
	    "%a, %d %b %Y %T GMT", &tm) ;

  event_add(gServer->m_timerEvent, &tv) ;
}

void GeostreamServer::freeConnection (Connection *conn) {
  // move conn from m_busyConns to m_freeConns
  m_busyConns.remove (conn) ;
  m_freeConns.push_back (conn) ;
}

FifoReader<Gvar::LineDoc> *GeostreamServer::newLineDocReader() {
  if (m_fifoLineDoc == NULL) {
	m_fifoLineDoc = new Fifo<Gvar::LineDoc>(m_fifoLineDocSize);
	m_gvarStream->LineDocWriter(m_fifoLineDoc->newWriter());
  }
  return m_fifoLineDoc->newReader();
}	
