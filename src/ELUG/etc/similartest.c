#include <stdio.h>
//#include "goesct2.h"
#include "ELUG_C.h"

static integer c__2 = 2;
static integer c__4 = 4;
static integer c__3068 = 3068;
static integer c__1402 = 1402;
static integer c__1403 = 1403;


int main()
{
    /* Initialized data */

    static doublereal rec[336] = { 0.,0.,0.,0.,-1.747405052185,84.06604003906,
	    -.034368492669,-.006102979183197,0.,0.,0.,428409632.,1653818727.,
	    0.,3e-4,-3e-4,-2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,
	    2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,
	    2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,
	    2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,2e-4,.004363,0.,5e-4,100.,.002,15.,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,4.,2.,2.,1e-5,0.,.01,2.,3.,-1e-5,0.,.01,3.,2.,
	    1e-5,0.,.01,3.,3.,-1e-5,0.,.01,5e-4,100.,.002,15.,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,4.,2.,2.,1e-5,0.,.01,2.,3.,-1e-5,0.,.01,3.,2.,1e-5,0.,.01,3.,
	    3.,-1e-5,0.,.01,5e-4,100.,.001,15.,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,4.,2.,2.,
	    1e-5,0.,.01,2.,3.,-1e-5,0.,.01,3.,2.,1e-5,0.,.01,3.,3.,-1e-5,0.,
	    .01,-5e-5,10.,.001,15.,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,4.,2.,2.,1e-5,0.,.01,
	    2.,3.,-1e-5,0.,.01,3.,2.,1e-5,0.,.01,3.,3.,-1e-5,0.,.01,-5e-5,10.,
	    .001,15.,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,
	    5e-6,5e-6,5e-6,5e-6,5e-6,5e-6,4.,2.,2.,1e-5,0.,.01,2.,3.,-1e-5,0.,
	    .01,3.,2.,1e-5,0.,.01,3.,3.,-1e-5,0.,.01 };
    static doublereal dlat[2] = { 50.,-50. };
    static doublereal dlon[2] = { -150.,-50. };
    static real svx = (float)-2.1e-5;
    static real svy = (float)1.4e-5;
    static real doff[8]	/* was [4][2] */ = { (float)2.8e-5,(float)5.6e-5,(
	    float)-2.8e-5,(float)-5.6e-5,(float)8.4e-5,(float)1.12e-4,(float)
	    1.4e-5,(float)4.2e-5 };
    static integer icx = 1;
    static integer incx = 2715;
    static integer icy = 5;
    static integer incy = 2580;

    /* System generated locals */
    real r__1, r__2;
    doublereal d__1, d__2;

    /* Builtin functions */
    integer s_wsfe(), do_fio(), e_wsfe();

    /* Local variables */
    extern /* Subroutine */ int evsc2lpf_();
    static doublereal rlat;
    extern doublereal evln_();
    static integer flip_flg__;
    static doublereal rlon;
    extern doublereal scpx_();
    static real e;
    static integer i__, j;
    static real s;
    static doublereal t;
    static real rrlat, rrlon;
    static integer instr;
    static real rl, rp;
    static doublereal tu;
    extern /* Subroutine */ int lmodel_(), gpoint_(), lpoint_();
    static integer imc;
    static real geo[8]	/* was [4][2] */;
    static integer ier;
    extern /* Subroutine */ int sndeloc_(), setcons_();

	doublereal tmp_dlat, tmp_dlon;
	real tmp_rlat, tmp_rlon;
        real tmp_rl, tmp_rp;
	real tmp_e, tmp_s;

	tmp_dlat = 50.0;
	tmp_dlon = -135.0;

/* *********************************************************************** */
/*     SET CONSTANTS IN COMMON INSTCOMM */
    setcons_(&c__4, &c__3068, &c__2, &c__3068, &c__4, &c__1402, &c__2, &
	    c__1402);
/*     COMPUTE EPOCH TIME */
/*      TU=TIME50(REC(12)) */
    tu = (float)20557829.57612;
    t = tu + (float)20.;
    for (flip_flg__ = 1; flip_flg__ >= -1; flip_flg__ += -2) {
	for (imc = 0; imc <= 1; ++imc) {
/*     COMPUTE DATA NEEDED FOR INSTRUMENT TO EARTH COORDINATES */
/*     TRANSFORMATION */
	    lmodel_(&t, &tu, rec, &imc, &rlat, &rlon);
	   
	    d__1 = rlat * 57.295779513082323;
	    d__2 = rlon * 57.295779513082323;
	    printf("Subsatelllite Lat = %f  Lon = %f\n", d__1, d__2); 

	    for (instr = 1; instr <= 2; ++instr) {
/*     GEOGRAPHIC TO LINE/PIXEL COORDINATES TRANSFORMATION: */
/*       SET INPUT LATITUDE AND LONGITUDE */
		printf("Instrument = %d Flipflag %d IMC= %d \n\n", 
	            instr, flip_flg__, imc);
		printf("LAT/LON TO LINE/PIXEL TRANSFORMATION: \n");
		
		rlat = dlat[instr - 1] * .017453292519943295;
		rlon = dlon[instr - 1] * .017453292519943295;

		printf("     LAT= %f  LON= %f \n", dlat[instr-1], dlon[instr-1]);
		printf("     RLAT= %f  RLON= %f \n", rlat, rlon);

/*       TRANSFORM LAT/LON TO N-S AND E-W INSTRUMENT ANGLES */
		rrlat = rlat;
		rrlon = rlon;
		gpoint_(&instr, &flip_flg__, &rrlat, &rrlon, &e, &s, &ier);
		
		printf("     e= %f  s= %f \n", e, s);

	        if ((instr == 1) && (imc == 0) && (flip_flg__  == 1)) {

		 printf("here is random test\n");
	         tmp_rlat = tmp_dlat * .017453292519943295;
	         tmp_rlon = tmp_dlon * .017453292519943295;
	
		 gpoint_(&instr, &flip_flg__, &tmp_rlat, &tmp_rlon, &tmp_e, &tmp_s, &ier);
		 printf("ier = %d \n", ier);
	         evsc2lpf_(&instr, &tmp_e, &tmp_s, &tmp_rl, &tmp_rp);
                    printf("*********     E= %f  S= %f \n", tmp_e, tmp_s);
                    printf("*********     Line= %f  Pixel= %f \n", tmp_rl, tmp_rp);
 
                }
	
		if (ier == 0) {
/*     CONVERT N-S AND E-W ANGLES TO LINE/PIXEL COORDINATES */
		    evsc2lpf_(&instr, &e, &s, &rl, &rp);
		    d__1 = e * 57.295779513082323;
		    d__2 = s * 57.295779513082323;
		   
		    printf("     E= %f  S= %f \n", d__1, d__2);
		    printf("     Line= %f  Pixel= %f \n", rl, rp);

/*     REVERSE TRANSFORMATION: LINE/PIXEL TO GEOGRAPHIC COORDINATES */
/*       CONVERT LINE/PIXEL NUMBERS TO N-S AND E-W INSTRUMENT ANGLES */
		    printf("LINE/PIXEL TO LAT/LON TRANSFORMATION: \n");
		    printf("     Line= %f  Pixel= %f \n", rl, rp);
		    
		    e = evln_(&instr, &rl);
		    s = scpx_(&instr, &rp);
		    printf("     e= %f  s= %f \n", e, s);
		    d__1 = e * 57.295779513082323;
		    d__2 = s * 57.295779513082323;
		    printf("    dE= %f dS= %f \n", d__1, d__2);
/*       TRANSFORM N-S AND E-W ANGLES TO GEOGRAPHIC COORDINATES */
		    rrlat = rlat;
		    rrlon = rlon;
		    printf("     before lpoint: RLAT= %f  RLON= %f \n", rlat, rlon);
		    lpoint_(&instr, &flip_flg__, &e, &s, &rrlat, &rrlon, &ier)
			    ;
		    if (ier == 0) {
			d__1 = rlat * 57.295779513082323;
			d__2 = rlon * 57.295779513082323;
		        printf("     after lpoint: RRLAT= %f  RRLON= %f \n", rrlat, rrlon);
		        printf("     after lpoint: DLAT= %f  DLON= %f \n\n", d__1, d__2);
		    }
		}
	    }
	}
/*     TRANSFORM CYCLES/INCREMENTS, SERVO ERROR VALUES AND THE */
/*     FACTORY-MEASURED DETECTOR OFFSETS TO LAT/LON COORDINATES */
/*     FOR THE FOUR DETECTORS OF THE SOUNDER INSTRUMENT */
	sndeloc_(&flip_flg__, &icx, &incx, &icy, &incy, &svx, &svy, doff, geo)
		;
	r__1 = svx * (float)1e6;
	r__2 = svy * (float)1e6;
	for (j = 1; j <= 2; ++j) {
	    for (i__ = 1; i__ <= 4; ++i__) {
		r__1 = doff[i__ + (j << 2) - 5] * (float)1e6;
	    }
	}
	for (i__ = 1; i__ <= 4; ++i__) {
	    d__1 = geo[i__ - 1] * 57.295779513082323;
	    d__2 = geo[i__ + 3] * 57.295779513082323;
	}
	setcons_(&c__4, &c__3068, &c__2, &c__3068, &c__4, &c__1403, &c__2, &
		c__1403);
    }

return 0;
} 
