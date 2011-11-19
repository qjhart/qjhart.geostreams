/** Elug provides methods to convert between 
 *  lon/lat and GOES pixel/line
 */

#include "ELUG_C.h"

#define PI 3.1415926
inline double rad2deg(double angle) {
  return 180.0 * angle / PI;
}

inline double deg2rad(double deg) {
  return deg * PI / 180.0; 
}

/**
 *  Follow the Earth Location User's Guide (ELUG),
 *  provide methods to do point2point conversion   
 *  between lat/lon and GOES line/pixel 
 *
 *  Elug calls functions provided by ELUG_C.c,
 *  which was generated from ELUG Fortran code by f2c
 *  then changes were made to ELUG_C.c to remove
 *  all connections to f2c library. Thus ELUG_C.c
 *  becomes a STANDALONE c program.
 */
class Elug{
	int flipflag;   // 1=normal    -1=inverted
	int imc;          // 1=disaled 0=enabled
	int instr;       // 1=imager   2=sounder

	double subsat_lat;
	double subsat_lon;
        double t;
	double tu; 
	int model;
	float elev;
	float scan;
	float rrlat;
	float rrlon;
	int ierr;

	float rec[336];

  public:

    /** If user doesn't specify the Rec array, 
     *  a default Rec for test purpose will be
     *	set in the constructor
     */
    Elug();

    /**
     * Set constants in common INSTCOMM
     * 
     * Attention: if flipflag is changed, then 
     * setcons() has to be called again. Different
     * parameters have to be passed into it.
     *      @return 0 if OK
     */
     int setcons(int nscyc1, int nsinc1, 
		  int ewcyc1, int ewinc1, int nscyc2, 
		 int nsinc2, int ewcyc2, int ewinc2);


     /**  
      *     @param Rec an array of 336 O&A 
      *      variables from GVAR block 0  
      */
     void setRec(float* Rec);

     /** Remember to reset setcons() if it's 
      *  called in the middle of some program
      */
     void setFlipflag(int Flipflag);

     /** Imc = 0 enabled
      *  Imc = 1 disabled
      */
     void setImc(int Imc);

     /**  @param Instr 1=Imager 2=Sounder */ 
     void setInstr(int Instr);

     /** compute time 
      *  needs to be fixed
      */
     double timex(int ny, int nd, int nh, 
	int nm, double s) ;

     double time50(int *rec12);

     /** subsatellite lat/lon is obtained
      *  after this method is called
      *    @param t  = Time
      *    @param tu = EpochTime
      */
     int lmodel(double t, double tu, int imc) ;

     /**  
      *    @return elev value
      */
     float evln(int instr, float line); 

     /**  
      *    @return scan value
      */
     float scpx(int instr, float pixel); 


     /** from pixel/line to scan/elev 
      *    @param ev: elev by this method
      *    @param sc: scan by this method
      */
     void pl2se(int instr, float pixel, 
		float line, float *ev, float *sc);

     
     /** from scan/elev to lon/lat
      *    @param rlat = lat in radiant
      *    @param rlon = lon in radiant
      *    @return 0 if OK else error
      */
     int se2ll(int instr, int flipflag, 
	       float scan, float elev, 
	       float *rlon, float *rlat);


     /** from lat/lon to scan/elev
      *    @param sc: scan by this method
      *    @param ev: elev by this method 
      *    @return 0 if OK else error
      */
     int ll2se(int instr, int flipflag, 
	       float rlon, float rlat, 
	       float *ev, float *sc);
     

     /** from scan/elev to pixel/line 
      *    @return 0 if OK
      */ 
     void se2pl(int instr, 
		float scan, float elev, 
		float *pixel, float *line);    



     /** from pixel/line to lon/lat
      *    @return 0 if OK else error
      */  
     int pl2ll(float pixel, float line, 
	       float *rlon, float *rlat);

     /** from lon/lat to pixel/line
      *    @return 0 if OK else error
      */
     int ll2pl(float rlon, float rlat, 
	       float *pixel, float *line);


     float getEvln(int, float) ;
     float getScpx(int, float) ;

     double getSubsatLat() ;
     double getSubsatLon() ;

     int getFlipflag();

     int getImc();

     int getInstr();

     ~Elug();

};
