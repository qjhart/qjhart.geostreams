/**
 * This file defines the functions for the class.
 */
#include "block0.h"

Block0::Block0 (GvarBlock* block) {
  this->block = block ;

  // retrieve the line document
  block0Doc = new Block0Doc (block) ;
}

Block0::~Block0 () {
  delete block0Doc ;
  delete block ;
}

Block0Doc* Block0::getBlock0Doc () {
  return block0Doc ;
}


