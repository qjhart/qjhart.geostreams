package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.IServerConnection;
import geostreams.image.Images;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ObjectToken;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Base class for binary operators.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: BinaryOperatorActor.java,v 1.5 2007/06/01 21:40:01 crueda Exp $
 */
public abstract class BinaryOperatorActor extends AbstractStreamActor {
	/** FIXME gscon-integration: The server connection of input2?? */  
	protected IServerConnection gscon2;
	
	/** Input port 2.*/
	public TypedIOPort input2;
	
	/** {@inheritDoc} */
	protected BinaryOperatorActor(CompositeEntity container, String name, String opName)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, opName);
		_createInputPort(false);
		_createOutputPort(true);
		input2 = new TypedIOPort(this, "input2", true, false);
		input2.setTypeEquals(BaseType.OBJECT);
	}
	
	/**
	 * The actual operation occurs here.
	 * 
	 * @param inImg1 Operand; PRE: !inImg1.isNull().
	 * @param inImg2 Operand; PRE: !inImg2.isNull().
	 * @return The resulting image; can be null, in which case, fire()
	 * takes care of generating the nullImage.
	 */
	protected abstract IImage _operate(IImage inImg1, IImage inImg2) throws IllegalActionException;
	
	protected void _updateConnection2(IImage img) {
		IServerConnection sc = img.getServerConnection();
		assert sc != null;
		gscon2 = sc;
//		getStreamDefs();  FIXME gscon-integration
	}

	/**
	 * Gets the input images and performs a binary operation on them.
	 * If any of the input images isNull, then the nullImage is output;
	 * otherwise it calls _operate(img1, img2). If _operate returns null,
	 * then the nullImage is output; 
	 * otherwise the returned image outImg is given the current 
	 * value of _streamID, ie. outImg.setStreamID(_streamID),
	 * and broadcasted.
	 */
	public void fire() throws IllegalActionException {
		if ( input.getWidth() == 0 || !input.hasToken(0) ) {
			return;
		}
		IImage inImg1 = (IImage) ((ObjectToken) input.get(0)).getValue();

		if ( input2.getWidth() == 0 || !input2.hasToken(0) ) {
			return;
		}
		IImage inImg2 = (IImage) ((ObjectToken) input2.get(0)).getValue();

		IImage outImg = null;

		if ( !inImg1.isNull() && !inImg2.isNull() ) {
			_updateConnection(inImg1);
			_updateConnection2(inImg2);

			if ( inImg1.getChannelID() != inImg2.getChannelID() ) {
				throw new IllegalActionException("BinaryOperatorActor: Different channels: "
						+inImg1.getChannelID()+ " != " +inImg2.getChannelID());
			}
	
			outImg = _operate(inImg1, inImg2);
		}
		
		if ( outImg == null ) {
			outImg = Images.nullImage;
		}
		else {
			// make the output image "belong" to the stream _streamID:
			outImg.setStreamID(_streamID);
		}

		output.broadcast(new ObjectToken(outImg));		
	}

	public void wrapup() throws IllegalActionException {
    	super.wrapup();
		gscon2 = null;
    }
}