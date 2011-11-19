/**
 * Defines the data structure of PointSet_XYatT.
 *
 * @author jzhang - created on Apr 26, 2004
 */
#ifndef POINTSET_XYATT_H
#define POINTSET_XYATT_H

#include "image/pointset.h"
#include "image/point_xy.h"
#include "image/point_xyt.h"

template <class pointType, class TIMESTAMP>
class PointSet_XYatT: public PointSet {
private:
  Point_XY<pointType>* lowPoint ;
  Point_XY<pointType>* highPoint ;

  TIMESTAMP time ;

  void sort (pointType* values, int length) ;

public:
  PointSet_XYatT (Point_XY<pointType>* lowPoint,
                  Point_XY<pointType>* highPoint,
                  TIMESTAMP time) ;

  ~PointSet_XYatT () ;
  
  // the cardinality of this pointset
  int card () ;

  // return the lowest point of this pointset
  Point_XY<pointType>* getLowPoint () ;

  // return the highest point of this pointset
  Point_XY<pointType>* getHighPoint () ;

  // return the time.
  TIMESTAMP getTime () ;

  // Check if this pointset contains a given point.
  bool containsPoint (Point* p) ;

  // Check if this pointset contains another pointset
  bool containsPointSet (PointSet *pointSet) ;

  // Check if two pointsets have no overlapped points
  bool exclusive (PointSet *pointSet) ;

  // == operator
  bool operator== (PointSet* pointSet) ;

  // && operator: return the overlapped points between this pointset and
  // the other given pointset.
  PointSet* operator&& (PointSet * pointSet) ;

  void print () ;
} ;

#include "image/pointset_xyatt.cpp"

#endif
