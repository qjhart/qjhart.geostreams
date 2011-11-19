/*
	GeoStreams Project
	Chunk
	Carlos A. Rueda
	$Id: Chunk.java,v 1.1 2004/07/28 01:16:17 crueda Exp $
*/
package geostreams.mngstream;

import java.io.InputStream;
import java.io.IOException;

/** A chunk in the stream */
public class Chunk {
	/** reported length of this chunk. */
	private int len;
	
	/** Name.*/
	private String name;
	
	/** the entire data for this chunk. */
	private byte[] data;

	/** Usable length of data array. */
	private int dataLen;
	
	// aux to read in length + name
	private static byte[] lenname = new byte[8];

	/** Creates an empty chunk. 
	 *  Call read(InputStream) next to fill in data .*/
	public Chunk() {
	}
	
	/** Fills in this chunk with data from the InputStream.*/
	public void read(InputStream is) throws IOException {
		// read in length + name
		for ( int i = 0; i < 8; i++ ) {
			lenname[i] = (byte) is.read();
		};
		len = Util.bytearray2int(lenname, 0);
		name = new String(lenname, 4, 4);
		//System.out.println("len = [" +len+ "]  name = [" +name+ "]");
		
		dataLen = 4 + 4 + len + 4;
		if ( data == null || data.length < dataLen )
			data = new byte[dataLen];
		System.arraycopy(lenname, 0, data, 0, 8);
		for ( int i = 0; i < len + 4; i++ ) {
			data[8 + i] = (byte) is.read();
		}
	}
	
	/** Gets the usable length of getData(). 
	 *  (Do not use getData().length). */
	public int getDataLength() {
		return dataLen;
	}

	/** Gets the name this chunk. */
	public String getName() {
		return name;
	}

	/** Gets the entire data this chunk. */
	public byte[] getData() {
		return data;
	}
}

