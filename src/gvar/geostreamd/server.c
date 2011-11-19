/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: server.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: server.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "geostreamd.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>

struct server *
server_new(struct server *def)
{
  struct server *server;
  if ((server = malloc (sizeof *server)) == (void *) 0)
    {
      log_add (LOG_ERR, "new server");
      exit (1);
    }
  server->addr.s_addr = 0;
  if (def != NULL)
    {
      memcpy(server,def,sizeof *server);
    }
  return server;
}

struct server *
server_init (struct server *server)
{
  struct client *client;
  int n;
  if ((server->event_p = malloc (sizeof *(server->event_p))) == NULL)
    {
      free(server);
      return NULL;
    }
  if ((server->timer_v = malloc (sizeof *(server->timer_v))) == NULL)
    {
      free(server->event_p);
      free(server);
      return NULL;
    } 
  if ((server->free_clients = ll_new ()) == NULL)
    {
      log_add (LOG_ERR, "init free_clients");
      exit (1);
    }
  for (n = 0; n < server->num_clients; n++)
    {
      if ((client = client_new (server,server->write_style)) == NULL)
        {
          log_add (LOG_ERR, "New Clients");
          exit (1);
        }
      ll_link (server->free_clients, (struct ll_item *)client);
    }

  if ((server->busy_clients = ll_new ()) == NULL)
    {
      log_add (LOG_ERR, "init free_clients");
      exit (1);
    }
  return (server);
}

void
server_connect (struct server *server)
{
  int onoff;
  struct sockaddr_in sa;

  server->fd = socket (AF_INET, SOCK_STREAM, 0);
  if (server->fd == -1)
    {
      log_add (LOG_ERR, "socket");
      exit (1);
    }
  onoff = 1;
  if (setsockopt (server->fd, SOL_SOCKET, SO_REUSEADDR, &onoff, sizeof onoff)
      == -1)
    {
      log_add (LOG_ERR, "setsockopt %s", strerror (errno));
      exit (1);
    }
  if (server->req_style==EVENTS)
    fcntl (server->fd, F_SETFL, O_NONBLOCK);
  memset (&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr = server->addr;
  sa.sin_port = htons (server->port);
  if (bind (server->fd, (struct sockaddr *) &sa, sizeof sa) == -1)
    {
      log_add (LOG_ERR, "bind");
      exit (1);
    }
}

void
server_free_client (struct server *s, struct client *c)
{
  int was_empty;
  was_empty=ll_empty(s->free_clients);
  debug("server_free_client: %s %p",ll_empty(s->free_clients)?"EMPTY":"",c);
  ll_unlink (s->busy_clients, (struct ll_item *)c);
  ll_link (s->free_clients, (struct ll_item *)c);
  /* If the free_clients was empty, we can restart new_clients again */
  debug("server_free_client: %s %p",ll_empty(s->free_clients)?"EMPTY":"",c);
  if (was_empty)
    {
      debug("server_free_client: Listen");
      event_add (s->event_p, NULL);    
    }
}

void
event_server_timekeeper (int fd, short event, void *arg)
{
  struct server *s = arg;
  struct tm tm;
  time_t new_time;
  struct timeval tv;
  timerclear(&tv);
  tv.tv_sec=1;

  time (&new_time);
  Time_Cur = new_time;
  gmtime_r (&new_time, &tm);
  strftime (Time_Str, TIME_STR_LEN, "%a, %d %b %Y %T GMT", &tm);
  /* debug(Time_Str); */
  event_add(s->timer_v,&tv);
}  

void
event_server_new_clients (int fd, short event, void *arg)
{
  int count;
  int new_fd;
  struct client *c;
  struct server *server=arg;

  c = ll_shift(server->free_clients);
  if (c == NULL)
    {
      debug("event_server_new_client ALL BUSY");
      return;
    }
  else 
    {
      debug("new_client: %p",c);
      event_add (server->event_p, NULL);
      if ((new_fd = client_accept (c, fd,server->req_style)) == -1)
	{
	  debug("accept %s",strerror(errno));
	  ll_link (server->free_clients, (struct ll_item *)c);
	  return;
	}
      else {
	debug("event_server_new_client fd:%d",new_fd);
	ll_link (server->busy_clients, (struct ll_item *)c);
	count++;
      }
    }
}

int
server_listen (struct server *server)
{
  struct timeval tv;
  timerclear(&tv);
  tv.tv_sec=1;

  /* If backlog fills up, clients are refused */
  if (listen (server->fd, SERVER_BACKLOG) == -1)
    {
      perror("listen");
      return(-1);
    }
  debug ("listening");
  event_init ();

  evtimer_set(server->timer_v,event_server_timekeeper,server);
  event_add(server->timer_v,&tv);

  event_set (server->event_p, server->fd, EV_READ, 
	     event_server_new_clients,server);
  event_add (server->event_p, NULL);
  event_dispatch ();
  return 0;
}

