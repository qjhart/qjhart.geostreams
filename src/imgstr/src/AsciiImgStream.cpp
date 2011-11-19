/*
	GeoStreams Project
	AsciiImgStream - image stream in an ad hoc ASCII format
	Carlos A. Rueda-Velasquez
	$Id: AsciiImgStream.cpp,v 1.10 2005/07/09 07:17:27 crueda Exp $
*/
#include "ImgStream.h"

#include <iostream>
#include <sstream>

/**
  * Creates an ASCII image stream.
  * Writes the reference space info in a line as follows:
  *	   "RS x y w h\n"
  */
AsciiImgStream::AsciiImgStream(StreamWriter* writer, ReferenceSpace& rs) 
: BaseImgStream(writer, rs) {
	Rectangle& rect = rs.rect;
	ostringstream ostr;
	ostr<< "RS"
		<< " " <<rect.x
		<< " " <<rect.y 
		<< " " <<rect.w 
		<< " " <<rect.h
		<< "\n"
	;
	string str = ostr.str();
	writer->writeBuffer(str.c_str(), str.length());
}


/**
  * Writes a frame definition in a line as follows:
  *	   "FR id x y w h\n"
  */
int AsciiImgStream::startFrame(FrameDef& fr) {
	Rectangle& rect = fr.getRectangle();
	ostringstream ostr;
	ostr<< "FR"
		<< " " <<fr.getID()
		<< " " <<rect.x
		<< " " <<rect.y 
		<< " " <<rect.w 
		<< " " <<rect.h
		<< "\n"
	;
	string str = ostr.str();
	return writer->writeBuffer(str.c_str(), str.length());
}

/**
  * Writes a channel definition in a line as follows:
  *	   "CH id psx psy aspect sampleSize name\n"
  */
int AsciiImgStream::writeChannelDef(ChannelDef& ch) {
	ostringstream ostr;
	ostr<< "CH"
		<< " " <<ch.getID()
		<< " " <<ch.getPixelSizeX()
		<< " " <<ch.getPixelSizeY()
		<< " " <<ch.getAspect()
		<< " " <<ch.getSampleSize()
		<< " " <<ch.getName()
		<< "\n"
	;
	string str = ostr.str();
	return writer->writeBuffer(str.c_str(), str.length());
}


/**
  * Writes one line as follows:
  *    "RW f c x y w v v ...\n"
  * where v v ...  are the w pixel values in the row
  */
int AsciiImgStream::writeRow(Row& row) {
	ostringstream ostr;
	ostr<< "RW " <<row.f<< " " <<row.c<< " " <<row.x<< " " <<row.y<< " " <<row.w;
	for ( int j = 0; j < row.w; j++ ) {
		ostr<< " " <<row.v[j];
	}
	ostr<< "\n";
	string str = ostr.str();
	return writer->writeBuffer(str.c_str(), str.length());
}

