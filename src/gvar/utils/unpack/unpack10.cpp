#include "unpack10.h"

// This function converts every 10 bits of a byte array into 16 bits.
void unpack10 (uchar8* in,
               int len,
               uint16* out) {
  // We notice that every 5 bytes (40 bits) of 'in' will be converted
  // to 4 16-bits. So I will try to put 4 bytes into a 'uint32' value
  // and 1 byte in a 'uchar8' value.
  uint32 fourBytes ;
  uchar8 oneByte ;

  // the index for the output string
  int outIndex = 0 ;
  for (int i=0; i<len; i+=5) {
    fourBytes = (in[i]<<24) + (in[i+1]<<16) + (in[i+2]<<8) + in[i+3] ;
    oneByte = in[i+4] ;

    uint32 temp = fourBytes >> 22 ;
    out[outIndex++] = (uint16)temp ;

    temp = (fourBytes << 10) >> 22 ;
    out[outIndex++] = (uint16)temp ;

    temp = (fourBytes << 20) >> 22 ;
    out[outIndex++] = (uint16)temp ;

    temp = ((fourBytes << 30) >> 22) + oneByte ;
    out[outIndex++] = (uint16)temp ;

  }
}
