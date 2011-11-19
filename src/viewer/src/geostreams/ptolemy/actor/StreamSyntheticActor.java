package geostreams.ptolemy.actor;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.Images;
import geostreams.image.ReferenceSpace;
import geostreams.ptolemy.icon.OperatorIcon;

import java.awt.Color;
import java.awt.Font;
import java.awt.image.BufferedImage;

import ptolemy.data.ObjectToken;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Generates a stream of synthetic 1-row images.
 * The images will incrementally cover the ROI indicated in the ROI 
 * parameter which is always included.
 * 
 * <p>
 * NOTE: a single shared BUfferedImage is used by all created IImage objects.
 *
 * <p>
 * NOTE: simple functionality mainly for demo purposes.
 *
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamSyntheticActor.java,v 1.3 2007/06/23 18:38:29 crueda Exp $
 */
public abstract class StreamSyntheticActor extends StreamGeneratorActor {
	// TODO fixed referenceSpace
	// <ReferenceSpace x="0" y="0" width="30676" height="15769"/>
	protected final ReferenceSpace referenceSpace = new ReferenceSpace(0, 0, 30676, 15769);
	

	protected OperatorIcon _icon;
	
	/** the image to be output */
	protected IImage _outImg;
	protected int[] _pixels;
	
	
	/** the shared buffered image */
	private BufferedImage outBufImg;
	
	/** row locations for the next generated image */
	private int yOffset;

	private ChannelDef channelDef;

	/** Always creates the ROI parameter */
	public StreamSyntheticActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name);

		_icon = new OperatorIcon(this, "_icon", "?");
		_icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		_icon.setFont(new Font("SansSerif", Font.PLAIN, 10));

		_createRoiParameter();
	}

	
	/** Uses the current value of _streamID.
	 * The pixels array is allocated
	 */
	public void initialize() throws IllegalActionException {
		super.initialize();
		
		yOffset = 0;
		
		// create the buffered image:
		outBufImg = Images.createBufferedImage(_roi.width, 1);
		_pixels = new int[_roi.width * 1];
		
		// TODO fixed channelDef
		channelDef = new ChannelDef(_streamID, _streamID, 1, 1, 0.888889, 10);
		
		_istream = Images.createStream(_streamID, referenceSpace, channelDef);
		_generatedStreamObtained();

//		TODO port-name - remove this -- we don't change it
//		try {
//			output.setName(_streamID);
//		}
//		catch (NameDuplicationException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
	}
	
	/** Called for a subclass to fill in the already allocated pixels array;
	 * Nothing is done here.
	 */
	protected void _prepareNextImage() throws IllegalActionException {
		// nothing.
	}
	
	
	/** creates the next image to be output */
	private IImage _nextImage() throws IllegalActionException {
		_outImg = Images.createImage(_streamID, channelDef.getID(), _roi.x, _roi.y + yOffset, outBufImg);
		
		_prepareNextImage();

		final int offset = 0;
		final int scansize = _roi.width;
		outBufImg.setRGB(0, 0, _roi.width, 1, _pixels, offset, scansize);

		yOffset = (yOffset + 1) % _roi.height;
		
		_outImg.setStream(_istream);
		
		return _outImg;
	}
	
	
	/** 
	  * Obtains next image and broadcasts it.
	  */
	public void fire() throws IllegalActionException {
		IImage img = _nextImage();
		if ( img != null ) {
			super.fire();
			output.broadcast(new ObjectToken(img));
		}
	}

	public void wrapup() throws IllegalActionException {
    	super.wrapup();
    	_outImg = null;
    	_pixels = null;
    	outBufImg = null;
    	channelDef = null;
    }

}
