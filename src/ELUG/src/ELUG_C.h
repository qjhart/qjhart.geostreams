#if defined __cplusplus
extern  "C" {
#endif

//#include "f2c.h"


double evln_(int *instr, float *rline);
int evsc2lpf_(int *instr, float *elev, float *scan, float *rl, float *rp);
double gatt_(int *k0, float *rec, double *wa, double *te);
int gpoint_(int *instr, int *flip_flg__, float *rlat, float *rlon, 
	    float *alf, float *gam, int *ierr);
int inst2er_(float *r__, float *p, float *y, double *a, double *at);
int lmodel_(double *t, double *tu, float *rec, int *imc, double *rlat, double *rlon);
int lpoint_(int *instr, int *flip_flg__, float *alpha0, float *zeta0, 
	    float *rlat, float *rlon, int *ierr);
double scpx_(int *instr, float *pix);
int setcons_(int *nscyc1, int *nsinc1, int *ewcyc1, int *ewinc1, int *nscyc2, 
	     int *nsinc2, int *ewcyc2, int *ewinc2);
int sndeloc_(int *flip, int *cyew, int *incew, int *cyns, int *incns, float *svew, 
	     float *svns, float *doff, float *geo);
double time50_(int *i__);
double timex_(int *ny, int *nd, int *nh, int *nm, double *s);
/* comlen instcomm_ 72 */
/* comlen elcomm_ 124 */


#if defined __cplusplus
}
#endif
