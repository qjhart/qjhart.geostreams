// $Id: gvnetapi.cpp,v 1.9 2004/06/11 19:26:23 jiezhang Exp $

#define LINUX

#ifdef OS2
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef LINUX
#include <netinet/tcp.h>
#endif

/* the header for prototypes and errors */
#include "gvnetapi.h"
#include "gvarHeader/gvarHeader.h"
#include "gvarBlock/gvarBlock.h"
#include "lineDoc/lineDoc.h"
#include "block1to10/block1to10.h"
#include "block0/block0.h"
#include "block0/block0Doc.h"
// #include "utils/options/options.h"
// #include "analysis/pointset.h"
#include "image/imageatt.h"
#include "image/image_listofimages.h"
#include "mngstream/MngStream.h"

#include "mngstream/MngStream.h"

#include <iostream>
using std::iostream ;
using std::cout ;

#ifndef OS2
#define min(a,b) ((a) <= (b) ? (a) : (b))
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif



static int s;
static char buffer[GVNETBUFFSIZE];
static unsigned int expseqnum;

// Options Opt ;

/* the function to open the API and connection to the server */
/* you must give it the IP address of the server as a parameter ie "10.22.9.44" */
/* it will return zero if the connection was successfully established */
int gvarnet_open(char *ipaddr)

{
	struct sockaddr_in servername;
	int optval;

	s=socket(PF_INET,SOCK_STREAM,0);
	if (s==-1) return(E_SOCKET);
	memset(&servername,0,sizeof(servername));
#ifdef OS2
	servername.sin_len=sizeof(servername);
#endif
	servername.sin_family=AF_INET;
	servername.sin_addr.s_addr=inet_addr(ipaddr);
	servername.sin_port=htons(21009);
	if (connect(s,(struct sockaddr *)&servername,sizeof(servername))==-1) return(E_CONNECT);
	optval=1;
	// SCT added 8/4/03
	setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
	// non-portable options that are required to speed up the keep-alive
	// process by default it will wait 2hrs! after idle before the fist keep alive
#ifdef LINUX
	// since this link must be over a fast network these values are justified
	// sets all three default parameters so nothing is left to some riduculous default
	optval=30;  // let it be idle for 30 seconds before sending a keep alive
	setsockopt(s,SOL_TCP,TCP_KEEPIDLE,&optval,sizeof(optval));
	optval=2;   // go for two retries before error out
	setsockopt(s,SOL_TCP,TCP_KEEPCNT,&optval,sizeof(optval));
	optval=15;  // fifteen seconds between retries
	setsockopt(s,SOL_TCP,TCP_KEEPINTVL,&optval,sizeof(optval));
#endif
	expseqnum=1;
	return(0);
}

/* closes the connection */
int gvarnet_close()

{
	int rc;

#ifdef OS2
	rc=soclose(s);
#else
	rc=close(s);
#endif
	return((rc==-1) ? E_CLOSE:0);
}

/* called to read a complete GOES GVAR block, the caller must provide a sufficently large buffer */
/* 32K is recommended.  The buffer its size, and a pointer to an integer that upon succesful return */
/* will be filled in with the blocks size must be provided, the function will return zero if a */
/* GVAR block was sucessfully read */
int gvarnet_read(char *pbuff,int buffsize,int *pnumread)

{
	static char startpatt[8]={0xAA,0xD6,0x3E,0x69,0x02,0x5A,0x7F,0x55};
	static char endpatt[8]={0x55,0x7F,0x5A,0x02,0x69,0x3E,0xD6,0xAA};
	int bp,datalen,numread;
	unsigned int seqnum;

	datalen=16;
	bp=0;
	while(1) {
		// 8/4/03 added the no signal flag below
		numread=recv(s,buffer+bp,datalen-bp,MSG_NOSIGNAL);
		if (numread<0) return(E_READ);
		if (numread==0) return(N_CLOSED);
		bp+=numread;
		if (bp==16) {
			if (memcmp(buffer,startpatt,8)) return(E_BAD_START);
			datalen=ntohl(*((int*)(buffer+8)))+24;
			seqnum=ntohl(*((int*)(buffer+12)));
			if (seqnum!=expseqnum) return(E_BAD_SEQNUM);
			if (datalen>GVNETBUFFSIZE) return(E_BAD_SIZE);
			expseqnum++;
		}
		if (bp==datalen) {
			if (memcmp(buffer+datalen-8,endpatt,8)) return(E_BAD_END);
			memcpy(pbuff,buffer+16,min(datalen-24,buffsize));
			*pnumread=min(datalen-24,buffsize);
			return(0);
		}
	}
}

// create an image.
ImageAtT<int, int, uint16> *createImage (Block0 *block0, Block1to10 *block1to10) {
  Block0Doc* block0Doc = block0->getBlock0Doc () ;

  int frameId = block0Doc->frame () ;
  int row = block0Doc->nsln() + ((block1to10->getBlock())->getHeader ())->blockId () - 3 ;
  int columnStart = block0Doc->wpx() ;
  // std::cout << "row=" << row <<", colStart=" << columnStart << "\n" ;
  int size = block1to10->getDataLen () ;

  list<uint16>* data = new list<uint16>() ;
  for (int i=0; i<size; i++) {
    data->push_back ((block1to10->getData ())[i]) ;
  }

  // ImageAtT<int, int, uint16>* image =
  ImageAtT<int, int, uint16> *image = new ImageAtT<int, int, uint16>
   (frameId, columnStart, row, (columnStart+size-1), row, data) ;

  // image->print () ;
  return image ;
}


/* An Test/Example main that simply reads the blocks and prints thier size */
//#ifdef TESTMAIN
int main(int argc,char *argv[])

{
  int i,rc,numread;
  char *blkbuff;

  if (argc<4) {
    cerr << "./gvnetapi <IP_addr> mngfilename maxRows\n" ;
     return 0 ;
  }
  cerr << "GVAR API Test Program\n" ;

  Block0 * block0 = NULL ;
  GvarBlock* block = NULL ;
  GvarHeader* header = NULL ;
  Block1to10 *block1to10 = NULL ;

  // create an Image_listOfImages instance
  Image_ListofImages<int, int, uint16> *imageList = 
    new Image_ListofImages<int, int, uint16> () ;

  int numOfRowSoFar = 0 ;
  int oldFrameId = -1 ;

  int loopNo = 0 ;

  char *mngfilename = argv[2] ;
  int maxRowNo = atoi(argv[3]) ;

  MngStream *mngStream = new MngStream (mngfilename, 30680, 15787) ;

  if ((rc=gvarnet_open(argv[1]))==0) {
     blkbuff=(char*)malloc(GVNETBUFFSIZE);
     for (i=0;;i++) {
        rc=gvarnet_read(blkbuff,GVNETBUFFSIZE,&numread);
        if (rc!=0) {
          cerr << "Error Reading Blocks RC=" << rc << "\n" ;
          return(1);
        }
        cerr << "Read Block " << i << " Size " << numread << "\n" ;

        block = new GvarBlock (blkbuff, numread) ;

        header = block->getHeader () ;

        if (header->blockId () == 0) {

          if (block0 != NULL) {
            delete block0 ;
            block0 = NULL ;
          }
          block0 = new Block0 (block) ;
        }

        else if ((header->blockId ()>= 3) &&
            (header->blockId () <= 10)) {
          if (block1to10 != NULL) {
            delete block1to10 ;
            block1to10 = NULL ;
          }
          block1to10 = new Block1to10 (block) ;

          if (block0 != NULL) {

            ImageAtT<int, int, uint16>* image = createImage (block0, block1to10) ;
            uint16* line[1] = { image->getData () } ;

            int x = image->getLowX () ;
            int y = image->getLowY () ;
            int size = image->getDataSize () ;

            mngStream->addImage (x, y, line, size, 1) ;
            delete[] line[0] ;
            delete image ;
            // delete block ;

            loopNo ++ ;

            if (loopNo > maxRowNo) {
               break ;
            }

          }
        } 

        // other blocks
        else {
          delete block ;
        }

        //delete block ;
     }
     free(blkbuff);

     mngStream->close () ;
     delete mngStream ;
     gvarnet_close();
  } else {
     cerr << "Error opening or connecting to socket RC=" << rc << "\n" ;
     return(1);
  }
  return(0);
}
//#endif

/* end of GVAR Net API */


