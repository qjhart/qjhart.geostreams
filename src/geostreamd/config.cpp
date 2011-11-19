/*
  config - geostreamd

  Copyright (C) 2004 Quinn Hart

  $Id: config.cpp,v 1.1 2005/04/06 23:52:02 jiezhang Exp $

  $Log: config.cpp,v $
  Revision 1.1  2005/04/06 23:52:02  jiezhang
  Remove missed old HTTP server C code and check in some more HTTP server C++ code.

  Revision 1.1  2004/11/30 19:01:15  qjhart
  New server


*/
#include "dotconf.h"
#include "geostreamd.h"

FUNC_ERRORHANDLER(errorhandler);
static DOTCONF_CB(cb_port);
static DOTCONF_CB(cb_num_clients);
static DOTCONF_CB(cb_facility);
static DOTCONF_CB(cb_req_style);
static DOTCONF_CB(cb_write_style);
static DOTCONF_CB(cb_host);
static DOTCONF_CB(cb_gvar);

static const configoption_t options[] = {
	{"Port", ARG_INT, cb_port, NULL, CTX_ALL },
	{"MaxClients",ARG_INT,cb_num_clients,NULL,CTX_ALL},
	{"LocalFacility",ARG_INT,cb_facility,NULL,CTX_ALL},
	{"RequestStyle",ARG_STR,cb_req_style,NULL,CTX_ALL},
	{"WriteStyle",ARG_STR,cb_write_style,NULL,CTX_ALL},
	{"Host",ARG_STR,cb_host,NULL,CTX_ALL},
	{"Gvar",ARG_STR,cb_gvar,NULL,CTX_ALL},
	LAST_OPTION
};

FUNC_ERRORHANDLER(errorhandler)
{
  gLog->add (type, msg); /* type Matches SYSLOG */
  return (type==DCLOG_ERR)?1:0;
}

DOTCONF_CB(cb_gvar)
{
  char *cp;
  struct gs_config *config = (struct gs_config *)ctx;
  if ((config->def->m_gvar=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");
  cp = strchr(cmd->data.str,':');
  if (cp!=NULL)
    {
      *cp=0;
      config->def->m_gvarPort=atoi(cp+1);
    }
  if ((config->def->m_gvarName=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");

  debug("(%p)%s: %s",config->def,cmd->name,cmd->data.str);
  return NULL;
}

DOTCONF_CB(cb_host)
{
  char *cp;
  struct gs_config *config = (struct gs_config *)ctx;
  if ((config->def->m_host=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");
  cp = strchr(cmd->data.str,':');
  if (cp!=NULL)
    {
      *cp=0;
      config->def->m_port=atoi(cp+1);
    }
  if ((config->def->m_hostname=strdup(cmd->data.str)) == (void *)0)
    return("Insuffcient Memory");

  debug("(%p)%s: %s",config->def,cmd->name,cmd->data.str);
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
    config->def->m_port=port;
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
  config->def->m_numOfConns=num_clients;
  return NULL;
}

DOTCONF_CB(cb_req_style)
{
  struct gs_config *config = (struct gs_config *)ctx;
  if (strcasecmp(cmd->data.str,"THREADS")==0)
    config->def->m_reqStyle = Geostream::THREADS;
  else if (strcasecmp(cmd->data.str,"EVENTS")==0)
    config->def->m_reqStyle = Geostream::EVENTS;
  else 
    return "Bad Request Style";
  return NULL;
}

DOTCONF_CB(cb_write_style)
{
  struct gs_config *config = (struct gs_config *)ctx;
  if (strcasecmp(cmd->data.str,"THREADS")==0)
    config->def->m_writeStyle = Geostream::THREADS;
  else if (strcasecmp(cmd->data.str,"EVENTS")==0)
    config->def->m_writeStyle = Geostream::EVENTS;
  else 
    return "Bad Write Style";
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
    gLog->add (LOG_ERR,"Error opening config file");
    return 1;
  }
    
  if (dotconf_command_loop(configfile) == 0) {
    gLog->add (LOG_ERR,"Error reading config file");
    return 1;
  }
  dotconf_cleanup(configfile);
  return 0;
}

void config_print(struct gs_config *config)
{
  Geostream::GeostreamServer *server_p;
  server_p= config->def;
  printf("%s:%d\n",server_p->m_hostname,server_p->m_port);
}
