package geostreams.image;

import java.awt.Rectangle;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;

public class DataReader implements IDataReader {
	private DataInputStream dis;
	private final byte[] atag = new byte[2];

	DataReader(InputStream input) throws IOException {
		dis = new DataInputStream(input);
	}
	
	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#readTag()
	 */
	public String readTag() throws IOException {
		String tag = dis.read(atag) == atag.length ? new String(atag) : "EOF";
		return tag;
	}
	
	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#readUnsignedByte()
	 */
	public int readUnsignedByte() throws IOException {
		return dis.readUnsignedByte();
	}
	
	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#readShort()
	 */
	public short readShort() throws IOException {
		return dis.readShort();
	}
	
	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#readFloat()
	 */
	public float readFloat() throws IOException {
		return dis.readFloat();
	}

	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#readString()
	 */
	public String readString() throws IOException {
		StringBuffer sb = new StringBuffer();
		int ch;
		while ( (ch = dis.read()) > 0 ) {
			sb.append((char) ch);
		}
		return sb.toString();
	}

	/* (non-Javadoc)
	 * @see geostreams.image.IDataReader#close()
	 */
	public void close() throws IOException {
		if ( dis != null )
			dis.close();
		dis = null;
	}

	public Rectangle readRectangle(Rectangle rect) throws IOException {
		if ( rect == null ) {
			rect = new Rectangle();
		}
		rect.x      = readShort(); 
		rect.y      = readShort(); 
		rect.width  = readShort(); 
		rect.height = readShort();

		return rect;
	}
}
