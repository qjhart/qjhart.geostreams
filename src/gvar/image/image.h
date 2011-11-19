/**
 * Defines the data structure for an image.
 *
 * @author jzhang - created on Apr 26, 2004
 */
#ifndef IMAGE_H
#define IMAGE_H

#include "image/pointset.h"
#include <iostream>

template <class pointType, class TIMESTAMP, class valueType>
class Image {
protected:
  int imageId ;

public:

  int getImageId () { return imageId ; }

  virtual int numOfRows () { return 0 ; } 
  virtual pointType getLowX () { return -1 ; } 
  virtual pointType getHighX () { return -1 ; } 
  virtual pointType getLowY () { return -1 ; } 
  virtual pointType getHighY () { return -1 ; } 
  virtual valueType* getData () { return NULL ; } 
  virtual int getDataSize () { return 0 ; } 

  virtual Image<pointType, TIMESTAMP, valueType> * restriction (PointSet* pointSet) { return NULL ; }

  virtual Image<pointType, TIMESTAMP, valueType> * extension (Image<pointType, TIMESTAMP, valueType> * image) { return NULL ; }

  virtual Image<pointType, TIMESTAMP, valueType> * transform (Image<pointType, TIMESTAMP, valueType> * transformImage) { return NULL ; }

  virtual Image<pointType, TIMESTAMP, valueType> * concat (Image<pointType, TIMESTAMP, valueType> * image) { return NULL ; }

  virtual Image<pointType, TIMESTAMP, valueType> * operator+ (Image<pointType, TIMESTAMP, valueType> * image) { return NULL ; }

  virtual Image<pointType, TIMESTAMP, valueType> * operator- (Image<pointType, TIMESTAMP, valueType> * image) { return NULL ; }

  virtual void print () {std::cout <<"print an image in base class.\n" ;}
} ;

#endif
