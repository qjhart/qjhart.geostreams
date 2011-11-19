#ifndef POINT_H
#define POINT_H

#include <iostream>

using namespace std ;

template <typename DIMTYPE>
DIMTYPE* getArray (DIMTYPE x, DIMTYPE y) {
  DIMTYPE* values = new DIMTYPE[2] ;
  values[0] = x ;
  values[1] = y ;

  return values ;
}

/**
 * The data structure to represent a multi-dimensional point.
 */
template <typename DIMTYPE>
class Point {
protected:
  int _numOfDims ;

  // the values for each dimension
  DIMTYPE* values ;

public:
  Point (int numOfDims, DIMTYPE values[]) ;
  virtual ~Point () ;

  DIMTYPE* dims () ;
  int numOfDims () ;

  void print () ;
} ;

template <typename DIMTYPE>
Point<DIMTYPE>::Point (int numOfDims, DIMTYPE values[]) {
  this->_numOfDims = numOfDims ;
  this->values = new DIMTYPE[numOfDims] ;

  for (int dimNo=0; dimNo<numOfDims; dimNo++) {
    this->values[dimNo] = values[dimNo] ;
  }
}

template <typename DIMTYPE>
Point<DIMTYPE>::~Point () {
  if (values != NULL) delete[] values ;
}

template <typename DIMTYPE>
DIMTYPE* Point<DIMTYPE>::dims () {
  return values ;
}

template <typename DIMTYPE>
int Point<DIMTYPE>::numOfDims () {
  return _numOfDims ;
}

template <typename DIMTYPE>
void Point<DIMTYPE>::print () {
  cout << "(" ;
  for (int dimNo=0; dimNo<_numOfDims; dimNo++) {
    if (dimNo != 0) cout << ", " ;
    cout << values[dimNo] ;
  }
  cout << ")\n" ;
}


template <typename DIMTYPE>
class Point_2 : public Point<DIMTYPE> {
public:
  Point_2 (DIMTYPE x, DIMTYPE y) ;
  ~Point_2 () ;

  DIMTYPE x () ;
  DIMTYPE y () ;
} ;

template <typename DIMTYPE>
Point_2<DIMTYPE>::Point_2 (DIMTYPE x, DIMTYPE y): 
  Point<DIMTYPE> (2, getArray<DIMTYPE>(x, y)) {
}

template <typename DIMTYPE>
Point_2<DIMTYPE>::~Point_2 () {
}

template <typename DIMTYPE>
DIMTYPE Point_2<DIMTYPE>::x () {
  return dims[0] ;
}

template <typename DIMTYPE>
DIMTYPE Point_2<DIMTYPE>::y () {
  return dims[1] ;
}

#endif
