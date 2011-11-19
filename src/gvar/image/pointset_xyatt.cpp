/**
 * Defines the data structure of PointSet_XYatT.
 *
 * @author jzhang - created on Apr 26, 2004.
 */
#ifndef POINTSET_XYATT_CPP
#define POINTSET_XYATT_CPP

#include "pointset_xyatt.h"

template <class pointType, class TIMESTAMP>
PointSet_XYatT<pointType, TIMESTAMP>::PointSet_XYatT 
  (Point_XY<pointType>* lowPoint,
   Point_XY<pointType>* highPoint,
   TIMESTAMP time) {
  this->lowPoint = lowPoint ;
  this->highPoint = highPoint ;
  this->time = time ;
}

template <class pointType, class TIMESTAMP>
PointSet_XYatT<pointType, TIMESTAMP>::~PointSet_XYatT () {
  if (lowPoint != NULL) delete lowPoint ;
  if (highPoint != NULL) delete highPoint ;
}

template <class pointType, class TIMESTAMP>
int PointSet_XYatT<pointType, TIMESTAMP>::card () {
  return (highPoint->getX () - lowPoint->getX () + 1) * 
         (highPoint->getY () - lowPoint->getY () + 1) ;
}

template <class pointType, class TIMESTAMP>
Point_XY<pointType>* PointSet_XYatT<pointType, TIMESTAMP>::getLowPoint () {
  return lowPoint ;
}

template <class pointType, class TIMESTAMP>
Point_XY<pointType>* PointSet_XYatT<pointType, TIMESTAMP>::getHighPoint () {
  return highPoint ;
}

// return the timestamp of this pointset
template <class pointType, class TIMESTAMP>
TIMESTAMP PointSet_XYatT<pointType, TIMESTAMP>::getTime () {
  return time ;
}

template <class pointType, class TIMESTAMP>
bool PointSet_XYatT<pointType, TIMESTAMP>::containsPoint (Point* p) {
  Point_XYT<pointType, TIMESTAMP>* point = 
    (Point_XYT<pointType, TIMESTAMP>*)p ;

  if (point->getTime () != this->getTime ()) {
    return false ;
  }

  if (point->getX () < (this->lowPoint)->getX () ||
      point->getX () > (this->highPoint)->getX () ||
      point->getY () < (this->lowPoint)->getY () ||
      point->getY () > (this->highPoint)->getY ()) {
    return false ;
  }

  return true ;
}

template <class pointType, class TIMESTAMP>
bool PointSet_XYatT<pointType, TIMESTAMP>::containsPointSet 
  (PointSet *pointSet) {
  PointSet_XYatT<pointType, TIMESTAMP> *ps = 
    (PointSet_XYatT<pointType, TIMESTAMP>*)pointSet ;

  if (ps->getTime () != getTime ()) {
    return false ;
  }

  if (((lowPoint->getX () <= (ps->getLowPoint())->getX ()) &&
       (highPoint->getX () >= (ps->getHighPoint())->getX ())) &&
      ((lowPoint->getY () <= (ps->getLowPoint ())->getY ()) &&
       (highPoint->getY () >= (ps->getHighPoint ())->getY ()))) {
    return true ;
  }

  return false ;
}

template <class pointType, class TIMESTAMP>
bool PointSet_XYatT<pointType, TIMESTAMP>::exclusive (PointSet* pointSet) {
   PointSet_XYatT<pointType, TIMESTAMP> *ps = 
    (PointSet_XYatT<pointType, TIMESTAMP>*)pointSet ;

   if (time != ps->getTime ()) {
     return true ;
   }

   if (lowPoint->getX () > ps->getHighPoint ()->getX () ||
       highPoint->getX () < ps->getLowPoint ()->getX () ||
       lowPoint->getY () > ps->getHighPoint ()->getY () ||
       highPoint->getY () < ps->getHighPoint ()->getY ()) {
     return true ;
   }

   return false ;
}

template <class pointType, class TIMESTAMP>
bool PointSet_XYatT<pointType, TIMESTAMP>::operator== (PointSet* pointSet) {
  PointSet_XYatT<pointType, TIMESTAMP> *ps = 
   (PointSet_XYatT<pointType, TIMESTAMP>*)pointSet ;

  if (time != ps->getTime ()) {
    return false ;
  }

  if (lowPoint->getX () == ps->getLowPoint ()->getX () &&
      lowPoint->getY () == ps->getLowPoint ()->getY () &&
      highPoint->getX () == ps->getHighPoint ()->getX () &&
      highPoint->getY () == ps->getHighPoint ()->getY ()) {
    return true ;
  }

  return false ;
}

template <class pointType, class TIMESTAMP>
PointSet* PointSet_XYatT<pointType, TIMESTAMP>::operator&& 
  (PointSet* pointSet) {
  PointSet_XYatT<pointType, TIMESTAMP> *ps = 
   (PointSet_XYatT<pointType, TIMESTAMP>*)pointSet ;

  if (exclusive (ps)) {
    return NULL ;
  }

  pointType lowPointX, highPointX, lowPointY, highPointY ;
  pointType values[4] ;
  values[0] = lowPoint->getX () ;
  values[1] = highPoint->getX () ;
  values[2] = ps->getLowPoint()->getX () ;
  values[3] = ps->getHighPoint ()->getX () ;

  sort (values, 4) ;

  lowPointX = values[1] ;
  highPointX = values[2] ;

  values[0] = lowPoint->getY () ;
  values[1] = highPoint->getY () ;
  values[2] = ps->getLowPoint ()->getY () ;
  values[3] = ps->getHighPoint()->getY () ;

  sort (values, 4) ;

  lowPointY = values[1] ;
  highPointY = values[2] ;

  PointSet_XYatT<pointType, TIMESTAMP> *newPS =
    new PointSet_XYatT<pointType, TIMESTAMP> 
      (new Point_XY<pointType> (lowPointX, lowPointY),
       new Point_XY<pointType> (highPointX, highPointY),
       ps->getTime ()) ;

  return newPS ;
}

template <class pointType, class TIMESTAMP>
void PointSet_XYatT<pointType, TIMESTAMP>::sort 
  (pointType* values, int length) {
   // sort values array
  for (int i=0; i<length; i++) {
    for (int j=i; j<length; j++) {
      if (values[j] < values[i]) {
        pointType temp = values[j] ;
        values[j] = values[i] ;
        values[i] = temp ;
      }
    }
  }
}

template <class pointType, class TIMESTAMP>
void PointSet_XYatT<pointType, TIMESTAMP>::print () {
  std::cout << "Low point: " ;
  lowPoint->print () ;
  std::cout << "High point: " ;
  highPoint->print () ;
  std::cout << "Time: " << time << std::endl ;
}

#endif
