package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import geostreams.image.IStream;
import geostreams.image.Images;
import geostreams.util.Utils;
import ptolemy.data.ObjectToken;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO : 0 istream
 * 
 * Base class for unary operators.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamUnaryOperatorActor.java,v 1.4 2007/06/13 00:08:43 crueda Exp $
 */
public abstract class StreamUnaryOperatorActor extends StreamOperatorActor {

	private String _frameID;

	protected StreamUnaryOperatorActor(CompositeEntity container, String name, String opName)
	throws NameDuplicationException, IllegalActionException {
		this(container, name, opName, opName);
	}
	
	protected StreamUnaryOperatorActor(CompositeEntity container, String name,
			String opName, String streamID)
	throws NameDuplicationException, IllegalActionException {
		super(container, name, opName, streamID);
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
//				if ( container instanceof IStreamGenerator ) {
//					IStreamGenerator streamActor = (IStreamGenerator) container;
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
			if ( _istream == null ) {
				IStream inStr = inImg1.getStream();
				assert inStr != null || Utils.fail("inImg1 = " +inImg1);
				_istream = Images.createStream(_streamID, inStr);
				
				if ( _roi != null ) {
					_istream.setFov(_roi);
					
					// TODO remote this first attempt to associate a frame corresponding to the ROI
//					Map<String, FrameDef> frameDefs = _istream.getFrameDefs();
//					frameDefs.clear();
//					_frameID = "fr0";
//					FrameDef frameDef = new FrameDef(_frameID, _roi);
//					frameDefs.put(frameDef.getID(), frameDef);
//					Map<String, FrameStatus> frameStatusMap = _istream.getFrameStatusMap();
//					frameStatusMap.clear();
//					FrameStatus frameStatus = new FrameStatus(frameDef);
//					frameStatusMap.put(frameDef.getID(), frameStatus);
				}
				
				_generatedStreamObtained();
			}
			outImg = _operate(inImg1);
		}
		
		if ( outImg == null ) {
			outImg = Images.nullImage;
		}
		else {
			// make the output image "belong" to our generated stream
			outImg.setStream(_istream);
			outImg.setFrameID(_frameID);
			_istream.getStreamStatus().incrementImageCount();
		}

		output.broadcast(new ObjectToken(outImg));
	}
}