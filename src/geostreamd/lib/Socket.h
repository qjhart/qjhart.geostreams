#ifndef GEOSTREAM_LIB_SOCKET_H
#define GEOSTREAM_LIB_SOCKET_H

/**
 * Define a class to handle the socket connection.
 *
 * @author jiezhang - created on Mar 28, 2005.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

#include "Exception.h"

namespace Geostream {
  namespace Lib {
    const int MAX_CONNECTIONS = 64 ;
    const int MAX_RECV_MSG_LEN = 2048 ;

    class Socket {
    public:
      Socket () ;
      virtual ~Socket () ;

      // server initialization
      void create () throw (SocketException) ;
      void bind (const int port) throw (SocketException) ;
      void listen () const throw (SocketException) ;
      int accept () const throw (SocketException) ;

      // wait for the connection from the clients
      void connect (const std::string host, const int port) 
	throw (SocketException) ;

      // data transimission
      void send (const std::string) const throw (SocketException) ;
      int recv (std::string&) const throw (SocketException) ;

      void setNonBlocking (const bool) throw (SocketException) ;

      inline bool isValid() const { return (m_sock != -1) ; }

    protected:
      
      int m_sock ;
      sockaddr_in m_addr ;
    } ;
  }
}

#endif
