#ifndef __GEOSTREAM_GVARSTREAM_H
#define __GEOSTREAM_GVARSTREAM_H

#define GVNETBUFFSIZE 32768

#include "gvar/Gvar.h"
#include "Fifo.h"

namespace Geostream {
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

	int connect(void);
	static void readEventHandler (int fd,short event, void *arg) ;
	Geostream::FifoWriter<Gvar::LineDoc> *m_LineDocWriter;
	void new_block(void);
	
  public:
	GvarStream(char *ipaddr,int port);
	~GvarStream();
	FifoWriter<Gvar::LineDoc> *LineDocWriter(void);
	FifoWriter<Gvar::LineDoc> *LineDocWriter(FifoWriter<Gvar::LineDoc> *);
	int listen(void);
	void read(void);
  };
}

#endif
