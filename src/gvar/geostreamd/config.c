/*
  config - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: config.c,v 1.1 2004/08/12 17:41:53 singhals Exp $

  $Log: config.c,v $
  Revision 1.1  2004/08/12 17:41:53  singhals
  check-in httpd server code.


*/
#include "dotconf.h"
#include "geostreamd.h"

static const char end_Server[] = "</Server>";

FUNC_ERRORHANDLER(errorhandler);
static DOTCONF_CB(cb_port);
static DOTCONF_CB(cb_num_clients);
static DOTCONF_CB(cb_document_root);
static DOTCONF_CB(cb_facility);
static DOTCONF_CB(cb_req_style);
static DOTCONF_CB(cb_write_style);
static DOTCONF_CB(cb_cache_size);
static DOTCONF_CB(cb_server_open);
static DOTCONF_CB(cb_server_close);
static DOTCONF_CB(cb_host);
static DOTCONF_CB(cb_files);

static const configoption_t options[] = {
	{"Port", ARG_INT, cb_port, NULL, CTX_ALL },
	{"MaxClients",ARG_INT,cb_num_clients,NULL,CTX_ALL},
	{"DocumentRoot",ARG_STR,cb_document_root,NULL,CTX_ALL},
	{"LocalFacility",ARG_INT,cb_facility,NULL,CTX_ALL},
	{"RequestStyle",ARG_STR,cb_req_style,NULL,CTX_ALL},
	{"WriteStyle",ARG_STR,cb_write_style,NULL,CTX_ALL},
	{"CacheSize",ARG_INT,cb_cache_size,NULL,CTX_ALL},
	{"<SERVER>",ARG_NONE,cb_server_open,NULL,CTX_ALL},
	{"Host",ARG_STR,cb_host,NULL,CTX_ALL},
	{"Files",ARG_STR,cb_files,NULL,CTX_ALL},
	{end_Server, ARG_NONE, cb_server_close, NULL,CTX_ALL},
	LAST_OPTION
};

FUNC_ERRORHANDLER(errorhandler)
{
  log_add(type, msg); /* type Matches SYSLOG */
  return (type==DCLOG_ERR)?1:0;
}

DOTCONF_CB(cb_server_open) 
{
  struct gs_config *config = (struct gs_config *)ctx;
  const char *err = 0;
  struct server *new_server;
  struct server *old_def=config->def;
  
  if ((new_server=server_new(config->def)) == (void *) 0)
    {
      return "Cannot Allocate new server";
    }
  if (config->servers==NULL)	/* First one */
    {
      config->servers=new_server;
      new_server->next=new_server;
      new_server->prev=new_server;
    }
  else 
    {
      config->servers->prev->next=new_server;
      new_server->prev=config->servers->prev;
      config->servers->prev=new_server;
      new_server->next=config->servers;      
    }
  
  /* Fake out the rest */
  config->def=new_server;

  while (!cmd->configfile->eof)
    {
      err = dotconf_command_loop_until_error(cmd->configfile);
      if (!err)
	{
	  err = "</Server> is missing";
	  break;
	}
      
      if (err == end_Server)
	break;
      
      dotconf_warning(cmd->configfile, DCLOG_ERR, 0, err);
    }

  config->def=old_def;  
  if (err != end_Server)
    return err;
  
  return NULL;
}

DOTCONF_CB(cb_server_close)
{
  return end_Server;
}

DOTCONF_CB(cb_host)
{
  char *cp;
  struct gs_config *config = (struct gs_config *)ctx;
  if ((config->def->host=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");
  cp = strchr(cmd->data.str,':');
  if (cp!=NULL)
    {
      *cp=0;
      config->def->port=atoi(cp+1);
    }
  if ((config->def->hostname=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");

  debug("(%p)%s: %s",config->def,cmd->name,cmd->data.str);
  return NULL;
}

DOTCONF_CB(cb_files)
{
  struct gs_config *config = (struct gs_config *)ctx;

  if ((config->def->files=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");
  debug("(%p)%s: %s",config->def,cmd->name,config->def->files);
  return NULL;
}

DOTCONF_CB(cb_port)
{
  struct gs_config *config = (struct gs_config *)ctx;
  int port=cmd->data.value;

  if (port<1000) {
    dotconf_warning(cmd->configfile,DCLOG_WARNING,ERR_USER,
		    "%s: %d is not allowed by user",cmd->name,port);
  } else {
    config->def->port=port;
    debug("(%p)%s: %d",config->def,cmd->name,port);
  }
  return NULL;
}

DOTCONF_CB(cb_num_clients)
{
  struct gs_config *config = (struct gs_config *)ctx;
  int num_clients=cmd->data.value;
  if (num_clients < 1) {
    dotconf_warning(cmd->configfile,DCLOG_WARNING,ERR_USER,
		    "%s: %d is invalid - Ignoring",cmd->name,cmd->data.value);
  }
  config->def->num_clients=num_clients;
  return NULL;
}

DOTCONF_CB(cb_document_root)
{
  struct gs_config *config = (struct gs_config *)ctx;
  char *root;
  if ((root=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");
  config->def->document_root=root;
  config->def->document_root_z=strlen (root);
  debug("(%p)%s: %s",config->def,cmd->name,config->def->document_root);
  return NULL;
}

DOTCONF_CB(cb_req_style)
{
  struct gs_config *config = (struct gs_config *)ctx;
  if (strcasecmp(cmd->data.str,"THREADS")==0)
    config->def->req_style=THREADS;
  else if (strcasecmp(cmd->data.str,"EVENTS")==0)
    config->def->req_style=EVENTS;
  else 
    return "Bad Request Style";
  return NULL;
}

DOTCONF_CB(cb_write_style)
{
  struct gs_config *config = (struct gs_config *)ctx;
  if (strcasecmp(cmd->data.str,"THREADS")==0)
    config->def->write_style=THREADS;
  else if (strcasecmp(cmd->data.str,"EVENTS")==0)
    config->def->write_style=EVENTS;
  else 
    return "Bad Write Style";
  return NULL;
}

DOTCONF_CB(cb_cache_size)
{
  struct gs_config *config = (struct gs_config *)ctx;
  int c=cmd->data.value;
  if (c < 0) {
    dotconf_warning(cmd->configfile,DCLOG_WARNING,ERR_USER,
		    "%s: %d is invalid - Ignoring",cmd->name,cmd->data.value);
    return "Invalid Cache Size";
  }
  config->def->cache_size=c;
  return NULL;
}

DOTCONF_CB(cb_facility)
{
  struct gs_config *config = (struct gs_config *)ctx;
  int f=cmd->data.value;
  if (f < 0 || f > 7) {
    dotconf_warning(cmd->configfile,DCLOG_WARNING,ERR_USER,
		    "%s: %d is invalid - Ignoring",cmd->name,cmd->data.value);
  }
  switch (f)
    {
    case (0):
      config->facility = LOG_LOCAL0;
      break;
    case (1):
      config->facility = LOG_LOCAL1;
      break;
    case (2):
      config->facility = LOG_LOCAL2;
      break;
    case (3):
      config->facility = LOG_LOCAL3;
      break;
    case (4):
      config->facility = LOG_LOCAL4;
      break;
    case (5):
      config->facility = LOG_LOCAL5;
      break;
    case (6):
      config->facility = LOG_LOCAL6;
      break;
    case (7):
      config->facility = LOG_LOCAL7;
      break;
    default:
      break;
    }
  return NULL;
}

int config_file(struct gs_config *config) {
  configfile_t *configfile;

  configfile = dotconf_create(config->conf_file,options,
			      (void *)config, CASE_INSENSITIVE);

  configfile->errorhandler = (dotconf_errorhandler_t) errorhandler;
  
  if (!configfile) {
    log_add(LOG_ERR,"Error opening config file");
    return 1;
  }
    
  if (dotconf_command_loop(configfile) == 0) {
    log_add(LOG_ERR,"Error reading config file");
    return 1;
  }
  dotconf_cleanup(configfile);
  return 0;
}

void config_print(struct gs_config *config)
{
  struct server *server_p;
  server_p= config->servers;
  do
    {
      printf("%s:%d %s\n",server_p->hostname,server_p->port,server_p->files);
      server_p=server_p->next;
    }   while (server_p != config->servers);
}
