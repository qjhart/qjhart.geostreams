#ifndef GEOSTREAM_LIB_LIST_MT_H
#define GEOSTREAM_LIB_LIST_MT_H

#include <list>
#include <pthread.h>

#include "RWLock.h"

/**
 * Define a thread-safe double linked list.
 *
 * @author jiezhang - created on Mar 21, 2004
 */
namespace Geostream {
  namespace Lib {

    class RWLock ;

    template <typename DATATYPE>
    class List_MT {
    public:
      List_MT () ;
      ~List_MT () ;

      bool empty () ;
      void push_back (DATATYPE* data) ;
      DATATYPE* pop_front () ;
      DATATYPE& front () ;
      void remove (DATATYPE* data) ;
      int size () ;

    private:
      std::list<DATATYPE*> listMT ;
      RWLock* rwLock ;
    } ;

#include "List_MT.cpp"
  }
}

#endif
