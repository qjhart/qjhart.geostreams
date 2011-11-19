package geostreams.image;

import java.awt.Rectangle;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;


/**
  * Creates IImageStream objects according to recognized
  * input formats.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: ImageStreams.java,v 1.2 2006/12/21 02:13:11 crueda Exp $
  * @navassoc - creates - IImageStream
  */
class ImageStreams {
	
	/** 
	 * @return the new image stream. 
	 * @param server NetInterfaceServer
	 * @param channelID Desired channel 
	 * @param roi Desired ROI 
	 * @param nodata No data? 
	 */
	static BasicImageStream openStream(String server, String channelID, Rectangle roi, boolean nodata) 
	throws Exception {
		String request = "geostream?format=1&channels=" +channelID;
		if ( roi != null ) {
			request += "&bbox=" +roi.x+ "," +roi.y+ "," +roi.width+ "," +roi.height;
		}
		if (nodata) {
			request += "&nodata";
		}
		
		BasicImageStream imgStream = _openStream(channelID, server, request);
		if (imgStream != null) {
			imgStream.channelID = channelID;
		}
		return imgStream;
	}

	/** Opens the stream with the complete URL given.
	 * @return the new image stream. 
	 */
	private static BasicImageStream _openStream(String channelDef, String server, String request) 
	throws Exception {
		
		BasicImageStream imgStream = null;

		if ( !server.startsWith("http:")
		&&   !server.startsWith("file:") ) {
			server = "file:" +server;
		}
		
		System.out.println("Opening: " +server+ "/" +request);
		
		String format = null;
		
		if ( request.indexOf("channels=") >= 0 ) {
			//
			// see enum Format in ImgStream.h
			//
			if ( request.indexOf("format=1") >= 0 
			||   request.indexOf("format_1") >= 0 
			) {
				format = "binary";
			}
			else if ( request.indexOf("format=2") >= 0
			     ||   request.indexOf("format_2") >= 0        
			) {
				format = "ascii";
			}
			else if ( request.indexOf("format=") >= 0
			     ||   request.indexOf("format_") >= 0
			) {
				System.out.println(" The format is not recognized");
				return null;
			}
		}
		else if ( request.endsWith(".bin") ) {
			format = "binary";
		}			
		else if ( request.endsWith(".txt") ) {
			format = "ascii";
		}
		
		if ( format == null ) { // assume ascii stream
			System.out.println("(assuming ascii)");
			format = "ascii";
		}
		System.out.println("  with format: " +format);

		
		URL server_url = new URL(server);
		InputStream is;
		if ( server_url.getProtocol().equals("file")) {
			File file = new File(server_url.getPath()+ "/" +request);
			if ( !file.exists() ) {
				request = request.replaceAll("&|=|\\?", "_");
				String fileName = server_url.getPath()+ "/" +request;	
				if ( format.equals("binary") ) {
					imgStream = new BinaryImageStream(channelDef, fileName);
					return imgStream;
				}
				file = new File(fileName);	
			}
			is = new FileInputStream(file);
		}
		else {
			//URL cmd_url = new URL(server_url.getPath()+ "/" +request);
			URL cmd_url = new URL(server+ "/" +request);
			is = cmd_url.openStream();
		}

		if ( format.equals("binary") ) {
			imgStream = new BinaryImageStream(channelDef, is);
		}
		else if ( format.equals("ascii") ) {
			imgStream = new AsciiImageStream(channelDef, is);
		}
		else {
			imgStream = new MngImageStream(channelDef, is);
		}
		
		return imgStream;
	}

	// non-instanceable 
	private ImageStreams() {}
	
	
	///////////////////////////////////////////////////////////////////////////
	//
	// test program to dump the contents of an image stream stored in a file
	//
	private static IDataReader _reader;
	private static String _tag;
	private static void _out(Object obj){
		System.out.println(obj.toString());
	}
	private static void _getNextTag() throws IOException {
		_tag = _reader.readTag();
		_out("[[[" +_tag+ "]]]");
	}
	private static short _readShort() throws IOException {
		short val = _reader.readShort();
		_out("" +val);
		return val;
	}
	private static void _readFloat() throws IOException {
		_out("" +_reader.readFloat());
	}
	private static void _readUnsignedByte() throws IOException {
		_out("" +_reader.readUnsignedByte());
	}
	private static void _readString() throws IOException {
		_out("" +_reader.readString());
	}
	private static boolean _nextImage() throws IOException {
		_out("NEXT IMAGE");
		while ( _tag.equals("FR") ) {
			_readShort(); 
			_readShort(); 
			_readShort(); 
			_readShort(); 
			_readShort(); 
			
			_getNextTag();
		}
		if ( _tag.equals("RW") ) {
			_readShort(); 
			_readShort(); 
			_readShort(); 
			_readShort(); 
			int width = _readShort(); 
			
			// get data
			_out("DATA: " +width+ " pixels");
			for ( int i = 0; i < width; i++ ) {
				_readShort();
			}
		}
		else if ( _tag.equals("EOF") ) {
			return false;
		}
		else {
			throw new IOException("Expecting RW.  Got [" +_tag+"]");
		}
		
		_getNextTag();
		
		return true;		
	}
	public static void main(String[] args) throws Exception {
		String fileName = "/home/carueda/goesdata/local/geostream_format_1_channels_2";
		final int maxImages = 0;
		
		System.err.println("INPUT: " +fileName);
		if ( args.length == 1 && args[0].equals("stdReader") ) {
			System.err.println("USING STANDARD READER");
			_reader = new DataReader(new FileInputStream(fileName));
		}
		else {
			System.err.println("USING CHANNELS");
			_reader = new ChannelDataReader(fileName);
		}
		System.err.println("max num of images = " +maxImages);
		
		_getNextTag();
		if ( !_tag.equals("RS") ) {
			throw new IOException("RS expected");
		}
		Rectangle rect= _reader.readRectangle(null);
		_out(rect);

		_getNextTag();
		
		while ( _tag.equals("CH") ) {
			System.out.println("BinaryImageStream: IGNORING channel definitions in stream. Update server.");
			_readShort();  
			_readShort(); 
			_readShort(); 
			_readFloat(); 
			_readUnsignedByte(); 
			_readString();

			_getNextTag();
		}

		for (int i = 0; i < maxImages && _nextImage(); i++) {
		}
		
		System.err.println("DONE");
	}
}

