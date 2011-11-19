package geostreams.ptolemy.actor;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.Images;

import java.awt.Rectangle;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * RegionOfInterest operator.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: RegionOfInterestActor.java,v 1.12 2007/06/01 21:40:01 crueda Exp $
 */
public class RegionOfInterestActor extends UnaryOperatorActor {
	private static final String[] ROIS = {
		"12000 5000 20000 11000",
		"9500 6000 22000 8500",
	};

	/** {@inheritDoc} */
	public RegionOfInterestActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "ROI");
		_createRoiParameter();
		roiParam.setExpression(ROIS[0]);
		for (int i = 0; i < ROIS.length; i++) {
			roiParam.addChoice(ROIS[i]);
		}
	}
	
	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		IImage outImg = null;
		
		ChannelDef channelDef = gscon.getChannelDefForStream(inImg1.getStreamID());
		int psx = channelDef.getPixelSizeX();
		int psy = channelDef.getPixelSizeY();

		// intersect the ROI with the bounds of the input image:
		// note that dimension of inBounds needs to be in units of reference space coordinates
		Rectangle inBounds = new Rectangle(inImg1.getX(), inImg1.getY(), psx * inImg1.getWidth(), psy * inImg1.getHeight());
		Rectangle inters = _roi.intersection(inBounds);
		if ( inters.isEmpty() ) {
			outImg = Images.nullImage;
		}
		else {
			outImg = Images.extractImage(inImg1, inters);
		}
		return outImg;
	}
}
