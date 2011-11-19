#include "Gvar.h"

namespace Gvar {
  Header* Block::createHeader (char* buff) {
	
    uchar8 temp[30] ;
    memcpy (temp, buff, 30) ;
//     for (int i=0; i<30; i++) {
//       temp[i] = (uchar8)buff[i] ;
//     }
    
    Header* header = new Header (temp) ;

    return header ;
  }
  
  
  Block::Block(char* buff, int buff_size)
  {

    m_rawDataLen = buff_size ;
    m_rawData = new char[m_rawDataLen] ; // (char*)malloc (m_rawDataLen) ;
    memcpy (m_rawData, buff, m_rawDataLen) ;

    header = createHeader (buff) ;
	
    // delete the first 3 30 bytes
    data = new uchar8[buff_size-90] ;
    for (int i=90; i<buff_size; i++) {
      data[i-90] = buff[i] ;
    }
  } 
  
  Block::~Block () {
    delete[] data ;
    delete header ;

    delete[] m_rawData ;
  }
  
  uchar8* Block::getData()
  { return data ;}
  
  Header* Block::getHeader()
  {return header ;}
}
