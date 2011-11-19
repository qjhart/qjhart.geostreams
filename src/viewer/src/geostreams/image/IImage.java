package geostreams.image;

import java.awt.image.BufferedImage;

/**
  * Interface of images coming in a stream.
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: IImage.java,v 1.9 2007/06/13 00:08:43 crueda Exp $
  */
public interface IImage {
	
	/** gets the starting x location. */
	public int getX();

	/** gets the starting y location. */
	public int getY();
	
	/** gets the width in pixels of this image. */
	public int getWidth();
	
	/** gets the height in pixels of this image. */
	public int getHeight();
	
	/** is this image null?  */
	public boolean isNull();

	/** Creates a shallow copy of this image. */
	public IImage clone();

	/** Returns the stream that produces this image. */
	public IStream getStream();
	
	/** Sets the stream this image is associated to. */
	public void setStream(IStream istream);
	
	
	/** gets the ID of the frame this image belongs to. */
	public String getFrameID();


	/** TODO quick hack
	 * sets the ID of the frame this image belongs to. */
	public void setFrameID(String frameID);


	
	
	// TODO To be removed -- use getStream().getID()
	public String getStreamID(); 	
	
	// TODO To be removed
	public void setStreamID(String streamID);
	
	
	// TODO To be removed -- use getStream().getChannelDef().getID()
	public String getChannelID();


	
	///////////////// TODO To be eventually removed ///////////////////////
	public BufferedImage getBufferedImage();
	

	
	
	
	///////////////// TODO To be eventually removed ///////////////////////
	public void setServerConnection(IServerConnection serverConnection);
	public IServerConnection getServerConnection();
	
}

