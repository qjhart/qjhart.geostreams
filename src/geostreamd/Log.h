#ifndef GEOSTREAM_LOG_H
#define GEOSTREAM_LOG_H

/**
 * Define a structure for logging.
 *
 * @author jiezhang - created on Mar 23, 2005
 */

#include <stdio.h>
#include <syslog.h>             /* for LOG_DEBUG, etc */
#include <stdarg.h>

#include "lib/Exception.h"

namespace Geostream {

  extern char Debug ;

  class Geostream::Lib::IllegalStateException ;

  class Log {
  public:
    Log (const char *prgname, int option, int facility,
	 const char *defaultAccessLogFile, const char* defaultLogFile) ;
    Log (const char *defaultAccessLogFile, const char* defaultLogFile) ;
    ~Log () ;

    void init () throw (Geostream::Lib::IllegalStateException) ;
    void add (int priority, const char* format, ...) ;

  private:
    char* m_prgname ;
    int m_option ;
    int m_facility ;

    char* m_defaultAccessLogFile ;
    char* m_defaultLogFile ;
    FILE* m_accessFile ;
    FILE* m_logFile ;
  } ;
}

#endif
