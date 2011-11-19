/*
  buffer - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: buffer.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: buffer.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "geostreamd.h"
#include <stdlib.h>
#include <unistd.h>

struct buffer
{
  char rewindable;
  char *scratch;
  ssize_t scratch_size;
  char *s;
  char *r;
  char *w;
  char *e;
};

#define BUFFER_SIZE(b) (b->e-b->s)
#define BUFFER_CANWRITE(b) ((b->w - b->r >= 0)?b->e - b->w : b->r - b->w - 1)
#define BUFFER_LEFT(b)     ((b->w - b->r >= 0)?(b->e - b->s) - (b->w - b->r) : b->r - b->w - 1)

#define BUFFER_CANREAD(b)  ((b->w - b->r >= 0)?b->w - b->r : b->e - b->r)

struct buffer *
buffer_malloc (size_t size)
{
  struct buffer *b;
  void *s;
  b = malloc (sizeof *b);
  if (b)
    {
      if ((s = malloc (size)) != NULL)
        {
          b->s = s;
          b->r = s;
          b->w = s;
          b->e = s + size;
          b->rewindable = 1;
          b->scratch = NULL;
          return b;
        }
      else
        free (b);
    }
  return NULL;
}

void
buffer_free (struct buffer *b)
{
  if (b->s)
    free (b->s);
  free (b);
}

void
buffer_reset (struct buffer *b)
{
  b->rewindable = 1;
  b->r = b->w = b->s;
}

inline char *
buffer_readp (struct buffer *b)
{
  return b->r;
}

inline int
buffer_empty (struct buffer *b)
{
  return (b->r == b->w);
}

/* Returns -1 if can't rewind */
int
buffer_rewind (struct buffer *b)
{
  if (b->rewindable)
    {
      b->r = b->s;
      return 0;
    }
  else
    return -1;
}

inline int
buffer_getc (struct buffer *b)
{
  char c = EOF;
  if (BUFFER_CANREAD (b) > 0)
    c = *b->r++;
  if (b->r > b->e)
    b->r = b->s;
  return c;
}

size_t
buffer_read_from_fd (struct buffer * b, int fd)
{
  ssize_t nr;
  nr = read (fd, b->w, BUFFER_CANWRITE (b));
  if (nr != -1)
    {
      b->w += nr;
      if (b->w == b->e)
        b->w = b->s;
    }
  return nr;
}

size_t
buffer_write_to_fd (struct buffer * b, int fd)
{
  ssize_t nw;
  if (BUFFER_CANREAD (b) == 0)
    return 0;                   /* Buffer is done */
  nw = write (fd, b->r, BUFFER_CANREAD (b));
  if (nw != -1)
    {
      b->r += nw;
      if (b->r == b->e)
        b->r = b->s;
    }
  return nw;
}

int
buffer_strcat (struct buffer *b, char *str)
{
  int n;
  for (n = 0; *str; n++)
    {
      if (BUFFER_LEFT (b) == 0)
        return -1;
      *b->w++ = *str++;
      if (b->w == b->e)
        b->w = b->s;
    }
  return n;
}

int
buffer_strcat_n (struct buffer *b, ...)
{
  int n;
  va_list ap;
  char *str;

  va_start (ap, b);
  /* Must end in null string */
  while ((str = va_arg (ap, char *)) != NULL)
    {
      for (n = 0; *str; n++)
        {
          if (BUFFER_LEFT (b) == 0)
            return -1;
          *b->w++ = *str++;
          if (b->w == b->e)
            b->w = b->s;
        }
    }
  return n;
}

int
buffer_printf (struct buffer *b, char *format, ...)
{
  int size;
  size = BUFFER_CANWRITE (b);
  int written;

  va_list ap;
  va_start (ap, format);
  written = vsnprintf (b->w, size, format, ap);
  if (written < 0)
    return written;

  else if (written > BUFFER_LEFT (b))
    {                           /* Expensive */
      if (b->scratch != NULL && (b->scratch_size < written))
        {
          free (b->scratch);
          b->scratch = NULL;
        }

      if (b->scratch == NULL)
        {
          if ((b->scratch = malloc (written)) == NULL)
            return -1;
        }

      written = vsnprintf (b->scratch, written, format, ap);
      if (written < 0)
        return -1;
      memcpy (b->s, b->scratch + size, (written - size));
      b->w = b->s + (written - size);
    }
  else
    b->w += written;
  return written;
}
