/*
	GeoStreams Project
	misc utilities
	Carlos A. Rueda
	$Id: Util.java,v 1.3 2004/07/26 22:43:54 crueda Exp $
*/
package geostreams.mngstream;

/** some utilities */
abstract class Util {
	private Util() {} 
	
	static int bytearray2int(byte[] b, int from) {
		int a0 = 0xff & b[from];
		int a1 = 0xff & b[from+1];
		int a2 = 0xff & b[from+2];
		int a3 = 0xff & b[from+3];
		int len = (a0<<24) | (a1<<16) | (a2<<8) | a3;
		return len; 
	}
}
