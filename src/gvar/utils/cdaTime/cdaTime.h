#ifndef CDATIME_H
#define CDATIME_H
/* To interpret CDA time tags. See goes i-m oge specs, pg 69.
 */

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>

#include "include/types.h"

using namespace std ;

class CdaTime{ 
 private: 
  unsigned yr1000:4;		/*w1 0 3*/
  unsigned yr100:4;		/*w1 4 7*/ 

  unsigned yr10:4;		/*w2 0 3*/ 
  unsigned yr1:4;		/*w2 4 7*/ 

  unsigned flywheel:1;		/*w3 0  */ 
  unsigned doy100:3;		/*w3 1 3*/ 
  unsigned doy10:4;		/*w3 4 7*/ 

  unsigned doy1:4;		/*w4 0 3*/ 
  unsigned hr10:4;		/*w4 4 7*/ 

  unsigned hr1:4;		/*w5 0 3*/ 
  unsigned min10:4;		/*w5 4 7*/ 

  unsigned min1:4;		/*w6 0 3*/ 
  unsigned sec10:4;		/*w6 4 7*/ 

  unsigned sec1:4;		/*w7 0 3*/ 
  unsigned msec100:4;		/*w7 4 7*/ 

  unsigned msec10:4;		/*w8 0 3*/ 
  unsigned msec1:4;		/*w8 4 7*/ 
 public:
  CdaTime(){ };
  CdaTime(int * in);
  CdaTime(CdaTime & out);

  void setTime (uchar8* in) ;
  int msec();
  int  sec();
  int  min();
  int  hrs();
  int  day();
  int year();
  int time_code_generator_state();
  void print( ostream & out) ; 
  void timeStr( char * ); 

  friend ostream& operator << (ostream & out, CdaTime & t);
  long int TimDifSec(CdaTime & t2);
  
  void toIEEE(int *);
  void toMcIDAS(int * );
};


inline int CdaTime::msec()
{ return(100 * msec100 + 10 * msec10 + msec1 );}

inline int CdaTime::sec()
{ return( 10 * sec10 + sec1 );}

inline int CdaTime::min()
{ return( 10 * min10 + min1 );}

inline int CdaTime::hrs()
{ return( 10 * hr10  + hr1  );}

inline int CdaTime::day()
{ return(100 * doy100 + 10 * doy10 + doy1 );}

inline int CdaTime::year()
{ return(1000* yr1000 + 100* yr100 + 10* yr10  +  yr1 );}

inline int CdaTime::time_code_generator_state()
{ return ( flywheel); }

#endif /* CDATIME_H */


