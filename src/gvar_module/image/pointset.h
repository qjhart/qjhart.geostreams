/**
 * Defines the data structure for a pointset.
 *
 * @author jzhang - created on Apr 26, 2004
 */
#ifndef POINTSET_H
#define POINTSET_H

#include "image/point.h"

class PointSet {

public:
  // the cardinality of this pointset
  virtual int card () = 0 ;

  // return the lowest point of this pointset
  virtual Point* getLowPoint () = 0 ;

  // return the highest point of this pointset
  virtual Point* getHighPoint () = 0 ;

  // Check if this pointset contains a given point.
  virtual bool containsPoint (Point* p) = 0 ;

  // Check if this pointset contains another pointset
  virtual bool containsPointSet (PointSet *pointSet) = 0 ;

  // Check if two pointsets have no overlapped points
  virtual bool exclusive (PointSet *pointSet) = 0 ;

  // == operator
  virtual bool operator== (PointSet* pointSet) = 0 ;

  // && operator: return the overlapped points between this pointset and
  // the other given pointset.
  virtual PointSet* operator&& (PointSet * pointSet) = 0 ;

  // print out this object as a string
  virtual void print () {} 
} ;

#endif
