/*
  goes - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: GvarStream.cpp,v 1.4 2005/06/03 18:59:16 qjhart Exp $

  $Log: GvarStream.cpp,v $
  Revision 1.4  2005/06/03 18:59:16  qjhart
  Removed libgvar from source

  Revision 1.3  2005/04/13 22:29:09  qjhart
  Started Fifo version

  Revision 1.2  2005/04/06 23:43:20  jiezhang
  Remove the old HTTP server C code and check in the HTTP server C++ code.

  Revision 1.1  2005/03/06 10:35:01  qjhart
  Added some CPP versions of some files.  Updated Geostreamd

  Revision 1.1  2004/11/30 19:01:15  qjhart
  New server


*/
//#include "geostreamd#include <sys/types.h>
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

#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#define debug(args...) fprintf(stderr,args); fflush (stderr)

#include "GvarStream.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "gvar/Gvar.h"

static const char rcsid[] =
  "$Id: GvarStream.cpp,v 1.4 2005/06/03 18:59:16 qjhart Exp $" ;

static char startpatt[8]={0xAA,0xD6,0x3E,0x69,0x02,0x5A,0x7F,0x55};
static char endpatt[8]={0x55,0x7F,0x5A,0x02,0x69,0x3E,0xD6,0xAA};

// extern char Debug ;
// extern Geostream::Log* gLog ;

namespace Geostream {

  GvarStream::GvarStream(char *ipaddr,int port)
  {
	if ((this->event_p = (struct event *)malloc (sizeof *(this->event_p))) 
		== NULL)
	  {
		delete this;
	  }
	
	if ((this->ipaddr = (char *)malloc(strlen(ipaddr))) == (void *) 0) {
	  delete this;
	}
	strcpy(this->ipaddr,ipaddr);
	this->port=port;
	this->seqnum=0;
	this->bp = (char *)this->blkbuf;
  }
  
  GvarStream::~GvarStream () {
    if (event_p != NULL) {
	  // Is this okay? 
	  //http://www.parashift.com/c++-faq-lite/freestore-mgmt.html#faq-16.3
      delete event_p ;
    }

    if (ipaddr != NULL) {
      delete ipaddr ;
    }
  }

  FifoWriter<Gvar::LineDoc> *GvarStream::LineDocWriter(FifoWriter<Gvar::LineDoc> *p) {
	m_LineDocWriter = p;
	return m_LineDocWriter;
  }

  FifoWriter<Gvar::LineDoc> *GvarStream::LineDocWriter(void) {
	return m_LineDocWriter;
  }

  int GvarStream::connect(void)
  {
	int fd;
	struct sockaddr_in sa;
	int optval;
	
	fd=socket(PF_INET,SOCK_STREAM,0);
	if (fd==-1) return(fd);
	memset(&sa,0,sizeof(sa));
	sa.sin_family=AF_INET;
	// inet_aton(host_p->h_addr_list[0],(in_addr *)&sa.sin_addr.s_addr);
	sa.sin_addr.s_addr=inet_addr(this->ipaddr);
	sa.sin_port=htons(this->port);
	if (::connect(fd,(struct sockaddr *)&sa,sizeof(sa))==-1) return -1;
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

  int GvarStream::listen (void)
  {
	debug ("gvar listening");
	this->fd=this->connect();

	debug ("fd=%d\n", fd) ;
	// event_set (this->event_p, this->fd, EV_READ | EV_PERSIST, 
	// 	   GvarStream::readEventHandler,(void *)this);
	// event_add (this->event_p, NULL);

	return 0;
  }

  void GvarStream::read(void) {
	ssize_t n;
	int size;
	unsigned int seqnum;
	
#if 1
	int datalen = 16 ;
	int bp = 0 ;
        while(1) {
	  numread=recv(fd, blkbuf+bp,datalen-bp, MSG_NOSIGNAL);
	  if (numread<=0) perror("Read error") ;
	  bp+=numread;
	  if (bp==16) {
	    if (memcmp(blkbuf,startpatt,8)) perror("Bad Start") ;
	    datalen=ntohl(*((int*)(blkbuf+8)))+24;
	    seqnum=ntohl(*((int*)(blkbuf+12)));
	    if (seqnum!=this->seqnum+1) perror("Bad Sequence:") ;
	    if (datalen>GVNETBUFFSIZE) perror("Block Too Big") ;
	    this->seqnum=seqnum;
	    this->end=this->blkbuf+datalen;
	  }
	  if (bp==datalen) {
	    if (memcmp(blkbuf+datalen-8,endpatt,8))  perror("Bad End") ;
	    // memcpy(pbuff,buffer+16,min(datalen-24,buffsize));
	    // *pnumread=min(datalen-24,buffsize);
	    break ;
	  }
        }

	// debug("s:%d size:%d togo:%d total:%d\n",this->seqnum,datalen,this->end-this->bp,this->end-this->blkbuf);
	new_block () ;
#else

	if (this->bp-this->blkbuf < 16) 		/* New Block */
	  {
		n=::read(this->fd,this->bp,16-(this->bp-this->blkbuf));
		if (n <= 0) {
		  perror("Read");
		  return;
		}
		this->bp+=n;
		if (this->bp -  this->blkbuf == 16) {
		  if (memcmp(this->blkbuf,startpatt,8)) 
			{
			  perror("Bad Start");
			}
		  size=ntohl(*((int*)(this->blkbuf+8)))+24;
		  if (size>GVNETBUFFSIZE) 
			{
			  perror("Block Too Big");
			}
		  this->end=this->blkbuf+size;
		  
		  seqnum=ntohl(*((int*)(this->blkbuf+12)));
		  // debug("NEW seq:%d size:%d read:%d",seqnum,size,n);
		  if (seqnum != this->seqnum+1) 
			{
			  perror("Bad Sequence:");
			}
		  this->seqnum=seqnum;
		}
	  }
	/* Read Data */
	n=::read(fd,this->bp,this->end-this->bp);
	// debug("s:%d size:%d togo:%d total:%d",this->seqnum,n,this->end-this->bp,this->end-this->blkbuf);
	if (n <= 0) {
	  perror("Read");
	  return;
	}
	this->bp+=n;
	if (this->bp==this->end) {			/* Complete Block */
	  this->new_block();
	  /* reset the buffer */
	  this->bp=(char *)this->blkbuf;
	  /* Test of Junk */
	  // SeqNum=this->seqnum;
	}

#endif
  }
  
  // If Buffers are attached to the GVAR stream, they are filled with the 
  // new block
  void GvarStream::new_block(void) {	  
	//	if (memcmp(this->bp-8,endpatt,8))
	// throw Lib::Exception("Bad GVAR Block Ending");

    Gvar::Block* block = 
      new Gvar::Block (this->blkbuf+16,this->end-this->blkbuf-24);
    Gvar::Header* header = block->getHeader () ;
    //debug("id=%d size=%d@%d repeat=%d count=%d\n",
    //	  header->blockId(),header->wordCount(),header->wordSize(),
    //	  header->repeatFlag(),header->blockCount());
    if ((header->blockId ()>= 1) && (header->blockId () <= 10)) {
      Gvar::Block1to10 *block1to10 = new Gvar::Block1to10 (block) ;
      Gvar::LineDoc *lineDoc = block1to10->getLineDoc () ;
      if (m_LineDocWriter != NULL) {
	m_LineDocWriter->write(lineDoc);
	debug("spcid=%d spsid=%d lside=%d lidet=%d licha=%d risct=%d l1scan=%d	l2scan=%d lpixls=%d lwords=%d lzcor=%d\n",lineDoc->spcid(),lineDoc->spsid(),lineDoc->lside(),lineDoc->lidet(),lineDoc->licha(),lineDoc->risct(),lineDoc->l1scan(),lineDoc->l2scan(),lineDoc->lpixls(),lineDoc->lwords(),lineDoc->lzcor());
      }
      // delete block1to10 ;
    }
    // delete block ;
  }


  void GvarStream::readEventHandler (int fd,short event, void *arg) {
    GvarStream *g;
    g = (GvarStream *)arg;
    g->read();
  }
}

