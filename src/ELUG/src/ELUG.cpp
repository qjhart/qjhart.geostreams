/*** Elug provides methods to convert between lat/lon and GOES line/pixel ***/
#include <iostream>
#include <stdlib.h>
#include "ELUG_C.h"
#include "ELUG.h"


using namespace std;

static float   one_rec[] = {
          0.00000000,
          0.00000000,
          0.00000000,
          0.00000000,
         -1.74740505,
         84.06604004,
         -0.03436849,
         -0.00610298,
          0.00000000,
          0.00000000,
          0.00000000,
  428409632.00000000,
 1653818727.00000000,
          0.00000000,
          0.00030000,
         -0.00030000,
         -0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00020000,
          0.00436300,
          0.00000000,
          0.00050000,
        100.00000000,
          0.00200000,
         15.00000000,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          4.00000000,
          2.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          2.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          0.00050000,
        100.00000000,
          0.00200000,
         15.00000000,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          4.00000000,
          2.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          2.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          0.00050000,
        100.00000000,
          0.00100000,
         15.00000000,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          4.00000000,
          2.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          2.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
         -0.00005000,
         10.00000000,
          0.00100000,
         15.00000000,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          4.00000000,
          2.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          2.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
         -0.00005000,
         10.00000000,
          0.00100000,
         15.00000000,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          0.00000500,
          4.00000000,
          2.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          2.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          2.00000000,
          0.00001000,
          0.00000000,
          0.01000000,
          3.00000000,
          3.00000000,
         -0.00001000,
          0.00000000,
          0.01000000
};

Elug::Elug() {
  setRec(one_rec);
}

/*** SET CONSTANTS IN COMMON INSTCOMM ***/
int Elug::setcons(int nscyc1, int nsinc1, 
		  int ewcyc1, int ewinc1, int nscyc2, 
		  int nsinc2, int ewcyc2, int ewinc2){
 
  return setcons_(&nscyc1, &nsinc1, &ewcyc1, &ewinc1, 
		  &nscyc2, &nsinc2, &ewcyc2, &ewinc2);
}

void Elug::setRec(float* Rec){
  for ( int i = 0; i < 336; i++ )
    rec[i] = Rec[i];
}

void Elug::setFlipflag(int Flipflag){
  flipflag = Flipflag;
}

void Elug::setImc(int Imc){
  imc = Imc;
}

void Elug::setInstr(int Instr){
  instr = Instr;
}


/*** compute time ***/
double Elug::timex(int ny, int nd, int nh, 
		       int nm, double s) {
  return timex_(&ny, &nd, &nh, &nm, &s);
}

double Elug:: time50(int *rec12) {
  return time50_(rec12);
}

int Elug::lmodel(double t, double tu, int imc) {

  int ier = lmodel_(&t, &tu, rec, &imc, &subsat_lat, &subsat_lon);
  return ier;
}



float Elug::evln(int instr, float line) {
  return (float) evln_(&instr,&line);
}

float Elug::scpx(int instr, float pixel) {
  return (float) scpx_(&instr,&pixel);
}

void Elug::pl2se(int instr, float pixel, float line, float *ev, float *sc) {
  *ev = evln(instr, line);
  *sc = scpx(instr, pixel);

  elev = *ev;
  scan = *sc;
}


int Elug::se2ll(int instr, int flipflag, float scan, float elev, 
		float *rlon, float *rlat) {
  int ierr;
  lpoint_(&instr, &flipflag, &elev, &scan, rlat, rlon, &ierr);

  return ierr;
}


int Elug::ll2se(int instr, int flipflag, float rlon, float rlat, 
	float *ev, float *sc) {
  int ierr;
  gpoint_(&instr, &flipflag, &rlat, &rlon, ev, sc, &ierr);
  
  elev = *ev;
  scan = *sc;

  return ierr;
}
                                                                                                     
void Elug::se2pl(int instr, float scan, float elev, 
	float *pixel, float *line) {
  evsc2lpf_(&instr, &elev, &scan, line, pixel);
}

float Elug::getEvln(int instr, float line) {
  return elev;
}

float Elug::getScpx(int instr, float pixel) {
  return scan;
}



int Elug::pl2ll(float pixel, float line, float *rlon, float *rlat) {
  float elev;
  float scan;
  int ierr;

  pl2se(instr, pixel, line, &elev, &scan); 

  ierr = se2ll(instr, flipflag, scan, elev, rlon, rlat);

  return ierr;
}

int Elug::ll2pl(float rlon, float rlat, float *pixel, float *line) {
  float elev;
  float scan;
  int ierr;

  ierr = ll2se(instr, flipflag, rlon, rlat, &elev, &scan);
  //cout<<"elev = "<<elev <<" scan = "<<scan<<endl;

  if(ierr == 0) {
	se2pl(instr, scan, elev, pixel, line);
  }
  return ierr;
}



int Elug::getFlipflag(){
  return flipflag;
}

int Elug::getImc(){
  return imc;
}

int Elug::getInstr(){
  return instr;
}

double Elug::getSubsatLat() {
  return subsat_lat; 
}

double Elug::getSubsatLon() {
  return subsat_lon;
}

Elug::~Elug(){
}



