package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * TODO : 0 istream
 * 
 * Base class for stream sink actors.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: StreamSinkActor.java,v 1.1 2007/06/03 21:15:37 crueda Exp $
 */
public abstract class StreamSinkActor extends TypedAtomicActor {
	public TypedIOPort input;

	/**
	 * @param multiPort input should be multiport?
	 */
	protected StreamSinkActor(CompositeEntity container, String name, boolean multiPort)
	throws NameDuplicationException, IllegalActionException {
		super(container, name);

		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(BaseType.OBJECT);
		input.setMultiport(multiPort);
	}
	
	
	/**
	 * Actual action occurs here.
	 * 
	 * @param inImg1 Operand; PRE: !inImg1.isNull().
	 */
	protected abstract void _sink(IImage inImg1) throws IllegalActionException;

	/**
	 * Gets the input images and _sink() on each even if inImg1.isNull().
	 */
	public void fire() throws IllegalActionException {
		if ( input.numberOfSources() > 0 && input.hasToken(0) ) {
			for ( int i = 0; i < input.numberOfSources(); i++ ) {
				Token token = input.get(i);
				ObjectToken objToken = (ObjectToken) token;
				
				IImage inImg1 = (IImage) objToken.getValue();
				_sink(inImg1);
			}
		}
	}
}
