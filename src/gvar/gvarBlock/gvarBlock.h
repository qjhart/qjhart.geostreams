#ifndef GVARBLOCK_H
#define GVARBLOCK_H

#include "include/types.h"
#include "gvarHeader/gvarHeader.h"

class GvarBlock { 
 private:

  GvarHeader* header ;
  uchar8* data;  // pointer to buffer for gvar data stream in memory

  GvarHeader* createHeader (char* buff) ;

 public:
  GvarBlock(char* buff, int buff_size); 
  ~GvarBlock();
  uchar8* getData();//Find block of data and copy contents to Data

  GvarHeader* getHeader () ; // Return the header
};

#endif // GVARBLOCK_H


