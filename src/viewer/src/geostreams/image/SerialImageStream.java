package geostreams.image;

import java.awt.Rectangle;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;

/**
 * An image stream that reads from an object stream.
 * NOTE: not completely implemented, used, or tested yet.
 * 
 * <pre>
 * 		Rectangle
 * 		FrameDef*
 * 		BasicImage*
 * 		
 * </pre>
 * 
 * @author Carlos Rueda
 * @version $Id: SerialImageStream.java,v 1.2 2007/01/12 02:34:07 crueda Exp $
 */
class SerialImageStream extends BasicImageStream {
	//
	// I follow the general logic in BinaryImageStream, even conserving some of
	// the names in there, eg., "tag", which here is an Object instead of String
	
	private InputStream input;
	private ObjectInputStream reader;
	
	
	/**	the next tag. Acts as a lookahead element. */
	private Object tag;
	
	private String exMsg = ""; // for toString

	/**
	 * 
	 */
	public SerialImageStream(String channelDef, InputStream input) {
		super(channelDef);
		this.input = input;
		System.out.println("SerialImageStream created");
	}

	private void _getNextTag() throws IOException {
//		System.out.print("               Reading tag=");   System.out.flush();
		try {
			tag = reader.readObject();
		}
		catch(EOFException ex) {
			tag = "EOF";
		}
		catch(IOException ex) {
			exMsg = ex.getClass()+ "->" +ex.getMessage();
			throw ex;
		}
		catch(Exception ex) {
			exMsg = ex.getClass()+ "->" +ex.getMessage();
			throw new IOException(ex.getMessage());
		}
//		System.out.println("[" +tag+ "]");
	}

	public void startReading() throws IOException {
		if ( reader != null ) {
			return;
		}
		System.out.println("SerialImageStream: creating ObjectInputStream");
		reader = new ObjectInputStream(input);

		System.out.println("SerialImageStream: reading stream ...");
		
		_getNextTag();
		if ( !(tag instanceof Rectangle) ) {
			throw new IOException("Rectangle for RS expected");
		}
		
		referenceSpace.rect = (Rectangle) tag;

		_getNextTag();
	}

	public IImage nextImage() throws IOException {
		// while next item is FR:
		while ( tag instanceof FrameDef ) {
			FrameDef frameDef = (FrameDef) tag;
			
			_imgObserver.notifyFrameStarts(this.channelID, frameDef);
			
			_getNextTag();
		}
		
		// now, we should get a RW item:
		if ( tag instanceof BasicImage ) {
			BasicImage basicImage = (BasicImage) tag;
			return basicImage;
		}
		else if ( "EOF".equals(tag) ) {
			if ( _nullImageSent ) {
				return null;
			}
			else {
				_nullImageSent = true;
				return Images.nullImage;
			}
		}
		else {
			throw new IOException("Expecting BasicImage.  Got [" +tag+"]");
		}
		
	}

	public void close() throws IOException {
		reader.close();
	}
	
	public String toString() {
		return "SerialImageStream: str=[" +exMsg+ "]";
	}
}
