#include <event.h>
#include "CallBack.h"
#include "HttpRequestBuffer.h"

using namespace Geostream::HttpRequestBuffer ;

static const char rcsid[] =
  "$Id: HttpRequestBuffer.cpp,v 1.2 2005/06/03 18:59:16 qjhart Exp $" ;

HttpRequestBuffer (ssize_t size) {
  char *s = (char*)malloc (size) ;  
  if (s!= NULL) {
    m_s = s ;
    m_r = s ;
    m_w = s ;
    m_e = s + size ;
    m_scratch = NULL ;
  }
}

~HttpRequestBuffer () {
  if (m_s != NULL) {
    delete[] m_s ;
  }
}

void reset () {
  m_rewindable = 1 ;
  m_r = m_s ;
  m_w = m_s ;
}

char* readp () {
  return m_r ;
}

bool empty () {
  return (m_r == m_w) ;
}

void rewind () throw (Geostream::Lib::Exception) {
  if (m_rewindable) {
    m_r = m_s ;
  }
  throw Geostream::Lib::Exception("Buffer Not Rewindable");
}

int getc () {
  char c = EOF ;

  if (sizeForRead () > 0) {
    c = *(m_r ++) ;
  }

  if (m_r > m_e) {
    m_r = m_s ;
  }

  return c ;
}

ssize_t readFrom (int fd) {
  ssize_t nr = read (fd, m_w, sizeForWrite ()) ;

  if (nr != -1) {
    m_w += nr ;
    if (m_w == m_e) {
      m_w = m_s ;
    }
  }

  return nr ;
}


ssize_t writeTo(int fd) {
  ssize_t cnt;
  ssize_t tot;
  while ((cnt = conn->m_buffer->writeToOnce (fd)) != 0) {
	tot += cnt;
  }
  return tot;
}

void writeToCallBack (int sock, short event, void *arg) {
  Callback* cb = (CallBack *)arg ;

  if (event & EV_WRITE) // Write if ready, else wait for it.
	writeTo (sock) ;
  if (empty()) {
	cb->callback(sock,NULL,cb->arg); // Callback
  } else {
	event_set (cb->event, sock, EV_WRITE,
			   sendBufferEventThen, cb) ;
	event_add (cb->event, NULL) ;
  }
}

ssize_t writeToOnce (int fd) throw Geostream::Lib::Exception {
  // If buffer is full
  if (sizeForRead () == 0) {
    return 0 ;
  }
  ssize_t nw = write (fd, m_r, sizeForRead ()) ;
  if (nw != -1) {
    m_r += nw ;
    if (m_r == m_e) {
      m_r = m_s ;
    }
	return nw ;
  } else
    throw Geostream::Lib::ErrnoException() ;
}

int strcat (char* str) {
  int len = strlen (str) ;

  int n ;
  for (n=0; n<len; n++) {
    if (left () == 0) {
      return -1 ;
    }

    *(m_w ++) = *(str+n) ;
    if (m_w == m_e) {
      m_w = m_e ;
    }
  }

  return n ;
}

// int strcat_n (...) {
//   va_list ap ;
//   va_start (ap) ;

//   char* str ;
//   int n = 0 ;
//   while ((str = va_arg (ap, char*)) != NULL) {
//     int len = strlen (str) ;
//     for (n=0; n<len; n++) {
//       if (left () == 0) {
// 	return -1 ;
//       }

//       *(m_w ++) = *(str+n) ;

//       if (m_w == m_e) {
// 	m_w = m_s ;
//       }
//     }
//   }

//   return n ;
// }

int printf (char* format, ...) {
  int size = sizeForWrite () ;

  va_list ap ;
  va_start (ap, format) ;
  int written = vsnprintf (m_w, size, format, ap) ;
  if (written < 0) {
    return written ;
  }

  else if (written > left ()) {
    if (m_scratch != NULL && m_scratch_size < written) {
      delete[] m_scratch ;
      m_scratch = NULL ;
    }

    if (m_scratch == NULL) {
      m_scratch = (char*) malloc (written) ;
      if (m_scratch == NULL) {
	return -1 ;
      }
    }

    written = vsnprintf (m_scratch, written, format, ap) ;
    if (written < 0) {
      return -1 ;
    }

    memcpy (m_s, m_scratch + size, (written - size)) ;
    m_w = m_s + (written - size) ;
  }

  else {
    m_w += written ;
  }

  return written ;
}
