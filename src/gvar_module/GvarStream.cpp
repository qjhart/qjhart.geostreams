/*
  goes - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: GvarStream.cpp,v 1.12 2005/08/18 17:22:21 jiezhang Exp $

  $Log: GvarStream.cpp,v $
  Revision 1.12  2005/08/18 17:22:21  jiezhang
  Add the support for region of interest request.

  Revision 1.11  2005/08/10 00:45:54  jiezhang
  Reduce the times for copying data by calling a new Row class.

  Revision 1.10  2005/08/06 22:33:37  jiezhang
  Fix seqNum in GvarStream.cpp.

  Revision 1.9  2005/08/04 14:45:40  jiezhang
  Add request query string parsing.

  Revision 1.8  2005/08/03 00:07:23  jiezhang
  Minor changes to compile in g++ 3.4.4.

  Revision 1.7  2005/07/29 00:10:19  haiyan
  add constructor/methods to read block from file


  Revision 1.6  2005/07/27 05:43:30  jiezhang
  Change to support multiple output format.

  Revision 1.5  2005/07/17 03:14:16  jiezhang
  Fix an error when determining the start y position for block3to10.

  Revision 1.4  2005/07/14 20:33:45  jiezhang
  Create and read/write FIFO in a shared memory segment.

  Revision 1.3  2005/07/07 20:58:06  jiezhang
  Check in the code to support row by row output from apache.

  Revision 1.2  2005/06/16 02:55:45  jiezhang
  Add more support for writing GRASS files.

  Revision 1.1.1.1  2005/06/03 22:21:41  jiezhang
  Apache module for geostream server.

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
// #include <uri.h>
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

#include "Gvar.h"
#include "Block.h"

static const char rcsid[] =
  "$Id: GvarStream.cpp,v 1.12 2005/08/18 17:22:21 jiezhang Exp $" ;

static char startpatt[8]={0xAA,0xD6,0x3E,0x69,0x02,0x5A,0x7F,0x55};
static char endpatt[8]={0x55,0x7F,0x5A,0x02,0x69,0x3E,0xD6,0xAA};

// extern char Debug ;
// extern Geostream::Log* gLog ;

namespace Geostream {

  GvarStream::GvarStream(char *ipaddr,int port) {
    workingOnFile = false;

    if ((this->event_p = (struct event *)malloc (sizeof *(this->event_p))) 
	== NULL) {
      delete this;
    }
	
    if ((this->ipaddr = (char *)malloc(strlen(ipaddr)+1)) == (void *) 0) {
      delete this;
    }
    strcpy(this->ipaddr,ipaddr);
    this->port=port;
    this->seqnum=0;
    this->bp = (char *)this->blkbuf;

    m_allRowsWriter = NULL ;

    for (int channelNo=0; channelNo<NUM_OF_CHANNELS; channelNo++) {
      m_rowWriters[channelNo] = NULL ;
    }

    m_shmRowsWriter = NULL ;

    m_block0 = NULL ;

    m_prevFrameId = -1 ;
    m_prevFrameDef = NULL ;
  
    blkFile = NULL;

  }
 

  //constructor for read block from file 
  GvarStream::GvarStream (char* filename) {
    workingOnFile = true;

    blkFile = fopen (filename, "rb") ;
    
    if (blkFile==NULL) exit(1);

    fd = fileno(blkFile);

    this->seqnum=0;
    this->bp = (char *)this->blkbuf;

    m_allRowsWriter = NULL ;

    for (int channelNo=0; channelNo<NUM_OF_CHANNELS; channelNo++) {
      m_rowWriters[channelNo] = NULL ;
    }

    m_shmRowsWriter = NULL ;
    m_block0 = NULL ;
    m_prevFrameId = -1 ;
    m_prevFrameDef = NULL ;
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

    if (m_block0 != NULL) {
      delete m_block0 ;
    }

    delete m_prevFrameDef ;

    if (blkFile != NULL) {
      fclose(blkFile);
    }

    close () ;
  }

  FifoWriter<Gvar::LineDoc> *GvarStream::LineDocWriter(FifoWriter<Gvar::LineDoc> *p) {
	m_LineDocWriter = p;
	return m_LineDocWriter;
  }

  FifoWriter<Gvar::LineDoc> *GvarStream::LineDocWriter(void) {
	return m_LineDocWriter;
  }

  FifoWriter<Row> *GvarStream::allRowsWriter (FifoWriter<Row> *w) {
    if (m_allRowsWriter != NULL && m_allRowsWriter != w) {
      delete m_allRowsWriter ;
    }

    m_allRowsWriter = w ;

    return m_allRowsWriter ;
  }

  FifoWriter<Row> *GvarStream::allRowsWriter () {
    return m_allRowsWriter ;
  }

  FifoWriter<Row> *GvarStream::rowWriter (FifoWriter<Row> *w, int channelNo) {
    if (m_rowWriters[channelNo] != NULL && m_rowWriters[channelNo] != w) {
      delete m_rowWriters[channelNo] ;
    }

    m_rowWriters[channelNo] = w ;

    return m_rowWriters[channelNo] ;
  }

  FifoWriter<Row> *GvarStream::rowWriter (int channelNo) {
    return m_rowWriters[channelNo] ;
  }

  Geostream::RowFifoWriter* GvarStream::shmRowsWriter 
  (Geostream::RowFifoWriter* w) {
    if (m_shmRowsWriter != NULL && m_shmRowsWriter != w) {
      delete m_shmRowsWriter ;
    }

    m_shmRowsWriter = w ;
    return m_shmRowsWriter ;
  }

  Geostream::RowFifoWriter* GvarStream::shmRowsWriter () {
    return m_shmRowsWriter ;
  }

  Geostream::RowFifo* GvarStream::shmRowFifo (Geostream::RowFifo* fifo) {
    m_shmRowFifo = fifo ;
    return m_shmRowFifo ;
  }

  Geostream::RowFifo* GvarStream::shmRowFifo () {
    return m_shmRowFifo ;
  }


  // FifoWriter<Gvar::Block0

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

  bool GvarStream::listen (void)
  {
	debug ("gvar listening\n");
	this->fd=this->connect();
	this->seqnum=0;

	// debug ("fd=%d\n", fd) ;
	// event_set (this->event_p, this->fd, EV_READ | EV_PERSIST, 
	// 	   GvarStream::readEventHandler,(void *)this);
	// event_add (this->event_p, NULL);

	return (fd != -1) ;
  }

  bool GvarStream::read(void) {
	ssize_t n;
	int size;
	unsigned int seqnum;
	
#if 1
	int datalen = 16 ;
	int bp = 0 ;

        while(1) {
	  numread=recv(fd, blkbuf+bp,datalen-bp, MSG_NOSIGNAL);
	  if (numread<=0) {
	    fprintf (stderr, "Read error"); fflush (stderr);
	    return false ;
	  }

	  bp+=numread;
	  if (bp==16) {
	    if (memcmp(blkbuf,startpatt,8)) {
	      fprintf (stderr, "Bad Start"); fflush (stderr) ;
	      return false ;
	    }

	    datalen=ntohl(*((int*)(blkbuf+8)))+24;
	    seqnum=ntohl(*((int*)(blkbuf+12)));
	    if (seqnum!=this->seqnum+1) {
	      fprintf (stderr, "Bad Sequence"); fflush (stderr) ;
	    }
	    if (datalen>GVNETBUFFSIZE) {
	      fprintf (stderr, "Block Too Big"); fflush (stderr) ;
	      return false ;
	    }
	    this->seqnum=seqnum;
	    this->end=this->blkbuf+datalen;
	  }
	  if (bp==datalen) {
	    if (memcmp(blkbuf+datalen-8,endpatt,8))  {
	      fprintf (stderr, "Bad End") ; fflush (stderr) ;
	      return false ;
	    }
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

	return true ;
  }

  Gvar::Block* GvarStream::readBlock () {
	if ( workingOnFile ) {
		return readBlockFile();
	}
	else {
		return readBlockSocket();
	}
  }

  Gvar::Block* GvarStream::readBlockSocket () {
    ssize_t n;
    int size;
    unsigned int seqnum;

    int datalen = 16 ;
    int bp = 0 ;
    while(1) {
      numread=recv(fd, blkbuf+bp,datalen-bp, MSG_NOSIGNAL);
      if (numread<=0) {
	fprintf(stderr, "Read error") ; fflush (stderr) ;
	return NULL ;
      }
      bp+=numread;
      if (bp==16) {
        if (memcmp(blkbuf,startpatt,8)) {
	  fprintf (stderr, "Bad Start") ; fflush (stderr) ;
	  return NULL ;
	}
        datalen=ntohl(*((int*)(blkbuf+8)))+24;
        seqnum=ntohl(*((int*)(blkbuf+12)));
        if (seqnum!=this->seqnum+1) {
	  fprintf (stderr, "Bad Sequence:") ; fflush (stderr) ;
	}

        if (datalen>GVNETBUFFSIZE) {
	  fprintf (stderr, "Block Too Big") ; fflush (stderr) ;
	  return NULL ;
	}
        this->seqnum=seqnum;
        this->end=this->blkbuf+datalen;
      }
      if (bp==datalen) {
        if (memcmp(blkbuf+datalen-8,endpatt,8)) {
	  fprintf (stderr, "Bad End") ; fflush (stderr) ;
	  return NULL ;
	}
        break ;
      }
    }

    // debug("s:%d size:%d togo:%d total:%d\n",this->seqnum,datalen,this->end-this->bp,this->end-this->blkbuf);
    Gvar::Block* block = 
      new Gvar::Block (this->blkbuf+16,this->end-this->blkbuf-24);

    return block ;
  }


  /** read block from a file instead of a socket
   *  the 16 start bytes and 8 end bytes are removed 
   *  when being stored on GOES10 
   */
  Gvar::Block* GvarStream::readBlockFile() {
    ssize_t n;
    int size;
    unsigned int seqnum;

    int datalen = 16 ;
    int bp = 0 ;

    fseek (blkFile, 0, SEEK_END);
    datalen = ftell (blkFile);
    rewind (blkFile);
      
    while(1) {
       numread=fread(blkbuf+bp, 1, datalen-bp, blkFile);

       if (numread<=0) perror("Read error") ;
       bp+=numread;
      
       /** unless the first 16 bytes and last 8 bytes 
        *  are not removed, this if clause is not needed
        */
       if (bp==16) {
         if (memcmp(blkbuf,startpatt,8)) perror("Bad Start") ;
         datalen=ntohl(*((int*)(blkbuf+8)))+24;
         seqnum=ntohl(*((int*)(blkbuf+12)));
         if (seqnum!=this->seqnum+1) perror("Bad Sequence:") ;
       }

       if (datalen>GVNETBUFFSIZE) perror("Block Too Big") ;
         this->seqnum=seqnum;
         this->end=this->blkbuf+datalen;
       
       if (bp==datalen) {
         break ;
      }
     
    }

    // debug("s:%d size:%d togo:%d total:%d\n",this->seqnum,datalen,this->end-this->bp,this->end-this->blkbuf);
    Gvar::Block* block = 
      new Gvar::Block (this->blkbuf,this->end-this->blkbuf);

    return block ;
  }



  void GvarStream::close () {
    if (fd != -1) {
      shutdown (fd, SHUT_RD) ;
      fd = -1 ;
    }

  }
  
  // If Buffers are attached to the GVAR stream, they are filled with the 
  // new block
  void GvarStream::new_block(void) {	  
    //	if (memcmp(this->bp-8,endpatt,8))
    // throw Lib::Exception("Bad GVAR Block Ending");

    Gvar::Block* block = 
      new Gvar::Block (this->blkbuf+16,this->end-this->blkbuf-24);

    convertBlockToRows (block) ;

//     Gvar::Header* header = block->getHeader () ;
//     //debug("id=%d size=%d@%d repeat=%d count=%d\n",
//     //	  header->blockId(),header->wordCount(),header->wordSize(),
//     //	  header->repeatFlag(),header->blockCount());
//     if ((header->blockId ()>= 3) && (header->blockId () <= 10)) {
//       Gvar::Block3to10 *block3to10 = new Gvar::Block3to10 (block) ;
//       Gvar::LineDoc *lineDoc = block3to10->getLineDoc () ;
//       if (m_LineDocWriter != NULL) {
// 	m_LineDocWriter->write(lineDoc);
// 	debug("spcid=%d spsid=%d lside=%d lidet=%d licha=%d risct=%d l1scan=%d	l2scan=%d lpixls=%d lwords=%d lzcor=%d\n",lineDoc->spcid(),lineDoc->spsid(),lineDoc->lside(),lineDoc->lidet(),lineDoc->licha(),lineDoc->risct(),lineDoc->l1scan(),lineDoc->l2scan(),lineDoc->lpixls(),lineDoc->lwords(),lineDoc->lzcor());
//       }
      // delete block3to10 ;
//  }
    // delete block ;
  }

  void GvarStream::convertBlockToRows (Gvar::Block *block) {

    Gvar::Header *header = block->getHeader () ;

    if (header->blockId () == 0) {
      if (m_block0 != NULL) {
	delete m_block0 ;
      }

      m_block0 = new Gvar::Block0 (block) ;

      // check if this is a new frame
      Gvar::Block0Doc* block0Doc = m_block0->getBlock0Doc () ;

      if (block0Doc->frame () != m_prevFrameId) {
	Rectangle rect (block0Doc->wpx (), block0Doc->nln (), 
			block0Doc->epx()-block0Doc->wpx()+1,
			block0Doc->sln()-block0Doc->nln()+1) ;
	if (m_prevFrameDef != NULL) {
	  delete m_prevFrameDef ;
	}

	m_prevFrameDef = new FrameDef (block0Doc->frame (), rect);

	Geostream::writeFrameDef (m_shmRowFifo, rect, block0Doc->frame ()) ;

	debug ("A new frame arrives.\n") ;

	m_prevFrameId = block0Doc->frame () ;
      }
    }

    else if (header->blockId () == 1 || 
           header->blockId () == 2) {
      if (m_block0 == NULL) {
	delete block ;
	return ;
      }

      Gvar::Block1or2* block1or2 = new Gvar::Block1or2 (block) ;

      convertBlock1or2ToRows (m_block0, block1or2) ;

      delete block1or2;
      delete block ;
    }

    else if (header->blockId () >= 3 && 
	     header->blockId () <= 10) {
      if (m_block0 == NULL) {
	delete block ;
	return ;
      }

      Gvar::Block3to10* block3to10 = new Gvar::Block3to10 (block) ;

      convertBlock3to10ToRows (m_block0, block3to10) ;

      delete block3to10;
      delete block ;
    }

    else { // other blocks
      delete block ;
    }

  }

  void GvarStream::convertBlock1or2ToRows (Gvar::Block0* block0,
					   Gvar::Block1or2 *block1or2) {
    Gvar::Block0Doc* block0Doc = block0->getBlock0Doc () ;

    Gvar::Header *header = block1or2->getBlock()->getHeader () ;

    int frameId = block0Doc->frame () ;
    int columnStart = block0Doc->wpx () ;
    int rowStart = block0Doc->nsln () ;
    int size ;

    uint16* data;
    Gvar::LineDoc* lineDoc;

    if(header->blockId () == 1) {
      // This block records data from two channels: ch4 and ch5. There are
      // total 4 linedocs, the first two for ch4 and the other two for ch5.

      int lineDocNo = 0 ;

      // handle the data for ch4
      for(lineDocNo=0; lineDocNo<2; lineDocNo++) {
	data = block1or2->getData(lineDocNo) ;
	lineDoc = block1or2->getLineDoc(lineDocNo) ;

	size = block1or2->getDataLen (lineDocNo) ;

	if(lineDoc->licha() == 4) {
	  if ((lineDoc->lidet() == 1) || (lineDoc->lidet() == 2)) {
	    writeRow (frameId, lineDoc->licha()-1, 
		      columnStart, rowStart+lineDocNo,
		      data, size) ;
	  }
	}//if
      }//for

      // handle data for ch5
      for(lineDocNo = 2; lineDocNo < 4; lineDocNo++) {
	data = block1or2->getData(lineDocNo);
	lineDoc = block1or2->getLineDoc(lineDocNo);

	size = block1or2->getDataLen(lineDocNo) ;

	if(lineDoc->licha() == 5) {
	  if((lineDoc->lidet() == 3) || (lineDoc->lidet() == 4)) {
	    writeRow (frameId, lineDoc->licha ()-1, 
		      columnStart, rowStart+lineDocNo,
		      data, size) ;
	  }
	}//if
      }//for
    }//else if

    else if(header->blockId () == 2) {
      // This block contains data from two channels: ch2 and ch3. There are total
      // three linedocs, the first two for ch2 and the last one for ch3.

      // handle data for ch2
      int lineDocNo ;
      for(lineDocNo = 0; lineDocNo < 2; lineDocNo++) {
	data = block1or2->getData(lineDocNo);
	lineDoc = block1or2->getLineDoc(lineDocNo) ;

	size = block1or2->getDataLen (lineDocNo) ;

	if(lineDoc->licha() == 2) {
	  if((lineDoc->lidet() == 5) || (lineDoc->lidet() == 6)) {
            writeRow (frameId, lineDoc->licha ()-1,
                      columnStart, rowStart+lineDocNo,
                      data, size) ;
	  }
	}
      }//for

      // handle data for ch3
      lineDocNo = 2 ;
      data = block1or2->getData(lineDocNo);
      lineDoc = block1or2->getLineDoc(lineDocNo) ;

      size = block1or2->getDataLen (lineDocNo) ;

      if(lineDoc->licha() == 3) {
	if(lineDoc->lidet() == 7) {
	  writeRow (frameId, lineDoc->licha ()-1,
		    columnStart, rowStart+lineDocNo,
		    data, size) ;
 	}
      }//if
    }//else if
    
  }


  void GvarStream::convertBlock3to10ToRows (Gvar::Block0* block0,
					   Gvar::Block3to10 *block3to10) {
    Gvar::Block0Doc* block0Doc = block0->getBlock0Doc () ;

    int frameId = block0Doc->frame () ;
    int columnStart = block0Doc->wpx () ;
    int rowStart = block0Doc->nsln () ;
    int size ;

    uint16* data;
    Gvar::LineDoc* lineDoc;
  
    data = block3to10->getData ();
    lineDoc = block3to10->getLineDoc () ;

    size = block3to10->getDataLen() ;

    int blockId = ((block3to10->getBlock())->getHeader())->blockId () ;

    if(lineDoc->licha() == 1) {
      if((lineDoc->lidet() >= 1) && (lineDoc->lidet() <= 8)) {
	writeRow (frameId, lineDoc->licha ()-1,
		  columnStart, rowStart+(blockId-3),
		  data, size) ;
      }
    }//if
  }

  void GvarStream::writeRow (int frameId, int channelNo, int x, int y,
			     uint16* data, int size) {

//     if (m_rowWriters[channelNo] != NULL) {
//       m_rowWriters[channelNo]->write (row) ;
//     }

    if (size >= 21000) {
      fprintf (stderr, "ERROR: Write a row with size of %d.\n", size) ; fflush (stderr) ;
      size = 21000 ;
    }

    if (m_allRowsWriter != NULL) {
      // debug ("Write a row: frameId=%d, channelNo=%d, x=%d, y=%d, size=%d\n", 
      //     frameId, channelNo, x, y, size) ;
      Row *row = new Row (frameId, channelNo, x, y, size, data, true) ;

      m_allRowsWriter->write (row) ;
    }

    if (m_shmRowsWriter != NULL) {
      // debug ("Write a row: frameId=%d, channelNo=%d, x=%d, y=%d, size=%d\n",
      //     frameId, channelNo, x, y, size) ;
 
      RowMax* rowMax = new RowMax (frameId, channelNo, x, y, size, data) ;

      m_shmRowsWriter->write (rowMax) ;
    }
  }

  void GvarStream::readEventHandler (int fd,short event, void *arg) {
    GvarStream *g;
    g = (GvarStream *)arg;
    g->read();
  }
}

