package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.image.BufferedImage;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Absolute value operator.
 * The operation treats each pixel value in AVAL format.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamAbsoluteValueActor.java,v 1.3 2007/09/11 09:34:56 crueda Exp $
 */
public class StreamAbsoluteValueActor extends StreamUnaryOperatorActor {
	
	/**
	 * Construct an actor with the given container and name.
	 * 
	 * @param container
	 *            The container.
	 * @param name
	 *            The name of this actor.
	 * @exception IllegalActionException
	 *                If the actor cannot be contained by the proposed
	 *                container.
	 * @exception NameDuplicationException
	 *                If the container already has an actor with this name.
	 */
	public StreamAbsoluteValueActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "ABS");
	}
	
	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		IImage outImg = null;

		BufferedImage inBufImg = inImg1.getBufferedImage();
		int w = inBufImg.getWidth();
		int h = inBufImg.getHeight();
		
		// get a copy of the buffered image:
		BufferedImage outBufImg = Images.extractImage(inBufImg, 0, 0, w, h);
		
		// apply operator to each pixel
		int[] pixels = new int[w * h];
		int offset = 0;
		int scansize = w;
		outBufImg.getRGB(0, 0, w, h, pixels, offset, scansize);
		for (int i = 0; i < pixels.length; i++) {
			int aval = pixels[i];
			int val = aval & 0xffffff; 
			int alpha = (aval >>> 24) & 0xff;
			
			val = Math.abs(pixels[i]);
			aval = (alpha << 24) | (val & 0xffffff);
			
			pixels[i] = aval;
		}
		outBufImg.setRGB(0, 0, w, h, pixels, offset, scansize);
		outImg = Images.createImage(inImg1, inImg1.getX(), inImg1.getY(), outBufImg);

		return outImg;
	}
}
