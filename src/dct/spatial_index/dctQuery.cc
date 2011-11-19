/**
 * Testing the DCT.
 *
 * @author jzhang - created on July 28, 2004
 */
#include <SpatialIndex.h>
#include <Tools.h>
#include "DCT.h"
#include "Node.h"

using namespace SpatialIndex ;
using namespace std ;

#define INSERT 1
#define DELETE 0
#define QUERY 2

class MyVisitor : public IVisitor
{
list<int> idList ;

public:

  void visitNode(const INode& n) {}

  void visitData(const IData& d) {
    // cout << " " << d.getIdentifier() ;
    idList.push_back (d.getIdentifier ()) ;
  }

  void visitData(std::vector<const IData*>& v) {}

  void printList () {
    idList.sort () ;

    list<int>::iterator itr = idList.begin () ;
    while (itr != idList.end ()) {
      cout << " " << (*itr) ;
      itr ++ ;
    }

    cout << "\n" ;
  }
};

int main (int argc, char** argv) {

  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <data_file>\n" ;
    return -1 ;
  }

  ifstream inFile (argv[1]) ;
  if (!inFile) {
    cerr << "Can not open the file " << argv[1] << ".\n" ;
    return -1 ;
  }

  try {

    unsigned long id, operation ;
    double xMin, yMin, xMax, yMax ;
    double mins[2];
    double maxs[2] ;

    SpatialIndex::DCT::DCT *dct = new SpatialIndex::DCT::DCT (2) ;

    Tools::ResourceUsage ru ;

    ru.start () ;

    // build the tree
    while (inFile) {
      inFile >> operation >> id >> xMin >> yMin >> xMax >> yMax ;

      // skip the line if there is an error
      if (!inFile.good ()) continue ;

      mins[0] = xMin; 
      mins[1] = yMin ;
      maxs[0] = xMax; 
      maxs[1] = yMax ;

      Tools::Geometry::Region* region =
        new Tools::Geometry::Region (mins, maxs, 2) ;

// cout << region->getLow (0) << "," << region->getLow (1) << "\n" ;
// cout << region->getHigh (0) << "," << region->getHigh (1) << "\n" ;

      if (operation == INSERT) {
        dct->insert (id, region) ;
      }

      else if (operation == DELETE) {
        if (dct->remove (id, region) == false) {
          cerr << "******ERROR******" << endl;
          cerr << "Cannot delete id: " << id << "\n" ;
          return -1 ;
        }
      }

      else if (operation == QUERY) {
        std::list<SpatialIndex::DCT::Node*>* allRegions = dct->find (region) ;

        cout << id ;
        std::list<SpatialIndex::DCT::Node*>::iterator itr = allRegions->begin () ;
        while (itr != allRegions->end ()) {
          cout << " " << (*itr)->getIdentifier () ;
          itr ++ ;
        }
        cout << "\n" ;

        delete allRegions ;
        delete region ;
      }
    }

    ru.stop () ;

    cerr << "Seconds: " << ru.getTotalTime () << "\n" ;

    delete dct ;

  } catch (Tools::Exception& e) {
    cerr << "******ERROR******" << endl;
    std::string s = e.what();
    cerr << s << endl;
    return -1;
  } catch (...) {
    cerr << "******ERROR******" << endl;
    cerr << "other exception" << endl;
    return -1;
  }
}
