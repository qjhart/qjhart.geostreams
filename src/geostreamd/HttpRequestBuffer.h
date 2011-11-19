#ifndef GEOSTREAM_HTTPREQUESTBUFFER_H
#define GEOSTREAM_HTTPREQUESTBUFFER_H

/**
 * Define a buffer structure for HTTP requests.
 *
 * @author jiezhang - created on Mar 23, 2005
 */
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

namespace Geostream {

  class HttpRequestBuffer {
  public:
    HttpRequestBuffer (ssize_t size) ;
    ~HttpRequestBuffer () ;

    void reset () ;
    char* readp () ;
    bool empty () ;
    void rewind () ;
    int getc () ;
    ssize_t readFrom (int fd) ;
    ssize_t writeTo (int fd) ;
    int strcat (char* str) ;
    int printf (char* format, ...) ;


  private:
    char m_rewindable;
    char *m_scratch;
    ssize_t m_scratch_size;
    char *m_s;
    char *m_r;
    char *m_w;
    char *m_e;

    inline int sizeForRead () {
      return ((m_w - m_r >= 0) ? (m_w - m_r) : (m_e - m_r)) ;
    }

    inline int sizeForWrite () {
      return ((m_w - m_r >= 0) ? (m_e - m_w) : (m_r - m_w - 1)) ;
    }

    inline int size () {
      return (m_e - m_s) ;
    }

    inline int left () {
      return ((m_w - m_r >= 0) ? ((m_e - m_s) - (m_w - m_r)) : 
	      (m_r - m_w - 1)) ;
    }

  } ;

}

#endif
