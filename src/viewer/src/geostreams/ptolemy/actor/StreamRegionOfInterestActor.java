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
 * @version $Id: StreamRegionOfInterestActor.java,v 1.3 2007/09/11 09:34:56 crueda Exp $
 */
public class StreamRegionOfInterestActor extends StreamUnaryOperatorActor {
	private static final String[] ROIS = {
		"12000 5000 20000 11000",
		"9500 6000 22000 8500",
	};

	/** {@inheritDoc} */
	public StreamRegionOfInterestActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "SR");
		_createRoiParameter();
		roiParam.setExpression(ROIS[0]);
		for (int i = 0; i < ROIS.length; i++) {
			roiParam.addChoice(ROIS[i]);
		}
	}
	
	protected IImage _operate(IImage inImg1) throws IllegalActionException {
		IImage outImg = null;
		
		ChannelDef channelDef = _istream.getChannelDef();
		int psx = channelDef.getPixelSizeX();
		int psy = channelDef.getPixelSizeY();

		// intersect the ROI with the bounds of the input image:
		// note that dimension of inBounds needs to be in units of reference space coordinates
		Rectangle inBounds = new Rectangle(inImg1.getX(), inImg1.getY(), psx * inImg1.getWidth(), psy * inImg1.getHeight());
		Rectangle inters = _roi.intersection(inBounds);
		if ( !inters.isEmpty() ) {
			outImg = Images.extractImage(inImg1, inters);
		}
		return outImg;
	}
}
