package geostreams.image;

import java.io.IOException;

/**
 * Interface to access the images in a stream.
 * 
 * @author Carlos Rueda
 * @version $Id: IImageStream.java,v 1.3 2007/06/01 21:40:01 crueda Exp $
 * @navassoc - gets - IImage
 */
interface IImageStream {

	/**
	 * Certain events associated to the stream are notified via this
	 * interface.
	 */
	public interface IObserver {
		/**
		 * Called when a new frame starts.
		 * 
		 * @param streamID ID of corresponding stream.
		 * @param frameDef Feame definition
		 */
		public void notifyFrameStarts(String streamID, FrameDef frameDef);
	}

	/**
	 * Sets the observer to this stream.
	 * @param obs
	 */
	public void setImageStreamObserver(IObserver obs);
	
	/** 
	 * Call this before getting data from this stream. 
	 */
	public void startReading() throws IOException;

	/** 
	 * Reads the next image from the stream and returns it.
	 * 
	 * @return the next image from the stream; If there are no more images from the 
	 *         stream, then a null-image is returned (ie., isNull() on the returned 
	 *         object will return true). However, this will happen only once when
	 *         the input stream is exhausted; subsequent calls of this method will return null.  
	 */
	public IImage nextImage() throws IOException;

	/**
	 * @return
	 */
	public String getStreamID();
	
	/**
	 * The ID of the channel of the images in this stream.
	 * @return The ID of the channel of the images in this stream.
	 */
	public String getChannelID();

	public ReferenceSpace getReferenceSpace();

	/** Closes this stream. */
	public void close() throws IOException;

}