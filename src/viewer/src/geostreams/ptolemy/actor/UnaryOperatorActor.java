package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.Images;
import ptolemy.data.ObjectToken;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Base class for unary operators.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: UnaryOperatorActor.java,v 1.6 2007/06/04 17:49:30 crueda Exp $
 */
public abstract class UnaryOperatorActor extends AbstractStreamActor {
	/** {@inheritDoc} */
	protected UnaryOperatorActor(CompositeEntity container, String name, String opName)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, opName);
		_createInputPort(false);
		_createOutputPort(true);
	}
	
//	/**
//	 * TODO: first tests to obtain info about the actor that is supplying my
//	 * input stream ...  INCOMPLETE
//	 */
//	@SuppressWarnings("unchecked")
//	public void initialize() throws IllegalActionException {
//		super.initialize();
//		if ( input != null ) {
//			int numSources = input.numberOfSources();
//			if ( numSources >= 1 ) {
//				List<IOPort> sources = input.sourcePortList();
//				IOPort source = sources.get(0);
//				NamedObj container = source.getContainer();
//				if ( container instanceof AbstractStreamActor ) {
//					AbstractStreamActor streamActor = (AbstractStreamActor) container;
//					
//				}
//			}
//		}
//	}
	
	/**
	 * The actual operation occurs here.
	 * 
	 * @param inImg1 Operand; PRE: !inImg1.isNull().
	 * @return The resulting image; can be null, in which case, fire()
	 * takes care of generating the nullImage.
	 */
	protected abstract IImage _operate(IImage inImg1) throws IllegalActionException;

	/**
	 * Gets the input image and performs a unary operation on it.
	 * If the input image isNull, then the nullImage is output;
	 * otherwise it calls _operate(img1). If _operate returns null,
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
		
		IImage outImg = null;
		
		if ( !inImg1.isNull() ) {
			_updateConnection(inImg1);
			outImg = _operate(inImg1);
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
}