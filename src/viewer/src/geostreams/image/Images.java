package geostreams.image;

import geostreams.util.Utils;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.io.Serializable;
import java.util.Hashtable;

/**
 * IImage and AWT image utilities.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Images.java,v 1.13 2007/07/29 02:50:30 crueda Exp $
 */
public class Images {
	
	// TODO : 0 istream
	public static IStream createStream(String streamID, IStream from) {
		assert streamID != null;
		assert from != null;
		BasicStream str = (BasicStream) from;
		BasicStream nstr = new BasicStream(str);
		
		nstr.setStreamID(streamID);
		
		return nstr;
	}
	public static IStream createStream(String streamID, IServerConnection isconn) {
		BasicStream nstr = new BasicStream(streamID, isconn);
		return nstr;
	}
	public static IStream createStream(String streamID, ReferenceSpace referenceSpace, ChannelDef channelDef) {
		BasicStream nstr = new BasicStream(streamID, referenceSpace, channelDef);
		return nstr;
	}
	
	
	/**
	 *  Creates a new IImage with given location and image data,
	 *  together with other attributes from a given image.
	 * 
	 * @param from Image to copy attribute from.
	 * @param x X location
	 * @param y Y location
	 * @param bufImg Data for the image
	 * @return the new object.
	 * @throws IllegalArgumentException If from.isNull()
	 * @throws IllegalArgumentException If bufImg argument is null
	 */
	public static IImage createImage(IImage from, int x, int y, BufferedImage bufImg) {
		if ( from.isNull() ) {
			throw new IllegalArgumentException("Given image is null (from.isNull())");
		}
		if ( bufImg == null ) {
			throw new IllegalArgumentException("bufImg argument must be non-null");
		}
		BasicImage img = (BasicImage) from;
		BasicImage nimg = new BasicImage(img.getStreamID(), img.getChannelID()); 
		nimg.serverConnection = img.serverConnection;
		nimg.frameID = img.frameID;
		nimg.x = x;
		nimg.y = y;
		nimg.width = bufImg.getWidth();
		nimg.height = bufImg.getHeight();
		nimg.data = null;
		nimg.buff_img = bufImg;
		return nimg;
	}
	
	/**
	 * Extracts a subregion from an IImage.
	 * Subregion given in reference space coordinates and units.
	 * @param inImg
	 * @param rect Desired subregion
	 * @return
	 */
	public static IImage extractImage(IImage inImg, Rectangle rect) {
		IImage outImg;
		if ( rect.isEmpty() ) {
			outImg = Images.nullImage;
		}
		else {
			outImg = extractImage(inImg, rect.x, rect.y, rect.width, rect.height);
		}
		return outImg;
	}
	
	/**
	 * Extracts a subregion from an IImage.
	 * Subregion given in reference space coordinates and units.
	 * @param inImg The source image
	 * @param x
	 * @param y
	 * @param w
	 * @param h
	 * @return The subregion
	 */
	public static IImage extractImage(IImage inImg, int x, int y, int w, int h) {
		// region to extract from input image:
		
		ChannelDef channelDef = null;
		IStream istream = inImg.getStream();
		if ( istream != null ) {
			channelDef = istream.getChannelDef();
		}
		else {
			// FIXME 0 : istream  This will dissapear
			IServerConnection gscon = inImg.getServerConnection();
			if ( gscon != null ) {
				StreamDef streamDef = gscon.getStreamDef(inImg.getStreamID());
				channelDef = streamDef.getChannelDef();
			}
		}
		assert channelDef != null || Utils.fail("Image.extractImage: channelDef not found for streamID: " +inImg.getStreamID());
		
		int psx = channelDef.getPixelSizeX();
		int psy = channelDef.getPixelSizeY();
		int sub_x = (x - inImg.getX()) / psx;
		int sub_y = (y - inImg.getY()) / psy;
		int sub_w = w / psx;
		int sub_h = h / psy;
		if ( sub_w == 0 ) {
			sub_w = 1;
		}
		if ( sub_h == 0 ) {
			sub_h = 1;
		}
		
		BufferedImage inBufImg = inImg.getBufferedImage();
		BufferedImage outBufImg = extractImage(inBufImg, sub_x, sub_y, sub_w, sub_h);
		IImage outImg = Images.createImage(inImg, x, y, outBufImg);
		return outImg;
	}
	
	/**
	 * Extracts a subregion from an AWT image.
	 * 
	 * @param inBufImg The source image
	 * @param sub_x
	 * @param sub_y
	 * @param sub_w
	 * @param sub_h
	 * @return The extracted subregion
	 */
	public static BufferedImage extractImage(BufferedImage inBufImg, int sub_x, int sub_y, int sub_w, int sub_h) {
		// TODO do we have to *copy* the data?  determine an adequate mechanism.
		BufferedImage outBufImg;
		if ( true ) {
			// option: copy:
			Rectangle sub_rect = new Rectangle(sub_x, sub_y, sub_w, sub_h);
			WritableRaster raster = (WritableRaster) inBufImg.getData(sub_rect);
			raster = raster.createWritableTranslatedChild(0, 0);
			Hashtable<?, ?> properties = null;
			outBufImg = new BufferedImage(
					inBufImg.getColorModel(),
					raster,
					inBufImg.isAlphaPremultiplied(),
					properties 
			);
		}
		else {
			// option: just subimage
			outBufImg = inBufImg.getSubimage(sub_x, sub_y, sub_w, sub_h);
		}
		return outBufImg;
	}

	/** A null image. This object cannot be modified. isNull() is guaranteed to 
	 * return true. */
	public static final IImage nullImage = new NullImage();  //new BasicImage();
	
	private static class NullImage  implements IImage, Serializable {
		public boolean isNull() { return true; }
		public BufferedImage getBufferedImage() { return null; }
		public String getStreamID() { return null; }
		public String getChannelID() { return null; }
		public int[] getData() { return null; }
		public String getFrameID() { return null; }
		public int getHeight() { return 0; }
		public IServerConnection getServerConnection() { return null; }
		public int getWidth() { return 0; }
		public int getX() { return 0; }
		public int getY() { return 0; }
		public IImage clone() { return this; }
		public void setServerConnection(IServerConnection serverConnection) {
			throw new UnsupportedOperationException();
		}
		public void setStreamID(String streamID) {
			throw new UnsupportedOperationException();
		}
		public IStream getStream() { return null; }
		public void setStream(IStream istream) {
			throw new UnsupportedOperationException();
		}
		public void setFrameID(String frameID) {
			throw new UnsupportedOperationException();
		}
	}

	public static BufferedImage createBufferedImage(int w, int h) {
		BufferedImage outBufImg = new BufferedImage(w, h, BufferedImage.TYPE_4BYTE_ABGR);
		return outBufImg;
	}
	
	/**
	 *  Creates a new IImage.
	 * 
	 * @param streamID
	 * @param channelID
	 * @param x X location
	 * @param y Y location
	 * @param bufImg Data for the image
	 * @return the new object.
	 * @throws IllegalArgumentException If bufImg argument is null
	 */
	public static IImage createImage(String streamID, String channelID, int x, int y, BufferedImage bufImg) {
		if ( bufImg == null ) {
			throw new IllegalArgumentException("bufImg argument must be non-null");
		}
		BasicImage nimg = new BasicImage(streamID, channelID); 
		nimg.frameID = "?";
		nimg.x = x;
		nimg.y = y;
		nimg.width = bufImg.getWidth();
		nimg.height = bufImg.getHeight();
		nimg.data = null;
		nimg.buff_img = bufImg;
		return nimg;
	}
	
	/**
	 * Creates a new IImage.
	 * 
	 * @param streamID
	 * @param channelID
	 * @param x X location
	 * @param y Y location
	 * @param w
	 * @param h
	 * @return
	 */
	public static IImage createImage(String streamID, String channelID, int x, int y, int w, int h) {
		BufferedImage bufImg = createBufferedImage(w, h);
		return createImage(streamID, channelID, x, y, bufImg);
	}

	/**
	 * Extracts a pixel from an IImage.
	 * Pixel coordinate given in reference space coordinates and units.
	 * @param inImg The source image
	 * @param x
	 * @param y
	 * @return The pixel value
	 */
	public static int extractPixel(IImage inImg, int x, int y) {
		ChannelDef channelDef = null;
		IStream istream = inImg.getStream();
		if ( istream != null ) {
			channelDef = istream.getChannelDef();
		}
		else {
			// FIXME 0 : istream  This will dissapear
			IServerConnection gscon = inImg.getServerConnection();
			if ( gscon != null ) {
				StreamDef streamDef = gscon.getStreamDef(inImg.getStreamID());
				channelDef = streamDef.getChannelDef();
			}
		}
		assert channelDef != null || Utils.fail("Image.extractPixel: channelDef not found for streamID: " +inImg.getStreamID());
		
		int psx = channelDef.getPixelSizeX();
		int psy = channelDef.getPixelSizeY();
		int sub_x = (x - inImg.getX()) / psx;
		int sub_y = (y - inImg.getY()) / psy;
		
		assert sub_y == 0 || Utils.fail("Expect sub_y to be zero:  sub_y=" +sub_y+ "  sub_x=" +sub_x);
		
		int value = 0;
		
		int[] data = ((BasicImage) inImg).data;
		if ( data != null ) {
			value = data[sub_x];
		}
		else {
			BufferedImage inBufImg = inImg.getBufferedImage();
			int[] array = new int[1];
			inBufImg.getRaster().getPixel(sub_x, sub_y, array);
			value = array[0];
		}
		
		return value;
	}
}
