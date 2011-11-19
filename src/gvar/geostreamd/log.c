/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: log.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: log.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include <stdio.h>
#include <syslog.h>
#include "geostreamd.h"

/* LOGGING FACILITIES */
void
log_open (const char *prgname, int option, int facility)
{
#ifndef NO_SYSLOG
  openlog (prgname, option, facility);
#else
  servfile = fopen ("geostreamd_access.log", "a");
  logfile = fopen ("geostreamd.log", "a");
  setlinebuf (servfile);
  setlinebuf (logfile);
#endif
}

#if 0
void
debug (const char *format, ...)
{
  va_list ap;
  if (Debug)
    {
      va_start (ap, format);
#ifndef NO_SYSLOG
      vsyslog (LOG_DEBUG, format, ap);
#else
      vfprintf (logfile, format, ap);
      fprintf (logfile, "\n");
#endif
      va_end (ap);
    }
}
#endif

void
die (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);
#ifndef NO_SYSLOG
  vsyslog (LOG_ERR, format, ap);
#else
  vfprintf (logfile, format, ap);
  fprintf (logfile, "\n");
#endif
  va_end (ap);
  exit (1);
}

/* Add a single log */
void
log_add (int priority, const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
#ifndef NO_SYSLOG
  vsyslog (priority, format, ap);
#else
  FILE *f;
  if (priority == LOG_NOTICE)
    f = servfile;
  else
    f = logfile;
  vfprintf (f, format, ap);
  fprintf (f, "\n");
#endif
  va_end (ap);
}

void
log_close (void)
{
#ifndef NO_SYSLOG
  closelog ();
#else
  fclose (servfile);
  fclose (logfile);
#endif
}
