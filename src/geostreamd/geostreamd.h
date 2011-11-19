/*
  geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: geostreamd.h,v 1.3 2005/04/06 23:43:20 jiezhang Exp $

  $Log: geostreamd.h,v $
  Revision 1.3  2005/04/06 23:43:20  jiezhang
  Remove the old HTTP server C code and check in the HTTP server C++ code.

  Revision 1.2  2005/03/06 10:29:25  qjhart
  New Make Style

  Revision 1.1  2004/11/30 19:01:15  qjhart
  New server


*/
#include "GeostreamServer.h"

// extern int DEFAULT_LOG_FACILITY ;

extern Geostream::Log *gLog ;

struct gs_config {
  int facility;
  char *conf_file;
  Geostream::GeostreamServer *def;
};

/* extern time_t Time_Cur; */
/* extern char Time_Str[TIME_STR_LEN]; */
extern char Debug ;
extern int SeqNum; 

/* config.c */
int config_file(struct gs_config *config);
void config_print(struct gs_config *config);
