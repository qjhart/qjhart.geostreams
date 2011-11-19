#ifndef __GEOSTREAM_GVARSTREAM_H
#define __GEOSTREAM_GVARSTREAM_H

#include "LineDoc.h"
#include "Block.h"
#include "Fifo.h"
#include "ShmRowFifo.h"
#include "ImgStream.h"

#define GVNETBUFFSIZE 32768
#define NUM_OF_CHANNELS 5

namespace Geostream {

  static const char* channelsDesc[NUM_OF_CHANNELS] = {
    "Channel 1, 0.52-0.75microns - cloud cover",
    "Channel 2,   3.8-4.0microns - night cloud cover",
    "Channel 3,   6.4-7.0microns - water vapor",
    "Channel 4, 10.2-11.2microns - sea surface temp.",
    "Channel 5, 11.5-12.5microns - sea surface temp."
  } ;

  static const int xRes[NUM_OF_CHANNELS] = {1,4,4,4,4} ;
  static const int yRes[NUM_OF_CHANNELS] = {1,4,8,4,4} ;


  class GvarStream {
  private:
	unsigned int seqnum;
	struct event *event_p;
	char *ipaddr;
	int port;
	int fd;
	int numread;
	char *bp;
	char *end;
	char blkbuf[GVNETBUFFSIZE];

	FILE* blkFile;
	bool workingOnFile;

	FifoWriter<Gvar::LineDoc> *m_LineDocWriter ;
	FifoWriter<Row>* m_allRowsWriter ;
	FifoWriter<Row> *m_rowWriters[NUM_OF_CHANNELS] ;

	Geostream::RowFifoWriter* m_shmRowsWriter ;

	// the shared memory row fifo
	Geostream::RowFifo *m_shmRowFifo ;

	// store the last block0 we have seen
	Gvar::Block0 *m_block0 ;

	// store the last frame id we have seen
	int m_prevFrameId ;

	// store the last FrameDef we have seen
	FrameDef *m_prevFrameDef ;

	int connect(void);

	void convertBlockToRows (Gvar::Block *block) ;
	void convertBlock1or2ToRows (Gvar::Block0* block0,
				     Gvar::Block1or2 *block1or2) ;
	void convertBlock3to10ToRows (Gvar::Block0* block0,
				      Gvar::Block3to10 *block3to10) ;

	void writeRow (int frameId, int channelNo,
		       int x, int y,
		       uint16* data, int size) ;
	
        Gvar::Block* readBlockSocket () ;
        Gvar::Block* readBlockFile (); 

  public:
	GvarStream(char *ipaddr,int port);

        GvarStream(char* filename);
 
	~GvarStream();
	bool listen(void);
	bool read(void);
	static void readEventHandler (int fd,short event, void *arg) ;

	void close () ;

	FifoWriter<Gvar::LineDoc> *LineDocWriter
	  (FifoWriter<Gvar::LineDoc> *p) ;
	FifoWriter<Gvar::LineDoc> *LineDocWriter(void) ;

	FifoWriter<Row> *allRowsWriter (FifoWriter<Row> *w) ;
	FifoWriter<Row> *allRowsWriter () ;

	FifoWriter<Row> *rowWriter (FifoWriter<Row> *w, int channelNo) ;
	FifoWriter<Row> *rowWriter (int channelNo) ;

	Geostream::RowFifoWriter* shmRowsWriter (Geostream::RowFifoWriter* w) ;
	Geostream::RowFifoWriter* shmRowsWriter () ;

	Geostream::RowFifo* shmRowFifo (Geostream::RowFifo* fifo) ;
	Geostream::RowFifo* shmRowFifo () ;

	inline FrameDef* getPrevFrameDef () {
	  return m_prevFrameDef ;
	}

	void new_block(void) ;

        Gvar::Block* readBlock () ;

	// carueda 9/6/07
	Gvar::Block0 *getLastBlock0(void) {
		return m_block0;
	}
  };
}

#endif
