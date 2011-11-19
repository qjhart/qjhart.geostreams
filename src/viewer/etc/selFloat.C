#include "selFloat.h"
#include <memory.h>
#include <iostream>
//#include <stream.h>

using namespace std;

double SelFloat::ToDouble() { 
  double f=1.0; 

  union {SELFloat SEL; float IEEE; int INT; } F1; 
  memcpy(&F1, &F0, sizeof(F1));
  
  if(F1.SEL.sign) { 
    F1.INT = 0xffffffff ^ F1.INT ;
    f = -1.0;
  }
  
  if(F1.SEL.exp > 0x40)
    f *= 1.0 * F1.SEL.m*(1<<(4*(F1.SEL.exp-0x40)))/0x1000000; 
  else 
    f *= 1.0 * F1.SEL.m/(1<<(4*(0x40-F1.SEL.exp)))/0x1000000; 
  return(f);
}

float SelFloat::ToFloat() { 
  return((float) ToDouble());
}

ostream& operator << (ostream & out, SelFloat & s){
  out << (double) s ; 
  return(out);
}

void SelFloat::ByteSwap(void)
{
  unsigned long l = (unsigned long)F0.INT;
  l = ((l&0xff000000)>>24)|
      ((l&0x00ff0000)>>8)|
      ((l&0x0000ff00)<<8)|
      ((l&0x000000ff)<<24);
  F0.INT = (int)l;
}

