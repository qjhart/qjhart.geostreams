#ifndef CASCADENODE_H
#define CASCADENODE_H

#include <list>

#include "SkipList.h"
#include "Point.h"
#include "Region.h"

/**
 * The data structure for a node in the Dynamice Cascade Tree.
 *
 * @author jzhang - created on Jul 01, 2004
 */

template <typename IDTYPE,typename DIMTYPE>
class CascadeNode {

private:
  int skipListMaxHeight ;
  DIMTYPE maxDimValue ;
  IDTYPE maxIdValue ;

  // The dimension number
  // -1 represents this node stores the result regions that intersect
  // with a query.
  int dimNo ;

  // The lists to maintain regions for the min and max values for 
  // each dimension.
  // These lists are null if this is the result node.
  SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *minList ;
  SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *maxList ;

  // a pointer to point to the node that has been accessed last
  // in the previous search.
  // SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > 
  //   *currMinAtMinList ;
  SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > 
    *currMaxAtMinList ;
  SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > 
    *currMinAtMaxList ;
  // SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > 
  //  *currMaxAtMaxList ;

  DIMTYPE oldMin ;
  DIMTYPE oldMax ;
  bool existOldMin ;
  bool existOldMax ;

  // to maintain the result regions.
  // This list is null if this is not the result node.
  SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> >* resultList ;

  // insert a region to the next cascade node.
  void cascadeInsert (Region<IDTYPE, DIMTYPE> *region) ;

  // delete a region in the next cascade node.
  void cascadeDelete (Region<IDTYPE, DIMTYPE> *region) ;

public:

  // The next node for this cascade node
  CascadeNode *next ;

  CascadeNode (int dimNo, int skipListMaxHeight, 
               DIMTYPE maxDimValue, IDTYPE maxIdValue) ;
  ~CascadeNode () ;

  int getDimNo () ;

  void insertRegion (DIMTYPE dimValue, IDTYPE idValue,
                     Region<IDTYPE, DIMTYPE> *region, bool isMin) ;
  void insertRegion (IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region) ;
  void deleteRegion (DIMTYPE dimValue, IDTYPE idValue,
                     Region<IDTYPE, DIMTYPE> *region, bool isMin) ;
  void deleteRegion (IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region) ;

  void performPointQuery (Point<DIMTYPE>* point) ;
  void performWindowQuery (Region<IDTYPE, DIMTYPE>* region) ;
  list<Region<IDTYPE, DIMTYPE>*>* resultRegions () ;
};

template <typename IDTYPE, typename DIMTYPE>
CascadeNode<IDTYPE, DIMTYPE>::CascadeNode (int dimNo, int skipListMaxHeight, 
  DIMTYPE maxDimValue, IDTYPE maxIdValue) {
  this->dimNo = dimNo ;
  this->skipListMaxHeight = skipListMaxHeight ;
  this->maxDimValue = maxDimValue ;
  this->maxIdValue = maxIdValue ;

  existOldMin = false ;
  existOldMax = false ;

  if (dimNo == -1) {
    minList = NULL ;
    maxList = NULL ;
    // currMinAtMinList = NULL ;
    currMaxAtMinList = NULL ;
    currMinAtMaxList = NULL ;
    // currMaxAtMaxList = NULL ;
    resultList = new SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> >
      (0.5, skipListMaxHeight, &(this->maxIdValue)) ;
  } else {
    minList = 
      new SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > >
      (0.5, skipListMaxHeight, &(this->maxDimValue)) ;
    maxList = 
      new SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > >
      (0.5, skipListMaxHeight, &(this->maxDimValue)) ;
    // currMinAtMinList = minList->getHead () ;
    currMaxAtMinList = minList->getHead () ;
    currMinAtMaxList = maxList->getHead () ;
    // currMaxAtMaxList = maxList->getHead () ;
    resultList = NULL ;
  }

  next = NULL ;
}

template <typename IDTYPE, typename DIMTYPE>
CascadeNode<IDTYPE, DIMTYPE>::~CascadeNode () {
  if (minList != NULL) delete minList ;
  if (maxList != NULL) delete maxList ;
  if (resultList != NULL) delete resultList ;
}

template <typename IDTYPE, typename DIMTYPE>
int CascadeNode<IDTYPE, DIMTYPE>::getDimNo () {
  return dimNo ;
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::insertRegion
  (DIMTYPE dimValue, IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region,
   bool isMin) {
  if (dimNo == -1) {
    return ;
  }

  SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *regionList ;
  if (isMin) {
    regionList = minList ;
  } else {
    regionList = maxList ;
  }

  SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *node =
    regionList->retrieve (&dimValue) ;

  if (node == NULL) {
    SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *idList =
      new SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > 
        (0.5, skipListMaxHeight, &maxIdValue) ;
    IDTYPE* idTemp = new IDTYPE ;
    *idTemp = idValue ;
    idList->insert (idTemp, region) ;

    DIMTYPE* dimTemp = new DIMTYPE ;
    *dimTemp = dimValue ;
    bool succeed = regionList->insert (dimTemp, idList) ;

    if (!succeed) {
      cerr << "Can not insert the value " << dimValue << "\n" ;
    }
  } else {
    SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *idList = 
      node->getObj () ;
    IDTYPE *idTemp = new IDTYPE ;
    *idTemp = idValue ;
    idList->insert (idTemp, region) ;
  }

   if (currMaxAtMinList != minList->getHead () &&
       currMinAtMaxList != maxList->getHead () &&
       (region->maxs ())[dimNo] > *(currMinAtMaxList->getKey ()) &&
       *(currMaxAtMinList->getKey ()) >= (region->mins ())[dimNo]) {

    if (next != NULL) {
      cascadeInsert (region) ;
    }
  }
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::insertRegion
  (IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region) {
  if (dimNo != -1) {
    return ;
  }

  IDTYPE *idTemp = new IDTYPE ;
  *idTemp = idValue ;
  resultList->insert (idTemp, region) ;
}


template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::deleteRegion
  (DIMTYPE dimValue, IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region,
   bool isMin) {
  if (dimNo == -1) {
    return ;
  }

  SkipList<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *regionList ;
  if (isMin) {
    regionList = minList ;
  } else {
    regionList = maxList ;
  }

  SkipNode<DIMTYPE, SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > > *node =
    regionList->retrieve (&dimValue) ;

  if (node != NULL) {
    SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *idList = node->getObj () ;
    if (idList != NULL) {

      idList->removeWithKey (&idValue) ;

      if (idList->isEmpty ()) {
        if (currMaxAtMinList == node) {
          currMaxAtMinList = currMaxAtMinList->prev () ;
        }

        if (currMinAtMaxList == node) {
          currMinAtMaxList = currMinAtMaxList->next () ;
        }

        regionList->removeWithKey (&dimValue) ;
        delete idList ;
      }
    }
  }  

  if (next != NULL) {
    cascadeDelete (region) ;
  }
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::deleteRegion
  (IDTYPE idValue, Region<IDTYPE, DIMTYPE> *region) {
  if (dimNo != -1) {
    return ;
  }

  resultList->removeWithKey (&idValue) ;
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::performPointQuery (Point<DIMTYPE>* point) {

  Region<IDTYPE, DIMTYPE> *r = 
    new Region<IDTYPE, DIMTYPE> (0, point, point) ;

  performWindowQuery (r) ;

/*
  if (dimNo == -1) {
    return ;
  }
  
  DIMTYPE oldValue ;
  bool existOldValue = false ;

  if (currMinNode != regionList->getHead ()) {
    oldValue = *(currMinNode->getKey ()) ;
    existOldValue = true ;
  }

  while ((currMinNode != regionList->getHead () &&
           ((point->dims ())[dimNo] < *(currMinNode->getKey ())))) {
      
      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMinNode->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;
      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if ((r->mins ())[dimNo] == *(currMinNode->getKey ())) {
          if (existOldValue &&
              (r->maxs ())[dimNo] > oldValue) {
            cascadeDelete (r) ;
          }
        } else if ((r->mins ())[dimNo] <= (point->dims ())[dimNo]) {
          cascadeInsert (r) ;
        }
        idNode = idNode->next () ;
      }

      currMinNode = currMinNode->prev () ;
    }


  while ((currMinNode->next () != regionList->getTail () &&
    ((point->dims ())[dimNo] > *(currMinNode->next()->getKey ())))) {
      currMinNode = currMinNode->next () ;

      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMinNode->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;
      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if ((r->maxs ())[dimNo] == *(currMinNode->getKey ())) {
          if (existOldValue &&
              (oldValue >= (r->mins ())[dimNo])) {
            cascadeDelete (r) ;
          }
        } else if ((r->maxs ())[dimNo] > (point->dims ())[dimNo]) {
          cascadeInsert (r) ;
        }
        idNode = idNode->next () ;
      }
    }
*/
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::performWindowQuery
  (Region<IDTYPE, DIMTYPE>* region) {

  if (dimNo == -1) {
    return ;
  }

  // max is shrinking

    while ((currMaxAtMinList == minList->getTail ()) ||
           ((currMaxAtMinList != minList->getHead ()) &&
           ((region->maxs ())[dimNo] <= *(currMaxAtMinList->getKey ())))) {
      if (currMaxAtMinList == minList->getTail ()) {
        currMaxAtMinList = currMaxAtMinList->prev () ;
        // continue ;
      }

      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMaxAtMinList->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;

      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if (!existOldMin ||
            ((r->maxs ())[dimNo] > oldMin)) {
          cascadeDelete (r) ;
        }

        idNode = idNode->next () ;
      }

      currMaxAtMinList = currMaxAtMinList->prev () ;
    }

  // min is shrinking

    while ((currMinAtMaxList == maxList->getHead ()) ||
           ((currMinAtMaxList != maxList->getTail ()) &&
           ((region->mins ())[dimNo] >= *(currMinAtMaxList->getKey ())))) {

      if (currMinAtMaxList == maxList->getHead ()) {
        currMinAtMaxList = currMinAtMaxList->next () ;
        // continue ;
      }

      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMinAtMaxList->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;
      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if (!existOldMax ||
            (r->mins ())[dimNo] <= oldMax) {
          cascadeDelete (r) ;
        }

        idNode = idNode->next () ;
      }

      currMinAtMaxList = currMinAtMaxList->next () ;
    }

  // max is growing
    while ((currMaxAtMinList != minList->getTail ()) &&
      (*(currMaxAtMinList->next ()->getKey ()) < (region->maxs ())[dimNo])) {

      currMaxAtMinList = currMaxAtMinList->next () ;

      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMaxAtMinList->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;
      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if ((r->maxs ())[dimNo] > (region->mins ())[dimNo]) {
          cascadeInsert (r) ;
        }
    
        idNode = idNode->next () ;
      }
    }

  // min is growing
    while ((currMinAtMaxList->prev () != maxList->getHead ()) &&
      ((region->mins ())[dimNo] < *(currMinAtMaxList->prev()->getKey ()))) {

      currMinAtMaxList = currMinAtMaxList->prev () ;

      SkipList<IDTYPE, Region<IDTYPE, DIMTYPE> > *dimList = 
        currMinAtMaxList->getObj () ;
      SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> > *idNode = 
        dimList->getHead ()->next () ;
      while (idNode != dimList->getTail ()) {
        Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

        if ((r->mins ())[dimNo] < (region->maxs ())[dimNo]) {
          cascadeInsert (r) ;
        }

        idNode = idNode->next () ;
      }

    }

  existOldMax = true ;
  oldMax = (region->maxs ())[dimNo] ;
  existOldMin = true ;
  oldMin = (region->mins ())[dimNo] ;
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::cascadeInsert
  (Region<IDTYPE, DIMTYPE> *region) {
  IDTYPE idValue = region->id () ;

  int nextDimNo = next->getDimNo () ;
  if (nextDimNo == -1) {
    next->insertRegion (idValue, region) ;
  } else {
    next->insertRegion ((region->mins ())[nextDimNo], idValue, region, true) ;
    next->insertRegion ((region->maxs ())[nextDimNo], idValue, region, false) ;
  }
}

template <typename IDTYPE, typename DIMTYPE>
void CascadeNode<IDTYPE, DIMTYPE>::cascadeDelete
  (Region<IDTYPE, DIMTYPE> *region) {
  IDTYPE idValue = region->id () ;

  int nextDimNo = next->getDimNo () ;
  if (nextDimNo == -1) {
    next->deleteRegion (idValue, region) ;
  } else {
    next->deleteRegion ((region->mins ())[nextDimNo], idValue, region, true) ;
    next->deleteRegion ((region->maxs ())[nextDimNo], idValue, region, false) ;
  }
}

template <typename IDTYPE, typename DIMTYPE>
list<Region<IDTYPE, DIMTYPE> *> *
CascadeNode<IDTYPE, DIMTYPE>::resultRegions () {
  if (dimNo != -1) {
    return NULL ;
  }

  list<Region<IDTYPE, DIMTYPE>* >* regions =
    new list<Region<IDTYPE, DIMTYPE>* > () ;
  SkipNode<IDTYPE, Region<IDTYPE, DIMTYPE> >* idNode =
    resultList->getHead ()->next () ;
  while (idNode != resultList->getTail ()) {
    Region<IDTYPE, DIMTYPE> *r = idNode->getObj () ;

    regions->push_back (r) ;
    idNode = idNode->next () ;
  }

  return regions ;
}

#endif
