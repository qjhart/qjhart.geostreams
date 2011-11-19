#ifndef GEOSTREAM_CONNECTION_H
#define GEOSTREAM_CONNECTION_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "GeostreamServerConfig.h"
#include "lib/Exception.h"
#include "gvar/Gvar.h"
#include "Fifo.h"

extern int SeqNum ;

using namespace Geostream;

class GeostreamServer ;
class HttpRequest ;
class HttpRequestBuffer ;

  class Connection {
  public:
  Connection (GeostreamServer* server, MultiType writeStyle) ;
  ~Connection () ;
  
  void reset (int keepAlive) ;
  int accept (MultiType reqStyle)
	throw (Geostream::Lib::GeostreamServerException);
  int cork (int on) throw (Geostream::Lib::GeostreamServerException) ;
  int send () ;
  void prepareHeader () ;
  int getResponseCode () ;
  
  private:
  GeostreamServer* m_server ;
  MultiType m_writeStyle ;
  HttpRequestBuffer* m_buffer ;
  HttpRequest *m_request ;
  
  int m_sock ;
  MultiType m_reqStyle ;
  bool m_keepAlive ;
  int m_responseCode ;
  char m_timeStr[TIME_STR_LEN] ;
  char *m_fn ;
  int m_fnZ ;
  
  // Test of Streaming work
  Geostream::FifoReader<Gvar::LineDoc> *m_LineDocReader;
  
  struct event* m_requestEvent ;
  pthread_t *m_requestThread ;
  
  static const char* RES_Info[] ; 
  
  static const char* RES_Success[] ;
  
  static const char* RES_Redirection[] ;
  
  static const char* RES_Client_Error[] ;
  
  static const char* RES_Server_Error[] ;
  
  public:
  static const int CLIENT_BUFFER_SIZE = 2048 ;
  
  } ;

class EventConnection : public Connection {
 public:
  virtual static void handleRequest (int sock, short event, void *arg) ;
  virtual static void eventSend (int sock, short event, void *arg) ;
}
class ThreadConnection : public Connection {
 public:
  virtual void handleRequest (Connection* conn) ;
  virtual void threadSend (Connection* conn) ;
}


#endif
