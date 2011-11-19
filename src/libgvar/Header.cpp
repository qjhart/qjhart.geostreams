#include "Gvar.h"

#define WORD_COUNT_MIN 8040
#define WORD_COUNT_MAX 21008
#define WORD_SIZE_MAX 11
#define BLOCK_ID_MAX 12

namespace Gvar {
  Header::Header (uchar8* header) {

	for (int i=0; i<30; i++) {
	  if (&header[i] == NULL) {
		printf ("This is not a valid header.\n") ;
		return ;
	  }
	}
	
	BlockId = header[0] ;
	WordSize = header[1] ;
	WordCountMsb = header[2] ;
	WordCountLsb = header[3] ;
	ProductIdMsb = header[4] ;
	ProductIdLsb = header[5] ;
	RepeatFlag = header[6] ;
	VersionNumber = header[7] ;
	DataValid = header[8] ;
	AsciiBin = header[9] ;
	W11 = header[10] ;
	RangeWord = header[11] ;
	BlockCountMsb = header[12] ;
	BlockCountLsb = header[13] ;
	
	for (int i=0; i<14; i++) {
	  W15_28[i] = header[14+i] ;
	}
	
	CrcMsb = header[28] ;
	CrcLsb = header[29] ;
  }
  
  int Header::isValidHeader() { 
	return((wordSize() != 0 ) 
		   && (wordCount() != 0 )  
		   && (blockCount() != 0 )    
		   && (blockId() < BLOCK_ID_MAX) 
		   && (wordSize() < WORD_SIZE_MAX) 
		   && (wordCount() < WORD_COUNT_MAX)  
		   && (dataValid() == 1)
		   
		   );
  }	
  
  
  void Header::print( ostream & out ) {
	out << "\n\t Block Header "; 
	out << "\n Block Id " << blockId(); 
	out << "\n Word size " << wordSize() ; 
	out << "\n Word count " << wordCount() ; 
	out << "\n Product Id " << productId() ; 
	//	out << "\n\t" <<  ProductIDText[productID() ] ; 
	out << "\n Repeat flag " << repeatFlag() ; 
	out << "\n Version number " << versionNumber() ; 
	out << "\n Data Valid flag " << dataValid() ; 
	out << "\n ASCII / BIN data flag " << asciiBin() ; 
	out << "\n Range word " << rangeWord() ; 
	out << "\n Block count " << blockCount() ;
	out << "\n";
  }
}
