/**
 * Defines the methods in the class Point_XY.
 *
 * @author jzhang - created on Apr 20, 2004
 */
#ifndef POINT_XY_CPP
#define POINT_XY_CPP

#include "image/point_xy.h"

template <class pointType>
Point_XY<pointType>::Point_XY (pointType x, pointType y) {
  this->x = x ;
  this->y = y ;
}

template <class pointType>
pointType Point_XY<pointType>::getX () {
  return x ;
}

template <class pointType>
void Point_XY<pointType>::setX (pointType x) {
  this->x = x ;
}

template <class pointType>
pointType Point_XY<pointType>::getY () {
  return y ;
}

template <class pointType>
void Point_XY<pointType>::setY (pointType y) {
  this->y = y ;
}

template <class pointType>
void Point_XY<pointType>::print () {
  std::cout << "x: " << x << std::endl
            << "y: " << y << std::endl ;
}

#endif
