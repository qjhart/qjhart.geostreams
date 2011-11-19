/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: geostreamd.h,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: geostreamd.h,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uri.h>
#include <time.h>
#include <event.h>
#include <pthread.h>
#include <errno.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>


/* Defaults */
#define TIME_STR_LEN 30
#define DEFAULT_LOG_FACILITY LOG_USER
#define SERVER_BACKLOG 64
#define CLIENT_BUFFER_SIZE 2048
#define CACHE_SIZE 0
#define MAX_HOSTS 32

/* types */
typedef unsigned int response_code;

/* config */

enum multi_type { THREADS,EVENTS };

/* server */
struct server
{
  struct server *prev;
  struct server *next;
  char *host;		/* This host (of config) */
  char *files;
  char *hostname;
  int port;
  char *document_root;
  int document_root_z;

  unsigned int num_clients;
  enum multi_type req_style;
  enum multi_type write_style;
  int cache_size;
  int fd;
  struct in_addr addr;
  struct event *event_p;
  struct event *timer_v;
  struct ll *free_clients;       /* List of clients to choose from */
  struct ll *busy_clients;       /* List of working clients */
};

struct gs_config {
  int facility;
  char *conf_file;
  struct server *def;
  struct server *servers;	 /* List of Hosts */
};

/* Globals */
struct cache *Cache;
char Debug;
char Verbose;
char Daemon;
time_t Time_Cur;
char Time_Str[TIME_STR_LEN];
#ifdef NO_SYSLOG
FILE *servfile;
FILE *logfile;
#endif

enum request_method
{
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

/* config.c */

int config_file(struct gs_config *config);
void config_print(struct gs_config *config);

/* buffer */
struct buffer;
inline char *buffer_readp (struct buffer *b);
inline int buffer_empty (struct buffer *b);
int buffer_rewind (struct buffer *b);
inline int buffer_getc (struct buffer *b);
struct buffer *buffer_malloc (size_t size);
void buffer_free (struct buffer *b);
void buffer_reset (struct buffer *b);
int buffer_strcat (struct buffer *b, char *str);
int buffer_printf (struct buffer *b, char *format, ...);
size_t buffer_read_from_fd (struct buffer *b, int fd);
size_t buffer_write_to_fd (struct buffer *b, int fd);

/* cache */
struct cfile 
{
  struct cfile *prev;
  struct cfile *next;
  char *fn;
  int fn_z;
  int fd;
  void *mmap;
  struct stat finfo;
  int count;
};
struct cache *cache_new(int maximum_size,int start_count);
void cache_file_done(struct cache *c,struct cfile *cf);
struct cfile *cache_file_get(struct cache *c,char *fn,
			     enum request_method m,response_code *rc);


/* client */
struct server;
struct client;
struct ll;
struct ll_item;

struct ll *ll_new ();
int ll_empty (struct ll *l);
inline void *ll_shift (struct ll *l);
void *ll_head(struct ll *l);
void *ll_tail(struct ll *l);
void *ll_pop(struct ll *l);
void ll_unshift (struct ll *l,struct ll_item *c);
void ll_free (struct ll *c);
void ll_unlink (struct ll *l, struct ll_item *c);
void ll_link (struct ll *l, struct ll_item *c);
void ll_tohead (struct ll *l, struct ll_item *c);

struct client *client_new (struct server *s,enum multi_type req_style);
int client_accept (struct client *c, int listen_fd,enum multi_type write_style);

/* request */
enum request_state
{
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
};


struct request *request_new (struct client *c);
void request_free (struct request *r);
void request_close (struct request *r);
inline char *request_status_line (struct request *r);
inline enum request_method request_method (struct request *r);
inline uri_t *request_uri (struct request *r);
inline char request_keep_alive (struct request *r);
enum request_state request_scan (struct request *r, struct buffer *b);
int request_parse (struct request *r, char *input);

/* server */

struct server *server_new(struct server *server);
struct server *server_init (struct server *server);
void server_connect (struct server *);
int server_listen (struct server *);
void server_free_client (struct server *s, struct client *c);

/* Most Common Responses, but can use numbers too */
#define RES_WORKING 0           /* Internal Code */
#define RES_CLOSED 1            /* Internal Code Client is gone */
#define RES_GOODREQUEST 2       /* Internal Code Client is gone */

#define RES_Continue 100
#define RES_OK 200
#define RES_BadRequest 400      /* Section 10.4.1: */
#define RES_MethodNotAllowed 405        /* Section 10.4.6: */
#define RES_RequestTimeout 408  /* Section 10.4.9: */
#define RES_ServerError 500     /* Section 10.5.1: */
#define RES_TooLarge    413

/* log */
#include <syslog.h>             /* for LOG_DEBUG, etc */
void log_open (const char *prgname, int option, int facility);
void die (const char *format, ...);
#if 0
void debug (const char *format, ...);
#else
#define debug(args...) if(Debug){log_add(LOG_DEBUG,args);}
#endif

void log_add (int priority, const char *format, ...);
void log_close (void);

