#include "ELUG_C.h"
#include "goesct3.h"

void pl2se(integer instr, real pixel, real line, real *elev, real *scan)
{
  *elev = evln_(&instr, &line);
  *scan = scpx_(&instr,&pixel);
}


void se2pl(integer instr, doublereal scan, doublereal elev, real *line, real *pixel)
{
  real rscan;
  real relev;
  
  rscan = (real) scan;
  relev = (real) elev;
  evsc2lpf_(&instr, &relev, &rscan, line, pixel);
}


 /* Returns undef on ierr=1
  * new argument: flipflag
  */
int ll2pl(integer instr, integer flipflag, real rlon, real rlat, real *line, real *pixel)
{
  real alf;
  real gam;
  integer ierr;

  gpoint_(&instr, &flipflag, &rlat, &rlon, &alf, &gam, &ierr);
  if(ierr == 0) 
    {
      evsc2lpf_(&instr, &alf, &gam, line, pixel);
    }
  return ierr;
}


/* Returns undef on ierr=1
 * new argument: flipflag
 */
int ll2se(integer instr, integer flipflag, doublereal rlon, doublereal rlat, real *ev, real *sc)
{
  real rrlon;
  real rrlat;
  integer ierr;

  rrlon = (real) rlon;
  rrlat = (real) rlat;
  gpoint_(&instr, &flipflag, &rrlat, &rrlon, ev, sc, &ierr);

  return ierr;
}





/*  new arguments: instr, flipflag
 */
int se2ll(integer instr, integer flipflag, real scan, real elev, real *rlat, real *rlon)
{
  integer ierr;

  lpoint_(&instr, &flipflag, &elev, &scan, rlat, rlon, &ierr);
  return ierr;
}


/* new argument: flipflag
 */
int pl2ll(integer instr, integer flipflag, real pixel, real line, real *rlat, real *rlon)
{
  real elev;
  real scan;
  integer ierr;

  elev = (real) evln_(&instr,&line);
  scan = (real) scpx_(&instr,&pixel);
  ierr = 21;
  lpoint_(&instr, &flipflag, &elev, &scan, rlat, rlon, &ierr);
  
  return ierr;
}

int setcons(integer nscyc1, integer nsinc1, integer ewcyc1, integer ewinc1, integer nscyc2, integer nsinc2, integer ewcyc2, integer ewinc2) 
{
  return setcons_(&nscyc1, &nsinc1, &ewcyc1, &ewinc1, &nscyc2, &nsinc2, &ewcyc2, &ewinc2);
}

doublereal timex(integer ny, integer nd, integer nh, integer nm, doublereal s) {
  return timex_(&ny, &nd, &nh, &nm, &s);
}


int lmodel(doublereal t, doublereal tu, doublereal *rec, integer imc, doublereal *rlat, doublereal *rlon) {
  return lmodel_(&t, &tu, rec, &imc, rlat, rlon);
}
