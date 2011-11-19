#ifndef GEOSTREAM_GEOSTREAMSERVER_H
#define GEOSTREAM_GEOSTREAMSERVER_H

#include "GeostreamServerConfig.h"
#include "GvarStream.h"
#include <list>

#include "lib/List_MT.h"
#include "lib/Socket.h"
#include "lib/Exception.h"
#include "Fifo.h"
#include "gvar/Gvar.h"

/**
 * Define the structure for a geostream server.
 *
 * @author jiezhang - created on Mar 21, 2005
 */

namespace Geostream {

  class Connection ;
  // template class Geostream::Lib::List_MT<Connection> ;

  class GeostreamServer : private Geostream::Lib::Socket {
  public:
    GeostreamServer (int port, unsigned int numOfClients, 
		     MultiType reqStyle, MultiType writeStyle, int lineDocSize) ;
    virtual ~GeostreamServer () ;

    void init () throw (Geostream::Lib::GeostreamServerException) ;
    void start () throw (Geostream::Lib::GeostreamServerException) ;
    int accept () throw (Geostream::Lib::GeostreamServerException) ;

    static void newConnection (int sock, short event, void* arg) ;
    static void timeKeeper (int sock, short event, void *arg) ;
    void freeConnection (Connection* conn) ;

    /**
     * The server info.
     */
    char* m_host ;
    int m_port ;
    char* m_hostname ;

    unsigned int m_numOfConns ;
    MultiType m_reqStyle ;
    MultiType m_writeStyle ;

    struct event *m_connEvent ;
    struct event *m_timerEvent ;

/*     // list of free connections */
/*	Geostream::Lib::List_MT<Connection> m_freeConns ; */
/*     // lits of busy connections */
/*     Geostream::Lib::List_MT<Connection> m_busyConns ; */
    std::list<Connection*> m_freeConns ;
    std::list<Connection*> m_busyConns ;

    char m_timeStr[TIME_STR_LEN] ;

    /**
     * The GVAR connection info.
     */
	GvarStream *m_gvarStream;
	char *m_gvar;
    int m_gvarPort ;
    char* m_gvarName ;

	FifoReader<Gvar::LineDoc> *newLineDocReader();
	

  private:
	Fifo<Gvar::LineDoc> *m_fifoLineDoc;
	int m_fifoLineDocSize;

  } ;

}

#endif
