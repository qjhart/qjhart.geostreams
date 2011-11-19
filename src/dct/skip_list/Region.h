#ifndef REGION_H
#define REGION_H

#include "Point.h"

/**
 * The data structure for a region.
 *
 * @author jzhang - Jul 01, 2004
 */
template <typename IDTYPE, typename DIMTYPE>
class Region {
private:
  // the number of dimensions
  int _numOfDims ;

  // the id for this region
  int _id ;

  // the minimum values in all dimensions for this region
  DIMTYPE *minValues ;

  // the maximum values in all dimensions for this region
  DIMTYPE *maxValues ;

public:
  Region (IDTYPE id, Point<DIMTYPE>* lowPoint, Point<DIMTYPE>* highPoint) ;
  ~Region () ;

  IDTYPE id () ;
  DIMTYPE* mins () ;
  DIMTYPE* maxs () ;

  int numOfDims () ;

  void print () ;
} ;

template <typename IDTYPE, typename DIMTYPE>
Region<IDTYPE, DIMTYPE>::Region
  (IDTYPE id, Point<DIMTYPE>* lowPoint, Point<DIMTYPE>* highPoint) {
  this->_id = id ;

  if (lowPoint->numOfDims () != highPoint->numOfDims ()) {
    cerr << "Can not create a region for two points "
         << "of different dimensions.\n" ;
    exit (0) ;
  }

  this->_numOfDims = lowPoint->numOfDims () ;
  this->minValues = new DIMTYPE[_numOfDims] ;
  this->maxValues = new DIMTYPE[_numOfDims] ;
  for (int dimNo=0; dimNo<_numOfDims; dimNo++) {
    this->minValues[dimNo] = (lowPoint->dims ())[dimNo] ;
    this->maxValues[dimNo] = (highPoint->dims ())[dimNo] ;
  }
}

template <typename IDTYPE, typename DIMTYPE>
Region<IDTYPE, DIMTYPE>::~Region () {
  delete[] minValues ;
  delete[] maxValues ;
}

template <typename IDTYPE, typename DIMTYPE>
IDTYPE Region<IDTYPE, DIMTYPE>::id () {
  return _id ;
}

template <typename IDTYPE, typename DIMTYPE>
DIMTYPE* Region<IDTYPE, DIMTYPE>::mins () {
  return minValues ;
}


template <typename IDTYPE, typename DIMTYPE>
DIMTYPE* Region<IDTYPE, DIMTYPE>::maxs () {
  return maxValues ;
}

template <typename IDTYPE, typename DIMTYPE>
void Region<IDTYPE, DIMTYPE>::print () {
  cout << "(" ;

  for (int dimNo=0; dimNo<_numOfDims; dimNo++) {
    if (dimNo != 0) cout << ", " ;
    cout << minValues[dimNo] ;
  }
  cout << ")" ;

  cout << "-(" ;

  for (int dimNo=0; dimNo<_numOfDims; dimNo++) {
    if (dimNo != 0) cout << ", " ;
    cout << maxValues[dimNo] ;
  }
  cout << ")" ;


}

#endif
