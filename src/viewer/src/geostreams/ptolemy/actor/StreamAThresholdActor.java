package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.image.BufferedImage;

import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.Attribute;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Threshold operator.
 * Applies thresholding to the input images, which are assumed to
 * be gray-level. A fully transparent pixel is left unchanged.
 * Otherwise, pixel values greater than the given threshold are
 * given the output value specified in a parameter, or are just retained
 * if this value is not specified. Pixel with values
 * below the threshold are given the value 0.
 * 
 * <p>
 * NOTE: This framework assumes in general that images a gray-level; so,
 * normally you will specify a decimal number for the output value; 
 * however, specifying a colored output would be useful for connecting the
 * output port of this operator with a visualization actor.
 * This functionality may change for a more complete color-mapping scheme
 * in the framework.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamAThresholdActor.java,v 1.5 2007/09/11 09:34:56 crueda Exp $
 */
public class StreamAThresholdActor extends StreamUnaryOperatorActor {
	/** the Threshold parameter */
	public StringParameter thresholdParam;
	private int threshold;
	
	/** Output value if input value is &gt;= threshold. Blank to apply semi-thresholding  */
	public StringParameter outputValueParam;
	private int outputValue;
	private boolean semiThresholding;

	
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
	public StreamAThresholdActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "THR");

		thresholdParam = new StringParameter(this, "thresholdParam");
		thresholdParam.setExpression("140");
		thresholdParam.setDisplayName("Threshold");
		
        outputValueParam = new StringParameter(this, "outputValueParam");
        outputValueParam.setExpression("0xff0000ff");
        outputValueParam.setDisplayName("Output color (blank to apply semi-thresholding)");
	}
	
	private void _updateThreshold(String thresholdString) {
		threshold = Integer.parseInt(thresholdString);
	}
	
	private void _updateOutputValue(String outputValueString) {
		outputValue = 0xffffffff;
		if ( outputValueString.length() > 0 ) {
			if ( outputValueString.toLowerCase().startsWith("0x") ) {
				// full color spec:
				outputValue = (int) (0xffffffff & Long.parseLong(outputValueString.substring(2), 16));
			}
			else {
				// opaque gray-level color:
				int val = 0xff & Integer.parseInt(outputValueString);
				outputValue = (255 << 24) | (val << 16) | (val << 8) | val;	
			}
		}
		semiThresholding = outputValueString.length() == 0 ;
	}
	
	/** {@inheritDoc} */
	public void initialize() throws IllegalActionException {
		super.initialize();
		_updateThreshold(thresholdParam.stringValue().trim());
		_updateOutputValue(outputValueParam.stringValue().trim());
	}
	
    public void attributeChanged(Attribute attribute)
            throws IllegalActionException {
        if (attribute == thresholdParam) {
        	_updateThreshold(thresholdParam.stringValue().trim());
        }
        else if (attribute == outputValueParam) {
        	_updateOutputValue(outputValueParam.stringValue().trim());
        }
        super.attributeChanged(attribute);
    }

	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		IImage outImg = null;

		BufferedImage inBufImg = inImg1.getBufferedImage();
		int w = inBufImg.getWidth();
		int h = inBufImg.getHeight();
		
		// get a copy of the buffered image:
		BufferedImage outBufImg = Images.extractImage(inBufImg, 0, 0, w, h);
		
		// apply thresholding to each pixel
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
				val = aval & 0xffffff; 
				if ( val < threshold ) {
					aval = 0;
				}
				else if ( semiThresholding ) {
					// OK, just leave the current values.
					nonNullPixels++;
				}
				else {
					// Set user-defined value
					aval = outputValue;
					nonNullPixels++;
				}
			}
			pixels[i] = aval;
		}
		
		if ( nonNullPixels > 0 ) {
			outBufImg.setRGB(0, 0, w, h, pixels, offset, scansize);
			outImg = Images.createImage(inImg1, inImg1.getX(), inImg1.getY(), outBufImg);
		}
		
		return outImg;
	}
}
