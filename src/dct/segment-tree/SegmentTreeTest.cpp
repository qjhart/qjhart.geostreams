/**
 * This is the main file to test the performance for querying a set of given
 * data points in a segment tree of a given set of query boxes.
 *
 * The data for quey boxes is provided in a file, with the following 
 * format for each line:
 *
 *   id <dim1_min> <dim2_min> ... <dim1_max> <dim2_max> ....
 *
 * @author jzhang - created on Jun 23, 2004
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using namespace std ;

#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <utility>
#include <vector>
#include <iterator>
#include <list>
#include <CGAL/Segment_tree_k.h>
#include <CGAL/Range_segment_tree_traits.h>

#include "util/FileReader.h"

#define LINE_BUFFER_SIZE 1024

typedef CGAL::Cartesian<long> Representation;
typedef CGAL::Segment_tree_map_traits_2<Representation, int> Traits;
typedef CGAL::Segment_tree_2<Traits > Segment_tree_2_type;

typedef Traits::Interval Interval;
typedef Traits::Pure_interval Pure_interval;
typedef Traits::Key Key;

Segment_tree_2_type* buildSegmentTree_2 (char* filename) {

  int numOfDims = 2 ;

  // all query boxes
  list<Interval> queryBoxList ;

  FileReader *fr = new FileReader (filename) ;

  // the values in a line
  char* values[1+2*numOfDims] ;

  while (!fr->eof ()) {
    bool succeed = fr->getNextLine (values, 1+2*numOfDims) ;

    if (!succeed) {
      continue ;
    }
//cout << values[0] << " " << values[1] << " " << values[2] << " " << values[3] << " " << values[4] << "\n" ;
    queryBoxList.push_back 
      (Interval (Pure_interval (Key (atol (values[1]), atol (values[2])),
                                Key (atol (values[3]), atol (values[4]))),
       atoi(values[0]))) ;
  }

  delete fr ;
  for (int i=0; i<1+2*numOfDims; i++) {
    delete values[i] ;
  }

  // build the segment tree
  Segment_tree_2_type *Segment_tree_2 = new Segment_tree_2_type
    (queryBoxList.begin (), queryBoxList.end ()) ;

  if (Segment_tree_2->segment_tree_2->is_valid ()) {
    // cout << "Succeed.\n" ;
  } else {
    cout << "Failed.\n" ;
  }

  return Segment_tree_2 ;
}

void windowQuerySegmentTree_2 (Segment_tree_2_type *Segment_tree_2,
                               Interval *interval) {

/*  cout << "Make a query: (" << interval->first.first.x () << ", "
       << interval->first.first.y () << "), ("
       << interval->first.second.x () << ", "
       << interval->first.second.y () << ") ... \n" ;*/

  // window query
  list<Interval> resultList ;

  Segment_tree_2->window_query (*interval, back_inserter (resultList)) ;

  list<int> idList ;

  list<Interval>::iterator itr = resultList.begin () ;
  while (itr != resultList.end ()) {
    /*
    cout << (*itr).second << " (" << (*itr).first.first.x () << ", "
         << (*itr).first.first.y () << "), ("
         << (*itr).first.second.x () << ", "
         << (*itr).first.second.y () << ")\n" ;*/
    idList.push_back ((*itr).second) ;

    // cout << (*itr).first.first.x() << "-" << (*itr).first.second.x() << " " 
    //      << (*itr).first.first.y() << "-" << (*itr).first.second.y() 
    //       << ": " << (*itr).second << std::endl; 
    itr++;
  }

  idList.sort () ;
  cout << interval->second ;
  list<int>::iterator itr1 = idList.begin () ;
  while (itr1 != idList.end ()) {
    cout << " " << (*itr1) ;
    itr1 ++ ;
  }
  cout << "\n" ;
}

int main (int argc, char* argv[]) {

  if (argc < 3) {
    cerr << "./SegmentTreeTest <data_filename> <query_filename>\n" ;
    return 0 ;
  }

  char *dataFilename = argv[1] ;
  char *queryFilename = argv[2] ;

  // build the segment tree
  Segment_tree_2_type *Segment_tree_2 = buildSegmentTree_2 (dataFilename) ;

  // a window query
  // Interval interval = Interval(Pure_interval(Key(3,10), Key(12,15)),0);
  Interval interval = Interval(Pure_interval(Key(9000000,4500000), Key(9000001, 4500001)),0);
  windowQuerySegmentTree_2 (Segment_tree_2, &interval) ;

  // window queries
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

    Interval interval = 
      Interval(Pure_interval(Key(atol (values[1]), atol (values[2])), 
                             Key(atol (values[3]), atol (values[4]))),
               atoi (values[0]));
//    cout << "Query number " << values[0] << "\n" ;
    windowQuerySegmentTree_2 (Segment_tree_2, &interval) ;
 }

  for (int i=0; i<numOfWords; i++) {
    delete values[i] ;
  }

  delete fr ;

  delete Segment_tree_2 ;

  return 0 ;
}
