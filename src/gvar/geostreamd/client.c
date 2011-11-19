/*
  client - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: client.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: client.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "geostreamd.h"
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fnmatch.h>

void client_prepare_headers (struct client * c);
int client_sendfile(struct client *c);

void event_client_send_response (int fd, short event, void *arg);
void event_client_sendfile (int fd, short event, void *c);
void event_client_request (int fd, short event, void *arg);

void pthread_client_request(struct client *c);
void pthread_client_sendfile(struct client *c);


/* 1xx: Informational - Request received, continuing process
   
   2xx: Success - The action was successfully received,
   understood, and accepted

   3xx: Redirection - Further action must be taken in order to
   complete the request

   4xx: Client Error - The request contains bad syntax or cannot
   be fulfilled

   5xx: Server Error - The server failed to fulfill an apparently
   valid request
*/
static char *RES_Info[] = {
  "Continue",                   /*100 */
  "Switching Protocols"         /*101 */
};

static char *RES_Success[] = {
  "OK",                         /* 200 */
  "Created",
  "Accepted",
  "Non Authoritative Information",      /* 203 */
  "No Content",                 /* 204 */
  "Reset Content",              /* 205 */
  "Partial Content"             /* 206 */
};

static char *RES_Redirection[] = {
  "Multiple Choices",           /* 300 */
  "Moved Permanently",          /* 301 */
  "Found",                      /* 302 */
  "See Other",                  /* 303 */
  "Not Modified",               /* 304 */
  "Use Proxy",                  /* 305 */
  "Temporary Redirect"          /* 307 */
};

static char *RES_Client_Error[] = {
  "Bad Request",                /* 400 */
  "Unauthorized",               /* 401 */
  "Payment Required",           /* 402 */
  "Forbidden",                  /* 403 */
  "Not Found",                  /* 404 */
  "Method Not Allowed",         /* 405 */
  "Not Acceptable",             /* 406 */
  "Proxy Authentication Required",      /* 407 */
  "Request Timeout",            /* 408 */
  "Conflict",                   /* 409 */
  "Gone",                       /* 410 */
  "Length Required",            /* 411 */
  "Precondition Failed",        /* 412 */
  "Request Entity Too Large",   /* 413 */
  "Request URI Too Large",      /* 414 */
  "Unsupported Media Type",     /* 415 */
  "Requested range not satisfiable",    /* 416 */
  "Expectation Failed"          /* 417 */
};

static char *RES_Server_Error[] = {
  "Internal Server Error",      /* 500 */
  "Not Implemented",            /* 501 */
  "Bad Gateway",                /* 502 */
  "Service Unavailable",        /* 503 */
  "Gateway Timeout",            /* 504 */
  "HTTP Version not supported"  /* 505 */
};

struct client
{
  struct client *prev;
  struct client *next;
  int times_used;
  struct server *server;
  struct sockaddr_in sa_remote;
  struct sockaddr_in sa_local;
  struct request *request;
  response_code response;
  char keep_alive;
  char *fn;
  int fn_z;
  int fd;
  char time_str[TIME_STR_LEN];
  /* buffer */
  struct buffer *buffer;

  /* Local File info */
  struct cfile *cfile;
  off_t content_length;
  off_t of_offset;

  enum multi_type write_style;
  enum multi_type req_style;
  struct event *event_p;
  pthread_t *pthr_p;
  struct host * hosts_can_serve[MAX_HOSTS];
};

/*
  client functions
 */

void
client_free (struct client *c)
{
  if (c->event_p != NULL)
    free (c->event_p);
  if (c->pthr_p != NULL)
     free(c->pthr_p);
  if (c->request != NULL)
    request_free (c->request);
  if (c->buffer != NULL)
    buffer_free (c->buffer);
  free (c);
}

struct client *
client_new (struct server *s,enum multi_type write_style)
{
  struct client *c;

  if ((c = malloc (sizeof *c)) == (void *) 0)
    {
      return NULL;
    }
  memset (c, 0, sizeof *c);

  if ((c->buffer = buffer_malloc (CLIENT_BUFFER_SIZE)) == NULL)
    {
      client_free (c);
      return NULL;
    }

  if ((c->request = request_new (c)) == NULL)
    {
      client_free (c);
      return NULL;
    }
  if ((c->event_p = malloc (sizeof *(c->event_p))) == NULL)
    {
      client_free (c);
      return NULL;
    }
  if ((c->pthr_p = (pthread_t *)malloc(sizeof *c->pthr_p)) == NULL)
    {
      client_free (c);
      return NULL;
    }

  c->server = s;
  c->write_style=write_style;
  return c;
}

void
client_reset_or_free(struct client *c,int keep_alive)
{
  int err=0;
  int rfd_err = 0;
  if (c->cfile !=NULL)
    {
      if (c->cfile->fd !=0)
	{
	  if (close (c->cfile->fd))
	    rfd_err = errno;
	}
      cache_file_done(Cache,c->cfile);
    }
  buffer_reset (c->buffer);
  request_close (c->request);
  c->content_length = 0;
  c->of_offset = 0;
  if (keep_alive)
    {
      if (c->req_style==EVENTS)
	{
	  event_set (c->event_p, c->fd, EV_READ,event_client_request,c);
	  event_add(c->event_p,NULL);
	}
    }
  else
    {
      if (c->fd != 0)
	if(close(c->fd) !=0)
	  err = errno;
      server_free_client (c->server, c);
    }
  debug ("client_reset_or_free fd:%d [%d] cfile->mmap:%p cfile->fd:%d [%d] %s",
	 c->fd,err,(c->cfile)?c->cfile->mmap:0,(c->cfile)?c->cfile->fd:0, rfd_err,
	 (keep_alive)?"keep_alive":"close");
  if (c->cfile) 
    c->cfile->fd = 0;
}

int
client_accept (struct client *c, int listen_fd,enum multi_type req_style)
{
  socklen_t l;
  int fd;

  c->req_style=req_style;
  c->fd=0;
  c->times_used++;
  strncpy (c->time_str, Time_Str, TIME_STR_LEN);

  l = sizeof c->sa_remote;
  fd = accept (listen_fd, (struct sockaddr *) &c->sa_remote, &l);
  if (fd == -1)
    {
      if (errno != EAGAIN)
        log_add (LOG_ERR, "accept %s", strerror (errno));
      return -1;
    }
  fcntl (fd, F_SETFD, FD_CLOEXEC);
  l = sizeof c->sa_local;
  if (getsockname (fd, (struct sockaddr *) &c->sa_local, &l) == -1)
    {
      close (fd);
      return -1;
    }              
  c->fd = fd;
  if (c->req_style==EVENTS) 
    {
      fcntl (fd, F_SETFL, O_NONBLOCK);
      event_set (c->event_p, fd, EV_READ, 
		 event_client_request, c);
      event_add (c->event_p, NULL);
    }
  else 
    {
      pthread_create(c->pthr_p,NULL,(void *(*)(void *))pthread_client_request,(void *)c);
      pthread_detach(*c->pthr_p);
    } 
  return fd;
}

inline int
client_fd (struct client *c)
{
  return c->fd;
}

int
client_cork(struct client *c,int on) 
{
  int onoff=on;
  int rc;
  rc=setsockopt(c->fd, IPPROTO_TCP, TCP_CORK, &onoff, sizeof onoff);
  if (rc == -1)
    log_add (LOG_ERR, "setsockopt %s", strerror (errno));
  return rc;
}

int
client_sendfile(struct client *c)
{
  int count;
  int fd=c->fd;
  if (c->cfile->mmap != NULL)
    {
      count = write (fd,c->cfile->mmap,c->content_length - c->of_offset);
      debug("%d=write(%d,%p,%ld) of %d", count,fd,c->cfile->mmap,(int)c->content_length-c->of_offset,(int)c->content_length);
      if (count != -1) c->of_offset+=count;
    }
  else if (c->cfile->fd > 0)
    {
      count =sendfile (fd,c->cfile->fd, &c->of_offset,
		       c->content_length - c->of_offset);
      debug("%d=sendfile(%d,%d,%ld) of %d", count,fd,c->cfile->fd,
	     (int)c->content_length-c->of_offset,(int)c->content_length);
    }
  return count;
}

response_code
client_request_serv (struct client * c)
{
  enum request_method m;
  response_code rc;
  struct server *s=c->server;
  
  m = request_method (c->request);

  if (m == HEAD || m == GET)
    {
      uri_t *uri;
      char *furi;
      int fn_z;
      uri = request_uri (c->request);
      furi = uri_path (uri);

      /* Is this furi servicable by this client */
      if ((strlen(s->files)) > 0 && 
	  fnmatch(s->files,furi, FNM_PATHNAME) != 0)
	{
	  struct server *ns;
	  struct server *okay[255];
	  int count=0;
	  ns=s->next;
	  while (ns != s)
	    {
	      if ((strlen(ns->files)) == 0 || 
		  fnmatch(ns->files,furi, FNM_PATHNAME) == 0)
		okay[count++]=ns;
	      ns=ns->next;
	    }
	  if (count==0)
	    return 404;		/* Doesn't exist */
	  int j;
	  if (count==1)
	    j=0;
	  else
	    j=(int) count*(rand()/(RAND_MAX+1.0));
	  uri_scheme_set(uri,"http");
	  uri_host_set(uri,okay[j]->hostname);
#if 1
	  char port[10];
	  sprintf(port,"%d",okay[j]->port);
	  uri_port_set(uri,port);
#endif
	  debug("Redirecting to %s:%d",okay[j]->hostname,okay[j]->port);
	  return 302;
	}
      fn_z = strlen (furi) + s->document_root_z + 128;
      if (c->fn_z < fn_z)
        {
          if ((c->fn = realloc (c->fn,fn_z)) == NULL)

            return -1;
          c->fn_z = fn_z;
        }
      sprintf (c->fn, "%s/%s", s->document_root, furi);
      if ((c->cfile = cache_file_get(Cache,c->fn,m,&rc))==NULL)
	return rc;
      c->content_length = c->cfile->finfo.st_size;
      return 200;
    }
  else if (m == POST)
    {
      return 501;
    }

  return 501;
}

void
client_prepare_headers (struct client * c)
{
  char *reason;
  response_code rc=c->response;
  
  /* Can we reuse some buffer? */
  struct buffer *b;
  b = c->buffer;
  buffer_reset (b);

  /* Status Line */
  if (rc >= 100 && rc <= 101)
    reason = RES_Info[rc - 100];
  else if (rc >= 200 && rc <= 206)
    reason = RES_Success[rc - 200];
  else if (rc >= 300 && rc <= 307)
    reason = RES_Redirection[rc - 300];
  else if (rc >= 400 && rc <= 417)
    reason = RES_Client_Error[rc - 400];
  else if (rc >= 500 && rc <= 505)
    reason = RES_Server_Error[rc - 500];
  else
    reason = "UNKNOWN";
  buffer_printf (b, "HTTP/1.1 %d %s\r\n", rc, reason);

  /* Default Headers */
  buffer_printf (b,
                 "Server: geostreamd liburi/2.13\r\nDate: %s\r\nLocation: %s\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: %s\r\n\r\n",
                 c->time_str,
		 uri_uri(request_uri (c->request)),
                 c->content_length,
                 "text/html", (c->keep_alive) ? "Keep-Alive" : "closed");

  log_add (LOG_NOTICE, "%s - - [%s] \"%s\" %d %d",
           inet_ntoa (c->sa_remote.sin_addr), c->time_str,
           request_status_line (c->request), rc, c->content_length);
}

response_code
client_request (struct client *c)
{
  ssize_t count;
  struct buffer *b;
  b = c->buffer;
  enum request_state rs;
  response_code rc;
  count = buffer_read_from_fd (b, c->fd);
  switch (count)
    {
    case -1:
      debug("buffer: %s",strerror(errno));
      return -1;
    case 0:
      return RES_CLOSED;
    case CLIENT_BUFFER_SIZE:
      return 414;
    }
  rs = request_scan (c->request, b);
  if (rs == ERR)
      return RES_BadRequest;
  if (rs != REQ)		/* Keep Reading */
      return RES_WORKING;
  if (buffer_rewind (b) == -1)
       return 414;
  rc = request_parse (c->request, buffer_readp (b));
  if (rc != RES_GOODREQUEST)
    return rc;
#if 1
  if (request_keep_alive (c->request))
    c->keep_alive = 1;
#endif

  return client_request_serv (c);
}

void
event_client_request (int fd, short event, void *arg)
{
  struct client *c=arg;
  c->response=client_request(c);
  
  switch(c->response) {
  case(RES_CLOSED):
    client_reset_or_free(c,0);
    break;
  case(-1):
    log_add(LOG_WARNING,"event_client_request-end fd:%d",fd);
  case(RES_WORKING):
      event_add (c->event_p, NULL); /* Redo */
      break;
  case(414):
  case(RES_BadRequest):
  default:
      client_prepare_headers (c);
      event_set (c->event_p, fd, EV_WRITE, 
		 event_client_send_response, c);      
      event_add (c->event_p, NULL);
      break;
  }
}

void
event_client_send_response (int fd, short event, void *arg)
{
  struct client *c=arg;
  ssize_t count;
  struct buffer *b;
  b = c->buffer;
  count = buffer_write_to_fd (b, fd);
  if (count == -1)
    {
      log_add(LOG_WARNING,"client fd:%d unexpectedly died",fd);
      client_reset_or_free(c,0);
      return;
    }
  if (buffer_empty (b))
    {                       /* EOB */
      if (request_method(c->request)==GET && c->response==200) 
	{
	  if(c->write_style==EVENTS) 
	    {
	      client_cork(c,1);
	      event_set(c->event_p,c->fd,EV_WRITE,event_client_sendfile,c);
	      event_add (c->event_p, NULL);
	    }
	  else 
	    {
	      pthread_create(c->pthr_p,NULL,(void *(*)(void *))pthread_client_sendfile,(void *)c);
	      pthread_detach(*c->pthr_p);
	    }
	}
      else
	client_reset_or_free(c,c->keep_alive);
    }
  else 				/* Write more Headers */
    {
      event_set (c->event_p, c->fd, EV_WRITE, 
		 event_client_send_response, c);      
      event_add (c->event_p, NULL);
    }
}

void
pthread_client_sendfile(struct client *c)
{
  int count;
  client_cork(c,1);
  count=client_sendfile(c);
  client_cork(c,0);
  if (count == -1) {
    log_add(LOG_ERR,"sendfile/write fd:%d %s",c->fd,strerror(errno));
    c->keep_alive=0;
  }
  client_reset_or_free(c,c->keep_alive);
}

void
event_client_sendfile (int fd, short event, void *arg)
{
  int count;
  struct client *c=arg;
  count=client_sendfile(c);
  if (count == -1)
    {
      if (errno == EAGAIN)
	{
	  event_add (c->event_p, NULL);	/* Redo sendfile */
	}
      else {
	log_add(LOG_ERR,"client fd:%d %s",fd,strerror(errno));
	client_cork(c,0);
	client_reset_or_free(c,0);
      }
    }
  else if (c->of_offset == c->content_length)
    {                       /* EOF */
      client_cork(c,0);
      client_reset_or_free(c,c->keep_alive);
    }
  else 
    {
      event_add(c->event_p,NULL); /* Redo sendfile */
    }
}

void 
pthread_client_request(struct client *c) 
{
  struct buffer *b=c->buffer;
  int fd=c->fd;
  
  while(1) {
    c->response=client_request(c);
    switch(c->response) {
    case(RES_CLOSED):
      break;
    case(-1):
      log_add(LOG_WARNING,"pthread_client_request-end fd:%d",fd);
      break;
    default:
      client_prepare_headers(c);
      int count;
      while ((count=buffer_write_to_fd(b,c->fd)) !=0) {
	if (count == -1)
	  {
	    log_add(LOG_WARNING,"client fd:%d unexpectedly died",fd);
	    client_reset_or_free(c,0);
	    return;
	  }
      }
      if (request_method(c->request)==GET && c->response==200) 
	{
	  client_cork(c,1);
	  count=client_sendfile(c);
	  client_cork(c,0);
	}
      client_reset_or_free(c,1);
      if (c->keep_alive)
	continue;
    }
    break;
  }
  client_reset_or_free(c,0);
}
