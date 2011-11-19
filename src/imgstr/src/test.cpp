/*
	GeoStreams Project
	A simple test to exercise the ImgStream module
	Carlos A. Rueda-Velasquez
	$Id: test.cpp,v 1.3 2005/07/09 07:17:27 crueda Exp $

	This is a compilable and executable code following the schematic
	usage described in README file.
	
	To build and run this demo:
		make test
		./test
	You will get a number of lines containing the generated stream.
*/
#include "ImgStream.h"


/** 
  * We need to provide a StreamWriter, so here it is:
  * This is an implementation of StreamWriter that writes to a regular
  * file or to stdout.
  */
class FileStreamWriter : public StreamWriter {
	string filename;
	FILE* file;
	
public:

	/** @param filename The output filename. If equals to "-" then
	  * it will write to stdout
	  */
	FileStreamWriter(string filename) : filename(filename) {
		if ( filename == "-" )
			file = fdopen(fileno(stdout), "wb");
		else
			file = fopen(filename.c_str(), "wb");
	}
	
	~FileStreamWriter() {
		if ( file && filename != "-" ) 
			fclose(file);
		file = 0;
	};

	/** calls fwrite(buf, nbytes, 1, file) and returns 0
	  * if that call was successful. */	
	int writeBuffer(const void* buf, int nbytes) {
		if ( 1 == fwrite(buf, nbytes, 1, file) )
			return 0;   // OK
		else
			return -1;  //   error ocurred
	}
};



/** The stream will be written to stdout unless the name of a file
  * is given as the only argument to this program. The special
  * name "-" will also refer to the stdout.
  */
int main(int argc, char** argv) {
	string filename = "-";
	
	if ( argc > 1 )
		filename = argv[1];
	
	FileStreamWriter writer(filename);
	Format format = ASCII_FORMAT;
	ReferenceSpace rs(5000, 5000, 25000, 17000);
	ImgStream* imgStream = ImgStream::createImgStream(&writer, format, rs);

	// some channel definitions:	
	ChannelDef channel_def0("VISIBLE", 0, 1, 1, 16./18., 10);
	imgStream->writeChannelDef(channel_def0);
	
	ChannelDef channel_def1("INFRARED", 1, 2, 2, 16./18., 10);
	imgStream->writeChannelDef(channel_def1);
	
	
	// here, a loop
	bool keep_running = true;
	while ( keep_running ) {
		// when a frame starts:
		Rectangle rect(15000, 5000, 10, 5);  // rectangle associated to frame
		FrameDef frame_def(0, rect);
		imgStream->startFrame(frame_def);
		
		
		// to send rows
		int current_frame_id = 0;                  // lets say 0
		int channelNo = 1;                         // lets say 1
		int w = 10;                                // number of columns in line
		uint16 line[10] = { 1,2,3,4,5,6,7,8,9,0};  // as an example
		int x = 15000;
		int y = 5000;
		for ( int i = 0; i < 5; i++ ) {
			Row row(current_frame_id, channelNo, x, y + i, w, line) ;
			imgStream->writeRow(row) ;
		}
		
		
		keep_running = false;   // just to finish this test :-)
	}
	
	delete imgStream;
	
	return 0;
}



