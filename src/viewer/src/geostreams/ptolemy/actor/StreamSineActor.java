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
 * @version $Id: StreamSineActor.java,v 1.1 2007/06/05 00:37:41 crueda Exp $
 */
public class StreamSineActor extends StreamSyntheticActor {
	
	private static final double TWO_PI = 2*Math.PI;
	
	// would be parameters when the time comes :-)
	private final int num_Waves = 3;
	private final double _phaseIncrement = Math.PI / 10000000;

	
	private double _phaseOffset;

	public StreamSineActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name);

		_icon.setText("Sine");
		_streamID = "Sine";
	}
	
	public void initialize() throws IllegalActionException {
		super.initialize();
		_phaseOffset = 0;
	}	

	protected void _prepareNextImage() throws IllegalActionException {
		for (int i = 0; i < _pixels.length; i++) {
			double arg = _phaseOffset + i * ((num_Waves) * TWO_PI) / _pixels.length; 
			int val = 127 + (int) ( 127 * Math.sin(arg) );
			_pixels[i] = (255 << 24) | (val << 16) | (val << 8) | val;	
			_phaseOffset +=_phaseIncrement;
		}
	}

}
