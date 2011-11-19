#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>

using namespace Geostream::Lib ;

Socket::Socket () : m_sock (-1) {
  memset (&m_addr, 0, sizeof (m_addr));
}

Socket::~Socket () {
  if (isValid()) {
    ::close (m_sock) ;
  }
}

void Socket::create () throw (SocketException) {
  m_sock = 
    socket (AF_INET, SOCK_STREAM, 0 );

  if (!isValid()) {
    std::string errMsg = "Can not create a socket." ;
    throw SocketException (errMsg) ;
  }


  int onoff = 1;
  if (setsockopt (m_sock, SOL_SOCKET, SO_REUSEADDR, 
		  &onoff, sizeof (onoff)) == -1 ) {
    std::string errMsg =
      "Can not set the option associated with the socket." ;
    throw SocketException (errMsg) ;
  }
}

void Socket::bind (const int port)
  throw (SocketException) {
  if (!isValid ()) {
    std::string errMsg = "bind (): not a valid socket." ;
    throw (SocketException) (errMsg) ;
  }

  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons (port) ;

  if (::bind (m_sock, (struct sockaddr*)&m_addr, sizeof (m_addr)) == -1) {
    std::string errMsg = "bind (): can not bind the socket." ;
    throw SocketException (errMsg) ;
  }
}


void Socket::listen () const
  throw (SocketException) {
  if (!isValid ()) {
    std::string errMsg = "listen (): not a valid socket." ;
    throw SocketException (errMsg) ;
  }

  if (::listen (m_sock, MAX_CONNECTIONS) == -1) {
    std::string errMsg = "listen (): can not listen to the connection." ;
    throw SocketException (errMsg) ;
  }
}


int Socket::accept () const
  throw (SocketException) {
  if (!isValid ()) {
    std::string errMsg = "accept (): not a valid socket." ;
    throw SocketException (errMsg) ;
  }

  int addr_length = sizeof (m_addr) ;
  int sock ;
  if ((sock = ::accept (m_sock, (sockaddr*)&m_addr, 
			(socklen_t*) &addr_length)) <= 0) {
    std::string errMsg = "accept (): can not accept a connection." ;
    throw SocketException (errMsg) ;
  }

  return sock ;
}


void Socket::send (const std::string s) const
  throw (SocketException) {
  if (!isValid ()) {
    std::string errMsg = "send (): not a valid socket." ;
    throw SocketException (errMsg) ;
  }

  if (::send (m_sock, s.c_str (), s.size (), MSG_NOSIGNAL) == -1) {
    std::string errMsg = "send (): can not send a message." ;
    throw SocketException (errMsg) ;
  }
}


int Socket::recv (std::string& s) const
  throw (SocketException) {
  if (!isValid ()) {
    std::string errMsg = "recv (): not a valid socket." ;
    throw SocketException (errMsg) ;
  }

  char buf [MAX_RECV_MSG_LEN + 1];

  s = "";

  memset ( buf, 0, MAX_RECV_MSG_LEN + 1 );

  int status = ::recv (m_sock, buf, MAX_RECV_MSG_LEN, 0) ;

  if (status == -1) {
    std::string errMsg = "recv (): can not receive a message." ;
    throw SocketException (errMsg) ;
  }

  else {
    s = buf;
  }

  return status;
}



void Socket::connect (const std::string host, const int port)
throw (SocketException) {
  if (!isValid()) {
    std::string errMsg = "connect (): not a valid socket." ;
    throw SocketException (errMsg) ;
  }

  m_addr.sin_family = AF_INET ;
  m_addr.sin_port = htons (port) ;

  int status = inet_pton (AF_INET, host.c_str (), &m_addr.sin_addr) ;

  if (errno == EAFNOSUPPORT) {
    std::string errMsg = "connect (): not support." ;
    throw SocketException (errMsg) ;
  }

  if (::connect (m_sock, (sockaddr*)&m_addr, sizeof (m_addr)) != 0) {
    std::string errMsg = "connect (): can not create a connection." ;
    throw SocketException (errMsg) ;
  }
}

void Socket::setNonBlocking (const bool b)
throw (SocketException) {
  if (!isValid()) {
    std::string errMsg = "setNonBlocking (): not a valid socket." ;
    throw (SocketException) (errMsg) ;
  }

  int opts;

  opts = fcntl (m_sock, F_GETFL ) ;

  if (opts < 0) {
    std::string errMsg = "setNonBlocking (): can not read the socket flag." ;
    throw SocketException (errMsg) ;
  }

  if (b) {
    opts = (opts | O_NONBLOCK) ;
  } else {
    opts = (opts & ~O_NONBLOCK) ;
  }

  if (fcntl (m_sock, F_SETFL, opts) == -1) {
    std::string errMsg = "setNonBlocking (): can not set the socket flag." ;
    throw SocketException (errMsg) ;
  }
}
