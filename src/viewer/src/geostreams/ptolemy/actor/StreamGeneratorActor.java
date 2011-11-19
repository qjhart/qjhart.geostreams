package geostreams.ptolemy.actor;

import java.awt.Rectangle;

import geostreams.image.IStream;
import geostreams.image.IStreamGenerator;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO : 0 istream
 * 
 * Base class for IStreamGenerator actors.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamGeneratorActor.java,v 1.1 2007/06/04 23:42:30 crueda Exp $
 */
public abstract class StreamGeneratorActor extends TypedAtomicActor
implements IStreamGenerator {

	/** Output port. */
    public TypedIOPort output;

    /** ROI parameter. Created when _createRoiParameter is called. */
	public StringParameter roiParam;

	protected Rectangle _roi;
	

	protected String _streamID;
	protected IStream _istream;  
	
	protected StreamGeneratorActor(CompositeEntity container, String name)
	throws NameDuplicationException, IllegalActionException {
		super(container, name);

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(BaseType.OBJECT);
		output.setMultiport(true);
	}
	
	protected void _createRoiParameter() throws IllegalActionException, NameDuplicationException {
		if ( roiParam == null ) {
			roiParam = new StringParameter(this, "roiParam");
			roiParam.setExpression("9500 6000 22000 8500");
			roiParam.setDisplayName("Region of interest");
		}
	}

	/**
	 * If _createOutputPort() has been called, calls output.setName(_streamID).
	 * If _createRoiParameter() has been called, it obtains _roi.
	 * istream is assigned null.
	 */
	public void initialize() throws IllegalActionException {
		super.initialize();
		_istream = null;
		if ( roiParam != null ) {
	        try {
	        	String[] toks = roiParam.stringValue().split("\\s+");
	        	int[] vals = new int[toks.length];
	        	for (int i = 0; i < vals.length; i++) {
					vals[i] = Integer.parseInt(toks[i]);
				}
	        	int roi_x = vals[0];
	        	int roi_y = vals[1];
	        	int roi_w = vals[2] - roi_x + 1;
	        	int roi_h = vals[3] - roi_y + 1;
	        	_roi = new Rectangle(roi_x, roi_y, roi_w, roi_h);
	        }
	        catch (Exception ex) {
	        	throw new IllegalActionException("Invalid ROI spec: " +ex.getMessage());
			}
		}
	}

	
	public IStream getGeneratedStream() {
		return _istream;
	}

	/**
	 * Should be called by a subclass (eg, in its fire() method) when the generated 
	 * _istream has been obtained; sub-subclasses can use this as a notification.
	 * Nothing is done in this base class.
	 */
	protected void _generatedStreamObtained() {
		// nothing.
	}
	
	public void wrapup() throws IllegalActionException {
    	super.wrapup();
    	_istream = null;
    	_roi = null;
    }
}
