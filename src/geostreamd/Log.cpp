#include "Log.h"
#include "lib/Exception.h"

using namespace Geostream ;

static const char rcsid[] =
  "$Id: Log.cpp,v 1.1 2005/04/06 23:43:20 jiezhang Exp $" ;

Log::Log (const char *prgname, int option, int facility,
	  const char *defaultAccessLogFile, const char* defaultLogFile) {

  m_prgname = (char*)malloc (strlen (prgname) + 1) ;
  strcpy (m_prgname, prgname) ;

  m_option = option ;
  m_facility = facility ;

  m_defaultAccessLogFile = (char*)malloc (strlen(defaultAccessLogFile)+1) ;
  strcpy (m_defaultAccessLogFile, defaultAccessLogFile) ;

  m_defaultLogFile = (char*)malloc (strlen(defaultLogFile) + 1) ;
  strcpy (m_defaultLogFile, defaultLogFile) ;
}

Log::Log (const char *defaultAccessLogFile, const char* defaultLogFile) {
  m_prgname = NULL ;

  m_defaultAccessLogFile = (char*)malloc (strlen(defaultAccessLogFile)+1) ;
  strcpy (m_defaultAccessLogFile, defaultAccessLogFile) ;

  m_defaultLogFile = (char*)malloc (strlen(defaultLogFile) + 1) ;
  strcpy (m_defaultLogFile, defaultLogFile) ;
}

Log::~Log () {

#ifndef NO_SYSLOG
  closelog ();
#else
  if (m_accessFile != NULL) {
    fclose (m_accessFile) ;
  }

  if (m_logFile != NULL) {
    fclose (m_logFile);
  }
#endif

  if (m_prgname != NULL) {
    delete[] m_prgname ;
  }
  delete[] m_defaultAccessLogFile ;
  delete[] m_defaultLogFile ;
}

void Log::init () throw (Geostream::Lib::IllegalStateException) {
#ifndef NO_SYSLOG
  openlog (m_prgname, m_option, m_facility);
#else
  m_accessFile = fopen (m_defaultAccessLogFile, "a");
  m_logFile = fopen (m_defaultLogFile, "a");

  if (m_accessFile == NULL || m_logFile == NULL) {
    std::string errMsg = "Can not create log files." ;
    throw IllegalStateException (errMsg) ;
  }
  setlinebuf (m_accessFile);
  setlinebuf (m_logFile);
#endif
}

void Log::add (int priority, const char* format, ...) {
  va_list ap;

  va_start (ap, format);
#ifndef NO_SYSLOG
  vsyslog (priority, format, ap);
#else
  FILE *f;
  if (priority == LOG_NOTICE) {
    f = m_accessFile ;
  } else {
    f = m_logFile ;
  }

  vfprintf (f, format, ap);
  fprintf (f, "\n");
#endif
  va_end (ap);
}

