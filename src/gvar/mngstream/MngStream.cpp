/*
	GeoStreams Project
	MngStream - Module to create a MNG stream of images
	$Id: MngStream.cpp,v 1.7 2004/06/10 19:23:12 crueda Exp $
	See MngStream.h for public doc.
*/

#include "MngStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define PNG_MNG_FEATURES_SUPPORTED
#include <png.h>


// to catch SIGPIPE, see below.
static void sighandler(int signum) {
	fprintf(stderr, "-->signal received: %d\n.  exiting.", signum);
	exit(111);
}
static int sighandler_set = 0;

// table for CRC check
static unsigned int* crcTable = 0;
static void makeCrcTable() {
	if ( !crcTable ) {
		crcTable = new unsigned int[256];
		for( int i = 0; i < 256; i++){
			unsigned int crc = i;
			for( int j = 0; j < 8; j++) {
				if(crc & 1){ 
					crc = 0xedb88320 ^ (crc >> 1); 
				}
				else {
					crc = crc >> 1; 
				}
			}
			crcTable[i] = crc;
		}
	}
}

// we need to write in "network" byte order
// this function reverses the bytes of a datum
static void reverse_bytes(void* datum, int len) {
	char* bin = (char*) datum;
	char bout[len]; 
	for ( int i = 0; i < len; i++ ) {
		bout[i] = bin[len - i - 1];
	}
	memcpy(datum, bout, len);
}

// reverse the bytes of an int. If you're on a little-endian
// machine (like intel), then this means the conversion is
// to a "network" (big-endian) format.
static int network_uint(unsigned int i) {
	reverse_bytes(&i, sizeof(i));
	return i;
}
static int network_int(int i) {
	reverse_bytes(&i, sizeof(i));
	return i;
}

// reverse the shorts in an array
static void reverse_shortarray(unsigned short* array, int len) {
	for ( int i = 0; i < len; i++ ) {
		reverse_bytes(array + len, sizeof(unsigned short));
	}
}



// the MHDR structure
typedef struct {
	char name[4];     // == "MHDR"
	unsigned int frame_width;
	unsigned int frame_height;
	unsigned int ticks_per_second;
	unsigned int layer_count;
	unsigned int frame_count;
	unsigned int play_time;
	unsigned int simplicity_profile;
} MHDR;

// the DEFI structure,  12-byte long  (except for name)
typedef struct {
	char name[4];     // == "DEFI"
	unsigned short object_id;
	unsigned char do_not_show;
	unsigned char concrete_flag;
	int x_location;
	int y_location;
} DEFI;


///////////////////////////////////////////////////////////////////////

// class with data for actual implementation
struct MS {
	FILE* mngfile;
	int fwidth;
	int fheight;
	int mhdrWritten;
	
	// is the PNG signature already processed?
	int pngSigProcessed;
	
	MS(char* mngfilename_, int fwidth_, int fheight_);
	MS(FILE* mngfile_, int fwidth_, int fheight_);
	void init(FILE* mngfile_, int fwidth_, int fheight_);
	void writeChunk(const void* chunkData, unsigned int len);
	void writeDEFI(int x, int y);
	int addPng(int x, int y, char* filename);
	int addImage(int x, int y, uint16* rows[], int iwidth, int iheight);
	void close();
	void writeMHDR();
};



static int writePng(MS* ms, uint16* rows[], int iwidth, int iheight);



	
MS::MS(FILE* mngfile, int fwidth, int fheight) {
	init(mngfile, fwidth, fheight);
}

MS::MS(char* mngfilename, int fwidth, int fheight) {
	FILE* mngfile = fopen(mngfilename, "wb");
	if ( !mngfile ) {
		fprintf (stderr, "Cannot open output file %s.\n", mngfilename);
		exit(1);
	}
	init(mngfile, fwidth, fheight);
}
	
void MS::init(FILE* mngfile_, int fwidth_, int fheight_) {
	mngfile = mngfile_;
	fwidth = fwidth_;
	fheight = fheight_;
	mhdrWritten = 0;
	
	makeCrcTable();
}

// unconditionally writes the MHDR
void MS::writeMHDR() {
	fprintf(mngfile, "\x8aMNG\x0d\x0a\x1a\x0a");
	MHDR mhdr;
	strncpy(mhdr.name, "MHDR", 4);
	mhdr.frame_width = network_uint(fwidth);
	mhdr.frame_height = network_uint(fheight);
	mhdr.ticks_per_second = network_uint('d');
	mhdr.layer_count = 0;
	mhdr.frame_count = 0;
	mhdr.play_time = 0;
	mhdr.simplicity_profile = network_uint(1);
	writeChunk(&mhdr, sizeof(mhdr));
	mhdrWritten = 1;
}


// see above for a description of the general layout of each chunk
void MS::writeChunk(const void* chunkData, unsigned int len) {
	unsigned int crc = 0xffffffff;
	unsigned int net_len4 = network_uint(len - 4);

	fwrite(&net_len4, sizeof(net_len4), 1,  mngfile);
	fwrite(chunkData, len, 1,  mngfile);
	unsigned char* bytes = (unsigned char*) chunkData;
	for ( unsigned int i = 0; i < len; i++ ) {
		unsigned int $_ = (unsigned int) bytes[i];
		crc = crcTable[(crc ^ $_) & 0xff] ^ (crc >> 8);
	}
	crc = network_uint(~crc);
	fwrite(&crc, sizeof(crc), 1,  mngfile);
}


void MS::writeDEFI(int x, int y) {
	DEFI defi;
	strncpy(defi.name, "DEFI", 4);
	defi.object_id = 0;
	defi.do_not_show = 0;
	defi.concrete_flag = 1;
	defi.x_location = network_int(x);
	defi.y_location = network_int(y);
	writeChunk(&defi, sizeof(defi));
}



int MS::addPng(int x, int y, char* filename) {
	FILE* infile = fopen(filename, "r");
	if ( !infile) {
		fprintf(stderr, "Cannot open %s\n", filename);
		return 1;
	}
	
	// signature is ignored
	char png_header[8];
	if ( 1 != fread(png_header, sizeof(png_header), 1, infile) ) {
		fprintf(stderr, "Cannot read PNG header from %s\n", filename);
		return 2;
	}
	if ( png_sig_cmp((png_byte*) png_header, 0, 8) ) {
		fprintf(stderr, "Are you sure %s is a PNG?\n", filename);
		return 3;
	}
	
	if ( !mhdrWritten ) {
		writeMHDR();
	}
	
	writeDEFI(x, y);

	// now transfer png contents to mng stream
	char png_buffer[1024];
	int numread;
	while ( (numread = fread(png_buffer, 1, sizeof(png_buffer), infile)) > 0 ) {
		fwrite(png_buffer, 1, numread,  mngfile);
	}
	fclose(infile);
	return 0;
}

int MS::addImage(int x, int y, uint16* rows[], int iwidth, int iheight) {
	if ( !mhdrWritten ) {
		writeMHDR();
	}
	
	if ( iwidth > 0 && iheight > 0 ) {
		writeDEFI(x, y);
		return writePng(this, rows, iwidth, iheight);
	}
	else {
		fprintf(stderr, "Warning: trying to add an empty image, (w,h)=(%d,%d)\n",
			iwidth, iheight
		);
		return -1;
	}
}

// finishes the mng
void MS::close() {
	writeChunk("MEND", 4);
	fclose(mngfile);
}
	

///////////////////////////////////////////////////////////////////////

MngStream::MngStream(char* mngfilename, int fwidth, int fheight) {
	if ( strcmp(mngfilename, "-") == 0 ) {
		FILE* rstdout = fdopen(fileno(stdout), "wb");
		prv_data = new MS(rstdout, fwidth, fheight);
	}
	else
		prv_data = new MS(mngfilename, fwidth, fheight);
	
	// catch SIGPIPE to exit:
	if ( !sighandler_set ) {
		signal(SIGPIPE, sighandler);
		sighandler_set = 1;
	}
}

MngStream::MngStream(FILE* mngfile, int fwidth, int fheight) {
	prv_data = new MS(mngfile, fwidth, fheight);
}


void MngStream::close() {
	if ( !prv_data )
		return;

	MS* ms = (MS*) prv_data;
	ms->close();
	delete ms;
	prv_data = 0;
}

MngStream::~MngStream() {
	close();
}
	

int MngStream::addImage(int x, int y, uint16* rows[], int iwidth, int iheight) {
	MS* ms = (MS*) prv_data;
	int status = ms->addImage(x, y, rows, iwidth, iheight);
	if ( status ) {
		fprintf (stderr, "An error ocurred while trying to write PNG.\n");
		exit(1);
	}
	return status;
}
	

int MngStream::addPng(int x, int y, char* filename) {
	MS* ms = (MS*) prv_data;
	return ms->addPng(x, y, filename);	
}



///////////////////////////////////////////////////////////////////////////
//     PNG part

// this is only to skip the PNG signature as it seems there is no
// way to instruct libpng explicitly to do it
static void my_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	MS* ms = (MS*) png_get_io_ptr(png_ptr);
	if ( !ms->pngSigProcessed && length >= 8 ) {
		// data should start with the PNG signature:
		if ( png_sig_cmp(data, 0, 8) ) {
			fprintf(stderr, "MngStream: Warning: Expecting PNG signature to be skipped\n");
		}
		else {
			data += 8;
			length -= 8;
		}
		ms->pngSigProcessed = 1;
	}
	
	if ( length > 0 ) {
		png_size_t written = fwrite(data, 1, length, ms->mngfile);
		if ( length != written ) {
			char msg[1024];
			sprintf(msg, 
				"my_write_data: Cannot write requested bytes"
				"length=%d  written=%d", length, written
			);
			fprintf(stderr, "[%s]\n", msg);
			fflush(stderr);
			png_error(png_ptr, msg);
			return;
		}
		fflush(ms->mngfile);
	}
		
}

static void my_flush_data(png_structp png_ptr) {
	MS* ms = (MS*) png_get_io_ptr(png_ptr);
	fflush(ms->mngfile);
}


// Is this really necessary?
// (for now this is not done )
static void reverse_rows(uint16* rows[], int width, int height) {
	if ( 1 )
		return;
	for ( int i = 0; i < height; i++ )
		reverse_shortarray(rows[i], width);
}

static char* show_bits(uint16 v) {
	static char str[32];
	for ( int i = 0; i < 16; i++ ) {
		int b = v >> i;
		str[15 - i] = 0x01 & b ? '1' : '0';
	}
	str[16] = 0;
	return str;
}

static void show_rows(uint16* rows[], int width, int height) {
	
	// NOW TESTING IF BIT LOCATION IN TWO BYTES MAKES ANY DIFFERENCE
	// SO NOW I'M JUST DRPPING MSB BITS
	for ( int j = 0; j < height; j++ ) {
		for ( int i = 0; i < width; i++ ) {
			rows[j][i] = rows[j][i] >> 2;
		}
	}
	
	if ( 1 ) 
		return;
	
	for ( int i = 0; i < width; i++ ) {
		fprintf(stderr, "bits = %s %u", show_bits(rows[0][i]), rows[0][i]);
		if ( rows[0][i] > 255 )
			fprintf(stderr, "  !");
		fprintf(stderr, "\n");
	}
}

// outputs an image in PNG format to a MNG stream 
static int writePng(MS* ms, uint16* rows[], int width, int height) {
	png_structp png_ptr = png_create_write_struct(
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
	);
	if (!png_ptr) {
		fprintf (stderr, "Cannot create png_ptr.\n");
		return 1;
	}

	// Some first attempts to instruct libpng to know we are embedding PNG's
	// in a MNG stream.
	// png_permit_mng_features(png_ptr, PNG_ALL_MNG_FEATURES);
	// png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, 64);
	
	// only to skip the PNG signature--see above
	ms->pngSigProcessed = 0;
	png_set_write_fn(png_ptr, ms, my_write_data, my_flush_data);
		
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		return 2;
	}
	
	if ( setjmp(png_jmpbuf(png_ptr)) ) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return 3;
	}
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
	
	int color_type = PNG_COLOR_TYPE_GRAY;
	int interlace_type = PNG_INTERLACE_NONE;
	int sample_depth = 16;
	int filter_type = PNG_FILTER_TYPE_DEFAULT; 
	//int filter_type = PNG_INTRAPIXEL_DIFFERENCING; DOESN'T WORK
	png_set_IHDR(png_ptr, info_ptr,
	             width, height,
	             sample_depth, color_type, interlace_type,
	             PNG_COMPRESSION_TYPE_DEFAULT, filter_type);
	png_write_info(png_ptr, info_ptr);

	if ( 1 ) 
		show_rows(rows, width, height);
		
	if ( setjmp(png_jmpbuf(png_ptr)) ) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		// leave the image in the original order
		reverse_rows(rows, width, height);
		return 4;
	}
	
	reverse_rows(rows, width, height);
	png_write_image(png_ptr, (png_bytep*) rows);
	reverse_rows(rows, width, height);

	if ( setjmp(png_jmpbuf(png_ptr)) ) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return 5;
	}
	png_write_end(png_ptr, NULL);
	
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return 0;
}


