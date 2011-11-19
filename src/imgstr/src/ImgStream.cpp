/*
	GeoStreams Project
	ImgStream - image stream creation
	Carlos A. Rueda-Velasquez
	$Id: ImgStream.cpp,v 1.3 2005/07/06 02:26:11 crueda Exp $
*/
#include "ImgStream.h"

#include <iostream>

ImgStream* ImgStream::createImgStream(
	StreamWriter* writer, Format format, ReferenceSpace& rs) {

	
	ImgStream* imgstr = 0;
	
	switch ( format ) {
		
		case ASCII_FORMAT:
			imgstr = new AsciiImgStream(writer, rs); 
			break;
			
		case BIN_FORMAT:
			imgstr = new BinImgStream(writer, rs); 
			break;

		case MNG_FORMAT:
			imgstr = new MngImgStream(writer, rs); 
			break;
			
		case APNG_FORMAT:
			// FIXME: NOT IMPLEMENTED 
			cerr<< "createImgStream: error: APNG_FORMAT not implemented yet\n"; 
			break;
			
		case GRASS_FORMAT:
			// FIXME: NOT IMPLEMENTED 
			cerr<< "createImgStream: error: GRASS_FORMAT not implemented yet\n"; 
			break;
			
		case UNKNOWN_FORMAT:
			cerr<< "createImgStream: error: unknown format specified\n"; 
			break;
	}
	
	return imgstr;
}


