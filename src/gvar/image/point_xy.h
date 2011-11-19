/**
 * Defines a data structure of a point_xy. A point_xy is determined by
 * two-dimensional geometry point (x, y). The type of x and y can be
 * integer, double or float.
 *
 * @author jzhang - on Apr 20, 2004
 */
#ifndef POINT_XY_H
#define POINT_XY_H

#include "image/point.h"

template <class pointType>
class Point_XY : public Point {

private:
  pointType x ;
  pointType y ;

public:
  Point_XY (pointType x, pointType y) ;

  pointType getX () ;
  void setX (pointType x) ;
  pointType getY () ;
  void setY (pointType y) ;

  void print () ;

} ;

#include "image/point_xy.cpp"

#endif
