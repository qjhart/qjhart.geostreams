package geostreams.ptolemy.actor;

import ptolemy.data.expr.StringParameter;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Generates a stream with constant value images.
 * The images will incrementally cover the ROI indicated in the ROI 
 * parameter.
 * 
 * <p>
 * The constant pixel value can be given in decimal (eg, 170) or hexidecimal, eg., 0xffff0000.
 * If decimal, then the value is taken for all RGB components with an
 * alpha of 0xff (opaque); if hexadecimal, it is read as the full ARGB
 * composition of the color including the alpha channel.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamConstantActor.java,v 1.1 2007/06/04 23:42:30 crueda Exp $
 */
public class StreamConstantActor extends StreamSyntheticActor {

	private final String _defaultConstant = "0x20ff0000";
	
	/** the constant parameter */
	public StringParameter constantParam;
	
	private int constant;
	
	
	public StreamConstantActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name);

		_icon.setText("Const");

		_updateConstant(_defaultConstant);
		
		constantParam = new StringParameter(this, "constantParam");
		constantParam.setExpression(_defaultConstant);
		constantParam.setDisplayName("Constant");
	}

	private void _updateConstant(String tok) {
		try {
			if ( tok.toLowerCase().startsWith("0x") ) {
				// full color spec:
				constant = (int) (0xffffffff & Long.parseLong(tok.substring(2), 16));
			}
			else {
				// opaque gray-level color:
				int val = 0xff & Integer.parseInt(tok);
				constant = (255 << 24) | (val << 16) | (val << 8) | val;	
			}
			_icon.setText(Integer.toHexString(constant));
		}
		catch(RuntimeException ex) {
			// Ignore.
			return;
		}
	}
	
	/** Sets the constant, the _streamID, and calls super.initialize)(*/
	public void initialize() throws IllegalActionException {
		String consts = constantParam.stringValue().trim();
		_updateConstant(consts);
		_streamID = "k_" +consts;

		// after we got out stream_ID:
		super.initialize();
		
		// we fill in pixels only once:
		for (int i = 0; i < _pixels.length; i++) {
			_pixels[i] = constant;				
		}
	}

}
