#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include "Fifo.h"

using std::cout;

static const char rcsid[] =
  "$Id: fifo_test.cpp,v 1.3 2005/04/13 22:29:09 qjhart Exp $" ;

void* writer_function (void* ptr) ;
void* reader_function (void *ptr) ;

struct reader_info {
  int readerId ;
  Geostream::FifoReader<int> * rp;
} ;

main() {
  int max=50;
  Geostream::Fifo<int> *fifo;
  cout << "FOO\n";
  fifo = new Geostream::Fifo<int>(max);

  pthread_t writer_thread ;
  Geostream::FifoWriter<int> *wp = fifo->newWriter();
  pthread_create(&writer_thread,NULL,&writer_function,(void*)wp);

  int i;
  pthread_t reader_thread[10] ;
  for (i=0;i<10;i++) {
    Geostream::FifoReader<int> *rp = fifo->newReader( ) ;
    struct reader_info *rinfo =
      (struct reader_info*)malloc (sizeof (struct reader_info*)) ;
    rinfo->readerId = i ;
    rinfo->rp = rp ;
    pthread_create(&reader_thread[i],NULL,&reader_function,(void*)rinfo) ;
  }

  void* writeReturn ;
  pthread_join (writer_thread, &writeReturn) ;
}

void* writer_function (void* ptr) {
  Geostream::FifoWriter<int> *wp = (Geostream::FifoWriter<int> *)ptr ;

  int counter = 0 ;
  while (1) {
    sleep (1) ;
    int* counterToWriter = new int ;
    *counterToWriter = counter ;
    wp->write (counterToWriter);
    counter = (++counter)%50 ;
  }
}

void* reader_function (void *ptr) {
  struct reader_info *rinfo = (struct reader_info *)ptr ;
  int readerId = rinfo->readerId ;
  Geostream::FifoReader<int> *rp = rinfo->rp ;
  while (1) {
    sleep (readerId) ;
    int* foo = rp->read();
    std::cout << "Reader " << readerId 
	      << " reads: " << *foo << "\n" ;
    std::cout.flush () ;
  }
}
