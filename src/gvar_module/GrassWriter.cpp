#define LINUX

#ifdef OS2
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <errno.h>

#ifdef LINUX
#include <netinet/tcp.h>
#endif

/* the header for prototypes and errors */
#include "GrassWriter.h"
#include "Block.h"
#include "Block0Doc.h"
#include "LineDoc.h"
#include "cdaTime.h"
#include "image/imageatt.h"

#define DIRPERMS 0755
#define BLOCK0_SIZE 8040



GrassWriter::GrassWriter (char* path) {
  m_path = new char[strlen(path)+1] ;
  sprintf (m_path, "%s", path) ;

  m_prevFrameId = -1 ;
  m_block0 = NULL ;

  resetNumOfRowsAndColsPerChannel () ;
}

GrassWriter::~GrassWriter () {
  delete[] m_path ;

  for (int channelNo=0; channelNo<GrassWriter::m_numOfChannels; channelNo++) {
    if (m_headerOuts[channelNo].is_open ()) 
      m_headerOuts[channelNo].close () ;
    if (m_outs[channelNo].is_open ())
      m_outs[channelNo].close () ;
  }

  if (m_block0 != NULL) delete m_block0 ;
}

void GrassWriter::resetNumOfRowsAndColsPerChannel() {
  for (int channelNo=0; channelNo<GrassWriter::m_numOfChannels; channelNo++) {
    m_numOfRowsPerChannel[channelNo] = 0 ;
    m_numOfColsPerChannel[channelNo] = 0;
  }
}

void GrassWriter::writeHeaderFile (int channelNo) {

  string cellhdFile = cellhdDir + "/ch" + ((char)(channelNo+1+48)) ;
  m_headerOuts[channelNo].open (cellhdFile.c_str ()) ;
  m_headerOuts[channelNo] << "proj:       " << 0 << endl;
  m_headerOuts[channelNo] << "zone:       " << 0 << endl;
  m_headerOuts[channelNo] << "north:      -" << m_north << endl;
  m_headerOuts[channelNo] << "south:      -" << m_south << endl;
  m_headerOuts[channelNo] << "east:       " << m_east << endl;
  m_headerOuts[channelNo] << "west:       " << m_west << endl;
  m_headerOuts[channelNo] << "e-w resol:  " << ((float)(m_east - m_west + 1) / (float)m_numOfColsPerChannel[channelNo]) << endl;
  m_headerOuts[channelNo] << "n-s resol:  " << ((float)(m_south - m_north + 1) / (float)m_numOfRowsPerChannel[channelNo]) << endl;
  m_headerOuts[channelNo] << "rows:       " << m_numOfRowsPerChannel[channelNo] << endl;
  m_headerOuts[channelNo] << "cols:       " << m_numOfColsPerChannel[channelNo] << endl;
  m_headerOuts[channelNo] << "format:     " << 1 << endl;
  m_headerOuts[channelNo] << "compressed: " << 0 << endl;

  m_headerOuts[channelNo].close () ;

}//writeHeaderFile(int)

void GrassWriter::writeAllHeaderFiles () {
  for (int channelNo=0; channelNo<GrassWriter::m_numOfChannels; channelNo++) {
    if(m_outs[channelNo].is_open()) {
      m_outs[channelNo].close();
    }

    // write the header files for the previous frame
    if (m_prevFrameId != -1) writeHeaderFile (channelNo) ;
  }
}

void GrassWriter::write(Block* block) {
  Header* header = block->getHeader () ;

  if (header->blockId () == 0) {
    if (m_block0 != NULL) {
      delete m_block0 ;
    }

    m_block0 = new Block0 (block) ;
    Block0Doc* block0doc = m_block0->getBlock0Doc();
    uchar8* data = block->getData () ;
    int newFrameId = block0doc->frame () ;

    if (newFrameId != m_prevFrameId) {
    // new frame!!
 
      // flush & close files of old frame if open
      if(m_block0Out.is_open()) {
        m_block0Out.close();
      }

      writeAllHeaderFiles () ;

      m_prevFrameId = newFrameId;
  
      resetNumOfRowsAndColsPerChannel () ;
 
      // reset north, south, east, west values
      m_north =  block0doc->nsln() ; 
      //m_south = (block0doc->nsln() + 7) ; 
      m_east = block0doc->epx() ;
      m_west = block0doc->wpx() ;

      CdaTime* curTime = m_block0->getBlock0Doc()->getCurrentTime();

      if(curTime == NULL) {
        cout << "Current time is NULL" << endl;
        exit(1);
      }
      
      char imcIdentifier[5] ;
      m_block0->getBlock0Doc ()->getImcIdentifier (imcIdentifier) ;
      imcIdentifier[4] = '\0' ;

      // create the parent directory for current frames
      char* locationString =  curTime->getYMDEpoch(imcIdentifier) ;
      parentDir =  (string)m_path + "/" + (string)locationString ;
      delete[] locationString ;
      cout << parentDir << endl; 
      if(mkdir(parentDir.c_str (), DIRPERMS)) {
        if(errno != EEXIST) {
          cout << "Could not make directory " << parentDir.c_str () << endl;
          exit(1);
        }
      }

      char* timeString = curTime->getHM () ;
      topChildDir = parentDir + "/" + (string)timeString ;
      delete[] timeString ;
      cout << topChildDir << endl;
      if (mkdir (topChildDir.c_str (), DIRPERMS)) {
        if(errno != EEXIST) {
          cout << "Could not make directory " << topChildDir.c_str () << endl;
          exit(1);
        }
      }

      cellDir = topChildDir + "/cell" ;
      cout << cellDir << endl;
      if(mkdir(cellDir.c_str (), DIRPERMS)) {
        if(errno != EEXIST) {
          cout << "Could not make directory " << cellDir.c_str () << endl;
          exit(1);
        }
      }

      cellhdDir = topChildDir + "/cellhd" ;
      cout << cellhdDir << endl;
      if(mkdir(cellhdDir.c_str (), DIRPERMS)) {
        if(errno != EEXIST) {
	  cout << "Could not make directory " << cellhdDir.c_str () << endl;
	  exit(1);
        }
      }

      // open new files to write
      string block0File = topChildDir + "/block0file" ;
      m_block0Out.open (block0File.c_str (), 
			ios::out | ios::binary) ;

      //write Block0 info into block0file... metadata info
      m_block0Out.write (block->getRawData(), block->getRawDataLen ()) ;
      m_block0Out.close () ;

      for (int channelNo=0; channelNo<GrassWriter::m_numOfChannels; 
	   channelNo++) {
	string channelFile = cellDir + "/ch" + ((char)(channelNo+1+48)) ;
	m_outs[channelNo].open (channelFile.c_str ()) ;
      }
    }
    else { // same frame
      m_south = (block0doc->nsln() + 7) ; 
    } 
  }
  else if (header->blockId () == 1 || 
           header->blockId () == 2) {
    if (m_block0 == NULL) {
      delete block ;
      return ;
    }

    Block1or2* block1or2 = new Block1or2 (block) ;
    uint16* data;
    LineDoc* lineDoc;

    if(header->blockId () == 1) {
      //write data into ch4
      for(int i=0; i<2; i++) {
        data = block1or2->getData(i) ;
        lineDoc = block1or2->getLineDoc(i) ;

        if(lineDoc->licha() == 4) {
	  if ((lineDoc->lidet() == 1) || (lineDoc->lidet() == 2)) {
	    writeDataToChannel (lineDoc->licha()-1, data, 
				block1or2->getDataLen(i)) ;
	  }
        }//if
      }//for

      //cpy data into ch5
      for(int i = 2; i < 4; i++) {
        data = block1or2->getData(i);
        lineDoc = block1or2->getLineDoc(i);

        if(lineDoc->licha() == 5) {
          if((lineDoc->lidet() == 3) || (lineDoc->lidet() == 4)) {
	    writeDataToChannel (lineDoc->licha()-1, data, 
				block1or2->getDataLen(i)) ;
	  }
        }//if
      }//for
    }//else if
    else if(header->blockId () == 2) {
      //cpy data into ch2
      for(int i = 0; i < 2; i++) {
        data = block1or2->getData(i);
        lineDoc = block1or2->getLineDoc(i) ;

        if(lineDoc->licha() == 2) {
          if((lineDoc->lidet() == 5) || (lineDoc->lidet() == 6)) {
	    writeDataToChannel (lineDoc->licha()-1, data, 
				block1or2->getDataLen(i)) ;
	  }
        }
      }//for

      //cpy data into ch3
      data = block1or2->getData(2);
      lineDoc = block1or2->getLineDoc(2) ;

      if(lineDoc->licha() == 3) {
        if(lineDoc->lidet() == 7) { 
	  writeDataToChannel (lineDoc->licha()-1, data, 
			      block1or2->getDataLen (lineDoc->licha()-1)) ;
	}
      }//if
    }//else if
    
    delete block1or2;
    delete block ;
  }//else if
  else if (header->blockId () >= 3 && 
           header->blockId () <= 10) {
    if (m_block0 == NULL) {
      delete block ;
      return ;
    }

    Block3to10* block3to10 = new Block3to10 (block) ;
    uint16* data;
    LineDoc* lineDoc;
  
    data = block3to10->getData();
    lineDoc = block3to10->getLineDoc() ;

    if(lineDoc->licha() == 1) {
      if((lineDoc->lidet() >= 1) && (lineDoc->lidet() <= 8)) {
	writeDataToChannel (lineDoc->licha()-1, data, 
			    block3to10->getDataLen()) ;
      }
    }//if
    delete block3to10;
    delete block ;
  }//else if

  else { // other blocks
    delete block ;
  }

}

void GrassWriter::writeDataToChannel
(int channelNo, uint16* data, int dataLen) {
  m_numOfRowsPerChannel[channelNo] ++ ;

  if(m_numOfColsPerChannel[channelNo] == 0)
    m_numOfColsPerChannel[channelNo] = dataLen ;

  for(int j=0; j<dataLen; j++) {
    m_outs[channelNo] << ((uchar8) (data[j] >> 8)) ;
    m_outs[channelNo] << ((uchar8) data[j]) ;    
  }//for

  m_outs[channelNo].flush () ;
}
