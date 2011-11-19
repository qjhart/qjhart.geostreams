#ifndef __spatialindex_dct_dct_h
#define __spatialindex_dct_dct_h

#include "Node.h"

namespace SpatialIndex {
  namespace DCT {

    template <int dims, int dimNo>
    class Interval {
    private:
      std::set<Node *, getLow<dimNo> > min_endp;
      std::set<Node *, getHigh<dimNo> > max_endp;
      std::set<Node *, getLow<dimNo> >::iterator current_node_max ;
      std::set<Node *, getHigh<dimNo> >::iterator current_node_min ;

      bool existOldMin ;
      bool existOldMax ;
      double oldMin, oldMax ;

    public:
      Interval<dims, dimNo+1> *cascade ;

      Interval () ;
      virtual ~Interval();

      int dimension() { return(dimNo); }
      void insert (Node *r) ;
      void remove (Node *r) ;
      void find (Tools::Geometry::Region *r) ;
      // std::list<Node*>* getAllRegions () ;
    } ;

    template <int dims, int dimNo>
    Interval<dims, dimNo>::Interval () {
      cascade = NULL ;
      current_node_max = (std::set<Node*, getLow<dimNo> >::iterator)NULL ;
      current_node_min = (std::set<Node*, getHigh<dimNo> >::iterator)NULL ;

      existOldMin = existOldMax = false ;
    }

    template <int dims, int dimNo>
    Interval<dims, dimNo>::~Interval () {
    }

    template <int dims, int dimNo>
    void Interval<dims, dimNo>::insert (Node *r) {
      min_endp.insert(r) ;
      max_endp.insert(r) ;

      if (
        ((current_node_max == min_endp.end ()) ||
         ((current_node_max !=
           ((std::set<Node*, getLow<dimNo> >::iterator)NULL)) &&
          (((*current_node_max)->getRegion ())->getLow(dimNo) >= 
            (r->getRegion ())->getLow(dimNo)))) &&

        ((current_node_min == max_endp.begin ()) ||
         ((current_node_min != max_endp.end ()) &&
          (((*current_node_min)->getRegion ())->getHigh(dimNo) <
          (r->getRegion ())->getHigh(dimNo))))) {

        if (cascade != NULL) {
          cascade->insert(r);
        }
      }
    }

    template <int dims, int dimNo>
    void Interval<dims, dimNo>::remove (Node *r) {
      if ((current_node_max !=
           ((std::set<Node*, getLow<dimNo> >::iterator)NULL)) &&
          (current_node_max != min_endp.end ()) &&
          ((*current_node_max)->getIdentifier () == r->getIdentifier ())) {
        
        if (current_node_max == min_endp.begin ()) {
          current_node_max = ((std::set<Node*, getLow<dimNo> >::iterator)NULL);
        } else {
          current_node_max --;
        }
      }

      if ((current_node_min !=
           ((std::set<Node*, getHigh<dimNo> >::iterator)NULL)) &&
          (current_node_min != max_endp.end ()) &&
          (*current_node_min)->getIdentifier () == r->getIdentifier ()) {
        current_node_min ++ ;
      }


      min_endp.erase (r) ;
      max_endp.erase (r) ;

      if (cascade != NULL) {
        cascade->remove(r);
      }
    }
	
    template <int dims, int dimNo>
    void Interval<dims, dimNo>::find (Tools::Geometry::Region *r) {

      double stab_min=r->getLow(dimNo);
      double stab_max=r->getHigh(dimNo);

      // Shrink Max (backup Current_node_max in min_endp)
      while ((current_node_max == min_endp.end ()) ||
        ((current_node_max !=
          ((std::set<Node*, getLow<dimNo> >::iterator)NULL)) &&
        ((*current_node_max)->getRegion ()->getLow (dimNo) >= stab_max))) {
        if (current_node_max == min_endp.end ()) {
          if (current_node_max == min_endp.begin ()) {
            current_node_max = 
              (std::set<Node*, getLow<dimNo> >::iterator)NULL ;
            break ;
          } else {
            current_node_max -- ;
            continue ;
          }
        }

        Tools::Geometry::Region *tempRegion =
          (*current_node_max)->getRegion () ;
        if (!existOldMin ||
            (tempRegion->getHigh(dimNo) > oldMin)) {
          cascade->remove (*current_node_max) ;
        }

        if (current_node_max == min_endp.begin ()) {
          current_node_max = (std::set<Node*, getLow<dimNo> >::iterator)NULL ;
        } else {
          current_node_max -- ;
        }
      }

      // Shrinking Min (move up current_node_min in max_endp
      while ((current_node_min ==
        ((std::set<Node*, getHigh<dimNo> >::iterator)NULL)) ||
        ((current_node_min != max_endp.end ()) &&
        ((*current_node_min)->getRegion ()->getHigh (dimNo) <= stab_min))) {
        if (current_node_min ==
          ((std::set<Node*, getHigh<dimNo> >::iterator)NULL)) {
          current_node_min = max_endp.begin () ;
          continue ;
        }

        Tools::Geometry::Region *tempRegion = 
          (*current_node_min)->getRegion () ;
        if (!existOldMax ||
            (oldMax >= tempRegion->getLow (dimNo))) {
          cascade->remove (*current_node_min) ;
        }

        current_node_min ++ ;
      }

      // Growing Max
      std::set<Node*, getLow<dimNo> >::iterator tempMax = current_node_max ;
      while ((current_node_max ==
        ((std::set<Node*, getLow<dimNo> >::iterator)NULL)) ||
             ((tempMax != min_endp.end ()) &&
              ((++tempMax) != min_endp.end ()) &&
              ((*tempMax)->getRegion ()->getLow (dimNo) < stab_max))) {

        if (current_node_max ==
          ((std::set<Node*, getLow<dimNo> >::iterator)NULL)) {
          current_node_max = min_endp.begin () ;
          tempMax = current_node_max ;

          if (tempMax == min_endp.end () ||
              (*tempMax)->getRegion ()->getLow (dimNo) >= stab_max) {
            break ;
          }
        } else {
          current_node_max ++ ;
        }

        if (current_node_max != min_endp.end ()) {
          Tools::Geometry::Region *tempRegion =
            (*current_node_max)->getRegion () ;
          if (tempRegion->getHigh (dimNo) > stab_min) {
            cascade->insert (*current_node_max) ;
          }
        }

        tempMax = current_node_max ;
      }

      // Growing Min
      std::set<Node*, getHigh<dimNo> >::iterator tempMin = current_node_min ;
      while ((tempMin != (std::set<Node*, getHigh<dimNo> >::iterator)NULL) &&
             (tempMin != max_endp.begin ()) &&
             (((*(--tempMin))->getRegion ()->getHigh (dimNo) > stab_min))) {
        current_node_min -- ;

        Tools::Geometry::Region *tempRegion =
          (*current_node_min)->getRegion () ;
        if (tempRegion->getLow (dimNo) < stab_max) {
          cascade->insert (*current_node_min) ;
        }

        tempMin = current_node_min ;
      }


      existOldMax = true ;
      oldMax = stab_max ;
      existOldMin = true ;
      oldMin = stab_min ;
    }

/*
    template <int dims, int dimNo> 
    std::list<Node*>* Interval<dims, dimNo>::getAllRegions () {
      std::list<Node*> *allRegions = new std::list<Node*> () ;

      std::set<Node*, getLow<dimNo> >::iterator itr = min_endp.begin () ;

      while (itr != min_endp.end ()) {
        allRegions->push_back (*itr) ;
      }

      return allRegions ;
    }*/

    class Interval<2, 2> {
    private:
      // std::set<Node *, getLow<2> > regions ;
      std::set<Node*, getId > regions ;

    public:
      Interval () ;
      virtual ~Interval();

      int dimension() { return(-1); }
      void insert (Node *r) ;
      void remove (Node *r) ;
      void find (Tools::Geometry::Region *r) ;
      std::list<Node*>* getAllRegions () ;
    } ;

    Interval<2,2>::Interval () {
    }

    Interval<2,2>::~Interval () {
    }

    void Interval<2,2>::insert (Node *r) {
      regions.insert (r) ;
    }

    void Interval<2,2>::remove (Node *r) {
      regions.erase (r) ;
    }

    void Interval<2,2>::find (Tools::Geometry::Region *r) {
    }

    std::list<Node*>* Interval<2,2>::getAllRegions () {
      std::list<Node*> *allRegions = new std::list<Node*> () ;

      std::set<Node*, getId >::iterator itr = regions.begin () ;

      while (itr != regions.end ()) {
        allRegions->push_back (*itr) ;
        itr++ ;
      }

      return allRegions ;
    }

    class DCT {

    private:
      int numOfDims ;
      Interval<2, 0> *root ;
      Interval<2, 2> *tail ;

    public:
      DCT (int numOfDims) ;
      virtual ~DCT () ;

      void insert (int id, Tools::Geometry::Region *r) ;
      bool remove (int id, Tools::Geometry::Region *r) ;
      std::list<Node*>* find (Tools::Geometry::Region *r) ;
    } ;

    DCT::DCT (int numOfDims) {
      this->numOfDims = numOfDims ;

      /*
      for (int dimNo=0; dimNo<numOfDims; dimNo++) {
        if (dimNo == 0) {
          root = new Interval (dimNo) ;
          tail = root; 
        } else {
          tail->cascade = new Interval (dimNo) ;
          tail = tail->cascade ;
        }
      }*/

      if (numOfDims == 2) {
        root = new Interval<2, 0> () ;

        root->cascade = new Interval<2, 1> () ;
        root->cascade->cascade = new Interval<2, 2> () ;
        tail = root->cascade->cascade ;
      }
    }

    DCT::~DCT() {
      /*
      Interval *temp = root ;
      while (temp != NULL) {
        Interval *temp1 = temp ;
        temp = temp->cascade ;
        delete temp1 ;
      }*/
      Interval<2, 1> *temp = root->cascade ;
      delete root ;
      delete temp ;
      delete tail ;
    }

    void DCT::insert(int id, Tools::Geometry::Region* r) {
      Node* node = new Node (id, r) ;
      root->insert (node) ;
    }

    bool DCT::remove (int id, Tools::Geometry::Region* r) {
      // root->remove (id, r) ;

      return false ;
    }

    std::list<Node*>* DCT::find
      (Tools::Geometry::Region* r) {
      /*
      Interval *node = root ;

      while (node != tail) {
        node->find (r) ;
        node = node->cascade ;
      }*/

      root->find (r) ;
      root->cascade->find (r) ;

      return tail->getAllRegions () ;
    }

    class Node ;
    template Interval<2,0> ;
    template Interval<2,1> ;
    template Interval<2,2> ;
    class DCT ;
  }
}

#endif
