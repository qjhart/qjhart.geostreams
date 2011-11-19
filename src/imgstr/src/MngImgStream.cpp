/*
	GeoStreams Project
	MngImgStream - image stream in MNG format
	Carlos A. Rueda-Velasquez
	$Id: MngImgStream.cpp,v 1.7 2005/07/08 20:45:23 crueda Exp $
*/
#include "ImgStream.h"

#include <iostream>

// FIXME:  NO IMPLEMENTATION YET


MngImgStream::MngImgStream(StreamWriter* writer, ReferenceSpace& rs) 
: BaseImgStream(writer, rs) {
}

int MngImgStream::writeChannelDef(ChannelDef& ch) {
	// FIXME
	return 1;
}

int MngImgStream::startFrame(FrameDef& fr) {
	// FIXME
	return 1;
}

int MngImgStream::writeRow(Row& row) {
	// FIXME
	return -1;
}


