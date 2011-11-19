/* 
   GOESD - Streaming socket MNG writer.

   Copyright (C) 2004 University of California, Davis

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

  $Id: goesd.cpp,v 1.1 2004/01/30 18:37:21 qjhart Exp $

  $Log: goesd.cpp,v $
  Revision 1.1  2004/01/30 18:37:21  qjhart
  Start at goesd


*/

static const char rcsid[] = "$Id: goesd.cpp,v 1.1 2004/01/30 18:37:21 qjhart Exp $";

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <getopt.h>

#include <event.h>

/* the header for prototypes and errors */
#include "goesd.h"
#include "gvarHeader/gvarHeader.h"
#include "gvarBlock/gvarBlock.h"
#include "lineDoc/lineDoc.h"
#include "block1to10/block1to10.h"



#include <iostream>
using std::iostream ;
using std::cout ;

#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))


static char startpatt[8]={0xAA,0xD6,0x3E,0x69,0x02,0x5A,0x7F,0x55};
static char endpatt[8]={0x55,0x7F,0x5A,0x02,0x69,0x3E,0xD6,0xAA};

struct gvar {
  unsigned int seqnum;
  int numread;
  char *bp;
  char *end;
  char blkbuf[GVNETBUFFSIZE];
};

struct goesd *goesd_new(void) {
  struct goesd *g;
  if ((g=(struct goesd *)malloc(sizeof *g)) == NULL)
    return NULL;
  memset(g, 0, sizeof *g);
  return g;
}  

void
gvar_init(struct gvar *g) {
  g->seqnum=0;
  g->bp = (char *)g->blkbuf;
}


int gvar_connect(struct gvar *g,char *ipaddr)
{
  int fd;
  struct sockaddr_in sa;
  int optval;
 
  fd=socket(PF_INET,SOCK_STREAM,0);
  if (fd==-1) return(fd);
  memset(&sa,0,sizeof(sa));
  sa.sin_family=AF_INET;
  // inet_aton(host_p->h_addr_list[0],(in_addr *)&sa.sin_addr.s_addr);
  sa.sin_addr.s_addr=inet_addr(ipaddr);
  sa.sin_port=htons(21009);
  if (connect(fd,(struct sockaddr *)&sa,sizeof(sa))==-1) return -1;
  optval=1;
  // SCT added 8/4/03
  setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
  // non-portable options that are required to speed up the keep-alive
  // process by default it will wait 2hrs! after idle before the fist keep alive
  // since this link must be over a fast network these values are justified
  // sets all three default parameters so nothing is left to some riduculous default
  optval=30;  // let it be idle for 30 seconds before sending a keep alive
  setsockopt(fd,SOL_TCP,TCP_KEEPIDLE,&optval,sizeof(optval));
  optval=2;   // go for two retries before error out
  setsockopt(fd,SOL_TCP,TCP_KEEPCNT,&optval,sizeof(optval));
  optval=15;  // fifteen seconds between retries
  setsockopt(fd,SOL_TCP,TCP_KEEPINTVL,&optval,sizeof(optval));
  return(fd);
}
  

void gvar_read(int fd,short event, void *arg) {
  struct gvar *g = (struct gvar *)arg;
  ssize_t n;
  int size;
  unsigned int seqnum;

  if (g->blkbuf==g->bp) 
    {
      n=read(fd,g->bp,16);
      if (n <= 0) {
	perror("Read");
	return;
      }
      if (memcmp(g->bp,startpatt,8)) 
	{
	  perror("Bad Start");
	  return;
	}
      size=ntohl(*((int*)(g->bp+8)))+24;
      if (size>GVNETBUFFSIZE) 
	{
	  perror("Block Too Big");
	  return;
	}
      g->end=g->bp+size;
      
      seqnum=ntohl(*((int*)(g->bp+12)));
      if (seqnum != g->seqnum+1) 
	{
	  perror("Bad Sequence");
	  return;
	}
      g->seqnum=seqnum;
      g->bp+=n;
    }

  n=read(fd,g->bp,g->end-g->bp);
  if (n <= 0) {
    perror("Read");
    return;
  }
  g->bp+=n;
  if (g->bp==g->end) {
    if (memcmp(g->bp-8,endpatt,8))
      {
	perror("Bad Ending");
	return;
      }    
    GvarBlock* block = new GvarBlock (g->blkbuf+16,g->end-g->blkbuf-24) ;    
    GvarHeader* header = block->getHeader () ;
    header->print (cout) ;
    if ((header->blockId ()>= 1) &&
	(header->blockId () <= 10)) {
      Block1to10 *block1to10 = new Block1to10 (block) ;
      LineDoc *lineDoc = block1to10->getLineDoc () ;
      lineDoc->print (cout) ;
      delete block1to10 ;
    }
    delete block ;
  }
}

void
fifo_accept(int listen_fd, short event, void *arg)
{
  socklen_t l;
  int new_fd;
  struct event new_ev;

  sockaddr sa;

  l = sizeof sa;
  new_fd = accept (listen_fd, (struct sockaddr *) &sa, &l);
  if (new_fd == -1)
    {
      if (errno != EAGAIN) 
        syslog (LOG_ERR, "accept %s", strerror (errno));
      return;
    }
  fcntl (new_fd, F_SETFD, FD_CLOEXEC);
  fcntl (new_fd, F_SETFL, O_NONBLOCK);

  fprintf(stderr, "fifo_read called with fd: %d, event: %d, arg: %p\n",
	  listen_fd, event, arg);

  int i;
  int rc;
  time_t f;
  char *t;
  for (i=0; i< 10; i++) {
    f=time(NULL);
    t=ctime(&f);
    if((rc=write(new_fd,t,strlen(t)))==-1)
      perror("write");
    else if (rc==0)
      fprintf(stderr,"No Write");
  }

  //event_set(&new_ev, socket, EV_WRITE, fifo_write, &new_ev);
  //event_add(&new_ev, NULL);
  close(new_fd);

}


int main(int argc,char *argv[])
{ 
  struct gvar *gvar;
  int gvar_fd;
  struct event gvar_v;

  struct event listen_v;
  struct stat st;
  struct sockaddr_un sa;

  char *host="169.237.153.55";

  char *fifo = "/tmp/goes/linedoc";
  int listener;

  Debug=0;
  Verbose=0;

  static struct option long_options[] = {
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"host",1,0,'h'},
    {"config-file",1,0,'f'},
    {0, 0, 0, 0}
  };

  if ((gvar = (struct gvar *) malloc(sizeof * gvar)) == 0) 
    {
      perror("gvar");
      return(1);
    }
  gvar_init(gvar);

  while (1) {
    int option_index = 0;
    int c = getopt_long (argc, argv, "dvh:",
			 long_options, &option_index);
    
    if (c == -1)
      break;
    switch (c) {
    case 'h':       
      host=optarg;
    case 'd':
      Debug=1;
    case 'v':
      Verbose=1;
      break;
    default:
      exit(-1);
    }
  }

  syslog(LOG_NOTICE,"%s starting",argv[0]);

  // Connect to gvar
#if 0
  if ((gvar_fd=gvar_connect(gvar,host)) == -1)
    {
      perror("gvar_connect");
      return(1);
    }
#endif  

  // Connect to PNG/MNG readers
  if (lstat (fifo, &st) == 0) {
    if ((st.st_mode & S_IFMT) == S_IFREG)
      {
	errno = EEXIST;
	perror("lstat");
	exit (1);
      }
  }
#if 0
  if (mkfifo (fifo, 0600) == -1) {
    perror("mkfifo");
    exit (1);
  }
#endif

  listener = socket(PF_UNIX,SOCK_STREAM,0);
  if (listener == -1) {
    perror("open");
    exit (1);
  }
  sa.sun_family=AF_UNIX;
  strncpy(sa.sun_path,fifo,108);
  unlink (fifo);
  if (bind(listener,(const sockaddr *)&sa,sizeof sa) == -1) 
    {
      perror("listen");
      exit (1);
    }

  if (listen (listener, 128) == -1)
    {
      perror("listen");
      exit (1);
    }

  event_init();
  //event_set(&gvar_v, gvar_fd, EV_READ | EV_PERSIST, gvar_read, gvar);
  //event_add(&gvar_v, NULL);
  event_set(&listen_v, listener, EV_READ | EV_PERSIST, fifo_accept, gvar);
  event_add(&listen_v, NULL);
  event_dispatch();

  if (Debug) syslog(LOG_DEBUG,"cleaning up");
  //free(blkbuff);
  //goes_close(g);
  return (0);
}


