#include "GvarConverter.h"

using namespace Geostream ;

GvarConverter::GvarConverter (GvarStream* gvar, 
			      Fifo<Row>* rowFifo,
			      ClientRequest& request,
			      ReferenceSpace* rs, 
			      StreamWriter *writer) :
  m_gvar (gvar), m_rowFifo (rowFifo), m_clientRequest (request), m_rs (rs) {

  m_shmRowFifo = NULL ;
  m_shmRowReader = NULL ;

  m_imgStream = ImgStream::createImgStream (writer, m_clientRequest.format, *rs) ;
}

GvarConverter::GvarConverter (RowFifo* shmRowFifo,
			      ClientRequest& request,
			      ReferenceSpace* rs, 
			      StreamWriter *writer) :
  m_gvar (NULL), m_shmRowFifo (shmRowFifo), m_clientRequest (request), m_rs (rs) {

  m_rowFifo = NULL ;
  m_rowReader = NULL ;

  m_imgStream = ImgStream::createImgStream (writer, m_clientRequest.format, *rs) ;
}

GvarConverter::~GvarConverter () {
  fprintf (stderr, "deleting GvarConverter...\n"); fflush (stderr) ;
  delete m_imgStream ;
  if (m_rowReader != NULL) delete m_rowReader ;
  if (m_shmRowReader != NULL) delete m_shmRowReader ;
}

void GvarConverter::convert () {

  for (int channelNo=0; channelNo<NUM_OF_CHANNELS; channelNo++) {
    // send all channels definition
    if (m_clientRequest.isChannelRequested[channelNo]) {
      ChannelDef channel_def
        (Geostream::channelsDesc[channelNo], channelNo,
         Geostream::xRes[channelNo], Geostream::yRes[channelNo], 16./18., 10);
      m_imgStream->writeChannelDef(channel_def) ;
    }
  }

  // create readers
  if (m_rowFifo != NULL) {
    m_rowReader = m_rowFifo->newReader () ;
  } else if (m_shmRowFifo != NULL) {
    m_shmRowReader = Geostream::newReader (m_shmRowFifo) ;
  }

  int prevFrameId = -1 ;
  Row *row ;
  RowMax *rowMax ;

  // write rows
  while ((m_rowReader != NULL && (row = m_rowReader->read ())) ||
	 (m_shmRowReader != NULL && (rowMax = m_shmRowReader->read ()))) {
    int currFrameId = -1 ;
    if (m_rowReader != NULL) {
      currFrameId = row->f ;
    } else if (m_shmRowReader != NULL) {
      currFrameId = rowMax->f ;
    }

    // new frame
    if (currFrameId != prevFrameId) {
      // send the previous frameDef
      if (m_rowReader != NULL) {
	FrameDef* frameDef = m_gvar->getPrevFrameDef () ;
	fprintf (stderr, "Sending a new frameDef.\n"); fflush (stderr) ;
	m_imgStream->startFrame (*frameDef) ;
      } else if (m_shmRowReader != NULL) {
	FrameDef* frameDef = Geostream::getFrameDef (m_shmRowFifo) ;
	fprintf (stderr, "Sending a new frameDef.\n"); fflush (stderr) ;
	m_imgStream->startFrame (*frameDef) ;
	delete frameDef ;
      }

      prevFrameId = currFrameId ;
    }

    int channelNo = -1 ;

    if (m_rowReader != NULL) {
      channelNo = row->c ;
    } else if (m_shmRowReader != NULL) {
      channelNo = rowMax->c ;
    }

    if (channelNo == -1) {
      fprintf (stderr, "unexpected error - channelNo=-1\n") ; 
      fflush (stderr) ;
    }

    if (channelNo != -1 && m_clientRequest.isChannelRequested[channelNo]) {

      int returnCode = -1 ;
      if (m_rowReader != NULL) {

	int finalX = row->x ;
	int finalY = row->y ;
	int finalWidth = row->w ;

	if (m_clientRequest.bbox.width != 0 && 
	    m_clientRequest.bbox.height != 0) {
	  if (finalY >= m_clientRequest.bbox.y &&
	      finalY <= (m_clientRequest.bbox.y + 
			 m_clientRequest.bbox.height - 1)) {

	    finalX = (m_clientRequest.bbox.x < row->x) ? 
	      row->x : (row->x + (m_clientRequest.bbox.x - row->x + 1)/
			Geostream::xRes[channelNo]) ;
	    finalWidth = 
	      ((row->x + row->w*Geostream::xRes[channelNo]) >
	       (m_clientRequest.bbox.x + m_clientRequest.bbox.width)) ?
	      ((m_clientRequest.bbox.x + m_clientRequest.bbox.width + 1 
		- finalX)/Geostream::xRes[channelNo]) :
	      (row->x + row->w + 1 - finalX) ;
	  } else {
	    finalWidth = 0 ;
	  }
	}

	// if this row doesn't satisfy the request
	if (finalWidth <= 0) {
	  finalWidth = -1 ;
	}

	// if this is a "nodata" request, then set finalWidth to 0.
	if (m_clientRequest.nodata && finalWidth > 0) {
	  finalWidth = 0 ;
	}

	if (finalWidth >= 0) {
	  Row finalRow (row->f, row->c, finalX, finalY, finalWidth, 
			row->v+(finalX-row->x)/Geostream::xRes[channelNo], 
			false) ;
	  returnCode = m_imgStream->writeRow (finalRow) ;

	  fprintf 
	    (stderr, 
	     "send a row: frameId=%d, channelNo=%d, x=%d, y=%d, size=%d\n",
	     row->f, row->c, finalX, finalY, finalWidth) ; 
	  fflush (stderr) ;
	} else {
	  returnCode = 0 ;
	}
      } else if (m_shmRowReader != NULL) {
	// add this to make sure if the client is too slow, I don't send
	// the wrong data
	if (m_clientRequest.isChannelRequested[rowMax->c]) {
	  int finalX = rowMax->x ;
	  int finalY = rowMax->y ;
	  int finalWidth = rowMax->w ;

	  if (m_clientRequest.bbox.width != 0 && 
	      m_clientRequest.bbox.height != 0) {

	    if (finalY >= m_clientRequest.bbox.y &&
		finalY <= (m_clientRequest.bbox.y + 
			   m_clientRequest.bbox.height - 1)) {

	      finalX = (m_clientRequest.bbox.x < rowMax->x) ? 
		rowMax->x : m_clientRequest.bbox.x ;
	      finalWidth = 
		((rowMax->x + rowMax->w*Geostream::xRes[channelNo]) >
		 (m_clientRequest.bbox.x + m_clientRequest.bbox.width)) ?
		((m_clientRequest.bbox.x + m_clientRequest.bbox.width + 1 
		  - finalX)/Geostream::xRes[channelNo]) :
		(rowMax->x + rowMax->w + 1 - finalX) ;

	    } else {
	      finalWidth = 0 ;
	    }
	  }
	  
	  // if this row doesn't satisfy the request
	  if (finalWidth <= 0) {
	    finalWidth = -1 ;
	  }

	  // if this is a "nodata" request, then set finalWidth to 0.
	  if (m_clientRequest.nodata && finalWidth > 0) {
	    finalWidth = 0 ;
	  }

	  if (finalWidth >= 0) {
	    Row row (rowMax->f, rowMax->c, finalX, finalY, 
		     finalWidth, 
		     rowMax->v+(finalX-rowMax->x)/Geostream::xRes[channelNo],
		     false) ;

	    returnCode = m_imgStream->writeRow (row) ;
	    fprintf 
	      (stderr, 
	       "send a row: frameId=%d, channelNo=%d, x=%d, y=%d, size=%d\n",
	       rowMax->f, rowMax->c, finalX, finalY, finalWidth) ; 
	    fflush (stderr) ;
	  } else {
	    // ignore this row
	    returnCode = 0 ;
	  }
	} else {
	  // ignore this row
	  returnCode = 0 ;
	}
      }

      if (returnCode != 0) break ;
    }
  }
}
