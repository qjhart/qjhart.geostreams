/*
  geostreamd

  I took list code from 'Pthreads Programming'

  $Id: rwlock.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: rwlock.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include <pthread.h>
#include "rwlock.h"

int 
pthread_rdwr_init_np(pthread_rdwr_t *rdwrp, pthread_rdwrattr_t *attrp)
{
  rdwrp->readers_reading=0;
  rdwrp->writers_writing=0;
  pthread_mutex_init(&rdwrp->mutex,NULL);
  pthread_cond_init(&rdwrp->lock_free,NULL);
  return 0;
}
int 
pthread_rdwr_rlock_np(pthread_rdwr_t *rdwrp)
{
  pthread_mutex_lock(&rdwrp->mutex);
  while(rdwrp->writers_writing) {
    pthread_cond_wait(&rdwrp->lock_free,&rdwrp->mutex);
  }
  rdwrp->readers_reading++;
  pthread_mutex_unlock(&rdwrp->mutex);
  return 0;
}
int 
pthread_rdwr_wlock_np(pthread_rdwr_t *rdwrp)
{
  pthread_mutex_lock(&rdwrp->mutex);
  while(rdwrp->writers_writing || rdwrp->readers_reading) {
    pthread_cond_wait(&rdwrp->lock_free,&rdwrp->mutex);
  }
  rdwrp->writers_writing++;
  pthread_mutex_unlock(&rdwrp->mutex);
  return 0;
  
}
int
pthread_rdwr_runlock_np(pthread_rdwr_t *rdwrp)
{
  pthread_mutex_lock(&rdwrp->mutex);
  if (rdwrp->readers_reading==0) 
    {
      pthread_mutex_unlock(&rdwrp->mutex);
      return -1;
    }
  else
    {
      rdwrp->readers_reading--;
      if (rdwrp->readers_reading==0)
	pthread_cond_signal(&rdwrp->lock_free);
      pthread_mutex_unlock(&rdwrp->mutex);
      return 0;
  }
}  
int
pthread_rdwr_wunlock_np(pthread_rdwr_t *rdwrp)
{
  pthread_mutex_lock(&rdwrp->mutex);
  if (rdwrp->writers_writing==0) 
    {
      pthread_mutex_unlock(&rdwrp->mutex);
      return -1;
    }
  else
    {
      rdwrp->writers_writing=0;
      pthread_cond_signal(&rdwrp->lock_free);
      pthread_mutex_unlock(&rdwrp->mutex);
      return 0;
  }
}
