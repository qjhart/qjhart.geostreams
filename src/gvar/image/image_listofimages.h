/**
 * Defines the data structure for Image_ListofImages.
 *
 * @author jzhang - created on Apr 26, 2004
 */
#ifndef IMAGE_LISTOFIMAGES_H
#define IMAGE_LISTOFIMAGES_H

#include <list>
#include "image/image.h"

template <class pointType, class TIMESTAMP, class valueType>
class Image_ListofImages {
private:
  std::list<Image<pointType, TIMESTAMP, valueType>* >* imageList ;

public:
  Image_ListofImages () ;

  void append (Image<pointType, TIMESTAMP, valueType> * image) ;

  pointType getLowX () ;
  pointType getHighX () ;
  pointType getLowY () ;
  pointType getHighY () ;
  int numOfRows () ;
  valueType** getData () ;
  int getRowSize () ;

  void print () ;
} ;

#include "image/image_listofimages.cpp"

#endif
