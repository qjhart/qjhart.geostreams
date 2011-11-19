#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

//Quinn's way to insert ELUG code into perl source.
#include "ELUG.c"

// This might fix some errors in linking on some f2c platforms.
int  MAIN__( )
 {  return(0);
}


MODULE = Gvar		PACKAGE = Gvar		

void 
pl2se(instr,pixel,line)
	integer instr
	real pixel
	real line
INIT:
	real elev;
	real scan;
PPCODE:
	elev=evln_(&instr,&line);
	scan=scpx_(&instr,&pixel);
	XPUSHs(sv_2mortal(newSVnv(scan)));
	XPUSHs(sv_2mortal(newSVnv(elev)));

void 
se2pl(instr,scan,elev)
     integer instr
     doublereal scan;
     doublereal elev;
INIT:
     real rscan;
     real relev;
     real line;
     real pixel;
PPCODE:
#	evsc2l_(&instr, &elev, &scan, &line, &pixel);

	rscan = (real) scan;
	relev = (real) elev;
	evsc2lpf_(&instr, &relev, &rscan, &line, &pixel);
	XPUSHs(sv_2mortal(newSVnv((doublereal) pixel)));
	XPUSHs(sv_2mortal(newSVnv((doublereal) line)));

# Returns undef on ierr=1
# new argument: flipflag
void 
ll2pl(instr,flipflag,rlon,rlat)
     integer instr
	 integer flipflag
     real rlon
     real rlat
INIT:
	 real alf;
	 real gam;
     real line;
     real pixel;
     integer ierr;
PPCODE:
#	gpoint_(&rlat, &rlon, &alf, &gam, &ierr);
	gpoint_(&instr, &flipflag, &rlat, &rlon, &alf, &gam, &ierr);
	if(ierr == 0) {
#	  evsc2l_(&instr, &alf, &gam, &line, &pixel);
	  evsc2lpf_(&instr, &alf, &gam, &line, &pixel);
	  XPUSHs(sv_2mortal(newSVnv(pixel)));
	  XPUSHs(sv_2mortal(newSVnv(line)));
	}

# Returns undef on ierr=1
# new argument: flipflag
void 
ll2se(instr,flipflag,rlon,rlat)
     integer instr
	 integer flipflag
     doublereal rlon
     doublereal rlat
INIT:
     real rrlon;
     real rrlat;
	 real ev;
	 real sc;
     integer ierr;
PPCODE:
#	gpoint_(&rlat, &rlon, &alf, &gam, &ierr);    ev==alf, sc==gam
	rrlon = (real) rlon;
	rrlat = (real) rlat;
	gpoint_(&instr, &flipflag, &rrlat, &rrlon, &ev, &sc, &ierr);
	if(ierr == 0) {
		XPUSHs(sv_2mortal(newSVnv((doublereal) sc)));
		XPUSHs(sv_2mortal(newSVnv((doublereal) ev)));
	}

void
lmodel(t, tu, rec, imc)
	doublereal t
	doublereal tu
	SV *rec
	integer imc
INIT:
	int rc;
	doublereal rlat;
	doublereal rlon;
	doublereal *recptr;
PPCODE:
	recptr=(doublereal *)SvPV_nolen(rec);
	lmodel_(&t, &tu, recptr, &imc, &rlat, &rlon);
	XPUSHs(sv_2mortal(newSVnv(rlon)));
	XPUSHs(sv_2mortal(newSVnv(rlat)));

	
# new arguments: instr, flipflag
void
se2ll(instr,flipflag,scan,elev)
	integer instr
	integer flipflag
	real scan
	real elev
INIT:
	real rlat;
	real rlon;
	integer ierr;
PPCODE:
#	lpoint_(&elev,&scan,&rlat, &rlon,&ierr);
	lpoint_(&instr, &flipflag, &elev, &scan, &rlat, &rlon, &ierr);
	if(ierr == 0) {
  	  XPUSHs(sv_2mortal(newSVnv(rlon)));
	  XPUSHs(sv_2mortal(newSVnv(rlat)));
	}

	
# new argument: flipflag
void 
pl2ll(instr,flipflag,pixel,line)
	integer instr
	integer flipflag
	real pixel
	real line
INIT:
	real elev;
	real scan;
	real rlat;
	real rlon;
	integer ierr;
PPCODE:
	elev = (real) evln_(&instr,&line);
	scan = (real) scpx_(&instr,&pixel);
#	lpoint_(&elev,&scan,&rlat, &rlon,&ierr);
	ierr = 21;
	lpoint_(&instr, &flipflag, &elev, &scan, &rlat, &rlon, &ierr);
	XPUSHs(sv_2mortal(newSVnv(rlon)));
	XPUSHs(sv_2mortal(newSVnv(rlat)));

# old 'setcon'
# void
# setcon(instr, ns_nad_cy__, ns_nad_inc__, ew_nad_cy__, ew_nad_inc__)
# 	integer instr
# 	integer ns_nad_cy__
# 	integer ns_nad_inc__
# 	integer ew_nad_cy__
# 	integer ew_nad_inc__
# CODE:
# 	setcon_(&instr, &ns_nad_cy__, &ns_nad_inc__, &ew_nad_cy__, &ew_nad_inc__);
#
# new 'setcons', with 8 parameters:
#	NSCYC1, NSINC1 ? north/south cycles and increments of the Imager nadir (I*4)
#	EWCYC1, EWINC1 ? east/west cycles and increments of the Imager nadir (I*4)
#	NSCYC2, NSINC2 ? north/south cycles and increments of the Sounder nadir (I*4)
#	EWCYC2, EWINC2 ? east/west cycles and increments of the Sounder nadir (I*4)
# "It needs to be called whenever the origin position in GVAR changes."
void
setcons(nscyc1, nsinc1, ewcyc1, ewinc1, nscyc2, nsinc2, ewcyc2, ewinc2)
	integer nscyc1
	integer nsinc1
	integer ewcyc1
	integer ewinc1
	integer nscyc2
	integer nsinc2
	integer ewcyc2
	integer ewinc2
CODE:
	setcons_(&nscyc1, &nsinc1, &ewcyc1, &ewinc1, &nscyc2, &nsinc2, &ewcyc2, &ewinc2);
	
	
E_f
time50(bcd)
	SV *bcd
INIT:
	integer *bcdptr;
CODE:
	bcdptr=(integer *)SvPV_nolen(bcd);
	printf("bcd = [%d, %d]\n", bcdptr[0], bcdptr[1]);
	RETVAL=time50_(bcdptr);
OUTPUT:
	RETVAL
	
doublereal
timex(ny, nd, nh, nm, s)
	integer	ny
	integer	nd
	integer	nh
	integer	nm
	doublereal s
CODE:
	RETVAL=timex_(&ny,&nd,&nh,&nm,&s);
OUTPUT:
	RETVAL
