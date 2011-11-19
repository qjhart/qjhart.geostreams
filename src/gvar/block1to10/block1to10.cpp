/**
 * This file defines the functions for the class.
 */
#include "block1to10.h"

Block1to10::Block1to10 (GvarBlock* block) {
  this->block = block ;

  // retrieve the line document
  lineDoc = new LineDoc (block) ;
  data = lineDoc->getData () ;
}

Block1to10::~Block1to10 () {
  if (lineDoc != NULL) delete lineDoc ;
  if (block != NULL) delete block ;
  // if (data != NULL) delete[] data ;
}

LineDoc* Block1to10::getLineDoc () {
  return lineDoc ;
}

uint16* Block1to10::getData () {
  return data ;
}

int Block1to10::getDataLen () {
  return lineDoc->lpixls () ;
}
