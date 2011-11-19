#ifndef POINTSET_H
#define POINTSET_H

#include <iostream>
using namespace std ;

#include "block1to10/block1to10.h"
#include "block0/block0.h"

/**
 * Define a pointset class.
 */
class PointSet {
private:
  int frameId ;
  int row ;
  int columnStart ;
  int size ;
  uint16* data ;
  int scale;
public:
  PointSet (Block0 *block0, Block1to10 *block1to10) ;
  PointSet(int frameid, int row_no, int col_start, int image_size, uint16* image_data, int scale_value);
  ~PointSet () ;

  int getFrameId () ;
  int getRow () ;
  int getColumnStart () ;
  int getSize () ;
  uint16* getData () ;
  int getScale();
  
  void setSize(int newsize);
  void setData(uint16* image_data);
  void setColumnStart(int num);
  void setScale(int num);
  void print () ;
} ;

inline PointSet::~PointSet () { delete data ; }
inline int PointSet::getFrameId () { return frameId ; }
inline int PointSet::getRow () { return row ; }
inline int PointSet::getColumnStart () { return columnStart ;}
inline int PointSet::getSize () { return size ; }
inline uint16* PointSet::getData () { return data ; }
inline int PointSet::getScale(){return scale;}

inline void PointSet::setSize(int newsize) { size = newsize;}
inline void PointSet::setData(uint16* image_data){data = image_data;}
inline void PointSet::setColumnStart(int num){columnStart = num;}
inline void PointSet::setScale(int num){ scale = num;}
#endif
