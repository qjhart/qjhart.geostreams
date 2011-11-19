package geostreams.ptolemy.actor;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.StreamDef;
import geostreams.ptolemy.IStreamActor;
import geostreams.ptolemy.icon.OperatorIcon;
import geostreams.util.Utils;

import java.awt.Rectangle;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import ptolemy.actor.IOPort;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.expr.StringParameter;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Convenience base class with common functionality for stream actors.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: AbstractStreamActor.java,v 1.9 2007/06/02 20:00:49 crueda Exp $
 */
public abstract class AbstractStreamActor extends TypedAtomicActor
implements IStreamActor {

	/** Input port. Created when _createInputPort() is called */
	public TypedIOPort input;
	
	/** Output port. Created when _createOutputPort() is called */
    public TypedIOPort output;

    
    /** ROI parameter. Created when _createRoiParameter is called. */
	public StringParameter roiParam;

	protected Rectangle _roi;
	
	protected Map<String, Object> props;
	
	/** FIXME gscon-integration: The server connection of the input and the output?? */  
	protected IServerConnection gscon;
	
	protected List<StreamDef> streamDefs = null;
	
	/** Name of the operator, used in the icon */
	protected String _opName;
	protected OperatorIcon _icon;
	
	/** This is used to name the output port */ 
	protected String _streamID;
	
	protected static void _printRect(String s, Rectangle r) {
		System.out.println(s+ r.x+ " " +r.y+ " " + r.width+ " " +r.height);
	}
	
	/**
	 * 
	 * @param container
	 * @param name
	 * @param opName _opName and _streamID are given the value of this parameter.
	 * @throws NameDuplicationException
	 * @throws IllegalActionException
	 */
	protected AbstractStreamActor(CompositeEntity container, String name, String opName)
	throws NameDuplicationException, IllegalActionException {
		super(container, name);
		_opName = _streamID = opName;
		
		// TODO: remove
		props = new HashMap<String,Object>();
		if ( opName != null ) {
			_icon = new OperatorIcon(this, "_icon", _opName);
		}
	}
	
	protected void _createRoiParameter() throws IllegalActionException, NameDuplicationException {
		if ( roiParam == null ) {
			roiParam = new StringParameter(this, "roiParam");
			roiParam.setExpression("9500 6000 22000 8500");
			roiParam.setDisplayName("Region of interest");
		}
	}
	
	protected void _createInputPort(boolean multiPort) throws IllegalActionException, NameDuplicationException {
		if ( input == null ) {
			input = new TypedIOPort(this, "input", true, false);
			input.setTypeEquals(BaseType.OBJECT);
			input.setMultiport(multiPort);
		}
	}
	
	protected void _createOutputPort(boolean multiPort) throws IllegalActionException, NameDuplicationException {
		if ( output == null ) {
			output = new TypedIOPort(this, "output", false, true);
			output.setTypeEquals(BaseType.OBJECT);
			output.setMultiport(multiPort);
		}
	}
	
	/**
	 * If _createOutputPort() has been called, calls output.setName(_streamID).
	 * If _createRoiParameter() has been called, it obtains _roi.
	 */
	public void initialize() throws IllegalActionException {
		super.initialize();
		if ( output != null ) {
			try {
				output.setName(_streamID);
			}
			catch (NameDuplicationException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
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
	        	_printRect(getClass().getSimpleName()+ ": ROI = ", _roi);
	        }
	        catch (Exception ex) {
	        	throw new IllegalActionException("Invalid ROI spec: " +ex.getMessage());
			}
		}
	}

	protected void _updateConnection(IImage img) {
		IServerConnection sc = img.getServerConnection();
		assert sc != null;
		gscon = sc;
		getStreamDefs();
	}

	public List<StreamDef> getStreamDefs() {
		assert gscon != null || Utils.fail("_updateConnection NOT YET CALLED");
		
		if ( streamDefs == null && gscon != null ) {
			streamDefs = new ArrayList<StreamDef>();
			for (IOPort outputPort : new IOPort[] { output} ) {
				IOPort output = outputPort;
				String outName = output.getName();

				String streamID = outName;
				StreamDef streamDef = gscon.getStreamDef(streamID);
				
				Utils.log("iiiiiiiiii   AbstractStreamActor: streamDef(" +streamID+ ") -> " +streamDef);
				
				
				if ( streamDef == null ) {
					// FIXME gscon-integration: TEMPORARY TRICK  -- see below also
					Utils.log("iiiiiiiiii   AbstractStreamActor: streamID" +streamID+ " not in gscon.  ADDING");
					
					streamDef = new StreamDef(streamID,
							new ChannelDef(streamID, streamID, 4, 8, 0.888889, 10)
					);
					gscon.addStreamDef(streamID, streamDef);
				}
				
				assert streamDef != null ;

				streamDefs.add(streamDef);
			}
		}
		return streamDefs;
	}
	
	

	public IServerConnection getServerConnection() {
		return gscon;
	}
	
	public void wrapup() throws IllegalActionException {
    	super.wrapup();
		gscon = null;
		streamDefs = null;
    }


}