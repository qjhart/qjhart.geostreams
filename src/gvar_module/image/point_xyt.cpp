/**
 * Defines the methods in class Point_XYT.
 *
 * @author jzhang - created on Apr 20, 2004
 */
#ifndef POINT_XYT_CPP
#define POINT_XYT_CPP

#include "point_xyt.h"

template <class pointType, class timestamp>
Point_XYT<pointType, timestamp>::Point_XYT 
  (Point_XY<pointType>* point_xy, timestamp time) {
  this->point_xy = point_xy ;
  this->time = time ;
}

template <class pointType, class timestamp>
Point_XYT<pointType, timestamp>::Point_XYT 
  (pointType x, pointType y, timestamp time) {
  this->point_xy = new Point_XY<pointType> (x, y) ;
  this->time = time ;
}

template <class pointType, class timestamp>
Point_XY<pointType>* Point_XYT<pointType, timestamp>::getPoint_XY () {
  return point_xy ;
}

template <class pointType, class timestamp>
timestamp Point_XYT<pointType, timestamp>::getTime () {
  return time ;
}

template <class pointType, class timestamp>
pointType Point_XYT<pointType, timestamp>::getX () {
  return point_xy->getX () ;
}

template <class pointType, class timestamp>
pointType Point_XYT<pointType, timestamp>::getY () {
  return point_xy->getY () ;
}

#endif
