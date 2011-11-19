#include "cdaTime.h"

CdaTime::CdaTime(int * in){
  memcpy(this, in, sizeof(CdaTime));
}

int CdaTime::month() {
  int yearnum = year();
  int numofdays = day();

  for (int month = 1; month <= 12; month++) {
    numofdays -= getNumOfDaysInMonth(month,yearnum);
    if (numofdays < 0)
      return month ;
  }
  return -1;
}

int CdaTime::dayofmonth() {
  int yearnum = year();
  int numofdays = day();
  for(int month = 1; month <= 12; month++) {
      numofdays -= getNumOfDaysInMonth(month, yearnum);
      if(numofdays < 0)
        return numofdays += getNumOfDaysInMonth(month,yearnum);
  }//for  
  return -1;
}

int CdaTime::getNumOfDaysInMonth(int month, int year) {
  switch(month) {
    case 1: return 31;
    case 2: 
      if( year % 4 == 0)
        return 29;
      return 28;
    case 3: return 31;
    case 4: return 30;
    case 5: return 31;
    case 6: return 30;
    case 7: return 31;
    case 8: return 31;
    case 9: return 30;
    case 10: return 31;
    case 11: return 30;
    case 12: return 31;
    default: 
      cout << month << " is not a valid month." << endl;
      exit(1);
  }//switch
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

  return (out) ; 
}//<<

char* CdaTime::getYMDEpoch (char* Epoch) {
  char* YMDE = NULL;
  YMDE = new char[50];

  char monthStr[3]  ;
  formatTimeDigits (month (), monthStr) ;

  char dayStr[3] ;
  formatTimeDigits (dayofmonth(), dayStr) ;

  sprintf(YMDE, "%d-%s-%s-%s", year(), monthStr, dayStr, Epoch);
  return YMDE;
}

char* CdaTime::getHM () {
  char* HM = NULL;
  HM = new char[50];

  char hourStr[3]  ;
  formatTimeDigits (hrs (), hourStr) ;

  char minuteStr[3] ;
  formatTimeDigits (min(), minuteStr) ;

  sprintf(HM, "%s-%s", hourStr, minuteStr);
  return HM; 
}

void CdaTime::formatTimeDigits (int digits, char* finalStr) {
  if (digits < 10) {
    finalStr[0] = '0' ;
    finalStr[1] = (char)(digits+48) ;
    finalStr[2] = '\0' ;
  } else {
    sprintf (finalStr, "%d", digits) ;
  }
}

char* CdaTime::getConcatenatedTime () {
  //cout << "in cdaTime gettime function" << endl ;
  char* conTime = NULL;
  conTime = new char[50];
  char temp[10];
  strcpy(conTime,"\0");
  sprintf(temp,"%d",year());
  strcat(conTime,temp);
  strcat(conTime,"_");
  sprintf(temp,"%d",day());
  strcat(conTime,temp);
  strcat(conTime,"_");
  sprintf(temp,"%d",hrs());
  strcat(conTime,temp);
  strcat(conTime,"_");
  sprintf(temp,"%d",min());
  strcat(conTime,temp);
  strcat(conTime,"_");
  sprintf(temp,"%d",sec());
  strcat(conTime,temp);
  strcat(conTime,"_");
  sprintf(temp,"%d",msec());
  strcat(conTime,temp);
  //cout << " conTime : " << conTime<< endl;
  return conTime;
}

void CdaTime::print( ostream& out ) {
  out << year() <<"\t" << day() << "\t" << hrs() << "\t" ;
  out <<  min() << "\t" << sec() <<"\t" << msec() << "\t";
  out << time_code_generator_state();
  out << endl ;
}

long int CdaTime::TimDifSec(CdaTime & t2) {
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
