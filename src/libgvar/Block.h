#ifndef GVAR_BLOCK_H
#define GVAR_BLOCK_H

  /* refer page 61, goes-i/m oge specs  */

#define HEADER_BSZ 30  

#define WORD_COUNT_MIN 8040
#define WORD_COUNT_MAX 21008
#define WORD_SIZE_MAX 11
#define BLOCK_ID_MAX 12

#include <iostream>
#include "types.h"
#include "Block0Doc.h"
#include "LineDoc.h"
using std::iostream ;
using std::ostream ;

namespace Gvar {
  class Header{ 
  private:
	uchar8 BlockId;       // 1 
	uchar8 WordSize;      // 2 
	uchar8 WordCountMsb;  // 3	
	uchar8 WordCountLsb;  //  4 
	uchar8 ProductIdMsb;  // 5
	uchar8 ProductIdLsb;  // 6	
	uchar8 RepeatFlag;    // 7
	uchar8 VersionNumber; // 8
	uchar8 DataValid;     // 9
	uchar8 AsciiBin;      // 10
	uchar8 W11;           // 11
	uchar8 RangeWord;     // 12 
	uchar8 BlockCountMsb; // 13
	uchar8 BlockCountLsb; // 14 
	uchar8 W15_28[14];    // 15 28
	uchar8 CrcMsb;        //  29
	uchar8 CrcLsb;        // 30	

  public:
	Header (uchar8* header) ;

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
  
  inline uint16 Header::blockId() 
	{ return ( (BlockId)==240 ? 0: (BlockId) ) ; }
  
  inline uint16 Header::wordSize() 
	{ return WordSize; } 
  
  inline int Header::wordCount() 
	//{ return( ((int)(WordCountLsb) <<8)	+WordCountMsb) ; }
	{ return( ((int)(WordCountMsb) <<8)	+WordCountLsb) ;  }
  
  inline uint16 Header::productId() 
	//{ return( ((uint16) (ProductIdLsb) <<8) +ProductIdMsb ); }
	{ return( ((uint16) (ProductIdMsb) <<8) +ProductIdLsb ); }
  
  inline uint16 Header::repeatFlag()
	{ return RepeatFlag ; }
  
  inline uint16 Header::versionNumber()
	{ return VersionNumber; }
  
  inline uint16 Header::dataValid() 
	{ return DataValid ; }
  
  inline uint16 Header::asciiBin() 
	{ return AsciiBin; }
  
  inline uint16 Header::rangeWord() 
	{ return RangeWord; }
  
  inline uint16 Header::blockCount() 
	//{ return(((uint16) (BlockCountLsb)<<8)+BlockCountMsb ); }
	{ return(((uint16) (BlockCountMsb)<<8)+BlockCountLsb ); }
  
  inline uint16 Header::crc() 
	//{ return(((uint16) (CrcLsb)<<8) + CrcMsb); }
	{ return(((uint16) (CrcMsb)<<8) + CrcLsb); }
  
  inline	int Header::nBytes() 
	{ return( wordSize() * wordCount() / 8 ); }
  
  class Block { 
  private:
	
	Header* header ;
	uchar8* data;  // pointer to buffer for gvar data stream in memory
	
	Header* createHeader (char* buff) ;

	char* m_rawData ;
	int m_rawDataLen ;
	
  public:
	Block(char* buff, int buff_size); 
	~Block();
	uchar8* getData();//Find block of data and copy contents to Data
	
	Header* getHeader () ; // Return the header

	// Return the gvar raw data for this block
	inline char* getRawData () {
	  return m_rawData ;
	}
	// Return the length of the gvar raw data for this block
	inline int getRawDataLen () {
	  return m_rawDataLen ;
	}
  };

  class Block0 {
  private:
    // the pointer to GvarBlock
    Block* block ;

    // the line document of the data
    Block0Doc* block0Doc ;

  public:
    //Block0 (Block* b):block(b), block0Doc(new Block0Doc(b)) {} ;
    Block0 (Block* b) ;
    ~Block0 () ;

    Block* getBlock () ;
    Block0Doc* getBlock0Doc ();
  };
  
  inline Block* Block0::getBlock () { return block ; }
  inline Block0Doc* Block0::getBlock0Doc () { return block0Doc ; }


  class Block1or2 {
  private:
    // the pointer to GvarBlock
    Block* block ;

    // the line document of the data
    LineDoc** lineDoc;  

    // the number of linedocs
    int numOfLineDocs ;

    // the pixel data
    //uint16* data[4] ;

  public:
    Block1or2 (Block*) ;
    ~Block1or2 () ;

    Block* getBlock () ;
    LineDoc* getLineDoc (int) ;
    uint16* getData (int) ;
    // uint16** getData () ;
    int getDataLen (int) ;
    int getNumLineDocs () ;
  };

  inline Block* Block1or2::getBlock () { return block ; }
  inline LineDoc* Block1or2::getLineDoc (int i) { return lineDoc[i] ; }
  inline int Block1or2::getNumLineDocs () { return numOfLineDocs ; } 
 
  class Block3to10 {	
  private:
    // the pointer to GvarBlock
    Block* block ;
	
    // the line document of the data
    LineDoc* lineDoc ;
	
    // the pixel data
    uint16* data ;
	
    // the number of pixels in data
    int dataLen ;
	
  public:
    Block3to10 (Block*) ;

    ~Block3to10 () ;
	
    Block* getBlock () ;
    LineDoc* getLineDoc () ;
    uint16*  getData () ;
    int getDataLen () ;
  } ;
  
  inline Block* Block3to10::getBlock () { return block ; }
}
#endif // GVAR_H
  

