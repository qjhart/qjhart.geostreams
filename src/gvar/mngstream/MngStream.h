/*
	GeoStreams Project
	MngStream - Module to create a MNG stream of images
	$Id: MngStream.h,v 1.4 2004/05/27 18:37:55 crueda Exp $
*/
#ifndef MngStream_h
#define MngStream_h

#ifndef _STDIO_H
#include <stdio.h>    // FILE
#endif


typedef unsigned short uint16;

/* 
  Represents a MNG stream.
  This is a first version to test the basic required functionality.
  Currently the stream is to be written in a file 
  (maybe next I will provide a way to write to a socket).
  
  See test programs pgms2mng.cc and pngs2mng.cc for examples
  of usage.
*/
class MngStream {
public:
	// Creates a new mng stream on a file.
	// mngfilename --obvious
	// fwidth, fheight are the dimensions of the frame associated
	// to the stream
	MngStream(char* mngfilename, int fwidth, int fheight);
	
	// Creates a mng stream on a given standard stream.
	// This is useful for writing to stdout and thus to make a CGI, 
	// for example, mng.php call ``pgms2mng - ...'' to write
	// the stream to stdout, `-'
	// fwidth, fheight are the dimensions of the frame associated
	// to the stream
	MngStream(FILE* mngfile, int fwidth, int fheight);
	
	
	// destroys this stream.  If the stream is still open,
	// then it's first closed
	~MngStream();
	
	// adds an image to this stream.
	// x,y is the location of this image relative to the frame.
	// iwidth, iheight are the dimensions of the image.
	// return 0 iff OK.
	// If iwidth <= 0 || iheight <= 0, no image is added and -1 is returned.
	int addImage(int x, int y, uint16* rows[], int iwidth, int iheight);
	
	// adds a PNG file
	// x,y is the location of this image relative to the frame.
	// return 0 iff OK.
	int addPng(int x, int y, char* filename);
	
	// closes this mng stream. 
	// After this, this stream becomes no longer valid.
	void close();

private:
	void* prv_data;
};

#endif
