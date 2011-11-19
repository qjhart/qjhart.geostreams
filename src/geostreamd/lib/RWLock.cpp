#include "RWLock.h"

using namespace Geostream::Lib ;

RWLock::RWLock () {
  numOfReaders = 0 ;
  numOfWriters = 0 ;
  pthread_mutex_init (&mutex, NULL) ;
  pthread_cond_init (&rwlock, NULL) ;
}

RWLock::~RWLock () {
}

void RWLock::rlock () 
  throw (LockException) {
  lock () ;

  while (numOfWriters > 0) {
    pthread_cond_wait (&rwlock, &mutex) ;
  }
  numOfReaders ++ ;

  unlock () ;
}

void RWLock::runlock ()
  throw (LockException) {
  lock () ;

  if (numOfReaders > 0) {
    numOfReaders -- ;

    if (numOfReaders == 0) {
      pthread_cond_signal (&rwlock) ;
    }
  }

  unlock () ;
}

void RWLock::wlock ()
  throw (LockException) {
  lock () ;

  while (numOfWriters > 0 || numOfReaders > 0) {
    pthread_cond_wait (&rwlock, &mutex) ;
  }

  numOfWriters ++ ;

  unlock () ;
}

void RWLock::wunlock ()
  throw (LockException) {
  lock () ;

  if (numOfWriters > 0) {
    numOfWriters -- ;
    pthread_cond_signal (&rwlock) ;
  }

  unlock () ;
}

void RWLock::lock ()
  throw (LockException) {
  int returnCode = pthread_mutex_lock (&mutex) ;
  if (returnCode != 0) {
    std::string errMsg = "Can not lock the given mutex 'mutex'." ;
    throw LockException (errMsg) ;
  }
}

void RWLock::unlock () 
  throw (LockException) {
  int returnCode = pthread_mutex_unlock (&mutex) ;
  if (returnCode != 0) {
    std::string errMsg = "Can not unlock the given mutex 'mutex'." ;
    throw LockException (errMsg) ;
  }
}

