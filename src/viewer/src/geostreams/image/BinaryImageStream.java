package geostreams.image;

import java.io.InputStream;
import java.io.IOException;

/**
  * An image stream based on reading data using an IDataReader 
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: BinaryImageStream.java,v 1.4 2007/06/01 21:40:01 crueda Exp $
  */ 
class BinaryImageStream extends BasicImageStream {
	private Object input;
	private IDataReader reader;
	
	/**	the next tag. Acts as a lookahead element. */
	private String tag;

	private String exMsg = null; // for toString
	
	/** creates an image stream.                            
	  */
	BinaryImageStream(String channelDef, InputStream input) {
		super(channelDef);
		this.input = input;
		System.out.println(_getName()+ " created");
	}
	
	/** creates an image stream.                            
	 */
	BinaryImageStream(String channelDef, String input) {
		super(channelDef);
		this.input = input;
		System.out.println("BinaryImageStream created");
	}
	
	public void startReading() throws IOException {
		if ( reader != null ) {
			return;
		}
		
		if ( input instanceof InputStream ) {
			System.out.println("BinaryImageStream: creating DataReader");
			reader = new DataReader((InputStream) input);
		}
		else {
			System.out.println("BinaryImageStream: creating ChannelDataReader");
			reader = new ChannelDataReader(input.toString());
		}
		System.out.println("BinaryImageStream: reading stream ...");
		
		_getNextTag();
		if ( !tag.equals("RS") ) {
			throw new IOException("RS expected");
		}
		
		reader.readRectangle(referenceSpace.rect); 

		_getNextTag();

		// TODO (fix in server) consume possible channel definitions:
		while ( tag.equals("CH") ) {
			System.out.println("BinaryImageStream: IGNORING channel definitions in stream. Update server.");
			/*int channelID  =*/ String.valueOf(reader.readShort());  
			/*int psx        =*/ reader.readShort(); 
			/*int psy        =*/ reader.readShort(); 
			/*double aspect  =*/ reader.readFloat(); 
			/*int sampleSize =*/ reader.readUnsignedByte(); 
			/*String name    =*/ reader.readString();

			
			_getNextTag();
		}
		
	}
	
	private void _getNextTag() throws IOException {
//		System.out.print("               Reading tag=");   System.out.flush();
		try {
			tag = reader.readTag();
		}
		catch(IOException ex) {
			exMsg = ex.getClass()+ "->" +ex.getMessage();
			throw ex;
		}
//		System.out.println("[" +tag+ "]");
	}

	public IImage nextImage() throws IOException {

		// while next item is FR:
		while ( tag.equals("FR") ) {
			FrameDef frameDef = new FrameDef();
			
			frameDef.ID          = String.valueOf(reader.readShort()); 
			frameDef.rect.x      = reader.readShort(); 
			frameDef.rect.y      = reader.readShort(); 
			frameDef.rect.width  = reader.readShort(); 
			frameDef.rect.height = reader.readShort();
			
			_imgObserver.notifyFrameStarts(this.channelID, frameDef);
			
			_getNextTag();
		}
		
		// now, we should get a RW item:
		if ( tag.equals("RW") ) {
			BasicImage basicImage = new BasicImage(this.getStreamID(), this.getChannelID());
			basicImage.data = null;
			basicImage.buff_img = null;
			
			basicImage.frameID    = String.valueOf(reader.readShort()); 
			String thisChannelID  = String.valueOf(reader.readShort()); 
			basicImage.x          = reader.readShort(); 
			basicImage.y          = reader.readShort(); 
			basicImage.width      = reader.readShort(); 
			basicImage.height     = 1;
			
			if ( !channelID.equals(thisChannelID) ) {
				System.out.println("BinaryImageStream: Ignoring unexpected channel for this image: " +
						"channelID=" +channelID+ "  thisChannelID=" +thisChannelID);
			}
			
			// get data
			basicImage.adjustDataArray();
			int img_size = basicImage.getWidth() * basicImage.getHeight();
			
			for ( int i = 0; i < img_size; i++ ) {
				int val = reader.readShort();
				basicImage.data[i] = val; 
			}
			
			_getNextTag();
			
			return basicImage;		
		}
		else if ( tag.equals("EOF") ) {
			if ( _nullImageSent ) {
				return null;
			}
			else {
				_nullImageSent = true;
				return Images.nullImage;
			}
		}
		else {
			throw new IOException("Expecting RW.  Got [" +tag+"]");
		}
	}

	public void close() throws IOException {
		if ( reader != null )
			reader.close();
		reader = null;
	}

	public String toString() {
		return _getName()+ (exMsg != null ? " ex=[" +exMsg+ "]" : "");
	}
}

