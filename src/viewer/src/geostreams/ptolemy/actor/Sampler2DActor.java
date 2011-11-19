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
 * @version $Id: Sampler2DActor.java,v 1.1 2007/09/16 09:47:03 crueda Exp $
 */
public class Sampler2DActor extends TypedAtomicActor {
	
	public TypedIOPort input;
    public TypedIOPort input2;
	public TypedIOPort output;
    public TypedIOPort output2;
	
	protected OperatorIcon _icon;
	
	
    public Sampler2DActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
        _icon = new OperatorIcon(this, "_icon", "Sampler");
        _icon.setBackgroundColor(new Color(0x189597)); //Color.CYAN);

		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(new ArrayType(BaseType.DOUBLE));

        input2 = new TypedIOPort(this, "input2", true, false);
        input2.setTypeEquals(new ArrayType(BaseType.DOUBLE));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(new ArrayType(BaseType.DOUBLE));
		output.setMultiport(true);

        output2 = new TypedIOPort(this, "output2", false, true);
        output2.setTypeEquals(new ArrayType(BaseType.DOUBLE));
        output2.setMultiport(true);
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
