#ifndef __spatialindex_dct_node_h
#define __spatialindex_dct_node_h

namespace SpatialIndex {
  namespace DCT {

    class Node {
    private:
      long m_identifier; // The unique ID of this node.
      Tools::Geometry::Region* m_region ; // Region for this node	
    public:

      Node(long id, Tools::Geometry::Region* region);
      virtual ~Node();

      //
      // SpatialIndex::IEntry interface
      //
      virtual long getIdentifier() const;
      virtual void getShape(IShape** out) const throw (std::exception);

      Tools::Geometry::Region* getRegion () const ;
      friend std::ostream& operator<<(std::ostream& os, const Node& n);
    }; // Node

    Node::Node (long id, Tools::Geometry::Region *region) {
      m_identifier = id ;
      m_region = region ;
    }

    Node::~Node () {
    }

    long Node::getIdentifier () const {
      return m_identifier ;
    }

    void Node::getShape (IShape** out) const
    throw (std::exception) {
    }

    Tools::Geometry::Region* Node::getRegion () const {
      return m_region ;
    }

    std::ostream& operator<<(std::ostream& os,const Node &n) {
      unsigned long i;
      Region* r = n.m_region;
   
      os << n.getIdentifier() <<":";
      os << "[";
      for (i = 0; i < r->m_dimension-1; i++) {
        os << r->m_pLow[i] << " ";
      }
      os << r->m_pLow[i];
   
      os << ",";
   
      for (i = 0; i < r->m_dimension-1; i++) {
        os << r->m_pHigh[i] << " ";
      }
      os << r->m_pHigh[i];
      os <<"]";
   
      return os;
    }

    template <int I> struct getLow {
      bool operator()(Node* r1, Node* r2) const {

        if (I == r1->getRegion ()->getDimension ()) {  // sort by id
          return r1->getIdentifier () < r2->getIdentifier () ;
        } 

        // sort by the dimensional value and id
        else if (r1->getRegion ()->getLow(I) == r2->getRegion ()->getLow(I) ) {
          return r1->getIdentifier () < r2->getIdentifier () ;
        } else {
          return r1->getRegion ()->getLow(I) < r2->getRegion ()->getLow(I);
        }
      }
    };

    template <int I> struct getHigh {
      bool operator()(Node* r1, Node* r2) const {
        if (I == r1->getRegion ()->getDimension ()) {  // sort by id
          return r1->getIdentifier () < r2->getIdentifier () ;
        } 

        // sort by the dimensional value and id
        else if (r1->getRegion ()->getHigh(I) == r2->getRegion ()->getHigh(I)){
          return r1->getIdentifier () < r2->getIdentifier ();
        } else {
          return r1->getRegion ()->getHigh(I) < r2->getRegion ()->getHigh(I);
        }
      }
    };

    struct getId {
      bool operator() (Node* r1, Node* r2) const {
        return r1->getIdentifier () < r2->getIdentifier () ;
      }
    } ;

  }
}
#endif // __dct_node_h
