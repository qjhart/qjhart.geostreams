/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: ll.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: ll.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "geostreamd.h"
#include "rwlock.h"

struct ll
{
  struct ll_item *head;
  struct ll_item *tail;
  pthread_rdwr_t rwlock;
};

/* All items need to start with prev,next */
struct ll_item {
  struct ll_item *prev;
  struct ll_item *next;
};

struct ll *
ll_new ()
{
  struct ll *l;
  l = malloc (sizeof *l);
  if (l)
    {
      pthread_rdwr_init_np(&l->rwlock,NULL);
      l->head = (void *) NULL;
      l->tail = (void *) NULL;
    }
  return l;
}

inline void *
ll_head(struct ll *l)
{
  return l->head;
}

inline void *
ll_tail(struct ll *l)
{
  return l->tail;
}

int
ll_empty(struct ll *l)
{
  return (l->head==NULL);
}

void *
ll_shift (struct ll *l)
{
  struct ll_item *h;

  pthread_rdwr_wlock_np(&l->rwlock);
  h=l->head;
  if (h != NULL) 
    {
      h->prev=NULL;
      l->head=h->next;
      if (l->head != NULL)
	l->head->prev=NULL;
      h->next=NULL;
      if (l->tail==h)
	l->tail=NULL;
    }
  pthread_rdwr_wunlock_np(&l->rwlock);
  return h;
}

void
ll_unshift (struct ll *l, struct ll_item *c)
{
  struct ll_item *t;

  pthread_rdwr_wlock_np(&l->rwlock);
  t = l->head;
  c->next = t;
  c->prev = 0;
  if (t == (void *) NULL)
    l->tail = c;
  else
    t->prev = c;
  l->head = c;
  pthread_rdwr_wunlock_np(&l->rwlock);
}


void *
ll_pop(struct ll *l)
{
  struct ll_item *t;
  pthread_rdwr_wlock_np(&l->rwlock);
  t=l->tail;
  if (t != NULL)
    {
      l->tail=t->prev;
      t->prev=0;
      if (l->tail != NULL)
	l->tail->next=0;
      if (l->head==t) 
	l->head=0;
    }
  pthread_rdwr_wunlock_np(&l->rwlock);
  return t;
}  

void
ll_unlink (struct ll *l, struct ll_item *c)
{
  struct ll_item *p, *n;

  pthread_rdwr_wlock_np(&l->rwlock);
  p = c->prev;
  n = c->next;
  if (p)
    p->next = n;
  if (n)
    n->prev = p;
  c->prev = 0;
  c->next = 0;
  if (c == l->head)
    l->head = n;
  if (c == l->tail)
    l->tail = p;
  pthread_rdwr_wunlock_np(&l->rwlock);
}

void
ll_link (struct ll *l, struct ll_item *c)
{
  struct ll_item *t;

  pthread_rdwr_wlock_np(&l->rwlock);
  t = l->tail;
  c->prev = t;
  c->next = 0;
  if (t == (void *) NULL)
    l->head = c;
  else
    t->next = c;
  l->tail = c;
  pthread_rdwr_wunlock_np(&l->rwlock);
}

void
ll_tohead (struct ll *l, struct ll_item *c)
{
  struct ll_item *t;

  pthread_rdwr_wlock_np(&l->rwlock);
  t = l->head;
  if (c->prev != NULL) // Not already first ?
    {
      c->prev->next = c->next;
      if (c->next != NULL)
        c->next->prev = c->prev; // Item now removed
      t->prev = c;
      c->next = t;
      c->prev = 0;
      l->head = c; // Item now at head
    }
  pthread_rdwr_wunlock_np(&l->rwlock);
}
