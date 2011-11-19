/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: cache.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: cache.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include <sys/mman.h>
#include <fcntl.h>
#include "geostreamd.h"

struct cache
{
  struct ll *free;
  struct ll *busy;
  int current_size;
  int maximum_size;
};


struct cfile *
cfile_new() 
{
  struct cfile *c;

  if ((c = malloc (sizeof *c)) == (void *) 0)
    {
      return NULL;
    }
  memset (c, 0, sizeof *c);
  return c;
}

struct cache *
cache_new(int maximum_size,int start_count) 
{
  struct cache *c;
  
  if ((c = malloc (sizeof *c)) == (void *) 0)
    {
      return NULL;
    }
  memset (c, 0, sizeof *c);
  if ((c->free = ll_new ()) == NULL)
    {
      free(c);
      return NULL;
    }
  if ((c->busy = ll_new ()) == NULL)
    {
      free(c->free);
      free(c);
      return NULL;
    }

  c->maximum_size=maximum_size;
  int i;
  struct cfile *f;
  for (i=0;i<start_count;i++) {
    f=cfile_new();
    ll_link(c->free,(struct ll_item *)f);
  }
  return c;
}

void 
cache_free_cfile(struct cache *c,struct cfile *f)
{
  if (f->mmap != NULL) 
    {
      munmap(f->mmap,f->finfo.st_size);
      c->current_size-=f->finfo.st_size;
    }
  ll_unlink(c->busy,(struct ll_item *)f);
  ll_link(c->free,(struct ll_item *)f);
}

struct cfile *
cache_get_cfile(struct cache *c) 
{
  struct cfile *f;
  if (((f=ll_shift(c->free))==NULL) && ((f=cfile_new())==NULL))
	return NULL;
  ll_unlink(c->free,(struct ll_item *)f); /* Okay if f isn't in c->free */
  ll_unshift(c->busy,(struct ll_item *)f);
  return f;
}

struct cfile *
cache_exisiting_file(struct cache *c,char *fn)
{
  struct cfile *f;
  int i = 0;
  f=ll_head(c->busy);
  while (f != NULL) 
    {
      i++;
      if (strcmp(f->fn,fn)==0)
      {
        debug("Found %s in the cache in position %d",fn,i);
        ll_tohead(c->busy,(struct ll_item *)f);
	return f;
      }
      f=f->next;
    }
  return NULL;
}

void
cache_file_done(struct cache *c,struct cfile *cf)
{
  if (cf)
    cf->count--;
}

struct cfile *
cache_file_get(struct cache *c,char *fn,
	       enum request_method m,response_code *rc)
{
  struct cfile *f;
  if ((f=cache_exisiting_file(c,fn))!=NULL)
    {
      *rc=RES_WORKING;
    }
  else
    {
      if ((f=cache_get_cfile(c))==NULL)
	{
	  *rc=500;
	  return NULL;
	}
      int fn_z = strlen (fn) + 128;
      if (f->fn_z < fn_z)
        {
          if ((f->fn = realloc (f->fn,fn_z)) == NULL)
	    {
	      *rc=500;
	      return NULL;
	    }
          f->fn_z = fn_z;
        }
      strcpy(f->fn,fn);
      f->fd = open(f->fn, O_RDONLY | O_NONBLOCK);
      debug ("open %s fd:%d (%s)", fn,f->fd,(f->fd==-1)?strerror (errno):"");
      if (f->fd == -1)
        {
	  cache_free_cfile(c,f);
          switch (errno)
            {
            case (EACCES):     /* Bad Permissions */
              *rc=403;
            case (ENOENT):     /* Doesn't Exist */
            case (ENOTDIR):    /* Path doesn't Exist */
            case (ENXIO):      /* FIFO? */
            case (EFAULT):     /* Not reachable */
              *rc=404;
            default:
              *rc=500;
            }
	  return NULL;
        }
      if (fstat (f->fd, &f->finfo) == -1)
        {
	  cache_free_cfile(c,f);
          close (f->fd);
          log_add (LOG_ERR, "fstat %s", strerror (errno));
          *rc=500;
	  return NULL;
        }
      if (!S_ISREG (f->finfo.st_mode))
	{
	  cache_free_cfile(c,f);
	  *rc=404;
	  return NULL;
	}
      if (m == HEAD)
        {
          close (f->fd);
        }
      else
	{
	  struct cfile *del;
	  /* Try and free up enough cache space */
	  while (c->current_size+f->finfo.st_size > c->maximum_size)
	    {
	      if((del=ll_pop(c->busy))==NULL)
		break;
	      if (del->count>0)
		{
		  ll_link(c->busy,(struct ll_item *)del);
		  break;
		}
	      else
		{
		  cache_free_cfile(c,del);
		  ll_link(c->free,(struct ll_item *)del);
		}
	    }
	  if (c->current_size+f->finfo.st_size < c->maximum_size) 
	    {
	      f->count++;
	      c->current_size+=f->finfo.st_size;
	      f->mmap=mmap(NULL,f->finfo.st_size,PROT_READ,MAP_SHARED,f->fd,0);
	      if (f->mmap == (void *)-1)
		{
		  debug("mmap %s",strerror(errno));
		  f->mmap=NULL;
		}
	      else 
		{
		  /* mlock(f->mmap,f->finfo.st_size); */
		  close(f->fd);
		  f->fd=-1;
		}
	    }
	  else
	    {
	      f->mmap=NULL;
	    }
	}
    }
  debug("Cache(fn=%s,mmap=%p,fd=%d) %d/%d",
	fn,f->mmap,f->fd,c->current_size,c->maximum_size);
  return f;
}

