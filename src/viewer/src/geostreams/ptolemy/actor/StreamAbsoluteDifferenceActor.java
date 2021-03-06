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
 * Absolute difference operator.
 * 
 * FIXME: should on the AVAL format
 * 
 * The operation is applied on each of the RGB components,
 * eg., R = abs(R1 - R2).
 * For the alpha channel the result is A = min(A1, A2).
 * 
 * NOTE: This is an "integrated (Diff -&gt; Abs)" operator as we 
 * still have to handle images with negative values.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamAbsoluteDifferenceActor.java,v 1.1 2007/06/06 09:33:01 crueda Exp $
 */
public class StreamAbsoluteDifferenceActor extends StreamBinaryOperatorActor {
	
	private int _psx;
	private int _psy;
	
	/** {@inheritDoc} */
	public StreamAbsoluteDifferenceActor(CompositeEntity container, String name)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, "AbsDiff");
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
		int offset = 0;
		int scansize = w;
		inBufImg1.getRGB(0, 0, w, h, pixels1, offset, scansize);
		inBufImg2.getRGB(0, 0, w, h, pixels2, offset, scansize);

		//
		// NOTE: inBufImg1 is used to store the result.
		//
		
		for (int i = 0; i < pixels1.length; i++) {
			int val11 = pixels1[i] & 0xff; 
			int val12 = (pixels1[i] >> 8) & 0xff; 
			int val13 = (pixels1[i] >> 16) & 0xff;

			int val21 = pixels2[i] & 0xff; 
			int val22 = (pixels2[i] >> 8) & 0xff; 
			int val23 = (pixels2[i] >> 16) & 0xff;
			
			int valRes1 = Math.abs(val11 - val21);
			int valRes2 = Math.abs(val12 - val22);
			int valRes3 = Math.abs(val13 - val23);
			
			int valRes4 = 255;
			
			// <transparency>
			if ( true ) {
				int val14 = (pixels1[i] >>> 24) & 0xff;
				int val24 = (pixels2[i] >>> 24) & 0xff;
				valRes4 = Math.min(val14, val24);
			}
			// </transparency>
			
			pixels1[i] = (valRes4 << 24) | (valRes3 << 16) | (valRes2 << 8) | valRes1;
		}
		
		inBufImg1.setRGB(0, 0, w, h, pixels1, offset, scansize);
		outImg = Images.createImage(inImg1, inters.x, inters.y, inBufImg1);

		return outImg;
	}
}