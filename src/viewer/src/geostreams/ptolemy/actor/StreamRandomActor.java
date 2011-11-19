package geostreams.ptolemy.actor;

import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Generates a stream with random value images.
 * The images will incrementally cover the ROI indicated in the ROI 
 * parameter.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamRandomActor.java,v 1.1 2007/06/04 23:42:30 crueda Exp $
 */
public class StreamRandomActor extends StreamSyntheticActor {
	
	public StreamRandomActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name);

		_icon.setText("Random");
		_streamID = "random";
	}

	protected void _prepareNextImage() throws IllegalActionException {
		for (int i = 0; i < _pixels.length; i++) {
			int val = (int) ( 255 * Math.random());
			_pixels[i] = (255 << 24) | (val << 16) | (val << 8) | val;	
		}
	}

}
