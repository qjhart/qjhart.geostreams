/**
 * Define a pointset class.
 */
#include "analysis/pointset.h"

PointSet::PointSet (Block0 *block0, Block1to10 *block1to10) {
  Block0Doc* block0Doc = block0->getBlock0Doc () ;

  frameId = block0Doc->frame () ;
  row = block0Doc->nln() + ((block1to10->getBlock())->getHeader ())->blockId () - 3 ;
  columnStart = block0Doc->wpx() ;
//  cout << "epx=" << block0Doc->epx () << endl ;
  size = block1to10->getDataLen () ;
  scale = 1;
  data = new uint16[size] ;
  for (int i=0; i<size; i++) {
    data[i] = (block1to10->getData ())[i] ;
  }
}

void PointSet::print () {
  cout << "frameId = " << frameId << endl ;
  cout << "row=" << row << endl ;
  cout << "columnStart=" << columnStart << endl ;
  cout << "size=" << size << endl ;
  cout << "scale=" << scale << endl;
  for (int i=0; i<20; i++) {
    cout << data[i] << " " ;
  }
  cout << endl ;
}


PointSet::PointSet(int frameid, int row_no, int col_start, int image_size, uint16* image_data, int scale_value){
  frameId = frameid;
  row = row_no;
  columnStart = col_start;
  size = image_size;
  scale = scale_value;
  data = new uint16[size];
  for (int i=0; i<size; i++) {
    data[i] = image_data[i] ;
  }

}
