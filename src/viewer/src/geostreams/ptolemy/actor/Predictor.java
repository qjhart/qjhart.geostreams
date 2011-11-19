package geostreams.ptolemy.actor;

import geostreams.ptolemy.icon.OperatorIcon;

import java.awt.Color;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.ArrayToken;
import ptolemy.data.type.ArrayType;
import ptolemy.data.type.BaseType;
import ptolemy.kernel.CompositeEntity;
import ptolemy.kernel.util.IllegalActionException;
import ptolemy.kernel.util.NameDuplicationException;

/**
 * 
 * 
 * TODO Currently, this is not implemented. Input token is output
 * without any modification.
 * 
 * @author Carlos Rueda-Velasquez
 * @version $Id: Predictor.java,v 1.1 2007/09/17 09:34:22 crueda Exp $
 */
public class Predictor extends TypedAtomicActor {
    
	public TypedIOPort input;
	public TypedIOPort output;
	
	protected OperatorIcon _icon;
    
	
    public Predictor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new OperatorIcon(this, "_icon", "Predictor");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
//		output.setTypeEquals(BaseType.DOUBLE);
		output.setMultiport(true);

	}

	public void fire() throws IllegalActionException {
		if ( input.getWidth() == 0 || !input.hasToken(0) ) {
			return;
		}
		ArrayToken token = (ArrayToken) input.get(0);
		// ... no conversion yet.
		output.broadcast(token);
	}
}
