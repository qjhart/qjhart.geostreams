/**
 * 
 */
package geostreams.image;

import java.awt.Rectangle;
import java.io.IOException;

/**
 * @author Carlos Rueda
 * @version $Id: IDataReader.java,v 1.1 2006/11/28 06:17:44 crueda Exp $
 */
interface IDataReader {

	public String readTag() throws IOException;

	public Rectangle readRectangle(Rectangle rect) throws IOException;

	public int readUnsignedByte() throws IOException;

	public short readShort() throws IOException;

	public float readFloat() throws IOException;

	public String readString() throws IOException;

	public void close() throws IOException;

}