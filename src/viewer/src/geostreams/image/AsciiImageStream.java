/*
 * GeoStreams
 * Carlos Rueda-Velasquez
 */
package geostreams.image;

import java.io.InputStream;
import java.io.IOException;

/**
  * An image stream based on reading lines
  * from a text source.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: AsciiImageStream.java,v 1.3 2007/06/01 21:40:01 crueda Exp $
  */ 
class AsciiImageStream extends BasicImageStream {
	InputStream input;
	LineReader reader;
		
	/**	the next line of tokens. Acts as a lookahead element. */
	private String[] tokens;
		
	/** creates an image stream. 
	  * @param input Provider of data. 
	  */
	AsciiImageStream(String channelDef, InputStream input) {
		super(channelDef);
		this.input = input;
		System.out.println("AsciiImageStream created");
	}
	
	public void startReading() throws IOException {
		if ( reader != null ) {
			return;
		}
		reader = new LineReader(input);
		System.out.println("AsciiImageStream: reading stream ...");
		reader.startReading();
		
		tokens = _readTokenLine();
		
		if ( !tokens[0].equals("RS") )
			throw new IOException("RS expected");

		if ( tokens.length < 5 )
			throw new IOException("RS line must have at least 5 tokens");
		
		// format:
		//	tokens[0] = "RS"
		//	tokens[1] = x
		//	tokens[2] = y
		//	tokens[3] = width     
		//	tokens[4] = height     
		
		referenceSpace.rect.x      = Integer.parseInt(tokens[1]); 
		referenceSpace.rect.y      = Integer.parseInt(tokens[2]); 
		referenceSpace.rect.width  = Integer.parseInt(tokens[3]); 
		referenceSpace.rect.height = Integer.parseInt(tokens[4]);

		tokens = _readTokenLine();

		// TODO (fix in server) consume possible channel definitions:
		while ( tokens[0].equals("CH") ) {
			System.out.println("AsciiImageStream: IGNORING channel definitions in stream. Update server.");
			if ( tokens.length < 7 )
				throw new IOException("CH line must have at least 7 tokens");
			
			// format:
			//	tokens[0] = "CH"
			//	tokens[1] = channel ID
			//	tokens[2] = pixel size in x
			//	tokens[3] = pixel size in y
			//	tokens[4] = aspect ratio
			//	tokens[5] = sample size
			//	tokens[6] = name

			// IGNORING ALL OF THESE.  See connection.ServerConnection
//			channelID      = tokens[1];
//			int psx        = Integer.parseInt(tokens[2]);
//			int psy        = Integer.parseInt(tokens[3]);
//			double aspect  = Double.parseDouble(tokens[4]);
//			int sampleSize = Integer.parseInt(tokens[5]);
//			String name    = tokens[6];


			tokens = _readTokenLine();
		}
	}

	public IImage nextImage() throws IOException {
		// make the image null in case we get EOF:
//		BasicImage basicImage = new BasicImage(this);
		BasicImage basicImage = new BasicImage(this.getStreamID(), this.getChannelID());

		// while next item is FR:
		while ( tokens[0].equals("FR") ) {		
			if ( tokens.length < 6 )
				throw new IOException("FR line must have at least 6 tokens");
			
			// format:
			//	tokens[0] = "FR"
			//	tokens[1] = frame ID
			//	tokens[2] = x
			//	tokens[3] = y
			//	tokens[4] = width
			//	tokens[5] = height

			FrameDef frameDef = new FrameDef();
			
			frameDef.ID          = tokens[1]; 
			frameDef.rect.x      = Integer.parseInt(tokens[2]); 
			frameDef.rect.y      = Integer.parseInt(tokens[3]); 
			frameDef.rect.width  = Integer.parseInt(tokens[4]); 
			frameDef.rect.height = Integer.parseInt(tokens[5]);
			
			_imgObserver.notifyFrameStarts(this.streamID, frameDef);
			
			tokens = _readTokenLine();
		}		
		
		// now, we should get a RW item:
		if ( tokens[0].equals("RW") ) {
			
			if ( tokens.length < 6 )
				throw new IOException("RW line must have at least 6 tokens");
			
			// format:
			//	tokens[0] = "RW"
			//	tokens[1] = frame ID
			//	tokens[2] = channel ID
			//	tokens[3] = x location
			//	tokens[4] = y location
			//	tokens[5] = width
			//	tokens[6:] = data (width*1 values)
			
			basicImage.frameID    = tokens[1]; 
			String thisChannelID  = tokens[2]; 
			basicImage.x          = Integer.parseInt(tokens[3]); 
			basicImage.y          = Integer.parseInt(tokens[4]); 
			basicImage.width      = Integer.parseInt(tokens[5]); 
			basicImage.height     = 1;
			
			if ( !channelID.equals(thisChannelID) ) {
				System.out.println("BinaryImageStream: Ignoring unexpected channel for this image");
			}
			
			// get data
			int img_size = basicImage.getWidth() * basicImage.getHeight();
			if ( basicImage.data == null || basicImage.data.length < img_size )
				basicImage.data = new int[img_size];
			
			int numvals = Math.min(basicImage.data.length, tokens.length - 6);
			for ( int i = 0; i < numvals; i++ ) {
				int val = Integer.parseInt(tokens[6 + i]);
				basicImage.data[i] = val; 
			}
		}
		else if ( tokens[0].equals("EOF") ) {
			return null;
		}
		else {
			throw new IOException("Expecting RW");
		}
		
		tokens = _readTokenLine();
		
		return basicImage;		
	}

	public void close() throws IOException {
		if ( reader != null )
			reader.close();
		reader = null;
	}
	
	private static final String[] EOF = { "EOF" };
	
	/** reads the next line of tokens. */
	private String[] _readTokenLine() throws IOException {
		String[] toks = EOF;
		if ( reader != null ) {
			try {
				while ( true ) {  // to skip empty lines
					String line = reader.readLine();
					if ( line == null )
						break;
					String[] toks2 = line.split("\\s+");
					if ( toks2.length > 0 ) {
						toks = toks2;
						break;
					}
				}
			}
			catch(IOException ex) {
				try{ reader.close(); } catch (Exception ex2){}
				reader = null;
			}
		}

		return toks;
	}

	
	static class LineReader {
		private InputStream input;
		private StringBuffer sb;
		private int ch;
		
		LineReader(InputStream input) throws IOException {
			this.input = input;
			sb = new StringBuffer();
		}
		
		void startReading() throws IOException {
			ch = input.read();
		}
		
		String readLine() throws IOException {
			sb.setLength(0);
			if ( input == null || ch < 0 ) {
				input = null;
				return null;
			}
			while ( ch >= 0 && ch != '\n' ) {
				sb.append((char) ch);
				ch = input.read();
			}
			if ( ch >= 0 )
				ch = input.read();
				
			return sb.toString();
		}

		void close() throws IOException {
			if ( input != null )
				input.close();
			input = null;
		}
	}
}

