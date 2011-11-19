/**
 * This file defines the functions for the class.
 */
#include "Gvar.h"

namespace Gvar {
  Block0::Block0 (Block* block) {
    this->block = block;
    block0Doc = new Block0Doc (block) ;
  }

  Block0::~Block0 () {
    if(block0Doc != NULL) delete block0Doc ;
    if(block != NULL) delete block ;
  }
}//namespace Gvar
