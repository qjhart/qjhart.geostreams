/**
 *
 * This is a hack of fifo buffer for Row objects. This buffer is
 * allocated in a shared memory segment.
 *
 * created by jiezhang - Jul 12, 2005
 *
 */

#ifndef __GEOSTREAM_SHMROWFIFO_H
#define __GEOSTREAM_SHMROWFIFO_H

#include <set>
#include <semaphore.h>
#include <pthread.h>

// shared memory
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "ImgStream.h"

#define MAX_NUM_OF_READERS 1000
#define MAX_NUM_OF_ITEMS 400
#define MAX_ROW_SIZE 21000

#define SHM_KEY 10001

extern int errno ;

namespace Geostream {

  class RowFifoPointer ;
  class RowFifoReader ;
  class RowFifoWriter ;

  class RowMax {
  public:
    /** Frame ID */
    int f;

    /** Channel ID */
    int c;

    /** x location */
    int x;

    /** y location */
    int y;

    /** real size of the row */
    int w ;

    /** pixel values */
    uint16 v [MAX_ROW_SIZE];

    RowMax (int frameId, int channelNo, int x, int y, 
	    int size, uint16* data) {
      f = frameId ;
      c = channelNo ;
      this->x = x ;
      this->y= y ;
      w = size ;
      memcpy (v, data, size*sizeof (uint16)) ;
    }
  } ;

  typedef struct RowFifoStr {
    // the latest frame
    Rectangle rect ;
    int frameId ;

    int writerPos ;
    bool isReaderSemsUsed[MAX_NUM_OF_READERS] ;
    sem_t readerSems[MAX_NUM_OF_READERS] ;
    RowMax fifo [MAX_NUM_OF_ITEMS];
    pthread_mutex_t mutex;
    pthread_mutex_t readerSemMutex ;
    sem_t writerCreated ;
  } RowFifo ;

  extern RowFifo* createRowFifo () ;
  extern RowFifo* retrieveRowFifo () ;
  extern void writeFrameDef (RowFifo* rowFifo, Rectangle &rect, int frameId) ;
  extern FrameDef* getFrameDef (RowFifo* rowFifo) ;
  extern RowFifoWriter *newWriter (RowFifo* rowFifo) ;
  extern RowFifoReader *newReader (RowFifo* rowFifo) ;
  extern void removeReaderSem (RowFifo* fifo, int semPos) ;

  class RowFifoPointer {        
  public:
    RowFifo *fifo;

    RowFifoPointer (RowFifo* fifo) :
      fifo (fifo) {
    }
  } ;

  class RowFifoWriter: public RowFifoPointer {
  public:

    RowFifoWriter (RowFifo *fifo, int& write_p) :
      RowFifoPointer (fifo), write_p (write_p) {
    }

    void write (RowMax *data) {

      pthread_mutex_lock (&(fifo->readerSemMutex)) ;

      pthread_mutex_lock (&(fifo->mutex));
      copyRow (&(fifo->fifo[fifo->writerPos]), data) ;
      fifo->writerPos = (fifo->writerPos+1) % MAX_NUM_OF_ITEMS ;

      for (int i=0; i<MAX_NUM_OF_READERS; i++) {
	if (fifo->isReaderSemsUsed[i]) {

	  // reset the slow readers
	  if ((fifo->readerSems[i]).__sem_value >= MAX_NUM_OF_ITEMS-1) { 
	    fprintf (stderr, "reset a slow client.\n"); fflush (stderr) ;
	    sem_init (&(fifo->readerSems[i]), 0, 0) ;
	  }

	  sem_post (&(fifo->readerSems[i])) ;
	}
      }

      pthread_mutex_unlock(&(fifo->mutex));

      pthread_mutex_unlock (&(fifo->readerSemMutex)) ;

      // delete this row
      delete data ;
    }

  private:
    int& write_p;

    void copyRow (RowMax* copyTo, RowMax* copyFrom) {
      copyTo->f = copyFrom->f ;
      copyTo->c = copyFrom->c ;
      copyTo->x = copyFrom->x ;
      copyTo->y = copyFrom->y ;
      copyTo->w = copyFrom->w ;

      memcpy (copyTo->v, copyFrom->v, copyTo->w*sizeof (copyTo->v[0])) ;
    }
  } ;

  class RowFifoReader: public RowFifoPointer {
  private:
    int semPos ;
    int fifo_p ;
        
  public:
        RowFifoReader (RowFifo* fifo, int semPos, int fifo_p):
	  RowFifoPointer (fifo), semPos (semPos), fifo_p (fifo_p) {
        }

        RowMax *read() {
          RowMax *data;
          sem_wait(&(fifo->readerSems[semPos])) ;

          data = &(fifo->fifo[fifo_p]) ;

          fifo_p = (fifo_p + 1) % MAX_NUM_OF_ITEMS ;

          return data;
        }

        ~RowFifoReader() {
           removeReaderSem (fifo, semPos);
        }
  } ;

}

#endif
