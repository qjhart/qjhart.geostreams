#ifndef DYNAMICCASCADETREE_H
#define DYNAMICCASCADETREE_H

#include<sys/times.h>

#include "CascadeNode.h"

/**
 * The data structure for a DynamicCascadeTree.
 *
 * @author jzhang - created on Jul 02, 2004
 */
template <typename IDTYPE, typename DIMTYPE>
class DynamicCascadeTree {
private:
  int skipListMaxHeight ;
  DIMTYPE maxDimValue ;
  IDTYPE maxIdValue ;
  int numOfDims ;

  CascadeNode<IDTYPE, DIMTYPE>* root ;

  CascadeNode<IDTYPE, DIMTYPE>* resultNode ;

public:

  DynamicCascadeTree (int numOfDims, int skipListMaxHeight, 
                      DIMTYPE maxDimValue, IDTYPE maxIdValue) ;
  ~DynamicCascadeTree () ;

  void insertRegion (Region<IDTYPE, DIMTYPE> *region) ;
  void deleteRegion (Region<IDTYPE, DIMTYPE> *region) ;

  list<Region<IDTYPE, DIMTYPE>* >* pointQuery (Point<DIMTYPE>* point) ;
  list<Region<IDTYPE, DIMTYPE>* >* windowQuery 
    (Region<IDTYPE, DIMTYPE>* region) ;
} ;

template <typename IDTYPE, typename DIMTYPE>
DynamicCascadeTree<IDTYPE, DIMTYPE>::DynamicCascadeTree
  (int numOfDims, int skipListMaxHeight, 
   DIMTYPE maxDimValue, IDTYPE maxIdValue) {
  this->numOfDims = numOfDims ;
  this->skipListMaxHeight = skipListMaxHeight ;
  this->maxDimValue = maxDimValue ;
  this->maxIdValue = maxIdValue ;

  CascadeNode<IDTYPE, DIMTYPE> *prev = NULL ;

  for (int dimNo=0; dimNo<numOfDims; dimNo++) {
    CascadeNode<IDTYPE, DIMTYPE> *curr = new CascadeNode<IDTYPE, DIMTYPE> 
      (dimNo, skipListMaxHeight, maxDimValue, maxIdValue) ;
    if (root == NULL) {
      root = curr ;
    }

    if (prev != NULL) {
      prev->next = curr ;
    }

    prev = curr ;
  }

  // the result node
  CascadeNode<IDTYPE, DIMTYPE> *curr = new CascadeNode<IDTYPE, DIMTYPE>
    (-1, skipListMaxHeight, maxDimValue, maxIdValue) ;
  prev->next = curr ;
  resultNode = curr ;
}

template <typename IDTYPE, typename DIMTYPE>
DynamicCascadeTree<IDTYPE, DIMTYPE>::~DynamicCascadeTree () {
  CascadeNode<IDTYPE, DIMTYPE> *curr = root ;

  do {
    delete curr ;
    curr = curr->next ;
  } while (curr != NULL) ;
}

template <typename IDTYPE, typename DIMTYPE>
void DynamicCascadeTree<IDTYPE, DIMTYPE>::insertRegion
  (Region<IDTYPE, DIMTYPE> *region) {
//  struct tms tm_buf ;
//  times (&tm_buf) ;
//  cout << "before 1st insert time = " << (tm_buf.tms_utime+tm_buf.tms_stime) << "\n" ;

  root->insertRegion ((region->mins ())[0], region->id (), region, true) ;

//  times (&tm_buf) ;
//  cout << "after 1st insert time = " << (tm_buf.tms_utime+tm_buf.tms_stime) << "\n" ;

  root->insertRegion ((region->maxs ())[0], region->id (), region, false) ;

//  times (&tm_buf) ;
//  cout << "after 2nd insert time = " << (tm_buf.tms_utime+tm_buf.tms_stime) << "\n" ;
}

template <typename IDTYPE, typename DIMTYPE>
void DynamicCascadeTree<IDTYPE, DIMTYPE>::deleteRegion
  (Region<IDTYPE, DIMTYPE> *region) {
}

template <typename IDTYPE, typename DIMTYPE>
list<Region<IDTYPE, DIMTYPE>* >*
DynamicCascadeTree<IDTYPE, DIMTYPE>::pointQuery
  (Point<DIMTYPE>* point) {

  CascadeNode<IDTYPE, DIMTYPE> *node = root ;
  while (node != NULL &&
         node->getDimNo () != -1) {
    node->performPointQuery (point) ;
    node = node->next ;
  }

  return resultNode->resultRegions () ;

}

template <typename IDTYPE, typename DIMTYPE>
list<Region<IDTYPE, DIMTYPE>* >* 
DynamicCascadeTree<IDTYPE, DIMTYPE>::windowQuery
  (Region<IDTYPE, DIMTYPE>* region) {

  CascadeNode<IDTYPE, DIMTYPE> *node = root ;
  while (node != NULL &&
         node->getDimNo () != -1) {
    node->performWindowQuery (region) ;
    node = node->next ;
  }

  return resultNode->resultRegions () ;
}


#endif
