#include "ShmRowFifo.h"

namespace Geostream {

  RowFifo* createRowFifo () {

    RowFifo* returnRowFifo ;

    int shmid ;
    int shmSize = sizeof (RowFifo) ;

    if ((shmid = shmget (SHM_KEY, shmSize, IPC_CREAT | 0666)) < 0) {
      fprintf (stderr, "can not create a shared mem seg: shmSize=%d, errno=%d\n", 
	       shmSize, errno); 
      if (errno == ENOSPC) {
        fprintf (stderr, "The requested shared mem seg is too large.\n"); fflush (stderr) ;
      }
      fflush (stderr) ;

      return NULL ;
    }

    if ((returnRowFifo = (RowFifo*)shmat (shmid, NULL, 0)) == (RowFifo*)-1) {
      fprintf (stderr, "can not attach the shared mem seg.\n"); 
      fflush (stderr) ;

//       struct shmid_ds buf ;
//       if (shmctl (shmid, IPC_RMID, &buf) < 0) {
// 	fprintf (stderr, "can not destroy the shared mem seg.\n"); 
// 	fflush (stderr) ;
//       }

      return NULL ;
    }

    returnRowFifo->writerPos = 0 ;

    // initialize some values
    for (int i=0; i<MAX_NUM_OF_READERS; i++) {
      returnRowFifo->isReaderSemsUsed[i] = false ;
      sem_init (&(returnRowFifo->readerSems[i]), 0, 0) ;
    }

    pthread_mutex_init (&returnRowFifo->mutex, NULL) ;
    pthread_mutex_init (&returnRowFifo->readerSemMutex, NULL) ;
    sem_init (&returnRowFifo->writerCreated, 0, 0) ;

    return returnRowFifo ;
  }

  RowFifo* retrieveRowFifo () {
    RowFifo* returnRowFifo ;

    int shmid ;
    int shmSize = sizeof (RowFifo) ;
 
    if ((shmid = shmget (SHM_KEY, shmSize, 0666)) < 0) {
      fprintf (stderr, "can not get a shared mem seg: shmSize=%d\n",
               shmSize);
      fflush (stderr) ;
      return NULL ;
    }

    if ((returnRowFifo = (RowFifo*)shmat (shmid, NULL, 0)) == (RowFifo*)-1) {
      fprintf (stderr, "can not attach the shared mem seg.\n");
      fflush (stderr) ;

//       struct shmid_ds buf ;
//       if (shmctl (shmid, IPC_RMID, &buf) < 0) {
//         fprintf (stderr, "can not destroy the shared mem seg.\n");
//         fflush (stderr) ;
//       }

      return NULL ;
    }

    return returnRowFifo ;
  }

  void writeFrameDef (RowFifo* rowFifo, Rectangle &rect, int frameId) {
    if (rowFifo == NULL) return ;

    rowFifo->rect.x = rect.x ;
    rowFifo->rect.y = rect.y ;
    rowFifo->rect.w = rect.w ;
    rowFifo->rect.h = rect.h ;

    rowFifo->frameId = frameId ;
  }

  FrameDef* getFrameDef (RowFifo* rowFifo) {
    FrameDef* frameDef = new FrameDef (rowFifo->frameId, rowFifo->rect) ;

    return frameDef ;
  }

  RowFifoWriter *newWriter (RowFifo* rowFifo) {
    if (rowFifo == NULL) return NULL ;

    RowFifoWriter* writer = 
      new RowFifoWriter (rowFifo, rowFifo->writerPos) ;
    sem_post (&(rowFifo->writerCreated)) ;

    fprintf (stderr, "start a new writer\n"); fflush (stderr) ;

    return writer ;
  }
        
  RowFifoReader *newReader (RowFifo* rowFifo) {
    if (rowFifo == NULL) return NULL ;

    sem_wait (&(rowFifo->writerCreated)) ;

    // try to find the first unused reader semaphore.
    // if there is no such a semaphore, then return NULL.
    int i ;

    pthread_mutex_lock (&(rowFifo->readerSemMutex)) ;
    for (i=0; i<MAX_NUM_OF_READERS; i++) {
      if (!(rowFifo->isReaderSemsUsed[i])) {
	break ;
      }
    }

    if (i<MAX_NUM_OF_READERS) {
      sem_init (&(rowFifo->readerSems[i]), 0, 0) ;
      rowFifo->isReaderSemsUsed[i] = true ;
    }
    pthread_mutex_unlock (&(rowFifo->readerSemMutex)) ;

    RowFifoReader *reader = NULL ;

    if (i < MAX_NUM_OF_READERS) {
      reader = new RowFifoReader
	(rowFifo, i, rowFifo->writerPos) ;
    }

    sem_post (&(rowFifo->writerCreated)) ;

    return reader ;
  }

  void removeReaderSem (RowFifo* fifo, int semPos) {
    pthread_mutex_lock (&(fifo->readerSemMutex)) ;
    fifo->isReaderSemsUsed[semPos] = false ;
    pthread_mutex_unlock (&(fifo->readerSemMutex)) ;

    fprintf (stderr, "Reset the reader semaphore: pos=%d.\n", semPos) ; 
    fflush (stderr) ;

    // detach the shared memory segment
    if (shmdt (fifo) == -1) {
      fprintf (stderr, "fail to detach the shared memory segment.") ; fflush (stderr) ;
    } else {
      fprintf (stderr, "detach the shared memory segment.") ; fflush (stderr) ;
    }
  }
}

