/*
 * @author Jie Zhang - preliminary version
 * @author Carlos Rueda - updates
 * @version $Id: GrassWriterUsingLib.h,v 1.1 2007/10/11 07:50:11 crueda Exp $
 */

#ifndef GRASSWRITERUSINGLIB_H
#define GRASSWRITERUSINGLIB_H

#include "Block.h"
#include <string>


/*
 * A Grass writer using GRASS library.
 */
class GrassWriterUsingLib {
 private:
   static const int m_numOfChannels = 5;

   int m_prevFrameId;
   char* m_gisbase_name ;
   char* m_location_name;
   Gvar::Block0 *m_block0 ;

   // the file descriptor for each channel
   int ch_fd[GrassWriterUsingLib::m_numOfChannels];

   // the data buffer for each channel
   void *ch_row_buf[GrassWriterUsingLib::m_numOfChannels];

   // the number of rows for each channel.
   int m_numOfRowsPerChannel[GrassWriterUsingLib::m_numOfChannels] ;

   // the ew and ns resolution for each channel
   static const int ew_res[GrassWriterUsingLib::m_numOfChannels];
   static const int ns_res[GrassWriterUsingLib::m_numOfChannels];

   void writeRow(Gvar::Block *block, int channelNo, int min_lidet, int max_lidet);

   void writeRow(Gvar::LineDoc* lineDoc,
				   uint16 *data,
				   int dataLen,
				   int channelNo,
				   int min_lidet,
				   int max_lidet);
 public:
   GrassWriterUsingLib (char* gisbase_name, char *location_name) ;
   ~GrassWriterUsingLib () ;
   inline int getNumOfRowsPerChannel(int channelNo) {
     return m_numOfRowsPerChannel[channelNo] ;
   }
   void write(Gvar::Block *block);
};


#endif
