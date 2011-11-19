#ifndef SELFLOAT_H
#define SELFLOAT_H
#include <iostream>
//#include <stream.h>

#ifdef _LSBF_
struct SELFloat
{
  unsigned m:24;
  unsigned exp:7;
  unsigned sign:1;
};
#else
struct SELFloat{ 
  unsigned sign: 1;
  unsigned exp: 7;
  unsigned m: 24;
};
#endif

typedef struct SELFloat SELFloat; 

class SelFloat{
  union {SELFloat SEL; float IEEE; int INT; } F0; 
  double ToDouble(); 
  float ToFloat();
 public:
  SelFloat(float f) { F0.IEEE = f; }
  void setFloat(float f) { F0.IEEE = f; }
  operator float(){ return ToFloat() ;} 
  operator double(){ return ToDouble(); }
  friend std::ostream & operator<<(std::ostream & out, SelFloat & s);
  void ByteSwap(void);

};



#endif // SELFLOAT_H
