package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.Color;
import java.awt.Font;
import java.awt.image.BufferedImage;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Map-color operator.
 * Maps a (data) image to a color image. 
 * 
 * <p>
 * NOTE: very preliminary - using for various tests
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamMapColorActor.java,v 1.1 2007/06/09 01:11:14 crueda Exp $
 */
public class StreamMapColorActor extends StreamUnaryOperatorActor {
	
	public StreamMapColorActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "color");
		
		_icon.setFont(new Font("SansSerif", Font.PLAIN, 10));
	}

	
	/** {@inheritDoc} */

	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		IImage outImg = null;
		
		BufferedImage inBufImg = inImg1.getBufferedImage();
		int w = inBufImg.getWidth();
		int h = inBufImg.getHeight();
		
		// get a copy of the buffered image:
		BufferedImage outBufImg = Images.extractImage(inBufImg, 0, 0, w, h);
		
		int[] pixels = new int[w * h];
		final int offset = 0;
		final int scansize = w;
		outBufImg.getRGB(0, 0, w, h, pixels, offset, scansize);
		int nonNullPixels = 0;
		for (int i = 0; i < pixels.length; i++) {
			int aval = pixels[i];
			int val;
			int alpha = (aval >>> 24) & 0xff;
			
			if ( alpha == 0 ) {   
				aval = 0;
			}
			else {
				nonNullPixels++;
				val = aval & 0xffffff;
				val = _getRGB(val);
				aval = (alpha << 24) | (val & 0xffffff);
			}
			pixels[i] = aval;
		}
		if ( true || nonNullPixels > 0 ) {
			outBufImg.setRGB(0, 0, w, h, pixels, offset, scansize);
			outImg = Images.createImage(inImg1, inImg1.getX(), inImg1.getY(), outBufImg);
		}
		
		return outImg;
	}


	private int _getRGB(int val) {
		Color color = Color.cyan;
		
		// TODO Auto-generated method stub
		return color.getRGB();
	}
}
