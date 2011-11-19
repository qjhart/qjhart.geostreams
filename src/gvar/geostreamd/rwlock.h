/*
  geostreamd

  I took list code from 'Pthreads Programming'

  $Id: rwlock.h,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: rwlock.h,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include <pthread.h>

typedef struct rdwr_var {
  int readers_reading;
  int writers_writing;
  pthread_mutex_t mutex;
  pthread_cond_t lock_free;
} pthread_rdwr_t;
typedef void *pthread_rdwrattr_t;
#define pthread_rdwrattr_default NULL
int pthread_rdwr_init_np(pthread_rdwr_t *rdwrp, pthread_rdwrattr_t *attrp);
int pthread_rdwr_rlock_np(pthread_rdwr_t *rdwrp);
int pthread_rdwr_wlock_np(pthread_rdwr_t *rdwrp);
int pthread_rdwr_runlock_np(pthread_rdwr_t *rdwrp);
int pthread_rdwr_wunlock_np(pthread_rdwr_t *rdwrp);
