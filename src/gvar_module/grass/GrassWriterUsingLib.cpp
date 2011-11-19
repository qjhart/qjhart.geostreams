/*
 * @author Jie Zhang - 10/10/07 - preliminary version
 * @author Carlos Rueda
 * @version $Id: GrassWriterUsingLib.cpp,v 1.1 2007/10/11 07:50:11 crueda Exp $
 */

extern "C" {

#include <grass/gis.h>
#include <grass/glocale.h>

	// I'm including this  and the actual source make_maset.c as part of
	// the build process 'cause they're missing in libgrass_gis -- See README.txt (carlos rueda)
	int G_make_mapset( char *gisdbase_name, char *location_name, char *mapset_name );
	int G__make_mapset( char *gisdbase_name, char *location_name, char *mapset_name );
}

#include "GrassWriterUsingLib.h"

const int GrassWriterUsingLib::ew_res[GrassWriterUsingLib::m_numOfChannels] = {1,4,4,4,4};
const int GrassWriterUsingLib::ns_res[GrassWriterUsingLib::m_numOfChannels] = {1,4,8,4,4};

using namespace Gvar;


//
// Most of what follows as in Jie's preliminary code
//

GrassWriterUsingLib::GrassWriterUsingLib(char *gisbase_name,
					 char *location_name)
{
  m_gisbase_name = new char[strlen(gisbase_name) + 1];
  sprintf(m_gisbase_name, "%s", gisbase_name);
  m_location_name = new char[strlen(location_name) + 1];
  sprintf(m_location_name, "%s", location_name);

  m_prevFrameId = -1;
  m_block0 = NULL;
}

GrassWriterUsingLib::~GrassWriterUsingLib()
{
  for (int channelNo=0; channelNo<GrassWriterUsingLib::m_numOfChannels; channelNo++) {
    // close cell file and the necessary support files,
    // such as cellhd, cats, hist, etc.
    if ( ch_fd[channelNo] != 0 ) {
    	G_close_cell(ch_fd[channelNo]);
    }
  }

  delete[] m_gisbase_name;
  delete[] m_location_name;

  if (m_block0 != NULL) delete m_block0 ;
}

void GrassWriterUsingLib::write(Block *block) {
  Header* header = block->getHeader () ;

  if (header->blockId () == 0) {
    if (m_block0 != NULL) {
      delete m_block0 ;
    }

    m_block0 = new Block0 (block) ;
    Block0Doc* block0doc = m_block0->getBlock0Doc();
    uchar8* data = block->getData () ;
    int newFrameId = block0doc->frame () ;

    // Another way to check is the frame start bit in the ISCAN buffer.
    if (newFrameId != m_prevFrameId) {
      // new frame!!

      // Close the old frame
      for (int channelNo=0; channelNo<GrassWriterUsingLib::m_numOfChannels;
	   channelNo++) {
	// close cell file and the necessary support files,
	// such as cellhd, cats, hist, etc.
	G_close_cell(ch_fd[channelNo]);
      }

      m_prevFrameId = newFrameId;

      CdaTime* curTime = block0doc->getCurrentTime();

      if(curTime == NULL) {
        cout << "Current time is NULL" << endl;
        exit(1);
      }
      
      char imcIdentifier[5] ;
      block0doc->getImcIdentifier (imcIdentifier) ;
      imcIdentifier[4] = '\0' ;

      /* set mapset_name */
      char *mapset_name = curTime->getYMDEpoch(imcIdentifier) ;

      // Create a new mapset.
      cout<< "Making mapset '" << mapset_name << "'" <<endl;
      // use env vars for gisbase and location (ie, pass NULL for those):
      //if (G_make_mapset(m_gisbase_name, m_location_name, mapset_name) != 0)
      if (G__make_mapset(NULL, NULL, mapset_name) != 0) {
	      cout<< "Cannot create a new mapset: " << mapset_name << ".  Already existing?  Continuing..." << endl;
	      //G_fatal_error("Can not create a new mapset: %s", mapset_name);
	}

      // set the environment to this new mapset.
      G_setenv("MAPSET", mapset_name);

      delete[] mapset_name;

      // set the Region properly
      struct Cell_head window;

      // reset north, south, east, west values
      window.north =  block0doc->nsln() ;
      window.south = (block0doc->nsln() + 7) ;
      window.east = block0doc->epx() ;
      window.west = block0doc->wpx() ;

      /* This is only set for the duration of the call */
      G_set_window(&window);

      // Now open each channel
      for (int channelNo=0; channelNo<GrassWriterUsingLib::m_numOfChannels;
	   channelNo++) {
	window.ns_res = ns_res[channelNo];
	window.ew_res = ew_res[channelNo];

	G_set_window(&window);
	char channel_name[10];
	sprintf(channel_name, "ch%d", (channelNo+1));
	
	RASTER_MAP_TYPE data_type = CELL_TYPE;

	if ((ch_fd[channelNo] = G_open_raster_new(channel_name, data_type)) < 0)
	  G_fatal_error(_("Could not open <%s>"), channel_name);

	ch_row_buf[channelNo] = (CELL*) G_allocate_raster_buf(data_type);
      }
    }

    else  if (header->blockId () == 1 || 
	      header->blockId () == 2) {
      if (m_block0 == NULL) {
	delete block ;
	return ;
      }

      Block1or2* block1or2 = new Block1or2 (block) ;

      if(header->blockId () == 1) {
	//write data into ch4
	for(int i=0; i<2; i++) {
	  writeRow(block1or2->getLineDoc(i),
		   block1or2->getData(i),
		   block1or2->getDataLen(i),
		   4 - 1, // channel 4
		   1, 2);
	}
	
	//copy data into ch5
	for(int i = 2; i < 4; i++) {
	  writeRow(block1or2->getLineDoc(i),
		   block1or2->getData(i),
		   block1or2->getDataLen(i),
		   5 - 1, // channel 5
		   3, 4);
	}
      }

      else if(header->blockId () == 2) {
	//copy data into ch2
	for(int i = 0; i < 2; i++) {
	  writeRow(block1or2->getLineDoc(i),
		   block1or2->getData(i),
		   block1or2->getDataLen(i),
		   2 - 1,  // channel 2
		   5, 6);
	}

	//cpy data into ch3
	writeRow(block1or2->getLineDoc(3-1),
		 block1or2->getData(3-1),
		 block1or2->getDataLen(3-1),
		 3 - 1, // channel 3
		 7, 7);
      }
    
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
  
      writeRow(block3to10->getLineDoc(),
	       block3to10->getData(),
	       block3to10->getDataLen(),
	       1 - 1, // channel 1
	       1, 8);

      delete block3to10;
      delete block ;
    }//else if

    else { // other blocks
      delete block ;
    }
  }
}

/*
 * Write a row for channel 'channelNo'. 
 *
 * Param:
 *   channelNo: starting from 0.
 */
void GrassWriterUsingLib::writeRow(LineDoc* lineDoc,
				   uint16 *data,
				   int dataLen,
				   int channelNo,
				   int min_lidet,
				   int max_lidet)
{
  if (lineDoc->licha() == channelNo + 1) {
    if((lineDoc->lidet() >= min_lidet) && (lineDoc->lidet() <= max_lidet)) {
      memcpy(ch_row_buf[channelNo], data, dataLen);
      G_put_map_row(ch_fd[channelNo], (CELL*) ch_row_buf[channelNo]);
      delete (CELL*) ch_row_buf[channelNo];
      m_numOfRowsPerChannel[channelNo]++;
    }
  }
}
