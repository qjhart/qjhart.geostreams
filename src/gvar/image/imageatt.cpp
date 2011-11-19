/**
 * Defines the data structure for ImageAtT.
 *
 * @author jzhang - created on Apr 26, 2004.
 */
#ifndef IMAGEATT_CPP
#define IMAGEATT_CPP

#include "image/imageatt.h"

template <class pointType, class TIMESTAMP, class valueType>
ImageAtT<pointType, TIMESTAMP, valueType>::ImageAtT (int id,
                    PointSet_XYatT<pointType, TIMESTAMP>* ps,
                    std::list<valueType>* values) {
  imageId = id ;
  pointSet = ps ;
  this->values = values ;
}

template <class pointType, class TIMESTAMP, class valueType>
ImageAtT<pointType, TIMESTAMP, valueType>::ImageAtT (int id,
   pointType lowPointX,
   pointType lowPointY,
   pointType highPointX,
   pointType highPointY,
   std::list<valueType>* values) {
  imageId = id ;
  pointSet = 
    new PointSet_XYatT<pointType, TIMESTAMP>
      (new Point_XY<pointType> (lowPointX, lowPointY),
       new Point_XY<pointType> (highPointX, highPointY),
       id) ;
  /*this->values = new std::list<valueType> () ;

  typename std::list<valueType>::iterator itr ;
  for (itr=values->begin(); itr!=values->end(); itr++) {
    this->values->push_back (*itr) ;
    }*/
  this->values = values ;
}

template<class pointType, class TIMESTAMP, class valueType>
ImageAtT<pointType, TIMESTAMP, valueType>::~ImageAtT () {
  if (values != NULL) {
    values->clear () ;
    delete values ;
  }
  if (pointSet != NULL) delete pointSet ;
  // delete values ;
}

template <class pointType, class TIMESTAMP, class valueType>
PointSet_XYatT<pointType, TIMESTAMP>* 
  ImageAtT<pointType, TIMESTAMP, valueType>::getPointSet () {
  return pointSet ;
}

template <class pointType, class TIMESTAMP, class valueType>
std::list<valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::getValues () {
  return values ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType ImageAtT<pointType, TIMESTAMP, valueType>::getLowX () {
  return (pointSet->getLowPoint ())->getX () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType ImageAtT<pointType, TIMESTAMP, valueType>::getHighX () {
  return (pointSet->getHighPoint ())->getX () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType ImageAtT<pointType, TIMESTAMP, valueType>::getLowY () {
  return (pointSet->getLowPoint ())->getY () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType ImageAtT<pointType, TIMESTAMP, valueType>::getHighY () {
  return (pointSet->getHighPoint ())->getY () ;
}

template <class pointType, class TIMESTAMP, class valueType>
int ImageAtT<pointType, TIMESTAMP, valueType>::numOfRows () {
  return (getHighY () - getLowY () + 1) ;
}

template <class pointType, class TIMESTAMP, class valueType>
valueType* ImageAtT<pointType, TIMESTAMP, valueType>::getData () {
  typename std::list<valueType>::iterator itr ;
  int size = values->size () ;

  valueType* valuesArray = new valueType[size] ;

  int index = 0 ;
  for (itr=values->begin (); itr!=values->end (); itr++) {
    valuesArray[index++] = *itr ;
  }

  return valuesArray ;
}

template <class pointType, class TIMESTAMP, class valueType>
int ImageAtT<pointType, TIMESTAMP, valueType>::getDataSize () {
  int size = values->size () ;
  return size ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::restriction (PointSet* pointSet) {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::extension (Image<pointType, TIMESTAMP, valueType>* image) {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::transform (Image<pointType, TIMESTAMP, valueType>* transformImage) {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::concat (Image<pointType, TIMESTAMP, valueType>* image) {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::operator+ (Image<pointType, TIMESTAMP, valueType>* image) {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
Image<pointType, TIMESTAMP, valueType>* ImageAtT<pointType, TIMESTAMP, valueType>::operator- (Image<pointType, TIMESTAMP, valueType>* image)  {
  return NULL ;
}

template <class pointType, class TIMESTAMP, class valueType>
void ImageAtT<pointType, TIMESTAMP, valueType>::print () {
  std::cout << "Imageid=" << imageId << endl ;

  // pointSet->print () ;

  std::cout << "low x=" << getLowX () << endl ;
  std::cout << "high x=" << getHighX () << endl ;
  std::cout << "low y=" << getLowY () << endl ;
  std::cout << "high y=" << getHighY () << endl ;

  // print out the data
  /*
  valueType* data = getData () ;
  int size = getDataSize () ;

  for (int i=0; i<size; i++) {
    std::cout << data[i] << " " ;
  }
  std::cout << "\n" ; */
}

#endif
