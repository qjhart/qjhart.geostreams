/**
 * Defines the data structure of a point_xyt. A point_xyt
 * associates a timestamp for a point_xy.
 *
 * @author jzhang - created on Apr 20, 2004
 */
#ifndef POINT_XYT_H
#define POINT_XYT_H

#include "image/point_xy.h"

template <class pointType, class TIMESTAMP>
class Point_XYT : public Point {

private:
  TIMESTAMP time ;
  Point_XY<pointType>* point_xy ;

public:
  Point_XYT (Point_XY<pointType>* point_xy, TIMESTAMP time) ;
  Point_XYT (pointType x, pointType y, TIMESTAMP time) ;

  Point_XY<pointType>* getPoint_XY () ;
  TIMESTAMP getTime () ;
  pointType getX () ;
  pointType getY () ;
  
} ;

#include "image/point_xyt.cpp"

#endif
