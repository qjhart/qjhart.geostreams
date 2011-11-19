#ifndef BLOCK0_H
#define BLOCK0_H

/**
 * This file declares the functions and variables for the class
 * Block0, which stores the data of block 0 aka the 'Doc' Block.
 */
#include "gvarBlock/gvarBlock.h"
#include "block0Doc.h"
#include "include/types.h"

#include <iostream>
using namespace std;

class Block0 {

  private:
    // the pointer to GvarBlock
    GvarBlock* block ;
    
    // the block0 information data
    Block0Doc* block0Doc ;

  public:
    Block0 (GvarBlock*) ;
    ~Block0 ();
    Block0Doc* getBlock0Doc () ;
} ;

#endif
