#ifndef IMAGE_LISTOFIMAGES_CPP
#define IMAGE_LISTOFIMAGES_CPP

/**
 * Defines the data structure for Image_ListofImages.
 *
 * @author jzhang - created on Apr 26, 2004.
 */
#include "image_listofimages.h"
#include "imageatt.h"

template <class pointType, class TIMESTAMP, class valueType>
Image_ListofImages<pointType, TIMESTAMP, valueType>::Image_ListofImages () {
  imageList = new std::list<Image<pointType, TIMESTAMP, valueType>* > () ;
}

template <class pointType, class TIMESTAMP, class valueType>
void Image_ListofImages<pointType, TIMESTAMP, valueType>::append (Image<pointType, TIMESTAMP, valueType>* image) {
  imageList->push_back (image) ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType Image_ListofImages<pointType, TIMESTAMP, valueType>::getLowX () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;
  itr = imageList->begin () ;
  return (*itr)->getLowX () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType Image_ListofImages<pointType, TIMESTAMP, valueType>::getHighX () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;
  itr = imageList->begin () ;
  return (*itr)->getHighX () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType Image_ListofImages<pointType, TIMESTAMP, valueType>::getLowY () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;
  itr = imageList->begin () ;
  return (*itr)->getLowY () ;
}

template <class pointType, class TIMESTAMP, class valueType>
pointType Image_ListofImages<pointType, TIMESTAMP, valueType>::getHighY () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;
  itr = imageList->end () ;
  itr-- ;
  return (*itr)->getHighY () ;
}

template <class pointType, class TIMESTAMP, class valueType>
int Image_ListofImages<pointType, TIMESTAMP, valueType>::numOfRows () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;

  int rows = 0 ;
  for (itr=imageList->begin (); itr!=imageList->end (); itr++) {
    rows += (*itr)->numOfRows () ;
  }
  return rows ;
}

template <class pointType, class TIMESTAMP, class valueType>
valueType** Image_ListofImages<pointType, TIMESTAMP, valueType>::getData () {
  typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;

  valueType** data = new (valueType*)[numOfRows ()] ;
  int index = 0 ;
  for (itr=imageList->begin (); itr!=imageList->end (); itr++) {
    data[index++] = (*itr)->getData () ;
  }

  return data ;
}

template <class pointType, class TIMESTAMP, class valueType>
int Image_ListofImages<pointType, TIMESTAMP, valueType>::getRowSize () {
  return getHighX () - getLowX () + 1 ;
}


template <class pointType, class TIMESTAMP, class valueType>
void Image_ListofImages<pointType, TIMESTAMP, valueType>::print () {
  // typename std::list<Image<pointType, TIMESTAMP, valueType>* >::iterator itr ;

  // for (itr=imageList->begin (); itr!=imageList->end (); itr++) {
  //   (*itr)->print () ;
  // }

  std::cout << "(xl, yl): (" << getLowX () << ", " << getLowY () << ")\n" ;

  std::cout << "(xh, yh): (" << getHighX () << ", " << getHighY () << ")\n" ;
  std::cout << "num of rows: " << numOfRows () << "\n\n" ;

  std::cout << "row size: " << getRowSize () << "\n" ;
  // print out data
  /*
  valueType** data = getData () ;
  for (int i=0; i<numOfRows (); i++) {
    for (int j=0; j<getRowSize (); j++) {
      std::cout << data[i][j] << " " ;
    }
    std:: cout << "\n" ;
  } 

  std:: cout << "\n\n" ;*/
}

#endif
