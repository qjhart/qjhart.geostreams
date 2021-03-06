/* ELUG_AND_TEST.F -- translated by f2c (version 20010618).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Common Block Declarations */

struct {
    integer incmax[2];
    real elvmax[2], scnmax[2], elvincr[2], scnincr[2], elvln[2], scnpx[2], 
	    ewnom[2], nsnom[2];
} instcomm_;

#define instcomm_1 instcomm_

struct {
    doublereal xs[3], bt[9]	/* was [3][3] */, q3;
    real pitch, roll, yaw, pma, rma;
} elcomm_;

#define elcomm_1 elcomm_

/* Table of constant values */

static integer c__62 = 62;
static integer c__117 = 117;
static integer c__172 = 172;
static integer c__227 = 227;
static integer c__282 = 282;
static integer c__2 = 2;
static integer c__1 = 1;
static integer c__4 = 4;
static integer c__3068 = 3068;
static integer c__1402 = 1402;
static integer c__1403 = 1403;

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : EVLN */
/* **   SOURCE    : EVLN.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A     10/27/88  IL   INITIAL CREATION */
/* *********************************************************************** */
/* ** */
/* **   THIS FUNCTION CONVERTS FRACTIONAL LINE NUMBER TO ELEVATION ANGLE */
/* **   IN RADIANS. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
doublereal evln_(instr, rline)
integer *instr;
real *rline;
{
    /* System generated locals */
    real ret_val;


/*     CALLING PARAMETERS */

/*                       INSTRUMENT CODE (1-IMAGER, 2-SOUNDER) */
/*                       FRACTIONAL LINE  NUMBER */

/*     LOCAL VARIABLES - NONE */

/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    if (*instr == 1) {
	ret_val = instcomm_1.elvmax[*instr - 1] - (*rline - (float)4.5) * 
		instcomm_1.elvln[*instr - 1];
    } else {
	ret_val = instcomm_1.elvmax[*instr - 1] - (*rline - (float)2.5) * 
		instcomm_1.elvln[*instr - 1];
    }
    return ret_val;
} /* evln_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : EVSC2LPF */
/* **   SOURCE    : EVSC2LPF.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A     10/27/88  IL   INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   THIS SUBROUTINE CONVERTS ELEVATION AND SCAN ANGLES */
/* **   TO THE FRACTIONAL LINE AND PIXEL NUMBERS. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int evsc2lpf_(instr, elev, scan, rl, rp)
integer *instr;
real *elev, *scan, *rl, *rp;
{

/*     CALLING PARAMETERS */

/*                       INSTRUMENT CODE (1-IMAGER, 2-SOUNDER) */
/*                       ELEVATION ANGLE IN RADIANS */
/*                       SCAN ANGLE IN RADIANS */
/*                       LINE NUMBER */
/*                       PIXEL NUMBER */

/*     LOCAL VARIABLES - NONE */


/*     INCLUDE FILES */

/* ************************************************************** */

/*     COMPUTE FRACTIONAL LINE NUMBER */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    *rl = (instcomm_1.elvmax[*instr - 1] - *elev) / instcomm_1.elvln[*instr - 
	    1];
    if (*instr == 1) {
	*rl += (float)4.5;
    } else {
	*rl += (float)2.5;
    }

/*     COMPUTE FRACTIONAL PIXEL NUMBER */

    *rp = (instcomm_1.scnmax[*instr - 1] + *scan) / instcomm_1.scnpx[*instr - 
	    1] + (float)1.;
    return 0;
} /* evsc2lpf_ */

/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : GATT */
/* **   SOURCE    : F.GATT */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A     12/01/88  IL   INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **    THIS FUNCTION COMPUTES AN ATTITUDE/MISALIGNMENT ANGLE FROM A */
/* **    GIVEN SUBSET OF THE O&A PARAMETERS IN GVAR BLOK 0. */
/* **    ARGUMENT K0 INDICATES THE FIRST WORD OF THE SUBSET. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : LMODEL */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
doublereal gatt_(k0, rec, wa, te)
integer *k0;
doublereal *rec, *wa, *te;
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1;
    static doublereal equiv_0[1], equiv_1[1];

    /* Builtin functions */
    double exp(), cos(), pow_di();

    /* Local variables */
    static integer i__;
#define j ((integer *)equiv_0)
    static integer k, l;
#define m ((integer *)equiv_1)
    static integer ll;
    static doublereal ir;
#define jr (equiv_0)
#define mr (equiv_1)
    static doublereal att;


/*     CALLING PARAMETERS */

/*                    STARTING POSITION OF A PARAMETER SUBSET IN THE */
/*                    O&A SET */
/*                    INPUT O&A PARAMETER SET */
/*                    INPUT SOLAR ORBIT ANGLE IN RADIANS */
/*                    INPUT EXPONENTIAL TIME DELAY FROM EPOCH (MINUTES) */

/*     LOCAL VARIABLES */


/*     INCLUDE FILES */

/* *********************************************************************** */

/*     CONSTANT COMPONENT */

    /* Parameter adjustments */
    --rec;

    /* Function Body */
    k = *k0;
    att = rec[k + 2];

/*     COMPUTES THE EXPONENTIAL TERM */

    if (*te >= 0. && rec[k + 1] > 0.) {
	att += rec[k] * exp(-(*te) / rec[k + 1]);
    }

/*     EXTRACTS THE NUMBER OF SINUSOIDS */

    ir = rec[k + 3];
    i__ = (integer) ir;

/*     CALCULATION OF SINUSOIDS */

    i__1 = i__;
    for (l = 1; l <= i__1; ++l) {
	att += rec[k + (l << 1) + 2] * cos(*wa * l + rec[k + (l << 1) + 3]);
/* L10: */
    }

/*     POINTER TO THE NUMBER OF MONOMIAL SINUSOIDS */

    k += 34;

/*     EXTACTS NUMBER OF MONOMIAL SINUSOIDS */

    ir = rec[k];
    i__ = (integer) ir;
/*     KKK=REC(K) */

/*     COMPUTES MONOMIAL SINUSOIDS */

    i__1 = i__;
    for (l = 1; l <= i__1; ++l) {
	ll = k + l * 5;

/*          ORDER OF SINUSOID */

	*jr = rec[ll - 4];

/*          ORDER OF MONOMIAL SINUSOID */

	*mr = rec[ll - 3];

	d__1 = *wa - rec[ll];
	att += rec[ll - 2] * pow_di(&d__1, m) * cos(*j * *wa + rec[ll - 1]);
/* L20: */
    }
    ret_val = att;
    return ret_val;
} /* gatt_ */

#undef mr
#undef jr
#undef m
#undef j


/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : GPOINT */
/* **   SOURCE    : GPOINT.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     12/10/87  IL   INITIAL CREATION */
/* **   2     06/10/88  IL   REPLACED ASIN WITH ATAN TO SAVE TIME */
/* **   3     06/02/89  IL   COORDINATE AXES CHANGED ACCORDING TO */
/* **                        FORD'S DEFINITION IN SDAIP, DRL 504-01 */
/* **   4     12/01/93  IL   IMPLEMENTED NEW FORMULAE FOR SCAN ANGLE */
/* **                        CORRECTIONS DUE TO MISALIGNMENTS. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   THIS SUBROUTINE CONVERTS GEOGRAPHIC LATITUDE AND LONGITUDE */
/* **   TO THE RELATED ELEVATION AND SCAN ANGLES. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int gpoint_(instr, flip_flg__, rlat, rlon, alf, gam, ierr)
integer *instr, *flip_flg__;
real *rlat, *rlon, *alf, *gam;
integer *ierr;
{
    /* System generated locals */
    real r__1, r__2;

    /* Builtin functions */
    double sin(), sqrt(), cos(), atan(), tan();

    /* Local variables */
    static real doff, sing, slat;
    static doublereal f[3];
    static real u[3], alpha1, w1, w2, ff, ft[3];


/*     CALLING PARAMETERS */

/*                    INSTRUMENT CODE (1=IMAGER,2=SOUNDER) */
/*                   S/C ORIENTATION FLAG (1=NORMAL,-1=INVERTED) */
/*             GEOGRAPHIC LATITUDE IN RADIANS (INPUT) */
/*             GEOGRAPHIC LONGITUDE IN RADIANS (INPUT) */
/*             ELEVATION ANGLE IN RADIANS (OUTPUT) */
/*             SCAN ANGLE IN RADIANS (OUTPUT) */
/*           OUTPUT STATUS; 0 - SUCCESSFUL COMPLETION, */
/*                          1 - POINT WITH GIVEN LAT/LON IS INVISIBLE */

/*     LOCAL VARIABLES */

/*              POINTING VECTOR IN EARTH-CENTERED COORDINATES */
/*              POINTING VECTOR IN INSTRUMENT COORDINATES */
/*              COORDINATES OF THE EARTH POINT (KM) */
/*                    WORK SPACE */

/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCONS */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCONS.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   MATHEMATICAL AND EARTH-RELATED CONSTANTS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*                    DEGREES TO RADIANS CONVERSION PI/180 */
/*                    NOMINAL RADIAL DISTANCE OF SATELLITE (km) */
/*                    EARTH EQUATORIAL RADIUS (km) */
/*                    EARTH FLATTENING COEFFICIENT = 1-(BE/AE) */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   INSTRUMENT POSITION AND ATTITUDE VARIABLES AND TRANSFORMATION */
/* **   MATRIX */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     COMMON VARIABLES */

/*                      NORMALIZED S/C POSITION IN ECEF COORDINATES */
/*                      ECEF TO INSTRUMENT COORDINATES TRANSFORMATION */
/*                      USED IN SUBROUTINE LPOINT */
/*                          PITCH,ROLL,YAW ANGLES OF INSTRUMENT (RAD) */
/*                          PITCH,ROLL MISALIGNMENTS OF INSTRUMENT (RAD) */
/* ******************************************************************** */

/*  COMPUTES SINUS OF GEOGRAPHIC (GEODETIC) LATITUDE */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    sing = sin(*rlat);
    w1 = sing * (float)-.013344266818532624 * sing;

/*      COMPUTE SIGN OF MISALIGNMENT CORRECTIONS AND ORIGIN OFFSET CORRECTIONS */

    ff = (real) (*flip_flg__);
    if (*instr == 2) {
	ff = -ff;
    }
    doff = instcomm_1.scnmax[*instr - 1] - instcomm_1.ewnom[*instr - 1];

/*     SINUS OF THE GEOCENTRIC LATITUDE */

    slat = ((w1 * (float).375 - (float).5) * w1 + (float)1.) * sing / (float)
	    1.0067396607958714;

/*    COMPUTES LOCAL EARTH RADIUS AT SPECIFIED POINT */

    w2 = slat * slat;
    w1 = w2 * (float).006739660795871405;
    w1 = (w1 * (float).375 - (float).5) * w1 + (float)1.;

/*     COMPUTES CARTESIAN COORDINATES OF THE POINT */

    u[2] = slat * w1;
    w2 = w1 * sqrt((float)1. - w2);
    u[0] = w2 * cos(*rlon);
    u[1] = w2 * sin(*rlon);

/*     POINTING VECTOR FROM SATELLITE TO THE EARTH POINT */

    f[0] = u[0] - elcomm_1.xs[0];
    f[1] = u[1] - elcomm_1.xs[1];
    f[2] = u[2] - elcomm_1.xs[2];
    w2 = u[0] * (real) f[0] + u[1] * (real) f[1] + u[2] * (real) f[2] * (
	    float)1.0067396607958714;

/*     VERIFIES VISIBILITY OF THE POINT */

    if (w2 > (float)0.) {
	*ierr = 1;
	*alf = (float)99999.;
	*gam = (float)99999.;
	return 0;
    }

/*    CONVERTS POINTING VECTOR TO INSTRUMENT COORDINATES */

    ft[0] = elcomm_1.bt[0] * f[0] + elcomm_1.bt[1] * f[1] + elcomm_1.bt[2] * 
	    f[2];
    ft[1] = elcomm_1.bt[3] * f[0] + elcomm_1.bt[4] * f[1] + elcomm_1.bt[5] * 
	    f[2];
    ft[2] = elcomm_1.bt[6] * f[0] + elcomm_1.bt[7] * f[1] + elcomm_1.bt[8] * 
	    f[2];

/*     CONVERTS POINTING VECTOR TO SCAN AND ELEVATION ANGLES AND */
/*     CORRECTS FOR THE ROLL AND PITCH MISALIGNMENTS */

/* Computing 2nd power */
    r__1 = ft[1];
/* Computing 2nd power */
    r__2 = ft[2];
    *gam = atan(ft[0] / sqrt(r__1 * r__1 + r__2 * r__2));
    *alf = -atan(ft[1] / ft[2]);
    w1 = sin(*alf);
    w2 = cos(*gam);
    alpha1 = *alf + elcomm_1.rma * ((float)1. - cos(*alf) / w2) + 
	    elcomm_1.pma * w1 * (ff / w2 + tan(*gam));
    *gam -= ff * elcomm_1.rma * w1;
    *alf = alpha1 + alpha1 * *gam * doff;
    *gam -= alpha1 * (float).5 * alpha1 * doff;
    *ierr = 0;
    return 0;
} /* gpoint_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : INST2ER */
/* **   SOURCE    : INST2ER.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     08/16/88  IL   INITIAL CREATION */
/* **   2     11/11/88  IL   TRIGONOMETRIC FUNCTIONS REPLACED WITH */
/* **                        SMALL ANGLE APPROXIMATIONS */
/* **   3    06/02/89   IL   COORDINATE AXES CHANGED ACCORDING TO */
/* **                        FORD'S DEFINITION IN SDAIP, DRL 504-01 */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   INST2ER ACCEPTS THE SINGLE PRECISION ROLL, PITCH AND YAW ANGLES */
/* **   OF AN INSTRUMENT AND RETURNS THE DOUBLE PRECISION INSTRUMENT TO */
/* **   EARTH COORDINATES TRANSFORMATION MATRIX. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int inst2er_(r__, p, y, a, at)
real *r__, *p, *y;
doublereal *a, *at;
{
    /* Builtin functions */
    double sin(), cos();

    /* Local variables */
    static integer i__, j;
    static real cp, cr, cy, sp, sr, sy;
    static doublereal rpy[9]	/* was [3][3] */;


/*     CALLING PARAMETERS */

/*                                   ROLL ANGLE IN RADIANS */
/*                                   PITCH ANGLE IN RADIANS */
/*                                   YAW ANGLE IN RADIANS */
/*                                   SPACECRAFT TO ECEF COORDINATES */
/*                                   TRANSFORMATION MATRIX */
/*                                   INSTRUMENT TO ECEF COORDINATES */
/*                                   TRANSFORMATION MATRIX */

/*     LOCAL VARIABLES */

/*                                   INSTRUMENT TO BODY COORDINATES */
/*                                   TRANSFORMATION MATRIX */
/*                                   INDICES */

/*     INCLUDE FILES */

/* ********************************************************************** */

/*     COMPUTE INSTRUMENT TO BODY COORDINATES TRANSFORMATION MATRIX */

    /* Parameter adjustments */
    at -= 4;
    a -= 4;

    /* Function Body */
    sp = sin(*p);
    cp = cos(*p);
    sr = sin(*r__);
    cr = cos(*r__);
    sy = sin(*y);
    cy = cos(*y);
    rpy[0] = cy * cp;
    rpy[1] = cy * sp * sr + sy * cr;
    rpy[2] = sy * sr - cy * sp * cr;
    rpy[3] = -sy * cp;
    rpy[4] = cy * cr - sp * sr * sy;
    rpy[5] = cy * sr + sy * sp * cr;
    rpy[6] = sp;
    rpy[7] = -cp * sr;
    rpy[8] = cp * cr;

/*     MULTIPLICATION OF MATRICES A AND RPY */

    for (i__ = 1; i__ <= 3; ++i__) {
	for (j = 1; j <= 3; ++j) {
/* L10: */
	    at[i__ + j * 3] = a[i__ + 3] * rpy[j * 3 - 3] + a[i__ + 6] * rpy[
		    j * 3 - 2] + a[i__ + 9] * rpy[j * 3 - 1];
	}
    }
    return 0;
} /* inst2er_ */

/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : LMODEL */
/* **   SOURCE    : F.LMODEL */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     01/09/89  IL   INITIAL CREATION */
/* **   2     06/02/89  IL   COORDINATE AXES CHANGED ACCORDING TO */
/* **                        FORD'S DEFINITION IN SDAIP, DRL 504-01 */
/* **   3     08/21/89  IL   CORRECTED ORBIT ANGLE COMPUTATIONS */
/* **   4     03/08/94  SC   S/C COMPENSATION APPLIED UNCONDITIONALLY; */
/* **                        REFERENCE RADIAL DISTANCE, LATITUDE AND */
/* **                        ORBIT YAW SET TO ZERO IF IMC DISABLED. */
/* **   5     03/08/94  SC   ADDED TRAP FOR SLAT=SYAW=0; CORRECTED */
/* **                        EXPRESSION FOR LAM. */
/* *********************************************************************** */
/* ** */
/* **   THIS SUBROUTINE COMPUTES THE POSITION OF THE SATELLITE AND THE */
/* **   ATTITUDE OF THE IMAGER OR SOUNDER.  THE CALCULATIONS ARE BASED */
/* **   ON THE OATS ORBIT AND ATTITUDE MODEL REPRESENTED BY THE O&A */
/* **   PARAMETER SET IN GVAR BLOCK 0. */
/* **        INPUTS: */
/* **          TIME, EPOCH TIME, O&A PARAMETER SET, IMC STATUS. */
/* ** */
/* **        OUTPUTS: */
/* **          THE SPACECRAFT POSITION VECTOR IN EARTH FIXED COORDINATES; */
/* **          THE GEOMETRIC ROLL, PITCH, YAW ANGLES AND THE ROLL, */
/* **          PITCH MISALIGNMENTS FOR EITHER THE IMAGER OR THE SOUNDER; */
/* **          THE EARTH FIXED TO INSTRUMENT FRAME TRANSFORMATION MATRIX; */
/* **          GEOGRAPHIC LATITUDE AND LONGITUDE AT SUBSATELLITE POINT. */
/* ** */
/* **   DESCRIPTION */
/* **   LMODEL ACCEPTS AN INPUT DOUBLE PRECISION TIME IN MINUTES FROM */
/* **   1950, JAN.1.0 AND AN INPUT SET OF O&A PARAMETERS AND COMPUTES */
/* **   POSITION OF THE SATELLITE, THE ATTITUDE ANGLES AND ATTITUDE */
/* **   MISALIGNMENTS AND THE INSTRUMENT TO EARTH FIXED COORDINATES */
/* **   TRANSFORMATION MATRIX. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: /ELCOMM/ XS,Q3,PITCH,ROLL,YAW,PMA,RMA,BT */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : INST2ER,GATT */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int lmodel_(t, tu, rec, imc, rlat, rlon)
doublereal *t, *tu, *rec;
integer *imc;
doublereal *rlat, *rlon;
{
    /* System generated locals */
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double sin(), cos(), sqrt(), atan2(), tan(), atan();

    /* Local variables */
    static doublereal dlat;
    extern doublereal gatt_();
    static doublereal slat, dyaw, syaw, b[9]	/* was [3][3] */, r__, u, w, 
	    ca, ci, sa, dr, cu, te, wa, cw, si, ts, su, sw, cw1, c2w, cw3, 
	    sw1, s2w, sw3;
    extern /* Subroutine */ int inst2er_();
    static doublereal asc, lam, phi, psi;


/*     CALLING ARGUMENTS */

/*                                   INPUT TIME FROM 1950, JAN 1.0 (MIN) */
/*                                   EPOCH TIME FROM 1950, JAN 1.0 (MIN) */
/*                                   INPUT O&A PARAMETER SET */
/*                                   INPUT IMC STATUS: 0 - ON, 1 - OFF */
/*                                   SUBSATELLITE GEODETIC LATITUDE (RAD) */
/*                                   SUBSATELLITE LONGITUDE IN RADIANS */

/*     LOCAL VARIABLES */

/*                    NORMALIZED SATELLITE DISTANCE (IN UNITS OF KMER9) */
/*                    TIME FROM EPOCH IN MINUTES */
/*                    SPACCRAFT TO EARTH FIXED COORDINATES TRANSFORMATION */
/*                    MATRIX */
/*                    EXPONENENTIAL TIME DELAY FROM EPOCH (IN MINUTES) */
/*                    SUBSATELLITE GEOCENTRIC LATITUDE IN RADIANS */
/*                    RADIAL DISTANCE FROM THE NOMINAL (KM) */
/*                    ORBITAL YAW (IN RADIANS) */
/*                    IMC LONGITUDE (IN RADIANS) */
/*                    ARGUMENT OF LATITUDE (IN RADIANS) */
/*                    SIN(U), COS(U) */
/*                    SINE AND COSINE OF THE ORBIT INCLINATION */
/*                    SINE OF GEOCENTRIC LATITUDE */
/*                    LONGITUDE OF THE ASCENDING NODE IN RADIANS */
/*                    SINE AND COSINE OF ASC */
/*                    SINE OF THE ORBIT YAW */
/*                    SOLAR ORBIT ANGLE IN RADIANS */
/*                    ORBIT ANGLE IN RADIANS */
/*                    SIN(W),  COS(W) */
/*                    SIN(2*W),  COS(2*W) */
/*                    SIN(0.927*W),  COS(0.927*W) */
/*                    SINE AND COSINE OF 1.9268*W */
/*                    CHANGE IN SINE OF GEOCENTRIC LATITUDE */
/*                    CHANGE IN SINE OF ORBIT YAW */
/*                    SUBROUTINE FUNCTION */
/*                    WORK AREAS */

/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCONS */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCONS.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   MATHEMATICAL AND EARTH-RELATED CONSTANTS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*                    DEGREES TO RADIANS CONVERSION PI/180 */
/*                    NOMINAL RADIAL DISTANCE OF SATELLITE (km) */
/*                    EARTH EQUATORIAL RADIUS (km) */
/*                    EARTH FLATTENING COEFFICIENT = 1-(BE/AE) */

/* *********************************************************************** */

/* *********************************************************************** */

/*     ASSIGN REFERENCE VALUES TO THE SUBSATELLITE LONGITUDE AND */
/*     LATITUDE, THE RADIAL DISTANCE AND THE ORBIT YAW. */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   INSTRUMENT POSITION AND ATTITUDE VARIABLES AND TRANSFORMATION */
/* **   MATRIX */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     COMMON VARIABLES */

/*                      NORMALIZED S/C POSITION IN ECEF COORDINATES */
/*                      ECEF TO INSTRUMENT COORDINATES TRANSFORMATION */
/*                      USED IN SUBROUTINE LPOINT */
/*                          PITCH,ROLL,YAW ANGLES OF INSTRUMENT (RAD) */
/*                          PITCH,ROLL MISALIGNMENTS OF INSTRUMENT (RAD) */
    /* Parameter adjustments */
    --rec;

    /* Function Body */
    lam = rec[5];
    dr = rec[6];
    phi = rec[7];
    psi = rec[8];

/*     ASSIGN REFERENCE VALUES TO THE ATTITUDES AND MISALIGNMENTS */

    elcomm_1.roll = rec[9];
    elcomm_1.pitch = rec[10];
    elcomm_1.yaw = rec[11];
    elcomm_1.rma = (float)0.;
    elcomm_1.pma = (float)0.;

/*     IF IMC IS OFF, COMPUTE CHANGES IN THE SATELLITE ORBIT */

    if (*imc != 0) {

/*     SET REFERENCE RADIAL DISTANCE, LATITUDE AND ORBIT YAW TO ZERO */

	dr = 0.;
	phi = 0.;
	psi = 0.;

/*     COMPUTE TIME SINCE EPOCH (IN MINUTES) */

	ts = *t - *tu;

/*     COMPUTES ORBIT ANGLE AND THE RELATED TRIGONOMETRIC FUNCTIONS. */
/*     EARTH ROTATIONAL RATE=.729115E-4 (RAD/S) */

	w = ts * .0043746900000000005;
	sw = sin(w);
	cw = cos(w);
	sw1 = sin(w * .927);
	cw1 = cos(w * .927);
	s2w = sin(w * 2.);
	c2w = cos(w * 2.);
	sw3 = sin(w * 1.9268);
	cw3 = cos(w * 1.9268);

/*     COMPUTES CHANGE IN THE IMC LONGITUDE FROM THE REFERENCE */

	lam = lam + rec[18] + (rec[19] + rec[20] * w) * w + (rec[27] * sw1 + 
		rec[28] * cw1 + rec[21] * sw + rec[22] * cw + rec[23] * s2w + 
		rec[24] * c2w + rec[25] * sw3 + rec[26] * cw3 + w * (rec[29] *
		 sw + rec[30] * cw)) * 2.;

/*     COMPUTES CHANGE IN RADIAL DISTANCE FROM THE REFERENCE (KM) */

	dr = dr + rec[31] + rec[32] * cw + rec[33] * sw + rec[34] * c2w + rec[
		35] * s2w + rec[36] * cw3 + rec[37] * sw3 + rec[38] * cw1 + 
		rec[39] * sw1 + w * (rec[40] * cw + rec[41] * sw);

/*     COMPUTES THE SINE OF THE CHANGE IN THE GEOCENTRIC LATITUDE */

	dlat = rec[42] + rec[43] * cw + rec[44] * sw + rec[45] * c2w + rec[46]
		 * s2w + w * (rec[47] * cw + rec[48] * sw) + rec[49] * cw1 + 
		rec[50] * sw1;

/*     COMPUTES GEOCENTRIC LATITUDE BY USING AN EXPANSION FOR ARCSINE */

	phi += dlat * (dlat * dlat / 6. + 1.);

/*     COMPUTES SINE OF THE CHANGE IN THE ORBIT YAW */

	dyaw = rec[51] + rec[52] * sw + rec[53] * cw + rec[54] * s2w + rec[55]
		 * c2w + w * (rec[56] * sw + rec[57] * cw) + rec[58] * sw1 + 
		rec[59] * cw1;

/*     COMPUTES THE ORBIT YAW BY USING AN EXPANSION FOR ARCSINE. */

	psi += dyaw * (dyaw * dyaw / 6. + 1.);

/*     CALCULATION OF CHANGES IN THE SATELLITE ORBIT ENDS HERE */

    }

/*     CONVERSION OF THE IMC LONGITUDE AND ORBIT YAW TO THE SUBSATELLITE */
/*     LONGITUDE AND THE ORBIT INCLINATION (REF: GOES-PCC-TM-2473, INPUTS */
/*     REQUIRED FOR EARTH LOCATION AND GRIDDING BY SPS,  JUNE 6, 1988) */

    slat = sin(phi);
    syaw = sin(psi);
/* Computing 2nd power */
    d__1 = slat;
/* Computing 2nd power */
    d__2 = syaw;
    si = d__1 * d__1 + d__2 * d__2;
    ci = sqrt(1. - si);
    si = sqrt(si);
    if (slat == 0. && syaw == 0.) {
	u = 0.;
    } else {
	u = atan2(slat, syaw);
    }
    su = sin(u);
    cu = cos(u);

/*     COMPUTES LONGITUDE OF THE ASCENDING NODE */

    asc = lam - u;
    sa = sin(asc);
    ca = cos(asc);

/*     COMPUTES THE SUBSATELLITE GEOGRAPHIC LATITUDE */

    *rlat = atan(tan(phi) * (float)1.0067396607958714);

/*     COMPUTES THE SUBSATELLITE LONGITUDE */

    *rlon = asc + atan2(ci * su, cu);

/*     COMPUTES THE SPACECRAFT TO EARTH FIXED COORDINATES TRANSFORMATION */
/*     MATRIX: */
/*         (VECTOR IN ECEF COORDINATES) = B * (VECTOR IN S/C COORDINATES) */

    b[3] = -sa * si;
    b[4] = ca * si;
    b[5] = -ci;
    b[6] = -ca * cu + sa * su * ci;
    b[7] = -sa * cu - ca * su * ci;
    b[8] = -slat;
    b[0] = -ca * su - sa * cu * ci;
    b[1] = -sa * su + ca * cu * ci;
    b[2] = cu * si;

/*     COMPUTES THE NORMALIZED SPACECRAFT POSITION VECTOR IN EARTH FIXED */
/*     COORDINATES - XS. */

    r__ = (dr + 42164.365) / 6378.137;
    elcomm_1.xs[0] = -b[6] * r__;
    elcomm_1.xs[1] = -b[7] * r__;
    elcomm_1.xs[2] = -b[8] * r__;

/*     PRECOMPUTES Q3 (USED IN LPOINT) */

/* Computing 2nd power */
    d__1 = elcomm_1.xs[0];
/* Computing 2nd power */
    d__2 = elcomm_1.xs[1];
/* Computing 2nd power */
    d__3 = elcomm_1.xs[2];
    elcomm_1.q3 = d__1 * d__1 + d__2 * d__2 + d__3 * d__3 * (float)
	    1.0067396607958714 - 1.;

/*     COMPUTES THE ATTITUDES AND MISALIGNMENTS IF IMC IS OFF */

    if (*imc != 0) {

/*     COMPUTES THE SOLAR ORBIT ANGLE */

	wa = rec[60] * ts;

/*     COMPUTES THE DIFFERENCE BETWEEN CURRENT TIME, TS, AND THE */
/*     EXPONENTIAL TIME, REC(61). NOTE THAT BOTH TIMES ARE SINCE EPOCH. */

	te = ts - rec[61];

/*     COMPUTES ROLL + ROLL MISALIGNMENT */

	elcomm_1.roll += gatt_(&c__62, &rec[1], &wa, &te);

/*     COMPUTES PITCH + PITCH MISALIGNMENT */

	elcomm_1.pitch += gatt_(&c__117, &rec[1], &wa, &te);

/*     COMPUTES YAW */

	elcomm_1.yaw += gatt_(&c__172, &rec[1], &wa, &te);

/*     COMPUTES ROLL MISALIGNMENT */

	elcomm_1.rma = gatt_(&c__227, &rec[1], &wa, &te);

/*     COMPUTES PITCH MISALIGNMENT */

	elcomm_1.pma = gatt_(&c__282, &rec[1], &wa, &te);

/*     APPLY THE SPCECRAFT COMPENSATION */

	elcomm_1.roll += rec[15];
	elcomm_1.pitch += rec[16];
	elcomm_1.yaw += rec[17];
    }

/*     COMPUTES THE INSTRUMENT TO EARTH FIXED COORDINATES TRANSFORMATION */
/*     MATRIX - BT */

    inst2er_(&elcomm_1.roll, &elcomm_1.pitch, &elcomm_1.yaw, b, elcomm_1.bt);
    return 0;
} /* lmodel_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : LPOINT */
/* **   SOURCE    : LPOINT.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     01/09/89  IL   INITIAL CREATION */
/* **   2     06/02/89  IL   COORDINATE AXES CHANGED ACCORDING TO */
/* **                        FORD'S DEFINITION IN SDAIP, DRL504-01 */
/* **   3     12/01/93  IL   IMPLEMENTED NEW FORMULAE FOR SCAN ANGLE */
/* **                        CORRECTIONS DUE TO MISALIGNMENTS */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   THIS SUBROUTINE CONVERTS THE INSTRUMENT ELEVATION AND SCAN */
/* **   ANGLES TO THE RELATED GEOGRAPHIC LATITUDE AND LONGITUDE. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int lpoint_(instr, flip_flg__, alpha0, zeta0, rlat, rlon, 
	ierr)
integer *instr, *flip_flg__;
real *alpha0, *zeta0, *rlat, *rlon;
integer *ierr;
{
    /* System generated locals */
    real r__1, r__2, r__3;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double cos(), sin(), tan(), sqrt(), atan(), atan2();

    /* Local variables */
    static real doff, zeta;
    static doublereal d__;
    static real g[3];
    static doublereal h__;
    static real u[3], alpha, d1;
    static doublereal g1[3], q1, q2;
    static real ca, da, ff, sa, cz, dz;


/*     CALLING PARAMETERS */

/*                    INSTRUMENT CODE (1=IMAGER,2=SOUNDER) */
/*                   S/C ORIENTATION FLAG (1=NORMAL,-1=INVERTED) */
/*             ELEVATION ANGLE (RAD) */
/*             SCAN ANGLE (RAD) */
/*             LATITUDE IN RADIANS (OUTPUT) */
/*             LONGITUDE IN RADIANS (OUTPUT) */
/*             OUTPUT STATUS; 0 - POINT ON THE EARTH */
/*                            1 - INSTRUMENT POINTS OFF EARTH */

/*     LOCAL VARIABLES */

/*             POINTING VECTOR IN EARTH-CENTERED COORDINATES */
/*             SLANT DISTANCE TO THE EARTH POINT (KM) */
/*             WORK SPACE */
/*             POINTING VECTOR IN INSTRUMENT COORDINATES */
/*             COORDINATES OF THE EARTH POINT (KM) */
/*             WORK SPACE */

/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCONS */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCONS.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   MATHEMATICAL AND EARTH-RELATED CONSTANTS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*                    DEGREES TO RADIANS CONVERSION PI/180 */
/*                    NOMINAL RADIAL DISTANCE OF SATELLITE (km) */
/*                    EARTH EQUATORIAL RADIUS (km) */
/*                    EARTH FLATTENING COEFFICIENT = 1-(BE/AE) */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   INSTRUMENT POSITION AND ATTITUDE VARIABLES AND TRANSFORMATION */
/* **   MATRIX */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     COMMON VARIABLES */

/*                      NORMALIZED S/C POSITION IN ECEF COORDINATES */
/*                      ECEF TO INSTRUMENT COORDINATES TRANSFORMATION */
/*                      USED IN SUBROUTINE LPOINT */
/*                          PITCH,ROLL,YAW ANGLES OF INSTRUMENT (RAD) */
/*                          PITCH,ROLL MISALIGNMENTS OF INSTRUMENT (RAD) */
/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    *ierr = 1;

/*      COMPUTE SIGN OF MISALIGNMENT CORRECTIONS AND ORIGIN OFFSET CORRECTIONS */

    ff = (real) (*flip_flg__);
    if (*instr == 2) {
	ff = -ff;
    }
    doff = instcomm_1.scnmax[*instr - 1] - instcomm_1.ewnom[*instr - 1];

/*     ADD THE NEW SECOND ORDER ORIGIN OFFSET CORRECTION */

    alpha = *alpha0 - *alpha0 * *zeta0 * doff;
    zeta = *zeta0 + *alpha0 * (float).5 * *alpha0 * doff;

/*     COMPUTES TRIGONOMETRIC FUNCTIONS OF THE SCAN AND ELEVATION */
/*     ANGLES CORRECTED FOR THE ROLL AND PITCH MISALIGNMENTS */

    ca = cos(alpha);
    sa = sin(alpha);
    cz = cos(zeta);
    da = alpha - elcomm_1.pma * sa * (ff / cz + tan(zeta)) - elcomm_1.rma * ((
	    float)1. - ca / cz);
    dz = zeta + ff * elcomm_1.rma * sa;

/*    COMPUTES POINTING VECTOR IN INSTRUMENT COORDINATES */

    cz = cos(dz);
    g[0] = sin(dz);
    g[1] = -cz * sin(da);
    g[2] = cz * cos(da);

/*     TRANSFORMS THE POINTING VECTOR TO EARTH-FIXED COORDINATES */

    g1[0] = elcomm_1.bt[0] * g[0] + elcomm_1.bt[3] * g[1] + elcomm_1.bt[6] * 
	    g[2];
    g1[1] = elcomm_1.bt[1] * g[0] + elcomm_1.bt[4] * g[1] + elcomm_1.bt[7] * 
	    g[2];
    g1[2] = elcomm_1.bt[2] * g[0] + elcomm_1.bt[5] * g[1] + elcomm_1.bt[8] * 
	    g[2];

/*     COMPUTES COEFFICIENTS AND SOLVES A QUADRATIC EQUATION TO */
/*     FIND THE INTERSECT OF THE POINTING VECTOR WITH THE EARTH */
/*     SURFACE */

/* Computing 2nd power */
    d__1 = g1[0];
/* Computing 2nd power */
    d__2 = g1[1];
/* Computing 2nd power */
    d__3 = g1[2];
    q1 = d__1 * d__1 + d__2 * d__2 + d__3 * d__3 * (float)1.0067396607958714;
    q2 = elcomm_1.xs[0] * g1[0] + elcomm_1.xs[1] * g1[1] + elcomm_1.xs[2] * (
	    float)1.0067396607958714 * g1[2];
    d__ = q2 * q2 - q1 * elcomm_1.q3;
    if (abs(d__) < 1e-9) {
	d__ = (float)0.;
    }

/*     IF THE DISCIMINANTE OF THE EQUATION, D, IS NEGATIVE, THE */
/*     INSTRUMENT POINTS OFF THE EARTH */

    if (d__ < 0.) {
	*rlat = (float)999999.;
	*rlon = (float)999999.;
	return 0;
    }
    d__ = sqrt(d__);

/*     SLANT DISTANCE FROM THE SATELLITE TO THE EARTH POINT */

    h__ = -(q2 + d__) / q1;

/*     CARTESIAN COORDINATES OF THE EARTH POINT */

    u[0] = elcomm_1.xs[0] + h__ * g1[0];
    u[1] = elcomm_1.xs[1] + h__ * g1[1];
    u[2] = elcomm_1.xs[2] + h__ * g1[2];

/*     SINUS OF GEOCENTRIC LATITUDE */

/* Computing 2nd power */
    r__1 = u[0];
/* Computing 2nd power */
    r__2 = u[1];
/* Computing 2nd power */
    r__3 = u[2];
    d1 = u[2] / sqrt(r__1 * r__1 + r__2 * r__2 + r__3 * r__3);

/*     GEOGRAPHIC (GEODETIC) COORDINATES OF THE POINT */

    *rlat = atan(d1 * (float)1.0067396607958714 / sqrt((float)1. - d1 * d1));
    *rlon = atan2(u[1], u[0]);
    *ierr = 0;
    return 0;
} /* lpoint_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : SCPX */
/* **   SOURCE    : SCPX.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A     09/22/87  IL   INITIAL CREATION */
/* *********************************************************************** */
/* ** */
/* **   THIS FUNCTION CONVERTS FRACTIONAL PIXEL NUMBER TO SCAN ANGLE */
/* **   IN RADIANS. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
doublereal scpx_(instr, pix)
integer *instr;
real *pix;
{
    /* System generated locals */
    real ret_val;


/*  CALLING PARAMETERS */

/*                       INSTRUMENT CODE (1-IMAGER, 2-SOUNDER) */
/*                       FRACTIONAL PIXEL NUMBER */

/*     LOCAL VARIABLES */


/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    ret_val = (*pix - (float)1.) * instcomm_1.scnpx[*instr - 1] - 
	    instcomm_1.scnmax[*instr - 1];
    return ret_val;
} /* scpx_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : SETCONS */
/* **   SOURCE    : SETCONS.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     02/16/89  IL   INITIAL CREATION */
/* **   2     05/27/94  IL   ADDED ARGUMENTS NSCYC1,..,EWINC2 */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   THIS SUBROUTINE GENERATES CONSTANTS IN COMMON  INSTCOMM */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: INSTCOMM */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int setcons_(nscyc1, nsinc1, ewcyc1, ewinc1, nscyc2, nsinc2, 
	ewcyc2, ewinc2)
integer *nscyc1, *nsinc1, *ewcyc1, *ewinc1, *nscyc2, *nsinc2, *ewcyc2, *
	ewinc2;
{
    /* Initialized data */

    static logical first = TRUE_;


/*     CALLING PARAMETERS */

/*                        N-S CYCLES OF THE IMAGER OFFSET */
/*                        N-S INCREMENTS OF THE IMAGER OFFSET */
/*                        E-W CYCLES OF THE IMAGER OFFSET */
/*                        E-W INCREMENTS OF THE IMAGER OFFSET */
/*                        N-S CYCLES OF THE SOUNDER OFFSET */
/*                        N-S INCREMENTS OF THE SOUNDER OFFSET */
/*                        E-W CYCLES OF THE SOUNER OFFSET */
/*                        E-W INCREMENTS OF THE SOUNDER OFFSET */

/*     LOCAL VARIABLES */


/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCONS */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCONS.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   MATHEMATICAL AND EARTH-RELATED CONSTANTS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*                    DEGREES TO RADIANS CONVERSION PI/180 */
/*                    NOMINAL RADIAL DISTANCE OF SATELLITE (km) */
/*                    EARTH EQUATORIAL RADIUS (km) */
/*                    EARTH FLATTENING COEFFICIENT = 1-(BE/AE) */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
/* *********************************************************************** */
    if (first) {
	first = FALSE_;
	instcomm_1.incmax[0] = 6136;
	instcomm_1.incmax[1] = 2805;
	instcomm_1.elvincr[0] = (float).049087378124999997 / 
		instcomm_1.incmax[0];
	instcomm_1.elvincr[1] = (float).049087378124999997 / 
		instcomm_1.incmax[1];
	instcomm_1.scnincr[0] = (float).098174756249999995 / 
		instcomm_1.incmax[0];
	instcomm_1.scnincr[1] = (float).098174756249999995 / 
		instcomm_1.incmax[1];
	instcomm_1.elvln[0] = instcomm_1.elvincr[0] * 28 / 8;
	instcomm_1.elvln[1] = instcomm_1.elvincr[1] * 64 / 4;
	instcomm_1.scnpx[0] = instcomm_1.scnincr[0];
	instcomm_1.scnpx[1] = instcomm_1.scnincr[1] * 8;
	instcomm_1.nsnom[0] = instcomm_1.incmax[0] * (float)4.5 * 
		instcomm_1.elvincr[0];
	instcomm_1.nsnom[1] = instcomm_1.incmax[1] * (float)4.5 * 
		instcomm_1.elvincr[1];
	instcomm_1.ewnom[0] = instcomm_1.incmax[0] * (float)2.5 * 
		instcomm_1.scnincr[0];
	instcomm_1.ewnom[1] = instcomm_1.incmax[1] * (float)2.5 * 
		instcomm_1.scnincr[1];
    }

/*     SET NEW OFFSETS */

    instcomm_1.elvmax[0] = (instcomm_1.incmax[0] * *nscyc1 + *nsinc1) * 
	    instcomm_1.elvincr[0];
    instcomm_1.scnmax[0] = (instcomm_1.incmax[0] * *ewcyc1 + *ewinc1) * 
	    instcomm_1.scnincr[0];
    instcomm_1.elvmax[1] = instcomm_1.nsnom[1] * (float)2. - (
	    instcomm_1.incmax[1] * *nscyc2 + *nsinc2) * instcomm_1.elvincr[1];
    instcomm_1.scnmax[1] = (instcomm_1.incmax[1] * *ewcyc2 + *ewinc2) * 
	    instcomm_1.scnincr[1];
    return 0;
} /* setcons_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : SNDELOC */
/* **   SOURCE    : SNDELOC.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     02/16/89  IL   INITIAL CREATION */
/* **   2     03/09/90  IL   CORRECTED ANGULAR DETECTOR OFFSETS */
/* *********************************************************************** */
/* ** */
/* **   SNDELOC ACCEPTS THE MIRROR POSITION IN CYCLES AND INCREMENTS, */
/* **   SERVO ERROR VALUES, AND THE POSITIONAL OFFSETS FOR FOUR DETECTORS */
/* **   OF A SELECTED SOUNDER CHANNEL AND COMPUTES THE DETECTOR EARTH */
/* **   LOCATIONS IN LATITUDE/LONGITUDE COORDINATES. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : LPOINT */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/* Subroutine */ int sndeloc_(flip, cyew, incew, cyns, incns, svew, svns, 
	doff, geo)
integer *flip, *cyew, *incew, *cyns, *incns;
real *svew, *svns, *doff, *geo;
{
    /* Builtin functions */
    double sin(), cos();

    /* Local variables */
    static real cose, sine, e, h__;
    static integer i__;
    static real s, de, sc, ds, ev;
    extern /* Subroutine */ int lpoint_();
    static integer ier;


/*     CALLING PARAMETERS */

/*                   S/C ORIENTATION FLAG (1=NORMAL,-1=INVERTED) */
/*                         E-W CYCLES */
/*                         E-W INCREMENTS */
/*                         N-S CYCLES */
/*                         N-S INCREMENTS */
/*                         E-W SERVO ERROR IN RADIANS */
/*                         N-S SERVO ERROR IN RADIANS */
/*                     OFFSETS FOR 4 DETECTORS (RADIANS) */
/*                          DOFF(*,1) = E-W OFFSET */
/*                          DOFF(*,2) = N-S OFFSET */
/*                     NOTE: OFFSETS ARE GIVEN RELATIVE TO NOMINAL */
/*                           POSITIONS OF DETECTORS */
/*                     GEOGRAPHIC COORDINATES RELATED TO 4 DETECTORS */
/*                           GEO(*,1) = LATITUDE IN RADIANS */
/*                           GEO(*,2) = LONGITUDE IN RADIANS */

/*     LOCAL VARIABLES */


/*     INCLUDE FILES */

/* *********************************************************************** */

/*     CONVERT THE MIRROR POSITION, GIVEN IN CYCLES AND INCREMENTS, TO */
/*     ELEVATION AND SCAN ANGLES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
    /* Parameter adjustments */
    geo -= 5;
    doff -= 5;

    /* Function Body */
    if (*flip == 1) {
	e = instcomm_1.elvmax[1] + ((*cyns - 9) * instcomm_1.incmax[1] + *
		incns) * instcomm_1.elvincr[1] + *svns;
	s = (*cyew * instcomm_1.incmax[1] + *incew) * instcomm_1.scnincr[1] - 
		instcomm_1.scnmax[1] + *svew;
    } else {
	e = instcomm_1.elvmax[1] - (*cyns * instcomm_1.incmax[1] + *incns) * 
		instcomm_1.elvincr[1] - *svns;
	s = ((5 - *cyew) * instcomm_1.incmax[1] - *incew) * 
		instcomm_1.scnincr[1] - instcomm_1.scnmax[1] - *svew;
    }

/*     CORRECT ELEVATION AND SCAN ANGLES FOR SERVO ERRORS OBTAINING THE */
/*     TRUE MIRROR POINTING */

    sine = *flip * sin(e);
    cose = cos(e);
    h__ = instcomm_1.scnpx[1] * (float)-2.;

/*     COMPUTE EARTH LOCATIONS FOR FOUR DETECTORS */

    for (i__ = 1; i__ <= 4; ++i__) {

/*     COMPUTE POSITIONAL OFFSETS OF I-TH DETECTOR */

	de = ((float)2.5 - i__) * instcomm_1.elvln[1] + doff[i__ + 8];
	ds = h__ + doff[i__ + 4];

/*     CONVERT POSITIONAL OFFSETS TO ANGULAR OFFSETS AND */
/*     CORRECT ELEVATION AND SCAN ANGLES */

	ev = e + de * cose + ds * sine;
	sc = s - de * sine + ds * cose;

/*     TRANSFORM DETECTOR'S POINTING ANGLES TO GEOGRAPHIC COORDINATES */
/*     OF THE CORRESPONDING POINT ON THE EARTH SURFACE. */
/*     NOTE:  IF A DETECTOR LOOKS OFF THE EARTH, THE RELATED LATITUDE */
/*            AND LONGITUDE ARE SET TO 999999. */

	lpoint_(&c__2, flip, &ev, &sc, &geo[i__ + 4], &geo[i__ + 8], &ier);
	h__ = -h__;
/* L10: */
    }
    return 0;
} /* sndeloc_ */

/* *********************************************************************** */
/* carueda 09/16/02 */
/* TIME50 source is not in the book. The code below is copied from: */
/*  ftp://rsd.gsfc.nasa.gov/pub/goesbook/nav_FORT */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   NOAA/NESDIS/SOCC/SOFTWARE BRANCH */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : TIME50 */
/* **   SOURCE    : F.TIME50 */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: THOMAS I. BABICKI */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A      2/17/89  TB   INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   TIME50 ACCEPTS TWO WORDS CONTAINING DATE AND TIME */
/* **   AND RETURNS TIME EXPRESSED AS DOUBLE PRECISION MINUTES FROM */
/* **   1950 JAN. 1.0 */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : ANY */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : NONE */
/* ** */
/* *********************************************************************** */
doublereal time50_(i__)
integer *i__;
{
    /* Format strings */
    static char fmt_1000[] = "(\0021\002,\002YEAR =\002,i4,/,1x,\002JDAY \
=\002,i3,/,1x,\002HOUR =\002,i2,/,1x,\002MIN  =\002,i2,/,1x,\002SEC  =\002,f\
6.3)";

    /* System generated locals */
    real ret_val;

    /* Builtin functions */
    integer s_wsfe(), do_fio(), e_wsfe();

    /* Local variables */
    static integer j;
    static doublereal s;
    static integer nd, nh, nm, ny, iaa, iab, iac;
    static real def;
    static integer nbc;

    /* Fortran I/O blocks */
    static cilist io___99 = { 0, 6, 0, fmt_1000, 0 };



/*     CALLING PARAMETERS */


/*     LOCAL VARIABLES */

/*                    YEAR */
/*                    DAY OF YEAR */
/*                    HOUR */
/*                    MINUTE */
/*                    SECONDS */

/*     INCLUDE FILES - REC */


/* *********************************************************************** */

/*     CONVERT INPUT YEAR, DAY OF YEAR, HOUR AND MINUTE */
/*     TO INTEGER VALUES. */

    /* Parameter adjustments */
    --i__;

    /* Function Body */
    ny = i__[1] / 10000;
    iaa = i__[1] - ny * 10000;
    nd = (i__[1] - ny * 10000) * (float).1;
    iab = (iaa - nd * 10) * 10;
    nbc = i__[2] / (float)1e7;
    iac = i__[2] - nbc * 10000000;
    nh = iab + nbc;
    def = (real) (i__[2] - iac);
    nm = iac * (float)1e-5;
    s = (i__[2] - (def + nm * 100000)) * .001;
    s_wsfe(&io___99);
    do_fio(&c__1, (char *)&ny, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nd, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nh, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&nm, (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&s, (ftnlen)sizeof(doublereal));
    e_wsfe();

/* *********************************************************************** */

/*     HERE WE CONVERT INTEGER YEAR AND DAY OF YEAR TO NUMBER OF */
/*     DAYS FROM 0 HOUR UT, 1950 JAN. 1.0 */
/*     THIS CONVERTION IS BASED ON AN ALGORITHM BY FLIEGEL AND VAN */
/*     FLANDERN, COMM. OF ACM, VOL.11, NO. 10, OCT. 1968 (P.657) */

    j = nd + (ny + 4799) * 1461 / 4 - (ny + 4899) / 100 * 3 / 4 - 2465022;

/*     COMPUTE TIME IN MINUTES FROM JANUARY 1.0, 1950 */

    ret_val = j * 1440. + nh * 60. + nm + s / 60.;
    return ret_val;
} /* time50_ */

/* *********************************************************************** */
/* carueda 09/16/02 */
/* TIMEX source is not in the book. The code below is copied from: */
/*  ftp://rsd.gsfc.nasa.gov/pub/goesbook/nav_FORT */
/* *********************************************************************** */
/* *********************************************************************** */
/* **   NOAA/NESDIS/SOCC/SOFTWARE BRANCH */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   ROUTINE   : TIMEX */
/* **   SOURCE    : F.TIMEX */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: THOMAS I. BABICKI */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   A      4/25/89  TB   INITIAL CREATION */
/* *********************************************************************** */
doublereal timex_(ny, nd, nh, nm, s)
integer *ny, *nd, *nh, *nm;
doublereal *s;
{
    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    static integer j;



/*     CALLING PARAMETERS */

/*                    YEAR */
/*                    DAY OF YEAR */
/*                    HOUR */
/*                    MINUTE */
/*                    SECONDS */

/* *********************************************************************** */

/* C      PRINT 1000,NY,ND,NH,NM,S */
/* C 1000 FORMAT (/,1X,'YEAR =',I4,/,1X,'JDAY =',I3,/,1X, */
/* C     *            'HOUR =',I2,/,1X,'MIN  =',I2,/,1X, */
/* C     *            'SEC  =',F6.3) */

/* *********************************************************************** */

    j = *nd + (*ny + 4799) * 1461 / 4 - (*ny + 4899) / 100 * 3 / 4 - 2465022;

/*     COMPUTE ACTUAL TIME IN MINUTES FROM JANUARY 1.0, 1950 */

    ret_val = j * 1440. + *nh * 60. + *nm + *s / 60.;
    return ret_val;
} /* timex_ */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   PROGRAM   : TEST */
/* **   SOURCE    : TEST.FOR */
/* **   LOAD NAME : ANY */
/* **   PROGRAMMER: IGOR LEVINE */
/* ** */
/* **   VER.    DATA    BY   COMMENT */
/* **   ----  --------  ---  --------------------------------------------- */
/* **   1     10/03/89  IL   INITIAL CREATION */
/* **   2     06/06/94  IL   NOW SETCONS IS CALLED WITH 8 ARGUMENTS */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   THIS PROGRAM GENERATES TEST CASES TO VALIDATE THE USER'S */
/* **   IMPLEMENTATIONS OF SOFTWARE FOR TRANSFORMATIONS BETWEEN */
/* **   THE LATITUDE/LONGITUDE AND LINE/PIXEL COORDINAT SYSTEMS. */
/* **   THE RELATED O&A PARAMETER SET IS DEFINED IN INCLUDE FILE I.ELREC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   CALLED BY       : NONE */
/* **   COMMONS MODIFIED: NONE */
/* **   INPUTS          : NONE */
/* **   OUTPUTS         : NONE */
/* **   ROUTINES CALLED : SETCONS,LMODEL,EVLN,SCPX,LPOINT, */
/* **                     GPOINT,EVSC2LPF,SNDELOC */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
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



/*     CALLING PARAMETERS */


/*     LOCAL VARIABLES */


/*     INCLUDE FILES */

/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCONS */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCONS.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   MATHEMATICAL AND EARTH-RELATED CONSTANTS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*                    DEGREES TO RADIANS CONVERSION PI/180 */
/*                    NOMINAL RADIAL DISTANCE OF SATELLITE (km) */
/*                    EARTH EQUATORIAL RADIUS (km) */
/*                    EARTH FLATTENING COEFFICIENT = 1-(BE/AE) */

/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : INSTCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : INSTCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     02/16/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   COMMON AREA FOR INSTRUMENT-RELATED CONTROL PARAMETERS */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     VARIABLES */
/*        CONSTANTS NEEDED TO PERFORM TRANSFORMATIONS BETWEEN THE */
/*        LATITUDE/LONGITUDE, LINE/PIXEL AND INSTRUMENT CYCLES/INCREMENTS */
/*        COORDINATES. */

/*                       NUMBER OF INCREMENTS PER CYCLE */
/*                       BOUNDS IN ELEVATION (RADIANS) */
/*                       BOUNDS IN SCAN ANGLE (RADIANS) */
/*                       CHANGE IN ELEVATION ANGLE PER INCREMENT (RAD) */
/*                       CHANGE IN SCAN ANGLE PER INCREMENT (RADIANS) */
/*                       ELEVATION ANGLE PER DETECTOR LINE (RADIANS) */
/*                       SCAN ANGLE PER PIXEL (RADIANS) */
/*                       EW CENTER OF INSTRUMENT */
/*                       NS CENTER OF INSTRUMENT */
/* *********************************************************************** */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : ELCOMM */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELCOMM.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/09/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   INSTRUMENT POSITION AND ATTITUDE VARIABLES AND TRANSFORMATION */
/* **   MATRIX */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */

/*     COMMON VARIABLES */

/*                      NORMALIZED S/C POSITION IN ECEF COORDINATES */
/*                      ECEF TO INSTRUMENT COORDINATES TRANSFORMATION */
/*                      USED IN SUBROUTINE LPOINT */
/*                          PITCH,ROLL,YAW ANGLES OF INSTRUMENT (RAD) */
/*                          PITCH,ROLL MISALIGNMENTS OF INSTRUMENT (RAD) */
/*      INCLUDE 'SETCONS.FOR' */
/* *********************************************************************** */
/* *********************************************************************** */
/* ** */
/* **   INTEGRAL SYSTEMS, INC. */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   PROJECT   : OPERATIONS GROUND EQUIPMENT FOR GOES-NEXT */
/* **   SYSTEM    : EARTH LOCATION USERS GUIDE */
/* **   NAME      : REC */
/* **   TYPE      : DATA AREA */
/* **   SOURCE    : ELREC.INC */
/* ** */
/* **   VER.    DATA    BY                COMMENT */
/* **   ----  --------  ----------------  -------------------------------- */
/* **   A     01/19/89  I. LEVINE         INITIAL CREATION */
/* ** */
/* *********************************************************************** */
/* ** */
/* **   DESCRIPTION */
/* **   TEST SET OF O&A PARAMETERS IN THE GVAR BLOCK 0 FORMAT */
/* ** */
/* *********************************************************************** */
/* *********************************************************************** */
/*   CONTENT OF CORRESPONDING WORDS: */
/*     REC(5) - REFERENCE LONGITUDE */
/*     REC(6) - DISTANCE FROM NOMINAL */
/*     REC(7) - REFERENCE LATITUDE */
/*     REC(8) - REFERENCE ORBIT YAW */
/*     REC(9) - REFERENCE ROLL */
/*     REC(10) - REFERENCE PITCH */
/*     REC(11) - REFERENCE YAW */
/*     REC(12),REC(13) - EPOCH TIME IN THE BCD FORMAT /YYYYDDDH HMMSSLLL/ */
/*                       WHERE Y=YEAR, D=DAY, H=HOUR, M=MINUTE, */
/*                             S=SECONDS, L=MILLISECONDS. */
/*     REC(14) - UNUSED */
/*     REC(15) - SPACECRAFT COMPENSATION, ROLL */
/*     REC(16) - SPACECRAFT COMPENSATION, PITCH */
/*     REC(17) - SPACECRAFT COMPENSATION, YAW */
/*     REC(62) - ROLL COEFFICIENTS BEGIN HERE */
/*     REC(117) - PITCH COEFFICIENTS BEGIN HERE */
/*     REC(172) - YAW COEFFICIENTS BEGIN HERE */
/*     REC(227) - ROLL MISALIGNMENT COEFFICIENTS BEGIN HERE */
/*     REC(282) - PITCH MISALIGNMENT COEFFICIENTS BEGIN HERE */
/* *********************************************************************** */
/*  REFER.  LON=-100.1189, LAT=-1.9813, ORB.YAW=-.34967 deg, DIST=84.066 */
/*  EPOCH  02/01/89  6:29:34.567 */
/*  SPECECRAFT COMPENSATION */
/*  COEFFICIENTS FOR CHANGES IN LON, LAT, RADIAL DISTANCE AND ORBIT YAW */
/*  DAILY SOLAR RATE (rad/min) AND EXPONENTIAL START TIME FROM EPOCH */
/*  ROLL COEFFICIENTS BEGIN HERE (FROM WORD 62): */
/*  PITCH COEFFICIENTS BEGIN HERE (FROM WORD 117): */
/*  YAW COEFFICIENTS BEGIN HERE (FROM WORD 172): */
/*  RMA COEFFICIENTS BEGIN HERE (FROM WORD 227): */
/*  PMA COEFFICIENTS BEGIN HERE (FROM WORD 282): */
/* *********************************************************************** */
/* *********************************************************************** */

/*     EXPECTED RESULTS: */
/*     ******** ******** */

/*     EPOCH TIME = 20557829.57612 */

/*   NORMAL SPACECRAFT */

/*     IMC = 0 (ENABLED)  SUBSATELLITE LAT =-1.9824   LON =-100.1249 */

/*     INSTRUMENT = 1   (IMAGER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT = 50.0000    LON =-150.0000 */
/*         N-S = 7.0688     E-W =-4.5246 */
/*         LINE =  3487.36  PIXEL = 10405.39 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  3487.36  PIXEL = 10405.39 */
/*         N-S = 7.0688     E-W =-4.5246 */
/*         LAT = 50.0000    LON =-150.0000 */

/*     INSTRUMENT = 2   (SOUNDER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT =-50.0000    LON = -50.0000 */
/*         N-S =-6.8659     E-W = 4.5781 */
/*         LINE =  1219.41  PIXEL =  1162.87 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  1219.41  PIXEL =  1162.87 */
/*         N-S =-6.8659     E-W = 4.5781 */
/*         LAT =-50.0000    LON = -50.0000 */


/*     IMC = 1 (DISABLED)  SUBSATELLITE LAT = 0.0509   LON =-100.0017 */

/*     INSTRUMENT = 1   (IMAGER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT = 50.0000    LON =-150.0000 */
/*         N-S = 6.8594     E-W =-4.6513 */
/*         LINE =  3617.92  PIXEL = 10267.15 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  3617.92  PIXEL = 10267.15 */
/*         N-S = 6.8594     E-W =-4.6513 */
/*         LAT = 49.9999    LON =-149.9997 */

/*     INSTRUMENT = 2   (SOUNDER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT =-50.0000    LON = -50.0000 */
/*         N-S =-7.1650     E-W = 4.3902 */
/*         LINE =  1238.05  PIXEL =  1151.16 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  1238.05  PIXEL =  1151.16 */
/*         N-S =-7.1650     E-W = 4.3902 */
/*         LAT =-49.9999    LON = -50.0003 */

/*       CYCLES/INCREMENTS TO LAT/LON TRANSFORMATION: */
/*         E-W CYCLES/INCREMENTS = 1, 2715 */
/*         N-S CYCLES/INCREMENTS = 5, 2580 */
/*         E-W SERVO ERROR = -21.00 (mrad) */
/*         N-S SERVO ERROR =  14.00 (mrad) */

/*         DETECTOR      #1      #2      #3      #4 */
/*         E-W OFFSET   28.00   56.00  -28.00  -56.00 (mrad) */
/*         N-S OFFSET   84.00  112.00   14.00   42.00 (mrad) */

/*         DETECTOR #1   LAT = 25.1035  LON =-118.8478 */
/*         DETECTOR #2   LAT = 25.0270  LON =-118.3774 */
/*         DETECTOR #3   LAT = 24.8625  LON =-118.8069 */
/*         DETECTOR #4   LAT = 24.7853  LON =-118.3595 */

/*   FLIPPED (INVERTED SPACECRAFT */

/*     IMC = 0 (ENABLED)  SUBSATELLITE LAT =-1.9824   LON =-100.1249 */

/*     INSTRUMENT = 1   (IMAGER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT = 50.0000    LON =-150.0000 */
/*         N-S = 7.0688     E-W =-4.5246 */
/*         LINE =  3487.36  PIXEL = 10405.39 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  3487.36  PIXEL = 10405.39 */
/*         N-S = 7.0688     E-W =-4.5246 */
/*         LAT = 50.0000    LON =-150.0000 */

/*     INSTRUMENT = 2   (SOUNDER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT =-50.0000    LON = -50.0000 */
/*         N-S =-6.8659     E-W = 4.5780 */
/*         LINE =  1219.35  PIXEL =  1162.99 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  1219.35  PIXEL =  1162.99 */
/*         N-S =-6.8659     E-W = 4.5780 */
/*         LAT =-50.0000    LON = -50.0000 */


/*     IMC = 1 (DISABLED)  SUBSATELLITE LAT = 0.0509   LON =-100.0017 */

/*     INSTRUMENT = 1   (IMAGER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT = 50.0000    LON =-150.0000 */
/*         N-S = 6.8450     E-W =-4.6370 */
/*         LINE =  3626.88  PIXEL = 10282.76 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  3626.88  PIXEL = 10282.76 */
/*         N-S = 6.8450     E-W =-4.6370 */
/*         LAT = 49.9998    LON =-149.9996 */

/*     INSTRUMENT = 2   (SOUNDER) */

/*       LAT/LON TO LINE/PIXEL TRANSFORMATION: */
/*         LAT =-50.0000    LON = -50.0000 */
/*         N-S =-7.1800     E-W = 4.4052 */
/*         LINE =  1238.93  PIXEL =  1152.22 */
/*       LINE/PIXEL TO LAT/LON TRANSFORMATION: */
/*         LINE =  1238.93  PIXEL =  1152.22 */
/*         N-S =-7.1800     E-W = 4.4052 */
/*         LAT =-49.9998    LON = -50.0003 */

/*       CYCLES/INCREMENTS TO LAT/LON TRANSFORMATION: */
/*         E-W CYCLES/INCREMENTS = 1, 2715 */
/*         N-S CYCLES/INCREMENTS = 5, 2580 */
/*         E-W SERVO ERROR = -21.00 (mrad) */
/*         N-S SERVO ERROR =  14.00 (mrad) */

/*         DETECTOR      #1      #2      #3      #4 */
/*         E-W OFFSET   28.00   56.00  -28.00  -56.00 (mrad) */
/*         N-S OFFSET   84.00  112.00   14.00   42.00 (mrad) */

/*         DETECTOR #1   LAT =-22.5543  LON = -80.4361 */
/*         DETECTOR #2   LAT =-22.6288  LON = -79.9716 */
/*         DETECTOR #3   LAT =-22.7889  LON = -80.3995 */
/*         DETECTOR #4   LAT =-22.8645  LON = -79.9554 */

/*     NOTE: LAT, LON, AND N-S, E-W ANGLES ARE IN DEGREES. */

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
