#include "lineDoc.h"
#include "SpaceCraftIDText.h"
#include "SourceChannelIDText.h"

LineDoc::LineDoc (GvarBlock* block) {
  GvarHeader* header = block->getHeader () ;
  int wordCount = header->wordCount () ;

  uint16* unpacked=new uint16[MAX_BLOCK_SZ];

  // The value 'wordCount' means the number of words (10 bits each)
  // plus 2 (for CRC). So the number of bytes should be 
  // ((wordCount-2)*10)/8.

  // if ((wordCount-2)*10)%8 is not 0, then we get the invalid data
  /*if (((wordCount-2)*10)%8 != 0) {
    printf ("The value in Word Count field does not match the data.") ;
    //return ;
    }*/

  unpack10 (block->getData (), ((wordCount-2)*10)/8, unpacked) ;

  Spcid = unpacked[0] ;
  Spsid = unpacked[1] ;
  Lside = unpacked[2] ;
  Lidet = unpacked[3] ;
  Licha = unpacked[4] ;
  RisctMsb  = unpacked[5] ;
  RisctLsb = unpacked[6] ;
  L1scan = unpacked[7] ;
  L2scan = unpacked[8] ;
  LpixlsMsb = unpacked[9] ;
  LpixlsLsb = unpacked[10] ;
  LwordsMsb = unpacked[11] ;
  LwordsLsb = unpacked[12] ;
  Lzcor  = unpacked[13] ;
  Llag   = unpacked[14] ;
  Lspar  = unpacked[15] ;

  int numOfPixels = ((int)LpixlsMsb << 10) + LpixlsLsb ;

  data = new uint16[numOfPixels] ;
  for (int i=16; i<16+numOfPixels; i++) {
    data[i-16] = unpacked[i] ;
  }

  delete[] unpacked ;
}

LineDoc::~LineDoc () {
  if (data != NULL) delete[] data ;
}

void LineDoc::print( ostream & out ){ 
  
  out << "\n\t Line Documentation Definition" ; 

  out << "\n spcid : Spacecraft ID               " << spcid() ; 
  //	out << "\n\t " << SpaceCraftIDText[spcid() ];  

  out << "\n spsid : Sensor processing system ID " << spsid() ; 

  out << "\n lside : Active detector config ID   " << lside() ; 

  out << "\n lidet : Detector ID                 " << lidet() ; 

  out << "\n licha : Source channel ID           " << licha() ;
  out << "\n\t" << SourceChannelIDText[licha()];

  out << "\n risct : Output scan count           " << risct() ; 

  out << "\n l1scan: Imager scan status (1)      " << l1scan() ; 

  out << "\n l2scan: Imager scan status (2)      " << l2scan() ; 

  out << "\n lpixls: Number of pixels in record  " << lpixls() ; 

  out << "\n lwords: Number of words in record   " << lwords() ; 

  out << "\n lzcor : Pixel offset employed at    ";
  out << "\n\t western edge of scan line   " << lzcor() ; 

  out << "\n llag  : Data lag ID                 " << llag() ; 

  out << "\n" ; 
  
}
