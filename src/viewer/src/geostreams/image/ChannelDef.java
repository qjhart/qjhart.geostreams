package geostreams.image;

import java.io.Serializable;


/**
  * Describes the geometry of a particular channel in a stream. This
  * geometry is given by the size of the pixel in this channel. This
  * size is measured in terms of the pixel resolution of the reference
  * space associated to the stream.
  * Also the sample size (number of bits per sample) is associated to 
  * a channel.
  * 
  * NOTE: Although multiple channels can be associated to a GVAR "stream" 
  * via Jie's module, the general model in this framework, however, is 
  * that a "stream" has ony ONE ChannelDef (geometry). 
  *
  * @author Carlos Rueda-Velasquez
  * @version $Id: ChannelDef.java,v 1.3 2007/06/02 20:00:49 crueda Exp $
  */
public class ChannelDef implements Serializable {
	/** Short name of this channel. */
	public String getName() { return name; }
	
	/** ID of this channel definition. */
	public String getID() { return ID; }
	
	/** Size of the pixel in the channel in the x direction
	  * in terms of number of pixels in the reference space. */
	public int getPixelSizeX() { return pixelSizeX; }
	
	/** Size of the pixel in the channel in the y direction
	  * in terms of number of pixels in the reference space. */
	public int getPixelSizeY() { return pixelSizeY; }

	/** The aspect ratio for the channel. */
	public double getAspect() { return aspect; }

	/** Gets the sample size in bits. Normally this will be
	  * 10 bits when the image source provides the original
	  * data without any processing. However, there are cases
	  * in which the image already has overgone some sort
	  * of processing and then the sample size could have been
	  * reduced (e.g., MngImageStream produces 8-bit images).
	  */
	public int getSampleSize() { return sampleSize; };
	
	public String toString() {
		return name;
	}
	
	public String getDescriptiveString() {
		return "ID=" +ID
		     + " sx=" +pixelSizeX
		     + " sy=" +pixelSizeY
		     + " aspect=" +aspect
		     + " sampleSize=" +sampleSize
		     + " name=" +name
		;
	}

	/** A short name */
	String name;
	
	/** ID of this channel definition */
	String ID;
	
	/** Size of the pixel in the x direction
	  * in terms of number of pixels in the reference space. */
	int pixelSizeX;
	
	/** Size of the pixel in the y direction
	  * in terms of number of pixels in the reference space. */
	int pixelSizeY;

	/** The aspect ratio  */
	double aspect;
	
	/** Number of bits per sample. */
	int sampleSize;

	/** Creates a channel definition with given attributes */
	public ChannelDef(String ID, String name, int pixelSizeX, int pixelSizeY,
		double aspect, int sampleSize) 
	{
		this.ID = ID;
		this.name = name;
		this.pixelSizeX = pixelSizeX;
		this.pixelSizeY = pixelSizeY;
		this.aspect = aspect;
		this.sampleSize = sampleSize;
	}
		
}

