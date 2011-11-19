/*
 * Basic tests on defining Kepler actors
 * Carlos Rueda-Velasquez 
 */
package edu.ucdavis.geostreams.kepler.tests;

import ptolemy.actor.TypedAtomicActor;
import ptolemy.kernel.CompositeEntity;
import ptolemy.data.UnsignedByteToken;
import ptolemy.data.ArrayToken;
import ptolemy.kernel.util.*;
import ptolemy.actor.IOPort;
import ptolemy.actor.TypedIOPort;
import ptolemy.data.Token;
import ptolemy.data.StringToken;
import ptolemy.data.type.BaseType;
import ptolemy.data.type.Type;
import ptolemy.data.type.ArrayType;

/**
 * This actor reads a byte array and does something with it :=)
 */
public class ByteArrayInputActor extends TypedAtomicActor {

	/** Construct an actor with the given container and name.
	  *  @param container The container.
	  *  @param name The name of this actor.
	  *  @exception IllegalActionException If the actor cannot be contained
	  *   by the proposed container.
	  *  @exception NameDuplicationException If the container already has an
	  *   actor with this name.
	  */
    public ByteArrayInputActor(CompositeEntity container, String name)
    throws NameDuplicationException, IllegalActionException  {
        super(container, name);
		
		input = new TypedIOPort(this, "input", true, false);
		input.setTypeEquals(new ArrayType(BaseType.UNSIGNED_BYTE));

		output = new TypedIOPort(this, "output", false, true);
		output.setTypeEquals(BaseType.STRING);

        _attachText("_iconDescription", "<svg>\n" +
                "<rect x=\"0\" y=\"0\" "
                + "width=\"60\" height=\"20\" "
                + "style=\"fill:white\"/>\n" +
                "</svg>\n");
    }

    ///////////////////////////////////////////////////////////////////
    ////                     ports and parameters                  ////

	/** The input port.
     */
    public TypedIOPort input = null;

	
	/** The output port.
     */
    public TypedIOPort output = null;

	
    ///////////////////////////////////////////////////////////////////
    ////                         public methods                    ////

    public void fire() throws IllegalActionException {
        super.fire();
        ArrayToken dataArrayToken = null;
        try {
          dataArrayToken = (ArrayToken) input.get(0);
        }
        catch (Exception ex) {
			String msg = "An exception occured: " +ex.getMessage();
			System.out.println(msg);
			output.send(0, new StringToken(msg));
			return;
		}
		Token[] array = dataArrayToken.arrayValue();
		for ( int i = 0; i < array.length; i++ ) {
			output.send(0, new StringToken("arry[" +i+ "] = " +array[i]));
		}
    }
}

