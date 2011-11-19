// GVAR Network API Header
// Copyright  Automated Sciences March 2001

/* maximum GVAR packet size is less than this */
#define GVNETBUFFSIZE 32768

/* function prototypes for the API functions */
int gvarnet_open(char *ipaddr);
int gvarnet_close(void);
int gvarnet_read(char *pbuff,int buffsize,int *numread);

/* error definitions */
#define E_SOCKET  10
#define E_CONNECT 11
#define E_CLOSE   12
#define E_READ    13
#define E_BAD_START 14
#define E_BAD_END   15
#define E_BAD_SIZE  16
#define E_BAD_SEQNUM  17

/* notification that the socket was closed by the server */
/* this should only happen if the GVAR daemon process goes down */
#define N_CLOSED      20

