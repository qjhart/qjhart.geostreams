/**
 * common point to call conversion routines 
 * @author Carlos A. Rueda-Velasquez
 * @version $Id: myelug.h,v 1.4 2006/12/08 02:29:53 crueda Exp $
 */

void myelug_init(void);

int myelug_lldeg2pl(float lon, float lat, float *pixel, float* line);

int myelug_pl2lldeg(float pixel, float line, float *rlon, float *rlat);


class LineStringWriter {
public:
	LineStringWriter() { reset(); }
	void reset();
	void addPoint(int ipixel, int iline, float lon, float lat);	
private:
	struct { int pixel, line; float lon, lat; } previous;
	bool have_previous;
	bool points_included;
};
