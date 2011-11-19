#include "HttpRequest.h"
#include "Connection.h"

using namespace Geostream ;

static const char rcsid[] =
  "$Id: HttpRequest.cpp,v 1.1 2005/04/06 23:43:20 jiezhang Exp $" ;

HttpRequest::HttpRequest () {
  m_statusLineZ = 0 ;
  m_statusLine = NULL ;
}

HttpRequest::~HttpRequest () {
  if (m_statusLine != NULL) {
    delete[] m_statusLine ;
  }
}

void HttpRequest::newRequest (Connection *conn) 
  throw (Geostream::Lib::IllegalStateException) {
  m_conn = conn ;
  m_method = NOT_SET ;
  m_state = START ;

  m_uri = uri_alloc_1 () ;
  if (m_uri == NULL) {
    std::string errMsg = "Can not allocate a URI." ;
    throw Geostream::Lib::IllegalStateException (errMsg) ;
  }
}

void HttpRequest::close () {
  m_keepAlive = 0 ;
  m_state = START ;
  m_method = NOT_SET ;
}

RequestState HttpRequest::scan (HttpRequestBuffer *buf) {
  RequestState state = m_state ;

  char aChar ;
  while (((aChar = buf->getc ()) != EOF) &&
	 (state != ERR) &&
	 (state != REQ)) {
    switch (state) {
    case START:
      switch (aChar) {
      case '\r':
      case '\n':
	state = ERR;
	break;
      case ' ':
      case '\t':
	break;
      default:
	state = METHOD;
	break;
      }
      break ;

    case METHOD:
      switch (aChar) {
      case ' ':
      case '\t':
	state = URI;
	break;
      case '\r':
      case '\n':
	state = ERR;
	break;
      default:
	break ;
      }
      break ;

    case URI:
      switch (aChar) {
      case ' ':
      case '\t':
	state = VERSION;
	break;
      case '\r':
	state = LAST_R;
	break;
      case '\n':
	state = REQ;
	break;
      default:
	break ;
      }
      break ;

    case VERSION:
      switch(aChar) {
      case '\n':
	state = HEADER_RN;
	break;
      case '\r':
	state = HEADER_R;
	break;
      default:
	break ;
      }
      break ;

    case HEADER:
      switch (aChar) {
      case '\r':
	state = HEADER_R;
	break;
      case '\n':
	state = HEADER_RN;
	break;
      default:
	break ;
      }
      break ;

    case HEADER_R:
      switch (aChar) {
      case '\n':
	state = HEADER_RN;
	break;
      default:
	state = ERR;
	break;

      }
      break ;

    case HEADER_RN:
      switch (aChar) {
      case '\r':
	state = LAST_R;
	break;
      case '\n':
	state = REQ;
	break;
      default:
	state = HEADER;
	break;
      }
      break ;

    case LAST_R:
      switch (aChar) {
      case '\n':
	state = REQ;
	break;
      default:
	state = ERR;
	break ;
      }
      break ;

    case (ERR):
    case (REQ):
      break;
    }
  }

  m_state = state ;
  return m_state ;
}

int HttpRequest::parse (char* input) {

  debug ("%s\n", input) ;

  /* Parse Request-Line */
  char* next = strchr (input, '\n');

  if (next == 0) {
    return RES_BadRequest ;
  }

  if (*(--next) == '\r') {
    *next = 0 ;
  }
  *(next++) = 0 ;
  next++ ;

  int len = strlen (input) ;
  if (m_statusLineZ < len - 1) {
    // debug("status line realloc %d", m_statusLineZ);
    m_statusLine = (char*)realloc (m_statusLine, len+80) ;
    if (m_statusLine == NULL) {
      return -1 ;
    }
    m_statusLineZ = len + 80 ;
  }

  strncpy (m_statusLine, input, len+1) ;

  char* s = strchr (input, ' ') ;
  if (s == 0) {
    return RES_BadRequest ;
  }

  *(s++) = 0 ;
  if (strcmp (input, "HEAD") == 0) {
    m_method = HEAD ;
  } else if (strcmp (input, "GET") == 0) {
    m_method = GET ;
  } else if (strcmp (input, "POST") == 0) {
    m_method = POST ;
  } else if (strcmp (input, "OPTIONS") == 0 ||
	     strcmp (input, "PUT") == 0 ||
	     strcmp (input, "DELETE") == 0 ||
	     strcmp (input, "TRACE") == 0 || strcmp (input, "CONNECT") == 0) {
    return RES_MethodNotAllowed;
  } else {
    return 501;
  }

  input = s ;

  s = strchr (input, ' ') ;
  if (s == 0) {
    if (uri_realloc (m_uri, input, strlen (input)) == URI_NOT_CANNONICAL) {
      return RES_BadRequest ;
    }
    m_version = NULL ;
    m_versionMajor = 0 ;
    m_versionMinor = 0 ;
  }

  else {
    *(s++) = 0 ;
    if (uri_realloc (m_uri, input, strlen (input)) == URI_NOT_CANNONICAL) {
      return RES_BadRequest ;
    }

    m_version = s ;

    if ((strlen (s) != 8) || (strncmp (s, "HTTP/", 5) != 0)) {
      return RES_BadRequest;
    }

    m_versionMajor = *(s + 5) - 0x30 ;
    m_versionMinor = *(s + 7) - 0x30 ;
    if (m_versionMajor == 1 && m_versionMinor == 1) {
      m_keepAlive = 1;
    }
  }

  if (m_version) {
    // the header
    while (input = next, next = strchr (input, '\n')) {
      if (*(--next) == '\r') {
	*next = 0 ;
      }
      *(next++) = 0 ;
      next ++ ;
      
      if (next == input + 1 || next == input + 2)
	break;

      char* name = input ;
      s = strchr (name, ':') ;
      if (s == 0) {
	// debug ("Ill-Formed Request Header %s", name);
	return RES_BadRequest;
      }
      *(s++) = 0 ;
      while (*s == ' ') {
	s++ ;
      }

      char *value = s ;
      len = strlen (name) ;

      switch (len) {
#if 0
      case (2):
	if (strcmp (name, "TE") == 0)
	  1;
	break;
#endif
      case (4):
	if (strcmp (name, "From") == 0)
	  strncpy (m_from, value, HEADER_LENGTH);
	else if (strcmp (name, "Host") == 0)
	  strncpy (m_host, value, HEADER_LENGTH);
	break;

#if 0
      case (5):
	if (strcmp (name, "Range") == 0)
	  1;
	break;
      case (6):
	if (strcmp (name, "Accept") == 0)
	  1;
	else if (strcmp (name, "Expect") == 0)
	  1;
	break;
      case (7):
	if (strcmp (name, "Referer") == 0)
	  1;
	break;
      case (8):
	if (strcmp (name, "If-Match") == 0)
	  1;
	else if (strcmp (name, "If-Range") == 0)
	  1;
	break;
#endif
      case (10):
	if (strcmp (name, "User-Agent") == 0) {
	  strncpy (m_userAgent, value, HEADER_LENGTH);
	}

	if (strcmp (name, "Connection") == 0) {
	  while (1) {
	    size_t token_z ;

	    value = nextHeader (value, &token_z);
	    if (token_z == 0) break;
	    if (token_z == 10 &&
		strncasecmp (value, "keep-alive", token_z) == 0 &&
		m_versionMajor == 1 && m_versionMinor == 1) {
	      m_keepAlive = 1;
	    } else if (token_z == 5 &&
		       strncasecmp (value, "close", token_z) == 0) {
	      m_keepAlive = 0;
	    }
	    value += token_z;
	  }
	}
	break;
#if 0
      case (12):
	if (strcmp (name, "Max-Forwards") == 0)
	  1;
	break;
      case (13):
	if (strcmp (name, "Authorization") == 0)
	  1;
	else if (strcmp (name, "If-None-Match") == 0)
	  1;
	break;
      case (14):
	if (strcmp (name, "Accept-Charset") == 0)
	  1;
	break;
      case (15):
	if (strcmp (name, "Accept-Encoding") == 0)
	  1;
	else if (strcmp (name, "Accept-Language") == 0)
	  1;
	break;
      case (17):
	if (strcmp (name, "If-Modified-Since") == 0)
	  1;
	break;
      case (19):
	if (strcmp (name, "If-Unmodified-Since") == 0)
	  1;
	else if (strcmp (name, "Proxy-Authorization") == 0)
	  1;
	break;
#endif
      default:
	break;
      }
    }
  }
  return RES_GOODREQUEST;       /* OKAY */

}

char* HttpRequest::nextHeader (char* str, size_t* lp) {

  while (*str == ',' || *str == ' ') {
    ++str ;
  }

  size_t pos = 0 ;
  char lastChar = 0 ;
  int inquotedString = 0 ;
  char aChar ;
  while ((aChar = str[pos]) != 0) {
    if (aChar == ',' && inquotedString == 0) {
      break ;
    }

    if (aChar == '"' &&
	(inquotedString == 0 && lastChar != '\\')) {
      inquotedString = (inquotedString == 0) ;
    }
    lastChar = aChar ;
    ++pos ;
  }

  while (pos > 0 && str[pos-1] == ' ') {
    --pos ;
  }
  *lp = pos ;
  return str ;
}

