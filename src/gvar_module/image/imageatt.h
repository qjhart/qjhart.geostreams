/**
 * Defines the data structure for ImageAtT.
 *
 * @author jzhang - created on Apr 26, 2004.
 */
#ifndef IMAGEATT_H
#define IMAGEATT_H

#include <list>
#include <iostream>

#include "image/pointset.h"
#include "image/pointset_xyatt.h"
#include "image/image.h"

template <class pointType, class TIMESTAMP, class valueType>
class ImageAtT : public Image<pointType, TIMESTAMP, valueType> {
private:
  PointSet_XYatT<pointType, TIMESTAMP>* pointSet ;
  std::list<valueType>* values ;

public:
  ImageAtT (int id, 
            PointSet_XYatT<pointType, TIMESTAMP>* ps,
            std::list<valueType>* values) ;

  ImageAtT (int id,
            pointType lowPointX,
            pointType lowPointY,
            pointType highPointX,
            pointType highPointY,
            std::list<valueType>* values) ;

  ~ImageAtT () ;

  PointSet_XYatT<pointType, TIMESTAMP>* getPointSet () ;
  std::list<valueType>* getValues () ;

  pointType getLowX () ;
  pointType getHighX () ;
  pointType getLowY () ;
  pointType getHighY () ;
  int numOfRows () ;
  valueType* getData () ;
  int getDataSize () ;

  Image<pointType, TIMESTAMP, valueType> * restriction (PointSet* pointSet) ;

  Image<pointType, TIMESTAMP, valueType> * extension (Image<pointType, TIMESTAMP, valueType>* image) ;

  Image<pointType, TIMESTAMP, valueType> * transform (Image<pointType, TIMESTAMP, valueType> * transformImage) ;

  Image<pointType, TIMESTAMP, valueType> * concat (Image<pointType, TIMESTAMP, valueType> * image)  ;

  Image<pointType, TIMESTAMP, valueType> * operator+ (Image<pointType, TIMESTAMP, valueType> * image)  ;

  Image<pointType, TIMESTAMP, valueType> * operator- (Image<pointType, TIMESTAMP, valueType> * image)  ;

  // print out this object as a string.
  void print () ;

} ;

#include "image/imageatt.cpp"

#endif
