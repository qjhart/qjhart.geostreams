#ifndef GEOSTREAM_HTTPREQUEST_H
#define GEOSTREAM_HTTPREQUEST_H

/**
 * Define a simple HTTP request.
 *
 * @author jiezhang - created on Mar 23, 2005
 */

#include <unistd.h>
#include <uri.h>

#include "HttpRequestBuffer.h"
#include "lib/Exception.h"

namespace Geostream {

  enum RequestState {
    START,
    METHOD,
    URI,
    VERSION,
    HEADER,
    HEADER_R,
    HEADER_RN,
    LAST_R,
    ERR,
    REQ
  } ;
    
  enum RequestMethod {
    GET,
    POST,
    HEAD,
    /* NOT */
    OPTIONS,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
    /* DEFAULT */
    NOT_SET,
  };

  const char* Response[600];
  Repsonse[100]= "Continue";
  Response[101]= "Switching Protocols";
  Response[200]="OK";

  class HttpRequestBuffer ;
  class Connection ;

  class HttpRequest {
  public:


	// Shouldn't these be MACROS ? 
    static const int HEADER_LENGTH = 80 ;

    static const int RES_WORKING = 0 ; /* Internal Code */
    static const int RES_CLOSED = 1 ; /* Internal Code Client is gone */
    static const int RES_GOODREQUEST = 2 ; /* Internal Code Request OK */

    static const int RES_Continue = 100 ;
    static const int RES_OK = 200 ;
    static const int RES_BadRequest = 400 ; /* Section 10.4.1: */
    static const int RES_MethodNotAllowed = 405  ; /* Section 10.4.6: */
    static const int RES_RequestTimeout = 408 ; /* Section 10.4.9: */
    static const int RES_TooLarge = 413 ;
    static const int RES_ServerError = 500 ; /* Section 10.5.1: */

    HttpRequest () ;
    ~HttpRequest () ;

    void newRequest (Connection *conn) 
      throw (Geostream::Lib::IllegalStateException) ;
    void close () ;
    RequestState scan (HttpRequestBuffer *buf) ;
    int parse (char* input) ;

    /**
     * Access methods.
     */
    inline char* getStatusLine () {
      return m_statusLine ;
    }

    inline RequestMethod getRequestMethod () {
      return m_method ;
    }

    inline uri_t* getURI () {
      return m_uri ;
    }

    inline char getKeepAlive () {
      return m_keepAlive ;
    }

  private:
    /* Local Info */
    char *m_filename ;
    Connection *m_conn ;
    RequestState m_state ;

    /* Request Parsing */
    RequestMethod m_method ;
    uri_t *m_uri;
    char *m_version;
    int m_versionMajor;
    int m_versionMinor;

    char *m_statusLine;
    int m_statusLineZ;

    char m_keepAlive;              /* 1 => KeepAlive */
    /* Everything breaks with multi-line RFC values */
    char m_from[HEADER_LENGTH];
    char m_host[HEADER_LENGTH];
    char m_userAgent[HEADER_LENGTH];

#if 0
    /* General Ones */
    char *CacheControl;
    char *Date;
    char *Pragma;
    char *Trailer;
    char *TransferEncoding;
    char *Upgrade;
    char *Via;
    char *Warning;
    /* Requests */
    char *Accept;
    char *AcceptCharset;
    char *AcceptEncoding;
    char *AcceptLanguage;
    char *Authorization;
    char *Expect;
    char *IfMatch;
    char *IfModifiedSince;
    char *IfNoneMatch;
    char *IfRange;
    char *IfUnmodifiedSince;
    char *MaxForwards;
    char *ProxyAuthorization;
    char *Range;
    char *Referer;
    char *TE;
#endif

    char* nextHeader (char* str, size_t* lp) ;

    friend class Connection ;

  } ;

}

#endif
