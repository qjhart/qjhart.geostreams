/*
Look for "Coordinating Activities with Semaphores"
http://dis.cs.umass.edu/~wagner/threads_html/sect4.html

A Fifo is an object that can have one writer, and any number of
readers.  The main role of the fifo is to keep track of the data
between the readers and writers, and also to monitor the threads that
are being used by the readers, and to stop any readers that have
caught up to the writer.  The fifo can be either fixed or varying
size.  For fixed fifos, readers that are too slow can be
invalidated, or the writer can be stopped, based on the calling
parameter. 

Fixed size fifos use a circular array.  The writer knows when it has
overtaken a reader when the reader's semaphore count exceeds the
length of the fifo.  Here, the fifo can invalidate the reader, or
stop the writer thread.

 0 1 2 3 4 5 6 7 8 9 a b c d e f g
| | | | | | | | | | | | | | | | | | 
   ^         ^           ^
   |         |           | Writer
   |         Reader[0]      
    Reader[1]

Varying fifos can use a linked list.  They should maintain a pool of
list elements, so that there's not too much overhead in allocaing list
elements.  



It would be nice to include the ability to communicate via callbacks
to the reader functions as well.

Still unclear how to manage the tuples in the system, In terms of
readers removing the tuples.  Can have either a StorageManager, or
have Tuples, that count their usage.

The idea of the Fifo, is that there is a FifoPointer Class for
each of the readers.  The count in this class is both a count of the
number of available fifos, and also acts as a semaphore, blocking
reads when the fifo is empty.

*/

#ifndef __GEOSTREAM_FIFO_H
#define __GEOSTREAM_FIFO_H

#include <set>
#include <semaphore.h>
#include <pthread.h>

namespace Geostream {

  template <class X> class Fifo;
  template <class X> class FifoPointer;
  template <class X> class FifoReader;
  template <class X> class FifoWriter;

  template <class X> class FifoPointer {        
  public:
        Fifo<X> *fifo;
        //virtual ~FifoPointer();
  } ;

  template <class X> class FifoWriter: FifoPointer<X> {
  private:
        int write_p;
  public:
        // FifoWriter (Fifo<X> *fifo, int write_p);
        // void write ( X *data);
        

        FifoWriter (Fifo<X> *fifo, int write_p) {
          this->fifo = fifo ;
          this->write_p = write_p ;
        }

        void write( X *data) {
          std::set <FifoReader<X> *>* r;
          r = &(this->fifo->readers);
          pthread_mutex_lock(&this->fifo->mutex);

          if (this->fifo->fifo[this->write_p] != 0) {
            delete this->fifo->fifo[this->write_p] ;
          }

          this->fifo->fifo[this->write_p] = data;
          this->write_p = (this->write_p+1) % this->fifo->size;
          pthread_mutex_unlock(&this->fifo->mutex);

          for(typename std::set<FifoReader<X>* >::iterator i=r->begin(); 
                  i != r->end();i++ ) {
                sem_post(&((*i)->count));
/* 		if (((*i)->count).__sem_value >= this->fifo->size) { */
/*                   std::cout << "delete a reader...\n"; std::cout.flush () ; */
/*                   delete *i; */
/*                 } */
          }
        }

        friend class Fifo<X> ;
  } ;

  template <class X> class FifoReader: public FifoPointer<X> {
  private:
        sem_t count;
        int fifo_p;                                /* Pointer into the Fifo */
        
  public:
        // FifoReader(Fifo<X>* fifo, int fifo_p);

        FifoReader(Fifo<X>* fifo, int fifo_p) {
          this->fifo = fifo ;
          this->fifo_p = fifo_p ;
          sem_init(&count,0,0);
        }

        X *read() {
          X *data;
          sem_wait(&(this->count)) ;
          data=this->fifo->fifo[this->fifo_p];
          this->fifo_p = (this->fifo_p + 1) % this->fifo->size;

          return data;
        }

        ~FifoReader() {
          this->fifo->removeReader(this);
        }
        
        friend class FifoWriter<X> ;
  } ;

  template <class X> class Fifo {
  private:
        FifoWriter<X> *writer ;
        std::set<FifoReader<X>*> readers;
        int size;
        X **fifo;                        /* This should be a std::vector? */
        pthread_mutex_t mutex;
        sem_t writerCreated ;

  public:
        Fifo(int num) {
          size = num;
          fifo = new X*[size] ;
          for (int dataNo=0; dataNo<size; dataNo++) {
            fifo[dataNo] = 0 ;
          }
          pthread_mutex_init(&mutex,NULL);
          sem_init (&writerCreated, 0, 0) ;

          writer = NULL ;
        }

        ~Fifo () {
          if (writer != NULL) {
            delete writer ;
          }

          for (typename std::set<FifoReader<X>*>::iterator itr=readers.begin();
               itr != readers.end (); itr++) {
            delete *itr ;
          }

          for (int no=0; no<size; no++) {
            delete fifo[no] ;
          }

          delete fifo ;
        }
        
        FifoWriter<X>* newWriter () {
          std::cout << "Creating a new writer...\n" ; std::cout.flush () ;
          writer = new FifoWriter<X>(this, 0) ;
          sem_post (&writerCreated) ;
          std::cout << "Done creating a new writer...\n" ; std::cout.flush () ;
          return writer ;
        }
        
        FifoReader<X>* newReader() {
          std::cout << "Creating a new reader...\n" ; std::cout.flush () ;
          sem_wait (&writerCreated) ;
          FifoReader<X>* reader = new FifoReader<X> (this, writer->write_p) ;
          readers.insert (reader);
          sem_post (&writerCreated) ;
          std::cout << "Done creating a new reader...\n" ; std::cout.flush () ;
          return reader;
        }
        
        void removeReader(FifoReader<X>* reader) {
          readers.erase (reader);
        }

        friend class FifoWriter<X> ;
        friend class FifoReader<X> ;
  } ;
}
#endif
