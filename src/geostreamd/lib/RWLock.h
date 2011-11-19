#ifndef GEOSTREAM_LIB_RWLOCK_H
#define GEOSTREAM_LIB_RWLOCK_H

/**
 * Define a lock structure for read and write operations.
 *
 * @author jiezhang - created on Mar 21, 2005
 */

#include <pthread.h>

#include "Exception.h"

namespace Geostream {
  namespace Lib {

    typedef void* pthread_rdwrattr_t ;

    class LockException ;

    class RWLock {
    public:
      RWLock () ;
      ~RWLock () ;

      void rlock () throw (LockException) ;
      void runlock () throw (LockException) ;
      void wlock () throw (LockException) ;
      void wunlock () throw (LockException) ;

      // const void* pthread_rdwrattr_default = NULL ;

    private:
      int numOfReaders ;
      int numOfWriters ;
      pthread_mutex_t mutex;
      pthread_cond_t rwlock ;

      void lock () throw (LockException) ;
      void unlock () throw (LockException) ;
     
    } ;
  }
}


#endif
