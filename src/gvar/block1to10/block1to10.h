#ifndef BLOCK1TO10_H
#define BLOCK1TO10_H

/**
 * This file declares the functions and variables for the class
 * Block1to10, which stores the data of block 1 to 10.
 */
#include "gvarBlock/gvarBlock.h"
#include "lineDoc/lineDoc.h"

class Block1to10 {

  private:
    // the pointer to GvarBlock
    GvarBlock* block ;

    // the line document of the data
    LineDoc* lineDoc ;

    // the pixel data
    uint16* data ;

    // the number of pixels in data
    int dataLen ;

  public:
    Block1to10 (GvarBlock*) ;
    ~Block1to10 () ;

    GvarBlock* getBlock () ;
    LineDoc* getLineDoc () ;
    uint16*  getData () ;
    int getDataLen () ;
} ;

inline GvarBlock* Block1to10::getBlock () { return block ; }

#endif
