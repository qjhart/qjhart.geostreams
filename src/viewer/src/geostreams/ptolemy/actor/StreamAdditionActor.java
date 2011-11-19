package geostreams.ptolemy.actor;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.Rectangle;
import java.awt.image.BufferedImage;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Addition operator.
 *
 * The operation treats each pixel value in AVAL format.
 *
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamAdditionActor.java,v 1.1 2007/06/06 19:34:19 crueda Exp $
 */
public class StreamAdditionActor extends StreamBinaryOperatorActor {
	
//	TODO: factor out common functionality into StreamBinaryOperatorActor
	
	private int _psx;
	private int _psy;
	
	/** {@inheritDoc} */
	public StreamAdditionActor(CompositeEntity container, String name)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, "+");
	}
	
	protected void _generatedStreamObtained() {
		super._generatedStreamObtained();
		ChannelDef channelDef = _istream.getChannelDef();
		_psx = channelDef.getPixelSizeX();
		_psy = channelDef.getPixelSizeY();
	}

	@Override
	protected IImage _operate(IImage inImg1, IImage inImg2) throws IllegalActionException {
		IImage outImg = null;

		// get area of intersection

		Rectangle inBounds1 = new Rectangle(inImg1.getX(), inImg1.getY(), _psx * inImg1.getWidth(), _psy * inImg1.getHeight());
		Rectangle inBounds2 = new Rectangle(inImg2.getX(), inImg2.getY(), _psx * inImg2.getWidth(), _psy * inImg2.getHeight());
		
		Rectangle inters = inBounds1.intersection(inBounds2);
		
		if ( inters.isEmpty() ) {
//			System.out.print(" <0>"); System.out.flush();
			return outImg;
		}
		
		IImage intersImg1 = Images.extractImage(inImg1, inters);
		IImage intersImg2 = Images.extractImage(inImg2, inters);
		
		BufferedImage inBufImg1 = intersImg1.getBufferedImage();
		BufferedImage inBufImg2 = intersImg2.getBufferedImage();
		
		int w = inBufImg1.getWidth();
		int h = inBufImg1.getHeight();

		int[] pixels1 = new int[w * h];
		int[] pixels2 = new int[w * h];
		final int offset = 0;
		final int scansize = w;
		inBufImg1.getRGB(0, 0, w, h, pixels1, offset, scansize);
		inBufImg2.getRGB(0, 0, w, h, pixels2, offset, scansize);

		//
		// NOTE: inBufImg1 is used to store the result.
		//
		
		for (int i = 0; i < pixels1.length; i++) {
			int aval1 = pixels1[i];
			int aval2 = pixels2[i];
			
			int val1 = aval1 & 0xffffff; 
			int alpha1 = (aval1 >>> 24) & 0xff;

			int val2 = aval2 & 0xffffff; 
			int alpha2 = (aval2 >>> 24) & 0xff;
			
			int val = val1 + val2;
			int alpha = Math.max(alpha1, alpha2);
			
			int aval = (alpha << 24) | (val & 0xffffff);
			
			pixels1[i] = aval;
		}
		
		inBufImg1.setRGB(0, 0, w, h, pixels1, offset, scansize);
		outImg = Images.createImage(inImg1, inters.x, inters.y, inBufImg1);

		return outImg;
	}
}