/*
 * GeoStreams Project
 * Carlos Rueda-Velasquez
 */
package geostreams.image;

import java.awt.image.BufferedImage;
import java.awt.image.PixelGrabber;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
  * A basic image implementation.
  * 
  * @author Carlos Rueda-Velasquez
  * @version $Id: BasicImage.java,v 1.13 2007/06/13 00:08:42 crueda Exp $
  */
class BasicImage implements IImage, Serializable {
	
	////////////////
	// TODO : 0 istream
	
	// An associated IStream would be sent separately
	transient private IStream istream;
	
	public IStream getStream() { return istream; }
	public void setStream(IStream istream) { 
		this.istream = istream;
		if ( this.istream != null) {
			this.streamID = this.istream.getStreamID();
		}
	}
	////////////////

	
	
	/**
	 * ID of the stream this image belongs to.
	 */
	private String streamID; 	

	private String channelID;
	
	String frameID;
	int x, y, width, height;	  
	
	/** width*height values */
	int[] data;
	
	transient IServerConnection serverConnection;
	transient BufferedImage buff_img;
	
	private boolean isNull;
	
	/**
	 * Bare-bones constructor; it only sets the streamID and channelID attrs.
	 *
	 * @param channelID Used for both streamID and channelID
	 */
	BasicImage(String channelID) {
		this(channelID,  channelID);
	}
	
	BasicImage(String streamID, String channelID) {
		this.streamID = streamID;
		this.channelID = channelID;
	}
	
	public void setStreamID(String streamID) {
		this.streamID = streamID;
	}
	
	public IImage clone() {
		BasicImage nimg = new BasicImage(this.streamID, this.channelID);
		nimg.serverConnection = this.serverConnection;
		nimg.frameID = this.frameID;
		nimg.x = this.x;
		nimg.y = this.y;
		nimg.width = this.width;
		nimg.height = this.height;
		nimg.data = this.data;
		nimg.buff_img = this.buff_img;
		nimg.isNull = this.isNull;
		return nimg;
	}
	
	/** adjust the data array according to current with and height */
	void adjustDataArray() {
		int img_size = getWidth() * getHeight();
		if ( data == null || data.length < img_size ) {
			data = new int[img_size];
		}
	}
	
	public void setServerConnection(IServerConnection serverConnection) {
		this.serverConnection = serverConnection;
	}
	public IServerConnection getServerConnection() { return serverConnection; }

	public boolean isNull() { return isNull; }
	public final String getStreamID() { return streamID; }
	public final String getChannelID() { return channelID; }
	public String getFrameID() { return frameID; }
	public void setFrameID(String frameID) {
		this.frameID = frameID;
	}
	public int getX() { return x; }
	public int getY() { return y; }
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	
	
//	public int[] getData() { return data; }
	
	private int[] getData() {
		if ( data != null )
			return data;
		
		int w = width;
		int h = height;
		data = new int[w * h];
		PixelGrabber pg = new PixelGrabber(buff_img, 0, 0, w, h, data, 0, w);
		try {
			pg.grabPixels();
		} catch (InterruptedException e) {
			System.err.println("interrupted waiting for pixels!");
			Thread.currentThread().interrupt();
		}
		
		///// // else: get data from the buff_img
		///// 
		///// how this works?
		///// 
		///// Raster raster = buff_img.getData();
		///// 
		///// data = raster.getPixels(0, 0, getWidth(), getHeight(), (int[])null);
		
		
		for ( int i = 0; i < data.length; i++ ) {
			data[i] = 0xff & data[i];
		}
		
		return data;
	}
	
	
	
	/**
	 * TODO (low) Does a gamma correction, but any correction should actually 
	 * be done as a separate operation.
	 */
	public synchronized BufferedImage getBufferedImage() { 
		if ( buff_img == null ) {
			assert data != null ;
			
			int sampleSize = -1;
			
			if ( istream != null) {
				ChannelDef channelDef = istream.getChannelDef();
				sampleSize = channelDef.getSampleSize();
			}
			else if ( serverConnection != null ) {    // TODO remove this else block
				ChannelDef channelDef = serverConnection.getChannelDefForStream(getStreamID());
				sampleSize = channelDef.getSampleSize();
			}
			
			int[] _pixels = new int[width*height];
			
			final double gamma = 1.0/2.5;
			final double max = 255.0;
			for ( int i = 0, img_size = width*height; i < img_size; i++ ) {
				int val = data[i];
				if ( sampleSize > 8 ) {
					// We know that it must be 10 bits, then.
					// So, only right-shift 1 bit so the intensity
					// doesn't decrease too much and the image shows
					// better:
					
					int shift = sampleSize - 8;                       

					// TODO gamma correction NOTE: This condition is hard-coded for channel 3
					////if ( iimg.getChannelID() == 3 ) {
					////	shift = 2;
					////}
					////else {
					////	shift = 1;
					////}
					
					val = 0xFF & (val >> shift);
				}
		
				if ( val > 255 )
					val = 255;

				double fval = val;
				
				// gamma correction:
				fval = Math.pow(fval/max, gamma);
				fval = fval * 256;
				val = (int) Math.round(fval);
				if ( val > 255 )
					val = 255;
				
				// create gray scale pixel:  TODO alpha?
				_pixels[i] = (255 << 24) | (val << 16) | (val << 8) | val;
			}
			
			buff_img = new BufferedImage(width, height, 
//					BufferedImage.TYPE_INT_RGB
					BufferedImage.TYPE_4BYTE_ABGR
			);
			
			int[] rgbArray = _pixels;
			int offset = 0;
			int scansize = width;
			buff_img.setRGB(0, 0, width, height, rgbArray, offset, scansize);
		}
		return buff_img; 
	}
	
	/**
	 * Serialize this instance.
	 * 
	 * @param oos
	 * @throws IOException
	 */
	private void writeObject(ObjectOutputStream oos) throws IOException {
		// force data array is non-empty
		getData();
		assert data != null ;
		oos.defaultWriteObject();
	}

		
	/** gets a string version of this image.
	  * including a few of the first pixel values
	  */
	public String toString() {
		if ( isNull() )
			return "<null-image>";
		
		StringBuffer sb = new StringBuffer("{img|");
		sb.append("ID=" +streamID+ ",");
		sb.append("FR=" +frameID+ ",");
		sb.append("CH=" +channelID+ ",");
		sb.append("x=" +x+ ",");
		sb.append("y=" +y+ ",");
		sb.append("w=" +width+ ",");
		sb.append("h=" +height+ ",");
		sb.append("d=");
		if ( data != null ) {
			int n = Math.min(5, data.length);
			for ( int i = 0; i < n; i++ ) {
				sb.append(data[i]+ ",");
			}
			sb.append("..");
		}
		else {
			sb.append("<img>");
		}
		sb.append("}");
		return sb.toString();
	}
}

