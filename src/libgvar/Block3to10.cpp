/**
 * This file defines the functions for the class.
 */
#include "Gvar.h"

namespace Gvar {
  Block3to10::Block3to10 (Block* block) {
	this->block = block ;
	
	// retrieve the line document
	this->lineDoc = new LineDoc (block) ;
	this->data = lineDoc->getData () ;
  }
  
  Block3to10::~Block3to10 () {
	if (lineDoc != NULL) delete lineDoc ;
  }
  
  LineDoc* Block3to10::getLineDoc () {
	return lineDoc ;
  }
  
  uint16* Block3to10::getData () {
	return data ;
  }
  
  int Block3to10::getDataLen () {
	return lineDoc->lpixls () ;
  }
}
