/**
 * This is a test for DynamicCascadeTree.
 *
 * @author jzhang - created on Jun 29, 2004
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std ;

#include "DynamicCascadeTree.h"
#include "util/FileReader.h"

DynamicCascadeTree<int, double>* buildDCT 
  (char* filename, double maxDimValue, int maxIdValue) {
  int numOfDims = 2 ;

  DynamicCascadeTree<int, double> *dct =
    new DynamicCascadeTree<int, double> (2, 32, maxDimValue, maxIdValue) ;

  FileReader *fr = new FileReader (filename) ;

  // the values in a line
  char* values[1+2*numOfDims] ;

  while (!fr->eof ()) {
    bool succeed = fr->getNextLine (values, 1+2*numOfDims) ;

    if (!succeed) {
      continue ;
    }

    Region<int, double> *r =
      new Region<int, double> (atoi( values[0]) ,
        new Point_2<double>(strtod(values[1],(char **)NULL), strtod(values[2],(char **)NULL)),
        new Point_2<double>(strtod(values[3],(char **)NULL), strtod(values[4],(char **)NULL))) ;
    dct->insertRegion (r) ;
  }

  delete fr ;
  for (int i=0; i<1+2*numOfDims; i++) {
    delete values[i] ;
  }
  return dct ;
}

int main (int argc, char* argv[]) {
  if (argc < 5) {
    cerr << "./DynamicCascadeTree_Test <data_filename> <query_filename> "
         << "<max_dim_value> <max_id_value>\n" ;
    return 0 ;
  }

  char *dataFilename = argv[1] ;
  char *queryFilename = argv[2] ;
  double maxDimValue = strtod (argv[3],(char **)NULL) ;
  int maxIdValue = atoi (argv[4]) ;

  // build DCT

  DynamicCascadeTree<int, double> *dct =
    buildDCT (dataFilename, maxDimValue, maxIdValue) ;

  // query
  FileReader *fr = new FileReader (queryFilename) ;

  int numOfDims = 2 ;
  int numOfWords = 1+2*numOfDims ;
  // the values in a line
  char* values[numOfWords] ;

  while (!fr->eof ()) {
    bool succeed = fr->getNextLine (values, numOfWords) ;

    if (!succeed) {
      cerr << "Ignore a query...\n" ;
      continue ;
    }

//    cout << "Query number " << values[0] << "\n" ;

    Region<int, double> *r =
      new Region<int, double> (atoi( values[0]) ,
        new Point_2<double>(strtod(values[1],(char **)NULL), strtod(values[2],(char **)NULL)),
        new Point_2<double>(strtod(values[3],(char **)NULL), strtod(values[4],(char **)NULL))) ;

    list<Region<int, double>*> *resultList =
      dct->windowQuery (r) ;

//  cout << "result size=" << resultList->size () << "\n" ;

//    cout << values[0] ;
    list<int> idList ;

    list<Region<int, double>*>::iterator itr =
      resultList->begin () ;
    while (itr != resultList->end ()) {
      idList.push_back ((*itr)->id ()) ;
      itr++ ;
    }

    idList.sort () ;

    cout << values[0] ;
    list<int>::iterator itr1 = idList.begin () ;
    while (itr1 != idList.end ()) {
      cout << " " << (*itr1) ;
      itr1 ++ ;
    }

    cout << "\n" ;
    delete resultList ;
  }

  for (int i=0; i<numOfWords; i++) {
    delete values[i] ;
  }

  delete fr ;
  delete dct ;
}
