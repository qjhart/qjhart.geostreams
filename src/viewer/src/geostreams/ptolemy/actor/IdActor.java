package geostreams.ptolemy.actor;

import geostreams.image.IImage;
import ptolemy.data.ObjectToken;
import ptolemy.data.Token;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * Identity transform actor.
 * It simply broadcasts the input tokens without any transformation.
 * Defined mainly for testing general handling of stream generator
 * actors in the framework.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: IdActor.java,v 1.4 2007/05/27 06:33:33 crueda Exp $
 */
public class IdActor extends AbstractStreamActor {
	/** {@inheritDoc} */
	public IdActor(CompositeEntity container, String name)
			throws NameDuplicationException, IllegalActionException {
		super(container, name, "ID");
		_createInputPort(false);
		_createOutputPort(true);

		_attachText("_iconDescription", ActorUtilities.getIconText(55, "Id"));
	}
	
	public void fire() throws IllegalActionException {
		if ( input.getWidth() == 0 || !input.hasToken(0) ) {
			return;
		}
		final Token token = input.get(0);
		ObjectToken objToken = (ObjectToken) token;
		Object obj = objToken.getValue();
		
		if ( obj instanceof IImage ) {
			_updateConnection((IImage) obj);
		}
		
		output.broadcast(token);
	}   
}
