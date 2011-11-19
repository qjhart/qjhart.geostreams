package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.Font;
import java.awt.image.BufferedImage;

import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.Attribute;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Map-constant color operator.
 * It outputs the same input image but with all pixel values
 * changed to a constant color corresponding to one of the
 * values in a given list of colors. A new color is used
 * when a frame starts coming to the input stream. 
 * 
 * <p>
 * Each color can be given in decimal (eg, 170) or hexidecimal, eg., 0xffff0000.
 * If decimal, then the value is taken for all RGB components with an
 * alpha of 0xff (opaque); if hexadecimal, it is read as the full ARGB
 * composition of the color including the alpha channel.
 * 
 *
 * @author Carlos Rueda-Velasquez
 * @version $Id: MapConstantActor.java,v 1.5 2007/06/03 06:29:27 crueda Exp $
 */
public class MapConstantActor extends UnaryOperatorActor {
	private final String _defaultConstants = "0x20ff0000 0x2000ff00 0x200000ff  170";
	
	/** the constant parameter */
	public StringParameter constantsParam;
	
	private int[] constants;
	private int currentIndex = 0;
	
	/** to detect change in frame, so to update the color to use */
	private String oldFrameID;
	
	
	public MapConstantActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "");
		
		_updateConstants(_defaultConstants);
		_icon.setText(Integer.toHexString(constants[currentIndex]));
		_icon.setFont(new Font("SansSerif", Font.PLAIN, 10));
		
		constantsParam = new StringParameter(this, "constantsParam");
		constantsParam.setExpression(_defaultConstants);
		constantsParam.setDisplayName("Constants");
	}

	private void _updateConstants(String consts) {
		String[] toks = consts.split("\\s+");
		int[] newConstants = new int[toks.length];
		try {
			for (int i = 0; i < newConstants.length; i++) {
				String tok = toks[i];
				if ( tok.toLowerCase().startsWith("0x") ) {
					// full color spec:
					newConstants[i] = (int) (0xffffffff & Long.parseLong(tok.substring(2), 16));
				}
				else {
					// opaque gray-level color:
					int val = 0xff & Integer.parseInt(tok);
					newConstants[i] = (255 << 24) | (val << 16) | (val << 8) | val;	
				}
			}
		}
		catch(RuntimeException ex) {
			// Ignore.
			return;
		}
		if ( newConstants.length > 0 ) {
			constants = newConstants;
			currentIndex = 0;
			_icon.setText(Integer.toHexString(constants[currentIndex]));
		}
	}
	
	/** {@inheritDoc} */
	public void initialize() throws IllegalActionException {
		super.initialize();
		String consts = constantsParam.stringValue().trim();
		_updateConstants(consts);
		oldFrameID = null;
	}
	
    public void attributeChanged(Attribute attribute) throws IllegalActionException {
        if (attribute == constantsParam) {
    		String consts = constantsParam.stringValue().trim();
    		_updateConstants(consts);

        }
        super.attributeChanged(attribute);
    }

	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		if ( oldFrameID == null ) {
			// first image
			oldFrameID = inImg1.getFrameID();
		}
		else if ( !oldFrameID.equals(inImg1.getFrameID()) ) {
			oldFrameID = inImg1.getFrameID();
			currentIndex = (currentIndex + 1) % constants.length;
			_icon.setText(Integer.toHexString(constants[currentIndex]));
		}
		
		IImage outImg = null;
		
		final int color = constants[currentIndex];

		BufferedImage inBufImg = inImg1.getBufferedImage();
		int w = inBufImg.getWidth();
		int h = inBufImg.getHeight();
		
		// get a copy of the buffered image:
		BufferedImage outBufImg = Images.extractImage(inBufImg, 0, 0, w, h);
		
		// apply constant to each pixel
		int[] pixels = new int[w * h];
		int offset = 0;
		int scansize = w;
		for (int i = 0; i < pixels.length; i++) {
			pixels[i] = color;				
		}
		outBufImg.setRGB(0, 0, w, h, pixels, offset, scansize);
		outImg = Images.createImage(inImg1, inImg1.getX(), inImg1.getY(), outBufImg);
		
		return outImg;
	}
}
