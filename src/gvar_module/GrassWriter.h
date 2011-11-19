#ifndef GRASSWRITER_H
#define GRASSWRITER_H

#include "Block.h"
#include <fstream>  // instead of <fstream.h>   (carueda 9/27/07)
#include <string>

using namespace Gvar;

class GrassWriter {
private:
   static const int m_numOfChannels = 5;

   int m_prevFrameId;
   char* m_path ;
   Gvar::Block0 *m_block0 ;
   ofstream m_block0Out;
   ofstream m_outs[GrassWriter::m_numOfChannels] ;
   ofstream m_headerOuts[GrassWriter::m_numOfChannels] ;

   int m_numOfRowsPerChannel[GrassWriter::m_numOfChannels] ;
   int m_numOfColsPerChannel[GrassWriter::m_numOfChannels] ;

   double m_north ;
   double m_south ;
   double m_east ;
   double m_west ;

   string parentDir;
   string topChildDir;
   string cellDir;
   string cellhdDir;

   void writeDataToChannel (int channelNo, uint16* data, int dataLen) ;

public:
   GrassWriter (char* path) ;
   ~GrassWriter () ;
   void write(Block* block);
   void writeHeaderFile(int channelNo);
   inline int getNumOfRowsPerChannel(int channelNo) {
     return m_numOfRowsPerChannel[channelNo] ;
   }
   inline int getNumOfColsPerChannel(int channelNo) {
     return m_numOfColsPerChannel[channelNo] ;
   }
   void resetNumOfRowsAndColsPerChannel() ;

   void writeAllHeaderFiles () ;
};

#endif
