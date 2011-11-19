#ifndef GVARHEADER_H
#define GVARHEADER_H

#include <string.h>
#include "include/types.h"

#include <iostream>
using std::iostream ;
using std::ostream ;

/* refer page 61, goes-i/m oge specs  */

#define HEADER_BSZ 30  

#define WORD_COUNT_MIN 8040
#define WORD_COUNT_MAX 21008
#define WORD_SIZE_MAX 11
#define BLOCK_ID_MAX 12

class GvarHeader{ 
 private:
        uchar8 BlockId;  // 1 
        uchar8 WordSize;  // 2 
        uchar8 WordCountMsb; // 3	
        uchar8 WordCountLsb; //  4 
        uchar8 ProductIdMsb; // 5
        uchar8 ProductIdLsb; // 6	
        uchar8 RepeatFlag;  // 7
        uchar8 VersionNumber; // 8
        uchar8 DataValid;    // 9
        uchar8 AsciiBin;    // 10
        uchar8 W11;          // 11
        uchar8 RangeWord;    // 12 
        uchar8 BlockCountMsb;   // 13
        uchar8 BlockCountLsb; // 	14 
        uchar8 W15_28[14];    // 15 28
        uchar8 CrcMsb;  //  29
	uchar8 CrcLsb; // 30	

public:

        /*
	GvarHeader(uchar8 BlockId,
                   uchar8 WordSize,
                   uchar8 WordCountMsb,
              uchar8 WordCountLsb,
              uchar8 ProductIdMsb,
              uchar8 ProductIdLsb,
              uchar8 RepeatFlag,
              uchar8 VersionNumber,
              uchar8 DataValid,
              uchar8 AsciiBin,
              uchar8 W11,
              uchar8 RangeWord,
              uchar8 BlockCountMsb,
              uchar8 BlockCountLsb,
              uchar8 W15_28[14],
              uchar8 CrcMsb,
	      uchar8 CrcLsb); */
        GvarHeader (uchar8* header) ;

	int isValidHeader() ; 

	uint16 blockId();
	uint16 wordSize();
	int wordCount();
	uint16 productId();
	uint16 repeatFlag();
	uint16 versionNumber();
	uint16 dataValid();
	uint16 asciiBin();
	uint16 rangeWord();
	uint16 blockCount();
	uint16 crc();
	int nBytes();

	void print(ostream & out) ; 
};

inline uint16 GvarHeader::blockId() 
{ return ( (BlockId)==240 ? 0: (BlockId) ) ; }

inline uint16 GvarHeader::wordSize() 
{ return WordSize; } 

inline int GvarHeader::wordCount() 
     //{ return( ((int)(WordCountLsb) <<8)	+WordCountMsb) ; }
{ return( ((int)(WordCountMsb) <<8)	+WordCountLsb) ;  }

inline uint16 GvarHeader::productId() 
     //{ return( ((uint16) (ProductIdLsb) <<8) +ProductIdMsb ); }
{ return( ((uint16) (ProductIdMsb) <<8) +ProductIdLsb ); }

inline uint16 GvarHeader::repeatFlag()
{ return RepeatFlag ; }

inline uint16 GvarHeader::versionNumber()
{ return VersionNumber; }

inline uint16 GvarHeader::dataValid() 
{ return DataValid ; }

inline uint16 GvarHeader::asciiBin() 
{ return AsciiBin; }

inline uint16 GvarHeader::rangeWord() 
{ return RangeWord; }

inline uint16 GvarHeader::blockCount() 
     //{ return(((uint16) (BlockCountLsb)<<8)+BlockCountMsb ); }
{ return(((uint16) (BlockCountMsb)<<8)+BlockCountLsb ); }

inline uint16 GvarHeader::crc() 
     //{ return(((uint16) (CrcLsb)<<8) + CrcMsb); }
{ return(((uint16) (CrcMsb)<<8) + CrcLsb); }

inline	int GvarHeader::nBytes() 
{ return( wordSize() * wordCount() / 8 ); }

#endif




