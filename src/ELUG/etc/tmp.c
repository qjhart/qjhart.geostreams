/* Main program */ int MAIN__()
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

    /* Format strings */
    static char fmt_500[] = "(1x,\002EPOCH TIME =\002,f15.5)";
    static char fmt_510[] = "(//1x,\002IMC =\002,i2,3x,\002SUBSATELLITE LA\
T =\002,f7.4,3x,\002LON =\002,f9.4)";
    static char fmt_520[] = "(/1x,\002INSTRUMENT =\002,i2//3x,\002LAT/LON TO\
 LINE/PIXEL TRANSFORMATION:\002/5x,\002LAT =\002,f8.4,4x,\002LON =\002,f9.4)";
    static char fmt_530[] = "(5x,\002N-S =\002,f7.4,5x,\002E-W =\002,f7.4/\
5x,\002LINE =\002,f9.2,2x,\002PIXEL =\002,f9.2)";
    static char fmt_540[] = "(3x,\002LINE/PIXEL TO LAT/LON TRANSFORMATION\
:\002/5x,\002LINE =\002,f9.2,2x,\002PIXEL =\002,f9.2/5x,\002N-S =\002,f7.4,5\
x,\002E-W =\002,f7.4)";
    static char fmt_550[] = "(5x,\002LAT =\002,f8.4,4x,\002LON =\002,f9.4)";
    static char fmt_560[] = "(/3x,\002CYCLES/INCREMENTS TO LAT/LON TRANSFORM\
ATION:\002/5x,\002E-W CYCLES/INCREMENTS =\002,i2,\002,\002,i5/5x,\002N-S CYC\
LES/INCREMENTS =\002,i2,\002,\002,i5/5x,\002E-W SERVO ERROR =\002,f7.2,\002 \
(mrad)\002/5x,\002N-S SERVO ERROR =\002,f7.2,\002 (mrad)\002)";
    static char fmt_570[] = "(/5x,\002DETECTOR\002,6x,\002#1\002,6x,\002#\
2\002,6x,\002#3\002,6x,\002#4\002/5x,\002E-W OFFSET\002,4f8.2,\002 (mrad)\
\002/5x,\002N-S OFFSET\002,4f8.2,\002 (mrad)\002/)";
    static char fmt_580[] = "(5x,\002DETECTOR #\002,i1,3x,\002LAT =\002,f8.4\
,2x,\002LON =\002,f9.4)";

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

    /* Fortran I/O blocks */
    static cilist io___113 = { 0, 6, 0, fmt_500, 0 };
    static cilist io___119 = { 0, 6, 0, fmt_510, 0 };
    static cilist io___121 = { 0, 6, 0, fmt_520, 0 };
    static cilist io___129 = { 0, 6, 0, fmt_530, 0 };
    static cilist io___130 = { 0, 6, 0, fmt_540, 0 };
    static cilist io___131 = { 0, 6, 0, fmt_550, 0 };
    static cilist io___133 = { 0, 6, 0, fmt_560, 0 };
    static cilist io___134 = { 0, 6, 0, fmt_570, 0 };
    static cilist io___137 = { 0, 6, 0, fmt_580, 0 };




/* *********************************************************************** */
/*     SET CONSTANTS IN COMMON INSTCOMM */
    setcons_(&c__4, &c__3068, &c__2, &c__3068, &c__4, &c__1402, &c__2, &
	    c__1402);
/*     COMPUTE EPOCH TIME */
/*      TU=TIME50(REC(12)) */
    tu = (float)20557829.57612;
    s_wsfe(&io___113);
    do_fio(&c__1, (char *)&tu, (ftnlen)sizeof(doublereal));
    e_wsfe();
    t = tu + (float)20.;
    for (flip_flg__ = 1; flip_flg__ >= -1; flip_flg__ += -2) {
	for (imc = 0; imc <= 1; ++imc) {
/*     COMPUTE DATA NEEDED FOR INSTRUMENT TO EARTH COORDINATES */
/*     TRANSFORMATION */
	    lmodel_(&t, &tu, rec, &imc, &rlat, &rlon);
	    s_wsfe(&io___119);
	    do_fio(&c__1, (char *)&imc, (ftnlen)sizeof(integer));
	    d__1 = rlat * 57.295779513082323;
	    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	    d__2 = rlon * 57.295779513082323;
	    do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(doublereal));
	    e_wsfe();
	    for (instr = 1; instr <= 2; ++instr) {
/*     GEOGRAPHIC TO LINE/PIXEL COORDINATES TRANSFORMATION: */
/*       SET INPUT LATITUDE AND LONGITUDE */
		rlat = dlat[instr - 1] * .017453292519943295;
		rlon = dlon[instr - 1] * .017453292519943295;
		s_wsfe(&io___121);
		do_fio(&c__1, (char *)&instr, (ftnlen)sizeof(integer));
		do_fio(&c__1, (char *)&dlat[instr - 1], (ftnlen)sizeof(
			doublereal));
		do_fio(&c__1, (char *)&dlon[instr - 1], (ftnlen)sizeof(
			doublereal));
		e_wsfe();
/*       TRANSFORM LAT/LON TO N-S AND E-W INSTRUMENT ANGLES */
		rrlat = rlat;
		rrlon = rlon;
		gpoint_(&instr, &flip_flg__, &rrlat, &rrlon, &e, &s, &ier);
		if (ier == 0) {
/*     CONVERT N-S AND E-W ANGLES TO LINE/PIXEL COORDINATES */
		    evsc2lpf_(&instr, &e, &s, &rl, &rp);
		    s_wsfe(&io___129);
		    d__1 = e * 57.295779513082323;
		    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
		    d__2 = s * 57.295779513082323;
		    do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(doublereal));
		    do_fio(&c__1, (char *)&rl, (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&rp, (ftnlen)sizeof(real));
		    e_wsfe();
/*     REVERSE TRANSFORMATION: LINE/PIXEL TO GEOGRAPHIC COORDINATES */
/*       CONVERT LINE/PIXEL NUMBERS TO N-S AND E-W INSTRUMENT ANGLES */
		    e = evln_(&instr, &rl);
		    s = scpx_(&instr, &rp);
		    s_wsfe(&io___130);
		    do_fio(&c__1, (char *)&rl, (ftnlen)sizeof(real));
		    do_fio(&c__1, (char *)&rp, (ftnlen)sizeof(real));
		    d__1 = e * 57.295779513082323;
		    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
		    d__2 = s * 57.295779513082323;
		    do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(doublereal));
		    e_wsfe();
/*       TRANSFORM N-S AND E-W ANGLES TO GEOGRAPHIC COORDINATES */
		    rrlat = rlat;
		    rrlon = rlon;
		    lpoint_(&instr, &flip_flg__, &e, &s, &rrlat, &rrlon, &ier)
			    ;
		    if (ier == 0) {
			s_wsfe(&io___131);
			d__1 = rlat * 57.295779513082323;
			do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(
				doublereal));
			d__2 = rlon * 57.295779513082323;
			do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(
				doublereal));
			e_wsfe();
		    }
		}
	    }
	}
/*     TRANSFORM CYCLES/INCREMENTS, SERVO ERROR VALUES AND THE */
/*     FACTORY-MEASURED DETECTOR OFFSETS TO LAT/LON COORDINATES */
/*     FOR THE FOUR DETECTORS OF THE SOUNDER INSTRUMENT */
	sndeloc_(&flip_flg__, &icx, &incx, &icy, &incy, &svx, &svy, doff, geo)
		;
	s_wsfe(&io___133);
	do_fio(&c__1, (char *)&icx, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&incx, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&icy, (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&incy, (ftnlen)sizeof(integer));
	r__1 = svx * (float)1e6;
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	r__2 = svy * (float)1e6;
	do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___134);
	for (j = 1; j <= 2; ++j) {
	    for (i__ = 1; i__ <= 4; ++i__) {
		r__1 = doff[i__ + (j << 2) - 5] * (float)1e6;
		do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	    }
	}
	e_wsfe();
	s_wsfe(&io___137);
	for (i__ = 1; i__ <= 4; ++i__) {
	    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    d__1 = geo[i__ - 1] * 57.295779513082323;
	    do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	    d__2 = geo[i__ + 3] * 57.295779513082323;
	    do_fio(&c__1, (char *)&d__2, (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
	setcons_(&c__4, &c__3068, &c__2, &c__3068, &c__4, &c__1403, &c__2, &
		c__1403);
    }
} /* MAIN__ */

/* Main program alias */ int test_ () { MAIN__ (); }
