/**
 * This file defines the data structure of a point.
 *
 * @author jzhang - on Apr 20, 2004
 */
#ifndef POINT_H
#define POINT_H

#include <iostream>

class Point {
public:
  virtual bool operator== ( Point& p ) { return false ; }
  virtual bool operator< ( Point& p ) {return false ; }
  virtual bool operator> ( Point& p ) {return false ; }
  virtual Point* operator+ ( Point& p ) {return NULL ; }
  virtual int numOfDims () { return 0 ; } 

  virtual void print () {}
};

#endif
