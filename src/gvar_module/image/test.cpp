#include "point_xy.h" 
using namespace std ;

int main () {
  Point_XY<int> *point = new Point_XY<int>(3,4) ;

  point->print () ;

  return 0 ;
}