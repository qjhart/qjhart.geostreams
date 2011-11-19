/**
 * Testing the R-tree's main-memory storage manager.
 *
 * @author jzhang - created on July 28, 2004
 */
#include <SpatialIndex.h>

using namespace SpatialIndex ;
using namespace std ;

#define INSERT 1
#define DELETE 0
#define QUERY 2

class MyVisitor : public IVisitor
{
// list<int> idList ;
set<int> idList ;

public:

  void visitNode(const INode& n) {}

  void visitData(const IData& d) {
    // cout << " " << d.getIdentifier() ;
    // idList.push_back (d.getIdentifier ()) ;
    idList.insert (d.getIdentifier ()) ;
  }

  void visitData(std::vector<const IData*>& v) {}

  void printList () {
    // idList.sort () ;

    // list<int>::iterator itr = idList.begin () ;
    set<int>::iterator itr = idList.begin () ;
    while (itr != idList.end ()) {
      cout << " " << (*itr) ;
      itr ++ ;
    }

    cout << "\n" ;
  }
};

int main (int argc, char** argv) {

  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <data_file> capacity\n" ;
    return -1 ;
  }

  ifstream dataIn (argv[1]) ;
  if (!dataIn) {
    cerr << "Can not open the file " << argv[1] << ".\n" ;
    return -1 ;
  }

  try {
    IStorageManager *mem = StorageManager::createNewMemoryStorageManager() ;

    long indexIdentifier;
    ISpatialIndex* tree = 
      RTree::createNewRTree(*mem, 0.7, atoi(argv[2]), atoi(argv[2]), 
                            2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

    unsigned long id, operation ;
    double xMin, yMin, xMax, yMax ;
    double mins[2], maxs[2] ;

    Tools::ResourceUsage ru ;

    ru.start () ;

    // build the tree
    while (dataIn) {
      dataIn >> operation >> id >> xMin >> yMin >> xMax >> yMax ;

      // skip the line if there is an error
      if (!dataIn.good ()) continue ;

      mins[0] = xMin; mins[1] = yMin ;
      maxs[0] = xMax; maxs[1] = yMax ;

      Region r = Region (mins, maxs, 2) ;

      if (operation == INSERT) {
        tree->insertData (0, 0, r, id) ;
      }

      else if (operation == DELETE) {
        if (tree->deleteData (r, id) == false) {
          cerr << "******ERROR******" << endl;
          cerr << "Cannot delete id: " << id << "\n" ;
          return -1 ;
        }
      }

      else if (operation == QUERY) {
        MyVisitor vis ;
        tree->intersectsWithQuery (r, vis) ;
        cout << id ;
        vis.printList () ;
      }
    }

    ru.stop () ;

    cerr << "Seconds: " << ru.getTotalTime () << "\n" ;

    bool ret = tree->isIndexValid () ;
    if (ret == false) {
      cerr << "ERROR: Structure is invalid!\n" ;
    } else {
      cerr << "The stucture seems O.K\n" ;
    }

    delete tree ;
    delete mem ;

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
