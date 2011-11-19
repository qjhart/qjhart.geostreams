/**
 * 
 */
package geostreams.image;

import java.awt.Rectangle;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

/**
 * @author Carlos Rueda
 * @version $Id: ChannelDataReader.java,v 1.1 2006/11/28 06:17:44 crueda Exp $
 */
public class ChannelDataReader implements IDataReader {
	private FileChannel dis;
	private ByteBuffer buffer = ByteBuffer.allocate(8);
	private byte[] array = buffer.array();
	
	ChannelDataReader(String input) throws IOException {
		dis = new FileInputStream(input).getChannel();
	}
	
	public String readTag() throws IOException {
		buffer.position(0);
		buffer.limit(2);
		String tag = dis.read(buffer) == 2 ? new String(array, 0, 2): "EOF";
		return tag;
	}

	public int readUnsignedByte() throws IOException {
		buffer.position(0);
		buffer.limit(1);
		dis.read(buffer);
		return (int) 0xff & array[0];
	}

	public short readShort() throws IOException {
		buffer.position(0);
		buffer.limit(2);
		dis.read(buffer);
		return (short) ( (int) (0xff & array[0]) << 8 | (0xff & array[1]) );
	}

	public float readFloat() throws IOException {
		buffer.position(0);
		buffer.limit(4);
		dis.read(buffer);
		int bits = (int) (0xff & array[0]) << 8*3 | (0xff & array[1]) << 8*2 | (0xff & array[2] << 8) | (0xff & array[3]);
		return Float.intBitsToFloat(bits);
	}

	public String readString() throws IOException {
		StringBuffer sb = new StringBuffer();
		buffer.limit(1);
		
		while ( true ) {
			buffer.position(0);
			dis.read(buffer);
			int ch = array[0];
			if ( ch > 0 ) {
				sb.append((char) ch);
			}
			else {
				return sb.toString();	
			}
		}
	}

	public void close() throws IOException {
		dis.close();
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
