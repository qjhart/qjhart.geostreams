/** higher level routines on top of goesct2.h **/

#include "f2c.h"

void pl2se(integer instr, real pixel, real line, real *elev, real *scan);
void se2pl(integer instr, doublereal scan, doublereal elev, real *line, real *pixel);
int ll2pl(integer instr, integer flipflag, real rlon, real rlat, real *line, real *pixel);
int ll2se(integer instr, integer flipflag, doublereal rlon, doublereal rlat, real *ev, real *sc);
int se2ll(integer instr, integer flipflag, real scan, real elev, real *rlat, real *rlon);
int pl2ll(integer instr, integer flipflag, real pixel, real line, real *rlat, real *rlon);
int setcons(integer nscyc1, integer nsinc1, integer ewcyc1, integer ewinc1, integer nscyc2, integer nsinc2, integer ewcyc2, integer ewinc2);
doublereal timex(integer ny, integer nd, integer nh, integer nm, doublereal s);
int lmodel(doublereal t, doublereal tu, doublereal *rec, integer imc, doublereal *rlat, doublereal *rlon);
