#include "cdaTime.h"

CdaTime::CdaTime(int * in){ 
  memcpy(this, in, sizeof(CdaTime));
}

ostream& operator << (ostream & out,  CdaTime & t)
{ 
  struct tm time_str; 
  memset(&time_str, 0, sizeof(time_str));
  time_str.tm_yday = t.day();
  // was: year 2000 becomes 100
  //time_str.tm_year = (t.year()-1900 );
  // now: year 2000 becomes 00
  time_str.tm_year = t.year () -1900 ; //(t.year()%100);
  time_str.tm_hour = t.hrs () ;
  time_str.tm_min = t.min () ;
  time_str.tm_sec = t.sec () ;

  mktime(&time_str);

  //Year
  out.fill('0');
  out.width(2);
  out << time_str.tm_year;
  // Month
  out.width(2);
  out.fill('0');
  out << time_str.tm_mon+1;
  // Day 
  out.width(2);
  out.fill('0');
  out << time_str.tm_mday;
  //Hour
  out.width(2);
  out.fill('0');
  out << time_str.tm_hour ;
  //min 
  out.width(2);
  out.fill('0');
  out << time_str.tm_min ;

  return (out) ; }


void CdaTime::print( ostream& out ){ 
  out << year() <<"\t" << day() << "\t" << hrs() << "\t" ; 
  out <<  min() << "\t" << sec() <<"\t" << msec() << "\t";
  out << time_code_generator_state();
  out << endl ;
}

long int CdaTime::TimDifSec(CdaTime & t2){
  return  ((year() - t2.year())* (365 * 24 * 60 * 60 ) +
	   (day()  - t2.day() )* (      24 * 60 * 60 ) +
	   (hrs()  - t2.hrs() )* (           60 * 60 ) +
	   (min()  - t2.min() )* (                60 ) + 
	   (sec()  - t2.sec() ) 
	   );
}
    
void CdaTime::toIEEE(int* ieee){
  ieee[0] = ( 
	     (yr1000 * 1000 + yr100 * 100 + yr10 * 10 + yr1 ) * 10000
	     + (doy100 * 100 + doy10 * 10 + doy1 ) * 10
             + hr10 
             );

  ieee[1] = (
             hr1 * 10000000  
             + (min10*10 + min1 )* 100000 
             + (sec10 * 10 + sec1)* 1000 
             + msec100 * 100 + msec10*10 + msec1
	     );
}

void CdaTime::setTime (uchar8* in) {
  yr1000 = in[0] >> 4 ;
  yr100 = in[0] & 0x0f ;

  yr10 = in[1] >> 4 ;
  yr1 = in[1] & 0x0f ;

  flywheel = in[2] >> 7 ;
  doy100 = (in[2] >> 4) & 0x07 ;
  doy10 = in[2] & 0x0f ;

  doy1 = in[3] >> 4 ;
  hr10 = in[3] & 0x0f ;

  hr1 = in[4] >> 4 ;
  min10 = in[4] & 0x0f ;

  min1 = in[5] >> 4 ;
  sec10 = in[5] & 0x0f ;

  sec1 = in[6] >> 4 ;
  msec100 = in[6] & 0x0f ;

  msec10 = in[7] >> 4 ;
  msec1 = in[7] &0x0f ;
}
