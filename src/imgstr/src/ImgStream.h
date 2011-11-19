/*
	GeoStreams Project
	ImgStream - image stream creation
	Carlos A. Rueda-Velasquez
	$Id: ImgStream.h,v 1.18 2005/08/09 09:47:33 crueda Exp $

   For a schematic usage of this module, see the README file.
   For a simple example with actual executable code, see test.cpp
*/
#ifndef ImgStream_h
#define ImgStream_h

#ifndef _STDIO_H
#include <stdio.h>    // FILE
#endif

#include <string>

using namespace std;


typedef unsigned short uint16;


/** An enumeration with codes for possible output stream formats. */
enum Format { 
	UNKNOWN_FORMAT = 0, 
	BIN_FORMAT     = 1, 
	ASCII_FORMAT   = 2, 
	MNG_FORMAT     = 3, 
	APNG_FORMAT    = 4, 
	GRASS_FORMAT   = 5 
};



/**
  * A Rectangle specifies an area in a coordinate space that is
  * enclosed by the Rectangle object's top-left point (x, y) in the
  * coordinate space, its width, and its height.
  * Rectangles are used to specify reference spaces and frame extensions.  
  */
class Rectangle {
public:
	int x;
	int y;
	int w;
	int h;
	
	Rectangle(int x, int y, int w, int h) : x(x), y(y), w(w), h(h)
	{}
};


/**
  * Defines the space (currently a rectangle) in which all frames
  * associated to a given stream are expected to be contained.
  * 
  * This concept is more useful for clients interested in doing some
  * kind of visualization, like the applet viewer, since they will use
  * this to allocate the area in which incoming images (rows) will be
  * displayed.
  */
class ReferenceSpace {
public:
	Rectangle rect;
	
	ReferenceSpace(int x, int y, int w, int h) : rect(x, y, w, h)
	{}
};


/**
  * Describes the geometry of a particular channel in a stream. This
  * geometry is given by the size of the pixel in this channel. This
  * size is measured in terms of the pixel resolution of the reference
  * space associated to the stream.
  * Also the sample size (number of bits per sample) is associated to 
  * a channel.
  * Multiple channels can be associated to a stream.
  */
class ChannelDef {
public:
	/** Short name of this channel. */
	string getName() { return name; }
	
	/** ID of this channel. */
	int getID() { return ID; }
	
	/** Size of the pixel in the channel in the x direction
	  * in terms of number of pixels in the reference space. */
	int getPixelSizeX() { return pixelSizeX; }
	
	/** Size of the pixel in the channel in the y direction
	  * in terms of number of pixels in the reference space. */
	int getPixelSizeY() { return pixelSizeY; }

	/** The aspect ratio for the channel. */
	double getAspect() { return aspect; }

	/** Gets the sample size in bits. */
	int getSampleSize() { return sampleSize; };
	
	/** Creates the geometry specification of a channel.
	  */
	ChannelDef(string name, int ID, int pixelSizeX, int pixelSizeY, double aspect, int sampleSize) : 
	name(name), ID(ID), pixelSizeX(pixelSizeX), pixelSizeY(pixelSizeY), aspect(aspect), sampleSize(sampleSize)
	{}

private:
	/** A short name */
	string name;
	
	/** ID of this channel definition */
	int ID;
	
	/** Size of the pixel in the x direction
	  * in terms of number of pixels in the reference space. */
	int pixelSizeX;
	
	/** Size of the pixel in the y direction
	  * in terms of number of pixels in the reference space. */
	int pixelSizeY;

	/** The aspect ratio  */
	double aspect;

	/** Number of bits per sample. */
	int sampleSize;
};


/**
  * Describes the geometry (currently a rectangle) of a particular 
  * frame in a stream. 
  * An ID is used to identify the frame.
  */
class FrameDef {
public:
	/** ID of this frame definition. */
	int getID() { return ID; }
	
	/** Gets the rectangle associated to this frame. */
	Rectangle& getRectangle() { return rect; }
	

	/** Creates the geometry specification of a frame.
	  */
	FrameDef(int ID, Rectangle& rect) : 
	ID(ID), rect(rect)
	{}

private:
	/** ID of this frame definition */
	int ID;
	
	/** rectangle associated to this frame. */
	Rectangle rect;
};



/**
  * A row of pixel values belonging to a particular frame and channel. 
  */
class Row {
public:
	/** Frame ID */
	int f;

	/** Channel ID */
	int c;

	/** x location */
	int x;

	/** y location */
	int y;

	/** number of pixel values */
	int w;

	/** pixel values */
	uint16* v;

	/** Creates a row.
	  * @param f Frame ID
	  * @param c Channel ID
	  * @param x,y is the location of this row relative to the reference space
	  *        and channel geometry.
	  * @param w Number of pixels in the row
	  * @param vv Pixel values
	  * @param copyData if true, data is copied to an internally allocated memory;
	  *        otherwise, a direct pointer assignment (v = vv) is done. 
	  */
	Row(int f, int c, int x, int y, int w, uint16* vv, bool copyData = false) : 
	f(f), c(c), x(x), y(y), w(w), copyData(copyData) {
		if ( copyData ) {
			v = new uint16[w];
			for ( int i = 0; i < w; i++ ) {
				v[i] = vv[i];
			}
		}
		else {
			v = vv;
		}
	}
	
	~Row() {
		if ( copyData && v )
			delete[] v;
	}
private:
	bool copyData;

};


/**
  * The interface of objects that writes data to an image stream.
  * You will need to subclass this interface and give an instance of 
  * your subclass when creating an image stream.
  */
class StreamWriter {
public:
	
	virtual ~StreamWriter() {};
	
	/** This method is called by an ImgStream object to write
	  * data to the stream.
	  *
	  * @return 0 iff OK; otherwise the write is considered to have
	  *         failed so the process of writing data to the
	  *         stream will be cancelled.
	  */
	virtual int writeBuffer(const void* buf, int nbytes) { return 0; }
};


/**
  * An interface describing an output image stream. A factory method is
  * used to create instances of concrete implementations of this
  * interface according to a particular format and a reference space.
  *
  * For a schematic usage of this module, see the README file.
  */
class ImgStream {
public:
	
	virtual ~ImgStream() {};
	
	/** Gets the writer associated to this stream. */
	virtual StreamWriter* getWriter() { return 0; }
	
	/** Gets the format of this stream. */
	virtual Format getFormat() { return UNKNOWN_FORMAT; }
	
	/** Gets the reference space of this stream. 
	  * Location of all images written out are relative to this space. 
	  */
	virtual ReferenceSpace* getReferenceSpace() { return 0; }
	
	/** Writes a channel definition to this stream. 
	  * You will use channel_def.ID to refer to the given channel in
	  * other methods.
	  * Call this method as many times as neccessary before doing the
	  * the first call to startFrame().
	  *
	  * @return 0 iff OK
	  */
	virtual int writeChannelDef(ChannelDef& channel_def) { return 1; }
	
	/** Starts a frame.
	  * You will use frame_def.ID to refer to the given frame in
	  * other methods.
	  *
	  * @return 0 iff OK
	  */
	virtual int startFrame(FrameDef& frame_def) { return 1; }
	
	
	/**
	  * Writes a row to the given frame and channel.
	  *
	  * @param row The row to be written out.
	  * @return 0 iff OK. If row.w <= 0, nothing is written and -1 is returned.
	  */
	virtual int writeRow(Row& row) { return 1; }
	
	/**
	  * closes this stream. 
	  * This stream becomes no longer valid.
	  */
	virtual void close() {};
	
	/**
	  * Factory function to create an image stream of the desired format an
	  * associated to the given reference space.
	  *
	  * @param writer The object that does the actual writing of data to the stream.
	  * @param format Desired format
	  * @param rs Reference space.
	  */
	static ImgStream* createImgStream(StreamWriter* writer, Format format, ReferenceSpace& rs);
	
};

/**
  * A basic implementation of an image stream.
  */
class BaseImgStream : public ImgStream {
public:
	
	/** Creates the basic definition of an image stream. 
	  */
	BaseImgStream(StreamWriter* writer, ReferenceSpace& rs)
	: writer(writer), rs(rs) {}
	
	virtual ~BaseImgStream() { close(); }
	virtual StreamWriter* getWriter() { return writer; }
	virtual ReferenceSpace* getReferenceSpace() { return &rs; }
	
protected:
	StreamWriter* writer;
	ReferenceSpace& rs;
};



/**
  * image stream in an ad hoc ASCII format
  */
class AsciiImgStream: public BaseImgStream {
public:
	AsciiImgStream(StreamWriter* writer, ReferenceSpace& rs);
	virtual ~AsciiImgStream() {};
	virtual int writeChannelDef(ChannelDef& channel_def);
	virtual int startFrame(FrameDef& frame_def);
	virtual int writeRow(Row& row);
	Format getFormat() { return ASCII_FORMAT; }
};

/**
  * image stream in an ad hoc binary format
  */
class BinImgStream: public BaseImgStream {
public:
	BinImgStream(StreamWriter* writer, ReferenceSpace& rs);
	virtual ~BinImgStream() {};
	virtual int writeChannelDef(ChannelDef& channel_def);
	virtual int startFrame(FrameDef& frame_def);
	virtual int writeRow(Row& row);
	Format getFormat() { return BIN_FORMAT; }
};


/**
  * image stream in MNG format
  */
class MngImgStream: public BaseImgStream {
public:
	MngImgStream(StreamWriter* writer, ReferenceSpace& rs);
	virtual ~MngImgStream() {};
	virtual int writeChannelDef(ChannelDef& channel_def);
	virtual int startFrame(FrameDef& frame_def);
	virtual int writeRow(Row& row);
	Format getFormat() { return MNG_FORMAT; }
};




#endif
