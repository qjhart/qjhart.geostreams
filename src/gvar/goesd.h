/* 
   GOESD - Streaming socket MNG writer.

   Copyright (C) 2004 University of California, Davis

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

  $Id: goesd.h,v 1.1 2004/01/30 18:37:21 qjhart Exp $

  $Log: goesd.h,v $
  Revision 1.1  2004/01/30 18:37:21  qjhart
  Start at goesd


*/

#define USE_SYSLOG
#ifdef USE_SYSLOG 
#include <syslog.h>
#endif

/* maximum GVAR packet size is less than this */
#define GVNETBUFFSIZE 32768

/* goesd */
struct goesd {
  int gvar_fd;
  int port;
  int listen_fd;

  struct in_addr addr;
  unsigned int num_clients;
  struct pollfd *ufds;
};

/* Globals */
char Debug;
char Verbose;

