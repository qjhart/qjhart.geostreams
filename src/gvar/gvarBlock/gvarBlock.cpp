#include "gvarBlock.h"

GvarHeader* GvarBlock::createHeader (char* buff) {

  uchar8 temp[30] ;
  for (int i=0; i<30; i++) {
    temp[i] = (uchar8)buff[i] ;
  }

  GvarHeader* header = new GvarHeader (temp) ;

  return header ;
}


GvarBlock::GvarBlock(char* buff, int buff_size)
{
  header = createHeader (buff) ;

  // delete the first 3 30 bytes
  data = new uchar8[buff_size-90] ;
  for (int i=90; i<buff_size; i++) {
    data[i-90] = buff[i] ;
  }
} 

GvarBlock::~GvarBlock () {
  delete[] data ;
  delete header ;
}

uchar8* GvarBlock::getData()
{ return data ;}

GvarHeader* GvarBlock::getHeader()
{return header ;}

