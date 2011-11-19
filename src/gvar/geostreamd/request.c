/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: request.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: request.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "geostreamd.h"
#include <unistd.h>


#define HEADER_LENGTH 80

struct request
{
  /* Local Info */
  char *filename;
  struct client *client;
  enum request_state state;

  /* Request Parsing */
  enum request_method method;
  uri_t *uri;
  char *version;
  int version_major;
  int version_minor;

  char *status_line;
  int status_line_z;

  char keep_alive;              /* 1 => KeepAlive */
  /* Everything breaks with multi-line RFC values */
  char From[HEADER_LENGTH];
  char Host[HEADER_LENGTH];
  char UserAgent[HEADER_LENGTH];

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
};

/* www.mathopd.org - request.c */
static const char *
header_list_next (const char *s, size_t * lp)
{
  int inquotedstring;
  char c, lastc;
  size_t l;

  while (*s == ',' || *s == ' ')
    ++s;
  l = 0;
  lastc = 0;
  inquotedstring = 0;
  while ((c = s[l]) != 0)
    {
      if (c == ',' && inquotedstring == 0)
        break;
      if (c == '"' && (inquotedstring == 0 || lastc != '\\'))
        inquotedstring = inquotedstring == 0;
      lastc = c;
      ++l;
    }
  while (l > 0 && s[l - 1] == ' ')
    --l;
  *lp = l;
  return s;
}

/* Common Accessors */
inline char *
request_status_line (struct request *r)
{
  return r->status_line;
}

inline enum request_method
request_method (struct request *r)
{
  return r->method;
}

inline uri_t *
request_uri (struct request * r)
{
  return r->uri;
}

inline char
request_keep_alive (struct request *r)
{
  return r->keep_alive;
}

/*  */
struct request *
request_new (struct client *c)
{
  struct request *r;
  if ((r = malloc (sizeof *r)) == NULL)
    {
      log_add (LOG_ERR, "Malloc Error Request");
      return NULL;
    }
  memset (r, 0, sizeof *r);
  /* Set Header Buffers */

  r->client = c;
  r->method = NOT_SET;
  r->state = START;

  if ((r->uri = uri_alloc_1 ()) == NULL)
    {
      request_free (r);
      log_add (LOG_ERR, "Malloc Error Request");
    }
  return r;
}

void
request_close (struct request *r)
{
  r->keep_alive=0;
  r->state = START;
  r->method = NOT_SET;
}

void
request_free (struct request *r)
{
  if (r->status_line)
    free(r->status_line);
  free (r);
}

enum request_state
request_scan (struct request *r, struct buffer *b)
{
  enum request_state state;
  char ch;
  state = r->state;

  while (((ch = buffer_getc (b)) != EOF) && state != ERR && state != REQ)
    {
      switch (state)
        {
        case START:
          switch (ch)
            {
            default:
              state = METHOD;
              break;
            case '\r':
            case '\n':
              state = ERR;
              break;
            case ' ':
            case '\t':
              break;
            }
          break;
        case METHOD:
          switch (ch)
            {
            default:
              break;
            case ' ':
            case '\t':
              state = URI;
              break;
            case '\r':
            case '\n':
              state = ERR;
              break;
            }
          break;
        case URI:
          switch (ch)
            {
            default:
              break;
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
            }
          break;
        case VERSION:
          switch (ch)
            {
            default:
              break;
            case '\n':
              state = HEADER_RN;
              break;
            case '\r':
              state = HEADER_R;
              break;
            }
          break;
        case HEADER:
          switch (ch)
            {
            default:
              break;
            case '\r':
              state = HEADER_R;
              break;
            case '\n':
              state = HEADER_RN;
              break;
            }
          break;
        case HEADER_R:
          switch (ch)
            {
            default:
              state = ERR;
              break;
            case '\n':
              state = HEADER_RN;
              break;
            }
          break;
        case HEADER_RN:
          switch (ch)
            {
            default:
              state = HEADER;
              break;
            case '\r':
              state = LAST_R;
              break;
            case '\n':
              state = REQ;
              break;
            }
          break;
        case LAST_R:
          switch (ch)
            {
            default:
              state = ERR;
            case '\n':
              state = REQ;
              break;
            }
          break;
        case (ERR):
        case (REQ):
          break;
        }
    }
  r->state = state;
  return state;
}

int
request_parse (struct request *r, char *input)
{
  int len;
  char *name;
  const char *value;
  char *next;
  char *s;

  size_t token_z;

  /* input is sort of not unsafe cause we scanned it, 
     and it's at start of buffer */
  /* Parse Request-Line */
  next = strchr (input, '\n');
  if (next == 0)
    return RES_BadRequest;
  if (*--next == '\r')
    *next = 0;                  /* Divide */
  *next++ = 0;
  next++;

  len = strlen (input);
  if (r->status_line_z < len-1 )
    {
      debug("status line realloc %d",r->status_line_z);
      if ((r->status_line = realloc (r->status_line, len+80)) == NULL)
        return -1;
      r->status_line_z = len+80;
    }
  strncpy (r->status_line, input, len + 1);

  s = strchr (input, ' ');
  if (s == 0)
    return RES_BadRequest;
  *s++ = 0;
  if (strcmp (input, "HEAD") == 0)
    r->method = HEAD;
  else if (strcmp (input, "GET") == 0)
    r->method = GET;
  else if (strcmp (input, "POST") == 0)
    r->method = POST;
  else if (strcmp (input, "OPTIONS") == 0 ||
           strcmp (input, "PUT") == 0 ||
           strcmp (input, "DELETE") == 0 ||
           strcmp (input, "TRACE") == 0 || strcmp (input, "CONNECT") == 0)
    {
      return RES_MethodNotAllowed;
    }
  else
    {
      return 501;
    }
  input = s;
  s = strchr (input, ' ');
  if (s == 0)
    {
      /* This is a simple Request, so handle it */
      if (uri_realloc (r->uri, input, strlen (input)) == URI_NOT_CANNONICAL)
        return RES_BadRequest;
      r->version = NULL;
      r->version_major = 0;
      r->version_minor = 0;
    }
  else
    {
      *s++ = 0;
      if (uri_realloc (r->uri, input, strlen (input)) == URI_NOT_CANNONICAL)
        return RES_BadRequest;
      r->version = s;
      if ((strlen (s) != 8) || (strncmp (s, "HTTP/", 5) != 0))
        {
          return RES_BadRequest;
        }
      r->version_major = *(s + 5) - 0x30;
      r->version_minor = *(s + 7) - 0x30;
      if (r->version_major == 1 && r->version_minor == 1)
        {
          r->keep_alive = 1;
        }
    }

  if (r->version)
    {
      /* Now the Headers */
      while (input = next, next = strchr (input, '\n'))
        {
          if (*--next == '\r')
            *next = 0;          /* Divide */
          *next++ = 0;
          next++;

          if (next == input + 1 || next == input + 2)
            break;

          name = input;
          s = strchr (name, ':');
          if (s == 0)
            {
              debug ("Ill-Formed Request Header %s", name);
              return RES_BadRequest;
            }
          *s++ = 0;
          while (*s == ' ')
            s++;
          value = s;
          len = strlen (name);

          /* debug ("Request %s: %s", name, value); */

          /* Silently Ignore Headers not understood */
          /* I have to do a much better job of decoding the request headers.
             Right now, I'm ignoring them.
           */

          switch (len)
            {
#if 0
            case (2):
              if (strcmp (name, "TE") == 0)
                1;
              break;
#endif
            case (4):
              if (strcmp (name, "From") == 0)
                strncpy (r->From, value, HEADER_LENGTH);
              else if (strcmp (name, "Host") == 0)
                strncpy (r->Host, value, HEADER_LENGTH);
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
              if (strcmp (name, "User-Agent") == 0)
                strncpy (r->UserAgent, value, HEADER_LENGTH);
              if (strcmp (name, "Connection") == 0)
                {
                  while (1)
                    {
                      value=header_list_next (value, &token_z);
                      if (token_z == 0)
                        break;
                      if (token_z == 10 &&
                          strncasecmp (value, "keep-alive", token_z) == 0 &&
                          r->version_major == 1 && r->version_minor == 1)
                        r->keep_alive = 1;
                      else if (token_z == 5
                               && strncasecmp (value, "close", token_z) == 0)
                        r->keep_alive = 0;
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
