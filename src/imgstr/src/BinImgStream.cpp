/*
	GeoStreams Project
	BinImgStream - image stream in an ad hoc binary format
	Carlos A. Rueda-Velasquez
	$Id: BinImgStream.cpp,v 1.11 2005/07/27 22:18:25 crueda Exp $
*/
#include "ImgStream.h"

#include <iostream>


inline bool host_is_little_endian() {
	const long l = 1; 
	const void *ptr = &l;
	char b = *(char*) ptr;
	return b == 1;	
}


// this function reverses the bytes of a datum
static void reverse_bytes(void* datum, int len) {
	char* bin = (char*) datum;
	char bout[len]; 
	for ( int i = 0; i < len; i++ ) {
		bout[i] = bin[len - i - 1];
	}
	memcpy(datum, bout, len);
}


// reverse the shorts in an array
static void reverse_shortarray(short* array, int len) {
	for ( int i = 0; i < len; i++ ) {
		reverse_bytes(array + i, sizeof(short));
	}
}

// reverse the shorts in an array
static void reverse_floatarray(float* array, int len) {
	for ( int i = 0; i < len; i++ ) {
		reverse_bytes(array + i, sizeof(float));
	}
}


/**
  * Creates a binary image stream.
  * Writes the reference space info as follows:
  *	   2-chars:  "RS"
  *	   short:  x
  *	   short:  y
  *	   short:  w
  *	   short:  h
  */
BinImgStream::BinImgStream(StreamWriter* writer, ReferenceSpace& rs) 
: BaseImgStream(writer, rs) {
	Rectangle& rect = rs.rect;          
	const int buf_size = 2*sizeof(char) + 4*sizeof(short);
	char buffer[buf_size];
	strcpy((char*) buffer, "RS");
	short* shorts = (short*) (&buffer[2]);
	shorts[0] = (short) rect.x;
	shorts[1] = (short) rect.y;
	shorts[2] = (short) rect.w;
	shorts[3] = (short) rect.h;
	
	if ( host_is_little_endian() ) {
		reverse_shortarray(shorts, 4);
	}
	
	writer->writeBuffer(buffer, buf_size);
}

/**
  * Writes a channel definition as follows:
  *	   2-chars:  "CH"
  *	   short:  id
  *	   short:  pixel size in x
  *	   short:  pixel size in y
  *	   float: aspect  
  *	   uchar: sampleSize
  *	   char*: name (nul-terminated string)  
  */
int BinImgStream::writeChannelDef(ChannelDef& ch) {
	int buf_size = 2*sizeof(char) + 3*sizeof(short) + sizeof(float) 
	             + sizeof(unsigned char)
	             + ch.getName().length() + 1
	;
	char* buffer = new char[buf_size];
	if ( !buffer ) {
		cerr<< "NO ENOUGH MEMORY FOR CHANNEL-DEF BUFFER!"
		    << " ch.getName().length() = " <<ch.getName().length()<< endl
		;
		return -1;
	}

	strcpy(buffer, "CH");
	short* shorts = (short*) (&buffer[2]);
	shorts[0] = (short) ch.getID();
	shorts[1] = (short) ch.getPixelSizeX();
	shorts[2] = (short) ch.getPixelSizeY();
	float* floats = (float*) (&shorts[3]);
	floats[0] = (float) ch.getAspect();
	unsigned char* sampleSize = (unsigned char*) (&floats[1]);
	sampleSize[0] = (unsigned char) ch.getSampleSize();
	char* name = (char*) (&sampleSize[1]);
	strcpy(name, ch.getName().c_str());

	if ( host_is_little_endian() ) {
		reverse_shortarray(shorts, 3);
		reverse_floatarray(floats, 1);
	}
	
	int ret = writer->writeBuffer(buffer, buf_size);
	delete[] buffer;
	return ret;
}


/**
  * Writes a frame definition as follows:
  *	   2-chars:  "FR"
  *	   short:  id
  *	   short:  x
  *	   short:  y
  *	   short:  w
  *	   short:  h
  */
int BinImgStream::startFrame(FrameDef& fr) {
	Rectangle& rect = fr.getRectangle();
	const int buf_size = 2*sizeof(char) + 5*sizeof(short);
	char buffer[buf_size];
	strcpy((char*) buffer, "FR");
	short* shorts = (short*) (&buffer[2]);
	shorts[0] = (short) fr.getID();
	shorts[1] = (short) rect.x;
	shorts[2] = (short) rect.y;
	shorts[3] = (short) rect.w;
	shorts[4] = (short) rect.h;
	
	if ( host_is_little_endian() ) {
		reverse_shortarray(shorts, 5);
	}
	
	return writer->writeBuffer(buffer, buf_size);
}



/**
  * Writes a row as follows:
  *	   2-chars:  "RW"
  *	   short:  frame id
  *	   short:  channel id
  *	   short:  x location
  *	   short:  y location
  *	   short:  width
  *	   short*:  data (width pixel values)
  */
int BinImgStream::writeRow(Row& row) {
	int buf_size = 2*sizeof(char) + 5*sizeof(short)
	         + row.w * sizeof(short)
	;
	char* buffer = new char[buf_size];
	if ( !buffer ) {
		cerr<< "NO ENOUGH MEMORY FOR ROW BUFFER!  row.w = " <<row.w<< endl;
		return -1;
	}
	strcpy((char*) buffer, "RW");
	short* shorts = (short*) (&buffer[2]);
	shorts[0] = (short) row.f;
	shorts[1] = (short) row.c;
	shorts[2] = (short) row.x;
	shorts[3] = (short) row.y;
	shorts[4] = (short) row.w;
	for ( int j = 0; j < row.w; j++ ) {
		shorts[5 + j] = (short) row.v[j];
	}	
	
	if ( host_is_little_endian() ) {
		reverse_shortarray(shorts, 5 + row.w);
	}
	
	int ret = writer->writeBuffer(buffer, buf_size);
	delete[] buffer;
	return ret;
}

