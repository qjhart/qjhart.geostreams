#ifndef LINE_DOC_H
#define LINE_DOC_H

#include "include/types.h"
#include "include/gvar.h"
#include "gvarBlock/gvarBlock.h"
#include "gvarHeader/gvarHeader.h"
#include "utils/unpack/unpack10.h"

#define IMAGER_LINES_MAX 1974
#define LINEDOC_BSZ 32

#include <iostream>
using std::iostream ;
using std::ostream ;

class LineDoc{ 
private: 
  uint16 Spcid; //1 
  uint16 Spsid; // 2
  uint16 Lside; // 3
  uint16 Lidet; // 4
  

  uint16 Licha; // 5
  uint16 RisctMsb ; // 6 
  uint16 RisctLsb; // 7 

  uint16 L1scan;   //8
  uint16 L2scan;   //9

  uint16 LpixlsMsb; // 10 
  uint16 LpixlsLsb; // 11

  uint16 LwordsMsb; // 12 
  uint16 LwordsLsb; // 13

  uint16 Lzcor ;    // 14 
  uint16 Llag  ;    // 15 
  uint16 Lspar ;    // 16 

  // the data
  uint16* data ;

public: 
  LineDoc(GvarBlock* block) ; 
  ~LineDoc () ;

  int spcid() { return( Spcid ); }
  int spsid() { return( Spsid ); }
  int lside() { return( Lside ); }
  int lidet() { return( Lidet ); }
  int licha() { return( Licha ); }
  uint16 risct() {return(((uint16)RisctMsb<<10)+RisctLsb ); }
  int l1scan() { return( L1scan ) ; }
  int l2scan() { return( L2scan ) ; }
  int lpixls() { return( ((int)LpixlsMsb << 10) + LpixlsLsb ); } 
  int lwords() { return( ((int)LwordsMsb << 10) + LwordsLsb ); } 
  int lzcor() { return( Lzcor ); }
  int llag() { return( Llag ); }

  int channel() { return (Licha -1) ; }

  int detector(){ return( (Licha == 1) ? (Lidet+3)&7 : (Lidet-1)&1); }
// channel	logical det	normal Lidet	flip Lidet
// visible: norm: det = (Lidet+3)&7  flip: det = (12-Lidet)&7
//	1	0		5		4
//	1	1		6		3
//	1	2		7		2
//	1	3		8		1
//	1	4		1		8
//	1	5		2		7
//	1	6		3		6
//	1	7		4		5
// infrared: norm: det = (Lidet-1)&1  flip: det = (chan 3: 0, others: Lidet&1)
//	2	0		5		6
//	2	1		6		5
//	3	0		7		7
//	4	0		1		2
//	4	1		2		1
//	5	0		3		4
//	5	1		4		3
// BUT: the flip bit (bit 16 of Iscan) is not included with linedoc data so
//      I modify Lidet in the case of flip mode so that the above function
//      will return the correct logical detector value.
// visible: Lidet = (det+4)&7+1 = (9-Lidet) where det is the block number - 3
// infrared: Lidet = (2*c+d)%8+1 where c=channel, d=expected detector
// see gvar.C, list.C for more details 

  int side() { return((Lside)?1:0); }
  
  uint16* getData () { return data ; }
  
  LineDoc* next() { return (LineDoc*) ( (uint16*) this + lwords() ) ; }

  int valid();  
  void print( ostream & ) ; 

  int bytes(); 

}; 

inline int LineDoc::valid() {
  return (lpixls() < lwords() ) ; }

inline int LineDoc::bytes() { return(lwords()*10/8); }  

#endif


