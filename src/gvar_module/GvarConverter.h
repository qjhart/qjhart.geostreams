/**
 * Define a structure to convert a gvar stream format into
 * several other formats, like binary format, ascii format,
 * grass format, and etc.
 *
 * @author jiezhang - created on July 05, 2005
 */
#include "GvarStream.h"
#include "ImgStream.h"
#include "ShmRowFifo.h"

namespace Geostream {

  class ClientRequest {
  public:
    bool isChannelRequested[NUM_OF_CHANNELS] ;
    Format format ;
    bool requestGetInfo ;

    // representing a box: x,y,w,h. When w=0 or h=0, it means that
    // the client did not set bbox.
    struct bboxStruct {
      int x ;
      int y ;
      int width ;
      int height ;
    } bbox ;

    bool nodata ;

    ClientRequest () {
      format = ASCII_FORMAT ;

      for (int channelNo=0; channelNo<NUM_OF_CHANNELS; channelNo++) {
	isChannelRequested[channelNo] = false ;
      }

      requestGetInfo = false ;

      bbox.width = bbox.height = 0 ;

      nodata = false ;
    }
  } ;

  class GvarConverter {

  public:
    /**
     * Constructor
     *
     * @param gvar - the gvar stream
     * @param rowFifo - the buffer to put all rows
     * @param request - the request from clients on how and what 
     *                  to convert.
     * @param rs - the reference space for the data
     * @param writer - a visitor to define how to write the output data
     *
     * Note: In gvar object, the allRowsWriter has to be set before
     * calling this constructor.
     */
    GvarConverter (GvarStream* gvar, Fifo<Row>* rowFifo, 
		   ClientRequest& request, ReferenceSpace* rs,
		   StreamWriter* writer) ;

    /**
     * Constructor
     *
     * @param shmRowFifo - the buffer to put all rows
     * @param request - the request from clients on how and what 
     *                  to convert.
     * @param rs - the reference space for the data
     * @param writer - a visitor to define how to write the output data
     *
     * Note: In gvar object, the allRowsWriter has to be set before
     * calling this constructor.
     */
    GvarConverter (Geostream::RowFifo* shmRowFifo, 
		   ClientRequest& request, ReferenceSpace* rs,
		   StreamWriter* writer) ;

    ~GvarConverter () ;

    void convert () ;

  private:

    // the gvar stream
    GvarStream* m_gvar ;

    // the buffer to store all rows. 
    Fifo<Row>* m_rowFifo ;

    // the shared memory buffer to store all rows.
    RowFifo* m_shmRowFifo ;

    // the client request
    ClientRequest &m_clientRequest ;

    // the reference space for the data
    ReferenceSpace* m_rs ;

    // output image stream
    ImgStream *m_imgStream ;

    // the row reader
    Geostream::FifoReader<Row> *m_rowReader ;

    // the row reader pointing to the shared memory
    Geostream::RowFifoReader *m_shmRowReader ;

  } ;
}
