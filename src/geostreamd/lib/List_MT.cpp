#include "List_MT.h"

using namespace Geostream::Lib ;

template <typename DATATYPE>
List_MT<DATATYPE>::List_MT () {
  rwLock = new RWLock () ;
}

template <typename DATATYPE>
List_MT<DATATYPE>::~List_MT () {
  delete rwLock ;

  for (typename std::list<DATATYPE*>::iterator itr=listMT.begin ();
       itr != listMT.end () ; itr++) {
    delete *itr ;
  }
}

template <typename DATATYPE>
bool List_MT<DATATYPE>::empty () {
  bool isEmpty = false ;

  rwLock->rlock () ;
  isEmpty = listMT.empty () ;
  rwLock->runlock () ;

  return isEmpty ;
}

template <typename DATATYPE>
void List_MT<DATATYPE>::push_back (DATATYPE* data) {
  rwLock->wlock () ;
  listMT.push_back (data) ;
  rwLock->wunlock () ;
}

template <typename DATATYPE>
DATATYPE* List_MT<DATATYPE>::pop_front () {
  DATATYPE* data = NULL ;

  rwLock->wlock () ;
  data = listMT.front () ;
  listMT.pop_front () ;
  rwLock->wunlock () ;

  return data ;
}

template <typename DATATYPE>
DATATYPE& List_MT<DATATYPE>::front () {
  return listMT.front () ;
}

template <typename DATATYPE>
void List_MT<DATATYPE>::remove (DATATYPE* data) {
  rwLock->wlock () ;
  listMT.remove (data) ;
  rwLock->wunlock () ;
}

template <typename DATATYPE>
int List_MT<DATATYPE>::size () {
  int size ;
  rwLock->rlock () ;
  size = listMT.size () ;
  rwLock->runlock () ;

  return size ;
}
