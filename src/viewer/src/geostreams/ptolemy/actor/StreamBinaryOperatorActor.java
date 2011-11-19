package geostreams.ptolemy.actor;

import java.awt.Rectangle;

import geostreams.image.ChannelDef;
import geostreams.image.IImage;
import geostreams.image.IStream;
import geostreams.image.Images;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ObjectToken;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO : 0 istream
 * 
 * Base class for binary operators.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamBinaryOperatorActor.java,v 1.4 2007/06/13 00:08:43 crueda Exp $
 */
public abstract class StreamBinaryOperatorActor extends StreamOperatorActor {
	
	/** Input port 2.*/
	public TypedIOPort input2;
	
	protected StreamBinaryOperatorActor(CompositeEntity container, String name, String opName)
	throws NameDuplicationException, IllegalActionException {
		this(container, name, opName, opName);
	}

	protected StreamBinaryOperatorActor(CompositeEntity container, String name,
			String opName, String streamID)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, opName, streamID);
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
	
	/**
	 * Obtains the stream for this binary operator and calls _generatedStreamObtained().
	 * @param str
	 * @param str2
	 */
	private void _obtainGeneratedStream(IStream str, IStream str2) throws IllegalActionException {
		assert str != null;
		assert str2 != null;
		
		// channelDef'must be equal:
		ChannelDef channelDef = str.getChannelDef();
		ChannelDef channelDef2 = str2.getChannelDef();
		
		if ( channelDef.getPixelSizeX() != channelDef2.getPixelSizeX()
		||   channelDef.getPixelSizeY() != channelDef2.getPixelSizeY()
		||   Math.abs(channelDef.getAspect() - channelDef2.getAspect()) > 0.01  // TODO arbitrary epsilon
		) {
			
			throw new IllegalActionException("BinaryOperatorActor: Different channels: "
					+str.getChannelDef().getID()+ " != " +str2.getChannelDef().getID());
		}
		
//		FIXME how to get the new output _istream
		_istream = Images.createStream(_streamID, str);
		
		Rectangle fov = str.getFOV();
		if ( fov != null ) {
			Rectangle fov2 = str2.getFOV();
			if ( fov2 != null ) {
				Rectangle fovR = fov.intersection(fov2);
				_istream.setFov(fovR);
			}
		}
		
		_generatedStreamObtained();
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
			if ( _istream == null ) {
				_obtainGeneratedStream(inImg1.getStream(), inImg2.getStream());
			}

			outImg = _operate(inImg1, inImg2);
		}
		
		if ( outImg == null ) {
			outImg = Images.nullImage;
		}
		else {
			// make the output image "belong" to our generated stream
			outImg.setStream(_istream);
			_istream.getStreamStatus().incrementImageCount();
		}

		output.broadcast(new ObjectToken(outImg));		
	}
}